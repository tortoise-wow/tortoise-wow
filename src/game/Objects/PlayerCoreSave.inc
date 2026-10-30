// Shared production quest/skill/spell append and ACK implementation.

bool Player::AppendFullPlayerSave(PlayerSaveBatch& batch)
{
    if (batch.owner || !IsInventorySaveEnabled() || IsBeingTeleportedFar() ||
        CharacterDatabase.HasOrderedPersistenceFault() ||
        !CharacterDatabase.IsOwnedTransactionParticipant(GetGUIDLow()))
        return false;

    batch.owner = this;
    if (!_AppendCharacterRowToDB(true))
        return false;

    // Full mode replaces the inventory-only capture; never serialize NEW rows twice.
    batch.inventory.owner = this;
    if (!_SaveInventory(&batch.inventory))
        return false;
    batch.inventory.prepared = true;

    if (!_SaveQuestStatus(&batch) || !_SaveSkills(&batch) || !_SaveSpells(&batch) || !_SaveAuras() ||
        !m_reputationMgr.AppendSaveToDB(batch.reputation) || !m_honorMgr.AppendSaveToDB(batch.honor))
        return false;

    static SqlStatementID saveVariable;
    for (const auto& pair : m_variables)
    {
        SqlStatement stmt = CharacterDatabase.CreateStatement(saveVariable,
            "REPLACE INTO `character_variables` (`lowGuid`, `variableType`, `value`) VALUES (?, ?, ?)");
        stmt.addUInt32(GetGUIDLow());
        stmt.addUInt32(static_cast<uint32>(pair.first));
        stmt.addString(pair.second);
        if (!stmt.Execute())
            return false;
    }

    batch.itemLogCount = m_unsavedItemLogs.size();
    for (const auto& info : m_unsavedItemLogs)
        if (!CharacterDatabase.PExecute("INSERT INTO `character_item_logs` (`playerLowGuid`, `itemLowGuid`, `itemEntry`, `itemCount`, `action`, `timestamp`) VALUES ('%u', '%u', '%u', '%u', '%u', '%llu')",
            GetGUIDLow(), info.guidLow, info.entry, info.count, info.action, info.timestamp))
            return false;

    batch.prepared = true;
    return true;
}

bool Player::CompleteFullPlayerSave(PlayerSaveBatch& batch)
{
    if (batch.owner != this || !batch.prepared || batch.itemLogCount > m_unsavedItemLogs.size())
        return false;
    for (const auto& saved : batch.quests)
    {
        auto itr = mQuestStatus.find(saved.first);
        if (itr == mQuestStatus.end() || itr->second.uState != saved.second)
            return false;
    }
    for (const auto& saved : batch.skills)
    {
        auto itr = mSkillStatus.find(saved.first);
        if (itr == mSkillStatus.end() || itr->second.uState != saved.second)
            return false;
    }
    for (const auto& saved : batch.spells)
    {
        auto itr = m_spells.find(saved.first);
        if (itr == m_spells.end() || itr->second.state != saved.second)
            return false;
    }
    if (!CompleteInventoryAndGoldSave(batch.inventory) ||
        !m_reputationMgr.CompleteSaveToDB(batch.reputation) || !m_honorMgr.CompleteSaveToDB(batch.honor))
        return false;

    for (const auto& saved : batch.quests)
        if (saved.second == QUEST_DELETED)
            mQuestStatus.erase(saved.first);
        else
            mQuestStatus.find(saved.first)->second.uState = QUEST_UNCHANGED;
    for (const auto& saved : batch.skills)
        if (saved.second == SKILL_DELETED)
            mSkillStatus.erase(saved.first);
        else
            mSkillStatus.find(saved.first)->second.uState = SKILL_UNCHANGED;
    for (const auto& saved : batch.spells)
        if (saved.second == PLAYERSPELL_REMOVED)
            m_spells.erase(saved.first);
        else
            m_spells.find(saved.first)->second.state = PLAYERSPELL_UNCHANGED;
    m_unsavedItemLogs.erase(m_unsavedItemLogs.begin(), m_unsavedItemLogs.begin() + batch.itemLogCount);
    batch.prepared = false;
    return true;
}

