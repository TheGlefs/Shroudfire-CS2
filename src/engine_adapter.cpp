#include "engine_adapter.h"
#include "mode.h"
#include "knife_targeting.h"
#include "surge_velocity.h"

#include <const.h>
#include <in_buttons.h>
#include <bspflags.h>
#include <entity2/entityinstance.h>
#include <entity2/entitykeyvalues.h>
#include <entity2/entitysystem.h>
#include <schemasystem/schemasystem.h>
#include <engine/igameeventsystem.h>
#include <networksystem/inetworkmessages.h>
#include <tier1/utlsymbollarge.h>
#include <tier1/utlstring.h>
#include <tier1/utlvector.h>
#include <variant.h>
#include <gametrace.h>
#include <igamesystemfactory.h>
#include <igameevents.h>
#include <irecipientfilter.h>
#include <tier1/utlstringtoken.h>
#include "gameevents.pb.h"
#include "usermessages.pb.h"
#include "cstrike15_usermessages.pb.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#include <link.h>
#endif

#include <array>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <limits>

// CS2 passes this interface when it builds the map's client resource manifest.
// The layout follows the Source 2 game-system interface used by CS2 plugins.
class IEntityResourceManifest {
public:
    virtual void AddResource(const char *) = 0;
    virtual void AddResource(const char *, void *) = 0;
    virtual void AddResource(const char *, void *, void *, void *) = 0;
    virtual void Unknown04() = 0;
    virtual void Unknown05() = 0;
    virtual void Unknown06() = 0;
    virtual void Unknown07() = 0;
    virtual void Unknown08() = 0;
    virtual void Unknown09() = 0;
    virtual void Unknown10() = 0;
};

CBaseGameSystemFactory **CBaseGameSystemFactory::sm_pFirst = nullptr;

namespace {
class ShroudfireResourceGameSystem final : public CAutoGameSystem {
public:
    DECLARE_GAME_SYSTEM();
    void Shutdown() override;
    GS_EVENT(BuildGameSessionManifest) {
        if (!msg || !msg->m_pResourceManifest) return;
        msg->m_pResourceManifest->AddResource(
            "particles/blood_impact/blood_impact_mist1.vpcf");
        std::fprintf(stderr, "[Shroudfire] Blood sprite resources added to map manifest.\n");
    }
};
ShroudfireResourceGameSystem resource_game_system;
CGameSystemStaticFactory<ShroudfireResourceGameSystem> *resource_factory = nullptr;

void ShroudfireResourceGameSystem::Shutdown() {
    delete resource_factory;
    resource_factory = nullptr;
}
}
#include <fstream>
#include <tuple>

namespace {

struct StateChangeInfo {
    int size = 0;
    int padding = 0;
    CUtlVector<uint32_t> offsets;
    const char *field_name = nullptr;
    const char *file_name = nullptr;
    uint32_t unknown = 0xffffffffu;
    uint32_t array_index = 0xffffffffu;
    uint32_t path_index = 0xffffffffu;
    uint16_t tail = 0;

    explicit StateChangeInfo(uint32_t &offset) : offsets(&offset, 1, 1) {}
};

static_assert(sizeof(StateChangeInfo) == 64);
static_assert(offsetof(StateChangeInfo, offsets) == 8);

} // namespace

CEntityIdentity *CEntitySystem::GetEntityIdentity(CEntityIndex index) {
    if (index.Get() < 0 || index.Get() >= MAX_TOTAL_ENTITIES - 1) return nullptr;
    CEntityIdentity *chunk = m_EntityList.m_pIdentityChunks[index.Get() / MAX_ENTITIES_IN_LIST];
    if (!chunk) return nullptr;
    CEntityIdentity *identity = &chunk[index.Get() % MAX_ENTITIES_IN_LIST];
    return identity->GetEntityIndex() == index ? identity : nullptr;
}

CEntityIdentity *CEntitySystem::GetEntityIdentity(const CEntityHandle &handle) {
    if (!handle.IsValid()) return nullptr;
    const int index = handle.GetEntryIndex();
    if (index < 0 || index >= MAX_TOTAL_ENTITIES - 1) return nullptr;
    CEntityIdentity *chunk = m_EntityList.m_pIdentityChunks[index / MAX_ENTITIES_IN_LIST];
    if (!chunk) return nullptr;
    CEntityIdentity *identity = &chunk[index % MAX_ENTITIES_IN_LIST];
    return identity->GetRefEHandle() == handle ? identity : nullptr;
}

namespace {

// Private CS2 calls stay disabled after a game update until these builds are verified.
#ifdef _WIN32
// Windows CS2 1.41.9.2 (Steam build 26144732); native signatures and vtables reviewed.
constexpr std::uintmax_t kServerSize = 34687128;
constexpr uint32_t kServerCrc32 = 0x42261a2c;
constexpr size_t kEntitySystemOffset = 88;
constexpr size_t kChangeTeamVtable = 105;
constexpr size_t kRespawnVtable = 274;
constexpr size_t kDropWeaponVtable = 28;
constexpr size_t kSetStateChangedVtable = 28;
constexpr size_t kCommitSuicideVtable = 387;
constexpr size_t kTeleportVtable = 165;
constexpr size_t kNavPhysicsVtableRva = 0x1a1ce10;
constexpr size_t kTraceShapeFilterVtable = 3;
constexpr size_t kPointContentsVtable = 5;
#else
// Linux CS2 1.41.9.2 (Steam build 26144732); native signatures and vtables reviewed.
constexpr std::uintmax_t kServerSize = 41804088;
constexpr uint32_t kServerCrc32 = 0x86bdf70a;
constexpr size_t kEntitySystemOffset = 80;
constexpr size_t kChangeTeamVtable = 104;
constexpr size_t kRespawnVtable = 276;
constexpr size_t kDropWeaponVtable = 29;
constexpr size_t kSetStateChangedVtable = 29;
constexpr size_t kCommitSuicideVtable = 387;
constexpr size_t kTeleportVtable = 164;
constexpr size_t kNavPhysicsVtableRva = 0x26d2e20;
constexpr size_t kTraceShapeFilterVtable = 5;
constexpr size_t kPointContentsVtable = 6;
#endif
// Verified against the fingerprinted modules: the CTraceFilter shape overload
// is slot 3 on Windows and 5 on Linux; point contents is slot 5 and 6 respectively.

bool schema_field(ISchemaSystem *schema, const char *class_name,
                  const char *field_name, uint32_t &offset) {
#ifdef _WIN32
    CSchemaSystemTypeScope *scope = schema->FindTypeScopeForModule("server.dll");
#else
    CSchemaSystemTypeScope *scope = schema->FindTypeScopeForModule("libserver.so");
#endif
    SchemaClassInfoData_t *info = scope ? scope->FindDeclaredClass(class_name).Get() : nullptr;
    if (!info) return false;
    for (int i = 0; i < info->m_nFieldCount; ++i) {
        if (std::strcmp(info->m_pFields[i].m_pszName, field_name) == 0) {
            offset = info->m_pFields[i].m_nSingleInheritanceOffset;
            return true;
        }
    }
    return false;
}

bool global_schema_field(ISchemaSystem *schema, const char *class_name,
                         const char *field_name, uint32_t &offset) {
    CSchemaSystemTypeScope *scope = schema->GlobalTypeScope();
    SchemaClassInfoData_t *info = scope ? scope->FindDeclaredClass(class_name).Get() : nullptr;
    if (!info) return false;
    for (int i = 0; i < info->m_nFieldCount; ++i) {
        if (std::strcmp(info->m_pFields[i].m_pszName, field_name) == 0) {
            offset = info->m_pFields[i].m_nSingleInheritanceOffset;
            return true;
        }
    }
    return false;
}

uint32_t crc32_file(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return 0;
    uint32_t crc = 0xffffffffu;
    std::array<char, 65536> bytes {};
    while (file) {
        file.read(bytes.data(), bytes.size());
        const auto count = file.gcount();
        for (std::streamsize i = 0; i < count; ++i) {
            crc ^= static_cast<unsigned char>(bytes[static_cast<size_t>(i)]);
            for (int bit = 0; bit < 8; ++bit)
                crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
        }
    }
    return ~crc;
}

#ifdef _WIN32
bool executable_in_module(void *function, void *module) {
    MEMORY_BASIC_INFORMATION region {};
    if (!function || !VirtualQuery(function, &region, sizeof(region)) ||
        region.AllocationBase != module || region.State != MEM_COMMIT) return false;
    const DWORD protection = region.Protect & 0xff;
    return protection == PAGE_EXECUTE || protection == PAGE_EXECUTE_READ ||
        protection == PAGE_EXECUTE_READWRITE || protection == PAGE_EXECUTE_WRITECOPY;
}

template <size_t N>
void *find_signature(HMODULE module, const std::array<int, N> &pattern) {
    const auto *image = reinterpret_cast<const uint8_t *>(module);
    const auto *dos = reinterpret_cast<const IMAGE_DOS_HEADER *>(image);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) return nullptr;
    const auto *nt = reinterpret_cast<const IMAGE_NT_HEADERS *>(image + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return nullptr;
    const uint8_t *match = nullptr;
    const IMAGE_SECTION_HEADER *section = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (!(section[i].Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;
        const auto *start = image + section[i].VirtualAddress;
        const size_t length = section[i].Misc.VirtualSize;
        for (size_t position = 0; position + pattern.size() <= length; ++position) {
            bool equal = true;
            for (size_t byte = 0; byte < pattern.size(); ++byte) {
                if (pattern[byte] >= 0 && start[position + byte] != pattern[byte]) {
                    equal = false;
                    break;
                }
            }
            if (!equal) continue;
            if (match) return nullptr;
            match = start + position;
        }
    }
    return const_cast<uint8_t *>(match);
}
#elif defined(__linux__)
bool executable_in_module(void *function, void *module) {
    Dl_info info {};
    return function && dladdr(function, &info) && info.dli_fbase == module;
}

template <size_t N>
void *find_signature(void *module, const std::array<int, N> &pattern) {
    struct Search {
        void *module;
        const std::array<int, N> &pattern;
        const uint8_t *match = nullptr;
        bool ambiguous = false;
    } search {module, pattern};
    dl_iterate_phdr([](dl_phdr_info *image, size_t, void *context) {
        auto &state = *static_cast<Search *>(context);
        if (reinterpret_cast<void *>(image->dlpi_addr) != state.module) return 0;
        for (int index = 0; index < image->dlpi_phnum; ++index) {
            const auto &segment = image->dlpi_phdr[index];
            if (segment.p_type != PT_LOAD || !(segment.p_flags & PF_X)) continue;
            const auto *start = reinterpret_cast<const uint8_t *>(image->dlpi_addr + segment.p_vaddr);
            const auto length = static_cast<size_t>(segment.p_memsz);
            for (size_t offset = 0; offset + N <= length; ++offset) {
                size_t byte = 0;
                for (; byte < N; ++byte)
                    if (state.pattern[byte] >= 0 && start[offset + byte] != state.pattern[byte]) break;
                if (byte != N) continue;
                if (state.match) {
                    state.ambiguous = true;
                    return 1;
                }
                state.match = start + offset;
            }
        }
        return 1;
    }, &search);
    return search.ambiguous ? nullptr : const_cast<uint8_t *>(search.match);
}
#endif

} // namespace

bool EngineAdapter::initialize(IGameResourceService *resources, ISchemaSystem *schema,
                               const void *server_interface, INetworkMessages *network_messages,
                               IGameEventSystem *event_system,
                               std::string &error) {
    ready_ = false;
    resources_ = nullptr;
    server_module_ = nullptr;
    terminate_round_function_ = nullptr;
    remove_entity_function_ = nullptr;
    create_entity_function_ = nullptr;
    dispatch_spawn_function_ = nullptr;
    take_damage_function_ = nullptr;
    weapon_can_use_function_ = nullptr;
    can_acquire_function_ = nullptr;
    sound_filter_function_ = nullptr;
    process_movement_function_ = nullptr;
    ignore_armor_damage_flag_ = 0;
    set_player_pawn_function_ = nullptr;
    switch_team_function_ = nullptr;
    game_event_manager_init_function_ = nullptr;
    accept_input_function_ = nullptr;
    fade_fields_ok_ = false;
    blood_fields_ok_ = false;
    particle_fields_ok_ = false;
    point_hurt_fields_ok_ = false;
    glow_fields_ok_ = false;
    keyvalues_constructor_function_ = nullptr;
    keyvalues_find_or_create_function_ = nullptr;
    keyvalues_set_string_function_ = nullptr;
    hud_set_player_class_function_ = nullptr;
    hud_set_player_dialog_function_ = nullptr;
    win_layout_handle_ = 0;
    spawn_rotation_ok_ = false;
    html_restart_fields_ok_ = false;
    buy_rules_fields_ok_ = false;
    html_game_rules_index_ = -1;
    fade_entity_ = 0;
    blood_entity_ = 0;
    blood_particles_.clear();
    claw_hurt_entity_ = 0;
    extra_spawn_handles_.clear();
    base_spawn_counts_ = {};
#ifdef _WIN32
    HMODULE module = nullptr;
    const void *server_method = server_interface
        ? (*reinterpret_cast<void *const *const *>(server_interface))[0] : nullptr;
    if (!server_method || !GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(server_method), &module)) {
        error = "Cannot resolve the CS2 game server module";
        return false;
    }
    std::array<wchar_t, 32768> path {};
    const DWORD length = GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size()));
    if (!length || length >= path.size()) {
        error = "Cannot find the loaded CS2 server.dll";
        return false;
    }
    std::error_code filesystem_error;
    const std::filesystem::path dll_path(path.data());
    const auto size = std::filesystem::file_size(dll_path, filesystem_error);
    const auto crc = filesystem_error ? 0 : crc32_file(dll_path);
    if (filesystem_error || size != kServerSize || crc != kServerCrc32) {
        error = "Unsupported server module " + dll_path.string() + " (size=" +
            std::to_string(size) + ", CRC32=" + std::to_string(crc) +
            "); engine actions are disabled";
        return false;
    }
#elif defined(__linux__)
    const void *server_method = server_interface
        ? (*reinterpret_cast<void *const *const *>(server_interface))[0] : nullptr;
    Dl_info module_info {};
    if (!server_method || !dladdr(server_method, &module_info) ||
        !module_info.dli_fbase || !module_info.dli_fname) {
        error = "Cannot resolve the CS2 Linux game server module";
        return false;
    }
    void *module = module_info.dli_fbase;
    std::error_code filesystem_error;
    const std::filesystem::path dll_path(module_info.dli_fname);
    const auto size = std::filesystem::file_size(dll_path, filesystem_error);
    const auto crc = filesystem_error ? 0 : crc32_file(dll_path);
    if (filesystem_error || size != kServerSize || crc != kServerCrc32) {
        error = "Unsupported server module " + dll_path.string() + " (size=" +
            std::to_string(size) + ", CRC32=" + std::to_string(crc) +
            "); engine actions are disabled";
        return false;
    }
