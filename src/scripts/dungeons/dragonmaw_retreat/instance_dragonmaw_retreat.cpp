#include "scriptPCH.h"
#include "dragonmaw_retreat.h"
#include <algorithm>

struct instance_dragonmaw_retreat : public ScriptedInstance
{
    explicit instance_dragonmaw_retreat(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    }

    uint64 m_uiSearistraszGUID;
    uint64 m_uiReserveGateGUID;
    bool m_bZuluhedDead;
    std::vector<ObjectGuid> m_vDeadDragonmawEnchanters;

    void Initialize() override
    {
        m_uiSearistraszGUID = 0;
        m_uiReserveGateGUID = 0;
        m_bZuluhedDead = false;
        m_vDeadDragonmawEnchanters.clear();
    }

    void OnCreatureCreate(Creature* pCreature) override
    {
        if (pCreature->GetEntry() == NPC_SEARISTRASZ)
            m_uiSearistraszGUID = pCreature->GetGUID();

        // A boss killed earlier in a saved instance loads dead: open the gate again.
        if (pCreature->GetEntry() == NPC_ZULUHED_THE_WHACKED && !pCreature->IsAlive())
        {
            m_bZuluhedDead = true;
            OpenReserveGate();
        }
    }

    void OnObjectCreate(GameObject* pGo) override
    {
        if (pGo->GetEntry() == GO_DRAGONMAW_RESERVE_GATE)
        {
            m_uiReserveGateGUID = pGo->GetGUID();

            // The gate can load after the dead boss.
            if (m_bZuluhedDead)
                OpenReserveGate();
        }
    }

    // The Reserve Gate leads to the back exit and opens once Zuluhed the Whacked is dead.
    void OpenReserveGate()
    {
        if (GameObject* pGate = instance->GetGameObject(m_uiReserveGateGUID))
            if (pGate->GetGoState() != GO_STATE_ACTIVE)
                pGate->SetGoState(GO_STATE_ACTIVE);
    }

    void OnCreatureDeath(Creature* pCreature) override
    {
        if (pCreature->GetEntry() == NPC_ZULUHED_THE_WHACKED)
        {
            m_bZuluhedDead = true;
            OpenReserveGate();
        }

        if (pCreature->GetEntry() == NPC_DRAGONMAW_ENCHANTER)
        {
            MarkDragonmawEnchanterDead(pCreature->GetObjectGuid());

            if (GetDragonmawEnchantersDeadCount() >= MAX_DRAGONMAW_ENCHANTERS)
            {
                if (Creature* pSearistrasz = GetSearistrasz())
                {
                    if (pSearistrasz->HasAura(SPELL_ENCHANTING_FLAMES))
                        DoScriptText(SAY_DRAGONMAW_SEARISTRASZ_FLAME_REMOVED, pSearistrasz);

                    pSearistrasz->RemoveAurasDueToSpell(SPELL_ENCHANTING_FLAMES);
                }
            }
        }
    }

    uint32 GetData(uint32 uiType) override
    {
        if (uiType == DATA_DRAGONMAW_ENCHANTERS_DEAD)
            return GetDragonmawEnchantersDeadCount();

        return 0;
    }

    uint64 GetData64(uint32 uiType) override
    {
        if (uiType == DATA_SEARISTRASZ_GUID)
            return m_uiSearistraszGUID;

        return 0;
    }

    Creature* GetSearistrasz()
    {
        return m_uiSearistraszGUID ? instance->GetCreature(m_uiSearistraszGUID) : nullptr;
    }

    void MarkDragonmawEnchanterDead(ObjectGuid guid)
    {
        if (std::find(m_vDeadDragonmawEnchanters.begin(), m_vDeadDragonmawEnchanters.end(), guid) == m_vDeadDragonmawEnchanters.end())
            m_vDeadDragonmawEnchanters.push_back(guid);
    }

    uint32 GetDragonmawEnchantersDeadCount() const
    {
        return static_cast<uint32>(m_vDeadDragonmawEnchanters.size());
    }
};

InstanceData* GetInstanceData_instance_dragonmaw_retreat(Map* pMap)
{
    return new instance_dragonmaw_retreat(pMap);
}

void AddSC_instance_dragonmaw_retreat()
{
    Script* pNewscript = new Script;
    pNewscript->Name = "instance_dragonmaw_retreat";
    pNewscript->GetInstanceData = &GetInstanceData_instance_dragonmaw_retreat;
    pNewscript->RegisterSelf();
}
