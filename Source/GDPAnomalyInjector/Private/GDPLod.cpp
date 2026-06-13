// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "GDPLod.h"

#include "GDPTargeting.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"   // SetForcedLodModel / ForcedLodModel / GetStaticMesh
#include "Components/SkinnedMeshComponent.h"  // SetForcedLOD / GetForcedLOD / GetNumLODs (USkeletalMeshComponent derives)
#include "Engine/StaticMesh.h"                // UStaticMesh::GetNumLODs

TArray<TWeakObjectPtr<UMeshComponent>> GDPLod::ResolveLodComponents(UWorld* World, const FString& Substring)
{
	TArray<TWeakObjectPtr<UMeshComponent>> Result;

	// Static and skinned mesh components are disjoint siblings under UMeshComponent (no overlap),
	// so concatenating the two matches is duplicate-free. Both reuse GDPTargeting's label-free rule.
	for (const TWeakObjectPtr<UStaticMeshComponent>& Weak : GDPTargeting::FindComponentsMatching<UStaticMeshComponent>(World, Substring))
	{
		if (UStaticMeshComponent* Comp = Weak.Get())
		{
			Result.Add(Comp);
		}
	}
	for (const TWeakObjectPtr<USkinnedMeshComponent>& Weak : GDPTargeting::FindComponentsMatching<USkinnedMeshComponent>(World, Substring))
	{
		if (USkinnedMeshComponent* Comp = Weak.Get())
		{
			Result.Add(Comp);
		}
	}
	return Result;
}

int32 GDPLod::GetWorstLod(const UMeshComponent* Component)
{
	if (const UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component))
	{
		const UStaticMesh* Mesh = Static->GetStaticMesh();
		return Mesh ? FMath::Max(Mesh->GetNumLODs(), 1) : 1;
	}
	if (const USkinnedMeshComponent* Skinned = Cast<USkinnedMeshComponent>(Component))
	{
		// Runtime render-data LOD count — the analog of UStaticMesh::GetNumLODs (gotcha G19).
		return FMath::Max(Skinned->GetNumLODs(), 1);
	}
	return 1;
}

int32 GDPLod::GetForcedLod(const UMeshComponent* Component)
{
	if (const UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component))
	{
		return Static->ForcedLodModel;
	}
	if (const USkinnedMeshComponent* Skinned = Cast<USkinnedMeshComponent>(Component))
	{
		return Skinned->GetForcedLOD();
	}
	return 0;
}

void GDPLod::SetForcedLod(UMeshComponent* Component, int32 LodIndex)
{
	if (UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component))
	{
		Static->SetForcedLodModel(LodIndex);
	}
	else if (USkinnedMeshComponent* Skinned = Cast<USkinnedMeshComponent>(Component))
	{
		Skinned->SetForcedLOD(LodIndex);
	}
}

int32 GDPLod::ResolveTargetLod(const UMeshComponent* Component, int32 RequestedOrSentinel)
{
	const int32 Worst = GetWorstLod(Component);
	if (RequestedOrSentinel == WorstLodSentinel)
	{
		return Worst;
	}
	return FMath::Clamp(RequestedOrSentinel, 1, FMath::Max(Worst, 1));
}
