/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TransportMgr.h"
#include "Transport.h"
#include "MoveSpline.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "MoveMap.h"
#include "World.h"

TransportMgr sTransportMgr;

TransportTemplate::~TransportTemplate()
{
    // Collect shared pointers into a set to avoid deleting the same memory more than once
    std::set<TransportSpline*> splines;
    for (const auto& keyFrame : keyFrames)
        splines.insert(keyFrame.Spline);

    for (const auto& spline : splines)
        delete spline;
}

TransportMgr::TransportMgr() { }

TransportMgr::~TransportMgr() { }

void TransportMgr::Unload()
{
    // let os clean the memory, sometimes crashes on delete, dunno why
    //for (auto const& pTransport : m_shipTransports)
    //    delete pTransport;
    m_shipTransports.clear();

    _transportTemplates.clear();
}

void TransportMgr::LoadTransportTemplates()
{
    uint32 oldMSTime = WorldTimer::getMSTime();

    QueryResult* result = WorldDatabase.Query("SELECT `entry` FROM `gameobject_template` WHERE `type` = 15 ORDER BY entry ASC");

    if (!result)
    {
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        GameObjectInfo const* goInfo = sObjectMgr.GetGameObjectInfo(entry);
        if (goInfo == nullptr)
        {
            sLog.outErrorDb("Transport %u has no associated GameObjectTemplate from `gameobject_template` , skipped.", entry);
            continue;
        }

        if (goInfo->moTransport.taxiPathId >= sTaxiPathNodesByPath.size())
        {
            sLog.outErrorDb("Transport %u (name: %s) has an invalid path specified in `gameobject_template`.`data0` (%u) field, skipped.", entry, goInfo->name.c_str(), goInfo->moTransport.taxiPathId);
            continue;
        }

        // paths are generated per template, saves us from generating it again in case of instanced transports
        TransportTemplate& transport = _transportTemplates[entry];
        transport.entry = entry;
        GeneratePath(goInfo, &transport);
        MMAP::MMapFactory::createOrGetMMapManager()->loadGameObject(goInfo->displayId);

        // transports in instance are only on one map
        if (transport.inInstance)
            _instanceTransports[*transport.mapsUsed.begin()].insert(entry);

        ++count;
    }
    while (result->NextRow());

    delete result;
}

class SplineRawInitializer
{
public:
    SplineRawInitializer(Movement::PointsArray& points) : _points(points) { }

    void operator()(uint8& mode, bool& cyclic, Movement::PointsArray& points, int& lo, int& hi) const
    {
        mode = Movement::SplineBase::ModeCatmullrom;
        cyclic = false;
        points.assign(_points.begin(), _points.end());
        lo = 1;
        hi = points.size() - 2;
    }

    Movement::PointsArray& _points;
};

