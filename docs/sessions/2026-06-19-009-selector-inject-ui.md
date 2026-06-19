# Session 009 — Object Selector + Inject UI (minimal) (2026-06-19)

## Goal
Stand up a real-Play, in-game UI to **select a visible on-screen object** (Tab-cycle over the m4
`AnomalyViewport` visible set) and **inject an anomaly on it**, then revert — the roadmap's "debug UI"
reframed from toggle-on/off to select-and-inject. Minimal first cut. Built on m4 (`7c34275`).

## Locked design (from the planning turn, all approved)
- New **separate** subsystem `UAnomalySelectorSubsystem : UTickableWorldSubsystem` (Game+PIE only) owns
  selection state + input polling + HUD draw, and calls the existing injector's public `Apply`/`Revert`.
  The injector subsystem, `IAnomaly`, and all 7 anomalies stay **untouched**. (If `IAnomaly` had to change →
  STOP + flag. It did not.)
- **Explicit-core / thin-shell split (as m4):** public methods `AdvanceSelection` / `SelectPrevious` /
  `CycleAnomalyChoice` / `InjectSelected` / `RevertSelected` + readbacks `GetSelectedActorName` /
  `GetVisibleActorNames` / `GetAnomalyChoice` are the state-gatable surface. Two thin shells drive them: the
  `IAI.Selector.*` console commands (bridge gate) and per-tick raw input poll + immediate-mode HUD (eyeball).
- Tab cycles visible **actors** from the m4 actor-level visible set (frustum AND occlusion). Selection tracked
  by `TWeakObjectPtr` identity; name-sorted order for v1 (deterministic/testable; screen-X is the next polish).
  If the selected actor leaves the visible set → clear selection.
- Inherently visible-scoped: the candidate set IS the visible set, so this path does **not** touch
  `IAI.SetViewportScoping`.
- Inject path passes the selected actor's `GetName()` as the Apply substring → **no `IAnomaly` change**.
- UI offers the 4 object-scoped anomalies (`missing_object`, `flicker`, `lod_corruption`, `lod_popping`) on the
  selected actor, default args. Globals + `lighting_mismatch` stay console-only for v1.
- Minimal presentation = **immediate-mode HUD** (Canvas / debug-draw), NO UMG asset, NO custom-depth outline.
  Highlight = world-space debug box + on-screen name label; two on-screen lists (visible names, anomaly choices).
- Game-agnostic: HUD must not need the host's HUD/GameMode class; input must not need host input-mapping changes.
  Configurable keybinds (console) with sane defaults.
- Activation `IAI.SelectorUI <0|1>`, default OFF (dormant → existing gates byte-identical).

## Ambiguities — owner rulings (S1–S10)
- **S1 (RULED):** ship the **`=` exact-match sentinel** in v1. Leading `=` in `FindActorsMatching` → strip +
  `GetName().Equals(rest, IgnoreCase)`; substring path byte-identical with no `=`. The sanctioned helper touch
  (verify-item 5 pre-authorized). `InjectSelected()` passes `"=" + GetName()`. **Load-bearing beyond m5** — the
  future auto-injection path (same arbitrary-actor collision risk) reuses it. Streamed-sublevel duplicate-name
  ceiling (would need pointer identity = an `IAnomaly` change) is the accepted v1 limit — document, don't solve.
- **S7 (RULED):** keybinds Tab (next) / Shift+Tab (prev) / C (cycle) / G (inject) / H (revert), rebindable via
  `IAI.SelectorBind`. Document the Steam-overlay Shift+Tab collision (fine in PIE; rebind escapes it) — informed default.
- **S2–S6, S8–S10 (RATIFIED as recommended):** compose existing `AnomalyViewport` (no change to it); console-command
  wrappers for the bridge gate; refresh only-when-enabled + ~0.1 s throttle + on-demand before Advance/Prev;
  exact-name is the v1 identity ceiling; selector does NOT touch `SetViewportScoping`; fixed 4-anomaly set with default
  args; revert-by-`LastInjectedId`; debug-box highlight (dev-only).

## Three usability facts (documented in architecture.md + runbook §6a, per the ruling)
1. **One instance per id:** injecting the same anomaly id on a second object reverts-then-reapplies — only one object
   can carry a given anomaly type at a time; the first reappears (registry reality, G12-style).
2. **Cycle order is name-sorted (alphabetical)** in v1 (deterministic for the bridge gate). Spatial screen-X
   (left-to-right) ordering is the intended next UX polish.
3. **Keep `IAI.SetViewportScoping 0`** while using the selector — it is self-scoping; global scoping ON adds a
   redundant visibility re-test on inject that can drop a target occluded in the sub-second between select and inject.

