#include "AnomalyLod.h"

#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"

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

bool AnomalyLod::HasMultipleLods(const UMeshComponent* Component)
{
	return GetWorstLod(Component) >= 2;
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

AnomalyLod::FCurrentLod AnomalyLod::GetCurrentLod(UWorld* World, const UMeshComponent* Component)
{
	FCurrentLod Out;
	if (!Component)
	{
		return Out;
	}

	if (const UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component))
	{
		if (Static->ForcedLodModel > 0)
		{
			Out.Level = Static->ForcedLodModel - 1;
			Out.Source = TEXT("forced-lod-model");
			Out.bKnown = true;
			return Out;
		}

		const UStaticMesh* Mesh = Static->GetStaticMesh();
		const FStaticMeshRenderData* RenderData = Mesh ? Mesh->GetRenderData() : nullptr;
		if (!RenderData)
		{
			return Out;
		}

		const FBoxSphereBounds& B = Static->Bounds;
		const float ScreenSize = AnomalyViewport::ComputeBoundsScreenSizeForActiveView(
			World, B.Origin, (float)B.SphereRadius);
		if (ScreenSize < 0.0f)
		{
			return Out;
		}

		Out.ScreenSize = ScreenSize;
		Out.Source = TEXT("predicted-from-screen-size");
		Out.bKnown = true;
		Out.Level = 0;
		const int32 NumLods = FMath::Max(Mesh->GetNumLODs(), 1);
		for (int32 LodIndex = NumLods - 1; LodIndex >= 0; --LodIndex)
		{
			if (RenderData->ScreenSize[LodIndex].GetValue() > ScreenSize)
			{
				Out.Level = LodIndex;
				break;
			}
		}
		return Out;
	}

	if (const USkinnedMeshComponent* Skinned = Cast<USkinnedMeshComponent>(Component))
	{
		if (Skinned->GetForcedLOD() > 0)
		{
			Out.Level = Skinned->GetForcedLOD() - 1;
			Out.Source = TEXT("forced-lod-model");
			Out.bKnown = true;
			return Out;
		}
		Out.Level = Skinned->GetPredictedLODLevel();
		Out.Source = TEXT("component-predicted-lod");
		Out.bKnown = true;
		return Out;
	}

	return Out;
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
