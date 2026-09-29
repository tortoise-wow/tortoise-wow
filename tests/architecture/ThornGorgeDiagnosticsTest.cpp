#include "ThornGorgeDiagnostics.h"
#include <cstdlib>
#include <iostream>
void check(bool value) { if (!value) { std::cerr << "Diagnostic budget failed\n"; std::exit(1); } }
int main()
{
    ThornGorge::DiagnosticBudget b;
    check(!b.Event() && !b.Event(true) && !b.Advance(60000) && !b.ManualSnapshot());
    check(b.events==0 && b.sinceWindow==0 && b.sinceSnapshot==0);
    b.Configure(99,1); check(b.level==2 && b.interval==1000);
    check(!b.Advance(999)); check(b.Advance(1));
    for (unsigned i=0;i<64;++i) check(b.Event());
    for (unsigned i=0;i<1000;++i) check(!b.Event());
    check(b.Event(true)); check(b.TakeSuppressed()==1000); check(b.TakeSuppressed()==0);
    b.Advance(999); check(!b.Event()); b.Advance(1); check(b.Event());
    check(b.ManualSnapshot()); check(!b.ManualSnapshot()); b.Advance(999); check(!b.ManualSnapshot());
    b.Advance(1); check(b.ManualSnapshot());
    b.Configure(1,5000); check(b.Advance(60000)); check(!b.Advance(0)); check(!b.Advance(4999)); check(b.Advance(1));
    b.Configure(-1,999999); check(b.level==0 && b.interval==60000 && b.events==0 && b.manualCooldown==0);
    check(!b.Event(true) && !b.Advance(1000) && !b.ManualSnapshot());
    std::cout << "off, clamping, rate limit, suppression, manual cooldown and stall checks passed\n";
}
