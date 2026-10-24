-- broadcast_text 92031 points at an emote the client does not have.
-- Ported from Vit Barabas's twow-core 20260906134216_world.sql.
--
--     BroadcastText (Id: 92031) ... has emoteId2 55 but emote does not exist.
--
-- (The message names the wrong field - it is emote_id1.)
-- Emotes.dbc has no emote 55. ObjectMgr already sets it to 0 in memory on every
-- start; this stores the same value so the startup warning stops. No change in
-- game. The old value is in the WHERE clause so a re-run cannot overwrite a
-- later correction.
UPDATE `broadcast_text` SET `emote_id1` = 0
WHERE `entry` = 92031 AND `emote_id1` = 55;
