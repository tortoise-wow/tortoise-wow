#include "inventory_save_environment.h"

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
