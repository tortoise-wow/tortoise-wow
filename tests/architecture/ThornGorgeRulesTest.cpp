#include "ThornGorgeRules.h"
#include <iostream>
#include <stdexcept>
#include <random>
using namespace ThornGorge;
static void Check(bool result, char const* message) { if (!result) throw std::runtime_error(message); }
int main()
{
    Rules fast;
    for (unsigned i=0; i<12; ++i) fast.Capture(0, 15, 0);
    Check(fast.owner[0] == Neutral && fast.progress[0] == 98, "large group cannot capture before 13 samples");
    fast.Capture(0, 15, 0); Check(fast.owner[0] == Alliance, "large group captures at capped speed");
    fast.ConfigureCapture(0); Check(fast.maxCaptureAdvantage == 1, "capture lower clamp");
    fast.ConfigureCapture(99); Check(fast.maxCaptureAdvantage == 5, "capture upper clamp");
    for (unsigned n=0;n<4;++n)
    {
        Check(NodeIconState(n, Neutral)==3606+n*3, "neutral AreaPOI state");
        Check(NodeIconState(n, Horde)==3607+n*3, "red AreaPOI state");
        Check(NodeIconState(n, Alliance)==3608+n*3, "blue AreaPOI state");
    }
    Rules a;
    for (unsigned i = 0; i < 100; ++i) a.Capture(0, 15, 15);
    Check(a.progress[0] == 50 && a.owner[0] == Neutral, "equal teams must not capture");
    for (unsigned i = 0; i < 25; ++i) a.Capture(0, 1, 0);
    Check(a.owner[0] == Alliance && a.progress[0] == 100, "solo capture takes 25 samples");
    a.Tick(1999); Check(a.score[0] == 0, "no early resource tick");
    a.Tick(1); Check(a.score[0] == 1, "one base resource tick");
    for (unsigned i = 0; i < 16; ++i) a.Capture(0, 0, 1);
    Check(a.owner[0] == Neutral, "assault neutralizes before enemy capture");
    for (unsigned i = 0; i < 34; ++i) a.Capture(0, 0, 1);
    Check(a.owner[0] == Horde && a.progress[0] == 0, "enemy takeover");
    Rules f;
    Check(!f.PickUp(0) && f.PickUp(11), "valid carrier required");
    Check(!f.PickUp(22), "simultaneous pickup cannot replace carrier");
    Check(!f.Drop(22), "stale player cannot drop someone else's flag");
    Check(!f.Deliver(11, Alliance, 0), "neutral base cannot score delivery");
    f.owner[0] = Alliance;
    Check(!f.Deliver(22, Alliance, 0), "stale carrier cannot deliver");
    Check(!f.Deliver(11, Horde, 0), "enemy base cannot receive delivery");
    Check(f.Deliver(11, Alliance, 0) && f.score[0] == 75, "delivery bonus");
    Check(!f.Deliver(11, Alliance, 0) && !f.PickUp(22), "no duplicate delivery or pickup during respawn");
    f.Tick(23000 - 1); Check(f.flag == Respawning, "WSG-length flag respawn is not early");
    f.Tick(1); Check(f.flag == Center, "flag respawns on time");
    Check(f.PickUp(22) && f.Drop(22), "death/disconnect drop");
    f.Tick(2107); // Real match: pickup started two seconds after drop.
    f.Tick(10000); Check(f.flag == Dropped, "drop survives travel and full native pickup cast");
    Check(f.PickUp(33), "another player can recover dropped flag");
    f.Tick(10000); Check(f.flag == Carried && f.carrier == 33, "old reset timer cannot clear new carrier");
    Check(f.Drop(33), "second drop");
    f.Tick(29999); Check(f.flag == Dropped, "unclaimed drop does not reset early");
    f.Tick(1); Check(f.flag == Center && f.carrier == 0, "unclaimed flag returns");
    f.PickUp(44); f.Finish(); auto scores = f.score;
    f.Tick(90000); f.Capture(0, 0, 15); f.AddPoints(Horde, 500);
    Check(!f.PickUp(55) && !f.Deliver(44, Alliance, 0) && f.carrier == 0 && f.score == scores, "ended match is inert");
    Rules tie;
    tie.owner = {{Alliance, Alliance, Horde, Horde}};
    tie.score = {{1599, 1599}}; tie.Tick(2000);
    Check(tie.HasWinner() && tie.Leader() == Neutral && tie.score[0] == 1600 && tie.score[1] == 1600, "simultaneous final tick has no faction bias");
    Rules cap; cap.owner.fill(Alliance); cap.score[0] = 1500;
    cap.PickUp(7); cap.Deliver(7, Alliance, 0);
    Check(cap.score[0] == 1600, "flag bonus clamps score");
    Rules batched, stepped; batched.owner = stepped.owner = {{Alliance, Alliance, Alliance, Horde}};
    batched.Tick(120000); for (int i=0;i<120;++i) stepped.Tick(1000);
    Check(batched.score == stepped.score, "resource catch-up preserves tick accounting");
    std::mt19937 rng(821);
    for (unsigned game=0; game<1000; ++game)
    {
        Rules match;
        for (unsigned sec=0; sec<1800 && !match.HasWinner(); ++sec)
        {
            for (unsigned node=0;node<4;++node) match.Capture(node, rng()%16, rng()%16);
            if (rng()%5==0) match.PickUp(1+rng()%30);
            if (rng()%7==0) match.Drop(match.carrier);
            if (rng()%3==0) match.Deliver(match.carrier, Team(rng()%2), rng()%4);
            match.Tick(1000);
            Check(match.score[0]<=1600 && match.score[1]<=1600, "scores stay bounded");
            Check((match.flag==Carried) == (match.carrier!=0), "flag/carrier invariant");
            for (int p:match.progress) Check(p>=0 && p<=100, "capture progress stays bounded");
        }
        match.Finish(); Check(match.carrier==0, "match cleanup");
    }
    std::cout << "Thorn Gorge rules: edge cases and 1000 simulated matches passed\n";
}
