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

#include "ObjectMgr.h"
#include "Database/DatabaseEnv.h"
#include "Database/DatabaseImpl.h"
#include "Database/SQLStorageImpl.h"
#include "Policies/SingletonImp.h"

#include "SQLStorages.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectGuid.h"
#include "ScriptObjects.h"
#include "ScriptMgr.h"
#include "SpellMgr.h"
#include "UpdateMask.h"
#include "World.h"
#include "Group.h"
#include "Transport.h"
#include "Language.h"
#include "PoolManager.h"
#include "GameEventMgr.h"
#include "Spell.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "MapPersistentStateMgr.h"
#include "SpellAuras.h"
#include "Util.h"
#include "WaypointManager.h"
#include "GossipDef.h"
#include "Mail.h"
#include "Formulas.h"
#include "InstanceData.h"
#include "CharacterDatabaseCache.h"
#include "HardcodedEvents.h"
#include "miscellaneous/feature_transmog.h"
#include "Geometry.h"

#include <limits>

ObjectMgr sObjectMgr;

#include "utf8cpp/utf8.h"

bool normalizePlayerName(std::string& name, size_t max_len, bool checkCaps)
{
    if (name.empty())
        return false;

    if (name[0] == -61 && name[1] == -97)
        return false;

    std::wstring wstr_buf;
    if (!Utf8toWStr(name, wstr_buf))
        return false;

    size_t len = wstr_buf.size();
    if (len > max_len)
        return false;

    if (checkCaps)
    {
        wstr_buf[0] = wcharToUpper(wstr_buf[0]);
        for (size_t i = 1; i < len; ++i)
            wstr_buf[i] = wcharToLower(wstr_buf[i]);
    }

    return WStrToUtf8(wstr_buf, name);
}

LanguageDesc lang_description[LANGUAGES_COUNT] =
{
    { LANG_ADDON,           0, 0                       },
    { LANG_UNIVERSAL,       0, 0                       },
    { LANG_ORCISH,        669, SKILL_LANG_ORCISH       },
    { LANG_DARNASSIAN,    671, SKILL_LANG_DARNASSIAN   },
    { LANG_TAURAHE,       670, SKILL_LANG_TAURAHE      },
    { LANG_DWARVISH,      672, SKILL_LANG_DWARVEN      },
    { LANG_COMMON,        668, SKILL_LANG_COMMON       },
    { LANG_DEMONIC,       815, SKILL_LANG_DEMON_TONGUE },
    { LANG_TITAN,         816, SKILL_LANG_TITAN        },
    { LANG_THALASSIAN,    813, SKILL_LANG_THALASSIAN   },
    { LANG_DRACONIC,      814, SKILL_LANG_DRACONIC     },
    { LANG_KALIMAG,       817, SKILL_LANG_OLD_TONGUE   },
    { LANG_GNOMISH,      7340, SKILL_LANG_GNOMISH      },
    { LANG_TROLL,        7341, SKILL_LANG_TROLL        },
    { LANG_GUTTERSPEAK, 17737, SKILL_LANG_GUTTERSPEAK  },
};

LanguageDesc const* GetLanguageDescByID(uint32 lang)
{
    for (const auto& i : lang_description)
    {
        if (uint32(i.lang_id) == lang)
            return &i;
    }

    return nullptr;
}

bool IsLanguageSkill(uint32 Skill)
{
    for (int i = 0; i < LANGUAGES_COUNT; ++i)
    {
        if (lang_description[i].skill_id == Skill)
            return true;
    }
    return false;
}

template<typename T>
T IdGenerator<T>::Generate()
{
    if (m_nextGuid >= std::numeric_limits<T>::max() - 1)
    {
        sLog.outError("%s guid overflow!! Can't continue, shutting down server. ", m_name);
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_nextGuid++;
}

template uint32 IdGenerator<uint32>::Generate();
template uint64 IdGenerator<uint64>::Generate();

ObjectMgr::ObjectMgr() :
    m_GuildIds("Guild ids"),
    m_MailIds("Mail ids"),
    m_GroupIds("Group ids"),
    m_PetitionIds("Petition ids"),
    m_FirstTemporaryCreatureGuid(1),
    m_FirstTemporaryGameObjectGuid(1),
    // Nostalrius
    DBCLocaleIndex(0),
    m_OldMailCounter(0)
{}

ObjectMgr::~ObjectMgr()
{
    for (auto& i : m_PetInfoMap)
        delete[] i.second;

    // free only if loaded
    for (auto& class_ : m_PlayerClassInfo)
        delete[] class_.levelInfo;

    for (auto& race : m_PlayerInfo)
        for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
            delete[] race[class_].levelInfo;

    // free objects
    for (auto& itr : m_GroupMap)
        delete itr.second;

    for (auto& itr : m_CacheVendorTemplateItemMap)
        itr.second.Clear();

    for (auto& itr : m_CacheVendorItemMap)
        itr.second.Clear();

    for (auto& itr : m_CacheTrainerSpellMap)
        itr.second.Clear();

    for (auto& itr : m_playerCacheData)
        delete itr.second;

    for (auto& itr : m_itemTransmogs)
        delete itr.second;
}

void ObjectMgr::LoadAllIdentifiers()
{
    m_ItemIdSet.clear();

    Field* fields;
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT DISTINCT `entry` FROM `item_template`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_ItemIdSet.insert(id);
        } while (result->NextRow());
    }
    
    m_QuestIdSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `entry` FROM `quest_template`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_QuestIdSet.insert(id);
        } while (result->NextRow());
    }

    m_CreatureIdSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `entry` FROM `creature_template`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_CreatureIdSet.insert(id);
        } while (result->NextRow());
    }

    m_GameObjectIdSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `entry` FROM `gameobject_template`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_GameObjectIdSet.insert(id);
        } while (result->NextRow());
    }

    m_CreatureGuidSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `guid` FROM `creature`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_CreatureGuidSet.insert(id);
        } while (result->NextRow());
    }

    m_GameObjectGuidSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `guid` FROM `gameobject`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_GameObjectGuidSet.insert(id);
        } while (result->NextRow());
    }

    m_AreaTriggerIdSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `id` FROM `areatrigger_template`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_AreaTriggerIdSet.insert(id);
        } while (result->NextRow());
    }

    m_CreatureSpellsIdSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `entry` FROM `creature_spells`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_CreatureSpellsIdSet.insert(id);
        } while (result->NextRow());
    }

    m_VendorTemplateIdSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `entry` FROM `npc_vendor_template`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_VendorTemplateIdSet.insert(id);
        } while (result->NextRow());
    }

    m_GossipMenuIdSet.clear();
    result.reset(WorldDatabase.Query("SELECT DISTINCT `entry` FROM `gossip_menu`"));

    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            m_GossipMenuIdSet.insert(id);
        } while (result->NextRow());
    }

    sSpellMgr.LoadExistingSpellIds();
}

// Nostalrius
void ObjectMgr::LoadSpellDisabledEntrys()
{
    m_DisabledSpells.clear();                                // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry` FROM `spell_disabled`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 spellid = fields[0].GetUInt32();
        if (!sSpellMgr.GetSpellEntry(spellid))
        {
            if (!sSpellMgr.IsExistingSpellId(spellid))
                sLog.outError("Spell entry %u from `spell_disabled` doesn't exist, ignoring.", spellid);
            continue;
        }
        m_DisabledSpells.insert(spellid);
    }
    while (result->NextRow());
}

void ObjectMgr::LoadMapLootDisabled()
{
    m_DisabledMapLoots.clear();                                // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `map_id` FROM `map_loot_disabled`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 mapid = fields[0].GetUInt32();
        m_DisabledMapLoots.insert(mapid);
    }
    while (result->NextRow());
}

void ObjectMgr::LoadCinematicsWaypoints()
{
    m_CinematicWaypoints.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `cinematic`, `timer`, `position_x`, `position_y`, `position_z` FROM `cinematic_waypoints`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        CinematicWaypointEntry tmp;

        tmp.cinematic_id = fields[0].GetUInt32();
        tmp.time         = fields[1].GetUInt32();
        tmp.position.x   = fields[2].GetFloat();
        tmp.position.y   = fields[3].GetFloat();
        tmp.position.z   = fields[4].GetFloat();

        m_CinematicWaypoints.push_back(tmp);
    }
    while (result->NextRow());
}

Position const* ObjectMgr::GetCinematicPosition(uint32 cinematicId, uint32 elapsed_time)
{
    // We are looking for a waypoint :
    // 1- corresponds to the cinematic_id
    // 2- biggest possible
    // 3- With a time < elapsed_time (otherwise we always stay at the same)
    Position const* tmpBestPos = nullptr;
    uint32 tmpBestTimer  = 0;
    std::vector<CinematicWaypointEntry>::const_iterator it;
    for (it = m_CinematicWaypoints.begin(); it != m_CinematicWaypoints.end(); ++it)
    {
        if (cinematicId == it->cinematic_id)
        {
            if (it->time < elapsed_time)
            {
                if (it->time > tmpBestTimer)
                {
                    tmpBestTimer = it->time;
                    tmpBestPos   = &(it->position);
                }
            }
        }
    }
    return tmpBestPos;
}

Position const* ObjectMgr::GetCinematicInitialPosition(uint32 cinematicId)
{
    std::vector<CinematicWaypointEntry>::const_iterator it;
    for (it = m_CinematicWaypoints.begin(); it != m_CinematicWaypoints.end(); ++it)
    {
        if (cinematicId == it->cinematic_id)
        {
            if (it->time == 0)
                return &(it->position);
        }
    }
    // Not found
    //sLog.outError("Can not find the starting point of cinematic %u", cinematicId);
    return nullptr;
}

// Phasing system
// ALTER TABLE characters ADD COLUMN world_phase_mask int(11) unsigned not null default 0;
void ObjectMgr::LoadPlayerPhaseFromDb()
{
    m_PlayerPhases.clear();

    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT guid, world_phase_mask FROM characters"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 guid = fields[0].GetUInt32();
        uint32 mask = fields[1].GetUInt32();
        m_PlayerPhases[guid] = mask;
    }
    while (result->NextRow());
}

uint32 ObjectMgr::GetPlayerWorldMaskByGUID(const uint64 guid)
{
    return m_PlayerPhases[GUID_LOPART(guid)];
}
void ObjectMgr::SetPlayerWorldMask(const uint64 guid, uint32 newWorldMask)
{
    if (m_PlayerPhases[GUID_LOPART(guid)] == newWorldMask)
        return;

    m_PlayerPhases[GUID_LOPART(guid)] = newWorldMask;
}

uint32 ObjectMgr::GetSavedVariable(uint32 index, uint32 defaultValue, bool *exist)
{
    auto itr = m_SavedVariables.find(index);

    if (itr == m_SavedVariables.end())
    {
        if (exist)
            *exist = false;
        return defaultValue;
    }

    if (exist)
        *exist = true;

    return itr->second.uiValue;
}

SavedVariable& ObjectMgr::_InsertVariable(uint32 index, uint32 value, bool saved)
{
    SavedVariable tmp;
    tmp.uiIndex      = index;
    tmp.uiValue      = value;
    tmp.bSavedInDb   = saved;

    m_SavedVariables[index] = tmp;
    return m_SavedVariables[index];
}

void ObjectMgr::_SaveVariable(const SavedVariable& toSave)
{
    // Must do this in a transaction, else if worker threads > 1 we could do one before the other
    // when order is important...
    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM `variables` WHERE `index` = %u", toSave.uiIndex);
    CharacterDatabase.PExecute("INSERT INTO `variables` (`index`, `value`) VALUES (%u, %u)", toSave.uiIndex, toSave.uiValue);
    CharacterDatabase.CommitTransaction();
}

void ObjectMgr::InitSavedVariable(uint32 index, uint32 value)
{
    auto itr = m_SavedVariables.find(index);

    if (itr != m_SavedVariables.end())
        return;
    
    // If we are there, it means that the variable does not exist.
    SavedVariable& variable = _InsertVariable(index, value, true);
    _SaveVariable(variable);
}

void ObjectMgr::SetSavedVariable(uint32 index, uint32 value, bool autoSave)
{
    auto itr = m_SavedVariables.find(index);

    if (itr != m_SavedVariables.end())
    {
        // If the value has not changed.
        if (itr->second.uiValue == value)
            return;

        itr->second.uiValue = value;
        if (autoSave)
            _SaveVariable(itr->second);
        else
            itr->second.bSavedInDb = false;
        return;
    }

    // If we are here, it means that the variable does not exist.
    SavedVariable& variable = _InsertVariable(index, value, autoSave);
    if (autoSave)
        _SaveVariable(variable);
}

void ObjectMgr::LoadVariable(uint32 index, uint32* variable, uint32 defaultValue, uint32 maxValue, uint32 minValue)
{
    bool inIndex = false;
    (*variable) = GetSavedVariable(index, defaultValue, &inIndex);
    uint32 originalValue = (*variable);
    if (maxValue != 0 && (*variable) > maxValue)
        (*variable) = defaultValue;
    if ((*variable) < minValue)
        (*variable) = defaultValue;
    if (!inIndex)
        _InsertVariable(index, (*variable), true);
    if (originalValue != (*variable))
        SetSavedVariable(index, (*variable), true);
}
void ObjectMgr::SaveVariables()
{
    for (auto it = m_SavedVariables.begin(); it != m_SavedVariables.end(); ++it)
    {
        if (!it->second.bSavedInDb)
            _SaveVariable(it->second);
    }
}

void ObjectMgr::LoadSavedVariable()
{
    m_SavedVariables.clear();

    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `index`, `value` FROM `variables`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        _InsertVariable(fields[0].GetUInt32(), fields[1].GetUInt32(), true);
    }
    while (result->NextRow());
}

// Caching player data
void ObjectMgr::LoadPlayerCacheData(uint32 lowGuid)
{
    std::unique_ptr<QueryResult> result;
    if (!lowGuid)
    {
        // load all characters (likely at startup)
        m_playerCacheData.clear();
        m_accountPlayersCacheData.clear();
        m_playerNameToGuid.clear();

        result.reset(CharacterDatabase.Query(
            //       0       1       2        3         4          5       6        7       8      9             10            11            12             13           14
            "SELECT `guid`, `race`, `class`, `gender`, `account`, `name`, `level`, `zone`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `taxi_path`, `mortality_status` FROM `characters`;"));
    }
    else
    {
        // load a single character (likely just restored)
        result.reset(CharacterDatabase.PQuery(
            //       0       1       2        3         4          5       6        7       8      9             10            11            12             13           14
            "SELECT `guid`, `race`, `class`, `gender`, `account`, `name`, `level`, `zone`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `taxi_path`, `mortality_status` FROM `characters` WHERE `guid`=%u;", lowGuid));
    }
    
    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        // guid, race, class, gender, account, name
        std::string name = fields[5].GetCppString();
        if (normalizePlayerName(name))
        {
            PlayerCacheData* data = InsertPlayerInCache(fields[0].GetUInt32(), fields[1].GetUInt32(), fields[2].GetUInt32(),
                fields[3].GetUInt32(), fields[4].GetUInt32(), name, fields[6].GetUInt32(), fields[7].GetUInt32(), fields[14].GetUInt8());

            UpdatePlayerCachedPosition(data, fields[8].GetUInt32(), fields[9].GetFloat(), fields[10].GetFloat(),
                fields[11].GetFloat(), fields[12].GetFloat(), !fields[13].GetCppString().empty());
        }
    }
    while (result->NextRow());
}

PlayerCacheData* ObjectMgr::GetPlayerDataByGUID(uint32 guidLow) const
{
    auto itr = m_playerCacheData.find(guidLow);
    if (itr != m_playerCacheData.end())
        return itr->second;
    return nullptr;
}

PlayerCacheData* ObjectMgr::GetPlayerDataByName(std::string const& name) const
{
    if (ObjectGuid guid = GetPlayerGuidByName(name))
        return GetPlayerDataByGUID(guid.GetCounter());
    return nullptr;
}

ObjectGuid ObjectMgr::GetPlayerGuidByName(std::string const& name) const
{
    auto itr = m_playerNameToGuid.find(name);
    if (itr != m_playerNameToGuid.end())
        return ObjectGuid(HIGHGUID_PLAYER, itr->second);
    return ObjectGuid();
}

bool ObjectMgr::GetPlayerNameByGUID(ObjectGuid guid, std::string &name) const
{
    if (PlayerCacheData* pData = sObjectMgr.GetPlayerDataByGUID(guid.GetCounter()))
    {
        name = pData->sName;
        return true;
    }
    return false;
}

Team ObjectMgr::GetPlayerTeamByGUID(ObjectGuid guid) const
{
    if (Player* player = GetPlayer(guid))
        return Player::TeamForRace(player->GetRace());

    if (PlayerCacheData* pData = sObjectMgr.GetPlayerDataByGUID(guid.GetCounter()))
        return Player::TeamForRace(pData->uiRace);

    return TEAM_NONE;
}

uint8 ObjectMgr::GetPlayerClassByGUID(ObjectGuid guid) const
{
    // Prevent DB access for online player.
    if (Player* player = GetPlayer(guid))
    {
        return player->GetClass();
    }

    uint32 lowguid = guid.GetCounter();

    if (PlayerCacheData* data = GetPlayerDataByGUID(lowguid))
    {
        return data->uiClass;
    }
    else
    {
        std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT `class` FROM `characters` WHERE `guid` = '%u'", lowguid));

        if (result)
        {
            uint8 pClass = (*result)[0].GetUInt8();
            return pClass;
        }
    }
    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByGUID(ObjectGuid guid) const
{
    if (Player* player = GetPlayer(guid))
        return player->GetSession()->GetAccountId();

    if (PlayerCacheData* pData = sObjectMgr.GetPlayerDataByGUID(guid.GetCounter()))
        return pData->uiAccount;

    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByPlayerName(std::string const& name) const
{
    if (PlayerCacheData* pData = sObjectMgr.GetPlayerDataByName(name))
        return pData->uiAccount;
    return 0;
}

PlayerCacheData* ObjectMgr::InsertPlayerInCache(Player *pPlayer)
{
    WorldSession* pSession = pPlayer->GetSession();
    if (!pSession)
        return nullptr;
    uint32 accountId = pSession->GetAccountId();

    return InsertPlayerInCache(pPlayer->GetGUIDLow(), pPlayer->GetRace(), pPlayer->GetClass(), pPlayer->GetGender(), accountId, pPlayer->GetName(), pPlayer->GetLevel(), pPlayer->GetCachedZoneId(), pPlayer->GetHardcoreStatus());
}

void ObjectMgr::UpdatePlayerCachedPosition(Player *pPlayer)
{
    auto iter = m_playerCacheData.find(pPlayer->GetGUIDLow());
    PlayerCacheData* data = nullptr;
    if (iter == m_playerCacheData.end())
        data = InsertPlayerInCache(pPlayer);
    else
        data = iter->second;

    if (!data)
        return;

    UpdatePlayerCachedPosition(data, pPlayer->GetMapId(), pPlayer->GetPositionX(), pPlayer->GetPositionY(),
        pPlayer->GetPositionZ(), pPlayer->GetOrientation(), pPlayer->IsTaxiFlying());
}

void ObjectMgr::UpdatePlayerCachedPosition(uint32 lowGuid, uint32 mapId, float posX, float posY, float posZ, float o, bool inFlight)
{
    auto iter = m_playerCacheData.find(lowGuid);
    if (iter == m_playerCacheData.end())
        return;

    UpdatePlayerCachedPosition(iter->second, mapId, posX, posY, posZ, o, inFlight);
}

void ObjectMgr::UpdatePlayerCachedPosition(PlayerCacheData* data, uint32 mapId, float posX, float posY, float posZ, float o, bool inFlight)
{
    data->uiMapId = mapId;
    data->fPosX = posX;
    data->fPosY = posY;
    data->fPosZ = posZ;
    data->fOrientation = o;
    data->bInFlight = inFlight;
}

void ObjectMgr::UpdatePlayerCache(Player* pPlayer)
{
    auto iter = m_playerCacheData.find(pPlayer->GetGUIDLow());
    PlayerCacheData* data = nullptr;
    if (iter == m_playerCacheData.end())
        data = InsertPlayerInCache(pPlayer);
    else
        data = iter->second;

    if (!data)
        return;
    if (pPlayer->GetSession())
        UpdatePlayerCache(data, pPlayer->GetRace(), pPlayer->GetClass(), pPlayer->GetGender(), pPlayer->GetSession()->GetAccountId(), pPlayer->GetName(), pPlayer->GetLevel(), pPlayer->GetCachedZoneId(), pPlayer->GetHardcoreStatus());

    UpdatePlayerCachedPosition(data, pPlayer->GetMapId(), pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), pPlayer->IsTaxiFlying());
}

void ObjectMgr::UpdatePlayerCache(PlayerCacheData* data, uint32 race, uint32 _class, uint32 gender, uint32 accountId, std::string const& name, uint32 level, uint32 zoneId, uint8 hardcoreStatus)
{
    data->uiAccount = accountId;
    data->uiRace = race;
    data->uiClass = _class;
    data->uiGender = gender;
    data->uiLevel = level;
    data->sName = name;
    data->uiZoneId = zoneId;
    data->uiHardcoreStatus = hardcoreStatus;
}

PlayerCacheData* ObjectMgr::InsertPlayerInCache(uint32 lowGuid, uint32 race, uint32 _class, uint32 gender, uint32 accountId, std::string const& name, uint32 level, uint32 zoneId, uint8 hardcoreStatus)
{
    PlayerCacheData* data = new PlayerCacheData;
    data->uiGuid = lowGuid;
    UpdatePlayerCache(data, race, _class, gender, accountId, name, level, zoneId, hardcoreStatus);

    m_playerCacheData[lowGuid] = data;
    m_playerNameToGuid[name] = lowGuid;

    std::set<PlayerCacheData*>& AccountChars = m_accountPlayersCacheData[accountId];
    AccountChars.insert(data);

    return data;
}

void ObjectMgr::DeletePlayerFromCache(uint32 lowGuid)
{
    auto itr = m_playerCacheData.find(lowGuid);
    if (itr != m_playerCacheData.end())
    {
        uint32 AccId = itr->second->uiAccount;
        m_accountPlayersCacheData[AccId].erase(itr->second);
        auto itr2 = m_playerNameToGuid.find(itr->second->sName);
        if (itr2 != m_playerNameToGuid.end())
            m_playerNameToGuid.erase(itr2);
        delete itr->second;
        m_playerCacheData.erase(itr);
    }
}

void ObjectMgr::DeletePlayerNameFromCache(std::string const& name)
{
    auto itr = m_playerNameToGuid.find(name);
    if (itr != m_playerNameToGuid.end())
        m_playerNameToGuid.erase(itr);
}

void ObjectMgr::ChangePlayerNameInCache(uint32 guidLow, std::string const& oldName, std::string const& newName)
{
    auto itr = m_playerCacheData.find(guidLow);
    if (itr != m_playerCacheData.end())
    {
        m_playerNameToGuid.erase(oldName);
        m_playerNameToGuid[newName] = guidLow;
        itr->second->sName = newName;
    }
}

void ObjectMgr::GetPlayerDataForAccount(uint32 accountId, std::vector<PlayerCacheData*>& data) const
{
    auto AccPlayerCacheIter = m_accountPlayersCacheData.find(accountId);
    if (AccPlayerCacheIter != m_accountPlayersCacheData.end())
    {
        const std::set<PlayerCacheData*>& PlayersInAcc = AccPlayerCacheIter->second;

        for (PlayerCacheData* pPlayerCache : PlayersInAcc)
        {
            data.push_back(pPlayerCache);
        }
    }
}

void ObjectMgr::LoadActivePlayersPerFaction()
{
    m_ActivePlayersPerFaction.clear();                              // need for reload case

    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT `race` FROM `characters` WHERE `level` > 10 && `logout_time` > %u", (time(nullptr) - MONTH)));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 race = fields[0].GetUInt32();
        Team team = Player::TeamForRace(race);
        m_ActivePlayersPerFaction[team]++;

    } while (result->NextRow());
}

bool ObjectMgr::IsFactionImbalanced(Team team)
{
    if (!sWorld.getConfig(CONFIG_BOOL_ENABLE_FACTION_BALANCE))
        return false;
    Team const oppositeTeam = team == ALLIANCE ? HORDE : ALLIANCE;
    float const maxImbalance = sWorld.getConfig(CONFIG_FLOAT_MAX_FACTION_IMBALANCE) + 1.0f;
    return m_ActivePlayersPerFaction[team] > uint32(m_ActivePlayersPerFaction[oppositeTeam] * maxImbalance);
}

Group* ObjectMgr::GetGroupById(uint32 id) const
{
    GroupMap::const_iterator itr = m_GroupMap.find(id);
    if (itr != m_GroupMap.end())
        return itr->second;

    return nullptr;
}

Group* ObjectMgr::GetGroupByMember(ObjectGuid memberGuid)
{
    for (auto const& itr : m_GroupMap)
        if (itr.second->IsMember(memberGuid))
            return itr.second;
    return nullptr;
}

void ObjectMgr::LoadCreatureLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_CreatureLocaleMap.clear();                              // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `name_loc1`, `subname_loc1`, `name_loc2`, `subname_loc2`, `name_loc3`, `subname_loc3`, `name_loc4`, `subname_loc4`, `name_loc5`, `subname_loc5`, `name_loc6`, `subname_loc6`, `name_loc7`, `subname_loc7`, `name_loc8`, `subname_loc8` FROM `locales_creature`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!GetCreatureTemplate(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_creature` has data for not existed creature entry %u, skipped.", entry);
            continue;
        }

        CreatureLocale& data = m_CreatureLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }
            str = fields[1 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.SubName.size() <= idx)
                        data.SubName.resize(idx + 1);

                    data.SubName[idx] = str;
                }
            }
        }
    }
    while (result->NextRow());
}

void ObjectMgr::LoadGossipMenuItemsLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_GossipMenuItemsLocaleMap.clear();                      // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `menu_id`, `id`,"
                          "`option_text_loc1`, `box_text_loc1`, `option_text_loc2`, `box_text_loc2`,"
                          "`option_text_loc3`, `box_text_loc3`, `option_text_loc4`, `box_text_loc4`,"
                          "`option_text_loc5`, `box_text_loc5`, `option_text_loc6`, `box_text_loc6`,"
                          "`option_text_loc7`, `box_text_loc7`, `option_text_loc8`, `box_text_loc8` "
                          "FROM `locales_gossip_menu_option`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint16 menuId   = fields[0].GetUInt16();
        uint16 id       = fields[1].GetUInt16();

        GossipMenuItemsMapBounds bounds = GetGossipMenuItemsMapBounds(menuId);

        bool found = false;
        if (bounds.first != bounds.second)
        {
            for (GossipMenuItemsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
            {
                if (itr->second.id == id)
                {
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            ERROR_DB_STRICT_LOG("Table `locales_gossip_menu_option` has data for nonexistent gossip menu %u item %u, skipped.", menuId, id);
            continue;
        }

        GossipMenuItemsLocale& data = m_GossipMenuItemsLocaleMap[MAKE_PAIR32(menuId, id)];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[2 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.OptionText.size() <= idx)
                        data.OptionText.resize(idx + 1);

                    data.OptionText[idx] = str;
                }
            }
            str = fields[2 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.BoxText.size() <= idx)
                        data.BoxText.resize(idx + 1);

                    data.BoxText[idx] = str;
                }
            }
        }
    }
    while (result->NextRow());
}

void ObjectMgr::LoadPointOfInterestLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_PointOfInterestLocaleMap.clear();                              // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `icon_name_loc1`, `icon_name_loc2`, `icon_name_loc3`, `icon_name_loc4`, `icon_name_loc5`, `icon_name_loc6`, `icon_name_loc7`, `icon_name_loc8` FROM `locales_points_of_interest`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!GetPointOfInterest(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_points_of_interest` has data for nonexistent POI entry %u, skipped.", entry);
            continue;
        }

        PointOfInterestLocale& data = m_PointOfInterestLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (str.empty())
                continue;

            int idx = GetOrNewIndexForLocale(LocaleConstant(i));
            if (idx >= 0)
            {
                if (data.IconName.size() <= size_t(idx))
                    data.IconName.resize(idx + 1);

                data.IconName[idx] = str;
            }
        }
    }
    while (result->NextRow());
}

void ObjectMgr::LoadCreatureTemplates()
{
    //                                                               0        1              2              3              4              5                   6       7          8                 9            10           11            12            13          14          15       16         17           18            19           20       21                 22                     23             24      25               26         27         28            29              30                31                  32                    33            34            35               36              37              38               39               40              41                42                43                     44      45            46         47                    48                  49          50          51            52           53            54            55           56           57           58           59               60                   61                62       63          64          65         66               67              68          69               70              71              72            73           74                      75                    76                77             78                79
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `display_id1`, `display_id2`, `display_id3`, `display_id4`, `mount_display_id`, `name`, `subname`, `gossip_menu_id`, `level_min`, `level_max`, `health_min`, `health_max`, `mana_min`, `mana_max`, `armor`, `faction`, `npc_flags`, `speed_walk`, `speed_run`, `scale`, `detection_range`, `call_for_help_range`, `leash_range`, `rank`, `xp_multiplier`, `dmg_min`, `dmg_max`, `dmg_school`, `attack_power`, `dmg_multiplier`, `base_attack_time`, `ranged_attack_time`, `unit_class`, `unit_flags`, `dynamic_flags`, `beast_family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `ranged_dmg_min`, `ranged_dmg_max`, `ranged_attack_power`, `type`, `type_flags`, `loot_id`, `pickpocket_loot_id`, `skinning_loot_id`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `spell_id1`, `spell_id2`, `spell_id3`, `spell_id4`, `spell_list_id`, `pet_spell_list_id`, `spawn_spell_id`, `auras`, `gold_min`, `gold_max`, `ai_name`, `movement_type`, `inhabit_type`, `civilian`, `racial_leader`, `regeneration`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `school_immune_mask`, `immunity_flags`, `flags_extra`, `phase_quest_id`, `script_name` FROM `creature_template`"));

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        LoadCreatureInfo(fields);
        
    } while (result->NextRow());
}

void ObjectMgr::LoadCreatureTemplate(uint32 entry)
{
    //                                                                0        1              2              3              4              5                   6       7          8                 9            10           11            12            13          14          15       16         17           18            19           20       21                 22                     23             24      25               26         27         28            29              30                31                  32                    33            34            35               36              37              38               39               40              41                42                43                     44      45            46         47                    48                  49          50          51            52           53            54            55           56           57           58           59               60                   61                62       63          64          65         66               67              68          69               70              71              72            73           74                      75                    76                77             78                79
    std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT `entry`, `display_id1`, `display_id2`, `display_id3`, `display_id4`, `mount_display_id`, `name`, `subname`, `gossip_menu_id`, `level_min`, `level_max`, `health_min`, `health_max`, `mana_min`, `mana_max`, `armor`, `faction`, `npc_flags`, `speed_walk`, `speed_run`, `scale`, `detection_range`, `call_for_help_range`, `leash_range`, `rank`, `xp_multiplier`, `dmg_min`, `dmg_max`, `dmg_school`, `attack_power`, `dmg_multiplier`, `base_attack_time`, `ranged_attack_time`, `unit_class`, `unit_flags`, `dynamic_flags`, `beast_family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `ranged_dmg_min`, `ranged_dmg_max`, `ranged_attack_power`, `type`, `type_flags`, `loot_id`, `pickpocket_loot_id`, `skinning_loot_id`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `spell_id1`, `spell_id2`, `spell_id3`, `spell_id4`, `spell_list_id`, `pet_spell_list_id`, `spawn_spell_id`, `auras`, `gold_min`, `gold_max`, `ai_name`, `movement_type`, `inhabit_type`, `civilian`, `racial_leader`, `regeneration`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `school_immune_mask`, `immunity_flags`, `flags_extra`, `phase_quest_id`, `script_name` FROM `creature_template` WHERE `entry`=%u", entry));

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        LoadCreatureInfo(fields);

    } while (result->NextRow());
}

void ObjectMgr::LoadCreatureInfo(Field* fields)
{
    uint32 entry = fields[0].GetUInt32();
    std::unique_ptr<CreatureInfo>& pInfo = m_creatureInfoMap[entry];
    if (!pInfo)
        pInfo = std::make_unique<CreatureInfo>();

    pInfo->entry = entry;
    pInfo->display_id[0] = fields[1].GetUInt32();
    pInfo->display_id[1] = fields[2].GetUInt32();
    pInfo->display_id[2] = fields[3].GetUInt32();
    pInfo->display_id[3] = fields[4].GetUInt32();
    pInfo->mount_display_id = fields[5].GetUInt32();
    pInfo->name = fields[6].GetCppString();
    pInfo->subname = fields[7].GetCppString();
    pInfo->gossip_menu_id = fields[8].GetUInt32();
    pInfo->level_min = fields[9].GetUInt32();
    pInfo->level_max = fields[10].GetUInt32();
    pInfo->health_min = fields[11].GetUInt32();
    pInfo->health_max = fields[12].GetUInt32();
    pInfo->mana_min = fields[13].GetUInt32();
    pInfo->mana_max = fields[14].GetUInt32();
    pInfo->armor = fields[15].GetUInt32();
    pInfo->faction = fields[16].GetUInt32();
    pInfo->npc_flags = fields[17].GetUInt32();
    pInfo->speed_walk = fields[18].GetFloat();
    pInfo->speed_run = fields[19].GetFloat();
    pInfo->scale = fields[20].GetFloat();
    pInfo->detection_range = fields[21].GetFloat();
    pInfo->call_for_help_range = fields[22].GetFloat();
    pInfo->leash_range = fields[23].GetFloat();
    pInfo->rank = fields[24].GetUInt32();
    pInfo->xp_multiplier = fields[25].GetFloat();
    pInfo->dmg_min = fields[26].GetFloat();
    pInfo->dmg_max = fields[27].GetFloat();
    pInfo->dmg_school = fields[28].GetUInt32();
    pInfo->attack_power = fields[29].GetUInt32();
    pInfo->dmg_multiplier = fields[30].GetFloat();
    pInfo->base_attack_time = fields[31].GetUInt32();
    pInfo->ranged_attack_time = fields[32].GetUInt32();
    pInfo->unit_class = fields[33].GetUInt32();
    pInfo->unit_flags = fields[34].GetUInt32();
    pInfo->dynamic_flags = fields[35].GetUInt32();
    pInfo->beast_family = fields[36].GetUInt32();
    pInfo->trainer_type = fields[37].GetUInt32();
    pInfo->trainer_spell = fields[38].GetUInt32();
    pInfo->trainer_class = fields[39].GetUInt32();
    pInfo->trainer_race = fields[40].GetUInt32();
    pInfo->ranged_dmg_min = fields[41].GetFloat();
    pInfo->ranged_dmg_max = fields[42].GetFloat();
    pInfo->ranged_attack_power = fields[43].GetUInt32();
    pInfo->type = fields[44].GetUInt32();
    pInfo->type_flags = fields[45].GetUInt32();
    pInfo->loot_id = fields[46].GetUInt32();
    pInfo->pickpocket_loot_id = fields[47].GetUInt32();
    pInfo->skinning_loot_id = fields[48].GetUInt32();
    pInfo->holy_res = fields[49].GetInt32();
    pInfo->fire_res = fields[50].GetInt32();
    pInfo->nature_res = fields[51].GetInt32();
    pInfo->frost_res = fields[52].GetInt32();
    pInfo->shadow_res = fields[53].GetInt32();
    pInfo->arcane_res = fields[54].GetInt32();
    pInfo->spells[0] = fields[55].GetUInt32();
    pInfo->spells[1] = fields[56].GetUInt32();
    pInfo->spells[2] = fields[57].GetUInt32();
    pInfo->spells[3] = fields[58].GetUInt32();
    pInfo->spell_list_id = fields[59].GetUInt32();
    pInfo->pet_spell_list_id = fields[60].GetUInt32();
    pInfo->spawn_spell_id = fields[61].GetUInt32();
    delete[] pInfo->auras;
    pInfo->auras = (uint32*)(fields[62].GetString() ? mangos_strdup(fields[62].GetString()) : nullptr);
    pInfo->gold_min = fields[63].GetUInt32();
    pInfo->gold_max = fields[64].GetUInt32();
    pInfo->ai_name = fields[65].GetCppString();
    pInfo->movement_type = fields[66].GetUInt32();
    pInfo->inhabit_type = fields[67].GetUInt32();
    pInfo->civilian = fields[68].GetBool();
    pInfo->racial_leader = fields[69].GetBool();
    pInfo->regeneration = fields[70].GetUInt32();
    pInfo->equipment_id = fields[71].GetUInt32();
    pInfo->trainer_id = fields[72].GetUInt32();
    pInfo->vendor_id = fields[73].GetUInt32();
    pInfo->mechanic_immune_mask = fields[74].GetUInt32();
    pInfo->school_immune_mask = fields[75].GetUInt32();
    pInfo->immunity_flags = fields[76].GetUInt32();
    pInfo->flags_extra = fields[77].GetUInt32();
    pInfo->phase_quest_id = fields[78].GetUInt32();
    pInfo->script_id = sScriptMgr.GetScriptId(fields[79].GetString());
    CheckCreatureTemplate(pInfo.get());
}

template <class T>
void ConvertCreatureAurasField(T* addon, char const* table, char const* guidEntryStr, uint32 guidOrEntry)
{
    // Now add the auras, format "spell1 spell2 ..."
    char* p,* s;
    std::vector<int> val;
    s = p = (char*)reinterpret_cast<char const*>(addon->auras);
    if (p)
    {
        while (p[0] != 0)
        {
            ++p;
            if (p[0] == ' ')
            {
                val.push_back(atoi(s));
                s = ++p;
            }
        }
        if (p != s)
            val.push_back(atoi(s));

        // free char* loaded memory
        delete[](char*)reinterpret_cast<char const*>(addon->auras);
    }

    // empty list
    if (val.empty())
    {
        addon->auras = nullptr;
        return;
    }

    // replace by new structures array
    const_cast<uint32*&>(addon->auras) = new uint32[val.size() + 1];

    uint32 i = 0;
    for (int32 j : val)
    {
        uint32& cAura = const_cast<uint32&>(addon->auras[i]);
        cAura = uint32(j);

        SpellEntry const* AdditionalSpellInfo = sSpellMgr.GetSpellEntry(cAura);
        if (!AdditionalSpellInfo)
        {
            sLog.outErrorDb("Creature (%s: %u) has wrong spell %u defined in `auras` field in `%s`.", guidEntryStr, guidOrEntry, cAura, table);
            continue;
        }

        if (std::find(&addon->auras[0], &addon->auras[i], cAura) != &addon->auras[i])
        {
            sLog.outErrorDb("Creature (%s: %u) has duplicate spell %u defined in `auras` field in `%s`.", guidEntryStr, guidOrEntry, cAura, table);
            continue;
        }

        ++i;
    }

    // fill terminator element (after last added)
    const_cast<uint32&>(addon->auras[i]) = 0;
}

void ObjectMgr::CheckCreatureTemplate(CreatureInfo* cInfo)
{
    FactionTemplateEntry const* factionTemplate = GetFactionTemplateEntry(cInfo->faction);
    if (!factionTemplate)
        sLog.outErrorDb("Creature (Entry: %u) has nonexistent faction template (%u)", cInfo->entry, cInfo->faction);

    // used later for scale
    CreatureDisplayInfoEntry const* displayScaleEntry = nullptr;

    for (int i = 0; i < MAX_DISPLAY_IDS_PER_CREATURE; ++i)
    {
        if (cInfo->display_id[i])
        {
            CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(cInfo->display_id[i]);
            if (!displayEntry)
            {
                sLog.outErrorDb("Creature (Entry: %u) has nonexistent display_id%d (%u), can crash client", cInfo->entry, i + 1, cInfo->display_id[i]);
                sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `display_id%d`=0 WHERE `entry`=%u;", i + 1, cInfo->entry);
                cInfo->display_id[i] = 0;
            }
            else if (!displayScaleEntry)
                displayScaleEntry = displayEntry;

            CreatureDisplayInfoAddon const* minfo = sCreatureDisplayInfoAddonStorage.LookupEntry<CreatureDisplayInfoAddon>(cInfo->display_id[i]);
            if (!minfo)
                sLog.outErrorDb("Creature (Entry: %u) is using display_id%d (%u), but creature_display_info_addon data is missing for this id.", cInfo->entry, i + 1, cInfo->display_id[i]);
        }
    }

    if (!displayScaleEntry)
        sLog.outErrorDb("Creature (Entry: %u) does not have any valid display id", cInfo->entry);

    if (cInfo->mount_display_id && !sCreatureDisplayInfoStore.LookupEntry(cInfo->mount_display_id))
    {
        sLog.outErrorDb("Creature (Entry: %u) has nonexistent mount_display_id (%u), can crash client", cInfo->entry, cInfo->mount_display_id);
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `mount_display_id`=0 WHERE `entry`=%u;", cInfo->entry);
        cInfo->mount_display_id = 0;
    }

    // use below code for 0-checks for unit_class
    if (!cInfo->unit_class)
        ERROR_DB_STRICT_LOG("Creature (Entry: %u) does not have proper unit_class(%u) for creature_template", cInfo->entry, cInfo->unit_class);
    else if (((1 << (cInfo->unit_class - 1)) & CLASSMASK_ALL_CREATURES) == 0)
        sLog.outErrorDb("Creature (Entry: %u) has invalid unit_class(%u) for creature_template", cInfo->entry, cInfo->unit_class);

    if (cInfo->dmg_school >= MAX_SPELL_SCHOOL)
    {
        sLog.outErrorDb("Creature (Entry: %u) has invalid spell school value (%u) in `dmg_school`", cInfo->entry, cInfo->dmg_school);
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `dmg_school`=%u WHERE entry=%u;", (uint32)SPELL_SCHOOL_NORMAL, cInfo->entry);
        cInfo->dmg_school = SPELL_SCHOOL_NORMAL;
    }

    if (cInfo->base_attack_time == 0)
    {
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `base_attack_time`=%u WHERE entry=%u;", BASE_ATTACK_TIME, cInfo->entry);
        cInfo->base_attack_time  = BASE_ATTACK_TIME;
    }

    if (cInfo->ranged_attack_time == 0)
    {
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `ranged_attack_time`=%u WHERE entry=%u;", BASE_ATTACK_TIME, cInfo->entry);
        cInfo->ranged_attack_time = BASE_ATTACK_TIME;
    }

    if ((cInfo->npc_flags & UNIT_NPC_FLAG_TRAINER) && cInfo->trainer_type >= MAX_TRAINER_TYPE)
        sLog.outErrorDb("Creature (Entry: %u) has wrong trainer type %u", cInfo->entry, cInfo->trainer_type);

    if (cInfo->type && !sCreatureTypeStore.LookupEntry(cInfo->type))
    {
        sLog.outErrorDb("Creature (Entry: %u) has invalid creature type (%u) in `type`", cInfo->entry, cInfo->type);
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `type`=%u WHERE entry=%u;", (uint32)CREATURE_TYPE_HUMANOID, cInfo->entry);
        cInfo->type = CREATURE_TYPE_HUMANOID;
    }

    // must exist or used hidden but used in data horse case
    if (cInfo->beast_family && !sCreatureFamilyStore.LookupEntry(cInfo->beast_family) && cInfo->beast_family != CREATURE_FAMILY_HORSE_CUSTOM)
    {
        sLog.outErrorDb("Creature (Entry: %u) has invalid creature family (%u) in `beast_family`", cInfo->entry, cInfo->beast_family);
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `beast_family`=%u WHERE entry=%u;", 0, cInfo->entry);
        cInfo->beast_family = 0;
    }

    if (cInfo->inhabit_type <= 0 || cInfo->inhabit_type > INHABIT_ANYWHERE)
    {
        sLog.outErrorDb("Creature (Entry: %u) has wrong value (%u) in `inhabit_type`, creature will not correctly walk/swim", cInfo->entry, cInfo->inhabit_type);
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `inhabit_type`=%u WHERE entry=%u;", (uint32)INHABIT_ANYWHERE, cInfo->entry);
        cInfo->inhabit_type = INHABIT_ANYWHERE;
    }

    if (cInfo->pet_spell_list_id)
    {
        CreatureSpellDataEntry const* spellDataId = sCreatureSpellDataStorage.LookupEntry<CreatureSpellDataEntry>(cInfo->pet_spell_list_id);
        if (!spellDataId)
            sLog.outErrorDb("Creature (Entry: %u) has nonexistent pet_spell_list_id (%u)", cInfo->entry, cInfo->pet_spell_list_id);
    }

    if (cInfo->spell_list_id)
    {
        CreatureSpellsList const* spellsList = GetCreatureSpellsList((cInfo->spell_list_id));
        if (!spellsList)
        {
            sLog.outErrorDb("Creature (Entry: %u) has nonexistent spell list id (%u)", cInfo->entry, cInfo->spell_list_id);
            cInfo->spell_list_id = 0;
        }
    }

    if (cInfo->spawn_spell_id)
    {
        SpellEntry const* pSpellEntry = sSpellMgr.GetSpellEntry(cInfo->spawn_spell_id);
        if (!pSpellEntry || !pSpellEntry->HasEffect(SPELL_EFFECT_SPAWN))
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid spawn_spell_id (%u), set to 0", cInfo->entry, cInfo->spawn_spell_id);
            sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `spawn_spell_id`=0 WHERE `entry`=%u;", cInfo->entry);
            cInfo->spawn_spell_id = 0;
        }
    }

    for (int j = 0; j < CREATURE_MAX_SPELLS; ++j)
    {
        if (cInfo->spells[j] && !sSpellMgr.GetSpellEntry(cInfo->spells[j]))
        {
            sLog.outErrorDb("Creature (Entry: %u) has nonexistent spell_id%d (%u), set to 0", cInfo->entry, j + 1, cInfo->spells[j]);
            sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `spell_id%u`=0 WHERE entry=%u;", j + 1, cInfo->entry);
            cInfo->spells[j] = 0;
        }
    }

    if (cInfo->movement_type >= MAX_DB_MOTION_TYPE)
    {
        sLog.outErrorDb("Creature (Entry: %u) has wrong movement generator type (%u), ignored and set to IDLE.", cInfo->entry, cInfo->movement_type);
        sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `movement_type`=%u WHERE entry=%u;", (uint32)IDLE_MOTION_TYPE, cInfo->entry);
        cInfo->movement_type = IDLE_MOTION_TYPE;
    }

    if (cInfo->equipment_id > 0)                         // 0 no equipment
    {
        if (!GetEquipmentInfo(cInfo->equipment_id))
        {
            sLog.outErrorDb("Table `creature_template` have creature (Entry: %u) with equipment_id %u not found in table `creature_equip_template`, set to no equipment.", cInfo->entry, cInfo->equipment_id);
            sLog.out(LOG_DBERRFIX, "UPDATE creature_template SET `equipment_id`=0 WHERE entry=%u;", cInfo->entry);
            cInfo->equipment_id = 0;
        }
    }

    if (cInfo->vendor_id > 0)
    {
        if (!(cInfo->npc_flags & UNIT_NPC_FLAG_VENDOR))
            sLog.outErrorDb("Table `creature_template` have creature (Entry: %u) with vendor_id %u but not have flag UNIT_NPC_FLAG_VENDOR (%u), vendor items will ignored.", cInfo->entry, cInfo->vendor_id, UNIT_NPC_FLAG_VENDOR);
    }

    /// if not set custom creature scale then load scale from CreatureDisplayInfo.dbc
    if (cInfo->scale <= 0.0f)
    {
        if (displayScaleEntry)
            cInfo->scale = displayScaleEntry->scale;
        else
            cInfo->scale = DEFAULT_OBJECT_SCALE;
    }

    if (cInfo->xp_multiplier < 0.0f)
    {
        sLog.outErrorDb("Creature (Entry: %u) has negative experience multiplier (%f)", cInfo->entry, cInfo->xp_multiplier);
        cInfo->xp_multiplier = 1.0f;
    }

    if (cInfo->detection_range < 0.0f)
    {
        sLog.outErrorDb("Creature (Entry: %u) has negative detection distance (%f)", cInfo->entry, cInfo->detection_range);
        cInfo->detection_range = 20.0f;
    }

    if (cInfo->call_for_help_range < 0.0f)
    {
        sLog.outErrorDb("Creature (Entry: %u) has negative call for help radius (%f)", cInfo->entry, cInfo->call_for_help_range);
        cInfo->call_for_help_range = 5.0f;
    }

    if (cInfo->leash_range)
    {
        if (cInfo->leash_range < 0.0f)
        {
            sLog.outErrorDb("Creature (Entry: %u) has negative leash distance (%f)", cInfo->entry, cInfo->leash_range);
            cInfo->leash_range = 0.0f;
        }
        else if (cInfo->leash_range < cInfo->detection_range)
        {
            sLog.outErrorDb("Creature (Entry: %u) has leash distance below detection distance (%f)", cInfo->entry, cInfo->leash_range);
            cInfo->leash_range = 0.0f;
        }
    }

    if (cInfo->flags_extra & CREATURE_FLAG_EXTRA_DESPAWN_INSTANTLY)
    {
        if (cInfo->gold_min || cInfo->gold_max)
            sLog.outErrorDb("Creature (Entry: %u) with despawn instantly flag has gold loot assigned. It will never be lootable.", cInfo->entry);

        if (cInfo->loot_id)
            sLog.outErrorDb("Creature (Entry: %u) with despawn instantly flag has corpse loot assigned. It will never be lootable.", cInfo->entry);

        if (cInfo->skinning_loot_id)
            sLog.outErrorDb("Creature (Entry: %u) with despawn instantly flag has skinning loot assigned. It will never be lootable.", cInfo->entry);
    }

    ConvertCreatureAurasField<CreatureInfo>(cInfo, "creature_template", "Entry", cInfo->entry);
}

void ObjectMgr::LoadCreatureAddons(SQLStorage& creatureaddons, char const* entryName, char const* comment)
{
    creatureaddons.Load();

    // check data correctness and convert 'auras'
    for (uint32 i = 1; i < creatureaddons.GetMaxEntry(); ++i)
    {
        CreatureDataAddon const* addon = creatureaddons.LookupEntry<CreatureDataAddon>(i);
        if (!addon)
            continue;

        if (addon->display_id > 0)
        {
            if (!sCreatureDisplayInfoStore.LookupEntry(addon->display_id))
            {
                sLog.outErrorDb("Creature (%s %u) have invalid display id (%u) defined in `%s`.", entryName, addon->guid, addon->display_id, creatureaddons.GetTableName());
                const_cast<CreatureDataAddon*>(addon)->display_id = 0;
            }
        }

        if (addon->mount_display_id > 0)
        {
            if (!sCreatureDisplayInfoStore.LookupEntry(addon->mount_display_id))
            {
                sLog.outErrorDb("Creature (%s %u) have invalid display id for mount (%u) defined in `%s`.", entryName, addon->guid, addon->mount_display_id, creatureaddons.GetTableName());
                const_cast<CreatureDataAddon*>(addon)->mount_display_id = 0;
            }
        }

        if (addon->equipment_id > 0)
        {
            if (!GetEquipmentInfo(addon->equipment_id))
            {
                sLog.outErrorDb("Creature (%s %u) have invalid equipment id (%i) defined in `%s`.", entryName, addon->guid, addon->equipment_id, creatureaddons.GetTableName());
                const_cast<CreatureDataAddon*>(addon)->equipment_id = -1;
            }
        }

        if (addon->stand_state >= MAX_UNIT_STAND_STATE)
        {
            sLog.outErrorDb("Creature (%s %u) has unknown stand state (%u) defined in `%s`.", entryName, addon->guid, addon->stand_state, creatureaddons.GetTableName());
            const_cast<CreatureDataAddon*>(addon)->stand_state = UNIT_STAND_STATE_STAND;
        }

        if (addon->sheath_state > SHEATH_STATE_RANGED)
        {
            sLog.outErrorDb("Creature (%s %u) has unknown sheath state (%u) defined in `%s`.", entryName, addon->guid, addon->sheath_state, creatureaddons.GetTableName());
            const_cast<CreatureDataAddon*>(addon)->sheath_state = SHEATH_STATE_MELEE;
        }

        if (!sEmotesStore.LookupEntry(addon->emote_state))
        {
            sLog.outErrorDb("Creature (%s %u) have invalid emote (%u) defined in `%s`.", entryName, addon->guid, addon->emote_state, creatureaddons.GetTableName());
            const_cast<CreatureDataAddon*>(addon)->emote_state = 0;
        }

        ConvertCreatureAurasField<CreatureDataAddon>(const_cast<CreatureDataAddon*>(addon), creatureaddons.GetTableName(), entryName, addon->guid);
    }
}

void ObjectMgr::LoadCreatureAddons()
{
    LoadCreatureAddons(sCreatureDataAddonStorage, "GUID", "creature addons");

    // check entry ids
    for (uint32 i = 1; i < sCreatureDataAddonStorage.GetMaxEntry(); ++i)
        if (CreatureDataAddon const* addon = sCreatureDataAddonStorage.LookupEntry<CreatureDataAddon>(i))
            if (m_CreatureDataMap.find(addon->guid) == m_CreatureDataMap.end())
                if (!sObjectMgr.IsExistingCreatureGuid(addon->guid))
                    sLog.outErrorDb("Creature (GUID: %u) does not exist but has a record in `creature_addon`", addon->guid);
}

EquipmentInfo const* ObjectMgr::GetEquipmentInfo(uint32 entry)
{
    return sEquipmentStorage.LookupEntry<EquipmentInfo>(entry);
}

void ObjectMgr::LoadEquipmentTemplates()
{
    sEquipmentStorage.Load(true);

    for (uint32 i = 0; i < sEquipmentStorage.GetMaxEntry(); ++i)
    {
        EquipmentInfo const* eqInfo = sEquipmentStorage.LookupEntry<EquipmentInfo>(i);

        if (!eqInfo)
            continue;

        for (uint8 j = 0; j < 3; ++j)
        {
            if (!eqInfo->equipentry[j])
                continue;

            ItemPrototype const *itemProto = GetItemPrototype(eqInfo->equipentry[j]);
            if (!itemProto)
            {
                sLog.outErrorDb("Unknown item (entry=%u) in creature_equip_template.equipentry%u for entry = %u, forced to 0.", eqInfo->equipentry[j], j + 1, i);
                const_cast<EquipmentInfo*>(eqInfo)->equipentry[j] = 0;
                continue;
            }

            if (itemProto->InventoryType != INVTYPE_WEAPON &&
                    itemProto->InventoryType != INVTYPE_SHIELD &&
                    itemProto->InventoryType != INVTYPE_RANGED &&
                    itemProto->InventoryType != INVTYPE_2HWEAPON &&
                    itemProto->InventoryType != INVTYPE_WEAPONMAINHAND &&
                    itemProto->InventoryType != INVTYPE_WEAPONOFFHAND &&
                    itemProto->InventoryType != INVTYPE_HOLDABLE &&
                    itemProto->InventoryType != INVTYPE_THROWN &&
                    itemProto->InventoryType != INVTYPE_RANGEDRIGHT &&
                    itemProto->InventoryType != INVTYPE_RELIC)
            {
                sLog.outErrorDb("Item (entry=%u) in creature_equip_template.equipentry%u for entry = %u is not equipable in a hand, forced to 0.", eqInfo->equipentry[j], j + 1, i);
                const_cast<EquipmentInfo*>(eqInfo)->equipentry[j] = 0;
            }
        }
    }
}

CreatureDisplayInfoAddon const* ObjectMgr::GetCreatureDisplayInfoAddon(uint32 display_id)
{
    return sCreatureDisplayInfoAddonStorage.LookupEntry<CreatureDisplayInfoAddon>(display_id);
}

CreatureDisplayInfoAddon const* ObjectMgr::GetCreatureDisplayInfoRandomGender(uint32 display_id)
{
    CreatureDisplayInfoAddon const* minfo = GetCreatureDisplayInfoAddon(display_id);
    if (!minfo)
        return nullptr;

    // If a display id for another gender exists, 50% chance to use it
    if (minfo->display_id_other_gender != 0 && urand(0, 1) == 0)
    {
        CreatureDisplayInfoAddon const* minfo_tmp = GetCreatureDisplayInfoAddon(minfo->display_id_other_gender);
        if (!minfo_tmp)
        {
            sLog.outErrorDb("Model (Entry: %u) has display_id_other_gender %u not found in table `creature_display_info_addon`. ", minfo->display_id, minfo->display_id_other_gender);
            return minfo;                                   // not fatal, just use the previous one
        }
        else
            return minfo_tmp;
    }
    else
        return minfo;
}

void ObjectMgr::LoadCreatureDisplayInfoAddon()
{
    sCreatureDisplayInfoAddonStorage.Load();

    // post processing
    for (uint32 i = 1; i < sCreatureDisplayInfoAddonStorage.GetMaxEntry(); ++i)
    {
        CreatureDisplayInfoAddon const* minfo = sCreatureDisplayInfoAddonStorage.LookupEntry<CreatureDisplayInfoAddon>(i);
        if (!minfo)
            continue;

        if (!sCreatureDisplayInfoStore.LookupEntry(minfo->display_id))
            sLog.outErrorDb("Table `creature_display_info_addon` has data for nonexistent display id (%u).", minfo->display_id);

        if (minfo->gender > GENDER_NONE)
        {
            sLog.outErrorDb("Table `creature_display_info_addon` has invalid gender (%u) for display id (%u).", uint32(minfo->gender), minfo->display_id);
            const_cast<CreatureDisplayInfoAddon*>(minfo)->gender = GENDER_MALE;
        }

        if (minfo->display_id_other_gender)
        {
            if (minfo->display_id_other_gender == minfo->display_id)
            {
                sLog.outErrorDb("Table `creature_display_info_addon` has redundant display_id_other_gender (%u) defined for display id %u.", minfo->display_id_other_gender, minfo->display_id);
                const_cast<CreatureDisplayInfoAddon*>(minfo)->display_id_other_gender = 0;
            }
            else if (!sCreatureDisplayInfoStore.LookupEntry(minfo->display_id_other_gender))
            {
                sLog.outErrorDb("Table `creature_display_info_addon` has nonexistent display_id_other_gender (%u) defined for display id %u.", minfo->display_id_other_gender, minfo->display_id);
                const_cast<CreatureDisplayInfoAddon*>(minfo)->display_id_other_gender = 0;
            }
        }
    }

    // character races expected have display info data in table
    for (uint32 race = 1; race < sChrRacesStore.GetNumRows(); ++race)
    {
        ChrRacesEntry const* raceEntry = sChrRacesStore.LookupEntry(race);
        if (!raceEntry)
            continue;

        if (!((1 << (race - 1)) & RACEMASK_ALL_PLAYABLE))
            continue;

        if (CreatureDisplayInfoAddon const* minfo = GetCreatureDisplayInfoAddon(raceEntry->model_f))
        {
            if (minfo->gender != GENDER_FEMALE)
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong gender %u for character race %u female display id %u", minfo->gender, race, raceEntry->model_f);

            if (minfo->display_id_other_gender != raceEntry->model_m)
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong other gender display id %u for character race %u female display id %u", minfo->display_id_other_gender, race, raceEntry->model_f);

            if (minfo->bounding_radius <= 0.0f)
            {
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong bounding_radius %f for character race %u female display id %u, use %f instead", minfo->bounding_radius, race, raceEntry->model_f, DEFAULT_WORLD_OBJECT_SIZE);
                const_cast<CreatureDisplayInfoAddon*>(minfo)->bounding_radius = DEFAULT_WORLD_OBJECT_SIZE;
            }

            if (minfo->combat_reach <= 0.0f)
            {
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong combat_reach %f for character race %u female display id %u, use 1.5f instead", minfo->combat_reach, race, raceEntry->model_f);
                const_cast<CreatureDisplayInfoAddon*>(minfo)->combat_reach = 1.5f;
            }
        }
        else
            sLog.outErrorDb("Table `creature_display_info_addon` expect have data for character race %u female display id %u", race, raceEntry->model_f);

        if (CreatureDisplayInfoAddon const* minfo = GetCreatureDisplayInfoAddon(raceEntry->model_m))
        {
            if (minfo->gender != GENDER_MALE)
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong gender %u for character race %u male display id %u", minfo->gender, race, raceEntry->model_m);

            if (minfo->display_id_other_gender != raceEntry->model_f)
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong other gender display id %u for character race %u male display id %u", minfo->display_id_other_gender, race, raceEntry->model_m);

            if (minfo->bounding_radius <= 0.0f)
            {
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong bounding_radius %f for character race %u male display id %u, use %f instead", minfo->bounding_radius, race, raceEntry->model_f, DEFAULT_WORLD_OBJECT_SIZE);
                const_cast<CreatureDisplayInfoAddon*>(minfo)->bounding_radius = DEFAULT_WORLD_OBJECT_SIZE;
            }

            if (minfo->combat_reach <= 0.0f)
            {
                sLog.outErrorDb("Table `creature_display_info_addon` have wrong combat_reach %f for character race %u male display id %u, use 1.5f instead", minfo->combat_reach, race, raceEntry->model_m);
                const_cast<CreatureDisplayInfoAddon*>(minfo)->combat_reach = 1.5f;
            }
        }
        else
            sLog.outErrorDb("Table `creature_display_info_addon` expect have data for character race %u male display id %u", race, raceEntry->model_m);

    }
}

void ObjectMgr::LoadCreatureSpells()
{
    // First we need to collect all script ids.
    std::set<uint32> spellScriptSet;

    std::unique_ptr<QueryResult> result (WorldDatabase.Query("SELECT `id` FROM `creature_spells_scripts`"));
    
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();;
            spellScriptSet.insert(id);
        } while (result->NextRow());
    }

    std::set<uint32> spellScriptSetFull = spellScriptSet;

    // Now we load creature_spells.
    m_CreatureSpellsMap.clear(); // for reload case

                                     //       0        1            2                3               4                 5                 6              7                    8                    9                   10                  11
    result.reset(WorldDatabase.Query("SELECT `entry`, `spellId_1`, `probability_1`, `castTarget_1`, `targetParam1_1`, `targetParam2_1`, `castFlags_1`, `delayInitialMin_1`, `delayInitialMax_1`, `delayRepeatMin_1`, `delayRepeatMax_1`, `scriptId_1`, "
                                      //               12           13               14              15                16                17             18                   19                   20                  21                  22
                                                     "`spellId_2`, `probability_2`, `castTarget_2`, `targetParam1_2`, `targetParam2_2`, `castFlags_2`, `delayInitialMin_2`, `delayInitialMax_2`, `delayRepeatMin_2`, `delayRepeatMax_2`, `scriptId_2`, "
                                     //                23           24               25              26                27                28             29                   30                   31                  32                  33
                                                     "`spellId_3`, `probability_3`, `castTarget_3`, `targetParam1_3`, `targetParam2_3`, `castFlags_3`, `delayInitialMin_3`, `delayInitialMax_3`, `delayRepeatMin_3`, `delayRepeatMax_3`, `scriptId_3`, "
                                     //                34           35               36              37                38                39             40                   41                   42                  43                  44
                                                     "`spellId_4`, `probability_4`, `castTarget_4`, `targetParam1_4`, `targetParam2_4`, `castFlags_4`, `delayInitialMin_4`, `delayInitialMax_4`, `delayRepeatMin_4`, `delayRepeatMax_4`, `scriptId_4`, "
                                     //                45           46               47              48                49                50             51                   52                   53                  54                  55
                                                     "`spellId_5`, `probability_5`, `castTarget_5`, `targetParam1_5`, `targetParam2_5`, `castFlags_5`, `delayInitialMin_5`, `delayInitialMax_5`, `delayRepeatMin_5`, `delayRepeatMax_5`, `scriptId_5`, "
                                     //                56           57               58              59                60                61             62                   63                   64                  65                  66
                                                     "`spellId_6`, `probability_6`, `castTarget_6`, `targetParam1_6`, `targetParam2_6`, `castFlags_6`, `delayInitialMin_6`, `delayInitialMax_6`, `delayRepeatMin_6`, `delayRepeatMax_6`, `scriptId_6`, "
                                     //                67           68               69              70                71                72             73                   74                   75                  76                  77
                                                     "`spellId_7`, `probability_7`, `castTarget_7`, `targetParam1_7`, `targetParam2_7`, `castFlags_7`, `delayInitialMin_7`, `delayInitialMax_7`, `delayRepeatMin_7`, `delayRepeatMax_7`, `scriptId_7`, "
                                     //                78           79               80              81                82                83             84                   85                   86                  87                  88
                                                     "`spellId_8`, `probability_8`, `castTarget_8`, `targetParam1_8`, `targetParam2_8`, `castFlags_8`, `delayInitialMin_8`, `delayInitialMax_8`, `delayRepeatMin_8`, `delayRepeatMax_8`, `scriptId_8` FROM `creature_spells`"));
    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();;

        CreatureSpellsList spellsList;

        for (uint8 i = 0; i < CREATURE_SPELLS_MAX_SPELLS; i++)
        {
            uint16 spellId = fields[1 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt16();
            if (spellId)
            {
                if (!sSpellMgr.GetSpellEntry(spellId))
                {
                    if (!sSpellMgr.IsExistingSpellId(spellId))
                        sLog.outErrorDb("Entry %u in table `creature_spells` has non-existent spell %u used as spellId_%u, skipping spell.", entry, spellId, i);
                    continue;
                }

                uint8 probability      = fields[2 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt8();

                if ((probability == 0) || (probability > 100))
                {
                    sLog.outErrorDb("Entry %u in table `creature_spells` has invalid probability_%u value %u, setting it to 100 instead.", entry, i, probability);
                    probability = 100;
                }

                uint8 castTarget       = fields[3 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt8();
                uint32 targetParam1    = fields[4 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt32();
                uint32 targetParam2    = fields[5 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt32();

                if (!sScriptMgr.CheckScriptTargets(castTarget, targetParam1, targetParam2, "creature_spells", entry))
                    continue;

                uint16 castFlags       = fields[6 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt16();

                // in the database we store timers as seconds
                // based on screenshot of blizzard creature spells editor
                uint32 delayInitialMin = fields[7 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt16() * IN_MILLISECONDS;
                uint32 delayInitialMax = fields[8 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt16() * IN_MILLISECONDS;

                if (delayInitialMin > delayInitialMax)
                {
                    sLog.outErrorDb("Entry %u in table `creature_spells` has invalid initial timers (Min_%u = %u, Max_%u = %u), skipping spell.", entry, i, delayInitialMin, i, delayInitialMax);
                    continue;
                }

                uint32 delayRepeatMin  = fields[9 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt16() * IN_MILLISECONDS;
                uint32 delayRepeatMax  = fields[10 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt16() * IN_MILLISECONDS;

                if (delayRepeatMin > delayRepeatMax)
                {
                    sLog.outErrorDb("Entry %u in table `creature_spells` has invalid repeat timers (Min_%u = %u, Max_%u = %u), skipping spell.", entry, i, delayRepeatMin, i, delayRepeatMax);
                    continue;
                }

                uint32 scriptId = fields[11 + i * CREATURE_SPELLS_MAX_COLUMNS].GetUInt32();

                if (scriptId)
                {
                    if (spellScriptSetFull.find(scriptId) == spellScriptSetFull.end())
                    {
                        sLog.outErrorDb("Entry %u in table `creature_spells` has non-existent scriptId_%u = %u, setting it to 0 instead.", entry, i, scriptId);
                        scriptId = 0;
                    }
                    else
                        spellScriptSet.erase(scriptId);
                }

                spellsList.emplace_back(spellId, probability, castTarget, targetParam1, targetParam2, castFlags, delayInitialMin, delayInitialMax, delayRepeatMin, delayRepeatMax, scriptId);
            }
        }

        if (!spellsList.empty())
            m_CreatureSpellsMap.insert(CreatureSpellsMap::value_type(entry, spellsList));

    } while (result->NextRow());

    for (const auto itr : spellScriptSet)
        sLog.outErrorDb("Table `creature_spells_scripts` contains unused script, id %u.", itr);
}

void ObjectMgr::LoadCreatures(bool reload)
{
    //                                                                          0                  1                2                 3                 4      5
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `creature`.`guid`, `creature`.`id`, `creature`.`id2`, `creature`.`id3`, `creature`.`id4`, `map`, "
    //                      6             7             8             9              10                  11                  12
                          "`position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecsmin`, `spawntimesecsmax`, `wander_distance`, "
    //                      13                14              15               16
                          "`health_percent`, `mana_percent`, `movement_type`, `event`,"
    //                                      17                                     18            19             20
                          "`pool_creature`.`pool_entry`, `pool_creature_template`.`pool_entry`, `spawn_flags`, `visibility_mod` "
                          "FROM `creature` "
                          "LEFT OUTER JOIN `game_event_creature` ON `creature`.`guid` = `game_event_creature`.`guid` "
                          "LEFT OUTER JOIN `pool_creature` ON `creature`.`guid` = `pool_creature`.`guid` "
                          "LEFT OUTER JOIN `pool_creature_template` ON `creature`.`id` = `pool_creature_template`.`id`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 guid         = fields[ 0].GetUInt32();
        uint32 first_entry  = fields[ 1].GetUInt32();
        float curhealth     = fields[13].GetFloat();
        float curmana       = fields[14].GetFloat();
        uint32 spawnFlags   = fields[19].GetUInt32();
        bool is_dead        = spawnFlags & SPAWN_FLAG_DEAD;

        if (!first_entry)
        {
            sLog.outErrorDb("Table `creature` has creature (GUID: %u) with non existing creature entry %u, skipped.", guid, first_entry);
            continue;
        }

        bool skip = false;
        for (int i = 0; i < MAX_CREATURE_IDS_PER_SPAWN; i++)
        {
            if (uint32 entry = fields[1 + i].GetUInt32())
            {
                CreatureInfo const* cInfo = GetCreatureTemplate(entry);
                if (!cInfo)
                {
                    sLog.outErrorDb("Table `creature` has creature (GUID: %u) with non existing creature entry %u, skipped.", guid, entry);
                    sLog.out(LOG_DBERRFIX, "DELETE FROM `creature` WHERE `guid`=%u;", guid);
                    skip = true;
                    break;
                }

                if ((cInfo->regeneration & REGEN_FLAG_HEALTH) && (cInfo->health_min > 0) && (curhealth < 100.0f) && !is_dead)
                {
                    sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with REGEN_FLAG_HEALTH and low current health percent (%g%%).", guid, first_entry, curhealth);
                    sLog.out(LOG_DBERRFIX, "UPDATE `creature` SET `health_percent`=100 WHERE `guid`=%u AND `id`=%u;", guid, first_entry);
                    curhealth = 100.0f;
                }

                if ((cInfo->regeneration & REGEN_FLAG_POWER) && (cInfo->mana_min > 0) && (curmana < 100.0f))
                {
                    sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with REGEN_FLAG_POWER and low current mana percent (%g%%).", guid, first_entry, curmana);
                    sLog.out(LOG_DBERRFIX, "UPDATE `creature` SET `mana_percent=100 WHERE `guid`=%u AND `id`=%u;", guid, first_entry);
                    curmana = 100.0f;
                }

                if (curhealth > 100.0f)
                {
                    sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with more than 100%% health.", guid, first_entry);
                    sLog.out(LOG_DBERRFIX, "UPDATE `creature` SET `health_percent`=100 WHERE `guid`=%u AND `id`=%u;", guid, first_entry);
                    curhealth = 100.0f;
                }

                if (curmana > 100.0f)
                {
                    sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with more than 100%% mana.", guid, first_entry);
                    sLog.out(LOG_DBERRFIX, "UPDATE `creature` SET `mana_percent`=100 WHERE `guid`=%u AND `id`=%u;", guid, first_entry);
                    curmana = 100.0f;
                }
            }
        }

        if (skip)
            continue;

        bool alreadyPresent = reload && m_CreatureDataMap.find(guid) != m_CreatureDataMap.end();
        CreatureData& data = m_CreatureDataMap[guid];

        data.creature_id[0]     = fields[ 1].GetUInt32();
        data.creature_id[1]     = fields[ 2].GetUInt32();
        data.creature_id[2]     = fields[ 3].GetUInt32();
        data.creature_id[3]     = fields[ 4].GetUInt32();
        data.position.mapId     = fields[ 5].GetUInt16();
        data.position.x         = fields[ 6].GetFloat();
        data.position.y         = fields[ 7].GetFloat();
        data.position.z         = fields[ 8].GetFloat();
        data.position.o         = fields[ 9].GetFloat();
        data.spawntimesecsmin   = fields[10].GetUInt32();
        data.spawntimesecsmax   = fields[11].GetUInt32();
        data.wander_distance    = fields[12].GetFloat();
        data.health_percent     = curhealth;
        data.mana_percent       = curmana;
        data.movement_type      = fields[15].GetUInt8();
        data.spawn_flags        = spawnFlags;
        data.visibility_mod     = fields[20].GetFloat();
        data.instanciatedContinentInstanceId = sMapMgr.GetContinentInstanceId(data.position.mapId, data.position.x, data.position.y);
        int16 gameEvent         = fields[16].GetInt16();
        int16 GuidPoolId        = fields[17].GetInt16();
        int16 EntryPoolId       = fields[18].GetInt16();

        MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(data.position.mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u) that spawned at nonexistent map (Id: %u), skipped.", guid, data.position.mapId);
            sLog.out(LOG_DBERRFIX, "DELETE FROM `creature` WHERE `guid`=%u AND `id`=%u;", guid, data.creature_id[0]);
            continue;
        }

        if (data.spawntimesecsmax < data.spawntimesecsmin)
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `spawntimesecsmax` (%u) value lower than `spawntimesecsmin` (%u), it will be adjusted to %u.",
                guid, data.creature_id[0], uint32(data.spawntimesecsmax), uint32(data.spawntimesecsmin), uint32(data.spawntimesecsmin));
            data.spawntimesecsmax = data.spawntimesecsmin;
        }

        if (data.wander_distance < 0.0f)
        {
            sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `wander_distance`< 0, set to 0.", guid, data.creature_id[0]);
            sLog.out(LOG_DBERRFIX, "UPDATE `creature` SET `wander_distance`=0 WHERE `guid`=%u AND `id`=%u;", guid, data.creature_id[0]);
            data.wander_distance = 0.0f;
        }
        else if (data.movement_type == RANDOM_MOTION_TYPE)
        {
            if (data.wander_distance == 0.0f)
            {
                sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `MovementType`=1 (random movement) but with `wander_distance`=0, replace by idle movement type (0).", guid, data.creature_id[0]);
                sLog.out(LOG_DBERRFIX, "UPDATE `creature` SET `movement_type`=%u WHERE `guid`=%u AND `id`=%u;", (uint32)IDLE_MOTION_TYPE, guid, data.creature_id[0]);
                data.movement_type = IDLE_MOTION_TYPE;
            }
        }
        else if (data.movement_type == IDLE_MOTION_TYPE)
        {
            if (data.wander_distance != 0.0f)
            {
                sLog.outErrorDb("Table `creature` have creature (GUID: %u Entry: %u) with `MovementType`=0 (idle) have `wander_distance`<>0, set to 0.", guid, data.creature_id[0]);
                sLog.out(LOG_DBERRFIX, "UPDATE `creature` SET `wander_distance`=0 WHERE `guid`=%u AND `id`=%u;", guid, data.creature_id[0]);
                data.wander_distance = 0.0f;
            }
        }

        if (!alreadyPresent && gameEvent == 0 && GuidPoolId == 0 && EntryPoolId == 0) // if not this is to be managed by GameEvent System or Pool system
            AddCreatureToGrid(guid, &data);

    }
    while (result->NextRow());
}

void ObjectMgr::AddCreatureToGrid(uint32 guid, CreatureData const* data)
{
    CellPair cell_pair = MaNGOS::ComputeCellPair(data->position.x, data->position.y);
    uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

    std::unique_lock<std::mutex> lock(m_MapObjectGuids_lock);
    CellObjectGuids& cell_guids = m_MapObjectGuids[data->position.mapId][cell_id];
    cell_guids.creatures.insert(guid);
}

void ObjectMgr::RemoveCreatureFromGrid(uint32 guid, CreatureData const* data)
{
    CellPair cell_pair = MaNGOS::ComputeCellPair(data->position.x, data->position.y);
    uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

    std::unique_lock<std::mutex> lock(m_MapObjectGuids_lock);
    CellObjectGuids& cell_guids = m_MapObjectGuids[data->position.mapId][cell_id];
    cell_guids.creatures.erase(guid);
}

void ObjectMgr::LoadGameobjects(bool reload)
{
    //                                                                            0                    1     2      3             4             5             6
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `gameobject`.`guid`, `gameobject`.`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`,"
    //                      7            8            9            10           11                12              13       14      15
                          "`rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecsmin`, `spawntimesecsmax`, `animprogress`, `state`, `event`, "
    //                                        16                                       17            18             19
                          "`pool_gameobject`.`pool_entry`, `pool_gameobject_template`.`pool_entry`, `spawn_flags`, `visibility_mod` "
                          "FROM `gameobject` "
                          "LEFT OUTER JOIN `game_event_gameobject` ON `gameobject`.`guid` = `game_event_gameobject`.`guid` "
                          "LEFT OUTER JOIN `pool_gameobject` ON `gameobject`.`guid` = `pool_gameobject`.`guid` "
                          "LEFT OUTER JOIN `pool_gameobject_template` ON `gameobject`.`id` = `pool_gameobject_template`.`id`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        const uint32 guid  = fields[0].GetUInt32();
        const uint32 entry = fields[1].GetUInt32();

        GameObjectInfo const* gInfo = GetGameObjectInfo(entry);
        if (!gInfo)
        {
            sLog.outErrorDb("Table `gameobject` has gameobject (GUID: %u) with non existing gameobject entry %u, skipped.", guid, entry);
            sLog.out(LOG_DBERRFIX, "DELETE FROM gameobject WHERE guid=%u;", guid);
            continue;
        }

        if (gInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(gInfo->displayId))
        {
            sLog.outErrorDb("Gameobject (GUID: %u Entry %u GoType: %u) have invalid displayId (%u), not loaded.", guid, entry, gInfo->type, gInfo->displayId);
            continue;
        }

        bool alreadyPresent = reload && m_GameObjectDataMap.find(guid) != m_GameObjectDataMap.end();
        GameObjectData& data = m_GameObjectDataMap[guid];

        data.id               = entry;
        data.position.mapId   = fields[ 2].GetUInt32();
        data.position.x       = fields[ 3].GetFloat();
        data.position.y       = fields[ 4].GetFloat();
        data.position.z       = fields[ 5].GetFloat();
        data.position.o       = fields[ 6].GetFloat();
        data.rotation0        = fields[ 7].GetFloat();
        data.rotation1        = fields[ 8].GetFloat();
        data.rotation2        = fields[ 9].GetFloat();
        data.rotation3        = fields[10].GetFloat();
        data.spawntimesecsmin = fields[11].GetInt32();
        data.spawntimesecsmax = fields[12].GetInt32();
        data.spawn_flags      = fields[18].GetUInt32();
        data.visibility_mod   = fields[19].GetFloat();
        data.instanciatedContinentInstanceId = sMapMgr.GetContinentInstanceId(data.position.mapId, data.position.x, data.position.y);

        MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(data.position.mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) that spawned at nonexistent map (Id: %u), skip", guid, data.id, data.position.mapId);
            continue;
        }

        if (data.spawntimesecsmin == 0 && gInfo->IsDespawnAtAction())
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with `spawntimesecs` (0) value, but gameobejct marked as despawnable at action.", guid, data.id);

        if (data.spawntimesecsmax < data.spawntimesecsmin)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with `spawntimesecsmax` (%u) value lower than `spawntimesecsmin` (%u), it will be adjusted to %u.",
                guid, data.id, uint32(data.spawntimesecsmax), uint32(data.spawntimesecsmin), uint32(data.spawntimesecsmin));
            data.spawntimesecsmax = data.spawntimesecsmin;
        }

        data.animprogress   = fields[13].GetUInt32();

        uint32 go_state     = fields[14].GetUInt32();
        if (go_state >= MAX_GO_STATE)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid `state` (%u) value, skip", guid, data.id, go_state);
            continue;
        }
        data.go_state       = GOState(go_state);

        int16 gameEvent     = fields[15].GetInt16();
        int16 GuidPoolId    = fields[16].GetInt16();
        int16 EntryPoolId   = fields[17].GetInt16();

        if (data.rotation0 < -1.0f || data.rotation0 > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation0 (%f) value, skip", guid, data.id, data.rotation0);
            continue;
        }

        if (data.rotation1 < -1.0f || data.rotation1 > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation1 (%f) value, skip", guid, data.id, data.rotation1);
            continue;
        }

        if (data.rotation2 < -1.0f || data.rotation2 > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation2 (%f) value, skip", guid, data.id, data.rotation2);
            continue;
        }

        if (data.rotation3 < -1.0f || data.rotation3 > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation3 (%f) value, skip", guid, data.id, data.rotation3);
            continue;
        }

        if (!MapManager::IsValidMapCoord(data.position.mapId, data.position.x, data.position.y, data.position.z, data.position.o))
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid coordinates, skip", guid, data.id);
            sLog.out(LOG_DBERRFIX, "DELETE FROM gameobject WHERE guid=%u;", guid);
            continue;
        }

        if (!alreadyPresent && gameEvent == 0 && GuidPoolId == 0 && EntryPoolId == 0) // if not this is to be managed by GameEvent System or Pool system
            AddGameobjectToGrid(guid, &data);
    }
    while (result->NextRow());
}

void ObjectMgr::AddGameobjectToGrid(uint32 guid, GameObjectData const* data)
{
    CellPair cell_pair = MaNGOS::ComputeCellPair(data->position.x, data->position.y);
    uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

    std::unique_lock<std::mutex> lock(m_MapObjectGuids_lock);
    CellObjectGuids& cell_guids = m_MapObjectGuids[data->position.mapId][cell_id];
    cell_guids.gameobjects.insert(guid);
}

void ObjectMgr::RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data)
{
    CellPair cell_pair = MaNGOS::ComputeCellPair(data->position.x, data->position.y);
    uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

    std::unique_lock<std::mutex> lock(m_MapObjectGuids_lock);
    CellObjectGuids& cell_guids = m_MapObjectGuids[data->position.mapId][cell_id];
    cell_guids.gameobjects.erase(guid);
}

// In order to keep database item template data correct for each patch, fix changed spell effects used by some items here.
// For cases where the spell data itself changed and we need to use a substitute spell id in later clients to recreate old item version.
void ObjectMgr::CorrectItemEffects(uint32 itemId, _ItemSpell& itemSpell)
{
    // Blade of Eternal Darkness
    // The spell data was changed, and the trigger became On Equip instead of On Use in 1.10.
    if ((itemSpell.SpellId == 21978) && (itemSpell.SpellTrigger == 0))
        itemSpell.SpellTrigger = 1;
    // Rhok\'delar, Longbow of the Ancient Keepers
    // The spell data was changed and the spell id removed from this item in 1.8.
    if ((itemSpell.SpellId == 23193) && (itemId == 18713))
        itemSpell.SpellId = 0;
    // Lok\'delar, Stave of the Ancient Keepers
    // The spell data was changed and the spell id removed from this item in 1.8.
    if ((itemSpell.SpellId == 23194) && (itemId == 18715))
        itemSpell.SpellId = 0;
}

void ObjectMgr::FillObtainedItemsList(std::set<uint32>& obtainedItems)
{
    // These are all the items that have been obtained by players.
    {
        std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT DISTINCT `itemEntry` FROM `item_instance`"));
        if (result)
        {
            do
            {
                Field *fields = result->Fetch();
                uint32 itemId = fields[0].GetUInt32();
                obtainedItems.insert(itemId);
            } while (result->NextRow());
        }
    }
    // Items used by spells need to be marked as discovered too.
    {
        for (uint32 spellId = 1; spellId < sSpellMgr.GetMaxSpellId(); ++spellId)
        {
            SpellEntry const *pSpellProto = sSpellMgr.GetSpellEntry(spellId);
            if (!pSpellProto)
                continue;

            for (uint32 i : pSpellProto->Totem)
            {
                if (i)
                    obtainedItems.insert(i);
            }

            for (int32 i : pSpellProto->Reagent)
            {
                if (i)
                    obtainedItems.insert(i);
            }

            for (int32 i = 0; i < MAX_SPELL_EFFECTS; i++)
            {
                if (pSpellProto->Effect[i] == SPELL_EFFECT_CREATE_ITEM)
                    obtainedItems.insert(pSpellProto->EffectItemType[i]);
            }
        }
    }
}

void ObjectMgr::LoadItemPrototypes()
{
    m_itemPrototypesMap.clear();

    //                                                                0        1        2           3       4              5                6          7        8            9            10            11                12                 13                14            15                16                17                     18                19                     20                    21                             22                          23           24           25                 26            27             28            29             30            31             32            33             34            35             36            37             38            39             40            41             42            43             44             45              46       47           48           49          50          51           52          53          54           55          56          57           58          59          60           61          62          63           64       65       66          67          68            69           70            71            72           73                74                75                76                 77                 78                         79           80                81                82                83                 84                 85                         86           87                88                89                90                 91                 92                         93           94                95                96                97                 98                 99                         100          101               102               103               104                105                106                        107        108          109              110              111            112        113         114       115                116       117               118           119          120         121           122              123          124               125               126            127                 128             129
    std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT `entry`, `class`, `subclass`, `name`, `description`, `display_id`, `quality`, `flags`, `buy_count`, `buy_price`, `sell_price`, `inventory_type`, `allowable_class`, `allowable_race`, `item_level`, `required_level`, `required_skill`, `required_skill_rank`, `required_spell`, `required_honor_rank`, `required_city_rank`, `required_reputation_faction`, `required_reputation_rank`, `max_count`, `stackable`, `container_slots`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `delay`, `range_mod`, `ammo_type`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `dmg_min3`, `dmg_max3`, `dmg_type3`, `dmg_min4`, `dmg_max4`, `dmg_type4`, `dmg_min5`, `dmg_max5`, `dmg_type5`, `block`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmrate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmrate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmrate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmrate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmrate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `page_text`, `page_language`, `page_material`, `start_quest`, `lock_id`, `material`, `sheath`, `random_property`, `set_id`, `max_durability`, `area_bound`, `map_bound`, `duration`, `bag_family`, `disenchant_id`, `food_type`, `min_money_loot`, `max_money_loot`, `wrapped_gift`, `extra_flags`, `other_team_entry`, `script_name` "
        " FROM `item_template`"));
    if (!result)
    {
        sLog.outError("`item_template` table is empty!");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();

        ItemPrototype& item = m_itemPrototypesMap[entry];
        item.ItemId = entry;
        item.Class = fields[1].GetUInt8();
        item.SubClass = fields[2].GetUInt8();
        item.Name1 = strdup(fields[3].GetString());
        item.Description = strdup(fields[4].GetString());
        item.DisplayInfoID = fields[5].GetUInt32();
        item.Quality = fields[6].GetUInt8();
        item.Flags = fields[7].GetUInt32();
        item.BuyCount = fields[8].GetUInt8();
        item.BuyPrice = fields[9].GetUInt32();
        item.SellPrice = fields[10].GetUInt32();
        item.InventoryType = fields[11].GetUInt8();
        item.AllowableClass = fields[12].GetInt32();
        item.AllowableRace = fields[13].GetInt32();
        item.ItemLevel = fields[14].GetUInt8();
        item.RequiredLevel = fields[15].GetUInt8();
        item.RequiredSkill = fields[16].GetUInt16();
        item.RequiredSkillRank = fields[17].GetUInt16();
        item.RequiredSpell = fields[18].GetUInt32();
        item.RequiredHonorRank = fields[19].GetUInt32();
        item.RequiredCityRank = fields[20].GetUInt32();
        item.RequiredReputationFaction = fields[21].GetUInt16();
        item.RequiredReputationRank = fields[22].GetUInt16();
        item.MaxCount = fields[23].GetUInt16();
        item.Stackable = fields[24].GetUInt16();
        item.ContainerSlots = fields[25].GetUInt8();
        for (int i = 0; i < MAX_ITEM_PROTO_STATS; i++)
        {
            item.ItemStat[i].ItemStatType = fields[26 + i * 2].GetUInt8();
            item.ItemStat[i].ItemStatValue = fields[27 + i * 2].GetInt16();
        }
        item.Delay = fields[46].GetUInt16();
        item.RangedModRange = fields[47].GetFloat();
        item.AmmoType = fields[48].GetUInt8();
        for (int i = 0; i < MAX_ITEM_PROTO_DAMAGES; i++)
        {
            item.Damage[i].DamageMin = fields[49 + i * 3].GetFloat();
            item.Damage[i].DamageMax = fields[50 + i * 3].GetFloat();
            item.Damage[i].DamageType = fields[51 + i * 3].GetUInt8();
        }
        item.Block = fields[64].GetUInt32();
        item.Armor = fields[65].GetInt16();
        item.HolyRes = fields[66].GetInt16();
        item.FireRes = fields[67].GetInt16();
        item.NatureRes = fields[68].GetInt16();
        item.FrostRes = fields[69].GetInt16();
        item.ShadowRes = fields[70].GetInt16();
        item.ArcaneRes = fields[71].GetInt16();
        for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
        {
            item.Spells[i].SpellId = fields[72 + i * 7].GetUInt32();
            item.Spells[i].SpellTrigger = fields[73 + i * 7].GetUInt8();
            item.Spells[i].SpellCharges = fields[74 + i * 7].GetInt16();
            item.Spells[i].SpellPPMRate = fields[75 + i * 7].GetFloat();
            item.Spells[i].SpellCooldown = fields[76 + i * 7].GetInt32();
            item.Spells[i].SpellCategory = fields[77 + i * 7].GetUInt16();
            item.Spells[i].SpellCategoryCooldown = fields[78 + i * 7].GetInt32();
        }
        item.Bonding = fields[107].GetUInt8();
        item.PageText = fields[108].GetUInt32();
        item.LanguageID = fields[109].GetUInt8();
        item.PageMaterial = fields[110].GetUInt8();
        item.StartQuest = fields[111].GetUInt32();
        item.LockID = fields[112].GetUInt32();
        item.Material = fields[113].GetInt16();
        item.Sheath = fields[114].GetUInt8();
        item.RandomProperty = fields[115].GetUInt32();
        item.ItemSet = fields[116].GetUInt32();
        item.MaxDurability = fields[117].GetUInt32();
        item.Area = fields[118].GetUInt32();
        item.Map = fields[119].GetInt16();
        item.Duration = fields[120].GetUInt32();
        item.BagFamily = fields[121].GetInt32();
        item.DisenchantID = fields[122].GetUInt32();
        item.FoodType = fields[123].GetUInt8();
        item.MinMoneyLoot = fields[124].GetUInt32();
        item.MaxMoneyLoot = fields[125].GetUInt32();
        item.WrappedGift = fields[126].GetUInt32();
        item.ExtraFlags = fields[127].GetUInt8();
        item.OtherTeamEntry = fields[128].GetUInt32();
        item.ScriptId = sScriptMgr.GetScriptId(fields[129].GetString());
    } while (result->NextRow());

    m_QuestStartingItemsMap.clear();
    
    // Load all currently obtained items by players.
    std::set<uint32> obtainedItems;
    FillObtainedItemsList(obtainedItems);

    // check data correctness
    for (auto const& itr : GetItemPrototypeMap())
    {
        uint32 i = itr.first;
        ItemPrototype const* proto = &itr.second;
        if (!proto)
            continue;

        if ((obtainedItems.find(i) != obtainedItems.end()) ||
            (proto->ExtraFlags & ITEM_EXTRA_MAIL_STATIONERY) ||
            !sWorld.getConfig(CONFIG_BOOL_PREVENT_ITEM_DATAMINING))
            proto->Discovered = true;

        if (proto->Class >= MAX_ITEM_CLASS)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Class value (%u)", i, proto->Class);
            const_cast<ItemPrototype*>(proto)->Class = ITEM_CLASS_JUNK;
        }

        if (proto->SubClass >= MaxItemSubclassValues[proto->Class])
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Subclass value (%u) for class %u", i, proto->SubClass, proto->Class);
            const_cast<ItemPrototype*>(proto)->SubClass = 0;// exist for all item classes
        }

        if (proto->Quality >= MAX_ITEM_QUALITY)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Quality value (%u)", i, proto->Quality);
            const_cast<ItemPrototype*>(proto)->Quality = ITEM_QUALITY_NORMAL;
        }

        if (proto->BuyCount <= 0)
        {
            const_cast<ItemPrototype*>(proto)->BuyCount = 1;
        }

        if (proto->InventoryType >= MAX_INVTYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong InventoryType value (%u)", i, proto->InventoryType);
            const_cast<ItemPrototype*>(proto)->InventoryType = INVTYPE_NON_EQUIP;
        }

        if (proto->InventoryType != INVTYPE_NON_EQUIP)
        {
            if (proto->Flags & ITEM_FLAG_LOOTABLE)
            {
                sLog.outErrorDb("Item container (Entry: %u) has not allowed for containers flag ITEM_FLAG_LOOTABLE (%u), flag removed.", i, ITEM_FLAG_LOOTABLE);
                const_cast<ItemPrototype*>(proto)->Flags &= ~ITEM_FLAG_LOOTABLE;
            }
        }
        else if (proto->InventoryType != INVTYPE_BAG)
        {
            if (proto->ContainerSlots > 0)
            {
                sLog.outErrorDb("Non-container item (Entry: %u) has ContainerSlots (%u), set to 0.", i, proto->ContainerSlots);
                const_cast<ItemPrototype*>(proto)->ContainerSlots = 0;
            }
        }

        if (proto->RequiredSkill >= MAX_SKILL_TYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong RequiredSkill value (%u)", i, proto->RequiredSkill);
            const_cast<ItemPrototype*>(proto)->RequiredSkill = 0;
        }

        {
            // can be used in equip slot, as page read use in inventory, or spell casting at use
            bool req = proto->InventoryType != INVTYPE_NON_EQUIP || proto->PageText;
            if (!req)
            {
                for (const auto& itr : proto->Spells)
                {
                    if (itr.SpellId)
                    {
                        req = true;
                        break;
                    }
                }
            }

            if (req)
            {
                if (!(proto->AllowableClass & CLASSMASK_ALL_PLAYABLE))
                    sLog.outErrorDb("Item (Entry: %u) not have in `AllowableClass` any playable classes (%u) and can't be equipped or use.", i, proto->AllowableClass);

                if (!(proto->AllowableRace & RACEMASK_ALL_PLAYABLE))
                    sLog.outErrorDb("Item (Entry: %u) not have in `AllowableRace` any playable races (%u) and can't be equipped or use.", i, proto->AllowableRace);
            }
        }

        if (proto->RequiredSpell && !sSpellMgr.GetSpellEntry(proto->RequiredSpell))
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong (nonexistent) spell in RequiredSpell (%u)", i, proto->RequiredSpell);
            const_cast<ItemPrototype*>(proto)->RequiredSpell = 0;
        }

        if (proto->RequiredReputationRank >= MAX_REPUTATION_RANK)
            sLog.outErrorDb("Item (Entry: %u) has wrong reputation rank in RequiredReputationRank (%u), item can't be used.", i, proto->RequiredReputationRank);

        if (proto->RequiredReputationFaction)
        {
            if (!GetFactionEntry(proto->RequiredReputationFaction))
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong (not existing) faction in RequiredReputationFaction (%u)", i, proto->RequiredReputationFaction);
                const_cast<ItemPrototype*>(proto)->RequiredReputationFaction = 0;
            }

            if (proto->RequiredReputationRank == MIN_REPUTATION_RANK)
                sLog.outErrorDb("Item (Entry: %u) has min. reputation rank in RequiredReputationRank (0) but RequiredReputationFaction > 0, faction setting is useless.", i);
        }
        // else if(proto->RequiredReputationRank > MIN_REPUTATION_RANK)
        //    sLog.outErrorDb("Item (Entry: %u) has RequiredReputationFaction ==0 but RequiredReputationRank > 0, rank setting is useless.",i);

        if (proto->Stackable == 0)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong value in stackable (%u), replace by default 1.", i, proto->Stackable);
            const_cast<ItemPrototype*>(proto)->Stackable = 1;
        }
        else if (proto->Stackable > 255)
        {
            sLog.outErrorDb("Item (Entry: %u) has too large value in stackable (%u), replace by hardcoded upper limit (255).", i, proto->Stackable);
            const_cast<ItemPrototype*>(proto)->Stackable = 255;
        }

        if (proto->ContainerSlots)
        {
            if (proto->ContainerSlots > MAX_BAG_SIZE)
            {
                sLog.outErrorDb("Item (Entry: %u) has too large value in ContainerSlots (%u), replace by hardcoded limit (%u).", i, proto->ContainerSlots, MAX_BAG_SIZE);
                const_cast<ItemPrototype*>(proto)->ContainerSlots = MAX_BAG_SIZE;
            }
        }

        for (int j = 0; j < MAX_ITEM_PROTO_STATS; ++j)
        {
            // for ItemStatValue != 0
            if (proto->ItemStat[j].ItemStatValue && proto->ItemStat[j].ItemStatType >= MAX_ITEM_MOD)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong stat_type%d (%u)", i, j + 1, proto->ItemStat[j].ItemStatType);
                const_cast<ItemPrototype*>(proto)->ItemStat[j].ItemStatType = 0;
            }
        }

        for (int j = 0; j < MAX_ITEM_PROTO_DAMAGES; ++j)
        {
            if (proto->Damage[j].DamageType >= MAX_SPELL_SCHOOL)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong dmg_type%d (%u)", i, j + 1, proto->Damage[j].DamageType);
                const_cast<ItemPrototype*>(proto)->Damage[j].DamageType = 0;
            }
        }

        // normal spell list
        {
            for (uint8 j = 0; j < MAX_ITEM_PROTO_SPELLS; ++j)
            {
                if (proto->Spells[j].SpellId)
                    CorrectItemEffects(proto->ItemId, const_cast<ItemPrototype*>(proto)->Spells[j]);
                
                if (proto->Spells[j].SpellTrigger >= MAX_ITEM_SPELLTRIGGER)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u)", i, j + 1, proto->Spells[j].SpellTrigger);
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
                // on hit can be sued only at weapon
                else if (proto->Spells[j].SpellTrigger == ITEM_SPELLTRIGGER_CHANCE_ON_HIT)
                {
                    if (proto->Class != ITEM_CLASS_WEAPON)
                        sLog.outErrorDb("Item (Entry: %u) isn't weapon (Class: %u) but has on hit spelltrigger_%d (%u), it will not triggered.", i, proto->Class, j + 1, proto->Spells[j].SpellTrigger);
                }

                if (proto->Spells[j].SpellId)
                {
                    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(proto->Spells[j].SpellId);
                    if (!spellInfo)
                    {
                        sLog.outErrorDb("Item (Entry: %u) has wrong (not existing) spell in spellid_%d (%u)", i, j + 1, proto->Spells[j].SpellId);
                        const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    }
                }

                if (proto->Spells[j].SpellCategory > 0)
                {
                    // An item's spell category is a free-form grouping key for shared
                    // cooldowns: the value is only ever used as a map key by
                    // Unit::HasSpellCategoryCooldown, never looked up in SpellCategory.dbc
                    // (this is that store's only reader in the whole core). A category the
                    // DBC does not list still works, so this is a note, not a fault - the
                    // value is deliberately left in place rather than cleared.
                    if (!sSpellCategoryStore.LookupEntry(proto->Spells[j].SpellCategory))
                        sLog.outDetail("Item (Entry: %u) has spell category in spellcategory_%d (%u) that is not listed in SpellCategory.dbc", i, j + 1, proto->Spells[j].SpellCategory);
                }
            }
        }

        if (proto->Bonding >= MAX_BIND_TYPE)
            sLog.outErrorDb("Item (Entry: %u) has wrong Bonding value (%u)", i, proto->Bonding);

        if (proto->PageText)
        {
            if (!sPageTextStore.LookupEntry<PageText>(proto->PageText))
                sLog.outErrorDb("Item (Entry: %u) has non existing first page (Id:%u)", i, proto->PageText);
        }

        if (proto->LockID && !sLockStore.LookupEntry(proto->LockID))
            sLog.outErrorDb("Item (Entry: %u) has wrong LockID (%u)", i, proto->LockID);

        if (proto->Sheath >= MAX_SHEATHETYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Sheath (%u)", i, proto->Sheath);
            const_cast<ItemPrototype*>(proto)->Sheath = SHEATHETYPE_NONE;
        }

        if (proto->RandomProperty && !sItemRandomPropertiesStore.LookupEntry(GetItemEnchantMod(proto->RandomProperty)))
        {
            sLog.outErrorDb("Item (Entry: %u) has unknown (wrong or not listed in `item_enchantment_template`) RandomProperty (%u)", i, proto->RandomProperty);
            const_cast<ItemPrototype*>(proto)->RandomProperty = 0;
        }

        if (proto->ItemSet && !sItemSetStore.LookupEntry(proto->ItemSet))
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong ItemSet (%u)", i, proto->ItemSet);
            const_cast<ItemPrototype*>(proto)->ItemSet = 0;
        }

        if (proto->Area && !AreaEntry::GetById(proto->Area))
            sLog.outErrorDb("Item (Entry: %u) has wrong Area (%u)", i, proto->Area);

        if (proto->Map && !sMapStorage.LookupEntry<MapEntry>(proto->Map))
            sLog.outErrorDb("Item (Entry: %u) has wrong Map (%u)", i, proto->Map);

        if (proto->BagFamily)
        {
            ItemBagFamilyEntry const* bf = sItemBagFamilyStore.LookupEntry(proto->BagFamily);
            if (!bf)
            {
                sLog.outErrorDb("Item (Entry: %u) has bag family %u not listed in ItemBagFamily.dbc, setted it to 0", i, proto->BagFamily);
                const_cast<ItemPrototype*>(proto)->BagFamily = 0;
            }
        }

        if (proto->DisenchantID)
        {
            if (proto->Quality > ITEM_QUALITY_EPIC || proto->Quality < ITEM_QUALITY_UNCOMMON)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong quality (%u) for disenchanting, remove disenchanting loot id.", i, proto->Quality);
                const_cast<ItemPrototype*>(proto)->DisenchantID = 0;
            }
            else if (proto->Class != ITEM_CLASS_WEAPON && proto->Class != ITEM_CLASS_ARMOR)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong item class (%u) for disenchanting, remove disenchanting loot id.", i, proto->Class);
                const_cast<ItemPrototype*>(proto)->DisenchantID = 0;
            }
        }

        if (proto->FoodType >= MAX_PET_DIET)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong FoodType value (%u)", i, proto->FoodType);
            const_cast<ItemPrototype*>(proto)->FoodType = 0;
        }

        if (proto->WrappedGift)
        {
            if (ItemPrototype const* pGift = GetItemPrototype(proto->WrappedGift))
                pGift->Discovered = true;
            else
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong (nonexistent) item in WrappedGift (%u)", i, proto->WrappedGift);

                const_cast<ItemPrototype*>(proto)->WrappedGift = 0;
            }
        }

        if (proto->ExtraFlags)
        {
            if (proto->ExtraFlags & ~ITEM_EXTRA_ALL)
                sLog.outErrorDb("Item (Entry: %u) has wrong ExtraFlags (%u) with unused bits set", i, proto->ExtraFlags);
        }


        if (proto->StartQuest > 0)
        // Item starts a quest, insert it into the quest->startItem map
        {
            if (m_QuestStartingItemsMap.find(proto->StartQuest) == m_QuestStartingItemsMap.end())
                m_QuestStartingItemsMap.insert(robin_hood::pair<uint32, uint32>(proto->StartQuest, proto->ItemId) );
            else
                sLog.outErrorDb("Item #%u also starts quest #%u.", i, proto->StartQuest);
        }
    }
}

void ObjectMgr::LoadItemLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_ItemLocaleMap.clear();                                 // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `name_loc1`, `description_loc1`, `name_loc2`, `description_loc2`, `name_loc3`, `description_loc3`, `name_loc4`, `description_loc4`, `name_loc5`, `description_loc5`, `name_loc6`, `description_loc6`, `name_loc7`, `description_loc7`, `name_loc8`, `description_loc8` FROM `locales_item`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!GetItemPrototype(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_item` has data for nonexistent item entry %u, skipped.", entry);
            continue;
        }

        ItemLocale& data = m_ItemLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }

            str = fields[1 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Description.size() <= idx)
                        data.Description.resize(idx + 1);

                    data.Description[idx] = str;
                }
            }
        }
    }
    while (result->NextRow());
}

void ObjectMgr::LoadItemRequiredTarget()
{
    m_ItemRequiredTarget.clear();                           // needed for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `type`, `target_entry` FROM `item_required_target`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 uiItemId      = fields[0].GetUInt32();
        uint32 uiType        = fields[1].GetUInt32();
        uint32 uiTargetEntry = fields[2].GetUInt32();

        ItemPrototype const* pItemProto = GetItemPrototype(uiItemId);

        if (!pItemProto)
        {
            if (!IsExistingItemId(uiItemId))
                sLog.outErrorDb("Table `item_required_target`: Entry %u listed for TargetEntry %u does not exist in `item_template`.", uiItemId, uiTargetEntry);
            continue;
        }

        bool bIsItemSpellValid = false;

        for (const auto& itr : pItemProto->Spells)
        {
            if (SpellEntry const* pSpellInfo = sSpellMgr.GetSpellEntry(itr.SpellId))
            {
                if (itr.SpellTrigger == ITEM_SPELLTRIGGER_ON_USE ||
                    itr.SpellTrigger == ITEM_SPELLTRIGGER_ON_NO_DELAY_USE)
                {
                    SpellScriptTargetBounds bounds = sSpellMgr.GetSpellScriptTargetBounds(pSpellInfo->Id);
                    if (bounds.first != bounds.second)
                        break;

                    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
                    {
                        if (pSpellInfo->EffectImplicitTargetA[j] == TARGET_UNIT_ENEMY ||
                                pSpellInfo->EffectImplicitTargetB[j] == TARGET_UNIT_ENEMY ||
                                pSpellInfo->EffectImplicitTargetA[j] == TARGET_UNIT ||
                                pSpellInfo->EffectImplicitTargetB[j] == TARGET_UNIT)
                        {
                            bIsItemSpellValid = true;
                            break;
                        }
                    }
                    if (bIsItemSpellValid)
                        break;
                }
            }
        }

        if (!bIsItemSpellValid)
        {
sLog.outErrorDb("Table `item_required_target`: Spell used by item %u does not have implicit target TARGET_UNIT_ENEMY(6), TARGET_DUELVSPLAYER(25), already listed in `spell_script_target` or doesn't have item spelltrigger.", uiItemId);
            continue;
        }

        if (!uiType || uiType > MAX_ITEM_REQ_TARGET_TYPE)
        {
            sLog.outErrorDb("Table `item_required_target`: Type %u for TargetEntry %u is incorrect.", uiType, uiTargetEntry);
            continue;
        }

        if (!uiTargetEntry)
        {
            sLog.outErrorDb("Table `item_required_target`: TargetEntry == 0 for Type (%u).", uiType);
            continue;
        }

        if (!GetCreatureTemplate(uiTargetEntry))
        {
            if (!sObjectMgr.IsExistingCreatureId(uiTargetEntry))
                sLog.outErrorDb("Table `item_required_target`: creature template entry %u does not exist.", uiTargetEntry);
            continue;
        }

        m_ItemRequiredTarget.insert(ItemRequiredTargetMap::value_type(uiItemId, ItemRequiredTarget(ItemRequiredTargetType(uiType), uiTargetEntry)));
    }
    while (result->NextRow());
}

void ObjectMgr::LoadPetLevelInfo()
{
    {
        //                                                               0                 1        2     3       4      5      6      7       8      9
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `creature_entry`, `level`, `hp`, `mana`, `str`, `agi`, `sta`, `inte`, `spi`, `armor` FROM `pet_levelstats`"));

        if (!result)
        {
            sLog.outErrorDb("Error loading `pet_levelstats` table or empty table.");
            return;
        }

        do
        {
            Field* fields = result->Fetch();

            uint32 creature_id = fields[0].GetUInt32();
            if (!GetCreatureTemplate(creature_id))
            {
                if (!IsExistingCreatureId(creature_id))
                    sLog.outErrorDb("Wrong creature id %u in `pet_levelstats` table, ignoring.", creature_id);
                continue;
            }

            uint32 current_level = fields[1].GetUInt32();
            if (current_level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            {
                if (current_level > PLAYER_STRONG_MAX_LEVEL) // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in `pet_levelstats` table, ignoring.", PLAYER_STRONG_MAX_LEVEL, current_level);
                else
                {
                    DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `pet_levelstats` table, ignoring.", current_level);
                }
                continue;
            }
            else if (current_level < 1)
            {
                sLog.outErrorDb("Wrong (<1) level %u in `pet_levelstats` table, ignoring.", current_level);
                continue;
            }

            PetLevelInfo*& pInfoMapEntry = m_PetInfoMap[creature_id];

            if (pInfoMapEntry == nullptr)
                pInfoMapEntry =  new PetLevelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)];

            // data for level 1 stored in [0] array element, ...
            PetLevelInfo* pLevelInfo = &pInfoMapEntry[current_level - 1];

            pLevelInfo->health = fields[2].GetUInt16();
            pLevelInfo->mana   = fields[3].GetUInt16();
            pLevelInfo->armor  = fields[9].GetUInt16();

            for (int i = 0; i < MAX_STATS; i++)
                pLevelInfo->stats[i] = fields[i + 4].GetUInt16();
        }
        while (result->NextRow());
    }

    // Fill gaps and check integrity
    for (const auto& itr : m_PetInfoMap)
    {
        PetLevelInfo* pInfo = itr.second;

        // fatal error if no level 1 data
        if (!pInfo || pInfo[0].health == 0)
        {
            sLog.outErrorDb("Creature %u does not have pet stats data for Level 1!", itr.first);
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        // fill level gaps
        for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
        {
            if (pInfo[level].health == 0)
            {
                sLog.outErrorDb("Creature %u has no data for Level %i pet stats data, using data of Level %i.", itr.first, level + 1, level);
                pInfo[level] = pInfo[level - 1];
            }
        }
    }
}

ItemPrototype const* ObjectMgr::GetItemPrototype(uint32 id) const
{
    const ItemPrototype* transmogrifyProto = sObjectMgr.GetItemTransmogrifyTemplate(id);
    if (transmogrifyProto)
        return transmogrifyProto;

    auto iter = m_itemPrototypesMap.find(id);
    if (iter == m_itemPrototypesMap.end())
        return nullptr;

    return &iter->second;
}

PetLevelInfo const* ObjectMgr::GetPetLevelInfo(uint32 creature_id, uint32 level) const
{
    if (level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        level = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

    PetLevelInfoMap::const_iterator itr = m_PetInfoMap.find(creature_id);
    if (itr == m_PetInfoMap.end())
        return nullptr;

    return &itr->second[level - 1];                         // data for level 1 stored in [0] array element, ...
}

void ObjectMgr::LoadPlayerInfo()
{
    // Load playercreateinfo
    {
        //                                                               0       1        2      3       4             5             6             7
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `race`, `class`, `map`, `zone`, `position_x`, `position_y`, `position_z`, `orientation` FROM `playercreateinfo`"));

        if (!result)
        {
            sLog.outErrorDb("Error loading `playercreateinfo` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        do
        {
            Field* fields = result->Fetch();

            uint32 current_race  = fields[0].GetUInt32();
            uint32 current_class = fields[1].GetUInt32();
            uint32 mapId         = fields[2].GetUInt32();
            uint32 areaId        = fields[3].GetUInt32();
            float  positionX     = fields[4].GetFloat();
            float  positionY     = fields[5].GetFloat();
            float  positionZ     = fields[6].GetFloat();
            float  orientation   = fields[7].GetFloat();

            ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
            if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong race %u in `playercreateinfo` table, ignoring.", current_race);
                continue;
            }

            ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
            if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong class %u in `playercreateinfo` table, ignoring.", current_class);
                continue;
            }

            // accept DB data only for valid position (and non instanceable)
            if (!MapManager::IsValidMapCoord(mapId, positionX, positionY, positionZ, orientation))
            {
                sLog.outErrorDb("Wrong home position for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                continue;
            }

            if (sMapStorage.LookupEntry<MapEntry>(mapId)->Instanceable())
            {
                sLog.outErrorDb("Home position in instanceable map for class %u race %u pair in `playercreateinfo` table, ignoring.", current_class, current_race);
                continue;
            }

            PlayerInfo* pInfo = &m_PlayerInfo[current_race][current_class];

            pInfo->mapId       = mapId;
            pInfo->areaId      = areaId;
            pInfo->positionX   = positionX;
            pInfo->positionY   = positionY;
            pInfo->positionZ   = positionZ;
            pInfo->orientation = orientation;

            pInfo->displayId_m = rEntry->model_m;
            pInfo->displayId_f = rEntry->model_f;
        }
        while (result->NextRow());
    }

    // Load playercreateinfo_item
    {
        //                                                               0       1        2         3
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `race`, `class`, `itemid`, `amount` FROM `playercreateinfo_item`"));

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                uint32 current_class = fields[1].GetUInt32();

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong race %u in `playercreateinfo_item` table, ignoring.", current_race);
                    continue;
                }

                ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
                if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong class %u in `playercreateinfo_item` table, ignoring.", current_class);
                    continue;
                }

                PlayerInfo* pInfo = &m_PlayerInfo[current_race][current_class];

                uint32 item_id = fields[2].GetUInt32();

                if (ItemPrototype const* pProto = GetItemPrototype(item_id))
                    pProto->Discovered = true;
                else
                {
                    sLog.outErrorDb("Item id %u (race %u class %u) in `playercreateinfo_item` table but not listed in `item_template`, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                uint32 amount  = fields[3].GetUInt32();

                if (!amount)
                {
                    sLog.outErrorDb("Item id %u (class %u race %u) have amount==0 in `playercreateinfo_item` table, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                pInfo->item.emplace_back(item_id, amount);
            }
            while (result->NextRow());
        }
    }

    // Load playercreateinfo_spell
    {
        //                                                                0       1        2
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `race`, `class`, `spell` FROM `playercreateinfo_spell`"));

        if (!result)
        {
            sLog.outErrorDb("Error loading `playercreateinfo_spell` table or empty table.");
        }
        else
        {
            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                uint32 current_class = fields[1].GetUInt32();

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong race %u in `playercreateinfo_spell` table, ignoring.", current_race);
                    continue;
                }

                ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
                if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong class %u in `playercreateinfo_spell` table, ignoring.", current_class);
                    continue;
                }

                uint32 spell_id = fields[2].GetUInt32();
                if (!sSpellMgr.GetSpellEntry(spell_id))
                {
                    sLog.outErrorDb("Non existing spell %u in `playercreateinfo_spell` table, ignoring.", spell_id);
                    continue;
                }

                PlayerInfo* pInfo = &m_PlayerInfo[current_race][current_class];
                pInfo->spell.push_back(spell_id);
            }
            while (result->NextRow());
        }
    }

    // Load playercreateinfo_action
    {
        //                                                               0       1        2         3         4
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `race`, `class`, `button`, `action`, `type` FROM `playercreateinfo_action`"));

        if (!result)
        {
            sLog.outErrorDb("Error loading `playercreateinfo_action` table or empty table.");
        }
        else
        {
            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                uint32 current_class = fields[1].GetUInt32();

                ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
                if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong race %u in `playercreateinfo_action` table, ignoring.", current_race);
                    continue;
                }

                ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
                if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
                {
                    sLog.outErrorDb("Wrong class %u in `playercreateinfo_action` table, ignoring.", current_class);
                    continue;
                }

                uint8 action_button  = fields[2].GetUInt8();
                uint32 action = fields[3].GetUInt32();
                uint8 action_type = fields[4].GetUInt8();

                if (!Player::IsActionButtonDataValid(action_button, action, action_type, nullptr))
                    continue;

                PlayerInfo* pInfo = &m_PlayerInfo[current_race][current_class];
                pInfo->action.emplace_back(action_button, action, action_type);
            }
            while (result->NextRow());
        }
    }

    // Loading levels data (class only dependent)
    {
        //                                                               0        1        2         3
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `class`, `level`, `basehp`, `basemana` FROM `player_classlevelstats`"));

        if (!result)
        {
            sLog.outErrorDb("Error loading `player_classlevelstats` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        do
        {
            Field* fields = result->Fetch();

            uint32 current_class = fields[0].GetUInt32();
            if (current_class >= MAX_CLASSES)
            {
                sLog.outErrorDb("Wrong class %u in `player_classlevelstats` table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[1].GetUInt32();
            if (current_level == 0)
            {
                sLog.outErrorDb("Wrong level %u in `player_classlevelstats` table, ignoring.", current_level);
                continue;
            }
            else if (current_level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            {
                if (current_level > PLAYER_STRONG_MAX_LEVEL) // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in `player_classlevelstats` table, ignoring.", PLAYER_STRONG_MAX_LEVEL, current_level);
                else
                {
                    DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `player_classlevelstats` table, ignoring.", current_level);
                }
                continue;
            }

            PlayerClassInfo* pClassInfo = &m_PlayerClassInfo[current_class];

            if (!pClassInfo->levelInfo)
                pClassInfo->levelInfo = new PlayerClassLevelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)];

            PlayerClassLevelInfo* pClassLevelInfo = &pClassInfo->levelInfo[current_level - 1];

            pClassLevelInfo->basehealth = fields[2].GetUInt16();
            pClassLevelInfo->basemana   = fields[3].GetUInt16();
        }
        while (result->NextRow());
    }

    // Fill gaps and check integrity
    for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
    {
        // skip nonexistent classes
        if (!sChrClassesStore.LookupEntry(class_))
            continue;

        PlayerClassInfo* pClassInfo = &m_PlayerClassInfo[class_];

        // fatal error if no level 1 data
        if (!pClassInfo->levelInfo || pClassInfo->levelInfo[0].basehealth == 0)
        {
            sLog.outErrorDb("Class %i Level 1 does not have health/mana data!", class_);
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        // fill level gaps
        for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
        {
            if (pClassInfo->levelInfo[level].basehealth == 0)
            {
                sLog.outErrorDb("Class %i Level %i does not have health/mana data. Using stats data of level %i.", class_, level + 1, level);
                pClassInfo->levelInfo[level] = pClassInfo->levelInfo[level - 1];
            }
        }
    }

    // Loading levels data (class/race dependent)
    {
        //                                                               0       1        2        3      4      5      6       7
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `race`, `class`, `level`, `str`, `agi`, `sta`, `inte`, `spi` FROM `player_levelstats`"));

        if (!result)
        {
            sLog.outErrorDb("Error loading `player_levelstats` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        do
        {
            Field* fields = result->Fetch();

            uint32 current_race = fields[0].GetUInt32();
            uint32 current_class = fields[1].GetUInt32();

            ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
            if (!rEntry || !((1 << (current_race - 1)) & RACEMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong race %u in `player_levelstats` table, ignoring.", current_race);
                continue;
            }

            ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(current_class);
            if (!cEntry || !((1 << (current_class - 1)) & CLASSMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Wrong class %u in `player_levelstats` table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[2].GetUInt32();
            if (current_level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            {
                if (current_level > PLAYER_STRONG_MAX_LEVEL) // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in `player_levelstats` table, ignoring.", PLAYER_STRONG_MAX_LEVEL, current_level);
                else
                {
                    DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `player_levelstats` table, ignoring.", current_level);
                }
                continue;
            }

            PlayerInfo* pInfo = &m_PlayerInfo[current_race][current_class];

            if (!pInfo->levelInfo)
                pInfo->levelInfo = new PlayerLevelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)];

            PlayerLevelInfo* pLevelInfo = &pInfo->levelInfo[current_level - 1];

            for (int i = 0; i < MAX_STATS; ++i)
                pLevelInfo->stats[i] = fields[i + 3].GetUInt8();
        }
        while (result->NextRow());
    }

    // Fill gaps and check integrity
    for (int race = 1; race < MAX_RACES; ++race)
    {
        // skip nonexistent races
        if (!((1 << (race - 1)) & RACEMASK_ALL_PLAYABLE) || !sChrRacesStore.LookupEntry(race))
            continue;

        for (int class_ = 1; class_ < MAX_CLASSES; ++class_)
        {
            // skip nonexistent classes
            if (!((1 << (class_ - 1)) & CLASSMASK_ALL_PLAYABLE) || !sChrClassesStore.LookupEntry(class_))
                continue;

            PlayerInfo* pInfo = &m_PlayerInfo[race][class_];

            // skip non loaded combinations
            if (!pInfo->displayId_m || !pInfo->displayId_f)
                continue;

            // fatal error if no level 1 data
            if (!pInfo->levelInfo || pInfo->levelInfo[0].stats[0] == 0)
            {
                sLog.outErrorDb("Race %i Class %i Level 1 does not have stats data!", race, class_);
                Log::WaitBeforeContinueIfNeed();
                exit(1);
            }

            // fill level gaps
            for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
            {
                if (pInfo->levelInfo[level].stats[0] == 0)
                {
                    sLog.outErrorDb("Race %i Class %i Level %i does not have stats data. Using stats data of level %i.", race, class_, level + 1, level);
                    pInfo->levelInfo[level] = pInfo->levelInfo[level - 1];
                }
            }
        }
    }

    // Loading xp per level data
    {
        m_PlayerXPperLevel.resize(sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)+1);
        for (uint32 level = 0; level <= sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
            m_PlayerXPperLevel[level] = 0;

        //                                                               0      1
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `lvl`, `xp_for_next_level` FROM `player_xp_for_level`"));

        if (!result)
        {
            sLog.outErrorDb("Error loading `player_xp_for_level` table or empty table.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }

        do
        {
            Field* fields = result->Fetch();

            uint32 current_level = fields[0].GetUInt32();
            uint32 current_xp    = fields[1].GetUInt32();

            if (current_level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            {
                if (current_level > PLAYER_STRONG_MAX_LEVEL) // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in `player_xp_for_level` table, ignoring.", PLAYER_STRONG_MAX_LEVEL, current_level);
                else
                {
                    DETAIL_LOG("Unused (> MaxPlayerLevel in mangosd.conf) level %u in `player_xp_for_levels` table, ignoring.", current_level);
                }
                continue;
            }
            //PlayerXPperLevel
            m_PlayerXPperLevel[current_level] = current_xp;
        }
        while (result->NextRow());;
    }

    // fill level gaps
    for (uint32 level = 1; level < sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL); ++level)
    {
        if (m_PlayerXPperLevel[level] == 0)
        {
            sLog.outErrorDb("Level %i does not have XP for level data. Using data of level [%i] + 100.", level + 1, level);
            m_PlayerXPperLevel[level] = m_PlayerXPperLevel[level - 1] + 100;
        }
    }
}

void ObjectMgr::GetPlayerClassLevelInfo(uint32 class_, uint32 level, PlayerClassLevelInfo* info) const
{
    if (level < 1 || class_ >= MAX_CLASSES)
        return;

    PlayerClassInfo const* pInfo = &m_PlayerClassInfo[class_];

    if (level > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        level = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

    *info = pInfo->levelInfo[level - 1];
}

void ObjectMgr::GetPlayerLevelInfo(uint32 race, uint32 class_, uint32 level, PlayerLevelInfo* info) const
{
    if (level < 1 || race   >= MAX_RACES || class_ >= MAX_CLASSES)
        return;

    PlayerInfo const* pInfo = &m_PlayerInfo[race][class_];
    if (pInfo->displayId_m == 0 || pInfo->displayId_f == 0)
        return;

    if (level <= sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        *info = pInfo->levelInfo[level - 1];
    else
        BuildPlayerLevelInfo(race, class_, level, info);
}

void ObjectMgr::BuildPlayerLevelInfo(uint8 race, uint8 _class, uint8 level, PlayerLevelInfo* info) const
{
    // base data (last known level)
    *info = m_PlayerInfo[race][_class].levelInfo[sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL) - 1];

    for (int lvl = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL) - 1; lvl < level; ++lvl)
    {
        switch (_class)
        {
            case CLASS_WARRIOR:
                info->stats[STAT_STRENGTH]  += (lvl > 23 ? 2 : (lvl > 1  ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 23 ? 2 : (lvl > 1  ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 36 ? 1 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                break;
            case CLASS_PALADIN:
                info->stats[STAT_STRENGTH]  += (lvl > 3  ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 33 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1 : (lvl > 7 && !(lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 6 && (lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 7 ? 1 : 0);
                break;
            case CLASS_HUNTER:
                info->stats[STAT_STRENGTH]  += (lvl > 4  ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 33 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && (lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1 : (lvl > 9 && !(lvl % 2) ? 1 : 0));
                break;
            case CLASS_ROGUE:
                info->stats[STAT_STRENGTH]  += (lvl > 5  ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 4  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 16 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 8 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1 : (lvl > 9 && !(lvl % 2) ? 1 : 0));
                break;
            case CLASS_PRIEST:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 1 : (lvl > 8 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 22 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 3  ? 1 : 0);
                break;
            case CLASS_SHAMAN:
                info->stats[STAT_STRENGTH]  += (lvl > 34 ? 1 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 4 ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 7 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_INTELLECT] += (lvl > 5 ? 1 : 0);
                info->stats[STAT_SPIRIT]    += (lvl > 4 ? 1 : 0);
                break;
            case CLASS_MAGE:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 5  ? 1 : 0);
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_INTELLECT] += (lvl > 24 ? 2 : (lvl > 1 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 33 ? 2 : (lvl > 2 ? 1 : 0));
                break;
            case CLASS_WARLOCK:
                info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_STAMINA]   += (lvl > 38 ? 2 : (lvl > 3 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl % 2) ? 1 : 0);
                info->stats[STAT_INTELLECT] += (lvl > 33 ? 2 : (lvl > 2 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 2 : (lvl > 3 ? 1 : 0));
                break;
            case CLASS_DRUID:
                info->stats[STAT_STRENGTH]  += (lvl > 38 ? 2 : (lvl > 6 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_STAMINA]   += (lvl > 32 ? 2 : (lvl > 4 ? 1 : 0));
                info->stats[STAT_AGILITY]   += (lvl > 38 ? 2 : (lvl > 8 && (lvl % 2) ? 1 : 0));
                info->stats[STAT_INTELLECT] += (lvl > 38 ? 3 : (lvl > 4 ? 1 : 0));
                info->stats[STAT_SPIRIT]    += (lvl > 38 ? 3 : (lvl > 5 ? 1 : 0));
        }
    }
}

void ObjectMgr::LoadGroups()
{
    //                                                                   0           1                2             3             4                5        6        7        8        9        10       11       12       13        14            15
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `mainTank`, `mainAssistant`, `lootMethod`, `looterGuid`, `lootThreshold`, `icon1`, `icon2`, `icon3`, `icon4`, `icon5`, `icon6`, `icon7`, `icon8`, `isRaid`, `leaderGuid`, `groupId` FROM `groups`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();
        Group *group = new Group;
        if (!group->LoadGroupFromDB(fields))
        {
            group->Disband();
            delete group;
            continue;
        }
        AddGroup(group);
    }
    while (result->NextRow());

    // -- loading members --
    //                                            0             1            2           3
    result.reset(CharacterDatabase.Query("SELECT `memberGuid`, `assistant`, `subgroup`, `groupId` FROM `group_member` ORDER BY `groupId`"));
    if (result)
    {
        Group* group = nullptr;                                // used as cached pointer for avoid relookup group for each member

        do
        {
            Field *fields = result->Fetch();

            uint32 memberGuidlow = fields[0].GetUInt32();
            ObjectGuid memberGuid = ObjectGuid(HIGHGUID_PLAYER, memberGuidlow);
            bool   assistent     = fields[1].GetBool();
            uint8  subgroup      = fields[2].GetUInt8();
            uint32 groupId       = fields[3].GetUInt32();
            if (!group || group->GetId() != groupId)
            {
                group = GetGroupById(groupId);
                if (!group)
                {
                    sLog.outErrorDb("Incorrect entry in group_member table : no group with Id %d for member %s!",
                                    groupId, memberGuid.GetString().c_str());
                    CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid = '%u'", memberGuidlow);
                    continue;
                }
            }

            if (!group->LoadMemberFromDB(memberGuidlow, subgroup, assistent))
            {
                sLog.outErrorDb("Incorrect entry in group_member table : member %s cannot be added to group (Id: %u)!",
                                memberGuid.GetString().c_str(), groupId);
                CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid = '%u'", memberGuidlow);
            }
        }
        while (result->NextRow());
    }

    // clean groups
    // TODO: maybe delete from the DB before loading in this case
    for (GroupMap::iterator itr = m_GroupMap.begin(); itr != m_GroupMap.end();)
    {
        if ((itr->second->GetMembersCount() < 2) || (itr->second->GetMembersCount() > 40))
        {
            itr->second->Disband();
            delete itr->second;
            itr = m_GroupMap.erase(itr);
        }
        else
            ++itr;
    }

    // -- loading instances --
    result.reset(CharacterDatabase.Query(
                 //                        0             1      2           3            4
                 "SELECT `group_instance`.`leaderGuid`, `map`, `instance`, `permanent`, `resettime`, "
                 // 5
                 "(SELECT COUNT(*) FROM `character_instance` WHERE `guid` = `group_instance`.`leaderGuid` AND `instance` = `group_instance`.`instance` AND `permanent` = 1 LIMIT 1), "
                 // 6
                 " `groups`.`groupId` "
                 "FROM `group_instance` LEFT JOIN `instance` ON `instance` = `id` LEFT JOIN `groups` ON `groups`.`leaderGUID` = `group_instance`.`leaderGUID` ORDER BY `leaderGuid`"
             ));

    if (result)
    {
        Group* group = nullptr;                                // used as cached pointer for avoid relookup group for each member

        do
        {
            Field *fields = result->Fetch();

            uint32 leaderGuidLow = fields[0].GetUInt32();
            uint32 mapId = fields[1].GetUInt32();
            uint32 groupId = fields[6].GetUInt32();

            if (!group || group->GetId() != groupId)
            {
                // find group id in map by leader low guid
                group = GetGroupById(groupId);
                if (!group)
                {
                    sLog.outErrorDb("Incorrect entry in group_instance table : no group with leader %d", leaderGuidLow);
                    continue;
                }
            }

            MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(mapId);
            if (!mapEntry || !mapEntry->IsDungeon())
            {
                sLog.outErrorDb("Incorrect entry in group_instance table : no dungeon map %d", mapId);
                continue;
            }

            DungeonPersistentState *state = (DungeonPersistentState*)sMapPersistentStateMgr.AddPersistentState(mapEntry, fields[2].GetUInt32(), (time_t)fields[4].GetUInt64(), (fields[5].GetUInt32() == 0), true);
            group->BindToInstance(state, fields[3].GetBool(), true);
        }
        while (result->NextRow());
    }
}

void ObjectMgr::LoadQuestSpellCastObjectives()
{
    //                                                               0        1      2           3              4               5 
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `idx`, `spell_id`, `player_guid`, `player_class`, `objective_text` FROM `quest_cast_objective`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 questId = fields[0].GetUInt32();
        uint8 idx = fields[1].GetUInt8();
        uint32 spellId = fields[2].GetUInt32();
        int32 playerGuid = fields[3].GetInt32();
        uint8 playerClass = fields[4].GetUInt8();

        if (idx >= QUEST_OBJECTIVES_COUNT)
        {
            sLog.outErrorDb("Invalid spell cast objective index for quest %u.", questId);
            continue;
        }

        SpellEntry const* pSpellEntry = sSpellMgr.GetSpellEntry(spellId);
        if (!pSpellEntry)
        {
            sLog.outErrorDb("Spell %u used as cast objective for quest %u does not exist.", spellId, questId);
            continue;
        }

        char const* playerName = "Any Player";

        if (playerGuid > 0)
        {
            if (PlayerCacheData* pData = sObjectMgr.GetPlayerDataByGUID(playerGuid))
            {
                playerName = pData->sName.c_str();
            }
        }
        else if (playerGuid < 0)
            playerName = "a Gamemaster";
        
        char const* objectiveText = fields[5].GetString();
        char objectiveTextBuffer[256] = {};

        if (objectiveText && strlen(objectiveText))
            snprintf(objectiveTextBuffer, 255, objectiveText, playerName);
        else
            snprintf(objectiveTextBuffer, 255, "Cast %s on %s", pSpellEntry->SpellName[0].c_str(), playerName);

        auto itrQuest = m_QuestTemplatesMap.find(questId);
        if (itrQuest == m_QuestTemplatesMap.end())
        {
            sLog.outErrorDb("Quest %u with defiend spell cast objectives does not exist.", questId);
            continue;
        }

        if (!itrQuest->second->ReqCreatureOrGOId[idx])
            itrQuest->second->m_reqCreatureOrGOcount++;

        if (playerGuid > 0)
            itrQuest->second->ReqCreatureOrGOId[idx] = playerGuid;
        else
            itrQuest->second->ReqCreatureOrGOId[idx] = questId + idx * 10;

        itrQuest->second->ReqCreatureOrGOCount[idx] = 1;
        itrQuest->second->ObjectiveText[idx] = objectiveTextBuffer;

        QuestSpellCastObjective data;
        data.questId = questId;
        data.idx = idx;
        data.playerGuid = playerGuid;
        data.playerClass = playerClass;
        m_questSpellCastObjectives[spellId].push_back(data);

    } while (result->NextRow());
}

//void ObjectMgr::ResetYearlyQuests()
//{
//    std::string questListStr;
//
//    for (auto& iter : m_QuestTemplatesMap)
//    {
//       std::unique_ptr<Quest> const& pQuest = iter.second;
//       if (pQuest->HasSpecialFlag(QUEST_SPECIAL_FLAG_YEARLY_RESET))
//       {
//           if (!questListStr.empty())
//               questListStr += ",";
//           questListStr += std::to_string(pQuest->GetQuestId());
//       }
//    }
//
//    if (questListStr.empty())
//    {
//        sLog.outErrorDb("No yearly quests defined in database.");
//        return;
//    }
//
//    tm local;
//    time_t curr;
//    time(&curr);
//    local = *(localtime(&curr));
//
//    // Last Year December 31st 23:59:59
//    local.tm_year -= 1;
//    local.tm_mon = 11;
//    local.tm_mday = 31;
//    local.tm_hour = 23;
//    local.tm_min = 59;
//    local.tm_sec = 59;
//
//    CharacterDatabase.PExecute("DELETE FROM `character_queststatus` WHERE (`rewarded` = 1) && (`timer` < %u) && (`quest` IN (%s))", mktime(&local), questListStr.c_str());
//}

void ObjectMgr::LoadQuests()
{
    // For reload case
    m_QuestTemplatesMap.clear();

    m_ExclusiveQuestGroups.clear();

    //                                                                0        1         2             3           4             5       6                  7                8                9
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `Method`, `ZoneOrSort`, `MinLevel`, `QuestLevel`, `Type`, `RequiredClasses`, `RequiredRaces`, `RequiredSkill`, `RequiredSkillValue`,"
    //                      10                     11                   12                       13                     14                       15                     16                  17
                          "`RepObjectiveFaction`, `RepObjectiveValue`, `RequiredMinRepFaction`, `RequiredMinRepValue`, `RequiredMaxRepFaction`, `RequiredMaxRepValue`, `SuggestedPlayers`, `LimitTime`,"
    //                      18            19              20             21             22                23                  24           25              26
                          "`QuestFlags`, `SpecialFlags`, `PrevQuestId`, `NextQuestId`, `ExclusiveGroup`, `NextQuestInChain`, `SrcItemId`, `SrcItemCount`, `SrcSpell`,"
    //                      27       28         29            30                 31                  32         33                34                35                36
                          "`Title`, `Details`, `Objectives`, `OfferRewardText`, `RequestItemsText`, `EndText`, `ObjectiveText1`, `ObjectiveText2`, `ObjectiveText3`, `ObjectiveText4`,"
    //                      37            38            39            40            41               42               43               44
                          "`ReqItemId1`, `ReqItemId2`, `ReqItemId3`, `ReqItemId4`, `ReqItemCount1`, `ReqItemCount2`, `ReqItemCount3`, `ReqItemCount4`,"
    //                      45              46              47              48              49                 50                 51                 52
                          "`ReqSourceId1`, `ReqSourceId2`, `ReqSourceId3`, `ReqSourceId4`, `ReqSourceCount1`, `ReqSourceCount2`, `ReqSourceCount3`, `ReqSourceCount4`,"
    //                      53                    54                    55                    56                    57                       58                       59                       60
                          "`ReqCreatureOrGOId1`, `ReqCreatureOrGOId2`, `ReqCreatureOrGOId3`, `ReqCreatureOrGOId4`, `ReqCreatureOrGOCount1`, `ReqCreatureOrGOCount2`, `ReqCreatureOrGOCount3`, `ReqCreatureOrGOCount4`,"
    //                      61               62               63               64
                          "`ReqSpellCast1`, `ReqSpellCast2`, `ReqSpellCast3`, `ReqSpellCast4`,"
    //                      65                  66                  67                  68                  69                  70
                          "`RewChoiceItemId1`, `RewChoiceItemId2`, `RewChoiceItemId3`, `RewChoiceItemId4`, `RewChoiceItemId5`, `RewChoiceItemId6`,"
    //                      71                     72                     73                     74                     75                     76
                          "`RewChoiceItemCount1`, `RewChoiceItemCount2`, `RewChoiceItemCount3`, `RewChoiceItemCount4`, `RewChoiceItemCount5`, `RewChoiceItemCount6`,"
    //                      77            78            79            80            81               82               83               84
                          "`RewItemId1`, `RewItemId2`, `RewItemId3`, `RewItemId4`, `RewItemCount1`, `RewItemCount2`, `RewItemCount3`, `RewItemCount4`,"
    //                      85                86                87                88                89                90              91              92              93              94
                          "`RewRepFaction1`, `RewRepFaction2`, `RewRepFaction3`, `RewRepFaction4`, `RewRepFaction5`, `RewRepValue1`, `RewRepValue2`, `RewRepValue3`, `RewRepValue4`, `RewRepValue5`,"
    //                      95               96                  97          98              99                   100                 101           102       103       104
                          "`RewOrReqMoney`, `RewMoneyMaxLevel`, `RewSpell`, `RewSpellCast`, `RewMailTemplateId`, `RewMailDelaySecs`, `PointMapId`, `PointX`, `PointY`, `PointOpt`,"
    //                      105              106              107              108              109                   110                   111                   112
                          "`DetailsEmote1`, `DetailsEmote2`, `DetailsEmote3`, `DetailsEmote4`, `DetailsEmoteDelay1`, `DetailsEmoteDelay2`, `DetailsEmoteDelay3`, `DetailsEmoteDelay4`,"
    //                      113                114              115                  116                  117                  118
                          "`IncompleteEmote`, `CompleteEmote`, `OfferRewardEmote1`, `OfferRewardEmote2`, `OfferRewardEmote3`, `OfferRewardEmote4`,"
    //                      119                       120                       121                       122
                          "`OfferRewardEmoteDelay1`, `OfferRewardEmoteDelay2`, `OfferRewardEmoteDelay3`, `OfferRewardEmoteDelay4`,"
    //                      123            124               125         126             127      128
                          "`StartScript`, `CompleteScript`, `MaxLevel`, `RewMailMoney`, `RewXP`, `RequiredCondition` "
                          " FROM `quest_template`"));
    if (!result)
    {
        sLog.outErrorDb("`quest_template` table is empty!");
        return;
    }

    // create multimap previous quest for each existing quest
    // some quests can have many previous maps set by NextQuestId in previous quest
    // for example set of race quests can lead to single not race specific quest

    do
    {
        Field *fields = result->Fetch();

        std::unique_ptr<Quest> newQuest = std::make_unique<Quest>(fields);
        m_QuestTemplatesMap[newQuest->GetQuestId()] = std::move(newQuest);
    }
    while (result->NextRow());

    // Post processing

    std::map<uint32, uint32> usedMailTemplates;

    for (auto & iter : m_QuestTemplatesMap)
    {
        const std::unique_ptr<Quest>& qinfo = iter.second;

        // additional quest integrity checks (GO, creature_template and item_template must be loaded already)

        if (qinfo->GetQuestMethod() >= QUEST_METHOD_LIMIT)
            sLog.outErrorDb("Quest %u has `Method` = %u, expected values are 0, 1 or 2.", qinfo->GetQuestId(), qinfo->GetQuestMethod());

        if (qinfo->m_SpecialFlags > QUEST_SPECIAL_FLAG_DB_ALLOWED)
            sLog.outErrorDb("Quest %u has `SpecialFlags` = %u, above max flags not allowed for database.", qinfo->GetQuestId(), qinfo->m_SpecialFlags);

        if (qinfo->HasQuestFlag(QUEST_FLAGS_AUTO_REWARDED))
        {
            // at auto-reward can be rewarded only RewChoiceItemId[0]
            for (int j = 1; j < QUEST_REWARD_CHOICES_COUNT; ++j)
            {
                if (uint32 id = qinfo->RewChoiceItemId[j])
                {
                    sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = %u but item from `RewChoiceItemId%d` can't be rewarded with quest flag QUEST_FLAGS_AUTO_REWARDED.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest ignore this data
                }
            }
        }

        // client quest log visual (area case)
        if (qinfo->ZoneOrSort > 0)
        {
            if (!AreaEntry::GetById(qinfo->ZoneOrSort))
            {
                sLog.outErrorDb("Quest %u has `ZoneOrSort` = %u (zone case) but zone with this id does not exist.",
                                qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // no changes, quest not dependent from this value but can have problems at client
            }
        }
        // client quest log visual (sort case)
        if (qinfo->ZoneOrSort < 0)
        {
            QuestSortEntry const* qSort = sQuestSortStore.LookupEntry(-int32(qinfo->ZoneOrSort));
            if (!qSort)
            {
                sLog.outErrorDb("Quest %u has `ZoneOrSort` = %i (sort case) but quest sort with this id does not exist.",
                                qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // no changes, quest not dependent from this value but can have problems at client (note some may be 0, we must allow this so no check)
            }

            //check for proper RequiredSkill value (skill case)
            if (uint32 skill_id = SkillByQuestSort(-int32(qinfo->ZoneOrSort)))
            {
                if (qinfo->RequiredSkill != skill_id)
                {
                    sLog.outErrorDb("Quest %u has `ZoneOrSort` = %i but `RequiredSkill` does not have a corresponding value (%u).",
                                    qinfo->GetQuestId(), qinfo->ZoneOrSort, skill_id);
                    //override, and force proper value here?
                }
            }
        }

        // RequiredClasses, can be 0/CLASSMASK_ALL_PLAYABLE to allow any class
        if (qinfo->RequiredClasses)
        {
            if (!(qinfo->RequiredClasses & CLASSMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Quest %u does not contain any playable classes in `RequiredClasses` (%u), value set to 0 (all classes).", qinfo->GetQuestId(), qinfo->RequiredClasses);
                qinfo->RequiredClasses = 0;
            }
        }

        // RequiredRaces, can be 0/RACEMASK_ALL_PLAYABLE to allow any race
        if (qinfo->RequiredRaces)
        {
            if (!(qinfo->RequiredRaces & RACEMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Quest %u does not contain any playable races in `RequiredRaces` (%u), value set to 0 (all races).", qinfo->GetQuestId(), qinfo->RequiredRaces);
                qinfo->RequiredRaces = 0;
            }
        }

        // RequiredSkill, can be 0
        if (qinfo->RequiredSkill)
        {
            if (!sSkillLineStore.LookupEntry(qinfo->RequiredSkill))
            {
                sLog.outErrorDb("Quest %u has `RequiredSkill` = %u but this skill does not exist",
                                qinfo->GetQuestId(), qinfo->RequiredSkill);
            }
        }

        if (qinfo->RequiredSkillValue)
        {
            if (qinfo->RequiredSkillValue > sWorld.GetConfigMaxSkillValue())
            {
                sLog.outErrorDb("Quest %u has `RequiredSkillValue` = %u but max possible skill is %u, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->RequiredSkillValue, sWorld.GetConfigMaxSkillValue());
                // no changes, quest can't be done for this requirement
            }
        }
        // else Skill quests can have 0 skill level, this is ok

        if (qinfo->RepObjectiveFaction && !GetFactionEntry(qinfo->RepObjectiveFaction))
        {
            sLog.outErrorDb("Quest %u has `RepObjectiveFaction` = %u but faction template %u does not exist, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RepObjectiveFaction, qinfo->RepObjectiveFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepFaction && !GetFactionEntry(qinfo->RequiredMinRepFaction))
        {
            sLog.outErrorDb("Quest %u has `RequiredMinRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepFaction, qinfo->RequiredMinRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMaxRepFaction && !GetFactionEntry(qinfo->RequiredMaxRepFaction))
        {
            sLog.outErrorDb("Quest %u has `RequiredMaxRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepFaction, qinfo->RequiredMaxRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMinRepValue > ReputationMgr::Reputation_Cap)
        {
            sLog.outErrorDb("Quest %u has `RequiredMinRepValue` = %d but max reputation is %u, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepValue, ReputationMgr::Reputation_Cap);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMaxRepValue && qinfo->RequiredMaxRepValue <= qinfo->RequiredMinRepValue)
        {
            sLog.outErrorDb("Quest %u has `RequiredMaxRepValue` = %d and `RequiredMinRepValue` = %d, quest can't be done.",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepValue, qinfo->RequiredMinRepValue);
            // no changes, quest can't be done for this requirement
        }

        if (!qinfo->RepObjectiveFaction && qinfo->RepObjectiveValue > 0)
        {
            sLog.outErrorDb("Quest %u has `RepObjectiveValue` = %d but `RepObjectiveFaction` is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RepObjectiveValue);
            // warning
        }

        if (!qinfo->RequiredMinRepFaction && qinfo->RequiredMinRepValue > 0)
        {
            sLog.outErrorDb("Quest %u has `RequiredMinRepValue` = %d but `RequiredMinRepFaction` is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepValue);
            // warning
        }

        if (!qinfo->RequiredMaxRepFaction && qinfo->RequiredMaxRepValue > 0)
        {
            sLog.outErrorDb("Quest %u has `RequiredMaxRepValue` = %d but `RequiredMaxRepFaction` is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepValue);
            // warning
        }

        if (qinfo->SrcItemId)
        {
            if (ItemPrototype const* pItemProto = GetItemPrototype(qinfo->SrcItemId))
            {
                pItemProto->Discovered = true; // all quest items count as discovered
                if (qinfo->SrcItemCount == 0)
                {
                    sLog.outErrorDb("Quest %u has `SrcItemId` = %u but `SrcItemCount` = 0, set to 1 but need fix in DB.",
                        qinfo->GetQuestId(), qinfo->SrcItemId);
                    qinfo->SrcItemCount = 1;                    // update to 1 for allow quest work for backward compatibility with DB
                }
            }
            else
            {
                sLog.outErrorDb("Quest %u has `SrcItemId` = %u but item with entry %u does not exist, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->SrcItemId, qinfo->SrcItemId);
                qinfo->SrcItemId = 0;                       // quest can't be done for this requirement
            }
        }
        else if (qinfo->SrcItemCount > 0)
        {
            sLog.outErrorDb("Quest %u has `SrcItemId` = 0 but `SrcItemCount` = %u, useless value.",
                            qinfo->GetQuestId(), qinfo->SrcItemCount);
            qinfo->SrcItemCount = 0;                        // no quest work changes in fact
        }

        if (qinfo->SrcSpell)
        {
            SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(qinfo->SrcSpell);
            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has `SrcSpell` = %u but spell %u doesn't exist, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->SrcSpell, qinfo->SrcSpell);
                qinfo->SrcSpell = 0;                        // quest can't be done for this requirement
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has `SrcSpell` = %u but spell %u is broken, quest can't be done.",
                                qinfo->GetQuestId(), qinfo->SrcSpell, qinfo->SrcSpell);
                qinfo->SrcSpell = 0;                        // quest can't be done for this requirement
            }
        }

        for (int j = 0; j < QUEST_ITEM_OBJECTIVES_COUNT; ++j)
        {
            if (uint32 id = qinfo->ReqItemId[j])
            {
                if (qinfo->ReqItemCount[j] == 0)
                {
                    sLog.outErrorDb("Quest %u has `ReqItemId%d` = %u but `ReqItemCount%d` = 0, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done for this requirement
                }

                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_DELIVER);

                if (ItemPrototype const* pItemProto = GetItemPrototype(id))
                    pItemProto->Discovered = true;
                else
                {
                    sLog.outErrorDb("Quest %u has `ReqItemId%d` = %u but item with entry %u does not exist, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->ReqItemCount[j] = 0;             // prevent incorrect work of quest
                }
            }
            else if (qinfo->ReqItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `ReqItemId%d` = 0 but `ReqItemCount%d` = %u, quest can't be done.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqItemCount[j]);
                qinfo->ReqItemCount[j] = 0;                 // prevent incorrect work of quest
            }
        }

        for (int j = 0; j < QUEST_SOURCE_ITEM_IDS_COUNT; ++j)
        {
            if (uint32 id = qinfo->ReqSourceId[j])
            {
                if (ItemPrototype const* pItemProto = GetItemPrototype(id))
                    pItemProto->Discovered = true;
                else
                {
                    sLog.outErrorDb("Quest %u has `ReqSourceId%d` = %u but item with entry %u does not exist, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    // no changes, quest can't be done for this requirement
                }
            }
            else
            {
                if (qinfo->ReqSourceCount[j] > 0)
                {
                    sLog.outErrorDb("Quest %u has `ReqSourceId%d` = 0 but `ReqSourceCount%d` = %u.",
                                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqSourceCount[j]);
                    // no changes, quest ignore this data
                }
            }
        }

        for (int j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            if (uint32 id = qinfo->ReqSpell[j])
            {
                SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(id);
                if (!spellInfo)
                {
                    sLog.outErrorDb("Quest %u has `ReqSpellCast%d` = %u but spell %u does not exist, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    continue;
                }

                if (!qinfo->ReqCreatureOrGOId[j])
                {
                    bool found = false;
                    for (int k = 0; k < MAX_EFFECT_INDEX; ++k)
                    {
                        if ((spellInfo->Effect[k] == SPELL_EFFECT_QUEST_COMPLETE && uint32(spellInfo->EffectMiscValue[k]) == qinfo->QuestId) ||
                                spellInfo->Effect[k] == SPELL_EFFECT_SEND_EVENT)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found)
                    {
                        if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT))
                        {
                            sLog.outErrorDb("Spell (id: %u) have SPELL_EFFECT_QUEST_COMPLETE or SPELL_EFFECT_SEND_EVENT for quest %u and ReqCreatureOrGOId%d = 0, but quest not have flag QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT. Quest flags or ReqCreatureOrGOId%d must be fixed, quest modified to enable objective.", spellInfo->Id, qinfo->QuestId, j + 1, j + 1);

                            // this will prevent quest completing without objective
                            const_cast<std::unique_ptr<Quest>&>(qinfo)->SetSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT);
                        }
                    }
                    else
                    {
                        sLog.outErrorDb("Quest %u has `ReqSpellCast%d` = %u and ReqCreatureOrGOId%d = 0 but spell %u does not have SPELL_EFFECT_QUEST_COMPLETE or SPELL_EFFECT_SEND_EVENT effect for this quest, quest can't be done.",
                                        qinfo->GetQuestId(), j + 1, id, j + 1, id);
                        // no changes, quest can't be done for this requirement
                    }
                }
            }
        }

        for (int j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            int32 id = qinfo->ReqCreatureOrGOId[j];
            if (id < 0 && !GetGameObjectInfo(-id))
            {
                sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = %i but gameobject %u does not exist, quest can't be done.",
                                qinfo->GetQuestId(), j + 1, id, uint32(-id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id > 0 && !GetCreatureTemplate(id))
            {
                sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = %i but creature with entry %u does not exist, quest can't be done.",
                                qinfo->GetQuestId(), j + 1, id, uint32(id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id)
            {
                // In fact SpeakTo and Kill are quite same: either you can speak to mob:SpeakTo or you can't:Kill/Cast

                qinfo->SetSpecialFlag(QuestSpecialFlags(QUEST_SPECIAL_FLAG_KILL_OR_CAST | QUEST_SPECIAL_FLAG_SPEAKTO));

                if (!qinfo->ReqCreatureOrGOCount[j])
                {
                    sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = %u but `ReqCreatureOrGOCount%d` = 0, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can be incorrectly done, but we already report this
                }
            }
            else if (qinfo->ReqCreatureOrGOCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `ReqCreatureOrGOId%d` = 0 but `ReqCreatureOrGOCount%d` = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqCreatureOrGOCount[j]);
                // no changes, quest ignore this data
            }
        }

        bool choice_found = false;
        for (int j = QUEST_REWARD_CHOICES_COUNT - 1; j >= 0; --j)
        {
            if (uint32 id = qinfo->RewChoiceItemId[j])
            {
                if (ItemPrototype const* pItemProto = GetItemPrototype(id))
                {
                    choice_found = true;
                    pItemProto->Discovered = true;
                }
                else
                {
                    sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewChoiceItemId[j] = 0;          // no changes, quest will not reward this
                }

                if (!qinfo->RewChoiceItemCount[j])
                {
                    sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = %u but `RewChoiceItemCount%d` = 0, quest can't be done.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done
                }
            }
            else if (choice_found)                          // 1.12.1 client (but not later) crash if have gap in item reward choices
            {
                sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = 0 but `RewChoiceItemId%d` = %u, client can crash at like data.",
                                qinfo->GetQuestId(), j + 1, j + 2, qinfo->RewChoiceItemId[j + 1]);
                // fill gap by clone later filled choice
                qinfo->RewChoiceItemId[j] = qinfo->RewChoiceItemId[j + 1];
                qinfo->RewChoiceItemCount[j] = qinfo->RewChoiceItemCount[j + 1];
            }
            else if (qinfo->RewChoiceItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `RewChoiceItemId%d` = 0 but `RewChoiceItemCount%d` = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewChoiceItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (int j = 0; j < QUEST_REWARDS_COUNT; ++j)
        {
            if (uint32 id = qinfo->RewItemId[j])
            {
                if (ItemPrototype const* pItemProto = GetItemPrototype(id))
                    pItemProto->Discovered = true;
                else
                {
                    sLog.outErrorDb("Quest %u has `RewItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewItemId[j] = 0;                // no changes, quest will not reward this item
                }

                if (!qinfo->RewItemCount[j])
                {
                    sLog.outErrorDb("Quest %u has `RewItemId%d` = %u but `RewItemCount%d` = 0, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes
                }
            }
            else if (qinfo->RewItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has `RewItemId%d` = 0 but `RewItemCount%d` = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (int j = 0; j < QUEST_REPUTATIONS_COUNT; ++j)
        {
            if (qinfo->RewRepFaction[j])
            {
                if (!qinfo->RewRepValue[j])
                {
                    sLog.outErrorDb("Quest %u has `RewRepFaction%d` = %u but `RewRepValue%d` = 0, quest will not reward this reputation.",
                                    qinfo->GetQuestId(), j + 1, qinfo->RewRepValue[j], j + 1);
                    // no changes
                }

                if (!GetFactionEntry(qinfo->RewRepFaction[j]))
                {
                    sLog.outErrorDb("Quest %u has `RewRepFaction%d` = %u but raw faction (faction.dbc) %u does not exist, quest will not reward reputation for this faction.",
                                    qinfo->GetQuestId(), j + 1, qinfo->RewRepFaction[j] , qinfo->RewRepFaction[j]);
                    qinfo->RewRepFaction[j] = 0;            // quest will not reward this
                }
            }
            else if (qinfo->RewRepValue[j] != 0)
            {
                sLog.outErrorDb("Quest %u has `RewRepFaction%d` = 0 but `RewRepValue%d` = %i.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewRepValue[j]);
                // no changes, quest ignore this data
            }
        }

        if (qinfo->RewSpell)
        {
            SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(qinfo->RewSpell);

            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u does not exist, spell removed as display reward.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u is broken, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }
            else if (GetTalentSpellCost(qinfo->RewSpell))
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u is talent, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }
        }

        if (qinfo->RewSpellCast)
        {
            SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(qinfo->RewSpellCast);

            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has `RewSpellCast` = %u but spell %u does not exist, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has `RewSpellCast` = %u but spell %u is broken, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }
            else if (GetTalentSpellCost(qinfo->RewSpellCast))
            {
                sLog.outErrorDb("Quest %u has `RewSpell` = %u but spell %u is talent, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }
        }

        if (qinfo->RewMailTemplateId)
        {
            uint32 mailTemplateId = abs(qinfo->RewMailTemplateId);
            if (!sMailTemplateStorage.LookupEntry<MailTemplateEntry>(mailTemplateId))
            {
                sLog.outErrorDb("Quest %u has `RewMailTemplateId` = %u but mail template  %u does not exist, quest will not have a mail reward.",
                                qinfo->GetQuestId(), mailTemplateId, mailTemplateId);
                qinfo->RewMailTemplateId = 0;               // no mail will send to player
                qinfo->RewMailDelaySecs = 0;                // no mail will send to player
            }
            else if (usedMailTemplates.find(mailTemplateId) != usedMailTemplates.end())
            {
                std::map<uint32, uint32>::const_iterator used_mt_itr = usedMailTemplates.find(mailTemplateId);
                sLog.outErrorDb("Quest %u has `RewMailTemplateId` = %u but mail template  %u already used for quest %u, quest will not have a mail reward.",
                                qinfo->GetQuestId(), mailTemplateId, mailTemplateId, used_mt_itr->second);
                qinfo->RewMailTemplateId = 0;               // no mail will send to player
                qinfo->RewMailDelaySecs = 0;                // no mail will send to player
            }
            else
                usedMailTemplates[mailTemplateId] = qinfo->GetQuestId();
        }

        if (qinfo->NextQuestInChain)
        {
            QuestMap::iterator qNextItr = m_QuestTemplatesMap.find(qinfo->NextQuestInChain);
            if (qNextItr == m_QuestTemplatesMap.end())
            {
                sLog.outErrorDb("Quest %u has `NextQuestInChain` = %u but quest %u does not exist, quest chain will not work.",
                                qinfo->GetQuestId(), qinfo->NextQuestInChain , qinfo->NextQuestInChain);
                qinfo->NextQuestInChain = 0;
            }
            else
                qNextItr->second->prevChainQuests.push_back(qinfo->GetQuestId());
        }

        // fill additional data stores
        if (qinfo->PrevQuestId)
        {
            if (m_QuestTemplatesMap.find(abs(qinfo->GetPrevQuestId())) == m_QuestTemplatesMap.end())
                sLog.outErrorDb("Quest %d has PrevQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetPrevQuestId());
            else
                qinfo->prevQuests.push_back(qinfo->PrevQuestId);
        }

        if (qinfo->NextQuestId)
        {
            QuestMap::iterator qNextItr = m_QuestTemplatesMap.find(abs(qinfo->GetNextQuestId()));
            if (qNextItr == m_QuestTemplatesMap.end())
                sLog.outErrorDb("Quest %d has NextQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetNextQuestId());
            else
            {
                int32 signedQuestId = qinfo->NextQuestId < 0 ? -int32(qinfo->GetQuestId()) : int32(qinfo->GetQuestId());
                qNextItr->second->prevQuests.push_back(signedQuestId);
            }
        }

        if (qinfo->ExclusiveGroup)
            m_ExclusiveQuestGroups.insert(ExclusiveQuestGroupsMap::value_type(qinfo->ExclusiveGroup, qinfo->GetQuestId()));

        if (qinfo->LimitTime)
            qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_TIMED);
    }

    // check QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT for spell with SPELL_EFFECT_QUEST_COMPLETE
    for (uint32 i = 0; i < sSpellMgr.GetMaxSpellId(); ++i)
    {
        SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(i);
        if (!spellInfo)
            continue;

        for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
        {
            if (spellInfo->Effect[j] != SPELL_EFFECT_QUEST_COMPLETE)
                continue;

            uint32 quest_id = spellInfo->EffectMiscValue[j];

            Quest const* quest = GetQuestTemplate(quest_id);

            // some quest referenced in spells not exist (outdated spells)
            if (!quest)
                continue;

            if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT))
            {
                sLog.outDetail("Spell (id: %u) have SPELL_EFFECT_QUEST_COMPLETE for quest %u , but quest does not have SpecialFlags QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT (2) set. Quest SpecialFlags should be corrected to enable this objective.", spellInfo->Id, quest_id);
            }
        }
    }
}

uint32 ObjectMgr::GetQuestStartingItemID(uint32 quest_id) const
{
    auto questItemPair = m_QuestStartingItemsMap.find(quest_id);

    if (questItemPair != m_QuestStartingItemsMap.end())
        return questItemPair->second;

    return 0;
}

void ObjectMgr::LoadQuestLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_QuestLocaleMap.clear();                                // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`,"
                          "`Title_loc1`, `Details_loc1`, `Objectives_loc1`, `OfferRewardText_loc1`, `RequestItemsText_loc1`, `EndText_loc1`, `ObjectiveText1_loc1`, `ObjectiveText2_loc1`, `ObjectiveText3_loc1`, `ObjectiveText4_loc1`,"
                          "`Title_loc2`, `Details_loc2`, `Objectives_loc2`, `OfferRewardText_loc2`, `RequestItemsText_loc2`, `EndText_loc2`, `ObjectiveText1_loc2`, `ObjectiveText2_loc2`, `ObjectiveText3_loc2`, `ObjectiveText4_loc2`,"
                          "`Title_loc3`, `Details_loc3`, `Objectives_loc3`, `OfferRewardText_loc3`, `RequestItemsText_loc3`, `EndText_loc3`, `ObjectiveText1_loc3`, `ObjectiveText2_loc3`, `ObjectiveText3_loc3`, `ObjectiveText4_loc3`,"
                          "`Title_loc4`, `Details_loc4`, `Objectives_loc4`, `OfferRewardText_loc4`, `RequestItemsText_loc4`, `EndText_loc4`, `ObjectiveText1_loc4`, `ObjectiveText2_loc4`, `ObjectiveText3_loc4`, `ObjectiveText4_loc4`,"
                          "`Title_loc5`, `Details_loc5`, `Objectives_loc5`, `OfferRewardText_loc5`, `RequestItemsText_loc5`, `EndText_loc5`, `ObjectiveText1_loc5`, `ObjectiveText2_loc5`, `ObjectiveText3_loc5`, `ObjectiveText4_loc5`,"
                          "`Title_loc6`, `Details_loc6`, `Objectives_loc6`, `OfferRewardText_loc6`, `RequestItemsText_loc6`, `EndText_loc6`, `ObjectiveText1_loc6`, `ObjectiveText2_loc6`, `ObjectiveText3_loc6`, `ObjectiveText4_loc6`,"
                          "`Title_loc7`, `Details_loc7`, `Objectives_loc7`, `OfferRewardText_loc7`, `RequestItemsText_loc7`, `EndText_loc7`, `ObjectiveText1_loc7`, `ObjectiveText2_loc7`, `ObjectiveText3_loc7`, `ObjectiveText4_loc7`,"
                          "`Title_loc8`, `Details_loc8`, `Objectives_loc8`, `OfferRewardText_loc8`, `RequestItemsText_loc8`, `EndText_loc8`, `ObjectiveText1_loc8`, `ObjectiveText2_loc8`, `ObjectiveText3_loc8`, `ObjectiveText4_loc8`"
                          " FROM `locales_quest`"
                                             ));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!GetQuestTemplate(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_quest` has data for nonexistent quest entry %u, skipped.", entry);
            continue;
        }

        QuestLocale& data = m_QuestLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 10 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Title.size() <= idx)
                        data.Title.resize(idx + 1);

                    data.Title[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Details.size() <= idx)
                        data.Details.resize(idx + 1);

                    data.Details[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 2].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Objectives.size() <= idx)
                        data.Objectives.resize(idx + 1);

                    data.Objectives[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 3].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.OfferRewardText.size() <= idx)
                        data.OfferRewardText.resize(idx + 1);

                    data.OfferRewardText[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 4].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.RequestItemsText.size() <= idx)
                        data.RequestItemsText.resize(idx + 1);

                    data.RequestItemsText[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 5].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.EndText.size() <= idx)
                        data.EndText.resize(idx + 1);

                    data.EndText[idx] = str;
                }
            }
            for (int k = 0; k < 4; ++k)
            {
                str = fields[1 + 10 * (i - 1) + 6 + k].GetCppString();
                if (!str.empty())
                {
                    int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        if ((int32)data.ObjectiveText[k].size() <= idx)
                            data.ObjectiveText[k].resize(idx + 1);

                        data.ObjectiveText[k][idx] = str;
                    }
                }
            }
        }
    }
    while (result->NextRow());
}

void ObjectMgr::LoadPetCreateSpells()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `spell1`, `spell2`, `spell3`, `spell4` FROM `petcreateinfo_spell`"));
    if (!result)
    {
        return;
    }

    m_PetCreateSpellMap.clear();

    do
    {
        Field *fields = result->Fetch();

        uint32 creature_id = fields[0].GetUInt32();

        if (!creature_id)
        {
            sLog.outErrorDb("Creature id %u listed in `petcreateinfo_spell` not exist.", creature_id);
            continue;
        }

        CreatureInfo const* cInfo = GetCreatureTemplate(creature_id);
        if (!cInfo)
        {
            sLog.outErrorDb("Creature id %u listed in `petcreateinfo_spell` not exist.", creature_id);
            continue;
        }

        if (CreatureSpellDataEntry const* petSpellEntry = cInfo->pet_spell_list_id ? sCreatureSpellDataStorage.LookupEntry<CreatureSpellDataEntry>(cInfo->pet_spell_list_id) : nullptr)
        {
            sLog.outErrorDb("Creature id %u listed in `petcreateinfo_spell` have set `pet_spell_list_id` field and will use its instead, skip.", creature_id);
            continue;
        }

        PetCreateSpellEntry PetCreateSpell;

        bool have_spell = false;
        bool have_spell_db = false;
        for (int i = 0; i < 4; ++i)
        {
            PetCreateSpell.spellid[i] = fields[i + 1].GetUInt32();

            if (!PetCreateSpell.spellid[i])
                continue;

            have_spell_db = true;

            SpellEntry const* i_spell = sSpellMgr.GetSpellEntry(PetCreateSpell.spellid[i]);
            if (!i_spell)
            {
                sLog.outErrorDb("Spell %u listed in `petcreateinfo_spell` does not exist", PetCreateSpell.spellid[i]);
                PetCreateSpell.spellid[i] = 0;
                continue;
            }

            have_spell = true;
        }

        if (!have_spell_db)
        {
            sLog.outErrorDb("Creature %u listed in `petcreateinfo_spell` have only 0 spell data, why it listed?", creature_id);
            continue;
        }

        if (!have_spell)
            continue;

        m_PetCreateSpellMap[creature_id] = PetCreateSpell;
    }
    while (result->NextRow());

    // cache spell->learn spell map for use in next loop
    std::map<uint32, uint32> learnCache;
    for (uint32 spell_id = 1; spell_id < sSpellMgr.GetMaxSpellId(); ++spell_id)
    {
        SpellEntry const *spellproto = sSpellMgr.GetSpellEntry(spell_id);
        if (!spellproto)
            continue;

        if (spellproto->Effect[0] != SPELL_EFFECT_LEARN_SPELL && spellproto->Effect[0] != SPELL_EFFECT_LEARN_PET_SPELL)
            continue;

        if (!spellproto->EffectTriggerSpell[0])
            continue;

        learnCache[spellproto->EffectTriggerSpell[0]] = spellproto->Id;
    }

    // fill data from DBC as more correct source if available
    for (auto const& itr : m_creatureInfoMap)
    {
        CreatureInfo const* cInfo = itr.second.get();
        if (!cInfo)
            continue;

        CreatureSpellDataEntry const* petSpellEntry = cInfo->pet_spell_list_id ? sCreatureSpellDataStorage.LookupEntry<CreatureSpellDataEntry>(cInfo->pet_spell_list_id) : nullptr;
        if (!petSpellEntry)
            continue;

        PetCreateSpellEntry PetCreateSpell;
        for (int i = 0; i < MAX_CREATURE_SPELL_DATA_SLOT; ++i)
        {
            uint32 petspell_id = petSpellEntry->spellId[i];
            if (petspell_id)
            {
                // in dbc stored spell for pet use, but for teaching work we need learn spell ids
                std::map<uint32, uint32>::const_iterator cache_itr = learnCache.find(petspell_id);
                if (cache_itr != learnCache.end())
                    petspell_id = cache_itr->second;
            }

            PetCreateSpell.spellid[i] = petspell_id;
        }

        m_PetCreateSpellMap[itr.first] = PetCreateSpell;
    }
}

void ObjectMgr::LoadPetSpellData()
{
    sCreatureSpellDataStorage.Load();
}

void ObjectMgr::LoadItemTexts()
{
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `id`, `text` FROM `item_text`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        m_ItemTextsMap[ fields[0].GetUInt32() ] = fields[1].GetCppString();
    }
    while (result->NextRow());
}

void ObjectMgr::LoadPageTexts()
{
    sPageTextStore.Load();

    for (uint32 i = 1; i < sPageTextStore.GetMaxEntry(); ++i)
    {
        // check data correctness
        PageText const* page = sPageTextStore.LookupEntry<PageText>(i);
        if (!page)
            continue;

        if (page->next_page && !sPageTextStore.LookupEntry<PageText>(page->next_page))
        {
            sLog.outErrorDb("Page text (Id: %u) has not existing next page (Id:%u)", i, page->next_page);
            continue;
        }

        // detect circular reference
        std::set<uint32> checkedPages;
        for (PageText const* pageItr = page; pageItr; pageItr = sPageTextStore.LookupEntry<PageText>(pageItr->next_page))
        {
            if (!pageItr->next_page)
                break;
            checkedPages.insert(pageItr->entry);
            if (checkedPages.find(pageItr->next_page) != checkedPages.end())
            {
                std::ostringstream ss;
                ss << "The text page(s) ";
                for (const auto checkedPage : checkedPages)
                    ss << checkedPage << " ";
                ss << "create(s) a circular reference, which can cause the server to freeze. Changing next_page of page "
                   << pageItr->entry << " to 0";
                sLog.outErrorDb("%s", ss.str().c_str());
                const_cast<PageText*>(pageItr)->next_page = 0;
                break;
            }
        }
    }
}

void ObjectMgr::LoadPageTextLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_PageTextLocaleMap.clear();                             // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8` FROM `locales_page_text`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!sPageTextStore.LookupEntry<PageText>(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_page_text` has data for nonexistent page text entry %u, skipped.", entry);
            continue;
        }

        PageTextLocale& data = m_PageTextLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (str.empty())
                continue;

            int idx = GetOrNewIndexForLocale(LocaleConstant(i));
            if (idx >= 0)
            {
                if ((int32)data.text.size() <= idx)
                    data.text.resize(idx + 1);

                data.text[idx] = str;
            }
        }
    }
    while (result->NextRow());
}

struct SQLMapLoader : public SQLStorageLoaderBase<SQLMapLoader, SQLStorage>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char const* src, D& dst)
    {
        dst = D(sScriptMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadMapTemplate()
{
    SQLMapLoader loader;
    loader.Load(sMapStorage);

    for (auto itr = sMapStorage.begin<MapEntry>(); itr < sMapStorage.end<MapEntry>(); ++itr)
    {
        if (itr->IsDungeon() && itr->parent > 0)
        {
            // check existence
            MapEntry const* parentEntry = sMapStorage.LookupEntry<MapEntry>(itr->parent);
            if (!parentEntry)
            {
                sLog.outErrorDb("ObjectMgr::LoadMapTemplate: bad parent map id %u for instance template %u template!",
                    itr->parent, itr->id);
                const_cast<MapEntry*>(*itr)->parent = 0;
                continue;
            }

            if (parentEntry->IsContinent())
            {
                sLog.outErrorDb("ObjectMgr::LoadMapTemplate: parent point to continent map id %u for instance template %u template, ignored, need be set only for non-continent parents!",
                                parentEntry->id, itr->id);
                const_cast<MapEntry*>(*itr)->parent = 0;
                continue;
            }
        }

        // if ghost entrance coordinates provided, can't be not exist for instance without ground entrance
        if (itr->ghostEntranceMap >= 0)
        {
            if (!MapManager::IsValidMapCoord(itr->ghostEntranceMap, itr->ghostEntranceX, itr->ghostEntranceY))
            {
                sLog.outErrorDb("ObjectMgr::LoadMapTemplate: ghost entrance coordinates invalid for instance template %u template, ignored, need be set only for non-continent parents!", itr->id);
                sMapStorage.EraseEntry(itr->id);
                continue;
            }

            MapEntry const* ghostEntry = sMapStorage.LookupEntry<MapEntry>(itr->ghostEntranceMap);
            if (!ghostEntry)
            {
                sLog.outErrorDb("ObjectMgr::LoadMapTemplate: bad ghost entrance for instance template %u template!", itr->id);
                sMapStorage.EraseEntry(itr->id);
                continue;
            }

            if (!ghostEntry->IsContinent())
            {
                sLog.outErrorDb("ObjectMgr::LoadMapTemplate: ghost entrance not at continent map id %u for instance template %u template, ignored, need be set only for non-continent parents!", ghostEntry->id, itr->id);
                sMapStorage.EraseEntry(itr->id);
                continue;
            }
        }

        // the reset_delay must be at least one day
        if (itr->resetDelay)
            const_cast<MapEntry*>(*itr)->resetDelay = std::max((uint32)1, (uint32)(itr->resetDelay * sWorld.getConfig(CONFIG_FLOAT_RATE_INSTANCE_RESET_TIME)));
    }
}

struct SQLWorldLoader : public SQLStorageLoaderBase<SQLWorldLoader, SQLStorage>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char const* src, D& dst)
    {
        dst = D(sScriptMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadNPCText()
{
    m_NpcTextMap.clear();                           // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `ID`, "
        "`Probability0`, `Probability1`, `Probability2`, `Probability3`, `Probability4`, `Probability5`, `Probability6`, `Probability7`, "
        "`BroadcastTextID0`, `BroadcastTextID1`, `BroadcastTextID2`, `BroadcastTextID3`, `BroadcastTextID4`, `BroadcastTextID5`, `BroadcastTextID6`, `BroadcastTextID7`"
        " FROM `npc_text`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 textID = fields[0].GetUInt32();
        if (!textID)
        {
            sLog.outErrorDb("Table `npc_text` has record wit reserved id 0, ignore.");
            continue;
        }

        NpcText& npcText = m_NpcTextMap[textID];

        for (uint8 i = 0; i < MAX_NPC_TEXT_OPTIONS; ++i)
        {
            npcText.Options[i].Probability = fields[1 + i].GetFloat();
            npcText.Options[i].BroadcastTextID = fields[9 + i].GetUInt32();
        }

        for (uint8 i = 0; i < MAX_NPC_TEXT_OPTIONS; i++)
        {
            if (npcText.Options[i].BroadcastTextID)
            {
                if (!GetBroadcastTextLocale(npcText.Options[i].BroadcastTextID))
                {
                    sLog.outErrorDb("NPCText (ID: %u) has a non-existing or incompatible BroadcastText (ID: %u, Index: %u)", textID, npcText.Options[i].BroadcastTextID, i);
                    npcText.Options[i].BroadcastTextID = 0;
                }
            }
        }

        for (uint8 i = 0; i < MAX_NPC_TEXT_OPTIONS; i++)
        {
            if (npcText.Options[i].Probability > 0 && npcText.Options[i].BroadcastTextID == 0)
            {
                sLog.outErrorDb("NPCText (ID: %u) has a probability (Index: %u) set, but no BroadcastTextID to go with it", textID, i);
                npcText.Options[i].Probability = 0;
            }
        }
    }
    while (result->NextRow());
}

class SingleMailReturner
{
public:
    SingleMailReturner() : returnToLowGuid(0) {}
    uint32 returnToLowGuid; // Items deleted if null
    time_t basetime;
    uint32 messageID;
    ObjectGuid receiverGuid;
    uint32 itemTextId;

    void Callback(QueryResult* result)
    {
        uint32 item_guid = 0;
        if (result)
        {
            Field *fields2 = result->Fetch();

            item_guid = fields2[0].GetUInt32();
            delete result;
        }
        if (!sObjectAccessor.FindPlayerNotInWorld(receiverGuid)) // Do not process online players!
        {
            if (!returnToLowGuid) // Delete mail and items
            {
                // mail open and then not returned
                if (item_guid)
                {
                    CharacterDatabase.PExecute("DELETE FROM `item_instance` WHERE `guid` = '%u'", item_guid);
                    CharacterDatabase.PExecute("DELETE FROM `mail_items` WHERE `mail_id` = '%u'", messageID);
                }
                if (itemTextId)
                    CharacterDatabase.PExecute("DELETE FROM `item_text` WHERE `id` = '%u'", itemTextId);

                CharacterDatabase.PExecute("UPDATE `mail` SET `isDeleted` = 1 WHERE `id` = '%u'", messageID);
                sLog.out(LOG_MAIL_AH, "About to DELETE mail id %u with receiver LowGUID %u and item GUID %u", messageID, receiverGuid.GetCounter(),item_guid);
            }
            else                // Return to sender
            {
                // mail will be returned:
                CharacterDatabase.PExecute("UPDATE `mail` SET `sender` = '%u', `receiver` = '%u', `expire_time` = '" UI64FMTD "', `deliver_time` = '" UI64FMTD "', `cod` = '0', `checked` = '%u' WHERE `id` = '%u'",
                                           receiverGuid.GetCounter(), returnToLowGuid, (uint64)(basetime + 30 * DAY), (uint64)basetime, MAIL_CHECK_MASK_RETURNED, messageID);
                if (item_guid)
                {
                    // update receiver in mail items for its proper delivery, and in instance_item for avoid lost item at sender delete
                    CharacterDatabase.PExecute("UPDATE `mail_items` SET `receiver` = %u WHERE `item_guid` = '%u'", returnToLowGuid, item_guid);
                    CharacterDatabase.PExecute("UPDATE `item_instance` SET `owner_guid` = %u WHERE `guid` = '%u'", returnToLowGuid, item_guid);
                }
            }
        }
        delete this;
    }
};

class OldMailsReturner
{
public:
    OldMailsReturner() : serverUp(false), basetime(0) {}
    bool serverUp;
    time_t basetime;
    void Callback(QueryResult* result)
    {
        if (!result)
        {
            sObjectMgr.ResetOldMailCounter();
            delete this;
            return;                                             // any mails need to be returned or deleted
        }

        uint32 skippedCount = 0;

        do
        {
            Field * fields = result->Fetch();

            Mail *m = new Mail;
            m->messageID = fields[0].GetUInt32();
            m->messageType = fields[1].GetUInt8();
            m->sender = fields[2].GetUInt32();
            m->receiverGuid = ObjectGuid(HIGHGUID_PLAYER, fields[3].GetUInt32());
            bool has_items = fields[5].GetBool();
            m->expire_time = (time_t)fields[6].GetUInt64();
            m->deliver_time = 0;
            m->COD = fields[7].GetUInt32();
            m->checked = fields[8].GetUInt32();
            m->mailTemplateId = fields[9].GetInt16();

            if (serverUp && sObjectAccessor.FindPlayerNotInWorld(m->receiverGuid))
            {
                // Online player. We wait for him to logout to send the mail back (ie next call)
                ++skippedCount;
                delete m;
                continue;
            }
            //delete or return mail:
            if (has_items)
            {
                SingleMailReturner* returner = new SingleMailReturner();
                // if it is mail from non-player, or if it's already return mail, it shouldn't be returned, but deleted
                if (m->messageType != MAIL_NORMAL || (m->checked & (MAIL_CHECK_MASK_COD_PAYMENT | MAIL_CHECK_MASK_RETURNED)))
                    returner->returnToLowGuid = 0;
                else
                {
                    returner->basetime = basetime;
                    returner->returnToLowGuid = m->sender;
                }
                returner->receiverGuid = m->receiverGuid;
                returner->itemTextId = m->itemTextId;
                returner->messageID = m->messageID;
                CharacterDatabase.AsyncPQueryUnsafe(returner, &SingleMailReturner::Callback, "SELECT `item_guid` FROM `mail_items` WHERE `mail_id`='%u'", m->messageID);
                delete m;
                continue;
            }

            if (m->itemTextId)
                CharacterDatabase.PExecute("DELETE FROM `item_text` WHERE `id` = '%u'", m->itemTextId);

            // deletemail = true;
            // delmails << m->messageID << ", ";
            sLog.out(LOG_MAIL_AH, "About to DELETE mail id %u with receiver LowGUID %u and sender LowGUID %u", m->messageID, m->receiverGuid.GetCounter(), m->sender);
            CharacterDatabase.PExecute("UPDATE `mail` SET `isDeleted` = 1 WHERE `id` = '%u'", m->messageID);
            delete m;
            
        }
        while (result->NextRow());
        sObjectMgr.IncrementOldMailCounter(skippedCount);
        delete result;
        delete this;
    }
};

//not very fast function but it is called only once a day, or on starting-up
void ObjectMgr::ReturnOrDeleteOldMails(bool serverUp)
{
    time_t basetime = time(nullptr);
    DEBUG_LOG("Returning mails current time: hour: %d, minute: %d, second: %d ", localtime(&basetime)->tm_hour, localtime(&basetime)->tm_min, localtime(&basetime)->tm_sec);
    //delete all old mails without item and without body immediately, if starting server
    if (!serverUp)
        CharacterDatabase.PExecute("UPDATE `mail` SET `isDeleted` = 1 WHERE `expire_time` < '" UI64FMTD "' AND `has_items` = '0' AND `itemTextId` = 0", (uint64)basetime);
    OldMailsReturner* cb = new OldMailsReturner();
    cb->serverUp = serverUp;
    cb->basetime = basetime;
    uint32 limit = serverUp ? 300 : 1000;
    CharacterDatabase.AsyncPQueryUnsafe(cb, &OldMailsReturner::Callback, "SELECT `id`, `messageType`, `sender`, `receiver`, `itemTextId`, `has_items`, `expire_time`, `cod`, `checked`, `mailTemplateId` FROM `mail` WHERE `expire_time` < '" UI64FMTD "' AND isDeleted = 0 LIMIT %u", (uint64)basetime, limit);
}

void ObjectMgr::LoadAreaTriggers()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT * FROM `areatrigger_template`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        AreaTriggerEntry areaTrigger;

        uint32 triggerId = fields[0].GetUInt32();

        areaTrigger.id = triggerId;
        areaTrigger.mapid = fields[1].GetUInt32();
        areaTrigger.x = fields[2].GetFloat();
        areaTrigger.y = fields[3].GetFloat();
        areaTrigger.z = fields[4].GetFloat();
        areaTrigger.radius = fields[5].GetFloat();
        areaTrigger.box_x = fields[6].GetFloat();
        areaTrigger.box_y = fields[7].GetFloat();
        areaTrigger.box_z = fields[8].GetFloat();
        areaTrigger.box_orientation = fields[9].GetFloat();

        m_AreaTriggersMap[triggerId] = areaTrigger;

    } while (result->NextRow());
}

void ObjectMgr::LoadQuestAreaTriggers()
{
    m_QuestAreaTriggerMap.clear();                           // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `id`, `quest` FROM `areatrigger_involvedrelation`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 trigger_ID = fields[0].GetUInt32();
        uint32 quest_ID   = fields[1].GetUInt32();

        AreaTriggerEntry const* atEntry = GetAreaTrigger(trigger_ID);
        if (!atEntry)
        {
            if (!IsExistingAreaTriggerId(trigger_ID))
                sLog.outErrorDb("Table `areatrigger_involvedrelation` has area trigger (ID: %u) not listed in `AreaTrigger.dbc`.", trigger_ID);

            continue;
        }

        Quest const* quest = GetQuestTemplate(quest_ID);
        if (!quest)
        {
            if (!sObjectMgr.IsExistingQuestId(quest_ID))
                sLog.outErrorDb("Table `areatrigger_involvedrelation` has record (id: %u) for not existing quest %u", trigger_ID, quest_ID);
            continue;
        }

        if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT))
        {
            sLog.outErrorDb("Table `areatrigger_involvedrelation` has record (id: %u) for not quest %u, but quest not have flag QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT. Trigger or quest flags must be fixed, quest modified to require objective.", trigger_ID, quest_ID);

            // this will prevent quest completing without objective
            const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT);

            // continue; - quest modified to required objective and trigger can be allowed.
        }

        m_QuestAreaTriggerMap[trigger_ID] = quest_ID;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadTavernAreaTriggers()
{
    m_TavernAreaTriggerSet.clear();                          // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `id` FROM `areatrigger_tavern`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 triggerId = fields[0].GetUInt32();

        AreaTriggerEntry const* atEntry = GetAreaTrigger(triggerId);
        if (!atEntry)
        {
            sLog.outErrorDb("Table `areatrigger_tavern` has area trigger (ID:%u) not listed in `AreaTrigger.dbc`.", triggerId);
            continue;
        }

        m_TavernAreaTriggerSet.insert(triggerId);
    } while (result->NextRow());
}

void ObjectMgr::LoadBattlegroundEntranceTriggers()
{
    m_BGEntranceTriggersMap.clear();                                  // need for reload case

    //                                                               0     1       2              3           4                  5                  6                  7
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `id`, `team`, `bg_template`, `exit_map`, `exit_position_x`, `exit_position_y`, `exit_position_z`, `exit_orientation` FROM `areatrigger_bg_entrance`"));
    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();
        BattlegroundEntranceTrigger bget;

        uint32 triggerId        = fields[0].GetUInt32();
        uint32 team             = fields[1].GetUInt32();
        uint8 bgTypeId          = fields[2].GetUInt8();

        bget.exit_mapId         = fields[3].GetUInt32();
        bget.exit_X             = fields[4].GetFloat();
        bget.exit_Y             = fields[5].GetFloat();
        bget.exit_Z             = fields[6].GetFloat();
        bget.exit_Orientation   = fields[7].GetFloat();

        AreaTriggerEntry const* bgetEntry = GetAreaTrigger(triggerId);
        if (!bgetEntry)
        {
            if (!IsExistingAreaTriggerId(triggerId))
                sLog.outErrorDb("Table `areatrigger_bg_entrance` has area trigger (ID:%u) not listed in `AreaTrigger.dbc`.", triggerId);
            continue;
        }

        if (team != ALLIANCE && team != HORDE)
        {
            sLog.outError("Table `areatrigger_bg_entrance` has team (ID:%u) that is not Horde or Alliance for area trigger (ID:%u).", team, triggerId);
            continue;
        }
        bget.team = Team(team);

        if (bgTypeId >= MAX_BATTLEGROUND_TYPE_ID)
        {
            sLog.outErrorDb("Table `areatrigger_bg_entrance` has nonexistent battleground type (ID:%u) for area trigger (ID:%u), ignored.", bgTypeId, triggerId);
            continue;
        }
        bget.bgTypeId = BattleGroundTypeId(bgTypeId);

        MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(bget.exit_mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Table `areatrigger_bg_entrance` has nonexistent exit map (ID: %u) for area trigger (ID:%u).", bget.exit_mapId, triggerId);
            continue;
        }

        if (bget.exit_X == 0 && bget.exit_Y == 0 && bget.exit_Z == 0)
        {
            sLog.outErrorDb("Table `areatrigger_bg_entrance` has area trigger (ID:%u) without battleground exit coordinates.", triggerId);
            continue;
        }

        m_BGEntranceTriggersMap[triggerId] = bget;
    }
    while (result->NextRow());
}

uint32 ObjectMgr::GetNearestTaxiNode(float x, float y, float z, uint32 mapid, Team team, std::optional<std::function<bool(const TaxiNodesEntry*)>> pred)
{
    bool found = false;
    float dist = 0.0f;
    uint32 id = 0;

    for (uint32 i = 1; i < GetMaxTaxiNodeId(); ++i)
    {
        TaxiNodesEntry const* node = GetTaxiNodeEntry(i);
        if (!node || node->map_id != mapid || !node->MountCreatureID[team == ALLIANCE ? 1 : 0])
            continue;

        uint8  field   = (uint8)((i - 1) / 32);
        uint32 submask = 1 << ((i - 1) % 32);

        if (field >= sTaxiNodesMask.size())
            continue;

        // skip not taxi network nodes
        if ((sTaxiNodesMask[field] & submask) == 0)
            continue;

        if (pred && !(*pred)(node))
            continue;

        float dist2 = (node->x - x) * (node->x - x) + (node->y - y) * (node->y - y) + (node->z - z) * (node->z - z);
        if (found)
        {
            if (dist2 < dist)
            {
                dist = dist2;
                id = i;
            }
        }
        else
        {
            found = true;
            dist = dist2;
            id = i;
        }
    }

    return id;
}

void ObjectMgr::GetTaxiPath(uint32 source, uint32 destination, uint32 &path, uint32 &cost)
{
    TaxiPathSetBySource::iterator src_i = sTaxiPathSetBySource.find(source);
    if (src_i == sTaxiPathSetBySource.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    TaxiPathSetForSource& pathSet = src_i->second;

    TaxiPathSetForSource::iterator dest_i = pathSet.find(destination);
    if (dest_i == pathSet.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    cost = dest_i->second.price;
    path = dest_i->second.ID;
}

uint32 ObjectMgr::GetTaxiMountDisplayId(uint32 id, Team team, bool allowed_alt_team /* = false */)
{
    uint16 mount_entry = 0;

    // select mount creature id
    TaxiNodesEntry const* node = GetTaxiNodeEntry(id);
    if (node)
    {
        if (team == ALLIANCE)
        {
            mount_entry = node->MountCreatureID[1];
            if (!mount_entry && allowed_alt_team)
                mount_entry = node->MountCreatureID[0];
        }
        else if (team == HORDE)
        {
            mount_entry = node->MountCreatureID[0];

            if (!mount_entry && allowed_alt_team)
                mount_entry = node->MountCreatureID[1];
        }
    }

    CreatureInfo const *mount_info = GetCreatureTemplate(mount_entry);
    if (!mount_info)
        return 0;

    uint16 mount_id = Creature::ChooseDisplayId(mount_info);
    if (!mount_id)
        return 0;

    CreatureDisplayInfoAddon const* minfo = GetCreatureDisplayInfoRandomGender(mount_id);
    if (minfo)
        mount_id = minfo->display_id;

    return mount_id;
}

void ObjectMgr::LoadGraveyardZones()
{
    m_GraveYardMap.clear();                                  // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT `id`, `ghost_zone`, `faction` FROM `game_graveyard_zone`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 safeLocId = fields[0].GetUInt32();
        uint32 zoneId = fields[1].GetUInt32();
        uint32 team   = fields[2].GetUInt32();

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(safeLocId);
        if (!entry)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for not existing graveyard (WorldSafeLocs.dbc id) %u, skipped.", safeLocId);
            continue;
        }

        const auto *areaEntry = AreaEntry::GetById(zoneId);
        if (!areaEntry)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for not existing zone id (%u), skipped.", zoneId);
            continue;
        }

        if (!areaEntry->IsZone())
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record subzone id (%u) instead of zone, skipped.", zoneId);
            continue;
        }

        if (team != TEAM_NONE && team != HORDE && team != ALLIANCE)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for non player faction (%u), skipped.", team);
            continue;
        }

        if (!AddGraveYardLink(safeLocId, zoneId, Team(team), false))
            sLog.outErrorDb("Table `game_graveyard_zone` has a duplicate record for Graveyard (ID: %u) and Zone (ID: %u), skipped.", safeLocId, zoneId);
    }
    while (result->NextRow());
}

WorldSafeLocsEntry const *ObjectMgr::GetClosestGraveYard(float x, float y, float z, uint32 MapId, Team team)
{
    // search for zone associated closest graveyard
    uint32 zoneId = sTerrainMgr.GetZoneId(MapId, x, y, z);

    // Simulate std. algorithm:
    //   found some graveyard associated to (ghost_zone,ghost_map)
    //
    //   if mapId == graveyard.mapId (ghost in plain zone or city or battleground) and search graveyard at same map
    //     then check faction
    //   if mapId != graveyard.mapId (ghost in instance) and search any graveyard associated
    //     then check faction
    GraveYardMapBounds bounds = m_GraveYardMap.equal_range(zoneId);

    if (bounds.first == bounds.second)
        return nullptr;

    // at corpse map
    bool foundNear = false;
    float distNear = 0.0f;
    WorldSafeLocsEntry const* entryNear = nullptr;

    // at entrance map for corpse map
    bool foundEntr = false;
    float distEntr = 0.0f;
    WorldSafeLocsEntry const* entryEntr = nullptr;

    // some where other
    WorldSafeLocsEntry const* entryFar = nullptr;

    MapEntry const* tempEntry = sMapStorage.LookupEntry<MapEntry>(MapId);

    for (GraveYardMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        GraveYardData const& data = itr->second;

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(data.safeLocId);
        if (!entry)
        {
            sLog.outErrorDb("Table `game_graveyard_zone` has record for not existing graveyard (WorldSafeLocs.dbc id) %u, skipped.", data.safeLocId);
            continue;
        }

        // skip enemy faction graveyard
        // team == 0 case can be at call from .neargrave
        if (data.team != TEAM_NONE && team != TEAM_NONE && data.team != team)
            continue;

        // find now nearest graveyard at other (continent) map
        if (MapId != entry->map_id)
        {
            // if find graveyard at different map from where entrance placed (or no entrance data), use any first
            if (!tempEntry ||
                    tempEntry->ghostEntranceMap < 0 ||
                    uint32(tempEntry->ghostEntranceMap) != entry->map_id ||
                    (tempEntry->ghostEntranceX == 0.0f && tempEntry->ghostEntranceY == 0.0f))
            {
                // not have any coordinates for check distance anyway
                entryFar = entry;
                continue;
            }

            // at entrance map calculate distance (2D);
            float dist2 = (entry->x - tempEntry->ghostEntranceX) * (entry->x - tempEntry->ghostEntranceX)
                          + (entry->y - tempEntry->ghostEntranceY) * (entry->y - tempEntry->ghostEntranceY);
            if (foundEntr)
            {
                if (dist2 < distEntr)
                {
                    distEntr = dist2;
                    entryEntr = entry;
                }
            }
            else
            {
                foundEntr = true;
                distEntr = dist2;
                entryEntr = entry;
            }
        }
        // find now nearest graveyard at same map
        else
        {
            float dist2 = (entry->x - x) * (entry->x - x) + (entry->y - y) * (entry->y - y) + (entry->z - z) * (entry->z - z);
            if (foundNear)
            {
                if (dist2 < distNear)
                {
                    distNear = dist2;
                    entryNear = entry;
                }
            }
            else
            {
                foundNear = true;
                distNear = dist2;
                entryNear = entry;
            }
        }
    }

    if (entryNear)
        return entryNear;

    if (entryEntr)
        return entryEntr;

    return entryFar;
}

GraveYardData const* ObjectMgr::FindGraveYardData(uint32 id, uint32 zoneId) const
{
    GraveYardMapBounds bounds = m_GraveYardMap.equal_range(zoneId);

    for (GraveYardMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (itr->second.safeLocId == id)
            return &itr->second;
    }

    return nullptr;
}

bool ObjectMgr::AddGraveYardLink(uint32 id, uint32 zoneId, Team team, bool inDB)
{
    if (FindGraveYardData(id, zoneId))
        return false;

    // add link to loaded data
    GraveYardData data;
    data.safeLocId = id;
    data.team = team;

    m_GraveYardMap.insert(GraveYardMap::value_type(zoneId, data));

    // add link to DB
    if (inDB)
    {
        sWorld.ExecuteUpdate("INSERT INTO `game_graveyard_zone` (`id`, `ghost_zone`, `faction`) "
                                  "VALUES ('%u', '%u','%u')", id, zoneId, uint32(team));
    }

    return true;
}

void ObjectMgr::LoadWorldSafeLocsFacing()
{
    uint32 count = 0;
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `id`, `orientation` FROM `world_safe_locs_facing`"));
    if (!result)
        return;

    do
    {
        Field *fields = result->Fetch();

        uint32 safeLocId = fields[0].GetUInt32();
        float orientation = fields[1].GetFloat();

        m_worldSafeLocsFacingMap[safeLocId] = orientation;
        count++;
    } while (result->NextRow());
}

float ObjectMgr::GetWorldSafeLocFacing(uint32 id) const
{
    auto itr = m_worldSafeLocsFacingMap.find(id);
    if (itr != m_worldSafeLocsFacingMap.end())
        return itr->second;

    return 0.0f;
}

CustomCharacterSkinEntry const* ObjectMgr::GetCustomCharacterSkin(uint32 token_id)
{
    auto skin = m_CustomCharacterSkinMap.find(token_id);
    if (skin != m_CustomCharacterSkinMap.end())
        return &skin->second;

    return nullptr;
}

void ObjectMgr::LoadAreaTriggerTeleports()
{
    m_AreaTriggerTeleportMap.clear();                                  // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query(
    //           0     1                 2                     3                4
        "SELECT `id`, `required_level`, `required_condition`, `required_phase`, `message`, "
    //    5             6                    7                    8                    9 
        "`target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation` "
        "FROM `areatrigger_teleport`"));
    
    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 triggerId = fields[0].GetUInt32();

        AreaTriggerTeleport at;

        at.requiredLevel      = fields[1].GetUInt8();
        at.requiredCondition  = fields[2].GetUInt32();
        at.requiredPhase      = fields[3].GetUInt8();
        at.message            = fields[4].GetCppString();
        at.destination.mapId  = fields[5].GetUInt32();
        at.destination.x      = fields[6].GetFloat();
        at.destination.y      = fields[7].GetFloat();
        at.destination.z      = fields[8].GetFloat();
        at.destination.o      = fields[9].GetFloat();

        AreaTriggerEntry const* atEntry = GetAreaTrigger(triggerId);
        if (!atEntry)
        {
            sLog.outErrorDb("Table `areatrigger_teleport` has area trigger (ID:%u) not listed in `AreaTrigger.dbc`.", triggerId);
            continue;
        }

        if (at.requiredCondition && !sConditionStorage.LookupEntry<ConditionEntry>(at.requiredCondition))
        {
            sLog.outErrorDb("Table `areatrigger_teleport` has nonexistent condition %u defined for trigger %u, ignoring", at.requiredCondition, triggerId);
            at.requiredCondition = 0;
        }

        MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(at.destination.mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Table `areatrigger_teleport` has nonexistent target map (ID: %u) for Area trigger (ID:%u).", at.destination.mapId, triggerId);
            continue;
        }

        if (!at.destination.x && !at.destination.y && !at.destination.z)
        {
            sLog.outErrorDb("Table `areatrigger_teleport` has area trigger (ID:%u) without target coordinates.", triggerId);
            continue;
        }

        m_AreaTriggerTeleportMap[triggerId] = at;

    }
    while (result->NextRow());
}

/*
 * Searches for the areatrigger which teleports players out of the given map (only direct to continent)
 */
AreaTriggerTeleport const* ObjectMgr::GetGoBackTrigger(uint32 map_id) const
{  
    MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(map_id);
    if (!mapEntry || !mapEntry->IsDungeon())
        return nullptr;

    AreaTriggerTeleport const* pClosestTrigger = nullptr;

    for (const auto& itr : m_AreaTriggerTeleportMap)
    {
        if (itr.second.destination.mapId == uint32(mapEntry->ghostEntranceMap))
        {
            AreaTriggerEntry const* atEntry = GetAreaTrigger(itr.first);
            if (atEntry && atEntry->mapid == map_id &&
               (!pClosestTrigger || Geometry::GetDistance2D(itr.second.destination.x, itr.second.destination.y, mapEntry->ghostEntranceX, mapEntry->ghostEntranceY) < Geometry::GetDistance2D(pClosestTrigger->destination.x, pClosestTrigger->destination.y, mapEntry->ghostEntranceX, mapEntry->ghostEntranceY)))
                pClosestTrigger = &itr.second;
        }
    }

    return pClosestTrigger;
}

/**
 * Searches for the areatrigger which teleports players to the given map
 */
AreaTriggerTeleport const* ObjectMgr::GetMapEntranceTrigger(uint32 Map) const
{
    for (const auto& itr : m_AreaTriggerTeleportMap)
    {
        if (itr.second.destination.mapId == Map)
        {
            AreaTriggerEntry const* atEntry = GetAreaTrigger(itr.first);
            if (atEntry)
                return &itr.second;
        }
    }

    return nullptr;
}

void ObjectMgr::PackGroupIds()
{
    // this routine renumbers groups in such a way so they start from 1 and go up

    // obtain set of all groups
    std::set<uint32> groupIds;

    // all valid ids are in the instance table
    // any associations to ids not in this table are assumed to be
    // cleaned already in CleanupInstances
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `groupId` FROM `groups` ORDER BY `groupId` ASC"));
    if (result)
    {
        do
        {
            Field *fields = result->Fetch();

            uint32 id = fields[0].GetUInt32();

            if (id == 0)
            {
                CharacterDatabase.BeginTransaction();
                CharacterDatabase.PExecute("DELETE FROM `groups` WHERE `groupId` = '%u'", id);
                CharacterDatabase.PExecute("DELETE FROM `group_member` WHERE `groupId` = '%u'", id);
                CharacterDatabase.CommitTransaction();
                continue;
            }

            groupIds.insert(id);
        }
        while (result->NextRow());
    }

    uint32 groupId = 1;
    // we do assume std::set is sorted properly on integer value
    for (uint32 i : groupIds)
    {
        if (i != groupId)
        {
            // remap group id
            CharacterDatabase.BeginTransaction();
            CharacterDatabase.PExecute("UPDATE `groups` SET `groupId` = '%u' WHERE `groupId` = '%u'", groupId, i);
            CharacterDatabase.PExecute("UPDATE `group_member` SET `groupId` = '%u' WHERE `groupId` = '%u'", groupId, i);
            CharacterDatabase.CommitTransaction();
        }

        ++groupId;
    }
    m_GroupIds.Set(groupId);
}

GuildHouseEntry const* ObjectMgr::GetGuildHouse(uint32 player_guild)
{
    auto guild_house = m_GuildHouseMap.find(player_guild);
    if (guild_house != m_GuildHouseMap.end())
        return &guild_house->second;

    return nullptr;
}

void ObjectMgr::LoadGuildHouses()
{
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT * FROM `guild_house`"));

    if (!result)
        return;
    do
    {
        Field* fields = result->Fetch();
        GuildHouseEntry guild_house;

        uint32 guild_id = fields[0].GetUInt32();

        guild_house.map_id = fields[1].GetUInt32();
        guild_house.position_x = fields[2].GetFloat();
        guild_house.position_y = fields[3].GetFloat();
        guild_house.position_z = fields[4].GetFloat();
        guild_house.orientation = fields[5].GetFloat();

        m_GuildHouseMap[guild_id] = guild_house;

    } while (result->NextRow());
}

void ObjectMgr::SetHighestGuids()
{
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT MAX(`guid`) FROM `characters`"));
    if (result)
        m_CharGuids.Set((*result)[0].GetUInt32() + 1);

    result.reset(WorldDatabase.Query("SELECT MAX(`guid`) FROM `creature`"));
    if (result)
        m_FirstTemporaryCreatureGuid = (*result)[0].GetUInt32() + 1;

    result.reset(CharacterDatabase.Query("SELECT MAX(`guid`) FROM `item_instance`"));
    if (result)
        m_ItemGuids.Set((*result)[0].GetUInt32() + 1);

    // Cleanup other tables from nonexistent guids (>=m_hiItemGuid)
    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM `character_inventory` WHERE `item` >= '%u'", m_ItemGuids.GetNextAfterMaxUsed());
    CharacterDatabase.PExecute("DELETE FROM `mail_items` WHERE `item_guid` >= '%u'", m_ItemGuids.GetNextAfterMaxUsed());
    CharacterDatabase.PExecute("DELETE FROM `auction` WHERE `itemguid` >= '%u'", m_ItemGuids.GetNextAfterMaxUsed());
    CharacterDatabase.CommitTransaction();

    result.reset(WorldDatabase.Query("SELECT MAX(`guid`) FROM `gameobject`"));
    if (result)
        m_FirstTemporaryGameObjectGuid = (*result)[0].GetUInt32() + 1;

    result.reset(CharacterDatabase.Query("SELECT `id` FROM `auction`"));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            m_AuctionsIds.insert(fields[0].GetUInt32());
        } while (result->NextRow());
    }
    m_NextAuctionId = 1;

    result.reset(CharacterDatabase.Query("SELECT MAX(`id`) FROM `mail`"));
    if (result)
        m_MailIds.Set((*result)[0].GetUInt32() + 1);

    result.reset(CharacterDatabase.Query("SELECT MAX(`id`) FROM `item_text`"));
    if (result)
        m_ItemTextIds.Set((*result)[0].GetUInt32() + 1);

    result.reset(CharacterDatabase.Query("SELECT MAX(`guid`) FROM `corpse`"));
    if (result)
        m_CorpseGuids.Set((*result)[0].GetUInt32() + 1);

    result.reset(CharacterDatabase.Query("SELECT MAX(`guildid`) FROM `guild`"));
    if (result)
        m_GuildIds.Set((*result)[0].GetUInt32() + 1);

    result.reset(CharacterDatabase.Query("SELECT MAX(`groupId`) FROM `groups`"));
    if (result)
        m_GroupIds.Set((*result)[0].GetUInt32() + 1);

    result.reset(CharacterDatabase.Query("SELECT MAX(`petitionguid`) FROM `petition`"));
    if (result)
        m_PetitionIds.Set((*result)[0].GetUInt32() + 1);

    // setup reserved ranges for static guids spawn
    m_StaticCreatureGuids.Set(m_FirstTemporaryCreatureGuid);
    m_FirstTemporaryCreatureGuid += sWorld.getConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE);

    m_StaticGameObjectGuids.Set(m_FirstTemporaryGameObjectGuid);
    m_FirstTemporaryGameObjectGuid += sWorld.getConfig(CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT);
}

uint32 ObjectMgr::CreateItemText(std::string text)
{
    uint32 newItemTextId = GenerateItemTextID();
    //insert new itempage to container
    m_ItemTextsMap[ newItemTextId ] = text;
    //save new itempage
    CharacterDatabase.escape_string(text);
    //any Delete query needed, itemTextId is maximum of all ids
    std::ostringstream query;
    query << "INSERT INTO `item_text` (`id`, `text`) VALUES ( '" << newItemTextId << "', '" << text << "')";
    CharacterDatabase.Execute(query.str().c_str());         //needs to be run this way, because mail body may be more than 1024 characters
    return newItemTextId;
}

void ObjectMgr::LoadGameObjectLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_GameObjectLocaleMap.clear();                           // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `name_loc7`, `name_loc8` FROM `locales_gameobject`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!GetGameObjectInfo(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_gameobject` has data for nonexistent gameobject entry %u, skipped.", entry);
            continue;
        }

        GameObjectLocale& data = m_GameObjectLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }
        }

    }
    while (result->NextRow());
}

inline void CheckGOLockId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    if (sLockStore.LookupEntry(dataN))
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but lock (Id: %u) not found.",
                    goInfo->id, goInfo->type, N, dataN, dataN);
}

inline void CheckGOLinkedTrapId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    if (GameObjectInfo const* trapInfo = sObjectMgr.GetGameObjectInfo(dataN))
    {
        if (trapInfo->type != GAMEOBJECT_TYPE_TRAP)
            sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but GO (Entry %u) have not GAMEOBJECT_TYPE_TRAP (%u) type.",
                            goInfo->id, goInfo->type, N, dataN, dataN, GAMEOBJECT_TYPE_TRAP);
    }
    else
        // too many error reports about nonexistent trap templates
        ERROR_DB_STRICT_LOG("Gameobject (Entry: %u GoType: %u) have data%d=%u but trap GO (Entry %u) not exist in `gameobject_template`.",
                            goInfo->id, goInfo->type, N, dataN, dataN);
}

inline void CheckGOSpellId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    if (sSpellMgr.GetSpellEntry(dataN))
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but Spell (Entry %u) not exist.",
                    goInfo->id, goInfo->type, N, dataN, dataN);
}

inline void CheckAndFixGOChairHeightId(GameObjectInfo const* goInfo, uint32 const& dataN, uint32 N)
{
    if (dataN <= (UNIT_STAND_STATE_SIT_HIGH_CHAIR - UNIT_STAND_STATE_SIT_LOW_CHAIR))
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but correct chair height in range 0..%i.",
                    goInfo->id, goInfo->type, N, dataN, UNIT_STAND_STATE_SIT_HIGH_CHAIR - UNIT_STAND_STATE_SIT_LOW_CHAIR);

    // prevent client and server unexpected work
    const_cast<uint32&>(dataN) = 0;
}

inline void CheckGONoDamageImmuneId(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) noDamageImmune field value.",
                    goInfo->id, goInfo->type, N, dataN);
}

inline void CheckGOConsumable(GameObjectInfo const* goInfo, uint32 dataN, uint32 N)
{
    // 0/1 correct values
    if (dataN <= 1)
        return;

    sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data%d=%u but expected boolean (0/1) consumable field value.",
                    goInfo->id, goInfo->type, N, dataN);
}

void ObjectMgr::LoadGameobjectInfo()
{
    m_GameObjectInfoMap.clear();                           // need for reload case

    //                                                               0        1       2            3       4          5        6       7        8        9        10       11       12       13       14       15       16       17        18        19        20        21        22        23        24        25        26        27        28        29        30        31         32         33                34
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `type`, `displayId`, `name`, `faction`, `flags`, `size`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `mingold`, `maxgold`, `phase_quest_id`, `script_name` FROM `gameobject_template`"));

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        GameObjectInfo& data = m_GameObjectInfoMap[entry];
        data.id = entry;
        data.type = fields[1].GetUInt32();
        data.displayId = fields[2].GetUInt32();
        data.name = fields[3].GetCppString();
        data.faction = fields[4].GetUInt32();
        data.flags = fields[5].GetUInt32();
        data.size = fields[6].GetFloat();
        for (int i = 0; i < 24; i++)
            data.raw.data[i] = fields[7 + i].GetUInt32();
        data.MinMoneyLoot = fields[31].GetUInt32();
        data.MaxMoneyLoot = fields[32].GetUInt32();
        data.PhaseQuestId = fields[33].GetUInt32();
        data.ScriptId = sScriptMgr.GetScriptId(fields[34].GetString());

    } while (result->NextRow());

    CheckGameObjectInfos();
}

void ObjectMgr::CheckGameObjectInfos()
{
    // some checks
    for (auto itr = m_GameObjectInfoMap.begin(); itr != m_GameObjectInfoMap.end(); ++itr)
    {
        if (itr->second.size <= 0.0f)                           // prevent use too small scales
        {
            ERROR_DB_STRICT_LOG("Gameobject (Entry: %u GoType: %u) have too small size=%f",
                itr->second.id, itr->second.type, itr->second.size);
            const_cast<GameObjectInfo*>(&itr->second)->size =  DEFAULT_OBJECT_SCALE;
        }

        if (itr->second.type >= GAMEOBJECT_TYPE_MAX)
        {
            sLog.outErrorDb("Gameobject (Entry: %u) have invalid type=%u in template, forcing it to type GENERIC (5) instead.",
                            itr->second.id, itr->second.type);
            const_cast<GameObjectInfo*>(&itr->second)->type = GAMEOBJECT_TYPE_GENERIC;
            memset(const_cast<GameObjectInfo*>(&itr->second)->raw.data, 0, sizeof(GameObjectInfo::raw.data));
        }

        switch (itr->second.type)
        {
            case GAMEOBJECT_TYPE_DOOR:                      //0
            {
                if (itr->second.door.lockId)
                    CheckGOLockId(&itr->second, itr->second.door.lockId, 1);
                CheckGONoDamageImmuneId(&itr->second, itr->second.door.noDamageImmune, 3);
                break;
            }
            case GAMEOBJECT_TYPE_BUTTON:                    //1
            {
                if (itr->second.button.lockId)
                    CheckGOLockId(&itr->second, itr->second.button.lockId, 1);
                if (itr->second.button.linkedTrapId)              // linked trap
                    CheckGOLinkedTrapId(&itr->second, itr->second.button.linkedTrapId, 3);
                CheckGONoDamageImmuneId(&itr->second, itr->second.button.noDamageImmune, 4);
                break;
            }
            case GAMEOBJECT_TYPE_QUESTGIVER:                //2
            {
                if (itr->second.questgiver.lockId)
                    CheckGOLockId(&itr->second, itr->second.questgiver.lockId, 0);
                CheckGONoDamageImmuneId(&itr->second, itr->second.questgiver.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_CHEST:                     //3
            {
                if (itr->second.chest.lockId)
                    CheckGOLockId(&itr->second, itr->second.chest.lockId, 0);

                CheckGOConsumable(&itr->second, itr->second.chest.consumable, 3);

                if (itr->second.chest.linkedTrapId)              // linked trap
                    CheckGOLinkedTrapId(&itr->second, itr->second.chest.linkedTrapId, 7);
                break;
            }
            case GAMEOBJECT_TYPE_TRAP:                      //6
            {
                if (itr->second.trap.lockId)
                    CheckGOLockId(&itr->second, itr->second.trap.lockId, 0);
                /* disable check for while, too many nonexistent spells
                if (goInfo->trap.spellId)                   // spell
                    CheckGOSpellId(goInfo,goInfo->trap.spellId,3);
                */
                break;
            }
            case GAMEOBJECT_TYPE_CHAIR:                     //7
                CheckAndFixGOChairHeightId(&itr->second, itr->second.chair.height, 1);
                break;
            case GAMEOBJECT_TYPE_SPELL_FOCUS:               //8
            {
                if (itr->second.spellFocus.focusId)
                {
                    if (!sSpellFocusObjectStore.LookupEntry(itr->second.spellFocus.focusId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data0=%u but SpellFocus (Id: %u) not exist.",
                            itr->second.id, itr->second.type, itr->second.spellFocus.focusId, itr->second.spellFocus.focusId);
                }

                if (itr->second.spellFocus.linkedTrapId)        // linked trap
                    CheckGOLinkedTrapId(&itr->second, itr->second.spellFocus.linkedTrapId, 2);
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:                    //10
            {
                if (itr->second.goober.lockId)
                    CheckGOLockId(&itr->second, itr->second.goober.lockId, 0);

                CheckGOConsumable(&itr->second, itr->second.goober.consumable, 3);

                if (itr->second.goober.pageId)                  // pageId
                {
                    if (!sPageTextStore.LookupEntry<PageText>(itr->second.goober.pageId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data7=%u but PageText (Entry %u) not exist.",
                            itr->second.id, itr->second.type, itr->second.goober.pageId, itr->second.goober.pageId);
                }
                /* disable check for while, too many nonexistent spells
                if (goInfo->goober.spellId)                 // spell
                    CheckGOSpellId(goInfo,goInfo->goober.spellId,10);
                */
                CheckGONoDamageImmuneId(&itr->second, itr->second.goober.noDamageImmune, 11);
                if (itr->second.goober.linkedTrapId)            // linked trap
                    CheckGOLinkedTrapId(&itr->second, itr->second.goober.linkedTrapId, 12);
                break;
            }
            case GAMEOBJECT_TYPE_AREADAMAGE:                //12
            {
                if (itr->second.areadamage.lockId)
                    CheckGOLockId(&itr->second, itr->second.areadamage.lockId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_CAMERA:                    //13
            {
                if (itr->second.camera.lockId)
                    CheckGOLockId(&itr->second, itr->second.camera.lockId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_MO_TRANSPORT:              //15
            {
                if (itr->second.moTransport.taxiPathId)
                {
                    if (itr->second.moTransport.taxiPathId >= sTaxiPathNodesByPath.size() || sTaxiPathNodesByPath[itr->second.moTransport.taxiPathId].empty())
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) have data0=%u but TaxiPath (Id: %u) not exist.",
                            itr->second.id, itr->second.type, itr->second.moTransport.taxiPathId, itr->second.moTransport.taxiPathId);
                }
                break;
            }
            case GAMEOBJECT_TYPE_SUMMONING_RITUAL:          //18
            {
                /* disable check for while, too many nonexistent spells
                // always must have spell
                CheckGOSpellId(goInfo,goInfo->summoningRitual.spellId,1);
                */
                break;
            }
            case GAMEOBJECT_TYPE_SPELLCASTER:               //22
            {
                // always must have spell
                CheckGOSpellId(&itr->second, itr->second.spellcaster.spellId, 0);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGSTAND:                 //24
            {
                if (itr->second.flagstand.lockId)
                    CheckGOLockId(&itr->second, itr->second.flagstand.lockId, 0);
                CheckGONoDamageImmuneId(&itr->second, itr->second.flagstand.noDamageImmune, 5);
                break;
            }
            case GAMEOBJECT_TYPE_FISHINGHOLE:               //25
            {
                if (itr->second.fishinghole.lockId)
                    CheckGOLockId(&itr->second, itr->second.fishinghole.lockId, 4);
                break;
            }
            case GAMEOBJECT_TYPE_FLAGDROP:                  //26
            {
                if (itr->second.flagdrop.lockId)
                    CheckGOLockId(&itr->second, itr->second.flagdrop.lockId, 0);
                CheckGONoDamageImmuneId(&itr->second, itr->second.flagdrop.noDamageImmune, 3);
                break;
            }
        }
    }
}

void ObjectMgr::AddGameobjectInfo(GameObjectInfo&& goinfo)
{
    const uint32 id = goinfo.id;
    auto itr = m_GameObjectInfoMap.emplace(id, std::move(goinfo));
    MANGOS_ASSERT(itr.second);
}

void ObjectMgr::LoadGameobjectsRequirements()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `guid`, `reqType`, `reqGuid` FROM `gameobject_requirement`"));
    _gobjRequirements.clear();

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 guid = fields[0].GetUInt32();
        if (!GetGOData(guid))
        {
            if (!sObjectMgr.IsExistingGameObjectGuid(guid))
                sLog.outErrorDb("Table `gameobject_requirement` has data for invalid game object guid (%u)", guid);
            continue;
        }

        GameObjectUseRequirement entry;
        entry.reqType = GameObjectUseRequirement::GameObjectUseRequireType(fields[1].GetUInt8());
        uint32 reqGuidLow = fields[2].GetUInt32();
        switch (entry.reqType)
        {
            case GameObjectUseRequirement::GOBJ_REQUIRE_DEAD_CREATURE:
                if (CreatureData const* data = GetCreatureData(reqGuidLow))
                    entry.guid = ObjectGuid(HIGHGUID_UNIT, data->creature_id[0], reqGuidLow);
                else
                {
                    sLog.outErrorDb("Table `gameobject_requirement` has not found creature guid (%u)", reqGuidLow);
                    continue;
                }
                break;
            case GameObjectUseRequirement::GOBJ_REQUIRE_ACTIVE_OBJECT:
                if (GameObjectData const* data = GetGOData(reqGuidLow))
                    entry.guid = ObjectGuid(HIGHGUID_GAMEOBJECT, data->id, reqGuidLow);
                else
                {
                    sLog.outErrorDb("Table `gameobject_requirement` has not found game object guid (%u)", reqGuidLow);
                    continue;
                }
                break;
            default:
                sLog.outErrorDb("Table `gameobject_requirement` has invalid reqType (%u)", entry.reqType);
                break;
        }

        _gobjRequirements[guid] = entry;
    }
    while (result->NextRow());
}

GameObjectUseRequirement const* ObjectMgr::GetGameObjectUseRequirement(ObjectGuid guid) const
{
    std::map<uint32, GameObjectUseRequirement>::const_iterator it = _gobjRequirements.find(guid.GetCounter());
    if (it != _gobjRequirements.end())
        return &it->second;
    return nullptr;
}

void ObjectMgr::LoadGameObjectDisplayInfoAddon()
{
    sGameObjectDisplayInfoAddonStorage.Load();
}

void ObjectMgr::LoadExplorationBaseXP()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `level`, `basexp` FROM `exploration_basexp`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 level  = fields[0].GetUInt32();
        uint32 basexp = fields[1].GetUInt32();
        m_BaseXPMap[level] = basexp;
    }
    while (result->NextRow());
}

uint32 ObjectMgr::GetBaseXP(uint32 level) const
{
    BaseXPMap::const_iterator itr = m_BaseXPMap.find(level);
    return itr != m_BaseXPMap.end() ? itr->second : 0;
}

uint32 ObjectMgr::GetXPForLevel(uint32 level) const
{
    if (m_PlayerXPperLevel.empty())
        return 0;

    return m_PlayerXPperLevel[std::min(level, uint32(m_PlayerXPperLevel.size() - 1))];
}

void ObjectMgr::LoadPetNames()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `word`, `entry`, `half` FROM `pet_name_generation`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        std::string word = fields[0].GetString();
        uint32 entry     = fields[1].GetUInt32();
        bool   half      = fields[2].GetBool();
        if (half)
            m_PetHalfNameMap1[entry].push_back(word);
        else
            m_PetHalfNameMap0[entry].push_back(word);
    }
    while (result->NextRow());
}

void ObjectMgr::LoadPetNumber()
{
    m_NextPetNumber = 1;
}

uint32 ObjectMgr::GeneratePetNumber()
{
    m_NextPetNumber = sCharacterDatabaseCache.GetNextAvailablePetNumber(m_NextPetNumber);
    return m_NextPetNumber++;
}

std::string ObjectMgr::GeneratePetName(uint32 entry)
{
    std::vector<std::string> & list0 = m_PetHalfNameMap0[entry];
    std::vector<std::string> & list1 = m_PetHalfNameMap1[entry];

    if (list0.empty() || list1.empty())
    {
        CreatureInfo const *cinfo = GetCreatureTemplate(entry);
        char const* petname = GetPetName(cinfo->beast_family, sWorld.GetDefaultDbcLocale());
        if (!petname)
            petname = cinfo->name.c_str();
        return std::string(petname);
    }

    return *(list0.begin() + urand(0, list0.size() - 1)) + *(list1.begin() + urand(0, list1.size() - 1));
}

void ObjectMgr::LoadCorpses()
{
    //                                                                            0       1                  2                      3                      4                      5                       6
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `corpse`.`guid`, `player`, `corpse`.`position_x`, `corpse`.`position_y`, `corpse`.`position_z`, `corpse`.`orientation`, `corpse`.`map`, "
    //                      7       8              9           10        11      12       13             14              15                16         17
                          "`time`, `corpse_type`, `instance`, `gender`, `race`, `class`, `playerBytes`, `playerBytes2`, `equipmentCache`, `guildId`, `playerFlags` FROM `corpse` "
                          "JOIN `characters` ON `player` = `characters`.`guid` "
                          "LEFT JOIN `guild_member` ON `player`=`guild_member`.`guid` WHERE `corpse_type` <> 0"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 guid = fields[0].GetUInt32();

        Corpse *corpse = new Corpse;
        if (!corpse->LoadFromDB(guid, fields))
        {
            delete corpse;
            continue;
        }

        sObjectAccessor.AddCorpse(corpse);
    }
    while (result->NextRow());
}

void ObjectMgr::LoadFactions()
{
    {
        m_FactionsMap.clear();
        std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT * FROM `faction`"));

        if (!result)
        {
            return;
        }

        do
        {
            Field* fields = result->Fetch();

            FactionEntry faction;

            uint32 factionId = fields[0].GetUInt32();

            faction.ID = factionId;
            faction.reputationListID = fields[1].GetInt32();
            faction.BaseRepRaceMask[0] = fields[2].GetUInt32();
            faction.BaseRepRaceMask[1] = fields[3].GetUInt32();
            faction.BaseRepRaceMask[2] = fields[4].GetUInt32();
            faction.BaseRepRaceMask[3] = fields[5].GetUInt32();
            faction.BaseRepClassMask[0] = fields[6].GetUInt32();
            faction.BaseRepClassMask[1] = fields[7].GetUInt32();
            faction.BaseRepClassMask[2] = fields[8].GetUInt32();
            faction.BaseRepClassMask[3] = fields[9].GetUInt32();
            faction.BaseRepValue[0] = fields[10].GetInt32();
            faction.BaseRepValue[1] = fields[11].GetInt32();
            faction.BaseRepValue[2] = fields[12].GetInt32();
            faction.BaseRepValue[3] = fields[13].GetInt32();
            faction.ReputationFlags[0] = fields[14].GetUInt32();
            faction.ReputationFlags[1] = fields[15].GetUInt32();
            faction.ReputationFlags[2] = fields[16].GetUInt32();
            faction.ReputationFlags[3] = fields[17].GetUInt32();
            faction.team = fields[18].GetUInt32();
            faction.name[0] = fields[19].GetCppString();

            m_FactionsMap[factionId] = faction;

        } while (result->NextRow());

        if (sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        {
            // Load localized texts (currently we only have 1.12 locales).
            //                                        0        1            2            3            4            5            6            7                   8                   9                   10                  11                  12
            result.reset(WorldDatabase.Query("SELECT `entry`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `description_loc1`, `description_loc2`, `description_loc3`, `description_loc4`, `description_loc5`, `description_loc6` FROM `locales_faction`"));
            if (result)
            {
                do
                {
                    auto fields = result->Fetch();
                    uint32 factionId = fields[0].GetUInt32();
                    auto itr = m_FactionsMap.find(factionId);
                    if (itr == m_FactionsMap.end())
                        continue;

                    FactionEntry& faction = itr->second;
                    faction.name[1] = fields[1].GetCppString();
                    faction.name[2] = fields[2].GetCppString();
                    faction.name[3] = fields[3].GetCppString();
                    faction.name[4] = fields[4].GetCppString();
                    faction.name[5] = fields[5].GetCppString();
                    faction.name[6] = fields[6].GetCppString();
                } while (result->NextRow());
            }
        }
    }
    
    {
        m_FactionTemplatesMap.clear();
        std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT * FROM `faction_template`"));

        if (!result)
        {
            return;
        }

        std::set<uint32> factionsWithEnemies;

        do
        {
            Field* fields = result->Fetch();

            FactionTemplateEntry faction;

            uint32 factionId = fields[0].GetUInt32();

            faction.ID = factionId;
            faction.faction = fields[1].GetUInt32();
            faction.factionFlags = fields[2].GetUInt32();
            faction.ourMask = fields[3].GetUInt32();
            faction.friendlyMask = fields[4].GetUInt32();
            faction.hostileMask = fields[5].GetUInt32();
            for (int i = 0; i < 4; ++i)
            {
                if (faction.enemyFaction[i] = fields[6 + i].GetUInt32())
                {
                    if (faction.factionFlags & (FACTION_TEMPLATE_SEARCH_FOR_ENEMIES_LOW_PRIO | FACTION_TEMPLATE_SEARCH_FOR_ENEMIES_MED_PRIO | FACTION_TEMPLATE_SEARCH_FOR_ENEMIES_HIG_PRIO))
                        factionsWithEnemies.insert(faction.enemyFaction[i]);
                }
            }
            for (int i = 0; i < 4; ++i)
            {
                faction.friendFaction[i] = fields[10 + i].GetUInt32();
            }

            m_FactionTemplatesMap[factionId] = faction;

        } while (result->NextRow());

        // This is needed to make sure passive factions who are someone's enemy notify their enemies upon moving.
        for (auto& itr : m_FactionTemplatesMap)
        {
            if (factionsWithEnemies.find(itr.second.faction) != factionsWithEnemies.end())
                itr.second.isEnemyOfAnother = true;
        }
    }
    
}

void ObjectMgr::LoadReputationRewardRate()
{
    m_RepRewardRateMap.clear();                             // for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `faction`, `quest_rate`, `creature_rate`, `spell_rate` FROM `reputation_reward_rate`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 factionId        = fields[0].GetUInt32();

        RepRewardRate repRate;

        repRate.quest_rate      = fields[1].GetFloat();
        repRate.creature_rate   = fields[2].GetFloat();
        repRate.spell_rate      = fields[3].GetFloat();

        FactionEntry const *factionEntry = GetFactionEntry(factionId);
        if (!factionEntry)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_reward_rate`", factionId);
            continue;
        }

        if (repRate.quest_rate < 0.0f)
        {
            sLog.outErrorDb("Table reputation_reward_rate has quest_rate with invalid rate %f, skipping data for faction %u", repRate.quest_rate, factionId);
            continue;
        }

        if (repRate.creature_rate < 0.0f)
        {
            sLog.outErrorDb("Table reputation_reward_rate has creature_rate with invalid rate %f, skipping data for faction %u", repRate.creature_rate, factionId);
            continue;
        }

        if (repRate.spell_rate < 0.0f)
        {
            sLog.outErrorDb("Table reputation_reward_rate has spell_rate with invalid rate %f, skipping data for faction %u", repRate.spell_rate, factionId);
            continue;
        }

        m_RepRewardRateMap[factionId] = repRate;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadReputationOnKill()
{
    //                                                               0              1                       2
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `creature_id`, `RewOnKillRepFaction1`, `RewOnKillRepFaction2`,"
    //                      3               4               5                     6               7               8                     9
                          "`IsTeamAward1`, `MaxStanding1`, `RewOnKillRepValue1`, `IsTeamAward2`, `MaxStanding2`, `RewOnKillRepValue2`, `TeamDependent` "
                          "FROM `creature_onkill_reputation`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 creature_id = fields[0].GetUInt32();

        ReputationOnKillEntry repOnKill;
        repOnKill.repfaction1          = fields[1].GetUInt32();
        repOnKill.repfaction2          = fields[2].GetUInt32();
        repOnKill.is_teamaward1        = fields[3].GetBool();
        repOnKill.reputation_max_cap1  = fields[4].GetUInt32();
        repOnKill.repvalue1            = fields[5].GetInt32();
        repOnKill.is_teamaward2        = fields[6].GetBool();
        repOnKill.reputation_max_cap2  = fields[7].GetUInt32();
        repOnKill.repvalue2            = fields[8].GetInt32();
        repOnKill.team_dependent       = fields[9].GetUInt8();

        if (!GetCreatureTemplate(creature_id))
        {
            if (!sObjectMgr.IsExistingCreatureId(creature_id))
                sLog.outErrorDb("Table `creature_onkill_reputation` have data for nonexistent creature entry (%u), skipped", creature_id);
            continue;
        }

        if (repOnKill.repfaction1)
        {
            FactionEntry const *factionEntry1 = GetFactionEntry(repOnKill.repfaction1);
            if (!factionEntry1)
            {
                sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.repfaction1);
                continue;
            }
        }

        if (repOnKill.repfaction2)
        {
            FactionEntry const *factionEntry2 = GetFactionEntry(repOnKill.repfaction2);
            if (!factionEntry2)
            {
                sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `creature_onkill_reputation`", repOnKill.repfaction2);
                continue;
            }
        }

        m_RepOnKillMap[creature_id] = repOnKill;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadReputationSpilloverTemplate()
{
    m_RepSpilloverTemplateMap.clear();                      // for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `faction`, `faction1`, `rate_1`, `rank_1`, `faction2`, `rate_2`, `rank_2`, `faction3`, `rate_3`, `rank_3`, `faction4`, `rate_4`, `rank_4` FROM `reputation_spillover_template`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 factionId                = fields[0].GetUInt32();

        RepSpilloverTemplate repTemplate;

        repTemplate.faction[0]          = fields[1].GetUInt32();
        repTemplate.faction_rate[0]     = fields[2].GetFloat();
        repTemplate.faction_rank[0]     = fields[3].GetUInt32();
        repTemplate.faction[1]          = fields[4].GetUInt32();
        repTemplate.faction_rate[1]     = fields[5].GetFloat();
        repTemplate.faction_rank[1]     = fields[6].GetUInt32();
        repTemplate.faction[2]          = fields[7].GetUInt32();
        repTemplate.faction_rate[2]     = fields[8].GetFloat();
        repTemplate.faction_rank[2]     = fields[9].GetUInt32();
        repTemplate.faction[3]          = fields[10].GetUInt32();
        repTemplate.faction_rate[3]     = fields[11].GetFloat();
        repTemplate.faction_rank[3]     = fields[12].GetUInt32();

        FactionEntry const *factionEntry = GetFactionEntry(factionId);

        if (!factionEntry)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", factionId);
            continue;
        }

        for (uint32 i = 0; i < MAX_SPILLOVER_FACTIONS; ++i)
        {
            if (repTemplate.faction[i])
            {
                FactionEntry const *factionSpillover = GetFactionEntry(repTemplate.faction[i]);

                if (!factionSpillover)
                {
                    sLog.outErrorDb("Spillover faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template` for faction %u, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (factionSpillover->reputationListID < 0)
                {
                    sLog.outErrorDb("Spillover faction (faction.dbc) %u for faction %u in `reputation_spillover_template` can not be listed for client, and then useless, skipping", repTemplate.faction[i], factionId);
                    continue;
                }

                if (repTemplate.faction_rank[i] >= MAX_REPUTATION_RANK)
                {
                    sLog.outErrorDb("Rank %u used in `reputation_spillover_template` for spillover faction %u is not valid, skipping", repTemplate.faction_rank[i], repTemplate.faction[i]);
                    continue;
                }
            }
        }

        FactionEntry const *factionEntry0 = GetFactionEntry(repTemplate.faction[0]);
        if (repTemplate.faction[0] && !factionEntry0)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[0]);
            continue;
        }
        FactionEntry const *factionEntry1 = GetFactionEntry(repTemplate.faction[1]);
        if (repTemplate.faction[1] && !factionEntry1)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[1]);
            continue;
        }
        FactionEntry const *factionEntry2 = GetFactionEntry(repTemplate.faction[2]);
        if (repTemplate.faction[2] && !factionEntry2)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[2]);
            continue;
        }
        FactionEntry const *factionEntry3 = GetFactionEntry(repTemplate.faction[3]);
        if (repTemplate.faction[3] && !factionEntry3)
        {
            sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in `reputation_spillover_template`", repTemplate.faction[3]);
            continue;
        }

        m_RepSpilloverTemplateMap[factionId] = repTemplate;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadPointsOfInterest()
{
    m_PointsOfInterestMap.clear();                              // need for reload case

    //                                                               0        1    2    3       4        5       6
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `x`, `y`, `icon`, `flags`, `data`, `icon_name` FROM `points_of_interest`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 point_id = fields[0].GetUInt32();

        PointOfInterest POI;
        POI.x                    = fields[1].GetFloat();
        POI.y                    = fields[2].GetFloat();
        POI.icon                 = fields[3].GetUInt32();
        POI.flags                = fields[4].GetUInt32();
        POI.data                 = fields[5].GetUInt32();
        POI.icon_name            = fields[6].GetCppString();

        if (!MaNGOS::IsValidMapCoord(POI.x, POI.y))
        {
            sLog.outErrorDb("Table `points_of_interest` (Entry: %u) have invalid coordinates (X: %f Y: %f), ignored.", point_id, POI.x, POI.y);
            continue;
        }

        m_PointsOfInterestMap[point_id] = POI;
    }
    while (result->NextRow());
}

void ObjectMgr::DeleteCreatureData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    CreatureData const* data = GetCreatureData(guid);
    if (data)
        RemoveCreatureFromGrid(guid, data);

    m_CreatureDataMap.erase(guid);
}

void ObjectMgr::DeleteGOData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    GameObjectData const* data = GetGOData(guid);
    if (data)
        RemoveGameobjectFromGrid(guid, data);

    m_GameObjectDataMap.erase(guid);
}

void ObjectMgr::AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    std::unique_lock<std::mutex> lock(m_MapObjectGuids_lock);
    CellObjectGuids& cell_guids = m_MapObjectGuids[mapid][cellid];
    cell_guids.corpses[player_guid] = instance;
}

void ObjectMgr::DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids& cell_guids = m_MapObjectGuids[mapid][cellid];
    cell_guids.corpses.erase(player_guid);
}

void ObjectMgr::LoadQuestRelationsHelper(QuestRelationsMap& map, char const* table)
{
    map.clear();                                            // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT `id`, `quest` FROM %s", table));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 id    = fields[0].GetUInt32();
        uint32 quest = fields[1].GetUInt32();

        if (m_QuestTemplatesMap.find(quest) == m_QuestTemplatesMap.end())
        {
            sLog.outErrorDb("Table `%s: Quest %u listed for entry %u does not exist.", table, quest, id);
            continue;
        }

        map.insert(QuestRelationsMap::value_type(id, quest));
    }
    while (result->NextRow());
}

void ObjectMgr::LoadGameobjectQuestRelations()
{
    LoadQuestRelationsHelper(m_GOQuestRelations, "gameobject_questrelation");

    for (const auto& itr : m_GOQuestRelations)
    {
        GameObjectInfo const* goInfo = GetGameObjectInfo(itr.first);
        if (!goInfo)
            sLog.outErrorDb("Table `gameobject_questrelation` have data for nonexistent gameobject entry (%u) and existing quest %u", itr.first, itr.second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog.outErrorDb("Table `gameobject_questrelation` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr.first, itr.second);
    }
}

void ObjectMgr::LoadGameobjectInvolvedRelations()
{
    LoadQuestRelationsHelper(m_GOQuestInvolvedRelations, "gameobject_involvedrelation");

    for (const auto& itr : m_GOQuestInvolvedRelations)
    {
        GameObjectInfo const* goInfo = GetGameObjectInfo(itr.first);
        if (!goInfo)
            sLog.outErrorDb("Table `gameobject_involvedrelation` have data for nonexistent gameobject entry (%u) and existing quest %u", itr.first, itr.second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog.outErrorDb("Table `gameobject_involvedrelation` have data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr.first, itr.second);
    }
}

void ObjectMgr::LoadCreatureQuestRelations()
{
    LoadQuestRelationsHelper(m_CreatureQuestRelations, "creature_questrelation");

    for (const auto& itr : m_CreatureQuestRelations)
    {
        CreatureInfo const* cInfo = GetCreatureTemplate(itr.first);
        if (!cInfo)
            sLog.outErrorDb("Table `creature_questrelation` have data for nonexistent creature entry (%u) and existing quest %u", itr.first, itr.second);
        else if (!(cInfo->npc_flags & UNIT_NPC_FLAG_QUESTGIVER))
            sLog.outDetail("Table `creature_questrelation` has creature entry (%u) for quest %u, but npc_flags does not include UNIT_NPC_FLAG_QUESTGIVER", itr.first, itr.second);
    }
}

void ObjectMgr::LoadCreatureInvolvedRelations()
{
    LoadQuestRelationsHelper(m_CreatureQuestInvolvedRelations, "creature_involvedrelation");

    for (const auto& itr : m_CreatureQuestInvolvedRelations)
    {
        CreatureInfo const* cInfo = GetCreatureTemplate(itr.first);
        if (!cInfo)
            sLog.outErrorDb("Table `creature_involvedrelation` have data for nonexistent creature entry (%u) and existing quest %u", itr.first, itr.second);
        else if (!(cInfo->npc_flags & UNIT_NPC_FLAG_QUESTGIVER))
            sLog.outDetail("Table `creature_involvedrelation` has creature entry (%u) for quest %u, but npc_flags does not include UNIT_NPC_FLAG_QUESTGIVER", itr.first, itr.second);
    }
}

void ObjectMgr::LoadTaxiNodes()
{
    // Getting the maximum ID.
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT MAX(`id`) FROM `taxi_nodes`"));

    if (!result)
    {
        return;
    }

    Field* fields = result->Fetch();
    uint32 maxTaxiNodeEntry = fields[0].GetUInt32() + 1;

    // Actually loading the taxi nodes.
    result.reset(WorldDatabase.Query("SELECT * FROM `taxi_nodes`"));

    if (!result)
    {
        return;
    }

    m_TaxiNodes.resize(maxTaxiNodeEntry);

    do
    {
        fields = result->Fetch();

        std::unique_ptr<TaxiNodesEntry> taxiNode = std::make_unique<TaxiNodesEntry>();

        uint32 nodeId = fields[0].GetUInt32();

        taxiNode->ID = nodeId;
        taxiNode->map_id = fields[1].GetUInt32();
        taxiNode->x = fields[2].GetFloat();
        taxiNode->y = fields[3].GetFloat();
        taxiNode->z = fields[4].GetFloat();
        taxiNode->name[0] = fields[5].GetCppString();
        taxiNode->MountCreatureID[0] = fields[6].GetUInt32();
        taxiNode->MountCreatureID[1] = fields[7].GetUInt32();

        m_TaxiNodes[nodeId] = std::move(taxiNode);

    } while (result->NextRow());

    if (sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
    {
        // Load localized texts (currently we only have 1.12 locales).
        //                                        0        1            2            3            4            5            6
        result.reset(WorldDatabase.Query("SELECT `entry`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6` FROM `locales_taxi_node`"));
        if (result)
        {
            do
            {
                fields = result->Fetch();
                uint32 nodeId = fields[0].GetUInt32();
                if ((nodeId > maxTaxiNodeEntry) || (!m_TaxiNodes[nodeId]))
                    continue;

                m_TaxiNodes[nodeId]->name[1] = fields[1].GetCppString();
                m_TaxiNodes[nodeId]->name[2] = fields[2].GetCppString();
                m_TaxiNodes[nodeId]->name[3] = fields[3].GetCppString();
                m_TaxiNodes[nodeId]->name[4] = fields[4].GetCppString();
                m_TaxiNodes[nodeId]->name[5] = fields[5].GetCppString();
                m_TaxiNodes[nodeId]->name[6] = fields[6].GetCppString();

            } while (result->NextRow());
        }
    }
}

void ObjectMgr::LoadTaxiPathTransitions()
{
    m_TaxiPathTransitions.clear();                                            // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `in_path`, `out_path`, `in_node`, `out_node` FROM `taxi_path_transitions`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 inPath = fields[0].GetUInt32();
        uint32 outPath = fields[1].GetUInt32();
        uint32 inNode = fields[2].GetUInt32();
        uint32 outNode = fields[3].GetUInt32();

        if (!sTaxiPathStore.LookupEntry(inPath))
        {
            sLog.outErrorDb("Table `taxi_path_transitions`: inPath %u does not exist", inPath);
            continue;
        }
        if (!sTaxiPathStore.LookupEntry(outPath))
        {
            sLog.outErrorDb("Table `taxi_path_transitions`: outPath %u does not exist", outPath);
            continue;
        }
        if (inNode >= sTaxiPathNodesByPath[inPath].size())
        {
            sLog.outErrorDb("Table `taxi_path_transitions`: inNode %u does not exist in inPath %u", inNode, inPath);
            continue;
        }
        if (outNode >= sTaxiPathNodesByPath[outPath].size())
        {
            sLog.outErrorDb("Table `taxi_path_transitions`: outNode %u does not exist in outPath %u", outNode, outPath);
            continue;
        }

        auto bounds = m_TaxiPathTransitions.equal_range(inPath);
        for (auto it = bounds.first; it != bounds.second; ++it)
        {
            if (it->second.outPath == outPath)
            {
                sLog.outErrorDb("Table `taxi_path_transitions`: duplicate of (inPath %u, outPath %u)", inPath, outPath);
                continue;
            }
        }

        TaxiPathTransition transition;

        transition.inPath = inPath;
        transition.outPath = outPath;
        transition.inNode = inNode;
        transition.outNode = outNode;

        m_TaxiPathTransitions.insert(TaxiPathTransitionsMap::value_type(inPath, transition));
    } while (result->NextRow());
}

void ObjectMgr::LoadReservedPlayersNames()
{
    m_ReservedNames.clear();                                // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `name` FROM `reserved_name`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        std::string name = fields[0].GetCppString();
        std::wstring wstr;
        if (!Utf8toWStr(name, wstr))
        {
            sLog.outError("Table `reserved_name` have invalid name: %s", name.c_str());
            continue;
        }

        wstrToLower(wstr);

        m_ReservedNames.insert(wstr);
    }
    while (result->NextRow());

    if (sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK))
    {
        std::unique_ptr<QueryResult> result2(CharacterDatabase.Query("SELECT `name` FROM `char_transfer_names`"));

        if (!result2)
        {
            return;
        }

        do
        {
            Field* fields = result2->Fetch();

            std::string name = fields[0].GetCppString();
            std::wstring wstr;
            if (!Utf8toWStr(name, wstr))
            {
                sLog.outError("Table `char_transfer_names` have invalid name: %s", name.c_str());
                continue;
            }

            wstrToLower(wstr);

            m_ReservedNames.insert(wstr);
        } while (result2->NextRow());
    }
}

void ObjectMgr::AddReservedName(std::string name)
{
    std::wstring wstr;
    if (!Utf8toWStr(name, wstr))
        return;

    wstrToLower(wstr);
    m_ReservedNames.insert(wstr);
}

bool ObjectMgr::IsReservedName(std::string const& name) const
{
    std::wstring wstr;
    if (!Utf8toWStr(name, wstr))
        return false;

    wstrToLower(wstr);

    return m_ReservedNames.find(wstr) != m_ReservedNames.end();
}

enum LanguageType
{
    LT_BASIC_LATIN    = 0x0000,
    LT_EXTENDEN_LATIN = 0x0001,
    LT_CYRILLIC       = 0x0002,
    LT_EAST_ASIA      = 0x0004,
    LT_ANY            = 0xFFFF
};

static LanguageType GetRealmLanguageType(bool create)
{
    switch (sWorld.getConfig(CONFIG_UINT32_REALM_ZONE))
    {
        case REALM_ZONE_UNKNOWN:                            // any language
        case REALM_ZONE_DEVELOPMENT:
        case REALM_ZONE_TEST_SERVER:
        case REALM_ZONE_QA_SERVER:
            return LT_ANY;
        case REALM_ZONE_UNITED_STATES:                      // extended-Latin
        case REALM_ZONE_OCEANIC:
        case REALM_ZONE_LATIN_AMERICA:
        case REALM_ZONE_ENGLISH:
        case REALM_ZONE_GERMAN:
        case REALM_ZONE_FRENCH:
        case REALM_ZONE_SPANISH:
            return LT_EXTENDEN_LATIN;
        case REALM_ZONE_KOREA:                              // East-Asian
        case REALM_ZONE_TAIWAN:
        case REALM_ZONE_CHINA:
            return LT_EAST_ASIA;
        case REALM_ZONE_RUSSIAN:                            // Cyrillic
            return LT_CYRILLIC;
        default:
            return create ? LT_BASIC_LATIN : LT_ANY;        // basic-Latin at create, any at login
    }
}

bool isValidString(std::wstring const& wstr, uint32 strictMask, bool numericOrSpace, bool create = false)
{
    if (strictMask == 0)                                    // any language, ignore realm
    {
        if (isExtendedLatinString(wstr, numericOrSpace))
            return true;
        if (isCyrillicString(wstr, numericOrSpace))
            return true;
        if (isEastAsianString(wstr, numericOrSpace))
            return true;
        return false;
    }

    if (strictMask & 0x2)                                   // realm zone specific
    {
        LanguageType lt = GetRealmLanguageType(create);
        if (lt & LT_EXTENDEN_LATIN)
            if (isExtendedLatinString(wstr, numericOrSpace))
                return true;
        if (lt & LT_CYRILLIC)
            if (isCyrillicString(wstr, numericOrSpace))
                return true;
        if (lt & LT_EAST_ASIA)
            if (isEastAsianString(wstr, numericOrSpace))
                return true;
    }

    if (strictMask & 0x1)                                   // basic Latin
    {
        if (isBasicLatinString(wstr, numericOrSpace))
            return true;
    }

    return false;
}

uint8 ObjectMgr::CheckPlayerName(std::string const& name, bool create)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return CHAR_NAME_INVALID_CHARACTER;

    if (wname.size() > MAX_PLAYER_NAME)
        return CHAR_NAME_TOO_LONG;

    uint32 minName = sWorld.getConfig(CONFIG_UINT32_MIN_PLAYER_NAME);
    if (wname.size() < minName)
        return CHAR_NAME_TOO_SHORT;

    uint32 strictMask = sWorld.getConfig(CONFIG_UINT32_STRICT_PLAYER_NAMES);
    if (!isValidString(wname, strictMask, false, create))
        return CHAR_NAME_MIXED_LANGUAGES;

    return ValidateName(wname);
}

bool ObjectMgr::IsValidCharterName(std::string const& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    if (wname.size() > MAX_CHARTER_NAME)
        return false;

    uint32 minName = sWorld.getConfig(CONFIG_UINT32_MIN_CHARTER_NAME);
    if (wname.size() < minName)
        return false;

    uint32 strictMask = sWorld.getConfig(CONFIG_UINT32_STRICT_CHARTER_NAMES);

    return isValidString(wname, strictMask, true);
}

PetNameInvalidReason ObjectMgr::CheckPetName(std::string const& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return PET_NAME_INVALID;

    if (wname.size() > MAX_PET_NAME)
        return PET_NAME_TOO_LONG;

    uint32 minName = sWorld.getConfig(CONFIG_UINT32_MIN_PET_NAME);
    if (wname.size() < minName)
        return PET_NAME_TOO_SHORT;

    uint32 strictMask = sWorld.getConfig(CONFIG_UINT32_STRICT_PET_NAMES);
    if (!isValidString(wname, strictMask, false))
        return PET_NAME_MIXED_LANGUAGES;

    switch (ValidateName(wname))
    {
        case CHAR_NAME_PROFANE:
            return PET_NAME_PROFANE;
        case CHAR_NAME_RESERVED:
            return PET_NAME_RESERVED;
        default:
            break;
    }

    return PET_NAME_SUCCESS;
}

int ObjectMgr::GetIndexForLocale(LocaleConstant loc)
{
    if (loc == LOCALE_enUS)
        return -1;

    for (size_t i = 0; i < m_LocalForIndex.size(); ++i)
        if (m_LocalForIndex[i] == loc)
            return i;

    return -1;
}

LocaleConstant ObjectMgr::GetLocaleForIndex(int i)
{
    if (i < 0 || i >= (int32)m_LocalForIndex.size())
        return LOCALE_enUS;

    return m_LocalForIndex[i];
}

int ObjectMgr::GetOrNewIndexForLocale(LocaleConstant loc)
{
    if (loc == LOCALE_enUS)
        return -1;

    for (size_t i = 0; i < m_LocalForIndex.size(); ++i)
        if (m_LocalForIndex[i] == loc)
            return i;

    m_LocalForIndex.push_back(loc);
    return m_LocalForIndex.size() - 1;
}

void ObjectMgr::LoadGameObjectForQuests()
{
    m_GameObjectForQuestSet.clear();                         // need for reload case

    if (m_GameObjectInfoMap.empty())
    {
        return;
    }

    // collect GO entries for GO that must activated
    for (auto itr = m_GameObjectInfoMap.begin(); itr != m_GameObjectInfoMap.end(); ++itr)
    {
        switch (itr->second.type)
        {
            case GAMEOBJECT_TYPE_QUESTGIVER:
            {
                if (m_GOQuestRelations.find(itr->second.id) != m_GOQuestRelations.end() ||
                        m_GOQuestInvolvedRelations.find(itr->second.id) != m_GOQuestInvolvedRelations.end())
                {
                    m_GameObjectForQuestSet.insert(itr->second.id);
                }

                break;
            }
            case GAMEOBJECT_TYPE_CHEST:
            {
                // scan GO chest with loot including quest items
                uint32 loot_id = itr->second.GetLootId();

                // always activate to quest, GO may not have loot, OR find if GO has loot for quest.
                if (itr->second.chest.questId || LootTemplates_Gameobject.HaveQuestLootFor(loot_id))
                {
                    m_GameObjectForQuestSet.insert(itr->second.id);
                }
                break;
            }
            case GAMEOBJECT_TYPE_GENERIC:
            {
                if (itr->second._generic.questID)               // quest related objects, has visual effects
                {
                    m_GameObjectForQuestSet.insert(itr->second.id);
                }
                break;
            }
            case GAMEOBJECT_TYPE_SPELL_FOCUS:
            {
                if (itr->second.spellFocus.questID)             // quest related objects, has visual effect
                {
                    m_GameObjectForQuestSet.insert(itr->second.id);
                }
                break;
            }
            case GAMEOBJECT_TYPE_GOOBER:
            {
                if (itr->second.goober.questId > 0)             // quests objects
                {
                    m_GameObjectForQuestSet.insert(itr->second.id);
                }
                break;
            }
            default:
                break;
        }
    }
}

void ObjectMgr::LoadSkillLineAbility()
{
    // Getting the maximum ID.
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT MAX(`id`) FROM `skill_line_ability`"));

    if (!result)
    {
        return;
    }
    Field* fields = result->Fetch();
    uint32 maxSkillLineAbilityId = fields[0].GetUInt32() + 1;

    // Actually loading the skills.
    result.reset(WorldDatabase.Query("SELECT * FROM `skill_line_ability`"));

    if (!result)
    {
        return;
    }

    m_SkillLineAbilities.resize(maxSkillLineAbilityId);

    do
    {
        fields = result->Fetch();

        std::unique_ptr<SkillLineAbilityEntry> skill = std::make_unique<SkillLineAbilityEntry>();
        uint32 id = fields[0].GetUInt32();

        skill->id = id;
        skill->skillId = fields[1].GetUInt32();
        skill->spellId = fields[2].GetUInt32();
        skill->racemask = fields[3].GetUInt32();
        skill->classmask = fields[4].GetUInt32();
        skill->req_skill_value = fields[5].GetUInt32();
        skill->forward_spellid = fields[6].GetUInt32();
        skill->learnOnGetSkill = fields[7].GetUInt32();
        skill->max_value = fields[8].GetUInt32();
        skill->min_value = fields[9].GetUInt32();
        skill->reqtrainpoints = fields[10].GetUInt32();

        m_SkillLineAbilities[id] = std::move(skill);

    } while (result->NextRow());
}

void ObjectMgr::LoadSoundEntries()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT * FROM `sound_entries`"));

    if (!result)
        return;

    do
    {
        Field const* fields = result->Fetch();

        SoundEntriesEntry sound;
        const uint32 soundId = fields[0].GetUInt32();

        sound.Id = soundId;
        sound.Name = fields[1].GetCppString();

        m_SoundEntriesMap[soundId] = sound;

    } while (result->NextRow());
}

void ObjectMgr::LoadBroadcastTexts()
{
    m_BroadcastTextLocaleMap.clear(); // for reload case

    //                                                               0        1            2              3           4            5              6            7            8            9               10              11
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `male_text`, `female_text`, `chat_type`, `sound_id`, `language_id`, `emote_id1`, `emote_id2`, `emote_id3`, `emote_delay1`, `emote_delay2`, `emote_delay3` FROM `broadcast_text`"));
    if (!result)
    {
        return;
    }

    m_BroadcastTextLocaleMap.rehash(result->GetRowCount());

    do
    {
        Field* fields = result->Fetch();

        BroadcastText bct;

        bct.entry = fields[0].GetUInt32();
        bct.maleText[LOCALE_enUS] = fields[1].GetString() ? fields[1].GetString() : std::string();
        bct.femaleText[LOCALE_enUS] = fields[2].GetString() ? fields[2].GetString() : std::string();
        bct.chatType = fields[3].GetUInt32();
        bct.soundId = fields[4].GetUInt32();
        bct.languageId = fields[5].GetUInt32();
        bct.emoteId1 = fields[6].GetUInt32();
        bct.emoteId2 = fields[7].GetUInt32();
        bct.emoteId3 = fields[8].GetUInt32();
        bct.emoteDelay1 = fields[9].GetUInt32();
        bct.emoteDelay2 = fields[10].GetUInt32();
        bct.emoteDelay3 = fields[11].GetUInt32();

        if (bct.soundId)
        {
            if (!GetSoundEntry(bct.soundId))
            {
                sLog.outErrorDb("BroadcastText (Id: %u) in table `broadcast_text` has SoundId %u but sound does not exist.", bct.entry, bct.soundId);
                bct.soundId = 0;
            }
        }

        if (!GetLanguageDescByID(bct.languageId))
        {
            sLog.outErrorDb("BroadcastText (Id: %u) in table `broadcast_text` using LanguageId %u but language does not exist.", bct.entry, bct.languageId);
            bct.languageId = LANG_UNIVERSAL;
        }

        if (bct.chatType > CHAT_TYPE_ZONE_YELL)
        {
            sLog.outErrorDb("BroadcastText (Id: %u) in table `broadcast_text` has ChatType %u but this chat type does not exist.", bct.entry, bct.chatType);
            bct.chatType = CHAT_TYPE_SAY;
        }

        if (bct.emoteId1)
        {
            if (!sEmotesStore.LookupEntry(bct.emoteId1))
            {
                sLog.outErrorDb("BroadcastText (Id: %u) in table `broadcast_text` has emoteId2 %u but emote does not exist.", bct.entry, bct.emoteId1);
                bct.emoteId1 = 0;
            }
        }

        if (bct.emoteId2)
        {
            if (!sEmotesStore.LookupEntry(bct.emoteId2))
            {
                sLog.outErrorDb("BroadcastText (Id: %u) in table `broadcast_text` has emoteId3 %u but emote does not exist.", bct.entry, bct.emoteId2);
                bct.emoteId2 = 0;
            }
        }

        if (bct.emoteId3)
        {
            if (!sEmotesStore.LookupEntry(bct.emoteId3))
            {
                sLog.outErrorDb("BroadcastText (Id: %u) in table `broadcast_text` has EmoteId3 %u but emote does not exist.", bct.entry, bct.emoteId3);
                bct.emoteId3 = 0;
            }
        }

        m_BroadcastTextLocaleMap[bct.entry] = bct;
    } while (result->NextRow());
}

void ObjectMgr::LoadBroadcastTextLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    //                                                               0     1                 2                 3                 4                 5                 6                 7                 8                 9                   10                  11                  12                  13                  14                  15                  16
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `male_text_loc1`, `male_text_loc2`, `male_text_loc3`, `male_text_loc4`, `male_text_loc5`, `male_text_loc6`, `male_text_loc7`, `male_text_loc8`, `female_text_loc1`, `female_text_loc2`, `female_text_loc3`, `female_text_loc4`, `female_text_loc5`, `female_text_loc6`, `female_text_loc7`, `female_text_loc8` FROM `locales_broadcast_text`"));
    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        BroadcastTextLocaleMap::iterator bct = m_BroadcastTextLocaleMap.find(entry);

        if (bct == m_BroadcastTextLocaleMap.end())
        {
            sLog.outErrorDb("BroadcastText (Id: %u) in table `locales_broadcast_text` does not exist. Skipped!", entry);
            continue;
        }

        BroadcastText& data = m_BroadcastTextLocaleMap[entry];

        // Load maleText
        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    // 0 -> default, idx in to idx+1
                    if ((int32)data.maleText.size() <= idx + 1)
                        data.maleText.resize(idx + 2);

                    data.maleText[idx + 1] = str;
                }
            }
        }

        // Load femaleText
        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[8 + i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    // 0 -> default, idx in to idx+1
                    if ((int32)data.femaleText.size() <= idx + 1)
                        data.femaleText.resize(idx + 2);

                    data.femaleText[idx + 1] = str;
                }
            }
        }
    } while (result->NextRow());
}

const char *ObjectMgr::GetBroadcastText(uint32 id, int locale_index, uint8 gender, bool forceGender) const
{
    if (BroadcastText const* bct = GetBroadcastTextLocale(id))
    {
        if ((gender == GENDER_FEMALE || gender == GENDER_NONE) && (forceGender || !bct->femaleText[LOCALE_enUS].empty()))
        {
            if ((int32)bct->femaleText.size() > locale_index + 1 && !bct->femaleText[locale_index + 1].empty())
                return bct->femaleText[locale_index + 1].c_str();
            else
                return bct->femaleText[0].c_str();
        }
        // else if (gender == GENDER_MALE)
        {
            if ((int32)bct->maleText.size() > locale_index + 1 && !bct->maleText[locale_index + 1].empty())
                return bct->maleText[locale_index + 1].c_str();
            else
                return bct->maleText[0].c_str();
        }
    }

    sLog.outErrorDb("Broadcast text id %i not found in DB.", id);
    return "<error>";
}

bool ObjectMgr::LoadModuleStrings()
{
    m_ModuleStringLocaleMap.clear();

    std::unique_ptr<QueryResult> tableResult(WorldDatabase.PQuery("SHOW TABLES LIKE 'module_string'"));
    if (!tableResult)
    {
        sLog.outInfo("Table `module_string` not found, module strings not loaded.");
        return true;
    }

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `module`, `id`, `content_default` FROM `module_string`"));
    if (!result)
    {
        sLog.outInfo("Loaded 0 module strings.");
        return true;
    }

    do
    {
        Field* fields = result->Fetch();
        std::string module = fields[0].GetCppString();
        uint32 id = fields[1].GetUInt32();

        if (module.empty())
        {
            sLog.outErrorDb("Table `module_string` contains empty module name for id %u, ignored.", id);
            continue;
        }

        if (!id)
        {
            sLog.outErrorDb("Table `module_string` contains reserved id 0 for module `%s`, ignored.", module.c_str());
            continue;
        }

        MangosStringLocale& data = m_ModuleStringLocaleMap[module][id];
        if (!data.Content.empty())
        {
            sLog.outErrorDb("Table `module_string` contains duplicate string `%s`:%u, ignored.", module.c_str(), id);
            continue;
        }

        data.Content.resize(1);
        data.Content[0] = fields[2].GetCppString();
    }
    while (result->NextRow());

    if (sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
    {
        std::unique_ptr<QueryResult> localeTableResult(WorldDatabase.PQuery("SHOW TABLES LIKE 'module_string_locale'"));
        if (localeTableResult)
        {
            std::unique_ptr<QueryResult> localeResult(WorldDatabase.Query("SELECT `module`, `id`, `locale`, `content` FROM `module_string_locale`"));
            if (localeResult)
            {
                do
                {
                    Field* fields = localeResult->Fetch();
                    std::string module = fields[0].GetCppString();
                    uint32 id = fields[1].GetUInt32();
                    uint32 locale = fields[2].GetUInt32();
                    std::string content = fields[3].GetCppString();

                    auto moduleItr = m_ModuleStringLocaleMap.find(module);
                    if (moduleItr == m_ModuleStringLocaleMap.end())
                    {
                        sLog.outErrorDb("Table `module_string_locale` contains locale for nonexistent module string `%s`:%u, skipped.", module.c_str(), id);
                        continue;
                    }

                    auto stringItr = moduleItr->second.find(id);
                    if (stringItr == moduleItr->second.end())
                    {
                        sLog.outErrorDb("Table `module_string_locale` contains locale for nonexistent module string `%s`:%u, skipped.", module.c_str(), id);
                        continue;
                    }

                    if (locale == LOCALE_enUS || locale >= MAX_LOCALE)
                    {
                        sLog.outErrorDb("Table `module_string_locale` contains invalid locale %u for module string `%s`:%u, skipped.", locale, module.c_str(), id);
                        continue;
                    }

                    int idx = GetOrNewIndexForLocale(LocaleConstant(locale));
                    if (idx >= 0)
                    {
                        MangosStringLocale& data = stringItr->second;
                        if ((int32)data.Content.size() <= idx + 1)
                            data.Content.resize(idx + 2);

                        data.Content[idx + 1] = content;
                    }
                }
                while (localeResult->NextRow());
            }
        }
    }

    size_t stringCount = 0;
    for (ModuleStringLocaleMap::value_type const& modulePair : m_ModuleStringLocaleMap)
        stringCount += modulePair.second.size();

    sLog.outString("Loaded %u module string%s from %u module%s.", uint32(stringCount), stringCount == 1 ? "" : "s",
        uint32(m_ModuleStringLocaleMap.size()), m_ModuleStringLocaleMap.size() == 1 ? "" : "s");
    return true;
}

const char* ObjectMgr::GetModuleString(std::string const& module, uint32 id, int locale_idx) const
{
    ModuleStringLocaleMap::const_iterator moduleItr = m_ModuleStringLocaleMap.find(module);
    if (moduleItr != m_ModuleStringLocaleMap.end())
    {
        MangosStringLocaleMap::const_iterator stringItr = moduleItr->second.find(id);
        if (stringItr != moduleItr->second.end())
        {
            MangosStringLocale const& data = stringItr->second;
            if ((int32)data.Content.size() > locale_idx + 1 && !data.Content[locale_idx + 1].empty())
                return data.Content[locale_idx + 1].c_str();

            if (!data.Content.empty())
                return data.Content[0].c_str();
        }
    }

    sLog.outErrorDb("Module string `%s`:%u not found in DB.", module.c_str(), id);
    return "<error>";
}

bool ObjectMgr::LoadMangosStrings(DatabaseType& db, char const* table, int32 min_value, int32 max_value, bool extra_content)
{
    int32 start_value = min_value;
    int32 end_value   = max_value;
    // some string can have negative indexes range
    if (start_value < 0)
    {
        if (end_value >= start_value)
        {
            sLog.outErrorDb("Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }

        // real range (max+1,min+1) exaple: (-10,-1000) -> -999...-10+1
        std::swap(start_value, end_value);
        ++start_value;
        ++end_value;
    }
    else
    {
        if (start_value >= end_value)
        {
            sLog.outErrorDb("Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }
    }

    // cleanup affected map part for reloading case
    for (MangosStringLocaleMap::iterator itr = m_MangosStringLocaleMap.begin(); itr != m_MangosStringLocaleMap.end();)
    {
        if (itr->first >= start_value && itr->first < end_value)
            m_MangosStringLocaleMap.erase(itr++);
        else
            ++itr;
    }

    std::unique_ptr<QueryResult> result(db.PQuery("SELECT `entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8` %s FROM %s", extra_content ? ", `sound`, `type`, `language`, `emote`" : "", table));

    if (!result)
    {
        return false;
    }

    do
    {
        Field *fields = result->Fetch();

        int32 entry = fields[0].GetInt32();

        if (entry == 0)
        {
            sLog.outErrorDb("Table `%s` contain reserved entry 0, ignored.", table);
            continue;
        }
        else if (entry < start_value || entry >= end_value)
        {
            sLog.outErrorDb("Table `%s` contain entry %i out of allowed range (%d - %d), ignored.", table, entry, min_value, max_value);
            continue;
        }

        MangosStringLocale& data = m_MangosStringLocaleMap[entry];

        if (!data.Content.empty())
        {
            sLog.outErrorDb("Table `%s` contain data for already loaded entry  %i (from another table?), ignored.", table, entry);
            continue;
        }

        data.Content.resize(1);

        // 0 -> default, idx in to idx+1
        data.Content[0] = fields[1].GetCppString();

        if (sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        {
            for (int i = 1; i < MAX_LOCALE; ++i)
            {
                std::string str = fields[i + 1].GetCppString();
                if (!str.empty())
                {
                    int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        // 0 -> default, idx in to idx+1
                        if ((int32)data.Content.size() <= idx + 1)
                            data.Content.resize(idx + 2);

                        data.Content[idx + 1] = str;
                    }
                }
            }
        }

        // Load additional string content if necessary
        if (extra_content)
        {
            data.SoundId     = fields[10].GetUInt32();
            data.Type        = fields[11].GetUInt32();
            data.LanguageId  = Language(fields[12].GetUInt32());
            data.Emote       = fields[13].GetUInt32();

            if (data.SoundId && !GetSoundEntry(data.SoundId))
            {
                sLog.outErrorDb("Entry %i in table `%s` has soundId %u but sound does not exist.", entry, table, data.SoundId);
                data.SoundId = 0;
            }

            if (!GetLanguageDescByID(data.LanguageId))
            {
                sLog.outErrorDb("Entry %i in table `%s` using Language %u but Language does not exist.", entry, table, uint32(data.LanguageId));
                data.LanguageId = LANG_UNIVERSAL;
            }

            if (data.Type > CHAT_TYPE_ZONE_YELL)
            {
                sLog.outErrorDb("Entry %i in table `%s` has Type %u but this Chat Type does not exist.", entry, table, data.Type);
                data.Type = CHAT_TYPE_SAY;
            }

            if (data.Emote && !sEmotesStore.LookupEntry(data.Emote))
            {
                sLog.outErrorDb("Entry %i in table `%s` has Emote %u but emote does not exist.", entry, table, data.Emote);
                data.Emote = EMOTE_ONESHOT_NONE;
            }
        }
    }
    while (result->NextRow());

    return true;
}

const char *ObjectMgr::GetMangosString(int32 entry, int locale_idx) const
{
    // locale_idx==-1 -> default, locale_idx >= 0 in to idx+1
    // Content[0] always exist if exist MangosStringLocale
    if (MangosStringLocale const *msl = GetMangosStringLocale(entry))
    {
        if ((int32)msl->Content.size() > locale_idx + 1 && !msl->Content[locale_idx + 1].empty())
            return msl->Content[locale_idx + 1].c_str();
        else
            return msl->Content[0].c_str();
    }

    if (entry > 0)
        sLog.outErrorDb("Entry %i not found in `mangos_string` table.", entry);
    else
        sLog.outErrorDb("Mangos string entry %i not found in DB.", entry);
    return "<error>";
}

bool ObjectMgr::LoadQuestGreetings()
{
    for (auto& i : m_QuestGreetingLocaleMap)
        i.clear(); // need for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `type`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `emote_id`, `emote_delay` FROM `quest_greeting`"));

    if (!result)
    {
        return false;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        uint8 type = fields[1].GetUInt8();

        switch (type)
        {
            case QUESTGIVER_CREATURE:
            {
                if (!sObjectMgr.GetCreatureTemplate(entry))
                {
                    if (!IsExistingCreatureId(entry))
                        sLog.outErrorDb("Table `quest_greeting` have entry for nonexistent creature template (Entry: %u), ignore", entry);
                    continue;
                }
                break;
            }
            case QUESTGIVER_GAMEOBJECT:
            {
                if (!sObjectMgr.GetGameObjectInfo(entry))
                {
                    sLog.outErrorDb("Table `quest_greeting` have entry for nonexistent gameobject template (Entry: %u), ignore", entry);
                    continue;
                }
                break;
            }
            default:
            {
                sLog.outErrorDb("Table `quest_greeting` have entry with invalid type (Type: %u), ignore", type);
                continue;
            }
        }

        QuestGreetingLocale& data = m_QuestGreetingLocaleMap[type][entry];

        data.Content.resize(1);

        // 0 -> default, idx in to idx+1
        data.Content[0] = fields[2].GetCppString();

        if (sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        {
            for (int i = 1; i < MAX_LOCALE; ++i)
            {
                std::string str = fields[i + 2].GetCppString();
                if (!str.empty())
                {
                    int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        // 0 -> default, idx in to idx+1
                        if ((int32)data.Content.size() <= idx + 1)
                            data.Content.resize(idx + 2);

                        data.Content[idx + 1] = str;
                    }
                }
            }
        }

        data.Emote = fields[11].GetUInt16();
        data.EmoteDelay = fields[12].GetUInt32();

        if (data.Emote && !sEmotesStore.LookupEntry(data.Emote))
        {
            sLog.outErrorDb("Entry %i in table `quest_greeting` has Emote %u but emote does not exist.", entry, data.Emote);
            data.Emote = EMOTE_ONESHOT_NONE;
        }
    } while (result->NextRow());

    return true;
}

bool ObjectMgr::LoadTrainerGreetings()
{
    m_TrainerGreetingLocaleMap.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8` FROM `npc_trainer_greeting`"));

    if (!result)
    {
        return false;
    }

    do
    {
        Field *fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();

        if (!sObjectMgr.GetCreatureTemplate(entry))
        {
            if (!IsExistingCreatureId(entry))
                sLog.outErrorDb("Table `npc_trainer_greeting` have entry for nonexistent creature template (Entry: %u), ignore", entry);
            continue;
        }

        TrainerGreetingLocale& data = m_TrainerGreetingLocaleMap[entry];

        data.Content.resize(1);

        // 0 -> default, idx in to idx+1
        data.Content[0] = fields[1].GetCppString();

        if (sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        {
            for (int i = 1; i < MAX_LOCALE; ++i)
            {
                std::string str = fields[i + 1].GetCppString();
                if (!str.empty())
                {
                    int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        // 0 -> default, idx in to idx+1
                        if ((int32)data.Content.size() <= idx + 1)
                            data.Content.resize(idx + 2);

                        data.Content[idx + 1] = str;
                    }
                }
            }
        }

    } while (result->NextRow());

    return true;
}

void ObjectMgr::LoadCustomCharacterSkins()
{
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT * FROM `custom_character_skins`"));

    if (!result)
        return;
    do
    {
        Field* fields = result->Fetch();
        CustomCharacterSkinEntry custom_character_skin;

        uint32 token_id = fields[0].GetUInt32();

        custom_character_skin.male_id = fields[1].GetUInt32();
        custom_character_skin.female_id = fields[2].GetUInt32();

        m_CustomCharacterSkinMap[token_id] = custom_character_skin;

    } while (result->NextRow());
}

void ObjectMgr::LoadFishingBaseSkillLevel()
{
    m_FishingBaseSkillMap.clear();                            // for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `skill` FROM `skill_fishing_base_level`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 entry  = fields[0].GetUInt32();
        int32 skill   = fields[1].GetInt32();

        const auto *fArea = AreaEntry::GetById(entry);
        if (!fArea)
        {
            sLog.outErrorDb("AreaId %u defined in `skill_fishing_base_level` does not exist", entry);
            continue;
        }

        m_FishingBaseSkillMap[entry] = skill;
    }
    while (result->NextRow());
}

SkillRangeType GetSkillRangeType(SkillLineEntry const* skill, SkillRaceClassInfoEntry const* rcEntry)
{
    if (sSkillTiersStore.LookupEntry(rcEntry->skillTierId))
        return SKILL_RANGE_RANK;

    switch (skill->categoryId)
    {
        case SKILL_CATEGORY_ARMOR:
            return SKILL_RANGE_MONO;
        case SKILL_CATEGORY_LANGUAGES:
            return SKILL_RANGE_LANGUAGE;
    }

    return SKILL_RANGE_LEVEL;
}

void ObjectMgr::LoadGameTele()
{
    m_GameTeleMap.clear();                                  // for reload case

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `id`, `position_x`, `position_y`, `position_z`, `orientation`, `map`, `name` FROM `game_tele`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 id         = fields[0].GetUInt32();
        GameTele gt;

        gt.x     = fields[1].GetFloat();
        gt.y     = fields[2].GetFloat();
        gt.z     = fields[3].GetFloat();
        gt.o     = fields[4].GetFloat();
        gt.mapId = fields[5].GetUInt32();
        gt.name  = fields[6].GetCppString();

        if (!MapManager::IsValidMapCoord(gt.mapId, gt.x, gt.y, gt.z, gt.o))
        {
            sLog.outErrorDb("Wrong position for id %u (name: %s) in `game_tele` table, ignoring.", id, gt.name.c_str());
            continue;
        }

        if (!Utf8toWStr(gt.name, gt.wnameLow))
        {
            sLog.outErrorDb("Wrong UTF8 name for id %u in `game_tele` table, ignoring.", id);
            continue;
        }

        wstrToLower(gt.wnameLow);

        m_GameTeleMap[id] = gt;
    }
    while (result->NextRow());
}

GameTele const* ObjectMgr::GetGameTele(std::string const& name) const
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return nullptr;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    // Alternative first GameTele what contains wnameLow as substring in case no GameTele location found
    const GameTele* alt = nullptr;
    for (const auto& itr : m_GameTeleMap)
        if (itr.second.wnameLow == wname)
            return &itr.second;
        else if (alt == nullptr && itr.second.wnameLow.find(wname) != std::wstring::npos)
            alt = &itr.second;

    return alt;
}

bool ObjectMgr::AddGameTele(GameTele& tele)
{
    // find max id
    uint32 new_id = 0;
    for (const auto & itr : m_GameTeleMap)
    {
        if (itr.first > new_id)
            new_id = itr.first;
    }

    // use next
    ++new_id;

    if (!Utf8toWStr(tele.name, tele.wnameLow))
        return false;

    wstrToLower(tele.wnameLow);

    m_GameTeleMap[new_id] = tele;

    return sWorld.ExecuteUpdate("REPLACE INTO `game_tele` (`id`, `position_x`, `position_y`, `position_z`, `orientation`, `map`, `name`) VALUES (%u,%f,%f,%f,%f,%u,'%s')", new_id, tele.x, tele.y, tele.z, tele.o, tele.mapId, tele.name.c_str());
}

bool ObjectMgr::DeleteGameTele(std::string const& name)
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    for (GameTeleMap::iterator itr = m_GameTeleMap.begin(); itr != m_GameTeleMap.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
        {
            sWorld.ExecuteUpdate("DELETE FROM `game_tele` WHERE `name` = '%s'", itr->second.name.c_str());
            m_GameTeleMap.erase(itr);
            return true;
        }
    }

    return false;
}

void ObjectMgr::LoadTrainers(char const* tableName, bool isTemplates)
{
    CacheTrainerSpellMap& trainerList = isTemplates ? m_CacheTrainerTemplateSpellMap : m_CacheTrainerSpellMap;

    // For reload case
    for (robin_hood::pair<const uint32, TrainerSpellData>& itr : trainerList)
    {
        itr.second.Clear();
    }
    trainerList.clear();

    std::set<uint32> skip_trainers;

    std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT `entry`, `spell`, `spellcost`, `reqskill`, `reqskillvalue`, `reqlevel` FROM %s", tableName));

    if (!result)
    {
        return;
    }

    std::set<uint32> talentIds;

    do
    {
        Field* fields = result->Fetch();

        uint32 entry  = fields[0].GetUInt32();
        uint32 spell  = fields[1].GetUInt32();

        SpellEntry const *spellinfo = sSpellMgr.GetSpellEntry(spell);
        if (!spellinfo)
        {
            sLog.outErrorDb("Table `%s` (Entry: %u ) has non existing spell %u, ignore", tableName, entry, spell);
            continue;
        }

        if (spellinfo->Effect[0] != SPELL_EFFECT_LEARN_SPELL)
        {
            sLog.outErrorDb("Table `%s` for trainer (Entry: %u) has non-learning spell %u, ignore", tableName, entry, spell);
            for (uint32 spell2 = 1; spell2 < sSpellMgr.GetMaxSpellId(); ++spell2)
            {
                if (SpellEntry const* spellEntry2 = sSpellMgr.GetSpellEntry(spell2))
                {
                    if (spellEntry2->Effect[0] == SPELL_EFFECT_LEARN_SPELL && spellEntry2->EffectTriggerSpell[0] == spell)
                    {
                        sLog.outErrorDb("  ... possible must be used spell %u instead", spell2);
                        break;
                    }
                }
            }
            continue;
        }

        if (!SpellMgr::IsSpellValid(spellinfo))
        {
            sLog.outErrorDb("Table `%s` (Entry: %u) has broken learning spell %u, ignore", tableName, entry, spell);
            continue;
        }

        if (GetTalentSpellCost(spell))
        {
            if (talentIds.find(spell) == talentIds.end())
            {
                sLog.outErrorDb("Table `%s` has talent as learning spell %u, ignore", tableName, spell);
                talentIds.insert(spell);
            }
            continue;
        }

        if (!isTemplates)
        {
            CreatureInfo const* cInfo = GetCreatureTemplate(entry);

            if (!cInfo)
            {
                if (!IsExistingCreatureId(entry))
                    sLog.outErrorDb("Table `%s` have entry for nonexistent creature template (Entry: %u), ignore", tableName, entry);
                continue;
            }

            if (!(cInfo->npc_flags & UNIT_NPC_FLAG_TRAINER))
            {
                if (skip_trainers.find(entry) == skip_trainers.end())
                {
                    sLog.outErrorDb("Table `%s` have data for creature (Entry: %u) without trainer flag, ignore", tableName, entry);
                    skip_trainers.insert(entry);
                }
                continue;
            }

            if (TrainerSpellData const* tSpells = cInfo->trainer_id ? GetNpcTrainerTemplateSpells(cInfo->trainer_id) : nullptr)
            {
                if (tSpells->spellList.find(spell) != tSpells->spellList.end())
                {
                    sLog.outErrorDb("Table `%s` (Entry: %u) has spell %u listed in trainer template %u, ignore", tableName, entry, spell, cInfo->trainer_id);
                    continue;
                }
            }
        }

        TrainerSpellData& data = trainerList[entry];

        TrainerSpell& trainerSpell = data.spellList[spell];
        trainerSpell.spell         = spell;
        trainerSpell.spellCost     = fields[2].GetUInt32();
        trainerSpell.reqSkill      = fields[3].GetUInt32();
        trainerSpell.reqSkillValue = fields[4].GetUInt32();
        trainerSpell.reqLevel      = fields[5].GetUInt32();

        if (trainerSpell.reqLevel)
        {
            if (trainerSpell.reqLevel == spellinfo->spellLevel)
                ERROR_DB_STRICT_LOG("Table `%s` (Entry: %u) has redundant reqlevel %u (=spell level) for spell %u", tableName, entry, trainerSpell.reqLevel, spell);
        }
        else
            trainerSpell.reqLevel = spellinfo->spellLevel;

        if (SpellMgr::IsProfessionSpell(spellinfo->EffectTriggerSpell[0]))
            data.trainerType = 2;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadTrainerTemplates()
{
    LoadTrainers("npc_trainer_template", true);

    // post loading check
    std::set<uint32> trainer_ids;

    for (const auto& tItr : m_CacheTrainerTemplateSpellMap)
        trainer_ids.insert(tItr.first);

    // We need to use a query to get all used trainer ids because of progression.
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `trainer_id` FROM `creature_template` WHERE `trainer_id` != 0"));

    if (result)
    {
        Field* fields;
        do
        {
            fields = result->Fetch();
            uint32 creature_id = fields[0].GetUInt32();
            uint32 trainer_id = fields[1].GetUInt32();
            if (m_CacheTrainerTemplateSpellMap.find(trainer_id) != m_CacheTrainerTemplateSpellMap.end())
                trainer_ids.erase(trainer_id);
        } while (result->NextRow());
    }

    for (const auto trainer_id : trainer_ids)
        sLog.outErrorDb("Table `npc_trainer_template` has trainer template %u not used by any trainers ", trainer_id);
}

void ObjectMgr::LoadVendors(char const* tableName, bool isTemplates)
{
    CacheVendorItemMap& vendorList = isTemplates ? m_CacheVendorTemplateItemMap : m_CacheVendorItemMap;

    // For reload case
    for (auto& itr : vendorList)
        itr.second.Clear();

    vendorList.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT `entry`, `item`, `maxcount`, `incrtime`, `itemflags`, `condition_id` FROM %s ORDER BY `entry`, `slot`", tableName));
    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry        = fields[0].GetUInt32();
        uint32 item_id      = fields[1].GetUInt32();
        uint32 maxcount     = fields[2].GetUInt32();
        uint32 incrtime     = fields[3].GetUInt32();
        uint32 itemflags    = fields[4].GetUInt32();
        uint32 conditionId  = fields[5].GetUInt32();

        if (!IsVendorItemValid(isTemplates, tableName, entry, item_id, maxcount, incrtime, conditionId, nullptr))
            continue;

        VendorItemData& vList = vendorList[entry];

        vList.AddItem(item_id, maxcount, incrtime, itemflags, conditionId);
    }
    while (result->NextRow());
}


void ObjectMgr::LoadVendorTemplates()
{
    LoadVendors("npc_vendor_template", true);

    // post loading check
    std::set<uint32> vendor_ids;

    for (const auto& vItr : m_CacheVendorTemplateItemMap)
        vendor_ids.insert(vItr.first);

    // We need to use a query to get all used vendor ids because of progression.
    //                                                              0        1
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `vendor_id` FROM `creature_template` WHERE `vendor_id` != 0"));

    if (result)
    {
        Field* fields;
        do
        {
            fields = result->Fetch();
            uint32 creature_id = fields[0].GetUInt32();
            uint32 vendor_id = fields[1].GetUInt32();
            if (m_CacheVendorTemplateItemMap.find(vendor_id) != m_CacheVendorTemplateItemMap.end())
                vendor_ids.erase(vendor_id);
            else if (!IsExistingVendorTemplateId(vendor_id))
                sLog.outErrorDb("Creature (Entry: %u) has vendor_id = %u for nonexistent vendor template", creature_id, vendor_id);
        } while (result->NextRow());
    }

    for (const auto vendor_id : vendor_ids)
        sLog.outErrorDb("Table `npc_vendor_template` has vendor template %u not used by any vendors ", vendor_id);
}

void ObjectMgr::LoadNpcGossips()
{
    m_CacheNpcTextIdMap.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `npc_guid`, `textid` FROM `npc_gossip`"));
    if (!result)
    {
        return;
    }

    uint32 guid, textid;

    do
    {
        Field* fields = result->Fetch();

        guid   = fields[0].GetUInt32();
        textid = fields[1].GetUInt32();

        if (!GetCreatureData(guid))
        {
            if (!sObjectMgr.IsExistingCreatureGuid(guid))
                sLog.outErrorDb("Table `npc_gossip` have nonexistent creature (GUID: %u) entry, ignore. ", guid);
            continue;
        }
        if (!GetNpcText(textid))
        {
            sLog.outErrorDb("Table `npc_gossip` for creature (GUID: %u) have wrong Textid (%u), ignore. ", guid, textid);
            continue;
        }

        m_CacheNpcTextIdMap[guid] = textid ;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadGossipMenus()
{
    // Check which script-ids in gossip_scripts are not used
    std::set<uint32> gossipScriptSet;
    for (const auto& itr : sGossipScripts)
        gossipScriptSet.insert(itr.first);

    // Load gossip_menu and gossip_menu_option data
    sLog.outString("Loading Gossip menus...");
    LoadGossipMenu(gossipScriptSet);
    sLog.outString("Loading Gossip menu options...");
    LoadGossipMenuItems(gossipScriptSet);

    for (const auto itr : gossipScriptSet)
        sLog.outErrorDb("Table `gossip_scripts` contain unused script, id %u.", itr);
}

void ObjectMgr::LoadGossipMenu(std::set<uint32>& gossipScriptSet)
{
    m_GossipMenusMap.clear();

    //                                                               0        1          2            3
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `entry`, `text_id`, `script_id`, `condition_id` FROM `gossip_menu`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        GossipMenus gMenu;

        gMenu.entry             = fields[0].GetUInt32();
        gMenu.text_id           = fields[1].GetUInt32();
        gMenu.script_id         = fields[2].GetUInt32();
        gMenu.condition_id      = fields[3].GetUInt32();

        if (!GetNpcText(gMenu.text_id))
        {
            sLog.outErrorDb("Table gossip_menu entry %u are using non-existing text_id %u", gMenu.entry, gMenu.text_id);
            continue;
        }

        if (gMenu.script_id)
        {
            if (sGossipScripts.find(gMenu.script_id) == sGossipScripts.end())
            {
                sLog.outErrorDb("Table gossip_menu for menu %u, text-id %u have script_id %u that does not exist in `gossip_scripts`, ignoring", gMenu.entry, gMenu.text_id, gMenu.script_id);
                continue;
            }

            // Remove used script id
            gossipScriptSet.erase(gMenu.script_id);
        }

        if (gMenu.condition_id)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(gMenu.condition_id);
            if (!condition)
            {
                sLog.outErrorDb("Table gossip_menu for menu %u, text-id %u has condition_id %u that does not exist in `conditions`, ignoring", gMenu.entry, gMenu.text_id, gMenu.condition_id);
                gMenu.condition_id = 0;
            }
        }

        m_GossipMenusMap.insert(GossipMenusMap::value_type(gMenu.entry, gMenu));
    }
    while (result->NextRow());

    // post loading tests
    for (auto const& itr : m_creatureInfoMap)
            if (itr.second->gossip_menu_id)
                if (m_GossipMenusMap.find(itr.second->gossip_menu_id) == m_GossipMenusMap.end())
                    sLog.outErrorDb("Creature (Entry: %u) has gossip_menu_id = %u for nonexistent menu", itr.second->entry, itr.second->gossip_menu_id);

    for (auto itr = m_GameObjectInfoMap.begin(); itr != m_GameObjectInfoMap.end(); ++itr)
        if (uint32 menuid = itr->second.GetGossipMenuId())
            if (m_GossipMenusMap.find(menuid) == m_GossipMenusMap.end())
                ERROR_DB_STRICT_LOG("Gameobject (Entry: %u) has gossip_menu_id = %u for nonexistent menu", itr->second.id, menuid);
}

void ObjectMgr::LoadGossipMenuItems(std::set<uint32>& gossipScriptSet)
{
    m_GossipMenuItemsMap.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query(
                              "SELECT `menu_id`, `id`, `option_icon`, `option_text`, `option_broadcast_text`, `option_id`, `npc_option_npcflag`, "
                              "`action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`, `box_broadcast_text`, "
                              "`condition_id` "
                              "FROM `gossip_menu_option` ORDER BY `menu_id`, `id`"));

    if (!result)
    {
        return;
    }

    // prepare data for unused menu ids
    std::set<uint32> menu_ids;                              // for later integrity check
    if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))   // check unused menu ids only in strict mode
    {
        for (const auto& itr : m_GossipMenusMap)
            if (itr.first)
                menu_ids.insert(itr.first);

        for (auto itr = m_GameObjectInfoMap.begin(); itr != m_GameObjectInfoMap.end(); ++itr)
            if (uint32 menuid = itr->second.GetGossipMenuId())
                menu_ids.erase(menuid);
    }

    // loading

    // prepare menuid -> CreatureInfo map for fast access
    typedef  std::multimap<uint32, const CreatureInfo*> Menu2CInfoMap;
    Menu2CInfoMap menu2CInfoMap;
    for (auto const& itr : m_creatureInfoMap)
            if (itr.second->gossip_menu_id)
                menu2CInfoMap.insert(Menu2CInfoMap::value_type(itr.second->gossip_menu_id, itr.second.get()));

    do
    {
        Field* fields = result->Fetch();

        GossipMenuItems gMenuItem;

        gMenuItem.menu_id               = fields[0].GetUInt32();
        gMenuItem.id                    = fields[1].GetUInt32();
        gMenuItem.option_icon           = fields[2].GetUInt8();
        gMenuItem.option_text           = fields[3].GetCppString();
        gMenuItem.option_broadcast_text = fields[4].GetUInt32();

        gMenuItem.option_id             = fields[5].GetUInt32();
        gMenuItem.npc_option_npcflag    = fields[6].GetUInt32();
        gMenuItem.action_menu_id        = fields[7].GetInt32();
        gMenuItem.action_poi_id         = fields[8].GetUInt32();
        gMenuItem.action_script_id      = fields[9].GetUInt32();
        gMenuItem.box_coded             = fields[10].GetUInt8() != 0;
        //gMenuItem.box_money             = fields[11].GetUInt32();
        gMenuItem.box_text              = fields[12].GetCppString();
        gMenuItem.box_broadcast_text    = fields[13].GetUInt32();

        gMenuItem.condition_id          = fields[14].GetUInt32();

        if (gMenuItem.menu_id)                              // == 0 id is special and not have menu_id data
        {
            if (m_GossipMenusMap.find(gMenuItem.menu_id) == m_GossipMenusMap.end())
            {
                sLog.outErrorDb("Gossip menu option (MenuId: %u) for nonexistent menu", gMenuItem.menu_id);
                continue;
            }
        }

        if ((gMenuItem.option_id == GOSSIP_OPTION_GOSSIP) && (gMenuItem.action_menu_id > 0))
        {
            if (m_GossipMenusMap.find(gMenuItem.action_menu_id) == m_GossipMenusMap.end())
                sLog.outErrorDb("Gossip menu option (MenuId: %u Id: %u) have action_menu_id = %u for nonexistent menu", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_menu_id);
            else if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
                menu_ids.erase(gMenuItem.action_menu_id);
        }

        if (gMenuItem.option_icon >= GOSSIP_ICON_MAX)
        {
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has unknown icon id %u. Replacing with GOSSIP_ICON_CHAT", gMenuItem.menu_id, gMenuItem.id, gMenuItem.option_icon);
            gMenuItem.option_icon = GOSSIP_ICON_CHAT;
        }

        if (!IsValidGossipOptionIconForBuild(gMenuItem.option_icon))
            gMenuItem.option_icon = GOSSIP_ICON_CHAT;

        if (gMenuItem.option_broadcast_text)
        {
            if (!GetBroadcastTextLocale(gMenuItem.option_broadcast_text))
            {
                sLog.outErrorDb("Table `gossip_menu_option` for MenuId %u, OptionID %u has non-existing or incompatible option_broadcast_text %u, ignoring.", gMenuItem.menu_id, gMenuItem.id, gMenuItem.option_broadcast_text);
                gMenuItem.option_broadcast_text = 0;
            }
        }

        if (gMenuItem.box_broadcast_text)
        {
            if (!GetBroadcastTextLocale(gMenuItem.box_broadcast_text))
            {
                sLog.outErrorDb("Table `gossip_menu_option` for MenuId %u, OptionID %u has non-existing or incompatible BoxBroadcastTextId %u, ignoring.", gMenuItem.menu_id, gMenuItem.id, gMenuItem.box_broadcast_text);
                gMenuItem.box_broadcast_text = 0;
            }
        }

        if (gMenuItem.option_id == GOSSIP_OPTION_NONE)
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u use option id GOSSIP_OPTION_NONE. Option will never be used", gMenuItem.menu_id, gMenuItem.id);

        if (gMenuItem.option_id >= GOSSIP_OPTION_MAX)
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has unknown option id %u. Option will not be used", gMenuItem.menu_id, gMenuItem.id, gMenuItem.option_id);

        if (gMenuItem.menu_id && (gMenuItem.npc_option_npcflag || !sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK)))
        {
            bool found_menu_uses = false;
            bool found_flags_uses = false;

            std::pair<Menu2CInfoMap::const_iterator, Menu2CInfoMap::const_iterator> tm_bounds = menu2CInfoMap.equal_range(gMenuItem.menu_id);
            for (Menu2CInfoMap::const_iterator it2 = tm_bounds.first; !found_flags_uses && it2 != tm_bounds.second; ++it2)
            {
                CreatureInfo const* cInfo = it2->second;

                found_menu_uses = true;

                // some from creatures with gossip menu can use gossip option base at npc_flags
                if (gMenuItem.npc_option_npcflag & cInfo->npc_flags)
                    found_flags_uses = true;

                // unused check data preparing part
                if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
                    menu_ids.erase(gMenuItem.menu_id);
            }

            if (found_menu_uses && !found_flags_uses)
                sLog.outDetail("Table gossip_menu_option for menu %u, id %u has `npc_option_npcflag` = %u but creatures using this menu does not have corresponding `npc_flags`. Option will not accessible in game.", gMenuItem.menu_id, gMenuItem.id, gMenuItem.npc_option_npcflag);
        }

        if (gMenuItem.action_poi_id && !GetPointOfInterest(gMenuItem.action_poi_id))
        {
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u use non-existing action_poi_id %u, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_poi_id);
            gMenuItem.action_poi_id = 0;
        }

        if (gMenuItem.action_script_id)
        {
            if (sGossipScripts.find(gMenuItem.action_script_id) == sGossipScripts.end())
            {
                sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u have action_script_id %u that does not exist in `gossip_scripts`, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_script_id);
                continue;
            }

            gossipScriptSet.erase(gMenuItem.action_script_id);
        }

        if (gMenuItem.condition_id)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(gMenuItem.condition_id);
            if (!condition)
            {
                sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has condition_id %u that does not exist in `conditions`, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.condition_id);
                gMenuItem.condition_id = 0;
            }
        }

        m_GossipMenuItemsMap.insert(GossipMenuItemsMap::value_type(gMenuItem.menu_id, gMenuItem));
    }
    while (result->NextRow());

    if (!sLog.HasLogFilter(LOG_FILTER_DB_STRICTED_CHECK))
    {
        for (const auto menu_id : menu_ids)
            sLog.outErrorDb("Table `gossip_menu` contain unused (in creature or GO or menu options) menu id %u.", menu_id);
    }
}

void ObjectMgr::AddVendorItem(uint32 entry, uint32 item, uint32 maxcount, uint32 incrtime, uint32 itemflags)
{
    VendorItemData& vList = m_CacheVendorItemMap[entry];
    vList.AddItem(item, maxcount, incrtime, itemflags, 0);

    sWorld.ExecuteUpdate("INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `itemflags`) VALUES('%u','%u','%u','%u','%u')", entry, item, maxcount, incrtime, itemflags);
}

bool ObjectMgr::RemoveVendorItem(uint32 entry, uint32 item)
{
    CacheVendorItemMap::iterator  iter = m_CacheVendorItemMap.find(entry);
    if (iter == m_CacheVendorItemMap.end())
        return false;

    if (!iter->second.FindItem(item))
        return false;

    iter->second.RemoveItem(item);
    sWorld.ExecuteUpdate("DELETE FROM `npc_vendor` WHERE `entry`='%u' AND `item`='%u'", entry, item);
    return true;
}

bool ObjectMgr::IsVendorItemValid(bool isTemplate, char const* tableName, uint32 vendor_entry, uint32 item_id, uint32 maxcount, uint32 incrtime, uint32 conditionId, Player* pl) const
{
    char const* idStr = isTemplate ? "vendor template" : "vendor";
    CreatureInfo const* cInfo = nullptr;

    if (!isTemplate)
    {
        cInfo = GetCreatureTemplate(vendor_entry);
        if (!cInfo)
        {
            if (pl)
                ChatHandler(pl).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
            else if (!IsExistingCreatureId(vendor_entry))
                sLog.outErrorDb("Table `%s` has data for nonexistent creature (Entry: %u), ignoring", tableName, vendor_entry);
            return false;
        }
    }

    if (ItemPrototype const* pItemProto = GetItemPrototype(item_id))
        pItemProto->Discovered = true; // all vendor items count as discovered
    else
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_ITEM_NOT_FOUND, item_id);
        else if (!IsExistingItemId(item_id))
            sLog.outErrorDb("Table `%s` for %s %u contain nonexistent item (%u), ignoring",
                            tableName, idStr, vendor_entry, item_id);
        return false;
    }

    if (maxcount > 0 && incrtime == 0)
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage("MaxCount!=0 (%u) but IncrTime==0", maxcount);
        else
            sLog.outErrorDb("Table `%s` has `maxcount` (%u) for item %u of %s %u but `incrtime`=0, ignoring",
                            tableName, maxcount, item_id, idStr, vendor_entry);
        return false;
    }
    else if (maxcount == 0 && incrtime > 0)
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage("MaxCount==0 but IncrTime<>=0");
        else
            sLog.outErrorDb("Table `%s` has `maxcount`=0 for item %u of %s %u but `incrtime`<>0, ignoring",
                            tableName, item_id, idStr, vendor_entry);
        return false;
    }

    if (conditionId && !sConditionStorage.LookupEntry<ConditionEntry>(conditionId))
    {
        sLog.outErrorDb("Table `%s` has `condition_id`=%u for item %u of %s %u but this condition is not valid, ignoring", tableName, conditionId, item_id, idStr, vendor_entry);
        return false;
    }

    VendorItemData const* vItems = isTemplate ? GetNpcVendorTemplateItemList(vendor_entry) : GetNpcVendorItemList(vendor_entry);
    VendorItemData const* tItems = isTemplate ? nullptr : GetNpcVendorTemplateItemList(vendor_entry);

    if (!vItems && !tItems)
        return true;                                        // later checks for non-empty lists

    if (vItems && vItems->FindItem(item_id))
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_ITEM_ALREADY_IN_LIST, item_id);
        else
            sLog.outErrorDb("Table `%s` has duplicate items %u for %s %u, ignoring",
                            tableName, item_id, idStr, vendor_entry);
        sLog.outErrorDb("Table `npc_vendor` has duplicate items %u for vendor (Entry: %u), ignoring",
                        item_id, vendor_entry);
        return false;
    }

    if (!isTemplate)
    {
        if (tItems && tItems->GetItem(item_id))
        {
            if (pl)
                ChatHandler(pl).PSendSysMessage(LANG_ITEM_ALREADY_IN_LIST, item_id);
            else
            {
                if (!cInfo->vendor_id)
                    sLog.outErrorDb("Table `%s` has duplicate items %u for %s %u, ignoring",
                                    tableName, item_id, idStr, vendor_entry);
                else
                    sLog.outErrorDb("Table `%s` has duplicate items %u for %s %u (or possible in vendor template %u), ignoring",
                                    tableName, item_id, idStr, vendor_entry, cInfo->vendor_id);
            }
            return false;
        }
    }

    uint32 countItems = vItems ? vItems->GetItemCount() : 0;
    countItems += tItems ? tItems->GetItemCount() : 0;

    if (countItems >= UINT8_MAX)
    {
        if (pl)
            ChatHandler(pl).SendSysMessage(LANG_COMMAND_ADDVENDORITEMITEMS);
        else
            sLog.outErrorDb("Table `%s` has too many items (%u >= %i) for %s %u, ignoring",
                            tableName, countItems, UINT8_MAX, idStr, vendor_entry);
        return false;
    }

    return true;
}

void ObjectMgr::AddGroup(Group* group)
{
    m_GroupMap[group->GetId()] = group ;
}

void ObjectMgr::RemoveGroup(Group* group)
{
    m_GroupMap.erase(group->GetId());
}

bool FindCreatureData::operator()(CreatureDataPair const& dataPair)
{
    // skip wrong entry ids
    if (i_id && !dataPair.second.HasCreatureId(i_id))
        return false;

    if (!i_anyData)
        i_anyData = &dataPair;

    // without player we can't find more stricted cases, so use fouded
    if (!i_player)
        return true;

    // skip diff. map cases
    if (dataPair.second.position.mapId != i_player->GetMapId())
        return false;

    float new_dist = i_player->GetDistance2d(dataPair.second.position.x, dataPair.second.position.y);

    if (!i_mapData || new_dist < i_mapDist)
    {
        i_mapData = &dataPair;
        i_mapDist = new_dist;
    }

    // skip not spawned (in any state),
    uint16 pool_id = sPoolMgr.IsPartOfAPool<Creature>(dataPair.first);
    if (pool_id && !i_player->GetMap()->GetPersistentState()->IsSpawnedPoolObject<Creature>(dataPair.first))
        return false;

    if (!i_spawnedData || new_dist < i_spawnedDist)
    {
        i_spawnedData = &dataPair;
        i_spawnedDist = new_dist;
    }

    return false;
}

CreatureDataPair const* FindCreatureData::GetResult() const
{
    if (i_spawnedData)
        return i_spawnedData;

    if (i_mapData)
        return i_mapData;

    return i_anyData;
}

bool FindGOData::operator()(GameObjectDataPair const& dataPair)
{
    // skip wrong entry ids
    if (i_id && dataPair.second.id != i_id)
        return false;

    if (!i_anyData)
        i_anyData = &dataPair;

    // without player we can't find more stricted cases, so use fouded
    if (!i_player)
        return true;

    // skip diff. map cases
    if (dataPair.second.position.mapId != i_player->GetMapId())
        return false;

    float new_dist = i_player->GetDistance2d(dataPair.second.position.x, dataPair.second.position.y);

    if (!i_mapData || new_dist < i_mapDist)
    {
        i_mapData = &dataPair;
        i_mapDist = new_dist;
    }

    // skip not spawned (in any state)
    uint16 pool_id = sPoolMgr.IsPartOfAPool<GameObject>(dataPair.first);
    if (pool_id && !i_player->GetMap()->GetPersistentState()->IsSpawnedPoolObject<GameObject>(dataPair.first))
        return false;

    if (!i_spawnedData || new_dist < i_spawnedDist)
    {
        i_spawnedData = &dataPair;
        i_spawnedDist = new_dist;
    }

    return false;
}

GameObjectDataPair const* FindGOData::GetResult() const
{
    if (i_mapData)
        return i_mapData;

    if (i_spawnedData)
        return i_spawnedData;

    return i_anyData;
}

uint32 ObjectMgr::AddGOData(uint32 entry, uint32 mapId, float x, float y, float z, float o, uint32 spawntimedelay, float rotation0, float rotation1, float rotation2, float rotation3)
{
    GameObjectInfo const* goinfo = GetGameObjectInfo(entry);
    if (!goinfo)
        return 0;

    Map* map = const_cast<Map*>(sMapMgr.FindMap(mapId));
    if (!map)
        return 0;

    uint32 guid = map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT);
    GameObjectData& data = NewGOData(guid);
    data.id             = entry;
    data.position.mapId = mapId;
    data.position.x     = x;
    data.position.y     = y;
    data.position.z     = z;
    data.position.o     = o;
    data.rotation0      = rotation0;
    data.rotation1      = rotation1;
    data.rotation2      = rotation2;
    data.rotation3      = rotation3;
    data.spawntimesecsmin = spawntimedelay;
    data.spawntimesecsmax = spawntimedelay;
    data.animprogress   = 100;
    data.go_state       = GO_STATE_READY;
    data.spawn_flags    = 0;

    AddGameobjectToGrid(guid, &data);

    // Spawn if necessary (loaded grids only)
    // We use spawn coords to spawn
    if (!map->Instanceable() && map->IsLoaded(x, y))
    {
        GameObject *go = new GameObject;
        if (!go->LoadFromDB(guid, map))
        {
            sLog.outError("AddGOData: cannot add gameobject entry %u to map", entry);
            delete go;
            return 0;
        }
        map->Add(go);
    }

    DEBUG_LOG("AddGOData: dbguid %u entry %u map %u x %f y %f z %f o %f", guid, entry, mapId, x, y, z, o);

    return guid;
}

bool ObjectMgr::MoveCreData(uint32 guid, uint32 mapId, const Position& pos)
{
    CreatureData& data = NewOrExistCreatureData(guid);
    if (!data.creature_id[0])
        return false;

    RemoveCreatureFromGrid(guid, &data);
    if (data.position.x == pos.x && data.position.y == pos.y && data.position.z == pos.z)
        return true;
    data.position.x = pos.x;
    data.position.y = pos.y;
    data.position.z = pos.z;
    data.position.o = pos.o;
    AddCreatureToGrid(guid, &data);

    // Spawn if necessary (loaded grids only)
    if (Map* map = const_cast<Map*>(sMapMgr.FindMap(mapId)))
    {
        // We use spawn coords to spawn
        if (!map->Instanceable() && map->IsLoaded(data.position.x, data.position.y))
        {
            Creature *creature = new Creature;
            if (!creature->LoadFromDB(guid, map))
            {
                sLog.outError("AddCreature: cannot add creature entry %u to map", guid);
                delete creature;
                return false;
            }
            map->Add(creature);
        }
    }
    return true;
}

uint32 ObjectMgr::AddCreData(uint32 entry, uint32 /*team*/, uint32 mapId, float x, float y, float z, float o, uint32 spawntimedelay)
{
    CreatureInfo const *cInfo = GetCreatureTemplate(entry);
    if (!cInfo)
        return 0;
    Map* map = const_cast<Map*>(sMapMgr.FindMap(mapId));
    if (!map)
        return 0;

    uint32 guid = map->GenerateLocalLowGuid(HIGHGUID_UNIT);
    CreatureData& data = NewOrExistCreatureData(guid);
    data.creature_id[0] = entry;
    data.position.mapId = mapId;
    data.position.x = x;
    data.position.y = y;
    data.position.z = z;
    data.position.o = o;
    data.spawntimesecsmin = spawntimedelay;
    data.spawntimesecsmax = spawntimedelay;
    data.wander_distance = 0;
    data.health_percent = 100.0f;
    data.mana_percent = 100.0f;
    data.movement_type = cInfo->movement_type;

    AddCreatureToGrid(guid, &data);

    // Spawn if necessary (loaded grids only)
    // We use spawn coords to spawn
    if (!map->Instanceable() && !map->IsRemovalGrid(x, y))
    {
        Creature* creature = new Creature;
        if (!creature->LoadFromDB(guid, map))
        {
            sLog.outError("AddCreature: cannot add creature entry %u to map", entry);
            delete creature;
            return 0;
        }
        map->Add(creature);
    }

    return guid;
}


void ObjectMgr::RemoveGraveYardLink(uint32 id, uint32 zoneId, Team team, bool inDB)
{
    GraveYardMap::iterator graveLow  = m_GraveYardMap.lower_bound(zoneId);
    GraveYardMap::iterator graveUp   = m_GraveYardMap.upper_bound(zoneId);
    if (graveLow == graveUp)
    {
        //sLog.outErrorDb("Table `game_graveyard_zone` incomplete: Zone %u Team %u does not have a linked graveyard.",zoneId,team);
        return;
    }

    bool found = false;

    GraveYardMap::iterator itr;

    for (itr = graveLow; itr != graveUp; ++itr)
    {
        GraveYardData & data = itr->second;

        // skip not matching safezone id
        if (data.safeLocId != id)
            continue;

        // skip enemy faction graveyard at same map (normal area, city, or battleground)
        // team == 0 case can be at call from .neargrave
        if (data.team != 0 && team != 0 && data.team != team)
            continue;

        found = true;
        break;
    }

    // no match, return
    if (!found)
        return;

    // remove from links
    m_GraveYardMap.erase(itr);

    // remove link from DB
    if (inDB)
        WorldDatabase.PExecute("DELETE FROM `game_graveyard_zone` WHERE `id` = '%u' AND `ghost_zone` = '%u' AND `faction` = '%u'", id, zoneId, team);
}

void ObjectMgr::LoadFactionChangeReputations()
{
    factionchange_reputations.clear();
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `alliance_id`, `horde_id` FROM `player_factionchange_reputations`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!GetFactionEntry(alliance) || !GetFactionEntry(horde))
        {
            sLog.outErrorDb("Faction change reputation pair %u/%u does not exist.", alliance, horde);
        }
        else
            factionchange_reputations[alliance] = horde;
    }
    while (result->NextRow());
}


void ObjectMgr::LoadFactionChangeSpells()
{
    factionchange_spells.clear();
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `alliance_id`, `horde_id` FROM `player_factionchange_spells`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!sSpellMgr.GetSpellEntry(alliance) || !sSpellMgr.GetSpellEntry(horde))
        {
            sLog.outErrorDb("Faction change spell pair %u/%u does not exist.", alliance, horde);
        }
        else
            factionchange_spells[alliance] = horde;
    }
    while (result->NextRow());
}


void ObjectMgr::LoadFactionChangeItems()
{
    factionchange_items.clear();
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `alliance_id`, `horde_id` FROM `player_factionchange_items`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!GetItemPrototype(alliance) || !GetItemPrototype(horde))
        {
            if (!IsExistingItemId(alliance) || !IsExistingItemId(horde))
                sLog.outErrorDb("Faction change item pair %u/%u does not exist", alliance, horde);
        }
        else
            factionchange_items[alliance] = horde;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadFactionChangeQuests()
{
    factionchange_quests.clear();
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `alliance_id`, `horde_id` FROM `player_factionchange_quests`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 alliance = fields[0].GetUInt32();
        uint32 horde = fields[1].GetUInt32();

        if (!GetQuestTemplate(alliance) || !GetQuestTemplate(horde))
        {
            if (!IsExistingQuestId(alliance) || !IsExistingQuestId(horde))
                sLog.outErrorDb("Faction change quest pair %u/%u does not exist.", alliance, horde);
        }
        else
            factionchange_quests[alliance] = horde;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadFactionChangeMounts()
{
    factionchange_mounts.clear();
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `RaceId`, `MountNum`, `ItemEntry` FROM `player_factionchange_mounts`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint8 RaceId     = fields[0].GetUInt8();
        uint8 MountNum   = fields[1].GetUInt8();
        uint32 ItemEntry = fields[2].GetUInt32();

        if (!GetItemPrototype(ItemEntry))
        {
            if (!IsExistingItemId(ItemEntry))
                sLog.outErrorDb("Faction change mount replacement %u/%u/%u is invalid. The item does not exist.", RaceId, MountNum, ItemEntry);
        }
        else
        {
            FactionChangeMountData data;
            data.RaceId     = Races(RaceId);
            data.MountNum   = MountNum;
            data.ItemEntry  = ItemEntry;
            factionchange_mounts.push_back(data);
        }
    }
    while (result->NextRow());
}

void ObjectMgr::RestoreDeletedItems()
{
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `id`, `player_guid`, `item_entry`, `stack_count` FROM `character_deleted_items`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        uint32 memberGuidlow = fields[1].GetUInt32();
        uint32 itemEntry = fields[2].GetUInt32();
        uint32 stackCount = fields[3].GetUInt32();
        
        if (ItemPrototype const* itemProto = GetItemPrototype(itemEntry))
        {
            ObjectGuid memberGuid = ObjectGuid(HIGHGUID_PLAYER, memberGuidlow);
            Player* pPlayer = ObjectAccessor::FindPlayerNotInWorld(memberGuid);

            if (Item* restoredItem = Item::CreateItem(itemEntry, stackCount ? stackCount : 1, pPlayer ? pPlayer : (const Player *) 0))
            {
                // save new item before send
                restoredItem->SaveToDB();

                // subject
                std::string subject = itemProto->Name1;

                // text
                std::string textFormat = GetMangosString(LANG_RESTORED_ITEM, DB_LOCALE_enUS);
                
                MailDraft(subject, textFormat)
                    .AddItem(restoredItem)
                    .SendMailTo(MailReceiver(memberGuid), MailSender(MAIL_NORMAL, memberGuid.GetCounter(), MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED, 0, 30 * DAY);

                CharacterDatabase.PExecute("DELETE FROM `character_deleted_items` WHERE `id` = %u", id);
            }
        }
    } while (result->NextRow());
}

uint32 GetRealMountEntry(uint32 entry)
{
    switch (entry)
    {
        case 50101:
            return 1132;
        case 50102:
            return 8591;
        case 50103:
            return 15277;
        case 50104:
            return 13332;
        case 50105:
            return 5655;
        case 50106:
            return 5873;
        case 50107:
            return 8631;
        case 50108:
            return 8595;
    }
    return entry;
}

bool ObjectMgr::GetMountDataByEntry(uint32 itemEntry, Races& race, uint8& mountNum) const
{
    // Mount custom Nostalrius encore dans la DB.
    itemEntry = GetRealMountEntry(itemEntry);
    for (const auto& itr : factionchange_mounts)
    {
        if (itr.ItemEntry == itemEntry)
        {
            race = itr.RaceId;
            mountNum = itr.MountNum;
            return true;
        }
    }

    return false;
}

uint32 ObjectMgr::GetMountItemEntry(Races race, uint8 num) const
{
    for (const auto& itr : factionchange_mounts)
        if (itr.RaceId == race && itr.MountNum == num)
            return itr.ItemEntry;

    return false;
}

uint32 ObjectMgr::GetRandomMountForRace(Races race) const
{
    // 1- Compter le nombre total de montures de cette race
    uint32 count = 0;
    for (const auto& itr : factionchange_mounts)
        if (itr.RaceId == race)
            ++count;
    // 2- Quelques verifs / Generer index aleatoire
    if (!count)
        return 0;
    count -= urand(0, count - 1);
    // 3- Reboucler
    for (const auto& itr : factionchange_mounts)
    {
        if (itr.RaceId == race)
        {
            if (count == 0)
                return itr.ItemEntry;
            --count;
        }
    }
    // 4- Un soucis ... ?
    return 0;
}

Races ObjectMgr::GetOppositeRace(Races origRace) const
{
    switch (origRace)
    {
        case RACE_HUMAN:
            return RACE_ORC;
        case RACE_ORC:
            return RACE_HUMAN;
        case RACE_DWARF:
            return RACE_TROLL;
        case RACE_NIGHTELF:
            return RACE_UNDEAD;
        case RACE_UNDEAD:
            return RACE_NIGHTELF;
        case RACE_TAUREN:
            return RACE_TAUREN;
        case RACE_GNOME:
            return RACE_TAUREN;
        case RACE_TROLL:
            return RACE_DWARF;
        default:
            return RACE_GOBLIN;
    }
}

void ObjectMgr::LoadConditions()
{
    SQLWorldLoader loader;
    loader.Load(sConditionStorage);

    for (uint32 i = 0; i < sConditionStorage.GetMaxEntry(); ++i)
    {
        ConditionEntry* condition = const_cast<ConditionEntry*>(sConditionStorage.LookupEntry<ConditionEntry>(i));
        if (!condition)
            continue;

        if (!condition->IsValid())
        {
            sLog.outErrorDb("ObjectMgr::LoadConditions: invalid condition_entry %u, skip", i);
            sConditionStorage.EraseEntry(i);
            continue;
        }
    }

    for (auto& itr : m_QuestTemplatesMap) // needs to be checked after loading conditions
    {
        Quest* qinfo = itr.second.get();

        if (qinfo->RequiredCondition)
        {
            const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(qinfo->RequiredCondition);
            if (!condition) // condition does not exist for some reason
                sLog.outErrorDb("Quest %u has `RequiredCondition` = %u but it does not exist.", qinfo->GetQuestId(), qinfo->RequiredCondition);
        }
    }
}


// Check if a player meets condition conditionId
bool ObjectMgr::IsConditionSatisfied(uint32 conditionId, WorldObject const* target, Map const* map, WorldObject const* source, ConditionSource conditionSourceType) const
{
    if (const ConditionEntry* condition = sConditionStorage.LookupEntry<ConditionEntry>(conditionId))
    {
        bool result = condition->Meets(target, map, source, conditionSourceType);
        if (result)
        {
            result = !ScriptRegistry<ConditionScript>::ForEachWithReturn([&](ConditionScript* script)
            {
                return !script->OnConditionCheck(conditionId, const_cast<WorldObject*>(source), const_cast<WorldObject*>(target));
            });
        }

        return result;
    }

    return false;
}

uint32 ObjectMgr::GenerateAuctionID()
{
    std::set<uint32>::const_iterator it = m_AuctionsIds.lower_bound(m_NextAuctionId);
    while (it != m_AuctionsIds.end() && *it == m_NextAuctionId)
    {
        ++m_NextAuctionId;
        ++it;
    }
    m_AuctionsIds.insert(m_NextAuctionId);
    return m_NextAuctionId++;
}

void ObjectMgr::FreeAuctionID(uint32 id)
{
    m_AuctionsIds.erase(id);
}

void ObjectMgr::GeneratePetNumberRange(uint32& first, uint32& last)
{
    first = GeneratePetNumber();
    uint32 prev = first;
    for (int i = 0; i < 1000; ++i)
    {
        uint32 nextGuid = GeneratePetNumber();
        // End of this range
        if (nextGuid != (prev + 1))
        {
            last = prev + 1;
            return;
        }

        prev = nextGuid;
    }

    last = first + 1000;
}

void ObjectMgr::LoadMailTemplate()
{
    sMailTemplateStorage.Load();
}

char const* ObjectMgr::GetMailTextTemplate(uint32 id, LocaleConstant locale_idx)
{
    if (MailTemplateEntry const* pTemplate = sMailTemplateStorage.LookupEntry<MailTemplateEntry>(id))
        return pTemplate->subject[locale_idx];

    return "Missing mail text template!";
}

void ObjectMgr::LoadAreaTemplate()
{
    sAreaStorage.Load();

    for (auto itr = sAreaStorage.begin<AreaEntry>(); itr != sAreaStorage.end<AreaEntry>() ; ++itr)
        if (itr->IsZone() && itr->MapId != 0 && itr->MapId != 1)
            sAreaFlagByMapId.insert(AreaFlagByMapId::value_type(itr->MapId, itr->ExploreFlag));
}

void ObjectMgr::LoadAreaLocales()
{
    if (!sWorld.getConfig(CONFIG_BOOL_LOAD_LOCALES))
        return;

    m_AreaLocaleMap.clear();

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `Entry`, `NameLoc1`, `NameLoc2`, `NameLoc3`, `NameLoc4`, `NameLoc5`, `NameLoc6`, `NameLoc7`, `NameLoc8` FROM `locales_area`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        if (!AreaEntry::GetById(entry))
        {
            ERROR_DB_STRICT_LOG("Table `locales_area` has data for nonexistent area entry %u, skipped.", entry);
            continue;
        }

        AreaLocale& data = m_AreaLocaleMap[entry];

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            auto str = fields[i].GetCppString();
            if (!str.empty())
            {
                int8 idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if ((int32)data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }
        }
    }
    while (result->NextRow());
}

void ObjectMgr::LoadCartographerAreas()
{
    memset(m_cartographerExploreMask, 0, sizeof(m_cartographerExploreMask));

    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `area_id` FROM `cartographer`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        AreaEntry const* pAreaEntry = AreaEntry::GetById(entry);

        if (!pAreaEntry)
        {
            ERROR_DB_STRICT_LOG("Table `cartographer` has data for nonexistent area entry %u, skipped.", entry);
            continue;
        }

        if (!pAreaEntry->ExploreFlag || pAreaEntry->ExploreFlag == 0xffff)
        {
            ERROR_DB_STRICT_LOG("Table `cartographer` has area %u with no explore flag, skipped.", entry);
            continue;
        }

        int offset = pAreaEntry->ExploreFlag / 32;
        if (offset >= PLAYER_EXPLORED_ZONES_SIZE)
        {
            ERROR_DB_STRICT_LOG("Table `cartographer` has area %u with invalid explore flag %u, skipped.", entry, pAreaEntry->ExploreFlag);
            continue;
        }

        uint32 val = (uint32)(1 << (pAreaEntry->ExploreFlag % 32));
        m_cartographerExploreMask[offset] |= val;

    } while (result->NextRow());
}

void ObjectMgr::GetAreaLocaleString(uint32 entry, int32 loc_idx, std::string* namePtr) const
{
    if (loc_idx >= 0)
    {
        if (const auto *al = GetAreaLocale(entry))
            if (namePtr && al->Name.size() > size_t(loc_idx) && !al->Name[loc_idx].empty())
                *namePtr = al->Name[loc_idx];
    }
}

AreaEntry const* ObjectMgr::GetAreaEntryByName(std::string const& name) const
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return 0;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    AreaEntry const* pAlt = nullptr;
    // Alternative first area Id that contains wnameLow as substring in case no exact name found
    for (auto itr = sAreaStorage.begin<AreaEntry>(); itr != sAreaStorage.end<AreaEntry>(); ++itr)
    {
        std::wstring areaName;
        if (!Utf8toWStr(itr->Name, areaName))
            continue;
        wstrToLower(areaName);

        if (areaName == wname)
            return *itr;
        else if (!pAlt && areaName.find(wname) != std::wstring::npos)
            pAlt = *itr;
    }

    return pAlt;
}

AreaEntry const* ObjectMgr::GetAreaEntryByExploreFlag(uint32 flag) const
{
    for (auto itr = sAreaStorage.begin<AreaEntry>(); itr < sAreaStorage.end<AreaEntry>(); ++itr)
    {
        if (itr->ExploreFlag == flag)
            return *itr;
    }
    return nullptr;
}

void ObjectMgr::LoadShop()
{
    auto CheckRegionRequirements = [&](ShopRegion region)
    {
        //For now just expect EU if SEA config is 0..
        ShopRegion currentRegion = sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK) ? ShopRegion::China : ShopRegion::Europe;

        return currentRegion == region || region == ShopRegion::Global;
    };


    m_ShopCategoriesMap.clear();
    m_shopLogs.clear();

    QueryResult* result = WorldDatabase.Query("SELECT ID, Name, Name_loc4, icon FROM shop_categories");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint8 id = fields[0].GetUInt8();
        std::string name = fields[1].GetString();
        std::string name_loc4 = fields[2].GetString();
        std::string icon = fields[3].GetString();

        ShopCategory shopcategory;
        shopcategory.Name = name;
        shopcategory.Name_loc4 = name_loc4;
        shopcategory.Icon = icon;

        m_ShopCategoriesMap[id] = shopcategory;

    } while (result->NextRow());

    delete result;

    m_ShopEntriesMap.clear();

    result = WorldDatabase.Query("SELECT ID, category, item, model_id, item_id, description, description_loc4, price, region_locked, position_x, position_y, position_z, rotation, scale FROM shop_items");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        uint8 category = fields[1].GetUInt8();
        uint32 item = fields[2].GetUInt32();
        uint32 ModelID = fields[3].GetUInt32();
        uint32 ItemID = fields[4].GetUInt32();
        std::string text = fields[5].GetString();
        std::string description_loc4 = fields[6].GetString();
        uint32 price = fields[7].GetUInt32();
        ShopRegion region = (ShopRegion)fields[8].GetUInt8();
        float PosX = fields[9].GetFloat();
        float PosY = fields[10].GetFloat();
        float PosZ = fields[11].GetFloat();
        float Rotation = fields[12].GetFloat();
        float Scale = fields[13].GetFloat();

        if (!CheckRegionRequirements(region))
            continue;

        auto raceChangeItems = make_array(50603, 50604, 50605, 50606, 50607, 50608, 50609, 50610, 50613, 50612);

        if (std::find(raceChangeItems.begin(), raceChangeItems.end(), item) != raceChangeItems.end() && sWorld.IsPvPRealm())
            continue;

        ShopEntry shopentry;
        shopentry.shopId = id;
        shopentry.Category = category;
        shopentry.Item = item;
        shopentry.ModelID = ModelID;
        shopentry.ItemDisplayID = ItemID;
        shopentry.Description = text;
        shopentry.Description_loc4 = description_loc4;
        shopentry.Price = price;
        shopentry.Position.x = PosX;
        shopentry.Position.y = PosY;
        shopentry.Position.z = PosZ;
        shopentry.Rotation = Rotation;
        shopentry.Scale = Scale;

        if (!price)
        {
            sLog.outErrorDb("ERROR for item Id %u, price is 0, skipping.", id);
            continue;
        }

        if (m_ShopEntriesMap.find(item) != m_ShopEntriesMap.end())
        {
            sLog.outErrorDb("ERROR for item Id %u, already has an entry in the shop for entry %u.", id, item);
            continue;
        }

        ShopCategoriesMap::iterator CategoryIter = m_ShopCategoriesMap.find(shopentry.Category);
        if (CategoryIter != m_ShopCategoriesMap.end())
        {
            CategoryIter->second.Items.push_back(shopentry);
        }
        else
        {
			sLog.outErrorDb("ERROR for item Id %u, category %u doesn't defined in category list!", id, shopentry.Category);
			continue;
        }

        m_ShopEntriesMap[item] = shopentry;

    } while (result->NextRow());

    delete result;

    // Sort items in categories
    for (auto& CategoryPair : m_ShopCategoriesMap)
    {
        ShopCategory& ShopCat = CategoryPair.second;
		std::sort(ShopCat.Items.begin(), ShopCat.Items.end(), [](ShopEntry const& t1, ShopEntry const& t2)
			{
				return t1.shopId < t2.shopId;
			});

        ShopCat.CachedItemEntries.resize(ShopCat.Items.size());
        for (uint32 i = 0; i < ShopCat.Items.size(); i++)
        {
            std::string& CachedEntry = ShopCat.CachedItemEntries[i];
            ShopEntry& Entry = ShopCat.Items[i];

            ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(Entry.Item);

            if (pProto == nullptr)
            {
                sLog.outErrorDb("ERROR for item Id %u, item_template missed info!", Entry.Item);
                continue;
            }

			std::string ItemName;


			if (sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK))
			{
				//ItemName = Entry.Description_loc4;
                ItemName = sObjectMgr.GetItemLocaleName(Entry.Item, LOCALE_zhCN);
			}
			else
			{
				//ItemName = Entry.Description;
                ItemName = pProto->Name1;
			}

            CachedEntry.resize(1024);
            int32 FormatResult = std::snprintf(CachedEntry.data(), 1024, "Entries:%u=%u=%s=%u=%s=%u=%u=%u=%.02f=%.02f=%.02f=%.02f=%u=%s=%u",
                Entry.Category,
                0, // TODO: subcategory
                ItemName.c_str(),
                Entry.Price,
                pProto->Description.c_str(),
                Entry.Item,
                Entry.ModelID,
                Entry.ItemDisplayID,
                Entry.Position.x,
                Entry.Position.y,
                Entry.Position.z,
                Entry.Rotation,
                0, // TODO: holiday
                "", // TODO: colors
                0); // TODO: gender

            MANGOS_ASSERT(FormatResult > 0);
            if (FormatResult > 1022)
            {
                sLog.outErrorDb("ERROR for item Id %u, entry size overflow buffer (size of buffer 1024)!", Entry.Item);
            }

            size_t StringRealLen = std::strlen(CachedEntry.c_str());
            CachedEntry.resize(StringRealLen + 1);
        }
    }


    result = LoginDatabase.PQuery("SELECT `id`, `time`, `account`, `guid`, `item`, `price`, `refunded`, UNIX_TIMESTAMP(time) FROM `shop_logs` WHERE `realm_id` = %u OR `realm_id` = 0 ORDER BY `account`, `time` ASC",
        realmID);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 id = fields[0].GetUInt32();
            std::string date = fields[1].GetString();
            uint32 accountId = fields[2].GetUInt32();
            uint32 charGuid = fields[3].GetUInt32();
            uint32 itemEntry = fields[4].GetUInt32();
            uint32 itemPrice = fields[5].GetUInt32();
            bool refunded = fields[6].GetBool();
            uint32 dateUnix = fields[7].GetUInt32();


            //ordered by time ASC so last elem in vec is latest log for easier shop log output
            auto item = new ShopLogEntry{ id, date, accountId, charGuid, itemEntry, itemPrice, refunded, dateUnix };
            m_shopLogs[accountId].push_back(item);
            m_shopLogsLookup[id] = item;

        } while (result->NextRow());
        delete result;
    }

    constexpr uint32 ShopRealmStart = 1000000;
    constexpr uint32 ShopRealmStep = 1000000;

    uint32 shopIdsStart = ShopRealmStart + (realmID * ShopRealmStep);
    uint32 shopIdsEnd = shopIdsStart + ShopRealmStep - 1;


    result = LoginDatabase.PQuery("SELECT MAX(id) FROM shop_logs WHERE id >= %u AND id <= %u", shopIdsStart, shopIdsEnd);

    if (!result)
        m_maxShopEntry = shopIdsStart + 1;
    else
    {
        m_maxShopEntry = result->Fetch()[0].GetUInt32() + 1;
        if (m_maxShopEntry == 0 || m_maxShopEntry == 1)
            m_maxShopEntry = shopIdsStart + 1;
        delete result;
    }

}

void ObjectMgr::LoadItemTransmogrifyTemplates()
{
    m_itemTransmogs.clear();

    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `ID`, `ItemID`, `DisplayID`, `SourceID` FROM `item_transmogs` ORDER BY `ID` DESC"));

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 ID = fields[0].GetUInt32();
        uint32 ItemID = fields[1].GetUInt32();
        uint32 DisplayID = fields[2].GetUInt32();
        uint32 SourceID = fields[3].GetUInt32();

        ItemPrototype* copy = new ItemPrototype;
        ItemPrototype const* base = GetItemPrototype(ItemID);

        if (!m_transmogId)
            m_transmogId = ID;

        if (!base)
            continue;

        *copy = *base;

        copy->ItemId = ID;
        copy->DisplayInfoID = DisplayID;
        copy->DestItemId = ItemID;
        copy->SourceItemId = SourceID;
        copy->Bonding = BIND_WHEN_PICKED_UP;

        m_itemTransmogs[ID] = copy;
    } while (result->NextRow());
}

uint32 ObjectMgr::CreateItemTransmogrifyTemplate(uint32 destItemId, uint32 sourceDisplayId, uint32 sourceItemId)
{
    uint32 destId = ++m_transmogId;

    ItemPrototype* copy = new ItemPrototype;
    ItemPrototype const* base = GetItemPrototype(destItemId);
    ItemPrototype const* source = GetItemPrototype(sourceItemId);

    if (!base)
        return false;

    *copy = *base;

    copy->ItemId = destId;
    copy->DisplayInfoID = sourceDisplayId;
    copy->DestItemId = destItemId;
    copy->SourceItemId = sourceItemId;
    copy->Bonding = BIND_WHEN_PICKED_UP;

    //copy->Sheath = source->Sheath; // 3 hip, 1 back
    
    m_itemTransmogs[destId] = copy;

    //Whoever wrote this should be burned at the stake.
    //sWorld.SendUpdateSingleItem(destId);
    CharacterDatabase.PExecuteLog("INSERT INTO `item_transmogs` (`ID`, `ItemID`, `DisplayID`, `SourceID`) VALUES ('%u','%u','%u', '%u')", 
        destId, destItemId, sourceDisplayId, sourceItemId);

    return destId;
}

void ObjectMgr::DeleteItemTransmogrifyTemplate(uint32 transmogrifyId)
{
    auto itr = m_itemTransmogs.find(transmogrifyId);
    if (itr == m_itemTransmogs.end())
        return;

    m_itemTransmogs.erase(transmogrifyId);
    sWorld.SendSingleItemInvalidate(transmogrifyId);
    CharacterDatabase.PExecuteLog("DELETE FROM `item_transmogs` WHERE `ID`=%u", transmogrifyId);
}

bool ObjectMgr::IsItemTypeTransmoggable(uint32 invType)
{
    std::vector<uint32> TransmoggableItemTypes = {
    INVTYPE_HEAD,
    INVTYPE_SHOULDERS,
    INVTYPE_BODY,
    INVTYPE_CHEST,
    INVTYPE_WAIST,
    INVTYPE_LEGS,
    INVTYPE_FEET,
    INVTYPE_WRISTS,
    INVTYPE_HANDS,
    INVTYPE_WEAPON,
    INVTYPE_SHIELD,
    INVTYPE_RANGED,
    INVTYPE_CLOAK,
    INVTYPE_2HWEAPON,
    INVTYPE_ROBE,
    INVTYPE_WEAPONMAINHAND,
    INVTYPE_WEAPONOFFHAND,
    INVTYPE_HOLDABLE,
    INVTYPE_THROWN,
    INVTYPE_RANGEDRIGHT,
    INVTYPE_TABARD,
    };

    return std::find(TransmoggableItemTypes.begin(), TransmoggableItemTypes.end(), invType) != TransmoggableItemTypes.end();
}

bool ObjectMgr::IsItemSubClassTransmoggable(uint32 subClass)
{
    std::vector<uint32> TransmoggableWeaponSubClasses = {
    ITEM_SUBCLASS_WEAPON_AXE,
    ITEM_SUBCLASS_WEAPON_AXE2,
    ITEM_SUBCLASS_WEAPON_BOW,
    ITEM_SUBCLASS_WEAPON_GUN,
    ITEM_SUBCLASS_WEAPON_MACE,
    ITEM_SUBCLASS_WEAPON_MACE2,
    ITEM_SUBCLASS_WEAPON_POLEARM,
    ITEM_SUBCLASS_WEAPON_SWORD,
    ITEM_SUBCLASS_WEAPON_SWORD2,
    ITEM_SUBCLASS_WEAPON_STAFF,
    ITEM_SUBCLASS_WEAPON_FIST,
    ITEM_SUBCLASS_WEAPON_MISC, // flowers etc
    ITEM_SUBCLASS_WEAPON_DAGGER,
    //ITEM_SUBCLASS_WEAPON_THROWN = 16,
    //ITEM_SUBCLASS_WEAPON_SPEAR,
    ITEM_SUBCLASS_WEAPON_CROSSBOW,
    ITEM_SUBCLASS_WEAPON_WAND,
    //ITEM_SUBCLASS_WEAPON_FISHING_POLE = 20
    };

    return std::find(TransmoggableWeaponSubClasses.begin(), TransmoggableWeaponSubClasses.end(), subClass) != TransmoggableWeaponSubClasses.end();

}

void ObjectMgr::AddPossibleTransmog(uint8 pClass, uint32 itemClass, uint32 itemSubClass, uint32 invType, uint32 displayId)
{
    if (std::find(NumPossibleTransmogs[pClass][itemClass][itemSubClass][invType].begin(),
        NumPossibleTransmogs[pClass][itemClass][itemSubClass][invType].end(), displayId) == NumPossibleTransmogs[pClass][itemClass][itemSubClass][invType].end())
        NumPossibleTransmogs[pClass][itemClass][itemSubClass][invType].push_back(displayId);
    return;
}

void ObjectMgr::FillPossibleTransmogs()
{
    for (uint32 i = 1; i < MAX_CLASSES; ++i)
        for (uint32 j = 1; j < MAX_ITEM_CLASS; ++j)
            for (uint32 k = 0; k < MAX_ITEM_SUBCLASS_WEAPON; ++k)
                for (uint32 l = 1; l < MAX_INVTYPE; ++l)
                    NumPossibleTransmogs[i][j][k][l].clear();

    for (auto const& itr : GetItemPrototypeMap())
    {
        ItemPrototype const* proto = &itr.second;
        if (!proto)
            continue;
        if (strstr(proto->Name1.c_str(), "Monster - "))
            continue;
        if (strstr(proto->Name1.c_str(), "OLDMonster - "))
            continue;
        if (!IsItemTypeTransmoggable(proto->InventoryType))
            continue;
        if (!IsItemSubClassTransmoggable(proto->SubClass))
            continue;

        // add chest / robe exception

        for (uint32 pClass = 1; pClass < MAX_CLASSES; ++pClass)
            AddPossibleTransmog(pClass, proto->Class, proto->SubClass, proto->InventoryType, proto->DisplayInfoID);
    }
    return;
}

uint32 ObjectMgr::GetPossibleTransmogs(uint8 pClass, uint32 itemClass, uint32 itemSubClass, uint32 invType, bool stupid)
{
    if (stupid)
        return NumPossibleTransmogs[pClass][itemClass][itemSubClass][invType].size();

    uint32 numItems = 0;

    if (itemClass == ITEM_CLASS_ARMOR && (invType == INVTYPE_BODY || invType == INVTYPE_TABARD))
        return NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_MISC][invType].size();

    if (itemClass == ITEM_CLASS_ARMOR && itemSubClass != ITEM_SUBCLASS_ARMOR_SHIELD)
    {
        if (pClass == CLASS_PRIEST || pClass == CLASS_WARLOCK || pClass == CLASS_MAGE)
        {
            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][invType].size();
        }

        if (pClass == CLASS_DRUID || pClass == CLASS_ROGUE)
        {
            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][invType].size();

            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][invType].size();

        }

        if (pClass == CLASS_HUNTER || pClass == CLASS_SHAMAN)
        {
            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][invType].size();

            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][invType].size();

            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_MAIL][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_MAIL][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_MAIL][invType].size();

        }
            
        if (pClass == CLASS_WARRIOR || pClass == CLASS_PALADIN)
        {
            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_CLOTH][invType].size();

            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_LEATHER][invType].size();

            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_MAIL][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_MAIL][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_MAIL][invType].size();

            // no plate robes so far, but who knows what we'll add in the future
            if (invType == INVTYPE_ROBE || invType == INVTYPE_CHEST) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_PLATE][INVTYPE_ROBE].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_PLATE][INVTYPE_CHEST].size();
            }
            else
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_ARMOR_PLATE][invType].size();

        }
    }

    if (itemClass == ITEM_CLASS_ARMOR && itemSubClass == ITEM_SUBCLASS_ARMOR_SHIELD)
    {
        if (pClass == CLASS_SHAMAN || pClass == CLASS_PALADIN || pClass == CLASS_WARRIOR)
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][invType].size();
    }

    if (itemClass == ITEM_CLASS_WEAPON)
    {
        // 1h axe/mace/sword
        if (itemSubClass == ITEM_SUBCLASS_WEAPON_AXE || itemSubClass == ITEM_SUBCLASS_WEAPON_MACE || itemSubClass == ITEM_SUBCLASS_WEAPON_SWORD)
        {
            if (invType == INVTYPE_WEAPON)
            {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPONOFFHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPONOFFHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPONOFFHAND].size();
            }
            if (invType == INVTYPE_WEAPONMAINHAND)
            {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPONOFFHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPONOFFHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPONOFFHAND].size();
            }
            if (invType == INVTYPE_WEAPONOFFHAND) {
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE][INVTYPE_WEAPONOFFHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE][INVTYPE_WEAPONOFFHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPON].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPONMAINHAND].size();
                numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD][INVTYPE_WEAPONOFFHAND].size();
            }
        }

        // 2h axe/mace/sword
        if (invType == INVTYPE_2HWEAPON && (itemSubClass == ITEM_SUBCLASS_WEAPON_AXE2 || itemSubClass == ITEM_SUBCLASS_WEAPON_MACE2 || itemSubClass == ITEM_SUBCLASS_WEAPON_SWORD2))
        {
            numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_AXE2][INVTYPE_2HWEAPON].size();
            numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_MACE2][INVTYPE_2HWEAPON].size();
            numItems += NumPossibleTransmogs[pClass][itemClass][ITEM_SUBCLASS_WEAPON_SWORD2][INVTYPE_2HWEAPON].size();
        }

        // fists
        if (itemSubClass == ITEM_SUBCLASS_WEAPON_FIST && (invType == INVTYPE_WEAPON || invType == INVTYPE_WEAPONMAINHAND))
        {
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][INVTYPE_WEAPON].size();
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][INVTYPE_WEAPONMAINHAND].size();
        }
        if (itemSubClass == ITEM_SUBCLASS_WEAPON_FIST && (invType == INVTYPE_WEAPONOFFHAND))
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][INVTYPE_WEAPONOFFHAND].size();
            
        // daggers
        if (itemSubClass == ITEM_SUBCLASS_WEAPON_DAGGER && (invType == INVTYPE_WEAPON || invType == INVTYPE_WEAPONMAINHAND || invType == INVTYPE_WEAPONOFFHAND))
        {
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][INVTYPE_WEAPON].size();
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][INVTYPE_WEAPONMAINHAND].size();
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][INVTYPE_WEAPONOFFHAND].size();
        }

        // staff polearm wand bow gun crossbow misc
        if (itemSubClass == ITEM_SUBCLASS_WEAPON_STAFF || itemSubClass == ITEM_SUBCLASS_WEAPON_POLEARM || 
            itemSubClass == ITEM_SUBCLASS_WEAPON_WAND ||
            itemSubClass == ITEM_SUBCLASS_WEAPON_BOW || itemSubClass == ITEM_SUBCLASS_WEAPON_GUN ||
            itemSubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW || itemSubClass == ITEM_SUBCLASS_WEAPON_MISC)
            numItems += NumPossibleTransmogs[pClass][itemClass][itemSubClass][invType].size();
    }

    return numItems;

}

void ObjectMgr::LoadShellCoinCount()
{
    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT `count` FROM `item_instance` WHERE `itemEntry`=%u", ITEM_SHELL_COIN));

    if (!result)
        return;

    int32 totalCount = 0;
    do
    {
        Field* fields = result->Fetch();

        int32 count = fields[0].GetInt32();
        totalCount += count;

    } while (result->NextRow());

    m_shellCoinCount = std::min(totalCount, SHELL_COIN_MAX_COUNT);
}

void ObjectMgr::BackupCharacterInventory()
{
    sLog.outInfo("Making copy of character_inventory table.");
    CharacterDatabase.DirectPExecute("TRUNCATE `character_inventory_copy`");
    CharacterDatabase.DirectPExecute("ALTER TABLE `character_inventory_copy` DISABLE KEYS");
    CharacterDatabase.DirectPExecute("INSERT INTO `character_inventory_copy` SELECT * FROM `character_inventory`");
    CharacterDatabase.DirectPExecute("ALTER TABLE `character_inventory_copy` ENABLE KEYS");
}

void ObjectMgr::LoadChatChannels()
{
    m_chatChannelsMap.clear();                                  // for reload case
    //                                                               0     1        2                3       4            5            6            7            8            9            10           11            12          13               14               15               16               17               18               19               20
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `id`, `flags`, `faction_group`, `name`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `name_loc7`, `name_flags`, `shortcut`, `shortcut_loc1`, `shortcut_loc2`, `shortcut_loc3`, `shortcut_loc4`, `shortcut_loc5`, `shortcut_loc6`, `shortcut_loc7`, `shortcut_flags` FROM `chat_channels`"));

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        ChatChannelsEntry channel;
        channel.id = id;
        channel.flags = fields[1].GetUInt32();
        channel.factionGroup = fields[2].GetUInt32();

        for (int i = 0; i < MAX_DBC_LOCALE; ++i)
            channel.name[i] = fields[3 + i].GetCppString();
        channel.nameFlags = fields[11].GetUInt32();

        for (int i = 0; i < MAX_DBC_LOCALE; ++i)
            channel.shortcut[i] = fields[12 + i].GetCppString();
        channel.shortcutFlags = fields[20].GetUInt32();

        m_chatChannelsMap.insert({ id, channel });
    } while (result->NextRow());
}

ChatChannelsEntry const* ObjectMgr::GetChannelEntryFor(uint32 channelId)
{
    auto itr = m_chatChannelsMap.find(channelId);
    if (itr != m_chatChannelsMap.end())
        return &itr->second;

    return nullptr;
}

ChatChannelsEntry const* ObjectMgr::GetChannelEntryFor(std::string const& name)
{
    for (auto const& itr : m_chatChannelsMap)
    {
        for (std::string const& entryName : itr.second.name)
        {
            // Not loaded locale
            if (entryName.empty())
                continue;

            std::size_t const zoneMarker = entryName.find("%s");
            if (zoneMarker == std::string::npos)
            {
                if (entryName == name)
                    return &itr.second;
                continue;
            }

            std::string const prefix = entryName.substr(0, zoneMarker);
            std::string const suffix = entryName.substr(zoneMarker + 2);
            if (name.size() < prefix.size() + suffix.size())
                continue;

            if (name.compare(0, prefix.size(), prefix) == 0 &&
                name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
                return &itr.second;
        }

        // search in shortcut name
        for (std::string const& shortcut : itr.second.shortcut)
        {
            // Not loaded locale
            if (shortcut.empty())
                continue;

            if (shortcut == name)
                return &itr.second;
        }
    }

    return nullptr;
}
