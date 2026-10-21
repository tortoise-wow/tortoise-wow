/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

#include "World.h"
#include "Common.h"
#include "Log.h"
#include "Object.h"
#include "Bag.h"
#include "Creature.h"
#include "Player.h"
#include "GameObject.h"
#include "Corpse.h"
#include "QuestDef.h"
#include "ItemPrototype.h"
#include "NPCHandler.h"
#include "Database/DatabaseEnv.h"
#include "Map.h"
#include "MapPersistentStateMgr.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Policies/Singleton.h"
#include "SQLStorages.h"
#include "Conditions.h"

#include <string>
#include <map>
#include <limits>

extern SQLStorage sCreatureDataLinkGroupStorage;

class Group;
class Item;

struct GameTele
{
    float  x = 0.0f;
    float  y = 0.0f;
    float  z = 0.0f;
    float  o = 0.0f;
    uint32 mapId = 0;
    std::string name;
    std::wstring wnameLow;
};

typedef robin_hood::unordered_map<uint32, GameTele > GameTeleMap;

struct AreaTriggerTeleport
{
    std::string message;
    uint8 requiredLevel = 0;
    uint32 requiredCondition = 0;
    uint8 requiredPhase = 0;
    WorldLocation destination;
};

struct ShopEntry
{
    uint32 shopId;
    uint32 Category;
    uint32 Item;
    uint32 ModelID;
    uint32 ItemDisplayID;
    std::string Description;
    std::string Description_loc4;
    uint32 Price;
    G3D::Vector3 Position;
    float Rotation;
    float Scale;
};


typedef robin_hood::unordered_map<uint32, ShopEntry> ShopEntriesMap;

struct ShopCategory
{
	std::string Name;
	std::string Name_loc4;
	std::string Icon;

    std::vector< ShopEntry > Items;

    std::vector< std::string > CachedItemEntries;
};

typedef std::map<uint8, ShopCategory> ShopCategoriesMap;


struct ShopLogEntry
{
    uint32 id;
    std::string date;
    uint32 accountId;
    uint32 charGuid;
    uint32 itemEntry;
    uint32 itemPrice;
    bool refunded;
    uint32 dateUnix;
};

//Acts like bitmask.
enum class ShopRegion : uint32
{
    Global = 0,
    Europe = 1,
    China  = 2
};

struct BattlegroundEntranceTrigger
{
    Team   team;
    BattleGroundTypeId  bgTypeId;
    uint32 exit_mapId;
    float  exit_X;
    float  exit_Y;
    float  exit_Z;
    float  exit_Orientation;
};

struct BroadcastText
{
    BroadcastText() : entry(0), soundId(0), chatType(0), languageId(0), emoteId1(0), emoteId2(0), emoteId3(0),
        emoteDelay1(0), emoteDelay2(0), emoteDelay3(0)
    {
        maleText.resize(LOCALE_enUS + 1);
        femaleText.resize(LOCALE_enUS + 1);
    }

    uint32 entry;
    std::vector<std::string> maleText;
    std::vector<std::string> femaleText;
    uint8  chatType;
    uint32 soundId;
    uint32 languageId;
    uint32 emoteId1;
    uint32 emoteId2;
    uint32 emoteId3;
    uint32 emoteDelay1;
    uint32 emoteDelay2;
    uint32 emoteDelay3;

    std::string const& GetText(int locale_index, uint8 gender, bool forceGender) const
    {
        if ((gender == GENDER_FEMALE || gender == GENDER_NONE) && (forceGender || !femaleText[LOCALE_enUS].empty()))
        {
            if ((int32)femaleText.size() > locale_index + 1 && !femaleText[locale_index + 1].empty())
                return femaleText[locale_index + 1];
            else
                return femaleText[0];
        }
        // else if (gender == GENDER_MALE)
        {
            if ((int32)maleText.size() > locale_index + 1 && !maleText[locale_index + 1].empty())
                return maleText[locale_index + 1];
            else
                return maleText[0];
        }
    }
};

typedef robin_hood::unordered_map<uint32, BroadcastText> BroadcastTextLocaleMap;

struct SoundEntriesEntry
{
    uint32          Id;
    std::string     Name;
};

// Number of spells in one template
#define CREATURE_SPELLS_MAX_SPELLS 8
// Columns in the db for each spell
#define CREATURE_SPELLS_MAX_COLUMNS 11

struct CreatureSpellsEntry
{
    const uint16 spellId;
    const uint8  probability;
    const uint8  castTarget;
    const uint32 targetParam1;
    const uint32 targetParam2;
    const uint16 castFlags;
    const uint32 delayInitialMin;
    const uint32 delayInitialMax;
    const uint32 delayRepeatMin;
    const uint32 delayRepeatMax;
    const uint32 scriptId;
    CreatureSpellsEntry(uint16 Id, uint8 Probability, uint8 CastTarget, uint32 TargetParam1, uint32 TargetParam2, uint16 CastFlags, uint32 InitialMin, uint32 InitialMax, uint32 RepeatMin, uint32 RepeatMax, uint32 ScriptId) : spellId(Id), probability(Probability), castTarget(CastTarget), targetParam1(TargetParam1), targetParam2(TargetParam2), castFlags(CastFlags), delayInitialMin(InitialMin), delayInitialMax(InitialMax), delayRepeatMin(RepeatMin), delayRepeatMax(RepeatMax), scriptId(ScriptId) {}
};

typedef std::vector<CreatureSpellsEntry> CreatureSpellsList;

typedef robin_hood::unordered_map<uint32, CreatureSpellsList> CreatureSpellsMap;

typedef std::map<uint32/*player guid*/,uint32/*instance*/> CellCorpseSet;
struct CellObjectGuids
{
    CellGuidSet creatures;
    CellGuidSet gameobjects;
    CellCorpseSet corpses;
};
typedef robin_hood::unordered_map<uint32/*cell_id*/,CellObjectGuids> CellObjectGuidsMap;
typedef robin_hood::unordered_map<uint32/*mapid*/,CellObjectGuidsMap> MapObjectGuids;

// mangos string ranges
#define MIN_MANGOS_STRING_ID           1                    // 'mangos_string'
#define MAX_MANGOS_STRING_ID           2000000000

struct MangosStringLocale
{
    MangosStringLocale() : SoundId(0), Type(0), LanguageId(LANG_UNIVERSAL), Emote(0) { }

    std::vector<std::string> Content;                       // 0 -> default, i -> i-1 locale index
    uint32 SoundId;
    uint8  Type;
    Language LanguageId;
    uint32 Emote;
};

struct QuestGreetingLocale
{
    QuestGreetingLocale() : Emote(0), EmoteDelay(0) { }

    std::vector<std::string> Content;                       // 0 -> default, i -> i-1 locale index
    uint16 Emote;
    uint32 EmoteDelay;
};

enum
{
    QUESTGIVER_CREATURE = 0,
    QUESTGIVER_GAMEOBJECT = 1,
    QUESTGIVER_TYPE_MAX = 2,
};

struct TrainerGreetingLocale
{
    TrainerGreetingLocale() { }
    std::vector<std::string> Content;                       // 0 -> default, i -> i-1 locale index
};

typedef robin_hood::unordered_map<uint32,CreatureData> CreatureDataMap;
typedef CreatureDataMap::value_type CreatureDataPair;

class FindCreatureData
{
    public:
        FindCreatureData(uint32 id, Player* player) : i_id(id), i_player(player),
            i_anyData(nullptr), i_mapData(nullptr), i_mapDist(0.0f), i_spawnedData(nullptr), i_spawnedDist(0.0f) {}

        bool operator() (CreatureDataPair const& dataPair);
        CreatureDataPair const* GetResult() const;

    private:
        uint32 i_id;
        Player* i_player;

        CreatureDataPair const* i_anyData;
        CreatureDataPair const* i_mapData;
        float i_mapDist;
        CreatureDataPair const* i_spawnedData;
        float i_spawnedDist;
};

typedef robin_hood::unordered_map<uint32, FactionEntry> FactionsMap;
typedef robin_hood::unordered_map<uint32, FactionTemplateEntry> FactionTemplatesMap;
typedef robin_hood::unordered_map<uint32, SoundEntriesEntry> SoundEntryMap;
typedef robin_hood::unordered_map<uint32, ItemPrototype> ItemPrototypeMap;
typedef robin_hood::unordered_map<uint32, std::unique_ptr<CreatureInfo>> CreatureInfoMap;
typedef robin_hood::unordered_map<uint32,GameObjectData> GameObjectDataMap;
typedef GameObjectDataMap::value_type GameObjectDataPair;

// ordered map so we can get max id easily
typedef std::map<uint32, GameObjectInfo> GameObjectInfoMap;

class FindGOData
{
    public:
        FindGOData(uint32 id, Player* player) : i_id(id), i_player(player),
            i_anyData(nullptr), i_mapData(nullptr), i_mapDist(0.0f), i_spawnedData(nullptr), i_spawnedDist(0.0f) {}

        bool operator() (GameObjectDataPair const& dataPair);
        GameObjectDataPair const* GetResult() const;

    private:
        uint32 i_id;
        Player* i_player;

        GameObjectDataPair const* i_anyData;
        GameObjectDataPair const* i_mapData;
        float i_mapDist;
        GameObjectDataPair const* i_spawnedData;
        float i_spawnedDist;
};

