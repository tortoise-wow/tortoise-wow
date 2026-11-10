// SPDX-License-Identifier: GPL-2.0-or-later
// Actual Database/SqlOperations/SqlDelayThread with an in-process connection;
// no SQL server, network, credentials, game process, or gameplay objects.
#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Database/SqlOperations.h"
#include "Database/DatabaseImpl.h"
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <algorithm>
#include <new>
#include <cstdlib>

// Deterministic actor-thread allocation failure at OwnedTransaction::Commit.
thread_local bool failNextAllocation = false;
void* operator new(std::size_t size)
{
    if (failNextAllocation) { failNextAllocation=false; throw std::bad_alloc(); }
    if (void* memory=std::malloc(size ? size : 1)) return memory;
    throw std::bad_alloc();
}

using namespace std::chrono_literals;
namespace {
std::atomic<unsigned> checks{0};
void Check(bool okay, const char* message) {
    if (!okay) { std::cerr << "FAIL " << message << std::endl; std::_Exit(1); }
    ++checks;
}
struct State {
    std::mutex mutex;
    std::condition_variable changed;
    std::vector<std::string> events;
    bool blocked = false, release = false, blockCommit = false, commitEntered = false;
    std::atomic<unsigned> connections{0};
    bool Record(std::string event) {
        std::unique_lock<std::mutex> lock(mutex);
        events.push_back(event);
        if (event == "UPDATE BLOCK" || (event == "COMMIT" && blockCommit)) {
            blocked = true; if (event == "COMMIT") commitEntered = true;
            changed.notify_all(); changed.wait(lock,[&] { return release; });
        }
        changed.notify_all(); return event != "UPDATE FAIL";
    }
    void WaitBlocked() { std::unique_lock<std::mutex> lock(mutex); Check(changed.wait_for(lock,2s,[&] { return blocked; }),"executor reached block"); }
    void Release() { std::lock_guard<std::mutex> lock(mutex); release = true; changed.notify_all(); }
    std::vector<std::string> Events() { std::lock_guard<std::mutex> lock(mutex); return events; }
};
class Result : public QueryResult {
public: Result() : QueryResult(1,0) {} bool NextRow() override { return false; }
};
class Connection : public SqlConnection {
    std::shared_ptr<State> m_state;
public:
    Connection(Database& db, std::shared_ptr<State> state) : SqlConnection(db),m_state(std::move(state)) { ++m_state->connections; }
    bool OpenConnection(bool) override { return true; }
    QueryResult* Query(const char* sql) override { m_state->Record(sql); return new Result; }
    QueryNamedResult* QueryNamed(const char* sql) override { m_state->Record(sql); return new QueryNamedResult(new Result,{}); }
    bool Execute(const char* sql) override { return m_state->Record(sql); }
    bool ExecuteMultiline(const char* sql) override { return m_state->Record(sql); }
    bool ExecuteStmt(int index, const SqlStmtParameters&) override { return m_state->Record("PREPARED " + m_db.GetStmtString(index)); }
    bool BeginTransaction() override { return m_state->Record("BEGIN"); }
    bool CommitTransaction() override { return m_state->Record("COMMIT"); }
    bool RollbackTransaction() override { return m_state->Record("ROLLBACK"); }
};
class FixtureDatabase : public Database {
    std::shared_ptr<State> m_state;
    SqlConnection* CreateConnection() override { return new Connection(*this,m_state); }
public:
    explicit FixtureDatabase(std::shared_ptr<State> state) :m_state(std::move(state)) {
        Check(ConfigureOrderedPersistence(),"configure before Initialize");
        Check(Initialize("OrderedFixture","fake;0;;;",1,4),"initialize fake connection workers");
        Check(m_numAsyncWorkers==1,"ordered mode uses exactly one executor");
        Check(EnableOwnedTransactions(),"permission can enable before AllowAsyncTransactions");
        Check(!BeginOwnedTransaction({1,2}),"owned API unavailable before runtime");
        AllowAsyncTransactions();
    }
    ~FixtureDatabase() override { StopServer(); }
    SqlResultQueue* Results() { return m_pResultQueue; }
};
template<class T> T Ready(std::future<T>& future, const char* message) {
    Check(future.wait_for(2s)==std::future_status::ready,message); return future.get();
}
void Barrier(FixtureDatabase& db) { std::unique_ptr<QueryResult> result(db.Query("SELECT barrier")); Check(bool(result),"read barrier completed"); }
struct Callback : MaNGOS::IQueryCallback {
    std::atomic<unsigned>& called; SqlQueryHolder* holder; QueryResult* result = nullptr;
    bool expectNull;
    Callback(std::atomic<unsigned>& calls,SqlQueryHolder* h,bool nullResult=true) :called(calls),holder(h),expectNull(nullResult) { threadSafe=false; }
    void Execute() override { Check((holder->GetResult(0)==nullptr)==expectNull,"login holder result matches execution/cancellation"); holder->DeleteAllResults(); ++called; }
    void SetResult(QueryResult* value) override { result=value; }
    QueryResult* GetResult() override { return result; }
};
}
int main(int, char**) {
    std::cout << std::unitbuf;
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        Check(!db.ConfigureOrderedPersistence(),"cannot configure over initialized worker pool");
        Check(db.BeginTransaction(11),"prior GUID transaction begins"); Check(db.Execute("UPDATE prior GUID11"),"prior SQL appends"); Check(db.CommitTransaction(),"prior GUID transaction admitted");
        Check(db.Execute("UPDATE global"),"global SQL admitted");
        std::atomic<unsigned> logins{0}; SqlQueryHolder priorLogin(22),laterLogin(11);
        priorLogin.SetSize(1); laterLogin.SetSize(1);
        Check(priorLogin.SetQuery(0,"SELECT login before pair"),"prior login initialized");
        Check(laterLogin.SetQuery(0,"SELECT login after pair"),"later login initialized");
        Check(priorLogin.Execute(new Callback(logins,&priorLogin,false),&db,db.Results()),"prior login admitted before owned batch");
        auto batch=db.BeginOwnedTransaction({22,11}); Check(bool(batch),"pair builder created");
        Check(db.IsOwnedTransactionParticipant(11)&&db.IsOwnedTransactionParticipant(22)&&!db.IsOwnedTransactionParticipant(33),"both distinct participants owned");
        Check(db.Execute("UPDATE pair11"),"pair first append"); Check(db.Execute("UPDATE pair22"),"pair second append"); Check(batch->Commit(),"pair actual COMMIT acknowledged");
        Check(laterLogin.Execute(new Callback(logins,&laterLogin,false),&db,db.Results()),"later login admitted after owned batch");
        Check(db.BeginTransaction(22),"later GUID transaction begins"); Check(db.Execute("UPDATE later GUID22"),"later append"); Check(db.CommitTransaction(),"later GUID transaction admitted");
        Check(db.DirectExecute("UPDATE direct"),"direct execution waits at FIFO tail");
        Check(db.BeginTransaction(11),"direct commit builder begins"); Check(db.Execute("UPDATE direct transaction"),"direct transaction append"); Check(db.CommitTransactionDirect(),"direct commit waits at FIFO tail");
        SqlStatementID id; auto stmt=db.CreateStatement(id,"UPDATE prepared SET value=?"); stmt.addUInt32(7); Check(stmt.DirectExecute(),"direct prepared waits in FIFO");
        Barrier(db);
        db.ProcessResultQueue(0); Check(logins==2,"healthy login holders complete once");
        auto events=state->Events();
        Check(events==std::vector<std::string>({"BEGIN","UPDATE prior GUID11","COMMIT","UPDATE global","SELECT login before pair","BEGIN","UPDATE pair11","UPDATE pair22","COMMIT","SELECT login after pair","BEGIN","UPDATE later GUID22","COMMIT","UPDATE direct","BEGIN","UPDATE direct transaction","COMMIT","PREPARED UPDATE prepared SET value=?","SELECT barrier"}),"global/GUID/pair/direct/query exact FIFO order");
        Check(!db.HasOrderedPersistenceFault(),"healthy work leaves no fault");
        std::cout<<"PASS unified global, GUID, pair, direct, prepared and read FIFO\n";
    }
    {
        auto state=std::make_shared<State>(); state->blockCommit=true; FixtureDatabase db(state);
        std::promise<bool> committed; auto done=committed.get_future();
        std::thread actor([&] { auto batch=db.BeginOwnedTransaction({1,2}); Check(bool(batch),"actor owns batch"); Check(db.Execute("UPDATE pair"),"actor append"); committed.set_value(batch->Commit()); });
        state->WaitBlocked(); Check(done.wait_for(20ms)==std::future_status::timeout,"owned result waits for real COMMIT");
        Check(db.Execute("UPDATE after blocked commit"),"admission gate released before owner waits");
        state->Release(); Check(Ready(done,"owned ACK resolved"),"owned commit true"); actor.join(); Barrier(db);
        auto events=state->Events(); Check(events==std::vector<std::string>({"BEGIN","UPDATE pair","COMMIT","UPDATE after blocked commit","SELECT barrier"}),"later write never crosses pair commit");
        std::cout<<"PASS actual ACK and admission-unlocked wait\n";
    }
    for(int misuse=0;misuse<7;++misuse) {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        auto batch=db.BeginOwnedTransaction({1,2}); Check(bool(batch),"owned builder for misuse"); Check(db.Execute("UPDATE never submitted"),"owned stage accepted");
        bool rejected=false;
        switch(misuse) {
        case 0: rejected=!db.BeginTransaction(1);break;
        case 1: rejected=!db.CommitTransaction();break;
        case 2: rejected=!db.RollbackTransaction();break;
        case 3: rejected=!db.CommitTransactionDirect();break;
        case 4: rejected=!db.DirectExecute("UPDATE escape");break;
        case 5: rejected=db.Query("SELECT escape")==nullptr;break;
        case 6: { SqlStatementID id; auto stmt=db.CreateStatement(id,"UPDATE escape SET value=?"); stmt.addUInt32(1); rejected=!stmt.DirectExecute();break; }
        }
        Check(rejected,"legacy ownership theft refused"); Check(db.InOwnedTransaction(),"owned TSS identity preserved");
        Check(!batch->Commit(),"poisoned batch cannot submit"); Check(state->Events().empty(),"poisoned staged SQL never executes");
        Check(db.HasOrderedPersistenceFault(),"post-mutation stage failure fences ordered persistence");
    }
    std::cout<<"PASS nested/direct/query ownership protection\n";
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        std::promise<void> staged,finish; auto stagedDone=staged.get_future(); auto mayFinish=finish.get_future();
        std::promise<bool> committed; auto committedDone=committed.get_future();
        std::thread oldActor([&] {
            Check(db.BeginTransaction(1),"old builder registers before capture");
            Check(db.Execute("UPDATE old staged snapshot"),"old snapshot staged");
            std::unique_ptr<QueryResult> read(db.Query("SELECT legacy builder read")); Check(bool(read),"legacy builder read does not deadlock on admission gate");
            staged.set_value(); mayFinish.wait(); committed.set_value(db.CommitTransaction());
        });
        Check(stagedDone.wait_for(2s)==std::future_status::ready,"old actor staged transaction");
        Check(!db.BeginOwnedTransaction({1,2}),"owned admission refuses any preexisting normal builder");
        Check(!db.HasOrderedPersistenceFault(),"temporary builder contention does not fault database");
        finish.set_value(); Check(Ready(committedDone,"old builder admitted"),"old normal commit admitted"); oldActor.join();
        auto pair=db.BeginOwnedTransaction({1,2}); Check(bool(pair),"owned admission succeeds after normal enqueue");
        Check(db.Execute("UPDATE pair after old snapshot"),"pair after old snapshot staged"); Check(pair->Commit(),"pair commits after old batch"); Barrier(db);
        Check(state->Events()==std::vector<std::string>({"SELECT legacy builder read","BEGIN","UPDATE old staged snapshot","COMMIT","BEGIN","UPDATE pair after old snapshot","COMMIT","SELECT barrier"}),"pre-staged normal snapshot never crosses pair");
        std::cout<<"PASS legacy builder lease protects pre-staged snapshots\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        Check(db.BeginTransaction(7),"builder for explicit rollback"); Check(db.Execute("UPDATE discarded"),"rollback append"); Check(db.RollbackTransaction(),"rollback releases builder lease");
        Check(db.BeginTransaction(9),"builder before failed append"); Check(!db.PExecute(nullptr),"failed normal append is reported"); Check(db.RollbackTransaction(),"failed append rollback releases normal builder lease");
        std::thread abandoned([&] { Check(db.BeginTransaction(8),"thread-local builder registered"); Check(db.Execute("UPDATE abandoned"),"thread-local unsubmitted append"); }); abandoned.join();
        auto pair=db.BeginOwnedTransaction({1,2}); Check(bool(pair),"rollback and TSS destruction release leases");
        Check(db.Execute("UPDATE valid pair"),"valid append after releases"); Check(pair->Commit(),"pair commits after lease cleanup"); Barrier(db);
        Check(state->Events()==std::vector<std::string>({"BEGIN","UPDATE valid pair","COMMIT","SELECT barrier"}),"unsubmitted rollback/TSS SQL never executes");
        std::cout<<"PASS rollback and TSS builder lease lifetime\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        std::promise<void> staging,allowCommit,laterBegan; auto stagingDone=staging.get_future();
        auto mayCommit=allowCommit.get_future(); auto beganDone=laterBegan.get_future();
        std::promise<bool> pairResult,laterResult; auto pairDone=pairResult.get_future(); auto laterDone=laterResult.get_future();
        std::thread pairActor([&] { auto pair=db.BeginOwnedTransaction({1,2}); Check(bool(pair),"owned stage starts"); Check(db.Execute("UPDATE pair staged"),"pair captures SQL"); staging.set_value(); mayCommit.wait(); pairResult.set_value(pair->Commit()); });
        Check(stagingDone.wait_for(2s)==std::future_status::ready,"pair builder holds admission");
        std::thread laterActor([&] { bool begun=db.BeginTransaction(2); laterBegan.set_value(); Check(begun,"later normal begin succeeds after gate release"); Check(db.Execute("UPDATE later captured"),"later SQL captured after owned admission"); laterResult.set_value(db.CommitTransactionDirect()); });
        Check(beganDone.wait_for(20ms)==std::future_status::timeout,"later normal capture cannot cross owned staging");
        allowCommit.set_value(); Check(Ready(pairDone,"pair completed"),"pair acknowledged"); pairActor.join();
        Check(Ready(laterDone,"later direct transaction completed"),"later direct acknowledged"); laterActor.join(); Barrier(db);
        Check(state->Events()==std::vector<std::string>({"BEGIN","UPDATE pair staged","COMMIT","BEGIN","UPDATE later captured","COMMIT","SELECT barrier"}),"later builder executes only after full pair");
        std::cout<<"PASS owned staging blocks later normal snapshot capture\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        auto batch=db.BeginOwnedTransaction({1,2}); std::string tooLong(MAX_QUERY_LEN+5,'x');
        Check(!db.PExecute("UPDATE '%s'",tooLong.c_str()),"truncated append returns false");
        Check(!batch->Commit(),"ignored append failure still prevents partial commit"); Check(state->Events().empty(),"no partial staged SQL");
        std::cout<<"PASS append failure latch\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        auto batch=db.BeginOwnedTransaction({1,2}); Check(bool(batch),"owned builder for allocation failure");
        Check(db.Execute("UPDATE never submitted"),"stage before commit allocation fault");
        failNextAllocation=true;
        Check(!batch->Commit(),"commit setup allocation failure returns false"); Check(!failNextAllocation,"allocation seam was exercised");
        Check(db.HasOrderedPersistenceFault(),"post-detach allocation failure latches fault"); Check(state->Events().empty(),"failed setup submits no SQL");
        auto completion=std::make_shared<std::promise<bool>>(); auto result=completion->get_future();
        auto rejected=new SqlPlainRequest("UPDATE rejected"); rejected->SetCompletion(completion);
        Check(!db.AddToDelayQueue(rejected),"admission rejected after setup fault");
        Check(!Ready(result,"rejected operation signals completion false"),"rejected promise is never orphaned");
        std::cout<<"PASS commit setup failure and rejected completion lifetime\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state); std::atomic<unsigned> callbacks{0};
        Check(db.Execute("UPDATE BLOCK"),"blocking first operation admitted"); state->WaitBlocked();
        Check(db.Execute("UPDATE FAIL"),"failing write admitted"); Check(db.Execute("UPDATE dependent"),"dependent write admitted before fault");
        SqlQueryHolder holder(2); holder.SetSize(1); Check(holder.SetQuery(0,"SELECT login"),"login holder ready");
        Check(holder.Execute(new Callback(callbacks,&holder),&db,db.Results()),"login holder admitted behind failing write");
        std::promise<bool> direct; auto done=direct.get_future(); std::thread waiter([&] { direct.set_value(db.DirectExecute("UPDATE direct dependent")); });
        std::promise<QueryResult*> read; auto readDone=read.get_future(); std::thread reader([&] { read.set_value(db.Query("SELECT dependent")); });
        // The first worker operation stays blocked until admissions can queue.
        std::this_thread::sleep_for(20ms); state->Release();
        Check(!Ready(done,"dependent direct waiter completed false"),"failed predecessor fences direct write"); waiter.join();
        Check(Ready(readDone,"dependent read waiter completed null")==nullptr,"failed predecessor fences read barrier"); reader.join();
        db.ProcessResultQueue(0); Check(callbacks==1,"canceled login completion delivered once");
        Check(state->Events()==std::vector<std::string>({"UPDATE BLOCK","UPDATE FAIL"}),"no later write/login/read SQL executes after failure");
        Check(db.HasOrderedPersistenceFault(),"write failure remains latched"); Check(!db.Execute("UPDATE rejected"),"new write rejected after fault");
        std::cout<<"PASS failed predecessor cancels queued writes, login and waiters\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        Check(db.Execute("UPDATE BLOCK"),"shutdown blocker admitted"); state->WaitBlocked();
        std::promise<bool> direct; auto done=direct.get_future(); std::thread waiter([&] { direct.set_value(db.DirectExecute("UPDATE shutdown canceled")); });
        std::this_thread::sleep_for(20ms);
        std::promise<void> stopped; auto stoppedDone=stopped.get_future(); std::thread stopping([&] { db.StopServer(); stopped.set_value(); });
        std::this_thread::sleep_for(20ms); state->Release();
        Check(Ready(done,"shutdown wakes synchronous waiter"),"healthy shutdown drains queued operation"); waiter.join();
        Check(stoppedDone.wait_for(2s)==std::future_status::ready,"shutdown joins without orphaned promise"); stopping.join();
        Check(state->Events()==std::vector<std::string>({"UPDATE BLOCK","UPDATE shutdown canceled"}),"healthy shutdown preserves admitted writes");
        std::cout<<"PASS shutdown drain and promise lifetime\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state);
        Check(db.Execute("UPDATE BLOCK"),"fault shutdown blocker admitted"); state->WaitBlocked();
        auto completion=std::make_shared<std::promise<bool>>(); auto result=completion->get_future();
        auto queued=new SqlPlainRequest("UPDATE fault shutdown canceled"); queued->SetCompletion(completion);
        Check(db.AddToDelayQueue(queued),"fault shutdown operation definitely admitted");
        db.MarkOrderedPersistenceFault();
        std::thread stopping([&] { db.StopServer(); }); state->Release();
        Check(!Ready(result,"fault shutdown cancels waiter"),"fault shutdown returns false"); stopping.join();
        Check(state->Events()==std::vector<std::string>({"UPDATE BLOCK"}),"fault shutdown does not run dependent SQL");
        std::cout<<"PASS fault shutdown cancellation\n";
    }
    {
        auto state=std::make_shared<State>(); FixtureDatabase db(state); std::promise<bool> callbackResult; auto done=callbackResult.get_future();
        std::function<void(bool)> callback=[&](bool okay) { Check(okay,"initial callback receives true"); callbackResult.set_value(db.DirectExecute("UPDATE reentrant")); };
        Check(db.Execute("UPDATE callback",false,&callback),"callback operation admitted");
        Check(!Ready(done,"worker reentrant synchronous call rejects without deadlock"),"worker self-wait prevented");
        Check(db.HasOrderedPersistenceFault(),"reentrant write rejection fences persistence");
        Check(state->Events()==std::vector<std::string>({"UPDATE callback"}),"reentrant write never escapes");
        std::cout<<"PASS worker synchronous reentrancy rejection\n";
    }
    std::cout<<checks<<" ordered persistence checks passed\n";
    return 0;
}
