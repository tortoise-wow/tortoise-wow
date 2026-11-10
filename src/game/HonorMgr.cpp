/*
 * Copyright (C) 2016 Elysium Project <https://elysium-project.org>
 */

#include "Formulas.h"
#include "HonorMgr.h"
#include "Language.h"
#include "World.h"
#include "Creature.h"
#include "Player.h"
#include "Database/DatabaseEnv.h"
#include "Policies/SingletonImp.h"
#include "ObjectAccessor.h"
#include "CharacterDatabaseCleaner.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>

#define HONOR_TO_RANK_POINTS 10.0f
#define CONQUEST_TO_RANK_POINTS 100.0f

static const float RankPointBounds[POSITIVE_HONOR_RANK_COUNT][2] =
{
    { 0.0f,      0.0f       }, // Rank 0 - Mostly for padding to make matrix declarations line up with real rank numbers
    { 0.0f,      2000.0f    }, // Rank 1
    { 2000.0f,   5000.0f    }, // Rank 2
    { 5000.0f,   10000.0f   }, // Rank 3
    { 10000.0f,  20000.0f   }, // Rank 4
    { 20000.0f,  35000.0f   }, // Rank 5
    { 35000.0f,  55000.0f   }, // Rank 6
    { 55000.0f,  80000.0f   }, // Rank 7
    { 80000.0f,  110000.0f  }, // Rank 8
    { 110000.0f, 145000.0f  }, // Rank 9
    { 145000.0f, 185000.0f  }, // Rank 10
    { 185000.0f, 300000.0f  }, // Rank 11
    { 300000.0f, 475000.0f  }, // Rank 12
    { 475000.0f, 700000.0f  }, // Rank 13
    { 700000.0f, 1000000.0f }  // Rank 14
};

HonorMaintenancer sHonorMaintenancer;

uint32 HonorMgr::m_mostHkYesterdayGuid = 0;
uint32 HonorMgr::m_mostDkYesterdayGuid = 0;

void HonorMgr::LoadMostDkHkYesterdayPlayers()
{
    sLog.outInfo("Loading players that had the most HK and DK yesterday.");

    uint32 maxDay = 0;
    std::map<uint32 /*guid*/, uint32 /*hk*/> hkRanking;
    std::map<uint32 /*guid*/, uint32 /*dk*/> dkRanking;
    QueryResult* result = CharacterDatabase.Query("SELECT `guid`, `type`, `date` FROM `character_honor_cp`");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            
            uint32 guid = fields[0].GetUInt32();
            uint32 type = fields[1].GetUInt32();
            uint32 date = fields[2].GetUInt32();

            if (date > maxDay)
            {
                maxDay = date;
                hkRanking.clear();
                dkRanking.clear();
            }
            else if (date < maxDay)
                continue;

            if (type == HONORABLE)
            {
                hkRanking[guid]++;
            }
            else if (type == DISHONORABLE)
            {
                dkRanking[guid]++;
            }

        } while (result->NextRow());
        delete result;
    }

    std::pair<uint32, uint32> mostHk;
    for (auto const& itr : hkRanking)
    {
        if (itr.second > mostHk.second)
        {
            mostHk.first = itr.first;
            mostHk.second = itr.second;
        }
    }
    m_mostHkYesterdayGuid = mostHk.first;

    std::pair<uint32, uint32> mostDk;
    for (auto const& itr : dkRanking)
    {
        if (itr.second > mostDk.second)
        {
            mostDk.first = itr.first;
            mostDk.second = itr.second;
        }
    }
    m_mostDkYesterdayGuid = mostDk.first;

    sLog.outInfo("Most HK by %u, Most DK by %u", m_mostHkYesterdayGuid, m_mostDkYesterdayGuid);

    if (m_mostHkYesterdayGuid)
    {
        if (PlayerCacheData* pData = sObjectMgr.GetPlayerDataByGUID(m_mostHkYesterdayGuid))
        {
            if (Quest* pQuest = (Quest*)sObjectMgr.GetQuestTemplate(QUEST_DAILY_MOST_HK))
            {
                std::string txt = "End " + pData->sName + "'s life.";
                pQuest->SetEndText(txt);
            }
        }
    }

    if (m_mostDkYesterdayGuid)
    {
        if (PlayerCacheData* pData = sObjectMgr.GetPlayerDataByGUID(m_mostDkYesterdayGuid))
        {
            if (Quest* pQuest = (Quest*)sObjectMgr.GetQuestTemplate(QUEST_DAILY_MOST_DK))
            {
                std::string txt = "End " + pData->sName + "'s life.";
                pQuest->SetEndText(txt);
            }
        }
    }
}

