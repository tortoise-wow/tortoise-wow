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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "SharedDefines.h"
#include "WorldSession.h"
#include "SessionTransport.h"
#include "PlayerLoginQueryHolder.h"

#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Handlers/CharacterCreation.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "UpdateMask.h"
#include "Auth/md5.h"
#include "AccountMgr.h"
#include "ObjectAccessor.h"
#include "Group.h"
#include "Database/DatabaseImpl.h"
#include "SocialMgr.h"
#include "Util.h"
#include "Language.h"
#include "Chat.h"
#include "ScriptObjects.h"
#include "Anticheat.h"
#include "MasterPlayer.h"
#include "PlayerBroadcaster.h"
#include "Mail.h"
#include "miscellaneous/feature_transmog.h"
#include "Config.hpp"
#include "Logging/DatabaseLogger.hpp"
#include <atomic>

namespace
{
uint64 NextLoginRequestToken()
{
    static std::atomic<uint64> nextToken{0};
    uint64 token = nextToken.fetch_add(1, std::memory_order_relaxed) + 1;
    if (!token)
        token = nextToken.fetch_add(1, std::memory_order_relaxed) + 1;
    return token;
}
}



// don't call WorldSession directly
// it may get deleted before the query callbacks get executed
// instead pass an account id to this handler
class CharacterHandler
{
public:
    void HandleCharEnumCallback(QueryResult * result, uint32 account)
    {
        WorldSession * session = sWorld.FindSession(account);
        if (!session)
        {
            delete result;
            return;
        }
        session->HandleCharEnum(result);
    }
    void HandlePlayerLoginCallback(QueryResult * /*dummy*/, SqlQueryHolder * holder)
    {
        if (!holder)
            return;

        LoginQueryHolder* loginHolder = static_cast<LoginQueryHolder*>(holder);
        if (loginHolder->GetTransport() == SessionTransport::Headless)
        {
            sWorld.HandleHeadlessLoginCallback(loginHolder);
            return;
        }

        WorldSession* session = sWorld.FindSession(loginHolder->GetAccountId());
        if (!session || !session->IsLoginRequest(loginHolder->GetGuid(),
            loginHolder->GetTransport(), loginHolder->GetRequestToken()))
        {
            delete loginHolder;
            return;
        }

        session->HandlePlayerLogin(loginHolder);
    }
} chrHandler;

bool WorldSession::HasHighLevelCharacter() const
{
    return _characterMaxLevel >= sWorld.getConfig(CONFIG_UINT32_HIGH_LEVEL_CHARACTER);
}

void WorldSession::HandleCharEnum(QueryResult * result)
{
    WorldPacket data(SMSG_CHAR_ENUM, 100);                  // we guess size

    uint8 num = 0;
    _characterMaxLevel = 0;

    data << num;

    if (result)
    {
        do
        {
            uint32 guidlow = (*result)[0].GetUInt32();
            uint32 level   = (*result)[7].GetUInt32();
            uint8 active = (*result)[22].GetUInt8();
            if (_characterMaxLevel < level)
                _characterMaxLevel = level;

            if (m_shouldBackupCharacters && level > sWorld.getConfig(CONFIG_UINT32_AUTO_PDUMP_MIN_CHAR_LEVEL) && !sWorld.IsCharacterPDumpedRecently(guidlow, sWorld.GetGameTime()))
                sWorld.SchedulePlayerDump(guidlow);

            if (!active)
                continue;

            DETAIL_LOG("Build enum data for char guid %u from account %u.", guidlow, GetAccountId());
            if (Player::BuildEnumData(result, &data))
                ++num;
        }
        while (result->NextRow());
        delete result;
    }

    m_shouldBackupCharacters = false;

    data.put<uint8>(0, num);
    _charactersCount = num;

    SendPacket(&data);
}

void WorldSession::HandleCharEnumOpcode(WorldPacket & /*recv_data*/)
{
    /// get all the data necessary for loading all characters (along with their pets) on the account
    CharacterDatabase.AsyncPQuery(&chrHandler, &CharacterHandler::HandleCharEnumCallback, GetAccountId(),
                                  //           0               1                2                3                 4                  5                       6                        7
                                  "SELECT characters.guid, characters.name, characters.race, characters.class, characters.gender, characters.playerBytes, characters.playerBytes2, characters.level, "
                                  //   8                9               10                     11                     12                     13                    14
                                  "characters.zone, characters.map, characters.position_x, characters.position_y, characters.position_z, guild_member.guildid, characters.playerFlags, "
                                  //  15                    16                   17                     18                   19                                20
                                  "characters.at_login,     0,                   0,                     0, characters.equipmentCache, characters.mortality_status, characters.total_deaths, characters.active "
                                  "FROM characters "
                                  "LEFT JOIN guild_member ON characters.guid = guild_member.guid "
                                  "WHERE characters.account = '%u' ORDER BY characters.guid "
                                  "LIMIT 0,10",
                                   GetAccountId());
}

