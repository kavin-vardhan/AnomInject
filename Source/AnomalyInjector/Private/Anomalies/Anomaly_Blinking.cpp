#include "Anomalies/Anomaly_Blinking.h"

#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "GameFramework/Actor.h"

bool FAnomaly_Blinking::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("blinking: usage <name-substring> [hz]"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	float Hz = DefaultHz;
	if (Args.Num() >= 2)
	{
		if (Args[1].IsNumeric())
		{
			Hz = FCString::Atof(*Args[1]);
		}
		else
		{
			UE_LOG(LogAnomaly, Warning, TEXT("blinking: '%s' is not a number; using %.1f Hz."), *Args[1], DefaultHz);
		}
	}
	if (!FMath::IsFinite(Hz) || Hz <= 0.0f)
	{
		UE_LOG(LogAnomaly, Warning, TEXT("blinking: invalid rate; using %.1f Hz."), DefaultHz);
		Hz = DefaultHz;
	}
	Hz = FMath::Min(Hz, MaxHz);
	HalfPeriodSeconds = 0.5f / Hz;

	Targets = UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World)
		? AnomalyViewport::FindVisibleActorsMatching(World, Args[0])
		: AnomalyTargeting::FindActorsMatching(World, Args[0]);
	Accumulator = 0.0f;
	bHiddenPhase = false;
	bActive = Targets.Num() > 0;

	UE_LOG(LogAnomaly, Log, TEXT("blinking: matched %d actor(s) for '%s' at %.2f Hz (half-period %.3fs)."),
		Targets.Num(), *Args[0], Hz, HalfPeriodSeconds);
	return bActive;
}

void FAnomaly_Blinking::Tick(float DeltaSeconds)
{
	if (!bActive)
	{
		return;
	}

	Accumulator += DeltaSeconds;
	while (Accumulator >= HalfPeriodSeconds)
	{
		Accumulator -= HalfPeriodSeconds;
		bHiddenPhase = !bHiddenPhase;

		int32 Affected = 0;
		for (const TWeakObjectPtr<AActor>& Weak : Targets)
		{
			if (AActor* Actor = Weak.Get())
			{
				Actor->SetActorHiddenInGame(bHiddenPhase);
				++Affected;
			}
		}
		UE_LOG(LogAnomaly, Verbose, TEXT("blinking toggle -> %s (%d actors)."),
			bHiddenPhase ? TEXT("HIDDEN") : TEXT("VISIBLE"), Affected);
	}
}

void FAnomaly_Blinking::Revert()
{
	for (const TWeakObjectPtr<AActor>& Weak : Targets)
	{
		if (AActor* Actor = Weak.Get())
		{
			Actor->SetActorHiddenInGame(false);
		}
	}
	Targets.Reset();
	Accumulator = 0.0f;
	bHiddenPhase = false;
	bActive = false;
}
