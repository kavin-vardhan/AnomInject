// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;
class UMeshComponent;
class UMaterialInterface;

/**
 * missing_texture — object-scoped (m8). Produces a "missing texture" look on an actor by swapping its
 * renderable static/skeletal mesh components' material slots to a shipped Lit gray/white UV-checker material.
 * The swap is a per-component SetMaterial override (object isolation — it never mutates the shared mesh or
 * material asset, so sibling actors sharing the same source are untouched). The material is the plugin-shipped
 * one held by the injector subsystem (CDO hard-ref; see GetMissingTextureMaterial). Each overridden slot's
 * prior material is captured for an exact revert. No Tick.
 *
 * NOTE: a flat-magenta variant (and a mode arg to select it) is deferred — unlit-emissive magenta lit the
 * scene via Lumen ("glowed" onto neighbours); the owner is revisiting that look. See the m8 session journal.
 */
class FAnomaly_MissingTexture final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("missing_texture")); }
	virtual FString GetDescription() const override { return TEXT("Missing-texture look (UV checker) on an actor's meshes."); }
	virtual FString GetUsage() const override { return TEXT(""); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	// No Tick override — inherits the no-op.

private:
	/** One overridden material slot, captured for an exact revert (the per-target state-capture convention). */
	struct FCapturedSlot
	{
		TWeakObjectPtr<UMeshComponent> Mesh;
		int32 SlotIndex = 0;
		TWeakObjectPtr<UMaterialInterface> OriginalMaterial;   // effective material before the swap; GC-safe
		bool bWasExplicitOverride = false;                     // OverrideMaterials[i] was set pre-apply
	};
	TArray<FCapturedSlot> Captured;
	bool bActive = false;
};
