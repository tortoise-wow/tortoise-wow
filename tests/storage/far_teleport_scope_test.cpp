// The runner extracts the unmodified ScheduleFarTeleport body from MapManager.cpp.
// AtomicPlayerSaveScope is the real implementation; only map/player/SQL edges
// are fixtures. No copied admission algorithm or actual teleport/service is used.
#include "atomic-scope-stubs/ScopeBoundary.h"
#pragma include_alias("Player.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("Mail/Mail.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("Database/DatabaseEnv.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("World.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("Log.h", "atomic-scope-stubs/ScopeBoundary.h")
#include "../../src/game/AtomicPlayerSave.cpp"
#include <mutex>

struct ScheduledTeleportData
{
    static unsigned destroyed;
    ~ScheduledTeleportData() { ++destroyed; }
};
unsigned ScheduledTeleportData::destroyed = 0;
struct TeleportPlayer
{
    unsigned transfers = 0;
    bool pending = false;
    void ExecuteTeleportFar(ScheduledTeleportData*) { ++transfers; }
    void SetPendingFarTeleport(bool value) { pending = value; }
};
class MapManager
{
public:
    bool asyncMapUpdating = false;
    std::mutex m_scheduledFarTeleportsLock;
    std::map<TeleportPlayer*, ScheduledTeleportData*> m_scheduledFarTeleports;
    void ScheduleFarTeleport(TeleportPlayer*, ScheduledTeleportData*);
};
#define Player TeleportPlayer
#include "tested-far-teleport.inc"
#undef Player

int main()
{
    using namespace ScopeTest;
    for (bool updating : {false, true})
        for (bool ownedScope : {false, true})
        {
            Reset();
            ScheduledTeleportData::destroyed = 0;
            Player participant(1);
            TeleportPlayer destination;
            MapManager maps;
            maps.asyncMapUpdating = updating;
            std::unique_ptr<AtomicPlayerSaveScope> scope;
            if (ownedScope)
            {
                scope = std::make_unique<AtomicPlayerSaveScope>(
                    std::initializer_list<Player*>{&participant});
                Check(bool(*scope), "actual actor scope admitted");
            }
            maps.ScheduleFarTeleport(&destination, new ScheduledTeleportData);
            if (updating || ownedScope)
            {
                Check(destination.transfers == 0 && destination.pending,
                    "map update or owned reward keeps transfer pending");
                Check(maps.m_scheduledFarTeleports.count(&destination) == 1 &&
                    ScheduledTeleportData::destroyed == 0,
                    "existing deferred queue retains transfer data ownership");
            }
            else
                Check(destination.transfers == 1 && !destination.pending &&
                    maps.m_scheduledFarTeleports.empty() &&
                    ScheduledTeleportData::destroyed == 1,
                    "ordinary non-map transfer remains immediate");
            if (scope)
            {
                beforeAck = [&]
                {
                    Check(destination.transfers == 0 && destination.pending,
                        "no detach or pet unsummon before actual scope ACK");
                };
                Check(scope->Commit(), "scope ACK succeeds before queue drain");
                Check(!AtomicPlayerSaveScope::Active(), "committed scope is detached");
            }
            for (const auto& queued : maps.m_scheduledFarTeleports)
            {
                queued.first->ExecuteTeleportFar(queued.second);
                delete queued.second;
            }
            maps.m_scheduledFarTeleports.clear();
            Check(destination.transfers == 1 && ScheduledTeleportData::destroyed == 1,
                "exactly one transfer and data destruction after safe queue drain");
        }
    std::cout << "PASS actual far-teleport scheduler with map and actor-scope admission\n";
    return 0;
}
