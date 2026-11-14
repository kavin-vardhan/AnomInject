#include "AnomalyLod.h"
#include "AnomalyDefaults.h"

#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "SceneView.h"
#include "SceneManagement.h"
#include "UnrealClient.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"

AnomalyLod::FLodViewScope::FLodViewScope(UWorld* World)
{
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	ULocalPlayer* Player = PC ? PC->GetLocalPlayer() : nullptr;
	UGameViewportClient* Client = World ? World->GetGameViewport() : nullptr;
	if (!World || !World->Scene || !Player || !Client || !Client->Viewport)
	{
		return;
	}
	Family = MakeUnique<FSceneViewFamilyContext>(FSceneViewFamily::ConstructionValues(
		Client->Viewport, World->Scene, Client->EngineShowFlags).SetRealtimeUpdate(true));
	FVector Location;
	FRotator Rotation;
	View = Player->CalcSceneView(Family.Get(), Location, Rotation, Client->Viewport);
}

AnomalyLod::FLodViewScope::~FLodViewScope()
{
}

bool AnomalyLod::IsForceLodCvarActive()
{
	static IConsoleVariable* ForceLod = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ForceLOD"));
	return ForceLod && ForceLod->GetInt() >= 0;
}

float AnomalyLod::GetStaticMeshLodDistanceScale()
{
	static IConsoleVariable* Scale = IConsoleManager::Get().FindConsoleVariable(TEXT("r.StaticMeshLODDistanceScale"));
	return Scale ? Scale->GetFloat() : 1.0f;
}

AnomalyLod::FEngineLodPrediction AnomalyLod::PredictEngineLod(const FSceneView* View, const UStaticMeshComponent* Component)
{
	FEngineLodPrediction Out;
	const UStaticMesh* Mesh = Component ? Component->GetStaticMesh() : nullptr;
	const FStaticMeshRenderData* Data = Mesh ? Mesh->GetRenderData() : nullptr;
	if (!View)
	{
		Out.Refusal = TEXT("no_view");
		return Out;
	}
	if (!Data || Data->LODResources.Num() < 1)
	{
		Out.Refusal = TEXT("no_render_data");
		return Out;
	}
	if (IsForceLodCvarActive())
	{
		Out.Refusal = TEXT("force_lod_cvar_active");
		return Out;
	}
	Out.NumLods = Data->LODResources.Num();
	const int32 RequestedMin = Component->bOverrideMinLOD ? Component->MinLOD : Mesh->GetMinLODIdx();
	Out.MinLod = Data->GetCurrentFirstLODIdx(FMath::Clamp(RequestedMin, 0, Out.NumLods - 1));
	Out.Scale = GetStaticMeshLodDistanceScale();
	const FBoxSphereBounds& Bounds = Component->Bounds;
	Out.EngineLod = FMath::Clamp((int32)ComputeStaticMeshLOD(Data, Bounds.Origin, (float)Bounds.SphereRadius, *View, Out.MinLod, Out.Scale),
		Out.MinLod, Out.NumLods - 1);
	Out.TemporalLod0 = FMath::Clamp((int32)ComputeTemporalStaticMeshLOD(Data, Bounds.Origin, (float)Bounds.SphereRadius, *View, Out.MinLod,
		Out.Scale, 0), Out.MinLod, Out.NumLods - 1);
	Out.TemporalLod1 = FMath::Clamp((int32)ComputeTemporalStaticMeshLOD(Data, Bounds.Origin, (float)Bounds.SphereRadius, *View, Out.MinLod,
		Out.Scale, 1), Out.MinLod, Out.NumLods - 1);
	Out.BoundsScreenSize = ComputeBoundsScreenSize(Bounds.Origin, (float)Bounds.SphereRadius, *View);
	Out.bValid = true;
	return Out;
}

AnomalyLod::FEngineLodPrediction AnomalyLod::PredictEngineLod(UWorld* World, const UStaticMeshComponent* Component)
{
	const FLodViewScope Scope(World);
	return PredictEngineLod(Scope.GetView(), Component);
}

int32 AnomalyLod::GetLodTriangleCount(const UStaticMeshComponent* Component, int32 LodIndex)
{
	const UStaticMesh* Mesh = Component ? Component->GetStaticMesh() : nullptr;
	const FStaticMeshRenderData* Data = Mesh ? Mesh->GetRenderData() : nullptr;
	if (!Data || !Data->LODResources.IsValidIndex(LodIndex))
	{
		return 0;
	}
	return (int32)Data->LODResources[LodIndex].GetNumTriangles();
}

int32 AnomalyLod::GetLodScreenSizes(const UStaticMeshComponent* Component, float (&OutSizes)[MaxLodEntries],
	int32 (&OutTriangles)[MaxLodEntries])
{
	for (int32 i = 0; i < MaxLodEntries; ++i)
	{
		OutSizes[i] = 0.0f;
		OutTriangles[i] = 0;
	}
	const UStaticMesh* Mesh = Component ? Component->GetStaticMesh() : nullptr;
	const FStaticMeshRenderData* Data = Mesh ? Mesh->GetRenderData() : nullptr;
	if (!Data)
	{
		return 0;
	}
	const int32 NumLods = FMath::Min(Data->LODResources.Num(), (int32)MaxLodEntries);
	for (int32 i = 0; i < MaxLodEntries && i < MAX_STATIC_MESH_LODS; ++i)
	{
		OutSizes[i] = Data->ScreenSize[i].GetValue();
	}
	for (int32 i = 0; i < NumLods; ++i)
	{
		OutTriangles[i] = (int32)Data->LODResources[i].GetNumTriangles();
	}
	return NumLods;
}

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

AnomalyLodPolicy::EForcedLodRefusal AnomalyLod::GetForcedLodRefusal(const UMeshComponent* Component)
{
	return AnomalyLodPolicy::DecideForcedLod(
		Component && Component->IsA<UHierarchicalInstancedStaticMeshComponent>(),
		Component && Component->IsA<USkinnedMeshComponent>(), AnomalyDefaults::GetLodAllowSkeletal(), GetWorstLod(Component));
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
