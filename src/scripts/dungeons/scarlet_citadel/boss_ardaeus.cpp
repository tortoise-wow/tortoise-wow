/*
 * Copyright (C) 2021-2022 Nolin (nolin.nolin.nolin.nolin@gmail.org)
 *
 * This is private software and may not be shared under any circumstances,
 * absent permission of Nolin.
 */

#include "scriptPCH.h"
#include "scarlet_citadel.h"
#include "boss_ardaeus.hpp"


/*
    TODO-List
    - Power Focus:
    Ardaeus is channeling his magical power which summons an object similar to a sun which is moving slowly down from the roof but increases it speed from time to time.
    The players have to do heavy damage on his "sun" to make it go back upwards. If the "sun" is touching the floor, the raid will wipe.

    - Calling for help:
    Ardaeus is calling for help which creates and echo of 11 possible creatures (statues) which have their own abilities.
    Once the player killed an echo, the boss will become attackable until the next "Calling for help."
    
    - Decide achievement (don't let sun move below a certain Z axis?)

    - .. and a lot more
*/

class boss_ardaeusAI : public ScriptedAI
{
public:
    explicit boss_ardaeusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiSunGuid = 0;
        m_pInstance = static_cast<instance_scarlet_citadel*>(pCreature->GetInstanceData());
        boss_ardaeusAI::Reset();
    }

private:

    bool m_bAchievementKillFailed{};

    uint32 m_uiCallForHelp_Timer{};

    ObjectGuid m_uiSunGuid{};

    std::list<ObjectGuid>m_lSummonedCallForHelpNPCs;

    instance_scarlet_citadel* m_pInstance{};

