# 2026-07-15 — 024 — m18: burst-boundary label alignment (async label stamp → end of tick)

Base: plugin `e2c6dd2` (tag `m17`, in sync with origin), dashboard `f978f1b` (untouched).
Investigation turn preceded this (same day); this journal covers the fix. **No commit this turn** — owner review first.

## Goal

Fix the burst-boundary label misalignment found while validating m17: ~17% of frames in the default burst config
carry a label that contradicts their own pixels. Same class as the m17 revert bug — it silently poisons the
training dataset — and it is **not anomaly-specific** (it hits every anomaly type).

## Root cause — stated definitively (it is an attribution off-by-one, NOT a render-lag inconsistency)

The hypothesis space had two candidates: **(a)** the label side failing to apply the same 1-frame render lag that
`ViewLagFrames=0` applies on the capture side, or **(b)** a plain off-by-one at the positive-span boundary — the span
attributed from the phase-change frame's PRE-change state instead of the state that frame actually renders.

**It is (b).** Not (a), not a combination. Two independent reasons:
1. **The render thread's lag is a THREAD lag, not a STATE lag.** A change made at the end of tick N lands in frame N's
   OWN render — proven by pixels (the fire-edge frame shows the anomaly). The premise behind (a) ("Apply lands on N, the
   pixels change on N+1") is simply false here, and was refuted by measurement before any code was written.
2. **`ViewLagFrames=0` applies no compensation for the label side to mirror.** L=0 means "use the view sampled this
   tick"; it is correct on the sync path only because that view already IS the previous frame's camera — a natural lag,
   not an applied one. The label side used the same no-compensation approach and is likewise correct on sync.
   The fix contains **no lag arithmetic at all**.

The span, note, is not computed by index arithmetic anywhere — it EMERGES from per-frame state sampling
(`AnomalyLabelWriter.cpp:51`, `anomaly_present = Fires.Num() > 0`). So "the boundary index was off" is true only as a
description of the OUTCOME; the CAUSE is that the boundary frame was sampled at the wrong point within its tick.

**One precision worth keeping:** the same mid-tick sampling point is **correct on sync and wrong on async**, because the
two capture paths return different rendered frames (sync `ReadPixels` = the previously presented frame N-1; the async
backbuffer grab = the arm tick's own frame N). So the bug is async-only — but that is a sampling-point/capture-path
mismatch, still (b), and still not a lag-compensation inconsistency.

## The mechanism (pinned in the investigation turn, re-confirmed here)

`CaptureCurrentFrame()` sampled the label from `Auto->GetLiveFires()` at **mid-tick N**; the phase machine then
calls `BeginFire()`/`BeginRevert()` **later in that same `Tick`**. The **async** capture (default since the
Stage-1 backbuffer work) grabs the render of the **arm tick itself** (frame N), i.e. the POST-transition world.
So the frame armed on a transition tick showed the new state while its label described the old one.

**Measured, packaged StackOBot (session_20260715-121622, m17 binary):** pixels positive on frames **[3..10]**,
labels positive on **[4..11]** → the label span ran exactly **one frame LATE** at both boundaries:
- **fire edge** (frame 3): labeled clean, pixels already show the checker = **false negative / contaminated negative**;
- **revert edge** (frame 11): labeled positive, pixels already clean = **false positive**.
17/100 frames (9 FN + 8 FP over 8 bursts). Steady-state fraction = **2 / (PositiveFrames + PostFrames)** = 16.7%
at defaults; independent of SettleFrames (settle ticks capture nothing) and PreFrames (lead-in happens once).

### ⚠ Direction correction vs the m18 brief

The brief's stated diagnosis was "Apply lands on frame N but the pixels change on frame N+1 → frame N is labeled
anomaly-present while its pixels are still clean → shift the span one frame LATER". **The measured data is the exact
inverse at both edges** (re-verified this turn before writing code), so the fix shifts the span one frame **EARLIER**.
The brief asked for the direction to be confirmed against the finding — it was, and it inverted. Root of the
confusion: the render thread's 1-frame lag is a **thread** lag, not a **state** lag — a change made at the end of
tick N still lands in frame N's own render (this is precisely why frame 3 shows the checker). Candidate (d) from the
investigation was explicitly refuted by pixels. Had the span been shifted "later", the error would have doubled to
2 frames at each boundary instead of being fixed.

### Why sync is NOT affected (and must not be shifted)

Sync `ReadPixels` returns the **previously presented** frame N-1, which pairs correctly with a mid-tick-N stamp —
this is the same fact the validated L=0 derivation rests on (architecture.md: "exactly the view that rendered the
`ReadPixels` frame; the two 1-frame lags cancel", G41). So the fix is **async-only**; applying it to sync would
break a currently-correct path.

## What was done (locked lever honoured: label attribution only)

Only *when the label's fire state is sampled* changed. `AnomalyCaptureSubsystem.{h,cpp}` only.

- `CaptureCurrentFrame()` (async branch): builds the snapshot and arms the capturer **without** `Fires`/`FireHidden`/
  `FirePos`, and records `ArmedLabelFrameId` + `bHasArmedLabel`.
- **New `FinalizeArmedLabel()`**: looks the snapshot up by arm id and fills in `Fires` (`GetLiveFires()`),
  `FireHidden` (`FActor->IsHidden()`) and `FirePos` — sampled **after** the transition.
- `Tick()`: calls `FinalizeArmedLabel()` as its **last statement, after the phase switch**, so the sample describes
  the end-of-tick world = exactly what that frame rendered.

**Hide vs non-hide (requirement 2) needs no special-casing — by construction.** `anomaly_present`
(`AnomalyLabelWriter.cpp:51`), the per-anomaly bbox + `bbox_valid` (:65-69), `AffectedFrames`
(`AnomalyCaptureSubsystem.cpp:1242`, the non-hide `frame_indices`) and `HiddenIndices` (:1251, the hide-type
`frame_indices`) **all derive from the same `Snap.Fires`/`FireHidden` sample**, and the gating rule
(:1280-1281, `bHideType = HiddenIndices.Num() > 0`) is untouched. Shifting the one sample shifts every surface
coherently: non-hide keeps a full live/bbox_valid span (verified non-empty), hide-type keeps the render-hidden set
(`missing_object` hides inside `BeginFire`, i.e. at end of tick — now correctly attributed to the frame that renders
hidden).

**`visible_positive` / `bbox_valid` coherence (requirement 3)** likewise falls out: a frame corrected to negative has
an empty `Fires` → `anomaly_present=false` → no anomalies array → no bbox; a frame corrected to positive gets its
entry and its projected box. Verified: `verify_capture.py` reports `present=True: 65 | visible-positive: 65 |
present-but-off-screen (no box): 0`.

**Consistency (requirement 4), stated precisely:** the label's **state** axis now uses the same model as the async
capture (frame N's content == end-of-tick-N world) — that residual off-by-one is gone. The **view** axis was left
alone as instructed (do not touch L=0): the async grab is camera N while the ring yields camera N-1, so the bbox is
*predicted* one frame stale under camera motion. Unmeasured — the validation scene's camera is static — and recorded
as the open half of the same root cause (G78; G41 reserved `IAI.Capture.ViewLag` for exactly this).

## Files touched

Plugin: `AnomalyCapture/Public/AnomalyCaptureSubsystem.h` (2 members + 1 decl),
`AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` (arm branch, new `FinalizeArmedLabel()`, one call in `Tick`).
Docs: `architecture.md` (label-stamp timing + the async L caveat), `gotchas.md` (G78), this journal, `CLAUDE.md`.
**NOT touched:** phase timing, settle-K, `SampleViewThisTick`/`ProjectionView`/`ViewLagFrames`, the sync path,
`IAnomaly`/injector/anomalies. No new dependency; catalog stays 8.

## Gates — all in a LOCAL PACKAGE (`Builds\MidRepro\Windows`), headless via the control server's WS surface

- **G1 boundary correctness — GREEN.** Non-hide (`missing_texture` on the visible `SkeletalMeshActor_3`): fire edge
  frame 3 is now `POSITIVE` and its pixels show the checker (region diff 22.3 vs ~1.1 clean); revert edge frame 11 is
  now `clean` with clean pixels (diff 2.8). Label span [3..10] == pixel span [3..10]. Visual confirmation via
  `verify_capture.py`: `frame_00003_annotated.png` shows the checkered Bot **with** the box + label (pre-fix: labeled
  clean, no box); `frame_00011_annotated.png` shows the restored Bot with **no** box (pre-fix: red box on a clean Bot).
- **G2 scale — GREEN.** `align_check.py` over the full 100-frame, 8-burst run: **0 mismatches** (was 17: 9 FN + 8 FP).
- **G3 hide-type — GREEN.** `missing_object` on the same actor: **0/100 mismatches**; `annotation.json`
  `affected_frames` = `start 3, end 10, count 8, frame_indices [3..10]` = the actually-hidden frames.
- **G4 non-hide — GREEN.** `missing_texture` `affected_frames` = `start 3, end 10, count 8, frame_indices [3..10]`,
  non-empty, matching the pixel-corrupted span.
- **G5 regression — GREEN.** Captured-frame **cadence is byte-identical** pre/post fix (gframe deltas
  `0,1,2,3,6,7,8,9,10,11,12,13,16,17,18,19` in both — the 3-gaps ARE the K=2 settle windows), proving phase timing and
  settle-K were not touched; only the label pattern moved, by one frame, same span width (8). Positive count
  64 → 65 = exactly +9 false-negatives corrected − 8 false-positives corrected, reconciling with the 17 pre-fix
  mismatches. Non-boundary frames' labels unchanged. Sync path and L=0 untouched in code.
- **G6 overlay — GREEN.** `verify_capture.py` runs clean on the corrected session: 100 frames, 65 valid boxes,
  `present=True: 65 | visible-positive: 65 | present-but-off-screen: 0`, `present` flips at the corrected boundaries.

## Residual-offset ruling — do the labels match REAL PIXELS, or are they only self-consistent?

The question that had to be answered before commit: if a render-lag effect were real AND separate from the span bug,
a SECOND off-by-one could survive. **Ruled OUT on the label/span axis**, and the ruling is pixel-truth, not index math:
`align_check.py` decodes the actual captured PNG bytes and classifies each frame from the pixels inside the labelled
`bbox_norm`, then compares that to the frame's own `anomaly_present`. Post-fix: **0 mismatches / 100 frames** — every
positive-labelled frame really is corrupted on screen and every clean-labelled frame really is clean.
- **First positive-labelled frame (idx 3)** — pixels region-diff **22.3** vs ~1.1 for a clean frame → the anomaly IS on
  screen. Visually confirmed: `annotated/frame_00003_annotated.png` shows the checkered Bot with the box + label.
- **First clean-labelled frame after revert (idx 11)** — diff **2.8** (vs ~23 while corrupted) → the anomaly is gone.
  Visually confirmed: `annotated/frame_00011_annotated.png` shows the restored Bot with no box.
The same evidence also kills the "second bug" premise: the hypothesised render-lag effect (pixels changing on N+1) is
**not real** — if it were, the first positive-labelled frame would show clean pixels. It shows the checker.

**Separate axis, still open, NOT a span offset:** the bbox POSITION under camera motion (the async grab is camera N while
the view ring yields camera N-1). It cannot move a frame between positive and negative — it only shifts where the box is
drawn — and it is unmeasured because the validation scene's camera is static. Excluded from m18 by the locked scope
(do not touch L=0). Recorded in G78 + architecture.md.

## Open / not addressed here

- **Async view (bbox) staleness** — the other half of the same root cause; needs a moving-camera capture to measure.
- **bbox from LIVE actor bounds at record-build time** (`BuildFrameLabelRecord`) — several ticks after the arm on the
  async path; harmless for static targets, wrong for moving ones. Related family, unfixed.
- **`blinking` toggle edges** — it toggles inside the injector subsystem's own tick; two `UTickableWorldSubsystem`s have
  no guaranteed relative order, so blink edges may be misattributed independently of burst boundaries. Unmeasured.
- **Sync capture writes BLACK frames in a packaged build** (investigation finding): `IAI.Capture.Async 0` + package →
  every PNG mean 0.00, same `ReadPixels` root as the m19 preview bug. Async is the only working packaged capture path.

## State

Code + docs written; G1–G6 green in a local package. **No commit.** On owner acceptance: strip comments (source is
already comment-free), commit as one `fix(capture)` + tag `m18`. The m17-Concorde-CONFIRMED docs flip from the
previous turn is still uncommitted in the tree and rides along with this commit. m19 (preview re-plumb) follows.
