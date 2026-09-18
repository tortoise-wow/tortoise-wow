#pragma once
#include <algorithm>
#include <cmath>
#include <iterator>
#include <random>
#include <utility>
#include <vector>

namespace BotScheduling
{
// Weighted sampling without replacement. Exponential clocks produce the same
// sequential weight-proportional distribution as repeated discrete draws, but
// sorting N independent clocks costs O(N log N), not N distribution rebuilds.
// Zero weights follow positive weights in uniformly random order. Item/weight
// pairs stay together, as in the original swapping implementation.
template<class D, class W, class Generator>
void WeightedPermutation(D first, D last, W weight, W weightEnd, Generator& random)
{
    using Value = typename std::iterator_traits<D>::value_type;
    using Weight = typename std::iterator_traits<W>::value_type;
    struct Entry { Value value; Weight weight; double clock; double tie; };
    std::vector<Entry> entries;
    auto outWeight = weight;
    for (auto item = first; item != last && weight != weightEnd; ++item, ++weight)
    {
        double const w = static_cast<double>(*weight);
        // 1-U lies in (0,1], avoiding log(0). Invalid weights are treated as
        // zero rather than permitting NaN to violate sort's strict ordering.
        double const u = 1.0 - std::generate_canonical<double, 53>(random);
        entries.push_back({*item, *weight,
            w > 0 && std::isfinite(w) ? -std::log(u) / w : INFINITY,
            std::generate_canonical<double, 53>(random)});
    }
    std::sort(entries.begin(), entries.end(), [](Entry const& a, Entry const& b)
    {
        return a.clock < b.clock || (a.clock == b.clock && a.tie < b.tie);
    });
    for (auto& entry : entries)
    {
        *first++ = std::move(entry.value);
        *outWeight++ = std::move(entry.weight);
    }
}
}
