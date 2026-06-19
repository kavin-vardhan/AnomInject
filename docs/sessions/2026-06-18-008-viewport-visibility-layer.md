# Session 008 — Viewport-Visibility Layer (2026-06-18)

## Goal
Stand up a shared **`AnomalyViewport`** helper so anomalies can be scoped to objects actually
**visible in the player's viewport** (frustum AND occlusion), and retrofit the existing object-scoped,
primitive-backed anomalies behind an **opt-in** toggle (default OFF). New bug types are a later
milestone — not this one. Built directly on the GDP-rename refactor (`351c7e8`).

## Locked design (from the planning turn, all approved)
- Helper `AnomalyViewport` — free functions, light Public header + heavy `.cpp` (AnomalyTargeting/Args/Lod
  convention). Occlusion backend kept **private** behind a backend-agnostic API.
- Core operates on an explicit view spec `FAnomalyViewInfo { Origin, Rotation, HorizontalFOVDeg, AspectRatio,
  bValid }` + world → deterministic, **synthetic-view-gatable**. Thin `GetActiveViewInfo(World)` resolves the
  live local-player view and degrades gracefully (warn + treat-as-unscoped) when there is none.
- Visibility = frustum (always; synchronous) AND occlusion.
- Retrofit scope = the **4** object-scoped primitive-backed anomalies (`missing_object`, `flicker`,
  `lod_corruption`, `lod_popping`). NOT `lighting_mismatch` (light ≠ primitive) nor the two globals.
- Scoping opt-in via `IAI.SetViewportScoping <0|1>`, **default OFF**; default-OFF leaves every existing gate
  byte-identical (regression gate).
- **No `IAnomaly` change, no new module dependency.** No per-frame cache in v1.

## Ambiguities — owner rulings
- **AMB-V1 (occlusion backend):** v1 core = **multi-sample camera-to-bounds line trace**
  (`LineTraceSingleByChannel`, `ECC_Visibility`; bounds center + 8 corners; ignore the target's own actor;
  unoccluded if any sample unblocked). Rationale: the core must stay deterministically synthetic-view-gatable,
  which render-time cannot honor (render-time is a property of the real rendered view — it made the verification
  model self-contradictory). `GetLastRenderTimeOnScreen()` is the verified drop-in **live** backend for the
  future capture/live-injection milestone (.cpp-only swap). Trace over-includes on no-collision/translucent
  occluders — accepted v1 trade-off (safe direction; never drops a visible target).
- **AMB-V2:** if/when render-time is used, it's `GetLastRenderTimeOnScreen()` (NOT
  `GetLastRenderTime()`/`WasRecentlyRendered()` — shadow-contaminated), with the custom tolerance
  `World->TimeSince(GetLastRenderTimeOnScreen()) <= max(Tol, DeltaTime+eps)`.
- **AMB-V3:** scoping ON + no live view → return the full matched set + one warning (treat-as-unscoped). No
  editor-viewport/UnrealEd fallback. Default-OFF and on-but-no-view both stay byte-identical to today.
- **AMB-V4:** `flicker→blinking` rename, region-darkening, new `flickering`, UI, auto-injection all out of scope;
  the id `flicker` is unchanged by the retrofit.
- **AMB-V5:** actor-scoped anomalies filter at actor granularity (visible iff any primitive visible);
  component-scoped LOD anomalies filter at component granularity.

## Source verification (UE 5.1.1, Release-5.1 @ `D:\UESource\UnrealEngine`)
- **Frustum chain (G24):** `FSceneViewProjectionData` (`SceneView.h:36`),
  `ComputeViewProjectionMatrix` (`:82`), reversed-Z infinite-far note (`:44`);
  `GetViewFrustumBounds(FConvexVolume&, VP, bUseNearPlane[, bUseFarPlane])` (`ConvexVolume.h:186/196`);
  `FConvexVolume::IntersectBox/IntersectSphere` (`:96/:114`); `Bounds` on `USceneComponent` (`SceneComponent.h:128`);
  view-rotation basis swap + `CalculateProjectionMatrixGivenView` (`LocalPlayer.cpp:1139/1149`);
  synthetic projection `FReversedZPerspectiveMatrix(max(0.001,FOV)*PI/360, Aspect, 1, Near)` via
  `FMinimalViewInfo::CalculateProjectionMatrix` (`CameraStackTypes.cpp:89`).
- **Occlusion signal (G22):** component render time updated gated on `PrimitiveDefinitelyUnoccludedMap` with
  `bUpdateLastRenderTimeOnScreen=true` in the main view (`SceneVisibility.cpp:2491`); shadow passes use
  `false` (`ShadowSetup.cpp:1672/1909`); `UpdateComponentLastRenderTime` writes `LastRenderTime` always +
  `LastRenderTimeOnScreen` only when the bool + `OwningActor->LastRenderTime` (`PrimitiveSceneInfo.cpp:2135`).
  ⇒ `GetLastRenderTimeOnScreen()` is the occlusion-clean signal; `GetLastRenderTime()`/`WasRecentlyRendered()`
  are shadow-contaminated.
- **Live resolver / Simulate (G23):** `UWorld::GetFirstPlayerController` (`World.h:2532`);
  `APlayerController::GetPlayerViewPoint` (`PlayerController.h:1826`) + `PlayerCameraManager` (`:265`);
  `UWorld::LineTraceSingleByChannel` (`World.h:1922`). Live finding: a StackOBot **Simulate** session *does*
  expose a usable view (`PlayerController_0` + valid `PlayerCameraManager` POV), refining the planning fear.

