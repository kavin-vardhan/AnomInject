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
bool FAnomalyCaptureBackend425::RequestFrame(UWorld* World, uint64 Id)
{
    if (!World || !World->IsGameWorld() || !World->GetGameViewport() || !FSlateApplication::IsInitialized()) { return false; }
    if (!Capturer)
    {
        Owner = World;
        Capturer = MakeShared<FAnomalyFrameCapturer, ESPMode::ThreadSafe>();
        Capturer->RegisterBackbufferHook();
        View = FSceneViewExtensions::NewExtension<FAnomalyViewMetadata425>(World);
        return false;
    }
    if (Owner.Get() != World || PendingAA.Num() >= 1) { return false; }
    UGameViewportClient* Client = World->GetGameViewport();
    TSharedPtr<SViewport> Widget = Client->GetGameViewportWidget();
    if (!Widget) { return false; }
    TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(Widget.ToSharedRef());
    if (!Window || !Client->Viewport || Client->Viewport->GetSizeXY() != FIntPoint(1280, 720)) { return false; }
    FIntRect ViewRect; int32 AA = -1; uint32 Frame = 0;
    if (!View->Read(ViewRect, AA, Frame) || ViewRect.Size() != FIntPoint(1280, 720)) { return false; }
    FArrangedChildren Children(EVisibility::Visible);
    Children.AddWidget(FArrangedWidget(Window.ToSharedRef(), Window->GetWindowGeometryInWindow()));
    FWidgetPath Path(Window.ToSharedRef(), Children);
    if (!Path.ExtendPathTo(FWidgetMatcher(Widget.ToSharedRef()), EVisibility::Visible)) { return false; }
    const FArrangedWidget Arranged = Path.FindArrangedWidget(Widget.ToSharedRef()).Get(FArrangedWidget::GetNullWidget());
    const FVector2D Origin = Arranged.Geometry.GetAbsolutePosition();
    const FVector2D Size = Arranged.Geometry.GetAbsoluteSize();
    FIntRect Rect(FMath::RoundToInt(Origin.X), FMath::RoundToInt(Origin.Y), FMath::RoundToInt(Origin.X + Size.X), FMath::RoundToInt(Origin.Y + Size.Y));
    if (Rect.Size() != FIntPoint(1280, 720)) { return false; }
    PendingAA.Add(Id, AA);
    Capturer->ArmForCapture(Id, Window.Get(), Rect);
    return true;
}
bool FAnomalyCaptureBackend425::PollFrame(FAnomalyCapturedFrame& Frame)
{
    if (!Capturer) { return false; }
    Capturer->EnqueueDrain();
    if (!Capturer->PopCompleted(Frame)) { return false; }
    int32* AA = PendingAA.Find(Frame.RequestId);
    if (!AA) { return false; }
    Frame.TemporalAaMethod = *AA;
    Frame.bTemporalViewKnown = true;
    PendingAA.Remove(Frame.RequestId);
    return true;
}
int32 FAnomalyCaptureBackend425::Dropped() const { return Capturer ? Capturer->GetDropped() : FinalDrops; }
FAnomalyReadbackLayout FAnomalyCaptureBackend425::Layout() const { return Capturer ? Capturer->GetReadbackLayout() : FinalLayout; }
void FAnomalyCaptureBackend425::Stop()
{
    if (!Capturer) { return; }
    Capturer->UnregisterBackbufferHook();
    View.Reset();
    FlushRenderingCommands();
    FinalDrops = Capturer->GetDropped();
    FinalLayout = Capturer->GetReadbackLayout();
    Capturer.Reset(); PendingAA.Empty();
}
TUniquePtr<IAnomalyCaptureBackend> AnomalyCompat::CreateCaptureBackend425()
{
    return MakeUnique<FAnomalyCaptureBackend425>();
}
#endif
