/*
 * Copyright (C) 2016 Elysium Project <https://elysium-project.org>
 */

#ifndef HONORMGR_H
#define HONORMGR_H

#include <unordered_map>
#include <list>
#include <vector>

class Player;
class Unit;
class QueryResult;

struct WeeklyScore
{
    WeeklyScore() : level(0), account(0), hk(0), dk(0), cp(0.0f), oldRp(0.0f), newRp(0.0f), earning(0.0f), standing(0), highestRank(0) {}

    uint8  level;
    uint32 account;
    uint32 hk;
    uint32 dk;
    float  cp;
    float  oldRp;
    float  newRp;
    float  earning;
    uint32 standing;
    uint8  highestRank;
};

typedef std::unordered_map<uint32, WeeklyScore> WeeklyScoresHash;

class HonorMaintenancer
{
    public:
        HonorMaintenancer() : m_lastMaintenanceDay(0), m_nextMaintenanceDay(0), m_markerToStart(false) {}
        ~HonorMaintenancer() {}

        void Initialize();
        void DoMaintenance();

        void LoadWeeklyScores();
        void DecayRankPoints();
        void SetCityRanks();
        void FlushRankPoints();
        void CreateCalculationReport();

        void FlushWeeklyQuests();

        float CalculateRpDecay(float rpEarning, const WeeklyScore& wk);

        void CheckMaintenanceDay();
        uint32 GetLastMaintenanceDay() const { return m_lastMaintenanceDay; }
        uint32 GetNextMaintenanceDay() const { return m_nextMaintenanceDay; }
        uint32 GetWeekBeginDay() const { return m_lastMaintenanceDay; }
        uint32 GetWeekEndDay() const { return m_lastMaintenanceDay + 6; }

        void ToggleMaintenanceMarker();
        void SetMaintenanceDays(uint32 last, uint32 next = 0);

    private:
        WeeklyScoresHash m_weeklyScores;

        uint32 m_lastMaintenanceDay;
        uint32 m_nextMaintenanceDay;
        bool m_markerToStart;
};

enum HonorType
{
    HONORABLE    = 1,
    DISHONORABLE = 2,
    BONUS        = 3,
    QUEST        = 4,
    OTHER        = 5
};

enum HonorState
{
   STATE_NEW       = 0,
   STATE_UNCHANGED = 1
};

struct HonorCP
{
   uint8  victimType;
   uint32 victimId;
   float  cp;
   uint32 date;
   uint8  type;
   uint8  state;
};

struct HonorRankInfo
{
    HonorRankInfo() : rank(0), visualRank(0), maxRP(0.0f), minRP(0.0f), positive(true) {}

    uint8 rank;        // internal range [0..18]
    int8  visualRank;  // number visualized in rank bar [-4..14]
    float maxRP;
    float minRP;
    bool  positive;
};

typedef std::list<HonorCP> HonorCPMap;

#define MIN_HONOR_KILLS 15
#define NEGATIVE_HONOR_RANK_COUNT 4
#define POSITIVE_HONOR_RANK_COUNT 15
#define HONOR_RANK_COUNT 19
#define QUEST_DAILY_MOST_HK 39981
#define QUEST_DAILY_MOST_DK 39980

class HonorMgr
{
    public:
        explicit HonorMgr(Player* owner) : m_spendableHonor(0), m_conquestPoints(0), m_weeklySpendableHonor(0), m_currencyWeekBeginDay(0), m_owner(owner) {}
        ~HonorMgr() {}

        void Save();
        class SaveBatch
        {
            friend class HonorMgr;
            HonorMgr* owner = nullptr;
            uint32 guid = 0;
            std::vector<HonorCP> entries;
            float rankPoints = 0, lastWeekCP = 0;
            uint32 standing = 0, lastWeekHK = 0, storedHK = 0, storedDK = 0;
            uint8 highestRank = 0;
            uint32 honor = 0, conquest = 0, weeklyHonor = 0, weekBegin = 0;
            bool prepared = false;
        public:
            SaveBatch() = default;
            SaveBatch(SaveBatch const&) = delete;
            SaveBatch& operator=(SaveBatch const&) = delete;
        };
        bool AppendSaveToDB(SaveBatch& batch);
        bool CompleteSaveToDB(SaveBatch& batch);
        void SaveStoredData();
        void Load(QueryResult* result);
        void LoadCurrency(QueryResult* result);
        void SaveCurrency();
        void SendHonorCurrencyUpdate() const;

