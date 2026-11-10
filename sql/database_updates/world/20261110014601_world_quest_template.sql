-- ==============================================
-- FILE: stale_replay_repair_quest_template.sql
-- GENERATED: 20261110014601
-- ==============================================
-- Repair for the October 6 out-of-order replay incident: restores fields that
-- the replayed May migrations rolled back on rows that still hold the exact
-- stale May value. Each UPDATE is guarded on the audited value, so this is a
-- no-op on a healthy database (0 rows affected) and safe to re-run.
-- Only upstream keys; no custom/recruiter rows. Item 17078 touches
-- disenchant_id only.
-- quest_template {"entry": "5"} Jitters' Growling Gut
UPDATE `quest_template` SET `PrevQuestId` = 163 WHERE `entry` <=> 5 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6"} Bounty on Garrick Padfoot
UPDATE `quest_template` SET `PrevQuestId` = 18 WHERE `entry` <=> 6 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "10"} The Scrimshank Redemption
UPDATE `quest_template` SET `PrevQuestId` = 82 WHERE `entry` <=> 10 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "15"} Investigate Echo Ridge
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 15 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "17"} Uldaman Reagent Run
UPDATE `quest_template` SET `PrevQuestId` = 2500 WHERE `entry` <=> 17 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "18"} Brotherhood of Thieves
UPDATE `quest_template` SET `PrevQuestId` = 783 WHERE `entry` <=> 18 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "23"} Ursangous's Paw
UPDATE `quest_template` SET `PrevQuestId` = 6383 WHERE `entry` <=> 23 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "24"} Shadumbra's Head
UPDATE `quest_template` SET `PrevQuestId` = 6383 WHERE `entry` <=> 24 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "30"} Trial of the Sea Lion
UPDATE `quest_template` SET `ReqSourceId1` = 15883, `ReqSourceId2` = 15882, `ReqSourceCount2` = 1 WHERE `entry` <=> 30 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3135383835' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "28"} Trial of the Lake
UPDATE `quest_template` SET `ReqSourceId1` = 15877, `ReqSourceCount1` = 1 WHERE `entry` <=> 28 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "32"} Rise of the Silithid
UPDATE `quest_template` SET `PrevQuestId` = 113 WHERE `entry` <=> 32 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "54"} Report to Goldshire
UPDATE `quest_template` SET `PrevQuestId` = 21 WHERE `entry` <=> 54 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "63"} Call of Water
UPDATE `quest_template` SET `PrevQuestId` = 220 WHERE `entry` <=> 63 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "81"} Ripple Delivery
UPDATE `quest_template` SET `PrevQuestId` = 77 WHERE `entry` <=> 81 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "105"} Alas, Andorhal
UPDATE `quest_template` SET `PrevQuestId` = 5098 WHERE `entry` <=> 105 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "114"} The Escape
UPDATE `quest_template` SET `PrevQuestId` = 112 WHERE `entry` <=> 114 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "115"} Shadow Magic
UPDATE `quest_template` SET `PrevQuestId` = 20 WHERE `entry` <=> 115 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "122"} Underbelly Scales
UPDATE `quest_template` SET `PrevQuestId` = 118 WHERE `entry` <=> 122 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "124"} A Baying of Gnolls
UPDATE `quest_template` SET `PrevQuestId` = 118 WHERE `entry` <=> 124 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "143"} Messenger to Westfall
UPDATE `quest_template` SET `PrevQuestId` = 121 WHERE `entry` <=> 143 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "145"} Messenger to Darkshire
UPDATE `quest_template` SET `PrevQuestId` = 144 WHERE `entry` <=> 145 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "147"} Manhunt
UPDATE `quest_template` SET `PrevQuestId` = 123 WHERE `entry` <=> 147 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "148"} Supplies from Darkshire
UPDATE `quest_template` SET `PrevQuestId` = 165 WHERE `entry` <=> 148 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "155"} The Defias Brotherhood
UPDATE `quest_template` SET `PrevQuestId` = 142 WHERE `entry` <=> 155 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "162"} Rise of the Silithid
UPDATE `quest_template` SET `PrevQuestId` = 113 WHERE `entry` <=> 162 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "170"} A New Threat
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 170 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "185"} Tiger Mastery
UPDATE `quest_template` SET `PrevQuestId` = 583 WHERE `entry` <=> 185 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "190"} Panther Mastery
UPDATE `quest_template` SET `PrevQuestId` = 583 WHERE `entry` <=> 190 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "194"} Raptor Mastery
UPDATE `quest_template` SET `PrevQuestId` = 583 WHERE `entry` <=> 194 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "206"} Mai'Zoth
UPDATE `quest_template` SET `PrevQuestId` = 205 WHERE `entry` <=> 206 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "209"} Skullsplitter Tusks
UPDATE `quest_template` SET `PrevQuestId` = 189 WHERE `entry` <=> 209 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "211"} Alas, Andorhal
UPDATE `quest_template` SET `PrevQuestId` = 5097 WHERE `entry` <=> 211 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "214"} Red Silk Bandanas
UPDATE `quest_template` SET `PrevQuestId` = 155 WHERE `entry` <=> 214 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "217"} In Defense of the King's Lands
UPDATE `quest_template` SET `PrevQuestId` = 263 WHERE `entry` <=> 217 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "237"} In Defense of the King's Lands
UPDATE `quest_template` SET `PrevQuestId` = 224 WHERE `entry` <=> 237 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "239"} Westbrook Garrison Needs Help!
UPDATE `quest_template` SET `PrevQuestId` = 76 WHERE `entry` <=> 239 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "249"} Morganth
UPDATE `quest_template` SET `PrevQuestId` = 248 WHERE `entry` <=> 249 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "253"} Bride of the Embalmer
UPDATE `quest_template` SET `PrevQuestId` = 252 WHERE `entry` <=> 253 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "254"} Digging Through the Dirt
UPDATE `quest_template` SET `PrevQuestId` = -253 WHERE `entry` <=> 254 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "261"} Down the Scarlet Path
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 261 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36313431' USING utf8mb4));
-- quest_template {"entry": "263"} In Defense of the King's Lands
UPDATE `quest_template` SET `PrevQuestId` = 237 WHERE `entry` <=> 263 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "271"} Vyrin's Revenge
UPDATE `quest_template` SET `PrevQuestId` = 258 WHERE `entry` <=> 271 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "272"} Trial of the Sea Lion
UPDATE `quest_template` SET `ReqSourceId1` = 15883, `ReqSourceId2` = 15882, `ReqSourceCount2` = 1 WHERE `entry` <=> 272 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3135383835' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "29"} Trial of the Lake
UPDATE `quest_template` SET `ReqSourceId1` = 15877, `ReqSourceCount1` = 1 WHERE `entry` <=> 29 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "276"} Tramping Paws
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 276 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'343633' USING utf8mb4));
-- quest_template {"entry": "287"} Frostmane Hold
UPDATE `quest_template` SET `PrevQuestId` = 420 WHERE `entry` <=> 287 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "297"} Gathering Idols
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 297 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'343336' USING utf8mb4));
-- quest_template {"entry": "308"} Distracting Jarven
UPDATE `quest_template` SET `PrevQuestId` = 403 WHERE `entry` <=> 308 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "309"} Protecting the Shipment
UPDATE `quest_template` SET `PrevQuestId` = 454 WHERE `entry` <=> 309 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "338"} The Green Hills of Stranglethorn
UPDATE `quest_template` SET `PrevQuestId` = 583 WHERE `entry` <=> 338 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "339"} Chapter I
UPDATE `quest_template` SET `PrevQuestId` = -338 WHERE `entry` <=> 339 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "340"} Chapter II
UPDATE `quest_template` SET `PrevQuestId` = -338 WHERE `entry` <=> 340 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "341"} Chapter III
UPDATE `quest_template` SET `PrevQuestId` = -338 WHERE `entry` <=> 341 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "342"} Chapter IV
UPDATE `quest_template` SET `PrevQuestId` = -338 WHERE `entry` <=> 342 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "355"} Speak with Sevren
UPDATE `quest_template` SET `PrevQuestId` = 354 WHERE `entry` <=> 355 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "359"} Forsaken Duties
UPDATE `quest_template` SET `PrevQuestId` = 358 WHERE `entry` <=> 359 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "374"} Proof of Demise
UPDATE `quest_template` SET `PrevQuestId` = 427 WHERE `entry` <=> 374 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "378"} The Fury Runs Deep
UPDATE `quest_template` SET `PrevQuestId` = 303 WHERE `entry` <=> 378 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "380"} Night Web's Hollow
UPDATE `quest_template` SET `PrevQuestId` = 376 WHERE `entry` <=> 380 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "397"} You Have Served Us Well
UPDATE `quest_template` SET `PrevQuestId` = 336 WHERE `entry` <=> 397 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "401"} Wait for Sirra to Finish
UPDATE `quest_template` SET `PrevQuestId` = 251 WHERE `entry` <=> 401 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "403"} Guarded Thunderbrew Barrel
UPDATE `quest_template` SET `PrevQuestId` = -310 WHERE `entry` <=> 403 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "405"} The Prodigal Lich
UPDATE `quest_template` SET `PrevQuestId` = 358 WHERE `entry` <=> 405 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "410"} The Dormant Shade
UPDATE `quest_template` SET `PrevQuestId` = -409 WHERE `entry` <=> 410 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "415"} Rejold's New Brew
UPDATE `quest_template` SET `PrevQuestId` = 315 WHERE `entry` <=> 415 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "425"} Ivar the Foul
UPDATE `quest_template` SET `PrevQuestId` = 430 WHERE `entry` <=> 425 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "426"} The Mills Overrun
UPDATE `quest_template` SET `PrevQuestId` = 404 WHERE `entry` <=> 426 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "427"} At War With The Scarlet Crusade
UPDATE `quest_template` SET `PrevQuestId` = 383 WHERE `entry` <=> 427 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "431"} Candles of Beckoning
UPDATE `quest_template` SET `PrevQuestId` = -409 WHERE `entry` <=> 431 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "434"} The Attack!
UPDATE `quest_template` SET `PrevQuestId` = 2746 WHERE `entry` <=> 434 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "440"} The Engraved Ring
UPDATE `quest_template` SET `PrevQuestId` = 439 WHERE `entry` <=> 440 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "446"} Thule Ravenclaw
UPDATE `quest_template` SET `PrevQuestId` = 444 WHERE `entry` <=> 446 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "455"} The Algaz Gauntlet
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 455 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'343638' USING utf8mb4));
-- quest_template {"entry": "457"} The Balance of Nature
UPDATE `quest_template` SET `PrevQuestId` = 456 WHERE `entry` <=> 457 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "489"} Seek Redemption!
UPDATE `quest_template` SET `PrevQuestId` = 488 WHERE `entry` <=> 489 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "498"} The Rescue
UPDATE `quest_template` SET `ReqSourceId1` = 3467, `ReqSourceId2` = 3499, `ReqSourceCount1` = 1, `ReqSourceCount2` = 1, `ReqCreatureOrGOId1` = -1721, `ReqCreatureOrGOId2` = -1722 WHERE `entry` <=> 498 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "526"} Lightforge Ingots
UPDATE `quest_template` SET `PrevQuestId` = 324 WHERE `entry` <=> 526 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "532"} Battle of Hillsbrad
UPDATE `quest_template` SET `ReqCreatureOrGOId3` = -1761 WHERE `entry` <=> 532 AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "546"} Souvenirs of Death
UPDATE `quest_template` SET `PrevQuestId` = 527 WHERE `entry` <=> 546 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "553"} Helcular's Revenge
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -1768, `ReqCreatureOrGOId2` = -1769, `ReqCreatureOrGOId3` = -1770 WHERE `entry` <=> 553 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "557"} Bracers of Binding
UPDATE `quest_template` SET `PrevQuestId` = 556 WHERE `entry` <=> 557 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "576"} Keep An Eye Out
UPDATE `quest_template` SET `PrevQuestId` = 595 WHERE `entry` <=> 576 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "577"} Some Assembly Required
UPDATE `quest_template` SET `PrevQuestId` = 575 WHERE `entry` <=> 577 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "584"} Bloodscalp Clan Heads
UPDATE `quest_template` SET `PrevQuestId` = 582 WHERE `entry` <=> 584 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "585"} Speaking with Nezzliok
UPDATE `quest_template` SET `PrevQuestId` = 584 WHERE `entry` <=> 585 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "586"} Speaking with Gan'zulah
UPDATE `quest_template` SET `PrevQuestId` = 584 WHERE `entry` <=> 586 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "587"} Up to Snuff
UPDATE `quest_template` SET `PrevQuestId` = 595 WHERE `entry` <=> 587 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "590"} A Rogue's Deal
UPDATE `quest_template` SET `PrevQuestId` = 8 WHERE `entry` <=> 590 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "592"} Saving Yenniku
UPDATE `quest_template` SET `PrevQuestId` = 591 WHERE `entry` <=> 592 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "593"} Filling the Soul Gem
UPDATE `quest_template` SET `PrevQuestId` = -592 WHERE `entry` <=> 593 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "598"} Split Bone Necklace
UPDATE `quest_template` SET `PrevQuestId` = 596 WHERE `entry` <=> 598 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "603"} Ansirem's Key
UPDATE `quest_template` SET `PrevQuestId` = 602 WHERE `entry` <=> 603 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "613"} Cracking Maury's Foot
UPDATE `quest_template` SET `PrevQuestId` = 609 WHERE `entry` <=> 613 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "615"} The Captain's Cutlass
UPDATE `quest_template` SET `PrevQuestId` = 8552 WHERE `entry` <=> 615 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "623"} Akiris by the Bundle
UPDATE `quest_template` SET `PrevQuestId` = 617 WHERE `entry` <=> 623 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "633"} The Thandol Span
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -2704 WHERE `entry` <=> 633 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "641"} Sigil of Thoradin
UPDATE `quest_template` SET `PrevQuestId` = 640 WHERE `entry` <=> 641 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "653"} Myzrael's Allies
UPDATE `quest_template` SET `PrevQuestId` = 652 WHERE `entry` <=> 653 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "654"} Tanaris Field Sampling
UPDATE `quest_template` SET `PrevQuestId` = 379, `ReqSourceId1` = 9437, `ReqSourceId2` = 9439, `ReqSourceId3` = 9442, `ReqSourceCount1` = 999, `ReqSourceCount2` = 999, `ReqSourceCount3` = 999 WHERE `entry` <=> 654 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'39343430' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'39343431' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'39343338' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38' USING utf8mb4));
-- quest_template {"entry": "656"} Summoning the Princess
UPDATE `quest_template` SET `PrevQuestId` = 692 WHERE `entry` <=> 656 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "660"} Hints of a New Plague?
UPDATE `quest_template` SET `PrevQuestId` = 657 WHERE `entry` <=> 660 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "662"} Deep Sea Salvage
UPDATE `quest_template` SET `PrevQuestId` = 663 WHERE `entry` <=> 662 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "667"} Death From Below
UPDATE `quest_template` SET `PrevQuestId` = 670 WHERE `entry` <=> 667 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "673"} Foul Magics
UPDATE `quest_template` SET `PrevQuestId` = 671 WHERE `entry` <=> 673 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "680"} The Real Threat
UPDATE `quest_template` SET `PrevQuestId` = 678 WHERE `entry` <=> 680 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "682"} Stromgarde Badges
UPDATE `quest_template` SET `PrevQuestId` = 681 WHERE `entry` <=> 682 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "683"} Sara Balloo's Plea
UPDATE `quest_template` SET `PrevQuestId` = 637 WHERE `entry` <=> 683 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "686"} A King's Tribute
UPDATE `quest_template` SET `PrevQuestId` = 683 WHERE `entry` <=> 686 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "687"} Theldurin the Lost
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 687 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'363533' USING utf8mb4));
-- quest_template {"entry": "688"} Myzrael's Allies
UPDATE `quest_template` SET `PrevQuestId` = 652 WHERE `entry` <=> 688 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "691"} Worth Its Weight in Gold
UPDATE `quest_template` SET `PrevQuestId` = 690 WHERE `entry` <=> 691 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "693"} Wand over Fist
UPDATE `quest_template` SET `PrevQuestId` = 691 WHERE `entry` <=> 693 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "694"} Trelane's Defenses
UPDATE `quest_template` SET `PrevQuestId` = 693 WHERE `entry` <=> 694 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "700"} A King's Tribute
UPDATE `quest_template` SET `PrevQuestId` = 689 WHERE `entry` <=> 700 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "715"} Liquid Stone
UPDATE `quest_template` SET `PrevQuestId` = 714 WHERE `entry` <=> 715 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "716"} Stone Is Better than Cloth
UPDATE `quest_template` SET `PrevQuestId` = 712 WHERE `entry` <=> 716 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "725"} Passing Word of a Threat
UPDATE `quest_template` SET `PrevQuestId` = 724 WHERE `entry` <=> 725 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "727"} To Ironforge for Yagyin's Digest
UPDATE `quest_template` SET `PrevQuestId` = 709 WHERE `entry` <=> 727 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "728"} To the Undercity for Yagyin's Digest
UPDATE `quest_template` SET `PrevQuestId` = 709 WHERE `entry` <=> 728 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "729"} The Absent Minded Prospector
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 729 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'373330' USING utf8mb4));
-- quest_template {"entry": "731"} The Absent Minded Prospector
UPDATE `quest_template` SET `PrevQuestId` = 729 WHERE `entry` <=> 731 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "732"} Tremors of the Earth
UPDATE `quest_template` SET `PrevQuestId` = 718 WHERE `entry` <=> 732 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "737"} Forbidden Knowledge
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 737 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'373335' USING utf8mb4));
-- quest_template {"entry": "746"} Dwarven Digging
UPDATE `quest_template` SET `ReqSourceId1` = 4702 WHERE `entry` <=> 746 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34373033' USING utf8mb4));
-- quest_template {"entry": "750"} The Hunt Continues
UPDATE `quest_template` SET `PrevQuestId` = 747 WHERE `entry` <=> 750 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "754"} Winterhoof Cleansing
UPDATE `quest_template` SET `PrevQuestId` = 748 WHERE `entry` <=> 754 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "756"} Thunderhorn Totem
UPDATE `quest_template` SET `PrevQuestId` = 754 WHERE `entry` <=> 756 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "758"} Thunderhorn Cleansing
UPDATE `quest_template` SET `PrevQuestId` = 756 WHERE `entry` <=> 758 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "759"} Wildmane Totem
UPDATE `quest_template` SET `PrevQuestId` = 758 WHERE `entry` <=> 759 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "760"} Wildmane Cleansing
UPDATE `quest_template` SET `PrevQuestId` = 759 WHERE `entry` <=> 760 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "764"} The Venture Co.
UPDATE `quest_template` SET `PrevQuestId` = 751 WHERE `entry` <=> 764 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "765"} Supervisor Fizsprocket
UPDATE `quest_template` SET `PrevQuestId` = 751 WHERE `entry` <=> 765 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "767"} Rite of Vision
UPDATE `quest_template` SET `PrevQuestId` = 763 WHERE `entry` <=> 767 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "769"} Kodo Hide Bag
UPDATE `quest_template` SET `PrevQuestId` = 768 WHERE `entry` <=> 769 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "772"} Rite of Vision
UPDATE `quest_template` SET `PrevQuestId` = 771 WHERE `entry` <=> 772 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "779"} Seal of the Earth
UPDATE `quest_template` SET `PrevQuestId` = -717 WHERE `entry` <=> 779 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "786"} Thwarting Kolkar Aggression
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -3189, `ReqCreatureOrGOId2` = -3190, `ReqCreatureOrGOId3` = -3192 WHERE `entry` <=> 786 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "40346"} The Way of Spiritwalking III
UPDATE `quest_template` SET `PrevQuestId` = 40345 WHERE `entry` <=> 40346 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "788"} Cutting Teeth
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 788 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34363431' USING utf8mb4));
-- quest_template {"entry": "789"} Sting of the Scorpid
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 789 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "794"} Burning Blade Medallion
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 794 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31343939' USING utf8mb4));
-- quest_template {"entry": "795"} Seal of the Earth
UPDATE `quest_template` SET `PrevQuestId` = -793 WHERE `entry` <=> 795 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "806"} Dark Storms
UPDATE `quest_template` SET `PrevQuestId` = 823 WHERE `entry` <=> 806 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "813"} Finding the Antidote
UPDATE `quest_template` SET `PrevQuestId` = -812 WHERE `entry` <=> 813 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "828"} Margoz
UPDATE `quest_template` SET `PrevQuestId` = 806 WHERE `entry` <=> 828 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "829"} Neeru Fireblade
UPDATE `quest_template` SET `PrevQuestId` = 827 WHERE `entry` <=> 829 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "838"} Scholomance
UPDATE `quest_template` SET `PrevQuestId` = 5098 WHERE `entry` <=> 838 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "841"} Another Power Source?
UPDATE `quest_template` SET `PrevQuestId` = 379 WHERE `entry` <=> 841 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "844"} Plainstrider Menace
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 844 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'383630' USING utf8mb4));
-- quest_template {"entry": "847"} Guile of the Raptor
UPDATE `quest_template` SET `PrevQuestId` = 702 WHERE `entry` <=> 847 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "849"} Revenge of Gann
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -3644 WHERE `entry` <=> 849 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "851"} Verog the Dervish
UPDATE `quest_template` SET `PrevQuestId` = 850 WHERE `entry` <=> 851 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "853"} Apothecary Zamah
UPDATE `quest_template` SET `PrevQuestId` = 848 WHERE `entry` <=> 853 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "860"} Sergra Darkthorn
UPDATE `quest_template` SET `PrevQuestId` = 861 WHERE `entry` <=> 860 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "863"} The Escape
UPDATE `quest_template` SET `PrevQuestId` = 858 WHERE `entry` <=> 863 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "870"} The Forgotten Pools
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 870 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'383836' USING utf8mb4));
-- quest_template {"entry": "872"} The Disruption Ends
UPDATE `quest_template` SET `PrevQuestId` = 871 WHERE `entry` <=> 872 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "877"} The Stagnant Oasis
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -3737 WHERE `entry` <=> 877 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "879"} Betrayal from Within
UPDATE `quest_template` SET `PrevQuestId` = 5052 WHERE `entry` <=> 879 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "882"} Ishamuhale
UPDATE `quest_template` SET `ReqSourceId1` = 10338 WHERE `entry` <=> 882 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35313031' USING utf8mb4));
-- quest_template {"entry": "889"} Spirit of the Wind
UPDATE `quest_template` SET `PrevQuestId` = 5052 WHERE `entry` <=> 889 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "900"} Samophlange
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -4072, `ReqCreatureOrGOId2` = -61936, `ReqCreatureOrGOId3` = -61935 WHERE `entry` <=> 900 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "905"} The Angry Scytheclaws
UPDATE `quest_template` SET `ReqSourceId1` = 5165, `ReqSourceCount1` = 999999, `ReqCreatureOrGOId2` = -6907, `ReqCreatureOrGOId3` = -6908, `ReqCreatureOrGOId4` = -6906 WHERE `entry` <=> 905 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "908"} Amongst the Ruins
UPDATE `quest_template` SET `PrevQuestId` = 6563 WHERE `entry` <=> 908 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "910"} Down at the Docks
UPDATE `quest_template` SET `PrevQuestId` = 172 WHERE `entry` <=> 910 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "911"} Gateway to the Frontier
UPDATE `quest_template` SET `PrevQuestId` = 172 WHERE `entry` <=> 911 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "918"} Timberling Seeds
UPDATE `quest_template` SET `PrevQuestId` = 997 WHERE `entry` <=> 918 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "920"} Tenaron's Summons
UPDATE `quest_template` SET `PrevQuestId` = 917 WHERE `entry` <=> 920 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8434"} Battle of Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8429 WHERE `entry` <=> 8434 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "926"} Flawed Power Stone
UPDATE `quest_template` SET `PrevQuestId` = -924 WHERE `entry` <=> 926 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "940"} Teldrassil
UPDATE `quest_template` SET `PrevQuestId` = 937 WHERE `entry` <=> 940 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "941"} Planting the Heart
UPDATE `quest_template` SET `PrevQuestId` = 927 WHERE `entry` <=> 941 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "951"} Mathystra Relics
UPDATE `quest_template` SET `PrevQuestId` = 950 WHERE `entry` <=> 951 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "953"} The Fall of Ameth'Aran
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -17188, `ReqCreatureOrGOId2` = -17189 WHERE `entry` <=> 953 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "957"} Bashal'Aran
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -16393 WHERE `entry` <=> 957 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "961"} Onu is meditating
UPDATE `quest_template` SET `PrevQuestId` = -944 WHERE `entry` <=> 961 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "973"} The Tower of Althalaxx
UPDATE `quest_template` SET `PrevQuestId` = 970 WHERE `entry` <=> 973 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "976"} Supplies to Auberdine
UPDATE `quest_template` SET `PrevQuestId` = 973 WHERE `entry` <=> 976 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "978"} Moontouched Wildkin
UPDATE `quest_template` SET `PrevQuestId` = 3661 WHERE `entry` <=> 978 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "986"} A Lost Master
UPDATE `quest_template` SET `PrevQuestId` = 985 WHERE `entry` <=> 986 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "990"} Trek to Ashenvale
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 990 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'393934' USING utf8mb4));
-- quest_template {"entry": "994"} Escape Through Force
UPDATE `quest_template` SET `PrevQuestId` = 993 WHERE `entry` <=> 994 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "995"} Escape Through Stealth
UPDATE `quest_template` SET `PrevQuestId` = 993 WHERE `entry` <=> 995 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1009"} Ruuzel
UPDATE `quest_template` SET `PrevQuestId` = 1007 WHERE `entry` <=> 1009 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1011"} Forsaken Diseases
UPDATE `quest_template` SET `PrevQuestId` = 4581 WHERE `entry` <=> 1011 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1012"} Insane Druids
UPDATE `quest_template` SET `PrevQuestId` = 1011 WHERE `entry` <=> 1012 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1016"} Elemental Bracers
UPDATE `quest_template` SET `ReqSourceId1` = 12220, `ReqSourceCount1` = 5 WHERE `entry` <=> 1016 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35343535' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- quest_template {"entry": "1020"} Orendil's Cure
UPDATE `quest_template` SET `PrevQuestId` = 1010 WHERE `entry` <=> 1020 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1022"} The Howling Vale
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -19027 WHERE `entry` <=> 1022 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "1024"} Raene's Cleansing
UPDATE `quest_template` SET `PrevQuestId` = 1023 WHERE `entry` <=> 1024 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1025"} An Aggressive Defense
UPDATE `quest_template` SET `PrevQuestId` = 1023 WHERE `entry` <=> 1025 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1026"} Raene's Cleansing
UPDATE `quest_template` SET `ReqSourceId1` = 5475 WHERE `entry` <=> 1026 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35343634' USING utf8mb4));
-- quest_template {"entry": "1032"} Satyr Slaying!
UPDATE `quest_template` SET `PrevQuestId` = 1031 WHERE `entry` <=> 1032 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1033"} Elune's Tear
UPDATE `quest_template` SET `PrevQuestId` = 1020 WHERE `entry` <=> 1033 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1034"} The Ruins of Stardust
UPDATE `quest_template` SET `PrevQuestId` = 1033 WHERE `entry` <=> 1034 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1035"} Fallen Sky Lake
UPDATE `quest_template` SET `PrevQuestId` = 1034 WHERE `entry` <=> 1035 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1043"} The Scythe of Elune
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -19030 WHERE `entry` <=> 1043 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "1045"} Raene's Cleansing
UPDATE `quest_template` SET `ReqSourceId1` = 5388, `ReqSourceCount1` = 1 WHERE `entry` <=> 1045 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1053"} In the Name of the Light
UPDATE `quest_template` SET `PrevQuestId` = 1052 WHERE `entry` <=> 1053 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1057"} Reclaiming the Charred Vale
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1057 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31303536' USING utf8mb4));
-- quest_template {"entry": "1060"} Letter to Jin'Zil
UPDATE `quest_template` SET `PrevQuestId` = 876 WHERE `entry` <=> 1060 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1062"} Goblin Invaders
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1062 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31303631' USING utf8mb4));
-- quest_template {"entry": "1063"} The Elder Crone
UPDATE `quest_template` SET `PrevQuestId` = 1062 WHERE `entry` <=> 1063 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1064"} Forsaken Aid
UPDATE `quest_template` SET `PrevQuestId` = 1063 WHERE `entry` <=> 1064 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1067"} Return to Thunder Bluff
UPDATE `quest_template` SET `PrevQuestId` = 1066 WHERE `entry` <=> 1067 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1068"} Shredding Machines
UPDATE `quest_template` SET `PrevQuestId` = 1062 WHERE `entry` <=> 1068 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1072"} An Old Colleague
UPDATE `quest_template` SET `PrevQuestId` = 1071 WHERE `entry` <=> 1072 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1075"} A Scroll from Mauren
UPDATE `quest_template` SET `PrevQuestId` = 1071 WHERE `entry` <=> 1075 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1079"} Covert Ops - Alpha
UPDATE `quest_template` SET `NextQuestInChain` = 1080, `ReqSourceId1` = 5695, `ReqSourceId2` = 5694, `ReqSourceId3` = 5693, `ReqSourceId4` = 5692, `ReqSourceCount2` = 1, `ReqSourceCount3` = 1, `ReqSourceCount4` = 1 WHERE `entry` <=> 1079 AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35373138' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1080"} Covert Ops - Beta
UPDATE `quest_template` SET `PrevQuestId` = 1079, `NextQuestInChain` = 1091 WHERE `entry` <=> 1080 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1082"} Update for Sentinel Thenysil
UPDATE `quest_template` SET `PrevQuestId` = 1084 WHERE `entry` <=> 1082 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1083"} Enraged Spirits
UPDATE `quest_template` SET `PrevQuestId` = 1091, `NextQuestInChain` = 1084 WHERE `entry` <=> 1083 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1084"} Wounded Ancients
UPDATE `quest_template` SET `PrevQuestId` = 1083, `NextQuestInChain` = 1082 WHERE `entry` <=> 1084 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1086"} The Flying Machine Airport
UPDATE `quest_template` SET `PrevQuestId` = 1067 WHERE `entry` <=> 1086 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1089"} The Den
UPDATE `quest_template` SET `ReqSourceId1` = 5687 WHERE `entry` <=> 1089 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35363839' USING utf8mb4));
-- quest_template {"entry": "1091"} Kaela's Update
UPDATE `quest_template` SET `PrevQuestId` = 1080, `NextQuestInChain` = 1083 WHERE `entry` <=> 1091 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1093"} Super Reaper 6000
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1093 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31343833' USING utf8mb4));
-- quest_template {"entry": "1103"} Call of Water
UPDATE `quest_template` SET `PrevQuestId` = -100 WHERE `entry` <=> 1103 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1106"} Martek the Exiled
UPDATE `quest_template` SET `PrevQuestId` = 1104 WHERE `entry` <=> 1106 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1107"} Encrusted Tail Fins
UPDATE `quest_template` SET `PrevQuestId` = 1105 WHERE `entry` <=> 1107 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1114"} Delivery to the Gnomes
UPDATE `quest_template` SET `PrevQuestId` = 1112 WHERE `entry` <=> 1114 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1115"} The Rumormonger
UPDATE `quest_template` SET `PrevQuestId` = 1114 WHERE `entry` <=> 1115 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1118"} Back to Booty Bay
UPDATE `quest_template` SET `PrevQuestId` = 1117 WHERE `entry` <=> 1118 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1120"} Get the Gnomes Drunk
UPDATE `quest_template` SET `PrevQuestId` = 1119 WHERE `entry` <=> 1120 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1121"} Get the Goblins Drunk
UPDATE `quest_template` SET `PrevQuestId` = 1119 WHERE `entry` <=> 1121 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1123"} Rabine Saturna
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1123 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31303030' USING utf8mb4));
-- quest_template {"entry": "1124"} Wasteland
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1124 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31313233' USING utf8mb4));
-- quest_template {"entry": "1126"} Hive in the Tower
UPDATE `quest_template` SET `ReqSourceId1` = 17345, `ReqSourceCount1` = 9999999 WHERE `entry` <=> 1126 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3137333436' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- quest_template {"entry": "1127"} Fool's Stout
UPDATE `quest_template` SET `PrevQuestId` = -1119 WHERE `entry` <=> 1127 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1133"} Journey to Astranaar
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1133 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31313332' USING utf8mb4));
-- quest_template {"entry": "1134"} Pridewings of Stonetalon
UPDATE `quest_template` SET `PrevQuestId` = 1008 WHERE `entry` <=> 1134 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1136"} Frostmaw
UPDATE `quest_template` SET `PrevQuestId` = 1131, `ReqSourceId1` = 5810 WHERE `entry` <=> 1136 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35383131' USING utf8mb4));
-- quest_template {"entry": "1137"} News for Fizzle
UPDATE `quest_template` SET `PrevQuestId` = 1108 WHERE `entry` <=> 1137 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1140"} The Tower of Althalaxx
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -19901, `ReqCreatureOrGOId2` = -20352 WHERE `entry` <=> 1140 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "1150"} Test of Endurance
UPDATE `quest_template` SET `ReqSourceId1` = 5845 WHERE `entry` <=> 1150 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35383433' USING utf8mb4));
-- quest_template {"entry": "1153"} A New Ore Sample
UPDATE `quest_template` SET `PrevQuestId` = 893 WHERE `entry` <=> 1153 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1170"} The Brood of Onyxia
UPDATE `quest_template` SET `PrevQuestId` = 1169 WHERE `entry` <=> 1170 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1172"} The Brood of Onyxia
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -20359 WHERE `entry` <=> 1172 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "1173"} Challenge Overlord Mok'Morokk
UPDATE `quest_template` SET `PrevQuestId` = 1172 WHERE `entry` <=> 1173 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1182"} Goblin Sponsorship
UPDATE `quest_template` SET `ReqSourceId1` = 5851 WHERE `entry` <=> 1182 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35383532' USING utf8mb4));
-- quest_template {"entry": "1185"} Under the Chitin Was...
UPDATE `quest_template` SET `PrevQuestId` = 6845 WHERE `entry` <=> 1185 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1190"} Keeping Pace
UPDATE `quest_template` SET `PrevQuestId` = 1137 WHERE `entry` <=> 1190 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1191"} Zamek's Distraction
UPDATE `quest_template` SET `PrevQuestId` = -1190 WHERE `entry` <=> 1191 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1192"} Indurium Ore
UPDATE `quest_template` SET `PrevQuestId` = 1194 WHERE `entry` <=> 1192 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1195"} The Sacred Flame
UPDATE `quest_template` SET `ReqSourceId1` = 5867 WHERE `entry` <=> 1195 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35383638' USING utf8mb4));
-- quest_template {"entry": "1239"} The Severed Head
UPDATE `quest_template` SET `PrevQuestId` = 1238 WHERE `entry` <=> 1239 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1250"} The Missing Diplomat
UPDATE `quest_template` SET `PrevQuestId` = 1249 WHERE `entry` <=> 1250 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1261"} Marg Speaks
UPDATE `quest_template` SET `PrevQuestId` = 1240 WHERE `entry` <=> 1261 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1267"} The Missing Diplomat
UPDATE `quest_template` SET `PrevQuestId` = 1324 WHERE `entry` <=> 1267 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1273"} Questioning Reethe
UPDATE `quest_template` SET `PrevQuestId` = 1269 WHERE `entry` <=> 1273 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1276"} The Black Shield
UPDATE `quest_template` SET `PrevQuestId` = 1323 WHERE `entry` <=> 1276 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1338"} Stormpike's Order
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1338 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31333339' USING utf8mb4));
-- quest_template {"entry": "1362"} The Kolkar of Desolace
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1362 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31333631' USING utf8mb4));
-- quest_template {"entry": "1364"} Mazen's Behest
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1364 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31333633' USING utf8mb4));
-- quest_template {"entry": "1365"} Khan Dez'hepah
UPDATE `quest_template` SET `PrevQuestId` = 1362 WHERE `entry` <=> 1365 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1366"} Centaur Bounty
UPDATE `quest_template` SET `PrevQuestId` = 1365 WHERE `entry` <=> 1366 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1369"} Broken Tears
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1369 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31333637' USING utf8mb4));
-- quest_template {"entry": "1370"} Stealing Supplies
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1370 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31333638' USING utf8mb4));
-- quest_template {"entry": "1375"} Khan Shaka
UPDATE `quest_template` SET `PrevQuestId` = 1371 WHERE `entry` <=> 1375 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1380"} Khan Hratha
UPDATE `quest_template` SET `ReqSourceId1` = 6074 WHERE `entry` <=> 1380 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36303737' USING utf8mb4));
-- quest_template {"entry": "1381"} Khan Hratha
UPDATE `quest_template` SET `ReqSourceId1` = 6074 WHERE `entry` <=> 1381 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36303737' USING utf8mb4));
-- quest_template {"entry": "1420"} Report to Helgrum
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1420 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31343138' USING utf8mb4));
-- quest_template {"entry": "1433"} Alliance Relations
UPDATE `quest_template` SET `PrevQuestId` = 1432 WHERE `entry` <=> 1433 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1434"} Befouled by Satyr
UPDATE `quest_template` SET `PrevQuestId` = 1432 WHERE `entry` <=> 1434 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1435"} The Burning of Spirits
UPDATE `quest_template` SET `ReqSourceId1` = 6766, `ReqSourceCount1` = 1 WHERE `entry` <=> 1435 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36343335' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3135' USING utf8mb4));
-- quest_template {"entry": "1436"} Alliance Relations
UPDATE `quest_template` SET `PrevQuestId` = 1435 WHERE `entry` <=> 1436 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1440"} Return to Vahlarriel
UPDATE `quest_template` SET `PrevQuestId` = 1439 WHERE `entry` <=> 1440 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1442"} Seeking the Kor Gem
UPDATE `quest_template` SET `PrevQuestId` = 1653 WHERE `entry` <=> 1442 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1454"} The Karnitol Shipwreck
UPDATE `quest_template` SET `PrevQuestId` = 1453 WHERE `entry` <=> 1454 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1458"} Reagents for Reclaimers Inc.
UPDATE `quest_template` SET `PrevQuestId` = 1453 WHERE `entry` <=> 1458 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1464"} Fire Sapta
UPDATE `quest_template` SET `PrevQuestId` = -1526 WHERE `entry` <=> 1464 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1469"} Rhapsody's Tale
UPDATE `quest_template` SET `PrevQuestId` = 1452 WHERE `entry` <=> 1469 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1473"} Creature of the Void
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1473 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31343738' USING utf8mb4));
-- quest_template {"entry": "1479"} The Bough of the Eternals
UPDATE `quest_template` SET `PrevQuestId` = 1468 WHERE `entry` <=> 1479 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1484"} The Corrupter
UPDATE `quest_template` SET `PrevQuestId` = 1482 WHERE `entry` <=> 1484 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1488"} The Corrupter
UPDATE `quest_template` SET `PrevQuestId` = 1484 WHERE `entry` <=> 1488 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1489"} Hamuul Runetotem
UPDATE `quest_template` SET `PrevQuestId` = 880 WHERE `entry` <=> 1489 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1491"} Smart Drinks
UPDATE `quest_template` SET `PrevQuestId` = 865 WHERE `entry` <=> 1491 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1501"} Creature of the Void
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1501 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31353036' USING utf8mb4));
-- quest_template {"entry": "1524"} Call of Fire
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1524 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31353232' USING utf8mb4));
-- quest_template {"entry": "1530"} Call of Water
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1530 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31353238' USING utf8mb4));
-- quest_template {"entry": "1558"} The Stonewrought Dam
UPDATE `quest_template` SET `PrevQuestId` = 1468 WHERE `entry` <=> 1558 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1559"} Flash Bomb Recipe
UPDATE `quest_template` SET `PrevQuestId` = 705 WHERE `entry` <=> 1559 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1641"} The Tome of Divinity
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1641 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32393938' USING utf8mb4));
-- quest_template {"entry": "1642"} The Tome of Divinity
UPDATE `quest_template` SET `PrevQuestId` = 1641 WHERE `entry` <=> 1642 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1645"} The Tome of Divinity
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1645 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32393937' USING utf8mb4));
-- quest_template {"entry": "1646"} The Tome of Divinity
UPDATE `quest_template` SET `PrevQuestId` = 1645 WHERE `entry` <=> 1646 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1651"} The Tome of Valor
UPDATE `quest_template` SET `PrevQuestId` = 1650 WHERE `entry` <=> 1651 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1655"} Bailor's Ore Shipment
UPDATE `quest_template` SET `PrevQuestId` = 1653 WHERE `entry` <=> 1655 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1666"} Marshal Haggard
UPDATE `quest_template` SET `PrevQuestId` = 1665 WHERE `entry` <=> 1666 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1667"} Dead-tooth Jack
UPDATE `quest_template` SET `ReqSourceId1` = 6783, `ReqSourceCount1` = 1 WHERE `entry` <=> 1667 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1686"} The Shade of Elura
UPDATE `quest_template` SET `PrevQuestId` = 1683 WHERE `entry` <=> 1686 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1687"} Spooky Lighthouse
UPDATE `quest_template` SET `PrevQuestId` = 1468 WHERE `entry` <=> 1687 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1688"} Surena Caledon
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1688 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31363835' USING utf8mb4));
-- quest_template {"entry": "1693"} Weapons of Elunite
UPDATE `quest_template` SET `PrevQuestId` = 1692 WHERE `entry` <=> 1693 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1700"} Grimand Elmore
UPDATE `quest_template` SET `PrevQuestId` = 1701 WHERE `entry` <=> 1700 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1702"} The Shieldsmith
UPDATE `quest_template` SET `PrevQuestId` = 1699 WHERE `entry` <=> 1702 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1703"} Mathiel
UPDATE `quest_template` SET `PrevQuestId` = 1701 WHERE `entry` <=> 1703 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1704"} Klockmort Spannerspan
UPDATE `quest_template` SET `PrevQuestId` = 1701 WHERE `entry` <=> 1704 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1706"} Grimand's Armor
UPDATE `quest_template` SET `PrevQuestId` = 1705 WHERE `entry` <=> 1706 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1709"} Klockmort's Creation
UPDATE `quest_template` SET `PrevQuestId` = 1708 WHERE `entry` <=> 1709 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1711"} Mathiel's Armor
UPDATE `quest_template` SET `PrevQuestId` = 1710 WHERE `entry` <=> 1711 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1714"} Essence of the Exile
UPDATE `quest_template` SET `PrevQuestId` = -1712 WHERE `entry` <=> 1714 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1716"} Devourer of Souls
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1716 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31373137' USING utf8mb4));
-- quest_template {"entry": "1782"} Furen's Armor
UPDATE `quest_template` SET `PrevQuestId` = 1701 WHERE `entry` <=> 1782 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1789"} The Symbol of Life
UPDATE `quest_template` SET `PrevQuestId` = -1783 WHERE `entry` <=> 1789 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1790"} The Symbol of Life
UPDATE `quest_template` SET `PrevQuestId` = -1786 WHERE `entry` <=> 1790 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1791"} The Windwatcher
UPDATE `quest_template` SET `PrevQuestId` = 1719 WHERE `entry` <=> 1791 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1792"} Whirlwind Weapon
UPDATE `quest_template` SET `PrevQuestId` = 1713 WHERE `entry` <=> 1792 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1796"} Components for the Enchanted Gold Bloodrobe
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1796 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34373336' USING utf8mb4));
-- quest_template {"entry": "1799"} Fragments of the Orb of Orahil
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1799 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34393635' USING utf8mb4));
-- quest_template {"entry": "1800"} Lordaeron Throne Room
UPDATE `quest_template` SET `PrevQuestId` = 172 WHERE `entry` <=> 1800 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1822"} Heirloom Weapon
UPDATE `quest_template` SET `PrevQuestId` = 1821 WHERE `entry` <=> 1822 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1839"} Ula'elek and the Brutal Gauntlets
UPDATE `quest_template` SET `PrevQuestId` = 1838 WHERE `entry` <=> 1839 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1840"} Orm Stonehoof and the Brutal Helm
UPDATE `quest_template` SET `PrevQuestId` = 1838 WHERE `entry` <=> 1840 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1841"} Velora Nitely and the Brutal Legguards
UPDATE `quest_template` SET `PrevQuestId` = 1838 WHERE `entry` <=> 1841 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1843"} Brutal Gauntlets
UPDATE `quest_template` SET `PrevQuestId` = 1842 WHERE `entry` <=> 1843 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1845"} Brutal Helm
UPDATE `quest_template` SET `PrevQuestId` = 1844 WHERE `entry` <=> 1845 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1846"} Dragonmaw Shinbones
UPDATE `quest_template` SET `ReqSourceId1` = 7131, `ReqSourceCount1` = 999 WHERE `entry` <=> 1846 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37313334' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38' USING utf8mb4));
-- quest_template {"entry": "1847"} Brutal Legguards
UPDATE `quest_template` SET `PrevQuestId` = 1846 WHERE `entry` <=> 1847 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1848"} Brutal Hauberk
UPDATE `quest_template` SET `PrevQuestId` = 1838 WHERE `entry` <=> 1848 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1858"} The Shattered Hand
UPDATE `quest_template` SET `ReqSourceId1` = 7208 WHERE `entry` <=> 1858 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37323935' USING utf8mb4));
-- quest_template {"entry": "1878"} Water Pouch Bounty
UPDATE `quest_template` SET `PrevQuestId` = 1707 WHERE `entry` <=> 1878 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1884"} Ju-Ju Heaps
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -102986 WHERE `entry` <=> 1884 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "1941"} Manaweave Robe
UPDATE `quest_template` SET `PrevQuestId` = 1921 WHERE `entry` <=> 1941 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1942"} Astral Knot Garment
UPDATE `quest_template` SET `PrevQuestId` = 1940 WHERE `entry` <=> 1942 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1946"} Nether-lace Garment
UPDATE `quest_template` SET `PrevQuestId` = 1945 WHERE `entry` <=> 1946 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1948"} Items of Power
UPDATE `quest_template` SET `PrevQuestId` = 1951, `ReqSourceId1` = 7273, `ReqSourceCount1` = 10 WHERE `entry` <=> 1948 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37323732' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- quest_template {"entry": "1949"} Hidden Secrets
UPDATE `quest_template` SET `PrevQuestId` = 1947 WHERE `entry` <=> 1949 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1952"} Mage's Wand
UPDATE `quest_template` SET `PrevQuestId` = 1948 WHERE `entry` <=> 1952 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1958"} Celestial Power
UPDATE `quest_template` SET `PrevQuestId` = 1957 WHERE `entry` <=> 1958 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1962"} Spellfire Robes
UPDATE `quest_template` SET `PrevQuestId` = 1961 WHERE `entry` <=> 1962 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2038"} Bingles' Missing Supplies
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2038 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32303339' USING utf8mb4));
-- quest_template {"entry": "2040"} Underground Assault
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2040 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32303431' USING utf8mb4));
-- quest_template {"entry": "2139"} Tharnariun's Hope
UPDATE `quest_template` SET `PrevQuestId` = 2138 WHERE `entry` <=> 2139 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2203"} Badlands Reagent Run II
UPDATE `quest_template` SET `PrevQuestId` = 2202 WHERE `entry` <=> 2203 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2242"} Destiny Calls
UPDATE `quest_template` SET `PrevQuestId` = 2241 WHERE `entry` <=> 2242 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2259"} Erion Shadewhisper
UPDATE `quest_template` SET `PrevQuestId` = 2241 WHERE `entry` <=> 2259 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2279"} The Platinum Discs
UPDATE `quest_template` SET `PrevQuestId` = 2278 WHERE `entry` <=> 2279 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2280"} The Platinum Discs
UPDATE `quest_template` SET `PrevQuestId` = 2278 WHERE `entry` <=> 2280 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2281"} Redridge Rendezvous
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2281 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32333030' USING utf8mb4));
-- quest_template {"entry": "2299"} To Hulfdan!
UPDATE `quest_template` SET `PrevQuestId` = 2218 WHERE `entry` <=> 2299 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2300"} SI:7
UPDATE `quest_template` SET `PrevQuestId` = 2205 WHERE `entry` <=> 2300 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2339"} Find the Gems and Power Source
UPDATE `quest_template` SET `PrevQuestId` = 2338 WHERE `entry` <=> 2339 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2340"} Deliver the Gems
UPDATE `quest_template` SET `PrevQuestId` = 2339 WHERE `entry` <=> 2340 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2358"} Horns of Nez'ra
UPDATE `quest_template` SET `PrevQuestId` = 2282 WHERE `entry` <=> 2358 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2379"} Zando'zan
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2379 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32333738' USING utf8mb4));
-- quest_template {"entry": "2383"} Simple Parchment
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 2383 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2399"} The Sprouted Fronds
UPDATE `quest_template` SET `PrevQuestId` = 931 WHERE `entry` <=> 2399 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2440"} The Platinum Discs
UPDATE `quest_template` SET `PrevQuestId` = 2280 WHERE `entry` <=> 2440 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2459"} Ferocitas the Dream Eater
UPDATE `quest_template` SET `ReqSourceId1` = 8049 WHERE `entry` <=> 2459 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38303530' USING utf8mb4));
-- quest_template {"entry": "2501"} Badlands Reagent Run II
UPDATE `quest_template` SET `PrevQuestId` = 2500 WHERE `entry` <=> 2501 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2522"} Kum'isha's Endeavors
UPDATE `quest_template` SET `PrevQuestId` = 2521 WHERE `entry` <=> 2522 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2582"} Rage of Ages
UPDATE `quest_template` SET `PrevQuestId` = 2581 WHERE `entry` <=> 2582 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2584"} Spirit of the Boar
UPDATE `quest_template` SET `PrevQuestId` = 2583 WHERE `entry` <=> 2584 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2586"} Salt of the Scorpok
UPDATE `quest_template` SET `PrevQuestId` = 2585 WHERE `entry` <=> 2586 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2602"} Infallible Mind
UPDATE `quest_template` SET `PrevQuestId` = 2601 WHERE `entry` <=> 2602 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2604"} Spiritual Domination
UPDATE `quest_template` SET `PrevQuestId` = 2603 WHERE `entry` <=> 2604 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2605"} The Thirsty Goblin
UPDATE `quest_template` SET `ReqSourceId2` = 8429, `ReqSourceId3` = 8430, `ReqSourceCount2` = 30, `ReqSourceCount3` = 30 WHERE `entry` <=> 2605 AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2608"} The Touch of Zanzil
UPDATE `quest_template` SET `PrevQuestId` = 2607 WHERE `entry` <=> 2608 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2609"} The Touch of Zanzil
UPDATE `quest_template` SET `PrevQuestId` = 2608 WHERE `entry` <=> 2609 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2661"} Delivery for Marin
UPDATE `quest_template` SET `PrevQuestId` = 2641 WHERE `entry` <=> 2661 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2721"} Kirith
UPDATE `quest_template` SET `PrevQuestId` = 2701 WHERE `entry` <=> 2721 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2747"} An Extraordinary Egg
UPDATE `quest_template` SET `PrevQuestId` = 2741 WHERE `entry` <=> 2747 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2748"} A Fine Egg
UPDATE `quest_template` SET `PrevQuestId` = 2741 WHERE `entry` <=> 2748 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2749"} An Ordinary Egg
UPDATE `quest_template` SET `PrevQuestId` = 2741 WHERE `entry` <=> 2749 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2750"} A Bad Egg
UPDATE `quest_template` SET `PrevQuestId` = 2741 WHERE `entry` <=> 2750 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2752"} On Iron Pauldrons
UPDATE `quest_template` SET `PrevQuestId` = 2751 WHERE `entry` <=> 2752 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2753"} Trampled Under Foot
UPDATE `quest_template` SET `PrevQuestId` = 2752 WHERE `entry` <=> 2753 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2754"} Horns of Frenzy
UPDATE `quest_template` SET `PrevQuestId` = 2753 WHERE `entry` <=> 2754 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2755"} Joys of Omosh
UPDATE `quest_template` SET `PrevQuestId` = 2754 WHERE `entry` <=> 2755 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2757"} Booty Bay or Bust!
UPDATE `quest_template` SET `PrevQuestId` = 2756 WHERE `entry` <=> 2757 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2760"} The Mithril Order
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2760 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32373537' USING utf8mb4));
-- quest_template {"entry": "2761"} Smelt On, Smelt Off
UPDATE `quest_template` SET `PrevQuestId` = 2760 WHERE `entry` <=> 2761 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2762"} The Great Silver Deceiver
UPDATE `quest_template` SET `PrevQuestId` = 2760 WHERE `entry` <=> 2762 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2763"} The Art of the Imbue
UPDATE `quest_template` SET `PrevQuestId` = 2760 WHERE `entry` <=> 2763 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2764"} Galvan's Finest Pupil
UPDATE `quest_template` SET `PrevQuestId` = 2761 WHERE `entry` <=> 2764 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2771"} A Good Head On Your Shoulders
UPDATE `quest_template` SET `PrevQuestId` = 2760 WHERE `entry` <=> 2771 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2772"} The World At Your Feet
UPDATE `quest_template` SET `PrevQuestId` = 2760 WHERE `entry` <=> 2772 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2773"} The Mithril Kid
UPDATE `quest_template` SET `PrevQuestId` = 2760 WHERE `entry` <=> 2773 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2801"} A Tale of Sorrow
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2801 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32373833' USING utf8mb4));
-- quest_template {"entry": "2842"} Chief Engineer Scooty
UPDATE `quest_template` SET `PrevQuestId` = -2841 WHERE `entry` <=> 2842 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2846"} Tiara of the Deep
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2846 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32383631' USING utf8mb4));
-- quest_template {"entry": "2848"} Wild Leather Shoulders
UPDATE `quest_template` SET `PrevQuestId` = 2847 WHERE `entry` <=> 2848 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2849"} Wild Leather Vest
UPDATE `quest_template` SET `PrevQuestId` = 2847 WHERE `entry` <=> 2849 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2850"} Wild Leather Helmet
UPDATE `quest_template` SET `PrevQuestId` = 2847 WHERE `entry` <=> 2850 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2851"} Wild Leather Boots
UPDATE `quest_template` SET `PrevQuestId` = 2847 WHERE `entry` <=> 2851 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2852"} Wild Leather Leggings
UPDATE `quest_template` SET `PrevQuestId` = 2847 WHERE `entry` <=> 2852 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2855"} Wild Leather Shoulders
UPDATE `quest_template` SET `PrevQuestId` = 2854 WHERE `entry` <=> 2855 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2856"} Wild Leather Vest
UPDATE `quest_template` SET `PrevQuestId` = 2854 WHERE `entry` <=> 2856 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2857"} Wild Leather Helmet
UPDATE `quest_template` SET `PrevQuestId` = 2854 WHERE `entry` <=> 2857 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2858"} Wild Leather Boots
UPDATE `quest_template` SET `PrevQuestId` = 2854 WHERE `entry` <=> 2858 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2859"} Wild Leather Leggings
UPDATE `quest_template` SET `PrevQuestId` = 2854 WHERE `entry` <=> 2859 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2879"} The Stave of Equinex
UPDATE `quest_template` SET `PrevQuestId` = 2943 WHERE `entry` <=> 2879 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2881"} Troll Necklace Bounty
UPDATE `quest_template` SET `PrevQuestId` = 2880 WHERE `entry` <=> 2881 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2924"} Essential Artificials
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2924 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32393235' USING utf8mb4));
-- quest_template {"entry": "2930"} Data Rescue
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2930 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32393331' USING utf8mb4));
-- quest_template {"entry": "2932"} Grim Message
UPDATE `quest_template` SET `ReqSourceId1` = 9320, `ReqSourceCount1` = 20 WHERE `entry` <=> 2932 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2938"} Venom to the Undercity
UPDATE `quest_template` SET `PrevQuestId` = 2937 WHERE `entry` <=> 2938 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2940"} Feralas: A History
UPDATE `quest_template` SET `PrevQuestId` = 2939 WHERE `entry` <=> 2940 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2947"} Return of the Ring
UPDATE `quest_template` SET `PrevQuestId` = 2945 WHERE `entry` <=> 2947 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2949"} Return of the Ring
UPDATE `quest_template` SET `PrevQuestId` = 2945 WHERE `entry` <=> 2949 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2952"} The Sparklematic 5200!
UPDATE `quest_template` SET `PrevQuestId` = 2951 WHERE `entry` <=> 2952 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2953"} More Sparklematic Action
UPDATE `quest_template` SET `PrevQuestId` = 2952 WHERE `entry` <=> 2953 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2963"} Portents of Uldum
UPDATE `quest_template` SET `PrevQuestId` = 2439 WHERE `entry` <=> 2963 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2965"} Portents of Uldum
UPDATE `quest_template` SET `PrevQuestId` = 2440 WHERE `entry` <=> 2965 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2967"} Return to Thunder Bluff
UPDATE `quest_template` SET `PrevQuestId` = 2954 WHERE `entry` <=> 2967 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2970"} Doling Justice
UPDATE `quest_template` SET `PrevQuestId` = 2969 WHERE `entry` <=> 2970 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2975"} The Ogres of Feralas
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2975 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32393831' USING utf8mb4));
-- quest_template {"entry": "2977"} Return to Ironforge
UPDATE `quest_template` SET `PrevQuestId` = 2954 WHERE `entry` <=> 2977 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2988"} Witherbark Cages
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -144066, `ReqCreatureOrGOId2` = -144067, `ReqCreatureOrGOId3` = -144068 WHERE `entry` <=> 2988 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "2994"} Saving Sharpbeak
UPDATE `quest_template` SET `ReqSourceId1` = 9472, `ReqSourceCount1` = 1, `ReqCreatureOrGOId2` = -144070 WHERE `entry` <=> 2994 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "2995"} Lines of Communication
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -144071, `ReqCreatureOrGOId2` = -144072, `ReqCreatureOrGOId3` = -144073 WHERE `entry` <=> 2995 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "3062"} Dark Heart
UPDATE `quest_template` SET `ReqSourceId1` = 9530 WHERE `entry` <=> 3062 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'39353238' USING utf8mb4));
-- quest_template {"entry": "3065"} Simple Tablet
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3065 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3082"} Etched Tablet
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3082 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8433"} Battle of Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8428 WHERE `entry` <=> 8433 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8432"} Battle of Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8427 WHERE `entry` <=> 8432 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3085"} Hallowed Tablet
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3085 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3086"} Glyphic Tablet
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3086 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3087"} Etched Parchment
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3087 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8431"} Battle of Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8426 WHERE `entry` <=> 8431 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3090"} Tainted Parchment
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3090 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3091"} Simple Note
UPDATE `quest_template` SET `PrevQuestId` = 747 WHERE `entry` <=> 3091 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3092"} Etched Note
UPDATE `quest_template` SET `PrevQuestId` = 747 WHERE `entry` <=> 3092 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3093"} Rune-Inscribed Note
UPDATE `quest_template` SET `PrevQuestId` = 747 WHERE `entry` <=> 3093 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3094"} Verdant Note
UPDATE `quest_template` SET `PrevQuestId` = 747 WHERE `entry` <=> 3094 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3095"} Simple Scroll
UPDATE `quest_template` SET `PrevQuestId` = 364 WHERE `entry` <=> 3095 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3096"} Encrypted Scroll
UPDATE `quest_template` SET `PrevQuestId` = 364 WHERE `entry` <=> 3096 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3097"} Hallowed Scroll
UPDATE `quest_template` SET `PrevQuestId` = 364 WHERE `entry` <=> 3097 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3098"} Glyphic Scroll
UPDATE `quest_template` SET `PrevQuestId` = 364 WHERE `entry` <=> 3098 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3099"} Tainted Scroll
UPDATE `quest_template` SET `PrevQuestId` = 364 WHERE `entry` <=> 3099 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3100"} Simple Letter
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 3100 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3101"} Consecrated Letter
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 3101 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3102"} Encrypted Letter
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 3102 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3103"} Hallowed Letter
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 3103 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3104"} Glyphic Letter
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 3104 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3105"} Tainted Letter
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 3105 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3106"} Simple Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3106 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3107"} Consecrated Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3107 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3108"} Etched Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3108 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3109"} Encrypted Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3109 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3110"} Hallowed Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3110 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3112"} Simple Memorandum
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3112 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3113"} Encrypted Memorandum
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3113 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3114"} Glyphic Memorandum
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3114 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3115"} Tainted Memorandum
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3115 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3116"} Simple Sigil
UPDATE `quest_template` SET `PrevQuestId` = 456 WHERE `entry` <=> 3116 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3117"} Etched Sigil
UPDATE `quest_template` SET `PrevQuestId` = 456 WHERE `entry` <=> 3117 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3118"} Encrypted Sigil
UPDATE `quest_template` SET `PrevQuestId` = 456 WHERE `entry` <=> 3118 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3119"} Hallowed Sigil
UPDATE `quest_template` SET `PrevQuestId` = 456 WHERE `entry` <=> 3119 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3120"} Verdant Sigil
UPDATE `quest_template` SET `PrevQuestId` = 456 WHERE `entry` <=> 3120 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3123"} Testing the Vessel
UPDATE `quest_template` SET `PrevQuestId` = 3122 WHERE `entry` <=> 3123 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3128"} Natural Materials
UPDATE `quest_template` SET `PrevQuestId` = 3122 WHERE `entry` <=> 3128 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3130"} Against the Hatecrest
UPDATE `quest_template` SET `PrevQuestId` = 2867 WHERE `entry` <=> 3130 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3201"} At Last!
UPDATE `quest_template` SET `PrevQuestId` = 3182 WHERE `entry` <=> 3201 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3301"} Mura Runetotem
UPDATE `quest_template` SET `PrevQuestId` = 880 WHERE `entry` <=> 3301 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3321"} Did You Lose This?
UPDATE `quest_template` SET `PrevQuestId` = 2771 WHERE `entry` <=> 3321 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3365"} Bring Back the Mug
UPDATE `quest_template` SET `PrevQuestId` = 3364 WHERE `entry` <=> 3365 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3369"} In Nightmares
UPDATE `quest_template` SET `PrevQuestId` = 6981 WHERE `entry` <=> 3369 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3370"} In Nightmares
UPDATE `quest_template` SET `PrevQuestId` = 6981 WHERE `entry` <=> 3370 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3374"} The Essence of Eranikus
UPDATE `quest_template` SET `PrevQuestId` = 3373 WHERE `entry` <=> 3374 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3375"} Replacement Phial
UPDATE `quest_template` SET `PrevQuestId` = -2201 WHERE `entry` <=> 3375 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3442"} The Flawless Flame
UPDATE `quest_template` SET `PrevQuestId` = 3441 WHERE `entry` <=> 3442 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3444"} The Stone Circle
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 3444 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'33333830' USING utf8mb4));
-- quest_template {"entry": "3446"} Into the Depths
UPDATE `quest_template` SET `PrevQuestId` = 3444 WHERE `entry` <=> 3446 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3447"} Secret of the Circle
UPDATE `quest_template` SET `PrevQuestId` = 3446 WHERE `entry` <=> 3447 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3450"} An Easy Pickup
UPDATE `quest_template` SET `PrevQuestId` = 3448 WHERE `entry` <=> 3450 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3462"} Squire Maltrake
UPDATE `quest_template` SET `PrevQuestId` = 3454 WHERE `entry` <=> 3462 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3463"} Set Them Ablaze!
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -149025, `ReqCreatureOrGOId2` = -149030, `ReqCreatureOrGOId3` = -149031, `ReqCreatureOrGOId4` = -149032 WHERE `entry` <=> 3463 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "3481"} Trinkets...
UPDATE `quest_template` SET `PrevQuestId` = 3463 WHERE `entry` <=> 3481 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3483"} Signal for Pickup
UPDATE `quest_template` SET `PrevQuestId` = 3451 WHERE `entry` <=> 3483 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3512"} In Eranikus' Own Words
UPDATE `quest_template` SET `PrevQuestId` = 3374 WHERE `entry` <=> 3512 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3518"} Delivery to Magatha
UPDATE `quest_template` SET `PrevQuestId` = 3517 WHERE `entry` <=> 3518 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3528"} The God Hakkar
UPDATE `quest_template` SET `ReqSourceId1` = 10663 WHERE `entry` <=> 3528 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3130363632' USING utf8mb4));
-- quest_template {"entry": "3541"} Delivery to Jes'rimon
UPDATE `quest_template` SET `PrevQuestId` = 3517 WHERE `entry` <=> 3541 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3542"} Delivery to Andron Gant
UPDATE `quest_template` SET `PrevQuestId` = 3517 WHERE `entry` <=> 3542 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3561"} Delivery to Archmage Xylem
UPDATE `quest_template` SET `PrevQuestId` = 3517 WHERE `entry` <=> 3561 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3566"} Rise, Obsidion!
UPDATE `quest_template` SET `PrevQuestId` = 3372 WHERE `entry` <=> 3566 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3569"} Seeping Corruption
UPDATE `quest_template` SET `PrevQuestId` = 3568 WHERE `entry` <=> 3569 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3570"} Seeping Corruption
UPDATE `quest_template` SET `PrevQuestId` = 3569 WHERE `entry` <=> 3570 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3602"} Azsharite
UPDATE `quest_template` SET `ReqSourceId2` = 10839, `ReqSourceId3` = 10840, `ReqSourceCount2` = 1, `ReqSourceCount3` = 1 WHERE `entry` <=> 3602 AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3621"} The Formation of Felbane
UPDATE `quest_template` SET `PrevQuestId` = 3602 WHERE `entry` <=> 3621 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3644"} Membership Card Renewal
UPDATE `quest_template` SET `PrevQuestId` = 3639 WHERE `entry` <=> 3644 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3645"} Membership Card Renewal
UPDATE `quest_template` SET `PrevQuestId` = 3641 WHERE `entry` <=> 3645 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3646"} Membership Card Renewal
UPDATE `quest_template` SET `PrevQuestId` = 3639 WHERE `entry` <=> 3646 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3647"} Membership Card Renewal
UPDATE `quest_template` SET `PrevQuestId` = 3643 WHERE `entry` <=> 3647 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3701"} The Smoldering Ruins of Thaurissan
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -153556 WHERE `entry` <=> 3701 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "3761"} Un'Goro Soil
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 3761 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'393336' USING utf8mb4));
-- quest_template {"entry": "3764"} Un'Goro Soil
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 3764 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'33373633' USING utf8mb4));
-- quest_template {"entry": "3781"} Morrowgrain Research
UPDATE `quest_template` SET `PrevQuestId` = 3764 WHERE `entry` <=> 3781 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3782"} Morrowgrain Research
UPDATE `quest_template` SET `PrevQuestId` = 3761 WHERE `entry` <=> 3782 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3787"} Jonespyre's Request
UPDATE `quest_template` SET `PrevQuestId` = 3785 WHERE `entry` <=> 3787 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3788"} Jonespyre's Request
UPDATE `quest_template` SET `PrevQuestId` = 3785 WHERE `entry` <=> 3788 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3791"} The Mystery of Morrowgrain
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 3791 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'33373837' USING utf8mb4));
-- quest_template {"entry": "3803"} Morrowgrain to Darnassus
UPDATE `quest_template` SET `PrevQuestId` = 3785 WHERE `entry` <=> 3803 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3804"} Morrowgrain to Thunder Bluff
UPDATE `quest_template` SET `PrevQuestId` = 3786 WHERE `entry` <=> 3804 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3825"} Ogre Head On A Stick = Party
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -160840 WHERE `entry` <=> 3825 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "3901"} Rattling the Rattlecages
UPDATE `quest_template` SET `PrevQuestId` = 364 WHERE `entry` <=> 3901 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3902"} Scavenging Deathknell
UPDATE `quest_template` SET `PrevQuestId` = 376 WHERE `entry` <=> 3902 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3909"} The Videre Elixir
UPDATE `quest_template` SET `PrevQuestId` = 3908, `ReqSourceId1` = 11242 WHERE `entry` <=> 3909 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3131323433' USING utf8mb4));
-- quest_template {"entry": "3921"} Wenikee Boltbucket
UPDATE `quest_template` SET `PrevQuestId` = 902 WHERE `entry` <=> 3921 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3923"} Rilli Greasygob
UPDATE `quest_template` SET `PrevQuestId` = 3922 WHERE `entry` <=> 3923 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3924"} Samophlange Manual
UPDATE `quest_template` SET `ReqSourceId1` = 11147, `ReqSourceId2` = 11148, `ReqSourceCount2` = 5 WHERE `entry` <=> 3924 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3131313439' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3981"} Commander Gor'shak
UPDATE `quest_template` SET `PrevQuestId` = 3906 WHERE `entry` <=> 3981 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4005"} Aquementas
UPDATE `quest_template` SET `ReqSourceId2` = 11172, `ReqSourceId3` = 11173, `ReqSourceId4` = 11169, `ReqSourceCount2` = 11, `ReqSourceCount3` = 1, `ReqSourceCount4` = 1 WHERE `entry` <=> 4005 AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4021"} Counterattack!
UPDATE `quest_template` SET `PrevQuestId` = 852 WHERE `entry` <=> 4021 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4022"} A Taste of Flame
UPDATE `quest_template` SET `PrevQuestId` = 3481 WHERE `entry` <=> 4022 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4024"} A Taste of Flame
UPDATE `quest_template` SET `PrevQuestId` = 4023 WHERE `entry` <=> 4024 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34303232' USING utf8mb4));
-- quest_template {"entry": "4041"} The Videre Elixir
UPDATE `quest_template` SET `PrevQuestId` = -3909 WHERE `entry` <=> 4041 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4082"} KILL ON SIGHT: High Ranking Dark Iron Officials
UPDATE `quest_template` SET `PrevQuestId` = 4081 WHERE `entry` <=> 4082 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4103"} Salve via Hunting
UPDATE `quest_template` SET `PrevQuestId` = 5882 WHERE `entry` <=> 4103 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4104"} Salve via Mining
UPDATE `quest_template` SET `PrevQuestId` = 5883 WHERE `entry` <=> 4104 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4105"} Salve via Gathering
UPDATE `quest_template` SET `PrevQuestId` = 5884 WHERE `entry` <=> 4105 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4106"} Salve via Skinning
UPDATE `quest_template` SET `PrevQuestId` = 5885 WHERE `entry` <=> 4106 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4107"} Salve via Disenchanting
UPDATE `quest_template` SET `PrevQuestId` = 5886 WHERE `entry` <=> 4107 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4108"} Salve via Hunting
UPDATE `quest_template` SET `PrevQuestId` = 5887 WHERE `entry` <=> 4108 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4109"} Salve via Mining
UPDATE `quest_template` SET `PrevQuestId` = 5888 WHERE `entry` <=> 4109 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4110"} Salve via Gathering
UPDATE `quest_template` SET `PrevQuestId` = 5889 WHERE `entry` <=> 4110 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4111"} Salve via Skinning
UPDATE `quest_template` SET `PrevQuestId` = 5890 WHERE `entry` <=> 4111 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4112"} Salve via Disenchanting
UPDATE `quest_template` SET `PrevQuestId` = 5891 WHERE `entry` <=> 4112 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4121"} Precarious Predicament
UPDATE `quest_template` SET `PrevQuestId` = 4122 WHERE `entry` <=> 4121 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4132"} Operation: Death to Angerforge
UPDATE `quest_template` SET `PrevQuestId` = 4121 WHERE `entry` <=> 4132 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4144"} Bloodpetal Sprouts
UPDATE `quest_template` SET `PrevQuestId` = 4143 WHERE `entry` <=> 4144 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4148"} Bloodpetal Zapper
UPDATE `quest_template` SET `PrevQuestId` = 4146 WHERE `entry` <=> 4148 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4245"} Chasing A-Me 01
UPDATE `quest_template` SET `PrevQuestId` = 4244 WHERE `entry` <=> 4245 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4261"} Ancient Spirit
UPDATE `quest_template` SET `PrevQuestId` = 4442 WHERE `entry` <=> 4261 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4264"} A Crumpled Up Note
UPDATE `quest_template` SET `PrevQuestId` = 4242 WHERE `entry` <=> 4264 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4285"} The Northern Pylon
UPDATE `quest_template` SET `PrevQuestId` = 4284 WHERE `entry` <=> 4285 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4287"} The Eastern Pylon
UPDATE `quest_template` SET `PrevQuestId` = 4284 WHERE `entry` <=> 4287 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4288"} The Western Pylon
UPDATE `quest_template` SET `PrevQuestId` = 4284 WHERE `entry` <=> 4288 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4293"} A Sample of Slime...
UPDATE `quest_template` SET `ReqSourceId1` = 12230, `ReqSourceCount1` = 100 WHERE `entry` <=> 4293 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3132323334' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35' USING utf8mb4));
-- quest_template {"entry": "4294"} ... and a Batch of Ooze
UPDATE `quest_template` SET `ReqSourceId1` = 12235, `ReqSourceCount1` = 100 WHERE `entry` <=> 4294 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3132323336' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35' USING utf8mb4));
-- quest_template {"entry": "4301"} The Mighty U'cha
UPDATE `quest_template` SET `PrevQuestId` = 4289 WHERE `entry` <=> 4301 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4322"} Jail Break!
UPDATE `quest_template` SET `PrevQuestId` = 4282 WHERE `entry` <=> 4322 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4402"} Galgar's Cactus Apple Surprise
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 4402 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4442"} Purified!
UPDATE `quest_template` SET `PrevQuestId` = 4441 WHERE `entry` <=> 4442 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4490"} Summon Felsteed
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 4490 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'33363331' USING utf8mb4));
-- quest_template {"entry": "4491"} A Little Help From My Friends
UPDATE `quest_template` SET `PrevQuestId` = 4492 WHERE `entry` <=> 4491 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4496"} Bungle in the Jungle
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 4496 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'34343933' USING utf8mb4));
-- quest_template {"entry": "4505"} Well of Corruption
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 4505 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36363035' USING utf8mb4));
-- quest_template {"entry": "4506"} Corrupted Sabers
UPDATE `quest_template` SET `PrevQuestId` = 4505 WHERE `entry` <=> 4506 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4507"} Pawn Captures Queen
UPDATE `quest_template` SET `PrevQuestId` = 4496 WHERE `entry` <=> 4507 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4508"} Calm Before the Storm
UPDATE `quest_template` SET `PrevQuestId` = 4507 WHERE `entry` <=> 4508 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4509"} Calm Before the Storm
UPDATE `quest_template` SET `PrevQuestId` = 4507 WHERE `entry` <=> 4509 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4513"} A Little Slime Goes a Long Way
UPDATE `quest_template` SET `PrevQuestId` = 4512 WHERE `entry` <=> 4513 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4603"} More Sparklematic Action
UPDATE `quest_template` SET `PrevQuestId` = 4605 WHERE `entry` <=> 4603 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4604"} More Sparklematic Action
UPDATE `quest_template` SET `PrevQuestId` = 4606 WHERE `entry` <=> 4604 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4605"} The Sparklematic 5200!
UPDATE `quest_template` SET `PrevQuestId` = 4601 WHERE `entry` <=> 4605 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4606"} The Sparklematic 5200!
UPDATE `quest_template` SET `PrevQuestId` = 4602 WHERE `entry` <=> 4606 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4621"} Avast Ye, Admiral!
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 4621 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31303336' USING utf8mb4));
-- quest_template {"entry": "4722"} Beached Sea Turtle
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4722 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4723"} Beached Sea Creature
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4723 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4725"} Beached Sea Turtle
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4725 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4727"} Beached Sea Turtle
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4727 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4728"} Beached Sea Creature
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4728 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4730"} Beached Sea Creature
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4730 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4731"} Beached Sea Turtle
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4731 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4732"} Beached Sea Turtle
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4732 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4733"} Beached Sea Creature
UPDATE `quest_template` SET `PrevQuestId` = 4681 WHERE `entry` <=> 4733 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4761"} Thundris Windweaver
UPDATE `quest_template` SET `PrevQuestId` = 984 WHERE `entry` <=> 4761 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4763"} The Blackwood Corrupted
UPDATE `quest_template` SET `PrevQuestId` = 4762, `ReqSourceId1` = 12341, `ReqSourceId2` = 12342, `ReqSourceId3` = 12343, `ReqSourceCount2` = 1, `ReqSourceCount3` = 1 WHERE `entry` <=> 4763 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3132333535' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4785"} Fine Gold Thread
UPDATE `quest_template` SET `PrevQuestId` = 4783 WHERE `entry` <=> 4785 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4861"} Enraged Wildkin
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 4861 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36363034' USING utf8mb4));
-- quest_template {"entry": "4865"} Serpent Wild
UPDATE `quest_template` SET `PrevQuestId` = 4821 WHERE `entry` <=> 4865 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4901"} Guardians of the Altar
UPDATE `quest_template` SET `PrevQuestId` = 979 WHERE `entry` <=> 4901 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4902"} Wildkin of Elune
UPDATE `quest_template` SET `PrevQuestId` = 4901 WHERE `entry` <=> 4902 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4906"} Further Corruption
UPDATE `quest_template` SET `PrevQuestId` = 4421 WHERE `entry` <=> 4906 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4907"} Tinkee Steamboil
UPDATE `quest_template` SET `PrevQuestId` = 4810 WHERE `entry` <=> 4907 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4962"} Shard of a Felhound
UPDATE `quest_template` SET `PrevQuestId` = -1799 WHERE `entry` <=> 4962 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4963"} Shard of an Infernal
UPDATE `quest_template` SET `PrevQuestId` = -1799 WHERE `entry` <=> 4963 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4964"} The Completed Orb of Dar'Orahil
UPDATE `quest_template` SET `PrevQuestId` = 4976 WHERE `entry` <=> 4964 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4972"} Counting Out Time
UPDATE `quest_template` SET `PrevQuestId` = 4971 WHERE `entry` <=> 4972 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4973"} Counting Out Time
UPDATE `quest_template` SET `PrevQuestId` = 4972 WHERE `entry` <=> 4973 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4975"} The Completed Orb of Noh'Orahil
UPDATE `quest_template` SET `PrevQuestId` = 4963 WHERE `entry` <=> 4975 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4986"} Glyphed Oaken Branch
UPDATE `quest_template` SET `PrevQuestId` = 4985 WHERE `entry` <=> 4986 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4987"} Glyphed Oaken Branch
UPDATE `quest_template` SET `PrevQuestId` = 4985 WHERE `entry` <=> 4987 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5022"} Better Late Than Never
UPDATE `quest_template` SET `PrevQuestId` = 5021 WHERE `entry` <=> 5022 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5023"} Better Late Than Never
UPDATE `quest_template` SET `PrevQuestId` = 5021 WHERE `entry` <=> 5023 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5042"} Agamaggan's Strength
UPDATE `quest_template` SET `PrevQuestId` = 5052 WHERE `entry` <=> 5042 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5043"} Agamaggan's Agility
UPDATE `quest_template` SET `PrevQuestId` = 5052 WHERE `entry` <=> 5043 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5044"} Wisdom of Agamaggan
UPDATE `quest_template` SET `PrevQuestId` = 5052 WHERE `entry` <=> 5044 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5045"} Rising Spirit
UPDATE `quest_template` SET `PrevQuestId` = 5052 WHERE `entry` <=> 5045 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5046"} Razorhide
UPDATE `quest_template` SET `PrevQuestId` = 5052 WHERE `entry` <=> 5046 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5050"} Good Luck Charm
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 5050 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35303438' USING utf8mb4));
-- quest_template {"entry": "5051"} Two Halves Become One
UPDATE `quest_template` SET `ReqSourceId1` = 12722 WHERE `entry` <=> 5051 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3132373233' USING utf8mb4));
-- quest_template {"entry": "5052"} Blood Shards of Agamaggan
UPDATE `quest_template` SET `PrevQuestId` = 878 WHERE `entry` <=> 5052 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5063"} Cap of the Scarlet Savant
UPDATE `quest_template` SET `PrevQuestId` = 5047 WHERE `entry` <=> 5063 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5065"} The Lost Tablets of Mosh'aru
UPDATE `quest_template` SET `PrevQuestId` = 3528 WHERE `entry` <=> 5065 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5067"} Leggings of Arcana
UPDATE `quest_template` SET `PrevQuestId` = 5047 WHERE `entry` <=> 5067 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5068"} Breastplate of Bloodthirst
UPDATE `quest_template` SET `PrevQuestId` = 5047 WHERE `entry` <=> 5068 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5088"} Arikara
UPDATE `quest_template` SET `PrevQuestId` = 5062 WHERE `entry` <=> 5088 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5092"} Clear the Way
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 5092 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35303636' USING utf8mb4));
-- quest_template {"entry": "5096"} Scarlet Diversions
UPDATE `quest_template` SET `PrevQuestId` = 0, `ReqSourceId1` = 12814, `ReqSourceCount1` = 1 WHERE `entry` <=> 5096 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35303933' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5097"} All Along the Watchtowers
UPDATE `quest_template` SET `PrevQuestId` = 5092 WHERE `entry` <=> 5097 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5098"} All Along the Watchtowers
UPDATE `quest_template` SET `PrevQuestId` = 5096 WHERE `entry` <=> 5098 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5123"} The Final Piece
UPDATE `quest_template` SET `PrevQuestId` = 5087 WHERE `entry` <=> 5123 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5125"} Aurius' Reckoning
UPDATE `quest_template` SET `PrevQuestId` = 5122 WHERE `entry` <=> 5125 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5127"} The Demon Forge
UPDATE `quest_template` SET `PrevQuestId` = 5126 WHERE `entry` <=> 5127 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5128"} Words of the High Chief
UPDATE `quest_template` SET `PrevQuestId` = 5123 WHERE `entry` <=> 5128 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5149"} Pamela's Doll
UPDATE `quest_template` SET `PrevQuestId` = 0, `ReqSourceId1` = 12886, `ReqSourceId2` = 12887, `ReqSourceId3` = 12888, `ReqSourceCount1` = 1, `ReqSourceCount2` = 1, `ReqSourceCount3` = 1 WHERE `entry` <=> 5149 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35313432' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5152"} Auntie Marlene
UPDATE `quest_template` SET `PrevQuestId` = 5149 WHERE `entry` <=> 5152 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5159"} Cleansed Water Returns to Felwood
UPDATE `quest_template` SET `PrevQuestId` = 5158 WHERE `entry` <=> 5159 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5165"} Dousing the Flames of Protection
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -176158, `ReqCreatureOrGOId2` = -176159, `ReqCreatureOrGOId3` = -176160, `ReqCreatureOrGOId4` = -176161 WHERE `entry` <=> 5165 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "5166"} Breastplates of the Chromatic Flight
UPDATE `quest_template` SET `PrevQuestId` = 5164 WHERE `entry` <=> 5166 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5167"} Legplates of the Chromatic Defier
UPDATE `quest_template` SET `PrevQuestId` = 5164 WHERE `entry` <=> 5167 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5168"} Heroes of Darrowshire
UPDATE `quest_template` SET `PrevQuestId` = 5210 WHERE `entry` <=> 5168 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5203"} Rescue From Jaedenar
UPDATE `quest_template` SET `PrevQuestId` = 5202 WHERE `entry` <=> 5203 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5206"} Marauders of Darrowshire
UPDATE `quest_template` SET `ReqSourceId1` = 13157, `ReqSourceCount1` = 99 WHERE `entry` <=> 5206 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5210"} Brother Carlin
UPDATE `quest_template` SET `PrevQuestId` = 5154 WHERE `entry` <=> 5210 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5215"} The Scourge Cauldrons
UPDATE `quest_template` SET `PrevQuestId` = 5092 WHERE `entry` <=> 5215 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5228"} The Scourge Cauldrons
UPDATE `quest_template` SET `PrevQuestId` = 5096 WHERE `entry` <=> 5228 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5238"} Mission Accomplished!
UPDATE `quest_template` SET `PrevQuestId` = 5236 WHERE `entry` <=> 5238 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5237"} Mission Accomplished!
UPDATE `quest_template` SET `PrevQuestId` = 5226 WHERE `entry` <=> 5237 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5241"} Uncle Carlin
UPDATE `quest_template` SET `PrevQuestId` = 5149 WHERE `entry` <=> 5241 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5242"} A Final Blow
UPDATE `quest_template` SET `PrevQuestId` = 5165 WHERE `entry` <=> 5242 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5244"} The Ruins of Kel'Theril
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 5244 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35323439' USING utf8mb4));
-- quest_template {"entry": "5248"} Tormented By the Past
UPDATE `quest_template` SET `PrevQuestId` = 5247 WHERE `entry` <=> 5248 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5251"} The Archivist
UPDATE `quest_template` SET `ReqCreatureOrGOId2` = -176245 WHERE `entry` <=> 5251 AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "5261"} Eagan Peltskinner
UPDATE `quest_template` SET `PrevQuestId` = 783 WHERE `entry` <=> 5261 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5262"} The Truth Comes Crashing Down
UPDATE `quest_template` SET `PrevQuestId` = 5251 WHERE `entry` <=> 5262 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5264"} Lord Maxwell Tyrosus
UPDATE `quest_template` SET `PrevQuestId` = 5263 WHERE `entry` <=> 5264 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5382"} Doctor Theolen Krastinov, the Butcher
UPDATE `quest_template` SET `ReqCreatureOrGOId2` = -176544, `ReqCreatureOrGOId3` = -176545 WHERE `entry` <=> 5382 AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "5461"} The Human, Ras Frostwhisper
UPDATE `quest_template` SET `PrevQuestId` = 5384 WHERE `entry` <=> 5461 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5505"} The Key to Scholomance
UPDATE `quest_template` SET `PrevQuestId` = 5803 WHERE `entry` <=> 5505 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5511"} The Key to Scholomance
UPDATE `quest_template` SET `PrevQuestId` = 5804 WHERE `entry` <=> 5511 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5519"} The Gordok Ogre Suit
UPDATE `quest_template` SET `PrevQuestId` = 5518 WHERE `entry` <=> 5519 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5522"} Leonid Barthalomew
UPDATE `quest_template` SET `PrevQuestId` = 4735 WHERE `entry` <=> 5522 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5526"} Shards of the Felvine
UPDATE `quest_template` SET `ReqSourceId1` = 18501 WHERE `entry` <=> 5526 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3138353430' USING utf8mb4));
-- quest_template {"entry": "5533"} Scholomance
UPDATE `quest_template` SET `PrevQuestId` = 5097 WHERE `entry` <=> 5533 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5534"} Kim'jael's "Missing" Equipment
UPDATE `quest_template` SET `PrevQuestId` = 3601 WHERE `entry` <=> 5534 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5582"} Healthy Dragon Scale
UPDATE `quest_template` SET `PrevQuestId` = 5529 WHERE `entry` <=> 5582 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5672"} Elune's Grace
UPDATE `quest_template` SET `RewSpell` = 52648 WHERE `entry` <=> 5672 AND (BINARY CAST(`RewSpell` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3532363338' USING utf8mb4));
-- quest_template {"entry": "5673"} Elune's Grace
UPDATE `quest_template` SET `RewSpell` = 52648 WHERE `entry` <=> 5673 AND (BINARY CAST(`RewSpell` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3532363338' USING utf8mb4));
-- quest_template {"entry": "5674"} Elune's Grace
UPDATE `quest_template` SET `RewSpell` = 52648 WHERE `entry` <=> 5674 AND (BINARY CAST(`RewSpell` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3532363338' USING utf8mb4));
-- quest_template {"entry": "5675"} Elune's Grace
UPDATE `quest_template` SET `RewSpell` = 52648 WHERE `entry` <=> 5675 AND (BINARY CAST(`RewSpell` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3532363338' USING utf8mb4));
-- quest_template {"entry": "5727"} Hidden Enemies
UPDATE `quest_template` SET `PrevQuestId` = 5726 WHERE `entry` <=> 5727 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5742"} Redemption
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 5742 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35353432' USING utf8mb4));
-- quest_template {"entry": "40333"} The Bloodsail Compound IV
UPDATE `quest_template` SET `PrevQuestId` = 40332 WHERE `entry` <=> 40333 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40332"} The Bloodsail Compound III
UPDATE `quest_template` SET `PrevQuestId` = 40331 WHERE `entry` <=> 40332 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40331"} The Bloodsail Compound II
UPDATE `quest_template` SET `PrevQuestId` = 40330 WHERE `entry` <=> 40331 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5882"} Salve via Hunting
UPDATE `quest_template` SET `PrevQuestId` = 4101 WHERE `entry` <=> 5882 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5883"} Salve via Mining
UPDATE `quest_template` SET `PrevQuestId` = 4101 WHERE `entry` <=> 5883 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5884"} Salve via Gathering
UPDATE `quest_template` SET `PrevQuestId` = 4101 WHERE `entry` <=> 5884 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5885"} Salve via Skinning
UPDATE `quest_template` SET `PrevQuestId` = 4101 WHERE `entry` <=> 5885 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5886"} Salve via Disenchanting
UPDATE `quest_template` SET `PrevQuestId` = 4101 WHERE `entry` <=> 5886 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5887"} Salve via Hunting
UPDATE `quest_template` SET `PrevQuestId` = 4102 WHERE `entry` <=> 5887 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5888"} Salve via Mining
UPDATE `quest_template` SET `PrevQuestId` = 4102 WHERE `entry` <=> 5888 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5889"} Salve via Gathering
UPDATE `quest_template` SET `PrevQuestId` = 4102 WHERE `entry` <=> 5889 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5890"} Salve via Skinning
UPDATE `quest_template` SET `PrevQuestId` = 4102 WHERE `entry` <=> 5890 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5891"} Salve via Disenchanting
UPDATE `quest_template` SET `PrevQuestId` = 4102 WHERE `entry` <=> 5891 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5921"} Moonglade
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 5921 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35393233' USING utf8mb4));
-- quest_template {"entry": "5922"} Moonglade
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 5922 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'35393236' USING utf8mb4));
-- quest_template {"entry": "5944"} In Dreams
UPDATE `quest_template` SET `PrevQuestId` = 5862 WHERE `entry` <=> 5944 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6022"} To Kill With Purpose
UPDATE `quest_template` SET `ReqSourceId1` = 15447, `ReqSourceCount1` = 7 WHERE `entry` <=> 6022 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3135343438' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- quest_template {"entry": "6135"} Duskwing, Oh How I Hate Thee...
UPDATE `quest_template` SET `PrevQuestId` = 6133 WHERE `entry` <=> 6135 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6136"} The Corpulent One
UPDATE `quest_template` SET `PrevQuestId` = 6022 WHERE `entry` <=> 6136 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6142"} Clam Bait
UPDATE `quest_template` SET `ReqSourceId1` = 15874 WHERE `entry` <=> 6142 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3135393234' USING utf8mb4));
-- quest_template {"entry": "6144"} The Call to Command
UPDATE `quest_template` SET `PrevQuestId` = 6135 WHERE `entry` <=> 6144 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6146"} Nathanos' Ruse
UPDATE `quest_template` SET `ReqSourceId2` = 15875, `ReqSourceCount2` = 1 WHERE `entry` <=> 6146 AND (BINARY CAST(`ReqSourceId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6381"} New Life
UPDATE `quest_template` SET `PrevQuestId` = 6301, `ReqCreatureOrGOId1` = -177929 WHERE `entry` <=> 6381 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "6382"} The Ashenvale Hunt
UPDATE `quest_template` SET `PrevQuestId` = 882 WHERE `entry` <=> 6382 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6395"} Marla's Last Wish
UPDATE `quest_template` SET `PrevQuestId` = 376, `ReqSourceId1` = 16333, `ReqSourceCount1` = 1, `ReqCreatureOrGOId1` = -178090 WHERE `entry` <=> 6395 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "6403"} The Great Masquerade
UPDATE `quest_template` SET `PrevQuestId` = 6402 WHERE `entry` <=> 6403 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6481"} Earthen Arise
UPDATE `quest_template` SET `PrevQuestId` = 6421 WHERE `entry` <=> 6481 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6543"} The Warsong Reports
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 6543 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36353431' USING utf8mb4));
-- quest_template {"entry": "6545"} Warsong Runner Update
UPDATE `quest_template` SET `PrevQuestId` = -6543 WHERE `entry` <=> 6545 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6546"} Warsong Outrider Update
UPDATE `quest_template` SET `PrevQuestId` = -6543 WHERE `entry` <=> 6546 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6547"} Warsong Scout Update
UPDATE `quest_template` SET `PrevQuestId` = -6543 WHERE `entry` <=> 6547 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6566"} What the Wind Carries
UPDATE `quest_template` SET `PrevQuestId` = 4974 WHERE `entry` <=> 6566 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6581"} Warsong Saw Blades
UPDATE `quest_template` SET `PrevQuestId` = -6571 WHERE `entry` <=> 6581 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6582"} The Test of Skulls, Scryer
UPDATE `quest_template` SET `PrevQuestId` = 6570 WHERE `entry` <=> 6582 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6583"} The Test of Skulls, Somnus
UPDATE `quest_template` SET `PrevQuestId` = 6570 WHERE `entry` <=> 6583 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6584"} The Test of Skulls, Chronalis
UPDATE `quest_template` SET `PrevQuestId` = 6570 WHERE `entry` <=> 6584 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6610"} Clamlette Surprise
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 6610 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36363131' USING utf8mb4));
-- quest_template {"entry": "6621"} King of the Foulweald
UPDATE `quest_template` SET `PrevQuestId` = 216 WHERE `entry` <=> 6621 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6662"} Me Brother, Nipsy
UPDATE `quest_template` SET `PrevQuestId` = 6661 WHERE `entry` <=> 6662 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6701"} Syndicate Emblems
UPDATE `quest_template` SET `PrevQuestId` = 6681 WHERE `entry` <=> 6701 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6741"} More Booty!
UPDATE `quest_template` SET `PrevQuestId` = 7224 WHERE `entry` <=> 6741 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6761"} The New Frontier
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 6761 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31303135' USING utf8mb4));
-- quest_template {"entry": "6781"} More Armor Scraps
UPDATE `quest_template` SET `PrevQuestId` = 7223 WHERE `entry` <=> 6781 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7046"} The Scepter of Celebras
UPDATE `quest_template` SET `PrevQuestId` = 7044 WHERE `entry` <=> 7046 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7067"} The Pariah's Instructions
UPDATE `quest_template` SET `ReqSourceId1` = 17757 WHERE `entry` <=> 7067 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3137373538' USING utf8mb4));
-- quest_template {"entry": "8407"} Fight for Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8402 WHERE `entry` <=> 8407 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8408"} Fight for Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8403 WHERE `entry` <=> 8408 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8406"} Fight for Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8401 WHERE `entry` <=> 8406 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8405"} Fight for Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8400 WHERE `entry` <=> 8405 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8404"} Fight for Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8399 WHERE `entry` <=> 8404 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7201"} The Last Element
UPDATE `quest_template` SET `PrevQuestId` = 3906 WHERE `entry` <=> 7201 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8398"} Claiming Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8396 WHERE `entry` <=> 8398 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7421"} Darkspear Defense
UPDATE `quest_template` SET `PrevQuestId` = 7361 WHERE `entry` <=> 7421 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7422"} Tuft it Out
UPDATE `quest_template` SET `PrevQuestId` = 7362 WHERE `entry` <=> 7422 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7423"} I've Got A Fever For More Bone Chips
UPDATE `quest_template` SET `PrevQuestId` = 7363 WHERE `entry` <=> 7423 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7424"} What the Hoof?
UPDATE `quest_template` SET `PrevQuestId` = 7364 WHERE `entry` <=> 7424 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7425"} Staghelm's Mojo Jamboree
UPDATE `quest_template` SET `PrevQuestId` = 7365 WHERE `entry` <=> 7425 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7426"} One Man's Love
UPDATE `quest_template` SET `PrevQuestId` = 7366 WHERE `entry` <=> 7426 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7427"} Wanted: MORE DWARVES!
UPDATE `quest_template` SET `PrevQuestId` = 7401 WHERE `entry` <=> 7427 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7428"} Wanted: MORE ORCS!
UPDATE `quest_template` SET `PrevQuestId` = 7402 WHERE `entry` <=> 7428 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7429"} Free Knot!
UPDATE `quest_template` SET `PrevQuestId` = 5525 WHERE `entry` <=> 7429 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7462"} The Treasure of the Shen'dralar
UPDATE `quest_template` SET `PrevQuestId` = 7461 WHERE `entry` <=> 7462 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7483"} Libram of Rapidity
UPDATE `quest_template` SET `PrevQuestId` = 7482 WHERE `entry` <=> 7483 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7484"} Libram of Focus
UPDATE `quest_template` SET `PrevQuestId` = 7482 WHERE `entry` <=> 7484 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7485"} Libram of Protection
UPDATE `quest_template` SET `PrevQuestId` = 7482 WHERE `entry` <=> 7485 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7488"} Lethtendris's Web
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 7488 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37343934' USING utf8mb4));
-- quest_template {"entry": "7489"} Lethtendris's Web
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 7489 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'37343932' USING utf8mb4));
-- quest_template {"entry": "7493"} The Journey Has Just Begun
UPDATE `quest_template` SET `PrevQuestId` = 7491 WHERE `entry` <=> 7493 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7497"} The Journey Has Just Begun
UPDATE `quest_template` SET `PrevQuestId` = 7496 WHERE `entry` <=> 7497 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7541"} Service to the Horde
UPDATE `quest_template` SET `PrevQuestId` = 1262 WHERE `entry` <=> 7541 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7623"} Lord Banehollow
UPDATE `quest_template` SET `PrevQuestId` = 7564 WHERE `entry` <=> 7623 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7626"} Bell of Dethmoora
UPDATE `quest_template` SET `PrevQuestId` = 7564 WHERE `entry` <=> 7626 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7627"} Wheel of the Black March
UPDATE `quest_template` SET `PrevQuestId` = 7564 WHERE `entry` <=> 7627 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7628"} Doomsday Candle
UPDATE `quest_template` SET `PrevQuestId` = 7564 WHERE `entry` <=> 7628 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7631"} Dreadsteed of Xoroth
UPDATE `quest_template` SET `PrevQuestId` = 7629 WHERE `entry` <=> 7631 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7633"} An Introduction
UPDATE `quest_template` SET `PrevQuestId` = 7632 WHERE `entry` <=> 7633 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7634"} Ancient Sinew Wrapped Lamina
UPDATE `quest_template` SET `PrevQuestId` = 7633 WHERE `entry` <=> 7634 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7635"} A Proper String
UPDATE `quest_template` SET `PrevQuestId` = 7633 WHERE `entry` <=> 7635 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7641"} The Work of Grimand Elmore
UPDATE `quest_template` SET `PrevQuestId` = 7640 WHERE `entry` <=> 7641 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7643"} Ancient Equine Spirit
UPDATE `quest_template` SET `PrevQuestId` = 7642 WHERE `entry` <=> 7643 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7645"} Manna-Enriched Horse Feed
UPDATE `quest_template` SET `PrevQuestId` = -7643 WHERE `entry` <=> 7645 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7647"} Judgment and Redemption
UPDATE `quest_template` SET `ReqSourceId1` = 18749 WHERE `entry` <=> 7647 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3138373939' USING utf8mb4));
-- quest_template {"entry": "7648"} Grimand's Finest Work
UPDATE `quest_template` SET `PrevQuestId` = 7642 WHERE `entry` <=> 7648 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7653"} Imperial Plate Belt
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 7653 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7654"} Imperial Plate Boots
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 7654 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7655"} Imperial Plate Bracer
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 7655 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7656"} Imperial Plate Chest
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 7656 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7657"} Imperial Plate Helm
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 7657 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7658"} Imperial Plate Leggings
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 7658 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7659"} Imperial Plate Shoulders
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 7659 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40002"} Grand Herbal Theft
UPDATE `quest_template` SET `PrevQuestId` = 40001 WHERE `entry` <=> 40002 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70000"} A Special Kind of Summons
UPDATE `quest_template` SET `PrevQuestId` = 7463 WHERE `entry` <=> 70000 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7666"} Again Into the Great Ossuary
UPDATE `quest_template` SET `PrevQuestId` = 7647 WHERE `entry` <=> 7666 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7725"} Again With the Zapped Giants
UPDATE `quest_template` SET `PrevQuestId` = 7003 WHERE `entry` <=> 7725 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7726"} Refuel for the Zapping
UPDATE `quest_template` SET `PrevQuestId` = 7721 WHERE `entry` <=> 7726 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7730"} Zukk'ash Infestation
UPDATE `quest_template` SET `PrevQuestId` = 2903 WHERE `entry` <=> 7730 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7731"} Stinglasher
UPDATE `quest_template` SET `PrevQuestId` = 2903 WHERE `entry` <=> 7731 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7733"} Improved Quality
UPDATE `quest_template` SET `PrevQuestId` = 2821 WHERE `entry` <=> 7733 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7734"} Improved Quality
UPDATE `quest_template` SET `PrevQuestId` = 2822 WHERE `entry` <=> 7734 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7737"} Gaining Acceptance
UPDATE `quest_template` SET `PrevQuestId` = 7722 WHERE `entry` <=> 7737 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8241"} Restoring Fiery Flux Supplies via Iron
UPDATE `quest_template` SET `PrevQuestId` = 7722 WHERE `entry` <=> 8241 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7787"} Rise, Thunderfury!
UPDATE `quest_template` SET `PrevQuestId` = 7786 WHERE `entry` <=> 7787 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8397"} Claiming Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8395 WHERE `entry` <=> 8397 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7796"} Additional Runecloth: Stormwind
UPDATE `quest_template` SET `PrevQuestId` = 7795 WHERE `entry` <=> 7796 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7801"} Additional Runecloth: Darnassus
UPDATE `quest_template` SET `PrevQuestId` = 7800 WHERE `entry` <=> 7801 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7806"} Additional Runecloth: Ironforge
UPDATE `quest_template` SET `PrevQuestId` = 7805 WHERE `entry` <=> 7806 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8392"} Claiming Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8394 WHERE `entry` <=> 8392 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8391"} Claiming Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8393 WHERE `entry` <=> 8391 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7812"} Additional Runecloth: Gnomeregan
UPDATE `quest_template` SET `PrevQuestId` = 7811 WHERE `entry` <=> 7812 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8390"} Conquering Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8370 WHERE `entry` <=> 8390 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8389"} Battle of Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8368 WHERE `entry` <=> 8389 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7819"} Additional Runecloth: Undercity
UPDATE `quest_template` SET `PrevQuestId` = 7818 WHERE `entry` <=> 7819 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8388"} For Great Honor
UPDATE `quest_template` SET `PrevQuestId` = 8367 WHERE `entry` <=> 8388 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8387"} Invaders of Alterac Valley
UPDATE `quest_template` SET `PrevQuestId` = 8369 WHERE `entry` <=> 8387 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8385"} Concerted Efforts
UPDATE `quest_template` SET `PrevQuestId` = 8371 WHERE `entry` <=> 8385 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8386"} Fight for Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8372 WHERE `entry` <=> 8386 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8384"} Claiming Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8374 WHERE `entry` <=> 8384 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7825"} Additional Runecloth: Thunder Bluff
UPDATE `quest_template` SET `PrevQuestId` = 7823 WHERE `entry` <=> 7825 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8383"} Remember Alterac Valley!
UPDATE `quest_template` SET `PrevQuestId` = 8375 WHERE `entry` <=> 8383 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7837"} Additional Runecloth: Darkspear Tribe
UPDATE `quest_template` SET `PrevQuestId` = 7836 WHERE `entry` <=> 7837 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7832"} Additional Runecloth: Orgrimmar
UPDATE `quest_template` SET `PrevQuestId` = 7824 WHERE `entry` <=> 7832 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7838"} Arena Grandmaster
UPDATE `quest_template` SET `PrevQuestId` = 7810 WHERE `entry` <=> 7838 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7925"} Mark of Honor
UPDATE `quest_template` SET `PrevQuestId` = 7789 WHERE `entry` <=> 7925 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7924"} Mark of Honor
UPDATE `quest_template` SET `PrevQuestId` = 7874 WHERE `entry` <=> 7924 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7922"} Mark of Honor
UPDATE `quest_template` SET `PrevQuestId` = 7876 WHERE `entry` <=> 7922 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7923"} Mark of Honor
UPDATE `quest_template` SET `PrevQuestId` = 7875 WHERE `entry` <=> 7923 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7921"} Talismans of Merit
UPDATE `quest_template` SET `PrevQuestId` = 7788 WHERE `entry` <=> 7921 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7877"} The Treasure of the Shen'dralar
UPDATE `quest_template` SET `PrevQuestId` = 7461 WHERE `entry` <=> 7877 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7939"} More Dense Grinding Stones
UPDATE `quest_template` SET `PrevQuestId` = 7893 WHERE `entry` <=> 7939 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7941"} More Armor Kits
UPDATE `quest_template` SET `PrevQuestId` = 7885 WHERE `entry` <=> 7941 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7942"} More Thorium Widgets
UPDATE `quest_template` SET `PrevQuestId` = 7898 WHERE `entry` <=> 7942 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7943"} More Bat Eyes
UPDATE `quest_template` SET `PrevQuestId` = 7903 WHERE `entry` <=> 7943 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8042"} Strength of Mount Mugamba
UPDATE `quest_template` SET `PrevQuestId` = 8041 WHERE `entry` <=> 8042 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8043"} Strength of Mount Mugamba
UPDATE `quest_template` SET `PrevQuestId` = 8042 WHERE `entry` <=> 8043 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8044"} The Rage of Mount Mugamba
UPDATE `quest_template` SET `PrevQuestId` = 8043 WHERE `entry` <=> 8044 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8046"} The Heathen's Brand
UPDATE `quest_template` SET `PrevQuestId` = 8045 WHERE `entry` <=> 8046 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8047"} The Heathen's Brand
UPDATE `quest_template` SET `PrevQuestId` = 8046 WHERE `entry` <=> 8047 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8048"} The Hero's Brand
UPDATE `quest_template` SET `PrevQuestId` = 8047 WHERE `entry` <=> 8048 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8050"} The Eye of Zuldazar
UPDATE `quest_template` SET `PrevQuestId` = 8049 WHERE `entry` <=> 8050 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8051"} The Eye of Zuldazar
UPDATE `quest_template` SET `PrevQuestId` = 8050 WHERE `entry` <=> 8051 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8052"} The All-Seeing Eye of Zuldazar
UPDATE `quest_template` SET `PrevQuestId` = 8051 WHERE `entry` <=> 8052 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7888"} Talismans of Merit
UPDATE `quest_template` SET `PrevQuestId` = 7872 WHERE `entry` <=> 7888 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8102"} The Pebble of Kajaro
UPDATE `quest_template` SET `PrevQuestId` = 8101 WHERE `entry` <=> 8102 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8103"} The Pebble of Kajaro
UPDATE `quest_template` SET `PrevQuestId` = 8102 WHERE `entry` <=> 8103 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8104"} The Jewel of Kajaro
UPDATE `quest_template` SET `PrevQuestId` = 8103 WHERE `entry` <=> 8104 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8107"} Kezan's Taint
UPDATE `quest_template` SET `PrevQuestId` = 8106 WHERE `entry` <=> 8107 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8108"} Kezan's Taint
UPDATE `quest_template` SET `PrevQuestId` = 8107 WHERE `entry` <=> 8108 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8109"} Kezan's Unstoppable Taint
UPDATE `quest_template` SET `PrevQuestId` = 8108 WHERE `entry` <=> 8109 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8111"} Enchanted South Seas Kelp
UPDATE `quest_template` SET `PrevQuestId` = 8110 WHERE `entry` <=> 8111 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8112"} Enchanted South Seas Kelp
UPDATE `quest_template` SET `PrevQuestId` = 8111 WHERE `entry` <=> 8112 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8113"} Pristine Enchanted South Seas Kelp
UPDATE `quest_template` SET `PrevQuestId` = 8112 WHERE `entry` <=> 8113 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8117"} Vision of Voodress
UPDATE `quest_template` SET `PrevQuestId` = 8116 WHERE `entry` <=> 8117 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8118"} Vision of Voodress
UPDATE `quest_template` SET `PrevQuestId` = 8117 WHERE `entry` <=> 8118 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8119"} The Unmarred Vision of Voodress
UPDATE `quest_template` SET `PrevQuestId` = 8118 WHERE `entry` <=> 8119 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7886"} Talismans of Merit
UPDATE `quest_template` SET `PrevQuestId` = 7873 WHERE `entry` <=> 7886 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7887"} Talismans of Merit
UPDATE `quest_template` SET `PrevQuestId` = 7871 WHERE `entry` <=> 7887 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8142"} Zandalrian Shadow Talisman
UPDATE `quest_template` SET `PrevQuestId` = 8141 WHERE `entry` <=> 8142 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8143"} Zandalrian Shadow Talisman
UPDATE `quest_template` SET `PrevQuestId` = 8142 WHERE `entry` <=> 8143 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8144"} Zandalrian Shadow Mastery Talisman
UPDATE `quest_template` SET `PrevQuestId` = 8143 WHERE `entry` <=> 8144 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8146"} The Maelstrom's Tendril
UPDATE `quest_template` SET `PrevQuestId` = 8145 WHERE `entry` <=> 8146 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8147"} The Maelstrom's Tendril
UPDATE `quest_template` SET `PrevQuestId` = 8146 WHERE `entry` <=> 8147 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8148"} The Maelstrom's Wrath
UPDATE `quest_template` SET `PrevQuestId` = 8147 WHERE `entry` <=> 8148 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8181"} Confront Yeh'kinya
UPDATE `quest_template` SET `PrevQuestId` = 4788 WHERE `entry` <=> 8181 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8182"} The Hand of Rastakhan
UPDATE `quest_template` SET `PrevQuestId` = 8181 WHERE `entry` <=> 8182 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8242"} Restoring Fiery Flux Supplies via Heavy Leather
UPDATE `quest_template` SET `PrevQuestId` = 7722 WHERE `entry` <=> 8242 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7736"} Restoring Fiery Flux Supplies via Kingsblood
UPDATE `quest_template` SET `PrevQuestId` = 7722 WHERE `entry` <=> 7736 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8251"} Magic Dust
UPDATE `quest_template` SET `PrevQuestId` = 8250 WHERE `entry` <=> 8251 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8278"} Noggle's Last Hope
UPDATE `quest_template` SET `PrevQuestId` = 8277 WHERE `entry` <=> 8278 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8280"} Securing the Supply Lines
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8280 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38323735' USING utf8mb4));
-- quest_template {"entry": "8282"} Noggle's Lost Satchel
UPDATE `quest_template` SET `PrevQuestId` = 8278 WHERE `entry` <=> 8282 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8288"} Only One May Rise
UPDATE `quest_template` SET `PrevQuestId` = 8286 WHERE `entry` <=> 8288 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8302"} The Hand of the Righteous
UPDATE `quest_template` SET `PrevQuestId` = 8301 WHERE `entry` <=> 8302 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8303"} Anachronos
UPDATE `quest_template` SET `PrevQuestId` = 8301 WHERE `entry` <=> 8303 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8309"} Glyph Chasing
UPDATE `quest_template` SET `PrevQuestId` = 8304 WHERE `entry` <=> 8309 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8310"} Breaking the Code
UPDATE `quest_template` SET `PrevQuestId` = 8304 WHERE `entry` <=> 8310 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8319"} Encrypted Twilight Texts
UPDATE `quest_template` SET `PrevQuestId` = 8318 WHERE `entry` <=> 8319 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8323"} True Believers
UPDATE `quest_template` SET `PrevQuestId` = 8279 WHERE `entry` <=> 8323 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7382"} Korrak the Everliving
UPDATE `quest_template` SET `PrevQuestId` = 7202 WHERE `entry` <=> 7382 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7381"} The Return of Korrak
UPDATE `quest_template` SET `PrevQuestId` = 7181 WHERE `entry` <=> 7381 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8413"} Da Voodoo
UPDATE `quest_template` SET `PrevQuestId` = 8412 WHERE `entry` <=> 8413 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8416"} Inert Scourgestones
UPDATE `quest_template` SET `PrevQuestId` = 8414 WHERE `entry` <=> 8416 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8421"} The Wrong Stuff
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8421 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38343139' USING utf8mb4));
-- quest_template {"entry": "7171"} Legendary Heroes
UPDATE `quest_template` SET `PrevQuestId` = 7170 WHERE `entry` <=> 7171 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7172"} The Eye of Command
UPDATE `quest_template` SET `PrevQuestId` = 7171 WHERE `entry` <=> 7172 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7169"} Honored Amongst the Guard
UPDATE `quest_template` SET `PrevQuestId` = 7168 WHERE `entry` <=> 7169 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7170"} Earned Reverence
UPDATE `quest_template` SET `PrevQuestId` = 7169 WHERE `entry` <=> 7170 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7166"} Legendary Heroes
UPDATE `quest_template` SET `PrevQuestId` = 7165 WHERE `entry` <=> 7166 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7167"} The Eye of Command
UPDATE `quest_template` SET `PrevQuestId` = 7166 WHERE `entry` <=> 7167 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7168"} Rise and Be Recognized
UPDATE `quest_template` SET `PrevQuestId` = 7162 WHERE `entry` <=> 7168 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7164"} Honored Amongst the Clan
UPDATE `quest_template` SET `PrevQuestId` = 7161 WHERE `entry` <=> 7164 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7165"} Earned Reverence
UPDATE `quest_template` SET `PrevQuestId` = 7164 WHERE `entry` <=> 7165 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7163"} Rise and Be Recognized
UPDATE `quest_template` SET `PrevQuestId` = 7161 WHERE `entry` <=> 7163 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3089"} Rune-Inscribed Parchment
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3089 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3088"} Encrypted Parchment
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3088 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3084"} Rune-Inscribed Tablet
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3084 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1463"} Earth Sapta
UPDATE `quest_template` SET `PrevQuestId` = -1517 WHERE `entry` <=> 1463 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3083"} Encrypted Tablet
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 3083 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1462"} Earth Sapta
UPDATE `quest_template` SET `PrevQuestId` = -1520 WHERE `entry` <=> 1462 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "924"} The Demon Seed
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -3525 WHERE `entry` <=> 924 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "8462"} Speak to Nafien
UPDATE `quest_template` SET `PrevQuestId` = 8460 WHERE `entry` <=> 8462 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8465"} Speak to Salfa
UPDATE `quest_template` SET `PrevQuestId` = 8461 WHERE `entry` <=> 8465 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8466"} Feathers for Grazle
UPDATE `quest_template` SET `PrevQuestId` = 8460 WHERE `entry` <=> 8466 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8469"} Beads for Salfa
UPDATE `quest_template` SET `PrevQuestId` = 8464 WHERE `entry` <=> 8469 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8492"} The Alliance Needs Copper Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8492 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8493"} The Alliance Needs More Copper Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8492 WHERE `entry` <=> 8493 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8494"} The Alliance Needs Iron Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8494 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8495"} The Alliance Needs More Iron Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8494 WHERE `entry` <=> 8495 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8499"} The Alliance Needs Thorium Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8499 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8500"} The Alliance Needs More Thorium Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8499 WHERE `entry` <=> 8500 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8503"} The Alliance Needs Stranglekelp!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8503 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8504"} The Alliance Needs More Stranglekelp!
UPDATE `quest_template` SET `PrevQuestId` = 8503 WHERE `entry` <=> 8504 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8505"} The Alliance Needs Purple Lotus!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8505 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8506"} The Alliance Needs More Purple Lotus!
UPDATE `quest_template` SET `PrevQuestId` = 8505 WHERE `entry` <=> 8506 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8509"} The Alliance Needs Arthas' Tears!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8509 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8510"} The Alliance Needs More Arthas' Tears!
UPDATE `quest_template` SET `PrevQuestId` = 8509 WHERE `entry` <=> 8510 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8511"} The Alliance Needs Light Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8511 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8512"} The Alliance Needs More Light Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8511 WHERE `entry` <=> 8512 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8513"} The Alliance Needs Medium Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8513 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8514"} The Alliance Needs More Medium Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8513 WHERE `entry` <=> 8514 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8515"} The Alliance Needs Thick Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8515 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8516"} The Alliance Needs More Thick Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8515 WHERE `entry` <=> 8516 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8517"} The Alliance Needs Linen Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8517 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8518"} The Alliance Needs More Linen Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8517 WHERE `entry` <=> 8518 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8520"} The Alliance Needs Silk Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8520 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8521"} The Alliance Needs More Silk Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8520 WHERE `entry` <=> 8521 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8522"} The Alliance Needs Runecloth Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8522 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8523"} The Alliance Needs More Runecloth Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8522 WHERE `entry` <=> 8523 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8524"} The Alliance Needs Rainbow Fin Albacore!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8524 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8525"} The Alliance Needs More Rainbow Fin Albacore!
UPDATE `quest_template` SET `PrevQuestId` = 8524 WHERE `entry` <=> 8525 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8526"} The Alliance Needs Roast Raptor!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8526 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8527"} The Alliance Needs More Roast Raptor!
UPDATE `quest_template` SET `PrevQuestId` = 8526 WHERE `entry` <=> 8527 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8528"} The Alliance Needs Spotted Yellowtail!
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8528 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8529"} The Alliance Needs More Spotted Yellowtail!
UPDATE `quest_template` SET `PrevQuestId` = 8528 WHERE `entry` <=> 8529 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8532"} The Horde Needs Copper Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8532 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8533"} The Horde Needs More Copper Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8532 WHERE `entry` <=> 8533 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8542"} The Horde Needs Tin Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8542 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8543"} The Horde Needs More Tin Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8542 WHERE `entry` <=> 8543 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8545"} The Horde Needs Mithril Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8545 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8546"} The Horde Needs More Mithril Bars!
UPDATE `quest_template` SET `PrevQuestId` = 8545 WHERE `entry` <=> 8546 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8548"} Volunteer's Battlegear
UPDATE `quest_template` SET `PrevQuestId` = 8800 WHERE `entry` <=> 8548 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8549"} The Horde Needs Peacebloom!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8549 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8550"} The Horde Needs More Peacebloom!
UPDATE `quest_template` SET `PrevQuestId` = 8549 WHERE `entry` <=> 8550 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8553"} The Captain's Cutlass
UPDATE `quest_template` SET `PrevQuestId` = 8552 WHERE `entry` <=> 8553 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8572"} Veteran's Battlegear
UPDATE `quest_template` SET `PrevQuestId` = 8800 WHERE `entry` <=> 8572 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8573"} Champion's Battlegear
UPDATE `quest_template` SET `PrevQuestId` = 8800 WHERE `entry` <=> 8573 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8577"} Stewvul, Ex-B.F.F.
UPDATE `quest_template` SET `PrevQuestId` = 8576 WHERE `entry` <=> 8577 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8579"} Mortal Champions
UPDATE `quest_template` SET `PrevQuestId` = 8595 WHERE `entry` <=> 8579 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8580"} The Horde Needs Firebloom!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8580 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8581"} The Horde Needs More Firebloom!
UPDATE `quest_template` SET `PrevQuestId` = 8580 WHERE `entry` <=> 8581 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8582"} The Horde Needs Purple Lotus!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8582 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8583"} The Horde Needs More Purple Lotus!
UPDATE `quest_template` SET `PrevQuestId` = 8582 WHERE `entry` <=> 8583 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8584"} Never Ask Me About My Business
UPDATE `quest_template` SET `PrevQuestId` = 8576 WHERE `entry` <=> 8584 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8588"} The Horde Needs Heavy Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8588 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8589"} The Horde Needs More Heavy Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8588 WHERE `entry` <=> 8589 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8590"} The Horde Needs Thick Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8590 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8591"} The Horde Needs More Thick Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8590 WHERE `entry` <=> 8591 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8597"} Draconic for Dummies
UPDATE `quest_template` SET `PrevQuestId` = 8576 WHERE `entry` <=> 8597 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8600"} The Horde Needs Rugged Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8600 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8601"} The Horde Needs More Rugged Leather!
UPDATE `quest_template` SET `PrevQuestId` = 8600 WHERE `entry` <=> 8601 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8604"} The Horde Needs Wool Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8604 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8605"} The Horde Needs More Wool Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8604 WHERE `entry` <=> 8605 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8607"} The Horde Needs Mageweave Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8607 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8608"} The Horde Needs More Mageweave Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8607 WHERE `entry` <=> 8608 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8609"} The Horde Needs Runecloth Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8609 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8610"} The Horde Needs More Runecloth Bandages!
UPDATE `quest_template` SET `PrevQuestId` = 8609 WHERE `entry` <=> 8610 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8611"} The Horde Needs Lean Wolf Steaks!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8611 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8612"} The Horde Needs More Lean Wolf Steaks!
UPDATE `quest_template` SET `PrevQuestId` = 8611 WHERE `entry` <=> 8612 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8613"} The Horde Needs Spotted Yellowtail!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8613 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8614"} The Horde Needs More Spotted Yellowtail!
UPDATE `quest_template` SET `PrevQuestId` = 8613 WHERE `entry` <=> 8614 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8615"} The Horde Needs Baked Salmon!
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8615 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8616"} The Horde Needs More Baked Salmon!
UPDATE `quest_template` SET `PrevQuestId` = 8615 WHERE `entry` <=> 8616 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8733"} Eranikus, Tyrant of the Dream
UPDATE `quest_template` SET `PrevQuestId` = 8555 WHERE `entry` <=> 8733 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8736"} The Nightmare Manifests
UPDATE `quest_template` SET `PrevQuestId` = 8735 WHERE `entry` <=> 8736 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8830"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8830 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8831"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8831 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8832"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8832 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8833"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8833 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8834"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8834 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8835"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8835 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8836"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8836 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8837"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8837 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8838"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8838 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8839"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8839 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8840"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8840 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8841"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8841 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8842"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8842 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8843"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8843 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8844"} One Commendation Signet
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8844 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8845"} Ten Commendation Signets
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8845 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8867"} Lunar Fireworks
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8867 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38383730' USING utf8mb4));
-- quest_template {"entry": "8893"} The Super Egg-O-Matic
UPDATE `quest_template` SET `PrevQuestId` = 2741 WHERE `entry` <=> 8893 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8921"} The Ectoplasmic Distiller
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8921 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393232' USING utf8mb4));
-- quest_template {"entry": "8926"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8926 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8927"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8927 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8929"} In Search of Anthion
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8929 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393236' USING utf8mb4));
-- quest_template {"entry": "8930"} In Search of Anthion
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8930 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393237' USING utf8mb4));
-- quest_template {"entry": "8931"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8931 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8932"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8932 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8933"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8933 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8934"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8934 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8935"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8935 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8936"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8936 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8937"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8977 WHERE `entry` <=> 8937 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8938"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8938 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8939"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8939 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8940"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8940 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8941"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8941 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8942"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8942 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8943"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8943 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8944"} Just Compensation
UPDATE `quest_template` SET `PrevQuestId` = 8978 WHERE `entry` <=> 8944 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8945"} Dead Man's Plea
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8945 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393239' USING utf8mb4));
-- quest_template {"entry": "8951"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8951 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8952"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8952 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8953"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8953 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8954"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8954 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8955"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8955 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8956"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8956 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8957"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8957 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8958"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8958 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8959"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 8959 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8960"} Bodley's Unfortunate Fate
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8960 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393531' USING utf8mb4));
-- quest_template {"entry": "8961"} Three Kings of Flame
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8961 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393630' USING utf8mb4));
-- quest_template {"entry": "8962"} Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8961 WHERE `entry` <=> 8962 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8963"} Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8961 WHERE `entry` <=> 8963 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8964"} Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8961 WHERE `entry` <=> 8964 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8965"} Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8961 WHERE `entry` <=> 8965 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8970"} I See Alcaz Island In Your Future...
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8970 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393636' USING utf8mb4));
-- quest_template {"entry": "8977"} Return to Deliana
UPDATE `quest_template` SET `PrevQuestId` = 8928 WHERE `entry` <=> 8977 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8978"} Return to Mokvar
UPDATE `quest_template` SET `PrevQuestId` = 8928 WHERE `entry` <=> 8978 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8985"} More Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8970 WHERE `entry` <=> 8985 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8986"} More Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8970 WHERE `entry` <=> 8986 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8987"} More Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8970 WHERE `entry` <=> 8987 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8988"} More Components of Importance
UPDATE `quest_template` SET `PrevQuestId` = 8970 WHERE `entry` <=> 8988 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8989"} The Right Piece of Lord Valthalak's Amulet
UPDATE `quest_template` SET `ReqSourceId1` = 22046 WHERE `entry` <=> 8989 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3232303438' USING utf8mb4));
-- quest_template {"entry": "8990"} The Right Piece of Lord Valthalak's Amulet
UPDATE `quest_template` SET `ReqSourceId1` = 22046 WHERE `entry` <=> 8990 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3232303438' USING utf8mb4));
-- quest_template {"entry": "8991"} The Right Piece of Lord Valthalak's Amulet
UPDATE `quest_template` SET `ReqSourceId1` = 22046 WHERE `entry` <=> 8991 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3232303438' USING utf8mb4));
-- quest_template {"entry": "8992"} The Right Piece of Lord Valthalak's Amulet
UPDATE `quest_template` SET `ReqSourceId1` = 22046 WHERE `entry` <=> 8992 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3232303438' USING utf8mb4));
-- quest_template {"entry": "8994"} Final Preparations
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 8994 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393839' USING utf8mb4));
-- quest_template {"entry": "8997"} Back to the Beginning
UPDATE `quest_template` SET `PrevQuestId` = 8996 WHERE `entry` <=> 8997 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8998"} Back to the Beginning
UPDATE `quest_template` SET `PrevQuestId` = 8996 WHERE `entry` <=> 8998 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8999"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 8999 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9000"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 9000 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9001"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 9001 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9002"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 9002 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9003"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 9003 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9004"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 9004 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9005"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 9005 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9006"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8997 WHERE `entry` <=> 9006 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9007"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9007 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9008"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9008 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9009"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9009 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9010"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9010 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9011"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9011 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9012"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9012 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9013"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9013 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9014"} Saving the Best for Last
UPDATE `quest_template` SET `PrevQuestId` = 8998 WHERE `entry` <=> 9014 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9015"} The Challenge
UPDATE `quest_template` SET `PrevQuestId` = 8950 WHERE `entry` <=> 9015 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9016"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 9016 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9017"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 9017 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9018"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 9018 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9019"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 9019 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9020"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 9020 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9021"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 9021 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9022"} Anthion's Parting Words
UPDATE `quest_template` SET `PrevQuestId` = 9015 WHERE `entry` <=> 9022 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9033"} Echoes of War
UPDATE `quest_template` SET `PrevQuestId` = 9121 WHERE `entry` <=> 9033 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9034"} Dreadnaught Breastplate
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9034 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9036"} Dreadnaught Legplates
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9036 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9037"} Dreadnaught Helmet
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9037 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9038"} Dreadnaught Pauldrons
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9038 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9039"} Dreadnaught Sabatons
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9039 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9040"} Dreadnaught Gauntlets
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9040 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9041"} Dreadnaught Waistguard
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9041 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9042"} Dreadnaught Bracers
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9042 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9043"} Redemption Tunic
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9043 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9044"} Redemption Legguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9044 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9045"} Redemption Headpiece
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9045 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9046"} Redemption Spaulders
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9046 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9047"} Redemption Boots
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9047 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9048"} Redemption Handguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9048 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9049"} Redemption Girdle
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9049 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9050"} Redemption Wristguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9050 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9054"} Cryptstalker Tunic
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9054 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9055"} Cryptstalker Legguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9055 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9056"} Cryptstalker Headpiece
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9056 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9057"} Cryptstalker Spaulders
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9057 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9058"} Cryptstalker Boots
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9058 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9059"} Cryptstalker Handguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9059 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9060"} Cryptstalker Girdle
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9060 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9061"} Cryptstalker Wristguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9061 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9068"} Earthshatter Tunic
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9068 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9069"} Earthshatter Legguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9069 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9070"} Earthshatter Headpiece
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9070 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9071"} Earthshatter Spaulders
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9071 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9072"} Earthshatter Boots
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9072 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9073"} Earthshatter Handguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9073 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9074"} Earthshatter Girdle
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9074 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9075"} Earthshatter Wristguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9075 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9077"} Bonescythe Breastplate
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9077 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9078"} Bonescythe Legplates
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9078 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9079"} Bonescythe Helmet
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9079 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9080"} Bonescythe Pauldrons
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9080 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9081"} Bonescythe Sabatons
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9081 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9082"} Bonescythe Gauntlets
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9082 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9083"} Bonescythe Waistguard
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9083 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9084"} Bonescythe Bracers
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9084 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9085"} Shadows of Doom
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9085 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9086"} Dreamwalker Tunic
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9086 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9087"} Dreamwalker Legguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9087 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9088"} Dreamwalker Headpiece
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9088 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9089"} Dreamwalker Spaulders
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9089 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9090"} Dreamwalker Boots
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9090 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9091"} Dreamwalker Handguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9091 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9092"} Dreamwalker Girdle
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9092 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9093"} Dreamwalker Wristguards
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9093 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9094"} Argent Dawn Gloves
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9094 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9095"} Frostfire Robe
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9095 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9096"} Frostfire Leggings
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9096 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9097"} Frostfire Circlet
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9097 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9098"} Frostfire Shoulderpads
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9098 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9099"} Frostfire Sandals
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9099 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9100"} Frostfire Gloves
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9100 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9101"} Frostfire Belt
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9101 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9102"} Frostfire Bindings
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9102 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9103"} Plagueheart Robe
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9103 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9104"} Plagueheart Leggings
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9104 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9105"} Plagueheart Circlet
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9105 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9106"} Plagueheart Shoulderpads
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9106 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9107"} Plagueheart Sandals
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9107 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9108"} Plagueheart Gloves
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9108 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9109"} Plagueheart Belt
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9109 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9110"} Plagueheart Bindings
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9110 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9111"} Robe of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9111 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9112"} Leggings of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9112 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9113"} Circlet of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9113 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9114"} Shoulderpads of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9114 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9115"} Sandals of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9115 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9116"} Gloves of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9116 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9117"} Belt of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9117 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9118"} Bindings of Faith
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9118 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9125"} Crypt Fiend Parts
UPDATE `quest_template` SET `PrevQuestId` = 9124 WHERE `entry` <=> 9125 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9127"} Bone Fragments
UPDATE `quest_template` SET `PrevQuestId` = 9126 WHERE `entry` <=> 9127 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9129"} Core of Elements
UPDATE `quest_template` SET `PrevQuestId` = 9128 WHERE `entry` <=> 9129 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9132"} Dark Iron Scraps
UPDATE `quest_template` SET `PrevQuestId` = 9131 WHERE `entry` <=> 9132 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9137"} Savage Fronds
UPDATE `quest_template` SET `PrevQuestId` = 9136 WHERE `entry` <=> 9137 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9229"} The Fate of Ramaladni
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9229 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9232"} The Only Song I Know...
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 9232 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9234"} Icebane Gauntlets
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9234 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9235"} Icebane Bracers
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9235 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9236"} Icebane Breastplate
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9236 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9237"} Glacial Cloak
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9237 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9238"} Glacial Wrists
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9238 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9239"} Glacial Gloves
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9239 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9240"} Glacial Vest
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9240 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9241"} Polar Bracers
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9241 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9242"} Polar Gloves
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9242 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9243"} Polar Tunic
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9243 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9244"} Icy Scale Bracers
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9244 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9245"} Icy Scale Gauntlets
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9245 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9246"} Icy Scale Breastplate
UPDATE `quest_template` SET `PrevQuestId` = 9233 WHERE `entry` <=> 9246 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9269"} Atiesh, Greatstaff of the Guardian
UPDATE `quest_template` SET `PrevQuestId` = 9251 WHERE `entry` <=> 9269 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9270"} Atiesh, Greatstaff of the Guardian
UPDATE `quest_template` SET `PrevQuestId` = 9251 WHERE `entry` <=> 9270 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9271"} Atiesh, Greatstaff of the Guardian
UPDATE `quest_template` SET `PrevQuestId` = 9251 WHERE `entry` <=> 9271 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9317"} Consecrated Sharpening Stones
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9317 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9319"} A Light in Dark Places
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -181346, `ReqCreatureOrGOId2` = -181347, `ReqCreatureOrGOId3` = -181348, `ReqCreatureOrGOId4` = -181349 WHERE `entry` <=> 9319 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "9320"} Major Mana Potion
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9320 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9322"} Wild Fires in Kalimdor
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -181342, `ReqCreatureOrGOId2` = -181339, `ReqCreatureOrGOId3` = -181343, `ReqCreatureOrGOId4` = -181340 WHERE `entry` <=> 9322 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "9323"} Wild Fires in the Eastern Kingdoms
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -181344, `ReqCreatureOrGOId2` = -181338, `ReqCreatureOrGOId3` = -181345, `ReqCreatureOrGOId4` = -181341 WHERE `entry` <=> 9323 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "9368"} The Festival of Fire
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -181336, `ReqCreatureOrGOId2` = -181337, `ReqCreatureOrGOId3` = -181335 WHERE `entry` <=> 9368 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "9386"} A Light in Dark Places
UPDATE `quest_template` SET `PrevQuestId` = 9319, `ReqCreatureOrGOId1` = -181346, `ReqCreatureOrGOId2` = -181347, `ReqCreatureOrGOId3` = -181348, `ReqCreatureOrGOId4` = -181349 WHERE `entry` <=> 9386 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "9422"} Scouring the Desert
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 9422 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'39343136' USING utf8mb4));
-- quest_template {"entry": "993"} A Lost Master
UPDATE `quest_template` SET `PrevQuestId` = 986 WHERE `entry` <=> 993 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "1390"} Oops, We Killed Them Again.
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 1390 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31323839' USING utf8mb4));
-- quest_template {"entry": "2361"} Restoring the Necklace
UPDATE `quest_template` SET `PrevQuestId` = 2204 WHERE `entry` <=> 2361 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2"} Sharptalon's Claw
UPDATE `quest_template` SET `PrevQuestId` = 6383 WHERE `entry` <=> 2 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8846"} Five Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8846 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8748"} The Path of the Protector
UPDATE `quest_template` SET `PrevQuestId` = 8747 WHERE `entry` <=> 8748 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8749"} The Path of the Protector
UPDATE `quest_template` SET `PrevQuestId` = 8748 WHERE `entry` <=> 8749 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8750"} The Path of the Protector
UPDATE `quest_template` SET `PrevQuestId` = 8749 WHERE `entry` <=> 8750 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8753"} The Path of the Conqueror
UPDATE `quest_template` SET `PrevQuestId` = 8752 WHERE `entry` <=> 8753 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8754"} The Path of the Conqueror
UPDATE `quest_template` SET `PrevQuestId` = 8753 WHERE `entry` <=> 8754 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8755"} The Path of the Conqueror
UPDATE `quest_template` SET `PrevQuestId` = 8754 WHERE `entry` <=> 8755 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8758"} The Path of the Invoker
UPDATE `quest_template` SET `PrevQuestId` = 8757 WHERE `entry` <=> 8758 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8759"} The Path of the Invoker
UPDATE `quest_template` SET `PrevQuestId` = 8758 WHERE `entry` <=> 8759 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8760"} The Path of the Invoker
UPDATE `quest_template` SET `PrevQuestId` = 8759 WHERE `entry` <=> 8760 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "972"} Water Sapta
UPDATE `quest_template` SET `PrevQuestId` = -63 WHERE `entry` <=> 972 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8223"} More Glowing Scorpid Blood
UPDATE `quest_template` SET `PrevQuestId` = 8223 WHERE `entry` <=> 8223 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8273"} Oran's Gratitude
UPDATE `quest_template` SET `PrevQuestId` = 2782 WHERE `entry` <=> 8273 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8316"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8316 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8324"} Still Believing
UPDATE `quest_template` SET `PrevQuestId` = 8323 WHERE `entry` <=> 8324 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8333"} Medallion of Station
UPDATE `quest_template` SET `PrevQuestId` = 8332 WHERE `entry` <=> 8333 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8342"} Twilight Ring of Lordship
UPDATE `quest_template` SET `PrevQuestId` = 8341 WHERE `entry` <=> 8342 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8362"} Abyssal Crests
UPDATE `quest_template` SET `PrevQuestId` = 8361 WHERE `entry` <=> 8362 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8363"} Abyssal Signets
UPDATE `quest_template` SET `PrevQuestId` = 8348 WHERE `entry` <=> 8363 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8364"} Abyssal Scepters
UPDATE `quest_template` SET `PrevQuestId` = 8352 WHERE `entry` <=> 8364 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8467"} Feathers for Nafien
UPDATE `quest_template` SET `PrevQuestId` = 8461 WHERE `entry` <=> 8467 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8508"} Field Duty Papers
UPDATE `quest_template` SET `PrevQuestId` = -8507 WHERE `entry` <=> 8508 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8574"} Stalwart's Battlegear
UPDATE `quest_template` SET `PrevQuestId` = 8800 WHERE `entry` <=> 8574 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8576"} Translating the Ledger
UPDATE `quest_template` SET `PrevQuestId` = 8575 WHERE `entry` <=> 8576 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8741"} The Champion Returns
UPDATE `quest_template` SET `PrevQuestId` = 8736 WHERE `entry` <=> 8741 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8743"} Bang a Gong!
UPDATE `quest_template` SET `PrevQuestId` = 8742 WHERE `entry` <=> 8743 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8745"} Treasure of the Timeless One
UPDATE `quest_template` SET `PrevQuestId` = 8742 WHERE `entry` <=> 8745 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8751"} The Protector of Kalimdor
UPDATE `quest_template` SET `PrevQuestId` = 8750 WHERE `entry` <=> 8751 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8756"} The Qiraji Conqueror
UPDATE `quest_template` SET `PrevQuestId` = 8755 WHERE `entry` <=> 8756 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8761"} The Grand Invoker
UPDATE `quest_template` SET `PrevQuestId` = 8760 WHERE `entry` <=> 8761 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8763"} The Hero of the Day
UPDATE `quest_template` SET `PrevQuestId` = 8762 WHERE `entry` <=> 8763 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8764"} The Changing of Paths - Protector No More
UPDATE `quest_template` SET `PrevQuestId` = 8751 WHERE `entry` <=> 8764 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8765"} The Changing of Paths - Invoker No More
UPDATE `quest_template` SET `PrevQuestId` = 8761 WHERE `entry` <=> 8765 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8766"} The Changing of Paths - Conqueror No More
UPDATE `quest_template` SET `PrevQuestId` = 8756 WHERE `entry` <=> 8766 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8798"} A Yeti of Your Own
UPDATE `quest_template` SET `PrevQuestId` = 5163 WHERE `entry` <=> 8798 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8847"} Ten Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8847 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8848"} Fifteen Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8848 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8849"} Twenty Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8849 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8850"} Thirty Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8795 WHERE `entry` <=> 8850 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9142"} Craftsman's Writ
UPDATE `quest_template` SET `PrevQuestId` = 9141 WHERE `entry` <=> 9142 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "3111"} Glyphic Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 3111 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "349"} Stranglethorn Fever
UPDATE `quest_template` SET `PrevQuestId` = -348 WHERE `entry` <=> 349 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9032"} Bodleys Unfortunate Fate
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 9032 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'38393537' USING utf8mb4));
-- quest_template {"entry": "8730"} Nefarius's Corruption
UPDATE `quest_template` SET `PrevQuestId` = 8555 WHERE `entry` <=> 8730 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8377"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8377 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8376"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8376 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8378"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8378 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8379"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8379 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8382"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8382 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8380"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8380 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8381"} Armaments of War
UPDATE `quest_template` SET `PrevQuestId` = 8315 WHERE `entry` <=> 8381 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8851"} Five Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8851 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8852"} Ten Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8852 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8853"} Fifteen Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8853 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8854"} Twenty Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8854 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8855"} Thirty Signets for War Supplies
UPDATE `quest_template` SET `PrevQuestId` = 8792 WHERE `entry` <=> 8855 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8435"} Battle of Warsong Gulch
UPDATE `quest_template` SET `PrevQuestId` = 8430 WHERE `entry` <=> 8435 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8440"} Conquering Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8436 WHERE `entry` <=> 8440 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8441"} Conquering Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8437 WHERE `entry` <=> 8441 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8442"} Conquering Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8438 WHERE `entry` <=> 8442 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8443"} Conquering Arathi Basin
UPDATE `quest_template` SET `PrevQuestId` = 8439 WHERE `entry` <=> 8443 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9341"} Tabard of the Argent Dawn
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9341 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9321"} Major Healing Potion
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9321 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9318"} Blessed Wizard Oil
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9318 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7480"} Libram of Protection
UPDATE `quest_template` SET `PrevQuestId` = 7481 WHERE `entry` <=> 7480 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7478"} Libram of Rapidity
UPDATE `quest_template` SET `PrevQuestId` = 7481 WHERE `entry` <=> 7478 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7479"} Libram of Focus
UPDATE `quest_template` SET `PrevQuestId` = 7481 WHERE `entry` <=> 7479 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6163"} Ramstein
UPDATE `quest_template` SET `PrevQuestId` = 6135 WHERE `entry` <=> 6163 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8555"} The Charge of the Dragonflights
UPDATE `quest_template` SET `PrevQuestId` = 8519 WHERE `entry` <=> 8555 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8575"} Azuregos's Magical Ledger
UPDATE `quest_template` SET `PrevQuestId` = 8555 WHERE `entry` <=> 8575 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8599"} Love Song for Narain
UPDATE `quest_template` SET `PrevQuestId` = -8597 WHERE `entry` <=> 8599 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8355"} Incoming Gumdrop
UPDATE `quest_template` SET `PrevQuestId` = -8311 WHERE `entry` <=> 8355 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8353"} Chicken Clucking for a Mint
UPDATE `quest_template` SET `PrevQuestId` = -8311 WHERE `entry` <=> 8353 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8354"} Chicken Clucking for a Mint
UPDATE `quest_template` SET `PrevQuestId` = -8312 WHERE `entry` <=> 8354 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8356"} Flexing for Nougat
UPDATE `quest_template` SET `PrevQuestId` = -8311 WHERE `entry` <=> 8356 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8357"} Dancing for Marzipan
UPDATE `quest_template` SET `PrevQuestId` = -8311 WHERE `entry` <=> 8357 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8358"} Incoming Gumdrop
UPDATE `quest_template` SET `PrevQuestId` = -8312 WHERE `entry` <=> 8358 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8359"} Flexing for Nougat
UPDATE `quest_template` SET `PrevQuestId` = -8312 WHERE `entry` <=> 8359 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8360"} Dancing for Marzipan
UPDATE `quest_template` SET `PrevQuestId` = -8312 WHERE `entry` <=> 8360 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8081"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8080 WHERE `entry` <=> 8081 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8124"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8123 WHERE `entry` <=> 8124 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8157"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8154 WHERE `entry` <=> 8157 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8158"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8155 WHERE `entry` <=> 8158 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8159"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8156 WHERE `entry` <=> 8159 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8163"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8160 WHERE `entry` <=> 8163 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8164"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8161 WHERE `entry` <=> 8164 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8165"} More Resource Crates
UPDATE `quest_template` SET `PrevQuestId` = 8162 WHERE `entry` <=> 8165 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9257"} Atiesh, Greatstaff of the Guardian
UPDATE `quest_template` SET `PrevQuestId` = 9251 WHERE `entry` <=> 9257 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "6622"} Triage
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 6622 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36363233' USING utf8mb4));
-- quest_template {"entry": "6624"} Triage
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 6624 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36363235' USING utf8mb4));
-- quest_template {"entry": "1795"} The Binding
UPDATE `quest_template` SET `PrevQuestId` = 1805 WHERE `entry` <=> 1795 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31383034' USING utf8mb4));
-- quest_template {"entry": "8732"} Field Duty Papers
UPDATE `quest_template` SET `PrevQuestId` = -8731 WHERE `entry` <=> 8732 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4381"} Crystal Restore
UPDATE `quest_template` SET `PrevQuestId` = 4321 WHERE `entry` <=> 4381 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4382"} Crystal Force
UPDATE `quest_template` SET `PrevQuestId` = 4321 WHERE `entry` <=> 4382 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4383"} Crystal Ward
UPDATE `quest_template` SET `PrevQuestId` = 4321 WHERE `entry` <=> 4383 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4384"} Crystal Yield
UPDATE `quest_template` SET `PrevQuestId` = 4321 WHERE `entry` <=> 4384 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4385"} Crystal Charge
UPDATE `quest_template` SET `PrevQuestId` = 4321 WHERE `entry` <=> 4385 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4386"} Crystal Spire
UPDATE `quest_template` SET `PrevQuestId` = 4321 WHERE `entry` <=> 4386 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "7636"} Stave of the Ancients
UPDATE `quest_template` SET `PrevQuestId` = 7633 WHERE `entry` <=> 7636 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "2478"} Mission: Possible But Not Probable
UPDATE `quest_template` SET `PrevQuestId` = 2458 WHERE `entry` <=> 2478 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "969"} Luck Be With You
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 969 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'36363036' USING utf8mb4));
-- quest_template {"entry": "4801"} Frostsaber E'ko
UPDATE `quest_template` SET `PrevQuestId` = 975 WHERE `entry` <=> 4801 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4802"} Winterfall E'ko
UPDATE `quest_template` SET `PrevQuestId` = 975 WHERE `entry` <=> 4802 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4803"} Shardtooth E'ko
UPDATE `quest_template` SET `PrevQuestId` = 975 WHERE `entry` <=> 4803 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4804"} Chillwind E'ko
UPDATE `quest_template` SET `PrevQuestId` = 975 WHERE `entry` <=> 4804 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4805"} Ice Thistle E'ko
UPDATE `quest_template` SET `PrevQuestId` = 975 WHERE `entry` <=> 4805 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4806"} Frostmaul E'ko
UPDATE `quest_template` SET `PrevQuestId` = 975 WHERE `entry` <=> 4806 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "4807"} Wildkin E'ko
UPDATE `quest_template` SET `PrevQuestId` = 975 WHERE `entry` <=> 4807 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5056"} Shy-Rotam
UPDATE `quest_template` SET `ReqSourceId1` = 12733, `ReqSourceCount1` = 1 WHERE `entry` <=> 5056 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "5057"} Past Endeavors
UPDATE `quest_template` SET `PrevQuestId` = 5056 WHERE `entry` <=> 5057 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8201"} A Collection of Heads
UPDATE `quest_template` SET `ReqSourceId1` = 19881, `ReqSourceCount1` = 5 WHERE `entry` <=> 8201 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3139383830' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'31' USING utf8mb4));
-- quest_template {"entry": "2770"} Gahz'rilla
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 2770 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'32373639' USING utf8mb4));
-- quest_template {"entry": "50006"} In need of Ol' Biggins. Again.
UPDATE `quest_template` SET `PrevQuestId` = 50005 WHERE `entry` <=> 50006 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "8799"} The Hero of the Day
UPDATE `quest_template` SET `PrevQuestId` = 8746 WHERE `entry` <=> 8799 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "50305"} Higher Stakes
UPDATE `quest_template` SET `PrevQuestId` = 815 WHERE `entry` <=> 50305 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "811"} Unity is Strength
UPDATE `quest_template` SET `PrevQuestId` = 810 WHERE `entry` <=> 811 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "820"} What Do You Rely On?
UPDATE `quest_template` SET `PrevQuestId` = 818 WHERE `entry` <=> 820 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60005"} [DEPRECATED] Once Upon a Sheep
UPDATE `quest_template` SET `RewItemId1` = 4241, `RewItemCount1` = 1 WHERE `entry` <=> 60005 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60007"} Lulu Looks Luscious to Lupines
UPDATE `quest_template` SET `RewItemId1` = 51221, `RewItemCount1` = 1 WHERE `entry` <=> 60007 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60008"} [DEPRECATED] Wool Would Work
UPDATE `quest_template` SET `PrevQuestId` = 60005, `RewItemId1` = 51224, `RewItemId2` = 51219, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 60008 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40371"} [DEPRECATED] A Diplomatic Mission
UPDATE `quest_template` SET `PrevQuestId` = 80256, `RewItemId1` = 65018, `RewItemCount1` = 1 WHERE `entry` <=> 40371 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40373"} [DEPRECATED] Appealing to the Alliance: Dwarves
UPDATE `quest_template` SET `PrevQuestId` = -40371, `RewItemId1` = 83017, `RewItemCount1` = 1 WHERE `entry` <=> 40373 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60130"} That Dirty Old Ring
UPDATE `quest_template` SET `RewItemId1` = 51856, `RewItemCount1` = 1 WHERE `entry` <=> 60130 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60072"} Trimming the Vilebranch
UPDATE `quest_template` SET `PrevQuestId` = 60071 WHERE `entry` <=> 60072 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60073"} Decimate Their Ranks
UPDATE `quest_template` SET `PrevQuestId` = 60072 WHERE `entry` <=> 60073 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60074"} Bring Down the Priestess!
UPDATE `quest_template` SET `PrevQuestId` = 60072 WHERE `entry` <=> 60074 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60107"} Cold Feet
UPDATE `quest_template` SET `PrevQuestId` = 375 WHERE `entry` <=> 60107 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60110"} Githyiss the Vile
UPDATE `quest_template` SET `PrevQuestId` = 3522 WHERE `entry` <=> 60110 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60111"} A Decisive Blow
UPDATE `quest_template` SET `PrevQuestId` = 487 WHERE `entry` <=> 60111 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60112"} Fallen Adventurers
UPDATE `quest_template` SET `PrevQuestId` = 376 WHERE `entry` <=> 60112 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60113"} Virulence
UPDATE `quest_template` SET `PrevQuestId` = 367 WHERE `entry` <=> 60113 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60116"} Paint the Roses Red
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 60116 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3630313137' USING utf8mb4));
-- quest_template {"entry": "60120"} [Deprecated] Triggered!
UPDATE `quest_template` SET `PrevQuestId` = 835 WHERE `entry` <=> 60120 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60121"} [Deprecated] The Mail Must Go Through
UPDATE `quest_template` SET `PrevQuestId` = 60120 WHERE `entry` <=> 60121 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60134"} Preventing Poison
UPDATE `quest_template` SET `PrevQuestId` = 60133 WHERE `entry` <=> 60134 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60145"} Down in the Ridge
UPDATE `quest_template` SET `PrevQuestId` = 15 WHERE `entry` <=> 60145 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60147"} Stalking the Felstalkers
UPDATE `quest_template` SET `PrevQuestId` = 792 WHERE `entry` <=> 60147 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60148"} Mother of the Hollow
UPDATE `quest_template` SET `PrevQuestId` = 380 WHERE `entry` <=> 60148 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60150"} Bristleback Aggression
UPDATE `quest_template` SET `PrevQuestId` = 750 WHERE `entry` <=> 60150 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80203"} Clearing Out Vermin
UPDATE `quest_template` SET `PrevQuestId` = 80200 WHERE `entry` <=> 80203 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80204"} [DEPRECATED] Gathering Intel
UPDATE `quest_template` SET `PrevQuestId` = 80203 WHERE `entry` <=> 80204 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80205"} Slaking Their Thirst
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80205 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80206"} Burnt Wheels
UPDATE `quest_template` SET `PrevQuestId` = 80205 WHERE `entry` <=> 80206 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80207"} Dark Iron Scrapping
UPDATE `quest_template` SET `PrevQuestId` = 80206 WHERE `entry` <=> 80207 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80208"} Sunblade Reunion
UPDATE `quest_template` SET `PrevQuestId` = 80207 WHERE `entry` <=> 80208 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80209"} Porting to Goldshire
UPDATE `quest_template` SET `PrevQuestId` = 80208 WHERE `entry` <=> 80209 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80210"} Providing a Balanced Diet
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80210 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80211"} Ranger's Letter
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80211 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80212"} Paladin's Letter
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80212 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80213"} Priest's Letter
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80213 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80214"} Magister's Letter
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80214 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80215"} Shady Letter
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80215 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80216"} Swordsman's Letter
UPDATE `quest_template` SET `PrevQuestId` = 80201 WHERE `entry` <=> 80216 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80106"} The Grumbling Grove
UPDATE `quest_template` SET `PrevQuestId` = 80105 WHERE `entry` <=> 80106 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80252"} [DEPRECATED] A Crystal Clear Task
UPDATE `quest_template` SET `PrevQuestId` = 80256 WHERE `entry` <=> 80252 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80253"} [DEPRECATED] Relics in Feralas
UPDATE `quest_template` SET `PrevQuestId` = 80256 WHERE `entry` <=> 80253 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80254"} [DEPRECATED] Smashing Zul'Mashar
UPDATE `quest_template` SET `PrevQuestId` = 80256 WHERE `entry` <=> 80254 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80256"} [DEPRECATED] Welcome to Alah'Thalas
UPDATE `quest_template` SET `PrevQuestId` = 80251 WHERE `entry` <=> 80256 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80115"} Grease-stained Letter
UPDATE `quest_template` SET `PrevQuestId` = 80100 WHERE `entry` <=> 80115 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80116"} Ooze-covered Letter
UPDATE `quest_template` SET `PrevQuestId` = 80100 WHERE `entry` <=> 80116 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80117"} Leather-covered Letter
UPDATE `quest_template` SET `PrevQuestId` = 80100 WHERE `entry` <=> 80117 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80118"} Fancy Letter
UPDATE `quest_template` SET `PrevQuestId` = 80100 WHERE `entry` <=> 80118 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80119"} Awful-looking Letter
UPDATE `quest_template` SET `PrevQuestId` = 80100 WHERE `entry` <=> 80119 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40425"} The Tome of Arcane Intricacies and Magical Phenomenon IX
UPDATE `quest_template` SET `RewItemId1` = 60622, `RewItemCount1` = 1 WHERE `entry` <=> 40425 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80301"} Lighting the Pyres
UPDATE `quest_template` SET `PrevQuestId` = 80313, `ReqCreatureOrGOId1` = -3000242 WHERE `entry` <=> 80301 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "80303"} Raiding Quel'Lithien Lodge
UPDATE `quest_template` SET `PrevQuestId` = 80313 WHERE `entry` <=> 80303 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80304"} The Means To Heal
UPDATE `quest_template` SET `PrevQuestId` = 80313 WHERE `entry` <=> 80304 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40396"} Captain Grayson's Revenge
UPDATE `quest_template` SET `RewItemId1` = 70070, `RewItemCount1` = 1 WHERE `entry` <=> 40396 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40392"} [DEPRECATED] Summons by the Ranger-General
UPDATE `quest_template` SET `PrevQuestId` = 40371 WHERE `entry` <=> 40392 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80258"} [DEPRECATED] Tears of the Poppy
UPDATE `quest_template` SET `PrevQuestId` = 80256 WHERE `entry` <=> 80258 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80260"} [DEPRECATED] Help With a Compassionate Matter
UPDATE `quest_template` SET `PrevQuestId` = 80256 WHERE `entry` <=> 80260 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60152"} They Call Him Morgan the Storm
UPDATE `quest_template` SET `PrevQuestId` = 604 WHERE `entry` <=> 60152 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "60153"} The Iron Patch
UPDATE `quest_template` SET `PrevQuestId` = 60152 WHERE `entry` <=> 60153 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80310"} Quark's Justice
UPDATE `quest_template` SET `PrevQuestId` = 80309 WHERE `entry` <=> 80310 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80312"} Arcane Arms
UPDATE `quest_template` SET `PrevQuestId` = 80311 WHERE `entry` <=> 80312 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80321"} [Deprecated] The Lies that Bind Us
UPDATE `quest_template` SET `PrevQuestId` = 80320 WHERE `entry` <=> 80321 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80322"} [Deprecated] The Lies That Bind Us
UPDATE `quest_template` SET `PrevQuestId` = 80321 WHERE `entry` <=> 80322 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80323"} [Deprecated] The Lies That Bind Us
UPDATE `quest_template` SET `PrevQuestId` = 80322 WHERE `entry` <=> 80323 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80306"} What's Yours is Ours
UPDATE `quest_template` SET `PrevQuestId` = 80305 WHERE `entry` <=> 80306 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80307"} Yet Another Smoke Cloud Above Orgrimmar
UPDATE `quest_template` SET `PrevQuestId` = 80306 WHERE `entry` <=> 80307 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80308"} Tobacco Thieves
UPDATE `quest_template` SET `PrevQuestId` = 80307 WHERE `entry` <=> 80308 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80309"} Odo's Gut Feeling
UPDATE `quest_template` SET `PrevQuestId` = 80308 WHERE `entry` <=> 80309 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80331"} [DEPRECATED] Taming the Beast
UPDATE `quest_template` SET `PrevQuestId` = 80330 WHERE `entry` <=> 80331 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80350"} Etched Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 80350 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80360"} Power Taken, Power Given
UPDATE `quest_template` SET `PrevQuestId` = 80313 WHERE `entry` <=> 80360 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80374"} Additional Runecloth: Silvermoon Remnants
UPDATE `quest_template` SET `PrevQuestId` = 80373 WHERE `entry` <=> 80374 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80379"} Additional Runecloth: Revantusk Tribe
UPDATE `quest_template` SET `PrevQuestId` = 80378 WHERE `entry` <=> 80379 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80381"} Shellcoins
UPDATE `quest_template` SET `SpecialFlags` = 1024 WHERE `entry` <=> 80381 AND (BINARY CAST(`SpecialFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80393"} Errand for Quel'Danil
UPDATE `quest_template` SET `PrevQuestId` = 80392 WHERE `entry` <=> 80393 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80401"} To Build a Pounder
UPDATE `quest_template` SET `PrevQuestId` = 80398 WHERE `entry` <=> 80401 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80404"} Delivery for Drazzit
UPDATE `quest_template` SET `PrevQuestId` = 80405 WHERE `entry` <=> 80404 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80405"} Secure the Cargo!
UPDATE `quest_template` SET `PrevQuestId` = 80403 WHERE `entry` <=> 80405 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80407"} A Letter From a Friend
UPDATE `quest_template` SET `PrevQuestId` = 80396 WHERE `entry` <=> 80407 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80369"} A Warm Feeling
UPDATE `quest_template` SET `PrevQuestId` = 8469 WHERE `entry` <=> 80369 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80410"} A Timely Situation
UPDATE `quest_template` SET `PrevQuestId` = 80409 WHERE `entry` <=> 80410 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70033"} The Seeker's Demise
UPDATE `quest_template` SET `PrevQuestId` = 70032 WHERE `entry` <=> 70033 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70034"} Hawk's Beak
UPDATE `quest_template` SET `PrevQuestId` = 70022 WHERE `entry` <=> 70034 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70001"} Lost to the Sands
UPDATE `quest_template` SET `PrevQuestId` = 70000 WHERE `entry` <=> 70001 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70002"} The Guidance of a Mad Man
UPDATE `quest_template` SET `PrevQuestId` = 7583 WHERE `entry` <=> 70002 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70003"} Draining the Soulflayer
UPDATE `quest_template` SET `PrevQuestId` = 70002 WHERE `entry` <=> 70003 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70049"} Pitch a Tent
UPDATE `quest_template` SET `PrevQuestId` = 70050 WHERE `entry` <=> 70049 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40390"} Aggression's End
UPDATE `quest_template` SET `RewChoiceItemId1` = 60522, `RewChoiceItemId2` = 60523, `RewChoiceItemId3` = 60524, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40390 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70056"} Report to Darkshire
UPDATE `quest_template` SET `PrevQuestId` = 70055 WHERE `entry` <=> 70056 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70057"} A Simple Memory
UPDATE `quest_template` SET `PrevQuestId` = 70055 WHERE `entry` <=> 70057 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "70058"} Law & Ogre
UPDATE `quest_template` SET `PrevQuestId` = 70056 WHERE `entry` <=> 70058 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40003"} Hookah for Your Troubles
UPDATE `quest_template` SET `PrevQuestId` = 40002 WHERE `entry` <=> 40003 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40299"} Shoulderguards of the Lightbringer
UPDATE `quest_template` SET `PrevQuestId` = 5047 WHERE `entry` <=> 40299 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40035"} Spoils Left Behind
UPDATE `quest_template` SET `PrevQuestId` = 40034 WHERE `entry` <=> 40035 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80605"} The First Opening of The Dark Portal
UPDATE `quest_template` SET `PrevQuestId` = 80604 WHERE `entry` <=> 80605 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40270"} The Maul'ogg Crisis VII
UPDATE `quest_template` SET `PrevQuestId` = 40269 WHERE `entry` <=> 40270 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40271"} The Maul'ogg Crisis VIII
UPDATE `quest_template` SET `PrevQuestId` = 40270 WHERE `entry` <=> 40271 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40357"} Lift the Anchor!
UPDATE `quest_template` SET `PrevQuestId` = 40356 WHERE `entry` <=> 40357 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55201"} Retaking The Roadways
UPDATE `quest_template` SET `RewItemId1` = 81402, `RewItemCount1` = 1 WHERE `entry` <=> 55201 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55205"} Report From Snowbeard
UPDATE `quest_template` SET `PrevQuestId` = 55204 WHERE `entry` <=> 55205 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55206"} Hunting Dragonmaw
UPDATE `quest_template` SET `PrevQuestId` = 55205 WHERE `entry` <=> 55206 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55208"} The Pie Delivery
UPDATE `quest_template` SET `PrevQuestId` = 55207 WHERE `entry` <=> 55208 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55210"} Tilting Oscilation Inhibitor?!
UPDATE `quest_template` SET `PrevQuestId` = 55209 WHERE `entry` <=> 55210 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55213"} Supplies for Barthos
UPDATE `quest_template` SET `PrevQuestId` = 55212 WHERE `entry` <=> 55213 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55211"} Oscilation Inhibited Disk!
UPDATE `quest_template` SET `PrevQuestId` = 55210, `RewItemId1` = 81411, `RewItemCount1` = 1 WHERE `entry` <=> 55211 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40114"} Uldum Awaits
UPDATE `quest_template` SET `PrevQuestId` = 40113 WHERE `entry` <=> 40114 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55006"} Backup Capacitor
UPDATE `quest_template` SET `PrevQuestId` = 55003 WHERE `entry` <=> 55006 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55008"} Frix's Folly
UPDATE `quest_template` SET `PrevQuestId` = 55007 WHERE `entry` <=> 55008 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55009"} Empty Handed
UPDATE `quest_template` SET `PrevQuestId` = 55008 WHERE `entry` <=> 55009 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55010"} Grabbing the Gear
UPDATE `quest_template` SET `PrevQuestId` = 55009 WHERE `entry` <=> 55010 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55012"} [Deprecated] Short Five Minute Adventure
UPDATE `quest_template` SET `PrevQuestId` = 55011 WHERE `entry` <=> 55012 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55013"} [Deprecated] Shaking a Cold Arm
UPDATE `quest_template` SET `PrevQuestId` = 55012 WHERE `entry` <=> 55013 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55016"} The Skystormer Project
UPDATE `quest_template` SET `PrevQuestId` = 55015 WHERE `entry` <=> 55016 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55015"} The Eldarath Project
UPDATE `quest_template` SET `PrevQuestId` = 55010 WHERE `entry` <=> 55015 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55014"} The Ethereal Project
UPDATE `quest_template` SET `PrevQuestId` = 55010 WHERE `entry` <=> 55014 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55017"} Report to Sparkwater!
UPDATE `quest_template` SET `PrevQuestId` = 55016 WHERE `entry` <=> 55017 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55018"} The Big Energy Project
UPDATE `quest_template` SET `PrevQuestId` = 55016 WHERE `entry` <=> 55018 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55020"} Misgotten Honor
UPDATE `quest_template` SET `PrevQuestId` = 55019 WHERE `entry` <=> 55020 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55021"} Beyond the Walls
UPDATE `quest_template` SET `PrevQuestId` = 55020 WHERE `entry` <=> 55021 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55022"} Taking Leave
UPDATE `quest_template` SET `PrevQuestId` = 55021 WHERE `entry` <=> 55022 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55023"} Relief and Reprise
UPDATE `quest_template` SET `PrevQuestId` = 55022 WHERE `entry` <=> 55023 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55027"} Employed Help
UPDATE `quest_template` SET `PrevQuestId` = 55026 WHERE `entry` <=> 55027 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55028"} The "Hidden" Crew
UPDATE `quest_template` SET `PrevQuestId` = 55027 WHERE `entry` <=> 55028 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55029"} In Need of Information
UPDATE `quest_template` SET `PrevQuestId` = 55028 WHERE `entry` <=> 55029 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55030"} Acquiring Information
UPDATE `quest_template` SET `PrevQuestId` = 55029 WHERE `entry` <=> 55030 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55031"} Smash Salt Tooth!
UPDATE `quest_template` SET `PrevQuestId` = 55030 WHERE `entry` <=> 55031 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55051"} Return to Port!
UPDATE `quest_template` SET `PrevQuestId` = 55031 WHERE `entry` <=> 55051 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40542"} Loneliest Murloc in the World
UPDATE `quest_template` SET `PrevQuestId` = 40541 WHERE `entry` <=> 40542 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55035"} Hidden-Hidden Reward
UPDATE `quest_template` SET `PrevQuestId` = 40681, `RewItemId1` = 81341, `RewItemCount1` = 1 WHERE `entry` <=> 55035 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55037"} Filling Back-Orders
UPDATE `quest_template` SET `PrevQuestId` = 55036 WHERE `entry` <=> 55037 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55038"} A Late Delivery
UPDATE `quest_template` SET `PrevQuestId` = 55037 WHERE `entry` <=> 55038 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55040"} Returning Property
UPDATE `quest_template` SET `PrevQuestId` = 55039, `RewItemId1` = 81342, `RewItemCount1` = 1 WHERE `entry` <=> 55040 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55045"} Ore on Time
UPDATE `quest_template` SET `PrevQuestId` = 55044 WHERE `entry` <=> 55045 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55050"} Grizlik's Wish
UPDATE `quest_template` SET `RewItemId1` = 81349, `RewItemCount1` = 1 WHERE `entry` <=> 55050 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "55300"} Imperial Plate Gauntlets
UPDATE `quest_template` SET `PrevQuestId` = 7652 WHERE `entry` <=> 55300 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40124"} Interfering Naga
UPDATE `quest_template` SET `PrevQuestId` = 40123 WHERE `entry` <=> 40124 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40121"} Alpha Aggression
UPDATE `quest_template` SET `PrevQuestId` = 40120 WHERE `entry` <=> 40121 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40247"} Staff of Eldara
UPDATE `quest_template` SET `PrevQuestId` = 40246 WHERE `entry` <=> 40247 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40246"} Aged Deep-Rod
UPDATE `quest_template` SET `PrevQuestId` = 40245 WHERE `entry` <=> 40246 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40245"} The Eldarath Harmonization Gem
UPDATE `quest_template` SET `PrevQuestId` = 40244 WHERE `entry` <=> 40245 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40244"} The Keeper's Possession
UPDATE `quest_template` SET `PrevQuestId` = 40243 WHERE `entry` <=> 40244 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40242"} Shell Shields
UPDATE `quest_template` SET `PrevQuestId` = 40041 WHERE `entry` <=> 40242 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40214"} Uncovering Evil
UPDATE `quest_template` SET `PrevQuestId` = 40213 WHERE `entry` <=> 40214 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40251"} Necklace of Insomnius
UPDATE `quest_template` SET `PrevQuestId` = 40212 WHERE `entry` <=> 40251 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40127"} The Dampening Must End
UPDATE `quest_template` SET `PrevQuestId` = 40126 WHERE `entry` <=> 40127 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40219"} The Jade Mine
UPDATE `quest_template` SET `PrevQuestId` = 40199 WHERE `entry` <=> 40219 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40220"} Batskin Letter
UPDATE `quest_template` SET `PrevQuestId` = 364 WHERE `entry` <=> 40220 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40231"} The Forgotten Tome
UPDATE `quest_template` SET `PrevQuestId` = 40095 WHERE `entry` <=> 40231 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40232"} Returning to Janathos
UPDATE `quest_template` SET `PrevQuestId` = 40231 WHERE `entry` <=> 40232 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40233"} The Orb of Kaladus
UPDATE `quest_template` SET `PrevQuestId` = 40232 WHERE `entry` <=> 40233 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40213"} The Hermit's Wrath
UPDATE `quest_template` SET `PrevQuestId` = 40251 WHERE `entry` <=> 40213 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40223"} Breaking Contracts
UPDATE `quest_template` SET `PrevQuestId` = 40222 WHERE `entry` <=> 40223 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40224"} Delivery for Oilfuse
UPDATE `quest_template` SET `PrevQuestId` = 40223 WHERE `entry` <=> 40224 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40225"} The Old Bonds
UPDATE `quest_template` SET `PrevQuestId` = 40224 WHERE `entry` <=> 40225 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40226"} Wally Wisecrack
UPDATE `quest_template` SET `PrevQuestId` = 40225 WHERE `entry` <=> 40226 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80721"} Grim News
UPDATE `quest_template` SET `PrevQuestId` = 80720 WHERE `entry` <=> 80721 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80722"} To Catch a Rat...
UPDATE `quest_template` SET `PrevQuestId` = 80721 WHERE `entry` <=> 80722 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40211"} Jadefire Revenge
UPDATE `quest_template` SET `PrevQuestId` = 40210 WHERE `entry` <=> 40211 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40212"} An Old Friend
UPDATE `quest_template` SET `PrevQuestId` = 40211 WHERE `entry` <=> 40212 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40195"} The Razzari Leaders
UPDATE `quest_template` SET `PrevQuestId` = 40194 WHERE `entry` <=> 40195 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40193"} Ashenvale Corruption
UPDATE `quest_template` SET `PrevQuestId` = 40192 WHERE `entry` <=> 40193 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40185"} Magical Compass
UPDATE `quest_template` SET `PrevQuestId` = 40182 WHERE `entry` <=> 40185 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40186"} Land ho? More like land no
UPDATE `quest_template` SET `PrevQuestId` = 40182 WHERE `entry` <=> 40186 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40187"} Captain of the Bloodsail Buccaneers
UPDATE `quest_template` SET `PrevQuestId` = 40186 WHERE `entry` <=> 40187 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40189"} To Make A Fortune
UPDATE `quest_template` SET `PrevQuestId` = 40188 WHERE `entry` <=> 40189 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40190"} The Blazno Touch
UPDATE `quest_template` SET `PrevQuestId` = 40189 WHERE `entry` <=> 40190 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40183"} First Mate McGillicuddy
UPDATE `quest_template` SET `PrevQuestId` = 40182 WHERE `entry` <=> 40183 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40182"} South Sea Losers
UPDATE `quest_template` SET `PrevQuestId` = 40181 WHERE `entry` <=> 40182 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40181"} Taking over Faldir's Cove
UPDATE `quest_template` SET `PrevQuestId` = 40180 WHERE `entry` <=> 40181 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40180"} Making a Statement
UPDATE `quest_template` SET `PrevQuestId` = 40179 WHERE `entry` <=> 40180 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40179"} Exterminate the Rat
UPDATE `quest_template` SET `PrevQuestId` = 40178 WHERE `entry` <=> 40179 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40178"} Loss of Good Rum
UPDATE `quest_template` SET `PrevQuestId` = 40174, `ReqCreatureOrGOId1` = -2010836 WHERE `entry` <=> 40178 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "40175"} The Bet
UPDATE `quest_template` SET `ReqSourceId1` = 60246 WHERE `entry` <=> 40175 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3630323437' USING utf8mb4));
-- quest_template {"entry": "40173"} Weapons Laying About
UPDATE `quest_template` SET `PrevQuestId` = 40172 WHERE `entry` <=> 40173 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40174"} A Cannon's Misfortune
UPDATE `quest_template` SET `PrevQuestId` = 40173 WHERE `entry` <=> 40174 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40171"} The Tower of Lapidis IX
UPDATE `quest_template` SET `PrevQuestId` = 40170 WHERE `entry` <=> 40171 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40170"} The Tower of Lapidis VIII
UPDATE `quest_template` SET `PrevQuestId` = 40169 WHERE `entry` <=> 40170 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40169"} The Tower of Lapidis VII
UPDATE `quest_template` SET `PrevQuestId` = 40168 WHERE `entry` <=> 40169 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40168"} The Tower of Lapidis VI
UPDATE `quest_template` SET `PrevQuestId` = 40167 WHERE `entry` <=> 40168 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40166"} The Tower of Lapidis IV
UPDATE `quest_template` SET `PrevQuestId` = 40165 WHERE `entry` <=> 40166 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40167"} The Tower of Lapidis V
UPDATE `quest_template` SET `PrevQuestId` = 40166 WHERE `entry` <=> 40167 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40165"} The Tower of Lapidis III
UPDATE `quest_template` SET `PrevQuestId` = 40164 WHERE `entry` <=> 40165 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40164"} The Tower of Lapidis II
UPDATE `quest_template` SET `PrevQuestId` = 40163 WHERE `entry` <=> 40164 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40159"} Rumors of the Great Fire Plume
UPDATE `quest_template` SET `PrevQuestId` = 40158 WHERE `entry` <=> 40159 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40156"} The Tyrant King
UPDATE `quest_template` SET `PrevQuestId` = 40155 WHERE `entry` <=> 40156 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40147"} Rooting Out Evil
UPDATE `quest_template` SET `PrevQuestId` = 40146 WHERE `entry` <=> 40147 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40146"} Feran's Report
UPDATE `quest_template` SET `PrevQuestId` = 40145 WHERE `entry` <=> 40146 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40144"} Southsea Interference
UPDATE `quest_template` SET `PrevQuestId` = 40143 WHERE `entry` <=> 40144 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40140"} Cleaning The Farm
UPDATE `quest_template` SET `PrevQuestId` = 40139 WHERE `entry` <=> 40140 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40252"} The Way-Stones of Old
UPDATE `quest_template` SET `PrevQuestId` = 40247 WHERE `entry` <=> 40252 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40137"} Strike Them Down
UPDATE `quest_template` SET `PrevQuestId` = 40136 WHERE `entry` <=> 40137 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40133"} The Profitable Activation
UPDATE `quest_template` SET `PrevQuestId` = 40131 WHERE `entry` <=> 40133 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40132"} [DEPRECATED] The Activation
UPDATE `quest_template` SET `PrevQuestId` = 40129 WHERE `entry` <=> 40132 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40131"} Requisitioning a Core
UPDATE `quest_template` SET `PrevQuestId` = 40130 WHERE `entry` <=> 40131 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40129"} [DEPRECATED] Stealing a Core
UPDATE `quest_template` SET `PrevQuestId` = 40128 WHERE `entry` <=> 40129 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40253"} Restoring the Ley Lines
UPDATE `quest_template` SET `PrevQuestId` = 40252 WHERE `entry` <=> 40253 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40254"} Keeping Secrets
UPDATE `quest_template` SET `PrevQuestId` = 40253 WHERE `entry` <=> 40254 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40075"} Swords to Sorrowguard
UPDATE `quest_template` SET `PrevQuestId` = 40074 WHERE `entry` <=> 40075 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40074"} A Delivery of Swords
UPDATE `quest_template` SET `PrevQuestId` = 40073 WHERE `entry` <=> 40074 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40072"} Areyntall Family Value
UPDATE `quest_template` SET `PrevQuestId` = 40071 WHERE `entry` <=> 40072 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40259"} The Dark-Rune Anvil
UPDATE `quest_template` SET `RewItemId1` = 81060, `RewItemCount1` = 1 WHERE `entry` <=> 40259 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40041"} Flaxwhisker Loyalty
UPDATE `quest_template` SET `PrevQuestId` = 40040 WHERE `entry` <=> 40041 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40042"} Delivery for Bromley
UPDATE `quest_template` SET `PrevQuestId` = 40041 WHERE `entry` <=> 40042 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40043"} The Missing Robot
UPDATE `quest_template` SET `PrevQuestId` = 40042 WHERE `entry` <=> 40043 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40044"} The Analyzation Chip
UPDATE `quest_template` SET `PrevQuestId` = 40043 WHERE `entry` <=> 40044 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40045"} Lorie's Logbook
UPDATE `quest_template` SET `PrevQuestId` = 40044 WHERE `entry` <=> 40045 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40046"} The Flight Logbook
UPDATE `quest_template` SET `PrevQuestId` = 40045 WHERE `entry` <=> 40046 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40047"} Report for Gnomeregan!
UPDATE `quest_template` SET `PrevQuestId` = 40046 WHERE `entry` <=> 40047 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40057"} Reclaiming the Tinkerspark Transponder
UPDATE `quest_template` SET `PrevQuestId` = 40048 WHERE `entry` <=> 40057 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40032"} Draenic Communication
UPDATE `quest_template` SET `PrevQuestId` = 40027 WHERE `entry` <=> 40032 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40376"} [DEPRECATED] The Kaldorei Problem
UPDATE `quest_template` SET `PrevQuestId` = 40371 WHERE `entry` <=> 40376 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40068"} Wobble-free Fizz Gear
UPDATE `quest_template` SET `PrevQuestId` = 40067 WHERE `entry` <=> 40068 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40067"} Fendo's Spanner
UPDATE `quest_template` SET `PrevQuestId` = 40066 WHERE `entry` <=> 40067 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40066"} Repair the Fizz Disk
UPDATE `quest_template` SET `PrevQuestId` = 40065 WHERE `entry` <=> 40066 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40065"} The Turbocharged Wobble-free Fizz Disk
UPDATE `quest_template` SET `PrevQuestId` = 40041 WHERE `entry` <=> 40065 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40064"} The Gyronautical Compass
UPDATE `quest_template` SET `PrevQuestId` = 40063 WHERE `entry` <=> 40064 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40062"} An Azure Scale for Gnomeregan!
UPDATE `quest_template` SET `PrevQuestId` = 40041 WHERE `entry` <=> 40062 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40063"} In Need of Sprugbolts
UPDATE `quest_template` SET `PrevQuestId` = 40041 WHERE `entry` <=> 40063 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40061"} Killing the Tide Lord
UPDATE `quest_template` SET `PrevQuestId` = 40039 WHERE `entry` <=> 40061 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40048"} Report from Gnomeregan!
UPDATE `quest_template` SET `PrevQuestId` = 40047 WHERE `entry` <=> 40048 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40056"} The Scroll of Cow Portal
UPDATE `quest_template` SET `PrevQuestId` = 40055 WHERE `entry` <=> 40056 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40055"} Mysterious Cow Scroll V
UPDATE `quest_template` SET `PrevQuestId` = 40054 WHERE `entry` <=> 40055 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40054"} Mysterious Cow Scroll IV
UPDATE `quest_template` SET `PrevQuestId` = 40053 WHERE `entry` <=> 40054 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40053"} Mysterious Cow Scroll III
UPDATE `quest_template` SET `PrevQuestId` = 40052 WHERE `entry` <=> 40053 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40052"} Mysterious Cow Scroll II
UPDATE `quest_template` SET `PrevQuestId` = 40051 WHERE `entry` <=> 40052 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40039"} The Rethress Tide Scepter
UPDATE `quest_template` SET `PrevQuestId` = 40038 WHERE `entry` <=> 40039 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40038"} Keeping Command
UPDATE `quest_template` SET `PrevQuestId` = 40037 WHERE `entry` <=> 40038 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40037"} The Storm Bay Mutiny
UPDATE `quest_template` SET `PrevQuestId` = 40034 WHERE `entry` <=> 40037 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40036"} A Broken Oath
UPDATE `quest_template` SET `PrevQuestId` = 40034 WHERE `entry` <=> 40036 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40033"} Finding Akh Z'ador
UPDATE `quest_template` SET `PrevQuestId` = 40032 WHERE `entry` <=> 40033 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40030"} Noboru the Cudgel
UPDATE `quest_template` SET `PrevQuestId` = 40029 WHERE `entry` <=> 40030 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40026"} Lord Rog's Favor
UPDATE `quest_template` SET `PrevQuestId` = 40025 WHERE `entry` <=> 40026 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40025"} Inner Binding Purification
UPDATE `quest_template` SET `PrevQuestId` = 40024 WHERE `entry` <=> 40025 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40024"} Lord Rog's Exiles
UPDATE `quest_template` SET `PrevQuestId` = 40023 WHERE `entry` <=> 40024 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40389"} Zalazane's Fall
UPDATE `quest_template` SET `PrevQuestId` = 826 WHERE `entry` <=> 40389 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40022"} The Witherbark Warband Leader
UPDATE `quest_template` SET `PrevQuestId` = 40021 WHERE `entry` <=> 40022 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40394"} [CANCELLED] Summons by the Alliance
UPDATE `quest_template` SET `PrevQuestId` = 40384 WHERE `entry` <=> 40394 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40115"} Guardian of the Gate
UPDATE `quest_template` SET `PrevQuestId` = 40114 WHERE `entry` <=> 40115 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40113"} Might of the Horde
UPDATE `quest_template` SET `PrevQuestId` = 40112 WHERE `entry` <=> 40113 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40112"} Disturbing Silence
UPDATE `quest_template` SET `PrevQuestId` = 40111 WHERE `entry` <=> 40112 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40111"} In a Rush
UPDATE `quest_template` SET `PrevQuestId` = 40110 WHERE `entry` <=> 40111 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40110"} Unseen Enemy
UPDATE `quest_template` SET `PrevQuestId` = 40109 WHERE `entry` <=> 40110 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40109"} Scars of the Past
UPDATE `quest_template` SET `PrevQuestId` = 40108 WHERE `entry` <=> 40109 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40107"} Gate Keeper
UPDATE `quest_template` SET `PrevQuestId` = 40106 WHERE `entry` <=> 40107 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40106"} Gates of Uldum
UPDATE `quest_template` SET `PrevQuestId` = 40105 WHERE `entry` <=> 40106 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40105"} Pride of the Dwarves
UPDATE `quest_template` SET `PrevQuestId` = 40104 WHERE `entry` <=> 40105 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40104"} The Missing Expedition
UPDATE `quest_template` SET `PrevQuestId` = 40103 WHERE `entry` <=> 40104 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40103"} The Race
UPDATE `quest_template` SET `PrevQuestId` = 40102 WHERE `entry` <=> 40103 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40102"} What Lurks in the Dark
UPDATE `quest_template` SET `PrevQuestId` = 40101 WHERE `entry` <=> 40102 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40101"} Original Owner
UPDATE `quest_template` SET `PrevQuestId` = 40100 WHERE `entry` <=> 40101 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40015"} The Hunt for Heketh
UPDATE `quest_template` SET `PrevQuestId` = 40014 WHERE `entry` <=> 40015 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40126"} Discovering the Source
UPDATE `quest_template` SET `PrevQuestId` = 40125 WHERE `entry` <=> 40126 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40125"} Out of Options
UPDATE `quest_template` SET `PrevQuestId` = 40124 WHERE `entry` <=> 40125 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40123"} The Dampening Mystery
UPDATE `quest_template` SET `PrevQuestId` = 40122 WHERE `entry` <=> 40123 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40263"} A Demonstration of Skill
UPDATE `quest_template` SET `PrevQuestId` = 40262 WHERE `entry` <=> 40263 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40265"} The Maul'ogg Crisis II
UPDATE `quest_template` SET `PrevQuestId` = 40264 WHERE `entry` <=> 40265 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40266"} The Maul'ogg Crisis III
UPDATE `quest_template` SET `PrevQuestId` = 40265 WHERE `entry` <=> 40266 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40267"} The Maul'ogg Crisis IV
UPDATE `quest_template` SET `PrevQuestId` = 40266 WHERE `entry` <=> 40267 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40268"} The Maul'ogg Crisis V
UPDATE `quest_template` SET `PrevQuestId` = 40267 WHERE `entry` <=> 40268 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40269"} The Maul'ogg Crisis VI
UPDATE `quest_template` SET `PrevQuestId` = 40268 WHERE `entry` <=> 40269 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40272"} The Maul'ogg Crisis IX
UPDATE `quest_template` SET `PrevQuestId` = 40271, `RewChoiceItemId1` = 60346, `RewChoiceItemId2` = 60347, `RewChoiceItemId3` = 60348, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40272 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40276"} Destroying the Deeptide
UPDATE `quest_template` SET `PrevQuestId` = 40275 WHERE `entry` <=> 40276 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40082"} Pestering the Pesterhide
UPDATE `quest_template` SET `PrevQuestId` = 40081 WHERE `entry` <=> 40082 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40084"} Magical Interference
UPDATE `quest_template` SET `PrevQuestId` = 40083 WHERE `entry` <=> 40084 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40085"} Stealing Arcane Goods
UPDATE `quest_template` SET `PrevQuestId` = 40083 WHERE `entry` <=> 40085 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40088"} Head of the Pack
UPDATE `quest_template` SET `PrevQuestId` = 40087 WHERE `entry` <=> 40088 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40097"} In the Dark
UPDATE `quest_template` SET `PrevQuestId` = 40096 WHERE `entry` <=> 40097 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40098"} Rightful Heir
UPDATE `quest_template` SET `PrevQuestId` = 40097 WHERE `entry` <=> 40098 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40099"} Attack from the Inside
UPDATE `quest_template` SET `PrevQuestId` = 40098 WHERE `entry` <=> 40099 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40277"} Fall Of The Usurper
UPDATE `quest_template` SET `PrevQuestId` = 40099 WHERE `entry` <=> 40277 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40279"} A Different Type of Lock
UPDATE `quest_template` SET `PrevQuestId` = 40278 WHERE `entry` <=> 40279 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40280"} Ways of Magic
UPDATE `quest_template` SET `PrevQuestId` = 40279 WHERE `entry` <=> 40280 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40281"} Into The Jaws
UPDATE `quest_template` SET `PrevQuestId` = 40280 WHERE `entry` <=> 40281 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40282"} Darlthos Legacy
UPDATE `quest_template` SET `PrevQuestId` = 40281 WHERE `entry` <=> 40282 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40830"} Scouring Greyshire
UPDATE `quest_template` SET `RewItemId1` = 61259, `RewItemCount1` = 1 WHERE `entry` <=> 40830 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40286"} The Search for the Blade
UPDATE `quest_template` SET `PrevQuestId` = 40285 WHERE `entry` <=> 40286 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40287"} The Search for the Edge
UPDATE `quest_template` SET `PrevQuestId` = 40286 WHERE `entry` <=> 40287 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40288"} Refueling the Blade
UPDATE `quest_template` SET `PrevQuestId` = 40287 WHERE `entry` <=> 40288 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40289"} To Honor the Forgotten
UPDATE `quest_template` SET `PrevQuestId` = 40288 WHERE `entry` <=> 40289 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40291"} Claws for Bloodfist Point
UPDATE `quest_template` SET `PrevQuestId` = 40290 WHERE `entry` <=> 40291 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40293"} Ritual of the Farseer
UPDATE `quest_template` SET `PrevQuestId` = 40292 WHERE `entry` <=> 40293 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40294"} Dragons of Azshara?
UPDATE `quest_template` SET `PrevQuestId` = 40293 WHERE `entry` <=> 40294 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40295"} Mystery of Lake Mennar
UPDATE `quest_template` SET `PrevQuestId` = 40294 WHERE `entry` <=> 40295 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40296"} Report to the Archdruid
UPDATE `quest_template` SET `PrevQuestId` = 40295 WHERE `entry` <=> 40296 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40297"} Stop the Dragonflight
UPDATE `quest_template` SET `PrevQuestId` = 40296 WHERE `entry` <=> 40297 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40302"} Torta's Egg
UPDATE `quest_template` SET `RewItemId1` = 23720, `RewItemCount1` = 1 WHERE `entry` <=> 40302 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40303"} The Tower of Lapidis X
UPDATE `quest_template` SET `PrevQuestId` = 40171 WHERE `entry` <=> 40303 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40305"} The Depths of Karazhan II
UPDATE `quest_template` SET `PrevQuestId` = 40304 WHERE `entry` <=> 40305 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40306"} The Depths of Karazhan III
UPDATE `quest_template` SET `PrevQuestId` = 40305 WHERE `entry` <=> 40306 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40307"} The Depths of Karazhan IV
UPDATE `quest_template` SET `PrevQuestId` = 40306 WHERE `entry` <=> 40307 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40308"} The Depths of Karazhan V
UPDATE `quest_template` SET `PrevQuestId` = 40307 WHERE `entry` <=> 40308 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40309"} The Depths of Karazhan VI
UPDATE `quest_template` SET `PrevQuestId` = 40308 WHERE `entry` <=> 40309 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40310"} The Depths of Karazhan VII
UPDATE `quest_template` SET `PrevQuestId` = 40309 WHERE `entry` <=> 40310 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40312"} The Mystery of Karazhan II
UPDATE `quest_template` SET `PrevQuestId` = 40311 WHERE `entry` <=> 40312 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40313"} The Mystery of Karazhan III
UPDATE `quest_template` SET `PrevQuestId` = 40312 WHERE `entry` <=> 40313 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40314"} The Mystery of Karazhan IV
UPDATE `quest_template` SET `PrevQuestId` = 40313 WHERE `entry` <=> 40314 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40315"} The Mystery of Karazhan V
UPDATE `quest_template` SET `PrevQuestId` = 40314 WHERE `entry` <=> 40315 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40316"} The Mystery of Karazhan VI
UPDATE `quest_template` SET `PrevQuestId` = 40315 WHERE `entry` <=> 40316 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40319"} A Call to Aid
UPDATE `quest_template` SET `PrevQuestId` = 40318 WHERE `entry` <=> 40319 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40317"} The Mystery of Karazhan VII
UPDATE `quest_template` SET `PrevQuestId` = 40316 WHERE `entry` <=> 40317 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40320"} A Call to Aid
UPDATE `quest_template` SET `PrevQuestId` = 40319 WHERE `entry` <=> 40320 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40321"} The Northeast Passage
UPDATE `quest_template` SET `PrevQuestId` = 80391 WHERE `entry` <=> 40321 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40322"} The Kaneq'nuun Elder
UPDATE `quest_template` SET `PrevQuestId` = 40321 WHERE `entry` <=> 40322 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40323"} An Offering for Kaneq'nuun
UPDATE `quest_template` SET `PrevQuestId` = 40322 WHERE `entry` <=> 40323 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40328"} Naga Tridents
UPDATE `quest_template` SET `RewItemId1` = 60475, `RewItemCount1` = 1 WHERE `entry` <=> 40328 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40325"} Icepoint Whiteclaws
UPDATE `quest_template` SET `PrevQuestId` = 40323 WHERE `entry` <=> 40325 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40324"} The Legend of Tarqsiku
UPDATE `quest_template` SET `PrevQuestId` = 40323, `RewItemId1` = 60470, `RewItemCount1` = 1 WHERE `entry` <=> 40324 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40334"} The Bloodsail Compound V
UPDATE `quest_template` SET `PrevQuestId` = 40333, `RewChoiceItemId1` = 60480, `RewChoiceItemId2` = 60481, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40334 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40339"} The Azurestone Order
UPDATE `quest_template` SET `PrevQuestId` = 40338, `RewItemId1` = 60495, `RewItemCount1` = 1 WHERE `entry` <=> 40339 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80723"} Trusted Apothecary
UPDATE `quest_template` SET `PrevQuestId` = 80722 WHERE `entry` <=> 80723 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80724"} Consulting an Expert
UPDATE `quest_template` SET `PrevQuestId` = 80723 WHERE `entry` <=> 80724 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80725"} In Gunther’s Favor
UPDATE `quest_template` SET `PrevQuestId` = 80724 WHERE `entry` <=> 80725 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80726"} Soul and Alchemy
UPDATE `quest_template` SET `PrevQuestId` = 80725 WHERE `entry` <=> 80726 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80727"} Dark Temper for a Dark Lady
UPDATE `quest_template` SET `PrevQuestId` = 80726 WHERE `entry` <=> 80727 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80728"} The Future Looks Grim
UPDATE `quest_template` SET `PrevQuestId` = 80727 WHERE `entry` <=> 80728 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80729"} A Different Shade of Red
UPDATE `quest_template` SET `PrevQuestId` = 80728 WHERE `entry` <=> 80729 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40358"} Golden Elves of Feralas
UPDATE `quest_template` SET `PrevQuestId` = 40357 WHERE `entry` <=> 40358 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40359"} Mechaflame's Bargain
UPDATE `quest_template` SET `PrevQuestId` = 40358 WHERE `entry` <=> 40359 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40360"} Highly Unexpected Event
UPDATE `quest_template` SET `PrevQuestId` = 40359 WHERE `entry` <=> 40360 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40361"} Taking the Booty Home
UPDATE `quest_template` SET `PrevQuestId` = 40360 WHERE `entry` <=> 40361 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40362"} A Historian Finds You
UPDATE `quest_template` SET `PrevQuestId` = 40361 WHERE `entry` <=> 40362 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40369"} Further Studies
UPDATE `quest_template` SET `PrevQuestId` = 6343, `RewItemId1` = 81159, `RewItemCount1` = 1 WHERE `entry` <=> 40369 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40372"} [DEPRECATED] Appealing to the Alliance: Humans
UPDATE `quest_template` SET `PrevQuestId` = -40371, `RewItemId1` = 83016, `RewItemCount1` = 1 WHERE `entry` <=> 40372 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40374"} [DEPRECATED] Appealing to the Alliance: Gnomes
UPDATE `quest_template` SET `PrevQuestId` = -40371, `RewItemId1` = 83019, `RewItemCount1` = 1 WHERE `entry` <=> 40374 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40375"} [DEPRECATED] Appealing to the Alliance: Night Elves
UPDATE `quest_template` SET `PrevQuestId` = -40371, `RewItemId1` = 83018, `RewItemCount1` = 1 WHERE `entry` <=> 40375 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40386"} [CANCELLED] Scourge!
UPDATE `quest_template` SET `PrevQuestId` = 40385 WHERE `entry` <=> 40386 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40384"} [DEPRECATED] A People Restored
UPDATE `quest_template` SET `PrevQuestId` = 40383 WHERE `entry` <=> 40384 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40387"} [CANCELLED] Returning to Alah'thalas
UPDATE `quest_template` SET `PrevQuestId` = 40386 WHERE `entry` <=> 40387 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40391"} A Hunt for Honor
UPDATE `quest_template` SET `PrevQuestId` = 40390 WHERE `entry` <=> 40391 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40393"} [DEPRECATED] Summons by Lady Windrunner
UPDATE `quest_template` SET `PrevQuestId` = 40379 WHERE `entry` <=> 40393 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40345"} The Way of Spiritwalking II
UPDATE `quest_template` SET `PrevQuestId` = 40344 WHERE `entry` <=> 40345 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40399"} By Any Means Necessary III
UPDATE `quest_template` SET `PrevQuestId` = 40398 WHERE `entry` <=> 40399 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40401"} By Any Means Necessary V
UPDATE `quest_template` SET `PrevQuestId` = 40400 WHERE `entry` <=> 40401 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40400"} By Any Means Necessary IV
UPDATE `quest_template` SET `PrevQuestId` = 40399 WHERE `entry` <=> 40400 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40398"} By Any Means Necessary II
UPDATE `quest_template` SET `PrevQuestId` = 40397 WHERE `entry` <=> 40398 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40404"} Return to Tharg
UPDATE `quest_template` SET `PrevQuestId` = 40403 WHERE `entry` <=> 40404 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40403"} Tharg's Gift to Friend
UPDATE `quest_template` SET `PrevQuestId` = 40402, `RewItemId1` = 60585, `RewItemCount1` = 1 WHERE `entry` <=> 40403 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40417"} Delivery to Wallace
UPDATE `quest_template` SET `PrevQuestId` = 40416 WHERE `entry` <=> 40417 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40421"} The Shadowsworn Enemy
UPDATE `quest_template` SET `PrevQuestId` = 40420 WHERE `entry` <=> 40421 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40422"} Shadowsworn Altar
UPDATE `quest_template` SET `PrevQuestId` = 40421 WHERE `entry` <=> 40422 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40423"} Blasted Ogres!
UPDATE `quest_template` SET `PrevQuestId` = 40420 WHERE `entry` <=> 40423 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40437"} Finding Private Holson II
UPDATE `quest_template` SET `PrevQuestId` = 40436 WHERE `entry` <=> 40437 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80701"} The Elusive SI:7
UPDATE `quest_template` SET `PrevQuestId` = 80700 WHERE `entry` <=> 80701 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80702"} Young and Foolish
UPDATE `quest_template` SET `PrevQuestId` = 80701 WHERE `entry` <=> 80702 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80730"} Caravan Meets Caravan
UPDATE `quest_template` SET `PrevQuestId` = 80702 WHERE `entry` <=> 80730 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80703"} Thandol Span
UPDATE `quest_template` SET `PrevQuestId` = 80730 WHERE `entry` <=> 80703 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80704"} Are You True to Your Nature?
UPDATE `quest_template` SET `PrevQuestId` = 80703 WHERE `entry` <=> 80704 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80705"} The Means of Persuading
UPDATE `quest_template` SET `PrevQuestId` = 80704 WHERE `entry` <=> 80705 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80706"} Seeking Justice or Vengeance?
UPDATE `quest_template` SET `PrevQuestId` = 80704 WHERE `entry` <=> 80706 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80707"} The Price Of Information
UPDATE `quest_template` SET `PrevQuestId` = 80705 WHERE `entry` <=> 80707 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80708"} Scarlet Aid
UPDATE `quest_template` SET `PrevQuestId` = 80707 WHERE `entry` <=> 80708 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80709"} Donning the Red Flag
UPDATE `quest_template` SET `PrevQuestId` = 80708 WHERE `entry` <=> 80709 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80710"} It’s All in Their Brains
UPDATE `quest_template` SET `PrevQuestId` = 80709 WHERE `entry` <=> 80710 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3830373239' USING utf8mb4));
-- quest_template {"entry": "80711"} Supplies We Need
UPDATE `quest_template` SET `PrevQuestId` = 80710 WHERE `entry` <=> 80711 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40445"} Dominance of the Skies
UPDATE `quest_template` SET `RewItemId1` = 60650, `RewItemCount1` = 1 WHERE `entry` <=> 40445 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40452"} To Strike Back : Ruins of Zul'Rasaz
UPDATE `quest_template` SET `RewItemId1` = 60657, `RewItemCount1` = 1 WHERE `entry` <=> 40452 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40455"} Farwell Commission
UPDATE `quest_template` SET `PrevQuestId` = 40454 WHERE `entry` <=> 40455 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40347"} The Way of Spiritwalking IV
UPDATE `quest_template` SET `PrevQuestId` = 40346 WHERE `entry` <=> 40347 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40348"} The Way of Spiritwalking V
UPDATE `quest_template` SET `PrevQuestId` = 40347, `RewSpellCast` = 51669 WHERE `entry` <=> 40348 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewSpellCast` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3435353030' USING utf8mb4));
-- quest_template {"entry": "40350"} The Way Of The Witch Doctor II
UPDATE `quest_template` SET `PrevQuestId` = 40349 WHERE `entry` <=> 40350 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40351"} The Way Of The Witch Doctor III
UPDATE `quest_template` SET `PrevQuestId` = 40350 WHERE `entry` <=> 40351 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40352"} The Way Of The Witch Doctor IV
UPDATE `quest_template` SET `PrevQuestId` = 40351 WHERE `entry` <=> 40352 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40353"} The Way Of The Witch Doctor V
UPDATE `quest_template` SET `PrevQuestId` = 40352, `RewSpellCast` = 47263 WHERE `entry` <=> 40353 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewSpellCast` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3435353034' USING utf8mb4));
-- quest_template {"entry": "40460"} Hearing Orvak's Story
UPDATE `quest_template` SET `PrevQuestId` = 40459 WHERE `entry` <=> 40460 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40461"} The Sternrock Stash
UPDATE `quest_template` SET `PrevQuestId` = 40460 WHERE `entry` <=> 40461 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40462"} Miners Union Mutiny
UPDATE `quest_template` SET `PrevQuestId` = 40461 WHERE `entry` <=> 40462 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40464"} Senatorial Revenge
UPDATE `quest_template` SET `PrevQuestId` = 40461, `RewItemId1` = 60668, `RewItemCount1` = 1 WHERE `entry` <=> 40464 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40465"} Discovering Golem Secrets
UPDATE `quest_template` SET `PrevQuestId` = 40461 WHERE `entry` <=> 40465 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40467"} The Arcane Golem Core
UPDATE `quest_template` SET `PrevQuestId` = 40466 WHERE `entry` <=> 40467 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40468"} Miners Union Mutiny II
UPDATE `quest_template` SET `PrevQuestId` = 40462 WHERE `entry` <=> 40468 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40469"} Vilegrip's Demise
UPDATE `quest_template` SET `PrevQuestId` = 40461, `RewItemId1` = 60675, `RewItemCount1` = 1 WHERE `entry` <=> 40469 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40471"} The Harvest Golem Mystery
UPDATE `quest_template` SET `RewChoiceItemId1` = 60678, `RewChoiceItemId2` = 60679, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40471 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40475"} The Harvest Golem Mystery
UPDATE `quest_template` SET `PrevQuestId` = 40474 WHERE `entry` <=> 40475 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40477"} The Harvest Golem Mystery
UPDATE `quest_template` SET `PrevQuestId` = 40476 WHERE `entry` <=> 40477 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40463"} The True High Foreman
UPDATE `quest_template` SET `PrevQuestId` = 40461 WHERE `entry` <=> 40463 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40466"} To Purchase Secret Information
UPDATE `quest_template` SET `PrevQuestId` = 40465 WHERE `entry` <=> 40466 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40487"} The Hateforge Report
UPDATE `quest_template` SET `PrevQuestId` = 40486 WHERE `entry` <=> 40487 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40488"} The King's Response
UPDATE `quest_template` SET `PrevQuestId` = 40487 WHERE `entry` <=> 40488 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40489"} Assaulting Hateforge
UPDATE `quest_template` SET `PrevQuestId` = 40488 WHERE `entry` <=> 40489 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40490"} Rumors of Hateforge Brew
UPDATE `quest_template` SET `RewItemId1` = 2686, `RewItemId2` = 60699, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40490 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40493"} Fueling the Blood Fury
UPDATE `quest_template` SET `PrevQuestId` = 40492 WHERE `entry` <=> 40493 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40495"} Forest Troll Scum
UPDATE `quest_template` SET `PrevQuestId` = 40494, `RewItemId1` = 60715, `RewItemCount1` = 1 WHERE `entry` <=> 40495 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40498"} Raider's Raid
UPDATE `quest_template` SET `PrevQuestId` = 40497 WHERE `entry` <=> 40498 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40497"} Raider's New Mount
UPDATE `quest_template` SET `PrevQuestId` = 40496 WHERE `entry` <=> 40497 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40502"} Of New and Old II
UPDATE `quest_template` SET `PrevQuestId` = 40501 WHERE `entry` <=> 40502 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40504"} Of New and Old IV
UPDATE `quest_template` SET `PrevQuestId` = 40503 WHERE `entry` <=> 40504 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40505"} Protecting Fresh Blood
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -2010905, `ReqCreatureOrGOId2` = -2010906, `ReqCreatureOrGOId3` = -2010907 WHERE `entry` <=> 40505 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "40506"} Report to Molk
UPDATE `quest_template` SET `PrevQuestId` = 40505 WHERE `entry` <=> 40506 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40507"} Destroy All Traces...
UPDATE `quest_template` SET `PrevQuestId` = 40506 WHERE `entry` <=> 40507 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40508"} Take No Chances
UPDATE `quest_template` SET `PrevQuestId` = 40507 WHERE `entry` <=> 40508 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40509"} The Final Crack
UPDATE `quest_template` SET `PrevQuestId` = 40508, `RewItemId1` = 60739, `RewItemCount1` = 1 WHERE `entry` <=> 40509 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40512"} Eperius Needs Grains
UPDATE `quest_template` SET `PrevQuestId` = 40511 WHERE `entry` <=> 40512 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40503"} Of New and Old III
UPDATE `quest_template` SET `PrevQuestId` = 40502 WHERE `entry` <=> 40503 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40513"} Polymorph Enslavement!
UPDATE `quest_template` SET `RewItemId1` = 60745, `RewItemCount1` = 1 WHERE `entry` <=> 40513 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40518"} The Ramolus Gem
UPDATE `quest_template` SET `RewItemId1` = 60757, `RewItemCount1` = 1 WHERE `entry` <=> 40518 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40523"} Sandfury Redemption II
UPDATE `quest_template` SET `PrevQuestId` = 40522 WHERE `entry` <=> 40523 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40524"} Sandfury Redemption III
UPDATE `quest_template` SET `PrevQuestId` = 40523 WHERE `entry` <=> 40524 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40525"} Of the Sandfury and the Darkspear
UPDATE `quest_template` SET `PrevQuestId` = 40524 WHERE `entry` <=> 40525 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40526"} Plight of the Sandfury
UPDATE `quest_template` SET `PrevQuestId` = 40525 WHERE `entry` <=> 40526 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40527"} End Ukorz Sandscalp
UPDATE `quest_template` SET `PrevQuestId` = 40526, `RewChoiceItemId1` = 60764, `RewChoiceItemId2` = 60765, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40527 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40529"} Letter from Maltimor
UPDATE `quest_template` SET `PrevQuestId` = 40484 WHERE `entry` <=> 40529 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40539"} Hunting Engineer Figgles
UPDATE `quest_template` SET `PrevQuestId` = 40538 WHERE `entry` <=> 40539 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40537"} Eager Little One
UPDATE `quest_template` SET `RewItemId1` = 60770, `RewItemCount1` = 1 WHERE `entry` <=> 40537 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "20004"} An Act of Humility
UPDATE `quest_template` SET `PrevQuestId` = 20003 WHERE `entry` <=> 20004 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40543"} Loneliest Murloc in the World
UPDATE `quest_template` SET `PrevQuestId` = 40541 WHERE `entry` <=> 40543 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40548"} Report from Falgran
UPDATE `quest_template` SET `PrevQuestId` = 1284 WHERE `entry` <=> 40548 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40549"} The Quillboar Suspects
UPDATE `quest_template` SET `PrevQuestId` = 40548 WHERE `entry` <=> 40549 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40550"} The Goblin Linguist
UPDATE `quest_template` SET `PrevQuestId` = 40549 WHERE `entry` <=> 40550 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40551"} A Strangle Favor
UPDATE `quest_template` SET `PrevQuestId` = 40550 WHERE `entry` <=> 40551 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40552"} Back to Falgran
UPDATE `quest_template` SET `PrevQuestId` = 40551 WHERE `entry` <=> 40552 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40553"} Suspicious No More
UPDATE `quest_template` SET `PrevQuestId` = 40552 WHERE `entry` <=> 40553 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40554"} The Shield's Owner
UPDATE `quest_template` SET `PrevQuestId` = 40553 WHERE `entry` <=> 40554 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40555"} The Search for the Grave
UPDATE `quest_template` SET `PrevQuestId` = 40554 WHERE `entry` <=> 40555 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40556"} Another Mystery to Solve
UPDATE `quest_template` SET `PrevQuestId` = 40555 WHERE `entry` <=> 40556 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40557"} The Troubles Ahead
UPDATE `quest_template` SET `PrevQuestId` = 40556 WHERE `entry` <=> 40557 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40558"} The Meeting with the Enemy
UPDATE `quest_template` SET `PrevQuestId` = 40557 WHERE `entry` <=> 40558 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40559"} The Betrayal from Within
UPDATE `quest_template` SET `PrevQuestId` = 40558 WHERE `entry` <=> 40559 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40560"} The Betrayal from Within
UPDATE `quest_template` SET `PrevQuestId` = 40559 WHERE `entry` <=> 40560 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40561"} Preparation for Divination
UPDATE `quest_template` SET `PrevQuestId` = 40560 WHERE `entry` <=> 40561 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40562"} Ritual of Divination
UPDATE `quest_template` SET `PrevQuestId` = 40561 WHERE `entry` <=> 40562 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40563"} To Meet Again
UPDATE `quest_template` SET `PrevQuestId` = 40562 WHERE `entry` <=> 40563 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40564"} The Second Meeting
UPDATE `quest_template` SET `PrevQuestId` = 40563 WHERE `entry` <=> 40564 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40565"} And Justice for All
UPDATE `quest_template` SET `PrevQuestId` = 40564 WHERE `entry` <=> 40565 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40567"} Sleepwalking at Day
UPDATE `quest_template` SET `PrevQuestId` = 40566 WHERE `entry` <=> 40567 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40572"} Aid Kagoro
UPDATE `quest_template` SET `PrevQuestId` = 1268 WHERE `entry` <=> 40572 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40573"} On a Boar's Trail
UPDATE `quest_template` SET `PrevQuestId` = 40572 WHERE `entry` <=> 40573 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40574"} Someone With A Pig's Tongue
UPDATE `quest_template` SET `PrevQuestId` = 40573 WHERE `entry` <=> 40574 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40575"} Willing to Serve
UPDATE `quest_template` SET `PrevQuestId` = 40574 WHERE `entry` <=> 40575 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40576"} A Glutton's Honesty
UPDATE `quest_template` SET `PrevQuestId` = 40575 WHERE `entry` <=> 40576 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40577"} Behind the Hoofprints
UPDATE `quest_template` SET `PrevQuestId` = 40576 WHERE `entry` <=> 40577 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40578"} Dragging It Back To Brackenwall
UPDATE `quest_template` SET `PrevQuestId` = 1276 WHERE `entry` <=> 40578 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40580"} Mighty Wattapo, the Wall
UPDATE `quest_template` SET `PrevQuestId` = 40579 WHERE `entry` <=> 40580 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40581"} Mysterious Shield No More
UPDATE `quest_template` SET `PrevQuestId` = 40580 WHERE `entry` <=> 40581 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40582"} Tedious Diplomacy
UPDATE `quest_template` SET `PrevQuestId` = 40581 WHERE `entry` <=> 40582 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40591"} Favor for Gelwig
UPDATE `quest_template` SET `PrevQuestId` = 40590 WHERE `entry` <=> 40591 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40605"} The Redbrand Archive
UPDATE `quest_template` SET `PrevQuestId` = 40604 WHERE `entry` <=> 40605 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40607"} Ritual Secrets of the Witherbark
UPDATE `quest_template` SET `PrevQuestId` = 40606 WHERE `entry` <=> 40607 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40743"} Exchanging Foreign Food
UPDATE `quest_template` SET `RewItemId1` = 60984, `RewItemCount1` = 1 WHERE `entry` <=> 40743 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40610"} Fighting the Pack
UPDATE `quest_template` SET `PrevQuestId` = 40633 WHERE `entry` <=> 40610 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40611"} The Blood of Lizards
UPDATE `quest_template` SET `PrevQuestId` = 40633 WHERE `entry` <=> 40611 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40616"} One of Us
UPDATE `quest_template` SET `RewChoiceItemId1` = 60853, `RewChoiceItemId2` = 60854, `RewChoiceItemId3` = 60855, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40616 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40617"} The Maraudine Front
UPDATE `quest_template` SET `PrevQuestId` = 40633 WHERE `entry` <=> 40617 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40618"} Raiding The Lesser Tribes
UPDATE `quest_template` SET `PrevQuestId` = 40633 WHERE `entry` <=> 40618 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40619"} The War for Food
UPDATE `quest_template` SET `PrevQuestId` = 40633 WHERE `entry` <=> 40619 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40621"} Meeting an Elder Centaur
UPDATE `quest_template` SET `PrevQuestId` = 40620 WHERE `entry` <=> 40621 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40622"} Sympathy for the Dead
UPDATE `quest_template` SET `PrevQuestId` = 40621 WHERE `entry` <=> 40622 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40623"} Troubling Winds
UPDATE `quest_template` SET `PrevQuestId` = 40621 WHERE `entry` <=> 40623 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40624"} Protecting the Weak
UPDATE `quest_template` SET `PrevQuestId` = 40622 WHERE `entry` <=> 40624 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40629"} An Honorary Gelkis
UPDATE `quest_template` SET `RewChoiceItemId1` = 60854, `RewChoiceItemId2` = 60859, `RewChoiceItemId3` = 60860, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40629 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40630"} Collapse of the Maraudine
UPDATE `quest_template` SET `PrevQuestId` = 40621 WHERE `entry` <=> 40630 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40631"} In Commune With Wind
UPDATE `quest_template` SET `PrevQuestId` = 40621 WHERE `entry` <=> 40631 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40632"} Our Mortal Enemies
UPDATE `quest_template` SET `PrevQuestId` = 40621 WHERE `entry` <=> 40632 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40633"} Enter the Village
UPDATE `quest_template` SET `PrevQuestId` = 40609 WHERE `entry` <=> 40633 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40637"} Earth Rumble
UPDATE `quest_template` SET `PrevQuestId` = 40636 WHERE `entry` <=> 40637 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40640"} Nazz's Got A Plan
UPDATE `quest_template` SET `PrevQuestId` = 40639 WHERE `entry` <=> 40640 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40641"} Grand Theft Shredder
UPDATE `quest_template` SET `PrevQuestId` = 40640, `RewChoiceItemId1` = 60872, `RewChoiceItemId2` = 60873, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40641 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40642"} Lighting the Oilmaster
UPDATE `quest_template` SET `PrevQuestId` = 40641 WHERE `entry` <=> 40642 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40646"} Least Paranoid Goblin
UPDATE `quest_template` SET `PrevQuestId` = 40645 WHERE `entry` <=> 40646 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40648"} Roses are Red, Dragons are Blue...
UPDATE `quest_template` SET `RewChoiceItemId1` = 60887, `RewChoiceItemId2` = 60888, `RewChoiceItemId3` = 60889, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40648 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40653"} Digging a Little Deeper
UPDATE `quest_template` SET `PrevQuestId` = 40652 WHERE `entry` <=> 40653 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40656"} Bane of Scalebane
UPDATE `quest_template` SET `PrevQuestId` = 40655 WHERE `entry` <=> 40656 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40659"} A Razor's Edge
UPDATE `quest_template` SET `RewItemId1` = 60896, `RewItemCount1` = 1 WHERE `entry` <=> 40659 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40662"} Charys' Response
UPDATE `quest_template` SET `PrevQuestId` = 40661 WHERE `entry` <=> 40662 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40672"} Anchor's End
UPDATE `quest_template` SET `PrevQuestId` = 40671 WHERE `entry` <=> 40672 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40673"} Cutting Ranks
UPDATE `quest_template` SET `PrevQuestId` = 40671 WHERE `entry` <=> 40673 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40674"} Lieutenant Alverold
UPDATE `quest_template` SET `PrevQuestId` = 40673 WHERE `entry` <=> 40674 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40675"} A Hero's Reward
UPDATE `quest_template` SET `RewChoiceItemId1` = 60915, `RewChoiceItemId2` = 60916, `RewChoiceItemId3` = 60917, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40675 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40677"} Lofty Heights
UPDATE `quest_template` SET `PrevQuestId` = 40676 WHERE `entry` <=> 40677 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40688"} The Blackhorn Conflict
UPDATE `quest_template` SET `RewItemId1` = 60927, `RewItemCount1` = 1 WHERE `entry` <=> 40688 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40690"} Blackhorn Village
UPDATE `quest_template` SET `RewChoiceItemId1` = 60928, `RewChoiceItemId2` = 60929, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40690 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40692"} The Battlebeard Legacy
UPDATE `quest_template` SET `RewItemId1` = 60931, `RewItemCount1` = 1 WHERE `entry` <=> 40692 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40693"} The Ornate Windshear Gemstone
UPDATE `quest_template` SET `RewItemId1` = 60933, `RewItemId2` = 60934, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40693 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40718"} Next Course of Action
UPDATE `quest_template` SET `PrevQuestId` = 40582 WHERE `entry` <=> 40718 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40719"} Exquisite Goblin Engineering
UPDATE `quest_template` SET `PrevQuestId` = 40718 WHERE `entry` <=> 40719 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40720"} Conclusive Evidence
UPDATE `quest_template` SET `PrevQuestId` = 40719 WHERE `entry` <=> 40720 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40721"} Tedious Diplomacy Once More
UPDATE `quest_template` SET `PrevQuestId` = 40720 WHERE `entry` <=> 40721 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40722"} Justice for Dustwallow
UPDATE `quest_template` SET `PrevQuestId` = 40721, `RewChoiceItemId1` = 60950, `RewChoiceItemId2` = 60951, `RewChoiceItemId3` = 60952, `RewChoiceItemId4` = 60953, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1, `RewChoiceItemCount4` = 1 WHERE `entry` <=> 40722 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40723"} Honoring Treaties
UPDATE `quest_template` SET `PrevQuestId` = 40722 WHERE `entry` <=> 40723 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40686"} A Pandaren Brewmaster
UPDATE `quest_template` SET `RewItemId1` = 60925, `RewItemCount1` = 1 WHERE `entry` <=> 40686 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40727"} Derelict Supplies
UPDATE `quest_template` SET `RewItemId1` = 60960, `RewItemCount1` = 1 WHERE `entry` <=> 40727 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40728"} Highvale Rise
UPDATE `quest_template` SET `PrevQuestId` = 40724, `RewChoiceItemId1` = 13446, `RewChoiceItemId2` = 13444, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40728 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40730"} Early Testing
UPDATE `quest_template` SET `PrevQuestId` = 40729 WHERE `entry` <=> 40730 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40731"} A Costly Favor
UPDATE `quest_template` SET `PrevQuestId` = 40730 WHERE `entry` <=> 40731 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40732"} Two Favors For The Price of One
UPDATE `quest_template` SET `PrevQuestId` = 40731 WHERE `entry` <=> 40732 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40733"} Gargantuan Information!
UPDATE `quest_template` SET `PrevQuestId` = 40732 WHERE `entry` <=> 40733 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40734"} A Must Have Discovery
UPDATE `quest_template` SET `PrevQuestId` = 40733 WHERE `entry` <=> 40734 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40735"} The Micro Filter Tazzo-Scope
UPDATE `quest_template` SET `PrevQuestId` = 40734 WHERE `entry` <=> 40735 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40736"} Gargantuan Studies
UPDATE `quest_template` SET `PrevQuestId` = 40735 WHERE `entry` <=> 40736 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40737"} The Final Test
UPDATE `quest_template` SET `PrevQuestId` = 40736 WHERE `entry` <=> 40737 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40738"} News for Danonzo!
UPDATE `quest_template` SET `PrevQuestId` = 40737 WHERE `entry` <=> 40738 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40739"} The Tel'Abim Banana Transmutation
UPDATE `quest_template` SET `PrevQuestId` = 40737 WHERE `entry` <=> 40739 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40740"} Tel'Abim Banana Transmutations!
UPDATE `quest_template` SET `PrevQuestId` = 40737 WHERE `entry` <=> 40740 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40742"} The Work Must Continue
UPDATE `quest_template` SET `PrevQuestId` = 40741, `RewChoiceItemId1` = 60972, `RewChoiceItemId2` = 60973, `RewChoiceItemId3` = 60974, `RewChoiceItemId4` = 60975, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1, `RewChoiceItemCount4` = 1 WHERE `entry` <=> 40742 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40744"} Gifts to Wrap
UPDATE `quest_template` SET `RewItemId1` = 60985, `RewItemCount1` = 1 WHERE `entry` <=> 40744 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40747"} A Programmer?
UPDATE `quest_template` SET `RewItemId1` = 60982, `RewItemCount1` = 1 WHERE `entry` <=> 40747 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40748"} Winter Veil Brew
UPDATE `quest_template` SET `RewItemId1` = 60983, `RewItemId2` = 51255, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40748 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40749"} Translating The Unknown
UPDATE `quest_template` SET `NextQuestInChain` = 40750 WHERE `entry` <=> 40749 AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40752"} Danonzo's Tel'Abim Surprise
UPDATE `quest_template` SET `PrevQuestId` = 40738, `RewItemId1` = 60976, `RewItemId2` = 60979, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40752 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40753"} Danonzo's Tel'Abim Delight
UPDATE `quest_template` SET `PrevQuestId` = 40738, `RewItemId1` = 60977, `RewItemId2` = 60980, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40753 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40754"} Danonzo's Tel'Abim Medley
UPDATE `quest_template` SET `PrevQuestId` = 40738, `RewItemId1` = 60978, `RewItemId2` = 60981, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40754 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40760"} Operation Final Repairs
UPDATE `quest_template` SET `RewChoiceItemId1` = 60996, `RewChoiceItemId2` = 60997, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40760 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40762"} The Dark Iron Desecrator
UPDATE `quest_template` SET `RewItemId1` = 61068, `RewItemCount1` = 1 WHERE `entry` <=> 40762 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40785"} King Morogo Thunderfoot!
UPDATE `quest_template` SET `RewItemId1` = 61091, `RewItemId2` = 61092, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40785 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40764"} Azotha Gold
UPDATE `quest_template` SET `PrevQuestId` = 40763 WHERE `entry` <=> 40764 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40766"} A Report From Bixxle
UPDATE `quest_template` SET `PrevQuestId` = 40765 WHERE `entry` <=> 40766 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40767"} Final Flight of the Venomflayer
UPDATE `quest_template` SET `RewItemId1` = 61074, `RewItemCount1` = 1 WHERE `entry` <=> 40767 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40768"} Intercepting Wavecrest
UPDATE `quest_template` SET `RewChoiceItemId1` = 61076, `RewChoiceItemId2` = 61077, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40768 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40769"} Chillwind Armor
UPDATE `quest_template` SET `RewItemId1` = 61079, `RewItemCount1` = 1 WHERE `entry` <=> 40769 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40771"} The Missing Friend!
UPDATE `quest_template` SET `RewItemId1` = 61080, `RewItemCount1` = 1 WHERE `entry` <=> 40771 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40778"} Zoki is No Thief!
UPDATE `quest_template` SET `PrevQuestId` = 40777 WHERE `entry` <=> 40778 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40779"} The Terrible Krampus
UPDATE `quest_template` SET `PrevQuestId` = 40778, `RewChoiceItemId1` = 61086, `RewChoiceItemId2` = 61087, `RewChoiceItemId3` = 61088, `RewChoiceItemId4` = 84038, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1, `RewChoiceItemCount4` = 1 WHERE `entry` <=> 40779 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40781"} Further Inquiries About Legend
UPDATE `quest_template` SET `PrevQuestId` = 40780 WHERE `entry` <=> 40781 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40782"} The Legend Comes To Life!
UPDATE `quest_template` SET `PrevQuestId` = 40781 WHERE `entry` <=> 40782 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40783"} Life Comes To The Legend!
UPDATE `quest_template` SET `PrevQuestId` = 40781 WHERE `entry` <=> 40783 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40787"} To Hunt The Hunter
UPDATE `quest_template` SET `RewItemId1` = 61095, `RewItemCount1` = 1 WHERE `entry` <=> 40787 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40788"} Heavy Earthen Cores
UPDATE `quest_template` SET `RewChoiceItemId1` = 8952, `RewChoiceItemId2` = 8766, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40788 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40791"} Kul Tiran Provisions: Special Goods
UPDATE `quest_template` SET `PrevQuestId` = 40790, `RewItemId1` = 61100, `RewItemCount1` = 1 WHERE `entry` <=> 40791 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "80606"} Revenge on Game Masters!
UPDATE `quest_template` SET `RewItemId1` = 84039, `RewItemCount1` = 1 WHERE `entry` <=> 80606 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40869"} Mastering the Formula III
UPDATE `quest_template` SET `RewChoiceItemId1` = 61404, `RewChoiceItemId2` = 61405, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40869 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "39991"} Securing the Roads
UPDATE `quest_template` SET `ReqSourceId1` = 51837, `ReqSourceCount1` = 1 WHERE `entry` <=> 39991 AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "39995"} Horde Defender's Axe
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 39995 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3339393934' USING utf8mb4));
-- quest_template {"entry": "39996"} Horde Defender's Axe
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 39996 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3339393935' USING utf8mb4));
-- quest_template {"entry": "39997"} Horde Defender's Axe
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 39997 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3339393936' USING utf8mb4));
-- quest_template {"entry": "39998"} Horde Defender's Axe
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 39998 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3339393937' USING utf8mb4));
-- quest_template {"entry": "39999"} Horde Defender's Axe
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 39999 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3339393938' USING utf8mb4));
-- quest_template {"entry": "40819"} The Key to Karazhan III
UPDATE `quest_template` SET `PrevQuestId` = 40818 WHERE `entry` <=> 40819 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40822"} The Key to Karazhan III
UPDATE `quest_template` SET `PrevQuestId` = 40818 WHERE `entry` <=> 40822 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40825"} The Key to Karazhan VI
UPDATE `quest_template` SET `PrevQuestId` = 40824 WHERE `entry` <=> 40825 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3430383231' USING utf8mb4));
-- quest_template {"entry": "80313"} Welcome to Amani'Alor
UPDATE `quest_template` SET `PrevQuestId` = 80302 WHERE `entry` <=> 80313 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "39992"} Securing the Keep
UPDATE `quest_template` SET `PrevQuestId` = 0, `ReqSourceId1` = 51837, `ReqSourceCount1` = 1 WHERE `entry` <=> 39992 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3339393931' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "39993"} Cleaning Job
UPDATE `quest_template` SET `PrevQuestId` = 0, `ReqSourceId1` = 51837, `ReqSourceCount1` = 1 WHERE `entry` <=> 39993 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3339393932' USING utf8mb4)) AND (BINARY CAST(`ReqSourceId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ReqSourceCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40848"} Quality Time with Blackthorn
UPDATE `quest_template` SET `PrevQuestId` = 40845 WHERE `entry` <=> 40848 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40856"} Backup System Activation
UPDATE `quest_template` SET `RewChoiceItemId1` = 61383, `RewChoiceItemId2` = 61384, `RewChoiceItemId3` = 61385, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40856 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40857"} Removing Leper Leadership
UPDATE `quest_template` SET `RewItemId1` = 61386, `RewItemCount1` = 1 WHERE `entry` <=> 40857 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40859"} Hydrocondensor Modulator
UPDATE `quest_template` SET `RewItemId1` = 61389, `RewItemCount1` = 1 WHERE `entry` <=> 40859 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40860"} The Dawnstone Coupler
UPDATE `quest_template` SET `RewItemId1` = 61391, `RewItemCount1` = 1 WHERE `entry` <=> 40860 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40861"} High Energy Regulator
UPDATE `quest_template` SET `RewItemId1` = 61393, `RewItemCount1` = 1 WHERE `entry` <=> 40861 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40863"} The Mercy of Humility
UPDATE `quest_template` SET `RewItemId1` = 61396, `RewItemCount1` = 1 WHERE `entry` <=> 40863 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40870"} Blooming Hyjalroot
UPDATE `quest_template` SET `RewItemId1` = 61199, `RewItemId2` = 61410, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40870 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40871"} Stockpiling Hyjalroot
UPDATE `quest_template` SET `PrevQuestId` = 40870 WHERE `entry` <=> 40871 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40876"} The Horn of Binding
UPDATE `quest_template` SET `RewItemId1` = 61199, `RewItemId2` = 61411, `RewItemCount1` = 1, `RewItemCount2` = 1 WHERE `entry` <=> 40876 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40882"} Blood for Blood
UPDATE `quest_template` SET `RewItemId1` = 61422, `RewItemCount1` = 1 WHERE `entry` <=> 40882 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40891"} The Corruption of the Barkskin Tribe
UPDATE `quest_template` SET `RewChoiceItemId1` = 61439, `RewChoiceItemId2` = 61440, `RewChoiceItemId3` = 61199, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 40891 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40894"} Continued Barkskin Vengeance
UPDATE `quest_template` SET `PrevQuestId` = 40893 WHERE `entry` <=> 40894 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40906"} Purified Dream Essence
UPDATE `quest_template` SET `PrevQuestId` = 40905 WHERE `entry` <=> 40906 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40924"} Signet of Silverlaine
UPDATE `quest_template` SET `RewChoiceItemId1` = 61467, `RewChoiceItemId2` = 61468, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 40924 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40917"} Eye for an Eye
UPDATE `quest_template` SET `PrevQuestId` = 40922 WHERE `entry` <=> 40917 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40918"} The Stranger and the Potion
UPDATE `quest_template` SET `PrevQuestId` = 40917 WHERE `entry` <=> 40918 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40919"} A Focus of the Ancients
UPDATE `quest_template` SET `PrevQuestId` = 40918 WHERE `entry` <=> 40919 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40920"} Beckoned by the Hateful Depths
UPDATE `quest_template` SET `PrevQuestId` = 40919 WHERE `entry` <=> 40920 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40921"} Drenched in Draconic Blood
UPDATE `quest_template` SET `PrevQuestId` = 40920 WHERE `entry` <=> 40921 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40922"} Eternal Mortality
UPDATE `quest_template` SET `PrevQuestId` = 40921 WHERE `entry` <=> 40922 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40923"} A Mysterious Boon
UPDATE `quest_template` SET `PrevQuestId` = 40922 WHERE `entry` <=> 40923 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40935"} Scarlet Corruption
UPDATE `quest_template` SET `RewItemId1` = 61478, `RewItemCount1` = 1 WHERE `entry` <=> 40935 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "40979"} Ebonmere Affairs
UPDATE `quest_template` SET `PrevQuestId` = 40977 WHERE `entry` <=> 40979 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9389"} Flickering Flames in the Eastern Kingdoms
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -181566, `ReqCreatureOrGOId2` = -181564, `ReqCreatureOrGOId3` = -181565, `ReqCreatureOrGOId4` = -181567 WHERE `entry` <=> 9389 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "9367"} The Festival of Fire
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -181332, `ReqCreatureOrGOId2` = -181333, `ReqCreatureOrGOId3` = -181334 WHERE `entry` <=> 9367 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "9388"} Flickering Flames in Kalimdor
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -181561, `ReqCreatureOrGOId2` = -181560, `ReqCreatureOrGOId3` = -181563, `ReqCreatureOrGOId4` = -181562 WHERE `entry` <=> 9388 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "40992"} Foulhide Mongrels
UPDATE `quest_template` SET `RewItemId1` = 6048, `RewItemCount1` = 1 WHERE `entry` <=> 40992 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41012"} The Upper Binding II
UPDATE `quest_template` SET `PrevQuestId` = 41011 WHERE `entry` <=> 41012 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41013"} The Upper Binding III
UPDATE `quest_template` SET `PrevQuestId` = 41012 WHERE `entry` <=> 41013 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41014"} The Upper Binding IV
UPDATE `quest_template` SET `PrevQuestId` = 41013 WHERE `entry` <=> 41014 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41020"} Lord Xanvarak
UPDATE `quest_template` SET `PrevQuestId` = 41016 WHERE `entry` <=> 41020 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41031"} The Gift of Ferocity
UPDATE `quest_template` SET `PrevQuestId` = 41030 WHERE `entry` <=> 41031 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41034"} A Vile Mantle
UPDATE `quest_template` SET `RewItemId1` = 61723, `RewItemCount1` = 1 WHERE `entry` <=> 41034 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41052"} Harness of Chimaeran
UPDATE `quest_template` SET `RewItemId1` = 61517, `RewItemCount1` = 1 WHERE `entry` <=> 41052 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41063"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41062 WHERE `entry` <=> 41063 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41064"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41063 WHERE `entry` <=> 41064 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41065"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41064 WHERE `entry` <=> 41065 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41066"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41065 WHERE `entry` <=> 41066 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41067"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41066 WHERE `entry` <=> 41067 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41075"} The Felwood Conspiracy
UPDATE `quest_template` SET `PrevQuestId` = 41073 WHERE `entry` <=> 41075 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41079"} Invocation of Shattering
UPDATE `quest_template` SET `PrevQuestId` = 41078 WHERE `entry` <=> 41079 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41080"} Invocation of Greater Protection
UPDATE `quest_template` SET `PrevQuestId` = 41078 WHERE `entry` <=> 41080 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41081"} Invocation of Expansive Mind
UPDATE `quest_template` SET `PrevQuestId` = 41078 WHERE `entry` <=> 41081 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41082"} Invocation of Greater Arcane Fortitude
UPDATE `quest_template` SET `PrevQuestId` = 41078 WHERE `entry` <=> 41082 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41087"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41067 WHERE `entry` <=> 41087 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41098"} [DEPRECATED] A Missive From Gadgetzan
UPDATE `quest_template` SET `PrevQuestId` = 80396 WHERE `entry` <=> 41098 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41099"} A Letter From Gadgetzan
UPDATE `quest_template` SET `PrevQuestId` = 80396 WHERE `entry` <=> 41099 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41100"} [DEPRECATED] A Letter From Rayne
UPDATE `quest_template` SET `PrevQuestId` = 80409 WHERE `entry` <=> 41100 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41101"} A Missive From Rayne
UPDATE `quest_template` SET `PrevQuestId` = 80409 WHERE `entry` <=> 41101 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41102"} A Call From Blowmaster
UPDATE `quest_template` SET `PrevQuestId` = 40130 WHERE `entry` <=> 41102 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41103"} [DEPRECATED] A Call From Torble
UPDATE `quest_template` SET `PrevQuestId` = 40128 WHERE `entry` <=> 41103 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41104"} Blowmaster's Guest of Honor
UPDATE `quest_template` SET `PrevQuestId` = 40131 WHERE `entry` <=> 41104 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41105"} [DEPRECATED] Sparksprocket's Guest of Honor
UPDATE `quest_template` SET `PrevQuestId` = 40129 WHERE `entry` <=> 41105 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41114"} Manuscript on Hydromancy II
UPDATE `quest_template` SET `PrevQuestId` = 40407 WHERE `entry` <=> 41114 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "39984"} Rich and Convicted
UPDATE `quest_template` SET `SpecialFlags` = 1024 WHERE `entry` <=> 39984 AND (BINARY CAST(`SpecialFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41117"} The Tablet of Zef'ek
UPDATE `quest_template` SET `PrevQuestId` = 40208 WHERE `entry` <=> 41117 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41130"} Leather Letter
UPDATE `quest_template` SET `PrevQuestId` = 7 WHERE `entry` <=> 41130 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41135"} Tainted Tablet
UPDATE `quest_template` SET `PrevQuestId` = 788 WHERE `entry` <=> 41135 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41136"} The Key to Karazhan IV
UPDATE `quest_template` SET `PrevQuestId` = 40819 WHERE `entry` <=> 41136 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41141"} I've Read It In A Book Once
UPDATE `quest_template` SET `PrevQuestId` = 41140 WHERE `entry` <=> 41141 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41142"} Why Not Both?
UPDATE `quest_template` SET `PrevQuestId` = 41141 WHERE `entry` <=> 41142 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41143"} Thunderforge Mastery
UPDATE `quest_template` SET `PrevQuestId` = 41142, `RewItemId1` = 40080, `RewItemCount1` = 1 WHERE `entry` <=> 41143 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41145"} A Proper Framing
UPDATE `quest_template` SET `PrevQuestId` = 41144 WHERE `entry` <=> 41145 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41146"} Every Step Of The Way
UPDATE `quest_template` SET `PrevQuestId` = 41145 WHERE `entry` <=> 41146 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41147"} Clipped Wings
UPDATE `quest_template` SET `PrevQuestId` = 41146 WHERE `entry` <=> 41147 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41148"} For Her
UPDATE `quest_template` SET `PrevQuestId` = 41147 WHERE `entry` <=> 41148 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41149"} Fly High, Little Dulin
UPDATE `quest_template` SET `PrevQuestId` = 41148 WHERE `entry` <=> 41149 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41152"} DV-500
UPDATE `quest_template` SET `PrevQuestId` = 41150 WHERE `entry` <=> 41152 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41155"} The Robotics Expert
UPDATE `quest_template` SET `PrevQuestId` = 41154 WHERE `entry` <=> 41155 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41156"} Foreman's Blues
UPDATE `quest_template` SET `PrevQuestId` = 41154 WHERE `entry` <=> 41156 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41157"} Delayed Shipments
UPDATE `quest_template` SET `PrevQuestId` = 41154 WHERE `entry` <=> 41157 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "39986"} Excessive Regulations
UPDATE `quest_template` SET `SpecialFlags` = 1024 WHERE `entry` <=> 39986 AND (BINARY CAST(`SpecialFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41170"} Solutions On Their Way
UPDATE `quest_template` SET `PrevQuestId` = 41169 WHERE `entry` <=> 41170 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41172"} War on Venture Co.
UPDATE `quest_template` SET `PrevQuestId` = 41171 WHERE `entry` <=> 41172 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41173"} Destroying Venture Co. Leadership
UPDATE `quest_template` SET `PrevQuestId` = 41172 WHERE `entry` <=> 41173 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41176"} She Sells Sea Shells
UPDATE `quest_template` SET `SpecialFlags` = 1024 WHERE `entry` <=> 41176 AND (BINARY CAST(`SpecialFlags` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41182"} Bubbling Baddies
UPDATE `quest_template` SET `PrevQuestId` = 41181 WHERE `entry` <=> 41182 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41190"} Thalassian Goulash
UPDATE `quest_template` SET `PrevQuestId` = 41187 WHERE `entry` <=> 41190 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41191"} A Crown of Flowers
UPDATE `quest_template` SET `PrevQuestId` = 41187 WHERE `entry` <=> 41191 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41192"} Arcane Golem Revolt
UPDATE `quest_template` SET `PrevQuestId` = 41187 WHERE `entry` <=> 41192 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41193"} A Troubling Presence
UPDATE `quest_template` SET `PrevQuestId` = 41189 WHERE `entry` <=> 41193 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41194"} A Letter from the Royalty
UPDATE `quest_template` SET `PrevQuestId` = 41189 WHERE `entry` <=> 41194 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41199"} Vigilant Last Defender
UPDATE `quest_template` SET `PrevQuestId` = 41198 WHERE `entry` <=> 41199 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41212"} Digging Deeper
UPDATE `quest_template` SET `PrevQuestId` = 40698 WHERE `entry` <=> 41212 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41215"} Whispers of Autumn
UPDATE `quest_template` SET `PrevQuestId` = 41214 WHERE `entry` <=> 41215 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41221"} Liadrin's Plea
UPDATE `quest_template` SET `PrevQuestId` = 41220 WHERE `entry` <=> 41221 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41226"} Magically Sealed Letter
UPDATE `quest_template` SET `PrevQuestId` = 41188 WHERE `entry` <=> 41226 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41227"} Elegant Letter
UPDATE `quest_template` SET `PrevQuestId` = 41188 WHERE `entry` <=> 41227 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41228"} Blessed Elegant Letter
UPDATE `quest_template` SET `PrevQuestId` = 41188 WHERE `entry` <=> 41228 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41229"} Shady Letter
UPDATE `quest_template` SET `PrevQuestId` = 41188 WHERE `entry` <=> 41229 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41230"} Plain Letter
UPDATE `quest_template` SET `PrevQuestId` = 41188 WHERE `entry` <=> 41230 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41231"} Feathered Letter
UPDATE `quest_template` SET `PrevQuestId` = 41188 WHERE `entry` <=> 41231 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41237"} Felstrider Retreat
UPDATE `quest_template` SET `PrevQuestId` = 41236 WHERE `entry` <=> 41237 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41259"} Journey to Auberdine
UPDATE `quest_template` SET `PrevQuestId` = 41251 WHERE `entry` <=> 41259 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41266"} The Big Man
UPDATE `quest_template` SET `PrevQuestId` = 41264 WHERE `entry` <=> 41266 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41280"} Demonstration
UPDATE `quest_template` SET `PrevQuestId` = 41279 WHERE `entry` <=> 41280 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41281"} Preparation
UPDATE `quest_template` SET `PrevQuestId` = 41280 WHERE `entry` <=> 41281 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41282"} The Final Cut
UPDATE `quest_template` SET `PrevQuestId` = 41281 WHERE `entry` <=> 41282 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41283"} Trinkets Make The Heart Grow Fonder
UPDATE `quest_template` SET `PrevQuestId` = 41275 WHERE `entry` <=> 41283 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41284"} You Should Put A Ring On It
UPDATE `quest_template` SET `PrevQuestId` = 41275 WHERE `entry` <=> 41284 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41285"} Left In Bad Faith
UPDATE `quest_template` SET `PrevQuestId` = 41275 WHERE `entry` <=> 41285 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41286"} The Artificer
UPDATE `quest_template` SET `PrevQuestId` = 41283 WHERE `entry` <=> 41286 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41287"} Sand In The Cracks
UPDATE `quest_template` SET `PrevQuestId` = 41276 WHERE `entry` <=> 41287 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41288"} Stones Of Radiance
UPDATE `quest_template` SET `PrevQuestId` = 41276 WHERE `entry` <=> 41288 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41289"} Foreign Knowledge
UPDATE `quest_template` SET `PrevQuestId` = 41276 WHERE `entry` <=> 41289 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41290"} The Artificer
UPDATE `quest_template` SET `PrevQuestId` = 41287 WHERE `entry` <=> 41290 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41292"} The Art Of Goldsmithing
UPDATE `quest_template` SET `PrevQuestId` = 41291 WHERE `entry` <=> 41292 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41293"} Elaborate Golden Bracelets
UPDATE `quest_template` SET `PrevQuestId` = 41291 WHERE `entry` <=> 41293 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41294"} Observations
UPDATE `quest_template` SET `PrevQuestId` = 41291 WHERE `entry` <=> 41294 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41295"} Dearest Gallitrea
UPDATE `quest_template` SET `PrevQuestId` = 41291 WHERE `entry` <=> 41295 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41312"} Restoration
UPDATE `quest_template` SET `RewItemId1` = 41372, `RewItemCount1` = 1 WHERE `entry` <=> 41312 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41319"} Mastercrafted Diamond Bangles
UPDATE `quest_template` SET `PrevQuestId` = 41317 WHERE `entry` <=> 41319 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41320"} Learn Of My Past
UPDATE `quest_template` SET `PrevQuestId` = 40033 WHERE `entry` <=> 41320 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41321"} Rift Fatigue: Mind
UPDATE `quest_template` SET `PrevQuestId` = 41320 WHERE `entry` <=> 41321 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41322"} Rift Fatigue: Body
UPDATE `quest_template` SET `PrevQuestId` = 41320 WHERE `entry` <=> 41322 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41323"} The Riftwalker's Cane
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 41323 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3431333231' USING utf8mb4));
-- quest_template {"entry": "41344"} Comically Large Candle
UPDATE `quest_template` SET `RewItemId1` = 92025, `RewItemCount1` = 1 WHERE `entry` <=> 41344 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41363"} Rampant Weeds
UPDATE `quest_template` SET `RewChoiceItemId1` = 3827, `RewChoiceItemId2` = 1710, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 41363 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41366"} Too Late to Prelate
UPDATE `quest_template` SET `RewItemId1` = 70225, `RewItemCount1` = 1 WHERE `entry` <=> 41366 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41367"} Against the Kolkar Dream
UPDATE `quest_template` SET `RewItemId1` = 70224, `RewItemCount1` = 1 WHERE `entry` <=> 41367 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41369"} The Scepter Rod of Medivh
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 41369 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3431303135' USING utf8mb4));
-- quest_template {"entry": "41370"} Tirisfal's Vestige
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 41370 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3431333132' USING utf8mb4));
-- quest_template {"entry": "41376"} Wrapping Warpwood
UPDATE `quest_template` SET `RewItemId1` = 51070, `RewItemCount1` = 1 WHERE `entry` <=> 41376 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41377"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41087 WHERE `entry` <=> 41377 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41380"} Scythe of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41087 WHERE `entry` <=> 41380 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41383"} Wisdom of Ur
UPDATE `quest_template` SET `PrevQuestId` = 0 WHERE `entry` <=> 41383 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3431333739' USING utf8mb4));
-- quest_template {"entry": "41386"} Pricolich Lycan
UPDATE `quest_template` SET `NextQuestInChain` = 0 WHERE `entry` <=> 41386 AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3431333934' USING utf8mb4));
-- quest_template {"entry": "41388"} Wisdom From Failure
UPDATE `quest_template` SET `NextQuestInChain` = 41389 WHERE `entry` <=> 41388 AND (BINARY CAST(`NextQuestInChain` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3431333933' USING utf8mb4));
-- quest_template {"entry": "41389"} Proof
UPDATE `quest_template` SET `PrevQuestId` = 41388 WHERE `entry` <=> 41389 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'3431333933' USING utf8mb4));
-- quest_template {"entry": "9333"} Argent Dawn Gloves
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9333 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9334"} Blessed Wizard Oil
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9334 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9335"} Consecrated Sharpening Stones
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9335 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9336"} Major Healing Potion
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9336 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9337"} Major Mana Potion
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9337 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "9343"} Tabard of the Argent Dawn
UPDATE `quest_template` SET `PrevQuestId` = 9153 WHERE `entry` <=> 9343 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41396"} Woven Dreams
UPDATE `quest_template` SET `PrevQuestId` = 41342 WHERE `entry` <=> 41396 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41518"} Mastery of Staves
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41518 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41519"} Staff Meeting
UPDATE `quest_template` SET `PrevQuestId` = 41518 WHERE `entry` <=> 41519 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41521"} Mastery of Thrown
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41521 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41522"} Throwing Hands
UPDATE `quest_template` SET `PrevQuestId` = 41521 WHERE `entry` <=> 41522 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41524"} Mastery of Guns
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41524 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41525"} One Bullet at a Time
UPDATE `quest_template` SET `PrevQuestId` = 41524 WHERE `entry` <=> 41525 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41527"} Mastery of Crossbows
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41527 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41528"} Crossing The Line
UPDATE `quest_template` SET `PrevQuestId` = 41527 WHERE `entry` <=> 41528 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41530"} Mastery of Bows
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41530 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41531"} Let The Arrow Fly
UPDATE `quest_template` SET `PrevQuestId` = 41530 WHERE `entry` <=> 41531 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41533"} Mastery of Polearms
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41533 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41534"} Polearm, Polearm...
UPDATE `quest_template` SET `PrevQuestId` = 41533 WHERE `entry` <=> 41534 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41536"} Mastery of Swords
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41536 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41537"} Scourge of Darrowshire
UPDATE `quest_template` SET `PrevQuestId` = 41536 WHERE `entry` <=> 41537 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41539"} Mastery of Hammers
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41539 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41540"} Hammer, Fall!
UPDATE `quest_template` SET `PrevQuestId` = 41539 WHERE `entry` <=> 41540 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41542"} Mastery of Axes
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41542 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41543"} The Warsong Echoes
UPDATE `quest_template` SET `PrevQuestId` = 41542 WHERE `entry` <=> 41543 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41545"} Mastery of Fist Weapons
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41545 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41546"} Fists Against the World
UPDATE `quest_template` SET `PrevQuestId` = 41545 WHERE `entry` <=> 41546 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41548"} Mastery of Daggers
UPDATE `quest_template` SET `PrevQuestId` = 41389 WHERE `entry` <=> 41548 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41549"} Vital Anatomy
UPDATE `quest_template` SET `PrevQuestId` = 41548 WHERE `entry` <=> 41549 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41551"} Pupil Once More
UPDATE `quest_template` SET `PrevQuestId` = 41292 WHERE `entry` <=> 41551 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41552"} Renewed Teachings
UPDATE `quest_template` SET `PrevQuestId` = 41282 WHERE `entry` <=> 41552 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41558"} Silver Hops
UPDATE `quest_template` SET `PrevQuestId` = 41557, `NextQuestId` = 41561, `ExclusiveGroup` = -41558 WHERE `entry` <=> 41558 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41559"} Red Hops
UPDATE `quest_template` SET `PrevQuestId` = 41557, `NextQuestId` = 41561, `ExclusiveGroup` = -41558 WHERE `entry` <=> 41559 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41560"} Black Hops
UPDATE `quest_template` SET `PrevQuestId` = 41557, `NextQuestId` = 41561, `ExclusiveGroup` = -41558 WHERE `entry` <=> 41560 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41565"} With A Twist Of Magic!
UPDATE `quest_template` SET `NextQuestId` = 41567, `ExclusiveGroup` = -41565 WHERE `entry` <=> 41565 AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41566"} Goldthorn Grog
UPDATE `quest_template` SET `NextQuestId` = 41567, `ExclusiveGroup` = -41565 WHERE `entry` <=> 41566 AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41568"} Great Golden Hops
UPDATE `quest_template` SET `PrevQuestId` = 41567 WHERE `entry` <=> 41568 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41632"} Ring of the Cryptstalker
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 41632 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41633"} Bonescythe Ring
UPDATE `quest_template` SET `PrevQuestId` = 9033 WHERE `entry` <=> 41633 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41637"} School Assistance
UPDATE `quest_template` SET `PrevQuestId` = 41636 WHERE `entry` <=> 41637 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41644"} Defying Odds
UPDATE `quest_template` SET `PrevQuestId` = 41643 WHERE `entry` <=> 41644 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41645"} The Slavemakers
UPDATE `quest_template` SET `PrevQuestId` = 41644 WHERE `entry` <=> 41645 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41646"} Covering All Possibilities
UPDATE `quest_template` SET `PrevQuestId` = 41645 WHERE `entry` <=> 41646 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41647"} Unconventional Means
UPDATE `quest_template` SET `PrevQuestId` = 41646 WHERE `entry` <=> 41647 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41648"} Deathcap And Widow's Frill
UPDATE `quest_template` SET `PrevQuestId` = 41647 WHERE `entry` <=> 41648 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41649"} The Everlook Broadcast Hijacking
UPDATE `quest_template` SET `PrevQuestId` = 41652 WHERE `entry` <=> 41649 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41650"} Crashing Waves, Screeching Thunder
UPDATE `quest_template` SET `PrevQuestId` = 41649 WHERE `entry` <=> 41650 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41651"} Pieces Falling Into Place
UPDATE `quest_template` SET `PrevQuestId` = 41650 WHERE `entry` <=> 41651 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41658"} Destruction of the Dragonmaw
UPDATE `quest_template` SET `PrevQuestId` = 41657 WHERE `entry` <=> 41658 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41664"} A Dark Knight Rises
UPDATE `quest_template` SET `PrevQuestId` = 41663 WHERE `entry` <=> 41664 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41665"} In Amber Disgust
UPDATE `quest_template` SET `PrevQuestId` = 41664 WHERE `entry` <=> 41665 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41669"} Fit for a Prince!
UPDATE `quest_template` SET `PrevQuestId` = 41668 WHERE `entry` <=> 41669 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41680"} Where is Wiggles?
UPDATE `quest_template` SET `PrevQuestId` = 41679 WHERE `entry` <=> 41680 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41681"} Wiggles Knows the Way
UPDATE `quest_template` SET `PrevQuestId` = 41680 WHERE `entry` <=> 41681 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41684"} Shadow's Vision
UPDATE `quest_template` SET `PrevQuestId` = 41648 WHERE `entry` <=> 41684 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41685"} A Veiled Threat
UPDATE `quest_template` SET `PrevQuestId` = 41684 WHERE `entry` <=> 41685 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41686"} Into The Tortured Past
UPDATE `quest_template` SET `PrevQuestId` = 41685 WHERE `entry` <=> 41686 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41690"} Still Not Enough
UPDATE `quest_template` SET `PrevQuestId` = 41689 WHERE `entry` <=> 41690 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41691"} Some May Call It Quackery
UPDATE `quest_template` SET `PrevQuestId` = 41690, `RewItemId1` = 31829, `RewItemCount1` = 1 WHERE `entry` <=> 41691 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41693"} The Last Lines
UPDATE `quest_template` SET `PrevQuestId` = 41692 WHERE `entry` <=> 41693 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41694"} To The Darkest Places
UPDATE `quest_template` SET `PrevQuestId` = 41693 WHERE `entry` <=> 41694 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41695"} Noppsy Spickerspan
UPDATE `quest_template` SET `PrevQuestId` = 41694 WHERE `entry` <=> 41695 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41696"} You May Call This Cheating...
UPDATE `quest_template` SET `PrevQuestId` = 41695, `NextQuestId` = 41699, `ExclusiveGroup` = -41696 WHERE `entry` <=> 41696 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41697"} Demons Galore
UPDATE `quest_template` SET `PrevQuestId` = 41695, `NextQuestId` = 41699, `ExclusiveGroup` = -41696 WHERE `entry` <=> 41697 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41698"} Explosives Make My Heart Go BOOM!
UPDATE `quest_template` SET `PrevQuestId` = 41695, `NextQuestId` = 41699, `ExclusiveGroup` = -41696 WHERE `entry` <=> 41698 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41702"} Tide-ying Up
UPDATE `quest_template` SET `PrevQuestId` = 41701 WHERE `entry` <=> 41702 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41710"} Piece Of A Bigger Picture
UPDATE `quest_template` SET `PrevQuestId` = 41709 WHERE `entry` <=> 41710 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41711"} Calming The Tempest
UPDATE `quest_template` SET `PrevQuestId` = 41710 WHERE `entry` <=> 41711 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41712"} Letter Far From Home
UPDATE `quest_template` SET `RewItemId1` = 6533, `RewItemCount1` = 1 WHERE `entry` <=> 41712 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41714"} The Solution Is Right Here
UPDATE `quest_template` SET `PrevQuestId` = 41713 WHERE `entry` <=> 41714 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41715"} Again Into the Great Ossuary
UPDATE `quest_template` SET `PrevQuestId` = 8258 WHERE `entry` <=> 41715 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41717"} The Dragonmaw Threat
UPDATE `quest_template` SET `PrevQuestId` = 41716 WHERE `entry` <=> 41717 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41718"} The Dragonmaw Orders
UPDATE `quest_template` SET `PrevQuestId` = 41717 WHERE `entry` <=> 41718 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41719"} Death to Geshgan
UPDATE `quest_template` SET `PrevQuestId` = 41718 WHERE `entry` <=> 41719 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41720"} Stolgaz Documents
UPDATE `quest_template` SET `PrevQuestId` = 41718 WHERE `entry` <=> 41720 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41721"} Zarm'geth Stronghold
UPDATE `quest_template` SET `PrevQuestId` = 41719 WHERE `entry` <=> 41721 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41724"} The Betrayal of Rethkag
UPDATE `quest_template` SET `RewChoiceItemId1` = 41789, `RewChoiceItemId2` = 41790, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 41724 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41726"} Scavenging the Wrecks
UPDATE `quest_template` SET `PrevQuestId` = 41725, `RewChoiceItemId1` = 41793, `RewChoiceItemId2` = 41794, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 41726 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41728"} Natural Remedies
UPDATE `quest_template` SET `PrevQuestId` = 41727 WHERE `entry` <=> 41728 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41729"} Dark Essence
UPDATE `quest_template` SET `PrevQuestId` = 41728 WHERE `entry` <=> 41729 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41730"} The Power of Uth'okk
UPDATE `quest_template` SET `PrevQuestId` = 41729 WHERE `entry` <=> 41730 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41731"} The Ritual of Uth'okk
UPDATE `quest_template` SET `PrevQuestId` = 41730, `RewItemId1` = 41798, `RewItemCount1` = 1 WHERE `entry` <=> 41731 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41733"} Rebuilding the Relic
UPDATE `quest_template` SET `PrevQuestId` = 41732, `RewItemId1` = 41801, `RewItemCount1` = 1 WHERE `entry` <=> 41733 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41737"} Signs of Corruption
UPDATE `quest_template` SET `PrevQuestId` = 41736 WHERE `entry` <=> 41737 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41738"} Yor'thegg's Ritual
UPDATE `quest_template` SET `PrevQuestId` = 41737 WHERE `entry` <=> 41738 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41739"} The Flame of Dagnoth
UPDATE `quest_template` SET `PrevQuestId` = 41738 WHERE `entry` <=> 41739 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41740"} The Deed for Dorthas
UPDATE `quest_template` SET `PrevQuestId` = 41739 WHERE `entry` <=> 41740 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41741"} Return to Yor'thegg
UPDATE `quest_template` SET `PrevQuestId` = 41740 WHERE `entry` <=> 41741 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41742"} Expelling Evil
UPDATE `quest_template` SET `PrevQuestId` = 41741, `RewChoiceItemId1` = 41815, `RewChoiceItemId2` = 41816, `RewChoiceItemId3` = 41817, `RewChoiceItemId4` = 41818, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1, `RewChoiceItemCount4` = 1 WHERE `entry` <=> 41742 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41745"} The Power of the Goddess
UPDATE `quest_template` SET `PrevQuestId` = 41394 WHERE `entry` <=> 41745 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41746"} Arcane Emanations
UPDATE `quest_template` SET `RewItemId1` = 2455, `RewItemCount1` = 1 WHERE `entry` <=> 41746 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41753"} A Blaze Unending
UPDATE `quest_template` SET `RewItemId1` = 41836, `RewItemCount1` = 1 WHERE `entry` <=> 41753 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41755"} The Redbrand Lie
UPDATE `quest_template` SET `PrevQuestId` = 41754 WHERE `entry` <=> 41755 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41757"} Blackheart's Demise
UPDATE `quest_template` SET `PrevQuestId` = 474, `RewChoiceItemId1` = 41842, `RewChoiceItemId2` = 41843, `RewChoiceItemId3` = 41844, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 41757 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41761"} Into The Hornet's Nest
UPDATE `quest_template` SET `PrevQuestId` = 41699 WHERE `entry` <=> 41761 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41762"} A Deal Worth Taking
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -2020225, `ReqCreatureOrGOId2` = -2020226, `ReqCreatureOrGOId3` = -2020227, `ReqCreatureOrGOId4` = -2020228 WHERE `entry` <=> 41762 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "41764"} Oil-Based Grievances
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -2020225, `ReqCreatureOrGOId2` = -2020226, `ReqCreatureOrGOId3` = -2020227, `ReqCreatureOrGOId4` = -2020228 WHERE `entry` <=> 41764 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4)) AND (BINARY CAST(`ReqCreatureOrGOId4` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "41766"} Shrillfluke's Early Retirement
UPDATE `quest_template` SET `PrevQuestId` = 41765 WHERE `entry` <=> 41766 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41767"} Rise of the Venture Co.
UPDATE `quest_template` SET `PrevQuestId` = 41766 WHERE `entry` <=> 41767 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41770"} Ride of the Ravasaur
UPDATE `quest_template` SET `PrevQuestId` = 41769 WHERE `entry` <=> 41770 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41776"} Nexus Shattering
UPDATE `quest_template` SET `PrevQuestId` = 41775 WHERE `entry` <=> 41776 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41777"} Nexus Convergence
UPDATE `quest_template` SET `PrevQuestId` = 41775 WHERE `entry` <=> 41777 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41787"} Heart of Darkness
UPDATE `quest_template` SET `PrevQuestId` = 41761 WHERE `entry` <=> 41787 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41789"} Ursan Tribe
UPDATE `quest_template` SET `PrevQuestId` = 41788 WHERE `entry` <=> 41789 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41790"} Reclaiming the Moonwell
UPDATE `quest_template` SET `PrevQuestId` = 41789 WHERE `entry` <=> 41790 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41801"} The Blue Dragonkin
UPDATE `quest_template` SET `PrevQuestId` = 41800 WHERE `entry` <=> 41801 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41802"} Tomb of Ancestors
UPDATE `quest_template` SET `PrevQuestId` = 41801 WHERE `entry` <=> 41802 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41803"} The Skardyn
UPDATE `quest_template` SET `PrevQuestId` = 41802 WHERE `entry` <=> 41803 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41804"} Memories of Dark Iron
UPDATE `quest_template` SET `PrevQuestId` = 41803 WHERE `entry` <=> 41804 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41805"} Bathed in Dragonfire
UPDATE `quest_template` SET `PrevQuestId` = 41804 WHERE `entry` <=> 41805 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41806"} Dragonfire Bombs!
UPDATE `quest_template` SET `PrevQuestId` = 41805 WHERE `entry` <=> 41806 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41807"} I Am Become Death...
UPDATE `quest_template` SET `PrevQuestId` = 41806 WHERE `entry` <=> 41807 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41808"} The Destroyer of Skardyn
UPDATE `quest_template` SET `PrevQuestId` = 41807 WHERE `entry` <=> 41808 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41809"} Smell of Dragonfire
UPDATE `quest_template` SET `PrevQuestId` = 41808 WHERE `entry` <=> 41809 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41810"} Verdant Rune
UPDATE `quest_template` SET `PrevQuestId` = 40990 WHERE `entry` <=> 41810 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41811"} The Farraki Ancient
UPDATE `quest_template` SET `RewChoiceItemId1` = 41916, `RewChoiceItemId2` = 41917, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 41811 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41812"} The Moonshrine Ruins
UPDATE `quest_template` SET `RewItemId1` = 41919, `RewItemCount1` = 1 WHERE `entry` <=> 41812 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41817"} Fear is Your Rival
UPDATE `quest_template` SET `PrevQuestId` = 41816 WHERE `entry` <=> 41817 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41818"} We are not Born Equal
UPDATE `quest_template` SET `PrevQuestId` = 41817 WHERE `entry` <=> 41818 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41820"} To My Father, Vol'jin
UPDATE `quest_template` SET `PrevQuestId` = 41819 WHERE `entry` <=> 41820 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41824"} Mycellakos
UPDATE `quest_template` SET `PrevQuestId` = 41823 WHERE `entry` <=> 41824 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41825"} The Matron Will Know
UPDATE `quest_template` SET `PrevQuestId` = 41824 WHERE `entry` <=> 41825 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41831"} Mere Thoughts
UPDATE `quest_template` SET `PrevQuestId` = 41830 WHERE `entry` <=> 41831 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41832"} Colony of Ants
UPDATE `quest_template` SET `PrevQuestId` = 41831 WHERE `entry` <=> 41832 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41833"} It Can't Rain All the Time
UPDATE `quest_template` SET `PrevQuestId` = 41832 WHERE `entry` <=> 41833 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41834"} Storm's End
UPDATE `quest_template` SET `PrevQuestId` = 41833 WHERE `entry` <=> 41834 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41837"} Storm, Twilight and Hammer
UPDATE `quest_template` SET `PrevQuestId` = 41836 WHERE `entry` <=> 41837 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41841"} Artifact of the Dark Lady
UPDATE `quest_template` SET `PrevQuestId` = 544 WHERE `entry` <=> 41841 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41843"} Assassin In Training
UPDATE `quest_template` SET `PrevQuestId` = 41761 WHERE `entry` <=> 41843 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41848"} To Cure the Withered
UPDATE `quest_template` SET `PrevQuestId` = 41847 WHERE `entry` <=> 41848 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41849"} To Cure the Withered
UPDATE `quest_template` SET `PrevQuestId` = 41848 WHERE `entry` <=> 41849 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41851"} The Gemstone of Naraz
UPDATE `quest_template` SET `PrevQuestId` = 41850 WHERE `entry` <=> 41851 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41856"} Reclaiming Sal'Galaz
UPDATE `quest_template` SET `PrevQuestId` = 41855 WHERE `entry` <=> 41856 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41858"} Gargill the Butcher
UPDATE `quest_template` SET `PrevQuestId` = 41857 WHERE `entry` <=> 41858 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41860"} Emerald Summon
UPDATE `quest_template` SET `PrevQuestId` = 41398 WHERE `entry` <=> 41860 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41861"} The Dreamer's Challenge
UPDATE `quest_template` SET `PrevQuestId` = 41860 WHERE `entry` <=> 41861 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41863"} The Dragonmaw War
UPDATE `quest_template` SET `PrevQuestId` = 41862 WHERE `entry` <=> 41863 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41864"} The Dragonmaw War
UPDATE `quest_template` SET `PrevQuestId` = 41863 WHERE `entry` <=> 41864 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41865"} The Dragonmaw War
UPDATE `quest_template` SET `PrevQuestId` = 41864 WHERE `entry` <=> 41865 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41868"} Shadow Curse
UPDATE `quest_template` SET `PrevQuestId` = 41867 WHERE `entry` <=> 41868 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41870"} Grimscale Revenge
UPDATE `quest_template` SET `PrevQuestId` = 41869 WHERE `entry` <=> 41870 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41877"} Ore Must Flow
UPDATE `quest_template` SET `PrevQuestId` = 41876 WHERE `entry` <=> 41877 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41878"} Ore Must Flow
UPDATE `quest_template` SET `PrevQuestId` = 41877 WHERE `entry` <=> 41878 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41880"} The Dragonmaw War
UPDATE `quest_template` SET `PrevQuestId` = 41865 WHERE `entry` <=> 41880 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41881"} Brangar's Folly
UPDATE `quest_template` SET `PrevQuestId` = 41872 WHERE `entry` <=> 41881 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41882"} The Dragonmaw War
UPDATE `quest_template` SET `PrevQuestId` = 41880 WHERE `entry` <=> 41882 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41884"} Destruction of the Dragonmaw
UPDATE `quest_template` SET `PrevQuestId` = 41883 WHERE `entry` <=> 41884 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41887"} The Recipe of Concoction of the Emerald Mongoose
UPDATE `quest_template` SET `PrevQuestId` = 40873 WHERE `entry` <=> 41887 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41899"} Blackroot Totems
UPDATE `quest_template` SET `PrevQuestId` = 41898 WHERE `entry` <=> 41899 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41906"} No Mercy For The Wicked
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = -3000203 WHERE `entry` <=> 41906 AND (BINARY CAST(`ReqCreatureOrGOId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'2d38333838363038' USING utf8mb4));
-- quest_template {"entry": "41907"} Riverhorn Village
UPDATE `quest_template` SET `RewChoiceItemId1` = 42074, `RewChoiceItemId2` = 42075, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 41907 AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41911"} Wolf in Sheep's Clothing
UPDATE `quest_template` SET `PrevQuestId` = 41910 WHERE `entry` <=> 41911 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41912"} An Ill Omen
UPDATE `quest_template` SET `PrevQuestId` = 41911 WHERE `entry` <=> 41912 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41913"} Draenei Divination
UPDATE `quest_template` SET `PrevQuestId` = 41912 WHERE `entry` <=> 41913 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41914"} Hooves and Horns, Clad in Red
UPDATE `quest_template` SET `PrevQuestId` = 41913 WHERE `entry` <=> 41914 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41915"} Answers from Father
UPDATE `quest_template` SET `PrevQuestId` = 41914 WHERE `entry` <=> 41915 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41916"} The Elder's End
UPDATE `quest_template` SET `PrevQuestId` = 41915 WHERE `entry` <=> 41916 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41917"} A Student's Determination
UPDATE `quest_template` SET `PrevQuestId` = 41916 WHERE `entry` <=> 41917 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41919"} More Silk for the Wounded
UPDATE `quest_template` SET `PrevQuestId` = 41918 WHERE `entry` <=> 41919 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41921"} A Familiar Disturbance
UPDATE `quest_template` SET `PrevQuestId` = 41322 WHERE `entry` <=> 41921 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41922"} Homecoming
UPDATE `quest_template` SET `PrevQuestId` = 41921 WHERE `entry` <=> 41922 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41931"} The Corruption of Timbermaw Hold
UPDATE `quest_template` SET `PrevQuestId` = 41930 WHERE `entry` <=> 41931 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41932"} Ancient Furbolg Remedies
UPDATE `quest_template` SET `PrevQuestId` = 41931 WHERE `entry` <=> 41932 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41933"} Unbridled Darkness
UPDATE `quest_template` SET `PrevQuestId` = 41932 WHERE `entry` <=> 41933 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41934"} What Remains
UPDATE `quest_template` SET `PrevQuestId` = 41933 WHERE `entry` <=> 41934 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41939"} Vortalus’ Edict
UPDATE `quest_template` SET `PrevQuestId` = 41938 WHERE `entry` <=> 41939 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41942"} The Plaguewood
UPDATE `quest_template` SET `PrevQuestId` = 41941 WHERE `entry` <=> 41942 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41949"} Horns of their Allies
UPDATE `quest_template` SET `PrevQuestId` = 41914 WHERE `entry` <=> 41949 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41950"} Parcel to Nowhere
UPDATE `quest_template` SET `PrevQuestId` = 41913 WHERE `entry` <=> 41950 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41951"} Merchant’s Knowledge
UPDATE `quest_template` SET `PrevQuestId` = 41950 WHERE `entry` <=> 41951 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41952"} Out of the Moonlight
UPDATE `quest_template` SET `PrevQuestId` = 41951 WHERE `entry` <=> 41952 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41953"} Draenethyst Recovery
UPDATE `quest_template` SET `PrevQuestId` = 41952 WHERE `entry` <=> 41953 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41955"} Tricolored Hide-ra
UPDATE `quest_template` SET `PrevQuestId` = 41954 WHERE `entry` <=> 41955 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41957"} The Root of it All
UPDATE `quest_template` SET `PrevQuestId` = 41956 WHERE `entry` <=> 41957 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41958"} Emerald Aid
UPDATE `quest_template` SET `PrevQuestId` = 41957 WHERE `entry` <=> 41958 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41959"} The Flute of Spirits
UPDATE `quest_template` SET `PrevQuestId` = 41958 WHERE `entry` <=> 41959 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41960"} One Good Turn Deserves Another
UPDATE `quest_template` SET `PrevQuestId` = 41959 WHERE `entry` <=> 41960 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41961"} Hidden in Plain Sight
UPDATE `quest_template` SET `PrevQuestId` = 41960 WHERE `entry` <=> 41961 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41962"} Fangorn, Lord of Eternity
UPDATE `quest_template` SET `PrevQuestId` = 41961 WHERE `entry` <=> 41962 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41964"} Buried in Ancient Ruins
UPDATE `quest_template` SET `PrevQuestId` = 41962 WHERE `entry` <=> 41964 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41965"} Purging Flames
UPDATE `quest_template` SET `PrevQuestId` = 41964 WHERE `entry` <=> 41965 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41966"} Peroth’arn, Nightmare’s Herald
UPDATE `quest_template` SET `PrevQuestId` = 41965 WHERE `entry` <=> 41966 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41972"} Report to Moonhoof Village
UPDATE `quest_template` SET `PrevQuestId` = 41970 WHERE `entry` <=> 41972 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41974"} Glimmering Hydra Scale
UPDATE `quest_template` SET `PrevQuestId` = 41973, `RewChoiceItemId1` = 42259, `RewChoiceItemId2` = 42260, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1 WHERE `entry` <=> 41974 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41975"} Zalwan's Cut
UPDATE `quest_template` SET `PrevQuestId` = 41974 WHERE `entry` <=> 41975 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41977"} Relics of the Windhorn Tribe
UPDATE `quest_template` SET `RewItemId1` = 42263, `RewItemCount1` = 1 WHERE `entry` <=> 41977 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41979"} Rumors of the Deathtotem
UPDATE `quest_template` SET `PrevQuestId` = 5064 WHERE `entry` <=> 41979 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41981"} Information for Cairne
UPDATE `quest_template` SET `PrevQuestId` = 41979 WHERE `entry` <=> 41981 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41982"} Destroy the Deathtotem
UPDATE `quest_template` SET `PrevQuestId` = 41981 WHERE `entry` <=> 41982 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "41995"} Mindless Monster
UPDATE `quest_template` SET `PrevQuestId` = 41994 WHERE `entry` <=> 41995 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42003"} The Silver Blade
UPDATE `quest_template` SET `PrevQuestId` = 42002 WHERE `entry` <=> 42003 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42004"} Drenched in Power
UPDATE `quest_template` SET `PrevQuestId` = 42003 WHERE `entry` <=> 42004 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42005"} The Rift Calls
UPDATE `quest_template` SET `PrevQuestId` = 42004 WHERE `entry` <=> 42005 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42007"} The Shattered Disc
UPDATE `quest_template` SET `PrevQuestId` = 42006 WHERE `entry` <=> 42007 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42011"} Collectors of Draenethyst
UPDATE `quest_template` SET `PrevQuestId` = 41998 WHERE `entry` <=> 42011 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42012"} Actual Collector of Draenethyst
UPDATE `quest_template` SET `PrevQuestId` = 42011 WHERE `entry` <=> 42012 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42014"} Roots of the Grove
UPDATE `quest_template` SET `PrevQuestId` = 42013 WHERE `entry` <=> 42014 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42015"} Lady Stargazer
UPDATE `quest_template` SET `PrevQuestId` = 42013 WHERE `entry` <=> 42015 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42016"} In Lucid Dreams
UPDATE `quest_template` SET `PrevQuestId` = 42013 WHERE `entry` <=> 42016 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42018"} Gifting the Matron
UPDATE `quest_template` SET `PrevQuestId` = 42017 WHERE `entry` <=> 42018 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42020"} Brother’s Duty
UPDATE `quest_template` SET `PrevQuestId` = 42019 WHERE `entry` <=> 42020 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42021"} Essence of Purification
UPDATE `quest_template` SET `PrevQuestId` = 41933 WHERE `entry` <=> 42021 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42022"} Hide of a Wild God
UPDATE `quest_template` SET `PrevQuestId` = 41933 WHERE `entry` <=> 42022 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42030"} Larceny and Extortion
UPDATE `quest_template` SET `PrevQuestId` = 42029 WHERE `entry` <=> 42030 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42031"} Elementium Lock
UPDATE `quest_template` SET `PrevQuestId` = 42030 WHERE `entry` <=> 42031 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42032"} Lock Blueprints: Crimson Friar Freidhelm
UPDATE `quest_template` SET `PrevQuestId` = 42031, `NextQuestId` = 42034, `ExclusiveGroup` = -42032 WHERE `entry` <=> 42032 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42033"} Lock Blueprints: Sorcerer-Thane Thaurissan
UPDATE `quest_template` SET `PrevQuestId` = 42031, `NextQuestId` = 42034, `ExclusiveGroup` = -42032 WHERE `entry` <=> 42033 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`NextQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`ExclusiveGroup` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42035"} Lock and Loaded
UPDATE `quest_template` SET `PrevQuestId` = 42034 WHERE `entry` <=> 42035 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42036"} Secrets of Ten Thousand Years
UPDATE `quest_template` SET `PrevQuestId` = 42035, `RewItemId1` = 42319, `RewItemId2` = 33330, `RewItemId3` = 33331, `RewItemCount1` = 1, `RewItemCount2` = 1, `RewItemCount3` = 1 WHERE `entry` <=> 42036 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42039"} Chieftain Ubukaz
UPDATE `quest_template` SET `PrevQuestId` = 42038 WHERE `entry` <=> 42039 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42041"} To Survive in the Jungle
UPDATE `quest_template` SET `RequiredSkill` = 142, `RequiredSkillValue` = 200 WHERE `entry` <=> 42041 AND (BINARY CAST(`RequiredSkill` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RequiredSkillValue` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42045"} Tainted Rune
UPDATE `quest_template` SET `PrevQuestId` = 179 WHERE `entry` <=> 42045 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42048"} Rainhorn’s Frustration
UPDATE `quest_template` SET `PrevQuestId` = 42047 WHERE `entry` <=> 42048 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42049"} In Need of Water
UPDATE `quest_template` SET `PrevQuestId` = 42001 WHERE `entry` <=> 42049 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42050"} In Water, Clarity
UPDATE `quest_template` SET `PrevQuestId` = 42049 WHERE `entry` <=> 42050 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42051"} Bound in Stone
UPDATE `quest_template` SET `PrevQuestId` = 42050 WHERE `entry` <=> 42051 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42053"} Jawtaker
UPDATE `quest_template` SET `PrevQuestId` = 42052 WHERE `entry` <=> 42053 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42054"} Hallowed Note
UPDATE `quest_template` SET `PrevQuestId` = 747 WHERE `entry` <=> 42054 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42056"} Garments of the Three Siblings
UPDATE `quest_template` SET `PrevQuestId` = 42055 WHERE `entry` <=> 42056 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42069"} The Withered Den
UPDATE `quest_template` SET `PrevQuestId` = 42068 WHERE `entry` <=> 42069 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42070"} One Heir to Another
UPDATE `quest_template` SET `PrevQuestId` = 41995 WHERE `entry` <=> 42070 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42071"} Father Will Listen
UPDATE `quest_template` SET `PrevQuestId` = 42070 WHERE `entry` <=> 42071 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42072"} Bloodhoof Stands with Moonhoof
UPDATE `quest_template` SET `PrevQuestId` = 42071 WHERE `entry` <=> 42072 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42073"} Facing the Elder
UPDATE `quest_template` SET `PrevQuestId` = 42072 WHERE `entry` <=> 42073 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42074"} Seeking the Truth
UPDATE `quest_template` SET `PrevQuestId` = 42073 WHERE `entry` <=> 42074 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42075"} Price of Betrayal
UPDATE `quest_template` SET `PrevQuestId` = 42080 WHERE `entry` <=> 42075 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42076"} Return to the Dream
UPDATE `quest_template` SET `PrevQuestId` = 42075 WHERE `entry` <=> 42076 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42077"} Mothshroud Falls
UPDATE `quest_template` SET `PrevQuestId` = 42076 WHERE `entry` <=> 42077 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42078"} Moonhoof Rests
UPDATE `quest_template` SET `PrevQuestId` = 42077 WHERE `entry` <=> 42078 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42079"} Secrets of Moonwhisper
UPDATE `quest_template` SET `RewItemId1` = 42386, `RewItemCount1` = 1 WHERE `entry` <=> 42079 AND (BINARY CAST(`RewItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42080"} Moondancer Lives?
UPDATE `quest_template` SET `PrevQuestId` = 42074 WHERE `entry` <=> 42080 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42082"} Starstrider Headdress
UPDATE `quest_template` SET `PrevQuestId` = 42081 WHERE `entry` <=> 42082 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42083"} The Rod of Preservation
UPDATE `quest_template` SET `PrevQuestId` = 42082 WHERE `entry` <=> 42083 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42084"} A Tale of Scales
UPDATE `quest_template` SET `PrevQuestId` = 42083 WHERE `entry` <=> 42084 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42085"} The Moonhoof Celebration
UPDATE `quest_template` SET `PrevQuestId` = 42084, `RewChoiceItemId1` = 42391, `RewChoiceItemId2` = 42392, `RewChoiceItemId3` = 42393, `RewChoiceItemCount1` = 1, `RewChoiceItemCount2` = 1, `RewChoiceItemCount3` = 1 WHERE `entry` <=> 42085 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemId3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount1` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount2` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4)) AND (BINARY CAST(`RewChoiceItemCount3` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42087"} The Light of Elunaris
UPDATE `quest_template` SET `PrevQuestId` = 42086 WHERE `entry` <=> 42087 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42090"} Serpents Without Heads
UPDATE `quest_template` SET `PrevQuestId` = 42089 WHERE `entry` <=> 42090 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42091"} Word to the High Priestess
UPDATE `quest_template` SET `PrevQuestId` = 42090 WHERE `entry` <=> 42091 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42095"} A Dryad’s Counsel
UPDATE `quest_template` SET `PrevQuestId` = 42094 WHERE `entry` <=> 42095 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42096"} The Rot of Elun’aran
UPDATE `quest_template` SET `PrevQuestId` = 42095 WHERE `entry` <=> 42096 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
-- quest_template {"entry": "42097"} Keeper of the Broken Grove
UPDATE `quest_template` SET `PrevQuestId` = 42096 WHERE `entry` <=> 42097 AND (BINARY CAST(`PrevQuestId` AS CHAR CHARACTER SET utf8mb4) <=> BINARY CONVERT(X'30' USING utf8mb4));