void TransportMgr::GeneratePath(GameObjectInfo const* goInfo, TransportTemplate* transport)
{
    uint32 pathId = goInfo->moTransport.taxiPathId;
    TaxiPathNodeList const& path = sTaxiPathNodesByPath[pathId];
    std::vector<KeyFrame>& keyFrames = transport->keyFrames;
    Movement::PointsArray splinePath, allPoints;
    bool mapChange = false;

    // The client reads a path only from consecutive `TaxiPathNode` records, so
    // nodes stored elsewhere are not part of it.
    size_t nodeCount = 1;
    while (nodeCount < path.size() && &path[nodeCount] == &path[nodeCount - 1] + 1)
        ++nodeCount;

    for (size_t i = 0; i < nodeCount; ++i)
        allPoints.push_back(G3D::Vector3(path[i].x, path[i].y, path[i].z));

    // Add extra points to allow derivative calculations for all path nodes
    allPoints.insert(allPoints.begin(), allPoints.front().lerp(allPoints[1], -0.2f));
    allPoints.push_back(allPoints.back().lerp(allPoints[allPoints.size() - 2], -0.2f));
    allPoints.push_back(allPoints.back().lerp(allPoints[allPoints.size() - 2], -1.0f));

    SplineRawInitializer initer(allPoints);
    TransportSpline orientationSpline;
    orientationSpline.init_spline_custom(initer);
    orientationSpline.initLengths();

    for (size_t i = 1; i < nodeCount - 1; ++i)
    {
        if (!mapChange)
        {
            TaxiPathNodeEntry const& node_i = path[i];
            if (i != nodeCount - 1 && (node_i.actionFlag & 1 || node_i.mapid != path[i + 1].mapid))
            {
                keyFrames.back().Teleport = true;
                mapChange = true;
            }
            else
            {
                KeyFrame k(node_i);
                G3D::Vector3 h;
                orientationSpline.evaluate_derivative(i + 1, 0.0f, h);
                k.InitialOrientation = MapManager::NormalizeOrientation(atan2(h.y, h.x) + M_PI);

                keyFrames.push_back(k);
                splinePath.push_back(G3D::Vector3(node_i.x, node_i.y, node_i.z));
                transport->mapsUsed.insert(k.Node->mapid);
            }
        }
        else
            mapChange = false;
    }

    ASSERT(!keyFrames.empty());

    if (transport->mapsUsed.size() > 1)
    {
        for (const auto itr : transport->mapsUsed)
            ASSERT(!sMapStorage.LookupEntry<MapEntry>(itr)->Instanceable());

        transport->inInstance = false;
    }
    else
        transport->inInstance = sMapStorage.LookupEntry<MapEntry>(*transport->mapsUsed.begin())->Instanceable();

    // last to first is always "teleport", even for closed paths
    keyFrames.back().Teleport = true;

    const float speed = float(goInfo->moTransport.moveSpeed);
    const float accel = float(goInfo->moTransport.accelRate);
    const float accel_dist = 0.5f * speed * speed / accel;

    transport->accelTime = speed / accel;
    transport->accelDist = accel_dist;

    // Distances and times are measured from one stop or teleport to the next.
    // The transport speeds up and slows down only at stops.
    auto isSegmentEnd = [](KeyFrame const& k) { return k.IsStopFrame() || k.IsTeleportFrame(); };

    int32 firstEnd = -1;
    int32 lastEnd = -1;

    // first cell is arrived at by teleportation :S
    keyFrames[0].DistFromPrev = 0;
    keyFrames[0].Index = 1;
    if (isSegmentEnd(keyFrames[0]))
    {
        firstEnd = 0;
        lastEnd = 0;
    }

    // find the rest of the distances between key points
    // Every path segment has its own spline
    size_t start = 0;
    for (size_t i = 1; i < keyFrames.size(); ++i)
    {
        if (keyFrames[i - 1].Teleport || i + 1 == keyFrames.size())
        {
            size_t extra = !keyFrames[i - 1].Teleport ? 1 : 0;
            TransportSpline* spline = new TransportSpline();
            spline->init_spline(&splinePath[start], i - start + extra, Movement::SplineBase::ModeCatmullrom);
            spline->initLengths();
            for (size_t j = start; j < i + extra; ++j)
            {
                keyFrames[j].Index = j - start + 1;
                keyFrames[j].DistFromPrev = spline->length(j - start, j + 1 - start);
                if (j > 0)
                    keyFrames[j - 1].NextDistFromPrev = keyFrames[j].DistFromPrev;
                keyFrames[j].Spline = spline;
            }

            if (keyFrames[i - 1].Teleport)
            {
                keyFrames[i].Index = i - start + 1;
                keyFrames[i].DistFromPrev = 0.0f;
                keyFrames[i - 1].NextDistFromPrev = 0.0f;
                keyFrames[i].Spline = spline;
            }

            start = i;
        }

        if (isSegmentEnd(keyFrames[i]))
        {
            // Remember the first segment end.
            if (firstEnd == -1)
                firstEnd = i;
            lastEnd = i;
        }
    }

    keyFrames.back().NextDistFromPrev = keyFrames.front().DistFromPrev;

    // At a segment end, `DistSinceStop` is 0, and `DistUntilStop` is the
    // distance to the next segment end.
    // this is required to properly handle cases of two stopping frames in a row (yes they do exist)
    float tmpDist = 0.0f;
    bool fromStop = false;
    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        int32 j = (i + lastEnd) % keyFrames.size();
        if (isSegmentEnd(keyFrames[j]))
        {
            tmpDist = 0.0f;
            // Past a teleport, the transport is already moving at full speed.
            fromStop = keyFrames[j].IsStopFrame() && !keyFrames[j].IsTeleportFrame();
        }
        else
            tmpDist += keyFrames[j].DistFromPrev;
        keyFrames[j].DistSinceStop = tmpDist;
        keyFrames[j].FromStop = fromStop;
    }

    tmpDist = 0.0f;
    bool toStop = keyFrames[firstEnd].IsStopFrame();
    for (int32 i = int32(keyFrames.size()) - 1; i >= 0; i--)
    {
        int32 j = (i + firstEnd) % keyFrames.size();
        tmpDist += keyFrames[(j + 1) % keyFrames.size()].DistFromPrev;
        keyFrames[j].DistUntilStop = tmpDist;
        keyFrames[j].ToStop = toStop;
        if (isSegmentEnd(keyFrames[j]))
        {
            tmpDist = 0.0f;
            toStop = keyFrames[j].IsStopFrame();
        }
    }

    // Returns the time the transport needs to cover a distance that starts or
    // ends at a standstill.
    auto rampTime = [&](float dist) -> float
    {
        if (dist < accel_dist)
            return sqrt(2 * dist / accel);
        return (dist - accel_dist) / speed + (speed / accel);
    };

    for (auto& keyFrame : keyFrames)
    {
        float total_dist = keyFrame.DistSinceStop + keyFrame.DistUntilStop;
        if (!keyFrame.FromStop) // The segment starts past a teleport.
            keyFrame.TimeTo = keyFrame.ToStop ? rampTime(keyFrame.DistUntilStop) : keyFrame.DistUntilStop / speed;
        else if (!keyFrame.ToStop) // The segment ends at a teleport.
            keyFrame.TimeTo = rampTime(total_dist) - rampTime(keyFrame.DistSinceStop);
        else if (total_dist < 2 * accel_dist) // won't reach full speed
        {
            if (keyFrame.DistSinceStop < keyFrame.DistUntilStop) // is still accelerating
            {
                // calculate accel+brake time for this short segment
                float segment_time = 2.0f * sqrt((keyFrame.DistUntilStop + keyFrame.DistSinceStop) / accel);
                // substract acceleration time
                keyFrame.TimeTo = segment_time - sqrt(2 * keyFrame.DistSinceStop / accel);
            }
            else // slowing down
                keyFrame.TimeTo = sqrt(2 * keyFrame.DistUntilStop / accel);
        }
        else if (keyFrame.DistSinceStop < accel_dist) // still accelerating (but will reach full speed)
        {
            // calculate accel + cruise + brake time for this long segment
            float segment_time = (keyFrame.DistUntilStop + keyFrame.DistSinceStop) / speed + (speed / accel);
            // substract acceleration time
            keyFrame.TimeTo = segment_time - sqrt(2 * keyFrame.DistSinceStop / accel);
        }
        else if (keyFrame.DistUntilStop < accel_dist) // already slowing down (but reached full speed)
            keyFrame.TimeTo = sqrt(2 * keyFrame.DistUntilStop / accel);
        else // at full speed
            keyFrame.TimeTo = (keyFrame.DistUntilStop / speed) + (0.5f * speed / accel);
    }

    // calculate tFrom times from tTo times
    float segmentTime = 0.0f;
    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        int32 j = (i + lastEnd) % keyFrames.size();
        if (isSegmentEnd(keyFrames[j]))
            segmentTime = keyFrames[j].TimeTo;
        keyFrames[j].TimeFrom = segmentTime - keyFrames[j].TimeTo;
    }

    // calculate path times
    keyFrames[0].ArriveTime = 0;
    float curPathTime = 0.0f;
    if (keyFrames[0].IsStopFrame())
    {
        curPathTime = float(keyFrames[0].Node->delay);
        keyFrames[0].DepartureTime = uint32(curPathTime * IN_MILLISECONDS);
    }

    for (size_t i = 1; i < keyFrames.size(); ++i)
    {
        curPathTime += keyFrames[i - 1].TimeTo;
        if (isSegmentEnd(keyFrames[i]))
        {
            keyFrames[i].ArriveTime = uint32(curPathTime * IN_MILLISECONDS);
            keyFrames[i - 1].NextArriveTime = keyFrames[i].ArriveTime;
            if (keyFrames[i].IsStopFrame())
                curPathTime += float(keyFrames[i].Node->delay);
            keyFrames[i].DepartureTime = uint32(curPathTime * IN_MILLISECONDS);
        }
        else
        {
            curPathTime -= keyFrames[i].TimeTo;
            keyFrames[i].ArriveTime = uint32(curPathTime * IN_MILLISECONDS);
            keyFrames[i - 1].NextArriveTime = keyFrames[i].ArriveTime;
            keyFrames[i].DepartureTime = keyFrames[i].ArriveTime;
        }
    }

    keyFrames.back().NextArriveTime = keyFrames.back().DepartureTime;
    // the client destroys a transport by itself after a while, refresh is needed mid course
    // Paths `117` and `122` are the Rut'theran Village to Auberdine and
    // Feathermoon Stronghold to Forgotten Coast ships, and `295` is the
    // Orgrimmar to Thunder Bluff zeppelin.
    if (pathId == 117 || pathId == 122 || pathId == 295)
        keyFrames[12].Update = true;
    // hack for custom boats:
    if ((pathId == 1500))
    {
        keyFrames[6].InitialOrientation = 2.3F;
        keyFrames[13].InitialOrientation = 2.2F;
    }
    transport->pathTime = keyFrames.back().DepartureTime;
}

