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

/**
 * Per-actor descriptor for the control-surface read-back (Slice 1, A4). One entry per renderable-visible
 * actor: identity + the renderable component kind + distance + a normalized screen-space rect for the
 * dashboard's overlay boxes / click-to-select. Computed in the SAME pass as the visibility test (no
 * second occlusion pass); the rect is a pure projection of the matched component's bounds.
 */
struct FRenderableActorInfo
{
	TWeakObjectPtr<AActor> Actor;
	FString ActorName;
	FString ClassName;
	FString ComponentType;              // "SM" | "SK" (the matched renderable component family; VFX removed, G33)
	float Distance = 0.0f;              // view origin -> matched component bounds centre

	// Normalized [0,1] screen rect, top-left origin (resolution-independent; the client scales to its view).
	FVector2D ScreenMin = FVector2D::ZeroVector;
	FVector2D ScreenMax = FVector2D::ZeroVector;
	bool bRectValid = false;            // false if the bounds couldn't be projected (e.g. straddling the camera)
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

	// --- Renderable-visible set (m5 follow-on) ---
	// For the object selector / future auto-injection, "visible" must mean VISIBLE AND RENDERABLE:
	// in-frustum, unoccluded, AND actually drawing geometry to the screen. A pure frustum+occlusion test
	// (the functions above) also passes non-rendering primitives — collision boxes, capsules, RVT bounds
	// boxes, editor billboards, landscape, debug/streaming actors — which must never be injectable targets
	// (injecting on a never-visible actor is the unlabeled-but-invisible sample this layer exists to
	// prevent). These are SEPARATE, additive entry points: every function above is byte-identical and
	// unchanged (the m4 scoping-ON path and all prior gates keep their guarantees).

	/**
	 * Is Component a renderable target = IsVisible() (NOT hidden-in-game, visible flag set, level visible)
	 * AND one of the rendering component families we treat as injectable geometry: static mesh OR skeletal/
	 * skinned mesh. This is a capability/TYPE test, NOT a class blocklist (a blocklist would rot on a different
	 * title; this stays game-agnostic). Null -> false.
	 * VFX REMOVED (G33): particle/VFX (UFXSystemComponent — Niagara + Cascade) was deliberately dropped from the
	 * allowlist, reversing the original G29/R1 inclusion — particles are not useful injectable geometry for the
	 * selector / auto-injector / dashboard set. The "=name" console escape hatch (AnomalyTargeting) still reaches
	 * VFX actors directly; it does NOT go through this predicate.
	 * Empty-instance refinement: an instanced static mesh (or HISM) with ZERO instances draws nothing, so it
	 * is treated as non-renderable ("renders nothing => not renderable") — this drops 0-instance landscape-grass
	 * ISMs (which would otherwise leak a LandscapeStreamingProxy in) while keeping real foliage / populated ISMs.
	 * IsVisible() (not ShouldRender()) is deliberate: ShouldRender() has a non-shipping branch that returns
	 * true for hidden collision components under a world flag — a determinism footgun for the synthetic gate.
	 * EXTENSION POINT: to make terrain selectable on a future title, add `|| IsA<ULandscapeComponent>()` in
	 * the .cpp (one line; still a type test) — intentionally NOT active here (landscape is excluded for v1).
	 */
	ANOMALYINJECTOR_API bool IsRenderableComponent(const UPrimitiveComponent* Component);

	/**
	 * Is Actor renderable-visible from View? Disjunction over its primitive components — true iff ANY is
	 * IsRenderableComponent AND in-frustum AND unoccluded (actor granularity). The cheap renderability
	 * type/flag test runs FIRST, so non-targets are rejected before the occlusion line traces (correctness
	 * + a perf win: fewer traces). Invalid view / null -> false.
	 */
	ANOMALYINJECTOR_API bool IsActorRenderableVisible(const FAnomalyViewInfo& View, UWorld* World, const AActor* Actor);

