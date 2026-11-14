# 095-12 — m59 review follow-ups: component scope, usable LODs, not-coarser refusal, skeletal switch docs

**Date:** 2026-11-14 · **Author:** Claude Code (Opus 5.5, xhigh), headless · **Branch:** `fix/m59-forced-lod-refusals`
(from `af1b08f`), merged into `integ/m56-m58` (from `876c719`). **Brief:** mailbox 095-12 under chat ruling
`_reviews\095-11-ruling.md` (items S3 for m59, S9, S10, S11, N14) and the review `_reviews\095-11-independent-review.md`.
**Light brief:** Editor target only, pure tests; no Game build, no launch, no leg, no cook, no Codex. **Receipts:**
`D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-12\`.

## 1. What each item changed
| Item | Commit | Change | Proof offline |
|---|---|---|---|
| S10 | `d8077fc` | `single_lod` counts usable LODs: N − first usable LOD, the first usable being MinLOD (or the component override) clamped up to the first LOD with vertices, for static and skinned meshes (`AnomalyLodPolicy::FirstUsableLod` / `UsableLodCount`, `AnomalyLod::GetLodFacts`). Both LOD anomalies use it through `GetForcedLodRefusal`. | 25 checks; mutants "MinLOD ignored" and "stripped ignored" fail 9 and 8 |
| S9 | `91a87c2` | `lod_corruption` compares the engine-clamped forced LOD with the engine-clamped LOD shown now (existing `GetCurrentLod` prediction, clamped to the usable range and the streamed-in first LOD) and refuses `not_coarser_than_current`; if the current LOD cannot be read (no view, no render data, unknown resident LOD) it refuses `current_lod_undetermined`. | 46 checks; mutants "equal is coarser", "unknown admits", "forced ignores MinLOD", "shown ignores resident" fail 7, 3, 2, 3 |
| S3 | `c5113e3` | When only some of an actor's renderable components are forced, both LOD anomalies set the event's component scope to the forced ones at Apply and clear it at Revert (`AnomalyLod::ScopeEventToForcedComponents`, reusing the texture anomalies' `AnomalyViewport::SetEventComponentScope`). The box (`GetActorLabelBounds`) and the mask tagging already honour the scope; the capture's `ResolveNodeIdentity` now picks its component inside the scope. A fully forced actor sets no scope (byte-identical path). | 7 checks; mutants "never scope" and "always scope" fail 3 and 2; the box/mask effect needs leg L59-8 |
| S11 + N14 | `2b4661c` | Console help, set/clear lines and the read-back describe the switch as it behaves: `1` lets skeletal meshes past that refusal only (the other refusals still apply, so a single-LOD skeletal mesh is still refused, as `single_lod`); inside a run the latched value holds, outside a run a change applies at once; the read-back prints the latched value with its own source and a "next run" note. Readme updated, with no filter promised for earlier datasets. | Editor compile; strings in the DLL |

All four code commits were Editor-compiled one by one (0 errors, 0 warnings; 2 parallel actions; each launched with at
least 9.8 GiB free). `lod_refusal_selftest`: 141 checks, 0 failures; 13 mutants all fail.

## 2. Decisions taken inside the brief
- **Unknown current LOD ⇒ refuse.** A positive label that cannot be shown to change anything is the defect m59 removes;
  a lost fire is only yield. The reason is named separately (`current_lod_undetermined`) so a host where it fires often is
  visible in `Auto.Yield`.
- **Streaming is part of the comparison, not of the count.** The usable count uses only permanent facts (MinLOD,
  vertices). The streamed-in first LOD enters the S9 comparison, where it belongs: a forced LOD below it is drawn at it.
- **Scope only on a partial set.** `ShouldScopeToForced` sets the scope only when at least one renderable component is not
  forced, so every event that forces the whole actor keeps the 095-09 box, mask and node.
- **`node.bounds` unchanged.** It still describes the actor's renderable bounds; only the node's identity fields follow the
  scope. Stated in the readme.
- **Side effect, deliberate:** texture-corruption events with skipped parts now name a corrupted component in
  `asset_name` / `component_class` too (same function).

## 3. Not done, named
- `lod_popping`'s targeted timer still fires on a target already at its worst LOD (it warns); the S9 ruling names
  `lod_corruption` only, and m57 replaces this timer (bench-only there). Reported to chat.
- The static current-LOD prediction ignores `r.StaticMeshLODDistanceScale` and dithering (G581 residual).
- No leg ran. Legs for the heavy brief: `predictions-095-09.txt` AMENDMENT 2 (L59-8 scope, L59-9 explicit index,
  L59-10 MinLOD, L59-11 switch read-back), dated before any leg.

## 4. Integration
`fix/m59-forced-lod-refusals` merged into `integ/m56-m58` (a true merge), Editor-compiled and tested under the same rules,
pushed; the checkout is left on `integ/m56-m58` with Editor binaries built from its tip. New gotcha **G581** (G580 is held
by `feat/m57-lod-spatial-trigger`). Next free: **G582**.
