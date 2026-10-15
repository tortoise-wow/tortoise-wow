#include "AutoScaler.hpp"
#include "Database/DatabaseEnv.h"
#include "Map.h"
#include "World.h"

#include <mutex>
#include <memory>
#include <shared_mutex>

using namespace AutoScaling;

// Unused
void AutoScaler::LoadFromDB()
{
    std::unique_ptr<QueryResult> result{ WorldDatabase.Query("SELECT * FROM `disabled_dungeon_scaling`") };

    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();
        disabledScaling.insert(fields[0].GetUInt32());
    } while (result->NextRow());
}

class Read_Mutex_Guard
{
public:
    explicit Read_Mutex_Guard(std::shared_mutex& mut)
        : mut(mut)
    {	// construct and lock
        mut.lock_shared();
    }
    ~Read_Mutex_Guard() noexcept
    {	// unlock
        mut.unlock_shared();
    }

    Read_Mutex_Guard(const Read_Mutex_Guard&) = delete;
    Read_Mutex_Guard& operator=(const Read_Mutex_Guard&) = delete;
private:
    std::shared_mutex& mut;
};

void AutoScaler::Scale(DungeonMap* map)
{
    if (!sWorld.getConfig(CONFIG_BOOL_AUTOSCALER_ENABLE))
        return;

    uint32 playerCount = map->GetPlayersCountExceptGMs();
    uint32 maxCount = map->GetMaxPlayers();

    if (playerCount == 0)
        return;


    auto& lock = map->GetObjectLock();
    Read_Mutex_Guard guard{ lock };
    auto& container = const_cast<TypeUnorderedMapContainer<AllMapStoredObjectTypes, ObjectGuid>&>(map->GetObjectStore());

    auto pairItr = container.range<Creature>();
    while (pairItr.first != pairItr.second)
    {
        auto creature = pairItr.first->second;
        if (creature && !creature->IsInCombat())
            ScaleCreature(creature, playerCount, maxCount, map);

        ++pairItr.first;
    }
}

void AutoScaler::ScaleCreature(Creature* creature, uint32 playerCount, uint32 maxCount, Map* map)
{
    (void)map;

    if (!sWorld.getConfig(CONFIG_BOOL_AUTOSCALER_ENABLE))
        return;

    if (creature->IsPet() && creature->GetOwner() && creature->GetOwner()->IsPlayer())
        return;

    if (creature->IsDead())
        return;

    // Linear scale: 1 player -> 1/maxPlayers, maxPlayers -> 1.0
    const uint32 maxPlayers = std::max<uint32>(maxCount, 1);
    const float clampedPlayers = static_cast<float>(std::min<uint32>(std::max<uint32>(playerCount, 1), maxPlayers));
    float healthScaleFactor = clampedPlayers / static_cast<float>(maxPlayers);
    float damageScaleFactor = clampedPlayers / static_cast<float>(maxPlayers);

    // Clamp scaling via config
    if (maxPlayers <= 5 && healthScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_5MAN_HP))
        healthScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_5MAN_HP);

    if (maxPlayers <= 5 && damageScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_5MAN_DMG))
        damageScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_5MAN_DMG);

    if (maxPlayers <= 10 && healthScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_10MAN_HP))
        healthScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_10MAN_HP);

    if (maxPlayers <= 10 && damageScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_10MAN_DMG))
        damageScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_10MAN_DMG);

    if (maxPlayers <= 20 && healthScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_20MAN_HP))
        healthScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_20MAN_HP);

    if (maxPlayers <= 20 && damageScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_20MAN_DMG))
        damageScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_20MAN_DMG);

    if (maxPlayers <= 40 && healthScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_40MAN_HP))
        healthScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_40MAN_HP);

    if (maxPlayers <= 40 && damageScaleFactor < sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_40MAN_DMG))
        damageScaleFactor = sWorld.getConfig(CONFIG_FLOAT_SCALAR_MIN_40MAN_DMG);

    const auto healthScaleValue = [healthScaleFactor](float value)
    {
        return value * healthScaleFactor;
    };

    const auto damageScaleValue = [damageScaleFactor](float value)
    {
        return value * damageScaleFactor;
    };

    creature->SetMaxHealth(std::max(1u, static_cast<uint32>(healthScaleValue(creature->GetCreateHealth()))));

    if (baseDamages.find(creature->GetEntry()) == baseDamages.end())
    {
        // store base vals.
        auto tup = std::make_tuple(
                std::make_pair(creature->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE), creature->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE)),
                std::make_pair(creature->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE), creature->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE)),
                creature->GetInt32Value(UNIT_FIELD_ATTACK_POWER));
        baseDamages[creature->GetEntry()] = std::move(tup);
    }

    auto& tup = baseDamages[creature->GetEntry()];

    creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, damageScaleValue(std::get<0>(tup).first));
    creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, damageScaleValue(std::get<0>(tup).second));

    creature->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, damageScaleValue(std::get<0>(tup).first));
    creature->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, damageScaleValue(std::get<0>(tup).second));

    creature->SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, damageScaleValue(std::get<1>(tup).first));
    creature->SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, damageScaleValue(std::get<1>(tup).second));

    creature->SetInt32Value(UNIT_FIELD_ATTACK_POWER, static_cast<int32>(damageScaleValue(static_cast<float>(std::get<2>(tup)))));

    // Push updated weapon damage values to client fields
    creature->UpdateDamagePhysical(BASE_ATTACK);
    creature->UpdateDamagePhysical(OFF_ATTACK);
    creature->UpdateDamagePhysical(RANGED_ATTACK);
}

void AutoScaler::GenerateScaledMoneyLoot(Creature* creature, Loot* loot)
{
    if (!sWorld.getConfig(CONFIG_BOOL_AUTOSCALER_ENABLE))
        return;

    uint32 playerCount = creature->GetMap()->GetPlayersCountExceptGMs();
    const uint32 maxCount = std::max<uint32>(((DungeonMap*)creature->GetMap())->GetMaxPlayers(), 1);
    const uint32 clampedPlayers = std::min<uint32>(std::max<uint32>(playerCount, 1), maxCount);
    const float gold_factor = static_cast<float>(clampedPlayers) / static_cast<float>(maxCount);
    loot->GenerateMoneyLoot(static_cast<uint32>(creature->GetGoldMin() * gold_factor),
                            static_cast<uint32>(creature->GetGoldMax() * gold_factor));
}
