// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "AnomalyTargeting.h"

#include "EngineUtils.h"        // TActorIterator
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

		// Exact-match sentinel (verify-item 5 / S1): a leading '=' switches from substring to exact
		// name equality. The selector's InjectSelected() passes "=" + Actor->GetName() so an inject
		// targets ONLY the selected actor, never a same-prefixed numbered sibling (e.g. "=Cube" must
		// not also hit "Cube2"). Object names cannot contain '=', so this never collides with a real
		// console substring query, and the substring path below is byte-identical when there is no '='.
		// This is the load-bearing primitive for the future arbitrary-actor auto-injection path too.
		const bool bExact = Query.StartsWith(TEXT("="));
		const FString Needle = bExact ? Query.RightChop(1) : Query;
		if (Needle.IsEmpty())
		{
			return Result;   // "=" with nothing after it matches nothing (rather than every actor)
		}

		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor)
			{
				continue;
			}

			// Match by Name or Class name only (case-insensitive). Never the editor label.
			// Exact mode compares the actor name (and class) for full equality; substring mode
			// keeps the original Contains behavior.
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
