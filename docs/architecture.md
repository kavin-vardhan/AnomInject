# Architecture (living — current as-built)

> **Reflects:** Viewport-Visibility Layer — a shared **`AnomalyViewport`** helper (frustum AND
> occlusion against an explicit view) and an **opt-in** subsystem toggle `IAI.SetViewportScoping <0|1>`
> (default **OFF**) that routes the four object-scoped, primitive-backed anomalies — `missing_object`,
> `flicker`, `lod_corruption`, `lod_popping` — through it so they affect only objects visible in the
> player's viewport. Built on M3 (LOD breadth + `AnomalyLod`), M2 (component/global anomalies + A1/A3
> helpers), M2.5/M2.6 (5.1 port + bridge sever). **Catalog: still 7 anomalies** (no new types this
> milestone). Required **no `IAnomaly` change** (the M1 lock held again) and **no new module
> dependency** (frustum/traces/camera are all Engine; `FReversedZPerspectiveMatrix` is Core).
> **State-gated green** (clean Development-Editor compile on 5.1, exit 0; synthetic-view frustum+occlusion
> gate + OFF-is-byte-identical regression gate driven over the bridge in a `MainWorld` Simulate session,
> session 008, 2026-06-18). Detail in `sessions/2026-06-18-008-viewport-visibility-layer.md`,
> `sessions/2026-06-13-006-m3-lod-breadth.md`,
> `sessions/2026-06-10-005-m2.5-m2.6-5.1-port-bridge-sever.md`,
> `sessions/2026-06-09-004-m2-breadth-round-1.md`; M1 in `sessions/2026-06-09-003-m1-implementation.md`.
> **Maintenance:** update this file to match the code at the end of every milestone; describe only
> what is built. Forward plans and design rationale live in the session journals.

## Engine support
- **Supported engine: UE 5.1 (canonical).** The two real target games are on 5.1, so 5.1 is the engine
  of record. Originally built and validated on a source-built **UE 5.4.4**; ported to 5.1 in M2.5 with
  **zero plugin-source changes** — all port watch-items (UTickableWorldSubsystem signatures,
  `GetComponents<T>`, `SetVisibility`/`SetLightColor`, `SetForcedLodModel`/`GetNumLODs`,
  `GNearClippingPlane`/`r.SetNearClipPlane`, dep set) were unchanged between 5.4 and 5.1. The module
  pins no `CppStandard`, inheriting 5.1's C++17 default (5.4 defaulted to C++20); the code uses no
  C++20-only syntax. Build-version constants are a **host** concern (gotcha G17): the 5.1 host targets
  use `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1`.
- **Canonical engine + host:** source-built UE 5.1 at `D:\UESource\UnrealEngine`; host project
  `D:\IntrusiveAnomalies\StackOBot` (natively-5.1 StackOBot). The old 5.4 host is retired.

## Purpose
AnomalyInjector injects labeled visual anomalies (graphics bugs) into a running UE5 game to
generate synthetic training data for bug-detection ML. Game-agnostic (public UE APIs only); tested
on the Stack O Bot sample.

## Module & load
- One **Runtime** module `AnomalyInjector`, `LoadingPhase Default`, `EnabledByDefault: true`
  (project-plugin scoped — gotcha G6), `VersionName 0.5.0`. Build.cs deps: `Core`, `CoreUObject`, `Engine` — **unchanged
  through the viewport milestone**: the A1 component finder, A3 arg parsing, the `AnomalyLod` LOD helper, every
  anomaly, and the new `AnomalyViewport` helper use only Engine/Core types. `AnomalyViewport`'s frustum
  (`FConvexVolume` / `GetViewFrustumBounds` / `FMinimalViewInfo`), line-trace occlusion
  (`UWorld::LineTraceSingleByChannel`), and live view resolution (`APlayerController` / `APlayerCameraManager`)
  are all Engine; `FReversedZPerspectiveMatrix` is Core. `camera_clipping` drives the near clip via the
  `r.SetNearClipPlane` console command + the `GNearClippingPlane` global (Core), deliberately avoiding a
  `RenderCore` dependency (gotcha G13).

