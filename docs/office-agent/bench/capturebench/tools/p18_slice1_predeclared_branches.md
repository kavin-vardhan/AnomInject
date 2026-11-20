# PART EIGHTEEN — `m26` SLICE 1 VALIDATION: PRE-DECLARED BRANCHES

**Written and committed BEFORE any leg ran.** Restated verbatim in the journal before any result is
read. A leg is discarded for HOW IT RAN, never for WHAT IT SHOWED (A63); every attempt is banked.

**What slice 1 is:** MEASURE ONLY. `IAI.Capture.Mask 1`. **LOG OUTPUT ONLY — nothing in any
artifact, `mask{provided}` stays `false`, nothing is vetoed.** The point of the slice is stated in
the brief and repeated here because it is the whole reason for the ordering:

> **THE MEASUREMENT MUST BE SHOWN CORRECT BEFORE IT IS TRUSTED TO ACT.**

---

## The four targets, and what each must return

| target | map | expectation | why |
|---|---|---|---|
| `StaticMeshActor_49` | `CB_GateLevel` | **NON-ZERO** | the on-calibration control; a proper hide scores 0.1023–0.1116 |
| `SM_Ramp2` | `MainWorld` | **NON-ZERO** | 🚨 the **A35 case**: peak-OUT **0.2955** > peak-IN **0.1785** |
| `InstancedFoliageActor_0_0_0` | `MainWorld` | **ZERO** | `H5` class (ii), measured: claims **100 %** of the frame, change lives in 4 of 64 cells |
| `BP_SplineSpawn_C` | `MainWorld` | **ZERO** | `H5` class (ii), measured: claims **22.9 %**, peak-in **0.0175** vs the control's **0.5515** |

⚠ **"ZERO" means the MASK COUNT is zero — the target's own silhouette contributed no surviving
pixels. It does NOT mean the actor is invisible.** These two actors draw *something*; the claim under
test is that what they draw is not what their label claims.

---

## Pre-declared branches

| # | branch | reading that selects it | consequence |
|---|---|---|---|
| **S1** | **MEASUREMENT CORRECT ON ALL FOUR** | non-zero on both controls, zero on both `H5` targets | ✅ **Slice 1 passes.** The measurement may be trusted to act in slice 3 |
| **S2** | **CORRECT ON CONTROLS, NOT ON `H5` TARGETS** | non-zero on the controls, **non-zero** on foliage / splinespawn too | the mask measures *something real* but does **not** discriminate — **the cure's premise is wrong as implemented.** Report, do not proceed |
| **S3** | **CORRECT ON `H5` TARGETS, NOT ON CONTROLS** | zero on the `H5` pair **and** zero (or near-zero) on a control | 🚨 **the mask is broken in the dangerous direction** — it would veto good events. HALT |
| **S4** | 🚨 **`NOT_MEASURED` WHERE A QUALIFYING FRAME SHOULD EXIST** | a target that is visible and un-hidden for many ticks still reports `NOT_MEASURED` | 🚨 **THE `LOCK-1` RULE IS FAILING.** This is the dangerous branch. **HALT** |
| **S5** | **MIXED / PARTIAL** | any pattern not covered above | report exactly what was seen, attribute nothing, HALT for a ruling |
| **S6** | **INVALID / NOT MEASURED (leg)** | the leg failed for a HOW-IT-RAN reason — no artifact, focus timeout, harness fault | discard, bank, re-run. **NOT a result.** |

⚠ **`S4` IS THE ONE TO WATCH.** `NOT_MEASURED` is *correct and expected* for a hide-type target with
no un-hidden captured tick — that is `P-2`'s rule working. It is a **FAILURE** only where a
qualifying frame demonstrably existed. **The report must state, per event, WHICH anomaly type
produced `NOT_MEASURED` and WHY**, against `P-2`'s rule:

> non-hide-type ⇒ any in-window tick · `blinking` ⇒ in-window ticks where the target is not hidden ·
> `missing_object` ⇒ **no in-window tick qualifies**, so it falls through to the post-revert window.

---

## Gates that run alongside

| gate | what |
|---|---|
| **G-1** | `P6` unchanged — the 48-field key-set measurement against a pre-`m26` banked leg. **0 added, 0 removed** |
| **G-2** | `annotation.json` field SET unchanged, **both delivery modes** |
| **G-3 (AMENDED)** | **CONTROL PAIR against a build that does NOT contain the shader** — `m25`'s preserved quartet vs `m26`-switch-OFF. Run-unique set established from a same-binary pair FIRST; the test pair's difference set must be a **SUBSET**; **EXTRAS MUST BE 0**. ⚠ *Amended because `G129` made "byte-identical with the switch OFF" unobtainable — a global shader has no switch-OFF state.* **Runs BOTH WAYS (`G96`)** |
| **G-6** | `speed_ratio` in the banked band (≈1.000–1.002 on the `M-1` legs); ring `missed == corrupted == 0` |
| **A-4** | peak-IN and peak-OUT reported **beside** each control's measurement |

---

## Conditions

- **A63 applies.** Every attempt banked, including discards.
- **`B1`** applies on `CB_GateLevel` / `StaticMeshActor_49` / 1280×720; **declared NOT APPLICABLE**
  on every MainWorld leg (`G117`) — **declared, never silently skipped**.
- ⛔ **NO A54 VERDICT ON MAINWORLD TARGETS** — the oracle is scoped to `CB_GateLevel` /
  `StaticMeshActor_49` (`G117`).
- **Marker** is irrelevant to the mask (it is not a per-frame differencer), but peak-IN/peak-OUT for
  `A-4` **must come from marker-OFF data** (`G125`) — the banked PART ELEVEN figures are marker-off
  and are used as the reference rather than re-measured.
- Build identity recorded as a **QUARTET** plus the `.utoc` timestamp (`G121`).

*(Written 2026-08-19, PART EIGHTEEN, before any slice-1 leg ran.)*
