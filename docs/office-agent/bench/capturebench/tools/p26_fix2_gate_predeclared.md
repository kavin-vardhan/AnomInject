# P26 — the fault-(ii) FIX GATE, pre-declared BEFORE implementation runs a single leg

**Written and committed before the fixed build exists.** Session 2026-08-20, journal PART
TWENTY-SIX. Implements the PART TWENTY-FIVE design (Option B + enforcing confirmation + probe +
`bRunning` guard) with amendments **A-1** (probe is a gate artefact: default OFF, delivery-inert
by GUARD, unmistakable logging, artifact-attributable, checklisted) and **A-2** (the residual
gets its own counter, reported per leg, and lands in `run_summary`).

## What the build under test changes (so the predictions are falsifiable against it)

- Mask block (verify → drain → collect → arm) moves from `Tick` to `FWorldDelegates::OnWorldTickEnd`
  (guards: world match · `bMaskMeasure` · **`bRunning`** · `Async`).
- Enforcing confirmation, WHITELIST: a frame contributes ONLY if its `OnEndFrame` sample ran and
  read visible. Buckets are DISJOINT per A-2:
  `framesContributed` · `framesDiscarded` (pollution: 255 detector / verify collision) ·
  `framesResidual` (arm accepted, confirmation read HIDDEN — the A-2 residual) ·
  `framesUnconfirmed` (sample never ran — instrument liveness, expected 0) ·
  `probeArms` (probe frames; never share any other bucket).
- `IAI.Capture.MaskProbe` (default OFF): at most ONE deliberate arm per run on a KNOWN-hidden
  tick; **inert in delivery mode by a code guard at the fire site regardless of the flag**;
  every probe line carries `PROBE`; `run_summary` gains `mask_probe_arms`.
- `run_summary` gains exactly TWO fields, always emitted: `mask_probe_arms`,
  `mask_residual_discards`. **Declared here; the artifact key-set check must show +2 in
  run_summary and 0 in annotation.json.** (Precedent: `capture_path`, `key_ring_*` at S4 —
  run_summary is not `P6`.)

## Predictions, per leg (restate verbatim before results)

### L1 — blinking control, `StaticMeshActor_49`, CB_GateLevel, Mask 1 (B1 APPLIES)
- Every full event: `arms=4 resolved=4 framesContributed=4 framesDiscarded=0 framesResidual=0
  framesUnconfirmed=0 probeArms=0 collisions=0 state=MEASURED_NONZERO`, `pctOfFrame` 7.2–7.3.
- Join: every armed frame REAL (view-sized) with `hiddenEof=0`; `M24 ENDFRAME` line count ==
  armed-frame count; `M23 PASS` line count == total arms issued; **no PASS record at mode != 3**
  (the stray post-`FinishRun` arm is gone).
- The final event may truncate at the 90-frame cap; whatever resolves is clean; zero resolved ⇒
  `NOT_MEASURED`, never `MEASURED_ZERO`.
- `run_summary`: `mask_probe_arms=0`, `mask_residual_discards=0`.
- A48 echo: the StartRun log line reads the probe EFFECTIVE state OFF on this leg (flag not set).

### L2 — `missing_texture`, same target/map (B1 APPLIES)
- Unchanged from P24: 8/8 events `4/4 contributed, 0 discarded/residual/unconfirmed,
  collisions=0, MEASURED_NONZERO` ~7.25 %. All REAL, `hiddenEof=0`.

### L3 — `missing_object`, same target/map (B1 APPLIES) — P-2's riskiest path, exercised
- Per full event: **ZERO in-window arms** (every in-window tick renders the target hidden ⇒
  `skippedHidden` only) and **4 post-revert arms** (6-tick window: `SettleAfterRevert` 2 +
  `PostGap` 4; one arm per tick; cap 4), **all 4 contributing**, `MEASURED_NONZERO` ~7.25 %.
