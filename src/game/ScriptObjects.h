#ifndef _SCRIPT_OBJECTS_H
#define _SCRIPT_OBJECTS_H

#include "ScriptMgr.h"
#include "Chat.h"
#include <vector>

class AuctionHouseObject;
class AuctionEntry;
class BattleGround;
class Channel;
class DynamicObject;
class Group;
class Guild;
class MailReceiver;
class MailSender;
class MailDraft;
class Pet;
class Transport;
class Weather;
class WorldPacket;
class WorldSession;
class WorldSocket;
struct ItemPrototype;
struct Loot;
struct LootStoreItem;
struct LootTemplate;
class LootStore;

inline void TortoiseEnableAllHooksIfEmpty(std::vector<uint16>& enabledHooks, uint16 totalAvailableHooks)
{
    if (!enabledHooks.empty())
        return;

    enabledHooks.reserve(totalAvailableHooks);
    for (uint16 hook = 0; hook < totalAvailableHooks; ++hook)
        enabledHooks.push_back(hook);
}

template<class TObject>
class UpdatableScript
{
    public:
        virtual void OnUpdate(TObject* /*object*/, uint32 /*diff*/) {}
};

template<class TMap>
class MapScript : public UpdatableScript<TMap>
{
    protected:
        explicit MapScript(uint32 mapId) : m_mapId(mapId) {}

    public:
        uint32 GetMapId() const { return m_mapId; }

        virtual void OnCreate(TMap* /*map*/) {}
        virtual void OnDestroy(TMap* /*map*/) {}
        virtual void OnLoadGridMap(TMap* /*map*/, uint32 /*gx*/, uint32 /*gy*/) {}
        virtual void OnUnloadGridMap(TMap* /*map*/, uint32 /*gx*/, uint32 /*gy*/) {}
        virtual void OnPlayerEnter(TMap* /*map*/, Player* /*player*/) {}
        virtual void OnPlayerLeave(TMap* /*map*/, Player* /*player*/) {}

    private:
        uint32 m_mapId;
};

enum WorldHook
{
    WORLDHOOK_ON_OPEN_STATE_CHANGE,
    WORLDHOOK_ON_AFTER_CONFIG_LOAD,
    WORLDHOOK_ON_LOAD_CUSTOM_DATABASE_TABLE,
    WORLDHOOK_ON_BEFORE_CONFIG_LOAD,
    WORLDHOOK_ON_MOTD_CHANGE,
    WORLDHOOK_ON_SHUTDOWN_INITIATE,
    WORLDHOOK_ON_SHUTDOWN_CANCEL,
    WORLDHOOK_ON_UPDATE,
    WORLDHOOK_ON_STARTUP,
    WORLDHOOK_ON_SHUTDOWN,
    WORLDHOOK_ON_AFTER_UNLOAD_ALL_MAPS,
    WORLDHOOK_ON_BEFORE_WORLD_INITIALIZED,
    WORLDHOOK_ON_CHANNEL_BROADCAST,
    WORLDHOOK_ON_BOT_LOGIN_YIELD,
    WORLDHOOK_END
};

class WorldScript : public ScriptObject
{
    protected:
        explicit WorldScript(char const* name, std::vector<uint16> enabledHooks = {})
            : ScriptObject(name, WORLDHOOK_END)
        {
            TortoiseEnableAllHooksIfEmpty(enabledHooks, WORLDHOOK_END);
            ScriptRegistry<WorldScript>::AddScript(this, std::move(enabledHooks));
        }

    public:
        virtual void OnOpenStateChange(bool /*open*/) {}
        virtual void OnAfterConfigLoad(bool /*reload*/) {}
        virtual void OnLoadCustomDatabaseTable() {}
        virtual void OnBeforeConfigLoad(bool /*reload*/) {}
        virtual void OnMotdChange(std::string& /*newMotd*/) {}
        virtual void OnShutdownInitiate(uint32 /*shutdownMask*/, uint32 /*exitCode*/) {}
        virtual void OnShutdownCancel() {}
        virtual void OnUpdate(uint32 /*diff*/) {}
        virtual void OnStartup() {}
        virtual void OnShutdown() {}
        virtual void OnAfterUnloadAllMaps() {}
        virtual void OnBeforeWorldInitialized() {}
        virtual void OnChannelBroadcast(uint32 /*guidLow*/, char const* /*channel*/, char const* /*msg*/) {}
        virtual void OnBotLoginYield(uint32 /*guidLow*/) {}
};

