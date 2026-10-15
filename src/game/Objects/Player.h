/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once

#include "Common.h"
#include "ModuleSlots.h"
#include "ItemPrototype.h"
#include "Unit.h"
#include "Item.h"
#include "MapNodes/AbstractPlayer.h"

#include "Database/DatabaseEnv.h"
#include "NPCHandler.h"
#include "QuestDef.h"
#include "Group.h"
#include "Bag.h"
#include "WorldSession.h"
#include "Pet.h"
#include "Util.h"                                           // for Tokens typedef
#include "ReputationMgr.h"
#include "BattleGround.h"
#include "DBCStores.h"
#include "SharedDefines.h"
#include "SpellMgr.h"
#include "HonorMgr.h"
#include "TransmogMgr.h"
#include "DynamicVisibilityMgr.h"

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <any>
#include <deque>

struct Mail;
class Channel;
class Creature;
class PlayerMenu;
class UpdateMask;
class SpellCastTargets;
class PlayerSocial;
class DungeonPersistentState;
class Spell;
class Item;
class ZoneScript;
class PlayerAI;
class PlayerBroadcaster;
class MapReference;

static constexpr uint8 PLAYER_MAX_SKILLS = 127;
constexpr uint8 PLAYER_EXPLORED_ZONES_SIZE = 64;
constexpr uint32 CORPSE_REPOP_TIME = (6 * MINUTE * IN_MILLISECONDS);
constexpr uint32 CAMERA_UPDATE_DELAY = 400;

// Note: SPELLMOD_* values is aura types in fact
enum SpellModType
{
    SPELLMOD_TYPE_NONE    = 0,
    SPELLMOD_FLAT         = 107,                            // SPELL_AURA_ADD_FLAT_MODIFIER
    SPELLMOD_PCT          = 108                             // SPELL_AURA_ADD_PCT_MODIFIER
};

enum EnvironmentFlags
{
    ENVIRONMENT_FLAG_NONE           = 0x00,
    ENVIRONMENT_FLAG_IN_WATER       = 0x01,                     // Swimming or standing in water
    ENVIRONMENT_FLAG_IN_MAGMA       = 0x02,                     // Swimming or standing in magma
    ENVIRONMENT_FLAG_IN_SLIME       = 0x04,                     // Swimming or standing in slime
    ENVIRONMENT_FLAG_HIGH_SEA       = 0x08,                     // Anywhere inside deep water area
    ENVIRONMENT_FLAG_UNDERWATER     = 0x10,                     // Swimming fully submerged in any liquid
    ENVIRONMENT_FLAG_HIGH_LIQUID    = 0x20,                     // In any liquid deep enough to be able to swim
    ENVIRONMENT_FLAG_LIQUID         = 0x40,                     // Anywhere indide area with any liquid

    ENVIRONMENT_MASK_LIQUID_HAZARD  = (ENVIRONMENT_FLAG_IN_MAGMA | ENVIRONMENT_FLAG_IN_SLIME),
    ENVIRONMENT_MASK_IN_LIQUID      = (ENVIRONMENT_FLAG_IN_WATER | ENVIRONMENT_MASK_LIQUID_HAZARD),
    ENVIRONMENT_MASK_LIQUID_FLAGS   = (ENVIRONMENT_FLAG_UNDERWATER | ENVIRONMENT_MASK_IN_LIQUID | ENVIRONMENT_FLAG_HIGH_SEA | ENVIRONMENT_FLAG_LIQUID | ENVIRONMENT_FLAG_HIGH_LIQUID),
};

enum CustomPlayerFlags
{
    CUSTOM_PLAYER_FLAG_HC_RESTORED            = 0x01,
    CUSTOM_PLAYER_FLAG_HC_SENT_INFERNO_INVITE = 0x02,
    CUSTOM_PLAYER_FLAG_BROKEN_GOBLIN          = 0x04,
    CUSTOM_PLAYER_FLAG_BYPASS_WHO_COOLDOWN    = 0x08,
    CUSTOM_PLAYER_FLAG_RECEIVED_LUNAR_GIFT    = 0x10,
    CUSTOM_PLAYER_FLAG_WAS_TRANSFERRED        = 0x20,
    CUSTOM_PLAYER_FLAG_RACE_CHANGE_CHECK      = 0x40,
    CUSTOM_PLAYER_FLAG_GOT_RACE_REFUND        = 0x80
};

enum BuyBankSlotResult
{
    ERR_BANKSLOT_FAILED_TOO_MANY    = 0,
    ERR_BANKSLOT_INSUFFICIENT_FUNDS = 1,
    ERR_BANKSLOT_NOTBANKER          = 2,
    ERR_BANKSLOT_OK                 = 3
};

enum PlayerSpellState
{
    PLAYERSPELL_UNCHANGED = 0,
    PLAYERSPELL_CHANGED   = 1,
    PLAYERSPELL_NEW       = 2,
    PLAYERSPELL_REMOVED   = 3
};

struct PlayerSpell
{
    PlayerSpellState state : 8;
    bool active            : 1;                             // show in spellbook
    bool dependent         : 1;                             // learned as result another spell learn, skill grow, quest reward, etc
    bool disabled          : 1;                             // first rank has been learned in result talent learn but currently talent unlearned, save max learned ranks
};

enum Challenges
{
    CHALLENGE_SLOW_AND_STEADY = 0,
    CHALLENGE_EXHAUSTION_MODE = 1,
    CHALLENGE_WAR_MODE = 2,
    CHALLENGE_HARDCORE = 3,
    CHALLENGE_VAGRANT_MODE = 4,
    CHALLENGE_BOARING_MODE = 5,
    CHALLENGE_CRAFTMASTER = 6,
    CHALLENGE_LUNATIC = 7,
    CHALLENGE_BREWMASTER = 8,
    CHALLENGE_HEROIC = 9,
};

enum ChallengeSpells
{
    SPELL_SLOW_AND_STEADY = 50000,
    SPELL_EXHAUSTION_MODE = 50004,
    SPELL_WAR_MODE = 50008,
    SPELL_HARDCORE = 50001,
    SPELL_VARGANT_MODE = 50014,
    SPELL_BOARING_MODE = 50071,
    SPELL_CRAFTMASTER = 57738,
    SPELL_LUNATIC = 57736,
    SPELL_BREWMASTER = 57746,
    SPELL_HEROIC = 57846,
};

constexpr uint32 LoreKeeperQuestRequirement = 1700;
constexpr uint32 SeekerQuestRequirement = 2800;

typedef std::unordered_map<uint32, PlayerSpell> PlayerSpellMap;

// Spell modifier (used to modify other spells)
struct SpellModifier
{
    SpellModifier() : op(MAX_SPELLMOD), type(SPELLMOD_TYPE_NONE), charges(0), value(0), mask(0), spellId(0), ownerAura(nullptr) {}

    SpellModifier(SpellModOp _op, SpellModType _type, int32 _value, uint32 _spellId, uint64 _mask, int16 _charges = 0) :
        op(_op), type(_type), charges(_charges), value(_value), mask(_mask), spellId(_spellId), ownerAura(nullptr) {}

    SpellModifier(SpellModOp _op, SpellModType _type, int32 _value, SpellEntry const* spellEntry, SpellEffectIndex eff, int16 _charges = 0);

    SpellModifier(SpellModOp _op, SpellModType _type, int32 _value, Aura* aura, int16 _charges = 0);

    bool isAffectedOnSpell(SpellEntry const* spell) const;

    SpellModOp   op   : 8;
    SpellModType type : 8;
    int16 charges     : 16; // 0 = infinite
    int32 value;
    uint64 mask;
    uint32 spellId;
    Aura* ownerAura;
};

typedef std::list<SpellModifier*> SpellModList;

enum TrainerSpellState
{
    TRAINER_SPELL_GREEN = 0,
    TRAINER_SPELL_RED   = 1,
    TRAINER_SPELL_GRAY  = 2,
    TRAINER_SPELL_GREEN_DISABLED = 10                       // custom value, not send to client: formally green but learn not allowed
};

enum ActionButtonUpdateState
{
    ACTIONBUTTON_UNCHANGED = 0,
    ACTIONBUTTON_CHANGED   = 1,
    ACTIONBUTTON_NEW       = 2,
    ACTIONBUTTON_DELETED   = 3
};

enum ActionButtonType
{
    ACTION_BUTTON_SPELL     = 0x00,
    ACTION_BUTTON_C         = 0x01,                         // click?
    ACTION_BUTTON_MACRO     = 0x40,
    ACTION_BUTTON_CMACRO    = ACTION_BUTTON_C | ACTION_BUTTON_MACRO,
    ACTION_BUTTON_ITEM      = 0x80
};

#define ACTION_BUTTON_ACTION(X) (uint32(X) & 0x00FFFFFF)
#define ACTION_BUTTON_TYPE(X)   ((uint32(X) & 0xFF000000) >> 24)
#define MAX_ACTION_BUTTON_ACTION_VALUE (0x00FFFFFF+1)

struct ActionButton
{
    uint32 packedData = 0;
    ActionButtonUpdateState uState = ACTIONBUTTON_NEW;

    // helpers
    ActionButtonType GetType() const { return ActionButtonType(ACTION_BUTTON_TYPE(packedData)); }
    uint32 GetAction() const { return ACTION_BUTTON_ACTION(packedData); }
    void SetActionAndType(uint32 action, ActionButtonType type)
    {
        uint32 newData = action | (uint32(type) << 24);
        if (newData != packedData || uState == ACTIONBUTTON_DELETED)
        {
            packedData = newData;
            if (uState != ACTIONBUTTON_NEW)
                uState = ACTIONBUTTON_CHANGED;
        }
    }
};

constexpr uint8 MAX_ACTION_BUTTONS = 120;   // TBC 132 checked in 2.3.0

struct PlayerCreateInfoItem
{
    PlayerCreateInfoItem(uint32 id, uint32 amount) : item_id(id), item_amount(amount) {}

    uint32 item_id;
    uint32 item_amount;
};

typedef std::vector<PlayerCreateInfoItem> PlayerCreateInfoItems;

struct PlayerClassLevelInfo
{
    uint16 basehealth = 0;
    uint16 basemana = 0;
};

struct PlayerClassInfo
{
    PlayerClassLevelInfo* levelInfo = nullptr;                        //[level-1] 0..MaxPlayerLevel-1
};

struct PlayerLevelInfo
{
    uint8 stats[MAX_STATS] = { 0 };
};

enum class LogItemAction
{
    Looted = 1,
    TradeReceived,
    MailReceived,
    Deleted,
    Sold,
    Disenchanted,
    Traded,
    Auctioned,
    Mailed
};

struct LogItemInfo
{
    uint32 guidLow;
    uint32 entry;
    uint64 timestamp;
    uint32 count;
    LogItemAction action;
};

typedef std::vector<uint32> PlayerCreateInfoSpells;

struct PlayerCreateInfoAction
{
    PlayerCreateInfoAction() = default;
    PlayerCreateInfoAction(uint8 _button, uint32 _action, uint8 _type) : button(_button), type(_type), action(_action) {}

    uint8 button = 0;
    uint8 type = 0;
    uint32 action = 0;
};

typedef std::vector<PlayerCreateInfoAction> PlayerCreateInfoActions;

struct PlayerInfo
{
    uint32 mapId = 0;
    uint32 areaId = 0;
    float positionX = 0.0f;
    float positionY = 0.0f;
    float positionZ = 0.0f;
    float orientation = 0.0f;
    uint16 displayId_m = 0;
    uint16 displayId_f = 0;
    PlayerCreateInfoItems item;
    PlayerCreateInfoSpells spell;
    PlayerCreateInfoActions action;

    PlayerLevelInfo* levelInfo = nullptr;                             //[level-1] 0..MaxPlayerLevel-1
};

struct PvPInfo
{
    bool inPvPCombat = false;
    bool inPvPEnforcedArea = false;
    bool inPvPCapturePoint = false;
    bool isPvPFlagCarrier = false;
    uint32 timerPvPRemaining = 0;
    uint32 timerPvPContestedRemaining = 0;
};

struct DuelInfo
{
    Player* initiator = nullptr;
    ObjectGuid opponent;
    time_t startTimer = 0;
    time_t startTime = 0;
    time_t outOfBound = 0;
    uint32 transportGuid = 0;
    bool finished = false;
};

struct Areas
{
    uint32 areaID = 0;
    uint32 areaFlag = 0;
    float x1 = 0.0f;
    float x2 = 0.0f;
    float y1 = 0.0f;
    float y2 = 0.0f;
};

struct EnchantDuration
{
    EnchantDuration() = default;
    EnchantDuration(Item* _item, EnchantmentSlot _slot, uint32 _leftduration) : item(_item), slot(_slot), leftduration(_leftduration) { MANGOS_ASSERT(item); };

    Item* item = nullptr;
    EnchantmentSlot slot = MAX_ENCHANTMENT_SLOT;
    uint32 leftduration = 0;
};

typedef std::list<EnchantDuration> EnchantDurationList;
typedef std::list<Item*> ItemDurationList;

enum RaidGroupError
{
    ERR_RAID_GROUP_REQUIRED = 1,
    ERR_RAID_GROUP_FULL     = 2
};

enum DrunkenState
{
    DRUNKEN_SOBER   = 0,
    DRUNKEN_TIPSY   = 1,
    DRUNKEN_DRUNK   = 2,
    DRUNKEN_SMASHED = 3
};

enum PlayerFlags
{
    PLAYER_FLAGS_NONE                   = 0x00000000,
    PLAYER_FLAGS_GROUP_LEADER           = 0x00000001,
    PLAYER_FLAGS_AFK                    = 0x00000002,
    PLAYER_FLAGS_DND                    = 0x00000004,
    PLAYER_FLAGS_GM                     = 0x00000008,
    PLAYER_FLAGS_GHOST                  = 0x00000010,
    PLAYER_FLAGS_RESTING                = 0x00000020,
    PLAYER_FLAGS_UNK7                   = 0x00000040,       // admin?
    PLAYER_FLAGS_FFA_PVP                = 0x00000080,
    PLAYER_FLAGS_CONTESTED_PVP          = 0x00000100,       // Player has been involved in a PvP combat and will be attacked by contested guards
    PLAYER_FLAGS_PVP_DESIRED            = 0x00000200,       // Stores player's permanent PvP flag preference
    PLAYER_FLAGS_HIDE_HELM              = 0x00000400,
    PLAYER_FLAGS_HIDE_CLOAK             = 0x00000800,
    PLAYER_FLAGS_PARTIAL_PLAY_TIME      = 0x00001000,       // played long time
    PLAYER_FLAGS_NO_PLAY_TIME           = 0x00002000,       // played too long time
    PLAYER_FLAGS_UNK15                  = 0x00004000,
    PLAYER_FLAGS_UNK16                  = 0x00008000,       // strange visual effect (2.0.1), looks like PLAYER_FLAGS_GHOST flag
    PLAYER_FLAGS_SANCTUARY              = 0x00010000,       // player entered sanctuary
    PLAYER_FLAGS_TAXI_BENCHMARK         = 0x00020000,       // taxi benchmark mode (on/off) (2.0.1)
    PLAYER_FLAGS_PVP_TIMER              = 0x00040000,       // 3.0.2, pvp timer active (after you disable pvp manually)
    PLAYER_SALT_FLATS_RACER             = 0x00080000,       // Required for Mirage Raceway event.
  
};

// used in (PLAYER_FIELD_BYTES, 0) byte values
enum PlayerFieldByteFlags
{
    PLAYER_FIELD_BYTE_TRACK_STEALTHED   = 0x02,
    PLAYER_FIELD_BYTE_RELEASE_TIMER     = 0x08,             // Display time till auto release spirit
    PLAYER_FIELD_BYTE_NO_RELEASE_WINDOW = 0x10              // Display no "release spirit" window at all
};

// used in byte (PLAYER_FIELD_BYTES2,1) values
enum PlayerFieldByte2Flags
{
    PLAYER_FIELD_BYTE2_NONE              = 0x00,
    PLAYER_FIELD_BYTE2_DETECT_AMORE      = 0x01,            // SPELL_AURA_DETECT_AMORE
    PLAYER_FIELD_BYTE2_STEALTH           = 0x20,
    PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW = 0x40
};

enum ActivateTaxiReplies
{
    ERR_TAXIOK                      = 0,
    ERR_TAXIUNSPECIFIEDSERVERERROR  = 1,
    ERR_TAXINOSUCHPATH              = 2,
    ERR_TAXINOTENOUGHMONEY          = 3,
    ERR_TAXITOOFARAWAY              = 4,
    ERR_TAXINOVENDORNEARBY          = 5,
    ERR_TAXINOTVISITED              = 6,
    ERR_TAXIPLAYERBUSY              = 7,
    ERR_TAXIPLAYERALREADYMOUNTED    = 8,
    ERR_TAXIPLAYERSHAPESHIFTED      = 9,
    ERR_TAXIPLAYERMOVING            = 10,
    ERR_TAXISAMENODE                = 11,
    ERR_TAXINOTSTANDING             = 12
};

class MirrorTimer
{
    public:
        enum Type
        {
            FATIGUE         = 0,
            BREATH          = 1,
            FEIGNDEATH      = 2,

            NUM_CLIENT_TIMERS,

            ENVIRONMENTAL   = NUM_CLIENT_TIMERS,

            NUM_TIMERS
        };

        enum Status
        {
            UNCHANGED       = 0,
            FULL_UPDATE     = 1,
            STATUS_UPDATE   = 2,
        };

        MirrorTimer(Type type) : m_type(type), m_scale(-1), m_spellId(0), m_status(UNCHANGED), m_active(false), m_frozen(false) {}

        inline bool     IsActive() const { return m_active; }
        inline bool     IsRegenerating() const { return (m_scale > 0); }
        inline bool     IsFrozen() const { return (m_frozen && !IsRegenerating()); }

        inline Type     GetType() const { return m_type; }
        inline uint32   GetRemaining() const { return (m_tracker.GetInterval() - m_tracker.GetCurrent()); }
        inline uint32   GetDuration() const { return m_tracker.GetInterval(); }
        inline int32    GetScale() const { return m_scale; }
        inline uint32   GetSpellId() const { return m_spellId; }

        inline Status   FetchStatus();

        inline void Stop();

        inline void Start(uint32 interval, uint32 spellId = 0);
        inline void Start(uint32 current, uint32 max, uint32 spellId);

        inline void SetRemaining(uint32 duration);
        inline void SetDuration(uint32 duration);

        inline void SetFrozen(bool state);

        inline void SetScale(int32 scale);

        bool Update(uint32 diff);

    private:
        Type m_type;
        int32 m_scale;
        uint32 m_spellId;

        ShortIntervalTimer m_tracker;
        ShortIntervalTimer m_pulse;

        Status m_status;

        bool m_active;
        bool m_frozen;
};

// 2^n values
enum AtLoginFlags
{
    AT_LOGIN_NONE              = 0x00,
    AT_LOGIN_RENAME            = 0x01,
    AT_LOGIN_RESET_SPELLS      = 0x02,
    AT_LOGIN_RESET_TALENTS     = 0x04,
    //AT_LOGIN_CUSTOMIZE         = 0x08, -- used in post-3.x
    //AT_LOGIN_RESET_PET_TALENTS = 0x10, -- used in post-3.x
    AT_LOGIN_FIRST             = 0x20,
};

// Nostalrius
enum PlayerCheatOptions
{
    //PLAYER_CHEAT_GOD               = 0x001,
    PLAYER_CHEAT_NO_COOLDOWN       = 0x002,
    PLAYER_CHEAT_NO_CAST_TIME      = 0x004,
    PLAYER_CHEAT_NO_POWER          = 0x008,
    PLAYER_CHEAT_NO_MOD_SPEED      = 0x010,
    PLAYER_CHEAT_ALWAYS_CRIT       = 0x020,
    PLAYER_CHEAT_NO_CHECK_CAST     = 0x040,
    PLAYER_CHEAT_ALWAYS_PROC       = 0x080,
    PLAYER_VIDEO_MODE              = 0x100,
    PLAYER_CHEAT_UNRANDOMIZE       = 0x200,
    PLAYER_CHEAT_ALWAYS_SPELL_CRIT = 0x400,
    PLAYER_RANDOMIZE_WHISPER_NAMES = 0x800,
    PLAYER_ANON_MAIL               = 0x1000,
    PLAYER_CHEAT_NO_DAMAGE_RNG     = 0x2000
};

typedef std::map<uint32, QuestStatusData> QuestStatusMap;

enum QuestSlotOffsets
{
    QUEST_ID_OFFSET           = 0,
    QUEST_COUNT_STATE_OFFSET  = 1,                          // including counters 6bits+6bits+6bits+6bits + state 8bits
    QUEST_TIME_OFFSET         = 2
};

static constexpr uint8 MAX_QUEST_OFFSET = 3;

