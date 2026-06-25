#include "Anomalies/Anomaly_MissingTexture.h"

#include "AnomalyLod.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h"

bool FAnomaly_MissingTexture::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("missing_texture: usage <substring>"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>();
	UMaterialInterface* Swap = Injector ? Injector->GetMissingTextureMaterial() : nullptr;
	if (!Swap)
	{
		UE_LOG(LogAnomaly, Error, TEXT("missing_texture: shipped checker material is unavailable (null) — cannot apply."));
		return false;
	}

	const FString& Substring = Args[0];

	TArray<TWeakObjectPtr<UMeshComponent>> Meshes = AnomalyLod::ResolveLodComponents(World, Substring);
	if (UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World))
	{
		FAnomalyViewInfo View;
		if (AnomalyViewport::GetActiveViewInfo(World, View))
		{
			Meshes = AnomalyViewport::FilterVisibleComponents(View, World, Meshes);
		}
	}
	if (Meshes.Num() == 0)
	{
		UE_LOG(LogAnomaly, Log, TEXT("missing_texture: matched 0 mesh component(s) for '%s'."), *Substring);
		return false;
	}

	for (const TWeakObjectPtr<UMeshComponent>& Weak : Meshes)
	{
		UMeshComponent* Mesh = Weak.Get();
		if (!Mesh)
		{
			continue;
		}
		const int32 NumSlots = Mesh->GetNumMaterials();
		if (NumSlots == 0)
		{
			continue;
		}

		for (int32 i = 0; i < NumSlots; ++i)
		{
			FCapturedSlot Slot;
			Slot.Mesh = Mesh;
			Slot.SlotIndex = i;
			Slot.OriginalMaterial = Mesh->GetMaterial(i);
			Slot.bWasExplicitOverride = Mesh->OverrideMaterials.IsValidIndex(i) && Mesh->OverrideMaterials[i] != nullptr;
			Captured.Add(Slot);

			Mesh->SetMaterial(i, Swap);
		}
		UE_LOG(LogAnomaly, Log, TEXT("missing_texture: '%s' -> checker on %d slot(s)."),
			*Mesh->GetName(), NumSlots);
	}

	bActive = Captured.Num() > 0;
	UE_LOG(LogAnomaly, Log, TEXT("missing_texture: overrode %d slot(s) across %d component(s) for '%s' (checker)."),
		Captured.Num(), Meshes.Num(), *Substring);
	return bActive;
}

void FAnomaly_MissingTexture::Revert()
{
	for (const FCapturedSlot& Slot : Captured)
	{
		UMeshComponent* Mesh = Slot.Mesh.Get();
		if (!Mesh)
		{
			continue;
		}
		if (Slot.bWasExplicitOverride)
		{
			Mesh->SetMaterial(Slot.SlotIndex, Slot.OriginalMaterial.Get());
		}
		else
		{
			Mesh->SetMaterial(Slot.SlotIndex, nullptr);
		}
	}
	Captured.Reset();
	bActive = false;
}
