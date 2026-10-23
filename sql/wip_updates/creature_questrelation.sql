-- Michael Duguder gives the honor dailies Slay the Dishonorable (39980) and
-- Slay the Honorable (39981) again, from his restored entry 60084.
UPDATE `creature_questrelation` SET `id` = 60084 WHERE `id` = 60053 AND `quest` IN (39980, 39981);
