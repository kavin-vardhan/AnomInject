#include "Anomalies/Anomaly_CorruptedTexture.h"

#include "AnomalyLod.h"
#include "AnomalyViewport.h"
#include "AnomalyInjectorSubsystem.h"
#include "AnomalyInjectorLog.h"
#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"

namespace AnomalyCorruptedTextureLocal
{
	constexpr int32 GMaxParentChainDepth = 8;

	bool IsPinkDerived(const UMaterialInterface* Current, const UMaterialInterface* Pink)
	{
		if (!Current || !Pink)
		{
			return false;
		}
		if (Current == Pink)
		{
			return true;
		}
		const UMaterialInstance* Instance = Cast<UMaterialInstance>(Current);
		for (int32 Depth = 0; Instance && Depth < GMaxParentChainDepth; ++Depth)
		{
			if (Instance->Parent == Pink)
			{
				return true;
			}
			Instance = Cast<UMaterialInstance>(Instance->Parent);
		}
		return false;
	}

	UMeshComponent* FindLiveComponentByName(AActor* Owner, const FName& ComponentName)
	{
		if (!Owner || ComponentName.IsNone())
		{
			return nullptr;
		}
		TArray<UMeshComponent*> Components;
		Owner->GetComponents<UMeshComponent>(Components);
		for (UMeshComponent* Component : Components)
		{
			if (Component && Component->GetFName() == ComponentName)
			{
				return Component;
			}
		}
		return nullptr;
	}
}

bool FAnomaly_CorruptedTexture::Apply(UWorld* World, const TArray<FString>& Args)
{
	if (!World)
	{
		return false;
	}
	if (Args.Num() == 0 || Args[0].IsEmpty())
	{
		UE_LOG(LogAnomaly, Warning, TEXT("corrupted_texture: usage <substring>"));
		return false;
	}

	if (bActive)
	{
		Revert();
	}

	UAnomalyInjectorSubsystem* Injector = World->GetSubsystem<UAnomalyInjectorSubsystem>();
	UMaterialInterface* Swap = Injector ? Injector->GetCorruptedTextureMaterial() : nullptr;
	if (!Swap)
	{
		UE_LOG(LogAnomaly, Error, TEXT("corrupted_texture: shipped pink material is unavailable (null) — cannot apply."));
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
		UE_LOG(LogAnomaly, Log, TEXT("corrupted_texture: matched 0 mesh component(s) for '%s'."), *Substring);
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
			Slot.Owner = Mesh->GetOwner();
			Slot.ComponentName = Mesh->GetFName();
			Slot.SlotIndex = i;
			Slot.OriginalMaterial = Mesh->GetMaterial(i);
			Slot.bWasExplicitOverride = Mesh->OverrideMaterials.IsValidIndex(i) && Mesh->OverrideMaterials[i] != nullptr;
			Captured.Add(Slot);

			Mesh->SetMaterial(i, Swap);
		}
		UE_LOG(LogAnomaly, Log, TEXT("corrupted_texture: '%s' -> pink on %d slot(s)."),
			*Mesh->GetName(), NumSlots);
	}

	AppliedPink = Swap;
	bActive = Captured.Num() > 0;
	UE_LOG(LogAnomaly, Log, TEXT("corrupted_texture: overrode %d slot(s) across %d component(s) for '%s' (pink)."),
		Captured.Num(), Meshes.Num(), *Substring);
	return bActive;
}