enum PlayerHook
{
    PLAYERHOOK_ON_PLAYER_JUST_DIED,
    PLAYERHOOK_ON_PLAYER_RELEASED_GHOST,
    PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST,
    PLAYERHOOK_ON_PVP_KILL,
    PLAYERHOOK_ON_CREATURE_KILL,
    PLAYERHOOK_ON_LEVEL_CHANGED,
    PLAYERHOOK_ON_TALENTS_RESET,
    PLAYERHOOK_ON_BEFORE_UPDATE,
    PLAYERHOOK_ON_UPDATE,
    PLAYERHOOK_ON_MONEY_CHANGED,
    PLAYERHOOK_ON_GIVE_EXP,
    PLAYERHOOK_ON_REPUTATION_CHANGE,
    PLAYERHOOK_ON_LEARN_SPELL,
    PLAYERHOOK_ON_FORGOT_SPELL,
    PLAYERHOOK_ON_DUEL_REQUEST,
    PLAYERHOOK_ON_DUEL_START,
    PLAYERHOOK_ON_DUEL_END,
    PLAYERHOOK_ON_BEFORE_SEND_CHAT_MESSAGE,
    PLAYERHOOK_ON_EMOTE,
    PLAYERHOOK_ON_TEXT_EMOTE,
    PLAYERHOOK_ON_SPELL_CAST,
    PLAYERHOOK_ON_LOGIN,
    PLAYERHOOK_ON_BEFORE_LOGOUT,
    PLAYERHOOK_ON_LOGOUT,
    PLAYERHOOK_ON_CREATE,
    PLAYERHOOK_ON_DELETE,
    PLAYERHOOK_ON_SAVE,
    PLAYERHOOK_ON_UPDATE_ZONE,
    PLAYERHOOK_ON_UPDATE_AREA,
    PLAYERHOOK_ON_MAP_CHANGED,
    PLAYERHOOK_ON_BEFORE_TELEPORT,
    PLAYERHOOK_ON_LOOT_ITEM,
    PLAYERHOOK_ON_CHAT_SAY,
    PLAYERHOOK_ON_CHAT_YELL,
    PLAYERHOOK_ON_CHAT_CHANNEL,
    PLAYERHOOK_ON_CHAT_WHISPER,
    PLAYERHOOK_ON_CHAT_GUILD,
    PLAYERHOOK_ON_TEXT_EMOTE_HEARD,
    PLAYERHOOK_IS_MANAGED_BOT,
    PLAYERHOOK_GET_BOT_ROLES,
    PLAYERHOOK_ON_ADDON_MESSAGE,
    PLAYERHOOK_END
};

class PlayerScript : public ScriptObject
{
    protected:
        explicit PlayerScript(char const* name, std::vector<uint16> enabledHooks = {})
            : ScriptObject(name, PLAYERHOOK_END)
        {
            TortoiseEnableAllHooksIfEmpty(enabledHooks, PLAYERHOOK_END);
            ScriptRegistry<PlayerScript>::AddScript(this, std::move(enabledHooks));
        }

