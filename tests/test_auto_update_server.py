import hashlib
import importlib.util
from pathlib import Path
import tempfile
import unittest
from unittest.mock import Mock, patch


spec = importlib.util.spec_from_file_location(
    'auto_update_server', Path(__file__).resolve().parents[1] / 'tools/auto_update_server.py')
auto_update_server = importlib.util.module_from_spec(spec)
spec.loader.exec_module(auto_update_server)


class StartupVerificationTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.updater = auto_update_server.ServerUpdater.__new__(auto_update_server.ServerUpdater)
        self.updater.plugin = self.root / 'plugin'
        self.updater.game = self.root / 'game'
        self.updater.ambuild = self.root / 'ambuild'
        built = self.updater.plugin / 'build/package/cs2/addons/shroudfire/bin/linuxsteamrt64/shroudfire.so'
        built.parent.mkdir(parents=True)
        built.write_bytes(b'test plugin')
        (self.updater.game / 'csgo/addons/shroudfire/bin/linuxsteamrt64').mkdir(parents=True)
        self.log = self.root / 'server.log'
        for name in ('restore_loader', 'save_state', 'console', 'stop', 'log'):
            setattr(self.updater, name, Mock())
        self.updater.supported = Mock(return_value=True)
        self.updater.start = Mock(return_value=self.log)
        self.updater.capture = Mock(return_value='[Shroudfire] State: active')

    def test_initial_loading_and_changelevel_startups_complete(self):
        for reason in ('Loading', 'Changelevel'):
            with self.subTest(reason=reason):
                self.log.write_text(
                    '[Shroudfire] engine adapter ready for this server build.\n'
                    f'Host activate: {reason} (de_dust2)\n', encoding='utf-8')
                state = {'backup': str(self.root)}
                with patch.object(auto_update_server.subprocess, 'run'), \
                     patch.object(auto_update_server.time, 'sleep'):
                    self.updater.finish(state)
                self.assertEqual(state['phase'], 'complete')
                self.assertEqual(state['plugin_sha256'], hashlib.sha256(b'test plugin').hexdigest())
                self.updater.console.assert_called_with('sf_status')
                self.updater.stop.assert_not_called()

    def test_unready_servers_never_complete(self):
        ready = '[Shroudfire] engine adapter ready for this server build.\n'
        activated = 'Host activate: Loading (de_dust2)\n'
        cases = (
            (ready, '[Shroudfire] State: active'),
            (activated, '[Shroudfire] State: active'),
            (ready + activated, '[Shroudfire] State: disabled'),
            ('Unsupported server module\n' + activated, '[Shroudfire] State: active'),
        )
        for log, status in cases:
            with self.subTest(log=log, status=status):
                self.log.write_text(log, encoding='utf-8')
                self.updater.capture.return_value = status
                self.updater.stop.reset_mock()
                state = {'backup': str(self.root)}
                with patch.object(auto_update_server.subprocess, 'run'), \
                     patch.object(auto_update_server.time, 'sleep'), \
                     patch.object(auto_update_server.time, 'monotonic', side_effect=(0, 1, 151)):
                    with self.assertRaisesRegex(RuntimeError, 'did not pass startup verification'):
                        self.updater.finish(state)
                self.assertNotEqual(state.get('phase'), 'complete')
                self.updater.stop.assert_called_once()


if __name__ == '__main__':
    unittest.main()
