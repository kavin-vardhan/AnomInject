// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UWorld;
class UMeshComponent;

/**
 * Shared LOD helper (M3). Single source of truth for forced-LOD dispatch across the two
 * LOD-forceable component families:
 *   - UStaticMeshComponent  -> SetForcedLodModel(int32) / ForcedLodModel
 *   - USkinnedMeshComponent -> SetForcedLOD(int32) / GetForcedLOD()   (USkeletalMeshComponent derives)
 * Both APIs are 1-based: 0 = auto/off, N forces LOD (N-1).
 *
 * This lets lod_corruption and lod_popping treat a heterogeneous target set (a static prop AND the
 * skeletal Bot) uniformly: callers hold one per-target record keyed to the common base
 * TWeakObjectPtr<UMeshComponent>, and these free functions recover the concrete type via Cast<> to
 * pick the right getter/setter. No new module dependency — every type here lives in Engine.
 *
 * Convention-matched to GDPTargeting / GDPArgs: free functions in Public/ with the API macro; the
 * heavy concrete-component includes and the type dispatch stay in the .cpp.
 *
 * Consumers: lod_corruption (static + skeletal), lod_popping (ticking).
 */
namespace GDPLod
{
	/** Sentinel for "no explicit lod-index given" -> use the worst/highest LOD per component. */
	static constexpr int32 WorstLodSentinel = -1;

	/**
	 * Resolve every LOD-forceable mesh component (static OR skinned) whose owning actor matches
	 * Substring (GDPTargeting's label-free rule). Merges FindComponentsMatching<UStaticMeshComponent>
	 * and <USkinnedMeshComponent> into one list keyed to the common base. The two families are
	 * disjoint siblings under UMeshComponent, so the merge is duplicate-free. Weak-ptrs; GC-safe.
	 */
	GDPANOMALYINJECTOR_API TArray<TWeakObjectPtr<UMeshComponent>> ResolveLodComponents(UWorld* World, const FString& Substring);

	/**
	 * Per-component runtime LOD count (the 1-based worst index): static = GetStaticMesh()->GetNumLODs(),
	 * skinned = USkinnedMeshComponent::GetNumLODs() — the runtime render-data count, which is the true
	 * analog of the static accessor (NOT the asset's authored GetLODNum(); see gotcha G19). Returns >= 1.
	 */
	GDPANOMALYINJECTOR_API int32 GetWorstLod(const UMeshComponent* Component);

	/** Read the component's current forced-LOD (1-based; 0 = auto): static ForcedLodModel, skinned GetForcedLOD(). */
	GDPANOMALYINJECTOR_API int32 GetForcedLod(const UMeshComponent* Component);

	/** Force the component to LodIndex (1-based; 0 = auto): static SetForcedLodModel, skinned SetForcedLOD. */
	GDPANOMALYINJECTOR_API void SetForcedLod(UMeshComponent* Component, int32 LodIndex);

	/**
	 * Resolve a requested LOD against one component: WorstLodSentinel -> that component's worst LOD;
	 * an explicit (1-based) index -> clamped to [1, max(WorstLod, 1)]. Centralises the
	 * default-worst / explicit-clamp-per-component rule shared by lod_corruption and lod_popping.
	 */
	GDPANOMALYINJECTOR_API int32 ResolveTargetLod(const UMeshComponent* Component, int32 RequestedOrSentinel);
}