void WorldSession::HandleCharCreateOpcode(WorldPacket & recv_data)
{
    std::string name;
    uint8 race_, class_;
    recv_data >> name;
    recv_data >> race_;
    recv_data >> class_;
    uint8 gender, skin, face, hairStyle, hairColor, facialHair, outfitId;
    recv_data >> gender >> skin >> face;
    recv_data >> hairStyle >> hairColor >> facialHair >> outfitId;
    uint32 challengeMask;
    recv_data >> challengeMask;

    CharacterCreateInfo info;
    info.name = name;
    info.race = race_;
    info.class_ = class_;
    info.gender = gender;
    info.skin = skin;
    info.face = face;
    info.hairStyle = hairStyle;
    info.hairColor = hairColor;
    info.facialHair = facialHair;
    info.outfitId = outfitId;
    info.challengeMask = challengeMask;
    info.remoteAddress = GetRemoteAddress();
    info.currentRealmCharacterCount = _charactersCount;
    info.currentRealmCharacterCountProvided = true;

    CharacterCreateOutcome outcome = CharacterCreation::CreateCharacter(GetAccountId(), info);

    if (outcome.result == CHAR_CREATE_SUCCESS)
    {
        uint32 limit = sWorld.getConfig(CONFIG_UINT32_CHARACTERS_PER_REALM);
        if (outcome.newCharactersCount)
            _charactersCount = std::min(outcome.newCharactersCount, limit);
        else
            _charactersCount = std::min<uint32>(_charactersCount + 1, limit);
    }

    switch (outcome.failureReason)
    {
        case CharacterCreateFailureReason::InvalidClassOrRace:
        {
            std::stringstream oss;
            oss << "Attempt to create character of invalid Class (" << int(class_) << ") or Race (" << int(race_) << ")";
            ProcessAnticheatAction("PassiveAnticheat", oss.str().c_str(), CHEAT_ACTION_INFO_LOG);
            break;
        }
        case CharacterCreateFailureReason::NonPlayableRace:
        {
            std::stringstream oss;
            oss << "Attempt to create character of non-playable Race (" << int(race_) << ")";
            ProcessAnticheatAction("PassiveAnticheat", oss.str().c_str(), CHEAT_ACTION_INFO_LOG);
            break;
        }
        case CharacterCreateFailureReason::InvalidName:
            ProcessAnticheatAction("PassiveAnticheat", "Attempt to create character with invalid name", CHEAT_ACTION_INFO_LOG);
            break;
        case CharacterCreateFailureReason::None:
            break;
    }

    WorldPacket data(SMSG_CHAR_CREATE, 1);
    data << uint8(outcome.result);
    SendPacket(&data);
}

void WorldSession::HandleCharDeleteOpcode(WorldPacket & recv_data)
{
    ObjectGuid guid;
    recv_data >> guid;

    // can't delete loaded character
    if (ObjectAccessor::FindPlayerNotInWorld(guid))
        return;

    /*if (!_analyser->IsActive() || IsSuspicious())
    {
        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << (uint8)CHAR_DELETE_FAILED;
        SendPacket(&data);
        return;
    }*/

    uint32 accountId = 0;
    std::string name;

    // is guild leader
    if (sGuildMgr.GetGuildByLeader(guid))
    {
        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << (uint8)CHAR_DELETE_FAILED;
        SendPacket(&data);
        return;
    }

    uint32 lowguid = guid.GetCounter();

    if (sWorld.IsCharacterLocked(lowguid))
    {
        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << (uint8)CHAR_DELETE_FAILED_LOCKED_FOR_TRANSFER;
        SendPacket(&data);
        return;
    }

    PlayerCacheData* cacheData = sObjectMgr.GetPlayerDataByGUID(lowguid);
    if (!cacheData)
        return; // Character not found

    accountId = cacheData->uiAccount;
    name = cacheData->sName;

    // prevent deleting other players' characters using cheating tools
    if (accountId != GetAccountId())
        return;

    std::string IP_str = GetRemoteAddress();
    BASIC_LOG("Account: %d (IP: %s) Delete Character:[%s] (guid: %u)", GetAccountId(), IP_str.c_str(), name.c_str(), lowguid);
    sLog.out(LOG_CHAR, "[%s:%u@%s] Delete Character:[%s] (guid: %u)", GetUsername().c_str(), GetAccountId(), IP_str.c_str(), name.c_str(), lowguid);
    sDBLogger.LogCharAction({ lowguid, GetAccountId(), LogCharAction::ActionDelete, {} });

    // If the character is online (ALT-F4 logout for example)
    if (Player* onlinePlayer = sObjectAccessor.FindPlayer(guid))
        onlinePlayer->GetSession()->LogoutPlayer(true);

    Player::DeleteFromDB(guid, GetAccountId());
    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_DELETE, [&](PlayerScript* script)
    {
        script->OnDelete(guid, GetAccountId());
    });

    WorldPacket data(SMSG_CHAR_DELETE, 1);
    data << (uint8)CHAR_DELETE_SUCCESS;
    SendPacket(&data);
}

