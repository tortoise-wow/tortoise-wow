/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Boss_Axelus
SD%Complete: 75
SDComment: Broodcommander Axelus encounter in Onyxia's Lair.
SDCategory: Onyxia's Lair
EndScriptData
*/

#include "scriptPCH.h"

#include <list>
#include <vector>

enum
{
    SAY_AXELUS_AGGRO = 4901801,
    SAY_AXELUS_QUAKING_STOMP = 4901802,
    SAY_AXELUS_SEARING_CHAINS = 4901803,
    SAY_AXELUS_THROW_GLAIVE = 4901804,
    SAY_AXELUS_BANNER_OF_COMMAND = 4901805,
    SAY_AXELUS_DEATH = 4901806,

    SPELL_AXELUS_QUAKING_STOMP = 36883,
    SPELL_AXELUS_SEARING_CHAINS = 41537,
    SPELL_AXELUS_THROW_GLAIVE = 47403,
    SPELL_AXELUS_COMMANDING_SHOUT = 47404,
    SPELL_AXELUS_SEARING_CHAINS_VISUAL = 47405,
    SPELL_AXELUS_SEARING_CHAINS_AURA = 47406,
    SPELL_AXELUS_SEARED_SCALES = 47407,
    SPELL_AXELUS_BANNER_OF_COMMAND = 47408,
    SPELL_AXELUS_CONFLAGRATE = 47409,

    NPC_AXELUS_ONYXIAN_WARDER = 40068,
};

struct AxelusWarderSpawnPoint
{
    float x;
    float y;
    float z;
};

static AxelusWarderSpawnPoint const aAxelusWarderSpawnArea[] =
{
    { -145.547684f,  -66.676308f, -67.766953f },
    { -141.855316f,  -79.578644f, -67.828087f },
    { -148.508057f,  -94.943718f, -67.849464f },
    { -166.236832f, -110.796234f, -67.745415f },
    { -183.326996f, -107.137802f, -67.514931f },
    { -190.623413f, -105.073563f, -67.761330f },
    { -199.040344f,  -93.546791f, -67.997375f },
    { -200.901123f,  -84.949928f, -67.792381f },
    { -200.090897f,  -72.266724f, -67.359657f },
    { -193.654831f,  -54.372437f, -67.738411f },
    { -182.009186f,  -48.023052f, -67.876526f },
    { -170.366592f,  -47.452789f, -67.301476f },
    { -153.490448f,  -53.985733f, -67.378304f }
};

static bool IsPointInsideAxelusWarderSpawnArea(float x, float y)
{
    bool inside = false;
    uint32 const pointCount = sizeof(aAxelusWarderSpawnArea) / sizeof(aAxelusWarderSpawnArea[0]);

    for (uint32 i = 0, j = pointCount - 1; i < pointCount; j = i++)
    {
        AxelusWarderSpawnPoint const& first = aAxelusWarderSpawnArea[i];
        AxelusWarderSpawnPoint const& second = aAxelusWarderSpawnArea[j];

        if ((first.y > y) != (second.y > y) &&
            x < (second.x - first.x) * (y - first.y) / (second.y - first.y) + first.x)
            inside = !inside;
    }

    return inside;
}

static void SelectRandomAxelusWarderSpawnPosition(WorldObject const* reference, float& x, float& y, float& z)
{
    static float const minX = -200.901123f;
    static float const maxX = -141.855316f;
    static float const minY = -110.796234f;
    static float const maxY =  -47.452789f;

    for (uint8 i = 0; i < 64; ++i)
    {
        float const candidateX = minX + rand_norm_f() * (maxX - minX);
        float const candidateY = minY + rand_norm_f() * (maxY - minY);
        if (!IsPointInsideAxelusWarderSpawnArea(candidateX, candidateY))
            continue;

        x = candidateX;
        y = candidateY;
        z = -67.75f;
        reference->UpdateGroundPositionZ(x, y, z);
        return;
    }

    x = -176.0f;
    y = -84.0f;
    z = -67.75f;
    reference->UpdateGroundPositionZ(x, y, z);
}

