#include "AreaLookupIndex.h"
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <chrono>
struct Area { uint32_t id, flag, map; bool zone; };
static void check(bool b) { if (!b) std::abort(); }
int main()
{
    std::vector<Area> rows;
    AreaLookupIndex index;
    for (uint32_t i = 1; i <= 6000; ++i)
    {
        rows.push_back({i, i % 611, i % 13, i % 7 == 0});
        auto const& a = rows.back();
        index.Add(a.id, a.flag, a.map, a.zone);
    }
    for (uint32_t map = 0; map < 16; ++map)
        for (uint32_t flag = 0; flag < 615; ++flag)
        {
            uint32_t expected = 0;
            for (auto const& a : rows)
                if (flag && flag == a.flag)
                {
                    expected = a.id;
                    if (a.map == map) break;
                }
            check(index.Find(flag, map) == expected);
        }
    for (uint32_t map = 0; map < 16; ++map)
    {
        uint32_t expected = 0;
        for (auto const& a : rows)
            if (a.zone && a.map == map && map != 0 && map != 1)
            { expected = a.flag; break; }
        check(index.MapFlag(map) == expected);
    }
    index = AreaLookupIndex{};
    check(index.Find(1, 1) == 0 && index.MapFlag(3) == 0);
    puts("Area index matches legacy duplicate/fallback semantics across 9840 queries");
}
