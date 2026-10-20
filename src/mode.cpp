#include "mode.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace shroudfire {

Mode::Mode(Settings settings) : settings_(std::move(settings)) {}

std::vector<Action> Mode::take_actions() {
    std::vector<Action> result;
    result.swap(actions_);
    return result;
}

bool Mode::valid(int slot) const {
    return slot >= 1 && slot <= kMaxPlayers;
}

bool Mode::active_player(int slot) const {
    return valid(slot) && players_[slot].connected &&
           players_[slot].role != Role::spectator;
}

bool Mode::active_human(int slot) const {
    return active_player(slot) && !players_[slot].bot;
}

bool Mode::active() const {
    return settings_.enabled;
}

int Mode::active_humans() const {
    int count = 0;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) count += active_human(slot);
    return count;
}

int Mode::active_players() const {
    int count = 0;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) count += active_player(slot);
    return count;
}

int Mode::participants() const {
    int count = 0;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        count += active_player(slot) && players_[slot].round_participant &&
                 !players_[slot].waiting_next_round;
    }
    return count;
}

int Mode::alive(Role role) const {
    int count = 0;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        count += active_player(slot) && players_[slot].role == role && players_[slot].alive &&
                 !players_[slot].waiting_next_round;
    }
    return count;
}

uint64_t Mode::chat_recipients(int sender, bool team_only) const {
    if (!active() || !valid(sender) || !players_[sender].connected) return 0;
    uint64_t recipients = 0;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        const Player &recipient = players_[slot];
        if (recipient.connected && !recipient.bot &&
            (!team_only || recipient.role == players_[sender].role))
            recipients |= uint64_t{1} << (slot - 1);
    }
    return recipients;
}

bool Mode::can_spectate(int viewer, int target, bool invisant_teammates,
                        bool admin_observation) const {
    if (!active() || !valid(viewer) || !valid(target) || viewer == target) return false;
    const Player &observer = players_[viewer];
    const Player &other = players_[target];
    if (!observer.connected || observer.bot || observer.alive ||
        !other.connected || !other.alive || other.waiting_next_round) return false;
    if (admin_observation && observer.role == Role::spectator)
        return other.role != Role::spectator;
    return observer.role != Role::spectator && observer.role == other.role &&
        (observer.role != Role::invisant || invisant_teammates);
}

int Mode::desired_invisant_count() const {
    const int players = active_players();
    if (players < 2) return 0;
    const int ratio = std::max(2, settings_.armitive_per_invisant);
    return std::min((players + ratio) / (ratio + 1), players == 2 ? 1 : (players - 1) / 2);
}

int Mode::required_xp(int value) const {
    const int capped = std::clamp(value, 0, kMaxLevel);
    return std::max(1, settings_.xp_level_base) * capped * (capped + 1) / 2;
}

int Mode::level(int xp) const {
    int low = 0, high = kMaxLevel;
    while (low < high) {
        const int middle = (low + high + 1) / 2;
        if (xp >= required_xp(middle)) low = middle;
        else high = middle - 1;
    }
    return low;
}

void Mode::map_start(float now) {
    timeout_reveal_active_ = false;
    map_started_at_ = now;
    round_live_ = false;
    freeze_ended_ = false;
    roster_initialized_ = false;
    advance_next_round_ = false;
    round_result_emitted_ = false;
    actions_.clear();
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!p.connected) continue;
        p.alive = false;
        p.round_participant = false;
        p.waiting_next_round = false;
        p.spectator_locked = false;
        p.earned_invisant = false;
        p.forfeited_invisant = false;
        p.round_kills = 0;
        p.last_activity = now;
        p.camping_position_ready = false;
        p.camping_warned = false;
        p.camping_active = false;
        if (p.role == Role::invisant) assign(slot, Role::armitive);
    }
}

void Mode::queue_back(int slot) {
    players_[slot].queue_stamp = next_queue_stamp_++;
}

void Mode::assign(int slot, Role role) {
    Player &p = players_[slot];
    if (p.role == role) return;
    p.role = role;
    p.shield_until = 0.0f;
    p.shield_available_at = 0.0f;
    p.leap_available_at = 0.0f;
    p.streak_count = 0;
    p.last_kill_at = 0.0f;
    p.last_armitive_attacker = 0;
    p.camping_position_ready = false;
    p.camping_warned = false;
    p.camping_active = false;
    actions_.push_back({Effect::change_team, slot, 0, role, {}});
}

void Mode::connect(int slot, uint64_t steam_id, bool bot, const Profile *saved, int unix_time, float now) {
    if (!valid(slot)) return;
    Player p;
    p.connected = true;
    p.bot = bot;
    p.steam_id = steam_id;
    p.connected_at = now;
    p.last_activity = now;
    p.role = Role::armitive;
    if (saved && saved->steam_id == steam_id && steam_id != 0) {
        p.xp = saved->xp;
        p.points = saved->points;
        p.upgrades = saved->upgrades;
        p.stats = saved->stats;
        p.muted_audio = saved->muted_audio & kAudioMask;
        for (int role = 0; role < 2; ++role) {
            p.xp[role] = std::clamp(p.xp[role], 0, required_xp(kMaxLevel));
            int spent = 0;
            for (int upgrade = role * 8; upgrade < role * 8 + 8; ++upgrade) {
                p.upgrades[upgrade] = std::clamp(p.upgrades[upgrade], 0, kMaxUpgradeStage);
                spent += p.upgrades[upgrade];
            }
            const int capacity = kMaxLevel - spent;
            p.points[role] = std::clamp(std::max(p.points[role], level(p.xp[role]) - spent),
                                        0, capacity);
        }
        const bool within_grace = settings_.reconnect_grace == 0 ||
            (unix_time >= saved->last_seen && unix_time - saved->last_seen <= settings_.reconnect_grace);
        if (within_grace && saved->queue_stamp != 0) p.queue_stamp = saved->queue_stamp;
        next_queue_stamp_ = std::max(next_queue_stamp_, saved->queue_stamp + 1);
    }
    p.applied_upgrades = p.upgrades;
    players_[slot] = p;
    // CS2 removes bots while no humans are connected, so a test bot can only
    // join after the first human. Give it the queue place it would have held.
    if (players_[slot].queue_stamp == 0) {
        if (bot) players_[slot].queue_stamp = front_queue_stamp_--;
        else queue_back(slot);
    }
    actions_.push_back({Effect::change_team, slot, 0, Role::armitive, {}});
    if (round_live_) {
        const bool in_grace = now - map_started_at_ <= settings_.map_join_grace;
        players_[slot].waiting_next_round = !in_grace;
        if (in_grace)
            actions_.push_back({Effect::respawn, slot, 0, Role::armitive, {}});
    }
    if (active_players() == 2 && roster_initialized_ && desired_invisant_count() == 1 &&
        alive(Role::invisant) == 0) {
        roster_initialized_ = false;
        actions_.push_back({Effect::restart_round, 0, 0, Role::spectator, {}});
    }
}

