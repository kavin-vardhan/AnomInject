#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"

struct FAnomalyLabelKey425
{
    uint64 Epoch = 0;
    uint64 Request = 0;
    uint64 SampleFrame = 0;
    uint32 Owner = 0;
    uint32 FamilyFrame = 0;
    int32 SessionIndex = -1;
    int32 AA = -1;
    bool Masks = false;
    uint32 MaskTarget = 0;
    uint64 MaskBits = 0;
    FVector PlayerLocation=FVector::ZeroVector,ViewLocation=FVector::ZeroVector;
    FRotator PlayerRotation=FRotator::ZeroRotator,ViewRotation=FRotator::ZeroRotator;
    float PlayerFov=0,ViewFov=0;
    FString PlayerTarget;
};

namespace AnomalyLabelKey425
{
    constexpr bool Same(uint64 Epoch, uint32 Owner, uint64 Request, uint64 OtherEpoch, uint32 OtherOwner, uint64 OtherRequest)
    {
        return Epoch == OtherEpoch && Owner == OtherOwner && Request == OtherRequest && Request != 0;
    }
    static_assert(Same(1, 7, 3, 1, 7, 3), "Matching key rejected");
    static_assert(!Same(1, 7, 3, 2, 7, 3), "Stale run epoch admitted");
    static_assert(!Same(1, 7, 3, 1, 8, 3), "Foreign world admitted");
    static_assert(!Same(1, 7, 3, 1, 7, 4), "Wrong request admitted");
}
#endif
