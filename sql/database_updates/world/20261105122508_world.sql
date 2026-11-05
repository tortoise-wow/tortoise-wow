-- ==============================================
-- FILE: dranei_exile_quartermasters.sql
-- GENERATED: 20261105122508
-- ==============================================
INSERT INTO `creature`
(
    `guid`,
    `id`,
    `id2`,
    `id3`,
    `id4`,
    `map`,
    `position_x`,
    `position_y`,
    `position_z`,
    `orientation`,
    `spawntimesecsmin`,
    `spawntimesecsmax`,
    `wander_distance`,
    `health_percent`,
    `mana_percent`,
    `movement_type`,
    `spawn_flags`,
    `visibility_mod`
)
VALUES
(2594811, 63013, 0, 0, 0, 1, 6784.069824, -6148.120117, 56.1124, 2.7971999645233154, 300, 300, 0, 100, 100, 0, 0, 0),
(2594812, 63012, 0, 0, 0, 0, -10117.5, -2837.379883, 22.084101, 2.6974198818206787, 300, 300, 0, 100, 100, 0, 0, 0),
(2595909, 33005, 0, 0, 0, 0, -10111.900391, -2840.52002, 22.3256, 3.8040599822998047, 300, 300, 0, 100, 100, 0, 0, 0);

INSERT INTO `creature_template`
(
    `entry`,
    `display_id1`,
    `display_id2`,
    `display_id3`,
    `display_id4`,
    `mount_display_id`,
    `name`,
    `subname`,
    `gossip_menu_id`,
    `level_min`,
    `level_max`,
    `health_min`,
    `health_max`,
    `mana_min`,
    `mana_max`,
    `armor`,
    `faction`,
    `npc_flags`,
    `speed_walk`,
    `speed_run`,
    `scale`,
    `detection_range`,
    `call_for_help_range`,
    `leash_range`,
    `rank`,
    `xp_multiplier`,
    `dmg_min`,
    `dmg_max`,
    `dmg_school`,
    `attack_power`,
    `dmg_multiplier`,
    `base_attack_time`,
    `ranged_attack_time`,
    `unit_class`,
    `unit_flags`,
    `dynamic_flags`,
    `beast_family`,
    `trainer_type`,
    `trainer_spell`,
    `trainer_class`,
    `trainer_race`,
    `ranged_dmg_min`,
    `ranged_dmg_max`,
    `ranged_attack_power`,
    `type`,
    `type_flags`,
    `loot_id`,
    `pickpocket_loot_id`,
    `skinning_loot_id`,
    `holy_res`,
    `fire_res`,
    `nature_res`,
    `frost_res`,
    `shadow_res`,
    `arcane_res`,
    `spell_id1`,
    `spell_id2`,
    `spell_id3`,
    `spell_id4`,
    `spell_list_id`,
    `pet_spell_list_id`,
    `spawn_spell_id`,
    `auras`,
    `gold_min`,
    `gold_max`,
    `ai_name`,
    `movement_type`,
    `inhabit_type`,
    `civilian`,
    `racial_leader`,
    `regeneration`,
    `equipment_id`,
    `trainer_id`,
    `vendor_id`,
    `mechanic_immune_mask`,
    `school_immune_mask`,
    `immunity_flags`,
    `flags_extra`,
    `phase_quest_id`,
    `script_name`
)
VALUES
(63013, 11650, 0, 0, 0, 0, 'Mogh''ador', 'Moro''gai Tribe Quartermaster', 0, 55, 55, 4007, 4007, 0, 0, 3272, 160, 4, 1, 1.14286, 1.1, 18, 5, 0, 0, 1, 100.224724, 125.814018, 0, 228, 1, 2000, 2000, 1, 0, 0, 0, 0, 0, 0, 0, 84.475464, 116.153755, 162, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 'EventAI', 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, ''),
(63012, 11650, 0, 0, 0, 0, 'Zad''ran', 'Sanv Tribe Quartermaster', 0, 55, 55, 4007, 4007, 0, 0, 3272, 160, 4, 1, 1.14286, 1.1, 18, 5, 0, 0, 1, 100.224724, 125.814018, 0, 228, 1, 2000, 2000, 1, 0, 0, 0, 0, 0, 0, 0, 84.475464, 116.153755, 162, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 'EventAI', 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, '');

UPDATE `creature_template`
SET `speed_run` = 1.38571,
    `scale` = 1,
    `dmg_min` = 1.884348,
    `dmg_max` = 1.884348,
    `attack_power` = 24,
    `ranged_dmg_min` = 1.3376,
    `ranged_dmg_max` = 1.8392,
    `ranged_attack_power` = 20
WHERE `entry` = 33005;

INSERT INTO `npc_vendor`
(
    `entry`,
    `slot`,
    `item`,
    `maxcount`,
    `incrtime`,
    `itemflags`,
    `condition_id`
)
VALUES
(63012, 1, 30005, 0, 0, 0, 0),
(63012, 2, 33177, 0, 0, 0, 0),
(63012, 3, 33141, 0, 0, 0, 0),
(63012, 4, 33142, 0, 0, 0, 0),
(63012, 5, 33143, 0, 0, 0, 0),
(63012, 6, 33144, 0, 0, 0, 0),
(63012, 7, 33148, 0, 0, 0, 0),
(63012, 8, 33178, 0, 0, 0, 0),
(63012, 9, 37071, 0, 0, 0, 0),
(63013, 1, 33136, 0, 0, 0, 0),
(63013, 2, 33137, 0, 0, 0, 0),
(63013, 3, 33138, 0, 0, 0, 0),
(63013, 4, 33139, 0, 0, 0, 0),
(63013, 5, 33140, 0, 0, 0, 0),
(63013, 6, 33145, 0, 0, 0, 0),
(63013, 7, 33147, 0, 0, 0, 0),
(63013, 8, 37072, 0, 0, 0, 0);

-- ==============================================
-- FILE: frostmane_dragonmaw_graveyards.sql
-- GENERATED: 20261105122508
-- ==============================================
INSERT INTO `game_graveyard_zone`
(
    `id`,
    `ghost_zone`,
    `faction`
)
VALUES
(7, 5601, 0),
(109, 5734, 0);

UPDATE `map_template`
SET `ghost_entrance_map` = 0,
    `ghost_entrance_x` = -5023.99,
    `ghost_entrance_y` = -1443.13
WHERE `entry` = 822;

-- ==============================================
-- FILE: lothos_riftwalker.sql
-- GENERATED: 20261105122508
-- ==============================================
UPDATE `creature_template`
SET `npc_flags` = 3
WHERE `entry` = 14387;

-- ==============================================
-- FILE: frostmane_teleport.sql
-- GENERATED: 20261105122508
-- ==============================================
UPDATE `game_tele`
SET `name` = 'zOldfrostmane'
WHERE `id` = 811
AND `name` = 'frostmane';