public:
    void Reset() override
    {
        // Call for Help
        m_uiCallForHelp_Timer = 5000; //nsArdaeus::CALLFORHELP_REPEAT_TIMER;
        m_lSummonedCallForHelpNPCs.clear();

        // Achievement
        m_bAchievementKillFailed = false;

        // Trigger fight on gossip
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetFactionTemplateId(nsArdaeus::FACTION_NEUTRAL);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        if (!m_pInstance)
            return;

        m_creature->SetInCombatWithZone();

        m_pInstance->SetData(ScarletCitadelEncounter::TYPE_ARDAEUS, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (!m_pInstance)
            return;
        
        DespawnSun();
        DespawnCallForHelpNPCs();

        m_creature->MonsterSay(nsArdaeus::CombatNotification(nsArdaeus::CombatNotifications::RAIDWIPE), LANG_UNIVERSAL);

        m_pInstance->SetData(ScarletCitadelEncounter::TYPE_ARDAEUS, FAIL);
    }

    void JustDied(Unit* pKiller) override
    {
        if (!m_pInstance)
            return;
    
        DespawnSun();
        DespawnCallForHelpNPCs();

        if (!IsAchievementKillFailed())
        {
            SpawnAchievementReward(pKiller);
        }

        m_creature->MonsterSay(nsArdaeus::CombatNotification(nsArdaeus::CombatNotifications::BOSSDIED), LANG_UNIVERSAL);

        m_creature->SetRespawnDelay(nsArdaeus::SEVEN_DAYS);

        m_pInstance->SetData(ScarletCitadelEncounter::TYPE_ARDAEUS, DONE);


        if (Creature* pEricDark{ m_pInstance->GetSingleCreatureFromStorage(NPC_ERIC_VESPER) })
        {
            pEricDark->SetRespawnDelay(604800); // If Boss Ardaeus is dead, set Eric Dark's respawntimer to 7 days
        }
    }

    void SpawnSun()
    {
        if (Creature const* pSun{ m_creature->SummonCreature(nsArdaeus::NPC_SUN,
            nsArdaeus::vfSunMovePoints[0].m_fX,
            nsArdaeus::vfSunMovePoints[0].m_fY,
            nsArdaeus::vfSunMovePoints[0].m_fZ,
            nsArdaeus::vfSunMovePoints[0].m_fO,
            TEMPSUMMON_MANUAL_DESPAWN) })
        {
            m_uiSunGuid = pSun->GetObjectGuid();
        }
    }

    void DespawnSun()
    {
        if (IsSunSpawned())
        {
            if (const auto map{ m_creature->GetMap() })
            {
                if (Creature* pCreature{ map->GetCreature(m_uiSunGuid) })
                {
                    if (TemporarySummon* tmpSumm{ static_cast<TemporarySummon*>(pCreature) })
                    {
                        tmpSumm->UnSummon();
                        m_uiSunGuid = 0;
                    }
                }
            }
        }
        else
        {
            sLog.outError("[SC] Boss Ardaeus: DespawnSun() called but no GUID assigned!");
        }
    }

    void CallForHelp(const uint32& uiDiff)
    {
        if (m_uiCallForHelp_Timer < uiDiff)
        {
            const auto uiRnd{ urand(0, nsArdaeus::MAX_SPAWN_POINTS - 1) }; // Pick a random NPC (urand includes its upper bound)
            ObjectGuid uiStatueNpcGUID{};

            // Summon an invisible NPC in front of the random chosen statue which does a visual effect to the summoned creature
            Creature* pStatueNPC{ m_creature->SummonCreature(nsArdaeus::ARDAEUS_STATUE_NPC,
                nsArdaeus::vfStatueNPCsSpawnPoints[uiRnd].m_fX,
                nsArdaeus::vfStatueNPCsSpawnPoints[uiRnd].m_fY,
                nsArdaeus::vfStatueNPCsSpawnPoints[uiRnd].m_fZ,
                nsArdaeus::vfStatueNPCsSpawnPoints[uiRnd].m_fO,
                TEMPSUMMON_MANUAL_DESPAWN) };

            if (pStatueNPC)
            {
                uiStatueNpcGUID = pStatueNPC->GetObjectGuid(); // Store its GUID to remove it later
            }

            // Summon Ardaeus' guard
            if (Creature* pRandomNPC{ m_creature->SummonCreature(nsArdaeus::vfCallForHelpNPCs[uiRnd],
                nsArdaeus::vfCallForHelpSpawnPoint[0].m_fX,
                nsArdaeus::vfCallForHelpSpawnPoint[0].m_fY,
                nsArdaeus::vfCallForHelpSpawnPoint[0].m_fZ,
                nsArdaeus::vfCallForHelpSpawnPoint[0].m_fO,
                TEMPSUMMON_MANUAL_DESPAWN) })
            {
                m_lSummonedCallForHelpNPCs.push_back(pRandomNPC->GetObjectGuid()); // Store its GUID to remove it later

                if (pStatueNPC)
                {
                    pRandomNPC->CastSpell(pStatueNPC, nsArdaeus::VISUALSPELL_SUMMON_CALLFORHELP, true);
                }
            }

            // Despawn Statue NPC
            if (const auto map{ m_creature->GetMap() })
            {
                if (uiStatueNpcGUID)
                {
                    if (Creature* pCreature{ map->GetCreature(uiStatueNpcGUID) })
                    {
                        if (TemporarySummon* tmpSumm{ static_cast<TemporarySummon*>(pCreature) })
                        {
                            tmpSumm->UnSummon();
                        }
                    }
                }

                uiStatueNpcGUID = 0;
            }

            m_uiCallForHelp_Timer = nsArdaeus::CALLFORHELP_REPEAT_TIMER;
        }
        else
        {
            m_uiCallForHelp_Timer -= uiDiff;
        }
    }

    void DespawnCallForHelpNPCs()
    {
        if (!m_lSummonedCallForHelpNPCs.empty())
        {
            if (const auto map{ m_creature->GetMap() })
            {
                for (const auto& guid : m_lSummonedCallForHelpNPCs)
                {
                    if (Creature* pCreature{ map->GetCreature(guid) })
                    {
                        if (TemporarySummon* tmpSumm{ static_cast<TemporarySummon*>(pCreature) })
                        {
                            tmpSumm->UnSummon();
                        }
                    }
                }

                m_lSummonedCallForHelpNPCs.clear();
            }
        }
    }

    void SpawnAchievementReward(Unit* pKiller)
    {
        if (pKiller)
        {
            pKiller->SummonGameObject(nsArdaeus::GO_ACHIEVEMENT_CHEST,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fX,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fY,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fZ,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fO,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fR0,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fR1,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fR2,
                nsArdaeus::vfAchievementChestSpawnPoint[0].m_fR3,
                nsArdaeus::GO_ACHIEVEMENT_CHEST_DESPAWN_TIMER);
        }
        else
        {
            sLog.outError("[SC] Boss Ardaeus: SpawnAchievementReward() called but no pKiller found!");
        }
    }

    void AchievementKillFailed()
    {
        m_creature->HandleEmote(EMOTE_ONESHOT_LAUGH);
        m_creature->MonsterSay(nsArdaeus::CombatNotification(nsArdaeus::CombatNotifications::ACHIEVEMENT_FAILED), LANG_UNIVERSAL);

        m_bAchievementKillFailed = true;
    }

    bool IsAchievementKillFailed()
    {
        return m_bAchievementKillFailed;
    }

    bool IsSunSpawned()
    {
        return static_cast<bool>(m_uiSunGuid);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        CallForHelp(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ardaeus(Creature* pCreature)
{
    return new boss_ardaeusAI(pCreature);
}


class npc_sunAI : public ScriptedAI
{
public:
    explicit npc_sunAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = static_cast<instance_scarlet_citadel*>(pCreature->GetInstanceData());
        npc_sunAI::Reset();
    }

private:

    float m_fUpwardSpeed{}; // Do we ever want to change this value? Move to constexpr if not
    float m_fDownwardSpeed{};
    float m_fNewPositionZ{};

    uint32 m_uiDamageDone{};
    uint32 m_uiIncreaseSpeed_Timer{};
    uint32 m_uiAchievement_Timer{};

    instance_scarlet_citadel* m_pInstance{};

public:
    void Reset() override
    {
        m_uiDamageDone = 0;
        m_uiIncreaseSpeed_Timer = nsArdaeus::SUN_SPEED_INCREASE_TIMER;
        m_uiAchievement_Timer = nsArdaeus::ACHIEVEMENT_CHECK_TIMER;

        m_fUpwardSpeed = 0.5f;
        m_fDownwardSpeed = 0.5f;
        m_fNewPositionZ = 0.f;

        SetCombatMovement(false);

        m_creature->GetMotionMaster()->Clear();
        m_creature->SetSpeedRate(MOVE_RUN, .1f);
        m_creature->CastSpell(m_creature, 17131, true);
        m_creature->SetFly(true);
        m_creature->SetLevitate(true);
    }

    void MoveDownwards()
    {
        m_creature->GetMotionMaster()->Clear();

        m_creature->GetMotionMaster()->MovePoint(0,
            nsArdaeus::vfSunMovePoints[1].m_fX,
            nsArdaeus::vfSunMovePoints[1].m_fY,
            nsArdaeus::vfSunMovePoints[1].m_fZ,
            (MOVE_FLY_MODE), m_fDownwardSpeed);
    }

    void MoveUpwards()
    {
        m_creature->GetMotionMaster()->Clear();

        m_fNewPositionZ = (m_creature->GetPositionZ() + nsArdaeus::INCREASE_Z_AXIS);
        m_creature->GetMotionMaster()->MovePoint(0,
            nsArdaeus::vfSunMovePoints[0].m_fX,
            nsArdaeus::vfSunMovePoints[0].m_fY,
            m_fNewPositionZ,
            (MOVE_FLY_MODE), m_fUpwardSpeed);

        m_uiDamageDone = 0;
    }

    void UpdateSpeed(const uint32& uiDiff)
    {
        if (m_uiIncreaseSpeed_Timer < uiDiff)
        {
            m_fDownwardSpeed = nsArdaeus::INCREASE_SPEED;
            m_creature->UpdateSpeed(MOVE_RUN, m_fDownwardSpeed);

            m_uiIncreaseSpeed_Timer = nsArdaeus::SUN_SPEED_INCREASE_TIMER;
        }
        else
        {
            m_uiIncreaseSpeed_Timer -= uiDiff;
        }
    }

    void CheckForAchievement(const uint32& uiDiff)
    {
        if (m_uiAchievement_Timer < uiDiff)
        {
            if (m_creature->GetPositionZ() < nsArdaeus::ACHIEVEMENT_FAILED_BELOW)
            {
                if (Creature* pCreature{ m_pInstance->GetSingleCreatureFromStorage(NPC_ARDAEUS) })
                {
                    if (boss_ardaeusAI* boss_ardaeus{ dynamic_cast<boss_ardaeusAI*>(pCreature->AI()) })
                    {
                        if (!boss_ardaeus->IsAchievementKillFailed())
                        {
                            boss_ardaeus->AchievementKillFailed();
                        }
                    }
                }
            }

            m_uiAchievement_Timer = nsArdaeus::SUN_SPEED_INCREASE_TIMER;
        }
        else
        {
            m_uiAchievement_Timer -= uiDiff;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (Creature* pCreature{ m_pInstance->GetSingleCreatureFromStorage(NPC_ARDAEUS) })
        {
            if (boss_ardaeusAI* boss_ardaeus{ dynamic_cast<boss_ardaeusAI*>(pCreature->AI()) })
            {
                if (!boss_ardaeus->IsSunSpawned())
                {
                    return;
                }
            }
        }

        UpdateSpeed(uiDiff);
        CheckForAchievement(uiDiff);

        if (m_uiDamageDone > nsArdaeus::DAMAGE_DONE_TO_MOVE_UPWARDS &&
            ((m_creature->GetPositionZ() + nsArdaeus::INCREASE_Z_AXIS) < nsArdaeus::vfSunMovePoints[0].m_fZ)) // Prevent to shoot the Sun through the roof
        {
            MoveUpwards();
        }

        if (m_creature->GetPositionZ() > (m_fNewPositionZ - 0.05f)) // Add a small leeway as we work with float
        {
            MoveDownwards();
        }
    }
};

CreatureAI* GetAI_npc_sunAI(Creature* pCreature)
{
    return new npc_sunAI(pCreature);
}


bool GossipHello_boss_ardaeus(Player* pPlayer, Creature* pCreature)
{
    instance_scarlet_citadel const* m_pInstance{ static_cast<instance_scarlet_citadel*>(pCreature->GetInstanceData()) };

    if (m_pInstance /*&& (m_pInstance->GetData(TYPE_DAELUS) == DONE)*/) // TODO: Remove comment after testing
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, nsArdaeus::GOSSIP_ANSWER, GOSSIP_SENDER_MAIN, (GOSSIP_ACTION_INFO_DEF + 1));
        pPlayer->SEND_GOSSIP_MENU(nsArdaeus::GOSSIP_TEXT, pCreature->GetObjectGuid());
    }
    else
    {
        sLog.outError("[SC] Boss Ardeus: Boss spawned outside of dungeon or someone tried to start encounter without killing the first boss!");
        pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

bool GossipSelect_boss_ardaeus(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, const uint32 uiAction)
{
    if (!pPlayer || !pCreature)
        return false;

    switch (uiAction)
    {
        case (GOSSIP_ACTION_INFO_DEF + 1):
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            try
            {
                DoAfterTime(pCreature, (3 * IN_MILLISECONDS), [creature = pCreature]()
                    {
                        creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        creature->MonsterSay(nsArdaeus::CombatNotification(nsArdaeus::CombatNotifications::ABOUT_TO_START), LANG_UNIVERSAL);
                    });

                DoAfterTime(pCreature, (7 * IN_MILLISECONDS), [creature = pCreature]()
                    {
                        creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                        creature->MonsterYell(nsArdaeus::CombatNotification(nsArdaeus::CombatNotifications::START), LANG_UNIVERSAL);
                        if (boss_ardaeusAI* boss_ardaeus{ dynamic_cast<boss_ardaeusAI*>(creature->AI()) })
                        {
                            boss_ardaeus->SpawnSun();
                        }
                    });

                DoAfterTime(pCreature, (10 * IN_MILLISECONDS), [creature = pCreature]()
                    {
                        creature->SetFactionTemplateId(nsArdaeus::FACTION_SCARLET);
                        creature->SetInCombatWithZone();
                        if (npc_sunAI* npc_sun{ dynamic_cast<npc_sunAI*>(creature->AI()) })
                        {
                            npc_sun->MoveDownwards();
                        }
                    });
            }
            catch (const std::runtime_error& e)
            {
                sLog.outError("[SC] Boss Ardaeus: DoAfterTime() failed: %s", e.what());
            }

            break;
        }
    }

    return true;
}

struct npc_invar_onearmAI : public ScriptedAI
{
    explicit npc_invar_onearmAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_invar_onearmAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_invar_onearmAI(Creature* pCreature)
{
    return new npc_invar_onearmAI(pCreature);
}


struct npc_arellas_fireleafAI : public ScriptedAI
{
    explicit npc_arellas_fireleafAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_arellas_fireleafAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_arellas_fireleafAI(Creature* pCreature)
{
    return new npc_arellas_fireleafAI(pCreature);
}


struct npc_holia_sunshieldAI : public ScriptedAI
{
    explicit npc_holia_sunshieldAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_holia_sunshieldAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_holia_sunshieldAI(Creature* pCreature)
{
    return new npc_holia_sunshieldAI(pCreature);
}


struct npc_ferren_marcusAI : public ScriptedAI
{
    explicit npc_ferren_marcusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_ferren_marcusAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_ferren_marcusAI(Creature* pCreature)
{
    return new npc_ferren_marcusAI(pCreature);
}


struct npc_yana_bloodspearAI : public ScriptedAI
{
    explicit npc_yana_bloodspearAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_yana_bloodspearAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_yana_bloodspearAI(Creature* pCreature)
{
    return new npc_yana_bloodspearAI(pCreature);
}


struct npc_orman_stromgardeAI : public ScriptedAI
{
    explicit npc_orman_stromgardeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_orman_stromgardeAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_orman_stromgardeAI(Creature* pCreature)
{
    return new npc_orman_stromgardeAI(pCreature);
}


struct npc_fellari_swiftarrowAI : public ScriptedAI
{
    explicit npc_fellari_swiftarrowAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_fellari_swiftarrowAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_fellari_swiftarrowAI(Creature* pCreature)
{
    return new npc_fellari_swiftarrowAI(pCreature);
}


struct npc_dorgar_stoenbrowAI : public ScriptedAI
{
    explicit npc_dorgar_stoenbrowAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_dorgar_stoenbrowAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_dorgar_stoenbrowAI(Creature* pCreature)
{
    return new npc_dorgar_stoenbrowAI(pCreature);
}


struct npc_valea_twinbladesAI : public ScriptedAI
{
    explicit npc_valea_twinbladesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_valea_twinbladesAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_valea_twinbladesAI(Creature* pCreature)
{
    return new npc_valea_twinbladesAI(pCreature);
}


struct npc_harthal_truesightAI : public ScriptedAI
{
    explicit npc_harthal_truesightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_harthal_truesightAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_harthal_truesightAI(Creature* pCreature)
{
    return new npc_harthal_truesightAI(pCreature);
}


struct npc_admiral_barean_westwindAI : public ScriptedAI
{
    explicit npc_admiral_barean_westwindAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        npc_admiral_barean_westwindAI::Reset();
    }

    void Reset() override
    {

    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;
    }
};

CreatureAI* GetAI_npc_admiral_barean_westwindAI(Creature* pCreature)
{
    return new npc_admiral_barean_westwindAI(pCreature);
}


void AddSC_boss_ardaeus()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ardaeus";
    pNewscript->GetAI = &GetAI_boss_ardaeus;
    pNewscript->pGossipHello = &GossipHello_boss_ardaeus;
    pNewscript->pGossipSelect = &GossipSelect_boss_ardaeus;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_sun";
    pNewscript->GetAI = &GetAI_npc_sunAI;
    pNewscript->RegisterSelf();

    // Adds
    pNewscript = new Script;
    pNewscript->Name = "npc_invar_onearm";
    pNewscript->GetAI = &GetAI_npc_invar_onearmAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_arellas_fireleaf";
    pNewscript->GetAI = &GetAI_npc_arellas_fireleafAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_holia_sunshield";
    pNewscript->GetAI = &GetAI_npc_holia_sunshieldAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_ferren_marcus";
    pNewscript->GetAI = &GetAI_npc_ferren_marcusAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_yana_bloodspear";
    pNewscript->GetAI = &GetAI_npc_yana_bloodspearAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_orman_stromgarde";
    pNewscript->GetAI = &GetAI_npc_orman_stromgardeAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_fellari_swiftarrow";
    pNewscript->GetAI = &GetAI_npc_fellari_swiftarrowAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_dorgar_stoenbrow";
    pNewscript->GetAI = &GetAI_npc_dorgar_stoenbrowAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_valea_twinblades";
    pNewscript->GetAI = &GetAI_npc_valea_twinbladesAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_harthal_truesight";
    pNewscript->GetAI = &GetAI_npc_harthal_truesightAI;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_admiral_barean_westwind";
    pNewscript->GetAI = &GetAI_npc_admiral_barean_westwindAI;
    pNewscript->RegisterSelf();
}
