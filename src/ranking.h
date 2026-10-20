#pragma once

#include "mode.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <unordered_map>

namespace shroudfire {

inline double rank_score(const CombatStats &stats) {
    const double contributions = static_cast<double>(stats.kills) + stats.assists;
    if (contributions <= 0) return 0;
    const double events = contributions + stats.deaths;
    const double proportion = contributions / events;
    constexpr double z = 1.96;
    // Wilson's 95% lower bound discounts uncertain records. The additional
    // participation weight is half strength at 100 kills + assists, so a tiny
    // flawless record cannot dominate an established, effective player.
    // https://www.evanmiller.org/how-not-to-sort-by-average-rating.html
    const double lower = (proportion + z * z / (2 * events) - z * std::sqrt(
        (proportion * (1 - proportion) + z * z / (4 * events)) / events)) /
        (1 + z * z / events);
    return 1000 * std::clamp(lower, 0.0, 1.0) * contributions / (contributions + 100);
}

struct Rank {
    int position = 0;
    int players = 0;
    double score = 0;
};

inline std::string rank_stats_html(const CombatStats &stats, const Rank &rank) {
    char rating[32], ratio[48];
    std::snprintf(rating, sizeof(rating), "%.1f", rank.score);
    if (stats.deaths > 0)
        std::snprintf(ratio, sizeof(ratio), "%.2f",
            (static_cast<double>(stats.kills) + stats.assists) / stats.deaths);
    else std::snprintf(ratio, sizeof(ratio), "%s",
        stats.kills > 0 || stats.assists > 0 ? "inf" : "0");
    const std::array<const char *, 5> labels {{"Rating", "Kills", "Assists", "Deaths", "K/A/D ratio"}};
    const std::array<std::string, 5> values {{rating, std::to_string(stats.kills),
        std::to_string(stats.assists), std::to_string(stats.deaths), ratio}};
    std::string html = "<font color='#ffb900'>Rank </font><font color='#ffffff'>" +
        std::to_string(rank.position) + "</font><font color='#ffb900'> of </font>"
        "<font color='#ffffff'>" + std::to_string(rank.players) + "</font>";
    for (size_t row = 0; row < labels.size(); ++row) {
        html += "<br>";
        html += std::string("<font color='#ffb900'>") + labels[row] +
            " </font><font color='#ffffff'>" + values[row] + "</font>";
    }
    return html;
}

class Rankings {
public:
    void update(const Profile &profile) {
        if (profile.steam_id != 0) stats_[profile.steam_id] = profile.stats;
    }

    Rank rank(uint64_t steam_id, Role role) const {
        Rank result;
        if (role == Role::spectator) return result;
        const int index = role == Role::invisant ? 0 : 1;
        const auto found = stats_.find(steam_id);
        if (found != stats_.end()) {
            const auto &stats = found->second[index];
            if (stats.kills > 0 || stats.assists > 0 || stats.deaths > 0) {
                result.position = 1;
                result.score = rank_score(stats);
            }
        }
        for (const auto &player : stats_) {
            const auto &stats = player.second[index];
            if (stats.kills == 0 && stats.assists == 0 && stats.deaths == 0) continue;
            ++result.players;
            // Equal scores share a place (1, 1, 3); never break ties by Steam ID.
            if (result.position > 0 && rank_score(stats) > result.score) ++result.position;
        }
        return result;
    }

private:
    std::unordered_map<uint64_t, std::array<CombatStats, 2>> stats_;
};

} // namespace shroudfire
