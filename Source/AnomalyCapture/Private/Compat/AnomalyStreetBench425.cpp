#include "AnomalyStreetBench425.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyPixel425.h"
#include "AnomalyPreviewCapture.h"
#include "AnomalyStencilTag.h"
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
    auto Candidates=AnomalyViewport::GetCensusPrefilterActors(World);
    Candidates.Sort([](const TWeakObjectPtr<AActor>& A,const TWeakObjectPtr<AActor>& B) { return GetNameSafe(A.Get())<GetNameSafe(B.Get()); });
    int32 N=0;
    for (const auto& A:Candidates)
    {
        if (!A.IsValid() || N>=3) { continue; }
        TInlineComponentArray<UPrimitiveComponent*> Prims(A.Get());
        for (UPrimitiveComponent* Prim:Prims)
        {
            if (!AnomalyViewport::IsRenderableComponent(Prim)) { continue; }
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
    Protocol->SetStringField(TEXT("view_target"),GetNameSafe(Target.Get())); Protocol->SetNumberField(TEXT("grain_levels"),1); Protocol->SetNumberField(TEXT("drop_above_unstable_share"),0.05);
    Protocol->SetStringField(TEXT("reference_rule"),TEXT("First settled clean-hold frame per pose; unstable iff channel max-min exceeds one over all clean-hold frames; drop above 5 percent; freeze all before scored pass."));
    Protocol->SetNumberField(TEXT("setup_components"),Original.Num());
    return N==3 && JsonStreet425(Protocol,Directory/TEXT("c5-protocol.json")) && JsonStreet425(DepthSample(true),Directory/TEXT("depth-before.json"));
}
TSharedPtr<FJsonObject> FAnomalyStreetBench425::Describe(int32 Index) const
{
    const bool Clean=Index<PreFrames; const int32 Local=Clean?Index:Index-PreFrames;
    const int32 Span=Turn+Settle+(Clean?CleanHold:ScoreHold); const int32 P=FMath::Clamp(Local/Span,0,Poses-1),Step=Local%Span;
    auto R=MakeShared<FJsonObject>(); R->SetNumberField(TEXT("pose"),P); R->SetNumberField(TEXT("step"),Step);
    R->SetStringField(TEXT("phase"),Clean?TEXT("clean_prepass"):TEXT("scored"));
    R->SetBoolField(TEXT("hold"),Step>=Turn+Settle); R->SetBoolField(TEXT("score"),!Clean && Frozen && Pose[P].Keep && Step>=Turn+Settle);
    return R;
}
void FAnomalyStreetBench425::Prepare(int32 Index)
{
    if (!Owner.IsValid()) { return; }
    APlayerController* PC=Owner->GetFirstPlayerController(); if (!PC) { return; }
    auto D=Describe(Index); const int32 P=int32(D->GetNumberField(TEXT("pose"))),Step=int32(D->GetNumberField(TEXT("step")));
    const float To=Base.Yaw+60.0f*P; const float From=Base.Yaw+60.0f*((P+Poses-1)%Poses);
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
        if ((bool(Prim->bRenderCustomDepth)!=(IsTagged||Pair.Value.On)) || (!IsTagged && Changed)) { ++Diff; }
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
bool FAnomalyStreetBench425::Observe(const FAnomalyCapturedFrame& Frame,const TSharedPtr<FJsonObject>& Row)
{
    const int32 Index=int32(Frame.RequestId-1); if (Index>=PreFrames) { return true; }
    auto D=Describe(Index); const int32 P=int32(D->GetNumberField(TEXT("pose")));
    if (D->GetBoolField(TEXT("hold")))
    {
        FPose& V=Pose[P]; TArray<FColor> Pixels; Pixels.SetNumUninitialized(Frame.Width*Frame.Height);
        for (int32 I=0;I<Pixels.Num();++I)
        {
            const auto RGB=AnomalyPixel425::Decode(Frame.Format,AnomalyPixel425::Word(Frame.RawBytes.GetData()+I*4));
            if (!RGB.Valid) { return false; } Pixels[I]=FColor(RGB.R,RGB.G,RGB.B,255);
        }
        if (!V.Count) { V.Reference=Pixels; V.Minimum=Pixels; V.Maximum=Pixels; V.Index=Index; }
        for (int32 I=0;I<Pixels.Num();++I)
        {
            V.Minimum[I].R=FMath::Min(V.Minimum[I].R,Pixels[I].R); V.Maximum[I].R=FMath::Max(V.Maximum[I].R,Pixels[I].R);
            V.Minimum[I].G=FMath::Min(V.Minimum[I].G,Pixels[I].G); V.Maximum[I].G=FMath::Max(V.Maximum[I].G,Pixels[I].G);
            V.Minimum[I].B=FMath::Min(V.Minimum[I].B,Pixels[I].B); V.Maximum[I].B=FMath::Max(V.Maximum[I].B,Pixels[I].B);
        }
        auto Sample=MakeShared<FJsonObject>(); Sample->SetNumberField(TEXT("index"),Index); Sample->SetObjectField(TEXT("census"),Row->GetObjectField(TEXT("census")));
        V.Samples.Add(MakeShared<FJsonValueObject>(Sample)); ++V.Count;
    }
    return Index!=PreFrames-1 || Freeze(Frame.Width,Frame.Height);
}
bool FAnomalyStreetBench425::Freeze(int32 Width,int32 Height)
{
    auto R=MakeShared<FJsonObject>(); TArray<TSharedPtr<FJsonValue>> Rows;
    for (int32 P=0;P<Poses;++P)
    {
        FPose& V=Pose[P]; if (V.Count!=CleanHold) { return false; }
        TArray<FColor> Unstable; Unstable.SetNumUninitialized(V.Reference.Num()); int32 Count=0;
        for (int32 I=0;I<Unstable.Num();++I)
        {
            const bool U=int32(V.Maximum[I].R)-V.Minimum[I].R>1 || int32(V.Maximum[I].G)-V.Minimum[I].G>1 || int32(V.Maximum[I].B)-V.Minimum[I].B>1;
            Count+=U?1:0; Unstable[I]=U?FColor::White:FColor::Black;
        }
        V.Unstable=double(Count)/Unstable.Num(); V.Keep=V.Unstable<=0.05;
        const FString Ref=FString::Printf(TEXT("pose-%02d-reference.png"),P),Bad=FString::Printf(TEXT("pose-%02d-unstable.png"),P);
        TArray<uint8> Bytes;
        if (!AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,V.Reference,Width,Height,Bytes,0) || !FFileHelper::SaveArrayToFile(Bytes,*(Directory/Ref))) { return false; }
        Bytes.Reset(); if (!AnomalyPreview::EncodePixels(AnomalyPreview::EImageFormat::PNG,Unstable,Width,Height,Bytes,0) || !FFileHelper::SaveArrayToFile(Bytes,*(Directory/Bad))) { return false; }
        auto Row=MakeShared<FJsonObject>(); Row->SetNumberField(TEXT("pose"),P); Row->SetNumberField(TEXT("reference_index"),V.Index); Row->SetNumberField(TEXT("unstable_share"),V.Unstable); Row->SetBoolField(TEXT("kept"),V.Keep);
        Row->SetStringField(TEXT("reference"),Ref); Row->SetStringField(TEXT("unstable"),Bad); Row->SetArrayField(TEXT("silhouette_samples"),V.Samples); Rows.Add(MakeShared<FJsonValueObject>(Row));
        V.Reference.Empty(); V.Minimum.Empty(); V.Maximum.Empty(); V.Samples.Empty();
    }
    R->SetStringField(TEXT("frozen_utc"),FDateTime::UtcNow().ToIso8601()); R->SetArrayField(TEXT("poses"),Rows);
    Frozen=JsonStreet425(R,Directory/TEXT("prepass-frozen.json")); return Frozen;
}
bool FAnomalyStreetBench425::Stop()
{
    auto After=DepthSample(true); const bool Good=After->GetNumberField(TEXT("unexpected"))==0 && After->GetNumberField(TEXT("tagged_components"))==0;
    JsonStreet425(After,Directory/TEXT("depth-after.json"));
    for (const auto& Pair:Original) { if (Pair.Key.IsValid()) { Pair.Key->SetCustomDepthStencilValue(Pair.Value.Value); Pair.Key->SetCustomDepthStencilWriteMask(Pair.Value.Mask); Pair.Key->SetRenderCustomDepth(Pair.Value.On); } }
    if (Owner.IsValid()) { if (APlayerController* PC=Owner->GetFirstPlayerController()) { PC->SetControlRotation(Base); PC->SetIgnoreMoveInput(false); PC->SetIgnoreLookInput(false); } }
    if (Target.IsValid() && Owner.IsValid() && Target.Get()!=Owner->GetFirstPlayerController()->GetPawn()) { Target->SetActorRotation(TargetRotation); }
    return Good;
}
#endif
