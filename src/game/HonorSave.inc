// Production honor SQL append and actor-side ACK consumption. No worker owns
// this batch or its manager/player pointer; SQL requests copy scalar values.
#include <cstring>

namespace
{
bool HonorSaveFloatEqual(float a, float b)
{
    return std::memcmp(&a, &b, sizeof(float)) == 0;
}

bool HonorSaveEntryEqual(HonorCP const& a, HonorCP const& b)
{
    return a.victimType == b.victimType && a.victimId == b.victimId &&
        HonorSaveFloatEqual(a.cp, b.cp) && a.date == b.date && a.type == b.type && a.state == b.state;
}
}

bool HonorMgr::AppendHonorCP(HonorCP const& entry)
{
    return CharacterDatabase.PExecute("INSERT INTO `character_honor_cp` (`guid`, `victimType`, `victim`, `cp`, `date`, `type`) "
        " VALUES (%u, %u, %u, %.1f, %u, %u)", m_owner->GetGUIDLow(), entry.victimType, entry.victimId,
        finiteAlways(entry.cp), entry.date, entry.type);
}

bool HonorMgr::SaveStoredDataChecked()
{
    return m_owner && CharacterDatabase.PExecute("UPDATE `characters` SET `honorRankPoints` = %.1f, `honorStanding` = %u, `honorHighestRank` = %u, "
        "`honorLastWeekHK` = %u, `honorLastWeekCP` = %.1f, `honorStoredHK` = %u, `honorStoredDK` = %u WHERE `guid` = %u",
        finiteAlways(m_rankPoints), m_standing, m_highestRank.rank, m_lastWeekHK,
        finiteAlways(m_lastWeekCP), m_storedHK, m_storedDK, m_owner->GetGUIDLow());
}

bool HonorMgr::SaveCurrencyChecked()
{
    if (!m_owner) return false;
    ResetCurrencyWeekIfNeeded();
    return CharacterDatabase.PExecute("REPLACE INTO `character_pvp_currency` (`guid`, `honor`, `conquest`, `weekly_honor`, `week_begin_day`) "
        "VALUES (%u, %u, %u, %u, %u)", m_owner->GetGUIDLow(), m_spendableHonor, m_conquestPoints, m_weeklySpendableHonor, m_currencyWeekBeginDay);
}

void HonorMgr::SaveStoredData() { SaveStoredDataChecked(); }
void HonorMgr::SaveCurrency() { SaveCurrencyChecked(); }

void HonorMgr::ResetCurrencyWeekIfNeeded()
{
    uint32 weekBeginDay = sHonorMaintenancer.GetWeekBeginDay();
    if (!weekBeginDay) weekBeginDay = sWorld.GetGameDay();
    if (m_currencyWeekBeginDay != weekBeginDay)
    {
        m_weeklySpendableHonor = 0;
        m_currencyWeekBeginDay = weekBeginDay;
    }
}

void HonorMgr::Save()
{
    if (!m_owner || CharacterDatabase.HasOrderedPersistenceFault()) return;
    if (CharacterDatabase.InOwnedTransaction())
    {
        // Owned snapshots must keep NEW entries until their outer ACK.
        CharacterDatabase.MarkOrderedPersistenceFault();
        return;
    }
    // Preserve ordinary save's historical optimistic state consumption. Its
    // durable retry/intent handling remains A06; the owned path below is checked.
    HonorCPMap saved;
    for (auto& entry : m_honorCP)
    {
        if (entry.state == STATE_NEW)
        {
            AppendHonorCP(entry);
            entry.state = STATE_UNCHANGED;
            saved.push_back(entry);
        }
        else if (entry.state == STATE_UNCHANGED) saved.push_back(entry);
    }
    m_honorCP.swap(saved);
    SaveCurrency();
    SaveStoredData();
}

bool HonorMgr::AppendSaveToDB(SaveBatch& batch)
{
    if (!m_owner || batch.owner || CharacterDatabase.HasOrderedPersistenceFault() ||
        !CharacterDatabase.IsOwnedTransactionParticipant(m_owner->GetGUIDLow())) return false;
    ResetCurrencyWeekIfNeeded();
    batch.owner = this;
    batch.guid = m_owner->GetGUIDLow();
    batch.entries.assign(m_honorCP.begin(), m_honorCP.end());
    batch.rankPoints = m_rankPoints; batch.lastWeekCP = m_lastWeekCP;
    batch.standing = m_standing; batch.highestRank = m_highestRank.rank;
    batch.lastWeekHK = m_lastWeekHK; batch.storedHK = m_storedHK; batch.storedDK = m_storedDK;
    batch.honor = m_spendableHonor; batch.conquest = m_conquestPoints;
    batch.weeklyHonor = m_weeklySpendableHonor; batch.weekBegin = m_currencyWeekBeginDay;
    for (auto const& entry : batch.entries)
    {
        if (entry.state != STATE_NEW && entry.state != STATE_UNCHANGED) return false;
        if (entry.state == STATE_NEW && !AppendHonorCP(entry)) return false;
    }
    if (!SaveCurrencyChecked() || !SaveStoredDataChecked()) return false;
    batch.prepared = true;
    return true;
}

bool HonorMgr::CompleteSaveToDB(SaveBatch& batch)
{
    if (CharacterDatabase.InOwnedTransaction() || !m_owner || batch.owner != this || !batch.prepared || batch.guid != m_owner->GetGUIDLow() ||
        batch.entries.size() != m_honorCP.size() || !HonorSaveFloatEqual(batch.rankPoints, m_rankPoints) ||
        !HonorSaveFloatEqual(batch.lastWeekCP, m_lastWeekCP) || batch.standing != m_standing ||
        batch.highestRank != m_highestRank.rank || batch.lastWeekHK != m_lastWeekHK ||
        batch.storedHK != m_storedHK || batch.storedDK != m_storedDK ||
        batch.honor != m_spendableHonor || batch.conquest != m_conquestPoints ||
        batch.weeklyHonor != m_weeklySpendableHonor || batch.weekBegin != m_currencyWeekBeginDay) return false;
    auto current = m_honorCP.begin();
    for (auto const& entry : batch.entries)
    {
        if (!HonorSaveEntryEqual(entry, *current)) return false;
        ++current;
    }
    // Validate the whole capture before consuming even one dirty entry.
    for (auto& entry : m_honorCP)
        if (entry.state == STATE_NEW) entry.state = STATE_UNCHANGED;
    batch.entries.clear();
    batch.prepared = false;
    return true;
}
