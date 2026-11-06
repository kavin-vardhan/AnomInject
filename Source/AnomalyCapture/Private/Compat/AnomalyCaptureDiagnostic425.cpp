#include "AnomalyCaptureDiagnostic425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyCaptureLabels425.h"
#include "AnomalyUiMarker425.h"
#include "AnomalyPixel425.h"
#include "AnomalyPreviewCapture.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "UnrealClient.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/SlateRenderer.h"
#include "Widgets/SViewport.h"
#include "Widgets/SWindow.h"
#include "RenderingThread.h"
#include "CoreGlobals.h"
#include "RHICommandList.h"
#include "Misc/App.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeLock.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void FAnomalyCaptureDiagnostic425::Start(UWorld* World,const FString& Directory)
{
    Owner=World; Output=Directory; Started=FPlatformTime::Seconds(); Enabled=true;
    if (!World || !World->GetGameViewport() || !FSlateApplication::IsInitialized()) { return; }
    auto Widget=World->GetGameViewport()->GetGameViewportWidget();
    if (!Widget.IsValid()) { return; }
    Window=FSlateApplication::Get().FindWidgetWindow(Widget.ToSharedRef());
    DrawHandle=UDebugDrawService::Register(TEXT("Game"),FDebugDrawDelegate::CreateRaw(this,&FAnomalyCaptureDiagnostic425::Draw));
    BackbufferHandle=FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().AddRaw(this,&FAnomalyCaptureDiagnostic425::Backbuffer);
}
void FAnomalyCaptureDiagnostic425::Tick(ELevelTick Type,FName Hook)
{
    TickType=int32(Type); ObservedHook=Hook; ++HookTicks;
}
void FAnomalyCaptureDiagnostic425::Requested(uint64 Request,uint64 SampleFrame)
{
    RequestByFrame.Add(SampleFrame,Request);
}
void FAnomalyCaptureDiagnostic425::Draw(UCanvas* Canvas,APlayerController*)
{
    if (!Enabled || !Canvas || !Canvas->Canvas) { return; }
    const uint64 Sample=GFrameCounter;
    const uint64* Found=RequestByFrame.Find(Sample);
    const uint64 Request=Found && Draws.GetValue()<8 ? *Found : 0;
    ENQUEUE_RENDER_COMMAND(AnomalyMarkerFrame425)([this,Request,Sample](FRHICommandListImmediate&)
    { PresentedRequest=Request; PresentedFrame=Sample; });
    if (!Request) { return; }
    DrawnRequests.Add(Request);
    auto DrawRow=MakeShared<FJsonObject>(); DrawRow->SetNumberField(TEXT("request"),double(Request)); DrawRow->SetNumberField(TEXT("sample_frame"),double(Sample));
    DrawRows.Add(MakeShared<FJsonValueObject>(DrawRow));
    using namespace AnomalyUiMarker425;
    for (int32 I=0;I<8;++I)
    {
        const uint32 C=Color(I);
        FCanvasTileItem Tile(FVector2D(X+(I%4)*Cell,Y+(I/4)*Cell),FVector2D(Cell,Cell),FLinearColor(FColor((C>>16)&255,(C>>8)&255,C&255,255)));
        Tile.BlendMode=SE_BLEND_Opaque; Canvas->Canvas->DrawItem(Tile);
    }
    Draws.Increment();
}
void FAnomalyCaptureDiagnostic425::Backbuffer(SWindow& InWindow,const FTexture2DRHIRef& Buffer)
{
    if (Window.Pin().Get()!=&InWindow || !Buffer.IsValid()) { return; }
    BackbufferFormat.Set(int32(Buffer->GetFormat()));
    if (!PresentedRequest || ControlledRequests.Contains(PresentedRequest)) { return; }
    ControlledRequests.Add(PresentedRequest);
    using namespace AnomalyUiMarker425;
    TArray<FColor> Pixels;
    FRHICommandListExecutor::GetImmediateCommandList().ReadSurfaceData(Buffer,FIntRect(X,Y,X+Width,Y+Height),Pixels,FReadSurfaceDataFlags(RCM_UNorm));
    int32 Match=0;
    if (Pixels.Num()==Width*Height)
    {
        for (int32 I=0;I<8;++I)
        {
            const FColor P=Pixels[((I/4)*Cell+Cell/2)*Width+(I%4)*Cell+Cell/2];
            Match+=Matches(P.R,P.G,P.B,I)?1:0;
        }
    }
    if (Present(Match)) { Positive.Increment(); }
    {
        FScopeLock Lock(&Guard);
        auto Control=MakeShared<FJsonObject>(); Control->SetNumberField(TEXT("request"),double(PresentedRequest)); Control->SetNumberField(TEXT("sample_frame"),double(PresentedFrame));
        Control->SetNumberField(TEXT("matching_cells"),Match); Control->SetStringField(TEXT("image"),FString::Printf(TEXT("ui-control-%02d.png"),ControlPixels.Num()));
        ControlRows.Add(MakeShared<FJsonValueObject>(Control)); ControlPixels.Add(MoveTemp(Pixels));
    }
    Controls.Increment();
}
void FAnomalyCaptureDiagnostic425::Observe(const FAnomalyCapturedFrame& Frame)
{
    using namespace AnomalyUiMarker425;
    const bool Drawn=DrawnRequests.Contains(Frame.RequestId);
    bool ControlPositive=false;
    if (Drawn)
    {
        FlushRenderingCommands();
        FScopeLock Lock(&Guard);
        for (const auto& Value:ControlRows)
        {
            auto Control=Value->AsObject();
            if (Control->GetNumberField(TEXT("request"))==double(Frame.RequestId) && Control->GetNumberField(TEXT("sample_frame"))==double(Frame.LabelKey425.SampleFrame) && Present(int32(Control->GetNumberField(TEXT("matching_cells"))))) { ControlPositive=true; }
        }
        if (ControlPositive) { Paired.Increment(); }
    }
    int32 Match=0;
    const bool Extent=Frame.Width>=X+Width && Frame.Height>=Y+Height;
    if (Extent)
    {
        for (int32 I=0;I<8;++I)
        {
            const int32 PX=X+(I%4)*Cell+Cell/2,PY=Y+(I/4)*Cell+Cell/2;
            const auto P=AnomalyPixel425::Decode(Frame.Format,AnomalyPixel425::Word(Frame.RawBytes.GetData()+(PY*Frame.Width+PX)*4));
            Match+=P.Valid && Matches(P.R,P.G,P.B,I)?1:0;
        }
    }
    Checked.Increment(); if (!Extent || Present(Match)) { Hits.Increment(); }
    auto Row=MakeShared<FJsonObject>(); Row->SetNumberField(TEXT("request"),Frame.RequestId); Row->SetNumberField(TEXT("matching_cells"),Match);
    Row->SetNumberField(TEXT("sample_frame"),double(Frame.LabelKey425.SampleFrame)); Row->SetNumberField(TEXT("family_frame"),Frame.LabelKey425.FamilyFrame);
    Row->SetBoolField(TEXT("drawn"),Drawn); Row->SetBoolField(TEXT("control_positive"),ControlPositive);
    Row->SetBoolField(TEXT("extent"),Extent); Row->SetBoolField(TEXT("marker_present"),Present(Match)); Rows.Add(MakeShared<FJsonValueObject>(Row));
}
FString FAnomalyCaptureDiagnostic425::Line() const
{
    UWorld* World=Owner.Get(); UGameViewportClient* Client=World?World->GetGameViewport():nullptr;
    FViewport* Viewport=Client?Client->Viewport:nullptr;
    const FString Type=Viewport?Viewport->GetViewportType().ToString():TEXT("none");
    const FIntPoint Size=Viewport?Viewport->GetSizeXY():FIntPoint::ZeroValue;
    const int32 PreUi=Paired.GetValue()>0 && Checked.GetValue()>0 && Hits.GetValue()==0 ? 1 : 0;
    return FString::Printf(TEXT("viewport=%s client=%s viewport_size=%dx%d backbuffer_format=%d slate=%d pie=%d pre_ui=%d ui_draws=%d ui_controls=%d ui_positive=%d ui_checked=%d ui_hits=%d ui_paired=%d tick=%s tick_source=build_constant tick_type=%d tick_type_source=build_constant tick_calls=%d fixed_step=%d fixed_dt=%.9f fixed_rate=%d fixed_fps=%.6f smoothing=%d"),
        *Type,Client?*Client->GetClass()->GetName():TEXT("none"),Size.X,Size.Y,BackbufferFormat.GetValue(),Viewport && Viewport->IsSlateViewport()?1:0,Viewport && Viewport->IsPlayInEditorViewport()?1:0,
        PreUi,Draws.GetValue(),Controls.GetValue(),Positive.GetValue(),Checked.GetValue(),Hits.GetValue(),Paired.GetValue(),*ObservedHook.ToString(),TickType,HookTicks,
        FApp::UseFixedTimeStep()?1:0,FApp::GetFixedDeltaTime(),GEngine && GEngine->bUseFixedFrameRate?1:0,GEngine?GEngine->FixedFrameRate:0.0f,GEngine && GEngine->bSmoothFrameRate?1:0);
}
bool FAnomalyCaptureDiagnostic425::Stop()
{
    if (!Enabled) { return true; }
    Enabled=false; UDebugDrawService::Unregister(DrawHandle);
    if (FSlateApplication::IsInitialized()) { FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().Remove(BackbufferHandle); }
    FlushRenderingCommands();
    if (!AnomalyCaptureC2::Evidence()) { return true; }
    bool Saved=true;
    for (int32 I=0;I<ControlPixels.Num();++I)
    {
        TArray<uint8> Png;
        Saved &= AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,ControlPixels[I],AnomalyUiMarker425::Width,AnomalyUiMarker425::Height,Png,0) &&
            FFileHelper::SaveArrayToFile(Png,*(Output/FString::Printf(TEXT("ui-control-%02d.png"),I)));
    }
    auto O=MakeShared<FJsonObject>(); O->SetArrayField(TEXT("frames"),Rows); O->SetArrayField(TEXT("drawn_frames"),DrawRows); O->SetArrayField(TEXT("control_frames"),ControlRows); O->SetNumberField(TEXT("paired"),Paired.GetValue()); O->SetNumberField(TEXT("draws"),Draws.GetValue()); O->SetNumberField(TEXT("controls"),Controls.GetValue());
    O->SetNumberField(TEXT("positive_controls"),Positive.GetValue()); O->SetNumberField(TEXT("checked"),Checked.GetValue()); O->SetNumberField(TEXT("hits"),Hits.GetValue()); O->SetStringField(TEXT("last_state"),Line());
    FString Text; auto Writer=TJsonWriterFactory<>::Create(&Text); FJsonSerializer::Serialize(O,Writer);
    return FFileHelper::SaveStringToFile(Text,*(Output/TEXT("ui-check.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM) && Saved;
}
#endif
