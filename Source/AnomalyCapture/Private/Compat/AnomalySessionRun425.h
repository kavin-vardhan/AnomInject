#pragma once

namespace AnomalySessionRun425
{
    inline bool Wanted(bool Started, bool Eligible, bool Tail)
    {
        return !Tail && (Started || Eligible);
    }
}
