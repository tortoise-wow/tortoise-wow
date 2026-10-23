-- Michael Duguder: point his two spawns (Stormwind, Thunder Bluff) at his
-- restored entry 60084. They showed as a visible quest_41240_dummy_triger.
UPDATE `creature` SET `id` = 60084 WHERE `id` = 60053 AND `guid` IN (332778, 332779);
