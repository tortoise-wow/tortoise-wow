-- ==============================================
-- FILE: broadcast_text_onyxia.sql
-- GENERATED: 20261108142635
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
-- Broodcommander Axelus
(4901801, 'She must not be disturbed!', 'She must not be disturbed!', 1, 60754, 0, 0, 0, 0, 0, 0, 0),
(4901802, 'Break under my heel!', 'Break under my heel!', 1, 0, 0, 0, 0, 0, 0, 0, 0),
(4901803, 'There will be nothing but ash.', 'There will be nothing but ash.', 1, 60756, 0, 0, 0, 0, 0, 0, 0),
(4901804, 'Yours will be a slow death.', 'Yours will be a slow death.', 1, 60755, 0, 0, 0, 0, 0, 0, 0),
(4901805, 'I will not fall, warders to me!', 'I will not fall, warders to me!', 1, 60757, 0, 0, 0, 0, 0, 0, 0),
(4901806, 'Broodmother, they''re… coming…', 'Broodmother, they''re… coming…', 1, 60758, 0, 0, 0, 0, 0, 0, 0),
-- Cindarion
(5014301, 'The arrogance to attack the Broodmother in her lair must be punished!', 'The arrogance to attack the Broodmother in her lair must be punished!', 1, 0, 0, 0, 0, 0, 0, 0, 0),
(5014302, 'Hellfire rains down upon you!', 'Hellfire rains down upon you!', 1, 0, 0, 0, 0, 0, 0, 0, 0),
(5014303, 'You cannot contend with my fury!', 'You cannot contend with my fury!', 1, 0, 0, 0, 0, 0, 0, 0, 0),
(5014304, 'She will be the end of you all...', 'She will be the end of you all...', 1, 0, 0, 0, 0, 0, 0, 0, 0);

-- ==============================================
-- FILE: creature_equip_template_onyxia.sql
-- GENERATED: 20261108142635
-- ==============================================
INSERT INTO `creature_equip_template`
(
    `entry`,
    `equipentry1`,
    `equipentry2`,
    `equipentry3`
)
VALUES
(49018, 33150, 0, 0);

