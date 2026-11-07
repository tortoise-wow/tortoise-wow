// Shared production reputation append and ACK-consumption implementation.

void ReputationMgr::SaveToDB()
{
    if (CharacterDatabase.HasOrderedPersistenceFault())
        return;
    if (CharacterDatabase.InOwnedTransaction())
    {
        CharacterDatabase.MarkOrderedPersistenceFault();
        return;
    }
    SaveToDB(nullptr);
}

bool ReputationMgr::AppendSaveToDB(SaveBatch& batch)
{
    if (batch.owner || CharacterDatabase.HasOrderedPersistenceFault() ||
        !CharacterDatabase.IsOwnedTransactionParticipant(m_player->GetGUIDLow()))
        return false;
    batch.owner = this;
    batch.prepared = SaveToDB(&batch);
    return batch.prepared;
}

bool ReputationMgr::CompleteSaveToDB(SaveBatch& batch)
{
    if (batch.owner != this || !batch.prepared)
        return false;
    for (RepListID id : batch.factions)
    {
        auto itr = m_factions.find(id);
        if (itr == m_factions.end() || !itr->second.needSave)
            return false;
    }
    for (RepListID id : batch.factions)
        m_factions.find(id)->second.needSave = false;
    batch.factions.clear();
    batch.prepared = false;
    return true;
}

bool ReputationMgr::SaveToDB(SaveBatch* batch)
{
    static SqlStatementID delRep;
    static SqlStatementID insRep;

    SqlStatement stmtDel = CharacterDatabase.CreateStatement(delRep, "DELETE FROM character_reputation WHERE guid = ? AND faction=?");
    SqlStatement stmtIns = CharacterDatabase.CreateStatement(insRep, "INSERT INTO character_reputation (guid,faction,standing,flags) VALUES (?, ?, ?, ?)");

    for (auto& m_faction : m_factions)
    {
        FactionState& faction = m_faction.second;
        if (faction.needSave)
        {
            if (!stmtDel.PExecute(m_player->GetGUIDLow(), faction.ID) ||
                !stmtIns.PExecute(m_player->GetGUIDLow(), faction.ID, faction.Standing, faction.Flags))
                return false;
            if (batch)
                batch->factions.push_back(m_faction.first);
            else
                faction.needSave = false;
        }
    }
    return true;
}