void WorldSession::HandlePlayerLoginOpcode(WorldPacket & recv_data)
{
    ObjectGuid playerGuid;
    recv_data >> playerGuid;

    HeadlessSessionState headlessState = sWorld.GetHeadlessSessionState(playerGuid);
    if (PlayerLoading() || GetPlayer() != nullptr ||
        !playerGuid.IsPlayer() || sWorld.IsCharacterLocked(playerGuid.GetCounter()))
    {
        WorldPacket data(SMSG_CHARACTER_LOGIN_FAILED, 1);
        data << (uint8)1;
        SendPacket(&data);
        return;
    }

    PlayerCacheData* cacheData = sObjectMgr.GetPlayerDataByGUID(playerGuid.GetCounter());
    if (!cacheData || cacheData->uiAccount != GetAccountId())

    {
        WorldPacket data(SMSG_CHARACTER_LOGIN_FAILED, 1);
        data << (uint8)1;
        SendPacket(&data);
        return;
    }

    // Module hook: a bot session holding this character yields to the real client
    // logging in.
    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_BOT_LOGIN_YIELD,
        [&](WorldScript* s) { s->OnBotLoginYield(playerGuid.GetCounter()); });
    // A real client always wins. Pending/Loading Headless sessions have not
    // materialized a Player yet, so cancel them before dispatching this login.
    if ((headlessState == HeadlessSessionState::Pending ||
         headlessState == HeadlessSessionState::Loading) &&
        !sWorld.StopHeadlessSession(playerGuid, false))
    {
        WorldPacket data(SMSG_CHARACTER_LOGIN_FAILED, 1);
        data << (uint8)1;
        SendPacket(&data);
        return;
    }

    DEBUG_LOG("WORLD: Recvd Player Logon Message");
    LoginPlayer(playerGuid);
}

//This is what most initial priority is given.
//These values should stay in match with CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_PER_TICK to make sure whatever bonuses we give is in line.
//Should probably in future also take last login time into account.. ?
uint32 WorldSession::GetBasePriority() const
{
    const uint32 donatorSettings = sWorld.getConfig(CONFIG_UINT32_PRIORITY_QUEUE_DONATOR_SETTINGS);

    uint32 priority = 0;
    if (donatorSettings == 1) // enable donator boost on any account.
        priority += sWorld.getConfig(CONFIG_UINT32_PRIORITY_QUEUE_DONATOR_PRIORITY);
    else if (donatorSettings == 2) // donator boost for non-western only.
    {
        if (GetRegion() == WorldRegion::Eastern)
            priority += sWorld.getConfig(CONFIG_UINT32_PRIORITY_QUEUE_DONATOR_PRIORITY);
    }

    if (sWorld.getConfig(CONFIG_BOOL_PRIORITY_QUEUE_ENABLE_WESTERN_PRIORITY) && GetRegion() == WorldRegion::Western)
        priority += sWorld.getConfig(CONFIG_UINT32_PRIORITY_QUEUE_WESTERN_PRIORITY);


    priority += GetMaxLevelCharacterValue() >= sWorld.getConfig(CONFIG_UINT32_PRIORITY_QUEUE_HIGH_LEVEL_CHAR) ? sWorld.getConfig(CONFIG_UINT32_PRIORITY_QUEUE_HIGH_LEVEL_CHAR_PRIORITY) : 0;

    time_t currentTime = time(nullptr);
    uint32 diff = currentTime - GetJoinTimeStamp();
    uint32 diffInDays = diff / DAY;

    priority += diffInDays * sWorld.getConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_PER_ACCOUNT_DAY);


    return priority;
}

bool WorldSession::LoginPlayer(ObjectGuid loginPlayerGuid, uint64 requestToken)
{
    ASSERT(loginPlayerGuid.IsPlayer());
    if (m_playerLoading || GetPlayer())
        return false;

    if (!requestToken)
        requestToken = NextLoginRequestToken();

    LoginQueryHolder* holder = new LoginQueryHolder(GetAccountId(), loginPlayerGuid,
        GetTransport(), requestToken);
    if (!holder->Initialize())
    {
        delete holder;                                      // delete all unprocessed queries
        return false;
    }

    m_loginRequestGuid = loginPlayerGuid;
    m_loginRequestToken = requestToken;
    m_playerLoading = true;
    m_headlessLoginRequested = IsHeadless();
    if (!CharacterDatabase.DelayQueryHolderUnsafe(&chrHandler, &CharacterHandler::HandlePlayerLoginCallback, holder))
    {
        delete holder;
        m_playerLoading = false;
        m_headlessLoginRequested = false;
        return false;
    }

    return true;
}