    public:
        virtual void OnPlayerJustDied(Player* /*player*/) {}
        virtual void OnPlayerReleasedGhost(Player* /*player*/) {}
        virtual void OnPlayerCompleteQuest(Player* /*player*/, Quest const* /*quest*/) {}
        virtual void OnPVPKill(Player* /*killer*/, Player* /*killed*/) {}
        virtual void OnCreatureKill(Player* /*killer*/, Creature* /*killed*/) {}
        virtual void OnLevelChanged(Player* /*player*/, uint8 /*oldLevel*/) {}
        virtual void OnTalentsReset(Player* /*player*/, bool /*noCost*/) {}
        virtual void OnBeforeUpdate(Player* /*player*/, uint32 /*diff*/) {}
        virtual void OnUpdate(Player* /*player*/, uint32 /*diff*/) {}
        virtual void OnMoneyChanged(Player* /*player*/, int32& /*amount*/) {}
        virtual void OnGiveXP(Player* /*player*/, uint32& /*amount*/, Unit* /*victim*/) {}
        virtual void OnReputationChange(Player* /*player*/, uint32 /*factionId*/, int32& /*standing*/) {}
        virtual void OnLearnSpell(Player* /*player*/, uint32 /*spellId*/) {}
        virtual void OnForgotSpell(Player* /*player*/, uint32 /*spellId*/) {}
        virtual void OnDuelRequest(Player* /*target*/, Player* /*challenger*/) {}
        virtual void OnDuelStart(Player* /*player1*/, Player* /*player2*/) {}
        virtual void OnDuelEnd(Player* /*winner*/, Player* /*loser*/, uint32 /*type*/) {}
        virtual void OnBeforeSendChatMessage(Player* /*player*/, uint32& /*type*/, uint32& /*language*/, std::string& /*message*/) {}
        virtual void OnEmote(Player* /*player*/, uint32 /*emote*/) {}
        virtual void OnTextEmote(Player* /*player*/, uint32 /*textEmote*/, uint32 /*emoteNum*/, ObjectGuid /*guid*/) {}
        virtual void OnSpellCast(Player* /*player*/, Spell* /*spell*/, bool /*skipCheck*/) {}
        virtual void OnLogin(Player* /*player*/) {}
        virtual void OnBeforeLogout(Player* /*player*/) {}
        virtual void OnLogout(Player* /*player*/) {}
        virtual void OnCreate(Player* /*player*/) {}
        virtual void OnDelete(ObjectGuid /*guid*/, uint32 /*accountId*/) {}
        virtual void OnSave(Player* /*player*/) {}
        virtual void OnUpdateZone(Player* /*player*/, uint32 /*newZone*/, uint32 /*newArea*/) {}
        virtual void OnUpdateArea(Player* /*player*/, uint32 /*oldArea*/, uint32 /*newArea*/) {}
        virtual void OnMapChanged(Player* /*player*/) {}
        virtual void OnBeforeTeleport(Player* /*player*/, uint32 /*mapId*/, float /*x*/, float /*y*/, float /*z*/, float /*orientation*/) {}
        virtual void OnLootItem(Player* /*player*/, Item* /*item*/, uint32 /*count*/, ObjectGuid /*lootGuid*/) {}
        virtual void OnChatSay(Player* /*from*/, float /*range*/, char const* /*msg*/) {}
        virtual void OnChatYell(Player* /*from*/, float /*range*/, char const* /*msg*/) {}
        virtual void OnChatChannel(Player* /*from*/, char const* /*channel*/, char const* /*msg*/) {}
        virtual void OnChatWhisper(Player* /*from*/, char const* /*msg*/) {}
        virtual void OnChatGuild(Player* /*from*/, char const* /*msg*/) {}
        virtual void OnTextEmoteHeard(Player* /*from*/, uint32 /*textEmote*/, ObjectGuid /*target*/) {}
        virtual bool IsManagedBot(Player* /*who*/) { return false; }
        virtual uint8 GetBotRoles(Player* /*who*/) { return 0; }

        // A module may take an addon message as a command of its own. Return true
        // when the text was consumed; the core then does not relay it.
        virtual bool OnAddonMessage(Player* /*from*/, std::string const& /*msg*/) { return false; }
};

class CreatureScript : public ScriptObject, public UpdatableScript<Creature>
{
    protected:
        explicit CreatureScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<CreatureScript>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }

        virtual bool OnGossipHello(Player* /*player*/, Creature* /*creature*/) { return false; }
        virtual bool OnGossipSelect(Player* /*player*/, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/) { return false; }
        virtual bool OnGossipSelectCode(Player* /*player*/, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/, char const* /*code*/) { return false; }
        virtual bool OnQuestAccept(Player* /*player*/, Creature* /*creature*/, Quest const* /*quest*/) { return false; }
        virtual bool OnQuestComplete(Player* /*player*/, Creature* /*creature*/, Quest const* /*quest*/) { return false; }
        virtual bool OnQuestReward(Player* /*player*/, Creature* /*creature*/, Quest const* /*quest*/, uint32 /*option*/) { return false; }
        virtual uint32 GetDialogStatus(Player* /*player*/, Creature* /*creature*/) { return 0; }
        virtual CreatureAI* GetAI(Creature* /*creature*/) const { return nullptr; }
        virtual void OnFfaPvpStateUpdate(Creature* /*creature*/, bool /*enabled*/) {}
};

template<class AI>
class GenericCreatureScript : public CreatureScript
{
    public:
        explicit GenericCreatureScript(char const* name) : CreatureScript(name) {}
        CreatureAI* GetAI(Creature* creature) const override { return new AI(creature); }
};

#define RegisterCreatureAI(ai_name) new GenericCreatureScript<ai_name>(#ai_name)

