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

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
enum ItemUpdateState { ITEM_UNCHANGED, ITEM_CHANGED, ITEM_NEW, ITEM_REMOVED, ITEM_STASHED };
enum ItemLootUpdateState { ITEM_LOOT_NONE, ITEM_LOOT_TEMPORARY, ITEM_LOOT_UNCHANGED,
    ITEM_LOOT_CHANGED, ITEM_LOOT_NEW, ITEM_LOOT_REMOVED };
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
struct TestSession
{
    size_t anticheatCalls = 0;
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
    explicit Player(uint32 id) : guid(id) {}
    struct InventorySaveBatch { Player* owner = nullptr; std::vector<Item*> queue; bool prepared = false; };
    uint32 GetGUIDLow() const { return guid; }
    Guid GetObjectGuid() const { return {guid}; }
    uint32 GetMoney() const { return money; }
    std::string GetGuidStr() const { return std::to_string(guid); }
    TestSession* GetSession() { return &session; }
    Item* GetItemByPos(uint8 bagSlot, uint8 position) const
    { return bagSlot == INVENTORY_SLOT_BAG_0 && position < BUYBACK_SLOT_START ? m_items[position] : nullptr; }
    bool AppendInventoryAndGoldToDB(InventorySaveBatch&);
    bool CompleteInventoryAndGoldSave(InventorySaveBatch&);
    void _SaveInventory();
    bool _SaveInventory(InventorySaveBatch*);
};
Item::Item(uint32 id, Player* p, ItemUpdateState state, uint8 position)
    : guid(id), owner{p->guid}, player(p), uState(state), slot(position) {}

#include "../../src/game/Objects/ItemUpdateQueue.inc"
#include "../../src/game/Objects/ItemSave.inc"
#include "../../src/game/Objects/PlayerInventorySave.inc"

void Reset()
{
    CharacterDatabase = TestDatabase{};
    sObjectMgr = TestObjectMgr{};
    Item::deleted = 0;
}
Item* Add(Player& p, uint32 id, ItemUpdateState state, uint8 slot)
{
    Item* item = new Item(id, &p, state, slot);
    if (state != ITEM_REMOVED && state != ITEM_STASHED) p.m_items[slot] = item;
    item->AddToUpdateQueueOf(&p);
    return item;
}
size_t CountSql(std::string const& needle)
{
    return std::count_if(CharacterDatabase.queued.begin(), CharacterDatabase.queued.end(),
        [&](RecordedSql const& sql) { return sql.text.find(needle) != std::string::npos; });
}

void CompleteBatchPreservesEverythingUntilAcknowledged()
{
    Reset(); Player a(1), b(2); a.money = 123; b.money = 456;
    Item* fresh = Add(a, 10, ITEM_NEW, 1);
    Item* changed = Add(b, 20, ITEM_CHANGED, 2);
    changed->fields[ITEM_FIELD_FLAGS] = ITEM_DYNFLAG_WRAPPED;
    changed->m_lootState = ITEM_LOOT_CHANGED;
    changed->loot.gold = 7; changed->loot.items.push_back({});
    Item* removed = Add(a, 30, ITEM_REMOVED, 3);
    removed->transmog = 900; removed->fields[ITEM_FIELD_ITEM_TEXT_ID] = 901;
    removed->fields[ITEM_FIELD_FLAGS] = ITEM_DYNFLAG_WRAPPED;
    removed->m_lootState = ITEM_LOOT_UNCHANGED;
    sObjectMgr.transmogs.insert(900);
    Player::InventorySaveBatch pa, pb;
    assert(a.AppendInventoryAndGoldToDB(pa) && b.AppendInventoryAndGoldToDB(pb));
    assert(fresh->uState == ITEM_NEW && changed->uState == ITEM_CHANGED);
    assert(changed->m_lootState == ITEM_LOOT_CHANGED && removed->uState == ITEM_REMOVED);
    assert(Item::deleted == 0 && sObjectMgr.invalidations == 0);
    assert(a.m_itemUpdateQueue.size() == 2 && b.m_itemUpdateQueue.size() == 1);
    assert(CountSql("UPDATE characters SET money") == 2);
    assert(CountSql("character_gifts") == 2 && CountSql("item_text") == 1);
    assert(CountSql("DELETE FROM `item_transmogs`") == 1);
    assert(CountSql("item_loot") == 4);
    size_t statements = CharacterDatabase.queued.size();
    // The owning transaction's successful synchronous ACK precedes these calls.
    assert(a.CompleteInventoryAndGoldSave(pa) && b.CompleteInventoryAndGoldSave(pb));
    assert(fresh->uState == ITEM_UNCHANGED && changed->uState == ITEM_UNCHANGED);
    assert(changed->m_lootState == ITEM_LOOT_UNCHANGED);
    assert(Item::deleted == 1 && sObjectMgr.invalidations == 1);
    assert(CharacterDatabase.queued.size() == statements); // completion emits NO SQL
    assert(a.m_itemUpdateQueue.empty() && b.m_itemUpdateQueue.empty());
    assert(!a.CompleteInventoryAndGoldSave(pa));
    delete fresh; delete changed;
}