#endif
    if (!resources || !schema) {
        error = "Required CS2 game resource or schema interface is unavailable";
        return false;
    }
    for (CSchemaSystemTypeScope *scope : {schema->GlobalTypeScope(),
#ifdef _WIN32
             schema->FindTypeScopeForModule("server.dll")
#else
             schema->FindTypeScopeForModule("libserver.so")
#endif
         }) {
        CSchemaEnumInfo *flags = scope
            ? scope->FindDeclaredEnum("TakeDamageFlags_t").Get() : nullptr;
        if (!flags) continue;
        for (int i = 0; i < flags->m_nEnumeratorCount; ++i) {
            const auto &entry = flags->m_pEnumerators[i];
            if (std::strcmp(entry.m_pszName, "DFLAG_IGNORE_ARMOR") == 0 &&
                entry.m_nValue > 0 && entry.m_nValue <= UINT32_MAX)
                ignore_armor_damage_flag_ = static_cast<uint32_t>(entry.m_nValue);
        }
    }
    if (!ignore_armor_damage_flag_) {
        error = "CS2 schema has no DFLAG_IGNORE_ARMOR value";
        return false;
    }
    bool fields_ok = true;
    fields_ok &= schema_field(schema, "CCSPlayerController", "m_hPlayerPawn", fields_.player_pawn);
    fields_ok &= schema_field(schema, "CCSPlayerController", "m_pInGameMoneyServices",
                              fields_.money_services);
    fields_ok &= schema_field(schema, "CCSPlayerController_InGameMoneyServices", "m_iAccount",
                              fields_.money_account);
    fields_ok &= schema_field(schema, "CBasePlayerPawn", "m_hController", fields_.pawn_controller);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_hOwnerEntity", fields_.owner_entity);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_iHealth", fields_.health);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_fFlags", fields_.flags);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_lifeState", fields_.life_state);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_MoveType", fields_.move_type);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_iTeamNum", fields_.team);
    fields_ok &= schema_field(schema, "CBasePlayerPawn", "m_pWeaponServices", fields_.weapon_services);
    fields_ok &= schema_field(schema, "CCSPlayer_WeaponServices", "m_flNextAttack",
                              fields_.next_attack_time);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_pAimPunchServices",
                              fields_.aim_punch_services);
    fields_ok &= schema_field(schema, "CCSPlayer_AimPunchServices", "m_predictableBaseAngle",
                              fields_.aim_punch_angle);
    fields_ok &= schema_field(schema, "CCSPlayer_AimPunchServices", "m_predictableBaseAngleVel",
                              fields_.aim_punch_velocity);
    fields_ok &= schema_field(schema, "CBasePlayerPawn", "m_pItemServices", fields_.item_services);
    fields_ok &= schema_field(schema, "CEconItemView", "m_iItemDefinitionIndex", fields_.item_definition_index);
    fields_ok &= schema_field(schema, "CCSPlayer_ItemServices", "m_bHasHelmet", fields_.has_helmet);
    fields_ok &= schema_field(schema, "CCSPlayer_ItemServices", "m_bHasDefuser", fields_.has_defuser);
    fields_ok &= schema_field(schema, "CPlayer_WeaponServices", "m_hActiveWeapon", fields_.active_weapon);
    fields_ok &= schema_field(schema, "CPlayer_WeaponServices", "m_hLastWeapon", fields_.last_weapon);
    fields_ok &= schema_field(schema, "CPlayer_WeaponServices", "m_hMyWeapons", fields_.weapons);
    fields_ok &= schema_field(schema, "CBaseCombatCharacter", "m_hMyWearables", fields_.wearables);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_pHostageServices", fields_.hostage_services);
    fields_ok &= schema_field(schema, "CCSPlayer_HostageServices", "m_hCarriedHostageProp",
                              fields_.carried_hostage_prop);
    fields_ok &= schema_field(schema, "CCSGameRulesProxy", "m_pGameRules", fields_.game_rules);
    warmup_field_ok_ = schema_field(schema, "CCSGameRules", "m_bWarmupPeriod",
                                    fields_.warmup_period);
    freeze_field_ok_ = schema_field(schema, "CCSGameRules", "m_bFreezePeriod",
                                     fields_.freeze_period);
    html_restart_fields_ok_ = schema_field(schema, "CCSGameRules", "m_flRestartRoundTime",
                                           fields_.restart_round_time);
    html_restart_fields_ok_ &= schema_field(schema, "CCSGameRules", "m_bGameRestart",
                                            fields_.game_restart);
    round_end_count_field_ok_ = schema_field(schema, "CCSGameRules", "m_nRoundEndCount",
                                              fields_.round_end_count);
    buy_rules_fields_ok_ = schema_field(schema, "CCSGameRules", "m_bBuyTimeEnded",
                                       fields_.buy_time_ended);
    buy_rules_fields_ok_ &= schema_field(schema, "CCSGameRules", "m_bCTCantBuy",
                                        fields_.ct_cant_buy);
    buy_rules_fields_ok_ &= schema_field(schema, "CCSGameRules", "m_bTCantBuy",
                                        fields_.t_cant_buy);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_ArmorValue", fields_.armor);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_flGravityScale", fields_.gravity_scale);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_flActualGravityScale",
                              fields_.actual_gravity_scale);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_CBodyComponent", fields_.body_component);
    fields_ok &= schema_field(schema, "CBodyComponent", "m_pSceneNode", fields_.scene_node);
    fields_ok &= schema_field(schema, "CGameSceneNode", "m_vecAbsOrigin", fields_.abs_origin);
    spawn_rotation_ok_ = schema_field(schema, "CGameSceneNode", "m_angAbsRotation",
                                      fields_.abs_rotation);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_angEyeAngles", fields_.eye_angles);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_bInBuyZone", fields_.in_buy_zone);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_bWasInBuyZone", fields_.was_in_buy_zone);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_TouchingBuyZones",
                               fields_.touching_buy_zones);
    fields_ok &= schema_field(schema, "CBasePlayerPawn", "m_pMovementServices", fields_.movement_services);
    fields_ok &= schema_field(schema, "CPlayer_MovementServices", "m_nButtons", fields_.movement_buttons);
    schema_field(schema, "CPlayer_MovementServices_Humanoid", "m_bDucked",
                 fields_.movement_ducked);
    schema_field(schema, "CCSPlayer_MovementServices", "m_flDuckAmount",
                 fields_.movement_duck_amount);
    fields_ok &= schema_field(schema, "CPlayer_MovementServices", "m_flMaxspeed", fields_.maxspeed);
    fields_ok &= schema_field(schema, "CCSPlayerPawn", "m_flVelocityModifier",
                              fields_.velocity_modifier);
    fields_ok &= schema_field(schema, "CBasePlayerPawn", "m_pObserverServices",
                              fields_.observer_services);
    fields_ok &= schema_field(schema, "CCSPlayerController", "m_hObserverPawn",
                              fields_.observer_pawn);
    fields_ok &= schema_field(schema, "CPlayer_ObserverServices", "m_iObserverMode",
                              fields_.observer_mode);
    fields_ok &= schema_field(schema, "CPlayer_ObserverServices", "m_hObserverTarget",
                              fields_.observer_target);
    fields_ok &= schema_field(schema, "CBasePlayerPawn", "m_pCameraServices",
                              fields_.camera_services);
    fields_ok &= schema_field(schema, "CPlayer_CameraServices", "m_hTonemapController",
                              fields_.player_tonemap);
    fields_ok &= global_schema_field(schema, "CInButtonState", "m_pButtonStates", fields_.button_states);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_vecVelocity", fields_.velocity);
    fields_ok &= schema_field(schema, "CNetworkVelocityVector", "m_vecX", fields_.velocity_x);
    fields_ok &= schema_field(schema, "CNetworkVelocityVector", "m_vecY", fields_.velocity_y);
    fields_ok &= schema_field(schema, "CNetworkVelocityVector", "m_vecZ", fields_.velocity_z);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_bTakesDamage", fields_.takes_damage);
    fields_ok &= schema_field(schema, "CBaseEntity", "m_nSubclassID", fields_.subclass_id);
    fields_ok &= schema_field(schema, "CBasePlayerWeapon", "m_pReserveAmmo", fields_.reserve_ammo);
    fields_ok &= schema_field(schema, "CBasePlayerWeapon", "m_iClip1", fields_.clip1);
    fields_ok &= schema_field(schema, "CBasePlayerWeapon", "m_nNextPrimaryAttackTick",
                              fields_.primary_attack_tick);
    fields_ok &= schema_field(schema, "CBasePlayerWeapon", "m_nNextSecondaryAttackTick",
                              fields_.secondary_attack_tick);
    fields_ok &= schema_field(schema, "CBasePlayerWeaponVData", "m_iMaxClip1",
                              fields_.primary_clip_max);
    fields_ok &= schema_field(schema, "CBasePlayerWeaponVData", "m_bReserveAmmoAsClips",
                              fields_.reserve_as_clips);
    fields_ok &= schema_field(schema, "CCSWeaponBaseVData", "m_bReloadsSingleShells",
                              fields_.reloads_single_shells);
    fields_ok &= schema_field(schema, "CCSWeaponBase", "m_bInReload", fields_.weapon_in_reload);
    fields_ok &= schema_field(schema, "CCSWeaponBase", "m_fAccuracyPenalty",
                              fields_.accuracy_penalty);
    fields_ok &= schema_field(schema, "CCSWeaponBaseVData", "m_nPrimaryReserveAmmoMax",
                              fields_.primary_reserve_max);
    fields_ok &= schema_field(schema, "CCSWeaponBaseVData", "m_GearSlot", fields_.gear_slot);
    fields_ok &= schema_field(schema, "CCSWeaponBaseVData", "m_flHeadshotMultiplier",
                              fields_.headshot_multiplier);
    fields_ok &= schema_field(schema, "CTakeDamageInfo", "m_hAttacker", fields_.damage_attacker);
    fields_ok &= schema_field(schema, "CTakeDamageInfo", "m_flDamage", fields_.damage_amount);
    fields_ok &= schema_field(schema, "CTakeDamageInfo", "m_bitsDamageType", fields_.damage_type);
    fields_ok &= schema_field(schema, "CTakeDamageInfo", "m_nDamageFlags", fields_.damage_flags);
    fields_ok &= schema_field(schema, "CTakeDamageInfo", "m_iHitGroupId", fields_.damage_hitgroup);
    point_hurt_fields_ok_ = schema_field(schema, "CPointHurt", "m_nDamage",
                                          fields_.point_hurt_damage);
    point_hurt_fields_ok_ &= schema_field(schema, "CPointHurt", "m_bitsDamageType",
                                          fields_.point_hurt_type);
    point_hurt_fields_ok_ &= schema_field(schema, "CPointHurt", "m_flRadius",
                                          fields_.point_hurt_radius);
    point_hurt_fields_ok_ &= schema_field(schema, "CPointHurt", "m_strTarget",
                                          fields_.point_hurt_target);
    glow_fields_ok_ = schema_field(schema, "CSkeletonInstance", "m_modelState",
                                    fields_.model_state);
    glow_fields_ok_ &= schema_field(schema, "CModelState", "m_ModelName",
                                    fields_.model_name) ||
                       global_schema_field(schema, "CModelState", "m_ModelName",
                                           fields_.model_name);
    render_fields_ok_ = schema_field(schema, "CBaseModelEntity", "m_nRenderMode",
                                    fields_.render_mode);
    render_fields_ok_ &= schema_field(schema, "CBaseModelEntity", "m_clrRender",
                                    fields_.render_color);
    glow_fields_ok_ &= render_fields_ok_;
    glow_fields_ok_ &= schema_field(schema, "CBaseModelEntity", "m_Glow",
                                    fields_.glow_property);
    glow_fields_ok_ &= schema_field(schema, "CGlowProperty", "m_glowColorOverride",
                                    fields_.glow_color);
    glow_fields_ok_ &= schema_field(schema, "CGlowProperty", "m_iGlowType",
                                    fields_.glow_type);
    glow_fields_ok_ &= schema_field(schema, "CGlowProperty", "m_iGlowTeam",
                                    fields_.glow_team);
    glow_fields_ok_ &= schema_field(schema, "CGlowProperty", "m_nGlowRange",
                                    fields_.glow_range);
    glow_fields_ok_ &= schema_field(schema, "CGlowProperty", "m_nGlowRangeMin",
                                    fields_.glow_range_min);
    fields_ok &= schema_field(schema, "CTonemapController2", "m_flAutoExposureMin",
                              fields_.auto_exposure_min);
    fields_ok &= schema_field(schema, "CTonemapController2", "m_flAutoExposureMax",
                              fields_.auto_exposure_max);
    fade_fields_ok_ = schema_field(schema, "CBaseEntity", "m_spawnflags",
                                   fields_.spawn_flags);
    fade_fields_ok_ &= schema_field(schema, "CEnvFade", "m_fadeColor",
                                    fields_.fade_color);
    fade_fields_ok_ &= schema_field(schema, "CEnvFade", "m_Duration",
                                    fields_.fade_duration);
    fade_fields_ok_ &= schema_field(schema, "CEnvFade", "m_HoldDuration",
                                    fields_.fade_hold_duration);
    blood_fields_ok_ = schema_field(schema, "CBlood", "m_flAmount", fields_.blood_amount);
    blood_fields_ok_ &= schema_field(schema, "CBlood", "m_Color", fields_.blood_color);
    blood_fields_ok_ &= schema_field(schema, "CBlood", "m_vecSprayDir",
                                     fields_.blood_spray_dir);
    particle_fields_ok_ = schema_field(schema, "CParticleSystem", "m_bStartActive",
                                       fields_.particle_start_active);
    particle_fields_ok_ &= schema_field(schema, "CParticleSystem", "m_iszEffectName",
                                        fields_.particle_effect_name);
    if (!fields_ok) {
        error = "Required CS2 player visibility schema fields are unavailable";
        return false;
    }
#ifdef _WIN32
    // These entry signatures are unique within the fingerprinted game DLL.
    terminate_round_function_ = find_signature(module, std::array<int, 14> {
        0x48, 0x8b, 0xc4, 0x4c, 0x89, 0x48, -1, 0x48, 0x89, 0x48, -1, 0x55, 0x41, 0x54
    });
    remove_entity_function_ = find_signature(module, std::array<int, 11> {
        0x48, 0x85, 0xc9, 0x74, -1, 0x48, 0x8b, 0xd1, 0x48, 0x8b, 0x0d
    });
    create_entity_function_ = find_signature(module, std::array<int, 12> {
        0x48, 0x83, 0xec, -1, 0xc6, 0x44, 0x24, -1, -1, 0x4c, 0x8b, 0xc1
    });
    dispatch_spawn_function_ = find_signature(module, std::array<int, 28> {
        0x48, 0x89, 0x5c, 0x24, -1, 0x57, 0x48, 0x83, 0xec, -1, 0x48, 0x8b, 0xda,
        0x48, 0x8b, 0xf9, 0x48, 0x85, 0xc9, 0x0f, 0x84, -1, -1, -1, -1, 0x48, 0x85, 0xd2
    });
    take_damage_function_ = find_signature(module, std::array<int, 22> {
        0x40, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x48, 0x8d, 0x6c, 0x24,
        -1, 0x48, 0x81, 0xec, -1, -1, -1, -1, 0x4d, 0x8b, 0xe0
    });
    weapon_can_use_function_ = find_signature(module, std::array<int, 28> {
        0x48, 0x89, 0x5c, 0x24, -1, 0x48, 0x89, 0x6c, 0x24, -1, 0x56, 0x57,
        0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x83, 0xec, -1, 0x48,
        0x8b, 0x01, 0x48, 0x8b, 0xfa
    });
    can_acquire_function_ = find_signature(module, std::array {
        0x44, 0x89, 0x44, 0x24, -1, 0x48, 0x89, 0x54, 0x24, -1,
        0x48, 0x89, 0x4c, 0x24, -1, 0x55, 0x53, 0x56, 0x57,
        0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8b, 0xec
    });
    sound_filter_function_ = find_signature(module, std::array<int, 17> {
        0x40, 0x53, 0x48, 0x83, 0xec, -1, 0x4c, 0x89, 0x4c, 0x24,
        -1, 0x48, 0x8b, 0xd9, 0x45, 0x8b, 0xc8
    });
    process_movement_function_ = find_signature(module, std::array<int, 14> {
        0x40, 0x57, 0x41, 0x57, 0x48, 0x81, 0xec, -1, -1, -1, -1,
        0x48, 0x83, 0x79
    });
    game_event_manager_init_function_ = find_signature(module, std::array<int, 18> {
        0x40, 0x53, 0x48, 0x83, 0xec, -1, 0x48, 0x8b, 0x01,
        0x48, 0x8b, 0xd9, 0xff, 0x50, -1, 0x48, 0x8b, 0x03
    });
    set_player_pawn_function_ = find_signature(module, std::array<int, 9> {
        0x44, 0x88, 0x4c, 0x24, -1, 0x53, 0x57, 0x41, 0x54
    });
    switch_team_function_ = find_signature(module, std::array<int, 15> {
        0x40, 0x53, 0x57, 0x48, 0x81, 0xec, -1, -1, -1, -1,
        0x48, 0x8b, 0xd9, 0x8b, 0xfa
    });
    host_say_function_ = find_signature(module, std::array<int, 9> {
        0x44, 0x89, 0x4c, 0x24, -1, 0x44, 0x88, 0x44, 0x24
    });
    accept_input_function_ = find_signature(module, std::array<int, 27> {
        0x48, 0x89, 0x5c, 0x24, -1, 0x48, 0x89, 0x6c, 0x24, -1, 0x56, 0x57,
        0x41, 0x56, 0x48, 0x81, 0xec, -1, -1, -1, -1, 0x4d, 0x8b, 0xf0,
        0x48, 0x8b, 0xf1
    });
    keyvalues_constructor_function_ = find_signature(module, std::array<int, 26> {
        0x48, 0x89, 0x5c, 0x24, -1, 0x57, 0x48, 0x83, 0xec, -1,
        0x33, 0xff, 0x48, 0x8b, 0xd9, 0x48, 0x89, 0x79, -1,
        0x48, 0x89, 0x79, -1, 0x44, 0x88, 0x41
    });
    keyvalues_find_or_create_function_ = find_signature(module, std::array<int, 9> {
        0x48, 0x83, 0xec, 0x38, 0x8b, 0x02, 0x45, 0x33, 0xc0
    });
    keyvalues_set_string_function_ = find_signature(module, std::array<int, 19> {
        0x48, 0x89, 0x5c, 0x24, 0x10, 0x48, 0x89, 0x6c, 0x24, 0x18,
        0x56, 0x48, 0x83, 0xec, -1, 0x48, 0x83, 0x39, -1
    });
#else
    // Byte patterns from CounterStrikeSharp gamedata, checked against the
    // fingerprinted libserver.so. Linux call ABIs still need live validation.
    terminate_round_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x57, 0x41, 0x56, 0x49, 0x89, 0xfe,
        0x41, 0x55, 0x41, 0x54, 0x53, 0x48, 0x81, 0xec, -1, -1, -1, -1,
        0x48, 0x8d, 0x05, -1, -1, -1, -1, 0xf3, 0x0f, 0x11, 0x85,
        -1, -1, -1, -1, 0x89, 0xb5
    });
    remove_entity_function_ = find_signature(module, std::array{
        0x48, 0x89, 0xfe, 0x48, 0x85, 0xff, 0x74, -1,
        0x48, 0x8d, 0x05, -1, -1, -1, -1, 0x48
    });
    create_entity_function_ = find_signature(module, std::array{
        0x48, 0x8d, 0x05, -1, -1, -1, -1, 0x55, 0x48, 0x89, 0xfa
    });
    dispatch_spawn_function_ = find_signature(module, std::array{
        0x48, 0x85, 0xff, 0x74, -1, 0x55, 0x48, 0x89, 0xe5,
        0x41, 0x55, 0x41, 0x54, 0x49, 0x89, 0xfc
    });
    take_damage_function_ = find_signature(module, std::array{
        0x55, 0x66, 0x0f, 0xef, 0xc0, 0x48, 0x89, 0xe5, 0x41, 0x57,
        0x41, 0x56, 0x41, 0x55, 0x49, 0x89, 0xfd, 0x31, 0xff
    });
    weapon_can_use_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x8d, 0x15, -1, -1, -1, -1, 0x48, 0x89, 0xe5,
        0x41, 0x55, 0x41, 0x54, 0x49, 0x89, 0xf4, 0x53, 0x48, 0x89, 0xfb,
        0x48, 0x83, 0xec, -1, 0x48, 0x8b, 0x07, 0x48, 0x8b, 0x80,
        -1, -1, -1, -1, 0x48, 0x39, 0xd0, 0x0f, 0x85,
        -1, -1, -1, -1, 0x80, 0xbf
    });
    can_acquire_function_ = find_signature(module, std::array {
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x57, 0x41, 0x56, 0x49, 0x89, 0xf6,
        0x41, 0x55, 0x41, 0x54, 0x49, 0x89, 0xcc, 0x53, 0x48, 0x89, 0xfb,
        0x48, 0x83, 0xec, 0x78
    });
    sound_filter_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x53, 0x48, 0x89, 0xfb, 0x48, 0x83, 0xec,
        -1, 0xe8, -1, -1, -1, -1, 0x48, 0x89, 0xd8, 0x48, 0x8b, 0x5d,
        -1, 0xc9, 0xc3, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x48, 0xb8
    });
    process_movement_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x57, 0x41, 0x56, 0x41, 0x55,
        0x49, 0x89, 0xf5, 0x41, 0x54, 0x53, 0x48, 0x89, 0xfb,
        0x48, 0x83, 0xec, -1, 0x48, 0x8b, 0x7f
    });
    game_event_manager_init_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x53, 0x48, 0x89, 0xfb, 0x48, 0x83,
        0xec, -1, 0x48, 0x8b, 0x07, 0xff, 0x50, -1, 0x48, 0x8b, 0x03
    });
    set_player_pawn_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x8d, 0x87, -1, -1, -1, -1, 0x48, 0x89, 0xe5,
        0x41, 0x57, 0x41, 0x56, 0x41, 0x89, 0xce, 0x41, 0x55, 0x45, 0x89, 0xcd
    });
    switch_team_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x54, 0x49, 0x89, 0xfc, 0x89, 0xf7
    });
    host_say_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x57, 0x49, 0x89, 0xf7,
        0x41, 0x56, 0x41, 0x55, 0x41, 0x89, 0xd5
    });
    accept_input_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x57, 0x49, 0x89, 0xff,
        0x41, 0x56, 0x48, 0x8d, 0xbd, -1, -1, -1, -1,
        0x41, 0x55, 0x4c, 0x8d, 0xad
    });
    // Verified unique against the fingerprinted Linux libserver.so. The HUD
    // setters store per-player state on custom_hud_layout's networked entity.
    keyvalues_constructor_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x53, 0x48, 0x89, 0xfb, 0x48, 0x89,
        0xf7, 0x48, 0x83, 0xec, -1, 0x48, 0x8d, 0x05
    });
    keyvalues_find_or_create_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x56, 0x41, 0x55, 0x41, 0x54,
        0x53, 0x48, 0x83, 0xec, -1, 0x8b, 0x06, 0x66, 0x83, 0x7f,
        -1, -1, 0x89, 0x45, -1, 0x8b, 0x46, -1, 0x89, 0x45, -1, 0x0f, 0x8f
    });
    keyvalues_set_string_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x56, 0x41, 0x55, 0x49, 0x89,
        0xd5, 0x41, 0x54, 0x49, 0x89, 0xf4, 0x53, 0x48, 0x89, 0xfb,
        0x48, 0x8b, 0x3f
    });
    hud_set_player_dialog_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x56, 0x41, 0x55, 0x41, 0x54,
        0x53, 0x89, 0xf3, 0x48, 0x83, 0xec, -1, 0x85, 0xf6, 0x78, -1,
        0x49, 0x89, 0xfd, 0x39, 0xb7, -1, -1, -1, -1, 0x7f, -1,
        0x48, 0x83, 0xc4, -1, 0x5b, 0x41, 0x5c, 0x41, 0x5d,
        0x41, 0x5e, 0x5d, 0xc3, -1, -1, -1, 0x48, 0x89, 0xd6,
        0x31, 0xd2, 0x49, 0x89, 0xce, 0x66, 0x89, 0x55, -1,
        0x48, 0x8d, 0x55, -1, 0x4d, 0x89, 0xc4
    });
    hud_set_player_class_function_ = find_signature(module, std::array{
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x56, 0x41, 0x55, 0x41, 0x54,
        0x53, 0x89, 0xf3, 0x48, 0x83, 0xec, -1, 0x85, 0xf6, 0x78, -1,
        0x49, 0x89, 0xfd, 0x39, 0xb7, -1, -1, -1, -1, 0x7f, -1,
        0x48, 0x83, 0xc4, -1, 0x5b, 0x41, 0x5c, 0x41, 0x5d,
        0x41, 0x5e, 0x5d, 0xc3, -1, -1, -1, 0x48, 0x89, 0xd6,
        0x31, 0xd2, 0x49, 0x89, 0xce, 0x66, 0x89, 0x55, -1,
        0x48, 0x8d, 0x55, -1, 0x45, 0x89, 0xc4, 0xe8
    });
#endif
    if (!terminate_round_function_ ||
        !remove_entity_function_ || !take_damage_function_ || !weapon_can_use_function_ || !can_acquire_function_ ||
        !create_entity_function_ || !dispatch_spawn_function_ ||
        !game_event_manager_init_function_ || !set_player_pawn_function_ ||
        !switch_team_function_ || !host_say_function_) {
        error = "Required CS2 gameplay functions are unavailable or ambiguous";
        return false;
    }
    resources_ = resources;
    network_messages_ = network_messages;
    event_system_ = event_system;
    server_module_ = module;
#ifdef _WIN32
    auto *factory_reference = static_cast<uint8_t *>(find_signature(module,
        std::array<int, 17> {0x48, 0x8b, 0x1d, -1, -1, -1, -1, 0x48,
                             0x85, 0xdb, 0x0f, 0x84, -1, -1, -1, -1, 0xbd}));
    if (factory_reference && !resource_factory) {
        int32_t relative = 0;
        std::memcpy(&relative, factory_reference + 3, sizeof(relative));
        auto **head = reinterpret_cast<CBaseGameSystemFactory **>(
            factory_reference + 7 + relative);
        MEMORY_BASIC_INFORMATION region {};
        if (VirtualQuery(head, &region, sizeof(region)) &&
            region.AllocationBase == module && region.State == MEM_COMMIT) {
            CBaseGameSystemFactory::sm_pFirst = head;
            resource_factory = new CGameSystemStaticFactory<ShroudfireResourceGameSystem>(
                "ShroudfireResourceGameSystem", &resource_game_system);
            std::fprintf(stderr, "[Shroudfire] Blood resource game system registered.\n");
        }
    }
