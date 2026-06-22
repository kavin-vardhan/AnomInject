# 2026-06-22 — 014 — Screen-coverage candidate cull (AnomalyViewport)

**Type:** non-milestone feature (no tag, no version bump). One atomic `feat(viewport):` commit.
**Scope:** `AnomalyViewport.{h,cpp}` + docs only. `IAnomaly` untouched; deps unchanged (`Core/CoreUObject/Engine/InputCore`).

## Goal
Add a candidate-set refinement to the renderable-visible set, sibling to the poll-radius cull (011, `ae57b69`):
filter out actors whose on-screen footprint is too small a fraction of the viewport, so they are never offered as
anomaly targets (selector, auto-injector, dashboard A4). Narrow the same "renderable-visible" lane.

## Bootstrap (G43)
- HEAD `106dafa`; `git describe` = `m8-3-g106dafa` (tag `m8` = `acf29b4` missing_texture).
- **Working tree CLEAN** — no other-track WIP to protect this turn (control-server + labeling tracks fully landed on
  `master`: `af44e7d` WS commands, `a12d16e`/`e7fcabe` capture fixes, plus post-m8 `d820a55` UI-hide). Still path-scoped
  staging to `AnomalyViewport.{h,cpp}` + `docs/**` per discipline.

## Locked design (from chat-Claude brief; both forks confirmed by owner)
- **Metric** = projected 2D bbox area ÷ viewport area. AABB-over-OBB inflation is the safe (never-over-cull) direction.
- **Granularity** = per-**actor**: union world-AABB of the actor's renderable-**visible** components (those that passed
  renderable + frustum + occlusion + poll-radius). Cull the whole actor if under threshold.
- **Placement** = a shared per-actor classifier both live entry points funnel through (see Deviation below).
- **Threaded as a param** (live entry points opt in; synthetic/explicit-view pass the OFF sentinel).
- **Console** mirrors poll-radius: file-static `GMinScreenCoveragePct` (percent, [0,100]) + accessors; plain
  `IAI.SetMinScreenCoverage <pct>` (≤0 = OFF = byte-identical, defensive no-arg guard). **Tuning companion**
  `IAI.DumpCoverage` (ascending coverage %, marks would-be-culled).
- **Projector reuse**: the clamped `ProjectBoundsToScreenRect` (already in `AnomalyViewport`), fed the visible-component
  union — NOT m7's type-only/unclamped `ProjectActorBoundsToScreenRect`.
- **Eval order**: coverage is the most expensive, actor-level gate → runs LAST; composes with poll-radius.
- Robustness: behind-camera corners skipped + clamp-before-area (both already handled by `ProjectBoundsToScreenRect`).

## Deviation surfaced + approved (the one real fork)
The brief assumed a single shared actor-aggregation point existed. It did **not** — `GetVisibleRenderableActors` and
`GetVisibleRenderableActorInfos` each have their own actor loop, sharing only the per-component
`FirstRenderableVisibleComponent`. **Approved resolution:** introduce `ClassifyRenderableVisibleLive` as that shared
per-actor decision, called by both loops (guarantees `IAI.DumpVisible` set-identity with the cull ON). OFF path keeps the
cheap short-circuit (byte-identical result **and** cost); ON path does one union pass (first match + union bounds, no
double tracing). Dual-applied-gate alternative rejected (double-traces + drift risk). Also confirmed: feed the clamped
`ProjectBoundsToScreenRect` the visible-component union (the "same box as m7 label" framing was a motivation, not a
constraint — superseded; coverage = true clamped visible footprint). See gotcha **G51**.

## What was done (as-built)
`Source/AnomalyInjector/Public/AnomalyViewport.h`
- Doc block + `SetMinScreenCoveragePct(float)` / `GetMinScreenCoveragePct()` (mirror poll-radius accessors).

`Source/AnomalyInjector/Private/AnomalyViewport.cpp`
- Anon state `float GMinScreenCoveragePct = 0.0f` (percent; ≤0 = OFF).
- New anon helpers: `CollectRenderableVisibleUnion` (single full pass → first match + union FBox of passing comps),
  `PassesScreenCoverage` (project clamped union rect → area% ≥ P), `ClassifyRenderableVisibleLive` (OFF = short-circuit;
  ON = union pass + coverage).
- `GetVisibleRenderableActors`: build VP explicitly + derive frustum from it (now identical to the Infos pass), call the
  shared classifier.
- `GetVisibleRenderableActorInfos`: call the shared classifier (replaces the bare `FirstRenderableVisibleComponent`).
- Namespace accessors `SetMinScreenCoveragePct` (clamp [0,100], ≤0→0) / `GetMinScreenCoveragePct`.
- Console `IAI.SetMinScreenCoverage <pct>` (plain global) + `IAI.DumpCoverage` (WorldAndArgs diagnostic).

Docs: `architecture.md` (coverage subsection + 2 console rows), `gotchas.md` (**G51**), this journal.

## Stage gates
- **G-Compile** — clean Development-Editor compile on 5.1 (exit 0); both modules link.
- **G-OFF byte-identical (regression)** — default (`coverage 0`): `GetVisibleRenderableActors` set+order unchanged;
  `IAI.DumpVisible` MATCH; `missing_object`/`lod_corruption` round-trip unchanged; `ListAnomalies` = 8.
  *(Owner-flagged: the unified VP/frustum derivation in `GetVisibleRenderableActors` MUST reproduce today's EXACT set —
  if not, STOP, do not paper over a set drift.)*
- **G-DumpVisible identity, coverage ON** — positive `pct`: `GetVisibleRenderableActors ≡ GetVisibleRenderableActorInfos`.
- **G-Cull correctness** — `IAI.DumpCoverage` ascending; threshold between two actors culls sub-threshold, keeps supra.
- **G-Robustness** — partially-off-screen large actor reads its on-screen sliver (clamped), not full; behind-camera = 0.
- **G-Synthetic byte-identical** — explicit-view funcs unaffected (never apply coverage).
- **Owner eyeball** — pick threshold via `DumpCoverage`, smoke in real Play.

## State / hand-off
Code + docs complete. Gate status filled in at commit time. Commit (when gated + owner-eyeballed):
`feat(viewport): add changeable screen-coverage candidate cull` — path-scoped to `AnomalyViewport.{h,cpp}` + `docs/**`,
no tag, no version bump. **Deferred (cross-track, flagged):** surfacing coverage in `FRenderableActorInfo` / the dashboard
snapshot (not this pass).