static Creature* SummonAxelusWarder(Creature* axelus)
{
    if (!axelus)
        return nullptr;

    float x;
    float y;
    float z;
    SelectRandomAxelusWarderSpawnPosition(axelus, x, y, z);

    Creature* warder = axelus->SummonCreature(NPC_AXELUS_ONYXIAN_WARDER, x, y, z, frand(0.0f, M_PI_F * 2.0f), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
    if (!warder)
        return nullptr;

    warder->SetInCombatWithZone();
    return warder;
}

static bool IsAxelusBetweenChainedPlayers(Unit const* axelus, Unit const* first, Unit const* second)
{
    float const firstX = first->GetPositionX();
    float const firstY = first->GetPositionY();
    float const secondX = second->GetPositionX();
    float const secondY = second->GetPositionY();
    float const axelusX = axelus->GetPositionX();
    float const axelusY = axelus->GetPositionY();

    float const deltaX = secondX - firstX;
    float const deltaY = secondY - firstY;
    float const lengthSq = deltaX * deltaX + deltaY * deltaY;
    if (lengthSq <= 0.001f)
        return false;

    float const projection = ((axelusX - firstX) * deltaX + (axelusY - firstY) * deltaY) / lengthSq;
    if (projection < 0.0f || projection > 1.0f)
        return false;

    float const closestX = firstX + projection * deltaX;
    float const closestY = firstY + projection * deltaY;
    float const distanceX = axelusX - closestX;
    float const distanceY = axelusY - closestY;

    float hitboxRadius = axelus->GetCombatReach();
    if (axelus->GetObjectBoundingRadius() > hitboxRadius)
        hitboxRadius = axelus->GetObjectBoundingRadius();

    return distanceX * distanceX + distanceY * distanceY <= hitboxRadius * hitboxRadius;
}

struct spell_axelus_searing_chains : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit || !spell->m_casterUnit->GetMap())
            return false;

        Unit* caster = spell->m_casterUnit;
        std::vector<Player*> targets;
        std::vector<Player*> chainedTargets;

        Map::PlayerList const& players = caster->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = itr->getSource();
            if (!player || !player->IsAlive() || !player->IsTargetable(true, false))
                continue;

            if (!caster->IsHostileTo(player) || !caster->IsWithinDistInMap(player, 60.0f) || !caster->IsWithinLOSInMap(player))
                continue;

            if (player->HasAura(SPELL_AXELUS_SEARING_CHAINS_AURA))
                continue;

            if (caster->GetVictim() == player)
                continue;

            targets.push_back(player);
        }

        for (uint8 i = 0; i < 2 && !targets.empty(); ++i)
        {
            uint32 index = urand(0, uint32(targets.size() - 1));
            Player* target = targets[index];
            caster->CastSpell(target, SPELL_AXELUS_SEARING_CHAINS_AURA, true);
            chainedTargets.push_back(target);
            targets.erase(targets.begin() + index);
        }

        if (chainedTargets.size() == 2)
        {
            chainedTargets[0]->CastSpell(chainedTargets[1], SPELL_AXELUS_SEARING_CHAINS_VISUAL, true);
            chainedTargets[1]->CastSpell(chainedTargets[0], SPELL_AXELUS_SEARING_CHAINS_VISUAL, true);
        }

        return false;
    }
};

SpellScript* GetSpellScript_axelus_searing_chains(SpellEntry const*)
{
    return new spell_axelus_searing_chains;
}

