#include "plugin.h"
#include "chat_hints.h"
#include "kill_voice_hashes.h"
#include "round_start_voice_hashes.h"
#include <irecipientfilter.h>

#include <eiface.h>
#include <interfaces/interfaces.h>
#include <schemasystem/schemasystem.h>
#include <engine/igameeventsystem.h>
#include <entity2/entityinstance.h>
#include <networksystem/inetworkmessages.h>
#include <networksystem/netmessage.h>
#include "gameevents.pb.h"
#include "usermessages.pb.h"
#include "cstrike15_usermessages.pb.h"
#include "cs_usercmd.pb.h"
#include <tier1/convar.h>
#include <const.h>
#include <in_buttons.h>

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>

ShroudfirePlugin g_shroudfire_plugin;
PLUGIN_EXPOSE(ShroudfirePlugin, g_shroudfire_plugin);

namespace {
#ifdef _WIN32
constexpr size_t kPlayerRunCommandIndex = 25;
#else
constexpr size_t kPlayerRunCommandIndex = 26;
#endif
constexpr char kChatDefault = '\x01';
constexpr char kChatValue = '\x06';
constexpr char kChatBlue = '\x0B';
constexpr char kChatRed = '\x07';
constexpr char kChatGold = '\x09';
constexpr char kChatPrefix[] = " \x04[Shroudfire]\x01 ";

bool stock_kill_voice_name(const std::string &name) {
    return name.find("i_killed") != std::string::npos ||
        name.find("sees_enemy_killed") != std::string::npos ||
        name.find("sees_friend_killed") != std::string::npos ||
        name.find("sees_sniper_killed") != std::string::npos ||
        name.find("enemydown") != std::string::npos ||
        name.find("sniperkilled") != std::string::npos ||
        name.find("killedfriend") != std::string::npos ||
        name.find("radiobotkill") != std::string::npos ||
        name.find("radiobotcheer") != std::string::npos ||
        name.find("brag") != std::string::npos ||
        name.find("clearedarea") != std::string::npos ||
        name.find("round_won") != std::string::npos ||
        name.find("round_winning") != std::string::npos ||
        name.find("round_lost") != std::string::npos ||
        name.find("round_losing") != std::string::npos ||
        name.find("victory") != std::string::npos ||
        name.find("radio.clear") != std::string::npos ||
        name.find(".clear") != std::string::npos ||
        name.find(".cheer") != std::string::npos ||
        name.find(".taunt") != std::string::npos ||
        (name.find("vo/agents/") != std::string::npos &&
            (name.find("cheer") != std::string::npos ||
             name.find("taunt") != std::string::npos));
}

std::string escape_player_name(const std::string &name) {
    std::string escaped;
    for (unsigned char c : name) {
        if (c == '&') escaped += "&amp;";
        else if (c == '<') escaped += "&lt;";
        else if (c == '>') escaped += "&gt;";
        else if (c >= ' ') escaped += static_cast<char>(c);
    }
    return escaped.empty() ? "player" : escaped;
}
constexpr char kGuideJoinHint[] = " \x04[Shroudfire]\x01 New? Type "
    "\x09" "help\x01 to learn the game";
constexpr char kCheatChatHint[] = " \x04[Shroudfire]\x01 Type "
    "\x09" "cheat\x01 to learn why cheating is impossible";
constexpr char kAudioChatHint[] = " \x04[Shroudfire]\x01 Type "
    "\x09" "audio\x01 to toggle voice lines on or off";
constexpr char kUpgradeChatHint[] = " \x04[Shroudfire]\x01 Type "
    "\x09" "upgrade\x01 to learn about them";
constexpr char kBugsChatHint[] = " \x04[Shroudfire]\x01 Type "
    "\x09" "bugs\x01 to learn about footsteps and reloads";
constexpr char kMenuChatHint[] = " \x04[Shroudfire]\x01 Type "
    "\x09" "menu\x01 to choose your upgrades";
constexpr char kResetChatHint[] = " \x04[Shroudfire]\x01 Type "
    "\x09" "reset\x01 to reset your upgrades";
constexpr char kRankChatHint[] = " \x04[Shroudfire]\x01 Type "
    "\x09" "rank\x01 to see your rank";
constexpr std::array<const char *, 6> kArmitiveRoundStartSounds {{
    "sounds/shroudfire/face_the_invisant_fear_nothing.vsnd",
    "sounds/shroudfire/the_invisant_wants_blood_spill_its_own.vsnd",
    "sounds/shroudfire/the_invisant_thinks_its_safe_prove_it_wrong.vsnd",
    "sounds/shroudfire/the_invisant_can_bleed_make_it.vsnd",
    "sounds/shroudfire/the_invisant_needs_an_opening_give_it_none.vsnd",
    "sounds/shroudfire/the_invisant_is_closing_in_stand_your_ground.vsnd"
}};
constexpr std::array<const char *, 6> kInvisantRoundStartSounds {{
    "sounds/shroudfire/find_the_gap_take_the_kill.vsnd",
    "sounds/shroudfire/they_cannot_see_you_make_them_afraid.vsnd",
    "sounds/shroudfire/the_hunt_begins_leave_no_surviors.vsnd",
    "sounds/shroudfire/get_close_strike_fast.vsnd",
    "sounds/shroudfire/they_are_armed_they_are_afraid.vsnd",
    "sounds/shroudfire/choose_one_leave_none.vsnd"
}};
constexpr std::array<const char *, 5> kInvisantRevealWarningSounds {{
    "sounds/shroudfire/time_is_running_out_strike.vsnd",
    "sounds/shroudfire/your_invisibility_is_failing_end_this.vsnd",
    "sounds/shroudfire/the_veil_is_breaking_finish_the_hunt.vsnd",
    "sounds/shroudfire/the_shroud_is_fading_kill_them_now.vsnd",
    "sounds/shroudfire/your_advantage_is_fading_waste_no_time.vsnd"
}};
constexpr std::array<const char *, 5> kInvisantStarvationSounds {{
    "sounds/shroudfire/the_hunger_is_here_find_them.vsnd",
    "sounds/shroudfire/your_strength_is_fading_hunt.vsnd",
    "sounds/shroudfire/the_shroud_demands_blood_take_it.vsnd",
    "sounds/shroudfire/you_are_starving_make_the_next_strike_count.vsnd",
    "sounds/shroudfire/every_second_weakens_you_strike.vsnd"
}};
constexpr std::array<const char *, 5> kAfkWarningSounds {{
    "sounds/shroudfire/wake_up_take_a_step.vsnd",
    "sounds/shroudfire/breaks_over_come_back.vsnd",
    "sounds/shroudfire/eyes_up_youre_still_in_the_game.vsnd",
    "sounds/shroudfire/the_fight_continues_with_or_without_you.vsnd",
    "sounds/shroudfire/still_there_not_for_much_longer.vsnd"
}};
constexpr std::array<const char *, 5> kArmitiveCampingSounds {{
    "sounds/shroudfire/you_have_stayed_here_too_long_move.vsnd",
    "sounds/shroudfire/stay_there_and_this_place_becomes_your_grave.vsnd",
    "sounds/shroudfire/move_now_or_be_removed.vsnd",
    "sounds/shroudfire/keep_moving_death_is_closing_in.vsnd",
    "sounds/shroudfire/you_have_lingered_long_enough_go.vsnd"
}};
constexpr std::array<const char *, 10> kKillStreakSounds {{
    "sounds/shroudfire/first_blood.vsnd",
    "sounds/shroudfire/double_kill.vsnd",
    "sounds/shroudfire/killing_spree.vsnd",
    "sounds/shroudfire/ultra_kill.vsnd",
    "sounds/shroudfire/rampage.vsnd",
    "sounds/shroudfire/dominating.vsnd",
    "sounds/shroudfire/unstoppable.vsnd",
    "sounds/shroudfire/monster_kill.vsnd",
    "sounds/shroudfire/godlike.vsnd",
    "sounds/shroudfire/holy_shit.vsnd"
}};
constexpr std::array<const char *, 3> kArmitiveWinSounds {{
    "sounds/shroudfire/armitive_win.vsnd",
    "sounds/shroudfire/armitive_win2.vsnd",
    "sounds/shroudfire/armitive_win3.vsnd"
}};
constexpr std::array<const char *, 3> kInvisantWinSounds {{
    "sounds/shroudfire/invisant_win.vsnd",
    "sounds/shroudfire/invisant_win2.vsnd",
    "sounds/shroudfire/invisant_win3.vsnd"
}};
constexpr std::array<const char *, 3> kRoundDrawSounds {{
    "sounds/shroudfire/round_draw.vsnd",
    "sounds/shroudfire/round_draw2.vsnd",
    "sounds/shroudfire/round_draw3.vsnd"
}};
constexpr std::array<const char *, 16> kUpgradeNames {{
    "Lightweight", "Momentum", "Resilience", "Reforging", "Vampirism", "Bloodlust",
    "Mobility", "Surge", "Fortitude", "Recovery", "Overload", "Stockpile",
    "Barrage", "Control", "Dexterity", "Ward"
}};
constexpr std::array<const char *, 3> kLocalMovementBlockerEvents {{
    "shroudfire.silence_movement_steps",
    "shroudfire.silence_movement_jump",
    "shroudfire.silence_movement_land"
}};
constexpr std::array<const char *, 13> kHelpPages {{
    "<font color='#7fa7ff'><b>INVISANT</b></font><br>"
    "<font color='#7fa7ff'>Invis</font><font color='#ffb900'>ible assail</font><font color='#7fa7ff'>ant</font><font color='#ffb900'>.</font><br>"
    "<font color='#e07a6a'>Armitive</font><font color='#ffb900'> cannot hear your</font><br>"
    "<font color='#ffb900'>footsteps. Kill all </font><font color='#e07a6a'>Armitive</font><font color='#ffb900'> to</font><br>"
    "<font color='#ffb900'>win. Track living </font><font color='#e07a6a'>Armitive</font><br>"
    "<font color='#ffb900'>through walls. No fall damage.</font>",

    "<font color='#7fa7ff'><b>INVISANT</b></font><br>"
    "<font color='#ffb900'>Knife-only. </font><font color='#93c47d'>{KNIFE_CONE}</font><font color='#ffb900'> deg cone.</font><br>"
    "<font color='#93c47d'>{KNIFE_RANGE}</font><font color='#ffb900'> units of reach.</font><br>"
    "<font color='#93c47d'>{KNIFE_DAMAGE_MIN}-{KNIFE_DAMAGE_MAX}</font><font color='#ffb900'> damage per hit.</font><br>"
    "<font color='#ffb900'>No backstab bonus. Attacks at</font><br>"
    "<font color='#93c47d'>{KNIFE_SPEED}%</font><font color='#ffb900'> of normal speed.</font>",

    "<font color='#7fa7ff'><b>INVISANT</b></font><br>"
    "<font color='#ffb900'>After </font><font color='#93c47d'>{STARVATION_ROUND_DELAY}s</font><font color='#ffb900'> of live play, </font><font color='#93c47d'>{STARVATION_COMBAT_DELAY}s</font><br>"
    "<font color='#ffb900'>without dealing or taking</font><br>"
    "<font color='#ffb900'>damage starts </font><font color='#b9a176'>Starvation</font><font color='#ffb900'>.</font><br>"
    "<font color='#93c47d'>{STARVATION_DAMAGE}</font><font color='#ffb900'> HP / </font><font color='#93c47d'>{STARVATION_INTERVAL}s</font><font color='#ffb900'>; after </font><font color='#93c47d'>{STARVATION_ACCELERATION_DELAY}s</font><font color='#ffb900'>:</font><br>"
    "<font color='#93c47d'>{STARVATION_ACCELERATED_DAMAGE}</font><font color='#ffb900'> HP / </font><font color='#93c47d'>{STARVATION_ACCELERATED_INTERVAL}s</font><font color='#ffb900'> until combat.</font>",

    "<font color='#e07a6a'><b>ARMITIVE</b></font><br>"
    "<font color='#e07a6a'>Arm</font><font color='#ffb900'>ed fug</font><font color='#e07a6a'>itive</font><font color='#ffb900'>. Find the</font><br>"
    "<font color='#7fa7ff'>Invisant</font><font color='#ffb900'> with firearms, blood,</font><br>"
    "<font color='#ffb900'>and contact. Buy weapons</font><br>"
    "<font color='#ffb900'>permitted by this server.</font><br>"
    "<font color='#ffb900'>Win if any survive the timer.</font>",

    "<font color='#e07a6a'><b>ARMITIVE</b></font><br>"
    "<font color='#ffb900'>Headshot damage to </font><font color='#7fa7ff'>Invisant</font><font color='#ffb900'> is</font><br>"
    "<font color='#ffb900'>halved and capped at </font><font color='#93c47d'>90</font><font color='#ffb900'>. One</font><br>"
    "<font color='#ffb900'>headshot cannot kill a</font><br>"
    "<font color='#ffb900'>full-health </font><font color='#7fa7ff'>Invisant</font><font color='#ffb900'>.</font><br>"
    "<font color='#ffb900'>Timer ends: </font><font color='#7fa7ff'>Invisant</font><font color='#ffb900'> {TIMEOUT_RESULT}</font>",

    "<font color='#93c47d'><b>ROUND END</b></font><br>"
    "<font color='#ffb900'>The round result is final.</font><br>"
    "<font color='#ffb900'>Kills after round end give no</font><br>"
    "<font color='#ffb900'>XP, rank, or </font><font color='#7fa7ff'>Invisant</font><font color='#ffb900'> priority.</font><br>"
    "<font color='#ffb900'>Keep earned priority and wait</font><br>"
    "<font color='#ffb900'>for the next round.</font>",

    "<font color='#93c47d'><b>ROTATION</b></font><br>"
    "<font color='#ffb900'>Roles rotate each round. A</font><br>"
    "<font color='#e07a6a'>Armitive</font><font color='#ffb900'> who kills an </font><font color='#7fa7ff'>Invisant</font><br>"
    "<font color='#ffb900'>gets priority to become </font><font color='#7fa7ff'>Invisant</font><br>"
    "<font color='#ffb900'>next round.</font>",

    "<font color='#93c47d'><b>ROTATION</b></font><br>"
    "<font color='#ffb900'>Otherwise, the longest-waiting</font><br>"
    "<font color='#ffb900'>eligible player becomes</font><br>"
    "<font color='#7fa7ff'>Invisant</font><font color='#ffb900'>. Suicide forfeits</font><br>"
    "<font color='#ffb900'>next-round eligibility.</font>",

    "<font color='#93c47d'><b>RANKINGS</b></font><br>"
    "<font color='#ffb900'>Type </font><font color='#b9a176'>rank</font><font color='#ffb900'> in chat to see your</font><br>"
    "<font color='#7fa7ff'>Invisant</font><font color='#ffb900'> and </font><font color='#e07a6a'>Armitive</font><font color='#ffb900'> ranks.</font><br>"
    "<font color='#ffb900'>Kills and assists raise ratings;</font><br>"
    "<font color='#ffb900'>deaths lower them. More combat</font><br>"
    "<font color='#ffb900'>makes your record count more.</font>",

    "<font color='#93c47d'><b>PROGRESSION</b></font><br>"
    "<font color='#ffb900'>Kills, qualifying damage</font><br>"
    "<font color='#ffb900'>assists, and completed-round</font><br>"
    "<font color='#ffb900'>wins grant separate </font><font color='#7fa7ff'>Invisant</font><font color='#ffb900'> and</font><br>"
    "<font color='#e07a6a'>Armitive</font><font color='#ffb900'> XP.</font>",

    "<font color='#93c47d'><b>PROGRESSION</b></font><br>"
    "<font color='#ffb900'>As </font><font color='#e07a6a'>Armitive</font><font color='#ffb900'>, knife kills earn </font><font color='#93c47d'>{KNIFE_XP}%</font><br>"
    "<font color='#ffb900'>kill XP; pistol kills, </font><font color='#93c47d'>{PISTOL_XP}%</font><font color='#ffb900'>.</font><br>"
    "<font color='#ffb900'>Levels grant upgrade points. In</font><br>"
    "<font color='#ffb900'>chat, type </font><font color='#b9a176'>upgrade</font><font color='#ffb900'> for effects;</font><br>"
    "<font color='#b9a176'>menu</font><font color='#ffb900'> to spend or reset points.</font>",

    "<font color='#93c47d'><b>KILL STREAKS</b></font><br>"
    "<font color='#ffb900'>In 2v2+ rounds, the first enemy</font><br>"
    "<font color='#ffb900'>kill earns </font><font color='#b9a176'>First Blood</font><font color='#ffb900'>: </font><font color='#93c47d'>125%</font><br>"
    "<font color='#ffb900'>kill XP. Your second kill within</font><br>"
    "<font color='#93c47d'>10s</font><font color='#ffb900'> of the last earns </font><font color='#93c47d'>120%</font><font color='#ffb900'>.</font>",

    "<font color='#93c47d'><b>KILL STREAKS</b></font><br>"
    "<font color='#ffb900'>Each later kill within </font><font color='#93c47d'>10s</font><font color='#ffb900'> of</font><br>"
    "<font color='#ffb900'>your last adds </font><font color='#93c47d'>10%</font><font color='#ffb900'> kill XP, up</font><br>"
    "<font color='#ffb900'>to </font><font color='#93c47d'>200%</font><font color='#ffb900'> on your tenth kill. Only</font><br>"
    "<font color='#ffb900'>you hear the sounds.</font><br>"
    "<font color='#ffb900'>Type </font><font color='#b9a176'>help</font><font color='#ffb900'> to reopen this panel.</font>"
}};
constexpr std::array<const char *, 3> kCheatPages {{
    "<font color='#93c47d'><b>ESP AND WALLHACKS</b></font><br>"
    "<font color='#7fa7ff'>Invisant</font><font color='#ffb900'> are not made</font><br>"
    "<font color='#ffb900'>transparent. The server does not</font><br>"
    "<font color='#ffb900'>send </font><font color='#7fa7ff'>Invisant</font><font color='#ffb900'> players, or their</font><br>"
    "<font color='#ffb900'>weapon and movement sounds, to</font><br>"
    "<font color='#e07a6a'>Armitive</font><font color='#ffb900'> computers.</font>",

    "<font color='#93c47d'><b>ESP AND WALLHACKS</b></font><br>"
    "<font color='#ffb900'>ESP and wallhacks can only show</font><br>"
    "<font color='#ffb900'>data a computer receives. With</font><br>"
    "<font color='#ffb900'>no </font><font color='#7fa7ff'>Invisant</font><font color='#ffb900'> player data, they</font><br>"
    "<font color='#ffb900'>cannot reveal or track an</font><br>"
    "<font color='#7fa7ff'>Invisant</font><font color='#ffb900'>.</font>",

    "<font color='#93c47d'><b>ESP AND WALLHACKS</b></font><br>"
    "<font color='#e07a6a'>Armitive</font><font color='#ffb900'> still see blood and</font><br>"
    "<font color='#ffb900'>bullet impacts, and hear damage</font><br>"
    "<font color='#ffb900'>sounds. </font><font color='#d6b86a'>Report suspicious</font><br>"
    "<font color='#d6b86a'>behavior to an administrator.</font>"
}};

constexpr std::array<const char *, 2> kKnownBugPages {{
    "<font color='#78b9ff'><b>INVISANT AUDIO</b></font><br>"
    "<font color='#ffb900'>You still hear your footsteps,</font><br>"
    "<font color='#ffb900'>jumps and landings. CS2 plays</font><br>"
    "<font color='#ffb900'>these locally. The server filters</font><br>"
    "<font color='#ffb900'>them before sending to Armitive.</font>",

    "<font color='#ff7169'><b>DEXTERITY ANIMATIONS</b></font><br>"
    "<font color='#ffb900'>Reload and weapon switch</font><br>"
    "<font color='#ffb900'>animations may look unchanged.</font><br>"
    "<font color='#ffb900'>Dexterity still makes ammo ready</font><br>"
    "<font color='#ffb900'>and lets you fire sooner.</font>"
}};

constexpr std::array<const char *, 9> kUpgradePages {{
    "<font color='#ffb900'>Normal stages unlock at role</font><br>"
    "<font color='#ffb900'>levels </font><font color='#93c47d'>{N1}</font><font color='#ffb900'>, </font><font color='#93c47d'>{N2}</font><font color='#ffb900'>, </font><font color='#93c47d'>{N3}</font><font color='#ffb900'>, </font><font color='#93c47d'>{N4}</font><font color='#ffb900'> and </font><font color='#93c47d'>{N5}</font><br>"
    "<font color='#ffb900'>Ultimate stages unlock at role</font><br>"
    "<font color='#ffb900'>levels </font><font color='#93c47d'>{U1}</font><font color='#ffb900'>, </font><font color='#93c47d'>{U2}</font><font color='#ffb900'>, </font><font color='#93c47d'>{U3}</font><font color='#ffb900'>, </font><font color='#93c47d'>{U4}</font><font color='#ffb900'> and </font><font color='#93c47d'>{U5}</font><br>"
    "<font color='#ffb900'>Type </font><font color='#b9a176'>menu</font><font color='#ffb900'> to spend/reset points</font>",

    "<font color='#e07a6a'><b>ARMITIVE</b></font><br>"
    "<font color='#e07a6a'>Fortitude</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{FORTITUDE}</font><font color='#ffb900'> maximum</font><br>"
    "<font color='#ffb900'>health per stage</font><br>"
    "<font color='#e07a6a'>Recovery</font><font color='#ffb900'> - Restore </font><font color='#93c47d'>{RECOVERY}</font><font color='#ffb900'> health</font><br>"
    "<font color='#ffb900'>per stage every </font><font color='#93c47d'>{RECOVERY_INTERVAL}</font><font color='#ffb900'> seconds</font>",

    "<font color='#e07a6a'><b>ARMITIVE</b></font><br>"
    "<font color='#e07a6a'>Overload</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{OVERLOAD}%</font><font color='#ffb900'> extra</font><br>"
    "<font color='#ffb900'>magazine capacity per stage</font><br>"
    "<font color='#e07a6a'>Stockpile</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{STOCKPILE}</font><font color='#ffb900'> extra</font><br>"
    "<font color='#ffb900'>reserve ammo clip per stage</font>",

    "<font color='#e07a6a'><b>ARMITIVE</b></font><br>"
    "<font color='#e07a6a'>Barrage</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{BARRAGE}%</font><font color='#ffb900'> faster</font><br>"
    "<font color='#ffb900'>weapon fire rate per stage</font><br>"
    "<font color='#e07a6a'>Control</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{CONTROL}%</font><font color='#ffb900'> less recoil</font><br>"
    "<font color='#ffb900'>and bullet spread per stage</font>",

    "<font color='#e07a6a'><b>ARMITIVE</b></font><br>"
    "<font color='#e07a6a'>Dexterity</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{DEXTERITY}%</font><font color='#ffb900'> faster</font><br>"
    "<font color='#ffb900'>weapon switch and reload</font><br>"
    "<font color='#e07a6a'>Ultimate: Ward</font><font color='#ffb900'> - Press </font><font color='#93c47d'>E</font><font color='#ffb900'> for</font><br>"
    "<font color='#93c47d'>{WARD_BASE}</font><font color='#ffb900'> seconds of immunity at </font><font color='#93c47d'>1</font><font color='#ffb900'>/</font><font color='#93c47d'>5</font><font color='#ffb900'>,</font><br>"
    "<font color='#ffb900'>plus </font><font color='#93c47d'>{WARD_STEP}</font><font color='#ffb900'> seconds per stage</font>",

    "<font color='#7fa7ff'><b>INVISANT</b></font><br>"
    "<font color='#7fa7ff'>Lightweight</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{LIGHTWEIGHT}%</font><font color='#ffb900'> less</font><br>"
    "<font color='#ffb900'>gravity per stage</font><br>"
    "<font color='#7fa7ff'>Momentum</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{MOMENTUM}%</font><font color='#ffb900'> faster</font><br>"
    "<font color='#ffb900'>run speed per stage</font>",

    "<font color='#7fa7ff'><b>INVISANT</b></font><br>"
    "<font color='#7fa7ff'>Resilience</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{RESILIENCE}</font><font color='#ffb900'> armor</font><br>"
    "<font color='#ffb900'>per stage and helmet at </font><font color='#ffb900'>{HELMET_STAGE}/5</font><br>"
    "<font color='#7fa7ff'>Reforging</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{REFORGING}</font><font color='#ffb900'> armor per</font><br>"
    "<font color='#ffb900'>stage every </font><font color='#93c47d'>{REFORGING_INTERVAL}</font><font color='#ffb900'> seconds</font>",

    "<font color='#7fa7ff'><b>INVISANT</b></font><br>"
    "<font color='#7fa7ff'>Vampirism</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{VAMPIRISM}</font><font color='#ffb900'> health per</font><br>"
    "<font color='#ffb900'>stage when dealing damage</font><br>"
    "<font color='#7fa7ff'>Bloodlust</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{BLOODLUST}</font><font color='#ffb900'> health per</font><br>"
    "<font color='#ffb900'>stage when killing an enemy</font>",

    "<font color='#7fa7ff'><b>INVISANT</b></font><br>"
    "<font color='#7fa7ff'>Mobility</font><font color='#ffb900'> - Gain </font><font color='#93c47d'>{MOBILITY}%</font><font color='#ffb900'> faster</font><br>"
    "<font color='#ffb900'>crouching speed per stage</font><br>"
    "<font color='#7fa7ff'>Ultimate: Surge</font><font color='#ffb900'> - Press </font><font color='#93c47d'>E</font><font color='#ffb900'> for a</font><br>"
    "<font color='#93c47d'>{SURGE_BASE}</font><font color='#ffb900'> unit speed boost at </font><font color='#93c47d'>1</font><font color='#ffb900'>/</font><font color='#93c47d'>5</font><font color='#ffb900'>,</font><br>"
    "<font color='#ffb900'>increased by </font><font color='#93c47d'>{SURGE_STEP}</font><font color='#ffb900'> per stage</font>"
}};
struct SoundRecipientsRestore {
    IRecipientFilter *filter;
    uint64_t removed;
};
thread_local std::vector<SoundRecipientsRestore> sound_recipients_restore;
struct MovementFrameRestore { CGlobalVars *globals; float frametime; };
thread_local std::vector<MovementFrameRestore> movement_frametimes_restore;
struct ServerUserCommandBase {
    virtual ~ServerUserCommandBase();
    int command_number;
    uint8_t unknown[4];
};
struct ServerUserCommand : ServerUserCommandBase, CSGOUserCmdPB {
    void *button_vtable;
    uint64_t button_states[3];
    uint32_t unknown;
    uint32_t flags;
    void *unknown_command;
    void *parent_command;
};
#ifndef _WIN32
static_assert(sizeof(ServerUserCommand) == 144, "CS2 user command layout changed");
#endif
}

static CConVar<int> sf_normal_stage1_level("sf_normal_stage1_level", FCVAR_NONE,
    "Role level required for normal upgrade stage 1.", 1, true, 1, true, 40);
static CConVar<int> sf_normal_stage2_level("sf_normal_stage2_level", FCVAR_NONE,
    "Role level required for normal upgrade stage 2.", 5, true, 1, true, 40);
static CConVar<int> sf_normal_stage3_level("sf_normal_stage3_level", FCVAR_NONE,
    "Role level required for normal upgrade stage 3.", 10, true, 1, true, 40);
static CConVar<int> sf_normal_stage4_level("sf_normal_stage4_level", FCVAR_NONE,
    "Role level required for normal upgrade stage 4.", 15, true, 1, true, 40);
static CConVar<int> sf_normal_stage5_level("sf_normal_stage5_level", FCVAR_NONE,
    "Role level required for normal upgrade stage 5.", 20, true, 1, true, 40);
static CConVar<int> sf_ultimate_stage1_level("sf_ultimate_stage1_level", FCVAR_NONE,
    "Role level required for ultimate upgrade stage 1.", 8, true, 1, true, 40);
static CConVar<int> sf_ultimate_stage2_level("sf_ultimate_stage2_level", FCVAR_NONE,
    "Role level required for ultimate upgrade stage 2.", 16, true, 1, true, 40);
static CConVar<int> sf_ultimate_stage3_level("sf_ultimate_stage3_level", FCVAR_NONE,
    "Role level required for ultimate upgrade stage 3.", 24, true, 1, true, 40);
static CConVar<int> sf_ultimate_stage4_level("sf_ultimate_stage4_level", FCVAR_NONE,
    "Role level required for ultimate upgrade stage 4.", 32, true, 1, true, 40);
static CConVar<int> sf_ultimate_stage5_level("sf_ultimate_stage5_level", FCVAR_NONE,
    "Role level required for ultimate upgrade stage 5.", 40, true, 1, true, 40);
static CConVar<int> sf_invisant_base_armor("sf_invisant_base_armor", FCVAR_NONE,
    "Invisant armor before Resilience.", 25, true, 0, true, 200);
static CConVar<int> sf_knife_damage_min("sf_knife_damage_min", FCVAR_NONE,
    "Minimum Invisant knife damage per hit, inclusive.", 50, true, 1, true, 1000);
static CConVar<int> sf_knife_damage_max("sf_knife_damage_max", FCVAR_NONE,
    "Maximum Invisant knife damage per hit, inclusive.", 75, true, 1, true, 1000);
static CConVar<CUtlString> sf_armitive_blocked_weapons("sf_armitive_blocked_weapons", FCVAR_NONE,
    "Comma-separated weapons Armitive cannot buy; empty allows all weapons.", CUtlString("m249"));
static CConVar<float> sf_knife_range("sf_knife_range", FCVAR_NONE,
    "Maximum Invisant knife reach in units; 0 disables extended hits.",
    72.0f, true, 0.0f, true, 16384.0f);
static CConVar<float> sf_knife_cone("sf_knife_cone", FCVAR_NONE,
    "Full Invisant knife cone width in degrees; 360 covers every direction.",
    60.0f, true, 0.0f, true, 360.0f);
static CConVar<int> sf_knife_attack_speed_percent("sf_knife_attack_speed_percent", FCVAR_NONE,
    "Invisant knife attack speed as a percent of normal; 100 restores the normal rate.",
    66, true, 1, true, 100);
static CConVar<int> sf_knife_miss_sound("sf_knife_miss_sound", FCVAR_NONE,
    "Replay Invisant knife air-miss sounds spatially to Armitive players.",
    1, true, 0, true, 1);
static CConVar<int> sf_knife_world_hit_sound("sf_knife_world_hit_sound", FCVAR_NONE,
    "Replay Invisant knife wall and ground impact sounds spatially to Armitive players.",
    1, true, 0, true, 1);
static CConVar<int> sf_lightweight_gravity_percent("sf_lightweight_gravity_percent", FCVAR_NONE,
    "Gravity reduction percent per Lightweight stage.", 10, true, 0, true, 20);
static CConVar<int> sf_momentum_run_percent("sf_momentum_run_percent", FCVAR_NONE,
    "Run speed increase percent per Momentum stage.", 3, true, 0, true, 100);
static CConVar<int> sf_resilience_armor_per_stage("sf_resilience_armor_per_stage", FCVAR_NONE,
    "Maximum armor gained per Resilience stage.", 15, true, 0, true, 100);
static CConVar<int> sf_resilience_helmet_stage("sf_resilience_helmet_stage", FCVAR_NONE,
    "Resilience stage that grants a helmet.", 5, true, 1, true, 5);
static CConVar<int> sf_reforging_armor_per_stage("sf_reforging_armor_per_stage", FCVAR_NONE,
    "Armor regenerated per Reforging stage each interval.", 1, true, 0, true, 100);
static CConVar<int> sf_reforging_interval("sf_reforging_interval", FCVAR_NONE,
    "Seconds between Reforging armor ticks and after damage.", 3, true, 1, true, 60);
static CConVar<int> sf_vampirism_health_per_stage("sf_vampirism_health_per_stage", FCVAR_NONE,
    "Health restored per Vampirism stage on a damaging hit.", 3, true, 0, true, 100);
static CConVar<int> sf_bloodlust_health_per_stage("sf_bloodlust_health_per_stage", FCVAR_NONE,
    "Health restored per Bloodlust stage on a kill.", 6, true, 0, true, 100);
static CConVar<int> sf_mobility_crouch_percent("sf_mobility_crouch_percent", FCVAR_NONE,
    "Crouch speed increase percent per Mobility stage.", 15, true, 0, true, 100);
static CConVar<int> sf_surge_base_speed("sf_surge_base_speed", FCVAR_NONE,
    "Surge speed boost at stage 1. Default: 480.", 480, true, 0, true, 2000);
static CConVar<int> sf_surge_speed_per_stage("sf_surge_speed_per_stage", FCVAR_NONE,
    "Additional Surge speed boost for each stage after stage 1. Default: 30.", 30, true, 0, true, 1000);
static CConVar<int> sf_surge_add_momentum("sf_surge_add_momentum", FCVAR_NONE,
    "Whether Surge adds to current velocity (1) or replaces it (0). Default: 0.", 0, true, 0, true, 1);
static CConVar<int> sf_surge_cooldown("sf_surge_cooldown", FCVAR_NONE,
    "Seconds until Surge is ready after use.", 20, true, 0, true, 300);
static CConVar<int> sf_armitive_base_health("sf_armitive_base_health", FCVAR_NONE,
    "Armitive health before Fortitude.", 100, true, 1, true, 500);
static CConVar<int> sf_fortitude_health_per_stage("sf_fortitude_health_per_stage", FCVAR_NONE,
    "Maximum health gained per Fortitude stage.", 10, true, 0, true, 100);