#elif defined(__linux__)
    auto *factory_reference = static_cast<uint8_t *>(find_signature(module,
        std::array<int, 13> {0x4c, 0x8b, 0x35, -1, -1, -1, -1,
                             0x4d, 0x85, 0xf6, 0x75, -1, 0xe9}));
    if (factory_reference && !resource_factory) {
        int32_t relative = 0;
        std::memcpy(&relative, factory_reference + 3, sizeof(relative));
        auto **head = reinterpret_cast<CBaseGameSystemFactory **>(
            factory_reference + 7 + relative);
        Dl_info region {};
        if (dladdr(head, &region) && region.dli_fbase == module) {
            CBaseGameSystemFactory::sm_pFirst = head;
            resource_factory = new CGameSystemStaticFactory<ShroudfireResourceGameSystem>(
                "ShroudfireResourceGameSystem", &resource_game_system);
            std::fprintf(stderr, "[Shroudfire] Blood resource game system registered.\n");
        }
    }
#endif
    ready_ = true;
    return true;
}

CEntityInstance *EngineAdapter::controller(int slot) const {
    if (!ready_ || !resources_ || slot < 1 || slot > 64) return nullptr;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    return system ? system->GetEntityInstance(CEntityIndex(slot)) : nullptr;
}

CEntityInstance *EngineAdapter::pawn(int slot) const {
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *owner = controller(slot);
    if (!system || !owner) return nullptr;
    const auto &pawn_handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(owner) + fields_.player_pawn);
    CEntityInstance *entity = system->GetEntityInstance(pawn_handle);
    if (!entity || !entity->m_pEntity) return nullptr;
    const auto &owner_handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(entity) + fields_.pawn_controller);
    return system->GetEntityInstance(owner_handle) == owner ? entity : nullptr;
}

int EngineAdapter::sound_source_slot(int entity_index) const {
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system || entity_index <= 0 || entity_index >= MAX_TOTAL_ENTITIES) return 0;
    CEntityInstance *source = system->GetEntityInstance(CEntityIndex(entity_index));
    for (int depth = 0; source && source->m_pEntity && depth < 3; ++depth) {
        for (int slot = 1; slot <= 64; ++slot)
            if (source == controller(slot) || source == pawn(slot)) return slot;
        const auto &owner = *reinterpret_cast<const CEntityHandle *>(
            reinterpret_cast<const char *>(source) + fields_.owner_entity);
        CEntityInstance *next = system->GetEntityInstance(owner);
        if (next == source) break;
        source = next;
    }
    return 0;
}

void *EngineAdapter::entity_system() const {
    if (!ready_ || !resources_) return nullptr;
    CGameEntitySystem *system = nullptr;
    std::memcpy(&system, reinterpret_cast<const char *>(resources_) + kEntitySystemOffset,
                sizeof(system));
    return system;
}

bool EngineAdapter::collect_visual_group(int slot, int expected_team,
                                         std::array<int, kMaxVisualEntities> &indices,
                                         size_t &count) const {
    count = 0;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *pawn = this->pawn(slot);
    if (!system || !pawn) return false;
    if (*reinterpret_cast<const int *>(reinterpret_cast<const char *>(pawn) + fields_.health) <= 0 ||
        *reinterpret_cast<const uint8_t *>(reinterpret_cast<const char *>(pawn) + fields_.life_state) != 0 ||
        *reinterpret_cast<const uint8_t *>(reinterpret_cast<const char *>(pawn) + fields_.team) !=
            expected_team) return false;

    const auto append = [&](CEntityHandle handle) {
        if (!handle.IsValid()) return true;
        CEntityInstance *entity = system->GetEntityInstance(handle);
        // Weapon handles can outlive a dropped or removed weapon for a frame.
        // That stale entry must not make the entire Invisant pawn visible.
        if (!entity || !entity->m_pEntity) return true;
        const int index = entity->m_pEntity->m_EHandle.GetEntryIndex();
        if (index <= 0 || index >= MAX_EDICTS) return true;
        for (size_t i = 0; i < count; ++i) if (indices[i] == index) return true;
        if (count >= indices.size()) return false;
        indices[count++] = index;
        return true;
    };
    const auto append_handles = [&](const void *base, uint32_t offset, int maximum) {
        const auto *handles = reinterpret_cast<const CUtlVector<CEntityHandle> *>(
            reinterpret_cast<const char *>(base) + offset);
        const int entries = handles->Count();
        if (entries < 0 || entries > maximum) return false;
        for (int i = 0; i < entries; ++i)
            if (!append((*handles)[i])) return false;
        return true;
    };
    if (!append(pawn->m_pEntity->GetRefEHandle())) return false;
    void *weapons = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(pawn) + fields_.weapon_services);
    if (!weapons || !append(*reinterpret_cast<CEntityHandle *>(
            reinterpret_cast<char *>(weapons) + fields_.active_weapon)) ||
        !append(*reinterpret_cast<CEntityHandle *>(
            reinterpret_cast<char *>(weapons) + fields_.last_weapon)) ||
        !append_handles(weapons, fields_.weapons, 64) ||
        !append_handles(pawn, fields_.wearables, 32)) return false;
    void *hostage = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(pawn) + fields_.hostage_services);
    if (hostage && !append(*reinterpret_cast<CEntityHandle *>(
            reinterpret_cast<char *>(hostage) + fields_.carried_hostage_prop))) return false;
    return count > 0;
}

bool EngineAdapter::set_reveal_alpha(int slot, int expected_team, int alpha) {
    if (!ready_ || !render_fields_ok_) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    std::array<int, kMaxVisualEntities> indices {};
    size_t count = 0;
    if (!system || !collect_visual_group(slot, expected_team, indices, count)) return false;
    for (size_t index = 0; index < count; ++index) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(indices[index]));
        if (!entity || !entity->m_pEntity) return false;
        auto *base = reinterpret_cast<char *>(entity);
        auto *mode = reinterpret_cast<uint8_t *>(base + fields_.render_mode);
        auto *color = reinterpret_cast<uint8_t *>(base + fields_.render_color);
        const uint32_t handle = entity->m_pEntity->GetRefEHandle().ToInt();
        auto saved = original_reveal_rendering_.find(handle);
        if (saved == original_reveal_rendering_.end()) {
            RevealRenderState state;
            state.mode = *mode;
            std::memcpy(state.color.data(), color, state.color.size());
            saved = original_reveal_rendering_.emplace(handle, state).first;
        }
        const uint8_t next_mode = alpha >= 255 ? saved->second.mode : kRenderTransAlpha;
        const uint8_t next_alpha = static_cast<uint8_t>(
            saved->second.color[3] * std::clamp(alpha, 0, 255) / 255);
        if (*mode != next_mode) {
            if (!mark_state_changed(entity, fields_.render_mode)) return false;
            *mode = next_mode;
        }
        if (color[3] != next_alpha) {
            if (!mark_state_changed(entity, fields_.render_color)) return false;
            color[3] = next_alpha;
        }
    }
    return true;
}

void EngineAdapter::restore_reveal_rendering() {
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (system && render_fields_ok_) {
        for (const auto &[handle, saved] : original_reveal_rendering_) {
            CEntityInstance *entity = system->GetEntityInstance(CEntityHandle(handle));
            if (!entity || !entity->m_pEntity) continue;
            auto *base = reinterpret_cast<char *>(entity);
            if (mark_state_changed(entity, fields_.render_mode))
                *reinterpret_cast<uint8_t *>(base + fields_.render_mode) = saved.mode;
            if (mark_state_changed(entity, fields_.render_color))
                std::memcpy(base + fields_.render_color, saved.color.data(), saved.color.size());
        }
    }
    original_reveal_rendering_.clear();
}

bool EngineAdapter::mark_state_changed(CEntityInstance *entity, uint32_t offset) const {
#if defined(_WIN32) || defined(__linux__)
    if (!entity) return false;
    void *function = (*reinterpret_cast<void ***>(entity))[kSetStateChangedVtable];
    if (!executable_in_module(function, server_module_)) return false;
    StateChangeInfo info(offset);
    reinterpret_cast<void (*)(void *, StateChangeInfo *)>(function)(entity, &info);
    return true;
#else
    (void)entity; (void)offset;
    return false;
#endif
}

bool EngineAdapter::heal(int slot, int amount, int maximum_health) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity || amount <= 0 || maximum_health <= 0) return false;
    auto *value = reinterpret_cast<int *>(reinterpret_cast<char *>(entity) + fields_.health);
    if (*value <= 0) return false;
    const int updated = (std::min)(maximum_health, *value + amount);
    if (updated == *value) return true;
    if (!mark_state_changed(entity, fields_.health)) return false;
    *value = updated;
    return true;
}

bool EngineAdapter::restore_armor(int slot, int amount, int maximum_armor) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity || amount <= 0 || maximum_armor <= 0) return false;
    auto *value = reinterpret_cast<int *>(reinterpret_cast<char *>(entity) + fields_.armor);
    const int updated = (std::min)(maximum_armor, *value + amount);
    if (updated == *value) return true;
    if (!mark_state_changed(entity, fields_.armor)) return false;
    *value = updated;
    return true;
}

bool EngineAdapter::apply_spawn_attributes(int slot, int team, bool invisant_role, int health, int armor,
                                           bool helmet, float gravity_scale) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity || health < 1 || armor < 0 || gravity_scale <= 0.0f ||
        *reinterpret_cast<const uint8_t *>(reinterpret_cast<const char *>(entity) + fields_.team) != team)
        return false;
    auto *health_value = reinterpret_cast<int *>(reinterpret_cast<char *>(entity) + fields_.health);
    if (*health_value <= 0) return false;
    if (*health_value != health) {
        if (!mark_state_changed(entity, fields_.health)) return false;
        *health_value = health;
    }
    if (armor > 0) {
        auto *armor_value = reinterpret_cast<int *>(reinterpret_cast<char *>(entity) + fields_.armor);
        if (*armor_value != armor) {
            if (!mark_state_changed(entity, fields_.armor)) return false;
            *armor_value = armor;
        }
    }
    if (armor > 0) {
        void *items = *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(entity) + fields_.item_services);
        if (!items) return false;
        *reinterpret_cast<bool *>(reinterpret_cast<char *>(items) + fields_.has_helmet) = helmet;
        *reinterpret_cast<bool *>(reinterpret_cast<char *>(items) + fields_.has_defuser) = false;
    }
    auto *gravity_value = reinterpret_cast<float *>(
        reinterpret_cast<char *>(entity) + fields_.gravity_scale);
    if (*gravity_value != gravity_scale) {
        if (!mark_state_changed(entity, fields_.gravity_scale)) return false;
        *gravity_value = gravity_scale;
    }
    // Movement reads the effective scale; changing only the networked request
    // leaves the old effective gravity in place until SetGravityScale runs.
    *reinterpret_cast<float *>(reinterpret_cast<char *>(entity) +
        fields_.actual_gravity_scale) = gravity_scale;
    CEntityInstance *owner = controller(slot);
    if (!owner) return false;
    void *money = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(owner) + fields_.money_services);
    if (!money) return false;
    auto *account = reinterpret_cast<int *>(reinterpret_cast<char *>(money) + fields_.money_account);
    const int starting_money = invisant_role ? 0 : 16000;
    if (*account != starting_money) {
        if (!mark_state_changed(owner, fields_.money_services)) return false;
        *account = starting_money;
    }
    return true;
}

bool EngineAdapter::apply_upgrade_attributes(int slot, int team,
    const shroudfire::Attributes &before, const shroudfire::Attributes &after) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity || *reinterpret_cast<const uint8_t *>(
            reinterpret_cast<const char *>(entity) + fields_.team) != team) return false;
    auto *health = reinterpret_cast<int *>(reinterpret_cast<char *>(entity) + fields_.health);
    if (*health <= 0) return false;
    const int health_change = (std::max)(0, after.maximum_health - before.maximum_health);
    const int next_health = (std::min)(after.maximum_health, *health + health_change);
    if (next_health != *health) {
        if (!mark_state_changed(entity, fields_.health)) return false;
        *health = next_health;
    }
    if (before.maximum_armor > 0 || after.maximum_armor > 0) {
        auto *armor = reinterpret_cast<int *>(reinterpret_cast<char *>(entity) + fields_.armor);
        const int armor_change = (std::max)(0, after.maximum_armor - before.maximum_armor);
        const int next_armor = (std::min)(after.maximum_armor, *armor + armor_change);
        if (next_armor != *armor) {
            if (!mark_state_changed(entity, fields_.armor)) return false;
            *armor = next_armor;
        }
        void *items = *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(entity) + fields_.item_services);
        if (!items) return false;
        *reinterpret_cast<bool *>(reinterpret_cast<char *>(items) + fields_.has_helmet) = after.helmet;
    }
    auto *gravity = reinterpret_cast<float *>(
        reinterpret_cast<char *>(entity) + fields_.gravity_scale);
    if (std::fabs(*gravity - after.gravity_scale) > 0.001f) {
        if (!mark_state_changed(entity, fields_.gravity_scale)) return false;
        *gravity = after.gravity_scale;
    }
    *reinterpret_cast<float *>(reinterpret_cast<char *>(entity) +
        fields_.actual_gravity_scale) = after.gravity_scale;
    return true;
}

bool EngineAdapter::change_team(int slot, int team) const {
#if defined(_WIN32) || defined(__linux__)
    if (team < 1 || team > 3) return false;
    CEntityInstance *entity = controller(slot);
    if (!entity) return false;
    if (*reinterpret_cast<const uint8_t *>(
            reinterpret_cast<const char *>(entity) + fields_.team) == team) return true;
    // A live T/CT switch preserves the pawn. Spectator needs ChangeTeam to
    // retire it and create the observer pawn and camera services.
    if (team != 1 && player_alive(slot) && switch_team_function_) {
        reinterpret_cast<void (*)(void *, uint8_t)>(switch_team_function_)(
            entity, static_cast<uint8_t>(team));
        return true;
    }
    void *function = (*reinterpret_cast<void ***>(entity))[kChangeTeamVtable];
    if (!executable_in_module(function, server_module_)) return false;
    reinterpret_cast<void (*)(void *, int)>(function)(entity, team);
    return true;
#else
    (void)slot; (void)team;
    return false;
#endif
}

bool EngineAdapter::player_alive(int slot) const {
    CEntityInstance *entity = pawn(slot);
    return entity &&
        *reinterpret_cast<const int *>(reinterpret_cast<const char *>(entity) + fields_.health) > 0 &&
        *reinterpret_cast<const uint8_t *>(reinterpret_cast<const char *>(entity) + fields_.life_state) == 0;
}

int EngineAdapter::player_armor(int slot) const {
    CEntityInstance *entity = pawn(slot);
    return entity ? *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(entity) + fields_.armor) : 0;
}

int EngineAdapter::player_health(int slot) const {
    CEntityInstance *entity = pawn(slot);
    return entity ? *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(entity) + fields_.health) : 0;
}

bool EngineAdapter::active_weapon_is_knife(int slot) const {
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !system) return false;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    if (!services) return false;
    const CEntityHandle handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    const char *name = weapon && weapon->m_pEntity ? weapon->m_pEntity->GetClassname() : nullptr;
    return name && std::strstr(name, "knife") != nullptr;
}

void EngineAdapter::remap_knife_primary_attack(void *movement_services) const {
    if (!ready_ || !movement_services) return;
    auto *button_states = reinterpret_cast<uint64_t *>(
        reinterpret_cast<char *>(movement_services) + fields_.movement_buttons + fields_.button_states);
    for (int state = 0; state < 3; ++state) {
        if (button_states[state] & static_cast<uint64_t>(IN_ATTACK)) {
            button_states[state] &= ~static_cast<uint64_t>(IN_ATTACK);
            button_states[state] |= static_cast<uint64_t>(IN_ATTACK2);
        }
    }
}

float EngineAdapter::active_weapon_headshot_multiplier(int slot) const {
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !system) return 4.0f;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    if (!services) return 4.0f;
    const CEntityHandle handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    if (!weapon) return 4.0f;
    const void *vdata = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(weapon) + fields_.subclass_id + 4);
    if (!vdata) return 4.0f;
    const float multiplier = *reinterpret_cast<const float *>(
        reinterpret_cast<const char *>(vdata) + fields_.headshot_multiplier);
    return std::isfinite(multiplier) && multiplier > 0.0f ? multiplier : 4.0f;
}

bool EngineAdapter::weapon_use_context(void *services, void *weapon, int &slot,
                                       bool &knife) const {
    slot = 0;
    knife = false;
    if (!ready_ || !services || !weapon) return false;
    auto *item = reinterpret_cast<CEntityInstance *>(weapon);
    const char *name = item->m_pEntity ? item->m_pEntity->GetClassname() : nullptr;
    if (!name) return false;
    knife = std::strstr(name, "knife") != nullptr;
    for (int candidate = 1; candidate <= 64; ++candidate) {
        CEntityInstance *player = pawn(candidate);
        if (player && *reinterpret_cast<void *const *>(
                reinterpret_cast<const char *>(player) + fields_.weapon_services) == services) {
            slot = candidate;
            return true;
        }
    }
    return false;
}

bool EngineAdapter::purchase_context(void *services, void *item, int &slot,
                                     uint16_t &definition) const {
    slot = 0;
    definition = 0;
    if (!ready_ || !services || !item) return false;
    for (int candidate = 1; candidate <= 64; ++candidate) {
        const auto *player = pawn(candidate);
        if (player && *reinterpret_cast<void *const *>(
                reinterpret_cast<const char *>(player) + fields_.item_services) == services) {
            slot = candidate;
            definition = *reinterpret_cast<const uint16_t *>(
                static_cast<const char *>(item) + fields_.item_definition_index);
            return true;
        }
    }
    return false;
}

int EngineAdapter::drop_non_knife_weapons(int slot) const {
    if (!ready_) return -1;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *player = pawn(slot);
    if (!system || !player || !player_alive(slot)) return -1;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    if (!services) return -1;
    void *drop = (*reinterpret_cast<void ***>(services))[kDropWeaponVtable];
    if (!executable_in_module(drop, server_module_)) return -1;
    const auto *held = reinterpret_cast<const CUtlVector<CEntityHandle> *>(
        reinterpret_cast<const char *>(services) + fields_.weapons);
    const int count = held->Count();
    if (count < 0 || count > 64) return -1;
    std::array<CEntityHandle, 64> unwanted {};
    int unwanted_count = 0;
    bool has_knife = false;
    for (int i = 0; i < count; ++i) {
        const CEntityHandle handle = (*held)[i];
        CEntityInstance *weapon = system->GetEntityInstance(handle);
        const char *name = weapon && weapon->m_pEntity
            ? weapon->m_pEntity->GetClassname() : nullptr;
        if (!name || std::strncmp(name, "weapon_", 7) != 0) continue;
        if (std::strstr(name, "knife")) has_knife = true;
        else unwanted[unwanted_count++] = handle;
    }
    // Keep the player's current loadout if the spawn has not given a knife yet.
    if (!has_knife) return -1;
    int dropped = 0;
    for (int i = 0; i < unwanted_count; ++i) {
        CEntityInstance *weapon = system->GetEntityInstance(unwanted[i]);
        if (!weapon || !weapon->m_pEntity) continue;
        // The game keeps this entity networked until its normal cleanup.
        // Deleting it during a round transition can crash connected clients.
        reinterpret_cast<void (*)(void *, void *, void *, void *)>(drop)(
            services, weapon, nullptr, nullptr);
        ++dropped;
    }
    return dropped;
}

bool EngineAdapter::respawn(int slot) const {
#if defined(_WIN32) || defined(__linux__)
    CEntityInstance *entity = controller(slot);
    CEntityInstance *player_pawn = pawn(slot);
    if (!entity || !player_pawn || !set_player_pawn_function_) return false;
    void *function = (*reinterpret_cast<void ***>(entity))[kRespawnVtable];
    if (!executable_in_module(function, server_module_)) return false;
    reinterpret_cast<void (*)(void *, void *, bool, bool, bool, bool)>(
        set_player_pawn_function_)(entity, player_pawn, true, false, false, false);
    reinterpret_cast<void (*)(void *)>(function)(entity);
    return true;
#else
    (void)slot;
    return false;
#endif
}

bool EngineAdapter::terminate_round(int winning_team, float delay,
                                    std::string *diagnostic) const {
#if defined(_WIN32) || defined(__linux__)
    if (!ready_ || !terminate_round_function_ ||
        (winning_team != -1 && winning_team != 0 && winning_team != 2 && winning_team != 3)) {
        if (diagnostic) *diagnostic = "adapter not ready or invalid winner";
        return false;
    }
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) {
        if (diagnostic) *diagnostic = "game entity system unavailable";
        return false;
    }
    bool proxy_found = false;
    for (int index = 0; index < MAX_TOTAL_ENTITIES; ++index) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
        if (!entity || !entity->m_pEntity ||
            std::strcmp(entity->m_pEntity->GetClassname(), "cs_gamerules") != 0) continue;
        proxy_found = true;
        void *rules = *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(entity) + fields_.game_rules);
        if (!rules) continue;
        // GameCommencing (16) restarts the match on Linux and shows the
        // "Match starting" countdown. A draw (10) advances only the round.
#if defined(__linux__)
        const uint32_t restart_reason = 10u;
#else
        const uint32_t restart_reason = 16u;
#endif
        const uint32_t reason = winning_team == -1 ? restart_reason :
            winning_team == 2 ? 9u : winning_team == 3 ? 8u : 10u;
        reinterpret_cast<void (*)(void *, float, uint32_t, void *, uint8_t)>(
            terminate_round_function_)(rules, delay, reason, nullptr, 0);
        return true;
    }
    if (diagnostic) *diagnostic = proxy_found
        ? "cs_gamerules proxy has no game rules object" : "cs_gamerules proxy not found";
    return false;
