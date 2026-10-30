#define CORE_AUTO_REWARD_TEST
#include <functional>
#include <memory>
#include <initializer_list>
#include "inventory_save_environment.h"

using Quest = TestObjectMgr::Quest;
using ObjectGuid = Guid;
constexpr int QUEST_FLAGS_AUTO_REWARDED = 1, QUEST_STATE_COMPLETE = 1;
constexpr int MAX_QUEST_LOG_SIZE = 25, PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST = 1;
size_t completionHooks = 0;
struct PlayerScript { void OnPlayerCompleteQuest(Player*, Quest const*) { ++completionHooks; } } playerScript;
template<class T> struct ScriptRegistry
{
    template<class F> static void ForEachEnabledHook(int, F&& f) { f(&playerScript); }
};

struct TestAccessor
{
    std::map<uint32,Player*> players;
    Player* FindPlayer(Guid guid) const
    { auto found=players.find(guid.value); return found==players.end() ? nullptr : found->second; }
} sObjectAccessor;

// Atomic SQL/group behavior is a boundary here and has separate production tests.
// This test executes the real CompleteQuest/retry and real EventProcessor code.
class AtomicPlayerSaveScope
{
    bool valid, owner;
public:
    static bool busy, active;
    static size_t commits;
    static std::vector<std::function<void()>> actions;
    explicit AtomicPlayerSaveScope(std::initializer_list<Player*>) : valid(!busy), owner(!active)
    { if(valid) active=true; }
    ~AtomicPlayerSaveScope() { if(valid && owner) active=false; }
    explicit operator bool() const { return valid; }
    bool IsOwner() const { return owner; }
    void Fail() { valid=false; }
    bool Commit()
    {
        assert(valid && owner); active=false; ++commits;
        auto batch=std::move(actions); actions.clear();
        for(auto& action:batch) action();
        return true;
    }
    static void DeferAfterCommit(std::function<void()> action) { actions.push_back(std::move(action)); }
};
bool AtomicPlayerSaveScope::busy=false, AtomicPlayerSaveScope::active=false;
size_t AtomicPlayerSaveScope::commits=0;
std::vector<std::function<void()>> AtomicPlayerSaveScope::actions;

#include "../../src/game/Objects/PlayerQuestCompletion.inc"

void ResetAuto()
{
    CharacterDatabase=TestDatabase{}; sObjectMgr=TestObjectMgr{}; sWorld.stopped=false;
    sObjectAccessor.players.clear(); completionHooks=0;
    AtomicPlayerSaveScope::busy=true; AtomicPlayerSaveScope::active=false;
    AtomicPlayerSaveScope::commits=0; AtomicPlayerSaveScope::actions.clear();
    sObjectMgr.quests.emplace(100, Quest{});
}

void BusyRetriesAreDeduplicatedAndRecover()
{
    ResetAuto(); Player player(1); sObjectAccessor.players.emplace(1,&player);
    player.SetQuestStatus(100, QUEST_STATUS_INCOMPLETE);
    player.CompleteQuest(100); player.CompleteQuest(100);
    assert(player.GetQuestStatus(100)==QUEST_STATUS_INCOMPLETE);
    assert(player.m_Events.GetEvents().size()==1 && player.m_pendingAutoQuestRewards.size()==1);
    assert(player.rewards==0 && completionHooks==0 && AtomicPlayerSaveScope::commits==0);
    player.m_Events.Update(1);
    assert(player.m_Events.GetEvents().size()==1 && player.rewards==0);
    AtomicPlayerSaveScope::busy=false;
    player.m_Events.Update(1);
    assert(player.GetQuestStatus(100)==QUEST_STATUS_COMPLETE && player.rewards==1);
    assert(completionHooks==1 && AtomicPlayerSaveScope::commits==1);
    assert(player.m_Events.GetEvents().empty() && player.m_pendingAutoQuestRewards.empty());
}

void FaultAbandonmentAndActorDestructionCancelRetry()
{
    for(int reason=0;reason!=3;++reason)
    {
        ResetAuto(); Player player(1); player.SetQuestStatus(100,QUEST_STATUS_INCOMPLETE);
        player.CompleteQuest(100);
        if(reason==0) CharacterDatabase.fault=true;
        if(reason==1) player.SetQuestStatus(100,QUEST_STATUS_NONE);
        if(reason==2) sWorld.stopped=true;
        AtomicPlayerSaveScope::busy=false; player.m_Events.Update(1);
        assert(player.rewards==0 && player.m_Events.GetEvents().empty());
        assert(player.m_pendingAutoQuestRewards.empty() && completionHooks==0);
    }
    ResetAuto();
    {
        auto player=std::make_unique<Player>(1);
        player->SetQuestStatus(100,QUEST_STATUS_INCOMPLETE);
        player->CompleteQuest(100);
        assert(player->m_Events.GetEvents().size()==1);
    }
    // Actual EventProcessor destructor aborts/deletes lambdas without execution.
    assert(AtomicPlayerSaveScope::commits==0 && completionHooks==0);
}

void NonAutomaticCompletionKeepsOrdinaryBehavior()
{
    ResetAuto(); sObjectMgr.quests.at(100).automatic=false;
    Player player(1); player.SetQuestStatus(100,QUEST_STATUS_INCOMPLETE);
    player.CompleteQuest(100);
    assert(player.GetQuestStatus(100)==QUEST_STATUS_COMPLETE && completionHooks==1);
    assert(player.rewards==0 && player.m_Events.GetEvents().empty());
}

int main()
{
    BusyRetriesAreDeduplicatedAndRecover();
    FaultAbandonmentAndActorDestructionCancelRetry();
    NonAutomaticCompletionKeepsOrdinaryBehavior();
    std::cout << "Production automatic quest retry/event tests passed.\n";
}
