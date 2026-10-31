#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "AnomalyFrameCapturer.h"
class UWorld;
class FSceneViewFamily;
class FAnomalyMasks425
{
public:
    bool Start(UWorld* World);
    bool Restore();
    static uint32 Selection(UWorld* World);
    static bool Render(FRHICommandListImmediate& Cmd, FSceneViewFamily& Family, uint64 Request, uint32 Target,
        FAnomalyFrameCapturer& Capturer);
    static bool Save(const FAnomalyCapturedFrame& Frame, const FString& Output);
private:
    struct FSaved
    {
        TWeakObjectPtr<UPrimitiveComponent> Component;
        bool Depth = false;
        int32 Tag = 0;
        ERendererStencilMask WriteMask = ERendererStencilMask::ERSM_Default;
    };
    TArray<FSaved> Saved;
    int32 PriorCustomDepth = -1;
};
#endif
