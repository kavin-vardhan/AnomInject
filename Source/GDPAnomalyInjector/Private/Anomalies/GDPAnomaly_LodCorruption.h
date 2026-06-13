// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGDPAnomaly.h"

class UWorld;
class UMeshComponent;

/**
 * lod_corruption — component-scoped, no Tick. Forces matching mesh components to a corrupted LOD.
 * Resolves BOTH static and skeletal mesh components via the shared GDPLod helper, captures each
 * component's original forced-LOD keyed to its weak ptr, then forces the target LOD. Revert
 * restores the captured value per live component and skips stale ptrs (per-target state-capture
 * convention). A single apply can match a heterogeneous set — e.g. a static prop AND the skeletal
 * Bot — because the capture record is keyed to the common base UMeshComponent and GDPLod dispatches
 * the static/skeletal setter internally.
 *
 * Scope (M3): static OR skeletal. GDPLod hides the API split — UStaticMeshComponent::SetForcedLodModel
 * vs USkinnedMeshComponent::SetForcedLOD (both 1-based; 0 = auto, N forces LOD N-1). Supersedes the
 * M2 static-only scope (gotcha G16; settled accessor in G19). Default target = worst/highest LOD
 * resolved per component; only visible if the mesh actually ships multiple LODs (gotcha G15).
 */
class FGDPAnomaly_LodCorruption final : public IGDPAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lod_corruption")); }
	virtual FString GetDescription() const override { return TEXT("Force matching static or skeletal mesh components to a corrupted LOD (default worst)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [lod-index]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	// No Tick override — inherits the no-op.

private:
	/** Per-target capture: the prior forced-LOD (1-based, 0 = auto), keyed to the common-base weak ptr. */
	struct FCapturedLod
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		int32 PrevForcedLodModel = 0;
	};

	TArray<FCapturedLod> Captured;
	bool bActive = false;
};
