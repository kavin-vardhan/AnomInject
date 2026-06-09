// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/GDPAnomaly_MissingObject.h"

#include "GDPTargeting.h"
#include "GDPAnomalyInjectorLog.h"
#include "GameFramework/Actor.h"

bool FGDPAnomaly_MissingObject::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("missing_object: usage <name-substring>"));
		return false;
	}

	// Re-entrancy: revert-then-reapply so re-firing with new args never leaks state.
	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];
	const TArray<TWeakObjectPtr<AActor>> Matches = GDPTargeting::FindActorsMatching(World, Substring);
	for (const TWeakObjectPtr<AActor>& Weak : Matches)
	{
		if (AActor* Actor = Weak.Get())
		{
			Actor->SetActorHiddenInGame(true);
			HiddenActors.AddUnique(Weak);
			UE_LOG(LogGDPAnomaly, Log, TEXT("Hid actor '%s' (class '%s')."),
				*Actor->GetName(), *Actor->GetClass()->GetName());
		}
	}

	bActive = HiddenActors.Num() > 0;   // zero match -> not applied / inactive (AMB-2)
	UE_LOG(LogGDPAnomaly, Log, TEXT("missing_object: matched %d actor(s) for '%s'."), HiddenActors.Num(), *Substring);
	return bActive;
}

void FGDPAnomaly_MissingObject::Revert()
{
	for (const TWeakObjectPtr<AActor>& Weak : HiddenActors)
	{
		if (AActor* Actor = Weak.Get())
		{
			Actor->SetActorHiddenInGame(false);
		}
	}
	HiddenActors.Reset();
	bActive = false;
}
