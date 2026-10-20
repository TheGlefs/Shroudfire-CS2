#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class IGameResourceService;
class ISchemaSystem;
class INetworkMessages;
class IGameEventSystem;
class IGameEventManager2;
class CEntityInstance;
namespace shroudfire { struct Attributes; }

struct SoundEventResult {
    uint32_t guid = 0;
    uint32_t padding = 0;
    uint64_t stack_hash = 0;
    uint64_t reserved = 0;
};

class EngineAdapter {
public:
    static constexpr size_t kMaxVisualEntities = 100;

    bool initialize(IGameResourceService *resources, ISchemaSystem *schema,
                    const void *server_interface, INetworkMessages *network_messages,
                    IGameEventSystem *event_system,
                    std::string &error);
    bool ready() const { return ready_; }
    bool glow_ready() const { return ready_ && glow_fields_ok_ &&
        keyvalues_constructor_function_ && keyvalues_find_or_create_function_ &&
        keyvalues_set_string_function_; }
    bool change_team(int slot, int team) const;
    bool player_alive(int slot) const;
    int player_health(int slot) const;
    int player_armor(int slot) const;
    bool set_reveal_alpha(int slot, int expected_team, int alpha);
    void restore_reveal_rendering();
    bool active_weapon_is_knife(int slot) const;
    void remap_knife_primary_attack(void *movement_services) const;
    uint32_t ignore_armor_damage_flag() const { return ignore_armor_damage_flag_; }
    float active_weapon_headshot_multiplier(int slot) const;
    bool weapon_use_context(void *services, void *weapon, int &slot, bool &knife) const;
    bool purchase_context(void *services, void *item, int &slot, uint16_t &definition) const;
    int drop_non_knife_weapons(int slot) const;
    bool respawn(int slot) const;
    bool terminate_round(int winning_team, float delay, std::string *diagnostic = nullptr) const;
    std::optional<bool> warmup_active() const;
    bool set_freeze_period(bool frozen) const;
    bool heal(int slot, int amount, int maximum_health) const;
    bool restore_armor(int slot, int amount, int maximum_armor) const;
    bool apply_spawn_attributes(int slot, int team, bool invisant_role, int health, int armor,
                                bool helmet, float gravity_scale) const;
    bool apply_upgrade_attributes(int slot, int team, const shroudfire::Attributes &before,
                                  const shroudfire::Attributes &after) const;
    bool client_print(int slot, int destination, const char *message) const;
    bool stop_sound_event_hash(int slot, uint32_t hash) const;
    bool emit_sound_event(int source_slot, uint64_t recipients, const char *name) const;
    bool center_html(int slot, IGameEventManager2 *events, const char *html,
                     int duration_seconds) const;
    bool stabilize_center_html(float now, bool round_live,
                               bool presentation_frame, bool win_html_active,
                               bool mode_active);
    void clear_round_ui_tracking() {
        html_game_rules_index_ = -1;
        round_ui_was_live_ = false;
        round_ui_captured_ = false;
        round_end_count_before_ = 0;
        end_html_presentation_active_ = false;
        end_html_restart_time_ = 0.0f;
        end_html_game_restart_ = false;
        end_html_presented_restart_time_ = 0.0f;
        end_html_presented_game_restart_ = false;
    }
    bool show_win_layout(int slot, bool invisant_win, int xp, int kill_xp,
                         const char *victim_name, bool victim_invisant,
                         const char *advanced_role, int advanced_level);
    void hide_win_layout(int slot);
    void clear_win_layout_tracking() { win_layout_handle_ = 0; }
    bool refresh_buy_zone(int slot, int team);
    bool can_buy(int slot, int team);
    bool hud_message(int slot, int channel, float x, float y, float hold,
                     int red, int green, int blue, int alpha, const char *message) const;
    bool screen_fade(int slot, int duration, int flags, int red, int green,
                     int blue, int alpha, int hold_time = 0);
    void remove_fade_entity();
    void clear_fade_tracking() { fade_entity_ = 0; }
    bool commit_suicide(int slot) const;
    bool drain_health(int slot, int amount, bool &fatal) const;
    bool leap(int slot, int impulse, bool add_momentum) const;
    bool leap_clear(int slot, bool &clear) const;
    bool blood_burst(int victim_slot, int attacker_slot, int damage, int hitgroup,
                     int bursts, float now,
                     int *particles_created = nullptr);
    void expire_blood_particles(float now);
    void remove_blood_entity();
    void clear_blood_tracking() { blood_entity_ = 0; blood_particles_.clear(); }
    bool set_damage_enabled(int slot, bool enabled, bool &previous) const;
    bool accelerate_fire(int slot, float fire_delay_scale, int server_tick) const;
    bool apply_ammo_upgrades(int slot, int overload_stage, int stockpile_stage,
                             int overload_percent, int stockpile_clips,
                             float now = 0.0f, float reload_scale = 1.0f);
    bool intercept_overload_reload(int slot, int overload_stage, int overload_percent,
                                   float now, float reload_scale);
    void cancel_overload_reload_on_attack(int slot);
    void record_weapon_fire(int slot);
    bool apply_dexterity(int slot, int stage, int speed_percent, float now, int server_tick);
    bool apply_claw_timing(int slot, float now, int server_tick, bool active,
                           int speed_percent,
                           bool *attacked = nullptr);
    bool claw_sweep(int attacker_slot, int armitive_team, float reach,
                    float cone_width, int &target_slot) const;
    bool claw_damage(int attacker_slot, int victim_slot, int damage);
    void remove_claw_entity();
    void clear_claw_entity_tracking() { claw_hurt_entity_ = 0; }
    struct GlowPair {
        uint32_t pawn = 0;
        uint32_t relay = 0;
        uint32_t glow = 0;
        int relay_index = 0;
        int glow_index = 0;
        bool shielded = false;
        std::string model;
    };
    bool update_armitive_glow(int target_slot, bool shielded, bool through_walls,
                            GlowPair &pair);
    void remove_armitive_glow(GlowPair &pair);
    void begin_control(int slot, int stage, int reduction_percent, float now, int server_tick);
    void finish_control(int slot);
    void clear_weapon_tracking() {
        overload_weapons_.clear();
        dexterity_.fill({});
        claws_.fill({});
        control_.fill({});
    }
    void reset_round_ammo_tracking() { overload_weapons_.clear(); }
    bool set_invisant_speed(int slot, float multiplier) const;
    void *process_movement_function() const { return process_movement_function_; }
    int movement_source_slot(const void *services) const;
    bool suppress_buy_zone(int slot) const;
    void reset_player_buy_zone(int slot);
    void record_buy_zone_spawn(int slot, const std::array<float, 3> &position);
    int remove_objectives() const;
    int apply_tonemap(float exposure);
    void restore_tonemap();
    void clear_tonemap_tracking() {
        original_tonemap_.clear();
        original_player_tonemap_.clear();
        created_tonemap_ = 0;
        active_tonemap_ = 0;
    }
    bool spawn_counts(int &terrorists, int &counter_terrorists) const;
    int ensure_spawn_capacity(int terrorist_target, int counter_terrorist_target);
    void remove_extra_spawns();
    void clear_extra_spawn_tracking() {
        extra_spawn_handles_.clear();
        extra_spawn_zones_.clear();
        buyzone_spawn_.fill({});
        base_spawn_counts_ = {};
    }
    bool set_observer(int viewer_slot, int target_slot, bool free_roam = false) const;
    int observer_target_slot(int viewer_slot) const;
    int observer_mode(int viewer_slot) const;
    void *damage_function() const { return take_damage_function_; }
    void *weapon_can_use_function() const { return weapon_can_use_function_; }
    void *can_acquire_function() const { return can_acquire_function_; }
    void *sound_filter_function() const { return sound_filter_function_; }
    void *game_event_manager_init_function() const { return game_event_manager_init_function_; }
    void *host_say_function() const { return host_say_function_; }
    struct DamageContext {
        int victim = 0;
        int attacker = 0;
        uint32_t type = 0;
        uint32_t hitgroup = 0;
        float *damage = nullptr;
        uint32_t *flags = nullptr;
    };
    bool inspect_damage(void *victim_entity, void *info, DamageContext &context) const;
    struct Activity {
        std::array<float, 3> position {};
        std::array<float, 3> angles {};
        uint64_t buttons = 0;
        uint32_t flags = 0;
        bool crouched = false;
    };
    bool sample_activity(int slot, Activity &activity) const;
    void *observer_movement_services(int slot) const;
    int sound_source_slot(int entity_index) const;
    bool collect_visual_group(int slot, int expected_team,
                              std::array<int, kMaxVisualEntities> &indices,
                              size_t &count) const;

private:
    CEntityInstance *controller(int slot) const;
    CEntityInstance *pawn(int slot) const;
    void *entity_system() const;
    bool mark_state_changed(CEntityInstance *entity, uint32_t offset) const;

