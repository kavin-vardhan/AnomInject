// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/GDPAnomaly_Flicker.h"

#include "GDPTargeting.h"
#include "GDPAnomalyInjectorLog.h"
#include "GameFramework/Actor.h"

bool FGDPAnomaly_Flicker::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("flicker: usage <name-substring> [hz]"));
		return false;
	}

	// Re-entrancy: revert-then-reapply.
	if (bActive)
	{
		Revert();
	}

	// Parse optional Hz (AMB-6): default 5; non-numeric or <= 0 -> warn + fall back; clamp ceiling.
	float Hz = DefaultHz;
	if (Args.Num() >= 2)
	{
		if (Args[1].IsNumeric())
		{
			Hz = FCString::Atof(*Args[1]);
		}
		else
		{
			UE_LOG(LogGDPAnomaly, Warning, TEXT("flicker: '%s' is not a number; using %.1f Hz."), *Args[1], DefaultHz);
		}
	}
	if (!FMath::IsFinite(Hz) || Hz <= 0.0f)
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("flicker: invalid rate; using %.1f Hz."), DefaultHz);
		Hz = DefaultHz;
	}
	Hz = FMath::Min(Hz, MaxHz);
	HalfPeriodSeconds = 0.5f / Hz;

	Targets = GDPTargeting::FindActorsMatching(World, Args[0]);
	Accumulator = 0.0f;
	bHiddenPhase = false;
	bActive = Targets.Num() > 0;   // zero match -> not applied / inactive (AMB-2)

	UE_LOG(LogGDPAnomaly, Log, TEXT("flicker: matched %d actor(s) for '%s' at %.2f Hz (half-period %.3fs)."),
		Targets.Num(), *Args[0], Hz, HalfPeriodSeconds);
	return bActive;
}

void FGDPAnomaly_Flicker::Tick(float DeltaSeconds)
{
	if (!bActive)
	{
		return;
	}

	Accumulator += DeltaSeconds;
	// 'while' (not 'if') so a single long frame replays all elapsed half-periods and the
	// phase never desyncs.
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
		UE_LOG(LogGDPAnomaly, Verbose, TEXT("flicker toggle -> %s (%d actors)."),
			bHiddenPhase ? TEXT("HIDDEN") : TEXT("VISIBLE"), Affected);
	}
}

void FGDPAnomaly_Flicker::Revert()
{
	// Leave actors VISIBLE regardless of the current toggle phase.
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