typedef robin_hood::unordered_map<uint32,CreatureLocale> CreatureLocaleMap;
typedef robin_hood::unordered_map<uint32,GameObjectLocale> GameObjectLocaleMap;
typedef robin_hood::unordered_map<uint32,ItemLocale> ItemLocaleMap;
typedef robin_hood::unordered_map<uint32,QuestLocale> QuestLocaleMap;
typedef robin_hood::unordered_map<uint32,PageTextLocale> PageTextLocaleMap;
typedef robin_hood::unordered_map<int32,MangosStringLocale> MangosStringLocaleMap;
typedef robin_hood::unordered_map<std::string, MangosStringLocaleMap> ModuleStringLocaleMap;
typedef robin_hood::unordered_map<uint32,QuestGreetingLocale> QuestGreetingLocaleMap;
typedef robin_hood::unordered_map<uint32,GossipMenuItemsLocale> GossipMenuItemsLocaleMap;
typedef robin_hood::unordered_map<uint32,PointOfInterestLocale> PointOfInterestLocaleMap;
typedef robin_hood::unordered_map<uint32,AreaLocale> AreaLocaleMap;
typedef robin_hood::unordered_map<uint32, TrainerGreetingLocale> TrainerGreetingLocaleMap;

typedef std::multimap<int32, uint32> ExclusiveQuestGroupsMap;
typedef std::multimap<uint32, ItemRequiredTarget> ItemRequiredTargetMap;
typedef std::multimap<uint32, uint32> QuestRelationsMap;
typedef std::pair<ExclusiveQuestGroupsMap::const_iterator, ExclusiveQuestGroupsMap::const_iterator> ExclusiveQuestGroupsMapBounds;
typedef std::pair<ItemRequiredTargetMap::const_iterator, ItemRequiredTargetMap::const_iterator> ItemRequiredTargetMapBounds;
typedef std::pair<QuestRelationsMap::const_iterator, QuestRelationsMap::const_iterator> QuestRelationsMapBounds;

struct PetLevelInfo
{
    PetLevelInfo() : health(0), mana(0), armor(0) { for (uint16 & stat : stats) stat = 0; }

    uint16 stats[MAX_STATS];
    uint16 health;
    uint16 mana;
    uint16 armor;
};

// We assume the rate is in general the same for all three types below, but chose to keep three for scalability and customization
struct RepRewardRate
{
    float quest_rate;                                       // We allow rate = 0.0 in database. For this case,
    float creature_rate;                                    // it means that no reputation are given at all
    float spell_rate;                                       // for this faction/rate type.
};

struct ReputationOnKillEntry
{
    uint32 repfaction1;
    uint32 repfaction2;
    bool is_teamaward1;
    uint32 reputation_max_cap1;
    int32 repvalue1;
    bool is_teamaward2;
    uint32 reputation_max_cap2;
    int32 repvalue2;
    bool team_dependent;
};

struct RepSpilloverTemplate
{
    uint32 faction[MAX_SPILLOVER_FACTIONS];
    float faction_rate[MAX_SPILLOVER_FACTIONS];
    uint32 faction_rank[MAX_SPILLOVER_FACTIONS];
};

struct PointOfInterest
{
    uint32 entry = 0;
    float x = 0.0f;
    float y = 0.0f;
    uint32 icon = 0;
    uint32 flags = 0;
    uint32 data = 0;
    std::string icon_name;
};

struct GossipMenuItems
{
    uint32          menu_id = 0;
    uint32          id = 0;
    uint8           option_icon = 0;
    std::string     option_text;
    uint32          option_broadcast_text = 0;
    uint32          option_id = 0;
    uint32          npc_option_npcflag = 0;
    int32           action_menu_id = 0;
    uint32          action_poi_id = 0;
    uint32          action_script_id = 0;
    bool            box_coded = false;
    std::string     box_text;
    uint32          box_broadcast_text = 0;
    uint32          condition_id = 0;
};

struct GossipMenus
{
    uint32          entry = 0;
    uint32          text_id = 0;
    uint32          script_id = 0;
    uint32          condition_id = 0;
};

struct GuildHouseEntry
{
    uint32 map_id;
    float position_x;
    float position_y;
    float position_z;
    float orientation;
};

typedef std::multimap<uint32,GossipMenus> GossipMenusMap;
typedef std::pair<GossipMenusMap::const_iterator, GossipMenusMap::const_iterator> GossipMenusMapBounds;
typedef std::multimap<uint32,GossipMenuItems> GossipMenuItemsMap;
typedef std::pair<GossipMenuItemsMap::const_iterator, GossipMenuItemsMap::const_iterator> GossipMenuItemsMapBounds;

struct PetCreateSpellEntry
{
    uint32 spellid[4];
};

struct GraveYardData
{
    uint32 safeLocId;
    Team team;
};

typedef std::multimap<uint32, GraveYardData> GraveYardMap;
typedef std::pair<GraveYardMap::const_iterator, GraveYardMap::const_iterator> GraveYardMapBounds;
typedef robin_hood::unordered_map<uint32, float> WorldSafeLocsFacingMap;

struct TaxiPathTransition
{
    uint32 inPath;
    uint32 outPath;
    uint32 inNode;
    uint32 outNode;
};

typedef std::multimap<uint32, TaxiPathTransition> TaxiPathTransitionsMap;
typedef std::pair<TaxiPathTransitionsMap::const_iterator, TaxiPathTransitionsMap::const_iterator> TaxiPathTransitionsMapBounds;
typedef std::vector<std::unique_ptr<TaxiNodesEntry>> TaxiNodesStore;

// NPC gossip text id
typedef robin_hood::unordered_map<uint32, uint32> CacheNpcTextIdMap;
typedef robin_hood::unordered_map<uint32, NpcText> NpcTextMap;
typedef robin_hood::unordered_map<uint32, VendorItemData> CacheVendorItemMap;
typedef robin_hood::unordered_map<uint32, TrainerSpellData> CacheTrainerSpellMap;

struct CustomCharacterSkinEntry
{
    uint32 male_id;
    uint32 female_id;
};

enum SkillRangeType
{
    SKILL_RANGE_LANGUAGE,                                   // 300..300
    SKILL_RANGE_LEVEL,                                      // 1..max skill for level
    SKILL_RANGE_MONO,                                       // 1..1, grey monolite bar
    SKILL_RANGE_RANK,                                       // 1..skill for known rank
    SKILL_RANGE_NONE,                                       // 0..0 always
};

SkillRangeType GetSkillRangeType(SkillLineEntry const* skill, SkillRaceClassInfoEntry const* rcEntry);

#define MAX_PLAYER_NAME          12                         // max allowed by client name length
#define MAX_INTERNAL_PLAYER_NAME 15                         // max server internal player name length ( > MAX_PLAYER_NAME for support declined names )
#define MAX_PET_NAME             12                         // max allowed by client name length
#define MAX_CHARTER_NAME         24                         // max allowed by client name length

bool normalizePlayerName(std::string& name, size_t max_len = MAX_INTERNAL_PLAYER_NAME, bool checkCaps = true);

struct LanguageDesc
{
    Language lang_id;
    uint32   spell_id;
    uint32   skill_id;
};

extern LanguageDesc lang_description[LANGUAGES_COUNT];
LanguageDesc const* GetLanguageDescByID(uint32 lang);

class PlayerDumpReader;

template<typename T>
class IdGenerator
{
    public:                                                 // constructors
        explicit IdGenerator(char const* _name) : m_name(_name), m_nextGuid(1) {}

    public:                                                 // modifiers
        void Set(T val) { m_nextGuid = val; }
        T Generate();

    public:                                                 // accessors
        T GetNextAfterMaxUsed() const { return m_nextGuid; }

    private:                                                // fields
        char const* m_name;
        T m_nextGuid;
};

struct SavedVariable
{
    uint32 uiIndex;
    uint32 uiValue;
    bool bSavedInDb;
};
typedef std::unordered_map<uint32, SavedVariable> SavedVariablesMap;

struct PlayerCacheData
{
    uint32 uiGuid;
    uint32 uiLevel;
    uint32 uiAccount;
    uint32 uiRace;
    uint32 uiClass;
    uint32 uiGender;
    uint32 uiZoneId;
    uint32 uiMapId;
    std::string sName;
    float fPosX;
    float fPosY;
    float fPosZ;
    float fOrientation;
    bool bInFlight;
    uint8 uiHardcoreStatus;
};
typedef std::map<uint32 /*guid*/, PlayerCacheData*> PlayerCacheDataMap;

typedef std::map<uint32 /*accountId*/, std::set<PlayerCacheData*>> PlayerCachePerAccountMap;

struct FactionChangeMountData
{
    Races RaceId;
    uint8 MountNum;
    uint32 ItemEntry;
};

typedef std::vector<FactionChangeMountData> FactionChangeMountsData;

bool IsLanguageSkill(uint32 Skill);

typedef robin_hood::unordered_map<uint32, ItemPrototype*> ItemTransmogrifyTemplateMap;

enum PermVariables
{
    // ITEM ID RANGES ARE USED FOR AQ WAR EFFORT

