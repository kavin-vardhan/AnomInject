// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#include "Anomalies/Anomaly_MissingTexture.h"

#include "AnomalyLod.h"        // ResolveLodComponents: the generic SM/SK -> UMeshComponent resolver (LOD-flavoured
                              // name only; future chore: extract/rename to a neutral ResolveRenderableMeshComponents).
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

	// Re-entrancy: revert-then-reapply so re-firing never leaks (prior slot overrides restored).
	if (bActive)
	{
		Revert();
	}

	// Resolve the shipped checker material from the injector subsystem's CDO-held ref (Stage-1 cook guarantee).
	UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>();
	UMaterialInterface* Swap = Injector ? Injector->GetMissingTextureMaterial() : nullptr;
	if (!Swap)
	{
		UE_LOG(LogAnomaly, Error, TEXT("missing_texture: shipped checker material is unavailable (null) — cannot apply."));
		return false;
	}

	const FString& Substring = Args[0];

	// Static AND skeletal mesh components on matching actors, merged to the common UMeshComponent base.
	TArray<TWeakObjectPtr<UMeshComponent>> Meshes = AnomalyLod::ResolveLodComponents(World, Substring);
	// Opt-in viewport scoping (component granularity), mirroring lod_corruption: keep only visible meshes;
	// no live view -> treat-as-unscoped (AMB-V3); OFF -> identical to before (regression gate).
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
		return false;   // AMB-2: zero match -> not applied / inactive
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
			continue;   // empty component (no mesh set) -> nothing to recolor
		}

		// Override every material slot (the whole object loses its textures — the canonical look).
		for (int32 i = 0; i < NumSlots; ++i)
		{
			FCapturedSlot Slot;
			Slot.Mesh = Mesh;
			Slot.SlotIndex = i;
			Slot.OriginalMaterial = Mesh->GetMaterial(i);   // effective material (override if any, else asset slot)
			Slot.bWasExplicitOverride = Mesh->OverrideMaterials.IsValidIndex(i) && Mesh->OverrideMaterials[i] != nullptr;
			Captured.Add(Slot);

			Mesh->SetMaterial(i, Swap);
		}
		UE_LOG(LogAnomaly, Log, TEXT("missing_texture: '%s' -> checker on %d slot(s)."),
			*Mesh->GetName(), NumSlots);
	}

	bActive = Captured.Num() > 0;   // zero slots (empty mesh) -> not applied / inactive (AMB-2)
	UE_LOG(LogAnomaly, Log, TEXT("missing_texture: overrode %d slot(s) across %d component(s) for '%s' (checker)."),
		Captured.Num(), Meshes.Num(), *Substring);
	return bActive;
}

void FAnomaly_MissingTexture::Revert()
{
	// Restore each captured slot exactly; skip stale weak ptrs (GC-safe).
	for (const FCapturedSlot& Slot : Captured)
	{
		UMeshComponent* Mesh = Slot.Mesh.Get();
		if (!Mesh)
		{
			continue;
		}
		if (Slot.bWasExplicitOverride)
		{
			Mesh->SetMaterial(Slot.SlotIndex, Slot.OriginalMaterial.Get());   // restore the captured override ptr
		}
		else
		{
			Mesh->SetMaterial(Slot.SlotIndex, nullptr);   // clear the override we added -> back to the asset default
		}
	}
	Captured.Reset();
	bActive = false;
}