Profile Mode::profile(int slot, int unix_time) const {
    Profile result;
    if (!valid(slot)) return result;
    const Player &p = players_[slot];
    result.steam_id = p.steam_id;
    result.queue_stamp = p.queue_stamp;
    result.last_seen = unix_time;
    result.xp = p.xp;
    result.points = p.points;
    result.upgrades = p.upgrades;
    result.stats = p.stats;
    result.muted_audio = p.muted_audio;
    return result;
}

bool Mode::toggle_audio(int slot, AudioCategory category) {
    if (!valid(slot) || !players_[slot].connected || players_[slot].bot ||
        category >= AudioCategory::count) return false;
    players_[slot].muted_audio ^= uint32_t{1} << static_cast<int>(category);
    return true;
}

bool Mode::audio_enabled(int slot, AudioCategory category) const {
    return valid(slot) && players_[slot].connected && !players_[slot].bot &&
        category < AudioCategory::count &&
        (players_[slot].muted_audio & (uint32_t{1} << static_cast<int>(category))) == 0;
}

Profile Mode::disconnect(int slot, int unix_time) {
    Profile result = profile(slot, unix_time);
    if (!valid(slot)) return result;
    Player &p = players_[slot];
    p = {};
    for (auto &row : assist_damage_) row[slot] = 0;
    assist_damage_[slot].fill(0);
    if (active_players() == 1) {
        for (int other = 1; other <= kMaxPlayers; ++other) {
            if (active_player(other) && players_[other].role == Role::invisant)
                assign(other, Role::armitive);
        }
        roster_initialized_ = false;
        advance_next_round_ = false;
        if (round_live_) actions_.push_back({Effect::restart_round, 0, 0, Role::spectator, {}});
    }
    check_elimination();
    return result;
}

void Mode::join_spectator(int slot, bool preserve_lone_invisant) {
    if (!active_human(slot)) return;
    Player &p = players_[slot];
    p.earned_invisant = false;
    p.forfeited_invisant = true;
    p.spectator_locked = p.role == Role::invisant;
    p.round_participant = false;
    p.alive = false;
    assign(slot, Role::spectator);
    if (active_players() == 1 && !preserve_lone_invisant) {
        for (int other = 1; other <= kMaxPlayers; ++other) {
            if (active_player(other) && players_[other].role == Role::invisant)
                assign(other, Role::armitive);
        }
        roster_initialized_ = false;
        advance_next_round_ = false;
        if (round_live_) actions_.push_back({Effect::restart_round, 0, 0, Role::spectator, {}});
    }
    check_elimination();
}

bool Mode::request_team(int slot, Role requested, float now) {
    if (!valid(slot) || !players_[slot].connected || players_[slot].bot) return false;
    Player &p = players_[slot];
    if (!active()) return true;
    if (requested == Role::invisant) return false;
    if (requested == Role::spectator) {
        join_spectator(slot);
        return true;
    }
    if (p.role == Role::invisant || p.spectator_locked) return false;
    if (p.role == Role::armitive) return false;
    assign(slot, Role::armitive);
    p.waiting_next_round = round_live_ && now - map_started_at_ > settings_.map_join_grace;
    if (round_live_ && !p.waiting_next_round)
        actions_.push_back({Effect::respawn, slot, 0, Role::armitive, {}});
    return true;
}

void Mode::activity(int slot, float now, bool moved) {
    if (!active_human(slot)) return;
    Player &p = players_[slot];
    p.last_activity = now;
    p.afk_warned = false;
    p.last_afk_warning_second = -1;
    if (p.alive && p.afk_queue_removed && p.role == Role::armitive) {
        p.afk_queue_removed = false;
        queue_back(slot);
    }
    if (round_live_ && freeze_ended_ && p.alive && moved) {
        p.moved_this_round = true;
        p.consecutive_afk_deaths = 0;
    }
}

void Mode::movement_sample(int slot, float x, float y, float z, float now) {
    if (!active_human(slot)) return;
    Player &p = players_[slot];
    if (!active() || !round_live_ || !freeze_ended_ || !p.alive || p.waiting_next_round ||
        p.role != Role::armitive || settings_.camping_distance <= 0.0f) {
        p.camping_position_ready = false;
        p.camping_warned = false;
        p.camping_active = false;
        return;
    }
    if (!p.camping_position_ready || participants() < 2) {
        p.camping_position_ready = true;
        p.camping_warned = false;
        p.camping_active = false;
        p.camping_anchor_x = x;
        p.camping_anchor_y = y;
        p.camping_anchor_z = z;
        p.camping_anchor_at = now;
        p.last_camping_tick_at = now;
        return;
    }
    const float dx = x - p.camping_anchor_x;
    const float dy = y - p.camping_anchor_y;
    const float dz = z - p.camping_anchor_z;
    const float required = settings_.camping_distance;
    if (dx * dx + dy * dy + dz * dz >= required * required) {
        p.camping_warned = false;
        p.camping_active = false;
        p.camping_anchor_x = x;
        p.camping_anchor_y = y;
        p.camping_anchor_z = z;
        p.camping_anchor_at = now;
        p.last_camping_tick_at = now;
    }
}

bool Mode::forfeit_invisant(int slot) {
    if (!active() || !round_live_ || !active_human(slot)) return false;
    Player &p = players_[slot];
    if (!p.alive || p.role != Role::armitive || p.forfeited_invisant) return false;
    p.forfeited_invisant = true;
    p.earned_invisant = false;
    return true;
}

