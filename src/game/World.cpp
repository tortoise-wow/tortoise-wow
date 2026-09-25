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

/** \file
    \ingroup world
*/

#include "World.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"
#include "CustomMerchantMgr.h"
#include "Platform/Define.h"
#include "SystemConfig.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "HeadlessSessionMgr.h"
#include "WorldPacket.h"
#include "Weather.h"
#include "Player.h"
#include "AccountMgr.h"
#include "AuctionHouseMgr.h"
#include "ObjectMgr.h"
#include "CreatureEventAIMgr.h"
#include "GuildMgr.h"
#include "SpellMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "MassMailMgr.h"
#include "LootMgr.h"
#include "ItemEnchantmentMgr.h"
#include "MapManager.h"
#include "ScriptMgr.h"
#include "ScriptObjects.h"
#include "CreatureAIRegistry.h"
#include "Policies/SingletonImp.h"
#include "BattleGroundMgr.h"
#include "TemporarySummon.h"
#include "VMapFactory.h"
#include "GameEventMgr.h"
#include "PoolManager.h"
#include "Database/DatabaseImpl.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "MapPersistentStateMgr.h"
#include "WaypointManager.h"
#include "GMTicketMgr.h"
#include "Util.h"
#include "CharacterDatabaseCleaner.h"
#include "LFGMgr.h"
#include "LFTMgr.h"
#include "AutoBroadCastMgr.h"
#include "Transports/TransportMgr.h"
#include "ZoneScriptMgr.h"
#include "CharacterDatabaseCache.h"
#include "CreatureGroups.h"
#include "MoveMap.h"
#include "SpellModMgr.h"
#include "MovementBroadcaster.h"
#include "HonorMgr.h"
#include "ThreadPool.h"
#include "AuraRemovalMgr.h"
#include "GuardMgr.h"
#include "DailyQuestHandler.h"
#include "GuidObjectScaling.h"
#include "Database/AutoUpdater.hpp"
#include "CompanionManager.hpp"
#include "MountManager.hpp"
#include "ToyManager.hpp"
#include "PlayerDump.h"
#include "Anticheat/libanticheat.hpp"
#include "Anticheat/Config.hpp"
#include "re2/re2.h"
#include "Logging/DatabaseLogger.hpp"
#include "SuspiciousStatisticMgr.h"
#include "HttpApi/ApiServer.hpp"
#include "SocialMgr.h"
#include "Shop/ShopMgr.h"
#include "ChannelBroadcaster.h"
#include <ace/OS_NS_dirent.h>
#include "PerformanceMonitor.h"

#include <filesystem>

#ifdef USING_DISCORD_BOT
#include "DiscordBot/Bot.hpp"


namespace DiscordBot
{
    void RegisterHandlers();
}
#endif

namespace HttpApi
{
    void RegisterControllers();
}

#include <filesystem>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <string>
#include <iostream>

#include <chrono>

volatile bool World::m_stopEvent = false;
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;
volatile uint32 World::m_worldLoopCounter = 0;

float World::m_MaxVisibleDistanceOnContinents = DEFAULT_VISIBILITY_DISTANCE;
float World::m_MaxVisibleDistanceInInstances = DEFAULT_VISIBILITY_INSTANCE;
float World::m_MaxVisibleDistanceInBG = DEFAULT_VISIBILITY_BG;

float World::m_MaxVisibleDistanceInFlight = DEFAULT_VISIBILITY_DISTANCE;
float World::m_VisibleUnitGreyDistance = 0;
float World::m_VisibleObjectGreyDistance = 0;

float  World::m_relocation_lower_limit_sq = 10.f * 10.f;
uint32 World::m_relocation_ai_notify_delay = 1000u;

using namespace std::literals::chrono_literals;

void LoadGameObjectModelList();

World sWorld;

GuidObjectScaling sGuidObjectScaling;
CompanionManager sCompanionMgr;
MountManager sMountMgr;
ToyManager sToyMgr;

/// World constructor
World::World():
    m_playerLimit(0),
    m_allowMovement(true),
    m_gameTime(time(nullptr)),
    m_timeZoneOffset(0),
    m_gameDay((m_gameTime + m_timeZoneOffset) / DAY),
    m_startTime(m_gameTime),
    m_defaultDbcLocale(LOCALE_enUS),
    m_timeRate(1.0f),
    m_canProcessAsyncPackets(false)
{
    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_maxActiveSessionCount = 0;
    m_maxQueuedSessionCount = 0;
    m_MaintenanceTimeChecker = 0;
    m_anticrashRearmTimer = 0;

    m_defaultDbcLocale = LOCALE_enUS;
    m_availableDbcLocaleMask = 0;

    for (uint32 & value : m_configUint32Values)
        value = 0;

    for (int32 & value : m_configInt32Values)
        value = 0;

    for (float & value : m_configFloatValues)
        value = 0.0f;

    for (bool & value : m_configBoolValues)
        value = false;

    m_timeRate = 1.0f;
    m_charDbWorkerThread    = nullptr;
    m_headlessSessionMgr = std::make_unique<HeadlessSessionMgr>(*this);
}

/// World destructor
World::~World()
{
}

void World::Shutdown()
{
    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_SHUTDOWN, [](WorldScript* script)
    {
        script->OnShutdown();
    });

	sGuildMgr.SaveGuildBanks();
    sWorld.KickAll();                                       // save and kick all players
    sWorld.UpdateSessions(1);                               // real players unload required UpdateSessions call
    if (m_charDbWorkerThread && m_charDbWorkerThread->joinable())
        m_charDbWorkerThread->join();
}

AccountDataWrapper::~AccountDataWrapper()
{
    //relink lookuptable with normal table.
    auto itr = sWorld.m_accountDataLookup.find(m_data->username);
    if (itr != sWorld.m_accountDataLookup.end())
        itr->second = std::cref(*m_data);
    else
        sWorld.m_accountDataLookup.emplace(m_data->username, std::cref(*m_data));
}

void World::InternalShutdown()
{
	///- Empty the kicked session set
	while (!m_sessions.empty())
	{
		// not remove from queue, prevent loading new sessions
		delete m_sessions.begin()->second;
		m_sessions.erase(m_sessions.begin());
	}

    m_headlessSessionMgr->Shutdown();

	CliCommandHolder* command = nullptr;
	while (cliCmdQueue.next(command))
		delete command;

	VMAP::VMapFactory::clear();

	if (m_charDbWorkerThread)
	{
        if (m_charDbWorkerThread->joinable())
            m_charDbWorkerThread->join();
        m_charDbWorkerThread.reset(nullptr);
	}
	//TODO free addSessQueue

	m_broadcaster.reset();
    m_ChannelBroadcaster.reset();

    if (m_autoCommitThread.joinable())
        m_autoCommitThread.join();

    if (m_autoPDumpThread.joinable())
        m_autoPDumpThread.join();

    if (m_asyncPacketsThread.joinable())
        m_asyncPacketsThread.join();

    if (m_shopThread.joinable())
        m_shopThread.join();

#ifdef USING_DISCORD_BOT
    sDiscordBot->Stop();
#endif
}

/// Find a session by its id
WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end())
        return itr->second;                                 // also can return nullptr for kicked session
    else
        return nullptr;
}

/// Remove a given session
bool World::RemoveSession(uint32 id)
{
    ///- Find the session, kick the user, but we can't delete session at this moment to prevent iterator invalidation
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end() && itr->second)
    {
        if (itr->second->PlayerLoading())
            return false;
        itr->second->KickPlayer();
    }

    return true;
}

void World::AddSession(WorldSession* s)
{
    addSessQueue.add(s);
}

HeadlessSessionStartResult World::StartHeadlessSession(uint32 accountId, ObjectGuid characterGuid,
    LocaleConstant locale, std::string const& tag)
{
    return m_headlessSessionMgr->Start(accountId, characterGuid, locale, tag);
}

bool World::StopHeadlessSession(ObjectGuid characterGuid, bool save)
{
    return m_headlessSessionMgr->Stop(characterGuid, save);
}

HeadlessSessionState World::GetHeadlessSessionState(ObjectGuid characterGuid) const
{
    return m_headlessSessionMgr->GetState(characterGuid);
}

void World::HandleHeadlessLoginCallback(LoginQueryHolder* holder)
{
    m_headlessSessionMgr->HandleLoginCallback(holder);
}

bool World::ReclaimHeadlessSession(ObjectGuid characterGuid, WorldSession* session,
    WorldSession* replacement, uint32 accountId)
{
    return m_headlessSessionMgr->ReclaimForNetwork(characterGuid, session, replacement, accountId);
}

void World::StopHeadlessSessionsForAccount(uint32 accountId, bool save)
{
    m_headlessSessionMgr->StopForAccount(accountId, save);
}

bool World::HasOtherSessionForAccount(uint32 accountId, WorldSession const* excluded) const
{
    for (auto const& entry : m_sessions)
    {
        if (entry.second && entry.second != excluded &&
            entry.second->GetAccountId() == accountId)
            return true;
    }

    return false;
}

void World::AddSession_(WorldSession* s)
{
    MANGOS_ASSERT(s);

    //NOTE - Still there is race condition in WorldSession* being used in the Sockets

    ///- kick already loaded player with same account (if any) and remove session
    ///- if player is in loading and want to load again, return
    if (!RemoveSession(s->GetAccountId()))
    {
        s->KickPlayer();
        delete s;                                           // session not added yet in session list, so not listed in queue
        return;
    }

    // decrease session counts only at not reconnection case
    bool decrease_session = true;

    // if session already exist, prepare to it deleting at next world update
    // NOTE - KickPlayer() should be called on "old" in RemoveSession()
    {
        SessionMap::const_iterator old = m_sessions.find(s->GetAccountId());

        if (old != m_sessions.end())
        {
            // prevent decrease sessions count if session queued
            if (RemoveQueuedSession(old->second))
                decrease_session = false;
            if (!old->second->ForcePlayerLogoutDelay())
                delete old->second;
        }
    }

    m_sessions[s->GetAccountId()] = s;
    m_Ipconnections[s->GetBinaryAddress()]++;

    uint32 Sessions = GetActiveAndQueuedSessionCount();
    uint32 pLimit = GetPlayerAmountLimit();
    uint32 QueueSize = GetQueuedSessionCount();             //number of players in the queue

    //so we don't count the user trying to
    //login as a session and queue the socket that we are using
    if (decrease_session)
        --Sessions;

    int32 hardPlayerLimit = getConfig(CONFIG_UINT32_PLAYER_HARD_LIMIT);
    uint32 maxNonRegionalPercentage = getConfig(CONFIG_UINT32_MAX_PERCENTAGE_POP_NON_REGIONAL);
    uint32 maxRegionalPercentage = getConfig(CONFIG_UINT32_MAX_PERCENTAGE_POP_REGIONAL);

    uint32 maxNonRegionalPop = hardPlayerLimit / 100.f * (float)maxNonRegionalPercentage;
    uint32 maxRegionalPop = hardPlayerLimit / 100.f * (float)maxRegionalPercentage;

    ///uint32 CorrectMaxNonRegionalPop = (uint32)((float(maxNonRegionalPercentage) / 100.0f) * float(hardPlayerLimit));
    //uint32 CorrectMaxRegionalPop = (uint32)((float(maxRegionalPercentage) / 100.0f) * float(hardPlayerLimit));

    uint32 currentNonRegionalPop = loggedNonRegionSessions;
    uint32 currentRegionalPop = loggedRegionSessions;

    uint32 index = s->GetQueueIndex();
    uint32 currentPop = s->sessionDbcLocaleRaw == LOCALE_zhCN ? currentNonRegionalPop : currentRegionalPop;
    uint32 maxPop = s->sessionDbcLocaleRaw == LOCALE_zhCN ? maxNonRegionalPop : maxRegionalPop;

    if ((currentPop >= maxPop || GetActiveSessionCount() >= hardPlayerLimit) && !CanSkipQueue(s))
    {
        AddQueuedSession(s);
        UpdateMaxSessionCounters();
        DETAIL_LOG("PlayerQueue: Account id %u is in Queue Position (%u).", s->GetAccountId(), ++QueueSize);
        return;
    }

    // Checked for 1.12.2
    WorldPacket packet(SMSG_AUTH_RESPONSE, 1 + 4 + 1 + 4);
    packet << uint8(AUTH_OK);
    packet << uint32(0);                                    // BillingTimeRemaining
    packet << uint8(0);                                     // BillingPlanFlags
    packet << uint32(0);                                    // BillingTimeRested
    s->SendPacket(&packet);

    s->OnPassedQueue();

    UpdateMaxSessionCounters();

    // Updates the population
    if (pLimit > 0)
    {
        float popu = float(GetActiveSessionCount());        // updated number of users on the server
        popu /= pLimit;
        popu *= 2;

        static SqlStatementID id;

        SqlStatement stmt = LoginDatabase.CreateStatement(id, "UPDATE realmlist SET population = ? WHERE id = ?");
        stmt.PExecute(popu, realmID);

        DETAIL_LOG("Server Population (%f).", popu);
    }
}

int32 World::GetQueuedSessionPos(WorldSession* sess)
{
    if (getConfig(CONFIG_BOOL_ENABLE_PRIORITY_QUEUE))
    {
        uint32 position = 1;

        uint32 index = sess->GetQueueIndex();

        for (const auto& elem : m_priorityQueue[index])
        {
            if (elem.second == sess)
                return position;
            ++position;
        }
    }
    else
    {
        uint32 position = 1;

        for (Queue::const_iterator iter = m_QueuedSessions.begin(); iter != m_QueuedSessions.end(); ++iter, ++position)
            if ((*iter) == sess)
                return position;
    }
    return 0;
}

uint32 World::GetConnectionCountByIp(uint32 ip) const
{
    auto itr = m_Ipconnections.find(ip);
    if (itr != m_Ipconnections.end())
        return itr->second;
    return 0;
}

bool World::IsAprilFools() const
{
    return sObjectMgr.GetSavedVariable(VAR_APRIL_FOOLS_ENABLED, 0); 
}

void World::AddQueuedSession(WorldSession* sess)
{
    sess->SetInQueue(true);

    if (getConfig(CONFIG_BOOL_ENABLE_PRIORITY_QUEUE))
    {
        static uint32 idxMarker = 0; // use in future to collect latest search and start there next search instead of iterating whole queue.

        uint32 priority = sess->GetBasePriority();
        uint32 index = sess->GetQueueIndex();

        if (getConfig(CONFIG_BOOL_PRIORITY_QUEUE_ENABLE_IP_PENALTY))
        {
            if (GetConnectionCountByIp(sess->GetBinaryAddress()) > 1)
            {
                const uint32 priorityReduction = getConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_REDUCTION_MULTIBOX);
                if (priorityReduction >= priority)
                    priority = 0;
                else
                    priority -= priorityReduction;
            }
        }

        auto itr = m_priorityQueue[index].begin();
        for (; itr != m_priorityQueue[index].end(); ++itr)
        {
            if (itr->first < priority)
                break;
        }

        m_priorityQueue[index].insert(itr, std::make_pair(priority, sess));
        //m_priorityQueue[index].push_back(std::make_pair(priority, sess));
    }
    else
        m_QueuedSessions.push_back(sess);

    // [-ZERO] Possible wrong
    // The 1st SMSG_AUTH_RESPONSE needs to contain other info too.
    WorldPacket packet(SMSG_AUTH_RESPONSE, 1 + 4 + 1 + 4 + 4);
    packet << uint8(AUTH_WAIT_QUEUE);
    packet << uint32(0);                                    // BillingTimeRemaining
    packet << uint8(0);                                     // BillingPlanFlags
    packet << uint32(0);                                    // BillingTimeRested
    packet << uint32(GetQueuedSessionPos(sess));            // position in queue
    sess->SendPacket(&packet);

    //sess->SendAuthWaitQue (GetQueuePos (sess));
}

bool World::RemoveQueuedSession(WorldSession* sess)
{
    // sessions count including queued to remove (if removed_session set)
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;

    //we have to copy most of OG queue over because it wont allow to do runtime container ifs with different iterator traits.
    if (getConfig(CONFIG_BOOL_ENABLE_PRIORITY_QUEUE))
    {
        uint32 index = sess->GetQueueIndex();
        auto itr = m_priorityQueue[index].begin();

        bool found = false;

        for (; itr != m_priorityQueue[index].end(); ++itr, ++position)
        {
            if (itr->second == sess)
            {
                sess->SetInQueue(false);
                found = true;
                itr = m_priorityQueue[index].erase(itr);
                break;
            }
        }

        if (!found && sessions)
            --sessions;


        //always return here for now.
        return found;

        // accept first in queue
        if ((!m_playerLimit || (int32)sessions < m_playerLimit) && !m_priorityQueue[index].empty())
        {
            WorldSession* pop_sess = m_priorityQueue[index].begin()->second;
            pop_sess->SetInQueue(false);
            pop_sess->m_idleTime = WorldTimer::getMSTime();
            pop_sess->SendAuthWaitQue(0);
            m_priorityQueue[index].erase(m_priorityQueue[index].begin());

            itr = m_priorityQueue[index].begin();
            position = 1;
        }

        for (; itr != m_priorityQueue[index].end(); ++itr, ++position)
            itr->second->SendAuthWaitQue(position);

        return found;
    }
    else
    {
        Queue::iterator iter = m_QueuedSessions.begin();

        // search to remove and count skipped positions
        bool found = false;

        for (; iter != m_QueuedSessions.end(); ++iter, ++position)
        {
            if (*iter == sess)
            {
                sess->SetInQueue(false);
                iter = m_QueuedSessions.erase(iter);
                found = true;                                   // removing queued session
                break;
            }
        }

        // iter point to next socked after removed or end()
        // position store position of removed socket and then new position next socket after removed

        // if session not queued then we need decrease sessions count
        if (!found && sessions)
            --sessions;

        uint32 loggedInSessions = uint32(m_sessions.size() - m_QueuedSessions.size());
        if (loggedInSessions >= getConfig(CONFIG_UINT32_PLAYER_HARD_LIMIT))
            return found;

        // accept first in queue
        if ((!m_playerLimit || (int32)sessions < m_playerLimit) && !m_QueuedSessions.empty())
        {
            WorldSession* pop_sess = m_QueuedSessions.front();
            pop_sess->SetInQueue(false);
            pop_sess->m_idleTime = WorldTimer::getMSTime();
            pop_sess->SendAuthWaitQue(0);
            m_QueuedSessions.pop_front();

            // update iter to point first queued socket or end() if queue is empty now
            iter = m_QueuedSessions.begin();
            position = 1;
        }

        // update position from iter to end()
        // iter point to first not updated socket, position store new position
        for (; iter != m_QueuedSessions.end(); ++iter, ++position)
            (*iter)->SendAuthWaitQue(position);

        return found;
    }

}

void World::LoadConfigSettingsCommonPart(bool reload)
{
    if (!reload)
        m_lastDiffs.resize(50);

#ifdef USE_ANTICHEAT
    sAnticheatConfig.SetSource("anticheat.conf");
    sAnticheatConfig.loadConfigSettings();
#endif

    ///- Read the player limit and the Message of the day from the config file
    SetPlayerLimit(sConfig.GetIntDefault("PlayerLimit", DEFAULT_PLAYER_LIMIT), true);
    SetMotd(sConfig.GetStringDefault("Motd", "Welcome to the Massive Network Game Object Server."));
    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_MOTD_CHANGE, [&](WorldScript* script)
    {
        script->OnMotdChange(m_motd);
    });

    if (reload)
        sMapMgr.SetGridCleanUpDelay(getConfig(CONFIG_UINT32_INTERVAL_GRIDCLEAN));

    if (reload)
        sMapMgr.SetMapUpdateInterval(getConfig(CONFIG_UINT32_INTERVAL_MAPUPDATE));

    m_timeZoneOffset = sConfig.GetIntDefault("TimeZoneOffset", 0) * HOUR;

    if (reload)
    {
        m_timers[WUPDATE_UPTIME].SetInterval(getConfig(CONFIG_UINT32_UPTIME_UPDATE) * MINUTE * IN_MILLISECONDS);
        m_timers[WUPDATE_UPTIME].Reset();
    }

    m_timers[WUPDATE_CORPSES].SetInterval(getConfig(CONFIG_UINT32_CORPSES_UPDATE_MINUTES) * MINUTE * IN_MILLISECONDS);

    m_relocation_ai_notify_delay = sConfig.GetIntDefault("Visibility.AIRelocationNotifyDelay", 1000u);
    m_relocation_lower_limit_sq = pow(sConfig.GetFloatDefault("Visibility.RelocationLowerLimit", 10), 2);

    m_VisibleUnitGreyDistance = sConfig.GetFloatDefault("Visibility.Distance.Grey.Unit", 1);
    if (m_VisibleUnitGreyDistance > MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.Grey.Unit can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_VisibleUnitGreyDistance = MAX_VISIBILITY_DISTANCE;
    }
    m_VisibleObjectGreyDistance = sConfig.GetFloatDefault("Visibility.Distance.Grey.Object", 10);
    if (m_VisibleObjectGreyDistance > MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.Grey.Object can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_VisibleObjectGreyDistance = MAX_VISIBILITY_DISTANCE;
    }

    //visibility on continents
    m_MaxVisibleDistanceOnContinents = sConfig.GetFloatDefault("Visibility.Distance.Continents", DEFAULT_VISIBILITY_DISTANCE);
    if (m_MaxVisibleDistanceOnContinents < 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO))
    {
        sLog.outError("Visibility.Distance.Continents can't be less max aggro radius %f", 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceOnContinents = 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceOnContinents + m_VisibleUnitGreyDistance > MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.Continents can't be greater %f", MAX_VISIBILITY_DISTANCE - m_VisibleUnitGreyDistance);
        m_MaxVisibleDistanceOnContinents = MAX_VISIBILITY_DISTANCE - m_VisibleUnitGreyDistance;
    }

    //visibility in instances
    m_MaxVisibleDistanceInInstances = sConfig.GetFloatDefault("Visibility.Distance.Instances", DEFAULT_VISIBILITY_INSTANCE);
    if (m_MaxVisibleDistanceInInstances < 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO))
    {
        sLog.outError("Visibility.Distance.Instances can't be less max aggro radius %f", 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInInstances = 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInInstances + m_VisibleUnitGreyDistance > MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.Instances can't be greater %f", MAX_VISIBILITY_DISTANCE - m_VisibleUnitGreyDistance);
        m_MaxVisibleDistanceInInstances = MAX_VISIBILITY_DISTANCE - m_VisibleUnitGreyDistance;
    }

    //visibility in BG
    m_MaxVisibleDistanceInBG = sConfig.GetFloatDefault("Visibility.Distance.BG", DEFAULT_VISIBILITY_BG);
    if (m_MaxVisibleDistanceInBG < 45 * sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO))
    {
        sLog.outError("Visibility.Distance.BG can't be less max aggro radius %f", 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInBG = 45 * getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInBG + m_VisibleUnitGreyDistance > MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.BG can't be greater %f", MAX_VISIBILITY_DISTANCE - m_VisibleUnitGreyDistance);
        m_MaxVisibleDistanceInBG = MAX_VISIBILITY_DISTANCE - m_VisibleUnitGreyDistance;
    }

    m_MaxVisibleDistanceInFlight = sConfig.GetFloatDefault("Visibility.Distance.InFlight", DEFAULT_VISIBILITY_DISTANCE);
    if (m_MaxVisibleDistanceInFlight + m_VisibleObjectGreyDistance > MAX_VISIBILITY_DISTANCE)
    {
        sLog.outError("Visibility.Distance.InFlight can't be greater %f", MAX_VISIBILITY_DISTANCE - m_VisibleObjectGreyDistance);
        m_MaxVisibleDistanceInFlight = MAX_VISIBILITY_DISTANCE - m_VisibleObjectGreyDistance;
    }

    ///- Read the "Honor" directory from the config file (basically these are PvP ranking logs)
    std::string honorPath = sConfig.GetStringDefault("HonorDir", "./");

    // for empty string use current dir as for absent case
    if (honorPath.empty())
        honorPath = "./";
    // normalize dir path to path/ or path\ form
    else if (honorPath.at(honorPath.length() - 1) != '/' && honorPath.at(honorPath.length() - 1) != '\\')
        honorPath.append("/");

    if (reload)
    {
        if (honorPath != m_honorPath)
            sLog.outError("HonorDir option can't be changed at mangosd.conf reload, using current value (%s).", m_honorPath.c_str());
    }
    else
    {
        m_honorPath = honorPath;
    }


    ///- Read the "Data" directory from the config file
    std::string dataPath = sConfig.GetStringDefault("DataDir", "./");

    // for empty string use current dir as for absent case
    if (dataPath.empty())
        dataPath = "./";
    // normalize dir path to path/ or path\ form
    else if (dataPath.at(dataPath.length() - 1) != '/' && dataPath.at(dataPath.length() - 1) != '\\')
        dataPath.append("/");

    if (reload)
    {
        if (dataPath != m_dataPath)
            sLog.outError("DataDir option can't be changed at mangosd.conf reload, using current value (%s).", m_dataPath.c_str());
    }
    else
    {
        m_dataPath = dataPath;
    }

    bool enableLOS = sConfig.GetBoolDefault("vmap.enableLOS", false);
    bool enableHeight = sConfig.GetBoolDefault("vmap.enableHeight", false);
    bool disableModelUnload = sConfig.GetBoolDefault("Collision.Models.Unload", false);
    std::string ignoreSpellIds = sConfig.GetStringDefault("vmap.ignoreSpellIds", "");

    if (!enableHeight)
        sLog.outError("VMAP height use disabled! Creatures movements and other things will be in broken state.");

    VMAP::VMapFactory::createOrGetVMapManager()->setEnableLineOfSightCalc(enableLOS);
    VMAP::VMapFactory::createOrGetVMapManager()->setEnableHeightCalc(enableHeight);
    VMAP::VMapFactory::createOrGetVMapManager()->setUseManagedPtrs(!disableModelUnload);
    VMAP::VMapFactory::preventSpellsFromBeingTestedForLoS(ignoreSpellIds.c_str());
    sLog.outString("VMap data directory: %svmaps.", m_dataPath.c_str());
    sLog.outString("VMap support included. LineOfSight: %i | getHeight: %i | indoorCheck: %i.", enableLOS, enableHeight, getConfig(CONFIG_BOOL_VMAP_INDOOR_CHECK) ? 1 : 0);
    sLog.outString("MMap pathfinding %sabled.", getConfig(CONFIG_BOOL_MMAP_ENABLED) ? "en" : "dis");

    sLog.outString("Anticrash: 0x%x rearm after %u seconds.", getConfig(CONFIG_UINT32_ANTICRASH_OPTIONS), getConfig(CONFIG_UINT32_ANTICRASH_REARM_TIMER) / 1000);
    sLog.outString("Pathfinding: [%s]", getConfig(CONFIG_BOOL_MMAP_ENABLED) ? "Enabled" : "Disabled");

    // Update packet broadcaster config
    if (reload) {
        sWorld.m_broadcaster->UpdateConfiguration(getConfig(CONFIG_UINT32_PACKET_BCAST_THREADS),
            std::chrono::milliseconds(getConfig(CONFIG_UINT32_PACKET_BCAST_FREQUENCY)));
    }

    m_wardenModuleDirectory = sConfig.GetStringDefault("Warden.ModuleDir", "warden_modules");

    // Smartlog data
    sLog.InitSmartlogEntries(sConfig.GetStringDefault("Smartlog.ExtraEntries", ""));
    sLog.InitSmartlogGuids(sConfig.GetStringDefault("Smartlog.ExtraGuids", ""));

    if (getConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM) < 0)
    {
        sLog.outError("WORLD: CONFIG_UINT32_TRANSMOG_REQ_ITEM has wrong value (%u). Must be > 0. Set to 0.", getConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM));
        setConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM, 0);
    }

    if (getConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM_COUNT) <= 0 || getConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM_COUNT) > 200)
    {
        sLog.outError("WORLD: CONFIG_UINT32_TRANSMOG_REQ_ITEM_COUNT has wrong value (%u). Must be 1 - 200. Set to 1.", getConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM_COUNT));
        setConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM_COUNT, 1);
    }

    // If max bg queues is at 0, decide based on patch.
    if (getConfig(CONFIG_UINT32_BATTLEGROUND_QUEUES_COUNT) == 0)
        setConfig(CONFIG_UINT32_BATTLEGROUND_QUEUES_COUNT, 3);

    /*if (getConfig(CONFIG_BOOL_SEA_NETWORK))
    {
        sLog.outBasic("Overriding XP rates for chinese realm.");
        switch (getConfig(CONFIG_UINT32_GAME_TYPE))
        {
            case REALM_TYPE_PVP:
            case REALM_TYPE_RPPVP:
            {
                setConfig(CONFIG_FLOAT_RATE_XP_KILL, 1.6f);
                setConfig(CONFIG_FLOAT_RATE_XP_KILL_ELITE, 1.6f);
                setConfig(CONFIG_FLOAT_RATE_XP_QUEST, 1.6f);
                setConfig(CONFIG_FLOAT_RATE_XP_EXPLORE, 1.6f);
                break;
            }
            default:
            {
                setConfig(CONFIG_FLOAT_RATE_XP_KILL, 1.4f);
                setConfig(CONFIG_FLOAT_RATE_XP_KILL_ELITE, 1.4f);
                setConfig(CONFIG_FLOAT_RATE_XP_QUEST, 1.4f);
                setConfig(CONFIG_FLOAT_RATE_XP_EXPLORE, 1.4f);
                break;
            }
        }
    }*/

    if (getConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS) != 0 && getConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS) < 2)
    {
        sLog.outError("MaxOverspeedPings (%i) must be in range 2..infinity (or 0 to disable check). Set to 2.", getConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS));
        setConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS, 2);
    }

    if (getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHAT))
        setConfig(CONFIG_BOOL_GM_JOIN_OPPOSITE_FACTION_CHANNELS, false);

    if (reload)
        sMapMgr.SetGridCleanUpDelay(getConfig(CONFIG_UINT32_INTERVAL_GRIDCLEAN));

    if (reload)
        sMapMgr.SetMapUpdateInterval(getConfig(CONFIG_UINT32_INTERVAL_MAPUPDATE));

    m_autoPDumpDirectory = sConfig.GetStringDefault("PDumpDir", "pdump");

    m_minChatLevel = getConfig(CONFIG_UINT32_CHAT_MIN_LEVEL);

    m_timers[WUPDATE_CENSUS].SetInterval(60 * MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_SHELLCOIN].SetInterval(10 * MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_TOTAL_MONEY].SetInterval(6 * HOUR * IN_MILLISECONDS);
    m_timers[WUPDATE_COMMANDS].SetInterval(1 * MINUTE * IN_MILLISECONDS);

    g_bEnableStatGather = getConfig(CONFIG_BOOL_PERFORMANCE_ENABLE);

    auto pathString = sConfig.GetStringDefault("Database.AutoUpdate.Path", "");
    auto worldUpdateFolder = sConfig.GetStringDefault("Database.AutoUpdate.WorldUpdateName", "World");
    std::filesystem::path folderPath{ pathString };
    std::filesystem::directory_entry worldUpdatePath{ folderPath / worldUpdateFolder };
    m_worldUpdatesDirectory = fs::absolute(worldUpdatePath.path()).string();
    time_t curr;
    tm local;
    time(&curr);                                        // get current time_t value
    local = *(localtime(&curr));                        // dereference and assign
    char fName[128];
    sprintf(fName, "%04d%02d%02d%02d%02d%02d_world.sql", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
    m_worldUpdatesMigration = m_worldUpdatesDirectory + "/" + fName;
}

