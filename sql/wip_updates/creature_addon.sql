-- Companion cleanup to wip_updates/creature_movement.sql (Group B, donor-era
-- dead data). Guids 9874 and 21173 have no matching `creature` row and never
-- have in this repository's history, but still carry a stray creature_addon
-- row (stand/sheath/emote/aura data for a spawn that does not exist):
--
--     Creature (GUID: 9874) does not exist but has a record in `creature_addon`
--     Creature (GUID: 21173) does not exist but has a record in `creature_addon`
DELETE FROM `creature_addon` WHERE `guid` IN (9874, 21173);
