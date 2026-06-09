// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/GDPAnomaly_LodCorruption.h"

#include "GDPTargeting.h"
#include "GDPArgs.h"
#include "GDPAnomalyInjectorLog.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"   // UStaticMesh::GetNumLODs

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

	// No explicit index -> worst/highest LOD per component (sentinel). Explicit index is clamped
	// per component once we know that component's NumLODs (1-based; upper bound applied below).
	const bool bHasExplicitIndex = Args.IsValidIndex(1);
	const int32 RequestedIndex = bHasExplicitIndex
		? GDPArgs::GetInt(Args, 1, 1, 1, 64)   // 1-based; per-component upper clamp applied below
		: WorstLodSentinel;

	const TArray<TWeakObjectPtr<UStaticMeshComponent>> Meshes = GDPTargeting::FindComponentsMatching<UStaticMeshComponent>(World, Substring);
	if (Meshes.Num() == 0)
	{
		UE_LOG(LogGDPAnomaly, Log, TEXT("lod_corruption: matched 0 static-mesh component(s) for '%s'."), *Substring);
		return false;   // AMB-2: zero match -> not applied / inactive
	}

	for (const TWeakObjectPtr<UStaticMeshComponent>& Weak : Meshes)
	{
		UStaticMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}

		const UStaticMesh* StaticMesh = Mesh->GetStaticMesh();
		const int32 NumLODs = StaticMesh ? StaticMesh->GetNumLODs() : 1;

		// ForcedLodModel is 1-based (force to ForcedLodModel-1); worst/highest LOD => NumLODs.
		const int32 Target = (RequestedIndex == WorstLodSentinel)
			? NumLODs
			: FMath::Clamp(RequestedIndex, 1, FMath::Max(NumLODs, 1));

		FCapturedLod Record;
		Record.Mesh = Mesh;
		Record.PrevForcedLodModel = Mesh->ForcedLodModel;
		Captured.Add(Record);

		Mesh->SetForcedLodModel(Target);
		UE_LOG(LogGDPAnomaly, Log, TEXT("lod_corruption: '%s' forced LOD %d of %d [was %d]."),
			*Mesh->GetName(), Target, NumLODs, Record.PrevForcedLodModel);
	}

	bActive = Captured.Num() > 0;
	UE_LOG(LogGDPAnomaly, Log, TEXT("lod_corruption: forced LOD on %d component(s) for '%s'."), Captured.Num(), *Substring);
	return bActive;
}

void FGDPAnomaly_LodCorruption::Revert()
{
	// Restore each component's captured ForcedLodModel (usually 0 = auto); skip stale ptrs (GC-safe).
	for (const FCapturedLod& Record : Captured)
	{
		UStaticMeshComponent* Mesh = Record.Mesh.Get();
		if (!Mesh)
		{
			continue;
		}
		Mesh->SetForcedLodModel(Record.PrevForcedLodModel);
	}

	Captured.Reset();
	bActive = false;
}
