#pragma once
#include <cmath>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>

// Value-only snapshot used by owner-thread teleport planning. No Player/Map
// pointers outlive the call. Each query visits nine radius-sized buckets
// instead of scanning the entire online population for every candidate.
class PopulationSpatialIndex
{
    struct Key
    {
        uint32_t map, zone;
        int64_t x, y;
        bool operator==(Key const& b) const
        { return map == b.map && zone == b.zone && x == b.x && y == b.y; }
    };
    struct Hash
    {
        size_t operator()(Key const& k) const
        {
            size_t h = k.map;
            auto mix = [&](size_t v) { h ^= v + size_t(0x9e3779b9) + (h << 6) + (h >> 2); };
            mix(k.zone); mix(std::hash<int64_t>{}(k.x)); mix(std::hash<int64_t>{}(k.y));
            return h;
        }
    };
    struct Point { double x, y; };
public:
    explicit PopulationSpatialIndex(double radius) : radius(radius) {}
    void Add(uint32_t map, uint32_t zone, double x, double y)
    {
        if (!Valid(x, y)) return;
        cells[{map, zone, Bucket(x), Bucket(y)}].push_back({x, y});
    }
    bool AtLeast(uint32_t map, uint32_t zone, double x, double y, uint32_t limit) const
    {
        if (!limit) return true;
        if (!Valid(x, y)) return false;
        uint32_t count = 0;
        int64_t const bx = Bucket(x), by = Bucket(y);
        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
            {
                auto it = cells.find({map, zone, bx + dx, by + dy});
                if (it == cells.end()) continue;
                for (auto const& p : it->second)
                    if ((p.x-x)*(p.x-x) + (p.y-y)*(p.y-y) <= radius*radius && ++count >= limit)
                        return true;
            }
        return false;
    }
private:
    bool Valid(double x, double y) const
    {
        return radius > 0 && std::isfinite(radius) && std::isfinite(x) && std::isfinite(y) &&
            std::abs(x / radius) < 1e15 && std::abs(y / radius) < 1e15;
    }
    int64_t Bucket(double value) const { return static_cast<int64_t>(std::floor(value / radius)); }
    double radius;
    std::unordered_map<Key, std::vector<Point>, Hash> cells;
};
