#pragma once

#include "mode.h"
#include "menu_controls.h"
#include "storage.h"
#include "engine_adapter.h"
#include "center_notice.h"
#include <version_gen.h>
#include "chat_routing.h"
#include "ranking.h"
#include "audio_menu.h"
#include "weapon_restrictions.h"

#include <ISmmPlugin.h>
#include <eiface.h>
#include <checktransmitinfo.h>
#include <igameevents.h>
#include <iserver.h>
#include <playerslot.h>
#include <tier1/convar.h>

#include <array>
#include <atomic>
#include <chrono>
#include <optional>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

class IRecipientFilter;
class IGameEventSystem;
class INetworkMessageInternal;
class CNetMessage;

enum class GuideKind { help, upgrades, cheat, known_bugs, audio, rank };
class ShroudfirePlugin final : public ISmmPlugin, public IMetamodListener {
public:
    ShroudfirePlugin();
    bool Load(PluginId id, ISmmAPI *api, char *error, size_t maximum, bool late) override;
    bool Unload(char *error, size_t maximum) override;
    void AllPluginsLoaded() override;
    void OnLevelInit(const char *map, const char *entities, const char *old_level,
                     const char *landmark, bool load_game, bool background) override;
    void OnLevelShutdown() override;
    void FireGameEvent(IGameEvent *event);
    KHook::Return<bool> Hook_FireGameEvent(IGameEventManager2 *, IGameEvent *event,
                                            bool dont_broadcast);
    KHook::Return<void> Hook_GameEventManagerInit(IGameEventManager2 *manager);
    KHook::Return<void> Hook_GameEventManagerInitialized(IGameEventManager2 *manager);
    KHook::Return<void> Hook_HostSay(CEntityInstance *controller, CCommand &args,
                                    bool team_only, int unknown, const char *unknown_text);
    KHook::Return<void> Hook_HostSayPost(CEntityInstance *, CCommand &, bool, int, const char *);

    KHook::Return<void> Hook_GameFrame(IServerGameDLL *, bool simulating, bool first, bool last);
    KHook::Return<void> Hook_GameFramePre(IServerGameDLL *, bool simulating, bool first, bool last);
    KHook::Return<void> Hook_ClientActive(IServerGameClients *, CPlayerSlot slot, bool load_game,
                                          const char *name, uint64 xuid);
    KHook::Return<void> Hook_ClientDisconnect(IServerGameClients *, CPlayerSlot slot,
        ENetworkDisconnectionReason reason, const char *name, uint64 xuid, const char *network_id);
    KHook::Return<void> Hook_OnClientConnected(IServerGameClients *, CPlayerSlot slot,
        const char *name, uint64 xuid, const char *network_id, const char *address, bool fake);
    KHook::Return<void> Hook_ClientCommand(IServerGameClients *, CPlayerSlot slot, const CCommand &args);
    KHook::Return<void> Hook_DispatchConCommand(ICvar *, ConCommandRef,
        const CCommandContext &, const CCommand &);
    KHook::Return<void> Hook_CheckTransmit(ISource2GameEntities *, CCheckTransmitInfo **infos,
        int count, CBitVec<MAX_EDICTS> &union_edicts, CBitVec<MAX_EDICTS> &union_always,
        const Entity2Networkable_t **networkables, const uint16 *indices, int index_count);
    KHook::Return<int64_t> Hook_TakeDamage(void *victim, void *info, void *result);
    KHook::Return<bool> Hook_WeaponCanUse(void *services, void *weapon);
    KHook::Return<int> Hook_CanAcquire(void *services, void *item, int method, uint64_t context);
    KHook::Return<SoundEventResult> Hook_EmitSoundFilter(IRecipientFilter &filter,
        int source_index, const void *params);
    KHook::Return<SoundEventResult> Hook_EmitSoundFilterPost(IRecipientFilter &filter,
        int source_index, const void *params);
    KHook::Return<void> Hook_ProcessMovement(void *services, void *move);
    KHook::Return<void> Hook_ProcessMovementPost(void *services, void *move);
    KHook::Return<void> Hook_PlayerRunCommand(void *services, void *command);
    KHook::Return<void> Hook_PostSoundEvent(IGameEventSystem *, CSplitScreenSlot, bool,
        int, const uint64 *, INetworkMessageInternal *, const CNetMessage *,
        unsigned long, NetChannelBufType_t);