## The anomaly abstraction — `IAnomaly`  (`Public/IAnomaly.h`)
Plain C++ polymorphic interface (NOT a UCLASS — dispatch needs no reflection). One instance per type.
```
GetId() / GetDescription() / GetUsage()   // identity + help
Apply(UWorld*, const TArray<FString>&)->bool   // true iff an observable effect was applied
Tick(float DeltaSeconds)                  // no-op default; override only for ticking anomalies
Revert()                                  // undo everything; leaves IsActive()==false
IsActive()->bool
```
Contract: `Apply` returns **false** (and stays inactive) when an actor anomaly matches zero actors
(AMB-2); re-applying an active anomaly **reverts-then-reapplies** (no state leak); anomalies cache
their targets/world as `TWeakObjectPtr` inside `Apply` (GC-safe) so `Tick` needs only `DeltaSeconds`.

## Core component — `UAnomalyInjectorSubsystem` (the manager)
- A `UTickableWorldSubsystem` (UCLASS): one per world, auto-ticks, `GetWorld()`. **Game + PIE only**
  via `DoesSupportWorldType` (gotcha G7).
- **Owns the registry** `TMap<FName, TUniquePtr<IAnomaly>>` — plain C++, not a UPROPERTY. Registers
  one instance of each anomaly type in `Initialize` (explicit, no self-registration macros). Needs an
  out-of-line destructor (gotcha G9).
- **Tick:** drives `Tick(Dt)` on the active anomalies; the 2 s heartbeat now reports `(active: N/Total)`.
- **Deinitialize:** `RevertAllActive()` then `Super` — the generalized auto-restore-on-teardown.
- **Re-entrancy:** dispatch is thin; each anomaly's `Apply` does the revert-then-reapply.

## Lifecycle / targeting shapes (why these seven anomalies)
The catalog deliberately spans the axes the interface must cover, proving it generalizes. M1 proved
the lifecycle axis (static / ticking / global); M2 proved the targeting & mutation axis (component-
scoped across two component types, and a global driven by a console command); M3 proved a **heterogeneous
target set** (one apply spanning static + skeletal components, dispatched per type behind a shared helper):
| anomaly | scope | ticks? | proves |
|---|---|---|---|
| `missing_object` | actor | no | the static, actor-scoped baseline (re-homes the M0 hide) |
| `flicker` | actor | **yes** | the `Tick` path |
| `time_dilation` | world-global | no | the interface does **not** assume actor-scoping |
| `lighting_mismatch` | **component** (ULightComponent) | no | component-level targeting (A1) + per-target full-state capture + multi-mode args |
| `lod_corruption` | **component** (static + skeletal mesh) | no | one capture convention over a **heterogeneous** target set; static/skeletal dispatch via `AnomalyLod` (M3) |
| `lod_popping` | **component** (static + skeletal mesh) | **yes** | the `Tick` path reused (flicker mechanics) over the `AnomalyLod` LOD dispatch (M3) |
| `camera_clipping` | global (near-clip) | no | global capture/restore via a console **command** (no cvar, no new dep) |

## Shared helpers

### Targeting — `AnomalyTargeting`  (`Public/AnomalyTargeting.h`)
Free functions (deliberately not a base class). Single source of truth for the label-free match rule.
- `FindActorsMatching(World, Substring)` — matches by `Actor->GetName()` **or**
  `Actor->GetClass()->GetName()` `.Contains(substring)` (case-insensitive), **never** `GetActorLabel()`
  (editor-only — gotcha G2). Returns weak-ptrs. Used by `missing_object`, `flicker`.
- `FindComponentsMatching<T>(World, Substring)` (**A1**, header-only template) — resolves matching
  actors via `FindActorsMatching` (same rule), then gathers each actor's components of type `T`
  (`AActor::GetComponents<T>`). Returns `TArray<TWeakObjectPtr<T>>`. Handles standalone light/mesh
  actors (class name carries the substring, e.g. `APointLight`) and lights/meshes-as-components on
  other actors uniformly. Used by `lighting_mismatch` (`<ULightComponent>`) and `lod_corruption`
  (`<UStaticMeshComponent>`) — two consumers, two component types.

