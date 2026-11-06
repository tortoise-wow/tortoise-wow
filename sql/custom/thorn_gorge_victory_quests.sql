-- Thorn Gorge victory quests require native event credit before turn-in.
-- Preserve other quest flags, objectives and rewards. Apply before restarting
-- with the corresponding BG implementation. No character progress is reset.
UPDATE quest_template SET SpecialFlags = SpecialFlags | 2
WHERE entry IN (42098, 42099);
