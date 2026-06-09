// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/GDPAnomaly_TimeDilation.h"

#include "GDPAnomalyInjectorLog.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"   // Engine module — no new Build.cs dependency

bool FGDPAnomaly_TimeDilation::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("time_dilation: usage <scale>"));
		return false;
	}
	if (!Args[0].IsNumeric())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("time_dilation: '%s' is not a number."), *Args[0]);
		return false;
	}
	const float Scale = FCString::Atof(*Args[0]);

	// Re-entrancy: revert-then-reapply.
	if (bActive)
	{
		Revert();
	}

	WorldWeak = World;
	// AMB-3: capture the baseline BEFORE changing it; Revert restores exactly this.
	PreviousDilation = UGameplayStatics::GetGlobalTimeDilation(World);
	UGameplayStatics::SetGlobalTimeDilation(World, Scale);

	// WorldSettings clamps Min/MaxGlobalTimeDilation; report if the request was clamped (G11).
	const float Applied = UGameplayStatics::GetGlobalTimeDilation(World);
	if (!FMath::IsNearlyEqual(Applied, Scale))
	{
		UE_LOG(LogGDPAnomaly, Warning,
			TEXT("time_dilation: requested %.4f but clamped to %.4f (WorldSettings Min/MaxGlobalTimeDilation)."),
			Scale, Applied);
	}

	bActive = true;
	UE_LOG(LogGDPAnomaly, Log, TEXT("time_dilation: baseline %.4f -> %.4f."), PreviousDilation, Applied);
	return true;
}

void FGDPAnomaly_TimeDilation::Revert()
{
	if (UWorld* World = WorldWeak.Get())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, PreviousDilation);
		UE_LOG(LogGDPAnomaly, Log, TEXT("time_dilation: restored baseline %.4f."), PreviousDilation);
	}
	WorldWeak.Reset();
	PreviousDilation = 1.0f;   // fallback for any future Revert called before the next Apply
	bActive = false;
}
