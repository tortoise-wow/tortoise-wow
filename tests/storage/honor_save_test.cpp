// Production HonorSave.inc and real HonorMgr.h. Boundary-only player/DB/week
// inputs; no copied honor save algorithm, database, credentials or game process.
#include <cstdint>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
using uint8=uint8_t;using int8=int8_t;using uint16=uint16_t;using uint32=uint32_t;using int32=int32_t;using uint64=uint64_t;
class Player;class Unit;class QueryResult;
#include "../../src/game/HonorMgr.h"
namespace {unsigned checks=0;void Check(bool okay,char const* label){if(!okay){std::cerr<<"FAIL "<<label<<std::endl;std::abort();}++checks;}}
struct TestDatabase {
    bool owned=true,fault=false;std::set<uint32> participants{1};std::vector<std::string> sql;unsigned attempts=0,failAt=0;
    bool HasOrderedPersistenceFault()const{return fault;}bool InOwnedTransaction()const{return owned;}
    bool IsOwnedTransactionParticipant(uint32 guid)const{return owned&&participants.count(guid);}
    void MarkOrderedPersistenceFault(){fault=true;}
    bool PExecute(char const* format,...){++attempts;if(fault||(failAt&&attempts==failAt))return false;char buffer[2048];va_list args;va_start(args,format);vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);sql.push_back(buffer);return true;}
} CharacterDatabase;
class Player {public:uint32 guid;explicit Player(uint32 id):guid(id){}uint32 GetGUIDLow()const{return guid;}};
struct QueryResult {uint32 honor,conquest,weekly,week;};
void HonorMgr::LoadCurrency(QueryResult* row){m_spendableHonor=row->honor;m_conquestPoints=row->conquest;m_weeklySpendableHonor=row->weekly;m_currencyWeekBeginDay=row->week;}
struct TestWorld {uint32 day=100;uint32 GetGameDay()const{return day;}}sWorld;
HonorMaintenancer sHonorMaintenancer;
void HonorMaintenancer::SetMaintenanceDays(uint32 last,uint32 next){m_lastMaintenanceDay=last;m_nextMaintenanceDay=next;}
float finiteAlways(float value){return std::isfinite(value)?value:0.0f;}
#define MANGOS_ASSERT(value) assert(value)
#include "../../src/game/HonorSave.inc"
struct Fixture {
    Player player{1};HonorMgr honor{&player};
    Fixture(){CharacterDatabase=TestDatabase{};sWorld.day=100;sHonorMaintenancer.SetMaintenanceDays(100,107);QueryResult row{200,30,40,100};honor.LoadCurrency(&row);honor.SetRankPoints(1250.5f);honor.SetLastWeekCP(80.5f);honor.SetStanding(7);honor.SetLastWeekHK(13);honor.SetStoredHK(17);honor.SetStoredDK(2);HonorRankInfo highest;highest.rank=6;honor.SetHighestRank(highest);}
    void New(float cp=25.0f,uint8 state=STATE_NEW){honor.GetHonorCP().push_back(HonorCP{0,1,cp,100,QUEST,state});}
};
int main(){
    {
        Fixture f;f.New();f.New();f.New(5,STATE_UNCHANGED);HonorMgr::SaveBatch batch;
        Check(f.honor.AppendSaveToDB(batch),"honor append accepts registered owned actor");
        Check(CharacterDatabase.sql.size()==4,"duplicate NEW credits plus currency and stored row all appended");
        Check(CharacterDatabase.sql[0]==CharacterDatabase.sql[1],"identical credit entries remain two independent rows");
        Check(CharacterDatabase.sql[2].find("REPLACE INTO `character_pvp_currency`")==0,"currency row in same batch");
        Check(CharacterDatabase.sql[3].find("UPDATE `characters` SET `honorRankPoints`")==0,"stored rank row in same batch");
        Check(f.honor.GetHonorCP().front().state==STATE_NEW,"append does not consume CP state");
        Check(!f.honor.CompleteSaveToDB(batch),"completion before owned ACK refused");
        Check(f.honor.GetHonorCP().front().state==STATE_NEW,"early completion leaves NEW state");
        CharacterDatabase.owned=false;size_t sqlCount=CharacterDatabase.sql.size();
        Check(f.honor.CompleteSaveToDB(batch),"ACK completion succeeds");
        for(auto const& entry:f.honor.GetHonorCP())Check(entry.state==STATE_UNCHANGED,"only captured acknowledged CP becomes unchanged");
        Check(f.honor.GetHonorCP().size()==3,"completion preserves ordered credit list and duplicates");
        Check(CharacterDatabase.sql.size()==sqlCount,"completion issues no SQL");
        Check(!f.honor.CompleteSaveToDB(batch),"completion cannot be repeated");
        CharacterDatabase.owned=true;CharacterDatabase.sql.clear();HonorMgr::SaveBatch next;
        Check(f.honor.AppendSaveToDB(next)&&CharacterDatabase.sql.size()==2,"subsequent snapshot does not duplicate acknowledged CP");
    }
    for(unsigned failure=1;failure<=4;++failure){
        Fixture f;f.New();f.New(40);CharacterDatabase.failAt=failure;HonorMgr::SaveBatch batch;
        Check(!f.honor.AppendSaveToDB(batch),"every CP/currency/stored append failure propagates false");
        Check(f.honor.GetHonorCP().size()==2,"failed append preserves list");
        for(auto const& entry:f.honor.GetHonorCP())Check(entry.state==STATE_NEW,"failed append preserves every NEW credit");
        CharacterDatabase.owned=false;Check(!f.honor.CompleteSaveToDB(batch),"failed capture cannot be acknowledged");
    }
    {
        Fixture f;f.New();{HonorMgr::SaveBatch batch;Check(f.honor.AppendSaveToDB(batch),"capture before failed outer commit");}
        Check(f.honor.GetHonorCP().front().state==STATE_NEW,"abandoned/failed ACK batch consumes nothing");
    }
    {
        Fixture f;f.New();HonorMgr::SaveBatch batch;Check(f.honor.AppendSaveToDB(batch),"snapshot before scalar mutation");
        QueryResult changed{201,30,40,100};f.honor.LoadCurrency(&changed);CharacterDatabase.owned=false;
        Check(!f.honor.CompleteSaveToDB(batch)&&f.honor.GetHonorCP().front().state==STATE_NEW,"newer honor currency prevents dirty consumption");
    }
    {
        Fixture f;f.New();f.New(40);HonorMgr::SaveBatch batch;Check(f.honor.AppendSaveToDB(batch),"snapshot before last-entry mutation");
        f.honor.GetHonorCP().back().cp=41;CharacterDatabase.owned=false;
        Check(!f.honor.CompleteSaveToDB(batch),"changed last credit rejects whole completion");
        for(auto const& entry:f.honor.GetHonorCP())Check(entry.state==STATE_NEW,"validation happens before marking first credit");
    }
    {
        Fixture f;f.New();HonorMgr::SaveBatch batch;Check(f.honor.AppendSaveToDB(batch),"snapshot before appended credit");f.New();CharacterDatabase.owned=false;
        Check(!f.honor.CompleteSaveToDB(batch)&&f.honor.GetHonorCP().front().state==STATE_NEW,"new list entry prevents stale completion");
    }
    {
        Fixture f;f.New();HonorMgr::SaveBatch batch;Check(f.honor.AppendSaveToDB(batch),"snapshot before rank mutation");f.honor.SetRankPoints(1251);CharacterDatabase.owned=false;
        Check(!f.honor.CompleteSaveToDB(batch),"changed rank prevents stale completion");
    }
    {
        Fixture f;QueryResult row{200,30,99,90};f.honor.LoadCurrency(&row);HonorMgr::SaveBatch batch;
        Check(f.honor.AppendSaveToDB(batch),"week rollover snapshot accepted");
        Check(f.honor.GetWeeklySpendableHonor()==0&&CharacterDatabase.sql.front().find("200, 30, 0, 100")!=std::string::npos,"week normalized before currency capture");
        CharacterDatabase.owned=false;Check(f.honor.CompleteSaveToDB(batch),"rollover completion matches captured week");
    }
    {
        Fixture f;f.New();HonorMgr::SaveBatch batch;CharacterDatabase.participants.clear();Check(!f.honor.AppendSaveToDB(batch)&&CharacterDatabase.sql.empty(),"unregistered actor refused");
        CharacterDatabase.participants.insert(1);CharacterDatabase.fault=true;Check(!f.honor.AppendSaveToDB(batch)&&CharacterDatabase.sql.empty(),"faulted DB refused");
    }
    {
        Fixture f;f.New();f.honor.Save();Check(CharacterDatabase.fault&&CharacterDatabase.sql.empty()&&f.honor.GetHonorCP().front().state==STATE_NEW,"legacy Save inside owned scope refuses consumption");
    }
    {
        Fixture f;f.New();f.New(7,STATE_UNCHANGED);f.honor.GetHonorCP().push_back(HonorCP{0,1,3,100,QUEST,99});CharacterDatabase.owned=false;
        f.honor.Save();Check(CharacterDatabase.sql.size()==3,"ordinary Save preserves NEW plus currency/stored serialization");
        Check(f.honor.GetHonorCP().size()==2&&f.honor.GetHonorCP().front().state==STATE_UNCHANGED,"ordinary Save preserves historical optimistic consumption/drop semantics");
    }
    {
        Fixture f;f.New();HonorMgr::SaveBatch batch;Check(f.honor.AppendSaveToDB(batch),"first capture admitted");Check(!f.honor.AppendSaveToDB(batch),"same batch cannot append twice");
        Player otherPlayer(1);HonorMgr other(&otherPlayer);CharacterDatabase.owned=false;Check(!other.CompleteSaveToDB(batch),"same GUID different honor owner cannot complete capture");
    }
    {
        Fixture f;f.New();f.honor.GetHonorCP().front().state=99;HonorMgr::SaveBatch batch;Check(!f.honor.AppendSaveToDB(batch),"invalid CP state fails closed");Check(f.honor.GetHonorCP().front().state==99,"invalid state not erased before ACK");
    }
    std::cout<<checks<<" honor append/ACK checks passed\n";
    return 0;
}
