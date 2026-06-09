// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGDPAnomaly.h"

class UWorld;
class UStaticMeshComponent;

/**
 * lod_corruption — component-scoped, no Tick. Proves FindComponentsMatching<T> (A1) generalizes
 * to a different component type (UStaticMeshComponent, vs lighting_mismatch's ULightComponent).
 * Resolves static-mesh components via A1, captures each component's original forced-LOD setting
 * keyed to its weak ptr, then forces the target LOD. Revert restores the captured value per live
 * component and skips stale ptrs (per-target state-capture convention).
 *
 * Scope (M2 / AMB-2): STATIC-MESH-ONLY v1. UStaticMeshComponent::SetForcedLodModel and
 * USkeletal/USkinnedMeshComponent::SetForcedLOD are different APIs on different base classes; the
 * deterministic MainWorld targets are static meshes (e.g. SM_Boulder). Skeletal forced-LOD is a
 * documented M3 follow-up. See gotcha G16.
 *
 * Forced-LOD is 1-based: 0 = auto, N forces LOD (N-1). Default target = worst/highest = NumLODs
 * (resolved per component). Only visible if the mesh actually ships multiple LODs (gotcha G15).
 */
class FGDPAnomaly_LodCorruption final : public IGDPAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lod_corruption")); }
	virtual FString GetDescription() const override { return TEXT("Force matching static-mesh components to a corrupted LOD (default worst)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [lod-index]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	// No Tick override — inherits the no-op.

private:
	/** Per-target capture: the prior ForcedLodModel (1-based, 0 = auto), keyed to the weak ptr. */
	struct FCapturedLod
	{
		TWeakObjectPtr<UStaticMeshComponent> Mesh;
		int32 PrevForcedLodModel = 0;
	};

	TArray<FCapturedLod> Captured;
	bool bActive = false;

	/** Sentinel for "no explicit lod-index given" -> use the worst/highest LOD per component. */
	static constexpr int32 WorstLodSentinel = -1;
};
