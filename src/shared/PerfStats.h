#ifndef PERFSTATS_H
#define PERFSTATS_H
#include <atomic>

namespace PerfStats
{
    extern std::atomic<int> g_totalUnits;
    extern std::atomic<int> g_totalCreatures;
    extern std::atomic<int> g_totalPets;
    extern std::atomic<int> g_totalPlayers;
    extern std::atomic<int> g_totalCorpses;
    extern std::atomic<int> g_totalItems;
    extern std::atomic<int> g_totalGameObjects;
    extern std::atomic<int> g_totalDynamicObjects;
    extern std::atomic<int> g_totalQueryResults;
    extern std::atomic<int> g_totalMaps;

    extern int g_slowestMapId;
    extern int g_slowestMapUpdateTime;
};

#endif