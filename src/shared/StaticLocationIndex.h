#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <unordered_map>

// Exact map/XYZ keys: never quantize across area borders or different floors.
// Populate before world updates; Find never loads terrain or inserts a miss.
template<class Value> class StaticLocationIndex
{
    using Key = std::array<uint32_t, 4>;
    struct Hash
    {
        size_t operator()(Key const& key) const noexcept
        {
            size_t hash = 2166136261u;
            for (uint32_t part : key) hash = (hash ^ part) * size_t(16777619u);
            return hash;
        }
    };
    static bool MakeKey(uint32_t map, float x, float y, float z, Key& key)
    {
        key[0] = map;
        float values[] = {x, y, z};
        for (unsigned n = 0; n < 3; ++n)
        {
            if (!std::isfinite(values[n])) return false;
            if (values[n] == 0) values[n] = 0.0f; // canonicalize negative zero
            std::memcpy(&key[n + 1], &values[n], sizeof(float));
        }
        return true;
    }
    std::unordered_map<Key, Value, Hash> entries;
public:
    Value const* Find(uint32_t map, float x, float y, float z) const
    {
        Key key;
        if (!MakeKey(map, x, y, z, key)) return nullptr;
        auto it = entries.find(key);
        return it == entries.end() ? nullptr : &it->second;
    }
    bool Insert(uint32_t map, float x, float y, float z, Value const& value)
    {
        Key key;
        return MakeKey(map, x, y, z, key) && entries.emplace(key, value).second;
    }
    size_t Size() const { return entries.size(); }
};