enum QuestSlotStateMask
{
    QUEST_STATE_NONE     = 0x0000,
    QUEST_STATE_COMPLETE = 0x0001,
    QUEST_STATE_FAIL     = 0x0002
};

enum SkillUpdateState
{
    SKILL_UNCHANGED     = 0,
    SKILL_CHANGED       = 1,
    SKILL_NEW           = 2,
    SKILL_DELETED       = 3
};

struct SkillStatusData
{
    SkillStatusData(uint8 _pos, SkillUpdateState _uState) : pos(_pos), uState(_uState) {}
    uint8 pos;
    SkillUpdateState uState;
};

typedef std::unordered_map<uint32, SkillStatusData> SkillStatusMap;

constexpr uint32 GNOMECAR_DISPLAYID = 2490;
constexpr uint32 GOBLINCAR_DISPLAYID = 18320;

enum PlayerSlots
{
    // first slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_START           = 0,
    // last+1 slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_END             = 118,
    PLAYER_SLOTS_COUNT          = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

constexpr uint8 INVENTORY_SLOT_BAG_0 = 255;

enum EquipmentSlots                                         // 19 slots
{
    EQUIPMENT_SLOT_START        = 0,
    EQUIPMENT_SLOT_HEAD         = 0,
    EQUIPMENT_SLOT_NECK         = 1,
    EQUIPMENT_SLOT_SHOULDERS    = 2,
    EQUIPMENT_SLOT_BODY         = 3,
    EQUIPMENT_SLOT_CHEST        = 4,
    EQUIPMENT_SLOT_WAIST        = 5,
    EQUIPMENT_SLOT_LEGS         = 6,
    EQUIPMENT_SLOT_FEET         = 7,
    EQUIPMENT_SLOT_WRISTS       = 8,
    EQUIPMENT_SLOT_HANDS        = 9,
    EQUIPMENT_SLOT_FINGER1      = 10,
    EQUIPMENT_SLOT_FINGER2      = 11,
    EQUIPMENT_SLOT_TRINKET1     = 12,
    EQUIPMENT_SLOT_TRINKET2     = 13,
    EQUIPMENT_SLOT_BACK         = 14,
    EQUIPMENT_SLOT_MAINHAND     = 15,
    EQUIPMENT_SLOT_OFFHAND      = 16,
    EQUIPMENT_SLOT_RANGED       = 17,
    EQUIPMENT_SLOT_TABARD       = 18,
    EQUIPMENT_SLOT_END          = 19
};

enum InventorySlots                                         // 4 slots
{
    INVENTORY_SLOT_BAG_START    = 19,
    INVENTORY_SLOT_BAG_END      = 23
};

enum InventoryPackSlots                                     // 16 slots
{
    INVENTORY_SLOT_ITEM_START   = 23,
    INVENTORY_SLOT_ITEM_END     = 39
};

enum BankItemSlots                                          // 28 slots
{
    BANK_SLOT_ITEM_START        = 39,
    BANK_SLOT_ITEM_END          = 63
};

enum BankBagSlots                                           // 6 slots
{
    BANK_SLOT_BAG_START         = 63,
    BANK_SLOT_BAG_END           = 69
};

enum BuyBackSlots                                           // 12 slots
{
    // stored in m_buybackitems
    BUYBACK_SLOT_START          = 69,
    BUYBACK_SLOT_END            = 81
};

enum KeyRingSlots                                           // 32 slots
{
    KEYRING_SLOT_START          = 81,
    KEYRING_SLOT_END            = KEYRING_SLOT_START + 32
};

struct ItemPosCount
{
    ItemPosCount(uint16 _pos, uint8 _count) : pos(_pos), count(_count) {}
    bool isContainedIn(std::vector<ItemPosCount> const& vec) const;
    uint16 pos;
    uint8 count;
};
typedef std::vector<ItemPosCount> ItemPosCountVec;

enum TradeSlots
{
    TRADE_SLOT_COUNT            = 7,
    TRADE_SLOT_TRADED_COUNT     = 6,
    TRADE_SLOT_NONTRADED        = 6,
    TRADE_SLOT_INVALID          = -1
};

enum TransferAbortReason
{
    TRANSFER_ABORT_MAX_PLAYERS                  = 0x01,     // Transfer Aborted: instance is full
    TRANSFER_ABORT_NOT_FOUND                    = 0x02,     // Transfer Aborted: instance not found
    TRANSFER_ABORT_TOO_MANY_INSTANCES           = 0x03,     // You have entered too many instances recently.
    TRANSFER_ABORT_SILENTLY                     = 0x04,     // no message shown; the same effect give values above 5
    TRANSFER_ABORT_ZONE_IN_COMBAT               = 0x05,     // Unable to zone in while an encounter is in progress.
};

enum InstanceResetWarningType
{
    RAID_INSTANCE_WARNING_HOURS     = 1,                    // WARNING! %s is scheduled to reset in %d hour(s).
    RAID_INSTANCE_WARNING_MIN       = 2,                    // WARNING! %s is scheduled to reset in %d minute(s)!
    RAID_INSTANCE_WARNING_MIN_SOON  = 3,                    // WARNING! %s is scheduled to reset in %d minute(s). Please exit the zone or you will be returned to your bind location!
    RAID_INSTANCE_WELCOME           = 4                     // Welcome to %s. This raid instance is scheduled to reset in %s.
};

enum RestType
{
    REST_TYPE_NO        = 0,
    REST_TYPE_IN_TAVERN = 1,
    REST_TYPE_IN_CITY   = 2
};

enum DuelCompleteType
{
    DUEL_INTERRUPTED = 0,
    DUEL_WON = 1,
    DUEL_FLED = 2
};

/// Type of environmental damages
enum EnvironmentalDamageType
{
    DAMAGE_EXHAUSTED = 0,
    DAMAGE_DROWNING  = 1,
    DAMAGE_FALL      = 2,
    DAMAGE_LAVA      = 3,
    DAMAGE_SLIME     = 4,
    DAMAGE_FIRE      = 5,
    DAMAGE_FALL_TO_VOID = 6                                 // custom case for fall without durability loss
};

enum PlayedTimeIndex
{
    PLAYED_TIME_TOTAL = 0,
    PLAYED_TIME_LEVEL = 1
};

static constexpr uint8 MAX_PLAYED_TIME_INDEX = 2;

// used at player loading query list preparing, and later result selection
enum PlayerLoginQueryIndex
{
    PLAYER_LOGIN_QUERY_LOADFROM,
    PLAYER_LOGIN_QUERY_LOADGROUP,
    PLAYER_LOGIN_QUERY_LOADBOUNDINSTANCES,
    PLAYER_LOGIN_QUERY_LOADAURAS,
    PLAYER_LOGIN_QUERY_LOADSPELLS,
    PLAYER_LOGIN_QUERY_LOADQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADHONORCP,
    PLAYER_LOGIN_QUERY_LOADPVPCURRENCY,
    PLAYER_LOGIN_QUERY_LOADREPUTATION,
    PLAYER_LOGIN_QUERY_LOADTRANSMOGS,
    PLAYER_LOGIN_QUERY_LOADINVENTORY,
    PLAYER_LOGIN_QUERY_LOADITEMLOOT,
    PLAYER_LOGIN_QUERY_LOADACTIONS,
    PLAYER_LOGIN_QUERY_LOADSOCIALLIST,
    PLAYER_LOGIN_QUERY_LOADHOMEBIND,
    PLAYER_LOGIN_QUERY_LOADSPELLCOOLDOWNS,
    PLAYER_LOGIN_QUERY_LOADGUILD,
    PLAYER_LOGIN_QUERY_LOADBGDATA,
    PLAYER_LOGIN_QUERY_LOADSKILLS,
    PLAYER_LOGIN_QUERY_LOADMAILS,
    PLAYER_LOGIN_QUERY_LOADMAILEDITEMS,
    PLAYER_LOGIN_QUERY_BATTLEGROUND_DATA,
    PLAYER_LOGIN_QUERY_FORGOTTEN_SKILLS,
    PLAYER_LOGIN_QUERY_LOADVARIABLES,
    PLAYER_LOGIN_QUERY_ITEM_LOGS,
    PLAYER_LOGIN_QUERY_SAVED_SPECS,

    MAX_PLAYER_LOGIN_QUERY
};

enum PlayerDelayedOperations
{
    DELAYED_SAVE_PLAYER               = 0x01,
    DELAYED_RESURRECT_PLAYER          = 0x02,
    DELAYED_SPELL_CAST_DESERTER       = 0x04,
    DELAYED_CAST_HONORLESS_TARGET     = 0x08,
    DELAYED_END
};

enum ReputationSource
{
    REPUTATION_SOURCE_KILL,
    REPUTATION_SOURCE_QUEST,
    REPUTATION_SOURCE_SPELL
};

enum HardcoreStatus : uint8
{
    HARDCORE_MODE_STATUS_NONE     = 0,
    HARDCORE_MODE_STATUS_ALIVE    = 1,
    HARDCORE_MODE_STATUS_IMMORTAL = 2,
    HARDCORE_MODE_STATUS_DEAD     = 3,
    HARDCORE_MODE_STATUS_HC60      = 4
};

inline char const* HardcoreStatusToString(uint8 status)
{
    switch (status)
    {
        case HARDCORE_MODE_STATUS_NONE:
            return "None";
        case HARDCORE_MODE_STATUS_ALIVE:
            return "Alive";
        case HARDCORE_MODE_STATUS_IMMORTAL:
            return "Immortal";
        case HARDCORE_MODE_STATUS_DEAD:
            return "Dead";
        case HARDCORE_MODE_STATUS_HC60:
            return "HC60";
    }
    return "UNKNOWN";
}

enum PlayerTitles : uint8
{
    TITLE_PROTECTOR_OF_STORMWIND = 1,
    TITLE_OVERLORD_OF_ORGRIMMAR = 2,
    TITLE_THANE_OF_IRONFORGE = 3,
    TITLE_HIGH_SENTINEL_OF_DARNASSUS = 4,
    TITLE_DEATHLORD_OF_THE_UNDERCITY = 5,
    TITLE_CHIEFTAN_OF_THUNDERBLUFF = 6,
    TITLE_VOODOO_BOSS_OF_SENJIN = 7,
    TITLE_AVENGER_OF_GNOMEREGAN = 8,
    TITLE_BRUISER_OF_MUDSPROCKET = 9,
    TITLE_AVENGER_OF_QUELTHALAS = 10,
    TITLE_BLOOD_RING_CHAMPION = 11,
    TITLE_DEVELOPER = 12,
    TITLE_EVERLOOK_BROADCASTING_DJ = 13,
    TITLE_THE_NEWS_ANCHOR_ORC = 14,
    TITLE_SCARAB_LORD = 15,
    TITLE_CONQUEROR_OF_NAXXRAMAS = 16,
    TITLE_CARTOGRAPHER = 17,
    TITLE_STILL_ALIVE = 18,
    TITLE_ACTIVISION_BLIZZARD_SPY = 19,
    TITLE_CONQUEROR_OF_THE_FLAMES = 20,
    TITLE_CONQUEROR_OF_THE_DRAGONS = 21,
    TITLE_CONQUEROR_OF_THE_QIRAJI = 22,
    TITLE_CONQUEROR_OF_THE_SCOURGE = 23,
    TITLE_ASCENDED_CHAMPION = 24,
    TITLE_CONQUEROR_OF_DESOLATION = 25,
    TITLE_BANE_OF_THE_NATHREZIM = 26,
    TITLE_THE_JOLLY = 27,
    TITLE_ELUNES_CHOSEN = 28,
    TITLE_CRAFTMASTER = 29,
    TITLE_THE_WEE_WARLORD = 30,
    TITLE_PINT_SIZED_POLITICIAN = 31,
    TITLE_ARENAS_SMALLEST_SMASHER = 32,
    TITLE_LAZY_PEONS_FOREMAN = 33,
    TITLE_HERO_OF_GOLDSHIRE = 34,
    TITLE_PET_ROCK_STAR = 35,
    TITLE_FISHLAYER = 36,
    TITLE_THE_ADDONS_ARCHITECT = 37,
    TITLE_AZEROTHS_MOST_UNLUCKY_TOURIST = 38,
    TITLE_THE_HEROIC = 39,
    TITLE_THE_HAMBRINGER = 40,
    TITLE_SWINE_SLAYER = 40,
    TITLE_GUARDIAN_OF_THE_FLAME = 41,
    TITLE_SEEKER_OF_KNOWLEDGE = 42,
    TITLE_GRAND_FROGUS = 43,
    TITLE_THE_WANDERER = 44,
    TITLE_BREWMASTER = 45,
    TITLE_CRAZY_CAT_LADY = 46,
    TITLE_BLOODTHIRSTY = 47,
    TITLE_STORMWIELDER = 48,
    TITLE_SULFURON_CHAMPION = 49,
    TITLE_GUARDIAN_OF_TIRISFAL = 50,
    TITLE_BANE_OF_THE_SCARLET_CRUSADE = 51,
    TITLE_IMMORTAL = 52,
    TITLE_LOCAL_DRUNK = 53,
    TITLE_THE_ITSY_BITSY_HERO = 54,
    TITLE_BLACKWINGS_BANE = 55,
    TITLE_DRAGONSLAYER = 56,
    TITLE_CHAMPION_OF_THE_SHIFTING_SANDS = 57,
    TITLE_CHAMPION_OF_THE_FROZEN_CITADEL = 58,
    TITLE_LOREKEEPER = 63,
    TITLE_DIPLOMAT = 64,
    TITLE_GODSLAYER_OF_HAKKAR = 65,
    TITLE_DESTROYER_OF_DREAMS = 66,
    TITLE_HERO_OF_AZEROTH = 67,
    TITLE_MAX_LIMIT = 68
};

// Player summoning auto-decline time (in secs)
constexpr uint32 MAX_PLAYER_SUMMON_DELAY = (2 * MINUTE);
constexpr uint32 MAX_MONEY_AMOUNT = (0x7FFFFFFF - 1);

struct InstancePlayerBind
{
    DungeonPersistentState* state;
    bool perm;
    /* permanent PlayerInstanceBinds are created in Raid instances for players
       that aren't already permanently bound when they are inside when a boss is killed
       or when they enter an instance that the group leader is permanently bound to. */
    InstancePlayerBind() : state(nullptr), perm(false) {}
};

// Anti-farm gate from vanilla, compile-time (there is no config key for it -
// AccountInstancesPerHour in mangosd.conf does nothing). Raised for this
// server because the dungeon-clear test harness runs several bot groups
// through the same instance in parallel and five entries per account per hour
// stops the whole rig within minutes. Real players are unaffected at this
// value; the gate still exists.
static constexpr uint8 MAX_INSTANCE_PER_ACCOUNT_PER_HOUR = 100;

struct ResurrectionData
{
    ObjectGuid resurrectorGuid;
    WorldLocation location;
    uint32 instanceId = 0;
    uint32 health = 0;
    uint32 mana = 0;
};

class PlayerTaxi
{
    public:
        PlayerTaxi(): m_discount(0.0f) { m_taximask.fill(0); }
        ~PlayerTaxi() {}
        // Nodes
        void InitTaxiNodes(uint32 race, uint32 level);
        void LoadTaxiMask(const char* data);

        bool IsTaximaskNodeKnown(uint32 nodeidx) const
        {
            uint8  field   = uint8((nodeidx - 1) / 32);
            uint32 submask = 1 << ((nodeidx - 1) % 32);
            if (field >= m_taximask.size())
                return false;

            return (m_taximask[field] & submask) == submask;
        }
        bool SetTaximaskNode(uint32 nodeidx)
        {
            uint8  field   = uint8((nodeidx - 1) / 32);
            uint32 submask = 1 << ((nodeidx - 1) % 32);
            // Both siblings above and below already bound this; only the one
            // that writes did not. m_taximask holds 8 words, so anything past
            // node 256 lands outside it - and what sits immediately after it in
            // this class is m_TaxiDestinations, whose internal pointers then get
            // overwritten. nodeidx 0 is worse still: (0 - 1) / 32 is unsigned,
            // truncated to uint8, which gives field 255. The corrupted deque
            // then faults on the character's next periodic save, which is why
            // such a crash points at SaveTaxiDestinationsToString and never at
            // the taxi code that caused it.
            if (field >= m_taximask.size())
            {
                ReportOutOfRangeTaxiNode(nodeidx, field);
                return false;
            }

            if ((m_taximask[field] & submask) != submask)
            {
                m_taximask[field] |= submask;
                return true;
            }
            else
                return false;
        }

        void SetTaximaskNodeMasked(uint8 field, uint32 submask)
        {
            if (field >= m_taximask.size())
                return;

            m_taximask[field] |= submask;
        }

        void AppendTaximaskTo(ByteBuffer& data, bool all);

        // Destinations
        bool LoadTaxiDestinationsFromString(std::string const& values, Team team);
        std::string SaveTaxiDestinationsToString() const;

        void ClearTaxiDestinations()
        {
            m_TaxiDestinations.clear();
            m_taxiPath.clear();
            m_discount = 1.0f;
            m_taxiStartLocation.Clear();
        }
        void AddTaxiDestination(uint32 dest) { m_TaxiDestinations.push_back(dest); }
        void SetDiscount(float discount) { m_discount = discount; }
        uint32 GetTaxiSource() const { return m_TaxiDestinations.empty() ? 0 : m_TaxiDestinations.front(); }
        uint32 GetTaxiDestination() const { return m_TaxiDestinations.size() < 2 ? 0 : m_TaxiDestinations[1]; }
        uint32 GetCurrentTaxiPath() const;
        uint32 GetCurrentTaxiCost() const;
        uint32 NextTaxiDestination()
        {
            // pop_front on an empty deque is undefined; both callers
            // (FlightPathMovementGenerator::Update and
            // Player::ContinueTaxiFlight) call this without checking.
            if (m_TaxiDestinations.empty())
                return 0;

            m_TaxiDestinations.pop_front();
            return GetTaxiDestination();
        };
        TaxiPathNodeList const& GetTaxiPath() const { return m_taxiPath; };
        void AddTaxiPathNode(TaxiPathNodeEntry const& entry)
        {
            m_taxiPath.resize(m_taxiPath.size() + 1);
            m_taxiPath.set(m_taxiPath.size() - 1, &entry);
        }
        bool empty() const { return m_TaxiDestinations.empty(); }

        friend std::ostringstream& operator<< (std::ostringstream& ss, PlayerTaxi const& taxi);
        WorldLocation m_taxiStartLocation;
    private:
        float m_discount;
        // Out of line: this header has no logger.
        void ReportOutOfRangeTaxiNode(uint32 nodeidx, uint8 field) const;

        TaxiMask m_taximask;
        std::deque<uint32> m_TaxiDestinations;
        TaxiPathNodeList m_taxiPath;
};

std::ostringstream& operator<< (std::ostringstream& ss, PlayerTaxi const& taxi);

/// Holder for BattleGround data
struct BGData
{
    uint32 bgInstanceID = 0;                                ///< This variable is set to bg->m_InstanceID, saved
                                                            ///  when player is teleported to BG - (it is battleground's GUID)
    BattleGroundTypeId bgTypeID = BATTLEGROUND_TYPE_NONE;

    std::set<uint32>   bgAfkReporter;
    uint8              bgAfkReportedCount = 0;
    time_t             bgAfkReportedTimer = 0;
    uint32             bgQueueSlot = PLAYER_MAX_BATTLEGROUND_QUEUES;

    Team bgTeam = TEAM_NONE;                                ///< What side the player will be added to, saved

    WorldLocation joinPos;                                  ///< From where player entered BG, saved
    uint32 soulShardCount = 0;

    bool m_needSave = false;                                ///< true, if saved to DB fields modified after prev. save (marked as "saved" above)
};

struct TransactionPart;

class TradeData
{
    public:                                                 // constructors
        TradeData(Player* player, Player* trader) :
            m_player(player),  m_trader(trader), m_accepted(false), m_acceptProccess(false),
            m_money(0), m_spell(0), m_lastModificationTime(0), m_scamPreventionDelay(0){}

    public:                                                 // access functions

        Player* GetTrader() const { return m_trader; }
        TradeData* GetTraderData() const;

        Item* GetItem(TradeSlots slot) const;
        bool HasItem(ObjectGuid item_guid) const;

        uint32 GetSpell() const { return m_spell; }
        Item*  GetSpellCastItem() const;
        bool HasSpellCastItem() const { return !m_spellCastItem.IsEmpty(); }

        uint32 GetMoney() const { return m_money; }

        bool IsAccepted() const { return m_accepted; }
        bool IsInAcceptProcess() const { return m_acceptProccess; }

        TradeSlots GetTradeSlotForItem(ObjectGuid itemGuid) const;

        void FillTransactionLog(TransactionPart& log) const;

