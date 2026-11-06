-- Optional Thorn Gorge prototype. Apply to the TEST WORLD database only.
-- Requires the accompanying binary and Battleground.ThornGorge.Enabled = 1.
-- These tables include MyISAM: do not assume START TRANSACTION can roll back.
-- Plain INSERT intentionally fails on an existing/conflicting entry.
-- First test: one player per team; raise min_players_per_team to 5 after testing.
INSERT INTO battleground_template
    (id,min_players_per_team,max_players_per_team,min_level,max_level,
     alliance_win_spell,alliance_lose_spell,horde_win_spell,horde_lose_spell,
     alliance_start_location,horde_start_location,player_loot_id)
VALUES (6,1,15,31,60,0,0,0,0,165,166,0);

INSERT INTO battlemaster_entry (entry,bg_template)
VALUES (63203,6),(63204,6),(63205,6),(63206,6),(63207,6),(63208,6);

INSERT INTO world_safe_locs_facing (id,orientation)
VALUES (165,3.14159265),(166,0);

-- No character/account schema or data changes. Native battleground instances
-- create objectives and spirit guides; no persistent map spawns are needed.
-- Existing quest records are retained; this prototype does not define a new
-- quest completion contract, vendor unlock, reputation or mark reward.
