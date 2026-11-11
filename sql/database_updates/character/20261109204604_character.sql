-- Move transmog template IDs from 1 to 100000 up by at least 100000, so they
-- no longer overlap real item IDs. This is safe to re-run because it only
-- matches IDs up to 100000.

SET @shift := GREATEST(100000,
    COALESCE((SELECT MAX(`ID`) FROM `item_transmogs`), 0),
    COALESCE((SELECT MAX(`transmogrifyId`) FROM `item_instance`), 0),
    COALESCE((SELECT MAX(`transmogrifyId`) FROM `guild_bank`), 0),
    COALESCE((SELECT MAX(`transmogrifyId`) FROM `guild_bank_analysis`), 0));

-- Items outside of guild banks.
UPDATE `item_instance` SET `transmogrifyId` = `transmogrifyId` + @shift WHERE `transmogrifyId` BETWEEN 1 AND 100000;

-- Items in guild banks and their analysis copies.
UPDATE `guild_bank` SET `transmogrifyId` = `transmogrifyId` + @shift WHERE `transmogrifyId` BETWEEN 1 AND 100000;
UPDATE `guild_bank_analysis` SET `transmogrifyId` = `transmogrifyId` + @shift WHERE `transmogrifyId` BETWEEN 1 AND 100000;

-- `item_transmogs` cannot be rolled back, so the templates are moved last.
UPDATE `item_transmogs` SET `ID` = `ID` + @shift WHERE `ID` BETWEEN 1 AND 100000;
