#pragma once

#include "audio_settings.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace shroudfire {

constexpr int kMaxPlayers = 64;
constexpr int kMaxLevel = 40;
constexpr int kMaxUpgradeStage = 5;

enum class Role : uint8_t { spectator, armitive, invisant };
enum class DeathCause : uint8_t {
    enemy, suicide, fall, world, afk, starvation, camping, forced, timeout
};
enum class WeaponKind : uint8_t { other, knife, pistol };
enum class Upgrade : uint8_t {
    lightweight, momentum, resilience, reforging, vampirism, bloodlust, mobility, leap,
    fortitude, recovery, overload, stockpile, barrage, control, dexterity, shield
};
enum class Effect : uint8_t {
    change_team, respawn, restart_round, end_round, xp_changed, afk_warning_start, afk_warning,
    kill_afk, kick_afk, starvation_start, starvation_damage, camping_start, camping_damage,
    xp_notice, kill_reward, invisant_priority,
    round_win_reward, streak_cue, heal, restore_armor,
    leap, shield_start, shield_end, ability_ready, kill_timeout, post_round_kill, assist_reward,
    round_completed
};

struct Action {
    Effect effect;
    int player = 0;
    int value = 0;
    Role role = Role::spectator;
    std::string message;
    int target = 0;
    WeaponKind weapon = WeaponKind::other;
    int previous_level = -1;
};

struct Settings {
    bool enabled = true;
    int armitive_per_invisant = 4;
    int round_seconds = 120;
    bool timeout_kill_invisant = false;
    float timeout_reveal_seconds = 2.0f;
    int map_join_grace = 30;
    int afk_seconds = 30;
    int afk_warning_seconds = 10;
    int afk_limit = 5;
    float starvation_round_delay = 60.0f;
    float starvation_combat_delay = 45.0f;
    int starvation_damage = 1;
    float starvation_interval = 1.0f;
    float starvation_acceleration_delay = 20.0f;
    float starvation_accelerated_interval = 1.0f;
    int starvation_accelerated_damage = 3;
    float camping_distance = 256.0f;
    float camping_warning_seconds = 20.0f;
    float camping_damage_delay = 10.0f;
    int camping_damage = 10;
    int reconnect_grace = 600;
    int xp_kill_invisant = 50;
    int xp_kill_armitive = 100;
    int xp_round_win_invisant = 70;
    int xp_round_win_armitive = 35;
    int xp_armitive_pistol_percent = 125;
    int xp_armitive_knife_percent = 150;
    int xp_level_base = 100;
    float assist_fraction = 0.25f;
    float assist_damage_share = 0.25f;
    float kill_streak_window = 10.0f;
    std::array<int, 10> streak_percent {125, 120, 130, 140, 150, 160, 170, 180, 190, 200};
    std::array<int, 5> normal_upgrade_levels {1, 5, 10, 15, 20};
    std::array<int, 5> ultimate_upgrade_levels {8, 16, 24, 32, 40};
    int invisant_base_armor = 25;
    int lightweight_gravity_percent = 10;
    int momentum_run_percent = 3;
    int resilience_armor_per_stage = 15;
    int resilience_helmet_stage = 5;
    int reforging_armor_per_stage = 1;
    int reforging_interval = 3;
    int vampirism_health_per_stage = 3;
    int bloodlust_health_per_stage = 6;
    int mobility_crouch_percent = 15;
    int surge_base_speed = 480;
    int surge_speed_per_stage = 30;
    bool surge_add_momentum = false;
    int surge_cooldown = 20;
    int armitive_base_health = 100;
    int fortitude_health_per_stage = 10;
    int recovery_health_per_stage = 1;
    int recovery_interval = 3;
    int overload_magazine_percent = 10;
    int stockpile_clips_per_stage = 1;
    int barrage_fire_rate_percent = 7;
    int control_reduction_percent = 10;
    int dexterity_speed_percent = 12;
    float ward_base_seconds = 2.0f;
    float ward_seconds_per_stage = 0.5f;
    int ward_cooldown = 30;
    int upgrade_purchase_window = 20;
};

struct CombatStats {
    int kills = 0;
    int assists = 0;
    int deaths = 0;
};

struct Profile {
    uint64_t steam_id = 0;
    int queue_stamp = 0;
    int last_seen = 0;
    std::array<int, 2> xp {};
    std::array<int, 2> points {};
    std::array<int, 16> upgrades {};
    std::array<CombatStats, 2> stats {};
    uint32_t muted_audio = 0;
};

struct Attributes {
    int maximum_health = 100;
    int maximum_armor = 0;
    bool helmet = false;
    float gravity_scale = 1.0f;
    float movement_scale = 1.0f;
    float crouch_speed_scale = 1.0f;
    float magazine_scale = 1.0f;
    int extra_reserve_clips = 0;
    float fire_delay_scale = 1.0f;
    float reload_delay_scale = 1.0f;
    float recoil_scale = 1.0f;
    float spread_scale = 1.0f;
    int leap_impulse = 0;
    float shield_seconds = 0.0f;
};

