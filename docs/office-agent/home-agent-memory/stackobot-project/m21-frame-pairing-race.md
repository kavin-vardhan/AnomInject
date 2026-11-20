---
name: m21-frame-pairing-race
description: "2026-07-15 ROOT CAUSE of the -1 frame shift — the async arm→present pairing is timing-dependent (pacer sleep), NOT delivery mode and NOT content clock; m18's fix is only correct in the paced+sustainable regime"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
---

2026-07-15 INVESTIGATION (report only, no fix, no commit). Base: plugin **`c01a214` = m20**. Supersedes the delivery-mode and content-clock hypotheses. See [[m20-annotation-labeling-investigation]], [[m18-label-misalignment-investigation]].

**THE -1 SHIFT IS REAL AND NOW FULLY REPRODUCED — but neither delivery mode nor the content clock is the variable.**

**Two refuted premises (both owner-supplied, both disproved by measurement + code):**
1. *Delivery mode:* full-fidelity vs delivery on the same scene → BOTH frame-exact. `bDeliveryMode` gates only run.json (:604), the label-RECORD write (:816/:955 → LabelWriter :121/:247) and a run_summary flag (:1255); `AccumulateFrameEvents` (:802) is NOT gated, and the image filename uses the same `Snap->SessionIndex` that feeds the range.
2. *Content clock:* **the dev machine was ALREADY running WALL** — StackOBot's DefaultGame.ini has NO `[AnomalyCapture]` section, and the engine default is `EContentClock::Wall` (m15). Logs/run_summary confirm `clock=wall` on every session I validated. m15's "StackOBot is game-clock → set game via its ini" was a RECOMMENDATION never applied. Code: `ContentClock` only ever reaches `ComputeRunPacing`'s `LastRunPacing.StampedFps` (:1164/:1200) + log lines — it cannot touch a frame index.

**MEASURED TRUTH TABLE (same scene, missing_texture, PNG-filename cross-check):**
| clock | pace | target | speed_ratio | result |
|---|---|---|---|---|
| game | on | 30 | 1.000 | frame-exact |
| wall | on | 30 | 1.000 | frame-exact |
| wall | on | 240 | 4.599 | **−1** |
| game | on | 240 | 5.060 | **−1** |
| wall | **off** | 30 | **0.381** | **−1** |

**ROOT CAUSE: the async arm→present pairing is TIMING-DEPENDENT.** `FAnomalyFrameCapturer::OnBackBufferReadyToPresent` consumes "the NEXT present for this window" (AnomalyFrameCapturer.cpp:67-72) — an undefined frame. When m11's pacer SLEEPS at the top of the tick (pace ON *and* the rate is achievable), the render thread drains and presents frame N-1 during the sleep, so the arm placed in tick N is matched to present(N) → `arm(N) == render(N)` → m18's end-of-tick sample matches. With NO sleep (rate-starved OR pacing off), the arm is placed while N-1 is still pending → matched to present(N-1) → the file's CONTENT lags its index by one → labels/annotation read one EARLY vs pixels. Note run D proves it is not "slowness": ratio 0.381 (running FAST, unpaced) also shifts.

**SCOPE: labels.jsonl is shifted too, not just annotation** (verified on the slow run: labels [3..10] vs pixels [4..11]) — they share one snapshot, so EVERY anomaly and every emitter inherits it. This is a capture-pairing defect, not a labeling defect.

**⚠ m18 IS ONLY CORRECT IN THE PACED+SUSTAINABLE REGIME, and this is the mirror image of the m18 bug:** pre-m18 the label was the mid-tick (= end of tick N-1) state → CORRECT on unpaced/starved boxes, wrong on paced ones; post-m18 it is the end-of-tick-N state → correct on paced boxes, one EARLY on unpaced/starved ones. m18 was validated exactly in the regime where it is right. The owner's office box (loaded Concorde) cannot sustain the target → ratio > 1 → they see the −1 that m18 introduced *for that regime*. Neither sample point is right for both, because the PIXEL SOURCE moves.

