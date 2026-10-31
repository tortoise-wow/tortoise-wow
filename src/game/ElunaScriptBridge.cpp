/*
 * Bridges Turtle WoW's ScriptObject event bus to Eluna.  Map lifecycle and
 * world update hooks are wired directly in Map/World because those calls also
 * own the per-map Lua state lifetime.
 *
 * Originally written by Nighthawk42 for TuringKi/turtle-wow-custom (aa0b2be).
 *
 * Threading: hooks that use GetWorldEluna() get nullptr off the world thread
 * (World::GetEluna), so they are skipped rather than entering the world state
 * from a map thread. Object hooks use the object's map state and run on that
 * map's thread. Packet send/receive hooks are deliberately not bridged: those
 * run on network and async threads. Weather is dispatched from Weather.cpp to
 * the weather's map state, not from here. Game events are dispatched from
 * GameEventMgr: this core's GameEventScript registry is keyed by script id and
 * also serves spell and taxi "send event" scripts on map threads.
 */

#ifdef ENABLE_ELUNA

#include "ScriptObjects.h"

#include "AuctionHouseMgr.h"
#include "BattleGround.h"
#include "Creature.h"
#include "GameObject.h"
#include "Group.h"
#include "Guild.h"
#include "Item.h"
#include "LuaEngine.h"
#include "ElunaConfig.h"
#include "Player.h"
#include "Weather.h"
#include "World.h"
#include "WorldSession.h"

// The game library compiles a copy of dep/Eluna with dep/eluna-patches applied
// (cmake/ElunaSources.cmake). Each patch defines a marker; an unpatched copy
// stops the build here instead of compiling an engine without the fixes.
#include "BasicAI.h"
#include "ElunaEventMgr.h"
#include "ElunaTemplate.h"
#include "ElunaInstanceAI.h"
#include "ElunaCreatureAI.h"
#if !defined ELUNA_TW_PATCH_0001 || !defined ELUNA_TW_PATCH_0002 || !defined ELUNA_TW_PATCH_0003 || !defined ELUNA_TW_PATCH_0004 || !defined ELUNA_TW_PATCH_0005 || !defined ELUNA_TW_PATCH_0006 || !defined ELUNA_TW_PATCH_0007
#error "Eluna is compiled without dep/eluna-patches: re-run CMake (cmake/ElunaSources.cmake makes the patched copy)."
#endif

namespace
{
Eluna* GetWorldEluna()
{
    return sWorld.GetEluna();
}

class ElunaWorldScript final : public WorldScript
{
public:
    ElunaWorldScript() : WorldScript("ElunaWorldScript") {}

    void OnOpenStateChange(bool open) override
    {
        if (Eluna* e = GetWorldEluna())
            e->OnOpenStateChange(open);
    }

    void OnAfterConfigLoad(bool reload) override
    {
        // Initial configuration is dispatched after the global Lua state is
        // created in World::SetInitialWorldSettings().
        if (reload)
            if (Eluna* e = GetWorldEluna())
                e->OnConfigLoad(true);
    }

    void OnShutdownInitiate(uint32 shutdownMask, uint32 exitCode) override
    {
        if (Eluna* e = GetWorldEluna())
            e->OnShutdownInitiate(static_cast<ShutdownExitCode>(exitCode),
                                  static_cast<ShutdownMask>(shutdownMask));
    }

    void OnShutdownCancel() override
    {
        if (Eluna* e = GetWorldEluna())
            e->OnShutdownCancel();
    }
};

class ElunaPlayerScript final : public PlayerScript
{
public:
    ElunaPlayerScript() : PlayerScript("ElunaPlayerScript") {}

