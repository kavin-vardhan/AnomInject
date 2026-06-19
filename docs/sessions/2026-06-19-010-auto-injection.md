# Session 010 — Automatic Injection (m6) (2026-06-19)

## Goal
Stand up **automatic injection**: at gameplay start a UI selects which anomaly types are enabled; while
running, those anomalies fire **randomly on whatever renderable objects are on-screen at that moment**,
then auto-revert after a randomized hold. Live-autonomous — distinct from the deferred capture/replay
pipeline. Built on m5 (`aa2a3a4`): it consumes the two proven primitives `AnomalyViewport::
GetVisibleRenderableActors` (the renderable-visible set) and the `=` exact-match apply-by-name. New
**separate** subsystem; the injector, `IAnomaly`, and all 7 anomalies stay **untouched**.

## Locked design (from the scoping turn — chat-Claude rulings R-UI / R-CONC / R-LIFE / R-SEED / R-POOL / R-CAD / R-COEXIST, all approved, + the owner's green-light adjustments)
- **R-UI:** new `UAnomalyAutoInjectorSubsystem : UTickableWorldSubsystem` (Game+PIE), reusing m5's
  techniques — `UDebugDrawService` immediate-mode HUD (G25) + raw-input poll (G26) + `IAI.Auto.*` console
  + the explicit-core / thin-shell split. A SECOND independently-toggleable shell over the same injector
  core; calls only the injector's public `ApplyAnomaly`/`RevertAnomaly`. **No `IAnomaly`/injector/anomaly
  change** (held — nothing forced one).
- **R-CONC + OVERRIDE-1 (the one design change):** v1 is **concurrent but collision-free BY
  CONSTRUCTION** (no ref-count coordinator), via two scheduler invariants:
  - **(i) one live fire per id** — the injector registry holds one instance per id (re-Apply reverts-
    then-reapplies), so the scheduler never re-fires a still-live id (clean revert accounting + the
    natural concurrency ceiling: max live ≤ distinct enabled-id count).
  - **(ii) ONE ANOMALY PER ACTOR** — `Candidates = V − {actors hosting ANY live fire}`. This single
    invariant **subsumes both conflict groups** (bHidden: `missing_object`/`flicker`; forced-LOD:
    `lod_corruption`/`lod_popping`) AND the hide-masks-LOD case (a hide hiding a LOD change = an
    invisible/mislabeled sample). All 6 cross-pairs among the 4 are either same-resource or
    visibility-masks-LOD, so one-per-actor is strictly simpler than the per-group guard and needs **no
    id→group table** (the group concept + selective per-group same-actor stacking move to the deferred
    compound-anomaly milestone). *Supersedes the planning turn's per-group guard.*
- **R-LIFE:** fires auto-revert after a randomized hold, then free their slot. `persist` is a config flag
  (default off).
- **R-SEED:** one `FRandomStream`, seeded once per run (console-settable seed; default time-based via
  `FPlatformTime::Cycles`). Drives interval, id, target, hold. **Honest limit:** the seed reproduces the
  CHOICES given the same sequence of visible sets (and Step granularity); full run reproducibility with
  fixed visible sets is a capture/replay concern, not v1.
- **R-POOL:** v1 pool = the 4 object-scoped anomalies only (`missing_object`, `flicker`, `lod_corruption`,
  `lod_popping`). Globals + `lighting_mismatch` are a future non-object track (documented extension point).
