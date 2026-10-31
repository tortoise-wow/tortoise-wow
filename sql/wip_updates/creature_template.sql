-- Trainers nobody can train from.
-- Ported from Shyalya's twow-core 20260802160000_world.sql
-- (trainer_type_zero_class_zero.sql).
--
-- trainer_type 0 is TRAINER_TYPE_CLASS, and Creature::IsTrainerOf turns away
-- any player whose class differs from trainer_class. With trainer_class 0 that
-- is every player, so these 14 NPCs have a trainer flag and a spell list that
-- nobody can ever open.

-- Profession trainers: TRAINER_TYPE_TRADESKILLS (2)
UPDATE `creature_template`
SET `trainer_type` = 2
WHERE `entry` IN (
    8696,   -- Henry Stern (cooking)
    11557,  -- Meilosh, Timbermaw Hold Quartermaster (tailoring)
    60510,  -- Grubgar, Fisher Orc
    61277,  -- Todd Bolder (blacksmithing)
    61438,  -- Cassie Copperlight, Aspiring Tinkerer (engineering)
    62143,  -- Smith Martin, Expert Blacksmith
    62282,  -- Linus Huxley, Chef (cooking)
    62286,  -- Pinky Tosslehouse, Herbalist
    62290,  -- Enchantress Magilou, Expert Enchanter
    62299,  -- The Witch of Northwind (alchemy)
    63056,  -- Ulf Stonetotem, Artisan Blacksmith
    80320   -- Rodfather, The Eel Hunter (fishing)
);

-- Class trainers: give them their class
UPDATE `creature_template`
SET `trainer_class` = 3
WHERE `entry` = 61640;  -- Marksman Rembrandt Olar, Hunter Trainer

UPDATE `creature_template`
SET `trainer_class` = 8
WHERE `entry` = 80213;  -- Magistrix Ishalah, Portal Trainer
