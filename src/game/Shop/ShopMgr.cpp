/*
 * Copyright (C) 2020-2021 Turtle WoW
 *
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

#include "ShopMgr.h"
#include "World.h"
#include "WorldPlayerTasks.h"
#include "ObjectMgr.h"
#include "ObjectGuid.h"
#include "Creature.h"
#include "Object.h"
#include "PoolManager.h"
#include "Language.h"
#include "Log.h"
#include "MapManager.h"
#include "BattleGroundMgr.h"
#include "MassMailMgr.h"
#include "SpellMgr.h"
#include "Policies/SingletonImp.h"
#include "Database/DatabaseEnv.h"
#include "Database/DatabaseImpl.h"
#include "Player.h"
#include "Chat.h"

ShopMgr sShopMgr;

static char const* shopPrefix = "TW_SHOP";
class ShopSendBalanceTask

{
public:
    ShopSendBalanceTask(uint32 accountId, int32 balance) : m_accountId(accountId), m_balance(balance)
    {
    }
    void operator ()()
    {
        if (WorldSession* sess = sWorld.FindSession(m_accountId))
        {
            Player* player = sess->GetPlayer();
            if (!player || !player->IsInWorld())
                return;

            if (m_balance < 0)
                player->GetSession()->KickPlayer();
            else
                player->SendAddonMessage(shopPrefix, "Balance:" + std::to_string(m_balance));

        }
    }
    uint32 m_accountId;
    int32 m_balance;
};

class ShopSendBuyResultTask
{
public:
    ShopSendBuyResultTask(uint32 accountId, char const* message) : m_accountId(accountId), m_message(message)
    {
    }
    void operator ()()
    {
        if (WorldSession* sess = sWorld.FindSession(m_accountId))
        {
            Player* player = sess->GetPlayer();
            if (!player || !player->IsInWorld())
                return;

            player->SendAddonMessage(shopPrefix, "BuyResult:" + std::string(m_message));
        }
    }
    uint32 m_accountId;
    char const* m_message;
};

class ShopSendItemTask
{
public:
    ShopSendItemTask(uint32 accountId, uint32 guidLow, uint32 itemId, uint32 shopId, uint32 price) : m_accountId(accountId), m_guidLow(guidLow), m_itemId(itemId), m_shopId(shopId), m_price(price)
    {
    }
    void operator ()()
    {
        WorldSession* sess = sWorld.FindSession(m_accountId);
        if (!sess)
        {
            RefundPurchase();
            return;
        }
        
        Player* player = sess->GetPlayer();
        if (!player || !player->IsInWorld() || player->GetGUIDLow() != m_guidLow)
        {
            RefundPurchase();
            return;
        }

        int32 count = 1;
        ItemPosCountVec dest;
        uint32 noSpaceForCount = 0;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, m_itemId, count, &noSpaceForCount);
        if (msg != EQUIP_ERR_OK)
            count -= noSpaceForCount;

        if (count == 0 || dest.empty())
        {
            RefundPurchase();
            player->SendAddonMessage(shopPrefix, "BuyResult:bagsfulloralreadyhaveitem");
            return;
        }

        Item* item = player->StoreNewItem(dest, m_itemId, true, Item::GenerateItemRandomPropertyId(m_itemId));
        player->SendNewItem(item, count, false, true);
        player->SendAddonMessage(shopPrefix, "BuyResult:ok");
    }
    void RefundPurchase()
    {
        sLog.outError("Shop purchase failed, refunding! (Account %u Guid %u Item %u Shop %u Price %u)", m_accountId, m_guidLow, m_itemId, m_shopId, m_price);
        auto entry = sObjectMgr.GetShopLogEntry(m_shopId);
        MANGOS_ASSERT(entry);
        MANGOS_ASSERT(!entry->refunded);
        entry->refunded = true;
        LoginDatabase.PExecute("UPDATE `shop_coins` SET `coins` = (`coins`+%u) WHERE `id` = %u", m_price, m_accountId);
        LoginDatabase.PExecute("UPDATE `shop_logs` SET `refunded`=1 WHERE `id`=%u", m_shopId);
    }
    uint32 m_accountId;
    uint32 m_guidLow;
    int32 m_itemId;
    uint32 m_shopId;
    uint32 m_price;
};

bool ShopMgr::RequestBalance(uint32 accountId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto const& itr : m_pendingRequests)
    {
        if (itr.accountId == accountId && itr.itemId == 0)
            return false;
    }
    m_pendingRequests.push_back(ShopRequest(accountId, 0, 0));
    return true;
}

bool ShopMgr::RequestPurchase(uint32 accountId, uint32 guidLow, uint32 itemId)
{
    if (itemId == 92010) // egg can't be bought by level 1
    {
        auto session = sWorld.FindSession(accountId);

        if (session && session->GetPlayer() && session->GetPlayer()->GetLevel() == 1)
        {
            session->SendNotification("You can't buy this item at level 1.");
            return true;
        }

        if (session && session->GetPlayer() && session->GetPlayer()->GetLevel() < 10 && session->GetPlayer()->IsHardcore())
        {
            session->SendNotification("You can't buy this item if you are a Hardcore player under level 10.");
            return true;
        }

        if (session && session->GetPlayer() && session->GetPlayer()->HasItemCount(50745, 1, false))
        {
            session->SendNotification("You can't buy this item because of your glyph.");
            return true;
        }
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto const& itr : m_pendingRequests)
    {
        if (itr.accountId == accountId && itr.guidLow == guidLow && itr.itemId == itemId)
            return false;
    }
    m_pendingRequests.push_back(ShopRequest(accountId, guidLow, itemId));
    return true;
}

void ShopMgr::ProcessRequestsWorker()
{
    LoginDatabase.ThreadStart();
    thread_name("ShopMgr");
    while (!sWorld.IsStopped())
    {
        std::vector<ShopRequest> requests;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::swap(requests, m_pendingRequests);
        }
        
        for (auto const& itr : requests)
        {
            if (itr.itemId)
                BuyItem(itr.accountId, itr.guidLow, itr.itemId);
            else
                sWorld.AddAsyncTask({ ShopSendBalanceTask(itr.accountId, GetBalance(itr.accountId)) });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    LoginDatabase.ThreadEnd();
}

int32 ShopMgr::GetBalance(uint32 accountId)
{
    std::unique_ptr<QueryResult> result(LoginDatabase.PQuery("SELECT `coins` FROM `shop_coins` WHERE `id` = '%u'", accountId));

    int32 balance = 0;
    if (result)
    {
        Field* fields = result->Fetch();
        balance = fields[0].GetInt32();
    }
    else
    {
        LoginDatabase.PExecute("INSERT INTO `shop_coins` (`id`, `coins`) VALUES ('%u', 0)", accountId);
        return balance;
    }

    return balance;
}

void ShopMgr::BuyItem(uint32 accountId, uint32 guidLow, uint32 itemId)
{
    auto shopEntry = sObjectMgr.GetShopEntryInfo(itemId);

    if (!shopEntry)
    {
        sWorld.AddAsyncTask({ ShopSendBuyResultTask(accountId, "itemnotinshop") });
        return;
    }

    uint32 price = shopEntry->Price;
    int32 count = 1;
    int64 coins = GetBalance(accountId);

    if (coins > 0)
    {
        int64 newBalance = coins - price;

        if (newBalance >= 0 && newBalance < INT_MAX)
        {
            uint32 shopId = sObjectMgr.NextShopLogEntry();

            bool successTransaction =
                LoginDatabase.DirectPExecute("UPDATE `shop_coins` SET `coins` = %i WHERE `id` = %u", newBalance, accountId) &&
                LoginDatabase.DirectPExecute("INSERT INTO `shop_logs` (`id`, `time`, `guid`, `account`, `item`, `price`, `refunded`, `realm_id`) VALUES (%u, NOW(), %u, %u, %u, %u, 0, %u)", shopId, guidLow, accountId, itemId, price
                , realmID);

            if (!successTransaction)
            {
                sWorld.AddAsyncTask({ ShopSendBuyResultTask(accountId, "dberrorcantprocess") });
                return;
            }

            if (GetBalance(accountId) != newBalance)
            {
                sWorld.AddAsyncTask({ ShopSendBuyResultTask(accountId, "dberrorcantprocess") });
                return;
            }

            auto entry = new ShopLogEntry{
                shopId,
                GetCurrentTimeString(),
                accountId,
                guidLow,
                itemId,
                price,
                false,
                (uint32)time(nullptr)
            };

            sObjectMgr.GetShopLogEntries(accountId).push_back(entry);
            sObjectMgr.AddShopLogEntry(shopId, entry);

        WorldPlayerTasks::Add(ShopSendItemTask(accountId, guidLow, itemId, shopId, price));
            return;
        }
        else
        {
            sWorld.AddAsyncTask({ ShopSendBuyResultTask(accountId, "notenoughtokens") });
            return;
        }
    }
    else
    {
        sWorld.AddAsyncTask({ ShopSendBuyResultTask(accountId, "notenoughtokens") });
        return;
    }
}
