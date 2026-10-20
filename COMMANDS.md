# Shroudfire commands and settings

This is the reference for player chat shortcuts, administrator commands, and all server settings. **Invisant** means **invisible assailant**; **Armitive** means **armed fugitive**. For installation, start with the [README](README.md) or [Linux guide](LINUX.md).

[Player menus](#chat-shortcuts-and-menus) · [Console commands](#console-commands) · [Administrator access](#administrator-access) · [Server settings](#server-settings)

## Chat shortcuts and menus

Type these in chat without a prefix. Matching ignores capitalization and surrounding whitespace. Recognized shortcuts open private HUD panels and are consumed by the plugin.

| Chat text | Action |
| --- | --- |
| `help` | Open the gameplay guide. |
| `menu` | Open the upgrade menu for your current role. |
| `upgrade` | Open the upgrade guide. |
| `reset` | Refund your current role's upgrades and open the upgrade menu. |
| `rank` | Open your rank menu: Armitive stats, Invisant stats, and menu navigation. |
| `audio` | Open your audio settings menu. |
| `bugs` | Open the known bugs guide. |
| `cheat` | Open the visibility and anti-cheat guide. |
| `close` | Close an open upgrade menu or guide. |
| `1`–`8` | Buy the corresponding upgrade while the upgrade menu is open, subject to points and unlock levels. |

### Menu controls

The same keyboard controls work while alive or dead. Each key acts once per press; release it before using it again.

| Key | Action |
| --- | --- |
| A / D | Change pages. |
| W / S | Move through the options. |
| SPACE | Select the highlighted option. On an informational guide page, jump to the final options page with its first option selected. |
| R | Jump to the final page with Exit Menu selected. Press SPACE to exit. |

An open panel takes priority over spectator controls while dead. The spectator `spec_mode` command selects the current menu option. Mouse buttons do not navigate menus. Use `close` in chat to close a panel directly.

The upgrade menu has two pages of four upgrades and a final options page. Each page shows your current role's level and available points. Guide options link to the other guides, audio preferences, and the upgrade menu. Page and option navigation, shortcuts, selection, and exiting use distinct built-in interface sounds.

### Upgrade purchases and resets

Each role has its own XP, level, points, and eight upgrades. Buying requires an active role; ordinary spectators cannot buy upgrades. Normal and ultimate upgrades each have five stages, with unlock levels controlled by the settings below.

Purchases and resets during `sf_upgrade_purchase_window` apply immediately. Later choices are marked “next round” and take effect at the next round start. `reset` refunds only your current role and opens the first upgrade page, including when another guide is open or the menu is closed. Spending your final point leaves the menu open.

After buying Surge or Ward, use your Use key (normally E) to activate it. With `cl_use_opens_buy_menu 1`, an available buy menu takes priority over Ward. Cooldown attempts show the actual seconds remaining; a ready notice appears when the cooldown ends.

### Rankings

`rank` shows separate Armitive and Invisant pages with placement, rating, kills, assists, deaths, and K/A/D ratio. It works while alive, dead, or spectating. Rankings include saved offline players and current online records. Equal ratings share a place; a role with no recorded combat displays `Rank 0 of X`.

Kills and qualifying damage assists contribute equally. The displayed ratio is `(kills + assists) / deaths`; contributions without deaths display `inf`, and an empty record displays `0`. Ratings combine the Wilson lower bound with a participation weight:

```text
C = kills + assists
N = C + deaths
P = C / N
Z = 1.96
W = (P + Z²/(2N) - Z * sqrt((P*(1-P) + Z²/(4N))/N)) / (1 + Z²/N)
rating = 1000 * W * C / (C + 100)
```

Zero contributions score zero. The participation weight reaches half strength at 100 contributions and approaches full strength as the record grows. See [src/ranking.h](src/ranking.h) for the implementation.

Only live play after freeze time with at least two active human players counts. Bot combat, inactive victims, forced respawns, timeout cleanup, and post-round combat are excluded. Opponent kills, suicide, falls, world damage, AFK, starvation, and camping can count as live human deaths. Assist eligibility uses `sf_xp_assist_damage_share`; ranking contributions still count at the level cap and when XP awards are disabled. Stats persist through reconnects, map changes, level changes, and upgrade resets.

### Audio preferences

`audio` opens eight toggles across two settings pages, four per page: Round Start, Round End, Kill Streak, Starvation, Timeout, Camping, AFK, and Menu. Camping controls camping-warning voices independently of AFK. Menu controls navigation, shortcut, selection, toggle, and exit beeps throughout the HUD menus. All categories start ON; selecting one toggles it and saves the preference immediately.

Preferences follow the player across roles, reconnects, map changes, and server restarts. The final options page keeps the guide, upgrade-menu, and Exit Menu links; R selects Exit Menu. Menu sounds use built-in CS2 audio. Custom voices require both `sf_custom_sounds 1` and the Workshop pack mounted for clients. See the [Workshop guide](sound-pack/README.md).

## Console commands

Enter these in the player or server console according to their access level. Server-console commands do not require an administrator entry.

| Command | Access | Description |
| --- | --- | --- |
| `sf_level` | Connected player | Displays your Invisant and Armitive XP and levels. |
| `sf_status` | Generic admin or server console | Displays Shroudfire state, map rotation, teams, queue, AFK players, and spawn capacity. |
| `sf_cvar <name> [value]` | Generic admin or server console | Reads or sets a server cvar from the console. Requires the generic admin flag. |
| `sf_restart` | Generic admin or server console | Restarts the current round without advancing the Invisant roster, including with one active player. Requires the generic admin flag. |
| `sf_nextmap [map\|reset]` | Generic admin or server console | Displays or sets the next map; use reset to follow the configured rotation. |
| `sf_setlevel <player> <invisant\|armitive> <level>` | Root admin or server console | Sets an online player's Invisant or Armitive level to its starting XP, resets that role's purchased upgrades, and recalculates its available upgrade points for the assigned level. |
| `sf_invisant` | Connected root admin | Moves you to the Invisant team and respawns you at its spawn. |
| `sf_armitive` | Connected root admin | Moves you to the Armitive team and respawns you at its spawn. |
| `sf_observe [player]` | Connected generic admin; `sf_admin_spectate 1` | Moves you to Spectator to watch a living player or use a free camera. |
| `sf_update_notice <seconds>` | Server console only | Announces an upcoming CS2 update restart in chat. Server console only. |

```text
sf_cvar sf_round_time
sf_cvar sf_round_time 150
sf_restart
sf_nextmap
sf_nextmap de_mirage
sf_nextmap reset
sf_setlevel "Player Name" armitive 10
sf_observe "Player Name"
```

`sf_cvar` also supports stock CS2 cvars. Protected values are hidden, and changes are logged. Put persistent changes in your server config; console changes alone do not survive a restart.

`sf_setlevel` accepts levels 0–40. Player targets resolve by online human slot number, Steam64 ID, or a unique case-insensitive name fragment. Quote names containing spaces.

`sf_nextmap` without arguments shows the next available map. Give it an installed map name without an extension to override the next destination, or `reset` to return to the configured rotation. The override is used for one map change and is cleared when the next map loads. It chooses the destination without changing maps immediately; the round or time limit still controls when the change happens. Map names are checked against the server before acceptance and again before changing levels. If an overridden map becomes unavailable, the configured rotation supplies the fallback.

`sf_status` includes the current map, completed round count, accumulated play time, configured limits, and next destination. Changing limits or the rotation with `sf_cvar` takes effect during the current map. Raising or disabling a reached limit cancels its pending change if neither limit remains reached.

`sf_observe` moves the administrator to Spectator and logs the observation. With no target, it prefers a living Invisant, then a living Armitive, then a free camera. It waits up to five seconds for camera readiness and preserves its selected target in a two-player session. Pending requests end if either player disconnects, the viewer leaves Spectator, the target becomes unavailable, or administrator observation is disabled.

`sf_update_notice` accepts 1–3600 seconds and broadcasts a restart notice to all connected human players, including spectators. It sends the message only. The [Linux update monitor](LINUX.md#automatic-updates) schedules its own 30/20/10-second notices and server restart.

## Administrator access

Under the server's `game/csgo` directory, copy `addons/shroudfire/admins.example.txt` to `addons/shroudfire/admins.txt`. Add one Steam64 ID and access level per line:

```text
# Replace these IDs with your own.
76561198000000000 generic
76561198000000001 root
```

| Access | Permissions |
| --- | --- |
| `generic` | Status, cvar access, round restart, next-map selection, and administrator observation. |
| `root` | All generic permissions, level changes, and forcing your own Invisant or Armitive role. |

The plugin reads this file when it loads. Restart CS2 after editing administrator access. Keep the actual file private. Role forcing and observation require a connected player; `sf_update_notice` requires the server console.

## Intercepted CS2 commands

Shroudfire also handles these existing game commands while the mode is active:

| CS2 command | Behavior |
| --- | --- |
| `jointeam` | Applies spectator and role-rotation restrictions. |
| `chooseteam` | Suppresses server-side team-menu requests. |
| `joinclass` | Suppresses class changes for living players. |
| `spec_mode` | Selects an open HUD menu while dead instead of changing spectator mode. |
| `kill`, `explode` | Allows suicide during an active round. An Armitive suicide forfeits next-round Invisant eligibility; a solo death restarts the round. |
| `buy`, `autobuy`, `rebuy`, `buyammo1`, `buyammo2`, `buymenu` | Blocks buying for Invisant. Armitive buying follows normal permission checks and `sf_armitive_blocked_weapons`, including autobuy and rebuy. |
| Radio menus, callouts, and `playerchatwheel` | Suppresses Invisant callouts that could reveal their position. |
| `impulse 100`, `impulse 201` | Suppresses Invisant flashlight and spray inputs. |

## Server settings

The tables below list all compiled defaults, limits, and descriptions from [src/plugin.cpp](src/plugin.cpp). Integer settings accept whole numbers; float settings accept decimals. Enter settings in the server console, put them in a config, or use `sf_cvar` as an authorized administrator.

The supplied [configs/shroudfire.cfg](configs/shroudfire.cfg) is executed by the Casual server config. It changes two compiled defaults: `sf_enable` from `0` to `1` and `sf_silent_local_movement` from `0` to `1`. Custom voices remain disabled until Workshop downloads are configured.

For example, put this in the installed `game/csgo/cfg/shroudfire.cfg` to change the round duration:

```text
sf_cvar sf_round_time 150
```

### Game mode and queue

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_enable` | `0` | `0`–`1` (int) | Enables Shroudfire. |
| `sf_invisant_team` | `3` | `2`–`3` (int) | Selects the Invisant team: 2 for Terrorists or 3 for Counter-Terrorists. |
| `sf_visible_per_invisant` | `4` | `2`–`32` (int) | Maximum number of Armitive players per Invisant player. |
| `sf_round_time` | `120` | `30`–`3600` (int) | Seconds before an unfinished round ends with an Armitive win. |
| `sf_timeout_kill_invisant` | `0` | `0`–`1` (int) | Kill surviving Invisant at timeout; 0 reveals them during the 10-second round transition instead. |
| `sf_timeout_reveal_seconds` | `2` | `0`–`10` (float) | Seconds for surviving Invisant to fade from invisible to visible after timeout; 0 reveals instantly. |
| `sf_map_join_grace` | `30` | `0`–`120` (int) | Seconds after map start in which joining players can respawn as Armitive. |
| `sf_queue_reconnect_grace` | `600` | `0`–`86400` (int) | Seconds a reconnecting player retains queue age; 0 is unlimited. |
| `sf_upgrade_purchase_window` | `20` | `0`–`120` (int) | Seconds from round start when purchased or reset upgrades apply immediately. |

Invisant use CT by default. The ratio scales the roster with player count, and the queue rotates eligible players between roles. Timeout awards the round to Armitive; surviving Invisant are either revealed during the transition or killed, according to the timeout setting.

### Map rotation

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_map_max_rounds` | `20` | `0`–`1000` (int) | Completed rounds per map before changing maps; 0 disables the round limit. |
| `sf_map_time_limit` | `30` | `0`–`1440` (float) | Minutes per map with at least two active human players before changing maps; 0 disables the time limit. |
| `sf_map_rotation` | `de_dust2,de_mirage,de_inferno,de_nuke` | String | Comma-separated map rotation; unavailable maps are skipped. |

The first enabled limit reached schedules a map change. A live multiplayer round finishes first, followed by a five-second chat announcement before changing levels. Completed rounds with at least two participants count, including draws. Solo practice, initial restarts, and `sf_restart` do not add a completed round. The time counter runs while Shroudfire is enabled and at least two humans are on the playing teams, including freeze time and round transitions; it pauses for empty servers, solo play, and disabled gameplay.

Set either limit to `0` to disable it, or both to `0` to keep the current map indefinitely. Counters start fresh when a map loads; a plugin loaded into an existing map starts counting from that load. Player profiles remain persistent across map changes.

The rotation accepts comma-separated or whitespace-separated installed map names. It advances after the current map, wraps at the end, and ignores duplicate or unavailable entries. If the current map is absent from the list, it starts at the first available entry. A rotation with only the current map available reloads that map at the limit. If no destination is available, gameplay continues and the plugin reports the problem while checking for an updated destination every five seconds.

Use `sf_nextmap <map>` for an administrator override. The native `mp_timelimit` and `mp_winlimit` settings are held at `0`, and `mp_maxrounds` is held at `999999`, while Shroudfire is active so its own limits control rotation. CS2 can end the match during a round reset when both native round and time limits are `0`. The mode also holds `sv_warmup_to_freezetime_delay` at `0` to skip the pause between warmup and the first playable round. Its native minimum is temporarily lowered to zero while the mode is active, then restored when disabled or unloaded. Initial join recovery waits 0.1 seconds for connection setup and does not force a freeze before restarting.

### AFK handling

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_afk_seconds` | `30` | `0`–`600` (int) | Seconds of inactivity after freeze time before a player is killed; 0 disables AFK deaths. |
| `sf_afk_warning_seconds` | `10` | `0`–`600` (int) | Seconds before AFK death to start the HTML countdown and warning sounds; 0 disables the warnings. |
| `sf_afk_limit` | `5` | `0`–`100` (int) | Consecutive deaths without live-round movement before a player is kicked; 0 disables kicks. |

AFK timing starts after freeze time. An AFK warning or pending AFK death takes priority over camping warnings and damage.

### Invisant starvation

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_starvation_round_delay` | `60` | `0`–`3600` (float) | Seconds of live play after freeze time before Invisant starvation can begin. |
| `sf_starvation_combat_delay` | `45` | `0`–`3600` (float) | Seconds without dealing or taking damage before Invisant starvation begins. |
| `sf_starvation_damage` | `1` | `1`–`500` (int) | Health an Invisant loses per starvation tick before acceleration. |
| `sf_starvation_interval` | `1` | `0.1`–`60` (float) | Seconds between Invisant starvation ticks before acceleration. |
| `sf_starvation_acceleration_delay` | `20` | `0`–`3600` (float) | Seconds of uninterrupted starvation before accelerated ticks begin; 0 accelerates immediately. |
| `sf_starvation_accelerated_interval` | `1` | `0.1`–`60` (float) | Seconds between Invisant starvation ticks after acceleration. |
| `sf_starvation_accelerated_damage` | `3` | `1`–`500` (int) | Health an Invisant loses per starvation tick after acceleration. |

Both delays must have elapsed: starvation cannot start before the configured live-round delay, and the Invisant must have gone the combat delay without dealing or taking damage. Dealing or taking damage ends starvation and resets its acceleration. Starvation damage itself does not reset the timer. Solo play and rounds without a living Armitive do not cause starvation.

The first damage tick is immediate when starvation starts. With the defaults, it starts after 60 seconds of live play and 45 seconds without combat, deals 1 HP every second for 20 seconds, then deals 3 HP every second. A 100-HP Invisant that begins starving at 1:00 dies with roughly 0:15 left in a two-minute round if combat never resets it. Intervals support fractions of a second and run independently of the once-per-second mode update. These rates are fixed rather than automatically scaled to the round length.

### Armitive anti-camping

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_camping_distance` | `256` | `0`–`4096` (float) | Distance in units an Armitive must move from the tracked position before the camping timer resets; 0 disables anti-camping. |
| `sf_camping_warning_seconds` | `20` | `0`–`600` (float) | Seconds an Armitive may remain within the camping distance before the HTML and voice warning. |
| `sf_camping_damage_delay` | `10` | `0`–`600` (float) | Seconds from the Armitive camping warning until health loss begins. |
| `sf_camping_damage` | `10` | `1`–`500` (int) | Health an Armitive loses per second while camping. |

An Armitive must move beyond the tracked distance to reset the camping timer. With the defaults, the warning begins after 20 seconds and health loss starts 10 seconds later. Recovery pauses during camping damage and resumes after its normal post-damage interval.

### XP and kill streaks

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_xp_kill_invisant` | `50` | `0`–`10000` (int) | XP for an Invisant killing an Armitive player. |
| `sf_xp_kill_armitive` | `100` | `0`–`10000` (int) | XP for an Armitive player killing an Invisant. |
| `sf_xp_round_win_invisant` | `70` | `0`–`10000` (int) | XP for an Invisant participant on a completed win. |
| `sf_xp_round_win_armitive` | `35` | `0`–`10000` (int) | XP for an Armitive participant on a completed win. |
| `sf_xp_armitive_pistol_percent` | `125` | `0`–`1000` (int) | Armitive pistol kill XP as a percent of Armitive base kill XP; 100 removes the bonus. |
| `sf_xp_armitive_knife_percent` | `150` | `0`–`1000` (int) | Armitive knife kill XP as a percent of Armitive base kill XP; 100 removes the bonus. |
| `sf_xp_level_base` | `100` | `1`–`1000000` (int) | Base XP for the level curve. |
| `sf_xp_assist_fraction` | `0.25` | `0`–`1` (float) | Fraction of the current role's kill XP awarded for a qualifying assist. Set to 0 to disable assist XP. |
| `sf_xp_assist_damage_share` | `0.25` | `0`–`1` (float) | Minimum share of a victim's opposing-player damage required for assist XP. 0.25 means 25 percent. |
| `sf_xp_first_blood_percent` | `125` | `0`–`1000` (int) | Kill XP percent for First Blood; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_double_kill_percent` | `120` | `0`–`1000` (int) | Kill XP percent for Double Kill; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_killing_spree_percent` | `130` | `0`–`1000` (int) | Kill XP percent for Killing Spree; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_ultra_kill_percent` | `140` | `0`–`1000` (int) | Kill XP percent for Ultra Kill; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_rampage_percent` | `150` | `0`–`1000` (int) | Kill XP percent for Rampage; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_dominating_percent` | `160` | `0`–`1000` (int) | Kill XP percent for Dominating; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_unstoppable_percent` | `170` | `0`–`1000` (int) | Kill XP percent for Unstoppable; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_monster_kill_percent` | `180` | `0`–`1000` (int) | Kill XP percent for Monster Kill; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_godlike_percent` | `190` | `0`–`1000` (int) | Kill XP percent for Godlike; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_xp_holy_shit_percent` | `200` | `0`–`1000` (int) | Kill XP percent for Holy Shit; multiplies the Armitive knife or pistol bonus when applicable. |
| `sf_kill_streak_window` | `10` | `1`–`60` (float) | Maximum seconds between a player's consecutive opponent kills for a kill streak. |

Kill bonuses multiply where applicable. Qualifying assists receive a fraction of the role's kill XP, subject to the damage-share threshold. Kills after round end grant no XP, ranking contribution, or Invisant queue priority.

### Upgrade unlock levels

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_normal_stage1_level` | `1` | `1`–`40` (int) | Role level required for normal upgrade stage 1. |
| `sf_normal_stage2_level` | `5` | `1`–`40` (int) | Role level required for normal upgrade stage 2. |
| `sf_normal_stage3_level` | `10` | `1`–`40` (int) | Role level required for normal upgrade stage 3. |
| `sf_normal_stage4_level` | `15` | `1`–`40` (int) | Role level required for normal upgrade stage 4. |
| `sf_normal_stage5_level` | `20` | `1`–`40` (int) | Role level required for normal upgrade stage 5. |
| `sf_ultimate_stage1_level` | `8` | `1`–`40` (int) | Role level required for ultimate upgrade stage 1. |
| `sf_ultimate_stage2_level` | `16` | `1`–`40` (int) | Role level required for ultimate upgrade stage 2. |
| `sf_ultimate_stage3_level` | `24` | `1`–`40` (int) | Role level required for ultimate upgrade stage 3. |
| `sf_ultimate_stage4_level` | `32` | `1`–`40` (int) | Role level required for ultimate upgrade stage 4. |
| `sf_ultimate_stage5_level` | `40` | `1`–`40` (int) | Role level required for ultimate upgrade stage 5. |

Normal and ultimate upgrades have five stages. These settings specify the minimum role level for each stage; XP, points, and purchased stages are separate for Invisant and Armitive.

### Invisant upgrades and knife

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_invisant_base_armor` | `25` | `0`–`200` (int) | Invisant armor before Resilience. |
| `sf_knife_damage_min` | `50` | `1`–`1000` (int) | Minimum Invisant knife damage per hit, inclusive. |
| `sf_knife_damage_max` | `75` | `1`–`1000` (int) | Maximum Invisant knife damage per hit, inclusive. |
| `sf_knife_range` | `72` | `0`–`16384` (float) | Maximum Invisant knife reach in units; 0 disables extended hits. |
| `sf_knife_cone` | `60` | `0`–`360` (float) | Full Invisant knife cone width in degrees; 360 covers every direction. |
| `sf_knife_attack_speed_percent` | `66` | `1`–`100` (int) | Invisant knife attack speed as a percent of normal; 100 restores the normal rate. |
| `sf_knife_miss_sound` | `1` | `0`–`1` (int) | Replay Invisant knife air-miss sounds spatially to Armitive players. |
| `sf_knife_world_hit_sound` | `1` | `0`–`1` (int) | Replay Invisant knife wall and ground impact sounds spatially to Armitive players. |
| `sf_lightweight_gravity_percent` | `10` | `0`–`20` (int) | Gravity reduction percent per Lightweight stage. |
| `sf_momentum_run_percent` | `3` | `0`–`100` (int) | Run speed increase percent per Momentum stage. |
| `sf_resilience_armor_per_stage` | `15` | `0`–`100` (int) | Maximum armor gained per Resilience stage. |
| `sf_resilience_helmet_stage` | `5` | `1`–`5` (int) | Resilience stage that grants a helmet. |
| `sf_reforging_armor_per_stage` | `1` | `0`–`100` (int) | Armor regenerated per Reforging stage each interval. |
| `sf_reforging_interval` | `3` | `1`–`60` (int) | Seconds between Reforging armor ticks and after damage. |
| `sf_vampirism_health_per_stage` | `3` | `0`–`100` (int) | Health restored per Vampirism stage on a damaging hit. |
| `sf_bloodlust_health_per_stage` | `6` | `0`–`100` (int) | Health restored per Bloodlust stage on a kill. |
| `sf_mobility_crouch_percent` | `15` | `0`–`100` (int) | Crouch speed increase percent per Mobility stage. |
| `sf_surge_base_speed` | `480` | `0`–`2000` (int) | Surge speed boost at stage 1. Default: 480. |
| `sf_surge_speed_per_stage` | `30` | `0`–`1000` (int) | Additional Surge speed boost for each stage after stage 1. Default: 30. |
| `sf_surge_add_momentum` | `0` | `0`–`1` (int) | Whether Surge adds to current velocity (1) or replaces it (0). Default: 0. |
| `sf_surge_cooldown` | `20` | `0`–`300` (int) | Seconds until Surge is ready after use. |

Knife damage is rolled once per hit within the inclusive configured range for both native and extended hits. If the bounds are reversed, their lower and higher values define the effective range. Setting both to the same value gives fixed damage. Armitive knife damage follows CS2 rules.

Surge uses the Use key after its first stage is purchased. Native knife air misses and wall or ground impacts can be replayed spatially to Armitive. Player hits keep their impact sound without an additional air-miss cue.

### Armitive weapon purchases

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_armitive_blocked_weapons` | `m249` | String | Comma-separated weapons Armitive cannot buy; empty allows all weapons. |

Use comma-separated or whitespace-separated CS2 weapon names, case-insensitively, with an optional `weapon_` prefix. For example, `sf_cvar sf_armitive_blocked_weapons "m249,negev,awp"` blocks those three weapons; `sf_cvar sf_armitive_blocked_weapons ""` clears the list. Canonical names distinguish `m4a1` (M4A4) from `m4a1_silencer` (M4A1-S). Grenades and the Zeus (`taser`) are supported; common aliases include `m4a4`, `m4a1-s`, `usp-s`, `p2000`, and `sg553`. Unknown names are ignored and reported in the server console. Restrictions apply to purchases through the buy menu, console, autobuy, and rebuy. Dropped-weapon pickups and spawn loadouts follow their existing rules.

### Armitive upgrades

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_armitive_base_health` | `100` | `1`–`500` (int) | Armitive health before Fortitude. |
| `sf_fortitude_health_per_stage` | `10` | `0`–`100` (int) | Maximum health gained per Fortitude stage. |
| `sf_recovery_health_per_stage` | `1` | `0`–`100` (int) | Health regenerated per Recovery stage each interval. |
| `sf_recovery_interval` | `3` | `1`–`60` (int) | Seconds between Recovery health ticks and after damage. |
| `sf_overload_magazine_percent` | `10` | `0`–`50` (int) | Magazine capacity increase percent per Overload stage. |
| `sf_stockpile_clips_per_stage` | `1` | `0`–`5` (int) | Extra reserve clips per Stockpile stage. |
| `sf_barrage_fire_rate_percent` | `7` | `0`–`100` (int) | Weapon fire rate increase percent per Barrage stage. |
| `sf_control_reduction_percent` | `10` | `0`–`20` (int) | Recoil and spread reduction percent per Control stage. |
| `sf_dexterity_speed_percent` | `12` | `0`–`19` (int) | Reload and weapon switch time reduction percent per Dexterity stage. |
| `sf_ward_base_seconds` | `2` | `0.1`–`30` (float) | Ward immunity duration at stage 1 in seconds. |
| `sf_ward_seconds_per_stage` | `0.5` | `0`–`10` (float) | Additional Ward seconds for each stage after stage 1. |
| `sf_ward_cooldown` | `30` | `0`–`300` (int) | Seconds until Ward is ready after it ends. |

Ward uses the Use key after its first stage is purchased. Stockpile adds reserve clips; Overload increases magazine capacity and also reserve capacity where a weapon stores reserves as individual rounds. Dexterity changes server timing; first-person animations can retain their normal speed.

### Chat messages

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_audio_hint` | `1` | `0`–`1` (int) | Enables the automatic audio chat hint. |
| `sf_audio_hint_cooldown` | `60` | `0`–`86400` (int) | Seconds before the automatic audio chat hint can trigger again. |
| `sf_cheat_hint` | `1` | `0`–`1` (int) | Enables the automatic cheat chat hint. |
| `sf_cheat_hint_cooldown` | `60` | `0`–`86400` (int) | Seconds before the automatic cheat chat hint can trigger again. |
| `sf_upgrade_hint` | `1` | `0`–`1` (int) | Enables the automatic upgrade chat hint. |
| `sf_upgrade_hint_cooldown` | `60` | `0`–`86400` (int) | Seconds before the automatic upgrade chat hint can trigger again. |
| `sf_bugs_hint` | `1` | `0`–`1` (int) | Enables the automatic known-bugs chat hint. |
| `sf_bugs_hint_cooldown` | `60` | `0`–`86400` (int) | Seconds before the automatic known-bugs chat hint can trigger again. |
| `sf_menu_hint` | `1` | `0`–`1` (int) | Enables the automatic upgrade-menu chat hint. |
| `sf_menu_hint_cooldown` | `60` | `0`–`86400` (int) | Seconds before the automatic upgrade-menu chat hint can trigger again. |
| `sf_reset_hint` | `1` | `0`–`1` (int) | Enables the automatic upgrade-reset chat hint. |
| `sf_reset_hint_cooldown` | `60` | `0`–`86400` (int) | Seconds before the automatic upgrade-reset chat hint can trigger again. |
| `sf_rank_hint` | `1` | `0`–`1` (int) | Enables the automatic rank chat hint. |
| `sf_rank_hint_cooldown` | `60` | `0`–`86400` (int) | Seconds before the automatic rank chat hint can trigger again. |
| `sf_discord_link` | `https://discord.gg/TMV2DEPhqT` | String | Discord server invite link in chat messages; empty disables the messages. |
| `sf_discord_message` | `1` | `0`–`1` (int) | Enables Discord invite messages in chat. |
| `sf_discord_interval` | `1200` | `1`–`86400` (int) | Seconds between recurring Discord invite messages. |

Automatic hints match keywords anywhere in ordinary chat, ignoring capitalization. Each hint has its own cooldown shared across players and preserved across map changes. Exact menu shortcuts, including `upgrade`, open their panels silently. A cooldown of `0` permits every matching message; disabling a hint also cancels pending replies.

| Keywords | Suggested command |
| --- | --- |
| `audio`, `voice`, `sound`, `announce` | `audio` |
| `cheat`, `hack` | `cheat` |
| `upgrade` | `upgrade` |
| `reload`, `animation`, `footstep`, `dexterity` | `bugs` |
| `level`, `point`, `menu`, `xp` | `menu` |
| `reset` | `reset` |
| `rank` | `rank` |

Messages use a green `[Shroudfire]` prefix and gold command names. The upgrade hint reads `[Shroudfire] Type upgrade to learn about them`.

Discord messages use the configured invite and an independent timer. By default, they reach all connected human players every 20 minutes while the mode is enabled. Chat containing `discord` sends the invite and restarts the interval. An empty link or disabled announcement setting suppresses both paths. The timer continues across map changes; link and interval changes take effect immediately.

### HUD

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_hud` | `1` | `0`–`1` (int) | Show role, level, XP and queue position in the round-start center panel. |
| `sf_round_info_seconds` | `10` | `0`–`120` (int) | Seconds to show the round-start role, level, XP and queue panel. Set to 0 to disable. |
| `sf_hud_kill_seconds` | `5` | `0`–`120` (float) | Seconds to show each kill XP line, including post-round kills; 0 hides these lines. |
| `sf_hud_assist_seconds` | `5` | `0`–`120` (float) | Seconds to show each assist XP line; 0 hides these lines without changing XP awards. |
| `sf_hud_round_result_seconds` | `10` | `0`–`120` (float) | Seconds to show the round win or draw title; 0 hides the title. |
| `sf_hud_round_win_xp_seconds` | `10` | `0`–`120` (float) | Seconds to show round-win XP; 0 hides this line without changing XP awards. |
| `sf_hud_levelup_seconds` | `5` | `0`–`120` (float) | Seconds to show a level-up line; 0 hides these lines. |
| `sf_hud_priority_seconds` | `5` | `0`–`120` (float) | Seconds to show earned Invisant queue priority; 0 hides this line without changing priority. |
| `sf_hud_no_rewards_seconds` | `5` | `0`–`120` (float) | Seconds to show No rewards after round end following a post-round kill; 0 hides this line. |
| `sf_timeout_warning_seconds` | `10` | `0`–`120` (int) | Seconds before timeout when the Invisant fade warning and reveal voice begin; 0 disables both. |

Automatic notices share the HUD; open menus and guides take priority. Duration settings affect display time without changing rewards or game rules. Ward uses red, Surge uses blue, countdown numbers use white, and remaining notice text uses orange. A cooldown attempt or newly ready ability displays its notice for five seconds.

### Visibility, map effects, and observation

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_invisant_see_teammates` | `1` | `0`–`1` (int) | Allow Invisant players to see and spectate Invisant teammates. |
| `sf_wallhack` | `1` | `0`–`1` (int) | Show living Armitive with through-wall outlines to living Invisant players. |
| `sf_ward_screen_tint` | `85` | `0`–`255` (int) | Green screen tint strength while Ward is active. Set to 0 to disable; 255 is maximum. |
| `sf_extra_spawns` | `1` | `0`–`1` (int) | Create validated additional team spawns on Windows and Linux when the map does not have enough for the maximum roster. |
| `sf_blood_amount` | `4` | `0`–`6` (int) | Number of additional blood-splatter bursts when an Armitive damages an Invisant or an Invisant knife damages an Armitive. Set to 0 for normal blood only. |
| `sf_tonemap_exposure` | `0.4` | `0.05`–`1` (float) | Attempts to set HDR exposure on the map's tonemap controller. Stock de_dust2 lighting is baked and unaffected. |
| `sf_admin_spectate` | `1` | `0`–`1` (int) | Allow administrators with the generic flag to observe players or use a spectator free camera. |

Ordinary dead players observe living teammates. Neutral spectators have no ordinary target; disabling Invisant teammate visibility also blocks ordinary teammate observation for that role. Administrator observation is a logged exception. Tonemap exposure changes do not rebake map lighting.

### Custom voices

| Setting | Default | Allowed range | Description |
| --- | --- | --- | --- |
| `sf_custom_sounds` | `0` | `0`–`1` (int) | Play Shroudfire voice clips when its CS2 Workshop sound pack is mounted for clients. |
| `sf_silent_local_movement` | `0` | `0`–`1` (int) | Experimentally block an Invisant's own footsteps, jumps, and landings using the updated Workshop pack. |
| `sf_round_win_sound_delay` | `0.5` | `0`–`10` (float) | Delay in seconds before playing an Armitive win, Invisant win, or round draw voice clip. |

Mount the Workshop pack on the server and clients through MultiAddonManager before enabling its features. The supplied gameplay config enables local movement silencing; custom voices remain off until `sf_custom_sounds 1`. Players can mute individual voice categories with `audio`. See [the Workshop guide](sound-pack/README.md) for downloads and [local movement behavior](sound-pack/README.md#local-movement-silencing).