## What was done
1. **New `Public/AnomalyViewport.h` + `Private/AnomalyViewport.cpp`** — `FAnomalyViewInfo`; `GetActiveViewInfo`;
   `IsComponentInFrustum`, `IsComponentVisible`, `IsActorVisible`; `FilterVisibleActors` +
   header-only `FilterVisibleComponents<T>`; convenience finders `FindVisibleActorsMatching` +
   header-only `FindVisibleComponentsMatching<T>`. Reversed-Z VP assembly + line-trace occlusion private to the .cpp.
2. **Subsystem toggle** (`AnomalyInjectorSubsystem.{h,cpp}`) — `bViewportScopingEnabled=false` (default OFF),
   `SetViewportScoping`, member + static `IsViewportScopingEnabled`; diagnostic `TestVisibility`; console commands
   `IAI.SetViewportScoping` and `IAI.TestVisibility`; heartbeat now prints `scoping: ON/OFF`.
3. **4 retrofits** — `missing_object` / `flicker` branch to `AnomalyViewport::FindVisibleActorsMatching` when ON;
   `lod_corruption` / `lod_popping` resolve via `AnomalyLod` then `FilterVisibleComponents` when ON. OFF path
   unchanged. Excluded files untouched (`lighting_mismatch`, `time_dilation`, `camera_clipping`, leaf helpers,
   Build.cs, bridge, host).
4. **VersionName 0.4.0 → 0.5.0.** Docs updated (architecture/onboarding/runbook §6+§7/gotchas G22-G24/CLAUDE).
5. **Handoff/roadmap doc committed first** as a standalone `docs:` commit (`3da4562`) — it predated this milestone.

## Gates (clean compile exit 0; bridge, MainWorld Simulate)
- **Synthetic frustum (`IAI.TestVisibility SM_Ramp`):** looking at ramps → `frustum=1 unoccluded=1 visible=1`;
  looking away (behind) → `frustum=0` (near-plane); 53k units away in cone → `frustum=1` (far not clipping).
  Reversed-Z VP validated.
- **Synthetic occlusion (controlled wall via the editor-world-blocker trick):** wall between camera and ramps →
  both `frustum=1 unoccluded=0 visible=0`; clear from the far side (wall behind ramps) → both
  `frustum=1 unoccluded=1 visible=1`. Same targets; occlusion flips on line-of-sight only.
- **Regression (scoping OFF, default):** `missing_object SM_Ramp` hides/reverts both ramps (M1-identical);
  `lod_corruption SM_Ramp` forces `forced_lod_model 0→1→0` (M3-identical); `ListAnomalies` still **7**.
- **Scoping ON in Simulate:** live view resolved (no warning), both in-cone ramps affected. Treat-as-unscoped
  degrade is code-verified but not bridge-triggerable (Simulate always had a view).
- **Owner-pending:** live-resolver off-screen/on-screen discrimination in **real Play** (the only gate not
  closable in Simulate).

## Problem → Resolution
- **Synthetic occlusion needs a PIE-world blocker, but `EditorActorSubsystem.spawn_actor_from_object` refuses to
  spawn during play.** Resolution: end Simulate → spawn a large `/Engine/BasicShapes/Cube` (default collision
  blocks `ECC_Visibility`) in the **editor** world → restart Simulate (it duplicates into PIE) → gate → end
  Simulate → `destroy_actor` the blocker, never saving the map. (Recorded in runbook §7.)
- **Benign `Chaos::SweepQuery` ensure during Simulate** (`Dir.SizeSquared()≈1` from `UControlRigComponent::
  OnPostForwardsSolve` — the Bot's anim ControlRig) — unrelated to `AnomalyViewport`'s `LineTraceSingleByChannel`;
  disregarded.
- **Python enum/API friction over the bridge** (`CollisionResponse.ECR_BLOCK` → `.BLOCK`; `spawn_actor_from_object`
  returns None during play) — worked around; the cube's default collision blocks Visibility anyway.

## State
- Clean Development-Editor compile on 5.1 (exit 0); all bridge-closable gates green. Catalog unchanged at 7.
  VersionName 0.5.0. Editor + bridge left up at MainWorld for the owner's real-Play eyeball. MainWorld not saved
  (blocker deleted; level clean).
- Plugin repo HEAD `351c7e8` (rename) + `3da4562` (handoff doc, `docs:`); the viewport feature is **uncommitted**
  in the working tree pending owner acceptance.

## Hand-off
- **ACCEPTED** — owner live-gate green in real Play (off-screen matched object untouched, on-screen one affected).
  Both ratified deviations kept: `IAI.TestVisibility` (diagnostic / synthetic-gate entry point) and the public
  `IsComponentInFrustum` (documented frustum-only, NOT the visibility predicate, in its header + architecture.md).
  AMB-V3's no-view degrade stays code-verified (not bridge-triggerable — Simulate exposes a view; errs safe = OFF
  behavior; G23). Committed **`7c34275`**, tagged **`m4`**. Bridge/host stay unversioned (G8 unchanged).
- Next per the roadmap (`docs/viewport-and-roadmap-handoff.md`): finish the High-priority visual bugs built
  viewport-aware from birth, then the debug UI, then automatic injection.