        time_t GetLastModificationTime() const { return m_lastModificationTime; }
        void SetLastModificationTime(time_t t) { m_lastModificationTime = t; }

        time_t GetScamPreventionDelay() const { return m_scamPreventionDelay; }
        void SetScamPreventionDelay(time_t t) { m_scamPreventionDelay = t; }
    public:                                                 // access functions

        void SetItem(TradeSlots slot, Item* item);
        void SetSpell(uint32 spell_id, Item* castItem = nullptr);
        void SetMoney(uint32 money);

        void SetAccepted(bool state, bool crosssend = false);

        // must be called only from accept handler helper functions
        void SetInAcceptProcess(bool state) { m_acceptProccess = state; }

    private:                                                // internal functions

        void Update(bool for_trader = true);

    private:                                                // fields

        Player*    m_player;                                // Player who own of this TradeData
        Player*    m_trader;                                // Player who trade with m_player

        bool       m_accepted;                              // m_player press accept for trade list
        bool       m_acceptProccess;                        // one from player/trader press accept and this processed

        uint32     m_money;                                 // m_player place money to trade

        uint32     m_spell;                                 // m_player apply spell to non-traded slot item
        ObjectGuid m_spellCastItem;                         // applied spell casted by item use

        ObjectGuid m_items[TRADE_SLOT_COUNT];               // traded itmes from m_player side including non-traded slot

        time_t     m_lastModificationTime;                  // to prevent scam (change gold before the other validates)
        time_t     m_scamPreventionDelay;                    // to prevent scam, set a delay in milliseconds (CANNOT be less than or equal to 10ms) before accepting trade.
};

struct CinematicWaypointEntry
{
        uint32 cinematic_id = 0;
        uint32 time = 0;
        Position position;
};

static constexpr uint8 MAX_RACIAL_SPELLS = 10;

struct RacialSpells
{
    uint8 raceId = 0;
    uint32 spells[MAX_RACIAL_SPELLS] = { 0 };
};

namespace Anticheat
{
    class Movement;
}


struct AuraSaveStruct
{
    ObjectGuid caster_guid = 0;
    uint32 item_lowguid = 0;
    uint32 spellid = 0;
    uint32 stackcount = 0;
    uint32 remaincharges = 0;
    int32  damage[MAX_EFFECT_INDEX] = { 0 };
    uint32 periodicTime[MAX_EFFECT_INDEX] = { 0 };

    int32 maxduration = 0;
    int32 remaintime = 0;
    uint32 effIndexMask = 0;
};

struct ScheduledTeleportData
{
    ScheduledTeleportData() = default;
    ScheduledTeleportData(uint32 mapid, float x, float y, float z, float o,
        uint32 options)
        : targetMapId(mapid), x(x), y(y), z(z),
          orientation(o), options(options){};

    uint32 targetMapId = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float orientation = 0.0f;

    uint32 options = 0;
};

enum class PlayerVariables : uint32
{
    HardcoreMessagesEnabled = 1,
    HardcoreMessageLevel,
    UnlockedSpecTabs,
    FreeTalentResets,
    OriginalSkinByte,
    PendingChallengeMask
};

struct PlayerEggLoot
{
    uint32 Id;
    uint32 PlayerGuid;
    uint32 ItemId;
    uint32 ItemGuid;
    bool Refunded;
};

class Player final: public Unit
{
    friend class WorldSession;
    friend void Item::AddToUpdateQueueOf(Player* player);
    friend void Item::RemoveFromUpdateQueueOf(Player* player);
    public:
        explicit Player (WorldSession* session);
        ~Player() override;

        void CleanupsBeforeDelete() override;

        static UpdateMask updateVisualBits;
        static void InitVisibleBits();

        bool Create(uint32 guidlow, std::string const& name, uint8 race, uint8 class_, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair);
        // cmangos's 11-arg form (extra outfitId, ignored).
        bool Create(uint32 guidlow, std::string const& name, uint8 race, uint8 class_, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair, uint8 /*outfitId*/) {
            return Create(guidlow, name, race, class_, gender, skin, face, hairStyle, hairColor, facialHair);
        }
        void Update(uint32 update_diff, uint32 time) override;
        static bool BuildEnumData(QueryResult* result,  WorldPacket* pData);

        /**
         * @brief Can only be called from Master server (or ASSERT will fail)
         * @return player social structure.
         */
        PlayerSocial* GetSocial() const;
        /**
         * @brief Can only be called from Master server (or returns nullptr)
         * @return player social structure - or nullptr if no MasterPlayer
         */
        PlayerSocial* FindSocial() const;

        /*********************************************************/
        /***                 GAMEMASTER SYSTEM                 ***/
        /*********************************************************/

    private:
        uint32 m_gmInvisibilityLevel;
        uint32 m_currentTicketCounter;
        uint32 _playerOptions;
        bool m_shopAllowed = true;
        bool m_shopTransactionInProgress = false;
    public:
        bool IsAcceptTickets() const { return GetSession()->GetSecurity() >= SEC_DEVELOPER && (m_ExtraFlags & PLAYER_EXTRA_GM_ACCEPT_TICKETS); }
        void SetAcceptTicket(bool on) { if(on) m_ExtraFlags |= PLAYER_EXTRA_GM_ACCEPT_TICKETS; else m_ExtraFlags &= ~PLAYER_EXTRA_GM_ACCEPT_TICKETS; }

        bool IsGameMaster() const { return m_ExtraFlags & PLAYER_EXTRA_GM_ON; }
        void SetGameMaster(bool on);
        bool IsGMChat() const { return GetSession()->GetSecurity() >= SEC_OBSERVER && (m_ExtraFlags & PLAYER_EXTRA_GM_CHAT); }
        void SetGMChat(bool on) { if(on) m_ExtraFlags |= PLAYER_EXTRA_GM_CHAT; else m_ExtraFlags &= ~PLAYER_EXTRA_GM_CHAT; }
        bool IsHCChat() const { return !(m_ExtraFlags & PLAYER_EXTRA_DISABLE_HC_CHAT); }
        void SetHCChat(bool on) { if (!on) m_ExtraFlags |= PLAYER_EXTRA_DISABLE_HC_CHAT; else m_ExtraFlags &= ~PLAYER_EXTRA_DISABLE_HC_CHAT; }
        bool IsTaxiCheater() const { return m_ExtraFlags & PLAYER_EXTRA_TAXICHEAT; }
        void SetTaxiCheater(bool on) { if(on) m_ExtraFlags |= PLAYER_EXTRA_TAXICHEAT; else m_ExtraFlags &= ~PLAYER_EXTRA_TAXICHEAT; }
        bool IsGMVisible() const { return !(m_ExtraFlags & PLAYER_EXTRA_GM_INVISIBLE); }
        bool HasGMDisabledSocials() const { return m_ExtraFlags & PLAYER_EXTRA_GM_DISABLE_SOCIAL; }
        void SetGMSocials(bool on, bool init = false);
        void SetGMVisible(bool on);
        void SetPvPDeath(bool on) { if(on) m_ExtraFlags |= PLAYER_EXTRA_PVP_DEATH; else m_ExtraFlags &= ~PLAYER_EXTRA_PVP_DEATH; }

        void SetShopAllowed(bool allowed) { m_shopAllowed = allowed; }
        bool IsShopAllowed() const { return m_shopAllowed; }
        bool IsShopTransactionInprogress() const { return m_shopTransactionInProgress; }
        void SetShopTransactionInProgress(bool state) { m_shopTransactionInProgress = state; }

        bool HasOption(uint32 o) const { return (_playerOptions & o); }
        void EnableOption(uint32 o)    { _playerOptions |= o; }
        void RemoveOption(uint32 o)    { _playerOptions &= (~o); }
        void SetOption(PlayerCheatOptions o, bool on)
        {
            if (on)
                EnableOption(o);
            else
                RemoveOption(o);
        }
        uint32 GetGMInvisibilityLevel() const { return m_gmInvisibilityLevel; }
        void SetGMInvisibilityLevel(uint32 level) { m_gmInvisibilityLevel = level; }
        uint32 GetGMTicketCounter() const { return m_currentTicketCounter; }
        void SetGMTicketCounter(uint32 counter) { m_currentTicketCounter = counter; }

        std::optional<std::string> GetPlayerVariable(PlayerVariables variable) const
        {
            if (m_variables.find(variable) != m_variables.end())
                return m_variables.find(variable)->second;
            return {};
        }

        void SetPlayerVariable(PlayerVariables variable, std::string value)
        {
            m_variables[variable] = value;
        }


        bool hasIllusion = false;
        bool hasIllusionsDisabled = false;
        bool canSeeVendorList = false;

        /*********************************************************/
        /***                    STORAGE SYSTEM                 ***/
        /*********************************************************/
    private:
        std::unordered_map<PlayerVariables, std::string> m_variables;

        std::unordered_map<uint32, std::deque<LogItemInfo>> m_itemLogs; // itemEntry, (lowGuid, struct)
        std::vector<LogItemInfo> m_unsavedItemLogs;

        ObjectGuid m_lootGuid;
        Item* m_items[PLAYER_SLOTS_COUNT];
        uint32 m_currentBuybackSlot;

        std::vector<Item*> m_itemUpdateQueue;
        bool m_itemUpdateQueueBlocked;
        EnchantDurationList m_enchantDuration;
        ItemDurationList m_itemDuration;

        TradeData* m_trade;
        uint32 m_WeaponProficiency;
        uint32 m_ArmorProficiency;

