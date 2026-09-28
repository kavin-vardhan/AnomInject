#include "Anomalies/Anomaly_CameraClipping.h"

#include "AnomalyArgs.h"
#include "AnomalyDefaults.h"
#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "CoreGlobals.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"

void FAnomaly_CameraClipping::ExecuteSetNearClip(UWorld* World, float Value) const
{
	if (!GEngine)
	{
		return;
	}
	GEngine->Exec(World, *FString::Printf(TEXT("r.SetNearClipPlane %f"), Value));
}

bool FAnomaly_CameraClipping::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	const bool bTargeted = Args.Num() > 0 && Args[0].StartsWith(TEXT("="));

	WorldWeak = World;
	PreviousNearClip = GNearClippingPlane;
	Targets.Reset();
	TargetToken.Reset();
	TriggerTransitions = 0;
	bTargetedMode = bTargeted;
	bPushed = false;
	bCacheValid = false;

	if (!bTargeted)
	{
		AnomalousNearClip = AnomalyArgs::GetFloat(Args, 0, DefaultNearClip, MinNearClip, MaxNearClip);
		ExecuteSetNearClip(World, AnomalousNearClip);
		bActive = true;
		UE_LOG(LogAnomaly, Log,
			TEXT("camera_clipping: near clip %.3f -> %.3f. A frame is labelled positive only when rendered geometry lies in ")
			TEXT("the view slab between the two planes (depth %.3f..%.3f inside the frustum); the pre-086-02 sphere proxy ")
			TEXT("is kept as the diagnostic camera_clipping.sphere_proxy."),
			PreviousNearClip, GNearClippingPlane, PreviousNearClip, AnomalousNearClip);
		return true;
	}

	AnomalousNearClip = AnomalyArgs::GetFloat(Args, 1, DefaultNearClip, MinNearClip, MaxNearClip);
	TriggerRadiusCm = AnomalyDefaults::GetCameraClippingTriggerRadiusCm();
	TargetToken = Args[0];

	Targets = UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World)
		? AnomalyViewport::FindVisibleActorsMatching(World, TargetToken)
		: AnomalyTargeting::FindActorsMatching(World, TargetToken);

	if (Targets.Num() == 0)
	{
		UE_LOG(LogAnomaly, Warning,
			TEXT("camera_clipping: TARGETED mode matched 0 actor(s) for '%s'; applying nothing, so no fire is recorded ")
			TEXT("and no label is written. The near clip is untouched."),
			*TargetToken);
		WorldWeak.Reset();
		bTargetedMode = false;
		return false;
	}

	bActive = true;
	UE_LOG(LogAnomaly, Log,
		TEXT("camera_clipping: TARGETED on %d actor(s) for '%s' — near clip %.3f -> %.3f WHILE the player is within ")
		TEXT("%.2f cm of the target, restored to %.3f while outside. The effect follows proximity, so the rest of the ")
		TEXT("scene is not spuriously clipped. Nothing is pushed yet; the first evaluation happens on the next tick. ")
		TEXT("A frame counts positive only when the near plane is anomalous AND rendered geometry lies in the view ")
		TEXT("slab between the two planes, so if the player never approaches this event carries zero positive frames ")
		TEXT("and the m23 F-LABEL guard reports it."),
		Targets.Num(), *TargetToken, PreviousNearClip, AnomalousNearClip, TriggerRadiusCm, PreviousNearClip);
	return true;
}

bool FAnomaly_CameraClipping::IsPlayerWithinTriggerRadius() const
{
	UWorld* World = WorldWeak.Get();
	if (!World || TriggerRadiusCm <= 0.0f)
	{
		return false;
	}

	for (const TWeakObjectPtr<AActor>& Weak : Targets)
	{
		const AActor* Actor = Weak.Get();
		if (!Actor)
		{
			continue;
		}
		if (AnomalyViewport::GetActorPollDistanceCm(World, Actor) <= TriggerRadiusCm)
		{
			return true;
		}
	}
	return false;
}

