-- ==============================================
-- FILE: duke_dreadmoore.sql
-- GENERATED: 20261102022741
-- ==============================================
UPDATE `broadcast_text`
SET `emote_id1` = 0
WHERE `entry` = 92031
AND `emote_id1` = 55;

INSERT INTO `sound_entries`
(
    `id`,
    `name`
)
VALUES
(60443, 'Dukedread1'),
(60444, 'Dukedread2');

-- ==============================================
-- FILE: hyjal_fixes.sql
-- GENERATED: 20261102022741
-- ==============================================
INSERT INTO `broadcast_text`
(
    `entry`,
    `male_text`,
    `female_text`,
    `chat_type`,
    `sound_id`,
    `language_id`,
    `emote_id1`,
    `emote_id2`,
    `emote_id3`,
    `emote_delay1`,
    `emote_delay2`,
    `emote_delay3`
)
VALUES
(8032001, 'You shall be brought under heel intruder...', 'You shall be brought under heel intruder...', 0, 0, 0, 0, 0, 0, 0, 0, 0);

UPDATE `gameobject_template`
SET `size` = 1.2
WHERE `entry` = 2020315;

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
(115049, 80320, 0, 0, 0, 1, 5455.529785, -3574.649902, 1559.76001, 5.4893693923950195, 300, 300, 0, 100, 100, 0, 0, 0),
(115050, 80321, 0, 0, 0, 1, 5457.419922, -3573.580078, 1559.75, 2.3707399368286133, 300, 300, 0, 100, 100, 0, 0, 0);

UPDATE `creature_template`
SET `gossip_menu_id` = 80320,
    `scale` = 1.35,
    `auras` = 22650,
    `attack_power` = 252,
    `dmg_min` = 28.878721,
    `dmg_max` = 36.579712,
    `ranged_attack_power` = 180,
    `ranged_dmg_min` = 28.567617,
    `ranged_dmg_max` = 39.280472
WHERE `entry` = 80320;

UPDATE `creature_template`
SET `gossip_menu_id` = 0,
    `scale` = 0.4,
    `attack_power` = 252,
    `dmg_min` = 28.878721,
    `dmg_max` = 36.579712,
    `ranged_attack_power` = 180,
    `ranged_dmg_min` = 28.567617,
    `ranged_dmg_max` = 39.280472
WHERE `entry` = 80321;

INSERT INTO `gameobject`
(
    `guid`,
    `id`,
    `map`,
    `position_x`,
    `position_y`,
    `position_z`,
    `orientation`,
    `rotation0`,
    `rotation1`,
    `rotation2`,
    `rotation3`,
    `spawntimesecsmin`,
    `spawntimesecsmax`,
    `animprogress`,
    `state`,
    `spawn_flags`,
    `visibility_mod`
)
VALUES
(393854, 2001537, 1, 5457.06005859375, -3578.419921875, 1559.6300048828125, 0.4869399964809418, 0, 0, 0.24107173, 0.970507301, 300, 300, 100, 1, 0, 0),
(5030499, 2020315, 1, 5408.8798828125, -3537.550048828125, 1562.93994140625, 3.5509300231933594, 0, 0, 0.979128376, -0.203242771, 300, 300, 100, 1, 0, 0),
(5030500, 2020315, 1, 5523.27001953125, -3570.110107421875, 1567.6700439453125, 0.09910129755735397, 0, 0, 0.0495303746, 0.998772618, 300, 300, 100, 1, 0, 0),
(5030501, 2020315, 1, 5549.41015625, -3505.550048828125, 1565.25, 1.9000200033187866, 0, 0, 0.813421323, 0.581674954, 300, 300, 100, 1, 0, 0),
(5030503, 2020315, 1, 5454.97998046875, -3433.97998046875, 1563.010009765625, 3.3427999019622803, 0, 0, 0.994943722, -0.100434007, 300, 300, 100, 1, 0, 0),
(5030504, 2020315, 1, 5363.5498046875, -3493.179931640625, 1568.989990234375, 3.8462400436401367, 0, 0, 0.938573388, -0.345079695, 300, 300, 100, 1, 0, 0);

INSERT INTO `gossip_menu`
(
    `entry`,
    `text_id`,
    `script_id`,
    `condition_id`
)
VALUES
(80320, 8032001, 0, 0);