class GameObjectScript : public ScriptObject, public UpdatableScript<GameObject>
{
    protected:
        explicit GameObjectScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<GameObjectScript>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }

        virtual bool OnGossipHello(Player* /*player*/, GameObject* /*go*/) { return false; }
        virtual bool OnGossipSelect(Player* /*player*/, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/) { return false; }
        virtual bool OnGossipSelectCode(Player* /*player*/, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/, char const* /*code*/) { return false; }
        virtual bool OnQuestAccept(Player* /*player*/, GameObject* /*go*/, Quest const* /*quest*/) { return false; }
        virtual bool OnQuestReward(Player* /*player*/, GameObject* /*go*/, Quest const* /*quest*/, uint32 /*option*/) { return false; }
        virtual uint32 GetDialogStatus(Player* /*player*/, GameObject* /*go*/) { return 0; }
        virtual void OnDestroyed(GameObject* /*go*/, Player* /*player*/) {}
        virtual void OnDamaged(GameObject* /*go*/, Player* /*player*/) {}
        virtual void OnLootStateChanged(GameObject* /*go*/, uint32 /*state*/, Unit* /*unit*/) {}
        virtual void OnGameObjectStateChanged(GameObject* /*go*/, uint32 /*state*/) {}
        virtual GameObjectAI* GetAI(GameObject* /*go*/) const { return nullptr; }
};

template<class AI>
class GenericGameObjectScript : public GameObjectScript
{
    public:
        explicit GenericGameObjectScript(char const* name) : GameObjectScript(name) {}
        GameObjectAI* GetAI(GameObject* go) const override { return new AI(go); }
};

#define RegisterGameObjectAI(ai_name) new GenericGameObjectScript<ai_name>(#ai_name)

class ItemScript : public ScriptObject
{
    protected:
        explicit ItemScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<ItemScript>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }

        virtual bool OnQuestAccept(Player* /*player*/, Item* /*item*/, Quest const* /*quest*/) { return false; }
        virtual bool OnUse(Player* /*player*/, Item* /*item*/, SpellCastTargets& /*targets*/) { return false; }
        virtual bool OnUseSpell(Player* /*player*/, Item* /*item*/, SpellCastTargets const& /*targets*/) { return false; }
        virtual bool OnRemove(Player* /*player*/, Item* /*item*/) { return false; }
        virtual bool OnExpire(Player* /*player*/, ItemPrototype const* /*proto*/) { return false; }
};

class SpellScriptLoader : public ScriptObject
{
    protected:
        explicit SpellScriptLoader(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<SpellScriptLoader>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }
        virtual SpellScript* GetSpellScript() const { return nullptr; }
        virtual AuraScript* GetAuraScript() const { return nullptr; }
};

class AreaTriggerScript : public ScriptObject
{
    protected:
        explicit AreaTriggerScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<AreaTriggerScript>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }
        virtual bool OnTrigger(Player* /*player*/, AreaTriggerEntry const* /*trigger*/) { return false; }
};

class CommandScript : public ScriptObject
{
    protected:
        explicit CommandScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<CommandScript>::AddScript(this);
        }

    public:
        virtual std::vector<ChatCommand> GetCommands() const { return {}; }
};

class ModuleScript : public ScriptObject
{
    protected:
        explicit ModuleScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<ModuleScript>::AddScript(this);
        }
};

class AccountScript : public ScriptObject
{
    protected:
        explicit AccountScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<AccountScript>::AddScript(this);
        }

    public:
        virtual void OnAccountLogin(uint32 /*accountId*/) {}
        virtual void OnFailedAccountLogin(uint32 /*accountId*/) {}
        virtual void OnEmailChange(uint32 /*accountId*/) {}
        virtual void OnFailedEmailChange(uint32 /*accountId*/) {}
        virtual void OnPasswordChange(uint32 /*accountId*/) {}
        virtual void OnFailedPasswordChange(uint32 /*accountId*/) {}
};

class AllCommandScript : public ScriptObject
{
    protected:
        explicit AllCommandScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<AllCommandScript>::AddScript(this);
        }

    public:
        virtual bool CanExecuteCommand(ChatHandler* /*handler*/, char const* /*command*/, char const* /*args*/) { return true; }
};

class AllMapScript : public ScriptObject
{
    protected:
        explicit AllMapScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<AllMapScript>::AddScript(this);
        }

    public:
        virtual void OnCreateMap(Map* /*map*/) {}
        virtual void OnDestroyMap(Map* /*map*/) {}
        virtual void OnPlayerEnterAll(Map* /*map*/, Player* /*player*/) {}
        virtual void OnPlayerLeaveAll(Map* /*map*/, Player* /*player*/) {}
        virtual void OnMapUpdate(Map* /*map*/, uint32 /*diff*/) {}
};

class WorldMapScript : public ScriptObject, public MapScript<Map>
{
    protected:
        WorldMapScript(char const* name, uint32 mapId)
            : ScriptObject(name), MapScript<Map>(mapId)
        {
            ScriptRegistry<WorldMapScript>::AddScript(this);
        }