void Mode::spawn(int slot, float now) {
    if (!active_player(slot)) return;
    Player &p = players_[slot];
    p.alive = true;
    p.last_activity = now;
    p.last_invisant_damage_at = now;
    p.last_starvation_tick_at = now;
    p.starvation_active = false;
    p.camping_position_ready = false;
    p.camping_warned = false;
    p.camping_active = false;
    p.camping_anchor_at = now;
    p.last_camping_tick_at = now;
    p.last_damage_taken_at = now;
    p.last_weapon_fire_at = 0.0f;
    p.last_reforge_at = now;
    p.last_recovery_at = now;
    p.leap_available_at = 0.0f;
    p.shield_until = 0.0f;
    p.shield_available_at = 0.0f;
    p.leap_ready_announced = true;
    p.shield_ready_announced = true;
    p.afk_death_pending = false;
    p.afk_warned = false;
    p.last_afk_warning_second = -1;
    p.streak_count = 0;
    p.last_kill_at = 0.0f;
    p.last_armitive_attacker = 0;
    p.round_participant = !p.waiting_next_round;
    p.round_death_cause = DeathCause::forced;
    assist_damage_[slot].fill(0);
    for (auto &row : assist_damage_) row[slot] = 0;
}

bool Mode::afk(int slot, float now) const {
    return !players_[slot].bot && settings_.afk_seconds > 0 &&
           now - players_[slot].last_activity >= settings_.afk_seconds;
}

int Mode::oldest_candidate(const std::array<bool, kMaxPlayers + 1> &selected,
                           const std::array<bool, kMaxPlayers + 1> &previous,
                           bool rewards_only, bool allow_afk, float now) {
    int best = 0;
    int oldest = std::numeric_limits<int>::max();
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (selected[slot] || previous[slot] || !active_player(slot) || p.role != Role::armitive ||
            p.forfeited_invisant || (rewards_only && !p.earned_invisant) ||
            (!allow_afk && (p.afk_queue_removed || afk(slot, now)))) continue;
        if (p.queue_stamp == 0) queue_back(slot);
        if (p.queue_stamp < oldest) {
            best = slot;
            oldest = p.queue_stamp;
        }
    }
    return best;
}

void Mode::start_round(float now) {
    timeout_reveal_active_ = false;
    if (!active()) return;
    round_started_at_ = now;
    active_started_at_ = 0.0f;
    round_live_ = true;
    freeze_ended_ = false;
    round_result_emitted_ = false;
    first_blood_claimed_ = false;
    round_participants_ = 0;
    round_invisant_participants_ = 0;
    round_armitive_participants_ = 0;
    for (auto &row : assist_damage_) row.fill(0);
    std::array<bool, kMaxPlayers + 1> selected {};
    std::array<bool, kMaxPlayers + 1> previous {};
    std::array<Role, kMaxPlayers + 1> role_before {};
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        role_before[slot] = p.role;
        if (p.connected) p.spectator_locked = false;
        if (!active_player(slot)) continue;
        previous[slot] = p.role == Role::invisant;
        if (p.role == Role::armitive && p.queue_stamp == 0 && !p.afk_queue_removed) queue_back(slot);
    }
    if (!roster_initialized_ || advance_next_round_) {
        const int desired = desired_invisant_count();
        if (!roster_initialized_) {
            for (int slot = 1; slot <= kMaxPlayers; ++slot) {
                if (active_player(slot)) {
                    assign(slot, Role::armitive);
                    previous[slot] = false;
                    players_[slot].forfeited_invisant = false;
                }
            }
        }
        int count = 0;
        if (roster_initialized_) {
            while (count < desired) {
                const int slot = oldest_candidate(selected, previous, true, false, now);
                if (!slot) break;
                selected[slot] = true;
                ++count;
            }
            // Rewarded players displaced by a shrinking roster retain front-of-queue order.
            std::vector<int> displaced;
            for (int slot = 1; slot <= kMaxPlayers; ++slot) {
                const Player &p = players_[slot];
                if (active_player(slot) && p.role == Role::armitive && p.earned_invisant &&
                    !p.forfeited_invisant && !selected[slot] && !afk(slot, now)) displaced.push_back(slot);
            }
            std::sort(displaced.begin(), displaced.end(), [&](int a, int b) {
                return players_[a].queue_stamp < players_[b].queue_stamp;
            });
            for (auto it = displaced.rbegin(); it != displaced.rend(); ++it)
                players_[*it].queue_stamp = front_queue_stamp_--;
        }
        while (count < desired) {
            const int slot = oldest_candidate(selected, previous, false, false, now);
            if (!slot) break;
            selected[slot] = true;
            ++count;
        }
        if (!roster_initialized_) {
            while (count < desired) {
                const int slot = oldest_candidate(selected, previous, false, true, now);
                if (!slot) break;
                selected[slot] = true;
                ++count;
            }
        }
        if (roster_initialized_) {
            while (count < desired) {
                int best = 0;
                for (int slot = 1; slot <= kMaxPlayers; ++slot) {
                    const Player &p = players_[slot];
                    if (!previous[slot] || selected[slot] || !active_player(slot) || p.afk_queue_removed) continue;
                    if (!best || p.round_kills > players_[best].round_kills ||
                        (p.round_kills == players_[best].round_kills && p.rounds_invisant < players_[best].rounds_invisant)) best = slot;
                }
                if (!best) break;
                selected[best] = true;
                ++count;
            }
            while (count < desired) {
                const int slot = oldest_candidate(selected, previous, false, true, now);
                if (!slot) break;
                selected[slot] = true;
                ++count;
            }
        }
        for (int slot = 1; slot <= kMaxPlayers; ++slot) {
            if (!active_player(slot)) continue;
            Player &p = players_[slot];
            if (selected[slot]) {
                if (p.role != Role::invisant) {
                    p.promotion_stamp = p.queue_stamp;
                    p.queue_stamp = 0;
                    assign(slot, Role::invisant);
                }
            } else if (p.role == Role::invisant) {
                assign(slot, Role::armitive);
                queue_back(slot);
            }
        }
    }
    roster_initialized_ = true;
    advance_next_round_ = false;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!active_player(slot)) continue;
        p.earned_invisant = false;
        p.forfeited_invisant = false;
        p.round_kills = 0;
        p.streak_count = 0;
        p.last_kill_at = 0.0f;
        p.last_armitive_attacker = 0;
        p.waiting_next_round = false;
        p.applied_upgrades = p.upgrades;
        p.upgrade_pending.fill(false);
        p.spectator_locked = false;
        p.round_participant = true;
        p.round_death_cause = DeathCause::forced;
        p.moved_this_round = false;
        p.last_activity = now;
        p.last_invisant_damage_at = now;
        p.starvation_active = false;
        p.camping_position_ready = false;
        p.camping_warned = false;
        p.camping_active = false;
        if (p.role == Role::invisant) {
            ++round_invisant_participants_;
            ++p.rounds_invisant;
        } else ++round_armitive_participants_;
        ++round_participants_;
        actions_.push_back({Effect::respawn, slot,
                            p.role != role_before[slot] ? 3 : 0, p.role, {}});
    }
}

