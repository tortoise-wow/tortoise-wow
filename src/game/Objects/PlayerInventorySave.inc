// Shared production inventory persistence implementation; also compiled by storage tests.

bool Player::AppendInventoryAndGoldToDB(InventorySaveBatch& batch)
{
    if (batch.owner || m_DbSaveDisabled || m_itemUpdateQueueBlocked ||
        CharacterDatabase.HasOrderedPersistenceFault() ||
        !CharacterDatabase.IsOwnedTransactionParticipant(GetGUIDLow()))
        return false;

    batch.owner = this;
    if (!_SaveInventory(&batch))
        return false;

    static SqlStatementID updateGold;
    SqlStatement stmt = CharacterDatabase.CreateStatement(updateGold, "UPDATE characters SET money = ? WHERE guid = ?");
    if (!stmt.PExecute(GetMoney(), GetGUIDLow()))
        return false;

    batch.prepared = true;
    return true;
}

bool Player::CompleteInventoryAndGoldSave(InventorySaveBatch& batch)
{
    // No actor mutation, hooks or yields are allowed from append through this
    // synchronous completion. The DB worker owns SQL values, never these pointers.
    if (batch.owner != this || !batch.prepared || batch.queue != m_itemUpdateQueue)
        return false;

    for (Item* item : batch.queue)
    {
        if (!item)
            continue;
        item->RemoveFromUpdateQueueOf(this);
        item->CompleteSaveToDB();
    }
    m_itemUpdateQueue.clear();
    batch.queue.clear();
    batch.prepared = false;
    return true;
}

void Player::_SaveInventory()
{
    _SaveInventory(nullptr);
}

