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

#include "Creature.h"
#include "MapManager.h"
#include "RandomMovementGenerator.h"
#include "Map.h"
#include "Util.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "DetailedWorkDiagnostics.h"
#include "ExecutionWatch.h"

void RandomMovementGenerator::_setRandomLocation(Creature &creature)
{
    // Failed destination selection must not retry an expensive terrain/navmesh
    // query every tick. Success below retains the normal ten-second interval.
    i_nextMoveTime.Reset(1000 + creature.GetGUIDLow() % 1000);
    // Don't move if invalid coordinates have been set somehow.
    if (i_positionX == 0.0f && i_positionY == 0.0f)
        return;

    if (creature.CanFly())
    {
        //typedef std::vector<Vector3> PointsArray;
        Movement::PointsArray path;
        uint32 ptsPerCycle = ceil(i_wanderDistance * 2);
        static const uint32 nbCyclesPerPacket = 1;
        for (uint32 i = 0; i <= nbCyclesPerPacket * ptsPerCycle; ++i)
            path.push_back(Vector3(i_positionX + i_wanderDistance * cos(i * 2 * M_PI / ptsPerCycle), i_positionY + i_wanderDistance * sin(i * 2 * M_PI / ptsPerCycle), i_positionZ));
        Movement::MoveSplineInit init(creature, "RandomMovementGenerator (CanFly)");
        init.SetFly();
        init.SetWalk(false);
        init.MovebyPath(path);
        init.SetFirstPointId(1);
        init.Launch();
        i_nextMoveTime.Reset(0);
        return;
    }

    float destX, destY, destZ;
    DetailedWork::Scope pointWork(DetailedWork::RandomPoint, creature.GetGUIDLow());
    ExecutionWatch::Scope pointWatch(ExecutionWatch::RandomDestination, creature.GetMapId(), creature.GetInstanceId(), creature.GetGUIDLow());
    if (!creature.GetRandomPoint(i_positionX, i_positionY, i_positionZ, i_wanderDistance, destX, destY, destZ))
        return;
    pointWork.Finish();
    pointWatch.Finish();

    DetailedWork::Scope launchWork(DetailedWork::RandomLaunch, creature.GetGUIDLow());
    creature.AddUnitState(UNIT_STAT_ROAMING_MOVE);
    Movement::MoveSplineInit init(creature, "RandomMovementGenerator");
    // Like the reference generator, retain path scratch storage for subsequent
    // wander requests. Reset topology/filter context every time: Turtle map
    // workers and tiles can change, and old polygon references must not survive.
    if (!i_path)
        i_path = std::make_unique<PathFinder>(&creature);
    i_path->ResetForNewRequest();
    i_path->ExcludeSteepSlopes();
    {
        DetailedWork::Scope pathWork(DetailedWork::RandomPath, creature.GetGUIDLow());
        i_path->calculate(destX, destY, destZ);
    }
    init.Move(i_path.get());
    init.SetWalk(true);
    init.Launch();

    i_nextMoveTime.Reset(10 * IN_MILLISECONDS);
}

void RandomMovementGenerator::Initialize(Creature &creature)
{
    if (!creature.IsAlive())
        return;

    creature.AddUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    i_nextMoveTime.Reset(50);
}

void RandomMovementGenerator::Reset(Creature &creature)
{
    Initialize(creature);
}

void RandomMovementGenerator::Interrupt(Creature &creature)
{
    creature.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    creature.SetWalk(!creature.HasUnitState(UNIT_STAT_RUNNING), false);
}

void RandomMovementGenerator::Finalize(Creature &creature)
{
    creature.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    creature.SetWalk(!creature.HasUnitState(UNIT_STAT_RUNNING), false);
}

bool RandomMovementGenerator::Update(Creature &creature, const uint32 &diff)
{
    if (i_expireTime)
    {
        if (i_expireTime <= diff)
            return false;
        else
            i_expireTime -= diff;
    }

    // Most random movers spend their time either following an active spline or
    // waiting for the next wander timer.  Scheduling the asynchronous half on
    // every map update made every such creature take asyncMovesplineLock and
    // enter the deferred-motion pipeline even though there was no work to do.
    //
    // Keep the inexpensive state/timer checks on the map owner thread and only
    // defer the operation which can actually be expensive: selecting and
    // launching a new random path.
    if (creature.HasUnitState(UNIT_STAT_CAN_NOT_MOVE | UNIT_STAT_DISTRACTED))
    {
        i_nextMoveTime.Reset(0);
        creature.ClearUnitState(UNIT_STAT_ROAMING_MOVE);
        return true;
    }

    if (creature.IsNoMovementSpellCasted())
    {
        if (!creature.IsStopped())
            creature.StopMoving();
        return true;
    }

    if (creature.movespline->Finalized())
    {
        i_nextMoveTime.Update(diff);
        if (i_nextMoveTime.Passed())
            creature.GetMotionMaster()->SetNeedAsyncUpdate();
    }

    return true;
}

void RandomMovementGenerator::UpdateAsync(Creature &creature, uint32 /*diff*/)
{
    // Lock async updates for safety, see Unit::asyncMovesplineLock doc
    DetailedWork::Scope lockWork(DetailedWork::RandomLock, creature.GetGUIDLow());
    ExecutionWatch::Scope lockWatch(ExecutionWatch::RandomMotionLock, creature.GetMapId(), creature.GetInstanceId(), creature.GetGUIDLow());
    std::unique_lock<std::mutex> guard(creature.asyncMovesplineLock);
    lockWork.Finish();
    lockWatch.Finish();
    // Revalidate after dispatch because threaded motion may run later in the
    // same map update.  The timer is advanced by Update(); doing it again here
    // would double-count elapsed time when motion workers are enabled.
    if (!creature.HasUnitState(UNIT_STAT_CAN_NOT_MOVE | UNIT_STAT_DISTRACTED) &&
        !creature.IsNoMovementSpellCasted() && creature.movespline->Finalized() &&
        i_nextMoveTime.Passed())
        _setRandomLocation(creature);
}

bool RandomMovementGenerator::GetResetPosition(Creature& c, float& x, float& y, float& z)
{
    // use current if in range
    if (c.IsWithinDist2d(i_positionX, i_positionY, i_wanderDistance))
        c.GetPosition(x, y, z);
    else
    {
        x = i_positionX;
        y = i_positionY;
        z = i_positionZ;
    }

    return true;
}