void HonorMaintenancer::LoadWeeklyScores()
{
    uint32 weekBeginDay = GetWeekBeginDay();
    uint32 weekEndDay = GetWeekEndDay();

    std::ostringstream query;

    query << "SELECT `scores`.`guid`, `c`.`level`, `c`.`account`, `c`.`honorRankPoints`, `c`.`honorHighestRank`, SUM(`hk`), SUM(`dk`), SUM(`cp`) FROM"
        "("
        "  SELECT `guid` AS `guid`, COUNT(*) AS `hk`, 0 AS `dk`, SUM(`cp`) AS `cp` FROM `character_honor_cp` WHERE `type` = " << HONORABLE <<
        "  AND (`date` BETWEEN " << weekBeginDay << " AND " << weekEndDay << ") GROUP BY `guid`"
        "  UNION"
        "  SELECT `guid` AS `guid`, 0 AS `hk`, COUNT(*) AS `dk`, 0 AS `cp` FROM `character_honor_cp` WHERE `type` = " << DISHONORABLE <<
        "  AND (`date` BETWEEN " << weekBeginDay << " AND " << weekEndDay << ") GROUP BY `guid`"
        "  UNION"
        "  SELECT `guid` AS `guid`, 0 AS `hk`, 0 AS `dk`, SUM(`cp`) AS `cp` FROM `character_honor_cp` WHERE `type` NOT IN (" << HONORABLE << ", " << DISHONORABLE << ")"
        "  AND (`date` BETWEEN " << weekBeginDay << " AND " << weekEndDay << ") GROUP BY `guid`"
        "  UNION"
        "  SELECT `guid` AS `guid`, 0 AS `hk`, 0 AS `dk`, 0 AS `cp` FROM `characters` WHERE `honorRankPoints` > 0"
        ") AS `scores` INNER JOIN `characters` AS `c` ON `scores`.`guid` = `c`.`guid` GROUP BY `guid` ORDER BY `guid` ";

    QueryResult* result = CharacterDatabase.Query(query.str().c_str());

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            WeeklyScore score;
            score.level  = fields[1].GetUInt32();
            score.account = fields[2].GetUInt32();
            score.oldRp  = fields[3].GetFloat();
            score.highestRank = fields[4].GetUInt32();
            score.hk  = fields[5].GetUInt32();
            score.dk  = fields[6].GetUInt32();
            score.cp  = fields[7].GetFloat();
            m_weeklyScores[fields[0].GetUInt32()] = score;
        }
        while (result->NextRow());
        delete result;
    }
}

void HonorMaintenancer::DecayRankPoints()
{
    for (auto& pair : m_weeklyScores)
    {
        auto& weeklyScore = pair.second;

        weeklyScore.earning = 0.0f;
        weeklyScore.standing = 0;
        weeklyScore.newRp = finiteAlways(CalculateRpDecay(0.0f, weeklyScore));
        weeklyScore.newRp = std::min(HonorMgr::MaximumRpAtLevel(weeklyScore.level), weeklyScore.newRp);
    }
}

void HonorMaintenancer::SetCityRanks()
{
    CharacterDatabase.Execute("UPDATE `characters` SET `city_protector` = 0");

    std::map<uint8, std::pair<uint32, uint32>> highestStandingInRace =
    {
        {RACE_HUMAN, {0,0}},
        {RACE_ORC, {0,0}},
        {RACE_DWARF, {0,0}},
        {RACE_NIGHTELF, {0,0}},
        {RACE_UNDEAD, {0,0}},
        {RACE_TAUREN, {0,0}},
        {RACE_GNOME, {0,0}},
        {RACE_TROLL, {0,0}},
        {RACE_GOBLIN, {0,0}},
        {RACE_HIGH_ELF, {0,0}},
    };

    for (uint8 i = 1; i < MAX_RACES; ++i)
    {
        QueryResult *result = CharacterDatabase.PQuery("SELECT `guid`, `honorLastWeekHK` FROM characters WHERE `honorLastWeekHK` > 0 and race = '%u' ORDER BY `honorLastWeekHK` DESC LIMIT 1", i);

        if (result)
        {
            do
            {
                Field *fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                uint32 kills = fields[1].GetUInt32();

                highestStandingInRace[i] = std::make_pair(guid, kills);

            } while (result->NextRow());
        }

        delete result;
    }

    for (auto& standing : highestStandingInRace)
    {
        uint32 lowGuid = standing.second.first;

        if (lowGuid > 0)
            CharacterDatabase.PExecute("UPDATE `characters` SET city_protector = 1 WHERE `guid` = '%u'", standing.second.first);
    }
}

