-- Retires pool_entry 43525 "Mossheart and Rotmaw", the pre-1.17.0 rotating
-- rare-spawn pool (pool_template.max_limit = 1, 9 candidate guids
-- 2562705-2562713). None of the 9 candidates exist in `creature` - the
-- Black Morass 1.17.0 rework replaced this rotation with two permanent,
-- non-pooled bosses (Mossheart guid 2577961, Rotmaw guid 2577960, both
-- confirmed present on map 269), and this pool definition was never removed.
-- See wip_updates/creature_movement.sql for the matching waypoint cleanup and
-- wip_updates/pool_template.sql for the pool_template row itself.
DELETE FROM `pool_creature` WHERE `pool_entry` = 43525;
