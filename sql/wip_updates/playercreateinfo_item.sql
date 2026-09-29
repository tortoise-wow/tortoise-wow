-- Fix starter throwing weapons after conversion to single durable items.
-- Affected items:
--   2947 Small Throwing Knife
--   3111 Crude Throwing Axe
--
-- These items are stackable=1 and durability-based, so starter quantities
-- must be 1 instead of the legacy 100/200 values.

UPDATE `playercreateinfo_item`
SET `amount` = 1
WHERE (`race`, `class`, `itemid`) IN (
    (1,  4, 2947),
    (2,  4, 3111),
    (3,  4, 3111),
    (4,  4, 2947),
    (5,  4, 2947),
    (7,  4, 2947),
    (8,  1, 3111),
    (8,  4, 3111),
    (9,  4, 2947),
    (10, 4, 2947)
);