static CConVar<int> sf_recovery_health_per_stage("sf_recovery_health_per_stage", FCVAR_NONE,
    "Health regenerated per Recovery stage each interval.", 1, true, 0, true, 100);
static CConVar<int> sf_recovery_interval("sf_recovery_interval", FCVAR_NONE,
    "Seconds between Recovery health ticks and after damage.", 3, true, 1, true, 60);
static CConVar<int> sf_overload_magazine_percent("sf_overload_magazine_percent", FCVAR_NONE,
    "Magazine capacity increase percent per Overload stage.", 10, true, 0, true, 50);
static CConVar<int> sf_stockpile_clips_per_stage("sf_stockpile_clips_per_stage", FCVAR_NONE,
    "Extra reserve clips per Stockpile stage.", 1, true, 0, true, 5);
static CConVar<int> sf_barrage_fire_rate_percent("sf_barrage_fire_rate_percent", FCVAR_NONE,
    "Weapon fire rate increase percent per Barrage stage.", 7, true, 0, true, 100);
static CConVar<int> sf_control_reduction_percent("sf_control_reduction_percent", FCVAR_NONE,
    "Recoil and spread reduction percent per Control stage.", 10, true, 0, true, 20);
static CConVar<int> sf_dexterity_speed_percent("sf_dexterity_speed_percent", FCVAR_NONE,
    "Reload and weapon switch time reduction percent per Dexterity stage.", 12, true, 0, true, 19);
static CConVar<float> sf_ward_base_seconds("sf_ward_base_seconds", FCVAR_NONE,
    "Ward immunity duration at stage 1 in seconds.", 2.0f, true, 0.1f, true, 30.0f);
static CConVar<float> sf_ward_seconds_per_stage("sf_ward_seconds_per_stage", FCVAR_NONE,
    "Additional Ward seconds for each stage after stage 1.", 0.5f, true, 0.0f, true, 10.0f);
static CConVar<int> sf_ward_cooldown("sf_ward_cooldown", FCVAR_NONE,
    "Seconds until Ward is ready after it ends.", 30, true, 0, true, 300);
static CConVar<int> sf_upgrade_purchase_window("sf_upgrade_purchase_window", FCVAR_NONE,
    "Seconds from round start when purchased or reset upgrades apply immediately.",
    20, true, 0, true, 120);
static CConVar<int> sf_enable("sf_enable", FCVAR_NONE,
    "Enables Shroudfire.", 0, true, 0, true, 1);
static CConVar<int> sf_invisant_team("sf_invisant_team", FCVAR_NONE,
    "Selects the Invisant team: 2 for Terrorists or 3 for Counter-Terrorists.", 3, true, 2, true, 3);
static CConVar<int> sf_visible_per_invisant("sf_visible_per_invisant", FCVAR_NONE,
    "Maximum number of Armitive players per Invisant player.", 4, true, 2, true, 32);
static CConVar<int> sf_round_time("sf_round_time", FCVAR_NONE,
    "Seconds before an unfinished round ends with an Armitive win.", 120, true, 30, true, 3600);
static CConVar<int> sf_map_max_rounds("sf_map_max_rounds", FCVAR_NONE,
    "Completed rounds per map before changing maps; 0 disables the round limit.",
    20, true, 0, true, 1000);
static CConVar<float> sf_map_time_limit("sf_map_time_limit", FCVAR_NONE,
    "Minutes per map with at least two active human players before changing maps; 0 disables the time limit.",
    30.0f, true, 0.0f, true, 1440.0f);
static CConVar<CUtlString> sf_map_rotation("sf_map_rotation", FCVAR_NONE,
    "Comma-separated map rotation; unavailable maps are skipped.",
    CUtlString("de_dust2,de_mirage,de_inferno,de_nuke"));
static CConVar<int> sf_timeout_kill_invisant("sf_timeout_kill_invisant", FCVAR_NONE,
    "Kill surviving Invisant at timeout; 0 reveals them during the 10-second round transition instead.", 0, true, 0, true, 1);
static CConVar<float> sf_timeout_reveal_seconds("sf_timeout_reveal_seconds", FCVAR_NONE,
    "Seconds for surviving Invisant to fade from invisible to visible after timeout; 0 reveals instantly.", 2.0f, true, 0.0f, true, 10.0f);
static CConVar<int> sf_map_join_grace("sf_map_join_grace", FCVAR_NONE,
    "Seconds after map start in which joining players can respawn as Armitive.", 30, true, 0, true, 120);
static CConVar<int> sf_afk_seconds("sf_afk_seconds", FCVAR_NONE,
    "Seconds of inactivity after freeze time before a player is killed; 0 disables AFK deaths.",
    30, true, 0, true, 600);
static CConVar<int> sf_afk_warning_seconds("sf_afk_warning_seconds", FCVAR_NONE,
    "Seconds before AFK death to start the HTML countdown and warning sounds; 0 disables the warnings.",
    10, true, 0, true, 600);
static CConVar<int> sf_afk_limit("sf_afk_limit", FCVAR_NONE,
    "Consecutive deaths without live-round movement before a player is kicked; 0 disables kicks.",
    5, true, 0, true, 100);
static CConVar<float> sf_starvation_round_delay("sf_starvation_round_delay", FCVAR_NONE,
    "Seconds of live play after freeze time before Invisant starvation can begin.",
    60.0f, true, 0.0f, true, 3600.0f);
static CConVar<float> sf_starvation_combat_delay("sf_starvation_combat_delay", FCVAR_NONE,
    "Seconds without dealing or taking damage before Invisant starvation begins.",
    45.0f, true, 0.0f, true, 3600.0f);
static CConVar<int> sf_starvation_damage("sf_starvation_damage", FCVAR_NONE,
    "Health an Invisant loses per starvation tick before acceleration.",
    1, true, 1, true, 500);
static CConVar<float> sf_starvation_interval("sf_starvation_interval", FCVAR_NONE,
    "Seconds between Invisant starvation ticks before acceleration.",
    1.0f, true, 0.1f, true, 60.0f);
static CConVar<float> sf_starvation_acceleration_delay("sf_starvation_acceleration_delay", FCVAR_NONE,
    "Seconds of uninterrupted starvation before accelerated ticks begin; 0 accelerates immediately.",
    20.0f, true, 0.0f, true, 3600.0f);
static CConVar<float> sf_starvation_accelerated_interval("sf_starvation_accelerated_interval", FCVAR_NONE,
    "Seconds between Invisant starvation ticks after acceleration.",
    1.0f, true, 0.1f, true, 60.0f);
static CConVar<int> sf_starvation_accelerated_damage("sf_starvation_accelerated_damage", FCVAR_NONE,
    "Health an Invisant loses per starvation tick after acceleration.",
    3, true, 1, true, 500);
static CConVar<float> sf_camping_distance("sf_camping_distance", FCVAR_NONE,
    "Distance in units an Armitive must move from the tracked position before the camping timer resets; 0 disables anti-camping.",
    256.0f, true, 0.0f, true, 4096.0f);
static CConVar<float> sf_camping_warning_seconds("sf_camping_warning_seconds", FCVAR_NONE,
    "Seconds an Armitive may remain within the camping distance before the HTML and voice warning.",
    20.0f, true, 0.0f, true, 600.0f);
static CConVar<float> sf_camping_damage_delay("sf_camping_damage_delay", FCVAR_NONE,
    "Seconds from the Armitive camping warning until health loss begins.",
    10.0f, true, 0.0f, true, 600.0f);
static CConVar<int> sf_camping_damage("sf_camping_damage", FCVAR_NONE,
    "Health an Armitive loses per second while camping.", 10, true, 1, true, 500);
static CConVar<int> sf_queue_reconnect_grace("sf_queue_reconnect_grace", FCVAR_NONE,
    "Seconds a reconnecting player retains queue age; 0 is unlimited.", 600, true, 0, true, 86400);
static CConVar<int> sf_xp_kill_invisant("sf_xp_kill_invisant", FCVAR_NONE,
    "XP for an Invisant killing an Armitive player.", 50, true, 0, true, 10000);
static CConVar<int> sf_xp_kill_armitive("sf_xp_kill_armitive", FCVAR_NONE,
    "XP for an Armitive player killing an Invisant.", 100, true, 0, true, 10000);
static CConVar<int> sf_xp_round_win_invisant("sf_xp_round_win_invisant", FCVAR_NONE,
    "XP for an Invisant participant on a completed win.", 70, true, 0, true, 10000);
static CConVar<int> sf_xp_round_win_armitive("sf_xp_round_win_armitive", FCVAR_NONE,
    "XP for an Armitive participant on a completed win.", 35, true, 0, true, 10000);
static CConVar<int> sf_xp_armitive_pistol_percent("sf_xp_armitive_pistol_percent", FCVAR_NONE,
    "Armitive pistol kill XP as a percent of Armitive base kill XP; 100 removes the bonus.", 125, true, 0, true, 1000);
static CConVar<int> sf_xp_armitive_knife_percent("sf_xp_armitive_knife_percent", FCVAR_NONE,
    "Armitive knife kill XP as a percent of Armitive base kill XP; 100 removes the bonus.", 150, true, 0, true, 1000);
static CConVar<int> sf_xp_level_base("sf_xp_level_base", FCVAR_NONE,
    "Base XP for the level curve.", 100, true, 1, true, 1000000);
static CConVar<float> sf_kill_streak_window("sf_kill_streak_window", FCVAR_NONE,
    "Maximum seconds between a player's consecutive opponent kills for a kill streak.",
    10.0f, true, 1.0f, true, 60.0f);
static CConVar<float> sf_xp_assist_fraction("sf_xp_assist_fraction", FCVAR_NONE,
    "Fraction of the current role's kill XP awarded for a qualifying assist. Set to 0 to disable assist XP.",
    0.25f, true, 0.0f, true, 1.0f);
static CConVar<float> sf_xp_assist_damage_share("sf_xp_assist_damage_share", FCVAR_NONE,
    "Minimum share of a victim's opposing-player damage required for assist XP. 0.25 means 25 percent.",
    0.25f, true, 0.0f, true, 1.0f);
static CConVar<int> sf_xp_first_blood_percent("sf_xp_first_blood_percent", FCVAR_NONE,
    "Kill XP percent for First Blood; multiplies the Armitive knife or pistol bonus when applicable.", 125, true, 0, true, 1000);
static CConVar<int> sf_xp_double_kill_percent("sf_xp_double_kill_percent", FCVAR_NONE,
    "Kill XP percent for Double Kill; multiplies the Armitive knife or pistol bonus when applicable.", 120, true, 0, true, 1000);
static CConVar<int> sf_xp_killing_spree_percent("sf_xp_killing_spree_percent", FCVAR_NONE,
    "Kill XP percent for Killing Spree; multiplies the Armitive knife or pistol bonus when applicable.", 130, true, 0, true, 1000);
static CConVar<int> sf_xp_ultra_kill_percent("sf_xp_ultra_kill_percent", FCVAR_NONE,
    "Kill XP percent for Ultra Kill; multiplies the Armitive knife or pistol bonus when applicable.", 140, true, 0, true, 1000);
static CConVar<int> sf_xp_rampage_percent("sf_xp_rampage_percent", FCVAR_NONE,
    "Kill XP percent for Rampage; multiplies the Armitive knife or pistol bonus when applicable.", 150, true, 0, true, 1000);
static CConVar<int> sf_xp_dominating_percent("sf_xp_dominating_percent", FCVAR_NONE,
    "Kill XP percent for Dominating; multiplies the Armitive knife or pistol bonus when applicable.", 160, true, 0, true, 1000);
static CConVar<int> sf_xp_unstoppable_percent("sf_xp_unstoppable_percent", FCVAR_NONE,
    "Kill XP percent for Unstoppable; multiplies the Armitive knife or pistol bonus when applicable.", 170, true, 0, true, 1000);
static CConVar<int> sf_xp_monster_kill_percent("sf_xp_monster_kill_percent", FCVAR_NONE,
    "Kill XP percent for Monster Kill; multiplies the Armitive knife or pistol bonus when applicable.", 180, true, 0, true, 1000);
static CConVar<int> sf_xp_godlike_percent("sf_xp_godlike_percent", FCVAR_NONE,
    "Kill XP percent for Godlike; multiplies the Armitive knife or pistol bonus when applicable.", 190, true, 0, true, 1000);
static CConVar<int> sf_xp_holy_shit_percent("sf_xp_holy_shit_percent", FCVAR_NONE,
    "Kill XP percent for Holy Shit; multiplies the Armitive knife or pistol bonus when applicable.", 200, true, 0, true, 1000);
static CConVar<int> sf_invisant_see_teammates(
    "sf_invisant_see_teammates", FCVAR_NONE,
    "Allow Invisant players to see and spectate Invisant teammates.", 1, true, 0, true, 1);
static CConVar<CUtlString> sf_discord_link("sf_discord_link", FCVAR_NONE,
    "Discord server invite link in chat messages; empty disables the messages.",
    CUtlString("https://discord.gg/TMV2DEPhqT"));
static CConVar<int> sf_discord_message("sf_discord_message", FCVAR_NONE,
    "Enables Discord invite messages in chat.", 1, true, 0, true, 1);
static CConVar<int> sf_discord_interval("sf_discord_interval", FCVAR_NONE,
    "Seconds between recurring Discord invite messages.", 1200, true, 1, true, 86400);
static CConVar<int> sf_audio_hint("sf_audio_hint", FCVAR_NONE,
    "Enables the automatic audio chat hint.", 1, true, 0, true, 1);
static CConVar<int> sf_audio_hint_cooldown("sf_audio_hint_cooldown", FCVAR_NONE,
    "Seconds before the automatic audio chat hint can trigger again.", 60, true, 0, true, 86400);
static CConVar<int> sf_cheat_hint("sf_cheat_hint", FCVAR_NONE,
    "Enables the automatic cheat chat hint.", 1, true, 0, true, 1);
static CConVar<int> sf_cheat_hint_cooldown("sf_cheat_hint_cooldown", FCVAR_NONE,
    "Seconds before the automatic cheat chat hint can trigger again.", 60, true, 0, true, 86400);
static CConVar<int> sf_upgrade_hint("sf_upgrade_hint", FCVAR_NONE,
    "Enables the automatic upgrade chat hint.", 1, true, 0, true, 1);
static CConVar<int> sf_upgrade_hint_cooldown("sf_upgrade_hint_cooldown", FCVAR_NONE,
    "Seconds before the automatic upgrade chat hint can trigger again.", 60, true, 0, true, 86400);
static CConVar<int> sf_bugs_hint("sf_bugs_hint", FCVAR_NONE,
    "Enables the automatic known-bugs chat hint.", 1, true, 0, true, 1);
static CConVar<int> sf_bugs_hint_cooldown("sf_bugs_hint_cooldown", FCVAR_NONE,
    "Seconds before the automatic known-bugs chat hint can trigger again.", 60, true, 0, true, 86400);
static CConVar<int> sf_menu_hint("sf_menu_hint", FCVAR_NONE,
    "Enables the automatic upgrade-menu chat hint.", 1, true, 0, true, 1);
static CConVar<int> sf_menu_hint_cooldown("sf_menu_hint_cooldown", FCVAR_NONE,
    "Seconds before the automatic upgrade-menu chat hint can trigger again.", 60, true, 0, true, 86400);
static CConVar<int> sf_reset_hint("sf_reset_hint", FCVAR_NONE,
    "Enables the automatic upgrade-reset chat hint.", 1, true, 0, true, 1);
static CConVar<int> sf_reset_hint_cooldown("sf_reset_hint_cooldown", FCVAR_NONE,
    "Seconds before the automatic upgrade-reset chat hint can trigger again.", 60, true, 0, true, 86400);
static CConVar<int> sf_rank_hint("sf_rank_hint", FCVAR_NONE,
    "Enables the automatic rank chat hint.", 1, true, 0, true, 1);
static CConVar<int> sf_rank_hint_cooldown("sf_rank_hint_cooldown", FCVAR_NONE,
    "Seconds before the automatic rank chat hint can trigger again.", 60, true, 0, true, 86400);
static CConVar<int> sf_hud("sf_hud", FCVAR_NONE,
    "Show role, level, XP and queue position in the round-start center panel.",
    1, true, 0, true, 1);
static CConVar<int> sf_round_info_seconds("sf_round_info_seconds", FCVAR_NONE,
    "Seconds to show the round-start role, level, XP and queue panel. Set to 0 to disable.",
    10, true, 0, true, 120);
static CConVar<float> sf_hud_kill_seconds("sf_hud_kill_seconds", FCVAR_NONE,
    "Seconds to show each kill XP line, including post-round kills; 0 hides these lines.", 5.0f, true, 0.0f, true, 120.0f);
static CConVar<float> sf_hud_assist_seconds("sf_hud_assist_seconds", FCVAR_NONE,
    "Seconds to show each assist XP line; 0 hides these lines without changing XP awards.", 5.0f, true, 0.0f, true, 120.0f);
static CConVar<float> sf_hud_round_result_seconds("sf_hud_round_result_seconds", FCVAR_NONE,
    "Seconds to show the round win or draw title; 0 hides the title.", 10.0f, true, 0.0f, true, 120.0f);
static CConVar<float> sf_hud_round_win_xp_seconds("sf_hud_round_win_xp_seconds", FCVAR_NONE,
    "Seconds to show round-win XP; 0 hides this line without changing XP awards.", 10.0f, true, 0.0f, true, 120.0f);
static CConVar<float> sf_hud_levelup_seconds("sf_hud_levelup_seconds", FCVAR_NONE,
    "Seconds to show a level-up line; 0 hides these lines.", 5.0f, true, 0.0f, true, 120.0f);
static CConVar<float> sf_hud_priority_seconds("sf_hud_priority_seconds", FCVAR_NONE,
    "Seconds to show earned Invisant queue priority; 0 hides this line without changing priority.", 5.0f, true, 0.0f, true, 120.0f);
static CConVar<float> sf_hud_no_rewards_seconds("sf_hud_no_rewards_seconds", FCVAR_NONE,
    "Seconds to show No rewards after round end following a post-round kill; 0 hides this line.", 5.0f, true, 0.0f, true, 120.0f);
static CConVar<int> sf_timeout_warning_seconds("sf_timeout_warning_seconds", FCVAR_NONE,
    "Seconds before timeout when the Invisant fade warning and reveal voice begin; 0 disables both.", 10, true, 0, true, 120);
static CConVar<int> sf_wallhack("sf_wallhack", FCVAR_NONE,
    "Show living Armitive with through-wall outlines to living Invisant players.",
    1, true, 0, true, 1);
static CConVar<int> sf_ward_screen_tint(
    "sf_ward_screen_tint", FCVAR_NONE,
    "Green screen tint strength while Ward is active. Set to 0 to disable; 255 is maximum.",
    85, true, 0, true, 255);
static CConVar<int> sf_extra_spawns("sf_extra_spawns", FCVAR_NONE,
    "Create validated additional team spawns on Windows and Linux when the map does not have enough for the maximum roster.",
    1, true, 0, true, 1);
static CConVar<int> sf_blood_amount("sf_blood_amount", FCVAR_NONE,
    "Number of additional blood-splatter bursts when an Armitive damages an Invisant or an Invisant knife damages an Armitive. Set to 0 for normal blood only.",
    4, true, 0, true, 6);
static CConVar<float> sf_tonemap_exposure("sf_tonemap_exposure", FCVAR_NONE,
    "Attempts to set HDR exposure on the map's tonemap controller. Stock de_dust2 lighting is baked and unaffected.",
    0.40f, true, 0.05f, true, 1.0f);
static CConVar<int> sf_admin_spectate("sf_admin_spectate", FCVAR_NONE,
    "Allow administrators with the generic flag to observe players or use a spectator free camera.",
    1, true, 0, true, 1);
static CConVar<int> sf_custom_sounds("sf_custom_sounds", FCVAR_NONE,
    "Play Shroudfire voice clips when its CS2 Workshop sound pack is mounted for clients.",
    0, true, 0, true, 1);
static CConVar<int> sf_silent_local_movement("sf_silent_local_movement", FCVAR_NONE,
    "Experimentally block an Invisant's own footsteps, jumps, and landings using the updated Workshop pack.",
    0, true, 0, true, 1);
static CConVar<float> sf_round_win_sound_delay("sf_round_win_sound_delay", FCVAR_NONE,
    "Delay in seconds before playing an Armitive win, Invisant win, or round draw voice clip.",
    0.5f, true, 0.0f, true, 10.0f);

CON_COMMAND_F(sf_level, "Displays your Invisant and Armitive XP and levels.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    if (slot.IsValid()) g_shroudfire_plugin.show_level(slot.Get() + 1);
}

CON_COMMAND_F(sf_status,
    "Displays Shroudfire state, map rotation, teams, queue, AFK players, and spawn capacity.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_status(slot.IsValid() ? slot.Get() + 1 : 0);
}

CON_COMMAND_F(sf_update_notice,
    "Announces an upcoming CS2 update restart in chat. Server console only.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_update_notice(slot.IsValid() ? slot.Get() + 1 : 0, args);
}

CON_COMMAND_F(sf_cvar,
    "Reads or sets a server cvar from the console. Requires the generic admin flag.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_cvar(slot.IsValid() ? slot.Get() + 1 : 0, args);
}

CON_COMMAND_F(sf_setlevel,
    "Sets an online player's Invisant or Armitive level to its starting XP, resets that role's purchased upgrades, and recalculates its available upgrade points for the assigned level.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_setlevel(slot.IsValid() ? slot.Get() + 1 : 0, args);
}

CON_COMMAND_F(sf_restart,
    "Restarts the current round without advancing the Invisant roster, including with one active player. Requires the generic admin flag.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_restart(slot.IsValid() ? slot.Get() + 1 : 0);
}

CON_COMMAND_F(sf_nextmap,
    "Displays or sets the next map; use reset to follow the configured rotation.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_nextmap(slot.IsValid() ? slot.Get() + 1 : 0, args);
}

CON_COMMAND_F(sf_invisant,
    "Moves you to the Invisant team and respawns you at its spawn.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_invisant(slot.IsValid() ? slot.Get() + 1 : 0);
}

CON_COMMAND_F(sf_armitive,
    "Moves you to the Armitive team and respawns you at its spawn.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_armitive(slot.IsValid() ? slot.Get() + 1 : 0);
}

CON_COMMAND_F(sf_observe,
    "Moves you to Spectator to watch a living player or use a free camera.", FCVAR_NONE) {
    const CPlayerSlot slot = context.GetPlayerSlot();
    g_shroudfire_plugin.command_observe(slot.IsValid() ? slot.Get() + 1 : 0, args);
}

namespace {

GameEventKeySymbol_t event_key(const char *name) {
    return {CUtlStringToken(MurmurHash2LowerCase(name, STRINGTOKEN_MURMURHASH_SEED)), name};
}

bool pistol_weapon(const char *name) {
    static const char *pistols[] = {"glock", "hkp2000", "usp_silencer", "p250", "deagle",
        "elite", "fiveseven", "tec9", "cz75a", "revolver"};
    if (std::strncmp(name, "weapon_", 7) == 0) name += 7;
    for (const char *pistol : pistols) if (std::strcmp(name, pistol) == 0) return true;
    return false;
}

} // namespace

ShroudfirePlugin::ShroudfirePlugin()
    : game_frame_(&IServerGameDLL::GameFrame, this, &ShroudfirePlugin::Hook_GameFramePre,
                  &ShroudfirePlugin::Hook_GameFrame),
      post_sound_event_(static_cast<void (IGameEventSystem::*)(CSplitScreenSlot, bool,
          int, const uint64 *, INetworkMessageInternal *, const CNetMessage *,
          unsigned long, NetChannelBufType_t)>(&IGameEventSystem::PostEventAbstract),
          this, &ShroudfirePlugin::Hook_PostSoundEvent, nullptr),
      game_event_fire_(&IGameEventManager2::FireEvent, this,
                       &ShroudfirePlugin::Hook_FireGameEvent, nullptr),
      client_active_(&IServerGameClients::ClientActive, this, nullptr, &ShroudfirePlugin::Hook_ClientActive),
      client_disconnect_(&IServerGameClients::ClientDisconnect, this, nullptr,
                         &ShroudfirePlugin::Hook_ClientDisconnect),
      client_connected_(&IServerGameClients::OnClientConnected, this,
                        &ShroudfirePlugin::Hook_OnClientConnected, nullptr),
      client_command_(&IServerGameClients::ClientCommand, this,
                      &ShroudfirePlugin::Hook_ClientCommand, nullptr),
      cvar_command_(&ICvar::DispatchConCommand, this,
                    &ShroudfirePlugin::Hook_DispatchConCommand, nullptr),
      check_transmit_(&ISource2GameEntities::CheckTransmit, this, nullptr,
                      &ShroudfirePlugin::Hook_CheckTransmit) {}

bool ShroudfirePlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
    PLUGIN_SAVEVARS();
    for (auto &target : admin_observe_target_) target.store(0, std::memory_order_relaxed);
    GET_V_IFACE_CURRENT(GetEngineFactory, engine_, IVEngineServer2,
                        SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, cvar_, ICvar, CVAR_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, server_, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
    GET_V_IFACE_ANY(GetServerFactory, game_clients_, IServerGameClients,
                    INTERFACEVERSION_SERVERGAMECLIENTS);
    GET_V_IFACE_ANY(GetEngineFactory, network_service_, INetworkServerService,
                    NETWORKSERVERSERVICE_INTERFACE_VERSION);
    game_resources_ = static_cast<IGameResourceService *>(ismm->VInterfaceMatch(
        ismm->GetEngineFactory(), GAMERESOURCESERVICESERVER_INTERFACE_VERSION));
    ISchemaSystem *schema = static_cast<ISchemaSystem *>(ismm->VInterfaceMatch(
        ismm->GetEngineFactory(), SCHEMASYSTEM_INTERFACE_VERSION));
    game_entities_ = static_cast<ISource2GameEntities *>(ismm->VInterfaceMatch(
        ismm->GetServerFactory(), SOURCE2GAMEENTITIES_INTERFACE_VERSION));
    std::string engine_error;
    auto *network_messages = static_cast<INetworkMessages *>(ismm->VInterfaceMatch(
        ismm->GetEngineFactory(), NETWORKMESSAGES_INTERFACE_VERSION));
    auto *event_system = static_cast<IGameEventSystem *>(ismm->VInterfaceMatch(
        ismm->GetEngineFactory(), GAMEEVENTSYSTEM_INTERFACE_VERSION));
    if (!engine_adapter_.initialize(game_resources_, schema, server_, network_messages,
                                    event_system, engine_error))
        META_CONPRINTF("[Shroudfire] %s\n", engine_error.c_str());
    if (engine_adapter_.ready()) {
        damage_hook_ = std::make_unique<KHook::Function<int64_t, void *, void *, void *>>(
            this, &ShroudfirePlugin::Hook_TakeDamage, nullptr);
        damage_hook_->Configure(engine_adapter_.damage_function());
        weapon_can_use_hook_ = std::make_unique<KHook::Function<bool, void *, void *>>(
            this, &ShroudfirePlugin::Hook_WeaponCanUse, nullptr);
        weapon_can_use_hook_->Configure(engine_adapter_.weapon_can_use_function());
        can_acquire_hook_ = std::make_unique<KHook::Function<int, void *, void *, int, uint64_t>>(
            this, &ShroudfirePlugin::Hook_CanAcquire, nullptr);
        can_acquire_hook_->Configure(engine_adapter_.can_acquire_function());
        if (engine_adapter_.sound_filter_function()) {
            sound_filter_hook_ = std::make_unique<KHook::Function<SoundEventResult,
                IRecipientFilter &, int, const void *>>(
                    this, &ShroudfirePlugin::Hook_EmitSoundFilter,
                    &ShroudfirePlugin::Hook_EmitSoundFilterPost);
            sound_filter_hook_->Configure(engine_adapter_.sound_filter_function());
        }
        if (engine_adapter_.process_movement_function()) {
            process_movement_hook_ = std::make_unique<KHook::Function<void, void *, void *>>(
                this, &ShroudfirePlugin::Hook_ProcessMovement,
                &ShroudfirePlugin::Hook_ProcessMovementPost);
            process_movement_hook_->Configure(engine_adapter_.process_movement_function());
        } else META_CONPRINTF("[Shroudfire] Movement hook unavailable; Invisant knife remap disabled.\n");
        game_event_init_hook_ = std::make_unique<KHook::Function<void, IGameEventManager2 *>>(
            this, &ShroudfirePlugin::Hook_GameEventManagerInit,
            &ShroudfirePlugin::Hook_GameEventManagerInitialized);
        game_event_init_hook_->Configure(engine_adapter_.game_event_manager_init_function());
        host_say_hook_ = std::make_unique<KHook::Function<void, CEntityInstance *, CCommand &,
            bool, int, const char *>>(this, &ShroudfirePlugin::Hook_HostSay,
                                     &ShroudfirePlugin::Hook_HostSayPost);
        host_say_hook_->Configure(engine_adapter_.host_say_function());
    }
    g_pCVar = cvar_;
    META_CONVAR_REGISTER(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);
    profiles_.emplace(std::filesystem::path(ismm->GetBaseDir()) / "addons" / "shroudfire" / "data");
    rankings_ = {};
    for (const auto &profile : profiles_->load_all()) rankings_.update(profile);
    admins_.clear();
    {
        std::ifstream admins_file(std::filesystem::path(ismm->GetBaseDir()) /
            "addons" / "shroudfire" / "admins.txt");
        std::string line;
        while (std::getline(admins_file, line)) {
            std::istringstream input(line);
            std::string steam_id, access;
            if (!(input >> steam_id >> access) || steam_id.front() == '#') continue;
            char *end = nullptr;
            const uint64_t id = std::strtoull(steam_id.c_str(), &end, 10);
            if (end && *end == '\0' && id > 0 && (access == "generic" || access == "root"))
                admins_[id] = access == "root" ? 2 : 1;
        }
    }
    game_frame_.Add(server_);
    if (event_system) post_sound_event_.Add(event_system);
    client_active_.Add(game_clients_);
    client_disconnect_.Add(game_clients_);
    client_connected_.Add(game_clients_);
    client_command_.Add(game_clients_);
    cvar_command_.Add(cvar_);
    if (game_entities_) check_transmit_.Add(game_entities_);
    g_SMAPI->AddListener(this, this);
    if (late) {
        mode_.map_start(game_time());
        INetworkGameServer *game = network_service_ ? network_service_->GetIGameServer() : nullptr;
        if (game && game->GetMapName()) map_rotation_.current = game->GetMapName();
        map_rotation_.last_time = game_time();
    }
    META_CONPRINTF("[Shroudfire] CS2 development build loaded; engine adapter %s.\n",
                   engine_adapter_.ready() ? "ready for this server build" : "disabled");
    META_CONPRINTF("[Shroudfire] Armitive outline adapter %s.\n",
                   engine_adapter_.glow_ready() ? "ready" : "unavailable");
    return true;
}

bool ShroudfirePlugin::Unload(char *error, size_t maximum) {
    // The map resource system retains this plugin's callback until engine shutdown.
    // MetaMod can unload us safely only after a full server restart.
    if (engine_adapter_.ready()) {
        if (error && maximum)
            std::snprintf(error, maximum,
                "Shroudfire registered a CS2 game system; restart the server to unload it");
        return false;
    }
    invisant_transmit_mask_.store(0, std::memory_order_release);
    clear_armitive_glows();
    damage_hook_.reset();
    weapon_can_use_hook_.reset();
    can_acquire_hook_.reset();
    sound_filter_hook_.reset();
    player_run_command_hook_.reset();
    player_run_command_target_ = nullptr;
    observer_run_command_hook_.reset();
    observer_run_command_target_ = nullptr;
    process_movement_hook_.reset();
    game_event_init_hook_.reset();
    host_say_hook_.reset();
    pending_observers_.clear();
    for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer)
        end_observation(viewer, "plugin unloaded");
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
        if (guide_open_[slot]) close_guide(slot);
    restore_shields();
    engine_adapter_.remove_fade_entity();
    engine_adapter_.remove_blood_entity();
    engine_adapter_.remove_claw_entity();
    engine_adapter_.remove_extra_spawns();
    engine_adapter_.restore_tonemap();
    restore_game_cvars();
    if (game_events_) game_event_fire_.Remove(game_events_);
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        if (profiles_ && mode_.player(slot).connected && !mode_.player(slot).bot)
            profiles_->save(mode_.profile(slot, static_cast<int>(std::time(nullptr))));
    }
    game_frame_.Remove(server_);
    client_active_.Remove(game_clients_);
    client_disconnect_.Remove(game_clients_);
    client_connected_.Remove(game_clients_);
    client_command_.Remove(game_clients_);
    cvar_command_.Remove(cvar_);
    if (game_entities_) check_transmit_.Remove(game_entities_);
    ConVar_Unregister();
    return true;
}

void ShroudfirePlugin::AllPluginsLoaded() {}

float ShroudfirePlugin::game_time() const {
    INetworkGameServer *game = network_service_ ? network_service_->GetIGameServer() : nullptr;
    return game ? game->GetTime() : 0.0f;
}

