#pragma once
// Boundary-only fixtures. AtomicPlayerSave.cpp itself is included unchanged.
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "../../../src/game/AtomicPlayerSave.h"
using uint32 = uint32_t;
namespace ScopeTest {
inline unsigned checks=0,stops=0,publications=0,callbacks=0,mailDestroyed=0;
inline std::vector<std::string> events;
inline std::function<void()> beforeAck;
inline bool ack=true,busy=false,fault=false,owned=false;
inline unsigned begins=0,commits=0,abandoned=0;
inline std::set<uint32> participants;
inline void Check(bool condition,char const* label) {
    if(!condition){std::cerr<<"FAIL "<<label<<std::endl;std::abort();}++checks;
}
void CheckDetached(char const* label);
}
class PreparedMail {
public:
    bool okay=true;
    ~PreparedMail(){++ScopeTest::mailDestroyed;}
    bool Publish(){ScopeTest::CheckDetached("mail publication runs after TLS/collector/SQL deactivation");++ScopeTest::publications;ScopeTest::events.push_back("mail");return okay;}
};
class MailPublicationCollector {
public:
    virtual ~MailPublicationCollector()=default;
    virtual bool Collect(std::unique_ptr<PreparedMail>)=0;
    virtual void Fail()=0;
};
class MailDraft {
    static inline thread_local MailPublicationCollector* collector=nullptr;
public:
    static MailPublicationCollector* GetPublicationCollector(){return collector;}
    static MailPublicationCollector* SetPublicationCollector(MailPublicationCollector* value){auto old=collector;collector=value;return old;}
};
namespace ScopeTest {
inline void CheckDetached(char const* label){Check(!AtomicPlayerSaveScope::Active()&&!MailDraft::GetPublicationCollector()&&!owned,label);}
inline void Reset(){
    Check(!AtomicPlayerSaveScope::Active()&&!MailDraft::GetPublicationCollector(),"previous scope released TLS/collector");
    stops=publications=callbacks=mailDestroyed=begins=commits=abandoned=0;events.clear();beforeAck={};ack=true;busy=fault=owned=false;participants.clear();
}
}
class Database {
public:
    class OwnedTransaction {
        bool committed=false;
    public:
        ~OwnedTransaction(){if(!committed){++ScopeTest::abandoned;ScopeTest::owned=false;ScopeTest::participants.clear();}}
        bool Commit(){
            ++ScopeTest::commits;
            ScopeTest::Check(ScopeTest::owned&&AtomicPlayerSaveScope::Active()&&MailDraft::GetPublicationCollector(),"scope still active at real ACK boundary");
            if(ScopeTest::beforeAck)ScopeTest::beforeAck();
            ScopeTest::events.push_back("ack");committed=true;ScopeTest::owned=false;ScopeTest::participants.clear();return ScopeTest::ack;
        }
    };
    std::unique_ptr<OwnedTransaction> BeginOwnedTransaction(std::vector<uint32> guids){
        ++ScopeTest::begins;
        if(ScopeTest::busy||ScopeTest::fault||ScopeTest::owned)return nullptr;
        ScopeTest::owned=true;ScopeTest::participants={guids.begin(),guids.end()};return std::make_unique<OwnedTransaction>();
    }
    bool IsOwnedTransactionParticipant(uint32 guid){return ScopeTest::owned&&!ScopeTest::fault&&ScopeTest::participants.count(guid);}
    void MarkOrderedPersistenceFault(){ScopeTest::fault=true;}
};
inline Database CharacterDatabase;
class Player {
public:
    struct InventorySaveBatch {bool captured=false;};
    struct PlayerSaveBatch {bool captured=false;};
    uint32 guid;
    bool enabled=true,appendOkay=true,completeOkay=true,dirty=true;
    unsigned full=0,inventory=0,fullComplete=0,inventoryComplete=0;
    std::function<void()> onAppend;
    explicit Player(uint32 id):guid(id){}
    uint32 GetGUIDLow()const{return guid;}
    bool IsInventorySaveEnabled()const{return enabled;}
    bool AppendFullPlayerSave(PlayerSaveBatch& batch){
        ScopeTest::Check(ScopeTest::owned&&AtomicPlayerSaveScope::Active()&&MailDraft::GetPublicationCollector(),"full snapshot captured inside owned scope");
        ++full;batch.captured=true;ScopeTest::events.push_back("full"+std::to_string(guid));if(onAppend)onAppend();return appendOkay;
    }
    bool AppendInventoryAndGoldToDB(InventorySaveBatch& batch){
        ScopeTest::Check(ScopeTest::owned&&AtomicPlayerSaveScope::Active()&&MailDraft::GetPublicationCollector(),"inventory snapshot captured inside owned scope");
        ++inventory;batch.captured=true;ScopeTest::events.push_back("inventory"+std::to_string(guid));if(onAppend)onAppend();return appendOkay;
    }
    bool CompleteFullPlayerSave(PlayerSaveBatch& batch){
        ScopeTest::CheckDetached("full completion after TLS/collector/SQL deactivation");ScopeTest::Check(batch.captured,"complete captured full snapshot");
        ++fullComplete;ScopeTest::events.push_back("complete-full"+std::to_string(guid));if(completeOkay)dirty=false;return completeOkay;
    }
    bool CompleteInventoryAndGoldSave(InventorySaveBatch& batch){
        ScopeTest::CheckDetached("inventory completion after TLS/collector/SQL deactivation");ScopeTest::Check(batch.captured,"complete captured inventory snapshot");
        ++inventoryComplete;ScopeTest::events.push_back("complete-inventory"+std::to_string(guid));if(completeOkay)dirty=false;return completeOkay;
    }
};
constexpr int ERROR_EXIT_CODE=1;
struct World {static void StopNow(int code){ScopeTest::Check(code==ERROR_EXIT_CODE,"failure exits with error code");ScopeTest::CheckDetached("world stop occurs after context cleanup");++ScopeTest::stops;}};
struct TestLog {template<class... Args>void outError(char const*,Args...) {}};
inline TestLog sLog;
