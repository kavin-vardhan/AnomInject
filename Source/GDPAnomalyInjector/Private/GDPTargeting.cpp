// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "GDPTargeting.h"

#include "EngineUtils.h"        // TActorIterator
#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace GDPTargeting
{
	TArray<TWeakObjectPtr<AActor>> FindActorsMatching(UWorld* World, const FString& Substring)
	{
		TArray<TWeakObjectPtr<AActor>> Result;
		if (!World || Substring.IsEmpty())
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

			// Match by Name or Class name only (case-insensitive). Never the editor label.
			const bool bNameMatch = Actor->GetName().Contains(Substring);
			const bool bClassMatch = Actor->GetClass()->GetName().Contains(Substring);
			if (bNameMatch || bClassMatch)
			{
				Result.Add(Actor);
			}
		}
		return Result;
	}
}
