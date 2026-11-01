#include "AnomalyStreetBench425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyPixel425.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyStencilTag.h"
#include "AnomalyMasks425.h"
#include "AnomalyViewport.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Camera/PlayerCameraManager.h"
#include "Misc/FileHelper.h"
#include "Misc/DateTime.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
    bool JsonStreet425(const TSharedPtr<FJsonObject>& O,const FString& Path)
    {
        FString Text; auto W=TJsonWriterFactory<>::Create(&Text); FJsonSerializer::Serialize(O.ToSharedRef(),W);
        return FFileHelper::SaveStringToFile(Text,*Path,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    }
    TArray<TSharedPtr<FJsonValue>> XYZ425(const FVector& V)
    {
        return {MakeShared<FJsonValueNumber>(V.X),MakeShared<FJsonValueNumber>(V.Y),MakeShared<FJsonValueNumber>(V.Z)};
    }
}
bool FAnomalyStreetBench425::Start(UWorld* World,const FString& Output)
{
    Owner=World; Directory=Output;
    APlayerController* PC=World ? World->GetFirstPlayerController() : nullptr;
    if (!PC || !PC->PlayerCameraManager) { return false; }
    PC->GetPlayerViewPoint(Position,Base); Target=PC->GetViewTarget(); TargetRotation=Target.IsValid()?Target->GetActorRotation():Base;
    IgnoreMove=PC->IsMoveInputIgnored(); IgnoreLook=PC->IsLookInputIgnored();
    PC->SetIgnoreMoveInput(true); PC->SetIgnoreLookInput(true);
    OriginalViewRotation=Base; Base.Pitch=-12.0f;
    auto Candidates=AnomalyViewport::GetCensusPrefilterActors(World);
    Candidates.Sort([](const TWeakObjectPtr<AActor>& A,const TWeakObjectPtr<AActor>& B) { return GetNameSafe(A.Get())<GetNameSafe(B.Get()); });
    int32 N=0;
    for (const auto& A:Candidates)
    {
        if (!A.IsValid() || N>=3) { continue; }
        TInlineComponentArray<UPrimitiveComponent*> Prims(A.Get());
        for (UPrimitiveComponent* Prim:Prims)
        {
            if (!AnomalyViewport::IsRenderableComponent(Prim) || Prim->bRenderCustomDepth) { continue; }
            FPrior Prior; Prior.On=Prim->bRenderCustomDepth; Prior.Value=Prim->CustomDepthStencilValue; Prior.Mask=Prim->CustomDepthStencilWriteMask;
            Original.Add(Prim,Prior);
            Prim->SetCustomDepthStencilValue(N==0 ? AnomalyStencilTag::ReservedStencilBase+3 : 17+N);
            Prim->SetCustomDepthStencilWriteMask(N==1 ? ERendererStencilMask::ERSM_1 : ERendererStencilMask::ERSM_Default);
            Prim->SetRenderCustomDepth(N!=2); ++N; break;
        }
    }
    for (TActorIterator<AActor> It(World);It;++It)
    {
        TInlineComponentArray<UPrimitiveComponent*> Prims(*It);
        for (UPrimitiveComponent* Prim:Prims)
        {
            FPrior V; V.On=Prim->bRenderCustomDepth; V.Value=Prim->CustomDepthStencilValue; V.Mask=Prim->CustomDepthStencilWriteMask; Before.Add(Prim,V);
        }
    }
    auto Protocol=MakeShared<FJsonObject>(); Protocol->SetStringField(TEXT("utc"),FDateTime::UtcNow().ToIso8601());
    Protocol->SetNumberField(TEXT("poses"),Poses); Protocol->SetNumberField(TEXT("yaw_step"),60);
    Protocol->SetNumberField(TEXT("turn"),Turn); Protocol->SetNumberField(TEXT("settle"),Settle); Protocol->SetNumberField(TEXT("clean_hold"),CleanHold); Protocol->SetNumberField(TEXT("score_hold"),ScoreHold);
    Protocol->SetNumberField(TEXT("pre_frames"),PreFrames); Protocol->SetNumberField(TEXT("total_frames"),TotalFrames);
    Protocol->SetArrayField(TEXT("base_position"),XYZ425(Position)); Protocol->SetArrayField(TEXT("base_rotation"),XYZ425(FVector(Base.Pitch,Base.Yaw,Base.Roll)));
    Protocol->SetStringField(TEXT("view_target"),GetNameSafe(Target.Get())); Protocol->SetNumberField(TEXT("grain_levels"),1); Protocol->SetNumberField(TEXT("minimum_stable_share"),0.0);
    Protocol->SetNumberField(TEXT("grain_period"),7); Protocol->SetNumberField(TEXT("unstable_dilation"),2);
    Protocol->SetStringField(TEXT("grain_mode"),TEXT("phase_matched_viewstate_GetFrameIndex8_modulo7"));
    Protocol->SetStringField(TEXT("reference_rule"),TEXT("FU5 office smoke: six fixed poses; 28 off/on frames per pose; 240 auto frames per pose; all poses enabled without label-free grading. Wall strip compared by render grain phase against the off-hold RGB range with one-level tolerance."));
    Protocol->SetNumberField(TEXT("setup_components"),Original.Num());
    return N==3 && JsonStreet425(Protocol,Directory/TEXT("c5-protocol.json")) && JsonStreet425(DepthSample(true),Directory/TEXT("depth-before.json"));
}
TSharedPtr<FJsonObject> FAnomalyStreetBench425::Describe(int32 Index) const
{
    const bool Clean=Index<PreFrames; const int32 Local=Clean?Index:Index-PreFrames;
    const int32 Span=Turn+Settle+(Clean?2*CleanHold:ScoreHold); const int32 P=FMath::Clamp(Local/Span,0,Poses-1),Step=Local%Span;
    auto R=MakeShared<FJsonObject>(); R->SetNumberField(TEXT("pose"),P); R->SetNumberField(TEXT("step"),Step);
    R->SetStringField(TEXT("phase"),Clean?TEXT("clean_prepass"):TEXT("scored"));
    R->SetStringField(TEXT("hold_kind"),!Clean?TEXT("scored"):Step<Turn+Settle?TEXT("settle"):Step<Turn+Settle+CleanHold?TEXT("census_off"):TEXT("census_on"));
    R->SetBoolField(TEXT("hold"),Step>=Turn+Settle); R->SetBoolField(TEXT("score"),!Clean && Frozen && Pose[P].Keep && Step>=Turn+Settle);
    return R;
}
void FAnomalyStreetBench425::Prepare(int32 Index)
{
    if (!Owner.IsValid()) { return; }
    APlayerController* PC=Owner->GetFirstPlayerController(); if (!PC) { return; }
    auto D=Describe(Index); const int32 P=int32(D->GetNumberField(TEXT("pose"))),Step=int32(D->GetNumberField(TEXT("step")));
    const bool Clean=Index<PreFrames;
    FAnomalyMasks425::SetBenchCensus(!Clean || Step>=Turn+Settle+CleanHold);
    const float To=Base.Yaw+60.0f*P; const float From=P==0 ? To : Base.Yaw+60.0f*(P-1);
    FRotator R=Base; R.Yaw=Step<Turn ? From+FMath::FindDeltaAngleDegrees(From,To)*float(Step+1)/Turn : To;
    PC->SetControlRotation(R);
    if (Target.IsValid() && Target.Get()!=PC->GetPawn()) { Target->SetActorRotation(R); }
}
bool FAnomalyStreetBench425::AutoOn(int32 Index) const
{
    auto D=Describe(Index); const int32 Step=int32(D->GetNumberField(TEXT("step")));
    return D->GetBoolField(TEXT("score")) && Step<Turn+Settle+ScoreHold-30;
}
TSharedPtr<FJsonObject> FAnomalyStreetBench425::DepthSample(bool Full) const
{
    auto R=MakeShared<FJsonObject>(); TArray<TSharedPtr<FJsonValue>> Values; int32 Diff=0,On=0;
    TSet<TWeakObjectPtr<UPrimitiveComponent>> Tagged; AnomalyStencilTag::GetTaggedComponents(Tagged);
    for (const auto& Pair:Before)
    {
        UPrimitiveComponent* Prim=Pair.Key.Get(); if (!Prim) { ++Diff; continue; }
        const bool Changed=bool(Prim->bRenderCustomDepth)!=Pair.Value.On || Prim->CustomDepthStencilValue!=Pair.Value.Value || Prim->CustomDepthStencilWriteMask!=Pair.Value.Mask;
        const bool IsTagged=Tagged.Contains(Pair.Key);
        if ((bool(Prim->bRenderCustomDepth)!=(IsTagged||Pair.Value.On)) || (!IsTagged && Changed) || (Pair.Value.On && (Changed || IsTagged))) { ++Diff; }
        On+=Prim->bRenderCustomDepth ? 1 : 0;
        if (Full || Prim->bRenderCustomDepth || Changed || Original.Contains(Pair.Key))
        {
            auto V=MakeShared<FJsonObject>(); V->SetStringField(TEXT("component"),Prim->GetPathName());
            V->SetBoolField(TEXT("on"),Prim->bRenderCustomDepth); V->SetNumberField(TEXT("value"),Prim->CustomDepthStencilValue); V->SetNumberField(TEXT("write_mask"),int32(Prim->CustomDepthStencilWriteMask));
            V->SetBoolField(TEXT("prior_on"),Pair.Value.On); V->SetNumberField(TEXT("prior_value"),Pair.Value.Value); V->SetNumberField(TEXT("prior_write_mask"),int32(Pair.Value.Mask));
            V->SetBoolField(TEXT("tagged"),IsTagged); V->SetBoolField(TEXT("bench_setup"),Original.Contains(Pair.Key)); Values.Add(MakeShared<FJsonValueObject>(V));
        }
    }
    R->SetNumberField(TEXT("candidates"),Before.Num()); R->SetNumberField(TEXT("on_count"),On); R->SetNumberField(TEXT("tagged_components"),Tagged.Num()); R->SetNumberField(TEXT("unexpected"),Diff); R->SetArrayField(TEXT("components"),Values); return R;
}
bool FAnomalyStreetBench425::Ready(int32 Index) const
{
    if (Index>=PreFrames) { return Frozen; }
    const int32 Span=Turn+Settle+2*CleanHold,P=Index/Span,Step=Index%Span;
    if (P>0 && !Pose[P-1].Complete) { return false; }
    return Step<Turn+Settle+CleanHold || Pose[P].OffReady;
}
bool FAnomalyStreetBench425::Observe(const FAnomalyCapturedFrame& Frame,const TSharedPtr<FJsonObject>& Row)
{
    const int32 Index=int32(Frame.RequestId-1); if (Index>=PreFrames) { return true; }
    auto D=Describe(Index); const int32 P=int32(D->GetNumberField(TEXT("pose")));
    if (!D->GetBoolField(TEXT("hold"))) { return true; }
    FPose& V=Pose[P]; const int32 Phase=Frame.LabelKey425.GrainPhase;
    if (Phase<0 || Phase>=7) { return false; }
    FPhase& F=V.Phase[Phase]; TArray<FColor> Pixels; Pixels.SetNumUninitialized(Frame.Width*Frame.Height);
    for (int32 I=0;I<Pixels.Num();++I)
    {
        const auto RGB=AnomalyPixel425::Decode(Frame.Format,AnomalyPixel425::Word(Frame.RawBytes.GetData()+I*4));
        if (!RGB.Valid) { return false; } Pixels[I]=FColor(RGB.R,RGB.G,RGB.B,255);
    }
    auto MakeMap=[&]()
    {
        TArray<uint8> Source,Bad; Source.SetNumZeroed(Pixels.Num()); Bad.SetNumZeroed(Pixels.Num());
        for (int32 Q=0;Q<7;++Q)
        {
            const FPhase& A=V.Phase[Q]; if (!A.Count) { continue; }
            for (int32 I=0;I<Pixels.Num();++I)
            {
                Source[I]|=(int32(A.Maximum[I].R)-A.Minimum[I].R>1 || int32(A.Maximum[I].G)-A.Minimum[I].G>1 || int32(A.Maximum[I].B)-A.Minimum[I].B>1)?1:0;
            }
        }
        for (int32 I=0;I<Pixels.Num();++I)
        {
            if (!Source[I]) { continue; }
            const int32 X=I%Frame.Width,Y=I/Frame.Width;
            for (int32 DY=-2;DY<=2;++DY) { for (int32 DX=-2;DX<=2;++DX)
            {
                const int32 XX=X+DX,YY=Y+DY;
                if (XX>=0 && YY>=0 && XX<Frame.Width && YY<Frame.Height) { Bad[YY*Frame.Width+XX]=1; }
            } }
        }
        return Bad;
    };
    auto Compare=[&](const TArray<uint8>& Bad)
    {
        int32 Changed=0,Stable=0;
        for (int32 I=0;I<Pixels.Num();++I)
        {
            if (Bad[I]) { continue; } ++Stable;
            const FColor A=Pixels[I],B=F.Reference[I];
            Changed+=(FMath::Abs(int32(A.R)-B.R)>1 || FMath::Abs(int32(A.G)-B.G)>1 || FMath::Abs(int32(A.B)-B.B)>1)?1:0;
        }
        auto R=MakeShared<FJsonObject>(); R->SetNumberField(TEXT("index"),Index); R->SetNumberField(TEXT("changed_stable_pixels"),Changed);
        R->SetNumberField(TEXT("stable_pixels"),Stable); R->SetNumberField(TEXT("fraction"),double(Changed)/FMath::Max(1,Stable)); return R;
    };
    if (D->GetStringField(TEXT("hold_kind"))==TEXT("census_off"))
    {
        if (!F.Count) { F.Reference=Pixels; F.Minimum=Pixels; F.Maximum=Pixels; F.Index=Index; }
        if (V.Count>=CleanHold/2)
        {
            auto Self=Compare(V.HalfMap); V.SelfMax=FMath::Max(V.SelfMax,Self->GetNumberField(TEXT("fraction"))); V.SelfFrames.Add(MakeShared<FJsonValueObject>(Self));
        }
        for (int32 I=0;I<Pixels.Num();++I)
        {
            F.Minimum[I].R=FMath::Min(F.Minimum[I].R,Pixels[I].R); F.Maximum[I].R=FMath::Max(F.Maximum[I].R,Pixels[I].R);
            F.Minimum[I].G=FMath::Min(F.Minimum[I].G,Pixels[I].G); F.Maximum[I].G=FMath::Max(F.Maximum[I].G,Pixels[I].G);
            F.Minimum[I].B=FMath::Min(F.Minimum[I].B,Pixels[I].B); F.Maximum[I].B=FMath::Max(F.Maximum[I].B,Pixels[I].B);
        }
        ++F.Count; ++V.Count;
        auto S=MakeShared<FJsonObject>(); S->SetNumberField(TEXT("index"),Index); S->SetNumberField(TEXT("phase"),Phase); V.OffSamples.Add(MakeShared<FJsonValueObject>(S));
        if (V.Count==CleanHold/2) { V.HalfMap=MakeMap(); }
        if (V.Count==CleanHold)
        {
            V.Bad=MakeMap(); int32 BadCount=0; for (uint8 B:V.Bad) { BadCount+=B; }
            V.Unstable=double(BadCount)/V.Bad.Num(); V.OffReady=true;
            for (int32 Q=0;Q<7;++Q) { if (!V.Phase[Q].Count) { return false; } V.Phase[Q].Minimum.Empty(); V.Phase[Q].Maximum.Empty(); }
        }
    }
    else
    {
        if (!V.OffReady || !F.Count) { return false; }
        auto Test=Compare(V.Bad); V.InterferenceMax=FMath::Max(V.InterferenceMax,Test->GetNumberField(TEXT("fraction"))); V.InterferenceFrames.Add(MakeShared<FJsonValueObject>(Test));
        auto Sample=MakeShared<FJsonObject>(); Sample->SetNumberField(TEXT("index"),Index); Sample->SetObjectField(TEXT("census"),Row->GetObjectField(TEXT("census")));
        V.Samples.Add(MakeShared<FJsonValueObject>(Sample)); ++V.OnCount;
        if (V.OnCount==CleanHold)
        {
            V.Keep=true; V.Complete=true;
        }
    }
    return Index!=PreFrames-1 || Freeze(Frame.Width,Frame.Height);
}
bool FAnomalyStreetBench425::Freeze(int32 Width,int32 Height)
{
    auto R=MakeShared<FJsonObject>(); TArray<TSharedPtr<FJsonValue>> Rows;
    for (int32 P=0;P<Poses;++P)
    {
        FPose& V=Pose[P]; if (!V.Complete || V.Count!=CleanHold || V.OnCount!=CleanHold) { return false; }
        TArray<TSharedPtr<FJsonValue>> Refs;
        for (int32 Q=0;Q<7;++Q)
        {
            const FString Name=FString::Printf(TEXT("pose-%02d-phase-%d-reference.png"),P,Q); TArray<uint8> Bytes;
            if (!AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,V.Phase[Q].Reference,Width,Height,Bytes,0) || !FFileHelper::SaveArrayToFile(Bytes,*(Directory/Name))) { return false; }
            auto Ref=MakeShared<FJsonObject>(); Ref->SetNumberField(TEXT("phase"),Q); Ref->SetNumberField(TEXT("index"),V.Phase[Q].Index); Ref->SetStringField(TEXT("file"),Name); Refs.Add(MakeShared<FJsonValueObject>(Ref));
            V.Phase[Q].Reference.Empty();
        }
        const FString Bad=FString::Printf(TEXT("pose-%02d-unstable.png"),P); TArray<FColor> Pixels; Pixels.SetNumUninitialized(V.Bad.Num());
        for (int32 I=0;I<Pixels.Num();++I) { Pixels[I]=V.Bad[I]?FColor::White:FColor::Black; }
        TArray<uint8> Bytes;
        if (!AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,Pixels,Width,Height,Bytes,0) || !FFileHelper::SaveArrayToFile(Bytes,*(Directory/Bad))) { return false; }
        auto Row=MakeShared<FJsonObject>(); Row->SetNumberField(TEXT("pose"),P); Row->SetNumberField(TEXT("unstable_share"),V.Unstable); Row->SetBoolField(TEXT("kept"),V.Keep);
        Row->SetArrayField(TEXT("references"),Refs); Row->SetStringField(TEXT("unstable"),Bad); Row->SetArrayField(TEXT("off_samples"),V.OffSamples); Row->SetArrayField(TEXT("silhouette_samples"),V.Samples);
        Row->SetNumberField(TEXT("self_max"),V.SelfMax); Row->SetNumberField(TEXT("interference_max"),V.InterferenceMax);
        Row->SetArrayField(TEXT("self_frames"),V.SelfFrames); Row->SetArrayField(TEXT("interference_frames"),V.InterferenceFrames); Rows.Add(MakeShared<FJsonValueObject>(Row));
        V.Bad.Empty(); V.HalfMap.Empty(); V.Samples.Empty(); V.OffSamples.Empty(); V.SelfFrames.Empty(); V.InterferenceFrames.Empty();
    }
    R->SetStringField(TEXT("frozen_utc"),FDateTime::UtcNow().ToIso8601()); R->SetArrayField(TEXT("poses"),Rows);
    Frozen=JsonStreet425(R,Directory/TEXT("prepass-frozen.json")); return Frozen;
}
bool FAnomalyStreetBench425::Stop()
{
    auto After=DepthSample(true); const bool Good=After->GetNumberField(TEXT("unexpected"))==0 && After->GetNumberField(TEXT("tagged_components"))==0;
    JsonStreet425(After,Directory/TEXT("depth-after.json"));
    for (const auto& Pair:Original) { if (Pair.Key.IsValid()) { Pair.Key->SetCustomDepthStencilValue(Pair.Value.Value); Pair.Key->SetCustomDepthStencilWriteMask(Pair.Value.Mask); Pair.Key->SetRenderCustomDepth(Pair.Value.On); } }
    if (Owner.IsValid()) { if (APlayerController* PC=Owner->GetFirstPlayerController()) { PC->SetControlRotation(OriginalViewRotation); PC->SetIgnoreMoveInput(false); PC->SetIgnoreLookInput(false); } }
    if (Target.IsValid() && Owner.IsValid() && Target.Get()!=Owner->GetFirstPlayerController()->GetPawn()) { Target->SetActorRotation(TargetRotation); }
    return Good;
}
#endif
