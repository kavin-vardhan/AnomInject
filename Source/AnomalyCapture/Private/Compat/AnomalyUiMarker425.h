#pragma once
#include <cstdint>
namespace AnomalyUiMarker425
{
    constexpr int X=16,Y=16,Cell=16,Width=64,Height=32;
    inline std::uint32_t Color(int Index)
    {
        const std::uint32_t Colors[8]={0xff00ff,0x00ff00,0x0000ff,0xffff00,0x00ffff,0xff0000,0xffffff,0x000000};
        return Colors[Index&7];
    }
    inline bool Matches(int R,int G,int B,int Index)
    {
        const std::uint32_t C=Color(Index);
        const int DR=R-int((C>>16)&255),DG=G-int((C>>8)&255),DB=B-int(C&255);
        return DR>=-4 && DR<=4 && DG>=-4 && DG<=4 && DB>=-4 && DB<=4;
    }
    inline bool Present(int MatchingCells) { return MatchingCells>=7; }
}
