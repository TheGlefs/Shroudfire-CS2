#pragma once

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace shroudfire {

struct WeaponRestrictions {
    std::string setting;
    std::vector<uint16_t> definitions;
    std::vector<std::string> unknown;

    bool configure(const std::string &value) {
        if (value == setting) return false;
        setting = value;
        definitions.clear();
        unknown.clear();
        // Canonical CS2 weapon names and common names for loadout variants.
        static constexpr struct { std::string_view name; uint16_t definition; } weapons[] {
            {"deagle", 1}, {"elite", 2}, {"fiveseven", 3}, {"glock", 4},
            {"ak47", 7}, {"aug", 8}, {"awp", 9}, {"famas", 10},
            {"g3sg1", 11}, {"galilar", 13}, {"m249", 14}, {"m4a1", 16},
            {"mac10", 17}, {"p90", 19}, {"mp5sd", 23}, {"ump45", 24},
            {"xm1014", 25}, {"bizon", 26}, {"mag7", 27}, {"negev", 28},
            {"sawedoff", 29}, {"tec9", 30}, {"taser", 31}, {"hkp2000", 32},
            {"mp7", 33}, {"mp9", 34}, {"nova", 35}, {"p250", 36},
            {"scar20", 38}, {"sg556", 39}, {"ssg08", 40},
            {"flashbang", 43}, {"hegrenade", 44}, {"smokegrenade", 45},
            {"molotov", 46}, {"decoy", 47}, {"incgrenade", 48},
            {"m4a1_silencer", 60}, {"usp_silencer", 61}, {"cz75a", 63}, {"revolver", 64},
            {"m4a4", 16}, {"m4a1-s", 60}, {"usp", 61}, {"usp-s", 61},
            {"sg553", 39}, {"p2000", 32}, {"galil", 13}, {"mp5", 23},
            {"cz75-auto", 63}, {"r8", 64}, {"he", 44}, {"smoke", 45}
        };
        std::string list = value;
        std::replace(list.begin(), list.end(), ',', ' ');
        std::istringstream tokens(list);
        std::string token;
        while (tokens >> token) {
            std::transform(token.begin(), token.end(), token.begin(),
                [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            if (token.rfind("weapon_", 0) == 0) token.erase(0, 7);
            const auto weapon = std::find_if(std::begin(weapons), std::end(weapons),
                [&token](const auto &entry) { return entry.name == token; });
            if (weapon == std::end(weapons)) unknown.push_back(token);
            else if (std::find(definitions.begin(), definitions.end(), weapon->definition) == definitions.end())
                definitions.push_back(weapon->definition);
        }
        return true;
    }
};

} // namespace shroudfire
