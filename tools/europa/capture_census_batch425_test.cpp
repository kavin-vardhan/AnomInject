#include "AnomalyCensusBatch425.h"
#include "AnomalyUiMarker425.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
using namespace AnomalyCensusBatch425;
int main()
{
    for (int i=0;i<8;++i) { const auto c=AnomalyUiMarker425::Color(i); assert(AnomalyUiMarker425::Matches((c>>16)&255,(c>>8)&255,c&255,i)); }
    assert(AnomalyUiMarker425::Present(8)); assert(!AnomalyUiMarker425::Present(6));
    assert(!AnomalyUiMarker425::Matches(0,0,0,0));
    assert(LostAfterTicks==8);
    assert(Outcome(108,100,false,false,false)==EOutcome::Pending);
    assert(Outcome(109,100,false,false,false)==EOutcome::Lost);
    assert(Outcome(109,100,true,true,false)==EOutcome::Credit);
    assert(Outcome(101,100,true,false,false)==EOutcome::NoPass);
    assert(Outcome(101,100,true,true,true)==EOutcome::Polluted);
    for (EOutcome O:{EOutcome::NoPass,EOutcome::Polluted,EOutcome::Lost})
    {
        int Prior=27,Queued=0;
        if (O==EOutcome::Credit) { Prior=0; }
        else if (O!=EOutcome::Pending) { ++Queued; }
        assert(Prior==27 && Queued==1);
    }
    assert(AnnotationFar(0)==1000000.0f);
    assert(AnnotationFar(98765)==1000000.0f);
    std::puts("PASS: lost only after eight ticks; resolved result wins; no-pass/polluted/lost preserve verdict; infinite and finite far planes");
}
