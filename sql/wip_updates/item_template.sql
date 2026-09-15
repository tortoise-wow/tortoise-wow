-- Four items carry a disenchant_id they can never use:
--
--     Item (Entry: 1973) has wrong item class (15) for disenchanting, remove disenchanting loot id.
--     Item (Entry: 4143) has wrong item class (9) for disenchanting, remove disenchanting loot id.
--     Item (Entry: 61549) has wrong item class (11) for disenchanting, remove disenchanting loot id.
--     Item (Entry: 9400) has wrong quality (1) for disenchanting, remove disenchanting loot id.
--
-- ObjectMgr clears the id at load, so nothing is disenchantable either way -
-- this only stops the data claiming otherwise.
--
--     1973  Orb of Deception       class 15 Miscellaneous  - not disenchantable
--     4143  Tome of Conjure Food II class 9 Recipe         - not disenchantable
--     61549 Swiftfeather Quiver    class 11 Quiver         - not disenchantable
--     9400  Baelog's Shortbow      weapon, but quality 1   - needs uncommon or better
--
-- 1973 is the clearest case: it is filed under Miscellaneous -> Junk on
-- Turtle's own item database, its tooltip reads "Use: Adds a toy to the
-- player's toy collection", and it lists Disenchant ID: 0
-- (https://octowow.st/db/?item=1973). It is a toy, not a trinket, so there is
-- nothing for enchanting to yield.
--
-- The old value is in the WHERE clause so re-running this cannot overwrite a
-- later correction.
UPDATE `item_template` SET `disenchant_id` = 0
WHERE `entry` = 1973 AND `disenchant_id` = 48;

UPDATE `item_template` SET `disenchant_id` = 0
WHERE `entry` = 4143 AND `disenchant_id` = 45;

UPDATE `item_template` SET `disenchant_id` = 0
WHERE `entry` = 9400 AND `disenchant_id` = 28;

UPDATE `item_template` SET `disenchant_id` = 0
WHERE `entry` = 61549 AND `disenchant_id` = 49;

-- item_template carries a row with entry 0, every column empty:
--
--     Item (Entry: 0) has wrong value in stackable (0), replace by default 1.
--
-- It is the last tuple of the REPLACE in 20260505222630_world.sql ("Updated
-- items to 1.18.1"), written as all NULLs and stored as zeroes - the trailing
-- blank row an export leaves behind.
--
-- Entry 0 is not a valid item id, and nothing references it: every column in
-- tw_world and tw_char that can hold an item id was checked, and no
-- inventory, mail, auction, guild bank, loot or deleted-item row has ever
-- held 0.
--
-- It is not only untidy. ObjectMgr::GetItemPrototype is a plain map lookup
-- with no guard on 0, and this row is in that map, so GetItemPrototype(0)
-- returns a valid pointer to an empty prototype instead of nullptr.
-- Item::CreateItem checks only `count < 1` and then trusts that lookup to
-- reject unknown ids - so while the row is present, a stray 0 anywhere
-- upstream creates a phantom item instead of failing safely. Callers guard
-- the sentinel themselves today (quest rewards test
-- `if (pQuest->RewItemId[i])` before use), which is exactly what makes one
-- missing guard enough to matter. Deleting the row restores the fail-safe at
-- the funnel.
--
-- 20260505222630_world.sql itself is left alone: it has already been applied
-- and recorded on live databases, and editing it would change its hash and
-- re-run it everywhere.
DELETE FROM `item_template` WHERE `entry` = 0;
