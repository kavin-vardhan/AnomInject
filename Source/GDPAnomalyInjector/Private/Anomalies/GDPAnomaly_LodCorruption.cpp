// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/GDPAnomaly_LodCorruption.h"

#include "GDPLod.h"
#include "GDPArgs.h"
#include "GDPAnomalyInjectorLog.h"
#include "Components/MeshComponent.h"   // UMeshComponent (GetName on the resolved target)

bool FGDPAnomaly_LodCorruption::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogGDPAnomaly, Warning, TEXT("lod_corruption: usage <substring> [lod-index]"));
		return false;
	}

	// Re-entrancy: revert-then-reapply so re-firing never leaks (prior forced-LODs restored).
	if (bActive)
	{
		Revert();
	}

	const FString& Substring = Args[0];

	// No explicit index -> worst/highest LOD per component (sentinel). Explicit index is 1-based and
	// clamped per component once we know that component's LOD count (via GDPLod::ResolveTargetLod).
	const bool bHasExplicitIndex = Args.IsValidIndex(1);
	const int32 RequestedIndex = bHasExplicitIndex
		? GDPArgs::GetInt(Args, 1, 1, 1, 64)   // 1-based; per-component upper clamp applied below
		: GDPLod::WorstLodSentinel;

	// Static AND skeletal: ResolveLodComponents merges both families keyed to the common base.
	const TArray<TWeakObjectPtr<UMeshComponent>> Meshes = GDPLod::ResolveLodComponents(World, Substring);
	if (Meshes.Num() == 0)
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("lod_corruption: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;   // AMB-2: zero match -> not applied / inactive
	}

	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}

		const int32 NumLODs = GDPLod::GetWorstLod(Mesh);
		const int32 Target = GDPLod::ResolveTargetLod(Mesh, RequestedIndex);

		FCapturedLod Record;
		Record.Mesh = Mesh;
		Record.PrevForcedLodModel = GDPLod::GetForcedLod(Mesh);
		Captured.Add(Record);

		GDPLod::SetForcedLod(Mesh, Target);
		UE_LOG(LogGDPAnomaly, Log, TEXT("lod_corruption: '%s' forced LOD %d of %d [was %d]."),
			*Mesh->GetName(), Target, NumLODs, Record.PrevForcedLodModel);
	}

	bActive = Captured.Num() > 0;
	UE_LOG(LogGDPAnomaly, Log, TEXT("lod_corruption: forced LOD on %d component(s) for '%s'."), Captured.Num(), *Substring);
	return bActive;
}

void FGDPAnomaly_LodCorruption::Revert()
{
	// Restore each component's captured forced-LOD (usually 0 = auto); skip stale ptrs (GC-safe).
	for (const FCapturedLod& Record : Captured)
	{
		UMeshComponent* Mesh = Record.Mesh.Get();
		if (!Mesh)
		{
			continue;
		}
		GDPLod::SetForcedLod(Mesh, Record.PrevForcedLodModel);
	}

	Captured.Reset();
	bActive = false;
}