## Source verification (UE 5.1, Release-5.1 @ `D:\UESource\UnrealEngine`) — the 5 items
1. **HUD draw (G25):** `UDebugDrawService::Register(const TCHAR* Name, FDebugDrawDelegate)` (`Debug/DebugDrawService.h:23`,
   Engine; delegate `(UCanvas*, APlayerController*)` `:16`), drawn by `UGameViewportClient::Draw` →
   `UDebugDrawService::Draw(EngineShowFlags, …)` (`GameViewportClient.cpp:1820`), gated per show flag
   (`DebugDrawService.cpp:84-111`). Register under **`Game`** (`ShowFlagsValues.inl:267`), forced ON for non-editor
   views (`ShowFlags.h:430` `SetGame(InitMode != ESFIM_Editor …)`). **Renders with no host HUD class.** Beats the
   AHUD-post-render path (needs host HUD class) and a Slate viewport widget (pulls Slate).
2. **Input poll (G26):** `APlayerController::IsInputKeyDown` (`PlayerController.h:1563`) / `WasInputKeyJustPressed`
   (`:1567`) → `PlayerInput->IsPressed/WasJustPressed` (`PlayerController.cpp:5530-5538`) → raw `KeyStateMap`
   (`PlayerInput.cpp:1807-1809` / `1691-1693`). **Mapping-independent.** `FKey`/`EKeys` in InputCore
   (`InputCoreTypes.cpp:32/68/124`). Local PC reached via `World->GetFirstPlayerController()` each tick.
3. **Minimal deps (G27):** immediate-mode HUD = `UDebugDrawService` + `UCanvas` + `FCanvasTextItem` + `DrawDebugBox`,
   all **Engine**. Only non-Engine type is `FKey`/`EKeys` (**InputCore**), already a *public* dep of Engine
   (`Engine.Build.cs`) — transitive, but declared explicitly for IWYU. **No Slate/SlateCore/UMG.** Net dep delta: +1.
4. **Custom-depth outline (report only, NOT v1):** would need the project Custom Depth-Stencil setting (`r.CustomDepth=3`)
   + a shipped post-process outline material (reads `CustomStencil`) + runtime `SetRenderCustomDepth`/
   `SetCustomDepthStencilValue` (`PrimitiveComponent.h:1758/1762`; fields `:595/:674`). A render-pipeline + content
   surface the `DrawDebugBox` highlight sidesteps. Deferred.
5. **Substring collision (G28):** `FindActorsMatching` is substring (`AnomalyTargeting.cpp:28-30`). Passing a bare
   `GetName()` over-matches numbered siblings (`Cube` ⊂ `Cube2`) — real for arbitrary actors (stock StackOBot
   `_UAID_…` names are effectively unique, so low risk on stock content). Fix = the S1 `=` sentinel in the single
   match chokepoint → all 4 object-scoped anomalies inherit exact targeting with no anomaly edits, no `IAnomaly` change.

## What was done
1. **New `Public/AnomalySelectorSubsystem.h` + `Private/AnomalySelectorSubsystem.cpp`** — the subsystem: lifecycle
   (Game+PIE, GetStatId), the explicit-core methods + readbacks, `SetUIEnabled` (idempotent; registers/unregisters the
   HUD delegate), `SetKeyBinding`, `LogStatus`; `PollInput` (raw-key thin shell), `DrawHUD` (immediate-mode HUD thin
   shell), `RefreshVisibleSet` (TActorIterator → `GetActiveViewInfo` → `FilterVisibleActors` → name-sort → reconcile),
   and the `IAI.Selector*` / `IAI.SelectorUI` / `IAI.SelectorBind` console command surface.
2. **`=` exact-match sentinel** in `AnomalyTargeting::FindActorsMatching` (param renamed `Substring`→`Query`; leading
   `=` → strip + `Equals(IgnoreCase)`; empty-after-`=` matches nothing). Header documents it. **The only leaf-helper
   touch**; substring behavior byte-identical with no `=`. `AnomalyViewport` and the anomalies are unchanged (they
   inherit exact mode via the chokepoint).
3. **`AnomalyInjector.Build.cs`** — added `InputCore` (IWYU; comment notes it's the first dep since M0, transitive via
   Engine, and that the HUD is immediate-mode so no Slate/UMG).
4. **`AnomalyInjector.uplugin`** — VersionName 0.5.0 → **0.6.0**.
5. **Docs** — architecture.md (Reflects blurb, deps/version, `=` sentinel note, new "Object Selector + Inject UI"
   section, control surface, game-agnostic invariant), CLAUDE.md (Current status / in-flight / milestones), gotchas
   **G25–G28**, runbook (§6a selector smoke + §7 gate rows), onboarding (file tree + control surface), this journal.