        // internal common parts for CanStore/StoreItem functions
        InventoryResult _CanStoreItem_InSpecificSlot(uint8 bag, uint8 slot, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool swap, Item* pSrcItem) const;
        InventoryResult _CanStoreItem_InBag(uint8 bag, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool merge, bool non_specialized, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        InventoryResult _CanStoreItem_InInventorySlots(uint8 slot_begin, uint8 slot_end, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool merge, Item* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        Item* _StoreItem(uint16 pos, Item* pItem, uint32 count, bool clone, bool update);
        InventoryResult CanTakeMoreSimilarItems(Item* pItem) const { return _CanTakeMoreSimilarItems(pItem->GetEntry(), pItem->GetCount(), pItem); }
        InventoryResult CanTakeMoreSimilarItems(uint32 entry, uint32 count) const { return _CanTakeMoreSimilarItems(entry, count, nullptr); }
        InventoryResult CanEquipUniqueItem(ItemPrototype const* itemProto, uint8 except_slot = NULL_SLOT) const;
        InventoryResult _CanTakeMoreSimilarItems(uint32 entry, uint32 count, Item* pItem, uint32* no_space_count = nullptr) const;
        InventoryResult _CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 entry, uint32 count, Item* pItem = nullptr, bool swap = false, uint32* no_space_count = nullptr) const;
        void ApplyEquipCooldown(Item* pItem);
        bool CheckAmmoCompatibility(const ItemPrototype* ammo_proto) const;
        void QuickEquipItem(uint16 pos, Item* pItem);
        void VisualizeItem(uint8 slot, Item* pItem);
    public:
        void SetVisibleItemSlot(uint8 slot, Item* pItem);
    private:
        // in trade, guild bank, mail....
        void RemoveItemDependentAurasAndCasts(Item* pItem);
        void UpdateEnchantTime(uint32 time);
        void UpdateItemDuration(uint32 time, bool realtimeonly = false);
        void AddEnchantmentDurations(Item* item);
        void RemoveEnchantmentDurations(Item* item);
        void RemoveAllEnchantments(EnchantmentSlot slot);
        void AddEnchantmentDuration(Item* item, EnchantmentSlot slot, uint32 duration);
        void SendEnchantmentDurations() const;
        void AddItemDurations(Item* item);
        void RemoveItemDurations(Item* item);
        void SendItemDurations() const;
        uint32 CountFreeInventorySlots() const;
    public:
        Item* AddItem(uint32 itemId, uint32 count = 1);
        void InterruptSpellsWithCastItem(Item* item);
        uint8 FindEquipSlot(ItemPrototype const* proto, uint32 slot, bool swap) const;
        uint32 GetItemCount(uint32 item, bool inBankAlso = false, Item* skipItem = nullptr) const;
        Item* GetItemByGuid(ObjectGuid guid) const;
        Item* GetItemByPos(uint16 pos) const;
        Item* GetItemByPos(uint8 bag, uint8 slot) const;
        Item* GetWeaponForAttack(WeaponAttackType attackType) const { return GetWeaponForAttack(attackType,false,false); }
        Item* GetWeaponForAttack(WeaponAttackType attackType, bool nonbroken, bool useable) const;
        static uint32 GetAttackBySlot(uint8 slot);        // MAX_ATTACK if not weapon slot
        std::vector<Item*> &GetItemUpdateQueue() { return m_itemUpdateQueue; }
        static bool IsInventoryPos(uint16 pos) { return IsInventoryPos(pos >> 8, pos & 255); }
        static bool IsInventoryPos(uint8 bag, uint8 slot);
        static bool IsEquipmentPos(uint16 pos) { return IsEquipmentPos(pos >> 8, pos & 255); }
        static bool IsEquipmentPos(uint8 bag, uint8 slot);
        static bool IsBagPos(uint16 pos);
        static bool IsBankPos(uint16 pos) { return IsBankPos(pos >> 8, pos & 255); }
        static bool IsBankPos(uint8 bag, uint8 slot);
        bool IsValidPos(uint16 pos, bool explicit_pos) const { return IsValidPos(pos >> 8, pos & 255, explicit_pos); }
        bool IsValidPos(uint8 bag, uint8 slot, bool explicit_pos) const;
        uint8 GetBankBagSlotCount() const { return GetByteValue(PLAYER_BYTES_2, 2); }
        void SetBankBagSlotCount(uint8 count) { SetByteValue(PLAYER_BYTES_2, 2, count); }
        bool HasItemCount(const uint32 item, const uint32 count = 1, const bool inBankAlso = false) const;
        void ApplyForAllItems(std::function<void(Item*)> func, bool inBank = false) const;
        bool HasItemFitToSpellReqirements(SpellEntry const* spellInfo, Item const* ignoreItem = nullptr);
        bool HasItemWithIdEquipped(uint32 item, uint32 count = 1, uint8 except_slot = NULL_SLOT) const;
        InventoryResult CanStoreNewItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 item, uint32 count, uint32* no_space_count = nullptr) const
        {
            return _CanStoreItem(bag, slot, dest, item, count, nullptr, false, no_space_count);
        }
        InventoryResult CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap = false) const
        {
            if(!pItem)
                return EQUIP_ERR_ITEM_NOT_FOUND;
            uint32 count = pItem->GetCount();
            return _CanStoreItem(bag, slot, dest, pItem->GetEntry(), count, pItem, swap, nullptr);

        }
        InventoryResult CanStoreItems(Item** pItem,int count) const;
        InventoryResult CanEquipNewItem(uint8 slot, uint16 &dest, uint32 item, bool swap) const;
        InventoryResult CanEquipItem(uint8 slot, uint16 &dest, Item* pItem, bool swap, bool not_loading = true) const;
        InventoryResult CanEquipItem(uint8 slot, uint16& dest, ItemPrototype const* pProto, Item* pItem = nullptr, bool swap = false, bool not_loading = true) const;
        InventoryResult CanUnequipItems(uint32 item, uint32 count) const;
        InventoryResult CanUnequipItem(uint16 src, bool swap) const;
        InventoryResult CanBankItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap, bool not_loading = true) const;
        InventoryResult CanUseItem(Item* pItem, bool not_loading = true) const;
        InventoryResult CanUseItem(ItemPrototype const* pItem, bool not_loading = true) const;
        InventoryResult CanUseAmmo(uint32 item) const;
        Item* StoreNewItem(ItemPosCountVec const& pos, uint32 item, bool update,int32 randomPropertyId = 0);
        Item* StoreItem(ItemPosCountVec const& pos, Item* pItem, bool update);
        Item* EquipNewItem(uint16 pos, uint32 item, bool update);
        Item* EquipItem(uint16 pos, Item* pItem, bool update);
        void AutoUnequipWeaponsIfNeed();
        void AutoUnequipOffhandIfNeed();
        void AutoUnequipItemFromSlot(uint32 slot, bool sendMail = true);
        void SatisfyItemRequirements(ItemPrototype const* pItem);
        bool StoreNewItemInBestSlots(uint32 item_id, uint32 item_count, uint32 enchantId = 0);
        Item* StoreNewItemInInventorySlot(uint32 itemEntry, uint32 amount);
        void AutoStoreLoot(Loot& loot, bool broadcast = false, uint8 bag = NULL_BAG, uint8 slot = NULL_SLOT);
        void SetAmmo(uint32 item);
        void RemoveAmmo();
        void SendProficiency(ItemClass itemClass, uint32 itemSubclassMask) const;
        unsigned int GetShapeshiftDisplay(ShapeshiftForm form);
        Item* BankItem(ItemPosCountVec const& dest, Item* pItem, bool update)
        {
            return StoreItem(dest, pItem, update);
        }
        void RemoveItem(const uint8 bag, const uint8 slot, const bool update = true);
        void MailHardcoreModeRewards(uint32 level);
        void MailVagrantModeRewards(uint32 level);
        void MailBoaringModeRewards(uint32 level);
        void MailBrewmasterModeRewards();
        void AnnounceHardcoreModeLevelUp(uint32 level);
        // Titles
        bool IsCityProtector();
        bool IsScarabLord();
        bool IsImmortal();
        // ---
        void MailCityProtectorMedallion();
        bool IsIgnoringTitles();
        void SetIgnoringTitles(bool shouldIgnore);
        void MoveItemFromInventory(uint8 bag, uint8 slot, bool update);
        bool IsInMainCity();
        void SetFlying(bool apply);
        bool IsFlying() const { return m_isFlying; }
        bool m_isFlying = false;
        // in trade, auction, guild bank, mail....
        Item* MoveItemToInventory(ItemPosCountVec const& dest, Item* pItem, bool update, bool in_characterInventoryDB = false);
        void DestroyItem(uint8 bag, uint8 slot, bool update);
        void DestroyItemCount(const uint32 item, const uint32 count, const bool update, const bool unequip_check = false, const bool check_bank = false);
        void DestroyItemCount(Item* item, uint32& count, bool update);
        /**
         * @brief Destroys equipped item $itemId and updates the Player
         * @param itemId
         * @return true if an item got deleted
         */
        bool DestroyEquippedItem(uint32 itemId);
        void DestroyConjuredItems(bool update);
        void DestroyZoneLimitedItem(bool update, uint32 new_zone);
        void SplitItem(uint16 src, uint16 dst, uint32 count);
        void SwapItem(uint16 src, uint16 dst);
        void AddItemToBuyBackSlot(Item* pItem, uint32 money, ObjectGuid vendorGuid);
        Item* GetItemFromBuyBackSlot(uint32 slot);
        void RemoveItemFromBuyBackSlot(uint32 slot, bool del);
        uint32 GetBuyBackItemPrice(uint32 slot) const { return GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + slot - BUYBACK_SLOT_START); }

        uint32 GetMaxKeyringSize() const { return KEYRING_SLOT_END - KEYRING_SLOT_START; }

        void SendEquipError(InventoryResult msg, Item* pItem, Item* pItem2 = nullptr, uint32 itemid = 0) const;
        void SendBuyError(BuyResult msg, Creature* pCreature, uint32 item, uint32 param) const;
        void SendSellError(SellResult msg, Creature* pCreature, ObjectGuid itemGuid, uint32 param) const;
        void SendOpenContainer() const;
        void AddWeaponProficiency(uint32 newflag) { m_WeaponProficiency |= newflag; }
        void AddArmorProficiency(uint32 newflag) { m_ArmorProficiency |= newflag; }
        uint32 GetWeaponProficiency() const { return m_WeaponProficiency; }
        uint32 GetArmorProficiency() const { return m_ArmorProficiency; }
        bool IsTwoHandUsed() const
        {
            Item* mainItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            return mainItem && mainItem->GetProto()->InventoryType == INVTYPE_2HWEAPON;
        }
        void SendNewItem(Item* item, uint32 count, bool received, bool created, bool broadcast = false, bool showInChat = true);
        bool BuyItemFromVendor(ObjectGuid vendorGuid, uint32 item, uint8 count, uint8 bag, uint8 slot);
        void OnReceivedItem(Item* item);
        float GetAverageItemLevel() const;

        const auto& GetItemLogs() const { return m_itemLogs; }

        void LogItem(Item* item, LogItemAction action, uint32 count = 0);

        float GetReputationPriceDiscount(Creature const* pCreature) const;
        // bot passes FactionTemplateEntry. Stub.
        float GetReputationPriceDiscount(FactionTemplateEntry const* /*ft*/) const { return 1.0f; }

        Player* GetTrader() const { return m_trade ? m_trade->GetTrader() : nullptr; }
        TradeData* GetTradeData() const { return m_trade; }
        // Server-side / scriptable trade initiation: opens the trade window with
        // `other` using the same preconditions as the CMSG_INITIATE_TRADE handler,
        // then leaves items, gold and acceptance to the normal trade path. Purely
        // additive -- changes no existing behaviour. False if a precondition fails.
        bool BeginTradeWith(Player* other);
        void TradeCancel(bool sendback, TradeStatus status = TRADE_STATUS_TRADE_CANCELED);

        uint32 GetTimeLoggedIn() const { return m_timeLoggedIn; }
        void AddLoggedInTime(uint32 diff) { m_timeLoggedIn += diff; }

        uint32 GetMoney() const { return GetUInt32Value(PLAYER_FIELD_COINAGE); }
        void LogModifyMoney(int32 d, const char* type, ObjectGuid fromGuid = ObjectGuid(), uint32 data = 0);
        void ModifyMoney(int32 d);
        void LootMoney(int32 g, Loot* loot);
        std::string GetShortDescription() const; // "player:guid [username:accountId@IP]"

        void SetMoney(uint32 value)
        {
            SetUInt32Value(PLAYER_FIELD_COINAGE, value);
            MoneyChanged(value);
        }

        ObjectGuid const& GetLootGuid() const { return m_lootGuid; }
        void SetLootGuid(ObjectGuid const& guid) { m_lootGuid = guid; }
        void RemovedInsignia(Player* looterPlr, Corpse* corpse);
        void SendLoot(ObjectGuid guid, LootType loot_type, Player* pVictim = nullptr);
        void SendLootRelease(ObjectGuid guid) const;
        void SendLootError(ObjectGuid guid, LootError error) const;
        void SendNotifyLootItemRemoved(uint8 lootSlot) const;
        void SendNotifyLootMoneyRemoved() const;
        bool IsAllowedToLoot(Creature const* creature);
        float GetMaxLootDistance(Unit const* pUnit) const;

        void ApplyEnchantment(Item* item,EnchantmentSlot slot,bool apply, bool apply_dur = true, bool ignore_condition = false);
        void ApplyEnchantment(Item* item,bool apply);

        void DurabilityLossAll(double percent, bool inventory);
        void DurabilityLoss(Item* item, double percent);
        void DurabilityPointsLossAll(int32 points, bool inventory);
        void DurabilityPointsLoss(Item* item, int32 points);
        void DurabilityPointLossForEquipSlot(EquipmentSlots slot);
        uint32 DurabilityRepairAll(bool cost, float discountMod);
        uint32 DurabilityRepair(uint16 pos, bool cost, float discountMod);

        /*********************************************************/
        /***                    GOSSIP SYSTEM                  ***/
        /*********************************************************/

    public:
        void PrepareGossipMenu(WorldObject* pSource, uint32 menuId = 0);
        void SendPreparedGossip(WorldObject* pSource);
        void OnGossipSelect(WorldObject* pSource, uint32 gossipListId);

        uint32 GetGossipTextId(uint32 menuId, WorldObject* source);
        static uint32 GetGossipTextId(WorldObject* pSource);
        PlayerMenu* PlayerTalkClass;

        /*********************************************************/
        /***                    QUEST SYSTEM                   ***/
        /*********************************************************/

    private:
        // We allow only one timed quest active at the same time. Below can then be simple value instead of set.
        typedef std::set<uint32> QuestSet;
        QuestSet m_timedquests;

        ObjectGuid m_dividerGuid;
        uint32 m_ingametime;
        QuestStatusMap mQuestStatus;
        void AdjustQuestReqItemCount(Quest const* pQuest, QuestStatusData& questStatusData);
        bool CanGiveQuestSourceItemIfNeed(Quest const* pQuest, ItemPosCountVec* dest = nullptr) const;
        void GiveQuestSourceItemIfNeed(Quest const* pQuest);

    public:
        uint16 FindQuestSlot(uint32 quest_id) const;
        uint32 GetQuestSlotQuestId(uint16 slot) const { return GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET); }
        void SetQuestSlot(uint16 slot, uint32 quest_id, uint32 timer = 0)
        {
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_ID_OFFSET, quest_id);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, 0);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer);
        }
        void SetQuestSlotCounter(uint16 slot, uint8 counter, uint8 count)
        {
            uint32 val = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET);
            val &= ~(0x3F << (counter * 6));
            val |= ((uint32)count << (counter * 6));
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, val);
        }
        void SetQuestSlotState(uint16 slot, uint8 state) { SetByteFlag(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, 3, state); }
        void RemoveQuestSlotState(uint16 slot, uint8 state) { RemoveByteFlag(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, 3, state); }
        void SetQuestSlotTimer(uint16 slot, uint32 timer) { SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer); }
    public:
        uint32 GetQuestLevelForPlayer(Quest const* pQuest) const { return pQuest && (pQuest->GetQuestLevel() > 0) ? pQuest->GetQuestLevel() : GetLevel(); }
        void PrepareQuestMenu(ObjectGuid guid, uint32 exceptQuestId = 0);
        void SendPreparedQuest(ObjectGuid guid);
        bool IsActiveQuest(uint32 quest_id) const;        // can be taken or taken
        bool IsCurrentQuest(uint32 quest_id, uint8 completedOrNot = 0) const;
        Quest const* GetNextQuest(ObjectGuid guid, Quest const* pQuest);
        bool CanSeeStartQuest(Quest const* pQuest) const;
        bool CanTakeQuest(Quest const* pQuest, bool msg, bool skipStatusCheck = false) const;
        bool CanAddQuest(Quest const* pQuest, bool msg) const;
        bool CanCompleteQuest(uint32 quest_id) const;
        bool CanCompleteRepeatableQuest(Quest const* pQuest) const;
        bool CanRewardQuest(Quest const* pQuest, bool msg) const;
        bool CanRewardQuest(Quest const* pQuest, uint32 reward, bool msg) const;
        void AddQuest(Quest const* pQuest, Object* questGiver);
        void FullQuestComplete(uint32 questId); // Equivalent to .quest complete
        void CompleteQuest(uint32 quest_id);
        void RemoveQuest(uint32 quest_id);
        void RemoveQuestAtSlot(uint32 slot);
        void IncompleteQuest(uint32 quest_id);
        void RewardQuest(Quest const* pQuest, uint32 reward, WorldObject* questGiver, bool announce = true);
        void FailQuest(uint32 quest_id);

        bool SatisfyQuestChallenges(Quest const* quest, bool msg) const;
        bool SatisfyQuestSkill(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestCondition(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLevel(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLog(bool msg) const;
        bool SatisfyQuestPreviousQuest(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestClass(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestRace(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestReputation(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestStatus(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestTimed(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestExclusiveGroup(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestNextChain(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestPrevChain(Quest const* qInfo, bool msg) const;
        bool TakeOrReplaceQuestStartItems(const uint32 quest_id, const bool msg, const bool giveQuestStartItem);
        bool GetQuestRewardStatus(uint32 quest_id) const;
        const QuestStatusData* GetQuestStatusData(uint32 quest_id) const;
        QuestStatus GetQuestStatus(uint32 quest_id) const;
        void SetQuestStatus(uint32 quest_id, QuestStatus status);

        void SwapQuestSlot(uint16 slot1,uint16 slot2)
        {
            for (int i = 0; i < MAX_QUEST_OFFSET ; ++i)
            {
                uint32 temp1 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot1 + i);
                uint32 temp2 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot2 + i);

                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot1 + i, temp2);
                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot2 + i, temp1);
            }
        }

        bool IsAtGroupRewardDistance(WorldObject const* pRewardSource) const;
        void RewardSinglePlayerAtKill(Unit* pVictim);
        void RewardPlayerAndGroupAtEvent(uint32 creature_id, WorldObject* pRewardSource);
        void RewardPlayerAndGroupAtCast(WorldObject* pRewardSource, uint32 spellid = 0);
        void AreaExploredOrEventHappens(uint32 questId);
        void GroupEventHappens(uint32 questId, WorldObject const* pEventObject);
        void GroupEventFailHappens(uint32 questId);
        void ItemAddedQuestCheck(uint32 entry, uint32 count);
        void ItemRemovedQuestCheck(uint32 entry, uint32 count);
        void KilledMonster(CreatureInfo const* cInfo, ObjectGuid guid);
        void KilledMonsterCredit(uint32 entry, ObjectGuid guid = ObjectGuid());
        void CastedCreatureOrGO(uint32 entry, ObjectGuid guid, uint32 spell_id, bool original_caster = true);
        void TalkedToCreature(uint32 entry, ObjectGuid guid);
        void MoneyChanged(uint32 value);
        void ReputationChanged(FactionEntry const* factionEntry);
        bool HasQuestForItem(uint32 itemid) const;
        bool HasQuestForGO(int32 GOId) const;
        void UpdateForQuestWorldObjects();
        bool CanShareQuest(uint32 quest_id) const;
        QuestStatusMap& getQuestStatusMap() { return mQuestStatus; };
        QuestStatusMap& GetQuestStatusMap() { return mQuestStatus; }

        void SendQuestCompleteEvent(uint32 quest_id) const;
        void SendQuestReward(Quest const* pQuest, uint32 XP, Object* questGiver) const;
        void SendQuestFailed(uint32 quest_id) const;
        void SendQuestFailedAtTaker(uint32 quest_id, uint32 reason = INVALIDREASON_DONT_HAVE_REQ) const;
        void SendQuestTimerFailed(uint32 quest_id) const;
        void SendCanTakeQuestResponse(uint32 msg) const;
        void SendQuestConfirmAccept(Quest const* pQuest, Player* pReceiver) const;
        void SendPushToPartyResponse(Player* pPlayer, uint8 msg) const;
        void SendQuestUpdateAddItem(Quest const* pQuest, uint32 item_idx, uint32 current, uint32 count);
        void SendQuestUpdateAddCreatureOrGo(Quest const* pQuest, ObjectGuid guid, uint32 creatureOrGO_idx, uint32 count);

        ObjectGuid GetDividerGuid() const { return m_dividerGuid; }
        void SetDividerGuid(ObjectGuid guid) { m_dividerGuid = guid; }
        void ClearDividerGuid() { m_dividerGuid.Clear(); }

        uint32 GetInGameTime() const { return m_ingametime; }
        void SetInGameTime(uint32 time) { m_ingametime = time; }

        void AddTimedQuest(uint32 quest_id) { m_timedquests.insert(quest_id); }
        void RemoveTimedQuest(uint32 quest_id) { m_timedquests.erase(quest_id); }

        /*********************************************************/
        /***                   LOAD SYSTEM                     ***/
        /*********************************************************/

    private:
        void LoadAura(AuraSaveStruct& saveStruct, uint32 timediff);
        bool SaveAura(SpellAuraHolder* holder, AuraSaveStruct& saveStruct);
        void _LoadAuras(QueryResult* result, uint32 timediff);
        void _LoadBoundInstances(QueryResult* result);
        void _LoadInventory(QueryResult* result, uint32 timediff, bool& has_epic_mount);
        void _LoadItemLoot(QueryResult* result);
        void _LoadQuestStatus(QueryResult* result);
        void _LoadGroup(QueryResult* result);
        void _LoadSkills(QueryResult* result);
        void _LoadForgottenSkills(QueryResult* result);
        void LoadSkillsFromFields();
        void _LoadSpells(QueryResult* result);
        void _LoadPlayerVariables(QueryResult* result);
        void _LoadPlayerItemLogs(QueryResult* result);
        bool _LoadHomeBind(QueryResult* result);
        void _LoadBGData(QueryResult* result);
        void _LoadIntoDataField(const char* data, uint32 startOffset, uint32 count);
        void _LoadGuild(QueryResult* result);
        uint32 m_atLoginFlags;
    public:
        bool LoadFromDB(ObjectGuid guid, SqlQueryHolder* holder);
        void SendPacketsAtRelogin();
        static uint32 GetZoneIdFromDB(ObjectGuid guid);
        static uint32 GetLevelFromDB(ObjectGuid guid);
        void Initialize(uint32 guid);
        static bool LoadPositionFromDB(ObjectGuid guid, uint32& mapid, float& x,float& y,float& z,float& o, bool& in_flight);
        void LoadCorpse();
        void LoadPet();
        bool HasAtLoginFlag(AtLoginFlags f) const { return m_atLoginFlags & f; }
        void SetAtLoginFlag(AtLoginFlags f) { m_atLoginFlags |= f; }
        void RemoveAtLoginFlag(AtLoginFlags f, bool in_db_also = false);
        bool InGurubashiArena(bool checkOutsideArea) const;
  
        /*********************************************************/
        /***                   SAVE SYSTEM                     ***/
        /*********************************************************/
        
    private:
        void _SaveAuras();
        void _SaveInventory();
        void _SaveQuestStatus();
        void _SaveSkills();
        void _SaveSpells();
        void _SaveBGData();
        void _SaveStats();

        void _SetCreateBits(UpdateMask* updateMask, Player* target) const override;
        void _SetUpdateBits(UpdateMask* updateMask, Player* target) const override;
        uint32 m_nextSave;
    public:
        bool SaveToDB(bool online = true, bool force = false, bool direct = false);
        void SaveInventoryAndGoldToDB();                    // fast save function for item/money cheating preventing
        void SaveGoldToDB();
        static void SavePositionInDB(ObjectGuid guid, uint32 mapid, float x,float y,float z,float o,uint32 zone);

        static void DeleteFromDB(ObjectGuid playerguid, uint32 accountId, bool updateRealmChars = true, bool deleteFinally = false);
        static void DeleteOldCharacters();
        static void DeleteOldCharacters(uint32 keepDays);

        uint32 GetSaveTimer() const { return m_nextSave; }
        void   SetSaveTimer(uint32 timer) { m_nextSave = timer; }

        /*********************************************************/
        /***                    PET SYSTEM                     ***/
        /*********************************************************/

    private:
        ObjectGuid m_miniPetGuid;
        uint32 m_temporaryUnsummonedPetNumber;
        void SendPetSkillWipeConfirm() const;
    public:
        void SendPetTameFailure(PetTameFailureReason reason) const;
        void PetSpellInitialize();
        void CharmSpellInitialize();
        void PossessSpellInitialize();
        void RemovePetActionBar();

        // Take possession of a new spawned creature
        Creature* SummonPossessedMinion(uint32 creatureId, uint32 spellId, float x, float y, float z, float ang, uint32 duration);
        void UnsummonPossessedMinion();

        uint32 m_stableSlots;
        uint32 m_petEntry;
        uint32 m_petSpell;

        void RemovePet(PetSaveMode mode);
        void RemoveMiniPet();
        Pet* GetMiniPet() const override;
        void AutoReSummonPet();

        // use only in Pet::Unsummon/Spell::DoSummon
        void _SetMiniPet(Pet* pet) { m_miniPetGuid = pet ? pet->GetObjectGuid() : ObjectGuid(); }

        // Temporarily removed pet cache
        uint32 GetTemporaryUnsummonedPetNumber() const { return m_temporaryUnsummonedPetNumber; }
        void SetTemporaryUnsummonedPetNumber(uint32 petnumber) { m_temporaryUnsummonedPetNumber = petnumber; }
        void UnsummonPetTemporaryIfAny();
        void ResummonPetTemporaryUnSummonedIfAny();
        bool IsPetNeedBeTemporaryUnsummoned() const;
        
        /*********************************************************/
        /***                   SPELL SYSTEM                    ***/
        /*********************************************************/

    private:
        PlayerSpellMap m_spells;
        float m_auraBaseMod[BASEMOD_END][MOD_END];
        SpellModList m_spellMods[MAX_SPELLMOD];
        uint32 m_lastFromClientCastedSpellID;
        void _LoadSpellCooldowns(QueryResult* result);
        void _SaveSpellCooldowns();

        void _LoadPlayerSavedSpecs(QueryResult* result);

        bool IsNeedCastPassiveLikeSpellAtLearn(SpellEntry const* spellInfo) const;
        void SendInitialSpells() const;
        bool AddSpell(uint32 spell_id, bool active, bool learning, bool dependent, bool disabled);
    public:
        bool HasSpell(uint32 spell) const override;
        bool HasActiveSpell(uint32 spell) const;            // show in spellbook
        TrainerSpellState GetTrainerSpellState(TrainerSpell const* trainer_spell) const;
        // cmangos passes (spell, reqLevel); ignore reqLevel.
        TrainerSpellState GetTrainerSpellState(TrainerSpell const* trainer_spell, uint32 /*reqLevel*/) const { return GetTrainerSpellState(trainer_spell); }
        bool IsSpellFitByClassAndRace(uint32 spell_id, uint32* pReqlevel = nullptr) const;
        bool IsImmuneToSpellEffect(SpellEntry const* spellInfo, SpellEffectIndex index, bool castOnSelf) const override;
        void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs) override;
        void SendClearCooldown(uint32 spell_id, Unit* target) const;
        void SendClearAllCooldowns(Unit* target) const;
        void SendSpellCooldown(uint32 spellId, uint32 cooldown, ObjectGuid target) const;
        void SendSpellRemoved(uint32 spell_id) const;

        void LearnSpell(uint32 spell_id, bool dependent, bool talent = false);
        void RemoveSpell(uint32 spell_id, bool disabled = false, bool learn_low_rank = true, bool hardReset = false);
        // cmangos camelCase + 2-arg form.
        void removeSpell(uint32 spell_id, bool disabled = false, bool learn_low_rank = true) { RemoveSpell(spell_id, disabled, learn_low_rank); }
        void ResetSpells();
        void LearnDefaultSpells();
        void LearnQuestRewardedSpells();
        void LearnQuestRewardedSpells(Quest const* quest);
        void LearnGameMasterSpells();
        void LearnSpellHighRank(uint32 spellid);
        uint32 GetSpellRank(SpellEntry const* spellInfo) final;

        bool HasDamagingWeaponProc() const;
        void CastItemCombatSpell(Unit* Target, WeaponAttackType attType, float chanceMultiplier = 1.0f);
        void CastItemUseSpell(Item* item, SpellCastTargets const& targets);
        // AzerothCore appends the client cast counter and a glyph index. The
        // counter is an echo this call path never needs, glyphs are 3.x.
        void CastItemUseSpell(Item* item, SpellCastTargets const& targets, uint8 /*castCount*/, uint32 /*glyphIndex*/)
        { CastItemUseSpell(item, targets); }

        // needed by vanish and improved sap
        void CastHighestStealthRank();

        PlayerSpellMap const& GetSpellMap() const { return m_spells; }
        PlayerSpellMap      & GetSpellMap() { return m_spells; }

        void AddSpellMod(SpellModifier* mod, bool apply);
        void SendSpellMod(SpellModifier const* mod) const;
        bool HasInstantCastingSpellMod(SpellEntry const* spellInfo) const;
        bool IsAffectedBySpellmod(SpellEntry const* spellInfo, SpellModifier* mod, Spell* spell = nullptr) const;
        template <class T> T ApplySpellMod(uint32 spellId, SpellModOp op, T &basevalue, Spell* spell = nullptr);
        SpellModifier* GetSpellMod(SpellModOp op, uint32 spellId) const;
        void RemoveSpellMods(Spell* spell);
        void RestoreSpellMods(Spell* spell, uint32 ownerAuraId = 0, Aura* aura = nullptr);
        void RestoreAllSpellMods(uint32 ownerAuraId = 0, Aura* aura = nullptr);
        void DropModCharge(SpellModifier* mod, Spell* spell);

        std::vector<ItemSetEffect*> m_ItemSetEff;

        /*********************************************************/
        /***                   TALENT SYSTEM                   ***/
        /*********************************************************/

    private:
        mutable uint32 m_resetTalentsMultiplier;
        time_t m_resetTalentsTime;
        uint32 m_usedTalentCount;
        uint32 m_extraBonusTalentCount;

    public:
        void UpdateFreeTalentPoints(bool resetIfNeed = true);
    private:
        void UpdateResetTalentsMultiplier() const;
        // moved to public; bot's Talentspec.h
        // calls this on bot Player instances. No encapsulation concern (pure getter).
        void SendTalentWipeConfirm(ObjectGuid guid) const;
    public:
        uint32 GetResetTalentsCost() const;
        uint32 CalculateTalentsPoints() const;
        uint32 GetFreeTalentPoints() const { return GetUInt32Value(PLAYER_CHARACTER_POINTS1); }
        void SetFreeTalentPoints(uint32 points) { SetUInt32Value(PLAYER_CHARACTER_POINTS1, points); }
        void SetBonusTalentCount(uint32 count) { m_extraBonusTalentCount = count; UpdateFreeTalentPoints(); }
        uint32 GetBonusTalentCount() const { return m_extraBonusTalentCount; }
        void AddBonusTalent(uint32 count) { m_extraBonusTalentCount += count; UpdateFreeTalentPoints(); }
        void RemoveBonusTalent(uint32 count) { m_extraBonusTalentCount = count >= m_extraBonusTalentCount ? 0 : m_extraBonusTalentCount - count; UpdateFreeTalentPoints(); }
        bool ResetTalents(bool no_cost = false);
        void InitTalentForLevel();
        void LearnTalent(uint32 talentId, uint32 talentRank);

        /*********************************************************/
        /***                    STAT SYSTEM                    ***/
        /*********************************************************/

    private:
        float m_modManaRegen;
        float m_modManaRegenInterrupt;
        float m_SpellCritPercentage[MAX_SPELL_SCHOOL];
        float m_carryHealthRegen;
        ObjectGuid m_comboTargetGuid;
        int8 m_comboPoints;
        uint32 m_weaponChangeTimer;
        bool m_canParry;
        bool m_canBlock;
        bool m_canDualWield;
        float m_ammoDPS;
        uint32 m_foodEmoteTimer;

        void RegenerateAll();
        void Regenerate(Powers power);
        void RegenerateHealth();
        void HandleFoodEmotes(uint32 diff);

        static float GetHealthBonusFromStamina(float stamina);
        static float GetManaBonusFromIntellect(float intellect);
        float GetMeleeCritFromAgility() const;
        float GetDodgeFromAgility() const;
        float GetSpellCritFromIntellect() const;
        void InitStatBuffMods()
        {
            for (int i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(PLAYER_FIELD_POSSTAT0 + i, 0);
            for (int i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(PLAYER_FIELD_NEGSTAT0 + i, 0);
        }
        void _ApplyAllStatBonuses();
        void _RemoveAllStatBonuses();
        void _ApplyItemMods(Item* item, uint8 slot, bool apply);
        void _RemoveAllItemMods();
        void _ApplyAllItemMods();
        void _ApplyItemBonuses(ItemPrototype const* proto, uint8 slot, bool apply);
        void _ApplyAmmoBonuses();
    public:
        void GiveXP(uint32 xp, Unit* victim);
        void GiveNegativeXP(uint32 percent, Unit* victim);
        void GiveLevel(uint32 level);
        void InitStatsForLevel(bool reapplyMods = false);


        void CheckInfernoInvite();

        void RewardRage(uint32 damage, bool attacker);
        uint8 GetComboPoints() const { return m_comboPoints; }
        float GetManaRegen() const { return m_modManaRegen; }
        ObjectGuid const& GetComboTargetGuid() const { return m_comboTargetGuid; }

        void AddComboPoints(Unit* target, int8 count);
        void ClearComboPoints();
        void SetComboPoints();

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateManaRegen() override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType) override;
        void UpdateSpellDamageAndHealingBonus();
        void UpdateDefenseBonusesMod();
        void UpdateBlockPercentage();
        void UpdateCritPercentage(WeaponAttackType attType);
        void UpdateAllCritPercentages();
        void UpdateParryPercentage();
        void UpdateDodgePercentage();
        void UpdateAllSpellCritChances();
        void UpdateSpellCritChance(uint32 school);
        void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, float& min_damage, float& max_damage, uint8 index = 0) const;

        uint32 GetShieldBlockValue() const override;                 // overwrite Unit version (virtual)
        bool CanParry() const { return m_canParry; }
        void SetCanParry(bool value);
        bool CanBlock() const { return m_canBlock; }
        void SetCanBlock(bool value);
        bool CanDualWield() const { return m_canDualWield; }
        void SetCanDualWield(bool value) { m_canDualWield = value; }

        void ApplyStatBuffMod(Stats stat, float val, bool apply) { ApplyModSignedFloatValue((val > 0 ? PLAYER_FIELD_POSSTAT0 + stat : PLAYER_FIELD_NEGSTAT0 + stat), val, apply); }
        void ApplyStatPercentBuffMod(Stats stat, float val, bool apply)
        {
            ApplyPercentModFloatValue(PLAYER_FIELD_POSSTAT0 + stat, val, apply);
            ApplyPercentModFloatValue(PLAYER_FIELD_NEGSTAT0 + stat, val, apply);
        }
        float GetPosStat(Stats stat) const { return GetFloatValue(PLAYER_FIELD_POSSTAT0 + stat); }
        float GetNegStat(Stats stat) const { return GetFloatValue(PLAYER_FIELD_NEGSTAT0 + stat); }
        float GetResistanceBuffMods(SpellSchools school, bool positive) const { return GetFloatValue(positive ? PLAYER_FIELD_RES_BUFF_MODS_POSITIVE + school : PLAYER_FIELD_RES_BUFF_MODS_NEGATIVE + school); }
        void SetResistanceBuffMods(SpellSchools school, bool positive, float val) { SetFloatValue(positive ? PLAYER_FIELD_RES_BUFF_MODS_POSITIVE + school : PLAYER_FIELD_RES_BUFF_MODS_NEGATIVE + school, val); }
        void ApplyResistanceBuffModsMod(SpellSchools school, bool positive, float val, bool apply) { ApplyModSignedFloatValue(positive ? PLAYER_FIELD_RES_BUFF_MODS_POSITIVE + school : PLAYER_FIELD_RES_BUFF_MODS_NEGATIVE + school, val, apply); }
        void ApplyResistanceBuffModsPercentMod(SpellSchools school, bool positive, float val, bool apply) { ApplyPercentModFloatValue(positive ? PLAYER_FIELD_RES_BUFF_MODS_POSITIVE + school : PLAYER_FIELD_RES_BUFF_MODS_NEGATIVE + school, val, apply); }

        float GetAmmoDPS() const { return m_ammoDPS; }
        void SetRegularAttackTime(bool resetTimer = true);
        void SetBaseModValue(BaseModGroup modGroup, BaseModType modType, float value) { m_auraBaseMod[modGroup][modType] = value; }
        void HandleBaseModValue(BaseModGroup modGroup, BaseModType modType, float amount, bool apply);
        float GetBaseModValue(BaseModGroup modGroup, BaseModType modType) const;
        float GetTotalBaseModValue(BaseModGroup modGroup) const;
        float GetTotalPercentageModValue(BaseModGroup modGroup) const { return m_auraBaseMod[modGroup][FLAT_MOD] + m_auraBaseMod[modGroup][PCT_MOD]; }

        void _ApplyWeaponDependentAuraMods(Item* item, WeaponAttackType attackType, bool apply);
        void _ApplyWeaponDependentAuraCritMod(Item* item, WeaponAttackType attackType, Aura* aura, bool apply);
        void _ApplyWeaponDependentAuraDamageMod(Item* item, WeaponAttackType attackType, Aura* aura, bool apply);
        void _ApplyWeaponDependentAuraResistanceMod(Item* item, WeaponAttackType attackType, Aura* aura, bool apply);

        void InitDataForForm(bool reapplyMods = false);
        void ApplyItemEquipSpell(Item* item, bool apply, bool form_change = false);
        void ApplyEquipSpell(SpellEntry const* spellInfo, Item* item, bool apply, bool form_change = false);
        void UpdateEquipSpellsAtFormChange();
        void outDebugStatsValues() const;

        float GetSpellCritPercent(SpellSchools school) const { return m_SpellCritPercentage[school]; }
        void SetSpellCritPercent(SpellSchools school, float percent) { m_SpellCritPercentage[school] = percent; }

        /*********************************************************/
        /***                   SKILLS SYSTEM                   ***/
        /*********************************************************/

    public:
        void InitPrimaryProfessions();
    private:
        void UpdateSkillTrainedSpells(uint16 id, uint16 currVal);                                   // learns/unlearns spells dependent on a skill
        void UpdateSpellTrainedSkills(uint32 spellId, bool apply, bool hardReset = false);                                  // learns/unlearns skills dependent on a spell
        void UpdateOldRidingSkillToNew(bool has_epic_mount);
    public:
        void UpdateSkillsForLevel();
        void UpdateSkillsForLevel(uint32 /*level*/) { UpdateSkillsForLevel(); }
    private:
        SkillStatusMap mSkillStatus;
        std::unordered_map<uint16, uint16> m_mForgottenSkills;


        std::array<std::vector<uint32>, 4> m_savedSpecSpells;
        
    public:
        uint32 GetFreePrimaryProfessionPoints() const { return GetUInt32Value(PLAYER_CHARACTER_POINTS2); }
        void SetFreePrimaryProfessions(uint16 profs) { SetUInt32Value(PLAYER_CHARACTER_POINTS2, profs); }

        bool UpdateSkill(uint32 skill_id, uint32 step);
        bool UpdateSkillPro(uint16 SkillId, int32 Chance, uint32 step);

        bool UpdateCraftSkill(uint32 spellid);
        bool UpdateGatherSkill(uint32 SkillId, uint32 SkillValue, uint32 RedLevel, uint32 Multiplicator = 1);
        bool UpdateFishingSkill();

        uint32 GetBaseDefenseSkillValue() const { return GetSkillValuePure(SKILL_DEFENSE); }
        uint32 GetBaseWeaponSkillValue(WeaponAttackType attType) const;

        void UpdateCombatSkills(Unit* pVictim, WeaponAttackType& attType, const bool defence);

        void SetSkill(uint16 id, uint16 currVal, uint16 maxVal, uint16 step = 0);
        uint16 GetSkill(uint16 id, bool bonusPerm, bool bonusTemp, bool max = false) const;
        uint16 GetPureMaxSkillValue(uint32 skill) const;    // max
        inline uint16 GetSkillValue(uint16 id) const { return GetSkill(id, true, true); }           // skill value + perm. bonus + temp bonus
        inline uint16 GetSkillValueBase(uint16 id) const { return GetSkill(id, true, false); }      // skill value + perm. bonus
        inline uint16 GetSkillValuePure(uint16 id) const { return GetSkill(id, false, false); }     // skill value
        inline uint16 GetSkillMax(uint16 id) const { return GetSkill(id, true, true, true); }       // skill max + perm. bonus + temp bonus
        inline uint16 GetSkillMaxPure(uint16 id) const { return GetSkill(id, false, false, true); } // skill max
        bool ModifySkillBonus(uint16 id, int16 diff, bool permanent = false);
        int16 GetSkillBonus(uint16 id, bool permanent = false) const;
        inline int16 GetSkillBonusPermanent(uint16 id) const { return GetSkillBonus(id, true); }    // skill perm. bonus
        inline int16 GetSkillBonusTemporary(uint16 id) const { return GetSkillBonus(id); }          // skill temp bonus
        bool HasSkill(uint16 id) const;

        void UpdateSkillsToMaxSkillsForLevel();             // for .levelup

        /*********************************************************/
        /***                  LOCATION SYSTEM                  ***/
        /*********************************************************/

    private:
        
        uint32 m_zoneUpdateTimer;

        bool m_BGQueueAllowed;

        // Current teleport data
        WorldLocation m_teleport_dest;
        // Invalidates map-owned work, including near teleports within one map.
        uint64 m_mapWorkGeneration = 0;
        uint32 m_lastAIUpdateMs = 0;
        uint32 m_backgroundAIDueSinceMs = 0;
        uint32 m_teleport_options;
        bool mSemaphoreTeleport_Near;
        bool mSemaphoreTeleport_Far;
        bool mPendingFarTeleport;

        uint32 m_DelayedOperations;
        bool m_bCanDelayTeleport;
        bool m_bHasDelayedTeleport;
        bool m_bHasBeenAliveAtDelayedTeleport;
        uint32 m_areaCheckTimer; // Trigger call to UpdateTerainEnvironmentFlags/CheckAreaExploreAndOutdoor

        // to fix an 1.12 client problem with transports
        // sometimes they need a refresh before being usable
        bool m_justBoarded;
        void SetJustBoarded(bool hasBoarded) { m_justBoarded = hasBoarded; }
        bool HasJustBoarded() { return m_justBoarded; }

        void SetCanDelayTeleport(bool setting) { m_bCanDelayTeleport = setting; }
        bool IsHasDelayedTeleport() const
        {
            // we should not execute delayed teleports for now dead players but has been alive at teleport
            // because we don't want player's ghost teleported from graveyard
            return m_bHasDelayedTeleport && (IsAlive() || !m_bHasBeenAliveAtDelayedTeleport);
        }

        bool SetDelayedTeleportFlagIfCan()
        {
            m_bHasDelayedTeleport = m_bCanDelayTeleport;
            m_bHasBeenAliveAtDelayedTeleport = IsAlive();
            return m_bHasDelayedTeleport;
        }

        void ScheduleDelayedOperation(uint32 operation)
        {
            if (operation < DELAYED_END)
                m_DelayedOperations |= operation;
        }

        bool IsAllowedToMove(Unit* unit) const;

        Unit* m_mover;
        GridReference<Player> m_gridRef;
        MapReference m_mapRef;

        float m_fallStartZ;
        uint32 m_bNextRelocationsIgnored;

        // Recall position
        uint32 m_recallMap;
        float  m_recallX;
        float  m_recallY;
        float  m_recallZ;
        float  m_recallO;

        time_t m_summon_expire;
        uint32 m_summon_mapid;
        float  m_summon_x;
        float  m_summon_y;
        float  m_summon_z;

        Camera m_camera;
        ObjectGuid m_pendingCameraUpdate;
        uint32 m_cameraUpdateTimer;
        float m_longSightRange;
        uint32 m_longSightSpell;

        // Homebind coordinates
        uint32 m_homebindMapId;
        uint16 m_homebindAreaId;
        float m_homebindX;
        float m_homebindY;
        float m_homebindZ;

        // knockback/jumping states
        bool launched;
        // not null only if player has knockback state
        float xy_speed;

        LiquidTypeEntry const* m_lastLiquid;
        uint8 m_isunderwater;

        void UpdateZoneDependentAuras();
        void UpdateAreaDependentAuras();                    // subzones
        void UpdateTerainEnvironmentFlags();
        void CheckAreaExploreAndOutdoor(void);
    public:
        void AddToWorld() override;
        void RemoveFromWorld() override;

        /* Switch from instanceId of same map.
        * Assumes that you can enter the map.
        * Should be called in a thread-safe environment (not in map update for example !)
        */
        bool SwitchInstance(uint32 newInstanceId);
        bool TeleportTo(uint32 mapid, float x, float y, float z, float orientation, uint32 options = 0);
        // AzerothCore's long form appends a unit to face and forceNewInstance.
        // This core picks the instance copy by binds alone, so the force flag
        // has nowhere to act: a hop between two copies of the SAME map lands in
        // the caller's current copy. That narrows one tool - the test
        // spectator's run-to-run hop on one map - and nothing else; hops across
        // maps work. Documented at the call site too.
        bool TeleportTo(uint32 mapid, float x, float y, float z, float orientation,
                        uint32 options, Unit* /*faceTarget*/, bool /*forceNewInstance*/ = false)
        { return TeleportTo(mapid, x, y, z, orientation, options); }
        template <class T>
        bool TeleportTo(T const& loc, uint32 options = 0)
        {
            return TeleportTo(loc.mapId, loc.x, loc.y, loc.z, loc.o, options);
        }

        // _NOT_ thread-safe. Must be executed by the map manager after map updates, since we
        // remove objects from the map
        bool ExecuteTeleportFar(ScheduledTeleportData* data);
        void SendNewWorld();
        void HandleReturnOnTeleportFail(WorldLocation const& oldLoc);

        void SetBGQueueAllowed(bool allow) { m_BGQueueAllowed = allow; }
        bool IsAllowedToQueueBGDueToTabard() { return m_BGQueueAllowed; };

        bool TeleportToBGEntryPoint();

        void UpdateZone(uint32 newZone,uint32 newArea);
        void UpdateArea(uint32 newArea);

        GridReference<Player> &GetGridRef() { return m_gridRef; }
        MapReference &GetMapRef() { return m_mapRef; }

        bool SetPosition(float x, float y, float z, float orientation, bool teleport = false);
        void SetBindPoint(ObjectGuid guid) const;

        WorldLocation& GetTeleportDest() { return m_teleport_dest; }
        bool IsBeingTeleported() const { return mSemaphoreTeleport_Near || mSemaphoreTeleport_Far || mPendingFarTeleport; }
        bool IsBeingTeleportedNear() const { return mSemaphoreTeleport_Near; }
        bool IsBeingTeleportedFar() const { return mSemaphoreTeleport_Far; }
        void SetSemaphoreTeleportNear(bool semphsetting);
        uint64 GetMapWorkGeneration() const { return m_mapWorkGeneration; }
        uint32 GetAIElapsed(uint32 now) const
        {
            return m_lastAIUpdateMs ? now - m_lastAIUpdateMs : 0;
        }
        uint32 ConsumeAIElapsed(uint32 now)
        {
            uint32 const diff = GetAIElapsed(now);
            m_lastAIUpdateMs = now;
            return diff;
        }
        uint32 BackgroundAIDueAge(uint32 now)
        {
            if (!m_backgroundAIDueSinceMs) m_backgroundAIDueSinceMs = now;
            return now - m_backgroundAIDueSinceMs;
        }
        void ClearBackgroundAIDueAge() { m_backgroundAIDueSinceMs = 0; }
        void SetSemaphoreTeleportFar(bool semphsetting);
        void SetPendingFarTeleport(bool pending) { mPendingFarTeleport = pending; }
        void ExecuteTeleportNear();
        void ProcessDelayedOperations();

        bool HasMovementFlag(MovementFlags f) const;        // for script access to m_movementInfo.HasMovementFlag
        void UpdateFallInformationIfNeed(MovementInfo const& minfo, uint16 opcode);
        void SetFallInformation(float fallStartZ)
        {
            m_fallStartZ = fallStartZ;
        }
        // cmangos passes (time, fallStartZ).
        void SetFallInformation(uint32 /*time*/, float fallStartZ) { m_fallStartZ = fallStartZ; }
        void HandleFall(MovementInfo const& movementInfo);
        bool IsFalling() const { return m_fallStartZ != 0; }

        bool IsControlledByOwnClient() const { return m_session->GetClientMoverGuid() == GetObjectGuid(); }
        void SetClientControl(Unit* target, uint8 allowMove);
        void SetMover(Unit* target) { m_mover = target ? target : this; }
        Unit* GetMover() const { return m_mover; } // can never be null
        Unit* GetConfirmedMover() const; // only returns mover confirmed by client, can be null
        bool IsSelfMover() const { return m_mover == this; } // normal case for player not controlling other unit
        bool IsNextRelocationIgnored() const { return m_bNextRelocationsIgnored ? true : false; }
        void SetNextRelocationsIgnoredCount(uint32 count) { m_bNextRelocationsIgnored = count; }
        void DoIgnoreRelocation() { if (m_bNextRelocationsIgnored) --m_bNextRelocationsIgnored; }

        ObjectGuid const& GetFarSightGuid() const { return GetGuidValue(PLAYER_FARSIGHT); }

        void SaveRecallPosition();
        // AzerothCore-facing reads of the recall slot; the fields stay private.
        uint32 GetRecallMap() const { return m_recallMap; }
        float GetRecallX() const { return m_recallX; }
        float GetRecallY() const { return m_recallY; }
        float GetRecallZ() const { return m_recallZ; }
        float GetRecallO() const { return m_recallO; }
        void GetRecallPosition(uint32& map, float& x, float& y, float& z, float& o)
        {
            map = m_recallMap;
            x = m_recallX;
            y = m_recallY;
            z = m_recallZ;
            o = m_recallO;
        };

        void SetHomebindToLocation(WorldLocation const& loc, uint32 area_id);
        void RelocateToHomebind() { SetLocationMapId(m_homebindMapId); Relocate(m_homebindX, m_homebindY, m_homebindZ); }
        // Read-only homebind access for modules (mod-dungeon-clear evicts its
        // test party to the bind point - the same fields the hearthstone uses).
        uint32 GetHomebindMapId() const { return m_homebindMapId; }
        float GetHomebindX() const { return m_homebindX; }
        float GetHomebindY() const { return m_homebindY; }
        float GetHomebindZ() const { return m_homebindZ; }
        bool TeleportToHomebind(uint32 options = 0, bool hearthCooldown = true);

        // currently visible objects at player client
        ObjectGuidSet m_visibleGUIDs;
        mutable std::shared_mutex m_visibleGUIDs_lock;
        std::map<ObjectGuid, bool> m_visibleGobjQuestActivated;
        mutable std::mutex m_visibleGobjsQuestAct_lock;

        bool IsInVisibleList(WorldObject const* u) const;
        void ClearVisibleObjects();
        bool IsInVisibleList_Unsafe(WorldObject const* u) const { return this == u || m_visibleGUIDs.find(u->GetObjectGuid()) != m_visibleGUIDs.end(); }
        bool IsVisibleInGridForPlayer(Player const* pl) const override;
        bool IsVisibleGloballyFor(Player* pl) const;
        void UpdateVisibilityOf(WorldObject const* viewPoint, WorldObject* target);
        template<class T>
        void UpdateVisibilityOf(WorldObject const* viewPoint, T* target, UpdateData& data, std::set<WorldObject*>& visibleNow);
        void ActivateBroadcastListeners(std::set<WorldObject*> const& visibleNow);

        Camera& GetCamera() { return m_camera; }
        // AzerothCore spellings over this core's Camera. apply=true binds the
        // view to the object, false releases it; GetViewpoint answers what the
        // camera looks through when that is not the player himself.
        void SetViewpoint(WorldObject* target, bool apply)
        {
            if (apply)
                m_camera.SetView(target);
            else
                m_camera.ResetView();
        }
        // AzerothCore rebuilds what this player can see after the viewpoint
        // moved. The camera does that here when its view changes; a manual nudge
        // is a no-op because SetView/ResetView already schedule it.
        void UpdateVisibilityForPlayer() {}
        WorldObject* GetViewpoint()
        {
            WorldObject* body = m_camera.GetBody();
            return body == (WorldObject*)this ? nullptr : body;
        }
        void ScheduleCameraUpdate(ObjectGuid guid);

        uint32 GetLongSight() const { return m_longSightSpell; }
        void SetLongSight(const Aura* aura = nullptr);
        void UpdateLongSight();

        bool CanWalk() const override { return true; }
        bool CanSwim() const override { return true; }
        bool CanFly() const override { return IsFlying(); }

        void SetFly(bool enable) override;

        // Anti undermap
        void SaveNoUndermapPosition(float x, float y, float z, float o)
        {
            m_lastSafePosition.x = x;
            m_lastSafePosition.y = y;
            m_lastSafePosition.z = z + 2.0f;
            m_lastSafePosition.o = 0;
            m_undermapPosValid = true;
        }
        bool UndermapRecall()
        {
            if (!m_undermapPosValid || IsBeingTeleported())
                return false;
            if (GetDistance2d(m_lastSafePosition) > 100.0f)
            {
                m_undermapPosValid = false;
                return false;
            }
            NearTeleportTo(m_lastSafePosition, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
            m_undermapPosValid = false;
            return true;
        }
        Position m_lastSafePosition;
        bool  m_undermapPosValid;

        uint32 GetHomeBindMap() const { return m_homebindMapId; }
        uint16 GetHomeBindAreaId() const { return m_homebindAreaId; }
        // cmangos GetHomebindLocation populates out-params.
        void GetHomebindLocation(float& x, float& y, float& z, uint32& mapId) const {
            x = m_homebindX; y = m_homebindY; z = m_homebindZ; mapId = m_homebindMapId;
        }
        WorldLocation GetHomeBindLocation() const { return WorldLocation(m_homebindMapId, m_homebindX, m_homebindY, m_homebindZ); }

        void SendSummonRequest(ObjectGuid summonerGuid, uint32 mapId, uint32 zoneId, float x, float y, float z);
        void SetSummonPoint(uint32 mapid, float x, float y, float z)
        {
            m_summon_expire = time(nullptr) + MAX_PLAYER_SUMMON_DELAY;
            m_summon_mapid = mapid;
            m_summon_x = x;
            m_summon_y = y;
            m_summon_z = z;
        }
        void SummonIfPossible(bool agree);
        void SetTransport(Transport* t) override;
        void DismountCheck();

        // knockback/jumping states
        bool IsLaunched() const { return launched; }
        void SetLaunched(bool apply) { launched = apply; }
        float GetXYSpeed() const { return xy_speed; }
        void SetXYSpeed(float speed) { xy_speed = speed; }

        void SendInitialPacketsBeforeAddToMap();
        void SendInitialPacketsAfterAddToMap(bool login = true);

        /*********************************************************/
        /***              ENVIRONMENTAL SYSTEM                 ***/
        /*********************************************************/

    protected: 
        uint8 m_environmentFlags = ENVIRONMENT_FLAG_NONE;
        float m_environmentBreathingMultiplier = 1.0f;
        MirrorTimer m_mirrorTimers[MirrorTimer::NUM_TIMERS] = { MirrorTimer::FATIGUE, MirrorTimer::BREATH, MirrorTimer::FEIGNDEATH, MirrorTimer::ENVIRONMENTAL };

        void SetEnvironmentFlags(EnvironmentFlags flags, bool apply);

        void SendMirrorTimerStart(uint32 type, uint32 remaining, uint32 duration, int32 scale, bool paused = false, uint32 spellId = 0);
        void SendMirrorTimerStop(uint32 type);
        void SendMirrorTimerPause(uint32 type, bool state);

        void FreezeMirrorTimers(bool state);
        void UpdateMirrorTimers(uint32 diff, bool send = true);

        bool CheckMirrorTimerActivation(MirrorTimer::Type timer) const;
        bool CheckMirrorTimerDeactivation(MirrorTimer::Type timer) const;

        void OnMirrorTimerExpirationPulse(MirrorTimer::Type timer);

        uint32 GetMirrorTimerMaxDuration(MirrorTimer::Type timer) const;
        SpellAuraHolder const* GetMirrorTimerBuff(MirrorTimer::Type timer) const;
    public:
        bool IsMirrorTimerActive(MirrorTimer::Type timer) const;
        bool IsUnderwater() const override { return (m_environmentFlags & ENVIRONMENT_FLAG_UNDERWATER); }
        bool IsInWater() const override { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_WATER); }
        inline bool IsInMagma() const { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_MAGMA); }
        inline bool IsInSlime() const { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_SLIME); }
        inline bool IsInHighSea() const { return (m_environmentFlags & ENVIRONMENT_FLAG_HIGH_SEA); }
        inline bool IsInHighLiquid() const { return (m_environmentFlags & ENVIRONMENT_FLAG_HIGH_LIQUID); }

        inline uint32 GetWaterBreathingInterval() const;
        void SetWaterBreathingIntervalMultiplier(float multiplier);

        void SendMirrorTimers(bool forced = false);

        uint32 EnvironmentalDamage(EnvironmentalDamageType type, uint32 damage);

        /*********************************************************/
        /***                    REST SYSTEM                    ***/
        /*********************************************************/

    private:
        uint32 m_restTime;
        time_t time_inn_enter;
        uint32 inn_trigger_id;
        float m_rest_bonus;
        RestType rest_type;
        void UpdateInnerTime(time_t time) { time_inn_enter = time; }
    public:
        /**
        * \brief: compute rest bonus
        * \param: time_t timePassed > time from last check
        * \param: bool offline      > is the player was offline?
        * \param: bool inRestPlace  > if it was offline, is the player was in city/tavern/inn?
        * \returns: float
        **/
        static constexpr float RESTED_XP_TAVERN_CAP = 1.0f;
        static constexpr float RESTED_XP_TENT_CAP = 0.25f;
        static constexpr float RESTED_XP_CLIENT_RATIO = 0.5f;
        static constexpr float RESTED_XP_TENT_RATE = 0.000575f;
        // Back to the classic hundred percent. Turtle halved this, which turns the
        // rested pool into a mild discount instead of the double experience it is
        // meant to be, and quietly punishes anyone who cannot play daily.
        static constexpr uint32 RESTED_XP_KILL_BONUS_PCT = 100;

        float ComputeRest(time_t timePassed, bool offline = false, bool inRestPlace = false);
        float GetRestBonusCap(float visibleRestedLevelFraction) const;
        uint32 GetRestedKillBonusForXP(uint32 xp) const;
        void AddRestBonus(float rest_bonus, float rest_bonus_cap);
        float GetRestBonus() const { return m_rest_bonus; }
        void SetRestBonus(float rest_bonus_new);
        RestType GetRestType() const { return rest_type; }
        void SetRestType(RestType n_r_type, uint32 areaTriggerId = 0);
        time_t GetTimeInnEnter() const { return time_inn_enter; }
        bool IsRested() const { return GetRestTime() >= 10 * IN_MILLISECONDS; }
        uint32 GetXPRestBonus(uint32 xp);
        uint32 GetRestTime() const { return m_restTime; }
        void SetRestTime(uint32 v) { m_restTime = v; }

        /*********************************************************/
        /***                    TAXI SYSTEM                    ***/
        /*********************************************************/
        
    public:
        PlayerTaxi m_taxi;
        PlayerTaxi& GetTaxi() { return m_taxi; }
        PlayerTaxi const& GetTaxi() const { return m_taxi; }
        void InitTaxiNodes() { m_taxi.InitTaxiNodes(GetRace(), GetLevel()); }
        bool ActivateTaxiPathTo(std::vector<uint32> const& nodes, Creature* npc = nullptr, uint32 spellid = 0, bool nocheck = false);
        bool ActivateTaxiPathTo(uint32 taxi_path_id, uint32 spellid = 0, bool nocheck = false);
        void TaxiStepFinished();
        void ContinueTaxiFlight();
        void CleanupFlagsOnTaxiPathFinished();

        /*********************************************************/
        /***                 CINEMATIC SYSTEM                  ***/
        /*********************************************************/

    private:
        uint32 m_cinematic;
        void UpdateCinematic(uint32 diff);
    public:
        uint32 GetCinematic() { return m_cinematic; }
        void SetCinematic(uint32 cine) { m_cinematic = cine; }
        void SendCinematicStart(uint32 CinematicSequenceId);

        void CinematicEnd();
        void CinematicStart(uint32 id)
            ;

        void LogHCDeath();

        uint32 watching_cinematic_entry;
        Position cinematic_start;
        Position const* cinematic_current_waypoint;
        uint32 cinematic_last_check;
        uint32 cinematic_elapsed_time;

        /*********************************************************/
        /***                   COMBAT SYSTEM                   ***/
        /*********************************************************/

    private:
        AutoAttackCheckResult m_swingErrorMsg;
        int32 m_cannotBeDetectedTimer;

        // Stealth detection system
        void HandleStealthedUnitsDetection();
    public:
        AutoAttackCheckResult GetLastSwingErrorMsg() const { return m_swingErrorMsg; }
        void SetSwingErrorMsg(AutoAttackCheckResult val) { m_swingErrorMsg = val; }
        void SendAttackSwingCantAttack() const;
        void SendAttackSwingCancelAttack() const;
        void SendAttackSwingDeadTarget() const;
        void SendAttackSwingNotStanding() const;
        void SendAttackSwingNotInRange() const;
        void SendAttackSwingBadFacingAttack() const;
        void SendAutoRepeatCancel() const;
        void SendFeignDeathResisted() const;
        void SendExplorationExperience(uint32 Area, uint32 Experience) const;
        void SendFactionAtWar(uint32 reputationId, bool apply) const;
        AutoAttackCheckResult CanAutoAttackTarget(Unit const*) const override;

        // Cannot be detected by creature (Should be tested in AI::MoveInLineOfSight)
        void SetCannotBeDetectedTimer(uint32 milliseconds) { m_cannotBeDetectedTimer = milliseconds; };
        bool CanBeDetected() const override { return m_cannotBeDetectedTimer <= 0; }


        Unit* GetFirstAttacker() const;
        // Nostalrius
        // Gestion des PlayerAI
        PlayerAI* i_AI;
        PlayerAI* AI() { return i_AI; }
        void setAI(PlayerAI* otherAI) { i_AI = otherAI; }
        void SetControlledBy(Unit* Who);
        bool ChangeRace(uint8 newRace, uint8 newGender, uint32 playerbyte1, uint32 playerbyte2);
        void RemoveAI();

        // =========================================================================
        // Module storage and cmangos compat aliases.
        //
        // The bot-lifecycle group that used to head this block is gone: creating,
        // destroying and reaching a PlayerbotAI is the module's business now, done
        // through the slots below. What is left:
        //   1. Module storage — an opaque pointer per claimed slot, see
        //      ModuleSlots.h. The core allocates it and never reads it.
        //   2. cmangos camelCase / signature aliases — one-line forwarders to
        //      the Penqle-named equivalent so the vendored bot source compiles
        //      unmodified. Each is tagged with the cmangos name it shadows.
        //   3. Stub no-ops for cmangos APIs Penqle doesn't have (SetCanFly,
        //      OnTaxiFlightEject, GetCurrentCell, ...). Safe because the bot
        //      module tolerates no-op behavior at these sites.
        // =========================================================================

        // Module storage. See ModuleSlots.h for who owns which slot.
        void* GetModuleSlot(uint8 slot) const { return slot < MODULE_SLOT_MAX ? m_moduleSlots[slot] : nullptr; }
        void SetModuleSlot(uint8 slot, void* value) { if (slot < MODULE_SLOT_MAX) m_moduleSlots[slot] = value; }
        template<class T> T* GetModuleSlotAs(uint8 slot) const { return static_cast<T*>(GetModuleSlot(slot)); }
        // isRealPlayer: a bot's AI is non-null and not flagged as real-player; otherwise this is a real player.

        // cmangos-style aliases the bot module uses on Player:
        // IsInGroup(other) — checks if `other` is in the same group as this player.
        // IsInGroup(other, sub) — same; sub flag is cmangos's "same subgroup" check;
        //   stub to ignore
        bool IsInGroup(Player const* other, bool /*sub*/ = false) const { return other && GetGroup() && GetGroup() == other->GetGroup(); }
        bool IsInGroup(Unit const* other, bool /*sub*/ = false) const {
            Player const* p = (other && other->GetTypeId() == TYPEID_PLAYER) ? (Player const*)other : nullptr;
            return IsInGroup(p);
        }
        // CanInteract: cmangos has CanInteract(GameObject*); Penqle has CanInteractWithGameObject.
        bool CanInteract(GameObject const* go) const { return CanInteractWithGameObject(go); }
        bool CanInteract(Creature const* c, uint32 mask = ~0u) const { return CanInteractWithNPC(c, mask); }
        // AddCooldown: cmangos forwards spell+item cooldown; Penqle uses AddSpellAndCategoryCooldowns.
        void AddCooldown(SpellEntry const& spellInfo, ItemPrototype const* proto, bool /*permanent*/ = false) {
            AddSpellAndCategoryCooldowns(&spellInfo, proto ? proto->ItemId : 0);
        }
        // IsSpellReady: cmangos checks spell cooldown; Penqle uses HasSpellCooldown (inverted).
        bool IsSpellReady(SpellEntry const& spellInfo) const { return !HasSpellCooldown(spellInfo.Id); }
        bool IsSpellReady(SpellEntry const* spellInfo) const { return spellInfo && !HasSpellCooldown(spellInfo->Id); }
        float GetHealthBonusFromStamina() const { return GetHealthBonusFromStamina(GetStat(STAT_STAMINA)); }
        float GetManaBonusFromIntellect() const { return GetManaBonusFromIntellect(GetStat(STAT_INTELLECT)); }
        bool IsSpellReady(uint32 spellId) const { return !HasSpellCooldown(spellId); }
        // 2-arg form: cmangos passes spell + item proto for item-based ability cooldowns.
        bool IsSpellReady(SpellEntry const& spellInfo, ItemPrototype const* /*proto*/) const { return !HasSpellCooldown(spellInfo.Id); }
        bool IsSpellReady(uint32 spellId, ItemPrototype const* /*proto*/) const { return !HasSpellCooldown(spellId); }
        // HasMana: cmangos shorthand for power-type check; Penqle uses GetPowerType() == POWER_MANA.
        bool HasMana() const { return GetPowerType() == POWER_MANA; }
        // CanReachWithMeleeAttack: cmangos name; Penqle uses CanReachWithMeleeAutoAttack.
        bool CanReachWithMeleeAttack(Unit const* pVictim) const { return CanReachWithMeleeAutoAttack(pVictim); }

        // cmangos camelCase / alternate-name aliases used by vendored bot module:
        void learnSpell(uint32 spell_id, bool dependent, bool talent = false) { LearnSpell(spell_id, dependent, talent); }
        // GetPlayerMenu: cmangos returns PlayerMenu*; Penqle uses PlayerTalkClass.
        PlayerMenu* GetPlayerMenu() const { return PlayerTalkClass; }
        // Whisper: cmangos signature (text, lang, target). Out-of-line in Player.cpp.
        void Whisper(const std::string& text, uint32 language, ObjectGuid receiver);
        void Whisper(const char* text, uint32 language, ObjectGuid receiver) { Whisper(std::string(text ? text : ""), language, receiver); }
        // GetItemByEntry: cmangos finds first Item* in inventory/equipment with matching entry. Out-of-line in Player.cpp.
        Item* GetItemByEntry(uint32 itemEntry) const;
        // IsFreeFlying: cmangos shorthand. Penqle has IsFlying(); we forward.
        bool IsFreeFlying() const { return IsFlying(); }
        // Mail iterators: cmangos exposes them on Player. Penqle hosts mail on MasterPlayer; forward.
        // Return type spelled as the underlying iterator to avoid including MasterPlayer.h here.
        std::deque<Mail*>::iterator GetMailBegin();
        std::deque<Mail*>::iterator GetMailEnd();
        // GetMItem: cmangos returns mail-attachment Item* by id. Penqle hosts on MasterPlayer.
        Item* GetMItem(uint32 id);
        // RemoveMail: cmangos forwarder; Penqle hosts on MasterPlayer.
        void RemoveMail(uint32 id);

        // cmangos lower/uppercase aliases.
        bool isTaxiCheater() const { return IsTaxiCheater(); }
        bool isDND() const { return IsDND(); }
        // IsStandState: cmangos shorthand for "is the unit standing (not sitting/lying)".
        bool IsStandState() const { return GetStandState() == UNIT_STAND_STATE_STAND; }
        // GetHonorHighestRankInfo: cmangos forwarder.
        HonorRankInfo GetHonorHighestRankInfo() const { return m_honorMgr.GetHighestRank(); }
        // IsTaxiDebug: cmangos shorthand for "currently on a flight path".
        bool IsTaxiDebug() const { return IsTaxiFlying(); }
        // GetDbGuid: cmangos returns DB-side player guid (low part).
        uint32 GetDbGuid() const { return GetGUIDLow(); }
        // SetCanFly: cmangos sets the unit's flying capability flag; Penqle uses different mechanism.
        // Stub no-op;
        void SetCanFly(bool /*can*/) {}
        // getStandState: cmangos camelCase alias.
        uint8 getStandState() const { return GetStandState(); }
        // GetTaxiPathSpline: cmangos exposes spline of in-flight taxi route. Stub returns nullptr.
        void* GetTaxiPathSpline() const { return nullptr; }
        // SendMessageToPlayer: cmangos sends a SMSG_MESSAGECHAT to target player. Stub forwards to whisper.
        void SendMessageToPlayer(Player* target, std::string const& msg) {
            if (target) Whisper(msg, LANG_UNIVERSAL, target->GetObjectGuid());
        }
        // 1-arg form: cmangos has just (msg) for self-message; stub no-op.
        void SendMessageToPlayer(std::string const& /*msg*/) {}
        // IsStunnedByLogout: cmangos has it; Penqle uses HasUnitState UNIT_STAT_STUNNED. Stub returns false.
        bool IsStunnedByLogout() const { return false; }
        // isAFK: cmangos camelCase alias.
        bool isAFK() const { return IsAFK(); }
        // GetHonorRankInfo: cmangos accessor; Penqle uses GetHonorMgr().GetRank().
        HonorRankInfo GetHonorRankInfo() const { return m_honorMgr.GetRank(); }
        // GetFaction: cmangos returns the player's faction template id.
        uint32 GetFaction() const { return GetFactionTemplateId(); }
        // IsSitState: cmangos shorthand.
        bool IsSitState() const { return GetStandState() == UNIT_STAND_STATE_SIT; }
        // learnClassLevelSpells / learnDefaultSpells: cmangos training helpers; Penqle has equivalents.
        void learnClassLevelSpells(bool /*includeHighLevelQuestRewards*/ = false) {}
        // learnDefaultSpells: the creation spells (playercreateinfo_spell). Was a
        // no-op stub; the core has the real thing, so hand over - the bot factory
        // calls this on every randomize to make sure the race/class starters
        // (Holy Light 1, Seal of Righteousness 1, ...) are present before the
        // trainer scan builds rank chains on top of them.
        void learnDefaultSpells() { LearnDefaultSpells(); }
        // isGMVisible: cmangos shorthand for "GM is visible to others".
        bool isGMVisible() const { return !(m_ExtraFlags & PLAYER_EXTRA_GM_INVISIBLE); }
        // setCinematic: cmangos sets cinematic state. Stub no-op.
        void setCinematic(uint32 /*cinematic*/) {}
        // TakeQuestSourceItem: cmangos quest helper. Stub no-op.
        void TakeQuestSourceItem(uint32 /*quest_id*/, bool /*sendUpdate*/ = true) {}
        // OnTaxiFlightEject: cmangos handler called when a bot is forced off a taxi.
        // Was a no-op stub, which meant MovementAction::UseTaxi could never end the
        // flight it calls this to end, so a bot already in the air could not start its
        // next hop. Out of line in Player.cpp - it needs the MotionMaster generator
        // types. Every caller in tree passes force = true; the argument is kept for
        // signature compatibility and has no meaning here.
        void OnTaxiFlightEject(bool force = false);
        // GetMountInfo: cmangos returns the bot's saved mount data with Name field. Stub returns nullptr.
        struct MountInfoStub { std::string Name; };
        MountInfoStub const* GetMountInfo() const { return nullptr; }
        // GetMaster: cmangos returns Player's master (party leader / bot owner). Out-of-line in Player.cpp.
        Player* GetMaster() const;
        // MeleeAttackStart/Stop: cmangos forwarders to AttackerStateUpdate. Stub no-op.
        void MeleeAttackStart(Unit* /*pVictim*/) {}
        void MeleeAttackStop(Unit* /*pVictim*/ = nullptr) {}
        // RemoveAllCooldowns: cmangos has it; Penqle has RemoveAllSpellCooldown. Forwarder.
        void RemoveAllCooldowns(bool /*sendOnly*/ = false) { RemoveAllSpellCooldown(); }
        // isMovingOrTurning: cmangos check; Penqle uses HasMovementFlag with MASK_MOVING_OR_TURN.
        bool isMovingOrTurning() const { return m_movementInfo.HasMovementFlag(MOVEFLAG_MASK_MOVING_OR_TURN); }
        // CanEnterNewInstance: cmangos check; Penqle CheckInstanceCount/etc. Stub returns true.
        bool CanEnterNewInstance(uint32 /*instanceId*/) const { return true; }
        // GetCurrentCell: cmangos returns the cell the player is in. Stub returns default Cell.
        // (Bot only uses it to check movement; safe default works for compile.)
        struct CellStub {};
        CellStub GetCurrentCell() const { return {}; }
        // IsInGuild: cmangos shorthand for HasGuild.
        bool IsInGuild() const { return GetGuildId() != 0; }
        // 1-arg form: cmangos checks specific guild.
        bool IsInGuild(uint32 guildId) const { return GetGuildId() == guildId; }
        // SendMailResult: cmangos sends a mail action result packet. Stub no-op.
        void SendMailResult(uint32 /*mailId*/, uint32 /*action*/, uint32 /*result*/, uint32 /*equipError*/ = 0, uint32 /*item_guid*/ = 0, uint32 /*item_count*/ = 0) {}
        // GetMailSize / RemoveMItem: cmangos forwarders; Penqle hosts on MasterPlayer. Out-of-line.
        uint32 GetMailSize();
        void RemoveMItem(uint32 id);
        // CanNoReagentCast: cmangos shorthand for "spell uses no reagent due to talent". Stub returns false.
        bool CanNoReagentCast(SpellEntry const* /*spellInfo*/) const { return false; }
        // HasTarget: cmangos shorthand for has selection (0/1-arg forms).
        bool HasTarget() const { return !GetSelectionGuid().IsEmpty(); }
        bool HasTarget(ObjectGuid guid) const { return GetSelectionGuid() == guid; }
        // isGMChat: cmangos camelCase alias.
        bool isGMChat() const { return IsGMChat(); }
        // TaxiFlightInterrupt: cmangos stops taxi flight. Stub no-op.
        void TaxiFlightInterrupt(bool /*saveOnHere*/ = false) {}
        // GetSkillInfo / SetSkillStep: cmangos skill helpers. Stubs (variadic forms accept any signature).
        template<typename... A> bool GetSkillInfo(A... /*args*/) const { return false; }
        template<typename... A> void SetSkillStep(A... /*args*/) {}
        // IsFacingTargetsBack: cmangos check; Penqle has no equivalent. Stub returns false.
        bool IsFacingTargetsBack(Unit const* /*target*/) const { return false; }
        // resetTalents: cmangos camelCase. Penqle has ResetTalents.
        bool resetTalents(bool no_cost = false) { return ResetTalents(no_cost); }
        // resetSpells: cmangos camelCase.
        void resetSpells() { /* Penqle has no equivalent; stub no-op */ }
        // learnQuestRewardedSpells: cmangos quest reward learner. Stub no-op.
        void learnQuestRewardedSpells() {}
        // GetAngleAt: cmangos returns angle from (x1,y1) to (x2,y2). Stub uses 4-arg form via atan2.
        float GetAngleAt(float x1, float y1, float x2, float y2) const {
            return atan2(y2 - y1, x2 - x1);
        }
        float GetAngleAt(float x, float y) const { return GetAngle(x, y); }
        // IsStunned: cmangos shorthand for HasUnitState(UNIT_STAT_STUNNED).
        bool IsStunned() const { return HasUnitState(UNIT_STAT_STUNNED); }

        void ModPossessPet(Pet* pet, bool apply, AuraRemoveMode m_removeMode = AURA_REMOVE_BY_DEFAULT);

        void SetDeathState(DeathState s) override;                   // overwrite Unit::SetDeathState

        /*********************************************************/
        /***                  SESSION SYSTEM                   ***/
        /*********************************************************/

    private:
        WorldSession* m_session;
        uint32 m_skippedUpdateTime;
        time_t m_logintime;
        time_t m_Last_tick;
        bool m_isIgnoringTitles;
        uint32 m_Played_time[MAX_PLAYED_TIME_INDEX];
        uint32 m_totalDeathCount;
    public:
        WorldSession* GetSession() const { return m_session; }
        void SetSession(WorldSession* s);

        void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const override;
        void DestroyForPlayer(Player* target) const override;
        void SendLogXPGain(uint32 GivenXP,Unit* victim,uint32 RestXP) const;

        void SendMessageToSet(WorldPacket* data, bool self) const override;
        void SendMessageToSetInRange(WorldPacket* data, float fist, bool self) const override;
        void SendMessageToSetInRange(WorldPacket* data, float dist, bool self, bool own_team_only) const;
        // bot module calls these with WorldPacket& by value.
        void SendMessageToSet(WorldPacket& data, bool self) const { SendMessageToSet(&data, self); }
        void SendMessageToSetInRange(WorldPacket& data, float dist, bool self) const { SendMessageToSetInRange(&data, dist, self); }
        void SendInitWorldStates(uint32 zone) const;
        void SendUpdateWorldState(uint32 Field, uint32 Value) const;
        void SendDirectMessage(WorldPacket* data) const;

        uint32 GetTotalPlayedTime() const { return m_Played_time[PLAYED_TIME_TOTAL]; }
        uint32 GetLevelPlayedTime() const { return m_Played_time[PLAYED_TIME_LEVEL]; }
        time_t GetLoginTime() const { return m_logintime; }

        void AddSkippedUpdateTime(uint32 t) { m_skippedUpdateTime += t; }
        uint32 GetSkippedUpdateTime() const { return m_skippedUpdateTime; }
        void ResetSkippedUpdateTime() { m_skippedUpdateTime = 0; }

        enum class HardcoreInteractionResult
        {
            Allowed = 1,
            NotHardcore,
            TargetNotHardcore,
            LevelLimitExceeded
        };

        void SetHardcoreStatus(uint8 status);
        uint8 GetHardcoreStatus() { return m_hardcoreStatus; };
        bool IsHardcore() const{ return (m_hardcoreStatus == HARDCORE_MODE_STATUS_ALIVE || m_hardcoreStatus == HARDCORE_MODE_STATUS_DEAD || m_hardcoreStatus == HARDCORE_MODE_STATUS_HC60); }
        bool isImmortal() const { return m_hardcoreStatus == HARDCORE_MODE_STATUS_IMMORTAL; }
        bool IsHC60() const { return m_hardcoreStatus == HARDCORE_MODE_STATUS_HC60; }
        HardcoreInteractionResult HandleHardcoreInteraction(Player* target, bool checkLevelDiff);
        void SpawnHardcoreGravestone();
        static std::string HardcoreResultToString(HardcoreInteractionResult result);

        bool SetupHardcoreMode();

        bool HasXPGainEnabled() const { return m_xpGain; }
        void SetXPGain(bool value) { m_xpGain = value; }

        void UpdateTotalDeathCount();
        uint32 GetTotalDeathCount() const { return m_totalDeathCount; };
        uint32  GetTotalQuestCount();

        /*********************************************************/
        /***                 MISC GAME SYSTEMS                 ***/
        /*********************************************************/

        void HandleTransferChecks();

        void HandleRaceChangeFixup();

    private:
        uint8 m_newStandState;
        uint32 m_standStateTimer;
        uint32 m_DetectInvTimer;
        uint32 m_ExtraFlags;
        ObjectGuid m_curSelectionGuid;
        ResurrectionData m_resurrectData;
        uint32 m_drunkTimer;
        uint16 m_drunk;
        void HandleSobering();
        uint32 m_deathTimer;
        time_t m_deathExpireTime;
        bool m_repopAtGraveyardPending;
        ObjectGuid m_selectedGobj; // For GM commands
        ObjectGuid m_escortingGuid;
        uint32 customFlags = 0;
        uint8 m_hardcoreStatus;
        bool m_xpGain = true;
        uint32 m_hardcoreKickTimer;
        uint32 m_timeLoggedIn = 0;
        
        void SendMountResult(UnitMountResult result) const;
        void SendDismountResult(UnitDismountResult result) const;
        void UpdateCorpseReclaimDelay();

        // For chronoboon item
        uint32 m_worldBuffCheckTimer;
    public:

        uint32 m_hardcoreSaveItemsTimer;

        bool HasCustomFlag(CustomPlayerFlags flag) const
        {
            return (customFlags & flag) == flag;
        }

        void SetCustomFlag(CustomPlayerFlags flags)
        {
            customFlags |= flags;
        }

        void RemoveCustomFlag(CustomPlayerFlags flags)
        {
            customFlags &= ~flags;
        }


        // Prevents environmental damage and aggro for hardcore chars shortly after login, disconnect, and riding transport.
        mutable time_t m_hardcoreImmunityTime = 0;
        bool HasHCImmunity() const final
        {
            if (m_hardcoreImmunityTime)
            {
                if (m_hardcoreImmunityTime >= time(nullptr))
                    return true;
                else
                    m_hardcoreImmunityTime = 0;
            }

            return false;
        }
        void SetHCImmunityTimer(uint32 secs)
        {
            m_hardcoreImmunityTime = time(nullptr) + secs;
        }

        void ScheduleStandStateChange(uint8 state);
        void ClearScheduledStandState() { m_newStandState = MAX_UNIT_STAND_STATE; m_standStateTimer = 0; }
        bool IsStandingUpForProc() const override;
        UnitMountResult Mount(uint32 mount, uint32 spellId = 0) override;
        UnitDismountResult Unmount(bool from_aura = false) override;

        void RecallPvPGear();

        bool CanInteractWithQuestGiver(Object* questGiver) const;
        Creature* FindNearestInteractableNpcWithFlag(uint32 npcFlags) const;
        Creature* GetNPCIfCanInteractWith(ObjectGuid guid, uint32 npcflagmask) const;
        // Optional read-only explanation of the same native eligibility checks.
        bool CanInteractWithNPC(Creature const* pCreature, uint32 npcflagmask, char const** failureReason = nullptr) const;
        GameObject* GetGameObjectIfCanInteractWith(ObjectGuid guid, uint32 gameobject_type = MAX_GAMEOBJECT_TYPE) const;
        bool CanInteractWithGameObject(GameObject const* pGo, uint32 gameobject_type = MAX_GAMEOBJECT_TYPE) const;
        bool CanSeeHealthOf(Unit const* pTarget) const;

        ObjectGuid const& GetSelectedGobj() const { return m_selectedGobj; }
        void SetSelectedGobj(ObjectGuid guid) { m_selectedGobj = guid; }
        ObjectGuid const& GetSelectionGuid() const { return m_curSelectionGuid; }
        // AzerothCore spelling.
        void SetSelection(ObjectGuid guid) { SetSelectionGuid(guid); }
        // AzerothCore spellings.
        bool CanSeeOrDetect(Unit const* u, bool /*detect*/ = true, bool /*inVisibleList*/ = false, bool /*is3dDistance*/ = true) const
        { return u && u->IsVisibleForOrDetect(this, this, false); }
        float GetObjectSize() const { return GetObjectBoundingRadius(); }
        // AzerothCore's long form carries casting/vehicle flags this core has
        // no seat for; the coordinates and orientation are the teleport. The
        // using-declaration keeps the inherited forms visible - declaring an
        // overload here hides them, and this class calls the Position form on
        // itself a few hundred lines up.
        using Unit::NearTeleportTo;
        bool NearTeleportTo(float x, float y, float z, float o, bool /*casting*/, bool /*vehicleTeleport*/ = false, bool /*withPet*/ = false)
        { return TeleportTo(GetMapId(), x, y, z, o, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET); }
        // Instance data IS the instance script on this core; the AzerothCore
        // face of it (GetBossState and friends) hangs off InstanceData as
        // virtuals with honest defaults, so this cast-free accessor is safe on
        // every map.
        InstanceData* GetInstanceScript() const { return GetMap() ? GetMap()->GetInstanceData() : nullptr; }
        void SetSelectionGuid(ObjectGuid guid) { m_curSelectionGuid = guid; SetTargetGuid(guid); }
        Unit* GetSelectedUnit() { return GetMap()->GetUnit(m_curSelectionGuid); }
        Creature* GetSelectedCreature() { return GetMap()->GetCreature(m_curSelectionGuid); }
        Player* GetSelectedPlayer() { return GetMap()->GetPlayer(m_curSelectionGuid); }
        Object* GetObjectByTypeMask(ObjectGuid guid, TypeMask typemask);

        void SetResurrectRequestData(ObjectGuid guid, uint16 mapId, uint32 instanceId, float x, float y, float z, float o, uint32 health, uint32 mana)
        {
            m_resurrectData.resurrectorGuid = guid;
            m_resurrectData.location.mapId = mapId;
            m_resurrectData.location.x = x;
            m_resurrectData.location.y = y;
            m_resurrectData.location.z = z;
            m_resurrectData.location.o = o;
            m_resurrectData.instanceId = instanceId;
            m_resurrectData.health = health;
            m_resurrectData.mana = mana;
        }
        void ClearResurrectRequestData() { SetResurrectRequestData(ObjectGuid(), 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0); }
        bool IsRessurectRequestedBy(ObjectGuid guid) const { return m_resurrectData.resurrectorGuid == guid; }
        // Correctly spelled alias. AzerothCore writes it with one s, and so does
        // English; the name above is kept because this tree already calls it.
        bool isResurrectRequestedBy(ObjectGuid guid) const { return IsRessurectRequestedBy(guid); }
        bool IsRessurectRequested() const { return !m_resurrectData.resurrectorGuid.IsEmpty(); }
        // bot uses cmangos camelCase isRessurectRequested.
        bool isRessurectRequested() const { return IsRessurectRequested(); }
        ObjectGuid const& GetResurrector() const { return m_resurrectData.resurrectorGuid; }
        void ResurectUsingRequestData();

        static bool IsActionButtonDataValid(uint8 button, uint32 action, uint8 type, Player* player);
        void RefreshBitsForVisibleUnits(UpdateMask* mask, uint32 objectTypeMask = TYPEMASK_UNIT);

        Corpse* GetCorpse() const;
        void SpawnCorpseBones();
        Corpse* CreateCorpse();
        void KillPlayer();
        uint32 GetResurrectionSpellId() const;
        void ResurrectPlayer(float restore_percent, bool applySickness = false, bool forceHc = false);
        void BuildPlayerRepop();
        void RepopAtGraveyard();
        void ScheduleRepopAtGraveyard();

        // Nostalrius : Phasing
        void SetWorldMask(uint32 newMask) override;

        void RemoveDelayedOperation(uint32 operation)
        {
            m_DelayedOperations &= ~operation;
        }
        inline bool HasScheduledEvent() const { return m_Events.HasScheduledEvent(); }

        void SetEscortingGuid(const ObjectGuid& guid) { m_escortingGuid = guid; }
        const ObjectGuid& GetEscortingGuid() const { return m_escortingGuid; }

        void SetDrunkValue(uint16 newDrunkValue, uint32 itemid = 0);
        uint16 GetDrunkValue() const { return m_drunk; }
        static DrunkenState GetDrunkenstateByValue(uint16 value);

        uint32 GetDeathTimer() const { return m_deathTimer; }
        uint32 GetCorpseReclaimDelay(bool pvp) const;
        void SendCorpseReclaimDelay(bool load = false) const;

        // Giperion Turtle: Z limit should be customized
        float ZFelldownLimit = -500.0f;

public:
    bool IsPvP() const override
    {
        if (HasChallenge(CHALLENGE_WAR_MODE))
            return true;

        return Unit::IsPvP();
    }
    
    bool HasChallenge(Challenges challenge) const
    {
        static std::unordered_map<uint32, uint32> challenge_spells
        {
            {CHALLENGE_SLOW_AND_STEADY, SPELL_SLOW_AND_STEADY},
            {CHALLENGE_EXHAUSTION_MODE, SPELL_EXHAUSTION_MODE},
            {CHALLENGE_WAR_MODE,        SPELL_WAR_MODE},
            {CHALLENGE_HARDCORE,        SPELL_HARDCORE},
            {CHALLENGE_VAGRANT_MODE,    SPELL_VARGANT_MODE},
            {CHALLENGE_BOARING_MODE,    SPELL_BOARING_MODE},
            {CHALLENGE_CRAFTMASTER,     SPELL_CRAFTMASTER},
            {CHALLENGE_LUNATIC,         SPELL_LUNATIC},
            {CHALLENGE_BREWMASTER,      SPELL_BREWMASTER},
            {CHALLENGE_HEROIC,          SPELL_HEROIC},
        };

        auto itr = challenge_spells.find(challenge);

        if (itr == challenge_spells.end())
            return false;

        return HasSpell(itr->second);
    };

    ObjectGuid GetChampionGUID() const
    {
        return m_currentChampionGuid;
    }

    void SetChampion(ObjectGuid champion)
    {
        m_currentChampionGuid = champion;
    }

    private:
        ObjectGuid m_currentChampionGuid;

// Chat system:

    private:
        typedef std::list<::Channel*> JoinedChannelsList;
        JoinedChannelsList m_channels;
        void UpdateLocalChannels(uint32 newZone);
        std::string m_name;
    public:
        void JoinedChannel(::Channel* c);
        void LeftChannel(::Channel* c);
        void CleanupChannels();
        void LeaveLFGChannel();

        bool IsAllowedWhisperFrom(ObjectGuid guid) const;
        bool IsEnabledWhisperRestriction() const { return m_ExtraFlags & PLAYER_EXTRA_WHISP_RESTRICTION; }
        void SetWhisperRestriction(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_WHISP_RESTRICTION; else m_ExtraFlags &= ~PLAYER_EXTRA_WHISP_RESTRICTION; }

        bool IsAcceptWhispers() const { return m_ExtraFlags & PLAYER_EXTRA_ACCEPT_WHISPERS; }
        void SetAcceptWhispers(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_ACCEPT_WHISPERS; else m_ExtraFlags &= ~PLAYER_EXTRA_ACCEPT_WHISPERS; }
        uint32 GetExtraFlags() const { return m_ExtraFlags; }

        bool ToggleAFK();
        bool ToggleDND();
        bool IsAFK() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK); }
        bool IsDND() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DND); }

        // Inactivity start

        //************************************
        // Desc:    Check if player is online, but doesn't actually do anything in 5 min (4 in BG)
        //          That includes movement, spell and (if requested) chat activity
        //          Check for movement is anti-afk proof
        // Returns:   bool
        // Parameter: bool bIncludeChat: if true, we check the chat activity as well
        //************************************
        bool IsBasicallyInactive(bool bIncludeChat = false) const;

        uint32 m_lastMovementTimer = 0;
        uint32 m_lastSpellTimer = 0;
        uint32 m_lastChatMessageTimer = 0;

        //************************************
        // Desc:    Last time when player moved in milliseconds. Call WorldTimer::getMSTime() - GetLastMovementTime() to get delta
        // Returns:   uint32
        //************************************
        uint32 GetLastMovementTime() const
        {
            return m_lastMovementTimer;
        }

        //************************************
        // Desc:    Last time when player casted spell in milliseconds. Call WorldTimer::getMSTime() - GetLastSpellCastedTime() to get delta
        // Returns:   uint32
        //************************************
        uint32 GetLastSpellCastedTime() const
        {
            return m_lastSpellTimer;
        }

        //************************************
        // Desc:    Last time when player send a chat message in milliseconds. Call WorldTimer::getMSTime() - GetLastChatMessageTime() to get delta
        // Returns:   uint32
        //************************************
        uint32 GetLastChatMessageTime() const
        {
            return m_lastChatMessageTimer;
        }

        // Internal functions that assign timer to current time
        void UpdateMovementActivityTimer();
        void UpdateSpellActivityTimer();
        void UpdateChatActivityTimer();
        void UpdateVelocity();
        void UpdateSavedVelocityPositionToCurrentPos();

        //velocity per second
        float m_velocity = 0.0f;

        //velocity per 3 min
        float m_velocityPer3Min = 0.0f;

        //************************************
        // Desc:    Internal function which calculates 2D velocity. Doesn't change player
        // Returns:   Calculated delta
        // Parameter: uint32 deltaTime : in milliseconds
        // Parameter: const WorldLocation & PrevPos : previous position
        //************************************
        float GetVelocity(uint32 deltaTime, const WorldLocation& PrevPos) const;


        //************************************
        // Desc:    A very approximate 2D velocity, which doesn't account a lot of things
        // Returns:   float
        //************************************
        float GetVelocityPerSecond() const
        {
            return m_velocity;
        }

        //************************************
        // Desc:    A very approximate 2D velocity, which doesn't account a lot of things. Calculated once per 3 min.
        // NOTE:    We just compare previous position, so if player moved a 1000 units and then go back in less then 3 min, then his velocity will be 0
        // Returns:   float
        //************************************
        float GetVelocityPer3Min() const
        {
            return m_velocityPer3Min;
        }

        uint32 m_lastUpdatedVelocityPerSecondTimer = 0;
        uint32 m_lastUpdatedVelocityPer3MinTimer = 0;

        WorldLocation m_lastSecondPosition;
        WorldLocation m_last3MinPosition;

        // if we already reported a player moving too fast - don't do that again
        bool bIsReportedAsSpeedhacker = false;

        // Inactivity end

        uint8 GetChatTag() const;

        char const* GetName() const final { return m_name.c_str(); }
        void SetName(std::string const& newname) { m_name = newname; }

        float GetYellRange() const;
        void Say(std::string const& text, const uint32 language) const;
        void Yell(std::string const& text, const uint32 language) const;
        void TextEmote(std::string const& text) const;