    void OnPlayerReleasedGhost(Player* player) override { if (Eluna* e = player->GetEluna()) e->OnRepop(player); }
    void OnPVPKill(Player* killer, Player* killed) override { if (Eluna* e = killer->GetEluna()) e->OnPVPKill(killer, killed); }
    void OnCreatureKill(Player* killer, Creature* killed) override { if (Eluna* e = killer->GetEluna()) e->OnCreatureKill(killer, killed); }
    void OnLevelChanged(Player* player, uint8 oldLevel) override { if (Eluna* e = player->GetEluna()) e->OnLevelChanged(player, oldLevel); }
    void OnTalentsReset(Player* player, bool noCost) override { if (Eluna* e = player->GetEluna()) e->OnTalentsReset(player, noCost); }
    void OnMoneyChanged(Player* player, int32& amount) override { if (Eluna* e = player->GetEluna()) e->OnMoneyChanged(player, amount); }
    void OnGiveXP(Player* player, uint32& amount, Unit* victim) override { if (Eluna* e = player->GetEluna()) e->OnGiveXP(player, amount, victim); }
    void OnReputationChange(Player* player, uint32 factionId, int32& standing) override { if (Eluna* e = player->GetEluna()) e->OnReputationChange(player, factionId, standing, true); }
    void OnLearnSpell(Player* player, uint32 spellId) override { if (Eluna* e = player->GetEluna()) e->OnLearnSpell(player, spellId); }
    void OnDuelRequest(Player* target, Player* challenger) override { if (Eluna* e = target->GetEluna()) e->OnDuelRequest(target, challenger); }
    void OnDuelStart(Player* player1, Player* player2) override { if (Eluna* e = player1->GetEluna()) e->OnDuelStart(player1, player2); }
    void OnDuelEnd(Player* winner, Player* loser, uint32 type) override { if (Eluna* e = winner->GetEluna()) e->OnDuelEnd(winner, loser, static_cast<DuelCompleteType>(type)); }
    void OnEmote(Player* player, uint32 emote) override { if (Eluna* e = player->GetEluna()) e->OnEmote(player, emote); }
    void OnTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, ObjectGuid guid) override { if (Eluna* e = player->GetEluna()) e->OnTextEmote(player, textEmote, emoteNum, guid); }
    void OnSpellCast(Player* player, Spell* spell, bool skipCheck) override { if (Eluna* e = player->GetEluna()) e->OnSpellCast(player, spell, skipCheck); }
    void OnLogin(Player* player) override { if (Eluna* e = player->GetEluna()) e->OnLogin(player); }
    void OnLogout(Player* player) override { if (Eluna* e = player->GetEluna()) e->OnLogout(player); }
    void OnCreate(Player* player) override { if (Eluna* e = GetWorldEluna()) e->OnCreate(player); }
    void OnDelete(ObjectGuid guid, uint32) override { if (Eluna* e = GetWorldEluna()) e->OnDelete(guid.GetCounter()); }
    void OnSave(Player* player) override { if (Eluna* e = player->GetEluna()) e->OnSave(player); }
    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea) override { if (Eluna* e = player->GetEluna()) e->OnUpdateZone(player, newZone, newArea); }
    void OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea) override { if (Eluna* e = player->GetEluna()) e->OnUpdateArea(player, oldArea, newArea); }
    void OnLootItem(Player* player, Item* item, uint32 count, ObjectGuid lootGuid) override { if (Eluna* e = player->GetEluna()) e->OnLootItem(player, item, count, lootGuid); }
};

class ElunaCreatureScript final : public AllCreatureScript
{
public:
    ElunaCreatureScript() : AllCreatureScript("ElunaCreatureScript") {}
    void OnCreatureAddWorld(Creature* creature) override { if (Eluna* e = creature->GetEluna()) e->OnAddToWorld(creature); }
    void OnCreatureRemoveWorld(Creature* creature) override { if (Eluna* e = creature->GetEluna()) e->OnRemoveFromWorld(creature); }
};

class ElunaUnitScript final : public UnitScript
{
public:
    ElunaUnitScript() : UnitScript("ElunaUnitScript") {}
    void OnUnitEnterCombat(Unit* unit, Unit* victim) override
    {
        if (unit->IsPlayer())
            if (Eluna* e = unit->GetEluna())
                e->OnPlayerEnterCombat(static_cast<Player*>(unit), victim);
    }
    void OnUnitExitCombat(Unit* unit) override
    {
        if (unit->IsPlayer())
            if (Eluna* e = unit->GetEluna())
                e->OnPlayerLeaveCombat(static_cast<Player*>(unit));
    }
    void OnUnitDeath(Unit* unit, Unit* killer) override
    {
        if (unit->IsPlayer() && killer && killer->IsCreature())
            if (Eluna* e = unit->GetEluna())
                e->OnPlayerKilledByCreature(static_cast<Creature*>(killer), static_cast<Player*>(unit));
    }
};

class ElunaGameObjectScript final : public AllGameObjectScript
{
public:
    ElunaGameObjectScript() : AllGameObjectScript("ElunaGameObjectScript") {}
    void OnGameObjectAddWorld(GameObject* gameObject) override { if (Eluna* e = gameObject->GetEluna()) e->OnAddToWorld(gameObject); }
    void OnGameObjectRemoveWorld(GameObject* gameObject) override { if (Eluna* e = gameObject->GetEluna()) e->OnRemoveFromWorld(gameObject); }
    void OnGameObjectUpdate(GameObject* gameObject, uint32 diff) override { if (Eluna* e = gameObject->GetEluna()) e->UpdateAI(gameObject, diff); }
};

class ElunaItemScript final : public AllItemScript
{
public:
    ElunaItemScript() : AllItemScript("ElunaItemScript") {}
    bool CanItemUse(Player* player, Item* item, SpellCastTargets& targets) override { if (Eluna* e = player->GetEluna()) return !e->OnUse(player, item, targets); return false; }
    bool CanItemQuestAccept(Player* player, Item* item, Quest const* quest) override { if (Eluna* e = player->GetEluna()) return e->OnQuestAccept(player, item, quest); return false; }
};

