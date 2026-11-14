#include "AnomalyLod.h"
#include "AnomalyDefaults.h"

#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkinnedAsset.h"
#include "StaticMeshResources.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "HAL/IConsoleManager.h"

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
	return GetUsableLodCount(Component) >= 2;
}

AnomalyLod::FLodFacts AnomalyLod::GetLodFacts(const UMeshComponent* Component)
{
	FLodFacts Out;
	if (const UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component))
	{
		const UStaticMesh* Mesh = Static->GetStaticMesh();
		const FStaticMeshRenderData* Data = Mesh ? Mesh->GetRenderData() : nullptr;
		if (!Data || Data->LODResources.Num() <= 0)
		{
			return Out;
		}
		Out.NumLods = Data->LODResources.Num();
		const int32 MeshMinLod = Mesh->GetMinLODIdx();
		Out.MinLod = Static->bOverrideMinLOD ? Static->MinLOD : MeshMinLod;
		Out.MinLodSource = Static->bOverrideMinLOD ? TEXT("component-override") : TEXT("mesh");
#if WITH_EDITOR
		static const TConsoleVariableData<int32>* StripMinLodCVar =
			IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.StaticMesh.StripMinLodDataDuringCooking"));
		if (StripMinLodCVar && StripMinLodCVar->GetValueOnGameThread() != 0 && MeshMinLod > Out.MinLod)
		{
			Out.MinLod = MeshMinLod;
			Out.MinLodSource = TEXT("mesh-stripped-when-cooked");
		}
#endif
		uint32 WithVertices = 0;
		for (int32 Lod = 0; Lod < FMath::Min(Out.NumLods, 32); ++Lod)
		{
			if (Data->LODResources[Lod].GetNumVertices() > 0)
			{
				WithVertices |= (1u << Lod);
			}
		}
		Out.FirstUsable = AnomalyLodPolicy::FirstUsableLod(Out.NumLods, Out.MinLod, WithVertices);
		Out.UsableCount = AnomalyLodPolicy::UsableLodCount(Out.NumLods, Out.MinLod, WithVertices);
		const int32 Resident = Out.UsableCount > 0 ? Data->GetCurrentFirstLODIdx(Out.FirstUsable) : INDEX_NONE;
		Out.bResidentKnown = Resident != INDEX_NONE;
		Out.FirstResident = Out.bResidentKnown ? Resident : Out.FirstUsable;
		return Out;
	}
	if (const USkinnedMeshComponent* Skinned = Cast<USkinnedMeshComponent>(Component))
	{
		FSkeletalMeshRenderData* Data = Skinned->GetSkinnedAsset() ? Skinned->GetSkeletalMeshRenderData() : nullptr;
		if (!Data || Data->LODRenderData.Num() <= 0)
		{
			return Out;
		}
		Out.NumLods = Data->LODRenderData.Num();
		Out.MinLod = Skinned->ComputeMinLOD();
		Out.MinLodSource = Skinned->bOverrideMinLod ? TEXT("component-override") : TEXT("mesh");
		uint32 WithVertices = 0;
		for (int32 Lod = 0; Lod < FMath::Min(Out.NumLods, 32); ++Lod)
		{
			if (Data->LODRenderData[Lod].GetNumVertices() > 0)
			{
				WithVertices |= (1u << Lod);
			}
		}
		Out.FirstUsable = AnomalyLodPolicy::FirstUsableLod(Out.NumLods, Out.MinLod, WithVertices);
		Out.UsableCount = AnomalyLodPolicy::UsableLodCount(Out.NumLods, Out.MinLod, WithVertices);
		const bool bStreamable = Skinned->GetSkinnedAsset()->IsStreamable();
		const int32 Resident = Out.UsableCount <= 0 ? INDEX_NONE
			: bStreamable ? Data->GetPendingFirstLODIdx(Out.FirstUsable) : Out.FirstUsable;
		Out.bResidentKnown = Resident != INDEX_NONE;
		Out.FirstResident = Out.bResidentKnown ? Resident : Out.FirstUsable;
		return Out;
	}
	return Out;
}

int32 AnomalyLod::GetUsableLodCount(const UMeshComponent* Component)
{
	return GetLodFacts(Component).UsableCount;
}

FString AnomalyLod::DescribeLodFacts(const FLodFacts& Facts)
{
	return FString::Printf(TEXT("%d LOD(s), min LOD %d (%s), first usable LOD %d, %d usable"),
		Facts.NumLods, Facts.MinLod, Facts.MinLodSource, Facts.FirstUsable, Facts.UsableCount);
}

AnomalyLodPolicy::EForcedLodRefusal AnomalyLod::GetForcedLodRefusal(const UMeshComponent* Component)
{
	return AnomalyLodPolicy::DecideForcedLod(
		Component && Component->IsA<UHierarchicalInstancedStaticMeshComponent>(),
		Component && Component->IsA<USkinnedMeshComponent>(), AnomalyDefaults::GetLodAllowSkeletal(), GetUsableLodCount(Component));
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