// Faction system:

    private:
        Team m_team;
        ReputationMgr  m_reputationMgr;
        std::set<uint32> m_temporaryAtWarFactions;
        bool m_DbSaveDisabled; // used for faction change
    public:
        static Team TeamForRace(uint8 race);
        Team GetTeam() const { return m_team; }
        TeamId GetTeamId() const { return m_team == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE; }
        static uint32 GetFactionForRace(uint8 race);
        void SetFactionForRace(uint8 race);

        ReputationMgr&       GetReputationMgr() { return m_reputationMgr; }
        ReputationMgr const& GetReputationMgr() const { return m_reputationMgr; }
        ReputationRank GetReputationRank(uint32 faction_id) const;
        void RewardReputation(Unit* pVictim, float rate);
        void RewardReputation(Quest const* pQuest);
        int32 CalculateReputationGain(ReputationSource source, int32 rep, int32 faction, uint32 creatureOrQuestLevel = 0, bool noAuraBonus = false);
        void SetTemporaryAtWarWithFaction(uint32 factionId) { m_temporaryAtWarFactions.insert(factionId); }
        void ClearTemporaryWarWithFactions();

        bool ChangeRace(uint8 newRace);
        bool ChangeItemsForRace(uint8 oldRace, uint8 newRace);
        bool ChangeReputationsForRace(uint8 oldRace, uint8 newRace);
        bool ChangeQuestsForRace(uint8 oldRace, uint8 newRace);
        bool ConvertSpell(uint32 oldSpellId, uint32 newSpellId);
        bool ChangeSpellsForRace(uint8 oldRace, uint8 newRace);

