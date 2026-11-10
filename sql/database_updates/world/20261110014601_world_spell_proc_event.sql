-- ==============================================
-- FILE: stale_replay_repair_spell_proc_event.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- spell_proc_event {"entry": "15058"} 
UPDATE `spell_proc_event` SET `procFlags` = 0 WHERE `entry` <=> 15058 AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3635353336' USING utf8mb4));
-- spell_proc_event {"entry": "51317"} 
UPDATE `spell_proc_event` SET `procFlags` = 0 WHERE `entry` <=> 51317 AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3639393034' USING utf8mb4));
-- spell_proc_event {"entry": "52940"} 
UPDATE `spell_proc_event` SET `procFlags` = 0 WHERE `entry` <=> 52940 AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3837303430' USING utf8mb4));
-- spell_proc_event {"entry": "16998"} 
UPDATE `spell_proc_event` SET `SpellFamilyMask0` = 137438955520, `SpellFamilyMask1` = 8388608, `procFlags` = 0 WHERE `entry` <=> 16998 AND (BINARY CAST(`SpellFamilyMask0` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'313337343338393539363136' USING utf8mb4)) AND (BINARY CAST(`SpellFamilyMask1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3136' USING utf8mb4));
-- spell_proc_event {"entry": "16089"} 
UPDATE `spell_proc_event` SET `procFlags` = 0 WHERE `entry` <=> 16089 AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3635353336' USING utf8mb4));
-- spell_proc_event {"entry": "16266"} 
UPDATE `spell_proc_event` SET `SpellFamilyMask2` = 0, `procFlags` = 332116, `procEx` = 65536 WHERE `entry` <=> 16266 AND (BINARY CAST(`SpellFamilyMask2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38333838363038' USING utf8mb4)) AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34' USING utf8mb4)) AND (BINARY CAST(`procEx` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- spell_proc_event {"entry": "51598"} 
UPDATE `spell_proc_event` SET `procFlags` = 0 WHERE `entry` <=> 51598 AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3136' USING utf8mb4));
-- spell_proc_event {"entry": "52628"} 
UPDATE `spell_proc_event` SET `procFlags` = 0 WHERE `entry` <=> 52628 AND (BINARY CAST(`procFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'323632313434' USING utf8mb4));
-- spell_proc_event {"entry": "20915"} 
UPDATE `spell_proc_event` SET `ppmRate` = 9 WHERE `entry` <=> 20915 AND (BINARY CAST(`ppmRate` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37' USING utf8mb4));
-- spell_proc_event {"entry": "20918"} 
UPDATE `spell_proc_event` SET `ppmRate` = 9 WHERE `entry` <=> 20918 AND (BINARY CAST(`ppmRate` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37' USING utf8mb4));
-- spell_proc_event {"entry": "20919"} 
UPDATE `spell_proc_event` SET `ppmRate` = 9 WHERE `entry` <=> 20919 AND (BINARY CAST(`ppmRate` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37' USING utf8mb4));
-- spell_proc_event {"entry": "20920"} 
UPDATE `spell_proc_event` SET `ppmRate` = 9 WHERE `entry` <=> 20920 AND (BINARY CAST(`ppmRate` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37' USING utf8mb4));