void HonorMaintenancer::FlushWeeklyQuests()
{
    //CharacterDatabase.PExecute("DELETE FROM `character_queststatus` WHERE `quest` IN (50322, 50323)");
    CharacterDatabase.PExecute("DELETE FROM `character_queststatus` WHERE `quest` IN (%u, %u)", QUEST_DAILY_MOST_DK, QUEST_DAILY_MOST_HK);
}

void HonorMaintenancer::FlushRankPoints()
{
    // Immediately reset honor standing before flushing
    CharacterDatabase.Execute("UPDATE `characters` SET `honorStanding` = 0 WHERE `honorStanding` > 0");

    for (auto& pair : m_weeklyScores)
    {
        auto weeklyScore = pair.second;

        HonorRankInfo currentRank = HonorMgr::CalculateRank(weeklyScore.newRp);
        HonorRankInfo highestRank;
        HonorMgr::InitRankInfo(highestRank);
        highestRank.rank = weeklyScore.highestRank;
        HonorMgr::CalculateRankInfo(highestRank);

        if (currentRank.visualRank > 0 && (currentRank.visualRank > highestRank.visualRank))
            highestRank = currentRank;

        CharacterDatabase.PExecute("UPDATE `characters` SET `honorHighestRank` = %u, `honorRankPoints` = %.1f, `honorStanding` = %u, "
            "`honorLastWeekHK` = %u, `honorStoredHK` = (`honorStoredHK` + %u), `honorStoredDK` = (`honorStoredDK` + %u), `honorLastWeekCP` = %.1f WHERE `guid` = %u",
            highestRank.rank,
            finiteAlways(weeklyScore.newRp), weeklyScore.standing,
            weeklyScore.hk, weeklyScore.hk, weeklyScore.dk,
            finiteAlways(weeklyScore.cp), pair.first);
    }

    // Not includes weekend day, for correct view in honor tab for group "Yesterday"
    CharacterDatabase.PExecute("DELETE FROM `character_honor_cp` WHERE `date` < %u", GetWeekEndDay());
}

void HonorMaintenancer::DoMaintenance()
{
    if (!m_markerToStart)
        return;

    if (sWorld.getConfig(CONFIG_BOOL_BACKUP_CHARACTER_INVENTORY))
        sObjectMgr.BackupCharacterInventory();

    sLog.outInfo("Beginning character name cleanup...");
    CharacterDatabaseCleaner::FreeInactiveCharacterNames();

    sLog.outHonor("[MAINTENANCE] Honor maintenance starting.");

    {
        std::ofstream honorUpdateFile{ "honorupdate.txt" };
        if (honorUpdateFile)
            honorUpdateFile << "1";
    }

    sLog.outHonor("[MAINTENANCE] Load weekly players scores.");
    LoadWeeklyScores();
    sLog.outHonor("[MAINTENANCE] Decay rank points.");
    DecayRankPoints();
    sLog.outHonor("[MAINTENANCE] Flush rank points.");
    FlushRankPoints();
    sLog.outHonor("[MAINTENANCE] Assign city ranks.");
    SetCityRanks();
    sLog.outHonor("[MAINTENANCE] Flush weekly quests.");
    FlushWeeklyQuests();

    CreateCalculationReport();
    sLog.outHonor("[MAINTENANCE] Honor maintenance finished.");

    ToggleMaintenanceMarker();
    SetMaintenanceDays(GetNextMaintenanceDay());
}

