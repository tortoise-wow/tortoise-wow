-- ==============================================
-- FILE: stale_replay_repair_gameobject_template.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- gameobject_template {"entry": "12843"} Sunken Treasure Chest
UPDATE `gameobject_template` SET `faction` = 94, `size` = 0.75 WHERE `entry` <=> 12843 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "20809"} Ship, Spadowprey Village - Moonhoof Village
UPDATE `gameobject_template` SET `flags` = 40 WHERE `entry` <=> 20809 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "176296"} Portal to Stormwind
UPDATE `gameobject_template` SET `flags` = 2 WHERE `entry` <=> 176296 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "178854"} Meeting Stone
UPDATE `gameobject_template` SET `faction` = 35 WHERE `entry` <=> 178854 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "179598"} Meeting Stone
UPDATE `gameobject_template` SET `faction` = 35 WHERE `entry` <=> 179598 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "179865"} Meeting Stone
UPDATE `gameobject_template` SET `faction` = 35 WHERE `entry` <=> 179865 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "180336"} Pet Rock
UPDATE `gameobject_template` SET `size` = 2 WHERE `entry` <=> 180336 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "223364"} Bloodstone
UPDATE `gameobject_template` SET `size` = 4 WHERE `entry` <=> 223364 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300420"} Volatile Fungus
UPDATE `gameobject_template` SET `faction` = 14, `size` = 1.5 WHERE `entry` <=> 300420 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300421"} Forgewright's Brazier
UPDATE `gameobject_template` SET `flags` = 36 WHERE `entry` <=> 300421 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "300602"} Brazier of Corruption
UPDATE `gameobject_template` SET `flags` = 32, `size` = 1.03788 WHERE `entry` <=> 300602 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300603"} Brazier of Defilement
UPDATE `gameobject_template` SET `flags` = 32, `size` = 1.03788 WHERE `entry` <=> 300603 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300604"} Brazier of Decay
UPDATE `gameobject_template` SET `flags` = 32, `size` = 1.03788 WHERE `entry` <=> 300604 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300605"} Brazier of Putrification
UPDATE `gameobject_template` SET `flags` = 32, `size` = 1.03788 WHERE `entry` <=> 300605 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300606"} Barrier of Ursol
UPDATE `gameobject_template` SET `flags` = 48, `size` = 3 WHERE `entry` <=> 300606 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300608"} Azureshimmer Feather
UPDATE `gameobject_template` SET `flags` = 4, `size` = 1.4 WHERE `entry` <=> 300608 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "300609"} Relic of Elunaris
UPDATE `gameobject_template` SET `flags` = 4, `size` = 4 WHERE `entry` <=> 300609 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "1000512"} Demon Gate
UPDATE `gameobject_template` SET `script_name` = CONVERT(X'676f5f7761726c6f636b5f64656d6f6e5f67617465' USING utf8mb4) WHERE `entry` <=> 1000512 AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'' USING utf8mb4));
-- gameobject_template {"entry": "2006138"} World KHAZMODAN BLACKROCK PASSIVEDOODADS Doors DarkIronPortcullis.mdx
UPDATE `gameobject_template` SET `flags` = 48, `script_name` = CONVERT(X'676f5f73746f726d77726f756768745f7365637265745f646f6f72' USING utf8mb4) WHERE `entry` <=> 2006138 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'' USING utf8mb4));
-- gameobject_template {"entry": "2020134"} Quillboar Cache
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020134 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020148"} Honey Drop Inn
UPDATE `gameobject_template` SET `size` = 0.2 WHERE `entry` <=> 2020148 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020149"} The Plump Pumpkin
UPDATE `gameobject_template` SET `size` = 0.4 WHERE `entry` <=> 2020149 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020154"} Magilou's Almanac
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020154 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020155"} Dusty Urn
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020155 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020156"} Crawford Family Chest
UPDATE `gameobject_template` SET `flags` = 2 WHERE `entry` <=> 2020156 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020157"} Suspicious Pumpkin
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020157 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020158"} Half-Buried Container
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020158 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020159"} Weird Barrel
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020159 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020160"} Dragon Vault
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020160 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020161"} Flame of Farrak
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020161 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020162"} Inconspicuous Documents
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020162 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020163"} Blackrock Coffer
UPDATE `gameobject_template` SET `flags` = 6 WHERE `entry` <=> 2020163 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020164"} Deathcap
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020164 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020165"} Widow's Frill
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020165 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020168"} Shatterblade Bounty Board
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020168 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020169"} Autumny Leaf Pile
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020169 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020170"} Black Sealed Chest
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.5 WHERE `entry` <=> 2020170 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020171"} Ingvild's Pie
UPDATE `gameobject_template` SET `flags` = 4, `size` = 3 WHERE `entry` <=> 2020171 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020172"} Crawford Wardrobe
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020172 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020173"} Dark Iron Gunpowder Keg
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020173 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020174"} Stolen Horse Feed Sack
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020174 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020175"} Grimmen Greens
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020175 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020176"} Winter's Kiss
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.6 WHERE `entry` <=> 2020176 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020177"} Muddy Journal
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020177 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020178"} Unremarkable Barrel
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020178 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020179"} Unremarkable Crate
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020179 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020180"} Unremarkable Box
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020180 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020181"} Wooden Plank
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020181 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020182"} Tidal Altar
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020182 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020183"} Unsent Letter
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020183 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020189"} Stolgaz Documents
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020189 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020190"} Dark Iron Components
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020190 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020191"} Intricate Parts
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020191 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020192"} Reachroot
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020192 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020193"} Flame of Dagnoth
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020193 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020196"} Tempered Blade
UPDATE `gameobject_template` SET `faction` = 101, `flags` = 32 WHERE `entry` <=> 2020196 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020197"} Eternal Flame
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.4 WHERE `entry` <=> 2020197 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020198"} Redbrand Tablet
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020198 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020214"} Dathronag the Mighty
UPDATE `gameobject_template` SET `size` = 2 WHERE `entry` <=> 2020214 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020215"} Algoron the Uniter
UPDATE `gameobject_template` SET `size` = 2 WHERE `entry` <=> 2020215 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020216"} Lower Reserve Memorial
UPDATE `gameobject_template` SET `size` = 2 WHERE `entry` <=> 2020216 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020217"} Kwabit's Chest
UPDATE `gameobject_template` SET `flags` = 6 WHERE `entry` <=> 2020217 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020219"} Chest of Dathronag
UPDATE `gameobject_template` SET `size` = 1.8 WHERE `entry` <=> 2020219 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020220"} Pedestal of Unity
UPDATE `gameobject_template` SET `flags` = 32, `size` = 1.4 WHERE `entry` <=> 2020220 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020221"} Sermon of the Church
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020221 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020222"} Dusty Wardrobe
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020222 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020224"} Darkedged Bone
UPDATE `gameobject_template` SET `size` = 0.2 WHERE `entry` <=> 2020224 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020225"} Control Panel Alpha
UPDATE `gameobject_template` SET `flags` = 36, `script_name` = CONVERT(X'676f5f736c69636b7769636b5f636f6e74726f6c5f70616e656c' USING utf8mb4) WHERE `entry` <=> 2020225 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020226"} Control Panel Beta
UPDATE `gameobject_template` SET `flags` = 36, `script_name` = CONVERT(X'676f5f736c69636b7769636b5f636f6e74726f6c5f70616e656c' USING utf8mb4) WHERE `entry` <=> 2020226 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020227"} Control Panel Gamma
UPDATE `gameobject_template` SET `flags` = 36, `script_name` = CONVERT(X'676f5f736c69636b7769636b5f636f6e74726f6c5f70616e656c' USING utf8mb4) WHERE `entry` <=> 2020227 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020228"} Control Panel Delta
UPDATE `gameobject_template` SET `flags` = 36, `script_name` = CONVERT(X'676f5f736c69636b7769636b5f636f6e74726f6c5f70616e656c' USING utf8mb4) WHERE `entry` <=> 2020228 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020230"} Daria Balor, Protector of our Home
UPDATE `gameobject_template` SET `size` = 2 WHERE `entry` <=> 2020230 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020231"} Earthen Ring Memorial
UPDATE `gameobject_template` SET `size` = 0.8 WHERE `entry` <=> 2020231 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020232"} Lower Reserve Gate
UPDATE `gameobject_template` SET `flags` = 34, `size` = 1.15 WHERE `entry` <=> 2020232 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020233"} Stormwrought Castle Gate
UPDATE `gameobject_template` SET `flags` = 34, `size` = 0.8 WHERE `entry` <=> 2020233 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020234"} Moonshrine Bloom
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.4 WHERE `entry` <=> 2020234 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020235"} Crate of Balor Moonshine
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020235 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020236"} Chamber Lever
UPDATE `gameobject_template` SET `flags` = 32, `script_name` = CONVERT(X'676f5f73746f726d77726f756768745f7365637265745f646f6f72' USING utf8mb4) WHERE `entry` <=> 2020236 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`script_name` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020237"} Replenishable Mana Crystal
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020237 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020238"} Earthen Relic
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.6 WHERE `entry` <=> 2020238 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020239"} Groldan's Stash
UPDATE `gameobject_template` SET `faction` = 1 WHERE `entry` <=> 2020239 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020240"} Rusty Locket
UPDATE `gameobject_template` SET `flags` = 32, `size` = 0.2 WHERE `entry` <=> 2020240 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020243"} Banner of Baggoth
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020243 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020244"} Dragonmaw Supply Crate
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020244 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020245"} Brangar's Grain Crate
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020245 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020246"} Brangar's Journal
UPDATE `gameobject_template` SET `faction` = 1 WHERE `entry` <=> 2020246 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020248"} Stone Brick
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.6 WHERE `entry` <=> 2020248 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020249"} Stormwrought Crystal
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020249 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020251"} Dragonmaw Retreat Reserve Gate
UPDATE `gameobject_template` SET `faction` = 114, `flags` = 33, `size` = 0.909789 WHERE `entry` <=> 2020251 AND (BINARY CAST(`faction` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020253"} Servo-Motor 3000
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020253 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020260"} Northwind Wild Grapes
UPDATE `gameobject_template` SET `size` = 1.4 WHERE `entry` <=> 2020260 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020264"} Riverhorn Heirloom
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020264 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020265"} Maghan's Cargo
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.6 WHERE `entry` <=> 2020265 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020266"} Draenethyst Formation
UPDATE `gameobject_template` SET `size` = 0.6 WHERE `entry` <=> 2020266 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020277"} Simple Wood Tree
UPDATE `gameobject_template` SET `size` = 0.2 WHERE `entry` <=> 2020277 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020280"} Simple Wood Tree
UPDATE `gameobject_template` SET `size` = 0.4 WHERE `entry` <=> 2020280 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020284"} Bright Wood Tree
UPDATE `gameobject_template` SET `size` = 0.5 WHERE `entry` <=> 2020284 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020299"} Star Wood Tree
UPDATE `gameobject_template` SET `size` = 0.6 WHERE `entry` <=> 2020299 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020300"} Dead Wood Tree
UPDATE `gameobject_template` SET `size` = 0.3 WHERE `entry` <=> 2020300 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020307"} Narkogg’s Medicine Pouch
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020307 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020308"} Putrid Flower
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020308 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020312"} WANTED!
UPDATE `gameobject_template` SET `flags` = 32, `size` = 1.2 WHERE `entry` <=> 2020312 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020313"} Draenei Wares
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.8 WHERE `entry` <=> 2020313 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020317"} Horn Fragment
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.2 WHERE `entry` <=> 2020317 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020319"} Moonwhisper Berry Bush
UPDATE `gameobject_template` SET `size` = 0.8 WHERE `entry` <=> 2020319 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020320"} Windhorn Relic
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020320 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020321"} Idol of Neptulon
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020321 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020322"} Maras’ethil Relic
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020322 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020323"} Maras’ethil Relic
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020323 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020324"} Maras’ethil Relic
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020324 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020325"} Maras’ethil Relic
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020325 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020326"} Scroll of Lo’sho
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020326 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020327"} Mothsilk Cocoon
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.7 WHERE `entry` <=> 2020327 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020332"} Mu’sha, the Left Eye of the Earthmother
UPDATE `gameobject_template` SET `size` = 1.3 WHERE `entry` <=> 2020332 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020333"} An’she, the Right Eye of the Earthmother
UPDATE `gameobject_template` SET `size` = 1.3 WHERE `entry` <=> 2020333 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020335"} Maras’ethil Runestone
UPDATE `gameobject_template` SET `flags` = 32 WHERE `entry` <=> 2020335 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020337"} Informant’s Belongings
UPDATE `gameobject_template` SET `flags` = 2 WHERE `entry` <=> 2020337 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020338"} Airfield Supplies
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020338 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020339"} Tablet of Kaz'gan
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020339 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020341"} Sunsworn Supplies
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020341 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020342"} Meteor Shard
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020342 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020343"} Forlorn Crate
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020343 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020344"} Stolen Moonhoof Goods
UPDATE `gameobject_template` SET `flags` = 4, `size` = 0.5 WHERE `entry` <=> 2020344 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020345"} Nendis Memento
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020345 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020346"} Nendis Memento
UPDATE `gameobject_template` SET `flags` = 4 WHERE `entry` <=> 2020346 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- gameobject_template {"entry": "2020411"} Wilderness Survival
UPDATE `gameobject_template` SET `size` = 2 WHERE `entry` <=> 2020411 AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- gameobject_template {"entry": "2020420"} Elunaris Relic
UPDATE `gameobject_template` SET `flags` = 4, `size` = 2 WHERE `entry` <=> 2020420 AND (BINARY CAST(`flags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`size` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
