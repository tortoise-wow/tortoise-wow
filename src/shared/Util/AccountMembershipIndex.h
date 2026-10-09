#ifndef MANTECH_ACCOUNT_MEMBERSHIP_INDEX_H
#define MANTECH_ACCOUNT_MEMBERSHIP_INDEX_H

#include <cstdint>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <unordered_set>

namespace ManTech {
// The existing ordered list still drives account/character creation. Only
// membership queries use this index. Registration preserves duplicate entries
// and iteration order, and publishes membership after the list append succeeds.
class AccountMembershipIndex {
    mutable std::shared_mutex mutex;
    std::unordered_set<std::uint32_t> ids;
public:
    void Append(std::list<std::uint32_t>& ordered, std::uint32_t id) {
        std::unique_lock<std::shared_mutex> lock(mutex);
        auto inserted = ids.insert(id);
        try { ordered.push_back(id); }
        catch (...) {
            if (inserted.second) ids.erase(inserted.first);
            throw;
        }
    }
    bool Contains(std::uint32_t id) const {
        std::shared_lock<std::shared_mutex> lock(mutex);
        return ids.find(id) != ids.end();
    }
};

// PlayerbotAIConfig is a process singleton. All four registration sites in the
// pinned module use this same index, including runtime account creation.
inline AccountMembershipIndex& PlayerbotAccountMembership() {
    static AccountMembershipIndex index;
    return index;
}
}
#endif