#else
    (void)winning_team; (void)delay; (void)diagnostic;
    return false;
#endif
}

std::optional<bool> EngineAdapter::warmup_active() const {
    if (!ready_ || !warmup_field_ok_) return std::nullopt;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return std::nullopt;
    for (int index = 0; index < MAX_TOTAL_ENTITIES; ++index) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
        if (!entity || !entity->m_pEntity ||
            std::strcmp(entity->m_pEntity->GetClassname(), "cs_gamerules") != 0) continue;
        void *rules = *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(entity) + fields_.game_rules);
        if (!rules) return std::nullopt;
        return *reinterpret_cast<const bool *>(
            reinterpret_cast<const char *>(rules) + fields_.warmup_period);
    }
    return std::nullopt;
}

bool EngineAdapter::set_freeze_period(bool frozen) const {
#if defined(__linux__)
    if (!ready_ || !freeze_field_ok_) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return false;
    for (int index = 0; index < MAX_TOTAL_ENTITIES; ++index) {
        CEntityInstance *proxy = system->GetEntityInstance(CEntityIndex(index));
        if (!proxy || !proxy->m_pEntity ||
            std::strcmp(proxy->m_pEntity->GetClassname(), "cs_gamerules") != 0) continue;
        void *rules = *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(proxy) + fields_.game_rules);
        if (!rules) return false;
        auto *freeze = reinterpret_cast<bool *>(
            reinterpret_cast<char *>(rules) + fields_.freeze_period);
        if (*freeze == frozen) return true;
        *freeze = frozen;
        mark_state_changed(proxy, fields_.game_rules);
        return true;
    }
#else
    (void)frozen;
#endif
    return false;
}

bool EngineAdapter::client_print(int slot, int destination, const char *message) const {
    if (!ready_ || slot < 1 || slot > 64 || !controller(slot) ||
        !network_messages_ || !event_system_ || !message ||
        destination < 1 || destination > 6)
        return false;
    INetworkMessageInternal *type = network_messages_->FindNetworkMessagePartial("TextMsg");
    if (!type) return false;
    auto *data = type->AllocateMessage()->ToPB<CUserMessageTextMsg>();
    if (!data) return false;
    data->set_dest(destination);
    data->add_param(message);
    CPlayerBitVec recipients;
    recipients.Set(slot - 1);
    event_system_->PostEventAbstract(CSplitScreenSlot(-1), false, ABSOLUTE_PLAYER_LIMIT,
        reinterpret_cast<const uint64 *>(recipients.Base()), type, data, 0, BUF_RELIABLE);
    delete data;
    return true;
}

bool EngineAdapter::stop_sound_event_hash(int slot, uint32_t hash) const {
    if (!ready_ || slot < 1 || slot > 64 || !controller(slot) ||
        !network_messages_ || !event_system_) return false;
    INetworkMessageInternal *type = network_messages_->FindNetworkMessageById(
        GE_SosStopSoundEventHash);
    auto *data = type ? type->AllocateMessage()->ToPB<CMsgSosStopSoundEventHash>() : nullptr;
    if (!data) return false;
    data->set_soundevent_hash(hash);
    data->set_source_entity_index(-1);
    CPlayerBitVec recipients;
    recipients.Set(slot - 1);
    event_system_->PostEventAbstract(CSplitScreenSlot(-1), false, ABSOLUTE_PLAYER_LIMIT,
        reinterpret_cast<const uint64 *>(recipients.Base()), type, data, 0, BUF_RELIABLE);
    delete data;
    return true;
}

bool EngineAdapter::emit_sound_event(int source_slot, uint64_t recipients, const char *name) const {
#if defined(_WIN32) || defined(__linux__)
    if (!ready_ || !sound_filter_function_ || !recipients || !name) return false;
    CEntityInstance *source = pawn(source_slot);
    Activity activity;
    if (!source || !source->m_pEntity || !sample_activity(source_slot, activity))
        return false;

    // Source 2's EmitSound parameters differ from the SDK's Source 1 struct.
    struct SoundParameters {
        const char *name = nullptr;
        Vector origin {0.0f, 0.0f, 0.0f};
        float volume = 1.0f;
        float delay = 0.0f;
        CEntityIndex speaker {-1};
        uint32_t force_guid = 0;
        CEntityIndex soundscape {0};
        uint16_t pitch = 100;
        uint8_t flags = 1u << 4; // Emit at origin; the sound event adds its body-height offset.
    } params;
    static_assert(sizeof(SoundParameters) == 48);
    static_assert(offsetof(SoundParameters, origin) == 8);
    static_assert(offsetof(SoundParameters, volume) == 20);
    static_assert(offsetof(SoundParameters, force_guid) == 32);
    static_assert(offsetof(SoundParameters, soundscape) == 36);
    static_assert(offsetof(SoundParameters, pitch) == 40);
    static_assert(offsetof(SoundParameters, flags) == 42);
    static_assert(sizeof(SoundEventResult) == 24);
    params.name = name;
    params.origin.Init(activity.position[0], activity.position[1], activity.position[2]);
    params.speaker = source->m_pEntity->GetEntityIndex();

    class SoundRecipients final : public IRecipientFilter {
    public:
        explicit SoundRecipients(uint64_t mask) {
            for (int slot = 0; slot < 64; ++slot)
                if (mask & (uint64_t {1} << slot)) players_.Set(slot);
        }
        NetChannelBufType_t GetNetworkBufType() const override { return BUF_RELIABLE; }
        bool IsInitMessage() const override { return false; }
        const CPlayerBitVec &GetRecipients() const override { return players_; }
        CPlayerSlot GetPredictedPlayerSlot() const override { return CPlayerSlot(-1); }
    private:
        CPlayerBitVec players_;
    } filter(recipients);

    // Use the native emitter to allocate the event GUID and encode its parameters.
    const auto result = reinterpret_cast<SoundEventResult (*)(
        IRecipientFilter &, int, const SoundParameters *)>(sound_filter_function_)(
            filter, params.speaker.Get(), &params);
    return result.guid != 0;
#else
    (void)source_slot; (void)recipients; (void)name;
    return false;
#endif
}

bool EngineAdapter::center_html(int slot, IGameEventManager2 *events, const char *html,
    int duration_seconds) const {
    if (!ready_ || slot < 1 || slot > 64 || !controller(slot) || !events || !html ||
        !network_messages_ || !event_system_) return false;
    IGameEvent *event = events->CreateEvent("show_survival_respawn_status", true);
    if (!event) return false;
    auto key = [](const char *name) {
        return GameEventKeySymbol_t {
            CUtlStringToken(MurmurHash2LowerCase(name, STRINGTOKEN_MURMURHASH_SEED)), name};
    };
    event->SetString(key("loc_token"), html);
    event->SetInt(key("duration"), duration_seconds);
    event->SetPlayer(key("userid"), CPlayerSlot(slot - 1));
    INetworkMessageInternal *type = network_messages_->FindNetworkMessageById(
        GE_Source1LegacyGameEvent);
    auto *data = type ? type->AllocateMessage()->ToPB<CMsgSource1LegacyGameEvent>() : nullptr;
    if (!data) {
        events->FreeEvent(event);
        return false;
    }
    const bool serialized = events->SerializeEvent(event, data);
    events->FreeEvent(event);
    if (!serialized) {
        delete data;
        return false;
    }
    CPlayerBitVec recipients;
    recipients.Set(slot - 1);
    event_system_->PostEventAbstract(CSplitScreenSlot(-1), false, ABSOLUTE_PLAYER_LIMIT,
        reinterpret_cast<const uint64 *>(recipients.Base()), type, data, 0, BUF_RELIABLE);
    delete data;
    return true;
}

bool EngineAdapter::stabilize_center_html(float now, bool round_live,
                                         bool presentation_frame,
                                         bool win_html_active, bool mode_active) {
    if (!mode_active && !end_html_presentation_active_) return false;
    if (!ready_ || !html_restart_fields_ok_ || !std::isfinite(now)) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return false;
    CEntityInstance *proxy = nullptr;
    if (html_game_rules_index_ >= 0) {
        proxy = system->GetEntityInstance(CEntityIndex(html_game_rules_index_));
        if (!proxy || !proxy->m_pEntity ||
            std::strcmp(proxy->m_pEntity->GetClassname(), "cs_gamerules") != 0)
            proxy = nullptr;
    }
    if (!proxy) {
        html_game_rules_index_ = -1;
        for (int index = 0; index < MAX_TOTAL_ENTITIES; ++index) {
            CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
            if (!entity || !entity->m_pEntity ||
                std::strcmp(entity->m_pEntity->GetClassname(), "cs_gamerules") != 0) continue;
            proxy = entity;
            html_game_rules_index_ = index;
            break;
        }
    }
    if (!proxy) return false;
    void *rules = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(proxy) + fields_.game_rules);
    if (!rules) return false;
    auto *restart_time_field = reinterpret_cast<float *>(
        reinterpret_cast<char *>(rules) + fields_.restart_round_time);
    auto *game_restart = reinterpret_cast<bool *>(
        reinterpret_cast<char *>(rules) + fields_.game_restart);
    if (end_html_presentation_active_) {
        // A console command may have scheduled a real restart since the last
        // presentation. Restore only the values we temporarily substituted.
        if (*restart_time_field == end_html_presented_restart_time_ &&
            *game_restart == end_html_presented_game_restart_) {
            *restart_time_field = end_html_restart_time_;
            *game_restart = end_html_game_restart_;
            mark_state_changed(proxy, fields_.game_rules);
        }
        end_html_presentation_active_ = false;
    }
    if (!mode_active) {
        round_ui_was_live_ = false;
        round_ui_captured_ = false;
        return true;
    }
    // The pre-frame hook must leave native match and round state intact.
    // m_bGameRestart also controls server simulation, not just the menu UI.
    if (!presentation_frame) return true;
    if (round_end_count_field_ok_) {
        auto *round_end_count = reinterpret_cast<uint8_t *>(
            reinterpret_cast<char *>(rules) + fields_.round_end_count);
        if (round_live && !round_ui_was_live_) {
            round_end_count_before_ = *round_end_count;
            round_ui_captured_ = true;
        }
        if (!round_live && round_ui_captured_ &&
            *round_end_count != round_end_count_before_) {
            // Preserve the live-round value so the client never constructs
            // the stock ROUND WON and MVP panels during the result period.
            *round_end_count = round_end_count_before_;
            mark_state_changed(proxy, fields_.game_rules);
        }
    }
    const float restart_time = *restart_time_field;
    if (!std::isfinite(restart_time)) return false;
    round_ui_was_live_ = round_live;
    // Replicate the menu state only after simulation, for live menus as well
    // as result panels. The next pre-frame hook restores the native state.
    const bool result_html = win_html_active && !round_live;
    const float presented_time = result_html ? now - 1.0f : restart_time;
    const bool presented_restart = result_html || (round_live && restart_time < now);
    if (presented_time != restart_time || *game_restart != presented_restart) {
        end_html_restart_time_ = restart_time;
        end_html_game_restart_ = *game_restart;
        end_html_presented_restart_time_ = presented_time;
        end_html_presented_game_restart_ = presented_restart;
        *restart_time_field = presented_time;
        *game_restart = presented_restart;
        end_html_presentation_active_ = true;
        mark_state_changed(proxy, fields_.game_rules);
    }
    static bool logged = false;
    if (!logged) {
        std::fprintf(stderr, "[Shroudfire] HTML menu stabilization active (restart %.2f, time %.2f).\n",
                     restart_time, now);
        logged = true;
    }
    return true;
}

bool EngineAdapter::show_win_layout(int slot, bool invisant_win, int xp, int kill_xp,
                                    const char *victim_name, bool victim_invisant,
                                    const char *advanced_role, int advanced_level) {
#ifndef _WIN32
    if (!ready_ || slot < 1 || slot > 64 || !controller(slot) ||
        !create_entity_function_ || !dispatch_spawn_function_ ||
        !keyvalues_constructor_function_ || !keyvalues_find_or_create_function_ ||
        !keyvalues_set_string_function_ || !hud_set_player_class_function_ ||
        !hud_set_player_dialog_function_) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return false;
    CEntityInstance *layout = win_layout_handle_
        ? system->GetEntityInstance(CEntityHandle(win_layout_handle_)) : nullptr;
    if (layout && (!layout->m_pEntity ||
                   std::strcmp(layout->m_pEntity->GetClassname(), "custom_hud_layout") != 0))
        layout = nullptr;
    if (!layout) {
        win_layout_handle_ = 0;
        layout = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
            create_entity_function_)("custom_hud_layout", -1);
        if (!layout || !layout->m_pEntity) return false;
        auto *keys = static_cast<CEntityKeyValues *>(::operator new(sizeof(CEntityKeyValues)));
        reinterpret_cast<void (*)(CEntityKeyValues *, CKV3Arena *, EntityKVAllocatorType_t)>(
            keyvalues_constructor_function_)(keys, nullptr, EKV_ALLOCATOR_NORMAL);
        const CKV3MemberName layout_key("layout");
        KeyValues3 *layout_value = reinterpret_cast<KeyValues3 *(*)(
            CEntityKeyValues *, const CKV3MemberName *, const char *)>(
                keyvalues_find_or_create_function_)(keys, &layout_key, nullptr);
        if (!layout_value) {
            reinterpret_cast<void (*)(void *)>(remove_entity_function_)(layout);
            return false;
        }
        reinterpret_cast<void (*)(CEntityKeyValues *, KeyValues3 *, const char *)>(
            keyvalues_set_string_function_)(keys, layout_value,
                "panorama/layout/custom_game/invisant_win.vxml_c");
        reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(layout, keys);
        win_layout_handle_ = layout->m_pEntity->GetRefEHandle().ToInt();
        std::fprintf(stderr, "[Shroudfire] Custom WIN HUD entity created.\n");
    }
    const int player_slot = slot - 1;
    auto set_class = reinterpret_cast<void (*)(void *, int, CUtlString, CUtlString, bool)>(
        hud_set_player_class_function_);
    auto set_text = reinterpret_cast<void (*)(void *, int, CUtlString, CUtlString, CUtlString)>(
        hud_set_player_dialog_function_);
    set_text(layout, player_slot, "RoundXpValue", "value", std::to_string(xp).c_str());
    set_text(layout, player_slot, "KillXpValue", "value", std::to_string(kill_xp).c_str());
    set_text(layout, player_slot, "VictimName", "value", victim_name ? victim_name : "");
    set_text(layout, player_slot, "LevelRole", "value", advanced_role ? advanced_role : "");
    set_text(layout, player_slot, "LevelValue", "value",
             std::to_string(advanced_level).c_str());
    set_class(layout, player_slot, "ArmitiveTitle", "show", !invisant_win);
    set_class(layout, player_slot, "InvisantTitle", "show", invisant_win);
    set_class(layout, player_slot, "RoundLine", "show", xp > 0);
    set_class(layout, player_slot, "KillLine", "show", kill_xp > 0 && victim_name && *victim_name);
    set_class(layout, player_slot, "VictimName", "invisant", victim_invisant);
    set_class(layout, player_slot, "LevelLine", "show", advanced_level > 0);
    set_class(layout, player_slot, "LevelRole", "invisant", advanced_role &&
              std::strcmp(advanced_role, "Invisant") == 0);
    set_class(layout, player_slot, "LevelValue", "invisant", advanced_role &&
              std::strcmp(advanced_role, "Invisant") == 0);
    set_class(layout, player_slot, "WinRoot", "show", true);
    return true;
#else
    (void)slot; (void)invisant_win; (void)xp; (void)kill_xp;
    (void)victim_name; (void)victim_invisant;
    (void)advanced_role; (void)advanced_level;
    return false;
#endif
}

void EngineAdapter::hide_win_layout(int slot) {
#ifndef _WIN32
    if (!ready_ || slot < 1 || slot > 64 || !win_layout_handle_ ||
        !hud_set_player_class_function_) return;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *layout = system
        ? system->GetEntityInstance(CEntityHandle(win_layout_handle_)) : nullptr;
    if (layout && layout->m_pEntity &&
        std::strcmp(layout->m_pEntity->GetClassname(), "custom_hud_layout") == 0)
        reinterpret_cast<void (*)(void *, int, CUtlString, CUtlString, bool)>(
            hud_set_player_class_function_)(layout, slot - 1,
                CUtlString("WinRoot"), CUtlString("show"), false);
#else
    (void)slot;
#endif
}

bool EngineAdapter::refresh_buy_zone(int slot, int team) {
    if (!ready_ || slot < 1 || slot > 64 || (team != 2 && team != 3)) return false;
    CEntityInstance *player_pawn = pawn(slot);
    if (!player_pawn || !player_pawn->m_pEntity) return false;
    const void *body = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player_pawn) + fields_.body_component);
    const void *scene = body ? *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(body) + fields_.scene_node) : nullptr;
    if (!scene) return false;
    std::array<float, 3> position {};
    std::memcpy(position.data(), reinterpret_cast<const char *>(scene) + fields_.abs_origin,
                3 * sizeof(float));
    bool in_area = false;
    const auto &anchor = buyzone_spawn_[slot];
    if (anchor.pawn == player_pawn->m_pEntity->GetRefEHandle().ToInt() &&
        std::fabs(position[2] - anchor.position[2]) <= 96.0f) {
        const float dx = position[0] - anchor.position[0];
        const float dy = position[1] - anchor.position[1];
        in_area = dx * dx + dy * dy <= 320.0f * 320.0f;
    }
    for (const auto &zone : extra_spawn_zones_) {
        if (in_area) break;
        if (zone.team != team ||
            std::fabs(position[2] - zone.position[2]) > 72.0f) continue;
        const float dx = position[0] - zone.position[0];
        const float dy = position[1] - zone.position[1];
        in_area = dx * dx + dy * dy <= 112.0f * 112.0f;
    }
    const auto *touching = reinterpret_cast<const CUtlVector<CEntityHandle> *>(
        reinterpret_cast<const char *>(player_pawn) + fields_.touching_buy_zones);
    if (touching->Count() > 0 && touching->Count() <= 16) {
        auto *system = static_cast<CGameEntitySystem *>(entity_system());
        for (int index = 0; system && index < touching->Count(); ++index) {
            CEntityInstance *zone = system->GetEntityInstance((*touching)[index]);
            if (!zone || !zone->m_pEntity ||
                std::strcmp(zone->m_pEntity->GetClassname(), "func_buyzone") != 0) continue;
            const int zone_team = *reinterpret_cast<const uint8_t *>(
                reinterpret_cast<const char *>(zone) + fields_.team);
            if (zone_team == 0 || zone_team == team) {
                in_area = true;
                break;
            }
        }
    }
    for (uint32_t offset : {fields_.in_buy_zone, fields_.was_in_buy_zone}) {
        auto *flag = reinterpret_cast<bool *>(reinterpret_cast<char *>(player_pawn) + offset);
        if (*flag != in_area && mark_state_changed(player_pawn, offset)) *flag = in_area;
    }
    return in_area;
}

bool EngineAdapter::can_buy(int slot, int team) {
    if (!buy_rules_fields_ok_ || !refresh_buy_zone(slot, team)) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return false;
    CEntityInstance *proxy = nullptr;
    if (html_game_rules_index_ >= 0) {
        proxy = system->GetEntityInstance(CEntityIndex(html_game_rules_index_));
        if (!proxy || !proxy->m_pEntity ||
            std::strcmp(proxy->m_pEntity->GetClassname(), "cs_gamerules") != 0)
            proxy = nullptr;
    }
    if (!proxy) {
        html_game_rules_index_ = -1;
        for (int index = 0; index < MAX_TOTAL_ENTITIES; ++index) {
            CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
            if (!entity || !entity->m_pEntity ||
                std::strcmp(entity->m_pEntity->GetClassname(), "cs_gamerules") != 0) continue;
            proxy = entity;
            html_game_rules_index_ = index;
            break;
        }
    }
    if (!proxy) return false;
    void *rules = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(proxy) + fields_.game_rules);
    if (!rules) return false;
    const char *base = reinterpret_cast<const char *>(rules);
    return !*reinterpret_cast<const bool *>(base + fields_.buy_time_ended) &&
        !*reinterpret_cast<const bool *>(base +
            (team == 3 ? fields_.ct_cant_buy : fields_.t_cant_buy));
}