void ShroudfirePlugin::OnLevelInit(const char *map, const char *, const char *, const char *, bool, bool) {
    map_rotation_ = {};
    if (map) map_rotation_.current = map;
    map_rotation_.last_time = game_time();
    local_movement_next_refresh_at_.fill(0.0f);
    engine_adapter_.restore_reveal_rendering();
    timeout_reveal_ready_mask_ = 0;
    timeout_reveal_last_alpha_.fill(-1);
    if (engine_) engine_->PrecacheGeneric(
        "particles/blood_impact/blood_impact_mist1.vpcf");
    mode_.map_start(game_time());
    clear_armitive_glows();
    last_glow_refresh_ = 0.0f;
    last_mode_tick_ = 0.0f;
    last_profile_save_ = 0.0f;
    pending_initial_restart_ = 0.0f;
    solo_rejoin_freeze_pending_ = false;
    solo_rejoin_freeze_until_ = 0.0f;
    initial_restart_attempts_ = 0;
    engine_adapter_.clear_weapon_tracking();
    engine_adapter_.clear_tonemap_tracking();
    engine_adapter_.clear_fade_tracking();
    engine_adapter_.clear_blood_tracking();
    engine_adapter_.clear_claw_entity_tracking();
    engine_adapter_.clear_extra_spawn_tracking();
    engine_adapter_.clear_win_layout_tracking();
    engine_adapter_.clear_round_ui_tracking();
    pending_claw_hits_.fill({});
    last_native_claw_hit_tick_.fill(0);
    claw_extended_applying_.fill(false);
    last_extended_claw_hit_tick_.fill(0);
    spawn_config_ = -1;
    guide_open_.fill(false);
    guide_last_html_.fill({});
    guide_last_sent_at_.fill(0.0f);
    guide_first_send_at_.fill(0.0f);
    guide_hint_due_.fill(0.0f);
    cheat_hint_pending_at_ = 0.0f;
    audio_hint_pending_at_ = 0.0f;
    upgrade_hint_pending_at_ = 0.0f;
    bugs_hint_pending_at_ = 0.0f;
    menu_hint_pending_at_ = 0.0f;
    reset_hint_pending_at_ = 0.0f;
    rank_hint_pending_at_ = 0.0f;
    discord_hint_pending_at_ = 0.0f;
    center_notices_.fill({});
    auto_hud_visible_.fill(false);
    round_info_until_ = 0.0f;
    pending_round_end_at_ = 0.0f;
    pending_round_winner_ = 0;
    round_timeout_requested_ = false;
    round_win_voice_at_ = 0.0f;
    round_win_voice_role_ = shroudfire::Role::spectator;
    last_observer_check_ = 0.0f;
    objectives_removed_ = false;
    tonemap_reported_ = false;
}

void ShroudfirePlugin::OnLevelShutdown() {
    map_rotation_ = {};
    local_movement_next_refresh_at_.fill(0.0f);
    engine_adapter_.restore_reveal_rendering();
    timeout_reveal_ready_mask_ = 0;
    clear_armitive_glows();
    last_glow_refresh_ = 0.0f;
    last_mode_tick_ = 0.0f;
    last_profile_save_ = 0.0f;
    pending_initial_restart_ = 0.0f;
    solo_rejoin_freeze_pending_ = false;
    solo_rejoin_freeze_until_ = 0.0f;
    initial_restart_attempts_ = 0;
    objectives_removed_ = false;
    invisant_transmit_mask_.store(0, std::memory_order_release);
    engine_adapter_.clear_fade_tracking();
    engine_adapter_.clear_blood_tracking();
    engine_adapter_.clear_claw_entity_tracking();
    engine_adapter_.clear_win_layout_tracking();
    engine_adapter_.clear_round_ui_tracking();
    engine_adapter_.remove_extra_spawns();
    pending_claw_hits_.fill({});
    last_native_claw_hit_tick_.fill(0);
    claw_extended_applying_.fill(false);
    last_extended_claw_hit_tick_.fill(0);
    spawn_config_ = -1;
    guide_open_.fill(false);
    guide_last_html_.fill({});
    guide_last_sent_at_.fill(0.0f);
    guide_first_send_at_.fill(0.0f);
    guide_hint_due_.fill(0.0f);
    round_info_until_ = 0.0f;
    pending_round_end_at_ = 0.0f;
    pending_round_winner_ = 0;
    round_timeout_requested_ = false;
    round_win_voice_at_ = 0.0f;
    round_win_voice_role_ = shroudfire::Role::spectator;
    center_notices_.fill({});
    auto_hud_visible_.fill(false);
    cheat_hint_pending_at_ = 0.0f;
    audio_hint_pending_at_ = 0.0f;
    upgrade_hint_pending_at_ = 0.0f;
    bugs_hint_pending_at_ = 0.0f;
    menu_hint_pending_at_ = 0.0f;
    reset_hint_pending_at_ = 0.0f;
    rank_hint_pending_at_ = 0.0f;
    discord_hint_pending_at_ = 0.0f;
    restore_shields();
    pending_engine_actions_.clear();
    pending_spawns_.clear();
    pending_fire_acceleration_.clear();
    pending_observers_.clear();
    for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer)
        end_observation(viewer, "map ended");
    restore_game_cvars();
    engine_adapter_.restore_tonemap();
    engine_adapter_.clear_weapon_tracking();
    engine_adapter_.clear_tonemap_tracking();
    mode_.end_round(shroudfire::Role::spectator, false);
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        if (profiles_ && mode_.player(slot).connected && !mode_.player(slot).bot)
            profiles_->save(mode_.profile(slot, static_cast<int>(std::time(nullptr))));
    }
}

void ShroudfirePlugin::sync_settings() {
    auto &s = mode_.settings();
    s.enabled = sf_enable.Get() != 0 && engine_adapter_.ready() && game_entities_ && game_events_;
    s.armitive_per_invisant = sf_visible_per_invisant.Get();
    if (armitive_weapon_restrictions_.configure(sf_armitive_blocked_weapons.Get().String())) {
        for (const auto &name : armitive_weapon_restrictions_.unknown)
            META_CONPRINTF("[Shroudfire] Unknown weapon in sf_armitive_blocked_weapons: %s.\n", name.c_str());
    }
    s.round_seconds = sf_round_time.Get();
    s.timeout_kill_invisant = sf_timeout_kill_invisant.Get() != 0;
    s.timeout_reveal_seconds = sf_timeout_reveal_seconds.Get();
    s.map_join_grace = sf_map_join_grace.Get();
    s.afk_seconds = sf_afk_seconds.Get();
    s.afk_warning_seconds = sf_afk_warning_seconds.Get();
    s.afk_limit = sf_afk_limit.Get();
    s.starvation_round_delay = sf_starvation_round_delay.Get();
    s.starvation_combat_delay = sf_starvation_combat_delay.Get();
    s.starvation_damage = sf_starvation_damage.Get();
    s.starvation_interval = sf_starvation_interval.Get();
    s.starvation_acceleration_delay = sf_starvation_acceleration_delay.Get();
    s.starvation_accelerated_interval = sf_starvation_accelerated_interval.Get();
    s.starvation_accelerated_damage = sf_starvation_accelerated_damage.Get();
    s.camping_distance = sf_camping_distance.Get();
    s.camping_warning_seconds = sf_camping_warning_seconds.Get();
    s.camping_damage_delay = sf_camping_damage_delay.Get();
    s.camping_damage = sf_camping_damage.Get();
    s.reconnect_grace = sf_queue_reconnect_grace.Get();
    s.xp_kill_invisant = sf_xp_kill_invisant.Get();
    s.xp_kill_armitive = sf_xp_kill_armitive.Get();
    s.xp_round_win_invisant = sf_xp_round_win_invisant.Get();
    s.xp_round_win_armitive = sf_xp_round_win_armitive.Get();
    s.xp_armitive_pistol_percent = sf_xp_armitive_pistol_percent.Get();
    s.xp_armitive_knife_percent = sf_xp_armitive_knife_percent.Get();
    s.xp_level_base = sf_xp_level_base.Get();
    s.kill_streak_window = sf_kill_streak_window.Get();
    s.assist_fraction = sf_xp_assist_fraction.Get();
    s.assist_damage_share = sf_xp_assist_damage_share.Get();
    s.streak_percent = {
        sf_xp_first_blood_percent.Get(),
        sf_xp_double_kill_percent.Get(),
        sf_xp_killing_spree_percent.Get(),
        sf_xp_ultra_kill_percent.Get(),
        sf_xp_rampage_percent.Get(),
        sf_xp_dominating_percent.Get(),
        sf_xp_unstoppable_percent.Get(),
        sf_xp_monster_kill_percent.Get(),
        sf_xp_godlike_percent.Get(),
        sf_xp_holy_shit_percent.Get()
    };
    s.normal_upgrade_levels = {sf_normal_stage1_level.Get(), sf_normal_stage2_level.Get(), sf_normal_stage3_level.Get(), sf_normal_stage4_level.Get(), sf_normal_stage5_level.Get()};
    s.ultimate_upgrade_levels = {sf_ultimate_stage1_level.Get(), sf_ultimate_stage2_level.Get(), sf_ultimate_stage3_level.Get(), sf_ultimate_stage4_level.Get(), sf_ultimate_stage5_level.Get()};
    s.invisant_base_armor = sf_invisant_base_armor.Get();
    s.lightweight_gravity_percent = sf_lightweight_gravity_percent.Get();
    s.momentum_run_percent = sf_momentum_run_percent.Get();
    s.resilience_armor_per_stage = sf_resilience_armor_per_stage.Get();
    s.resilience_helmet_stage = sf_resilience_helmet_stage.Get();
    s.reforging_armor_per_stage = sf_reforging_armor_per_stage.Get();
    s.reforging_interval = sf_reforging_interval.Get();
    s.vampirism_health_per_stage = sf_vampirism_health_per_stage.Get();
    s.bloodlust_health_per_stage = sf_bloodlust_health_per_stage.Get();
    s.mobility_crouch_percent = sf_mobility_crouch_percent.Get();
    s.surge_base_speed = sf_surge_base_speed.Get();
    s.surge_speed_per_stage = sf_surge_speed_per_stage.Get();
    s.surge_add_momentum = sf_surge_add_momentum.Get() != 0;
    s.surge_cooldown = sf_surge_cooldown.Get();
    s.armitive_base_health = sf_armitive_base_health.Get();
    s.fortitude_health_per_stage = sf_fortitude_health_per_stage.Get();
    s.recovery_health_per_stage = sf_recovery_health_per_stage.Get();
    s.recovery_interval = sf_recovery_interval.Get();
    s.overload_magazine_percent = sf_overload_magazine_percent.Get();
    s.stockpile_clips_per_stage = sf_stockpile_clips_per_stage.Get();
    s.barrage_fire_rate_percent = sf_barrage_fire_rate_percent.Get();
    s.control_reduction_percent = sf_control_reduction_percent.Get();
    s.dexterity_speed_percent = sf_dexterity_speed_percent.Get();
    s.ward_base_seconds = sf_ward_base_seconds.Get();
    s.ward_seconds_per_stage = sf_ward_seconds_per_stage.Get();
    s.ward_cooldown = sf_ward_cooldown.Get();
    s.upgrade_purchase_window = sf_upgrade_purchase_window.Get();
}

void ShroudfirePlugin::sync_game_cvars() {
    if (!mode_.active()) {
        if (glow_count_.load(std::memory_order_acquire)) clear_armitive_glows();
        objectives_removed_ = false;
        tonemap_reported_ = false;
        restore_shields();
        engine_adapter_.remove_fade_entity();
        engine_adapter_.restore_reveal_rendering();
        engine_adapter_.remove_blood_entity();
        engine_adapter_.remove_claw_entity();
        engine_adapter_.restore_tonemap();
        restore_game_cvars();
        return;
    }
    if (!objectives_removed_) {
        const int removed = engine_adapter_.remove_objectives();
        if (removed >= 0) objectives_removed_ = true;
    }
    const int tonemap_count = engine_adapter_.apply_tonemap(
        sf_tonemap_exposure.Get());
    if (!tonemap_reported_ && tonemap_count >= 0) {
        META_CONPRINTF("[Shroudfire] Tonemap controllers on this map: %d.\n", tonemap_count);
        tonemap_reported_ = true;
    }
    for (size_t i = 0; i < suicide_cheat_flags_removed_.size(); ++i) {
        ConCommandRef command = cvar_->FindConCommand(i == 0 ? "kill" : "explode");
        if (command.IsValidRef() && command.IsFlagSet(FCVAR_CHEAT)) {
            command.RemoveFlags(FCVAR_CHEAT);
            suicide_cheat_flags_removed_[i] = true;
        }
    }
    if (!game_cvars_overridden_) {
        for (auto &setting : game_cvars_) {
            ConVarRefAbstract cvar(setting.name, true);
            setting.available = cvar.IsValidRef() && cvar.IsConVarDataAvailable();
            if (setting.available) {
                setting.original = cvar.GetFloat();
                if (std::strcmp(setting.name, "sv_warmup_to_freezetime_delay") == 0 && cvar.HasMin()) {
                    // CS2 clamps this delay to at least three seconds. Give
                    // this cvar a mode-owned zero minimum, preserving the
                    // original allocation for disable/unload restoration.
                    auto *data = cvar.GetConVarData();
                    if (data->TypeTraits()->StringToValue("0", &warmup_delay_zero_minimum_)) {
                        warmup_delay_original_minimum_ = data->MinValue();
                        data->SetMinValue(&warmup_delay_zero_minimum_);
                    }
                }
                if (setting.minimum)
                    setting.enforced = (std::max)(setting.original, setting.enforced);
            }
        }
        for (auto &setting : loadout_cvars_) {
            ConVarRefAbstract cvar(setting.name, true);
            setting.available = cvar.IsValidRef() && cvar.IsConVarDataAvailable();
            if (setting.available) setting.original = cvar.GetString().String();
        }
        game_cvars_overridden_ = true;
    }
    const float round_minutes = static_cast<float>(mode_.settings().round_seconds) / 60.0f;
    for (auto &setting : game_cvars_) {
        if (!setting.available) continue;
        ConVarRefAbstract cvar(setting.name, true);
        if (setting.round_time) setting.enforced = round_minutes;
        if (std::strcmp(setting.name, "mp_ignore_round_win_conditions") == 0)
            setting.enforced = mode_.active_players() >= 2 ? 0.0f : 1.0f;
        if (setting.integer) {
            if (cvar.GetInt() != static_cast<int>(setting.enforced))
                cvar.SetInt(static_cast<int>(setting.enforced));
        } else if (std::abs(cvar.GetFloat() - setting.enforced) > 0.001f) {
            cvar.SetFloat(setting.enforced);
        }
    }
    for (size_t i = 0; i < loadout_cvars_.size(); ++i) {
        const auto &setting = loadout_cvars_[i];
        if (!setting.available) continue;
        const bool invisant_team_defaults = i < 2
            ? sf_invisant_team.Get() == 2
            : sf_invisant_team.Get() == 3;
        const char *desired = invisant_team_defaults ? "" : setting.original.c_str();
        ConVarRefAbstract cvar(setting.name, true);
        if (std::strcmp(cvar.GetString().String(), desired) != 0)
            cvar.SetString(CUtlString(desired));
    }
}

bool ShroudfirePlugin::map_available(const std::string &map) const {
    if (!engine_ || map.empty() || map.size() > 127 || map.front() == '/' ||
        map.back() == '/' || map.find("//") != std::string::npos) return false;
    for (const unsigned char character : map) {
        if (!std::isalnum(character) && character != '_' && character != '-' && character != '/')
            return false;
    }
    return engine_->IsMapValid(map.c_str()) > 0;
}

std::string ShroudfirePlugin::next_map() const {
    if (!map_rotation_.override_map.empty() && map_available(map_rotation_.override_map))
        return map_rotation_.override_map;
    std::string configured(sf_map_rotation.Get().String());
    std::replace(configured.begin(), configured.end(), ',', ' ');
    std::istringstream input(configured);
    std::vector<std::string> maps;
    for (std::string map; input >> map;) {
        if (std::find(maps.begin(), maps.end(), map) == maps.end()) maps.push_back(map);
    }
    if (maps.empty()) return {};
    const auto current = std::find(maps.begin(), maps.end(), map_rotation_.current);
    const size_t start = current == maps.end() ? 0
        : (static_cast<size_t>(current - maps.begin()) + 1) % maps.size();
    for (size_t offset = 0; offset < maps.size(); ++offset) {
        const std::string &map = maps[(start + offset) % maps.size()];
        if (map_available(map)) return map;
    }
    return {};
}

void ShroudfirePlugin::update_map_rotation(float now) {
    auto &rotation = map_rotation_;
    if (rotation.current.empty() || rotation.changing) return;
    const bool enabled = sf_enable.Get() != 0 && mode_.active();
    if (enabled && mode_.active_humans() >= 2)
        rotation.played_seconds += (std::max)(0.0f, now - rotation.last_time);
    rotation.last_time = now;
    const bool round_limit = sf_map_max_rounds.Get() > 0 &&
        rotation.rounds >= sf_map_max_rounds.Get();
    const bool time_limit = sf_map_time_limit.Get() > 0 &&
        rotation.played_seconds >= sf_map_time_limit.Get() * 60.0;
    const auto announce = [this](const std::string &message) {
        std::string chat = kChatPrefix;
        chat += message;
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (player.connected && !player.bot)
                engine_adapter_.client_print(slot, 3, chat.c_str());
        }
        META_CONPRINTF("[Shroudfire] %s\n", message.c_str());
    };
    if (!enabled || (!round_limit && !time_limit)) {
        if (rotation.limit_announced || rotation.change_at > 0.0f)
            announce("Map change cancelled; the current map limit is no longer reached.");
        rotation.limit_announced = false;
        rotation.unavailable_reported = false;
        rotation.change_at = 0.0f;
        rotation.retry_at = 0.0f;
        rotation.pending.clear();
        return;
    }
    if (mode_.round_live() && mode_.active_humans() >= 2) {
        if (!rotation.limit_announced)
            announce("Map limit reached. The map will change after this round.");
        rotation.limit_announced = true;
        return;
    }
    if (now < rotation.retry_at ||
        (rotation.change_at > now && !rotation.pending.empty())) return;
    const std::string destination = next_map();
    if (destination.empty()) {
        if (!rotation.unavailable_reported)
            announce("Map limit reached, but no next map is available. Configure sf_map_rotation or sf_nextmap.");
        rotation.unavailable_reported = true;
        rotation.change_at = 0.0f;
        rotation.pending.clear();
        rotation.retry_at = now + 5.0f;
        return;
    }
    rotation.unavailable_reported = false;
    if (rotation.change_at == 0.0f || rotation.pending != destination) {
        rotation.pending = destination;
        rotation.change_at = now + 5.0f;
        announce("Changing map to " + destination + " in 5 seconds.");
        return;
    }
    rotation.changing = true;
    META_CONPRINTF("[Shroudfire] Changing map from %s to %s after %d rounds and %.1f minutes.\n",
        rotation.current.c_str(), destination.c_str(), rotation.rounds,
        rotation.played_seconds / 60.0);
    engine_->ChangeLevel(destination.c_str(), nullptr);
}

void ShroudfirePlugin::restore_shields() {
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        shield_next_sound_at_[slot] = 0.0f;
        shield_tint_alpha_[slot] = 0;
        if (!shield_applied_[slot]) continue;
        bool ignored = false;
        engine_adapter_.set_damage_enabled(slot, shield_previous_damage_[slot], ignored);
        engine_adapter_.screen_fade(slot, 0, 17, 0, 0, 0, 0);
        shield_applied_[slot] = false;
    }
}

void ShroudfirePlugin::end_observation(int viewer, const char *reason) {
    if (viewer < 1 || viewer > shroudfire::kMaxPlayers) return;
    const int target = admin_observe_target_[viewer].exchange(0, std::memory_order_acq_rel);
    if (!target) return;
    const float duration = (std::max)(0.0f, game_time() - admin_observe_started_at_[viewer]);
    META_LOG(g_PLAPI, "Admin observation ended: administrator %llu, target %llu, reason %s, duration %.1f seconds",
        static_cast<unsigned long long>(mode_.player(viewer).steam_id),
        static_cast<unsigned long long>(admin_observe_target_steam_[viewer]),
        reason, duration);
    admin_observe_target_steam_[viewer] = 0;
    admin_observe_started_at_[viewer] = 0.0f;
}

void ShroudfirePlugin::restore_game_cvars() {
    for (size_t i = 0; i < suicide_cheat_flags_removed_.size(); ++i) {
        if (!suicide_cheat_flags_removed_[i]) continue;
        ConCommandRef command = cvar_->FindConCommand(i == 0 ? "kill" : "explode");
        if (command.IsValidRef()) command.AddFlags(FCVAR_CHEAT);
        suicide_cheat_flags_removed_[i] = false;
    }
    if (!game_cvars_overridden_) return;
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
        if (mode_.player(slot).connected && mode_.player(slot).alive) {
            engine_adapter_.apply_ammo_upgrades(slot, 0, 0,
                mode_.settings().overload_magazine_percent,
                mode_.settings().stockpile_clips_per_stage);
            engine_adapter_.apply_dexterity(slot, 0,
                mode_.settings().dexterity_speed_percent, 0.0f, 0);
            engine_adapter_.apply_claw_timing(slot, 0.0f, 0, false,
                sf_knife_attack_speed_percent.Get());
        }
    for (auto &setting : game_cvars_) {
        if (!setting.available) continue;
        ConVarRefAbstract cvar(setting.name, true);
        if (!cvar.IsValidRef() || !cvar.IsConVarDataAvailable()) continue;
        if (setting.integer) cvar.SetInt(static_cast<int>(setting.original));
        else cvar.SetFloat(setting.original);
    }
    if (warmup_delay_original_minimum_) {
        ConVarRefAbstract cvar("sv_warmup_to_freezetime_delay", true);
        if (cvar.IsValidRef() && cvar.IsConVarDataAvailable())
            cvar.GetConVarData()->SetMinValue(warmup_delay_original_minimum_);
        warmup_delay_original_minimum_ = nullptr;
    }
    for (const auto &setting : loadout_cvars_) {
        if (!setting.available) continue;
        ConVarRefAbstract cvar(setting.name, true);
        if (cvar.IsValidRef() && cvar.IsConVarDataAvailable())
            cvar.SetString(CUtlString(setting.original.c_str()));
    }
    game_cvars_overridden_ = false;
}

void ShroudfirePlugin::broadcast_discord_invite() {
    discord_hint_pending_at_ = 0.0f;
    if (!mode_.active() || !sf_discord_message.Get() || sf_discord_link.Get().IsEmpty()) return;
    last_discord_message_ = std::chrono::steady_clock::now();
    std::string message = kChatPrefix;
    message += "Join our discord server ";
    message += kChatGold;
    message += sf_discord_link.Get().String();
    message += kChatDefault;
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        const auto &player = mode_.player(slot);
        if (player.connected && !player.bot)
            engine_adapter_.client_print(slot, 3, message.c_str());
    }
}

void ShroudfirePlugin::sample_player_activity(float now) {
    const bool active = mode_.active();
    struct PendingHint { float *due; const char *message; bool enabled; };
    const PendingHint hints[] = {
        {&audio_hint_pending_at_, kAudioChatHint, sf_audio_hint.Get() != 0},
        {&cheat_hint_pending_at_, kCheatChatHint, sf_cheat_hint.Get() != 0},
        {&upgrade_hint_pending_at_, kUpgradeChatHint, sf_upgrade_hint.Get() != 0},
        {&bugs_hint_pending_at_, kBugsChatHint, sf_bugs_hint.Get() != 0},
        {&menu_hint_pending_at_, kMenuChatHint, sf_menu_hint.Get() != 0},
        {&reset_hint_pending_at_, kResetChatHint, sf_reset_hint.Get() != 0},
        {&rank_hint_pending_at_, kRankChatHint, sf_rank_hint.Get() != 0}
    };
    for (const auto &hint : hints) {
        if (!hint.enabled) {
            *hint.due = 0.0f;
            continue;
        }
        if (*hint.due <= 0.0f || now < *hint.due) continue;
        *hint.due = 0.0f;
        if (!active) continue;
        for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer) {
            const auto &recipient = mode_.player(viewer);
            if (recipient.connected && !recipient.bot)
                engine_adapter_.client_print(viewer, 3, hint.message);
        }
    }
    if (discord_hint_pending_at_ > 0.0f && now >= discord_hint_pending_at_)
        broadcast_discord_invite();
    if (!active) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
            if (menu_open_[slot]) close_menu(slot);
    }
    INetworkGameServer *game = network_service_ ? network_service_->GetIGameServer() : nullptr;
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        const auto &player = mode_.player(slot);
        if (!player.connected || player.bot) continue;
        if (guide_hint_due_[slot] > 0.0f && now >= guide_hint_due_[slot]) {
            if (mode_.active() && player.role != shroudfire::Role::spectator) {
                engine_adapter_.client_print(slot, 3, kGuideJoinHint);
                guide_hint_due_[slot] = -1.0f;
            } else guide_hint_due_[slot] = 0.0f;
        }
        if (guide_open_[slot]) render_guide(slot);
        else if (menu_open_[slot]) {
            if (player.role != menu_role_[slot]) close_menu(slot);
            else render_menu(slot);
        }
        EngineAdapter::Activity current;
        uint64_t menu_pressed = 0;
        if (player.alive) {
            if (!engine_adapter_.sample_activity(slot, current)) continue;
            auto &input = panel_menu_input_[slot];
            menu_pressed = input.take_pressed();
            if (active) {
                const bool was_camping_warned = player.camping_warned;
                mode_.movement_sample(slot, current.position[0], current.position[1],
                                      current.position[2], now);
                if (was_camping_warned && !mode_.player(slot).camping_warned)
                    update_hud(now);
            }
        } else {
            if (!menu_open_[slot] && !guide_open_[slot]) continue;
            void *services = engine_adapter_.observer_movement_services(slot);
            // Capture observer input before CS2 consumes its camera-control buttons.
            auto **vtable = services ? *reinterpret_cast<void ***>(services) : nullptr;
            if (vtable && vtable[kPlayerRunCommandIndex] &&
                vtable[kPlayerRunCommandIndex] != player_run_command_target_ &&
                !observer_run_command_hook_) {
                observer_run_command_hook_ = std::make_unique<KHook::Function<void, void *, void *>>(
                    this, &ShroudfirePlugin::Hook_PlayerRunCommand, nullptr);
                observer_run_command_target_ = vtable[kPlayerRunCommandIndex];
                observer_run_command_hook_->Configure(observer_run_command_target_);
            }
            auto &input = panel_menu_input_[slot];
            if (services && input.services == services) current.buttons = input.buttons;
            menu_pressed = input.take_pressed();
            if (input.select_pending) {
                menu_pressed |= IN_JUMP;
                input.select_pending = false;
            }
        }
        if (!activity_sample_ready_[slot]) {
            activity_samples_[slot] = current;
            activity_sample_ready_[slot] = true;
            if (player.alive && player.role == shroudfire::Role::armitive) {
                engine_adapter_.record_buy_zone_spawn(slot, current.position);
            }
            if (!menu_pressed) continue;
        }
        const auto &previous = activity_samples_[slot];
        const uint64_t pressed = (current.buttons & ~previous.buttons) | menu_pressed;
        if (guide_open_[slot] && guide_kind_[slot] == GuideKind::audio) {
            auto &audio = audio_menus_[slot];
            if (pressed & IN_RELOAD) {
                audio.page = shroudfire::kAudioSettingsPages;
                audio.selection = audio.row_count() - 1;
                play_sound(slot, shroudfire::kMenuShortcutSound, shroudfire::AudioCategory::menu);
                render_guide(slot);
            } else if (pressed & IN_MOVELEFT) {
                audio.move(-1, true);
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
            } else if (pressed & IN_MOVERIGHT) {
                audio.move(1, true);
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
            } else if (pressed & IN_FORWARD) {
                audio.move(-1);
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
            } else if (pressed & IN_BACK) {
                audio.move(1);
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
            } else if (pressed & IN_JUMP) {
                if (audio.page < shroudfire::kAudioSettingsPages) {
                    const auto category = shroudfire::kAudioMenuCategories[
                        audio.page * shroudfire::kAudioRowsPerPage + audio.selection];
                    if (mode_.toggle_audio(slot, category)) {
                        play_sound(slot, mode_.audio_enabled(slot, category)
                            ? shroudfire::kAudioToggleOnSound : shroudfire::kAudioToggleOffSound,
                            shroudfire::AudioCategory::menu);
                        if (profiles_ &&
                            !profiles_->save(mode_.profile(slot, static_cast<int>(std::time(nullptr)))))
                            engine_adapter_.client_print(slot, 3,
                                " \x04[Shroudfire]\x01 Audio preference changed, but could not be saved.");
                    }
                } else if (audio.selection == audio.row_count() - 1) {
                    play_sound(slot, shroudfire::kMenuExitSound, shroudfire::AudioCategory::menu);
                    close_guide(slot);
                } else {
                    play_sound(slot, shroudfire::kMenuOptionSound, shroudfire::AudioCategory::menu);
                    if (audio.selection == 0) show_guide(slot, GuideKind::help);
                    else if (audio.selection == 1) show_guide(slot, GuideKind::cheat);
                    else if (audio.selection == 2) show_guide(slot, GuideKind::known_bugs);
                    else if (audio.selection == 3) show_guide(slot, GuideKind::upgrades);
                    else show_menu(slot);
                }
            }
            render_guide(slot);
        } else if (guide_open_[slot]) {
            if (guide_kind_[slot] == GuideKind::rank &&
                (pressed & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK |
                    IN_JUMP | IN_RELOAD)))
                guide_last_html_[slot].clear();
            const int content_pages = static_cast<int>(guide_kind_[slot] == GuideKind::rank ? 2
                : guide_kind_[slot] == GuideKind::cheat
                ? kCheatPages.size() : guide_kind_[slot] == GuideKind::upgrades
                ? kUpgradePages.size() : guide_kind_[slot] == GuideKind::known_bugs
                ? kKnownBugPages.size() : kHelpPages.size());
            const int page_count = content_pages + 1;
            const int option_count = 6;
            if (pressed & IN_RELOAD) {
                guide_page_[slot] = content_pages;
                guide_option_selection_[slot] = option_count - 1;
                play_sound(slot, shroudfire::kMenuShortcutSound, shroudfire::AudioCategory::menu);
                render_guide(slot);
            } else if ((pressed & IN_MOVELEFT) ||
                ((pressed & IN_FORWARD) &&
                 (guide_page_[slot] != content_pages || guide_option_selection_[slot] == 0))) {
                guide_page_[slot] = (guide_page_[slot] + page_count - 1) % page_count;
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_guide(slot);
            } else if ((pressed & IN_MOVERIGHT) ||
                       ((pressed & IN_BACK) &&
                        (guide_page_[slot] != content_pages ||
                         guide_option_selection_[slot] == option_count - 1))) {
                guide_page_[slot] = (guide_page_[slot] + 1) % page_count;
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_guide(slot);
            } else if (guide_page_[slot] == content_pages && (pressed & IN_FORWARD)) {
                guide_option_selection_[slot] =
                    (guide_option_selection_[slot] + option_count - 1) % option_count;
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_guide(slot);
            } else if (guide_page_[slot] == content_pages && (pressed & IN_BACK)) {
                guide_option_selection_[slot] =
                    (guide_option_selection_[slot] + 1) % option_count;
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_guide(slot);
            } else if (pressed & IN_JUMP) {
                if (guide_page_[slot] != content_pages) {
                    guide_page_[slot] = content_pages;
                    guide_option_selection_[slot] = 0;
                    play_sound(slot, shroudfire::kMenuShortcutSound, shroudfire::AudioCategory::menu);
                    render_guide(slot);
                } else {
                    const int selected = guide_option_selection_[slot];
                    if (selected == option_count - 1) {
                        play_sound(slot, shroudfire::kMenuExitSound, shroudfire::AudioCategory::menu);
                        close_guide(slot);
                    } else {
                        play_sound(slot, shroudfire::kMenuOptionSound, shroudfire::AudioCategory::menu);
                        if (guide_kind_[slot] == GuideKind::help) {
                            if (selected == 0) show_guide(slot, GuideKind::cheat);
                            else if (selected == 1) show_guide(slot, GuideKind::known_bugs);
                            else if (selected == 2) show_guide(slot, GuideKind::upgrades);
                            else if (selected == 3) show_menu(slot);
                            else show_audio_settings(slot);
                        } else if (guide_kind_[slot] == GuideKind::upgrades) {
                            if (selected == 0) show_guide(slot, GuideKind::help);
                            else if (selected == 1) show_guide(slot, GuideKind::cheat);
                            else if (selected == 2) show_guide(slot, GuideKind::known_bugs);
                            else if (selected == 3) show_menu(slot);
                            else show_audio_settings(slot);
                        } else if (guide_kind_[slot] == GuideKind::cheat) {
                            if (selected == 0) show_guide(slot, GuideKind::help);
                            else if (selected == 1) show_guide(slot, GuideKind::known_bugs);
                            else if (selected == 2) show_guide(slot, GuideKind::upgrades);
                            else if (selected == 3) show_menu(slot);
                            else show_audio_settings(slot);
                        } else {
                            if (selected == 0) show_guide(slot, GuideKind::help);
                            else if (selected == 1) show_guide(slot, GuideKind::cheat);
                            else if (selected == 2) show_guide(slot, GuideKind::upgrades);
                            else if (selected == 3) show_menu(slot);
                            else show_audio_settings(slot);
                        }
                    }
                }
            }
        } else if (menu_open_[slot]) {
            const int page = menu_selection_[slot] < 8 ? menu_selection_[slot] / 4 : 2;
            const int row = menu_selection_[slot] - page * 4;
            const int row_count = page == 2 ? 5 : 4;
            if (pressed & IN_RELOAD) {
                menu_selection_[slot] = 12;
                play_sound(slot, shroudfire::kMenuShortcutSound, shroudfire::AudioCategory::menu);
                render_menu(slot);
            } else if (pressed & IN_MOVELEFT) {
                const int next_page = (page + 2) % 3;
                menu_selection_[slot] = next_page * 4 + (std::min)(row, next_page == 2 ? 4 : 3);
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_menu(slot);
            } else if (pressed & IN_MOVERIGHT) {
                const int next_page = (page + 1) % 3;
                menu_selection_[slot] = next_page * 4 + (std::min)(row, next_page == 2 ? 4 : 3);
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_menu(slot);
            } else if (pressed & IN_FORWARD) {
                if (row == 0) {
                    const int previous_page = (page + 2) % 3;
                    menu_selection_[slot] = previous_page * 4 + (previous_page == 2 ? 4 : 3);
                } else menu_selection_[slot] = page * 4 + row - 1;
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_menu(slot);
            } else if (pressed & IN_BACK) {
                if (row == row_count - 1) {
                    const int next_page = (page + 1) % 3;
                    menu_selection_[slot] = next_page * 4;
                } else menu_selection_[slot] = page * 4 + row + 1;
                play_sound(slot, shroudfire::kMenuNavigateSound, shroudfire::AudioCategory::menu);
                render_menu(slot);
            } else if (pressed & IN_JUMP) {
                const int selected = menu_selection_[slot];
                if (selected < 8) {
                    const char choice[] = {static_cast<char>('1' + selected), '\0'};
                    handle_chat_command(slot, choice, false);
                } else if (selected < 11) {
                    play_sound(slot, shroudfire::kMenuOptionSound, shroudfire::AudioCategory::menu);
                    if (selected == 8) show_guide(slot, GuideKind::rank);
                    else if (selected == 9) show_guide(slot, GuideKind::known_bugs);
                    else show_guide(slot, GuideKind::upgrades);
                } else if (selected == 11) handle_chat_command(slot, "reset", false);
                else handle_chat_command(slot, "close", false);
            }
        }
        if (!active) {
            activity_samples_[slot] = current;
            continue;
        }
        const bool use_pressed = (current.buttons & (uint64_t{1} << 5)) != 0 &&
            (previous.buttons & (uint64_t{1} << 5)) == 0;
        const float dx = current.position[0] - previous.position[0];
        const float dy = current.position[1] - previous.position[1];
        const float dz = current.position[2] - previous.position[2];
        const float pitch = std::fabs(current.angles[0] - previous.angles[0]);
        const float yaw = std::fabs(current.angles[1] - previous.angles[1]);
        const bool moved = player.alive && dx * dx + dy * dy + dz * dz >= 16.0f;
        const bool moved_by_player = moved &&
            ((current.buttons | previous.buttons) &
             (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT | IN_JUMP)) != 0;
        const bool looked = (std::min)(pitch, 360.0f - pitch) >= 3.0f ||
            (std::min)(yaw, 360.0f - yaw) >= 3.0f;
        if (player.alive && (moved || looked || current.buttons != previous.buttons)) {
            const bool was_afk = player.afk_warned || player.afk_queue_removed;
            mode_.activity(slot, now, moved_by_player);
            activity_samples_[slot] = current;
            if (was_afk) update_hud(now);
        } else if (!player.alive) {
            // Dead panels need the previous frame's buttons too: a held key
            // must not count as another press until it has been released.
            activity_samples_[slot] = current;
        }
        if (player.alive && player.role == shroudfire::Role::invisant) {
            const auto attributes = mode_.attributes(slot);
            engine_adapter_.suppress_buy_zone(slot);
            engine_adapter_.set_invisant_speed(slot, attributes.movement_scale);
        } else if (player.alive && player.role == shroudfire::Role::armitive) {
            engine_adapter_.set_invisant_speed(slot, 1.0f);
            engine_adapter_.apply_ammo_upgrades(slot,
                player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::overload)],
                player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::stockpile)],
                mode_.settings().overload_magazine_percent,
                mode_.settings().stockpile_clips_per_stage, now,
                mode_.attributes(slot).reload_delay_scale);
            if (game) engine_adapter_.apply_dexterity(slot,
                player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::dexterity)],
                mode_.settings().dexterity_speed_percent, now, game->GetServerTick());
        }
        bool claw_attacked = false;
        engine_adapter_.apply_claw_timing(slot, now,
            game ? game->GetServerTick() : 0,
            game && player.alive && player.role == shroudfire::Role::invisant,
            sf_knife_attack_speed_percent.Get(),
            &claw_attacked);
        if (claw_attacked && mode_.round_live() && mode_.freeze_ended()) {
            int victim = 0;
            if (engine_adapter_.claw_sweep(slot, 5 - sf_invisant_team.Get(),
                    sf_knife_range.Get(), sf_knife_cone.Get(), victim) && victim > 0) {
                const auto &target = mode_.player(victim);
                if (target.connected && target.alive &&
                    target.role == shroudfire::Role::armitive)
                    pending_claw_hits_[slot] = {victim, game->GetServerTick(),
                                                 player.steam_id, target.steam_id};
            }
        }
        if (use_pressed && player.alive) {
            if (player.role == shroudfire::Role::armitive &&
                engine_adapter_.can_buy(slot, 5 - sf_invisant_team.Get()))
                continue;
            if (player.role == shroudfire::Role::invisant &&
                player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::leap)] > 0 &&
                mode_.round_live() && mode_.freeze_ended() &&
                player.leap_available_at <= now) {
                bool clear = false;
                if (!engine_adapter_.leap_clear(slot, clear) || !clear) {
                    engine_adapter_.client_print(slot, 4, "Surge blocked");
                    play_sound(slot, "sounds/buttons/blip2.vsnd");
                    continue;
                }
            }
            if (mode_.use_ability(slot, now)) {
                center_notices_[slot].ability_until = 0.0f;
                process_actions();
                update_hud(now);
            } else {
                const bool has_ability = player.role == shroudfire::Role::invisant
                    ? player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::leap)] > 0
                    : player.role == shroudfire::Role::armitive &&
                      player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::shield)] > 0;
                if (has_ability && mode_.round_live()) {
                    if (!mode_.freeze_ended())
                        engine_adapter_.client_print(slot, 4, "Ability unavailable until freeze time ends");
                    else {
                        const float available = player.role == shroudfire::Role::invisant
                            ? player.leap_available_at : player.shield_available_at;
                        if (available > now && !(player.role == shroudfire::Role::armitive &&
                            mode_.shield_active(slot, now))) {
                            CenterNotice &notice = center_notices_[slot];
                            notice.merge(CenterNoticeKind::ability, now, 5.0f);
                            notice.ability_role = player.role;
                            notice.ability_available_at = available;
                            update_hud(now);
                            play_sound(slot, "sounds/buttons/blip2.vsnd");
                        }
                    }
                }
            }
        }
    }
}