    // Dragons of Nightmare support
    VAR_ALIVE_COUNT = 30000,    // unused
    VAR_REQ_UPDATE  = 30001,    // keep at DEF_STOP_DELAY unless all dragons are dead
    VAR_RESP_TIME   = 30002,    // next event time; should be set in here once last dragon is killed
    VAR_REQ_PERM    = 30003,    // unused
    VAR_PERM_1      = 30004,    // saved permutation result
    VAR_PERM_2      = 30005,
    VAR_PERM_3      = 30006,
    VAR_PERM_4      = 30007,

    DEF_ALIVE_COUNT = 4,        // default alive dragons count for VAR_ALIVE_COUNT
    DEF_STOP_DELAY  = 300,      // default times event check will not stop the event

    NPC_YSONDRE     = 14887,
    NPC_LETHON      = 14888,
    NPC_EMERISS     = 14889,
    NPC_TAERAR      = 14890,

    GUID_YSONDRE    = 52350,
    GUID_LETHON     = 52359,
    GUID_EMERISS    = 52364,
    GUID_TAERAR     = 52357,

    // Elemental Invasion support
    VAR_FIRE        = 30008,    // indicates sub events status through server restarts
    VAR_WATER       = 30009,
    VAR_EARTH       = 30010,
    VAR_AIR         = 30011,
    VAR_FIRE_KILLS  = 30012,    // keeps killed invaders per stage count
    VAR_WATER_KILLS = 30013,
    VAR_EARTH_KILLS = 30014,
    VAR_AIR_KILLS   = 30015,
    VAR_INVAS_TIMER = 30016,    // next invasion time
    VAR_DELAY_FIRE  = 30017,    // time at which boss event should be stopped
    VAR_DELAY_WATER = 30018,
    VAR_DELAY_EARTH = 30019,
    VAR_DELAY_AIR   = 30020,

    STAGE_BOSS         = 5,     // boss is spawned
    STAGE_BOSS_DOWN    = 6,     // boss is dead, event stop time not yet set by core
    STAGE_BOSS_DESPAWN = 7,     // boss is dead, waiting to stop event

    EVENT_IND_FIRE  = 0,
    EVENT_IND_AIR   = 1,
    EVENT_IND_EARTH = 2,
    EVENT_IND_WATER = 3,

    // Stranglethorn Fishing Extravaganza support
    VAR_TOURNAMENT  = 30021,    // last quest completion time
    VAR_TOURN_GOES  = 30022,    // tournament was started already
    VAR_TOURN_OVER  = 30023,    // tournament is over

    // War Effort shared contributions
    VAR_WE_ALLIANCE_COPPER          = 30024,
    VAR_WE_HORDE_COPPER             = 30025,
    VAR_WE_ALLIANCE_PURPLELOTUS     = 30026,
    VAR_WE_HORDE_PURPLELOTUS        = 30027,
    VAR_WE_ALLIANCE_THICKLEATHER    = 30028,
    VAR_WE_HORDE_THICKLEATHER       = 30029,
    VAR_WE_ALLIANCE_SPOTYELLOW      = 30030,
    VAR_WE_HORDE_SPOTYELLOW         = 30031,
    VAR_WE_ALLIANCE_RUNEBANDAGE     = 30032,
    VAR_WE_HORDE_RUNEBANDAGE        = 30033,

    // War Effort stage and event control
    VAR_WE_STAGE                    = 30050,
    VAR_WE_STAGE_TRANSITION_TIME    = 30051,    // The time that the stage transitioned
    VAR_WE_GONG_TIME                = 30052,    // The time at which the gong was rung
    VAR_WE_GONG_BANG_TIMES          = 30053,    // Track how many times the gong has been rung
    VAR_WE_AUTOCOMPLETE_TIME        = 30054,    // The last time the progress auto complete was performed
    VAR_WE_HIVE_REWARD              = 30055,    // A mask of slain colossus events to start

    VAR_APRIL_FOOLS_ENABLED         = 42000
};

class GameObjectUseRequirement
{
public:
    enum GameObjectUseRequireType
    {
        GOBJ_REQUIRE_DEAD_CREATURE  = 0,
        GOBJ_REQUIRE_ACTIVE_OBJECT  = 1,
    };
    GameObjectUseRequireType reqType;
    ObjectGuid guid;
};

class PvPMaintenanceMaker;

struct QuestSpellCastObjective
{
    uint32 questId = 0;
    uint8 idx = 0;
    int32 playerGuid = 0;
    uint8 playerClass = 0;
};

#define SHELL_COIN_BASE_PRICE 20
#define SHELL_COIN_MAX_COUNT ((INT_MAX / SHELL_COIN_BASE_PRICE) - 1)
#define ITEM_SHELL_COIN 81118

class ObjectMgr
{
    friend class PlayerDumpReader;

    public:
        ObjectMgr();
        ~ObjectMgr();

        // Stores all existing ids in the database, not necessarily valid or loaded.
        void LoadAllIdentifiers();
        bool IsExistingItemId(uint32 id) const { return (m_ItemIdSet.find(id) != m_ItemIdSet.end()); }
        bool IsExistingQuestId(uint32 id) const { return (m_QuestIdSet.find(id) != m_QuestIdSet.end()); }
        bool IsExistingCreatureId(uint32 id) const { return (m_CreatureIdSet.find(id) != m_CreatureIdSet.end()); }
        bool IsExistingGameObjectId(uint32 id) const { return (m_GameObjectIdSet.find(id) != m_GameObjectIdSet.end()); }
        bool IsExistingCreatureGuid(uint32 id) const { return (m_CreatureGuidSet.find(id) != m_CreatureGuidSet.end()); }
        bool IsExistingGameObjectGuid(uint32 id) const { return (m_GameObjectGuidSet.find(id) != m_GameObjectGuidSet.end()); }
        bool IsExistingAreaTriggerId(uint32 id) const { return (m_AreaTriggerIdSet.find(id) != m_AreaTriggerIdSet.end()); }
        bool IsExistingCreatureSpellsId(uint32 id) const { return (m_CreatureSpellsIdSet.find(id) != m_CreatureSpellsIdSet.end()); }
        bool IsExistingVendorTemplateId(uint32 id) const { return (m_VendorTemplateIdSet.find(id) != m_VendorTemplateIdSet.end()); }
        bool IsExistingGossipMenuId(uint32 id) const { return (m_GossipMenuIdSet.find(id) != m_GossipMenuIdSet.end()); }

        typedef robin_hood::unordered_map<uint32, Item*> ItemMap;

        typedef std::unordered_map<uint32, Group*> GroupMap;

        typedef robin_hood::unordered_map<uint32, std::unique_ptr<Quest>> QuestMap;
        typedef robin_hood::unordered_map<uint32, AreaTriggerEntry> AreaTriggerMap;
        typedef std::map<uint32, AreaTriggerTeleport> AreaTriggerTeleportMap;
        typedef robin_hood::unordered_map<uint32, BattlegroundEntranceTrigger> BGEntranceTriggerMap;
        typedef robin_hood::unordered_map<uint32, GuildHouseEntry> GuildHouseMap;

        typedef robin_hood::unordered_map<uint32, RepRewardRate > RepRewardRateMap;
        typedef robin_hood::unordered_map<uint32, ReputationOnKillEntry> RepOnKillMap;
        typedef robin_hood::unordered_map<uint32, RepSpilloverTemplate> RepSpilloverTemplateMap;

        typedef robin_hood::unordered_map<uint32, PointOfInterest> PointOfInterestMap;
        typedef robin_hood::unordered_map<uint32, CustomCharacterSkinEntry> CustomCharacterSkinMap;
        typedef robin_hood::unordered_map<uint32, PetCreateSpellEntry> PetCreateSpellMap;

        void LoadChatChannels();
        ChatChannelsEntry const* GetChannelEntryFor(uint32 channelId);
        ChatChannelsEntry const* GetChannelEntryFor(std::string const& name);

        static Player* GetPlayer(const char* name) { return ObjectAccessor::FindPlayerByName(name);}
        static Player* GetPlayer(ObjectGuid guid) { return ObjectAccessor::FindPlayer(guid); }

        GameObjectInfo const* GetGameObjectInfo(uint32 id)
        {
            auto itr = m_GameObjectInfoMap.find(id);
            if (itr != m_GameObjectInfoMap.end())
                return &itr->second;

            return nullptr;
        }
        uint32 GetMaxGameObjectInfoEntry() const
        {
            if (m_GameObjectInfoMap.empty())
                return 0;

            return m_GameObjectInfoMap.rbegin()->first;
        }
        GameObjectInfoMap const& GetGameObjectInfoMap() const
        {
            return m_GameObjectInfoMap;
        }

        void LoadGameobjectInfo();
        void CheckGameObjectInfos();
        void AddGameobjectInfo(GameObjectInfo&& goinfo);
        void LoadGameObjectDisplayInfoAddon();
        void LoadGameobjectsRequirements();
        GameObjectUseRequirement const* GetGameObjectUseRequirement(ObjectGuid guid) const;
        std::map<uint32, GameObjectUseRequirement> _gobjRequirements;

        void PackGroupIds();
        void LoadGuildHouses();
        Group* GetGroupByMember(ObjectGuid memberGuid);
        Group* GetGroupById(uint32 id) const;
        void AddGroup(Group* group);
        void RemoveGroup(Group* group);
        GroupMap::iterator GetGroupMapBegin() { return m_GroupMap.begin(); }
        GroupMap::iterator GetGroupMapEnd() { return m_GroupMap.end(); }

