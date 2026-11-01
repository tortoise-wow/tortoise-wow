#pragma once
// Executes the production .inc implementations with deterministic object/SQL boundaries.
// Full-server compilation validates the real declarations; dispatcher ACK/order tests
// are separate. No database, server process, credentials, or copied save logic is used.
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
enum ItemUpdateState { ITEM_UNCHANGED, ITEM_CHANGED, ITEM_NEW, ITEM_REMOVED, ITEM_STASHED };
enum ItemLootUpdateState { ITEM_LOOT_NONE, ITEM_LOOT_TEMPORARY, ITEM_LOOT_UNCHANGED,
    ITEM_LOOT_CHANGED, ITEM_LOOT_NEW, ITEM_LOOT_REMOVED };
enum QuestUpdateState { QUEST_NEW, QUEST_CHANGED, QUEST_UNCHANGED, QUEST_DELETED };
enum SkillUpdateState { SKILL_UNCHANGED, SKILL_CHANGED, SKILL_NEW, SKILL_DELETED };
enum PlayerSpellState { PLAYERSPELL_UNCHANGED, PLAYERSPELL_CHANGED, PLAYERSPELL_NEW, PLAYERSPELL_REMOVED };
enum QuestStatus { QUEST_STATUS_NONE, QUEST_STATUS_INCOMPLETE, QUEST_STATUS_COMPLETE };
#define MANGOS_ASSERT(value) assert(value)
#define IN_MILLISECONDS 1000
#define PLAYER_SKILL_VALUE_INDEX(position) (100 + (position))
#define SKILL_VALUE(value) uint16((value) & 0xffff)
#define SKILL_MAX(value) uint16((value) >> 16)
#ifdef CORE_AUTO_REWARD_TEST
#include "../../src/framework/Utilities/EventProcessor.h"
#include "../../src/framework/Utilities/EventProcessor.cpp"
#endif
enum EnchantmentSlot { PERM_ENCHANTMENT_SLOT, MAX_ENCHANTMENT_SLOT = 7 };
constexpr uint8 BUYBACK_SLOT_START = 69, BUYBACK_SLOT_END = 81, INVENTORY_SLOT_BAG_0 = 0;
constexpr uint32 MAX_ITEM_PROTO_SPELLS = 5, ITEM_DYNFLAG_WRAPPED = 8;
enum Fields { ITEM_FIELD_FLAGS, ITEM_FIELD_DURATION, ITEM_FIELD_DURABILITY,
    ITEM_FIELD_ITEM_TEXT_ID, ITEM_FIELD_CREATOR, ITEM_FIELD_GIFTCREATOR };
constexpr int CHEAT_ACTION_INFO_LOG = 0;

struct Guid
{
    uint32 value = 0;
    uint32 GetCounter() const { return value; }
    std::string GetString() const { return std::to_string(value); }
    operator uint32() const { return value; }
};
struct RecordedSql { std::string text; std::vector<std::string> values; };
struct SqlStatementID {};
struct TestDatabase;
struct SqlStatement
{
    TestDatabase* db;
    RecordedSql sql;
    template<class T> void add(T value) { std::ostringstream out; out << +value; sql.values.push_back(out.str()); }
    void addUInt32(uint32 v) { add(v); }
    void addUInt16(uint16 v) { add(v); }
    void addUInt64(uint64 v) { add(v); }
    void addUInt8(uint8 v) { add(v); }
    void addInt32(int32_t v) { add(v); }
    void addString(std::string const& v) { sql.values.push_back(v); }
    bool Execute();
    bool DirectExecute();
    template<class... Args> bool PExecute(Args... args) { (add(args), ...); return Execute(); }
};
struct TestDatabase
{
    bool fault = false, owned = true;
    std::set<uint32> participants {1, 2};
    std::vector<RecordedSql> queued;
    size_t attempts = 0, failAt = 0;
    bool HasOrderedPersistenceFault() const { return fault; }
    bool InOwnedTransaction() const { return owned; }
    bool IsOwnedTransactionParticipant(uint32 guid) const { return owned && participants.count(guid); }
    void MarkOrderedPersistenceFault() { fault = true; }
    SqlStatement CreateStatement(SqlStatementID&, char const* text) { return {this, {text, {}}}; }
    bool Enqueue(RecordedSql const& sql)
    {
        ++attempts;
        if (fault || (failAt && attempts == failAt)) return false;
        queued.push_back(sql);
        return true;
    }
    bool PExecute(char const* format, ...)
    {
        char buffer[512];
        va_list args; va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args); va_end(args);
        return Enqueue({buffer, {}});
    }
} CharacterDatabase;
bool SqlStatement::Execute() { bool ok = db->Enqueue(sql); sql.values.clear(); return ok; }
bool SqlStatement::DirectExecute() { assert(!db->owned); return Execute(); }

