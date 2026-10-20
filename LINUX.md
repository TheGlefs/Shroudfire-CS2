# Shroudfire server on Linux

Run Shroudfire on a Linux x86-64 CS2 dedicated server using MetaMod:Source. This guide covers installation, configuration, startup, and the optional update monitor. The [README](README.md) explains gameplay and building with AI; [COMMANDS.md](COMMANDS.md) documents every command and server setting.

The native adapter supports the server-module fingerprints listed in [Compatibility](README.md#compatibility-and-known-limitations). Check that section when installing or updating CS2.

## Prepare the host

Use a dedicated, unprivileged account for SteamCMD and CS2. Install your distribution's SteamCMD and CS2 runtime dependencies, plus Bash, curl, and tar. Building on this host also requires Git, a C++17 compiler, and Python 3 with pip and virtual-environment support. The optional update monitor additionally requires tmux.

The examples assume the server and source checkout belong to the same account. Set these paths in the shell used for installation, adjusting them to your host:

```bash
STEAMCMD_DIR="$HOME/steamcmd"
CS2_DIR="$HOME/cs2-server"
SHROUDFIRE_DIR="$HOME/Shroudfire"
```

`SHROUDFIRE_DIR` must point to an existing checkout of this repository. Keep enough disk space for CS2, build dependencies, Workshop assets, and update backups.

## Install CS2

Download SteamCMD and install CS2:

```bash
mkdir -p "$STEAMCMD_DIR" "$CS2_DIR"
curl -fsSL https://media.steampowered.com/installer/steamcmd_linux.tar.gz \
  | tar -xz -C "$STEAMCMD_DIR"
"$STEAMCMD_DIR/steamcmd.sh" \
  +force_install_dir "$CS2_DIR" \
  +login anonymous +app_update 730 validate +quit
```

If the server cannot find `steamclient.so`, check the server account's Steam library path. A common arrangement is:

```bash
mkdir -p "$HOME/.steam/sdk64"
test -e "$HOME/.steam/sdk64/steamclient.so" || \
  ln -s "$STEAMCMD_DIR/linux64/steamclient.so" "$HOME/.steam/sdk64/steamclient.so"
```

Inspect an existing broken link before replacing it. For distribution-specific runtime requirements, consult the [SteamCMD documentation](https://developer.valvesoftware.com/wiki/SteamCMD) and [CS2 dedicated-server documentation](https://developer.valvesoftware.com/wiki/Counter-Strike_2/Dedicated_Servers).

## Build the plugin

Follow the [build instructions](README.md#build), or ask your AI coding assistant to build Shroudfire for the server's Linux x86-64 environment. Use a compiler and runtime libraries compatible with the deployment host.

The resulting package is under `$SHROUDFIRE_DIR/build/package/cs2`. It includes:

```text
addons/metamod/shroudfire.vdf
addons/shroudfire/bin/linuxsteamrt64/shroudfire.so
addons/shroudfire/admins.example.txt
addons/shroudfire/LICENSE
addons/shroudfire/NOTICE
addons/shroudfire/third-party/metamod-source-LICENSE.txt
```

The server configs and Workshop pack are installed separately. To transfer a build from another Linux machine, package it from the repository root:

```bash
mkdir -p dist
tar -czf dist/shroudfire-cs2-linux.tar.gz -C build/package/cs2 addons
```

Transfer that archive to the server using your normal file-transfer method. Extract it into `game/csgo` during the installation step below.

## Install MetaMod and Shroudfire

Stop CS2 before installing or replacing the plugin. Download the Linux Source 2 package from [MetaMod:Source](https://www.metamodsource.net/downloads.php?branch=master) and extract it into `$CS2_DIR/game/csgo`.

Following the [Source 2 loader instructions](https://wiki.alliedmods.net/Installing_SourceMM#Source_2), add this as the first `Game` entry inside `SearchPaths` in `$CS2_DIR/game/csgo/gameinfo.gi`:

```text
Game csgo/addons/metamod
```

Keep the file's other search paths. Game updates can replace this file, so check the entry after updating CS2.

Copy the locally built Shroudfire package:

```bash
cp -a "$SHROUDFIRE_DIR/build/package/cs2/." "$CS2_DIR/game/csgo/"
```

If you transferred an archive instead, extract it with:

```bash
tar -xzf /path/to/shroudfire-cs2-linux.tar.gz -C "$CS2_DIR/game/csgo"
```

## Configure the server

Merge [configs/server.cfg](configs/server.cfg) into `$CS2_DIR/game/csgo/cfg/server.cfg`. Choose your own hostname, tags, password policy, and public-server settings. Then install the Casual and Shroudfire configs:

```bash
cp "$SHROUDFIRE_DIR/configs/gamemode_casual_server.cfg" "$CS2_DIR/game/csgo/cfg/"
cp "$SHROUDFIRE_DIR/configs/shroudfire.cfg" "$CS2_DIR/game/csgo/cfg/"
```

Start the server in Casual mode (`game_type 0`, `game_mode 0`). CS2 executes `gamemode_casual_server.cfg`, which disables bots, automatic team balancing, and warmup, then runs `exec shroudfire.cfg`.

Edit `game/csgo/cfg/shroudfire.cfg` for round rules, map rotation, progression, upgrades, HUD, and chat settings. It enables Shroudfire with `sf_enable 1`; the compiled plugin default is `0`. [COMMANDS.md](COMMANDS.md#server-settings) lists the compiled defaults and allowed ranges.

The example also enables local movement silencing, which requires the Workshop pack on the server and clients. Follow [the Workshop setup](sound-pack/README.md), or set this in your installed `shroudfire.cfg` when running without that pack:

```text
sf_cvar sf_silent_local_movement 0
```

Custom voices remain off until you set `sf_custom_sounds 1` after configuring client downloads.

### Map rotation

The default limits are 20 completed rounds or 30 minutes with at least two humans playing, whichever is reached first. Shroudfire finishes the current multiplayer round and announces the next map before switching.

Set the limits and rotation in the installed `game/csgo/cfg/shroudfire.cfg`:

```text
sf_cvar sf_map_max_rounds 20
sf_cvar sf_map_time_limit 30
sf_cvar sf_map_rotation "de_dust2,de_mirage,de_inferno,de_nuke"
```

Set either limit to `0` to disable it, or both to `0` for unlimited play on a map. Time pauses when fewer than two humans are playing or the mode is disabled. Unavailable rotation entries are skipped; if none are available, the plugin keeps the current map and reports the problem.

In the server console, `sf_nextmap de_mirage` overrides the next destination, `sf_nextmap` shows it, and `sf_nextmap reset` returns to the rotation. Connected administrators with `generic` access can use the same commands. `sf_status` shows map progress and limits. See [the command reference](COMMANDS.md#map-rotation) for counting rules and rotation behavior.

### Steam authentication

For a public server, create a Game Server Login Token for App ID 730 at [Steam Game Server Account Management](https://steamcommunity.com/dev/managegameservers). Put it in the server's private `cfg/server.cfg`:

```text
sv_setsteamaccount "YOUR_TOKEN"
```

Limit access to that file:

```bash
chmod 600 "$CS2_DIR/game/csgo/cfg/server.cfg"
```

The included launcher reads the token from this file and supplies it before Steam registration. Keep the real token out of the repository. For a LAN server, use the launcher's `CS2_LAN=1` option.

### Administrator access and player data

On a first installation, copy the administrator example:

```bash
cp "$CS2_DIR/game/csgo/addons/shroudfire/admins.example.txt" \
  "$CS2_DIR/game/csgo/addons/shroudfire/admins.txt"
```

Edit `admins.txt` with one Steam64 ID and `generic` or `root` access level per line. See [administrator access](COMMANDS.md#administrator-access) for permissions. Restart CS2 after changing this file.

Player profiles live in `game/csgo/addons/shroudfire/data`. Preserve that directory, your private administrator list, and your customized configs when updating or migrating.

## Start the server

Copy the launcher into the server's `game` directory and run it:

```bash
cp "$SHROUDFIRE_DIR/configs/start-shroudfire.sh" "$CS2_DIR/game/"
chmod 755 "$CS2_DIR/game/start-shroudfire.sh"
"$CS2_DIR/game/start-shroudfire.sh"
```

The launcher starts a dedicated server in Casual mode with VAC enabled, Shroudfire enabled, and no bots. It uses these environment variables:

| Variable | Default | Purpose |
| --- | --- | --- |
| `CS2_GAME_DIR` | The launcher's directory | Directory containing `cs2.sh`. |
| `CS2_PORT` | `27015` | Server UDP port. |
| `CS2_MAP` | `de_dust2` | Initial map. |
| `CS2_LAN` | `0` | Set to `1` for LAN mode. |
| `SHROUDFIRE_ENABLE` | `1` | Launch-time `sf_enable` value. Server configs can override it afterward. |

For example:

```bash
CS2_PORT=27016 CS2_MAP=de_mirage "$CS2_DIR/game/start-shroudfire.sh"
```

To run directly from the repository, point it at the game's directory:

```bash
CS2_GAME_DIR="$CS2_DIR/game" "$SHROUDFIRE_DIR/configs/start-shroudfire.sh"
```

Extra arguments are forwarded to `cs2.sh`. For an interactive server that survives SSH disconnects, start `tmux new -s shroudfire`, then run the launcher inside that session. Detach with Ctrl+B, then D; return with `tmux attach -t shroudfire`. Enter `quit` in the CS2 console to stop cleanly.

Allow the selected UDP port through the host firewall, hosting-provider firewall, and NAT forwarding where applicable.

## Check the installation

Run these in the server console:

```text
meta version
meta list
sf_status
status
```

MetaMod should list Shroudfire, and startup should report `engine adapter ready for this server build`. `sf_status` shows the mode's state, map rotation, teams, queue, AFK players, and spawn capacity.

Connect a CS2 client using `connect YOUR_SERVER:27015`, substituting the actual address and port. Type `help`, `menu`, and `upgrade` in chat to open the player panels. The same WSAD, SPACE, and R menu controls work while alive and dead.

## Automatic updates

The optional [tools/auto_update_server.py](tools/auto_update_server.py) supervises CS2 and checks Steam for updates every five minutes. It requires Python 3, tmux, SteamCMD, the installed launcher, and a configured AMBuild tree at `<plugin_dir>/build`. Run it as the same account as the server.

Copy its config to a private location:

```bash
cp "$SHROUDFIRE_DIR/configs/auto-update.example.json" "$HOME/shroudfire-auto-update.json"
chmod 600 "$HOME/shroudfire-auto-update.json"
```

Edit the JSON to match your installation:

| Field | Value |
| --- | --- |
| `server_dir` | Absolute CS2 installation path containing `game`. |
| `plugin_dir` | Absolute Shroudfire checkout path containing `src` and `build`. |
| `steamcmd` | Absolute path to `steamcmd.sh`. |
| `ambuild` | Absolute path to the build environment's `ambuild` executable. |
| `session` | tmux session used by CS2; default `shroudfire`. |
| `check_interval` | Steam check interval in seconds; default `300`, minimum `60`. |

Stop any other automatic restart supervisor before starting this monitor. Its `.shroudfire-supervisor.lock` prevents two copies of this monitor from updating the same server concurrently.

Run it from your service manager or a persistent shell:

```bash
"$SHROUDFIRE_DIR/.venv/bin/python" "$SHROUDFIRE_DIR/tools/auto_update_server.py" \
  --config "$HOME/shroudfire-auto-update.json"
```

Add `--check` to query Steam and print the installed/required versions without applying an update. Add `--once` to process a pending update and exit instead of supervising continuously.

When an update is required, the monitor:

1. Announces the restart to all human players at 30, 20, and 10 seconds, requiring console acknowledgement of each notice.
2. Stops CS2 cleanly and backs up addons, configs, profiles, the previous server module, and plugin source.
3. Updates CS2 with SteamCMD and restores the MetaMod search-path entry.
4. Checks native adapter compatibility, rebuilds and installs the plugin, then starts CS2 and verifies adapter readiness and mode activation.
5. Resumes monitoring and crash recovery after a successful update.

An unsupported server module leaves the update in `awaiting_compatibility`. Ask AI to review the new binary and update the adapter as described in the [README](README.md#build-or-update-with-ai). Once the source supports the installed binary, the monitor's next check rebuilds and completes the update. Keep the fingerprint guard in place.

Logs, backups, and `state.json` are stored under `$CS2_DIR/auto-update`. Inspect the recorded error and logs when an update stops. Backups accumulate, so include that directory in disk maintenance.

## Troubleshooting and updates

| Symptom | Check |
| --- | --- |
| `meta` is unknown | Linux Source 2 MetaMod package, extraction location, and the first `Game` search path in `gameinfo.gi`. |
| Shroudfire is missing from `meta list` | VDF and `.so` paths, x86-64 architecture, runtime libraries, and startup errors. |
| `Unsupported server module` or a disabled engine adapter | The loaded module's fingerprint and the supported values in `src/engine_adapter.cpp`. Update the native adapter for a different binary. |
| The mode stays disabled | Adapter readiness, game interfaces, `sf_enable`, and whether the Casual config executed `shroudfire.cfg`. |
| Steam registration or remote joining fails | Login token, private config, Steam libraries, UDP port, firewalls, and public network routing. |
| Custom voices are missing | MultiAddonManager, Workshop IDs, server/client mounts, client downloads, `sf_custom_sounds`, and the player's `audio` preferences. |
| Movement sounds do not match expectations | Workshop pack version, default role teams, and the [local movement overrides](sound-pack/README.md#local-movement-silencing). |
| Live unloading is refused | Stop CS2, replace or remove plugin files, and restart. The engine retains plugin callbacks until shutdown. |
| The update monitor waits for compatibility | Review the new server binary and adapter, then inspect `auto-update/state.json` and the associated logs. |

For a manual update, back up configs, administrator access, and player profiles; stop CS2; install the new package; recheck the MetaMod search path; then restart and inspect `sf_status`. A full server restart is required when replacing the plugin.