    struct Fields {
        uint32_t player_pawn = 0;
        uint32_t money_services = 0;
        uint32_t money_account = 0;
        uint32_t pawn_controller = 0;
        uint32_t owner_entity = 0;
        uint32_t health = 0;
        uint32_t flags = 0;
        uint32_t life_state = 0;
        uint32_t move_type = 0;
        uint32_t team = 0;
        uint32_t weapon_services = 0;
        uint32_t next_attack_time = 0;
        uint32_t aim_punch_services = 0;
        uint32_t aim_punch_angle = 0;
        uint32_t aim_punch_velocity = 0;
        uint32_t item_services = 0;
        uint32_t item_definition_index = 0;
        uint32_t has_helmet = 0;
        uint32_t has_defuser = 0;
        uint32_t active_weapon = 0;
        uint32_t last_weapon = 0;
        uint32_t weapons = 0;
        uint32_t wearables = 0;
        uint32_t hostage_services = 0;
        uint32_t carried_hostage_prop = 0;
        uint32_t game_rules = 0;
        uint32_t warmup_period = 0;
        uint32_t freeze_period = 0;
        uint32_t restart_round_time = 0;
        uint32_t game_restart = 0;
        uint32_t round_end_count = 0;
        uint32_t buy_time_ended = 0;
        uint32_t ct_cant_buy = 0;
        uint32_t t_cant_buy = 0;
        uint32_t armor = 0;
        uint32_t gravity_scale = 0;
        uint32_t actual_gravity_scale = 0;
        uint32_t body_component = 0;
        uint32_t scene_node = 0;
        uint32_t abs_origin = 0;
        uint32_t abs_rotation = 0;
        uint32_t eye_angles = 0;
        uint32_t movement_services = 0;
        uint32_t movement_buttons = 0;
        uint32_t movement_ducked = 0;
        uint32_t movement_duck_amount = 0;
        uint32_t button_states = 0;
        uint32_t velocity = 0;
        uint32_t velocity_x = 0;
        uint32_t velocity_y = 0;
        uint32_t velocity_z = 0;
        uint32_t takes_damage = 0;
        uint32_t subclass_id = 0;
        uint32_t reserve_ammo = 0;
        uint32_t clip1 = 0;
        uint32_t primary_attack_tick = 0;
        uint32_t secondary_attack_tick = 0;
        uint32_t primary_clip_max = 0;
        uint32_t reserve_as_clips = 0;
        uint32_t reloads_single_shells = 0;
        uint32_t weapon_in_reload = 0;
        uint32_t accuracy_penalty = 0;
        uint32_t primary_reserve_max = 0;
        uint32_t gear_slot = 0;
        uint32_t headshot_multiplier = 0;
        uint32_t observer_services = 0;
        uint32_t observer_pawn = 0;
        uint32_t observer_mode = 0;
        uint32_t observer_target = 0;
        uint32_t camera_services = 0;
        uint32_t player_tonemap = 0;
        uint32_t maxspeed = 0;
        uint32_t velocity_modifier = 0;
        uint32_t in_buy_zone = 0;
        uint32_t was_in_buy_zone = 0;
        uint32_t touching_buy_zones = 0;
        uint32_t damage_attacker = 0;
        uint32_t damage_amount = 0;
        uint32_t damage_type = 0;
        uint32_t damage_flags = 0;
        uint32_t damage_hitgroup = 0;
        uint32_t point_hurt_damage = 0;
        uint32_t point_hurt_type = 0;
        uint32_t point_hurt_radius = 0;
        uint32_t point_hurt_target = 0;
        uint32_t auto_exposure_min = 0;
        uint32_t auto_exposure_max = 0;
        uint32_t spawn_flags = 0;
        uint32_t fade_color = 0;
        uint32_t fade_duration = 0;
        uint32_t fade_hold_duration = 0;
        uint32_t blood_amount = 0;
        uint32_t blood_color = 0;
        uint32_t blood_spray_dir = 0;
        uint32_t particle_start_active = 0;
        uint32_t particle_effect_name = 0;
        uint32_t model_state = 0;
        uint32_t model_name = 0;
        uint32_t render_mode = 0;
        uint32_t render_color = 0;
        uint32_t glow_property = 0;
        uint32_t glow_color = 0;
        uint32_t glow_type = 0;
        uint32_t glow_team = 0;
        uint32_t glow_range = 0;
        uint32_t glow_range_min = 0;
    } fields_;

