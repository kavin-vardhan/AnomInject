// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class AActor;
class UWorld;

/**
 * Shared targeting helpers. Single source of truth for the label-free matching rule so
 * every actor-scoped anomaly (missing_object, flicker, ...) resolves targets identically.
 */
namespace GDPTargeting
{
	/**
	 * Find actors in World whose actor Name OR class name contains Substring
	 * (case-insensitive). Deliberately never matches the editor label
	 * (GetActorLabel is editor-only and absent in cooked builds). Returns weak-ptrs so
	 * destroyed actors never dangle. Empty/null world or empty substring -> empty array.
	 */
	GDPANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FindActorsMatching(UWorld* World, const FString& Substring);
}
