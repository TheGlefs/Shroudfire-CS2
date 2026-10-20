#include "storage.h"

#include <charconv>
#include <fstream>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif

namespace shroudfire {

std::optional<Profile> ProfileStore::load(uint64_t steam_id) const {
    if (steam_id == 0) return std::nullopt;
    std::ifstream file(directory_ / (std::to_string(steam_id) + ".txt"), std::ios::binary);
    if (!file) return std::nullopt;
    int version = 0;
    Profile p;
    if (!(file >> version >> p.steam_id >> p.queue_stamp >> p.last_seen) ||
        (version < 1 || version > 4) || p.steam_id != steam_id || p.last_seen < 0)
        return std::nullopt;
    for (int &xp : p.xp) if (!(file >> xp) || xp < 0) return std::nullopt;
    for (int &points : p.points) if (!(file >> points) || points < 0) return std::nullopt;
    for (int &stage : p.upgrades) if (!(file >> stage) || stage < 0 || stage > 5) return std::nullopt;
    if (version >= 2) {
        for (auto &stats : p.stats) {
            if (!(file >> stats.kills >> stats.assists >> stats.deaths) ||
                stats.kills < 0 || stats.assists < 0 || stats.deaths < 0) return std::nullopt;
        }
    }
    if (version >= 3) {
        int muted = 0;
        const int valid_mask = version == 3 ? 31 : static_cast<int>(kAudioMask);
        if (!(file >> muted) || muted < 0 || muted > valid_mask) return std::nullopt;
        p.muted_audio = static_cast<uint32_t>(muted);
    }
    return p;
}

std::vector<Profile> ProfileStore::load_all() const {
    std::vector<Profile> profiles;
    std::error_code error;
    std::filesystem::directory_iterator file(directory_, error), end;
    for (; !error && file != end; file.increment(error)) {
        if (file->path().extension() != ".txt" || !file->is_regular_file(error)) continue;
        const std::string name = file->path().stem().string();
        uint64_t steam_id = 0;
        const auto parsed = std::from_chars(name.data(), name.data() + name.size(), steam_id);
        if (parsed.ec != std::errc{} || parsed.ptr != name.data() + name.size()) continue;
        if (auto profile = load(steam_id)) profiles.push_back(*profile);
    }
    return profiles;
}

bool ProfileStore::save(const Profile &p) const {
    if (p.steam_id == 0) return false;
    std::error_code error;
    std::filesystem::create_directories(directory_, error);
    if (error) return false;
    const auto final = directory_ / (std::to_string(p.steam_id) + ".txt");
    const auto pending = directory_ / (std::to_string(p.steam_id) + ".pending");
    {
        std::ofstream file(pending, std::ios::binary | std::ios::trunc);
        if (!file) return false;
        file << "4 " << p.steam_id << ' ' << p.queue_stamp << ' ' << p.last_seen;
        for (int xp : p.xp) file << ' ' << xp;
        for (int points : p.points) file << ' ' << points;
        for (int stage : p.upgrades) file << ' ' << stage;
        for (const auto &stats : p.stats)
            file << ' ' << stats.kills << ' ' << stats.assists << ' ' << stats.deaths;
        file << ' ' << p.muted_audio << '\n';
        file.flush();
        if (!file) return false;
    }
#ifdef _WIN32
    return MoveFileExW(pending.c_str(), final.c_str(),
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    std::filesystem::rename(pending, final, error);
    return !error;
#endif
}

} // namespace shroudfire
