#pragma once
#include <cstddef>
#include <utility>

// Stable in-place filtering with a resumable cursor. No element is revisited;
// the caller owns the source lifetime and decides the shared time/work budget.
struct IncrementalFilter
{
    size_t read = 0, write = 0;
    template<class Container, class Predicate, class Permit>
    bool Advance(Container& values, Predicate remove, Permit permit)
    {
        while (read < values.size())
        {
            if (!permit()) return false;
            if (!remove(values[read]))
            {
                if (write != read) values[write] = std::move(values[read]);
                ++write;
            }
            ++read;
        }
        values.resize(write);
        read = write = 0;
        return true;
    }
};