void World::LoadConfigSettings(bool reload)
{
    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_BEFORE_CONFIG_LOAD, [&](WorldScript* script)
    {
        script->OnBeforeConfigLoad(reload);
    });

    if (reload)
    {
        if (!sConfig.Reload())
        {
            sLog.outError("World settings reload fail: can't read settings from %s.", sConfig.GetFilename().c_str());
            return;
        }

        if (!sConfig.LoadModulesConfigs())
        {
            sLog.outError("World settings reload fail: can't read module settings for %s.", sConfig.GetFilename().c_str());
            return;
        }
    }

    ///- Read the version of the configuration file and warn the user in case of emptiness or mismatch
    uint32 confVersion = sConfig.GetIntDefault("ConfVersion", 0);
    if (!confVersion)
    {
        sLog.outError("*****************************************************************************");
        sLog.outError(" WARNING: mangosd.conf does not include a ConfVersion variable.");
        sLog.outError("          Your configuration file may be out of date!");
        sLog.outError("*****************************************************************************");
        Log::WaitBeforeContinueIfNeed();
    }
    else
    {
        if (confVersion < _MANGOSDCONFVERSION)
        {
            sLog.outError("*****************************************************************************");
            sLog.outError(" WARNING: Your mangosd.conf version indicates your conf file is out of date!");
            sLog.outError("          Please check for updates, as your current default values may cause");
            sLog.outError("          unexpected behavior.");
            sLog.outError("*****************************************************************************");
            Log::WaitBeforeContinueIfNeed();
        }
    }

    // Always load configuration directly from file; skip database overrides/exports.
    LoadConfigSettingsFromFile();

    LoadConfigSettingsCommonPart(reload);

    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_AFTER_CONFIG_LOAD, [&](WorldScript* script)
    {
        script->OnAfterConfigLoad(reload);
    });
}

bool World::LoadConfigSettingsFromDB(bool reload)
{
    std::unique_ptr<QueryResult> result(LoginDatabase.PQuery("SELECT * FROM `world_config` WHERE `realm_id`=%u", realmID));
    if (!result)
    {
#ifdef WIN32
        sLog.outError("No data in `world_config` table! Press enter to load from file.");
        getchar();
#endif
        return false;
    }

    if (result->GetFieldCount() != (1 + CONFIG_UINT32_VALUE_COUNT + CONFIG_INT32_VALUE_COUNT + CONFIG_FLOAT_VALUE_COUNT + CONFIG_BOOL_VALUE_COUNT))
    {
#ifdef WIN32
        sLog.outError("Wrong structure of `world_config` table! Press enter to load from file.");
        getchar();
#endif
        return false;
    }

    do
    {
        auto fields = result->Fetch();
        uint32 realmId = fields[0].GetUInt32();

        uint32 offset = 1;

        for (uint32 i = 0; i < CONFIG_UINT32_VALUE_COUNT; ++i)
            m_configUint32Values[i] = fields[offset + i].GetUInt32();
        offset += CONFIG_UINT32_VALUE_COUNT;

        for (uint32 i = 0; i < CONFIG_INT32_VALUE_COUNT; ++i)
            m_configInt32Values[i] = fields[offset + i].GetInt32();
        offset += CONFIG_INT32_VALUE_COUNT;

        for (uint32 i = 0; i < CONFIG_FLOAT_VALUE_COUNT; ++i)
            m_configFloatValues[i] = fields[offset + i].GetFloat();
        offset += CONFIG_FLOAT_VALUE_COUNT;

        for (uint32 i = 0; i < CONFIG_BOOL_VALUE_COUNT; ++i)
            m_configBoolValues[i] = fields[offset + i].GetBool();
        offset += CONFIG_BOOL_VALUE_COUNT;

    } while (result->NextRow());

    return true;
}

