#include "Compat/AnomalyAutoLod425.h"
#include "AnomalySessionRun425.h"
#include <cstdio>

int main()
{
    int Checks=0;
    for (int Count=2; Count<=8; ++Count)
    {
        for (int Drawn=0; Drawn<Count; ++Drawn)
        {
            int Forced=AnomalyAutoLod425::ForcedModel(Count,Drawn);
            if (Forced != (Drawn==Count-1 ? 1 : Count) || Forced-1==Drawn) { return 1; }
            ++Checks;
        }
    }
    if (AnomalyAutoLod425::ForcedModel(1,0) || AnomalyAutoLod425::ForcedModel(2,-1) || AnomalyAutoLod425::ForcedModel(2,2)) { return 2; }
    bool Started=false,Running=false;
    int Starts=0,Stops=0,OldStarts=0;
    bool OldRunning=false;
    for (int Frame=0; Frame<2352; ++Frame)
    {
        int Step=(Frame-624)%288;
        bool Eligible=Frame>=624 && Step>=48 && Step<258;
        bool Wanted=AnomalySessionRun425::Wanted(Started,Eligible,Frame>=2328);
        if (Wanted!=Running) { if (Wanted) { ++Starts; Started=true; } else { ++Stops; } Running=Wanted; }
        if (Eligible && !OldRunning) { ++OldStarts; }
        OldRunning=Eligible;
        if (Frame<672 && Running) { return 3; }
        if (Frame>=2328 && Running) { return 4; }
    }
    if (Starts!=1 || Stops!=1 || OldStarts!=6) { return 5; }
    if (AnomalyAutoLod425::ForcedModel(2,1)==2) { return 6; }
    std::printf("PASS: %d LOD choices; invalid/single LOD refused; one session start and stop; old six-start and unchanged-coarsest negatives fail\n",Checks);
    return 0;
}
