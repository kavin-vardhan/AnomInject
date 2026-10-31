#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "SceneViewExtension.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
class FAnomalyViewMetadata425 : public FSceneViewExtensionBase
{
public:
    FAnomalyViewMetadata425(const FAutoRegister& AutoRegister, UWorld* World);
    void SetupViewFamily(FSceneViewFamily&) override {}
    void SetupView(FSceneViewFamily&, FSceneView&) override {}
    void BeginRenderViewFamily(FSceneViewFamily& Family) override;
    bool IsActiveThisFrame(FViewport* Viewport) const override;
    bool Read(FIntRect& Rect, int32& AA, uint32& FamilyFrame) const;
private:
    TWeakObjectPtr<UWorld> Owner;
    mutable FCriticalSection Guard;
    FIntRect LastRect;
    int32 LastAA = -1;
    uint32 LastFrame = 0;
    bool Valid = false;
};
#endif
