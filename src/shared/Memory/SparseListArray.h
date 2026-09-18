#ifndef MANTECH_SPARSE_LIST_ARRAY_H
#define MANTECH_SPARSE_LIST_ARRAY_H
#include "Memory/MemoryLedger.h"
#include <array>
#include <atomic>
#include <list>
#include <memory>
#include <cassert>
namespace ManTech {
// Two-level pointer index: unused groups of aura types occupy only one pointer.
// Pages and exposed lists live until Unit destruction. List nodes, iterators,
// stable empty references and the owning Unit's mutation rules are unchanged.
template<class T, std::size_t N> class SparseListArray {
    using List=std::list<T>;
    static constexpr std::size_t PageSize=8;
    struct Page {
        std::array<std::atomic<List*>,PageSize> buckets{};
        ~Page() {
            for(auto const& bucket:buckets)
                if(auto list=bucket.load(std::memory_order_relaxed)) {
                    delete list;
                    MemoryLedger::Remove(MemoryKind::AuraBuckets,sizeof(List));
                }
        }
    };
    mutable std::array<std::atomic<Page*>,(N+PageSize-1)/PageSize> pages{};
    Page* MaterializePage(std::size_t index) const {
        auto& slot=pages[index/PageSize];
        auto page=slot.load(std::memory_order_acquire);
        if(!page) {
            auto candidate=std::make_unique<Page>();
            if(slot.compare_exchange_strong(page,candidate.get(),std::memory_order_acq_rel)) {
                page=candidate.release();
                MemoryLedger::Add(MemoryKind::AuraIndexes,sizeof(Page));
            }
        }
        return page;
    }
public:
    SparseListArray()=default;
    SparseListArray(SparseListArray const&)=delete;
    SparseListArray& operator=(SparseListArray const&)=delete;
    ~SparseListArray() {
        for(auto const& slot:pages)
            if(auto page=slot.load(std::memory_order_relaxed)) {
                delete page;
                MemoryLedger::Remove(MemoryKind::AuraIndexes,sizeof(Page));
            }
    }
    // Non-escaping reads never materialize a page or an empty list.
    List const& operator[](std::size_t index) const {
        assert(index<N);
        static List const empty;
        auto page=pages[index/PageSize].load(std::memory_order_acquire);
        auto list=page ? page->buckets[index%PageSize].load(std::memory_order_acquire) : nullptr;
        return list ? *list : empty;
    }
    List const& Stable(std::size_t index) const {
        assert(index<N);
        auto& bucket=MaterializePage(index)->buckets[index%PageSize];
        auto list=bucket.load(std::memory_order_acquire);
        if(!list) {
            auto candidate=std::make_unique<List>();
            if(bucket.compare_exchange_strong(list,candidate.get(),std::memory_order_acq_rel)) {
                list=candidate.release();
                MemoryLedger::Add(MemoryKind::AuraBuckets,sizeof(List));
            }
        }
        return *list;
    }
    List& Mutable(std::size_t index) { return const_cast<List&>(Stable(index)); }
};
}
#endif
