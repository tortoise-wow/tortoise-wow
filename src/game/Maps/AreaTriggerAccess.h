#pragma once

class Player;
struct AreaTriggerTeleport;

enum class AreaTriggerTeleportAccess
{
    Allowed, InvalidDestination, Phase, Corpse, Level, Condition, Combat
};

// Read-only eligibility shared by native activation and route planning.
// May select the existing corpse-map entrance in trigger. Does not execute
// scripts, teleport, resurrect or send messages. Call on the player's owner.
AreaTriggerTeleportAccess CheckAreaTriggerTeleportAccess(Player* player, AreaTriggerTeleport const*& trigger);