### Argument parsing — `AnomalyArgs`  (`Public/AnomalyArgs.h` / `Private/AnomalyArgs.cpp`)
**A3.** `GetFloat / GetInt (value, Index, Default, Min, Max)` and `GetString (value, Index, Default)`.
Consolidates the AMB-6 parse/clamp/warn behavior: missing index → `Default` (silent); non-numeric →
warn + `Default`; out-of-range → warn + clamp; **never fails `Apply`**. Used by the M2 anomalies and
by `lod_popping` (Hz). (M1's `flicker`/`time_dilation` keep their inline parse — validated code left
untouched; the cosmetic divergence is intentional, not a TODO.)

### LOD forced-LOD dispatch — `AnomalyLod`  (`Public/AnomalyLod.h` / `Private/AnomalyLod.cpp`)  **(M3)**
Free functions (AnomalyTargeting/AnomalyArgs convention), justified by **2 consumers** (`lod_corruption`,
`lod_popping`). Single source of truth for forced-LOD across the two LOD-forceable component families —
`UStaticMeshComponent` (`SetForcedLodModel`/`ForcedLodModel`, count via `GetStaticMesh()->GetNumLODs()`)
and `USkinnedMeshComponent` (`SetForcedLOD`/`GetForcedLOD`, count via the component's own `GetNumLODs()`;
`USkeletalMeshComponent` derives). Both APIs are **1-based** (0 = auto/off; N forces LOD N-1). Surface:
- `ResolveLodComponents(World, Substring)` → merges `FindComponentsMatching<UStaticMeshComponent>` and
  `<USkinnedMeshComponent>` (disjoint siblings → duplicate-free) into `TArray<TWeakObjectPtr<UMeshComponent>>`.
- `GetWorstLod(Comp)` / `GetForcedLod(Comp)` / `SetForcedLod(Comp, n)` — dispatch on the concrete type via
  `Cast<>` internally, so callers hold one record keyed to the common base `UMeshComponent`.
- `ResolveTargetLod(Comp, RequestedOrSentinel)` — `WorstLodSentinel` → that component's worst LOD; an
  explicit 1-based index → clamped to `[1, max(WorstLod,1)]` (the default-worst / explicit-clamp rule).
The skinned LOD count uses the **runtime render-data** accessor `USkinnedMeshComponent::GetNumLODs()`,
the analog of the static `UStaticMesh::GetNumLODs()` — **not** the asset's authored `GetLODNum()`
(gotcha G19). All types are in `Engine` → **no new module dependency**.

### Viewport visibility — `AnomalyViewport`  (`Public/AnomalyViewport.h` / `Private/AnomalyViewport.cpp`)  **(viewport milestone)**
Free functions (the AnomalyTargeting/AnomalyArgs/AnomalyLod convention: light Public header, heavy includes +
backend dispatch in the .cpp). Single source of truth for "is this object actually visible to the player" =
**inside the camera frustum AND not occluded**. Lets the four object-scoped anomalies be opt-in scoped so a
corrupted frame is labeled-and-visible (the ML-relevant case), not labeled-but-invisible.
- **Core operates on an explicit view spec** `FAnomalyViewInfo { Origin, Rotation, HorizontalFOVDeg, AspectRatio,
  bValid }` + the world. Pure function of (view, world) → deterministic and **state-gatable with a synthetic
  view** (no live player needed). This split is deliberate: it keeps the live lookup thin and separately validated.
- **Frustum (always; synchronous):** assemble the reversed-Z VP from the view spec exactly as the engine's live
  path (`FReversedZPerspectiveMatrix` via `FMinimalViewInfo::CalculateProjectionMatrix` + the world→view basis
  swap), `GetViewFrustumBounds(VP, bUseNearPlane=true, bUseFarPlane=false)`, then `FConvexVolume::IntersectSphere`
  then `IntersectBox` against `UPrimitiveComponent::Bounds` (gotcha G24).