INSERT INTO `npc_text`
(
    `ID`,
    `BroadcastTextID0`,
    `Probability0`,
    `BroadcastTextID1`,
    `Probability1`,
    `BroadcastTextID2`,
    `Probability2`,
    `BroadcastTextID3`,
    `Probability3`,
    `BroadcastTextID4`,
    `Probability4`,
    `BroadcastTextID5`,
    `Probability5`,
    `BroadcastTextID6`,
    `Probability6`,
    `BroadcastTextID7`,
    `Probability7`
)
VALUES
(8032001, 8032001, 1.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0);

-- ==============================================
-- FILE: michael_duguder.sql
-- GENERATED: 20261102022741
-- ==============================================
DELETE FROM `creature`
WHERE `guid` IN (
    332778, 332779
    );

-- ==============================================
-- FILE: missing_gossips_northwind.sql
-- GENERATED: 20261102022741
-- ==============================================
INSERT INTO `broadcast_text`
(
    `entry`,
    `male_text`,
    `female_text`,
    `chat_type`,
    `sound_id`,
    `language_id`,
    `emote_id1`,
    `emote_id2`,
    `emote_id3`,
    `emote_delay1`,
    `emote_delay2`,
    `emote_delay3`
)
VALUES
(6228101, 'Chef Huxley has me on a tight leash. Sir Amberwood has ordered us to serve all tournament contestants the finest cuisine we have to offer; so if you need anything, please make it quick!', 'Chef Huxley has me on a tight leash. Sir Amberwood has ordered us to serve all tournament contestants the finest cuisine we have to offer; so if you need anything, please make it quick!', 0, 0, 0, 0, 0, 0, 0, 0, 0),
(6228201, 'No time to twiddle my thumbs, we''ve got meals to prepare and stomachs to fill! Interested in helping out? I can teach you a few simple dishes.', 'No time to twiddle my thumbs, we''ve got meals to prepare and stomachs to fill! Interested in helping out? I can teach you a few simple dishes.', 0, 0, 0, 0, 0, 0, 0, 0, 0);

UPDATE `creature_template`
SET `gossip_menu_id` = 62281
WHERE `entry` = 62281;

UPDATE `creature_template`
SET `gossip_menu_id` = 62282
WHERE `entry` = 62282;

INSERT INTO `gossip_menu`
(
    `entry`,
    `text_id`,
    `script_id`,
    `condition_id`
)
VALUES
(62281, 6228101, 0, 0),
(62282, 6228201, 0, 0);

INSERT INTO `npc_text`
(
    `ID`,
    `BroadcastTextID0`,
    `Probability0`,
    `BroadcastTextID1`,
    `Probability1`,
    `BroadcastTextID2`,
    `Probability2`,
    `BroadcastTextID3`,
    `Probability3`,
    `BroadcastTextID4`,
    `Probability4`,
    `BroadcastTextID5`,
    `Probability5`,
    `BroadcastTextID6`,
    `Probability6`,
    `BroadcastTextID7`,
    `Probability7`
)
VALUES
(6228101, 6228101, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0),
(6228201, 6228201, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0, 0, 0.0);

-- ==============================================
-- FILE: trainer_fixes.sql
-- GENERATED: 20261102022741
-- ==============================================
UPDATE `creature_template`
SET `trainer_type` = 2
WHERE `entry` IN (
    8696,   -- Henry Stern (cooking)
    11557,  -- Meilosh, Timbermaw Hold Quartermaster (tailoring)
    60510,  -- Grubgar, Fisher Orc
    62143,  -- Smith Martin, Expert Blacksmith
    62282,  -- Linus Huxley, Chef (cooking)
    62286,  -- Pinky Tosslehouse, Herbalist
    62290,  -- Enchantress Magilou, Expert Enchanter
    62299,  -- The Witch of Northwind (alchemy)
    63056,  -- Ulf Stonetotem, Artisan Blacksmith
    80320   -- Rodfather, The Eel Hunter (fishing)
);

-- Class trainers: give them their class
UPDATE `creature_template`
SET `trainer_class` = 3
WHERE `entry` = 61640;  -- Marksman Rembrandt Olar, Hunter Trainer

UPDATE `creature_template`
SET `trainer_class` = 8
WHERE `entry` = 80213;  -- Magistrix Ishalah, Portal Trainer

