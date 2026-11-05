#pragma once
#include "CoreTypes.h"
namespace AnomalyTransitionCounts425
{
    template<typename TTransitions>
    int32 CameraUnconfirmedEntries(const TTransitions& Transitions)
    {
        int32 Count=0;
        for (const auto& Entry:Transitions) { Count+=Entry.Value.Reasons.FindRef(TEXT("camera_clipping_unconfirmed")); }
        return Count;
    }
}
