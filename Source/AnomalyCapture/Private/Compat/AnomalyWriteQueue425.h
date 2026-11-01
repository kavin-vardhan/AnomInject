#pragma once

namespace AnomalyWriteQueue425
{
    constexpr int MaxPending = 4;

    template <typename Queue, typename Consume>
    int MakeRoom(Queue& Writes, Consume ConsumeResult)
    {
        int Drained = 0;
        while (Writes.Num() >= MaxPending)
        {
            ConsumeResult(Writes[0].Get());
            Writes.RemoveAt(0);
            ++Drained;
        }
        return Drained;
    }
}
