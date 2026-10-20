#pragma once

#include "mode.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <vector>

enum class CenterNoticeKind { none, kill, win, level, post_round_kill, assist, ability };

inline constexpr char kInvisibilityWarningHtml[] =
    "<font color='#78b9ff'>Invisibility</font>"
    "<font color='#ffb900'> is about to fade</font><br>"
    "<font color='#ffb900'>Kill the </font><font color='#ff7169'>Armitive</font>"
    "<font color='#ffb900'> or be revealed</font>";

inline constexpr char kWaitingForPlayersHtml[] =
    "<font color='#ffb900'>Waiting for more players</font><br>"
    "<font color='#ffb900'>Type </font><font color='#b9a176'>help</font>"
    "<font color='#ffb900'> to learn </font><font color='#93c47d'>Shroudfire</font><br>"
    "<font color='#ffb900'>You will be first to play </font>"
    "<font color='#78b9ff'>Invisant</font>";

enum class CenterLinePriority { result, xp, info, footer };

struct CenterPanelLine {
    std::string html;
    float added_at = 0.0f;
    CenterLinePriority priority = CenterLinePriority::info;
};

struct CenterPanel {
    std::vector<CenterPanelLine> lines;

    std::string html(bool waiting_for_players = false) const {
        if (lines.empty() && waiting_for_players) return kWaitingForPlayersHtml;
        std::vector<size_t> selected;
        for (size_t index = 0; index < lines.size(); ++index) selected.push_back(index);
        if (selected.size() > 8) {
            std::sort(selected.begin(), selected.end(), [&](size_t left, size_t right) {
                if (lines[left].added_at != lines[right].added_at)
                    return lines[left].added_at > lines[right].added_at;
                return left > right;
            });
            selected.resize(8);
        }
        std::sort(selected.begin(), selected.end(), [&](size_t left, size_t right) {
            if (lines[left].priority != lines[right].priority)
                return lines[left].priority < lines[right].priority;
            if (lines[left].added_at != lines[right].added_at)
                return lines[left].added_at < lines[right].added_at;
            return left < right;
        });
        std::string result;
        for (size_t index : selected) {
            if (!result.empty()) result += "<br>";
            result += lines[index].html;
        }
        return result;
    }
};

struct CenterCombatReward {
    int xp = 0;
    shroudfire::Role recipient_role = shroudfire::Role::spectator;
    shroudfire::Role victim_role = shroudfire::Role::spectator;
    shroudfire::WeaponKind weapon = shroudfire::WeaponKind::other;
    std::string victim_name;
    std::string streak;
    bool post_round = false;
    float until = 0.0f;
    float added_at = 0.0f;
    bool assist = false;
};

struct CenterNotice {
    CenterNoticeKind kind = CenterNoticeKind::none;
    float until = 0.0f;
    bool visible = false;
    int xp = 0;
    int advanced_level = 0;
    shroudfire::Role advanced_role = shroudfire::Role::spectator;
    shroudfire::Role winner = shroudfire::Role::spectator;
    shroudfire::Role ability_role = shroudfire::Role::spectator;
    bool invisant_priority = false;
    float win_until = 0.0f;
    float xp_until = 0.0f;
    float level_until = 0.0f;
    float priority_until = 0.0f;
    float no_rewards_until = 0.0f;
    float ability_until = 0.0f;
    float ability_available_at = 0.0f;
    float refresh_at = 0.0f;
    float win_at = 0.0f;
    float xp_at = 0.0f;
    float level_at = 0.0f;
    float priority_at = 0.0f;
    float no_rewards_at = 0.0f;
    float ability_at = 0.0f;
    std::vector<CenterCombatReward> combat_rewards;

    void merge(CenterNoticeKind incoming, float now, float duration) {
        if (kind == CenterNoticeKind::none || until <= now) *this = {};
        if (kind == CenterNoticeKind::none || incoming == CenterNoticeKind::win)
            kind = incoming;
        float deadline = now + duration;
        if (incoming == CenterNoticeKind::win) {
            if (win_until <= now) {
                win_until = deadline;
                win_at = now;
            }
            deadline = win_until;
        }
        if (incoming == CenterNoticeKind::level) {
            level_until = now + duration;
            level_at = now;
        }
        if (incoming == CenterNoticeKind::ability) {
            ability_until = deadline;
            ability_at = now;
        }
        until = std::max(until, deadline);
        visible = false;
    }

    float next_expiry(float now) const {
        float next = until;
        for (float deadline : {win_until, xp_until, level_until, priority_until, no_rewards_until, ability_until})
            if (deadline > now) next = std::min(next, deadline);
        for (const auto &reward : combat_rewards)
            if (reward.until > now) next = std::min(next, reward.until);
        return next;
    }

