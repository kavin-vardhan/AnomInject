#include "AnomalyCaptureBackend425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Widgets/SViewport.h"
#include "Slate/SceneViewport.h"
#include "Layout/WidgetPath.h"
#include "Layout/ArrangedWidget.h"
#include "Layout/ArrangedChildren.h"
#include "RenderingThread.h"
#include "CoreGlobals.h"
#include "AnomalyCaptureLabels425.h"
#include "AnomalyMasks425.h"
bool FAnomalyCaptureBackend425::RequestFrame(UWorld* World, uint64 Id)
{
    if (!World || !World->IsGameWorld() || !World->GetGameViewport() || !FSlateApplication::IsInitialized()) { return false; }
    if (!Capturer)
    {
        Owner = World;
        Capturer = MakeShared<FAnomalyFrameCapturer, ESPMode::ThreadSafe>();
        Capturer->SetLayoutPathName(TEXT("view-family-pre-ui-425"));
        View = FSceneViewExtensions::NewExtension<FAnomalyViewMetadata425>(World);
        return false;
    }
    if (Owner.Get() != World || PendingAA.Num() >= (LabelEpoch ? 8 : 1)) { return false; }
    UGameViewportClient* Client = World->GetGameViewport();
    TSharedPtr<SViewport> Widget = Client->GetGameViewportWidget();
    if (!Widget) { return false; }
    TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(Widget.ToSharedRef());
    if (!Window || !Client->Viewport || Client->Viewport->GetSizeXY().X <= 0 || Client->Viewport->GetSizeXY().Y <= 0) { return false; }
    FIntRect ViewRect; int32 AA = -1; uint32 Frame = 0;
    if (!View->Read(ViewRect, AA, Frame) || ViewRect.Size() != Client->Viewport->GetSizeXY()) { return false; }
    FAnomalyLabelKey425 Key;
    Key.Epoch = LabelEpoch; Key.Owner = World->GetUniqueID(); Key.Request = Id;
    Key.SampleFrame = GFrameCounter; Key.SessionIndex = int32(Id - 1);
    Key.Masks = AnomalyCaptureC2::Masks();
    Key.MaskBits = Key.Masks ? FAnomalyMasks425::Selection(World) : 0;
    Key.MaskTarget = Key.MaskBits ? 1 : 0;
    if (!View->Queue(Key, Window.Get(), ViewRect, Capturer)) { return false; }
    PendingAA.Add(Id, AA);
    return true;
}
bool FAnomalyCaptureBackend425::PollFrame(FAnomalyCapturedFrame& Frame)
{
    if (!Capturer) { return false; }
    Capturer->EnqueueDrain();
    if (!Capturer->PopCompleted(Frame)) { return false; }
    int32* AA = PendingAA.Find(Frame.RequestId);
    if (!AA) { return false; }
    if (!View->Take(Frame.RequestId, Frame.LabelKey425)) { PendingAA.Remove(Frame.RequestId); return false; }
    Frame.TemporalAaMethod = Frame.LabelKey425.AA;
    Frame.bTemporalViewKnown = true;
    PendingAA.Remove(Frame.RequestId);
    return true;
}
int32 FAnomalyCaptureBackend425::Dropped() const { return Capturer ? Capturer->GetDropped() + (View ? View->Rejected() : 0) : FinalDrops; }
FAnomalyReadbackLayout FAnomalyCaptureBackend425::Layout() const { return Capturer ? Capturer->GetReadbackLayout() : FinalLayout; }
void FAnomalyCaptureBackend425::Stop()
{
    if (!Capturer) { return; }
    Capturer->UnregisterBackbufferHook();
    FlushRenderingCommands();
    FinalDrops = Capturer->GetDropped() + View->Rejected() + PendingAA.Num();
    View.Reset();
    FinalLayout = Capturer->GetReadbackLayout();
    Capturer.Reset(); PendingAA.Empty();
}
TUniquePtr<IAnomalyCaptureBackend> AnomalyCompat::CreateCaptureBackend425()
{
    return MakeUnique<FAnomalyCaptureBackend425>();
}
#endif