void EveryAppendFailureKeepsDirtyObjects()
{
    size_t total = 0;
    for (size_t failure = 0; failure <= total || failure == 0; ++failure)
    {
        Reset(); CharacterDatabase.failAt = failure;
        Player a(1), b(2);
        Item* fresh = Add(a, 10, ITEM_NEW, 1);
        Item* changed = Add(b, 20, ITEM_CHANGED, 2);
        changed->fields[ITEM_FIELD_FLAGS] = ITEM_DYNFLAG_WRAPPED;
        changed->m_lootState = ITEM_LOOT_CHANGED;
        changed->loot.gold = 7; changed->loot.items.push_back({});
        Item* removed = Add(a, 30, ITEM_REMOVED, 3);
        removed->transmog = 900; removed->fields[ITEM_FIELD_ITEM_TEXT_ID] = 901;
        removed->fields[ITEM_FIELD_FLAGS] = ITEM_DYNFLAG_WRAPPED;
        removed->m_lootState = ITEM_LOOT_UNCHANGED;
        sObjectMgr.transmogs.insert(900);
        Player::InventorySaveBatch pa, pb;
        bool ok = a.AppendInventoryAndGoldToDB(pa) && b.AppendInventoryAndGoldToDB(pb);
        if (failure == 0) { assert(ok); total = CharacterDatabase.attempts; }
        else assert(!ok);
        // Rejected staging or a failed/unknown COMMIT never calls Complete.
        assert(fresh->uState == ITEM_NEW && changed->uState == ITEM_CHANGED);
        assert(changed->m_lootState == ITEM_LOOT_CHANGED && removed->uState == ITEM_REMOVED);
        assert(Item::deleted == 0 && sObjectMgr.invalidations == 0);
        assert(a.m_itemUpdateQueue.size() == 2 && b.m_itemUpdateQueue.size() == 1);
        delete fresh; delete changed; delete removed;
    }
    assert(total > 10);
}

void PreflightAndLocalQueueLifecycle()
{
    Reset(); Player a(1);
    Item* stale = Add(a, 10, ITEM_NEW, 1); a.m_items[1] = nullptr;
    Player::InventorySaveBatch bad;
    assert(!a.AppendInventoryAndGoldToDB(bad));
    assert(CharacterDatabase.queued.empty() && a.session.anticheatCalls == 0 && Item::deleted == 0);
    assert(stale->uState == ITEM_NEW && stale->IsInUpdateQueue());
    stale->SetState(ITEM_REMOVED, &a); // real production NEW->REMOVED deletes, nulls queue
    assert(Item::deleted == 1 && a.m_itemUpdateQueue[0] == nullptr);
    Item* buyback = new Item(20, &a, ITEM_UNCHANGED, BUYBACK_SLOT_START);
    a.m_items[BUYBACK_SLOT_START] = buyback;
    Item* queuedBuyback = Add(a, 21, ITEM_CHANGED, 7);
    a.m_items[7] = nullptr;
    a.m_items[BUYBACK_SLOT_START + 1] = queuedBuyback; // stored slot must be normalized too
    Item* stashed = Add(a, 30, ITEM_STASHED, 2);
    a.m_enchantDuration.push_back({buyback, PERM_ENCHANTMENT_SLOT, 23});
    Player::InventorySaveBatch good;
    assert(a.AppendInventoryAndGoldToDB(good));
    assert(buyback->uState == ITEM_CHANGED && buyback->IsInUpdateQueue());
    assert(queuedBuyback->GetSlot() == BUYBACK_SLOT_START + 1);
    assert(a.CompleteInventoryAndGoldSave(good));
    assert(buyback->uState == ITEM_UNCHANGED && !buyback->IsInUpdateQueue());
    assert(stashed->uState == ITEM_STASHED && !stashed->IsInUpdateQueue());
    delete buyback; delete queuedBuyback; delete stashed;

    Reset(); Player b(2);
    Item* duplicate = Add(b, 40, ITEM_CHANGED, 1); b.m_itemUpdateQueue.push_back(duplicate);
    Player::InventorySaveBatch duplicated;
    assert(!b.AppendInventoryAndGoldToDB(duplicated) && CharacterDatabase.queued.empty());
    delete duplicate;
    Reset(); Player c(1);
    Bag* bag = new Bag(50, &c, ITEM_REMOVED, 1); bag->empty = false; bag->AddToUpdateQueueOf(&c);
    Player::InventorySaveBatch nonempty;
    assert(!c.AppendInventoryAndGoldToDB(nonempty) && CharacterDatabase.queued.empty());
    delete bag;

    Reset(); Player d(1);
    Item* moved = Add(d, 60, ITEM_CHANGED, 1); moved->uQueuePos = 100;
    Player::InventorySaveBatch invalidPosition;
    assert(!d.AppendInventoryAndGoldToDB(invalidPosition) && CharacterDatabase.queued.empty());
    delete moved;
}