void Mode::freeze_end(float now) {
    if (!round_live_ || freeze_ended_) return;
    freeze_ended_ = true;
    active_started_at_ = now;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!active_player(slot) || !p.alive || p.waiting_next_round) continue;
        p.last_activity = now;
        p.afk_warned = false;
        p.last_afk_warning_second = -1;
        p.camping_position_ready = false;
        p.camping_warned = false;
        p.camping_active = false;
        p.camping_anchor_at = now;
        p.last_camping_tick_at = now;
        if (p.role == Role::invisant) {
            p.last_invisant_damage_at = now;
            p.last_starvation_tick_at = now;
            p.starvation_active = false;
        }
    }
}

int Mode::award_xp(int slot, Role role, int amount) {
    if (!active_player(slot) || amount <= 0 || role == Role::spectator) return 0;
    Player &p = players_[slot];
    const int index = role == Role::invisant ? 0 : 1;
    const int before = level(p.xp[index]);
    const int previous_xp = p.xp[index];
    const int maximum = required_xp(kMaxLevel);
    p.xp[index] = std::min(maximum, p.xp[index] + std::min(amount, maximum - p.xp[index]));
    p.points[index] += level(p.xp[index]) - before;
    actions_.push_back({Effect::xp_changed, slot, p.xp[index], role, {},
                        0, WeaponKind::other, before});
    return p.xp[index] - previous_xp;
}

void Mode::end_round(Role winner, bool completed, bool timed_out, float now) {
    if (!round_live_) return;
    round_live_ = false;
    advance_next_round_ = completed && round_participants_ >= 2;
    if (advance_next_round_)
        actions_.push_back({Effect::round_completed, 0, 0, winner, {}});
    if (advance_next_round_ && winner != Role::spectator) {
        const int amount = winner == Role::invisant ? settings_.xp_round_win_invisant : settings_.xp_round_win_armitive;
        for (int slot = 1; slot <= kMaxPlayers; ++slot) {
            if (active_player(slot) && players_[slot].role == winner && players_[slot].round_participant &&
                !players_[slot].waiting_next_round) {
                const int gained = award_xp(slot, winner, amount);
                actions_.push_back({Effect::round_win_reward, slot, gained, winner, {}});
            }
        }
    } else if (!advance_next_round_) {
        for (int slot = 1; slot <= kMaxPlayers; ++slot) players_[slot].earned_invisant = false;
    }
    if (completed && timed_out && winner == Role::armitive) {
        if (!settings_.timeout_kill_invisant) {
            timeout_reveal_active_ = true;
            timeout_reveal_started_at_ = now;
            timeout_reveal_duration_ = std::clamp(settings_.timeout_reveal_seconds, 0.0f, 10.0f);
            return;
        }
        // Record the result before physical deaths can re-enter elimination handling.
        for (int slot = 1; slot <= kMaxPlayers; ++slot) {
            const Player &player = players_[slot];
            if (active_player(slot) && player.alive && player.role == Role::invisant)
                actions_.push_back({Effect::kill_timeout, slot, 0, Role::invisant, {}});
        }
    }
}

void Mode::begin_round_end() {
    if (round_live_) round_result_emitted_ = true;
}

void Mode::damage(int attacker, int victim, int health_damage, float now) {
    if (!round_live_ || round_result_emitted_ || !active_player(attacker) ||
        !active_player(victim) || attacker == victim ||
        players_[attacker].role == players_[victim].role || health_damage <= 0 ||
        players_[attacker].waiting_next_round || players_[victim].waiting_next_round) return;
    assist_damage_[attacker][victim] += health_damage;
    if (players_[attacker].role == Role::invisant) {
        players_[attacker].last_invisant_damage_at = now;
        players_[attacker].starvation_active = false;
        const int stage = players_[attacker].applied_upgrades[static_cast<int>(Upgrade::vampirism)];
        if (stage > 0) actions_.push_back({Effect::heal, attacker,
            settings_.vampirism_health_per_stage * stage, Role::invisant, {}});
    }
    if (players_[attacker].role == Role::armitive && players_[victim].role == Role::invisant)
        players_[victim].last_armitive_attacker = attacker;
}

void Mode::damage_taken(int victim, float now, bool counts_for_starvation) {
    if (!active_player(victim)) return;
    Player &player = players_[victim];
    player.last_damage_taken_at = now;
    if (counts_for_starvation && round_live_ && player.alive &&
        player.role == Role::invisant && !player.waiting_next_round) {
        player.last_invisant_damage_at = now;
        player.starvation_active = false;
    }
}

void Mode::weapon_fire(int slot, float now) {
    if (active_player(slot)) players_[slot].last_weapon_fire_at = now;
}