## Problem → Resolution
- **Most-vexing-parse on the key construction.** `const FKey Key(FName(*Args[1]));` parsed as a function declaration
  (`error C2664: cannot convert 'const FKey (*)(FName *[])'`). Resolution: a named intermediate —
  `const FName KeyName(*Args[1]); const FKey Key(KeyName);`. Clean compile after.

## Gates
- **Clean Development-Editor compile on 5.1 (exit 0)** — `[3/4] Link UnrealEditor-AnomalyInjector.dll`, total ~11 s.
- **Bridge state-gate (Simulate) — PENDING:** `IAI.SelectorUI 1` → repeated `IAI.Selector.Next` → `IAI.Selector.Status`
  asserts the selected name cycles the name-sorted visible set; `IAI.Selector.Cycle` cycles the 4 ids; `IAI.Selector.Inject`
  → selected actor's hidden flag / forced-LOD changed (and the `=` exact-match hit ONLY that actor, not a numbered
  sibling); `IAI.Selector.Revert` → restored. Deterministic in Simulate (view resolves, G23).
- **OFF-regression — PENDING:** `IAI.SelectorUI 0` → every M0–m4 gate identical (subsystem dormant); the `=` sentinel
  leaves substring gates (`SM_Ramp` → 2 ramps, `Bot`, `Foliage`) byte-identical.
- **Owner eyeball (real Play) — PENDING:** Tab cycles the box + lists through on-screen objects and skips occluded ones;
  choose anomaly + inject → selected object affected; revert restores. (Input + HUD are not bridge-driveable.)

## Follow-on (folded into the same uncommitted m5): renderable-target filter
Live eyeball surfaced a correctness gap: the selector's visible set included **non-renderable** actors
(`RuntimeVirtualTextureVolume`, `PlayerStart`, `GameplayDebuggerCategoryReplicator`, `LandscapeStreamingProxy`,
`RoomBuilderSquare`) — they carry primitives (collision/bounds boxes, capsules) that pass frustum+occlusion but draw
no useful geometry. Injecting on them = the unlabeled-but-invisible sample the viewport layer exists to prevent.

**Decision: "visible set" now means "renderable-visible set."** The filter lives in `AnomalyViewport` (one source of
truth — the selector AND future auto-injection consume the identical set). Rulings (all ratified as recommended):
- **R1** — predicate = `IsVisible()` AND a base-TYPE allowlist: `IsRenderableComponent(Comp)` =
  `Comp->IsVisible() && (IsA<UStaticMeshComponent>() || IsA<USkinnedMeshComponent>() || IsA<UFXSystemComponent>())`.
  Capability/type test, not a class blocklist (game-agnostic). Renderability runs FIRST (before occlusion traces — perf).
- **R2** — additive new entry points only (`IsRenderableComponent` / `IsActorRenderableVisible` /
  `FilterRenderableVisibleActors` / `GetVisibleRenderableActors`); the m4 visibility functions are **byte-identical** (the
  scoping-ON path + all prior gates keep their guarantees without re-proving).
- **R3** — `IsVisible()`, not `ShouldRender()` (which has a non-shipping hidden-collision branch — a determinism footgun).
- **R4** — HUD `LastInjectResult` line surfaces the AMB-2 zero-match (LOD anomaly on a pure-VFX actor) in real Play,
  not log-only.
- **R5** — landscape excluded (falls out of the allowlist); `ULandscapeComponent` is a documented one-line extension point.
- **R6** — `GetVisibleRenderableActors` returns **empty** on no view (offer nothing, never blind) — deliberately distinct
  from the finders' treat-as-unscoped. Two callers, two safe directions; do not reconcile.

**Source verification (5.1):** `UFXSystemComponent : UPrimitiveComponent` (ENGINE_API, `ParticleSystemComponent.h:355`)
is the common base of `UNiagaraComponent` (Niagara **plugin**, `NiagaraComponent.h:36`) + `UParticleSystemComponent`
(`:459`) ⇒ VFX caught with **no Niagara dep**. `URuntimeVirtualTextureComponent : USceneComponent`
(`RuntimeVirtualTextureComponent.h:17`) ⇒ never a primitive; the RVT volume's false positive is its `UBoxComponent`
"Bounds" (`RuntimeVirtualTextureVolume.cpp:18`), excluded by the allowlist. `USceneComponent::IsVisible()` folds in
`!bHiddenInGame` (`SceneComponent.cpp:3140-3149`); `ShouldRender()`'s collision-debug branch (`:3075-3104`) is the
footgun avoided by R3. **No dep change (deps stay Core/CoreUObject/Engine/InputCore), no `.uplugin` change (0.6.0),
no `IAnomaly`/injector/anomaly/m4-visibility change.**

