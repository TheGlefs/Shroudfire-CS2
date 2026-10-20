#pragma once

#include "audio_settings.h"

#include <algorithm>
#include <string>

namespace shroudfire {

constexpr std::array<const char *, 6> kAudioMenuOptions {{
    "Help", "Anti Cheat", "Known Bugs", "Upgrade Info", "Upgrade Menu", "Exit Menu"
}};

// This order is independent of AudioCategory because its values are persisted as bits.
constexpr std::array<AudioCategory, 8> kAudioMenuCategories {{
    AudioCategory::round_start, AudioCategory::round_end, AudioCategory::kill_streak,
    AudioCategory::starvation, AudioCategory::timeout, AudioCategory::camping,
    AudioCategory::afk, AudioCategory::menu
}};
constexpr int kAudioRowsPerPage = 4;
constexpr int kAudioSettingsPages =
    (static_cast<int>(kAudioMenuCategories.size()) + kAudioRowsPerPage - 1) / kAudioRowsPerPage;
constexpr int kAudioMenuPages = kAudioSettingsPages + 1;
constexpr const char *kAudioToggleOnSound = "sounds/buttons/blip1.vsnd";
constexpr const char *kAudioToggleOffSound = "sounds/buttons/blip2.vsnd";

// Keep settings and menu links on separate pages within the eight-line HUD.
struct AudioMenu {
    int page = 0;
    int selection = 0;

    int row_count() const {
        return page < kAudioSettingsPages
            ? (std::min)(kAudioRowsPerPage, static_cast<int>(kAudioMenuCategories.size()) - page * kAudioRowsPerPage)
            : static_cast<int>(kAudioMenuOptions.size());
    }

    void move(int direction, bool change_page = false) {
        if (change_page) {
            page = (page + (direction < 0 ? -1 : 1) + kAudioMenuPages) % kAudioMenuPages;
            selection = (std::min)(selection, row_count() - 1);
        } else if (selection + direction < 0) {
            page = (page + kAudioMenuPages - 1) % kAudioMenuPages;
            selection = row_count() - 1;
        } else if (selection + direction >= row_count()) {
            page = (page + 1) % kAudioMenuPages;
            selection = 0;
        } else selection += direction;
    }

    std::string html(uint32_t muted) const {
        std::string result = "<b><font color='#b7d1db'>AUDIO SETTINGS</font></b> "
            "<font color='#ffffff'>" + std::to_string(page + 1) + "/" +
            std::to_string(kAudioMenuPages) + "</font><br>";
        for (int row = 0; row < row_count(); ++row) {
            if (row == selection) result += "<font color='#ffd36b'>&gt;[</font>";
            if (page < kAudioSettingsPages) {
                const int category = static_cast<int>(kAudioMenuCategories[page * kAudioRowsPerPage + row]);
                const bool enabled = (muted & (uint32_t{1} << category)) == 0;
                result += enabled ? "<font color='#93c47d'>" : "<font color='#ffae61'>";
                result += kAudioNames[category];
                result += enabled ? " ON</font>" : " OFF</font>";
            } else {
                result += "<font color='#c69bff'>";
                result += kAudioMenuOptions[row];
                result += "</font>";
            }
            if (row == selection) result += "<font color='#ffd36b'>]&lt;</font>";
            if (row + 1 < row_count()) result += "<br>";
        }
        return result;
    }
};

} // namespace shroudfire