Transport* TransportMgr::CreateTransport(uint32 entry, uint32 guid /*= 0*/)
{
    TransportTemplate const* tInfo = GetTransportTemplate(entry);
    if (!tInfo)
    {
        sLog.outErrorDb("Transport %u will not be loaded, `transport_template` missing", entry);
        return nullptr;
    }

    // create transport...
    Transport* trans = new Transport();

    // ...at first waypoint
    TaxiPathNodeEntry const* startNode = tInfo->keyFrames.begin()->Node;
    uint32 mapId = startNode->mapid;
    float x = startNode->x;
    float y = startNode->y;
    float z = startNode->z;
    float o = tInfo->keyFrames.begin()->InitialOrientation;

    /*
    // Debug log for transport:
    if (entry == 190550)
    {
        for (size_t i = 0; i < tInfo->keyFrames.size(); ++i)
        {
            sLog.outString("Map: %u | X: %f | Y: %f | O: %f | Teleport: %u | Update: %u | TimeTo: %u | TimeFrom: %u", 
                tInfo->keyFrames[i].Node->mapid, tInfo->keyFrames[i].Node->x, tInfo->keyFrames[i].Node->y, tInfo->keyFrames[i].InitialOrientation,
                tInfo->keyFrames[i].IsTeleportFrame(), tInfo->keyFrames[i].IsUpdateFrame(), tInfo->keyFrames[i].TimeTo, tInfo->keyFrames[i].TimeFrom);
        }
    }
    */

    // initialize the gameobject base
    // HIGHGUID_MO_TRANSPORT
    uint32 guidLow = guid ? guid : sObjectMgr.GenerateStaticGameObjectLowGuid();
    if (!trans->Create(guidLow, entry, mapId, x, y, z, o, 255))
    {
        delete trans;
        return nullptr;
    }

    if (MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(mapId))
    {
        if (mapEntry->Instanceable() != tInfo->inInstance)
        {
            sLog.outError("Transport %u (name: %s) attempted creation in instance map (id: %u) but it is not an instanced transport!", entry, trans->GetName(), mapId);
            delete trans;
            return nullptr;
        }
    }

    // Add transport to all continent instances.
    sMapMgr.GetOrCreateContinentInstances(mapId, trans, trans->m_maps);
    for (auto const& pMap : trans->m_maps)
    {
        trans->SetMap(pMap);
        trans->GetMap()->Add<Transport>(trans);
    }

    // set the instance at these coordinates as the main one
    uint32 newInstanceId = sMapMgr.GetContinentInstanceId(mapId, x, y);
    trans->SetLocationInstanceId(newInstanceId);
    Map* newMap = sMapMgr.CreateMap(mapId, trans);
    trans->SetMap(newMap);
    MANGOS_ASSERT(trans->m_maps.find(newMap) != trans->m_maps.end());
    
    return trans;
}

void TransportMgr::SpawnContinentTransports()
{
    if (_transportTemplates.empty())
        return;

    uint32 oldMSTime = WorldTimer::getMSTime();

    QueryResult* result = WorldDatabase.Query("SELECT guid, entry FROM transports");

    uint32 count = 0;
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();

            if (TransportTemplate const* tInfo = GetTransportTemplate(entry))
            {
                if (!tInfo->inInstance)
                {
                    if (Transport* pTransport = CreateTransport(entry, guid))
                    {
                        ++count;
                        m_shipTransports.insert(pTransport);
                    }
                }
            }
        }
        while (result->NextRow());
        delete result;
    }
}

void TransportMgr::Update(uint32 const diff)
{
    for (auto const& pTransport : m_shipTransports)
        pTransport->Update(diff, diff);
}