void Mode::death(int victim, int attacker, DeathCause cause, WeaponKind weapon, float now) {
    if (!active_player(victim)) return;
    Player &dead = players_[victim];
    const bool scoring = round_live_ && !round_result_emitted_;
    const bool ranked_death = active() && scoring && freeze_ended_ && dead.alive &&
        dead.round_participant && !dead.waiting_next_round && !dead.bot &&
        active_humans() >= 2 && cause != DeathCause::forced && cause != DeathCause::timeout &&
        (cause != DeathCause::enemy || (active_human(attacker) && attacker != victim &&
            players_[attacker].role != dead.role && !players_[attacker].waiting_next_round));
    if (ranked_death) {
        auto &deaths = dead.stats[dead.role == Role::invisant ? 0 : 1].deaths;
        if (deaths < std::numeric_limits<int>::max()) ++deaths;
    }
    dead.alive = false;
    dead.round_death_cause = cause;
    dead.shield_until = 0.0f;
    dead.streak_count = 0;
    dead.afk_death_pending = false;
    dead.camping_position_ready = false;
    dead.camping_warned = false;
    dead.camping_active = false;
    if (!scoring) {
        // Transition kills cannot change rewards, streaks, or the next-round queue.
        if (cause == DeathCause::enemy && active_player(attacker) && attacker != victim &&
            players_[attacker].role != dead.role)
            actions_.push_back({Effect::post_round_kill, attacker, 0,
                players_[attacker].role, {}, victim, weapon});
        for (auto &row : assist_damage_) row[victim] = 0;
        return;
    }
    const bool inactive_death = !dead.bot &&
        (cause == DeathCause::afk ||
         (cause == DeathCause::enemy && !dead.moved_this_round));
    if (inactive_death) {
        dead.earned_invisant = false;
        dead.afk_queue_removed = true;
        dead.queue_stamp = 0;
        ++dead.consecutive_afk_deaths;
        if (settings_.afk_limit > 0 && dead.consecutive_afk_deaths >= settings_.afk_limit)
            actions_.push_back({Effect::kick_afk, victim, 0, dead.role, {}});
    }
    if (cause == DeathCause::suicide || cause == DeathCause::fall) {
        dead.earned_invisant = false;
        dead.forfeited_invisant = true;
    }
    bool enemy = cause == DeathCause::enemy && active_player(attacker) && attacker != victim &&
        players_[attacker].role != dead.role && !players_[attacker].waiting_next_round &&
        !dead.waiting_next_round;
    if (enemy && round_live_ && participants() >= 2) {
        Player &killer = players_[attacker];
        const Role role = killer.role;
        if (!inactive_death) {
            const bool ranked_kill = ranked_death && !killer.bot;
            if (ranked_kill) {
                auto &kills = killer.stats[role == Role::invisant ? 0 : 1].kills;
                if (kills < std::numeric_limits<int>::max()) ++kills;
            }
            const int base = role == Role::invisant ? settings_.xp_kill_invisant : settings_.xp_kill_armitive;
            const int weapon_percent = role == Role::armitive && weapon == WeaponKind::knife
                ? settings_.xp_armitive_knife_percent
                : role == Role::armitive && weapon == WeaponKind::pistol
                ? settings_.xp_armitive_pistol_percent : 100;
            int amount = std::lround(base * weapon_percent / 100.0f);
            const int previous_streak = killer.streak_count;
            const bool continued = previous_streak > 0 && now >= killer.last_kill_at &&
                now - killer.last_kill_at <= settings_.kill_streak_window;
            killer.streak_count = continued ? std::min(10, previous_streak + 1) : 1;
            killer.last_kill_at = now;
            int streak_index = -1;
            if (!first_blood_claimed_) {
                first_blood_claimed_ = true;
                if (round_invisant_participants_ >= 2 && round_armitive_participants_ >= 2) streak_index = 0;
            } else if (continued) streak_index = killer.streak_count - 1;
            if (streak_index >= 0)
                amount = std::lround(amount * settings_.streak_percent[streak_index] / 100.0f);
            const int gained = award_xp(attacker, role, amount);
            static constexpr const char *streak_names[] = {
                "First Blood", "Double Kill", "Killing Spree", "Ultra Kill", "Rampage",
                "Dominating", "Unstoppable", "Monster Kill", "Godlike", "Holy Shit"
            };
            const int streak_percent = streak_index >= 0 ? settings_.streak_percent[streak_index] : 100;
            const int combined_percent = std::lround(weapon_percent * streak_percent / 100.0f);
            if (streak_index >= 0 && (!continued || previous_streak < 10)) {
                actions_.push_back({Effect::xp_notice, attacker, 0, role,
                    std::string("[Shroudfire] ") + streak_names[streak_index] + ": " +
                    std::to_string(combined_percent) + "% kill XP rate"});
                actions_.push_back({Effect::streak_cue, attacker, streak_index, role, {}});
            }
            actions_.push_back({Effect::kill_reward, attacker, gained, role,
                streak_index >= 0 && (!continued || previous_streak < 10)
                    ? streak_names[streak_index] : "", victim, weapon});
            int total = 0;
            for (int slot = 1; slot <= kMaxPlayers; ++slot) {
                if (active_player(slot) && players_[slot].role == role && !players_[slot].waiting_next_round)
                    total += assist_damage_[slot][victim];
            }
            if (total > 0) {
                const int assist_xp = std::lround(base * settings_.assist_fraction);
                for (int slot = 1; slot <= kMaxPlayers; ++slot) {
                    const int contributed = assist_damage_[slot][victim];
                    if (slot != attacker && active_player(slot) && players_[slot].role == role &&
                        !players_[slot].waiting_next_round && contributed > 0 &&
                        contributed + 0.001f >= total * settings_.assist_damage_share) {
                        if (ranked_kill && !players_[slot].bot) {
                            auto &assists = players_[slot].stats[role == Role::invisant ? 0 : 1].assists;
                            if (assists < std::numeric_limits<int>::max()) ++assists;
                        }
                        const int gained = award_xp(slot, role, assist_xp);
                        if (gained > 0)
                            actions_.push_back({Effect::assist_reward, slot, gained, role,
                                {}, victim});
                    }
                }
            }
        }
        ++killer.round_kills;
        if (role == Role::invisant) {
            const int stage = killer.applied_upgrades[static_cast<int>(Upgrade::bloodlust)];
            if (stage > 0) actions_.push_back({Effect::heal, attacker,
                settings_.bloodlust_health_per_stage * stage, role, {}});
        }
        if (dead.role == Role::invisant && role == Role::armitive && !killer.earned_invisant) {
            killer.earned_invisant = true;
            actions_.push_back({Effect::invisant_priority, attacker, 0, role, {}});
        }
    } else if (dead.role == Role::invisant && round_live_ &&
               (cause == DeathCause::world || cause == DeathCause::suicide || cause == DeathCause::fall)) {
        const int last = dead.last_armitive_attacker;
        if (active_player(last) && players_[last].role == Role::armitive &&
            !players_[last].earned_invisant) {
            players_[last].earned_invisant = true;
        }
    }
    for (auto &row : assist_damage_) row[victim] = 0;
    check_elimination();
}

