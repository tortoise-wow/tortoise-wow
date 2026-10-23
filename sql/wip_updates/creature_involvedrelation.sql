-- Michael Duguder takes back the honor dailies 39980 and 39981 at his
-- restored entry 60084.
UPDATE `creature_involvedrelation` SET `id` = 60084 WHERE `id` = 60053 AND `quest` IN (39980, 39981);
