#include "AnomalyLod.h"
#include "Compat/AnomalyEngineCompat.h"

#include "AnomalyTargeting.h"
#include "AnomalyViewport.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#if ANOMALY_ENGINE_425
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "SceneManagement.h"
#include "SceneView.h"
#include "UnrealClient.h"
#endif

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
		const FStaticMeshRenderData* RenderData = AnomalyCompat::StaticRenderData(Mesh);
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
			if (AnomalyCompat::StaticLodScreenSize(RenderData, LodIndex) > ScreenSize)
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
		Out.Level = AnomalyCompat::PredictedLod(Skinned);
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

#if ANOMALY_ENGINE_425
AnomalyLod::FCurrentLod AnomalyLod::PredictDrawnLod425(UWorld* World, const UMeshComponent* Component)
{
    const UStaticMeshComponent* Static = Cast<UStaticMeshComponent>(Component);
    if (!Static) { return GetCurrentLod(World, Component); }
    FCurrentLod Out;
    const UStaticMesh* Mesh = Static->GetStaticMesh();
    const FStaticMeshRenderData* Data = AnomalyCompat::StaticRenderData(Mesh);
    APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
    ULocalPlayer* Player = PC ? PC->GetLocalPlayer() : nullptr;
    UGameViewportClient* Client = World ? World->GetGameViewport() : nullptr;
    if (!Data || Data->LODResources.Num() < 2 || !Player || !Client || !Client->Viewport) { return Out; }
    FSceneViewFamilyContext Family(FSceneViewFamily::ConstructionValues(Client->Viewport, World->Scene, Client->EngineShowFlags).SetRealtimeUpdate(true));
    FVector Location;
    FRotator Rotation;
    FSceneView* View = Player->CalcSceneView(&Family, Location, Rotation, Client->Viewport);
    if (!View) { return Out; }
    const int32 MeshMin = Mesh->MinLOD.GetValueForFeatureLevel(View->GetFeatureLevel());
    int32 Min = Static->bOverrideMinLOD ? Static->MinLOD : MeshMin;
#if WITH_EDITOR
    const IConsoleVariable* Strip = IConsoleManager::Get().FindConsoleVariable(TEXT("r.StaticMesh.StripMinLodDataDuringCooking"));
    if (Strip && Strip->GetInt()) { Min = FMath::Max(Min, MeshMin); }
#endif
    Min = Data->GetCurrentFirstLODIdx(FMath::Clamp(Min, 0, Data->LODResources.Num()-1));
    const IConsoleVariable* Force = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ForceLOD"));
    const IConsoleVariable* Scale = IConsoleManager::Get().FindConsoleVariable(TEXT("r.StaticMeshLODDistanceScale"));
    const float DistanceScale = Scale ? Scale->GetFloat() : 1.0f;
    const FBoxSphereBounds& Bounds = Static->Bounds;
    Out.Level = ComputeStaticMeshLOD(Data, Bounds.Origin, Bounds.SphereRadius, *View, Min, DistanceScale != 0 ? 1.0f/DistanceScale : 1.0f);
    if (Static->ForcedLodModel > 0) { Out.Level = FMath::Clamp(Static->ForcedLodModel, Min+1, Data->LODResources.Num())-1; }
    if (Force && Force->GetInt() >= 0) { Out.Level = FMath::Clamp(Force->GetInt(), Min, Data->LODResources.Num()-1); }
#if WITH_EDITOR
    if (!Client->EngineShowFlags.LOD && !(Force && Force->GetInt() >= 0) && Static->ForcedLodModel <= 0) { Out.Level = 0; }
#endif
    Out.Level = FMath::Clamp(Out.Level, Min, Data->LODResources.Num()-1);
    Out.ScreenSize = ComputeBoundsScreenSize(Bounds.Origin, Bounds.SphereRadius, *View);
    Out.Source = TEXT("engine-screen-size-game-thread");
    Out.bKnown = true;
    return Out;
}
#endif
