# AnomalyInjector — canonical context

Personal research project **"GDP: Anomaly Injection"** (intrusive UE5 track). This is a
UE5 plugin that injects **labeled visual anomalies** (graphics bugs — missing objects,
lighting mismatch, LOD corruption, flicker, etc.) into UE5 games, to generate synthetic
training data for bug-detection ML. It is **game-agnostic** (public UE APIs only) and is
tested on Stack O Bot. A separate non-intrusive tool exists elsewhere and is out of scope.

This file is the **canonical entry point**. The folder it lives in is its own git repository
and is the single source of truth for the project.

## Current status — keep this current; it is the cold-start "you are here"
- **Latest (in flight):** **Automatic Injection (m6) — CODE-COMPLETE, clean compile (exit 0); ALL BRIDGE GATES GREEN;
  owner real-Play eyeball PENDING; NOT yet committed/tagged (2026-06-19).** New **separate** `UAnomalyAutoInjectorSubsystem`
  (`Public/AnomalyAutoInjectorSubsystem.h` + `Private/AnomalyAutoInjectorSubsystem.cpp`, `UTickableWorldSubsystem`,
  Game+PIE) that auto-fires the **4** object-scoped anomalies **randomly on the renderable objects currently on-screen**
  (drawn from `AnomalyViewport::GetVisibleRenderableActors` + applied via the `=` exact-match token), each
  **auto-reverting** after a randomized hold. **Concurrent but collision-free by construction** (no coordinator) via two
  invariants: **(i)** one live fire per id (the registry's one-instance-per-id) + **(ii)** **one anomaly per actor**
  (`OVERRIDE-1` — subsumes both conflict groups *and* the hide-masks-LOD case, so **no id→group table**; supersedes the
  planning turn's per-group guard). All randomness from **one seeded `FRandomStream`** (console-settable seed, default
  time-based) on a **fixed draw protocol** independent of apply-result (R-SEED). **Explicit-core / thin-shell split (as
  m4/m5):** the deterministic core `AdvanceTime`/`TryFireOnce` is bridge-driveable as `IAI.Auto.Step`/`IAI.Auto.FireOnce`
  **without real time and without Enable/Run**; two thin shells drive it — the `IAI.Auto.*` console (bridge gate) +
  raw-input poll (keys `1-4`/`J`/`K`, distinct from the selector's) + a right-anchored immediate-mode HUD (eyeball).
  **Two switches, both default OFF → dormant → existing gates byte-identical:** `IAI.Auto.Enable <0|1>` (HUD/keys) and
  `IAI.Auto.Run <0|1>` (firing; forced OFF when !Enabled). Fires **auto-revert** after a randomized hold (R-LIFE;
  `IAI.Auto.Persist` flag, default off). **Self-scoping** — does NOT touch `IAI.SetViewportScoping` (warns if it is ON);
  no view → fire nothing (never blind). **Manual selector/console injection of a pool id during an auto run is
  unsupported → warn-not-block (R-COEXIST).** **No `IAnomaly`/injector/anomaly/leaf-helper change; no new dep**
  (`FRandomStream` = Core; deps stay `Core/CoreUObject/Engine/InputCore`); **catalog stays 7** (orchestration over the
  existing catalog). VersionName → **0.7.0**. **Clean Development-Editor compile on 5.1 (exit 0).** **Bridge state-gates
  GREEN (MainWorld Simulate):** deterministic headless fire + `=` exact-match (1 of 21 EnergyOrb siblings hit),
  auto-revert on hold-elapse, collision-free concurrent (3 distinct ids × 3 distinct actors, no 4th fire — invariants
  (i)+(ii)+cap), seed-reproducible target, OFF-regression byte-identical (`SM_Ramp`→2), both coexistence warnings fire
  without blocking. **Next:** owner real-Play eyeball → one `feat:` commit + `git tag m6`.
  → `docs/sessions/2026-06-19-010-auto-injection.md`.
- **Prior milestone (as-built):** **Object Selector + Inject UI (minimal) — COMPLETE (committed `aa2a3a4`, tagged `m5`) (2026-06-19).**
  A new **separate** `UAnomalySelectorSubsystem` (`Public/AnomalySelectorSubsystem.h` + `Private/AnomalySelectorSubsystem.cpp`,
  `UTickableWorldSubsystem`, Game+PIE only) that lets the player **select a visible on-screen object** (Tab-cycle over the
  **renderable-visible set** — frustum AND occlusion AND renders-to-screen) and **inject** one of the four object-scoped anomalies on it (default args), then
  revert — calling the existing injector's public `ApplyAnomaly`/`RevertAnomaly`. **Explicit-core / thin-shell split (as m4):**
  public methods `AdvanceSelection`/`SelectPrevious`/`CycleAnomalyChoice`/`InjectSelected`/`RevertSelected` + readbacks
  `GetSelectedActorName`/`GetVisibleActorNames`/`GetAnomalyChoice` are the bridge-gatable surface; two thin shells drive them —
  the `IAI.Selector.*` console commands (bridge gate) and per-tick **raw input polling** + an **immediate-mode HUD**
  (real-Play eyeball). Targeting is made exact by a new **`=` sentinel** in `AnomalyTargeting::FindActorsMatching`
  (leading `=` → `GetName().Equals(IgnoreCase)`; substring path **byte-identical** with no `=`); `InjectSelected` passes
  `"=" + GetName()` so it hits only the selected actor (the **only** leaf-helper change — additive; verify-item 5 pre-authorized).
  HUD = `UDebugDrawService::Register("Game", …)` (host-blind, no game HUD class — G25) drawing a visible-names list + an
  anomaly list + a `DrawDebugBox`/label on the selection; input = `WasInputKeyJustPressed`/`IsInputKeyDown` raw key state
  (no host mappings — G26); defaults Tab/Shift+Tab/C/G/H, rebindable via `IAI.SelectorBind`. Activation **`IAI.SelectorUI <0|1>`,
  default OFF → dormant → existing gates byte-identical.** **First dep since M0: `InputCore`** (FKey/EKeys; transitive via Engine,
  declared for IWYU) — **no Slate/UMG** (immediate-mode). **Renderable-target filter folded in** (m5 follow-on): the selector's
  visible set means **renderable-visible** — new additive `AnomalyViewport::IsRenderableComponent` (`IsVisible()` + a
  static/skeletal/`UFXSystemComponent` base-type allowlist; VFX caught with no Niagara dep) excludes volumes/spawn-points/
  debug/landscape (the m4 visibility funcs stay byte-identical); a HUD `LastInjectResult` line surfaces the AMB-2 zero-match;
  `GetVisibleRenderableActors` returns empty on no-view (offer nothing, never blind). This is the set **auto-injection** will
  consume (gotcha G29). **No `IAnomaly` change, injector subsystem + all 7 anomalies untouched;
  catalog stays 7.** VersionName → **0.6.0**. **Clean Development-Editor compile on 5.1 (exit 0).** Combined gate **green**
  over the bridge (MainWorld Simulate): selection cycles the name-sorted renderable-visible set; `=` exact-match inject
  hits exactly the selected actor (1 of 17 prefix-siblings); the renderable filter excludes RVTVolume / PlayerStart /
  GameplayDebugger / zero-instance-grass LandscapeStreamingProxy while keeping meshes + foliage + NiagaraActors +
  RoomBuilderSquare; zero-match (Niagara + `lod_corruption`) surfaced; OFF-regression byte-identical (`SM_Ramp`→2,
  `=SM_Ramp2…`→1). **Owner real-Play eyeball green — ACCEPTED.** → `docs/sessions/2026-06-19-009-selector-inject-ui.md`.
- **Prior milestone (as-built):** **Viewport-Visibility Layer — COMPLETE (committed `7c34275`, tagged `m4`) (2026-06-18).**
  New shared helper **`AnomalyViewport`** (`Public/AnomalyViewport.h` + `Private/AnomalyViewport.cpp`,
  AnomalyTargeting/Args/Lod convention) = "is this object visible to the player" via **frustum AND occlusion**
  over an explicit view spec `FAnomalyViewInfo` (deterministic, synthetic-view-gatable) + a thin live resolver
  `GetActiveViewInfo` (first local player's POV; treat-as-unscoped + warn on no view). Occlusion backend (AMB-V1)
  = **multi-sample camera-to-bounds line trace** (`ECC_Visibility`, center+8 corners), private behind the
  backend-agnostic API; `GetLastRenderTimeOnScreen()` is the documented live backend for the future
  capture/live-injection milestone (.cpp-only swap — G22). New opt-in toggle **`IAI.SetViewportScoping <0|1>`
  (default OFF)** + diagnostic **`IAI.TestVisibility`** (synthetic-gate driver). The **4** object-scoped
  primitive-backed anomalies (`missing_object`, `flicker`, `lod_corruption`, `lod_popping`) consult the toggle and
  route through `AnomalyViewport` only when ON; `lighting_mismatch` + the two globals are excluded by design.
  **No `IAnomaly` change, no new module dependency** (frustum/traces/camera = Engine, `FReversedZPerspectiveMatrix` =
  Core; both locks held). **Clean Development-Editor compile on 5.1 (exit 0)**; over the bridge (MainWorld Simulate):
  synthetic frustum gate (behind→out, far→in, in-cone→in — reversed-Z VP validated, G24), synthetic occlusion gate
  (controlled wall: blocked→0 / clear→1 at frustum=1), and **OFF-is-byte-identical regression** (`missing_object`
  + `lod_corruption` round-trips M-identical, ListAnomalies still 7) all **green**. Catalog unchanged at **7**.
  VersionName → **0.5.0**. → `docs/sessions/2026-06-18-008-viewport-visibility-layer.md`.
- **Prior as-built:** **Refactor — "GDP" prefix removed from the plugin — COMPLETE + COMMITTED `351c7e8` (2026-06-18).**
  Pure mechanical rename, **no behavior change**: module/plugin/folder/`Build.cs`/`.uplugin` `GDPAnomalyInjector`→`AnomalyInjector`;
  `UGDPAnomalyInjectorSubsystem`→`UAnomalyInjectorSubsystem`; `IGDPAnomaly`→`IAnomaly`; `FGDPAnomaly_*`→`FAnomaly_*`;
  API macro `GDPANOMALYINJECTOR_API`→`ANOMALYINJECTOR_API`; log category `LogGDPAnomaly`→`LogAnomaly`;
  helpers `GDPTargeting/GDPArgs/GDPLod`→`AnomalyTargeting/AnomalyArgs/AnomalyLod`; console commands `GDP.*`→`IAI.*`.
  Project identity **"GDP: Anomaly Injection"** retained (code-prefix strip only; copyright/`CreatedBy` unchanged). Clean
  Development-Editor compile on 5.1 (exit 0) + light bridge re-gate green (module loads under the new name, `IAI.ListAnomalies`
  lists the **7** sorted under `LogAnomaly`, `IAI.Apply/Revert missing_object SM_Ramp` round-trips). One `refactor:` commit, **no tag**;
  bridge/host unchanged (G21). → `docs/sessions/2026-06-18-007-rename-strip-gdp-prefix.md`.
- **Prior milestone:** **M3 — LOD breadth fill — COMPLETE (committed `c54351a`, tagged `m3`).**
  `lod_corruption` extended to **static OR skeletal** meshes (same ID — one "LOD corruption" category; mesh
  type is an implementation detail), new ticking **`lod_popping`** (flicker mechanics), and a new shared
  helper **`AnomalyLod`** (`Public/AnomalyLod.h`+`Private/AnomalyLod.cpp`) absorbing the static/skeletal forced-LOD
  dispatch (2 consumers). Registry lists **7** (sorted). **No `IAnomaly` change** (M1 lock held again)
  and **no new module dependency**. Clean Development-Editor compile on 5.1 (exit 0); all 9 state gates
  driven green over the bridge in a `MainWorld` Simulate session — incl. the static **regression**
  (M2-identical), the **heterogeneous** apply (`lod_corruption Bot` = 1 static + 2 skinned in one apply),
  `lod_popping` oscillation, re-apply no-leak, RevertAll, teardown. **The Bot is single-LOD → skeletal
  anomalies are state-validated, no Bot visual** (G20). VersionName → 0.4.0.
  → `docs/sessions/2026-06-13-006-m3-lod-breadth.md`.
- **Prior as-built:** **M2.5 (UE 5.1 port) + M2.6 (bridge sever) — COMPLETE (2026-06-10).** **UE 5.1 is now
  the canonical engine** (the two real target games are on 5.1). Host = `D:\IntrusiveAnomalies\StackOBot`
  (natively-5.1); source engine = 5.1 at `D:\UESource\UnrealEngine`. The six anomalies compile clean on 5.1
  with **zero plugin-source changes** (all 7 port watch-items unchanged; only host-target build constants
  changed — G17), and all **10** stage gates were re-driven **green over the MCP bridge** + owner-confirmed
  visuals (flicker blink, magenta movable sun, near-clip). The `unreal-mcpython` bridge was ported to 5.1 by
  **severing its `BehaviorTreeEditor` dependency** (G8) — costs only the 2 BT-authoring tools.
  → `docs/sessions/2026-06-10-005-m2.5-m2.6-5.1-port-bridge-sever.md`.
- **Earlier:** **M2 — Breadth Round 1 — COMPLETE (all 8 stage gates passed).**
  Adds two shared helpers — **A1** `AnomalyTargeting::FindComponentsMatching<T>` (component targeting) and
  **A3** `AnomalyArgs` (parse/clamp/warn) — and three anomalies: `lighting_mismatch` (component, ULightComponent),
  `lod_corruption` (component, UStaticMeshComponent, static-only), `camera_clipping` (global near-clip).
  Registry lists **6** (sorted). **No `IAnomaly` change was needed — the M1 lock held.** Clean headless
  compile + gates 2–7 verified live in PIE `MainWorld` (unreal-mcpython bridge + owner eyeball, 2026-06-09).
  → `docs/sessions/2026-06-09-004-m2-breadth-round-1.md`, `docs/architecture.md`.
- **Resolved (M3):** **AMB-1 → skinned LOD count via `USkinnedMeshComponent::GetNumLODs()`** (runtime
  render-data count — the analog of static `GetNumLODs()`; not the asset's authored `GetLODNum()`) — G19.
  **AMB-2 → single tagged capture record keyed to the common base `UMeshComponent`** + `Cast<>` dispatch in
  `AnomalyLod` (not two typed lists); this is what lets one apply span a heterogeneous static+skeletal set.
  **AMB-3 → `lod_popping` default 2 Hz, ceiling 30 Hz.** Supersedes G16's static-only scope.
- **Resolved (M2):** **AMB-M2-1 → defer A2/`AnomalyCvar`** — near-clip is a console *command* + the
  `GNearClippingPlane` global, not an `IConsoleVariable`, so `camera_clipping` is self-contained (no
  `RenderCore` dep); AnomalyCvar lands with its first real cvar consumer (G13). **AMB-M2-2 → static-only
  `lod_corruption`** was the M2 stopgap; **resolved in M3** (static + skeletal via `AnomalyLod`, G19). M2 ships 2 helpers (A1, A3).
- **Resolved (M1):** **AMB-3 → capture-baseline** — `time_dilation` Revert restores the pre-Apply value.
  Generalized in M2 to the **per-target/global state-capture convention** (see architecture.md). G11.
- **In flight:** **m6 (automatic injection)** — code-complete + compile-green + **all bridge gates GREEN**; **only the
  owner real-Play eyeball remains; commit + `git tag m6` after it.** **Next action:** owner presses Play in `MainWorld`,
  `IAI.Auto.Enable 1` → pick types (keys 1-4) → `IAI.Auto.Run 1` (J), confirms on-screen-only fires that auto-revert,
  one-per-actor, never blind (runbook §6b). After m6: the High-priority new bug types (born viewport-aware AND auto-injectable), then the
  Tier-2 runtime control server (ships-as-a-build control surface). Also still queued: the `flicker→blinking` rename +
  new `flickering` (handoff §2.3), region-darkening (§2.4), the selector's screen-X ordering polish. Bridge/host stay
  unversioned (G8 unchanged).
- Milestones: M0 (`…-001`), M1 (`…-003`), M2 (`…-004`), M2.5+M2.6 (`…-005`), M3 (`…-006`) fully passed
  + tagged; rename refactor (`…-007`) committed `351c7e8` (no tag); **Viewport-Visibility Layer (`…-008`) committed
  `7c34275`, tagged `m4`**; **Object Selector + Inject UI (`…-009`) committed `aa2a3a4`, tagged `m5`**;
  **Automatic Injection (`…-010`) code-complete + compile-green, gates/eyeball pending, uncommitted.**

## Documentation system — how these docs fit together (read in this order)
- **CLAUDE.md** (this file) — canonical context, environment, invariants, workflow rules, and the
  **Current status** above. Start here.
- **[docs/architecture.md](docs/architecture.md)** — **living** current-as-built design reference
  + the **anomaly catalog**. "The whole picture in one read." Describes only what is in the code
  *now*; forward plans live in the journals, never here.
- **[docs/onboarding.md](docs/onboarding.md)** — what this is, how the work is run, where things live.
- **[docs/setup-runbook.md](docs/setup-runbook.md)** — **living** recipe to build/run from scratch.
- **[docs/gotchas.md](docs/gotchas.md)** — **append-only** non-obvious lessons (G1, G2, …).
- **[docs/sessions/](docs/sessions/)** — one journal per session, `YYYY-MM-DD-NNN-slug.md`: the
  chronological record (Goal / What done / Problem→Resolution / Deviations / State / Hand-off) and
  the home for milestone **plans** and **design decisions** (including open/blocking ones).

## Environment
- Engine: **source-built UE 5.1** (Release-5.1) at `D:\UESource\UnrealEngine`, registered to the
  `.uproject`'s `EngineAssociation` GUID `{B34F356C-4AE7-256A-F0E1-318A632BB902}` under
  `HKCU\Software\Epic Games\Unreal Engine\Builds`. (Originally validated on source-built UE 5.4.4 — see
  the Engine support note in architecture.md. After any engine re-sync, **rebuild ShaderCompileWorker** — G18.)
- Host project: **StackOBot** at `D:\IntrusiveAnomalies\StackOBot` (natively-5.1; the old 5.4 host at
  `D:\Unreal Projects\StackOBot` is retired).
- Plugin in-tree at `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\` (its own git repo, `master`).
- Windows, MSVC. Build target: **StackOBotEditor / Development / Win64**. Host-target build constants:
  `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1` (G17).
- Functional smoke tests run in **PIE via the `unreal-mcpython` MCP bridge** (host tooling, NOT part of
  this repo — see gotcha G8; on 5.1 its `BehaviorTreeEditor` dependency is severed). State/log reads close
  the non-visual gates; the owner eyeballs visuals.

## Architecture (current as-built: M0 — full detail in docs/architecture.md)
- One **Runtime** module `AnomalyInjector`, `LoadingPhase = Default`, `EnabledByDefault: true`.
- Build.cs deps: `Core`, `CoreUObject`, `Engine` (later may add `Renderer`, `RenderCore`, `RHI`,
  `Slate`, `InputCore`).
- Core injector = a `UTickableWorldSubsystem` (`UAnomalyInjectorSubsystem`) — auto-ticks,
  world-scoped, gives `GetWorld()`. Restricted to **Game + PIE** worlds via `DoesSupportWorldType`
  (never the editor preview world).
- Control surface = console commands via `FAutoConsoleCommandWithWorldAndArgs`, module-scoped,
  resolving the subsystem from the world the console passes in, null-guarded.
- M0 anomaly = ONE hardcoded hide (`IAI.HideActor` / `IAI.ShowAllActors`). The general anomaly
  **interface + registry is the M1 design** (see Current status + journal 002), not yet in code.

## Invariants (do not violate)
- **Plugin stays game-agnostic.** The `AnomalyInjector` module may depend only on
  `Core`/`CoreUObject`/`Engine` (later `Renderer`/`RenderCore`/`RHI`/`Slate`/`InputCore`)
  and must **never `#include` or reference host game-module types** (e.g. anything from the
  `StackOBot` module). Host-specific buildability lives in the project, never in the plugin.
- **Matching is label-free.** Targeting matches by actor Name or Class only.
  `GetActorLabel()` is editor-only and absent in cooked builds — `ListActors` may print the
  label (guarded by `WITH_EDITOR`) but nothing matches on it.

## Workflow & doc-maintenance rules
- **Two-Claude split.** Design decisions come from an orchestrating "chat Claude" and are
  ferried by Kavin (project owner). The implementing Claude implements. Genuine design forks
  or ambiguities are surfaced back (listed standalone), not improvised.
- **Plan-before-code.** A new milestone's first response is a file-by-file plan only; no
  implementation until approved.
- **Commits — Conventional Commits.** Prefixes: `feat:` (new anomaly or capability), `fix:` (bug),
  `docs:` (doc-only), `refactor:` (no behavior change), `chore:` (build/tooling). Scope anomaly-specific
  changes, e.g. `feat(flicker): …`. **Tag each milestone** with `git tag m<N>` after its commit so
  milestones diff cleanly (`m1..m2`, and a changelog can be auto-derived later). The git repo is the
  plugin folder (`master`); host scaffolding lives outside it and is not committed here.
- **Doc discipline — leave the docs able to (a) cold-start a fresh session and (b) explain the
  whole plugin to any UE dev.** When you start or advance a milestone you MUST, before the session
  closes:
  1. Update **Current status** (above) — the single "you are here" marker (latest as-built /
     in flight / open decisions / next action).
  2. Update **docs/architecture.md** to match the new as-built state, including the **anomaly
     catalog** — describe current code only, never aspirational.
  3. Write/append the **session journal** under `docs/sessions/` (history + the milestone plan +
     design decisions, including any open/blocking decisions).
  4. **Append** new lessons to `docs/gotchas.md` (never delete; supersede).
  5. Keep `docs/setup-runbook.md` and `docs/onboarding.md` current with the build/run steps and
     the control surface as they actually are.
  - Division of labor: **architecture.md = current state** ("what it is"); **journals = history +
    plans** ("how we got here / where we're going"); **runbook = repro**; **gotchas = lessons**.
