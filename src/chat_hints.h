#pragma once

#include <string_view>

namespace shroudfire {

struct ChatHintMatches {
    bool audio = false;
    bool cheat = false;
    bool upgrade = false;
    bool bugs = false;
    bool menu = false;
    bool reset = false;
    bool rank = false;
};

// Input is normalized to lowercase by the chat command handler before this is called.
inline ChatHintMatches match_chat_hints(std::string_view input) {
    if (input == "audio" || input == "bugs" || input == "rank" || input == "menu" ||
        input == "upgrade" || input == "cheat" || input == "reset")
        return {};

    const auto contains = [input](std::string_view keyword) {
        return input.find(keyword) != std::string_view::npos;
    };
    return {
        contains("audio") || contains("voice") || contains("sound") || contains("announce"),
        contains("cheat") || contains("hack"),
        contains("upgrade"),
        contains("reload") || contains("animation") || contains("footstep") || contains("dexterity"),
        contains("level") || contains("point") || contains("menu") || contains("xp"),
        contains("reset"),
        contains("rank")
    };
}

} // namespace shroudfire