void FAnomaly_CameraClipping::Tick(float DeltaSeconds)
{
	if (!bActive || !bTargetedMode)
	{
		return;
	}

	UWorld* World = WorldWeak.Get();
	if (!World)
	{
		return;
	}

	const bool bWithin = IsPlayerWithinTriggerRadius();
	if (bWithin == bPushed)
	{
		return;
	}

	bPushed = bWithin;
	++TriggerTransitions;
	ExecuteSetNearClip(World, bPushed ? AnomalousNearClip : PreviousNearClip);
	UE_LOG(LogAnomaly, Verbose,
		TEXT("camera_clipping: TRIGGER %s for '%s' — near clip now %.3f (transition %d)."),
		bPushed ? TEXT("ENTER") : TEXT("LEAVE"), *TargetToken, GNearClippingPlane, TriggerTransitions);
}

const FAnomalyNearClipSlabResult& FAnomaly_CameraClipping::EvaluateNow() const
{
	UWorld* World = WorldWeak.Get();
	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	float Fov = 0.0f;
	if (World)
	{
		if (const APlayerController* PC = World->GetFirstPlayerController())
		{
			if (PC->PlayerCameraManager)
			{
				const FMinimalViewInfo& Pov = PC->PlayerCameraManager->GetCameraCacheView();
				Location = Pov.Location;
				Rotation = Pov.Rotation;
				Fov = Pov.FOV;
			}
		}
	}

	if (bCacheValid && CacheFrame == GFrameCounter && CacheLocation.Equals(Location, 0.0)
		&& CacheRotation.Equals(Rotation, 0.0) && CacheFov == Fov && CacheBaseline == PreviousNearClip
		&& CacheAnomalous == AnomalousNearClip)
	{
		return CacheSlab;
	}

	AnomalyViewport::EvaluateNearClipSlab(World, PreviousNearClip, AnomalousNearClip, CacheSlab);
	bCacheSphereProxy = AnomalyViewport::IsGeometryWithinNearClipRadius(World);
	bCacheValid = true;
	CacheFrame = GFrameCounter;
	CacheLocation = Location;
	CacheRotation = Rotation;
	CacheFov = Fov;
	CacheBaseline = PreviousNearClip;
	CacheAnomalous = AnomalousNearClip;
	return CacheSlab;
}

bool FAnomaly_CameraClipping::GetFrameEvaluation(FAnomalyNearClipSlabResult& OutSlab, bool& bOutSphereProxy) const
{
	if (!bActive)
	{
		return false;
	}
	if (bTargetedMode && !bPushed)
	{
		OutSlab = FAnomalyNearClipSlabResult();
		OutSlab.BaselineNear = PreviousNearClip;
		OutSlab.AnomalousNear = PreviousNearClip;
		bOutSphereProxy = false;
		return true;
	}
	OutSlab = EvaluateNow();
	bOutSphereProxy = bCacheSphereProxy;
	return true;
}

bool FAnomaly_CameraClipping::IsCurrentlyAnomalous() const
{
	if (!bActive)
	{
		return false;
	}
	if (bTargetedMode && !bPushed)
	{
		return false;
	}
	return EvaluateNow().bSlab;
}

void FAnomaly_CameraClipping::Revert()
{
	ExecuteSetNearClip(WorldWeak.Get(), PreviousNearClip);
	if (bTargetedMode)
	{
		UE_LOG(LogAnomaly, Log,
			TEXT("camera_clipping: TARGETED fire on '%s' ended — restored near clip %.3f, %d proximity transition(s), ")
			TEXT("last state %s."),
			*TargetToken, PreviousNearClip, TriggerTransitions, bPushed ? TEXT("INSIDE") : TEXT("outside"));
	}
	else
	{
		UE_LOG(LogAnomaly, Log, TEXT("camera_clipping: restored near clip %.3f."), PreviousNearClip);
	}

	WorldWeak.Reset();
	Targets.Reset();
	TargetToken.Reset();
	PreviousNearClip = 10.0f;
	AnomalousNearClip = DefaultNearClip;
	TriggerRadiusCm = 0.0f;
	TriggerTransitions = 0;
	bTargetedMode = false;
	bPushed = false;
	bActive = false;
	bCacheValid = false;
}