void HonorMaintenancer::CreateCalculationReport()
{
    std::string timestamp = Log::GetTimestampStr();
    std::string filename = "HCR_" + timestamp + ".txt";
    std::string path = sWorld.GetHonorPath() + filename;

    std::ofstream ofs;
    ofs.open(path.c_str());
    if (!ofs.is_open())
    {
        sLog.outError("Can't create HCR file!");
        return;
    }

    ofs << "Honor Rank Point Decay" << std::endl << std::endl;
    ofs << "Tracked players: " << m_weeklyScores.size() << std::endl << std::endl;
    ofs << std::flush;

    for (auto& pair : m_weeklyScores)
    {
        auto ws = pair.second;

        ofs << "Guid: " << pair.first
            << ", HK: " << ws.hk
            << ", DK: " << ws.dk
            << ", CP: " << ws.cp
            << ", oldRp: " << ws.oldRp
            << ", earning: " << ws.earning
            << ", newRp: " << ws.newRp
            << ", standing: " << ws.standing << std::endl << std::flush;
    }

    ofs.close();
}

float HonorMaintenancer::CalculateRpDecay(float rpEarning, const WeeklyScore& wk)
{
    float decay = floor((0.2f * wk.oldRp) + 0.5f);
    float delta = rpEarning - decay;

    if (delta < 0)
        delta = delta / 2;

    if (delta < -2500)
        delta = -2500;

    float newRp = wk.oldRp + delta;

    if (wk.highestRank > 1 + NEGATIVE_HONOR_RANK_COUNT)
    {
        uint8 visualRank = wk.highestRank - NEGATIVE_HONOR_RANK_COUNT;
        float minRpForRank = RankPointBounds[visualRank][0];
        if (newRp < minRpForRank)
            newRp = minRpForRank;
    }
    return newRp;
}

float HonorMgr::MaximumRpAtLevel(uint8 level)
{
    if (level <= 29)
        return 6500.0f;
    if (level >= 30 && level <= 35)
        return 7150.0f + 1380.0f * (level - 30);
    if (level >= 36 && level <= 39)
        return 14050.0f + 3156.25f * (level - 35);
    if (level >= 40 && level <= 43)
        return 26675.0f + 5806.25f * (level - 39);
    if (level >= 44 && level <= 52)
        return 49900.0f + 14300.0f * (level - 43);
    if (level >= 53 && level <= 60)
        return 178600.0f + 102675.0f * (level - 52);
    return 1000000.0f;
}

void HonorMaintenancer::CheckMaintenanceDay()
{
    if (sWorld.GetGameDay() >= m_nextMaintenanceDay && !m_markerToStart)
    {
        // Restart 15 minutes after honor weekend by server time
        if (sWorld.getConfig(CONFIG_BOOL_AUTO_HONOR_RESTART))
            sWorld.ShutdownServ(900, SHUTDOWN_MASK_RESTART, SHUTDOWN_EXIT_CODE);
        else
            sLog.outString("HonorMaintenancer: Server needs to be restarted to perform honor rank calculations.");

        ToggleMaintenanceMarker();
    }
}

void HonorMaintenancer::ToggleMaintenanceMarker()
{
    m_markerToStart = !m_markerToStart;
    CharacterDatabase.PExecute("INSERT INTO `saved_variables` (`key`, `honorMaintenanceMarker`) VALUES (0, %u) "
        "ON DUPLICATE KEY UPDATE `honorMaintenanceMarker` = %u", m_markerToStart, m_markerToStart);
}

void HonorMaintenancer::SetMaintenanceDays(uint32 last, uint32 next)
{
    m_lastMaintenanceDay = last;

    if (!next)
        m_nextMaintenanceDay = m_lastMaintenanceDay + 7;

    CharacterDatabase.PExecute("INSERT INTO `saved_variables` (`key`, `lastHonorMaintenanceDay`, `nextHonorMaintenanceDay`) VALUES (0, %u, %u) "
        "ON DUPLICATE KEY UPDATE `lastHonorMaintenanceDay` = %u, `nextHonorMaintenanceDay` = %u",
        m_lastMaintenanceDay, m_nextMaintenanceDay, m_lastMaintenanceDay, m_nextMaintenanceDay);
}

void HonorMaintenancer::Initialize()
{
    QueryResult* result = CharacterDatabase.Query("SELECT `lastHonorMaintenanceDay`, `nextHonorMaintenanceDay`, `honorMaintenanceMarker` FROM `saved_variables`");
    if (result)
    {
        Field* fields = result->Fetch();
        m_lastMaintenanceDay = fields[0].GetUInt32();
        m_nextMaintenanceDay = fields[1].GetUInt32();
        m_markerToStart = fields[2].GetBool();
        delete result;
    }

    if (!m_lastMaintenanceDay)
        SetMaintenanceDays(sWorld.GetLastMaintenanceDay());
}