void NewItemTransferAndMergeQueueLifetime()
{
    Reset(); Player a(1), b(2);
    Item* transfer = Add(a, 10, ITEM_NEW, 1);
    transfer->RemoveFromUpdateQueueOf(&a); a.m_items[1] = nullptr;
    transfer->owner = {2}; transfer->player = &b; transfer->slot = 2; b.m_items[2] = transfer;
    transfer->SetState(ITEM_CHANGED, &b); // production NEW must remain NEW on transfer
    Player::InventorySaveBatch pa, pb;
    assert(a.AppendInventoryAndGoldToDB(pa) && b.AppendInventoryAndGoldToDB(pb));
    assert(transfer->uState == ITEM_NEW && CountSql("INSERT INTO character_inventory") == 1);
    assert(a.CompleteInventoryAndGoldSave(pa) && b.CompleteInventoryAndGoldSave(pb));
    transfer->SetState(ITEM_REMOVED, &b); // admitted NEW now requires a DB tombstone
    assert(Item::deleted == 0 && transfer->uState == ITEM_REMOVED);
    Player::InventorySaveBatch deletion;
    assert(b.AppendInventoryAndGoldToDB(deletion) && b.CompleteInventoryAndGoldSave(deletion));
    assert(Item::deleted == 1 && CountSql("DELETE FROM character_inventory") == 1);

    Reset(); Player source(1), destination(2);
    Item* unsaved = Add(source, 20, ITEM_NEW, 1);
    unsaved->RemoveFromUpdateQueueOf(&source); source.m_items[1] = nullptr;
    unsaved->owner = {2}; unsaved->player = &destination;
    unsaved->SetState(ITEM_REMOVED, &destination); // fully merged unsaved source has no DB row
    assert(Item::deleted == 1 && source.m_itemUpdateQueue[0] == nullptr);
    Item* stack = Add(destination, 21, ITEM_CHANGED, 2); stack->count = 8;
    Player::InventorySaveBatch ps, pd;
    assert(source.AppendInventoryAndGoldToDB(ps) && destination.AppendInventoryAndGoldToDB(pd));
    assert(CountSql("DELETE FROM character_inventory") == 0 && CountSql("INSERT INTO character_inventory") == 0);
    assert(source.CompleteInventoryAndGoldSave(ps) && destination.CompleteInventoryAndGoldSave(pd));
    delete stack;
}

void AdmissionGuardsAndQueueDrift()
{
    for (int guard = 0; guard < 4; ++guard)
    {
        Reset(); Player a(1); Item* item = Add(a, 10, ITEM_CHANGED, 1);
        if (guard == 0) a.m_DbSaveDisabled = true;
        if (guard == 1) a.m_itemUpdateQueueBlocked = true;
        if (guard == 2) CharacterDatabase.fault = true;
        if (guard == 3) CharacterDatabase.participants.clear();
        Player::InventorySaveBatch batch;
        assert(!a.AppendInventoryAndGoldToDB(batch));
        assert(item->uState == ITEM_CHANGED && CharacterDatabase.queued.empty()); delete item;
    }
    Reset(); Player a(1); Item* item = Add(a, 10, ITEM_CHANGED, 1);
    Player::InventorySaveBatch batch;
    assert(a.AppendInventoryAndGoldToDB(batch));
    a.m_itemUpdateQueue.push_back(nullptr);
    assert(!a.CompleteInventoryAndGoldSave(batch) && item->uState == ITEM_CHANGED);
    delete item;
    Reset(); Player b(2); Item* legacy = Add(b, 20, ITEM_REMOVED, 2);
    legacy->SaveToDB(); // legacy consume API cannot steal an owned batch
    assert(CharacterDatabase.fault && Item::deleted == 0 && legacy->uState == ITEM_REMOVED);
    delete legacy;
    Reset(); Player outsider(3); Item* third = Add(outsider, 30, ITEM_REMOVED, 3);
    third->SaveToDB(); // even an unregistered third owner cannot consume in this scope
    assert(CharacterDatabase.fault && Item::deleted == 0 && CharacterDatabase.queued.empty());
    delete third;
}

int main()
{
    CompleteBatchPreservesEverythingUntilAcknowledged();
    EveryAppendFailureKeepsDirtyObjects();
    PreflightAndLocalQueueLifecycle();
    NewItemTransferAndMergeQueueLifetime();
    AdmissionGuardsAndQueueDrift();
    std::cout << "Production inventory/item save tests passed.\n";
}