void ShroudfirePlugin::refresh_local_movement_sounds(float now) {
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        const auto &player = mode_.player(slot);
        const bool enabled = mode_.active() && sf_silent_local_movement.Get() &&
            player.connected && !player.bot && player.alive &&
            player.role == shroudfire::Role::invisant;
        if (!enabled) {
            local_movement_next_refresh_at_[slot] = 0.0f;
            continue;
        }
        if (now < local_movement_next_refresh_at_[slot]) continue;
        const uint64_t recipient = uint64_t {1} << (slot - 1);
        for (const char *event : kLocalMovementBlockerEvents)
            engine_adapter_.emit_sound_event(slot, recipient, event);
        // `block_other_duration` lasts 0.8 seconds in the Workshop event.
        // Refresh earlier so packet jitter cannot open an audible gap.
        local_movement_next_refresh_at_[slot] = now + 0.45f;
    }
}

void ShroudfirePlugin::play_sound(int slot, const char *sound_name,
    shroudfire::AudioCategory category) const {
    if (!engine_ || slot < 1 || slot > shroudfire::kMaxPlayers ||
        !mode_.player(slot).connected || mode_.player(slot).bot || !sound_name)
        return;
    if (category != shroudfire::AudioCategory::count && !mode_.audio_enabled(slot, category)) return;
    engine_->ClientCommand(CPlayerSlot(slot - 1), "play %s", sound_name);
}

void ShroudfirePlugin::clear_armitive_glows() {
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        glow_relay_indices_[slot].store(0, std::memory_order_release);
        glow_prop_indices_[slot].store(0, std::memory_order_release);
        ward_relay_indices_[slot].store(0, std::memory_order_release);
        ward_prop_indices_[slot].store(0, std::memory_order_release);
        if (armitive_glows_[slot].relay || armitive_glows_[slot].glow)
            engine_adapter_.remove_armitive_glow(armitive_glows_[slot]);
        if (ward_glows_[slot].relay || ward_glows_[slot].glow)
            engine_adapter_.remove_armitive_glow(ward_glows_[slot]);
    }
    glow_count_.store(0, std::memory_order_release);
    glow_viewer_mask_.store(0, std::memory_order_release);
    ward_viewer_mask_.store(0, std::memory_order_release);
    shield_glow_mask_.store(0, std::memory_order_release);
}

void ShroudfirePlugin::refresh_armitive_glows(float now) {
    const bool tracking_allowed = sf_wallhack.Get() && mode_.active() &&
        mode_.round_live() && mode_.freeze_ended();
    uint64_t viewers = 0;
    uint64_t ward_viewers = 0;
    if (mode_.active() && mode_.round_live()) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &viewer = mode_.player(slot);
            if (!viewer.connected || viewer.bot || !viewer.alive) continue;
            if (tracking_allowed && viewer.role == shroudfire::Role::invisant)
                viewers |= uint64_t{1} << (slot - 1);
            else if (viewer.role == shroudfire::Role::armitive)
                ward_viewers |= uint64_t{1} << (slot - 1);
        }
    }
    glow_viewer_mask_.store(viewers, std::memory_order_release);
    ward_viewer_mask_.store(ward_viewers, std::memory_order_release);
    if (now - last_glow_refresh_ < 0.5f) return;
    last_glow_refresh_ = now;
    uint64_t shielded_targets = 0;
    if (mode_.active() && mode_.round_live()) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
            if (mode_.shield_active(slot, now))
                shielded_targets |= uint64_t {1} << (slot - 1);
    }
    shield_glow_mask_.store(shielded_targets, std::memory_order_release);
    if (viewers || (ward_viewers && shielded_targets)) {
        glow_count_.store(1, std::memory_order_release);
    }
    int count = 0;
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        const auto &target = mode_.player(slot);
        const bool shielded = (shielded_targets & (uint64_t {1} << (slot - 1))) != 0;
        const bool armitive = target.connected && target.alive &&
            target.role == shroudfire::Role::armitive;
        if (armitive && viewers && engine_adapter_.update_armitive_glow(
                slot, shielded, true, armitive_glows_[slot])) {
            glow_relay_indices_[slot].store(armitive_glows_[slot].relay_index,
                                            std::memory_order_release);
            glow_prop_indices_[slot].store(armitive_glows_[slot].glow_index,
                                           std::memory_order_release);
            ++count;
        } else {
            glow_relay_indices_[slot].store(0, std::memory_order_release);
            glow_prop_indices_[slot].store(0, std::memory_order_release);
            if (armitive_glows_[slot].relay || armitive_glows_[slot].glow)
                engine_adapter_.remove_armitive_glow(armitive_glows_[slot]);
        }
        const uint64_t other_armitive = ward_viewers & ~(uint64_t{1} << (slot - 1));
        if (armitive && shielded && other_armitive && engine_adapter_.update_armitive_glow(
                slot, true, false, ward_glows_[slot])) {
            ward_relay_indices_[slot].store(ward_glows_[slot].relay_index,
                                            std::memory_order_release);
            ward_prop_indices_[slot].store(ward_glows_[slot].glow_index,
                                           std::memory_order_release);
            ++count;
        } else {
            ward_relay_indices_[slot].store(0, std::memory_order_release);
            ward_prop_indices_[slot].store(0, std::memory_order_release);
            if (ward_glows_[slot].relay || ward_glows_[slot].glow)
                engine_adapter_.remove_armitive_glow(ward_glows_[slot]);
        }
    }
    glow_count_.store(count, std::memory_order_release);
}

void ShroudfirePlugin::update_hud(float now) {
    if (!sf_hud.Get() || !mode_.active()) return;
    int connected_players = 0;
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
        connected_players += mode_.player(slot).connected;
    const bool round_end_queued = std::any_of(pending_engine_actions_.begin(),
        pending_engine_actions_.end(), [](const PendingEngineAction &pending) {
            return pending.action.effect == shroudfire::Effect::end_round;
        });
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        const auto &p = mode_.player(slot);
        if (!p.connected || p.bot || menu_open_[slot] || guide_open_[slot]) continue;
        CenterNotice &notice = center_notices_[slot];
        if (!p.alive || p.waiting_next_round || p.role != notice.ability_role ||
            p.applied_upgrades[static_cast<int>(p.role == shroudfire::Role::invisant
                ? shroudfire::Upgrade::leap : shroudfire::Upgrade::shield)] == 0)
            notice.ability_until = 0.0f;
        const bool notice_active = notice.kind != CenterNoticeKind::none && now < notice.until;
        // The native round-end event adds the result to a final kill's panel.
        if (notice_active && notice.kind != CenterNoticeKind::win &&
            !notice.combat_rewards.empty() && round_end_queued) continue;
        const int afk_seconds = mode_.afk_countdown(slot, now);
        const float afk_at = p.last_activity + mode_.settings().afk_seconds - afk_seconds;
        const bool starving = p.alive && p.role == shroudfire::Role::invisant &&
            p.starvation_active && mode_.round_live() && mode_.freeze_ended() &&
            mode_.active_players() >= 2 && mode_.alive(shroudfire::Role::armitive) > 0;
        const bool camping = p.alive && p.role == shroudfire::Role::armitive &&
            !p.afk_death_pending && afk_seconds <= 0 && p.camping_warned &&
            mode_.round_live() && mode_.freeze_ended() &&
            mode_.active_players() >= 2;
        const int camping_damage_seconds = camping && !p.camping_active
            ? (std::max)(1, static_cast<int>(std::ceil(p.camping_anchor_at +
                mode_.settings().camping_warning_seconds +
                mode_.settings().camping_damage_delay - now))) : 0;
        CenterPanel panel = notice.panel(now, afk_seconds, afk_at, starving,
            p.last_starvation_tick_at, camping, camping_damage_seconds,
            static_cast<int>(std::lround(mode_.settings().camping_distance)),
            p.camping_anchor_at + mode_.settings().camping_warning_seconds);
        const bool round_info_active = p.role != shroudfire::Role::spectator && now < round_info_until_;
        if (round_info_active) {
            const bool invisant_role = p.role == shroudfire::Role::invisant;
            const int xp = p.xp[invisant_role ? 0 : 1];
            const int level = mode_.level(xp);
            char xp_text[48];
            if (level >= shroudfire::kMaxLevel)
                std::snprintf(xp_text, sizeof(xp_text), "%d", xp);
            else {
                const int start = mode_.required_xp(level);
                const int next = mode_.required_xp(level + 1);
                std::snprintf(xp_text, sizeof(xp_text), "%d/%d", xp - start, next - start);
            }
            const char *accent = invisant_role ? "#78b9ff" : "#ff7169";
            char line[256];
            std::snprintf(line, sizeof(line),
                "<b><font color='%s'>%s</font></b><br>"
                "<font color='%s'>Level</font> <font color='#ffffff'>%d</font> "
                "<font color='%s'>XP</font> <font color='#ffffff'>%s</font>",
                accent, invisant_role ? "INVISANT" : "ARMITIVE",
                accent, level, accent, xp_text);
            std::string role_html = line;
            std::string role_info;
            if (invisant_role) {
                role_info += "<br><font color='#ffb900'>Invisible, silent, knife-only</font>"
                    "<br><font color='#ffb900'>Eliminate all </font>"
                    "<font color='#ff7169'>Armitive</font>"
                    "<font color='#ffb900'> to win</font>";
            } else {
                char queue_text[32];
                const int queue = mode_.queue_position(slot, now);
                const bool afk_queue = p.afk_queue_removed ||
                    (mode_.settings().afk_seconds > 0 &&
                     now - p.last_activity >= mode_.settings().afk_seconds);
                if (queue > 0) std::snprintf(queue_text, sizeof(queue_text), "%d", queue);
                else std::snprintf(queue_text, sizeof(queue_text), "%s",
                    afk_queue ? "AFK" : "Not queued");
                std::snprintf(line, sizeof(line),
                    "<br><font color='#78b9ff'>Invisant</font> "
                    "<font color='#b7d1db'>queue</font> "
                    "<font color='%s'>%s</font>",
                    afk_queue ? "#b9a176" : "#ffffff", queue_text);
                role_info += line;
                role_info += "<br><font color='#ffb900'>Use guns; look for blood</font>"
                    "<br><font color='#ffb900'>Kill the </font>"
                    "<font color='#78b9ff'>Invisant</font>"
                    "<font color='#ffb900'> or survive</font>";
            }
            role_html += role_info;
            size_t at = 0;
            int row = 0;
            while (at < role_html.size()) {
                const size_t end = role_html.find("<br>", at);
                panel.lines.push_back({role_html.substr(at,
                    end == std::string::npos ? std::string::npos : end - at), round_info_at_,
                    row == 0 ? CenterLinePriority::result : row == 1
                        ? CenterLinePriority::xp : CenterLinePriority::info});
                if (end == std::string::npos) break;
                at = end + 4;
                ++row;
            }
        }
        if (p.alive && p.role == shroudfire::Role::invisant &&
            mode_.round_live() && mode_.freeze_ended() &&
            !mode_.settings().timeout_kill_invisant && mode_.active_players() >= 2 &&
            sf_timeout_warning_seconds.Get() > 0 &&
            mode_.round_time_remaining(now) <= sf_timeout_warning_seconds.Get()) {
            if (timeout_warning_at_ <= 0.0f) timeout_warning_at_ = now;
            const std::string warning = kInvisibilityWarningHtml;
            const size_t split = warning.find("<br>");
            panel.lines.push_back({warning.substr(0, split), timeout_warning_at_, CenterLinePriority::info});
            panel.lines.push_back({warning.substr(split + 4), timeout_warning_at_, CenterLinePriority::info});
        }
        const std::string html = panel.html(connected_players == 1);
        if (!html.empty()) {
            const bool sent = engine_adapter_.center_html(slot, game_events_, html.c_str(), 2);
            auto_hud_visible_[slot] = sent;
            if (notice_active) {
                notice.visible = sent;
                notice.refresh_at = notice.next_expiry(now);
                if (round_info_active)
                    notice.refresh_at = (std::min)(notice.refresh_at, round_info_until_);
            }
            if (round_info_active) round_info_sent_[slot] = sent;
        } else if (notice_active || auto_hud_visible_[slot]) {
            notice.visible = engine_adapter_.center_html(slot, game_events_, " ", -1);
            auto_hud_visible_[slot] = false;
            if (notice_active) notice.refresh_at = notice.next_expiry(now);
        }
        if (mode_.shield_active(slot, now))
            engine_adapter_.hud_message(slot, 2, -1.0f, 0.15f, 1.2f,
                120, 175, 255, 255, "SHIELD ACTIVE");
    }
}

int ShroudfirePlugin::event_player(IGameEvent *event, const char *key) const {
    const CPlayerSlot slot = event->GetPlayerSlot(event_key(key));
    return slot.IsValid() && slot.Get() < shroudfire::kMaxPlayers ? slot.Get() + 1 : 0;
}

KHook::Return<bool> ShroudfirePlugin::Hook_FireGameEvent(IGameEventManager2 *manager,
    IGameEvent *event, bool dont_broadcast) {
    if (event && mode_.active() &&
        std::strcmp(event->GetName(), "player_team") == 0)
        event->SetBool(event_key("silent"), true);
    FireGameEvent(event);
    if (event && !dont_broadcast && mode_.active()) {
        const char *name = event->GetName();
        if (std::strcmp(name, "round_mvp") == 0 ||
            std::strcmp(name, "cs_win_panel_round") == 0 ||
            std::strcmp(name, "round_end") == 0) {
            // Server listeners process these events, while the client receives
            // the custom result panel instead of the stock round summary.
            return {KHook::Action::Supersede,
                game_event_fire_.CallOriginal(manager, event, true)};
        }
    }
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_GameEventManagerInit(IGameEventManager2 *manager) {
    game_events_ = manager;
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_GameEventManagerInitialized(IGameEventManager2 *manager) {
    if (manager && game_events_ == manager) {
        game_event_fire_.Add(manager);
        META_CONPRINTF("[Shroudfire] CS2 game event manager initialized.\n");
    }
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_HostSay(CEntityInstance *controller, CCommand &args,
    bool team_only, int, const char *) {
    // KHook runs the post callback even when a command is superseded. Pair every
    // invocation, including handled menu commands and nested native broadcasts.
    chat_routing_.begin(controller ? controller->GetEntityIndex().Get() : 0, team_only);
    if (controller && args.ArgC() > 1) {
        const int slot = controller->GetEntityIndex().Get();
        if (slot >= 1 && slot <= shroudfire::kMaxPlayers) {
            if (handle_chat_command(slot, args.Arg(1)))
                return {KHook::Action::Supersede};
        }
    }
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_HostSayPost(CEntityInstance *, CCommand &,
    bool, int, const char *) {
    chat_routing_.end();
    return {KHook::Action::Ignore};
}

void ShroudfirePlugin::FireGameEvent(IGameEvent *event) {
    if (!event) return;
    sync_settings();
    const char *name = event->GetName();
    const float now = game_time();
    if (std::strcmp(name, "round_start") == 0) {
        if (map_rotation_.change_at > 0.0f && mode_.active() && sf_enable.Get() != 0 &&
            ((sf_map_max_rounds.Get() > 0 && map_rotation_.rounds >= sf_map_max_rounds.Get()) ||
             (sf_map_time_limit.Get() > 0 &&
              map_rotation_.played_seconds >= sf_map_time_limit.Get() * 60.0))) {
            const std::string destination = next_map();
            if (!destination.empty()) {
                // Finish the pending change before CS2 starts another playable round.
                map_rotation_.pending = destination;
                map_rotation_.change_at = now;
                return;
            }
            // Keep gameplay running if the configured destination disappeared.
            map_rotation_.change_at = 0.0f;
            map_rotation_.pending.clear();
        }
        engine_adapter_.restore_reveal_rendering();
        timeout_reveal_ready_mask_ = 0;
        timeout_reveal_last_alpha_.fill(-1);
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
            engine_adapter_.hide_win_layout(slot);
        pending_round_end_at_ = 0.0f;
        pending_round_winner_ = 0;
        round_timeout_requested_ = false;
        draw_termination_requested_ = false;
        round_voice_at_ = 0.0f;
        round_win_voice_at_ = 0.0f;
        round_win_voice_role_ = shroudfire::Role::spectator;
        center_notices_.fill({});
        timeout_warning_at_ = 0.0f;
        timeout_warning_voice_played_ = false;
        round_info_sent_.fill(false);
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            if (menu_open_[slot]) {
                menu_last_html_[slot].clear();
                menu_first_send_at_[slot] = now + 0.2f;
            }
            if (guide_open_[slot]) {
                guide_last_html_[slot].clear();
                guide_first_send_at_[slot] = now + 0.2f;
            }
        }
        objectives_removed_ = false;
        engine_adapter_.reset_round_ammo_tracking();
        mode_.start_round(now);
        // A playable native round completes first-join recovery. Warmup
        // round starts must still let the first player end warmup.
        if (!engine_adapter_.warmup_active().value_or(false)) {
            pending_initial_restart_ = 0.0f;
            initial_restart_attempts_ = 0;
        }
#if defined(__linux__)
        if (solo_rejoin_freeze_pending_) {
            solo_rejoin_freeze_pending_ = false;
            solo_rejoin_freeze_until_ = now + 5.0f;
            engine_adapter_.set_freeze_period(true);
        }
#endif
        round_info_at_ = now;
        round_info_until_ = mode_.active() ? now + sf_round_info_seconds.Get() : 0.0f;
        if (mode_.active()) {
            round_start_voice_stop_next_at_ = now;
            round_start_voice_stop_passes_ = 1;
        }
    }
    else if (std::strcmp(name, "round_freeze_end") == 0) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
            activity_sample_ready_[slot] = false;
        if (solo_rejoin_freeze_until_ == 0.0f || now >= solo_rejoin_freeze_until_)
            mode_.freeze_end(now);
        if (mode_.active()) {
            round_start_voice_stop_next_at_ = now;
            round_start_voice_stop_passes_ = 1;
            if (sf_custom_sounds.Get() && mode_.round_live() &&
                mode_.active_players() >= 2)
                round_voice_at_ = now + 0.25f;
        }
    }
    else if (std::strcmp(name, "round_end") == 0) {
        pending_round_end_at_ = 0.0f;
        pending_round_winner_ = 0;
        round_voice_at_ = 0.0f;
        // The client result events and replicated end count are suppressed so
        // the custom WIN layout can remain visible through the real round end.
        round_start_voice_stop_passes_ = 0;
        const bool self_wipe_draw = std::any_of(pending_engine_actions_.begin(),
            pending_engine_actions_.end(), [](const PendingEngineAction &pending) {
                return pending.action.effect == shroudfire::Effect::end_round &&
                    pending.action.role == shroudfire::Role::spectator;
            });
        if (self_wipe_draw) event->SetInt(event_key("winner"), 0);
        const int winner = event->GetInt(event_key("winner"));
        if (mode_.active()) {
            if (winner == 0) {
                if (now >= restart_draw_suppression_until_)
                    round_draw_stop_until_ = now + 1.0f;
            }
            else {
                round_win_stop_next_at_ = now + 0.45f;
                round_win_stop_passes_ = 12;
            }
        }
        restore_shields();
        const shroudfire::Role role = winner == sf_invisant_team.Get()
            ? shroudfire::Role::invisant : winner == (5 - sf_invisant_team.Get())
            ? shroudfire::Role::armitive : shroudfire::Role::spectator;
        if (mode_.active() && mode_.round_live() &&
            mode_.active_players() >= 2 && role != shroudfire::Role::spectator) {
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                const auto &player = mode_.player(slot);
                if (!player.connected || player.bot) continue;
                CenterNotice &notice = center_notices_[slot];
                notice.merge(CenterNoticeKind::win, now, sf_hud_round_result_seconds.Get());
                notice.winner = role;
            }
        }
        if (sf_custom_sounds.Get() && mode_.active() && mode_.round_live() &&
            mode_.active_players() >= 2 &&
            (role != shroudfire::Role::spectator || self_wipe_draw) &&
            round_win_voice_at_ <= 0.0f) {
            round_win_voice_at_ = now + sf_round_win_sound_delay.Get();
            round_win_voice_role_ = role;
        }
        const bool timed_out = role == shroudfire::Role::armitive &&
            mode_.round_live() && mode_.freeze_ended() &&
            (round_timeout_requested_ || mode_.round_time_remaining(now) == 0);
        mode_.end_round(role, role != shroudfire::Role::spectator || self_wipe_draw, timed_out, now);
        round_info_until_ = 0.0f;
    } else if (std::strcmp(name, "player_spawn") == 0) {
        const int slot = event_player(event, "userid");
        if (slot) activity_sample_ready_[slot] = false;
        if (slot) panel_menu_input_[slot] = {};
        if (slot) engine_adapter_.reset_player_buy_zone(slot);
        if (slot) {
            shield_next_sound_at_[slot] = 0.0f;
            shield_tint_alpha_[slot] = 0;
        }
        if (slot) role_respawn_pending_[slot] = false;
        if (slot && mode_.active_players() == 1) initial_restart_attempts_ = 0;
        if (slot && shield_applied_[slot]) {
            bool ignored = false;
            engine_adapter_.set_damage_enabled(slot, shield_previous_damage_[slot], ignored);
            engine_adapter_.screen_fade(slot, 0, 17, 0, 0, 0, 0);
            shield_applied_[slot] = false;
        }
        mode_.spawn(slot, now);
        if (slot) forced_death_pending_[slot] = false;
        if (mode_.active() && slot > 0 && mode_.player(slot).connected)
            pending_spawns_.push_back({slot, mode_.player(slot).steam_id, now + 0.1f});
    } else if (std::strcmp(name, "player_hurt") == 0) {
        const int victim = event_player(event, "userid");
        const int attacker = event_player(event, "attacker");
        const int health_damage = event->GetInt(event_key("dmg_health"));
        if (victim && mode_.player(victim).bot && attacker &&
            mode_.player(attacker).role == shroudfire::Role::invisant)
            META_CONPRINTF("[Shroudfire] Bot knife hurt: health %d, armor %d, remaining %d, armor left %d.\n",
                health_damage, event->GetInt(event_key("dmg_armor")),
                engine_adapter_.player_health(victim), engine_adapter_.player_armor(victim));
        mode_.damage(attacker, victim, health_damage, now);
        // Actual incoming health/armor damage resets starvation, including
        // environmental damage. Starvation's own forced damage cannot feed it.
        const bool counts_for_starvation =
            (health_damage > 0 || event->GetInt(event_key("dmg_armor")) > 0) &&
            !(victim && forced_death_pending_[victim] &&
              forced_death_cause_[victim] == shroudfire::DeathCause::starvation);
        mode_.damage_taken(victim, now, counts_for_starvation);
        if (mode_.active() && victim && attacker && attacker != victim &&
            health_damage > 0 && mode_.player(victim).connected &&
            mode_.player(attacker).connected) {
            const auto &source = mode_.player(attacker);
            const auto &target = mode_.player(victim);
            const bool knife_hit = source.role == shroudfire::Role::invisant &&
                target.role == shroudfire::Role::armitive &&
                (claw_extended_applying_[attacker] ||
                 engine_adapter_.active_weapon_is_knife(attacker));
            if (knife_hit) {
                // Both stock knife contact and point_hurt arrive here only
                // after health damage. Misses and shielded hits get no impact.
                play_sound(attacker, "sounds/weapons/knife/knife_stab.vsnd");
                play_sound(victim, "sounds/weapons/knife/knife_stab.vsnd");
                uint64_t recipients = 0;
                for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                    const auto &viewer = mode_.player(slot);
                    if (viewer.connected && !viewer.bot && slot != attacker && slot != victim)
                        recipients |= uint64_t {1} << (slot - 1);
                }
                // Use the victim as the source so Invisant sound filtering preserves the impact.
                if (recipients) engine_adapter_.emit_sound_event(victim, recipients, "Weapon_Knife.Stab");
            }
            if (sf_blood_amount.Get() > 0 && (knife_hit ||
                (target.role == shroudfire::Role::invisant &&
                 source.role == shroudfire::Role::armitive && !source.bot))) {
                int particles_created = 0;
                const bool emitted = engine_adapter_.blood_burst(victim, attacker,
                    health_damage, event->GetInt(event_key("hitgroup")),
                    sf_blood_amount.Get(), now, &particles_created);
                if (target.bot)
                    META_CONPRINTF("[Shroudfire] Bot blood burst: victim %d, damage %d, emitted %d, particles %d.\n",
                        victim, health_damage, emitted ? 1 : 0, particles_created);
            }
        }
    } else if (std::strcmp(name, "weapon_fire") == 0) {
        const int slot = event_player(event, "userid");
        if (slot && mode_.player(slot).role == shroudfire::Role::armitive)
            engine_adapter_.record_weapon_fire(slot);
        mode_.weapon_fire(slot, now);
        if (slot && mode_.active() && mode_.player(slot).connected &&
            mode_.player(slot).role == shroudfire::Role::armitive &&
            mode_.attributes(slot).fire_delay_scale < 1.0f)
            pending_fire_acceleration_.push_back({slot, mode_.player(slot).steam_id,
                                                  now + 0.001f});
    } else if (std::strcmp(name, "player_death") == 0) {
        const int victim = event_player(event, "userid");
        if (victim) {
            activity_sample_ready_[victim] = false;
            panel_menu_input_[victim] = {};
        }
        if (victim) {
            shield_next_sound_at_[victim] = 0.0f;
            shield_tint_alpha_[victim] = 0;
        }
        if (victim && shield_applied_[victim]) {
            bool ignored = false;
            engine_adapter_.set_damage_enabled(victim, shield_previous_damage_[victim], ignored);
            engine_adapter_.screen_fade(victim, 0, 17, 0, 0, 0, 0);
            shield_applied_[victim] = false;
        }
        const int attacker = event_player(event, "attacker");
        const char *weapon = event->GetString(event_key("weapon"));
        if (attacker && attacker != victim && std::strcmp(weapon, "point_hurt") == 0 &&
            mode_.player(attacker).connected &&
            mode_.player(attacker).role == shroudfire::Role::invisant) {
            event->SetString(event_key("weapon"), "knife");
            weapon = "knife";
        }
        shroudfire::DeathCause cause = shroudfire::DeathCause::world;
        if (victim && forced_death_pending_[victim]) cause = forced_death_cause_[victim];
        else if (victim && suicide_command_[victim]) cause = shroudfire::DeathCause::suicide;
        else if (std::strcmp(weapon, "fall") == 0) cause = shroudfire::DeathCause::fall;
        else if (attacker && attacker != victim) cause = shroudfire::DeathCause::enemy;
        if (victim) {
            suicide_command_[victim] = false;
            forced_death_pending_[victim] = false;
        }
        const shroudfire::WeaponKind kind = std::strstr(weapon, "knife")
            ? shroudfire::WeaponKind::knife : pistol_weapon(weapon)
            ? shroudfire::WeaponKind::pistol : shroudfire::WeaponKind::other;
        if (victim && mode_.active() && !role_respawn_pending_[victim] &&
            (cause == shroudfire::DeathCause::afk ||
             cause == shroudfire::DeathCause::starvation ||
             cause == shroudfire::DeathCause::camping)) {
            std::string victim_name = player_names_[victim];
            victim_name.erase(std::remove_if(victim_name.begin(), victim_name.end(),
                [](unsigned char ch) { return ch < 0x20; }), victim_name.end());
            std::string message = " \x04[Shroudfire]\x01 ";
            message += mode_.player(victim).role == shroudfire::Role::invisant
                ? kChatBlue : kChatRed;
            message += victim_name;
            message += kChatDefault;
            message += cause == shroudfire::DeathCause::afk
                ? " was killed for being " : " died from ";
            message += '\x10';
            message += cause == shroudfire::DeathCause::afk ? "AFK" :
                cause == shroudfire::DeathCause::starvation ? "Starvation" : "Camping";
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                const auto &recipient = mode_.player(slot);
                if (recipient.connected && !recipient.bot)
                    engine_adapter_.client_print(slot, 3, message.c_str());
            }
        }
        if (!victim || !role_respawn_pending_[victim])
            mode_.death(victim, attacker, cause, kind, now);
        for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer)
            if (admin_observe_target_[viewer].load(std::memory_order_relaxed) == victim)
                end_observation(viewer, "target died");
    } else if (std::strcmp(name, "player_team") == 0) {
        const int slot = event_player(event, "userid");
        if (slot && event->GetInt(event_key("team")) == 1) mode_.join_spectator(slot);
    }
    process_actions();
    if (std::strcmp(name, "player_death") == 0) {
        const bool draw_pending = std::any_of(pending_engine_actions_.begin(),
            pending_engine_actions_.end(), [](const PendingEngineAction &pending) {
                return pending.action.effect == shroudfire::Effect::end_round &&
                    pending.action.role == shroudfire::Role::spectator;
            });
        if (draw_pending && mode_.round_live() && !draw_termination_requested_) {
            draw_termination_requested_ = true;
            if (!engine_adapter_.terminate_round(0, 10.0f)) {
                draw_termination_requested_ = false;
                META_CONPRINTF("[Shroudfire] Could not start the suicide/AFK draw.\n");
            }
        }
        update_hud(now);
    }
}

