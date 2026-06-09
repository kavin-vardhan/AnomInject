// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/GDPAnomaly_CameraClipping.h"

#include "GDPArgs.h"
#include "GDPAnomalyInjectorLog.h"
#include "CoreGlobals.h"        // GNearClippingPlane (Core) — captured baseline
#include "Engine/Engine.h"      // GEngine->Exec
#include "Engine/World.h"

void FGDPAnomaly_CameraClipping::ExecuteSetNearClip(UWorld* World, float Value) const
{
	if (!GEngine)
	{
		return;
	}
	// r.SetNearClipPlane is a console COMMAND (not a cvar); its handler calls SetNearClipPlaneGlobals,
	// which sets GNearClippingPlane and enqueues the render-thread sync. World is just the Exec context.
	GEngine->Exec(World, *FString::Printf(TEXT("r.SetNearClipPlane %f"), Value));
}

bool FGDPAnomaly_CameraClipping::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}

	// Re-entrancy: revert-then-reapply so re-firing never stacks (single capture set).
	if (bActive)
	{
		Revert();
	}

	const float NewNearClip = GDPArgs::GetFloat(Args, 0, DefaultNearClip, MinNearClip, MaxNearClip);

	WorldWeak = World;
	// Capture the baseline BEFORE changing it (AMB-3 convention); Revert restores exactly this.
	PreviousNearClip = GNearClippingPlane;
	ExecuteSetNearClip(World, NewNearClip);

	bActive = true;
	UE_LOG(LogGDPAnomaly, Log, TEXT("camera_clipping: near clip %.3f -> %.3f."), PreviousNearClip, GNearClippingPlane);
	return true;
}

void FGDPAnomaly_CameraClipping::Revert()
{
	// The global is restored regardless of whether the world survives; pass it as Exec context if alive.
	ExecuteSetNearClip(WorldWeak.Get(), PreviousNearClip);
	UE_LOG(LogGDPAnomaly, Log, TEXT("camera_clipping: restored near clip %.3f."), PreviousNearClip);

	WorldWeak.Reset();
	PreviousNearClip = 10.0f;   // fallback for any future Revert before the next Apply
	bActive = false;
}