/// Initialize config values
void World::LoadConfigSettingsFromFile(bool reload)
{
    ///- Read all rates from the config file
    setConfigPos(CONFIG_FLOAT_RATE_HEALTH,               "Rate.Health", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_MANA,           "Rate.Mana", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_POWER_RAGE_INCOME,       "Rate.Rage.Income", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_POWER_RAGE_LOSS,      "Rate.Rage.Loss", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_POWER_FOCUS,             "Rate.Focus", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_LOYALTY,              "Rate.Loyalty", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_POWER_ENERGY,            "Rate.Energy", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_SKILL_DISCOVERY,      "Rate.Skill.Discovery",      1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_POOR,       "Rate.Drop.Item.Poor",       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_NORMAL,     "Rate.Drop.Item.Normal",     1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_UNCOMMON,   "Rate.Drop.Item.Uncommon",   1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_RARE,       "Rate.Drop.Item.Rare",       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_EPIC,       "Rate.Drop.Item.Epic",       1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_LEGENDARY,  "Rate.Drop.Item.Legendary",  1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_ARTIFACT,   "Rate.Drop.Item.Artifact",   1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_ITEM_REFERENCED, "Rate.Drop.Item.Referenced", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DROP_MONEY,           "Rate.Drop.Money", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_XP_KILL,                 "Rate.XP.Kill",    1.0f);
    setConfig(CONFIG_FLOAT_RATE_XP_KILL_ELITE,		     "Rate.XP.Kill.Elite",    1.0f);
    setConfig(CONFIG_FLOAT_RATE_XP_QUEST,                "Rate.XP.Quest",   1.0f);
    setConfig(CONFIG_FLOAT_RATE_XP_EXPLORE,              "Rate.XP.Explore", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REPUTATION_GAIN,           "Rate.Reputation.Gain", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REPUTATION_LOWLEVEL_KILL,  "Rate.Reputation.LowLevel.Kill", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REPUTATION_LOWLEVEL_QUEST, "Rate.Reputation.LowLevel.Quest", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_NORMAL_DAMAGE,               "Rate.Creature.Normal.Damage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_DAMAGE,          "Rate.Creature.Elite.Elite.Damage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_DAMAGE,      "Rate.Creature.Elite.RAREELITE.Damage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE,      "Rate.Creature.Elite.WORLDBOSS.Damage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_DAMAGE,           "Rate.Creature.Elite.RARE.Damage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_NORMAL_HP,                   "Rate.Creature.Normal.HP", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_HP,              "Rate.Creature.Elite.Elite.HP", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_HP,          "Rate.Creature.Elite.RAREELITE.HP", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_HP,          "Rate.Creature.Elite.WORLDBOSS.HP", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_HP,               "Rate.Creature.Elite.RARE.HP", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_NORMAL_SPELLDAMAGE,          "Rate.Creature.Normal.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE,     "Rate.Creature.Elite.Elite.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE, "Rate.Creature.Elite.RAREELITE.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE, "Rate.Creature.Elite.WORLDBOSS.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_SPELLDAMAGE,      "Rate.Creature.Elite.RARE.SpellDamage", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CREATURE_AGGRO,              "Rate.Creature.Aggro", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REST_INGAME,                    "Rate.Rest.InGame", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REST_OFFLINE_IN_TAVERN_OR_CITY, "Rate.Rest.Offline.InTavernOrCity", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_REST_OFFLINE_IN_WILDERNESS,     "Rate.Rest.Offline.InWilderness", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_DAMAGE_FALL,                 "Rate.Damage.Fall", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_AUCTION_TIME,                "Rate.Auction.Time", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_AUCTION_DEPOSIT,                "Rate.Auction.Deposit", 1.0f);
    setConfig(CONFIG_FLOAT_RATE_AUCTION_CUT,                    "Rate.Auction.Cut", 1.0f);
    setConfigPos(CONFIG_UINT32_AUCTION_DEPOSIT_MIN,             "Auction.Deposit.Min", 0);
    setConfig(CONFIG_UINT32_ACCOUNT_CONCURRENT_AUCTION_LIMIT,   "Auction.AccountConcurrentLimit", 0);
    setConfig(CONFIG_FLOAT_RATE_WAR_EFFORT_RESOURCE,            "Rate.WarEffortResourceComplete", 0.0f);
    setConfig(CONFIG_UINT32_WAR_EFFORT_AUTOCOMPLETE_PERIOD,     "WarEffortResourceCompletePeriod", 86400);
    setConfig(CONFIG_FLOAT_RATE_HONOR,                  "Rate.Honor", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_MINING_AMOUNT,       "Rate.Mining.Amount", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_MINING_NEXT,         "Rate.Mining.Next", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_INSTANCE_RESET_TIME, "Rate.InstanceResetTime", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_TALENT,              "Rate.Talent", 1.0f);
    setConfigPos(CONFIG_FLOAT_RATE_CORPSE_DECAY_LOOTED, "Rate.Corpse.Decay.Looted", 0.0f);
    setConfig(CONFIG_UINT32_RESPEC_BASE_COST,           "Rate.RespecBaseCost", 1);
    setConfig(CONFIG_UINT32_RESPEC_MULTIPLICATIVE_COST, "Rate.RespecMultiplicativeCost", 5);
    setConfig(CONFIG_UINT32_RESPEC_MIN_MULTIPLIER,      "Rate.RespecMinMultiplier", 2);
    setConfig(CONFIG_UINT32_RESPEC_MAX_MULTIPLIER,      "Rate.RespecMaxMultiplier", 10);
    setConfig(CONFIG_UINT32_BARBERSHOP_COST_BASE,       "Barbershop.Cost.Base", 5);
    setConfig(CONFIG_UINT32_BARBERSHOP_COST_STYLE,      "Barbershop.Cost.Style", 100);
    setConfig(CONFIG_UINT32_BARBERSHOP_COST_COLOR,      "Barbershop.Cost.Color", 50);
    setConfig(CONFIG_UINT32_BARBERSHOP_COST_FACIAL_HAIR, "Barbershop.Cost.FacialHair", 75);

    setConfigMinMax(CONFIG_FLOAT_RATE_TARGET_POS_RECALCULATION_RANGE, "TargetPosRecalculateRange", 1.5f, CONTACT_DISTANCE, ATTACK_DISTANCE);

    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_DAMAGE, "DurabilityLossChance.Damage", 0.5f);
    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_ABSORB, "DurabilityLossChance.Absorb", 0.5f);
    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_PARRY,  "DurabilityLossChance.Parry",  0.05f);
    setConfigPos(CONFIG_FLOAT_RATE_DURABILITY_LOSS_BLOCK,  "DurabilityLossChance.Block",  0.05f);

    setConfigPos(CONFIG_FLOAT_LISTEN_RANGE_SAY,       "ListenRange.Say",       25.0f);
    setConfigPos(CONFIG_FLOAT_LISTEN_RANGE_YELL,      "ListenRange.Yell",     300.0f);
    setConfigPos(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE, "ListenRange.TextEmote", 25.0f);

    setConfigPos(CONFIG_FLOAT_GROUP_XP_DISTANCE, "MaxGroupXPDistance", 74.0f);
    setConfigPos(CONFIG_UINT32_PASSWORD_CHANGE_REWARD_ITEM, "PasswordChangeRewardItem", 13342);

    setConfigPos(CONFIG_FLOAT_CREATURE_FAMILY_ASSISTANCE_RADIUS,      "CreatureFamilyAssistanceRadius",     10.0f);
    setConfigPos(CONFIG_FLOAT_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS, "CreatureFamilyFleeAssistanceRadius", 30.0f);
    setConfig(CONFIG_FLOAT_THREAT_RADIUS, "ThreatRadius", 50.0f);
    setConfig(CONFIG_FLOAT_MAX_CREATURE_ATTACK_RADIUS, "MaxCreaturesAttackRadius", 40.0f);
    setConfig(CONFIG_FLOAT_MAX_PLAYERS_STEALTH_DETECT_RANGE, "MaxPlayersStealthDetectRange", 40.0f);

    ///- Read other configuration items from the config file
    setConfig(CONFIG_UINT32_LOGIN_PER_TICK, "LoginPerTick", 0);
    setConfig(CONFIG_UINT32_PLAYER_HARD_LIMIT, "PlayerHardLimit", 0);
    //setConfig(CONFIG_BOOL_LOGIN_REGION_QUEUE, "LoginRegionQueue", false);
    //setConfig(CONFIG_UINT32_LOGIN_REGION_QUEUE_LEVEL_THRESHOLD, "LoginVIPQueueLevelThreshold", 55);
    setConfig(CONFIG_UINT32_LOGIN_QUEUE_GRACE_PERIOD_SECS, "LoginQueue.GracePeriodSecs", 0);
    setConfig(CONFIG_UINT32_CHARACTER_SCREEN_MAX_IDLE_TIME, "CharacterScreenMaxIdleTime", 0);
    setConfig(CONFIG_UINT32_ASYNC_QUERIES_TICK_TIMEOUT, "AsyncQueriesTickTimeout", 0);
    setConfigMinMax(CONFIG_UINT32_COMPRESSION, "Compression", 1, 1, 9);
    setConfig(CONFIG_BOOL_ADDON_CHANNEL, "AddonChannel", true);
    setConfig(CONFIG_BOOL_CLEAN_CHARACTER_DB, "CleanCharacterDB", true);
    setConfig(CONFIG_BOOL_GRID_UNLOAD, "GridUnload", true);
    setConfig(CONFIG_BOOL_CLEANUP_TERRAIN, "CleanupTerrain", true);
    setConfigPos(CONFIG_UINT32_INTERVAL_SAVE, "PlayerSave.Interval", 15 * MINUTE * IN_MILLISECONDS);
    setConfigMinMax(CONFIG_UINT32_MIN_LEVEL_STAT_SAVE, "PlayerSave.Stats.MinLevel", 0, 0, MAX_LEVEL);
    setConfig(CONFIG_BOOL_STATS_SAVE_ONLY_ON_LOGOUT, "PlayerSave.Stats.SaveOnlyOnLogout", true);

    setConfigMin(CONFIG_UINT32_INTERVAL_GRIDCLEAN, "GridCleanUpDelay", 5 * MINUTE * IN_MILLISECONDS, MIN_GRID_DELAY);
    setConfigMin(CONFIG_UINT32_INTERVAL_MAPUPDATE, "MapUpdateInterval", 100, MIN_MAP_UPDATE_DELAY);
    setConfig(CONFIG_UINT32_INTERVAL_CHANGEWEATHER, "ChangeWeatherInterval", 10 * MINUTE * IN_MILLISECONDS);

    if (configNoReload(reload, CONFIG_UINT32_PORT_WORLD, "WorldServerPort", DEFAULT_WORLDSERVER_PORT))
        setConfig(CONFIG_UINT32_PORT_WORLD, "WorldServerPort", DEFAULT_WORLDSERVER_PORT);

    if (configNoReload(reload, CONFIG_UINT32_GAME_TYPE, "GameType", 0))
        setConfig(CONFIG_UINT32_GAME_TYPE, "GameType", 0);

    if (configNoReload(reload, CONFIG_UINT32_REALM_ZONE, "RealmZone", REALM_ZONE_DEVELOPMENT))
        setConfig(CONFIG_UINT32_REALM_ZONE, "RealmZone", REALM_ZONE_DEVELOPMENT);

    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_ACCOUNTS,            "AllowTwoSide.Accounts", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHAT,    "AllowTwoSide.Interaction.Chat", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHANNEL, "AllowTwoSide.Interaction.Channel", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP,   "AllowTwoSide.Interaction.Group", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GUILD,   "AllowTwoSide.Interaction.Guild", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_TRADE,   "AllowTwoSide.Interaction.Trade", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_AUCTION, "AllowTwoSide.Interaction.Auction", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_MAIL,    "AllowTwoSide.Interaction.Mail", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_WHO_LIST,            "AllowTwoSide.WhoList", false);
    setConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_ADD_FRIEND,          "AllowTwoSide.AddFriend", false);

    setConfig(CONFIG_FLOAT_MAX_FACTION_IMBALANCE, "MaxFactionImbalance", 0.1f);

    setConfig(CONFIG_UINT32_STRICT_PLAYER_NAMES,  "StrictPlayerNames",  0);
    setConfig(CONFIG_UINT32_STRICT_CHARTER_NAMES, "StrictCharterNames", 0);
    setConfig(CONFIG_UINT32_STRICT_PET_NAMES,     "StrictPetNames",     0);

    setConfigMinMax(CONFIG_UINT32_MIN_PLAYER_NAME,  "MinPlayerName",  2, 1, MAX_PLAYER_NAME);
    setConfigMinMax(CONFIG_UINT32_MIN_CHARTER_NAME, "MinCharterName", 2, 1, MAX_CHARTER_NAME);
    setConfigMinMax(CONFIG_UINT32_MIN_PET_NAME,     "MinPetName",     2, 1, MAX_PET_NAME);

    setConfig(CONFIG_UINT32_CHARACTERS_CREATING_DISABLED, "CharactersCreatingDisabled", 0);
    setConfigMinMax(CONFIG_UINT32_CHARACTERS_PER_REALM, "CharactersPerRealm", 10, 1, 10);
    setConfigMin(CONFIG_UINT32_CHARACTERS_PER_ACCOUNT, "CharactersPerAccount", 50, getConfig(CONFIG_UINT32_CHARACTERS_PER_REALM));

    setConfigMinMax(CONFIG_UINT32_SKIP_CINEMATICS, "SkipCinematics", 0, 0, 2);
    setConfig(CONFIG_BOOL_OBJECT_HEALTH_VALUE_SHOW, "ShowHealthValues", false);
    if (configNoReload(reload, CONFIG_UINT32_MAX_PLAYER_LEVEL, "MaxPlayerLevel", PLAYER_MAX_LEVEL))
        setConfigMinMax(CONFIG_UINT32_MAX_PLAYER_LEVEL, "MaxPlayerLevel", PLAYER_MAX_LEVEL, 1, PLAYER_STRONG_MAX_LEVEL);
    setConfigMinMax(CONFIG_UINT32_START_PLAYER_LEVEL, "StartPlayerLevel", 1, 1, getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));
    setConfigMinMax(CONFIG_UINT32_START_PLAYER_MONEY, "StartPlayerMoney", 0, 0, MAX_MONEY_AMOUNT);
    setConfigPos(CONFIG_UINT32_MAX_HONOR_POINTS, "MaxHonorPoints", 75000);
    setConfigMinMax(CONFIG_UINT32_START_HONOR_POINTS, "StartHonorPoints", 0, 0, getConfig(CONFIG_UINT32_MAX_HONOR_POINTS));
    setConfigMin(CONFIG_UINT32_MIN_HONOR_KILLS, "MinHonorKills", MIN_HONOR_KILLS, 1);
    setConfig(CONFIG_UINT32_WEEKLY_HONOR_CAP, "WeeklyHonorCap", 20000);
    setConfigMinMax(CONFIG_UINT32_MAINTENANCE_DAY, "MaintenanceDay", 4, 0, 6);
    setConfig(CONFIG_BOOL_AUTO_HONOR_RESTART, "AutoHonorRestart", true);
    setConfig(CONFIG_BOOL_ALL_TAXI_PATHS, "AllFlightPaths", false);
    setConfig(CONFIG_BOOL_CAST_UNSTUCK, "CastUnstuck", true);
    setConfig(CONFIG_UINT32_MAX_SPELL_CASTS_IN_CHAIN, "MaxSpellCastsInChain", 10);

    setConfig(CONFIG_BOOL_INSTANCE_IGNORE_LEVEL, "Instance.IgnoreLevel", false);
    setConfig(CONFIG_BOOL_INSTANCE_IGNORE_RAID,  "Instance.IgnoreRaid", false);
    setConfig(CONFIG_UINT32_INSTANCE_RESET_TIME_HOUR, "Instance.ResetTimeHour", 4);
    setConfig(CONFIG_UINT32_INSTANCE_UNLOAD_DELAY,    "Instance.UnloadDelay", 30 * MINUTE * IN_MILLISECONDS);

    setConfig(CONFIG_UINT32_MAX_PRIMARY_TRADE_SKILL, "MaxPrimaryTradeSkill", 2);
    setConfigMinMax(CONFIG_UINT32_MIN_PETITION_SIGNS, "MinPetitionSigns", 9, 0, 9);

    setConfig(CONFIG_UINT32_GM_LOGIN_STATE,       "GM.LoginState",    2);
    setConfig(CONFIG_UINT32_GM_VISIBLE_STATE,     "GM.Visible",       2);
    setConfig(CONFIG_UINT32_GM_ACCEPT_TICKETS,    "GM.AcceptTickets", 2);
    setConfig(CONFIG_UINT32_GM_CHAT,              "GM.Chat",          2);
    setConfig(CONFIG_UINT32_GM_WISPERING_TO,      "GM.WhisperingTo",  2);
    setConfig(CONFIG_UINT32_GM_LEVEL_IN_GM_LIST,  "GM.InGMList.Level",  SEC_ADMINISTRATOR);
    setConfig(CONFIG_UINT32_GM_LEVEL_IN_WHO_LIST, "GM.InWhoList.Level", SEC_ADMINISTRATOR);
    setConfig(CONFIG_BOOL_GM_LOG_TRADE,           "GM.LogTrade", false);
    setConfigMinMax(CONFIG_UINT32_START_GM_LEVEL, "GM.StartLevel", 1, getConfig(CONFIG_UINT32_START_PLAYER_LEVEL), MAX_LEVEL);
    setConfig(CONFIG_BOOL_DIE_COMMAND_CREDIT,     "GM.CreditOnDie", true);
    setConfig(CONFIG_BOOL_GM_LOWER_SECURITY,      "GM.LowerSecurity", false);
    setConfig(CONFIG_BOOL_GM_ALLOW_TRADES,        "GM.AllowTrades", true);
    setConfig(CONFIG_BOOL_GMS_ALLOW_PUBLIC_CHANNELS,         "GM.AllowPublicChannels", false);
    setConfig(CONFIG_BOOL_GM_JOIN_OPPOSITE_FACTION_CHANNELS, "GM.JoinOppositeFactionChannels", false);
    setConfig(CONFIG_BOOL_GM_START_ON_GM_ISLAND, "GM.StartOnGMIsland", true);
    setConfig(CONFIG_BOOL_GMTICKETS_ENABLE,           "GMTickets.Enable", true);
    setConfig(CONFIG_UINT32_GMTICKETS_MINLEVEL,       "GMTickets.MinLevel", 0);
    setConfig(CONFIG_UINT32_GMTICKETS_ADMIN_SECURITY, "GMTickets.Admin.Security", SEC_CONSOLE);

    setConfig(CONFIG_UINT32_GROUP_VISIBILITY, "Visibility.GroupMode", 0);

    setConfig(CONFIG_UINT32_MAIL_DELIVERY_DELAY, "MailDeliveryDelay", HOUR);
    setConfig(CONFIG_UINT32_MAIL_MAX_PER_HOUR, "MailMaxPerHour", 3);

    setConfigMin(CONFIG_UINT32_MASS_MAILER_SEND_PER_TICK, "MassMailer.SendPerTick", 10, 1);

    setConfig(CONFIG_UINT32_BANLIST_RELOAD_TIMER, "BanListReloadTimer", 60);
    setConfigPos(CONFIG_UINT32_UPTIME_UPDATE, "UpdateUptimeInterval", 10);

    setConfig(CONFIG_UINT32_SKILL_CHANCE_ORANGE, "SkillChance.Orange", 100);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_YELLOW, "SkillChance.Yellow", 75);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_GREEN,  "SkillChance.Green",  25);
    setConfig(CONFIG_UINT32_SKILL_CHANCE_GREY,   "SkillChance.Grey",   0);

    setConfigPos(CONFIG_UINT32_SKILL_CHANCE_MINING_STEPS,   "SkillChance.MiningSteps",   75);
    setConfigPos(CONFIG_UINT32_SKILL_CHANCE_SKINNING_STEPS, "SkillChance.SkinningSteps", 75);

    setConfigPos(CONFIG_UINT32_SKILL_GAIN_CRAFTING,  "SkillGain.Crafting",  1);
    setConfigPos(CONFIG_UINT32_SKILL_GAIN_DEFENSE,   "SkillGain.Defense",   1);
    setConfigPos(CONFIG_UINT32_SKILL_GAIN_GATHERING, "SkillGain.Gathering", 1);
    setConfig(CONFIG_UINT32_SKILL_GAIN_WEAPON,       "SkillGain.Weapon",    1);

    setConfig(CONFIG_BOOL_SKILL_FAIL_LOOT_FISHING,         "SkillFail.Loot.Fishing", false);
    setConfig(CONFIG_BOOL_SKILL_FAIL_GAIN_FISHING,         "SkillFail.Gain.Fishing", false);
    setConfig(CONFIG_BOOL_SKILL_FAIL_POSSIBLE_FISHINGPOOL, "SkillFail.Possible.FishingPool", true);

    setConfig(CONFIG_UINT32_MAX_OVERSPEED_PINGS, "MaxOverspeedPings", 2);

    setConfig(CONFIG_UINT32_AUTO_RESTART_MAX_SERVER_UPTIME, "AutoRestart.MaxServerUptime", 0);
    setConfig(CONFIG_UINT32_AUTO_RESTART_HOUR_MIN, "AutoRestart.HourMin", 0);
    setConfig(CONFIG_UINT32_AUTO_RESTART_HOUR_MAX, "AutoRestart.HourMax", 0);

    setConfig(CONFIG_UINT32_DIFF_HC_PROTECTION, "Hardcore.DiffProtection", 1100);

    setConfig(CONFIG_BOOL_SAVE_RESPAWN_TIME_IMMEDIATELY, "SaveRespawnTimeImmediately", true);
    setConfig(CONFIG_BOOL_WEATHER, "ActivateWeather", true);

    setConfig(CONFIG_BOOL_ALWAYS_MAX_SKILL_FOR_LEVEL, "AlwaysMaxSkillForLevel", false);

    setConfig(CONFIG_UINT32_CHATFLOOD_MESSAGE_COUNT, "ChatFlood.MessageCount", 10);
    setConfig(CONFIG_UINT32_CHATFLOOD_MESSAGE_DELAY, "ChatFlood.MessageDelay", 1);
    setConfig(CONFIG_UINT32_CHATFLOOD_MUTE_TIME,     "ChatFlood.MuteTime", 10);

    setConfig(CONFIG_BOOL_EVENT_ANNOUNCE, "Event.Announce", false);

    setConfig(CONFIG_UINT32_CREATURE_FAMILY_ASSISTANCE_DELAY, "CreatureFamilyAssistanceDelay", 1500);
    setConfig(CONFIG_UINT32_CREATURE_FAMILY_FLEE_DELAY,       "CreatureFamilyFleeDelay",       7000);

    setConfig(CONFIG_UINT32_WORLD_BOSS_LEVEL_DIFF, "WorldBossLevelDiff", 3);

    setConfig(CONFIG_INT32_QUEST_LOW_LEVEL_HIDE_DIFF, "Quests.LowLevelHideDiff", 4);
    setConfig(CONFIG_INT32_QUEST_HIGH_LEVEL_HIDE_DIFF, "Quests.HighLevelHideDiff", 7);

    setConfig(CONFIG_BOOL_QUEST_IGNORE_RAID, "Quests.IgnoreRaid", false);

    setConfig(CONFIG_BOOL_DETECT_POS_COLLISION, "DetectPosCollision", true);

    setConfig(CONFIG_BOOL_RESTRICTED_LFG_CHANNEL,      "Channel.RestrictedLfg", true);
    setConfig(CONFIG_BOOL_SILENTLY_GM_JOIN_TO_CHANNEL, "Channel.SilentlyGMJoin", false);
    setConfig(CONFIG_BOOL_STRICT_LATIN_IN_GENERAL_CHANNELS, "Channel.StrictLatinInGeneral", false);

    setConfig(CONFIG_BOOL_CHAT_FAKE_MESSAGE_PREVENTING, "ChatFakeMessagePreventing", true);

    setConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_SEVERITY, "ChatStrictLinkChecking.Severity", 2);
    setConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_KICK,     "ChatStrictLinkChecking.Kick", 0);

    setConfig(CONFIG_BOOL_SEND_LOOT_ROLL_UPON_RECONNECT, "SendLootRollUponReconnect", false);

    setConfig(CONFIG_UINT32_BONES_EXPIRE_MINUTES,      "Bones.ExpireMinutes", 60);
    setConfig(CONFIG_UINT32_CORPSES_UPDATE_MINUTES,    "Corpses.UpdateMinutes", 20);
    setConfig(CONFIG_BOOL_CORPSE_EMPTY_LOOT_SHOW,      "Corpse.EmptyLootShow", true);
    setConfigPos(CONFIG_UINT32_CORPSE_DECAY_NORMAL,    "Corpse.Decay.NORMAL",    300);
    setConfigPos(CONFIG_UINT32_CORPSE_DECAY_RARE,      "Corpse.Decay.RARE",      900);
    setConfigPos(CONFIG_UINT32_CORPSE_DECAY_ELITE,     "Corpse.Decay.ELITE",     600);
    setConfigPos(CONFIG_UINT32_CORPSE_DECAY_RAREELITE, "Corpse.Decay.RAREELITE", 1200);
    setConfigPos(CONFIG_UINT32_CORPSE_DECAY_WORLDBOSS, "Corpse.Decay.WORLDBOSS", 3600);

    setConfig(CONFIG_INT32_DEATH_SICKNESS_LEVEL,          "Death.SicknessLevel", 11);
    setConfig(CONFIG_BOOL_DEATH_CORPSE_RECLAIM_DELAY_PVP, "Death.CorpseReclaimDelay.PvP", true);
    setConfig(CONFIG_BOOL_DEATH_CORPSE_RECLAIM_DELAY_PVE, "Death.CorpseReclaimDelay.PvE", true);
    setConfig(CONFIG_BOOL_DEATH_BONES_WORLD,              "Death.Bones.World", true);
    setConfig(CONFIG_BOOL_DEATH_BONES_BG,                 "Death.Bones.Battleground", true);
    setConfigMinMax(CONFIG_FLOAT_GHOST_RUN_SPEED_WORLD,   "Death.Ghost.RunSpeed.World", 1.0f, 0.1f, 10.0f);
    setConfigMinMax(CONFIG_FLOAT_GHOST_RUN_SPEED_BG,      "Death.Ghost.RunSpeed.Battleground", 1.0f, 0.1f, 10.0f);

    setConfig(CONFIG_UINT32_AV_MIN_PLAYERS_IN_QUEUE, "Alterac.MinPlayersInQueue", 0);
    setConfig(CONFIG_UINT32_AV_INITIAL_MAX_PLAYERS,  "Alterac.InitMaxPlayers", 0);
    setConfig(CONFIG_BOOL_BATTLEGROUND_CAST_DESERTER,                  "Battleground.CastDeserter", true);
    setConfigMinMax(CONFIG_UINT32_BATTLEGROUND_QUEUE_ANNOUNCER_JOIN,   "Battleground.QueueAnnouncer.Join", 0, 0, 2);
    setConfig(CONFIG_BOOL_BATTLEGROUND_QUEUE_ANNOUNCER_START,          "Battleground.QueueAnnouncer.Start", false);
    setConfig(CONFIG_UINT32_BATTLEGROUND_INVITATION_TYPE,              "Battleground.InvitationType", 0);
    setConfig(CONFIG_UINT32_BATTLEGROUND_PREMATURE_FINISH_TIMER,       "BattleGround.PrematureFinishTimer", 5 * MINUTE * IN_MILLISECONDS);
    setConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH, "BattleGround.PremadeGroupWaitForMatch", 0);
    setConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_QUEUE_GROUP_MIN_SIZE, "BattleGround.PremadeQueue.MinGroupSize", 6);
    setConfig(CONFIG_BOOL_BATTLEGROUND_RANDOMIZE,                      "BattleGround.RandomizeQueues", false);
    setConfig(CONFIG_UINT32_BATTLEGROUND_GROUP_LIMIT,                  "BattleGround.GroupQueueLimit", 40);
    setConfig(CONFIG_BOOL_TAG_IN_BATTLEGROUNDS,                        "BattleGround.TagInBattleGrounds", true);
    setConfigMinMax(CONFIG_UINT32_BATTLEGROUND_QUEUES_COUNT,           "BattleGround.QueuesCount", 0, 0, 3);
    setConfig(CONFIG_BOOL_ENABLE_CROSSFACTION_BATTLEGROUNDS,           "BattleGround.Crossfaction", false);
    setConfig(CONFIG_BOOL_ENABLE_GEAR_RATING_QUEUE,                    "BattleGround.GearQueue", false);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_REPUTATION_RATE_AV,            "BattleGround.Rate.Reputation.AV", 1);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_REPUTATION_RATE_WS,            "BattleGround.Rate.Reputation.WS", 1);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_REPUTATION_RATE_AB,            "BattleGround.Rate.Reputation.AB", 1);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_REPUTATION_RATE_SV,            "BattleGround.Rate.Reputation.SV", 1);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_HONOR_RATE_AV,                 "BattleGround.Rate.Honor.AV", 0.75f);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_HONOR_RATE_WS,                 "BattleGround.Rate.Honor.WS", 1.25f);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_HONOR_RATE_AB,                 "BattleGround.Rate.Honor.AB", 1.25f);
    setConfig(CONFIG_FLOAT_BATTLEGROUND_HONOR_RATE_SV,                 "BattleGround.Rate.Honor.SV", 1.25f);

    setConfig(CONFIG_BOOL_OUTDOORPVP_EP_ENABLE, "OutdoorPvP.EP.Enable", true);
    setConfig(CONFIG_BOOL_OUTDOORPVP_SI_ENABLE, "OutdoorPvP.SI.Enable", true);

    setConfig(CONFIG_BOOL_PLAYER_COMMANDS, "PlayerCommands", true);
    setConfig(CONFIG_UINT32_INSTANT_LOGOUT, "InstantLogout", SEC_OBSERVER);
    setConfigMin(CONFIG_UINT32_GROUP_OFFLINE_LEADER_DELAY, "Group.OfflineLeaderDelay", 300, 0);
    setConfigMin(CONFIG_UINT32_GUILD_EVENT_LOG_COUNT, "Guild.EventLogRecordsCount", GUILD_EVENTLOG_MAX_RECORDS, GUILD_EVENTLOG_MAX_RECORDS);

    setConfig(CONFIG_UINT32_MIRRORTIMER_FATIGUE_MAX, "MirrorTimer.Fatigue.Max", 0);
    setConfig(CONFIG_UINT32_MIRRORTIMER_BREATH_MAX, "MirrorTimer.Breath.Max", 60);
    setConfig(CONFIG_UINT32_MIRRORTIMER_ENVIRONMENTAL_MAX, "MirrorTimer.Environmental.Max", 1);
    setConfig(CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MIN, "EnvironmentalDamage.Min", 605);
    setConfigMin(CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MAX, "EnvironmentalDamage.Max", 610, getConfig(CONFIG_UINT32_ENVIRONMENTAL_DAMAGE_MIN));

    setConfig(CONFIG_BOOL_PET_UNSUMMON_AT_MOUNT,      "PetUnsummonAtMount", false);
    setConfigMinMax(CONFIG_UINT32_PET_DEFAULT_LOYALTY, "PetDefaultLoyalty", 1, 1, 6);

    //setConfig(CONFIG_UINT32_ANTIFLOOD_SANCTION,       "Antiflood.Sanction", CHEAT_ACTION_KICK);

    setConfig(CONFIG_UINT32_MAX_AGE_SHOW_WARNING, "Account.ShowWarningAge", 3);

    setConfig(CONFIG_UINT32_HIGH_LEVEL_CHARACTER, "Account.HighLevelCharacter", 30);

    setConfig(CONFIG_BOOL_PTR, "PTR", false);

    setConfig(CONFIG_BOOL_VISIBILITY_FORCE_ACTIVE_OBJECTS, "Visibility.ForceActiveObjects", true);
    
    ///- Load the CharDelete related config options
    setConfigMinMax(CONFIG_UINT32_CHARDELETE_METHOD, "CharDelete.Method", 0, 0, 1);
    setConfigMinMax(CONFIG_UINT32_CHARDELETE_MIN_LEVEL, "CharDelete.MinLevel", 0, 0, getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));
    setConfigPos(CONFIG_UINT32_CHARDELETE_KEEP_DAYS, "CharDelete.KeepDays", 30);

    if (configNoReload(reload, CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE, "GuidReserveSize.Creature", 100))
        setConfigPos(CONFIG_UINT32_GUID_RESERVE_SIZE_CREATURE,   "GuidReserveSize.Creature",   100);
    if (configNoReload(reload, CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT, "GuidReserveSize.GameObject", 100))
        setConfigPos(CONFIG_UINT32_GUID_RESERVE_SIZE_GAMEOBJECT, "GuidReserveSize.GameObject", 100);

    setConfig(CONFIG_BOOL_VMAP_INDOOR_CHECK, "vmap.enableIndoorCheck", true);
    setConfig(CONFIG_BOOL_PET_LOS, "vmap.petLoS", true);
    setConfig(CONFIG_BOOL_MMAP_ENABLED, "mmap.enabled", true);

    setConfig(CONFIG_UINT32_EMPTY_MAPS_UPDATE_TIME, "MapUpdate.Empty.UpdateTime", 0);
    setConfigMinMax(CONFIG_UINT32_MAP_OBJECTSUPDATE_THREADS, "MapUpdate.ObjectsUpdate.MaxThreads", 4, 1, 20);
    setConfigMinMax(CONFIG_UINT32_MAP_OBJECTSUPDATE_TIMEOUT, "MapUpdate.ObjectsUpdate.Timeout", 100, 10, 2000);
    setConfigMinMax(CONFIG_UINT32_MAP_VISIBILITYUPDATE_THREADS, "MapUpdate.VisibilityUpdate.MaxThreads", 4, 1, 20);
    setConfigMinMax(CONFIG_UINT32_MAP_VISIBILITYUPDATE_TIMEOUT, "MapUpdate.VisibilityUpdate.Timeout", 100, 10, 2000);
    setConfigMinMax(CONFIG_UINT32_MAPUPDATE_INSTANCED_UPDATE_THREADS, "MapUpdate.Instanced.UpdateThreads", 2, 0, 20);
    setConfigMinMax(CONFIG_UINT32_MTCELLS_THREADS, "MapUpdate.Continents.MTCells.Threads", 0, 0, 20);
    setConfigMinMax(CONFIG_UINT32_MTCELLS_SAFEDISTANCE, "MapUpdate.Continents.MTCells.SafeDistance", 1066, 0, 34112);
    setConfigMinMax(CONFIG_UINT32_MAPUPDATE_UPDATE_PACKETS_DIFF, "MapUpdate.UpdatePacketsDiff", 100, 1, 10000);
    setConfigMinMax(CONFIG_UINT32_MAPUPDATE_UPDATE_PLAYERS_DIFF, "MapUpdate.UpdatePlayersDiff", 100, 1, 10000);
    setConfigMinMax(CONFIG_UINT32_MAPUPDATE_UPDATE_CELLS_DIFF, "MapUpdate.UpdateCellsDiff", 100, 1, 10000);
    setConfigMinMax(CONFIG_UINT32_INACTIVE_PLAYERS_SKIP_UPDATES, "Continents.InactivePlayers.SkipUpdates", 0, 0, 100);
    setConfig(CONFIG_UINT32_MAPUPDATE_TICK_LOWER_GRID_ACTIVATION_DISTANCE, "MapUpdate.ReduceGridActivationDist.Tick", 0);
    setConfig(CONFIG_UINT32_MAPUPDATE_TICK_INCREASE_GRID_ACTIVATION_DISTANCE, "MapUpdate.IncreaseGridActivationDist.Tick", 0);
    setConfig(CONFIG_UINT32_MAPUPDATE_MIN_GRID_ACTIVATION_DISTANCE, "MapUpdate.MinGridActivationDistance", 0);
    setConfig(CONFIG_UINT32_MAPUPDATE_TICK_LOWER_VISIBILITY_DISTANCE, "MapUpdate.ReduceVisDist.Tick", 0);
    setConfig(CONFIG_UINT32_MAPUPDATE_TICK_INCREASE_VISIBILITY_DISTANCE, "MapUpdate.IncreaseVisDist.Tick", 0);
    setConfig(CONFIG_UINT32_MAPUPDATE_MIN_VISIBILITY_DISTANCE, "MapUpdate.MinVisibilityDistance", 0);
    setConfig(CONFIG_BOOL_CONTINENTS_INSTANCIATE, "Continents.Instanciate", false);
    setConfig(CONFIG_UINT32_CONTINENTS_MOTIONUPDATE_THREADS, "Continents.MotionUpdate.Threads", 0);
    setConfig(CONFIG_BOOL_TERRAIN_PRELOAD_CONTINENTS, "Terrain.Preload.Continents", 1);
    setConfig(CONFIG_BOOL_TERRAIN_PRELOAD_INSTANCES, "Terrain.Preload.Instances", 1);

    setConfig(CONFIG_BOOL_ENABLE_MOVEMENT_EXTRAPOLATION_PLAYER, "Movement.ExtrapolatePlayerPosition", false);
    setConfig(CONFIG_BOOL_ENABLE_MOVEMENT_EXTRAPOLATION_PET, "Movement.ExtrapolatePetPosition", true);
    setConfig(CONFIG_UINT32_MOVEMENT_CHANGE_ACK_TIME, "Movement.PendingAckResponseTime", 2000);
    setConfigMinMax(CONFIG_UINT32_MAX_POINTS_PER_MVT_PACKET, "Movement.MaxPointsPerPacket", 80, 5, 10000);
    setConfigMinMax(CONFIG_UINT32_RELOCATION_VMAP_CHECK_TIMER, "Movement.RelocationVmapsCheckDelay", 0, 0, 2000);

    setConfigMinMax(CONFIG_UINT32_SPELLS_CCDELAY, "Spells.CCDelay", 200, 0, 20000);
    setConfigMinMax(CONFIG_UINT32_DEBUFF_LIMIT, "DebuffLimit", 16, 1, 40);

    setConfig(CONFIG_UINT32_ANTICRASH_OPTIONS, "Anticrash.Options", 0);
    setConfig(CONFIG_UINT32_ANTICRASH_REARM_TIMER, "Anticrash.Rearm.Timer", 0);

    setConfig(CONFIG_UINT32_PERFLOG_SLOW_WORLD_UPDATE, "PerformanceLog.SlowWorldUpdate", 100);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_MAPSYSTEM_UPDATE, "PerformanceLog.SlowMapSystemUpdate", 100);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_MAP_UPDATE, "PerformanceLog.SlowMapUpdate", 100);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_ASYNC_QUERIES, "PerformanceLog.SlowAsynQueries", 100);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_UNIQUE_SESSION_UPDATE, "PerformanceLog.SlowUniqueSessionUpdate", 20);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_PACKET, "PerformanceLog.SlowPackets", 20);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_MAP_PACKETS, "PerformanceLog.SlowMapPackets", 60);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_SESSIONS_UPDATE, "PerformanceLog.SlowSessionsUpdate", 0);
    setConfig(CONFIG_UINT32_PERFLOG_SLOW_PACKET_BCAST, "PerformanceLog.SlowPacketBroadcast", 0);
    setConfig(CONFIG_UINT32_LOG_MONEY_TRADES_TRESHOLD, "LogMoneyTreshold", 10000);

    setConfig(CONFIG_FLOAT_DYN_RESPAWN_CHECK_RANGE, "DynamicRespawn.Range", -1.0f);
    setConfig(CONFIG_FLOAT_DYN_RESPAWN_MAX_REDUCTION_RATE, "DynamicRespawn.MaxReductionRate", 0.0f);
    setConfig(CONFIG_FLOAT_DYN_RESPAWN_PERCENT_PER_PLAYER, "DynamicRespawn.PercentPerPlayer", 0.0f);
    setConfig(CONFIG_UINT32_DYN_RESPAWN_MIN_RESPAWN_TIME, "DynamicRespawn.MinRespawnTime", 0);
    setConfig(CONFIG_UINT32_DYN_RESPAWN_MIN_RESPAWN_TIME_ELITE, "DynamicRespawn.MinEliteRespawnTime", 0);
    setConfig(CONFIG_UINT32_DYN_RESPAWN_MIN_RESPAWN_TIME_INDOORS, "DynamicRespawn.MinIndoorRespawnTime", 0);
    setConfig(CONFIG_UINT32_DYN_RESPAWN_AFFECT_RESPAWN_TIME_BELOW, "DynamicRespawn.AffectRespawnTimeBelow", 0);
    setConfig(CONFIG_UINT32_DYN_RESPAWN_AFFECT_LEVEL_BELOW, "DynamicRespawn.AffectLevelBelow", 0);
    setConfig(CONFIG_UINT32_DYN_RESPAWN_PLAYERS_THRESHOLD, "DynamicRespawn.PlayersThreshold", 0);
    setConfig(CONFIG_UINT32_DYN_RESPAWN_PLAYERS_LEVELDIFF, "DynamicRespawn.PlayersMaxLevelDiff", 0);

    setConfig(CONFIG_UINT32_CHANNEL_INVITE_MIN_LEVEL, "ChannelInvite.MinLevel", 10);
    setConfig(CONFIG_BOOL_WHISPER_RESTRICTION, "WhisperRestriction", false);
    setConfig(CONFIG_UINT32_WORLD_CHAN_MIN_LEVEL, "WorldChan.MinLevel", 0);
    setConfig(CONFIG_UINT32_WORLD_CHAN_CD, "WorldChan.Cooldown", 0);
    setConfig(CONFIG_UINT32_WORLD_CHAN_CD_MAX_LEVEL, "WorldChan.CooldownMaxLevel", 0);
    setConfig(CONFIG_UINT32_WORLD_CHAN_CD_SCALING, "WorldChan.CooldownScaling", 0);
    setConfig(CONFIG_UINT32_WORLD_CHAN_CD_USE_ACCOUNT_MAX_LEVEL, "WorldChan.CooldownUseAccountLevel", 0);
    setConfig(CONFIG_UINT32_PUB_CHANS_MUTE_VANISH_LEVEL, "PublicChansMute.BypassLevel", 61);
    setConfig(CONFIG_UINT32_SAY_MIN_LEVEL, "SayMinLevel", 0);
    setConfig(CONFIG_UINT32_YELL_MIN_LEVEL, "YellMinLevel", 0);
    setConfig(CONFIG_UINT32_SAY_EMOTE_MIN_LEVEL, "SayEmoteMinLevel", 0);
    setConfig(CONFIG_UINT32_WHISP_DIFF_ZONE_MIN_LEVEL, "WhisperDiffZone.MinLevel", 0);
    setConfig(CONFIG_UINT32_YELLRANGE_LINEARSCALE_MAXLEVEL, "YellRange.LinearScale.MaxLevel", 0);
    setConfig(CONFIG_UINT32_YELLRANGE_QUADRATICSCALE_MAXLEVEL, "YellRange.QuadraticScale.MaxLevel", 0);
    setConfig(CONFIG_UINT32_YELLRANGE_MIN, "YellRange.Min", 0);
    setConfig(CONFIG_UINT32_WHISPER_TARGETS_MAX, "WhisperTargets.MaxTargets", 0);
    setConfig(CONFIG_UINT32_WHISPER_TARGETS_DECAY, "WhisperTargets.DecayTime", 0);
    setConfig(CONFIG_UINT32_WHISPER_TARGETS_BYPASS_LEVEL, "WhisperTargets.BypassLevel", 0);

    setConfig(CONFIG_UINT32_ITEM_INSTANTSAVE_QUALITY, "Item.InstantSaveQuality", ITEM_QUALITY_ARTIFACT);
    setConfig(CONFIG_UINT32_ITEM_RARELOOT_QUALITY, "Item.RareLoot.Quality", ITEM_QUALITY_EPIC);
    setConfig(CONFIG_BOOL_PREVENT_ITEM_DATAMINING, "Item.PreventDataMining", true);

    setConfig(CONFIG_UINT32_MAILSPAM_EXPIRE_SECS, "MailSpam.ExpireSecs", 0);
    setConfig(CONFIG_UINT32_MAILSPAM_MAX_MAILS, "MailSpam.MaxMails", 2);
	setConfig(CONFIG_UINT32_MAILSPAM_LEVEL, "MailSpam.Level", 1);
	setConfig(CONFIG_UINT32_MAILSPAM_ACCOUNT_LEVEL, "MailSpam.AccountCharLevel", 1);
    setConfig(CONFIG_UINT32_MAILSPAM_MONEY, "MailSpam.Money", 0);
    setConfig(CONFIG_BOOL_MAILSPAM_ITEM, "MailSpam.Item", false);
    setConfig(CONFIG_UINT32_COD_FORCE_TAG_MAX_LEVEL, "Mails.COD.ForceTag.MaxLevel", 0);

    setConfigMinMax(CONFIG_UINT32_ASYNC_TASKS_THREADS_COUNT,       "AsyncTasks.Threads", 1, 1, 20);
    setConfig(CONFIG_BOOL_KICK_PLAYER_ON_BAD_PACKET,               "Network.KickOnBadPacket", false);
    setConfig(CONFIG_UINT32_PACKET_BCAST_THREADS,                  "Network.PacketBroadcast.Threads", 0);
    setConfig(CONFIG_UINT32_PACKET_BCAST_FREQUENCY,                "Network.PacketBroadcast.Frequency", 50);
    setConfig(CONFIG_UINT32_PBCAST_DIFF_LOWER_VISIBILITY_DISTANCE, "Network.PacketBroadcast.ReduceVisDistance.DiffAbove", 0);

    // PvP options
    setConfig(CONFIG_BOOL_ACCURATE_PVP_EQUIP_REQUIREMENTS, "PvP.AccurateEquipRequirements", true);
    setConfig(CONFIG_BOOL_ACCURATE_PVP_PURCHASE_REQUIREMENTS, "PvP.AccuratePurchaseRequirements", true);
    setConfig(CONFIG_BOOL_ACCURATE_PVP_TIMELINE, "PvP.AccurateTimeline", true);
    setConfig(CONFIG_BOOL_ACCURATE_PVP_REWARDS, "PvP.AccurateRewards", true);
    setConfig(CONFIG_BOOL_ENABLE_DK, "PvP.DishonorableKills", true);
    setConfig(CONFIG_FLOAT_OPEN_WORLD_HONOR_MULTIPLIER, "PvP.OpenWorldHonorMultiplier", 1.5f);

    // Progression settings
    setConfig(CONFIG_UINT32_CONTENT_PHASE, "Progression.ContentPhase", CONTENT_PHASE_4);
    setConfig(CONFIG_BOOL_ACCURATE_PETS, "Progression.AccuratePetStatistics", true);
    setConfig(CONFIG_BOOL_ACCURATE_LFG, "Progression.AccurateLFGAvailability", true);
    setConfig(CONFIG_BOOL_ACCURATE_PVE_EVENTS, "Progression.AccuratePVEEvents", true);
    setConfig(CONFIG_BOOL_ACCURATE_SPELL_EFFECTS, "Progression.AccurateSpellEffects", true);
    setConfig(CONFIG_BOOL_NO_RESPEC_PRICE_DECAY, "Progression.NoRespecPriceDecay", true);
    setConfig(CONFIG_BOOL_NO_QUEST_XP_TO_GOLD, "Progression.NoQuestXpToGold", true);
    setConfig(CONFIG_BOOL_RESTORE_DELETED_ITEMS, "Progression.RestoreDeletedItems", true);

    setConfig(CONFIG_BOOL_ENABLE_PRIORITY_QUEUE, "PriorityQueue.Enable", false);
    setConfig(CONFIG_BOOL_PRIORITY_QUEUE_ENABLE_WESTERN_PRIORITY, "PriorityQueue.WesternEnable", false);

    setConfig(CONFIG_BOOL_ENABLE_DYNAMIC_VISIBILITIES, "DynamicVisibility.Enable", false);

    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_PER_TICK, "PriorityQueue.PriorityPerTick", 1);
    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_DONATOR_SETTINGS, "PriorityQueue.DonatorSettings", 0);
    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_DONATOR_PRIORITY, "PriorityQueue.DonatorPriority", 0);
    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_WESTERN_PRIORITY, "PriorityQueue.WesternPriority", 0);
    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_HIGH_LEVEL_CHAR, "PriorityQueue.HighLevelChar", 50);
    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_HIGH_LEVEL_CHAR_PRIORITY, "PriorityQueue.HighLevelCharPriority", 0);
    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_PER_ACCOUNT_DAY, "PriorityQueue.PriorityPerAccountDay", 0);
    setConfig(CONFIG_BOOL_PRIORITY_QUEUE_ENABLE_IP_PENALTY, "PriorityQueue.IpPenaltyEnable", false);
    setConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_REDUCTION_MULTIBOX, "PriorityQueue.IpPenaltyPriorityReduction", 0);
    setConfig(CONFIG_UINT32_MAX_PERCENTAGE_POP_NON_REGIONAL, "Queue.MaxNonRegionalPercentage", 100);
    setConfig(CONFIG_UINT32_MAX_PERCENTAGE_POP_REGIONAL, "Queue.MaxRegionalPercentage", 100);

    // Antispam
    setConfig(CONFIG_BOOL_AC_ANTISPAM_ENABLED, "Antispam.Enable", true);
    setConfig(CONFIG_BOOL_AC_ANTISPAM_BAN_ENABLED, "Antispam.BanEnable", false);
    setConfig(CONFIG_BOOL_AC_ANTISPAM_MERGE_ALL_WHISPERS, "Antispam.MergeAllWhispers", false);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_MAX_RESTRICTION_LEVEL, "Antispam.MaxRestrictionLevel", 20);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_ORIGINAL_NORMALIZE_MASK, "Antispam.OriginalNormalizeMask", 0);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_FULLY_NORMALIZE_MASK, "Antispam.FullyNormalizeMask", 0);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_SCORE_THRESHOLD, "Antispam.ScoreThreshold", 4);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_MUTETIME, "Antispam.Mutetime", 3600);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_CHAT_MASK, "Antispam.ChatMask", 0);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_DETECT_THRESHOLD, "Antispam.DetectThreshold", 3);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_REPEAT_COUNT, "Antispam.RepeatCount", 5);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_UPDATE_TIMER, "Antispam.UpdateTimer", 60000);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_MESSAGE_BLOCK_SIZE, "Antispam.MessageBlockSize", 5);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_FREQUENCY_TIME, "Antispam.FrequencyTime", 3);
    setConfig(CONFIG_UINT32_AC_ANTISPAM_FREQUENCY_COUNT, "Antispam.FrequencyCount", 3);

    setConfig(CONFIG_UINT32_CREATURE_SUMMON_LIMIT, "MaxCreatureSummonLimit", DEFAULT_CREATURE_SUMMON_LIMIT);

    // Server autobroadcast settings:
    setConfig(CONFIG_UINT32_AUTOBROADCAST_INTERVAL, "AutoBroadcast.Timer", 60000);

    setConfig(CONFIG_BOOL_TRANSMOG_ENABLED, "Transmog.Enable", false);
    setConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM, "Transmog.ReqItemID", 0);
    setConfig(CONFIG_UINT32_TRANSMOG_REQ_ITEM_COUNT, "Transmog.ReqItemCount", 1);
    setConfig(CONFIG_FLOAT_TRANSMOG_REQ_MONEY_RATE, "Transmog.ReqMoneyRate", 0.0);
    setConfig(CONFIG_BOOL_STATIC_OBJECT_LOS, "StaticObjectLOS", true);
    setConfig(CONFIG_BOOL_DUAL_SPEC, "DualSpec", false);
    
    setConfig(CONFIG_BOOL_HARDCORE_DISABLE_DUEL, "Hardcore.Disable.Duel", false);

    setConfig(CONFIG_UINT32_BG_SV_SPARK_MAX_COUNT, "BattlegroundSV.MaxSparks", 100);

    setConfig(CONFIG_UINT32_ITEM_LOG_RESTORE_QUALITY, "ItemRestoreLog.MinQuality", 3);

    setConfig(CONFIG_UINT32_CHAT_MIN_LEVEL, "Chat.MinLevel", 5);

    setConfig(CONFIG_UINT32_ACCOUNT_DATA_LAST_LOGIN_DAYS, "AccountData.LastLoginDays", 60);

    setConfig(CONFIG_BOOL_ITEM_LOG_RESTORE_QUEST_ITEMS, "ItemRestoreLog.QuestItems", false);
    setConfig(CONFIG_BOOL_LOAD_LOCALES, "LoadLocales", true);

    setConfig(CONFIG_BOOL_ENABLE_FACTION_BALANCE, "FactionBalance.Enable", false);
    setConfig(CONFIG_BOOL_BLOCK_ALL_HANZI, "Hanzi.BlockAll", false);

    setConfig(CONFIG_BOOL_BACKUP_CHARACTER_INVENTORY, "BackupCharacterInventory", false);

    setConfig(CONFIG_BOOL_ANALYSIS_STOP_ON_CORRECT_EXTENDED_DATA, "Analysis.StopOnCorrectExtendedData", false);
    setConfig(CONFIG_BOOL_ANALYSIS_DO_SHARED_DATA_DETAILED_REPORT, "Analysis.DoSharedDataDetailedReport", false);
    setConfig(CONFIG_BOOL_ANALYSIS_ALLOW_RELAXED_IP, "Analysis.AllowRelaxedIp", true);
    setConfig(CONFIG_BOOL_ANALYSIS_LOG_DISCORD_SUMMARY, "Analysis.AlwaysLogDiscordSummary", false);
    setConfig(CONFIG_BOOL_ANALYSIS_PING_ON_WARNING, "Analysis.PingOnWarning", false);
    setConfig(CONFIG_BOOL_ANALYSIS_AUTOMATIC_PUNIHSMENT, "Analysis.AutomaticMark", false);


    setConfig(CONFIG_UINT32_ACCOUNT_TRUSTED_IP_PERCENTAGE, "Analysis.TrustedIpPercentageLogins", 25);
    setConfig(CONFIG_UINT32_ANALYSIS_NO_FINGERPRINT_MATCH_WEIGHT, "Analysis.NoFingerPrintMatchWeight", 150);
    setConfig(CONFIG_UINT32_ANALYSIS_NO_EXTENDED_DATA_MATCH_WEIGHT, "Analysis.NoExtendedDataMatchWeight", 150);
    setConfig(CONFIG_UINT32_ANALYSIS_NO_CPU_DATA_WEIGHT, "Analysis.NoCpuDataWeight", 10);
    setConfig(CONFIG_UINT32_ANALYSIS_NO_CPU_DATA_MATCH_WEIGHT, "Analysis.NoCpuDataMatchWeight", 150);
    setConfig(CONFIG_UINT32_ANALYSIS_NO_EXTENDED_DATA_WEIGHT, "Analysis.NoExtendedDataWeight", 10);
    setConfig(CONFIG_UINT32_ANALYSIS_WARNING_THRESHOLD, "Analysis.WarningThreshold", 300);

    setConfig(CONFIG_BOOL_SUSPICIOUS_ENABLE, "Suspicious.Enable", true);
    setConfig(CONFIG_BOOL_SUSPICIOUS_MOVEMENT_ENABLE, "Suspicious.Movement.Enable", true);
    setConfig(CONFIG_BOOL_SUSPICIOUS_FISHING_ENABLE, "Suspicious.Fishing.Enable", true);
    setConfig(CONFIG_BOOL_SUSPICIOUS_NPC_KILLED_ENABLE, "Suspicious.Npckilling.Enable", true);
    setConfig(CONFIG_FLOAT_SUSPICIOUS_MOVEMENTSPEED_REPORT_THRESHOLD, "Suspicious.Movement.MovementSpeedThreshold", 100.0f);

    // Enforce English only on EU realms:
    setConfig(CONFIG_BOOL_ENFORCED_ENGLISH, "EnforceEnglish", false);
    // Chinese realms:
    setConfig(CONFIG_BOOL_SEA_NETWORK, "NiHao", false);
    // Festive things:
    setConfig(CONFIG_BOOL_HOLIDAY_EVENT, "HolidayEvent", true);
    // Shop:
    setConfig(CONFIG_UINT32_SHOP_REFUND_WINDOW, "Shop.RefundWindow", DAY * 2);

    setConfig(CONFIG_UINT32_AUTO_PDUMP_MIN_CHAR_LEVEL, "AutoPDump.MinCharLevel", 30);
    setConfig(CONFIG_UINT32_AUTO_PDUMP_DELETE_AFTER_DAYS, "AutoPDump.DeleteAfterDays", 60);

    setConfig(CONFIG_BOOL_PERFORMANCE_ENABLE, "Perf.Enable", true);

    setConfig(CONFIG_UINT32_PERFORMANCE_REPORT_INTERVAL, "Perf.ReportInterval", 600);
    setConfig(CONFIG_UINT32_MAX_GOLD_TRANSFERRED, "Transfer.MaxGold", 300000);
    setConfig(CONFIG_UINT32_MAX_ITEM_STACK_TRANSFERRED, "Transfer.MaxItemStack", 50);
    setConfig(CONFIG_UINT32_DYNAMIC_SCALING_POP, "DynamicScaling.PopulationStart", BLIZZLIKE_REALM_POPULATION);

    // Migration for auto committing updates.
    setConfig(CONFIG_UINT32_AUTO_COMMIT_MINUTES, "AutoCommit.Minutes", 0);
}