    CenterPanel panel(float now, int afk_seconds = 0, float afk_at = 0.0f,
                      bool starving = false, float starvation_at = 0.0f,
                      bool camping = false, int camping_damage_seconds = 0,
                      int camping_distance = 0, float camping_at = 0.0f) const {
        CenterPanel result;
        if (now < win_until) {
            const bool invisible = winner == shroudfire::Role::invisant;
            const bool draw = winner == shroudfire::Role::spectator;
            result.lines.push_back({std::string("<b><font color='") +
                (draw ? "#ffffff" : invisible ? "#78b9ff" : "#ff7169") + "'>" +
                (draw ? "ROUND DRAW" : invisible ? "INVISANT WIN" : "ARMITIVE WIN") +
                "</font></b>", win_at, CenterLinePriority::result});
        }
        if (xp > 0 && now < xp_until) {
            result.lines.push_back({"<font color='#93c47d'>+" + std::to_string(xp) + "</font>"
                "<font color='" + std::string(winner == shroudfire::Role::invisant
                    ? "#78b9ff" : "#ff7169") +
                "'> XP</font><font color='#ffffff'> round win</font>", xp_at, CenterLinePriority::xp});
        }
        for (const auto &reward : combat_rewards) {
            if (now >= reward.until) continue;
            std::string label = reward.assist ? "assist" : reward.post_round ? "kill" : reward.streak;
            if (!label.empty()) {
                std::transform(label.begin(), label.end(), label.begin(),
                    [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            } else if (reward.recipient_role == shroudfire::Role::armitive &&
                       reward.weapon == shroudfire::WeaponKind::pistol) {
                label = "pistol kill";
            } else if (reward.recipient_role == shroudfire::Role::armitive &&
                       reward.weapon == shroudfire::WeaponKind::knife) {
                label = "knife kill";
            } else label = "kill";
            // Reserve room for the XP and kill label without cutting a UTF-8 character.
            const std::string amount = std::to_string(reward.xp);
            const size_t name_limit = 32 - std::min<size_t>(23, amount.size() + 6 + label.size() + 3);
            std::string name;
            size_t characters = 0;
            for (unsigned char ch : reward.victim_name) {
                if ((ch & 0xc0) != 0x80 && ++characters > name_limit) {
                    name += "...";
                    break;
                }
                if (ch == '&') name += "&amp;";
                else if (ch == '<') name += "&lt;";
                else if (ch == '>') name += "&gt;";
                else if (ch >= ' ') name += static_cast<char>(ch);
            }
            if (name.empty()) name = "player";
            result.lines.push_back({"<font color='#93c47d'>+" + amount + "</font>"
                "<font color='" + std::string(reward.recipient_role == shroudfire::Role::invisant
                    ? "#78b9ff" : "#ff7169") + "'> XP</font>"
                "<font color='#ffffff'> " + label + " </font>"
                "<font color='" + std::string(reward.victim_role == shroudfire::Role::invisant
                    ? "#78b9ff" : "#ff7169") + "'>" + name + "</font>",
                reward.added_at, CenterLinePriority::xp});
        }
        if (advanced_level > 0 && now < level_until) {
            const bool invisible = advanced_role == shroudfire::Role::invisant;
            result.lines.push_back({"<font color='#ffffff'>You advanced to </font>"
                "<font color='" + std::string(invisible ? "#78b9ff" : "#ff7169") +
                "'>" + (invisible ? "Invisant" : "Armitive") + "</font>"
                "<font color='#ffffff'> level </font><font color='" +
                std::string(invisible ? "#78b9ff" : "#ff7169") + "'>" +
                std::to_string(advanced_level) + "</font>", level_at, CenterLinePriority::xp});
        }
        if (invisant_priority && now < priority_until) {
            result.lines.push_back({"<font color='#78b9ff'>Invisant</font>"
                "<font color='#b7d1db'> priority</font>"
                "<font color='#ffb900'> next round</font>", priority_at, CenterLinePriority::info});
        }
        if (now < ability_until && ability_role != shroudfire::Role::spectator) {
            const bool invisant = ability_role == shroudfire::Role::invisant;
            const int seconds = static_cast<int>(std::ceil(ability_available_at - now));
            std::string line = std::string("<font color='") +
                (invisant ? "#78b9ff" : "#ff7169") + "'>" +
                (invisant ? "Surge" : "Ward") + "</font><font color='#ffb900'> ready";
            if (seconds > 0)
                line += " in </font><font color='#ffffff'>" + std::to_string(seconds) +
                    "</font><font color='#ffb900'>" + (seconds == 1 ? " second" : " seconds");
            line += "</font>";
            result.lines.push_back({line, ability_at, CenterLinePriority::info});
        }
        if (afk_seconds > 0)
            result.lines.push_back({"<font color='#b9a176'>AFK</font>"
                "<font color='#ffb900'> death in </font>"
                "<font color='#b9a176'>" + std::to_string(afk_seconds) + "</font>"
                "<font color='#ffb900'>" + (afk_seconds == 1 ? std::string(" second") : " seconds") +
                "</font>", afk_at, CenterLinePriority::info});
        if (starving) {
            result.lines.push_back({"<font color='#ffb900'>You are dying of </font>"
                "<font color='#b9a176'>Starvation</font>", starvation_at, CenterLinePriority::info});
            result.lines.push_back({"<font color='#ffb900'>Deal or take damage or die</font>",
                starvation_at, CenterLinePriority::info});
        }
        // AFK removal wins when both inactivity systems reach their warning threshold together.
        if (camping && afk_seconds <= 0) {
            if (camping_damage_seconds > 0) {
                result.lines.push_back({"<font color='#b9a176'>Camping</font>"
                    "<font color='#ffb900'> health loss in </font>"
                    "<font color='#ffffff'>" + std::to_string(camping_damage_seconds) + "</font>"
                    "<font color='#ffb900'>" +
                    (camping_damage_seconds == 1 ? std::string(" second") : " seconds") +
                    "</font>", camping_at, CenterLinePriority::info});
            } else {
                result.lines.push_back({"<font color='#ffb900'>You are dying from </font>"
                    "<font color='#b9a176'>Camping</font>", camping_at, CenterLinePriority::info});
            }
            result.lines.push_back({"<font color='#ffb900'>Move </font>"
                "<font color='#ffffff'>" + std::to_string(camping_distance) + "</font>"
                "<font color='#ffb900'> units " +
                (camping_damage_seconds > 0 ? std::string("to reset") : "or die") + "</font>",
                camping_at, CenterLinePriority::info});
        }
        if (now < no_rewards_until)
            result.lines.push_back({"<font color='#ffb900'>No rewards after round end</font>",
                no_rewards_at, CenterLinePriority::footer});
        return result;
    }
};
