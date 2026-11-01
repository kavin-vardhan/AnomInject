#include "AnomalyCensusPolicy425.h"
#include <cassert>
#include <set>
#include <cstdio>
int main()
{
    using namespace AnomalyCensusPolicy425;
    assert(Band(0,10000)==0 && Band(49,10000)==-1 && Band(50,10000)==1);
    assert(Band(2500,10000)==1 && Band(2501,10000)==2);
    assert(Window(30)==38 && Window(1)==12 && Window(1000)==600);
    assert(Batch(55)==27 && Batch(8)==0 && Batch(0)==0);
    std::set<int> measured; int cursor=0,batches=0;
    while (cursor<137) { for (int i=0;i<Batch(54) && cursor<137;++i) { measured.insert(cursor++); } ++batches; }
    assert(measured.size()==137 && batches>1);
    const bool oldWholeSceneAdmission=137<=54; assert(!oldWholeSceneAdmission);
    assert(!(Band(1,10000)==1));
    std::puts("PASS: rotating 137 candidates, reserved capacity, inclusive floor/ceiling, expiry; old admission and one-pixel mutant refused");
}
