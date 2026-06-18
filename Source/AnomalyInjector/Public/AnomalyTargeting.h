// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"   // AActor::GetComponents<T> for the header-only FindComponentsMatching

class UWorld;

/**
 * Shared targeting helpers. Single source of truth for the label-free matching rule so
 * every actor- and component-scoped anomaly resolves targets identically.
 */
namespace AnomalyTargeting
{
	/**
	 * Find actors in World whose actor Name OR class name contains Substring
	 * (case-insensitive). Deliberately never matches the editor label
	 * (GetActorLabel is editor-only and absent in cooked builds). Returns weak-ptrs so
	 * destroyed actors never dangle. Empty/null world or empty substring -> empty array.
	 */
	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FindActorsMatching(UWorld* World, const FString& Substring);

	/**
	 * Component-level targeting (M2 / A1). Same match rule as FindActorsMatching — resolve
	 * matching actors by Name/Class substring, then gather every component of type T on each
	 * (AActor::GetComponents<T>). Returns weak-ptrs. Handles both standalone light/mesh actors
	 * (whose class name carries the substring, e.g. APointLight) and lights/meshes-as-components
	 * on other actors uniformly. Header-only template so it instantiates per consumer type;
	 * reuses the single FindActorsMatching implementation above for the match rule.
	 *
	 * Consumers: lighting_mismatch (ULightComponent), lod_corruption (UStaticMeshComponent).
	 */
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
