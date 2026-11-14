# 095-09 — m59: no positive label where a forced LOD changes nothing

**Date:** 2026-11-14 · **Author:** Claude Code (Opus 5.5, xhigh), headless · **Branch:** `fix/m59-forced-lod-refusals` off
`master` `ebc34ed`, merged into `integ/m56-m58`. **Brief:** mailbox 095-09 under chat ruling `_reviews\095-08-ruling.md`
("A label defect in delivered code: fixed first, as m59"). **Light brief:** no Game build, no editor or game launch, no
capture leg, no cook. **Receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-09\`.

## 1. Goal
Two earlier source readings said the delivered build can write positive labels on frames where nothing changes on
screen: `lod_popping` on a hierarchical instanced static mesh (HISM), and `lod_corruption` on a HISM or a single-LOD
mesh. Verify each claim in the delivered code and the UE 5.1.1 source, refuse every confirmed case by a named reason,
and report the skeletal one-frame question without changing it.

## 2. Part 1 — the claims (full evidence with `file:line`: `part1-claims-095-09.md`)
| # | Claim | Verdict |
|---|---|---|
| 1 | A HISM ignores a forced LOD; a plain ISM? | **Confirmed.** The HISM proxy is dynamic-relevance only and picks LODs per cluster from `ClampedMinLOD` and the `foliage.*` cvars; `ForcedLod` has zero references in `HierarchicalInstancedStaticMesh.cpp`. A plain ISM honours it for the whole component (it uses the base static-mesh draw path and `GetLOD`). |
| 2 | `lod_popping` selects a HISM outside foliage and labels it | **Confirmed.** The selection filter drops only foliage-actor owners and zero-instance ISMs; Apply resolves every static-mesh component of the actor, HISM included, and has no instancing check. A targeted fire also reaches a named foliage actor. Every popped frame is labelled (`AnomalyState`, active bit only). |
| 3 | `lod_corruption`: single-LOD, HISM, ISM; auto pool and targeted | **Confirmed, narrower reach:** not in the auto pool or the selector on `master`; reachable by a targeted capture. Apply had **no guard**: single-LOD and HISM components were forced and every on-screen frame labelled (`FireWindow`). A multi-LOD plain ISM changes visibly. The 4.25 branch skips single-LOD and uses its predictor in the auto pool only; no HISM check either. |
| 4 | Skeletal: which frame's render shows a plugin-set forced LOD | **Confirmed from source, not measured.** `SetForcedLOD` marks nothing dirty; the component reads it in its own `TG_PrePhysics` tick. A write from a tickable or the capture tick shows on the **next** frame's render; under `IAI.Bench.SynthTickOrder` the injector's dispatch runs before the component tick and shows the same frame. |

The m26 veto deletes only `MEASURED_ZERO` events; each of these targets draws pixels, so nothing downstream caught them.

## 3. Part 2 — the fix (`19c0717`)
- New pure header `Source/AnomalyInjector/Public/AnomalyLodPolicy.h`: `DecideForcedLod(bHism, NumLods)` → `None` /
  `HierarchicalInstanced` / `SingleLod`; names `hism_ignores_forced_lod`, `single_lod`; `DominantRefusal` picks the name
  reported when every component was refused.
- `AnomalyLod::GetForcedLodRefusal(Component)` applies it (`IsA<UHierarchicalInstancedStaticMeshComponent>`, which
  includes foliage components; LOD count from `GetWorstLod`).
- `lod_popping`: the HISM refusal sits beside the existing single-LOD refusal, same log style, counted in both summary
  lines. `lod_corruption`: both refusals, every fire (targeted or auto-pool, with or without a `lod-index`), per-component
  log lines, a REFUSED ALL line, and a refused count on the success line. All refused ⇒ Apply returns `false` ⇒ no fire,
  no event, no label.
- Both anomalies now override `GetLastRefusalReason`, so the `Auto.Yield` tally names the reason instead of `refused`
  (only the two new names; the other `lod_popping` gates still read `refused`).
- Not changed: plain ISM; skeletal targets; Nanite (already refused by default, and Nanite frames are not annotation
  members when allowed); the graded "already at its coarsest LOD" case of `lod_corruption`.
- No targeted-fire rule is contradicted: `lod_popping`'s own log already says the single-LOD guard applies to both paths.
- Size: 58 changed lines in existing source files plus the 27-line pure header written for the selftest (brief guide
  "about 60"; stated, not hidden).

**Tests.** `tools/lod_refusal_selftest.cpp` 29 checks, 0 failures; three mutants (HISM check removed, single-LOD
threshold 1, dominance tie) fail 9, 1 and 2 checks. Editor target (Development Win64, 2 actions, launched at 10.5 GiB
free): exit 0, 0 warnings, 0 errors; the new strings are in `UnrealEditor-AnomalyInjector.dll` beside pre-existing
controls. All 16 pure C++ selftests pass (four need include paths and the readiness test runs from `tools`, a harness
detail). Python: label rule 65, pixel gate 98, change oracle 35, kit 93, unit tests 49, exclusion gate, measure offset,
lever audit source PASS (53) and binary PASS, lever selftest 12.

## 4. Skeletal: recommendation (behaviour not changed)
**Recommend (b): refuse skeletal targets in both LOD anomalies until a leg measures them.** Reasons:
1. The source reading predicts a one-frame label/picture disagreement on **both** edges of every `lod_corruption`
   event in **both** tick orders (the onset frame is captured on the fire tick and labelled while the old LOD is still on
   screen; the revert frame is unlabelled while the forced LOD is still on screen), and on every `lod_popping` pop edge in
   the native order. No flag excuses these frames on the release defaults (temporal transition windows 0/0/0; the settle
   ticks come after the edge frame).
2. The bench has no known multi-LOD skeletal mesh: SKM_Bot is single-LOD (G20, so m59 already refuses it as
   `single_lod`), SKM_Backpack's LOD count is not established, the m53 skeletal cube is a fixture cube. A leg needs a new
   cooked asset (a 2-LOD copy of SKM_Bot authored by editor Python) and would then need a fix design anyway.
3. The cost is small: StackOBot has no multi-LOD skeletal content; on a client title it removes character LOD events
   whose edge frames are predicted wrong. Chat's owner question 2 (pops on characters or props only) decides whether to
   lift it, via the measuring leg L59-6.

## 5. Part 3 — integration
`fix/m59-forced-lod-refusals` merged into `integ/m56-m58` (a true merge), compiled and tested under the same rules,
pushed; the primary checkout is left on `integ/m56-m58` with Editor binaries built from its tip. Results in the status
block and the receipts.

## 6. Legs for the heavy gate brief (frozen: `predictions-095-09.txt`, SHA-256 `DDFA0C43…DDE6DD97`)
- **L59-1** `lod_corruption` on single-LOD `StaticMeshActor_49` (CB_GateLevel): release applies and the pixel gate reads
  NO-TRACE; m59 refuses `single_lod`, no event.
- **L59-2 / L59-3** `lod_corruption` / `lod_popping` on `InstancedFoliageActor_0_0_0` (MainWorld, SM_Bush 2 LODs): release
  applies and labels; m59 refuses `hism_ignores_forced_lod`. The release pixel reading is reported, not decisive.
- **L59-4** control: `CB_LodFix_N` (plain multi-LOD) still applies on m59; edges as 089-03.
- **L59-5** optional decisive pair: HISM and ISM copies of CB_LodFix_N's mesh (fixture-script option on 095-07's cook,
  or a new bench spawn lever); release HISM = NO-TRACE, ISM flips; m59 refuses the HISM only.
- **L59-6** only if chat picks skeletal option (a).

## 7. Deviations and notes
- The brief's "through the auto pool" for `lod_corruption`: it is not in the auto pool on `master`; reach is targeted
  only. The refusal still applies to every path.
- The client readme gains the refusal sentence and a parenthetical that the "same frame" statement holds for static
  meshes. Whether to tell the client that earlier datasets may hold such labels is a communication decision for the owner;
  the readme does not say it.
- New gotcha **G579**. Next free: **G580**.