    void show_level(int slot);
    void show_menu(int slot);
    void render_menu(int slot);
    void close_menu(int slot);
    void show_guide(int slot, GuideKind kind);
    void show_audio_settings(int slot);
    void render_guide(int slot);
    void close_guide(int slot);
    void command_status(int slot);
    void command_update_notice(int slot, const CCommand &args);
    void command_cvar(int slot, const CCommand &args);
    void command_setlevel(int slot, const CCommand &args);
    void command_restart(int slot);
    void command_nextmap(int slot, const CCommand &args);
    void command_invisant(int slot);
    void command_armitive(int slot);
    void command_observe(int slot, const CCommand &args);

    const char *GetAuthor() override { return PLUGIN_AUTHOR; }
    const char *GetName() override { return PLUGIN_DISPLAY_NAME; }
    const char *GetDescription() override { return PLUGIN_DESCRIPTION; }
    const char *GetURL() override { return PLUGIN_URL; }
    const char *GetLicense() override { return PLUGIN_LICENSE; }
    const char *GetVersion() override { return PLUGIN_FULL_VERSION; }
    const char *GetDate() override { return __DATE__; }
    const char *GetLogTag() override { return PLUGIN_LOGTAG; }

private:
    void sync_settings();
    void sync_game_cvars();
    void restore_game_cvars();
    bool map_available(const std::string &map) const;
    std::string next_map() const;
    void update_map_rotation(float now);
    void sample_player_activity(float now);
    void update_hud(float now);
    void refresh_armitive_glows(float now);
    void clear_armitive_glows();
    void play_sound(int slot, const char *sound_name,
        shroudfire::AudioCategory category = shroudfire::AudioCategory::count) const;
    void print_upgrade_purchase(int slot, shroudfire::Upgrade upgrade) const;
    bool authorize_admin(int slot, bool root) const;
    void admin_reply(int slot, const char *message) const;
    int find_player(const char *argument) const;
    void restore_shields();
    void refresh_local_movement_sounds(float now);
    void end_observation(int viewer, const char *reason);
    bool handle_chat_command(int slot, const char *message, bool from_chat = true);
    void broadcast_discord_invite();
    void process_actions();
    void apply_engine_actions(float now);
    void refresh_transmit_state();
    void refresh_timeout_reveal(float now);
    int event_player(IGameEvent *event, const char *key) const;
    float game_time() const;