bool Player::_SaveInventory(InventorySaveBatch* batch)
{
    if (CharacterDatabase.HasOrderedPersistenceFault())
        return false;
    if (!batch && CharacterDatabase.InOwnedTransaction())
    {
        CharacterDatabase.MarkOrderedPersistenceFault();
        return false;
    }

    // Turtle: save buyback items in db too, so they persist through logout
    std::set<Item*> buyBackItems;
    for (uint8 i = BUYBACK_SLOT_START; i < BUYBACK_SLOT_END; ++i)
    {
        Item* item = m_items[i];
        if (!item)
            continue;

        // GetItemByPos intentionally excludes buyback slots. Validate all actual
        // buyback pointers in the deferred path, even if already dirty/queued.
        if (batch)
        {
            if (!buyBackItems.insert(item).second)
                return false;
            item->SetSlot(i);
            item->SetContainer(nullptr);
            if (item->GetState() == ITEM_UNCHANGED)
                item->SetState(ITEM_CHANGED, this);
        }

        if (std::find(m_itemUpdateQueue.begin(), m_itemUpdateQueue.end(), item) == m_itemUpdateQueue.end())
        {
            buyBackItems.insert(item);
            item->SetSlot(i);
            item->SetContainer(nullptr);

            if (batch)
            {
                item->SetState(item->GetState() == ITEM_UNCHANGED ? ITEM_CHANGED : item->GetState(), this);
                if (std::find(m_itemUpdateQueue.begin(), m_itemUpdateQueue.end(), item) == m_itemUpdateQueue.end())
                    return false;
            }
            else
            {
                if (item->GetState() == ITEM_UNCHANGED)
                    item->FSetState(ITEM_CHANGED);
                m_itemUpdateQueue.push_back(item);
            }
        }
    }

    // update enchantment durations
    for (auto& itr : m_enchantDuration)
        itr.item->SetEnchantmentDuration(itr.slot, itr.leftduration);

    // if no changes
    if (batch)
    {
        // Corrupt/stale entries cannot be repaired by callbacks while captured
        // Item pointers are live. Reject before any SQL and keep all dirty state.
        std::set<Item*> seen;
        for (size_t pos = 0; pos < m_itemUpdateQueue.size(); ++pos)
        {
            Item* item = m_itemUpdateQueue[pos];
            if (!item)
                continue;
            if (!seen.insert(item).second || item->GetOwnerGuid() != GetObjectGuid())
                return false;
            if (item->IsInUpdateQueue() && item->GetQueuePos() != pos)
                return false;
            // Deleting a nonempty bag could destroy another captured queue item.
            if (item->GetState() == ITEM_REMOVED && item->IsBag() && !item->ToBag()->IsEmpty())
                return false;
            if (item->GetState() != ITEM_REMOVED && item->GetState() != ITEM_STASHED &&
                buyBackItems.find(item) == buyBackItems.end() &&
                GetItemByPos(item->GetBagSlot(), item->GetSlot()) != item)
                return false;
        }
        batch->queue = m_itemUpdateQueue;
    }

    if (m_itemUpdateQueue.empty()) return true;

    for (auto& item : m_itemUpdateQueue)
    {
        if (!item)
            continue;

        if (!batch && item->GetState() != ITEM_REMOVED && item->GetState() != ITEM_STASHED &&
            buyBackItems.find(item) == buyBackItems.end())
        {
            // Plusieurs tests anti dupli ...
            Item *test = GetItemByPos(item->GetBagSlot(), item->GetSlot());

            if (test == nullptr)
            {
                uint32 bagTestGUID = 0;
                if (Item* test2 = GetItemByPos(INVENTORY_SLOT_BAG_0, item->GetBagSlot()))
                    bagTestGUID = test2->GetGUIDLow();
                // according to the test that was just performed nothing should be in this slot, delete
                static SqlStatementID deleteInventoryAntiDupli;
                SqlStatement stmt = CharacterDatabase.CreateStatement(deleteInventoryAntiDupli, "DELETE FROM character_inventory WHERE bag=? AND slot=? AND guid=?");
                stmt.addUInt32(bagTestGUID);
                stmt.addUInt32(item->GetSlot());
                stmt.addUInt32(GetGUIDLow());
                stmt.Execute();
                // also THIS item should be somewhere else, cheat attempt
                GetSession()->ProcessAnticheatAction("PassiveAnticheat", "_SaveInventory: item not found", CHEAT_ACTION_INFO_LOG);
                item->FSetState(ITEM_REMOVED); // we are IN updateQueue right now, can't use SetState which modifies the queue
                // don't skip, let the switch delete it
            }
            else if (test != item)
            {
                if (item->GetState() != ITEM_NEW) // only for existing items, no dupes
                    item->SaveToDB();
                // ...but do not save position in inventory
                continue;
            }
        }
        if (!batch && item->GetOwnerGuid() != GetObjectGuid())
            GetSession()->ProcessAnticheatAction("PassiveAnticheat", "_SaveInventory: attempting to save not owned item", CHEAT_ACTION_INFO_LOG);

        static SqlStatementID insertInventory ;
        static SqlStatementID updateInventory ;
        static SqlStatementID deleteInventory ;

        Bag *container = item->GetContainer();
        uint32 bag_guid = container ? container->GetGUIDLow() : 0;

        switch (item->GetState())
        {
            case ITEM_NEW:
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(insertInventory, "INSERT INTO character_inventory (guid,bag,slot,item,item_template) VALUES (?, ?, ?, ?, ?)");
                stmt.addUInt32(GetGUIDLow());
                stmt.addUInt32(bag_guid);
                stmt.addUInt8(item->GetSlot());
                stmt.addUInt32(item->GetGUIDLow());
                stmt.addUInt32(item->GetEntry());
                if (!stmt.Execute() && batch)
                    return false;
            }
            break;
            case ITEM_CHANGED:
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(updateInventory, "UPDATE character_inventory SET guid = ?, bag = ?, slot = ?, item_template = ? WHERE item = ?");
                stmt.addUInt32(GetGUIDLow());
                stmt.addUInt32(bag_guid);
                stmt.addUInt8(item->GetSlot());
                stmt.addUInt32(item->GetEntry());
                stmt.addUInt32(item->GetGUIDLow());
                if (!stmt.Execute() && batch)
                    return false;
            }
            break;
            case ITEM_REMOVED:
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(deleteInventory, "DELETE FROM character_inventory WHERE item = ?");
                if (!stmt.PExecute(item->GetGUIDLow()) && batch)
                    return false;
            }
            break;
            case ITEM_UNCHANGED:
                break;
        }

        if (batch)
        {
            if (!item->AppendSaveToDB())
                return false;
        }
        else
            item->SaveToDB();                               // standalone inventory record
    }
    if (!batch)
        m_itemUpdateQueue.clear();
    return true;
}
