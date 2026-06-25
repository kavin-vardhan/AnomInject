#include "Anomalies/Anomaly_MissingObject.h"

#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "GameFramework/Actor.h"

bool FAnomaly_MissingObject::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("missing_object: usage <name-substring>"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];
	const TArray<TWeakObjectPtr<AActor>> Matches =
		UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World)
			? AnomalyViewport::FindVisibleActorsMatching(World, Substring)
			: AnomalyTargeting::FindActorsMatching(World, Substring);
	for (const TWeakObjectPtr<AActor>& Weak : Matches)
	{
		if (AActor* Actor = Weak.Get())
		{
			Actor->SetActorHiddenInGame(true);
			HiddenActors.AddUnique(Weak);
			UE_LOG(LogAnomaly, Log, TEXT("Hid actor '%s' (class '%s')."),
				*Actor->GetName(), *Actor->GetClass()->GetName());
		}
	}

	bActive = HiddenActors.Num() > 0;
	UE_LOG(LogAnomaly, Log, TEXT("missing_object: matched %d actor(s) for '%s'."), HiddenActors.Num(), *Substring);
	return bActive;
}

void FAnomaly_MissingObject::Revert()
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