    IServerGameDLL *server_ {};
    IServerGameClients *game_clients_ {};
    IVEngineServer2 *engine_ {};
    ICvar *cvar_ {};
    INetworkServerService *network_service_ {};
    IGameEventManager2 *game_events_ {};
    IGameResourceService *game_resources_ {};
    ISource2GameEntities *game_entities_ {};
    EngineAdapter engine_adapter_;
    shroudfire::Mode mode_;
    std::optional<shroudfire::ProfileStore> profiles_;
    shroudfire::Rankings rankings_;
    std::array<bool, shroudfire::kMaxPlayers + 1> fake_player_ {};
    // 0: inactive, -1: administrator free camera, positive: observed player slot.
    std::array<std::atomic<int>, shroudfire::kMaxPlayers + 1> admin_observe_target_ {};
    std::array<uint64_t, shroudfire::kMaxPlayers + 1> admin_observe_target_steam_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> admin_observe_started_at_ {};
    std::array<std::string, shroudfire::kMaxPlayers + 1> player_names_ {};
    std::unordered_map<uint64_t, uint8_t> admins_;
    std::array<bool, shroudfire::kMaxPlayers + 1> suicide_command_ {};
    std::array<bool, 2> suicide_cheat_flags_removed_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> forced_death_pending_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> role_respawn_pending_ {};
    std::array<shroudfire::DeathCause, shroudfire::kMaxPlayers + 1> forced_death_cause_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> menu_open_ {};
    std::array<shroudfire::Role, shroudfire::kMaxPlayers + 1> menu_role_ {};
    std::array<int, shroudfire::kMaxPlayers + 1> menu_selection_ {};
    std::array<std::string, shroudfire::kMaxPlayers + 1> menu_last_html_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> menu_last_sent_at_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> menu_first_send_at_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> guide_open_ {};
    std::array<GuideKind, shroudfire::kMaxPlayers + 1> guide_kind_ {};
    std::array<int, shroudfire::kMaxPlayers + 1> guide_page_ {};
    std::array<int, shroudfire::kMaxPlayers + 1> guide_option_selection_ {};
    std::array<shroudfire::AudioMenu, shroudfire::kMaxPlayers + 1> audio_menus_ {};
    std::array<std::string, shroudfire::kMaxPlayers + 1> guide_last_html_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> guide_last_sent_at_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> guide_first_send_at_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> guide_hint_due_ {};
    std::chrono::steady_clock::time_point cheat_hint_available_at_ {};
    float cheat_hint_pending_at_ = 0.0f;
    std::chrono::steady_clock::time_point audio_hint_available_at_ {};
    float audio_hint_pending_at_ = 0.0f;
    std::chrono::steady_clock::time_point upgrade_hint_available_at_ {};
    float upgrade_hint_pending_at_ = 0.0f;
    std::chrono::steady_clock::time_point bugs_hint_available_at_ {};
    float bugs_hint_pending_at_ = 0.0f;
    std::chrono::steady_clock::time_point menu_hint_available_at_ {};
    float menu_hint_pending_at_ = 0.0f;
    std::chrono::steady_clock::time_point reset_hint_available_at_ {};
    float reset_hint_pending_at_ = 0.0f;
    std::chrono::steady_clock::time_point rank_hint_available_at_ {};
    float rank_hint_pending_at_ = 0.0f;
    float discord_hint_pending_at_ = 0.0f;
    std::array<EngineAdapter::Activity, shroudfire::kMaxPlayers + 1> activity_samples_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> activity_sample_ready_ {};
    std::array<shroudfire::PanelMenuInput, shroudfire::kMaxPlayers + 1> panel_menu_input_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> shield_applied_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> local_movement_next_refresh_at_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> shield_previous_damage_ {};
    std::array<int, shroudfire::kMaxPlayers + 1> shield_tint_alpha_ {};
    std::array<float, shroudfire::kMaxPlayers + 1> shield_next_sound_at_ {};
    struct PendingClawHit {
        int victim = 0;
        int tick = 0;
        uint64_t attacker_steam = 0;
        uint64_t victim_steam = 0;
    };
    std::array<PendingClawHit, shroudfire::kMaxPlayers + 1> pending_claw_hits_ {};
    std::array<int, shroudfire::kMaxPlayers + 1> last_native_claw_hit_tick_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> claw_extended_applying_ {};
    std::array<int, shroudfire::kMaxPlayers + 1> last_extended_claw_hit_tick_ {};
    std::array<CenterNotice, shroudfire::kMaxPlayers + 1> center_notices_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> round_info_sent_ {};
    std::array<bool, shroudfire::kMaxPlayers + 1> auto_hud_visible_ {};
    float round_info_until_ = 0.0f;
    float round_info_at_ = 0.0f;
    float timeout_warning_at_ = 0.0f;
    bool timeout_warning_voice_played_ = false;
    float pending_round_end_at_ = 0.0f;
    int pending_round_winner_ = 0;
    bool round_timeout_requested_ = false;
    bool draw_termination_requested_ = false;
    float round_voice_at_ = 0.0f;
    float round_win_voice_at_ = 0.0f;
    shroudfire::Role round_win_voice_role_ = shroudfire::Role::spectator;
    int last_armitive_round_voice_ = -1;
    int last_invisant_round_voice_ = -1;
    int last_armitive_win_voice_ = -1;
    int last_invisant_win_voice_ = -1;
    int last_draw_voice_ = -1;
    int last_timeout_warning_voice_ = -1;
    int last_starvation_voice_ = -1;
    int last_afk_warning_voice_ = -1;
    int last_camping_voice_ = -1;
    float round_start_voice_stop_next_at_ = 0.0f;
    int round_start_voice_stop_passes_ = 0;
    float round_draw_stop_until_ = 0.0f;
    float restart_draw_suppression_until_ = 0.0f;
    float round_win_stop_next_at_ = 0.0f;
    int round_win_stop_passes_ = 0;
    std::array<EngineAdapter::GlowPair, shroudfire::kMaxPlayers + 1> armitive_glows_ {};
    std::array<EngineAdapter::GlowPair, shroudfire::kMaxPlayers + 1> ward_glows_ {};
    std::array<std::atomic<int>, shroudfire::kMaxPlayers + 1> glow_relay_indices_ {};
    std::array<std::atomic<int>, shroudfire::kMaxPlayers + 1> glow_prop_indices_ {};
    std::array<std::atomic<int>, shroudfire::kMaxPlayers + 1> ward_relay_indices_ {};
    std::array<std::atomic<int>, shroudfire::kMaxPlayers + 1> ward_prop_indices_ {};
    std::atomic<int> glow_count_ {0};
    std::atomic<uint64_t> glow_viewer_mask_ {0};
    std::atomic<uint64_t> ward_viewer_mask_ {0};
    std::atomic<uint64_t> shield_glow_mask_ {0};
    float last_glow_refresh_ = 0.0f;
    float last_mode_tick_ = 0.0f;
    float last_observer_check_ = 0.0f;
    float last_profile_save_ = 0.0f;
    std::chrono::steady_clock::time_point last_discord_message_ = std::chrono::steady_clock::now();
    float pending_initial_restart_ = 0.0f;
    bool solo_rejoin_freeze_pending_ = false;
    float solo_rejoin_freeze_until_ = 0.0f;
    int initial_restart_attempts_ = 0;
    struct MapRotation {
        std::string current;
        std::string override_map;
        std::string pending;
        int rounds = 0;
        double played_seconds = 0.0;
        float last_time = 0.0f;
        float change_at = 0.0f;
        float retry_at = 0.0f;
        bool limit_announced = false;
        bool unavailable_reported = false;
        bool changing = false;
    } map_rotation_;
    struct GameCvar {
        const char *name;
        float original = 0.0f;
        float enforced = 0.0f;
        bool integer = true;
        bool available = false;
        bool round_time = false;
        bool minimum = false;
    };
    std::array<GameCvar, 28> game_cvars_ {{
        {"mp_autoteambalance", 0, 0, true},
        {"mp_autokick", 0, 0, true},
        {"mp_suicide_penalty", 0, 0, true},
        {"mp_limitteams", 0, 0, true},
        {"mp_friendlyfire", 0, 0, true},
        {"mp_solid_teammates", 0, 1, true},
        {"mp_forcecamera", 0, 1, true},
        {"mp_ignore_round_win_conditions", 0, 1, true},
        // CS2 can end the match on reset when both native limits are zero.
        // Keep its round ceiling above normal play; Shroudfire owns rotation.
        {"mp_maxrounds", 0, 999999, true},
        {"mp_timelimit", 0, 0, false},
        {"mp_winlimit", 0, 0, true},
        {"mp_match_can_clinch", 0, 0, true},
        {"mp_warmup_online_enabled", 0, 0, true},
        {"sv_warmup_to_freezetime_delay", 0, 0, false},
        {"mp_give_player_c4", 0, 0, true},
        {"mp_buytime", 0, 20, true},
        {"mp_buy_anywhere", 0, 0, true},
        {"sv_alltalk", 0, 1, true},
        {"sv_deadtalk", 0, 1, true},
        {"sv_full_alltalk", 0, 1, true},
        {"sv_voiceenable", 0, 1, true},
        {"sv_allchat", 0, 1, true},
        {"sv_talk_enemy_living", 0, 1, true},
        {"sv_talk_enemy_dead", 0, 1, true},
        {"sv_maxspeed", 0, 300, false, false, false, true},
        {"mp_roundtime", 0, 0, false, false, true},
        {"mp_roundtime_defuse", 0, 0, false, false, true},
        {"mp_roundtime_hostage", 0, 0, false, false, true}
    }};
    struct LoadoutCvar {
        const char *name;
        std::string original;
        bool available = false;
    };
    std::array<LoadoutCvar, 4> loadout_cvars_ {{
        {"mp_t_default_primary"}, {"mp_t_default_secondary"},
        {"mp_ct_default_primary"}, {"mp_ct_default_secondary"}
    }};
    bool game_cvars_overridden_ = false;
    CVValue_t warmup_delay_zero_minimum_ {0.0f};
    CVValue_t *warmup_delay_original_minimum_ = nullptr;
    bool objectives_removed_ = false;
    int spawn_config_ = -1;
    bool tonemap_reported_ = false;
    std::unique_ptr<KHook::Function<int64_t, void *, void *, void *>> damage_hook_;
    std::unique_ptr<KHook::Function<bool, void *, void *>> weapon_can_use_hook_;
    std::unique_ptr<KHook::Function<int, void *, void *, int, uint64_t>> can_acquire_hook_;
    shroudfire::WeaponRestrictions armitive_weapon_restrictions_;
    std::unique_ptr<KHook::Function<SoundEventResult, IRecipientFilter &, int,
                                    const void *>> sound_filter_hook_;
    std::unique_ptr<KHook::Function<void, void *, void *>> process_movement_hook_;
    std::unique_ptr<KHook::Function<void, void *, void *>> player_run_command_hook_;
    void *player_run_command_target_ = nullptr;
    std::unique_ptr<KHook::Function<void, void *, void *>> observer_run_command_hook_;
    void *observer_run_command_target_ = nullptr;
    std::unique_ptr<KHook::Function<void, IGameEventManager2 *>> game_event_init_hook_;
    std::unique_ptr<KHook::Function<void, CEntityInstance *, CCommand &, bool, int,
                                    const char *>> host_say_hook_;
    ChatRouting chat_routing_;
    bool chat_split_reported_ = false;
    std::mt19937 random_engine_ {std::random_device{}()};
    struct PendingEngineAction {
        shroudfire::Action action;
        float due = 0.0f;
        uint64_t steam_id = 0;
    };
    std::vector<PendingEngineAction> pending_engine_actions_;
    struct PendingSpawn {
        int slot = 0;
        uint64_t steam_id = 0;
        float due = 0.0f;
    };
    std::vector<PendingSpawn> pending_spawns_;
    std::vector<PendingSpawn> pending_fire_acceleration_;
    struct PendingObserve {
        int viewer = 0;
        int target = 0;
        uint64_t viewer_steam = 0;
        uint64_t target_steam = 0;
        float due = 0.0f;
        float expires_at = 0.0f;
    };
    std::vector<PendingObserve> pending_observers_;
    std::atomic<uint64_t> invisant_transmit_mask_ {0};
    uint64_t timeout_reveal_ready_mask_ = 0;
    std::array<int, shroudfire::kMaxPlayers + 1> timeout_reveal_last_alpha_ {};
    std::atomic<uint64_t> invisant_viewer_mask_ {0};
    std::atomic<int> invisant_transmit_team_ {3};
    std::atomic<bool> teammate_transmit_view_ {true};

