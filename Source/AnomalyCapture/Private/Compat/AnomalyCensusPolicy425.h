#pragma once
#include <algorithm>
namespace AnomalyCensusPolicy425
{
    constexpr float Floor = 0.5f;
    constexpr float Ceiling = 25.0f;
    constexpr int Headroom = 8;
    inline int Batch(int free) { return std::max(0,std::min(free/2,free-Headroom)); }
    inline int Window(int ticks) { return std::max(12,std::min(ticks+8,600)); }
    inline int Band(int pixels,int area)
    {
        if (pixels<=0 || area<=0) { return 0; }
        const float pct=100.0f*pixels/area;
        return pct<Floor ? -1 : pct>Ceiling ? 2 : 1;
    }
}
