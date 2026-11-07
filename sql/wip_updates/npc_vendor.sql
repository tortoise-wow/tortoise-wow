-- Syndicate Quartermaster Anna Lacroix (80946) - issue #607
--
-- The Syndicate quartermaster sold one item out of thirteen.
--
-- Thirteen items are gated behind reputation with faction 70, the Syndicate, and
-- they read as a designed ladder: item level 30 at Honored, 40 at Revered, 50
-- and 62-63 at the top, with buy prices from 17 silver to 32 gold. Nobody sets
-- buy prices on loot.
--
-- Anna Lacroix (80946, "Syndicate Quartermaster") had exactly one row in
-- npc_vendor - the Syndicate Mask. The other twelve existed in item_template,
-- correctly gated, and were sold by nobody at all. That is an import that
-- stopped early, not a design.
--
-- No condition_id is needed. Player::BuyItemFromVendor checks
-- RequiredReputationFaction and RequiredReputationRank itself and answers
-- BUY_ERR_REPUTATION_REQUIRE, so the rank on each item is the gate. maxcount 0
-- and incrtime 0 mean unlimited stock, matching the row that was already there.
--
-- Slots are numbered in the order the ladder is climbed, so the list reads from
-- the cheapest rank to the highest rather than by item id.

INSERT IGNORE INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `itemflags`, `condition_id`)
VALUES
    -- Honored
    (80946,  2, 55018, 0, 0, 0, 0),   -- Yellowed Mask
    (80946,  3, 55019, 0, 0, 0, 0),   -- Enforcer Guard Belt
    (80946,  4, 55020, 0, 0, 0, 0),   -- Faithful Loop
    -- Revered
    (80946,  5, 55021, 0, 0, 0, 0),   -- Alvelo's Reinforced Bludgeon
    (80946,  6, 55022, 0, 0, 0, 0),   -- Bishop Miranda's Rosary
    -- Rank 6
    (80946,  7, 55023, 0, 0, 0, 0),   -- Hath's Stalwart Defense
    (80946,  8, 55024, 0, 0, 0, 0),   -- Arnella's Silken Trousers
    (80946,  9, 55025, 0, 0, 0, 0),   -- Trand's Lightweight Shoulder Pads
    -- Rank 7
    (80946, 10, 55028, 0, 0, 0, 0),   -- Kavdan's Patient Watch
    (80946, 11, 55029, 0, 0, 0, 0),   -- Valea's Alacrity
    (80946, 12, 55030, 0, 0, 0, 0),   -- Aliden's Prejudice
    (80946, 13, 55031, 0, 0, 0, 0)    -- Pattern: Royal Robes of the Alteraci Princess
ON DUPLICATE KEY UPDATE `slot` = VALUES(`slot`);

-- The mask that was already listed keeps its place at the front.
UPDATE `npc_vendor` SET `slot` = 1 WHERE `entry` = 80946 AND `item` = 81080;