// PvP System

    private:
        HonorMgr  m_honorMgr;
        void UpdatePvPFlagTimer(uint32 diff);
        void UpdatePvPContestedFlagTimer(uint32 diff);
    public:
        PvPInfo pvpInfo;
        void UpdatePvP(bool state, bool overriding = false);
        void UpdatePvPContested(bool state, bool overriding = false);

        bool IsPvPDesired() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_DESIRED); }
        bool IsFFAPvP() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_FFA_PVP); }
        void SetFFAPvP(bool state);
        bool IsInInterFactionMode() const;

        // todo: -maybe move UpdateDuelFlag+DuelComplete to independent DuelHandler.
        DuelInfo* m_duel;
        bool IsInDuelWith(Player const* player) const { return m_duel && m_duel->opponent == player->GetObjectGuid() && m_duel->startTime != 0; }
        void UpdateDuelFlag(time_t currTime);
        void CheckDuelDistance(time_t currTime);
        void DuelComplete(DuelCompleteType type);
        void SendDuelCountdown(uint32 counter) const;

        void RewardHonor(Unit* uVictim, uint32 groupSize);
        void RewardHonorOnDeath();
        bool IsHonorOrXPTarget(Unit* pVictim) const;
        // cmangos camelCase alias.
        bool isHonorOrXPTarget(Unit* pVictim) const { return IsHonorOrXPTarget(pVictim); }

        HonorMgr&       GetHonorMgr() { return m_honorMgr; }
        HonorMgr const& GetHonorMgr() const { return m_honorMgr; }

