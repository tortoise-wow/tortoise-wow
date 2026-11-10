// Compile the ACTUAL root game scope implementation with boundary-only aliases.
// No copied AtomicPlayerSave logic, SQL server, game process or credentials.
#include "atomic-scope-stubs/ScopeBoundary.h"
#pragma include_alias("Player.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("Mail/Mail.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("Database/DatabaseEnv.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("World.h", "atomic-scope-stubs/ScopeBoundary.h")
#pragma include_alias("Log.h", "atomic-scope-stubs/ScopeBoundary.h")
#include "../../src/game/AtomicPlayerSave.cpp"
using namespace ScopeTest;
void CollectMail(bool okay=true){auto mail=std::make_unique<PreparedMail>();mail->okay=okay;Check(MailDraft::GetPublicationCollector()&&MailDraft::GetPublicationCollector()->Collect(std::move(mail)),"actor mail collected");}
int main(){
    std::cout<<std::unitbuf;
    Reset();
    {
        Player a(1),b(2);
        AtomicPlayerSaveScope owner{&b,&a};Check(bool(owner)&&owner.IsOwner(),"pair outer scope owns SQL");
        {
            AtomicPlayerSaveScope nested{&a};Check(bool(nested)&&!nested.IsOwner(),"singleton nested scope joins pair");
            Check(nested.RequestFullSave(a)&&nested.RequestFullSave(a),"nested upgrades participant to full snapshot idempotently");
            CollectMail();AtomicPlayerSaveScope::DeferAfterCommit([&]{CheckDetached("callback runs with context off");++callbacks;events.push_back("callback");});
            Check(nested.Commit(),"nested commit stages without independent ACK");
        }
        Check(a.full==0&&a.inventory==0&&b.full==0&&b.inventory==0,"no early/duplicate snapshots before outer seal");
        Check(commits==0&&publications==0&&callbacks==0&&a.dirty&&b.dirty,"nothing publishes or cleans before outer ACK");
        beforeAck=[&]{Check(a.full==1&&a.inventory==0&&b.full==0&&b.inventory==1,"seal selects full A and inventory B exactly once");Check(publications==0&&callbacks==0&&a.dirty&&b.dirty,"mail/callback/dirty consumption wait for ACK");};
        Check(owner.Commit(),"outer scope commits acknowledged pair");
        Check(a.fullComplete==1&&a.inventoryComplete==0&&b.fullComplete==0&&b.inventoryComplete==1,"one matching completion per participant");
        Check(publications==1&&callbacks==1&&!a.dirty&&!b.dirty,"publication and cleanup after acknowledged commit");
        Check(events==std::vector<std::string>({"full1","inventory2","ack","complete-full1","complete-inventory2","mail","callback"}),"exact seal ACK complete publish callback order");
        Check(!owner.Commit(),"outer owner cannot commit twice");
    }
    Check(!fault&&stops==0,"successful owner destruction does not fault");
    std::cout<<"PASS nested pair/singleton full-versus-inventory seal and ACK order\n";
    for(bool failAck:{false,true}){
        Reset();Player a(1);a.appendOkay=failAck;ack=!failAck;
        {AtomicPlayerSaveScope scope{&a};Check(bool(scope),"failure fixture admitted");Check(scope.RequestFullSave(a),"full save requested");CollectMail();AtomicPlayerSaveScope::DeferAfterCommit([]{++callbacks;});
         Check(!scope.Commit(),"prepare/ACK failure propagates false");Check(a.dirty&&a.fullComplete==0&&a.inventoryComplete==0,"failure never clears dirty state");Check(publications==0&&callbacks==0,"failure never publishes mail/callback");}
        Check(fault&&stops==1,"failure fences persistence and stops world exactly once");Check(mailDestroyed==1,"failed actor mail ownership cleaned once");
        Check(commits==(failAck?1u:0u),"prepare failure never attempts COMMIT; ACK failure attempts once");
    }
    std::cout<<"PASS prepare and ambiguous/failed ACK containment\n";
    Reset();
    {Player a(1);busy=true;AtomicPlayerSaveScope scope{&a};Check(!scope&&!scope.IsOwner(),"busy DB admission refuses scope");Check(a.dirty&&a.full==0&&a.inventory==0,"busy admission occurs before mutation/snapshot");}
    Check(!fault&&stops==0&&commits==0,"busy admission does not fault or stop");
    Reset();
    {Player a(1);AtomicPlayerSaveScope scope{&a};Check(bool(scope),"abandoned owner created");CollectMail();}
    Check(fault&&stops==1&&abandoned==1&&publications==0&&mailDestroyed==1,"abandoned owner fences without mail publication");
    std::cout<<"PASS busy begin and abandoned owner lifecycle\n";
    Reset();
    {Player a(1),b(2);AtomicPlayerSaveScope owner{&a};AtomicPlayerSaveScope mismatch{&b};Check(!mismatch,"nested unregistered participant refused");Check(!owner.Commit(),"mismatched participant poisons owner");Check(a.full==0&&a.inventory==0&&publications==0,"mismatch never seals/publishes");}
    Check(fault&&stops==1,"participant mismatch failure fences world");
    Reset();
    {Player a(1),imposter(1);AtomicPlayerSaveScope owner{&a};Check(!owner.RequestFullSave(imposter),"same GUID but different Player identity cannot request snapshot");Check(!owner.Commit(),"wrong pointer snapshot request fails owner");}
    Check(fault&&stops==1,"wrong snapshot identity contained");
    std::cout<<"PASS participant and snapshot pointer mismatch\n";
    Reset();
    {
        Player a(1),b(2);AtomicPlayerSaveScope outer{&a};
        AtomicPlayerSaveScope::DeferAfterCommit([&]{CheckDetached("post-ACK action starts from detached context");AtomicPlayerSaveScope next{&b};Check(bool(next)&&next.IsOwner(),"post-ACK callback may own a new transaction");Check(next.RequestFullSave(b),"new scope requests full snapshot");Check(next.Commit(),"new scope commits independently");++callbacks;});
        Check(outer.Commit(),"outer ACK and reentrant actor action succeed");
        Check(begins==2&&commits==2&&a.inventoryComplete==1&&b.fullComplete==1&&callbacks==1,"new post-ACK scope is neither nested nor stolen");
    }
    Check(!fault&&stops==0,"new post-ACK scope leaves no fault");
    std::cout<<"PASS post-ACK action can begin new actor scope\n";
    Reset();
    {Player a(1);a.onAppend=[] {auto mail=std::make_unique<PreparedMail>();Check(!MailDraft::GetPublicationCollector()->Collect(std::move(mail)),"collection during seal is refused");};AtomicPlayerSaveScope owner{&a};Check(!owner.Commit(),"late mail during snapshot poisons seal");Check(a.dirty&&a.inventoryComplete==0&&commits==0&&publications==0,"late collect cannot escape failed seal");}
    Check(fault&&stops==1,"late seal mutation fenced");
    Reset();
    AtomicPlayerSaveScope::DeferAfterCommit([]{CheckDetached("outside-scope immediate callback has no context");++callbacks;});Check(callbacks==1,"outside-scope callback retains ordinary immediate behavior");
    std::cout<<checks<<" actual atomic actor scope checks passed\n";
    return 0;
}
