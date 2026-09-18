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

#include "Log.h"
#include "World.h"
#include "VMapFactory.h"
#include "MoveMap.h"
#include "MoveMapSharedDefines.h"
#include "Config/Config.h"

namespace MMAP
{
// ######################## MMapFactory ########################
// our global singelton copy
MMapManager *g_MMapManager = nullptr;

MMapManager* MMapFactory::createOrGetMMapManager()
{
    if (g_MMapManager == nullptr)
    {
        g_MMapManager = new MMapManager();
        sLog.outString("NAVMESH_LIFETIME shared_queries=1 exclusive_tile_changes=1 guarded_terrain_cleanup=1");
    }

    return g_MMapManager;
}

void MMapFactory::clear()
{
    if (g_MMapManager)
    {
        delete g_MMapManager;
        g_MMapManager = nullptr;
    }
}

// ######################## MMapManager ########################
MMapManager::~MMapManager()
{
    for (const auto& loadedMMap : loadedMMaps)
        delete loadedMMap.second;
    for (const auto& model : loadedModels)
        delete model.second;

    // by now we should not have maps loaded
    // if we had, tiles in MMapData->mmapLoadedTiles, their actual data is lost!
}

bool MMapManager::loadMapData(uint32 mapId)
{
    std::shared_lock<std::shared_mutex> rlock(loadedMMaps_lock);
    // we already have this map loaded?
    if (loadedMMaps.find(mapId) != loadedMMaps.end())
    {
        return true;
    }
    rlock.unlock();

    if (!sWorld.getConfig(CONFIG_BOOL_MMAP_ENABLED))
        return false;

    // load and init dtNavMesh - read parameters from file
    uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i.mmap") + 1;
    char *fileName = new char[pathLen];
    snprintf(fileName, pathLen, (sWorld.GetDataPath() + "mmaps/%03i.mmap").c_str(), mapId);

    FILE* file = fopen(fileName, "rb");
    if (!file)
    {
        DEBUG_LOG("MMAP:loadMapData: Error: Could not open mmap file '%s'", fileName);
        delete [] fileName;
        return false;
    }

    dtNavMeshParams params;
    if (fread(&params, sizeof(dtNavMeshParams), 1, file) != 1)
    {
        fclose(file);
        sLog.outError("MMAP:loadMapData: Truncated parameters in %s", fileName);
        delete [] fileName;
        return false;
    }
    fclose(file);

    dtNavMesh* mesh = dtAllocNavMesh();
    MANGOS_ASSERT(mesh);
    dtStatus dtResult = mesh->init(&params);
    if (dtStatusFailed(dtResult))
    {
        dtFreeNavMesh(mesh);
        sLog.outError("MMAP:loadMapData: Failed to initialize dtNavMesh for mmap %03u from file %s with %u tiles. Result 0x%x.", mapId, fileName, params.maxTiles, dtResult);
        delete [] fileName;
        return false;
    }

    delete [] fileName;

    DETAIL_LOG("MMAP:loadMapData: Loaded %03i.mmap", mapId);

    // store inside our map list
    MMapData* mmap_data = new MMapData(mesh);
    mmap_data->mmapLoadedTiles.clear();

    std::unique_lock<std::shared_mutex> wlock(loadedMMaps_lock);
    if (loadedMMaps.find(mapId) == loadedMMaps.end())
        loadedMMaps.insert(std::pair<uint32, MMapData*>(mapId, mmap_data));
    else
        delete mmap_data;

    return true;
}

uint32 MMapManager::packTileID(int32 x, int32 y)
{
    return uint32(x << 16 | y);
}

bool MMapManager::IsMapTileLoaded(uint32 mapId, int32 x, int32 y) const
{
    if (x < 0 || x >= 64 || y < 0 || y >= 64)
        return false;
    std::shared_lock<std::shared_mutex> mapGuard(loadedMMaps_lock);
    auto const entry = loadedMMaps.find(mapId);
    if (entry == loadedMMaps.end())
        return false;
    std::lock_guard<std::mutex> tileGuard(entry->second->tilesLoading_lock);
    return entry->second->mmapLoadedTiles.count(packTileID(x, y)) != 0;
}

bool MMapManager::loadMap(uint32 mapId, int32 x, int32 y)
{
    // make sure the mmap is loaded and ready to load tiles
    if (!loadMapData(mapId))
        return false;

    // get this mmap data
    std::shared_lock<std::shared_mutex> rlock(loadedMMaps_lock);
    MMapData* mmap = loadedMMaps.at(mapId);
    rlock.unlock();
    MANGOS_ASSERT(mmap->navMesh);

    // check if we already have this tile loaded
    uint32 packedGridPos = packTileID(x, y);
    std::unique_lock<std::mutex> wlock(mmap->tilesLoading_lock);
    if (mmap->mmapLoadedTiles.find(packedGridPos) != mmap->mmapLoadedTiles.end())
        return false;

    // load this tile :: mmaps/MMMXXYY.mmtile
    uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i%02i%02i.mmtile") + 1;
    char *fileName = new char[pathLen];
    snprintf(fileName, pathLen, (sWorld.GetDataPath() + "mmaps/%03i%02i%02i.mmtile").c_str(), mapId, y, x);

    FILE *file = fopen(fileName, "rb");
    if (!file)
    {
        //mmaps not generated on every tile. But it's often generating, where vmap placed (most of the time)
        if (VMAP::VMapFactory::createOrGetVMapManager()->existsMap((sWorld.GetDataPath() + "vmaps").c_str(), mapId, x, y))
        {
            DEBUG_LOG("MMAP:loadMap: Could not open mmtile file '%s' and vmap is exist in this tile", fileName);
        }
        delete [] fileName;
        return false;
    }
    delete [] fileName;

    // read header
    MmapTileHeader fileHeader{};
    if (fread(&fileHeader, sizeof(MmapTileHeader), 1, file) != 1 || !fileHeader.size)
    {
        sLog.outError("MMAP:loadMap: Truncated tile header for map %u grid %i,%i", mapId, x, y);
        fclose(file);
        return false;
    }

    if (fileHeader.mmapMagic != MMAP_MAGIC)
    {
        sLog.outError("MMAP:loadMap: Bad header in mmap %03u%02i%02i.mmtile", mapId, x, y);
        fclose(file);
        return false;
    }

    if (fileHeader.mmapVersion != MMAP_VERSION)
    {
        sLog.outError("MMAP:loadMap: %03u%02i%02i.mmtile was built with generator v%i, expected v%i",
                      mapId, x, y, fileHeader.mmapVersion, MMAP_VERSION);
        fclose(file);
        return false;
    }

    unsigned char* data = (unsigned char*)dtAlloc(fileHeader.size, DT_ALLOC_PERM);
    MANGOS_ASSERT(data);

    size_t result = fread(data, fileHeader.size, 1, file);
    if (!result)
    {
        sLog.outError("MMAP:loadMap: Bad header or data in mmap %03u%02i%02i.mmtile", mapId, x, y);
        dtFree(data);
        fclose(file);
        return false;
    }

    fclose(file);
    dtTileRef tileRef = 0;

    // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
    dtStatus dResult = mmap->navMesh->addTile(data, fileHeader.size, DT_TILE_FREE_DATA, 0, &tileRef);
    if (dtStatusSucceed(dResult))
    {
        ManTech::MemoryLedger::Add(ManTech::MemoryKind::NavTiles, fileHeader.size);
        mmap->mmapLoadedTiles.insert(std::pair<uint32, dtTileRef>(packedGridPos, tileRef));
        ++loadedTiles;
        return true;
    }
    else
    {
        sLog.outError("MMAP:loadMap: Could not load %03u%02i%02i.mmtile into navmesh [result 0x%x]", mapId, x, y, dResult);
        dtFree(data);
        return false;
    }

    return false;
}

bool MMapManager::unloadMap(uint32 mapId, int32 x, int32 y)
{
    // Loading and unloading share the tile-table lock. Detour additionally
    // excludes active queries while either operation changes tile topology.
    if (!sWorld.getConfig(CONFIG_BOOL_MMAP_TILE_UNLOAD))
        return false;

    std::shared_lock<std::shared_mutex> mapGuard(loadedMMaps_lock);
    // check if we have this map loaded
    if (loadedMMaps.find(mapId) == loadedMMaps.end())
    {
        // file may not exist, therefore not loaded
        DEBUG_LOG("MMAP:unloadMap: Asked to unload not loaded navmesh map. %03u%02i%02i.mmtile", mapId, x, y);
        return false;
    }

    MMapData* mmap = loadedMMaps[mapId];

    std::unique_lock<std::mutex> tileGuard(mmap->tilesLoading_lock);

    // check if we have this tile loaded
    uint32 packedGridPos = packTileID(x, y);
    if (mmap->mmapLoadedTiles.find(packedGridPos) == mmap->mmapLoadedTiles.end())
    {
        // file may not exist, therefore not loaded
        DEBUG_LOG("MMAP:unloadMap: Asked to unload not loaded navmesh tile. %03u%02i%02i.mmtile", mapId, x, y);
        return false;
    }

    dtTileRef tileRef = mmap->mmapLoadedTiles[packedGridPos];

    auto const* removedTile = mmap->navMesh->getTileByRef(tileRef);
    size_t const removedBytes = removedTile ? removedTile->dataSize : 0;
    // unload, and mark as non loaded
    dtStatus dtResult = mmap->navMesh->removeTile(tileRef, nullptr, nullptr);
    if (dtStatusFailed(dtResult))
    {
        // this is technically a memory leak
        // if the grid is later reloaded, dtNavMesh::addTile will return error but no extra memory is used
        // we cannot recover from this error - assert out
        sLog.outError("MMAP:unloadMap: Could not unload %03u%02i%02i.mmtile from navmesh", mapId, x, y);
        MANGOS_ASSERT(false);
    }
    else
    {
        ManTech::MemoryLedger::Remove(ManTech::MemoryKind::NavTiles, removedBytes);
        mmap->mmapLoadedTiles.erase(packedGridPos);
        --loadedTiles;
        return true;
    }

    return false;
}

bool MMapManager::unloadMap(uint32 mapId)
{
    // Retain the small mesh/query owner until shutdown: legacy callers cache
    // raw query pointers. Reclaim tile buffers, not their live owner object.
    if (!sWorld.getConfig(CONFIG_BOOL_MMAP_TILE_UNLOAD))
        return false;

    std::shared_lock<std::shared_mutex> mapGuard(loadedMMaps_lock);
    if (loadedMMaps.find(mapId) == loadedMMaps.end())
    {
        // file may not exist, therefore not loaded
        DEBUG_LOG("MMAP:unloadMap: Asked to unload not loaded navmesh map %03u", mapId);
        return false;
    }

    // unload all tiles from given map
    MMapData* mmap = loadedMMaps[mapId];
    std::unique_lock<std::mutex> tileGuard(mmap->tilesLoading_lock);
    dtAccessGate::Write navWrite(mmap->navMesh->accessGate());
    for (MMapTileSet::iterator i = mmap->mmapLoadedTiles.begin(); i != mmap->mmapLoadedTiles.end(); ++i)
    {
        uint32 x = (i->first >> 16);
        uint32 y = (i->first & 0x0000FFFF);
        auto const* removedTile = mmap->navMesh->getTileByRef(i->second);
        size_t const removedBytes = removedTile ? removedTile->dataSize : 0;
        dtStatus dtResult = mmap->navMesh->removeTile(i->second, nullptr, nullptr);
        if (dtStatusFailed(dtResult))
            sLog.outError("MMAP:unloadMap: Could not unload %03u%02i%02i.mmtile from navmesh", mapId, x, y);
        else
        {
            ManTech::MemoryLedger::Remove(ManTech::MemoryKind::NavTiles, removedBytes);
            --loadedTiles;
        }
    }

    mmap->mmapLoadedTiles.clear();
    DETAIL_LOG("MMAP:unloadMap: Unloaded %03i.mmap", mapId);

    return true;
}

bool MMapManager::unloadMapInstance(uint32 mapId, std::thread::id instanceId)
{
    // Only an owning thread may retire its own query after its operations end.
    if (instanceId != std::this_thread::get_id()) return false;
    std::shared_lock<std::shared_mutex> mapGuard(loadedMMaps_lock);
    // check if we have this map loaded
    if (loadedMMaps.find(mapId) == loadedMMaps.end())
    {
        // file may not exist, therefore not loaded
        DEBUG_LOG("MMAP:unloadMapInstance: Asked to unload not loaded navmesh map %03u", mapId);
        return false;
    }

    MMapData* mmap = loadedMMaps[mapId];
    std::unique_lock<std::shared_mutex> queryGuard(mmap->navMeshQueries_lock);
    if (mmap->navMeshQueries.find(instanceId) == mmap->navMeshQueries.end())
    {
        DEBUG_LOG("MMAP:unloadMapInstance: Asked to unload not loaded dtNavMeshQuery mapId %03u instanceId %u", mapId, instanceId);
        return false;
    }

    dtNavMeshQuery* query = mmap->navMeshQueries[instanceId];

    ManTech::MemoryLedger::Remove(ManTech::MemoryKind::NavQueries, query->getOwnedMemoryBytes());
    dtFreeNavMeshQuery(query);
    mmap->navMeshQueries.erase(instanceId);
    DETAIL_LOG("MMAP:unloadMapInstance: Unloaded mapId %03u instanceId %u", mapId, instanceId);

    return true;
}

dtNavMesh const* MMapManager::GetNavMesh(uint32 mapId)
{
    std::shared_lock<std::shared_mutex> mapGuard(loadedMMaps_lock);
    if (loadedMMaps.find(mapId) == loadedMMaps.end())
        return nullptr;

    return loadedMMaps[mapId]->navMesh;
}

dtNavMeshQuery const* MMapManager::GetNavMeshQuery(uint32 mapId)
{
    std::shared_lock<std::shared_mutex> mapGuard(loadedMMaps_lock);
    if (loadedMMaps.find(mapId) == loadedMMaps.end())
        return nullptr;

    std::thread::id tid= std::this_thread::get_id();
    MMapData* mmap = loadedMMaps[mapId];
    std::shared_lock<std::shared_mutex> lock(mmap->navMeshQueries_lock);

    NavMeshQuerySet::iterator it = mmap->navMeshQueries.find(tid);
    dtNavMeshQuery* navMeshQuery = nullptr;
    if (it == mmap->navMeshQueries.end())
    {
        lock.unlock();
        std::unique_lock<std::shared_mutex> ulock(mmap->navMeshQueries_lock);

        // allocate mesh query
        navMeshQuery = dtAllocNavMeshQuery();
        MANGOS_ASSERT(navMeshQuery);
        // Complex custom maps can exhaust the legacy 2048-node search even
        // when a complete corridor exists. Keep the budget per map and query
        // owner, with a fixed upper bound; never replace a live query on reload.
        std::string nodeKey = "mmap.QueryNodes." + std::to_string(mapId);
        int const requestedNodes = sConfig.GetIntDefault(nodeKey.c_str(), 2048);
        int const queryNodes = std::max(2048, std::min(65535, requestedNodes));
        dtStatus dtResult = navMeshQuery->init(mmap->navMesh, queryNodes);
        if (dtStatusFailed(dtResult))
        {
            ulock.unlock();
            dtFreeNavMeshQuery(navMeshQuery);
            sLog.outError("MMAP:GetNavMeshQuery: Failed to initialize dtNavMeshQuery for mapId %03u thread %u", mapId, tid);
            return nullptr;
        }

        DETAIL_LOG("MMAP:GetNavMeshQuery: created dtNavMeshQuery for mapId %03u thread %u nodes %d", mapId, tid, queryNodes);
        mmap->navMeshQueries.insert(std::pair<std::thread::id, dtNavMeshQuery*>(tid, navMeshQuery));
        ManTech::MemoryLedger::Add(ManTech::MemoryKind::NavQueries, navMeshQuery->getOwnedMemoryBytes());
    }
    else
        navMeshQuery = it->second;

    return navMeshQuery;
}

bool MMapManager::loadGameObject(uint32 displayId)
{
    std::lock_guard<std::mutex> modelGuard(lockForModels);
    // we already have this map loaded?
    if (loadedModels.find(displayId) != loadedModels.end())
        return true;

    // load and init dtNavMesh - read parameters from file
    uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/go%04i.mmap") + 1;
    char *fileName = new char[pathLen];
    snprintf(fileName, pathLen, (sWorld.GetDataPath() + "mmaps/go%04i.mmap").c_str(), displayId);

    FILE* file = fopen(fileName, "rb");
    if (!file)
    {
        DEBUG_LOG("MMAP:loadGameObject: Error: Could not open mmap file %s", fileName);
        delete [] fileName;
        return false;
    }

    MmapTileHeader fileHeader;
    fread(&fileHeader, sizeof(MmapTileHeader), 1, file);

    if (fileHeader.mmapMagic != MMAP_MAGIC)
    {
        sLog.outError("MMAP:loadGameObject: Bad header in mmap %s", fileName);
        fclose(file);
        return false;
    }

    if (fileHeader.mmapVersion != MMAP_VERSION)
    {
        sLog.outError("MMAP:loadGameObject: %s was built with generator v%i, expected v%i",
                      fileName, fileHeader.mmapVersion, MMAP_VERSION);
        fclose(file);
        return false;
    }
    unsigned char* data = (unsigned char*)dtAlloc(fileHeader.size, DT_ALLOC_PERM);
    MANGOS_ASSERT(data);

    size_t result = fread(data, fileHeader.size, 1, file);
    if (!result)
    {
        sLog.outError("MMAP:loadGameObject: Bad header or data in mmap %s", fileName);
        fclose(file);
        return false;
    }

    fclose(file);

    dtNavMesh* mesh = dtAllocNavMesh();
    MANGOS_ASSERT(mesh);
    dtStatus r = mesh->init(data, fileHeader.size, DT_TILE_FREE_DATA);
    if (dtStatusFailed(r))
    {
        dtFreeNavMesh(mesh);
        sLog.outError("MMAP:loadGameObject: Failed to initialize dtNavMesh from file %s. Result 0x%x.", fileName, r);
        delete [] fileName;
        return false;
    }
    DETAIL_LOG("MMAP:loadGameObject: Loaded file %s [size=%u]", fileName, fileHeader.size);
    delete [] fileName;

    MMapData* mmap_data = new MMapData(mesh);
    loadedModels.insert(std::pair<uint32, MMapData*>(displayId, mmap_data));
    return true;
}

dtNavMeshQuery const* MMapManager::GetModelNavMeshQuery(uint32 displayId)
{
    std::lock_guard<std::mutex> modelGuard(lockForModels);
    if (loadedModels.find(displayId) == loadedModels.end())
        return nullptr;

    std::thread::id tid = std::this_thread::get_id();
    MMapData* mmap = loadedModels[displayId];
    if (mmap->navMeshQueries.find(tid) == mmap->navMeshQueries.end())
    {
        // Model and per-thread query lookup are protected from the first read.
        if (mmap->navMeshQueries.find(tid) == mmap->navMeshQueries.end())
        {
            // allocate mesh query
            dtNavMeshQuery* query = dtAllocNavMeshQuery();
            MANGOS_ASSERT(query);
            if (dtStatusFailed(query->init(mmap->navMesh, 2048)))
            {
                dtFreeNavMeshQuery(query);
                sLog.outError("MMAP:GetNavMeshQuery: Failed to initialize dtNavMeshQuery for displayid %03u tid %u", displayId, tid);
                return nullptr;
            }

            DETAIL_LOG("MMAP:GetNavMeshQuery: created dtNavMeshQuery for displayid %03u tid %u", displayId, tid);
            mmap->navMeshQueries.insert(std::pair<std::thread::id, dtNavMeshQuery*>(tid, query));
            ManTech::MemoryLedger::Add(ManTech::MemoryKind::NavQueries, query->getOwnedMemoryBytes());
        }
    }

    return mmap->navMeshQueries[tid];
}
}
