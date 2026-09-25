#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std::chrono_literals;
static void Check(bool ok) { if (!ok) throw std::runtime_error("navmesh lifetime assertion failed"); }

static dtTileRef AddFlatTile(dtNavMesh& mesh, int x)
{
    unsigned short verts[] = {0,0,0, 0,0,10, 10,0,10, 10,0,0};
    unsigned short polys[] = {0,1,2,3, 0x8000,0x8001,0x8002,0x8003};
    unsigned short flags[] = {1};
    unsigned char areas[] = {0};
    dtNavMeshCreateParams params{};
    params.verts = verts; params.vertCount = 4;
    params.polys = polys; params.polyFlags = flags; params.polyAreas = areas;
    params.polyCount = 1; params.nvp = 4;
    params.tileX = x; params.tileY = 0; params.tileLayer = 0;
    params.bmin[0] = float(10*x); params.bmax[0] = float(10*(x+1));
    params.bmax[1] = 2; params.bmax[2] = 10;
    params.cs = 1; params.ch = 1; params.walkableHeight = 2;
    params.walkableRadius = 0.5f; params.walkableClimb = 1; params.buildBvTree = true;
    unsigned char* data = nullptr; int size = 0;
    Check(dtCreateNavMeshData(&params, &data, &size));
    dtTileRef ref = 0;
    auto status = mesh.addTile(data, size, DT_TILE_FREE_DATA, 0, &ref);
    if (dtStatusFailed(status)) dtFree(data);
    Check(dtStatusSucceed(status) && ref);
    return ref;
}

static void CheckGate()
{
    dtAccessGate gate;
    // Nested readers and readers inside a writer are supported without taking
    // the underlying non-recursive shared_mutex a second time.
    {
        dtAccessGate::Write write(&gate);
        dtAccessGate::Read read(&gate);
        dtAccessGate::Write nestedWrite(&gate);
        dtAccessGate::Read nestedRead(&gate);
    }
    {
        dtAccessGate::Read read(&gate);
        dtAccessGate::Read nested(&gate);
        bool rejected = false;
        try { dtAccessGate::Write invalidUpgrade(&gate); }
        catch (std::logic_error const&) { rejected = true; }
        Check(rejected);
    }
    std::future<void> waitingWriter;
    std::promise<void> attempting;
    {
        dtAccessGate::Read read(&gate);
        waitingWriter = std::async(std::launch::async, [&] {
            attempting.set_value(); dtAccessGate::Write write(&gate);
        });
        attempting.get_future().wait();
        Check(waitingWriter.wait_for(25ms) == std::future_status::timeout);
    }
    Check(waitingWriter.wait_for(2s) == std::future_status::ready);
    waitingWriter.get();
    {
        dtAccessGate::Read read(&gate);
        auto reader = std::async(std::launch::async, [&] { dtAccessGate::Read other(&gate); });
        Check(reader.wait_for(2s) == std::future_status::ready);
        reader.get();
    }
}

int main()
{
    CheckGate();
    dtNavMesh mesh;
    dtNavMeshParams params{};
    params.tileWidth = 10; params.tileHeight = 10; params.maxTiles = 8; params.maxPolys = 8;
    Check(dtStatusSucceed(mesh.init(&params)));
    auto left = AddFlatTile(mesh, 0), right = AddFlatTile(mesh, 1);
    dtNavMeshQuery query;
    Check(dtStatusSucceed(query.init(&mesh, 128)));
    dtQueryFilter filter;
    float start[] = {5,0,5}, end[] = {15,0,5}, extents[] = {2,3,2}, nearest[3];
    dtPolyRef a = 0, b = 0;
    Check(dtStatusSucceed(query.findNearestPoly(start, extents, &filter, &a, nearest)) && a);
    Check(dtStatusSucceed(query.findNearestPoly(end, extents, &filter, &b, nearest)) && b);
    dtPolyRef path[16]; int pathCount = 0;
    Check(dtStatusSucceed(query.findPath(a,b,start,end,&filter,path,&pathCount,16)) && pathCount == 2);
    Check(dtStatusInProgress(query.initSlicedFindPath(a,b,start,end,&filter)));
    Check(dtStatusSucceed(mesh.removeTile(right,nullptr,nullptr)));
    Check(dtStatusFailed(query.updateSlicedFindPath(8,nullptr)));
    Check(dtStatusFailed(query.closestPointOnPoly(b,end,nearest,nullptr)));
    // A real removeTile must wait while a caller uses a returned raw polygon.
    // This deterministically covers the lifetime contract, not just stress
    // scheduling that might happen to avoid the vulnerable interleaving.
    std::future<dtStatus> removing;
    std::promise<void> removeAttempt;
    bool blocked = false;
    {
        auto read = mesh.acquireRead();
        dtMeshTile const* tile = nullptr; dtPoly const* poly = nullptr;
        Check(dtStatusSucceed(mesh.getTileAndPolyByRef(a, &tile, &poly)));
        removing = std::async(std::launch::async, [&] {
            removeAttempt.set_value();
            return mesh.removeTile(left,nullptr,nullptr);
        });
        removeAttempt.get_future().wait();
        blocked = removing.wait_for(25ms) == std::future_status::timeout;
        Check(tile->header->polyCount == 1 && poly->vertCount == 4);
    }
    Check(removing.wait_for(2s) == std::future_status::ready);
    Check(dtStatusSucceed(removing.get()) && blocked);

    // Two writers repeatedly connect/unlink adjacent tiles while two independent
    // query objects perform real Detour searches. No DB/server/client required.
    std::atomic<bool> startWork{false}, stopReaders{false};
    std::atomic<unsigned> reads{0}, mutations{0};
    std::vector<std::future<void>> readers, writers;
    for (int n=0;n<2;++n) readers.push_back(std::async(std::launch::async,[&] {
        dtNavMeshQuery q; Check(dtStatusSucceed(q.init(&mesh,128)));
        while (!startWork) std::this_thread::yield();
        while (!stopReaders)
        {
            float p[3] = {5,0,5}, near[3]; dtPolyRef ref = 0;
            Check(dtStatusSucceed(q.findNearestPoly(p,extents,&filter,&ref,near)));
            if(ref) { float height; q.getPolyHeight(ref,p,&height); }
            { auto read = mesh.acquireRead();
              for(int i=0;i<mesh.getMaxTiles();++i)
              { auto tile=static_cast<dtNavMesh const&>(mesh).getTile(i); if(tile->header) Check(tile->header->polyCount == 1); }
            }
            ++reads;
        }
    }));
    for(int x=0;x<2;++x) writers.push_back(std::async(std::launch::async,[&,x] {
        while(!startWork) std::this_thread::yield();
        for(int i=0;i<5000;++i)
        { auto ref=AddFlatTile(mesh,x); Check(dtStatusSucceed(mesh.removeTile(ref,nullptr,nullptr))); mutations+=2; }
    }));
    startWork=true;
    std::exception_ptr failure;
    for(auto& writer:writers) { try { writer.get(); } catch(...) { failure=std::current_exception(); } }
    stopReaders=true;
    for(auto& reader:readers) reader.get();
    if(failure) std::rethrow_exception(failure);
    Check(mutations == 20000 && reads > 0);
    { auto read=mesh.acquireRead(); for(int i=0;i<mesh.getMaxTiles();++i) Check(!static_cast<dtNavMesh const&>(mesh).getTile(i)->header); }
    std::cout << "PASS: 20000 real tile mutations, " << reads << " concurrent queries; reader lifetime, nested scopes, stale refs and sliced invalidation\n";
}