void World::ExportConfigSettingsToDB()
{
    sLog.outString("Skipping config export to database (disabled).");

    // Disabled: previously wrote all config values into world_config.
}

void autoCommitWorkerThread()
{
    time_t lastUpdateTime = time(0);
    while (!sWorld.IsStopped())
    {
        time_t now = time(0);
        if (sWorld.GetMigration().HasChanges() &&
           (lastUpdateTime + sWorld.getConfig(CONFIG_UINT32_AUTO_COMMIT_MINUTES) * MINUTE) < now)
        {
            lastUpdateTime = now;
            sWorld.GetMigration().CommitUpdates();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

constexpr uint32 MailReturnDelay = 30 * MINUTE * IN_MILLISECONDS;

void charactersDatabaseWorkerThread()
{
    static uint32 returnDelay = MailReturnDelay;
    static uint32 currentMs = WorldTimer::getMSTime();

    thread_name("CharDBWorker");

    CharacterDatabase.ThreadStart();
    while (!sWorld.IsStopped())
    {
        std::this_thread::sleep_for(2s);

        uint32 diff = WorldTimer::getMSTimeDiffToNow(currentMs);
        currentMs = WorldTimer::getMSTime();
        
        if (returnDelay >= diff)
        {
            returnDelay -= diff;
            continue;
        }

        if (CharacterDatabase.HasAsyncQuery())
            continue;

        Player::DeleteOldCharacters();
        sObjectMgr.ReturnOrDeleteOldMails(true);
        returnDelay = MailReturnDelay;
    }
    CharacterDatabase.ThreadEnd();
}

void World::RestoreLostGOs()
{
    std::fstream file{ "log.txt" };

    if (!file)
        return;

    std::regex pattern{ "gobject (add|delete|turn) (\\d+) +X: +([+-]?[0-9]*[.]?[0-9]+) +Y: +([+-]?[0-9]*[.]?[0-9]+) +Z: +([+-]?[0-9]*[.]?[0-9]+) +Map: +(\\d+)" };


    const uint32 startingGuid = 5000095;
    uint32 currentGuid = startingGuid;

    std::unordered_map<uint32, GameObject*> guidLinkage;
    std::string val;
    while (std::getline(file, val))
    {
        std::smatch match;
        if (!std::regex_search(val, match, pattern))
            continue;

        const std::string command = match[1].str();
        const uint32 entryOrGuid =  atoi(match[2].str().c_str());
        float xCoord = strtod(match[3].str().c_str(), nullptr);
        float yCoord = strtod(match[4].str().c_str(), nullptr);
        float zCoord = strtod(match[5].str().c_str(), nullptr);
        uint32 mapId  = atoi(match[6].str().c_str());

        if (command.find("add") != std::string::npos)
        {
            const GameObjectInfo* gInfo = sObjectMgr.GetGameObjectInfo(entryOrGuid);

            if (!gInfo)
            {
                sLog.outError("ERROR WITH GO PROTO.");
                continue;
            }

            Map* map = sMapMgr.FindMap(mapId);

            if (!map)
            {
                sLog.outError("ERROR WITH MAP ID.");
                continue;
            }

            GameObject* pGameObj = new GameObject;

            // used guids from specially reserved range (can be 0 if no free values)
            uint32 db_lowGUID = sObjectMgr.GenerateStaticGameObjectLowGuid();
            if (!db_lowGUID)
            {
                sLog.outError("ERROR WITH lowGUID.");
                continue;
            }

            if (!pGameObj->Create(db_lowGUID, gInfo->id, map, xCoord, yCoord, zCoord, 0.f, 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
            {
                delete pGameObj;
                continue;
            }


            pGameObj->SetRespawnTime(300); // Default 5 min.
       
            guidLinkage[currentGuid] = pGameObj;
            ++currentGuid;
        }

        if (command.find("delete") != std::string::npos)
        {
            guidLinkage.erase(entryOrGuid);
        }

        if (command.find("turn") != std::string::npos)
        {
            //dont have orientation from logs so can't correct orientation for GOs
        }

        for (auto obj : guidLinkage)
        {
            obj.second->SaveToDB(obj.second->GetMapId());
        }
    }
}

void ExportLogs()
{
    //Let's start with loot logs..
    constexpr uint32 MaxLineLength = 8092;
    std::string logfn = sConfig.GetStringDefault("LootsLogFile", "");

    std::string logsDir = sConfig.GetStringDefault("LogsDir", "");
    if (!logsDir.empty())
    {
        if ((logsDir.at(logsDir.length() - 1) != '/') && (logsDir.at(logsDir.length() - 1) != '\\'))
            logsDir.append("/");
    }

    std::string path = logsDir + logfn;


    
    std::ios::sync_with_stdio(false);

    {
        static const re2::RE2 lootPattern = R"((\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) (\w+):(\d+) \[\w+:(\d+)@(\d+.\d+.\d+.\d)\] (gets|loots) (\S+) \[loot from (Creature|Item|Gameobject) \((?:Entry: (\d+))? ?Guid: (\d+)\)\])";
        std::ifstream file(path);
        std::string line;

        std::vector<std::string> insertValues;


        if (file.is_open())
        {
            while (std::getline(file, line))
            {
                LootLogEntry logEntry;
                std::string dateTime;
                std::string lootOrMoneyString;
                std::string lootContentString;
                std::string enumString;
                uint32 entry = 0;

                if (!re2::RE2::PartialMatch(line, lootPattern, &dateTime, &logEntry.receiverName, &logEntry.receiverGuid, &logEntry.receiverAccountId, &logEntry.receiverIp, &lootOrMoneyString,
                 &lootContentString, &enumString, &entry, &logEntry.sourceGuid))
                {
                    //redo the match for some off-logs where we dont have entry but have the rest.
                    if (!re2::RE2::PartialMatch(line, lootPattern, &dateTime, &logEntry.receiverName, &logEntry.receiverGuid, &logEntry.receiverAccountId, &logEntry.receiverIp, &lootOrMoneyString,
                        &lootContentString, &enumString, nullptr, &logEntry.sourceGuid))
                    {
                        continue;
                    }

                }

                //format even further..
                logEntry.sourceEntry = entry;

                const bool isMoneyLog = lootOrMoneyString == "gets";

                if (isMoneyLog)
                {
                    auto idx = 0;
                    uint32 gold = 0, silver = 0, copper = 0;
                    if (size_t goldPos = lootContentString.find_first_of('g'); goldPos != std::string::npos)
                    {
                        gold = atoi(lootContentString.substr(idx, goldPos - idx).c_str());
                        idx = goldPos + 1;
                    }

                    if (size_t silverPos = lootContentString.find_first_of('s'); silverPos != std::string::npos)
                    {
                        silver = atoi(lootContentString.substr(idx, silverPos - idx).c_str());
                        idx = silverPos + 1;
                    }

                    if (size_t copperPos = lootContentString.find_first_of('c'); copperPos != std::string::npos)
                    {
                        copper = atoi(lootContentString.substr(idx, copperPos - idx).c_str());
                    }

                    logEntry.money = (gold * GOLD) + (silver * SILVER) + copper;
                }
                else
                {
                    static const re2::RE2 itemLootPattern = R"((\d+)x(\d+))";
                    re2::RE2::PartialMatch(lootContentString, itemLootPattern, &logEntry.itemCount, &logEntry.itemEntry);
                }

                if (enumString == "Gameobject")
                    logEntry.sourceType = LogLoot::SourceGameObject;
                else if (enumString == "Item")
                    logEntry.sourceType = LogLoot::SourceItem;
                else
                    logEntry.sourceType = LogLoot::SourceCreature;

                

                insertValues.push_back(string_format("('{}', '{}', {}, {}, '{}', '{}', {}, {}, {}, {}, {}, '{}')", dateTime.c_str(), logEntry.receiverName.c_str(), logEntry.receiverGuid, logEntry.receiverAccountId, logEntry.receiverIp.c_str(), logEntry.sourceType.data(),
                    logEntry.sourceGuid, logEntry.sourceEntry, logEntry.money, logEntry.itemEntry, logEntry.itemCount, logEntry.lootType.data()));

                if (insertValues.size() >= 500)
                {
                    std::string multiValueInsert = "INSERT INTO `character_loot` (`date_time`, `receiver_name`, `receiver_guid`, `receiver_account_id`, `receiver_ip`, `source_type`, `source_guid`, `source_entry`, `money`, `item_entry`, `item_count`, `loot_type`) VALUES";
                    multiValueInsert.reserve(MaxLineLength * 48);
                    for (const auto& elem : insertValues)
                    {
                        multiValueInsert += elem;
                        multiValueInsert += ',';
                    }
                    multiValueInsert[multiValueInsert.length() - 1] = ';'; // change , to ; for last value.
                    insertValues.clear();
                    LogsDatabase.Execute(multiValueInsert.c_str());
                }

            }
        }
    }
}

void World::StopHttpApiServer()
{
    if (_server)
        _server->Stop();
}

void CheckEggExploit()
{
    if (std::filesystem::exists("egglog.txt"))
        return;

    std::ofstream fileStream{ "egglog.txt" };

    if (!fileStream)
        return;

    std::unique_ptr<QueryResult> charResult = std::unique_ptr<QueryResult>(CharacterDatabase.Query("SELECT guid, account, mortality_status FROM characters"));

    std::unordered_map<uint32, std::pair<uint32, uint8>> charAccounts;

    if (charResult)
    {
        do
        {
            auto fields = charResult->Fetch();

            charAccounts[fields[0].GetUInt32()] = std::make_pair(fields[1].GetUInt32(), fields[2].GetUInt8());
        } while (charResult->NextRow());
    }

    auto eggResult = std::unique_ptr<QueryResult>(CharacterDatabase.Query("SELECT playerGuid, COUNT(*) AS amount FROM character_egg_loot "
        "WHERE refunded = 0 GROUP BY playerGuid ORDER BY amount DESC"));


    std::unordered_map<uint32, uint32> charEggLootResult;
    if (eggResult)
    {
        do
        {
            auto fields = eggResult->Fetch();

            charEggLootResult[fields[0].GetUInt32()] = fields[1].GetUInt32();
        } while (eggResult->NextRow());
    }

    auto logResult = std::unique_ptr<QueryResult>(LoginDatabase.PQuery("SELECT guid, COUNT(*) AS amount FROM shop_logs WHERE item = 92010 "
        "AND refunded = 1 AND realm_id = %u GROUP BY guid ORDER BY amount DESC", realmID));


    std::unordered_map<uint32, uint32> charShopLog;
    if (logResult)
    {
        do
        {
            auto fields = logResult->Fetch();

            charShopLog[fields[0].GetUInt32()] = fields[1].GetUInt32();
        } while (logResult->NextRow());
    }

    for (const auto& [guid, amount] : charShopLog)
    {
        if (auto itr = charEggLootResult.find(guid); itr != charEggLootResult.end())
        {
            uint32 accountId = 0;
            bool isHardcore = false;
            if (auto it = charAccounts.find(guid); it != charAccounts.end())
            {
                accountId = it->second.first;
                isHardcore = it->second.second ? true : false;
            }
            else
                fileStream << "CHARACTER DELETED : ";

            if (abs((int)amount - (int)itr->second) > 1)
            {
                fileStream << "Char Guid " << guid << " has inconsistent egg refunds " << abs((int)amount - (int)itr->second) << ". Account ID " <<
                    accountId << (accountId ? "." : " Deleted.") << " IsHardcore (if not deleted) " << isHardcore << "\n";
            }
            else
                fileStream << "\n";
        }
        else
        {
            if (amount > 1)
                fileStream << "All eggs refunded on char Guid " << guid << ".\n";
        }
    }
}

void LoadPlayerEggLoot();

    /// Initialize the World
    void World::SetInitialWorldSettings()
    {
        const bool enableHttpApi = sConfig.GetIntDefault("HttpApi.Enable", 1) != 0;
        if (enableHttpApi)
        {
            //Have to do it like this to get proper thread handling in the threadpool of the HTTPS api backend to allow querying on any post or get handlers.
            HttpApi::ApiServer::SetInitThreadCallback([]() { mysql_thread_init(); });
            HttpApi::ApiServer::SetDestroyThreadCallback([]() { mysql_thread_end(); });

            _server = std::unique_ptr<HttpApi::ApiServer, ApiServerDeleter>(new HttpApi::ApiServer);
            HttpApi::RegisterControllers();
            _server->Start(sConfig.GetStringDefault("HttpApi.BindIP", "127.0.0.1"), sConfig.GetIntDefault("HttpApi.BindPort", 50000));
        }
        else
            sLog.outString("HTTP API server disabled by config.");
///- Initialize the random number generator
    srand((unsigned int)time(nullptr));

    ///- Time server startup
    uint32 uStartTime = WorldTimer::getMSTime();

    sLog.outString("Loading config...");
    LoadConfigSettings();

    if (sConfig.GetIntDefault("Logs.Export", 0) == 1)
    {
        sLog.outString("Initializing Log Exporter.. The server will shutdown after completion.");
        ExportLogs();
        return;
    }

    ///- Check the existence of the map files for all races start areas.
    if (!MapManager::ExistMapAndVMap(0, -6240.32f, 331.033f) ||
            !MapManager::ExistMapAndVMap(0, -8949.95f, -132.493f) ||
            !MapManager::ExistMapAndVMap(1, -618.518f, -4251.67f) ||
            !MapManager::ExistMapAndVMap(0, 1676.35f, 1677.45f) ||
            !MapManager::ExistMapAndVMap(1, 10311.3f, 832.463f) ||
            !MapManager::ExistMapAndVMap(1, -2917.58f, -257.98f))
    {
        sLog.outError("Correct *.map files not found in path '%smaps' or *.vmtree/*.vmtile files in '%svmaps'. Please place *.map and vmap files in appropriate directories or correct the DataDir value in the mangosd.conf file.", m_dataPath.c_str(), m_dataPath.c_str());
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }


    ///- Loading strings. Getting no records means core load has to be canceled because no error message can be output.
    sLog.outString("Loading MaNGOS strings...");
    if (!sObjectMgr.LoadMangosStrings())
    {
        Log::WaitBeforeContinueIfNeed();
        exit(1);                                            // Error message displayed in function already
    }

    sLog.outString("Initiating auto-updater...");
    using namespace DBUpdater;
    if (!sAutoUpdater.ProcessUpdates())
    {
        sLog.outError("DB AutoUpdater FAILED, cancelling server.");
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    sLog.outString("Loading module strings...");
    if (!sObjectMgr.LoadModuleStrings())
    {
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    CheckEggExploit();

    ///- Loads existing IDs in the database.
    sLog.outString("Loading existing IDs in the database...");
    sObjectMgr.LoadAllIdentifiers();

    ///- Chargements des variables (necessaire pour le OutdoorJcJ)
    sLog.outString("Loading saved variables...");
    sObjectMgr.LoadSavedVariable();

    ///- Update the realm entry in the database with the realm type from the config file
    //No SQL injection as values are treated as integers

    // not send custom type REALM_FFA_PVP to realm list
    uint32 server_type = IsFFAPvPRealm() ? REALM_TYPE_PVP : getConfig(CONFIG_UINT32_GAME_TYPE);
    uint32 realm_zone = getConfig(CONFIG_UINT32_REALM_ZONE);
    LoginDatabase.PExecute("UPDATE realmlist SET icon = %u, timezone = %u WHERE id = '%u'", server_type, realm_zone, realmID);

    sAccountMgr.Load();

    ///- Remove the bones (they should not exist in DB though) and old corpses after a restart
    CharacterDatabase.PExecute("DELETE FROM corpse WHERE corpse_type = '0' OR time < (UNIX_TIMESTAMP()-'%u')", 3 * DAY);

    sLog.outString("Loading chat channels...");
    sObjectMgr.LoadChatChannels();
    sLog.outString("Loading script names...");
    sScriptMgr.LoadScriptNames();
    sLog.outString("Loading spells...");
    sSpellMgr.LoadSpells();
    sLog.outString("Loading factions...");
    sObjectMgr.LoadFactions();
    sLog.outString("Loading sounds...");
    sObjectMgr.LoadSoundEntries();
    sLog.outString("Loading taxi nodex...");
    sObjectMgr.LoadTaxiNodes();
    sLog.outString("Loading area triggers...");
    sObjectMgr.LoadAreaTriggers();
    sLog.outString("Loading skill lines...");
    sObjectMgr.LoadSkillLineAbility();
    sLog.outString("Loading mail templates...");
    sObjectMgr.LoadMailTemplate();
    sLog.outString("Loading pet spell data...");
    sObjectMgr.LoadPetSpellData();
    sLog.outString("Loading world safe location orientation...");
    sObjectMgr.LoadWorldSafeLocsFacing();
    sLog.outString("Loading DBC data...");
    LoadDBCStores(m_dataPath);
    DetectDBCLang();
    sObjectMgr.SetDBCLocaleIndex(GetDefaultDbcLocale());    // Get once for all the locale index of DBC language (console/broadcasts)
    sLog.outString("Loading map templates...");
    sObjectMgr.LoadMapTemplate();
    sLog.outString("Loading area templates...");
    sObjectMgr.LoadAreaTemplate();
    sLog.outString("Loading spell mods...");
    sSpellModMgr.LoadSpellMods();
    sSpellMgr.AssignInternalSpellFlags();
    sLog.outString("Loading skill line ability maps...");
    sSpellMgr.LoadSkillLineAbilityMaps();
    sLog.outString("Loading race and class skills...");
    sSpellMgr.LoadSkillRaceClassInfoMap();

	///- Clean up and pack instances
	sMapPersistentStateMgr.CleanupInstances();              // must be called before `creature_respawn`/`gameobject_respawn` tables

	sMapPersistentStateMgr.PackInstances();

	sObjectMgr.PackGroupIds();                              // must be after CleanupInstances

	sMapPersistentStateMgr.ScheduleInstanceResets();        // Must be after cleanup and packing

    ///- Init highest guids before any guid using table loading to prevent using not initialized guids in some code.
    sObjectMgr.SetHighestGuids();                           // must be after packing instances
    sLog.outString("Loading broadcast texts...");
    sObjectMgr.LoadBroadcastTexts();
    sLog.outString("Loading page texts...");
    sObjectMgr.LoadPageTexts();
    sLog.outString("Loading gameobject info...");
    sObjectMgr.LoadGameobjectInfo();
    sLog.outString("Loading transport templates...");
    sTransportMgr.LoadTransportTemplates();
    sLog.outString("Loading spell chains...");
    sSpellMgr.LoadSpellChains();
    sLog.outString("Loading spell elixirs...");
    sSpellMgr.LoadSpellElixirs();
    sLog.outString("Loading spell learn skills...");
    sSpellMgr.LoadSpellLearnSkills();                       // must be after LoadSpellChains
    sLog.outString("Loading spell skill bonuses...");
    sSpellMgr.LoadSpellSkillBonuses();
    sLog.outString("Loading spell learn spells...");
    sSpellMgr.LoadSpellLearnSpells();
    sLog.outString("Loading spell proc events...");
    sSpellMgr.LoadSpellProcEvents();
    sLog.outString("Loading spell proc item enchants...");
    sSpellMgr.LoadSpellProcItemEnchant();                   // must be after LoadSpellChains
    sLog.outString("Loading spell threats...");
    sSpellMgr.LoadSpellThreats();
    sLog.outString("Loading spell enchant charges...");
    sSpellMgr.LoadSpellEnchantCharges();
    sLog.outString("Loading NPC texts...");
    sObjectMgr.LoadNPCText();
    LoadRandomEnchantmentsTable();
    sLog.outString("Loading items...");
    sObjectMgr.LoadItemPrototypes();
    sLog.outString("Loading item texts...");
    sObjectMgr.LoadItemTexts();
    sLog.outString("Loading creature display data...");
    sObjectMgr.LoadCreatureDisplayInfoAddon();
    sLog.outString("Loading creature equipment templates...");
    sObjectMgr.LoadEquipmentTemplates();
    sLog.outString("Loading creature spells...");
    sObjectMgr.LoadCreatureSpells();
    sLog.outString("Loading creature templates...");
    sObjectMgr.LoadCreatureTemplates();
    sLog.outString("Loading spell script targets...");
    sSpellMgr.LoadSpellScriptTarget();                      // must be after LoadCreatureTemplates and LoadGameobjectInfo
    sLog.outString("Loading item required targets...");
    sObjectMgr.LoadItemRequiredTarget();
    sLog.outString("Loading reputation reward rates...");
    sObjectMgr.LoadReputationRewardRate();
    sLog.outString("Loading reputation reward values...");
    sObjectMgr.LoadReputationOnKill();
    sLog.outString("Loading reputation spillover templates...");
    sObjectMgr.LoadReputationSpilloverTemplate();
    sLog.outString("Loading points of interest...");
    sObjectMgr.LoadPointsOfInterest();
    sLog.outString("Loading defaul pet spells...");
    sObjectMgr.LoadPetCreateSpells();
    sLog.outString("Loading creatures...");
    sObjectMgr.LoadCreatures();
    sLog.outString("Loading creature addons...");
    sObjectMgr.LoadCreatureAddons();                        // must be after LoadCreatureTemplates() and LoadCreatures()
    sCreatureGroupsManager.Load();
    sLog.outString("Loading gameobjects...");
    sObjectMgr.LoadGameobjects();
    sLog.outString("Loading gameobject requirements...");
    sObjectMgr.LoadGameobjectsRequirements();
    sLog.outString("Loading gameobject display data...");
    sObjectMgr.LoadGameObjectDisplayInfoAddon();
    sCreatureLinkingMgr.LoadFromDB();
    sLog.outString("Loading object pooling data...");
    sPoolMgr.LoadFromDB();
    sLog.outString("Loading weather...");
    sWeatherMgr.LoadWeatherZoneChances();
    sLog.outString("Loading quests...");
    sObjectMgr.LoadQuests();                                // must be loaded after DBCs, creature_template, item_template, gameobject tables
    sLog.outString("Loading quest relations...");
    sObjectMgr.LoadQuestRelations();                        // must be after quest load
    sLog.outString("Loading quest greetings...");
    sObjectMgr.LoadQuestGreetings();
    sLog.outString("Loading trainer greetings...");
    sObjectMgr.LoadTrainerGreetings();
    sGameEventMgr.LoadFromDB();
    sLog.outString("Loading scaling...");
    sGuidObjectScaling.LoadFromDB();
    sLog.outString("Loading conditions...");
    sObjectMgr.LoadConditions();
    sLog.outString("Loading creature respawn timers...");
    sMapPersistentStateMgr.LoadCreatureRespawnTimes();
    sLog.outString("Loading gameobject respawn timers...");
    sMapPersistentStateMgr.LoadGameobjectRespawnTimes();
    sLog.outString("Loading spell areas...");
    sSpellMgr.LoadSpellAreas();
    sLog.outString("Loading area trigger teleports...");
    sObjectMgr.LoadAreaTriggerTeleports();                  // must be after item template load
    sLog.outString("Loading quest area trigger...");
    sObjectMgr.LoadQuestAreaTriggers();                     // must be after LoadQuests
    sLog.outString("Loading tavern area triggers...");
    sObjectMgr.LoadTavernAreaTriggers();
    sLog.outString("Loading battlegroun entry triggers...");
    sObjectMgr.LoadBattlegroundEntranceTriggers();
    sLog.outString("Loading area trigger scripts...");
    sScriptMgr.LoadAreaTriggerScripts();
    sLog.outString("Loading event ID scripts...");
    sScriptMgr.LoadEventIdScripts();
    sLog.outString("Loading graveyard zones...");
    sObjectMgr.LoadGraveyardZones();
    sLog.outString("Loading spell target positions...");
    sSpellMgr.LoadSpellTargetPositions();
    sLog.outString("Loading spell affects...");
    sSpellMgr.LoadSpellAffects();
    sLog.outString("Loading spell pet auras...");
    sSpellMgr.LoadSpellPetAuras();
    sLog.outString("Loading player info...");
    sObjectMgr.LoadPlayerInfo();
    sLog.outString("Loading exploration base XP...");
    sObjectMgr.LoadExplorationBaseXP();
    sLog.outString("Loading pet names...");
    sObjectMgr.LoadPetNames();
    sLog.outString("Initiating a character DB cleanup...");
    CharacterDatabaseCleaner::CleanDatabase();
    sLog.outString("Loading character cache data...");
    sObjectMgr.LoadPlayerCacheData();
    sLog.outString("Loading quest spell cast objectives...");
    sObjectMgr.LoadQuestSpellCastObjectives(); // Must be after LoadQuests and LoadPlayerCacheData
    HonorMgr::LoadMostDkHkYesterdayPlayers();
    sLog.outString("Loading the max pet number...");
    sObjectMgr.LoadPetNumber();
    sLog.outString("Loading pet level stats...");
    sObjectMgr.LoadPetLevelInfo();
    sLog.outString("Loading player corpses...");
	sObjectMgr.LoadCorpses();
    sLog.outString("Loading loot tables...");
    LootIdSet ids_set;
    LoadLootTables(ids_set);
    sLog.outString("Loading custom character skins...");
    sObjectMgr.LoadCustomCharacterSkins();
    sLog.outString("Loading fishing base level requirements...");
    sObjectMgr.LoadFishingBaseSkillLevel();
    sLog.outString("Loading NPC gossips...");
    sObjectMgr.LoadNpcGossips();                            // must be after load Creature and LoadNPCText
    sLog.outString("Loading gossip scripts...");
    sScriptMgr.LoadGossipScripts();                         // must be before gossip menu options
    sObjectMgr.LoadGossipMenus();
    sLog.outString("Loading vendor templates...");
    sObjectMgr.LoadVendorTemplates();                       // must be after load ItemTemplate
    sLog.outString("Loading vendors...");
    sObjectMgr.LoadVendors();                               // must be after load CreatureTemplate, VendorTemplate, and ItemTemplate
    sLog.outString("Loading custom merchants...");
    sCustomMerchantMgr.Load();
    sLog.outString("Loading trainer templates...");
    sObjectMgr.LoadTrainerTemplates();                      // must be after load CreatureTemplate
    sLog.outString("Loading trainers...");
    sObjectMgr.LoadTrainers();                              // must be after load CreatureTemplate, TrainerTemplate
    sLog.outString("Loading creature movement scripts...");
    sScriptMgr.LoadCreatureMovementScripts();
    sLog.outString("Loading waypoints...");
    sWaypointMgr.Load();
    sLog.outString("Loading localization data...");
    sObjectMgr.LoadBroadcastTextLocales();
    sObjectMgr.LoadCreatureLocales();                       // must be after CreatureInfo loading
    sObjectMgr.LoadGameObjectLocales();                     // must be after GameobjectInfo loading
    sObjectMgr.LoadItemLocales();                           // must be after ItemPrototypes loading
    sObjectMgr.LoadQuestLocales();                          // must be after QuestTemplates loading
    sObjectMgr.LoadPageTextLocales();                       // must be after PageText loading
    sObjectMgr.LoadGossipMenuItemsLocales();                // must be after gossip menu items loading
    sObjectMgr.LoadPointOfInterestLocales();                // must be after POI loading
    sObjectMgr.LoadAreaLocales();
    sObjectMgr.LoadCartographerAreas();
    sLog.outString("Loading auction houses...");	
	sAuctionMgr.LoadAuctionHouses();
    sLog.outString("Loading auction items...");
	sAuctionMgr.LoadAuctionItems();
    sLog.outString("Loading auctions...");
	sAuctionMgr.LoadAuctions();
    sLog.outString("Loading guilds...");
	sGuildMgr.LoadGuilds();
    sLog.outString("Loading guild petisions...");
    sObjectMgr.LoadGuildHouses();
    sLog.outString("Loading guild houses...");
	sGuildMgr.LoadPetitions();
    sLog.outString("Loading groups...");
	sObjectMgr.LoadGroups();
    sLog.outString("Loading reserved player names...");
	sObjectMgr.LoadReservedPlayersNames();
    sLog.outString("Loading gameobjects for quests...");
    sObjectMgr.LoadGameObjectForQuests();
    sLog.outString("Loading battlemasters...");
    sBattleGroundMgr.LoadBattleMastersEntry();
    sLog.outString("Loading battle events...");
    sBattleGroundMgr.LoadBattleEventIndexes();
    sLog.outString("Loading GM telepors...");
    sObjectMgr.LoadGameTele();
    sLog.outString("Loading taxi path transitions...");
    sObjectMgr.LoadTaxiPathTransitions();
    sLog.outString("Loading tickets...");
	sTicketMgr.Initialize();
	sTicketMgr.LoadTickets();
    sLog.outString("Loading surveys...");
	sTicketMgr.LoadSurveys();
    sTicketMgr.LoadTicketTemplates();
    sLog.outString("Returning old mails...");
	sObjectMgr.ReturnOrDeleteOldMails(false);
    sLog.outString("Loading quest start scripts...");
    sScriptMgr.LoadQuestStartScripts();                     // must be after load Creature/Gameobject(Template/Data) and QuestTemplate
    sLog.outString("Loading quest completion scripts...");
    sScriptMgr.LoadQuestEndScripts();                       // must be after load Creature/Gameobject(Template/Data) and QuestTemplate
    sLog.outString("Loading spell scripts...");
    sScriptMgr.LoadSpellScripts();                          // must be after load Creature/Gameobject(Template/Data)
    sLog.outString("Loading creature spell scripts...");
    sScriptMgr.LoadCreatureSpellScripts();
    sLog.outString("Loading gameobject scripts...");
    sScriptMgr.LoadGameObjectScripts();                     // must be after load Creature/Gameobject(Template/Data)
    sLog.outString("Loading event scripts...");
    sScriptMgr.LoadEventScripts();                          // must be after load Creature/Gameobject(Template/Data)
    sLog.outString("Loading generic scripts...");
    sScriptMgr.LoadGenericScripts();
    sLog.outString("Loading creature EventAI scripts...");
    sScriptMgr.LoadCreatureEventAIScripts();
    sScriptMgr.CheckAllScriptTexts();
    sLog.outString("Loading creature EventAI events...");
    sEventAIMgr.LoadCreatureEventAI_Events();
    sScriptMgr.Initialize();
    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_LOAD_CUSTOM_DATABASE_TABLE, [](WorldScript* script)
    {
        script->OnLoadCustomDatabaseTable();
    });
    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_BEFORE_WORLD_INITIALIZED, [](WorldScript* script)
    {
        script->OnBeforeWorldInitialized();
    });
    sLog.outString("Loading aura removal handler...");
    sAuraRemovalMgr.LoadFromDB();
    sLog.outString("Loading daily quests handler...");
    sDailyQuestHandler.LoadFromDB(true);
    // sLog.outString("Resetting yearly quests...");
    // sObjectMgr.ResetYearlyQuests();
    sLog.outString("Loading companion manager...");
    sCompanionMgr.LoadFromDB();
    sLog.outString("Loading mount manager...");
    sMountMgr.LoadFromDB();
    sLog.outString("Loading toy manager...");
    sToyMgr.LoadFromDB();
	///- Loading shop tables
	sLog.outString("Loading shop...");
	sObjectMgr.LoadShop();
	sLog.outString("Initialize Performance monitor...");
    sPerfMonitor.Initialize();

    sLog.outString("Loading dynamic visibility templates...");
    sDynamicVisMgr.LoadFromDB(false);

    sLog.outString("Loading cached Account data...");
    LoadAccountData();

    ///- Initialize game time and timers
    m_gameTime = time(nullptr);
    m_startTime = m_gameTime;
    m_gameDay = (m_gameTime + m_timeZoneOffset) / DAY;

    tm local;
    time_t curr;
    time(&curr);
    local = *(localtime(&curr));                            // dereference and assign
    char isoDate[128];
    sprintf(isoDate, "%04d-%02d-%02d %02d:%02d:%02d",
            local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);

    LoginDatabase.PExecute("INSERT INTO uptime (`realmid`, `starttime`, `startstring`, `uptime`, `revision`) VALUES('%u', " UI64FMTD ", '%s', 0, '%s')",
                           realmID, uint64(m_startTime), isoDate, REVISION_HASH);

    // Only store destroyed items for 30 days.
    CharacterDatabase.PExecute("DELETE FROM `character_destroyed_items` WHERE (`time` + %u) < " UI64FMTD, 30 * DAY, m_startTime);

    m_timers[WUPDATE_AUCTIONS].SetInterval(MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_UPTIME].SetInterval(getConfig(CONFIG_UINT32_UPTIME_UPDATE)*MINUTE * IN_MILLISECONDS);
    //Update "uptime" table based on configuration entry in minutes.
    m_timers[WUPDATE_CORPSES].SetInterval(20 * MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_SAVE_VAR].SetInterval(MINUTE * IN_MILLISECONDS);

    // Update groups with offline leader after delay in seconds
    m_timers[WUPDATE_GROUPS].SetInterval(IN_MILLISECONDS);

    ///- Initialize static helper structures
    AIRegistry::Initialize();
    Player::InitVisibleBits();
    sLog.outString("Loading gameobject model list...");
    LoadGameObjectModelList();

    sLog.outString("Initiating map manager...");
    sMapMgr.Initialize();
    sLog.outString("Deleting expired bans...");
    sBattleGroundMgr.CreateInitialBattleGrounds();
    CheckLootTemplates_Reference(ids_set);
    sLog.outString("Initiating zone scripts...");
    sZoneScriptMgr.InitZoneScripts();
    sLog.outString("Loading transport on continents...");
	sTransportMgr.SpawnContinentTransports();
    sLog.outString("Deleting expired bans...");
	LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");
    sLog.outString("Initiating honor maintenance...");
	sHonorMaintenancer.Initialize();
	sHonorMaintenancer.DoMaintenance();

    uint32 nextGameEvent = sGameEventMgr.Initialize();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);    //depend on next event

    sLog.outString("Loading disable spells...");
    sObjectMgr.LoadSpellDisabledEntrys();
    sLog.outString("Loading anticheat...");
    sAnticheatLib->Initialize();
    sLog.outString("Loading autobroadcat...");
	sAutoBroadCastMgr.Load();
    sLog.outString("Loading player phasing...");
	sObjectMgr.LoadPlayerPhaseFromDb();
    sLog.outString("Caching player pets...");
	sCharacterDatabaseCache.LoadAll();
    sLog.outString("Loading faction change reputations...");
	sObjectMgr.LoadFactionChangeReputations();
    sLog.outString("Loading faction change spells...");
	sObjectMgr.LoadFactionChangeSpells();
    sLog.outString("Loading faction change items...");
	sObjectMgr.LoadFactionChangeItems();
    sLog.outString("Loading faction change quests...");
	sObjectMgr.LoadFactionChangeQuests();
    sLog.outString("Loading faction change mounts...");
	sObjectMgr.LoadFactionChangeMounts();
    sLog.outString("Loading loot-disable map list...");
    sObjectMgr.LoadMapLootDisabled();
    sLog.outString("Loading cinematic waypoints...");
    sObjectMgr.LoadCinematicsWaypoints();
    sLog.outString("Loading transmogrification templates...");
    if (sWorld.getConfig(CONFIG_BOOL_TRANSMOG_ENABLED) || true) //temp, idk if this is enabled on ptr
    {
        sLog.outString("Loading transmogrification templates...");
        sObjectMgr.LoadItemTransmogrifyTemplates();
    }
    sLog.outString("Loading spell groups...");
    sSpellMgr.LoadSpellGroups();
    sLog.outString("Loading spell group stack rules...");
    sSpellMgr.LoadSpellGroupStackRules();

    sLog.outInfo("Beginning inactive character deletion...");
    CharacterDatabaseCleaner::DeleteInactiveCharacters();

    sLog.outInfo("Loading active players per faction...");
    sObjectMgr.LoadActivePlayersPerFaction();

    sLog.outInfo("Loading RBAC for chat commands...");
    ChatHandler::LoadRbacPermissions();

    if (getConfig(CONFIG_BOOL_RESTORE_DELETED_ITEMS))
    {
        sLog.outString("Restoring deleted items...");
        sObjectMgr.RestoreDeletedItems();
    }

    sLog.outString("Loading shell coins...");
    sObjectMgr.LoadShellCoinCount();
    m_lastShellCoinPrice = sObjectMgr.GetShellCoinBuyPrice();
    
    m_broadcaster =
        std::make_unique<MovementBroadcaster>(sWorld.getConfig(CONFIG_UINT32_PACKET_BCAST_THREADS),
                                              std::chrono::milliseconds(sWorld.getConfig(CONFIG_UINT32_PACKET_BCAST_FREQUENCY)));

    m_ChannelBroadcaster = std::make_unique<ChannelBroadcaster>();
    m_charDbWorkerThread.reset(new std::thread(&charactersDatabaseWorkerThread));
    m_autoPDumpThread = std::thread(&World::AutoPDumpWorker, this);
    m_asyncPacketsThread = std::thread(&World::ProcessAsyncPackets, this);
    m_shopThread = std::thread(&ShopMgr::ProcessRequestsWorker, &sShopMgr);

    DeleteOldPDumps();

#ifdef USE_ANTICHEAT
	sSuspiciousStatisticMgr.Initialize();
#endif

    if (getConfig(CONFIG_UINT32_AUTO_COMMIT_MINUTES))
        m_autoCommitThread = std::thread(&autoCommitWorkerThread);

	uint32 uTransmogFillStartTime = WorldTimer::getMSTime();
	sObjectMgr.FillPossibleTransmogs();
	uint32 uTransmogFillDuration = WorldTimer::getMSTimeDiff(uTransmogFillStartTime, WorldTimer::getMSTime());
	sLog.outString("Loading possible transmogs: %i minutes %i seconds", uTransmogFillDuration / 60000, (uTransmogFillDuration % 60000) / 1000);

    AccountAnalyser::CheckExtendedHashes();

    LoadPlayerEggLoot();

    sLog.outString("Fixing Hardcore Guild bank items..");
    sGuildMgr.FixupInfernoBanks();

#ifdef USING_DISCORD_BOT
    sLog.outString("Loading Discord Bot...");

    auto token = sConfig.GetStringDefault("DiscordBot.Token", "");
    if (!token.empty())
    {
        DiscordBot::RegisterHandlers();
        sDiscordBot->Setup(token);
    }
#endif

    {
        std::ofstream honorUpdateFile{ "honorupdate.txt" };
        if (honorUpdateFile)
            honorUpdateFile << "0";
    }

    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_STARTUP, [](WorldScript* script)
    {
        script->OnStartup();
    });

    sLog.outString("Current content phase is set to %u.", GetContentPhase() + 1);
    uint32 uStartInterval = WorldTimer::getMSTimeDiff(uStartTime, WorldTimer::getMSTime());
    sLog.outString("World server is up and running! Loading time: %i minutes %i seconds", uStartInterval / 60000, (uStartInterval % 60000) / 1000);
}

void World::DetectDBCLang()
{
    uint32 m_lang_confid = sConfig.GetIntDefault("DBC.Locale", 255);

    if (m_lang_confid != 255 && m_lang_confid >= MAX_DBC_LOCALE)
    {
        sLog.outError("Incorrect DBC.Locale! Must be >= 0 and < %d (set to 0)", MAX_DBC_LOCALE);
        m_lang_confid = LOCALE_enUS;
    }

    ChrRacesEntry const* race = sChrRacesStore.LookupEntry(RACE_HUMAN);
    MANGOS_ASSERT(race);

    std::string availableLocalsStr;

    uint32 default_locale = MAX_DBC_LOCALE;
    for (int i = MAX_DBC_LOCALE - 1; i >= 0; --i)
    {
        if (strlen(race->name[i]) > 0)                      // check by race names
        {
            default_locale = i;
            m_availableDbcLocaleMask |= (1 << i);
            availableLocalsStr += localeNames[i];
            availableLocalsStr += " ";
        }
    }

    if (default_locale != m_lang_confid && m_lang_confid < MAX_DBC_LOCALE &&
            (m_availableDbcLocaleMask & (1 << m_lang_confid)))
        default_locale = m_lang_confid;

    if (default_locale >= MAX_DBC_LOCALE)
    {
        sLog.outError("Unable to determine your DBC Locale! (corrupt DBC?)");
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    m_defaultDbcLocale = LocaleConstant(default_locale);

    sLog.outString("Using %s DBC locale as default.", localeNames[m_defaultDbcLocale]);
}

void World::ApiServerDeleter::operator()(HttpApi::ApiServer* p)
{
    delete p;
}

void World::ProcessAsyncPackets()
{
    thread_name("AsyncPackets");
    while (!sWorld.IsStopped())
    {
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        } while (!m_canProcessAsyncPackets);
        
        for (auto const& itr : m_sessions)
        {
            WorldSession* pSession = itr.second;

            MapSessionFilter updater(pSession);
            updater.SetProcessType(PACKET_PROCESS_DB_QUERY);
            pSession->ProcessPackets(updater);

            if (!m_canProcessAsyncPackets)
                break;
        }
    }
}

void TotalMoneyCallback(QueryResult* result, uint32 money)
{
    if (!result)
        return;

    CharacterDatabase.PExecute("INSERT INTO characters_total_money (total_gold) VALUES (%u)", (*result)[0].GetUInt32());
    delete result;
}


/// Update the World !
void World::Update(uint32 diff)
{
    XScopeStatTimer ScopeStatTimer(sPerfMonitor.WorldTick);

    ///- Update the different timers
    for (auto& timer : m_timers)
    {
        if (timer.GetCurrent() >= 0)
            timer.Update(diff);
        else
            timer.SetCurrent(0);
    }

    ///- Update the game time and check for shutdown time
    _UpdateGameTime();

    ///-Update mass mailer tasks if any
    sMassMailMgr.Update();

    /// <ul><li> Handle auctions when the timer has passed
    if (m_timers[WUPDATE_AUCTIONS].Passed())
    {
        m_timers[WUPDATE_AUCTIONS].Reset();
        ///- Handle expired auctions
        sAuctionMgr.Update();
    }

    if (m_timers[WUPDATE_TOTAL_MONEY].Passed())
    {
        m_timers[WUPDATE_TOTAL_MONEY].Reset();
        uint32 money = 0;
        CharacterDatabase.AsyncPQuery(&TotalMoneyCallback, money, "SELECT ROUND(SUM(money) / 10000) FROM characters");
    }

    m_canProcessAsyncPackets = false;

    if (m_timers[WUPDATE_COMMANDS].Passed())
    {
        m_timers[WUPDATE_COMMANDS].Reset();
        LoginDatabase.AsyncPQuery(this, &World::LoadPendingCommands, "SELECT `id`, `command` FROM `pending_commands` WHERE `realm_id`=%u && `run_at_time` <= %u", realmID, GetGameTime());
    }

    /// <li> Handle session updates
    UpdateSessions(diff);
    m_canProcessAsyncPackets = true;

    /// <li> Update uptime table
    if (m_timers[WUPDATE_UPTIME].Passed())
    {
        uint32 tmpDiff = uint32(m_gameTime - m_startTime);
        uint32 onlineClientsNum = GetActiveSessionCount();
        uint32 maxClientsNum = GetMaxActiveSessionCount();
        uint32 queuedClientsNum = GetQueuedSessionCount();
        uint32 maxQueuedClientsNum = GetMaxQueuedSessionCount();

        m_timers[WUPDATE_UPTIME].Reset();
        LoginDatabase.PExecute("UPDATE `uptime` SET `uptime` = %u, `onlineplayers` = %u, `maxplayers` = %u, `queue` = %u, `maxqueue` = %u WHERE `realmid` = %u AND `starttime` = " UI64FMTD, tmpDiff, onlineClientsNum, maxClientsNum, queuedClientsNum, maxQueuedClientsNum, realmID, uint64(m_startTime));
    }

    ///- Update objects (maps, transport, creatures,...)
    uint32 updateMapSystemTime = WorldTimer::getMSTime();
    //TODO: find a better place for this
    if (!m_updateThreads)
    {
        m_updateThreads = std::unique_ptr<ThreadPool>( new ThreadPool(
                    getConfig(CONFIG_UINT32_ASYNC_TASKS_THREADS_COUNT),"WorldAsync",
                    ThreadPool::ClearMode::UPPON_COMPLETION)
                                             );
        m_updateThreads->start<ThreadPool::MySQL<>>();
    }
    std::unique_lock<std::mutex> lock(m_asyncTaskQueueMutex);
    _asyncTasks.swap(_asyncTasksBusy);
    std::future<void> job = m_updateThreads->processWorkload(_asyncTasksBusy);
    _asyncTasks.clear();
    lock.unlock();

    sTransportMgr.Update(diff);
    sMapMgr.Update(diff);
    sBattleGroundMgr.Update(diff);
    sLFGMgr.Update(diff);
    sLFTMgr.Update(diff);
    sGuardMgr.Update(diff);
    sZoneScriptMgr.Update(diff);
    sDynamicVisMgr.UpdateVisibility(diff);

    ///- Update groups with offline leaders
    if (m_timers[WUPDATE_GROUPS].Passed())
    {
        m_timers[WUPDATE_GROUPS].Reset();
        if (const uint32 delay = getConfig(CONFIG_UINT32_GROUP_OFFLINE_LEADER_DELAY))
        {
            for (ObjectMgr::GroupMap::const_iterator i = sObjectMgr.GetGroupMapBegin(); i != sObjectMgr.GetGroupMapEnd(); ++i)
                i->second->UpdateOfflineLeader(m_gameTime, delay);
        }
    }

    uint32 asyncWaitBegin = WorldTimer::getMSTime();
    if (job.valid())
        job.wait();

    updateMapSystemTime = WorldTimer::getMSTimeDiffToNow(updateMapSystemTime);
    if (getConfig(CONFIG_UINT32_PERFLOG_SLOW_MAPSYSTEM_UPDATE) && updateMapSystemTime > getConfig(CONFIG_UINT32_PERFLOG_SLOW_MAPSYSTEM_UPDATE))
        sLog.out(LOG_PERFORMANCE, "Update map system: %ums [%ums for async]", updateMapSystemTime, WorldTimer::getMSTimeDiffToNow(asyncWaitBegin));

    ///- Sauvegarde des variables internes (table variables) : MaJ par rapport a la DB
    if (m_timers[WUPDATE_SAVE_VAR].Passed())
    {
        m_timers[WUPDATE_SAVE_VAR].Reset();
        sObjectMgr.SaveVariables();
    }

    // execute callbacks from sql queries that were queued recently
    uint32 asyncQueriesTime = WorldTimer::getMSTime();
    UpdateResultQueue();
    asyncQueriesTime = WorldTimer::getMSTimeDiffToNow(asyncQueriesTime);
    if (getConfig(CONFIG_UINT32_PERFLOG_SLOW_ASYNC_QUERIES) && asyncQueriesTime > getConfig(CONFIG_UINT32_PERFLOG_SLOW_ASYNC_QUERIES))
        sLog.out(LOG_PERFORMANCE, "Update async queries: %ums", asyncQueriesTime);

    ///- Erase old corpses
    if (m_timers[WUPDATE_CORPSES].Passed())
    {
        m_timers[WUPDATE_CORPSES].Reset();

        sObjectAccessor.RemoveOldCorpses();
    }

    ///- Process Game events when necessary
    if (m_timers[WUPDATE_EVENTS].Passed())
    {
        m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
        uint32 nextGameEvent = sGameEventMgr.Update();
        m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
        m_timers[WUPDATE_EVENTS].Reset();
    }

    if (m_timers[WUPDATE_CENSUS].Passed())
    {
        m_timers[WUPDATE_CENSUS].Reset();

        uint32 alliancePlayers = 0, hordePlayers = 0;

        const SessionMap& players = sWorld.GetAllSessions();
        for (const auto& [id, session] : players)
        {
            auto player = session->GetPlayer();
            if (!player || !player->IsInWorld())
                continue;

            if (player->GetTeamId() == TEAM_HORDE)
                ++hordePlayers;
            else
                ++alliancePlayers;
        }

        WorldDatabase.PExecute("INSERT INTO `player_census` (`alliance_players`, `horde_players`, `total_players`, `date_time`) VALUES (%u, %u, %u, NOW())", alliancePlayers,
            hordePlayers, hordePlayers + alliancePlayers);
    }

    if (m_timers[WUPDATE_SHELLCOIN].Passed())
    {
        m_timers[WUPDATE_SHELLCOIN].Reset();

        int32 buyPrice = sObjectMgr.GetShellCoinBuyPrice();
        int32 coinCount = sObjectMgr.GetShellCoinCount();

        if (m_lastShellCoinPrice)
        {
            std::string message;
            if (buyPrice > m_lastShellCoinPrice)
                message = "Shellcoin price has increased to " + MoneyToString(buyPrice) + " (up " + std::to_string(int32((float(buyPrice) / float(m_lastShellCoinPrice)) * 100.0f - 100.0f)) + "%).";
            else if (buyPrice < m_lastShellCoinPrice)
                message = "Shellcoin price has decreased to " + MoneyToString(buyPrice) + " (down " + std::to_string(int32(100.0f - (float(buyPrice) / float(m_lastShellCoinPrice)) * 100.0f)) + "%).";

            if (!message.empty())
            {
                for (auto const& guid : m_shellCoinOwners)
                {
                    if (Player* pPlayer = sObjectAccessor.FindPlayer(guid))
                    {
                        if (pPlayer->IsInWorld())
                            ChatHandler(pPlayer).SendSysMessage(message.c_str());
                    }
                }
            }
        }

        CharacterDatabase.PExecute("INSERT INTO `logs_shellcoin` (`time`, `count`, `price`) VALUES (%u, %u, %u)",
            m_gameTime, coinCount, buyPrice);

        m_lastShellCoinPrice = buyPrice;
    }

    /// </ul>
    ///- Move all creatures with "delayed move" and remove and delete all objects with "delayed remove"
    sMapMgr.RemoveAllObjectsInRemoveList();

    // update the instance reset times
    sMapPersistentStateMgr.Update();

    /// Maintenance checker
    if (m_MaintenanceTimeChecker < diff)
    {
        sHonorMaintenancer.CheckMaintenanceDay();
        m_MaintenanceTimeChecker = 60000;
    }
    else
        m_MaintenanceTimeChecker -= diff;

    // Update AutoBroadcast
    sAutoBroadCastMgr.Update(diff);
    // Update liste des ban si besoin
    sAccountMgr.Update(diff);

    sDailyQuestHandler.Update(diff);

    uint32 popCount = sWorld.GetActiveSessionCount();
    if (!popCount)
        popCount = 1;

    if (popCount > sWorld.getConfig(CONFIG_UINT32_DYNAMIC_SCALING_POP))
        m_dynamicRespawnRatio = float(sWorld.getConfig(CONFIG_UINT32_DYNAMIC_SCALING_POP) / float(popCount));
    else
        m_dynamicRespawnRatio = 1.f;

    // And last, but not least handle the issued cli commands
    ProcessCliCommands();

    //cleanup unused GridMap objects as well as VMaps
    if (getConfig(CONFIG_BOOL_CLEANUP_TERRAIN))
        sTerrainMgr.Update(diff);

	sGuildMgr.Update(diff);

    if (!m_ShutdownTimer && !m_stopEvent &&
        getConfig(CONFIG_UINT32_AUTO_RESTART_MAX_SERVER_UPTIME) &&
        getConfig(CONFIG_UINT32_AUTO_RESTART_MAX_SERVER_UPTIME) < GetUptime() &&
        GetGameDay() != sHonorMaintenancer.GetNextMaintenanceDay())
    {
        struct tm* tm_struct = localtime(&m_gameTime);
        if (tm_struct->tm_hour >= getConfig(CONFIG_UINT32_AUTO_RESTART_HOUR_MIN) && tm_struct->tm_hour <= getConfig(CONFIG_UINT32_AUTO_RESTART_HOUR_MAX))
        {
            sLog.outInfo("Restarting server due to exceeding maximum uptime.");
            sWorld.ShutdownServ(900, SHUTDOWN_MASK_RESTART, SHUTDOWN_EXIT_CODE);
        }
    }

    // Moved here from the head of this function. Firing first meant a module acted
    // before UpdateSessions, sMapMgr, sBattleGroundMgr and sLFTMgr had run, so every
    // module tick decided on the PREVIOUS tick's world -- a module may hold several
    // WorldScripts and was steering sixty crews on stale positions.
    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_UPDATE, [&](WorldScript* script)
    {
        script->OnUpdate(diff);
    });
}

/// Send a packet to all players (except self if mentioned)
void World::SendGlobalMessage(WorldPacket *packet, WorldSession *self, uint32 team)
{
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            if (session != self)
            {
                Player* player = session->GetPlayer();
                if (player && player->IsInWorld() && (team == TEAM_NONE || player->GetTeam() == team))
                    session->SendPacket(packet);
            }
        }
    }
}

