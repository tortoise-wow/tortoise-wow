// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef MANGOS_CHARACTER_STORAGE_H
#define MANGOS_CHARACTER_STORAGE_H

#include "QueryResult.h"
#include <memory>

// Historical copies are read-only archives, never members of a runtime save transaction.
// Both startup and the real-connector tests use this exact metadata query and verdict.
template<class DatabaseConnection>
bool HasTransactionalCharacterStorage(DatabaseConnection& database)
{
    std::unique_ptr<QueryResult> result(database.Query(
        "SELECT COUNT(*), COALESCE(SUM(ENGINE IS NULL OR ENGINE <> 'InnoDB'), 0), "
        "@@GLOBAL.innodb_flush_log_at_trx_commit, @@GLOBAL.innodb_force_recovery "
        "FROM information_schema.TABLES WHERE TABLE_SCHEMA = DATABASE() "
        "AND TABLE_TYPE = 'BASE TABLE' AND TABLE_NAME NOT IN "
        "('character_bck','character_duplicate_account','character_reputation_broken',"
        "'character_skills_copy','characters_namecleanup2020_2')"));
    if (!result)
        return false;
    Field* fields = result->Fetch();
    return fields[0].GetUInt32() != 0 && fields[1].GetUInt32() == 0 &&
        fields[2].GetUInt32() == 1 && fields[3].GetUInt32() == 0;
}

#endif
