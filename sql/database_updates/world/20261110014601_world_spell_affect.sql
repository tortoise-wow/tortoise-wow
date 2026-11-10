-- ==============================================
-- FILE: stale_replay_repair_spell_affect.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- spell_affect {"entry": "51318", "effectId": "1"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51318 AND `effectId` <=> 1 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51317", "effectId": "2"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51317 AND `effectId` <=> 2 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51317", "effectId": "1"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51317 AND `effectId` <=> 1 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51318", "effectId": "2"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51318 AND `effectId` <=> 2 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51319", "effectId": "1"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51319 AND `effectId` <=> 1 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51319", "effectId": "2"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51319 AND `effectId` <=> 2 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51320", "effectId": "1"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51320 AND `effectId` <=> 1 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51320", "effectId": "2"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51320 AND `effectId` <=> 2 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51321", "effectId": "1"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51321 AND `effectId` <=> 1 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
-- spell_affect {"entry": "51321", "effectId": "2"} 
UPDATE `spell_affect` SET `SpellFamilyMask` = 137438953472 WHERE `entry` <=> 51321 AND `effectId` <=> 2 AND (BINARY CAST(`SpellFamilyMask` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34323934393637323936' USING utf8mb4));
