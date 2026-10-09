#include "PerfStats.h"
namespace PerfStats
{
    std::atomic<int> g_totalUnits{0};
    std::atomic<int> g_totalCreatures{0};
    std::atomic<int> g_totalPets{0};
    std::atomic<int> g_totalPlayers{0};
    std::atomic<int> g_totalCorpses{0};
    std::atomic<int> g_totalItems{0};
    std::atomic<int> g_totalGameObjects{0};
    std::atomic<int> g_totalDynamicObjects{0};
    std::atomic<int> g_totalQueryResults{0};
    std::atomic<int> g_totalMaps{0};

    int g_slowestMapId;
    int g_slowestMapUpdateTime;
};