    public:
        bool IsAfterDatabaseLoad() const override { return true; }
};

class InstanceMapScript : public ScriptObject, public MapScript<Map>
{
    protected:
        InstanceMapScript(char const* name, uint32 mapId)
            : ScriptObject(name), MapScript<Map>(mapId)
        {
            ScriptRegistry<InstanceMapScript>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }
        virtual InstanceData* GetInstanceData(Map* /*map*/) const { return nullptr; }
};

template<class TInstanceData>
class GenericInstanceMapScript : public InstanceMapScript
{
    public:
        GenericInstanceMapScript(char const* name, uint32 mapId) : InstanceMapScript(name, mapId) {}
        InstanceData* GetInstanceData(Map* map) const override { return new TInstanceData(map); }
};

#define RegisterInstanceScript(script_name, map_id) new GenericInstanceMapScript<script_name>(#script_name, map_id)

class TransportScript : public ScriptObject, public UpdatableScript<Transport>
{
    protected:
        explicit TransportScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<TransportScript>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }
        virtual void OnAddPassenger(Transport* /*transport*/, Player* /*player*/) {}
        virtual void OnRemovePassenger(Transport* /*transport*/, Player* /*player*/) {}
        virtual void OnRelocate(Transport* /*transport*/, uint32 /*waypointId*/, uint32 /*mapId*/, float /*x*/, float /*y*/, float /*z*/) {}
};

class WeatherScript : public ScriptObject, public UpdatableScript<Weather>
{
    protected:
        explicit WeatherScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<WeatherScript>::AddScript(this);
        }

    public:
        bool IsDatabaseBound() const override { return true; }
        virtual void OnChange(Weather* /*weather*/, uint32 /*state*/, float /*grade*/) {}
};

class DynamicObjectScript : public ScriptObject, public UpdatableScript<DynamicObject>
{
    protected:
        explicit DynamicObjectScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<DynamicObjectScript>::AddScript(this);
        }
};

enum UnitHook
{
    UNITHOOK_ON_HEAL,
    UNITHOOK_ON_DAMAGE,
    UNITHOOK_MODIFY_MELEE_DAMAGE,
    UNITHOOK_MODIFY_SPELL_DAMAGE_TAKEN,
    UNITHOOK_MODIFY_HEAL_RECEIVED,
    UNITHOOK_ON_AURA_APPLY,
    UNITHOOK_ON_AURA_REMOVE,
    UNITHOOK_ON_UNIT_UPDATE,
    UNITHOOK_ON_UNIT_ENTER_COMBAT,
    UNITHOOK_ON_UNIT_EXIT_COMBAT,
    UNITHOOK_ON_UNIT_DEATH,
    UNITHOOK_ON_BUFF_RECEIVED,
    UNITHOOK_END
};

class UnitScript : public ScriptObject
{
    protected:
        explicit UnitScript(char const* name, std::vector<uint16> enabledHooks = {})
            : ScriptObject(name, UNITHOOK_END)
        {
            TortoiseEnableAllHooksIfEmpty(enabledHooks, UNITHOOK_END);
            ScriptRegistry<UnitScript>::AddScript(this, std::move(enabledHooks));
        }

    public:
        virtual void OnHeal(Unit* /*healer*/, Unit* /*receiver*/, uint32& /*gain*/) {}
        virtual void OnDamage(Unit* /*attacker*/, Unit* /*victim*/, uint32& /*damage*/) {}
        virtual void ModifyMeleeDamage(Unit* /*target*/, Unit* /*attacker*/, uint32& /*damage*/) {}
        virtual void ModifySpellDamageTaken(Unit* /*target*/, Unit* /*attacker*/, int32& /*damage*/, SpellEntry const* /*spellInfo*/) {}
        virtual void ModifyHealReceived(Unit* /*target*/, Unit* /*healer*/, uint32& /*heal*/, SpellEntry const* /*spellInfo*/) {}
        virtual void OnAuraApply(Unit* /*unit*/, Aura* /*aura*/) {}
        virtual void OnAuraRemove(Unit* /*unit*/, Aura* /*aura*/) {}
        virtual void OnUnitUpdate(Unit* /*unit*/, uint32 /*diff*/) {}
        virtual void OnUnitEnterCombat(Unit* /*unit*/, Unit* /*victim*/) {}
        virtual void OnUnitExitCombat(Unit* /*unit*/) {}
        virtual void OnUnitDeath(Unit* /*unit*/, Unit* /*killer*/) {}
        virtual void OnBuffReceived(Unit* /*target*/, Player* /*caster*/, uint32 /*spellId*/) {}
};