KHook::Return<void> ShroudfirePlugin::Hook_GameFramePre(IServerGameDLL *, bool simulating, bool, bool) {
    if (simulating)
        engine_adapter_.stabilize_center_html(game_time(), mode_.round_live(),
                                              false, false, mode_.active());
    if (!simulating || !mode_.active() || !mode_.round_live() || !mode_.freeze_ended())
        return {KHook::Action::Ignore};
    INetworkGameServer *game = network_service_ ? network_service_->GetIGameServer() : nullptr;
    if (!game) return {KHook::Action::Ignore};
    const float now = game_time();
    const int tick = game->GetServerTick();
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        const auto &player = mode_.player(slot);
        if (player.connected && !player.bot && player.alive &&
            player.role == shroudfire::Role::armitive)
            engine_adapter_.begin_control(slot,
                player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::control)],
                mode_.settings().control_reduction_percent, now, tick);
    }
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_GameFrame(IServerGameDLL *, bool simulating, bool, bool) {
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
        engine_adapter_.finish_control(slot);
    if (simulating && mode_.active()) {
        const int armitive_team = 5 - sf_invisant_team.Get();
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (player.connected && player.alive && player.role == shroudfire::Role::armitive)
                engine_adapter_.refresh_buy_zone(slot, armitive_team);
        }
    }
    const float now = game_time();
    if (simulating && sf_custom_sounds.Get() && mode_.active() && mode_.round_live() &&
        mode_.freeze_ended() && mode_.active_players() >= 2 &&
        !mode_.settings().timeout_kill_invisant && sf_timeout_warning_seconds.Get() > 0 &&
        !timeout_warning_voice_played_ &&
        mode_.round_time_remaining(now) <= sf_timeout_warning_seconds.Get()) {
        int voice = std::uniform_int_distribution<int>(0,
            static_cast<int>(kInvisantRevealWarningSounds.size()) -
            (last_timeout_warning_voice_ >= 0 ? 2 : 1))(random_engine_);
        if (last_timeout_warning_voice_ >= 0 && voice >= last_timeout_warning_voice_) ++voice;
        bool played = false;
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (!player.connected || player.bot || !player.alive ||
                player.role != shroudfire::Role::invisant) continue;
            play_sound(slot, kInvisantRevealWarningSounds[voice],
                shroudfire::AudioCategory::timeout);
            played = true;
        }
        if (played) {
            timeout_warning_voice_played_ = true;
            last_timeout_warning_voice_ = voice;
        }
    }
    if (simulating && mode_.active() && mode_.round_live() && mode_.freeze_ended() &&
        mode_.active_players() >= 2 && !round_timeout_requested_ &&
        mode_.round_time_remaining(now) <= 1) {
        // End just before CS2's native clock expires. Its stock timeout can
        // award the CT team, which may be Invisant on this server.
        round_timeout_requested_ = true;
        if (engine_adapter_.terminate_round(5 - sf_invisant_team.Get(), 10.0f)) {
            mode_.begin_round_end();
        } else {
            round_timeout_requested_ = false;
            META_CONPRINTF("[Shroudfire] Could not award the timeout to Armitive.\n");
        }
    }
#if defined(__linux__)
    if (simulating && solo_rejoin_freeze_until_ > 0.0f) {
        if (now < solo_rejoin_freeze_until_ && mode_.active_players() == 1) {
            engine_adapter_.set_freeze_period(true);
        } else {
            engine_adapter_.set_freeze_period(false);
            solo_rejoin_freeze_until_ = 0.0f;
            solo_rejoin_freeze_pending_ = false;
            if (mode_.round_live() && !mode_.freeze_ended()) mode_.freeze_end(now);
        }
    }
#endif
    if (simulating && round_voice_at_ > 0.0f && now >= round_voice_at_) {
        round_voice_at_ = 0.0f;
        if (sf_custom_sounds.Get() && mode_.active() && mode_.round_live() &&
            mode_.freeze_ended() && mode_.active_players() >= 2) {
            int armitive = std::uniform_int_distribution<int>(0,
                static_cast<int>(kArmitiveRoundStartSounds.size()) - 1)(random_engine_);
            int invisant_voice = std::uniform_int_distribution<int>(0,
                static_cast<int>(kInvisantRoundStartSounds.size()) - 1)(random_engine_);
            if (armitive == last_armitive_round_voice_)
                armitive = (armitive + std::uniform_int_distribution<int>(1,
                    static_cast<int>(kArmitiveRoundStartSounds.size()) - 1)(random_engine_)) %
                    kArmitiveRoundStartSounds.size();
            if (invisant_voice == last_invisant_round_voice_)
                invisant_voice = (invisant_voice +
                    std::uniform_int_distribution<int>(1,
                        static_cast<int>(kInvisantRoundStartSounds.size()) - 1)(random_engine_)) %
                    kInvisantRoundStartSounds.size();
            bool played_armitive = false;
            bool played_invisant = false;
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                const auto &player = mode_.player(slot);
                if (!player.connected || player.bot || !player.round_participant) continue;
                if (player.role == shroudfire::Role::armitive) {
                    play_sound(slot, kArmitiveRoundStartSounds[armitive], shroudfire::AudioCategory::round_start);
                    played_armitive = true;
                } else if (player.role == shroudfire::Role::invisant) {
                    play_sound(slot, kInvisantRoundStartSounds[invisant_voice], shroudfire::AudioCategory::round_start);
                    played_invisant = true;
                }
            }
            if (played_armitive) last_armitive_round_voice_ = armitive;
            if (played_invisant) last_invisant_round_voice_ = invisant_voice;
        }
    }
    if (simulating && round_win_voice_at_ > 0.0f && now >= round_win_voice_at_) {
        round_win_voice_at_ = 0.0f;
        if (sf_custom_sounds.Get() && mode_.active()) {
            const std::array<const char *, 3> *sounds = &kRoundDrawSounds;
            int *last_voice = &last_draw_voice_;
            if (round_win_voice_role_ == shroudfire::Role::invisant) {
                sounds = &kInvisantWinSounds;
                last_voice = &last_invisant_win_voice_;
            } else if (round_win_voice_role_ == shroudfire::Role::armitive) {
                sounds = &kArmitiveWinSounds;
                last_voice = &last_armitive_win_voice_;
            }
            int voice = std::uniform_int_distribution<int>(0,
                static_cast<int>(sounds->size()) - 1)(random_engine_);
            if (voice == *last_voice)
                voice = (voice + std::uniform_int_distribution<int>(1,
                    static_cast<int>(sounds->size()) - 1)(random_engine_)) % sounds->size();
            bool played = false;
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                const auto &player = mode_.player(slot);
                if (player.connected && !player.bot) {
                    play_sound(slot, (*sounds)[voice], shroudfire::AudioCategory::round_end);
                    played = true;
                }
            }
            if (played) *last_voice = voice;
        }
        round_win_voice_role_ = shroudfire::Role::spectator;
    }
    if (simulating) {
        bool notice_expired = false;
        if (round_info_until_ > 0.0f && now >= round_info_until_) {
            round_info_until_ = 0.0f;
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                if (!round_info_sent_[slot]) continue;
                const auto &notice = center_notices_[slot];
                if (!menu_open_[slot] && !guide_open_[slot] &&
                    mode_.player(slot).connected && !mode_.player(slot).bot &&
                    (notice.kind == CenterNoticeKind::none || now >= notice.until))
                    engine_adapter_.center_html(slot, game_events_, " ", -1);
            }
            round_info_sent_.fill(false);
            notice_expired = true;
        }
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            CenterNotice &notice = center_notices_[slot];
            if (notice.kind == CenterNoticeKind::none) continue;
            if (now < notice.until) {
                if (notice.visible && now >= notice.refresh_at) {
                    notice.visible = false;
                    notice_expired = true;
                }
                continue;
            }
            if (notice.kind == CenterNoticeKind::win)
                engine_adapter_.hide_win_layout(slot);
            if (notice.visible &&
                !menu_open_[slot] && !guide_open_[slot] &&
                mode_.player(slot).connected && !mode_.player(slot).bot)
                engine_adapter_.center_html(slot, game_events_, " ", -1);
            notice = {};
            notice_expired = true;
        }
        if (notice_expired) update_hud(now);
    }
    if (simulating && mode_.active() && now < round_draw_stop_until_) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (!player.connected || player.bot) continue;
            for (const char *name : {"Event.RoundDraw", "Announcer.RoundDraw.CS2_Classic"})
                engine_adapter_.stop_sound_event_hash(slot,
                    MurmurHash2LowerCase(name, 0x53524332));
        }
    }
    if (simulating && mode_.active() && round_win_stop_passes_ > 0 &&
        now >= round_win_stop_next_at_) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (!player.connected || player.bot) continue;
            for (const char *name : {"Event.CTWin", "Event.TERWin",
                                     "Announcer.CTWin.CS2_Classic",
                                     "Announcer.TWin.CS2_Classic"})
                engine_adapter_.stop_sound_event_hash(slot,
                    MurmurHash2LowerCase(name, 0x53524332));
        }
        --round_win_stop_passes_;
        round_win_stop_next_at_ = now + 0.1f;
    }
    if (simulating && mode_.active() && round_start_voice_stop_passes_ > 0 &&
        now >= round_start_voice_stop_next_at_) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (!player.connected || player.bot) continue;
            for (const uint32_t hash : kRoundStartVoiceHashes)
                engine_adapter_.stop_sound_event_hash(slot, hash);
        }
        --round_start_voice_stop_passes_;
        round_start_voice_stop_next_at_ = now + 0.1f;
    }
    if (simulating && mode_.active()) {
        if (mode_.expire_shields(now)) process_actions();
        if (mode_.tick_starvation(now)) process_actions();
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            if (!shield_applied_[slot] || !mode_.shield_active(slot, now)) continue;
            const int tint = sf_ward_screen_tint.Get();
            if (tint != shield_tint_alpha_[slot]) {
                const int remaining = (std::max)(1, static_cast<int>(std::lround(
                    (mode_.player(slot).shield_until - now) * 1000.0f)));
                if (engine_adapter_.screen_fade(slot, 100, 2, 35, 190, 65, tint, remaining))
                    shield_tint_alpha_[slot] = tint;
            }
            if (shield_next_sound_at_[slot] > 0.0f && now >= shield_next_sound_at_[slot]) {
                play_sound(slot, "sounds/buttons/blip1.vsnd");
                shield_next_sound_at_[slot] = now + 0.75f;
            }
        }
    }
    if (simulating) engine_adapter_.expire_blood_particles(now);
    if (simulating && pending_round_end_at_ > 0.0f && now >= pending_round_end_at_) {
        const int winner = pending_round_winner_;
        pending_round_end_at_ = 0.0f;
        pending_round_winner_ = 0;
        if (mode_.active() && !engine_adapter_.terminate_round(winner, 2.6f))
            META_CONPRINTF("[Shroudfire] Could not complete the round.\n");
    }
    if (simulating) {
        for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer) {
            const int target = admin_observe_target_[viewer].load(std::memory_order_acquire);
            if (!target) continue;
            const auto &player = mode_.player(viewer);
            if (!mode_.active() || !sf_admin_spectate.Get() ||
                !player.connected || player.bot || player.alive ||
                player.role != shroudfire::Role::spectator ||
                (target > 0
                    ? !mode_.can_spectate(viewer, target, sf_invisant_see_teammates.Get() != 0, true) ||
                      engine_adapter_.observer_target_slot(viewer) != target
                    : engine_adapter_.observer_mode(viewer) != 4 ||
                      engine_adapter_.observer_target_slot(viewer) != 0))
                end_observation(viewer, "target or view changed");
        }
    }
    if (simulating && mode_.active() && now - last_observer_check_ >= 0.1f) {
        last_observer_check_ = now;
        for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer) {
            const auto &player = mode_.player(viewer);
            if (!player.connected || player.bot || player.alive ||
                admin_observe_target_[viewer].load(std::memory_order_acquire))
                continue;
            const int current_target = engine_adapter_.observer_target_slot(viewer);
            int target = 0;
            if (mode_.can_spectate(viewer, current_target, sf_invisant_see_teammates.Get() != 0))
                target = current_target;
            if (!target) {
                for (int candidate = 1; candidate <= shroudfire::kMaxPlayers; ++candidate) {
                    if (mode_.can_spectate(viewer, candidate, sf_invisant_see_teammates.Get() != 0)) {
                        target = candidate;
                        break;
                    }
                }
            }
            const int observer_mode = engine_adapter_.observer_mode(viewer);
            if (observer_mode < 0) continue;
            if (target) {
                if (observer_mode != 2 || current_target != target)
                    engine_adapter_.set_observer(viewer, target);
            } else if (observer_mode != 1 || current_target != 0) {
                engine_adapter_.set_observer(viewer, 0);
            }
        }
    }
    if (simulating) sample_player_activity(now);
    if (simulating) refresh_local_movement_sounds(now);
    if (simulating) refresh_armitive_glows(now);
    if (simulating && mode_.active()) {
        INetworkGameServer *game = network_service_
            ? network_service_->GetIGameServer() : nullptr;
        if (game) {
            const int tick = game->GetServerTick();
            for (int attacker = 1; attacker <= shroudfire::kMaxPlayers; ++attacker) {
                const PendingClawHit pending = pending_claw_hits_[attacker];
                if (!pending.victim || tick <= pending.tick) continue;
                pending_claw_hits_[attacker] = {};
                const auto &source = mode_.player(attacker);
                const auto &target = mode_.player(pending.victim);
                if (!mode_.round_live() || !mode_.freeze_ended() ||
                    !source.connected || source.bot || !source.alive ||
                    source.role != shroudfire::Role::invisant ||
                    source.steam_id != pending.attacker_steam ||
                    !target.connected || !target.alive ||
                    target.role != shroudfire::Role::armitive ||
                    target.steam_id != pending.victim_steam ||
                    last_native_claw_hit_tick_[attacker] >= pending.tick)
                    continue;
                claw_extended_applying_[attacker] = true;
                const int amount = std::uniform_int_distribution<int>(
                    (std::min)(sf_knife_damage_min.Get(), sf_knife_damage_max.Get()),
                    (std::max)(sf_knife_damage_min.Get(), sf_knife_damage_max.Get()))(random_engine_);
                const bool hit = engine_adapter_.claw_damage(attacker, pending.victim, amount);
                claw_extended_applying_[attacker] = false;
                if (hit) {
                    last_extended_claw_hit_tick_[attacker] = tick;
                }
            }
        }
    }
    if (simulating && now - last_mode_tick_ >= 1.0f) {
        last_mode_tick_ = now;
        sync_settings();
        sync_game_cvars();
        // Use elapsed real time so map changes do not restart the interval.
        const auto chat_now = std::chrono::steady_clock::now();
        if (!mode_.active() || !sf_discord_message.Get() || sf_discord_link.Get().IsEmpty()) {
            last_discord_message_ = chat_now;
        } else if (chat_now - last_discord_message_ >= std::chrono::seconds(sf_discord_interval.Get())) {
            broadcast_discord_invite();
        }
        mode_.tick(now);
        process_actions();
        update_hud(now);
        INetworkGameServer *game = network_service_ ? network_service_->GetIGameServer() : nullptr;
        if (!mode_.settings().enabled || !sf_extra_spawns.Get() || !game) {
            if (spawn_config_ != -1) engine_adapter_.remove_extra_spawns();
            spawn_config_ = -1;
        } else {
            const int maximum = std::clamp(game->GetMaxClients(), 2, shroudfire::kMaxPlayers);
            const int ratio = (std::max)(2, mode_.settings().armitive_per_invisant);
            const int team = sf_invisant_team.Get();
            const int config = 1 | (ratio << 1) | (team << 10) | (maximum << 12);
            if (config != spawn_config_) {
                engine_adapter_.remove_extra_spawns();
                const int invisant_maximum = (std::min)(
                    (maximum + ratio) / (ratio + 1),
                    maximum == 2 ? 1 : (maximum - 1) / 2);
                const int terrorist_target = team == 2 ? invisant_maximum
                    : maximum - invisant_maximum;
                const int counter_terrorist_target = team == 3 ? invisant_maximum
                    : maximum - invisant_maximum;
                const int added = engine_adapter_.ensure_spawn_capacity(
                    terrorist_target, counter_terrorist_target);
                if (added >= 0) {
                    META_CONPRINTF("[Shroudfire] Added %d validated team spawns for %d slots.\n",
                                   added, maximum);
                    spawn_config_ = config;
                }
            }
        }
    }
    if (simulating && pending_initial_restart_ > 0.0f && now >= pending_initial_restart_) {
        pending_initial_restart_ = 0.0f;
        if (mode_.active() && mode_.active_humans() == 1) {
            const bool warmup = engine_adapter_.warmup_active().value_or(false);
            if (warmup && engine_ && initial_restart_attempts_++ < 5) {
                META_CONPRINTF("[Shroudfire] Ending warmup for the first player.\n");
                // Ending warmup already starts a round. Do not queue another
                // warmup-end command or an admin restart behind it.
                engine_->ServerCommand("mp_warmup_end\n");
            } else if (!warmup && mode_.round_live()) {
                META_CONPRINTF("[Shroudfire] Starting the first player's round.\n");
                if (mode_.admin_restart()) process_actions();
            } else if (!warmup && initial_restart_attempts_++ < 5) {
                META_CONPRINTF("[Shroudfire] Starting a native round for the first player.\n");
                restart_draw_suppression_until_ = now + 2.0f;
                round_draw_stop_until_ = now + 2.0f;
                // Arm recovery before the native call so a synchronous
                // round_start can cancel it as well.
                pending_initial_restart_ = now + 3.0f;
                if (!engine_adapter_.terminate_round(-1, 0.0f))
                    META_CONPRINTF("[Shroudfire] Native round transition unavailable.\n");
#if defined(__linux__)
                else {
                    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                        if (!mode_.player(slot).connected || mode_.player(slot).bot) continue;
                        for (const char *name : {"Event.RoundDraw", "Announcer.RoundDraw.CS2_Classic"})
                            engine_adapter_.stop_sound_event_hash(slot,
                                MurmurHash2LowerCase(name, 0x53524332));
                    }
                }
#endif
            }
        }
    }
    if (simulating && now - last_profile_save_ >= 60.0f) {
        last_profile_save_ = now;
        const int unix_time = static_cast<int>(std::time(nullptr));
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (profiles_ && player.connected && !player.bot)
                profiles_->save(mode_.profile(slot, unix_time));
        }
    }
    if (simulating) {
        apply_engine_actions(now);
        refresh_timeout_reveal(now);
        refresh_transmit_state();
        update_map_rotation(now);
        if (map_rotation_.changing) return {KHook::Action::Ignore};
    }
    if (simulating) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            if (!suicide_command_[slot]) continue;
            suicide_command_[slot] = false;
            const auto &player = mode_.player(slot);
            if (!mode_.active() || !mode_.round_live() || !player.connected ||
                player.bot || !player.alive || player.role == shroudfire::Role::spectator)
                continue;
            forced_death_cause_[slot] = shroudfire::DeathCause::suicide;
            forced_death_pending_[slot] = true;
            if (!engine_adapter_.commit_suicide(slot)) {
                forced_death_pending_[slot] = false;
                engine_adapter_.client_print(slot, 3,
                    "[Shroudfire] Could not complete suicide.");
            }
        }
    }
    if (simulating && mode_.active()) {
        // AFK death and other queued actions can finish a round in this post-frame
        // hook. Apply the client result-panel state after those actions, before
        // the frame's game rules are replicated.
        bool win_html_active = false;
#if defined(__linux__)
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &notice = center_notices_[slot];
            if (mode_.player(slot).connected && !mode_.player(slot).bot &&
                (menu_open_[slot] || guide_open_[slot] ||
                 auto_hud_visible_[slot] ||
                 (notice.kind != CenterNoticeKind::none && now < notice.until))) {
                win_html_active = true;
                break;
            }
        }
#endif
        engine_adapter_.stabilize_center_html(now, mode_.round_live(),
                                              true, win_html_active, true);
    }
    return {KHook::Action::Ignore};
}

KHook::Return<bool> ShroudfirePlugin::Hook_WeaponCanUse(void *services, void *weapon) {
    int slot = 0;
    bool knife = false;
    if (!engine_adapter_.weapon_use_context(services, weapon, slot, knife))
        return {KHook::Action::Ignore};
    const auto &player = mode_.player(slot);
    if (mode_.active() && player.connected && !player.bot && player.alive &&
        player.role == shroudfire::Role::invisant && !knife)
        return {KHook::Action::Supersede, false};
    return {KHook::Action::Ignore};
}

KHook::Return<int> ShroudfirePlugin::Hook_CanAcquire(void *services, void *item,
                                                  int method, uint64_t) {
    // CS2 AcquireMethod: 0 = pickup, 1 = purchase. This also covers the
    // buy menu, autobuy, and rebuy without changing dropped-weapon pickups.
    if (!mode_.active() || method != 1) return {KHook::Action::Ignore};
    int slot = 0;
    uint16_t definition = 0;
    if (!engine_adapter_.purchase_context(services, item, slot, definition))
        return {KHook::Action::Ignore};
    sync_settings();
    const auto &player = mode_.player(slot);
    if (!player.connected) return {KHook::Action::Ignore};
    const auto &blocked = armitive_weapon_restrictions_.definitions;
    if (player.role == shroudfire::Role::invisant ||
        (player.role == shroudfire::Role::armitive &&
         std::find(blocked.begin(), blocked.end(), definition) != blocked.end()))
        return {KHook::Action::Supersede, 10}; // AcquireResult::NotAllowedByProhibition
    return {KHook::Action::Ignore};
}

