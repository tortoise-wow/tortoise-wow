#ifndef MANGOS_MOVEMENT_VIEWER_SET_H
#define MANGOS_MOVEMENT_VIEWER_SET_H

#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <vector>

// Inverse visibility index. Store identifiers, never player/session pointers.
// Snapshot before network work so no visibility or socket lock nests inside it.
template<class Guid>
class MovementViewerSet
{
public:
    void Add(Guid guid)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        auto it = std::lower_bound(m_guids.begin(), m_guids.end(), guid);
        if (it == m_guids.end() || *it != guid)
            m_guids.insert(it, guid);
    }
    void Remove(Guid guid)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        auto it = std::lower_bound(m_guids.begin(), m_guids.end(), guid);
        if (it != m_guids.end() && *it == guid)
            m_guids.erase(it);
    }
    std::vector<Guid> Snapshot() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_guids;
    }
    void Clear()
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_guids.clear();
    }
private:
    mutable std::shared_mutex m_mutex;
    std::vector<Guid> m_guids;
};

#endif
