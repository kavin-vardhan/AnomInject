#pragma once

namespace AnomalyAutoLod425
{
    inline int ForcedModel(int Count, int Drawn)
    {
        if (Count < 2 || Drawn < 0 || Drawn >= Count) { return 0; }
        return Drawn == Count - 1 ? 1 : Count;
    }
}
