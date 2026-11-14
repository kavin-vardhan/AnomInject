#include "AnomalyLodTrigger.h"

#include "AnomalyDefaults.h"
#include "AnomalyViewport.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/UObjectIterator.h"

namespace
{
	struct FViewSample
	{
		FVector Location = FVector::ZeroVector;
		float Yaw = 0.0f;
		double Time = 0.0;
		uint64 Frame = 0;
	};

	static constexpr int32 RingSize = 16;
	static constexpr int32 MotionFramesBack = 8;
	static constexpr float CandidateMaxDistanceCm = 10000.0f;

	struct FViewRing
	{
		TWeakObjectPtr<UWorld> World;
		FViewSample Samples[RingSize];
		int32 Count = 0;
		int32 Head = 0;
	};

	FViewRing& Ring()
	{
		static FViewRing Instance;
		return Instance;
	}

	float CoveragePctFromRect(const FVector2D& Min, const FVector2D& Max)
	{
		const double W = FMath::Clamp((double)Max.X, 0.0, 1.0) - FMath::Clamp((double)Min.X, 0.0, 1.0);
		const double H = FMath::Clamp((double)Max.Y, 0.0, 1.0) - FMath::Clamp((double)Min.Y, 0.0, 1.0);
		return (float)(FMath::Max(0.0, W) * FMath::Max(0.0, H) * 100.0);
	}
}

AnomalyLodTriggerRule::FSettings AnomalyLodTrigger::GetSettings()
{
	AnomalyLodTriggerRule::FSettings S;
	S.CoverageMinPct = AnomalyDefaults::GetLodPoppingMinCoveragePct();
	S.EntryFrames = AnomalyDefaults::GetLodTriggerSetting(AnomalyDefaults::ELodTriggerSetting::EntryFrames);
	S.HoldCapFrames = AnomalyDefaults::GetLodTriggerSetting(AnomalyDefaults::ELodTriggerSetting::HoldCapFrames);
	S.bDownPop = AnomalyDefaults::GetLodTriggerSetting(AnomalyDefaults::ELodTriggerSetting::DownPop) != 0;
	return S;
}

void AnomalyLodTrigger::RecordViewSample(UWorld* World)
{
	FViewRing& R = Ring();
	if (R.World.Get() != World)
	{
		R = FViewRing();
		R.World = World;
	}
	FAnomalyViewInfo View;
	if (!World || !AnomalyViewport::GetActiveViewInfo(World, View) || !View.bValid)
	{
		return;
	}
	if (R.Count > 0 && R.Samples[(R.Head + RingSize - 1) % RingSize].Frame == GFrameCounter)
	{
		return;
	}
	FViewSample& S = R.Samples[R.Head];
	S.Location = View.Origin;
	S.Yaw = View.Rotation.Yaw;
	S.Time = World->GetTimeSeconds();
	S.Frame = GFrameCounter;
	R.Head = (R.Head + 1) % RingSize;
	R.Count = FMath::Min(R.Count + 1, RingSize);
}

bool AnomalyLodTrigger::GetMotion(UWorld* World, FMotion& Out)
{
	Out = FMotion();
	const FViewRing& R = Ring();
	FAnomalyViewInfo View;
	if (!World || R.World.Get() != World || R.Count < 2 || !AnomalyViewport::GetActiveViewInfo(World, View) || !View.bValid)
	{
		return false;
	}
	const int32 Back = FMath::Min(MotionFramesBack, R.Count - 1);
	const FViewSample& Old = R.Samples[(R.Head + RingSize - 1 - Back) % RingSize];
	const FViewSample& Last = R.Samples[(R.Head + RingSize - 1) % RingSize];
	const double Now = World->GetTimeSeconds();
	const double Seconds = Now - Old.Time;
	const int32 Frames = (int32)FMath::Max<uint64>(1, GFrameCounter - Old.Frame);
	if (Seconds <= 0.0)
	{
		return false;
	}
	Out.CameraNow = View.Origin;
	Out.CameraOld = Old.Location;
	Out.YawNow = View.Rotation.Yaw;
	Out.YawRateDegPerS = (float)(FRotator::NormalizeAxis(View.Rotation.Yaw - Old.Yaw) / Seconds);
	Out.Seconds = (float)Seconds;
	Out.FramesBack = Frames;
	Out.FrameSeconds = (float)(Seconds / (double)Frames);
	Out.bValid = Last.Frame != 0;
	return Out.bValid;
}