void HonorMgr::ClearHonorData()
{
    m_honorCP.clear();
    m_totalHK = 0;
    m_totalDK = 0;
    m_storedHK = 0;
    m_storedDK = 0;
    m_standing = 0;
    m_lastWeekHK = 0;
    m_rankPoints = 0.0f;
    m_lastWeekCP = 0.0f;
    InitRankInfo(m_rank);
    InitRankInfo(m_highestRank);
}

void HonorMgr::Reset()
{
    if (!m_owner)
        return;

    ClearHonorData();

    // It will delete all honor cp from database imediatly
    CharacterDatabase.PExecute("DELETE FROM `character_honor_cp` WHERE `guid` = %u", m_owner->GetGUIDLow());
    SaveStoredData();

    Update();
}

void HonorMgr::ClearHonorCP()
{
    m_honorCP.clear();
}

#include "HonorSave.inc"

void HonorMgr::Load(QueryResult* result)
{
    if (result)
    {
        m_honorCP.clear();

        do
        {
            Field* fields = result->Fetch();

            HonorCP honorCP;
            honorCP.victimType = fields[0].GetUInt8();
            honorCP.victimId   = fields[1].GetUInt32();
            honorCP.cp         = fields[2].GetFloat();
            honorCP.date       = fields[3].GetUInt32();
            honorCP.type       = fields[4].GetUInt8();
            honorCP.state      = STATE_UNCHANGED;

            m_honorCP.push_back(honorCP);
        }
        while (result->NextRow());

        // result will be delete in character query holder later
    }
}

void HonorMgr::LoadCurrency(QueryResult* result)
{
    m_spendableHonor = 0;
    m_conquestPoints = 0;
    m_weeklySpendableHonor = 0;
    m_currencyWeekBeginDay = sHonorMaintenancer.GetWeekBeginDay();

    if (result)
    {
        Field* fields = result->Fetch();
        m_spendableHonor = fields[0].GetUInt32();
        m_conquestPoints = fields[1].GetUInt32();
        m_weeklySpendableHonor = fields[2].GetUInt32();
        m_currencyWeekBeginDay = fields[3].GetUInt32();
    }

    ResetCurrencyWeekIfNeeded();
}

void HonorMgr::SendHonorCurrencyUpdate() const
{
    if (!m_owner)
        return;

    m_owner->SendAddonMessage("TW_HONOR", "S2C_HONOR_RESPONSE;" + std::to_string(m_spendableHonor));
}

uint32 HonorMgr::AddSpendableHonor(uint32 amount)
{
    if (!amount)
        return 0;

    ResetCurrencyWeekIfNeeded();

    uint32 weeklyHonorCap = sWorld.getConfig(CONFIG_UINT32_WEEKLY_HONOR_CAP);
    if (m_weeklySpendableHonor >= weeklyHonorCap)
        return 0;

    uint32 amountToAdd = std::min(amount, weeklyHonorCap - m_weeklySpendableHonor);
    m_spendableHonor += amountToAdd;
    m_weeklySpendableHonor += amountToAdd;
    SendHonorCurrencyUpdate();
    return amountToAdd;
}

uint32 HonorMgr::ModifySpendableHonor(int32 amount)
{
    ResetCurrencyWeekIfNeeded();

    if (amount > 0)
    {
        uint32 amountToAdd = uint32(std::min<int64>(amount, int64(std::numeric_limits<uint32>::max() - m_spendableHonor)));
        m_spendableHonor += amountToAdd;
        SendHonorCurrencyUpdate();
        return amountToAdd;
    }

    if (amount == 0)
        return 0;

    uint32 amountToRemove = uint32(std::min<int64>(m_spendableHonor, -int64(amount)));
    m_spendableHonor -= amountToRemove;
    SendHonorCurrencyUpdate();
    return amountToRemove;
}

bool HonorMgr::SpendSpendableHonor(uint32 amount)
{
    ResetCurrencyWeekIfNeeded();

    if (amount > m_spendableHonor)
        return false;

    m_spendableHonor -= amount;
    SendHonorCurrencyUpdate();
    return true;
}