        CreatureDisplayInfoAddon const* GetCreatureDisplayInfoAddon(uint32 display_id);
        CreatureDisplayInfoAddon const* GetCreatureDisplayInfoRandomGender(uint32 display_id);

        EquipmentInfo const *GetEquipmentInfo( uint32 entry );
        static CreatureDataAddon const *GetCreatureAddon( uint32 lowguid )
        {
            return sCreatureDataAddonStorage.LookupEntry<CreatureDataAddon>(lowguid);
        }

        ItemPrototype const* GetItemPrototype(uint32 id) const;

        ItemPrototypeMap const& GetItemPrototypeMap() const
        {
            return m_itemPrototypesMap;
        }

        PetLevelInfo const* GetPetLevelInfo(uint32 creature_id, uint32 level) const;

        PlayerClassInfo const* GetPlayerClassInfo(uint32 class_) const
        {
            if(class_ >= MAX_CLASSES) return nullptr;
            return &m_PlayerClassInfo[class_];
        }
        void GetPlayerClassLevelInfo(uint32 class_,uint32 level, PlayerClassLevelInfo* info) const;

        PlayerInfo const* GetPlayerInfo(uint32 race, uint32 class_) const
        {
            if(race   >= MAX_RACES)   return nullptr;
            if(class_ >= MAX_CLASSES) return nullptr;
            PlayerInfo const* info = &m_PlayerInfo[race][class_];
            if(info->displayId_m==0 || info->displayId_f==0) return nullptr;
            return info;
        }
        void GetPlayerLevelInfo(uint32 race, uint32 class_,uint32 level, PlayerLevelInfo* info) const;

        ObjectGuid GetPlayerGuidByName(std::string const& name) const;
        bool GetPlayerNameByGUID(ObjectGuid guid, std::string &name) const;
        Team GetPlayerTeamByGUID(ObjectGuid guid) const;
        uint8 GetPlayerClassByGUID(ObjectGuid guid) const;
        uint32 GetPlayerAccountIdByGUID(ObjectGuid guid) const;
        uint32 GetPlayerAccountIdByPlayerName(std::string const& name) const;

        void LoadActivePlayersPerFaction();
        bool IsFactionImbalanced(Team team);
        void IncreaseActivePlayersCount(Team team) { m_ActivePlayersPerFaction[team]++; }
        void DecreaseActivePlayersCount(Team team) { m_ActivePlayersPerFaction[team]--; }

        uint32 GetNearestTaxiNode( float x, float y, float z, uint32 mapid, Team team, std::optional<std::function<bool(const TaxiNodesEntry*)>> pred = std::nullopt);
        void GetTaxiPath( uint32 source, uint32 destination, uint32 &path, uint32 &cost);
        uint32 GetTaxiMountDisplayId( uint32 id, Team team, bool allowed_alt_team = false);

        void LoadTaxiPathTransitions();
        TaxiPathTransitionsMapBounds GetTaxiPathTransitionsMapBounds(uint32 entry) const
        {
            return m_TaxiPathTransitions.equal_range(entry);
        }

        void RemoveFakeName(Player* player)
        {
            m_fakeNames.erase(player->GetGUIDLow());
        }
        void AddFakeName(Player* player, const std::string& name)
        {
            m_fakeNames[player->GetGUIDLow()] = name;
        }

        void LoadTaxiNodes();
        TaxiNodesEntry const* GetTaxiNodeEntry(uint32 id) const { return id < GetMaxTaxiNodeId() ? m_TaxiNodes[id].get() : nullptr; }
        uint32 GetMaxTaxiNodeId() const { return m_TaxiNodes.size(); }
#ifdef ENABLE_ELUNA
        void SetTaxiNodeEntry(uint32 id, std::unique_ptr<TaxiNodesEntry>& entry)
        {
            if (m_TaxiNodes.size() <= id)
                m_TaxiNodes.resize(id + 1);
            m_TaxiNodes[id] = std::move(entry);
        }
#endif

        Quest const* GetQuestTemplate(uint32 quest_id) const
        {
            auto itr = m_QuestTemplatesMap.find(quest_id);
            return itr != m_QuestTemplatesMap.end() ? itr->second.get() : nullptr;
        }
        QuestMap const& GetQuestTemplates() const { return m_QuestTemplatesMap; }

        // Return the ID of the item that starts a quest.
        // Return 0 if no such item exists.
        uint32 GetQuestStartingItemID(uint32 quest_id) const;

        uint32 GetQuestForAreaTrigger(uint32 triggerId) const
        {
            auto itr = m_QuestAreaTriggerMap.find(triggerId);
            if(itr != m_QuestAreaTriggerMap.end())
                return itr->second;
            return 0;
        }
        bool IsTavernAreaTrigger(uint32 triggerId) const
        {
            return m_TavernAreaTriggerSet.find(triggerId) != m_TavernAreaTriggerSet.end();
        }

        bool IsGameObjectForQuests(uint32 entry) const
        {
            return m_GameObjectForQuestSet.find(entry) != m_GameObjectForQuestSet.end();
        }

        WorldSafeLocsEntry const *GetClosestGraveYard(float x, float y, float z, uint32 MapId, Team team);
        bool AddGraveYardLink(uint32 id, uint32 zone, Team team, bool inDB = true);
        void RemoveGraveYardLink(uint32 id, uint32 zone, Team team, bool inDB = false);
        void LoadGraveyardZones();
        GraveYardData const* FindGraveYardData(uint32 id, uint32 zone) const;
        void LoadWorldSafeLocsFacing();
        float GetWorldSafeLocFacing(uint32 id) const;
        CustomCharacterSkinEntry const* GetCustomCharacterSkin(uint32 token_id);
        GuildHouseEntry const* GetGuildHouse(uint32 player_guild);

        AreaTriggerTeleport const* GetAreaTriggerTeleport(uint32 trigger) const
        {
            auto itr = m_AreaTriggerTeleportMap.find( trigger );
            if( itr != m_AreaTriggerTeleportMap.end( ) )
                return &itr->second;
            return nullptr;
        }

        AreaTriggerTeleport const* GetGoBackTrigger(uint32 Map) const;
        AreaTriggerTeleport const* GetMapEntranceTrigger(uint32 Map) const;

        void LoadAreaTriggers();
        void LoadCustomCharacterSkins();

        AreaTriggerEntry const* GetAreaTrigger(uint32 id) const
        {
            auto itr = m_AreaTriggersMap.find(id);
            if (itr != m_AreaTriggersMap.end())
                return &itr->second;

            return nullptr;
        }

        AreaTriggerMap const& GetAreaTriggersMap() const
        {
            return m_AreaTriggersMap;
        }

        BattlegroundEntranceTrigger const* GetBattlegroundEntranceTrigger(uint32 trigger) const
        {
            auto itr = m_BGEntranceTriggersMap.find( trigger );
            if( itr != m_BGEntranceTriggersMap.end( ) )
                return &itr->second;
            return nullptr;
        }

        RepRewardRate const* GetRepRewardRate(uint32 factionId) const
        {
            auto itr = m_RepRewardRateMap.find(factionId);
            if (itr != m_RepRewardRateMap.end())
                return &itr->second;

            return nullptr;
        }

        ReputationOnKillEntry const* GetReputationOnKillEntry(uint32 id) const
        {
            auto itr = m_RepOnKillMap.find(id);
            if(itr != m_RepOnKillMap.end())
                return &itr->second;
            return nullptr;
        }

        RepSpilloverTemplate const* GetRepSpilloverTemplate(uint32 factionId) const
        {
            auto itr = m_RepSpilloverTemplateMap.find(factionId);
            if (itr != m_RepSpilloverTemplateMap.end())
                return &itr->second;

            return nullptr;
        }

        PointOfInterest const* GetPointOfInterest(uint32 id) const
        {
            auto itr = m_PointsOfInterestMap.find(id);
            if(itr != m_PointsOfInterestMap.end())
                return &itr->second;
            return nullptr;
        }

        PetCreateSpellEntry const* GetPetCreateSpellEntry(uint32 id) const
        {
            auto itr = m_PetCreateSpellMap.find(id);
            if(itr != m_PetCreateSpellMap.end())
                return &itr->second;
            return nullptr;
        }

        void LoadGroups();
        void LoadQuests();
        void LoadQuestSpellCastObjectives();
        std::vector<QuestSpellCastObjective> const* GetQuestCastObjectivesForSpell(uint32 spellId)
        {
            auto itr = m_questSpellCastObjectives.find(spellId);
            if (itr != m_questSpellCastObjectives.end())
                return &itr->second;

            return nullptr;
        }
        // void ResetYearlyQuests();
        void LoadQuestRelations()
        {
            LoadGameobjectQuestRelations();
            LoadGameobjectInvolvedRelations();
            LoadCreatureQuestRelations();
            LoadCreatureInvolvedRelations();
        }
        void LoadGameobjectQuestRelations();
        void LoadGameobjectInvolvedRelations();
        void LoadCreatureQuestRelations();
        void LoadCreatureInvolvedRelations();