bool EngineAdapter::hud_message(int slot, int channel, float x, float y, float hold,
                                int red, int green, int blue, int alpha,
                                const char *message) const {
    if (!ready_ || slot < 1 || slot > 64 || !controller(slot) || !network_messages_ ||
        !event_system_ || !message) return false;
    INetworkMessageInternal *type = network_messages_->FindNetworkMessage("CCSUsrMsg_HudMsg");
    if (!type) return false;
    auto *data = type->AllocateMessage()->ToPB<CCSUsrMsg_HudMsg>();
    if (!data) return false;
    data->set_channel(channel);
    data->mutable_pos()->set_x(x);
    data->mutable_pos()->set_y(y);
    for (auto *color : {data->mutable_clr1(), data->mutable_clr2()}) {
        color->set_r(red);
        color->set_g(green);
        color->set_b(blue);
        color->set_a(alpha);
    }
    data->set_effect(0);
    data->set_fade_in_time(0.0f);
    data->set_fade_out_time(0.0f);
    data->set_hold_time(hold);
    data->set_fx_time(0.0f);
    data->set_text(message);
    CPlayerBitVec recipients;
    recipients.Set(slot - 1);
    event_system_->PostEventAbstract(CSplitScreenSlot(-1), false, ABSOLUTE_PLAYER_LIMIT,
        reinterpret_cast<const uint64 *>(recipients.Base()), type, data, 0, BUF_RELIABLE);
    delete data;
    return true;
}

bool EngineAdapter::screen_fade(int slot, int duration, int flags, int red, int green,
                                int blue, int alpha, int hold_time) {
#if defined(_WIN32) || defined(__linux__)
    (void)flags;
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !system || !fade_fields_ok_ || !accept_input_function_ ||
        !create_entity_function_ || !dispatch_spawn_function_) return false;
    CEntityInstance *fade = fade_entity_
        ? system->GetEntityInstance(CEntityHandle(fade_entity_)) : nullptr;
    if (!fade || !fade->m_pEntity) {
        fade_entity_ = 0;
        if (alpha == 0) return true;
        fade = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
            create_entity_function_)("env_fade", -1);
        if (!fade || !fade->m_pEntity) return false;
        reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(fade, nullptr);
        fade_entity_ = fade->m_pEntity->GetRefEHandle().ToInt();
    }
    auto *base = reinterpret_cast<char *>(fade);
    const uint32_t spawn_flags = alpha > 0 ? 4u : 5u;
    if (!mark_state_changed(fade, fields_.spawn_flags) ||
        !mark_state_changed(fade, fields_.fade_color)) return false;
    *reinterpret_cast<uint32_t *>(base + fields_.spawn_flags) = spawn_flags;
    *reinterpret_cast<uint32_t *>(base + fields_.fade_color) =
        static_cast<uint32_t>(red) | (static_cast<uint32_t>(green) << 8) |
        (static_cast<uint32_t>(blue) << 16) | (static_cast<uint32_t>(alpha) << 24);
    *reinterpret_cast<float *>(base + fields_.fade_duration) =
        alpha > 0 ? (std::max)(0.05f, static_cast<float>(duration) / 1000.0f) : 0.0f;
    *reinterpret_cast<float *>(base + fields_.fade_hold_duration) =
        (std::max)(0.0f, static_cast<float>(hold_time) / 1000.0f);
    variant_t value;
    reinterpret_cast<void (*)(void *, const char *, void *, void *, variant_t *, int, void *)>(
        accept_input_function_)(fade, "Fade", player, player, &value, 0, nullptr);
    return true;
#else
    (void)slot; (void)duration; (void)flags; (void)red; (void)green;
    (void)blue; (void)alpha; (void)hold_time;
    return false;
#endif
}

void EngineAdapter::remove_fade_entity() {
#if defined(_WIN32) || defined(__linux__)
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *entity = system && fade_entity_
        ? system->GetEntityInstance(CEntityHandle(fade_entity_)) : nullptr;
    if (entity && remove_entity_function_)
        reinterpret_cast<void (*)(void *)>(remove_entity_function_)(entity);
#endif
    fade_entity_ = 0;
}

bool EngineAdapter::blood_burst(int victim_slot, int attacker_slot, int damage, int hitgroup,
                                int bursts, float now, int *particles_created) {
    if (particles_created) *particles_created = 0;
#if defined(_WIN32) || defined(__linux__)
    if (!ready_ || !blood_fields_ok_ || !accept_input_function_ ||
        !create_entity_function_ || !dispatch_spawn_function_ ||
        damage <= 0 || bursts <= 0 || bursts > 6) return false;
    CEntityInstance *victim = pawn(victim_slot);
    Activity victim_activity;
    Activity attacker_activity;
    if (!victim || !sample_activity(victim_slot, victim_activity) ||
        !sample_activity(attacker_slot, attacker_activity)) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return false;
    CEntityInstance *blood = blood_entity_
        ? system->GetEntityInstance(CEntityHandle(blood_entity_)) : nullptr;
    if (!blood || !blood->m_pEntity) {
        blood_entity_ = 0;
        blood = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
            create_entity_function_)("env_blood", -1);
        if (!blood || !blood->m_pEntity) return false;
        reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(blood, nullptr);
        blood_entity_ = blood->m_pEntity->GetRefEHandle().ToInt();
    }
    void *teleport = (*reinterpret_cast<void ***>(blood))[kTeleportVtable];
    if (!executable_in_module(teleport, server_module_)) return false;
    auto *base = reinterpret_cast<char *>(blood);
    const float dx = victim_activity.position[0] - attacker_activity.position[0];
    const float dy = victim_activity.position[1] - attacker_activity.position[1];
    const float dz = victim_activity.position[2] - attacker_activity.position[2];
    const float length = (std::max)(1.0f, std::sqrt(dx * dx + dy * dy + dz * dz));
    const float direction[3] {dx / length, dy / length, dz / length};
    float hit_height = 48.0f;
    switch (hitgroup) {
        case HITGROUP_HEAD: hit_height = 68.0f; break;
        case HITGROUP_NECK: hit_height = 60.0f; break;
        case HITGROUP_CHEST: hit_height = 52.0f; break;
        case HITGROUP_STOMACH: hit_height = 40.0f; break;
        case HITGROUP_LEFTLEG:
        case HITGROUP_RIGHTLEG: hit_height = 24.0f; break;
        default: break;
    }
    *reinterpret_cast<float *>(base + fields_.blood_amount) =
        static_cast<float>(std::clamp(damage + 36, 48, 96));
    *reinterpret_cast<uint32_t *>(base + fields_.blood_color) = 0;
    for (int burst = 0; burst < bursts; ++burst) {
        // Alternate CS2's drip and stream modes; streams also leave decals.
        *reinterpret_cast<uint32_t *>(base + fields_.spawn_flags) =
            burst % 2 == 0 ? 0u : 2u | 8u;
        float position[3] {
            victim_activity.position[0] + static_cast<float>((burst * 7) % 9 - 4),
            victim_activity.position[1] + static_cast<float>((burst * 5) % 9 - 4),
            victim_activity.position[2] + hit_height + static_cast<float>((burst * 3) % 9 - 4)
        };
        float spray[3] {
            direction[0] + static_cast<float>((burst * 3) % 5 - 2) * 0.06f,
            direction[1] + static_cast<float>((burst * 2) % 5 - 2) * 0.06f,
            direction[2] + static_cast<float>((burst * 4) % 5 - 2) * 0.06f
        };
        const float spray_length = (std::max)(1.0f, std::sqrt(
            spray[0] * spray[0] + spray[1] * spray[1] + spray[2] * spray[2]));
        for (float &axis : spray) axis /= spray_length;
        std::memcpy(base + fields_.blood_spray_dir, spray, sizeof(spray));
        reinterpret_cast<void (*)(void *, const void *, const void *, const void *)>(
            teleport)(blood, position, nullptr, nullptr);
        variant_t value;
        reinterpret_cast<void (*)(void *, const char *, void *, void *, variant_t *, int, void *)>(
            accept_input_function_)(blood, "EmitBlood", victim, victim, &value, 0, nullptr);
    }
    if (particle_fields_ok_ && remove_entity_function_) {
        // This effect is an independent world entity. The hidden player pawn is
        // never transmitted to Armitive clients, including during the hit.
        static constexpr char effect[] =
            "particles/blood_impact/blood_impact_mist1.vpcf";
        for (int burst = 0; burst < (std::min)(bursts, 2); ++burst) {
            while (blood_particles_.size() >= 24) {
                CEntityInstance *old = system->GetEntityInstance(CEntityHandle(
                    blood_particles_.front().first));
                if (old) reinterpret_cast<void (*)(void *)>(remove_entity_function_)(old);
                blood_particles_.erase(blood_particles_.begin());
            }
            CEntityInstance *particle = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
                create_entity_function_)("env_particle_glow", -1);
            if (!particle || !particle->m_pEntity) break;
            auto *particle_base = reinterpret_cast<char *>(particle);
            *reinterpret_cast<bool *>(particle_base + fields_.particle_start_active) = true;
            *reinterpret_cast<CUtlSymbolLarge *>(particle_base + fields_.particle_effect_name) =
                CUtlSymbolLarge(effect);
            void *particle_teleport = (*reinterpret_cast<void ***>(particle))[kTeleportVtable];
            if (!executable_in_module(particle_teleport, server_module_)) {
                reinterpret_cast<void (*)(void *)>(remove_entity_function_)(particle);
                break;
            }
            const float yaw = victim_activity.angles[1] * 0.01745329251994329577f;
            const float position[3] {
                victim_activity.position[0] - std::cos(yaw) * 11.0f,
                victim_activity.position[1] - std::sin(yaw) * 11.0f,
                victim_activity.position[2] + hit_height
            };
            const float particle_angles[3] {0.0f, victim_activity.angles[1], 0.0f};
            reinterpret_cast<void (*)(void *, const void *, const void *, const void *)>(
                particle_teleport)(particle, position, particle_angles, nullptr);
            reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(particle, nullptr);
            variant_t start;
            reinterpret_cast<void (*)(void *, const char *, void *, void *, variant_t *, int, void *)>(
                accept_input_function_)(particle, "Start", nullptr, nullptr, &start, 0, nullptr);
            blood_particles_.push_back({particle->m_pEntity->GetRefEHandle().ToInt(), now + 1.0f});
            if (particles_created) ++*particles_created;
        }
    }
    return true;
#else
    (void)victim_slot; (void)attacker_slot; (void)damage; (void)hitgroup;
    (void)bursts; (void)now;
    return false;
#endif
}

void EngineAdapter::expire_blood_particles(float now) {
#if defined(_WIN32) || defined(__linux__)
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system || !remove_entity_function_) return;
    for (auto it = blood_particles_.begin(); it != blood_particles_.end();) {
        if (it->second > now) { ++it; continue; }
        CEntityInstance *particle = system->GetEntityInstance(CEntityHandle(it->first));
        if (particle) reinterpret_cast<void (*)(void *)>(remove_entity_function_)(particle);
        it = blood_particles_.erase(it);
    }
#else
    (void)now;
#endif
}

void EngineAdapter::remove_blood_entity() {
#if defined(_WIN32) || defined(__linux__)
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *blood = system && blood_entity_
        ? system->GetEntityInstance(CEntityHandle(blood_entity_)) : nullptr;
    if (blood && remove_entity_function_)
        reinterpret_cast<void (*)(void *)>(remove_entity_function_)(blood);
    if (system && remove_entity_function_) {
        for (const auto &[handle, expiry] : blood_particles_) {
            (void)expiry;
            CEntityInstance *particle = system->GetEntityInstance(CEntityHandle(handle));
            if (particle)
                reinterpret_cast<void (*)(void *)>(remove_entity_function_)(particle);
        }
    }
#endif
    blood_entity_ = 0;
    blood_particles_.clear();
}

bool EngineAdapter::commit_suicide(int slot) const {
#if defined(_WIN32) || defined(__linux__)
    CEntityInstance *entity = pawn(slot);
    if (!entity) return false;
    void *function = (*reinterpret_cast<void ***>(entity))[kCommitSuicideVtable];
    if (!executable_in_module(function, server_module_)) return false;
    reinterpret_cast<void (*)(void *, bool, bool)>(function)(entity, false, true);
    return true;
#else
    (void)slot;
    return false;
#endif
}

bool EngineAdapter::drain_health(int slot, int amount, bool &fatal) const {
    fatal = false;
    CEntityInstance *entity = pawn(slot);
    if (!entity || amount <= 0) return false;
    auto *health = reinterpret_cast<int *>(reinterpret_cast<char *>(entity) + fields_.health);
    if (*health <= 0) return false;
    if (*health <= amount) {
        fatal = true;
        return commit_suicide(slot);
    }
    if (!mark_state_changed(entity, fields_.health)) return false;
    *health -= amount;
    return true;
}

bool EngineAdapter::leap(int slot, int impulse, bool add_momentum) const {
#if defined(_WIN32) || defined(__linux__)
    CEntityInstance *entity = pawn(slot);
    if (!entity || impulse <= 0) return false;
    void *function = (*reinterpret_cast<void ***>(entity))[kTeleportVtable];
    if (!executable_in_module(function, server_module_)) return false;
    const char *base = reinterpret_cast<const char *>(entity);
    if (*reinterpret_cast<const int *>(base + fields_.health) <= 0) return false;
    float angles[3] {};
    std::memcpy(angles, base + fields_.eye_angles, sizeof(angles));
    const char *current = base + fields_.velocity;
    const std::array<float, 3> previous {
        *reinterpret_cast<const float *>(current + fields_.velocity_x),
        *reinterpret_cast<const float *>(current + fields_.velocity_y),
        *reinterpret_cast<const float *>(current + fields_.velocity_z)
    };
    const auto velocity = shroudfire::surge_velocity(previous, angles[0], angles[1],
                                                   static_cast<float>(impulse), add_momentum);
    reinterpret_cast<void (*)(void *, const void *, const void *, const void *)>(function)(
        entity, nullptr, nullptr, velocity.data());
    return true;
#else
    (void)slot; (void)impulse; (void)add_momentum;
    return false;
#endif
}

bool EngineAdapter::leap_clear(int slot, bool &clear) const {
    clear = false;
#if defined(_WIN32) || defined(__linux__)
    CEntityInstance *entity = pawn(slot);
    Activity activity;
    if (!entity || !sample_activity(slot, activity)) return false;
    if (*reinterpret_cast<const uint8_t *>(
            reinterpret_cast<const char *>(entity) + fields_.move_type) != MOVETYPE_WALK)
        return true;
    auto **vtable = reinterpret_cast<void **>(
        reinterpret_cast<char *>(server_module_) + kNavPhysicsVtableRva);
    void *function = vtable[kTraceShapeFilterVtable];
    if (!executable_in_module(function, server_module_)) return false;

    const float pitch = activity.angles[0] * 0.01745329251994329577f;
    const float yaw = activity.angles[1] * 0.01745329251994329577f;
    const Vector start(activity.position[0], activity.position[1], activity.position[2] + 2.0f);
    const Vector end(start.x + 32.0f * std::cos(pitch) * std::cos(yaw),
                     start.y + 32.0f * std::cos(pitch) * std::sin(yaw),
                     start.z - 32.0f * std::sin(pitch));
    const float height = (activity.flags & FL_DUCKING) ? 54.0f : 72.0f;
    const Ray_t hull(Vector(-16.0f, -16.0f, 0.0f), Vector(16.0f, 16.0f, height));
    CTraceFilter filter(entity, nullptr, 0xffff, MASK_PLAYERSOLID,
                        COLLISION_GROUP_PLAYER_MOVEMENT, true);
    filter.m_nInteractsAs = CONTENTS_PLAYER;
    CGameTrace trace;
    void *nav_interface = vtable;
    reinterpret_cast<void (*)(void *, const Ray_t &, const Vector &, const Vector &,
                               CTraceFilter *, CGameTrace *)>(function)(
        &nav_interface, hull, start, end, &filter, &trace);
    clear = !trace.m_bStartInSolid && std::isfinite(trace.m_flFraction) &&
            trace.m_flFraction >= 0.98f;
    return true;
#else
    (void)slot;
    return false;
#endif
}

bool EngineAdapter::set_damage_enabled(int slot, bool enabled, bool &previous) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity) return false;
    auto *value = reinterpret_cast<bool *>(reinterpret_cast<char *>(entity) + fields_.takes_damage);
    previous = *value;
    if (*value == enabled) return true;
    // Damage gating is server-side; a network update is attempted for builds
    // that expose the field to clients.
    mark_state_changed(entity, fields_.takes_damage);
    *value = enabled;
    return true;
}

bool EngineAdapter::intercept_overload_reload(int slot, int overload_stage,
                                              int overload_percent, float now,
                                              float reload_scale) {
    if (!ready_ || slot < 1 || slot > 64 || overload_stage <= 0 ||
        overload_percent <= 0) return false;
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !system) return false;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    if (!services) return false;
    const CEntityHandle handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    const char *name = weapon && weapon->m_pEntity ? weapon->m_pEntity->GetClassname() : nullptr;
    if (!name) return false;
    const char *base = reinterpret_cast<const char *>(weapon);
    const void *vdata = *reinterpret_cast<void *const *>(base + fields_.subclass_id + 4);
    if (!vdata) return false;
    if (*reinterpret_cast<const uint32_t *>(
            reinterpret_cast<const char *>(vdata) + fields_.gear_slot) > 1)
        return false;
    const int base_clip = *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(vdata) + fields_.primary_clip_max);
    if (base_clip < 1 || base_clip > 200) return false;
    const int maximum_clip = base_clip + (std::max)(1,
        base_clip * overload_stage * overload_percent / 100);
    auto *clip = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) + fields_.clip1);
    auto *reserve = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) +
        fields_.reserve_ammo);
    auto tracked = overload_weapons_.find(handle.ToInt());
    if (tracked != overload_weapons_.end() && tracked->second.owner_slot == slot &&
        tracked->second.shell_reload_next_at > 0.0f) return true;
    if (*clip < 0 || *clip > 1000) return false;
    if (*clip >= maximum_clip) return true;
    const bool single_shells = *reinterpret_cast<const bool *>(
        reinterpret_cast<const char *>(vdata) + fields_.reloads_single_shells) ||
        std::strcmp(name, "weapon_nova") == 0 ||
        std::strcmp(name, "weapon_xm1014") == 0;
    if (!single_shells) return false;
    if (*reserve <= 0 || *reserve > 10000 || !std::isfinite(now) ||
        now <= 0.0f || tracked == overload_weapons_.end() ||
        tracked->second.owner_slot != slot)
        return true;
    auto &state = tracked->second;
    state.reload_start_clip = *clip;
    state.reload_start_reserve = *reserve;
    state.shell_reload_clip = *clip;
    state.shell_reload_reserve = *reserve;
    state.shell_reload_next_at = now + 0.6f *
        (std::max)(0.1f, reload_scale);
    return true;
}

void EngineAdapter::cancel_overload_reload_on_attack(int slot) {
    if (!ready_ || slot < 1 || slot > 64) return;
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !system) return;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    if (!services) return;
    const CEntityHandle handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    auto tracked = overload_weapons_.find(handle.ToInt());
    if (tracked == overload_weapons_.end() || tracked->second.owner_slot != slot ||
        tracked->second.shell_reload_next_at <= 0.0f) return;
    tracked->second.reload_start_clip = tracked->second.shell_reload_clip;
    tracked->second.shell_reload_next_at = 0.0f;
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    if (!weapon) return;
    auto *reloading = reinterpret_cast<bool *>(
        reinterpret_cast<char *>(weapon) + fields_.weapon_in_reload);
    if (*reloading && mark_state_changed(weapon, fields_.weapon_in_reload))
        *reloading = false;
}

