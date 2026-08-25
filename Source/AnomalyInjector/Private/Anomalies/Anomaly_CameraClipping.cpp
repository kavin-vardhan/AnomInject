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

	if (!bTargeted)
	{
		AnomalousNearClip = AnomalyArgs::GetFloat(Args, 0, DefaultNearClip, MinNearClip, MaxNearClip);
		ExecuteSetNearClip(World, AnomalousNearClip);
		bActive = true;
		UE_LOG(LogAnomaly, Log, TEXT("camera_clipping: near clip %.3f -> %.3f."), PreviousNearClip, GNearClippingPlane);
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
		TEXT("A frame counts positive only when the near plane is anomalous AND geometry is actually within the ")
		TEXT("near-clip radius, so if the player never approaches this event carries zero positive frames and the ")
		TEXT("m23 F-LABEL guard reports it."),
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
	return AnomalyViewport::IsGeometryWithinNearClipRadius(WorldWeak.Get());
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
}
