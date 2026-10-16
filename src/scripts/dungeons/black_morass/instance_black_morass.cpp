/*
 * Copyright (C) 2021-2022 Nolin (nolin.nolin.nolin.nolin@gmail.org)
 *
 * This is private software and may not be shared under any circumstances,
 * absent permission of Nolin.
 */

#include "scriptPCH.h"
#include "black_morass.h"
#include <algorithm>


instance_black_morass::instance_black_morass(Map* pMap) : ScriptedInstance(pMap)
{
    instance_black_morass::Initialize();
}

void instance_black_morass::Initialize()
{
    // A new instance runs Initialize twice (this constructor, then Map::CreateInstanceData),
    // a loaded one only once (here): summon the Entity once either way.
    if (!unknownEntity.IsEmpty())
        return;

    if (Creature* entityCreature{ instance->SummonCreature(66003, -1591.57f, 7106.07f, 23.73f, 0, TEMPSUMMON_CORPSE_DESPAWN) })
    {
        unknownEntity = entityCreature->GetObjectGuid();
    }
}

void instance_black_morass::OnPlayerEnter(Player* pPlayer)
{
    if (pPlayer)
    {
        pPlayer->PlayDirectMusic(30274, pPlayer);
    }
}

void instance_black_morass::Update(uint32 uiDiff)
{
    Map::PlayerList const& playerList = GetMap()->GetPlayers();
    for (auto const& itr : playerList)
    {
        if (Player* player = itr.getSource())
        {
            if (!player->IsBeingTeleported() && player->GetPositionZ() < -281.f)
            {
                // Player fell out of bounds - teleport to entrance.
                player->TeleportTo(269, -2002.500000f, 6575.299805f, -154.938095f, 5.700000f);
            }
        }
    }
}

InstanceData* GetInstanceData_instance_black_morass(Map* pMap)
{
    return new instance_black_morass(pMap);
}


void AddSC_instance_black_morass()
{
    Script* pNewscript{ new Script };
    pNewscript->Name = "instance_black_morass";
    pNewscript->GetInstanceData = &GetInstanceData_instance_black_morass;
    pNewscript->RegisterSelf();
}