struct spell_axelus_searing_chains_aura : public AuraScript
{
    void OnAfterApply(Aura* aura, bool apply) override
    {
        if (apply || aura->GetEffIndex() != EFFECT_INDEX_0 || aura->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        Unit* target = aura->GetTarget();
        Unit* caster = aura->GetCaster();
        if (!target || !target->IsAlive() || !caster || !caster->IsAlive())
            return;

        caster->CastSpell(target, SPELL_AXELUS_CONFLAGRATE, true);
        target->RemoveAurasDueToSpell(SPELL_AXELUS_SEARING_CHAINS_VISUAL);
    }

    void OnPeriodicDamageAfterDeal(Aura* aura, uint32 /*damage*/, CleanDamage const* /*cleanDamage*/) override
    {
        Unit* target = aura->GetTarget();
        Unit* caster = aura->GetCaster();
        if (!target || !target->GetMap())
            return;

        Player* chainedPartner = nullptr;
        ObjectGuid const casterGuid = aura->GetCasterGuid();
        Map::PlayerList const& players = target->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = itr->getSource();
            if (!player || player->GetObjectGuid() == target->GetObjectGuid() || !player->IsAlive())
                continue;

            if (player->GetSpellAuraHolder(SPELL_AXELUS_SEARING_CHAINS_AURA, casterGuid))
            {
                chainedPartner = player;
                break;
            }
        }

        if (!chainedPartner || (caster && caster->IsAlive() && !IsAxelusBetweenChainedPlayers(caster, target, chainedPartner)))
            return;

        ObjectGuid const partnerGuid = chainedPartner->GetObjectGuid();
        target->m_Events.AddLambdaEventAtOffset([target, partnerGuid, casterGuid]()
        {
            if (!target->IsInWorld() || !target->GetSpellAuraHolder(SPELL_AXELUS_SEARING_CHAINS_AURA, casterGuid))
                return;

            Unit* partner = ObjectAccessor::GetUnit(*target, partnerGuid);
            if (!partner || !partner->GetSpellAuraHolder(SPELL_AXELUS_SEARING_CHAINS_AURA, casterGuid))
                return;

            if (Unit* caster = ObjectAccessor::GetUnit(*target, casterGuid))
            {
                if (caster->IsAlive())
                    caster->CastSpell(caster, SPELL_AXELUS_SEARED_SCALES, true);
            }

            target->RemoveAurasByCasterSpell(SPELL_AXELUS_SEARING_CHAINS_AURA, casterGuid);
            target->RemoveAurasDueToSpell(SPELL_AXELUS_SEARING_CHAINS_VISUAL);
            partner->RemoveAurasByCasterSpell(SPELL_AXELUS_SEARING_CHAINS_AURA, casterGuid);
            partner->RemoveAurasDueToSpell(SPELL_AXELUS_SEARING_CHAINS_VISUAL);
        }, 1);
    }
};

AuraScript* GetAuraScript_axelus_searing_chains_aura(SpellEntry const*)
{
    return new spell_axelus_searing_chains_aura;
}

struct broodcommander_axelusAI : public ScriptedAI
{
    broodcommander_axelusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiSearingChainsTimer;
    uint32 m_uiQuakingStompTimer;
    uint32 m_uiThrowGlaiveTimer;
    uint32 m_uiCommandingShoutTimer;
    uint32 m_uiWarderTimer;
    bool m_bUsedBannerOfCommand;
    std::list<ObjectGuid> m_lSummonedWarders;

    void Reset() override
    {
        DespawnSummonedWarders();

        m_uiSearingChainsTimer = 15000;
        m_uiQuakingStompTimer = 6000;
        m_uiThrowGlaiveTimer = urand(21000, 23000);
        m_uiCommandingShoutTimer = urand(12000, 14000);
        m_uiWarderTimer = 15000;
        m_bUsedBannerOfCommand = false;
    }

    void Aggro(Unit* /*who*/) override
    {
        DoScriptText(SAY_AXELUS_AGGRO, m_creature);
    }

    void DespawnSummonedWarders()
    {
        for (const auto& guid : m_lSummonedWarders)
        {
            if (Creature* warder = m_creature->GetMap()->GetCreature(guid))
                warder->ForcedDespawn();
        }

        m_lSummonedWarders.clear();
    }

