/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19  Distrib 10.6.22-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: tw_char
-- ------------------------------------------------------
-- Server version	10.6.22-MariaDB-0ubuntu0.22.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `tw_char`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `tw_char` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */;

USE `tw_char`;

-- Active character persistence uses InnoDB so failed multi-table saves can roll back.
-- Historical archival copies keep their original engines and must remain read-only.

--
-- Table structure for table `account_data`
--

DROP TABLE IF EXISTS `account_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_data` (
  `account` int(10) unsigned NOT NULL DEFAULT 0,
  `type` int(10) unsigned NOT NULL DEFAULT 0,
  `time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `data` longblob NOT NULL,
  PRIMARY KEY (`account`,`type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `arena_stats_single`
--

DROP TABLE IF EXISTS `arena_stats_single`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `arena_stats_single` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `arena_id` int(10) unsigned NOT NULL DEFAULT 0,
  `team_id` int(10) unsigned NOT NULL DEFAULT 0,
  `level` int(10) unsigned NOT NULL DEFAULT 0,
  `item_level` int(10) unsigned NOT NULL DEFAULT 0,
  `class` int(10) unsigned NOT NULL DEFAULT 0,
  `race` int(10) unsigned NOT NULL DEFAULT 0,
  `won` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `duration` int(10) unsigned NOT NULL DEFAULT 0,
  `date` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auction`
--

DROP TABLE IF EXISTS `auction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `auction` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `houseid` int(10) unsigned NOT NULL DEFAULT 0,
  `itemguid` int(10) unsigned NOT NULL DEFAULT 0,
  `item_template` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Item Identifier',
  `itemowner` int(10) unsigned NOT NULL DEFAULT 0,
  `buyoutprice` int(11) NOT NULL DEFAULT 0,
  `time` bigint(20) NOT NULL DEFAULT 0,
  `buyguid` int(10) unsigned NOT NULL DEFAULT 0,
  `lastbid` int(11) NOT NULL DEFAULT 0,
  `startbid` int(11) NOT NULL DEFAULT 0,
  `deposit` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `key_item_guid` (`itemguid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bounty_quest_targets`
--

DROP TABLE IF EXISTS `bounty_quest_targets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `bounty_quest_targets` (
  `id` int(10) unsigned NOT NULL DEFAULT 1,
  `horde_player` int(10) unsigned NOT NULL DEFAULT 0,
  `alliance_player` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bugreport`
--

DROP TABLE IF EXISTS `bugreport`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `bugreport` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `type` longtext NOT NULL,
  `content` longtext NOT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Debug System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `bugreports`
--

DROP TABLE IF EXISTS `bugreports`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `bugreports` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `reportTime` timestamp NULL DEFAULT current_timestamp(),
  `playerGuid` int(11) DEFAULT NULL,
  `playerMap` int(11) DEFAULT NULL,
  `playerInstanceId` int(11) DEFAULT NULL,
  `playerX` float DEFAULT NULL,
  `playerY` float DEFAULT NULL,
  `playerZ` float DEFAULT NULL,
  `clientIp` varchar(100) DEFAULT NULL,
  `reportType` tinyint(4) DEFAULT NULL,
  `reportText` text DEFAULT NULL,
  `serverInformation` text DEFAULT NULL,
  `bugStatus` enum('New','NeedTest','Fixed','Invalid','Duplicate','Confirmed') NOT NULL DEFAULT 'New',
  KEY `idx_id` (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `census`
--

DROP TABLE IF EXISTS `census`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `census` (
  `time` datetime DEFAULT NULL,
  `race` int(11) DEFAULT NULL,
  `onlineCount` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_account_data`
--

DROP TABLE IF EXISTS `character_account_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_account_data` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `type` int(10) unsigned NOT NULL DEFAULT 0,
  `time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `data` longblob NOT NULL,
  PRIMARY KEY (`guid`,`type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_action`
--

DROP TABLE IF EXISTS `character_action`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_action` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `button` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `action` int(10) unsigned NOT NULL DEFAULT 0,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`button`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_armory_stats`
--

DROP TABLE IF EXISTS `character_armory_stats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_armory_stats` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `maxhealth` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower1` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower2` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower3` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower4` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower5` int(10) unsigned NOT NULL DEFAULT 0,
  `strength` float NOT NULL DEFAULT 0,
  `agility` float NOT NULL DEFAULT 0,
  `stamina` float NOT NULL DEFAULT 0,
  `intellect` float NOT NULL DEFAULT 0,
  `spirit` float NOT NULL DEFAULT 0,
  `armor` int(10) unsigned NOT NULL DEFAULT 0,
  `resHoly` int(10) unsigned NOT NULL DEFAULT 0,
  `resFire` int(10) unsigned NOT NULL DEFAULT 0,
  `resNature` int(10) unsigned NOT NULL DEFAULT 0,
  `resFrost` int(10) unsigned NOT NULL DEFAULT 0,
  `resShadow` int(10) unsigned NOT NULL DEFAULT 0,
  `resArcane` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgModNormal` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgModHoly` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgModFire` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgModNature` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgModFrost` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgModShadow` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgModArcane` int(10) unsigned NOT NULL DEFAULT 0,
  `blockPct` float NOT NULL DEFAULT 0,
  `dodgePct` float NOT NULL DEFAULT 0,
  `parryPct` float NOT NULL DEFAULT 0,
  `meleeCritPct` float NOT NULL DEFAULT 0,
  `rangedCritPct` float NOT NULL DEFAULT 0,
  `attackPower` float NOT NULL DEFAULT 0,
  `rangedAttackPower` float NOT NULL DEFAULT 0,
  `meleeDamage` text NOT NULL,
  `rangedDamage` text NOT NULL,
  `meleeWeaponSpeed` float NOT NULL DEFAULT 0,
  `rangedWeaponSpeed` float NOT NULL DEFAULT 0,
  `castSpeed` float NOT NULL DEFAULT 0,
  `meleeHit` float NOT NULL DEFAULT 0,
  `rangedHit` float NOT NULL DEFAULT 0,
  `spellHit` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_aura`
--

DROP TABLE IF EXISTS `character_aura`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_aura` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `caster_guid` bigint(20) unsigned NOT NULL DEFAULT 0 COMMENT 'Full Global Unique Identifier',
  `item_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `spell` int(10) unsigned NOT NULL DEFAULT 0,
  `stackcount` int(10) unsigned NOT NULL DEFAULT 1,
  `remaincharges` int(10) unsigned NOT NULL DEFAULT 0,
  `basepoints0` int(11) NOT NULL DEFAULT 0,
  `basepoints1` int(11) NOT NULL DEFAULT 0,
  `basepoints2` int(11) NOT NULL DEFAULT 0,
  `periodictime0` int(10) unsigned NOT NULL DEFAULT 0,
  `periodictime1` int(10) unsigned NOT NULL DEFAULT 0,
  `periodictime2` int(10) unsigned NOT NULL DEFAULT 0,
  `maxduration` int(11) NOT NULL DEFAULT 0,
  `remaintime` int(11) NOT NULL DEFAULT 0,
  `effIndexMask` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`caster_guid`,`item_guid`,`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_aura_suspended`
--

DROP TABLE IF EXISTS `character_aura_suspended`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_aura_suspended` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `caster_guid` bigint(20) unsigned NOT NULL DEFAULT 0 COMMENT 'Full Global Unique Identifier',
  `item_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `spell` int(10) unsigned NOT NULL DEFAULT 0,
  `stackcount` int(10) unsigned NOT NULL DEFAULT 1,
  `remaincharges` int(10) unsigned NOT NULL DEFAULT 0,
  `basepoints0` int(11) NOT NULL DEFAULT 0,
  `basepoints1` int(11) NOT NULL DEFAULT 0,
  `basepoints2` int(11) NOT NULL DEFAULT 0,
  `periodictime0` int(10) unsigned NOT NULL DEFAULT 0,
  `periodictime1` int(10) unsigned NOT NULL DEFAULT 0,
  `periodictime2` int(10) unsigned NOT NULL DEFAULT 0,
  `maxduration` int(11) NOT NULL DEFAULT 0,
  `remaintime` int(11) NOT NULL DEFAULT 0,
  `effIndexMask` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`caster_guid`,`item_guid`,`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_battleground_data`
--

DROP TABLE IF EXISTS `character_battleground_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_battleground_data` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `instance_id` int(10) unsigned NOT NULL DEFAULT 0,
  `team` int(10) unsigned NOT NULL DEFAULT 0,
  `join_x` float NOT NULL DEFAULT 0,
  `join_y` float NOT NULL DEFAULT 0,
  `join_z` float NOT NULL DEFAULT 0,
  `join_o` float NOT NULL DEFAULT 0,
  `join_map` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_bck`
--

DROP TABLE IF EXISTS `character_bck`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_bck` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `account` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Account Identifier',
  `name` varchar(12) NOT NULL DEFAULT '',
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `gender` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `xp` int(10) unsigned NOT NULL DEFAULT 0,
  `money` int(10) unsigned NOT NULL DEFAULT 0,
  `playerBytes` int(10) unsigned NOT NULL DEFAULT 0,
  `playerBytes2` int(10) unsigned NOT NULL DEFAULT 0,
  `playerFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `map` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Map Identifier',
  `orientation` float NOT NULL DEFAULT 0,
  `taximask` longtext DEFAULT NULL,
  `online` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `cinematic` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `totaltime` int(10) unsigned NOT NULL DEFAULT 0,
  `leveltime` int(10) unsigned NOT NULL DEFAULT 0,
  `logout_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `is_logout_resting` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `rest_bonus` float NOT NULL DEFAULT 0,
  `resettalents_multiplier` int(10) unsigned NOT NULL DEFAULT 0,
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `trans_x` float NOT NULL DEFAULT 0,
  `trans_y` float NOT NULL DEFAULT 0,
  `trans_z` float NOT NULL DEFAULT 0,
  `trans_o` float NOT NULL DEFAULT 0,
  `transguid` bigint(20) unsigned NOT NULL DEFAULT 0,
  `extra_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `stable_slots` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `at_login` int(10) unsigned NOT NULL DEFAULT 0,
  `zone` int(10) unsigned NOT NULL DEFAULT 0,
  `death_expire_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `taxi_path` mediumtext DEFAULT NULL,
  `honorRankPoints` float NOT NULL DEFAULT 0,
  `honorHighestRank` int(10) unsigned NOT NULL DEFAULT 0,
  `honorStanding` int(10) unsigned NOT NULL DEFAULT 0,
  `honorLastWeekHK` int(10) unsigned NOT NULL DEFAULT 0,
  `honorLastWeekCP` decimal(11,1) NOT NULL DEFAULT 0.0,
  `honorStoredHK` int(11) NOT NULL DEFAULT 0,
  `honorStoredDK` int(11) NOT NULL DEFAULT 0,
  `watchedFaction` int(10) unsigned NOT NULL DEFAULT 0,
  `drunk` smallint(5) unsigned NOT NULL DEFAULT 0,
  `health` int(10) unsigned NOT NULL DEFAULT 0,
  `power1` int(10) unsigned NOT NULL DEFAULT 0,
  `power2` int(10) unsigned NOT NULL DEFAULT 0,
  `power3` int(10) unsigned NOT NULL DEFAULT 0,
  `power4` int(10) unsigned NOT NULL DEFAULT 0,
  `power5` int(10) unsigned NOT NULL DEFAULT 0,
  `exploredZones` longtext DEFAULT NULL,
  `equipmentCache` longtext DEFAULT NULL,
  `ammoId` int(10) unsigned NOT NULL DEFAULT 0,
  `actionBars` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `deleteInfos_Account` int(10) unsigned DEFAULT NULL,
  `deleteInfos_Name` varchar(12) DEFAULT NULL,
  `deleteDate` bigint(20) DEFAULT NULL,
  `area` int(10) unsigned NOT NULL DEFAULT 0,
  `world_phase_mask` int(11) DEFAULT 0,
  `customFlags` int(11) NOT NULL DEFAULT 0,
  `city_protector` tinyint(4) NOT NULL DEFAULT 0,
  `regexFilterCount` int(10) unsigned NOT NULL DEFAULT 0,
  `isGMCharacter` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ignore_titles` tinyint(4) NOT NULL DEFAULT 0,
  `mortality_status` tinyint(4) NOT NULL DEFAULT 0,
  `total_deaths` int(11) NOT NULL DEFAULT 0,
  `xp_gain` tinyint(3) unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `idx_account` (`account`) USING BTREE,
  KEY `idx_online` (`online`) USING BTREE,
  KEY `idx_name` (`name`) USING BTREE,
  KEY `deleteDate` (`deleteDate`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_bgqueue`
--

DROP TABLE IF EXISTS `character_bgqueue`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_bgqueue` (
  `PlayerGUID` int(11) NOT NULL DEFAULT 0,
  `playerName` varchar(12) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `playerIP` int(11) NOT NULL,
  `BGtype` int(11) NOT NULL,
  `action` int(11) NOT NULL,
  `time` int(11) NOT NULL,
  PRIMARY KEY (`PlayerGUID`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_deleted_items`
--

DROP TABLE IF EXISTS `character_deleted_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_deleted_items` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `player_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `item_entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `stack_count` mediumint(8) unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_destroyed_items`
--

DROP TABLE IF EXISTS `character_destroyed_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_destroyed_items` (
  `player_guid` int(10) unsigned NOT NULL,
  `item_entry` mediumint(8) unsigned NOT NULL,
  `stack_count` mediumint(8) unsigned NOT NULL,
  `time` bigint(20) unsigned NOT NULL,
  KEY `player_guid` (`player_guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC COMMENT='items that players have thrown away';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_duplicate_account`
--

DROP TABLE IF EXISTS `character_duplicate_account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_duplicate_account` (
  `account` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_egg_loot`
--

DROP TABLE IF EXISTS `character_egg_loot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_egg_loot` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `playerGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `itemEntry` int(10) unsigned NOT NULL DEFAULT 0,
  `itemGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `refunded` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_forgotten_skills`
--

DROP TABLE IF EXISTS `character_forgotten_skills`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_forgotten_skills` (
  `guid` int(10) unsigned NOT NULL COMMENT 'Global Unique Identifier',
  `skill` mediumint(8) unsigned NOT NULL,
  `value` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY (`guid`,`skill`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_gifts`
--

DROP TABLE IF EXISTS `character_gifts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_gifts` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `item_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `entry` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`item_guid`) USING BTREE,
  KEY `idx_guid` (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_homebind`
--

DROP TABLE IF EXISTS `character_homebind`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_homebind` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `map` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Map Identifier',
  `zone` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Zone Identifier',
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_honor_cp`
--

DROP TABLE IF EXISTS `character_honor_cp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_honor_cp` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `victimType` tinyint(3) unsigned NOT NULL DEFAULT 4,
  `victim` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Creature / Player Identifier',
  `cp` float NOT NULL DEFAULT 0,
  `date` int(10) unsigned NOT NULL DEFAULT 0,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  KEY `idx_guid` (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_instance`
--

DROP TABLE IF EXISTS `character_instance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_instance` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `instance` int(10) unsigned NOT NULL DEFAULT 0,
  `permanent` tinyint(1) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`instance`) USING BTREE,
  KEY `idx_instance` (`instance`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_inventory`
--

DROP TABLE IF EXISTS `character_inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_inventory` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `bag` int(10) unsigned NOT NULL DEFAULT 0,
  `slot` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `item` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Item Global Unique Identifier',
  `item_template` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Item Identifier',
  PRIMARY KEY (`item`) USING BTREE,
  KEY `idx_guid` (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_item_logs`
--

DROP TABLE IF EXISTS `character_item_logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_item_logs` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `playerLowGuid` int(10) unsigned NOT NULL,
  `itemLowGuid` int(10) unsigned NOT NULL,
  `itemEntry` int(10) unsigned NOT NULL,
  `itemCount` int(10) unsigned NOT NULL,
  `action` int(10) unsigned NOT NULL,
  `timestamp` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `playerLowGuid` (`playerLowGuid`,`itemLowGuid`,`itemEntry`) USING BTREE,
  KEY `action` (`action`) USING BTREE,
  KEY `timestamp` (`timestamp`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_pet`
--

DROP TABLE IF EXISTS `character_pet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_pet` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `entry` int(10) unsigned NOT NULL DEFAULT 0,
  `owner` int(10) unsigned NOT NULL DEFAULT 0,
  `modelid` int(10) unsigned DEFAULT 0,
  `CreatedBySpell` int(10) unsigned NOT NULL DEFAULT 0,
  `PetType` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `level` int(10) unsigned NOT NULL DEFAULT 1,
  `exp` int(10) unsigned NOT NULL DEFAULT 0,
  `Reactstate` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `loyaltypoints` int(11) NOT NULL DEFAULT 0,
  `loyalty` int(10) unsigned NOT NULL DEFAULT 0,
  `trainpoint` int(11) NOT NULL DEFAULT 0,
  `name` varchar(100) DEFAULT 'Pet',
  `renamed` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `slot` int(10) unsigned NOT NULL DEFAULT 0,
  `curhealth` int(10) unsigned NOT NULL DEFAULT 1,
  `curmana` int(10) unsigned NOT NULL DEFAULT 0,
  `curhappiness` int(10) unsigned NOT NULL DEFAULT 0,
  `savetime` bigint(20) unsigned NOT NULL DEFAULT 0,
  `resettalents_cost` int(10) unsigned NOT NULL DEFAULT 0,
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `abdata` longtext DEFAULT NULL,
  `teachspelldata` longtext DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_owner` (`owner`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Pet System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_pvp_currency`
--

DROP TABLE IF EXISTS `character_pvp_currency`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_pvp_currency` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `honor` int(10) unsigned NOT NULL DEFAULT 0,
  `conquest` int(10) unsigned NOT NULL DEFAULT 0,
  `weekly_honor` int(10) unsigned NOT NULL DEFAULT 0,
  `week_begin_day` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_queststatus`
--

DROP TABLE IF EXISTS `character_queststatus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_queststatus` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `quest` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Quest Identifier',
  `status` int(10) unsigned NOT NULL DEFAULT 0,
  `rewarded` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `explored` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `timer` bigint(20) unsigned NOT NULL DEFAULT 0,
  `mobcount1` int(10) unsigned NOT NULL DEFAULT 0,
  `mobcount2` int(10) unsigned NOT NULL DEFAULT 0,
  `mobcount3` int(10) unsigned NOT NULL DEFAULT 0,
  `mobcount4` int(10) unsigned NOT NULL DEFAULT 0,
  `itemcount1` int(10) unsigned NOT NULL DEFAULT 0,
  `itemcount2` int(10) unsigned NOT NULL DEFAULT 0,
  `itemcount3` int(10) unsigned NOT NULL DEFAULT 0,
  `itemcount4` int(10) unsigned NOT NULL DEFAULT 0,
  `reward_choice` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`quest`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_reputation`
--

DROP TABLE IF EXISTS `character_reputation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_reputation` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `faction` int(10) unsigned NOT NULL DEFAULT 0,
  `standing` int(11) NOT NULL DEFAULT 0,
  `flags` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`faction`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_reputation_broken`
--

DROP TABLE IF EXISTS `character_reputation_broken`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_reputation_broken` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `faction` int(10) unsigned NOT NULL DEFAULT 0,
  `standing` int(11) NOT NULL DEFAULT 0,
  `flags` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`faction`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_skills`
--

DROP TABLE IF EXISTS `character_skills`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_skills` (
  `guid` int(10) unsigned NOT NULL COMMENT 'Global Unique Identifier',
  `skill` mediumint(8) unsigned NOT NULL,
  `value` mediumint(8) unsigned NOT NULL,
  `max` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY (`guid`,`skill`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_skills_copy`
--

DROP TABLE IF EXISTS `character_skills_copy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_skills_copy` (
  `guid` int(10) unsigned NOT NULL COMMENT 'Global Unique Identifier',
  `skill` mediumint(8) unsigned NOT NULL,
  `value` mediumint(8) unsigned NOT NULL,
  `max` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY (`guid`,`skill`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_social`
--

DROP TABLE IF EXISTS `character_social`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_social` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Character Global Unique Identifier',
  `friend` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Friend Global Unique Identifier',
  `flags` tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT 'Friend Flags',
  PRIMARY KEY (`guid`,`friend`,`flags`) USING BTREE,
  KEY `idx_guid` (`guid`) USING BTREE,
  KEY `idx_friend` (`friend`) USING BTREE,
  KEY `idx_guid_flags` (`guid`,`flags`) USING BTREE,
  KEY `idx_friend_flags` (`friend`,`flags`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_spell`
--

DROP TABLE IF EXISTS `character_spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_spell` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `spell` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Spell Identifier',
  `active` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `disabled` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`spell`) USING BTREE,
  KEY `idx_spell` (`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_spell_cooldown`
--

DROP TABLE IF EXISTS `character_spell_cooldown`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_spell_cooldown` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier, Low part',
  `spell` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Spell Identifier',
  `item` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Item Identifier',
  `time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `catTime` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_spell_dual_spec`
--

DROP TABLE IF EXISTS `character_spell_dual_spec`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_spell_dual_spec` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `spell` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Spell Identifier',
  `spec` tinyint(3) unsigned NOT NULL DEFAULT 1 COMMENT 'primary or secondary',
  PRIMARY KEY (`guid`,`spell`,`spec`) USING BTREE,
  KEY `idx_spell` (`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_stats`
--

DROP TABLE IF EXISTS `character_stats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_stats` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier, Low part',
  `maxhealth` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower1` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower2` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower3` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower4` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower5` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower6` int(10) unsigned NOT NULL DEFAULT 0,
  `maxpower7` int(10) unsigned NOT NULL DEFAULT 0,
  `strength` int(10) unsigned NOT NULL DEFAULT 0,
  `agility` int(10) unsigned NOT NULL DEFAULT 0,
  `stamina` int(10) unsigned NOT NULL DEFAULT 0,
  `intellect` int(10) unsigned NOT NULL DEFAULT 0,
  `spirit` int(10) unsigned NOT NULL DEFAULT 0,
  `armor` int(10) unsigned NOT NULL DEFAULT 0,
  `resHoly` int(10) unsigned NOT NULL DEFAULT 0,
  `resFire` int(10) unsigned NOT NULL DEFAULT 0,
  `resNature` int(10) unsigned NOT NULL DEFAULT 0,
  `resFrost` int(10) unsigned NOT NULL DEFAULT 0,
  `resShadow` int(10) unsigned NOT NULL DEFAULT 0,
  `resArcane` int(10) unsigned NOT NULL DEFAULT 0,
  `blockPct` float unsigned NOT NULL DEFAULT 0,
  `dodgePct` float unsigned NOT NULL DEFAULT 0,
  `parryPct` float unsigned NOT NULL DEFAULT 0,
  `critPct` float unsigned NOT NULL DEFAULT 0,
  `rangedCritPct` float unsigned NOT NULL DEFAULT 0,
  `attackPower` int(10) unsigned NOT NULL DEFAULT 0,
  `rangedAttackPower` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_ticket`
--

DROP TABLE IF EXISTS `character_ticket`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_ticket` (
  `ticket_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `ticket_text` text DEFAULT NULL,
  `response_text` text DEFAULT NULL,
  `ticket_lastchange` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`ticket_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_titles`
--

DROP TABLE IF EXISTS `character_titles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_titles` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `title` int(10) unsigned NOT NULL DEFAULT 0,
  `active` tinyint(4) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`title`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_transmogs`
--

DROP TABLE IF EXISTS `character_transmogs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_transmogs` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `itemId` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`itemId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_tutorial`
--

DROP TABLE IF EXISTS `character_tutorial`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_tutorial` (
  `account` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Account Identifier',
  `tut0` int(10) unsigned NOT NULL DEFAULT 0,
  `tut1` int(10) unsigned NOT NULL DEFAULT 0,
  `tut2` int(10) unsigned NOT NULL DEFAULT 0,
  `tut3` int(10) unsigned NOT NULL DEFAULT 0,
  `tut4` int(10) unsigned NOT NULL DEFAULT 0,
  `tut5` int(10) unsigned NOT NULL DEFAULT 0,
  `tut6` int(10) unsigned NOT NULL DEFAULT 0,
  `tut7` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`account`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_variables`
--

DROP TABLE IF EXISTS `character_variables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_variables` (
  `lowGuid` int(10) unsigned NOT NULL,
  `variableType` int(10) unsigned NOT NULL,
  `value` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`lowGuid`,`variableType`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_xp_from_log`
--

DROP TABLE IF EXISTS `character_xp_from_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_xp_from_log` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `xp` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`type`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters`
--

DROP TABLE IF EXISTS `characters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `account` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Account Identifier',
  `name` varchar(12) NOT NULL DEFAULT '',
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `gender` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `xp` int(10) unsigned NOT NULL DEFAULT 0,
  `money` int(10) unsigned NOT NULL DEFAULT 0,
  `playerBytes` int(10) unsigned NOT NULL DEFAULT 0,
  `playerBytes2` int(10) unsigned NOT NULL DEFAULT 0,
  `playerFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `map` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Map Identifier',
  `orientation` float NOT NULL DEFAULT 0,
  `taximask` longtext DEFAULT NULL,
  `online` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `cinematic` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `totaltime` int(10) unsigned NOT NULL DEFAULT 0,
  `leveltime` int(10) unsigned NOT NULL DEFAULT 0,
  `logout_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `is_logout_resting` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `rest_bonus` float NOT NULL DEFAULT 0,
  `resettalents_multiplier` int(10) unsigned NOT NULL DEFAULT 0,
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `trans_x` float NOT NULL DEFAULT 0,
  `trans_y` float NOT NULL DEFAULT 0,
  `trans_z` float NOT NULL DEFAULT 0,
  `trans_o` float NOT NULL DEFAULT 0,
  `transguid` bigint(20) unsigned NOT NULL DEFAULT 0,
  `extra_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `stable_slots` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `at_login` int(10) unsigned NOT NULL DEFAULT 0,
  `zone` int(10) unsigned NOT NULL DEFAULT 0,
  `death_expire_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `taxi_path` text DEFAULT NULL,
  `honorRankPoints` float NOT NULL DEFAULT 0,
  `honorHighestRank` int(10) unsigned NOT NULL DEFAULT 0,
  `honorStanding` int(10) unsigned NOT NULL DEFAULT 0,
  `honorLastWeekHK` int(10) unsigned NOT NULL DEFAULT 0,
  `honorLastWeekCP` decimal(11,1) NOT NULL DEFAULT 0.0,
  `honorStoredHK` int(11) NOT NULL DEFAULT 0,
  `honorStoredDK` int(11) NOT NULL DEFAULT 0,
  `watchedFaction` int(10) unsigned NOT NULL DEFAULT 0,
  `drunk` smallint(5) unsigned NOT NULL DEFAULT 0,
  `health` int(10) unsigned NOT NULL DEFAULT 0,
  `power1` int(10) unsigned NOT NULL DEFAULT 0,
  `power2` int(10) unsigned NOT NULL DEFAULT 0,
  `power3` int(10) unsigned NOT NULL DEFAULT 0,
  `power4` int(10) unsigned NOT NULL DEFAULT 0,
  `power5` int(10) unsigned NOT NULL DEFAULT 0,
  `exploredZones` longtext DEFAULT NULL,
  `equipmentCache` longtext DEFAULT NULL,
  `ammoId` int(10) unsigned NOT NULL DEFAULT 0,
  `actionBars` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `deleteInfos_Account` int(10) unsigned DEFAULT NULL,
  `deleteInfos_Name` varchar(12) DEFAULT NULL,
  `deleteDate` bigint(20) DEFAULT NULL,
  `area` int(10) unsigned NOT NULL DEFAULT 0,
  `world_phase_mask` int(11) DEFAULT 0,
  `customFlags` int(11) NOT NULL DEFAULT 0,
  `city_protector` tinyint(4) NOT NULL DEFAULT 0,
  `regexFilterCount` int(10) unsigned NOT NULL DEFAULT 0,
  `isGMCharacter` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ignore_titles` tinyint(4) NOT NULL DEFAULT 0,
  `mortality_status` tinyint(4) NOT NULL DEFAULT 0,
  `total_deaths` int(11) NOT NULL DEFAULT 0,
  `xp_gain` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `active` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `extraBonusTalentCount` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `idx_account` (`account`) USING BTREE,
  KEY `idx_online` (`online`) USING BTREE,
  KEY `idx_name` (`name`) USING BTREE,
  KEY `deleteDate` (`deleteDate`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_namecleanup2020_2`
--

DROP TABLE IF EXISTS `characters_namecleanup2020_2`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_namecleanup2020_2` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `account` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Account Identifier',
  `name` varchar(12) NOT NULL DEFAULT '',
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `gender` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `xp` int(10) unsigned NOT NULL DEFAULT 0,
  `money` int(10) unsigned NOT NULL DEFAULT 0,
  `playerBytes` int(10) unsigned NOT NULL DEFAULT 0,
  `playerBytes2` int(10) unsigned NOT NULL DEFAULT 0,
  `playerFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `map` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Map Identifier',
  `orientation` float NOT NULL DEFAULT 0,
  `taximask` longtext DEFAULT NULL,
  `online` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `cinematic` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `totaltime` int(10) unsigned NOT NULL DEFAULT 0,
  `leveltime` int(10) unsigned NOT NULL DEFAULT 0,
  `logout_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `is_logout_resting` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `rest_bonus` float NOT NULL DEFAULT 0,
  `resettalents_multiplier` int(10) unsigned NOT NULL DEFAULT 0,
  `resettalents_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `trans_x` float NOT NULL DEFAULT 0,
  `trans_y` float NOT NULL DEFAULT 0,
  `trans_z` float NOT NULL DEFAULT 0,
  `trans_o` float NOT NULL DEFAULT 0,
  `transguid` bigint(20) unsigned NOT NULL DEFAULT 0,
  `extra_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `stable_slots` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `at_login` int(10) unsigned NOT NULL DEFAULT 0,
  `zone` int(10) unsigned NOT NULL DEFAULT 0,
  `death_expire_time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `taxi_path` text DEFAULT NULL,
  `honorRankPoints` float NOT NULL DEFAULT 0,
  `honorHighestRank` int(10) unsigned NOT NULL DEFAULT 0,
  `honorStanding` int(10) unsigned NOT NULL DEFAULT 0,
  `honorLastWeekHK` int(10) unsigned NOT NULL DEFAULT 0,
  `honorLastWeekCP` decimal(11,1) NOT NULL DEFAULT 0.0,
  `honorStoredHK` int(11) NOT NULL DEFAULT 0,
  `honorStoredDK` int(11) NOT NULL DEFAULT 0,
  `watchedFaction` int(10) unsigned NOT NULL DEFAULT 0,
  `drunk` smallint(5) unsigned NOT NULL DEFAULT 0,
  `health` int(10) unsigned NOT NULL DEFAULT 0,
  `power1` int(10) unsigned NOT NULL DEFAULT 0,
  `power2` int(10) unsigned NOT NULL DEFAULT 0,
  `power3` int(10) unsigned NOT NULL DEFAULT 0,
  `power4` int(10) unsigned NOT NULL DEFAULT 0,
  `power5` int(10) unsigned NOT NULL DEFAULT 0,
  `exploredZones` longtext DEFAULT NULL,
  `equipmentCache` longtext DEFAULT NULL,
  `ammoId` int(10) unsigned NOT NULL DEFAULT 0,
  `actionBars` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `deleteInfos_Account` int(10) unsigned DEFAULT NULL,
  `deleteInfos_Name` varchar(12) DEFAULT NULL,
  `deleteDate` bigint(20) DEFAULT NULL,
  `area` int(10) unsigned NOT NULL DEFAULT 0,
  `world_phase_mask` int(11) DEFAULT 0,
  `customFlags` int(11) NOT NULL DEFAULT 0,
  `city_protector` tinyint(4) NOT NULL DEFAULT 0,
  `regexFilterCount` int(10) unsigned NOT NULL DEFAULT 0,
  `isGMCharacter` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ignore_titles` tinyint(4) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `idx_account` (`account`) USING BTREE,
  KEY `idx_online` (`online`) USING BTREE,
  KEY `idx_name` (`name`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_total_money`
--

DROP TABLE IF EXISTS `characters_total_money`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_total_money` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `total_gold` int(10) unsigned NOT NULL DEFAULT 0,
  `time` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `corpse`
--

DROP TABLE IF EXISTS `corpse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `corpse` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `player` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Character Global Unique Identifier',
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  `map` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Map Identifier',
  `time` bigint(20) unsigned NOT NULL DEFAULT 0,
  `corpse_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `instance` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `idx_type` (`corpse_type`) USING BTREE,
  KEY `idx_instance` (`instance`) USING BTREE,
  KEY `idx_player` (`player`) USING BTREE,
  KEY `idx_time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Death System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_respawn`
--

DROP TABLE IF EXISTS `creature_respawn`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_respawn` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `respawntime` bigint(20) NOT NULL DEFAULT 0,
  `instance` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `map` int(10) unsigned DEFAULT 0,
  PRIMARY KEY (`guid`,`instance`) USING BTREE,
  KEY `idx_instance` (`instance`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Grid Loading System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_event_status`
--

DROP TABLE IF EXISTS `game_event_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event_status` (
  `event` smallint(5) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`event`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Game event system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_respawn`
--

DROP TABLE IF EXISTS `gameobject_respawn`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_respawn` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `respawntime` bigint(20) NOT NULL DEFAULT 0,
  `instance` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `map` int(10) unsigned DEFAULT 0,
  PRIMARY KEY (`guid`,`instance`) USING BTREE,
  KEY `idx_instance` (`instance`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Grid Loading System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_subsurveys`
--

DROP TABLE IF EXISTS `gm_subsurveys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gm_subsurveys` (
  `surveyId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `subsurveyId` int(10) unsigned NOT NULL DEFAULT 0,
  `rank` int(10) unsigned NOT NULL DEFAULT 0,
  `comment` text NOT NULL,
  PRIMARY KEY (`surveyId`,`subsurveyId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_surveys`
--

DROP TABLE IF EXISTS `gm_surveys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gm_surveys` (
  `surveyId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `mainSurvey` int(10) unsigned NOT NULL DEFAULT 0,
  `overallComment` longtext NOT NULL,
  `createTime` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`surveyId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_tickets`
--

DROP TABLE IF EXISTS `gm_tickets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gm_tickets` (
  `ticketId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier of ticket creator',
  `name` varchar(12) NOT NULL COMMENT 'Name of ticket creator',
  `message` text NOT NULL,
  `createTime` int(10) unsigned NOT NULL DEFAULT 0,
  `mapId` smallint(5) unsigned NOT NULL DEFAULT 0,
  `posX` float NOT NULL DEFAULT 0,
  `posY` float NOT NULL DEFAULT 0,
  `posZ` float NOT NULL DEFAULT 0,
  `lastModifiedTime` int(10) unsigned NOT NULL DEFAULT 0,
  `closedBy` int(11) NOT NULL DEFAULT 0,
  `assignedTo` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'GUID of admin to whom ticket is assigned',
  `comment` text NOT NULL,
  `response` text NOT NULL,
  `completed` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `escalated` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `viewed` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `haveTicket` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `ticketType` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `securityNeeded` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ticketId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `group_instance`
--

DROP TABLE IF EXISTS `group_instance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `group_instance` (
  `leaderGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `instance` int(10) unsigned NOT NULL DEFAULT 0,
  `permanent` tinyint(1) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`leaderGuid`,`instance`) USING BTREE,
  KEY `idx_instance` (`instance`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `group_member`
--

DROP TABLE IF EXISTS `group_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `group_member` (
  `groupId` int(10) unsigned NOT NULL,
  `memberGuid` int(10) unsigned NOT NULL,
  `assistant` tinyint(1) unsigned NOT NULL,
  `subgroup` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`groupId`,`memberGuid`) USING BTREE,
  KEY `idx_memberGuid` (`memberGuid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Groups';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `groups`
--

DROP TABLE IF EXISTS `groups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `groups` (
  `groupId` int(10) unsigned NOT NULL,
  `leaderGuid` int(10) unsigned NOT NULL,
  `mainTank` int(10) unsigned NOT NULL,
  `mainAssistant` int(10) unsigned NOT NULL,
  `lootMethod` tinyint(3) unsigned NOT NULL,
  `looterGuid` int(10) unsigned NOT NULL,
  `lootThreshold` tinyint(3) unsigned NOT NULL,
  `icon1` int(10) unsigned NOT NULL,
  `icon2` int(10) unsigned NOT NULL,
  `icon3` int(10) unsigned NOT NULL,
  `icon4` int(10) unsigned NOT NULL,
  `icon5` int(10) unsigned NOT NULL,
  `icon6` int(10) unsigned NOT NULL,
  `icon7` int(10) unsigned NOT NULL,
  `icon8` int(10) unsigned NOT NULL,
  `isRaid` tinyint(1) unsigned NOT NULL,
  PRIMARY KEY (`groupId`) USING BTREE,
  UNIQUE KEY `key_leaderGuid` (`leaderGuid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Groups';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild`
--

DROP TABLE IF EXISTS `guild`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild` (
  `guildid` int(10) unsigned NOT NULL DEFAULT 0,
  `name` varchar(255) NOT NULL DEFAULT '',
  `leaderguid` int(10) unsigned NOT NULL DEFAULT 0,
  `EmblemStyle` int(11) NOT NULL DEFAULT 0,
  `EmblemColor` int(11) NOT NULL DEFAULT 0,
  `BorderStyle` int(11) NOT NULL DEFAULT 0,
  `BorderColor` int(11) NOT NULL DEFAULT 0,
  `BackgroundColor` int(11) NOT NULL DEFAULT 0,
  `info` text NOT NULL,
  `motd` varchar(255) NOT NULL DEFAULT '',
  `createdate` bigint(20) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guildid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Guild System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_bank`
--

DROP TABLE IF EXISTS `guild_bank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_bank` (
  `guildid` int(10) unsigned NOT NULL,
  `guid` int(11) NOT NULL,
  `isInferno` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `tab` int(10) unsigned NOT NULL DEFAULT 0,
  `slot` tinyint(3) unsigned NOT NULL,
  `item_template` int(10) unsigned NOT NULL,
  `creatorGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `giftCreatorGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `count` int(10) unsigned NOT NULL DEFAULT 0,
  `duration` int(11) NOT NULL DEFAULT 0,
  `charges` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `flags` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `enchantments` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `randomPropertyId` smallint(6) NOT NULL DEFAULT 0,
  `transmogrifyId` int(10) unsigned NOT NULL DEFAULT 0,
  `durability` smallint(5) unsigned NOT NULL DEFAULT 0,
  `text` int(10) unsigned NOT NULL DEFAULT 0,
  `generated_loot` tinyint(4) DEFAULT 0,
  UNIQUE KEY `guildidandguidandinferno` (`guildid`,`guid`,`isInferno`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_bank_analysis`
--

DROP TABLE IF EXISTS `guild_bank_analysis`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_bank_analysis` (
  `guildid` int(10) unsigned NOT NULL,
  `guid` int(11) NOT NULL,
  `isInferno` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `tab` int(10) unsigned NOT NULL DEFAULT 0,
  `slot` tinyint(3) unsigned NOT NULL,
  `item_template` int(10) unsigned NOT NULL,
  `creatorGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `giftCreatorGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `count` int(10) unsigned NOT NULL DEFAULT 0,
  `duration` int(11) NOT NULL DEFAULT 0,
  `charges` tinytext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `flags` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `enchantments` mediumtext CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `randomPropertyId` smallint(6) NOT NULL DEFAULT 0,
  `transmogrifyId` int(10) unsigned NOT NULL DEFAULT 0,
  `durability` smallint(5) unsigned NOT NULL DEFAULT 0,
  `text` int(10) unsigned NOT NULL DEFAULT 0,
  `generated_loot` tinyint(4) DEFAULT 0,
  UNIQUE KEY `guildidandguidandinferno` (`guildid`,`guid`,`isInferno`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_bank_log`
--

DROP TABLE IF EXISTS `guild_bank_log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_bank_log` (
  `log_id` int(11) NOT NULL AUTO_INCREMENT,
  `isInferno` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `guildid` int(11) NOT NULL,
  `player` int(11) NOT NULL,
  `action` int(11) NOT NULL,
  `tab` int(11) NOT NULL DEFAULT 1,
  `item` int(11) NOT NULL,
  `randomPropertyId` int(11) NOT NULL DEFAULT 0,
  `enchant` int(11) NOT NULL DEFAULT 0,
  `count` int(11) NOT NULL,
  `stamp` bigint(20) NOT NULL,
  PRIMARY KEY (`log_id`) USING BTREE,
  KEY `stamp` (`stamp`,`isInferno`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_bank_money`
--

DROP TABLE IF EXISTS `guild_bank_money`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_bank_money` (
  `guildid` int(11) NOT NULL,
  `isInferno` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `money` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guildid`,`isInferno`) USING BTREE,
  UNIQUE KEY `guildid` (`guildid`,`isInferno`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_bank_tabs`
--

DROP TABLE IF EXISTS `guild_bank_tabs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_bank_tabs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `guildid` int(11) NOT NULL,
  `isInferno` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `tabs` int(11) NOT NULL DEFAULT 0,
  `name1` varchar(64) NOT NULL DEFAULT 'Tab1',
  `name2` varchar(64) NOT NULL DEFAULT 'Tab2',
  `name3` varchar(64) NOT NULL DEFAULT 'Tab3',
  `name4` varchar(64) NOT NULL DEFAULT 'Tab4',
  `name5` varchar(64) NOT NULL DEFAULT 'Tab5',
  `icon1` varchar(128) NOT NULL DEFAULT 'inv_misc_bag_08',
  `icon2` varchar(128) NOT NULL DEFAULT 'inv_misc_bag_05',
  `icon3` varchar(128) NOT NULL DEFAULT 'inv_misc_bag_03',
  `icon4` varchar(128) NOT NULL DEFAULT 'inv_misc_bag_06',
  `icon5` varchar(128) NOT NULL DEFAULT 'inv_misc_bag_02',
  `withdrawal1` int(11) NOT NULL DEFAULT 0,
  `withdrawal2` int(11) NOT NULL DEFAULT 0,
  `withdrawal3` int(11) NOT NULL DEFAULT 0,
  `withdrawal4` int(11) NOT NULL DEFAULT 0,
  `withdrawal5` int(11) NOT NULL DEFAULT 0,
  `minrank1` int(11) NOT NULL DEFAULT 0,
  `minrank2` int(11) NOT NULL DEFAULT 0,
  `minrank3` int(11) NOT NULL DEFAULT 0,
  `minrank4` int(11) NOT NULL DEFAULT 0,
  `minrank5` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_eventlog`
--

DROP TABLE IF EXISTS `guild_eventlog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_eventlog` (
  `guildid` int(11) NOT NULL COMMENT 'Guild Identificator',
  `LogGuid` int(11) NOT NULL COMMENT 'Log record identificator - auxiliary column',
  `EventType` tinyint(1) NOT NULL COMMENT 'Event type',
  `PlayerGuid1` int(11) NOT NULL COMMENT 'Player 1',
  `PlayerGuid2` int(11) NOT NULL COMMENT 'Player 2',
  `NewRank` tinyint(4) NOT NULL COMMENT 'New rank(in case promotion/demotion)',
  `TimeStamp` bigint(20) NOT NULL COMMENT 'Event UNIX time',
  PRIMARY KEY (`guildid`,`LogGuid`) USING BTREE,
  KEY `idx_PlayerGuid1` (`PlayerGuid1`) USING BTREE,
  KEY `idx_PlayerGuid2` (`PlayerGuid2`) USING BTREE,
  KEY `idx_LogGuid` (`LogGuid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Guild Eventlog';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_house`
--

DROP TABLE IF EXISTS `guild_house`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_house` (
  `guild_id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `map_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`guild_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Teleportation coordinates for Guild Housing';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_member`
--

DROP TABLE IF EXISTS `guild_member`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_member` (
  `guildid` int(10) unsigned NOT NULL DEFAULT 0,
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `rank` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `pnote` varchar(255) NOT NULL DEFAULT '',
  `offnote` varchar(255) NOT NULL DEFAULT '',
  UNIQUE KEY `key_guid` (`guid`) USING BTREE,
  KEY `idx_guildid` (`guildid`) USING BTREE,
  KEY `idx_guildid_rank` (`guildid`,`rank`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Guild System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild_rank`
--

DROP TABLE IF EXISTS `guild_rank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `guild_rank` (
  `guildid` int(10) unsigned NOT NULL DEFAULT 0,
  `rid` int(10) unsigned NOT NULL,
  `rname` varchar(255) NOT NULL DEFAULT '',
  `rights` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guildid`,`rid`) USING BTREE,
  KEY `idx_rid` (`rid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Guild System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `hardcore_deaths`
--

DROP TABLE IF EXISTS `hardcore_deaths`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `hardcore_deaths` (
  `lowGuid` int(10) unsigned NOT NULL,
  `race` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `class` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `level` int(10) unsigned NOT NULL,
  `attackerEntry` int(10) unsigned NOT NULL,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `mapId` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`lowGuid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `instance`
--

DROP TABLE IF EXISTS `instance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `instance` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `map` int(10) unsigned NOT NULL DEFAULT 0,
  `resettime` bigint(20) NOT NULL DEFAULT 0,
  `data` longtext DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_map` (`map`) USING BTREE,
  KEY `idx_resettime` (`resettime`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `instance_reset`
--

DROP TABLE IF EXISTS `instance_reset`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `instance_reset` (
  `mapid` int(10) unsigned NOT NULL DEFAULT 0,
  `resettime` bigint(20) NOT NULL DEFAULT 0,
  PRIMARY KEY (`mapid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_instance`
--

DROP TABLE IF EXISTS `item_instance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_instance` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `itemEntry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `owner_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `creatorGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `giftCreatorGuid` int(10) unsigned NOT NULL DEFAULT 0,
  `count` int(10) unsigned NOT NULL DEFAULT 1,
  `duration` int(11) NOT NULL DEFAULT 0,
  `charges` tinytext DEFAULT NULL,
  `flags` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `enchantments` text NOT NULL,
  `randomPropertyId` smallint(6) NOT NULL DEFAULT 0,
  `transmogrifyId` int(10) unsigned NOT NULL DEFAULT 0,
  `durability` smallint(5) unsigned NOT NULL DEFAULT 0,
  `text` int(10) unsigned NOT NULL DEFAULT 0,
  `generated_loot` tinyint(4) DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `idx_owner_guid` (`owner_guid`) USING BTREE,
  KEY `idx_itemEntry` (`itemEntry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Item System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_loot`
--

DROP TABLE IF EXISTS `item_loot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_loot` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `owner_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `itemid` int(10) unsigned NOT NULL DEFAULT 0,
  `amount` int(10) unsigned NOT NULL DEFAULT 0,
  `property` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`itemid`) USING BTREE,
  KEY `idx_owner_guid` (`owner_guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Item System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_text`
--

DROP TABLE IF EXISTS `item_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_text` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `text` longtext DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Item System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_transmogs`
--

DROP TABLE IF EXISTS `item_transmogs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_transmogs` (
  `ID` int(10) unsigned NOT NULL,
  `ItemID` int(10) unsigned NOT NULL,
  `DisplayID` int(10) unsigned NOT NULL,
  `SourceID` int(11) NOT NULL,
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `logs_anticheat`
--

DROP TABLE IF EXISTS `logs_anticheat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `logs_anticheat` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `time` datetime NOT NULL DEFAULT current_timestamp(),
  `realm` int(10) unsigned NOT NULL,
  `account` int(10) unsigned NOT NULL,
  `ip` varchar(16) NOT NULL,
  `fingerprint` int(10) unsigned NOT NULL,
  `actionMask` int(10) unsigned DEFAULT NULL,
  `player` varchar(32) NOT NULL,
  `info` varchar(512) NOT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `account` (`account`) USING BTREE,
  KEY `ip` (`ip`) USING BTREE,
  KEY `time` (`time`) USING BTREE,
  KEY `realm` (`realm`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `logs_movement`
--

DROP TABLE IF EXISTS `logs_movement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `logs_movement` (
  `account` int(10) unsigned NOT NULL,
  `guid` int(10) unsigned NOT NULL,
  `posx` float NOT NULL,
  `posy` float NOT NULL,
  `posz` float NOT NULL,
  `map` int(10) unsigned NOT NULL,
  `desyncMs` int(11) NOT NULL,
  `desyncDist` float NOT NULL,
  `cheats` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `logs_shellcoin`
--

DROP TABLE IF EXISTS `logs_shellcoin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `logs_shellcoin` (
  `time` bigint(20) NOT NULL DEFAULT 0,
  `count` int(11) NOT NULL DEFAULT 0,
  `price` int(11) NOT NULL DEFAULT 0,
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `logs_spamdetect`
--

DROP TABLE IF EXISTS `logs_spamdetect`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `logs_spamdetect` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `time` timestamp NOT NULL DEFAULT current_timestamp(),
  `realm` int(10) unsigned NOT NULL,
  `accountId` int(11) DEFAULT 0,
  `fromIP` varchar(16) NOT NULL,
  `fromFingerprint` int(10) unsigned NOT NULL,
  `comment` varchar(8192) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `logs_warden`
--

DROP TABLE IF EXISTS `logs_warden`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `logs_warden` (
  `entry` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Log entry ID',
  `check` smallint(5) unsigned NOT NULL COMMENT 'Failed Warden check ID',
  `action` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT 'Action taken (enum WardenActions)',
  `account` int(10) unsigned NOT NULL COMMENT 'Account ID',
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Player GUID',
  `map` int(10) unsigned DEFAULT NULL COMMENT 'Map ID',
  `position_x` float DEFAULT NULL COMMENT 'Player position X',
  `position_y` float DEFAULT NULL COMMENT 'Player position Y',
  `position_z` float DEFAULT NULL COMMENT 'Player position Z',
  `date` timestamp NOT NULL DEFAULT current_timestamp() COMMENT 'Date of the log entry',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Warden log of failed checks';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mail`
--

DROP TABLE IF EXISTS `mail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mail` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `messageType` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stationery` tinyint(4) NOT NULL DEFAULT 41,
  `mailTemplateId` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `sender` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Character Global Unique Identifier',
  `receiver` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Character Global Unique Identifier',
  `subject` longtext DEFAULT NULL,
  `itemTextId` int(10) unsigned NOT NULL DEFAULT 0,
  `has_items` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `expire_time` bigint(20) NOT NULL DEFAULT 0,
  `deliver_time` bigint(20) NOT NULL DEFAULT 0,
  `money` int(10) unsigned NOT NULL DEFAULT 0,
  `cod` int(10) unsigned NOT NULL DEFAULT 0,
  `checked` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `isDeleted` tinyint(3) unsigned DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `idx_receiver` (`receiver`) USING BTREE,
  KEY `FK_mail_item_text` (`itemTextId`) USING BTREE,
  KEY `expire_time` (`expire_time`) USING BTREE,
  KEY `isDeleted` (`isDeleted`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Mail System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mail_items`
--

DROP TABLE IF EXISTS `mail_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mail_items` (
  `mail_id` int(10) unsigned NOT NULL DEFAULT 0,
  `item_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `item_template` int(10) unsigned NOT NULL DEFAULT 0,
  `receiver` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Character Global Unique Identifier',
  PRIMARY KEY (`mail_id`,`item_guid`) USING BTREE,
  KEY `idx_receiver` (`receiver`) USING BTREE,
  KEY `idx_item_guid` (`item_guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `migrations`
--

DROP TABLE IF EXISTS `migrations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `migrations` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(255) NOT NULL DEFAULT '0',
  `Hash` varchar(128) NOT NULL DEFAULT '0',
  `AppliedAt` datetime NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pet_aura`
--

DROP TABLE IF EXISTS `pet_aura`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pet_aura` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `caster_guid` bigint(20) unsigned NOT NULL DEFAULT 0 COMMENT 'Full Global Unique Identifier',
  `item_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `spell` int(10) unsigned NOT NULL DEFAULT 0,
  `stackcount` int(10) unsigned NOT NULL DEFAULT 1,
  `remaincharges` int(10) unsigned NOT NULL DEFAULT 0,
  `basepoints0` int(11) NOT NULL DEFAULT 0,
  `basepoints1` int(11) NOT NULL DEFAULT 0,
  `basepoints2` int(11) NOT NULL DEFAULT 0,
  `periodictime0` int(10) unsigned NOT NULL DEFAULT 0,
  `periodictime1` int(10) unsigned NOT NULL DEFAULT 0,
  `periodictime2` int(10) unsigned NOT NULL DEFAULT 0,
  `maxduration` int(11) NOT NULL DEFAULT 0,
  `remaintime` int(11) NOT NULL DEFAULT 0,
  `effIndexMask` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`caster_guid`,`item_guid`,`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Pet System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pet_spell`
--

DROP TABLE IF EXISTS `pet_spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pet_spell` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier',
  `spell` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Spell Identifier',
  `active` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Pet System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pet_spell_cooldown`
--

DROP TABLE IF EXISTS `pet_spell_cooldown`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pet_spell_cooldown` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier, Low part',
  `spell` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Spell Identifier',
  `time` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`spell`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `petition`
--

DROP TABLE IF EXISTS `petition`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `petition` (
  `ownerguid` int(10) unsigned NOT NULL,
  `petitionguid` int(10) unsigned DEFAULT 0,
  `charterguid` int(10) unsigned DEFAULT NULL COMMENT 'Charter item GUID',
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`ownerguid`) USING BTREE,
  UNIQUE KEY `key_ownerguid_petitionguid` (`ownerguid`,`petitionguid`) USING BTREE,
  UNIQUE KEY `charterguid` (`charterguid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Guild System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `petition_sign`
--

DROP TABLE IF EXISTS `petition_sign`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `petition_sign` (
  `ownerguid` int(10) unsigned NOT NULL,
  `petitionguid` int(10) unsigned NOT NULL DEFAULT 0,
  `playerguid` int(10) unsigned NOT NULL DEFAULT 0,
  `player_account` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`petitionguid`,`playerguid`) USING BTREE,
  KEY `idx_playerguid` (`playerguid`) USING BTREE,
  KEY `idx_ownerguid` (`ownerguid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Guild System';
/*!40101 SET character_set_client = @saved_cs_client */;


--
-- Table structure for table `saved_variables`
--

DROP TABLE IF EXISTS `saved_variables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `saved_variables` (
  `key` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `cleaning_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `lastHonorMaintenanceDay` int(10) unsigned NOT NULL DEFAULT 0,
  `nextHonorMaintenanceDay` int(10) unsigned NOT NULL DEFAULT 0,
  `honorMaintenanceMarker` tinyint(1) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`key`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Variable Saves';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `store_racechange`
--

DROP TABLE IF EXISTS `store_racechange`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `store_racechange` (
  `guid` int(10) unsigned NOT NULL,
  `race` tinyint(1) unsigned NOT NULL,
  `gender` tinyint(1) unsigned NOT NULL,
  `playerbytes1` int(10) unsigned NOT NULL,
  `playerbytes2` int(10) unsigned NOT NULL,
  `transaction` int(10) unsigned NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`transaction`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Race change requests from web backend. Playerbytes is from ''characters'' table, is used to copy new character outfit.';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `variables`
--

DROP TABLE IF EXISTS `variables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `variables` (
  `index` int(10) unsigned NOT NULL DEFAULT 0,
  `value` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`index`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `whisper_targets`
--

DROP TABLE IF EXISTS `whisper_targets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `whisper_targets` (
  `account` int(10) unsigned NOT NULL,
  `target_guid` int(10) unsigned NOT NULL,
  `time` int(10) unsigned NOT NULL,
  UNIQUE KEY `account_target` (`account`,`target_guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `world`
--

DROP TABLE IF EXISTS `world`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `world` (
  `map` int(10) unsigned NOT NULL DEFAULT 0,
  `data` longtext DEFAULT NULL,
  PRIMARY KEY (`map`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `worldstates`
--

DROP TABLE IF EXISTS `worldstates`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `worldstates` (
  `entry` int(11) DEFAULT NULL,
  `value` int(11) DEFAULT NULL,
  `comment` varchar(255) DEFAULT NULL,
  UNIQUE KEY `key_entry` (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `tw_logon`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `tw_logon` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */;

USE `tw_logon`;

--
-- Table structure for table `account`
--

DROP TABLE IF EXISTS `account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `username` varchar(32) NOT NULL,
  `sha_pass_hash` varchar(40) NOT NULL,
  `rank` int(10) unsigned NOT NULL DEFAULT 0,
  `sessionkey` longtext DEFAULT NULL,
  `v` longtext DEFAULT NULL,
  `s` longtext DEFAULT NULL,
  `email` text DEFAULT NULL,
  `joindate` timestamp NOT NULL DEFAULT current_timestamp(),
  `tfa_verif` varchar(255) DEFAULT NULL,
  `last_ip` varchar(30) NOT NULL DEFAULT '0.0.0.0',
  `last_local_ip` varchar(30) NOT NULL DEFAULT '0.0.0.0',
  `failed_logins` int(10) unsigned NOT NULL DEFAULT 0,
  `locked` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `last_login` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `online` tinyint(4) NOT NULL DEFAULT 0,
  `expansion` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mutetime` bigint(20) NOT NULL DEFAULT 0,
  `mutereason` varchar(255) NOT NULL DEFAULT '',
  `muteby` varchar(50) NOT NULL DEFAULT '',
  `locale` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `os` varchar(4) NOT NULL DEFAULT '',
  `platform` varchar(4) NOT NULL DEFAULT '',
  `current_realm` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `banned` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `security` varchar(255) DEFAULT NULL,
  `pass_verif` varchar(255) DEFAULT NULL COMMENT 'Web recover password',
  `email_verif` tinyint(1) NOT NULL DEFAULT 0 COMMENT 'Email verification',
  `forum_username` varchar(32) DEFAULT NULL,
  `token_key` varchar(100) DEFAULT '',
  `email_keyword` varchar(16) DEFAULT NULL,
  `email_status` int(11) DEFAULT NULL,
  `email_letterid` int(11) DEFAULT 0,
  `email_sub` tinyint(4) NOT NULL DEFAULT 1,
  `server` tinyint(4) NOT NULL DEFAULT 0,
  `comments` text DEFAULT NULL,
  `geolock_pin` int(11) DEFAULT 0,
  `queue_skip` tinyint(4) NOT NULL DEFAULT 0,
  `active` tinyint(3) unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `idx_username` (`username`) USING BTREE,
  KEY `idx_gmlevel` (`rank`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Account System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_banned`
--

DROP TABLE IF EXISTS `account_banned`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_banned` (
  `banid` bigint(20) NOT NULL AUTO_INCREMENT,
  `id` bigint(20) NOT NULL DEFAULT 0 COMMENT 'Account id',
  `bandate` bigint(20) NOT NULL DEFAULT 0,
  `unbandate` bigint(20) NOT NULL DEFAULT 0,
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(4) NOT NULL DEFAULT 1,
  `realm` tinyint(4) NOT NULL DEFAULT 1,
  `gmlevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`,`bandate`) USING BTREE,
  UNIQUE KEY `banid` (`banid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Ban List';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_banned_withautobancrap`
--

DROP TABLE IF EXISTS `account_banned_withautobancrap`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_banned_withautobancrap` (
  `banid` bigint(20) NOT NULL AUTO_INCREMENT,
  `id` bigint(20) NOT NULL DEFAULT 0 COMMENT 'Account id',
  `bandate` bigint(20) NOT NULL DEFAULT 0,
  `unbandate` bigint(20) NOT NULL DEFAULT 0,
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(4) NOT NULL DEFAULT 1,
  `realm` tinyint(4) NOT NULL DEFAULT 1,
  `gmlevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`,`bandate`) USING BTREE,
  UNIQUE KEY `banid` (`banid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Ban List';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_copy_060621`
--

DROP TABLE IF EXISTS `account_copy_060621`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_copy_060621` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `username` varchar(32) NOT NULL,
  `sha_pass_hash` varchar(40) NOT NULL,
  `gmlevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `sessionkey` longtext DEFAULT NULL,
  `v` longtext DEFAULT NULL,
  `s` longtext DEFAULT NULL,
  `email` text DEFAULT NULL,
  `joindate` timestamp NOT NULL DEFAULT current_timestamp(),
  `tfa_verif` varchar(255) DEFAULT NULL,
  `last_ip` varchar(30) NOT NULL DEFAULT '0.0.0.0',
  `last_local_ip` varchar(30) NOT NULL DEFAULT '0.0.0.0',
  `failed_logins` int(10) unsigned NOT NULL DEFAULT 0,
  `locked` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `last_login` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `online` tinyint(4) NOT NULL DEFAULT 0,
  `expansion` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mutetime` bigint(20) NOT NULL DEFAULT 0,
  `mutereason` varchar(255) NOT NULL DEFAULT '',
  `muteby` varchar(50) NOT NULL DEFAULT '',
  `locale` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `os` varchar(4) NOT NULL DEFAULT '',
  `current_realm` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `banned` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `security` varchar(255) DEFAULT NULL,
  `pass_verif` varchar(255) DEFAULT NULL COMMENT 'Web recover password',
  `email_verif` tinyint(1) NOT NULL DEFAULT 0 COMMENT 'Email verification',
  `forum_username` varchar(32) DEFAULT NULL,
  `token_key` varchar(100) DEFAULT '',
  `email_keyword` varchar(16) DEFAULT NULL,
  `email_status` int(11) DEFAULT NULL,
  `email_sub` tinyint(4) NOT NULL DEFAULT 1,
  `server` tinyint(4) NOT NULL DEFAULT 0,
  `comments` text DEFAULT NULL,
  `geolock_pin` int(11) DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `idx_username` (`username`) USING BTREE,
  KEY `idx_gmlevel` (`gmlevel`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Account System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_ip_logins`
--

DROP TABLE IF EXISTS `account_ip_logins`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_ip_logins` (
  `account_id` int(10) unsigned NOT NULL,
  `account_ip` varchar(32) NOT NULL DEFAULT '0.0.0.0',
  `login_count` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`account_id`,`account_ip`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_mailstatus_deprecated`
--

DROP TABLE IF EXISTS `account_mailstatus_deprecated`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_mailstatus_deprecated` (
  `message_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `account_id` int(10) unsigned NOT NULL DEFAULT 0,
  `email` text DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `letterid` int(11) DEFAULT NULL,
  PRIMARY KEY (`message_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_mailstatus_deprecated_archive`
--

DROP TABLE IF EXISTS `account_mailstatus_deprecated_archive`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_mailstatus_deprecated_archive` (
  `message_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `account_id` int(10) unsigned NOT NULL DEFAULT 0,
  `email` text DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `letterid` int(11) DEFAULT NULL,
  PRIMARY KEY (`message_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_muted`
--

DROP TABLE IF EXISTS `account_muted`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_muted` (
  `muteid` bigint(20) NOT NULL AUTO_INCREMENT,
  `id` bigint(20) NOT NULL DEFAULT 0 COMMENT 'Account id',
  `mutedate` bigint(20) NOT NULL DEFAULT 0,
  `unmutedate` bigint(20) NOT NULL DEFAULT 0,
  `mutedby` varchar(50) NOT NULL,
  `mutereason` varchar(255) NOT NULL,
  PRIMARY KEY (`id`,`mutedate`) USING BTREE,
  UNIQUE KEY `banid` (`muteid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Mute History';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `account_twofactor_allowed`
--

DROP TABLE IF EXISTS `account_twofactor_allowed`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account_twofactor_allowed` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip_address` varchar(30) NOT NULL DEFAULT '0',
  `account_id` int(10) unsigned DEFAULT NULL,
  `expires_at` bigint(20) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `antispam_blacklist`
--

DROP TABLE IF EXISTS `antispam_blacklist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `antispam_blacklist` (
  `word` varchar(32) NOT NULL DEFAULT '',
  `regex` tinyint(4) NOT NULL DEFAULT 0,
  PRIMARY KEY (`word`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `antispam_client`
--

DROP TABLE IF EXISTS `antispam_client`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `antispam_client` (
  `Regex` varchar(255) NOT NULL,
  `Note` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`Regex`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `antispam_replacement`
--

DROP TABLE IF EXISTS `antispam_replacement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `antispam_replacement` (
  `from` varchar(32) NOT NULL DEFAULT '',
  `to` varchar(32) NOT NULL DEFAULT '',
  PRIMARY KEY (`from`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `antispam_scores`
--

DROP TABLE IF EXISTS `antispam_scores`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `antispam_scores` (
  `word` varchar(32) NOT NULL DEFAULT '',
  `score` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `type` tinyint(1) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`word`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `antispam_unicode`
--

DROP TABLE IF EXISTS `antispam_unicode`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `antispam_unicode` (
  `from` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `to` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`from`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `antispam_unicode_replacement`
--

DROP TABLE IF EXISTS `antispam_unicode_replacement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `antispam_unicode_replacement` (
  `from` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `to` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`from`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_transfers`
--

DROP TABLE IF EXISTS `character_transfers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_transfers` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(12) NOT NULL DEFAULT '',
  `source_realm_id` int(11) NOT NULL,
  `source_account_id` bigint(20) unsigned NOT NULL,
  `source_character_guid` bigint(20) unsigned NOT NULL,
  `data` longtext DEFAULT NULL,
  `target_realm_id` int(11) NOT NULL,
  `target_account_id` bigint(20) unsigned DEFAULT NULL,
  `status` enum('pending','in_progress','transferred','failed') NOT NULL DEFAULT 'pending',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `config`
--

DROP TABLE IF EXISTS `config`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `config` (
  `id` int(11) NOT NULL,
  `value` varchar(45) DEFAULT NULL,
  `comment` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `fingerprint_autoban`
--

DROP TABLE IF EXISTS `fingerprint_autoban`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `fingerprint_autoban` (
  `fingerprint` bigint(20) NOT NULL DEFAULT 0 COMMENT 'Account id',
  `banreason` varchar(255) NOT NULL DEFAULT 'Unknown',
  PRIMARY KEY (`fingerprint`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Fingerprint Ban List';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `fingerprint_banned`
--

DROP TABLE IF EXISTS `fingerprint_banned`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `fingerprint_banned` (
  `fingerprint` bigint(20) NOT NULL DEFAULT 0 COMMENT 'Account id',
  `bandate` bigint(20) NOT NULL DEFAULT 0,
  `unbandate` bigint(20) NOT NULL DEFAULT 0,
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  PRIMARY KEY (`fingerprint`,`bandate`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Fingerprint Ban List';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `geoip`
--

DROP TABLE IF EXISTS `geoip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `geoip` (
  `network_start_integer` int(11) DEFAULT NULL,
  `network_last_integer` int(11) DEFAULT NULL,
  `geoname_id` text DEFAULT NULL,
  `registered_country_geoname_id` text DEFAULT NULL,
  `represented_country_geoname_id` text DEFAULT NULL,
  `is_anonymous_proxy` int(11) DEFAULT NULL,
  `is_satellite_provider` int(11) DEFAULT NULL,
  `postal_code` text DEFAULT NULL,
  `latitude` double DEFAULT NULL,
  `longitude` double DEFAULT NULL,
  `accuracy_radius` int(11) DEFAULT NULL,
  KEY `ip_start` (`network_start_integer`) USING BTREE,
  KEY `ip_end` (`network_last_integer`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gp_history`
--

DROP TABLE IF EXISTS `gp_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gp_history` (
  `account_id` int(10) unsigned NOT NULL,
  `code` varchar(256) NOT NULL,
  PRIMARY KEY (`account_id`,`code`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `hwprint_autobans`
--

DROP TABLE IF EXISTS `hwprint_autobans`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `hwprint_autobans` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `extendedPrint` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `hwprint_marks`
--

DROP TABLE IF EXISTS `hwprint_marks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `hwprint_marks` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `extendedPrint` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ip_banned`
--

DROP TABLE IF EXISTS `ip_banned`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `ip_banned` (
  `ip` varchar(32) NOT NULL DEFAULT '0.0.0.0',
  `bandate` int(11) NOT NULL,
  `unbandate` int(11) NOT NULL,
  `bannedby` varchar(50) NOT NULL DEFAULT '[Console]',
  `banreason` varchar(128) NOT NULL DEFAULT 'no reason',
  PRIMARY KEY (`ip`,`bandate`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Banned IPs';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `migrations`
--

DROP TABLE IF EXISTS `migrations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `migrations` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(255) NOT NULL DEFAULT '0',
  `Hash` varchar(128) NOT NULL DEFAULT '0',
  `AppliedAt` datetime NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pending_commands`
--

DROP TABLE IF EXISTS `pending_commands`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pending_commands` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'auto incrementing identifier for the row',
  `realm_id` int(10) unsigned DEFAULT NULL COMMENT 'id of the realm that should run the command',
  `command` varchar(250) NOT NULL DEFAULT '' COMMENT 'full comand with parameters',
  `run_at_time` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'unixtime',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rbac_account_permissions`
--

DROP TABLE IF EXISTS `rbac_account_permissions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `rbac_account_permissions` (
  `account_id` int(11) NOT NULL,
  `permission_id` int(11) NOT NULL,
  `granted` tinyint(3) unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`account_id`,`permission_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rbac_command_permissions`
--

DROP TABLE IF EXISTS `rbac_command_permissions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `rbac_command_permissions` (
  `command` varchar(128) NOT NULL,
  `permission_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`command`,`permission_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_bin ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rbac_permissions`
--

DROP TABLE IF EXISTS `rbac_permissions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `rbac_permissions` (
  `id` int(10) unsigned NOT NULL,
  `name` varchar(64) NOT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `realmcharacters`
--

DROP TABLE IF EXISTS `realmcharacters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `realmcharacters` (
  `realmid` int(10) unsigned NOT NULL DEFAULT 0,
  `acctid` bigint(20) unsigned NOT NULL,
  `numchars` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`realmid`,`acctid`) USING BTREE,
  KEY `acctid` (`acctid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Realm Character Tracker';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `realmlist`
--

DROP TABLE IF EXISTS `realmlist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `realmlist` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL DEFAULT '',
  `address` varchar(32) NOT NULL DEFAULT '127.0.0.1',
  `port` int(11) NOT NULL DEFAULT 8085,
  `icon` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `realmflags` tinyint(3) unsigned NOT NULL DEFAULT 2 COMMENT 'Supported masks: 0x1 (invalid, not show in realm list), 0x2 (offline, set by mangosd), 0x4 (show version and build), 0x20 (new players), 0x40 (recommended)',
  `timezone` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `allowedSecurityLevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `population` float unsigned NOT NULL DEFAULT 0,
  `realmbuilds` varchar(64) NOT NULL DEFAULT '5875',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `idx_name` (`name`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Realm System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `shop_coins`
--

DROP TABLE IF EXISTS `shop_coins`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `shop_coins` (
  `id` int(10) unsigned NOT NULL,
  `coins` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = latin1 */ ;
/*!50003 SET character_set_results = latin1 */ ;
/*!50003 SET collation_connection  = latin1_swedish_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `shop_insert` AFTER INSERT ON `shop_coins` FOR EACH ROW BEGIN
INSERT INTO `shop_diff` (prev_bonus, new_bonus, accountid, date, query) VALUES (0, NEW.coins, NEW.id, UNIX_TIMESTAMP(now()), "INSERT");
END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = latin1 */ ;
/*!50003 SET character_set_results = latin1 */ ;
/*!50003 SET collation_connection  = latin1_swedish_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `shop_update` AFTER UPDATE ON `shop_coins` FOR EACH ROW BEGIN
IF OLD.coins != NEW.coins THEN  
INSERT INTO `shop_diff` (prev_bonus, new_bonus, accountid, date, query) VALUES (OLD.coins, NEW.coins, NEW.id, UNIX_TIMESTAMP(now()), "UPDATE");
END IF;
END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = latin1 */ ;
/*!50003 SET character_set_results = latin1 */ ;
/*!50003 SET collation_connection  = latin1_swedish_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `shop_delete` AFTER DELETE ON `shop_coins` FOR EACH ROW BEGIN
INSERT INTO `shop_diff` (prev_bonus, new_bonus, accountid, date, query) VALUES (OLD.coins, 0, OLD.id, UNIX_TIMESTAMP(now()), "DELETE");
END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `shop_coins_history`
--

DROP TABLE IF EXISTS `shop_coins_history`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `shop_coins_history` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `account_id` int(10) unsigned NOT NULL,
  `points` int(11) NOT NULL,
  `actual_points` int(11) NOT NULL,
  `new_points` int(11) NOT NULL,
  `type` varchar(256) NOT NULL,
  `system` varchar(256) NOT NULL,
  `reference` varchar(256) NOT NULL,
  `date` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `shop_diff`
--

DROP TABLE IF EXISTS `shop_diff`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `shop_diff` (
  `guid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `accountid` int(10) unsigned NOT NULL DEFAULT 0,
  `prev_bonus` int(11) NOT NULL,
  `new_bonus` int(11) NOT NULL,
  `date` int(10) unsigned NOT NULL DEFAULT 0,
  `query` varchar(32) DEFAULT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=59 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `shop_logs`
--

DROP TABLE IF EXISTS `shop_logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `shop_logs` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `time` timestamp NOT NULL DEFAULT current_timestamp(),
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `account` int(10) unsigned NOT NULL DEFAULT 0,
  `item` int(10) unsigned NOT NULL DEFAULT 0,
  `price` int(10) unsigned NOT NULL DEFAULT 0,
  `refunded` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `realm_id` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `account` (`account`) USING BTREE,
  KEY `time` (`time`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=2000058 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `statistics_online`
--

DROP TABLE IF EXISTS `statistics_online`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `statistics_online` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `online` int(10) unsigned DEFAULT 0,
  `online_alli` int(10) unsigned DEFAULT 0,
  `online_horde` int(10) unsigned DEFAULT 0,
  `connections` int(10) unsigned DEFAULT 0,
  `realm` tinyint(3) unsigned DEFAULT 0,
  `date` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `statistics_online_old`
--

DROP TABLE IF EXISTS `statistics_online_old`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `statistics_online_old` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `online` int(10) unsigned DEFAULT 0,
  `online_alli` int(11) DEFAULT 0,
  `online_horde` int(11) DEFAULT 0,
  `connections` int(10) unsigned DEFAULT 0,
  `realm` tinyint(3) unsigned DEFAULT 0,
  `date` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `system_fingerprint_usage`
--

DROP TABLE IF EXISTS `system_fingerprint_usage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `system_fingerprint_usage` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `fingerprint` int(10) unsigned NOT NULL,
  `account` int(10) unsigned NOT NULL,
  `ip` varchar(16) NOT NULL,
  `realm` int(10) unsigned NOT NULL,
  `time` timestamp NOT NULL DEFAULT current_timestamp(),
  `architecture` varchar(16) DEFAULT NULL,
  `cputype` varchar(64) DEFAULT NULL,
  `activecpus` int(10) unsigned DEFAULT NULL,
  `totalcpus` int(10) unsigned DEFAULT NULL,
  `pagesize` int(10) unsigned DEFAULT NULL,
  `timezoneBias` int(10) unsigned NOT NULL,
  `largepageMinimum` int(10) unsigned NOT NULL,
  `suiteMask` int(10) unsigned NOT NULL,
  `mitigationPolicies` int(10) unsigned NOT NULL,
  `numberPhysicalPages` int(10) unsigned NOT NULL,
  `sharedDataFlags` int(10) unsigned NOT NULL,
  `testRestInstruction` bigint(20) unsigned NOT NULL,
  `qpcFrequency` bigint(20) NOT NULL,
  `qpcSystemTimeIncrement` bigint(20) unsigned NOT NULL,
  `unparkedProcessorCount` int(10) unsigned NOT NULL,
  `enclaveFeatureMask` int(10) unsigned NOT NULL,
  `qpcData` int(10) unsigned NOT NULL,
  `timeZoneId` int(10) unsigned NOT NULL,
  `osVersion` enum('None','WinXP','Win7','Win8','Vista','Win10Up','<Unknown>') NOT NULL DEFAULT '<Unknown>',
  `extendedHash` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `fingerprint` (`fingerprint`) USING BTREE,
  KEY `account` (`account`) USING BTREE,
  KEY `ip` (`ip`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `system_fingerprint_usage_archive`
--

DROP TABLE IF EXISTS `system_fingerprint_usage_archive`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `system_fingerprint_usage_archive` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `fingerprint` int(10) unsigned NOT NULL,
  `account` int(10) unsigned NOT NULL,
  `ip` varchar(16) NOT NULL,
  `realm` int(10) unsigned NOT NULL,
  `time` datetime NOT NULL DEFAULT current_timestamp(),
  `architecture` varchar(16) DEFAULT NULL,
  `cputype` varchar(64) DEFAULT NULL,
  `activecpus` int(10) unsigned DEFAULT NULL,
  `totalcpus` int(10) unsigned DEFAULT NULL,
  `pagesize` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `fingerprint` (`fingerprint`) USING BTREE,
  KEY `account` (`account`) USING BTREE,
  KEY `ip` (`ip`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `uptime`
--

DROP TABLE IF EXISTS `uptime`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `uptime` (
  `realmid` int(10) unsigned NOT NULL,
  `starttime` bigint(20) unsigned NOT NULL DEFAULT 0,
  `startstring` varchar(64) NOT NULL DEFAULT '',
  `uptime` bigint(20) unsigned NOT NULL DEFAULT 0,
  `onlineplayers` smallint(5) unsigned NOT NULL DEFAULT 0,
  `maxplayers` smallint(5) unsigned NOT NULL DEFAULT 0,
  `queue` smallint(5) unsigned NOT NULL DEFAULT 0,
  `maxqueue` smallint(5) unsigned NOT NULL DEFAULT 0,
  `revision` varchar(255) NOT NULL DEFAULT 'Turtle WoW',
  PRIMARY KEY (`realmid`,`starttime`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Uptime system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `world_config`
--

DROP TABLE IF EXISTS `world_config`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `world_config` (
  `realm_id` int(11) unsigned NOT NULL,
  `compression` int(10) unsigned NOT NULL DEFAULT 0,
  `login_queue_grace_period_secs` int(10) unsigned NOT NULL DEFAULT 0,
  `character_screen_max_idle_time` int(10) unsigned NOT NULL DEFAULT 0,
  `player_hard_limit` int(10) unsigned NOT NULL DEFAULT 0,
  `antiflood_sanction` int(10) unsigned NOT NULL DEFAULT 0,
  `packet_bcast_threads` int(10) unsigned NOT NULL DEFAULT 0,
  `packet_bcast_frequency` int(10) unsigned NOT NULL DEFAULT 0,
  `mailspam_expire_secs` int(10) unsigned NOT NULL DEFAULT 0,
  `mailspam_max_mails` int(10) unsigned NOT NULL DEFAULT 0,
  `mailspam_level` int(10) unsigned NOT NULL DEFAULT 0,
  `mailspam_account_level` int(10) unsigned NOT NULL DEFAULT 0,
  `mailspam_money` int(10) unsigned NOT NULL DEFAULT 0,
  `empty_maps_update_time` int(10) unsigned NOT NULL DEFAULT 0,
  `cod_force_tag_max_level` int(10) unsigned NOT NULL DEFAULT 0,
  `pub_chans_mute_vanish_level` int(10) unsigned NOT NULL DEFAULT 0,
  `gmtickets_admin_security` int(10) unsigned NOT NULL DEFAULT 0,
  `gmtickets_minlevel` int(10) unsigned NOT NULL DEFAULT 0,
  `battleground_queues_count` int(10) unsigned NOT NULL DEFAULT 0,
  `corpses_update_minutes` int(10) unsigned NOT NULL DEFAULT 0,
  `bones_expire_minutes` int(10) unsigned NOT NULL DEFAULT 0,
  `async_tasks_threads_count` int(10) unsigned NOT NULL DEFAULT 0,
  `av_min_players_in_queue` int(10) unsigned NOT NULL DEFAULT 0,
  `av_initial_max_players` int(10) unsigned NOT NULL DEFAULT 0,
  `inactive_players_skip_updates` int(10) unsigned NOT NULL DEFAULT 0,
  `item_instantsave_quality` int(10) unsigned NOT NULL DEFAULT 0,
  `item_rareloot_quality` int(10) unsigned NOT NULL DEFAULT 0,
  `whisp_diff_zone_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `channel_invite_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `world_chan_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `world_chan_cd` int(10) unsigned NOT NULL DEFAULT 0,
  `world_chan_cd_max_level` int(10) unsigned NOT NULL DEFAULT 0,
  `world_chan_cd_scaling` int(10) unsigned NOT NULL DEFAULT 0,
  `world_chan_cd_use_account_max_level` int(10) unsigned NOT NULL DEFAULT 0,
  `say_emote_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `say_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `yell_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `yellrange_min` int(10) unsigned NOT NULL DEFAULT 0,
  `whisper_targets_max` int(10) unsigned NOT NULL DEFAULT 0,
  `whisper_targets_bypass_level` int(10) unsigned NOT NULL DEFAULT 0,
  `whisper_targets_decay` int(10) unsigned NOT NULL DEFAULT 0,
  `yellrange_linearscale_maxlevel` int(10) unsigned NOT NULL DEFAULT 0,
  `yellrange_quadraticscale_maxlevel` int(10) unsigned NOT NULL DEFAULT 0,
  `dyn_respawn_players_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `dyn_respawn_players_leveldiff` int(10) unsigned NOT NULL DEFAULT 0,
  `dyn_respawn_min_respawn_time` int(10) unsigned NOT NULL DEFAULT 0,
  `dyn_respawn_min_respawn_time_elite` int(10) unsigned NOT NULL DEFAULT 0,
  `dyn_respawn_min_respawn_time_indoors` int(10) unsigned NOT NULL DEFAULT 0,
  `dyn_respawn_affect_respawn_time_below` int(10) unsigned NOT NULL DEFAULT 0,
  `dyn_respawn_affect_level_below` int(10) unsigned NOT NULL DEFAULT 0,
  `mtcells_threads` int(10) unsigned NOT NULL DEFAULT 0,
  `mtcells_safedistance` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_instanced_update_threads` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_update_packets_diff` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_update_players_diff` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_update_cells_diff` int(10) unsigned NOT NULL DEFAULT 0,
  `log_money_trades_treshold` int(10) unsigned NOT NULL DEFAULT 0,
  `relocation_vmap_check_timer` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_tick_lower_visibility_distance` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_tick_increase_visibility_distance` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_min_visibility_distance` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_tick_lower_grid_activation_distance` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_tick_increase_grid_activation_distance` int(10) unsigned NOT NULL DEFAULT 0,
  `pbcast_diff_lower_visibility_distance` int(10) unsigned NOT NULL DEFAULT 0,
  `mapupdate_min_grid_activation_distance` int(10) unsigned NOT NULL DEFAULT 0,
  `continents_motionupdate_threads` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_world_update` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_map_update` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_mapsystem_update` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_sessions_update` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_unique_session_update` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_async_queries` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_packet` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_map_packets` int(10) unsigned NOT NULL DEFAULT 0,
  `perflog_slow_packet_bcast` int(10) unsigned NOT NULL DEFAULT 0,
  `async_queries_tick_timeout` int(10) unsigned NOT NULL DEFAULT 0,
  `login_per_tick` int(10) unsigned NOT NULL DEFAULT 0,
  `anticrash_rearm_timer` int(10) unsigned NOT NULL DEFAULT 0,
  `anticrash_options` int(10) unsigned NOT NULL DEFAULT 0,
  `max_points_per_mvt_packet` int(10) unsigned NOT NULL DEFAULT 0,
  `debuff_limit` int(10) unsigned NOT NULL DEFAULT 0,
  `spells_ccdelay` int(10) unsigned NOT NULL DEFAULT 0,
  `pet_default_loyalty` int(10) unsigned NOT NULL DEFAULT 0,
  `map_objectsupdate_threads` int(10) unsigned NOT NULL DEFAULT 0,
  `map_objectsupdate_timeout` int(10) unsigned NOT NULL DEFAULT 0,
  `map_visibilityupdate_threads` int(10) unsigned NOT NULL DEFAULT 0,
  `map_visibilityupdate_timeout` int(10) unsigned NOT NULL DEFAULT 0,
  `interval_save` int(10) unsigned NOT NULL DEFAULT 0,
  `interval_gridclean` int(10) unsigned NOT NULL DEFAULT 0,
  `interval_mapupdate` int(10) unsigned NOT NULL DEFAULT 0,
  `interval_changeweather` int(10) unsigned NOT NULL DEFAULT 0,
  `port_world` int(10) unsigned NOT NULL DEFAULT 0,
  `game_type` int(10) unsigned NOT NULL DEFAULT 0,
  `realm_zone` int(10) unsigned NOT NULL DEFAULT 0,
  `strict_player_names` int(10) unsigned NOT NULL DEFAULT 0,
  `strict_charter_names` int(10) unsigned NOT NULL DEFAULT 0,
  `strict_pet_names` int(10) unsigned NOT NULL DEFAULT 0,
  `min_player_name` int(10) unsigned NOT NULL DEFAULT 0,
  `min_charter_name` int(10) unsigned NOT NULL DEFAULT 0,
  `min_pet_name` int(10) unsigned NOT NULL DEFAULT 0,
  `characters_creating_disabled` int(10) unsigned NOT NULL DEFAULT 0,
  `characters_per_account` int(10) unsigned NOT NULL DEFAULT 0,
  `characters_per_realm` int(10) unsigned NOT NULL DEFAULT 0,
  `skip_cinematics` int(10) unsigned NOT NULL DEFAULT 0,
  `max_player_level` int(10) unsigned NOT NULL DEFAULT 0,
  `start_player_level` int(10) unsigned NOT NULL DEFAULT 0,
  `start_player_money` int(10) unsigned NOT NULL DEFAULT 0,
  `max_honor_points` int(10) unsigned NOT NULL DEFAULT 0,
  `start_honor_points` int(10) unsigned NOT NULL DEFAULT 0,
  `min_honor_kills` int(10) unsigned NOT NULL DEFAULT 0,
  `instance_reset_time_hour` int(10) unsigned NOT NULL DEFAULT 0,
  `instance_unload_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `max_spell_casts_in_chain` int(10) unsigned NOT NULL DEFAULT 0,
  `max_primary_trade_skill` int(10) unsigned NOT NULL DEFAULT 0,
  `min_petition_signs` int(10) unsigned NOT NULL DEFAULT 0,
  `gm_login_state` int(10) unsigned NOT NULL DEFAULT 0,
  `gm_visible_state` int(10) unsigned NOT NULL DEFAULT 0,
  `gm_accept_tickets` int(10) unsigned NOT NULL DEFAULT 0,
  `gm_chat` int(10) unsigned NOT NULL DEFAULT 0,
  `gm_wispering_to` int(10) unsigned NOT NULL DEFAULT 0,
  `gm_level_in_gm_list` int(10) unsigned NOT NULL DEFAULT 0,
  `gm_level_in_who_list` int(10) unsigned NOT NULL DEFAULT 0,
  `start_gm_level` int(10) unsigned NOT NULL DEFAULT 0,
  `group_visibility` int(10) unsigned NOT NULL DEFAULT 0,
  `mail_delivery_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `mass_mailer_send_per_tick` int(10) unsigned NOT NULL DEFAULT 0,
  `mail_max_per_hour` int(10) unsigned NOT NULL DEFAULT 0,
  `uptime_update` int(10) unsigned NOT NULL DEFAULT 0,
  `auction_deposit_min` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_chance_orange` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_chance_yellow` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_chance_green` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_chance_grey` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_chance_mining_steps` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_chance_skinning_steps` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_gain_crafting` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_gain_defense` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_gain_gathering` int(10) unsigned NOT NULL DEFAULT 0,
  `skill_gain_weapon` int(10) unsigned NOT NULL DEFAULT 0,
  `max_overspeed_pings` int(10) unsigned NOT NULL DEFAULT 0,
  `chatflood_message_count` int(10) unsigned NOT NULL DEFAULT 0,
  `chatflood_message_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `chatflood_mute_time` int(10) unsigned NOT NULL DEFAULT 0,
  `creature_family_assistance_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `creature_family_flee_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `world_boss_level_diff` int(10) unsigned NOT NULL DEFAULT 0,
  `chat_strict_link_checking_severity` int(10) unsigned NOT NULL DEFAULT 0,
  `chat_strict_link_checking_kick` int(10) unsigned NOT NULL DEFAULT 0,
  `corpse_decay_normal` int(10) unsigned NOT NULL DEFAULT 0,
  `corpse_decay_rare` int(10) unsigned NOT NULL DEFAULT 0,
  `corpse_decay_elite` int(10) unsigned NOT NULL DEFAULT 0,
  `corpse_decay_rareelite` int(10) unsigned NOT NULL DEFAULT 0,
  `corpse_decay_worldboss` int(10) unsigned NOT NULL DEFAULT 0,
  `instant_logout` int(10) unsigned NOT NULL DEFAULT 0,
  `battleground_invitation_type` int(10) unsigned NOT NULL DEFAULT 0,
  `battleground_premade_queue_group_min_size` int(10) unsigned NOT NULL DEFAULT 0,
  `battleground_premature_finish_timer` int(10) unsigned NOT NULL DEFAULT 0,
  `battleground_premade_group_wait_for_match` int(10) unsigned NOT NULL DEFAULT 0,
  `battleground_queue_announcer_join` int(10) unsigned NOT NULL DEFAULT 0,
  `group_offline_leader_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `guild_event_log_count` int(10) unsigned NOT NULL DEFAULT 0,
  `mirrortimer_fatigue_max` int(10) unsigned NOT NULL DEFAULT 0,
  `mirrortimer_breath_max` int(10) unsigned NOT NULL DEFAULT 0,
  `mirrortimer_environmental_max` int(10) unsigned NOT NULL DEFAULT 0,
  `environmental_damage_min` int(10) unsigned NOT NULL DEFAULT 0,
  `environmental_damage_max` int(10) unsigned NOT NULL DEFAULT 0,
  `min_level_stat_save` int(10) unsigned NOT NULL DEFAULT 0,
  `maintenance_day` int(10) unsigned NOT NULL DEFAULT 0,
  `chardelete_keep_days` int(10) unsigned NOT NULL DEFAULT 0,
  `chardelete_method` int(10) unsigned NOT NULL DEFAULT 0,
  `chardelete_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `guid_reserve_size_creature` int(10) unsigned NOT NULL DEFAULT 0,
  `guid_reserve_size_gameobject` int(10) unsigned NOT NULL DEFAULT 0,
  `respec_base_cost` int(10) unsigned NOT NULL DEFAULT 0,
  `respec_multiplicative_cost` int(10) unsigned NOT NULL DEFAULT 0,
  `respec_min_multiplier` int(10) unsigned NOT NULL DEFAULT 0,
  `respec_max_multiplier` int(10) unsigned NOT NULL DEFAULT 0,
  `battleground_group_limit` int(10) unsigned NOT NULL DEFAULT 0,
  `creature_summon_limit` int(10) unsigned NOT NULL DEFAULT 0,
  `war_effort_autocomplete_period` int(10) unsigned NOT NULL DEFAULT 0,
  `account_concurrent_auction_limit` int(10) unsigned NOT NULL DEFAULT 0,
  `banlist_reload_timer` int(10) unsigned NOT NULL DEFAULT 0,
  `autobroadcast_interval` int(10) unsigned NOT NULL DEFAULT 0,
  `transmog_req_item` int(10) unsigned NOT NULL DEFAULT 0,
  `transmog_req_item_count` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_ban_duration` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_reverse_time_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_reverse_time_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_null_time_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_null_time_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_skipped_heartbeats_threshold_tick` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_skipped_heartbeats_threshold_total` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_skipped_heartbeats_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_time_desync_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_time_desync_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_num_desyncs_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_num_desyncs_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_overspeed_distance_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_overspeed_distance_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_overspeed_jump_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_overspeed_jump_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_jump_speed_change_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_jump_speed_change_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_multi_jump_threshold_tick` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_multi_jump_threshold_total` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_multi_jump_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wall_climb_threshold_tick` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wall_climb_threshold_total` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wall_climb_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_unreachable_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_unreachable_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fly_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fly_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_no_fall_time_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_no_fall_time_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_teleport_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_teleport_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_tele_to_transport_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_tele_to_transport_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fake_transport_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fake_transport_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_water_walk_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_water_walk_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_slow_fall_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_slow_fall_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_hover_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_hover_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fixed_z_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fixed_z_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_root_move_threshold_tick` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_root_move_threshold_total` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_root_move_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_self_root_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_self_root_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wrong_ack_data_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wrong_ack_data_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_pending_ack_delay_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_pending_ack_delay_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_explore_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_explore_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_explore_high_level_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_explore_high_level_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_forbidden_area_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_forbidden_area_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `movement_change_ack_time` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_warden_client_response_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_warden_client_check_holdoff` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_warden_default_penalty` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_warden_client_ban_duration` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_warden_num_mem_checks` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_warden_num_other_checks` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_warden_db_loglevel` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_max_restriction_level` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_original_normalize_mask` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_fully_normalize_mask` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_score_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_mutetime` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_chat_mask` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_detect_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_repeat_count` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_update_timer` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_message_block_size` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_frequency_time` int(10) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_frequency_count` int(10) unsigned NOT NULL DEFAULT 0,
  `bg_sv_spark_max_count` int(10) unsigned NOT NULL DEFAULT 0,
  `item_log_restore_quality` int(10) unsigned NOT NULL DEFAULT 0,
  `chat_min_level` int(10) unsigned NOT NULL DEFAULT 0,
  `auto_commit_minutes` int(10) unsigned NOT NULL DEFAULT 0,
  `account_data_last_login_days` int(10) unsigned NOT NULL DEFAULT 0,
  `password_change_reward_item` int(10) unsigned NOT NULL DEFAULT 0,
  `max_age_show_warning` int(10) unsigned NOT NULL DEFAULT 0,
  `high_level_character` int(10) unsigned NOT NULL DEFAULT 0,
  `account_trusted_ip_percentage` int(10) unsigned NOT NULL DEFAULT 0,
  `analysis_no_fingerprint_match_weight` int(10) unsigned NOT NULL DEFAULT 0,
  `analysis_no_extended_data_match_weight` int(10) unsigned NOT NULL DEFAULT 0,
  `analysis_no_cpu_data_weight` int(10) unsigned NOT NULL DEFAULT 0,
  `analysis_no_cpu_data_match_weight` int(10) unsigned NOT NULL DEFAULT 0,
  `analysis_no_extended_data_weight` int(10) unsigned NOT NULL DEFAULT 0,
  `analysis_warning_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `auto_restart_max_server_uptime` int(10) unsigned NOT NULL DEFAULT 0,
  `auto_restart_hour_min` int(10) unsigned NOT NULL DEFAULT 0,
  `auto_restart_hour_max` int(10) unsigned NOT NULL DEFAULT 0,
  `diff_hc_protection` int(10) unsigned NOT NULL DEFAULT 0,
  `login_region_queue_level_threshold` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_priority_per_tick` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_donator_settings` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_donator_priority` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_western_priority` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_high_level_char` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_high_level_char_priority` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_priority_per_account_day` int(10) unsigned NOT NULL DEFAULT 0,
  `priority_queue_priority_reduction_multibox` int(10) unsigned NOT NULL DEFAULT 0,
  `max_percentage_pop_non_regional` int(10) unsigned NOT NULL DEFAULT 0,
  `max_percentage_pop_regional` int(10) unsigned NOT NULL DEFAULT 0,
  `auto_pdump_min_char_level` int(10) unsigned NOT NULL DEFAULT 0,
  `auto_pdump_delete_after_days` int(10) unsigned NOT NULL DEFAULT 0,
  `content_phase` int(10) unsigned NOT NULL DEFAULT 0,
  `shop_refund_window` int(10) unsigned NOT NULL DEFAULT 0,
  `performance_report_interval` int(10) unsigned NOT NULL DEFAULT 0,
  `max_gold_transferred` int(10) unsigned NOT NULL DEFAULT 0,
  `max_item_stack_transferred` int(10) unsigned NOT NULL DEFAULT 0,
  `dynamic_scaling_pop` int(10) unsigned NOT NULL DEFAULT 0,
  `death_sickness_level` int(11) NOT NULL DEFAULT 0,
  `quest_low_level_hide_diff` int(11) NOT NULL DEFAULT 0,
  `quest_high_level_hide_diff` int(11) NOT NULL DEFAULT 0,
  `ac_anticheat_max_allowed_desync` int(11) NOT NULL DEFAULT 0,
  `rate_health` float NOT NULL DEFAULT 0,
  `max_creature_attack_radius` float NOT NULL DEFAULT 0,
  `max_players_stealth_detect_range` float NOT NULL DEFAULT 0,
  `dyn_respawn_check_range` float NOT NULL DEFAULT 0,
  `dyn_respawn_percent_per_player` float NOT NULL DEFAULT 0,
  `dyn_respawn_max_reduction_rate` float NOT NULL DEFAULT 0,
  `rate_power_mana` float NOT NULL DEFAULT 0,
  `rate_power_rage_income` float NOT NULL DEFAULT 0,
  `rate_power_rage_loss` float NOT NULL DEFAULT 0,
  `rate_power_focus` float NOT NULL DEFAULT 0,
  `rate_power_energy` float NOT NULL DEFAULT 0,
  `rate_skill_discovery` float NOT NULL DEFAULT 0,
  `rate_drop_item_poor` float NOT NULL DEFAULT 0,
  `rate_drop_item_normal` float NOT NULL DEFAULT 0,
  `rate_drop_item_uncommon` float NOT NULL DEFAULT 0,
  `rate_drop_item_rare` float NOT NULL DEFAULT 0,
  `rate_drop_item_epic` float NOT NULL DEFAULT 0,
  `rate_drop_item_legendary` float NOT NULL DEFAULT 0,
  `rate_drop_item_artifact` float NOT NULL DEFAULT 0,
  `rate_drop_item_referenced` float NOT NULL DEFAULT 0,
  `rate_drop_money` float NOT NULL DEFAULT 0,
  `rate_xp_kill` float NOT NULL DEFAULT 0,
  `rate_xp_kill_elite` float NOT NULL DEFAULT 0,
  `rate_xp_quest` float NOT NULL DEFAULT 0,
  `rate_xp_explore` float NOT NULL DEFAULT 0,
  `rate_reputation_gain` float NOT NULL DEFAULT 0,
  `rate_reputation_lowlevel_kill` float NOT NULL DEFAULT 0,
  `rate_reputation_lowlevel_quest` float NOT NULL DEFAULT 0,
  `rate_creature_normal_hp` float NOT NULL DEFAULT 0,
  `rate_creature_elite_elite_hp` float NOT NULL DEFAULT 0,
  `rate_creature_elite_rareelite_hp` float NOT NULL DEFAULT 0,
  `rate_creature_elite_worldboss_hp` float NOT NULL DEFAULT 0,
  `rate_creature_elite_rare_hp` float NOT NULL DEFAULT 0,
  `rate_creature_normal_damage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_elite_damage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_rareelite_damage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_worldboss_damage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_rare_damage` float NOT NULL DEFAULT 0,
  `rate_creature_normal_spelldamage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_elite_spelldamage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_rareelite_spelldamage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_worldboss_spelldamage` float NOT NULL DEFAULT 0,
  `rate_creature_elite_rare_spelldamage` float NOT NULL DEFAULT 0,
  `rate_creature_aggro` float NOT NULL DEFAULT 0,
  `rate_rest_ingame` float NOT NULL DEFAULT 0,
  `rate_rest_offline_in_tavern_or_city` float NOT NULL DEFAULT 0,
  `rate_rest_offline_in_wilderness` float NOT NULL DEFAULT 0,
  `rate_damage_fall` float NOT NULL DEFAULT 0,
  `rate_auction_time` float NOT NULL DEFAULT 0,
  `rate_auction_deposit` float NOT NULL DEFAULT 0,
  `rate_auction_cut` float NOT NULL DEFAULT 0,
  `rate_honor` float NOT NULL DEFAULT 0,
  `rate_mining_amount` float NOT NULL DEFAULT 0,
  `rate_mining_next` float NOT NULL DEFAULT 0,
  `rate_talent` float NOT NULL DEFAULT 0,
  `rate_loyalty` float NOT NULL DEFAULT 0,
  `rate_corpse_decay_looted` float NOT NULL DEFAULT 0,
  `rate_instance_reset_time` float NOT NULL DEFAULT 0,
  `rate_target_pos_recalculation_range` float NOT NULL DEFAULT 0,
  `rate_durability_loss_damage` float NOT NULL DEFAULT 0,
  `rate_durability_loss_parry` float NOT NULL DEFAULT 0,
  `rate_durability_loss_absorb` float NOT NULL DEFAULT 0,
  `rate_durability_loss_block` float NOT NULL DEFAULT 0,
  `listen_range_say` float NOT NULL DEFAULT 0,
  `listen_range_yell` float NOT NULL DEFAULT 0,
  `listen_range_textemote` float NOT NULL DEFAULT 0,
  `creature_family_flee_assistance_radius` float NOT NULL DEFAULT 0,
  `creature_family_assistance_radius` float NOT NULL DEFAULT 0,
  `group_xp_distance` float NOT NULL DEFAULT 0,
  `threat_radius` float NOT NULL DEFAULT 0,
  `ghost_run_speed_world` float NOT NULL DEFAULT 0,
  `ghost_run_speed_bg` float NOT NULL DEFAULT 0,
  `rate_war_effort_resource` float NOT NULL DEFAULT 0,
  `transmog_req_money_rate` float NOT NULL DEFAULT 0,
  `ac_movement_cheat_teleport_distance` float NOT NULL DEFAULT 0,
  `ac_movement_cheat_wall_climb_angle` float NOT NULL DEFAULT 0,
  `battleground_reputation_rate_av` float NOT NULL DEFAULT 0,
  `battleground_reputation_rate_ws` float NOT NULL DEFAULT 0,
  `battleground_reputation_rate_ab` float NOT NULL DEFAULT 0,
  `battleground_reputation_rate_sv` float NOT NULL DEFAULT 0,
  `battleground_honor_rate_av` float NOT NULL DEFAULT 0,
  `battleground_honor_rate_ws` float NOT NULL DEFAULT 0,
  `battleground_honor_rate_ab` float NOT NULL DEFAULT 0,
  `battleground_honor_rate_sv` float NOT NULL DEFAULT 0,
  `suspicious_movementspeed_report_threshold` float NOT NULL DEFAULT 0,
  `max_faction_imbalance` float NOT NULL DEFAULT 0,
  `open_world_honor_multiplier` float NOT NULL DEFAULT 0,
  `grid_unload` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `object_health_value_show` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `gms_allow_public_channels` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `gmtickets_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `tag_in_battlegrounds` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `continents_instanciate` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `gm_join_opposite_faction_channels` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `gm_allow_trades` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `die_command_credit` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `terrain_preload_continents` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `terrain_preload_instances` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `cleanup_terrain` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `outdoorpvp_ep_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `outdoorpvp_si_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `mmap_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `player_commands` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `save_respawn_time_immediately` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_accounts` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_interaction_chat` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_interaction_channel` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_interaction_group` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_interaction_guild` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_interaction_trade` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_interaction_auction` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_interaction_mail` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_who_list` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `allow_two_side_add_friend` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `instance_ignore_level` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `instance_ignore_raid` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `cast_unstuck` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `gm_log_trade` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `gm_lower_security` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `always_max_skill_for_level` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `weather` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `event_announce` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `quest_ignore_raid` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `detect_pos_collision` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `restricted_lfg_channel` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `silently_gm_join_to_channel` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `strict_latin_in_general_channels` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `chat_fake_message_preventing` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `addon_channel` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `corpse_empty_loot_show` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `death_corpse_reclaim_delay_pvp` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `death_corpse_reclaim_delay_pve` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `death_bones_world` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `death_bones_bg` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `all_taxi_paths` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `skill_fail_loot_fishing` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `skill_fail_gain_fishing` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `skill_fail_possible_fishingpool` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `battleground_cast_deserter` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `battleground_queue_announcer_start` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `kick_player_on_bad_packet` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `pet_los` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `stats_save_only_on_logout` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `clean_character_db` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `vmap_indoor_check` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `pet_unsummon_at_mount` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_dk` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_movement_extrapolation_player` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_movement_extrapolation_pet` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `whisper_restriction` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `mailspam_item` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_pvp_equip_requirements` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_pvp_purchase_requirements` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_pvp_timeline` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_pvp_rewards` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `battleground_randomize` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_crossfaction_battlegrounds` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_gear_rating_queue` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `send_loot_roll_upon_reconnect` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_pets` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_spell_effects` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_pve_events` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `accurate_lfg` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `no_respec_price_decay` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `no_quest_xp_to_gold` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `auto_honor_restart` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `restore_deleted_items` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `unlinked_auction_houses` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `prevent_item_datamining` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `transmog_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `static_object_los` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `dual_spec` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `hardcore_disable_duel` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_players_only` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_notify_cheaters` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_log_data` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_reverse_time_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_null_time_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_skipped_heartbeats_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_time_desync_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_num_desyncs_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_speed_hack_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_overspeed_distance_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_overspeed_jump_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_overspeed_jump_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_jump_speed_change_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_jump_speed_change_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_multi_jump_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_multi_jump_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wall_climb_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wall_climb_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_unreachable_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fly_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fly_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_no_fall_time_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_no_fall_time_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_teleport_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_teleport_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_tele_to_transport_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_tele_to_transport_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fake_transport_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_water_walk_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_water_walk_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_slow_fall_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_slow_fall_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_hover_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_hover_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fixed_z_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_fixed_z_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_root_move_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_root_move_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_self_root_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_self_root_reject` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_wrong_ack_data_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_pending_ack_delay_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_explore_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_explore_high_level_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_movement_cheat_forbidden_area_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_warden_win_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_warden_osx_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_warden_players_only` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_ban_enabled` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ac_antispam_merge_all_whispers` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `visibility_force_active_objects` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `ptr` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `gm_start_on_gm_island` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `item_log_restore_quest_items` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enforced_english` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `sea_network` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `backup_character_inventory` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `analysis_stop_on_correct_extended_data` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `analysis_do_shared_data_detailed_report` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `analysis_allow_relaxed_ip` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `analysis_log_discord_summary` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `analysis_ping_on_warning` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `analysis_automatic_punihsment` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `suspicious_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `suspicious_movement_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `suspicious_fishing_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `suspicious_npc_killed_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `login_region_queue` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_priority_queue` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `priority_queue_enable_western_priority` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_dynamic_visibilities` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `priority_queue_enable_ip_penalty` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `load_locales` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `enable_faction_balance` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `block_all_hanzi` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `holiday_event` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `performance_enable` tinyint(1) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`realm_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `tw_logs`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `tw_logs` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */;

USE `tw_logs`;

--
-- Table structure for table `character_loot`
--

DROP TABLE IF EXISTS `character_loot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_loot` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `date_time` timestamp NOT NULL DEFAULT current_timestamp(),
  `receiver_name` varchar(20) NOT NULL,
  `receiver_guid` int(11) unsigned NOT NULL DEFAULT 0,
  `receiver_account_id` int(11) unsigned NOT NULL DEFAULT 0,
  `receiver_ip` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '0.0.0.0',
  `source_type` enum('Creature','GameObject','Item') NOT NULL,
  `source_guid` int(11) unsigned NOT NULL DEFAULT 0,
  `source_entry` int(11) unsigned NOT NULL DEFAULT 0,
  `money` int(11) unsigned NOT NULL DEFAULT 0,
  `item_entry` int(11) unsigned NOT NULL DEFAULT 0,
  `item_count` int(11) unsigned NOT NULL DEFAULT 0,
  `loot_type` enum('Kill','Roll','Profession','Container') NOT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `receiver_guid` (`receiver_guid`) USING BTREE,
  KEY `source_entry` (`source_entry`) USING BTREE,
  KEY `item_entry` (`item_entry`) USING BTREE,
  FULLTEXT KEY `receiver_name` (`receiver_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_520_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Current Database: `tw_world`
--

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `tw_world` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */;

USE `tw_world`;

--
-- Table structure for table `achievement`
--

DROP TABLE IF EXISTS `achievement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `achievement` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredFaction` int(11) NOT NULL DEFAULT 0,
  `mapId` int(11) NOT NULL DEFAULT 0,
  `parentAchievement` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `description1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `descriptionFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `categoryId` int(10) unsigned NOT NULL DEFAULT 0,
  `points` int(10) unsigned NOT NULL DEFAULT 0,
  `orderInCategory` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `icon` int(10) unsigned NOT NULL DEFAULT 0,
  `titleReward1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleReward16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `titleRewardFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `count` int(10) unsigned NOT NULL DEFAULT 0,
  `refAchievement` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `achievement_category`
--

DROP TABLE IF EXISTS `achievement_category`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `achievement_category` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `parentCategory` int(11) NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `sortOrder` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `achievement_criteria`
--

DROP TABLE IF EXISTS `achievement_criteria`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `achievement_criteria` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `referredAchievement` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredType` int(10) unsigned NOT NULL DEFAULT 0,
  `assetType` int(10) unsigned NOT NULL DEFAULT 0,
  `assetCount` int(10) unsigned NOT NULL DEFAULT 0,
  `startEvent` int(10) unsigned NOT NULL DEFAULT 0,
  `startAsset` int(10) unsigned NOT NULL DEFAULT 0,
  `failEvent` int(10) unsigned NOT NULL DEFAULT 0,
  `failAsset` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `timedType` int(10) unsigned NOT NULL DEFAULT 0,
  `timerStartEvent` int(10) unsigned NOT NULL DEFAULT 0,
  `timeLimit` int(10) unsigned NOT NULL DEFAULT 0,
  `showOrder` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `animationdata`
--

DROP TABLE IF EXISTS `animationdata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `animationdata` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `WeaponFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `BodyFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `FallbackAnimationID` int(10) unsigned NOT NULL DEFAULT 0,
  `BehaviourID` int(10) unsigned NOT NULL DEFAULT 0,
  `BehaviourTier` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `area_template`
--

DROP TABLE IF EXISTS `area_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `area_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `map_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `zone_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `explore_flag` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `flags` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `area_level` mediumint(9) NOT NULL DEFAULT 0,
  `name` varchar(100) NOT NULL DEFAULT '',
  `team` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `liquid_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areagroup`
--

DROP TABLE IF EXISTS `areagroup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areagroup` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaId1` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaId2` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaId3` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaId4` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaId5` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaId6` int(10) unsigned NOT NULL DEFAULT 0,
  `NextGroup` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areatable`
--

DROP TABLE IF EXISTS `areatable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areatable` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Map` int(10) unsigned NOT NULL DEFAULT 0,
  `zone` int(10) unsigned NOT NULL DEFAULT 0,
  `exploreFlag` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundPreferences` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundPreferencesUnderwater` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundAmbience` int(10) unsigned NOT NULL DEFAULT 0,
  `ZoneMusic` int(10) unsigned NOT NULL DEFAULT 0,
  `zoneIntroMusic` int(10) unsigned NOT NULL DEFAULT 0,
  `area_level` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlag` int(10) unsigned NOT NULL DEFAULT 0,
  `FactionGroup` int(10) unsigned NOT NULL DEFAULT 0,
  `LiquidType1` int(10) unsigned NOT NULL DEFAULT 0,
  `LiquidType2` int(10) unsigned NOT NULL DEFAULT 0,
  `LiquidType3` int(10) unsigned NOT NULL DEFAULT 0,
  `LiquidType4` int(10) unsigned NOT NULL DEFAULT 0,
  `MinElevation` float NOT NULL DEFAULT 0,
  `AmbientMultiplier` float NOT NULL DEFAULT 0,
  `Light` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areatrigger`
--

DROP TABLE IF EXISTS `areatrigger`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areatrigger` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `ContinentID` int(10) unsigned NOT NULL DEFAULT 0,
  `X` float NOT NULL DEFAULT 0,
  `Y` float NOT NULL DEFAULT 0,
  `Z` float NOT NULL DEFAULT 0,
  `Radius` float NOT NULL DEFAULT 0,
  `Box_Length` float NOT NULL DEFAULT 0,
  `Box_Width` float NOT NULL DEFAULT 0,
  `Box_Height` float NOT NULL DEFAULT 0,
  `Box_Yaw` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areatrigger_bg_entrance`
--

DROP TABLE IF EXISTS `areatrigger_bg_entrance`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areatrigger_bg_entrance` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `name` text DEFAULT NULL,
  `team` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `bg_template` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `exit_map` smallint(5) unsigned NOT NULL DEFAULT 0,
  `exit_position_x` float NOT NULL DEFAULT 0,
  `exit_position_y` float NOT NULL DEFAULT 0,
  `exit_position_z` float NOT NULL DEFAULT 0,
  `exit_orientation` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areatrigger_involvedrelation`
--

DROP TABLE IF EXISTS `areatrigger_involvedrelation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areatrigger_involvedrelation` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `quest` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Quest Identifier',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Trigger System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areatrigger_tavern`
--

DROP TABLE IF EXISTS `areatrigger_tavern`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areatrigger_tavern` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `name` text DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Trigger System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areatrigger_teleport`
--

DROP TABLE IF EXISTS `areatrigger_teleport`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areatrigger_teleport` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `name` varchar(64) NOT NULL DEFAULT '',
  `message` varchar(128) NOT NULL DEFAULT '',
  `required_level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `required_condition` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `required_phase` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `target_map` smallint(5) unsigned NOT NULL DEFAULT 0,
  `target_position_x` float NOT NULL DEFAULT 0,
  `target_position_y` float NOT NULL DEFAULT 0,
  `target_position_z` float NOT NULL DEFAULT 0,
  `target_orientation` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Trigger System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `areatrigger_template`
--

DROP TABLE IF EXISTS `areatrigger_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `areatrigger_template` (
  `id` smallint(5) unsigned NOT NULL,
  `map_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `radius` float NOT NULL DEFAULT 0,
  `box_x` float NOT NULL DEFAULT 0,
  `box_y` float NOT NULL DEFAULT 0,
  `box_z` float NOT NULL DEFAULT 0,
  `box_orientation` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `auctionhousebot`
--

DROP TABLE IF EXISTS `auctionhousebot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `auctionhousebot` (
  `item` int(10) unsigned NOT NULL COMMENT 'Item Id',
  `stack` tinyint(3) unsigned NOT NULL DEFAULT 1 COMMENT 'Stack Size',
  `bid` int(10) unsigned NOT NULL DEFAULT 1 COMMENT 'Bid Price',
  `buyout` int(10) unsigned NOT NULL DEFAULT 1 COMMENT 'Buyout Price'
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `autobroadcast`
--

DROP TABLE IF EXISTS `autobroadcast`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `autobroadcast` (
  `string_id` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `battleground_events`
--

DROP TABLE IF EXISTS `battleground_events`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `battleground_events` (
  `map` smallint(6) NOT NULL,
  `event1` tinyint(3) unsigned NOT NULL,
  `event2` tinyint(3) unsigned NOT NULL,
  `description` varchar(255) NOT NULL,
  PRIMARY KEY (`map`,`event1`,`event2`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `battleground_template`
--

DROP TABLE IF EXISTS `battleground_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `battleground_template` (
  `id` mediumint(8) unsigned NOT NULL,
  `min_players_per_team` smallint(5) unsigned NOT NULL DEFAULT 0,
  `max_players_per_team` smallint(5) unsigned NOT NULL DEFAULT 0,
  `min_level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `max_level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `alliance_win_spell` smallint(5) unsigned NOT NULL DEFAULT 0,
  `alliance_lose_spell` smallint(5) unsigned NOT NULL DEFAULT 0,
  `horde_win_spell` smallint(5) unsigned NOT NULL DEFAULT 0,
  `horde_lose_spell` smallint(5) unsigned NOT NULL DEFAULT 0,
  `alliance_start_location` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'WorldSafeLocs.dbc',
  `horde_start_location` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'WorldSafeLocs.dbc',
  `player_loot_id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'entry from reference_loot_template',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `battlemaster_entry`
--

DROP TABLE IF EXISTS `battlemaster_entry`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `battlemaster_entry` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Entry of a creature',
  `bg_template` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Battleground template id',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `broadcast_text`
--

DROP TABLE IF EXISTS `broadcast_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `broadcast_text` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `male_text` longtext DEFAULT NULL,
  `female_text` longtext DEFAULT NULL,
  `chat_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `sound_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `language_id` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `emote_id1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `emote_id2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `emote_id3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `emote_delay1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `emote_delay2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `emote_delay3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cartographer`
--

DROP TABLE IF EXISTS `cartographer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cartographer` (
  `area_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`area_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC COMMENT='list of all areas that must be explored to gain the cartographer title';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `charstartoutfit`
--

DROP TABLE IF EXISTS `charstartoutfit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `charstartoutfit` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `gender` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `outfitId` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `itemId1` int(11) NOT NULL DEFAULT 0,
  `itemId2` int(11) NOT NULL DEFAULT 0,
  `itemId3` int(11) NOT NULL DEFAULT 0,
  `itemId4` int(11) NOT NULL DEFAULT 0,
  `itemId5` int(11) NOT NULL DEFAULT 0,
  `itemId6` int(11) NOT NULL DEFAULT 0,
  `itemId7` int(11) NOT NULL DEFAULT 0,
  `itemId8` int(11) NOT NULL DEFAULT 0,
  `itemId9` int(11) NOT NULL DEFAULT 0,
  `itemId10` int(11) NOT NULL DEFAULT 0,
  `itemId11` int(11) NOT NULL DEFAULT 0,
  `itemId12` int(11) NOT NULL DEFAULT 0,
  `itemId13` int(11) NOT NULL DEFAULT 0,
  `itemId14` int(11) NOT NULL DEFAULT 0,
  `itemId15` int(11) NOT NULL DEFAULT 0,
  `itemId16` int(11) NOT NULL DEFAULT 0,
  `itemId17` int(11) NOT NULL DEFAULT 0,
  `itemId18` int(11) NOT NULL DEFAULT 0,
  `itemId19` int(11) NOT NULL DEFAULT 0,
  `itemId20` int(11) NOT NULL DEFAULT 0,
  `itemId21` int(11) NOT NULL DEFAULT 0,
  `itemId22` int(11) NOT NULL DEFAULT 0,
  `itemId23` int(11) NOT NULL DEFAULT 0,
  `itemId24` int(11) NOT NULL DEFAULT 0,
  `displayInfo1` int(11) NOT NULL DEFAULT 0,
  `displayInfo2` int(11) NOT NULL DEFAULT 0,
  `displayInfo3` int(11) NOT NULL DEFAULT 0,
  `displayInfo4` int(11) NOT NULL DEFAULT 0,
  `displayInfo5` int(11) NOT NULL DEFAULT 0,
  `displayInfo6` int(11) NOT NULL DEFAULT 0,
  `displayInfo7` int(11) NOT NULL DEFAULT 0,
  `displayInfo8` int(11) NOT NULL DEFAULT 0,
  `displayInfo9` int(11) NOT NULL DEFAULT 0,
  `displayInfo10` int(11) NOT NULL DEFAULT 0,
  `displayInfo11` int(11) NOT NULL DEFAULT 0,
  `displayInfo12` int(11) NOT NULL DEFAULT 0,
  `displayInfo13` int(11) NOT NULL DEFAULT 0,
  `displayInfo14` int(11) NOT NULL DEFAULT 0,
  `displayInfo15` int(11) NOT NULL DEFAULT 0,
  `displayInfo16` int(11) NOT NULL DEFAULT 0,
  `displayInfo17` int(11) NOT NULL DEFAULT 0,
  `displayInfo18` int(11) NOT NULL DEFAULT 0,
  `displayInfo19` int(11) NOT NULL DEFAULT 0,
  `displayInfo20` int(11) NOT NULL DEFAULT 0,
  `displayInfo21` int(11) NOT NULL DEFAULT 0,
  `displayInfo22` int(11) NOT NULL DEFAULT 0,
  `displayInfo23` int(11) NOT NULL DEFAULT 0,
  `displayInfo24` int(11) NOT NULL DEFAULT 0,
  `invType1` int(11) NOT NULL DEFAULT 0,
  `invType2` int(11) NOT NULL DEFAULT 0,
  `invType3` int(11) NOT NULL DEFAULT 0,
  `invType4` int(11) NOT NULL DEFAULT 0,
  `invType5` int(11) NOT NULL DEFAULT 0,
  `invType6` int(11) NOT NULL DEFAULT 0,
  `invType7` int(11) NOT NULL DEFAULT 0,
  `invType8` int(11) NOT NULL DEFAULT 0,
  `invType9` int(11) NOT NULL DEFAULT 0,
  `invType10` int(11) NOT NULL DEFAULT 0,
  `invType11` int(11) NOT NULL DEFAULT 0,
  `invType12` int(11) NOT NULL DEFAULT 0,
  `invType13` int(11) NOT NULL DEFAULT 0,
  `invType14` int(11) NOT NULL DEFAULT 0,
  `invType15` int(11) NOT NULL DEFAULT 0,
  `invType16` int(11) NOT NULL DEFAULT 0,
  `invType17` int(11) NOT NULL DEFAULT 0,
  `invType18` int(11) NOT NULL DEFAULT 0,
  `invType19` int(11) NOT NULL DEFAULT 0,
  `invType20` int(11) NOT NULL DEFAULT 0,
  `invType21` int(11) NOT NULL DEFAULT 0,
  `invType22` int(11) NOT NULL DEFAULT 0,
  `invType23` int(11) NOT NULL DEFAULT 0,
  `invType24` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chartitles`
--

DROP TABLE IF EXISTS `chartitles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `chartitles` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `unk1` int(11) NOT NULL DEFAULT 0,
  `maleTitle1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitle16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `maleTitleFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `femaleTitle1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitle16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `femaleTitleFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `titleMaskId` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chat_channels`
--

DROP TABLE IF EXISTS `chat_channels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `chat_channels` (
  `id` int(11) NOT NULL DEFAULT 0,
  `flags` int(11) NOT NULL DEFAULT 0,
  `faction_group` int(11) NOT NULL DEFAULT 0,
  `name` text DEFAULT NULL,
  `name_loc1` text DEFAULT NULL,
  `name_loc2` text DEFAULT NULL,
  `name_loc3` text DEFAULT NULL,
  `name_loc4` text DEFAULT NULL,
  `name_loc5` text DEFAULT NULL,
  `name_loc6` text DEFAULT NULL,
  `name_loc7` text DEFAULT NULL,
  `name_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `shortcut` text DEFAULT NULL,
  `shortcut_loc1` text DEFAULT NULL,
  `shortcut_loc2` text DEFAULT NULL,
  `shortcut_loc3` text DEFAULT NULL,
  `shortcut_loc4` text DEFAULT NULL,
  `shortcut_loc5` text DEFAULT NULL,
  `shortcut_loc6` text DEFAULT NULL,
  `shortcut_loc7` text DEFAULT NULL,
  `shortcut_flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chatchannels`
--

DROP TABLE IF EXISTS `chatchannels`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `chatchannels` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `factionGroup` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlag` int(10) unsigned NOT NULL DEFAULT 0,
  `ShortName1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortNameFlag` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chrclasses`
--

DROP TABLE IF EXISTS `chrclasses`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `chrclasses` (
  `Id` int(10) unsigned NOT NULL DEFAULT 0,
  `Field01` int(10) unsigned NOT NULL DEFAULT 0,
  `DisplayPower` int(10) unsigned NOT NULL DEFAULT 0,
  `PetNameToken` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `NameFemale1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemaleFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `NameMale1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMaleFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `FileName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellClassSet` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `CinematicSequenceId` int(10) unsigned NOT NULL DEFAULT 0,
  `RequiredExpansion` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `chrraces`
--

DROP TABLE IF EXISTS `chrraces`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `chrraces` (
  `Id` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `FactionId` int(10) unsigned NOT NULL DEFAULT 0,
  `ExploreationSoundId` int(10) unsigned NOT NULL DEFAULT 0,
  `MaleDisplayId` int(10) unsigned NOT NULL DEFAULT 0,
  `FemaleDisplayId` int(10) unsigned NOT NULL DEFAULT 0,
  `ClientPrefix` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `BaseLanguage` int(10) unsigned NOT NULL DEFAULT 0,
  `CreatureType` int(10) unsigned NOT NULL DEFAULT 0,
  `ResSicknessSpellId` int(10) unsigned NOT NULL DEFAULT 0,
  `SpalshSoundId` int(10) unsigned NOT NULL DEFAULT 0,
  `ClientFileString` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `CinematicSequenceId` int(10) unsigned NOT NULL DEFAULT 0,
  `Alliance` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `NameFemale1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemale16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFemaleFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `NameMale1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMale16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameMaleFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `FacialHairCustom1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `FacialHairCustom2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `HairCustomisation` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `RequiredExpansion` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cinematic_waypoints`
--

DROP TABLE IF EXISTS `cinematic_waypoints`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cinematic_waypoints` (
  `cinematic` int(11) DEFAULT 0,
  `timer` int(11) DEFAULT 0,
  `position_x` float DEFAULT NULL,
  `position_y` float DEFAULT NULL,
  `position_z` float DEFAULT NULL,
  `comment` varchar(255) DEFAULT NULL,
  `id` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=129 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cinematiccamera`
--

DROP TABLE IF EXISTS `cinematiccamera`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cinematiccamera` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `FilePath` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Voiceover` int(10) unsigned NOT NULL DEFAULT 0,
  `X` float NOT NULL DEFAULT 0,
  `Y` float NOT NULL DEFAULT 0,
  `Z` float NOT NULL DEFAULT 0,
  `O` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cinematicsequences`
--

DROP TABLE IF EXISTS `cinematicsequences`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cinematicsequences` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera1` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera2` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera3` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera4` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera5` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera6` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera7` int(10) unsigned NOT NULL DEFAULT 0,
  `Camera8` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `collection_mount`
--

DROP TABLE IF EXISTS `collection_mount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `collection_mount` (
  `itemId` int(10) unsigned NOT NULL,
  `spellId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`itemId`,`spellId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `collection_pet`
--

DROP TABLE IF EXISTS `collection_pet`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `collection_pet` (
  `itemId` int(10) unsigned NOT NULL,
  `spellId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`itemId`,`spellId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `collection_toy`
--

DROP TABLE IF EXISTS `collection_toy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `collection_toy` (
  `itemId` int(10) unsigned NOT NULL,
  `spellId` int(10) unsigned NOT NULL,
  PRIMARY KEY (`itemId`,`spellId`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `conditions`
--

DROP TABLE IF EXISTS `conditions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `conditions` (
  `condition_entry` mediumint(8) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `type` tinyint(4) NOT NULL DEFAULT 0 COMMENT 'Type of the condition',
  `value1` int(11) NOT NULL DEFAULT 0 COMMENT 'data field one for the condition',
  `value2` int(11) NOT NULL DEFAULT 0 COMMENT 'data field two for the condition',
  `value3` int(11) NOT NULL DEFAULT 0 COMMENT 'data field three for the condition',
  `value4` int(11) NOT NULL DEFAULT 0 COMMENT 'data field four for the condition',
  `flags` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT 'general condition flags',
  PRIMARY KEY (`condition_entry`) USING BTREE,
  UNIQUE KEY `unique_conditions` (`type`,`value1`,`value2`,`flags`,`value3`,`value4`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=1678803 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Condition System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature`
--

DROP TABLE IF EXISTS `creature`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Global Unique Identifier',
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Creature Template Id',
  `id2` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Creature Template Id',
  `id3` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Creature Template Id',
  `id4` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Creature Template Id',
  `map` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'Map Identifier',
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  `spawntimesecsmin` int(10) unsigned NOT NULL DEFAULT 120,
  `spawntimesecsmax` int(10) unsigned NOT NULL DEFAULT 120,
  `wander_distance` float NOT NULL DEFAULT 5,
  `health_percent` float NOT NULL DEFAULT 100,
  `mana_percent` float unsigned NOT NULL DEFAULT 100,
  `movement_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spawn_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `visibility_mod` float DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `idx_map` (`map`) USING BTREE,
  KEY `idx_id` (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=2583681 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Creature System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_addon`
--

DROP TABLE IF EXISTS `creature_addon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_addon` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `display_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `mount_display_id` smallint(6) NOT NULL DEFAULT -1,
  `equipment_id` int(11) NOT NULL DEFAULT -1,
  `stand_state` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `sheath_state` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `emote_state` smallint(5) unsigned NOT NULL DEFAULT 0,
  `auras` text DEFAULT NULL,
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC COMMENT='Extra data for creature spawn.';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_ai_events`
--

DROP TABLE IF EXISTS `creature_ai_events`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_ai_events` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Identifier',
  `creature_id` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Creature Template Identifier',
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Condition id from conditions table',
  `event_type` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT 'Event Type',
  `event_inverse_phase_mask` int(11) NOT NULL DEFAULT 0 COMMENT 'Mask which phases this event will not trigger in',
  `event_chance` int(10) unsigned NOT NULL DEFAULT 100,
  `event_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `event_param1` int(11) NOT NULL DEFAULT 0,
  `event_param2` int(11) NOT NULL DEFAULT 0,
  `event_param3` int(11) NOT NULL DEFAULT 0,
  `event_param4` int(11) NOT NULL DEFAULT 0,
  `action1_script` int(10) unsigned NOT NULL DEFAULT 0,
  `action2_script` int(10) unsigned NOT NULL DEFAULT 0,
  `action3_script` int(10) unsigned NOT NULL DEFAULT 0,
  `comment` varchar(255) NOT NULL DEFAULT '' COMMENT 'Event Comment',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=98800624 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='EventAI Scripts';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_ai_scripts`
--

DROP TABLE IF EXISTS `creature_ai_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_ai_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_battleground`
--

DROP TABLE IF EXISTS `creature_battleground`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_battleground` (
  `guid` int(10) unsigned NOT NULL COMMENT 'Creature''s GUID',
  `event1` tinyint(3) unsigned NOT NULL COMMENT 'main event',
  `event2` tinyint(3) unsigned NOT NULL COMMENT 'sub event',
  PRIMARY KEY (`guid`,`event1`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Creature battleground indexing system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_display_info_addon`
--

DROP TABLE IF EXISTS `creature_display_info_addon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_display_info_addon` (
  `display_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `bounding_radius` float NOT NULL DEFAULT 0,
  `combat_reach` float NOT NULL DEFAULT 0,
  `gender` tinyint(3) unsigned NOT NULL DEFAULT 2,
  `display_id_other_gender` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`display_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Creature System (display id related info)';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_equip_template`
--

DROP TABLE IF EXISTS `creature_equip_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_equip_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Unique entry',
  `equipentry1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `equipentry2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `equipentry3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Creature System (Equipment)';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_groups`
--

DROP TABLE IF EXISTS `creature_groups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_groups` (
  `leader_guid` int(10) unsigned NOT NULL,
  `member_guid` int(10) unsigned NOT NULL,
  `dist` float unsigned NOT NULL,
  `angle` float unsigned NOT NULL,
  `flags` int(10) unsigned NOT NULL,
  PRIMARY KEY (`member_guid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_involvedrelation`
--

DROP TABLE IF EXISTS `creature_involvedrelation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_involvedrelation` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `quest` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Quest Identifier',
  PRIMARY KEY (`id`,`quest`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Creature System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_linking`
--

DROP TABLE IF EXISTS `creature_linking`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_linking` (
  `guid` int(10) unsigned NOT NULL COMMENT 'creature.guid of the slave mob that is linked',
  `master_guid` int(10) unsigned NOT NULL COMMENT 'master to trigger events',
  `flag` mediumint(8) unsigned NOT NULL COMMENT 'flag - describing what should happen when',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Creature Linking System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_linking_template`
--

DROP TABLE IF EXISTS `creature_linking_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_linking_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'creature_template.entry of the slave mob that is linked',
  `map` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'Id of map of the mobs',
  `master_entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'master to trigger events',
  `flag` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'flag - describing what should happen when',
  `search_range` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'search_range - describing in which range (spawn-coords) master and slave are linked together',
  PRIMARY KEY (`entry`,`map`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Creature Linking System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_loot_template`
--

DROP TABLE IF EXISTS `creature_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'entry 0 used for player insignia loot',
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`,`groupid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_movement`
--

DROP TABLE IF EXISTS `creature_movement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_movement` (
  `id` int(10) unsigned NOT NULL COMMENT 'Creature GUID',
  `point` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  `waittime` int(10) unsigned NOT NULL DEFAULT 0,
  `wander_distance` float unsigned NOT NULL DEFAULT 0,
  `script_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`,`point`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Creature System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_movement_scripts`
--

DROP TABLE IF EXISTS `creature_movement_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_movement_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_movement_special`
--

DROP TABLE IF EXISTS `creature_movement_special`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_movement_special` (
  `id` int(10) unsigned NOT NULL,
  `point` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  `waittime` int(10) unsigned NOT NULL DEFAULT 0,
  `wander_distance` float unsigned NOT NULL DEFAULT 0,
  `script_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`,`point`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Creature System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_movement_template`
--

DROP TABLE IF EXISTS `creature_movement_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_movement_template` (
  `entry` mediumint(8) unsigned NOT NULL COMMENT 'Creature entry',
  `point` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  `waittime` int(10) unsigned NOT NULL DEFAULT 0,
  `wander_distance` float unsigned NOT NULL DEFAULT 0,
  `script_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`point`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Creature waypoint system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_onkill_reputation`
--

DROP TABLE IF EXISTS `creature_onkill_reputation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_onkill_reputation` (
  `creature_id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Creature Identifier',
  `RewOnKillRepFaction1` smallint(6) NOT NULL DEFAULT 0,
  `RewOnKillRepFaction2` smallint(6) NOT NULL DEFAULT 0,
  `MaxStanding1` tinyint(4) NOT NULL DEFAULT 0,
  `IsTeamAward1` tinyint(4) NOT NULL DEFAULT 0,
  `RewOnKillRepValue1` mediumint(9) NOT NULL DEFAULT 0,
  `MaxStanding2` tinyint(4) NOT NULL DEFAULT 0,
  `IsTeamAward2` tinyint(4) NOT NULL DEFAULT 0,
  `RewOnKillRepValue2` mediumint(9) NOT NULL DEFAULT 0,
  `TeamDependent` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`creature_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Creature OnKill Reputation gain';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_questrelation`
--

DROP TABLE IF EXISTS `creature_questrelation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_questrelation` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `quest` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Quest Identifier',
  PRIMARY KEY (`id`,`quest`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Creature System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_spells`
--

DROP TABLE IF EXISTS `creature_spells`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_spells` (
  `entry` int(10) unsigned NOT NULL DEFAULT 0,
  `name` varchar(255) NOT NULL DEFAULT '',
  `spellId_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_1` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_1` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellId_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_2` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_2` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellId_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_3` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_3` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellId_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_4` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_4` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellId_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_5` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_5` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_5` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellId_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_6` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_6` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_6` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellId_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_7` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_7` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_7` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_7` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellId_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `probability_8` tinyint(3) unsigned NOT NULL DEFAULT 100,
  `castTarget_8` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `targetParam1_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `targetParam2_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `castFlags_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMin_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayInitialMax_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMin_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `delayRepeatMax_8` smallint(5) unsigned NOT NULL DEFAULT 0,
  `scriptId_8` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_spells_scripts`
--

DROP TABLE IF EXISTS `creature_spells_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_spells_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creature_template`
--

DROP TABLE IF EXISTS `creature_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creature_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `display_id1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `display_id2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `display_id3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `display_id4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `mount_display_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `name` char(100) NOT NULL DEFAULT '0',
  `subname` char(100) DEFAULT NULL,
  `gossip_menu_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `level_min` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `level_max` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `health_min` int(10) unsigned NOT NULL DEFAULT 0,
  `health_max` int(10) unsigned NOT NULL DEFAULT 0,
  `mana_min` int(10) unsigned NOT NULL DEFAULT 0,
  `mana_max` int(10) unsigned NOT NULL DEFAULT 0,
  `armor` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `faction` smallint(5) unsigned NOT NULL DEFAULT 0,
  `npc_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `speed_walk` float NOT NULL DEFAULT 1 COMMENT 'Result of 2.5/2.5, most common value',
  `speed_run` float NOT NULL DEFAULT 1.14286 COMMENT 'Result of 8.0/7.0, most common value',
  `scale` float NOT NULL DEFAULT 1,
  `detection_range` float NOT NULL DEFAULT 20,
  `call_for_help_range` float NOT NULL DEFAULT 5,
  `leash_range` float NOT NULL DEFAULT 0,
  `rank` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `xp_multiplier` float NOT NULL DEFAULT 1,
  `dmg_min` float NOT NULL DEFAULT 0,
  `dmg_max` float NOT NULL DEFAULT 0,
  `dmg_school` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `attack_power` int(10) unsigned NOT NULL DEFAULT 0,
  `dmg_multiplier` float NOT NULL DEFAULT 1,
  `base_attack_time` int(10) unsigned NOT NULL DEFAULT 0,
  `ranged_attack_time` int(10) unsigned NOT NULL DEFAULT 0,
  `unit_class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `unit_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `dynamic_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `beast_family` tinyint(4) NOT NULL DEFAULT 0,
  `trainer_type` tinyint(4) NOT NULL DEFAULT 0,
  `trainer_spell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `trainer_class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `trainer_race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `ranged_dmg_min` float NOT NULL DEFAULT 0,
  `ranged_dmg_max` float NOT NULL DEFAULT 0,
  `ranged_attack_power` smallint(5) unsigned NOT NULL DEFAULT 0,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `type_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `loot_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `pickpocket_loot_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `skinning_loot_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `holy_res` smallint(6) NOT NULL DEFAULT 0,
  `fire_res` smallint(6) NOT NULL DEFAULT 0,
  `nature_res` smallint(6) NOT NULL DEFAULT 0,
  `frost_res` smallint(6) NOT NULL DEFAULT 0,
  `shadow_res` smallint(6) NOT NULL DEFAULT 0,
  `arcane_res` smallint(6) NOT NULL DEFAULT 0,
  `spell_id1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell_id2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell_id3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell_id4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell_list_id` int(10) unsigned NOT NULL DEFAULT 0,
  `pet_spell_list_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spawn_spell_id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Spell with effect 46 that will be cast immediately upon spawning. Creature will remain unattackable until the cast finishes.',
  `auras` text DEFAULT NULL,
  `gold_min` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `gold_max` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ai_name` char(64) NOT NULL DEFAULT '',
  `movement_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `inhabit_type` tinyint(3) unsigned NOT NULL DEFAULT 3,
  `civilian` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `racial_leader` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `regeneration` tinyint(3) unsigned NOT NULL DEFAULT 3,
  `equipment_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `trainer_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `vendor_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `mechanic_immune_mask` int(10) unsigned NOT NULL DEFAULT 0,
  `school_immune_mask` int(10) unsigned NOT NULL DEFAULT 0,
  `immunity_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `flags_extra` int(10) unsigned NOT NULL DEFAULT 0,
  `phase_quest_id` int(10) unsigned NOT NULL DEFAULT 0,
  `script_name` char(64) NOT NULL DEFAULT '',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Creature System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creaturedisplayinfo`
--

DROP TABLE IF EXISTS `creaturedisplayinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creaturedisplayinfo` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `ModelID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `ExtendedDisplayInfoID` int(10) unsigned NOT NULL DEFAULT 0,
  `CreatureModelScale` float NOT NULL DEFAULT 0,
  `CreatureModelAlpha` int(10) unsigned NOT NULL DEFAULT 0,
  `TextureVariation_1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `TextureVariation_2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `TextureVariation_3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `PortraitTextureName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `BloodLevel` int(11) NOT NULL DEFAULT 0,
  `BloodID` int(10) unsigned NOT NULL DEFAULT 0,
  `NPCSoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `ParticleColorID` int(10) unsigned NOT NULL DEFAULT 0,
  `CreatureGeosetData` int(10) unsigned NOT NULL DEFAULT 0,
  `ObjectEffectPackageID` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creaturedisplayinfoextra`
--

DROP TABLE IF EXISTS `creaturedisplayinfoextra`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creaturedisplayinfoextra` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `DisplayRaceID` int(10) unsigned NOT NULL DEFAULT 0,
  `DisplaySexID` int(10) unsigned NOT NULL DEFAULT 0,
  `SkinID` int(10) unsigned NOT NULL DEFAULT 0,
  `FaceID` int(10) unsigned NOT NULL DEFAULT 0,
  `HairStyleID` int(10) unsigned NOT NULL DEFAULT 0,
  `HairColorID` int(10) unsigned NOT NULL DEFAULT 0,
  `FacialHairID` int(10) unsigned NOT NULL DEFAULT 0,
  `HeadDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `ShouldersDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `ShirtDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `ChestDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `BeltDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `LegsDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `BootsDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `BracersDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `GlovesDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `TabardDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `CapeDisplayID` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `BakedTextureIDblp` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creaturemodeldata`
--

DROP TABLE IF EXISTS `creaturemodeldata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creaturemodeldata` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `modelPath` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sizeClass` int(10) unsigned NOT NULL DEFAULT 0,
  `modelScale` float NOT NULL DEFAULT 0,
  `bloodId` int(10) unsigned NOT NULL DEFAULT 0,
  `footprintTextureId` int(10) unsigned NOT NULL DEFAULT 0,
  `footprintTextureLength` float NOT NULL DEFAULT 0,
  `footprintTextureWidth` float NOT NULL DEFAULT 0,
  `footprintParticleScale` float NOT NULL DEFAULT 0,
  `foleyMaterialId` int(10) unsigned NOT NULL DEFAULT 0,
  `footstepShakeSize` int(10) unsigned NOT NULL DEFAULT 0,
  `deathThudShakeSize` int(10) unsigned NOT NULL DEFAULT 0,
  `soundData` int(10) unsigned NOT NULL DEFAULT 0,
  `collisionWidth` float NOT NULL DEFAULT 0,
  `collisionHeight` float NOT NULL DEFAULT 0,
  `mountHeight` float NOT NULL DEFAULT 0,
  `geoBoxMinX` float NOT NULL DEFAULT 0,
  `geoBoxMinY` float NOT NULL DEFAULT 0,
  `geoBoxMinZ` float NOT NULL DEFAULT 0,
  `geoBoxMaxX` float NOT NULL DEFAULT 0,
  `geoBoxMaxY` float NOT NULL DEFAULT 0,
  `geoBoxMaxZ` float NOT NULL DEFAULT 0,
  `worldEffectScale` float NOT NULL DEFAULT 0,
  `attachedEffectScale` float NOT NULL DEFAULT 0,
  `MissileCollisionRadius` float NOT NULL DEFAULT 0,
  `MissileCollisionPush` float NOT NULL DEFAULT 0,
  `MissileCollisionRaise` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `creaturesounddata`
--

DROP TABLE IF EXISTS `creaturesounddata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `creaturesounddata` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundExertionID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundExertionCriticalID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundInjuryID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundInjuryCriticalID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundInjuryCrushingBlowID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundDeathID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundStunID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundStandID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundFootstepID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundAggroID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundWingFlapID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundWingGlideID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundAlertID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundFidget1` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundFidget2` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundFidget3` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundFidget4` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundFidget5` int(10) unsigned NOT NULL DEFAULT 0,
  `CustomAttack1` int(10) unsigned NOT NULL DEFAULT 0,
  `CustomAttack2` int(10) unsigned NOT NULL DEFAULT 0,
  `CustomAttack3` int(10) unsigned NOT NULL DEFAULT 0,
  `CustomAttack4` int(10) unsigned NOT NULL DEFAULT 0,
  `NPCSoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `LoopSoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `CreatureImpactType` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundJumpStartID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundJumpEndID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundPetAttackID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundPetOrderID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundPetDismissID` int(10) unsigned NOT NULL DEFAULT 0,
  `FidgetDelaySecondsMin` int(10) unsigned NOT NULL DEFAULT 0,
  `FidgetDelaySecondsMax` int(10) unsigned NOT NULL DEFAULT 0,
  `BirthSoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellCastDirectedSoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `SubmergeSoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `SubmergedSoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `CreatureSoundDataIDPet` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `currencycategory`
--

DROP TABLE IF EXISTS `currencycategory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `currencycategory` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `currencytypes`
--

DROP TABLE IF EXISTS `currencytypes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `currencytypes` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `item` int(10) unsigned NOT NULL DEFAULT 0,
  `category` int(10) unsigned NOT NULL DEFAULT 0,
  `bitIndex` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `custom_character_skins`
--

DROP TABLE IF EXISTS `custom_character_skins`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `custom_character_skins` (
  `token_id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'References item_template entry',
  `skin_male` smallint(5) unsigned NOT NULL DEFAULT 0,
  `skin_female` smallint(5) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`token_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Custom character skins';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `custom_graveyards`
--

DROP TABLE IF EXISTS `custom_graveyards`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `custom_graveyards` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `name` varchar(64) NOT NULL DEFAULT '',
  `map_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `zone_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `area_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `max_level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `map_gy_alliance` smallint(5) unsigned NOT NULL DEFAULT 0,
  `gy_x_alliance` float NOT NULL DEFAULT 0,
  `gy_y_alliance` float NOT NULL DEFAULT 0,
  `gy_z_alliance` float NOT NULL DEFAULT 0,
  `orientation_alliance` float NOT NULL DEFAULT 0,
  `map_gy_horde` smallint(5) unsigned NOT NULL DEFAULT 0,
  `gy_x_horde` float NOT NULL DEFAULT 0,
  `gy_y_horde` float NOT NULL DEFAULT 0,
  `gy_z_horde` float NOT NULL DEFAULT 0,
  `orientation_horde` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Custom graveyards';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `custom_merchant`
--

DROP TABLE IF EXISTS `custom_merchant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `custom_merchant` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `slot` smallint(5) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `count` int(10) unsigned NOT NULL DEFAULT 1,
  `extendedcost` int(10) unsigned NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE,
  KEY `entry_slot` (`entry`,`slot`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Npc System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `daily_quest_timer`
--

DROP TABLE IF EXISTS `daily_quest_timer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `daily_quest_timer` (
  `nextResetTime` bigint(20) unsigned NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `disenchant_loot_template`
--

DROP TABLE IF EXISTS `disenchant_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `disenchant_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Recommended id selection: item_level*100 + item_quality',
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dungeonencounter`
--

DROP TABLE IF EXISTS `dungeonencounter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `dungeonencounter` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `mapId` int(10) unsigned NOT NULL DEFAULT 0,
  `difficulty` int(10) unsigned NOT NULL DEFAULT 0,
  `orderIndex` int(11) NOT NULL DEFAULT 0,
  `bit` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `iconId` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dynamic_visibility_template`
--

DROP TABLE IF EXISTS `dynamic_visibility_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `dynamic_visibility_template` (
  `area_id` int(10) unsigned NOT NULL,
  `min_vis_distance` int(10) unsigned NOT NULL,
  `max_vis_distance` int(10) unsigned NOT NULL,
  `decrease_tick_diff` int(10) unsigned NOT NULL,
  `increase_tick_diff` int(10) unsigned NOT NULL,
  PRIMARY KEY (`area_id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_scripts`
--

DROP TABLE IF EXISTS `event_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `event_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `exploration_basexp`
--

DROP TABLE IF EXISTS `exploration_basexp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `exploration_basexp` (
  `level` tinyint(4) NOT NULL DEFAULT 0,
  `basexp` mediumint(9) NOT NULL DEFAULT 0,
  PRIMARY KEY (`level`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Exploration System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `faction`
--

DROP TABLE IF EXISTS `faction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `faction` (
  `id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `reputation_list_id` mediumint(9) NOT NULL DEFAULT 0,
  `base_rep_race_mask1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_race_mask2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_race_mask3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_race_mask4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_class_mask1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_class_mask2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_class_mask3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_class_mask4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `base_rep_value1` mediumint(9) NOT NULL DEFAULT 0,
  `base_rep_value2` mediumint(9) NOT NULL DEFAULT 0,
  `base_rep_value3` mediumint(9) NOT NULL DEFAULT 0,
  `base_rep_value4` mediumint(9) NOT NULL DEFAULT 0,
  `reputation_flags1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `reputation_flags2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `reputation_flags3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `reputation_flags4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `team` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `name1` varchar(256) NOT NULL DEFAULT '',
  `name2` varchar(256) NOT NULL DEFAULT '',
  `name3` varchar(256) NOT NULL DEFAULT '',
  `name4` varchar(256) NOT NULL DEFAULT '',
  `name5` varchar(256) NOT NULL DEFAULT '',
  `name6` varchar(256) NOT NULL DEFAULT '',
  `name7` varchar(256) NOT NULL DEFAULT '',
  `name8` varchar(256) NOT NULL DEFAULT '',
  `description1` varchar(512) NOT NULL DEFAULT '',
  `description2` varchar(512) NOT NULL DEFAULT '',
  `description3` varchar(512) NOT NULL DEFAULT '',
  `description4` varchar(512) NOT NULL DEFAULT '',
  `description5` varchar(512) NOT NULL DEFAULT '',
  `description6` varchar(512) NOT NULL DEFAULT '',
  `description7` varchar(512) NOT NULL DEFAULT '',
  `description8` varchar(512) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `faction_template`
--

DROP TABLE IF EXISTS `faction_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `faction_template` (
  `id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `faction_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `faction_flags` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `our_mask` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `friendly_mask` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `hostile_mask` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `enemy_faction1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `enemy_faction2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `enemy_faction3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `enemy_faction4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `friend_faction1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `friend_faction2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `friend_faction3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `friend_faction4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `factiongroup`
--

DROP TABLE IF EXISTS `factiongroup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `factiongroup` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `MaskID` int(11) NOT NULL DEFAULT 0,
  `internalName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `factiontemplate`
--

DROP TABLE IF EXISTS `factiontemplate`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `factiontemplate` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Faction` int(11) NOT NULL DEFAULT 0,
  `Flags` int(11) NOT NULL DEFAULT 0,
  `FactionGroup` int(11) NOT NULL DEFAULT 0,
  `FriendGroup` int(11) NOT NULL DEFAULT 0,
  `EnemyGroup` int(11) NOT NULL DEFAULT 0,
  `Enemies1` int(11) NOT NULL DEFAULT 0,
  `Enemies2` int(11) NOT NULL DEFAULT 0,
  `Enemies3` int(11) NOT NULL DEFAULT 0,
  `Enemies4` int(11) NOT NULL DEFAULT 0,
  `Friends1` int(11) NOT NULL DEFAULT 0,
  `Friends2` int(11) NOT NULL DEFAULT 0,
  `Friends3` int(11) NOT NULL DEFAULT 0,
  `Friends4` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `filedata`
--

DROP TABLE IF EXISTS `filedata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `filedata` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `FileName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `FilePath` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `fishing_loot_template`
--

DROP TABLE IF EXISTS `fishing_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `fishing_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'entry 0 used for junk loot at fishing fail (if allowed by config option)',
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_event`
--

DROP TABLE IF EXISTS `game_event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event` (
  `entry` mediumint(8) unsigned NOT NULL COMMENT 'Entry of the game event',
  `start_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT 'Absolute start date, the event will never start before',
  `end_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT 'Absolute end date, the event will never start afler',
  `occurence` bigint(20) unsigned NOT NULL DEFAULT 5184000 COMMENT 'Delay in minutes between occurences of the event',
  `length` bigint(20) unsigned NOT NULL DEFAULT 2592000 COMMENT 'Length in minutes of the event',
  `holiday` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Client side holiday id',
  `description` varchar(255) DEFAULT NULL COMMENT 'Description of the event displayed in console',
  `hardcoded` tinyint(4) NOT NULL DEFAULT 0,
  `disabled` tinyint(4) NOT NULL DEFAULT 0,
  `required_phase` tinyint(4) NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_event_creature`
--

DROP TABLE IF EXISTS `game_event_creature`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event_creature` (
  `guid` int(10) unsigned NOT NULL,
  `event` smallint(6) NOT NULL DEFAULT 0 COMMENT 'Put negatives values to remove during event',
  PRIMARY KEY (`guid`,`event`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_event_creature_data`
--

DROP TABLE IF EXISTS `game_event_creature_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event_creature_data` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `entry_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `display_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `equipment_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell_start` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell_end` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `event` smallint(5) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`guid`,`event`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_event_gameobject`
--

DROP TABLE IF EXISTS `game_event_gameobject`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event_gameobject` (
  `guid` int(10) unsigned NOT NULL,
  `event` smallint(6) NOT NULL DEFAULT 0 COMMENT 'Put negatives values to remove during event',
  PRIMARY KEY (`guid`,`event`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_event_mail`
--

DROP TABLE IF EXISTS `game_event_mail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event_mail` (
  `event` smallint(6) NOT NULL DEFAULT 0 COMMENT 'Negatives value to send at event stop, positive value for send at event start.',
  `raceMask` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `quest` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `mailTemplateId` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `senderEntry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`event`,`raceMask`,`quest`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Game event system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_event_quest`
--

DROP TABLE IF EXISTS `game_event_quest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_event_quest` (
  `quest` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'entry from quest_template',
  `event` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'entry from game_event',
  PRIMARY KEY (`quest`,`event`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Game event system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_graveyard_zone`
--

DROP TABLE IF EXISTS `game_graveyard_zone`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_graveyard_zone` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ghost_zone` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `faction` smallint(5) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`,`ghost_zone`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Trigger System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_tele`
--

DROP TABLE IF EXISTS `game_tele`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_tele` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  `map` smallint(5) unsigned NOT NULL DEFAULT 0,
  `name` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=941 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Tele Command';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `game_weather`
--

DROP TABLE IF EXISTS `game_weather`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `game_weather` (
  `zone` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spring_rain_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `spring_snow_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `spring_storm_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `summer_rain_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `summer_snow_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `summer_storm_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `fall_rain_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `fall_snow_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `fall_storm_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `winter_rain_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `winter_snow_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `winter_storm_chance` tinyint(3) unsigned NOT NULL DEFAULT 25,
  `comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`zone`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Weather System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject`
--

DROP TABLE IF EXISTS `gameobject`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Global Unique Identifier',
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Gameobject Identifier',
  `map` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'Map Identifier',
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  `rotation0` float NOT NULL DEFAULT 0,
  `rotation1` float NOT NULL DEFAULT 0,
  `rotation2` float NOT NULL DEFAULT 0,
  `rotation3` float NOT NULL DEFAULT 0,
  `spawntimesecsmin` int(11) NOT NULL DEFAULT 0,
  `spawntimesecsmax` int(11) NOT NULL DEFAULT 0,
  `animprogress` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `state` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spawn_flags` int(10) unsigned NOT NULL DEFAULT 0,
  `visibility_mod` float DEFAULT 0,
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `idx_map` (`map`) USING BTREE,
  KEY `idx_id` (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=5019644 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Gameobject System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_battleground`
--

DROP TABLE IF EXISTS `gameobject_battleground`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_battleground` (
  `guid` int(10) unsigned NOT NULL COMMENT 'GameObject''s GUID',
  `event1` tinyint(3) unsigned NOT NULL COMMENT 'main event',
  `event2` tinyint(3) unsigned NOT NULL COMMENT 'sub event',
  PRIMARY KEY (`guid`,`event1`,`event2`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='GameObject battleground indexing system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_display_info_addon`
--

DROP TABLE IF EXISTS `gameobject_display_info_addon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_display_info_addon` (
  `display_id` int(11) NOT NULL DEFAULT 0,
  `min_x` float NOT NULL DEFAULT 0,
  `min_y` float NOT NULL DEFAULT 0,
  `min_z` float NOT NULL DEFAULT 0,
  `max_x` float NOT NULL DEFAULT 0,
  `max_y` float NOT NULL DEFAULT 0,
  `max_z` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`display_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_involvedrelation`
--

DROP TABLE IF EXISTS `gameobject_involvedrelation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_involvedrelation` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `quest` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Quest Identifier',
  PRIMARY KEY (`id`,`quest`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_loot_template`
--

DROP TABLE IF EXISTS `gameobject_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_questrelation`
--

DROP TABLE IF EXISTS `gameobject_questrelation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_questrelation` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `quest` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Quest Identifier',
  PRIMARY KEY (`id`,`quest`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_requirement`
--

DROP TABLE IF EXISTS `gameobject_requirement`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_requirement` (
  `guid` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Global Unique Identifier',
  `reqType` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Gameobject Identifier',
  `reqGuid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Gameobject Identifier',
  PRIMARY KEY (`guid`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=397162 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Gameobject System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_scripts`
--

DROP TABLE IF EXISTS `gameobject_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobject_template`
--

DROP TABLE IF EXISTS `gameobject_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobject_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `displayId` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `name` varchar(100) NOT NULL DEFAULT '',
  `faction` smallint(5) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `size` float NOT NULL DEFAULT 1,
  `data0` int(10) unsigned NOT NULL DEFAULT 0,
  `data1` int(11) NOT NULL DEFAULT 0,
  `data2` int(10) unsigned NOT NULL DEFAULT 0,
  `data3` int(10) unsigned NOT NULL DEFAULT 0,
  `data4` int(10) unsigned NOT NULL DEFAULT 0,
  `data5` int(10) unsigned NOT NULL DEFAULT 0,
  `data6` int(11) NOT NULL DEFAULT 0,
  `data7` int(10) unsigned NOT NULL DEFAULT 0,
  `data8` int(10) unsigned NOT NULL DEFAULT 0,
  `data9` int(10) unsigned NOT NULL DEFAULT 0,
  `data10` int(10) unsigned NOT NULL DEFAULT 0,
  `data11` int(10) unsigned NOT NULL DEFAULT 0,
  `data12` int(10) unsigned NOT NULL DEFAULT 0,
  `data13` int(10) unsigned NOT NULL DEFAULT 0,
  `data14` int(10) unsigned NOT NULL DEFAULT 0,
  `data15` int(10) unsigned NOT NULL DEFAULT 0,
  `data16` int(10) unsigned NOT NULL DEFAULT 0,
  `data17` int(10) unsigned NOT NULL DEFAULT 0,
  `data18` int(10) unsigned NOT NULL DEFAULT 0,
  `data19` int(10) unsigned NOT NULL DEFAULT 0,
  `data20` int(10) unsigned NOT NULL DEFAULT 0,
  `data21` int(10) unsigned NOT NULL DEFAULT 0,
  `data22` int(10) unsigned NOT NULL DEFAULT 0,
  `data23` int(10) unsigned NOT NULL DEFAULT 0,
  `mingold` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `maxgold` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `phase_quest_id` int(10) unsigned NOT NULL DEFAULT 0,
  `script_name` varchar(64) NOT NULL DEFAULT '',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Gameobject System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameobjectdisplayinfo`
--

DROP TABLE IF EXISTS `gameobjectdisplayinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gameobjectdisplayinfo` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `ModelName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Sound1` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound2` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound3` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound4` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound5` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound6` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound7` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound8` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound9` int(10) unsigned NOT NULL DEFAULT 0,
  `Sound10` int(10) unsigned NOT NULL DEFAULT 0,
  `GeoBoxMinX` float NOT NULL DEFAULT 0,
  `GeoBoxMinY` float NOT NULL DEFAULT 0,
  `GeoBoxMinZ` float NOT NULL DEFAULT 0,
  `GeoBoxMaxX` float NOT NULL DEFAULT 0,
  `GeoBoxMaxY` float NOT NULL DEFAULT 0,
  `GeoBoxMaxZ` float NOT NULL DEFAULT 0,
  `ObjectEffectPackageID` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gametips`
--

DROP TABLE IF EXISTS `gametips`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gametips` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gemproperties`
--

DROP TABLE IF EXISTS `gemproperties`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gemproperties` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellItemEnchantmentRef` int(10) unsigned NOT NULL DEFAULT 0,
  `maxCount_inv` int(10) unsigned NOT NULL DEFAULT 0,
  `maxCount_item` int(10) unsigned NOT NULL DEFAULT 0,
  `gemType` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `generic_scripts`
--

DROP TABLE IF EXISTS `generic_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `generic_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_subsurveys`
--

DROP TABLE IF EXISTS `gm_subsurveys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gm_subsurveys` (
  `surveyId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `subsurveyId` int(10) unsigned NOT NULL DEFAULT 0,
  `rank` int(10) unsigned NOT NULL DEFAULT 0,
  `comment` text NOT NULL,
  PRIMARY KEY (`surveyId`,`subsurveyId`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_surveys`
--

DROP TABLE IF EXISTS `gm_surveys`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gm_surveys` (
  `surveyId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `mainSurvey` int(10) unsigned NOT NULL DEFAULT 0,
  `overallComment` longtext NOT NULL,
  `createTime` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`surveyId`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_ticket_template`
--

DROP TABLE IF EXISTS `gm_ticket_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gm_ticket_template` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL DEFAULT '',
  `text` varchar(256) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gm_tickets`
--

DROP TABLE IF EXISTS `gm_tickets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gm_tickets` (
  `ticketId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `guid` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Global Unique Identifier of ticket creator',
  `name` varchar(12) NOT NULL COMMENT 'Name of ticket creator',
  `message` text NOT NULL,
  `createTime` int(10) unsigned NOT NULL DEFAULT 0,
  `mapId` smallint(5) unsigned NOT NULL DEFAULT 0,
  `posX` float NOT NULL DEFAULT 0,
  `posY` float NOT NULL DEFAULT 0,
  `posZ` float NOT NULL DEFAULT 0,
  `lastModifiedTime` int(10) unsigned NOT NULL DEFAULT 0,
  `closedBy` int(11) NOT NULL DEFAULT 0,
  `assignedTo` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'GUID of admin to whom ticket is assigned',
  `comment` text NOT NULL,
  `response` text NOT NULL,
  `completed` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `escalated` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `viewed` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `haveTicket` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `ticketType` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `securityNeeded` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ticketId`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gossip_menu`
--

DROP TABLE IF EXISTS `gossip_menu`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gossip_menu` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `text_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `script_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`text_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gossip_menu_option`
--

DROP TABLE IF EXISTS `gossip_menu_option`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gossip_menu_option` (
  `menu_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `option_icon` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `option_text` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `option_broadcast_text` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `option_id` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `npc_option_npcflag` int(10) unsigned NOT NULL DEFAULT 0,
  `action_menu_id` mediumint(9) NOT NULL DEFAULT 0,
  `action_poi_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `action_script_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `box_coded` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `box_money` int(10) unsigned NOT NULL DEFAULT 0,
  `box_text` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `box_broadcast_text` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`menu_id`,`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gossip_scripts`
--

DROP TABLE IF EXISTS `gossip_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `gossip_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `holidaydescriptions`
--

DROP TABLE IF EXISTS `holidaydescriptions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `holidaydescriptions` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `description1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `descriptionFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `holidaynames`
--

DROP TABLE IF EXISTS `holidaynames`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `holidaynames` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `holidays`
--

DROP TABLE IF EXISTS `holidays`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `holidays` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `duration1` int(11) NOT NULL DEFAULT 0,
  `duration2` int(11) NOT NULL DEFAULT 0,
  `duration3` int(11) NOT NULL DEFAULT 0,
  `duration4` int(11) NOT NULL DEFAULT 0,
  `duration5` int(11) NOT NULL DEFAULT 0,
  `duration6` int(11) NOT NULL DEFAULT 0,
  `duration7` int(11) NOT NULL DEFAULT 0,
  `duration8` int(11) NOT NULL DEFAULT 0,
  `duration9` int(11) NOT NULL DEFAULT 0,
  `duration10` int(11) NOT NULL DEFAULT 0,
  `eventDate1` int(11) NOT NULL DEFAULT 0,
  `eventDate2` int(11) NOT NULL DEFAULT 0,
  `eventDate3` int(11) NOT NULL DEFAULT 0,
  `eventDate4` int(11) NOT NULL DEFAULT 0,
  `eventDate5` int(11) NOT NULL DEFAULT 0,
  `eventDate6` int(11) NOT NULL DEFAULT 0,
  `eventDate7` int(11) NOT NULL DEFAULT 0,
  `eventDate8` int(11) NOT NULL DEFAULT 0,
  `eventDate9` int(11) NOT NULL DEFAULT 0,
  `eventDate10` int(11) NOT NULL DEFAULT 0,
  `eventDate11` int(11) NOT NULL DEFAULT 0,
  `eventDate12` int(11) NOT NULL DEFAULT 0,
  `eventDate13` int(11) NOT NULL DEFAULT 0,
  `eventDate14` int(11) NOT NULL DEFAULT 0,
  `eventDate15` int(11) NOT NULL DEFAULT 0,
  `eventDate16` int(11) NOT NULL DEFAULT 0,
  `eventDate17` int(11) NOT NULL DEFAULT 0,
  `eventDate18` int(11) NOT NULL DEFAULT 0,
  `eventDate19` int(11) NOT NULL DEFAULT 0,
  `eventDate20` int(11) NOT NULL DEFAULT 0,
  `eventDate21` int(11) NOT NULL DEFAULT 0,
  `eventDate22` int(11) NOT NULL DEFAULT 0,
  `eventDate23` int(11) NOT NULL DEFAULT 0,
  `eventDate24` int(11) NOT NULL DEFAULT 0,
  `eventDate25` int(11) NOT NULL DEFAULT 0,
  `eventDate26` int(11) NOT NULL DEFAULT 0,
  `region` int(11) NOT NULL DEFAULT 0,
  `looping` int(11) NOT NULL DEFAULT 0,
  `calendarFlags1` int(11) NOT NULL DEFAULT 0,
  `calendarFlags2` int(11) NOT NULL DEFAULT 0,
  `calendarFlags3` int(11) NOT NULL DEFAULT 0,
  `calendarFlags4` int(11) NOT NULL DEFAULT 0,
  `calendarFlags5` int(11) NOT NULL DEFAULT 0,
  `calendarFlags6` int(11) NOT NULL DEFAULT 0,
  `calendarFlags7` int(11) NOT NULL DEFAULT 0,
  `calendarFlags8` int(11) NOT NULL DEFAULT 0,
  `calendarFlags9` int(11) NOT NULL DEFAULT 0,
  `calendarFlags10` int(11) NOT NULL DEFAULT 0,
  `eventCalendarName` int(10) unsigned NOT NULL DEFAULT 0,
  `eventCalendarDescription` int(10) unsigned NOT NULL DEFAULT 0,
  `eventCalendarOverlay` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `priority` int(11) NOT NULL DEFAULT 0,
  `eventSchedulerType` int(11) NOT NULL DEFAULT 0,
  `eventFlags` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `instance_buff_removal`
--

DROP TABLE IF EXISTS `instance_buff_removal`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `instance_buff_removal` (
  `map_id` int(10) unsigned NOT NULL COMMENT 'MapId to remove aura from',
  `spell_id` int(10) unsigned NOT NULL COMMENT 'aura id to remove on entering MapId',
  `enabled` tinyint(1) NOT NULL COMMENT 'aura removal enabled or not',
  `flags` int(11) NOT NULL COMMENT 'flags, see AuraRemovalMgr.h',
  `comment` varchar(256) NOT NULL COMMENT 'description, what is removed',
  PRIMARY KEY (`map_id`,`spell_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Aura removal on map entry';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item`
--

DROP TABLE IF EXISTS `item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item` (
  `itemID` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemClass` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemSubClass` int(10) unsigned NOT NULL DEFAULT 0,
  `sound_override_subclassid` int(11) NOT NULL DEFAULT 0,
  `MaterialID` int(11) NOT NULL DEFAULT 0,
  `ItemDisplayInfo` int(10) unsigned NOT NULL DEFAULT 0,
  `InventorySlotID` int(10) unsigned NOT NULL DEFAULT 0,
  `SheathID` int(10) unsigned NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_display_info`
--

DROP TABLE IF EXISTS `item_display_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_display_info` (
  `ID` int(11) NOT NULL,
  `icon` varchar(255) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_enchantment_template`
--

DROP TABLE IF EXISTS `item_enchantment_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_enchantment_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ench` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `chance` float unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`ench`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Item Random Enchantment System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_loot_template`
--

DROP TABLE IF EXISTS `item_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_required_target`
--

DROP TABLE IF EXISTS `item_required_target`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_required_target` (
  `entry` mediumint(8) unsigned NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `target_entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  UNIQUE KEY `entry_type_target` (`entry`,`type`,`target_entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_template`
--

DROP TABLE IF EXISTS `item_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `subclass` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `name` varchar(255) NOT NULL DEFAULT '',
  `description` varchar(255) NOT NULL DEFAULT '',
  `display_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `quality` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `buy_count` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `buy_price` int(10) unsigned NOT NULL DEFAULT 0,
  `sell_price` int(10) unsigned NOT NULL DEFAULT 0,
  `inventory_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `allowable_class` mediumint(9) NOT NULL DEFAULT -1,
  `allowable_race` mediumint(9) NOT NULL DEFAULT -1,
  `item_level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `required_level` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `required_skill` smallint(5) unsigned NOT NULL DEFAULT 0,
  `required_skill_rank` smallint(5) unsigned NOT NULL DEFAULT 0,
  `required_spell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `required_honor_rank` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `required_city_rank` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `required_reputation_faction` smallint(5) unsigned NOT NULL DEFAULT 0,
  `required_reputation_rank` smallint(5) unsigned NOT NULL DEFAULT 0,
  `max_count` smallint(5) unsigned NOT NULL DEFAULT 0,
  `stackable` smallint(5) unsigned NOT NULL DEFAULT 1,
  `container_slots` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_type1` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value1` smallint(6) NOT NULL DEFAULT 0,
  `stat_type2` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value2` smallint(6) NOT NULL DEFAULT 0,
  `stat_type3` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value3` smallint(6) NOT NULL DEFAULT 0,
  `stat_type4` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value4` smallint(6) NOT NULL DEFAULT 0,
  `stat_type5` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value5` smallint(6) NOT NULL DEFAULT 0,
  `stat_type6` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value6` smallint(6) NOT NULL DEFAULT 0,
  `stat_type7` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value7` smallint(6) NOT NULL DEFAULT 0,
  `stat_type8` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value8` smallint(6) NOT NULL DEFAULT 0,
  `stat_type9` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value9` smallint(6) NOT NULL DEFAULT 0,
  `stat_type10` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `stat_value10` smallint(6) NOT NULL DEFAULT 0,
  `delay` smallint(5) unsigned NOT NULL DEFAULT 1000,
  `range_mod` float NOT NULL DEFAULT 0,
  `ammo_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dmg_min1` float NOT NULL DEFAULT 0,
  `dmg_max1` float NOT NULL DEFAULT 0,
  `dmg_type1` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dmg_min2` float NOT NULL DEFAULT 0,
  `dmg_max2` float NOT NULL DEFAULT 0,
  `dmg_type2` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dmg_min3` float NOT NULL DEFAULT 0,
  `dmg_max3` float NOT NULL DEFAULT 0,
  `dmg_type3` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dmg_min4` float NOT NULL DEFAULT 0,
  `dmg_max4` float NOT NULL DEFAULT 0,
  `dmg_type4` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dmg_min5` float NOT NULL DEFAULT 0,
  `dmg_max5` float NOT NULL DEFAULT 0,
  `dmg_type5` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `block` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `armor` smallint(6) NOT NULL DEFAULT 0,
  `holy_res` smallint(6) NOT NULL DEFAULT 0,
  `fire_res` smallint(6) NOT NULL DEFAULT 0,
  `nature_res` smallint(6) NOT NULL DEFAULT 0,
  `frost_res` smallint(6) NOT NULL DEFAULT 0,
  `shadow_res` smallint(6) NOT NULL DEFAULT 0,
  `arcane_res` smallint(6) NOT NULL DEFAULT 0,
  `spellid_1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spelltrigger_1` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spellcharges_1` tinyint(4) NOT NULL DEFAULT 0,
  `spellppmrate_1` float NOT NULL DEFAULT 0,
  `spellcooldown_1` int(11) NOT NULL DEFAULT -1,
  `spellcategory_1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spellcategorycooldown_1` int(11) NOT NULL DEFAULT -1,
  `spellid_2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spelltrigger_2` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spellcharges_2` tinyint(4) NOT NULL DEFAULT 0,
  `spellppmrate_2` float NOT NULL DEFAULT 0,
  `spellcooldown_2` int(11) NOT NULL DEFAULT -1,
  `spellcategory_2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spellcategorycooldown_2` int(11) NOT NULL DEFAULT -1,
  `spellid_3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spelltrigger_3` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spellcharges_3` tinyint(4) NOT NULL DEFAULT 0,
  `spellppmrate_3` float NOT NULL DEFAULT 0,
  `spellcooldown_3` int(11) NOT NULL DEFAULT -1,
  `spellcategory_3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spellcategorycooldown_3` int(11) NOT NULL DEFAULT -1,
  `spellid_4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spelltrigger_4` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spellcharges_4` tinyint(4) NOT NULL DEFAULT 0,
  `spellppmrate_4` float NOT NULL DEFAULT 0,
  `spellcooldown_4` int(11) NOT NULL DEFAULT -1,
  `spellcategory_4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spellcategorycooldown_4` int(11) NOT NULL DEFAULT -1,
  `spellid_5` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spelltrigger_5` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spellcharges_5` tinyint(4) NOT NULL DEFAULT 0,
  `spellppmrate_5` float NOT NULL DEFAULT 0,
  `spellcooldown_5` int(11) NOT NULL DEFAULT -1,
  `spellcategory_5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spellcategorycooldown_5` int(11) NOT NULL DEFAULT -1,
  `bonding` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `page_text` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `page_language` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `page_material` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `start_quest` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `lock_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `material` tinyint(4) NOT NULL DEFAULT 0,
  `sheath` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `random_property` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `set_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `max_durability` smallint(5) unsigned NOT NULL DEFAULT 0,
  `area_bound` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `map_bound` smallint(6) NOT NULL DEFAULT 0,
  `duration` int(10) unsigned NOT NULL DEFAULT 0,
  `bag_family` mediumint(9) NOT NULL DEFAULT 0,
  `disenchant_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `food_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `min_money_loot` int(10) unsigned NOT NULL DEFAULT 0,
  `max_money_loot` int(10) unsigned NOT NULL DEFAULT 0,
  `wrapped_gift` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `extra_flags` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `other_team_entry` int(10) unsigned DEFAULT 1,
  `script_name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE,
  KEY `items_index` (`class`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Item System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `item_transmogrify_template`
--

DROP TABLE IF EXISTS `item_transmogrify_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `item_transmogrify_template` (
  `ID` int(10) unsigned NOT NULL,
  `ItemID` int(10) unsigned NOT NULL,
  `DisplayID` int(10) unsigned NOT NULL,
  `SourceID` int(11) NOT NULL,
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `itemclass`
--

DROP TABLE IF EXISTS `itemclass`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `itemclass` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `SecondaryID` int(10) unsigned NOT NULL DEFAULT 0,
  `IsWeapon` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `itemdisplayinfo`
--

DROP TABLE IF EXISTS `itemdisplayinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `itemdisplayinfo` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `LeftModel` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `RightModel` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `LeftModelTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `RightModelTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `InventoryIcon1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `InventoryIcon2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `GeosetGroup1` int(10) unsigned NOT NULL DEFAULT 0,
  `GeosetGroup2` int(10) unsigned NOT NULL DEFAULT 0,
  `GeosetGroup3` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellVisualID` int(10) unsigned NOT NULL DEFAULT 0,
  `GroupSoundIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `HelmetGeosetVisual1` int(10) unsigned NOT NULL DEFAULT 0,
  `HelmetGeosetVisual2` int(10) unsigned NOT NULL DEFAULT 0,
  `UpperArmTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `LowerArmTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `HandsTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `UpperTorsoTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `LowerTorsoTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `UpperLegTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `LowerLegTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `FootTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ItemVisual` int(10) unsigned NOT NULL DEFAULT 0,
  `ParticleColorID` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `itemextendedcost`
--

DROP TABLE IF EXISTS `itemextendedcost`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `itemextendedcost` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `costHonour` int(10) unsigned NOT NULL DEFAULT 0,
  `costArena` int(10) unsigned NOT NULL DEFAULT 0,
  `unknown1` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItem1` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItem2` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItem3` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItem4` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItem5` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItemCount1` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItemCount2` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItemCount3` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItemCount4` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredItemCount5` int(10) unsigned NOT NULL DEFAULT 0,
  `personalRating` int(10) unsigned NOT NULL DEFAULT 0,
  `purchaseGroup` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `itemset`
--

DROP TABLE IF EXISTS `itemset`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `itemset` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId1` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId2` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId3` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId4` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId5` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId6` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId7` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId8` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId9` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId10` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId11` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId12` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId13` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId14` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId15` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId16` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemId17` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus1` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus2` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus3` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus4` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus5` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus6` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus7` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonus8` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold1` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold2` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold3` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold4` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold5` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold6` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold7` int(10) unsigned NOT NULL DEFAULT 0,
  `SetBonusThreshold8` int(10) unsigned NOT NULL DEFAULT 0,
  `RequiredSkill` int(10) unsigned NOT NULL DEFAULT 0,
  `RequiredSkillRank` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `itemsubclass`
--

DROP TABLE IF EXISTS `itemsubclass`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `itemsubclass` (
  `Class` int(10) unsigned NOT NULL DEFAULT 0,
  `subClass` int(10) unsigned NOT NULL DEFAULT 0,
  `prerequisiteProficiency` int(10) unsigned NOT NULL DEFAULT 0,
  `postrequisiteProficiency` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `displayFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `weaponParrySeq` int(10) unsigned NOT NULL DEFAULT 0,
  `weaponReadySeq` int(10) unsigned NOT NULL DEFAULT 0,
  `weaponAttackSeq` int(10) unsigned NOT NULL DEFAULT 0,
  `WeaponSwingSize` int(10) unsigned NOT NULL DEFAULT 0,
  `displayName1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayName16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `displayNameFlag` int(10) unsigned NOT NULL DEFAULT 0,
  `verboseName1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseName16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `verboseNameFlag` int(10) unsigned NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lfgdungeongroup`
--

DROP TABLE IF EXISTS `lfgdungeongroup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `lfgdungeongroup` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `order` int(10) unsigned NOT NULL DEFAULT 0,
  `parent` int(10) unsigned NOT NULL DEFAULT 0,
  `type` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lfgdungeons`
--

DROP TABLE IF EXISTS `lfgdungeons`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `lfgdungeons` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `minLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `maxLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `targetLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `targetLevelMin` int(10) unsigned NOT NULL DEFAULT 0,
  `targetLevelMax` int(10) unsigned NOT NULL DEFAULT 0,
  `mapId` int(10) unsigned NOT NULL DEFAULT 0,
  `difficulty` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `typeId` int(10) unsigned NOT NULL DEFAULT 0,
  `faction` int(11) NOT NULL DEFAULT 0,
  `texture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `expansion` int(10) unsigned NOT NULL DEFAULT 0,
  `orderId` int(10) unsigned NOT NULL DEFAULT 0,
  `groupId` int(10) unsigned NOT NULL DEFAULT 0,
  `tooltip1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltipFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `light`
--

DROP TABLE IF EXISTS `light`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `light` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `mapId` int(11) NOT NULL DEFAULT 0,
  `posX` float NOT NULL DEFAULT 0,
  `posY` float NOT NULL DEFAULT 0,
  `posZ` float NOT NULL DEFAULT 0,
  `falloffStart` float NOT NULL DEFAULT 0,
  `falloffEnd` float NOT NULL DEFAULT 0,
  `paramsClear` int(11) NOT NULL DEFAULT 0,
  `paramsClearUnderwater` int(11) NOT NULL DEFAULT 0,
  `paramsStorm` int(11) NOT NULL DEFAULT 0,
  `paramsStormUnderwater` int(11) NOT NULL DEFAULT 0,
  `paramsDeath` int(11) NOT NULL DEFAULT 0,
  `paramsUnk1` int(11) NOT NULL DEFAULT 0,
  `paramsUnk2` int(11) NOT NULL DEFAULT 0,
  `paramsUnk3` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lightfloatband`
--

DROP TABLE IF EXISTS `lightfloatband`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `lightfloatband` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `numEntries` int(11) NOT NULL DEFAULT 0,
  `timeValue1` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue2` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue3` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue4` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue5` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue6` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue7` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue8` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue9` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue10` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue11` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue12` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue13` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue14` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue15` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue16` int(10) unsigned NOT NULL DEFAULT 0,
  `floatValue1` float NOT NULL DEFAULT 0,
  `floatValue2` float NOT NULL DEFAULT 0,
  `floatValue3` float NOT NULL DEFAULT 0,
  `floatValue4` float NOT NULL DEFAULT 0,
  `floatValue5` float NOT NULL DEFAULT 0,
  `floatValue6` float NOT NULL DEFAULT 0,
  `floatValue7` float NOT NULL DEFAULT 0,
  `floatValue8` float NOT NULL DEFAULT 0,
  `floatValue9` float NOT NULL DEFAULT 0,
  `floatValue10` float NOT NULL DEFAULT 0,
  `floatValue11` float NOT NULL DEFAULT 0,
  `floatValue12` float NOT NULL DEFAULT 0,
  `floatValue13` float NOT NULL DEFAULT 0,
  `floatValue14` float NOT NULL DEFAULT 0,
  `floatValue15` float NOT NULL DEFAULT 0,
  `floatValue16` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lightintband`
--

DROP TABLE IF EXISTS `lightintband`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `lightintband` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `numEntries` int(11) NOT NULL DEFAULT 0,
  `timeValue1` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue2` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue3` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue4` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue5` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue6` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue7` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue8` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue9` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue10` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue11` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue12` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue13` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue14` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue15` int(10) unsigned NOT NULL DEFAULT 0,
  `timeValue16` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue1` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue2` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue3` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue4` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue5` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue6` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue7` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue8` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue9` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue10` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue11` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue12` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue13` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue14` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue15` int(10) unsigned NOT NULL DEFAULT 0,
  `colourValue16` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lightparams`
--

DROP TABLE IF EXISTS `lightparams`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `lightparams` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `higlightSky` int(10) unsigned NOT NULL DEFAULT 0,
  `skyboxId` int(10) unsigned NOT NULL DEFAULT 0,
  `cloudType` int(11) NOT NULL DEFAULT 0,
  `glow` float NOT NULL DEFAULT 0,
  `waterShallowAlpha` float NOT NULL DEFAULT 0,
  `waterDeepAlpha` float NOT NULL DEFAULT 0,
  `oceanShallowAlpha` float NOT NULL DEFAULT 0,
  `oceanDeepAlpha` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lightskybox`
--

DROP TABLE IF EXISTS `lightskybox`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `lightskybox` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `loadingscreens`
--

DROP TABLE IF EXISTS `loadingscreens`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `loadingscreens` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `FileName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `HasWideScreen` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_area`
--

DROP TABLE IF EXISTS `locales_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_area` (
  `Entry` mediumint(9) NOT NULL DEFAULT 0,
  `NameLoc0` varchar(100) NOT NULL DEFAULT '',
  `NameLoc1` varchar(100) NOT NULL DEFAULT '',
  `NameLoc2` varchar(100) NOT NULL DEFAULT '',
  `NameLoc3` varchar(100) NOT NULL DEFAULT '',
  `NameLoc4` varchar(100) NOT NULL DEFAULT '',
  `NameLoc5` varchar(100) NOT NULL DEFAULT '',
  `NameLoc6` varchar(100) NOT NULL DEFAULT '',
  `NameLoc7` varchar(100) NOT NULL DEFAULT '',
  `NameLoc8` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`Entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_broadcast_text`
--

DROP TABLE IF EXISTS `locales_broadcast_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_broadcast_text` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `male_text_loc0` longtext DEFAULT NULL,
  `male_text_loc1` longtext DEFAULT NULL,
  `male_text_loc2` longtext DEFAULT NULL,
  `male_text_loc3` longtext DEFAULT NULL,
  `male_text_loc4` longtext DEFAULT NULL,
  `male_text_loc5` longtext DEFAULT NULL,
  `male_text_loc6` longtext DEFAULT NULL,
  `male_text_loc7` longtext DEFAULT NULL,
  `male_text_loc8` longtext DEFAULT NULL,
  `female_text_loc0` longtext DEFAULT NULL,
  `female_text_loc1` longtext DEFAULT NULL,
  `female_text_loc2` longtext DEFAULT NULL,
  `female_text_loc3` longtext DEFAULT NULL,
  `female_text_loc4` longtext DEFAULT NULL,
  `female_text_loc5` longtext DEFAULT NULL,
  `female_text_loc6` longtext DEFAULT NULL,
  `female_text_loc7` longtext DEFAULT NULL,
  `female_text_loc8` longtext DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_creature`
--

DROP TABLE IF EXISTS `locales_creature`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_creature` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `name_loc0` varchar(100) NOT NULL DEFAULT '',
  `name_loc1` varchar(100) NOT NULL DEFAULT '',
  `name_loc2` varchar(100) NOT NULL DEFAULT '',
  `name_loc3` varchar(100) NOT NULL DEFAULT '',
  `name_loc4` varchar(100) NOT NULL DEFAULT '',
  `name_loc5` varchar(100) NOT NULL DEFAULT '',
  `name_loc6` varchar(100) NOT NULL DEFAULT '',
  `name_loc7` varchar(100) NOT NULL DEFAULT '',
  `name_loc8` varchar(100) NOT NULL DEFAULT '',
  `subname_loc0` varchar(100) DEFAULT NULL,
  `subname_loc1` varchar(100) DEFAULT NULL,
  `subname_loc2` varchar(100) DEFAULT NULL,
  `subname_loc3` varchar(100) DEFAULT NULL,
  `subname_loc4` varchar(100) DEFAULT NULL,
  `subname_loc5` varchar(100) DEFAULT NULL,
  `subname_loc6` varchar(100) DEFAULT NULL,
  `subname_loc7` varchar(100) DEFAULT NULL,
  `subname_loc8` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_faction`
--

DROP TABLE IF EXISTS `locales_faction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_faction` (
  `entry` int(10) unsigned NOT NULL,
  `name_loc0` varchar(256) NOT NULL DEFAULT '',
  `name_loc1` varchar(256) NOT NULL DEFAULT '',
  `name_loc2` varchar(256) NOT NULL DEFAULT '',
  `name_loc3` varchar(256) NOT NULL DEFAULT '',
  `name_loc4` varchar(256) NOT NULL DEFAULT '',
  `name_loc5` varchar(256) NOT NULL DEFAULT '',
  `name_loc6` varchar(256) NOT NULL DEFAULT '',
  `description_loc0` varchar(512) NOT NULL DEFAULT '',
  `description_loc1` varchar(512) NOT NULL DEFAULT '',
  `description_loc2` varchar(512) NOT NULL DEFAULT '',
  `description_loc3` varchar(512) NOT NULL DEFAULT '',
  `description_loc4` varchar(512) NOT NULL DEFAULT '',
  `description_loc5` varchar(512) NOT NULL DEFAULT '',
  `description_loc6` varchar(512) NOT NULL DEFAULT '',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_gameobject`
--

DROP TABLE IF EXISTS `locales_gameobject`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_gameobject` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `name_loc0` varchar(100) NOT NULL DEFAULT '',
  `name_loc1` varchar(100) NOT NULL DEFAULT '',
  `name_loc2` varchar(100) NOT NULL DEFAULT '',
  `name_loc3` varchar(100) NOT NULL DEFAULT '',
  `name_loc4` varchar(100) NOT NULL DEFAULT '',
  `name_loc5` varchar(100) NOT NULL DEFAULT '',
  `name_loc6` varchar(100) NOT NULL DEFAULT '',
  `name_loc7` varchar(100) NOT NULL DEFAULT '',
  `name_loc8` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_gossip_menu_option`
--

DROP TABLE IF EXISTS `locales_gossip_menu_option`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_gossip_menu_option` (
  `menu_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `option_text_loc0` text DEFAULT NULL,
  `option_text_loc1` text DEFAULT NULL,
  `option_text_loc2` text DEFAULT NULL,
  `option_text_loc3` text DEFAULT NULL,
  `option_text_loc4` text DEFAULT NULL,
  `option_text_loc5` text DEFAULT NULL,
  `option_text_loc6` text DEFAULT NULL,
  `option_text_loc7` text DEFAULT NULL,
  `option_text_loc8` text DEFAULT NULL,
  `box_text_loc0` text DEFAULT NULL,
  `box_text_loc1` text DEFAULT NULL,
  `box_text_loc2` text DEFAULT NULL,
  `box_text_loc3` text DEFAULT NULL,
  `box_text_loc4` text DEFAULT NULL,
  `box_text_loc5` text DEFAULT NULL,
  `box_text_loc6` text DEFAULT NULL,
  `box_text_loc7` text DEFAULT NULL,
  `box_text_loc8` text DEFAULT NULL,
  PRIMARY KEY (`menu_id`,`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_item`
--

DROP TABLE IF EXISTS `locales_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_item` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `name_loc0` varchar(100) NOT NULL DEFAULT '',
  `name_loc1` varchar(100) NOT NULL DEFAULT '',
  `name_loc2` varchar(100) NOT NULL DEFAULT '',
  `name_loc3` varchar(100) NOT NULL DEFAULT '',
  `name_loc4` varchar(100) NOT NULL DEFAULT '',
  `name_loc5` varchar(100) NOT NULL DEFAULT '',
  `name_loc6` varchar(100) NOT NULL DEFAULT '',
  `name_loc7` varchar(100) NOT NULL DEFAULT '',
  `name_loc8` varchar(100) NOT NULL DEFAULT '',
  `description_loc0` varchar(255) DEFAULT NULL,
  `description_loc1` varchar(255) DEFAULT NULL,
  `description_loc2` varchar(255) DEFAULT NULL,
  `description_loc3` varchar(255) DEFAULT NULL,
  `description_loc4` varchar(255) DEFAULT NULL,
  `description_loc5` varchar(255) DEFAULT NULL,
  `description_loc6` varchar(255) DEFAULT NULL,
  `description_loc7` varchar(255) DEFAULT NULL,
  `description_loc8` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_page_text`
--

DROP TABLE IF EXISTS `locales_page_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_page_text` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Text_loc0` longtext DEFAULT NULL,
  `Text_loc1` longtext DEFAULT NULL,
  `Text_loc2` longtext DEFAULT NULL,
  `Text_loc3` longtext DEFAULT NULL,
  `Text_loc4` longtext DEFAULT NULL,
  `Text_loc5` longtext DEFAULT NULL,
  `Text_loc6` longtext DEFAULT NULL,
  `Text_loc7` longtext DEFAULT NULL,
  `Text_loc8` longtext DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_points_of_interest`
--

DROP TABLE IF EXISTS `locales_points_of_interest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_points_of_interest` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `icon_name_loc0` text DEFAULT NULL,
  `icon_name_loc1` text DEFAULT NULL,
  `icon_name_loc2` text DEFAULT NULL,
  `icon_name_loc3` text DEFAULT NULL,
  `icon_name_loc4` text DEFAULT NULL,
  `icon_name_loc5` text DEFAULT NULL,
  `icon_name_loc6` text DEFAULT NULL,
  `icon_name_loc7` text DEFAULT NULL,
  `icon_name_loc8` text DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_quest`
--

DROP TABLE IF EXISTS `locales_quest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_quest` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Title_loc0` text DEFAULT NULL,
  `Title_loc1` text DEFAULT NULL,
  `Title_loc2` text DEFAULT NULL,
  `Title_loc3` text DEFAULT NULL,
  `Title_loc4` text DEFAULT NULL,
  `Title_loc5` text DEFAULT NULL,
  `Title_loc6` text DEFAULT NULL,
  `Title_loc7` text DEFAULT NULL,
  `Title_loc8` text DEFAULT NULL,
  `Details_loc0` text DEFAULT NULL,
  `Details_loc1` text DEFAULT NULL,
  `Details_loc2` text DEFAULT NULL,
  `Details_loc3` text DEFAULT NULL,
  `Details_loc4` text DEFAULT NULL,
  `Details_loc5` text DEFAULT NULL,
  `Details_loc6` text DEFAULT NULL,
  `Details_loc7` text DEFAULT NULL,
  `Details_loc8` text DEFAULT NULL,
  `Objectives_loc0` text DEFAULT NULL,
  `Objectives_loc1` text DEFAULT NULL,
  `Objectives_loc2` text DEFAULT NULL,
  `Objectives_loc3` text DEFAULT NULL,
  `Objectives_loc4` text DEFAULT NULL,
  `Objectives_loc5` text DEFAULT NULL,
  `Objectives_loc6` text DEFAULT NULL,
  `Objectives_loc7` text DEFAULT NULL,
  `Objectives_loc8` text DEFAULT NULL,
  `OfferRewardText_loc0` text DEFAULT NULL,
  `OfferRewardText_loc1` text DEFAULT NULL,
  `OfferRewardText_loc2` text DEFAULT NULL,
  `OfferRewardText_loc3` text DEFAULT NULL,
  `OfferRewardText_loc4` text DEFAULT NULL,
  `OfferRewardText_loc5` text DEFAULT NULL,
  `OfferRewardText_loc6` text DEFAULT NULL,
  `OfferRewardText_loc7` text DEFAULT NULL,
  `OfferRewardText_loc8` text DEFAULT NULL,
  `RequestItemsText_loc0` text DEFAULT NULL,
  `RequestItemsText_loc1` text DEFAULT NULL,
  `RequestItemsText_loc2` text DEFAULT NULL,
  `RequestItemsText_loc3` text DEFAULT NULL,
  `RequestItemsText_loc4` text DEFAULT NULL,
  `RequestItemsText_loc5` text DEFAULT NULL,
  `RequestItemsText_loc6` text DEFAULT NULL,
  `RequestItemsText_loc7` text DEFAULT NULL,
  `RequestItemsText_loc8` text DEFAULT NULL,
  `EndText_loc0` text DEFAULT NULL,
  `EndText_loc1` text DEFAULT NULL,
  `EndText_loc2` text DEFAULT NULL,
  `EndText_loc3` text DEFAULT NULL,
  `EndText_loc4` text DEFAULT NULL,
  `EndText_loc5` text DEFAULT NULL,
  `EndText_loc6` text DEFAULT NULL,
  `EndText_loc7` text DEFAULT NULL,
  `EndText_loc8` text DEFAULT NULL,
  `ObjectiveText1_loc0` text DEFAULT NULL,
  `ObjectiveText1_loc1` text DEFAULT NULL,
  `ObjectiveText1_loc2` text DEFAULT NULL,
  `ObjectiveText1_loc3` text DEFAULT NULL,
  `ObjectiveText1_loc4` text DEFAULT NULL,
  `ObjectiveText1_loc5` text DEFAULT NULL,
  `ObjectiveText1_loc6` text DEFAULT NULL,
  `ObjectiveText1_loc7` text DEFAULT NULL,
  `ObjectiveText1_loc8` text DEFAULT NULL,
  `ObjectiveText2_loc0` text DEFAULT NULL,
  `ObjectiveText2_loc1` text DEFAULT NULL,
  `ObjectiveText2_loc2` text DEFAULT NULL,
  `ObjectiveText2_loc3` text DEFAULT NULL,
  `ObjectiveText2_loc4` text DEFAULT NULL,
  `ObjectiveText2_loc5` text DEFAULT NULL,
  `ObjectiveText2_loc6` text DEFAULT NULL,
  `ObjectiveText2_loc7` text DEFAULT NULL,
  `ObjectiveText2_loc8` text DEFAULT NULL,
  `ObjectiveText3_loc0` text DEFAULT NULL,
  `ObjectiveText3_loc1` text DEFAULT NULL,
  `ObjectiveText3_loc2` text DEFAULT NULL,
  `ObjectiveText3_loc3` text DEFAULT NULL,
  `ObjectiveText3_loc4` text DEFAULT NULL,
  `ObjectiveText3_loc5` text DEFAULT NULL,
  `ObjectiveText3_loc6` text DEFAULT NULL,
  `ObjectiveText3_loc7` text DEFAULT NULL,
  `ObjectiveText3_loc8` text DEFAULT NULL,
  `ObjectiveText4_loc0` text DEFAULT NULL,
  `ObjectiveText4_loc1` text DEFAULT NULL,
  `ObjectiveText4_loc2` text DEFAULT NULL,
  `ObjectiveText4_loc3` text DEFAULT NULL,
  `ObjectiveText4_loc4` text DEFAULT NULL,
  `ObjectiveText4_loc5` text DEFAULT NULL,
  `ObjectiveText4_loc6` text DEFAULT NULL,
  `ObjectiveText4_loc7` text DEFAULT NULL,
  `ObjectiveText4_loc8` text DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_spell`
--

DROP TABLE IF EXISTS `locales_spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_spell` (
  `entry` int(10) unsigned NOT NULL,
  `name_loc0` varchar(256) NOT NULL DEFAULT '',
  `name_loc1` varchar(256) NOT NULL DEFAULT '',
  `name_loc2` varchar(256) NOT NULL DEFAULT '',
  `name_loc3` varchar(256) NOT NULL DEFAULT '',
  `name_loc4` varchar(256) NOT NULL DEFAULT '',
  `name_loc5` varchar(256) NOT NULL DEFAULT '',
  `name_loc6` varchar(256) NOT NULL DEFAULT '',
  `nameSubtext_loc0` varchar(256) NOT NULL DEFAULT '',
  `nameSubtext_loc1` varchar(256) NOT NULL DEFAULT '',
  `nameSubtext_loc2` varchar(256) NOT NULL DEFAULT '',
  `nameSubtext_loc3` varchar(256) NOT NULL DEFAULT '',
  `nameSubtext_loc4` varchar(256) NOT NULL DEFAULT '',
  `nameSubtext_loc5` varchar(256) NOT NULL DEFAULT '',
  `nameSubtext_loc6` varchar(256) NOT NULL DEFAULT '',
  `description_loc0` varchar(1024) NOT NULL DEFAULT '',
  `description_loc1` varchar(1024) NOT NULL DEFAULT '',
  `description_loc2` varchar(1024) NOT NULL DEFAULT '',
  `description_loc3` varchar(1024) NOT NULL DEFAULT '',
  `description_loc4` varchar(1024) NOT NULL DEFAULT '',
  `description_loc5` varchar(1024) NOT NULL DEFAULT '',
  `description_loc6` varchar(1024) NOT NULL DEFAULT '',
  `auraDescription_loc0` varchar(512) NOT NULL DEFAULT '',
  `auraDescription_loc1` varchar(512) NOT NULL DEFAULT '',
  `auraDescription_loc2` varchar(512) NOT NULL DEFAULT '',
  `auraDescription_loc3` varchar(512) NOT NULL DEFAULT '',
  `auraDescription_loc4` varchar(512) NOT NULL DEFAULT '',
  `auraDescription_loc5` varchar(512) NOT NULL DEFAULT '',
  `auraDescription_loc6` varchar(512) NOT NULL DEFAULT '',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locales_taxi_node`
--

DROP TABLE IF EXISTS `locales_taxi_node`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locales_taxi_node` (
  `entry` int(10) unsigned NOT NULL,
  `name_loc0` varchar(256) NOT NULL DEFAULT '',
  `name_loc1` varchar(256) NOT NULL DEFAULT '',
  `name_loc2` varchar(256) NOT NULL DEFAULT '',
  `name_loc3` varchar(256) NOT NULL DEFAULT '',
  `name_loc4` varchar(256) NOT NULL DEFAULT '',
  `name_loc5` varchar(256) NOT NULL DEFAULT '',
  `name_loc6` varchar(256) NOT NULL DEFAULT '',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `lock`
--

DROP TABLE IF EXISTS `lock`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `lock` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `type1` int(10) unsigned NOT NULL DEFAULT 0,
  `type2` int(10) unsigned NOT NULL DEFAULT 0,
  `type3` int(10) unsigned NOT NULL DEFAULT 0,
  `type4` int(10) unsigned NOT NULL DEFAULT 0,
  `type5` int(10) unsigned NOT NULL DEFAULT 0,
  `type6` int(10) unsigned NOT NULL DEFAULT 0,
  `type7` int(10) unsigned NOT NULL DEFAULT 0,
  `type8` int(10) unsigned NOT NULL DEFAULT 0,
  `index1` int(10) unsigned NOT NULL DEFAULT 0,
  `index2` int(10) unsigned NOT NULL DEFAULT 0,
  `index3` int(10) unsigned NOT NULL DEFAULT 0,
  `index4` int(10) unsigned NOT NULL DEFAULT 0,
  `index5` int(10) unsigned NOT NULL DEFAULT 0,
  `index6` int(10) unsigned NOT NULL DEFAULT 0,
  `index7` int(10) unsigned NOT NULL DEFAULT 0,
  `index8` int(10) unsigned NOT NULL DEFAULT 0,
  `skill1` int(10) unsigned NOT NULL DEFAULT 0,
  `skill2` int(10) unsigned NOT NULL DEFAULT 0,
  `skill3` int(10) unsigned NOT NULL DEFAULT 0,
  `skill4` int(10) unsigned NOT NULL DEFAULT 0,
  `skill5` int(10) unsigned NOT NULL DEFAULT 0,
  `skill6` int(10) unsigned NOT NULL DEFAULT 0,
  `skill7` int(10) unsigned NOT NULL DEFAULT 0,
  `skill8` int(10) unsigned NOT NULL DEFAULT 0,
  `action1` int(10) unsigned NOT NULL DEFAULT 0,
  `action2` int(10) unsigned NOT NULL DEFAULT 0,
  `action3` int(10) unsigned NOT NULL DEFAULT 0,
  `action4` int(10) unsigned NOT NULL DEFAULT 0,
  `action5` int(10) unsigned NOT NULL DEFAULT 0,
  `action6` int(10) unsigned NOT NULL DEFAULT 0,
  `action7` int(10) unsigned NOT NULL DEFAULT 0,
  `action8` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `locktype`
--

DROP TABLE IF EXISTS `locktype`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `locktype` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `ResourceName1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceName16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ResourceNameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `Verb1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Verb16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `VerbFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `CursorName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mail_loot_template`
--

DROP TABLE IF EXISTS `mail_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mail_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mail_text_template`
--

DROP TABLE IF EXISTS `mail_text_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mail_text_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `content_default` mediumtext NOT NULL,
  `content_loc1` mediumtext DEFAULT NULL,
  `content_loc2` mediumtext DEFAULT NULL,
  `content_loc3` mediumtext DEFAULT NULL,
  `content_loc4` mediumtext DEFAULT NULL,
  `content_loc5` mediumtext DEFAULT NULL,
  `content_loc6` mediumtext DEFAULT NULL,
  `content_loc7` mediumtext DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mangos_string`
--

DROP TABLE IF EXISTS `mangos_string`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mangos_string` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `content_default` text NOT NULL,
  `content_loc1` text DEFAULT NULL,
  `content_loc2` text DEFAULT NULL,
  `content_loc3` text DEFAULT NULL,
  `content_loc4` text DEFAULT NULL,
  `content_loc5` text DEFAULT NULL,
  `content_loc6` text DEFAULT NULL,
  `content_loc7` text DEFAULT NULL,
  `content_loc8` text DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `map`
--

DROP TABLE IF EXISTS `map`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `map` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `DirectoryName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `InstanceType` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `PVP` int(10) unsigned NOT NULL DEFAULT 0,
  `MapName_Lang_enUS` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_enGB` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_koKR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_frFR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_deDE` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_enCN` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_zhCN` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_enTW` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_zhTW` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_esES` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_esMX` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_ruRU` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_ptPT` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_ptBR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_itIT` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_Unk` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapName_Lang_Mask` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaTableID` int(10) unsigned NOT NULL DEFAULT 0,
  `MapDescription0_Lang_enUS` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_enGB` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_koKR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_frFR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_deDE` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_enCN` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_zhCN` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_enTW` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_zhTW` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_esES` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_esMX` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_ruRU` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_ptPT` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_ptBR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_itIT` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_Unk` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription0_Lang_Mask` int(10) unsigned NOT NULL DEFAULT 0,
  `MapDescription1_Lang_enUS` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_enGB` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_koKR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_frFR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_deDE` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_enCN` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_zhCN` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_enTW` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_zhTW` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_esES` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_esMX` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_ruRU` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_ptPT` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_ptBR` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_itIT` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_Unk` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `MapDescription1_Lang_Mask` int(10) unsigned NOT NULL DEFAULT 0,
  `LoadingScreenID` int(10) unsigned NOT NULL DEFAULT 0,
  `MinimapIconScale` float NOT NULL DEFAULT 0,
  `CorpseMapID` int(11) NOT NULL DEFAULT 0,
  `CorpseX` float NOT NULL DEFAULT 0,
  `CorpseY` float NOT NULL DEFAULT 0,
  `TimeOfDayOverride` int(11) NOT NULL DEFAULT 0,
  `ExpansionID` int(10) unsigned NOT NULL DEFAULT 0,
  `RaidOffset` int(10) unsigned NOT NULL DEFAULT 0,
  `MaxPlayers` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `map_loot_disabled`
--

DROP TABLE IF EXISTS `map_loot_disabled`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `map_loot_disabled` (
  `map_id` int(11) NOT NULL DEFAULT 0,
  `comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`map_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `map_template`
--

DROP TABLE IF EXISTS `map_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `map_template` (
  `entry` smallint(5) unsigned NOT NULL,
  `parent` int(10) unsigned NOT NULL DEFAULT 0,
  `map_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `linked_zone` int(10) unsigned NOT NULL DEFAULT 0,
  `player_limit` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `reset_delay` int(10) unsigned NOT NULL DEFAULT 0,
  `time_offset` int(11) NOT NULL DEFAULT 0 COMMENT 'seconds',
  `ghost_entrance_map` smallint(6) NOT NULL DEFAULT -1,
  `ghost_entrance_x` float NOT NULL DEFAULT 0,
  `ghost_entrance_y` float NOT NULL DEFAULT 0,
  `map_name` varchar(128) NOT NULL DEFAULT '',
  `script_name` varchar(128) NOT NULL DEFAULT '',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mapdifficulty`
--

DROP TABLE IF EXISTS `mapdifficulty`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mapdifficulty` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Map` int(10) unsigned NOT NULL DEFAULT 0,
  `difficulty` int(11) NOT NULL DEFAULT 0,
  `message1` int(10) unsigned NOT NULL DEFAULT 0,
  `message2` int(10) unsigned NOT NULL DEFAULT 0,
  `message3` int(10) unsigned NOT NULL DEFAULT 0,
  `message4` int(10) unsigned NOT NULL DEFAULT 0,
  `message5` int(10) unsigned NOT NULL DEFAULT 0,
  `message6` int(10) unsigned NOT NULL DEFAULT 0,
  `message7` int(10) unsigned NOT NULL DEFAULT 0,
  `message8` int(10) unsigned NOT NULL DEFAULT 0,
  `message9` int(10) unsigned NOT NULL DEFAULT 0,
  `message10` int(10) unsigned NOT NULL DEFAULT 0,
  `message11` int(10) unsigned NOT NULL DEFAULT 0,
  `message12` int(10) unsigned NOT NULL DEFAULT 0,
  `message13` int(10) unsigned NOT NULL DEFAULT 0,
  `message14` int(10) unsigned NOT NULL DEFAULT 0,
  `message15` int(10) unsigned NOT NULL DEFAULT 0,
  `message16` int(10) unsigned NOT NULL DEFAULT 0,
  `messageFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `raidDurationSeconds` int(10) unsigned NOT NULL DEFAULT 0,
  `maxPlayers` int(10) unsigned NOT NULL DEFAULT 0,
  `difficultyString` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `migrations`
--

DROP TABLE IF EXISTS `migrations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `migrations` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Name` varchar(255) NOT NULL DEFAULT '0',
  `Hash` varchar(128) NOT NULL DEFAULT '0',
  `AppliedAt` datetime NOT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=627 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `miraclerace_checkpoint`
--

DROP TABLE IF EXISTS `miraclerace_checkpoint`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `miraclerace_checkpoint` (
  `id` int(10) unsigned NOT NULL,
  `raceid` int(10) unsigned NOT NULL,
  `positionx` float NOT NULL,
  `positiony` float NOT NULL,
  `positionz` float NOT NULL,
  `cameraposx` float NOT NULL,
  `cameraposy` float NOT NULL,
  `cameraposz` float NOT NULL,
  `cameraposorientation` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `miraclerace_creaturespool`
--

DROP TABLE IF EXISTS `miraclerace_creaturespool`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `miraclerace_creaturespool` (
  `raceid` int(10) unsigned NOT NULL,
  `entry` int(10) unsigned NOT NULL,
  `chance` int(10) unsigned NOT NULL,
  `positionx` float NOT NULL,
  `positiony` float NOT NULL,
  `positionz` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `miraclerace_gameobject`
--

DROP TABLE IF EXISTS `miraclerace_gameobject`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `miraclerace_gameobject` (
  `raceid` int(10) unsigned NOT NULL,
  `entry` int(10) unsigned NOT NULL,
  `chance` int(10) unsigned NOT NULL,
  `positionx` float NOT NULL,
  `positiony` float NOT NULL,
  `positionz` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `movie`
--

DROP TABLE IF EXISTS `movie`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `movie` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `MoviePath` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Volume` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `moviefiledata`
--

DROP TABLE IF EXISTS `moviefiledata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `moviefiledata` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Resolution` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `movievariation`
--

DROP TABLE IF EXISTS `movievariation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `movievariation` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `MovieID` int(10) unsigned NOT NULL DEFAULT 0,
  `FileDataID` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `npc_gossip`
--

DROP TABLE IF EXISTS `npc_gossip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_gossip` (
  `npc_guid` int(10) unsigned NOT NULL DEFAULT 0,
  `textid` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`npc_guid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `npc_text`
--

DROP TABLE IF EXISTS `npc_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_text` (
  `ID` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `BroadcastTextID0` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability0` float NOT NULL DEFAULT 0,
  `BroadcastTextID1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability1` float NOT NULL DEFAULT 0,
  `BroadcastTextID2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability2` float NOT NULL DEFAULT 0,
  `BroadcastTextID3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability3` float NOT NULL DEFAULT 0,
  `BroadcastTextID4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability4` float NOT NULL DEFAULT 0,
  `BroadcastTextID5` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability5` float NOT NULL DEFAULT 0,
  `BroadcastTextID6` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability6` float NOT NULL DEFAULT 0,
  `BroadcastTextID7` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Probability7` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `npc_trainer`
--

DROP TABLE IF EXISTS `npc_trainer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_trainer` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellcost` int(10) unsigned NOT NULL DEFAULT 0,
  `reqskill` smallint(5) unsigned NOT NULL DEFAULT 0,
  `reqskillvalue` smallint(5) unsigned NOT NULL DEFAULT 0,
  `reqlevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  UNIQUE KEY `entry_spell` (`entry`,`spell`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `npc_trainer_greeting`
--

DROP TABLE IF EXISTS `npc_trainer_greeting`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_trainer_greeting` (
  `entry` int(10) unsigned NOT NULL DEFAULT 0,
  `content_default` text NOT NULL,
  `content_loc1` text NOT NULL,
  `content_loc2` text NOT NULL,
  `content_loc3` text NOT NULL,
  `content_loc4` text NOT NULL,
  `content_loc5` text NOT NULL,
  `content_loc6` text NOT NULL,
  `content_loc7` text NOT NULL,
  `content_loc8` text NOT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `npc_trainer_template`
--

DROP TABLE IF EXISTS `npc_trainer_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_trainer_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spellcost` int(10) unsigned NOT NULL DEFAULT 0,
  `reqskill` smallint(5) unsigned NOT NULL DEFAULT 0,
  `reqskillvalue` smallint(5) unsigned NOT NULL DEFAULT 0,
  `reqlevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  UNIQUE KEY `entry_spell` (`entry`,`spell`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `npc_vendor`
--

DROP TABLE IF EXISTS `npc_vendor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_vendor` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `slot` smallint(5) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `incrtime` int(10) unsigned NOT NULL DEFAULT 0,
  `itemflags` int(10) unsigned NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Npc System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `npc_vendor_template`
--

DROP TABLE IF EXISTS `npc_vendor_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `npc_vendor_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `slot` smallint(5) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `incrtime` int(10) unsigned NOT NULL DEFAULT 0,
  `itemflags` int(10) unsigned NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Npc System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `object_scaling`
--

DROP TABLE IF EXISTS `object_scaling`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `object_scaling` (
  `fullGuid` bigint(20) unsigned NOT NULL,
  `scale` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`fullGuid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `overridespelldata`
--

DROP TABLE IF EXISTS `overridespelldata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `overridespelldata` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid1` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid2` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid3` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid4` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid5` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid6` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid7` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid8` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid9` int(10) unsigned NOT NULL DEFAULT 0,
  `spellid10` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `page_text`
--

DROP TABLE IF EXISTS `page_text`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `page_text` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `text` longtext NOT NULL,
  `next_page` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Item System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pet_levelstats`
--

DROP TABLE IF EXISTS `pet_levelstats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pet_levelstats` (
  `creature_entry` mediumint(8) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL,
  `hp` smallint(5) unsigned NOT NULL,
  `mana` smallint(5) unsigned NOT NULL,
  `armor` int(10) unsigned NOT NULL DEFAULT 0,
  `str` smallint(5) unsigned NOT NULL,
  `agi` smallint(5) unsigned NOT NULL,
  `sta` smallint(5) unsigned NOT NULL,
  `inte` smallint(5) unsigned NOT NULL,
  `spi` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`creature_entry`,`level`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci PACK_KEYS=0 ROW_FORMAT=FIXED COMMENT='Stores pet levels stats.';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pet_name_generation`
--

DROP TABLE IF EXISTS `pet_name_generation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pet_name_generation` (
  `id` mediumint(8) unsigned NOT NULL AUTO_INCREMENT,
  `word` tinytext NOT NULL,
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `half` tinyint(4) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=261 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pet_spell_data`
--

DROP TABLE IF EXISTS `pet_spell_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pet_spell_data` (
  `entry` int(10) unsigned NOT NULL,
  `spell_id1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spell_id2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spell_id3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `spell_id4` smallint(5) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `petcreateinfo_spell`
--

DROP TABLE IF EXISTS `petcreateinfo_spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `petcreateinfo_spell` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `spell4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Pet Create Spells';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pickpocketing_loot_template`
--

DROP TABLE IF EXISTS `pickpocketing_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pickpocketing_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_census`
--

DROP TABLE IF EXISTS `player_census`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_census` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `alliance_players` int(10) unsigned NOT NULL,
  `horde_players` int(10) unsigned NOT NULL,
  `total_players` int(10) unsigned NOT NULL,
  `date_time` datetime NOT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=9727 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_classlevelstats`
--

DROP TABLE IF EXISTS `player_classlevelstats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_classlevelstats` (
  `class` tinyint(3) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL,
  `basehp` smallint(5) unsigned NOT NULL,
  `basemana` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`class`,`level`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci PACK_KEYS=0 ROW_FORMAT=FIXED COMMENT='Stores levels stats.';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_factionchange_items`
--

DROP TABLE IF EXISTS `player_factionchange_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_factionchange_items` (
  `alliance_id` int(11) NOT NULL,
  `horde_id` int(11) NOT NULL,
  `comment` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`alliance_id`,`horde_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_factionchange_mounts`
--

DROP TABLE IF EXISTS `player_factionchange_mounts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_factionchange_mounts` (
  `RaceId` int(11) NOT NULL,
  `MountNum` int(11) NOT NULL,
  `ItemEntry` int(11) NOT NULL,
  `Comment` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`RaceId`,`MountNum`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_factionchange_quests`
--

DROP TABLE IF EXISTS `player_factionchange_quests`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_factionchange_quests` (
  `alliance_id` int(11) NOT NULL,
  `horde_id` int(11) NOT NULL,
  `comment` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`alliance_id`,`horde_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_factionchange_reputations`
--

DROP TABLE IF EXISTS `player_factionchange_reputations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_factionchange_reputations` (
  `alliance_id` int(11) NOT NULL,
  `horde_id` int(11) NOT NULL,
  PRIMARY KEY (`alliance_id`,`horde_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_factionchange_spells`
--

DROP TABLE IF EXISTS `player_factionchange_spells`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_factionchange_spells` (
  `alliance_id` int(11) NOT NULL,
  `horde_id` int(11) NOT NULL,
  `comment` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`alliance_id`,`horde_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_levelstats`
--

DROP TABLE IF EXISTS `player_levelstats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_levelstats` (
  `race` tinyint(3) unsigned NOT NULL,
  `class` tinyint(3) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL,
  `str` tinyint(3) unsigned NOT NULL,
  `agi` tinyint(3) unsigned NOT NULL,
  `sta` tinyint(3) unsigned NOT NULL,
  `inte` tinyint(3) unsigned NOT NULL,
  `spi` tinyint(3) unsigned NOT NULL,
  PRIMARY KEY (`race`,`class`,`level`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci PACK_KEYS=0 ROW_FORMAT=FIXED COMMENT='Stores levels stats.';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `player_xp_for_level`
--

DROP TABLE IF EXISTS `player_xp_for_level`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `player_xp_for_level` (
  `lvl` int(10) unsigned NOT NULL,
  `xp_for_next_level` int(10) unsigned NOT NULL,
  PRIMARY KEY (`lvl`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;


--
-- Table structure for table `playercreateinfo`
--

DROP TABLE IF EXISTS `playercreateinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `playercreateinfo` (
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `map` smallint(5) unsigned NOT NULL DEFAULT 0,
  `zone` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `position_x` float NOT NULL DEFAULT 0,
  `position_y` float NOT NULL DEFAULT 0,
  `position_z` float NOT NULL DEFAULT 0,
  `orientation` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`race`,`class`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `playercreateinfo_action`
--

DROP TABLE IF EXISTS `playercreateinfo_action`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `playercreateinfo_action` (
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `button` smallint(5) unsigned NOT NULL DEFAULT 0,
  `action` int(10) unsigned NOT NULL DEFAULT 0,
  `type` smallint(5) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`race`,`class`,`button`) USING BTREE,
  KEY `playercreateinfo_race_class_index` (`race`,`class`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `playercreateinfo_item`
--

DROP TABLE IF EXISTS `playercreateinfo_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `playercreateinfo_item` (
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `itemid` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `amount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  KEY `playercreateinfo_race_class_index` (`race`,`class`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `playercreateinfo_spell`
--

DROP TABLE IF EXISTS `playercreateinfo_spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `playercreateinfo_spell` (
  `race` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `class` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `spell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `note` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`race`,`class`,`spell`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `points_of_interest`
--

DROP TABLE IF EXISTS `points_of_interest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `points_of_interest` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `icon` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `flags` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `data` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `icon_name` text NOT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pool_creature`
--

DROP TABLE IF EXISTS `pool_creature`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pool_creature` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `pool_entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `chance` float unsigned NOT NULL DEFAULT 0,
  `description` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'FLAG_SPAWN_ENABLE_IF_WORLD_POP_OVER_BLIZZLIKE = 1',
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `pool_idx` (`pool_entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pool_creature_template`
--

DROP TABLE IF EXISTS `pool_creature_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pool_creature_template` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `pool_entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `chance` float unsigned NOT NULL DEFAULT 0,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'FLAG_SPAWN_ENABLE_IF_WORLD_POP_OVER_BLIZZLIKE = 1',
  PRIMARY KEY (`id`) USING BTREE,
  KEY `pool_idx` (`pool_entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pool_gameobject`
--

DROP TABLE IF EXISTS `pool_gameobject`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pool_gameobject` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `pool_entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `chance` float unsigned NOT NULL DEFAULT 0,
  `description` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'FLAG_SPAWN_ENABLE_IF_WORLD_POP_OVER_BLIZZLIKE = 1',
  PRIMARY KEY (`guid`) USING BTREE,
  KEY `pool_idx` (`pool_entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pool_gameobject_template`
--

DROP TABLE IF EXISTS `pool_gameobject_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pool_gameobject_template` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `pool_entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `chance` float unsigned NOT NULL DEFAULT 0,
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'FLAG_SPAWN_ENABLE_IF_WORLD_POP_OVER_BLIZZLIKE = 1',
  PRIMARY KEY (`id`) USING BTREE,
  KEY `pool_idx` (`pool_entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pool_pool`
--

DROP TABLE IF EXISTS `pool_pool`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pool_pool` (
  `pool_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `mother_pool` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `chance` float NOT NULL DEFAULT 0,
  `description` varchar(255) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'FLAG_SPAWN_ENABLE_IF_WORLD_POP_OVER_BLIZZLIKE = 1',
  PRIMARY KEY (`pool_id`) USING BTREE,
  KEY `pool_idx` (`mother_pool`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pool_template`
--

DROP TABLE IF EXISTS `pool_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pool_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Pool entry',
  `max_limit` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Max number of objects (0) is no limit',
  `description` varchar(255) NOT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `instance` mediumint(9) NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `quest_cast_objective`
--

DROP TABLE IF EXISTS `quest_cast_objective`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `quest_cast_objective` (
  `entry` mediumint(8) unsigned NOT NULL COMMENT 'quest id',
  `idx` tinyint(3) unsigned NOT NULL COMMENT 'objective index (0 to 3)',
  `spell_id` int(10) unsigned NOT NULL,
  `player_guid` int(11) NOT NULL COMMENT 'low guid, 0 for any player, -1 for gm',
  `player_class` tinyint(4) NOT NULL DEFAULT 0 COMMENT '0 for any class',
  `objective_text` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`entry`,`idx`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `quest_end_scripts`
--

DROP TABLE IF EXISTS `quest_end_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `quest_end_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `quest_greeting`
--

DROP TABLE IF EXISTS `quest_greeting`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `quest_greeting` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `content_default` text NOT NULL,
  `content_loc1` text DEFAULT NULL,
  `content_loc2` text DEFAULT NULL,
  `content_loc3` text DEFAULT NULL,
  `content_loc4` text DEFAULT NULL,
  `content_loc5` text DEFAULT NULL,
  `content_loc6` text DEFAULT NULL,
  `content_loc7` text DEFAULT NULL,
  `content_loc8` text DEFAULT NULL,
  `emote_id` smallint(5) unsigned NOT NULL DEFAULT 0,
  `emote_delay` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`type`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `quest_start_scripts`
--

DROP TABLE IF EXISTS `quest_start_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `quest_start_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `quest_template`
--

DROP TABLE IF EXISTS `quest_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `quest_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Method` tinyint(3) unsigned NOT NULL DEFAULT 2,
  `ZoneOrSort` smallint(6) NOT NULL DEFAULT 0,
  `MinLevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `MaxLevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `QuestLevel` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `Type` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RequiredClasses` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RequiredRaces` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RequiredSkill` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RequiredSkillValue` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RequiredCondition` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RepObjectiveFaction` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RepObjectiveValue` mediumint(9) NOT NULL DEFAULT 0,
  `RequiredMinRepFaction` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RequiredMinRepValue` mediumint(9) NOT NULL DEFAULT 0,
  `RequiredMaxRepFaction` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RequiredMaxRepValue` mediumint(9) NOT NULL DEFAULT 0,
  `SuggestedPlayers` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `LimitTime` int(10) unsigned NOT NULL DEFAULT 0,
  `QuestFlags` smallint(5) unsigned NOT NULL DEFAULT 0,
  `SpecialFlags` smallint(5) unsigned NOT NULL DEFAULT 0,
  `PrevQuestId` mediumint(9) NOT NULL DEFAULT 0,
  `NextQuestId` mediumint(9) NOT NULL DEFAULT 0,
  `ExclusiveGroup` mediumint(9) NOT NULL DEFAULT 0,
  `NextQuestInChain` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `SrcItemId` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `SrcItemCount` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `SrcSpell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `Title` text DEFAULT NULL,
  `Details` text DEFAULT NULL,
  `Objectives` text DEFAULT NULL,
  `OfferRewardText` text DEFAULT NULL,
  `RequestItemsText` text DEFAULT NULL,
  `EndText` text DEFAULT NULL,
  `ObjectiveText1` text DEFAULT NULL,
  `ObjectiveText2` text DEFAULT NULL,
  `ObjectiveText3` text DEFAULT NULL,
  `ObjectiveText4` text DEFAULT NULL,
  `ReqItemId1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqItemId2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqItemId3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqItemId4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqItemCount1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqItemCount2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqItemCount3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqItemCount4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqSourceId1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSourceId2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSourceId3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSourceId4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSourceCount1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSourceCount2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSourceCount3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSourceCount4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqCreatureOrGOId1` mediumint(9) NOT NULL DEFAULT 0,
  `ReqCreatureOrGOId2` mediumint(9) NOT NULL DEFAULT 0,
  `ReqCreatureOrGOId3` mediumint(9) NOT NULL DEFAULT 0,
  `ReqCreatureOrGOId4` mediumint(9) NOT NULL DEFAULT 0,
  `ReqCreatureOrGOCount1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqCreatureOrGOCount2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqCreatureOrGOCount3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqCreatureOrGOCount4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `ReqSpellCast1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSpellCast2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSpellCast3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ReqSpellCast4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemId1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemId2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemId3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemId4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemId5` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemId6` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemCount1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemCount2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemCount3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemCount4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemCount5` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewChoiceItemCount6` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewItemId1` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewItemId2` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewItemId3` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewItemId4` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewItemCount1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewItemCount2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewItemCount3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewItemCount4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `RewRepFaction1` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction2` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction3` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction4` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'faction id from Faction.dbc in this case',
  `RewRepFaction5` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'faction id from Faction.dbc in this case',
  `RewRepValue1` mediumint(9) NOT NULL DEFAULT 0,
  `RewRepValue2` mediumint(9) NOT NULL DEFAULT 0,
  `RewRepValue3` mediumint(9) NOT NULL DEFAULT 0,
  `RewRepValue4` mediumint(9) NOT NULL DEFAULT 0,
  `RewRepValue5` mediumint(9) NOT NULL DEFAULT 0,
  `RewXP` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewOrReqMoney` int(11) NOT NULL DEFAULT 0,
  `RewMoneyMaxLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `RewSpell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewSpellCast` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `RewMailTemplateId` mediumint(9) NOT NULL DEFAULT 0,
  `RewMailDelaySecs` int(10) unsigned NOT NULL DEFAULT 0,
  `RewMailMoney` int(10) unsigned NOT NULL DEFAULT 0,
  `PointMapId` smallint(5) unsigned NOT NULL DEFAULT 0,
  `PointX` float NOT NULL DEFAULT 0,
  `PointY` float NOT NULL DEFAULT 0,
  `PointOpt` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `DetailsEmote1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `DetailsEmote2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `DetailsEmote3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `DetailsEmote4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `DetailsEmoteDelay1` int(10) unsigned NOT NULL DEFAULT 0,
  `DetailsEmoteDelay2` int(10) unsigned NOT NULL DEFAULT 0,
  `DetailsEmoteDelay3` int(10) unsigned NOT NULL DEFAULT 0,
  `DetailsEmoteDelay4` int(10) unsigned NOT NULL DEFAULT 0,
  `IncompleteEmote` smallint(5) unsigned NOT NULL DEFAULT 0,
  `CompleteEmote` smallint(5) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmote1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmote2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmote3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmote4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmoteDelay1` int(10) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmoteDelay2` int(10) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmoteDelay3` int(10) unsigned NOT NULL DEFAULT 0,
  `OfferRewardEmoteDelay4` int(10) unsigned NOT NULL DEFAULT 0,
  `StartScript` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `CompleteScript` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Quest System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `questsort`
--

DROP TABLE IF EXISTS `questsort`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `questsort` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `reference_loot_template`
--

DROP TABLE IF EXISTS `reference_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `reference_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `reputation_reward_rate`
--

DROP TABLE IF EXISTS `reputation_reward_rate`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `reputation_reward_rate` (
  `faction` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `quest_rate` float NOT NULL DEFAULT 1,
  `creature_rate` float NOT NULL DEFAULT 1,
  `spell_rate` float NOT NULL DEFAULT 1,
  PRIMARY KEY (`faction`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `reputation_spillover_template`
--

DROP TABLE IF EXISTS `reputation_spillover_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `reputation_spillover_template` (
  `faction` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'faction entry',
  `faction1` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'faction to give spillover for',
  `rate_1` float NOT NULL DEFAULT 0 COMMENT 'the given rep points * rate',
  `rank_1` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT 'max rank, above this will not give any spillover',
  `faction2` smallint(5) unsigned NOT NULL DEFAULT 0,
  `rate_2` float NOT NULL DEFAULT 0,
  `rank_2` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `faction3` smallint(5) unsigned NOT NULL DEFAULT 0,
  `rate_3` float NOT NULL DEFAULT 0,
  `rank_3` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `faction4` smallint(5) unsigned NOT NULL DEFAULT 0,
  `rate_4` float NOT NULL DEFAULT 0,
  `rank_4` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`faction`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Reputation spillover reputation gain';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `reserved_name`
--

DROP TABLE IF EXISTS `reserved_name`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `reserved_name` (
  `name` varchar(12) NOT NULL DEFAULT '',
  PRIMARY KEY (`name`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Player Reserved Names';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `screeneffect`
--

DROP TABLE IF EXISTS `screeneffect`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `screeneffect` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `type` int(10) unsigned NOT NULL DEFAULT 0,
  `colour` int(10) unsigned NOT NULL DEFAULT 0,
  `screenEdgeSize` int(10) unsigned NOT NULL DEFAULT 0,
  `blackWhiteValue` int(10) unsigned NOT NULL DEFAULT 0,
  `unknown` int(10) unsigned NOT NULL DEFAULT 0,
  `lightId` int(11) NOT NULL DEFAULT 0,
  `soundAmbienceID` int(10) unsigned NOT NULL DEFAULT 0,
  `soundMusicId` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `script_escort_data`
--

DROP TABLE IF EXISTS `script_escort_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `script_escort_data` (
  `creature_id` int(11) DEFAULT NULL,
  `quest` int(11) DEFAULT NULL,
  `escort_faction` int(11) DEFAULT NULL,
  UNIQUE KEY `creature_id` (`creature_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `script_texts`
--

DROP TABLE IF EXISTS `script_texts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `script_texts` (
  `entry` mediumint(9) NOT NULL,
  `content_default` text NOT NULL,
  `content_loc1` text DEFAULT NULL,
  `content_loc2` text DEFAULT NULL,
  `content_loc3` text DEFAULT NULL,
  `content_loc4` text DEFAULT NULL,
  `content_loc5` text DEFAULT NULL,
  `content_loc6` text DEFAULT NULL,
  `content_loc7` text DEFAULT NULL,
  `content_loc8` text DEFAULT NULL,
  `sound` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `language` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `emote` smallint(5) unsigned NOT NULL DEFAULT 0,
  `comment` text DEFAULT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Script Texts';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `script_waypoint`
--

DROP TABLE IF EXISTS `script_waypoint`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `script_waypoint` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'creature_template entry',
  `pointid` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `location_x` float NOT NULL DEFAULT 0,
  `location_y` float NOT NULL DEFAULT 0,
  `location_z` float NOT NULL DEFAULT 0,
  `waittime` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'waittime in millisecs',
  `point_comment` text DEFAULT NULL,
  PRIMARY KEY (`entry`,`pointid`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Script Creature waypoints';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `scripted_areatrigger`
--

DROP TABLE IF EXISTS `scripted_areatrigger`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `scripted_areatrigger` (
  `entry` mediumint(9) NOT NULL,
  `script_name` char(64) NOT NULL,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `scripted_event_id`
--

DROP TABLE IF EXISTS `scripted_event_id`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `scripted_event_id` (
  `id` mediumint(9) NOT NULL,
  `script_name` char(64) NOT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Script library scripted events';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `shop_categories`
--

DROP TABLE IF EXISTS `shop_categories`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `shop_categories` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` text DEFAULT NULL,
  `name_loc4` text DEFAULT NULL,
  `icon` text DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `shop_items`
--

DROP TABLE IF EXISTS `shop_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `shop_items` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `category` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `item` int(11) unsigned NOT NULL DEFAULT 0,
  `model_id` int(10) unsigned NOT NULL DEFAULT 0,
  `item_id` int(10) unsigned NOT NULL DEFAULT 0,
  `description` text DEFAULT NULL,
  `description_loc4` text DEFAULT NULL,
  `price` int(11) unsigned DEFAULT 0,
  `region_locked` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `position_x` float DEFAULT 0,
  `position_y` float DEFAULT 0,
  `position_z` float DEFAULT 0,
  `rotation` float DEFAULT 0,
  `scale` float DEFAULT 1,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=457 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `skill_fishing_base_level`
--

DROP TABLE IF EXISTS `skill_fishing_base_level`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `skill_fishing_base_level` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Area identifier',
  `skill` smallint(6) NOT NULL DEFAULT 0 COMMENT 'Base skill level requirement',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Fishing system';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `skill_line_ability`
--

DROP TABLE IF EXISTS `skill_line_ability`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `skill_line_ability` (
  `id` smallint(5) unsigned NOT NULL,
  `skill_id` int(10) unsigned NOT NULL DEFAULT 0,
  `spell_id` int(10) unsigned NOT NULL DEFAULT 0,
  `race_mask` int(10) unsigned NOT NULL DEFAULT 0,
  `class_mask` int(10) unsigned NOT NULL DEFAULT 0,
  `req_skill_value` int(10) unsigned NOT NULL DEFAULT 0,
  `superseded_by_spell` int(10) unsigned NOT NULL DEFAULT 0,
  `learn_on_get_skill` int(10) unsigned NOT NULL DEFAULT 0,
  `max_value` int(10) unsigned NOT NULL DEFAULT 0,
  `min_value` int(10) unsigned NOT NULL DEFAULT 0,
  `req_train_points` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `skillline`
--

DROP TABLE IF EXISTS `skillline`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `skillline` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `category` int(10) unsigned NOT NULL DEFAULT 0,
  `costId` int(10) unsigned NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `description1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `description16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `descriptionFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `spellIcon` int(10) unsigned NOT NULL DEFAULT 0,
  `tooltip1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltipFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `canLink` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `skilllineability`
--

DROP TABLE IF EXISTS `skilllineability`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `skilllineability` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `skillId` int(10) unsigned NOT NULL DEFAULT 0,
  `spellId` int(10) unsigned NOT NULL DEFAULT 0,
  `chrRaces` int(10) unsigned NOT NULL DEFAULT 0,
  `chrClasses` int(10) unsigned NOT NULL DEFAULT 0,
  `unk1` int(10) unsigned NOT NULL DEFAULT 0,
  `unk2` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredSkillValue` int(10) unsigned NOT NULL DEFAULT 0,
  `spellIdParent` int(10) unsigned NOT NULL DEFAULT 0,
  `acquireMethod` int(10) unsigned NOT NULL DEFAULT 0,
  `skillGreyLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `skillGreenLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `characterPoints1` int(10) unsigned NOT NULL DEFAULT 0,
  `characterPoints2` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `skillraceclassinfo`
--

DROP TABLE IF EXISTS `skillraceclassinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `skillraceclassinfo` (
  `Id` int(10) unsigned NOT NULL DEFAULT 0,
  `SkillLineDbcRecord` int(10) unsigned NOT NULL DEFAULT 0,
  `RaceMask` int(10) unsigned NOT NULL DEFAULT 0,
  `ClassMask` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `MinLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `SkillTierId` int(10) unsigned NOT NULL DEFAULT 0,
  `SkillCostIndex` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;

--
-- Table structure for table `skill_race_class_info_mod`
--

DROP TABLE IF EXISTS `skill_race_class_info_mod`;

CREATE TABLE `skill_race_class_info_mod` (
  `Id` int(10) unsigned NOT NULL DEFAULT 0,
  `SkillLineDbcRecord` int(11) NOT NULL DEFAULT -1,
  `RaceMask` int(11) NOT NULL DEFAULT -1,
  `ClassMask` int(11) NOT NULL DEFAULT -1,
  `Flags` int(11) NOT NULL DEFAULT -1,
  `MinLevel` int(11) NOT NULL DEFAULT -1,
  `SkillTierId` int(11) NOT NULL DEFAULT -1,
  `SkillCostIndex` int(11) NOT NULL DEFAULT -1,
  `Comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `skinning_loot_template`
--

DROP TABLE IF EXISTS `skinning_loot_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `skinning_loot_template` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `item` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `ChanceOrQuestChance` float NOT NULL DEFAULT 100,
  `groupid` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `mincountOrRef` mediumint(9) NOT NULL DEFAULT 1,
  `maxcount` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`item`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Loot System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sound_entries`
--

DROP TABLE IF EXISTS `sound_entries`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `sound_entries` (
  `id` int(11) NOT NULL DEFAULT 0,
  `name` varchar(128) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE KEY `ID` (`id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `soundentries`
--

DROP TABLE IF EXISTS `soundentries`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `soundentries` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundType` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Freq1` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq2` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq3` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq4` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq5` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq6` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq7` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq8` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq9` int(10) unsigned NOT NULL DEFAULT 0,
  `Freq10` int(10) unsigned NOT NULL DEFAULT 0,
  `FilePath` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Volume` float NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `MinDistance` float NOT NULL DEFAULT 0,
  `MaxDistance` float NOT NULL DEFAULT 0,
  `EAXDef` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundEntriesAdvancedID` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell`
--

DROP TABLE IF EXISTS `spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Category` int(10) unsigned NOT NULL DEFAULT 0,
  `Dispel` int(10) unsigned NOT NULL DEFAULT 0,
  `Mechanic` int(10) unsigned NOT NULL DEFAULT 0,
  `Attributes` int(10) unsigned NOT NULL DEFAULT 0,
  `AttributesEx` int(10) unsigned NOT NULL DEFAULT 0,
  `AttributesEx2` int(10) unsigned NOT NULL DEFAULT 0,
  `AttributesEx3` int(10) unsigned NOT NULL DEFAULT 0,
  `AttributesEx4` int(10) unsigned NOT NULL DEFAULT 0,
  `AttributesEx5` int(10) unsigned NOT NULL DEFAULT 0,
  `AttributesEx6` int(10) unsigned NOT NULL DEFAULT 0,
  `AttributesEx7` int(10) unsigned NOT NULL DEFAULT 0,
  `Stances` int(10) unsigned NOT NULL DEFAULT 0,
  `Unknown1` int(10) unsigned NOT NULL DEFAULT 0,
  `StancesNot` int(10) unsigned NOT NULL DEFAULT 0,
  `Unknown2` int(10) unsigned NOT NULL DEFAULT 0,
  `Targets` int(10) unsigned NOT NULL DEFAULT 0,
  `TargetCreatureType` int(10) unsigned NOT NULL DEFAULT 0,
  `RequiresSpellFocus` int(10) unsigned NOT NULL DEFAULT 0,
  `FacingCasterFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `CasterAuraState` int(10) unsigned NOT NULL DEFAULT 0,
  `TargetAuraState` int(10) unsigned NOT NULL DEFAULT 0,
  `CasterAuraStateNot` int(10) unsigned NOT NULL DEFAULT 0,
  `TargetAuraStateNot` int(10) unsigned NOT NULL DEFAULT 0,
  `CasterAuraSpell` int(10) unsigned NOT NULL DEFAULT 0,
  `TargetAuraSpell` int(10) unsigned NOT NULL DEFAULT 0,
  `ExcludeCasterAuraSpell` int(10) unsigned NOT NULL DEFAULT 0,
  `ExcludeTargetAuraSpell` int(10) unsigned NOT NULL DEFAULT 0,
  `CastingTimeIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `RecoveryTime` int(10) unsigned NOT NULL DEFAULT 0,
  `CategoryRecoveryTime` int(10) unsigned NOT NULL DEFAULT 0,
  `InterruptFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `AuraInterruptFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `ChannelInterruptFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `ProcFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `ProcChance` int(10) unsigned NOT NULL DEFAULT 0,
  `ProcCharges` int(10) unsigned NOT NULL DEFAULT 0,
  `MaximumLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `BaseLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `DurationIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `PowerType` int(10) unsigned NOT NULL DEFAULT 0,
  `ManaCost` int(10) unsigned NOT NULL DEFAULT 0,
  `ManaCostPerLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `ManaPerSecond` int(10) unsigned NOT NULL DEFAULT 0,
  `ManaPerSecondPerLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `RangeIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `Speed` float NOT NULL DEFAULT 0,
  `ModalNextSpell` int(10) unsigned NOT NULL DEFAULT 0,
  `StackAmount` int(10) unsigned NOT NULL DEFAULT 0,
  `Totem1` int(10) unsigned NOT NULL DEFAULT 0,
  `Totem2` int(10) unsigned NOT NULL DEFAULT 0,
  `Reagent1` int(11) NOT NULL DEFAULT 0,
  `Reagent2` int(11) NOT NULL DEFAULT 0,
  `Reagent3` int(11) NOT NULL DEFAULT 0,
  `Reagent4` int(11) NOT NULL DEFAULT 0,
  `Reagent5` int(11) NOT NULL DEFAULT 0,
  `Reagent6` int(11) NOT NULL DEFAULT 0,
  `Reagent7` int(11) NOT NULL DEFAULT 0,
  `Reagent8` int(11) NOT NULL DEFAULT 0,
  `ReagentCount1` int(10) unsigned NOT NULL DEFAULT 0,
  `ReagentCount2` int(10) unsigned NOT NULL DEFAULT 0,
  `ReagentCount3` int(10) unsigned NOT NULL DEFAULT 0,
  `ReagentCount4` int(10) unsigned NOT NULL DEFAULT 0,
  `ReagentCount5` int(10) unsigned NOT NULL DEFAULT 0,
  `ReagentCount6` int(10) unsigned NOT NULL DEFAULT 0,
  `ReagentCount7` int(10) unsigned NOT NULL DEFAULT 0,
  `ReagentCount8` int(10) unsigned NOT NULL DEFAULT 0,
  `EquippedItemClass` int(11) NOT NULL DEFAULT 0,
  `EquippedItemSubClassMask` int(11) NOT NULL DEFAULT 0,
  `EquippedItemInventoryTypeMask` int(11) NOT NULL DEFAULT 0,
  `Effect1` int(10) unsigned NOT NULL DEFAULT 0,
  `Effect2` int(10) unsigned NOT NULL DEFAULT 0,
  `Effect3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectDieSides1` int(11) NOT NULL DEFAULT 0,
  `EffectDieSides2` int(11) NOT NULL DEFAULT 0,
  `EffectDieSides3` int(11) NOT NULL DEFAULT 0,
  `EffectRealPointsPerLevel1` float NOT NULL DEFAULT 0,
  `EffectRealPointsPerLevel2` float NOT NULL DEFAULT 0,
  `EffectRealPointsPerLevel3` float NOT NULL DEFAULT 0,
  `EffectBasePoints1` int(11) NOT NULL DEFAULT 0,
  `EffectBasePoints2` int(11) NOT NULL DEFAULT 0,
  `EffectBasePoints3` int(11) NOT NULL DEFAULT 0,
  `EffectMechanic1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectMechanic2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectMechanic3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectImplicitTargetA1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectImplicitTargetA2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectImplicitTargetA3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectImplicitTargetB1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectImplicitTargetB2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectImplicitTargetB3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectRadiusIndex1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectRadiusIndex2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectRadiusIndex3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectApplyAuraName1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectApplyAuraName2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectApplyAuraName3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectAmplitude1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectAmplitude2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectAmplitude3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectMultipleValue1` float NOT NULL DEFAULT 0,
  `EffectMultipleValue2` float NOT NULL DEFAULT 0,
  `EffectMultipleValue3` float NOT NULL DEFAULT 0,
  `EffectChainTarget1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectChainTarget2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectChainTarget3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectItemType1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectItemType2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectItemType3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectMiscValue1` int(11) NOT NULL DEFAULT 0,
  `EffectMiscValue2` int(11) NOT NULL DEFAULT 0,
  `EffectMiscValue3` int(11) NOT NULL DEFAULT 0,
  `EffectMiscValueB1` int(11) NOT NULL DEFAULT 0,
  `EffectMiscValueB2` int(11) NOT NULL DEFAULT 0,
  `EffectMiscValueB3` int(11) NOT NULL DEFAULT 0,
  `EffectTriggerSpell1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectTriggerSpell2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectTriggerSpell3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectPointsPerComboPoint1` float NOT NULL DEFAULT 0,
  `EffectPointsPerComboPoint2` float NOT NULL DEFAULT 0,
  `EffectPointsPerComboPoint3` float NOT NULL DEFAULT 0,
  `EffectSpellClassMaskA1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskA2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskA3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskB1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskB2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskB3` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskC1` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskC2` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectSpellClassMaskC3` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellVisual1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellVisual2` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellIconID` int(10) unsigned NOT NULL DEFAULT 0,
  `ActiveIconID` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellPriority` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellName0` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellName8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellNameFlag0` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellNameFlag1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellNameFlag2` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellNameFlag3` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellNameFlag4` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellNameFlag5` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellNameFlag6` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellNameFlag7` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRank0` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRank8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellRankFlags0` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRankFlags1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRankFlags2` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRankFlags3` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRankFlags4` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRankFlags5` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRankFlags6` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellRankFlags7` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescription0` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescription8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellDescriptionFlags0` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescriptionFlags1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescriptionFlags2` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescriptionFlags3` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescriptionFlags4` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescriptionFlags5` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescriptionFlags6` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDescriptionFlags7` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTip0` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTip8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `SpellToolTipFlags0` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTipFlags1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTipFlags2` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTipFlags3` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTipFlags4` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTipFlags5` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTipFlags6` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellToolTipFlags7` int(10) unsigned NOT NULL DEFAULT 0,
  `ManaCostPercentage` int(10) unsigned NOT NULL DEFAULT 0,
  `StartRecoveryCategory` int(10) unsigned NOT NULL DEFAULT 0,
  `StartRecoveryTime` int(10) unsigned NOT NULL DEFAULT 0,
  `MaximumTargetLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyName` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyFlags1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyFlags2` int(10) unsigned NOT NULL DEFAULT 0,
  `MaximumAffectedTargets` int(10) unsigned NOT NULL DEFAULT 0,
  `DamageClass` int(10) unsigned NOT NULL DEFAULT 0,
  `PreventionType` int(10) unsigned NOT NULL DEFAULT 0,
  `StanceBarOrder` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectDamageMultiplier1` float NOT NULL DEFAULT 0,
  `EffectDamageMultiplier2` float NOT NULL DEFAULT 0,
  `EffectDamageMultiplier3` float NOT NULL DEFAULT 0,
  `MinimumFactionId` int(10) unsigned NOT NULL DEFAULT 0,
  `MinimumReputation` int(10) unsigned NOT NULL DEFAULT 0,
  `RequiredAuraVision` int(10) unsigned NOT NULL DEFAULT 0,
  `TotemCategory1` int(10) unsigned NOT NULL DEFAULT 0,
  `TotemCategory2` int(10) unsigned NOT NULL DEFAULT 0,
  `AreaGroupID` int(10) unsigned NOT NULL DEFAULT 0,
  `SchoolMask` int(10) unsigned NOT NULL DEFAULT 0,
  `RuneCostID` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellMissileID` int(10) unsigned NOT NULL DEFAULT 0,
  `PowerDisplayId` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectBonusMultiplier1` float NOT NULL DEFAULT 0,
  `EffectBonusMultiplier2` float NOT NULL DEFAULT 0,
  `EffectBonusMultiplier3` float NOT NULL DEFAULT 0,
  `SpellDescriptionVariableID` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDifficultyID` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_affect`
--

DROP TABLE IF EXISTS `spell_affect`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_affect` (
  `entry` smallint(5) unsigned NOT NULL DEFAULT 0,
  `effectId` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyMask` bigint(20) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`,`effectId`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_area`
--

DROP TABLE IF EXISTS `spell_area`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_area` (
  `spell` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `area` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `quest_start` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `quest_start_active` tinyint(1) unsigned NOT NULL DEFAULT 0,
  `quest_end` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `aura_spell` mediumint(9) NOT NULL DEFAULT 0,
  `racemask` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `gender` tinyint(1) unsigned NOT NULL DEFAULT 2,
  `autocast` tinyint(1) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`spell`,`area`,`quest_start`,`quest_start_active`,`aura_spell`,`racemask`,`gender`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_chain`
--

DROP TABLE IF EXISTS `spell_chain`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_chain` (
  `spell_id` mediumint(9) NOT NULL DEFAULT 0,
  `prev_spell` mediumint(9) NOT NULL DEFAULT 0,
  `first_spell` mediumint(9) NOT NULL DEFAULT 0,
  `rank` tinyint(4) NOT NULL DEFAULT 0,
  `req_spell` mediumint(9) NOT NULL DEFAULT 0,
  PRIMARY KEY (`spell_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Spell Additinal Data';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_check`
--

DROP TABLE IF EXISTS `spell_check`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_check` (
  `spellid` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyName` smallint(6) NOT NULL DEFAULT -1,
  `SpellFamilyMask` bigint(20) NOT NULL DEFAULT -1,
  `SpellIcon` int(11) NOT NULL DEFAULT -1,
  `SpellVisual` int(11) NOT NULL DEFAULT -1,
  `SpellCategory` int(11) NOT NULL DEFAULT -1,
  `EffectType` int(11) NOT NULL DEFAULT -1,
  `EffectAura` int(11) NOT NULL DEFAULT -1,
  `EffectIdx` tinyint(4) NOT NULL DEFAULT -1,
  `Name` varchar(40) NOT NULL DEFAULT '',
  `Code` varchar(40) NOT NULL DEFAULT '',
  PRIMARY KEY (`spellid`,`SpellFamilyName`,`SpellFamilyMask`,`SpellIcon`,`SpellVisual`,`SpellCategory`,`Code`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_disabled`
--

DROP TABLE IF EXISTS `spell_disabled`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_disabled` (
  `entry` int(10) unsigned NOT NULL COMMENT 'Disabled spell',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_effect_mod`
--

DROP TABLE IF EXISTS `spell_effect_mod`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_effect_mod` (
  `Id` int(10) unsigned NOT NULL DEFAULT 0,
  `EffectIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `Effect` int(11) NOT NULL DEFAULT -1,
  `EffectDieSides` int(11) NOT NULL DEFAULT -1,
  `EffectBaseDice` int(11) NOT NULL DEFAULT -1,
  `EffectDicePerLevel` float NOT NULL DEFAULT -1,
  `EffectRealPointsPerLevel` float NOT NULL DEFAULT -1,
  `EffectBasePoints` int(11) NOT NULL DEFAULT -1,
  `EffectAmplitude` int(11) NOT NULL DEFAULT -1,
  `EffectPointsPerComboPoint` float NOT NULL DEFAULT -1,
  `EffectChainTarget` int(11) NOT NULL DEFAULT -1,
  `EffectMultipleValue` float NOT NULL DEFAULT -1,
  `EffectMechanic` int(11) NOT NULL DEFAULT -1,
  `EffectImplicitTargetA` int(11) NOT NULL DEFAULT -1,
  `EffectImplicitTargetB` int(11) NOT NULL DEFAULT -1,
  `EffectRadiusIndex` int(11) NOT NULL DEFAULT -1,
  `EffectApplyAuraName` int(11) NOT NULL DEFAULT -1,
  `EffectItemType` int(11) NOT NULL DEFAULT -1,
  `EffectMiscValue` int(11) NOT NULL DEFAULT -1,
  `EffectTriggerSpell` int(11) NOT NULL DEFAULT -1,
  `Comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`Id`,`EffectIndex`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_elixir`
--

DROP TABLE IF EXISTS `spell_elixir`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_elixir` (
  `entry` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellId of potion',
  `mask` tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT 'Mask 0x1 battle 0x2 guardian 0x3 flask 0x7 unstable flasks 0xB shattrath flasks',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Spell System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_enchant_charges`
--

DROP TABLE IF EXISTS `spell_enchant_charges`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_enchant_charges` (
  `entry` int(10) unsigned NOT NULL,
  `charges` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_group`
--

DROP TABLE IF EXISTS `spell_group`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_group` (
  `group_id` int(10) unsigned NOT NULL DEFAULT 0,
  `group_spell_id` int(10) unsigned NOT NULL DEFAULT 0,
  `spell_id` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`group_id`,`group_spell_id`,`spell_id`) USING BTREE,
  UNIQUE KEY `group_id` (`group_id`,`group_spell_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Spell System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_group_stack_rules`
--

DROP TABLE IF EXISTS `spell_group_stack_rules`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_group_stack_rules` (
  `group_id` int(10) unsigned NOT NULL DEFAULT 0,
  `stack_rule` tinyint(4) NOT NULL DEFAULT 1,
  PRIMARY KEY (`group_id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_learn_spell`
--

DROP TABLE IF EXISTS `spell_learn_spell`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_learn_spell` (
  `entry` smallint(5) unsigned NOT NULL DEFAULT 0,
  `SpellID` smallint(5) unsigned NOT NULL DEFAULT 0,
  `Active` tinyint(3) unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`entry`,`SpellID`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Item System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_mod`
--

DROP TABLE IF EXISTS `spell_mod`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_mod` (
  `Id` int(11) NOT NULL DEFAULT 0,
  `procChance` int(11) DEFAULT -1,
  `procFlags` int(11) DEFAULT -1,
  `procCharges` int(11) DEFAULT -1,
  `DurationIndex` int(11) DEFAULT -1,
  `Category` int(11) DEFAULT -1,
  `CastingTimeIndex` int(11) DEFAULT -1,
  `StackAmount` int(11) DEFAULT -1,
  `SpellIconID` int(11) DEFAULT -1,
  `activeIconID` int(11) DEFAULT -1,
  `manaCost` int(11) DEFAULT -1,
  `Attributes` int(11) DEFAULT -1,
  `AttributesEx` int(11) DEFAULT -1,
  `AttributesEx2` int(11) DEFAULT -1,
  `AttributesEx3` int(11) DEFAULT -1,
  `AttributesEx4` int(11) DEFAULT -1,
  `Custom` int(11) DEFAULT 0,
  `InterruptFlags` int(11) DEFAULT -1,
  `AuraInterruptFlags` int(11) DEFAULT -1,
  `ChannelInterruptFlags` int(11) DEFAULT -1,
  `Dispel` int(11) NOT NULL DEFAULT -1,
  `Stances` int(11) DEFAULT -1,
  `StancesNot` int(11) DEFAULT -1,
  `SpellVisual` int(11) DEFAULT -1,
  `ManaCostPercentage` int(11) DEFAULT -1,
  `StartRecoveryCategory` int(11) DEFAULT -1,
  `StartRecoveryTime` int(11) DEFAULT -1,
  `MaxAffectedTargets` int(11) DEFAULT -1,
  `MaxTargetLevel` int(11) DEFAULT -1,
  `DmgClass` int(11) DEFAULT -1,
  `rangeIndex` int(11) DEFAULT -1,
  `RecoveryTime` int(11) NOT NULL DEFAULT -1,
  `CategoryRecoveryTime` int(11) NOT NULL DEFAULT -1,
  `SpellFamilyName` int(11) NOT NULL DEFAULT -1,
  `SpellFamilyFlags` bigint(20) unsigned DEFAULT 0,
  `Mechanic` int(11) DEFAULT -1,
  `EquippedItemClass` int(11) DEFAULT -1,
  `Comment` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`Id`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_pet_auras`
--

DROP TABLE IF EXISTS `spell_pet_auras`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_pet_auras` (
  `spell` mediumint(8) unsigned NOT NULL COMMENT 'dummy spell id',
  `pet` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'pet id; 0 = all',
  `aura` mediumint(8) unsigned NOT NULL COMMENT 'pet aura id',
  PRIMARY KEY (`spell`,`pet`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_proc_event`
--

DROP TABLE IF EXISTS `spell_proc_event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_proc_event` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `SchoolMask` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyName` smallint(5) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyMask0` bigint(20) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyMask1` bigint(20) unsigned NOT NULL DEFAULT 0,
  `SpellFamilyMask2` bigint(20) unsigned NOT NULL DEFAULT 0,
  `procFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `procEx` int(10) unsigned NOT NULL DEFAULT 0,
  `ppmRate` float NOT NULL DEFAULT 0,
  `CustomChance` float NOT NULL DEFAULT 0,
  `Cooldown` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_proc_item_enchant`
--

DROP TABLE IF EXISTS `spell_proc_item_enchant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_proc_item_enchant` (
  `entry` mediumint(8) unsigned NOT NULL,
  `ppmRate` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_script_target`
--

DROP TABLE IF EXISTS `spell_script_target`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_script_target` (
  `entry` mediumint(8) unsigned NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `targetEntry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `conditionId` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `inverseEffectMask` mediumint(8) unsigned NOT NULL DEFAULT 0,
  UNIQUE KEY `entry_type_target` (`entry`,`type`,`targetEntry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Spell System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_scripts`
--

DROP TABLE IF EXISTS `spell_scripts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_scripts` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `priority` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `command` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `datalong2` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong3` int(10) unsigned NOT NULL DEFAULT 0,
  `datalong4` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param1` int(10) unsigned NOT NULL DEFAULT 0,
  `target_param2` int(10) unsigned NOT NULL DEFAULT 0,
  `target_type` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `data_flags` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `dataint` int(11) NOT NULL DEFAULT 0,
  `dataint2` int(11) NOT NULL DEFAULT 0,
  `dataint3` int(11) NOT NULL DEFAULT 0,
  `dataint4` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `o` float NOT NULL DEFAULT 0,
  `condition_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comments` varchar(255) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_target_position`
--

DROP TABLE IF EXISTS `spell_target_position`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_target_position` (
  `id` mediumint(8) unsigned NOT NULL DEFAULT 0 COMMENT 'Identifier',
  `target_map` smallint(5) unsigned NOT NULL DEFAULT 0,
  `target_position_x` float NOT NULL DEFAULT 0,
  `target_position_y` float NOT NULL DEFAULT 0,
  `target_position_z` float NOT NULL DEFAULT 0,
  `target_orientation` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`,`target_map`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED COMMENT='Spell System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_template`
--

DROP TABLE IF EXISTS `spell_template`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_template` (
  `entry` int(10) unsigned NOT NULL DEFAULT 0,
  `school` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Resistances.dbc',
  `category` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellCategory.dbc',
  `castUI` int(10) unsigned NOT NULL DEFAULT 0,
  `dispel` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellDispelType.dbc',
  `mechanic` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellMechanic.dbc',
  `attributes` int(10) unsigned NOT NULL DEFAULT 0,
  `attributesEx` int(10) unsigned NOT NULL DEFAULT 0,
  `attributesEx2` int(10) unsigned NOT NULL DEFAULT 0,
  `attributesEx3` int(10) unsigned NOT NULL DEFAULT 0,
  `attributesEx4` int(10) unsigned NOT NULL DEFAULT 0,
  `stances` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellShapeshiftForm.dbc',
  `stancesNot` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellShapeshiftForm.dbc',
  `targets` int(10) unsigned NOT NULL DEFAULT 0,
  `targetCreatureType` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'CreatureType.dbc',
  `requiresSpellFocus` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellFocusObject.dbc',
  `casterAuraState` int(10) unsigned NOT NULL DEFAULT 0,
  `targetAuraState` int(10) unsigned NOT NULL DEFAULT 0,
  `castingTimeIndex` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellCastTimes.dbc',
  `recoveryTime` int(10) unsigned NOT NULL DEFAULT 0,
  `categoryRecoveryTime` int(10) unsigned NOT NULL DEFAULT 0,
  `interruptFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `auraInterruptFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `channelInterruptFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `procFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `procChance` int(10) unsigned NOT NULL DEFAULT 0,
  `procCharges` int(10) unsigned NOT NULL DEFAULT 0,
  `maxLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `baseLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `spellLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `durationIndex` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellDuration.dbc',
  `powerType` int(10) unsigned NOT NULL DEFAULT 0,
  `manaCost` int(10) unsigned NOT NULL DEFAULT 0,
  `manCostPerLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `manaPerSecond` int(10) unsigned NOT NULL DEFAULT 0,
  `manaPerSecondPerLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `rangeIndex` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellRange.dbc',
  `speed` float NOT NULL DEFAULT 0,
  `modelNextSpell` int(10) unsigned NOT NULL DEFAULT 0,
  `stackAmount` int(10) unsigned NOT NULL DEFAULT 0,
  `totem1` int(10) unsigned NOT NULL DEFAULT 0,
  `totem2` int(10) unsigned NOT NULL DEFAULT 0,
  `reagent1` int(11) NOT NULL DEFAULT 0,
  `reagent2` int(11) NOT NULL DEFAULT 0,
  `reagent3` int(11) NOT NULL DEFAULT 0,
  `reagent4` int(11) NOT NULL DEFAULT 0,
  `reagent5` int(11) NOT NULL DEFAULT 0,
  `reagent6` int(11) NOT NULL DEFAULT 0,
  `reagent7` int(11) NOT NULL DEFAULT 0,
  `reagent8` int(11) NOT NULL DEFAULT 0,
  `reagentCount1` int(10) unsigned NOT NULL DEFAULT 0,
  `reagentCount2` int(10) unsigned NOT NULL DEFAULT 0,
  `reagentCount3` int(10) unsigned NOT NULL DEFAULT 0,
  `reagentCount4` int(10) unsigned NOT NULL DEFAULT 0,
  `reagentCount5` int(10) unsigned NOT NULL DEFAULT 0,
  `reagentCount6` int(10) unsigned NOT NULL DEFAULT 0,
  `reagentCount7` int(10) unsigned NOT NULL DEFAULT 0,
  `reagentCount8` int(10) unsigned NOT NULL DEFAULT 0,
  `equippedItemClass` int(11) NOT NULL DEFAULT 0 COMMENT 'ItemClass.dbc',
  `equippedItemSubClassMask` int(11) NOT NULL DEFAULT 0 COMMENT 'ItemSubClass.dbc',
  `equippedItemInventoryTypeMask` int(11) NOT NULL DEFAULT 0,
  `effect1` int(10) unsigned NOT NULL DEFAULT 0,
  `effect2` int(10) unsigned NOT NULL DEFAULT 0,
  `effect3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectDieSides1` int(11) NOT NULL DEFAULT 0,
  `effectDieSides2` int(11) NOT NULL DEFAULT 0,
  `effectDieSides3` int(11) NOT NULL DEFAULT 0,
  `effectBaseDice1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectBaseDice2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectBaseDice3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectDicePerLevel1` float NOT NULL DEFAULT 0,
  `effectDicePerLevel2` float NOT NULL DEFAULT 0,
  `effectDicePerLevel3` float NOT NULL DEFAULT 0,
  `effectRealPointsPerLevel1` float NOT NULL DEFAULT 0,
  `effectRealPointsPerLevel2` float NOT NULL DEFAULT 0,
  `effectRealPointsPerLevel3` float NOT NULL DEFAULT 0,
  `effectBasePoints1` int(11) NOT NULL DEFAULT 0,
  `effectBasePoints2` int(11) NOT NULL DEFAULT 0,
  `effectBasePoints3` int(11) NOT NULL DEFAULT 0,
  `effectBonusCoefficient1` float NOT NULL DEFAULT -1,
  `effectBonusCoefficient2` float NOT NULL DEFAULT -1,
  `effectBonusCoefficient3` float NOT NULL DEFAULT -1,
  `effectMechanic1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectMechanic2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectMechanic3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectImplicitTargetA1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectImplicitTargetA2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectImplicitTargetA3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectImplicitTargetB1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectImplicitTargetB2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectImplicitTargetB3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectRadiusIndex1` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellRadius.dbc',
  `effectRadiusIndex2` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellRadius.dbc',
  `effectRadiusIndex3` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellRadius.dbc',
  `effectApplyAuraName1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectApplyAuraName2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectApplyAuraName3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectAmplitude1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectAmplitude2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectAmplitude3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectMultipleValue1` float NOT NULL DEFAULT 0,
  `effectMultipleValue2` float NOT NULL DEFAULT 0,
  `effectMultipleValue3` float NOT NULL DEFAULT 0,
  `effectChainTarget1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectChainTarget2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectChainTarget3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectItemType1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectItemType2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectItemType3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectMiscValue1` int(11) NOT NULL DEFAULT 0,
  `effectMiscValue2` int(11) NOT NULL DEFAULT 0,
  `effectMiscValue3` int(11) NOT NULL DEFAULT 0,
  `effectTriggerSpell1` int(10) unsigned NOT NULL DEFAULT 0,
  `effectTriggerSpell2` int(10) unsigned NOT NULL DEFAULT 0,
  `effectTriggerSpell3` int(10) unsigned NOT NULL DEFAULT 0,
  `effectPointsPerComboPoint1` float NOT NULL DEFAULT 0,
  `effectPointsPerComboPoint2` float NOT NULL DEFAULT 0,
  `effectPointsPerComboPoint3` float NOT NULL DEFAULT 0,
  `spellVisual1` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellVisual.dbc',
  `spellVisual2` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellVisual.dbc',
  `spellIconId` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellIcon.dbc',
  `activeIconId` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'SpellIcon.dbc',
  `spellPriority` int(10) unsigned NOT NULL DEFAULT 0,
  `name` varchar(256) NOT NULL DEFAULT '',
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `nameSubtext` varchar(256) NOT NULL DEFAULT '',
  `nameSubtextFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `description` varchar(1024) NOT NULL DEFAULT '',
  `descriptionFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `auraDescription` varchar(512) NOT NULL DEFAULT '',
  `auraDescriptionFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `manaCostPercentage` int(10) unsigned NOT NULL DEFAULT 0,
  `startRecoveryCategory` int(10) unsigned NOT NULL DEFAULT 0,
  `startRecoveryTime` int(10) unsigned NOT NULL DEFAULT 0,
  `minTargetLevel` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Custom Field',
  `maxTargetLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `spellFamilyName` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'ChrClasses.dbc',
  `spellFamilyFlags` bigint(20) unsigned NOT NULL DEFAULT 0,
  `maxAffectedTargets` int(10) unsigned NOT NULL DEFAULT 0,
  `dmgClass` int(10) unsigned NOT NULL DEFAULT 0,
  `preventionType` int(10) unsigned NOT NULL DEFAULT 0,
  `stanceBarOrder` int(11) NOT NULL DEFAULT 0,
  `dmgMultiplier1` float NOT NULL DEFAULT 0,
  `dmgMultiplier2` float NOT NULL DEFAULT 0,
  `dmgMultiplier3` float NOT NULL DEFAULT 0,
  `minFactionId` int(10) unsigned NOT NULL DEFAULT 0,
  `minReputation` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredAuraVision` int(10) unsigned NOT NULL DEFAULT 0,
  `customFlags` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'Custom Field',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spell_threat`
--

DROP TABLE IF EXISTS `spell_threat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spell_threat` (
  `entry` mediumint(8) unsigned NOT NULL,
  `Threat` smallint(6) NOT NULL,
  `multiplier` float NOT NULL DEFAULT 1 COMMENT 'threat multiplier for damage/healing',
  `ap_bonus` float NOT NULL DEFAULT 0 COMMENT 'additional threat bonus from attack power',
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellcasttimes`
--

DROP TABLE IF EXISTS `spellcasttimes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellcasttimes` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `CastingTime` int(11) NOT NULL DEFAULT 0,
  `CastingTimePerLevel` int(11) NOT NULL DEFAULT 0,
  `MinimumCastingTime` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellcategory`
--

DROP TABLE IF EXISTS `spellcategory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellcategory` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellchaineffects`
--

DROP TABLE IF EXISTS `spellchaineffects`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellchaineffects` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `avgSegLen` float NOT NULL DEFAULT 0,
  `width` float NOT NULL DEFAULT 0,
  `noiseScale` float NOT NULL DEFAULT 0,
  `texCoordScale` float NOT NULL DEFAULT 0,
  `segDuration` int(11) NOT NULL DEFAULT 0,
  `segDelay` int(11) NOT NULL DEFAULT 0,
  `texture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `jointCount` int(10) unsigned NOT NULL DEFAULT 0,
  `jointOffsetRadius` float NOT NULL DEFAULT 0,
  `jointsPerMinorJoint` int(11) NOT NULL DEFAULT 0,
  `minorJointsPerMajorPoint` int(11) NOT NULL DEFAULT 0,
  `minorJointScale` float NOT NULL DEFAULT 0,
  `majorJointScale` float NOT NULL DEFAULT 0,
  `jointMoveSpeed` float NOT NULL DEFAULT 0,
  `jointSmoothness` float NOT NULL DEFAULT 0,
  `minDurationBetweenJointJumps` float NOT NULL DEFAULT 0,
  `maxDurationBetweenJointJumps` float NOT NULL DEFAULT 0,
  `waveHeight` float NOT NULL DEFAULT 0,
  `waveFreq` float NOT NULL DEFAULT 0,
  `waveSpeed` float NOT NULL DEFAULT 0,
  `minWaveAngle` float NOT NULL DEFAULT 0,
  `maxWaveAngle` float NOT NULL DEFAULT 0,
  `minWaveSpin` float NOT NULL DEFAULT 0,
  `maxWaveSpin` float NOT NULL DEFAULT 0,
  `arcHeight` float NOT NULL DEFAULT 0,
  `minArcAngle` float NOT NULL DEFAULT 0,
  `maxArcAngle` float NOT NULL DEFAULT 0,
  `minArcSpin` float NOT NULL DEFAULT 0,
  `maxArcSpin` float NOT NULL DEFAULT 0,
  `delayBetweenEffects` float NOT NULL DEFAULT 0,
  `minFlickerOnDuration` float NOT NULL DEFAULT 0,
  `maxFlickerOnDuration` float NOT NULL DEFAULT 0,
  `minFlickerOffDuration` float NOT NULL DEFAULT 0,
  `maxFlickerOffDuration` float NOT NULL DEFAULT 0,
  `pulseSpeed` float NOT NULL DEFAULT 0,
  `pulseOnLength` float NOT NULL DEFAULT 0,
  `pulseFadeLength` float NOT NULL DEFAULT 0,
  `alpha` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `red` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `green` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `blue` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `blendMode` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `combo` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `renderLayer` int(11) NOT NULL DEFAULT 0,
  `textureLength` float NOT NULL DEFAULT 0,
  `wavePhase` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spelldescriptionvariables`
--

DROP TABLE IF EXISTS `spelldescriptionvariables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spelldescriptionvariables` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Formula` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spelldifficulty`
--

DROP TABLE IF EXISTS `spelldifficulty`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spelldifficulty` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Difficulties1` int(10) unsigned NOT NULL DEFAULT 0,
  `Difficulties2` int(10) unsigned NOT NULL DEFAULT 0,
  `Difficulties3` int(10) unsigned NOT NULL DEFAULT 0,
  `Difficulties4` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spelldispeltype`
--

DROP TABLE IF EXISTS `spelldispeltype`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spelldispeltype` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `Combinations` int(10) unsigned NOT NULL DEFAULT 0,
  `ImmunityPossible` int(10) unsigned NOT NULL DEFAULT 0,
  `InternalName` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellduration`
--

DROP TABLE IF EXISTS `spellduration`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellduration` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `BaseDuration` int(10) unsigned NOT NULL DEFAULT 0,
  `PerLevel` int(11) NOT NULL DEFAULT 0,
  `MaximumDuration` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellfocusobject`
--

DROP TABLE IF EXISTS `spellfocusobject`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellfocusobject` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellicon`
--

DROP TABLE IF EXISTS `spellicon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellicon` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellitemenchantment`
--

DROP TABLE IF EXISTS `spellitemenchantment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellitemenchantment` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `charges` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDispelType1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDispelType2` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellDispelType3` int(10) unsigned NOT NULL DEFAULT 0,
  `minAmount1` int(10) unsigned NOT NULL DEFAULT 0,
  `minAmount2` int(10) unsigned NOT NULL DEFAULT 0,
  `minAmount3` int(10) unsigned NOT NULL DEFAULT 0,
  `maxAmount1` int(10) unsigned NOT NULL DEFAULT 0,
  `maxAmount2` int(10) unsigned NOT NULL DEFAULT 0,
  `maxAmount3` int(10) unsigned NOT NULL DEFAULT 0,
  `objectId1` int(10) unsigned NOT NULL DEFAULT 0,
  `objectId2` int(10) unsigned NOT NULL DEFAULT 0,
  `objectId3` int(10) unsigned NOT NULL DEFAULT 0,
  `sRefName0` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefName15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `sRefNameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemVisuals` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `ItemCache` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellItemEnchantmentCondition` int(10) unsigned NOT NULL DEFAULT 0,
  `SkillLine` int(10) unsigned NOT NULL DEFAULT 0,
  `SkillLevel` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredLevel` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellmechanic`
--

DROP TABLE IF EXISTS `spellmechanic`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellmechanic` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellmissile`
--

DROP TABLE IF EXISTS `spellmissile`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellmissile` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `defaultPitchMin` float NOT NULL DEFAULT 0,
  `defaultPitchMax` float NOT NULL DEFAULT 0,
  `defaultSpeedMin` float NOT NULL DEFAULT 0,
  `defaultSpeedMax` float NOT NULL DEFAULT 0,
  `randomizeFacingMin` float NOT NULL DEFAULT 0,
  `randomizeFacingMax` float NOT NULL DEFAULT 0,
  `randomizePitchMin` float NOT NULL DEFAULT 0,
  `randomizePitchMax` float NOT NULL DEFAULT 0,
  `randomizeSpeedMin` float NOT NULL DEFAULT 0,
  `randomizeSpeedMax` float NOT NULL DEFAULT 0,
  `gravity` float NOT NULL DEFAULT 0,
  `maxDuration` float NOT NULL DEFAULT 0,
  `collisionRadius` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellmissilemotion`
--

DROP TABLE IF EXISTS `spellmissilemotion`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellmissilemotion` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `script` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `flags` int(11) NOT NULL DEFAULT 0,
  `missileCount` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellradius`
--

DROP TABLE IF EXISTS `spellradius`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellradius` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Radius` float NOT NULL DEFAULT 0,
  `RadiusPerLevel` float NOT NULL DEFAULT 0,
  `MaximumRadius` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellrange`
--

DROP TABLE IF EXISTS `spellrange`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellrange` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `MinimumRangeHostile` float NOT NULL DEFAULT 0,
  `MinimumRangeFriend` float NOT NULL DEFAULT 0,
  `MaximumRangeHostile` float NOT NULL DEFAULT 0,
  `MaximumRangeFriend` float NOT NULL DEFAULT 0,
  `Type` int(11) NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `ShortName1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortName16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `ShortNameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellrunecost`
--

DROP TABLE IF EXISTS `spellrunecost`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellrunecost` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `RuneCost1` int(10) unsigned NOT NULL DEFAULT 0,
  `RuneCost2` int(10) unsigned NOT NULL DEFAULT 0,
  `RuneCost3` int(10) unsigned NOT NULL DEFAULT 0,
  `RunePowerGain` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellshapeshiftform`
--

DROP TABLE IF EXISTS `spellshapeshiftform`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellshapeshiftform` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `ActionBar` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlag` int(10) unsigned NOT NULL DEFAULT 0,
  `CreatureType1` int(11) NOT NULL DEFAULT 0,
  `CreatureType2` int(11) NOT NULL DEFAULT 0,
  `SpellIcon` int(11) NOT NULL DEFAULT 0,
  `CombatRoundTime` int(11) NOT NULL DEFAULT 0,
  `Display1` int(10) unsigned NOT NULL DEFAULT 0,
  `Display2` int(10) unsigned NOT NULL DEFAULT 0,
  `Display3` int(10) unsigned NOT NULL DEFAULT 0,
  `Display4` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID1` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID2` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID3` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID4` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID5` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID6` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID7` int(10) unsigned NOT NULL DEFAULT 0,
  `presetSpellID8` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellvisual`
--

DROP TABLE IF EXISTS `spellvisual`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellvisual` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `PrecastKit` int(10) unsigned NOT NULL DEFAULT 0,
  `CastKit` int(10) unsigned NOT NULL DEFAULT 0,
  `ImpactKit` int(10) unsigned NOT NULL DEFAULT 0,
  `StateKit` int(10) unsigned NOT NULL DEFAULT 0,
  `StateDoneKit` int(10) unsigned NOT NULL DEFAULT 0,
  `ChannelKit` int(10) unsigned NOT NULL DEFAULT 0,
  `HasMissile` int(11) NOT NULL DEFAULT 0,
  `MissileModel` int(10) unsigned NOT NULL DEFAULT 0,
  `MissilePathType` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileDestinationAttachment` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileSound` int(10) unsigned NOT NULL DEFAULT 0,
  `AnimEventSoundId` int(10) unsigned NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `CasterImpactKit` int(10) unsigned NOT NULL DEFAULT 0,
  `TargetImpactKit` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileAttachment` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileFollowGroundHeight` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileFollowDropSpeed` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileFollowApproach` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileFollowGroundFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileMotion` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileTargetingKit` int(10) unsigned NOT NULL DEFAULT 0,
  `InstantAreaKit` int(10) unsigned NOT NULL DEFAULT 0,
  `ImpactAreaKit` int(10) unsigned NOT NULL DEFAULT 0,
  `PersistentAreaKit` int(10) unsigned NOT NULL DEFAULT 0,
  `MissileCastOffsetX` float NOT NULL DEFAULT 0,
  `MissileCastOffsetY` float NOT NULL DEFAULT 0,
  `MissileCastOffsetZ` float NOT NULL DEFAULT 0,
  `MissileImpactOffsetX` float NOT NULL DEFAULT 0,
  `MissileImpactOffsetY` float NOT NULL DEFAULT 0,
  `MissileImpactOffsetZ` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellvisualeffectname`
--

DROP TABLE IF EXISTS `spellvisualeffectname`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellvisualeffectname` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `FilePath` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `AreaEffectSize` float NOT NULL DEFAULT 0,
  `Scale` float NOT NULL DEFAULT 0,
  `MinAllowedScale` float NOT NULL DEFAULT 0,
  `MaxAllowedScale` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellvisualkit`
--

DROP TABLE IF EXISTS `spellvisualkit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellvisualkit` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `StartAnimId` int(10) unsigned NOT NULL DEFAULT 0,
  `AnimationId` int(10) unsigned NOT NULL DEFAULT 0,
  `HeadEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `ChestEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `BaseEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `LeftHandEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `RightHandEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `BreathEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `LeftWeaponEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `RightWeaponEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `SpecialEffect1` int(10) unsigned NOT NULL DEFAULT 0,
  `SpecialEffect2` int(10) unsigned NOT NULL DEFAULT 0,
  `SpecialEffect3` int(10) unsigned NOT NULL DEFAULT 0,
  `WorldEffect` int(10) unsigned NOT NULL DEFAULT 0,
  `SoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `ShakeID` int(10) unsigned NOT NULL DEFAULT 0,
  `CharProc1` int(10) unsigned NOT NULL DEFAULT 0,
  `CharProc2` int(10) unsigned NOT NULL DEFAULT 0,
  `CharProc3` int(10) unsigned NOT NULL DEFAULT 0,
  `CharProc4` int(10) unsigned NOT NULL DEFAULT 0,
  `CharParamZero1` float NOT NULL DEFAULT 0,
  `CharParamZero2` float NOT NULL DEFAULT 0,
  `CharParamZero3` float NOT NULL DEFAULT 0,
  `CharParamZero4` float NOT NULL DEFAULT 0,
  `CharParamOne1` float NOT NULL DEFAULT 0,
  `CharParamOne2` float NOT NULL DEFAULT 0,
  `CharParamOne3` float NOT NULL DEFAULT 0,
  `CharParamOne4` float NOT NULL DEFAULT 0,
  `CharParamTwo1` float NOT NULL DEFAULT 0,
  `CharParamTwo2` float NOT NULL DEFAULT 0,
  `CharParamTwo3` float NOT NULL DEFAULT 0,
  `CharParamTwo4` float NOT NULL DEFAULT 0,
  `CharParamThree1` float NOT NULL DEFAULT 0,
  `CharParamThree2` float NOT NULL DEFAULT 0,
  `CharParamThree3` float NOT NULL DEFAULT 0,
  `CharParamThree4` float NOT NULL DEFAULT 0,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellvisualkitareamodel`
--

DROP TABLE IF EXISTS `spellvisualkitareamodel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellvisualkitareamodel` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `enumID` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellvisualkitmodelattach`
--

DROP TABLE IF EXISTS `spellvisualkitmodelattach`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellvisualkitmodelattach` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `ParentSpellVisualKitId` int(10) unsigned NOT NULL DEFAULT 0,
  `SpellVisualEffectNameId` int(10) unsigned NOT NULL DEFAULT 0,
  `AttachmentId` int(10) unsigned NOT NULL DEFAULT 0,
  `OffsetX` float NOT NULL DEFAULT 0,
  `OffsetY` float NOT NULL DEFAULT 0,
  `OffsetZ` float NOT NULL DEFAULT 0,
  `Yaw` float NOT NULL DEFAULT 0,
  `Pitch` float NOT NULL DEFAULT 0,
  `Roll` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `spellvisualprecasttransitions`
--

DROP TABLE IF EXISTS `spellvisualprecasttransitions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `spellvisualprecasttransitions` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `PrecastLoadAnimName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `PrecastHoldAnimName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `stationery`
--

DROP TABLE IF EXISTS `stationery`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `stationery` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `itemId` int(10) unsigned NOT NULL DEFAULT 0,
  `texture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `talent`
--

DROP TABLE IF EXISTS `talent`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `talent` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `talentTabId` int(10) unsigned NOT NULL DEFAULT 0,
  `tierId` int(10) unsigned NOT NULL DEFAULT 0,
  `columnIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank1` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank2` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank3` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank4` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank5` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank6` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank7` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank8` int(10) unsigned NOT NULL DEFAULT 0,
  `spellRank9` int(10) unsigned NOT NULL DEFAULT 0,
  `prereqTalent1` int(10) unsigned NOT NULL DEFAULT 0,
  `prereqTalent2` int(10) unsigned NOT NULL DEFAULT 0,
  `prereqTalent3` int(10) unsigned NOT NULL DEFAULT 0,
  `prereqRank1` int(10) unsigned NOT NULL DEFAULT 0,
  `prereqRank2` int(10) unsigned NOT NULL DEFAULT 0,
  `prereqRank3` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `requiredSpellId` int(10) unsigned NOT NULL DEFAULT 0,
  `allowForPetFlags1` int(10) unsigned NOT NULL DEFAULT 0,
  `allowForPetFlags2` int(10) unsigned NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `talenttab`
--

DROP TABLE IF EXISTS `talenttab`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `talenttab` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Flags` int(10) unsigned NOT NULL DEFAULT 0,
  `iconId` int(10) unsigned NOT NULL DEFAULT 0,
  `raceMask` int(10) unsigned NOT NULL DEFAULT 0,
  `classMask` int(10) unsigned NOT NULL DEFAULT 0,
  `creatureFamilyCategory` int(10) unsigned NOT NULL DEFAULT 0,
  `orderIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `backgroundFileName` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `taxi_nodes`
--

DROP TABLE IF EXISTS `taxi_nodes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `taxi_nodes` (
  `id` smallint(5) unsigned NOT NULL,
  `map_id` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `name` varchar(256) NOT NULL DEFAULT '',
  `mount_creature_id1` smallint(5) unsigned NOT NULL DEFAULT 0,
  `mount_creature_id2` smallint(5) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `taxi_path_transitions`
--

DROP TABLE IF EXISTS `taxi_path_transitions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `taxi_path_transitions` (
  `in_path` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `out_path` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `in_node` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `out_node` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `comment` text DEFAULT NULL,
  PRIMARY KEY (`in_path`,`out_path`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `taxinodes`
--

DROP TABLE IF EXISTS `taxinodes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `taxinodes` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `map` int(11) NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `nameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `mount1` int(10) unsigned NOT NULL DEFAULT 0,
  `mount2` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `taxipath`
--

DROP TABLE IF EXISTS `taxipath`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `taxipath` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `fromTaxiNode` int(10) unsigned NOT NULL DEFAULT 0,
  `toTaxiNode` int(10) unsigned NOT NULL DEFAULT 0,
  `cost` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `taxipathnode`
--

DROP TABLE IF EXISTS `taxipathnode`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `taxipathnode` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `pathId` int(10) unsigned NOT NULL DEFAULT 0,
  `nodeIndex` int(10) unsigned NOT NULL DEFAULT 0,
  `mapId` int(10) unsigned NOT NULL DEFAULT 0,
  `x` float NOT NULL DEFAULT 0,
  `y` float NOT NULL DEFAULT 0,
  `z` float NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `delay` int(10) unsigned NOT NULL DEFAULT 0,
  `arrivalEventId` int(10) unsigned NOT NULL DEFAULT 0,
  `departureEventId` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `totemcategory`
--

DROP TABLE IF EXISTS `totemcategory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `totemcategory` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `CategoryType` int(10) unsigned NOT NULL DEFAULT 0,
  `CategoryMask` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `transports`
--

DROP TABLE IF EXISTS `transports`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `transports` (
  `guid` int(10) unsigned NOT NULL DEFAULT 0,
  `entry` mediumint(8) unsigned NOT NULL DEFAULT 0,
  `name` text DEFAULT NULL,
  `period` mediumint(8) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`entry`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Transports';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `variables`
--

DROP TABLE IF EXISTS `variables`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `variables` (
  `index` int(10) unsigned NOT NULL DEFAULT 0,
  `value` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`index`) USING BTREE
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci ROW_FORMAT=FIXED;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `vehicle`
--

DROP TABLE IF EXISTS `vehicle`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `vehicle` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `turnSpeed` float NOT NULL DEFAULT 0,
  `pitchSpeed` float NOT NULL DEFAULT 0,
  `pitchMin` float NOT NULL DEFAULT 0,
  `pitchMax` float NOT NULL DEFAULT 0,
  `seatID1` int(10) unsigned NOT NULL DEFAULT 0,
  `seatID2` int(10) unsigned NOT NULL DEFAULT 0,
  `seatID3` int(10) unsigned NOT NULL DEFAULT 0,
  `seatID4` int(10) unsigned NOT NULL DEFAULT 0,
  `seatID5` int(10) unsigned NOT NULL DEFAULT 0,
  `seatID6` int(10) unsigned NOT NULL DEFAULT 0,
  `seatID7` int(10) unsigned NOT NULL DEFAULT 0,
  `seatID8` int(10) unsigned NOT NULL DEFAULT 0,
  `mouseLookOffsetPitch` float NOT NULL DEFAULT 0,
  `cameraFadeDistScalarMin` float NOT NULL DEFAULT 0,
  `cameraFadeDistScalarMax` float NOT NULL DEFAULT 0,
  `cameraPitchOffset` float NOT NULL DEFAULT 0,
  `facingLimitRight` float NOT NULL DEFAULT 0,
  `facingLimitLeft` float NOT NULL DEFAULT 0,
  `msslTrgtTurnLingering` float NOT NULL DEFAULT 0,
  `msslTrgtPitchLingering` float NOT NULL DEFAULT 0,
  `msslTrgtMouseLingering` float NOT NULL DEFAULT 0,
  `msslTrgtEndOpacity` float NOT NULL DEFAULT 0,
  `msslTrgtArcSpeed` float NOT NULL DEFAULT 0,
  `msslTrgtArcRepeat` float NOT NULL DEFAULT 0,
  `msslTrgtArcWidth` float NOT NULL DEFAULT 0,
  `msslTrgtImpactRadius1` float NOT NULL DEFAULT 0,
  `msslTrgtImpactRadius2` float NOT NULL DEFAULT 0,
  `msslTrgtArcTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `msslTrgtImpactTexture` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `msslTrgtImpactModel1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `msslTrgtImpactModel2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `cameraYawOffset` float NOT NULL DEFAULT 0,
  `uiLocomotionType` int(10) unsigned NOT NULL DEFAULT 0,
  `msslTrgtImpactTexRadius` float NOT NULL DEFAULT 0,
  `uiSeatIndicatorType` int(10) unsigned NOT NULL DEFAULT 0,
  `PowerDisplay1` int(11) NOT NULL DEFAULT 0,
  `PowerDisplay2` int(11) NOT NULL DEFAULT 0,
  `PowerDisplay3` int(11) NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `vehicleseat`
--

DROP TABLE IF EXISTS `vehicleseat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `vehicleseat` (
  `ID` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(11) NOT NULL DEFAULT 0,
  `attachmentID` int(11) NOT NULL DEFAULT 0,
  `attachmentOffsetX` float NOT NULL DEFAULT 0,
  `attachmentOffsetY` float NOT NULL DEFAULT 0,
  `attachmentOffsetZ` float NOT NULL DEFAULT 0,
  `enterPreDelay` float NOT NULL DEFAULT 0,
  `enterSpeed` float NOT NULL DEFAULT 0,
  `enterGravity` float NOT NULL DEFAULT 0,
  `enterMinDuration` float NOT NULL DEFAULT 0,
  `enterMaxDuration` float NOT NULL DEFAULT 0,
  `enterMinArcHeight` float NOT NULL DEFAULT 0,
  `enterMaxArcHeight` float NOT NULL DEFAULT 0,
  `enterAnimStart` int(11) NOT NULL DEFAULT 0,
  `enterAnimLoop` int(11) NOT NULL DEFAULT 0,
  `rideAnimStart` int(11) NOT NULL DEFAULT 0,
  `rideAnimLoop` int(11) NOT NULL DEFAULT 0,
  `rideUpperAnimStart` int(11) NOT NULL DEFAULT 0,
  `rideUpperAnimLoop` int(11) NOT NULL DEFAULT 0,
  `exitPreDelay` float NOT NULL DEFAULT 0,
  `exitSpeed` float NOT NULL DEFAULT 0,
  `exitGravity` float NOT NULL DEFAULT 0,
  `exitMinDuration` float NOT NULL DEFAULT 0,
  `exitMaxDuration` float NOT NULL DEFAULT 0,
  `exitMinArcHeight` float NOT NULL DEFAULT 0,
  `exitMaxArcHeight` float NOT NULL DEFAULT 0,
  `exitAnimStart` int(11) NOT NULL DEFAULT 0,
  `exitAnimLoop` int(11) NOT NULL DEFAULT 0,
  `exitAnimEnd` int(11) NOT NULL DEFAULT 0,
  `passengerYaw` float NOT NULL DEFAULT 0,
  `passengerPitch` float NOT NULL DEFAULT 0,
  `passengerRoll` float NOT NULL DEFAULT 0,
  `passengerAttachmentID` int(11) NOT NULL DEFAULT 0,
  `vehicleEnterAnim` int(11) NOT NULL DEFAULT 0,
  `vehicleExitAnim` int(11) NOT NULL DEFAULT 0,
  `vehicleRideAnimLoop` int(11) NOT NULL DEFAULT 0,
  `vehicleEnterAnimBone` int(11) NOT NULL DEFAULT 0,
  `vehicleExitAnimBone` int(11) NOT NULL DEFAULT 0,
  `vehicleRideAnimLoopBone` int(11) NOT NULL DEFAULT 0,
  `vehicleEnterAnimDelay` float NOT NULL DEFAULT 0,
  `vehicleExitAnimDelay` float NOT NULL DEFAULT 0,
  `vehicleAbilityDisplay` int(10) unsigned NOT NULL DEFAULT 0,
  `enterUISoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `exitUISoundID` int(10) unsigned NOT NULL DEFAULT 0,
  `uiSkin` int(10) unsigned NOT NULL DEFAULT 0,
  `flagsB` int(11) NOT NULL DEFAULT 0,
  `cameraEnteringDelay` float NOT NULL DEFAULT 0,
  `cameraEnteringDuration` float NOT NULL DEFAULT 0,
  `cameraExitingDelay` float NOT NULL DEFAULT 0,
  `cameraExitingDuration` float NOT NULL DEFAULT 0,
  `cameraOffsetX` float NOT NULL DEFAULT 0,
  `cameraOffsetY` float NOT NULL DEFAULT 0,
  `cameraOffsetZ` float NOT NULL DEFAULT 0,
  `cameraPosChaseRate` float NOT NULL DEFAULT 0,
  `cameraFacingChaseRate` float NOT NULL DEFAULT 0,
  `cameraEnteringZoom` float NOT NULL DEFAULT 0,
  `cameraSeatZoomMin` float NOT NULL DEFAULT 0,
  `cameraSeatZoomMax` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `warden_checks`
--

DROP TABLE IF EXISTS `warden_checks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `warden_checks` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Check ID',
  `group_id` smallint(5) unsigned NOT NULL COMMENT 'Grouping ID',
  `type` tinyint(3) unsigned DEFAULT NULL COMMENT 'Check Type',
  `data` varchar(48) NOT NULL DEFAULT '',
  `str` varchar(20) NOT NULL DEFAULT '',
  `address` int(10) unsigned NOT NULL DEFAULT 0,
  `length` tinyint(3) unsigned NOT NULL DEFAULT 0,
  `result` varchar(24) NOT NULL DEFAULT '',
  `penalty` tinyint(4) NOT NULL DEFAULT -1 COMMENT 'Action to take if check fails',
  `comment` varchar(50) DEFAULT '' COMMENT 'Description of what the check is',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=1574 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC COMMENT='Warden System';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `warden_scans`
--

DROP TABLE IF EXISTS `warden_scans`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `warden_scans` (
  `id` smallint(5) unsigned NOT NULL AUTO_INCREMENT,
  `type` int(11) DEFAULT 0,
  `str` text DEFAULT NULL,
  `data` text DEFAULT NULL,
  `address` int(11) DEFAULT 0,
  `length` int(11) DEFAULT 0,
  `result` tinytext NOT NULL,
  `flags` smallint(5) unsigned NOT NULL,
  `comment` tinytext NOT NULL,
  UNIQUE KEY `id` (`id`) USING BTREE
) ENGINE=MyISAM AUTO_INCREMENT=93 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `wmoareatable`
--

DROP TABLE IF EXISTS `wmoareatable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `wmoareatable` (
  `id` int(11) NOT NULL DEFAULT 0,
  `wmoId` int(11) NOT NULL DEFAULT 0,
  `nameSetId` int(11) NOT NULL DEFAULT 0,
  `wmoGroupId` int(11) NOT NULL DEFAULT 0,
  `soundProviderPref` int(10) unsigned NOT NULL DEFAULT 0,
  `soundProviderPrefUnderwater` int(10) unsigned NOT NULL DEFAULT 0,
  `ambienceId` int(10) unsigned NOT NULL DEFAULT 0,
  `zoneMusic` int(10) unsigned NOT NULL DEFAULT 0,
  `introSound` int(10) unsigned NOT NULL DEFAULT 0,
  `flags` int(10) unsigned NOT NULL DEFAULT 0,
  `areaTableId` int(10) unsigned NOT NULL DEFAULT 0,
  `Name1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `Name16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `NameFlags` int(10) unsigned NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `world_safe_locs_facing`
--

DROP TABLE IF EXISTS `world_safe_locs_facing`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `world_safe_locs_facing` (
  `id` int(10) unsigned NOT NULL,
  `orientation` float NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_unicode_ci ROW_FORMAT=DYNAMIC;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `worldstateui`
--

DROP TABLE IF EXISTS `worldstateui`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `worldstateui` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `mapId` int(10) unsigned NOT NULL DEFAULT 0,
  `areaId` int(10) unsigned NOT NULL DEFAULT 0,
  `phaseShift` int(10) unsigned NOT NULL DEFAULT 0,
  `icon` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `text16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `textFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `tooltip1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltip16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `tooltipFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `worldState` int(11) NOT NULL DEFAULT 0,
  `type` int(11) NOT NULL DEFAULT 0,
  `dynamicIcon` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip1` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip2` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip3` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip4` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip5` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip6` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip7` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip8` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip9` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip10` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip11` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip12` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip13` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip14` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip15` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltip16` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `dynamicTooltipFlags` int(10) unsigned NOT NULL DEFAULT 0,
  `extendedUIStateVar` int(10) unsigned NOT NULL DEFAULT 0,
  `extendedUIStateVarNeutral` int(10) unsigned NOT NULL DEFAULT 0,
  `extendedUIStateVarUnk1` int(10) unsigned NOT NULL DEFAULT 0,
  `extendedUIStateVarUnk2` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `zonemusic`
--

DROP TABLE IF EXISTS `zonemusic`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `zonemusic` (
  `id` int(10) unsigned NOT NULL DEFAULT 0,
  `name` text CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci DEFAULT NULL,
  `silenceMinDay` int(10) unsigned NOT NULL DEFAULT 0,
  `silenceMinNight` int(10) unsigned NOT NULL DEFAULT 0,
  `silenceMaxDay` int(10) unsigned NOT NULL DEFAULT 0,
  `silenceMaxNight` int(10) unsigned NOT NULL DEFAULT 0,
  `dayMusic` int(10) unsigned NOT NULL DEFAULT 0,
  `nightMusic` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-12-01 21:42:49
