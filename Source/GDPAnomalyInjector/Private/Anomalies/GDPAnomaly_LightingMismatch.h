// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IGDPAnomaly.h"

class UWorld;
class ULightComponent;

/**
 * lighting_mismatch — component-scoped, no Tick. Resolves ULightComponents via the component
 * targeting helper (A1), captures each component's full mutable state (intensity, color,
 * visibility, cast-shadow) keyed to its weak ptr BEFORE mutating, then applies one of four
 * modes. Revert restores the captured state per still-live component and skips stale ptrs
 * (per-target state-capture convention).
 *
 * Modes (usage: <substring> [mode] [args], default dim):
 *   off                         -> SetVisibility(false)
 *   dim <factor 0-1, def 0.1>   -> SetIntensity(original * factor)
 *   recolor <r g b, def magenta>-> SetLightColor(r,g,b)
 *   noshadow                    -> SetCastShadows(false)
 *
 * Visibility note: only Movable (and partially Stationary) lights change the rendered image at
 * runtime; Static/baked lights change the component property but not the image (gotcha G14).
 */
class FGDPAnomaly_LightingMismatch final : public IGDPAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("lighting_mismatch")); }
	virtual FString GetDescription() const override { return TEXT("Mismatch lights on matching actors (off / dim / recolor / noshadow)."); }
	virtual FString GetUsage() const override { return TEXT("<substring> [off | dim <f> | recolor <r g b> | noshadow]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	// No Tick override — inherits the no-op.

private:
	/** Per-target capture: exactly the state lighting_mismatch can mutate, keyed to the weak ptr. */
	struct FCapturedLight
	{
		TWeakObjectPtr<ULightComponent> Light;
		float        Intensity = 0.0f;
		FLinearColor Color = FLinearColor::White;
		bool         bVisible = true;
		bool         bCastShadows = true;
	};

	TArray<FCapturedLight> Captured;
	bool bActive = false;
};
