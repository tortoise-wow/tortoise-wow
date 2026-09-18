#include "StaticLocationIndex.h"
#include <limits>
#include <iostream>
#include <stdexcept>
static void Check(bool ok) { if (!ok) throw std::runtime_error("static location index contract failed"); }
int main()
{
    StaticLocationIndex<unsigned> cache;
    Check(cache.Insert(0, 1, 2, 3, 41));
    Check(!cache.Insert(0, 1, 2, 3, 99));
    Check(*cache.Find(0, 1, 2, 3) == 41);
    Check(!cache.Find(1, 1, 2, 3)); // maps cannot alias
    Check(!cache.Find(0, 1, 2, std::nextafter(3.0f, 4.0f))); // floors/borders exact
    Check(cache.Insert(0, -0.0f, 0, 0, 7));
    Check(*cache.Find(0, 0, -0.0f, 0) == 7);
    Check(!cache.Insert(0, std::numeric_limits<float>::quiet_NaN(), 0, 0, 1));
    Check(!cache.Find(0, 0, std::numeric_limits<float>::infinity(), 0));
    auto const before = cache.Size();
    for (unsigned n = 0; n < 20000; ++n) Check(!cache.Find(10, float(n), 20, 30));
    Check(cache.Size() == before); // read misses never create work or entries
    for (unsigned n = 0; n < 20000; ++n) Check(cache.Insert(n % 3, float(n), 20, 30, n));
    for (unsigned n = 0; n < 20000; ++n) Check(*cache.Find(n % 3, float(n), 20, 30) == n);
    std::cout << "PASS: exact map/XYZ, no border alias, invalid coordinates, dedup and read-only misses\n";
}