namespace MaNGOS
{

    void MaNGOS::WorldWorldTextBuilder::operator()(WorldPacketList& data_list, int32 loc_idx)
    {
        char const* text = sObjectMgr.GetMangosString(i_textId, loc_idx);

        if (i_args)
        {
            // we need copy va_list before use or original va_list will corrupted
            va_list ap;
            va_copy(ap, *i_args);

            char str[2048];
            vsnprintf(str, 2048, text, ap);
            va_end(ap);

            do_helper(data_list, &str[0]);
        }
        else
            do_helper(data_list, (char*)text);
    }

    void MaNGOS::WorldWorldTextBuilder::do_helper(WorldPacketList& data_list, char* text)
    {
        char* pos = text;

        while (char* line = lineFromMessage(pos))
        {
            WorldPacket* data = new WorldPacket();
            ChatHandler::BuildChatPacket(*data, CHAT_MSG_SYSTEM, line);
            data_list.push_back(data);
        }
    }
}

/// Send a System Message to all players (except self if mentioned)
void World::SendWorldText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    MaNGOS::WorldWorldTextBuilder wt_builder(string_id, &ap);
    LocalizedPacketListDo<MaNGOS::WorldWorldTextBuilder> wt_do(wt_builder);
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            Player* player = session->GetPlayer();
            if (player && player->IsInWorld())
                wt_do(player);
        }
    }

    va_end(ap);
}

