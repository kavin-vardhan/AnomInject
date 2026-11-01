#pragma once
#include "AnomalyCaptureFeatures.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "SceneViewExtension.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
#include "AnomalyLabelFrame425.h"
#include "AnomalyFrameCapturer.h"
class FAnomalyViewMetadata425 : public FSceneViewExtensionBase
{
public:
    FAnomalyViewMetadata425(const FAutoRegister& AutoRegister, UWorld* World);
    void SetupViewFamily(FSceneViewFamily&) override {}
    void SetupView(FSceneViewFamily&, FSceneView&) override {}
    void BeginRenderViewFamily(FSceneViewFamily& Family) override;
    void PreRenderViewFamily_RenderThread(FRHICommandListImmediate&, FSceneViewFamily&) override {}
    void PreRenderView_RenderThread(FRHICommandListImmediate&, FSceneView&) override {}
    bool IsActiveThisFrame(FViewport* Viewport) const override;
    bool Read(FIntRect& Rect, int32& AA, uint32& FamilyFrame) const;
    bool Queue(const FAnomalyLabelKey425& Key, SWindow* Window, const FIntRect& Rect, TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer);
    bool Take(uint64 Request, FAnomalyLabelKey425& Key);
    void PostRenderViewFamily_RenderThread(FRHICommandListImmediate&, FSceneViewFamily& Family) override;
    int32 Rejected() const { return Rejections.GetValue(); }
    int32 PostRenderCount() const { return PostRenders.GetValue(); }
private:
    struct FTicket
    {
        FAnomalyLabelKey425 Key;
        SWindow* Window = nullptr;
        FIntRect Rect;
        TWeakPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer;
        bool Accepted = false;
        bool Rendered = false;
    };
    TMap<uint64, FTicket> Tickets;
    FThreadSafeCounter Rejections;
    FThreadSafeCounter PostRenders;
    TWeakObjectPtr<UWorld> Owner;
    mutable FCriticalSection Guard;
    FIntRect LastRect;
    int32 LastAA = -1;
    uint32 LastFrame = 0;
    bool Valid = false;
};
#endif