bool HonorMgr::AddConquestPoints(uint32 amount, bool grantRankPoints)
{
    if (!amount)
        return false;

    m_conquestPoints += amount;
    if (grantRankPoints && m_owner)
    {
        m_rankPoints += amount * CONQUEST_TO_RANK_POINTS;
        m_rankPoints = std::min(MaximumRpAtLevel(m_owner->GetLevel()), m_rankPoints);
        Update();
    }

    return true;
}

bool HonorMgr::SpendConquestPoints(uint32 amount)
{
    if (amount > m_conquestPoints)
        return false;

    m_conquestPoints -= amount;
    return true;
}

bool HonorMgr::Add(float cp, uint8 type, Unit* source)
{
    // Prevent give fake records to db with 0 honor
    if (cp <= 0 || !m_owner)
        return false;

    if (type != DISHONORABLE)
    {
        cp *= sWorld.getConfig(CONFIG_FLOAT_RATE_HONOR);
        if (cp <= 0.0f)
            return false;
    }

    // If not source, then give yourself
    Unit* realSource = source;
    if (!source)
        source = m_owner;

    // get IP if source is player
    std::string ip;
    if (Player* victim = source->ToPlayer())
        ip = victim->GetSession()->GetRemoteAddress();

    bool plr = source->GetTypeId() == TYPEID_PLAYER;

    float creditedHonor = type == DISHONORABLE ? -cp : 0.0f;

    // DK penalties are subtracted from your RP score immediately and are not included in weekly adjustment.
    if (type == DISHONORABLE)
        m_rankPoints = m_rankPoints - cp;
    else
    {
        m_rankPoints += cp * HONOR_TO_RANK_POINTS;
        m_rankPoints = std::min(MaximumRpAtLevel(m_owner->GetLevel()), m_rankPoints);
        creditedHonor = AddSpendableHonor(uint32(std::max(1.0f, std::ceil(cp))));
    }

    HonorCP honorCP;
    honorCP.date = sWorld.GetGameDay();
    honorCP.cp = creditedHonor >= 0.0f ? creditedHonor : cp;
    honorCP.victimId = (source->GetTypeId() == TYPEID_PLAYER ? source->GetGUIDLow() : source->GetEntry());
    honorCP.victimType = (source == m_owner ? 0 : source->GetTypeId());
    honorCP.type = type;
    honorCP.state = STATE_NEW;

    if (m_owner->GetMap()->IsBattleGround())
        sLog.outHonor("[BATTLEGROUND]: Player %s (account: %u) got %f honor for type %u, source %s %s (IP: %s)",
            m_owner->GetSession()->GetPlayerName(), m_owner->GetSession()->GetAccountId(), creditedHonor, type, plr ? "player" : "unit", source->GetName(), ip.c_str());
    else
        sLog.outHonor("[OPEN WORLD]: Player %s (account: %u) got %f honor for type %u, source %s %s (IP: %s)",
            m_owner->GetSession()->GetPlayerName(), m_owner->GetSession()->GetAccountId(), creditedHonor, type, plr ? "player" : "unit", source->GetName(), ip.c_str());

    m_honorCP.push_back(honorCP);

    if (creditedHonor != 0.0f)
        SendPVPCredit(realSource, creditedHonor);

    Update();
    return true;
}

