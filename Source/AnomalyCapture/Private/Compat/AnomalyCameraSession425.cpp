#include "AnomalyCameraSession425.h"
#include "AnomalyBenchGate.h"
#if ANOMALY_CAPTURE_FRAMES425
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyCaptureLog.h"
#include "AnomalyStreetBench425.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

int32 FAnomalyCameraSession425::FrameCount(UWorld* World,const TArray<FString>& Routes)
{
#if !UE_BUILD_SHIPPING
    if (World && World->GetMapName()==TEXT("EA_Core") && Routes.Contains(TEXT("camera_clipping")) &&
        AnomalyBenchGate::IsCapture425Enabled() && FParse::Param(FCommandLine::Get(),TEXT("IAICameraSlabBench")) &&
        FParse::Param(FCommandLine::Get(),TEXT("IAICameraRegression")))
    {
        return FAnomalyStreetBench425::TotalFrames+(FParse::Param(FCommandLine::Get(),TEXT("IAICameraFarOnly"))?0:6*48);
    }
#endif
    return FAnomalyStreetBench425::TotalFrames;
}
FAnomalyCameraSession425::~FAnomalyCameraSession425() { Stop(); }
bool FAnomalyCameraSession425::Start(UWorld* World,const TArray<FString>& Routes,const FString& Directory)
{
    Owner=World; Output=Directory; Baseline=GNearClippingPlane;
    if (!Routes.Contains(TEXT("camera_clipping"))) { return true; }
    auto* Injector=World ? World->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
    if (!Injector) { return false; }
    TArray<FString> Args;
#if !UE_BUILD_SHIPPING
    Bench=World->GetMapName()==TEXT("EA_Core") && AnomalyBenchGate::IsCapture425Enabled() &&
        FParse::Param(FCommandLine::Get(),TEXT("IAICameraSlabBench"));
    if (Bench)
    {
        const bool Regression=FParse::Param(FCommandLine::Get(),TEXT("IAICameraRegression"));
        FarOnly=Regression && FParse::Param(FCommandLine::Get(),TEXT("IAICameraFarOnly"));
        NearStart=Regression?FAnomalyStreetBench425::TotalFrames:FAnomalyStreetBench425::PreFrames; PoseHold=Regression?48:288;
        auto* PC=World->GetFirstPlayerController(); if (!PC) { return false; }
        AActor* Target=nullptr;
        for (TActorIterator<AActor> It(World);It;++It)
        {
            if (It->ActorHasTag(TEXT("EA_Camera")) && It->IsA<ACameraActor>()) { Camera=*It; }
            if (It->ActorHasTag(TEXT("EA_LOD"))) { Target=*It; }
        }
        if (!Camera.IsValid() || !Target) { return false; }
        auto* Mesh=Target->FindComponentByClass<UStaticMeshComponent>(); if (!Mesh) { return false; }
        PriorTarget=PC->GetViewTarget(); PriorAutoManage=PC->bAutoManageActiveCameraTarget;
        FarPosition=PriorPosition=Camera->GetActorLocation(); FarRotation=PriorRotation=Camera->GetActorRotation();
        PriorFov=CastChecked<ACameraActor>(Camera.Get())->GetCameraComponent()->FieldOfView;
        const FBox Bounds=Mesh->Bounds.GetBox(); NearPosition=Bounds.GetCenter(); NearPosition.X=Bounds.Min.X-50.0f;
        if (FParse::Param(FCommandLine::Get(),TEXT("IAICameraBaseline"))) { Args.Add(FString::SanitizeFloat(Baseline)); }
        Prepare(0);
    }
#endif
    Active=Injector->ApplyAnomaly(FName(TEXT("camera_clipping")),Args); StartFrame=GFrameCounter;
    auto Receipt=MakeShared<FJsonObject>(); Receipt->SetBoolField(TEXT("held"),Active);
    Receipt->SetBoolField(TEXT("bench"),Bench); Receipt->SetNumberField(TEXT("baseline_near"),Baseline);
    Receipt->SetNumberField(TEXT("held_near"),GNearClippingPlane);
    Receipt->SetStringField(TEXT("pose_rule"),TEXT("near_start far frames; six pose_hold-frame alternating near/far holds, starting near; near eye 50cm before EA_LOD front bounds"));
    Receipt->SetStringField(TEXT("near_position"),NearPosition.ToString()); Receipt->SetStringField(TEXT("far_position"),FarPosition.ToString());
    Receipt->SetNumberField(TEXT("start_frame"),double(StartFrame));
    Receipt->SetNumberField(TEXT("near_start"),NearStart); Receipt->SetNumberField(TEXT("pose_hold"),PoseHold);
    Receipt->SetBoolField(TEXT("far_only"),FarOnly);
    FString Text; auto W=TJsonWriterFactory<>::Create(&Text); FJsonSerializer::Serialize(Receipt,W);
    FFileHelper::SaveStringToFile(Text,*(Output/TEXT("camera-session-start.json")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 camera_clipping session held=%d baseline=%.3f near=%.3f bench=%d"),Active,Baseline,GNearClippingPlane,Bench);
    return Active;
}
void FAnomalyCameraSession425::Prepare(int32 Index)
{
#if !UE_BUILD_SHIPPING
    if (!Bench || !Owner.IsValid() || !Camera.IsValid()) { return; }
    auto* PC=Owner->GetFirstPlayerController(); if (!PC) { return; }
    const bool Near=!FarOnly && Index>=NearStart && ((Index-NearStart)/PoseHold)%2==0;
    const FRotator Rotation=Near ? FRotator::ZeroRotator : FarRotation;
    Camera->SetActorLocationAndRotation(Near ? NearPosition : FarPosition,Rotation);
    auto* C=CastChecked<ACameraActor>(Camera.Get())->GetCameraComponent(); C->SetFieldOfView(PriorFov);
    PC->bAutoManageActiveCameraTarget=false; PC->SetViewTarget(Camera.Get()); PC->SetControlRotation(Rotation);
#endif
}
void FAnomalyCameraSession425::Append(const TSharedPtr<FJsonObject>& Row,TArray<FAutoLiveFireInfo>& Fires)
{
    auto* Injector=Owner.IsValid() ? Owner->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
    if (!Active || !Injector) { return; }
    const FName Id(TEXT("camera_clipping")); const bool bPositive=Injector->IsAnomalyCurrentlyAnomalous(Id);
    FAnomalyNearClipSlabResult Slab; bool bSphereProxy=false;
    if (Injector->GetCameraClippingFrameEvaluation(Slab,bSphereProxy))
    {
		if (true)
		{
			Row->SetBoolField(TEXT("camera_clipping.slab"), bPositive);
			Row->SetBoolField(TEXT("camera_clipping.sphere_proxy"), bSphereProxy);
			Row->SetNumberField(TEXT("camera_clipping.slab_primitives"), Slab.SlabPrimitives);
			Row->SetNumberField(TEXT("camera_clipping.eye_inside_box"), Slab.EyeInsideBox);
			Row->SetBoolField(TEXT("camera_clipping.bounds_candidate"), Slab.bSatPositive);
			Row->SetNumberField(TEXT("camera_clipping.clipped_ray_fraction"), Slab.ClippedRayFraction);
			Row->SetNumberField(TEXT("camera_clipping.confirm_traces"), Slab.ConfirmTraces);
			Row->SetNumberField(TEXT("camera_clipping.confirm_hits"), Slab.ConfirmHits);
			Row->SetNumberField(TEXT("camera_clipping.confirm_misses"), Slab.ConfirmMisses);
			Row->SetNumberField(TEXT("camera_clipping.confirm_unresolved"), (Slab.ConfirmUnconfirmable + Slab.ConfirmOverCap + Slab.ConfirmTraceCapped + Slab.ConfirmNoRay + Slab.ConfirmTooFewValid));
			if ((bPositive && Slab.bUnconfirmed))
			{
				Row->SetBoolField(TEXT("camera_clipping.unconfirmed"), true);
			}
			if (Slab.bNearOverridden)
			{
				Row->SetBoolField(TEXT("camera_clipping.near_overridden"), true);
			}
		}
			FCameraClipRunAccum& A = Accum;
			A.BoundsCandidateFrames += Slab.bSatPositive ? 1 : 0;
			A.ConfirmedPositiveFrames += (bPositive && !Slab.bUnconfirmed) ? 1 : 0;
			A.UnconfirmedFrames += (bPositive && Slab.bUnconfirmed) ? 1 : 0;
			A.RejectedByTrianglesFrames += (Slab.bSatPositive && !bPositive) ? 1 : 0;
			A.ConfirmTracesTotal += Slab.ConfirmTraces;
			A.ConfirmTracesMax = FMath::Max(A.ConfirmTracesMax, Slab.ConfirmTraces);
			A.UnconfirmableTotal += Slab.ConfirmUnconfirmable;
			A.OverCapTotal += Slab.ConfirmOverCap;
			A.TraceCappedTotal += Slab.ConfirmTraceCapped;
			A.NoRayTotal += Slab.ConfirmNoRay;
			A.TooFewValidTotal += Slab.ConfirmTooFewValid;
			A.FullSlabFallbacksTotal += Slab.ConfirmFullSlabFallbacks;
			A.InvalidSegmentsTotal += Slab.ConfirmInvalidSegments;
			A.LandscapeTotal += Slab.LandscapeCandidates;
			A.SkinnedTotal += Slab.SkinnedUnconfirmable;
			A.NoCollisionTotal += Slab.NoCollisionUnconfirmable;
			A.NoComplexTotal += Slab.NoComplexUnconfirmable;
			A.WeldedTotal += Slab.WeldedUnconfirmable;
			A.InstanceTransformTotal += Slab.InstanceTransformUnconfirmable;
			A.ConfirmMicrosTotal += Slab.ConfirmMicros;
			A.ConfirmMicrosLabelledTotal += bPositive ? Slab.ConfirmMicros : 0.0;
			A.ConfirmMicrosMax = FMath::Max(A.ConfirmMicrosMax, Slab.ConfirmMicros);
			A.ClippedRayFractionSum += (bPositive && !Slab.bUnconfirmed) ? Slab.ClippedRayFraction : 0.0;
			++A.FramesEvaluated;
			A.SlabPositiveFrames += bPositive ? 1 : 0;
			A.SphereProxyPositiveFrames += bSphereProxy ? 1 : 0;
			A.SlabOnlyFrames += (bPositive && !bSphereProxy) ? 1 : 0;
			A.ProxyOnlyFrames += (!bPositive && bSphereProxy) ? 1 : 0;
			A.EyeInsideBoxFrames += Slab.EyeInsideBox > 0 ? 1 : 0;
			A.NearOverriddenFrames += Slab.bNearOverridden ? 1 : 0;
			A.BoxFallbacks += Slab.BoxFallbacks;
			A.FxExcludedMax = FMath::Max(A.FxExcludedMax, Slab.FxExcluded);
			A.EnumeratedTotal += Slab.Enumerated;
			A.CandidatesTotal += Slab.Candidates;
			A.InstancesTestedTotal += Slab.InstancesTested;
			A.MicrosTotal += Slab.Micros;
			A.MicrosMax = FMath::Max(A.MicrosMax, Slab.Micros);
			A.BaselineNear = Slab.BaselineNear;
			A.AnomalousNear = Slab.AnomalousNear;
			if (bPositive && !Slab.FirstHit.IsEmpty())
			{
				++A.FirstHits.FindOrAdd(Slab.FirstHit);
			}
    }
    if (bPositive)
    {
        FAutoLiveFireInfo F; F.Id=Id; F.Target=FString(); F.TargetActor=nullptr; F.SecondsRemaining=0.0f;
        F.StartFrame=StartFrame; F.bWholeFrameExtent=true; Fires.Add(F);
    }
}
bool FAnomalyCameraSession425::Stop()
{
    if (!Active) { return true; }
    auto* Injector=Owner.IsValid() ? Owner->GetSubsystem<UAnomalyInjectorSubsystem>() : nullptr;
    if (Injector) { Injector->RevertAnomaly(FName(TEXT("camera_clipping"))); }
    Active=false;
    if (Bench && Camera.IsValid() && Owner.IsValid())
    {
        Camera->SetActorLocationAndRotation(PriorPosition,PriorRotation);
        CastChecked<ACameraActor>(Camera.Get())->GetCameraComponent()->SetFieldOfView(PriorFov);
        if (auto* PC=Owner->GetFirstPlayerController()) { PC->bAutoManageActiveCameraTarget=PriorAutoManage; if (PriorTarget.IsValid()) { PC->SetViewTarget(PriorTarget.Get()); } }
    }
    UE_LOG(LogAnomalyCapture,Log,TEXT("Capture425 camera_clipping session reverted baseline=%.3f current=%.3f"),Baseline,GNearClippingPlane);
    return GNearClippingPlane==Baseline;
}
TSharedPtr<FJsonObject> FAnomalyCameraSession425::Summary() const
{
    auto Root=MakeShared<FJsonObject>(); const FCameraClipRunAccum* CameraClip=&Accum;
		if (CameraClip && CameraClip->FramesEvaluated > 0)
		{
			const double Frames = (double)CameraClip->FramesEvaluated;
			Root->SetStringField(TEXT("camera_clipping_label_rule"), TEXT("view_slab_bounds_then_triangle_confirm_v4"));
			Root->SetNumberField(TEXT("camera_clipping_bounds_candidate_frames"), CameraClip->BoundsCandidateFrames);
			Root->SetNumberField(TEXT("camera_clipping_confirmed_positive_frames"), CameraClip->ConfirmedPositiveFrames);
			Root->SetNumberField(TEXT("camera_clipping_unconfirmed_frames"), CameraClip->UnconfirmedFrames);
			Root->SetNumberField(TEXT("camera_clipping_bounds_rejected_by_triangles_frames"), CameraClip->RejectedByTrianglesFrames);
			Root->SetNumberField(TEXT("camera_clipping_confirm_traces_total"), (double)CameraClip->ConfirmTracesTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_traces_max"), CameraClip->ConfirmTracesMax);
			Root->SetNumberField(TEXT("camera_clipping_confirm_unconfirmable_candidates"), (double)CameraClip->UnconfirmableTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_over_cap_candidates"), (double)CameraClip->OverCapTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_trace_capped_candidates"), (double)CameraClip->TraceCappedTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_no_ray_candidates"), (double)CameraClip->NoRayTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_too_few_valid_trace_candidates"), (double)CameraClip->TooFewValidTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_full_slab_fallback_traces"), (double)CameraClip->FullSlabFallbacksTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_invalid_segments"), (double)CameraClip->InvalidSegmentsTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_landscape_candidates"), (double)CameraClip->LandscapeTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_skinned_unconfirmable"), (double)CameraClip->SkinnedTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_no_collision_unconfirmable"), (double)CameraClip->NoCollisionTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_no_complex_unconfirmable"), (double)CameraClip->NoComplexTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_welded_unconfirmable"), (double)CameraClip->WeldedTotal);
			Root->SetNumberField(TEXT("camera_clipping_confirm_instance_transform_unconfirmable"), (double)CameraClip->InstanceTransformTotal);
			const double LabelledFrames = (double)FMath::Max(1, CameraClip->ConfirmedPositiveFrames + CameraClip->UnconfirmedFrames);
			Root->SetNumberField(TEXT("camera_clipping_confirm_us_mean_per_labelled_frame"), CameraClip->ConfirmMicrosLabelledTotal / LabelledFrames);
			Root->SetNumberField(TEXT("camera_clipping_confirm_us_mean_per_candidate_frame"),
				CameraClip->ConfirmMicrosTotal / (double)FMath::Max(1, CameraClip->BoundsCandidateFrames));
			Root->SetNumberField(TEXT("camera_clipping_confirm_us_max"), CameraClip->ConfirmMicrosMax);
			Root->SetNumberField(TEXT("camera_clipping_confirm_budget_us"), 500.0);
			Root->SetNumberField(TEXT("camera_clipping_clipped_ray_fraction_mean_confirmed"),
				CameraClip->ClippedRayFractionSum / (double)FMath::Max(1, CameraClip->ConfirmedPositiveFrames));
			Root->SetNumberField(TEXT("camera_clipping_baseline_near"), CameraClip->BaselineNear);
			Root->SetNumberField(TEXT("camera_clipping_anomalous_near"), CameraClip->AnomalousNear);
			Root->SetNumberField(TEXT("camera_clipping_frames_evaluated"), CameraClip->FramesEvaluated);
			Root->SetNumberField(TEXT("camera_clipping_slab_positive_frames"), CameraClip->SlabPositiveFrames);
			Root->SetNumberField(TEXT("camera_clipping_sphere_proxy_positive_frames"), CameraClip->SphereProxyPositiveFrames);
			Root->SetNumberField(TEXT("camera_clipping_slab_only_frames"), CameraClip->SlabOnlyFrames);
			Root->SetNumberField(TEXT("camera_clipping_sphere_proxy_only_frames"), CameraClip->ProxyOnlyFrames);
			Root->SetNumberField(TEXT("camera_clipping_eye_inside_box_frames"), CameraClip->EyeInsideBoxFrames);
			Root->SetNumberField(TEXT("camera_clipping_near_overridden_frames"), CameraClip->NearOverriddenFrames);
			Root->SetNumberField(TEXT("camera_clipping_box_fallbacks"), CameraClip->BoxFallbacks);
			Root->SetNumberField(TEXT("camera_clipping_fx_excluded_max"), CameraClip->FxExcludedMax);
			Root->SetNumberField(TEXT("camera_clipping_primitives_enumerated_mean"), CameraClip->EnumeratedTotal / Frames);
			Root->SetNumberField(TEXT("camera_clipping_candidates_mean"), CameraClip->CandidatesTotal / Frames);
			Root->SetNumberField(TEXT("camera_clipping_instances_tested_total"), (double)CameraClip->InstancesTestedTotal);
			Root->SetNumberField(TEXT("camera_clipping_eval_ms_total"), CameraClip->MicrosTotal / 1000.0);
			Root->SetNumberField(TEXT("camera_clipping_eval_us_mean"), CameraClip->MicrosTotal / Frames);
			Root->SetNumberField(TEXT("camera_clipping_eval_us_max"), CameraClip->MicrosMax);
			TArray<TPair<FString, int32>> Hits;
			for (const TPair<FString, int32>& Hit : CameraClip->FirstHits)
			{
				Hits.Add(Hit);
			}
			Hits.Sort([](const TPair<FString, int32>& A, const TPair<FString, int32>& B)
			{
				return A.Value != B.Value ? A.Value > B.Value : A.Key < B.Key;
			});
			TArray<TSharedPtr<FJsonValue>> HitValues;
			for (int32 i = 0; i < Hits.Num() && i < 16; ++i)
			{
				HitValues.Add(MakeShared<FJsonValueString>(FString::Printf(TEXT("%s=%d"), *Hits[i].Key, Hits[i].Value)));
			}
			Root->SetArrayField(TEXT("camera_clipping_first_hits"), HitValues);
		}
    Root->SetNumberField(TEXT("session_global_baseline_near"),Baseline);
    Root->SetNumberField(TEXT("session_global_restored_near"),GNearClippingPlane);
    Root->SetBoolField(TEXT("session_global_near_restored"),GNearClippingPlane==Baseline);
    return Root;
}
#endif