        bool LoadMangosStrings(DatabaseType& db, char const* table, int32 min_value, int32 max_value, bool extra_content);
        bool LoadMangosStrings() { return LoadMangosStrings(WorldDatabase,"mangos_string",MIN_MANGOS_STRING_ID,MAX_MANGOS_STRING_ID, false); }
        bool LoadModuleStrings();
        void LoadBroadcastTexts();
        void LoadBroadcastTextLocales();
        bool LoadQuestGreetings();
        bool LoadTrainerGreetings();
        void LoadPetCreateSpells();
        void LoadPetSpellData();
        void LoadCreatureLocales();
        void LoadCreatureTemplates();
        void LoadCreatureTemplate(uint32 entry);
        void CheckCreatureTemplate(CreatureInfo* cInfo);

        CreatureInfo const* GetCreatureTemplate(uint32 id) const
        {
            auto itr = m_creatureInfoMap.find(id);
            if (itr != m_creatureInfoMap.end())
                return itr->second.get();

            return nullptr;
        }
        CreatureInfoMap const& GetCreatureInfoMap() const
        {
            return m_creatureInfoMap;
        }

        void LoadCreatures(bool reload = false);
        void LoadCreatureAddons();
        void LoadCreatureDisplayInfoAddon();
        void LoadCreatureSpells();
        void LoadEquipmentTemplates();
        void LoadGameObjectLocales();
        void LoadGameobjects(bool reload = false);
        void LoadItemPrototypes();
        void FillObtainedItemsList(std::set<uint32>&);
        void CorrectItemEffects(uint32, _ItemSpell&);
        void LoadItemRequiredTarget();
        void LoadItemLocales();
        void LoadQuestLocales();
        void LoadPageTextLocales();
        void LoadGossipMenuItemsLocales();
        void LoadPointOfInterestLocales();
        void LoadMapTemplate();
        void LoadMailTemplate();
        void LoadConditions();
        void LoadAreaTemplate();
        void LoadAreaLocales();
        void LoadCartographerAreas();
        void LoadShop();

        void LoadNPCText();

        void LoadAreaTriggerTeleports();
        void LoadQuestAreaTriggers();
        void LoadTavernAreaTriggers();
        void LoadGameObjectForQuests();
        void LoadBattlegroundEntranceTriggers();

        void LoadItemTexts();
        void LoadPageTexts();

        void LoadPlayerInfo();
        void LoadPetLevelInfo();
        void LoadExplorationBaseXP();
        void LoadPetNames();
        void LoadPetNumber();
        void LoadCorpses();
        void LoadFishingBaseSkillLevel();

        void LoadReputationRewardRate();
        void LoadReputationOnKill();
        void LoadReputationSpilloverTemplate();

        void LoadPointsOfInterest();

        void LoadGameTele();

        void LoadNpcGossips();

        void LoadGossipMenus();
        void LoadGossipMenu(std::set<uint32>& gossipScriptSet);
        void LoadGossipMenuItems(std::set<uint32>& gossipScriptSet);

        void LoadVendorTemplates();
        void LoadVendors() { LoadVendors("npc_vendor", false); }
        void LoadTrainerTemplates();
        void LoadTrainers() { LoadTrainers("npc_trainer", false); }

        void LoadItemTransmogrifyTemplates();

        std::string GeneratePetName(uint32 entry);
        uint32 GetBaseXP(uint32 level) const;
        uint32 GetXPForLevel(uint32 level) const;
        uint32 GetXPForPetLevel(uint32 level) const { return GetXPForLevel(level)/4; }

        int32 GetFishingBaseSkillLevel(uint32 entry) const
        {
            auto itr = m_FishingBaseSkillMap.find(entry);
            return itr != m_FishingBaseSkillMap.end() ? itr->second : 0;
        }

        void ReturnOrDeleteOldMails(bool serverUp);

        void SetHighestGuids();

        // used for set initial guid counter for map local guids
        uint32 GetFirstTemporaryCreatureLowGuid() const { return m_FirstTemporaryCreatureGuid; }
        uint32 GetFirstTemporaryGameObjectLowGuid() const { return m_FirstTemporaryGameObjectGuid; }

        // used in .npc add/.gobject add commands for adding static spawns
        uint32 GenerateStaticCreatureLowGuid() { if (m_StaticCreatureGuids.GetNextAfterMaxUsed() >= m_FirstTemporaryCreatureGuid) return 0; return m_StaticCreatureGuids.Generate(); }
        uint32 GenerateStaticGameObjectLowGuid() { if (m_StaticGameObjectGuids.GetNextAfterMaxUsed() >= m_FirstTemporaryGameObjectGuid) return 0; return m_StaticGameObjectGuids.Generate(); }

        uint32 GeneratePlayerLowGuid() { return m_CharGuids.Generate(); }
        uint32 GenerateItemLowGuid() { return m_ItemGuids.Generate(); }
        uint32 GenerateCorpseLowGuid() { return m_CorpseGuids.Generate(); }
        void FreeItemLowGuid(uint32 g) { return m_ItemGuids.FreeGuid(g); }

        uint32 GenerateAuctionID();
        void FreeAuctionID(uint32 id);
        uint32 GenerateGuildId() { return m_GuildIds.Generate(); }
        uint32 GenerateGroupId() { return m_GroupIds.Generate(); }
        uint32 GenerateItemTextID() { return m_ItemTextIds.Generate(); }
        uint32 GenerateMailID() { return m_MailIds.Generate(); }
        uint32 GeneratePetitionID() { return m_PetitionIds.Generate(); }
        uint32 GeneratePetNumber();

        void GenerateItemLowGuidRange(uint32& first, uint32& last) { m_ItemGuids.GenerateRange(first, last); }
        void GeneratePetNumberRange(uint32& first, uint32& last);

        uint32 CreateItemText(std::string text);
        void AddItemText(uint32 itemTextId, std::string text) { m_ItemTextsMap[itemTextId] = text; }
        std::string GetItemText( uint32 id )
        {
            ItemTextMap::const_iterator itr = m_ItemTextsMap.find( id );
            if ( itr != m_ItemTextsMap.end() )
                return itr->second;
            return "There is no info for this item";
        }

        CreatureDataPair const* GetCreatureDataPair(uint32 guid) const
        {
            auto itr = m_CreatureDataMap.find(guid);
            if (itr == m_CreatureDataMap.end()) return nullptr;
            return &*itr;
        }

        CreatureData const* GetCreatureData(uint32 guid) const
        {
            CreatureDataPair const* dataPair = GetCreatureDataPair(guid);
            return dataPair ? &dataPair->second : nullptr;
        }

        ObjectGuid GetOneCreatureByEntry(uint32 entry) const
        {
            auto itr = m_CreatureDataMap.begin();
            for (; itr != m_CreatureDataMap.end(); ++itr)
                if (itr->second.creature_id[0] == entry)
                    return ObjectGuid(HIGHGUID_UNIT, itr->second.creature_id[0], itr->first);
            return ObjectGuid();
        }

        CreatureData& NewOrExistCreatureData(uint32 guid) { return m_CreatureDataMap[guid]; }
        void DeleteCreatureData(uint32 guid);

        template<typename Worker>
        void DoCreatureData(Worker& worker) const
        {
            for (const auto& itr : m_CreatureDataMap)
                if (worker(itr))
                    break;
        }