const TCHAR* AnomalyLodTrigger::GetStaticRefusal(const UMeshComponent* Component)
{
	if (!Component)
	{
		return TEXT("no_component");
	}
	if (Component->IsA<USkinnedMeshComponent>())
	{
		return TEXT("skeletal_not_supported");
	}
	const UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component);
	if (!Static)
	{
		return TEXT("not_static_mesh");
	}
	if (Static->IsA<UInstancedStaticMeshComponent>())
	{
		return TEXT("instanced_not_supported");
	}
	if (AnomalyLod::GetWorstLod(Static) < 2)
	{
		return TEXT("single_lod");
	}
	if (AnomalyViewport::ComponentDrawsNanite(Static))
	{
		return TEXT("nanite_no_discrete_lods");
	}
	if (Static->ForcedLodModel != 0)
	{
		return TEXT("forced_lod_present");
	}
	if (!Static->IsRegistered() || !Static->IsVisible())
	{
		return TEXT("not_rendering");
	}
	if (AnomalyLod::IsForceLodCvarActive())
	{
		return TEXT("force_lod_cvar_active");
	}
	return nullptr;
}

float AnomalyLodTrigger::GetComponentCoveragePct(UWorld* World, const UStaticMeshComponent* Component, bool* bOutOnScreen)
{
	if (bOutOnScreen)
	{
		*bOutOnScreen = false;
	}
	FAnomalyViewInfo View;
	if (!World || !Component || !AnomalyViewport::GetActiveViewInfo(World, View) || !View.bValid)
	{
		return 0.0f;
	}
	if (bOutOnScreen)
	{
		*bOutOnScreen = AnomalyViewport::IsComponentVisible(View, World, Component);
	}
	FVector2D Min(FVector2D::ZeroVector);
	FVector2D Max(FVector2D::ZeroVector);
	if (!AnomalyViewport::ProjectBoxToScreenRect(View, Component->Bounds.GetBox(), Min, Max))
	{
		return 0.0f;
	}
	return CoveragePctFromRect(Min, Max);
}

float AnomalyLodTrigger::GetDistanceToBoundsCm(const FVector& Camera, const UStaticMeshComponent* Component)
{
	if (!Component)
	{
		return -1.0f;
	}
	return FMath::Max(0.0f, (float)(FVector::Dist(Camera, Component->Bounds.Origin) - Component->Bounds.SphereRadius));
}

bool AnomalyLodTrigger::IsShadowPossiblyInView(UWorld* World, const UStaticMeshComponent* Component)
{
	if (!World || !Component || !Component->CastShadow)
	{
		return false;
	}
	const UDirectionalLightComponent* Sun = nullptr;
	for (TObjectIterator<UDirectionalLightComponent> It; It; ++It)
	{
		const UDirectionalLightComponent* L = *It;
		if (L && L->GetWorld() == World && L->IsRegistered() && L->IsVisible() && L->CastShadows && L->bAffectsWorld)
		{
			Sun = L;
			break;
		}
	}
	if (!Sun)
	{
		return false;
	}
	FAnomalyViewInfo View;
	if (!AnomalyViewport::GetActiveViewInfo(World, View) || !View.bValid)
	{
		return true;
	}
	const FVector Dir = Sun->GetDirection().GetSafeNormal();
	const float Length = FMath::Clamp(Sun->DynamicShadowDistanceMovableLight, 1000.0f, 20000.0f);
	const FBox Box = Component->Bounds.GetBox();
	const FBox Swept = Box + Box.ShiftBy(Dir * Length);
	FVector2D Min(FVector2D::ZeroVector);
	FVector2D Max(FVector2D::ZeroVector);
	return AnomalyViewport::ProjectBoxToScreenRect(View, Swept, Min, Max);
}