void WorldSession::HandlePlayerLogin(LoginQueryHolder *holder)
{
    if (!holder || !IsLoginRequest(holder->GetGuid(), holder->GetTransport(),
        holder->GetRequestToken()) || holder->GetAccountId() != GetAccountId())
    {
        delete holder;
        return;
    }

    // The following fixes a crash. Use case:
    // Session1 created, requests login, kicked.
    // Session2 created, requests login, and receives 2 login callback.
    if (GetPlayer() || !m_playerLoading)
    {
        sLog.outInfo("[CRASH] HandlePlayerLogin on session %u with player %s [loading=%u]", GetAccountId(), GetPlayerName(), m_playerLoading);
        delete holder;
        m_playerLoading = false;
        return;
    }
    ObjectGuid playerGuid = holder->GetGuid();
    ASSERT(playerGuid.IsPlayer());
    bool const traceLoginEnabled = GetSocket() && holder->GetLoginRequestTime() &&
        TurtleDiagnostics::enabled.load(std::memory_order_relaxed);
    uint32 const loginRequestedAt = holder->GetLoginRequestTime();
    uint32 loginPreviousStage = loginRequestedAt;
    auto traceLogin = [&](char const* phase)
    {
        if (!traceLoginEnabled) return;
        uint32 const now = WorldTimer::getMSTime();
        sLog.out(LOG_PERFORMANCE, "PLAYER_LOGIN_STAGE account=%u guid=%u phase=%s stage_ms=%u since_request_ms=%u",
            GetAccountId(), playerGuid.GetCounter(), phase,
            WorldTimer::getMSTimeDiff(loginPreviousStage, now), WorldTimer::getMSTimeDiff(loginRequestedAt, now));
        loginPreviousStage = now;
    };
    // Includes DB queue, holder execution and callback dispatch, not SQL alone.
    traceLogin("db_results_ready");

    // If the character is online (ALT-F4 logout for example)
    Player *pCurrChar = sObjectAccessor.FindPlayer(playerGuid);
    MasterPlayer* pCurrMasterPlayer = sObjectAccessor.FindMasterPlayer(playerGuid);
    bool alreadyOnline = false;
    if (pCurrChar)
    {
        // Headless sessions never take over a character already owned by
        // another session. Network login is the reclaiming direction.
        if (IsHeadless())
        {
            delete holder;
            m_playerLoading = false;
            return;
        }

        WorldSession* previousSession = pCurrChar->GetSession();
        if (!previousSession)
        {
            KickPlayer();
            delete holder;
            m_playerLoading = false;
            return;
        }

        // Hacking attempt
        if (previousSession->GetAccountId() != GetAccountId())
        {
            KickPlayer();
            delete holder;
            m_playerLoading = false;
            return;
        }

        // Network login may reclaim only a manager-owned Headless session.
        if (previousSession->IsHeadless())
        {
            if (!sWorld.ReclaimHeadlessSession(playerGuid, previousSession, this, GetAccountId()))
            {
                KickPlayer();
                delete holder;
                m_playerLoading = false;
                return;
            }
        }
        else
        {
            previousSession->SetPlayer(nullptr);
            pCurrChar->SetSession(this);
        }

        // Need to attach packet bcaster to the new socket
        pCurrChar->m_broadcaster->ChangeSocket(GetSocket());
        alreadyOnline = true;

        // If the character had a logout request, then he is articifially stunned (cf CMSG_LOGOUT_REQUEST handler). Fix it here.
        if (pCurrChar->CanFreeMove())
        {
            pCurrChar->SetRootedReal(false);
            pCurrChar->SetStandState(UNIT_STAND_STATE_STAND);
            pCurrChar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        }
    }
    else
    {
        // Character found online but not in world ?
        if (HashMapHolder<Player>::Find(playerGuid))
        {
            sLog.outInfo("[CRASH] Trying to login already ingame character guid %u", playerGuid.GetCounter());
            KickPlayer();
            delete holder;
            m_playerLoading = false;
            return;
        }
        pCurrChar = new Player(this);
        pCurrChar->GetMotionMaster()->Initialize();
    }

    traceLogin("existing_character_resolved");
    // "GetAccountId()==db stored account id" checked in LoadFromDB (prevent login not own character using cheating tools)
    if (alreadyOnline)
        pCurrChar->SendPacketsAtRelogin();
    else if (!pCurrChar->LoadFromDB(playerGuid, holder))
    {
        KickPlayer();                                       // disconnect client, player no set to session and it will not deleted or saved at kick
        delete pCurrChar;                                   // delete it manually
        delete holder;                                      // delete all unprocessed queries
        m_playerLoading = false;
        return;
    }

    ASSERT(pCurrChar->GetSession() == this);
    traceLogin("character_loaded");
    SetPlayer(pCurrChar);
    if (m_antiCheat)
        m_antiCheat->NewPlayer();


    //WE DO NOT NEED TO SEND ALL POSSIBLE TRANSMOGS TO ANY PLAYER ON LOGIN
    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHHh
    /*if (sWorld.getConfig(CONFIG_BOOL_TRANSMOG_ENABLED))
    {
        std::vector<uint32> entries;
        for (auto const& itemTransmog : sObjectMgr.GetItemTransmogrifyTemplateMap())
            entries.push_back(itemTransmog.second->ItemId);

        if (!entries.empty())
            sWorld.SendUpdateMultipleItems(entries, this);
    }*/

    if (pCurrMasterPlayer)
    {
        if (WorldSession* previousSession = pCurrMasterPlayer->GetSession())
            previousSession->SetMasterPlayer(nullptr);
        pCurrMasterPlayer->SetSession(this);
        m_masterPlayer = pCurrMasterPlayer;
    }
    else
    {
        m_masterPlayer = new MasterPlayer(this);
        m_masterPlayer->LoadPlayer(GetPlayer());
        m_masterPlayer->LoadActions(holder->GetResult(PLAYER_LOGIN_QUERY_LOADACTIONS));
        m_masterPlayer->LoadSocial(holder->GetResult(PLAYER_LOGIN_QUERY_LOADSOCIALLIST));
        m_masterPlayer->LoadMails(holder->GetResult(PLAYER_LOGIN_QUERY_LOADMAILS));
        m_masterPlayer->LoadMailedItems(holder->GetResult(PLAYER_LOGIN_QUERY_LOADMAILEDITEMS));
    }

    bool hasDisabledSocials = (m_masterPlayer->m_ExtraFlags & PLAYER_EXTRA_GM_DISABLE_SOCIAL) == PLAYER_EXTRA_GM_DISABLE_SOCIAL;

    GetPlayer()->SetGMSocials(!hasDisabledSocials, true);

    m_masterPlayer->UpdateNextMailTimeAndUnreads();

    sObjectAccessor.AddObject(m_masterPlayer);

    WorldPacket data(SMSG_LOGIN_VERIFY_WORLD, 20);
    traceLogin("social_loaded");
    data << pCurrChar->GetMapId();
    data << pCurrChar->GetPositionX();
    data << pCurrChar->GetPositionY();
    data << pCurrChar->GetPositionZ();
    data << pCurrChar->GetOrientation();
    SendPacket(&data);

    data.Initialize(SMSG_ACCOUNT_DATA_TIMES, 128);
    for (int i = 0; i < 32; ++i)
        data << uint32(0);
    SendPacket(&data);

    // Send MOTD (1.12.1 not have SMSG_MOTD, so do it in another way)
    {
        uint32 linecount = 0;
        std::string str_motd = sWorld.GetMotd();
        std::string::size_type pos, nextpos;

        pos = 0;
        while ((nextpos = str_motd.find('@', pos)) != std::string::npos)
        {
            if (nextpos != pos)
            {
                ChatHandler(pCurrChar).PSendSysMessage(str_motd.substr(pos, nextpos - pos).c_str());
                ++linecount;
            }
            pos = nextpos + 1;
        }

        if (pos < str_motd.length())
        {
            ChatHandler(pCurrChar).PSendSysMessage(str_motd.substr(pos).c_str());
            ++linecount;
        }

        DEBUG_LOG("WORLD: Sent motd (SMSG_MOTD)");
    }

    if (char const* warning = sAccountMgr.GetWarningText(GetAccountId()))
    {
        ChatHandler(pCurrChar).PSendSysMessage("|cFFFF5500Your account has received a warning: %s|r", warning);
        SendNotification("WARNING: %s", warning);
    }

    if (Guild* guild = sGuildMgr.GetGuildById(pCurrChar->GetGuildId()))
    {
        WorldPacket data(SMSG_GUILD_EVENT, (2 + guild->GetMOTD().size() + 1));
        data << uint8(GE_MOTD);
        data << uint8(1);
        data << guild->GetMOTD();
        SendPacket(&data);
        DEBUG_LOG("WORLD: Sent guild-motd (SMSG_GUILD_EVENT)");

        guild->BroadcastEvent(GE_SIGNED_ON, pCurrChar->GetObjectGuid(), pCurrChar->GetName());
    }

    if (!pCurrChar->IsAlive())
        pCurrChar->SendCorpseReclaimDelay(true);

    if (pCurrChar->IsHardcore() && sWorld.GetUptime() < 15 * MINUTE)
        pCurrChar->SetHCImmunityTimer(20);

    pCurrChar->SendInitialPacketsBeforeAddToMap();
    GetMasterPlayer()->SendInitialActionButtons();

    // Show only player accounts cinematic at first log on
    if (!sWorld.getConfig(CONFIG_BOOL_PTR))
    {
        AccountMgr AccountMgr;
        if (AccountMgr.IsPlayerAccount(GetSecurity()))
        {
            if (!pCurrChar->GetCinematic())
            {
                pCurrChar->SetCinematic(1);

                if (ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(pCurrChar->GetRace()))
                    pCurrChar->SendCinematicStart(rEntry->CinematicSequence);
            }
        }
    }

    traceLogin("initial_packets_queued");
    if (!alreadyOnline && !pCurrChar->GetMap()->Add(pCurrChar))
    {
        // normal delayed teleport protection not applied (and this correct) for this case (Player object just created)
        AreaTriggerTeleport const* at = sObjectMgr.GetGoBackTrigger(pCurrChar->GetMapId());
        if (at)
            pCurrChar->TeleportTo(at->destination, pCurrChar->GetOrientation());
        else if (pCurrChar->GetMapId() == 533)
            pCurrChar->TeleportTo(0, 3362.15f, -3379.35f, 144.782f, 6.28319f); // Naxxramas has no exit trigger
        else
            pCurrChar->TeleportToHomebind();

        sMapMgr.ExecuteSingleDelayedTeleport(pCurrChar);
    }

    if (alreadyOnline)
        pCurrChar->GetMap()->ExistingPlayerLogin(pCurrChar); // SendInitSelf ...
    else
        sObjectAccessor.AddObject(pCurrChar);

    //DEBUG_LOG("Player %s added to Map.",pCurrChar->GetName());
    pCurrChar->GetSocial()->SendFriendList();
    pCurrChar->GetSocial()->SendIgnoreList();

    pCurrChar->SendInitialPacketsAfterAddToMap();
    traceLogin("map_and_initial_objects_added");
    if (alreadyOnline)
        pCurrChar->SendInitWorldStates(pCurrChar->GetCachedZoneId());

    static SqlStatementID updChars;

    SqlStatement stmt = CharacterDatabase.CreateStatement(updChars, "UPDATE characters SET online = 1 WHERE guid = ?");
    stmt.PExecute(pCurrChar->GetGUIDLow());

    if (!IsHeadless())
    {
        static SqlStatementID updAccount;
        stmt = LoginDatabase.CreateStatement(updAccount, "UPDATE account SET current_realm = ?, online = 1 WHERE id = ?");
        stmt.PExecute(realmID, GetAccountId());
    }

    pCurrChar->SetInGameTime(WorldTimer::getMSTime());

    // announce group about member online (must be after add to player list to receive announce to self)
    if (Group *group = pCurrChar->GetGroup())
        group->UpdatePlayerOnlineStatus(pCurrChar);

    // friend status
    // TODO: Call it when node finished loading also
    if (GetMasterPlayer())
    {
        GetMasterPlayer()->areaId = pCurrChar->GetCachedAreaId();
        GetMasterPlayer()->zoneId = pCurrChar->GetCachedZoneId();
        if (!pCurrChar->HasGMDisabledSocials())
            sSocialMgr.SendFriendStatus(GetMasterPlayer(), FRIEND_ONLINE, GetMasterPlayer()->GetObjectGuid(), true);

        if (Guild* guild = sGuildMgr.GetGuildById(pCurrChar->GetGuildId()))
            guild->AddToCache(GetMasterPlayer()->GetGUIDLow());
    }

    if (!alreadyOnline)
    {
        // Place character in world (and load zone) before some object loading
        pCurrChar->LoadCorpse();

        // setting Ghost+speed if dead
        if (pCurrChar->m_deathState != ALIVE)
        {
            // not blizz like, we must correctly save and load player instead...
            if (pCurrChar->GetRace() == RACE_NIGHTELF)
                pCurrChar->CastSpell(pCurrChar, 20584, true);   // auras SPELL_AURA_INCREASE_SPEED(+speed in wisp form), SPELL_AURA_INCREASE_SWIM_SPEED(+swim speed in wisp form), SPELL_AURA_TRANSFORM (to wisp form)
            
            pCurrChar->CastSpell(pCurrChar, 8326, true);        // auras SPELL_AURA_GHOST, SPELL_AURA_INCREASE_SPEED(why?), SPELL_AURA_INCREASE_SWIM_SPEED(why?)

            pCurrChar->SetWaterWalking(true);
        }
    }

    // Load pet if any (if player not alive and in taxi flight or another then pet will remember as temporary unsummoned)
    if (alreadyOnline)
    {
        pCurrChar->PetSpellInitialize();
        pCurrChar->SendMirrorTimers(true);
    }
    else
    {
        pCurrChar->ContinueTaxiFlight();
        pCurrChar->LoadPet();
    }
    traceLogin("social_corpse_and_pet_loaded");

    auto maskVar = pCurrChar->GetPlayerVariable(PlayerVariables::PendingChallengeMask);
    if (maskVar && *maskVar != "0")
    {
        uint32 challengeMask = std::stoul(*maskVar);

        static constexpr uint32 challengeSpells[] = {
            SPELL_HARDCORE,         // bit 0
            SPELL_SLOW_AND_STEADY,  // bit 1
            SPELL_WAR_MODE,         // bit 2
            SPELL_VARGANT_MODE,     // bit 3
            SPELL_CRAFTMASTER,      // bit 4
            SPELL_LUNATIC,          // bit 5
            SPELL_BOARING_MODE,     // bit 6
            SPELL_EXHAUSTION_MODE,  // bit 7
            SPELL_BREWMASTER,       // bit 8
            SPELL_HEROIC,           // bit 9
        };

        for (uint32 i = 0; i < (sizeof(challengeSpells) / sizeof(challengeSpells[0])); ++i)
        {
            if (challengeMask & (1u << i))
                pCurrChar->LearnSpell(challengeSpells[i], false);
        }

        if (challengeMask & 0x1) // Hardcore
            pCurrChar->SetupHardcoreMode();

        pCurrChar->SetPlayerVariable(PlayerVariables::PendingChallengeMask, "0");
    }

    // Set FFA PvP for non GM in non-rest mode
    if (sWorld.IsFFAPvPRealm() && !pCurrChar->IsGameMaster() && !pCurrChar->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING))
        pCurrChar->SetFFAPvP(true);

    if (pCurrChar->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
        pCurrChar->UpdatePvPContested(true);

    if (pCurrChar->HasChallenge(CHALLENGE_WAR_MODE))
        pCurrChar->SetPvP(true);

    if (pCurrChar->HasFlag(PLAYER_FLAGS, PLAYER_SALT_FLATS_RACER))
        pCurrChar->RemoveFlag(PLAYER_FLAGS, PLAYER_SALT_FLATS_RACER);

    // Apply at_login requests
    if (pCurrChar->HasAtLoginFlag(AT_LOGIN_RESET_SPELLS))
    {
        pCurrChar->ResetSpells();
        SendNotification(LANG_RESET_SPELLS);
    }

    if (pCurrChar->HasAtLoginFlag(AT_LOGIN_RESET_TALENTS))
    {
        pCurrChar->ResetTalents(true);
        SendNotification(LANG_RESET_TALENTS);               // we can use SMSG_TALENTS_INVOLUNTARILY_RESET here
    }

    if (pCurrChar->HasAtLoginFlag(AT_LOGIN_FIRST))
    {
        pCurrChar->RemoveAtLoginFlag(AT_LOGIN_FIRST);
    }

    pCurrChar->CheckInfernoInvite();

    // show time before shutdown if shutdown planned.
    if (sWorld.IsShutdowning())
        sWorld.ShutdownMsg(true, pCurrChar);

    if (sWorld.getConfig(CONFIG_BOOL_ALL_TAXI_PATHS))
        pCurrChar->SetTaxiCheater(true);

    if (pCurrChar->IsGameMaster())
        SendNotification(LANG_GM_ON);

    if (!pCurrChar->IsGMVisible())
        SendNotification(LANG_INVISIBLE_INVISIBLE, pCurrChar->GetGMInvisibilityLevel());

    std::string IP_str = GetRemoteAddress();

    sLog.out(LOG_CHAR, "[%s:%u@%s] Login Character:[%s] (guid: %u)%s",
             GetUsername().c_str(), GetAccountId(), IP_str.c_str(), pCurrChar->GetName(), pCurrChar->GetGUIDLow(), alreadyOnline ? " Player was already online" : "");
    if (!alreadyOnline && !pCurrChar->IsStandingUp() && !pCurrChar->HasUnitState(UNIT_STAT_STUNNED))
        pCurrChar->SetStandState(UNIT_STAND_STATE_STAND);

    sDBLogger.LogCharAction({ pCurrChar->GetGUIDLow(), GetAccountId(), LogCharAction::ActionLogin, {} });

    m_playerLoading = false;
    m_headlessLoginRequested = false;
    m_clientMoverGuid = pCurrChar->GetObjectGuid();
    traceLogin("login_flag_cleared");
    delete holder;
    if (alreadyOnline)
    {
        pCurrChar->UpdateControl();
        // Send "Release spirit" timer, etc ...
        if (pCurrChar->GetDeathState() == CORPSE)
            pCurrChar->KillPlayer();
    }

    sObjectMgr.UpdatePlayerCachedPosition(pCurrChar);

    if (sWorld.getConfig(CONFIG_BOOL_SEND_LOOT_ROLL_UPON_RECONNECT) && alreadyOnline)
        if (Group* pGroup = pCurrChar->GetGroup())
            pGroup->SendLootStartRollsForPlayer(pCurrChar);

    if (pCurrChar->GetRace() == RACE_GOBLIN && pCurrChar->HasCustomFlag(CUSTOM_PLAYER_FLAG_BROKEN_GOBLIN))
    {
        uint32 itemEntry = 80699;
        std::string subject = "Goblin's Appearance Token";
        std::string message = "Greetings! Use this to improve your look!\n\nSafe travels,\nTurtle WoW Team";
        Item* ToMailItem = Item::CreateItem(itemEntry, 1, pCurrChar);
        ToMailItem->SaveToDB();
        MailDraft(subject, sObjectMgr.CreateItemText(message))
            .AddItem(ToMailItem)
            .SendMailTo(pCurrChar, MailSender(MAIL_CREATURE, uint32(51550), MAIL_STATIONERY_DEFAULT), MAIL_CHECK_MASK_COPIED, 0, 30 * DAY);

        pCurrChar->RemoveCustomFlag(CUSTOM_PLAYER_FLAG_BROKEN_GOBLIN);
    }      
        

    // Chinese Lunar Festivale
    //if (sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK) && !pCurrChar->HasCustomFlag(CUSTOM_PLAYER_FLAG_RECEIVED_LUNAR_GIFT))
    //{
    //    uint32 itemEntry = 91790;
    //    
    //    std::string subject = sObjectMgr.GetMangosString(50306, 4);
    //    std::string message = sObjectMgr.GetMangosString(50307, 4);
    //    Item* ToMailItem = Item::CreateItem(itemEntry, 1, pCurrChar);
    //    ToMailItem->SaveToDB();
    //    MailDraft(subject, sObjectMgr.CreateItemText(message))
    //        .AddItem(ToMailItem)
    //        .SendMailTo(pCurrChar, MailSender(MAIL_CREATURE, uint32(51550), MAIL_STATIONERY_DEFAULT), MAIL_CHECK_MASK_COPIED, 0, 30 * DAY);
    //    pCurrChar->SetCustomFlag(CUSTOM_PLAYER_FLAG_RECEIVED_LUNAR_GIFT);
    //}


    if (pCurrChar->HasCustomFlag(CUSTOM_PLAYER_FLAG_WAS_TRANSFERRED))
    {
        pCurrChar->HandleTransferChecks();
        pCurrChar->RemoveCustomFlag(CUSTOM_PLAYER_FLAG_WAS_TRANSFERRED);
    }

    if (pCurrChar->HasCustomFlag(CUSTOM_PLAYER_FLAG_RACE_CHANGE_CHECK))
    {
        pCurrChar->HandleRaceChangeFixup();
        pCurrChar->RemoveCustomFlag(CUSTOM_PLAYER_FLAG_RACE_CHANGE_CHECK);
    }


    auto security = pCurrChar->GetSession()->GetSecurity();
    if (pCurrChar->GetSession()->GetSecurity() > SEC_PLAYER  && security <= SEC_ADMINISTRATOR)
    {
        sWorld.SendGMText(string_format("GM {} just logged in.", pCurrChar->GetName()));
    }


    pCurrChar->RecallPvPGear();

    // Update warden speeds
    //if (GetWarden())
        //for (int i = 0; i < MAX_MOVE_TYPE; ++i)
            //GetWarden()->SendSpeedChange(UnitMoveType(i), pCurrChar->GetSpeed(UnitMoveType(i)));

    //if (sWorld.getConfig(CONFIG_BOOL_TRANSMOG_ENABLED))
        //sTransmog.LoadTransmog(pCurrChar);


    ALL_SESSION_SCRIPTS(this, OnLogin(pCurrChar));
    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_LOGIN, [&](PlayerScript* script)
    {
        script->OnLogin(pCurrChar);
    });

}

