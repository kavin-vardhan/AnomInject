#include "Anomalies/Anomaly_Blinking.h"

#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "AnomalyDefaults.h"
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
		UE_LOG(LogAnomaly, Warning, TEXT("blinking: usage <name-substring> [half_period_frames]"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	static_assert(DefaultHalfPeriodFrames == AnomalyDefaults::BlinkingHalfPeriodCompiled,
		"blinking's compiled half-period and AnomalyDefaults::BlinkingHalfPeriodCompiled have drifted apart - "
		"the capture run-config echo reads the AnomalyDefaults copy, so a silent disagreement would print one "
		"number while the anomaly used another");
	static_assert(MinHalfPeriodFrames == AnomalyDefaults::HalfPeriodMin
		&& MaxHalfPeriodFrames == AnomalyDefaults::HalfPeriodMax,
		"blinking's half-period clamp and AnomalyDefaults' clamp have drifted apart");

	const int32 EffectiveDefault = AnomalyDefaults::GetHalfPeriodFrames(
		AnomalyDefaults::BlinkingHalfPeriodKey(), DefaultHalfPeriodFrames,
		MinHalfPeriodFrames, MaxHalfPeriodFrames, TEXT("blinking"));

	int32 Frames = EffectiveDefault;
	if (Args.Num() >= 2)
	{
		if (Args[1].IsNumeric())
		{
			Frames = FCString::Atoi(*Args[1]);
		}
		else
		{
			UE_LOG(LogAnomaly, Warning, TEXT("blinking: '%s' is not a whole number of frames; using %d."),
				*Args[1], EffectiveDefault);
			Frames = EffectiveDefault;
		}
	}
	if (Frames < MinHalfPeriodFrames || Frames > MaxHalfPeriodFrames)
	{
		UE_LOG(LogAnomaly, Warning, TEXT("blinking: half-period %d out of range [%d..%d]; using %d."),
			Frames, MinHalfPeriodFrames, MaxHalfPeriodFrames, EffectiveDefault);
		Frames = EffectiveDefault;
	}
	HalfPeriodFrames = Frames;

	Targets = UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World)
		? AnomalyViewport::FindVisibleActorsMatching(World, Args[0])
		: AnomalyTargeting::FindActorsMatching(World, Args[0]);
	FramesSinceToggle = 0;
	bHiddenPhase = false;
	bActive = Targets.Num() > 0;

	UE_LOG(LogAnomaly, Log, TEXT("blinking: matched %d actor(s) for '%s' at half-period %d frame(s)."),
		Targets.Num(), *Args[0], HalfPeriodFrames);
	return bActive;
}

void FAnomaly_Blinking::Tick(float DeltaSeconds)
{
	if (!bActive)
	{
		return;
	}

	++FramesSinceToggle;
	while (FramesSinceToggle >= HalfPeriodFrames)
	{
		FramesSinceToggle -= HalfPeriodFrames;
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
	FramesSinceToggle = 0;
	bHiddenPhase = false;
	bActive = false;
}