    IGameResourceService *resources_ = nullptr;
    INetworkMessages *network_messages_ = nullptr;
    IGameEventSystem *event_system_ = nullptr;
    void *server_module_ = nullptr;
    void *terminate_round_function_ = nullptr;
    void *remove_entity_function_ = nullptr;
    void *create_entity_function_ = nullptr;
    void *dispatch_spawn_function_ = nullptr;
    void *take_damage_function_ = nullptr;
    void *weapon_can_use_function_ = nullptr;
    void *can_acquire_function_ = nullptr;
    void *sound_filter_function_ = nullptr;
    void *process_movement_function_ = nullptr;
    uint32_t ignore_armor_damage_flag_ = 0;
    void *set_player_pawn_function_ = nullptr;
    void *switch_team_function_ = nullptr;
    void *game_event_manager_init_function_ = nullptr;
    void *host_say_function_ = nullptr;
    void *accept_input_function_ = nullptr;
    void *keyvalues_constructor_function_ = nullptr;
    void *keyvalues_find_or_create_function_ = nullptr;
    void *keyvalues_set_string_function_ = nullptr;
    void *hud_set_player_class_function_ = nullptr;
    void *hud_set_player_dialog_function_ = nullptr;
    uint32_t win_layout_handle_ = 0;
    bool ready_ = false;
    bool fade_fields_ok_ = false;
    bool blood_fields_ok_ = false;
    bool particle_fields_ok_ = false;
    bool point_hurt_fields_ok_ = false;
    bool glow_fields_ok_ = false;
    bool render_fields_ok_ = false;
    struct RevealRenderState {
        uint8_t mode = 0;
        std::array<uint8_t, 4> color {};
    };
    std::unordered_map<uint32_t, RevealRenderState> original_reveal_rendering_;
    bool spawn_rotation_ok_ = false;
    bool html_restart_fields_ok_ = false;
    bool round_end_count_field_ok_ = false;
    bool round_ui_was_live_ = false;
    bool round_ui_captured_ = false;
    uint8_t round_end_count_before_ = 0;
    bool end_html_presentation_active_ = false;
    float end_html_restart_time_ = 0.0f;
    bool end_html_game_restart_ = false;
    float end_html_presented_restart_time_ = 0.0f;
    bool end_html_presented_game_restart_ = false;
    bool warmup_field_ok_ = false;
    bool freeze_field_ok_ = false;
    bool buy_rules_fields_ok_ = false;
    int html_game_rules_index_ = -1;
    uint32_t fade_entity_ = 0;
    uint32_t blood_entity_ = 0;
    std::vector<std::pair<uint32_t, float>> blood_particles_;
    uint32_t claw_hurt_entity_ = 0;
    std::vector<uint32_t> extra_spawn_handles_;
    struct ExtraSpawnZone { int team = 0; std::array<float, 3> position {}; };
    std::vector<ExtraSpawnZone> extra_spawn_zones_;
    struct BuyZoneAnchor { uint32_t pawn = 0; std::array<float, 3> position {}; };
    std::array<BuyZoneAnchor, 65> buyzone_spawn_ {};
    std::array<int, 2> base_spawn_counts_ {};
    struct OverloadWeapon {
        int owner_slot = 0;
        int overload_stage = 0;
        int stockpile_stage = 0;
        int overload_percent = 10;
        int stockpile_clips = 1;
        bool reloading = false;
        int reserve_round_credit = 0;
        int reload_start_clip = 0;
        int reload_start_reserve = 0;
        int shell_reload_clip = 0;
        int shell_reload_reserve = 0;
        float shell_reload_next_at = 0.0f;
        int last_clip = 0;
        int last_reserve = 0;
        int pending_shots = 0;
    };
    std::unordered_map<uint32_t, OverloadWeapon> overload_weapons_;
    struct DexterityState {
        uint32_t weapon = 0;
        bool reloading = false;
        float reload_finish_at = 0.0f;
    };
    std::array<DexterityState, 65> dexterity_ {};
    struct ClawState {
        uint32_t weapon = 0;
        int primary_tick = 0;
        int secondary_tick = 0;
        float next_attack = 0.0f;
    };
    std::array<ClawState, 65> claws_ {};
    struct ControlSnapshot {
        uint32_t pawn = 0;
        uint32_t weapon = 0;
        int clip = 0;
        float reduction = 0.0f;
        float original_penalty = 0.0f;
        std::array<float, 3> angle {};
        std::array<float, 3> velocity {};
        bool valid = false;
    };
    std::array<ControlSnapshot, 65> control_ {};
    struct TonemapOriginal { float minimum = 1.0f; float maximum = 1.0f; };
    std::unordered_map<uint32_t, TonemapOriginal> original_tonemap_;
    std::unordered_map<uint32_t, uint32_t> original_player_tonemap_;
    uint32_t created_tonemap_ = 0;
    uint32_t active_tonemap_ = 0;
};