void World::SendGMTicketText(const char* text)
{
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            if (session->GetSecurity() > SEC_PLAYER)
            {
                Player* player = session->GetPlayer();
                if (player && player->IsInWorld() && player->IsAcceptTickets())
                    ChatHandler(player).SendSysMessage(text);
            }
        }
    }
}

void World::SendGMTicketText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    MaNGOS::WorldWorldTextBuilder wt_builder(string_id, &ap);
    LocalizedPacketListDo<MaNGOS::WorldWorldTextBuilder> wt_do(wt_builder);
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            if (session->GetSecurity() > SEC_PLAYER)
            {
                Player* player = session->GetPlayer();
                if (player && player->IsInWorld() && player->IsAcceptTickets())
                {
                    wt_do(player);
                }
            }
        }
    }
}

void World::SendGMTextFlags(uint32 accountFlags, int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    MaNGOS::WorldWorldTextBuilder wt_builder(string_id, &ap);
    LocalizedPacketListDo<MaNGOS::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second ||
            !itr->second->GetPlayer() ||
            !itr->second->GetPlayer()->IsInWorld() ||
            itr->second->GetSecurity() == SEC_PLAYER ||
            !(itr->second->GetAccountFlags() & accountFlags))
            continue;

        wt_do(itr->second->GetPlayer());
    }

    va_end(ap);
}


void World::SendGMText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    MaNGOS::WorldWorldTextBuilder wt_builder(string_id, &ap);
    LocalizedPacketListDo<MaNGOS::WorldWorldTextBuilder> wt_do(wt_builder);
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            if (session->GetSecurity() > SEC_PLAYER)
            {
                Player* player = session->GetPlayer();
                if (player && player->IsInWorld())
                {
                    wt_do(player);
                }
            }
        }
    }
}

void World::SendGMText(const std::string& message, uint32 minGmLevel)
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, message);
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            if (session->GetSecurity() >= minGmLevel)
            {
                Player* player = session->GetPlayer();
                if (player && player->IsInWorld())
                {
                    player->SendDirectMessage(&data);
                }
            }
        }
    }
}

/// DEPRICATED, only for debug purpose. Send a System Message to all players (except self if mentioned)
void World::SendGlobalText(const char* text, WorldSession *self)
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = mangos_strdup(text);
    char* pos = buf;

    while (char* line = ChatHandler::LineFromMessage(pos))
    {
        ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, line);
        SendGlobalMessage(&data, self);
    }

    delete [] buf;
}

/// Send a packet to all players (or players selected team) in the zone (except self if mentioned)
void World::SendZoneMessage(uint32 zone, WorldPacket *packet, WorldSession *self, uint32 team)
{
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            if (session != self)
            {
                Player* player = session->GetPlayer();
                if (player && player->IsInWorld() &&
                   (player->GetZoneId() == zone) &&
                   (team == TEAM_NONE || player->GetTeam() == team))
                {
                    session->SendPacket(packet);
                }
            }
        }
    }
}

/// Send a System Message to all players in the zone (except self if mentioned)
void World::SendZoneText(uint32 zone, const char* text, WorldSession *self, uint32 team)
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, text);
    SendZoneMessage(zone, &data, self, team);
}

void World::SendHardcoreMessage(WorldPacket* packet, WorldSession* self)
{
    for (const auto& itr : m_sessions)
    {
        if (WorldSession* session = itr.second)
        {
            if (session != self)
            {
                Player* player = session->GetPlayer();

                // base check
                if (player && player->IsInWorld() && player->IsHCChat() &&
                   ((player->IsHardcore() || player->IsHC60()) || player->GetSession()->GetSecurity() > SEC_PLAYER))
                {
                    // social check
                    if (player->GetSocial() && !player->GetSocial()->HasIgnore(self->GetPlayer()->GetObjectGuid()))
                        session->SendPacket(packet);          
                }
            }
            else
                self->SendPacket(packet);
        }
    }
}

/// Kick (and save) all players
void World::KickAll()
{
    m_QueuedSessions.clear();                               // prevent send queue update packet and login queued sessions
    m_priorityQueue[0].clear();
    m_priorityQueue[1].clear();

    // session not removed at kick and will removed in next update tick
    for (const auto& itr : m_sessions)
        itr.second->KickPlayer();
    for (const auto& itr : m_disconnectedSessions)
        (*itr).KickPlayer();
}

/// Kick (and save) all players with security level less `sec`
void World::KickAllLess(AccountTypes sec)
{
    // session not removed at kick and will removed in next update tick
    for (const auto& itr : m_sessions)
        if (WorldSession* session = itr.second)
            if (session->GetSecurity() < sec)
                session->KickPlayer();
}

void World::WarnAccount(uint32 accountId, std::string from, std::string reason, const char* type)
{
    LoginDatabase.escape_string(from);
    reason = std::string(type) + ": " + reason;
    LoginDatabase.escape_string(reason);

    LoginDatabase.PExecute("INSERT INTO account_banned (id, bandate, unbandate, bannedby, banreason, active, realm) VALUES ('%u', UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+1, '%s', '%s', '0', %u)",
        accountId, from.c_str(), reason.c_str(), realmID);
}

void World::RemoveWarning(uint32 warningId)
{
    LoginDatabase.PExecute("DELETE FROM account_banned WHERE banid = %u", warningId);
}

void World::BanAccount(uint32 accountId, uint32 duration, std::string reason, std::string const& author)
{
    LoginDatabase.escape_string(reason);
    std::string safe_author = author;
    LoginDatabase.escape_string(safe_author);

    //No SQL injection as strings are escaped
    LoginDatabase.PExecute("INSERT INTO account_banned (id, bandate, unbandate, bannedby, banreason, active, realm) VALUES ('%u', UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+%u, '%s', '%s', '1', %u)",
        accountId, duration, safe_author.c_str(), reason.c_str(), realmID);

    if (duration > 0)
        sAccountMgr.BanAccount(accountId, time(nullptr) + duration);
    else
        sAccountMgr.BanAccount(accountId, 0xFFFFFFFF);

    StopHeadlessSessionsForAccount(accountId, true);

    if (WorldSession* sess = FindSession(accountId))
    {
        if (std::string(sess->GetPlayerName()) != author)
        {
            sess->LogoutPlayer(true);
            sess->KickPlayer();
        }
    }
}

class BanQueryHolder : public SqlQueryHolder
{
public:
    BanQueryHolder(BanMode mode, std::string banTarget, uint32 duration, std::string reason, uint32 realmId, std::string author, uint32 authorAccountId) :
        m_mode(mode), m_duration(duration), m_reason(reason), m_realmId(realmId), m_author(author), m_banTarget(banTarget), m_accountId(authorAccountId) { }

    BanMode GetBanMode() const { return m_mode; }
    uint32 GetDuration() const { return m_duration; }
    std::string& GetReason() { return m_reason; }
    uint32 GetRealmId() const { return m_realmId; }
    std::string& GetAuthor() { return m_author; }
    std::string& GetBanTarget() { return m_banTarget; }
    uint32 GetAuthorAccountId() const { return m_accountId; }

private:
    BanMode m_mode;
    uint32 m_duration;
    std::string m_reason;
    uint32 m_realmId;
    std::string m_author;
    std::string m_banTarget;
    uint32 m_accountId;
};

// Not thread-safe, performed in async unsafe callback
class BanAccountHandler
{
public:
    void HandleAccountSelectResult(QueryResult*, SqlQueryHolder* queryHolder)
    {
        BanQueryHolder* holder = static_cast<BanQueryHolder*>(queryHolder);
        if (!holder)
            return;

        BanReturn banResult = BAN_SUCCESS;

        WorldSession* session = sWorld.FindSession(holder->GetAuthorAccountId());

        QueryResult* result = holder->GetResult(0);
        if (!result)
        {
            if (session)
                ChatHandler(session).SendBanResult(holder->GetBanMode(), BAN_NOTFOUND, holder->GetBanTarget(), holder->GetDuration(), holder->GetReason());
            delete holder;
            return;
        }

        ///- Disconnect all affected players (for IP it can be several)
        do
        {
            Field* fieldsAccount = result->Fetch();
            uint32 account = fieldsAccount->GetUInt32();

            if (holder->GetBanMode() != BAN_IP)
            {
                //No SQL injection as strings are escaped
                LoginDatabase.PExecute("INSERT INTO account_banned (id, bandate, unbandate, bannedby, banreason, active, realm) VALUES ('%u', UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+%u, '%s', '%s', '1', %u)",
                    account, holder->GetDuration(), holder->GetAuthor().c_str(), holder->GetReason().c_str(), holder->GetRealmId());
                if (holder->GetDuration() > 0)
                    sAccountMgr.BanAccount(account, time(nullptr) + holder->GetDuration());
                else
                    sAccountMgr.BanAccount(account, 0xFFFFFFFF);

                sWorld.StopHeadlessSessionsForAccount(account, true);
            }
            // Don't immediately kick if we're banning ourselves (destroys session, crash)
            if (account != holder->GetAuthorAccountId())
            {
                if (WorldSession* sess = sWorld.FindSession(account))
                {
                    sess->LogoutPlayer(true);
                    sess->KickPlayer();
                }
            }
        } while (result->NextRow());

        banResult = BAN_SUCCESS;

        sWorld.SendGMText(string_format("{} banned {} {} (Reason {}).", holder->GetAuthor().c_str(), holder->GetBanTarget().c_str(), holder->GetDuration() > 0 ?
            string_format("for {} seconds", holder->GetDuration()).c_str() : "permanently", holder->GetReason().c_str()));

        if (session)
        {
            ChatHandler(session).SendBanResult(holder->GetBanMode(), banResult, holder->GetBanTarget(), holder->GetDuration(), holder->GetReason());
        }

        holder->DeleteAllResults();
        delete holder;
    }
} banHandler;

/// Ban an account or ban an IP address, duration_secs if it is positive used, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string nameOrIP, uint32 duration_secs, std::string reason, std::string author)
{
    LoginDatabase.escape_string(nameOrIP);
    LoginDatabase.escape_string(reason);
    std::string safe_author = author;
    LoginDatabase.escape_string(safe_author);

    PlayerCacheData* authorData = sObjectMgr.GetPlayerDataByName(author);

    BanQueryHolder* holder = new BanQueryHolder(mode, nameOrIP, duration_secs, reason, realmID, safe_author,
        authorData ? authorData->uiAccount : 0);

    holder->SetSize(1);

    DatabaseType* db = nullptr;
    ///- Update the database with ban information
    switch (mode)
    {
        case BAN_IP:
            //No SQL injection as strings are escaped
            db = &LoginDatabase;

            holder->SetPQuery(0, "SELECT id FROM account WHERE last_ip = '%s'", nameOrIP.c_str());
            LoginDatabase.PExecute("INSERT INTO ip_banned VALUES ('%s',UNIX_TIMESTAMP(),UNIX_TIMESTAMP()+%u,'%s','%s')", nameOrIP.c_str(), duration_secs, safe_author.c_str(), reason.c_str());

            if (duration_secs > 0)
                sAccountMgr.BanIP(nameOrIP, time(nullptr) + duration_secs);
            else
                sAccountMgr.BanIP(nameOrIP, 0xFFFFFFFF);
            break;
        case BAN_ACCOUNT:
            //No SQL injection as string is escaped
            db = &LoginDatabase;
            holder->SetPQuery(0, "SELECT id FROM account WHERE username = '%s'", nameOrIP.c_str());
            break;
        case BAN_CHARACTER:
        {
            db = &CharacterDatabase;
            if (uint32 guid = sObjectMgr.GetPlayerGuidByName(nameOrIP))
                holder->SetPQuery(0, "SELECT account FROM characters WHERE guid = %u", guid);
            break;
        }
        default:
            return BAN_SYNTAX_ERROR;
    }

    db->DelayQueryHolderUnsafe(&banHandler, &BanAccountHandler::HandleAccountSelectResult, holder);

    return BAN_INPROGRESS;
}