void WorldSession::HandleSetFactionAtWarOpcode(WorldPacket & recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_SET_FACTION_ATWAR");

    uint32 repListID;
    uint8  flag;

    recv_data >> repListID;
    recv_data >> flag;

    GetPlayer()->GetReputationMgr().SetAtWar(repListID, flag);
}

void WorldSession::HandleTutorialFlagOpcode(WorldPacket & recv_data)
{
    uint32 iFlag;
    recv_data >> iFlag;

    uint32 wInt = (iFlag / 32);
    if (wInt >= 8)
    {
        //sLog.outError("CHEATER? Account:[%d] Guid[%u] tried to send wrong CMSG_TUTORIAL_FLAG", GetAccountId(),GetGUID());
        return;
    }
    uint32 rInt = (iFlag % 32);

    uint32 tutflag = GetTutorialInt(wInt);
    tutflag |= (1 << rInt);
    SetTutorialInt(wInt, tutflag);

    //DEBUG_LOG("Received Tutorial Flag Set {%u}.", iFlag);
}

void WorldSession::HandleTutorialClearOpcode(WorldPacket & /*recv_data*/)
{
    for (uint32 iI = 0; iI < 8; ++iI)
        SetTutorialInt(iI, 0xFFFFFFFF);
}

void WorldSession::HandleTutorialResetOpcode(WorldPacket & /*recv_data*/)
{
    for (uint32 iI = 0; iI < 8; iI++)
        SetTutorialInt(iI, 0x00000000);
}

