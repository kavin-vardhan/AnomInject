# Anomaly Injector — Handoff: post-M5 → Auto-Injection

**Purpose.** Bridge document for a cold reader (a fresh chat session, Claude Code, or a collaborator) covering what shipped in the M4→M5 arc and the decisions/sequencing carrying into the next milestone: **automatic injection**. Read this alongside `CLAUDE.md`, `architecture.md`, `gotchas.md`, the prior `viewport-and-roadmap-handoff.md`, and session journals `008`/`009`. This doc is the continuity artifact — the local repo (journals, gotchas, architecture) is invisible to a fresh chat; this + project memory carry context forward.

---

## 1. Current state — "you are here"

**What this is.** `AnomalyInjector` — a game-agnostic UE **5.1** plugin that injects labeled visual anomalies (graphics bugs) into a running game to produce synthetic ML training data. Public UE APIs only; tested on the StackOBot sample. Project identity is "GDP: Anomaly Injection"; the GDP *code* prefix was stripped (session 007).

**Repo / engine.** Plugin is its own git repo at `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\`. Source-built UE 5.1 at `D:\UESource\UnrealEngine`. Both real target games run 5.1.

**Git state.** Branch `master`; HEAD **`f342272`** (m5 acceptance docs). Latest milestone **m5** = `aa2a3a4`. Tags: `m1 → m2 → m2.5 → m3 → m4 → m5`. Bridge + host scaffolding remain **unversioned** (host tooling, per gotcha G8).

**Shape.** 7 anomalies, `IAnomaly` interface **unchanged since M1**, deps `Core / CoreUObject / Engine / InputCore`, VersionName **0.6.0**. Control = `IAI.*` console **+** the in-game selector (Tab-cycle + inject). Verification = bridge state-gates in a MainWorld **Simulate** session + owner eyeball in real **Play**.

**Anomaly catalog (7, unchanged):** `missing_object`, `flicker`, `time_dilation`, `lighting_mismatch`, `lod_corruption` (static+skeletal), `camera_clipping`, `lod_popping`.

---

## 2. What shipped this arc (M4 + M5)

### M4 — Viewport-Visibility Layer (`7c34275`, tag `m4`)
- New shared helper **`AnomalyViewport`** (free-function leaf, same convention as `AnomalyTargeting`/`AnomalyArgs`/`AnomalyLod`).
- Visibility = **frustum AND occlusion**. Frustum from the live player view (reversed-Z VP); occlusion = **multi-sample camera-to-bounds line trace** (center + 8 corners; visible if any sample reaches the target unblocked).
- **Core takes an explicit view spec** (`FAnomalyViewInfo`); a thin live resolver (`GetActiveViewInfo`) fills it from the player camera. This split is what makes the core deterministically state-gatable with a synthetic view.
- Opt-in toggle **`IAI.SetViewportScoping <0|1>`, default OFF** — when ON, the 4 object-scoped anomalies (`missing_object`, `flicker`, `lod_corruption`, `lod_popping`) route target resolution through `AnomalyViewport`. Default-OFF keeps every prior gate byte-identical.
- No `IAnomaly` change; no new dep.

### M5 — In-game Object Selector + Inject UI (`aa2a3a4`, tag `m5`)
- New **`UAnomalySelectorSubsystem`** (separate `UTickableWorldSubsystem`, Game+PIE) that calls the existing injector's public API — injector and `IAnomaly` untouched.
- **Tab-cycle** the visible objects, pick one of the 4 object-scoped anomalies, inject/revert. World-space **debug box + name label** on the selected actor; HUD lists visible objects + the 4 injectable anomalies.
- **Two shells over an explicit-core:** `IAI.Selector.*` console commands (bridge-driveable, state-gated) + raw input polling and an immediate-mode `UDebugDrawService` HUD (real-Play, owner-eyeballed).
- **`=` exact-match sentinel** added to `AnomalyTargeting::FindActorsMatching` — inject targets exactly the selected actor, never a prefix-sibling.
- **Renderable filter** added to `AnomalyViewport` (additive; M4 visibility functions left byte-identical).
- HUD **zero-match line** so a non-applicable combo announces itself.
- First dep since M0: **`InputCore`** (no Slate/UMG — immediate-mode beat the roadmap's anticipated UMG cost).

---

## 3. Decisions made this arc — rationale (the load-bearing part)

These are *why*, not just *what*, so the next session doesn't relitigate.

- **Occlusion backend = line-trace, NOT render-time.** The core must be deterministically synthetic-view-gatable (the deepest lock); render-time only describes the *real* frame the renderer drew, so it can't be evaluated against a synthetic view and would make the occlusion path eyeball-only. Line-trace is synchronous, pure `(view, world)`, Engine-only, works in Simulate.
- **`GetLastRenderTimeOnScreen()` is the *future* render-time backend** (reserved for the capture milestone, where render-fidelity — materials/foliage/translucency the trace misses — actually matters). It's behind the same private API, so swapping it in later is a `.cpp`-only change. Note: it's specifically `GetLastRenderTimeOnScreen()`, **not** `GetLastRenderTime()`/`WasRecentlyRendered()`, which are shadow-contaminated (an occluded shadow-caster bumps them).
- **`=` exact-match is load-bearing beyond M5.** A substring match would corrupt numbered siblings (select `Cube` → also hits `Cube2`) = a mislabeled-but-present frame, the exact failure the viewport layer exists to prevent. Auto-injection will use the same apply-by-name path on arbitrary actors, so this protects that too.
- **Renderable = a *visible* SM/SK/FX component that actually *draws something*** (instanced ⇒ instance count > 0). **Allowlist (type test), not a class blocklist** — game-agnostic; a blocklist rots on another title. VFX (Niagara + Cascade) caught via the common Engine base **`UFXSystemComponent`** → no Niagara module dep. **⚠ SUPERSEDED (VFX) by G33 (2026-06-20): VFX was removed from the renderable-visible set — renderable is now SM/SK only.**
- **VFX ruling:** particles stay selectable. `missing_object`/`blinking` work on them (real bugs). The **LOD anomalies won't match a particle** (no mesh LODs) — handled by the anomalies' own targeting, not by carving particles out of the visible set; the zero-match result is surfaced on the HUD so the non-applicable combo is visible, not a silent no-op. **⚠ REVERSED by G33 (2026-06-20): particles are NO LONGER selectable — VFX was carved out of the visible set after all. The `=name` console escape hatch still reaches VFX actors directly; the zero-match-on-VFX case is now reached only via that hatch, not the selector/auto set.**
- **Selector "no view → offer nothing"** (`GetVisibleRenderableActors` returns empty), deliberately **distinct** from the console finders' "no view → treat-as-unscoped." Two callers, two safe directions: a console command is an explicit human instruction (act, don't silently drop); the selector/auto-injection path has no legitimate visible set without a view, so offering anything = injecting blind. Do not "reconcile" these.
- **Auto-injection chosen as the next milestone, ahead of the new bug types.** It's the headless cousin of M5 (reuses `GetVisibleRenderableActors` + `=` apply-by-name, both proven), it's cheap now, and completing the core injection loop (manual + auto) means every future bug type is born both viewport-aware AND auto-injectable instead of retrofitted. Counter-argument (more bug variety makes auto-injection a richer demo) was weighed and set aside.
- **Dashboard parked → Tier-2 runtime in-build control server.** Firm architectural fact: the whole system **ships as a build, not an editor/project setup**, so the editor-attached MCP bridge won't exist in the shipped product. The eventual control surface is therefore a **runtime HTTP/WebSocket listener embedded in the plugin's runtime module**, not a dashboard over the bridge. This "ships-as-a-build" lens retroactively validates M5's selector being an in-game runtime UI. Sequenced **after** auto-injection. Known design flag: security surface of an open control port on a shipped game process.

---

> **RESOLVED in m6 (session 010, 2026-06-19).** Automatic injection was scoped, implemented, and compiled
> clean. The open questions in §5 below are now decided (see `sessions/2026-06-19-010-auto-injection.md` and
> `architecture.md` → "Automatic Injection"). Headline decisions: a separate `UAnomalyAutoInjectorSubsystem`;
> **concurrent but collision-free via a one-anomaly-per-actor invariant — NO ref-count coordinator** (the
> handoff's "first need for the coordinator" was reconsidered: the registry's one-instance-per-id already
> bounds concurrency, and one-per-actor subsumes both conflict groups + the hide-masks-LOD case); auto-revert
> after a randomized hold (persist is an off-by-default flag); one seeded `FRandomStream`; v1 pool = the 4
> object-scoped ids only. §4/§5 are retained below as the historical scoping brief.

## 4. Next milestone — Automatic Injection (scoping not yet started)

**Locked spec.** At gameplay start, a UI selects which anomaly types are enabled. During play, those anomalies fire **randomly on whatever renderable objects are on-screen at that moment.** This is the live-autonomous flavor — **distinct** from the deferred capture/replay/labeling dataset pipeline.

**Builds directly on:** `AnomalyViewport::GetVisibleRenderableActors(World)` (the renderable-visible set) + the `=` apply-by-name primitive. Both proven in M5.

**Run it as a scoping/architecture turn with Code first (xhigh), plan before code** — same discipline as M4/M5.

---

## 5. Open questions for the auto-injection scoping turn (NOT yet decided)

These are the design surface to resolve with Code, not pre-decided here:

- **Enable-set UI:** reuse M5's HUD/input shell or a new runtime surface? (Keep it in-game/runtime per the ships-as-a-build lens — no editor panel.)
- **Cadence/density:** how often do random fires happen — fixed interval, random interval, target density? One-at-a-time vs concurrent?
- **Concurrency (likely the headline):** firing multiple anomalies at once surfaces G12 (last-writer-wins on the shared `bHidden` flag) and the one-instance-per-id limit. This is probably the first real need for the **subsystem-level "hidden-by"/concurrency coordinator** flagged in G12 — addable without an `IAnomaly` change. Decide whether auto-injection needs it in v1 or can stay single-fire.
- **Lifecycle:** do fired anomalies auto-revert after a duration, or persist until reverted? (Matters for the later capture-pairing pipeline.)
- **Determinism/seeding:** ML wants reproducible runs — does the random path take a seed?
- **Anomaly pool:** the 4 object-scoped only, or include globals (`time_dilation`/`camera_clipping`) and `lighting_mismatch` on a separate non-object track?

---

## 6. Locked decisions (settled — don't relitigate)

- Visibility = frustum AND occlusion; v1 occlusion backend = line-trace; render-time (`GetLastRenderTimeOnScreen`) reserved for the capture milestone.
- `=` exact-match is the v1 targeting-identity ceiling (perfect pointer identity across streamed sublevels with duplicate names would need an `IAnomaly` change — rejected for now).
- Renderable = visible SM/SK/FX that draws something; allowlist, not blocklist; VFX via `UFXSystemComponent`. **⚠ VFX removed — G33 (2026-06-20); renderable is now SM/SK only.**
- Selector/auto-injection no-view → offer nothing.
- `IAnomaly` stays locked; deps stay `Core/CoreUObject/Engine/InputCore` (any growth is a flagged decision).
- Auto-injection is next; new bug types after; dashboard (Tier-2 runtime server) after that.

---

## 7. Corrections / things that changed this arc

- **Simulate DOES expose a usable local-player view** (contra the planning fear) — gotcha G23. So selection is bridge-state-gatable; the no-view degrade is **code-verified, not bridge-triggerable**.
- **`RoomBuilderSquare` is a genuine renderable** (243 instanced meshes) and is **kept** — an earlier "non-visual scaffolding" assumption was wrong. The capability/type filter corrected it automatically (the point of allowlist-by-capability over name guesses).
- **`LandscapeStreamingProxy` dropped** via the empty-instance guard (it only survived through zero-instance grass). Refinement: an instanced component with 0 instances is not renderable.
- The initial M4 brief's "render-time occlusion default" was wrong on **both** the accessor (named `GetLastRenderTime` instead of `GetLastRenderTimeOnScreen`) and the approach (render-time can't be synthetic-gated) — corrected to line-trace for v1.
- **Roadmap resequenced:** the debug UI was reframed as the M5 selector and pulled forward; auto-injection pulled ahead of the new bug types.

---

## 8. Pointers — what to have Code read for operational detail

- **`CLAUDE.md`** — canonical context + Current status.
- **`docs/sessions/2026-06-18-008-viewport-visibility-layer.md`** — M4 (viewport layer) detail.
- **`docs/sessions/2026-06-19-009-selector-inject-ui.md`** — M5 (selector + renderable filter) detail, incl. the renderable-set forward note.
- **`docs/viewport-and-roadmap-handoff.md`** — prior roadmap handoff (now committed `3da4562`).
- **`architecture.md`** — `AnomalyViewport`, `UAnomalySelectorSubsystem`, the renderable filter, control surface.
- **`gotchas.md` G22–G29** — occlusion signal (G22), Simulate view (G23), reversed-Z VP (G24), `UDebugDrawService` host-blind (G25), raw input mapping-independent (G26), InputCore-via-Engine (G27), `=` exact-match (G28), renderable predicate + `UFXSystemComponent` base + landscape extension point (G29).

---

*Closing step for the new session: it should have Code bootstrap cold from `CLAUDE.md` + journals 008/009 + this handoff, summarize current state back, then scope the auto-injection milestone (xhigh, plan-before-code).*