**IMMEDIATE WORKAROUND FOR DELIVERIES (no code):** keep pacing ON and lower `IAI.Capture.Fps` until `run_summary.json`'s `speed_ratio` ≈ 1.000 (within the 2% tolerance). `speed_ratio` is ALREADY reported per session and is the exact self-check: ratio ≈ 1.0 ⇒ labels frame-exact; ratio > 1.02 (or pacing off) ⇒ everything shifted −1. Re-capture affected client sessions this way.

**FIX SHAPE (reported, not built):** fix the PAIRING, not the sample point (do NOT re-tune m18/m20 — that just moves which regime breaks). Make the arm carry the identity of the render it expects and have the hook consume it only on THAT present (hold otherwise), instead of "next present wins". The hook has `GFrameNumberRenderThread`; the arm has `GFrameCounter`. Needs its own investigation: the observed `armed frame id=N submitted (rtframe=M)` delta VARIES (1 and 2) on this box, so the GFrameCounter↔GFrameNumberRenderThread offset must be established, not assumed. Rejected alternative: flushing rendering commands before arming = the game-thread stall m19 just removed. Then labels.jsonl + annotation stay consistent by construction (shared snapshot) and the result is independent of pacing, rate, clock and delivery mode.

**ADOPTED VALIDATION PRINCIPLE:** annotation/label gates must run in the CLIENT's regime — delivery mode AND wall clock AND a realistic (non-sustainable) rate. Every prior validation was paced+sustainable, which is precisely the one regime where the bug is invisible. `mode_audit.py` (annotation+PNGs only, no labels.jsonl needed) is the tool.

**2026-07-15 m21 FIX TURN (BUILT, NOT committed; base still `c01a214`=m20).** Owner withdrew the wall-mode brief, approved the pairing direction. **STEP 1 measured:** arm-id→consume-rtframe delta d=2 (99%) at ratio≈1 vs d=1 (100%) starved/pace-off, **mixed 88/12 within one starved run** → fixed-delta impossible, identity/order pairing required. **STEP 2 fix (~18 lines, `AnomalyFrameCapturer.{h,cpp}` ONLY):** `ArmForCapture` now enqueues registration via `ENQUEUE_RENDER_COMMAND` (weak-ptr like EnqueueDrain) → FIFO-ordered between present(N−1) and present(N) → "next present wins" deterministic; `FArm.RegisteredRtFrame` + `armRt=` in the consume log = per-frame proof. Preview tee inherits (shared class). **RESULTS: pace-off FIXED (0/100, was −1); ratio 1.05 exact; ratio≈1 byte-identical to m20; telemetry consume−armRt=0 100/100 in every regime incl. starved.**
**⚠ DEEPER RESIDUAL EXPOSED (→ proposed m22): at DEEP starvation (ratio≳3) the PRESENT ITSELF carries a stale scene.** One-time mid-run slip (consistently ~idx 16–22) → content permanently −1 (missing_texture, R3+R6 delivery) while pairing telemetry stays perfect; worse for render-STATE: blinking@240's 8-tick hide window (game-state proven by annotation) **never appeared in any presented pixel** (frame_00006.png visually confirmed Bot visible) — staleness is change-type-dependent (material swap slips by 1; hide/unhide can miss entirely). No arm-side pairing can fix content the present never contained. **m22 shape = scene-identity marker via minimal SVE** (BeginRenderViewFamily records GFrameCounter; RT hook publishes latest-rendered-scene; present hook consumes arm(N) only when presented scene==N, else marks the frame) + investigate the one-time slip + hide propagation. NOTE align_check's "fire live ⇒ pixels corrupted" assumption is WRONG for blinking (visible phase) — use blink_audit/mode_audit hidden-set comparison there; R7's 65-FP align_check readout was tool artifact, not data.
**SHIP RULE (sharpened, works today): `run_summary.speed_ratio ≤ ~1.05 & paced=true` → session trustworthy; ratio > ~1.05 → lower `IAI.Capture.Fps` + re-capture. Pace-off is now safe for labels post-m21 but keep pacing ON for m11 speed correctness.** Honest gates: G2/G3/G4 + G6(ratio≈1) GREEN; G1/G5 (deep starve) NOT green — deferred to m22 with evidence. New tools: `measure_offset.py`, `pairing_tel.py`. Docs drafted: journal 027, G82, CLAUDE.md → m21.
