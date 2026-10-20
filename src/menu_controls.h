#pragma once

#include <cstdint>

namespace shroudfire {

constexpr const char *kMenuNavigateSound = "sounds/ui/panorama/itemtile_rollover_09.vsnd";
constexpr const char *kMenuShortcutSound = "sounds/ui/book_page_fwd_01.vsnd";
constexpr const char *kMenuOptionSound = "sounds/ui/menu_accept.vsnd";
constexpr const char *kMenuExitSound = "sounds/ui/menu_back.vsnd";

struct PanelMenuInput {
    void *services = nullptr;
    uint64_t buttons = 0;
    uint64_t pressed = 0;
    bool select_pending = false;

    void capture(void *source, uint64_t held, uint64_t allowed) {
        if (services == source) pressed |= held & ~buttons & allowed;
        services = source;
        buttons = held;
    }

    void capture_press(uint64_t button, uint64_t allowed) {
        pressed |= button & allowed;
    }

    uint64_t take_pressed() {
        const uint64_t result = pressed;
        pressed = 0;
        return result;
    }
};

} // namespace shroudfire