void Mode::check_elimination() {
    if (!round_live_ || round_result_emitted_ || !active()) return;
    const int participant_count = participants();
    const int invisible = alive(Role::invisant), armitive = alive(Role::armitive);
    if (participant_count == 1 && invisible == 0 && armitive == 0) {
        // Solo rounds intentionally wait indefinitely while the player lives.
        // Once they die, use the native restart path instead of leaving an
        // empty round running with no opponent to trigger its termination.
        round_result_emitted_ = true;
        advance_next_round_ = false;
        actions_.push_back({Effect::restart_round, 0, 0, Role::spectator, {}});
        return;
    }
    if (participant_count < 2) return;
    const Role eliminated = armitive == 0 ? Role::armitive : Role::invisant;
    bool all_self_or_afk = true;
    bool saw_eliminated_player = false;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        const Player &player = players_[slot];
        if (!active_player(slot) || !player.round_participant || player.role != eliminated)
            continue;
        saw_eliminated_player = true;
        if (player.round_death_cause != DeathCause::suicide &&
            player.round_death_cause != DeathCause::afk) {
            all_self_or_afk = false;
            break;
        }
    }
    all_self_or_afk = all_self_or_afk && saw_eliminated_player;
    if (invisible > 0 && armitive == 0) {
        round_result_emitted_ = true;
        actions_.push_back({Effect::end_round, 0, 0,
            all_self_or_afk ? Role::spectator : Role::invisant, {}});
    } else if (armitive > 0 && invisible == 0) {
        round_result_emitted_ = true;
        actions_.push_back({Effect::end_round, 0, 0,
            all_self_or_afk ? Role::spectator : Role::armitive, {}});
    }
}

Attributes Mode::attributes(int slot) const {
    Attributes result;
    if (!active_player(slot)) return result;
    const Player &p = players_[slot];
    const auto &u = p.applied_upgrades;
    if (p.role == Role::invisant) {
        result.maximum_armor = settings_.invisant_base_armor +
            settings_.resilience_armor_per_stage * u[static_cast<int>(Upgrade::resilience)];
        result.helmet = u[static_cast<int>(Upgrade::resilience)] >=
            settings_.resilience_helmet_stage;
        result.gravity_scale = 1.0f - settings_.lightweight_gravity_percent / 100.0f *
            u[static_cast<int>(Upgrade::lightweight)];
        result.movement_scale = 1.0f + settings_.momentum_run_percent / 100.0f *
            u[static_cast<int>(Upgrade::momentum)];
        result.crouch_speed_scale = 1.0f + settings_.mobility_crouch_percent / 100.0f *
            u[static_cast<int>(Upgrade::mobility)];
        const int leap_stage = u[static_cast<int>(Upgrade::leap)];
        result.leap_impulse = leap_stage > 0 ? settings_.surge_base_speed +
            settings_.surge_speed_per_stage * (leap_stage - 1) : 0;
    } else if (p.role == Role::armitive) {
        result.maximum_health = settings_.armitive_base_health +
            settings_.fortitude_health_per_stage * u[static_cast<int>(Upgrade::fortitude)];
        result.magazine_scale = 1.0f + settings_.overload_magazine_percent / 100.0f *
            u[static_cast<int>(Upgrade::overload)];
        result.extra_reserve_clips = settings_.stockpile_clips_per_stage *
            u[static_cast<int>(Upgrade::stockpile)];
        result.fire_delay_scale = 1.0f / (1.0f + settings_.barrage_fire_rate_percent / 100.0f *
            u[static_cast<int>(Upgrade::barrage)]);
        result.reload_delay_scale = 1.0f - settings_.dexterity_speed_percent / 100.0f *
            u[static_cast<int>(Upgrade::dexterity)];
        result.recoil_scale = 1.0f - settings_.control_reduction_percent / 100.0f *
            u[static_cast<int>(Upgrade::control)];
        result.spread_scale = result.recoil_scale;
        const int shield_stage = u[static_cast<int>(Upgrade::shield)];
        result.shield_seconds = shield_stage > 0 ? settings_.ward_base_seconds +
            settings_.ward_seconds_per_stage * (shield_stage - 1) : 0.0f;
    }
    return result;
}

bool Mode::shield_active(int slot, float now) const {
    return active_player(slot) && players_[slot].role == Role::armitive &&
           players_[slot].alive && players_[slot].shield_until > now;
}

bool Mode::expire_shields(float now) {
    if (!active() || !round_live_) return false;
    bool expired = false;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!active_player(slot) || p.role != Role::armitive || p.shield_until <= 0.0f ||
            now < p.shield_until) continue;
        p.shield_until = 0.0f;
        actions_.push_back({Effect::shield_end, slot, 0, p.role, {}});
        expired = true;
    }
    return expired;
}

bool Mode::use_ability(int slot, float now, bool path_blocked) {
    if (!active() || !round_live_ || !freeze_ended_ || !active_player(slot) ||
        !players_[slot].alive || players_[slot].waiting_next_round) return false;
    Player &p = players_[slot];
    if (p.role == Role::invisant && p.applied_upgrades[static_cast<int>(Upgrade::leap)] > 0) {
        if (now < p.leap_available_at || path_blocked) return false;
        p.leap_available_at = now + settings_.surge_cooldown;
        p.leap_ready_announced = false;
        actions_.push_back({Effect::leap, slot, attributes(slot).leap_impulse, p.role, {}});
        return true;
    }
    if (p.role == Role::armitive && p.applied_upgrades[static_cast<int>(Upgrade::shield)] > 0) {
        if (now < p.shield_available_at) return false;
        p.shield_until = now + attributes(slot).shield_seconds;
        p.shield_available_at = p.shield_until + settings_.ward_cooldown;
        p.shield_ready_announced = false;
        actions_.push_back({Effect::shield_start, slot,
            static_cast<int>(std::lround(attributes(slot).shield_seconds * 1000.0f)), p.role, {}});
        return true;
    }
    return false;
}

