-- ==============================================
-- FILE: stale_replay_repair_area_template.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- area_template {"entry": "1216"} Timbermaw Hold
UPDATE `area_template` SET `flags` = 64 WHERE `entry` <=> 1216 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- area_template {"entry": "3438"} The Comb
UPDATE `area_template` SET `explore_flag` = 1582 WHERE `entry` <=> 3438 AND (BINARY CAST(`explore_flag` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31303736' USING utf8mb4));
-- area_template {"entry": "3452"} The Comb
UPDATE `area_template` SET `explore_flag` = 1581 WHERE `entry` <=> 3452 AND (BINARY CAST(`explore_flag` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31303732' USING utf8mb4));
-- area_template {"entry": "5734"} Frostmane Hollow
UPDATE `area_template` SET `flags` = 64 WHERE `entry` <=> 5734 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