        bool Add(float CP, uint8 type, Unit* source = nullptr);
        void Update();
        void Reset();
        void ClearHonorData();
        void ClearHonorCP();

        static void InitRankInfo(HonorRankInfo &prk);
        static void CalculateRankInfo(HonorRankInfo& prk);
        static HonorRankInfo CalculateRank(float rankPoints, uint32 totalHK = 0);
        uint32 CalculateTotalKills(Unit* victim) const;

        static void LoadMostDkHkYesterdayPlayers();
        static uint32 GetMostHkYesterdayPlayerGuid()
        {
            return m_mostHkYesterdayGuid;
        }
        static uint32 GetMostDkYesterdayPlayerGuid()
        {
            return m_mostDkYesterdayGuid;
        }
        
        static float DishonorableKillPoints(uint8 level);
        static float HonorableKillPoints(Player* killer, Player* victim, uint32 groupsize);
        static float MaximumRpAtLevel(uint8 level);

        HonorRankInfo GetRank() const { return m_rank; }
        uint8 GetCurrentHonorRank() const { return m_rank.rank; }
        void SetRank(HonorRankInfo rank) { m_rank = rank; }
        HonorRankInfo GetHighestRank() const { return m_highestRank; }
        void SetHighestRank(HonorRankInfo hignestRank) { m_highestRank = hignestRank; }
        void SetHighestRank(uint8 hignestRank)
        {
            m_highestRank.rank = hignestRank;
            CalculateRankInfo(m_highestRank);
        }
        uint32 GetStanding() const { return m_standing; }
        void SetStanding(uint32 standing) { m_standing = standing; }
        float GetRankPoints() const { return m_rankPoints; }
        void SetRankPoints(float rankPoints) { m_rankPoints = rankPoints; }
        uint32 GetStoredDK() const { return m_storedDK; }
        void SetStoredDK(uint32 storedDK) { m_storedDK = storedDK; }
        uint32 GetStoredHK() const { return m_storedHK; }
        void SetStoredHK(uint32 storedHK) { m_storedHK = storedHK; }
        uint32 GetTotalDK() const { return m_totalDK; }
        void SetTotalDK(uint32 totalDK) { m_totalDK = totalDK; }
        uint32 GetTotalHK() const { return m_totalHK; }
        void SetTotalHK(uint32 totalHK) { m_totalHK = totalHK; }
        float GetLastWeekCP() const { return m_lastWeekCP; }
        void SetLastWeekCP(float lastWeekCP) { m_lastWeekCP = lastWeekCP; }
        uint32 GetLastWeekHK() const { return m_lastWeekHK; }
        void SetLastWeekHK(uint32 lastWeekHK) { m_lastWeekHK = lastWeekHK; }
        
        HonorCPMap& GetHonorCP() { return m_honorCP; }

        void SendPVPCredit(Unit* victim, float honor);
        uint32 GetSpendableHonor() const { return m_spendableHonor; }
        uint32 GetConquestPoints() const { return m_conquestPoints; }
        uint32 GetWeeklySpendableHonor() const { return m_weeklySpendableHonor; }
        uint32 AddSpendableHonor(uint32 amount);
        uint32 ModifySpendableHonor(int32 amount);
        bool SpendSpendableHonor(uint32 amount);
        bool AddConquestPoints(uint32 amount, bool grantRankPoints = true);
        bool SpendConquestPoints(uint32 amount);

    private:
        void ResetCurrencyWeekIfNeeded();
        bool AppendHonorCP(HonorCP const& entry);
        bool SaveCurrencyChecked();
        bool SaveStoredDataChecked();

        HonorCPMap m_honorCP;
        float m_lastWeekCP;
        float m_rankPoints;
        uint32 m_lastWeekHK;
        uint32 m_storedHK;
        uint32 m_storedDK;
        uint32 m_totalHK;
        uint32 m_totalDK;
        HonorRankInfo m_rank;
        HonorRankInfo m_highestRank;
        uint32 m_standing;
        uint32 m_spendableHonor;
        uint32 m_conquestPoints;
        uint32 m_weeklySpendableHonor;
        uint32 m_currencyWeekBeginDay;
        static uint32 m_mostHkYesterdayGuid;
        static uint32 m_mostDkYesterdayGuid;

        Player* m_owner;
};

extern HonorMaintenancer sHonorMaintenancer;

#endif