bool Player::_SaveQuestStatus(PlayerSaveBatch* batch)
{
    static SqlStatementID insertQuestStatus ;

    static SqlStatementID updateQuestStatus ;

    // The caller owns the transaction and, in deferred mode, consumption after ACK.
    for (QuestStatusMap::iterator i = mQuestStatus.begin(); i != mQuestStatus.end();)
    {
        // Nostalrius
        if (i->second.uState == QUEST_DELETED)
        {
            static SqlStatementID deleteQuestStatus ;
            SqlStatement stmt = CharacterDatabase.CreateStatement(deleteQuestStatus, "DELETE FROM character_queststatus WHERE guid = ? AND quest = ?");
            if (!stmt.PExecute(GetGUIDLow(), i->first))
                return false;
            if (batch)
            {
                batch->quests.emplace_back(i->first, i->second.uState);
                ++i;
            }
            else
                i = mQuestStatus.erase(i);
            continue;
        }
        switch (i->second.uState)
        {
            case QUEST_NEW :
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(insertQuestStatus, "REPLACE INTO character_queststatus (guid,quest,status,rewarded,explored,timer,mobcount1,mobcount2,mobcount3,mobcount4,itemcount1,itemcount2,itemcount3,itemcount4,reward_choice) "
                                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

                stmt.addUInt32(GetGUIDLow());
                stmt.addUInt32(i->first);
                stmt.addUInt8(i->second.m_status);
                stmt.addUInt8(i->second.m_rewarded);
                stmt.addUInt8(i->second.m_explored);
                stmt.addUInt64(uint64(i->second.m_timer / IN_MILLISECONDS + sWorld.GetGameTime()));
                for (uint32 k : i->second.m_creatureOrGOcount)
                    stmt.addUInt32(k);
                for (uint32 k : i->second.m_itemcount)
                    stmt.addUInt32(k);
                stmt.addUInt32(i->second.m_reward_choice);
                if (!stmt.Execute())
                    return false;
            }
            break;
            case QUEST_CHANGED :
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(updateQuestStatus, "UPDATE character_queststatus SET status = ?,rewarded = ?,reward_choice = ?,explored = ?,timer = ?,"
                                    "mobcount1 = ?,mobcount2 = ?,mobcount3 = ?,mobcount4 = ?,itemcount1 = ?,itemcount2 = ?,itemcount3 = ?,itemcount4 = ?  WHERE guid = ? AND quest = ?");

                stmt.addUInt8(i->second.m_status);
                stmt.addUInt8(i->second.m_rewarded);
                stmt.addUInt32(i->second.m_reward_choice);
                stmt.addUInt8(i->second.m_explored);
                stmt.addUInt64(uint64(i->second.m_timer / IN_MILLISECONDS + sWorld.GetGameTime()));
                for (uint32 k : i->second.m_creatureOrGOcount)
                    stmt.addUInt32(k);
                for (uint32 k : i->second.m_itemcount)
                    stmt.addUInt32(k);
                stmt.addUInt32(GetGUIDLow());
                stmt.addUInt32(i->first);
                if (!stmt.Execute())
                    return false;
            }
            break;
            case QUEST_UNCHANGED:
                break;
        };
        if (batch)
        {
            if (i->second.uState != QUEST_UNCHANGED)
                batch->quests.emplace_back(i->first, i->second.uState);
        }
        else
            i->second.uState = QUEST_UNCHANGED;
        ++i;
    }
    return true;
}

