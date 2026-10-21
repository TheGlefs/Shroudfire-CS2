# Shroudfire Workshop pack — generated with elevenlabs.io

The Workshop pack supplies Shroudfire's custom voices, local movement sound overrides, and the `shroudfire_launch` instruction map. This directory contains the source assets; CS2 uses their compiled Workshop versions.

**Workshop item:** [Shroudfire sound pack](https://steamcommunity.com/sharedfiles/filedetails/?id=3819056800), ID `3819056800`.

The 51 voice MP3s were generated using the ElevenLabs free plan and have separate non-commercial terms. Read [SOUNDS-LICENSE.md](SOUNDS-LICENSE.md) before using or distributing them.

[Server setup](#use-the-voice-pack-on-a-server) · [Build and publish](#build-or-update-the-addon) · [Movement sounds](#local-movement-silencing) · [Maintenance](#maintain-the-source-files)

## Use the voice pack on a server

Install [MultiAddonManager](https://github.com/Source2ZE/MultiAddonManager) for the server's operating system, then add the pack to `game/csgo/cfg/multiaddonmanager/multiaddonmanager.cfg`:

```text
mm_extra_addons "3819056800"
mm_client_extra_addons "3819056800"
mm_addon_mount_download 1
```

The repository includes this setup in [configs/multiaddonmanager.cfg](../configs/multiaddonmanager.cfg). Preserve any other Workshop IDs already configured; multiple IDs are comma-separated. The pack must be downloaded and mounted on both the server and clients. Copying source MP3s into the server's files does not distribute playable assets to clients.

Once downloads are configured, enable custom voices in `game/csgo/cfg/shroudfire.cfg`:

```text
sf_cvar sf_custom_sounds 1
```

The supplied [gameplay config](../configs/shroudfire.cfg) leaves custom voices disabled until this setup is complete. Local movement silencing is enabled by that config and has its own behavior described below.

Reconnect a CS2 client so it receives the addon, then play a clip from the client console:

```text
play sounds/shroudfire/face_the_invisant_fear_nothing.vsnd
```

If this does not play, resolve the client download or mount before checking automatic voice playback. Keep `sf_custom_sounds 0` when the voice assets are unavailable.

### Player audio preferences

Type `audio` in chat to toggle Round Start, Round End, Kill Streak, Starvation, Timeout, Camping, AFK, and Menu audio across two pages of four settings. Camping controls camping-warning voices independently of AFK; Menu controls built-in HUD navigation and selection beeps. All categories start ON, and changes are saved per player.

These preferences persist across roles, reconnects, map changes, and server restarts. They affect custom voices; built-in menu, ability, level-up, hit, and countdown sounds remain separate. See [the command reference](../COMMANDS.md#audio-preferences) for controls and settings.

## Playback behavior

| Event | Voice selection |
| --- | --- |
| Round start | One of six clips for the player's role, after freeze time. |
| Round result | One of three clips for an Armitive win, Invisant win, or draw, after `sf_round_win_sound_delay` seconds (default `0.5`). |
| Timeout reveal warning | One of five clips for living Invisant when the warning begins. Uses the Timeout preference and applies when timeout reveals surviving Invisant. |
| Starvation | One of five clips when an Invisant starvation episode starts. |
| AFK warning | One of five clips when the AFK warning begins. |
| Armitive camping warning | One of five clips when the camping warning begins. Uses the AFK preference. |
| Kill streak | The matching streak clip, heard by the player who earned it. |

Randomized groups avoid immediately repeating their previous selection. The plugin chooses playback and timing; editing or recompiling an asset alone does not change that logic.

The pack also mutes the stock `Event.RoundDraw` and `Announcer.RoundDraw.CS2_Classic` events while mounted. These asset overrides apply independently of `sf_custom_sounds` and player voice preferences.

## Local movement silencing

CS2 predicts a player's own movement sounds locally, before the server can filter outgoing audio. This pack includes full stock event definitions with their volume set to zero:

| Override | Effect while the pack is mounted |
| --- | --- |
| CT footstep events | Silences footsteps for the default Invisant team. Armitive use T by default, so their footstep events are unaffected. |
| `Land_*`, `Land.Thud`, and `Default.Land` | Silences shared landing sounds for both roles. |
| `Default.WalkJump` | Silences the shared jump sound for both roles. |

The footstep overrides follow the CS2 team, not the Shroudfire role. Changing `sf_invisant_team` does not move those overrides to the other team. Weapon, damage, and custom voice audio are separate.

The plugin also has a local movement setting. Its compiled default is `sf_silent_local_movement 0`; the supplied `shroudfire.cfg` sets it to `1`. Disabling the setting stops the plugin's movement-blocker events, but **does not undo the volume overrides baked into a mounted pack**. Restoring those stock event volumes requires changing the addon overrides or removing the addon, then reconnecting clients to load the intended assets.

The override source is [soundevents/movement_overrides.vsndevts.inc](soundevents/movement_overrides.vsndevts.inc). The generator merges it into `soundevents_addon.vsndevts`; keep the complete definitions rather than replacing them with abbreviated blocks, which CS2's compiler discards. The current set contains 47 `CT_*.StepLeft` events, 48 `Land_*` events, and the three shared events listed above.

## Build or update the addon

Use Counter-Strike 2 Workshop Tools on Windows. Create or open a dedicated addon, normally named `shroudfire`, in the CS2 installation you intend to build from. Keep `sounds/shroudfire` and the `shroudfire.*` event names consistent with the plugin.

### Prepare the sources

Run these from the repository root, replacing the game path with your installation:

```powershell
python tools/generate_sound_pack.py
.\tools\prepare_workshop_sounds.ps1 -GamePath 'D:\SteamLibrary\steamapps\common\Counter-Strike Global Offensive'
```

The generator checks the clip list and produces the complete sound-event file. The PowerShell helper copies voice sources, generated events, sound stacks, manifests, the instruction map, and its materials into an **existing** addon's `content/csgo_addons/shroudfire` directory. Pass `-AddonName '<name>'` for a different addon.

The helper preserves unrelated addon files and the existing addon manifest. It prepares source files; compilation and publishing happen in Workshop Tools.

For the playable instruction map, configure `game/csgo_addons/<addon-name>/addoninfo.txt` with `IsPlayable = true` and `maps = [ "shroudfire_launch" ]`. On a fresh dedicated addon, use the supplied [addoninfo.txt](addoninfo.txt). On an existing addon, merge these fields while preserving its other settings and file encoding.

To prepare files manually, copy this directory's `sounds`, `soundevents`, `soundstacks`, `resourcemanifests`, `maps`, and `materials` folders into `content/csgo_addons/<addon-name>` and configure the manifest as described above.

### Compile the assets

Use the Workshop Tools Asset Browser and Hammer:

1. Compile the voice MP3s, `soundstacks/shroudfire_movement.vsndstck`, and `soundstacks/soundstacks_manifest.vrman`.
2. Compile `soundevents/soundevents_addon.vsndevts`, then `resourcemanifests/shroudfire_addon_resources.vrman`. The resource manifest registers the addon's sound events when the pack loads.
3. Compile `materials/shroudfire/instruction_logo.vtex`, then the three `.vmat` files in that directory. The logo material depends on this texture.
4. Build `maps/shroudfire_launch.vmap` in Hammer with physics, visibility, and baked lighting.

Check the compiled outputs under `game/csgo_addons/<addon-name>`: `.vsnd_c`, `.vsndstck_c`, `.vsndevts_c`, `.vrman_c`, `.vtex_c`, `.vmat_c`, and `maps/shroudfire_launch.vpk`. For movement changes, inspect the compiled event file to confirm the full override set survived compilation.

Open the map and check its floor, artwork, and connection instructions. Check voice playback and movement audio on the surfaces affected by an audio change, including concrete, wood, metal, sand, ladders, water, and wet surfaces.

### Publish the Workshop item

Publish through Workshop Manager using a Steam account with the necessary item permissions. Select `shroudfire_launch` as the map and include the compiled voice assets, sound events, manifests, sound stacks, materials, and texture dependencies.

For the project's item, update existing item `3819056800`. If publishing a separate item, use its assigned ID in both server addon lists. Include `elevenlabs.io` in the published title and retain the [voice-file notice](SOUNDS-LICENSE.md) in the distribution and item description. For example:

```text
Shroudfire Workshop pack — generated with elevenlabs.io
```

Publishing makes the compiled update available to subscribers. Reconnect clients and confirm they are using the new version before checking its behavior. Local ZIP archives are optional outputs and may be absent from a source checkout.

## Joining Shroudfire

Opening `shroudfire_launch` from **Play > Workshop Maps > GO** displays artwork and instructions for joining the server. The map is an entry point with instructions; gameplay runs on the Shroudfire server.

Add `cs2.shroudfire.com` to Favorites in the Community Server Browser, or enter this in the CS2 developer console:

```text
connect cs2.shroudfire.com
```

The Community Server Browser opens from the globe icon under Play. Enable the console under **Settings > Game > Enable Developer Console > Yes** if needed.

To change the map's instructions, edit the signs' `message` properties in [maps/shroudfire_launch.vmap](maps/shroudfire_launch.vmap), then rebuild and publish. The logo texture uses `shroudfire_blended.png`, a transparent derivative of the preserved original `shroudfire.png`.

## Maintain the source files

For changes, open the repository in an AI coding assistant and describe the voices, map content, or audio behavior you want. The [project README](../README.md#build-or-update-with-ai) explains the development workflow. Supply your Workshop addon path when preparing a local build.

| Source | Purpose |
| --- | --- |
| [sounds/shroudfire/](sounds/shroudfire/) | The 51 MP3 voice sources. |
| [tools/generate_sound_pack.py](../tools/generate_sound_pack.py) | Canonical clip list and sound-event generator. |
| [soundevents/movement_overrides.vsndevts.inc](soundevents/movement_overrides.vsndevts.inc) | Complete stock movement events with muted volumes. |
| [soundevents/soundevents_addon.vsndevts](soundevents/soundevents_addon.vsndevts) | Generated voice events, draw-announcer overrides, and merged movement overrides. |
| [soundstacks/](soundstacks/) | Movement sound stack and its manifest. |
| [resourcemanifests/shroudfire_addon_resources.vrman](resourcemanifests/shroudfire_addon_resources.vrman) | Addon sound-resource registration. |
| [maps/shroudfire_launch.vmap](maps/shroudfire_launch.vmap) | Instruction map. |
| [materials/shroudfire/](materials/shroudfire/) | Artwork, texture definition, and map materials. |
| [src/plugin.cpp](../src/plugin.cpp) | Voice selection, timing, player preferences, and movement-blocker emission. |

When adding or removing clips, update both the MP3 files and the generator's `SOUNDS` list. Regenerate the event file, prepare the addon, compile, and publish. Update the plugin's sound arrays and rebuild the plugin when changing which clips it selects.

Preserve file encodings and existing asset names unless updating every reference. In particular, `the_hunt_begins_leave_no_surviors` and `the_invisant_wants_blood_spill_its_own` intentionally match the supplied files. See [tools/README.md](../tools/README.md) for generator and preparation commands.

## Voice-file terms

The MP3 sources and their modified or converted copies are excluded from the repository's GPLv3 license. Their [separate notice](SOUNDS-LICENSE.md) permits non-commercial use with attribution and the stated conditions. Retain that notice and the required publishing attribution when distributing the pack.

A later paid ElevenLabs subscription does not change the rights to these existing free-plan files. For commercial distribution, remove them or replace them with audio whose terms permit the intended use. Other original Shroudfire material remains covered by the project's license unless a separate notice applies.
