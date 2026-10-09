#ifndef MANTECH_LAZY_STORAGE_H
#define MANTECH_LAZY_STORAGE_H
#include "Memory/MemoryLedger.h"
#include <array>
#include <atomic>
#include <memory>
#include <cassert>
namespace ManTech {
// Publish once, retain until owner destruction. Borrowed references and end
// iterators keep the underlying STL container's original lifetime guarantees.
template<class T> class LazyStorage {
    mutable std::atomic<T*> value{nullptr};
public:
    LazyStorage() = default;
    LazyStorage(LazyStorage const&) = delete;
    LazyStorage& operator=(LazyStorage const&) = delete;
    ~LazyStorage() { if (auto p=value.load(std::memory_order_relaxed)) {
        delete p; MemoryLedger::Remove(MemoryKind::OptionalState,sizeof(T));
    }}
    T& Get() const {
        auto p=value.load(std::memory_order_acquire);
        if (!p) {
            auto candidate=std::make_unique<T>();
            if (value.compare_exchange_strong(p,candidate.get(),std::memory_order_acq_rel)) {
                p=candidate.release(); MemoryLedger::Add(MemoryKind::OptionalState,sizeof(T));
            }
        }
        return *p;
    }
    // Non-escaping reads only. Use Get() for references exposed to callers.
    T const& Read() const {
        static T const empty;
        auto p=value.load(std::memory_order_acquire);
        return p ? *p : empty;
    }
};
template<class T, std::size_t N> class LazyStorageArray {
    std::array<LazyStorage<T>,N> values;
public:
    T& operator[](std::size_t n) { assert(n<N); return values[n].Get(); }
    T const& operator[](std::size_t n) const { assert(n<N); return values[n].Get(); }
    T const& Read(std::size_t n) const { assert(n<N); return values[n].Read(); }
};
}
#endif
