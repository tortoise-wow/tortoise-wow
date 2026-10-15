#ifndef MANTECH_WRITE_BUDGET_H
#define MANTECH_WRITE_BUDGET_H
#include "Memory/MemoryLedger.h"
#include <atomic>
#include <cstddef>
#include <memory>
namespace ManTech {
// Tokens cover payloads before posting, while queued, and while in flight.
class WriteBudget {
    struct State { std::atomic<std::size_t> bytes{0}; };
    std::shared_ptr<State> state=std::make_shared<State>();
    inline static std::atomic<std::size_t> globalBytes{0};
    static bool Reserve(std::atomic<std::size_t>& counter, std::size_t n, std::size_t limit) {
        auto old=counter.load(std::memory_order_relaxed);
        do { if(n>limit || old>limit-n) return false; }
        while(!counter.compare_exchange_weak(old,old+n,std::memory_order_relaxed));
        return true;
    }
public:
    class Token {
        friend class WriteBudget;
        std::shared_ptr<State> state;
        std::size_t bytes;
    public:
        Token(std::shared_ptr<State> s, std::size_t n):state(std::move(s)),bytes(n) { MemoryLedger::Add(MemoryKind::Network,n); }
    public:
        Token(Token const&)=delete;
        ~Token() { MemoryLedger::Remove(MemoryKind::Network,bytes); globalBytes.fetch_sub(bytes,std::memory_order_relaxed); state->bytes.fetch_sub(bytes,std::memory_order_relaxed); }
    };
    std::shared_ptr<Token> Acquire(std::size_t n, std::size_t socketLimit, std::size_t globalLimit) {
        if(!Reserve(state->bytes,n,socketLimit)) return {};
        if(!Reserve(globalBytes,n,globalLimit)) { state->bytes.fetch_sub(n,std::memory_order_relaxed); return {}; }
        try { return std::make_shared<Token>(state,n); }
        catch(...) { state->bytes.fetch_sub(n,std::memory_order_relaxed); globalBytes.fetch_sub(n,std::memory_order_relaxed); throw; }
    }
};
}
#endif
