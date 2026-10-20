# Shroudfire development tools

Run these commands from the repository root. Python tools use the standard library; build dependencies are described in [README.md](../README.md#build). Diagnostic game binaries, minidumps, decompiled assets, and profile databases are supplied separately and are not part of the repository.

| Tool | Usage | Purpose |
| --- | --- | --- |
| HUD guide generation | `python tools/generate_help_pages.py` | Regenerate the guide arrays in `src/plugin.cpp` from `guides.json`. A page has a title and at most five body lines. Review the generated source before rebuilding. |
| Voice event generation | `python tools/generate_sound_pack.py` | Validate the voice source files and regenerate `sound-pack/soundevents/soundevents_addon.vsndevts`. Compile and publish the addon separately. |
| Workshop source preparation | `./tools/prepare_workshop_sounds.ps1 -GamePath '<CS2-installation>' [-AddonName '<addon-name>']` | Copy voice sources, sound events, the addon resource manifest, sound stacks and their manifest, the instruction map, and its materials into an existing Windows Workshop Tools addon. See [sound-pack/README.md](../sound-pack/README.md#build-or-update-the-addon). |
| Agent voice hashes | `python tools/generate_kill_voice_hashes.py <decompiled-agent-soundevents-directory>` | Read `game_sounds_*.vsndevts` and regenerate `src/kill_voice_hashes.h` for stock kill and round-result voice events. |
| Linux signature scan | `python tools/scan_linux_signatures.py <libserver.so> <gamedata.json>` | Count Linux server byte-pattern matches from a supplied game-data JSON file. A unique match does not validate ABI compatibility. |
| Linux HUD signature scan | `python tools/scan_custom_hud_linux.py <libserver.so>` | Count matches for the script's custom HUD patterns. It does not update or enable the engine adapter. |
| Linux update monitor | `python tools/auto_update_server.py --config <auto-update.json> [--check|--once]` | Monitor CS2, announce update restarts at 30/20/10 seconds, back up state, update Steam files, and rebuild supported adapters. Unknown native builds wait for compatibility review. See [LINUX.md](../LINUX.md#automatic-updates). |
| Windows minidump inspection | `python tools/inspect_minidump.py <dump.dmp>` | Print exception details and nearby module return addresses from a minidump. |

The Linux launcher is under [configs/start-shroudfire.sh](../configs/start-shroudfire.sh). Its directory and environment-variable options are documented in [LINUX.md](../LINUX.md#start-the-server).
