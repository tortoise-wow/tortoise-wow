// Executes production native-reward classification/dispatch/title/aura orchestration.
// The spell engine is a boundary fake; real effect persistence is checked natively.
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using uint8=uint8_t; using uint32=uint32_t;
constexpr uint8 MAX_EFFECT_INDEX=3, EFFECT_INDEX_0=0;
enum { TYPEID_PLAYER, TYPEID_UNIT, TYPEID_GAMEOBJECT };
enum { SPELL_EFFECT_NONE=0,SPELL_EFFECT_TELEPORT_UNITS=5,SPELL_EFFECT_APPLY_AURA=6,
    SPELL_EFFECT_CREATE_ITEM=24,SPELL_EFFECT_LEARN_SPELL=36,SPELL_EFFECT_SKILL_STEP=44,
    SPELL_EFFECT_ADD_HONOR=45,SPELL_EFFECT_TRADE_SKILL=47,SPELL_EFFECT_DUMMY=3,
    SPELL_EFFECT_SUMMON_PET=56,SPELL_EFFECT_TRIGGER_SPELL=64,SPELL_EFFECT_SEND_EVENT=61 };
enum { TARGET_NONE=0,TARGET_UNIT_CASTER=1,TARGET_LOCATION_DATABASE=17,
    TARGET_UNIT_FRIEND=21,TARGET_UNIT=25,TARGET_AREA=20 };
enum { TARGET_FLAG_DEST_LOCATION=1,TARGET_FLAG_SOURCE_LOCATION=2 };
enum { SPELL_CAST_OK,SPELL_CAST_FAILED };
enum { SPELL_STATE_PREPARING,SPELL_STATE_FINISHED };
enum { TITLE_STORMWIELDER=1,TITLE_GUARDIAN_OF_TIRISFAL,TITLE_LOREKEEPER,TITLE_SEEKER_OF_KNOWLEDGE };
struct ObjectGuid { uint32 id; };
struct WorldObject { uint32 type=TYPEID_GAMEOBJECT; uint32 GetTypeId() const{return type;} };
struct Unit:WorldObject { Unit(){type=TYPEID_UNIT;} };
struct SpellTargetPosition { uint32 mapId; };
struct SpellEntry
{
    uint32 Id=7329, Targets=0, castTime=0;
    std::array<uint32,3> Effect{},EffectImplicitTargetA{},EffectImplicitTargetB{},EffectTriggerSpell{};
    float speed=0;
    bool channel=false,cc=false,failPrepare=false,unfinished=false;
    bool IsChanneledSpell() const{return channel;}
    bool IsCCSpell() const{return cc;}
    uint32 GetCastTime(Unit*,void*) const{return castTime;}
};
struct Quest
{
    uint32 id=1785, spell=0,cast=7329;
    uint32 GetQuestId()const{return id;}
    uint32 GetRewSpell()const{return spell;}
    uint32 GetRewSpellCast()const{return cast;}
};
class Player:public Unit
{
public:
    uint32 guid=1,mapId=0,items=0,honor=0,skills=0;
    std::set<uint32> learned,auras,titles,earned;
    std::vector<uint32> deferredWorld;
    Player(){type=TYPEID_PLAYER;}
    ObjectGuid GetObjectGuid()const{return {guid};}
    uint32 GetMapId()const{return mapId;}
    uint32 pendingMap=0;
    bool _ApplyNativeQuestRewards(Quest const*,WorldObject*,bool&);
    void GetZoneAndAreaId(uint32&zone,uint32&area){zone=1;area=2;}
    bool HasAura(uint32 id,uint8)const{return auras.count(id)!=0;}
    void RemoveAurasDueToSpell(uint32 id){auras.erase(id);}
    bool HasEarnedTitle(uint32 id)const{return earned.count(id)!=0;}
    void AwardTitle(uint32 id){titles.insert(id);}
    std::string GetName()const{return "Actor";}
    void CastSpell(Player*,uint32 id,bool){deferredWorld.push_back(id);}
};
struct SpellArea
{
    uint32 spellId;
    bool autocast=true,fit=true;
    bool IsFitToRequirements(Player*,uint32,uint32)const{return fit;}
};
using AreaMap=std::multimap<uint32,SpellArea const*>;
using SpellAreaForQuestMapBounds=std::pair<AreaMap::const_iterator,AreaMap::const_iterator>;
struct TestSpellMgr
{
    std::map<uint32,SpellEntry> spells;
    std::map<uint32,SpellTargetPosition> positions;
    AreaMap ended,started;
    SpellEntry const* GetSpellEntry(uint32 id)const
    {auto i=spells.find(id);return i==spells.end()?nullptr:&i->second;}
    SpellTargetPosition const* GetSpellTargetPosition(uint32 id)const
    {auto i=positions.find(id);return i==positions.end()?nullptr:&i->second;}
    SpellAreaForQuestMapBounds GetSpellAreaForQuestEndMapBounds(uint32 id)const{return ended.equal_range(id);}
    SpellAreaForQuestMapBounds GetSpellAreaForQuestMapBounds(uint32 id,bool)const{return started.equal_range(id);}
} sSpellMgr;
struct TestAccessor
{
    Player* player=nullptr;
    Player* FindPlayer(ObjectGuid guid){return player&&player->guid==guid.id?player:nullptr;}
} sObjectAccessor;
struct TestWorld {size_t announcements=0;void SendWorldText(int,char const*){++announcements;}} sWorld;
struct AtomicPlayerSaveScope
{
    static std::vector<std::function<void()>> actions;
    static void DeferAfterCommit(std::function<void()> action){actions.push_back(std::move(action));}
};
std::vector<std::function<void()>> AtomicPlayerSaveScope::actions;
struct SpellCastTargets {Player* target=nullptr;void setUnitTarget(Player* p){target=p;}};
class Spell
{
    SpellEntry const* entry;
    Player* target=nullptr;
    uint32 state=SPELL_STATE_PREPARING;
    bool success=false;
public:
    static std::vector<Spell*> events;
    static size_t forced;
    Spell(Unit*,SpellEntry const* info,bool):entry(info){events.push_back(this);}
    int prepare(SpellCastTargets targets)
    {
        target=targets.target;
        if(entry->failPrepare)return SPELL_CAST_FAILED;
        if(entry->castTime==0)cast(true);
        return SPELL_CAST_OK;
    }
    void cast(bool checked)
    {
        assert(checked&&target);
        if(entry->castTime)++forced;
        for(uint8 i=0;i<MAX_EFFECT_INDEX;++i)
            switch(entry->Effect[i])
            {
                case SPELL_EFFECT_LEARN_SPELL:target->learned.insert(entry->EffectTriggerSpell[i]);break;
                case SPELL_EFFECT_CREATE_ITEM:++target->items;break;
                case SPELL_EFFECT_APPLY_AURA:target->auras.insert(entry->Id);break;
                case SPELL_EFFECT_ADD_HONOR:target->honor+=7;break;
                case SPELL_EFFECT_SKILL_STEP:++target->skills;break;
                case SPELL_EFFECT_TELEPORT_UNITS:target->pendingMap=sSpellMgr.positions.at(entry->Id).mapId;break;
            }
        state=entry->unfinished?SPELL_STATE_PREPARING:SPELL_STATE_FINISHED;
        success=!entry->unfinished;
    }
    uint32 getState()const{return state;}
    bool isSuccessCast()const{return success;}
};
std::vector<Spell*> Spell::events;
size_t Spell::forced=0;

