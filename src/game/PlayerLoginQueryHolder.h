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
#include "Database/SqlOperations.h"
#include "ObjectGuid.h"
#include "SessionTransport.h"

class LoginQueryHolder : public SqlQueryHolder
{
public:
    LoginQueryHolder(uint32 accountId, ObjectGuid guid,
        SessionTransport transport = SessionTransport::Network, uint64 requestToken = 0)
        : SqlQueryHolder(guid.GetCounter()), m_accountId(accountId), m_guid(guid),
          m_transport(transport), m_requestToken(requestToken) { }

    ~LoginQueryHolder() override
    {
        // Queries should NOT be deleted by user
        DeleteAllResults();
    }

    ObjectGuid GetGuid() const { return m_guid; }
    uint32 GetAccountId() const { return m_accountId; }
    SessionTransport GetTransport() const { return m_transport; }
    uint64 GetRequestToken() const { return m_requestToken; }
    bool Initialize();
    void SetLoginRequestTime(uint32 value) { m_loginRequestTime = value; }
    uint32 GetLoginRequestTime() const { return m_loginRequestTime; }

private:
    friend class HeadlessSessionMgr;
    uint32 const m_accountId;
    ObjectGuid const m_guid;
    SessionTransport const m_transport;
    uint64 m_requestToken;
    uint32 m_loginRequestTime = 0;
};