/// Remove a ban from an account or IP address
bool World::RemoveBanAccount(BanMode mode, std::string const& source, std::string const& message, std::string nameOrIP)
{
    if (mode == BAN_IP)
    {
        LoginDatabase.escape_string(nameOrIP);
        LoginDatabase.PExecute("DELETE FROM ip_banned WHERE ip = '%s'", nameOrIP.c_str());
        sAccountMgr.UnbanIP(nameOrIP);
    }
    else
    {
        uint32 account = 0;
        if (mode == BAN_ACCOUNT)
            account = sAccountMgr.GetId(nameOrIP);
        else if (mode == BAN_CHARACTER)
            account = sObjectMgr.GetPlayerAccountIdByPlayerName(nameOrIP);

        if (!account)
            return false;

        //NO SQL injection as account is uint32
        LoginDatabase.PExecute("UPDATE account_banned SET active = '0' WHERE id = '%u'", account);
        sAccountMgr.UnbanAccount(account);
    }
    return true;
}

/// Update the game time
void World::_UpdateGameTime()
{
    ///- update the time
    time_t thisTime = time(nullptr);
    uint32 elapsed = uint32(thisTime - m_gameTime);
    m_gameTime = thisTime;
    m_gameDay = (m_gameTime + m_timeZoneOffset) / DAY;

    ///- if there is a shutdown timer
    if (!m_stopEvent && m_ShutdownTimer > 0 && elapsed > 0)
    {
        ///- ... and it is overdue, stop the world (set m_stopEvent)
        if (m_ShutdownTimer <= elapsed)
        {
            if (!(m_ShutdownMask & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
                m_stopEvent = true;                         // exist code already set
            else
                m_ShutdownTimer = 1;                        // minimum timer value to wait idle state
        }
        ///- ... else decrease it and if necessary display a shutdown countdown to the users
        else
        {
            m_ShutdownTimer -= elapsed;

            ShutdownMsg();
        }
    }
}

/// Shutdown the server
void World::ShutdownServ(uint32 time, uint32 options, uint8 exitcode)
{
    // ignore if server shutdown at next tick
    if (m_stopEvent)
        return;

    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_SHUTDOWN_INITIATE, [&](WorldScript* script)
    {
        script->OnShutdownInitiate(options, exitcode);
    });

    m_ShutdownMask = options;
    m_ExitCode = exitcode;

    ///- If the shutdown time is 0, set m_stopEvent (except if shutdown is 'idle' with remaining sessions)
    if (time == 0)
    {
        if (!(options & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
            m_stopEvent = true;                             // exist code already set
        else
            m_ShutdownTimer = 1;                            //So that the session count is re-evaluated at next world tick
    }
    ///- Else set the shutdown timer and warn users
    else
    {
        m_ShutdownTimer = time;
        ShutdownMsg(true);
    }
}

/// Display a shutdown message to the user(s)
void World::ShutdownMsg(bool show, Player* player)
{
    // not show messages for idle shutdown mode
    if (m_ShutdownMask & SHUTDOWN_MASK_IDLE)
        return;

    ///- Display a message every 12 hours, hours, 5 minutes, minute, 5 seconds and finally seconds
    if (show ||
            (m_ShutdownTimer < 10) ||
            // < 30 sec; every 5 sec
            (m_ShutdownTimer < 30        && (m_ShutdownTimer % 5) == 0) ||
            // < 5 min ; every 1 min
            (m_ShutdownTimer < 5 * MINUTE  && (m_ShutdownTimer % MINUTE) == 0) ||
            // < 30 min ; every 5 min
            (m_ShutdownTimer < 30 * MINUTE && (m_ShutdownTimer % (5 * MINUTE)) == 0) ||
            // < 12 h ; every 1 h
            (m_ShutdownTimer < 12 * HOUR   && (m_ShutdownTimer % HOUR) == 0) ||
            // > 12 h ; every 12 h
            (m_ShutdownTimer > 12 * HOUR   && (m_ShutdownTimer % (12 * HOUR)) == 0))
    {
        std::string str = secsToTimeString(m_ShutdownTimer);

        ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_TIME : SERVER_MSG_SHUTDOWN_TIME;

        SendServerMessage(msgid, str.c_str(), player);
        printf("Server is %s in %s\n", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shutting down"), str.c_str());
    }
}

/// Cancel a planned server shutdown
void World::ShutdownCancel()
{
    // nothing cancel or too later
    if (!m_ShutdownTimer || m_stopEvent)
        return;

    ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_SHUTDOWN_CANCEL, [](WorldScript* script)
    {
        script->OnShutdownCancel();
    });

    ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_CANCELLED : SERVER_MSG_SHUTDOWN_CANCELLED;

    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_ExitCode = SHUTDOWN_EXIT_CODE;                       // to default value
    SendServerMessage(msgid);

    DEBUG_LOG("Server %s cancelled.", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shutdown"));
}

/// Send a server message to the user(s)
void World::SendServerMessage(ServerMessageType type, const char *text, Player* player)
{
    WorldPacket data(SMSG_SERVER_MESSAGE, 50);              // guess size
    data << uint32(type);
    data << text;

    if (player)
        player->GetSession()->SendPacket(&data);
    else
        SendGlobalMessage(&data);
}

void World::UpdateSessions(uint32 diff)
{
    XScopeStatTimer ScopeStatTimer{sPerfMonitor.UpdateSession};
    ///- Update player limit if needed
    int32 hardPlayerLimit = getConfig(CONFIG_UINT32_PLAYER_HARD_LIMIT);
    if (hardPlayerLimit)
        m_playerLimit = std::min(hardPlayerLimit, m_playerLimit);

    uint32 maxNonRegionalPercentage = getConfig(CONFIG_UINT32_MAX_PERCENTAGE_POP_NON_REGIONAL);
    uint32 maxRegionalPercentage = getConfig(CONFIG_UINT32_MAX_PERCENTAGE_POP_REGIONAL);

    uint32 maxNonRegionalPop = hardPlayerLimit / 100.f * (float)maxNonRegionalPercentage;
    uint32 maxRegionalPop = hardPlayerLimit / 100.f * (float)maxRegionalPercentage;

    uint32 currentNonRegionalPop = loggedNonRegionSessions;
    uint32 currentRegionalPop = loggedRegionSessions;

    constexpr uint32 RegionalPopIndex = 0;
    constexpr uint32 NonRegionalPopIndex = 1;

    if (currentRegionalPop < maxRegionalPop && m_priorityQueue[RegionalPopIndex].size())
    {
        if (uint32 acceptNow = getConfig(CONFIG_UINT32_LOGIN_PER_TICK))
        {
            m_playerLimit = std::min(m_playerLimit + acceptNow, currentNonRegionalPop + currentRegionalPop + acceptNow);
            if ((hardPlayerLimit && m_playerLimit > hardPlayerLimit) || hardPlayerLimit <= GetActiveSessionCount())
            {
                m_playerLimit = hardPlayerLimit;
                acceptNow = 0;
            }

            for (uint32 i = 0; i < acceptNow && !m_priorityQueue[RegionalPopIndex].empty(); ++i)
            {
                // accept first in queue
                WorldSession* pop_sess = m_priorityQueue[RegionalPopIndex].front().second;
                pop_sess->SetInQueue(false);
                pop_sess->m_idleTime = WorldTimer::getMSTime();
                pop_sess->SendAuthWaitQue(0);
                m_priorityQueue[RegionalPopIndex].pop_front();
            }

            for (auto& elem : m_priorityQueue[RegionalPopIndex])
            {
                elem.first += diff * getConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_PER_TICK);
            }

            int position = 1;
            for (auto iter = m_priorityQueue[RegionalPopIndex].begin(); iter != m_priorityQueue[RegionalPopIndex].end(); ++iter, ++position)
            {
                iter->second->SendAuthWaitQue(position);
            }
        }
    }

    if (currentNonRegionalPop < maxNonRegionalPop && m_priorityQueue[NonRegionalPopIndex].size())
    {
        if (uint32 acceptNow = getConfig(CONFIG_UINT32_LOGIN_PER_TICK))
        {
            m_playerLimit = std::min(m_playerLimit + acceptNow, currentNonRegionalPop + currentRegionalPop + acceptNow);
            if ((hardPlayerLimit && m_playerLimit > hardPlayerLimit) || hardPlayerLimit <= GetActiveSessionCount())
            {
                m_playerLimit = hardPlayerLimit;
                acceptNow = 0;
            }


            for (uint32 i = 0; i < acceptNow && !m_priorityQueue[NonRegionalPopIndex].empty(); ++i)
            {
                // accept first in queue
                WorldSession* pop_sess = m_priorityQueue[NonRegionalPopIndex].front().second;
                pop_sess->SetInQueue(false);
                pop_sess->m_idleTime = WorldTimer::getMSTime();
                pop_sess->SendAuthWaitQue(0);
                m_priorityQueue[NonRegionalPopIndex].pop_front();
            }

            for (auto& elem : m_priorityQueue[NonRegionalPopIndex])
            {
                elem.first += diff * getConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_PER_TICK);
            }

            int position = 1;
            for (auto iter = m_priorityQueue[NonRegionalPopIndex].begin(); iter != m_priorityQueue[NonRegionalPopIndex].end(); ++iter, ++position)
            {
                iter->second->SendAuthWaitQue(position);
            }
        }
    }


    /*
    uint32 queuedSessions = getConfig(CONFIG_BOOL_ENABLE_PRIORITY_QUEUE) ? m_priorityQueue.size() : m_QueuedSessions.size();
    uint32 loggedInSessions = uint32(m_sessions.size() - queuedSessions); 
    if (m_playerLimit >= 0 && static_cast <int32> (loggedInSessions) < hardPlayerLimit)
    {
        if (uint32 acceptNow = getConfig(CONFIG_UINT32_LOGIN_PER_TICK))
        {
            m_playerLimit = std::min(m_playerLimit + acceptNow, loggedInSessions + acceptNow);
            if (hardPlayerLimit && m_playerLimit > hardPlayerLimit)
            {
                m_playerLimit = hardPlayerLimit;
                acceptNow = 0;
            }

            if (getConfig(CONFIG_BOOL_ENABLE_PRIORITY_QUEUE))
            {
                for (uint32 i = 0; i < acceptNow && !m_priorityQueue.empty(); ++i)
                {
                    // accept first in queue
                    WorldSession* pop_sess = m_priorityQueue.front().second;
                    pop_sess->SetInQueue(false);
                    pop_sess->m_idleTime = WorldTimer::getMSTime();
                    pop_sess->SendAuthWaitQue(0);
                    m_priorityQueue.pop_front();
                }

                for (auto& elem : m_priorityQueue)
                {
                    elem.first += diff * getConfig(CONFIG_UINT32_PRIORITY_QUEUE_PRIORITY_PER_TICK);
                }

                int position = 1;
                for (auto iter = m_priorityQueue.begin(); iter != m_priorityQueue.end(); ++iter, ++position)
                    iter->second->SendAuthWaitQue(position);
            }
            else
            {
                for (uint32 i = 0; i < acceptNow && !m_QueuedSessions.empty(); ++i)
                {
                    // accept first in queue
                    WorldSession* pop_sess = m_QueuedSessions.front();
                    pop_sess->SetInQueue(false);
                    pop_sess->m_idleTime = WorldTimer::getMSTime();
                    pop_sess->SendAuthWaitQue(0);
                    m_QueuedSessions.pop_front();
                }

                int position = 1;
                for (Queue::iterator iter = m_QueuedSessions.begin(); iter != m_QueuedSessions.end(); ++iter, ++position)
                    (*iter)->SendAuthWaitQue(position);
            }
        }
    }*/

    ///- Add new sessions
    WorldSession* sess;
    while (addSessQueue.next(sess))
        AddSession_(sess);

    m_headlessSessionMgr->PromotePending();

    ///- Then send an update signal to remaining ones
    time_t time_now = time(nullptr);

    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); )
    {
		WorldSession* pSession = itr->second;
		WorldSessionFilter updater(pSession);

		pSession->AddActiveTime(diff);
        if (!pSession->Update(updater))
        {
			if (pSession->PlayerLoading())
				sLog.outInfo("[CRASH] World::UpdateSession attempt to delete session %u loading a player.", pSession->GetAccountId());
			if (!RemoveQueuedSession(pSession) && pSession->HadQueue())
				m_accountsLastLogout[pSession->GetAccountId()] = time_now;
            itr = m_sessions.erase(itr);
			m_Ipconnections[pSession->GetBinaryAddress()]--;

			delete pSession;
        }
        else
        {
            itr++;
        }
    }


    ///- Update disconnected sessions
    for (SessionSet::iterator itr = m_disconnectedSessions.begin(); itr != m_disconnectedSessions.end(); )
    {
		WorldSession* pSession = *itr;

		if (!pSession->UpdateDisconnected(diff))
		{
			delete pSession;
            itr = m_disconnectedSessions.erase(itr);
		}
        else
        {
            itr++;
        }
    }

    m_headlessSessionMgr->Update(diff);
}

// This handles the issued and queued CLI/RA commands
void World::ProcessCliCommands()
{
    CliCommandHolder* command;
    while (cliCmdQueue.next(command))
    {
        DEBUG_LOG("CLI command under processing...");
        CliCommandHolder::Print* zprint = command->m_print;
        std::any callbackArg = command->m_callbackArg;
        CliHandler handler(command->m_cliAccountId, command->m_cliAccessLevel, callbackArg, zprint);
        handler.ParseCommands(command->m_command);

        if (command->m_commandFinished)
            command->m_commandFinished(callbackArg, !handler.HasSentErrorMessage());

        delete command;
    }
}

void utf8print(std::any, const char* str);
void commandFinished(std::any, bool /*sucess*/);

void World::LoadPendingCommands(QueryResult* pResult)
{
    if (!pResult)
        return;

    do
    {
        auto fields = pResult->Fetch();

        uint32 id = fields[0].GetUInt32();
        std::string command = fields[1].GetCppString();

        sLog.outBasic("Loaded command %u from database: %s", id, command.c_str());
        QueueCliCommand(new CliCommandHolder(0, SEC_CONSOLE, nullptr, command.c_str(), &utf8print, &commandFinished));
        LoginDatabase.PExecute("DELETE FROM `pending_commands` WHERE `id`=%u", id);

    } while (pResult->NextRow());

    delete pResult;
}

void World::InitResultQueue()
{
}

void World::UpdateResultQueue()
{
    //process async result queues
    CharacterDatabase.ProcessResultQueue(getConfig(CONFIG_UINT32_ASYNC_QUERIES_TICK_TIMEOUT));
    WorldDatabase.ProcessResultQueue(getConfig(CONFIG_UINT32_ASYNC_QUERIES_TICK_TIMEOUT));
    LoginDatabase.ProcessResultQueue(getConfig(CONFIG_UINT32_ASYNC_QUERIES_TICK_TIMEOUT));
}

void World::UpdateRealmCharCount(uint32 accountId)
{
    CharacterDatabase.AsyncPQuery(this, &World::_UpdateRealmCharCount, accountId,
                                  "SELECT COUNT(guid) FROM characters WHERE account = '%u'", accountId);
}

void World::_UpdateRealmCharCount(QueryResult *resultCharCount, uint32 accountId)
{
    if (resultCharCount)
    {
        Field *fields = resultCharCount->Fetch();
        uint32 charCount = fields[0].GetUInt32();
        delete resultCharCount;

        LoginDatabase.BeginTransaction();
        LoginDatabase.PExecute("REPLACE INTO realmcharacters (numchars, acctid, realmid) VALUES (%u, %u, %u)", charCount, accountId, realmID);
        LoginDatabase.CommitTransaction();
    }
}

void World::LoadAccountData()
{
    uint32 days = getConfig(CONFIG_UINT32_ACCOUNT_DATA_LAST_LOGIN_DAYS);
    if (!days)
        return;

    std::unique_ptr<QueryResult> result{ LoginDatabase.PQuery("SELECT id, username, email FROM account WHERE last_login >= NOW() - INTERVAL %u DAY", days) };

    uint32 count = 0;

    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        auto accountData = GetAccountData(id);
        accountData->id = id;
        accountData->username = fields[1].GetCppString();
        accountData->email = fields[2].GetCppString();
        ++count;
    } while (result->NextRow());

    result.reset(LoginDatabase.PQuery("SELECT account, extendedHash FROM system_fingerprint_usage GROUP BY account ORDER BY time DESC"));
    if (result)
    {
        do
        {
            auto fields = result->Fetch();
            auto accountData = GetAccountData(fields[0].GetUInt32());
            accountData->lastExtendedFingerprint = fields[1].GetUInt64();
        } while (result->NextRow());
    }

    sLog.outString("Loaded %u cached accounts.", count);
}

std::unordered_set<std::string> World::GetAccountNamesByFingerprint(uint32 fingerprint) const
{
    const auto itr = m_fingerprintAccounts.find(fingerprint);
    if (itr == m_fingerprintAccounts.end())
        return {};
    return itr->second;
}


void World::AddFingerprint(uint32 fingerprint, std::string accountName)
{
    m_fingerprintAccounts[fingerprint].insert(accountName);
}

void World::SetLastDiff(uint32 diff)
{
    m_lastDiff = diff;
    static uint32 currentDiffIndex = 0;

    if (currentDiffIndex >= m_lastDiffs.size())
        currentDiffIndex = 0;

    m_lastDiffs[currentDiffIndex] = m_lastDiff;
    ++currentDiffIndex;

    CheckDiffProtection();
}

void World::CheckDiffProtection()
{
    if (m_lastDiff > sWorld.getConfig(CONFIG_UINT32_DIFF_HC_PROTECTION) && m_diffThresholdHits < 20)
        m_diffThresholdHits += 5;

    if (m_diffThresholdHits > 0)
        --m_diffThresholdHits;

}

//If this triggers we should prevent HCs from dying as there's something badly wrong.
bool World::HitsDiffThreshold() const
{
    return m_diffThresholdHits > 0;
}

uint32 World::GetAverageDiff() const
{
    uint32 sum = 0;
    for (auto i : m_lastDiffs)
        sum += i;

    return sum / m_lastDiffs.size();
}

void World::SetPlayerLimit(int32 limit, bool needUpdate)
{
    if (limit < -SEC_ADMINISTRATOR)
        limit = -SEC_ADMINISTRATOR;

    bool const wasOpen = m_playerLimit >= 0;
    bool const isOpen = limit >= 0;

    // lock update need
    bool db_update_need = needUpdate || (limit < 0) != (m_playerLimit < 0) || (limit < 0 && m_playerLimit < 0 && limit != m_playerLimit);

    m_playerLimit = limit;

    if (db_update_need)
        LoginDatabase.PExecute("UPDATE realmlist SET allowedSecurityLevel = '%u' WHERE id = '%u'",
                               uint32(GetPlayerSecurityLimit()), realmID);

    if (wasOpen != isOpen)
    {
        ScriptRegistry<WorldScript>::ForEachEnabledHook(WORLDHOOK_ON_OPEN_STATE_CHANGE, [&](WorldScript* script)
        {
            script->OnOpenStateChange(isOpen);
        });
    }
}

void World::UpdateMaxSessionCounters()
{
    uint32 queueSize = getConfig(CONFIG_BOOL_ENABLE_PRIORITY_QUEUE) ? m_priorityQueue[0].size() + m_priorityQueue[1].size() : m_QueuedSessions.size();
    m_maxActiveSessionCount = std::max(m_maxActiveSessionCount, uint32(m_sessions.size() - queueSize));
    m_maxQueuedSessionCount = std::max(m_maxQueuedSessionCount, uint32(queueSize));
}

void World::setConfig(eConfigUInt32Values index, char const* fieldname, uint32 defvalue)
{
    setConfig(index, sConfig.GetIntDefault(fieldname, defvalue));
}

void World::setConfig(eConfigInt32Values index, char const* fieldname, int32 defvalue)
{
    setConfig(index, sConfig.GetIntDefault(fieldname, defvalue));
}

void World::setConfig(eConfigFloatValues index, char const* fieldname, float defvalue)
{
    setConfig(index, sConfig.GetFloatDefault(fieldname, defvalue));
}

void World::setConfig(eConfigBoolValues index, char const* fieldname, bool defvalue)
{
    setConfig(index, sConfig.GetBoolDefault(fieldname, defvalue));
}

void World::setConfigPos(eConfigUInt32Values index, char const* fieldname, uint32 defvalue)
{
    setConfig(index, fieldname, defvalue);
    if (int32(getConfig(index)) < 0)
    {
        sLog.outError("%s (%i) can't be negative. Using %u instead.", fieldname, int32(getConfig(index)), defvalue);
        setConfig(index, defvalue);
    }
}

void World::setConfigPos(eConfigFloatValues index, char const* fieldname, float defvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < 0.0f)
    {
        sLog.outError("%s (%f) can't be negative. Using %f instead.", fieldname, getConfig(index), defvalue);
        setConfig(index, defvalue);
    }
}

void World::setConfigMin(eConfigUInt32Values index, char const* fieldname, uint32 defvalue, uint32 minvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%u) must be >= %u. Using %u instead.", fieldname, getConfig(index), minvalue, minvalue);
        setConfig(index, minvalue);
    }
}

void World::setConfigMin(eConfigInt32Values index, char const* fieldname, int32 defvalue, int32 minvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%i) must be >= %i. Using %i instead.", fieldname, getConfig(index), minvalue, minvalue);
        setConfig(index, minvalue);
    }
}

void World::setConfigMin(eConfigFloatValues index, char const* fieldname, float defvalue, float minvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%f) must be >= %f. Using %f instead.", fieldname, getConfig(index), minvalue, minvalue);
        setConfig(index, minvalue);
    }
}

void World::setConfigMinMax(eConfigUInt32Values index, char const* fieldname, uint32 defvalue, uint32 minvalue, uint32 maxvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%u) must be in range %u...%u. Using %u instead.", fieldname, getConfig(index), minvalue, maxvalue, minvalue);
        setConfig(index, minvalue);
    }
    else if (getConfig(index) > maxvalue)
    {
        sLog.outError("%s (%u) must be in range %u...%u. Using %u instead.", fieldname, getConfig(index), minvalue, maxvalue, maxvalue);
        setConfig(index, maxvalue);
    }
}

void World::setConfigMinMax(eConfigInt32Values index, char const* fieldname, int32 defvalue, int32 minvalue, int32 maxvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%i) must be in range %i...%i. Using %i instead.", fieldname, getConfig(index), minvalue, maxvalue, minvalue);
        setConfig(index, minvalue);
    }
    else if (getConfig(index) > maxvalue)
    {
        sLog.outError("%s (%i) must be in range %i...%i. Using %i instead.", fieldname, getConfig(index), minvalue, maxvalue, maxvalue);
        setConfig(index, maxvalue);
    }
}

void World::setConfigMinMax(eConfigFloatValues index, char const* fieldname, float defvalue, float minvalue, float maxvalue)
{
    setConfig(index, fieldname, defvalue);
    if (getConfig(index) < minvalue)
    {
        sLog.outError("%s (%f) must be in range %f...%f. Using %f instead.", fieldname, getConfig(index), minvalue, maxvalue, minvalue);
        setConfig(index, minvalue);
    }
    else if (getConfig(index) > maxvalue)
    {
        sLog.outError("%s (%f) must be in range %f...%f. Using %f instead.", fieldname, getConfig(index), minvalue, maxvalue, maxvalue);
        setConfig(index, maxvalue);
    }
}

bool World::configNoReload(bool reload, eConfigUInt32Values index, char const* fieldname, uint32 defvalue)
{
    if (!reload)
        return true;

    uint32 val = sConfig.GetIntDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%u).", fieldname, getConfig(index));

    return false;
}

bool World::configNoReload(bool reload, eConfigInt32Values index, char const* fieldname, int32 defvalue)
{
    if (!reload)
        return true;

    int32 val = sConfig.GetIntDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%i).", fieldname, getConfig(index));

    return false;
}

bool World::configNoReload(bool reload, eConfigFloatValues index, char const* fieldname, float defvalue)
{
    if (!reload)
        return true;

    float val = sConfig.GetFloatDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%f).", fieldname, getConfig(index));

    return false;
}

bool World::configNoReload(bool reload, eConfigBoolValues index, char const* fieldname, bool defvalue)
{
    if (!reload)
        return true;

    bool val = sConfig.GetBoolDefault(fieldname, defvalue);
    if (val != getConfig(index))
        sLog.outError("%s option can't be changed at mangosd.conf reload, using current value (%s).", fieldname, getConfig(index) ? "'true'" : "'false'");

    return false;
}

void World::InvalidatePlayerDataToAllClients(ObjectGuid guid)
{
    WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
    data << guid;
    SendGlobalMessage(&data);
}

