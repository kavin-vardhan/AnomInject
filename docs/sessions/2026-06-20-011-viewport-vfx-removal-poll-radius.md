# Session 011 — Viewport: VFX removal + changeable poll-radius cull (2026-06-20)

## Goal
Two surgical, owner-locked fixes to the shared renderable-visible set in `AnomalyViewport` — the single
source of truth consumed by the M5 selector, the m6 auto-injector, AND (new since m6) the control-server
A4 read-back (`GetVisibleRenderableActorInfos` → dashboard) + the `IAI.DumpVisible` set-identity gate.
**Two atomic commits, not a numbered milestone, no tag.** Touch only `AnomalyViewport` (+ docs); the
injector / anomalies / selector / auto / control-server cores are untouched and inherit the refined set.

## Baseline (cold-bootstrap finding)
The working tree was **ahead of memory/docs**: past m6 (`41ba104`) sit two committed control-server slices
(`2645236` transport spike, `4c05344` core read-back / A4) plus uncommitted control-server WIP. So
`AnomalyViewport` now has a **third consumer** (the A4 read-back) and a hard **set-identity gate**
(`IAI.DumpVisible` asserts `GetVisibleRenderableActors` ≡ `GetVisibleRenderableActorInfos`, set+order). Both
fixes had to respect that, and the two fix commits had to be **path-scoped** to avoid sweeping the WIP.

## FIX 1 — remove VFX from the renderable-visible set (HARD REMOVE; reverses G29/R1)
- Dropped `|| Component->IsA<UFXSystemComponent>()` from the `IsRenderableComponent` allowlist → set is now
  **SM ∥ SK** only. Also removed the dead `"FX"` branch in `ClassifyRenderableComponent` and the now-unused
  `#include "Particles/ParticleSystemComponent.h"`. Header doc + `FRenderableActorInfo::ComponentType` comment
  updated.
- **Propagates in lockstep** to the selector, auto-injector, control-server A4 set, and `IAI.DumpVisible` —
  one source of truth, no fork (owner-confirmed: a dashboard listing untargetable particle actors would be
  misleading).
- **Escape hatch intact:** the console by-name finders (`AnomalyTargeting`, incl. `=name`) do NOT route
  through `IsRenderableComponent`, so `IAI.Apply <id> =<VfxName>` still reaches VFX actors. Verified by code
  path + the compile (only the renderable-set predicate changed).
- **Gate consequence:** the old "select a pure-VFX actor → LOD → 0 matched" scenario is no longer reachable
  *through the set* (VFX is no longer offered). The `0 matched` plumbing is unchanged; the zero-match gate is
  re-pointed to the `=name` console escape hatch.

## FIX 2 — changeable poll-radius distance cull (default OFF)
- New shared state `GPollRadius` (cm) in `AnomalyViewport.cpp` (file-static, single source of truth); accessors
  `SetPollRadius` / `GetPollRadius` (header-declared). Console **`IAI.SetPollRadius <value>`** registered in
  `AnomalyViewport.cpp` as a plain `FAutoConsoleCommand` (world-independent global — no subsystem to resolve);
  defensive: no arg → log current radius + usage, change nothing.
- Cull lives in the shared chokepoint `IsComponentRenderableVisibleInternal`, threaded as
  `const FVector& PollOrigin, float PollRadius`. Order: renderable type-test → **distance cull** → frustum →
  occlusion (cheapest-first; out-of-range actors rejected before any line trace). Metric =
  `Dist(PollOrigin, Bounds.Origin) - Bounds.SphereRadius <= R` (cached `Component->Bounds` — no double-compute).
- **Origin = the player PAWN** (`ResolvePollOrigin` → first PC's pawn; camera origin only as the no-pawn
  fallback). Locked: pawn, not camera. The dashboard's `Distance` field stays camera-relative — distinct on
  purpose, documented.
- **Both live entry points** (`GetVisibleRenderableActors` + `GetVisibleRenderableActorInfos`) pass the
  identical pawn-origin + radius → the `IAI.DumpVisible` set-identity gate still holds. The **explicit-view**
  functions (`IsActorRenderableVisible` / `FilterRenderableVisibleActors`) pass radius `0` → synthetic-gate
  surface stays byte-identical even with a radius set (threading the radius as a param, not reading the global
  inside the chokepoint, is what makes this opt-out possible).
- **Sentinel default OFF:** `R <= 0` disables the cull entirely → byte-identical regression guarantee.
- **Debug sphere (dev):** `UDebugDrawService("Game")` delegate draws a yellow sphere of radius R centered on
  the **live pawn, re-resolved every frame**; registered/unregistered on the OFF↔ON boundary (G25 hygiene).
  Accepted minor: a module unload while a radius is set leaks the handle (a teardown hook would touch the
  module `.cpp`, out of scope).
- **No new dependency** (`IConsoleManager`/`FVector` = Core; `UDebugDrawService`/`DrawDebugSphere`/`APawn` =
  Engine). `IAnomaly` untouched. No `.uplugin` version bump.

## Files
- **Code (FIX 1 + FIX 2):** `Source/AnomalyInjector/Public/AnomalyViewport.h`,
  `Source/AnomalyInjector/Private/AnomalyViewport.cpp` — the ONLY source files touched.
- **Docs (FIX 1):** gotchas **G33** + G29 SUPERSEDED-VFX annotation; architecture/runbook/handoff VFX-inclusion
  notes flipped; pure-VFX zero-match gate rows re-pointed to the `=name` escape hatch.
- **Docs (FIX 2):** gotchas **G34**; architecture renderable-set bullet + `IAI.SetPollRadius` in the control
  surface; runbook smoke step 12 + four poll-radius gate rows.
- **This journal** + CLAUDE.md Current-status refresh.

## Gates
- **Clean Development-Editor compile on 5.1 (exit 0)** before EACH commit. The control-server module recompiled
  against the changed header and **linked clean** both times — confirms the A4 consumer still builds.
- **Owner to smoke-test** against the gate rows (runbook §7): VFX excluded from the set everywhere; `=name`
  escape hatch still reaches VFX; radius culls beyond R in selector/auto/dashboard with `DumpVisible` still
  MATCH; `R<=0` byte-identical; debug sphere tracks the pawn.

## Commits (two atomic, path-scoped, compile-clean each; no tag)
- `9bbd398` **fix(viewport): remove VFX from renderable-visible set (reverses G29)**
- `<fix2>` **feat(viewport): add changeable poll-radius distance cull**

Staged explicitly (only `AnomalyViewport.{h,cpp}` + the touched `docs/**` per commit) — the control-server WIP
in the tree was left untouched (`git add -A` never used).

## Hand-off
- Both fixes land on top of `4c05344` (control-server Slice 1 stage 1). The renderable-visible set + the two
  primitives (`GetVisibleRenderableActors` / `=name`) stay byte-clean for the ongoing capture/labeling +
  control-server work. Next per the roadmap is unchanged (new viewport-aware bug types; Tier-2 control server).
