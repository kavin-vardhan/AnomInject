#include "AnomalyViewMetadata425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "Engine/GameViewportClient.h"
#include "Misc/ScopeLock.h"
#include "SceneInterface.h"
#include "CoreGlobals.h"
#include "UnrealClient.h"
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
    for (auto& Pair : Tickets)
    {
        FTicket& Ticket = Pair.Value;
        if (Ticket.Accepted) { continue; }
        if (!Valid || Ticket.Key.SampleFrame != GFrameCounter || Ticket.Key.Owner != World->GetUniqueID())
        {
            Rejections.Increment();
            continue;
        }
        Ticket.Key.FamilyFrame = Family.FrameNumber;
        Ticket.Key.AA = LastAA;
        Ticket.Accepted = true;
    }
}
bool FAnomalyViewMetadata425::Read(FIntRect& Rect, int32& AA, uint32& Frame) const
{
    FScopeLock Lock(&Guard);
    Rect = LastRect; AA = LastAA; Frame = LastFrame;
    return Valid;
}
bool FAnomalyViewMetadata425::Queue(const FAnomalyLabelKey425& Key, SWindow* Window, const FIntRect& Rect,
    TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer)
{
    FScopeLock Lock(&Guard);
    if (Tickets.Num() >= 8 || Tickets.Contains(Key.Request) || !Owner.IsValid() || Key.Owner != Owner->GetUniqueID()) { return false; }
    FTicket Ticket;
    Ticket.Key = Key; Ticket.Window = Window; Ticket.Rect = Rect; Ticket.Capturer = Capturer;
    Tickets.Add(Key.Request, Ticket);
    return true;
}
void FAnomalyViewMetadata425::PostRenderViewFamily_RenderThread(FRHICommandListImmediate&, FSceneViewFamily& Family)
{
    FScopeLock Lock(&Guard);
    for (auto& Pair : Tickets)
    {
        FTicket& Ticket = Pair.Value;
        if (!Ticket.Accepted || Ticket.Rendered || Ticket.Key.FamilyFrame != Family.FrameNumber) { continue; }
        Ticket.Rendered = true;
        if (TSharedPtr<FAnomalyFrameCapturer, ESPMode::ThreadSafe> Capturer = Ticket.Capturer.Pin())
        {
            const FTexture2DRHIRef Source = Family.RenderTarget ? Family.RenderTarget->GetRenderTargetTexture() : FTexture2DRHIRef();
            const FIntRect Rect = Family.Views.Num() == 1 ? Family.Views[0]->UnscaledViewRect : FIntRect();
            if (!Source.IsValid() || !Source->GetTexture2D() || Rect != Ticket.Rect)
            {
                Rejections.Increment();
                continue;
            }
            Capturer->CaptureView_RenderThread(Ticket.Key.Request, Source->GetTexture2D(), Rect);
        }
        else { Rejections.Increment(); }
    }
}
bool FAnomalyViewMetadata425::Take(uint64 Request, FAnomalyLabelKey425& Key)
{
    FScopeLock Lock(&Guard);
    FTicket* Ticket = Tickets.Find(Request);
    if (!Ticket || !Ticket->Accepted || !Ticket->Rendered) { Rejections.Increment(); return false; }
    Key = Ticket->Key;
    Tickets.Remove(Request);
    return true;
}
#endif
