/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MOVE_MAP_H
#define _MOVE_MAP_H

#include "Platform/CompilerDefs.h"
#include "Platform/Define.h"
#include <unordered_map>

#include "Detour/Include/DetourAlloc.h"
#include "Detour/Include/DetourNavMesh.h"
#include "Detour/Include/DetourNavMeshQuery.h"

#include <thread>
#include <shared_mutex>
#include <atomic>
#include "Memory/MemoryLedger.h"

//  memory management
inline void* dtCustomAlloc(size_t size, dtAllocHint /*hint*/)
{
    return (void*)new unsigned char[size];
}

inline void dtCustomFree(void* ptr)
{
    delete [] (unsigned char*)ptr;
}

//  move map related classes
namespace MMAP
{
    typedef std::unordered_map<uint32, dtTileRef> MMapTileSet;
    typedef std::unordered_map<std::thread::id, dtNavMeshQuery*> NavMeshQuerySet;

    // dummy struct to hold map's mmap data
    struct MMapData
    {
        MMapData(dtNavMesh* mesh) : navMesh(mesh)
        {
            ManTech::MemoryLedger::Add(ManTech::MemoryKind::NavShared, sizeof(*this) + mesh->getOwnedMemoryBytes());
            // Model meshes already own their single tile when installed.
            for (int i = 0; i < mesh->getMaxTiles(); ++i)
            {
                auto tile = static_cast<dtNavMesh const*>(mesh)->getTile(i);
                if (tile && tile->header && tile->data)
                    ManTech::MemoryLedger::Add(ManTech::MemoryKind::NavTiles, tile->dataSize);
            }
        }
        ~MMapData()
        {
            for (const auto& itr : navMeshQueries)
            {
                ManTech::MemoryLedger::Remove(ManTech::MemoryKind::NavQueries, itr.second->getOwnedMemoryBytes());
                dtFreeNavMeshQuery(itr.second);
            }

            if (navMesh)
            {
                for (int i = 0; i < navMesh->getMaxTiles(); ++i)
                {
                    auto tile = static_cast<dtNavMesh const*>(navMesh)->getTile(i);
                    if (tile && tile->header && tile->data)
                        ManTech::MemoryLedger::Remove(ManTech::MemoryKind::NavTiles, tile->dataSize);
                }
                ManTech::MemoryLedger::Remove(ManTech::MemoryKind::NavShared, sizeof(*this) + navMesh->getOwnedMemoryBytes());
                dtFreeNavMesh(navMesh);
            }
        }

        dtNavMesh* navMesh;

        // we have to use single dtNavMeshQuery for every instance, since those are not thread safe
        NavMeshQuerySet navMeshQueries; // threadId to query
        std::shared_mutex navMeshQueries_lock;
        MMapTileSet mmapLoadedTiles; // maps [map grid coords] to [dtTile]
        std::mutex tilesLoading_lock;
    };

    typedef std::unordered_map<uint32, MMapData*> MMapDataSet;

    // singelton class
    // holds all all access to mmap loading unloading and meshes
    class MMapManager
    {
        public:
            MMapManager() : loadedTiles(0) {}
            ~MMapManager();

            bool loadMap(uint32 mapId, int32 x, int32 y);
            bool IsMapTileLoaded(uint32 mapId, int32 x, int32 y) const;
            // bot's 4-arg forms.
            bool loadMap(uint32 mapId, int32 x, int32 y, uint32 /*instanceId*/) { return loadMap(mapId, x, y); }
            bool loadMap(std::string const& /*dataPath*/, uint32 mapId, int32 x, int32 y) { return loadMap(mapId, x, y); }
            bool loadGameObject(uint32 displayId);
            bool unloadMap(uint32 mapId, int32 x, int32 y);
            bool unloadMap(uint32 mapId);
            // bot calls these with various arg counts.
            // Stubs that ignore extra args.
            template<typename... A> bool loadAllMapTiles(A... /*args*/) { return false; }
            template<typename... A> bool loadMapInstance(A... /*args*/) { return false; }
            template<typename... A> bool IsMMapIsLoaded(A... /*args*/) const { return true; }
            template<typename... A> bool loadMapAlt(A... /*args*/) { return false; }
            bool unloadMapInstance(uint32 mapId, std::thread::id instanceId);

            // The returned [dtNavMeshQuery const*] is NOT threadsafe
            // Returns a NavMeshQuery valid for current thread only.
            dtNavMeshQuery const* GetNavMeshQuery(uint32 mapId);
            // bot's 2-arg form (instanceId ignored).
            dtNavMeshQuery const* GetNavMeshQuery(uint32 mapId, uint32 /*instanceId*/) { return GetNavMeshQuery(mapId); }
            dtNavMeshQuery const* GetModelNavMeshQuery(uint32 displayId);
            dtNavMesh const* GetNavMesh(uint32 mapId);

            uint32 getLoadedTilesCount() const { return loadedTiles; }
            uint32 getLoadedMapsCount() const { std::shared_lock<std::shared_mutex> guard(loadedMMaps_lock); return loadedMMaps.size(); }
        private:
            bool loadMapData(uint32 mapId);
            static uint32 packTileID(int32 x, int32 y);

            MMapDataSet loadedMMaps;
            mutable std::shared_mutex loadedMMaps_lock;
            MMapDataSet loadedModels;

            std::atomic<uint32> loadedTiles;
            std::mutex lockForModels;
    };

    // static class
    // holds all mmap global data
    // access point to MMapManager singelton
    class MMapFactory
    {
        public:
            static MMapManager* createOrGetMMapManager();
            static void clear();
    };
}

#endif  // _MOVE_MAP_H
