# Anomaly Injector — Handoff: m6 (Automatic Injection) shipped → new visual bugs

**Purpose.** Bridge document for a cold reader (a fresh chat, Claude Code, or a collaborator) covering what shipped in **m6 (automatic injection)** and the decisions/sequencing carrying into the next milestone (**new high-priority visual bugs**). Read alongside `CLAUDE.md`, `architecture.md`, `gotchas.md`, the prior `auto-injection-handoff.md` (its open questions are now **resolved** — see §7), and session journals `008`/`009`/`010`. This doc + project memory are the continuity artifacts — the local repo (journals, gotchas, architecture) is invisible to a fresh chat.

---

## 1. Current state — "you are here"

**What this is.** `AnomalyInjector` — a game-agnostic UE **5.1** plugin that injects labeled visual anomalies (graphics bugs) into a running game to produce synthetic ML training data. Public UE APIs only; tested on StackOBot. Project identity "GDP: Anomaly Injection"; the GDP *code* prefix was stripped (session 007).

**Repo / engine.** Plugin is its own git repo at `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\`. Source-built UE 5.1 at `D:\UESource\UnrealEngine`. Both real target games run 5.1. Bridge + host scaffolding remain **unversioned** (host tooling, G8).

**Git state.** Branch `master`; latest milestone **m6**. Tags: `m1 → m2 → m2.5 → m3 → m4 → m5 → m6`. The m6 commit is **`41ba104`** `feat(auto-injection): … (m6)` (prior HEAD was `f342272`); acceptance + this handoff land in a follow-up `docs:` commit (m6 stays pointed at the `feat:` commit — no retag).

**Shape.** **7 anomalies (unchanged)**, `IAnomaly` interface **unchanged since M1**, deps **`Core / CoreUObject / Engine / InputCore` (unchanged)**, VersionName **0.7.0**. Three `UTickableWorldSubsystem`s (Game+PIE): the injector, the M5 selector, and the new m6 auto-injector. Control = `IAI.*` console **+** the in-game selector (Tab-cycle) **+** the in-game auto-injector. Verification = bridge state-gates in a MainWorld **Simulate** session + owner eyeball in real **Play**.

**Anomaly catalog (7):** `missing_object`, `flicker`, `time_dilation`, `lighting_mismatch`, `lod_corruption` (static+skeletal), `camera_clipping`, `lod_popping`.

**The core injection loop is now complete** — manual (M5 selector) + automatic (m6). This was the reason auto-injection was sequenced ahead of new bug types: every future bug type is now born both **viewport-aware** AND **auto-injectable** instead of retrofitted.

---

## 2. What shipped in m6 — Automatic Injection

New **`UAnomalyAutoInjectorSubsystem`** (`Public/AnomalyAutoInjectorSubsystem.h` + `Private/AnomalyAutoInjectorSubsystem.cpp`) — the headless cousin of the M5 selector. Mirrors the selector's pattern exactly: a separate `UTickableWorldSubsystem` (Game+PIE), explicit-core / thin-shell split, calls only the injector's **public** `ApplyAnomaly`/`RevertAnomaly`. `IAnomaly`, the injector, the anomalies, and the leaf helpers are **untouched**. No new dep (`FRandomStream` = Core). Catalog stays 7. VersionName 0.6.0 → **0.7.0**.

**Behaviour.** At gameplay start a UI selects which of the 4 object-scoped anomalies are enabled; during play they fire **randomly on whatever renderable objects are on-screen at that moment** and auto-revert after a randomized hold. Live-autonomous — **distinct** from the deferred capture/replay/labeling pipeline.

**Builds on the two m5 primitives (both proven, kept byte-clean):** `AnomalyViewport::GetVisibleRenderableActors(World)` (renderable-visible set; empty on no view) + the `=` exact-match apply-by-name (`"=" + Actor->GetName()` through the `AnomalyTargeting::FindActorsMatching` chokepoint, G28).

**Control surface.**
- Console (bridge-gatable): `IAI.Auto.Enable <0|1>`, `Run <0|1>`, `Seed <int>`, `Pool <id> <0|1>` (Enable/Disable a pool id), `Interval <min> <max>`, `Hold <min> <max>`, `MaxConcurrent <n>`, `Persist <0|1>`, `Step <seconds>`, `FireOnce`, `Status`, `Bind <action> <key>`.
- In-game (eyeball): immediate-mode `UDebugDrawService` HUD anchored away from the selector (enable-set checklist, run state + seed, live-fire list with seconds remaining, last-result line) + raw-input keybinds (default **1–4** toggle the four pool ids, **J** start/stop run, **K** reseed — all rebindable).
- **Two switches:** `Enable` = HUD + keypoll (dormancy); `Run` = whether the scheduler actually fires (forced OFF when `!Enable`). `Step`/`FireOnce` drive the deterministic core **directly**, regardless of Enable/Run.

**Cadence defaults** (all console-settable; tuned for clear eyeballing — **tighten for dataset density later**): interval **[4, 9] s**, hold **[3, 6] s**, `MaxConcurrent` **4**.

---

## 3. Decisions made this session — rationale (the load-bearing part)

These are *why*, not just *what*, so the next session doesn't relitigate.

- **OVERRIDE-1 — scheduler guard = ONE-ANOMALY-PER-ACTOR (not per-conflict-group).** The original plan guarded only same-*conflict-group* on one actor (`{missing_object, flicker}` share `bHidden`; `{lod_corruption, lod_popping}` share forced-LOD). That was upgraded **before coding** to: at most one live fire per actor, period (`Candidates = V − {actors hosting ANY live fire}`). **Why:** the per-group guard would permit a hide-group anomaly + a LOD-group anomaly on the *same* actor — but the hide makes the object vanish, so the LOD becomes an **invisible, mislabeled sample**, the exact failure the viewport layer exists to prevent. And there is **no clean two-bug same-actor stack among the 4**: every cross-pair is either same-resource (`bHidden×bHidden`, `forcedLOD×forcedLOD`) or visibility-masks-LOD. One-per-actor subsumes **both** conflict groups (G12 `bHidden` last-writer-wins + the forced-LOD collision) **and** the hide-masks-LOD problem in a single invariant, and is **strictly simpler** — it let the `id→conflict-group` table be **dropped from v1** entirely.
- **Concurrency = collision-free BY CONSTRUCTION, no ref-count coordinator — confirmed, not assumed.** Code source-verified that the registry holds **one live instance per id** and re-Apply **reverts-then-reapplies** (`AnomalyInjectorSubsystem.cpp:250-263` + each anomaly's `if(bActive){Revert();}`). So invariant **(i)** — *don't re-fire a live id until it auto-reverts* — is **mandatory** for clean revert accounting (re-firing a live id un-targets its previous actor). Max concurrent is naturally bounded by the **distinct enabled-id count** (= 4 ceiling). The G12 ref-count **"hidden-by" coordinator stays DEFERRED** — it is only needed when we *deliberately* want stacked/compound anomalies on one actor (richer co-occurrence data); v1 sidesteps the need.
- **Determinism draw protocol (load-bearing).** All stream draws (Id, Target, Hold, interval) occur on a **fixed schedule independent of `ApplyAnomaly`'s result** — a zero-match (`false` return) must **not** shift stream position (draw Id/Target/Hold → then apply → register on success only). Skip-paths consume a **fixed** number of draws: `MaxConcurrent`-hit / empty-Eligible / empty-V = **zero** draws; the conflict (Candidates-empty) skip consumes **exactly the Id draw** (it happens after the Id draw). **Seed honesty:** the seed reproduces the *sequence of choices* **given the same sequence of visible sets** — over the bridge (`Step` + fixed camera ⇒ fixed V) it is fully reproducible; in real Play, frame timing and live visible-set contents vary, so **full run reproducibility is a capture/replay-pipeline concern, not v1**. The fixed per-attempt draw protocol is what keeps the stream position a deterministic function of the fire sequence. (Seeded via `FRandomStream`, **not** global `FMath::Rand`.)
- **First-fire timing.** On `SetRunning(true)`: seed the stream, then `FireTimer = Stream.FRandRange(IntervalMin, IntervalMax)` (the *first-interval* option, not `FireTimer=0`) — calmer start + a uniform draw protocol (an interval draw precedes every fire window, including the first).
- **Coexistence = WARN, not block.** Two guards, both shell-level: (1) selector + auto both enabled → warn; (2) `SetViewportScoping` ON at Run-start → warn (do **not** force it off — that would surprise a manual user; the auto-injector is already self-scoping via `GetVisibleRenderableActors`, so the M5-fact-#3 redundant-refilter risk is what the warning covers). **Known limit (documented, not solved):** the auto-injector can only track its **own** fires. A manual selector/console inject of a pool id **during an auto run** silently clobbers via the registry's revert-then-reapply and desyncs the auto revert deadline. Detecting it would need a per-id *active* readback on the injector = **forbidden by R-UI** (would touch the injector). So **"manual console/selector injection of a pool id during an auto run is unsupported."**
- **Pool = the 4 object-scoped only** (`missing_object`, `flicker`, `lod_corruption`, `lod_popping`). They are the ones that consume the renderable-visible set + apply-by-name. Globals (`time_dilation`, `camera_clipping`) mutate global state (no on-screen target); `lighting_mismatch` is light-scoped (not in the SM/SK/FX visible set). A separate **non-object "global track"** is a documented future extension, not v1.
- **`GetUsage()` is a human hint, not machine-readable resource identity (G31).** It returns argument-usage strings (e.g. `flicker → "<substring> [hz]"`), so no conflict-group table could read it anyway — and under OVERRIDE-1 none is needed. **If** selective per-group same-actor stacking is built later (deferred), use a tiny internal `id→group` table — **never parse `GetUsage()`**.

---

## 4. Verification / gates (m6)

- **Clean compile, exit 0** (UHT 0 warnings).
- **All bridge state-gates GREEN** (MainWorld Simulate, headless over the bridge): deterministic headless fire + `=` exact-match (1 of 21 `BP_EnergyOrb` siblings hit, not the lookalikes); auto-revert on hold-elapse (orb un-hidden); collision-free concurrent (3 distinct ids × 3 distinct actors, no illegal 4th — invariants i + ii + the enabled-id ceiling); seed reproducibility (same seed → identical target); **OFF-regression** (`missing_object SM_Ramp` → exactly 2 ramps, zero stray state); coexistence-warn (both warnings fire, neither blocks). World left pristine; Simulate ended.
- **Not bridge-forced (honest):** no-blind-fire and zero-match are the same `GetVisibleRenderableActors`-empty and `ApplyAnomaly`-false paths already gated in m5 — hard to force under a fixed Simulate camera, so they fold into the owner eyeball + code-identity.
- **Owner real-Play eyeball: GREEN** — anomalies fire only on on-screen renderable objects, auto-revert after their hold, never two on one actor, never fire with nothing on screen.

---

## 5. Forward plan / sequencing (survives a cold read)

1. **m6 DONE.** Next per the roadmap = **HIGH-priority new visual bugs**, built viewport-aware AND auto-injectable from birth (both primitives + the auto-injector now exist, so a new bug type plugs into manual *and* auto with no retrofit):
   - **Easy tier:** corrupted textures + object clipping (into geometry).
   - **Render tier:** screen tearing + framerate bugs.
2. **Animation bugs.**
3. **Tier-2 runtime in-build control server** — the eventual control surface. **Firm architectural fact:** the whole system **ships as a build, not an editor/project setup**, so the editor-attached MCP bridge **won't exist** in the shipped product. The control surface is therefore a **runtime HTTP/WebSocket listener embedded in the plugin's runtime module**, NOT a dashboard over the bridge. Known design flag: **security surface of an open control port** on a shipped game process. *(A separate **parallel exploratory chat** is prototyping a fully-local dashboard NOW — additive on top of the tab-cycle/hotkey controls, possibly with a UE Pixel Streaming preview. That track is **experimentation**; the production target + this main-sequence ordering are **unchanged**. Don't mistake a fast bridge/local prototype for the real runtime-server path.)*

**Also queued (not yet sequenced into the above):**
- `flicker` → **`blinking`** rename + a new **`flickering`** anomaly (scene-region / light on-off toggling).
- **Lighting enrichments:** region/area darkening (kill affecting lights — ties to the viewport layer), missing shadows (`CastShadow` off), flat/unbuilt-GI; enrich `lod_corruption` (stuck-at-LOD0 + LOD-boundary seam) and per-actor (not just global) speed bugs.
- **Selector screen-X (left-to-right) ordering** — queued UX polish (v1 is name-sorted).
- **Post-process milestone (deferred):** color / aliasing / blur + the `AnomalyCvar` helper (lands with the first genuine `IConsoleVariable` anomaly, G13).
- **Far horizon:** the separate **replay/capture plugin** (paired clean-vs-injected captures) + the **labeling system** (2D bbox from bounds + custom-depth/stencil masks + timestamp sidecar).

---

## 6. Open questions vs locked decisions

**LOCKED (don't relitigate):**
- v1 scheduler guard = **one-anomaly-per-actor**; concurrency **collision-free, no coordinator**; the **fixed determinism draw protocol**; **two-switch** model (Enable/Run) + direct-core `Step`/`FireOnce`; **warn-not-block** coexistence; **4-id pool**; cadence **defaults** (tunable).
- `IAnomaly` stays locked; deps stay `Core/CoreUObject/Engine/InputCore`; the `=` exact-match + `GetVisibleRenderableActors` stay byte-clean; renderable = visible SM/SK/FX that draws something (allowlist, not blocklist; VFX via `UFXSystemComponent`); no-view ⇒ offer nothing.

**OPEN / deferred-by-design:**
- **Conflict-group selective same-actor stacking** (deliberate compound anomalies) → needs the G12 ref-count "hidden-by" coordinator + **per-(id,target) registry keying** (to hold >1 instance of an id). Explicitly **not** v1.
- **Dataset-density tuning** of cadence (interval/hold/MaxConcurrent) for the eventual capture runs.
- **Globals / lighting "non-object track"** (a second selection path for `time_dilation`/`camera_clipping`/`lighting_mismatch`).
- The no-blind-fire + zero-match **real-Play discrimination** (folds into m5's already-gated paths; couldn't be forced under a fixed Simulate camera).

---

## 7. Corrections / things that changed this session

- **The "concurrency is the first real need for the coordinator" framing was REVISED.** It is **not**, for v1: registry one-instance-per-id + the one-anomaly-per-actor guard make v1 collision-free with **no coordinator**. G12's coordinator is still the correct **future** path for deliberate compound anomalies. Recorded as **G30**; G12 gets a **supersede-note** (not deleted).
- **The original m6 per-conflict-group guard → one-anomaly-per-actor (OVERRIDE-1)**, before coding; the `id→group` table was **dropped from v1**.
- **`auto-injection-handoff.md` §4/§5 open questions are now RESOLVED** by m6 — this doc supersedes it for the m6 close + forward plan.
- **New gotcha G31:** `GetUsage()` is a human hint, not the mutated-resource identity.

---

## 8. Pointers — what to have Code read for operational detail

- **`CLAUDE.md`** — canonical context + Current status (latest as-built = **m6**).
- **`docs/sessions/2026-06-19-010-auto-injection.md`** — m6 detail (goal, locked design = these rulings, source verification, file list, gates, hand-off).
- **`docs/sessions/2026-06-19-009-selector-inject-ui.md`** — M5 (selector + renderable filter + the two primitives this milestone reuses).
- **`docs/sessions/2026-06-18-008-viewport-visibility-layer.md`** — M4 (viewport layer).
- **`architecture.md`** — the new `UAnomalyAutoInjectorSubsystem` section, control surface, deps/version.
- **`gotchas.md`** — **G30** (collision-free-by-construction one-per-actor; supersedes G12's framing), **G31** (`GetUsage()` ≠ resource identity), **G12** (+ supersede-note; still the future coordinator path), **G28** (`=` exact-match), **G29** (renderable-visible set).
- **`auto-injection-handoff.md`** — prior handoff (open questions now resolved here).

---

*Closing note for the new session: have Code bootstrap cold from `CLAUDE.md` + journal 010 (+ 009/008 as needed) + this handoff, summarize current state back, then scope the next milestone (high-priority new visual bugs — built viewport-aware AND auto-injectable from birth). xhigh, plan-before-code.*
