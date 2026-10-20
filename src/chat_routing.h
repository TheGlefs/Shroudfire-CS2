#pragma once

#include <cstdint>
#include <vector>

// CS2 can post multiple SayText2 packets for one Host_Say call. Keep delivery
// scoped to that call so each allowed player sees it once, while a subsequent
// message with identical text still gets delivered normally.
class ChatRouting {
public:
    void begin(int sender, bool team_only) {
        broadcasts_.push_back({sender, team_only, 0});
    }

    void end() {
        if (!broadcasts_.empty()) broadcasts_.pop_back();
    }

    uint64_t recipients(int sender, bool packet_team_only,
                        uint64_t all_recipients, uint64_t team_recipients) {
        if (broadcasts_.empty() || broadcasts_.back().sender != sender)
            return packet_team_only ? team_recipients : all_recipients;
        auto &broadcast = broadcasts_.back();
        // The originating say/say_team command is authoritative; native packet
        // formatting must not broaden a team-only message to opposing players.
        const uint64_t allowed = broadcast.team_only ? team_recipients : all_recipients;
        const uint64_t pending = allowed & ~broadcast.delivered;
        broadcast.delivered |= pending;
        return pending;
    }

private:
    struct Broadcast {
        int sender;
        bool team_only;
        uint64_t delivered;
    };
    std::vector<Broadcast> broadcasts_;
};
