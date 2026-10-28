-- The imported Thorn Gorge quests still used the obsolete zone 5642.
-- Map 821 uses native client/server area 5722. Preserve objectives/rewards.
UPDATE quest_template SET ZoneOrSort = 5722
WHERE entry IN (42098, 42099) AND ZoneOrSort = 5642;
