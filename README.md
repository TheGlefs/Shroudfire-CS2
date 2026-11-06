# Shroudfire for Counter-Strike 2

Shroudfire is an asymmetric combat mode for **Counter-Strike 2**. **Invisant** (*invisible assailants*) hunt with invisibility and knives, while **Armitive** (*armed fugitives*) fight back with firearms or survive until time runs out. Roles rotate each round, and progression persists between sessions.

This edition runs as a native C++ MetaMod:Source plugin. For the SourceMod edition, see [Shroudfire for Counter-Strike: Source](https://github.com/TheGlefs/Shroudfire-CSS).

**Shroudfire was built using AI, but it is not slop. Great care was taken to get every detail right.** To build it, change it, or update it, open this repository in an AI coding assistant and describe what you want. See [Build or update with AI](#build-or-update-with-ai) for examples, or follow the manual build instructions below.

[How to play](#how-to-play) · [ESP and wallhacks](#esp-and-wallhacks) · [Upgrades](#upgrades) · [Chat shortcuts](#chat-shortcuts) · [Play](#play) · [Host a server](#install-and-configure) · [Build](#build) · [Discord](https://discord.gg/TMV2DEPhqT)

## How to play

**All values below are defaults and can be adjusted with server cvars.**

Rounds last **2 minutes**, with a roster that scales toward **4 Armitive per Invisant**.

### Invisant

You're invisible and armed only with a knife. Armitive cannot hear your footsteps. **Kill all Armitive to win.**

- Track living Armitive through walls and take no fall damage.
- Your knife hits in a **60° cone**, reaches **72 units**, and deals **50–75 raw damage**.
- Knife attacks run at **66% of normal speed**, with **no backstab bonus**.

**Starvation:** Once 60 seconds of live play have elapsed, 45 seconds without dealing or taking damage triggers starvation. You lose **1 HP/s**, rising to **3 HP/s** after 20 seconds of continuous starvation. If it starts at full health with 1:00 left, you'll die with roughly 0:15 left unless combat resets it.

### Armitive

Find the Invisant using firearms, blood, and contact. **Armitive win if anyone survives until time runs out.**

- The **M249** is blocked from purchase.
- Headshot damage against Invisant is **halved and capped at 90**. One headshot cannot kill a full-health Invisant.
- Move at least **256 units** from your tracked position to avoid camping. You'll receive a warning after **20 seconds**. After another **10 seconds**, you lose **10 HP/s** until you move away.

### Role rotation

Roles rotate every round. Killing an Invisant as Armitive gives you priority to become Invisant next round.

Otherwise, the longest-waiting eligible player becomes Invisant. **Suicide forfeits your eligibility for the next round.**

### Progression

Armitive and Invisant each have separate **XP, levels up to 40, upgrade points, and persistent combat rankings**. Your progression carries over between sessions.

- Earn XP through kills, qualifying damage assists, and completed-round wins.
- As Armitive, knife kills award **150% kill XP** and pistol kills award **125%**.
- Level up to earn upgrade points.
- Type `upgrade` for upgrade effects, `menu` to spend points, or `reset` to refund them.

### Kill streaks

In rounds with at least **2 players per side**, the first enemy kill earns **First Blood** and **125% kill XP**.

Your second kill within **10 seconds** of your last earns **120% kill XP**. Each additional kill within 10 seconds adds another **10 percentage points**, reaching **200% on your tenth kill**.

Only you hear your kill streak sounds.

### Rank

Type `rank` to view your statistics and rankings for both roles.

Kills after the round ends award **no XP, rank, or role priority**. Surviving Invisant are revealed during the transition after a timeout.

## ESP and wallhacks

Invisant aren't simply made transparent. During the hunt, the server doesn't send their player data, weapon sounds, or movement sounds to Armitive computers.

ESP and wallhacks can only display information a computer receives, so they **cannot reveal or track an Invisant** without that player data.

Missed knife swings and wall impacts remain audible clues. Armitive can also see blood and bullet impacts, and hear damage sounds. Surviving Invisant are revealed after timeout.

## Upgrades

Each role has **8 upgrades**, with **5 stages per upgrade**. Unlocks depend on your level in that role.

- **Normal stages:** levels **1, 5, 10, 15, 20**.
- **Ultimate stages:** levels **8, 16, 24, 32, 40**.
- Type `menu` to spend points or `reset` to refund them.
- Changes apply immediately during the opening **20 seconds** of a round. After that, they take effect next round.

### Armitive

- **Fortitude:** Gain 10 maximum health per stage.
- **Recovery:** Gain 1 health per stage every 3 seconds.
- **Overload:** Gain 10% extra magazine capacity per stage.
- **Stockpile:** Gain 1 extra reserve ammo clip per stage.
- **Barrage:** Gain 7% faster weapon fire rate per stage.
- **Control:** Gain 10% less recoil and bullet spread per stage.
- **Dexterity:** Gain 12% less weapon switch and reload time per stage.
- **Ultimate: Ward:** Press E for 2 seconds of immunity at 1/5, plus 0.5 seconds per later stage.

### Invisant

- **Lightweight:** Gain 10% less gravity per stage.
- **Momentum:** Gain 3% faster run speed per stage.
- **Resilience:** Gain 15 armor per stage and helmet at 5/5.
- **Reforging:** Gain 1 armor per stage every 3 seconds.
- **Vampirism:** Gain 3 health per stage when dealing damage.
- **Bloodlust:** Gain 6 health per stage when killing an enemy.
- **Mobility:** Gain 15% faster crouching speed per stage.
- **Ultimate: Surge:** Press E for a 480 unit speed boost at 1/5, increased by 30 per later stage.

## Chat shortcuts

Type these directly in chat **without a prefix**:

- `help` — Open the gameplay guide.
- `menu` — Open your current role's upgrade menu.
- `upgrade` — Open the upgrade guide.
- `reset` — Refund your current role's upgrades and open the upgrade menu.
- `rank` — Open your rank menu, including Armitive stats, Invisant stats, and menu navigation.
- `audio` — Open your audio settings.
- `bugs` — Open the known bugs guide.
- `cheat` — Open the visibility and anti-cheat guide.
- `close` — Close an open upgrade menu or guide.

### Menu controls

These controls work while **alive or dead**. Release each key before pressing it again.

- **A / D:** Change pages.
- **W / S:** Choose an option.
- **SPACE:** Select. On an informational guide page, jump to its options.
- **R:** Jump to the final page with **Exit Menu** selected, then press **SPACE** to exit.

While dead, an open panel takes priority over spectator controls. You can also choose upgrades by typing `1`–`8` in chat.

The `audio` menu lets you toggle **Round Start, Round End, Kill Streak, Starvation, Timeout, Camping, AFK, and Menu** sounds. Your choices persist across roles and sessions.

See [COMMANDS.md](COMMANDS.md) for the full command reference, administrator access, menu behavior, rankings, and server settings.

## Play

Connect from the CS2 developer console:

```text
connect cs2.shroudfire.com
```

You can also add `cs2.shroudfire.com` to Favorites in the Community Server Browser. The Workshop map `shroudfire_launch` displays joining instructions; playing the mode requires joining a Shroudfire server.

The first player starts as Armitive. Joining an empty server starts a fresh round promptly.

## Install and configure

You need a **Windows or Linux x86-64 CS2 dedicated server**, a Source 2 build of MetaMod:Source, and a Shroudfire package built for that operating system. Shroudfire supports both platforms; its native adapter targets specific server binaries. Read [Compatibility and known limitations](#compatibility-and-known-limitations) before installing.

For a complete Linux walkthrough, including SteamCMD, launch scripts, and automatic updates, follow [LINUX.md](LINUX.md).

1. **Build the plugin** using [AI](#build-or-update-with-ai) or the [manual instructions](#build). The installable package is written to `build/package/cs2`.
2. **Install MetaMod:Source** for your server's operating system. Follow its [Source 2 installation instructions](https://wiki.alliedmods.net/Installing_SourceMM#Source_2), adding `Game csgo/addons/metamod` as the first `Game` entry inside `SearchPaths` in `game/csgo/gameinfo.gi`.
3. **Stop CS2 and copy the package.** Copy the contents of `build/package/cs2` into the server's `game/csgo` directory, preserving the package's directory structure.
4. **Install the configs.** Merge `configs/server.cfg` into your server's `game/csgo/cfg/server.cfg`. Copy `configs/gamemode_casual_server.cfg` and `configs/shroudfire.cfg` into `game/csgo/cfg`. Choose your own hostname, map, port, and access settings.
5. **Set administrator access.** Copy the installed `addons/shroudfire/admins.example.txt` to `addons/shroudfire/admins.txt` and add your Steam64 IDs with `generic` or `root` access. See [administrator access](COMMANDS.md#administrator-access). The plugin reads this file when it loads.
6. **Start in Casual mode** with `game_type 0` and `game_mode 0`. The supplied Casual config disables bots, team balancing, and warmup, then executes `shroudfire.cfg` to enable the mode.

The plugin defaults to `sf_enable 0`; the supplied gameplay config sets it to `1`. Custom voices remain off until you configure their Workshop assets. The gameplay config also enables experimental local movement silencing, which needs the [Workshop pack](#workshop-pack-and-audio).

After startup, run these in the server console:

```text
meta version
meta list
sf_status
status
```

MetaMod should list Shroudfire, and the startup log must report **engine adapter ready for this server build**. `sf_status` reports whether the mode is active, waiting, or disabled. Connect clients and check role assignment, combat, menus, and round transitions before opening the server to players.

### Configuration and player data

| File or directory | Purpose |
| --- | --- |
| [configs/server.cfg](configs/server.cfg) | Example hostname, network, and public-server settings. Merge with your own config. |
| [configs/gamemode_casual_server.cfg](configs/gamemode_casual_server.cfg) | Casual-mode setup and execution of the gameplay config. |
| [configs/shroudfire.cfg](configs/shroudfire.cfg) | Gameplay balance, map rotation, upgrades, HUD, chat hints, and sound settings. |
| `game/csgo/addons/shroudfire/admins.txt` | Private Steam64 administrator list. |
| `game/csgo/addons/shroudfire/data/` | Persistent player XP, upgrades, combat stats, audio preferences, and queue data. |

Keep your actual Steam login token, passwords, and administrator list on the server. Back up configs and player data before updating or migrating. Stop and restart CS2 when replacing or removing the plugin; live unloading is refused once the engine adapter initializes.

### Map rotation

By default, a map changes after **20 completed rounds or 30 minutes of play**, whichever comes first. The time counter pauses when fewer than two humans are playing or Shroudfire is disabled. A live multiplayer round finishes before the change, and players receive a five-second announcement.

Configure the limits and map order in `game/csgo/cfg/shroudfire.cfg`:

```text
sf_cvar sf_map_max_rounds 20
sf_cvar sf_map_time_limit 30
sf_cvar sf_map_rotation "de_dust2,de_mirage,de_inferno,de_nuke"
```

Set a limit to `0` to disable it; set both to `0` for unlimited play on the current map. The rotation wraps after its last entry and skips unavailable maps.

`sf_nextmap [map|reset]` — Displays or sets the next map; use reset to follow the configured rotation.

Generic administrators and the server console can use `sf_nextmap de_mirage` to choose the next destination, `sf_nextmap` to view it, or `sf_nextmap reset` to use the rotation again. The override applies to one map change. Use `sf_status` to inspect the round count, play time, limits, and next map. See [map rotation settings](COMMANDS.md#map-rotation) for the full behavior.

### Server updates

The optional [Linux update monitor](LINUX.md#automatic-updates) checks Steam every five minutes, announces restarts at 30, 20, and 10 seconds, backs up server state, and updates game files. If a new binary needs adapter changes, it waits for compatibility review before resuming gameplay.

## Build

Build for the server's operating system and x86-64 architecture. You need Git, Python 3 with pip and virtual-environment support, and a C++17 compiler. On Windows, use PowerShell with the Visual Studio 2022 x64 C++ build environment loaded. On Linux, use GCC or Clang with runtime libraries compatible with the deployment host.

From the repository root, create and activate a Python environment.

**Windows:**

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
```

**Linux:**

```bash
python3 -m venv .venv
source .venv/bin/activate
```

With that environment active, fetch the build dependencies. These commands work on either platform:

```text
mkdir deps
git clone https://github.com/alliedmodders/ambuild deps/ambuild
python -m pip install ./deps/ambuild
git clone --recursive https://github.com/alliedmodders/metamod-source deps/metamod-source
git clone https://github.com/alliedmodders/hl2sdk-manifests deps/hl2sdk-manifests
git clone --recursive --branch cs2 https://github.com/alliedmodders/hl2sdk deps/hl2sdk-cs2
```

The project requires **AMBuild 2.2 or newer** and the **`cs2` branch of HL2SDK**. Configure and compile in a separate build directory:

```text
mkdir build
cd build
python ../configure.py -s cs2 --targets=x86_64 --enable-optimize --mms_path=../deps/metamod-source --hl2sdk-root=../deps --hl2sdk-manifests=../deps/hl2sdk-manifests
ambuild
```

The package under `build/package/cs2` contains the platform's plugin binary, MetaMod VDF, example administrator file, and license notices, including the Valve linking exception:

| Platform | Plugin binary within the package |
| --- | --- |
| Windows x86-64 | `addons/shroudfire/bin/win64/shroudfire.dll` |
| Linux x86-64 | `addons/shroudfire/bin/linuxsteamrt64/shroudfire.so` |

Install server configs and Workshop assets separately. For later source changes, run `ambuild` again from the configured build directory. Rebuilding unchanged source does not add support for a different CS2 server binary.

## Build or update with AI

Use AI to build the project, make gameplay changes, fix bugs, or adapt it to CS2 updates. Open this checkout in an AI coding assistant with access to the files and terminal, then give it a concrete request. For example:

> Build Shroudfire for Linux x86-64. Read the README and build scripts, set up the dependencies, compile the plugin, and show me the installable package.

> Change Ward's default cooldown to 25 seconds. Update the plugin, example config, in-game guide where applicable, and command documentation so they agree. Build the result and explain what changed.

> Update Shroudfire for the CS2 server binary at [path]. Inspect the engine adapter and the new binary, verify the native calls, offsets, and signatures, and rebuild. Keep the fingerprint guard and report what still needs live verification.

Tell the assistant your operating system, relevant local paths, and the behavior you want. For native compatibility work, provide the actual server binary and startup logs. A changed fingerprint alone is insufficient: the native interfaces and calls must be reviewed too. After rebuilding, verify the change on a CS2 server with multiple clients.

These are the main starting points for development:

| Area | Files |
| --- | --- |
| Roles, round rules, XP, and upgrades | [src/mode.h](src/mode.h), [src/mode.cpp](src/mode.cpp) |
| CS2 hooks, commands, HUD, and menus | [src/plugin.cpp](src/plugin.cpp), [src/plugin.h](src/plugin.h) |
| Native engine compatibility | [src/engine_adapter.cpp](src/engine_adapter.cpp), [src/engine_adapter.h](src/engine_adapter.h) |
| Player profiles and rankings | [src/storage.cpp](src/storage.cpp), [src/ranking.h](src/ranking.h) |
| In-game guide content | [guides.json](guides.json); regenerate with `python tools/generate_help_pages.py`. |
| Build and packaging | [configure.py](configure.py), [AMBuildScript](AMBuildScript), [AMBuilder](AMBuilder), [PackageScript](PackageScript) |
| Sound sources and Workshop assets | [sound-pack/](sound-pack/), [sound-pack/README.md](sound-pack/README.md) |
| Generation and diagnostic tools | [tools/README.md](tools/README.md) |

Preserve file encodings when editing. Keep changed commands and settings consistent with [COMMANDS.md](COMMANDS.md) and the example configs. Guide and sound generators are documented in [tools/README.md](tools/README.md).

## Workshop pack and audio

The [Workshop pack](https://steamcommunity.com/sharedfiles/filedetails/?id=3819056800) contains custom voices, movement sound overrides, and the `shroudfire_launch` instruction map. The plugin package does not distribute these assets; clients need the compiled addon mounted through MultiAddonManager. Follow [sound-pack/README.md](sound-pack/README.md) for installation, asset compilation, and publishing.

Custom voices cover role starts, round results, kill streaks, starvation, timeout warnings, AFK, and camping warnings. Leave `sf_custom_sounds 0` until client downloads are working, then set it to `1`. Players can toggle eight audio categories with `audio` across two pages of four settings. Camping controls camping-warning voices; Menu controls HUD navigation and selection beeps. Choices persist across sessions and roles.

Local movement silencing is experimental. The compiled default is `sf_silent_local_movement 0`, while the supplied gameplay config sets it to `1`. It requires the updated Workshop pack mounted on the server and clients. The pack's CT footstep overrides target the default Invisant team; shared jump and landing overrides can silence those local sounds for both roles. See [local movement silencing](sound-pack/README.md#local-movement-silencing) before enabling or changing it.

The pack's **51 MP3 voice files were generated with elevenlabs.io** on the ElevenLabs free plan. They have separate non-commercial terms and attribution requirements. Read [SOUNDS-LICENSE.md](sound-pack/SOUNDS-LICENSE.md) before using or redistributing them.

## Compatibility and known limitations

Shroudfire uses private CS2 engine calls. Before enabling them, the adapter checks the loaded server module's size and CRC32 against the values in [src/engine_adapter.cpp](src/engine_adapter.cpp):

| Platform | Server module | Size in bytes | CRC32 |
| --- | --- | ---: | --- |
| Windows x86-64 | `server.dll` | 34,672,792 | `16a83a46` |
| Linux x86-64 | `libserver.so` | 41,850,680 | `6371b479` |

Both adapters were reviewed for CS2 **1.41.9.6**, Steam build **26445667**.

These are the binaries accepted by this source revision. A CS2 update can leave the plugin loaded with its engine adapter disabled. If startup reports `Unsupported server module`, the adapter needs review for the new binary; `sf_enable 1`, a MetaMod update, or recompiling unchanged code cannot resolve it.

The server controls Invisant visibility and filters outgoing sound events. Check the in-game `bugs` and `cheat` guides for player-facing explanations of visibility, audio, and animations.

Other limits to keep in mind:

- CS2 predicts local movement sounds before server-side filtering. Silencing them requires the Workshop overrides described above.
- Dexterity changes server reload completion and weapon-switch attack timing; first-person animations can retain their normal speed.
- `sf_tonemap_exposure` adjusts a map's tonemap controller where possible. It cannot rebake stock-map lighting.

For load errors, Steam registration, launch options, and update recovery, see [Linux troubleshooting](LINUX.md#troubleshooting-and-updates).

## License and credits

Original Shroudfire source code and assets are licensed under [GNU GPL version 3 only (GPL-3.0-only)](LICENSE), with the [Valve linking exception](LICENSE-EXCEPTION), except where a separate notice applies. The 51 voice MP3s and their converted copies are excluded and governed by [the voice-file terms](sound-pack/SOUNDS-LICENSE.md). See [NOTICE](NOTICE) for copyright and scope notices. When distributing plugin binaries, provide their complete corresponding source, including build scripts, as required by GPLv3. Valve components remain subject to their own terms.

The native project started from AlliedModders' Source 2 MetaMod sample; its notice is retained in [third-party/metamod-source-LICENSE.txt](third-party/metamod-source-LICENSE.txt). Third-party components remain subject to their own licenses.
