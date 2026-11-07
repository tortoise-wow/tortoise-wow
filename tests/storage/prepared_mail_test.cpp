// Execute production PreparedMail.inc with real Mail.h declarations and small
// actor/SQL boundaries. This is not a second implementation of mail persistence.
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include "../../src/game/Mail/Mail.h"

namespace { unsigned checks=0; void Check(bool okay,char const* label) { if(!okay){std::cerr<<"FAIL "<<label<<'\n';std::abort();} ++checks; } }
struct RecordedSql { std::string text; std::vector<std::string> values; };
struct SqlStatementID {};
struct TestDatabase;
struct SqlStatement {
    TestDatabase* db; RecordedSql sql;
    void addUInt32(uint32 value){sql.values.push_back(std::to_string(value));}
    void addUInt8(uint8 value){addUInt32(value);}
    void addUInt16(uint16 value){addUInt32(value);}
    void addUInt64(uint64 value){sql.values.push_back(std::to_string(value));}
    void addString(std::string const& value){sql.values.push_back(value);}
    bool Execute();
};
struct TestDatabase {
    bool owned=true,transaction=true,fault=false,failed=false,commitOkay=true;
    std::set<uint32> participants{1};
    std::vector<RecordedSql> queued;
    unsigned begin=0,commit=0,direct=0,rollback=0,attempts=0,failAt=0;
    bool InOwnedTransaction()const{return owned;}
    bool InTransaction()const{return transaction;}
    bool HasOrderedPersistenceFault()const{return fault;}
    bool IsOwnedTransactionParticipant(uint32 guid)const{return owned&&!failed&&participants.count(guid);}
    void MarkOrderedPersistenceFault(){fault=true;}
    bool BeginTransaction(uint32){++begin; if(transaction){failed=true;return false;} transaction=true;return true;}
    bool CommitTransaction(){++commit; transaction=false;return commitOkay;}
    bool CommitTransactionDirect(){++direct;transaction=false;return commitOkay;}
    bool RollbackTransaction(){++rollback; transaction=false;queued.clear();return true;}
    SqlStatement CreateStatement(SqlStatementID&,char const* sql){return {this,{sql,{}}};}
    bool Enqueue(RecordedSql sql){++attempts;if(fault||failed||(failAt&&attempts==failAt)){failed=true;return false;}Check(transaction,"SQL append requires an existing transaction");queued.push_back(std::move(sql));return true;}
    bool PExecute(char const* format,...){char sql[512];va_list args;va_start(args,format);vsnprintf(sql,sizeof(sql),format,args);va_end(args);return Enqueue({sql,{}});}
    void Ack(bool success){Check(owned&&transaction,"outer actor owns ACK boundary");owned=false;transaction=false;if(!success){fault=true;queued.clear();}}
} CharacterDatabase;
bool SqlStatement::Execute(){return db->Enqueue(std::move(sql));}
enum ItemUpdateState {ITEM_UNCHANGED,ITEM_CHANGED,ITEM_NEW,ITEM_REMOVED};
class Item {
public:
    static unsigned created,deleted,legacySaves,completions,next;
    uint32 guid,entry,count; ObjectGuid owner; ItemUpdateState state=ITEM_NEW;
    bool world=false,container=false,queued=false;
    Item(uint32 id,uint32 n,ObjectGuid who):guid(++next),entry(id),count(n),owner(who){++created;}
    ~Item(){++deleted;}
    uint32 GetGUIDLow()const{return guid;}uint32 GetEntry()const{return entry;}uint32 GetCount()const{return count;}
    ObjectGuid GetOwnerGuid()const{return owner;}void SetOwnerGuid(ObjectGuid value){owner=value;}
    ObjectGuid GetObjectGuid()const{return ObjectGuid(guid);}
    ItemUpdateState GetState()const{return state;}bool IsInWorld()const{return world;}void* GetContainer(){return container?this:nullptr;}bool IsInUpdateQueue()const{return queued;}
    bool AppendSaveToDB(){return CharacterDatabase.IsOwnedTransactionParticipant(owner.GetCounter())&&CharacterDatabase.Enqueue({"ITEM",{std::to_string(guid),std::to_string(entry),std::to_string(count),std::to_string(owner.GetCounter())}});}
    void SaveToDB(){++legacySaves;if(CharacterDatabase.Enqueue({"ITEM",{std::to_string(guid),std::to_string(entry),std::to_string(count),std::to_string(owner.GetCounter())}}))state=ITEM_UNCHANGED;}
    void CompleteSaveToDB(){++completions;state=ITEM_UNCHANGED;}
    Item* CloneItem(uint32 n)const{return new Item(entry,n,ObjectGuid());}
    static Item* CreateItem(uint32 id,uint32 n,Player* player);
};
unsigned Item::created=0,Item::deleted=0,Item::legacySaves=0,Item::completions=0,Item::next=100;
struct Session;
class MasterPlayer {
public:
    Session* session=nullptr;Session* GetSession()const{return session;}
    std::map<uint32,Item*> items;std::vector<Mail*> mails;unsigned notifications=0;
    ~MasterPlayer(){for(auto& entry:items)delete entry.second;for(auto* mail:mails)delete mail;}
    void AddMItem(Item* item){items[item->GetGUIDLow()]=item;}
    Item* GetMItem(uint32 guid){auto i=items.find(guid);return i==items.end()?nullptr:i->second;}
    void AddMail(Mail* mail){mails.push_back(mail);}
    Mail* GetMail(uint32 id){for(auto* mail:mails)if(mail->messageID==id)return mail;return nullptr;}
    void AddNewMailDeliverTime(time_t){++notifications;}
};
struct Session { MasterPlayer* master;Player* player;MasterPlayer* GetMasterPlayer(){return master;}Player* GetPlayer(){return player;} };
class Player {
public: uint32 guid;Session session;std::map<uint32,Item*> inventory;
    Player(uint32 id,MasterPlayer* master):guid(id),session{master,this}{if(master)master->session=&session;}
    uint32 GetGUIDLow()const{return guid;}ObjectGuid GetObjectGuid()const{return ObjectGuid(guid);}Session* GetSession(){return &session;}
    Item* GetItemByGuid(ObjectGuid value)const{auto i=inventory.find(value.GetCounter());return i==inventory.end()?nullptr:i->second;}
};
Item* Item::CreateItem(uint32 id,uint32 n,Player* player){return new Item(id,n,player?player->GetObjectGuid():ObjectGuid());}
struct TestObjectMgr {
    uint32 mail=1000,nextText=2000;std::map<uint32,std::string> texts;std::map<uint32,Player*> players;std::set<uint32> offline;
    uint32 GenerateMailID(){return ++mail;}uint32 GenerateItemTextID(){return ++nextText;}
    std::string GetItemText(uint32 id){return texts[id];}void AddItemText(uint32 id,std::string text){texts[id]=std::move(text);}
    Player* GetPlayer(ObjectGuid guid){auto p=players.find(guid.GetCounter());return p==players.end()?nullptr:p->second;}
    uint32 GetPlayerAccountIdByGUID(ObjectGuid guid){return offline.count(guid.GetCounter())?10:0;}
} sObjectMgr;
struct TestObjectAccessor { std::map<uint32,MasterPlayer*> masters;MasterPlayer* FindMasterPlayer(ObjectGuid guid){auto i=masters.find(guid.GetCounter());return i==masters.end()?nullptr:i->second;} } sObjectAccessor;
struct LootItem {uint32 itemid,count;};
std::vector<LootItem> templateItems;
constexpr int LootTemplates_Mail=0;
struct Loot {
    explicit Loot(void*){}void FillLoot(uint32,int,Player*,bool,bool){}
    uint32 GetMaxSlotInLootFor(uint32){return uint32(templateItems.size());}
    LootItem* LootItemInSlot(uint32 index,uint32){return &templateItems[index];}
};
class MailScript {
public:virtual ~MailScript()=default;virtual void OnBeforeMailDraftSendMailTo(MailDraft*,MailReceiver const&,MailSender const&){}
};
std::function<void(MailDraft*)> beforeSend;
template<class T> struct ScriptRegistry {template<class F>static void ForEach(F callback){struct Hook:T {void OnBeforeMailDraftSendMailTo(MailDraft* draft,MailReceiver const&,MailSender const&)override{if(beforeSend)beforeSend(draft);}} hook;callback(&hook);}};
MailReceiver::MailReceiver(Player* receiver):m_receiver(receiver),m_receiver_guid(receiver->GetObjectGuid()){}
MailReceiver::MailReceiver(Player* receiver,ObjectGuid guid):m_receiver(receiver),m_receiver_guid(guid){}
#include "../../src/game/Mail/PreparedMail.inc"
struct Collector:MailPublicationCollector {
    bool failed=false,accept=true;std::vector<std::unique_ptr<PreparedMail>> mails;
    bool Collect(std::unique_ptr<PreparedMail> mail)override{if(!accept)return false;mails.push_back(std::move(mail));return true;}
    void Fail()override{failed=true;}
    bool PublishAll(){for(auto& mail:mails)if(!mail->Publish())return false;return true;}
};
struct Fixture {
    MasterPlayer master;Player player{1,&master};Collector collector;
    Fixture(){CharacterDatabase=TestDatabase{};sObjectMgr=TestObjectMgr{};sObjectAccessor.masters.clear();templateItems.clear();beforeSend={};sObjectMgr.players[1]=&player;sObjectAccessor.masters[1]=&master;MailDraft::SetPublicationCollector(&collector);Item::legacySaves=Item::completions=0;}
    ~Fixture(){MailDraft::SetPublicationCollector(nullptr);}
};
int main(){
    {
        Fixture f;templateItems={{42,2}};
        MailDraft draft(uint16(7));draft.SetMoney(50).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u),MAIL_CHECK_MASK_HAS_BODY,120,0,true);
        Check(!f.collector.failed&&f.collector.mails.size()==1,"owned template mail collected once");
        Check(CharacterDatabase.begin==0&&CharacterDatabase.commit==0&&CharacterDatabase.direct==0,"owned direct flag cannot steal transaction");
        Check(Item::legacySaves==0&&Item::completions==0,"owned attachments neither saved eagerly nor consumed before ACK");
        Check(f.master.mails.empty()&&f.master.items.empty()&&f.master.notifications==0,"no pre-ACK mailbox/item publication");
        Check(CharacterDatabase.queued.size()==3&&CharacterDatabase.queued[0].text=="ITEM","item+mail+link appended as one batch");
        Check(CharacterDatabase.Enqueue({"UPDATE quest rewarded",{}}),"quest completion joins same batch");
        CharacterDatabase.Ack(true);Check(f.collector.PublishAll(),"owned mail publishes after ACK");
        Check(f.master.mails.size()==1&&f.master.items.size()==1&&f.master.notifications==1,"one mailbox delivery after ACK");
        Check(f.master.mails[0]->money==50&&f.master.mails[0]->has_items,"mail values preserved");
        Check(f.collector.mails[0]->GetMailId()==f.master.mails[0]->messageID,"prepared metadata remains valid after publication");
    }
    {
        Fixture f;MailDraft draft("quoted ' subject","large body \"with\" text");
        Check(CharacterDatabase.queued.empty()&&sObjectMgr.texts.empty(),"body constructor has no SQL/cache publication");
        draft.SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(sObjectMgr.texts.empty()&&f.master.mails.empty(),"body text cache waits for ACK");
        Check(CharacterDatabase.queued.size()==2&&CharacterDatabase.queued[0].text.find("INSERT INTO item_text")==0,"body row joins mail batch");
        Check(CharacterDatabase.queued[0].values[1]=="large body \"with\" text","body copied into SQL parameters");
        CharacterDatabase.Ack(true);Check(f.collector.PublishAll(),"body mail publishes");Check(sObjectMgr.texts.size()==1,"body cache published after ACK");
    }
    for(unsigned fail=1;fail<=4;++fail){
        Fixture f;templateItems={{42,1}};CharacterDatabase.failAt=fail;
        MailDraft("reward","body").SetMailTemplate(7).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(f.collector.failed&&CharacterDatabase.fault,"every append failure faults collector/database");
        Check(f.master.mails.empty()&&f.master.items.empty()&&f.master.notifications==0&&sObjectMgr.texts.empty(),"failed append publishes nothing");
    }
    {
        Fixture f;templateItems={{42,1}};MailDraft(uint16(7)).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        unsigned deleted=Item::deleted;CharacterDatabase.Ack(false);f.collector.mails.clear();
        Check(Item::deleted==deleted+1,"failed outer ACK deletes only detached actor object, no DB delete");
        Check(f.master.mails.empty()&&f.master.notifications==0,"unknown/failed COMMIT never publishes");
    }
    {
        Fixture f;MailDraft::SetPublicationCollector(nullptr);
        MailDraft("no collector").SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(CharacterDatabase.fault&&CharacterDatabase.queued.empty()&&f.master.mails.empty(),"owned mail without actor collector fails closed");
    }
    {
        Fixture f;templateItems.clear();MailDraft(uint16(7)).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        CharacterDatabase.Ack(true);Check(f.collector.PublishAll(),"empty online template publishes");
        Check(f.master.mails[0]->has_items&&f.master.items.empty(),"empty roll is marked completed without invented attachment");
    }
    {
        Fixture f;MailDraft(uint16(7)).SendMailTo(MailReceiver(ObjectGuid(1)),MailSender(MAIL_CREATURE,11u));
        CharacterDatabase.Ack(true);Check(f.collector.PublishAll(),"offline template mail publishes metadata");
        Check(!f.master.mails[0]->has_items&&f.master.items.empty(),"offline template remains lazy");
        CharacterDatabase.owned=true;CharacterDatabase.transaction=true;CharacterDatabase.queued.clear();templateItems={{42,3}};
        f.master.mails[0]->prepareTemplateItems(&f.player);
        Check(!f.master.mails[0]->has_items&&f.master.items.empty(),"owned lazy generation waits for ACK");
        CharacterDatabase.Ack(true);Check(f.collector.mails.back()->Publish(),"lazy generated attachment publishes after ACK");
        Check(f.master.mails.size()==1&&f.master.mails[0]->has_items&&f.master.items.size()==1,"lazy update keeps existing mail identity");
    }
    {
        Fixture f;CharacterDatabase.owned=false;CharacterDatabase.transaction=true;MailDraft::SetPublicationCollector(nullptr);
        Item* external=new Item(42,1,ObjectGuid(99));external->state=ITEM_UNCHANGED;
        MailDraft("borrowed").AddItem(external).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_NORMAL,99u),MAIL_CHECK_MASK_NONE,0,0,true);
        Check(CharacterDatabase.begin==0&&CharacterDatabase.commit==0&&CharacterDatabase.direct==0&&CharacterDatabase.transaction,"legacy borrowed direct send neither begins nor commits");
        Check(Item::legacySaves==0&&f.master.items.size()==1,"existing legacy attachment not re-saved over owner override");
        Check(f.master.mails.size()==1,"legacy borrowed optimistic publication retained");
    }
    {
        Fixture f;CharacterDatabase.owned=false;CharacterDatabase.transaction=false;MailDraft::SetPublicationCollector(nullptr);templateItems={{42,1}};
        MailDraft(uint16(7)).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u),MAIL_CHECK_MASK_NONE,0,0,true);
        Check(CharacterDatabase.begin==1&&CharacterDatabase.direct==1&&CharacterDatabase.commit==0,"standalone owns one transaction and honors direct flag");
        Check(Item::legacySaves==1&&f.master.items.size()==1,"legacy generated NEW attachment persisted under transaction");
    }
    {
        Fixture f;CharacterDatabase.owned=false;CharacterDatabase.transaction=true;MailDraft::SetPublicationCollector(nullptr);
        Item* returned=new Item(42,1,ObjectGuid(99));returned->state=ITEM_CHANGED;
        MailDraft("return").AddItem(returned).SendReturnToSender(9,ObjectGuid(99),ObjectGuid(1));
        Check(CharacterDatabase.begin==0&&CharacterDatabase.commit==0&&CharacterDatabase.transaction,"return mail borrows legacy owner");
        Check(CharacterDatabase.queued[0].text=="ITEM"&&CharacterDatabase.queued[1].text.find("UPDATE item_instance SET owner_guid = '1'")==0,"return owner override follows item serialization");
    }
    {
        Fixture f;beforeSend=[](MailDraft* draft){draft->SetMoney(777);};
        MailDraft("hook").SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        CharacterDatabase.Ack(true);Check(f.collector.PublishAll()&&f.master.mails[0]->money==777,"actor hook runs before copied values are captured");
    }
    for(int attachment=0;attachment<3;++attachment){
        Fixture f;Item* live=new Item(42,1,ObjectGuid(1));live->queued=attachment==0;live->world=attachment==1;live->container=attachment==2;unsigned deleted=Item::deleted;
        MailDraft("invalid live attachment").AddItem(live).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(f.collector.failed&&CharacterDatabase.queued.empty(),"live inventory pointer cannot be adopted into mail");
        Check(Item::deleted==deleted&&(live->queued||live->world||live->container),"failed preparation preserves existing inventory/world/container ownership");delete live;
    }
    {
        Fixture f;MasterPlayer other;Item* foreign=new Item(42,1,ObjectGuid(99));foreign->state=ITEM_UNCHANGED;other.AddMItem(foreign);sObjectAccessor.masters[99]=&other;unsigned deleted=Item::deleted;
        MailDraft("foreign attachment").AddItem(foreign).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(f.collector.failed&&CharacterDatabase.queued.empty(),"foreign actor attachment rejected before SQL");
        Check(Item::deleted==deleted&&other.GetMItem(foreign->GetGUIDLow())==foreign,"foreign master retains rejected attachment ownership");
    }
    {
        Fixture f;Item* attached=new Item(42,1,ObjectGuid(1));attached->state=ITEM_UNCHANGED;f.master.AddMItem(attached);unsigned deleted=Item::deleted;
        MailDraft("unsupported receiver").AddItem(attached).SendMailTo(MailReceiver(ObjectGuid(2)),MailSender(MAIL_CREATURE,11u));
        Check(f.collector.failed&&CharacterDatabase.queued.empty(),"unregistered receiver rejected before pointer adoption");
        Check(Item::deleted==deleted&&f.master.GetMItem(attached->GetGUIDLow())==attached,"source mailbox survives unregistered recipient failure");
    }
    {
        Fixture f;MasterPlayer other;Player otherActor(2,&other);sObjectAccessor.masters[2]=&other;CharacterDatabase.participants.insert(2);
        Item* attached=new Item(42,1,ObjectGuid(2));attached->state=ITEM_UNCHANGED;uint32 guid=attached->GetGUIDLow();otherActor.inventory[guid]=attached;unsigned deleted=Item::deleted;
        MailDraft("other participant inventory").AddItem(attached).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(f.collector.failed&&CharacterDatabase.queued.empty(),"registered peer inventory is not a detached mail attachment");
        Check(Item::deleted==deleted&&otherActor.GetItemByGuid(ObjectGuid(guid))==attached,"peer actor retains clean inventory pointer after rejection");
        otherActor.inventory.clear();delete attached;
    }
    {
        Fixture f;f.collector.accept=false;templateItems={{42,1}};unsigned deleted=Item::deleted;
        MailDraft(uint16(7)).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(f.collector.failed&&CharacterDatabase.fault,"collector rejection fails outer context");
        Check(Item::deleted==deleted+1&&f.master.mails.empty(),"rejected prepared attachment has one actor owner");
    }
    {
        Fixture f;bool nested=false;
        beforeSend=[&](MailDraft*){if(nested)return;nested=true;MailDraft("nested level mail","inferno body").AddItem(new Item(43,1,ObjectGuid(1))).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));};
        MailDraft("outer quest mail").SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        Check(f.collector.mails.size()==2&&f.master.mails.empty(),"nested actor mail joins collector without early publication");
        Check(CharacterDatabase.begin==0&&CharacterDatabase.commit==0&&CharacterDatabase.direct==0,"nested mail never takes transaction ownership");
        CharacterDatabase.Ack(true);Check(f.collector.PublishAll(),"nested and outer mail publish under one ACK");
        Check(f.master.mails.size()==2&&f.master.items.size()==1&&f.master.notifications==2,"nested mail delivered exactly once");
    }
    {
        Fixture f;auto existing=new Mail{};existing->messageID=55;existing->mailTemplateId=7;existing->receiverGuid=ObjectGuid(1);f.master.AddMail(existing);
        existing->prepareTemplateItems(&f.player);Check(!existing->has_items,"lazy empty roll is not published before ACK");
        CharacterDatabase.Ack(true);Check(f.collector.PublishAll()&&existing->has_items,"lazy empty roll marked generated after ACK");
        unsigned attempts=CharacterDatabase.attempts;existing->prepareTemplateItems(&f.player);
        Check(CharacterDatabase.attempts==attempts,"completed empty lazy roll is never repeated");
    }
    {
        Fixture f;CharacterDatabase.owned=false;CharacterDatabase.transaction=false;MailDraft::SetPublicationCollector(nullptr);sObjectAccessor.masters.clear();
        Item* external=new Item(42,1,ObjectGuid(99));external->state=ITEM_UNCHANGED;unsigned deleted=Item::deleted;
        MailDraft("offline existing attachment").AddItem(external).SendMailTo(MailReceiver(ObjectGuid(2)),MailSender(MAIL_NORMAL,99u));
        Check(Item::deleted==deleted+1&&Item::legacySaves==0,"offline legacy publication consumes RAM object without re-saving old owner");
        Check(CharacterDatabase.commit==1&&CharacterDatabase.begin==1,"offline standalone preserves legacy transaction ownership");
    }
    {
        Fixture f;CharacterDatabase.owned=false;CharacterDatabase.transaction=true;MailDraft::SetPublicationCollector(nullptr);templateItems={{42,1}};
        auto existing=new Mail{};existing->messageID=56;existing->mailTemplateId=7;existing->receiverGuid=ObjectGuid(1);f.master.AddMail(existing);
        existing->prepareTemplateItems(&f.player);
        Check(CharacterDatabase.begin==0&&CharacterDatabase.commit==0&&CharacterDatabase.direct==0&&CharacterDatabase.transaction,"legacy lazy generation borrows caller transaction");
        Check(existing->has_items&&f.master.items.size()==1,"legacy lazy generation keeps optimistic actor publication");
    }
    {
        Fixture f;templateItems={{42,1}};MailDraft(uint16(7)).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));
        unsigned deleted=Item::deleted;sObjectAccessor.masters.clear();CharacterDatabase.Ack(true);
        Check(f.collector.PublishAll()&&Item::deleted==deleted+1,"missing master after ACK discards only detached RAM attachment");
        Check(CharacterDatabase.queued.size()==3&&f.master.mails.empty()&&f.master.notifications==0,"offline persisted mail keeps SQL rows and sends no actor notification");
    }
    {
        Fixture f;CharacterDatabase.owned=false;CharacterDatabase.transaction=false;CharacterDatabase.commitOkay=false;MailDraft::SetPublicationCollector(nullptr);templateItems={{42,1}};
        MailDraft(uint16(7)).SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u),MAIL_CHECK_MASK_NONE,0,0,true);
        Check(CharacterDatabase.direct==1&&CharacterDatabase.fault,"failed standalone direct COMMIT is reported/fenced");
        Check(f.master.mails.empty()&&f.master.items.empty()&&f.master.notifications==0,"failed direct COMMIT never publishes mail");
    }
    {
        Fixture f;unsigned deleted=Item::deleted;
        {MailDraft prototype("clone","body");Item* item=new Item(42,1,ObjectGuid(1));prototype.AddItem(item);MailDraft clone;clone.CloneFrom(prototype);
        Check(CharacterDatabase.queued.empty()&&sObjectMgr.texts.empty(),"CloneFrom saves neither body nor attachment eagerly");
        clone.SendMailTo(MailReceiver(&f.player),MailSender(MAIL_CREATURE,11u));CharacterDatabase.Ack(true);Check(f.collector.PublishAll(),"clone is persisted/published through owner");delete item;}
        Check(Item::deleted>=deleted+1,"prototype and clone ownership remain distinct");
    }
    std::cout<<checks<<" prepared mail checks passed\n";
    return 0;
}