bool EngineAdapter::apply_ammo_upgrades(int slot, int overload_stage, int stockpile_stage,
                                        int overload_percent, int stockpile_clips,
                                        float now, float reload_scale) {
    if (overload_stage < 0 || overload_stage > 5 ||
        stockpile_stage < 0 || stockpile_stage > 5) return false;
    CEntityInstance *entity = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!entity || !system) return false;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(entity) + fields_.weapon_services);
    if (!services) return false;
    const auto *weapons = reinterpret_cast<const CUtlVector<CEntityHandle> *>(
        reinterpret_cast<const char *>(services) + fields_.weapons);
    const CEntityHandle active_weapon = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    const int count = weapons->Count();
    if (count < 0 || count > 64) return false;
    bool found = false;
    for (int i = 0; i < count; ++i) {
        const CEntityHandle handle = (*weapons)[i];
        CEntityInstance *weapon = system->GetEntityInstance(handle);
        if (!weapon || !weapon->m_pEntity) continue;
        const char *name = weapon->m_pEntity->GetClassname();
        if (!name || std::strncmp(name, "weapon_", 7) != 0) continue;
        const char *base = reinterpret_cast<const char *>(weapon);
        void *vdata = *reinterpret_cast<void *const *>(base + fields_.subclass_id + 4);
        if (!vdata) continue;
        const uint32_t gear_slot = *reinterpret_cast<const uint32_t *>(
            reinterpret_cast<const char *>(vdata) + fields_.gear_slot);
        if (gear_slot > 1) continue;
        const int base_clip = *reinterpret_cast<const int *>(
            reinterpret_cast<const char *>(vdata) + fields_.primary_clip_max);
        const int base_reserve = *reinterpret_cast<const int *>(
            reinterpret_cast<const char *>(vdata) + fields_.primary_reserve_max);
        if (base_clip < 1 || base_clip > 200 || base_reserve < 1 || base_reserve > 500) continue;
        const bool clips = *reinterpret_cast<const bool *>(
            reinterpret_cast<const char *>(vdata) + fields_.reserve_as_clips);
        const bool single_shells = *reinterpret_cast<const bool *>(
            reinterpret_cast<const char *>(vdata) + fields_.reloads_single_shells) ||
            std::strcmp(name, "weapon_nova") == 0 ||
            std::strcmp(name, "weapon_xm1014") == 0;
        auto *clip = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) + fields_.clip1);
        auto *reserve = reinterpret_cast<int *>(
            reinterpret_cast<char *>(weapon) + fields_.reserve_ammo);
        if (*clip < 0 || *clip > 1000 || *reserve < 0 || *reserve > 10000) continue;
        found = true;
        auto &tracked = overload_weapons_[handle.ToInt()];
        const bool transferred = tracked.owner_slot != 0 && tracked.owner_slot != slot;
        if (transferred) {
            tracked.reserve_round_credit = 0;
            tracked.last_clip = 0;
            tracked.last_reserve = 0;
            tracked.pending_shots = 0;
            tracked.shell_reload_next_at = 0.0f;
        }
        const bool first_seen = tracked.owner_slot == 0 || transferred;
        const int previous_overload = transferred ? 0 : tracked.overload_stage;
        const int previous_stockpile = transferred ? 0 : tracked.stockpile_stage;
        const int previous_overload_percent = tracked.overload_percent;
        const int previous_stockpile_clips = tracked.stockpile_clips;
        int desired_clip = *clip;
        int desired_reserve = *reserve;
        if (transferred) {
            desired_reserve = (std::min)(base_reserve,
                desired_reserve + (clips ? 0 : (std::max)(0, desired_clip - base_clip)));
            desired_clip = (std::min)(base_clip, desired_clip);
        }
        const auto clip_bonus = [base_clip](int level, int percent) {
            return level > 0 && percent > 0
                ? (std::max)(1, base_clip * level * percent / 100) : 0;
        };
        const auto reserve_bonus = [base_reserve, clips, single_shells](int level, int percent) {
            // Whole reserve clips belong to Stockpile. Scaling their count
            // here makes Overload silently grant additional clips.
            return clips || single_shells ? 0 : base_reserve * level * percent / 100;
        };
        const auto stockpile_bonus = [base_clip, clips](int level, int clips_per_stage) {
            return level * clips_per_stage * (clips ? 1 : base_clip);
        };
        const int maximum_clip = base_clip + clip_bonus(overload_stage, overload_percent);
        const int maximum_reserve = base_reserve + reserve_bonus(overload_stage, overload_percent) +
                                    stockpile_bonus(stockpile_stage, stockpile_clips);
        if (first_seen) {
            // Reapply the capacity bonus to a weapon kept into a new round.
            desired_clip = (std::min)(maximum_clip,
                desired_clip + clip_bonus(overload_stage, overload_percent));
            desired_reserve = (std::min)(maximum_reserve,
                desired_reserve + reserve_bonus(overload_stage, overload_percent) +
                stockpile_bonus(stockpile_stage, stockpile_clips));
        } else {
            const int clip_increase = clip_bonus(overload_stage, overload_percent) -
                                      clip_bonus(previous_overload, previous_overload_percent);
            const int reserve_increase = reserve_bonus(overload_stage, overload_percent) +
                stockpile_bonus(stockpile_stage, stockpile_clips) -
                reserve_bonus(previous_overload, previous_overload_percent) -
                stockpile_bonus(previous_stockpile, previous_stockpile_clips);
            if (clip_increase > 0)
                desired_clip = (std::min)(maximum_clip, desired_clip + clip_increase);
            if (reserve_increase > 0)
                desired_reserve = (std::min)(maximum_reserve,
                    desired_reserve + reserve_increase);
        }
        if (overload_stage < previous_overload || stockpile_stage < previous_stockpile ||
            overload_percent < previous_overload_percent ||
            stockpile_clips < previous_stockpile_clips) {
            desired_reserve = (std::min)(maximum_reserve,
                desired_reserve + (clips ? 0 : (std::max)(0, desired_clip - maximum_clip)));
            desired_clip = (std::min)(maximum_clip, desired_clip);
            if (overload_stage == 0) tracked.reserve_round_credit = 0;
        }
        bool reloading = *reinterpret_cast<const bool *>(
            base + fields_.weapon_in_reload);
        if (single_shells && reloading && !tracked.reloading &&
            tracked.shell_reload_next_at <= 0.0f &&
            tracked.owner_slot == slot && overload_stage > 0) {
            // CS2 can start a shotgun reload without an explicit reload input
            // (for example at an empty tube). Move it to the same timed path
            // before its stock-capacity reload starts rewriting the clip.
            desired_clip = (std::min)(maximum_clip, (std::max)(0,
                tracked.last_clip - tracked.pending_shots));
            desired_reserve = tracked.last_reserve;
            if (desired_clip < maximum_clip && desired_reserve > 0 && now > 0.0f) {
                tracked.reload_start_clip = desired_clip;
                tracked.reload_start_reserve = desired_reserve;
                tracked.shell_reload_clip = desired_clip;
                tracked.shell_reload_reserve = desired_reserve;
                tracked.shell_reload_next_at = now + 0.6f *
                    (std::max)(0.1f, reload_scale);
            }
            if (!mark_state_changed(weapon, fields_.weapon_in_reload)) return false;
            *reinterpret_cast<bool *>(reinterpret_cast<char *>(weapon) +
                fields_.weapon_in_reload) = false;
            reloading = false;
        }
        if (reloading && !tracked.reloading) {
            const bool was_full = overload_stage > 0 && tracked.owner_slot == slot &&
                tracked.last_clip >= maximum_clip;
            tracked.reload_start_clip = was_full ? tracked.last_clip : desired_clip;
            tracked.reload_start_reserve = was_full ? tracked.last_reserve : desired_reserve;
        }
        if (single_shells && overload_stage > 0 &&
            (reloading || tracked.reloading) &&
            tracked.reload_start_clip < base_clip &&
            desired_clip >= base_clip && desired_clip < maximum_clip &&
            desired_reserve > 0 && now > 0.0f &&
            tracked.shell_reload_next_at <= 0.0f) {
            // Stop the native loader at its stock tube capacity. Waiting for
            // its reload flag to clear can leave it looping at eight shells.
            tracked.shell_reload_clip = desired_clip;
            tracked.shell_reload_reserve = desired_reserve;
            tracked.shell_reload_next_at = now + 0.6f *
                (std::max)(0.1f, reload_scale);
        }
        const bool manual_shell_reload = tracked.shell_reload_next_at > 0.0f;
        if (manual_shell_reload) {
            const bool interrupted = handle.ToInt() != active_weapon.ToInt() ||
                tracked.pending_shots > 0 || overload_stage == 0;
            desired_clip = (std::min)(maximum_clip, (std::max)(0,
                tracked.shell_reload_clip - tracked.pending_shots));
            desired_reserve = (std::min)(maximum_reserve,
                tracked.shell_reload_reserve);
            if (!interrupted && now >= tracked.shell_reload_next_at) {
                const float shell_time = 0.6f * (std::max)(0.1f, reload_scale);
                const int elapsed = 1 + static_cast<int>(
                    (now - tracked.shell_reload_next_at) / shell_time);
                const int transfer = (std::min)({elapsed,
                    maximum_clip - desired_clip, desired_reserve});
                desired_clip += transfer;
                desired_reserve -= transfer;
                tracked.shell_reload_clip = desired_clip;
                tracked.shell_reload_reserve = desired_reserve;
                tracked.shell_reload_next_at += shell_time * transfer;
            }
            if (interrupted || desired_clip >= maximum_clip || desired_reserve == 0)
                tracked.shell_reload_next_at = 0.0f;
            if (reloading) {
                if (!mark_state_changed(weapon, fields_.weapon_in_reload)) return false;
                *reinterpret_cast<bool *>(reinterpret_cast<char *>(weapon) +
                    fields_.weapon_in_reload) = false;
                reloading = false;
            }
        }
        if (overload_stage > 0 && tracked.owner_slot == slot &&
            previous_overload == overload_stage && !reloading && !tracked.reloading &&
            tracked.last_clip > base_clip && desired_clip <= base_clip &&
            desired_clip >= base_clip - 1 &&
            tracked.last_clip - desired_clip > 1) {
            // CS2 can restore its base magazine after an Overload reload or
            // shot, including a shotgun shot from an overfilled tube. The
            // weapon_fire event tells us how many rounds were actually spent.
            desired_clip = (std::max)(0, tracked.last_clip - tracked.pending_shots);
        }
        if (!single_shells && tracked.reloading &&
            (overload_stage > 0 || stockpile_stage > 0)) {
            const int loaded = (std::max)(0, desired_clip - tracked.reload_start_clip);
            const int expected = tracked.reload_start_reserve -
                (clips ? (loaded > 0 ? 1 : 0) : loaded);
            desired_reserve = (std::max)(desired_reserve,
                (std::min)(maximum_reserve, (std::max)(0, expected)));
        }
        const bool full_magazine_reload = !single_shells &&
            (reloading || tracked.reloading) &&
            overload_stage > 0 &&
            tracked.reload_start_clip >= maximum_clip;
        if (full_magazine_reload) {
            // CS2 accepts reload even when the Overload magazine is already
            // full, then replaces it with the weapon's smaller base magazine.
            desired_clip = maximum_clip;
            desired_reserve = (std::min)(maximum_reserve, tracked.reload_start_reserve);
        } else if (!single_shells && tracked.reloading &&
            (desired_clip > tracked.reload_start_clip || !reloading) &&
            overload_stage > 0 && desired_clip >= base_clip &&
            desired_clip < maximum_clip) {
            // CS2 can refill the base magazine before clearing m_bInReload.
            // Add Overload on the first refill tick, before the next snapshot.
            if (clips && tracked.reserve_round_credit < maximum_clip - desired_clip &&
                desired_reserve > 0) {
                --desired_reserve;
                tracked.reserve_round_credit += base_clip;
            }
            const int transfer = (std::min)(maximum_clip - desired_clip,
                clips ? tracked.reserve_round_credit : desired_reserve);
            desired_clip += transfer;
            if (clips) tracked.reserve_round_credit -= transfer;
            else desired_reserve -= transfer;
        }
        if (desired_clip != *clip) {
            if (!mark_state_changed(weapon, fields_.clip1)) return false;
            *clip = desired_clip;
        }
        if (desired_reserve != *reserve) {
            if (!mark_state_changed(weapon, fields_.reserve_ammo)) return false;
            *reserve = desired_reserve;
        }
        tracked.owner_slot = slot;
        tracked.overload_stage = overload_stage;
        tracked.stockpile_stage = stockpile_stage;
        tracked.overload_percent = overload_percent;
        tracked.stockpile_clips = stockpile_clips;
        tracked.reloading = reloading;
        tracked.last_clip = desired_clip;
        tracked.last_reserve = desired_reserve;
        tracked.pending_shots = 0;
    }
    return found;
}

void EngineAdapter::record_weapon_fire(int slot) {
    if (slot < 1 || slot > 64) return;
    CEntityInstance *entity = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!entity || !system) return;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(entity) + fields_.weapon_services);
    if (!services) return;
    const auto &handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    auto tracked = overload_weapons_.find(handle.ToInt());
    if (tracked != overload_weapons_.end() && tracked->second.owner_slot == slot &&
        tracked->second.overload_stage > 0 && tracked->second.pending_shots < 8)
        ++tracked->second.pending_shots;
}

bool EngineAdapter::apply_dexterity(int slot, int stage, int speed_percent,
                                   float now, int server_tick) {
    if (slot < 1 || slot > 64 || stage < 0 || stage > 5) return false;
    auto &tracked = dexterity_[slot];
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !system || server_tick < 0) { tracked = {}; return false; }
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    if (!services) { tracked = {}; return false; }
    const float time_scale = 1.0f - speed_percent / 100.0f * stage;
    const CEntityHandle handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    if (!weapon || !weapon->m_pEntity) { tracked = {}; return false; }
    const bool switched = tracked.weapon != handle.ToInt();
    if (switched) {
        tracked = {};
        tracked.weapon = handle.ToInt();
    }
    const bool reloading = *reinterpret_cast<const bool *>(
        reinterpret_cast<const char *>(weapon) + fields_.weapon_in_reload);
    const bool new_reload = reloading && !tracked.reloading;
    auto *next_attack = reinterpret_cast<float *>(
        reinterpret_cast<char *>(services) + fields_.next_attack_time);
    if (stage == 0) {
        tracked.reloading = reloading;
        tracked.reload_finish_at = 0.0f;
        return true;
    }
    if (switched && !reloading) {
        const float remaining = std::isfinite(*next_attack) ? *next_attack - now : 0.0f;
        if (remaining > 0.0f && remaining < 4.0f) {
            if (mark_state_changed(player, fields_.weapon_services))
                *next_attack = now + remaining * time_scale;
        }
        for (const uint32_t offset : {fields_.primary_attack_tick,
                                      fields_.secondary_attack_tick}) {
            auto *finish = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) + offset);
            const int remaining_ticks = *finish - server_tick;
            if (remaining_ticks > 0 && remaining_ticks < 256 &&
                mark_state_changed(weapon, offset))
                *finish = server_tick + static_cast<int>(std::ceil(
                    remaining_ticks * time_scale));
        }
    }
    if (!reloading) {
        tracked.reloading = false;
        tracked.reload_finish_at = 0.0f;
        return true;
    }
    tracked.reloading = true;
    if (new_reload) {
        const float remaining = std::isfinite(*next_attack)
            ? *next_attack - now : 0.0f;
        if (remaining > 0.0f && remaining < 10.0f)
            tracked.reload_finish_at = now + remaining * time_scale;
    }
    if (tracked.reload_finish_at <= 0.0f || now < tracked.reload_finish_at)
        return true;
    const char *base = reinterpret_cast<const char *>(weapon);
    const void *vdata = *reinterpret_cast<void *const *>(base + fields_.subclass_id + 4);
    const char *name = weapon->m_pEntity->GetClassname();
    if (!vdata || *reinterpret_cast<const bool *>(
            reinterpret_cast<const char *>(vdata) + fields_.reloads_single_shells) ||
        (name && (std::strcmp(name, "weapon_nova") == 0 ||
                  std::strcmp(name, "weapon_xm1014") == 0))) {
        return true;
    }
    const int base_clip = *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(vdata) + fields_.primary_clip_max);
    const auto ammo = overload_weapons_.find(handle.ToInt());
    const int overload_stage = ammo == overload_weapons_.end()
        ? 0 : ammo->second.overload_stage;
    const int maximum_clip = base_clip + (overload_stage > 0 &&
        ammo->second.overload_percent > 0
        ? (std::max)(1, base_clip * overload_stage * ammo->second.overload_percent / 100) : 0);
    const bool clips = *reinterpret_cast<const bool *>(
        reinterpret_cast<const char *>(vdata) + fields_.reserve_as_clips);
    auto *clip = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) + fields_.clip1);
    auto *reserve = reinterpret_cast<int *>(
        reinterpret_cast<char *>(weapon) + fields_.reserve_ammo);
    if (base_clip < 1 || base_clip > 200 || *clip < 0 || *clip > maximum_clip ||
        *reserve < 0 || *reserve > 10000) {
        return false;
    }
    const int loaded = (std::min)(maximum_clip - *clip, clips ?
        (*reserve > 0 ? base_clip : 0) : *reserve);
    if (loaded > 0) {
        if (!mark_state_changed(weapon, fields_.clip1) ||
            !mark_state_changed(weapon, fields_.reserve_ammo)) return false;
        *clip += loaded;
        const bool clip_already_spent = ammo != overload_weapons_.end() &&
            *clip - loaded > ammo->second.reload_start_clip;
        *reserve -= clips ? (clip_already_spent ? 0 : 1) : loaded;
    }
    mark_state_changed(weapon, fields_.weapon_in_reload);
    *reinterpret_cast<bool *>(reinterpret_cast<char *>(weapon) +
        fields_.weapon_in_reload) = false;
    if (std::isfinite(*next_attack) && *next_attack > now &&
        mark_state_changed(player, fields_.weapon_services))
        *next_attack = now;
    for (const uint32_t offset : {fields_.primary_attack_tick,
                                  fields_.secondary_attack_tick}) {
        auto *finish = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) + offset);
        if (*finish > server_tick && mark_state_changed(weapon, offset))
            *finish = server_tick;
    }
    tracked.reloading = false;
    tracked.reload_finish_at = 0.0f;
    return true;
}

bool EngineAdapter::apply_claw_timing(int slot, float now, int server_tick, bool active,
                                      int speed_percent, bool *attacked) {
    if (attacked) *attacked = false;
    if (slot < 1 || slot > 64) return false;
    auto &tracked = claws_[slot];
    if (!active) { tracked = {}; return true; }
    if (speed_percent < 1 || speed_percent > 100) { tracked = {}; return false; }
    const float speed_scale = speed_percent / 100.0f;
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !system || server_tick < 0) { tracked = {}; return false; }
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    if (!services) { tracked = {}; return false; }
    const CEntityHandle handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    const char *name = weapon && weapon->m_pEntity ? weapon->m_pEntity->GetClassname() : nullptr;
    if (!name || !std::strstr(name, "knife")) { tracked = {}; return false; }
    auto *primary = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) +
        fields_.primary_attack_tick);
    auto *secondary = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) +
        fields_.secondary_attack_tick);
    auto *next_attack = reinterpret_cast<float *>(reinterpret_cast<char *>(services) +
        fields_.next_attack_time);
    if (tracked.weapon != handle.ToInt()) {
        tracked = {handle.ToInt(), *primary, *secondary, *next_attack};
        return true;
    }
    bool attack_detected = false;
    for (auto &[current, previous, offset] : {
            std::tuple<int *, int *, uint32_t> {primary, &tracked.primary_tick,
                                                 fields_.primary_attack_tick},
            std::tuple<int *, int *, uint32_t> {secondary, &tracked.secondary_tick,
                                                 fields_.secondary_attack_tick}}) {
        const int remaining = *current - server_tick;
        if (*current != *previous && remaining > 1 && remaining <= 128) {
            attack_detected = true;
            const int slowed = server_tick + static_cast<int>(
                std::ceil(static_cast<float>(remaining) / speed_scale));
            if (slowed > *current && mark_state_changed(weapon, offset)) *current = slowed;
        }
        *previous = *current;
    }
    if (attack_detected && std::isfinite(*next_attack) && *next_attack > now &&
        *next_attack < now + 2.0f && *next_attack != tracked.next_attack &&
        mark_state_changed(player, fields_.weapon_services))
        *next_attack = now + (*next_attack - now) / speed_scale;
    tracked.next_attack = *next_attack;
    if (attacked) *attacked = attack_detected;
    return true;
}

bool EngineAdapter::claw_sweep(int attacker_slot, int armitive_team, float reach,
                               float cone_width, int &target_slot) const {
    target_slot = 0;
#if defined(_WIN32) || defined(__linux__)
    CEntityInstance *attacker = pawn(attacker_slot);
    Activity activity;
    if (!ready_ || !attacker || !sample_activity(attacker_slot, activity) ||
        (armitive_team != 2 && armitive_team != 3) ||
        !std::isfinite(reach) || !std::isfinite(cone_width) ||
        reach < 0.0f || reach > 16384.0f || cone_width < 0.0f || cone_width > 360.0f)
        return false;
    if (reach == 0.0f) return true;
    auto **vtable = reinterpret_cast<void **>(
        reinterpret_cast<char *>(server_module_) + kNavPhysicsVtableRva);
    void *function = vtable[kTraceShapeFilterVtable];
    if (!executable_in_module(function, server_module_)) return false;
    const float eye_height = activity.crouched ? 47.0f : 64.0f;
    const Vector start(activity.position[0], activity.position[1],
                       activity.position[2] + eye_height);
    const float pitch = activity.angles[0] * 0.01745329251994329577f;
    const float yaw = activity.angles[1] * 0.01745329251994329577f;
    const std::array<float, 3> forward {
        std::cos(pitch) * std::cos(yaw), std::cos(pitch) * std::sin(yaw), -std::sin(pitch)
    };
    const Ray_t line;
    float nearest = std::numeric_limits<float>::infinity();
    for (int slot = 1; slot <= shroudfire::kMaxPlayers; ++slot) {
        CEntityInstance *candidate = pawn(slot);
        Activity target;
        if (slot == attacker_slot || !candidate || !player_alive(slot) ||
            *reinterpret_cast<const uint8_t *>(reinterpret_cast<const char *>(candidate) +
                fields_.team) != armitive_team || !sample_activity(slot, target)) continue;

        // Player bodies have a 16-unit radius. Check the vertical body axis,
        // including its best angle toward the aim, so looking up or down and
        // crouching do not leave holes in the cone.
        const float bottom = target.position[2] + 16.0f;
        const float top = target.position[2] + (target.crouched ? 54.0f : 72.0f) - 16.0f;
        const float dx = target.position[0] - start.x;
        const float dy = target.position[1] - start.y;
        const float horizontal_dot = dx * forward[0] + dy * forward[1];
        const float closest_height = std::clamp(start.z, bottom, top);
        const float aim_height = horizontal_dot > 0.001f
            ? std::clamp(start.z + (dx * dx + dy * dy) * forward[2] / horizontal_dot,
                         bottom, top) : closest_height;
        for (const float height : {closest_height, aim_height, bottom, top}) {
            const std::array<float, 3> offset {dx, dy, height - start.z};
            const float distance = std::sqrt(dx * dx + dy * dy + offset[2] * offset[2]);
            if (distance >= nearest ||
                !shroudfire::knife_cone_intersects(offset, forward, 16.0f, reach, cone_width))
                continue;
            const Vector end(target.position[0], target.position[1], height);
            CTraceFilter filter(attacker, nullptr, 0xffff, MASK_SHOT,
                                COLLISION_GROUP_DEFAULT, true);
            filter.m_nInteractsAs = CONTENTS_PLAYER;
            CGameTrace trace;
            void *nav_interface = vtable;
            reinterpret_cast<void (*)(void *, const Ray_t &, const Vector &, const Vector &,
                                       CTraceFilter *, CGameTrace *)>(function)(
                &nav_interface, line, start, end, &filter, &trace);
            // A wall or another player blocks this candidate. Only one visible
            // target, the nearest body in the cone, is hit by a swing.
            if (!trace.m_bStartInSolid && trace.m_pEnt == candidate) {
                nearest = distance;
                target_slot = slot;
            }
        }
    }
    return true;
#else
    (void)attacker_slot; (void)armitive_team; (void)reach; (void)cone_width;
    return false;
#endif
}

