#ifndef MANTECH_POSITIVE_WEIGHTED_SHUFFLE_H
#define MANTECH_POSITIVE_WEIGHTED_SHUFFLE_H
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

namespace ManTech {
// Weighted sampling without replacement, preserving item/weight association.
// The caller supplies positive integer weights. A Fenwick tree replaces the
// repeated construction of a distribution over the entire remaining suffix.
template<class T, class Generator>
void PositiveWeightedShuffle(std::vector<T>& items, std::vector<std::uint32_t>& weights, Generator& generator)
{
    assert(items.size() == weights.size());
    std::size_t const count = items.size();
    if (!count)
        return;
    std::vector<std::uint64_t> tree(count + 1, 0);
    std::uint64_t total = 0;
    for (std::size_t i = 1; i <= count; ++i)
    {
        assert(weights[i - 1] > 0);
        total += weights[i - 1];
        tree[i] += weights[i - 1];
        auto const parent = i + (i & (~i + 1));
        if (parent <= count)
            tree[parent] += tree[i];
    }
    // Destination weights are at most 200,000; their sum is exactly represented
    // in double for any feasible destination list.
    assert(total <= (std::uint64_t{1} << 53));
    auto subtract = [&](std::size_t index, std::uint64_t value) {
        for (++index; index <= count; index += index & (~index + 1))
            tree[index] -= value;
    };
    auto add = [&](std::size_t index, std::uint64_t value) {
        for (++index; index <= count; index += index & (~index + 1))
            tree[index] += value;
    };
    std::size_t topBit = 1;
    while (topBit <= count / 2)
        topBit <<= 1;
    for (std::size_t first = 0; first < count; ++first)
    {
        double const draw = std::generate_canonical<double, std::numeric_limits<double>::digits>(generator);
        double const target = draw * static_cast<double>(total);
        std::size_t selected = first;
        if (target > 0)
        {
            std::size_t below = 0;
            std::uint64_t prefix = 0;
            for (std::size_t bit = topBit; bit; bit >>= 1)
            {
                auto const next = below + bit;
                if (next <= count && static_cast<double>(prefix + tree[next]) < target)
                {
                    below = next;
                    prefix += tree[next];
                }
            }
            selected = std::min(below, count - 1);
        }
        assert(selected >= first);
        auto const selectedWeight = weights[selected];
        if (selected != first)
        {
            subtract(selected, selectedWeight);
            add(selected, weights[first]);
            std::swap(items[first], items[selected]);
            std::swap(weights[first], weights[selected]);
        }
        subtract(first, selected == first ? selectedWeight : weights[selected]);
        total -= selectedWeight;
    }
}
}
#endif