void HonorMgr::Update() {
    if (!m_owner)
        return;

    if (m_rankPoints > 0.0f)
        m_rankPoints = std::min(MaximumRpAtLevel(m_owner->GetLevel()), m_rankPoints);

    uint32 todayHK = 0;
    uint32 todayDK = 0;
    uint32 yesterdayKills = 0;
    uint32 thisWeekKills = 0;
    float yesterdayCP = 0.0f;
    float thisWeekCP = 0.0f;

    uint32 today = sWorld.GetGameDay();
    uint32 yesterday = today - 1;
    uint32 thisWeekBegin = sHonorMaintenancer.GetWeekBeginDay();

    m_totalDK = m_storedDK;
    m_totalHK = m_storedHK;

    for (auto &itr : m_honorCP) {
        if (itr.type == HONORABLE) {
            if (itr.date == today)
                ++todayHK;

            if (itr.date == yesterday)
                ++yesterdayKills;

            if (itr.date >= thisWeekBegin) {
                ++thisWeekKills;
                ++m_totalHK;
            }
        }

        if (itr.type != DISHONORABLE) {
            if (itr.date == yesterday)
                yesterdayCP += itr.cp;

            if (itr.date >= thisWeekBegin)
                thisWeekCP += itr.cp;
        }

        if (itr.type == DISHONORABLE) {
            if (itr.date == today)
                ++todayDK;

            ++m_totalDK;
        }
    }

    m_rank = CalculateRank(m_rankPoints, m_totalHK);
    if (m_rank.visualRank > 0 && (m_rank.visualRank > m_highestRank.visualRank))
        SetHighestRank(m_rank);

    // HIGHEST RANK
    m_owner->SetByteValue(PLAYER_FIELD_BYTES, 3, m_highestRank.rank);
    // RANK (Patent)
    if (!m_owner->IsIgnoringTitles()) {
        m_owner->SetByteValue(PLAYER_BYTES_3, 3, m_rank.rank);
    
        float rankPoints = m_rankPoints >= 0.0f ? m_rankPoints : -1 * m_rankPoints;
        float rankProgress = (rankPoints - m_rank.minRP) / (m_rank.maxRP - m_rank.minRP);
        rankProgress = std::max(0.0f, std::min(1.0f, rankProgress));
        uint32 honorBar = uint8(rankProgress * (m_rank.positive ? 255 : -255));
    
        // PLAYER_FIELD_HONOR_BAR
        m_owner->SetByteValue(PLAYER_FIELD_BYTES2, 0, honorBar);
    }

    // TODAY
    m_owner->SetUInt16Value(PLAYER_FIELD_SESSION_KILLS, 0, todayHK);
    m_owner->SetUInt16Value(PLAYER_FIELD_SESSION_KILLS, 1, todayDK);

    // YESTERDAY
    m_owner->SetUInt32Value(PLAYER_FIELD_YESTERDAY_KILLS, yesterdayKills);
    m_owner->SetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION, uint32(yesterdayCP > 0.0f ? yesterdayCP : 0.0f));

    // THIS WEEK
    m_owner->SetUInt32Value(PLAYER_FIELD_THIS_WEEK_KILLS, thisWeekKills);
    m_owner->SetUInt32Value(PLAYER_FIELD_THIS_WEEK_CONTRIBUTION, uint32(thisWeekCP > 0.0f ? thisWeekCP : 0.0f));

    // LAST WEEK
    m_owner->SetUInt32Value(PLAYER_FIELD_LAST_WEEK_KILLS, m_lastWeekHK);
    m_owner->SetUInt32Value(PLAYER_FIELD_LAST_WEEK_CONTRIBUTION, uint32(m_lastWeekCP > 0.0f ? m_lastWeekCP : 0.0f));
    m_owner->SetUInt32Value(PLAYER_FIELD_LAST_WEEK_RANK, m_standing);

    // LIFE TIME
    m_owner->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, m_totalHK);
    m_owner->SetUInt32Value(PLAYER_FIELD_LIFETIME_DISHONORABLE_KILLS, m_totalDK);
}

void HonorMgr::InitRankInfo(HonorRankInfo &prk)
{
    prk.positive = true;
    prk.rank = 0;
    prk.visualRank = 0;
    prk.maxRP = 2000.00f;
    prk.minRP = 0.00f;
}

uint32 HonorMgr::CalculateTotalKills(Unit* victim) const
{
    if (!victim)
        return 0;

    uint32 totalKills = 0;
    uint32 id = 0;
    uint32 today = sWorld.GetGameDay();
    uint8 victimType = victim->GetTypeId();

    switch (victimType)
    {
        case TYPEID_PLAYER:
            id = ((Player*)victim)->GetGUIDLow();
            break;
        case TYPEID_UNIT:
            id = victim->GetEntry();
            break;
        default:
            return 0;
    }

    for (auto& honorCP : m_honorCP)
        if (honorCP.victimType == victimType && honorCP.victimId == id && honorCP.date == today)
            totalKills++;

    return totalKills;
}