struct TestLog { template<class... Args> void outError(char const*, Args...) {} } sLog;
struct TestObjectMgr
{
#ifdef CORE_AUTO_REWARD_TEST
    struct Quest
    {
        bool automatic = true;
        bool HasQuestFlag(int) const { return automatic; }
    };
    std::map<uint32,Quest> quests;
    Quest const* GetQuestTemplate(uint32 id) const
    { auto found=quests.find(id); return found==quests.end() ? nullptr : &found->second; }
#endif
    std::set<uint32> transmogs;
    size_t invalidations = 0;
    bool GetItemTransmogrifyTemplate(uint32 id) const { return transmogs.count(id) != 0; }
    void DeleteItemTransmogrifyTemplate(uint32 id, bool saveToDB = true)
    {
        if (!transmogs.erase(id)) return;
        ++invalidations;
        if (saveToDB) CharacterDatabase.PExecute("DELETE FROM `item_transmogs` WHERE `ID`=%u", id);
    }
} sObjectMgr;
struct QuestItem {};
struct LootItem { uint32 itemid = 42; uint8 count = 2; int32_t randomPropertyId = -7; bool is_blocked = false; };
struct TestLoot
{
    uint32 gold = 0;
    std::vector<LootItem> items;
    size_t GetMaxSlotInLootFor(uint32) const { return items.size(); }
    LootItem* LootItemInSlot(size_t slot, uint32, QuestItem** qitem) { *qitem = nullptr; return &items[slot]; }
};
class Player;
using RepListID = uint32;
struct FactionState { uint32 ID, Flags; int32_t Standing; bool needSave; };
class ReputationMgr
{
public:
    explicit ReputationMgr(Player* player) : m_player(player) {}
    struct SaveBatch { ReputationMgr* owner = nullptr; std::vector<RepListID> factions; bool prepared = false; };
    Player* m_player;
    std::map<RepListID, FactionState> m_factions;
    void SaveToDB();
    bool SaveToDB(SaveBatch*);
    bool AppendSaveToDB(SaveBatch&);
    bool CompleteSaveToDB(SaveBatch&);
};
// Honor internals have separate production-body tests. Here it is a checked
// participant/transport boundary for full player orchestration and failures.
class HonorMgr
{
public:
    struct SaveBatch { bool prepared = false; };
    bool AppendSaveToDB(SaveBatch& batch)
    { batch.prepared = CharacterDatabase.Enqueue({"HONOR_TRANSPORT_BOUNDARY", {}}); return batch.prepared; }
    bool CompleteSaveToDB(SaveBatch& batch)
    { if(!batch.prepared) return false; batch.prepared=false; return true; }
};
struct TestWorld
{
    bool stopped = false;
    uint64 GetGameTime() const { return 12345; }
    bool IsStopped() const { return stopped; }
} sWorld;
struct QuestStatusData
{
    uint8 m_status = 1;
    bool m_rewarded = true, m_explored = false;
    uint32 m_timer = 3000, m_reward_choice = 42;
    std::array<uint32,4> m_creatureOrGOcount {}, m_itemcount {};
    QuestUpdateState uState;
};
struct SkillStatusData { uint8 pos; SkillUpdateState uState; };
struct PlayerSpell
{
    PlayerSpellState state : 8;
    bool active : 1;
    bool dependent : 1;
    bool disabled : 1;
};
using QuestStatusMap = std::map<uint32, QuestStatusData>;
using SkillStatusMap = std::unordered_map<uint32, SkillStatusData>;
using PlayerSpellMap = std::unordered_map<uint32, PlayerSpell>;
struct TestItemLog { uint32 guidLow=1, entry=2, count=3, action=4; uint64 timestamp=5; };
class Bag;
class Item
{
public:
    static size_t deleted;
    uint32 guid, entry = 100, count = 1, transmog = 0;
    Guid owner;
    Player* player;
    std::array<uint32, 6> fields {};
    ItemUpdateState uState;
    int16_t uQueuePos = -1;
    ItemLootUpdateState m_lootState = ITEM_LOOT_NONE;
    uint8 slot = 0;
    Bag* container = nullptr;
    bool inWorld = false, bag = false, generatedLoot = false;
    TestLoot loot;
    Item(uint32 id, Player* p, ItemUpdateState state, uint8 position);
    virtual ~Item() { ++deleted; }
    uint32 GetGUIDLow() const { return guid; }
    uint32 GetEntry() const { return entry; }
    uint32 GetCount() const { return count; }
    Guid GetOwnerGuid() const { return owner; }
    std::string GetGuidStr() const { return std::to_string(guid); }
    Guid GetGuidValue(uint32 field) const { return {fields[field]}; }
    uint32 GetUInt32Value(uint32 field) const { return fields[field]; }
    bool HasFlag(uint32 field, uint32 mask) const { return (fields[field] & mask) != 0; }
    int32_t GetSpellCharges(uint8) const { return -3; }
    uint32 GetEnchantmentId(EnchantmentSlot) const { return 101; }
    uint32 GetEnchantmentDuration(EnchantmentSlot) const { return 55; }
    uint32 GetEnchantmentCharges(EnchantmentSlot) const { return 2; }
    void SetEnchantmentDuration(EnchantmentSlot, uint32) { SetState(ITEM_CHANGED); }
    uint16 GetItemRandomPropertyId() const { return 13; }
    uint32 GetTransmogrification() const { return transmog; }
    bool HasSavedLoot() const { return m_lootState != ITEM_LOOT_NONE && m_lootState != ITEM_LOOT_NEW && m_lootState != ITEM_LOOT_TEMPORARY; }
    bool IsInWorld() const { return inWorld; }
    void RemoveFromWorld() { inWorld = false; }
    ItemUpdateState GetState() const { return uState; }
    void FSetState(ItemUpdateState state) { uState = state; }
    void SetLootState(ItemLootUpdateState state)
    {
        assert(state == ITEM_LOOT_UNCHANGED);
        m_lootState = m_lootState == ITEM_LOOT_REMOVED ? ITEM_LOOT_NONE : ITEM_LOOT_UNCHANGED;
    }
    bool IsInUpdateQueue() const { return uQueuePos != -1; }
    uint16 GetQueuePos() const { return uQueuePos; }
    Player* GetOwner() const { return player; }
    void SetState(ItemUpdateState, Player* = nullptr);
    void AddToUpdateQueueOf(Player*);
    void RemoveFromUpdateQueueOf(Player*);
    void SaveToDB(bool direct = false);
    bool AppendSaveToDB();
    bool AppendSaveToDB(bool direct, bool deferred);
    void CompleteSaveToDB();
    void CompleteSaveToDB(bool deferred);
    uint8 GetSlot() const { return slot; }
    void SetSlot(uint8 position) { slot = position; }
    void SetContainer(Bag* value) { container = value; }
    Bag* GetContainer() const { return container; }
    uint8 GetBagSlot() const;
    bool IsBag() const { return bag; }
    Bag* ToBag();
};
size_t Item::deleted = 0;
class Bag : public Item
{
public:
    bool empty = true;
    Bag(uint32 id, Player* p, ItemUpdateState state, uint8 position) : Item(id, p, state, position) { bag = true; }
    bool IsEmpty() const { return empty; }
};
uint8 Item::GetBagSlot() const { return container ? container->GetSlot() : INVENTORY_SLOT_BAG_0; }
Bag* Item::ToBag() { return static_cast<Bag*>(this); }
struct WorldPacket
{
    uint16 opcode;
    std::vector<uint8> bytes;
};
struct Group
{
    std::vector<WorldPacket> broadcasts;
    void BroadcastPacket(WorldPacket* packet, bool) { broadcasts.push_back(*packet); }
};
struct TestSession
{
    size_t anticheatCalls = 0;
    std::vector<WorldPacket> packets;
    void SendPacket(WorldPacket const* packet) { packets.push_back(*packet); }
    void ProcessAnticheatAction(char const*, char const*, int) { ++anticheatCalls; }
};
struct Duration { Item* item; EnchantmentSlot slot; uint32 leftduration; };
class Player
{
public:
    uint32 guid, money = 0;
    bool m_DbSaveDisabled = false, m_itemUpdateQueueBlocked = false;
    std::array<Item*, 128> m_items {};
    std::vector<Item*> m_itemUpdateQueue;
    std::vector<Duration> m_enchantDuration;
    TestSession session;
    Group* group = nullptr;
    ReputationMgr m_reputationMgr;
    HonorMgr m_honorMgr;
    QuestStatusMap mQuestStatus;
    SkillStatusMap mSkillStatus;
    PlayerSpellMap m_spells;
    std::map<uint32,uint16> m_mForgottenSkills;
    std::map<uint32,uint32> skillValues;
    std::map<uint32,std::string> m_variables;
    std::vector<TestItemLog> m_unsavedItemLogs;
    bool teleported = false;
#ifdef CORE_AUTO_REWARD_TEST
    std::set<uint32> m_pendingAutoQuestRewards;
    EventProcessor m_Events;
    bool canComplete = true;
    size_t rewards = 0;
    void CompleteQuest(uint32);
    void _ScheduleAutoQuestRewardRetry(uint32);
    QuestStatus GetQuestStatus(uint32 id) const
    { auto found=mQuestStatus.find(id); return found==mQuestStatus.end() ? QUEST_STATUS_NONE : QuestStatus(found->second.m_status); }
    void SetQuestStatus(uint32 id, QuestStatus status) { mQuestStatus[id].m_status = status; }
    bool CanCompleteQuest(uint32 id) const { return canComplete && GetQuestStatus(id) == QUEST_STATUS_INCOMPLETE; }
    uint16 FindQuestSlot(uint32) const { return 25; }
    void SetQuestSlotState(uint16, int) {}
    bool RewardQuest(TestObjectMgr::Quest const*, uint32, Player*, bool) { ++rewards; return true; }
#endif
    explicit Player(uint32 id) : guid(id), m_reputationMgr(this) {}
    struct InventorySaveBatch { Player* owner = nullptr; std::vector<Item*> queue; bool prepared = false; };
    struct PlayerSaveBatch
    {
        Player* owner = nullptr;
        InventorySaveBatch inventory;
        std::vector<std::pair<uint32,QuestUpdateState>> quests;
        std::vector<std::pair<uint32,SkillUpdateState>> skills;
        std::vector<std::pair<uint32,PlayerSpellState>> spells;
        ReputationMgr::SaveBatch reputation;
        HonorMgr::SaveBatch honor;
        size_t itemLogCount = 0;
        bool prepared = false;
    };
    uint32 GetGUIDLow() const { return guid; }
    Guid GetObjectGuid() const { return {guid}; }
    uint32 GetMoney() const { return money; }
    uint32 GetUInt32Value(uint32 field) const { return skillValues.at(field); }
    bool IsInventorySaveEnabled() const { return !m_DbSaveDisabled && !m_itemUpdateQueueBlocked; }
    bool IsBeingTeleportedFar() const { return teleported; }
    bool _AppendCharacterRowToDB(bool)
    { return CharacterDatabase.Enqueue({"CHARACTER_ROW_TRANSPORT_BOUNDARY", {}}); }
    std::string GetGuidStr() const { return std::to_string(guid); }
    TestSession* GetSession() const { return const_cast<TestSession*>(&session); }
    Group* GetGroup() { return group; }
    Group const* GetGroup() const { return group; }
    void _SendRewardPacket(WorldPacket, bool broadcast = false) const;
    Item* GetItemByPos(uint8 bagSlot, uint8 position) const
    { return bagSlot == INVENTORY_SLOT_BAG_0 && position < BUYBACK_SLOT_START ? m_items[position] : nullptr; }
    bool AppendInventoryAndGoldToDB(InventorySaveBatch&);
    bool CompleteInventoryAndGoldSave(InventorySaveBatch&);
    bool AppendFullPlayerSave(PlayerSaveBatch&);
    bool CompleteFullPlayerSave(PlayerSaveBatch&);
    bool _SaveQuestStatus(PlayerSaveBatch* = nullptr);
    bool _SaveSkills(PlayerSaveBatch* = nullptr);
    bool _SaveSpells(PlayerSaveBatch* = nullptr);
    bool _SaveAuras() { return CharacterDatabase.Enqueue({"AURA_TRANSPORT_BOUNDARY", {}}); }
    void _SaveInventory();
    bool _SaveInventory(InventorySaveBatch*);
};
Item::Item(uint32 id, Player* p, ItemUpdateState state, uint8 position)
    : guid(id), owner{p->guid}, player(p), uState(state), slot(position) {}

#include "../../src/game/Objects/ItemUpdateQueue.inc"
#include "../../src/game/Objects/ItemSave.inc"
#include "../../src/game/Objects/PlayerInventorySave.inc"