struct Player {
    uint64_t steam_id = 0;
    bool connected = false;
    bool bot = false;
    bool alive = false;
    bool waiting_next_round = false;
    bool earned_invisant = false;
    bool forfeited_invisant = false;
    bool afk_queue_removed = false;
    bool afk_warned = false;
    bool afk_death_pending = false;
    bool starvation_active = false;
    bool camping_position_ready = false;
    bool camping_warned = false;
    bool camping_active = false;
    bool moved_this_round = false;
    bool round_participant = false;
    DeathCause round_death_cause = DeathCause::forced;
    bool spectator_locked = false;
    Role role = Role::spectator;
    int queue_stamp = 0;
    int promotion_stamp = 0;
    int rounds_invisant = 0;
    int round_kills = 0;
    int streak_count = 0;
    int consecutive_afk_deaths = 0;
    int last_armitive_attacker = 0;
    int last_afk_warning_second = -1;
    float connected_at = 0.0f;
    float last_activity = 0.0f;
    float last_kill_at = 0.0f;
    float last_invisant_damage_at = 0.0f;
    float last_starvation_tick_at = 0.0f;
    float starvation_started_at = 0.0f;
    float next_starvation_tick_at = 0.0f;
    float camping_anchor_x = 0.0f;
    float camping_anchor_y = 0.0f;
    float camping_anchor_z = 0.0f;
    float camping_anchor_at = 0.0f;
    float last_camping_tick_at = 0.0f;
    float last_damage_taken_at = 0.0f;
    float last_weapon_fire_at = 0.0f;
    float last_reforge_at = 0.0f;
    float last_recovery_at = 0.0f;
    float leap_available_at = 0.0f;
    float shield_until = 0.0f;
    float shield_available_at = 0.0f;
    bool leap_ready_announced = true;
    bool shield_ready_announced = true;
    std::array<int, 2> xp {};
    std::array<int, 2> points {};
    std::array<int, 16> upgrades {};
    std::array<int, 16> applied_upgrades {};
    std::array<bool, 16> upgrade_pending {};
    std::array<CombatStats, 2> stats {};
    uint32_t muted_audio = 0;
};

class Mode {
public:
    explicit Mode(Settings settings = {});
    Settings &settings() { return settings_; }
    const Settings &settings() const { return settings_; }
    const Player &player(int slot) const { return players_.at(slot); }
    std::vector<Action> take_actions();

    void map_start(float now);
    void connect(int slot, uint64_t steam_id, bool bot, const Profile *saved, int unix_time, float now);
    Profile disconnect(int slot, int unix_time);
    void join_spectator(int slot, bool preserve_lone_invisant = false);
    bool request_team(int slot, Role requested, float now);
    void activity(int slot, float now, bool moved = false);
    void movement_sample(int slot, float x, float y, float z, float now);
    bool forfeit_invisant(int slot);
    void spawn(int slot, float now);
    void start_round(float now);
    void freeze_end(float now);
    void begin_round_end();
    void end_round(Role winner, bool completed, bool timed_out = false, float now = 0.0f);
    void damage(int attacker, int victim, int health_damage, float now);
    void damage_taken(int victim, float now, bool counts_for_starvation = true);
    void weapon_fire(int slot, float now);
    void death(int victim, int attacker, DeathCause cause, WeaponKind weapon, float now);
    void tick(float now);
    bool tick_starvation(float now);
    bool use_ability(int slot, float now, bool path_blocked = false);
    bool shield_active(int slot, float now) const;
    bool expire_shields(float now);
    Attributes attributes(int slot) const;
    int required_upgrade_level(Upgrade upgrade, int current_stage) const;
    bool purchase(int slot, Upgrade upgrade, float now);
    void reset_upgrades(int slot, Role role, float now);
    void set_level(int slot, Role role, int level);
    bool admin_restart();
    bool admin_force_invisant(int slot);
    bool admin_force_armitive(int slot);
    Profile profile(int slot, int unix_time) const;
    bool toggle_audio(int slot, AudioCategory category);
    bool audio_enabled(int slot, AudioCategory category) const;

    int desired_invisant_count() const;
    int level(int xp) const;
    int required_xp(int level) const;
    int active_humans() const;
    int active_players() const;
    int alive(Role role) const;
    uint64_t chat_recipients(int sender, bool team_only) const;
    bool can_spectate(int viewer, int target, bool invisant_teammates,
                      bool admin_observation = false) const;
    int queue_position(int slot, float now) const;
    int round_time_remaining(float now) const;
    int afk_countdown(int slot, float now) const;
    bool active() const;
    bool round_live() const { return round_live_; }
    bool freeze_ended() const { return freeze_ended_; }
    bool timeout_reveal_active() const { return timeout_reveal_active_; }
    int timeout_reveal_alpha(float now) const;

private:
    bool valid(int slot) const;
    bool active_human(int slot) const;
    bool active_player(int slot) const;
    int participants() const;
    bool afk(int slot, float now) const;
    int oldest_candidate(const std::array<bool, kMaxPlayers + 1> &selected,
                         const std::array<bool, kMaxPlayers + 1> &previous,
                         bool rewards_only, bool allow_afk, float now);
    void assign(int slot, Role role);
    void queue_back(int slot);
    int award_xp(int slot, Role role, int amount);
    void check_elimination();

    Settings settings_;
    std::array<Player, kMaxPlayers + 1> players_ {};
    std::array<std::array<int, kMaxPlayers + 1>, kMaxPlayers + 1> assist_damage_ {};
    std::vector<Action> actions_;
    int next_queue_stamp_ = 1;
    int front_queue_stamp_ = -1;
    int round_participants_ = 0;
    int round_invisant_participants_ = 0;
    int round_armitive_participants_ = 0;
    bool round_live_ = false;
    bool freeze_ended_ = false;
    bool first_blood_claimed_ = false;
    bool roster_initialized_ = false;
    bool round_result_emitted_ = false;
    bool advance_next_round_ = false;
    bool timeout_reveal_active_ = false;
    float timeout_reveal_started_at_ = 0.0f;
    float timeout_reveal_duration_ = 2.0f;
    float map_started_at_ = 0.0f;
    float round_started_at_ = 0.0f;
    float active_started_at_ = 0.0f;
    float last_tick_at_ = 0.0f;
};

} // namespace shroudfire