bool EngineAdapter::claw_damage(int attacker_slot, int victim_slot, int damage) {
#if defined(_WIN32) || defined(__linux__)
    if (!ready_ || !point_hurt_fields_ok_ || !accept_input_function_ ||
        !create_entity_function_ || !dispatch_spawn_function_ ||
        damage <= 0 || damage > 1000) return false;
    CEntityInstance *attacker = pawn(attacker_slot);
    CEntityInstance *victim = pawn(victim_slot);
    Activity position;
    if (!attacker || !victim || !player_alive(victim_slot) ||
        !sample_activity(victim_slot, position)) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return false;
    CEntityInstance *hurt = claw_hurt_entity_
        ? system->GetEntityInstance(CEntityHandle(claw_hurt_entity_)) : nullptr;
    if (!hurt || !hurt->m_pEntity) {
        claw_hurt_entity_ = 0;
        hurt = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
            create_entity_function_)("point_hurt", -1);
        if (!hurt || !hurt->m_pEntity) return false;
        reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(hurt, nullptr);
        claw_hurt_entity_ = hurt->m_pEntity->GetRefEHandle().ToInt();
    }
    void *teleport = (*reinterpret_cast<void ***>(hurt))[kTeleportVtable];
    if (!executable_in_module(teleport, server_module_)) return false;
    auto *base = reinterpret_cast<char *>(hurt);
    *reinterpret_cast<int *>(base + fields_.point_hurt_damage) = damage;
    *reinterpret_cast<uint32_t *>(base + fields_.point_hurt_type) = 1u << 2; // DMG_SLASH
    *reinterpret_cast<float *>(base + fields_.point_hurt_radius) = 1.0f;
    static constexpr char kVictimName[] = "shroudfire_claw_victim";
    static constexpr char kVictimWildcard[] = "*shroudfire_claw_victim*";
    const CUtlSymbolLarge previous_name = victim->m_pEntity->m_name;
    victim->m_pEntity->m_name = CUtlSymbolLarge(kVictimName);
    *reinterpret_cast<CUtlSymbolLarge *>(base + fields_.point_hurt_target) =
        CUtlSymbolLarge(kVictimWildcard);
    const float center[3] {position.position[0], position.position[1],
                           position.position[2] + 48.0f};
    reinterpret_cast<void (*)(void *, const void *, const void *, const void *)>(
        teleport)(hurt, center, nullptr, nullptr);
    variant_t value;
    const int health_before = *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(victim) + fields_.health);
    reinterpret_cast<void (*)(void *, const char *, void *, void *, variant_t *, int, void *)>(
        accept_input_function_)(hurt, "Hurt", attacker, attacker, &value, 0, nullptr);
    victim->m_pEntity->m_name = previous_name;
    const int health_after = *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(victim) + fields_.health);
    return health_after < health_before;
#else
    (void)attacker_slot; (void)victim_slot; (void)damage;
    return false;
#endif
}

void EngineAdapter::remove_claw_entity() {
#if defined(_WIN32) || defined(__linux__)
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *hurt = system && claw_hurt_entity_
        ? system->GetEntityInstance(CEntityHandle(claw_hurt_entity_)) : nullptr;
    if (hurt && remove_entity_function_)
        reinterpret_cast<void (*)(void *)>(remove_entity_function_)(hurt);
#endif
    claw_hurt_entity_ = 0;
}

bool EngineAdapter::update_armitive_glow(int target_slot, bool shielded,
                                       bool through_walls, GlowPair &pair) {
#if defined(_WIN32) || defined(__linux__)
    if (!glow_ready() ||
        !accept_input_function_ || !create_entity_function_ ||
        !dispatch_spawn_function_ || !remove_entity_function_ ||
        target_slot < 1 || target_slot > 64 || !player_alive(target_slot)) return false;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *target = pawn(target_slot);
    if (!system || !target || !target->m_pEntity) return false;
    const void *body = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(target) + fields_.body_component);
    const void *scene = body ? *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(body) + fields_.scene_node) : nullptr;
    if (!scene) return false;
    const auto &model_symbol = *reinterpret_cast<const CUtlSymbolLarge *>(
        reinterpret_cast<const char *>(scene) + fields_.model_state + fields_.model_name);
    const char *name = model_symbol.String();
    if (!name || (std::strncmp(name, "models/", 7) != 0 &&
                  std::strncmp(name, "characters/", 11) != 0 &&
                  std::strncmp(name, "agents/", 7) != 0) ||
        std::strlen(name) > 260) return false;
    const uint32_t pawn_handle = target->m_pEntity->GetRefEHandle().ToInt();
    CEntityInstance *existing_glow = pair.glow
        ? system->GetEntityInstance(CEntityHandle(pair.glow)) : nullptr;
    if (pair.relay && existing_glow && pair.pawn == pawn_handle && pair.model == name &&
        system->GetEntityInstance(CEntityHandle(pair.relay))) {
        if (pair.shielded != shielded &&
            mark_state_changed(existing_glow, fields_.glow_property)) {
            const uint8_t outline[4] {
                static_cast<uint8_t>(shielded ? 75 : 255),
                static_cast<uint8_t>(shielded ? 255 : 113),
                static_cast<uint8_t>(shielded ? 105 : 105), 255
            };
            std::memcpy(reinterpret_cast<char *>(existing_glow) + fields_.glow_property +
                        fields_.glow_color, outline, sizeof(outline));
            pair.shielded = shielded;
        }
        return true;
    }
    remove_armitive_glow(pair);

    auto create_prop = [&](bool relay) -> CEntityInstance * {
        auto *entity = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
            create_entity_function_)("prop_dynamic", -1);
        if (!entity || !entity->m_pEntity) return nullptr;
        auto *keys = static_cast<CEntityKeyValues *>(::operator new(sizeof(CEntityKeyValues)));
        reinterpret_cast<void (*)(CEntityKeyValues *, CKV3Arena *, EntityKVAllocatorType_t)>(
            keyvalues_constructor_function_)(keys, nullptr, EKV_ALLOCATOR_NORMAL);
        const CKV3MemberName model_key("model");
        KeyValues3 *model_value = reinterpret_cast<KeyValues3 *(*)(
            CEntityKeyValues *, const CKV3MemberName *, const char *)>(
                keyvalues_find_or_create_function_)(keys, &model_key, nullptr);
        if (!model_value) {
            reinterpret_cast<void (*)(void *)>(remove_entity_function_)(entity);
            return nullptr;
        }
        reinterpret_cast<void (*)(CEntityKeyValues *, KeyValues3 *, const char *)>(
            keyvalues_set_string_function_)(keys, model_value, name);
        auto *base = reinterpret_cast<char *>(entity);
        *reinterpret_cast<uint32_t *>(base + fields_.spawn_flags) = 256;
        if (relay) *reinterpret_cast<uint8_t *>(base + fields_.render_mode) = 10;
        reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(entity, keys);
        return entity;
    };
    CEntityInstance *relay = create_prop(true);
    if (!relay) return false;
    CEntityInstance *glow = create_prop(false);
    if (!glow) {
        reinterpret_cast<void (*)(void *)>(remove_entity_function_)(relay);
        return false;
    }
    auto *base = reinterpret_cast<char *>(glow);
    auto *properties = base + fields_.glow_property;
    const uint8_t outline[4] {
        static_cast<uint8_t>(shielded ? 75 : 255),
        static_cast<uint8_t>(shielded ? 255 : 113),
        static_cast<uint8_t>(shielded ? 105 : 105), 255
    };
    const uint8_t transparent[4] {255, 255, 255, 1};
    std::memcpy(properties + fields_.glow_color, outline, sizeof(outline));
    std::memcpy(base + fields_.render_color, transparent, sizeof(transparent));
    // Keep the tracking outline through walls; Ward's teammate outline uses depth testing.
    *reinterpret_cast<int *>(properties + fields_.glow_type) = through_walls ? 3 : 1;
    *reinterpret_cast<int *>(properties + fields_.glow_team) = -1;
    *reinterpret_cast<int *>(properties + fields_.glow_range) = 5000;
    *reinterpret_cast<int *>(properties + fields_.glow_range_min) = 0;
    mark_state_changed(glow, fields_.glow_property);
    variant_t follow("!activator");
    auto accept = reinterpret_cast<void (*)(void *, const char *, void *, void *,
                                            variant_t *, int, void *)>(accept_input_function_);
    accept(relay, "FollowEntity", target, relay, &follow, 0, nullptr);
    accept(glow, "FollowEntity", relay, glow, &follow, 0, nullptr);
    pair.pawn = pawn_handle;
    pair.relay = relay->m_pEntity->GetRefEHandle().ToInt();
    pair.glow = glow->m_pEntity->GetRefEHandle().ToInt();
    pair.relay_index = relay->m_pEntity->GetEntityIndex().Get();
    pair.glow_index = glow->m_pEntity->GetEntityIndex().Get();
    pair.shielded = shielded;
    pair.model = name;
    return true;
#else
    (void)target_slot; (void)shielded; (void)through_walls; (void)pair;
    return false;
#endif
}

void EngineAdapter::remove_armitive_glow(GlowPair &pair) {
#if defined(_WIN32) || defined(__linux__)
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (system && remove_entity_function_) {
        for (const uint32_t handle : {pair.glow, pair.relay}) {
            CEntityInstance *entity = handle
                ? system->GetEntityInstance(CEntityHandle(handle)) : nullptr;
            if (entity)
                reinterpret_cast<void (*)(void *)>(remove_entity_function_)(entity);
        }
    }
#endif
    pair = {};
}

void EngineAdapter::begin_control(int slot, int stage, int reduction_percent,
                                  float now, int server_tick) {
    if (slot < 1 || slot > 64) return;
    control_[slot] = {};
    if (stage < 1 || stage > 5 || server_tick < 0) return;
    CEntityInstance *player = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!player || !player->m_pEntity || !system) return;
    Activity activity;
    if (!sample_activity(slot, activity) || !(activity.buttons & 1u)) return;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.weapon_services);
    void *aim = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.aim_punch_services);
    if (!services || !aim) return;
    const auto &handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    if (!weapon || !weapon->m_pEntity) return;
    void *vdata = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(weapon) + fields_.subclass_id + 4);
    if (!vdata) return;
    const uint32_t gear_slot = *reinterpret_cast<const uint32_t *>(
        reinterpret_cast<const char *>(vdata) + fields_.gear_slot);
    if (gear_slot > 1) return;
    const int clip = *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(weapon) + fields_.clip1);
    const int next_tick = *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(weapon) + fields_.primary_attack_tick);
    const float next_attack = *reinterpret_cast<const float *>(
        reinterpret_cast<const char *>(services) + fields_.next_attack_time);
    const bool reloading = *reinterpret_cast<const bool *>(
        reinterpret_cast<const char *>(weapon) + fields_.weapon_in_reload);
    if (clip < 1 || clip > 1000 || next_tick > server_tick ||
        !std::isfinite(next_attack) || next_attack > now || reloading) return;
    auto *penalty = reinterpret_cast<float *>(
        reinterpret_cast<char *>(weapon) + fields_.accuracy_penalty);
    if (!std::isfinite(*penalty) || *penalty < 0.0f || *penalty > 10.0f) return;
    ControlSnapshot snapshot;
    snapshot.pawn = player->m_pEntity->GetRefEHandle().ToInt();
    snapshot.weapon = handle.ToInt();
    snapshot.clip = clip;
    snapshot.reduction = reduction_percent / 100.0f * stage;
    snapshot.original_penalty = *penalty;
    std::memcpy(snapshot.angle.data(), reinterpret_cast<char *>(aim) + fields_.aim_punch_angle,
                sizeof(float) * 3);
    std::memcpy(snapshot.velocity.data(),
                reinterpret_cast<char *>(aim) + fields_.aim_punch_velocity, sizeof(float) * 3);
    if (!mark_state_changed(weapon, fields_.accuracy_penalty)) return;
    *penalty *= 1.0f - snapshot.reduction;
    snapshot.valid = true;
    control_[slot] = snapshot;
}

void EngineAdapter::finish_control(int slot) {
    if (slot < 1 || slot > 64) return;
    const ControlSnapshot snapshot = control_[slot];
    control_[slot] = {};
    if (!snapshot.valid) return;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return;
    CEntityInstance *player = system->GetEntityInstance(CEntityHandle(snapshot.pawn));
    CEntityInstance *weapon = system->GetEntityInstance(CEntityHandle(snapshot.weapon));
    if (!player || !weapon || !player->m_pEntity || !weapon->m_pEntity) return;
    const float reduction = snapshot.reduction;
    auto *penalty = reinterpret_cast<float *>(
        reinterpret_cast<char *>(weapon) + fields_.accuracy_penalty);
    if (std::isfinite(*penalty) && mark_state_changed(weapon, fields_.accuracy_penalty))
        *penalty = (std::max)(0.0f, *penalty + snapshot.original_penalty * reduction);
    const int clip = *reinterpret_cast<const int *>(
        reinterpret_cast<const char *>(weapon) + fields_.clip1);
    if (clip >= snapshot.clip || clip < 0) return;
    void *aim = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(player) + fields_.aim_punch_services);
    if (!aim || !mark_state_changed(player, fields_.aim_punch_services)) return;
    for (int axis = 0; axis < 3; ++axis) {
        auto *angle = reinterpret_cast<float *>(reinterpret_cast<char *>(aim) +
            fields_.aim_punch_angle) + axis;
        auto *velocity = reinterpret_cast<float *>(reinterpret_cast<char *>(aim) +
            fields_.aim_punch_velocity) + axis;
        if (std::isfinite(*angle) && std::isfinite(snapshot.angle[axis]))
            *angle = snapshot.angle[axis] + (*angle - snapshot.angle[axis]) * (1.0f - reduction);
        if (std::isfinite(*velocity) && std::isfinite(snapshot.velocity[axis]))
            *velocity = snapshot.velocity[axis] +
                (*velocity - snapshot.velocity[axis]) * (1.0f - reduction);
    }
}

bool EngineAdapter::accelerate_fire(int slot, float fire_delay_scale, int server_tick) const {
    if (fire_delay_scale <= 0.0f || fire_delay_scale >= 1.0f || server_tick < 0) return false;
    CEntityInstance *entity = pawn(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!entity || !system) return false;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(entity) + fields_.weapon_services);
    if (!services) return false;
    const auto &handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.active_weapon);
    CEntityInstance *weapon = system->GetEntityInstance(handle);
    if (!weapon || !weapon->m_pEntity) return false;
    const char *base = reinterpret_cast<const char *>(weapon);
    void *vdata = *reinterpret_cast<void *const *>(base + fields_.subclass_id + 4);
    if (!vdata || *reinterpret_cast<const int *>(
            reinterpret_cast<const char *>(vdata) + fields_.primary_clip_max) <= 0) return false;
    const uint32_t gear_slot = *reinterpret_cast<const uint32_t *>(
        reinterpret_cast<const char *>(vdata) + fields_.gear_slot);
    if (gear_slot > 1) return false;
    auto *next_tick = reinterpret_cast<int *>(reinterpret_cast<char *>(weapon) +
                                             fields_.primary_attack_tick);
    const int remaining = *next_tick - server_tick;
    if (remaining <= 1 || remaining > 128) return false;
    const int accelerated = server_tick + (std::max)(1,
        static_cast<int>(std::ceil(remaining * fire_delay_scale)));
    if (accelerated >= *next_tick ||
        !mark_state_changed(weapon, fields_.primary_attack_tick)) return false;
    *next_tick = accelerated;
    return true;
}

bool EngineAdapter::set_invisant_speed(int slot, float multiplier) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity || multiplier < 1.0f || multiplier > 2.5f) return false;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(entity) + fields_.movement_services);
    if (!services) return false;
    auto *maxspeed = reinterpret_cast<float *>(reinterpret_cast<char *>(services) + fields_.maxspeed);
    const float target = 250.0f * multiplier;
    if (std::fabs(*maxspeed - target) > 0.01f) *maxspeed = target;
    auto *modifier = reinterpret_cast<float *>(reinterpret_cast<char *>(entity) +
        fields_.velocity_modifier);
    if (std::fabs(*modifier - multiplier) > 0.01f) {
        if (!mark_state_changed(entity, fields_.velocity_modifier)) return false;
        *modifier = multiplier;
    }
    return true;
}

int EngineAdapter::movement_source_slot(const void *services) const {
    if (!services) return 0;
    for (int slot = 1; slot <= 64; ++slot) {
        const CEntityInstance *entity = pawn(slot);
        if (entity && *reinterpret_cast<void *const *>(
                reinterpret_cast<const char *>(entity) + fields_.movement_services) == services)
            return slot;
        if (observer_movement_services(slot) == services) return slot;
    }
    return 0;
}

bool EngineAdapter::suppress_buy_zone(int slot) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity) return false;
    auto *base = reinterpret_cast<char *>(entity);
    for (const uint32_t offset : {fields_.in_buy_zone, fields_.was_in_buy_zone}) {
        auto *value = reinterpret_cast<bool *>(base + offset);
        if (*value) {
            if (!mark_state_changed(entity, offset)) return false;
            *value = false;
        }
    }
    return true;
}

void EngineAdapter::reset_player_buy_zone(int slot) {
    if (slot < 1 || slot > 64) return;
    buyzone_spawn_[slot] = {};
}

void EngineAdapter::record_buy_zone_spawn(int slot,
                                          const std::array<float, 3> &position) {
    if (slot < 1 || slot > 64) return;
    CEntityInstance *entity = pawn(slot);
    if (!entity || !entity->m_pEntity) return;
    const uint32_t handle = entity->m_pEntity->GetRefEHandle().ToInt();
    const bool native_buy_zone = *reinterpret_cast<const bool *>(
        reinterpret_cast<const char *>(entity) + fields_.in_buy_zone);
    if (!native_buy_zone) buyzone_spawn_[slot] = {handle, position};
}

bool EngineAdapter::inspect_damage(void *victim_entity, void *info,
                                   DamageContext &context) const {
    if (!ready_ || !victim_entity || !info) return false;
    for (int slot = 1; slot <= 64; ++slot) {
        if (pawn(slot) == victim_entity) {
            context.victim = slot;
            break;
        }
    }
    if (!context.victim) return false;
    const char *base = reinterpret_cast<const char *>(info);
    const auto &attacker_handle = *reinterpret_cast<const CEntityHandle *>(
        base + fields_.damage_attacker);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    CEntityInstance *attacker_entity = system ? system->GetEntityInstance(attacker_handle) : nullptr;
    if (attacker_entity) {
        for (int slot = 1; slot <= 64; ++slot) {
            if (pawn(slot) == attacker_entity) {
                context.attacker = slot;
                break;
            }
        }
    }
    context.damage = reinterpret_cast<float *>(const_cast<char *>(base) + fields_.damage_amount);
    context.flags = reinterpret_cast<uint32_t *>(const_cast<char *>(base) + fields_.damage_flags);
    context.type = *reinterpret_cast<const uint32_t *>(base + fields_.damage_type);
    context.hitgroup = *reinterpret_cast<const uint32_t *>(base + fields_.damage_hitgroup);
    // Bullet hits can leave m_iHitGroupId invalid. The attached trace carries
    // the actual hit data; m_pTrace sits 16 bytes before m_iHitGroupId in
    // CTakeDamageInfo, and its hit data stores the group at +56.
    if (context.hitgroup != 1 && fields_.damage_hitgroup >= 16) {
        const auto *trace = *reinterpret_cast<const char *const *>(
            base + fields_.damage_hitgroup - 16);
        if (trace) {
            const auto *hit_data = *reinterpret_cast<const char *const *>(trace + 16);
            if (hit_data && *reinterpret_cast<const int32_t *>(hit_data + 56) == 1)
                context.hitgroup = 1;
        }
    }
    return std::isfinite(*context.damage) && *context.damage >= 0.0f;
}

