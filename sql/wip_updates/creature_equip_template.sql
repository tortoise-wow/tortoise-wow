-- Cho'Rush the Observer (14324): weapon by the class variant he rolled.
--
-- boss_gordok_king.cpp calls LoadEquipment(12070) for the priest, 12071 for the shaman and
-- 12072 for the mage variant. No creature_equip_template row exists for those three entries, and
-- Creature::LoadEquipment does nothing when it finds none, so he always holds the staff of his
-- template (14618) whatever he casts.
--
-- Items: priest 2176 (Ornate Priest Staff), shaman 17113 (Amberseal Keeper), mage 14618
-- (Jeweled Red Staff Low Red Flame, the staff he holds today).
--
-- Idempotent: each row is set by its entry and set again on a re-run.
REPLACE INTO `creature_equip_template` (`entry`, `equipentry1`, `equipentry2`, `equipentry3`) VALUES
(12070, 2176, 0, 0),   -- priest
(12071, 17113, 0, 0),  -- shaman
(12072, 14618, 0, 0);  -- mage