void Mode::tick(float now) {
    if (!active() || !round_live_) return;
    expire_shields(now);
    const int participant_count = participants();
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!active_player(slot) || !p.alive || p.waiting_next_round) continue;
        if (p.role == Role::armitive && !p.shield_ready_announced && now >= p.shield_available_at) {
            p.shield_ready_announced = true;
            actions_.push_back({Effect::ability_ready, slot, 0, p.role, {}});
        }
        if (p.role == Role::invisant && !p.leap_ready_announced && now >= p.leap_available_at) {
            p.leap_ready_announced = true;
            actions_.push_back({Effect::ability_ready, slot, 0, p.role, {}});
        }
        if (p.role == Role::invisant && p.applied_upgrades[static_cast<int>(Upgrade::reforging)] > 0 &&
            now - p.last_reforge_at >= settings_.reforging_interval) {
            p.last_reforge_at = now;
            if (now - p.last_damage_taken_at >= settings_.reforging_interval)
                actions_.push_back({Effect::restore_armor, slot,
                    settings_.reforging_armor_per_stage *
                    p.applied_upgrades[static_cast<int>(Upgrade::reforging)], p.role, {}});
        }
        if (p.role == Role::armitive && p.applied_upgrades[static_cast<int>(Upgrade::recovery)] > 0 &&
            now - p.last_recovery_at >= settings_.recovery_interval) {
            p.last_recovery_at = now;
            const float camping_damage_at = std::max(0.0f, settings_.camping_warning_seconds) +
                std::max(0.0f, settings_.camping_damage_delay);
            const bool taking_camping_damage = participant_count >= 2 && freeze_ended_ &&
                p.camping_position_ready && settings_.camping_distance > 0.0f &&
                now - p.camping_anchor_at >= camping_damage_at;
            if (!taking_camping_damage &&
                now - p.last_damage_taken_at >= settings_.recovery_interval)
                actions_.push_back({Effect::heal, slot,
                    settings_.recovery_health_per_stage *
                    p.applied_upgrades[static_cast<int>(Upgrade::recovery)], p.role, {}});
        }
    }
    if (participant_count < 2) {
        for (int slot = 1; slot <= kMaxPlayers; ++slot) {
            if (!active_player(slot)) continue;
            players_[slot].last_activity = now;
            players_[slot].camping_warned = false;
            players_[slot].camping_active = false;
            players_[slot].camping_anchor_at = now;
            players_[slot].last_camping_tick_at = now;
        }
        return;
    }
    if (freeze_ended_ && !round_result_emitted_ && now - active_started_at_ >= settings_.round_seconds) {
        round_result_emitted_ = true;
        actions_.push_back({Effect::end_round, 0, 0, Role::armitive, "Round timer expired"});
        return;
    }
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!active_player(slot) || !p.alive || p.waiting_next_round) continue;
        if (p.bot) continue;
        bool afk_priority = p.afk_death_pending;
        if (freeze_ended_ && settings_.afk_seconds > 0) {
            const float remaining = settings_.afk_seconds - (now - p.last_activity);
            if (remaining <= 0.0f && !p.afk_death_pending) {
                p.afk_death_pending = true;
                afk_priority = true;
                actions_.push_back({Effect::kill_afk, slot, 0, p.role, {}});
            } else if (settings_.afk_warning_seconds > 0 && remaining > 0.0f &&
                       remaining <= settings_.afk_warning_seconds) {
                afk_priority = true;
                const int seconds = std::max(1, static_cast<int>(std::ceil(remaining)));
                if (!p.afk_warned) {
                    p.afk_warned = true;
                    actions_.push_back({Effect::afk_warning_start, slot, seconds, p.role, {}});
                }
                if (seconds != p.last_afk_warning_second) {
                    p.last_afk_warning_second = seconds;
                    actions_.push_back({Effect::afk_warning, slot, seconds, p.role, {}});
                }
            }
        }
        if (freeze_ended_ && p.role == Role::armitive && p.camping_position_ready &&
            settings_.camping_distance > 0.0f && !afk_priority) {
            const float elapsed = now - p.camping_anchor_at;
            const float warning_at = std::max(0.0f, settings_.camping_warning_seconds);
            const float damage_at = warning_at + std::max(0.0f, settings_.camping_damage_delay);
            if (elapsed >= warning_at && !p.camping_warned) {
                p.camping_warned = true;
                actions_.push_back({Effect::camping_start, slot, 0, p.role, {}});
            }
            if (elapsed >= damage_at && !p.camping_active) {
                p.camping_active = true;
                p.last_camping_tick_at = now - 1.0f;
            }
            if (p.camping_active && now - p.last_camping_tick_at >= 1.0f) {
                p.last_camping_tick_at = now;
                actions_.push_back({Effect::camping_damage, slot,
                    std::max(1, settings_.camping_damage), p.role, {}});
            }
        }
    }
    tick_starvation(now);
    last_tick_at_ = now;
}

bool Mode::tick_starvation(float now) {
    if (!active() || !round_live_ || !freeze_ended_ || round_result_emitted_ ||
        participants() < 2 || alive(Role::armitive) == 0) return false;
    bool queued = false;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!active_human(slot) || !p.alive || p.waiting_next_round ||
            p.role != Role::invisant) continue;
        const float starvation_at = std::max(
            active_started_at_ + std::max(0.0f, settings_.starvation_round_delay),
            p.last_invisant_damage_at + std::max(0.0f, settings_.starvation_combat_delay));
        if (now < starvation_at) {
            p.starvation_active = false;
            continue;
        }
        if (!p.starvation_active) {
            p.starvation_active = true;
            p.starvation_started_at = now;
            p.next_starvation_tick_at = now;
            actions_.push_back({Effect::starvation_start, slot, 0, p.role, {}});
            queued = true;
        }
        if (now < p.next_starvation_tick_at) continue;
        const float acceleration_at = p.starvation_started_at +
            std::max(0.0f, settings_.starvation_acceleration_delay);
        const bool accelerated = now >= acceleration_at;
        const float interval = std::max(0.1f, accelerated
            ? settings_.starvation_accelerated_interval : settings_.starvation_interval);
        const int amount = std::max(1, accelerated
            ? settings_.starvation_accelerated_damage : settings_.starvation_damage);
        p.last_starvation_tick_at = now;
        // Retain fractional intervals between game frames. A delayed frame
        // applies one tick and resumes the cadence without a catch-up burst.
        p.next_starvation_tick_at += interval;
        if (p.next_starvation_tick_at <= now)
            p.next_starvation_tick_at = now + interval;
        if (!accelerated)
            p.next_starvation_tick_at = std::min(p.next_starvation_tick_at, acceleration_at);
        actions_.push_back({Effect::starvation_damage, slot, amount, p.role, {}});
        queued = true;
    }
    return queued;
}

int Mode::required_upgrade_level(Upgrade upgrade, int current_stage) const {
    const bool ultimate = upgrade == Upgrade::leap || upgrade == Upgrade::shield;
    return (ultimate ? settings_.ultimate_upgrade_levels : settings_.normal_upgrade_levels)
        [current_stage];
}

