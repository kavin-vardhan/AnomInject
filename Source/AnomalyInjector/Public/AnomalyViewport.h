// Copyright GDP Anomaly Injection Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnomalyTargeting.h"   // FindActorsMatching / FindComponentsMatching<T> for the convenience finders

class UWorld;
class AActor;
class UPrimitiveComponent;

/**
 * Shared viewport-visibility helper (viewport milestone). Single source of truth for "is this
 * object actually visible to the player" = inside the camera frustum AND not occluded. Lets the
 * object-scoped anomalies be opt-in scoped to what the player can actually see, so a corrupted
 * frame is labeled-and-visible (the ML-relevant case) rather than labeled-but-invisible.
 *
 * Design (locked):
 *  - The CORE operates on an explicit view spec (FAnomalyViewInfo: origin, rotation, FOV, aspect)
 *    plus the world. It is a pure function of (view, world): deterministic and state-gatable with a
 *    synthetic view (no live player needed). Frustum is exact math on the reversed-Z view-projection
 *    matrix; occlusion is a multi-sample camera-to-bounds line trace, private to the .cpp.
 *  - GetActiveViewInfo(World) is the THIN live resolver: it fills a view spec from the first local
 *    player's POV and degrades gracefully (one clear warning, bValid=false) when there is none
 *    (e.g. a Simulate session, gotcha G23) so callers treat-as-unscoped rather than dropping every
 *    target.
 *
 * The occlusion backend is intentionally PRIVATE behind this API (backend-agnostic). v1 = the line
 * trace (synchronous, deterministic, synthetic-gatable, Engine-only). The render-time signal
 * UPrimitiveComponent::GetLastRenderTimeOnScreen() is the documented drop-in LIVE backend for the
 * future capture/live-injection milestone (a .cpp-only swap), where render-fidelity matters and a
 * per-frame cache is in play. The trace over-includes on no-collision / translucent occluders
 * (the accepted v1 trade-off — it errs on the safe side, never dropping a visible target). See G22.
 *
 * Convention-matched to AnomalyTargeting / AnomalyArgs / AnomalyLod: free functions in Public/ with
 * the API macro; the heavy includes (frustum / trace / camera) and the occlusion dispatch stay in the
 * .cpp. No new module dependency — every type used here lives in Core/Engine.
 */

/** Explicit camera view used by the core visibility tests. Pose only; the world is passed separately. */
struct FAnomalyViewInfo
{
	/** Camera world-space location (also the trace origin for the occlusion test). */
	FVector Origin = FVector::ZeroVector;

	/** Camera world-space orientation; forward = Rotation.Vector(). */
	FRotator Rotation = FRotator::ZeroRotator;

	/** Full HORIZONTAL field of view, in degrees (matches FMinimalViewInfo::FOV). */
	float HorizontalFOVDeg = 90.0f;

	/** Aspect ratio = width / height. */
	float AspectRatio = 16.0f / 9.0f;

	/** False until a view is resolved. GetActiveViewInfo sets it on success; synthetic views set it directly. */
	bool bValid = false;
};

namespace AnomalyViewport
{
	/**
	 * Resolve the live local-player view (first player controller's POV) into OutView. Returns true and
	 * sets OutView.bValid on success. On failure (no world / no local PC / no camera — e.g. a Simulate
	 * session, gotcha G23) logs ONE clear warning, leaves OutView.bValid = false, and returns false.
	 * Callers then treat-as-unscoped (AMB-V3). Never falls back to the editor viewport (no UnrealEd dep).
	 */
	ANOMALYINJECTOR_API bool GetActiveViewInfo(UWorld* World, FAnomalyViewInfo& OutView);

	/**
	 * Is Component inside View's frustum? Pure math (no world, no occlusion test). Invalid view or null -> false.
	 * CAVEAT: this is a frustum-only primitive, NOT the visibility predicate. Visibility = frustum AND occlusion
	 * (the load-bearing invariant) — use IsComponentVisible / IsActorVisible for that. This exists for the
	 * synthetic gate's per-component diagnostics and frustum calibration; do not treat a true result as "visible".
	 */
	ANOMALYINJECTOR_API bool IsComponentInFrustum(const FAnomalyViewInfo& View, const UPrimitiveComponent* Component);

	/**
	 * Core test: is Component visible from View = inside the frustum AND not occluded? Pure function of
	 * (View, World, Component). Invalid view (!bValid) or null component/world -> false.
	 */
	ANOMALYINJECTOR_API bool IsComponentVisible(const FAnomalyViewInfo& View, UWorld* World, const UPrimitiveComponent* Component);

	/**
	 * Core test: is Actor visible from View? Disjunction over the actor's primitive components — visible
	 * iff ANY primitive component is visible (actor granularity, AMB-V5). An actor with no primitive
	 * component is never visible.
	 */
	ANOMALYINJECTOR_API bool IsActorVisible(const FAnomalyViewInfo& View, UWorld* World, const AActor* Actor);

	/** Subset of In whose actor IsActorVisible(View, ...). Stable order; skips stale weak ptrs. */
	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FilterVisibleActors(
		const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& In);

	/**
	 * Subset of In whose component IsComponentVisible(View, ...). Header-only template so it instantiates
	 * per consumer type T (T must derive from UPrimitiveComponent — the upcast resolves in the consumer
	 * TU, which already includes the concrete component type, mirroring FindComponentsMatching<T>).
	 */
	template <typename T>
	TArray<TWeakObjectPtr<T>> FilterVisibleComponents(
		const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<T>>& In)
	{
		TArray<TWeakObjectPtr<T>> Result;
		for (const TWeakObjectPtr<T>& Weak : In)
		{
			T* Component = Weak.Get();
			if (Component && IsComponentVisible(View, World, Component))   // T* -> const UPrimitiveComponent* upcast
			{
				Result.Add(Weak);
			}
		}
		return Result;
	}

	/**
	 * Convenience finder: resolve the live view, match actors (AnomalyTargeting::FindActorsMatching), and
	 * return only the visible ones. If the live view can't be resolved, returns the FULL matched set
	 * (treat-as-unscoped, AMB-V3 — GetActiveViewInfo already warned). Composed entry for actor-scoped anomalies.
	 */
	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FindVisibleActorsMatching(UWorld* World, const FString& Substring);

	/**
	 * Convenience finder: resolve the live view, match components of type T
	 * (AnomalyTargeting::FindComponentsMatching<T>), and return only the visible ones. No-view ->
	 * full matched set (treat-as-unscoped). Header-only template (mirrors the actor finder).
	 */
	template <typename T>
	TArray<TWeakObjectPtr<T>> FindVisibleComponentsMatching(UWorld* World, const FString& Substring)
	{
		TArray<TWeakObjectPtr<T>> Matched = AnomalyTargeting::FindComponentsMatching<T>(World, Substring);
		FAnomalyViewInfo View;
		if (!GetActiveViewInfo(World, View))
		{
			return Matched;   // no live view -> treat-as-unscoped
		}
		return FilterVisibleComponents<T>(View, World, Matched);
	}
}
