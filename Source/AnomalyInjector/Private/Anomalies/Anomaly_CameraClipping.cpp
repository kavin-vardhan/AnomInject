#include "Anomalies/Anomaly_CameraClipping.h"

#include "AnomalyArgs.h"
#include "AnomalyInjectorLog.h"
#include "CoreGlobals.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

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

	const float NewNearClip = AnomalyArgs::GetFloat(Args, 0, DefaultNearClip, MinNearClip, MaxNearClip);

	WorldWeak = World;
	PreviousNearClip = GNearClippingPlane;
	ExecuteSetNearClip(World, NewNearClip);

	bActive = true;
	UE_LOG(LogAnomaly, Log, TEXT("camera_clipping: near clip %.3f -> %.3f."), PreviousNearClip, GNearClippingPlane);
	return true;
}

void FAnomaly_CameraClipping::Revert()
{
	ExecuteSetNearClip(WorldWeak.Get(), PreviousNearClip);
	UE_LOG(LogAnomaly, Log, TEXT("camera_clipping: restored near clip %.3f."), PreviousNearClip);

	WorldWeak.Reset();
	PreviousNearClip = 10.0f;
	bActive = false;
}
