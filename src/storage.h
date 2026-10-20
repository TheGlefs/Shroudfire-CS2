#pragma once

#include "mode.h"

#include <filesystem>
#include <optional>

namespace shroudfire {

// One record per Steam ID keeps saves independent and avoids a process-wide rewrite.
class ProfileStore {
public:
    explicit ProfileStore(std::filesystem::path directory) : directory_(std::move(directory)) {}
    std::optional<Profile> load(uint64_t steam_id) const;
    std::vector<Profile> load_all() const;
    bool save(const Profile &profile) const;

private:
    std::filesystem::path directory_;
};

} // namespace shroudfire
