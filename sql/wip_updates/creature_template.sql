-- Michael Duguder: restore his template under a new entry, 60084.
-- 20260510092659_world.sql reused his entry 60053 for quest 41240's credit
-- trigger (quest_41240_dummy_triger), overwriting his name, model, level and
-- script. 60053 stays the trigger: the quest's script credits it with
-- KilledMonster(GetCreatureTemplate(60053)) and needs no spawn.
-- Row below is his original from sql/base, with only the entry changed.
DELETE FROM `creature_template` WHERE `entry` = 60084;
INSERT INTO `creature_template` VALUES
(60084,4450,0,0,0,0,'Michael Duguder','Peace Activist',0,60,60,1207,1256,1097,1142,1280,35,2,1,1.14286,0,18,5,0,0,1,48,60,0,144,1,2000,2000,2,0,0,0,0,0,0,0,53.8384,74.0278,100,7,0,942,942,0,0,0,0,0,0,0,6364,370,0,0,9420,0,0,NULL,0,0,'',0,3,0,0,3,0,0,0,0,0,0,0,0,'npc_daily_hk_dk');
