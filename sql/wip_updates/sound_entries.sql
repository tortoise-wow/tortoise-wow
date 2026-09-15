-- BroadcastText entries 30237/30238 ("I am the will of Naxxramas!" / "You
-- will learn nothing from me fools!") reference sound_id 60443/60444, which
-- have no matching row in sound_entries:
--
--     BroadcastText (Id: 30237) in table `broadcast_text` has SoundId 60443 but sound does not exist.
--     BroadcastText (Id: 30238) in table `broadcast_text` has SoundId 60444 but sound does not exist.
--
-- sound_entries only documents that an id is known-good (id, name); the
-- audio itself plays client-side once the id reaches it, resolved against
-- the client's own SoundEntries.dbc. Both ids are real there:
--
--     60443  Dukedread1  (Dukedread1.mp3)
--     60444  Dukedread2  (Dukedread2.mp3)
--
-- They were simply never carried into sql/base/tw_world_sound_entries.sql,
-- unlike neighboring custom content (e.g. the Zel'jeb the Ancient encounter
-- added its own 60512 'Zeljeb_aggro' etc. alongside that content).
INSERT INTO `sound_entries` (`id`, `name`) VALUES
(60443, 'Dukedread1'),
(60444, 'Dukedread2');