class ElunaLootScript final : public LootScript
{
public:
    ElunaLootScript() : LootScript("ElunaLootScript") {}
    void OnLootMoney(Player* player, uint32 gold) override { if (Eluna* e = player->GetEluna()) e->OnLootMoney(player, gold); }
};

class ElunaAuctionHouseScript final : public AuctionHouseScript
{
public:
    ElunaAuctionHouseScript() : AuctionHouseScript("ElunaAuctionHouseScript") {}
    void OnAuctionAdd(AuctionHouseObject* house, AuctionEntry* entry) override { if (Eluna* e = GetWorldEluna()) e->OnAdd(house, entry); }
    void OnAuctionRemove(AuctionHouseObject* house, AuctionEntry* entry) override { if (Eluna* e = GetWorldEluna()) e->OnRemove(house, entry); }
    void OnAuctionSuccessful(AuctionHouseObject* house, AuctionEntry* entry) override { if (Eluna* e = GetWorldEluna()) e->OnSuccessful(house, entry); }
    void OnAuctionExpire(AuctionHouseObject* house, AuctionEntry* entry) override { if (Eluna* e = GetWorldEluna()) e->OnExpire(house, entry); }
};

class ElunaBattlegroundScript final : public AllBattlegroundScript
{
public:
    ElunaBattlegroundScript() : AllBattlegroundScript("ElunaBattlegroundScript") {}
    void OnBattlegroundStart(BattleGround* bg) override { if (Eluna* e = bg->GetBgMap()->GetEluna()) e->OnBGStart(bg, bg->GetTypeID(), bg->GetInstanceID()); }
    void OnBattlegroundEnd(BattleGround* bg, uint32 winner) override { if (Eluna* e = bg->GetBgMap()->GetEluna()) e->OnBGEnd(bg, bg->GetTypeID(), bg->GetInstanceID(), static_cast<Team>(winner)); }
};

class ElunaGroupScript final : public GroupScript
{
public:
    ElunaGroupScript() : GroupScript("ElunaGroupScript") {}
    void OnCreate(Group* group, ObjectGuid leaderGuid, uint8 groupType) override { if (Eluna* e = GetWorldEluna()) e->OnCreate(group, leaderGuid, static_cast<GroupType>(groupType)); }
    void OnInviteMember(Group* group, ObjectGuid guid) override { if (Eluna* e = GetWorldEluna()) e->OnInviteMember(group, guid); }
    bool CanMemberAccept(Group* group, Player* player) override { if (Eluna* e = GetWorldEluna()) return e->OnMemberAccept(group, player); return true; }
    void OnAddMember(Group* group, ObjectGuid guid) override { if (Eluna* e = GetWorldEluna()) e->OnAddMember(group, guid); }
    void OnRemoveMember(Group* group, ObjectGuid guid, uint8 method) override { if (Eluna* e = GetWorldEluna()) e->OnRemoveMember(group, guid, method); }
    void OnChangeLeader(Group* group, ObjectGuid newLeaderGuid, ObjectGuid oldLeaderGuid) override { if (Eluna* e = GetWorldEluna()) e->OnChangeLeader(group, newLeaderGuid, oldLeaderGuid); }
    void OnDisband(Group* group) override { if (Eluna* e = GetWorldEluna()) e->OnDisband(group); }
};

class ElunaGuildScript final : public GuildScript
{
public:
    ElunaGuildScript() : GuildScript("ElunaGuildScript") {}
    void OnAddMember(Guild* guild, Player* player, uint8& rank) override { if (Eluna* e = GetWorldEluna()) e->OnAddMember(guild, player, rank); }
    void OnRemoveMember(Guild* guild, Player* player, bool isDisbanding, bool) override { if (Eluna* e = GetWorldEluna()) e->OnRemoveMember(guild, player, isDisbanding); }
    void OnCreate(Guild* guild, Player* leader, std::string const& name) override { if (Eluna* e = GetWorldEluna()) e->OnCreate(guild, leader, name); }
    void OnDisband(Guild* guild) override { if (Eluna* e = GetWorldEluna()) e->OnDisband(guild); }
    void OnMotdChanged(Guild* guild, std::string const& motd) override { if (Eluna* e = GetWorldEluna()) e->OnMOTDChanged(guild, motd); }
    void OnInfoChanged(Guild* guild, std::string const& info) override { if (Eluna* e = GetWorldEluna()) e->OnInfoChanged(guild, info); }
};
}

void AddElunaScripts()
{
    // Eluna.Enabled = 0: no bridge scripts, so the registries dispatch exactly
    // as in a build without Eluna.
    if (!sElunaConfig->IsElunaEnabled())
        return;

    new ElunaWorldScript();
    new ElunaPlayerScript();
    new ElunaCreatureScript();
    new ElunaUnitScript();
    new ElunaGameObjectScript();
    new ElunaItemScript();
    new ElunaLootScript();
    new ElunaAuctionHouseScript();
    new ElunaBattlegroundScript();
    new ElunaGroupScript();
    new ElunaGuildScript();
}

#endif