void FAnomaly_CorruptedTexture::Revert()
{
	UMaterialInterface* Pink = AppliedPink.Get();
	if (!Pink)
	{
		UE_LOG(LogAnomaly, Error,
			TEXT("corrupted_texture: revert cannot identify its own material (pink unavailable) — %d captured slot(s) left untouched to avoid stomping game materials."),
			Captured.Num());
		Captured.Reset();
		AppliedPink.Reset();
		bActive = false;
		return;
	}

	int32 Restored = 0;
	int32 DefaultReset = 0;
	int32 SkippedForeign = 0;
	int32 Unresolved = 0;
	int32 Swept = 0;
	int32 ReFound = 0;

	TArray<AActor*> TouchedOwners;

	for (const FCapturedSlot& Slot : Captured)
	{
		AActor* Owner = Slot.Owner.Get();
		if (Owner)
		{
			TouchedOwners.AddUnique(Owner);
		}

		UMeshComponent* Mesh = Slot.Mesh.Get();
		if (!Mesh)
		{
			Mesh = AnomalyCorruptedTextureLocal::FindLiveComponentByName(Owner, Slot.ComponentName);
			if (Mesh)
			{
				++ReFound;
			}
		}
		if (!Mesh)
		{
			++Unresolved;
			UE_LOG(LogAnomaly, Warning,
				TEXT("corrupted_texture: revert could not resolve component '%s' slot %d on '%s' (destroyed or renamed) — the owner sweep will catch any corruption left on its successor."),
				*Slot.ComponentName.ToString(), Slot.SlotIndex, *GetNameSafe(Owner));
			continue;
		}
		if (Slot.SlotIndex >= Mesh->GetNumMaterials())
		{
			++Unresolved;
			UE_LOG(LogAnomaly, Warning,
				TEXT("corrupted_texture: revert found component '%s' with %d slot(s); captured slot %d no longer exists — skipped."),
				*Mesh->GetName(), Mesh->GetNumMaterials(), Slot.SlotIndex);
			continue;
		}

		if (!AnomalyCorruptedTextureLocal::IsPinkDerived(Mesh->GetMaterial(Slot.SlotIndex), Pink))
		{
			++SkippedForeign;
			UE_LOG(LogAnomaly, Log,
				TEXT("corrupted_texture: revert left '%s' slot %d untouched — the game replaced it with '%s' after apply."),
				*Mesh->GetName(), Slot.SlotIndex, *GetNameSafe(Mesh->GetMaterial(Slot.SlotIndex)));
			continue;
		}

		UMaterialInterface* Original = Slot.OriginalMaterial.Get();
		if (Slot.bWasExplicitOverride && Original)
		{
			Mesh->SetMaterial(Slot.SlotIndex, Original);
			++Restored;
		}
		else
		{
			if (Slot.bWasExplicitOverride)
			{
				UE_LOG(LogAnomaly, Warning,
					TEXT("corrupted_texture: '%s' slot %d had a runtime material that no longer exists — reset to the mesh default so the game can re-take the slot."),
					*Mesh->GetName(), Slot.SlotIndex);
			}
			Mesh->SetMaterial(Slot.SlotIndex, nullptr);
			++DefaultReset;
		}
	}

	for (AActor* Owner : TouchedOwners)
	{
		TArray<UMeshComponent*> Components;
		Owner->GetComponents<UMeshComponent>(Components);
		for (UMeshComponent* Component : Components)
		{
			if (!Component)
			{
				continue;
			}
			const int32 NumSlots = Component->GetNumMaterials();
			for (int32 i = 0; i < NumSlots; ++i)
			{
				if (AnomalyCorruptedTextureLocal::IsPinkDerived(Component->GetMaterial(i), Pink))
				{
					Component->SetMaterial(i, nullptr);
					++Swept;
					UE_LOG(LogAnomaly, Warning,
						TEXT("corrupted_texture: swept leftover pink off '%s' slot %d on '%s' (component was re-created after apply) — reset to the mesh default."),
						*Component->GetName(), i, *Owner->GetName());
				}
			}
		}
	}

	UE_LOG(LogAnomaly, Log,
		TEXT("corrupted_texture: revert of %d captured slot(s) — restored=%d default-reset=%d left-to-game=%d unresolved=%d swept=%d (re-found=%d)."),
		Captured.Num(), Restored, DefaultReset, SkippedForeign, Unresolved, Swept, ReFound);

	Captured.Reset();
	AppliedPink.Reset();
	bActive = false;
}
