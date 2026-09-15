-- 304 boot warnings collapse to 15 distinct dead creature guids referenced by
-- creature_movement, in two unrelated groups. See the linked issue for the
-- full trace; summary below.
--
-- Group A (5 guids, 53 points): pool_creature pool_entry 43525 "Mossheart and
-- Rotmaw" is a pre-1.17.0 rotating rare-spawn pool (pool_template.max_limit =
-- 1) whose 9 candidates (2562705-2562713) have no matching `creature` row.
-- 5 of them ("Mossheart" candidates) still carry these waypoint paths.
--
-- Per the Turtle WoW Wiki Black Morass page
-- (https://turtle-wow.fandom.com/wiki/Black_Morass) and Patch 1.17.0 notes
-- (https://turtle-wow.fandom.com/wiki/Patch_1.17.0):
--
--     "Rotmaw and Mossheart are no longer rare bosses and can now always be
--     found in the swamp section of the dungeon, raising the total boss
--     count to 7."
--
-- Confirmed against this database: Mossheart (entry 65124) and Rotmaw (entry
-- 65122) already exist as permanent, non-pooled spawns on map 269 (Black
-- Morass) - guid 2577961 and guid 2577960 respectively. Pool 43525 and its
-- movement data are the pre-rework leftovers, superseded when the rework
-- shipped and never cleaned up. See wip_updates/pool_creature.sql and
-- wip_updates/pool_template.sql, which retire the pool itself.
--
-- Group B (10 guids, 251 points): 1068616, 660961, 9581, 21218, 9874, 21173,
-- 9296, 8880, 8988, 9001 have carried these waypoints since sql/base itself -
-- not something deleted later, never matched a `creature` row in this
-- repository's history. Donor-era dead data. Two of them (9874, 21173) also
-- carry a leftover creature_addon row, removed in
-- wip_updates/creature_addon.sql.
DELETE FROM `creature_movement` WHERE `id` IN (
    -- Group A: dead pre-1.17.0 Mossheart/Rotmaw pool candidates
    2562709, 2562710, 2562711, 2562712, 2562713,
    -- Group B: donor-era dead data, present since sql/base
    1068616, 660961, 9581, 21218, 9874, 21173, 9296, 8880, 8988, 9001
);