AnomalyLodTrigger::FArmEvaluation AnomalyLodTrigger::EvaluateArm(UWorld* World, const AnomalyLod::FLodViewScope& Scope,
	UStaticMeshComponent* Component, AnomalyLodTriggerRule::EMode Requested, const AnomalyLodTriggerRule::FSettings& Settings)
{
	using namespace AnomalyLodTriggerRule;
	FArmEvaluation E;
	if (const TCHAR* Static = GetStaticRefusal(Component))
	{
		E.Refusal = Static;
		return E;
	}
	E.Prediction = AnomalyLod::PredictEngineLod(Scope.GetView(), Component);
	if (!E.Prediction.bValid)
	{
		E.Refusal = E.Prediction.Refusal;
		return E;
	}
	float Sizes[AnomalyLod::MaxLodEntries];
	int32 Triangles[AnomalyLod::MaxLodEntries];
	const int32 NumLods = AnomalyLod::GetLodScreenSizes(Component, Sizes, Triangles);
	FAnomalyViewInfo View;
	if (!AnomalyViewport::GetActiveViewInfo(World, View) || !View.bValid)
	{
		E.Refusal = TEXT("no_view");
		return E;
	}
	FMotion Motion;
	AnomalyLodTrigger::GetMotion(World, Motion);
	bool bOnScreen = false;
	GetComponentCoveragePct(World, Component, &bOnScreen);
	const FVector Target = Component->Bounds.Origin;
	const float Radius = (float)Component->Bounds.SphereRadius;

	auto TryDistance = [&]() -> FString
	{
		if (!bOnScreen)
		{
			return TEXT("not_in_view");
		}
		FDistanceArmInput D;
		D.EngineLod = E.Prediction.EngineLod;
		D.TemporalLod0 = E.Prediction.TemporalLod0;
		D.TemporalLod1 = E.Prediction.TemporalLod1;
		D.BoundsScreenSize = E.Prediction.BoundsScreenSize;
		D.ReleaseScreenSize = ReleaseScreenSize(Sizes, AnomalyLod::MaxLodEntries, Triangles, NumLods, E.Prediction.EngineLod,
			E.Prediction.Scale, Settings);
		if (Motion.bValid)
		{
			const float DistNow = FMath::Max(1.0f, (float)FVector::Dist(Motion.CameraNow, Target));
			const float DistOld = FMath::Max(1.0f, (float)FVector::Dist(Motion.CameraOld, Target));
			D.ClosingSpeedCmPerS = (DistOld - DistNow) / FMath::Max(0.001f, Motion.Seconds);
			const float OldScreenSize = D.BoundsScreenSize * DistNow / DistOld;
			D.ScreenSizeRatePerFrame = (D.BoundsScreenSize - OldScreenSize) / (float)FMath::Max(1, Motion.FramesBack);
			const FVector Travel = Motion.CameraNow - Motion.CameraOld;
			const FVector ToTarget = Target - Motion.CameraNow;
			if (!Travel.IsNearlyZero() && !ToTarget.IsNearlyZero())
			{
				D.ApproachAngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(
					(float)FVector::DotProduct(Travel.GetSafeNormal(), ToTarget.GetSafeNormal()), -1.0f, 1.0f)));
			}
		}
		E.ClosingSpeedCmPerS = D.ClosingSpeedCmPerS;
		int32 Ahead = -1;
		const char* Why = DistanceArmRefusal(D, Settings, Ahead);
		if (Why)
		{
			return FString(UTF8_TO_TCHAR(Why));
		}
		E.HeldLod = E.Prediction.EngineLod;
		E.ReleaseScreenSize = D.ReleaseScreenSize;
		E.FramesAhead = Ahead;
		return FString();
	};

	auto TryViewEntry = [&]() -> FString
	{
		const FVector ToTarget = Target - View.Origin;
		const float Dist = FMath::Max(1.0f, (float)ToTarget.Size());
		const float Delta = FRotator::NormalizeAxis(ToTarget.Rotation().Yaw - View.Rotation.Yaw);
		const float AngularRadius = FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(Radius / Dist, 0.0f, 1.0f)));
		FViewEntryArmInput V;
		V.AngleOutsideFrustumDeg = FMath::Abs(Delta) - View.HorizontalFOVDeg * 0.5f - AngularRadius;
		if (bOnScreen && V.AngleOutsideFrustumDeg > 0.0f)
		{
			V.AngleOutsideFrustumDeg = 0.0f;
		}
		V.YawRateDegPerS = Motion.bValid ? FMath::Abs(Motion.YawRateDegPerS) : 0.0f;
		V.bYawTowardTarget = Motion.bValid && Motion.YawRateDegPerS != 0.0f && (Delta > 0.0f) == (Motion.YawRateDegPerS > 0.0f);
		V.FrameSeconds = Motion.bValid ? Motion.FrameSeconds : 0.0f;
		E.AngleOutsideFrustumDeg = V.AngleOutsideFrustumDeg;
		int32 Held = -1;
		for (int32 Lod = NumLods - 1; Lod > E.Prediction.EngineLod; --Lod)
		{
			if (ContrastPasses(Triangles, NumLods, Lod, E.Prediction.EngineLod, Settings.TriangleRatio))
			{
				Held = Lod;
				break;
			}
		}
		V.bContrastAtHeld = Held >= 0;
		FAnomalyViewInfo Facing = View;
		Facing.Rotation = ToTarget.Rotation();
		FVector2D Min(FVector2D::ZeroVector);
		FVector2D Max(FVector2D::ZeroVector);
		V.CoverageAsIfInViewPct = AnomalyViewport::ProjectBoxToScreenRect(Facing, Component->Bounds.GetBox(), Min, Max)
			? CoveragePctFromRect(Min, Max) : 0.0f;
		V.bShadowInView = V.AngleOutsideFrustumDeg > 0.0f && IsShadowPossiblyInView(World, Component);
		int32 Ahead = -1;
		const char* Why = ViewEntryArmRefusal(V, Settings, Ahead);
		if (Why)
		{
			return FString(UTF8_TO_TCHAR(Why));
		}
		E.HeldLod = Held;
		E.FramesAhead = Ahead;
		return FString();
	};

	FString Why;
	if (Requested == EMode::DistanceThreshold || Requested == EMode::Auto)
	{
		Why = TryDistance();
		if (Why.IsEmpty())
		{
			E.Mode = EMode::DistanceThreshold;
			E.bArmable = true;
			return E;
		}
		if (Requested == EMode::DistanceThreshold)
		{
			E.Refusal = Why;
			return E;
		}
	}
	const FString ViewWhy = TryViewEntry();
	if (ViewWhy.IsEmpty())
	{
		E.Mode = EMode::ViewEntry;
		E.bArmable = true;
		return E;
	}
	E.Refusal = (Requested == EMode::Auto && bOnScreen) ? Why : ViewWhy;
	return E;
}