// Outdoor PvP system:

    public:
        ZoneScript* GetZoneScript() const;
        // returns true if the player is in active state for outdoor pvp objective capturing, false otherwise
        bool IsOutdoorPvPActive() const;

// Battleground system:

    protected:
        /*
        this is an array of BG queues (BgTypeIDs) in which is player
        */
        struct BgBattleGroundQueueID_Rec
        {
            BattleGroundQueueTypeId bgQueueTypeId;
            uint32 invitedToInstance;
        };

        BgBattleGroundQueueID_Rec m_bgBattleGroundQueueID[PLAYER_MAX_BATTLEGROUND_QUEUES];
        BGData                    m_bgData;
    public:
        void AddToArenaQueue(bool queuedAsGroup);
        bool InArena()              const                { return InBattleGround() && GetBattleGround()->IsArena(); }
        bool InBattleGround()       const                { return m_bgData.bgInstanceID != 0; }
        uint32 GetBattleGroundId()  const                { return m_bgData.bgInstanceID; }
        BattleGroundTypeId GetBattleGroundTypeId() const { return m_bgData.bgTypeID; }
        uint32 GetCurrentBattlegroundQueueSlot() const { return m_bgData.bgQueueSlot; }
        BattleGround* GetBattleGround() const;

        static uint32 GetMinLevelForBattleGroundBracketId(BattleGroundBracketId bracket_id, BattleGroundTypeId bgTypeId);
        static uint32 GetMaxLevelForBattleGroundBracketId(BattleGroundBracketId bracket_id, BattleGroundTypeId bgTypeId);
        static BattleGroundBracketId GetBattleGroundBracketIdFromLevel(BattleGroundTypeId bgTypeId, uint32 level);
        BattleGroundBracketId GetBattleGroundBracketIdFromLevel(BattleGroundTypeId bgTypeId) const;

        bool InBattleGroundQueue() const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId != BATTLEGROUND_QUEUE_NONE)
                    return true;

            return false;
        }

        BattleGroundQueueTypeId GetQueuedBattleground() const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId != BATTLEGROUND_QUEUE_NONE)
                    return i.bgQueueTypeId;

            return BATTLEGROUND_QUEUE_NONE;
        }

        BattleGroundQueueTypeId GetBattleGroundQueueTypeId(uint32 index) const { return m_bgBattleGroundQueueID[index].bgQueueTypeId; }
        uint32 GetBattleGroundQueueIndex(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            for (int i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattleGroundQueueID[i].bgQueueTypeId == bgQueueTypeId)
                    return i;
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool IsInvitedForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId == bgQueueTypeId)
                    return i.invitedToInstance != 0;
            return false;
        }
        bool InBattleGroundQueueForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            return GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES;
        }

        void SetBattleGroundId(uint32 val, BattleGroundTypeId bgTypeId, uint32 queueSlot)
        {
            m_bgData.bgInstanceID = val;
            m_bgData.bgTypeID = bgTypeId;
            m_bgData.bgQueueSlot = queueSlot;
            m_bgData.m_needSave = true;
        }
        void SetSoulShardCountBeforeBgJoin(uint32 count)
        {
            m_bgData.soulShardCount = count;
        }
        uint32 GetSoulShardCountBeforeBgJoin() const
        {
            return m_bgData.soulShardCount;
        }
        uint32 AddBattleGroundQueueId(BattleGroundQueueTypeId val)
        {
            for (int i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattleGroundQueueID[i].bgQueueTypeId == BATTLEGROUND_QUEUE_NONE || m_bgBattleGroundQueueID[i].bgQueueTypeId == val)
                {
                    m_bgBattleGroundQueueID[i].bgQueueTypeId = val;
                    m_bgBattleGroundQueueID[i].invitedToInstance = 0;
                    return i;
                }
            }
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool HasFreeBattleGroundQueueId() const;
        void RemoveBattleGroundQueueId(BattleGroundQueueTypeId val)
        {
            for (auto& i : m_bgBattleGroundQueueID)
            {
                if (i.bgQueueTypeId == val)
                {
                    i.bgQueueTypeId = BATTLEGROUND_QUEUE_NONE;
                    i.invitedToInstance = 0;
                    return;
                }
            }
        }
        void SetInviteForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId, uint32 instanceId)
        {
            for (auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId == bgQueueTypeId)
                    i.invitedToInstance = instanceId;
        }
        bool IsInvitedForBattleGroundInstance(uint32 instanceId) const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.invitedToInstance == instanceId)
                    return true;
            return false;
        }
        WorldLocation const& GetBattleGroundEntryPoint() const { return m_bgData.joinPos; }
        void SetBattleGroundEntryPoint(uint32 mapId, float x, float y, float z, float orientation);
        void SetBattleGroundEntryPoint(Player* leader = nullptr, bool queuedAtBGPortal = false);

        void SetBGTeam(Team team) { m_bgData.bgTeam = team; m_bgData.m_needSave = true; }
        Team GetBGTeam() const { return m_bgData.bgTeam ? m_bgData.bgTeam : GetTeam(); }

        void LeaveBattleground(bool teleportToEntryPoint = true);
        bool CanJoinToBattleground() const;

        bool GetBGAccessByLevel(BattleGroundTypeId bgTypeId) const;
        bool CanUseBattleGroundObject() const;

