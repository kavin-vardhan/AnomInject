#include "AnomalyLod.h"

#include "AnomalyTargeting.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/StaticMesh.h"

TArray<TWeakObjectPtr<UMeshComponent>> AnomalyLod::ResolveLodComponents(UWorld* World, const FString& Substring)
{
	TArray<TWeakObjectPtr<UMeshComponent>> Result;

	for (const TWeakObjectPtr<UStaticMeshComponent>& Weak : AnomalyTargeting::FindComponentsMatching<UStaticMeshComponent>(World, Substring))
	{
		if (UStaticMeshComponent* Comp = Weak.Get())
		{
			Result.Add(Comp);
		}
	}
	for (const TWeakObjectPtr<USkinnedMeshComponent>& Weak : AnomalyTargeting::FindComponentsMatching<USkinnedMeshComponent>(World, Substring))
	{
		if (USkinnedMeshComponent* Comp = Weak.Get())
		{
			Result.Add(Comp);
		}
	}
	return Result;
}

int32 AnomalyLod::GetWorstLod(const UMeshComponent* Component)
{
	if (const UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component))
	{
		const UStaticMesh* Mesh = Static->GetStaticMesh();
		return Mesh ? FMath::Max(Mesh->GetNumLODs(), 1) : 1;
	}
	if (const USkinnedMeshComponent* Skinned = Cast<USkinnedMeshComponent>(Component))
	{
		return FMath::Max(Skinned->GetNumLODs(), 1);
	}
	return 1;
}

int32 AnomalyLod::GetForcedLod(const UMeshComponent* Component)
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

void AnomalyLod::SetForcedLod(UMeshComponent* Component, int32 LodIndex)
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

int32 AnomalyLod::ResolveTargetLod(const UMeshComponent* Component, int32 RequestedOrSentinel)
{
	const int32 Worst = GetWorstLod(Component);
	if (RequestedOrSentinel == WorstLodSentinel)
	{
		return Worst;
	}
	return FMath::Clamp(RequestedOrSentinel, 1, FMath::Max(Worst, 1));
}
