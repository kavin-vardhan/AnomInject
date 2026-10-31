#include "AnomalyViewMetadata425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "Engine/GameViewportClient.h"
#include "Misc/ScopeLock.h"
#include "SceneInterface.h"
FAnomalyViewMetadata425::FAnomalyViewMetadata425(const FAutoRegister& Register, UWorld* World)
    : FSceneViewExtensionBase(Register), Owner(World) {}
bool FAnomalyViewMetadata425::IsActiveThisFrame(FViewport* Viewport) const
{
    UWorld* World = Owner.Get();
    return World && World->GetGameViewport() && World->GetGameViewport()->Viewport == Viewport;
}
void FAnomalyViewMetadata425::BeginRenderViewFamily(FSceneViewFamily& Family)
{
    UWorld* World = Owner.Get();
    FScopeLock Lock(&Guard);
    Valid = World && Family.Scene == World->Scene && Family.Views.Num() == 1;
    if (Valid)
    {
        LastRect = Family.Views[0]->UnscaledViewRect;
        LastAA = int32(Family.Views[0]->AntiAliasingMethod);
        LastFrame = Family.FrameNumber;
    }
}
bool FAnomalyViewMetadata425::Read(FIntRect& Rect, int32& AA, uint32& Frame) const
{
    FScopeLock Lock(&Guard);
    Rect = LastRect; AA = LastAA; Frame = LastFrame;
    return Valid;
}
#endif
