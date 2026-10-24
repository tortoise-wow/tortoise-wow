-- Duke Dreadmoore (61972) speaks without a voice.
-- Ported from Vit Barabas's twow-core 20260906134216_world.sql.
--
--     BroadcastText (Id: 30237) ... has SoundId 60443 but sound does not exist.
--     BroadcastText (Id: 30238) ... has SoundId 60444 but sound does not exist.
--
-- ObjectMgr drops a sound id that `sound_entries` does not know and plays the
-- line silently. Both ids exist in the client's SoundEntries.dbc - 60443 is
-- Dukedread1.mp3 and 60444 is Dukedread2.mp3, under Sound\Interface\VA with
-- the other custom voice lines. Only the server-side row is missing, so the
-- recordings ship with the client but never play. The names are the ones the
-- DBC itself carries.
INSERT IGNORE INTO `sound_entries` (`id`, `name`) VALUES
(60443, 'Dukedread1'),
(60444, 'Dukedread2');
