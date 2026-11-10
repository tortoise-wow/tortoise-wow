-- ==============================================
-- FILE: stale_replay_repair_map_template.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- map_template {"entry": "816"} 
UPDATE `map_template` SET `ghost_entrance_map` = 0, `ghost_entrance_x` = -3813.14, `ghost_entrance_y` = -3587.64, `script_name` = CONVERT(X'696e7374616e63655f647261676f6e6d61775f72657472656174' USING utf8mb4) WHERE `entry` <=> 816 AND (BINARY CAST(`ghost_entrance_map` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d31' USING utf8mb4)) AND (BINARY CAST(`ghost_entrance_x` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ghost_entrance_y` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'' USING utf8mb4));
-- map_template {"entry": "818"} 
UPDATE `map_template` SET `player_limit` = 5, `ghost_entrance_map` = 0, `ghost_entrance_x` = -8821.21, `ghost_entrance_y` = 2900.45, `script_name` = CONVERT(X'696e7374616e63655f73746f726d77726f756768745f7275696e73' USING utf8mb4) WHERE `entry` <=> 818 AND (BINARY CAST(`player_limit` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ghost_entrance_map` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d31' USING utf8mb4)) AND (BINARY CAST(`ghost_entrance_x` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ghost_entrance_y` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'' USING utf8mb4));