class WorldObjectScript : public ScriptObject
{
    protected:
        explicit WorldObjectScript(char const* name)
            : ScriptObject(name)
        {
            ScriptRegistry<WorldObjectScript>::AddScript(this);
        }

    public:
        virtual void OnWorldObjectDestroy(WorldObject* /*object*/) {}
        virtual void OnWorldObjectCreate(WorldObject* /*object*/) {}
        virtual void OnWorldObjectSetMap(WorldObject* /*object*/, Map* /*map*/) {}
        virtual void OnWorldObjectResetMap(WorldObject* /*object*/) {}
        virtual void OnWorldObjectUpdate(WorldObject* /*object*/, uint32 /*diff*/) {}
};

class AllCreatureScript : public ScriptObject
{
    protected:
        explicit AllCreatureScript(char const* name) : ScriptObject(name) { ScriptRegistry<AllCreatureScript>::AddScript(this); }
    public:
        virtual void OnAllCreatureUpdate(Creature* /*creature*/, uint32 /*diff*/) {}
        virtual void OnCreatureAddWorld(Creature* /*creature*/) {}
        virtual void OnCreatureRemoveWorld(Creature* /*creature*/) {}
        virtual bool CanCreatureGossipHello(Player* /*player*/, Creature* /*creature*/) { return false; }
        virtual CreatureAI* GetCreatureAI(Creature* /*creature*/) const { return nullptr; }
};

class AllGameObjectScript : public ScriptObject
{
    protected:
        explicit AllGameObjectScript(char const* name) : ScriptObject(name) { ScriptRegistry<AllGameObjectScript>::AddScript(this); }
    public:
        virtual void OnGameObjectAddWorld(GameObject* /*go*/) {}
        virtual void OnGameObjectRemoveWorld(GameObject* /*go*/) {}
        virtual void OnGameObjectUpdate(GameObject* /*go*/, uint32 /*diff*/) {}
        virtual bool CanGameObjectGossipHello(Player* /*player*/, GameObject* /*go*/) { return false; }
        virtual bool CanGameObjectGossipSelect(Player* /*player*/, GameObject* /*go*/, uint32 /*sender*/, uint32 /*action*/, char const* /*code*/) { return false; }
        virtual GameObjectAI* GetGameObjectAI(GameObject* /*go*/) const { return nullptr; }
};

class AllItemScript : public ScriptObject
{
    protected:
        explicit AllItemScript(char const* name) : ScriptObject(name) { ScriptRegistry<AllItemScript>::AddScript(this); }
    public:
        virtual bool CanItemUse(Player* /*player*/, Item* /*item*/, SpellCastTargets& /*targets*/) { return false; }
        virtual bool CanItemQuestAccept(Player* /*player*/, Item* /*item*/, Quest const* /*quest*/) { return false; }
        virtual void OnItemRemove(Player* /*player*/, Item* /*item*/) {}
};

class AllSpellScript : public ScriptObject
{
    protected:
        explicit AllSpellScript(char const* name) : ScriptObject(name) { ScriptRegistry<AllSpellScript>::AddScript(this); }
    public:
        virtual bool CanPrepare(Spell* /*spell*/) { return true; }
        virtual void OnPrepare(Spell* /*spell*/) {}
        virtual void OnCast(Spell* /*spell*/) {}
        virtual void OnCastCancel(Spell* /*spell*/) {}
};

class DatabaseScript : public ScriptObject
{
    protected:
        explicit DatabaseScript(char const* name) : ScriptObject(name) { ScriptRegistry<DatabaseScript>::AddScript(this); }
    public:
        virtual void OnAfterDatabasesLoaded(uint32 /*updateFlags*/) {}
};

class GlobalScript : public ScriptObject
{
    protected:
        explicit GlobalScript(char const* name) : ScriptObject(name) { ScriptRegistry<GlobalScript>::AddScript(this); }
    public:
        virtual void OnItemDelFromDB(ObjectGuid /*itemGuid*/) {}
        virtual void OnLoadSpellCustomAttr(SpellEntry* /*spell*/) {}
        virtual void OnInstanceIdRemoved(uint32 /*instanceId*/) {}
};

enum ServerHook
{
    SERVERHOOK_ON_NETWORK_START,
    SERVERHOOK_ON_NETWORK_STOP,
    SERVERHOOK_ON_SOCKET_OPEN,
    SERVERHOOK_ON_SOCKET_CLOSE,
    SERVERHOOK_CAN_PACKET_SEND,
    SERVERHOOK_CAN_PACKET_RECEIVE,
    SERVERHOOK_END
};