TArray<TWeakObjectPtr<AActor>> AnomalyLodTrigger::CollectArmableActors(UWorld* World, TMap<FString, int32>& OutRefusals)
{
	TArray<TWeakObjectPtr<AActor>> Out;
	const AnomalyLod::FLodViewScope Scope(World);
	FAnomalyViewInfo View;
	if (!World || !Scope.IsValid() || !AnomalyViewport::GetActiveViewInfo(World, View) || !View.bValid)
	{
		OutRefusals.FindOrAdd(TEXT("no_view")) += 1;
		return Out;
	}
	const AnomalyLodTriggerRule::FSettings Settings = GetSettings();
	TSet<const AActor*> Seen;
	TArray<AActor*> Actors;
	for (TObjectIterator<UStaticMeshComponent> It; It; ++It)
	{
		UStaticMeshComponent* C = *It;
		if (!C || C->IsTemplate() || C->GetWorld() != World || !C->IsRegistered())
		{
			continue;
		}
		AActor* Owner = C->GetOwner();
		if (!Owner || Seen.Contains(Owner) || !AnomalyViewport::IsRenderableComponent(C) || AnomalyLod::GetWorstLod(C) < 2)
		{
			continue;
		}
		if (AnomalyLod::GetForcedLodRefusal(C) != AnomalyLodPolicy::EForcedLodRefusal::None)
		{
			continue;
		}
		if (GetDistanceToBoundsCm(View.Origin, C) > CandidateMaxDistanceCm)
		{
			continue;
		}
		const FArmEvaluation E = EvaluateArm(World, Scope, C, AnomalyLodTriggerRule::EMode::Auto, Settings);
		if (E.bArmable)
		{
			Seen.Add(Owner);
			Actors.Add(Owner);
		}
		else
		{
			OutRefusals.FindOrAdd(E.Refusal.IsEmpty() ? FString(TEXT("refused")) : E.Refusal) += 1;
		}
	}
	Actors.Sort([](const AActor& A, const AActor& B) { return A.GetName() < B.GetName(); });
	for (AActor* A : Actors)
	{
		Out.Add(A);
	}
	return Out;
}