    KHook::Virtual<IServerGameDLL, void, bool, bool, bool> game_frame_;
    KHook::Virtual<IGameEventSystem, void, CSplitScreenSlot, bool, int, const uint64 *,
        INetworkMessageInternal *, const CNetMessage *, unsigned long, NetChannelBufType_t>
        post_sound_event_;
    KHook::Virtual<IGameEventManager2, bool, IGameEvent *, bool> game_event_fire_;
    KHook::Virtual<IServerGameClients, void, CPlayerSlot, bool, const char *, uint64> client_active_;
    KHook::Virtual<IServerGameClients, void, CPlayerSlot, ENetworkDisconnectionReason,
                   const char *, uint64, const char *> client_disconnect_;
    KHook::Virtual<IServerGameClients, void, CPlayerSlot, const char *, uint64,
                   const char *, const char *, bool> client_connected_;
    KHook::Virtual<IServerGameClients, void, CPlayerSlot, const CCommand &> client_command_;
    KHook::Virtual<ICvar, void, ConCommandRef, const CCommandContext &,
        const CCommand &> cvar_command_;
    KHook::Virtual<ISource2GameEntities, void, CCheckTransmitInfo **, int,
        CBitVec<MAX_EDICTS> &, CBitVec<MAX_EDICTS> &, const Entity2Networkable_t **,
        const uint16 *, int> check_transmit_;
};

extern ShroudfirePlugin g_shroudfire_plugin;
PLUGIN_GLOBALVARS();
