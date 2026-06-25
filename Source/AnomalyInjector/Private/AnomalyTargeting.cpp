#include "AnomalyTargeting.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace AnomalyTargeting
{
	TArray<TWeakObjectPtr<AActor>> FindActorsMatching(UWorld* World, const FString& Query)
	{
		TArray<TWeakObjectPtr<AActor>> Result;
		if (!World || Query.IsEmpty())
		{
			return Result;
		}

		const bool bExact = Query.StartsWith(TEXT("="));
		const FString Needle = bExact ? Query.RightChop(1) : Query;
		if (Needle.IsEmpty())
		{
			return Result;
		}

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}

			bool bMatch;
			if (bExact)
			{
				bMatch = Actor->GetName().Equals(Needle, ESearchCase::IgnoreCase)
					|| Actor->GetClass()->GetName().Equals(Needle, ESearchCase::IgnoreCase);
			}
			else
			{
				bMatch = Actor->GetName().Contains(Needle)
					|| Actor->GetClass()->GetName().Contains(Needle);
			}

			if (bMatch)
			{
				Result.Add(Actor);
			}
		}
		return Result;
	}
}