-- ==============================================
-- FILE: creature_movement_onyxia.sql
-- GENERATED: 20261108142635
-- ==============================================
INSERT INTO `creature_movement`
(
    `id`,
    `point`,
    `position_x`,
    `position_y`,
    `position_z`,
    `orientation`,
    `waittime`,
    `wander_distance`,
    `script_id`
)
VALUES
(473281, 1, -16.172911, -191.927231, -51.075001, 0, 0, 0, 0),
(473281, 2, -22.430288, -191.927231, -51.075001, 0, 0, 0, 0),
(473281, 3, -28.381409, -193.86087, -51.075001, 0, 0, 0, 0),
(473281, 4, -33.443737, -197.538864, -51.075001, 0, 0, 0, 0),
(473281, 5, -37.121731, -202.601196, -51.075001, 0, 0, 0, 0),
(473281, 6, -39.055367, -208.552307, -51.075001, 0, 0, 0, 0),
(473281, 7, -39.055367, -214.809692, -51.075001, 0, 0, 0, 0),
(473281, 8, -37.121731, -220.760803, -51.075001, 0, 0, 0, 0),
(473281, 9, -33.443737, -225.823135, -51.075001, 0, 0, 0, 0),
(473281, 10, -28.381409, -229.501129, -51.075001, 0, 0, 0, 0),
(473281, 11, -22.430288, -231.434769, -51.075001, 0, 0, 0, 0),
(473281, 12, -16.172911, -231.434769, -51.075001, 0, 0, 0, 0),
(473281, 13, -10.221789, -229.501129, -51.075001, 0, 0, 0, 0),
(473281, 14, -5.159464, -225.823135, -51.075001, 0, 0, 0, 0),
(473281, 15, -1.481469, -220.760803, -51.075001, 0, 0, 0, 0),
(473281, 16, 0.452167, -214.809692, -51.075001, 0, 0, 0, 0),
(473281, 17, 0.452167, -208.552307, -51.075001, 0, 0, 0, 0),
(473281, 18, -1.481469, -202.601196, -51.075001, 0, 0, 0, 0),
(473281, 19, -5.159464, -197.538864, -51.075001, 0, 0, 0, 0),
(473281, 20, -10.221789, -193.86087, -51.075001, 0, 0, 0, 0),
(473281, 21, -16.172911, -191.927231, -51.075001, 0, 0, 0, 0),
(2593525, 1, -128.684006, -41.0317, -65.258995, 0, 0, 0, 0),
(2593525, 2, -134.684006, -46.2817, -66.008995, 0, 0, 0, 0),
(2593525, 3, -140.684006, -51.7817, -66.758995, 0, 0, 0, 0),
(2593525, 4, -136.203003, -46.876202, -65.842789, 0, 0, 0, 0),
(2593525, 5, -130.703003, -41.126202, -65.092789, 0, 0, 0, 0),
(2593525, 6, -124.953003, -35.376202, -63.342789, 0, 0, 0, 0),
(2593525, 7, -116.6175, -34.7967, -61.249996, 0, 0, 0, 0),
(2593525, 8, -108.6175, -34.5467, -61.749996, 0, 0, 0, 0),
(2593525, 9, -100.6175, -34.5467, -62.249996, 0, 0, 0, 0),
(2593525, 10, -92.6175, -34.2967, -61.499996, 0, 0, 0, 0),
(2593525, 11, -84.6175, -34.0467, -60.999996, 0, 0, 0, 0),
(2593525, 12, -76.6175, -33.7967, -58.999996, 0, 0, 0, 0),
(2593525, 13, -68.6175, -33.5467, -54.999996, 0, 0, 0, 0),
(2593525, 14, -60.6175, -33.5467, -52.499996, 0, 0, 0, 0),
(2593525, 15, -52.7141, -38.544899, -49.66333, 0, 0, 0, 0),
(2593525, 16, -48.7141, -41.544899, -46.66333, 0, 0, 0, 0),
(2593525, 17, -55.298, -36.8596, -51.016953, 0, 0, 0, 0),
(2593525, 18, -67.212997, -34.7328, -54.525658, 0, 0, 0, 0),
(2593525, 19, -75.212997, -34.7328, -58.525658, 0, 0, 0, 0),
(2593525, 20, -83.212997, -34.9828, -61.025658, 0, 0, 0, 0),
(2593525, 21, -91.212997, -34.9828, -61.775658, 0, 0, 0, 0),
(2593525, 22, -99.212997, -35.2328, -62.275658, 0, 0, 0, 0),
(2593525, 23, -107.212997, -35.2328, -62.275658, 0, 0, 0, 0),
(2593525, 24, -115.212997, -35.4828, -61.025658, 0, 0, 0, 0),
(2593525, 25, -121.212997, -35.4828, -62.025658, 0, 0, 0, 0),
(2593525, 26, -128.684006, -41.0317, -65.258995, 0, 0, 0, 0),
(2598890, 1, -160.742004, -65.537186, -67.311401, 0, 0, 0, 0),
(2598890, 2, -166.756927, -70.566986, -67.250092, 0, 0, 0, 0),
(2598890, 3, -172.756927, -75.816986, -67.250092, 0, 0, 0, 0),
(2598890, 4, -178.756927, -81.066986, -67.250092, 0, 0, 0, 0),
(2598890, 5, -160.914688, -65.465553, -67.29586, 0, 0, 0, 0),
(2598903, 1, -159.083496, -67.597176, -67.293076, 0, 0, 0, 0),
(2598903, 2, -165.251541, -72.711777, -67.196815, 0, 0, 0, 0),
(2598903, 3, -171.251541, -77.711777, -67.196815, 0, 0, 0, 0),
(2598903, 4, -176.001541, -81.711777, -67.196815, 0, 0, 0, 0),
(2598903, 5, -159.241989, -67.632156, -67.292389, 0, 0, 0, 0);