// Anticheat system:

    public:
        bool CanSpeak() const;
        bool FallGround(uint8 fallMode);
        void OnDisconnected();
        void RelocateToLastClientPosition();
        void GetSafePosition(float &x, float &y, float &z, Transport* onTransport = nullptr) const override;

// Packet broadcaster:

    public:
        std::shared_ptr<PlayerBroadcaster> m_broadcaster;
        void DeletePacketBroadcaster();
        void CreatePacketBroadcaster();
        std::shared_ptr<PlayerBroadcaster> GetPacketBroadcaster() { return m_broadcaster; }

// Instance system:

    private:
        uint32 m_HomebindTimer;
    public:
        void SendTransferAborted(uint8 reason) const;
        void SendInstanceResetWarning(uint32 mapid, uint32 time) const;

        void ResetInstances(InstanceResetMethod method);
        void SendResetInstanceSuccess(uint32 MapId) const;
        void SendResetInstanceFailed(uint32 reason, uint32 MapId) const;
        void SendResetFailedNotify();
        bool CheckInstanceCount(uint32 instanceId) const;
        void AddInstanceEnterTime(uint32 instanceId, time_t enterTime) const;

        typedef std::unordered_map< uint32 /*mapId*/, InstancePlayerBind > BoundInstancesMap;

        void UpdateHomebindTime(uint32 time);
        bool m_InstanceValid;
        // permanent binds and solo binds
        BoundInstancesMap m_boundInstances;
        InstancePlayerBind* GetBoundInstance(uint32 mapid);
        BoundInstancesMap& GetBoundInstances() { return m_boundInstances; }
        void UnbindInstance(uint32 mapid, bool unload = false);
        void UnbindInstance(BoundInstancesMap::iterator &itr, bool unload = false);
        InstancePlayerBind* BindToInstance(DungeonPersistentState* save, bool permanent, bool load = false);
        void SendRaidInfo() const;
        void SendSavedInstances() const;
        static void ConvertInstancesToGroup(Player* player, Group* group = nullptr, ObjectGuid player_guid = ObjectGuid());
        DungeonPersistentState* GetBoundInstanceSaveForSelfOrGroup(uint32 mapid);

// Group system:

    private:
        GroupReference m_group;
        GroupReference m_originalGroup;
        Group* m_groupInvite;
        uint32 m_groupUpdateMask;
        uint64 m_auraUpdateMask;
    public:
        Group* GetGroupInvite() { return m_groupInvite; }
        void SetGroupInvite(Group* group) { m_groupInvite = group; }
        Group* GetGroup() { return m_group.getTarget(); }
        const Group* GetGroup() const { return (const Group*)m_group.getTarget(); }
        GroupReference& GetGroupRef() { return m_group; }
        void SetGroup(Group* group, int8 subgroup = -1);
        uint8 GetSubGroup() const { return m_group.getSubGroup(); }
        uint32 GetGroupUpdateFlag() const { return m_groupUpdateMask; }
        void SetGroupUpdateFlag(uint32 flag) { m_groupUpdateMask |= flag; }
        const uint64& GetAuraUpdateMask() const { return m_auraUpdateMask; }
        void SetAuraUpdateSlot(uint8 slot) { m_auraUpdateMask |= (uint64(1) << slot); }
        void SetAuraUpdateMask(uint64 mask) { m_auraUpdateMask = mask; }
        Player* GetNextRandomRaidMember(float radius);
        PartyResult CanUninviteFromGroup(ObjectGuid uninvitedGuid) const;
        void UpdateGroupLeaderFlag(const bool remove = false);
        bool IsGroupVisibleFor(Player const* p) const;
        bool IsInSameGroupWith(Player const* p) const;
        bool IsInSameRaidWith(Player const* p) const { return p == this || (GetGroup() != nullptr && GetGroup() == p->GetGroup()); }
        void UninviteFromGroup();
        static void RemoveFromGroup(Group* group, ObjectGuid guid);
        void RemoveFromGroup() { RemoveFromGroup(GetGroup(), GetObjectGuid()); }
        void SendUpdateToOutOfRangeGroupMembers();
        void SendDestroyGroupMembers(bool includingSelf = false);

        // Forces the client to destroy and re-create every other player (incl. bots) currently in
        // view, so equipment that rendered "naked" on first sight (item display data not yet
        // cached when the create block arrived) is redrawn. Mimics the hearthstone/visibility
        // re-entry that is known to fix the issue.
        void RefreshVisiblePlayersForClient();

        // BattleGround Group System
        void SetBattleGroundRaid(Group* group, int8 subgroup = -1);
        void RemoveFromBattleGroundRaid();
        Group* GetOriginalGroup() { return m_originalGroup.getTarget(); }
        GroupReference& GetOriginalGroupRef() { return m_originalGroup; }
        uint8 GetOriginalSubGroup() const { return m_originalGroup.getSubGroup(); }
        void SetOriginalGroup(Group* group, int8 subgroup = -1);

// Guild system:

    private:
        uint32 m_GuildIdInvited;
    public:
        void SetInGuild(uint32 GuildId) { SetUInt32Value(PLAYER_GUILDID, GuildId); }
        void SetRank(uint32 rankId) { SetUInt32Value(PLAYER_GUILDRANK, rankId); }
        void SetGuildIdInvited(uint32 GuildId) { m_GuildIdInvited = GuildId; }
        uint32 GetGuildId() const { return GetUInt32Value(PLAYER_GUILDID); }
        uint32 GetRank() const { return GetUInt32Value(PLAYER_GUILDRANK); }
        static uint32 GetGuildIdFromDB(ObjectGuid guid);
        static uint32 GetRankFromDB(ObjectGuid guid);
        int GetGuildIdInvited() { return m_GuildIdInvited; }

// Misc.

    protected:
    public:
        //Giperion Elysium: Send message to upper place of player screen, even if he not in raid. Useful for scripts/events
        void SendRaidWarning(uint32 textId);
        void SendRaidWarning(const std::string& text);
        void SendRaidWarning(const char* text);

        bool IsObjectIsExclusiveVisible(ObjectGuid guid);
        void AddExclusiveVisibleObject(ObjectGuid guid);
        void RemoveExclusiveVisibleObject(ObjectGuid guid);
        std::list<ObjectGuid> m_exclusiveVisibleObjects;

        // Xerron Suspend World Buffs Start
    public:
        bool SuspendWorldBuffs();
        bool RestoreSuspendedWorldBuffs();
        void RemoveWorldBuffsIfAlreadySuspended();
        // Xerron Suspend World Buffs End

        // Xerron Dual Spec
    public:
        bool HasSavedTalentSpec(const std::uint8_t uiPrimaryOrSecondary);
        std::string SpecTalentPoints(const std::uint8_t uiPrimaryOrSecondary);
        void CountTalentsSpentInSavedSpec(uint32 specIndex, std::vector<uint32>& vTreeTalents);
        bool ActivateTalentSpec(const std::uint8_t uiPrimaryOrSecondary);
        bool SaveTalentSpec(const std::uint8_t uiPrimaryOrSecondary);
        // Xerron Dual Spec End

        // Tanatos Transmog
    public:
        //uint8 ApplyTransmogrifications(uint8 slot, uint32 sourceItemID, uint32 slotId);
        //std::string GetAvailableTransmogs(uint8 InventorySlotId, uint8 invType, uint32 destItemId);
        void TransmogSetVisibleItemSlot(uint8 slot, Item* pItem) { SetVisibleItemSlot(slot, pItem); }
        void UpdateItemDurability(Item* pItem, uint32 durability);
        void RemoveTransmogsToItem(uint32 itemId);

        TransmogMgr* GetTransmogMgr() { return _transmogMgr; }
    private:
        TransmogMgr* _transmogMgr;
        // Tanatos Transmog End

        // Titles System
    public:
        std::unordered_set<uint8> m_playerTitles;
        uint8 m_activeTitle = 0;
        bool HasEarnedTitle(uint8 titleId);
        bool HasTitle(uint8 title);
        void AwardTitle(int8 title);
        uint8 GetActiveTitle();
        void SendEarnedTitles();
        void ChangeTitle(uint8 title);
        void UpdateAppearance();
        bool HasAllZonesExplored();

        // General send addon message
    public:
        void SendAddonMessage(std::string prefix, std::string message);
        void SendAddonMessage(std::string prefix, std::string message, Player* from);

    private:
        // Per-player storage for modules. The core hands out the space and never
        // looks inside it; slot ids are claimed in ModuleSlots.h. A flat array
        // rather than a keyed map because the population module reads its slot on
        // every tick of every driven character, where a hash lookup would show.
        void* m_moduleSlots[MODULE_SLOT_MAX] = {};
};

void AddItemsSetItem(Player*player,Item* item);
void RemoveItemsSetItem(Player*player,ItemPrototype const* proto);

// The four bot dispatchers that used to be declared here are gone. They are
// module hooks now: ServerScript::CanPacketSend and ::OnPacketHandled for the
// two packet paths, PlayerScript::OnChatCommand, ::SetForcedRole and
// ::GetAllowedRoles for the rest. Ask through the Script_* helpers in
// ScriptMgr.h rather than reaching for a bot type from the core.

// "the bodies of template functions must be made available in a header file"
template <class T> T Player::ApplySpellMod(uint32 spellId, SpellModOp op, T &basevalue, Spell* spell)
{
    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellId);
    if (!spellInfo || spellInfo->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS)) return 0;
    int32 totalpct = 0;
    int32 totalflat = 0;
    for (const auto mod : m_spellMods[op])
    {
        if (!IsAffectedBySpellmod(spellInfo,mod,spell))
            continue;

        if (mod->type == SPELLMOD_FLAT)
            totalflat += mod->value;
        else if (mod->type == SPELLMOD_PCT)
        {
            // skip percent mods for null basevalue (most important for spell mods with charges)
            if(basevalue == T(0))
                continue;

            // special case (skip >10sec spell casts for instant cast setting)
            if(mod->op==SPELLMOD_CASTING_TIME  && basevalue >= T(10*IN_MILLISECONDS) && mod->value <= -100)
                continue;

            totalpct += mod->value;
        }

        // World of Warcraft Client Patch 1.11.0 (2006-06-20)
        // - Nature's Grace: You will no longer consume this effect when casting a 
        //   spell which was made instant by Nature's Swiftness.
        if (!((mod->op == SPELLMOD_CASTING_TIME) && (mod->type == SPELLMOD_FLAT) && HasInstantCastingSpellMod(spellInfo)))
            DropModCharge(mod, spell);

        // Nostalrius : fix ecorce (22812 - +1sec incant) + rapidite nature (17116 - sorts instant) = 0sec de cast
        if (mod->op == SPELLMOD_CASTING_TIME && mod->type == SPELLMOD_PCT && mod->value == -100)
        {
            totalpct = -100;
            totalflat = 0;
            break;
        }
    }

    float diff = (float)basevalue*(float)totalpct/100.0f + (float)totalflat;
    basevalue = T((float)basevalue + diff);
    return T(diff);
}
