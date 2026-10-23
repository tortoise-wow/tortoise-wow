-- Tower of Karazhan (map 814) entrance and exit. Both triggers are in
-- areatrigger_template already but had no teleport rows, so the door did
-- nothing. The entrance needs level 60 and the Upper Karazhan Tower Key
-- (condition 61234); the exit lands just outside the upper door, on the
-- bridge, facing away from it and clear of the entrance trigger.
DELETE FROM `areatrigger_teleport` WHERE `id` IN (5340, 5341);
INSERT INTO `areatrigger_teleport` (`id`, `name`, `message`, `required_level`, `required_condition`, `required_phase`, `target_map`, `target_position_x`, `target_position_y`, `target_position_z`, `target_orientation`) VALUES
(5340, 'Tower of Karazhan - Entrance', 'You must be part of a Raid group, at least level 60 and have the Upper Karazhan Tower Key to enter.', 60, 61234, 0, 814, -11048.2, -1985.84, 101.574, 2.1964),
(5341, 'Tower of Karazhan - Exit', '', 0, 0, 0, 0, -11033.6, -2005.54, 93.417, 5.4835);
