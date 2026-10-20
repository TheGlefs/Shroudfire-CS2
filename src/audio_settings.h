#pragma once

#include <array>
#include <cstdint>

namespace shroudfire {

enum class AudioCategory : uint8_t {
    round_start, round_end, kill_streak, starvation, afk, timeout, camping, menu, count
};

constexpr std::array<const char *, static_cast<int>(AudioCategory::count)> kAudioNames {{
    "Round Start", "Round End", "Kill Streak", "Starvation", "AFK", "Timeout", "Camping", "Menu"
}};
constexpr uint32_t kAudioMask = (uint32_t{1} << static_cast<int>(AudioCategory::count)) - 1;

} // namespace shroudfire
