# P28 — the replacement `N-2` control, then THE `H5` LEGS. Pre-declared BEFORE either runs.

**Written and committed before any leg of this turn.** Session 2026-08-20, journal PART
TWENTY-EIGHT. Binary under test: staged **`F93AEF71`** (both faults fixed + the extent
precondition), container unchanged from the `m26` cook.

🚨 **THESE ARE THE LEGS THE WHOLE MILESTONE HAS BEEN BLOCKED ON. The aimed-at result —
`MEASURED_ZERO` on an `H5` target — is exactly the result that most needs its evidence declared in
advance, because it is the answer we want.**

---

## LEG A — the replacement `N-2` control: `StaticMeshActor_73` (`Cylinder`, CB_GateLevel)

Non-Nanite (`/Engine/BasicShapes/Cylinder`, P27 §188.2), drawing, selectable, and distinct from
the item-1 calibration target. `blinking`, `IAI.Capture.Mask 1`, marker default.

| | prediction |
|---|---|
| state | **`MEASURED_NONZERO`** on every full event |
| buckets | `framesContributed=4`, `framesDiscarded=0`, **`framesNoPass=0`**, `framesResidual=0`, `framesUnconfirmed=0`, `probeArms=0` |
| `collisions` | **0** |
| `pctOfFrame` | **non-zero and PLAUSIBLE for a cylinder of its on-screen size.** ⛔ **No specific figure is predicted — this is a different mesh at a different transform from the `Cube`, so quoting 66,800 px would be a fabricated expectation** |
| `B1` | ⛔ **NOT APPLICABLE, declared in advance (`G117`)** — `CALIB_BBOX` is `StaticMeshActor_49`'s pixel bbox, so the pose gate has no scope on any other target and does not decide |

**⚠ THE TWO WEAKNESSES OF THIS CONTROL, CARRIED INTO THE RECORD RATHER THAN DISCOVERED LATER:**
1. **It shares CB_GateLevel with the item-1 control** — same lighting, same trivial geometry, same
   script-authored determinism. It demonstrates **NON-OVER-FIRING, not robustness.**
2. **Whether a stronger MainWorld candidate exists is UNESTABLISHED** — joining the non-Nanite
   asset list to placed, selectable, drawing actors needs a runtime census leg (`G122`'s exact-key
   rule). **Not run, not now.**
3. 🚨 **The A35 property is UNTESTED and stays untested** — no non-Nanite A35-shaped target exists
   anywhere in the measured set (P27 §190). This control cannot substitute for that.

**FAIL branches:** any `framesNoPass > 0` ⇒ a non-Nanite BasicShape failed to produce the pass ⇒
`G134`'s classifier is wrong ⇒ **HALT** · `MEASURED_ZERO` ⇒ the cure over-fires on a legitimate
drawing target ⇒ **`N-2` FAILS, HALT, do not run the `H5` legs**.

---

## LEG B / LEG C — 🚨 THE `H5` LEGS

**B: `InstancedFoliageActor_0_0_0`** (MainWorld, `blinking`) — `SM_Bush`, NON-Nanite (P27).
**C: `BP_SplineSpawn_C_UAID_A85E45CFE40412DE00_1511100424`** (MainWorld, `blinking`) —
`SM_GenericPlane`, NON-Nanite (P27).
`B1` **NOT APPLICABLE** on both (`G117`), declared in advance.

### The quantity that decides it — declared now so it cannot be chosen afterwards