void HonorMgr::CalculateRankInfo(HonorRankInfo& prk)
{
    if (prk.rank == 0)
    {
        InitRankInfo(prk);
        return;
    }

    prk.visualRank = prk.rank > NEGATIVE_HONOR_RANK_COUNT ? prk.rank - NEGATIVE_HONOR_RANK_COUNT : prk.rank * -1;

    if (prk.positive && prk.visualRank > 0)
    {
        uint8 visualRank = uint8(prk.visualRank);
        if (visualRank >= POSITIVE_HONOR_RANK_COUNT)
            visualRank = POSITIVE_HONOR_RANK_COUNT - 1;

        prk.minRP = RankPointBounds[visualRank][0];
        prk.maxRP = RankPointBounds[visualRank][1];
        return;
    }

    int8 rank = prk.rank - NEGATIVE_HONOR_RANK_COUNT;
    prk.maxRP = (rank) * 5000.00f;
    if (prk.maxRP < 0) // in negative rank case
        prk.maxRP *= -1;
    prk.minRP = prk.maxRP > 5000.0f ? prk.maxRP  - 5000.00f : 2000.00f;
}

HonorRankInfo HonorMgr::CalculateRank(float rankPoints, uint32 totalHK)
{
    HonorRankInfo prk;
    InitRankInfo(prk);

    // rank none
    if (rankPoints == 0)
        return prk;

    prk.positive = rankPoints > 0;
    if (!prk.positive)
    {
        rankPoints *= -1;

        uint8 rankCount = NEGATIVE_HONOR_RANK_COUNT;
        uint8 firstRank = 1;

        if (rankPoints < 2000.00f)
            prk.rank = NEGATIVE_HONOR_RANK_COUNT;
        else if (rankPoints > (rankCount - 1) * 5000.00f)
            prk.rank = firstRank;
        else
        {
            prk.rank = uint32(rankPoints / 5000.00f) + firstRank;
            prk.rank = NEGATIVE_HONOR_RANK_COUNT - prk.rank;
        }

        CalculateRankInfo(prk);
        return prk;
    }

    for (uint8 visualRank = 1; visualRank < POSITIVE_HONOR_RANK_COUNT; ++visualRank)
    {
        if (rankPoints < RankPointBounds[visualRank][1])
        {
            prk.rank = NEGATIVE_HONOR_RANK_COUNT + visualRank;
            CalculateRankInfo(prk);
            return prk;
        }
    }

    prk.rank = HONOR_RANK_COUNT - 1;
    CalculateRankInfo(prk);

    return prk;
}

float HonorMgr::DishonorableKillPoints(uint8 level)
{
    float result = 10.0f;
    if (level >= 30 && level <= 35)
        result = result + 1.5f * (level - 29);
    if (level >= 36 && level <= 41)
        result = result + 9 + 2 * (level - 35);
    if (level >= 42 && level <= 50)
        result = result + 21 + 3.2f * (level - 41);
    if (level >= 51)
        result = result + 50 + 4 * (level - 50);
    if (result > 100)
        return 100.0f;
    else
        return result;
}

float HonorMgr::HonorableKillPoints(Player* killer, Player* victim, uint32 groupSize)
{
    if (!killer || !victim || !groupSize)
        return 0.0;

    uint32 totalKills = killer->GetHonorMgr().CalculateTotalKills(victim);
    uint32 victimRank = victim->GetHonorMgr().GetRank().visualRank;
    uint8 killerLevel = killer->GetLevel();
    uint8 victimLevel = victim->GetLevel();

    return MaNGOS::Honor::GetHonorGain(killerLevel, victimLevel, victimRank, totalKills, groupSize, killer->InGurubashiArena(false));
}

void HonorMgr::SendPVPCredit(Unit* victim, float honor)
{
    if (!m_owner)
        return;

    WorldPacket data(SMSG_PVP_CREDIT, 4 + 8 + 4);
    data << int32(honor);

    if (!victim)
    {
        data << int64(0);
        data << int32(0);
    }
    else
    {
        data << victim->GetObjectGuid();

        constexpr int minimumRank = (HONOR_RANK_COUNT - POSITIVE_HONOR_RANK_COUNT) + 1;

        if (victim->IsCreature())
        {
            if (((Creature*)victim)->IsRacialLeader())
                data << int32(19);
            else
                data << int32(minimumRank);
        }
        else if (victim->IsPlayer())
        {
            // Never display just "HK:" without rank name.
            // When killing a player with no rank,
            // we need to send first rank instead.
            // https://youtu.be/hef06Cs6Q34?t=191
            // New classic client does this on its own.
            int32 rank = ((Player*)victim)->GetHonorMgr().GetRank().rank;
            if (!rank)
                rank = minimumRank;

            data << int32(rank);
        }
    }

    m_owner->_SendRewardPacket(std::move(data));
}