void WorldSession::HandleSetWatchedFactionOpcode(WorldPacket & recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_SET_WATCHED_FACTION");
    int32 repId;
    recv_data >> repId;
    GetPlayer()->SetInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, repId);
}

void WorldSession::HandleSetFactionInactiveOpcode(WorldPacket & recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_SET_FACTION_INACTIVE");
    uint32 replistid;
    uint8 inactive;
    recv_data >> replistid >> inactive;

    _player->GetReputationMgr().SetInactive(replistid, inactive);
}

void WorldSession::HandleShowingHelmOpcode(WorldPacket & /*recv_data*/)
{
    DEBUG_LOG("CMSG_SHOWING_HELM for %s", _player->GetName());
    _player->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM);
}

void WorldSession::HandleShowingCloakOpcode(WorldPacket & /*recv_data*/)
{
    DEBUG_LOG("CMSG_SHOWING_CLOAK for %s", _player->GetName());
    _player->ToggleFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_CLOAK);
}

void WorldSession::HandleCharRenameOpcode(WorldPacket& recv_data)
{
    ObjectGuid guid;
    std::string newname;

    recv_data >> guid;
    recv_data >> newname;

    // prevent character rename to invalid name
    if (!normalizePlayerName(newname))
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(CHAR_NAME_NO_NAME);
        SendPacket(&data);
        return;
    }

    uint8 res = ObjectMgr::CheckPlayerName(newname, true);
    if (res != CHAR_NAME_SUCCESS)
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(res);
        SendPacket(&data);
        return;
    }

    // check name limitations
    if (GetSecurity() == SEC_PLAYER && sObjectMgr.IsReservedName(newname))
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(CHAR_NAME_RESERVED);
        SendPacket(&data);
        return;
    }

    std::string escaped_newname = newname;
    CharacterDatabase.escape_string(escaped_newname);

    // make sure that the character belongs to the current account, that rename at login is enabled
    // and that there is no character with the desired new name
    CharacterDatabase.AsyncPQuery(&WorldSession::HandleChangePlayerNameOpcodeCallBack,
                                  GetAccountId(), newname,
                                  "SELECT guid, name FROM characters WHERE guid = %u AND account = %u AND (at_login & %u) = %u AND NOT EXISTS (SELECT NULL FROM characters WHERE name = '%s')",
                                  guid.GetCounter(), GetAccountId(), AT_LOGIN_RENAME, AT_LOGIN_RENAME, escaped_newname.c_str()
                                 );
}

