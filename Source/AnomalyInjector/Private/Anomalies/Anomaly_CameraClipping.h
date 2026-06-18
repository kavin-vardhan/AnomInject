// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IAnomaly.h"

class UWorld;

/**
 * camera_clipping — global, no actors, no Tick. Pushes the near clip plane out so geometry
 * near the camera clips away. Structurally the time_dilation shape (capture-baseline of a
 * global, restore on Revert) applied to the near-clip global.
 *
 * Mechanism (M2 / AMB-1): the near clip is NOT a console variable — `r.SetNearClipPlane` is an
 * FAutoConsoleCommand over the CORE global `GNearClippingPlane` (synced to the render thread by
 * RenderCore's SetNearClipPlaneGlobals). So we CAPTURE the baseline by reading GNearClippingPlane
 * (Core) and APPLY/REVERT by executing the `r.SetNearClipPlane <v>` console command (Engine) — no
 * new module dependency, and the command path correctly syncs the render-thread copy. The generic
 * cvar capture/restore helper (AnomalyCvar / A2) is deferred to the post-process/scalability milestone,
 * which has the genuine IConsoleVariable consumers it needs to earn its place. See gotcha G13.
 */
class FAnomaly_CameraClipping final : public IAnomaly
{
public:
	virtual FName   GetId() const override { return FName(TEXT("camera_clipping")); }
	virtual FString GetDescription() const override { return TEXT("Push the near clip plane out so near geometry clips away."); }
	virtual FString GetUsage() const override { return TEXT("[near-plane]"); }

	virtual bool Apply(UWorld* World, const TArray<FString>& Args) override;
	virtual void Revert() override;
	virtual bool IsActive() const override { return bActive; }
	// No Tick override — inherits the no-op.

private:
	/** World used only as the Exec context for the console command (the command itself is global). */
	TWeakObjectPtr<UWorld> WorldWeak;
	float PreviousNearClip = 10.0f;   // baseline captured in Apply (GNearClippingPlane default is 10)
	bool  bActive = false;

	static constexpr float DefaultNearClip = 100.0f;  // brief: default a large value, e.g. 100
	static constexpr float MinNearClip = 1.0f;        // r.SetNearClipPlane itself clamps to >= 1
	static constexpr float MaxNearClip = 100000.0f;

	/** Execute `r.SetNearClipPlane <Value>` via the console (Engine-only; syncs the render thread). */
	void ExecuteSetNearClip(UWorld* World, float Value) const;
};
