#include "AreaTriggerAccess.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Conditions.h"
#include "Corpse.h"
#include "Map.h"

AreaTriggerTeleportAccess CheckAreaTriggerTeleportAccess(Player* player, AreaTriggerTeleport const*& trigger)
{
    if (!player || !trigger || !player->FindMap())
        return AreaTriggerTeleportAccess::InvalidDestination;
    MapEntry const* targetMap = sMapStorage.LookupEntry<MapEntry>(trigger->destination.mapId);
    if (!targetMap)
        return AreaTriggerTeleportAccess::InvalidDestination;
    if (trigger->requiredPhase > sWorld.GetContentPhase())
        return AreaTriggerTeleportAccess::Phase;

    if (!player->IsAlive() && targetMap->IsDungeon())
    {
        uint32 corpseMapId = 0;
        if (Corpse* corpse = player->GetCorpse())
            corpseMapId = corpse->GetMapId();
        uint32 instanceMap = corpseMapId;
        do
        {
            if (instanceMap == targetMap->id)
                break;
            MapEntry const* instance = sMapStorage.LookupEntry<MapEntry>(instanceMap);
            instanceMap = instance && instance->IsDungeon() ? instance->parent : 0;
        } while (instanceMap);
        if (!instanceMap)
            return AreaTriggerTeleportAccess::Corpse;
        if (trigger->destination.mapId != corpseMapId)
            if (AreaTriggerTeleport const* corpseAt = sObjectMgr.GetMapEntranceTrigger(corpseMapId))
                trigger = corpseAt;
    }

    if (!player->IsGameMaster())
    {
        const bool levelCheck = player->GetLevel() < trigger->requiredLevel && !sWorld.getConfig(CONFIG_BOOL_INSTANCE_IGNORE_LEVEL);
        static constexpr uint32 allowedLunaticMaps[] = {36, 43, 389, 822};
        bool lunaticMap = false;
        for (uint32 mapId : allowedLunaticMaps)
            if (targetMap->id == mapId) { lunaticMap = true; break; }
        const bool blockedByLevel = levelCheck && !(lunaticMap && player->HasChallenge(CHALLENGE_LUNATIC));
        const bool blockedByCondition = trigger->requiredCondition &&
            !IsConditionSatisfied(trigger->requiredCondition, player, player->GetMap(), player, CONDITION_FROM_AREATRIGGER);
        if (blockedByLevel)
            return AreaTriggerTeleportAccess::Level;
        if (blockedByCondition)
            return AreaTriggerTeleportAccess::Condition;
        if (player->IsInCombat() && targetMap->IsContinent() && player->GetMap()->IsRaid())
            return AreaTriggerTeleportAccess::Combat;
    }
    return AreaTriggerTeleportAccess::Allowed;
}