void WorldSession::HandleChangePlayerNameOpcodeCallBack(QueryResult *result, uint32 accountId, std::string newname)
{
    WorldSession * session = sWorld.FindSession(accountId);
    if (!session)
    {
        delete result;
        return;
    }

    if (!result)
    {
        WorldPacket data(SMSG_CHAR_RENAME, 1);
        data << uint8(CHAR_CREATE_ERROR);
        session->SendPacket(&data);
        return;
    }

    uint32 guidLow = result->Fetch()[0].GetUInt32();
    ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, guidLow);
    std::string oldname = result->Fetch()[1].GetCppString();

    delete result;

    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("UPDATE characters set name = '%s', at_login = at_login & ~ %u WHERE guid ='%u'", newname.c_str(), uint32(AT_LOGIN_RENAME), guidLow);
    CharacterDatabase.CommitTransaction();

    sLog.out(LOG_CHAR, "[%s:%u@%s] Character:[%s] (guid:%u) Changed name to: %s", session->GetUsername().c_str(), session->GetAccountId(), session->GetRemoteAddress().c_str(), oldname.c_str(), guidLow, newname.c_str());

    sDBLogger.LogCharAction({ guidLow, session->GetAccountId(), LogCharAction::ActionRename, CharActionRenameEntry{0, oldname, newname} });

    WorldPacket data(SMSG_CHAR_RENAME, 1 + 8 + (newname.size() + 1));
    data << uint8(RESPONSE_SUCCESS);
    data << guid;
    data << newname;
    session->SendPacket(&data);

    sObjectMgr.ChangePlayerNameInCache(guidLow, oldname, newname);
    sWorld.InvalidatePlayerDataToAllClients(guid);
}