- **R-CAD:** randomized inter-fire interval [Min,Max], a MaxConcurrent cap, randomized per-fire hold
  [Min,Max] — all console-settable, all from the one stream. **Self-scoping:** targets drawn from
  `GetVisibleRenderableActors` directly; does NOT use `IAI.SetViewportScoping` (keeping it ON would make
  the `=` apply redundantly re-test visibility and could drop a target — m5 fact #3). No view → fire
  nothing this window (never inject blind; reuse the empty-on-no-view contract, R6/G29).
- **R-COEXIST:** manual selector/console injection of a pool id during an auto run is **unsupported**
  (clobbers via the registry's one-instance-per-id; the auto-injector tracks only its own fires) →
  **warn, not block**.

## Owner rulings on the four open decisions
- **(a) Two switches** — `Enable` = eyeball shell (HUD + keypoll) dormancy; `Run` = auto-tick firing
  (Tick→AdvanceTime); Run forced OFF when !Enabled.
- **(b) Conflict-skip = SKIP THE TICK** (honest determinism; redraw-different-id stays deferred). Under
  OVERRIDE-1: "Candidates empty (all visible actors already host a fire) → skip."
- **(c) Defaults** — keys `1`/`2`/`3`/`4` toggle the four types, `J` start/stop, `K` reseed (distinct from
  the selector's Tab/C/G/H); cadence interval [4,9]s, hold [3,6]s, MaxConcurrent 4. All console-settable.
- **(d) Warn-not-block** on both coexistence guards (selector+auto both enabled; `SetViewportScoping` ON
  at Run-start — warn, do NOT force it off).

## Three separated states (preserves the m4 "explicit core is fully bridge-gatable" principle)
- **Enable** (`IAI.Auto.Enable`): eyeball shell only — registers the HUD, polls keys. Default OFF →
  **dormant** (Tick early-returns, no delegate) → every existing M0–m5 gate byte-identical.
- **Run** (`IAI.Auto.Run`): auto-tick auto-feed — Tick → `AdvanceTime(DeltaTime)`. Forced OFF when !Enabled.
- **Step / FireOnce** (`IAI.Auto.Step <sec>` / `IAI.Auto.FireOnce`): **direct manual core drive** — work
  regardless of Enable/Run (given a configured enable-set + seed). This is how the bridge gates the
  scheduler deterministically without real time.

## Locked draw protocol (R-SEED, load-bearing — documented inline in `TryFireOnce`)
Stream draws happen on a FIXED schedule independent of `ApplyAnomaly`'s result, so a zero-match never
shifts stream position:
- Skip-paths consume **ZERO** draws: MaxConcurrent hit, empty Eligible, empty V.
- The **Candidates-empty** skip consumes **exactly the Id draw** (it happens after the Id draw).
- A real attempt draws **Id, Target, Hold** (in that order), THEN applies and registers **on success
  only** (a zero-match — e.g. an LOD id drawn onto a pure-VFX actor legitimately in V — is surfaced, not
  registered; the draws are already spent).
- `AdvanceTime`: an **interval draw** is armed at run-start and after each fire window (tied to the
  window, not to success). Frame-semantics: ≤ 1 fire window per call (drive multi-fire timing with
  repeated `Step` calls). On `SetRunning(true)`: reseed, then `FireTimer = FRandRange(IntervalMin,
  IntervalMax)` (the first-interval option, NOT 0 — uniform protocol + calmer start).

## Source verification (UE 5.1, Release-5.1) — the R-CONC checks
- **(a) Registry is one-live-instance-per-id ✅.** `TMap<FName, TUniquePtr<IAnomaly>> Anomalies`
  (`AnomalyInjectorSubsystem.h:94`), one entry per id added in `Initialize` (`.cpp:40-52`). `ApplyAnomaly`
  calls the single instance's `Apply` (`.cpp:250-263`); revert-then-reapply is inside each anomaly's Apply
  (`Anomaly_MissingObject.cpp:24-27`, `Anomaly_LodCorruption.cpp:25-28`, `Anomaly_Flicker.cpp:24-27`,
  `Anomaly_LodPopping.cpp:26-29`). ⇒ (i) is real, not assumed.
- **Conflict-group resources ✅.** `{missing_object, flicker}` both `SetActorHiddenInGame` (bHidden);
  `{lod_corruption, lod_popping}` both `AnomalyLod::SetForcedLod` (forced-LOD). Confirms the OVERRIDE-1
  rationale (one-per-actor subsumes both + hide-masks-LOD).
- **(b) `GetUsage()` does NOT expose the mutated resource** — it returns a human hint string
  (`"<name-substring> [hz]"` etc.; `IAnomaly.h:36`). So a group table could not have read it anyway — and
  under OVERRIDE-1 **no group table is needed at all** (moot for v1; if selective per-group stacking is
  built later, use a tiny internal id→group table, never parse `GetUsage()`). See G31.
- **M5 HUD/input coexistence ✅.** `UDebugDrawService::Register("Game", …)` returns an independent
  `FDelegateHandle`; a second subsystem's delegate is additive (both fire). Raw key polling reads shared
  read-only state off the same PC. The injector+selector already coexist; a third subsystem is the same
  pattern. Distinct keys + a right-column HUD anchor avoid double-reaction / overlap.

## What was done
1. **New `Public/AnomalyAutoInjectorSubsystem.h` + `Private/AnomalyAutoInjectorSubsystem.cpp`** — the
   subsystem: lifecycle (Game+PIE, GetStatId, Deinitialize = UnregisterHUD + clear, **no inject calls** on
   teardown — the injector restores on its own Deinitialize); the explicit core `AdvanceTime` (service
   reverts one pass + ≤1 timed fire window) and `TryFireOnce` (the locked draw protocol + invariants (i)
   one-per-id, (ii) one-per-actor, the MaxConcurrent cap, the no-blind-fire rule, the `=` exact-match
   apply); the enable-set + cadence config; readbacks (`GetEnabledIds`/`GetLiveFireSummaries`/
   `GetLiveFireCount`/`GetSeed`/`LogStatus`); the two thin shells — `PollInput` (raw keys → toggle pool /
   run / reseed) + `DrawHUD` (right-column immediate-mode HUD: types checklist, run/seed/cadence, live
   fires, last-result) — and the `IAI.Auto.*` console surface (Enable/Run/Seed/Pool/Interval/Hold/
   MaxConcurrent/Persist/Step/FireOnce/Status/Bind). `WarnOnCoexistence` (selector-on / scoping-on, warn
   only).
2. **No leaf-helper / anomaly / injector / `IAnomaly` change.** The auto-injector composes existing public
   surface only.
3. **`AnomalyInjector.Build.cs` — unchanged.** Deps stay `Core/CoreUObject/Engine/InputCore`
   (`FRandomStream` = Core; HUD types = Engine; `FKey`/`EKeys` = InputCore). Net dep delta: 0.
4. **`AnomalyInjector.uplugin`** — VersionName 0.6.0 → **0.7.0**.
5. **Docs** — architecture (new Automatic Injection section, control surface, deps/version, limitations),
   gotchas **G30** (one-per-actor collision-free model) + **G31** (GetUsage is a hint, no group table in
   v1) + supersede-note on **G12**, runbook (§6b smoke + gate rows), CLAUDE.md (Current status / in-flight
   / milestones), onboarding (file tree + control surface), this journal, and the handoff §4/§5 marked
   resolved.

## Problem → Resolution
- **`static_assert` referenced a private member** — `static_assert(GNumAutoPool == UAnomalyAutoInjectorSubsystem::NumPoolKeys)`
  in the .cpp couldn't see the then-private `NumPoolKeys` (C2248). Resolution: moved `NumPoolKeys` to the
  class's public section (a harmless size constant). Clean compile after.
- **Live Coding blocked the rebuild** — a brand-new `UCLASS` subsystem cannot be hot-added via Live Coding
  (it needs a real rebuild + editor restart to register the type), so the editor had to be closed for the
  build. (Owner closed it; the lingering process was force-stopped; build then ran clean.)

## Gates
- **Clean Development-Editor compile on 5.1 (exit 0)** — `[3/4] Link UnrealEditor-AnomalyInjector.dll`,
  UHT 0 warnings, ~9 s total.
- **Bridge state-gates (MainWorld Simulate, view resolves G23) — GREEN (driven via the `unreal-mcpython`
  bridge, 2026-06-19):**
  - **Sanity:** `AutoInjector subsystem initialized ... (Enable OFF)`; `IAI.ListAnomalies` = **7**;
    `IAI.Auto.Status` baseline = `enable OFF / run OFF`, interval [4,9]s, hold [3,6]s, maxConcurrent 4, all 4
    pool ids enabled, 0 live fires, default time-based seed.
  - **Deterministic fire + exact-match (headless, no Enable/Run):** `Seed 1234; Pool all 0; Pool missing_object 1;
    FireOnce` → `Auto.Fire: 'missing_object' on 'BP_EnergyOrb_C_12' -> applied`; of **21** EnergyOrb siblings,
    **only** `BP_EnergyOrb_C_12` was `hidden==True` (zero siblings) — the `=` exact-match hit exactly the target,
    and the core drove with no Enable/Run.
  - **Auto-revert (R-LIFE):** `Pool all 0; Step 7` → `Auto.Revert: 'missing_object' on 'BP_EnergyOrb_C_12'
    (hold elapsed)`; orb `hidden==False`; slot freed (no re-fire — pool empty).
  - **Collision-free concurrent (i + ii + cap):** `Seed 777`, enable `{missing_object, flicker, lod_corruption}`,
    repeated `FireOnce` → 3 fires on **3 distinct ids × 3 distinct actors** (`lod_corruption`→BP_Door,
    `missing_object`→StaticMeshActor, `flicker`→1M_Cube_Chamfer6); further `FireOnce` produced **no fire** (all
    3 ids live → eligible empty); `Status` = `--- 3 live fire(s) ---`. One-per-id + one-per-actor + the
    enabled-id-count ceiling all hold.
  - **Seed reproducibility:** `Seed 4242; Pool missing_object 1; FireOnce` twice (revert between) → identical
    target `StaticMeshActor_…1813654254` both runs (fixed camera ⇒ fixed visible set; R-SEED).
  - **OFF-regression:** auto-injector dormant (Enable OFF) → `IAI.Apply missing_object SM_Ramp` hid exactly the
    **2** ramps (byte-identical to M1/m4/m5); **no stray hidden renderables / no stuck forced-LODs** from the
    prior auto gates (the Step-with-empty-pool cleanup reverted everything).
  - **Coexistence-warn:** `SelectorUI 1` then `Auto.Enable 1` → `Warning: ... BOTH enabled — UNSUPPORTED ...`;
    `SetViewportScoping 1` then `Auto.Run 1` → `Warning: IAI.SetViewportScoping is ON; ... REDUNDANTLY re-test
    ...`. **Neither blocked** (commands proceeded).
  - **Teardown clean-up:** `Auto.Run 0 / Enable 0 / SelectorUI 0 / SetViewportScoping 0 / RevertAll` → world
    pristine (0 hidden renderables, 0 forced-LODs); Simulate ended.
- **Not bridge-driven (covered by code-identity + owner eyeball):** *no-blind-fire* (empty visible set →
  fire nothing) is the `GetVisibleRenderableActors` empty-on-no-view contract already gated in m5 (R6/G29),
  hard to force under a fixed Simulate camera; *zero-match* (LOD id on a pure-VFX actor → "0 matched", no slot
  leak) is the identical `ApplyAnomaly`-returns-false path already gated by m5's selector zero-match row.
- **Owner eyeball (real Play) — PENDING (the only gate left):** Enable shows the types checklist; pick a subset
  + Run → anomalies fire only on on-screen renderable objects, auto-revert after their hold, never two on one
  actor, never fire with nothing visible.

## State
- Clean compile on 5.1 (exit 0); **all bridge state-gates GREEN** (deterministic fire + exact-match,
  auto-revert, collision-free concurrent, seed-repro, OFF-regression, coexistence-warn). Catalog unchanged at
  **7** (orchestration over the existing catalog — no new anomaly). VersionName **0.7.0**. Deps unchanged
  (`Core/CoreUObject/Engine/InputCore`). `IAnomaly`/injector/anomalies/leaf-helpers untouched. Editor left up
  at a clean MainWorld (Simulate ended; world not modified).
- **NOT yet committed/tagged** — per the green-light, tag `m6` only after **all gates green + owner eyeball**;
  the owner real-Play eyeball is the one remaining gate.

## Hand-off
- **ACCEPTED** — owner real-Play eyeball green (anomalies fire only on on-screen renderable objects,
  auto-revert after their hold, one-per-actor, never blind) on top of all bridge state-gates green. Committed
  **`41ba104`**, tagged **`m6`** (one `feat:` commit: the 2 new source files + `.uplugin` 0.7.0 + the 4 doc
  updates + this journal + the resolved `auto-injection-handoff.md`). Acceptance recorded in a follow-up
  `docs:` commit (m5 precedent; m6 stays pointed at the `feat:` commit — no retag). Bridge/host stay
  unversioned (G8).
- **Next milestone after m6:** the High-priority new bug types (born viewport-aware AND auto-injectable),
  then the Tier-2 runtime control server (the ships-as-a-build control surface). The deferred compound/
  stacked-anomaly path (ref-count "hidden-by" coordinator + per-(id,target) registry keying, G12) is only
  needed when we deliberately want compound same-actor anomalies. See the post-m6 session-close handoff doc.
- **Next milestone after m6:** the High-priority new bug types (born viewport-aware AND auto-injectable),
  then the Tier-2 runtime control server (the ships-as-a-build control surface). The deferred compound/
  stacked-anomaly path (ref-count "hidden-by" coordinator + per-(id,target) registry keying, G12) is only
  needed when we deliberately want compound same-actor anomalies.
