-- ==============================================
-- FILE: stale_replay_repair_npc_vendor.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- npc_vendor {"entry": "2482", "item": "12251"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 2482 AND `item` <=> 12251 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2482", "item": "12252"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2482 AND `item` <=> 12252 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "80956", "item": "4116"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 80956 AND `item` <=> 4116 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "225", "item": "12247"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 225 AND `item` <=> 12247 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "225", "item": "12249"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 225 AND `item` <=> 12249 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "226", "item": "4797"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 226 AND `item` <=> 4797 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "226", "item": "4798"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 226 AND `item` <=> 4798 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "226", "item": "4799"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 226 AND `item` <=> 4799 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "226", "item": "4800"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 226 AND `item` <=> 4800 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "226", "item": "4816"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 226 AND `item` <=> 4816 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "793", "item": "4820"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 793 AND `item` <=> 4820 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "793", "item": "4821"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 793 AND `item` <=> 4821 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "793", "item": "4822"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 793 AND `item` <=> 4822 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "3355"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 844 AND `item` <=> 3355 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "3356"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 844 AND `item` <=> 3356 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "3357"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 844 AND `item` <=> 3357 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "4777"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 844 AND `item` <=> 4777 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "4778"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 844 AND `item` <=> 4778 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "4794"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 844 AND `item` <=> 4794 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "4795"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 844 AND `item` <=> 4795 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "4796"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 844 AND `item` <=> 4796 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "844", "item": "4817"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 844 AND `item` <=> 4817 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "954", "item": "4789"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 954 AND `item` <=> 4789 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "958", "item": "954"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 958 AND `item` <=> 954 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "958", "item": "955"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 958 AND `item` <=> 955 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "983", "item": "3928"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 983 AND `item` <=> 3928 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "984", "item": "12256"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 984 AND `item` <=> 12256 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1146", "item": "12248"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 1146 AND `item` <=> 12248 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1146", "item": "12250"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 1146 AND `item` <=> 12250 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1198", "item": "11303"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1198 AND `item` <=> 11303 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1214", "item": "4781"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 1214 AND `item` <=> 4781 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1214", "item": "4782"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 1214 AND `item` <=> 4782 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1214", "item": "4786"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 1214 AND `item` <=> 4786 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1298", "item": "11306"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1298 AND `item` <=> 11306 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1298", "item": "11307"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 1298 AND `item` <=> 11307 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1302", "item": "3355"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1302 AND `item` <=> 3355 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1302", "item": "3356"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1302 AND `item` <=> 3356 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1307", "item": "4824"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1307 AND `item` <=> 4824 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1307", "item": "4825"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 1307 AND `item` <=> 4825 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1307", "item": "4826"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 1307 AND `item` <=> 4826 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1307", "item": "4827"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1307 AND `item` <=> 4827 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1307", "item": "4828"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1307 AND `item` <=> 4828 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1307", "item": "4829"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1307 AND `item` <=> 4829 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1313", "item": "785"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1313 AND `item` <=> 785 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1313", "item": "2449"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1313 AND `item` <=> 2449 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1313", "item": "3355"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1313 AND `item` <=> 3355 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1313", "item": "3356"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1313 AND `item` <=> 3356 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1313", "item": "3357"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1313 AND `item` <=> 3357 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1316", "item": "1711"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1316 AND `item` <=> 1711 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1316", "item": "2290"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 1316 AND `item` <=> 2290 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1448", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 1448 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1448", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 1448 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1448", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 1448 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1454", "item": "4790"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1454 AND `item` <=> 4790 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1454", "item": "4792"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1454 AND `item` <=> 4792 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1454", "item": "4793"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1454 AND `item` <=> 4793 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1454", "item": "4827"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1454 AND `item` <=> 4827 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1454", "item": "4828"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1454 AND `item` <=> 4828 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1454", "item": "4829"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1454 AND `item` <=> 4829 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1459", "item": "11304"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 1459 AND `item` <=> 11304 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1669", "item": "4788"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1669 AND `item` <=> 4788 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1669", "item": "4789"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1669 AND `item` <=> 4789 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1678", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 1678 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1684", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 1684 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1694", "item": "4371"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 1694 AND `item` <=> 4371 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1694", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 1694 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "1694", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 1694 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2380", "item": "1710"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2380 AND `item` <=> 1710 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2380", "item": "3827"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2380 AND `item` <=> 3827 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2383", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2383 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2483", "item": "12250"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 2483 AND `item` <=> 12250 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2626", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2626 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2626", "item": "6533"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2626 AND `item` <=> 6533 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4827"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4827 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4828"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4828 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4829"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4829 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4830"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4830 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4831"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4831 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4832"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4832 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4833"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4833 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2679", "item": "4835"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 2679 AND `item` <=> 4835 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2682", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2682 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2682", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2682 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2682", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2682 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2683", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2683 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2683", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2683 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2684", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2684 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2684", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2684 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2685", "item": "4371"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2685 AND `item` <=> 4371 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2685", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2685 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2685", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2685 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2685", "item": "4404"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2685 AND `item` <=> 4404 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2687", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2687 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2687", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 2687 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2805", "item": "954"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2805 AND `item` <=> 954 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2805", "item": "1711"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2805 AND `item` <=> 1711 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2805", "item": "2290"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2805 AND `item` <=> 2290 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2812", "item": "1710"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2812 AND `item` <=> 1710 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2840", "item": "4778"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 2840 AND `item` <=> 4778 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2843", "item": "12248"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 2843 AND `item` <=> 12248 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2845", "item": "12257"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2845 AND `item` <=> 12257 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "2848", "item": "1710"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 2848 AND `item` <=> 1710 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3019", "item": "12249"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3019 AND `item` <=> 12249 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3029", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3029 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3091", "item": "4782"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3091 AND `item` <=> 4782 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3091", "item": "4786"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3091 AND `item` <=> 4786 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3096", "item": "4790"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3096 AND `item` <=> 4790 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3096", "item": "4792"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3096 AND `item` <=> 4792 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3096", "item": "4793"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3096 AND `item` <=> 4793 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3097", "item": "4794"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3097 AND `item` <=> 4794 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3097", "item": "4795"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3097 AND `item` <=> 4795 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3097", "item": "4796"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3097 AND `item` <=> 4796 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3133", "item": "4361"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3133 AND `item` <=> 4361 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3133", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3133 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3133", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3133 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3133", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3133 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3134", "item": "4827"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3134 AND `item` <=> 4827 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3134", "item": "4828"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3134 AND `item` <=> 4828 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3134", "item": "4829"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3134 AND `item` <=> 4829 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3134", "item": "4836"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3134 AND `item` <=> 4836 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3134", "item": "4837"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3134 AND `item` <=> 4837 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3134", "item": "4838"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3134 AND `item` <=> 4838 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3178", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3178 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4824"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3180 AND `item` <=> 4824 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4825"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 3180 AND `item` <=> 4825 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4826"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 3180 AND `item` <=> 4826 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4830"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3180 AND `item` <=> 4830 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4831"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3180 AND `item` <=> 4831 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4832"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3180 AND `item` <=> 4832 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4833"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3180 AND `item` <=> 4833 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "4835"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3180 AND `item` <=> 4835 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3180", "item": "5108"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 300 WHERE `entry` <=> 3180 AND `item` <=> 5108 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3315", "item": "12255"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3315 AND `item` <=> 12255 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3316", "item": "12256"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3316 AND `item` <=> 12256 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3333", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3333 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3413", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3413 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3413", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3413 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3413", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3413 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3479", "item": "4765"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 3479 AND `item` <=> 4765 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3486", "item": "4781"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3486 AND `item` <=> 4781 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3486", "item": "4782"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3486 AND `item` <=> 4782 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3486", "item": "4786"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3486 AND `item` <=> 4786 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3486", "item": "4790"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3486 AND `item` <=> 4790 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3486", "item": "4792"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3486 AND `item` <=> 4792 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3486", "item": "4793"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3486 AND `item` <=> 4793 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3490", "item": "2453"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3490 AND `item` <=> 2453 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3492", "item": "4794"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3492 AND `item` <=> 4794 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3492", "item": "4795"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3492 AND `item` <=> 4795 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3492", "item": "4796"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3492 AND `item` <=> 4796 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4797"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4797 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4798"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4798 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4799"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4799 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4800"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4800 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4816"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4816 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4820"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4820 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4821"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4821 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3493", "item": "4822"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3493 AND `item` <=> 4822 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3495", "item": "4361"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3495 AND `item` <=> 4361 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3495", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3495 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3495", "item": "4371"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3495 AND `item` <=> 4371 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3495", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3495 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3495", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 3495 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3497", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3497 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3536", "item": "4794"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3536 AND `item` <=> 4794 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3536", "item": "4795"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3536 AND `item` <=> 4795 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3536", "item": "4796"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3536 AND `item` <=> 4796 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3536", "item": "4830"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3536 AND `item` <=> 4830 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3536", "item": "4831"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3536 AND `item` <=> 4831 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3537", "item": "4827"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3537 AND `item` <=> 4827 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3537", "item": "4828"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3537 AND `item` <=> 4828 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3537", "item": "4829"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3537 AND `item` <=> 4829 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3537", "item": "4836"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3537 AND `item` <=> 4836 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3537", "item": "4837"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3537 AND `item` <=> 4837 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3537", "item": "4838"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3537 AND `item` <=> 4838 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3539", "item": "4817"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 3539 AND `item` <=> 4817 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3539", "item": "4818"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 3539 AND `item` <=> 4818 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3539", "item": "4824"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3539 AND `item` <=> 4824 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3539", "item": "4825"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 3539 AND `item` <=> 4825 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3539", "item": "4826"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 3539 AND `item` <=> 4826 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3539", "item": "12247"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 3539 AND `item` <=> 12247 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3539", "item": "12249"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 3539 AND `item` <=> 12249 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3543", "item": "4797"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3543 AND `item` <=> 4797 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3543", "item": "4798"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3543 AND `item` <=> 4798 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3543", "item": "4799"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3543 AND `item` <=> 4799 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3543", "item": "4800"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3543 AND `item` <=> 4800 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3543", "item": "4816"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3543 AND `item` <=> 4816 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3543", "item": "4833"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3543 AND `item` <=> 4833 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3543", "item": "4835"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3543 AND `item` <=> 4835 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3550", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3550 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3552", "item": "4789"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3552 AND `item` <=> 4789 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3554", "item": "4781"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3554 AND `item` <=> 4781 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3554", "item": "4782"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3554 AND `item` <=> 4782 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3554", "item": "4786"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3554 AND `item` <=> 4786 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3572", "item": "6533"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3572 AND `item` <=> 6533 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3572", "item": "6565"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 3572 AND `item` <=> 6565 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3658", "item": "4778"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3658 AND `item` <=> 4778 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3682", "item": "4766"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 3682 AND `item` <=> 4766 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3682", "item": "4797"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 3682 AND `item` <=> 4797 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3682", "item": "4798"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 3682 AND `item` <=> 4798 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3682", "item": "4799"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 3682 AND `item` <=> 4799 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3682", "item": "4800"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 3682 AND `item` <=> 4800 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3682", "item": "4816"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 8000 WHERE `entry` <=> 3682 AND `item` <=> 4816 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3683", "item": "4781"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3683 AND `item` <=> 4781 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3683", "item": "4782"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3683 AND `item` <=> 4782 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3683", "item": "4786"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 3683 AND `item` <=> 4786 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3684", "item": "4789"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3684 AND `item` <=> 4789 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "3951", "item": "11305"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 3951 AND `item` <=> 11305 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4083", "item": "2453"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4083 AND `item` <=> 2453 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4083", "item": "3356"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4083 AND `item` <=> 3356 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4827"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4827 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4828"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4828 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4829"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4829 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4830"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4830 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4831"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4831 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4832"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4832 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4833"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4833 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4085", "item": "4835"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4085 AND `item` <=> 4835 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4086", "item": "4817"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 4086 AND `item` <=> 4817 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4086", "item": "4818"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 4086 AND `item` <=> 4818 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4086", "item": "4836"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 4086 AND `item` <=> 4836 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4086", "item": "4837"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 4086 AND `item` <=> 4837 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4086", "item": "4838"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 4086 AND `item` <=> 4838 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4173", "item": "11303"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4173 AND `item` <=> 11303 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4173", "item": "11306"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4173 AND `item` <=> 11306 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4173", "item": "11308"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4173 AND `item` <=> 11308 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4175", "item": "12256"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4175 AND `item` <=> 12256 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4184", "item": "13895"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 604800 WHERE `entry` <=> 4184 AND `item` <=> 13895 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4184", "item": "13899"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 604800 WHERE `entry` <=> 4184 AND `item` <=> 13899 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4184", "item": "13900"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 604800 WHERE `entry` <=> 4184 AND `item` <=> 13900 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4222", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4222 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4232", "item": "12249"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4232 AND `item` <=> 12249 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4235", "item": "12247"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 4235 AND `item` <=> 12247 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4574", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4574 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4574", "item": "6533"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4574 AND `item` <=> 6533 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4587", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 4587 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4587", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 4587 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4587", "item": "4404"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 4587 AND `item` <=> 4404 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4604", "item": "11306"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 4604 AND `item` <=> 11306 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4604", "item": "11307"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 4604 AND `item` <=> 11307 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4878", "item": "954"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4878 AND `item` <=> 954 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4878", "item": "1711"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4878 AND `item` <=> 1711 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4878", "item": "3355"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4878 AND `item` <=> 3355 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4878", "item": "3356"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4878 AND `item` <=> 3356 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4878", "item": "3357"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4878 AND `item` <=> 3357 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4884", "item": "12250"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 4884 AND `item` <=> 12250 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4884", "item": "12251"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 4884 AND `item` <=> 12251 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4888", "item": "12248"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 4888 AND `item` <=> 12248 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4888", "item": "12250"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 4888 AND `item` <=> 12250 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4817"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 4890 AND `item` <=> 4817 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4824"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 4890 AND `item` <=> 4824 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4825"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 4890 AND `item` <=> 4825 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4826"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 86400 WHERE `entry` <=> 4890 AND `item` <=> 4826 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4830"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4890 AND `item` <=> 4830 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4831"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4890 AND `item` <=> 4831 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4832"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4890 AND `item` <=> 4832 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4833"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4890 AND `item` <=> 4833 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "4890", "item": "4835"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 4890 AND `item` <=> 4835 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5122", "item": "11303"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5122 AND `item` <=> 11303 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5122", "item": "11306"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5122 AND `item` <=> 11306 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5122", "item": "11307"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5122 AND `item` <=> 11307 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5158", "item": "10938"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 5158 AND `item` <=> 10938 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5162", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 5162 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5175", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5175 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5175", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5175 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5494", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 5494 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5494", "item": "6533"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 5494 AND `item` <=> 6533 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5519", "item": "4361"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5519 AND `item` <=> 4361 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5519", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5519 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5519", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5519 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5519", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 5519 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5757", "item": "10938"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 5757 AND `item` <=> 10938 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5758", "item": "10938"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 5758 AND `item` <=> 10938 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5940", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 5940 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "5942", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 5942 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "6730", "item": "4361"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 6730 AND `item` <=> 4361 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "6730", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 6730 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "6730", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 6730 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "6730", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 6730 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "6777", "item": "4361"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 6777 AND `item` <=> 4361 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "6777", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 6777 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "6777", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 6777 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "7945", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 7945 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8125", "item": "54002"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 8125 AND `item` <=> 54002 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8158", "item": "3928"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 8158 AND `item` <=> 3928 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8159", "item": "12253"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 8159 AND `item` <=> 12253 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8159", "item": "12254"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 8159 AND `item` <=> 12254 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8305", "item": "3355"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 8305 AND `item` <=> 3355 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8305", "item": "3356"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 8305 AND `item` <=> 3356 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8305", "item": "3357"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 8305 AND `item` <=> 3357 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8508", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 8508 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8508", "item": "6533"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 8508 AND `item` <=> 6533 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8666", "item": "8489"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 8666 AND `item` <=> 8489 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8678", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 8678 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8678", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 8678 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8679", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 8679 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "8679", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 8679 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "9544", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 9544 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "9544", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 9544 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "9544", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 9544 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "9548", "item": "11308"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 9548 AND `item` <=> 11308 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "9551", "item": "11304"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 9551 AND `item` <=> 11304 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "9551", "item": "11305"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 9551 AND `item` <=> 11305 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "9676", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 9676 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "10118", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 10118 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "11185", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 11185 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "11185", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 11185 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "11188", "item": "3928"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 11188 AND `item` <=> 3928 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "11188", "item": "6149"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 11188 AND `item` <=> 6149 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "12023", "item": "12254"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 12023 AND `item` <=> 12254 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "12029", "item": "11308"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 12029 AND `item` <=> 11308 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "12045", "item": "12249"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 12045 AND `item` <=> 12249 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "12045", "item": "12250"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 12045 AND `item` <=> 12250 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "12384", "item": "15902"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 12384 AND `item` <=> 15902 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "12962", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 12962 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "13476", "item": "1478"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 13476 AND `item` <=> 1478 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "13476", "item": "3012"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 13476 AND `item` <=> 3012 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14301", "item": "11305"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14301 AND `item` <=> 11305 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14637", "item": "4371"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 14637 AND `item` <=> 4371 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14637", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 14637 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14637", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 14637 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14740", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14740 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "785"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14846 AND `item` <=> 785 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "2453"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14846 AND `item` <=> 2453 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "8845"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14846 AND `item` <=> 8845 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "11303"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14846 AND `item` <=> 11303 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "11305"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14846 AND `item` <=> 11305 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "11306"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14846 AND `item` <=> 11306 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "11308"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14846 AND `item` <=> 11308 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "12248"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 14846 AND `item` <=> 12248 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "12251"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 14846 AND `item` <=> 12251 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "13456"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14846 AND `item` <=> 13456 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "19302"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14846 AND `item` <=> 19302 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14846", "item": "19303"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14846 AND `item` <=> 19303 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "2290"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14847 AND `item` <=> 2290 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "4421"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14847 AND `item` <=> 4421 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "4422"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14847 AND `item` <=> 4422 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "4424"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14847 AND `item` <=> 4424 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "4425"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14847 AND `item` <=> 4425 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "10305"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14847 AND `item` <=> 10305 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "10307"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14847 AND `item` <=> 10307 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "10310"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14847 AND `item` <=> 10310 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14847", "item": "13443"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 43200 WHERE `entry` <=> 14847 AND `item` <=> 13443 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "14860", "item": "11027"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 14860 AND `item` <=> 11027 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "60480", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 60480 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "60751", "item": "50024"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 36000 WHERE `entry` <=> 60751 AND `item` <=> 50024 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "60751", "item": "50025"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 36000 WHERE `entry` <=> 60751 AND `item` <=> 50025 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61274", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61274 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61276", "item": "61616"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61276 AND `item` <=> 61616 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61276", "item": "61617"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61276 AND `item` <=> 61617 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61439", "item": "4363"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 61439 AND `item` <=> 4363 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61439", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 61439 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61439", "item": "4389"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 61439 AND `item` <=> 4389 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61454", "item": "785"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61454 AND `item` <=> 785 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61454", "item": "2449"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61454 AND `item` <=> 2449 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61454", "item": "3355"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61454 AND `item` <=> 3355 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61454", "item": "3356"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61454 AND `item` <=> 3356 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61454", "item": "3357"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61454 AND `item` <=> 3357 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61574", "item": "61677"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 61574 AND `item` <=> 61677 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61574", "item": "61678"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 61574 AND `item` <=> 61678 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61574", "item": "61679"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 3600 WHERE `entry` <=> 61574 AND `item` <=> 61679 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61846", "item": "6365"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 61846 AND `item` <=> 6365 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "61914", "item": "12799"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 14400 WHERE `entry` <=> 61914 AND `item` <=> 12799 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "80962", "item": "7070"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 30000 WHERE `entry` <=> 80962 AND `item` <=> 7070 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "81028", "item": "4382"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 10800 WHERE `entry` <=> 81028 AND `item` <=> 4382 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "91413", "item": "12249"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 91413 AND `item` <=> 12249 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "92221", "item": "83406"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 92221 AND `item` <=> 83406 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "92221", "item": "83407"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 92221 AND `item` <=> 83407 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "92221", "item": "83408"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 92221 AND `item` <=> 83408 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- npc_vendor {"entry": "92221", "item": "83409"} 
UPDATE `npc_vendor` SET `maxcount` = 1, `incrtime` = 7200 WHERE `entry` <=> 92221 AND `item` <=> 83409 AND (BINARY CAST(`maxcount` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`incrtime` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