bool Mode::purchase(int slot, Upgrade upgrade, float now) {
    if (!active_human(slot)) return false;
    Player &p = players_[slot];
    const int choice = static_cast<int>(upgrade);
    const Role role = choice < 8 ? Role::invisant : Role::armitive;
    if (p.role != role) return false;
    const int role_index = role == Role::invisant ? 0 : 1;
    const int stage = p.upgrades[choice];
    if (stage >= kMaxUpgradeStage || p.points[role_index] <= 0) return false;
    if (level(p.xp[role_index]) < required_upgrade_level(upgrade, stage)) return false;
    ++p.upgrades[choice];
    const bool applies_now = !round_live_ ||
        now - round_started_at_ <= settings_.upgrade_purchase_window;
    if (applies_now)
        p.applied_upgrades[choice] = p.upgrades[choice];
    p.upgrade_pending[choice] = !applies_now;
    --p.points[role_index];
    return true;
}

void Mode::reset_upgrades(int slot, Role role, float now) {
    if (!active_human(slot) || role == Role::spectator) return;
    Player &p = players_[slot];
    const int start = role == Role::invisant ? 0 : 8;
    const bool applies_now = !round_live_ ||
        now - round_started_at_ <= settings_.upgrade_purchase_window;
    for (int i = start; i < start + 8; ++i) {
        p.points[start / 8] += p.upgrades[i];
        p.upgrades[i] = 0;
        if (applies_now)
            p.applied_upgrades[i] = 0;
        p.upgrade_pending[i] = !applies_now;
    }
    if (role == Role::armitive && applies_now && p.shield_until > 0.0f) {
        p.shield_until = 0.0f;
        actions_.push_back({Effect::shield_end, slot, 0, role, {}});
    }
}

void Mode::set_level(int slot, Role role, int value) {
    if (!active_human(slot) || role == Role::spectator) return;
    Player &p = players_[slot];
    if (role == Role::armitive && p.role == Role::armitive && p.shield_until > 0.0f) {
        p.shield_until = 0.0f;
        actions_.push_back({Effect::shield_end, slot, 0, role, {}});
    }
    const int index = role == Role::invisant ? 0 : 1;
    p.xp[index] = required_xp(value);
    p.points[index] = 0;
    for (int i = index * 8; i < index * 8 + 8; ++i) p.upgrades[i] = 0;
    for (int i = index * 8; i < index * 8 + 8; ++i) p.applied_upgrades[i] = 0;
    for (int i = index * 8; i < index * 8 + 8; ++i) p.upgrade_pending[i] = false;
    p.points[index] = std::clamp(value, 0, kMaxLevel);
    actions_.push_back({Effect::xp_changed, slot, p.xp[index], role, {}});
}

bool Mode::admin_restart() {
    if (!active() || !round_live_ || active_humans() < 1) return false;
    for (int slot = 1; slot <= kMaxPlayers; ++slot) {
        Player &p = players_[slot];
        if (!active_player(slot)) continue;
        if (p.role == Role::invisant && p.rounds_invisant > 0) --p.rounds_invisant;
        p.earned_invisant = false;
        p.round_kills = 0;
    }
    advance_next_round_ = false;
    round_result_emitted_ = true;
    actions_.push_back({Effect::restart_round, 0, 0, Role::spectator, {}});
    return true;
}

bool Mode::admin_force_invisant(int slot) {
    if (!active() || !active_human(slot) ||
        players_[slot].role == Role::invisant) return false;
    Player &p = players_[slot];
    p.promotion_stamp = p.queue_stamp;
    p.queue_stamp = 0;
    p.earned_invisant = false;
    p.forfeited_invisant = false;
    p.spectator_locked = false;
    p.waiting_next_round = false;
    assign(slot, Role::invisant);
    actions_.push_back({Effect::respawn, slot, 1, Role::invisant, {}});
    return true;
}

bool Mode::admin_force_armitive(int slot) {
    if (!active() || !active_human(slot) ||
        players_[slot].role == Role::armitive) return false;
    Player &p = players_[slot];
    p.earned_invisant = false;
    p.forfeited_invisant = false;
    p.spectator_locked = false;
    p.waiting_next_round = false;
    assign(slot, Role::armitive);
    if (p.queue_stamp == 0) queue_back(slot);
    actions_.push_back({Effect::respawn, slot, 1, Role::armitive, {}});
    return true;
}

int Mode::queue_position(int slot, float now) const {
    if (!active_human(slot)) return 0;
    const Player &p = players_[slot];
    if (p.role != Role::armitive || p.afk_queue_removed || afk(slot, now)) return 0;
    int position = 1;
    for (int other = 1; other <= kMaxPlayers; ++other) {
        if (other == slot || !active_player(other)) continue;
        const Player &candidate = players_[other];
        if (candidate.role != Role::armitive || candidate.afk_queue_removed ||
            afk(other, now)) continue;
        if (p.earned_invisant) {
            if (candidate.earned_invisant && candidate.queue_stamp < p.queue_stamp)
                ++position;
        } else if (candidate.earned_invisant || candidate.queue_stamp < p.queue_stamp) {
            ++position;
        }
    }
    return position;
}

int Mode::afk_countdown(int slot, float now) const {
    if (!active() || !round_live_ || !freeze_ended_ || !active_human(slot) ||
        participants() < 2 || settings_.afk_seconds <= 0 || settings_.afk_warning_seconds <= 0)
        return 0;
    const Player &player = players_[slot];
    if (!player.alive || player.waiting_next_round || !player.afk_warned || player.afk_death_pending)
        return 0;
    const float remaining = settings_.afk_seconds - (now - player.last_activity);
    if (remaining <= 0.0f || remaining > settings_.afk_warning_seconds) return 0;
    return static_cast<int>(std::ceil(remaining));
}

int Mode::round_time_remaining(float now) const {
    if (!round_live_ || !freeze_ended_) return settings_.round_seconds;
    return std::max(0, static_cast<int>(std::ceil(
        settings_.round_seconds - (now - active_started_at_))));
}

int Mode::timeout_reveal_alpha(float now) const {
    if (!timeout_reveal_active_ || timeout_reveal_duration_ <= 0.0f) return 255;
    return std::clamp(static_cast<int>(std::lround(
        255.0f * (now - timeout_reveal_started_at_) / timeout_reveal_duration_)), 0, 255);
}

} // namespace shroudfire