**`H5` class (ii) says the LABEL CLAIMS FAR MORE THAN THE TARGET DRAWS.** So the informative
comparison is **the mask's `pctOfFrame` against the SAME event's claimed `coverage_pct`**, not
against the controls' 7.25 %. Banked claims: foliage **`coverage_pct 100`** (`bbox_px
(0,0,1280,720)`, 59/59 rows); `BP_SplineSpawn_C` **22.9 % of frame**. Banked pixel reality:
whole-frame change **0.0069** (foliage, marker-off 0.0059) in **4 of 64** grid cells; spline peak
in-bbox change **0.0175** against a control hide's **0.5515**.

### Pre-declared branches

| # | reading | what it means |
|---|---|---|
| **Z1** | **`MEASURED_ZERO`** (`maxCount == 0`) **with ALL buckets clean** — `framesNoPass=0 framesResidual=0 framesUnconfirmed=0 collisions=0 probeArms=0`, `framesContributed = arms` | 🎯 **THE AIMED-AT RESULT: the cure identifies an `H5`-shaped target.** The label claims a large area; the target measurably draws nothing |
| **Z2** | small non-zero `maxCount`, buckets clean — reported as **`pctOfFrame` vs claimed `coverage_pct`** | ✅ **also the cure working**: drawn ≪ claimed is exactly `H5`'s claim. Report the RATIO; ⛔ **do NOT convert it into a veto threshold — slice 3's threshold is a separate decision and is NOT being calibrated here** |
| **Z3** | `maxCount` comparable to the controls (order 60,000+ px / ~7 %) with a claim near it | ⚠ **the target draws about what it claims** ⇒ a finding **about the target**, not a failure of the cure. `H5` would not be reproduced on that instance |
| **N1** | **`NOT_MEASURED` via `framesNoPass`** | ⛔ **the instrument could not see it — NOT evidence of `H5`.** Would also contradict P27's non-Nanite classification ⇒ report and HALT |
| **N2** | **`NOT_MEASURED` via `framesUnconfirmed` / `framesResidual` / `tagFailed`** | ⛔ instrument-side fault ⇒ NOT evidence of `H5` ⇒ report and HALT |
| **N3** | **`NOT_MEASURED` via `skippedHidden` only** (no qualifying tick) | ⛔ `LOCK-1` found no measurable frame ⇒ ADMIT ⇒ NOT evidence of `H5` |
| 🚨 **X** | **ANY zero or near-zero arriving with ANY bucket non-zero** | 🚨 **NOT YET INTERPRETABLE.** Report the buckets, attribute nothing, do not claim the cure worked. *A zero with clean buckets is the cure working; a zero with a dirty bucket is an unexplained zero, and this milestone has already produced one of those (`P26` event 1)* |

### Required reporting alongside the verdict — not optional, per the ruling

For **every** event of both legs: `state` · `maxCount` · `pctOfFrame` · the event's claimed
`coverage_pct` from `annotation.json` · `arms` · `resolved` · **`framesContributed` ·
`framesDiscarded` · `framesResidual` · `framesUnconfirmed` · `framesNoPass` · `probeArms` ·
`collisions` · `tagFailed`** · and **the per-frame `customStencilExtent` values from the join**
(every armed frame view-sized, or the exceptions named).

### What these legs do NOT establish, declared in advance

- ⛔ **NO INCIDENCE CLAIM.** Two instances on one title says nothing about how often `H5` occurs
  in the client's data.
- ⛔ **`H5` class (i) remains ENUMERATED, NOT OBSERVED** — no instance exists here to run.
- ⛔ **NOT a veto test.** Slice 3 does not exist; nothing is removed from `annotation.json` this
  turn. These legs measure; they do not act.
- ⛔ **NOT a Nanite result.** Both targets are non-Nanite, so these legs say nothing about the
  `G134` limit either way.

---

## Standing conditions for all three legs

`A63` — every attempt banked, discarded ones included. `B1` **NOT APPLICABLE** on all three
(`G117`), declared here rather than after. `P6` measured unchanged on both artifacts
(`annotation.json` 48/48; `run_summary` **+3** — `mask_probe_arms`, `mask_residual_discards`,
`mask_nopass_discards`, the third **APPROVED** by Ruling 1 of this turn).
⛔ CB_GateLevel untouched (`G99`) · stencil range 200/255 · no slice 2, no slice 3, **no tag**.
