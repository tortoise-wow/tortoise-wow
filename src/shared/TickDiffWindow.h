#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

// One world-thread writer; lock-free published values for diagnostic readers.
// Preserve Turtle's fixed-width (zero-padded at startup) average. The maximum
// uses that same window, so an old startup spike eventually expires.
template<std::size_t Size> class TickDiffWindow
{
    static_assert(Size > 0, "tick history cannot be empty");
    std::array<uint32_t, Size> history{};
    std::size_t cursor = 0;
    uint64_t sum = 0;
    std::atomic<uint32_t> current{0}, average{0}, maximum{0};
public:
    void Record(uint32_t diff)
    {
        sum -= history[cursor];
        history[cursor] = diff;
        sum += diff;
        cursor = (cursor + 1) % Size;
        current.store(diff, std::memory_order_relaxed);
        average.store(uint32_t(sum / Size), std::memory_order_relaxed);
        maximum.store(*std::max_element(history.begin(), history.end()), std::memory_order_relaxed);
    }
    uint32_t Current() const { return current.load(std::memory_order_relaxed); }
    uint32_t Average() const { return average.load(std::memory_order_relaxed); }
    uint32_t Maximum() const { return maximum.load(std::memory_order_relaxed); }
};