        CreatureLocale const* GetCreatureLocale(uint32 entry) const
        {
            auto itr = m_CreatureLocaleMap.find(entry);
            if(itr==m_CreatureLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        CreatureSpellsList const* GetCreatureSpellsList(uint32 entry) const
        {
            auto itr = m_CreatureSpellsMap.find(entry);
            if (itr == m_CreatureSpellsMap.end()) return nullptr;
            return &itr->second;
        }

        GameObjectLocale const* GetGameObjectLocale(uint32 entry) const
        {
            auto itr = m_GameObjectLocaleMap.find(entry);
            if(itr==m_GameObjectLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        std::string GetItemLocaleName(uint32 entry, uint32 loc_idx)
        {
            const ItemPrototype* proto = GetItemPrototype(entry);
            if (!proto)
                return "ITEM NOT EXIST";

            ItemLocale const* il = GetItemLocale(entry);

            if (!il)
                return proto->Name1;

            if (il->Name.size() < loc_idx)
                return proto->Name1;
            else
                return il->Name[loc_idx];
        }

        ItemLocale const* GetItemLocale(uint32 entry) const
        {
            auto itr = m_ItemLocaleMap.find(entry);
            if(itr==m_ItemLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        QuestLocale const* GetQuestLocale(uint32 entry) const
        {
            auto itr = m_QuestLocaleMap.find(entry);
            if(itr==m_QuestLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        NpcText const* GetNpcText(uint32 entry) const
        {
            auto itr = m_NpcTextMap.find(entry);
            if(itr==m_NpcTextMap.end()) return nullptr;
            return &itr->second;
        }

        PageTextLocale const* GetPageTextLocale(uint32 entry) const
        {
            auto itr = m_PageTextLocaleMap.find(entry);
            if(itr==m_PageTextLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        GossipMenuItemsLocale const* GetGossipMenuItemsLocale(uint32 entry) const
        {
            auto itr = m_GossipMenuItemsLocaleMap.find(entry);
            if(itr==m_GossipMenuItemsLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        PointOfInterestLocale const* GetPointOfInterestLocale(uint32 poi_id) const
        {
            auto itr = m_PointOfInterestLocaleMap.find(poi_id);
            if(itr==m_PointOfInterestLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        void GetAreaLocaleString(uint32 entry, int32 loc_idx, std::string* namePtr) const;
        AreaLocale const* GetAreaLocale(uint32 entry) const
        {
            AreaLocaleMap::const_iterator itr = m_AreaLocaleMap.find(entry);
            if (itr == m_AreaLocaleMap.end()) return nullptr;
            return &itr->second;
        }
        AreaEntry const* GetAreaEntryByName(std::string const& name) const;
        AreaEntry const* GetAreaEntryByExploreFlag(uint32 flag) const;
        uint32 const* GetCartographerExplorationMask() const { return m_cartographerExploreMask; }

        GameObjectDataPair const* GetGODataPair(uint32 guid) const
        {
            auto itr = m_GameObjectDataMap.find(guid);
            if(itr==m_GameObjectDataMap.end()) return nullptr;
            return &*itr;
        }

        GameObjectData const* GetGOData(uint32 guid) const
        {
            auto dataPair = GetGODataPair(guid);
            return dataPair ? &dataPair->second : nullptr;
        }

        GameObjectData& NewGOData(uint32 guid) { return m_GameObjectDataMap[guid]; }
        void DeleteGOData(uint32 guid);

        template<typename Worker>
        void DoGOData(Worker& worker) const
        {
            for (const auto& itr : m_GameObjectDataMap)
                if (worker(itr)) // arg = GameObjectDataPair
                    break;
        }

        char const* GetMailTextTemplate(uint32 id, LocaleConstant locale_idx = LOCALE_enUS);

        BroadcastText const* GetBroadcastTextLocale(uint32 id) const
        {
            BroadcastTextLocaleMap::const_iterator itr = m_BroadcastTextLocaleMap.find(id);
            if (itr != m_BroadcastTextLocaleMap.end())
                return &itr->second;
            return nullptr;
        }

        char const* GetBroadcastText(uint32 id, int locale_idx = DB_LOCALE_enUS, uint8 gender = GENDER_MALE, bool forceGender = false) const;

        MangosStringLocale const* GetMangosStringLocale(int32 entry) const
        {
            auto itr = m_MangosStringLocaleMap.find(entry);
            if(itr==m_MangosStringLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        const char *GetMangosString(int32 entry, int locale_idx) const;
        const char *GetMangosStringForDBCLocale(int32 entry) const { return GetMangosString(entry,DBCLocaleIndex); }
        const char* GetModuleString(std::string const& module, uint32 id, int locale_idx) const;
        const char* GetModuleString(char const* module, uint32 id, int locale_idx) const { return GetModuleString(std::string(module ? module : ""), id, locale_idx); }
        int32 GetDBCLocaleIndex() const { return DBCLocaleIndex; }
        void SetDBCLocaleIndex(uint32 lang) { DBCLocaleIndex = GetIndexForLocale(LocaleConstant(lang)); }

        QuestGreetingLocale const* GetQuestGreetingLocale(uint32 entry, uint8 type) const
        {
            auto itr = m_QuestGreetingLocaleMap[type].find(entry);
            if (itr == m_QuestGreetingLocaleMap[type].end()) return nullptr;
            return &itr->second;
        }

        TrainerGreetingLocale const* GetTrainerGreetingLocale(uint32 entry) const
        {
            auto itr = m_TrainerGreetingLocaleMap.find(entry);
            if (itr == m_TrainerGreetingLocaleMap.end()) return nullptr;
            return &itr->second;
        }

        // global grid objects state (static DB spawns, global spawn mods from gameevent system)
        CellObjectGuids const& GetCellObjectGuids(uint16 mapid, uint32 cell_id)
        {
            std::unique_lock<std::mutex> lock(m_MapObjectGuids_lock);
            CellObjectGuids const& guids = m_MapObjectGuids[mapid][cell_id];
            return guids;
        }
        std::mutex& GetCellLoadingObjectsMutex() // TODO: Mutex per cell?
        {
            return m_MapObjectGuids_lock;
        }

        // modifiers for global grid objects state (static DB spawns, global spawn mods from gameevent system)
        // Don't must be used for modify instance specific spawn state modifications
        void AddCreatureToGrid(uint32 guid, CreatureData const* data);
        void RemoveCreatureFromGrid(uint32 guid, CreatureData const* data);
        void AddGameobjectToGrid(uint32 guid, GameObjectData const* data);
        void RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data);
        void AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance);
        void DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid);

        // reserved names
        void LoadReservedPlayersNames();
        bool IsReservedName(std::string const& name) const;
        void AddReservedName(std::string name);

        // name with valid structure and symbols
        static uint8 CheckPlayerName(std::string const& name, bool create = false);
        static PetNameInvalidReason CheckPetName(std::string const& name);
        static bool IsValidCharterName(std::string const& name);

        int GetIndexForLocale(LocaleConstant loc);
        LocaleConstant GetLocaleForIndex(int i);

        bool IsConditionSatisfied(uint32 conditionId, WorldObject const* target, Map const* map, WorldObject const* source, ConditionSource conditionSourceType) const;

        GameTele const* GetGameTele(uint32 id) const
        {
            auto itr = m_GameTeleMap.find(id);
            if(itr==m_GameTeleMap.end()) return nullptr;
            return &itr->second;
        }

        GameTele const* GetGameTele(std::string const& name) const;
        GameTeleMap const& GetGameTeleMap() const { return m_GameTeleMap; }
        bool AddGameTele(GameTele& data);
        bool DeleteGameTele(std::string const& name);

        uint32 GetNpcGossip(uint32 entry) const
        {
            auto iter = m_CacheNpcTextIdMap.find(entry);
            if(iter == m_CacheNpcTextIdMap.end())
                return 0;

            return iter->second;
        }

        TrainerSpellData const* GetNpcTrainerSpells(uint32 entry) const
        {
            auto iter = m_CacheTrainerSpellMap.find(entry);
            if(iter == m_CacheTrainerSpellMap.end())
                return nullptr;

            return &iter->second;
        }

        TrainerSpellData const* GetNpcTrainerTemplateSpells(uint32 entry) const
        {
            auto iter = m_CacheTrainerTemplateSpellMap.find(entry);
            if(iter == m_CacheTrainerTemplateSpellMap.end())
                return nullptr;

            return &iter->second;
        }

        VendorItemData const* GetNpcVendorItemList(uint32 entry) const
        {
            auto iter = m_CacheVendorItemMap.find(entry);
            if(iter == m_CacheVendorItemMap.end())
                return nullptr;

            return &iter->second;
        }

        VendorItemData const* GetNpcVendorTemplateItemList(uint32 entry) const
        {
            auto iter = m_CacheVendorTemplateItemMap.find(entry);
            if(iter == m_CacheVendorTemplateItemMap.end())
                return nullptr;

            return &iter->second;
        }

        void AddVendorItem(uint32 entry,uint32 item, uint32 maxcount, uint32 incrtime, uint32 itemflags);
        bool RemoveVendorItem(uint32 entry,uint32 item);
        bool IsVendorItemValid(bool isTemplate, char const* tableName, uint32 vendor_entry, uint32 item, uint32 maxcount, uint32 incrtime, uint32 conditionId, Player* pl = nullptr) const;

        int GetOrNewIndexForLocale(LocaleConstant loc);

        ShopCategory const* GetShopCategoryInfo(uint8 category) const
        {
            auto iter = m_ShopCategoriesMap.find(category);
            if (iter == m_ShopCategoriesMap.end())
                return nullptr;

            return &iter->second;
        }

        ShopEntry const* GetShopEntryInfo(uint32 id) const
        {
            auto iter = m_ShopEntriesMap.find(id);
            if (iter == m_ShopEntriesMap.end())
                return nullptr;

            return &iter->second;
        }

        ShopEntriesMap const& GetShopEntriesList() const
        {
            return m_ShopEntriesMap;
        }

        const ShopCategoriesMap& GetShopCategoriesList() const
        {
            return m_ShopCategoriesMap;
        }

        std::vector<ShopLogEntry*>& GetShopLogEntries(uint32 accountId)
        {
            return m_shopLogs[accountId];
        }

        ShopLogEntry* GetShopLogEntry(uint32 shopId)
        {
            return m_shopLogsLookup[shopId];
        }

        void AddShopLogEntry(uint32 shopId, ShopLogEntry* shoplogEntry)
        {
            m_shopLogsLookup[shopId] = shoplogEntry;
        }

        uint32 NextShopLogEntry()
        {
            return ++m_maxShopEntry;
        }


        ItemRequiredTargetMapBounds GetItemRequiredTargetMapBounds(uint32 uiItemEntry) const
        {
            return m_ItemRequiredTarget.equal_range(uiItemEntry);
        }

        GossipMenusMapBounds GetGossipMenusMapBounds(uint32 uiMenuId) const
        {
            return m_GossipMenusMap.equal_range(uiMenuId);
        }

        GossipMenuItemsMapBounds GetGossipMenuItemsMapBounds(uint32 uiMenuId) const
        {
            return m_GossipMenuItemsMap.equal_range(uiMenuId);
        }

        ExclusiveQuestGroupsMapBounds GetExclusiveQuestGroupsMapBounds(int32 groupId) const
        {
            return m_ExclusiveQuestGroups.equal_range(groupId);
        }

        // Transmog
        ItemPrototype const* GetItemTransmogrifyTemplate(uint32 transmogrifyId) const
        {
            auto itr = m_itemTransmogs.find(transmogrifyId);
            if (itr == m_itemTransmogs.end()) return nullptr;
            return itr->second;
        }
        ItemTransmogrifyTemplateMap const& GetItemTransmogrifyTemplateMap() const { return m_itemTransmogs; }
        uint32 CreateItemTransmogrifyTemplate(uint32 destItemId, uint32 sourceDisplayId, uint32 sourceId);
        void DeleteItemTransmogrifyTemplate(uint32 transmogrifyId);
        std::map<uint32, std::map<uint32, std::map<uint32, std::map<uint32, TransmogContainer>>>> NumPossibleTransmogs;
        bool IsItemTypeTransmoggable(uint32 invType);
        bool IsItemSubClassTransmoggable(uint32 subClass);
        void FillPossibleTransmogs();
        void AddPossibleTransmog(uint8 pClass, uint32 itemClass, uint32 itemSubClass, uint32 invType, uint32 displayId);
        uint32 GetPossibleTransmogs(uint8 pClass, uint32 itemClass, uint32 itemSubClass, uint32 invType, bool stupid);


        // Deactivated Spells
        std::set<uint32>    m_DisabledSpells;
        void LoadSpellDisabledEntrys();
        bool IsSpellDisabled(uint32 spellid) const { return m_DisabledSpells.count(spellid) != 0; }
        // map_loot_disabled
        std::set<uint32>    m_DisabledMapLoots;
        void LoadMapLootDisabled();
        bool IsMapLootDisabled(uint32 mapId) const { return m_DisabledMapLoots.count(mapId) != 0; }
        // Cinematics
        void LoadCinematicsWaypoints();
        Position const* GetCinematicPosition(uint32 cinematicId, uint32 elapsed_time);
        Position const* GetCinematicInitialPosition(uint32 cinematicId);
        std::vector<CinematicWaypointEntry> m_CinematicWaypoints;
        // Phasing
        void LoadPlayerPhaseFromDb();
        uint32 GetPlayerWorldMaskByGUID(const uint64 guid);
        void SetPlayerWorldMask(const uint64 guid, uint32 newWorldMask);
        std::map<uint32, uint32> m_PlayerPhases;

        // Saving Variables
        SavedVariable& _InsertVariable(uint32 index, uint32 value, bool saved);
        void _SaveVariable(const SavedVariable& toSave);

        void InitSavedVariable(uint32 index, uint32 value);
        uint32 GetSavedVariable(uint32 index, uint32 defaultValue = 0, bool *exist = nullptr);
        void SetSavedVariable(uint32 index, uint32 value, bool SaveToDb = false);
        void LoadVariable(uint32 index, uint32* variable, uint32 defaultValue, uint32 maxValue=0, uint32 minValue=0);

        void LoadSavedVariable();
        void SaveVariables();
        SavedVariablesMap m_SavedVariables;

        // Caching Player Data
        void LoadPlayerCacheData(uint32 lowGuid = 0);
        PlayerCacheData* GetPlayerDataByGUID(uint32 lowGuid) const;
        PlayerCacheData* GetPlayerDataByName(std::string const& name) const;
        void GetPlayerDataForAccount(uint32 accountId, std::vector<PlayerCacheData*>& data) const;
        PlayerCacheData* InsertPlayerInCache(Player *pPlayer);
        PlayerCacheData* InsertPlayerInCache(uint32 lowGuid, uint32 race, uint32 _class, uint32 uiGender, uint32 account, std::string const& name, uint32 level, uint32 zoneId, uint8 hardcoreStatus);
        void DeletePlayerFromCache(uint32 lowGuid);
        void DeletePlayerNameFromCache(std::string const& name);
        void ChangePlayerNameInCache(uint32 lowGuid, std::string const& oldName, std::string const& newName);
        void UpdatePlayerCachedPosition(Player *pPlayer);
        void UpdatePlayerCachedPosition(uint32 lowGuid, uint32 mapId, float posX, float posY, float posZ, float o, bool inFlight);
        void UpdatePlayerCachedPosition(PlayerCacheData* data, uint32 mapId, float posX, float posY, float posZ, float o, bool inFlight);
        void UpdatePlayerCache(Player* pPlayer);
        void UpdatePlayerCache(PlayerCacheData* data, uint32 race, uint32 _class, uint32 gender, uint32 accountId, std::string const& name, uint32 level, uint32 zoneId, uint8 hardcoreStatus);

        PlayerCacheDataMap m_playerCacheData;

        // Turtle specific. Should be synchronized with m_playerCacheData, since it using pointers from that map!
        PlayerCachePerAccountMap m_accountPlayersCacheData;

        std::map<std::string, uint32> m_playerNameToGuid;

        uint32 AddCreData(uint32 entry, uint32 team, uint32 map, float, float, float, float, uint32 spawnDelay);
        uint32 AddGOData(uint32 entry, uint32 map, float, float, float, float, uint32 spawnTimeDelay, float, float, float, float);
        bool MoveCreData(uint32 guid, uint32 mapId, const Position& pos);

        // Sound Entries
        void LoadSoundEntries();
        SoundEntriesEntry const* GetSoundEntry(uint32 id) const
        {
            auto iter = m_SoundEntriesMap.find(id);
            if (iter == m_SoundEntriesMap.end())
                return nullptr;

            return &iter->second;
        }

        SoundEntryMap const& GetSoundEntriesMap() const
        {
            return m_SoundEntriesMap;
        }

        // Factions
        void LoadFactions();
        FactionEntry const* GetFactionEntry(uint32 id) const
        {
            auto iter = m_FactionsMap.find(id);
            if (iter == m_FactionsMap.end())
                return nullptr;

            return &iter->second;
        }
        FactionsMap const& GetFactionMap() const
        {
            return m_FactionsMap;
        }
        FactionTemplateEntry const* GetFactionTemplateEntry(uint32 id) const
        {
            auto iter = m_FactionTemplatesMap.find(id);
            if (iter == m_FactionTemplatesMap.end())
                return nullptr;

            return &iter->second;
        }
        FactionTemplatesMap const& GetFactionTemplateMap() const
        {
            return m_FactionTemplatesMap;
        }

        // Skill Line Abilities
        void LoadSkillLineAbility();
        SkillLineAbilityEntry const* GetSkillLineAbility(uint32 id) const { return id < GetMaxSkillLineAbilityId() ? m_SkillLineAbilities[id].get() : nullptr; }
        uint32 GetMaxSkillLineAbilityId() const { return m_SkillLineAbilities.size(); }



        // Changes of faction
        typedef std::map<uint32, uint32> CharacterConversionMap;
        CharacterConversionMap factionchange_reputations;
        CharacterConversionMap factionchange_spells;
        CharacterConversionMap factionchange_items;
        CharacterConversionMap factionchange_quests;
        FactionChangeMountsData factionchange_mounts;
        void LoadFactionChangeReputations();
        void LoadFactionChangeSpells();
        void LoadFactionChangeItems();
        void LoadFactionChangeQuests();
        void LoadFactionChangeMounts();
        void RestoreDeletedItems();
        bool GetMountDataByEntry(uint32 itemEntry, Races& race, uint8& mountNum) const;
        uint32 GetMountItemEntry(Races race, uint8 num) const;
        uint32 GetRandomMountForRace(Races race) const;
        Races GetOppositeRace(Races origRace) const;

        QuestRelationsMapBounds GetCreatureQuestRelationsMapBounds(uint32 entry) const
        {
            return m_CreatureQuestRelations.equal_range(entry);
        }

        QuestRelationsMapBounds GetCreatureQuestInvolvedRelationsMapBounds(uint32 entry) const
        {
            return m_CreatureQuestInvolvedRelations.equal_range(entry);
        }

        QuestRelationsMapBounds GetGOQuestRelationsMapBounds(uint32 entry) const
        {
            return m_GOQuestRelations.equal_range(entry);
        }

        QuestRelationsMapBounds GetGOQuestInvolvedRelationsMapBounds(uint32 entry) const
        {
            return m_GOQuestInvolvedRelations.equal_range(entry);
        }

        QuestRelationsMap& GetCreatureQuestRelationsMap() { return m_CreatureQuestRelations; }

        void ResetOldMailCounter() { m_OldMailCounter = 0; }
        void IncrementOldMailCounter(uint32 count) { m_OldMailCounter += count; }

        void LoadShellCoinCount();
        int32 GetShellCoinCount() const { return m_shellCoinCount; }
        int32 GetShellCoinSellPrice() const { return m_shellCoinCount * SHELL_COIN_BASE_PRICE; }
        int32 GetShellCoinBuyPrice() const { return (m_shellCoinCount + 1) * SHELL_COIN_BASE_PRICE; }
        void IncreaseShellCoinCount() { if (m_shellCoinCount < SHELL_COIN_MAX_COUNT) m_shellCoinCount++; }
        void DecreaseShellCoinCount() { if (m_shellCoinCount > 0) m_shellCoinCount--; }

        void BackupCharacterInventory();

    protected:

        // first free id for selected id type
        IdGenerator<uint32> m_GuildIds;
        IdGenerator<uint32> m_MailIds;
        IdGenerator<uint32> m_GroupIds;
        IdGenerator<uint32> m_PetitionIds;
        uint32              m_NextPetNumber;
        std::set<uint32>    m_AuctionsIds;
        uint32              m_NextAuctionId;

        // initial free low guid for selected guid type for map local guids
        uint32 m_FirstTemporaryCreatureGuid;
        uint32 m_FirstTemporaryGameObjectGuid;

        // guids from reserved range for use in .npc add/.gobject add commands for adding new static spawns (saved in DB) from client.
        ObjectGuidGenerator<HIGHGUID_UNIT>        m_StaticCreatureGuids;
        ObjectGuidGenerator<HIGHGUID_GAMEOBJECT>  m_StaticGameObjectGuids;

        // first free low guid for selected guid type
        ObjectGuidGenerator<HIGHGUID_PLAYER>     m_CharGuids;
        ObjectSafeGuidGenerator<HIGHGUID_ITEM>   m_ItemGuids;   // Needs to be thread safe
        ObjectSafeGuidGenerator<HIGHGUID_ITEM>   m_ItemTextIds;
        ObjectGuidGenerator<HIGHGUID_CORPSE>     m_CorpseGuids;

        QuestMap            m_QuestTemplatesMap;

        robin_hood::unordered_map<uint32, std::string> m_fakeNames;

        typedef robin_hood::unordered_map<uint32, uint32> QuestAreaTriggerMap;
        typedef robin_hood::unordered_map<uint32, std::string> ItemTextMap;
        // Map quest_id->id of start item
        typedef robin_hood::unordered_map<uint32, uint32> QuestStartingItemMap;
        typedef std::set<uint32> TavernAreaTriggerSet;
        typedef std::set<uint32> GameObjectForQuestSet;

        GroupMap            m_GroupMap;

        ItemTextMap         m_ItemTextsMap;

        AreaTriggerMap    m_AreaTriggersMap;
        QuestAreaTriggerMap m_QuestAreaTriggerMap;
        TavernAreaTriggerSet m_TavernAreaTriggerSet;
        GameObjectForQuestSet m_GameObjectForQuestSet;
        AreaTriggerTeleportMap      m_AreaTriggerTeleportMap;
        QuestStartingItemMap   m_QuestStartingItemsMap;
        BGEntranceTriggerMap m_BGEntranceTriggersMap;
        CustomCharacterSkinMap    m_CustomCharacterSkinMap;
        GuildHouseMap    m_GuildHouseMap;

        RepRewardRateMap    m_RepRewardRateMap;
        RepOnKillMap        m_RepOnKillMap;
        RepSpilloverTemplateMap m_RepSpilloverTemplateMap;

        GossipMenusMap      m_GossipMenusMap;
        GossipMenuItemsMap  m_GossipMenuItemsMap;
        PointOfInterestMap  m_PointsOfInterestMap;

        PetCreateSpellMap   m_PetCreateSpellMap;

        //character reserved names
        typedef std::set<std::wstring> ReservedNamesMap;
        ReservedNamesMap    m_ReservedNames;

        GraveYardMap        m_GraveYardMap;

        WorldSafeLocsFacingMap m_worldSafeLocsFacingMap;

        GameTeleMap         m_GameTeleMap;

        ItemRequiredTargetMap m_ItemRequiredTarget;

        typedef             std::vector<LocaleConstant> LocalForIndex;
        LocalForIndex        m_LocalForIndex;

        ExclusiveQuestGroupsMap m_ExclusiveQuestGroups;

        QuestRelationsMap       m_CreatureQuestRelations;
        QuestRelationsMap       m_CreatureQuestInvolvedRelations;
        QuestRelationsMap       m_GOQuestRelations;
        QuestRelationsMap       m_GOQuestInvolvedRelations;

        TaxiPathTransitionsMap  m_TaxiPathTransitions;
        TaxiNodesStore          m_TaxiNodes;

        ItemTransmogrifyTemplateMap     m_itemTransmogs;
        std::atomic_uint m_transmogId = 0;

        int DBCLocaleIndex;

        uint32 m_OldMailCounter;
        int32 m_shellCoinCount = 0;

    private:
        void LoadCreatureInfo(Field* result);
        void LoadCreatureAddons(SQLStorage& creatureaddons, char const* entryName, char const* comment);
        void LoadQuestRelationsHelper(QuestRelationsMap& map, char const* table);
        void LoadVendors(char const* tableName, bool isTemplates);
        void LoadTrainers(char const* tableName, bool isTemplates);

        // Storing all existing IDs in database.
        std::set<uint32> m_ItemIdSet;
        std::set<uint32> m_QuestIdSet;
        std::set<uint32> m_CreatureIdSet;
        std::set<uint32> m_GameObjectIdSet;
        std::set<uint32> m_CreatureGuidSet;
        std::set<uint32> m_GameObjectGuidSet;
        std::set<uint32> m_AreaTriggerIdSet;
        std::set<uint32> m_CreatureSpellsIdSet;
        std::set<uint32> m_VendorTemplateIdSet;
        std::set<uint32> m_GossipMenuIdSet;

        typedef std::map<uint32,PetLevelInfo*> PetLevelInfoMap;
        // PetLevelInfoMap[creature_id][level]
        PetLevelInfoMap m_PetInfoMap;                            // [creature_id][level]

        PlayerClassInfo m_PlayerClassInfo[MAX_CLASSES];

        void BuildPlayerLevelInfo(uint8 race, uint8 class_, uint8 level, PlayerLevelInfo* plinfo) const;
        PlayerInfo m_PlayerInfo[MAX_RACES][MAX_CLASSES];

        typedef std::vector<uint32> PlayerXPperLevel;       // [level]
        PlayerXPperLevel m_PlayerXPperLevel;

        robin_hood::unordered_map<Team, uint32> m_ActivePlayersPerFaction;

        ShopCategoriesMap m_ShopCategoriesMap;
        ShopEntriesMap m_ShopEntriesMap;

        robin_hood::unordered_map<uint32, std::vector<ShopLogEntry*>> m_shopLogs;
        robin_hood::unordered_map<uint32, ShopLogEntry*> m_shopLogsLookup;
        std::atomic_uint32_t m_maxShopEntry = 0;

        typedef std::map<uint32,uint32> BaseXPMap;          // [area level][base xp]
        BaseXPMap m_BaseXPMap;

        typedef std::map<uint32,int32> FishingBaseSkillMap; // [areaId][base skill level]
        FishingBaseSkillMap m_FishingBaseSkillMap;

        typedef std::map<uint32,std::vector<std::string> > HalfNameMap;
        HalfNameMap m_PetHalfNameMap0;
        HalfNameMap m_PetHalfNameMap1;

        MapObjectGuids m_MapObjectGuids;
        std::mutex m_MapObjectGuids_lock;

        CreatureDataMap m_CreatureDataMap;
        CreatureLocaleMap m_CreatureLocaleMap;
        CreatureSpellsMap m_CreatureSpellsMap;
        GameObjectDataMap m_GameObjectDataMap;
        GameObjectLocaleMap m_GameObjectLocaleMap;
        ItemLocaleMap m_ItemLocaleMap;
        QuestLocaleMap m_QuestLocaleMap;
        NpcTextMap m_NpcTextMap;
        PageTextLocaleMap m_PageTextLocaleMap;
        MangosStringLocaleMap m_MangosStringLocaleMap;
        ModuleStringLocaleMap m_ModuleStringLocaleMap;
        BroadcastTextLocaleMap m_BroadcastTextLocaleMap;
        QuestGreetingLocaleMap m_QuestGreetingLocaleMap[QUESTGIVER_TYPE_MAX];
        TrainerGreetingLocaleMap m_TrainerGreetingLocaleMap;
        GossipMenuItemsLocaleMap m_GossipMenuItemsLocaleMap;
        PointOfInterestLocaleMap m_PointOfInterestLocaleMap;
        AreaLocaleMap m_AreaLocaleMap;
        uint32 m_cartographerExploreMask[PLAYER_EXPLORED_ZONES_SIZE];

        FactionsMap m_FactionsMap;
        FactionTemplatesMap m_FactionTemplatesMap;

        SoundEntryMap m_SoundEntriesMap;
        ItemPrototypeMap m_itemPrototypesMap;
        CreatureInfoMap m_creatureInfoMap;
        GameObjectInfoMap m_GameObjectInfoMap;

        typedef std::vector<std::unique_ptr<SkillLineAbilityEntry>> SkillLineAbiilityStore;
        SkillLineAbiilityStore m_SkillLineAbilities;

        CacheNpcTextIdMap m_CacheNpcTextIdMap;
        CacheVendorItemMap m_CacheVendorTemplateItemMap;
        CacheVendorItemMap m_CacheVendorItemMap;
        CacheTrainerSpellMap m_CacheTrainerTemplateSpellMap;
        CacheTrainerSpellMap m_CacheTrainerSpellMap;
        robin_hood::unordered_map<uint32, ChatChannelsEntry> m_chatChannelsMap;
        robin_hood::unordered_map<uint32 /*spell id*/, std::vector<QuestSpellCastObjective>> m_questSpellCastObjectives;
};

extern ObjectMgr sObjectMgr;

#endif
