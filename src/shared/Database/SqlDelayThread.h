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

#ifndef __SQLDELAYTHREAD_H
#define __SQLDELAYTHREAD_H

#include "LockedQueue.h"

#include <atomic>
#include <string>

class Database;
class SqlOperation;
class SqlConnection;

class SqlDelayThread
{
    typedef LockedQueue<SqlOperation*, std::mutex> SqlQueue;

    private:
        SqlQueue m_sqlQueue;                                ///< Queue of SQL statements
        SqlQueue m_priorityQueue;                           ///< Real-client/login work
        Database *m_dbEngine;                               ///< Pointer to used Database engine
        SqlQueue m_serialDelayQueue;
        SqlQueue m_prioritySerialDelayQueue;
        SqlConnection *m_dbConnection;                     ///< Pointer to DB connection
        std::atomic<bool> m_running;
        // BY VALUE, not a pointer. The caller hands this down from a local
        // std::string in Master::_StartDB (name.c_str()), which dies the
        // moment that function returns - after which the delay thread was
        // reading whatever the world thread had since put on that stack.
        std::string Name;


        //process all enqueued requests
        size_t ProcessRequests();

    public:
        SqlDelayThread(const char* InName, Database* db, SqlConnection* conn);
        ~SqlDelayThread();

        ///< Put sql statement to delay queue
        bool Delay(SqlOperation* sql) { m_sqlQueue.add(sql); return true; }
        void addSerialOperation(SqlOperation *op);
        void addPriorityOperation(SqlOperation* op) { m_priorityQueue.add(op); }
        void addPrioritySerialOperation(SqlOperation* op) { m_prioritySerialDelayQueue.add(op); }
        size_t PendingCount() const
        {
            return m_priorityQueue.size() + m_prioritySerialDelayQueue.size() + m_serialDelayQueue.size();
        }
        bool HasAsyncQuery();
        size_t DrainRequests() { return ProcessRequests(); } // after all workers join

        virtual void Stop();                                ///< Stop event
        void run();                                 ///< Main Thread loop
};
#endif                                                      //__SQLDELAYTHREAD_H