class ServerScript : public ScriptObject
{
    protected:
        explicit ServerScript(char const* name, std::vector<uint16> enabledHooks = {})
            : ScriptObject(name, SERVERHOOK_END)
        {
            TortoiseEnableAllHooksIfEmpty(enabledHooks, SERVERHOOK_END);
            ScriptRegistry<ServerScript>::AddScript(this, std::move(enabledHooks));
        }

    public:
        virtual void OnNetworkStart() {}
        virtual void OnNetworkStop() {}
        virtual void OnSocketOpen(WorldSocket* /*socket*/) {}
        virtual void OnSocketClose(WorldSocket* /*socket*/) {}
        virtual bool CanPacketSend(WorldSession* /*session*/, WorldPacket const& /*packet*/) { return true; }
        virtual bool CanPacketReceive(WorldSession* /*session*/, WorldPacket const& /*packet*/) { return true; }
};

class MiscScript : public ScriptObject
{
    protected:
        explicit MiscScript(char const* name) : ScriptObject(name) { ScriptRegistry<MiscScript>::AddScript(this); }
    public:
        virtual void OnConstructObject(Object* /*object*/) {}
        virtual void OnDestructObject(Object* /*object*/) {}
        virtual void OnConstructPlayer(Player* /*player*/) {}
        virtual void OnDestructPlayer(Player* /*player*/) {}
        virtual void OnItemCreate(Item* /*item*/, ItemPrototype const* /*proto*/, Player const* /*owner*/) {}
        virtual bool CanApplySoulboundFlag(Item* /*item*/, ItemPrototype const* /*proto*/) { return true; }
};

class FormulaScript : public ScriptObject
{
    protected:
        explicit FormulaScript(char const* name) : ScriptObject(name) { ScriptRegistry<FormulaScript>::AddScript(this); }
    public:
        virtual void OnHonorCalculation(float& /*honor*/, uint8 /*level*/, float /*multiplier*/) {}
        virtual void OnGrayLevelCalculation(uint8& /*grayLevel*/, uint8 /*playerLevel*/) {}
        virtual void OnBaseGainCalculation(uint32& /*gain*/, uint8 /*playerLevel*/, uint8 /*mobLevel*/) {}
        virtual void OnGainCalculation(uint32& /*gain*/, Player* /*player*/, Unit* /*unit*/) {}
        virtual void OnGroupRateCalculation(float& /*rate*/, uint32 /*count*/, bool /*isRaid*/) {}
};

class ConditionScript : public ScriptObject
{
    protected:
        explicit ConditionScript(char const* name) : ScriptObject(name) { ScriptRegistry<ConditionScript>::AddScript(this); }
    public:
        bool IsDatabaseBound() const override { return true; }
        virtual bool OnConditionCheck(uint32 /*conditionId*/, WorldObject* /*source*/, WorldObject* /*target*/) { return true; }
};

class LootScript : public ScriptObject
{
    protected:
        explicit LootScript(char const* name) : ScriptObject(name) { ScriptRegistry<LootScript>::AddScript(this); }
    public:
        virtual void OnLootMoney(Player* /*player*/, uint32 /*gold*/) {}
        virtual void OnBeforeLootItemAdded(LootStoreItem& /*storeItem*/, Loot& /*loot*/) {}
        // Return true to take over loot generation (the core will skip its default template processing).
        virtual bool OnBeforeLootGenerated(Loot& /*loot*/, uint32 /*lootId*/, LootTemplate const& /*lootTemplate*/, LootStore const& /*lootStore*/, Player const* /*lootOwner*/, bool /*personal*/, bool /*noEmptyError*/, WorldObject const* /*looted*/) { return false; }
};

class GameEventScript : public ScriptObject
{
    protected:
        explicit GameEventScript(char const* name) : ScriptObject(name) { ScriptRegistry<GameEventScript>::AddScript(this); }
    public:
        bool IsDatabaseBound() const override { return true; }
        virtual void OnStart(uint16 /*eventId*/) {}
        virtual void OnStop(uint16 /*eventId*/) {}
        virtual void OnUpdate(uint16 /*eventId*/) {}
};

class AuctionHouseScript : public ScriptObject
{
    protected:
        explicit AuctionHouseScript(char const* name) : ScriptObject(name) { ScriptRegistry<AuctionHouseScript>::AddScript(this); }
    public:
        virtual void OnAuctionAdd(AuctionHouseObject* /*ah*/, AuctionEntry* /*entry*/) {}
        virtual void OnAuctionRemove(AuctionHouseObject* /*ah*/, AuctionEntry* /*entry*/) {}
        virtual void OnAuctionSuccessful(AuctionHouseObject* /*ah*/, AuctionEntry* /*entry*/) {}
        virtual void OnAuctionExpire(AuctionHouseObject* /*ah*/, AuctionEntry* /*entry*/) {}
};

