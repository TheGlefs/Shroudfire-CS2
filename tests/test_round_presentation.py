from pathlib import Path
import os
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]


class RoundPresentationTests(unittest.TestCase):
    def test_stock_round_audio_is_suppressed_across_rounds_without_changing_simulation(self):
        source = (ROOT / 'src/engine_adapter.cpp').read_text(encoding='utf-8')
        start = source.index('bool EngineAdapter::stabilize_center_html(')
        method = source[start:source.index('\nbool EngineAdapter::', start + 1)]
        header = (ROOT / 'src/engine_adapter.h').read_text(encoding='utf-8')
        start = header.index('    uint8_t round_end_count_native_')
        fields = header[start:header.index('    bool warmup_field_ok_', start)]
        program = r'''
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
struct Rules { float restart = 0; bool game_restart = false; uint8_t count = 7; uint8_t start_count = 3; bool no_music = false; };
struct Identity { const char *GetClassname() { return "cs_gamerules"; } };
struct CEntityInstance { Identity *m_pEntity; Rules *rules; };
struct CEntityIndex { int index; explicit CEntityIndex(int i) : index(i) {} };
constexpr int MAX_TOTAL_ENTITIES = 1;
struct CGameEntitySystem {
    CEntityInstance *proxy = nullptr;
    CEntityInstance *GetEntityInstance(CEntityIndex i) { return i.index == 0 ? proxy : nullptr; }
} system_instance;
class EngineAdapter {
public:
    bool stabilize_center_html(float, bool, bool, bool, bool);
    void *entity_system() { return &system_instance; }
    bool mark_state_changed(CEntityInstance *, uint32_t) { return true; }
    bool ready_ = true, html_restart_fields_ok_ = true, round_presentation_fields_ok_ = true;
    int html_game_rules_index_ = -1;
    struct Fields {
        uint32_t game_rules = offsetof(CEntityInstance, rules);
        uint32_t restart_round_time = offsetof(Rules, restart);
        uint32_t game_restart = offsetof(Rules, game_restart);
        uint32_t round_end_count = offsetof(Rules, count);
        uint32_t round_start_count = offsetof(Rules, start_count);
        uint32_t round_end_no_music = offsetof(Rules, no_music);
    } fields_;
''' + fields + '\n};\n' + method + r'''
int main() {
    Rules rules;
    Identity identity;
    CEntityInstance proxy{&identity, &rules};
    system_instance.proxy = &proxy;
    EngineAdapter adapter;
    // A client must see the same zero counters at every snapshot, even when
    // native round counters change and wrap. No delayed victory at round start.
    float now = 100;
    for (int round = 0; round < 260; ++round) {
        rules.restart = 0;
        rules.start_count = static_cast<uint8_t>(round + 3);
        const uint8_t native_start = rules.start_count;
        const uint8_t native_end = rules.count;
        for (int frame = 0; frame < 5; ++frame) {
            adapter.stabilize_center_html(now, true, true, true, true);
            assert(rules.count == 0 && rules.start_count == 0 && rules.no_music);
            adapter.stabilize_center_html(now, true, false, false, true);
            assert(rules.count == native_end && rules.start_count == native_start);
            assert(!rules.game_restart && !rules.no_music && rules.restart == 0);
            now += 1 / 64.0f;
        }
        rules.restart = now + 10;
        const float native_deadline = rules.restart;
        rules.count = static_cast<uint8_t>(native_end + 1);
        const uint8_t next_end = rules.count;
        float client_deadline = 0;
        for (int frame = 0; frame < 600; ++frame) {
            adapter.stabilize_center_html(now, false, true, true, true);
            assert(rules.count == 0 && rules.start_count == 0 && rules.no_music);
            if (frame == 0) client_deadline = rules.restart;
            assert(rules.restart == client_deadline);
            adapter.stabilize_center_html(now, false, false, false, true);
            assert(rules.count == next_end && rules.start_count == native_start);
            assert(rules.restart == native_deadline && !rules.game_restart && !rules.no_music);
            now += 1 / 64.0f;
        }
    }
    adapter.stabilize_center_html(now, true, true, true, true);
    // A real restart command between hooks must retain its new deadline/counts.
    rules.restart = now + 20;
    rules.count = 42;
    rules.start_count = 43;
    adapter.stabilize_center_html(now, true, false, false, true);
    assert(rules.restart == now + 20 && rules.count == 42 && rules.start_count == 43);
    // Disabling the mode restores native data, including when the restart pair
    // needed no substitution and only the audio presentation was active.
    rules = {now + 20, false, 8, 9, false};
    adapter.stabilize_center_html(now, true, true, false, true);
    assert(rules.count == 0 && rules.start_count == 0 && rules.no_music);
    adapter.stabilize_center_html(now, true, false, false, false);
    assert(rules.count == 8 && rules.start_count == 9 && !rules.no_music);
    assert(rules.restart == now + 20 && !rules.game_restart);
    // Native no-music values are also preserved when already enabled.
    rules = {0, false, 0, 0, true};
    adapter.stabilize_center_html(now, true, true, false, true);
    adapter.stabilize_center_html(now, true, false, false, false);
    assert(rules.no_music && rules.count == 0 && rules.start_count == 0);
    puts("Stock round audio replication regression checks passed.");
}
'''
        env = os.environ.copy()
        compiler = shutil.which('cl' if os.name == 'nt' else 'g++')
        if os.name == 'nt' and compiler is None:
            vswhere = Path(os.environ['ProgramFiles(x86)']) / 'Microsoft Visual Studio/Installer/vswhere.exe'
            installation = subprocess.check_output(
                [str(vswhere), '-latest', '-products', '*', '-property', 'installationPath'], text=True).strip()
            vcvars = Path(installation) / 'VC/Auxiliary/Build/vcvars64.bat'
            output = subprocess.check_output(f'cmd.exe /d /s /c "call "{vcvars}" >nul && set"', text=True)
            for line in output.splitlines():
                if '=' in line and not line.startswith('='):
                    name, value = line.split('=', 1)
                    env[name.upper()] = value
            compiler = shutil.which('cl', path=env['PATH'])
        self.assertIsNotNone(compiler, 'A C++17 compiler is required for the round presentation test')
        with tempfile.TemporaryDirectory(prefix='sf-round-', dir=ROOT) as directory:
            work = Path(directory)
            self.assertTrue(work.resolve().is_relative_to(ROOT.resolve()))
            path = work / 'test.cpp'
            path.write_text(program, encoding='utf-8')
            binary = work / ('test.exe' if os.name == 'nt' else 'test')
            args = ([compiler, '/nologo', '/std:c++17', '/EHsc', '/W4', str(path), f'/Fe:{binary}']
                    if os.name == 'nt' else [compiler, '-std=c++17', '-Wall', '-Wextra', '-Werror', str(path), '-o', str(binary)])
            compiled = subprocess.run(args, cwd=work, env=env, capture_output=True, text=True)
            self.assertEqual(compiled.returncode, 0, compiled.stdout + compiled.stderr)
            executed = subprocess.run([str(binary)], cwd=work, capture_output=True, text=True)
            self.assertEqual(executed.returncode, 0, executed.stdout + executed.stderr)


if __name__ == '__main__':
    unittest.main()
