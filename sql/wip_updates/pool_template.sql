-- Companion to wip_updates/pool_creature.sql: once pool 43525's candidates
-- are removed there, the pool definition itself is empty and pointless.
-- Not referenced by pool_pool (no parent/child pool nesting) or
-- pool_creature_template - safe to remove standalone.
DELETE FROM `pool_template` WHERE `entry` = 43525;