- **Occlusion (backend-agnostic; PRIVATE to the .cpp):** v1 = multi-sample **camera-to-bounds line trace**
  (`UWorld::LineTraceSingleByChannel`, `ECC_Visibility`; bounds center + 8 corners; ignore the target's own
  actor; unoccluded if **any** sample's path is unblocked). Synchronous, deterministic, synthetic-gatable.
  `UPrimitiveComponent::GetLastRenderTimeOnScreen()` is the documented drop-in **live** backend for the future
  capture/live-injection milestone — a .cpp-only swap (gotcha G22). Trade-off: the trace over-includes on
  no-collision / translucent occluders (safe direction — never drops a visible target).
- **Public surface:** `IsComponentVisible(View, World, Comp)`, `IsActorVisible(View, World, Actor)` (disjunction
  over the actor's primitive components — actor granularity); filter entry points `FilterVisibleActors(...)` and
  header-only `FilterVisibleComponents<T>(...)`; composed convenience finders wrapping AnomalyTargeting —
  `FindVisibleActorsMatching(World, Sub)` and header-only `FindVisibleComponentsMatching<T>(World, Sub)`. Also
  `IsComponentInFrustum(View, Comp)` — **frustum-only, NOT the visibility predicate** (visibility = frustum AND
  occlusion is the load-bearing invariant; this primitive exists for the synthetic-gate diagnostics / frustum
  calibration and must not be read as "is visible"). The occlusion test stays private to the .cpp.
- **Live resolver:** `GetActiveViewInfo(World, OutView)` fills the view spec from the first local player's POV
  (`APlayerController::GetPlayerViewPoint` + `PlayerCameraManager->GetFOVAngle()`; aspect from the game viewport).
  On no usable view it logs **one** warning, leaves `bValid=false`, and returns false; the convenience finders
  then return the **full matched set (treat-as-unscoped)**, never dropping targets. No editor-viewport fallback
  (no UnrealEd dep). A StackOBot Simulate session *does* expose a usable view (gotcha G23).
- **No per-frame cache in v1** (matched sets are small; visibility is tested once at Apply, not per tick). A
  frame-keyed memo of (view + per-primitive result) slots in here for the future live-injection milestone.
- All types are in `Core`/`Engine` → **no new module dependency**.

## Control surface (console commands)
Module-scoped `FAutoConsoleCommandWithWorldAndArgs`, resolved from the console's world, null-guarded
(warns outside Game/PIE). Output → Output Log, category `LogAnomaly`.
- `IAI.ListActors` — log `Class | Name | Label` per actor (targeting aid, not an anomaly).
- `IAI.ListAnomalies` — list registered anomalies as `id - description - usage` (sorted).
- `IAI.Apply <id> <args...>` — look up id, apply (reverts-then-reapplies if active).
- `IAI.Revert <id>` — revert one active anomaly.
- `IAI.RevertAll` — revert all active anomalies.
- `IAI.SetViewportScoping <0|1>` — toggle viewport-visibility scoping for the four object-scoped anomalies
  (default **OFF**; see "Viewport-visibility scoping" below).
- `IAI.TestVisibility <substring> <ox oy oz> <pitch yaw roll> [fovDeg] [aspect]` — **diagnostic** (not an
  anomaly): test the `AnomalyViewport` core against a **synthetic** view and log per-component
  `frustum / unoccluded / visible`. The deterministic synthetic-view state-gate driver.
*(M0's `IAI.HideActor` / `IAI.ShowAllActors` were removed — superseded by `IAI.Apply missing_object`
/ `IAI.RevertAll`.)*

## Anomaly catalog
| id | shape | usage | effect | revert | status |
|----|-------|-------|--------|--------|--------|
| `missing_object` | static, actor-scoped | `IAI.Apply missing_object <sub>` | `SetActorHiddenInGame(true)` on matches | un-hide / RevertAll / teardown | **as-built (M1)** |
| `flicker` | ticking, actor-scoped | `IAI.Apply flicker <sub> [hz]` | toggle hidden each half-period (default 5 Hz, clamp 60) | restore visible (any phase) | **as-built (M1)** |
| `time_dilation` | world-global, no tick | `IAI.Apply time_dilation <scale>` | `SetGlobalTimeDilation(scale)` (clamped — G11) | restore captured baseline (AMB-3) | **as-built (M1)** |
| `lighting_mismatch` | component (ULightComponent) | `IAI.Apply lighting_mismatch <sub> [off\|dim <f>\|recolor <r g b>\|noshadow]` | per mode: `SetVisibility(false)` / `SetIntensity(orig*f)` (def 0.1) / `SetLightColor(r,g,b)` (def magenta) / `SetCastShadows(false)`; default mode `dim` | restore captured intensity/color/visibility/cast-shadow per live comp; skip stale | **as-built (M2)** |
| `lod_corruption` | component (static **+ skeletal** mesh) | `IAI.Apply lod_corruption <sub> [lod-index]` | force each matched comp to a LOD via `AnomalyLod` (1-based; default worst per comp; explicit index clamped per comp). Static `SetForcedLodModel` / skinned `SetForcedLOD` | restore captured forced-LOD per live comp; skip stale | **as-built (M3)** — static + skeletal (G19; was static-only in M2, G16) |
| `lod_popping` | component (static **+ skeletal** mesh), **ticking** | `IAI.Apply lod_popping <sub> [hz]` | each half-period, snap every matched comp between its captured baseline LOD and its worst LOD via `AnomalyLod` (default 2 Hz, clamp ≤ 30) | restore captured baseline per live comp regardless of phase; reset accumulator/phase | **as-built (M3)** |
| `camera_clipping` | global (near-clip), no tick | `IAI.Apply camera_clipping [near]` | `r.SetNearClipPlane <near>` console command (default 100), pushing `GNearClippingPlane` out | restore captured baseline (~10) via the same command | **as-built (M2)** |

## Viewport-visibility scoping (opt-in; default OFF)
The subsystem holds one flag `bViewportScopingEnabled` (default **OFF**), toggled by `IAI.SetViewportScoping <0|1>`
and read by anomalies via the static `UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World)`. **Only the four
object-scoped, primitive-backed anomalies consult it:** `missing_object`, `flicker` (actor granularity — visible iff
**any** primitive component is visible) and `lod_corruption`, `lod_popping` (component granularity). When **ON**, each
routes target resolution through `AnomalyViewport` so it affects only objects visible in the player's view; when
**OFF**, each takes its original resolution path **byte-identical to before** (the regression gate). Excluded by
design: `lighting_mismatch` (a `ULightComponent` is not a primitive — that's the future region-darkening anomaly's
concern) and the two globals `time_dilation` / `camera_clipping` (whole-frame). **v1 semantics:** a matched object is
affected iff visible **at Apply time** (ticking anomalies fix their visible set at Apply; the tick does not re-test).
If scoping is ON but no live view resolves, the anomaly treats-as-unscoped (full matched set + one warning), so ON
never silently drops every target. The catalog rows above are unchanged in effect; scoping only narrows *which*
matched targets are acted on.

## Per-target / global state-capture convention
The generalization of M1's AMB-3 capture-baseline rule, followed by **every** state-mutating anomaly:
- **Capture exactly the state you mutate, before mutating it.** Globals: one baseline (e.g.
  `time_dilation` captures `GetGlobalTimeDilation`; `camera_clipping` captures `GNearClippingPlane`).
  Component/actor anomalies: a **small per-target record keyed to the weak ptr** (e.g.
  `lighting_mismatch` stores intensity/color/visibility/cast-shadow per `ULightComponent`;
  `lod_corruption` / `lod_popping` store the prior forced-LOD per `UMeshComponent`).
- **The record may key to a common base over a heterogeneous target set (M3).** `lod_corruption` and
  `lod_popping` key their record to `TWeakObjectPtr<UMeshComponent>` and let `AnomalyLod` dispatch the
  static-vs-skeletal getter/setter via `Cast<>`. A single `Apply` therefore captures/forces/reverts a
  **mixed** set — e.g. one `lod_corruption Bot` handles the Bot's static mesh component *and* its two
  skinned components together. The convention is unchanged; only the record's pointer type widened.
- **Revert restores the captured state and skips stale weak ptrs** (same GC-safety as `missing_object`).
- **Re-apply reverts-then-reapplies** so there is always exactly one capture set (no stacking — e.g.
  `lighting_mismatch recolor` re-applied never strands a recolored light; `lod_popping` re-applied
  mid-oscillation re-captures the *true* baseline, never a popped value).

## How to add an anomaly
1. Implement `IAnomaly` in `Private/Anomalies/Anomaly_<Name>.{h,cpp}`. In `Apply`: resolve+cache
   targets as weak-ptrs (`AnomalyTargeting::FindActorsMatching` for actors, `FindComponentsMatching<T>` for
   components, `AnomalyLod::ResolveLodComponents` for LOD-forceable static+skeletal meshes), parse args via
   `AnomalyArgs`, **capture per-target/global state before mutating** (convention above), mutate, return
   `false` if an actor/component anomaly matched zero targets (AMB-2). Undo in `Revert` (restore
   captured state per live target; skip stale). Override `Tick(float)` only if it ticks.
2. Register it in `UAnomalyInjectorSubsystem::Initialize`: `Register(MakeUnique<FAnomaly_<Name>>())`.
3. Include the header path-relative from `Private/`: `#include "Anomalies/Anomaly_<Name>.h"` (gotcha G10).
4. Add a catalog row above and a smoke line to the runbook.
No interface change is needed for actor-, component-, world-, or global/console-driven shapes (proven
across all seven anomalies — the M1 `IAnomaly` lock held through M3, including the ticking
`lod_popping` and the heterogeneous static+skeletal `lod_corruption`).

## Deferred (intentional — not forgotten)
- **`AnomalyCvar` (generic cvar capture/restore, the planned A2)** → post-process / scalability milestone.
  Deferred because its only would-be M2 consumer (`camera_clipping`) is driven by a console **command**,
  not an `IConsoleVariable` (gotcha G13), so A2 had zero real consumers and would have violated the
  ≥2-consumers bar. It lands with its first genuine `IConsoleVariable` anomaly.
- **`color_corruption` / `aliasing` / `blur`** → a dedicated post-process milestone (needs a deliberate
  injection-point decision: global PP volume vs camera `PostProcessSettings` vs PP material).
- **high/low-speed** → substantially covered by `time_dilation`; a `GlobalAnimRateScale` variant can be
  added later only if the label taxonomy needs the distinction.

## Limitations
- **Cross-anomaly target overlap** = last-writer-wins on the single `bHidden` flag (gotcha G12). Fine
  for one-anomaly-at-a-time use (terminal state after `RevertAll` is always visible). Compound /
  simultaneous anomalies will need a subsystem-level "hidden-by" coordinator — addable **without**
  touching `IAnomaly`. Flagged, not built.

## Game-agnostic invariant
The module depends only on `Core`/`CoreUObject`/`Engine` and never references host (StackOBot) types.
All anomalies use public UE APIs only — `SetActorHiddenInGame`, `UGameplayStatics`, `ULightComponent`
setters, `UStaticMeshComponent::SetForcedLodModel`, `USkinnedMeshComponent::SetForcedLOD`/`GetForcedLOD`/
`GetNumLODs`, and the `r.SetNearClipPlane` console command + `GNearClippingPlane` global. **Neither M2
nor M3 added a dependency** — the M3 LOD work touches only Engine component types (and specifically
still avoids `RenderCore` — G13). The Bot match in M3's gates is by class substring (`BP_Bot_C`), never
a host type or label.

## Verification model
Non-visual gates are checked in PIE via the `unreal-mcpython` MCP bridge (state/log reads: match
counts, `IsActive`, world time-dilation value, flicker toggle logs); the owner eyeballs the visual
gates (flicker, felt slowdown). See gotcha G8 for the bridge setup.
- **Bridge on 5.1 (M2.6):** the bridge (host tooling, not part of this plugin) is **GenOrca
  UnrealMCPython**, which targets UE 5.6+. To build on 5.1 its **`BehaviorTreeEditor` dependency was
  severed** (those graph-node UCLASSes are unexported pre-5.6 — G8). The bridge's BT-graph **authoring**
  tools (`build_behavior_tree`, `get_selected_bt_nodes`) are therefore **unavailable on 5.1**; everything
  this project's verification uses — `execute_python`, Output-Log reads, actor/component state reads —
  is intact. M2.5's full re-gate (Simulate session in an `EWorldType::PIE` world) was driven over the
  severed bridge. Full diagnosis + the restore-on-5.6 recipe are in G8.