bool Player::_SaveSkills(PlayerSaveBatch* batch)
{
    static SqlStatementID delSkills;
    static SqlStatementID insSkills;
    static SqlStatementID updSkills;

    // The caller owns the transaction and, in deferred mode, consumption after ACK.
    for (SkillStatusMap::iterator itr = mSkillStatus.begin(); itr != mSkillStatus.end();)
    {
        if (itr->second.uState == SKILL_UNCHANGED)
        {
            ++itr;
            continue;
        }

        if (itr->second.uState == SKILL_DELETED)
        {
            SqlStatement stmt = CharacterDatabase.CreateStatement(delSkills, "DELETE FROM character_skills WHERE guid = ? AND skill = ?");
            if (!stmt.PExecute(GetGUIDLow(), itr->first))
                return false;
            if (batch)
            {
                batch->skills.emplace_back(itr->first, itr->second.uState);
                ++itr;
            }
            else
                itr = mSkillStatus.erase(itr);
            continue;
        }

        uint32 valueData = GetUInt32Value(PLAYER_SKILL_VALUE_INDEX(itr->second.pos));
        uint16 value = SKILL_VALUE(valueData);
        uint16 max = SKILL_MAX(valueData);

        switch (itr->second.uState)
        {
            case SKILL_NEW:
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(insSkills, "INSERT INTO character_skills (guid, skill, value, max) VALUES (?, ?, ?, ?)");
                if (!stmt.PExecute(GetGUIDLow(), itr->first, value, max))
                    return false;
            }
            break;
            case SKILL_CHANGED:
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(updSkills, "UPDATE character_skills SET value = ?, max = ? WHERE guid = ? AND skill = ?");
                if (!stmt.PExecute(value, max, GetGUIDLow(), itr->first))
                    return false;
            }
            break;
            case SKILL_UNCHANGED:
            case SKILL_DELETED:
                MANGOS_ASSERT(false);
                break;
        };
        if (batch)
            batch->skills.emplace_back(itr->first, itr->second.uState);
        else
            itr->second.uState = SKILL_UNCHANGED;

        ++itr;
    }

    // Forgotten weapon skills.
    static SqlStatementID forSkills;

    for (const auto itr : m_mForgottenSkills)
    {
        if (itr.second > 1)
        {
            SqlStatement stmt = CharacterDatabase.CreateStatement(forSkills, "REPLACE INTO character_forgotten_skills (guid, skill, value) VALUES (?, ?, ?)");
            if (!stmt.PExecute(GetGUIDLow(), itr.first, itr.second))
                return false;
        }
    }
    return true;
}

bool Player::_SaveSpells(PlayerSaveBatch* batch)
{
    static SqlStatementID delSpells ;
    static SqlStatementID insSpells ;

    SqlStatement stmtDel = CharacterDatabase.CreateStatement(delSpells, "DELETE FROM character_spell WHERE guid = ? and spell = ?");
    SqlStatement stmtIns = CharacterDatabase.CreateStatement(insSpells, "REPLACE INTO character_spell (guid,spell,active,disabled) VALUES (?, ?, ?, ?)");

    for (PlayerSpellMap::iterator itr = m_spells.begin(); itr != m_spells.end();)
    {
        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.state == PLAYERSPELL_CHANGED)
            if (!stmtDel.PExecute(GetGUIDLow(), itr->first))
                return false;

        // add only changed/new not dependent spells
        if (!itr->second.dependent && (itr->second.state == PLAYERSPELL_NEW || itr->second.state == PLAYERSPELL_CHANGED))
            if (!stmtIns.PExecute(GetGUIDLow(), itr->first, uint8(itr->second.active ? 1 : 0), uint8(itr->second.disabled ? 1 : 0)))
                return false;

        if (batch)
        {
            if (itr->second.state != PLAYERSPELL_UNCHANGED)
            {
                // MSVC can preserve the bitfield expression through even an
                // enum cast during forwarding deduction; materialize a value first.
                const PlayerSpellState savedState = static_cast<PlayerSpellState>(itr->second.state);
                batch->spells.emplace_back(itr->first, savedState);
            }
            ++itr;
        }
        else if (itr->second.state == PLAYERSPELL_REMOVED)
            m_spells.erase(itr++);
        else
        {
            itr->second.state = PLAYERSPELL_UNCHANGED;
            ++itr;
        }

    }
    return true;
}
