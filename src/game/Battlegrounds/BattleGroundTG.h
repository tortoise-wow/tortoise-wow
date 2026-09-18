#ifndef TURTLE_BATTLEGROUND_TG_H
#define TURTLE_BATTLEGROUND_TG_H

#include "BattleGround.h"
#include "ThornGorgeRules.h"
#include "ThornGorgeDiagnostics.h"

class BattleGroundTGScore : public BattleGroundScore
{
public:
    uint32 FlagCaptures = 0;
};

class BattleGroundTG : public BattleGround
{
public:
    BattleGroundTG();
    void Reset() override;
    void Update(uint32 diff) override;
    bool SetupBattleGround() override;
    void StartingEventCloseDoors() override;
    void StartingEventOpenDoors() override;
    void AddPlayer(Player* player) override;
    void RemovePlayer(Player* player, ObjectGuid guid) override;
    void HandleKillPlayer(Player* player, Player* killer) override;
    void HandleAreaTrigger(Player* player, uint32 trigger) override;
    void EventPlayerClickedOnFlag(Player* player, GameObject* object) override;
    void CompleteFlagPickup(Player* player, GameObject* object);
    void EventPlayerDroppedFlag(Player* player) override;
    void EndBattleGround(Team winner) override;
    Team GetWinningTeam() const override;
    WorldSafeLocsEntry const* GetClosestGraveYard(Player* player) override;
    void FillInitialWorldStates(WorldPacket& data, uint32& count) override;
    void HandleCommand(Player* player, ChatHandler* handler, char* args) override;
    ObjectGuid GetFlagCarrierGuid(uint32 = 0) const override { return m_carrier; }
    // Map-owner bot consumers may query objectives, never mutate match state.
    bool GetObjective(Player* player, float& x, float& y, float& z) const;
    ObjectGuid GetAvailableFlag() const;
    bool AdmitBotDiagnostic(Player* player);

private:
    ThornGorge::Rules m_rules;
    ThornGorge::DiagnosticBudget m_diagnostics;
    uint32 m_snapshotSequence = 0;
    std::map<ObjectGuid, uint32> m_botDiagnosticTicks;
    unsigned m_captureCounts[4][2] = {};
    void Trace(char const* event, Player* player = nullptr, uint32 related = 0, char const* reason = "-", bool critical = false);
    void TraceSnapshot(char const* reason);
    void TraceMovement(Player* player, uint32 sequence);
    char const* FlagRejection(Player* player, GameObject* object);
    ObjectGuid m_carrier;
    uint32 m_captureTickMs = 1200;
    uint32 m_tick = 0;
    uint32 m_elapsed = 0;
    float m_flagX = 0, m_flagY = 0, m_flagZ = 0;
    float m_flagScale = 2.5f;
    void UpdateObjectives();
    void UpdateBanner(unsigned node);
    void SendNodeStates(unsigned node);
    void RewardVictoryQuests(Team winner);
    void SendStates();
    void SendClientState(Player* player);
    void Announce(char const* text);
    bool Eligible(Player* player) const;
    ThornGorge::Team Side(Player* player) const;
    bool OwnFlagObject(GameObject* object) const;
    void RestoreFlag();
};
#endif