-- ==============================================
-- FILE: creature_onyxia.sql
-- GENERATED: 20261108142635
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
(473281, 50143, 0, 0, 0, 249, -16.172911, -191.927231, -51.075001, 1.99952363967896, 86400, 86400, 0, 100, 100, 2, 0, 0),
(2593506, 49018, 0, 0, 0, 249, -189.647003, -94.280899, -67.804604, 0.724044024944305, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593508, 49016, 0, 0, 0, 249, -90.1894, -30.8176, -61.570599, 0.117565996944904, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593509, 49017, 0, 0, 0, 249, -116.697998, -130.190994, -49.479198, 1.14672005176544, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593510, 49016, 0, 0, 0, 249, -121.135002, -122.897003, -49.124599, 0.195601999759674, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593511, 49016, 0, 0, 0, 249, -90.424103, -38.791199, -62.292099, 0.0286983996629715, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593512, 49016, 0, 0, 0, 249, -203.25, -202.567001, -69.462097, 0.0993838980793953, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593513, 49016, 0, 0, 0, 249, -193.731003, -214.945007, -67.987, 0.769285976886749, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593514, 49017, 0, 0, 0, 249, -230.117996, -157.264008, -61.187901, 1.79238998889923, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593515, 49017, 0, 0, 0, 249, -239.319, -155.483002, -61.411301, 1.28188002109528, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593516, 49016, 0, 0, 0, 249, -235.723007, -161.084, -61.8186, 1.45577001571655, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593517, 49017, 0, 0, 0, 249, -113.948997, -211.610992, -73.780701, 3.16759991645813, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593520, 49016, 0, 0, 0, 249, -108.352997, -209.658997, -75.901604, 3.17360997200012, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593521, 49016, 0, 0, 0, 249, -108.247002, -218.462997, -76.308296, 3.17360997200012, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593522, 12129, 0, 0, 0, 249, -113.615997, -216.768005, -74.076202, 3.25399994850159, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593523, 49017, 0, 0, 0, 249, -181.768005, -228.315002, -65.654099, 5.84171009063721, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593524, 12129, 0, 0, 0, 249, -213.966003, -200.363998, -55.1311, 6.15586996078491, 86400, 86400, 0, 100, 100, 0, 0, 0),
(2593525, 12129, 0, 0, 0, 249, -128.684006, -41.0317, -65.258995, -3.12395215034485, 86400, 86400, 0, 100, 100, 2, 0, 0);

-- ==============================================
-- FILE: creature_spells_onyxia.sql
-- GENERATED: 20261108142635
-- ==============================================
INSERT INTO `creature_spells`
(
    `entry`,
    `name`,
    `spellId_1`,
    `probability_1`,
    `castTarget_1`,
    `targetParam1_1`,
    `targetParam2_1`,
    `castFlags_1`,
    `delayInitialMin_1`,
    `delayInitialMax_1`,
    `delayRepeatMin_1`,
    `delayRepeatMax_1`,
    `scriptId_1`,
    `spellId_2`,
    `probability_2`,
    `castTarget_2`,
    `targetParam1_2`,
    `targetParam2_2`,
    `castFlags_2`,
    `delayInitialMin_2`,
    `delayInitialMax_2`,
    `delayRepeatMin_2`,
    `delayRepeatMax_2`,
    `scriptId_2`,
    `spellId_3`,
    `probability_3`,
    `castTarget_3`,
    `targetParam1_3`,
    `targetParam2_3`,
    `castFlags_3`,
    `delayInitialMin_3`,
    `delayInitialMax_3`,
    `delayRepeatMin_3`,
    `delayRepeatMax_3`,
    `scriptId_3`,
    `spellId_4`,
    `probability_4`,
    `castTarget_4`,
    `targetParam1_4`,
    `targetParam2_4`,
    `castFlags_4`,
    `delayInitialMin_4`,
    `delayInitialMax_4`,
    `delayRepeatMin_4`,
    `delayRepeatMax_4`,
    `scriptId_4`,
    `spellId_5`,
    `probability_5`,
    `castTarget_5`,
    `targetParam1_5`,
    `targetParam2_5`,
    `castFlags_5`,
    `delayInitialMin_5`,
    `delayInitialMax_5`,
    `delayRepeatMin_5`,
    `delayRepeatMax_5`,
    `scriptId_5`,
    `spellId_6`,
    `probability_6`,
    `castTarget_6`,
    `targetParam1_6`,
    `targetParam2_6`,
    `castFlags_6`,
    `delayInitialMin_6`,
    `delayInitialMax_6`,
    `delayRepeatMin_6`,
    `delayRepeatMax_6`,
    `scriptId_6`,
    `spellId_7`,
    `probability_7`,
    `castTarget_7`,
    `targetParam1_7`,
    `targetParam2_7`,
    `castFlags_7`,
    `delayInitialMin_7`,
    `delayInitialMax_7`,
    `delayRepeatMin_7`,
    `delayRepeatMax_7`,
    `scriptId_7`,
    `spellId_8`,
    `probability_8`,
    `castTarget_8`,
    `targetParam1_8`,
    `targetParam2_8`,
    `castFlags_8`,
    `delayInitialMin_8`,
    `delayInitialMax_8`,
    `delayRepeatMin_8`,
    `delayRepeatMax_8`,
    `scriptId_8`
)
VALUES
(40068, 'Onyxian Warder - Onyxia''s Lair', 12097, 100, 1, 0, 0, 64, 3, 4, 41, 43, 0, 18958, 100, 1, 0, 0, 64, 5, 9, 23, 25, 0, 19983, 100, 1, 0, 0, 64, 8, 11, 11, 14, 0, 20203, 100, 6, 0, 0, 0, 6, 8, 14, 16, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0),
(50144, 'Onyxian Hatcher - Onyxia''s Lair', 19717, 100, 4, 0, 0, 0, 3, 5, 9, 11, 0, 33441, 100, 1, 0, 0, 64, 5, 7, 10, 12, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 0);

