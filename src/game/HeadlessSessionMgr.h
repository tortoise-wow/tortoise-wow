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
#include "ObjectGuid.h"
#include "SessionTransport.h"

#include <map>
#include <string>

class LoginQueryHolder;
class World;
class WorldSession;

enum class HeadlessSessionState : unsigned char
{
    NotFound,
    Pending,
    Loading,
    Active,
};

enum class HeadlessSessionStartResult : unsigned char
{
    Started,
    InvalidAccount,
    InvalidCharacter,
    CharacterLocked,
    CharacterNotOwned,
    Duplicate,
    ConflictingPlayer,
    QueryDispatchFailed,
    AccountBanned,
};

class HeadlessSessionMgr
{
public:
    explicit HeadlessSessionMgr(World& world) : m_world(world) { }
    ~HeadlessSessionMgr();

    HeadlessSessionStartResult Start(uint32 accountId, ObjectGuid characterGuid,
        LocaleConstant locale, std::string const& tag);
    // World-owner call. Consumes a completed, unbound headless query holder on
    // every result. The caller retains population policy, never session ownership.
    HeadlessSessionStartResult StartPrepared(LoginQueryHolder* holder,
        LocaleConstant locale, std::string const& tag);
    bool Stop(ObjectGuid characterGuid, bool save = true);
    // World owner only, nestable. Stops requested by callbacks are applied on
    // scope exit, after callers have stopped using the player/session.
    void BeginStopDeferral() { ++m_stopDeferralDepth; }
    void EndStopDeferral();
    HeadlessSessionState GetState(ObjectGuid characterGuid) const;

private:
    friend class World;

    // Called by World only. The manager remains the sole owner of all
    // headless-session transitions and callback identity.
    void PromotePending();
    void Update(uint32 diff);
    void Shutdown();
    void StopForAccount(uint32 accountId, bool save);
    void HandleLoginCallback(LoginQueryHolder* holder);
    bool ReclaimForNetwork(ObjectGuid characterGuid, WorldSession* session,
        WorldSession* replacement, uint32 accountId);

    struct SessionEntry
    {
        WorldSession* session = nullptr;
        uint32 accountId = 0;
        ObjectGuid characterGuid;
        uint64 requestToken = 0;
        uint32 outOfWorldElapsed = 0;
    };

    HeadlessSessionStartResult ValidateStart(uint32 accountId, ObjectGuid characterGuid) const;
    uint64 NextRequestToken();
    void DestroySession(SessionEntry& entry, bool save, bool clearCharacterOnline);
    SessionEntry* FindEntry(ObjectGuid characterGuid, uint32 accountId,
        SessionTransport transport, uint64 requestToken);

    World& m_world;
    std::map<ObjectGuid, SessionEntry> m_sessions;
    std::map<ObjectGuid, SessionEntry> m_pendingSessions;
    uint64 m_nextRequestToken = 0;
    uint32 m_stopDeferralDepth = 0;
    struct DeferredStop { uint64 token; bool save; };
    std::map<ObjectGuid, DeferredStop> m_deferredStops;
};
