"""Supervise a Linux CS2 server and apply Steam updates with chat warnings.

Unknown native engine builds wait for a reviewed adapter instead of bypassing
Shroudfire's fingerprint guard. A recurring compatibility review can resolve
that state; this process then rebuilds, verifies and restarts the server.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tarfile
import time
from urllib.request import urlopen
import zlib


class ServerUpdater:
    def __init__(self, config):
        self.server = Path(config['server_dir']).resolve()
        self.game = self.server / 'game'
        self.plugin = Path(config['plugin_dir']).resolve()
        self.steamcmd = Path(config['steamcmd']).resolve()
        self.ambuild = Path(config['ambuild']).resolve()
        self.session = config.get('session', 'shroudfire')
        self.launcher = self.game / 'start-shroudfire.sh'
        self.interval = max(60, int(config.get('check_interval', 300)))
        self.work = self.server / 'auto-update'
        self.work.mkdir(mode=0o700, exist_ok=True)
        self.state_file = self.work / 'state.json'
        self.source = self.plugin / 'src/engine_adapter.cpp'
        self.module = self.game / 'csgo/bin/linuxsteamrt64/libserver.so'
        self.last_start = 0.0

    def log(self, message):
        print(f'[{time.strftime("%Y-%m-%dT%H:%M:%S%z")}] {message}', flush=True)

    def save_state(self, state):
        pending = self.state_file.with_suffix('.tmp')
        pending.write_text(json.dumps(state, indent=2) + '\n', encoding='utf-8')
        pending.replace(self.state_file)

    def process(self):
        binary = (self.game / 'bin/linuxsteamrt64/cs2').resolve()
        for directory in Path('/proc').iterdir():
            if directory.name.isdigit():
                try:
                    if (directory / 'exe').resolve() == binary:
                        return int(directory.name)
                except (OSError, RuntimeError):
                    pass
        return None

    def console(self, command):
        subprocess.run(['tmux', 'send-keys', '-t', self.session + ':0.0', '-l', command], check=True)
        subprocess.run(['tmux', 'send-keys', '-t', self.session + ':0.0', 'Enter'], check=True)

    def capture(self):
        return subprocess.check_output(
            ['tmux', 'capture-pane', '-p', '-J', '-t', self.session + ':0.0', '-S', '-250'], text=True)

    def check(self):
        content = (self.game / 'csgo/steam.inf').read_text(encoding='utf-8')
        match = re.search(r'^PatchVersion=(\d+(?:\.\d+)+)\s*$', content, re.M)
        if not match:
            raise RuntimeError('Cannot read installed CS2 PatchVersion')
        version = int(match[1].replace('.', ''))
        url = f'https://api.steampowered.com/ISteamApps/UpToDateCheck/v1/?appid=730&version={version}'
        with urlopen(url, timeout=20) as response:
            result = json.load(response)['response']
        if result.get('success') is not True or not isinstance(result.get('up_to_date'), bool):
            raise RuntimeError('Steam returned an invalid update check')
        return not result['up_to_date'], version, result.get('required_version', version)

    def supported(self):
        source = self.source.read_bytes()
        linux = source.split(b'kServerSize =', 2)
        if len(linux) != 3:
            raise RuntimeError('Cannot identify Linux adapter fingerprint')
        section = linux[2].split(b'#endif', 1)[0]
        size = re.match(rb'\s*(\d+);', section)
        crc = re.search(rb'kServerCrc32 = 0x([0-9a-fA-F]+);', section)
        if not size or not crc:
            raise RuntimeError('Cannot parse Linux adapter fingerprint')
        data = self.module.read_bytes()
        return len(data) == int(size[1]) and zlib.crc32(data) == int(crc[1], 16)

    def countdown(self):
        start = time.monotonic()
        for seconds in (30, 20, 10):
            time.sleep(max(0, start + 30 - seconds - time.monotonic()))
            self.console(f'sf_update_notice {seconds}')
            deadline = time.monotonic() + 3
            expected = f'[Shroudfire] Restarting server to apply new CS2 update in {seconds} seconds.'
            while expected not in self.capture():
                if time.monotonic() >= deadline:
                    raise RuntimeError('Restart announcement was not acknowledged; update cancelled')
                time.sleep(0.1)
        time.sleep(max(0, start + 30 - time.monotonic()))

    def stop(self):
        pid = self.process()
        if not pid:
            return
        self.console('quit')
        deadline = time.monotonic() + 45
        while self.process():
            if time.monotonic() >= deadline:
                raise RuntimeError('CS2 did not stop cleanly; game files were not changed')
            time.sleep(0.25)

    def start(self):
        if self.process():
            return None
        self.last_start = time.monotonic()
        if subprocess.run(['tmux', 'has-session', '-t', self.session],
                          stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0:
            subprocess.run(['tmux', 'kill-session', '-t', self.session], check=True)
        log = self.work / f'server-{time.time_ns()}.log'
        log.touch(mode=0o600)
        # Attach logging before the launcher emits startup diagnostics.
        command = 'sleep 1; exec ' + shlex.quote(str(self.launcher))
        subprocess.run(['tmux', 'new-session', '-d', '-s', self.session,
                        '-c', str(self.game), command], check=True)
        subprocess.run(['tmux', 'pipe-pane', '-t', self.session + ':0.0',
                        'cat >> ' + shlex.quote(str(log))], check=True)
        return log

    def restore_loader(self):
        path = self.game / 'csgo/gameinfo.gi'
        data = path.read_bytes()
        if b'csgo/addons/metamod' in data:
            return
        newline = b'\r\n' if b'\r\n' in data else b'\n'
        data, count = re.subn(
            rb'(?m)(^[\t ]*SearchPaths[\t ]*\r?\n[\t ]*\{[\t ]*\r?\n)',
            lambda m: m[1] + b'\t\t\tGame\tcsgo/addons/metamod' + newline, data, count=1)
        if count != 1:
            raise RuntimeError('Cannot restore the MetaMod SearchPaths entry')
        path.write_bytes(data)

    def backup(self, directory):
        directory.mkdir(mode=0o700, exist_ok=True)
        shutil.copy2(self.module, directory / 'libserver.so')
        with tarfile.open(directory / 'server-and-mod.tar.gz', 'w:gz', compresslevel=1) as archive:
            for relative in ['game/csgo/addons', 'game/csgo/cfg', 'game/csgo/gameinfo.gi',
                             'game/csgo/steam.inf', 'game/start-shroudfire.sh',
                             'steamapps/appmanifest_730.acf']:
                path = self.server / relative
                if path.exists():
                    archive.add(path, arcname=relative)
            archive.add(self.plugin / 'src', arcname='plugin/src')

    def finish(self, state):
        self.restore_loader()
        if not self.supported():
            state.update(phase='awaiting_compatibility',
                         error='Updated CS2 binary needs native adapter review; fingerprint guard retained')
            self.save_state(state)
            self.log(state['error'])
            return
        build_log = Path(state['backup']) / 'build.log'
        with build_log.open('ab') as output:
            subprocess.run([str(self.ambuild)], cwd=self.plugin / 'build',
                           stdout=output, stderr=subprocess.STDOUT, check=True, timeout=600)
        built = self.plugin / 'build/package/cs2/addons/shroudfire/bin/linuxsteamrt64/shroudfire.so'
        installed = self.game / 'csgo/addons/shroudfire/bin/linuxsteamrt64/shroudfire.so'
        staged = installed.with_suffix('.so.new')
        shutil.copy2(built, staged)
        staged.replace(installed)
        log = self.start()
        if log is None:
            raise RuntimeError('Unexpected running server during update verification')
        deadline = time.monotonic() + 150
        while time.monotonic() < deadline:
            text = log.read_text(encoding='utf-8', errors='replace')
            adapter_ready = 'engine adapter ready for this server build' in text
            map_active = re.search(r'^Host activate: (?:Loading|Changelevel)\b', text, re.M)
            if adapter_ready and map_active:
                self.console('sf_status')
                time.sleep(0.5)
                if '[Shroudfire] State: active' in self.capture():
                    state.update(phase='complete', completed_at=time.time(), log=str(log),
                                 plugin_sha256=hashlib.sha256(installed.read_bytes()).hexdigest())
                    state.pop('error', None)
                    self.save_state(state)
                    self.log('CS2 update complete; Shroudfire adapter and game mode are ready')
                    return
            if 'engine adapter disabled' in text or 'Unsupported server module' in text:
                break
            time.sleep(1)
        self.stop()
        raise RuntimeError('Updated server did not pass startup verification; inspect ' + str(log))

    def resume(self, state):
        if self.process():
            # Recover if the monitor itself exited after launching a healthy
            # server but before saving its final completion record.
            if state.get('steam_complete') and self.supported():
                self.console('sf_status')
                time.sleep(0.5)
                if '[Shroudfire] State: active' in self.capture():
                    state.update(phase='complete', completed_at=time.time())
                    state.pop('error', None)
                    self.save_state(state)
                    return
            raise RuntimeError('Update recovery found an unverified running server')
        if state.get('phase') == 'backing_up':
            self.backup(Path(state['backup']))
            state['phase'] = 'updating'
            self.save_state(state)
        if not state.get('steam_complete'):
            with (Path(state['backup']) / 'steamcmd.log').open('ab') as output:
                subprocess.run([str(self.steamcmd), '+force_install_dir', str(self.server),
                                '+login', 'anonymous', '+app_update', '730', '+quit'],
                               stdout=output, stderr=subprocess.STDOUT, check=True, timeout=3600)
            required, _, _ = self.check()
            if required:
                raise RuntimeError('SteamCMD finished but Steam still requires an update')
            state.update(steam_complete=True, phase='verifying')
            self.save_state(state)
        self.finish(state)

    def once(self):
        state = json.loads(self.state_file.read_text()) if self.state_file.exists() else {}
        if state.get('phase') not in (None, 'complete', 'cancelled'):
            if state['phase'] == 'awaiting_compatibility' and not self.supported():
                return
            self.resume(state)
            return
        required, installed, target = self.check()
        if not required:
            return
        if shutil.disk_usage(self.server).free < 4 * 1024 ** 3:
            raise RuntimeError('At least 4 GiB free disk space is required before updating')
        for path in (self.steamcmd, self.ambuild, self.launcher, self.source):
            if not path.is_file():
                raise RuntimeError('Required update file is missing: ' + str(path))
        self.log(f'Steam requires CS2 version {target}; installed version is {installed}')
        # Failed warnings leave the running game untouched and can retry later.
        if self.process():
            self.countdown()
            self.stop()
        backup = self.work / f'update-{time.time_ns()}'
        state = dict(phase='backing_up', backup=str(backup), installed=installed,
                     target=target, started_at=time.time(), steam_complete=False)
        self.save_state(state)
        self.backup(backup)
        state['phase'] = 'updating'
        self.save_state(state)
        self.resume(state)

    def run(self):
        next_check = 0.0
        while True:
            now = time.monotonic()
            if now >= next_check:
                try:
                    self.once()
                except Exception as error:
                    self.log(f'Update check or application failed: {error}')
                    if self.state_file.exists():
                        state = json.loads(self.state_file.read_text())
                        if state.get('phase') not in ('complete', 'cancelled'):
                            state['error'] = str(error)
                            self.save_state(state)
                next_check = time.monotonic() + self.interval
            state = json.loads(self.state_file.read_text()) if self.state_file.exists() else {}
            if state.get('phase') in (None, 'complete', 'cancelled') and not self.process():
                if time.monotonic() - self.last_start >= 120:
                    try:
                        self.log('CS2 is absent; restarting it')
                        self.start()
                    except Exception as error:
                        self.log(f'Server recovery failed: {error}')
            time.sleep(5)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--config', type=Path, required=True)
    parser.add_argument('--check', action='store_true', help='Only query Steam; do not modify the server')
    parser.add_argument('--once', action='store_true', help='Apply a pending update, then exit')
    args = parser.parse_args()
    config = json.loads(args.config.read_text(encoding='utf-8'))
    updater = ServerUpdater(config)
    if args.check:
        required, installed, target = updater.check()
        print(json.dumps(dict(update_required=required, installed=installed, target=target)))
        return
    import fcntl
    with (updater.server / '.shroudfire-supervisor.lock').open('a') as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            raise SystemExit('Another Shroudfire supervisor is already running')
        os.umask(0o077)
        if args.once:
            updater.once()
        else:
            updater.log(f'Server monitor started; Steam update check every {updater.interval} seconds')
            updater.run()


if __name__ == '__main__':
    main()