#include "../../src/game/Objects/PlayerNativeQuestRewards.inc"

void ResetNative()
{
    for(Spell* spell:Spell::events)delete spell;
    Spell::events.clear();Spell::forced=0;
    sSpellMgr=TestSpellMgr{};sWorld=TestWorld{};
    AtomicPlayerSaveScope::actions.clear();
}
SpellEntry& AddSpell(uint32 effect)
{
    auto& entry=sSpellMgr.spells[7329];
    entry.Effect[0]=effect;entry.EffectImplicitTargetA[0]=TARGET_UNIT_FRIEND;
    entry.EffectTriggerSpell[0]=7328;
    return entry;
}

void TeachingAndLocalRewardsExecuteBeforeSnapshot()
{
    ResetNative();Player player;Unit giver;Quest quest;bool world=true;
    auto& teaching=AddSpell(SPELL_EFFECT_LEARN_SPELL);teaching.castTime=14000;
    assert(player._ApplyNativeQuestRewards(&quest,&giver,world));
    assert(!world&&player.learned.count(7328)&&Spell::forced==1);
    assert(Spell::events.back()->getState()==SPELL_STATE_FINISHED);
    ResetNative();Player local;bool localWorld=true;
    auto& reward=AddSpell(SPELL_EFFECT_CREATE_ITEM);
    reward.Effect[1]=SPELL_EFFECT_ADD_HONOR;reward.EffectImplicitTargetA[1]=TARGET_UNIT;
    reward.Effect[2]=SPELL_EFFECT_APPLY_AURA;reward.EffectImplicitTargetA[2]=TARGET_UNIT_FRIEND;
    assert(local._ApplyNativeQuestRewards(&quest,&giver,localWorld));
    assert(!localWorld&&local.items==1&&local.honor==7&&local.auras.count(7329));
}