	/** Subset of In that IsActorRenderableVisible(View, ...). Stable order; skips stale weak ptrs.
	 *  NOTE: this explicit-view function is part of the synthetic-gate surface and is NOT subject to the
	 *  poll-radius cull (it always passes radius 0 internally) — the cull is a property of the LIVE whole-scene
	 *  poll entry points below, not the deterministic (view, world) core. */
	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> FilterRenderableVisibleActors(
		const FAnomalyViewInfo& View, UWorld* World, const TArray<TWeakObjectPtr<AActor>>& In);

	// --- Poll-radius distance cull (changeable; default OFF) ---
	// An optional distance cull layered onto the LIVE renderable-visible poll (GetVisibleRenderableActors and
	// GetVisibleRenderableActorInfos only). When a positive radius R is set, an actor is in the set iff it is
	// renderable AND within R of the POLL ORIGIN (the player PAWN location, not the camera) AND in-frustum AND
	// unoccluded. Metric = sphere-approximated bounds distance: dist(PollOrigin, Bounds.Origin) - SphereRadius <= R.
	// The cull runs after the renderable type-test and before the occlusion line-traces (cheapest-cull-first).
	// SINGLE SHARED STATE, one source of truth, lives in AnomalyViewport.cpp. Default OFF via a sentinel:
	// R <= 0 disables the cull entirely, so behavior is BYTE-IDENTICAL to no-cull (no extra work, no actors
	// dropped). Positive R enables it. Applied identically to BOTH live entry points, so the IAI.DumpVisible
	// set-identity gate (GetVisibleRenderableActors vs GetVisibleRenderableActorInfos) still holds. The explicit-
	// view functions (IsActorRenderableVisible / FilterRenderableVisibleActors) are NOT culled (synthetic surface).
	// Console: IAI.SetPollRadius <value> (Unreal units / cm), registered in AnomalyViewport.cpp.

	/** Set the shared poll-radius cull distance in Unreal units (cm). R <= 0 disables the cull (default OFF).
	 *  Toggling across the 0 boundary also (un)registers the dev debug-draw sphere (drawn only while R > 0). */
	ANOMALYINJECTOR_API void SetPollRadius(float Radius);

	/** The current shared poll-radius (cm). <= 0 means the cull is OFF. */
	ANOMALYINJECTOR_API float GetPollRadius();

	/**
	 * Suppress ONLY the dev debug SPHERE draw (the radius VISUAL), leaving the poll-radius CULL fully active.
	 * The cull (GetVisibleRenderableActors / ...Infos) never reads this flag, so the visible SET is unchanged —
	 * this hides only the on-screen overlay. Intended for the frame-capture path: a captured game-viewport frame
	 * (FViewport::ReadPixels) bakes in the line-batcher sphere, so capture must suppress it while a run is active
	 * and restore on stop/teardown (the cull keeps shrinking the set as normal). Default = NOT suppressed (live
	 * monitoring shows the sphere). Single global writer expected (capture); set true on run-start, false on end.
	 */
	ANOMALYINJECTOR_API void SetDebugSphereSuppressed(bool bSuppressed);

	/** Is the dev debug sphere currently suppressed (e.g. during a capture run)? Does not affect the cull. */
	ANOMALYINJECTOR_API bool IsDebugSphereSuppressed();

	// --- Screen-coverage candidate cull (changeable; default OFF) ---
	// An optional ACTOR-LEVEL cull layered onto the LIVE renderable-visible poll (GetVisibleRenderableActors and
	// GetVisibleRenderableActorInfos only), sibling to the poll-radius cull. When a positive percentage P is set, an
	// actor is in the set iff it is renderable-visible (renderable AND in-frustum AND unoccluded AND within the poll
	// radius) AND its on-screen footprint covers >= P% of the viewport. Footprint = the CLAMPED [0,1] screen AABB of
	// the UNION of the actor's renderable-VISIBLE component bounds (the components that passed the per-component test),
	// projected with the same reversed-Z VP the frustum uses; coverage = that rect's area (the rect is normalized, so
	// the viewport area is 1). Clamp-before-area means a huge object with only a tiny on-screen sliver reads as the
	// sliver, not full coverage. SINGLE SHARED STATE in AnomalyViewport.cpp. Default OFF via a sentinel: P <= 0 disables
	// the cull, so behavior is BYTE-IDENTICAL to no-cull AND the cheap first-match short-circuit is preserved (the OFF
	// path costs no extra traces). Applied identically to BOTH live entry points through the shared per-actor classifier
	// (ClassifyRenderableVisibleLive), so the IAI.DumpVisible set-identity gate still holds with the cull ON. The
	// explicit-view functions (IsActorRenderableVisible / FilterRenderableVisibleActors) are NOT culled (synthetic
	// surface, conceptually P=0). Runs LAST (most expensive; actor-level). Console: IAI.SetMinScreenCoverage <pct>;
	// diagnostic IAI.DumpCoverage. Composes with the poll-radius cull (independent gates).

