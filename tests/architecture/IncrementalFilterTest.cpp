#include "IncrementalFilter.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
int main()
{
    for (size_t count : {size_t(0), size_t(1), size_t(156), size_t(1000), size_t(6000), size_t(20000)})
        for (size_t budget : {size_t(1), size_t(17), size_t(512)})
        {
            std::vector<unsigned> values;
            for (size_t i = 0; i < count; ++i) values.push_back(unsigned(i));
            auto expected = values;
            auto remove = [](unsigned n) { return n % 3 != 0; };
            expected.erase(std::remove_if(expected.begin(), expected.end(), remove), expected.end());
            IncrementalFilter filter;
            size_t visited = 0;
            bool done = false;
            while (!done)
            {
                size_t remaining = budget;
                done = filter.Advance(values, [&](unsigned n) { ++visited; return remove(n); },
                    [&]() { if (!remaining) return false; --remaining; return true; });
            }
            if (values != expected || visited != count || filter.read || filter.write) std::abort();
        }
    puts("Resumable filtering: exact-once, stable ordering, empty and 20k populations");
}