    void HealSummonedWarders()
    {
        for (const auto& guid : m_lSummonedWarders)
        {
            if (Creature* warder = m_creature->GetMap()->GetCreature(guid))
            {
                if (warder->IsAlive())
                    warder->SetHealth(warder->GetMaxHealth());
            }
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_AXELUS_ONYXIAN_WARDER)
            m_lSummonedWarders.push_back(summoned->GetObjectGuid());
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_AXELUS_ONYXIAN_WARDER)
            m_lSummonedWarders.remove(summoned->GetObjectGuid());
    }

    void JustReachedHome() override
    {
        DespawnSummonedWarders();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoScriptText(SAY_AXELUS_DEATH, m_creature);
        DespawnSummonedWarders();
    }

    void DamageTaken(Unit* /*doneBy*/, uint32& damage) override
    {
        if (m_bUsedBannerOfCommand || !damage)
            return;

        uint32 const maxHealth = m_creature->GetMaxHealth();
        uint32 const bannerHealth = maxHealth / 20;
        uint32 const currentHealth = m_creature->GetHealth();
        if (currentHealth > bannerHealth && damage < currentHealth - bannerHealth)
            return;

        damage = 0;
        m_bUsedBannerOfCommand = true;

        m_creature->CastSpell(m_creature, SPELL_AXELUS_BANNER_OF_COMMAND, true);
        SummonAxelusWarder(m_creature);
        m_creature->SetHealth(m_creature->GetMaxHealth());
        HealSummonedWarders();
        DoScriptText(SAY_AXELUS_BANNER_OF_COMMAND, m_creature);
    }

    bool HasQuakingStompCluster() const
    {
        std::vector<Player*> meleePlayers;
        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            Player* player = itr->getSource();
            if (!player || !player->IsAlive() || !player->IsTargetable(true, false) || !m_creature->IsHostileTo(player))
                continue;

            if (!m_creature->CanReachWithMeleeAutoAttack(player))
                continue;

            meleePlayers.push_back(player);
        }

        for (uint32 i = 0; i < meleePlayers.size(); ++i)
        {
            for (uint32 j = i + 1; j < meleePlayers.size(); ++j)
            {
                if (meleePlayers[i]->GetDistance2d(meleePlayers[j]) > 1.0f)
                    continue;

                for (uint32 k = j + 1; k < meleePlayers.size(); ++k)
                {
                    if (meleePlayers[i]->GetDistance2d(meleePlayers[k]) > 1.0f ||
                        meleePlayers[j]->GetDistance2d(meleePlayers[k]) > 1.0f)
                        continue;

                    for (uint32 l = k + 1; l < meleePlayers.size(); ++l)
                    {
                        if (meleePlayers[i]->GetDistance2d(meleePlayers[l]) <= 1.0f &&
                            meleePlayers[j]->GetDistance2d(meleePlayers[l]) <= 1.0f &&
                            meleePlayers[k]->GetDistance2d(meleePlayers[l]) <= 1.0f)
                            return true;
                    }
                }
            }
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->GetVictim())
            return;

        if (m_uiWarderTimer <= uiDiff)
        {
            SummonAxelusWarder(m_creature);
            m_uiWarderTimer = 45000;
        }
        else
            m_uiWarderTimer -= uiDiff;

        if (m_uiQuakingStompTimer <= uiDiff)
        {
            m_uiQuakingStompTimer = 6000;
            if (HasQuakingStompCluster() && DoCastSpellIfCan(m_creature, SPELL_AXELUS_QUAKING_STOMP) == CAST_OK)
            {
                DoScriptText(SAY_AXELUS_QUAKING_STOMP, m_creature);
                return;
            }
        }
        else
            m_uiQuakingStompTimer -= uiDiff;

        if (m_uiSearingChainsTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_AXELUS_SEARING_CHAINS) == CAST_OK)
            {
                DoScriptText(SAY_AXELUS_SEARING_CHAINS, m_creature);
                m_uiSearingChainsTimer = 15000;
                return;
            }
        }
        else
            m_uiSearingChainsTimer -= uiDiff;

        if (m_uiCommandingShoutTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_AXELUS_COMMANDING_SHOUT) == CAST_OK)
            {
                m_uiCommandingShoutTimer = urand(12000, 14000);
                return;
            }
        }
        else
            m_uiCommandingShoutTimer -= uiDiff;

        if (m_uiThrowGlaiveTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->GetVictim(), SPELL_AXELUS_THROW_GLAIVE) == CAST_OK)
            {
                DoScriptText(SAY_AXELUS_THROW_GLAIVE, m_creature);
                m_uiThrowGlaiveTimer = urand(21000, 23000);
                return;
            }
        }
        else
            m_uiThrowGlaiveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_broodcommander_axelus(Creature* pCreature)
{
    return new broodcommander_axelusAI(pCreature);
}

void AddSC_boss_axelus()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "broodcommander_axelus";
    newscript->GetAI = &GetAI_broodcommander_axelus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "spell_axelus_searing_chains";
    newscript->GetSpellScript = &GetSpellScript_axelus_searing_chains;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "spell_axelus_searing_chains_aura";
    newscript->GetAuraScript = &GetAuraScript_axelus_searing_chains_aura;
    newscript->RegisterSelf();
}