	/** Set the shared minimum on-screen coverage cull, as a PERCENT of the viewport area [0,100]. P <= 0 disables it
	 *  (default OFF) -> byte-identical to no-cull. */
	ANOMALYINJECTOR_API void SetMinScreenCoveragePct(float Pct);

	/** The current shared minimum on-screen coverage percent. <= 0 means the cull is OFF. */
	ANOMALYINJECTOR_API float GetMinScreenCoveragePct();

	/**
	 * Convenience for the object selector / future auto-injection: resolve the live view, enumerate all
	 * actors, and return the renderable-visible ones (UNSORTED; the caller orders).
	 * DELIBERATE CONTRACT — on no resolvable view this returns EMPTY (offer nothing), never the full scene.
	 * This differs ON PURPOSE from the FindVisible*Matching finders, which treat-as-unscoped (return the full
	 * matched set) on no view: those serve an explicit console instruction (act, don't silently drop), whereas
	 * the selector/auto-injection must never offer or inject BLIND (no legitimate visible set => nothing).
	 * Two callers, two safe directions — a future reader must NOT "reconcile" them.
	 */
	ANOMALYINJECTOR_API TArray<TWeakObjectPtr<AActor>> GetVisibleRenderableActors(UWorld* World);

	/**
	 * Control-surface read-back (Slice 1, A4): the renderable-visible set as FRenderableActorInfo — the same
	 * actors as GetVisibleRenderableActors (identical predicate + iteration order) PLUS, per actor, its
	 * class / renderable component kind / distance / a normalized screen-space rect. Runs ONE pass: the
	 * occlusion test fires once per actor (shared with the visibility predicate, no double traces); the rect
	 * is a pure projection against the resolved view. Same offer-nothing-on-no-view contract (empty on no view).
	 */
	ANOMALYINJECTOR_API TArray<FRenderableActorInfo> GetVisibleRenderableActorInfos(UWorld* World);

	/**
	 * Project an actor's renderable bounds to a normalized screen-space rect (top-left origin), for the
	 * capture/labeling milestone's 2D bounding box (L2). Built on the SAME reversed-Z VP path the frustum /
	 * GetVisibleRenderableActorInfos pass uses (the one source of truth) — synthetic and live views agree.
	 *
	 * The bounds are the UNION of the actor's static- + skeletal-mesh component bounds, selected by TYPE
	 * ONLY — deliberately NOT gated on IsVisible(). This is load-bearing: missing_object / flicker HIDE the
	 * actor, so by capture time it is no longer in the renderable-visible set, but its component bounds /
	 * transform persist. The label box = "where the (now-hidden) object is", the correct missing-object
	 * label — which is exactly why this projects the fired actor's bounds DIRECTLY rather than reading a rect
	 * off GetVisibleRenderableActorInfos (that set excludes the hidden actor).
	 *
	 * OutMin/OutMax are the UNCLAMPED normalized rect ([0,1] = on-screen; values may fall outside for a
	 * partially off-screen actor — the caller clamps when converting to pixels). Returns true iff the box has
	 * at least one corner in front of the camera AND the rect intersects the [0,1]x[0,1] screen; returns false
	 * (and zeroes the outputs) when the actor is entirely behind the camera or fully off-screen, or on an
	 * invalid view / null actor / no mesh component. A false return is a degenerate SPATIAL label only — the
	 * caller still records the frame's temporal label.
	 */
	ANOMALYINJECTOR_API bool ProjectActorBoundsToScreenRect(
		const FAnomalyViewInfo& View, const AActor* Actor, FVector2D& OutMin, FVector2D& OutMax);

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
