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

namespace MMAP
{
// ######################## MMapFactory ########################
// our global singelton copy
MMapManager *g_MMapManager = nullptr;

MMapManager* MMapFactory::createOrGetMMapManager()
{
    if (g_MMapManager == nullptr)
        g_MMapManager = new MMapManager();

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
    fread(&params, sizeof(dtNavMeshParams), 1, file);
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

bool MMapManager::loadMap(uint32 mapId, int32 x, int32 y)
{
    // make sure the mmap is loaded and ready to load tiles
    if (!loadMapData(mapId))
        return false;

    // Keep the registry entry alive until the tile load is complete. Full-map
    // unload takes the exclusive side of loadedMMaps_lock before deleting it.
    std::shared_lock<std::shared_mutex> rlock(loadedMMaps_lock);
    auto const mapIt = loadedMMaps.find(mapId);
    if (mapIt == loadedMMaps.end())
        return false;

    MMapData* mmap = mapIt->second;
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
    MmapTileHeader fileHeader;
    fread(&fileHeader, sizeof(MmapTileHeader), 1, file);

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
        fclose(file);
        return false;
    }

    fclose(file);
    dtTileRef tileRef = 0;

    // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
    // addTile rewires the shared dtNavMesh (tile lookup and neighbour links), so
    // exclude all readers for the complete mutation.
    std::unique_lock<std::shared_mutex> navLock(mmap->navMesh_lock);
    dtStatus dResult = mmap->navMesh->addTile(data, fileHeader.size, DT_TILE_FREE_DATA, 0, &tileRef);
    if (dtStatusSucceed(dResult))
    {
        mmap->mmapLoadedTiles.insert(std::pair<uint32, dtTileRef>(packedGridPos, tileRef));
        loadedTiles.fetch_add(1, std::memory_order_relaxed);
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
    // Stabilize MMapData while the tile is removed.
    std::shared_lock<std::shared_mutex> mapLock(loadedMMaps_lock);
    auto const mapIt = loadedMMaps.find(mapId);
    if (mapIt == loadedMMaps.end())
    {
        // file may not exist, therefore not loaded
        DEBUG_LOG("MMAP:unloadMap: Asked to unload not loaded navmesh map. %03u%02i%02i.mmtile", mapId, x, y);
        return false;
    }

    MMapData* mmap = mapIt->second;

    // Serialize the loaded-tile registry with loadMap().
    std::unique_lock<std::mutex> tileLock(mmap->tilesLoading_lock);
    uint32 packedGridPos = packTileID(x, y);
    auto const tileIt = mmap->mmapLoadedTiles.find(packedGridPos);
    if (tileIt == mmap->mmapLoadedTiles.end())
    {
        // file may not exist, therefore not loaded
        DEBUG_LOG("MMAP:unloadMap: Asked to unload not loaded navmesh tile. %03u%02i%02i.mmtile", mapId, x, y);
        return false;
    }

    dtTileRef tileRef = tileIt->second;

    // A Detour query can retain raw tile/poly pointers for the duration of a
    // logical operation. Wait for those readers before rewiring/freeing tiles.
    std::unique_lock<std::shared_mutex> navLock(mmap->navMesh_lock);
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
        mmap->mmapLoadedTiles.erase(tileIt);
        loadedTiles.fetch_sub(1, std::memory_order_relaxed);
        return true;
    }

    return false;
}

bool MMapManager::unloadMap(uint32 mapId)
{
    // Block new lookups first, then drain active navmesh readers. This order
    // guarantees that MMapData (including navMesh_lock itself) remains alive
    // until every query handle has released it.
    std::unique_lock<std::shared_mutex> mapLock(loadedMMaps_lock);
    auto const mapIt = loadedMMaps.find(mapId);
    if (mapIt == loadedMMaps.end())
    {
        // file may not exist, therefore not loaded
        DEBUG_LOG("MMAP:unloadMap: Asked to unload not loaded navmesh map %03u", mapId);
        return false;
    }

    MMapData* mmap = mapIt->second;
    std::unique_lock<std::shared_mutex> navLock(mmap->navMesh_lock);

    // unload all tiles from given map
    for (MMapTileSet::iterator i = mmap->mmapLoadedTiles.begin(); i != mmap->mmapLoadedTiles.end(); ++i)
    {
        uint32 x = (i->first >> 16);
        uint32 y = (i->first & 0x0000FFFF);
        dtStatus dtResult = mmap->navMesh->removeTile(i->second, nullptr, nullptr);
        if (dtStatusFailed(dtResult))
            sLog.outError("MMAP:unloadMap: Could not unload %03u%02i%02i.mmtile from navmesh", mapId, x, y);
        else
            loadedTiles.fetch_sub(1, std::memory_order_relaxed);
    }

    // Remove the registry entry while it is still globally exclusive so no new
    // handle can discover mmap. Release its own mutex before deleting mmap.
    loadedMMaps.erase(mapIt);
    navLock.unlock();
    delete mmap;
    DETAIL_LOG("MMAP:unloadMap: Unloaded %03i.mmap", mapId);

    return true;
}

bool MMapManager::unloadMapInstance(uint32 mapId, std::thread::id instanceId)
{
    std::shared_lock<std::shared_mutex> mapLock(loadedMMaps_lock);
    auto const mapIt = loadedMMaps.find(mapId);
    if (mapIt == loadedMMaps.end())
    {
        DEBUG_LOG("MMAP:unloadMapInstance: Asked to unload not loaded navmesh map %03u", mapId);
        return false;
    }

    MMapData* mmap = mapIt->second;

    // A query object must not be freed while a query handle can use it. The
    // exclusive navmesh gate drains all such users before touching the set.
    std::unique_lock<std::shared_mutex> navLock(mmap->navMesh_lock);
    std::unique_lock<std::shared_mutex> queryLock(mmap->navMeshQueries_lock);
    auto const queryIt = mmap->navMeshQueries.find(instanceId);
    if (queryIt == mmap->navMeshQueries.end())
    {
        DEBUG_LOG("MMAP:unloadMapInstance: Asked to unload not loaded dtNavMeshQuery mapId %03u instanceId %u", mapId, instanceId);
        return false;
    }

    dtFreeNavMeshQuery(queryIt->second);
    mmap->navMeshQueries.erase(queryIt);
    DETAIL_LOG("MMAP:unloadMapInstance: Unloaded mapId %03u instanceId %u", mapId, instanceId);

    return true;
}

bool MMapManager::IsNavMeshLoaded(uint32 mapId)
{
    std::shared_lock<std::shared_mutex> mapLock(loadedMMaps_lock);
    return loadedMMaps.find(mapId) != loadedMMaps.end();
}

dtNavMeshQuery const* MMapManager::GetOrCreateNavMeshQuery(MMapData* mmap, uint32 identifier, bool model)
{
    std::thread::id tid = std::this_thread::get_id();
    std::shared_lock<std::shared_mutex> lock(mmap->navMeshQueries_lock);
    auto it = mmap->navMeshQueries.find(tid);
    if (it != mmap->navMeshQueries.end())
        return it->second;

    lock.unlock();
    std::unique_lock<std::shared_mutex> ulock(mmap->navMeshQueries_lock);

    // Recheck after upgrading: another caller may have populated the entry
    // while the shared lock was released.
    it = mmap->navMeshQueries.find(tid);
    if (it != mmap->navMeshQueries.end())
        return it->second;

    dtNavMeshQuery* navMeshQuery = dtAllocNavMeshQuery();
    MANGOS_ASSERT(navMeshQuery);
    dtStatus dtResult = navMeshQuery->init(mmap->navMesh, 2048);
    if (dtStatusFailed(dtResult))
    {
        dtFreeNavMeshQuery(navMeshQuery);
        if (model)
            sLog.outError("MMAP:GetNavMeshQuery: Failed to initialize dtNavMeshQuery for displayid %03u thread %u", identifier, tid);
        else
            sLog.outError("MMAP:GetNavMeshQuery: Failed to initialize dtNavMeshQuery for mapId %03u thread %u", identifier, tid);
        return nullptr;
    }

    if (model)
        DETAIL_LOG("MMAP:GetNavMeshQuery: created dtNavMeshQuery for displayid %03u thread %u", identifier, tid);
    else
        DETAIL_LOG("MMAP:GetNavMeshQuery: created dtNavMeshQuery for mapId %03u thread %u", identifier, tid);

    mmap->navMeshQueries.insert(std::pair<std::thread::id, dtNavMeshQuery*>(tid, navMeshQuery));
    return navMeshQuery;
}

NavMeshQueryHandle MMapManager::AcquireNavMeshQuery(uint32 mapId)
{
    // Acquire the mesh reader while the registry itself is pinned. Full-map
    // unload takes these in the opposite modes but the same order.
    std::shared_lock<std::shared_mutex> mapLock(loadedMMaps_lock);
    auto const mapIt = loadedMMaps.find(mapId);
    if (mapIt == loadedMMaps.end())
        return NavMeshQueryHandle();

    MMapData* mmap = mapIt->second;
    std::shared_lock<std::shared_mutex> navLock(mmap->navMesh_lock);
    mapLock.unlock();

    dtNavMeshQuery const* query = GetOrCreateNavMeshQuery(mmap, mapId, false);
    if (!query)
        return NavMeshQueryHandle();

    return NavMeshQueryHandle(query, std::move(navLock));
}

bool MMapManager::loadGameObject(uint32 displayId)
{
    // we already have this model loaded?
    {
        std::unique_lock<std::mutex> modelLock(lockForModels);
        if (loadedModels.find(displayId) != loadedModels.end())
            return true;
    }

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
    std::unique_lock<std::mutex> modelLock(lockForModels);
    if (!loadedModels.insert(std::pair<uint32, MMapData*>(displayId, mmap_data)).second)
        delete mmap_data;
    return true;
}

NavMeshQueryHandle MMapManager::AcquireModelNavMeshQuery(uint32 displayId)
{
    std::unique_lock<std::mutex> modelLock(lockForModels);
    auto const modelIt = loadedModels.find(displayId);
    if (modelIt == loadedModels.end())
        return NavMeshQueryHandle();

    MMapData* mmap = modelIt->second;
    std::shared_lock<std::shared_mutex> navLock(mmap->navMesh_lock);
    modelLock.unlock();

    dtNavMeshQuery const* query = GetOrCreateNavMeshQuery(mmap, displayId, true);
    if (!query)
        return NavMeshQueryHandle();

    return NavMeshQueryHandle(query, std::move(navLock));
}

}
