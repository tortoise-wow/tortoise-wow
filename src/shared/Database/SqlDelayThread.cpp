#include "Util/DevDiagnostics.h"
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

#include "Database/SqlDelayThread.h"
#include "Database/SqlOperations.h"
#include "DatabaseEnv.h"

SqlDelayThread::SqlDelayThread(const char* InName, Database* db, SqlConnection* conn)
    : m_dbEngine(db), m_dbConnection(conn), m_running(true), Name(InName ? InName : "")
{
}

SqlDelayThread::~SqlDelayThread()
{
    //process all requests which might have been queued while thread was stopping
    while (ProcessRequests()) {}
    delete m_dbConnection;
}

void SqlDelayThread::addSerialOperation(SqlOperation *op)
{
    m_serialDelayQueue.add(op);
}

bool SqlDelayThread::HasAsyncQuery()
{
    return PendingCount() != 0;
}

void SqlDelayThread::run()
{
    #ifndef DO_POSTGRESQL
    mysql_thread_init();
    #endif

    char ThreadName[128];
    // snprintf, not sprintf: the source used to be a dangling pointer, so
    // whether this overflowed came down to where the next zero byte happened
    // to sit in a reused stack frame. The name is owned now, but a bounded
    // write costs nothing and closes the door.
    snprintf(ThreadName, sizeof(ThreadName), "SqlDelay %s", Name.c_str());

    thread_name(ThreadName);
    const uint32 loopSleepms = 10;

    const uint32 pingEveryLoop = m_dbEngine->GetPingIntervall() / loopSleepms;

    uint32 loopCounter = 0;
    while (m_running.load(std::memory_order_acquire))
    {
        // if the running state gets turned off while sleeping
        // empty the queue before exiting
        std::this_thread::sleep_for(std::chrono::milliseconds(loopSleepms));

        ProcessRequests();

        if((loopCounter++) >= pingEveryLoop)
        {
            loopCounter = 0;
            m_dbEngine->Ping();
            if (QueryResult* res = m_dbConnection->Query("SELECT 1"))
                delete res;
        }
    }

    // Preserve every accepted operation, not just one capped batch, on shutdown.
    while (ProcessRequests()) {}
    #ifndef DO_POSTGRESQL
    mysql_thread_end();
    #endif
}

void SqlDelayThread::Stop()
{
    m_running.store(false, std::memory_order_release);
}

size_t SqlDelayThread::ProcessRequests()
{
    MANTECH_DIAG_SCOPE(DbExecute, 1, nullptr);

    SqlOperation* s = nullptr;
    size_t processed = 0;

    uint32 priorityProcessed = 0;
    while (priorityProcessed++ < 32 && (m_prioritySerialDelayQueue.next(s) || m_priorityQueue.next(s)))
    {
        ++processed;
        bool result = s->Execute(m_dbConnection);
        const auto& callback = s->GetCallback();
        if (callback)
            (*callback)(result);
        delete s;
    }

    uint32 normalProcessed = 0;
    while (normalProcessed++ < 64 && m_dbEngine->NextDelayedOperation(s))
    {
        ++processed;
        bool result = s->Execute(m_dbConnection);
        const auto& callback = s->GetCallback();
        if (callback)
            (*callback)(result);
        delete s;
    }

    // Process any serial operations for this worker
    uint32 serialProcessed = 0;
    while (serialProcessed++ < 64 && m_serialDelayQueue.next(s))
    {
        ++processed;
        bool result = s->Execute(m_dbConnection);
        const auto& callback = s->GetCallback();
        if (callback)
            (*callback)(result);
        delete s;
    }
    return processed;
}
