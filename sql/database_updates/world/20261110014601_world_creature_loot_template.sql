-- ==============================================
-- FILE: stale_replay_repair_creature_loot_template.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- creature_loot_template {"entry": "11120", "item": "20400", "groupid": "1"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 11120 AND `item` <=> 20400 AND `groupid` <=> 1 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "11324", "item": "12862", "groupid": "7"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 11324 AND `item` <=> 12862 AND `groupid` <=> 7 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "11517", "item": "17041", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 11517 AND `item` <=> 17041 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "11517", "item": "17041", "groupid": "7"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 11517 AND `item` <=> 17041 AND `groupid` <=> 7 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "11518", "item": "55004", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 11518 AND `item` <=> 55004 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "11518", "item": "80111", "groupid": "3"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 11518 AND `item` <=> 80111 AND `groupid` <=> 3 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "11519", "item": "64", "groupid": "7"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 11519 AND `item` <=> 64 AND `groupid` <=> 7 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "11520", "item": "5235", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 11520 AND `item` <=> 5235 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "3671", "item": "2189", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 3671 AND `item` <=> 2189 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "3671", "item": "2190", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 3671 AND `item` <=> 2190 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "59988", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 59988 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "60608", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 60608 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "60609", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 60609 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "61164", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 61164 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "61348", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 61348 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "61564", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 61564 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "61934", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 61934 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "61935", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 61935 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "61949", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 61949 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "61961", "item": "107", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 61961 AND `item` <=> 107 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62017", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62017 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62018", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62018 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62019", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62019 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62025", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62025 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62337", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62337 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62555", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62555 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62606", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62606 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62607", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62607 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62618", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62618 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62794", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62794 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62809", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62809 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62813", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62813 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62814", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62814 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62815", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62815 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62825", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62825 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62826", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62826 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62830", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62830 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62831", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62831 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62832", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62832 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62871", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62871 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62892", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62892 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62893", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62893 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62894", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62894 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62895", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62895 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62988", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62988 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62990", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62990 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62992", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62992 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "62998", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 62998 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63000", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63000 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63006", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63006 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63027", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63027 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63028", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63028 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63029", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63029 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63030", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63030 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63035", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63035 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63036", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63036 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63037", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63037 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63041", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63041 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63042", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63042 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63063", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63063 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63067", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63067 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63068", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63068 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63117", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63117 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63142", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63142 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63146", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63146 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63147", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63147 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63150", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63150 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63151", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63151 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63156", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63156 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63157", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63157 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63158", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63158 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "63159", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 63159 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "639", "item": "108", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 639 AND `item` <=> 108 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "639", "item": "29980", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 639 AND `item` <=> 29980 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "643", "item": "50256", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 643 AND `item` <=> 50256 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "644", "item": "9496", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 644 AND `item` <=> 9496 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "645", "item": "9338", "groupid": "2"} 
UPDATE `creature_loot_template` SET `condition_id` = 61003 WHERE `entry` <=> 645 AND `item` <=> 9338 AND `groupid` <=> 2 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "91813", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 91813 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "92157", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 92157 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "92203", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 92203 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "92204", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 92204 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000190", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000190 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000288", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000288 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000289", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000289 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000290", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000290 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000291", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000291 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000292", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000292 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000293", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000293 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000294", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000294 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000297", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000297 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000298", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000298 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000345", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000345 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000356", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000356 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000357", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000357 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000358", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000358 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000359", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000359 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000360", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000360 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000361", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000361 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000362", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000362 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000363", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000363 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000364", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000364 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000365", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000365 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000367", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000367 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000369", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000369 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000370", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000370 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000371", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000371 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000372", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000372 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000373", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000373 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000377", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000377 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000378", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000378 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000379", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000379 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- creature_loot_template {"entry": "2000380", "item": "20400", "groupid": "0"} 
UPDATE `creature_loot_template` SET `condition_id` = 108 WHERE `entry` <=> 2000380 AND `item` <=> 20400 AND `groupid` <=> 0 AND (BINARY CAST(`condition_id` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