-- ==============================================
-- FILE: creature_template_onyxia.sql
-- GENERATED: 20261108142635
-- ==============================================
-- Onyxian Warder
UPDATE `creature_template`
SET `speed_walk` = 0.888888,
    `speed_run` = 1.14286,
    `scale` = 2,
    `dmg_min` = 1452.822998,
    `dmg_max` = 2035.909668,
    `attack_power` = 262,
    `unit_class` = 1,
    `ranged_dmg_min` = 277.778992,
    `ranged_dmg_max` = 403.528656,
    `ranged_attack_power` = 186,
    `spell_list_id` = `entry`
WHERE `entry` = 40068;

-- Onyxian Inciter
UPDATE `creature_template`
SET `speed_walk` = 0.888888,
    `speed_run` = 1.14286,
    `scale` = 2,
    `dmg_min` = 722.185913,
    `dmg_max` = 788.811279,
    `attack_power` = 81,
    `unit_class` = 8,
    `ranged_dmg_min` = 132.300003,
    `ranged_dmg_max` = 191.800003,
    `ranged_attack_power` = 52
WHERE `entry` = 49016;

-- Onyxian Flamespawn
UPDATE `creature_template`
SET `speed_walk` = 0.888888,
    `speed_run` = 1.14286,
    `scale` = 2,
    `dmg_min` = 883.968323,
    `dmg_max` = 958.202087,
    `attack_power` = 183,
    `unit_class` = 1,
    `ranged_dmg_min` = 193.899994,
    `ranged_dmg_max` = 282.100006,
    `ranged_attack_power` = 186
WHERE `entry` = 49017;

-- Broodcommander Axelus
UPDATE `creature_template`
SET `speed_walk` = 0.888888,
    `speed_run` = 1.14286,
    `scale` = 5,
    `dmg_min` = 2781.229004,
    `dmg_max` = 3209.260742,
    `attack_power` = 187,
    `unit_class` = 1,
    `ranged_dmg_min` = 196.699997,
    `ranged_dmg_max` = 285.600006,
    `ranged_attack_power` = 191,
    `ai_name` = '',
    `script_name` = 'broodcommander_axelus'
WHERE `entry` = 49018;

-- Cindarion
UPDATE `creature_template`
SET `speed_walk` = 1,
    `speed_run` = 1.28571,
    `scale` = 1.4,
    `dmg_min` = 2654.187744,
    `dmg_max` = 3575.726318,
    `attack_power` = 187,
    `unit_class` = 1,
    `unit_flags` = 33554496,
    `ranged_dmg_min` = 641.200012,
    `ranged_dmg_max` = 882,
    `ranged_attack_power` = 191,
    `inhabit_type` = 7,
    `ai_name` = '',
    `script_name` = 'boss_cindarion'
WHERE `entry` = 50143;

-- Onyxian Hatcher
UPDATE `creature_template`
SET `scale` = 1.3,
    `spell_list_id` = `entry`
WHERE `entry` = 50144;

-- ==============================================
-- FILE: gameobject_onyxia.sql
-- GENERATED: 20261108142635
-- ==============================================
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
(5026239, 2002565, 249, 32.44620132446289, -49.55099868774414, -2.2439699172973633, 4.610290050506592, 0, 0, 0.74226735, -0.670103858, 300, 300, 100, 1, 0, 0);

-- ==============================================
-- FILE: sound_entries_onyxia.sql
-- GENERATED: 20261108142635
-- ==============================================
INSERT INTO `sound_entries`
(
    `id`,
    `name`
)
VALUES
(60754, 'Axelus_Aggro'),
(60755, 'Axelus_Glaive'),
(60756, 'Axelus_Chains'),
(60757, 'Axelus_Banner'),
(60758, 'Axelus_Death');

-- ==============================================
-- FILE: spell_template_onyxia.sql
-- GENERATED: 20261108142635
-- ==============================================
UPDATE `spell_template`
SET `script_name` = 'spell_axelus_searing_chains'
WHERE `entry` = 41537;

UPDATE `spell_template`
SET `script_name` = 'spell_axelus_searing_chains_aura'
WHERE `entry` = 47406;