**Files (folded in):** `AnomalyViewport.{h,cpp}` (renderable entry points), `AnomalySelectorSubsystem.{h,cpp}`
(`RefreshVisibleSet` → `GetVisibleRenderableActors`; `LastInjectResult` set in inject/revert + drawn in `DrawHUD`),
docs (architecture / gotcha **G29** / this journal / runbook). Clean Development-Editor compile on 5.1 (exit 0).

**Live gate refined the predicate (the live-enumeration was load-bearing, as required).** Enumerating the false-positives
in the running PIE world corrected two assumptions:
- `RuntimeVirtualTextureVolume` → only a `UBoxComponent` (`visible=false`); `GameplayDebuggerPlayerManager` → zero
  primitives; `PlayerStart` → no qualifying primitive — all **correctly excluded**.
- `LandscapeStreamingProxy` → terrain (`LandscapeComponent`) correctly excluded, but it leaked in via
  `GrassInstancedStaticMeshComponent` with **instance count = 0** (empty grass). **Ruling: add an empty-ISM guard** —
  `IsRenderableComponent` requires `GetInstanceCount() > 0` for instanced static meshes (HISM derives from ISM, caught by
  the same `Cast`). Capability refinement of R1, not a blocklist.
- `RoomBuilderSquare` → 4 ISMs with **19/4/25/195 = 243 real instances** → genuinely renders. **Ruling: intentionally
  retained** as a valid rendering target; the "no visible geometry" premise was mistaken. Excluding a genuinely-rendering
  actor would reintroduce the name-based special-casing the allowlist exists to avoid.
- **Corrected definition (record):** *renderable = a visible SM/SK/FX component that actually draws something
  (instanced ⇒ instance count > 0).*

**Forward note:** "visible set = renderable visible set" is **load-bearing beyond the selector** — it is the exact set
the future **auto-injection** path consumes (alongside the `=` exact-match apply-by-name primitive). Keep both clean.

**Combined gate (must all pass before the single m5 commit):**
- Live: `IAI.Selector.Status` excludes RVTVolume / PlayerStart / GameplayDebuggerCategoryReplicator /
  LandscapeStreamingProxy / RoomBuilderSquare, and **live-enumerate those actors' primitives to prove *why*** (they're
  UBoxComponent/capsule/etc., not the allowlisted types); includes Bot, ramps, pressure plates, doors, foliage (HISM).
  Deterministic particle target if present → assert included; else flag for eyeball.
- Zero-match: non-mesh/pure-VFX actor + `lod_corruption` → inject → HUD "0 matched" + log trail.
- Regression: all prior m5 gates + OFF-path green; `SM_Ramp`→2 / `Bot` / `Foliage` substring unaffected; `=` exact-match
  still hits exactly 1; m4 scoping-ON path unchanged.
- Clean compile on 5.1 (exit 0).

## State
- Clean compile on 5.1 (exit 0) **with the renderable filter folded in**. Catalog unchanged at 7. VersionName 0.6.0.
  Combined gate green over the bridge (MainWorld Simulate); owner real-Play eyeball green. **Committed `aa2a3a4`,
  tagged `m5`** (the renderable filter is part of that single commit, not a separate one). Acceptance recorded in a
  follow-up `docs:` commit. Editor + bridge left up at MainWorld.

## Hand-off
- **ACCEPTED** — combined gate green over the bridge (selection cycle, `=` exact-match inject hitting exactly the
  selected actor, renderable filter excluding RVTVolume/PlayerStart/GameplayDebugger/zero-instance-grass
  LandscapeStreamingProxy while keeping meshes+foliage+NiagaraActors+RoomBuilderSquare, zero-match HUD, OFF-regression
  `SM_Ramp`→2 / `=SM_Ramp2…`→1) + owner real-Play eyeball green. Committed **`aa2a3a4`**, tagged **`m5`**; acceptance
  recorded in a follow-up `docs:` commit. Editor + bridge left up at `MainWorld`.
- **Next consumer:** automatic injection (roadmap) reuses the same renderable-visible set
  (`AnomalyViewport::GetVisibleRenderableActors`) + apply-by-name (`=`) primitives — keep them clean. Selector screen-X
  ordering is the queued UX polish. Bridge/host stay unversioned (G8 unchanged).
