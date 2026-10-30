-- Holding the Upper Karazhan Tower Key (61234), for the Tower of Karazhan
-- entrance (areatrigger 5340). The key goes on the keyring, which
-- CONDITION_ITEM counts.
DELETE FROM `conditions` WHERE `condition_entry` = 61234;
INSERT INTO `conditions` (`condition_entry`, `type`, `value1`, `value2`, `value3`, `value4`, `flags`) VALUES
(61234, 2, 61234, 1, 0, 0, 0);