void World::SendCreatureStatsInvalidate(uint32 entry, WorldSession* self)
{
    WorldPacket data(SMSG_CREATURE_QUERY_RESPONSE, 4);
    data << uint32(entry | 0x80000000);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendGameObjectStatsInvalidate(uint32 entry, WorldSession* self)
{
    WorldPacket data(SMSG_GAMEOBJECT_QUERY_RESPONSE, 4);
    data << uint32(entry | 0x80000000);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendGuildStatsInvalidate(uint32 guildId, WorldSession* self)
{
    WorldPacket data(SMSG_GUILD_QUERY_RESPONSE, 35);
    data << uint32(guildId);
    data << uint8(0);

    for (size_t i = 0; i < GUILD_RANKS_MAX_COUNT; ++i)
            data << uint8(0);

    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendQuestStatsInvalidate(uint32 questId, WorldSession* self)
{
    WorldPacket data(SMSG_QUEST_QUERY_RESPONSE, 100);       // guess size

    data << uint32(questId);                                // quest id
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);

    for (int i = 0; i < 4; ++i)
        data << uint32(0) << uint32(0);

    for (int i = 0; i < 6; ++i)
        data << uint32(0) << uint32(0);

    data << uint32(0);
    data << float(0.0f);
    data << float(0.0f);
    data << uint32(0);

    data << uint8(0);
    data << uint8(0);
    data << uint8(0);
    data << uint8(0);

    for (int i = 0; i < 4; ++i)
        data << uint32(0) << uint32(0) << uint32(0) << uint32(0);

    for (int i = 0; i < 4; ++i)
        data << uint8(0);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendNpcTextInvalidate(uint32 textId, WorldSession* self)
{
    WorldPacket data(SMSG_NPC_TEXT_UPDATE, 4);
    data << uint32(textId | 0x80000000);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendPlayerNameInvalidate(ObjectGuid guid, WorldSession* self)
{
    WorldPacket data(SMSG_NAME_QUERY_RESPONSE, 22);
    data << guid;
    data << uint8(0);
    data << uint8(0);

    data << uint32(0);
    data << uint32(0);
    data << uint32(0);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendPetNameInvalidate(uint32 petNumber, WorldSession* self)
{
    WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, 9);
    data << uint32(petNumber);
    data << uint8(0);
    data << uint32(0);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendPageTextInvalidate(uint32 pageId, WorldSession* self)
{
    WorldPacket data(SMSG_PAGE_TEXT_QUERY_RESPONSE, 9);
    data << uint32(pageId | 0x80000000);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendPetitionInvalidate(uint32 petitionId, WorldSession* self)
{
    WorldPacket data(SMSG_PETITION_QUERY_RESPONSE, 64);
    petitionId *= -1;
    data << petitionId;                           // petition guid
    data << uint64(0);
    data << uint8(0);
    data << uint8(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint16(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendSingleItemInvalidate(uint32 entry, WorldSession* self)
{
    WorldPacket data(SMSG_ITEM_QUERY_SINGLE_RESPONSE, 4);
    data << uint32(entry | 0x80000000);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendMultipleItemsInvalidate(const std::vector<uint32>& items, WorldSession* self)
{
    WorldPacket data(SMSG_ITEM_QUERY_MULTIPLE_RESPONSE, 1 + 4 * items.size());
    data << uint8(items.size());
    for (const auto& entry : items)
        data << uint32(entry | 0x80000000);

    if (self)
        self->SendPacket(&data);
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
                itr->second->SendPacket(&data);
        }
    }
}

void World::SendUpdateCreatureStats(const CreatureInfo& crInfo, WorldSession* self)
{
    WorldPacket data(SMSG_CREATURE_QUERY_RESPONSE, 100);
    data << crInfo.entry;
    size_t NamePos = data.wpos();
    data << crInfo.name;
    data << uint8(0) << uint8(0) << uint8(0);
    size_t SubNamePos = data.wpos();
    data << crInfo.subname;
    data << crInfo.type_flags;
    data << crInfo.type;
    data << crInfo.beast_family;
    data << crInfo.rank;
    data << uint32(0);
    data << crInfo.pet_spell_list_id;
    data << crInfo.display_id;
    data << crInfo.civilian;
    data << crInfo.racial_leader;

    if (self)
    {
        int loc_idx = self->GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
        {
            CreatureLocale const* cl = sObjectMgr.GetCreatureLocale(crInfo.entry);
            if (cl)
            {
                if (cl->Name.size() > size_t(loc_idx) && !cl->Name[loc_idx].empty())
                    data.put<std::string>(NamePos, cl->Name[loc_idx]);
                if (cl->SubName.size() > size_t(loc_idx) && !cl->SubName[loc_idx].empty())
                    data.put<std::string>(SubNamePos, cl->SubName[loc_idx]);
            }
        }

        self->SendPacket(&data);
    }
    else
    {
        SessionMap::const_iterator itr;
        for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        {
            if (itr->second)
            {
                int loc_idx = itr->second->GetSessionDbLocaleIndex();
                if (loc_idx >= 0)
                {
                    CreatureLocale const* cl = sObjectMgr.GetCreatureLocale(crInfo.entry);
                    if (cl)
                    {
                        if (cl->Name.size() > size_t(loc_idx) && !cl->Name[loc_idx].empty())
                            data.put<std::string>(NamePos, cl->Name[loc_idx]);
                        if (cl->SubName.size() > size_t(loc_idx) && !cl->SubName[loc_idx].empty())
                            data.put<std::string>(SubNamePos, cl->SubName[loc_idx]);
                    }
                }

                itr->second->SendPacket(&data);
            }
        }
    }
}

void World::SendUpdateSingleItem(uint32 entry, WorldSession* self)
{
    // While only for transmogrification
    ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(entry);
    if (pProto)
    {
        WorldPacket data(SMSG_ITEM_QUERY_SINGLE_RESPONSE, 600);
        data << pProto->ItemId;
        data << pProto->Class;
        // client known only 0 subclass (and 1-2 obsolute subclasses)
        data << (pProto->Class == ITEM_CLASS_CONSUMABLE ? uint32(0) : pProto->SubClass);
        size_t NamePos = data.wpos();
        data << pProto->Name1;                              // max length of any of 4 names: 256 bytes
        data << uint8(0x00);                                //pProto->Name2; // blizz not send name there, just uint8(0x00); <-- \0 = empty string = empty name...
        data << uint8(0x00);                                //pProto->Name3; // blizz not send name there, just uint8(0x00);
        data << uint8(0x00);                                //pProto->Name4; // blizz not send name there, just uint8(0x00);
        data << pProto->DisplayInfoID;
        data << pProto->Quality;
        data << pProto->Flags;
        data << pProto->BuyPrice;
        data << pProto->SellPrice;
        data << pProto->InventoryType;
        data << pProto->AllowableClass;
        data << pProto->AllowableRace;
        data << pProto->ItemLevel;
        data << pProto->RequiredLevel;
        data << pProto->RequiredSkill;
        data << pProto->RequiredSkillRank;
        data << pProto->RequiredSpell;
        // Item de style insigne
        if (pProto->Spells[0].SpellId != 0)
            data << uint32(0);
        else
            data << pProto->RequiredHonorRank;

        data << pProto->RequiredCityRank;
        data << pProto->RequiredReputationFaction;
        data << (pProto->RequiredReputationFaction > 0 ? pProto->RequiredReputationRank : 0);   // send value only if reputation faction id setted ( needed for some items)
        data << pProto->MaxCount;
        data << pProto->Stackable;
        data << pProto->ContainerSlots;
        for (int i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        {
            data << pProto->ItemStat[i].ItemStatType;
            data << pProto->ItemStat[i].ItemStatValue;
        }
        for (int i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
        {
            data << pProto->Damage[i].DamageMin;
            data << pProto->Damage[i].DamageMax;
            data << pProto->Damage[i].DamageType;
        }

        // resistances (7)
        data << pProto->Armor;
        data << pProto->HolyRes;
        data << pProto->FireRes;
        data << pProto->NatureRes;
        data << pProto->FrostRes;
        data << pProto->ShadowRes;
        data << pProto->ArcaneRes;

        data << pProto->Delay;
        data << pProto->AmmoType;
        data << (float)pProto->RangedModRange;

        for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
        {
            // send DBC data for cooldowns in same way as it used in Spell::SendSpellCooldown
            // use `item_template` or if not set then only use spell cooldowns
            SpellEntry const* spell = sSpellMgr.GetSpellEntry(pProto->Spells[s].SpellId);
            if (spell)
            {
                bool db_data = pProto->Spells[s].SpellCooldown >= 0 || pProto->Spells[s].SpellCategoryCooldown >= 0;

                data << pProto->Spells[s].SpellId;
                data << pProto->Spells[s].SpellTrigger;

                // let the database control the sign here.  negative means that the item should be consumed once the charges are consumed.
                data << pProto->Spells[s].SpellCharges;

                if (db_data)
                {
                    data << uint32(pProto->Spells[s].SpellCooldown);
                    data << uint32(pProto->Spells[s].SpellCategory);
                    data << uint32(pProto->Spells[s].SpellCategoryCooldown);
                }
                else
                {
                    data << uint32(spell->RecoveryTime);
                    data << uint32(spell->Category);
                    data << uint32(spell->CategoryRecoveryTime);
                }
            }
            else
            {
                data << uint32(0);
                data << uint32(0);
                data << uint32(0);
                data << uint32(-1);
                data << uint32(0);
                data << uint32(-1);
            }
        }
        data << pProto->Bonding;
        size_t DescPos = data.wpos();
        data << pProto->Description;
        data << pProto->PageText;
        data << pProto->LanguageID;
        data << pProto->PageMaterial;
        data << pProto->StartQuest;
        data << pProto->LockID;
        data << pProto->Material;
        data << pProto->Sheath;
        data << pProto->RandomProperty;
        data << pProto->Block;
        data << pProto->ItemSet;
        data << pProto->MaxDurability;
        data << pProto->Area;
        data << pProto->Map;                                // Added in 1.12.x & 2.0.1 client branch
        data << pProto->BagFamily;

        if (self)
        {
            int loc_idx = self->GetSessionDbLocaleIndex();
            if (loc_idx >= 0)
            {
                ItemLocale const* il = sObjectMgr.GetItemLocale(pProto->DestItemId);
                if (il)
                {
                    if (il->Name.size() > size_t(loc_idx) && !il->Name[loc_idx].empty())
                        data.put<std::string>(NamePos, il->Name[loc_idx]);
                    if (il->Description.size() > size_t(loc_idx) && !il->Description[loc_idx].empty())
                        data.put<std::string>(DescPos, il->Description[loc_idx]);
                }
            }

            self->SendPacket(&data);
        }
        else
        {
            SessionMap::const_iterator itr;
            for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
            {
                if (itr->second)
                {
                    int loc_idx = itr->second->GetSessionDbLocaleIndex();
                    if (loc_idx >= 0)
                    {
                        ItemLocale const* il = sObjectMgr.GetItemLocale(pProto->DestItemId);
                        if (il)
                        {
                            if (il->Name.size() > size_t(loc_idx) && !il->Name[loc_idx].empty())
                                data.put<std::string>(NamePos, il->Name[loc_idx]);
                            if (il->Description.size() > size_t(loc_idx) && !il->Description[loc_idx].empty())
                                data.put<std::string>(DescPos, il->Description[loc_idx]);
                        }
                    }

                    itr->second->SendPacket(&data);
                }
            }
        }
    }
}

void World::SendUpdateMultipleItems(const std::vector<uint32>& items, WorldSession* self)
{
    // While only for transmogrification
    WorldPacket data(SMSG_ITEM_QUERY_MULTIPLE_RESPONSE, 600*items.size());
    data << uint8(items.size());
    for (const auto& entry : items)
    {
        if (ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(entry))
        {
            data << pProto->ItemId;
            data << pProto->Class;
            // client known only 0 subclass (and 1-2 obsolute subclasses)
            data << (pProto->Class == ITEM_CLASS_CONSUMABLE ? uint32(0) : pProto->SubClass);
            size_t NamePos = data.wpos();
            data << pProto->Name1;                              // max length of any of 4 names: 256 bytes
            data << uint8(0x00);                                //pProto->Name2; // blizz not send name there, just uint8(0x00); <-- \0 = empty string = empty name...
            data << uint8(0x00);                                //pProto->Name3; // blizz not send name there, just uint8(0x00);
            data << uint8(0x00);                                //pProto->Name4; // blizz not send name there, just uint8(0x00);
            data << pProto->DisplayInfoID;
            data << pProto->Quality;
            data << pProto->Flags;
            data << pProto->BuyPrice;
            data << pProto->SellPrice;
            data << pProto->InventoryType;
            data << pProto->AllowableClass;
            data << pProto->AllowableRace;
            data << pProto->ItemLevel;
            data << pProto->RequiredLevel;
            data << pProto->RequiredSkill;
            data << pProto->RequiredSkillRank;
            data << pProto->RequiredSpell;
            // Item de style insigne
            if (pProto->Spells[0].SpellId != 0)
                data << uint32(0);
            else
                data << pProto->RequiredHonorRank;

            data << pProto->RequiredCityRank;
            data << pProto->RequiredReputationFaction;
            data << (pProto->RequiredReputationFaction > 0 ? pProto->RequiredReputationRank : 0);   // send value only if reputation faction id setted ( needed for some items)
            data << pProto->MaxCount;
            data << pProto->Stackable;
            data << pProto->ContainerSlots;
            for (int i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
            {
                data << pProto->ItemStat[i].ItemStatType;
                data << pProto->ItemStat[i].ItemStatValue;
            }
            for (int i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
            {
                data << pProto->Damage[i].DamageMin;
                data << pProto->Damage[i].DamageMax;
                data << pProto->Damage[i].DamageType;
            }

            // resistances (7)
            data << pProto->Armor;
            data << pProto->HolyRes;
            data << pProto->FireRes;
            data << pProto->NatureRes;
            data << pProto->FrostRes;
            data << pProto->ShadowRes;
            data << pProto->ArcaneRes;

            data << pProto->Delay;
            data << pProto->AmmoType;
            data << (float)pProto->RangedModRange;

            for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
            {
                // send DBC data for cooldowns in same way as it used in Spell::SendSpellCooldown
                // use `item_template` or if not set then only use spell cooldowns
                SpellEntry const* spell = sSpellMgr.GetSpellEntry(pProto->Spells[s].SpellId);
                if (spell)
                {
                    bool db_data = pProto->Spells[s].SpellCooldown >= 0 || pProto->Spells[s].SpellCategoryCooldown >= 0;

                    data << pProto->Spells[s].SpellId;
                    data << pProto->Spells[s].SpellTrigger;

                    // let the database control the sign here.  negative means that the item should be consumed once the charges are consumed.
                    data << pProto->Spells[s].SpellCharges;

                    if (db_data)
                    {
                        data << uint32(pProto->Spells[s].SpellCooldown);
                        data << uint32(pProto->Spells[s].SpellCategory);
                        data << uint32(pProto->Spells[s].SpellCategoryCooldown);
                    }
                    else
                    {
                        data << uint32(spell->RecoveryTime);
                        data << uint32(spell->Category);
                        data << uint32(spell->CategoryRecoveryTime);
                    }
                }
                else
                {
                    data << uint32(0);
                    data << uint32(0);
                    data << uint32(0);
                    data << uint32(-1);
                    data << uint32(0);
                    data << uint32(-1);
                }
            }
            data << pProto->Bonding;
            size_t DescPos = data.wpos();
            data << pProto->Description;
            data << pProto->PageText;
            data << pProto->LanguageID;
            data << pProto->PageMaterial;
            data << pProto->StartQuest;
            data << pProto->LockID;
            data << pProto->Material;
            data << pProto->Sheath;
            data << pProto->RandomProperty;
            data << pProto->Block;
            data << pProto->ItemSet;
            data << pProto->MaxDurability;
            data << pProto->Area;
            data << pProto->Map;                                // Added in 1.12.x & 2.0.1 client branch
            data << pProto->BagFamily;

            // TODO: global broadcast?
            if (self)
            {
                int loc_idx = self->GetSessionDbLocaleIndex();
                if (loc_idx >= 0)
                {
                    ItemLocale const* il = sObjectMgr.GetItemLocale(pProto->DestItemId);
                    if (il)
                    {
                        if (il->Name.size() > size_t(loc_idx) && !il->Name[loc_idx].empty())
                            data.put<std::string>(NamePos, il->Name[loc_idx]);
                        if (il->Description.size() > size_t(loc_idx) && !il->Description[loc_idx].empty())
                            data.put<std::string>(DescPos, il->Description[loc_idx]);
                    }
                }
            }
        }
    }

    if (self)
        self->SendPacket(&data);
}

void World::SetSessionDisconnected(WorldSession* sess)
{
    SessionMap::iterator itr = m_sessions.find(sess->GetAccountId());
    ASSERT(itr != m_sessions.end());
    if (sess->HadQueue())
        m_accountsLastLogout[sess->GetAccountId()] = time(nullptr);

    m_sessions.erase(itr);
    m_disconnectedSessions.insert(sess);
}

void World::AddAsyncTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(m_asyncTaskQueueMutex);
    _asyncTasks.push_back(task);
}

void World::StopDiscordBot()
{
#ifdef USING_DISCORD_BOT
    sDiscordBot->Stop();
#endif
}

void World::SendDiscordMessage(uint64 channelId, std::string message)
{
#ifdef USING_DISCORD_BOT
    sDiscordBot->SendMessageToChannel(channelId, message);
#endif
}

bool World::CanSkipQueue(WorldSession const* sess)
{
    if (sess->GetSecurity() > SEC_PLAYER)
        return true;

    if (sess->CanQueueSkip())
        return true;

    uint32 grace_period = getConfig(CONFIG_UINT32_LOGIN_QUEUE_GRACE_PERIOD_SECS);
    if (!grace_period)
        return false;

    auto prev_logout = m_accountsLastLogout.find(sess->GetAccountId());
    if (prev_logout == m_accountsLastLogout.end())
        return false;

    time_t now = time(nullptr);
    return (now - prev_logout->second) < grace_period;
}

uint32 World::InsertLog(std::string const& message, AccountTypes sec)
{
    uint32 key = m_logMessages.size();
    ArchivedLogMessage& s = m_logMessages[key];
    s.msg = message;
    s.sec = sec;
    return key;
}

World::ArchivedLogMessage* World::GetLog(uint32 logId, AccountTypes my_sec)
{
    LogMessagesMap::iterator it = m_logMessages.find(logId);
    if (it == m_logMessages.end() || it->second.sec > my_sec)
        return nullptr;
    return &(it->second);
}

void World::SetWorldUpdateTimer(WorldTimers timer, uint32 current)
{
    if (timer >= WUPDATE_COUNT)
        return;

    m_timers[timer].SetCurrent(current);
}

time_t World::GetWorldUpdateTimer(WorldTimers timer)
{
    if (timer >= WUPDATE_COUNT)
        return 0;

    return m_timers[timer].GetCurrent();
}

time_t World::GetWorldUpdateTimerInterval(WorldTimers timer)
{
    if (timer >= WUPDATE_COUNT)
        return 0;

    return m_timers[timer].GetInterval();
}

void SessionPacketSendTask::operator()()
{
    if (WorldSession* session = sWorld.FindSession(m_accountId))
    {
        session->SendPacket(&m_data);
    }
}

void World::LogChat(WorldSession* sess, const char* type, std::string const& msg, PlayerPointer target, uint32 chanId, const char* chanStr)
{
    ASSERT(sess);
    PlayerPointer plr = sess->GetPlayerPointer();
    ASSERT(plr);

    std::string log = FormatLoggedChat(sess, type, msg, target, chanId, chanStr);

    uint32 sessionSecurity = sess->GetSecurity();
    uint32 targetSecurity = target && target->GetSession() ? target->GetSession()->GetSecurity() : SEC_PLAYER;

    bool staffLog = sessionSecurity > SEC_PLAYER || targetSecurity > SEC_PLAYER;
    bool modLog = staffLog && (sessionSecurity < SEC_DEVELOPER || targetSecurity < SEC_DEVELOPER);

    if (staffLog)
        SendDiscordMessage(modLog ? 1101486865477021726 : 1075085609737142352, log); // always log GM chats to a seperate chn too
    

    sLog.out(LOG_CHAT, "%s", log.c_str());
}

std::string World::FormatLoggedChat(WorldSession* sess, const char* type, std::string const& msg, PlayerPointer target, uint32 chanId, const char* chanStr)
{
    ASSERT(sess);
    PlayerPointer plr = sess->GetPlayerPointer();
    ASSERT(plr);

    std::string stringType = type;

    if (sess->GetSecurity() >= SEC_MODERATOR || (target && target->GetSession() && target->GetSession()->GetSecurity() >= SEC_MODERATOR))
    {
        stringType += "|GM";
    }

    std::ostringstream ss;
    ss << plr->GetName() << ":" << sess->GetAccountId();

    if (target)
        return string_format("[{}] {}:{} -> {}:{} : {}", stringType.c_str(), ss.str().c_str(), plr->GetObjectGuid().GetCounter(), target->GetName(), target->GetObjectGuid().GetCounter(), msg.c_str());
    else if (chanId)
        return string_format("[{}:{}] {}:{} : {}", stringType.c_str(), chanId, ss.str().c_str(), plr->GetObjectGuid().GetCounter(), msg.c_str());
    else if (chanStr)
        return string_format("[{}:{}] {}:{} : {}", stringType.c_str(), chanStr, ss.str().c_str(), plr->GetObjectGuid().GetCounter(), msg.c_str());
    else
        return string_format("[{}] {}:{} : {}", stringType.c_str(), ss.str().c_str(), plr->GetObjectGuid().GetCounter(), msg.c_str());
}


void MigrationFile::SetAuthor(std::string const& author)
{
    if (!sWorld.getConfig(CONFIG_UINT32_AUTO_COMMIT_MINUTES))
        return;

    if (author != lastAuthor)
    {
        FILE* pFile;
        pFile = fopen(sWorld.GetWorldUpdatesMigration().c_str(), "a");
        if (pFile)
        {
            fprintf(pFile, "\n-- Changes by %s\n", author.c_str());
            fclose(pFile);
        }
        lastAuthor = author;
    }
}

void MigrationFile::AddRow(char const* row)
{
    if (!row)
        return;

    if (!sWorld.getConfig(CONFIG_UINT32_AUTO_COMMIT_MINUTES))
        return;

    FILE* pFile;
    pFile = fopen(sWorld.GetWorldUpdatesMigration().c_str(), "a");
    if (pFile)
    {
        fprintf(pFile, "%s;\n", row);
        fclose(pFile);
        hasChanges = true;
    }
}

void MigrationFile::AddRowFormat(char const* format, ...)
{
    if (!format)
        return;

    if (!sWorld.getConfig(CONFIG_UINT32_AUTO_COMMIT_MINUTES))
        return;

    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == -1)
        return;

    FILE* pFile;
    pFile = fopen(sWorld.GetWorldUpdatesMigration().c_str(), "a");
    if (pFile)
    {
        fprintf(pFile, "%s;\n", szQuery);
        fclose(pFile);
        hasChanges = true;
    }
}

void MigrationFile::CommitUpdates()
{
    hasChanges = false;
    std::string command = "cd \"" + sWorld.GetWorldUpdatesDirectory() + "\" && git add --all && git commit -m \"Direct world update.\" && git pull --rebase && git push";
    system(command.c_str());
}

bool World::ExecuteUpdate(char const* format, ...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == -1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s", format);
        return false;
    }

    GetMigration().AddRow(szQuery);
    return WorldDatabase.PExecuteLog(szQuery);
}

void World::SchedulePlayerDump(uint32 guidLow)
{
    std::lock_guard<std::mutex> lock(m_autoPDumpMutex);
    m_autoPDumpPendingGuids.insert(guidLow);
    m_lockedCharacterGuids.insert(guidLow);
}

void World::UnlockCharacter(uint32 guidLow)
{
    std::lock_guard<std::mutex> lock(m_autoPDumpMutex);
    m_lockedCharacterGuids.erase(guidLow);
}

bool World::IsCharacterLocked(uint32 guidLow)
{
    std::lock_guard<std::mutex> lock(m_autoPDumpMutex);
    return m_lockedCharacterGuids.find(guidLow) != m_lockedCharacterGuids.end();
}

bool World::IsCharacterPDumpedRecently(uint32 guidLow, time_t timestamp)
{
    std::lock_guard<std::mutex> lock(m_autoPDumpMutex);

    auto itr = m_autoPDumpCharTimes.find(guidLow);
    if (itr == m_autoPDumpCharTimes.end())
        return false;

    for (auto const& timestamp2 : itr->second)
        if (abs(timestamp2 - timestamp) < WEEK)
            return true;

    return false;
}

void World::AddPDumpedCharacterToList(uint32 guidLow, time_t timestamp)
{
    std::lock_guard<std::mutex> lock(m_autoPDumpMutex);
    m_autoPDumpCharTimes[guidLow].insert(timestamp);
}

void World::AutoPDumpWorker()
{
    CharacterDatabase.ThreadStart();
    thread_name("AutoPDump");
    while (!IsStopped())
    {
        std::this_thread::sleep_for(5s);
        
        std::set<uint32> dumpGuids;

        {
            std::lock_guard<std::mutex> lock(m_autoPDumpMutex);
            std::swap(dumpGuids, m_autoPDumpPendingGuids);
        }

        for (auto const& guid : dumpGuids)
        {
            char fileName[64] = {};
            sprintf(fileName, "Char%u-%u.bak", guid, (uint32)GetGameTime());
            switch (PlayerDumpWriter().WriteDump(sWorld.GetPDumpDirectory() + "/" + fileName, guid))
            {
                case DUMP_SUCCESS:
                    break;
                default:
                    sLog.outError("Failed to dump character %u.", guid);
                    break;
            }
            UnlockCharacter(guid);
            AddPDumpedCharacterToList(guid, GetGameTime());
        }
    }
    CharacterDatabase.ThreadEnd();
}

void World::DeleteOldPDumps()
{
    if (ACE_DIR* dirp = ACE_OS::opendir(ACE_TEXT(sWorld.GetPDumpDirectory().c_str())))
    {
        ACE_DIRENT* dp;
        
        std::set<std::string> filesToDelete;
        while (!!(dp = ACE_OS::readdir(dirp)))
        {
            if (strstr(dp->d_name, "Char"))
            {
                if (char* pDash = strstr(dp->d_name, "-"))
                {
                    if (char* pDot = strstr(dp->d_name, ".bak"))
                    {
                        uint32 guidLow = strtol(dp->d_name + 4, &pDash, 10);
                        time_t timestamp = strtol(pDash + 1, &pDot, 10);
                        
                        if ((sWorld.getConfig(CONFIG_UINT32_AUTO_PDUMP_DELETE_AFTER_DAYS) && ((timestamp + (sWorld.getConfig(CONFIG_UINT32_AUTO_PDUMP_DELETE_AFTER_DAYS) * DAY)) < time(nullptr)))
                            || IsCharacterPDumpedRecently(guidLow, timestamp)) // dont keep duplicates
                        {
                            std::string fullPath = sWorld.GetPDumpDirectory() + "/" + dp->d_name;
                            filesToDelete.insert(fullPath);
                        }
                        else
                            AddPDumpedCharacterToList(guidLow, timestamp);
                    }
                }
            }
        }   

#ifndef _WIN32
        // this causes a crash on Windows, so just accept a minor memory leak for now
        ACE_OS::closedir(dirp);
#endif

        if (!filesToDelete.empty())
        {
            sLog.outInfo("Deleting %u old pdumps...", (uint32)filesToDelete.size());
            for (auto const& file : filesToDelete)
                remove(file.c_str());
        }
    }
}