- Trailing event(s) whose post-revert window falls beyond the frame cap: fewer arms, possibly
  zero ⇒ **`NOT_MEASURED`, never `MEASURED_ZERO`** (the admit path).
- `collisions=0`, no DUMMY pass records, ENDFRAME count == armed count.

### L4 — the PROBE leg: `missing_object` + `IAI.Capture.MaskProbe 1` (B1 APPLIES) — F-6 item 5
- Exactly ONE probe arm in the run, on a known-hidden tick, logged with `PROBE` in the string.
- The probe frame: PASS record DUMMY (1×1) ⇒ **the 255 detector FIRES** (warning + collision
  recorded, detail names the unassigned tag); the `OnEndFrame` sample reads HIDDEN; the frame is
  bucketed **PROBE** — not residual, not ordinary pollution — and NEVER contributes.
- The probed event: `arms=4 resolved=4 probeArms=1 framesContributed=3 state=MEASURED_NONZERO`.
- `run_summary`: `mask_probe_arms=1` (the artifact attribution A-1 requires).
- All other events per L3. ⇒ **G96 BOTH WAYS on the new binary: the detectors silent on L1–L3
  and firing on demand on L4.**

### L5 — `SM_Ramp2` control, MainWorld, blinking (B1 **NOT APPLICABLE** — G117, declared)
- Mirror of `P20_M26S1_RAMP` config (target `SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137`).
- Every full event `MEASURED_NONZERO`, `collisions=0`, `framesDiscarded=0 framesResidual=0`;
  `pctOfFrame` plausible for the ramp's on-screen size (F-6 item 4 is a plausibility judgement,
  stated as such — no banked rect exists for this target at this pose).
- **A-4 beside the result, restated from the banked measurement: peak-OUT `0.2955` > peak-IN
  `0.1785` — the A35 case. `SM_Ramp2` MUST read non-zero (ADMITTED); in slice 1 nothing can veto,
  so the admission claim is that the MEASUREMENT is non-zero, which is what slice 3 will admit on.**

## F-6 mapping (all five items; STOP AT FIRST FAILURE, in this order)

| item | satisfied by |
|---|---|
| 1 — `StaticMeshActor_49` NON-ZERO, `collisions=0` | L1 |
| 2 — `SM_Ramp2` NON-ZERO, `collisions=0`, A-4 beside | L5 |
| 3 — arm counts match prediction | L1–L5 vs this file |
| 4 — `pctOfFrame` plausible | L1–L4: 7.2–7.3 vs banked 7.80 rect; L5: stated plausibility |
| 5 — the 255 detector proven still LIVE, both ways | L4 (fires on demand) + L1–L3 (silent) |

Order of execution: L1 → L2 → L3 → L4 (= F-6 complete) → L5 (= the SM_Ramp2 control).
**Then the `H5` legs are DECLARED UNBLOCKED and NOT RUN — their predictions belong to their own
pre-declared brief.**

## Failure branches

| branch | reading | action |
|---|---|---|
| **F-A** | any `framesResidual > 0` on ANY bench leg | 🚨 the A-2 residual firing where it should be zero — report the frames, attribute nothing, HALT |
| **F-B** | any `framesUnconfirmed > 0` | instrument liveness failure (G132 rule 2 family) — HALT |
| **F-C** | any DUMMY pass record on L1–L3 / any mode != 3 record on any leg | the fix did not close the read, or the stray arm survives — HALT |
| **F-D** | the probe does NOT fire all three mechanisms on L4 | F-6 item 5 FAILS — the detectors cannot be shown live — HALT |
| **F-E** | probe fires more than once, or fires with delivery ON in the A48 echo check | A-1 violated — HALT |
| **F-F** | key-set check ≠ (annotation 0/0, run_summary exactly +2) | undeclared artifact movement — HALT |
| **F-G** | any other prediction miss | report exactly what was seen, attribute nothing, HALT |
| **F-H** | leg invalid (A63/B1 where applicable) | discard, bank, re-run — how-it-ran, not what-it-showed |
