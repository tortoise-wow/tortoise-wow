-- Existing character-storage migration: run manually against the selected character schema.
-- Source: the character CREATE TABLE definitions in sql/create_databases.sql.
-- OFFLINE PREREQUISITES: stop and drain world, realm, SOAP, batch and administrator writers.
-- Take a complete, consistent backup and verify its restore into a separate private database.
-- Rehearse this exact conversion against the restored rows and compare row counts, logical
-- checksums, AUTO_INCREMENT values, indexes, columns and application integrity queries.
-- Review FULLTEXT/prefix indexes, zero dates, uniqueness/corruption and available disk space.
-- DDL auto-commits: stop on the first error; ROLLBACK cannot undo these engine changes.
-- Keep writers stopped through post-migration verification and any manual recovery decision.
-- Select the character database explicitly; this file deliberately contains no USE directive.
-- Do not run this script as a live startup update or with a client --force option.
--
-- Convert the 54 active MyISAM persistence tables. ROW_FORMAT=DYNAMIC replaces the
-- MyISAM FIXED format that InnoDB cannot inherit. Existing fields and keys are retained.
-- Existing InnoDB tables need no conversion. Preserve these five historical copies:
-- character_bck, character_duplicate_account, character_reputation_broken,
-- character_skills_copy, characters_namecleanup2020_2. Runtime saves must not use them.

ALTER TABLE `auction` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `bugreport` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `characters` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_action` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_battleground_data` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_bgqueue` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_forgotten_skills` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_gifts` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_homebind` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_honor_cp` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_instance` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_inventory` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_pet` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_queststatus` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_reputation` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_skills` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_social` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_spell` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_spell_cooldown` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_spell_dual_spec` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_stats` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_ticket` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_titles` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `character_tutorial` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `corpse` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `game_event_status` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `gm_subsurveys` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `gm_surveys` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `gm_tickets` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `groups` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `group_instance` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `group_member` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `guild` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `guild_bank_money` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `guild_bank_tabs` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `guild_eventlog` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `guild_house` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `guild_member` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `guild_rank` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `instance` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `instance_reset` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `item_loot` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `item_text` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `item_transmogs` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `logs_warden` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `mail_items` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `petition` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `petition_sign` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `pet_spell` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `pet_spell_cooldown` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `saved_variables` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `variables` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `world` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
ALTER TABLE `worldstates` ENGINE=InnoDB, ROW_FORMAT=DYNAMIC;