class BattlegroundScript : public ScriptObject
{
    protected:
        explicit BattlegroundScript(char const* name) : ScriptObject(name) { ScriptRegistry<BattlegroundScript>::AddScript(this); }
    public:
        bool IsDatabaseBound() const override { return true; }
        virtual BattleGround* CreateBattleground(uint32 /*typeId*/) { return nullptr; }
};

class BattlegroundMapScript : public ScriptObject, public MapScript<Map>
{
    protected:
        BattlegroundMapScript(char const* name, uint32 mapId) : ScriptObject(name), MapScript<Map>(mapId) { ScriptRegistry<BattlegroundMapScript>::AddScript(this); }
};

class AllBattlegroundScript : public ScriptObject
{
    protected:
        explicit AllBattlegroundScript(char const* name) : ScriptObject(name) { ScriptRegistry<AllBattlegroundScript>::AddScript(this); }
    public:
        virtual void OnBattlegroundStart(BattleGround* /*bg*/) {}
        virtual void OnBattlegroundEnd(BattleGround* /*bg*/, uint32 /*winner*/) {}
};

class GroupScript : public ScriptObject
{
    protected:
        explicit GroupScript(char const* name) : ScriptObject(name) { ScriptRegistry<GroupScript>::AddScript(this); }
    public:
        virtual void OnAddMember(Group* /*group*/, ObjectGuid /*guid*/) {}
        virtual void OnRemoveMember(Group* /*group*/, ObjectGuid /*guid*/, uint8 /*method*/) {}
        virtual void OnChangeLeader(Group* /*group*/, ObjectGuid /*newLeaderGuid*/, ObjectGuid /*oldLeaderGuid*/) {}
        virtual void OnDisband(Group* /*group*/) {}
        virtual void OnLootRollStarted(Group* /*group*/, ObjectGuid const& /*target*/, uint32 /*itemSlot*/, uint32 /*itemId*/) {}
};

class GuildScript : public ScriptObject
{
    protected:
        explicit GuildScript(char const* name) : ScriptObject(name) { ScriptRegistry<GuildScript>::AddScript(this); }
    public:
        virtual void OnAddMember(Guild* /*guild*/, Player* /*player*/, uint8& /*rank*/) {}
        virtual void OnRemoveMember(Guild* /*guild*/, Player* /*player*/, bool /*isDisbanding*/, bool /*isKicked*/) {}
        virtual void OnCreate(Guild* /*guild*/, Player* /*leader*/, std::string const& /*name*/) {}
        virtual void OnDisband(Guild* /*guild*/) {}
        virtual void OnGuildInvite(Player* /*invited*/) {}
};

class MailScript : public ScriptObject
{
    protected:
        explicit MailScript(char const* name) : ScriptObject(name) { ScriptRegistry<MailScript>::AddScript(this); }
    public:
        virtual void OnBeforeMailDraftSendMailTo(MailDraft* /*mailDraft*/, MailReceiver const& /*receiver*/, MailSender const& /*sender*/) {}
};

class MovementHandlerScript : public ScriptObject
{
    protected:
        explicit MovementHandlerScript(char const* name) : ScriptObject(name) { ScriptRegistry<MovementHandlerScript>::AddScript(this); }
    public:
        virtual void OnPlayerMove(Player* /*player*/) {}
};

class OutdoorPvPScript : public ScriptObject
{
    protected:
        explicit OutdoorPvPScript(char const* name) : ScriptObject(name) { ScriptRegistry<OutdoorPvPScript>::AddScript(this); }
    public:
        bool IsDatabaseBound() const override { return true; }
};

class PetScript : public ScriptObject
{
    protected:
        explicit PetScript(char const* name) : ScriptObject(name) { ScriptRegistry<PetScript>::AddScript(this); }
    public:
        virtual void OnPetAddToWorld(Pet* /*pet*/) {}
        virtual void OnPetRemoveFromWorld(Pet* /*pet*/) {}
};

class TicketScript : public ScriptObject
{
    protected:
        explicit TicketScript(char const* name) : ScriptObject(name) { ScriptRegistry<TicketScript>::AddScript(this); }
    public:
        virtual void OnTicketCreate(uint32 /*ticketId*/) {}
        virtual void OnTicketClose(uint32 /*ticketId*/) {}
        virtual void OnTicketResolve(uint32 /*ticketId*/) {}
};

#endif
