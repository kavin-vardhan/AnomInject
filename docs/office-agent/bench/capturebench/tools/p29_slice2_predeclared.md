# P29 — `m26` SLICE 2 (REPORTING). Pre-declared BEFORE implementation and before any leg.

Session 2026-08-20, journal PART TWENTY-NINE. Scope frozen by the ruling and restated here so the
implementation cannot quietly widen it.

## What slice 2 is, and what it is NOT

**IS:** `mask.provided` stops being a hardcoded `false` and becomes **the tri-state's bool**.
**IS NOT:** ⛔ no sub-fields under `mask` (a count, a rect, an extent are all SHAPE changes = `P6`)
· ⛔ `depth{}` untouched · ⛔ `annotation.json` field SET unchanged · ⛔ **no veto, no
`vetoed_events`, no threshold — slice 3 is NOT started and its rule is NOT assumed by anything
here.**

## THE MAPPING — the safety property, and the code path that guarantees it

| internal state | `mask.provided` | meaning |
|---|---|---|
| `NotMeasured` | **`false`** | never measured ⇒ **MUST ADMIT** |
| `MeasuredZero` | **`true`** | measured ⇒ may be invalidated at slice 3 |
| `MeasuredNonZero` | **`true`** | measured |

**GUARANTEE:** `provided` is derived from **`State` ALONE**, through a single function
(`MaskStateProvidesMeasurement`) whose **only `false` branches are `NotMeasured` and "no mask
record found"**. The count is **never** consulted and is **never** emitted, so
`MEASURED_ZERO` and `NOT_MEASURED` **cannot collapse to the same artifact value** — one is `true`,
the other is `false`, and no code path derives `provided` from a magnitude.
⚠ **With `IAI.Capture.Mask` OFF there is no record for any event ⇒ `provided` stays `false` on
every event ⇒ output byte-identical to today.** The switch-off inertness of the REPORTING change is
therefore structural.
⚠ **Ordering consequence, stated because it is a real change:** the final mask collect must move to
**before** `WriteSessionAnnotationFile()` (it currently runs after it, feeding only
`run_summary`). Nothing else about `FinishRun` moves.

## THE KNOWN-ANSWER SET — the five slice-1 targets, whose correct values are already banked

| # | target | slice-1 result | **predicted `mask.provided`** |
|---|---|---|---|
| 1 | `StaticMeshActor_49` (`Cube`) | `MEASURED_NONZERO` 7.25 % | **`true`** |
| 2 | `StaticMeshActor_73` (`Cylinder`) | `MEASURED_NONZERO` 5.27 % | **`true`** |
| 3 | `InstancedFoliageActor_0_0_0` | `MEASURED_NONZERO` ~1.4 % | **`true`** |
| 4 | `BP_SplineSpawn_C` | **`MEASURED_ZERO`** | **`true`** 🚨 *measured-zero is still MEASURED* |
| 5 | 🚨 **`SM_Ramp2`** (known-Nanite control) | **`NOT_MEASURED`** ×8 | 🚨 **`false` — THE ONLY FALSE IN THE SET** |

🚨 **TARGET 5 IS THE ONE THAT MATTERS.** It is the only target whose correct value is `false`.
**If `SM_Ramp2` reads `true`, the mapping is inverted somewhere and a slice-3 veto would DELETE
it** — a target that draws real geometry and that the instrument merely cannot see. **That single
reading is the difference between a safe cure and a data-destroying one.**
🚨 **AND TARGET 4 IS ITS MIRROR: if `BP_SplineSpawn_C` reads `false`, `MEASURED_ZERO` has collapsed
into `NOT_MEASURED` and slice 3 would never fire at all** — the cure silently doing nothing.
**Both directions are checked, on banked known answers.**

## GATES

| # | gate | pass condition |
|---|---|---|
| **G-2** | `annotation.json` field SET unchanged, **BOTH delivery modes** | 48/48, **0 added, 0 removed**, against a pre-`m26` banked leg, on an OFF leg AND an ON leg |
| **P6** | measured unchanged, both artifacts | annotation 48/48 · `run_summary` **+3** (`mask_probe_arms`, `mask_residual_discards`, `mask_nopass_discards`) and nothing more |
| **MAP** | `provided` exactly where slice 1 measured | the five rows above, **all five correct**, per event |
| **G-9** | **delivery orthogonality (`m25`'s certified property)** | a **POSE-MATCHED** OFF/ON pair (**`A64` — B1 passing on each leg is NOT sufficient**; the two legs' modal bboxes must MATCH each other, because P27 §189.2 measured that within-B1-tolerance pose differences move mask counts). Difference set ⊆ **run-unique set** (established empirically from a same-config OFF/OFF control pair on this binary) **∪ `delivery_mode`** ∪ the files delivery suppresses. **EXTRAS = 0.** **Invariant core asserted POSITIVELY, including `mask.provided` per event and the three `run_summary` mask fields** |
| **INERT** | mask OFF ⇒ unchanged | a leg with `IAI.Capture.Mask` unset emits `provided:false` on every event |

**Legs:** `P29_S2_CTRL49_A` (OFF — mapping #1 + control-pair A) · `P29_S2_CTRL49_B` (OFF —
control-pair B) · `P29_S2_CTRL49_DEL` (**delivery ON** — G-9 test leg) · `P29_S2_CYL73` ·
`P29_S2_FOLIAGE` · `P29_S2_SPLINE` · 🚨 `P29_S2_RAMP`.
`A63` — every attempt banked. `B1` applies on the `StaticMeshActor_49` legs; **NOT APPLICABLE**
on `CYL73`/`FOLIAGE`/`SPLINE`/`RAMP` (`G117`), declared here rather than after. Delivery legs
cannot be pose-gated from the artifact (no `labels.jsonl`) — the harness reports `coverage_ratio`
as the pose INDICATOR, never as a gate.

## FAILURE BRANCHES

| # | reading | action |
|---|---|---|
| **F-1** | 🚨 `SM_Ramp2` reads **`true`** | **the mapping is inverted ⇒ HALT.** A slice-3 veto built on this would delete a target it merely cannot see |
| **F-2** | 🚨 `BP_SplineSpawn_C` reads **`false`** | **`MEASURED_ZERO` collapsed into `NOT_MEASURED` ⇒ HALT.** The two zeros have shared a representation, which is the one thing forbidden everywhere in this chain |
| **F-3** | any annotation key added or removed | **`P6` MOVED ⇒ HALT** |
| **F-4** | any sub-field appears under `mask`, or `depth` changes | scope breach ⇒ **HALT** |
| **F-5** | G-9 extras ≠ 0 | `m25`'s certified delivery orthogonality is broken ⇒ **HALT** |
| **F-6** | a mask-OFF leg emits `provided:true` anywhere | the change is not inert when off ⇒ **HALT** |
| **F-7** | any leg's slice-1 state disagrees with its banked slice-1 result | the measurement moved under the reporting change ⇒ report, attribute nothing, **HALT** |

⛔ **Constraints for the turn: no slice 3, no tag, `P6` does not move, stencil range 200/255,
CB_GateLevel untouched (`G99`), `feature/stencil-capture` READ-ONLY. Slice 3's veto rule is NOT
designed, NOT proposed, and NOT assumed by this implementation.**