KHook::Return<int64_t> ShroudfirePlugin::Hook_TakeDamage(void *victim, void *info, void *) {
    if (!mode_.active()) return {KHook::Action::Ignore};
    EngineAdapter::DamageContext damage;
    if (!engine_adapter_.inspect_damage(victim, info, damage)) {
        static int missed_damage_contexts = 0;
        if (missed_damage_contexts++ < 12)
            META_CONPRINTF("[Shroudfire] Damage hook could not identify damage context.\n");
        return {KHook::Action::Ignore};
    }
    const auto &target = mode_.player(damage.victim);
    if (target.bot)
        META_CONPRINTF("[Shroudfire] Bot damage before: victim %d, attacker %d, hitgroup %u, type %u, amount %.1f.\n",
            damage.victim, damage.attacker, damage.hitgroup, damage.type, *damage.damage);
    if (!target.connected || !target.alive || !mode_.round_live())
        return {KHook::Action::Ignore};
    if (target.role == shroudfire::Role::armitive &&
        mode_.shield_active(damage.victim, game_time())) {
        *damage.damage = 0.0f;
    } else if (target.role == shroudfire::Role::invisant && (damage.type & 0x20u)) {
        *damage.damage = 0.0f;
    } else if (damage.attacker > 0 && damage.attacker != damage.victim) {
        const auto &source = mode_.player(damage.attacker);
        if (source.connected && source.alive) {
            if (target.role == shroudfire::Role::invisant &&
                source.role == shroudfire::Role::invisant) {
                *damage.damage = 0.0f;
            } else if (target.role == shroudfire::Role::invisant &&
                       source.role == shroudfire::Role::armitive && damage.hitgroup == 1) {
                // This hook receives the weapon's base damage before CS2 applies
                // the headshot multiplier. Apply that multiplier here, then halve
                // and cap the final damage so suppressing later modification does
                // not turn headshots into weaker body hits.
                const float multiplier = (std::max)(2.5f,
                    engine_adapter_.active_weapon_headshot_multiplier(damage.attacker));
                *damage.damage = (std::min)(90.0f, *damage.damage * multiplier * 0.5f);
                *damage.flags |= 256u; // DFLAG_SUPPRESS_DAMAGE_MODIFICATION
            } else if (target.role == shroudfire::Role::armitive &&
                       source.role == shroudfire::Role::invisant) {
                // Default loadouts and buy blocking do not prevent an Invisant
                // from picking up a dropped gun. Only a knife swing may deal
                // claw damage; a delayed bullet stays blocked after a switch.
                if (!engine_adapter_.active_weapon_is_knife(damage.attacker) ||
                    (damage.type & (1u << 2)) == 0) {
                    *damage.damage = 0.0f;
                    return {KHook::Action::Ignore};
                }
                if (!claw_extended_applying_[damage.attacker]) {
                    INetworkGameServer *game = network_service_
                        ? network_service_->GetIGameServer() : nullptr;
                    if (game) {
                        const int tick = game->GetServerTick();
                        if (last_extended_claw_hit_tick_[damage.attacker] > 0 &&
                            tick >= last_extended_claw_hit_tick_[damage.attacker] &&
                            // The engine can resolve the stock knife contact several
                            // ticks after the widened sweep. The slowed knife cannot
                            // start another swing inside this window.
                            tick - last_extended_claw_hit_tick_[damage.attacker] <= 24) {
                            *damage.damage = 0.0f;
                            return {KHook::Action::Ignore};
                        }
                        last_native_claw_hit_tick_[damage.attacker] = tick;
                    }
                }
                // The extended sweep already rolled its damage. Roll once
                // for stock knife contacts and preserve the sweep's result.
                if (!claw_extended_applying_[damage.attacker])
                    *damage.damage = static_cast<float>(std::uniform_int_distribution<int>(
                        (std::min)(sf_knife_damage_min.Get(), sf_knife_damage_max.Get()),
                        (std::max)(sf_knife_damage_min.Get(), sf_knife_damage_max.Get()))(random_engine_));
                *damage.flags |= 256u | engine_adapter_.ignore_armor_damage_flag();
            }
        }
    }
    if (target.bot)
        META_CONPRINTF("[Shroudfire] Bot damage after: %.1f.\n", *damage.damage);
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_PostSoundEvent(IGameEventSystem *system,
    CSplitScreenSlot split_screen, bool local_only, int, const uint64 *recipients,
    INetworkMessageInternal *type,
    const CNetMessage *message, unsigned long size, NetChannelBufType_t buffer_type) {
    const auto stock_chat_notice = [](std::string key) {
        std::transform(key.begin(), key.end(), key.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (!key.empty() && key[0] == '#') key.erase(0, 1);
        return key.rfind("player_cash_award_", 0) == 0 ||
            key.rfind("team_cash_award_", 0) == 0 ||
            key.rfind("player_point_award_", 0) == 0 ||
            key.rfind("notice_bonus_", 0) == 0 ||
            key.rfind("game_join_", 0) == 0 ||
            key.rfind("cstrike_game_join_", 0) == 0 ||
            key.rfind("cstrike_titlestxt_game_join_", 0) == 0;
    };
    if (mode_.active() && message && game_events_ &&
        message->GetMessageId() == GE_Source1LegacyGameEvent) {
        const auto *event = message->ToPB<CMsgSource1LegacyGameEvent>();
        if (event->eventid() == game_events_->LookupEventId("cs_pre_restart") ||
            event->eventid() == game_events_->LookupEventId("round_announce_match_start"))
            return {KHook::Action::Supersede};
    }
    if (mode_.active() && message &&
        (message->GetMessageId() == UM_TextMsg ||
         message->GetMessageId() == CS_UM_TextMsg_CSGOLegacy)) {
        const auto *notice = message->ToPB<CUserMessageTextMsg>();
        if (notice->param_size() > 0) {
            const std::string &text = notice->param(0);
            if (notice->dest() == 3 && stock_chat_notice(text))
                return {KHook::Action::Supersede};
            std::string lower = text;
            std::transform(lower.begin(), lower.end(), lower.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (lower.find("restart") != std::string::npos)
                META_CONPRINTF("[Shroudfire] Restart text message: %s.\n", text.c_str());
            if (text == "MATCH START" ||
                text == "#SFUI_Notice_Alert_Match_Start" ||
                text == "#CSGO_Notice_Alert_Match_Start")
                return {KHook::Action::Supersede};
        }
    }
    if (mode_.active() && message &&
        (message->GetMessageId() == UM_SayText2 ||
         message->GetMessageId() == CS_UM_SayText2_CSGOLegacy)) {
        const auto *notice = message->ToPB<CUserMessageSayText2>();
        if (stock_chat_notice(notice->messagename()))
            return {KHook::Action::Supersede};
        // Preserve native formatting and chat throttling, but route player text
        // independently of life state. Team-only messages never reach enemies.
        std::string key = notice->messagename();
        if (!key.empty() && key[0] == '#') key.erase(0, 1);
        const int sender = notice->entityindex();
        if (!local_only && notice->chat() && key.rfind("Cstrike_Chat_", 0) == 0 &&
            sender >= 1 && sender <= shroudfire::kMaxPlayers && mode_.player(sender).connected) {
            const bool team_only = key.rfind("Cstrike_Chat_All", 0) != 0;
            const uint64_t all_recipients = mode_.chat_recipients(sender, false);
            const uint64 recipients = chat_routing_.recipients(sender, team_only,
                all_recipients, mode_.chat_recipients(sender, true));
            if (recipients)
                post_sound_event_.CallOriginal(system, split_screen, local_only,
                    shroudfire::kMaxPlayers, &recipients, type, message, size, buffer_type);
            else if (all_recipients && !chat_split_reported_) {
                chat_split_reported_ = true;
                META_CONPRINTF("[Shroudfire] Consolidated native chat packets into one broadcast.\n");
            }
            return {KHook::Action::Supersede};
        }
    }
    if (mode_.active() && message && message->GetMessageId() == CS_UM_HudMsg) {
        const std::string &text = message->ToPB<CCSUsrMsg_HudMsg>()->text();
        std::string lower = text;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (lower.find("restart") != std::string::npos)
            META_CONPRINTF("[Shroudfire] Restart HUD message: %s.\n", text.c_str());
        if (text == "MATCH START" ||
            text == "#SFUI_Notice_Alert_Match_Start" ||
            text == "#CSGO_Notice_Alert_Match_Start")
            return {KHook::Action::Supersede};
    }
    if (mode_.active() && message && message->GetMessageId() == CS_UM_RawAudio) {
        const auto *audio = message->ToPB<CCSUsrMsg_RawAudio>();
        std::string name(audio->voice_filename());
        std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (name.find("letsgo") != std::string::npos ||
            name.find("locknload") != std::string::npos ||
            name.find("moveout") != std::string::npos ||
            name.find("move_out") != std::string::npos ||
            name.find("ctwin") != std::string::npos ||
            name.find("terwin") != std::string::npos ||
            name.find("rounddraw") != std::string::npos ||
            stock_kill_voice_name(name))
            return {KHook::Action::Supersede};
    }
    if (mode_.active() && message && message->GetMessageId() == CS_UM_SendAudio) {
        const auto *audio = message->ToPB<CCSUsrMsg_SendAudio>();
        std::string name(audio->radio_sound());
        std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (name.find("letsgo") != std::string::npos ||
            name.find("locknload") != std::string::npos ||
            name.find("moveout") != std::string::npos ||
            name.find("move_out") != std::string::npos ||
            name.find("ctwin") != std::string::npos ||
            name.find("terwin") != std::string::npos ||
            name.find("rounddraw") != std::string::npos ||
            stock_kill_voice_name(name))
            return {KHook::Action::Supersede};
    }
    if (mode_.active() && message && message->GetMessageId() == GE_SosStartSoundEvent) {
        const auto *sound = message->ToPB<CMsgSosStartSoundEvent>();
        if (std::binary_search(kRoundStartVoiceHashes.begin(),
                kRoundStartVoiceHashes.end(), sound->soundevent_hash()))
            return {KHook::Action::Supersede};
        if (std::binary_search(kKillVoiceHashes.begin(),
                kKillVoiceHashes.end(), sound->soundevent_hash()))
            return {KHook::Action::Supersede};
        switch (sound->soundevent_hash()) {
            case MurmurHash2LowerCase("Event.CTWin", 0x53524332):
            case MurmurHash2LowerCase("Event.TERWin", 0x53524332):
            case MurmurHash2LowerCase("Event.RoundDraw", 0x53524332):
            case MurmurHash2LowerCase("Announcer.CTWin.CS2_Classic", 0x53524332):
            case MurmurHash2LowerCase("Announcer.TWin.CS2_Classic", 0x53524332):
            case MurmurHash2LowerCase("Announcer.RoundDraw.CS2_Classic", 0x53524332):
                return {KHook::Action::Supersede};
            default:
                break;
        }
        const int source_slot = engine_adapter_.sound_source_slot(sound->source_entity_index());
        if (source_slot > 0 && mode_.player(source_slot).connected &&
            mode_.player(source_slot).role == shroudfire::Role::invisant) {
            bool replay_knife_sound = false;
            switch (sound->soundevent_hash()) {
                case MurmurHash2LowerCase("Weapon_Knife.Slash", 0x53524332):
                    replay_knife_sound = sf_knife_miss_sound.Get() != 0;
                    break;
                case MurmurHash2LowerCase("Weapon_Knife.HitWall", 0x53524332):
                case MurmurHash2LowerCase("Weapon_Knife.Hit.Light", 0x53524332):
                case MurmurHash2LowerCase("Weapon_Knife.Hit.Heavy", 0x53524332):
                    replay_knife_sound = sf_knife_world_hit_sound.Get() != 0;
                    break;
                default:
                    break;
            }
            if (replay_knife_sound) {
                uint64 armitive_recipients = 0;
                for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                    const auto &viewer = mode_.player(slot);
                    if (viewer.connected && !viewer.bot &&
                        viewer.role == shroudfire::Role::armitive)
                        armitive_recipients |= uint64 {1} << (slot - 1);
                }
                if (recipients) armitive_recipients &= ~*recipients;
                if (armitive_recipients)
                    post_sound_event_.CallOriginal(system, split_screen, local_only,
                        shroudfire::kMaxPlayers, &armitive_recipients, type, message,
                        size, buffer_type);
            }
            // These movement event IDs were observed on this CS2 build. The earlier
            // EmitSound recipient filter does not intercept their game-event path.
            switch (sound->soundevent_hash()) {
                case 1194677450u: case 3166948458u: case 3099536373u:
                case 4160462271u: case 1016523349u: case 1763490157u:
                    return {KHook::Action::Supersede};
                default:
                    break;
            }
        }
    }
    return {KHook::Action::Ignore};
}

KHook::Return<SoundEventResult> ShroudfirePlugin::Hook_EmitSoundFilter(
    IRecipientFilter &filter, int source_index, const void *params) {
    uint64_t removed = 0;
    if (mode_.active() && params) {
        const char *sound = *reinterpret_cast<const char *const *>(params);
        if (sound) {
            std::string name(sound);
            for (char &ch : name)
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            const bool stock_voice = name.find("ctwin") != std::string::npos ||
                name.find("terwin") != std::string::npos ||
                name.find("rounddraw") != std::string::npos ||
                name.find("announcer.twin") != std::string::npos ||
                name.find("letsgo") != std::string::npos ||
                name.find("locknload") != std::string::npos ||
                name.find("moveout") != std::string::npos ||
                stock_kill_voice_name(name);
            // CS2 labels damage and death feedback with the victim's entity, even when
            // the recipient is the attacker. Preserve those cues for both teams.
            const bool combat_feedback = name.rfind("player.damage", 0) == 0 ||
                name.rfind("player.death", 0) == 0;
            const bool local_movement_blocker = name == "shroudfire.silence_movement_steps" ||
                name == "shroudfire.silence_movement_jump" ||
                name == "shroudfire.silence_movement_land";
            const int source_slot = combat_feedback || local_movement_blocker ? 0
                : engine_adapter_.sound_source_slot(source_index);
            const bool invisant_player_source = source_slot > 0 &&
                mode_.player(source_slot).connected &&
                mode_.player(source_slot).role == shroudfire::Role::invisant;
            const bool invisant_source = invisant_player_source &&
                !mode_.player(source_slot).bot;
            const bool movement = name.find("footstep") != std::string::npos ||
                name.find(".step") != std::string::npos ||
                name.find("jump") != std::string::npos ||
                name.find("land") != std::string::npos;
            // Confirmed knife damage sends one replacement flesh impact from
            // the victim. Suppress the native Invisant-source copy to avoid
            // playing a second, distance-faded hit to the attacker.
            const bool claw_impact = name == "weapon_knife.stab" ||
                (name.rfind("weapon_knife.hit", 0) == 0 &&
                 name.find("flesh") != std::string::npos);
            const bool replace_claw_impact = invisant_player_source && claw_impact;
            if (stock_voice || invisant_source || replace_claw_impact) {
                auto &recipients = const_cast<CPlayerBitVec &>(filter.GetRecipients());
                for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                    if (!recipients.IsBitSet(slot - 1)) continue;
                    bool keep = !stock_voice;
                    if (replace_claw_impact) {
                        keep = false;
                    } else if (invisant_source && !stock_voice) {
                        const auto &viewer = mode_.player(slot);
                        if (viewer.connected && !viewer.bot) {
                            if (viewer.role == shroudfire::Role::invisant)
                                keep = !movement;
                            else if (viewer.role == shroudfire::Role::armitive)
                                keep = false;
                            else
                                keep = admin_observe_target_[slot].load(
                                    std::memory_order_acquire) == source_slot;
                        }
                    }
                    if (!keep) {
                        recipients.Clear(slot - 1);
                        removed |= uint64_t {1} << (slot - 1);
                    }
                }
            }
        }
    }
    sound_recipients_restore.push_back({&filter, removed});
    return {KHook::Action::Ignore, {}};
}

KHook::Return<SoundEventResult> ShroudfirePlugin::Hook_EmitSoundFilterPost(
    IRecipientFilter &filter, int, const void *) {
    if (!sound_recipients_restore.empty()) {
        const SoundRecipientsRestore saved = sound_recipients_restore.back();
        sound_recipients_restore.pop_back();
        if (saved.filter == &filter && saved.removed) {
            auto &recipients = const_cast<CPlayerBitVec &>(filter.GetRecipients());
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
                if (saved.removed & (uint64_t {1} << (slot - 1)))
                    recipients.Set(slot - 1);
        }
    }
    return {KHook::Action::Ignore, {}};
}

KHook::Return<void> ShroudfirePlugin::Hook_ProcessMovement(void *services, void *) {
    MovementFrameRestore saved {};
    if (services && !player_run_command_target_) {
        auto **vtable = *reinterpret_cast<void ***>(services);
        if (vtable && vtable[kPlayerRunCommandIndex]) {
            player_run_command_target_ = vtable[kPlayerRunCommandIndex];
            if (player_run_command_target_ != observer_run_command_target_) {
                player_run_command_hook_ =
                    std::make_unique<KHook::Function<void, void *, void *>>(
                        this, &ShroudfirePlugin::Hook_PlayerRunCommand, nullptr);
                player_run_command_hook_->Configure(player_run_command_target_);
            }
            META_CONPRINTF("[Shroudfire] Early knife input hook active.\n");
        }
    }
    INetworkGameServer *game = network_service_ ? network_service_->GetIGameServer() : nullptr;
    CGlobalVars *globals = game ? game->GetGlobals() : nullptr;
    if (mode_.active()) {
        const int slot = engine_adapter_.movement_source_slot(services);
        if (slot > 0) {
            const auto &player = mode_.player(slot);
            if (player.connected && player.alive &&
                player.role == shroudfire::Role::invisant &&
                engine_adapter_.active_weapon_is_knife(slot))
                engine_adapter_.remap_knife_primary_attack(services);
            if (player.connected && player.alive &&
                player.role == shroudfire::Role::invisant &&
                player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::mobility)] > 0 &&
                globals) {
                EngineAdapter::Activity activity;
                if (engine_adapter_.sample_activity(slot, activity) && activity.crouched &&
                    (activity.flags & FL_ONGROUND) != 0 &&
                    std::isfinite(globals->frametime) && globals->frametime > 0.0f &&
                    globals->frametime <= 0.1f) {
                    saved = {globals, globals->frametime};
                    globals->frametime *= mode_.attributes(slot).crouch_speed_scale;
                }
            }
        }
    }
    movement_frametimes_restore.push_back(saved);
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_PlayerRunCommand(void *services, void *command) {
    if (!mode_.active() || !services || !command) return {KHook::Action::Ignore};
    const int slot = engine_adapter_.movement_source_slot(services);
    if (slot < 1) return {KHook::Action::Ignore};
    const auto &player = mode_.player(slot);
    const bool panel_open = menu_open_[slot] || guide_open_[slot];
    const bool dead_panel = !player.alive && panel_open;
    if (!player.connected || (!player.alive && !dead_panel)) return {KHook::Action::Ignore};

    auto *input = reinterpret_cast<ServerUserCommand *>(command);
    auto *base = input->mutable_base();
    const uint64_t primary = static_cast<uint64_t>(IN_ATTACK);
    const uint64_t secondary = static_cast<uint64_t>(IN_ATTACK2);
    const uint64_t reload = static_cast<uint64_t>(IN_RELOAD);
    const uint64_t panel_buttons = IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT |
        IN_JUMP | IN_RELOAD;
    const uint64_t blocked_panel_buttons = panel_buttons | primary | secondary;
    if (panel_open) {
        uint64_t held = base ? base->buttons_pb().buttonstate1() : 0;
#ifndef _WIN32
        held |= input->button_states[0];
#endif
        auto &captured = panel_menu_input_[slot];
        captured.capture(services, held, panel_buttons);
        if (base) {
            // A tap can press and release within one command or server frame.
            for (const auto &step : base->subtick_moves())
                if (step.pressed()) captured.capture_press(step.button(), panel_buttons);
        }
    }
    const bool knife = player.alive && player.role == shroudfire::Role::invisant &&
        engine_adapter_.active_weapon_is_knife(slot);
    bool reload_requested = false;
    bool attack_requested = false;
#ifndef _WIN32
    for (uint64_t buttons : input->button_states) {
        reload_requested |= (buttons & reload) != 0;
        attack_requested |= (buttons & primary) != 0;
    }
#endif
    if (base) {
        const auto &buttons = base->buttons_pb();
        reload_requested |= ((buttons.buttonstate1() | buttons.buttonstate2() |
            buttons.buttonstate3()) & reload) != 0;
        attack_requested |= ((buttons.buttonstate1() | buttons.buttonstate2() |
            buttons.buttonstate3()) & primary) != 0;
        for (const auto &step : base->subtick_moves()) {
            reload_requested |= (step.button() & reload) != 0;
            attack_requested |= (step.button() & primary) != 0;
        }
    }
    const bool block_reload = player.alive && player.role == shroudfire::Role::armitive &&
        reload_requested && engine_adapter_.intercept_overload_reload(slot,
            player.applied_upgrades[static_cast<int>(shroudfire::Upgrade::overload)],
            mode_.settings().overload_magazine_percent, game_time(),
            mode_.attributes(slot).reload_delay_scale);
    if (player.alive && player.role == shroudfire::Role::armitive && attack_requested)
        engine_adapter_.cancel_overload_reload_on_attack(slot);
    if (!knife && !block_reload && !dead_panel) return {KHook::Action::Ignore};
#ifndef _WIN32
    for (uint64_t &buttons : input->button_states) {
        if (knife && (buttons & primary)) buttons = (buttons & ~primary) | secondary;
        if (block_reload) buttons &= ~reload;
        if (dead_panel) buttons &= ~blocked_panel_buttons;
    }
#endif
    if (base) {
        auto *buttons = base->mutable_buttons_pb();
        for (auto update : {
                 std::make_pair(buttons->buttonstate1(), &CInButtonStatePB::set_buttonstate1),
                 std::make_pair(buttons->buttonstate2(), &CInButtonStatePB::set_buttonstate2),
                 std::make_pair(buttons->buttonstate3(), &CInButtonStatePB::set_buttonstate3)}) {
            uint64_t changed = update.first;
            if (knife && (changed & primary)) changed = (changed & ~primary) | secondary;
            if (block_reload) changed &= ~reload;
            if (dead_panel) changed &= ~blocked_panel_buttons;
            if (changed != update.first) (buttons->*update.second)(changed);
        }
        for (int i = 0; i < base->subtick_moves_size(); ++i) {
            auto *step = base->mutable_subtick_moves(i);
            uint64_t changed = step->button();
            if (knife && (changed & primary)) changed = (changed & ~primary) | secondary;
            if (block_reload) changed &= ~reload;
            if (dead_panel) changed &= ~blocked_panel_buttons;
            if (changed != step->button()) step->set_button(changed);
        }
    }
    if (dead_panel && base) {
        base->set_forwardmove(0.0f);
        base->set_leftmove(0.0f);
        base->set_upmove(0.0f);
    }
    if (knife) {
        const int first_attack = input->attack1_start_history_index();
        if (first_attack >= 0) {
            if (input->attack2_start_history_index() < 0)
                input->set_attack2_start_history_index(first_attack);
            input->clear_attack1_start_history_index();
        }
    }
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_ProcessMovementPost(void *, void *) {
    if (!movement_frametimes_restore.empty()) {
        const MovementFrameRestore saved = movement_frametimes_restore.back();
        movement_frametimes_restore.pop_back();
        if (saved.globals) saved.globals->frametime = saved.frametime;
    }
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_OnClientConnected(IServerGameClients *, CPlayerSlot slot,
    const char *, uint64, const char *, const char *, bool fake) {
    if (slot.IsValid() && slot.Get() < shroudfire::kMaxPlayers) fake_player_[slot.Get() + 1] = fake;
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_ClientActive(IServerGameClients *, CPlayerSlot slot, bool,
    const char *name, uint64 xuid) {
    const int player = slot.Get() + 1;
    if (player < 1 || player > shroudfire::kMaxPlayers) return {KHook::Action::Ignore};
    sync_settings();
    const auto saved = profiles_ ? profiles_->load(xuid) : std::nullopt;
#if defined(__linux__)
    const bool rejoining_live_solo_round = !fake_player_[player] && mode_.active() &&
        mode_.active_players() == 0 && mode_.round_live();
#endif
    mode_.connect(player, xuid, fake_player_[player], saved ? &*saved : nullptr,
                  static_cast<int>(std::time(nullptr)), game_time());
    if (!fake_player_[player] && mode_.active() && mode_.active_humans() == 1) {
        initial_restart_attempts_ = 0;
        // Let connection setup finish, then start the round on the next frames.
        pending_initial_restart_ = game_time() + 0.1f;
#if defined(__linux__)
        if (rejoining_live_solo_round) solo_rejoin_freeze_pending_ = true;
#endif
    }
    player_names_[player] = name ? name : "";
    center_notices_[player] = {};
    auto_hud_visible_[player] = false;
    guide_hint_due_[player] = 0.0f;
    activity_sample_ready_[player] = false;
    panel_menu_input_[player] = {};
    forced_death_pending_[player] = false;
    role_respawn_pending_[player] = false;
    process_actions();
    update_hud(game_time());
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_ClientDisconnect(IServerGameClients *, CPlayerSlot slot,
    ENetworkDisconnectionReason, const char *, uint64, const char *) {
    const int player = slot.Get() + 1;
    if (player >= 1 && player <= shroudfire::kMaxPlayers) {
        end_observation(player, "administrator disconnected");
        engine_adapter_.apply_dexterity(player, 0,
            mode_.settings().dexterity_speed_percent, 0.0f, 0);
        engine_adapter_.apply_claw_timing(player, 0.0f, 0, false,
            sf_knife_attack_speed_percent.Get());
        pending_claw_hits_[player] = {};
        last_native_claw_hit_tick_[player] = 0;
        claw_extended_applying_[player] = false;
        last_extended_claw_hit_tick_[player] = 0;
        for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer)
            if (admin_observe_target_[viewer].load(std::memory_order_relaxed) == player)
                end_observation(viewer, "target disconnected");
        if (mode_.player(player).connected && !mode_.player(player).bot) {
            const auto profile = mode_.disconnect(player, static_cast<int>(std::time(nullptr)));
            rankings_.update(profile);
            if (profiles_) profiles_->save(profile);
        } else mode_.disconnect(player, static_cast<int>(std::time(nullptr)));
#if defined(__linux__)
        if (mode_.active_humans() == 0) {
            solo_rejoin_freeze_pending_ = false;
            if (solo_rejoin_freeze_until_ > 0.0f) {
                solo_rejoin_freeze_until_ = 0.0f;
                engine_adapter_.set_freeze_period(false);
            }
        }
#endif
        fake_player_[player] = false;
        local_movement_next_refresh_at_[player] = 0.0f;
        player_names_[player].clear();
        center_notices_[player] = {};
        auto_hud_visible_[player] = false;
        suicide_command_[player] = false;
        menu_open_[player] = false;
        guide_open_[player] = false;
        guide_hint_due_[player] = 0.0f;
        activity_sample_ready_[player] = false;
        panel_menu_input_[player] = {};
        forced_death_pending_[player] = false;
        role_respawn_pending_[player] = false;
        shield_applied_[player] = false;
        shield_next_sound_at_[player] = 0.0f;
        shield_tint_alpha_[player] = 0;
        process_actions();
        update_hud(game_time());
    }
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_ClientCommand(IServerGameClients *, CPlayerSlot slot,
    const CCommand &args) {
    const int player = slot.Get() + 1;
    if (player < 1 || player > shroudfire::kMaxPlayers || args.ArgC() == 0)
        return {KHook::Action::Ignore};
    sync_settings();
    const char *command = args.Arg(0);
    if ((std::strcmp(command, "say") == 0 || std::strcmp(command, "say_team") == 0) &&
        args.ArgC() > 1 && handle_chat_command(player, args.Arg(1)))
        return {KHook::Action::Supersede};
    if (mode_.active() && mode_.player(player).connected) {
        // spec_mode: Selects an open HUD menu while dead instead of changing spectator mode.
        if (std::strcmp(command, "spec_mode") == 0 && !mode_.player(player).alive &&
            !mode_.player(player).bot && (menu_open_[player] || guide_open_[player])) {
            panel_menu_input_[player].select_pending = true;
            return {KHook::Action::Supersede};
        }
        // chooseteam: Suppresses server-side team-menu requests while the mod is active.
        if (std::strcmp(command, "chooseteam") == 0)
            return {KHook::Action::Supersede};
        // joinclass: Suppresses class changes for living active players.
        if (std::strcmp(command, "joinclass") == 0 && mode_.player(player).alive)
            return {KHook::Action::Supersede};
        const bool buying = std::strcmp(command, "buy") == 0 ||
            std::strcmp(command, "autobuy") == 0 || std::strcmp(command, "rebuy") == 0 ||
            std::strcmp(command, "buyammo1") == 0 || std::strcmp(command, "buyammo2") == 0 ||
            std::strcmp(command, "buymenu") == 0;
        if (buying && (mode_.player(player).role == shroudfire::Role::invisant ||
            !engine_adapter_.can_buy(player, 5 - sf_invisant_team.Get())))
            return {KHook::Action::Supersede};
        if (mode_.player(player).role == shroudfire::Role::invisant) {
            // Radio and chat wheel: Suppresses Invisant callouts that reveal their position.
            static const char *signals[] = {
                "radio1", "radio2", "radio3", "playerchatwheel", "coverme", "takepoint",
                "holdpos", "regroup", "followme", "takingfire", "go", "fallback",
                "sticktog", "getinpos", "stormfront", "report", "roger", "enemyspot",
                "needbackup", "sectorclear", "inposition", "reportingin", "getout",
                "negative", "enemydown"
            };
            for (const char *signal : signals)
                if (std::strcmp(command, signal) == 0)
                    return {KHook::Action::Supersede};
            // impulse 100 and 201: Suppresses Invisant flashlight and spray inputs.
            if (std::strcmp(command, "impulse") == 0 && args.ArgC() > 1 &&
                (std::strcmp(args.Arg(1), "100") == 0 ||
                 std::strcmp(args.Arg(1), "201") == 0))
                return {KHook::Action::Supersede};
        }
    }
    if (std::strcmp(command, "jointeam") == 0 && args.ArgC() > 1) {
        const int team = std::atoi(args.Arg(1));
        sync_settings();
        if (team < 1 || team > 3) {
            if (!mode_.request_team(player, shroudfire::Role::invisant, game_time()))
                return {KHook::Action::Supersede};
            return {KHook::Action::Ignore};
        }
        const shroudfire::Role requested = team == 1 ? shroudfire::Role::spectator
            : team == sf_invisant_team.Get() ? shroudfire::Role::invisant
            : team == 5 - sf_invisant_team.Get() ? shroudfire::Role::armitive
            : shroudfire::Role::spectator;
        if (!mode_.request_team(player, requested, game_time()))
            return {KHook::Action::Supersede};
        end_observation(player, "administrator changed team");
        process_actions();
    }
    // kill and explode: Suicide during an active round; an Armitive player forfeits Invisant eligibility next round. A solo death automatically restarts the round.
    if ((std::strcmp(command, "kill") == 0 || std::strcmp(command, "explode") == 0) &&
        mode_.active() && mode_.round_live() && mode_.player(player).connected &&
        !mode_.player(player).bot && mode_.player(player).alive &&
        mode_.player(player).role != shroudfire::Role::spectator) {
        if (mode_.forfeit_invisant(player))
            engine_adapter_.client_print(player, 3,
                " \x04[Shroudfire]\x01 You cannot be \x0BInvisant\x01 next round by suicide");
        suicide_command_[player] = true;
        mode_.activity(player, game_time());
        return {KHook::Action::Supersede};
    }
    mode_.activity(player, game_time());
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_DispatchConCommand(ICvar *, ConCommandRef command,
    const CCommandContext &context, const CCommand &) {
    if (!command.IsValidRef()) return {KHook::Action::Ignore};
    const char *name = command.GetName();
    if (std::strcmp(name, "kill") != 0 && std::strcmp(name, "explode") != 0 &&
        std::strcmp(name, "spec_mode") != 0)
        return {KHook::Action::Ignore};
    const int player = context.GetPlayerSlot().Get() + 1;
    if (player < 1 || player > shroudfire::kMaxPlayers) return {KHook::Action::Ignore};
    sync_settings();
    if (std::strcmp(name, "spec_mode") == 0) {
        // spec_mode: Selects an open HUD menu while dead instead of changing spectator mode.
        if (mode_.active() && mode_.player(player).connected && !mode_.player(player).bot &&
            !mode_.player(player).alive && (menu_open_[player] || guide_open_[player])) {
            panel_menu_input_[player].select_pending = true;
            return {KHook::Action::Supersede};
        }
        return {KHook::Action::Ignore};
    }
    if (!mode_.active() || !mode_.round_live() || !mode_.player(player).connected ||
        mode_.player(player).bot || !mode_.player(player).alive)
        return {KHook::Action::Ignore};
    if (mode_.forfeit_invisant(player)) {
        META_CONPRINTF("[Shroudfire] %s from slot %d forfeited next-round Invisant eligibility.\n",
                       name, player);
        engine_adapter_.client_print(player, 3,
            " \x04[Shroudfire]\x01 You cannot be \x0BInvisant\x01 next round by suicide");
    }
    suicide_command_[player] = true;
    return {KHook::Action::Ignore};
}

KHook::Return<void> ShroudfirePlugin::Hook_CheckTransmit(ISource2GameEntities *,
    CCheckTransmitInfo **infos, int count, CBitVec<MAX_EDICTS> &,
    CBitVec<MAX_EDICTS> &, const Entity2Networkable_t **, const uint16 *, int) {
    const uint64_t invisible = invisant_transmit_mask_.load(std::memory_order_acquire);
    const bool glows = glow_count_.load(std::memory_order_acquire) > 0;
    if ((!invisible && !glows) || !infos || count <= 0 || count > shroudfire::kMaxPlayers)
        return {KHook::Action::Ignore};
    const uint64_t teammates = invisant_viewer_mask_.load(std::memory_order_acquire);
    const bool teammate_view = teammate_transmit_view_.load(std::memory_order_acquire);
    const int invisant_team = invisant_transmit_team_.load(std::memory_order_acquire);
    for (int i = 0; i < count; ++i) {
        CCheckTransmitInfo *info = infos[i];
        if (!info || !info->m_pTransmitEntity || !info->m_pTransmitAlways) continue;
        int viewer = -1;
        std::memcpy(&viewer, reinterpret_cast<const char *>(info) + 576, sizeof(viewer));
        if (viewer < 0 || viewer >= shroudfire::kMaxPlayers) continue;
        if (glows) {
            const bool tracking_viewer = (glow_viewer_mask_.load(std::memory_order_acquire) &
                                          (uint64_t {1} << viewer)) != 0;
            const bool ward_viewer = (ward_viewer_mask_.load(std::memory_order_acquire) &
                                      (uint64_t {1} << viewer)) != 0;
            const uint64_t shields = shield_glow_mask_.load(std::memory_order_acquire);
            for (int target = 1; target <= shroudfire::kMaxPlayers; ++target) {
                if (!tracking_viewer) {
                    for (const int index : {
                             glow_relay_indices_[target].load(std::memory_order_acquire),
                             glow_prop_indices_[target].load(std::memory_order_acquire)}) {
                        if (index <= 0 || index >= MAX_EDICTS) continue;
                        info->m_pTransmitEntity->Clear(index);
                        info->m_pTransmitAlways->Clear(index);
                    }
                }
                if (!ward_viewer || viewer == target - 1 ||
                    !(shields & (uint64_t {1} << (target - 1)))) {
                    for (const int index : {
                             ward_relay_indices_[target].load(std::memory_order_acquire),
                             ward_prop_indices_[target].load(std::memory_order_acquire)}) {
                        if (index <= 0 || index >= MAX_EDICTS) continue;
                        info->m_pTransmitEntity->Clear(index);
                        info->m_pTransmitAlways->Clear(index);
                    }
                }
            }
        }
        if (!invisible) continue;
        if (teammate_view && (teammates & (uint64_t{1} << viewer))) continue;
        for (int target = 0; target < shroudfire::kMaxPlayers; ++target) {
            if (viewer == target || !(invisible & (uint64_t{1} << target))) continue;
            if (admin_observe_target_[viewer + 1].load(std::memory_order_acquire) == target + 1)
                continue;
            std::array<int, EngineAdapter::kMaxVisualEntities> entities {};
            size_t entity_count = 0;
            engine_adapter_.collect_visual_group(target + 1, invisant_team,
                                                 entities, entity_count);
            // Filter even a partial group, including the pawn at index zero.
            // Alpha-only hiding still sends a trackable pawn and target ID.
            // An invalid owned-item handle must not expose the model either.
            for (size_t entity = 0; entity < entity_count; ++entity) {
                const int index = entities[entity];
                info->m_pTransmitEntity->Clear(index);
                info->m_pTransmitAlways->Clear(index);
            }
        }
    }
    return {KHook::Action::Ignore};
}

void ShroudfirePlugin::refresh_transmit_state() {
    uint64_t targets = 0;
    uint64_t viewers = 0;
    // During a native round reset CS2 discards and recreates pawns. Do not
    // alter their transmit sets until the new round has left freeze time.
    if (mode_.active() && ((mode_.round_live() && mode_.freeze_ended()) ||
                          mode_.timeout_reveal_active())) {
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
            const auto &player = mode_.player(slot);
            if (!player.connected || player.role != shroudfire::Role::invisant) continue;
            if (!player.bot) viewers |= uint64_t{1} << (slot - 1);
            if (player.alive && (!mode_.timeout_reveal_active() ||
                !(timeout_reveal_ready_mask_ & (uint64_t{1} << (slot - 1)))))
                targets |= uint64_t{1} << (slot - 1);
        }
    }
    invisant_viewer_mask_.store(viewers, std::memory_order_release);
    invisant_transmit_team_.store(sf_invisant_team.Get(), std::memory_order_release);
    teammate_transmit_view_.store(sf_invisant_see_teammates.Get() != 0,
                                  std::memory_order_release);
    invisant_transmit_mask_.store(targets, std::memory_order_release);
}

void ShroudfirePlugin::refresh_timeout_reveal(float now) {
    if (!mode_.active() || !mode_.timeout_reveal_active()) {
        engine_adapter_.restore_reveal_rendering();
        timeout_reveal_ready_mask_ = 0;
        timeout_reveal_last_alpha_.fill(-1);
        return;
    }
    const int alpha = mode_.timeout_reveal_alpha(now);
    uint64_t ready = 0;
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        const auto &player = mode_.player(slot);
        if (!player.connected || !player.alive || player.role != shroudfire::Role::invisant)
            continue;
        // Set the pawn, knife and wearables transparent before releasing the transmit filter.
        const int applied_alpha = timeout_reveal_last_alpha_[slot] < 0 && alpha < 255 ? 0 : alpha;
        if (!engine_adapter_.set_reveal_alpha(slot, sf_invisant_team.Get(), applied_alpha)) {
            if (timeout_reveal_last_alpha_[slot] != -2)
                META_CONPRINTF("[Shroudfire] Could not prepare timeout reveal for slot %d; keeping hidden.\n", slot);
            timeout_reveal_last_alpha_[slot] = -2;
            continue;
        }
        ready |= uint64_t{1} << (slot - 1);
        if (timeout_reveal_last_alpha_[slot] < 0 ||
            (applied_alpha == 255 && timeout_reveal_last_alpha_[slot] != 255))
            META_CONPRINTF("[Shroudfire] Timeout reveal slot %d: alpha %d, health %d.\n",
                slot, applied_alpha, engine_adapter_.player_health(slot));
        timeout_reveal_last_alpha_[slot] = applied_alpha;
    }
    timeout_reveal_ready_mask_ = ready;
}

void ShroudfirePlugin::show_level(int slot) {
    if (slot < 1 || slot > shroudfire::kMaxPlayers || !engine_) return;
    const auto &p = mode_.player(slot);
    if (!p.connected || p.bot) return;
    char message[256];
    std::snprintf(message, sizeof(message),
        "[Shroudfire] Invisant: level %d (%d XP) | Armitive: level %d (%d XP)\n",
        mode_.level(p.xp[0]), p.xp[0], mode_.level(p.xp[1]), p.xp[1]);
    if (!engine_adapter_.client_print(slot, 3, message))
        engine_->ClientPrintf(CPlayerSlot(slot - 1), message);
}

bool ShroudfirePlugin::authorize_admin(int slot, bool root) const {
    if (slot == 0) return true;
    if (slot < 1 || slot > shroudfire::kMaxPlayers) return false;
    const auto &player = mode_.player(slot);
    if (!player.connected || player.bot) return false;
    const auto admin = admins_.find(player.steam_id);
    return admin != admins_.end() && admin->second >= (root ? 2 : 1);
}

void ShroudfirePlugin::admin_reply(int slot, const char *message) const {
    if (slot == 0) META_CONPRINTF("%s\n", message);
    else if (engine_ && slot > 0 && slot <= shroudfire::kMaxPlayers) {
        std::string line(message);
        line.push_back('\n');
        engine_->ClientPrintf(CPlayerSlot(slot - 1), line.c_str());
    }
}

int ShroudfirePlugin::find_player(const char *argument) const {
    if (!argument || !*argument) return 0;
    char *end = nullptr;
    const uint64_t numeric = std::strtoull(argument, &end, 10);
    if (end && *end == '\0') {
        if (numeric >= 1 && numeric <= shroudfire::kMaxPlayers &&
            mode_.player(static_cast<int>(numeric)).connected &&
            !mode_.player(static_cast<int>(numeric)).bot) return static_cast<int>(numeric);
        for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot)
            if (mode_.player(slot).connected && !mode_.player(slot).bot &&
                mode_.player(slot).steam_id == numeric) return slot;
        return 0;
    }
    std::string query(argument);
    std::transform(query.begin(), query.end(), query.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    int found = 0;
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        if (!mode_.player(slot).connected || mode_.player(slot).bot) continue;
        std::string name = player_names_[slot];
        std::transform(name.begin(), name.end(), name.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (name.find(query) == std::string::npos) continue;
        if (found) return 0;
        found = slot;
    }
    return found;
}

void ShroudfirePlugin::command_update_notice(int slot, const CCommand &args) {
    if (slot != 0) {
        admin_reply(slot, "[Shroudfire] Server console only.");
        return;
    }
    char *end = nullptr;
    const long seconds = args.ArgC() == 2 ? std::strtol(args.Arg(1), &end, 10) : -1;
    if (!end || end == args.Arg(1) || *end != '\0' || seconds < 1 || seconds > 3600) {
        admin_reply(0, "[Shroudfire] Usage: sf_update_notice <seconds: 1-3600>");
        return;
    }
    char message[192];
    std::snprintf(message, sizeof(message),
        " \x04[Shroudfire]\x01 Restarting server to apply new CS2 update in %ld seconds.", seconds);
    for (int player = 1; player <= shroudfire::kMaxPlayers; ++player) {
        if (mode_.player(player).connected && !mode_.player(player).bot)
            engine_adapter_.client_print(player, 3, message);
    }
    META_CONPRINTF("[Shroudfire] Restarting server to apply new CS2 update in %ld seconds.\n", seconds);
}

void ShroudfirePlugin::command_status(int slot) {
    if (!authorize_admin(slot, false)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    sync_settings();
    char line[512];
    std::snprintf(line, sizeof(line), "[Shroudfire] State: %s | round: %s | humans: %d | target Invisant: %d",
        mode_.active() ? "active" : mode_.settings().enabled ? "waiting" : "disabled",
        mode_.round_live() ? "active" : "between rounds",
        mode_.active_humans(), mode_.desired_invisant_count());
    admin_reply(slot, line);
    const std::string destination = next_map();
    const std::string round_limit = sf_map_max_rounds.Get() > 0
        ? std::to_string(sf_map_max_rounds.Get()) : "off";
    char time_limit[32];
    if (sf_map_time_limit.Get() > 0)
        std::snprintf(time_limit, sizeof(time_limit), "%g", sf_map_time_limit.Get());
    else std::snprintf(time_limit, sizeof(time_limit), "off");
    std::snprintf(line, sizeof(line),
        "Map: %s | completed rounds %d (limit %s) | play time %.1f min (limit %s) | next: %s",
        map_rotation_.current.c_str(), map_rotation_.rounds, round_limit.c_str(),
        map_rotation_.played_seconds / 60.0, time_limit,
        destination.empty() ? "none available" : destination.c_str());
    admin_reply(slot, line);
    std::snprintf(line, sizeof(line), "Settings: %d Armitive per Invisant | round %ds | AFK death %ds | AFK limit %d",
        mode_.settings().armitive_per_invisant, mode_.settings().round_seconds,
        mode_.settings().afk_seconds, mode_.settings().afk_limit);
    admin_reply(slot, line);
    int terrorists = 0, counter_terrorists = 0;
    if (engine_adapter_.spawn_counts(terrorists, counter_terrorists)) {
        const int invisant_count = mode_.desired_invisant_count();
        const int armitive_count = mode_.active_players() - invisant_count;
        const int terrorist_need = sf_invisant_team.Get() == 2
            ? invisant_count : armitive_count;
        const int counter_need = sf_invisant_team.Get() == 3
            ? invisant_count : armitive_count;
        std::snprintf(line, sizeof(line), "Spawns: T %d/%d | CT %d/%d",
            terrorists, terrorist_need, counter_terrorists, counter_need);
        admin_reply(slot, line);
    }
    const float now = game_time();
    for (int target = 1; target <= shroudfire::kMaxPlayers; ++target) {
        const auto &p = mode_.player(target);
        if (!p.connected || p.bot) continue;
        const char *role = p.role == shroudfire::Role::invisant ? "Invisant"
            : p.role == shroudfire::Role::armitive ? "Armitive" : "spectator";
        std::snprintf(line, sizeof(line), "#%d %s: %s/%s | queue %d | earned %s | idle %.0fs | wait-next %s | spec-lock %s",
            target, player_names_[target].c_str(), role, p.alive ? "alive" : "dead",
            mode_.queue_position(target, now), p.earned_invisant ? "yes" : "no",
            (std::max)(0.0f, now - p.last_activity), p.waiting_next_round ? "yes" : "no",
            p.spectator_locked ? "yes" : "no");
        admin_reply(slot, line);
    }
}

void ShroudfirePlugin::command_cvar(int slot, const CCommand &args) {
    if (!authorize_admin(slot, false)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    if (args.ArgC() < 2) {
        admin_reply(slot, "[Shroudfire] Usage: sf_cvar <name> [value]");
        return;
    }
    ConVarRefAbstract cvar(args.Arg(1), true);
    if (!cvar.IsValidRef() || !cvar.IsConVarDataAvailable()) {
        admin_reply(slot, "[Shroudfire] Server cvar not found.");
        return;
    }
    const bool protected_value = cvar.IsFlagSet(FCVAR_PROTECTED);
    if (args.ArgC() > 2) {
        std::string value(args.Arg(2));
        for (int i = 3; i < args.ArgC(); ++i) {
            value += ' ';
            value += args.Arg(i);
        }
        if (!cvar.SetString(CUtlString(value.c_str()))) {
            admin_reply(slot, "[Shroudfire] Invalid value for server cvar.");
            return;
        }
        META_LOG(g_PLAPI, "Admin %llu set server cvar %s",
            static_cast<unsigned long long>(slot == 0 ? 0 : mode_.player(slot).steam_id),
            cvar.GetName());
    }
    std::string reply = "[Shroudfire] ";
    reply += cvar.GetName();
    reply += " = ";
    reply += protected_value ? "[protected]" : cvar.GetString().String();
    admin_reply(slot, reply.c_str());
}

void ShroudfirePlugin::command_setlevel(int slot, const CCommand &args) {
    if (!authorize_admin(slot, true)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    if (args.ArgC() != 4) {
        admin_reply(slot, "[Shroudfire] Usage: sf_setlevel <player> <invisant|armitive> <level>");
        return;
    }
    const int target = find_player(args.Arg(1));
    if (!target) {
        admin_reply(slot, "[Shroudfire] Player not found or name is ambiguous.");
        return;
    }
    std::string role_argument(args.Arg(2));
    std::transform(role_argument.begin(), role_argument.end(), role_argument.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    const shroudfire::Role role = role_argument == "invisant" ? shroudfire::Role::invisant
        : role_argument == "armitive"
        ? shroudfire::Role::armitive : shroudfire::Role::spectator;
    if (role == shroudfire::Role::spectator) {
        admin_reply(slot, "[Shroudfire] Role must be invisant or armitive.");
        return;
    }
    char *end = nullptr;
    const long level = std::strtol(args.Arg(3), &end, 10);
    if (!end || *end != '\0' || level < 0 || level > shroudfire::kMaxLevel) {
        admin_reply(slot, "[Shroudfire] Level must be 0-40.");
        return;
    }
    const auto before = mode_.attributes(target);
    const int previous_level = mode_.level(mode_.player(target).xp[
        role == shroudfire::Role::invisant ? 0 : 1]);
    mode_.set_level(target, role, static_cast<int>(level));
    process_actions();
    if (mode_.active() && mode_.player(target).alive && mode_.player(target).role == role) {
        const int team = role == shroudfire::Role::invisant
            ? sf_invisant_team.Get() : 5 - sf_invisant_team.Get();
        engine_adapter_.apply_upgrade_attributes(target, team, before, mode_.attributes(target));
    }
    char line[256];
    std::snprintf(line, sizeof(line), "[Shroudfire] %s: %s %d -> %ld (%d XP); upgrades reset; %ld points available.",
        player_names_[target].c_str(), role == shroudfire::Role::invisant ? "Invisant" : "Armitive",
        previous_level, level, mode_.required_xp(static_cast<int>(level)), level);
    admin_reply(slot, line);
    if (target != slot) engine_adapter_.client_print(target, 3, line);
    if (mode_.active() && mode_.player(target).role == role) show_menu(target);
}

void ShroudfirePlugin::command_nextmap(int slot, const CCommand &args) {
    if (!authorize_admin(slot, false)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    if (args.ArgC() > 2) {
        admin_reply(slot, "[Shroudfire] Usage: sf_nextmap [map|reset]");
        return;
    }
    if (args.ArgC() == 2) {
        const std::string map(args.Arg(1));
        if (map == "reset") map_rotation_.override_map.clear();
        else if (!map_available(map)) {
            admin_reply(slot, "[Shroudfire] Map is unavailable. Use an installed map name without its extension.");
            return;
        } else map_rotation_.override_map = map;
        map_rotation_.pending.clear();
        map_rotation_.retry_at = 0.0f;
        META_LOG(g_PLAPI, "Admin %llu set next map to %s",
            static_cast<unsigned long long>(slot == 0 ? 0 : mode_.player(slot).steam_id),
            map_rotation_.override_map.empty() ? "rotation" : map_rotation_.override_map.c_str());
    }
    const std::string destination = next_map();
    std::string reply = destination.empty()
        ? "[Shroudfire] No next map is available. Configure sf_map_rotation or sf_nextmap."
        : "[Shroudfire] Next map: " + destination + ".";
    if (!destination.empty()) {
        reply += sf_map_max_rounds.Get() == 0 && sf_map_time_limit.Get() == 0
            ? " Automatic map changes are disabled."
            : " Changes after the configured map limit.";
    }
    admin_reply(slot, reply.c_str());
}

void ShroudfirePlugin::command_restart(int slot) {
    if (!authorize_admin(slot, false)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    sync_settings();
    if (!mode_.admin_restart()) {
        admin_reply(slot, "[Shroudfire] No active round to restart.");
        return;
    }
    process_actions();
    admin_reply(slot, "[Shroudfire] Restarting the round.");
}

void ShroudfirePlugin::command_invisant(int slot) {
    if (!authorize_admin(slot, true)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    if (slot == 0) {
        admin_reply(slot, "[Shroudfire] Players only.");
        return;
    }
    sync_settings();
    if (!mode_.admin_force_invisant(slot)) {
        admin_reply(slot, "[Shroudfire] Mod unavailable or already Invisant.");
        return;
    }
    close_menu(slot);
    end_observation(slot, "administrator forced role");
    process_actions();
    admin_reply(slot, "[Shroudfire] Now Invisant.");
}

void ShroudfirePlugin::command_armitive(int slot) {
    if (!authorize_admin(slot, true)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    if (slot == 0) {
        admin_reply(slot, "[Shroudfire] Players only.");
        return;
    }
    sync_settings();
    if (!mode_.admin_force_armitive(slot)) {
        admin_reply(slot, "[Shroudfire] Mod unavailable or already Armitive.");
        return;
    }
    close_menu(slot);
    end_observation(slot, "administrator forced role");
    process_actions();
    admin_reply(slot, "[Shroudfire] Now Armitive.");
}

void ShroudfirePlugin::command_observe(int slot, const CCommand &args) {
    if (!authorize_admin(slot, false)) {
        admin_reply(slot, "[Shroudfire] Access denied.");
        return;
    }
    if (slot == 0) {
        admin_reply(slot, "[Shroudfire] Players only.");
        return;
    }
    sync_settings();
    if (!mode_.active() || !sf_admin_spectate.Get()) {
        admin_reply(slot, "[Shroudfire] Admin observation unavailable.");
        return;
    }
    int target = args.ArgC() > 1 ? find_player(args.Arg(1)) : 0;
    if (args.ArgC() <= 1) {
        for (int candidate = 1; candidate <= shroudfire::kMaxPlayers; ++candidate) {
            const auto &player = mode_.player(candidate);
            if (candidate == slot || !player.connected || !player.alive || player.waiting_next_round ||
                player.role == shroudfire::Role::spectator) continue;
            if (!target) target = candidate;
            if (player.role != shroudfire::Role::invisant) continue;
            target = candidate;
            break;
        }
    }
    if ((args.ArgC() > 1 && !target) || target == slot ||
        (target && (!mode_.player(target).connected || !mode_.player(target).alive ||
                    mode_.player(target).waiting_next_round ||
                    mode_.player(target).role == shroudfire::Role::spectator))) {
        admin_reply(slot, "[Shroudfire] No eligible living player found.");
        return;
    }
    end_observation(slot, "administrator selected another target");
    pending_observers_.erase(std::remove_if(pending_observers_.begin(), pending_observers_.end(),
        [slot](const PendingObserve &pending) { return pending.viewer == slot; }),
        pending_observers_.end());
    const auto viewer_steam = mode_.player(slot).steam_id;
    const auto target_steam = target ? mode_.player(target).steam_id : 0;
    // Observing in a two-player session must not demote or respawn the target.
    mode_.join_spectator(slot, true);
    // Prevent native round elimination before the last opponent leaves the team.
    sync_game_cvars();
    process_actions();
    const float now = game_time();
    pending_observers_.push_back({slot, target, viewer_steam, target_steam,
                                  now + 0.2f, now + 5.0f});
    admin_reply(slot, target
        ? (mode_.player(target).role == shroudfire::Role::invisant
            ? "[Shroudfire] Moving to Spectator to observe the Invisant."
            : "[Shroudfire] Moving to Spectator to observe the Armitive.")
        : "[Shroudfire] Moving to Spectator with a free camera.");
}

void ShroudfirePlugin::show_menu(int slot) {
    if (slot < 1 || slot > shroudfire::kMaxPlayers) return;
    if (!mode_.active()) return;
    const auto &player = mode_.player(slot);
    if (!player.connected || player.bot || player.role == shroudfire::Role::spectator) return;
    if (!menu_open_[slot] && !guide_open_[slot]) {
        activity_sample_ready_[slot] = false;
        panel_menu_input_[slot] = {};
    }
    guide_open_[slot] = false;
    guide_last_html_[slot].clear();
    guide_last_sent_at_[slot] = 0.0f;
    if (!menu_open_[slot] || menu_role_[slot] != player.role) {
        menu_selection_[slot] = 0;
    }
    menu_last_html_[slot].clear();
    menu_last_sent_at_[slot] = 0.0f;
    menu_open_[slot] = true;
    menu_role_[slot] = player.role;
    menu_first_send_at_[slot] = game_time() + 0.15f;
}

void ShroudfirePlugin::close_menu(int slot) {
    if (slot < 1 || slot > shroudfire::kMaxPlayers) return;
    if (!menu_open_[slot]) return;
    menu_open_[slot] = false;
    menu_last_html_[slot].clear();
    panel_menu_input_[slot] = {};
    menu_last_sent_at_[slot] = 0.0f;
    menu_first_send_at_[slot] = 0.0f;
    // Replace the last panel now and request its expiry; zero leaves a growing alert strip.
    engine_adapter_.center_html(slot, game_events_, " ", -1);
    update_hud(game_time());
}

void ShroudfirePlugin::show_guide(int slot, GuideKind kind) {
    if (slot < 1 || slot > shroudfire::kMaxPlayers) return;
    const auto &player = mode_.player(slot);
    if (!player.connected || player.bot) return;
    if (!menu_open_[slot] && !guide_open_[slot]) {
        activity_sample_ready_[slot] = false;
        panel_menu_input_[slot] = {};
    }
    menu_open_[slot] = false;
    menu_last_html_[slot].clear();
    menu_last_sent_at_[slot] = 0.0f;
    guide_open_[slot] = true;
    guide_kind_[slot] = kind;
    guide_page_[slot] = 0;
    guide_option_selection_[slot] = 0;
    guide_last_html_[slot].clear();
    guide_last_sent_at_[slot] = 0.0f;
    // The chat command reaches the server before the client's chat UI settles.
    // Drawing the panel on a later frame avoids the client discarding that first event.
    guide_first_send_at_[slot] = game_time() + 0.15f;
}

void ShroudfirePlugin::close_guide(int slot) {
    if (slot < 1 || slot > shroudfire::kMaxPlayers || !guide_open_[slot]) return;
    guide_open_[slot] = false;
    panel_menu_input_[slot] = {};
    guide_last_html_[slot].clear();
    guide_last_sent_at_[slot] = 0.0f;
    guide_first_send_at_[slot] = 0.0f;
    engine_adapter_.center_html(slot, game_events_, " ", -1);
    update_hud(game_time());
}

void ShroudfirePlugin::show_audio_settings(int slot) {
    // Audio Settings toggles Round Start, Round End, Kill Streak, Starvation, Timeout, Camping, AFK, and Menu sounds; all default to ON.
    if (slot < 1 || slot > shroudfire::kMaxPlayers ||
        !mode_.player(slot).connected || mode_.player(slot).bot) return;
    audio_menus_[slot] = {};
    show_guide(slot, GuideKind::audio);
}

void ShroudfirePlugin::render_guide(int slot) {
    if (slot < 1 || slot > shroudfire::kMaxPlayers || !guide_open_[slot]) return;
    const auto &player = mode_.player(slot);
    if (!player.connected || player.bot) {
        close_guide(slot);
        return;
    }
    const GuideKind kind = guide_kind_[slot];
    const float now = game_time();
    if (now < guide_first_send_at_[slot]) return;
    // Ranking walks the saved player cache; refresh at the panel's send rate.
    // Navigation clears the cached HTML so page changes still render immediately.
    if (kind == GuideKind::rank && !guide_last_html_[slot].empty() &&
        now - guide_last_sent_at_[slot] < 0.75f) return;
    const char *const *pages = kind == GuideKind::cheat ? kCheatPages.data()
        : kind == GuideKind::upgrades ? kUpgradePages.data()
        : kind == GuideKind::known_bugs ? kKnownBugPages.data() : kHelpPages.data();
    const int content_pages = static_cast<int>(kind == GuideKind::audio ? 0 : kind == GuideKind::rank ? 2
        : kind == GuideKind::cheat ? kCheatPages.size()
        : kind == GuideKind::upgrades ? kUpgradePages.size()
        : kind == GuideKind::known_bugs ? kKnownBugPages.size() : kHelpPages.size());
    const int page_count = content_pages + 1;
    const int page = guide_page_[slot];
    if (page < 0 || page >= page_count) return;
    char title[128];
    std::snprintf(title, sizeof(title),
        "<b><font color='%s'>%s</font></b> "
        "<font color='#ffffff'>%d/%d</font><br>",
        kind == GuideKind::rank && page < 2 ? (page == 0 ? "#ff7169" : "#78b9ff") : "#b7d1db",
        kind == GuideKind::cheat ? "ANTI CHEAT"
        : kind == GuideKind::upgrades ? "UPGRADES"
        : kind == GuideKind::rank ? (page == 0 ? "ARMITIVE STATS" : page == 1 ? "INVISANT STATS" : "RANK")
        : kind == GuideKind::known_bugs ? "KNOWN BUGS" : "HOW TO PLAY",
        page + 1, page_count);
    std::string html = title;
    if (kind == GuideKind::audio) {
        html = audio_menus_[slot].html(player.muted_audio);
    } else if (kind == GuideKind::rank && page < content_pages) {
        // Live records replace the saved cache, including spectators, without disk I/O.
        for (int other = 1; other <= shroudfire::kMaxPlayers; ++other) {
            const auto &connected = mode_.player(other);
            if (connected.connected && !connected.bot)
                rankings_.update(mode_.profile(other, 0));
        }
        const auto role = page == 0 ? shroudfire::Role::armitive : shroudfire::Role::invisant;
        html += shroudfire::rank_stats_html(player.stats[page == 0 ? 1 : 0],
            rankings_.rank(player.steam_id, role));
    } else if (page < content_pages) {
        html += pages[page];
    } else {
        // Six links plus the title and controls fill the HUD; omit the OPTIONS heading.
        const char *const help_options[] = {"Anti Cheat", "Known Bugs", "Upgrade Info", "Upgrade Menu", "Audio Settings", "Exit Menu"};
        const char *const upgrade_options[] = {"Help", "Anti Cheat", "Known Bugs", "Upgrade Menu", "Audio Settings", "Exit Menu"};
        const char *const cheat_options[] = {"Help", "Known Bugs", "Upgrade Info", "Upgrade Menu", "Audio Settings", "Exit Menu"};
        // Known Bugs and Rank share these links; Rank omits Known Bugs to fit six rows.
        const char *const shared_options[] = {"Help", "Anti Cheat", "Upgrade Info", "Upgrade Menu", "Audio Settings", "Exit Menu"};
        const char *const *options = kind == GuideKind::help ? help_options
            : kind == GuideKind::upgrades ? upgrade_options
            : kind == GuideKind::cheat ? cheat_options : shared_options;
        const int option_count = 6;
        for (int option = 0; option < option_count; ++option) {
            if (option == guide_option_selection_[slot])
                html += "<font color='#ffd36b'>&gt;[</font>";
            html += "<font color='#c69bff'>";
            html += options[option];
            html += "</font>";
            if (option == guide_option_selection_[slot])
                html += "<font color='#ffd36b'>]&lt;</font>";
            if (option + 1 < option_count) html += "<br>";
        }
    }
    if (kind == GuideKind::upgrades || kind == GuideKind::help) {
        auto replace = [&html](const char *marker, const std::string &value) {
            size_t at = 0;
            while ((at = html.find(marker, at)) != std::string::npos) {
                html.replace(at, std::strlen(marker), value);
                at += value.size();
            }
        };
        const auto &s = mode_.settings();
        if (kind == GuideKind::help) {
            replace("{KNIFE_XP}", std::to_string(s.xp_armitive_knife_percent));
            replace("{PISTOL_XP}", std::to_string(s.xp_armitive_pistol_percent));
            char knife_value[32];
            std::snprintf(knife_value, sizeof(knife_value), "%.5g", sf_knife_range.Get());
            replace("{KNIFE_RANGE}", knife_value);
            std::snprintf(knife_value, sizeof(knife_value), "%.5g", sf_knife_cone.Get());
            replace("{KNIFE_CONE}", knife_value);
            replace("{KNIFE_SPEED}", std::to_string(sf_knife_attack_speed_percent.Get()));
            replace("{KNIFE_DAMAGE_MIN}", std::to_string((std::min)(sf_knife_damage_min.Get(), sf_knife_damage_max.Get())));
            replace("{KNIFE_DAMAGE_MAX}", std::to_string((std::max)(sf_knife_damage_min.Get(), sf_knife_damage_max.Get())));
            char starvation_value[32];
            for (const auto &[marker, value] : std::array<std::pair<const char *, float>, 7> {{
                {"{STARVATION_ROUND_DELAY}", s.starvation_round_delay},
                {"{STARVATION_COMBAT_DELAY}", s.starvation_combat_delay},
                {"{STARVATION_DAMAGE}", static_cast<float>(s.starvation_damage)},
                {"{STARVATION_INTERVAL}", s.starvation_interval},
                {"{STARVATION_ACCELERATION_DELAY}", s.starvation_acceleration_delay},
                {"{STARVATION_ACCELERATED_INTERVAL}", s.starvation_accelerated_interval},
                {"{STARVATION_ACCELERATED_DAMAGE}", static_cast<float>(s.starvation_accelerated_damage)}
            }}) {
                std::snprintf(starvation_value, sizeof(starvation_value), "%.5g", value);
                replace(marker, starvation_value);
            }
            replace("{TIMEOUT_RESULT}", s.timeout_kill_invisant ? "die." : "revealed.");
        }
        if (kind == GuideKind::upgrades) {
            replace("{N1}", std::to_string(s.normal_upgrade_levels[0]));
            replace("{N2}", std::to_string(s.normal_upgrade_levels[1]));
            replace("{N3}", std::to_string(s.normal_upgrade_levels[2]));
            replace("{N4}", std::to_string(s.normal_upgrade_levels[3]));
            replace("{N5}", std::to_string(s.normal_upgrade_levels[4]));
            replace("{U1}", std::to_string(s.ultimate_upgrade_levels[0]));
            replace("{U2}", std::to_string(s.ultimate_upgrade_levels[1]));
            replace("{U3}", std::to_string(s.ultimate_upgrade_levels[2]));
            replace("{U4}", std::to_string(s.ultimate_upgrade_levels[3]));
            replace("{U5}", std::to_string(s.ultimate_upgrade_levels[4]));
            replace("{FORTITUDE}", std::to_string(s.fortitude_health_per_stage));
            replace("{RECOVERY}", std::to_string(s.recovery_health_per_stage));
            replace("{RECOVERY_INTERVAL}", std::to_string(s.recovery_interval));
            replace("{OVERLOAD}", std::to_string(s.overload_magazine_percent));
            replace("{STOCKPILE}", std::to_string(s.stockpile_clips_per_stage));
            replace("{BARRAGE}", std::to_string(s.barrage_fire_rate_percent));
            replace("{CONTROL}", std::to_string(s.control_reduction_percent));
            replace("{DEXTERITY}", std::to_string(s.dexterity_speed_percent));
            replace("{LIGHTWEIGHT}", std::to_string(s.lightweight_gravity_percent));
            replace("{MOMENTUM}", std::to_string(s.momentum_run_percent));
            replace("{RESILIENCE}", std::to_string(s.resilience_armor_per_stage));
            replace("{HELMET_STAGE}", std::to_string(s.resilience_helmet_stage));
            replace("{REFORGING}", std::to_string(s.reforging_armor_per_stage));
            replace("{REFORGING_INTERVAL}", std::to_string(s.reforging_interval));
            replace("{VAMPIRISM}", std::to_string(s.vampirism_health_per_stage));
            replace("{BLOODLUST}", std::to_string(s.bloodlust_health_per_stage));
            replace("{MOBILITY}", std::to_string(s.mobility_crouch_percent));
            replace("{SURGE_BASE}", std::to_string(s.surge_base_speed));
            replace("{SURGE_STEP}", std::to_string(s.surge_speed_per_stage));
            char seconds[32];
            std::snprintf(seconds, sizeof(seconds), "%g", s.ward_base_seconds);
            replace("{WARD_BASE}", seconds);
            std::snprintf(seconds, sizeof(seconds), "%g", s.ward_seconds_per_stage);
            replace("{WARD_STEP}", seconds);
        }
    }
    html += "<br><font color='#b7d1db'>Scroll </font><font color='#ffd36b'>WSAD</font>"
        "<font color='#b7d1db'> Select </font><font color='#ffd36b'>Space</font>"
        "<font color='#b7d1db'> Exit </font><font color='#ffd36b'>R</font>";
    if (html == guide_last_html_[slot] &&
        now - guide_last_sent_at_[slot] < 0.75f) return;
    if (engine_adapter_.center_html(slot, game_events_, html.c_str(), 600)) {
        guide_last_html_[slot] = std::move(html);
        guide_last_sent_at_[slot] = now;
    } else
        engine_adapter_.client_print(slot, 3, " \x04[Shroudfire]\x01 Guide panel unavailable.");
}

void ShroudfirePlugin::render_menu(int slot) {
    if (slot < 1 || slot > shroudfire::kMaxPlayers || !menu_open_[slot]) return;
    const auto &player = mode_.player(slot);
    if (!mode_.active() || !player.connected || player.bot ||
        player.role != menu_role_[slot]) {
        close_menu(slot);
        return;
    }
    const bool invisant_role = player.role == shroudfire::Role::invisant;
    const char *accent_color = invisant_role ? "#78b9ff" : "#ff7169";
    const int first = invisant_role ? 0 : 8;
    const int role_index = invisant_role ? 0 : 1;
    const auto &s = mode_.settings();
    char line[192];
    const int level = mode_.level(player.xp[role_index]);
    const int selected = menu_selection_[slot];
    const int page = selected < 8 ? selected / 4 : 2;
    const int first_visible = page * 4;
    const int visible_count = page == 2 ? 5 : 4;
    std::string html;
    html.reserve(800);
    std::snprintf(line, sizeof(line),
        "<b><font color='%s'>%s Upgrades</font></b> "
        "<font color='#ffffff'>%d/3</font><br>",
        accent_color, invisant_role ? "Invisant" : "Armitive", page + 1);
    html += line;
    std::snprintf(line, sizeof(line),
        "<font color='%s'>Level</font> <font color='#ffffff'>%d</font> "
        "<font color='%s'>Points</font> <font color='#ffffff'>%d</font><br>",
        accent_color, level, accent_color, player.points[role_index]);
    html += line;
    for (int choice = first_visible; choice < first_visible + visible_count; ++choice) {
        if (choice < 8) {
            const int stage = player.upgrades[first + choice];
            const char *name = first + choice == 7 ? "Ultimate: Surge" :
                first + choice == 15 ? "Ultimate: Ward" : kUpgradeNames[first + choice];
            const char *color = stage == 5 ? "#a6ddb3" :
                level < mode_.required_upgrade_level(
                    static_cast<shroudfire::Upgrade>(first + choice), stage) ? "#ffae61" : "#ffffff";
            if (choice == selected) {
                std::snprintf(line, sizeof(line),
                    "<font color='#ffd36b'>&gt;[</font>"
                    "<font color='%s'>%s %d/5</font>"
                    "<font color='#ffd36b'>]&lt;</font><br>",
                    color, name, stage);
            } else {
                std::snprintf(line, sizeof(line),
                    "<font color='%s'>%s %d/5</font><br>",
                    color, name, stage);
            }
        } else {
            const char *name = choice == 8 ? "Rank" :
                               choice == 9 ? "Known Bugs" :
                choice == 10 ? "Upgrade Info" :
                choice == 11 ? "Reset Points" : "Exit Menu";
            const char *color = "#c69bff";
            if (choice == selected) {
                std::snprintf(line, sizeof(line),
                    "<font color='#ffd36b'>&gt;[</font>"
                    "<font color='%s'>%s</font>"
                    "<font color='#ffd36b'>]&lt;</font><br>", color, name);
            } else {
                std::snprintf(line, sizeof(line),
                    "<font color='%s'>%s</font><br>", color, name);
            }
        }
        html += line;
        if (choice != selected || choice >= 8) continue;
        char info[96] = {};
        const int stage = player.upgrades[first + choice];
        switch (first + choice) {
        case 0: std::snprintf(info, sizeof(info), "%d%% gravity", 100 - s.lightweight_gravity_percent * stage); break;
        case 1: std::snprintf(info, sizeof(info), "%d%% run speed", 100 + s.momentum_run_percent * stage); break;
        case 2: std::snprintf(info, sizeof(info), "%d max armor", s.invisant_base_armor + s.resilience_armor_per_stage * stage); break;
        case 3: std::snprintf(info, sizeof(info), "%d armor regen / %ds", s.reforging_armor_per_stage * stage, s.reforging_interval); break;
        case 4: std::snprintf(info, sizeof(info), "%d health on hit", s.vampirism_health_per_stage * stage); break;
        case 5: std::snprintf(info, sizeof(info), "%d health on kill", s.bloodlust_health_per_stage * stage); break;
        case 6: std::snprintf(info, sizeof(info), "%d%% crouch speed", 100 + s.mobility_crouch_percent * stage); break;
        case 7: std::snprintf(info, sizeof(info), "%d speed boost", stage ? s.surge_base_speed + s.surge_speed_per_stage * (stage - 1) : 0); break;
        case 8: std::snprintf(info, sizeof(info), "%d max health", s.armitive_base_health + s.fortitude_health_per_stage * stage); break;
        case 9: std::snprintf(info, sizeof(info), "%d health regen / %ds", s.recovery_health_per_stage * stage, s.recovery_interval); break;
        case 10: std::snprintf(info, sizeof(info), "%d%% ammo capacity", 100 + s.overload_magazine_percent * stage); break;
        case 11: std::snprintf(info, sizeof(info), "%d extra ammo clip", s.stockpile_clips_per_stage * stage); break;
        case 12: std::snprintf(info, sizeof(info), "%d%% fire rate", 100 + s.barrage_fire_rate_percent * stage); break;
        case 13: std::snprintf(info, sizeof(info), "%d%% less recoil/spread", s.control_reduction_percent * stage); break;
        case 14: std::snprintf(info, sizeof(info), "%d%% reload/switch time", 100 - s.dexterity_speed_percent * stage); break;
        case 15: std::snprintf(info, sizeof(info), "%g seconds immunity",
            stage ? s.ward_base_seconds + s.ward_seconds_per_stage * (stage - 1) : 0.0); break;
        }
        html += "<font color='#c0c0c0'>";
        html += info;
        if (stage > 0 && player.upgrade_pending[first + choice])
            html += " (next round)";
        html += "</font><br>";
    }
    html += "<font color='#b7d1db'>Scroll </font><font color='#ffd36b'>WSAD</font>"
        "<font color='#b7d1db'> Select </font><font color='#ffd36b'>Space</font>"
        "<font color='#b7d1db'> Exit </font><font color='#ffd36b'>R</font>";
    const float now = game_time();
    if (now < menu_first_send_at_[slot]) return;
    if (html == menu_last_html_[slot] &&
        now - menu_last_sent_at_[slot] < 0.75f) return;
    if (engine_adapter_.center_html(slot, game_events_, html.c_str(), 600)) {
        menu_last_html_[slot] = std::move(html);
        menu_last_sent_at_[slot] = now;
    } else
        engine_adapter_.client_print(slot, 3,
            "[Shroudfire] Upgrade panel unavailable. Say 1-8, reset, or close.");
}

void ShroudfirePlugin::print_upgrade_purchase(int slot, shroudfire::Upgrade upgrade) const {
    const auto &player = mode_.player(slot);
    const auto &s = mode_.settings();
    const int choice = static_cast<int>(upgrade);
    const int stage = player.upgrades[choice];
    const bool invisant_role = choice < 8;
    const char role_color = invisant_role ? kChatBlue : kChatRed;
    std::string message = kChatPrefix;
    message += role_color;
    if (choice == 7 || choice == 15) message += "Ultimate: ";
    message += kUpgradeNames[choice];
    message += " [" + std::to_string(stage) + "/5]";
    message += kChatDefault;
    auto value = [&message](int amount, const char *unit = "") {
        message += kChatValue;
        message += std::to_string(amount);
        message += unit;
        message += kChatDefault;
    };
    switch (choice) {
    case 0: message += " You have "; value(s.lightweight_gravity_percent * stage, "%"); message += " less gravity"; break;
    case 1: message += " You run "; value(s.momentum_run_percent * stage, "%"); message += " faster"; break;
    case 2:
        message += " You have "; value(s.invisant_base_armor + s.resilience_armor_per_stage * stage);
        message += stage >= s.resilience_helmet_stage
            ? " max armor and helmet" : " max armor";
        break;
    case 3:
        message += " You gain "; value(s.reforging_armor_per_stage * stage);
        message += " armor every "; value(s.reforging_interval, "s");
        message += " while not taking damage";
        break;
    case 4: message += " You heal "; value(s.vampirism_health_per_stage * stage); message += " health per hit"; break;
    case 5: message += " You heal "; value(s.bloodlust_health_per_stage * stage); message += " health per kill"; break;
    case 6:
        message += " You crouch "; value(s.mobility_crouch_percent * stage, "%");
        message += " faster";
        break;
    case 7:
        message += " Press "; message += kChatValue; message += 'E'; message += kChatDefault;
        message += " for a "; value(s.surge_base_speed + s.surge_speed_per_stage * (stage - 1));
        message += " unit speed boost";
        break;
    case 8: message += " You have "; value(s.armitive_base_health + s.fortitude_health_per_stage * stage); message += " max health"; break;
    case 9:
        message += " You heal "; value(s.recovery_health_per_stage * stage);
        message += " health every "; value(s.recovery_interval, "s");
        message += " while not taking damage";
        break;
    case 10: message += " You have "; value(s.overload_magazine_percent * stage, "%"); message += " larger ammo capacity"; break;
    case 11:
        message += " You carry "; value(s.stockpile_clips_per_stage * stage);
        message += s.stockpile_clips_per_stage * stage == 1
            ? " extra reserve ammo clip" : " extra reserve ammo clips";
        break;
    case 12: message += " You fire weapons "; value(s.barrage_fire_rate_percent * stage, "%"); message += " faster"; break;
    case 13:
        message += " You have "; value(s.control_reduction_percent * stage, "%");
        message += " less weapon recoil and spread";
        break;
    case 14:
        message += " You reload and switch weapons in "; value(100 - s.dexterity_speed_percent * stage, "%");
        message += " of the normal time";
        break;
    case 15: {
        message += " Press "; message += kChatValue; message += 'E'; message += kChatDefault;
        message += " for ";
        message += kChatValue;
        char seconds[32];
        std::snprintf(seconds, sizeof(seconds), "%g",
            s.ward_base_seconds + s.ward_seconds_per_stage * (stage - 1));
        message += seconds;
        message += kChatDefault;
        message += " seconds of immunity";
        break;
    }
    }
    message += ' ';
    if (player.upgrade_pending[choice]) {
        message += kChatGold;
        message += "next round";
        message += kChatDefault;
        message += " as ";
    } else message += "as ";
    message += role_color;
    message += invisant_role ? "Invisant" : "Armitive";
    engine_adapter_.client_print(slot, 3, message.c_str());
}

bool ShroudfirePlugin::handle_chat_command(int slot, const char *message, bool from_chat) {
    if (!message || slot < 1 || slot > shroudfire::kMaxPlayers) return false;
    std::string input(message);
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"')
        input = input.substr(1, input.size() - 2);
    const size_t first = input.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return false;
    input = input.substr(first, input.find_last_not_of(" \t\r\n") - first + 1);
    std::transform(input.begin(), input.end(), input.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    const auto &speaker = mode_.player(slot);
    const auto hint_matches = shroudfire::match_chat_hints(input);
    if (from_chat && mode_.active() && speaker.connected && !speaker.bot) {
        const auto hint_now = std::chrono::steady_clock::now();
        const float pending_at = game_time() + 0.05f;
        if (sf_cheat_hint.Get() && hint_matches.cheat &&
            hint_now >= cheat_hint_available_at_) {
            cheat_hint_available_at_ = hint_now + std::chrono::seconds(sf_cheat_hint_cooldown.Get());
            cheat_hint_pending_at_ = pending_at;
        }
        if (sf_audio_hint.Get() && hint_matches.audio &&
            hint_now >= audio_hint_available_at_) {
            audio_hint_available_at_ = hint_now + std::chrono::seconds(sf_audio_hint_cooldown.Get());
            audio_hint_pending_at_ = pending_at;
        }
        if (sf_upgrade_hint.Get() && hint_matches.upgrade &&
            hint_now >= upgrade_hint_available_at_) {
            upgrade_hint_available_at_ = hint_now + std::chrono::seconds(sf_upgrade_hint_cooldown.Get());
            upgrade_hint_pending_at_ = pending_at;
        }
        if (sf_bugs_hint.Get() && hint_matches.bugs &&
            hint_now >= bugs_hint_available_at_) {
            bugs_hint_available_at_ = hint_now + std::chrono::seconds(sf_bugs_hint_cooldown.Get());
            bugs_hint_pending_at_ = pending_at;
        }
        if (sf_menu_hint.Get() && hint_matches.menu &&
            hint_now >= menu_hint_available_at_) {
            menu_hint_available_at_ = hint_now + std::chrono::seconds(sf_menu_hint_cooldown.Get());
            menu_hint_pending_at_ = pending_at;
        }
        if (sf_reset_hint.Get() && hint_matches.reset &&
            hint_now >= reset_hint_available_at_) {
            reset_hint_available_at_ = hint_now + std::chrono::seconds(sf_reset_hint_cooldown.Get());
            reset_hint_pending_at_ = pending_at;
        }
        if (sf_rank_hint.Get() && hint_matches.rank &&
            hint_now >= rank_hint_available_at_) {
            rank_hint_available_at_ = hint_now + std::chrono::seconds(sf_rank_hint_cooldown.Get());
            rank_hint_pending_at_ = pending_at;
        }
    }
    if (from_chat && mode_.active() && speaker.connected && !speaker.bot &&
        input.find("discord") != std::string::npos && discord_hint_pending_at_ == 0.0f &&
        sf_discord_message.Get() && !sf_discord_link.Get().IsEmpty())
        discord_hint_pending_at_ = game_time() + 0.05f;
    // audio: Open your audio settings menu.
    if (input == "audio") {
        show_audio_settings(slot);
        return true;
    }
    // bugs: Open the known bugs guide.
    if (input == "bugs") {
        show_guide(slot, GuideKind::known_bugs);
        return true;
    }
    // rank: Open your rank menu: Armitive stats, Invisant stats, and menu navigation.
    if (input == "rank") {
        show_guide(slot, GuideKind::rank);
        return true;
    }
    if (input == "menu") {
        show_menu(slot);
        return true;
    }
    if (input == "help") {
        show_guide(slot, GuideKind::help);
        return true;
    }
    // upgrade: Open the upgrade guide.
    if (input == "upgrade") {
        show_guide(slot, GuideKind::upgrades);
        return true;
    }
    if (input == "cheat") {
        show_guide(slot, GuideKind::cheat);
        return true;
    }
    if (guide_open_[slot] && input == "close") {
        close_guide(slot);
        return true;
    }
    // reset: Refund your current role's upgrades and open the upgrade menu.
    if (input == "reset") {
        show_menu(slot);
        if (!menu_open_[slot]) return true;
    }
    if (!menu_open_[slot]) return false;
    const auto &player = mode_.player(slot);
    if (!mode_.active() || !player.connected || player.bot || player.role != menu_role_[slot]) {
        close_menu(slot);
        return input == "reset";
    }
    const auto before = mode_.attributes(slot);
    const float now = game_time();
    if (input == "close") {
        play_sound(slot, shroudfire::kMenuExitSound, shroudfire::AudioCategory::menu);
        close_menu(slot);
        return true;
    }
    if (input == "reset") {
        play_sound(slot, "sounds/ui/beepclear.vsnd");
        const int first = player.role == shroudfire::Role::invisant ? 0 : 8;
        int reset_stages = 0;
        for (int i = first; i < first + 8; ++i) reset_stages += player.upgrades[i];
        mode_.reset_upgrades(slot, player.role, now);
        if (reset_stages > 0) {
            const bool invisant_role = first == 0;
            const int points = mode_.player(slot).points[invisant_role ? 0 : 1];
            std::string reply = kChatPrefix;
            reply += "Reset ";
            reply += kChatValue;
            reply += std::to_string(reset_stages);
            reply += ' ';
            reply += invisant_role ? kChatBlue : kChatRed;
            reply += invisant_role ? "Invisant" : "Armitive";
            reply += kChatDefault;
            reply += reset_stages == 1 ? " point (" : " points (";
            reply += kChatValue;
            reply += std::to_string(points);
            reply += kChatDefault;
            reply += " available)";
            engine_adapter_.client_print(slot, 3, reply.c_str());
        }
    } else if (input.size() == 1 && input[0] >= '1' && input[0] <= '8') {
        const int first = player.role == shroudfire::Role::invisant ? 0 : 8;
        const int choice = first + input[0] - '1';
        const auto upgrade = static_cast<shroudfire::Upgrade>(choice);
        if (!mode_.purchase(slot, upgrade, now)) {
            play_sound(slot, "sounds/ui/menu_invalid.vsnd");
            std::string reply = kChatPrefix;
            const int stage = player.upgrades[choice];
            if (stage >= shroudfire::kMaxUpgradeStage) {
                reply += first == 0 ? kChatBlue : kChatRed;
                if (choice == 7 || choice == 15) reply += "Ultimate: ";
                reply += kUpgradeNames[choice];
                reply += " [" + std::to_string(stage) + "/5]";
                reply += kChatDefault;
                reply += " is already at its maximum stage";
            } else if (player.points[first / 8] <= 0) {
                reply += "You're out of available points to spend";
            } else {
                const auto &levels = choice == 7 || choice == 15
                    ? mode_.settings().ultimate_upgrade_levels
                    : mode_.settings().normal_upgrade_levels;
                reply += choice == 7 || choice == 15
                    ? "Ultimate unlocks at levels " : "Upgrades unlock at levels ";
                for (int i = 0; i < 5; ++i) {
                    if (i > 0) reply += i == 4 ? ", and " : ", ";
                    reply += std::to_string(levels[i]);
                }
            }
            engine_adapter_.client_print(slot, 3, reply.c_str());
            return true;
        }
        play_sound(slot, "sounds/ui/menu_accept.vsnd");
        print_upgrade_purchase(slot, upgrade);
    } else return false;
    process_actions();
    if (profiles_) profiles_->save(mode_.profile(slot, static_cast<int>(std::time(nullptr))));
    if (mode_.player(slot).alive) {
        const auto attributes = mode_.attributes(slot);
        const int team = mode_.player(slot).role == shroudfire::Role::invisant
            ? sf_invisant_team.Get() : 5 - sf_invisant_team.Get();
        engine_adapter_.apply_upgrade_attributes(slot, team, before, attributes);
    }
    if (input == "reset") menu_selection_[slot] = 0;
    show_menu(slot);
    return true;
}

void ShroudfirePlugin::process_actions() {
    std::array<int, shroudfire::kMaxPlayers + 1> advanced_levels {};
    std::array<shroudfire::Role, shroudfire::kMaxPlayers + 1> advanced_roles {};
    const float now = game_time();
    for (const auto &action : mode_.take_actions()) {
        if (action.effect == shroudfire::Effect::round_completed) {
            if (mode_.active()) ++map_rotation_.rounds;
        } else if (action.effect == shroudfire::Effect::xp_changed) {
            if (profiles_)
                profiles_->save(mode_.profile(action.player, static_cast<int>(std::time(nullptr))));
            const int current_level = mode_.level(action.value);
            if (action.previous_level >= 0 && current_level > action.previous_level) {
                advanced_levels[action.player] = current_level;
                advanced_roles[action.player] = action.role;
                play_sound(action.player, "sounds/ui/xp_levelup.vsnd");
            }
        } else if (action.effect == shroudfire::Effect::post_round_kill ||
                   action.effect == shroudfire::Effect::kill_reward ||
                   action.effect == shroudfire::Effect::assist_reward) {
            CenterNotice &notice = center_notices_[action.player];
            const bool post_round = action.effect == shroudfire::Effect::post_round_kill;
            const bool assist = action.effect == shroudfire::Effect::assist_reward;
            const float reward_seconds = assist ? sf_hud_assist_seconds.Get() : sf_hud_kill_seconds.Get();
            const float footer_seconds = post_round ? sf_hud_no_rewards_seconds.Get() : 0.0f;
            notice.merge(post_round ? CenterNoticeKind::post_round_kill : assist
                ? CenterNoticeKind::assist : CenterNoticeKind::kill,
                         now, (std::max)(reward_seconds, footer_seconds));
            notice.combat_rewards.push_back({action.value, action.role,
                mode_.player(action.target).role, action.weapon,
                player_names_[action.target], action.message, post_round, now + reward_seconds, now, assist});
            if (post_round) {
                notice.no_rewards_until = now + footer_seconds;
                notice.no_rewards_at = now;
            }
        } else if (action.effect == shroudfire::Effect::invisant_priority) {
            CenterNotice &notice = center_notices_[action.player];
            notice.merge(CenterNoticeKind::kill, now, sf_hud_priority_seconds.Get());
            notice.invisant_priority = true;
            notice.priority_until = now + sf_hud_priority_seconds.Get();
            notice.priority_at = now;
        } else if (action.effect == shroudfire::Effect::round_win_reward) {
            CenterNotice &notice = center_notices_[action.player];
            notice.merge(CenterNoticeKind::kill, now, sf_hud_round_win_xp_seconds.Get());
            notice.winner = action.role;
            notice.xp = action.value;
            notice.xp_until = now + sf_hud_round_win_xp_seconds.Get();
            notice.xp_at = now;
            if (notice.xp > 0) notice.until = (std::max)(notice.until, notice.xp_until);
        } else if (action.effect == shroudfire::Effect::xp_notice) {
            engine_adapter_.client_print(action.player, 3, action.message.c_str());
        } else if (action.effect == shroudfire::Effect::starvation_start) {
            if (sf_custom_sounds.Get()) {
                int voice = std::uniform_int_distribution<int>(0,
                    static_cast<int>(kInvisantStarvationSounds.size()) -
                    (last_starvation_voice_ >= 0 ? 2 : 1))(random_engine_);
                if (last_starvation_voice_ >= 0 && voice >= last_starvation_voice_) ++voice;
                last_starvation_voice_ = voice;
                play_sound(action.player, kInvisantStarvationSounds[voice], shroudfire::AudioCategory::starvation);
            }
        } else if (action.effect == shroudfire::Effect::camping_start) {
            const auto &player = mode_.player(action.player);
            if (player.afk_death_pending || mode_.afk_countdown(action.player, now) > 0)
                continue;
            if (sf_custom_sounds.Get()) {
                int voice = std::uniform_int_distribution<int>(0,
                    static_cast<int>(kArmitiveCampingSounds.size()) -
                    (last_camping_voice_ >= 0 ? 2 : 1))(random_engine_);
                if (last_camping_voice_ >= 0 && voice >= last_camping_voice_) ++voice;
                last_camping_voice_ = voice;
                play_sound(action.player, kArmitiveCampingSounds[voice], shroudfire::AudioCategory::camping);
            }
        } else if (action.effect == shroudfire::Effect::streak_cue) {
            if (sf_custom_sounds.Get() && action.value >= 0 &&
                action.value < static_cast<int>(kKillStreakSounds.size()))
                play_sound(action.player, kKillStreakSounds[action.value], shroudfire::AudioCategory::kill_streak);
            else
                play_sound(action.player, action.value == 0
                    ? "sounds/ui/counter_beep.vsnd" : "sounds/buttons/blip1.vsnd");
        } else if (mode_.settings().enabled &&
                   (action.effect == shroudfire::Effect::change_team ||
                    action.effect == shroudfire::Effect::respawn ||
                    action.effect == shroudfire::Effect::restart_round ||
                    action.effect == shroudfire::Effect::end_round ||
                    action.effect == shroudfire::Effect::heal ||
                    action.effect == shroudfire::Effect::restore_armor ||
                    action.effect == shroudfire::Effect::kill_afk ||
                    action.effect == shroudfire::Effect::kill_timeout ||
                    action.effect == shroudfire::Effect::kick_afk ||
                    action.effect == shroudfire::Effect::starvation_damage ||
                    action.effect == shroudfire::Effect::camping_damage ||
                     action.effect == shroudfire::Effect::afk_warning_start ||
                    action.effect == shroudfire::Effect::afk_warning ||
                    action.effect == shroudfire::Effect::leap ||
                    action.effect == shroudfire::Effect::shield_start ||
                    action.effect == shroudfire::Effect::shield_end ||
                    action.effect == shroudfire::Effect::ability_ready)) {
            pending_engine_actions_.push_back({action, game_time() +
                (action.effect == shroudfire::Effect::respawn ? 0.1f : 0.0f),
                action.player > 0 ? mode_.player(action.player).steam_id : 0});
        }
    }
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        if (advanced_levels[slot] == 0) continue;
        CenterNotice &notice = center_notices_[slot];
        notice.merge(CenterNoticeKind::level, now, sf_hud_levelup_seconds.Get());
        notice.advanced_level = advanced_levels[slot];
        notice.advanced_role = advanced_roles[slot];
    }
    refresh_timeout_reveal(now);
    refresh_transmit_state();
}

void ShroudfirePlugin::apply_engine_actions(float now) {
    std::vector<PendingEngineAction> ready;
    auto pending = pending_engine_actions_.begin();
    while (pending != pending_engine_actions_.end()) {
        if (pending->due <= now) {
            ready.push_back(std::move(*pending));
            pending = pending_engine_actions_.erase(pending);
        } else ++pending;
    }
    if (!mode_.settings().enabled) {
        pending_spawns_.clear();
        pending_fire_acceleration_.clear();
        pending_observers_.clear();
        for (int viewer = 1; viewer <= shroudfire::kMaxPlayers; ++viewer)
            end_observation(viewer, "mod disabled");
        return;
    }
    for (const auto &queued : ready) {
        const auto &action = queued.action;
        if (action.player > 0 && (!mode_.player(action.player).connected ||
            mode_.player(action.player).steam_id != queued.steam_id)) continue;
        if (action.effect == shroudfire::Effect::change_team) {
            shield_next_sound_at_[action.player] = 0.0f;
            shield_tint_alpha_[action.player] = 0;
            engine_adapter_.apply_ammo_upgrades(action.player, 0, 0,
                mode_.settings().overload_magazine_percent,
                mode_.settings().stockpile_clips_per_stage);
            engine_adapter_.apply_dexterity(action.player, 0,
                mode_.settings().dexterity_speed_percent, 0.0f, 0);
            engine_adapter_.apply_claw_timing(action.player, 0.0f, 0, false,
                sf_knife_attack_speed_percent.Get());
            if (shield_applied_[action.player]) {
                bool ignored = false;
                engine_adapter_.set_damage_enabled(action.player,
                    shield_previous_damage_[action.player], ignored);
                engine_adapter_.screen_fade(action.player, 0, 17, 0, 0, 0, 0);
                shield_applied_[action.player] = false;
            }
            const int team = action.role == shroudfire::Role::spectator ? 1
                : action.role == shroudfire::Role::invisant ? sf_invisant_team.Get()
                : 5 - sf_invisant_team.Get();
            if (!engine_adapter_.change_team(action.player, team))
                META_CONPRINTF("[Shroudfire] Could not change team for slot %d.\n", action.player);
            else if (action.role != shroudfire::Role::spectator &&
                !mode_.player(action.player).bot && guide_hint_due_[action.player] == 0.0f)
                guide_hint_due_[action.player] = now + 3.0f;
        } else if (action.effect == shroudfire::Effect::respawn) {
            if (action.value == 3) {
                // A native round reset already retires the old pawn. Rebuild the
                // new role's loadout without adding a self-kill to the scoreboard.
                if (!engine_adapter_.respawn(action.player))
                    META_CONPRINTF("[Shroudfire] Could not spawn slot %d after round role change.\n",
                                   action.player);
            } else if (action.value == 1) {
                if (engine_adapter_.player_alive(action.player)) {
                    role_respawn_pending_[action.player] = true;
                    if (!engine_adapter_.commit_suicide(action.player)) {
                        role_respawn_pending_[action.player] = false;
                        META_CONPRINTF("[Shroudfire] Could not retire old pawn for slot %d.\n",
                                       action.player);
                        continue;
                    }
                }
                auto retry = action;
                retry.value = 2;
                pending_engine_actions_.push_back({retry, now + 0.1f, queued.steam_id});
            } else if (action.value == 2) {
                if (!engine_adapter_.respawn(action.player)) {
                    role_respawn_pending_[action.player] = false;
                    META_CONPRINTF("[Shroudfire] Could not respawn slot %d after role change.\n",
                                   action.player);
                    if (mode_.active_players() == 1 && mode_.admin_restart()) {
                        engine_adapter_.client_print(action.player, 3,
                            "[Shroudfire] Restarting the round to complete your role change.");
                        process_actions();
                    }
                }
            } else if (engine_adapter_.player_alive(action.player)) {
                pending_spawns_.push_back({action.player, queued.steam_id, now + 0.1f});
            } else if (!engine_adapter_.respawn(action.player))
                META_CONPRINTF("[Shroudfire] Could not respawn slot %d.\n", action.player);
        } else if (action.effect == shroudfire::Effect::restart_round) {
            round_start_voice_stop_passes_ = 0;
#if defined(__linux__)
            round_draw_stop_until_ = now + 2.0f;
#else
            round_draw_stop_until_ = 0.0f;
#endif
            restart_draw_suppression_until_ = now + 2.0f;
            std::string diagnostic;
            if (!engine_adapter_.terminate_round(-1, 0.0f, &diagnostic)) {
                META_CONPRINTF("[Shroudfire] Could not restart the round: %s.\n",
                               diagnostic.c_str());
                if (mode_.active_players() == 1 &&
                    mode_.alive(shroudfire::Role::armitive) + mode_.alive(shroudfire::Role::invisant) == 0 &&
                    initial_restart_attempts_++ < 30)
                    pending_initial_restart_ = now + 1.0f;
            }
#if defined(__linux__)
            else {
                for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                    if (!mode_.player(slot).connected || mode_.player(slot).bot) continue;
                    for (const char *name : {"Event.RoundDraw", "Announcer.RoundDraw.CS2_Classic"})
                        engine_adapter_.stop_sound_event_hash(slot,
                            MurmurHash2LowerCase(name, 0x53524332));
                }
            }
#endif
        } else if (action.effect == shroudfire::Effect::end_round) {
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                const auto &player = mode_.player(slot);
                if (!player.connected || player.bot) continue;
                CenterNotice &notice = center_notices_[slot];
                notice.merge(CenterNoticeKind::win, now, sf_hud_round_result_seconds.Get());
                notice.winner = action.role;
            }
            if (sf_custom_sounds.Get() && round_win_voice_at_ <= 0.0f) {
                round_win_voice_at_ = now + sf_round_win_sound_delay.Get();
                round_win_voice_role_ = action.role;
            }
            const bool timed_out = action.role == shroudfire::Role::armitive &&
                mode_.round_live() && mode_.freeze_ended() &&
                mode_.round_time_remaining(now) == 0;
            mode_.end_round(action.role, true, timed_out, now);
            round_info_until_ = 0.0f;
            process_actions();
            if (action.role == shroudfire::Role::spectator &&
                !draw_termination_requested_ &&
                !engine_adapter_.terminate_round(0, 10.0f))
                META_CONPRINTF("[Shroudfire] Could not resolve the suicide/AFK wipe as a draw.\n");
            // CS2 now resolves and scores the elimination itself. A second
            // termination here restarts the stock victory music.
            pending_round_winner_ = 0;
            pending_round_end_at_ = 0.0f;
        } else if (action.effect == shroudfire::Effect::heal &&
                   mode_.player(action.player).role == action.role) {
            engine_adapter_.heal(action.player, action.value,
                                 mode_.attributes(action.player).maximum_health);
        } else if (action.effect == shroudfire::Effect::restore_armor &&
                   mode_.player(action.player).role == action.role) {
            engine_adapter_.restore_armor(action.player, action.value,
                                          mode_.attributes(action.player).maximum_armor);
        } else if (action.effect == shroudfire::Effect::kill_timeout) {
            if (mode_.round_live() ||
                mode_.player(action.player).role != shroudfire::Role::invisant ||
                !engine_adapter_.player_alive(action.player)) continue;
            forced_death_cause_[action.player] = shroudfire::DeathCause::timeout;
            forced_death_pending_[action.player] = true;
            if (!engine_adapter_.commit_suicide(action.player) ||
                engine_adapter_.player_alive(action.player)) {
                forced_death_pending_[action.player] = false;
                META_CONPRINTF("[Shroudfire] Could not kill surviving Invisant slot %d at timeout.\n",
                    action.player);
            } else {
                META_CONPRINTF("[Shroudfire] Timeout killed Invisant slot %d (health %d).\n",
                    action.player, engine_adapter_.player_health(action.player));
            }
        } else if (action.effect == shroudfire::Effect::kill_afk) {
            forced_death_cause_[action.player] = shroudfire::DeathCause::afk;
            forced_death_pending_[action.player] = true;
            if (!engine_adapter_.commit_suicide(action.player))
                forced_death_pending_[action.player] = false;
        } else if (action.effect == shroudfire::Effect::starvation_damage ||
                   action.effect == shroudfire::Effect::camping_damage) {
            // Movement or combat may have ended the penalty after this action
            // was queued. Do not apply a stale health-drain tick.
            const auto &player = mode_.player(action.player);
            const bool starvation = action.effect == shroudfire::Effect::starvation_damage;
            if (!mode_.round_live() || !player.alive ||
                (starvation && (!player.starvation_active ||
                    player.role != shroudfire::Role::invisant)) ||
                (!starvation && (!player.camping_active ||
                    player.role != shroudfire::Role::armitive || player.afk_death_pending ||
                    mode_.afk_countdown(action.player, now) > 0))) continue;
            bool fatal = false;
            forced_death_cause_[action.player] = starvation
                ? shroudfire::DeathCause::starvation : shroudfire::DeathCause::camping;
            forced_death_pending_[action.player] = true;
            const bool drained = engine_adapter_.drain_health(action.player, action.value, fatal);
            if (drained && !starvation) mode_.damage_taken(action.player, now, false);
            if (!drained || !fatal)
                forced_death_pending_[action.player] = false;
        } else if (action.effect == shroudfire::Effect::kick_afk && engine_) {
            std::string name = player_names_[action.player];
            name.erase(std::remove_if(name.begin(), name.end(),
                [](unsigned char ch) { return ch < 0x20; }), name.end());
            std::string message = " \x04[Shroudfire]\x01 ";
            message += action.role == shroudfire::Role::invisant ? kChatBlue : kChatRed;
            message += name;
            message += kChatDefault;
            message += " was kicked for being \x10" "AFK";
            for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
                const auto &recipient = mode_.player(slot);
                if (recipient.connected && !recipient.bot)
                    engine_adapter_.client_print(slot, 3, message.c_str());
            }
            engine_->DisconnectClient(CPlayerSlot(action.player - 1), NETWORK_DISCONNECT_KICKED_IDLE,
                                      "Repeated inactivity");
        } else if (action.effect == shroudfire::Effect::afk_warning) {
            if (mode_.afk_countdown(action.player, now) > 0)
                play_sound(action.player, "sounds/ui/beep07.vsnd");
        } else if (action.effect == shroudfire::Effect::afk_warning_start) {
            if (mode_.afk_countdown(action.player, now) <= 0) continue;
            if (sf_custom_sounds.Get()) {
                int voice = std::uniform_int_distribution<int>(0,
                    static_cast<int>(kAfkWarningSounds.size()) -
                    (last_afk_warning_voice_ >= 0 ? 2 : 1))(random_engine_);
                if (last_afk_warning_voice_ >= 0 && voice >= last_afk_warning_voice_) ++voice;
                last_afk_warning_voice_ = voice;
                play_sound(action.player, kAfkWarningSounds[voice], shroudfire::AudioCategory::afk);
            }
        } else if (action.effect == shroudfire::Effect::leap) {
            engine_adapter_.leap(action.player, action.value, mode_.settings().surge_add_momentum);
            play_sound(action.player, "sounds/player/halloween/swish_01.vsnd");
        } else if (action.effect == shroudfire::Effect::shield_start) {
            bool previous = true;
            if (engine_adapter_.set_damage_enabled(action.player, false, previous)) {
                shield_previous_damage_[action.player] = previous;
                shield_applied_[action.player] = true;
                const int tint = sf_ward_screen_tint.Get();
                const bool tinted = tint == 0 || engine_adapter_.screen_fade(
                    action.player, 100, 2, 35, 190, 65, tint, action.value);
                if (!tinted)
                    META_CONPRINTF("[Shroudfire] Could not start shield tint for slot %d.\n",
                                   action.player);
                shield_tint_alpha_[action.player] = tinted ? tint : 0;
                play_sound(action.player, "sounds/buttons/blip1.vsnd");
                shield_next_sound_at_[action.player] = now + 0.75f;
            }
        } else if (action.effect == shroudfire::Effect::shield_end) {
            shield_next_sound_at_[action.player] = 0.0f;
            shield_tint_alpha_[action.player] = 0;
            if (shield_applied_[action.player]) {
                bool ignored = false;
                engine_adapter_.set_damage_enabled(action.player,
                    shield_previous_damage_[action.player], ignored);
                engine_adapter_.screen_fade(action.player, 0, 17, 0, 0, 0, 0);
                shield_applied_[action.player] = false;
            }
        } else if (action.effect == shroudfire::Effect::ability_ready) {
            const auto &player = mode_.player(action.player);
            if (!player.alive || player.role != action.role || player.waiting_next_round ||
                player.applied_upgrades[static_cast<int>(action.role == shroudfire::Role::invisant
                    ? shroudfire::Upgrade::leap : shroudfire::Upgrade::shield)] == 0) continue;
            CenterNotice &notice = center_notices_[action.player];
            notice.merge(CenterNoticeKind::ability, now, 5.0f);
            notice.ability_role = action.role;
            notice.ability_available_at = 0.0f;
            update_hud(now);
            play_sound(action.player, "sounds/ui/counter_beep.vsnd");
        }
    }
    auto spawn = pending_spawns_.begin();
    while (spawn != pending_spawns_.end()) {
        if (spawn->due > now) { ++spawn; continue; }
        const int slot = spawn->slot;
        if (mode_.player(slot).connected && mode_.player(slot).steam_id == spawn->steam_id &&
            mode_.player(slot).alive) {
            const auto &player = mode_.player(slot);
            const auto attributes = mode_.attributes(slot);
            const int team = player.role == shroudfire::Role::invisant
                ? sf_invisant_team.Get() : 5 - sf_invisant_team.Get();
            // A native restart can move a pawn after the role was assigned.
            // Reconcile the team before applying the role's loadout and stats.
            if (!engine_adapter_.change_team(slot, team))
                META_CONPRINTF("[Shroudfire] Could not restore assigned team for slot %d.\n", slot);
            engine_adapter_.apply_spawn_attributes(slot, team,
                player.role == shroudfire::Role::invisant,
                attributes.maximum_health, attributes.maximum_armor, attributes.helmet,
                attributes.gravity_scale);
            if (player.role == shroudfire::Role::invisant) {
                engine_adapter_.suppress_buy_zone(slot);
                const int dropped = engine_adapter_.drop_non_knife_weapons(slot);
                if (dropped < 0)
                    META_CONPRINTF("[Shroudfire] Could not verify knife-only inventory for slot %d.\n", slot);
                else if (dropped > 0)
                    META_CONPRINTF("[Shroudfire] Dropped %d non-knife weapon(s) from Invisant slot %d.\n",
                                   dropped, slot);
            }
            if (player.role == shroudfire::Role::invisant && !player.bot && engine_)
                engine_->ClientCommand(CPlayerSlot(slot - 1), "slot3");
            else if (player.role == shroudfire::Role::armitive) {
                EngineAdapter::Activity activity;
                if (engine_adapter_.sample_activity(slot, activity))
                    engine_adapter_.record_buy_zone_spawn(slot, activity.position);
            }
            const int role_index = player.role == shroudfire::Role::invisant ? 0 : 1;
            if (player.points[role_index] > 0) show_menu(slot);
        }
        spawn = pending_spawns_.erase(spawn);
    }
    auto shot = pending_fire_acceleration_.begin();
    while (shot != pending_fire_acceleration_.end()) {
        if (shot->due > now) { ++shot; continue; }
        const int slot = shot->slot;
        INetworkGameServer *game = network_service_ ? network_service_->GetIGameServer() : nullptr;
        if (game && mode_.player(slot).connected && mode_.player(slot).alive &&
            mode_.player(slot).steam_id == shot->steam_id &&
            mode_.player(slot).role == shroudfire::Role::armitive)
            engine_adapter_.accelerate_fire(slot, mode_.attributes(slot).fire_delay_scale,
                                            game->GetServerTick());
        shot = pending_fire_acceleration_.erase(shot);
    }
    auto observer = pending_observers_.begin();
    while (observer != pending_observers_.end()) {
        if (observer->due > now) { ++observer; continue; }
        const int viewer = observer->viewer;
        const int target = observer->target;
        if (mode_.active() && mode_.player(viewer).connected &&
            mode_.player(viewer).steam_id == observer->viewer_steam &&
            !mode_.player(viewer).bot && !mode_.player(viewer).alive &&
            mode_.player(viewer).role == shroudfire::Role::spectator &&
            sf_admin_spectate.Get() && authorize_admin(viewer, false) &&
            (!target || (mode_.player(target).steam_id == observer->target_steam &&
                mode_.can_spectate(viewer, target, sf_invisant_see_teammates.Get() != 0, true)))) {
            if (engine_adapter_.set_observer(viewer, target, target == 0)) {
                admin_observe_target_steam_[viewer] = observer->target_steam;
                admin_observe_started_at_[viewer] = now;
                admin_observe_target_[viewer].store(target ? target : -1, std::memory_order_release);
                META_LOG(g_PLAPI, "Admin observation started: administrator %llu, target %llu",
                    static_cast<unsigned long long>(observer->viewer_steam),
                    static_cast<unsigned long long>(observer->target_steam));
                engine_adapter_.client_print(viewer, 3, target
                    ? (mode_.player(target).role == shroudfire::Role::invisant
                        ? "[Shroudfire] Observing Invisant."
                        : "[Shroudfire] Observing Armitive.")
                    : "[Shroudfire] Spectator free camera ready.");
            } else if (now < observer->expires_at) {
                // ChangeTeam creates the observer pawn asynchronously.
                observer->due = now + 0.1f;
                ++observer;
                continue;
            } else {
                admin_reply(viewer, "[Shroudfire] Could not begin observation: spectator camera was not ready.");
            }
        } else if (mode_.player(viewer).connected &&
                   mode_.player(viewer).steam_id == observer->viewer_steam) {
            admin_reply(viewer, "[Shroudfire] Observation cancelled: viewer or target is no longer eligible.");
        }
        observer = pending_observers_.erase(observer);
    }
}
