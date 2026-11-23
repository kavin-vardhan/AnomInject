#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class UWorld;

namespace AnomalyTargeting
{
	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FindActorsMatching(UWorld* World, const FString& Query);

	ANOMALYINJECTOR_API int32 CountActorsNamed(UWorld* World, const FString& Name, TArray<FString>* OutPaths = nullptr,
		int32 MaxPaths = 3);

	ANOMALYINJECTOR_API FString MakePathToken(const AActor* Actor);

	ANOMALYINJECTOR_API FString DescribeFirstPaths(const TArray<TWeakObjectPtr<AActor>>& Actors, int32 MaxPaths = 3);

	template <typename T>
	TArray<TWeakObjectPtr<T>> FindComponentsMatching(UWorld* World, const FString& Substring)
	{
		TArray<TWeakObjectPtr<T>> Result;

		const TArray<TWeakObjectPtr<AActor>> Actors = FindActorsMatching(World, Substring);
		for (const TWeakObjectPtr<AActor>& Weak : Actors)
		{
			AActor* Actor = Weak.Get();
			if (!Actor)
			{
				continue;
			}

			TArray<T*> Components;
			Actor->GetComponents<T>(Components);
			for (T* Component : Components)
			{
				if (Component)
				{
					Result.Add(Component);
				}
			}
		}
		return Result;
	}
}
