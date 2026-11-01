#pragma once

#include <functional>
#include <initializer_list>
#include <memory>

class Player;

// A synchronous, actor-local owner above the SQL transaction. Nested core rewards
// join its participants; only the outermost scope serializes and acknowledges it.
class AtomicPlayerSaveScope
{
    struct State;
    std::unique_ptr<State> m_owner;
    State* m_state = nullptr;
    static thread_local State* s_current;

public:
    explicit AtomicPlayerSaveScope(std::initializer_list<Player*> participants);
    ~AtomicPlayerSaveScope();
    AtomicPlayerSaveScope(const AtomicPlayerSaveScope&) = delete;
    AtomicPlayerSaveScope& operator=(const AtomicPlayerSaveScope&) = delete;

    explicit operator bool() const;
    bool IsOwner() const { return bool(m_owner); }
    bool RequestFullSave(Player& player);
    bool Commit();
    void Fail();

    static bool Active();
    // Outside an atomic scope, preserve the ordinary immediate behavior.
    static void DeferAfterCommit(std::function<void()> action);
};