void UnsafeWorldEffectsAreNotAppliedInCore()
{
    for(uint32 effect:{uint32(SPELL_EFFECT_DUMMY),uint32(SPELL_EFFECT_SUMMON_PET),uint32(SPELL_EFFECT_TRIGGER_SPELL),uint32(SPELL_EFFECT_SEND_EVENT)})
    {
        ResetNative();Player player;Unit giver;Quest quest;bool world=false;AddSpell(effect);
        assert(player._ApplyNativeQuestRewards(&quest,&giver,world)&&world&&Spell::events.empty());
    }
    for(int restriction=0;restriction!=5;++restriction)
    {
        ResetNative();Player player;Unit giver;Quest quest;bool world=false;
        auto& entry=AddSpell(SPELL_EFFECT_LEARN_SPELL);
        if(restriction==0)entry.speed=1;
        if(restriction==1)entry.channel=true;
        if(restriction==2)entry.cc=true;
        if(restriction==3)entry.EffectImplicitTargetA[0]=TARGET_UNIT_CASTER;
        if(restriction==4)entry.EffectImplicitTargetA[0]=TARGET_AREA;
        assert(player._ApplyNativeQuestRewards(&quest,&giver,world)&&world&&player.learned.empty());
    }
}

void CheckedCastAndAuraTitleMutations()
{
    for(bool unfinished:{false,true})
    {
        ResetNative();Player player;Unit giver;Quest quest;bool world=false;
        auto& entry=AddSpell(SPELL_EFFECT_LEARN_SPELL);
        entry.failPrepare=!unfinished;entry.unfinished=unfinished;entry.castTime=unfinished?1000:0;
        assert(!player._ApplyNativeQuestRewards(&quest,&giver,world));
    }
    ResetNative();Player player;Unit giver;Quest quest;quest.id=7787;quest.cast=0;
    sObjectAccessor.player=&player;player.earned.insert(TITLE_SEEKER_OF_KNOWLEDGE);
    player.auras.insert(100);SpellArea end{100,true,false},start{200,true,true};
    sSpellMgr.ended.emplace(7787,&end);sSpellMgr.started.emplace(7787,&start);
    SpellEntry aura;aura.Id=200;aura.Effect[0]=SPELL_EFFECT_APPLY_AURA;
    aura.EffectImplicitTargetA[0]=TARGET_UNIT_CASTER;sSpellMgr.spells.emplace(200,aura);
    bool world=false;assert(player._ApplyNativeQuestRewards(&quest,&giver,world));
    assert(!player.auras.count(100)&&player.auras.count(200));
    assert(player.titles.count(TITLE_STORMWIELDER)&&player.titles.count(TITLE_SEEKER_OF_KNOWLEDGE));
    assert(sWorld.announcements==0&&AtomicPlayerSaveScope::actions.size()==1);
    AtomicPlayerSaveScope::actions.front()();assert(sWorld.announcements==1);
}

void OnlyInstantSingleFarTeleportJoinsCore()
{
    for(int restriction=0;restriction!=5;++restriction)
    {
        ResetNative();Player player;Unit giver;Quest quest;bool world=false;
        auto& entry=AddSpell(SPELL_EFFECT_TELEPORT_UNITS);
        entry.EffectImplicitTargetB[0]=TARGET_LOCATION_DATABASE;
        if(restriction!=1)sSpellMgr.positions.emplace(7329,SpellTargetPosition{restriction==2?0u:1u});
        if(restriction==3)entry.castTime=1000;
        if(restriction==4)entry.Effect[1]=SPELL_EFFECT_LEARN_SPELL;
        assert(player._ApplyNativeQuestRewards(&quest,&giver,world));
        if(restriction==0)assert(!world&&player.pendingMap==1);
        else assert(world&&player.pendingMap==0);
    }
}

int main()
{
    TeachingAndLocalRewardsExecuteBeforeSnapshot();
    UnsafeWorldEffectsAreNotAppliedInCore();
    CheckedCastAndAuraTitleMutations();
    OnlyInstantSingleFarTeleportJoinsCore();
    ResetNative();
    std::cout<<"Production native quest reward orchestration tests passed.\n";
}