int EngineAdapter::remove_objectives() const {
#if defined(_WIN32) || defined(__linux__)
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system || !remove_entity_function_) return -1;
    int removed = 0;
    for (int index = 65; index < MAX_TOTAL_ENTITIES; ++index) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
        if (!entity || !entity->m_pEntity) continue;
        const char *name = entity->m_pEntity->GetClassname();
        if (!name) continue;
        // Keep map objective triggers in place. CS2's round reset still owns
        // them even when C4 and hostages are absent; deleting those triggers
        // risks stale references on a later round.
        const bool objective = std::strncmp(name, "hostage_", 8) == 0 ||
            std::strcmp(name, "weapon_c4") == 0 ||
            std::strcmp(name, "planted_c4") == 0;
        if (!objective) continue;
        reinterpret_cast<void (*)(void *)>(remove_entity_function_)(entity);
        ++removed;
    }
    return removed;
#else
    return -1;
#endif
}

int EngineAdapter::apply_tonemap(float exposure) {
    if (exposure < 0.05f || exposure > 1.0f) return -1;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return -1;
    int adjusted = 0;
    CEntityInstance *selected = nullptr;
    auto adjust = [&](CEntityInstance *entity) {
        auto *minimum = reinterpret_cast<float *>(
            reinterpret_cast<char *>(entity) + fields_.auto_exposure_min);
        auto *maximum = reinterpret_cast<float *>(
            reinterpret_cast<char *>(entity) + fields_.auto_exposure_max);
        if (!std::isfinite(*minimum) || !std::isfinite(*maximum)) return;
        const uint32_t handle = entity->m_pEntity->GetRefEHandle().ToInt();
        if (handle != created_tonemap_ && original_tonemap_.find(handle) == original_tonemap_.end())
            original_tonemap_.emplace(handle, TonemapOriginal {*minimum, *maximum});
        if (std::fabs(*minimum - exposure) > 0.001f) {
            if (!mark_state_changed(entity, fields_.auto_exposure_min)) return;
            *minimum = exposure;
        }
        if (std::fabs(*maximum - exposure) > 0.001f) {
            if (!mark_state_changed(entity, fields_.auto_exposure_max)) return;
            *maximum = exposure;
        }
        if (!selected) selected = entity;
        ++adjusted;
    };
    for (int index = 65; index < MAX_TOTAL_ENTITIES; ++index) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
        if (!entity || !entity->m_pEntity) continue;
        const char *name = entity->m_pEntity->GetClassname();
        if (!name || (std::strcmp(name, "env_tonemap_controller2") != 0 &&
                      std::strcmp(name, "tonemap_controller2") != 0)) continue;
        adjust(entity);
    }
    if (!selected && !created_tonemap_ && create_entity_function_ && dispatch_spawn_function_) {
        auto *entity = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
            create_entity_function_)("env_tonemap_controller2", -1);
        if (entity && entity->m_pEntity) {
            reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(entity, nullptr);
            created_tonemap_ = entity->m_pEntity->GetRefEHandle().ToInt();
            adjust(entity);
        }
    }
    if (!selected && created_tonemap_) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityHandle(created_tonemap_));
        if (entity && entity->m_pEntity) adjust(entity);
        else created_tonemap_ = 0;
    }
    if (selected && selected->m_pEntity) {
        active_tonemap_ = selected->m_pEntity->GetRefEHandle().ToInt();
        for (int slot = 1; slot <= 64; ++slot) {
            CEntityInstance *player = pawn(slot);
            if (!player || !player->m_pEntity) continue;
            void *services = *reinterpret_cast<void *const *>(
                reinterpret_cast<const char *>(player) + fields_.camera_services);
            if (!services) continue;
            auto *current = reinterpret_cast<CEntityHandle *>(
                reinterpret_cast<char *>(services) + fields_.player_tonemap);
            if (current->ToInt() == active_tonemap_) continue;
            const uint32_t pawn_handle = player->m_pEntity->GetRefEHandle().ToInt();
            if (original_player_tonemap_.find(pawn_handle) == original_player_tonemap_.end())
                original_player_tonemap_.emplace(pawn_handle, current->ToInt());
            if (!mark_state_changed(player, fields_.camera_services)) continue;
            *current = CEntityHandle(active_tonemap_);
        }
    }
    return adjusted;
}

void EngineAdapter::restore_tonemap() {
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) { clear_tonemap_tracking(); return; }
    for (const auto &[pawn_handle, saved] : original_player_tonemap_) {
        CEntityInstance *player = system->GetEntityInstance(CEntityHandle(pawn_handle));
        if (!player || !player->m_pEntity) continue;
        void *services = *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(player) + fields_.camera_services);
        if (!services) continue;
        auto *current = reinterpret_cast<CEntityHandle *>(
            reinterpret_cast<char *>(services) + fields_.player_tonemap);
        if (current->ToInt() == active_tonemap_ &&
            mark_state_changed(player, fields_.camera_services))
            *current = CEntityHandle(saved);
    }
    original_player_tonemap_.clear();
    for (const auto &[handle, saved] : original_tonemap_) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityHandle(handle));
        if (!entity || !entity->m_pEntity) continue;
        auto *minimum = reinterpret_cast<float *>(
            reinterpret_cast<char *>(entity) + fields_.auto_exposure_min);
        auto *maximum = reinterpret_cast<float *>(
            reinterpret_cast<char *>(entity) + fields_.auto_exposure_max);
        if (mark_state_changed(entity, fields_.auto_exposure_min)) *minimum = saved.minimum;
        if (mark_state_changed(entity, fields_.auto_exposure_max)) *maximum = saved.maximum;
    }
    original_tonemap_.clear();
    if (created_tonemap_ && remove_entity_function_) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityHandle(created_tonemap_));
        if (entity) reinterpret_cast<void (*)(void *)>(remove_entity_function_)(entity);
    }
    created_tonemap_ = 0;
    active_tonemap_ = 0;
}

bool EngineAdapter::spawn_counts(int &terrorists, int &counter_terrorists) const {
    terrorists = 0;
    counter_terrorists = 0;
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!system) return false;
    int last_entity = 0;
    std::string candidates;
    for (int index = 65; index < MAX_TOTAL_ENTITIES; ++index) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
        if (!entity || !entity->m_pEntity) continue;
        last_entity = index;
        const char *name = entity->m_pEntity->GetClassname();
        if (!name) continue;
        if (std::strcmp(name, "info_player_terrorist") == 0) ++terrorists;
        else if (std::strcmp(name, "info_player_counterterrorist") == 0)
            ++counter_terrorists;
        else if (candidates.size() < 512 &&
                 (std::strstr(name, "spawn") || std::strstr(name, "info_player"))) {
            if (!candidates.empty()) candidates += ", ";
            candidates += name;
        }
    }
    static bool reported_missing_spawns = false;
    if (!reported_missing_spawns && terrorists == 0 && counter_terrorists == 0) {
        std::fprintf(stderr, "[Shroudfire] No spawn entities found (last entity %d; candidates: %s).\n",
                     last_entity, candidates.c_str());
        reported_missing_spawns = true;
    }
    return true;
}

int EngineAdapter::ensure_spawn_capacity(int terrorist_target, int counter_terrorist_target) {
#if defined(_WIN32) || defined(__linux__)
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!ready_ || !server_module_ || !system ||
        !create_entity_function_ || !dispatch_spawn_function_ ||
        !remove_entity_function_) return -1;
    auto **nav = reinterpret_cast<void **>(
        reinterpret_cast<char *>(server_module_) + kNavPhysicsVtableRva);
    void *trace_function = nav[kTraceShapeFilterVtable];
    void *contents_function = nav[kPointContentsVtable];
    if (!executable_in_module(trace_function, server_module_) ||
        !executable_in_module(contents_function, server_module_)) return -1;
    using TraceShape = void (*)(void *, const Ray_t &, const Vector &, const Vector &,
                                CTraceFilter *, CGameTrace *);
    const auto trace_shape = reinterpret_cast<TraceShape>(trace_function);
    void *nav_interface = nav;
    struct SpawnTraceFilter final : CTraceFilter {
        explicit SpawnTraceFilter(uint64_t mask)
            : CTraceFilter(mask, COLLISION_GROUP_PLAYER_MOVEMENT, true) {
            m_nInteractsAs = CONTENTS_PLAYER;
        }
        bool ShouldHitEntity(CEntityInstance *entity) override {
            const char *name = entity && entity->m_pEntity
                ? entity->m_pEntity->GetClassname() : nullptr;
            return !name || std::strstr(name, "player_pawn") == nullptr;
        }
    };
    struct SpawnPoint {
        Vector position;
        Vector angles;
    };
    std::array<std::vector<SpawnPoint>, 2> points;
    int entities = 0;
    for (int index = 0; index < MAX_TOTAL_ENTITIES; ++index) {
        CEntityInstance *entity = system->GetEntityInstance(CEntityIndex(index));
        if (!entity || !entity->m_pEntity) continue;
        ++entities;
        const char *name = entity->m_pEntity->GetClassname();
        const int team = name && std::strcmp(name, "info_player_terrorist") == 0 ? 0
            : name && std::strcmp(name, "info_player_counterterrorist") == 0 ? 1 : -1;
        if (team < 0 || points[team].size() >= 256) continue;
        const void *body = *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(entity) + fields_.body_component);
        const void *scene = body ? *reinterpret_cast<void *const *>(
            reinterpret_cast<const char *>(body) + fields_.scene_node) : nullptr;
        if (!scene) continue;
        SpawnPoint point;
        std::memcpy(point.position.Base(), reinterpret_cast<const char *>(scene) +
                    fields_.abs_origin, 3 * sizeof(float));
        if (spawn_rotation_ok_)
            std::memcpy(point.angles.Base(), reinterpret_cast<const char *>(scene) +
                        fields_.abs_rotation, 3 * sizeof(float));
        else point.angles = Vector(0.0f, 0.0f, 0.0f);
        points[team].push_back(point);
    }
    int added = 0;
    const int targets[] {terrorist_target, counter_terrorist_target};
    const char *classes[] {"info_player_terrorist", "info_player_counterterrorist"};
    for (int team = 0; team < 2; ++team) {
        auto &occupied = points[team];
        if (base_spawn_counts_[team] == 0)
            base_spawn_counts_[team] = static_cast<int>(occupied.size());
        const int bases = (std::min)(base_spawn_counts_[team],
                                      static_cast<int>(occupied.size()));
        if (!bases && targets[team] > 0) {
            std::fprintf(stderr, "[Shroudfire] No original %s spawn to clone.\n", classes[team]);
            continue;
        }
        for (int ring = 1; ring <= 8 && static_cast<int>(occupied.size()) < targets[team]; ++ring) {
            const float radius = 32.0f + ring * 24.0f;
            for (int base = 0; base < bases && static_cast<int>(occupied.size()) < targets[team]; ++base) {
                for (int direction = 0; direction < 8 &&
                     static_cast<int>(occupied.size()) < targets[team]; ++direction) {
                    if (occupied.size() >= 256 || extra_spawn_handles_.size() >= 128 ||
                        MAX_TOTAL_ENTITIES - entities <= 128) break;
                    const float bearing = (direction * 45.0f + (base % 2) * 22.5f) *
                        0.01745329251994329577f;
                    Vector candidate(occupied[base].position.x + radius * std::cos(bearing),
                                     occupied[base].position.y + radius * std::sin(bearing),
                                     occupied[base].position.z);
                    const Vector ground_start(candidate.x, candidate.y, candidate.z + 48.0f);
                    const Vector ground_end(candidate.x, candidate.y, candidate.z - 128.0f);
                    Ray_t line;
                    SpawnTraceFilter ground_filter(MASK_PLAYERSOLID_BRUSHONLY);
                    CGameTrace ground;
                    trace_shape(&nav_interface, line, ground_start, ground_end,
                                &ground_filter, &ground);
                    if (!ground.DidHit() || ground.m_bStartInSolid ||
                        ground.m_vHitNormal.z < 0.7f ||
                        (ground.m_pEnt && ground.m_pEnt->m_pEntity &&
                         ground.m_pEnt->m_pEntity->GetRefEHandle().GetEntryIndex() > 0))
                        continue;
                    candidate = ground.m_vEndPos + Vector(0.0f, 0.0f, 2.0f);
                    const uint64_t unwanted = CONTENTS_SOLID | CONTENTS_SLIME |
                        CONTENTS_WATER | CONTENTS_PLAYER_CLIP | CONTENTS_LADDER;
                    const uint64_t contents = reinterpret_cast<uint64_t (*)(
                        void *, const Vector *, uint64_t)>(contents_function)(
                            &nav_interface, &candidate, unwanted);
                    if (contents & unwanted) continue;
                    bool occupied_nearby = false;
                    for (const auto &point : occupied) {
                        const float dx = candidate.x - point.position.x;
                        const float dy = candidate.y - point.position.y;
                        if (std::fabs(candidate.z - point.position.z) < 72.0f &&
                            dx * dx + dy * dy < 2304.0f) {
                            occupied_nearby = true;
                            break;
                        }
                    }
                    if (occupied_nearby) continue;
                    const Ray_t hull(Vector(-16.0f, -16.0f, 0.0f),
                                     Vector(16.0f, 16.0f, 72.0f));
                    SpawnTraceFilter clearance_filter(MASK_PLAYERSOLID);
                    CGameTrace clearance;
                    trace_shape(&nav_interface, hull, candidate, candidate,
                                &clearance_filter, &clearance);
                    if (clearance.DidHit()) continue;
                    const Vector path_start = occupied[base].position + Vector(0.0f, 0.0f, 2.0f);
                    SpawnTraceFilter path_filter(MASK_PLAYERSOLID);
                    CGameTrace path;
                    trace_shape(&nav_interface, hull, path_start, candidate,
                                &path_filter, &path);
                    if (path.m_bStartInSolid || !std::isfinite(path.m_flFraction) ||
                        path.m_flFraction <= 0.98f) continue;
                    auto *spawn = reinterpret_cast<CEntityInstance *(*)(const char *, int)>(
                        create_entity_function_)(classes[team], -1);
                    if (!spawn || !spawn->m_pEntity) return added;
                    reinterpret_cast<void (*)(void *, void *)>(dispatch_spawn_function_)(spawn, nullptr);
                    void *teleport = (*reinterpret_cast<void ***>(spawn))[kTeleportVtable];
                    if (!executable_in_module(teleport, server_module_)) {
                        reinterpret_cast<void (*)(void *)>(remove_entity_function_)(spawn);
                        return added;
                    }
                    reinterpret_cast<void (*)(void *, const void *, const void *, const void *)>(
                        teleport)(spawn, &candidate, &occupied[base].angles, nullptr);
                    const void *spawn_body = *reinterpret_cast<void *const *>(
                        reinterpret_cast<const char *>(spawn) + fields_.body_component);
                    const void *spawn_scene = spawn_body ? *reinterpret_cast<void *const *>(
                        reinterpret_cast<const char *>(spawn_body) + fields_.scene_node) : nullptr;
                    Vector actual;
                    if (spawn_scene)
                        std::memcpy(actual.Base(), reinterpret_cast<const char *>(spawn_scene) +
                                    fields_.abs_origin, 3 * sizeof(float));
                    if (!spawn_scene || !std::isfinite(actual.x) || !std::isfinite(actual.y) ||
                        !std::isfinite(actual.z) || std::fabs(actual.x - candidate.x) > 1.0f ||
                        std::fabs(actual.y - candidate.y) > 1.0f ||
                        std::fabs(actual.z - candidate.z) > 1.0f) {
                        reinterpret_cast<void (*)(void *)>(remove_entity_function_)(spawn);
                        std::fprintf(stderr, "[Shroudfire] New %s spawn did not reach its validated position.\n",
                                     classes[team]);
                        return added;
                    }
                    extra_spawn_handles_.push_back(spawn->m_pEntity->GetRefEHandle().ToInt());
                    extra_spawn_zones_.push_back({team + 2,
                        {candidate.x, candidate.y, candidate.z}});
                    occupied.push_back({candidate, occupied[base].angles});
                    ++entities;
                    ++added;
                }
            }
        }
        if (static_cast<int>(occupied.size()) < targets[team])
            std::fprintf(stderr, "[Shroudfire] Only %zu/%d safe %s spawns available.\n",
                         occupied.size(), targets[team], classes[team]);
    }
    return added;
#else
    (void)terrorist_target; (void)counter_terrorist_target;
    return -1;
#endif
}

void EngineAdapter::remove_extra_spawns() {
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (system && remove_entity_function_) {
        for (uint32_t handle : extra_spawn_handles_) {
            CEntityInstance *entity = system->GetEntityInstance(CEntityHandle(handle));
            if (entity && entity->m_pEntity)
                reinterpret_cast<void (*)(void *)>(remove_entity_function_)(entity);
        }
    }
    clear_extra_spawn_tracking();
}

bool EngineAdapter::set_observer(int viewer_slot, int target_slot, bool free_roam) const {
    CEntityInstance *viewer_controller = controller(viewer_slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!viewer_controller || !system) return false;
    const auto &viewer_handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(viewer_controller) + fields_.observer_pawn);
    CEntityInstance *viewer = system->GetEntityInstance(viewer_handle);
    CEntityInstance *target = target_slot > 0 ? pawn(target_slot) : nullptr;
    if (!viewer || (target_slot > 0 && (!target || !target->m_pEntity))) return false;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(viewer) + fields_.observer_services);
    if (!services) return false;
    if (!mark_state_changed(viewer, fields_.observer_services)) return false;
    *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>(services) + fields_.observer_mode) =
        target ? 2 : (free_roam ? 4 : 1); // In-eye, roaming, or fixed camera.
    auto *handle = reinterpret_cast<CEntityHandle *>(
        reinterpret_cast<char *>(services) + fields_.observer_target);
    if (target) *handle = target->m_pEntity->GetRefEHandle();
    else handle->Term();
    return true;
}

int EngineAdapter::observer_mode(int viewer_slot) const {
    CEntityInstance *viewer_controller = controller(viewer_slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!viewer_controller || !system) return -1;
    const auto &viewer_handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(viewer_controller) + fields_.observer_pawn);
    CEntityInstance *viewer = system->GetEntityInstance(viewer_handle);
    if (!viewer) return -1;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(viewer) + fields_.observer_services);
    return services ? *reinterpret_cast<const uint8_t *>(
        reinterpret_cast<const char *>(services) + fields_.observer_mode) : -1;
}

int EngineAdapter::observer_target_slot(int viewer_slot) const {
    CEntityInstance *viewer_controller = controller(viewer_slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!viewer_controller || !system) return 0;
    const auto &viewer_handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(viewer_controller) + fields_.observer_pawn);
    CEntityInstance *viewer = system->GetEntityInstance(viewer_handle);
    if (!viewer) return 0;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(viewer) + fields_.observer_services);
    if (!services) return 0;
    const auto &target_handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(services) + fields_.observer_target);
    CEntityInstance *target = system->GetEntityInstance(target_handle);
    if (!target) return 0;
    for (int slot = 1; slot <= 64; ++slot)
        if (pawn(slot) == target) return slot;
    return 0;
}

void *EngineAdapter::observer_movement_services(int slot) const {
    CEntityInstance *owner = controller(slot);
    auto *system = static_cast<CGameEntitySystem *>(entity_system());
    if (!owner || !system) return nullptr;
    const auto &handle = *reinterpret_cast<const CEntityHandle *>(
        reinterpret_cast<const char *>(owner) + fields_.observer_pawn);
    CEntityInstance *observer = system->GetEntityInstance(handle);
    if (!observer || !observer->m_pEntity) return nullptr;
    void *services = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(observer) + fields_.movement_services);
    return services;
}

bool EngineAdapter::sample_activity(int slot, Activity &activity) const {
    CEntityInstance *entity = pawn(slot);
    if (!entity) return false;
    const auto *base = reinterpret_cast<const char *>(entity);
    activity.flags = *reinterpret_cast<const uint32_t *>(base + fields_.flags);
    const void *body = *reinterpret_cast<void *const *>(base + fields_.body_component);
    const void *movement = *reinterpret_cast<void *const *>(base + fields_.movement_services);
    if (!body || !movement) return false;
    const void *scene = *reinterpret_cast<void *const *>(
        reinterpret_cast<const char *>(body) + fields_.scene_node);
    if (!scene) return false;
    std::memcpy(activity.position.data(), reinterpret_cast<const char *>(scene) + fields_.abs_origin,
                3 * sizeof(float));
    std::memcpy(activity.angles.data(), base + fields_.eye_angles, 3 * sizeof(float));
    std::memcpy(&activity.buttons,
                reinterpret_cast<const char *>(movement) + fields_.movement_buttons + fields_.button_states,
                sizeof(activity.buttons));
    activity.crouched = (activity.buttons & (uint64_t {1} << 2)) != 0 ||
        (fields_.movement_ducked != 0 && *reinterpret_cast<const bool *>(
            reinterpret_cast<const char *>(movement) + fields_.movement_ducked)) ||
        (fields_.movement_duck_amount != 0 && *reinterpret_cast<const float *>(
            reinterpret_cast<const char *>(movement) + fields_.movement_duck_amount) >= 0.5f);
    return true;
}
