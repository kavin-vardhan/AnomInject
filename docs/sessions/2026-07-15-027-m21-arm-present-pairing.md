# 2026-07-15 — 027 — m21: deterministic arm→present pairing (the real root of the "−1 frame shift")

Base: plugin `c01a214` (tag `m20`, in sync), dashboard `f978f1b` (untouched). **No commit this turn.**

## How we got here — two wrong theories, then the measurement

The owner observed (office box, Concorde-era workflow): annotation `[15..22]` vs actually-corrupted PNGs
`frame_00016..frame_00023` — a uniform −1. Two supplied hypotheses were tested and refuted before this milestone:

1. **Delivery mode** — refuted: full-fidelity vs delivery A/B on the same scene were BOTH frame-exact; `bDeliveryMode`
   gates only run.json, the label-record write and a run_summary flag; `AccumulateFrameEvents` is not gated.
2. **Content clock (wall vs game)** — refuted: the dev machine had been running **wall all along** (no
   `[AnomalyCapture]` ini section; engine default `Wall` since m15), so wall was already validated frame-exact; and
   `ContentClock` only ever reaches `LastRunPacing.StampedFps` (`ComputeRunPacing`, :1164/:1200) — it cannot touch a
   frame index. The A/B (game@240 vs wall@240 both −1; game@30 vs wall@30 both exact) confirmed the clock is inert.

**The real variable is the capture rate regime** — measured truth table (pre-fix, PNG-filename cross-check):

| clock | pace | target | speed_ratio | result |
|---|---|---|---|---|
| game | on | 30 | 1.000 | frame-exact |
| wall | on | 30 | 1.000 | frame-exact |
| wall | on | 240 | 4.599 | −1 |
| game | on | 240 | 5.060 | −1 |
| wall | **off** | 30 | **0.381** | −1 |

Pace-off at ratio 0.381 runs FASTER than target and still shifts — so it is not "slowness"; it is **whether the m11
pacer sleeps before the arm**. And the shift hits **labels.jsonl too** (verified: labels `[3..10]` vs pixels `[4..11]`
on a starved run) — it was never an annotation bug; annotation and labels share one snapshot.

## STEP 1 — the measurement (pre-fix, m20 binary): no fixed arm→present delta exists

Parsed `armed frame id=N submitted (rtframe=M)` across 11 sessions (`measure_offset.py`):

| regime | ratio | delta = rtframe − armId |
|---|---|---|
| paced @30 (8 sessions, wall+game) | ≈1.000 | **d=2 : 99%** (d=1 only ever the first boot frame) |
| wall @240 | 4.599 | **d=1 : 100%** |
| game @240 | 5.060 | **d=1 : 88%, d=2 : 12% — MIXED within one run** |
| pace-off @30 | 0.381 | **d=1 : 100%** |

Three findings kill counter arithmetic: the delta shifts across regimes (2↔1); it correlates exactly with the pixel
pairing (d=2 = correct, d=1 = shifted); and it mixes within a single run. **The pairing must be per-frame by
identity/ordering, never by a fixed offset.**

## STEP 2 — the fix: arm registration rides the render-thread command stream

**Mechanism of the bug:** `ArmForCapture` mutated `PendingArms` directly on the game thread at an arbitrary wall-clock
moment; `OnBackBufferReadyToPresent` consumed the head arm at whatever present happened next in wall-clock time
(`AnomalyFrameCapturer.cpp:67-72`). When the pacer sleeps (ratio≈1), the render thread drains during the sleep and
presents N−1 before the arm → next present = N → correct. With no sleep, present(N−1) fires after the arm → consumed
one present early → every file's content lags its index by one.

**The fix (~18 lines, `AnomalyFrameCapturer.{h,cpp}`):** `ArmForCapture` now enqueues the registration via
`ENQUEUE_RENDER_COMMAND` (weak-ptr guarded, like `EnqueueDrain`). FIFO ordering of the render command stream
guarantees the registration executes after present(N−1)'s broadcast (enqueued during tick N−1) and before present(N)'s
(enqueued at the end of tick N, after the world tick in which we armed). "Next present wins" is thereby deterministic
in every regime. `FArm` gains `RegisteredRtFrame` and the consume log now prints `armRt` alongside `rtframe` — the
per-frame determinism proof. No other files changed; both the capture path and the m19 preview tee inherit the fix
(shared class); `NumPendingApprox` has no consumers and drain semantics are unchanged
(`FlushRenderingCommands` in the drain loop also executes queued registrations).

## Validation — the fix works; and it exposed a DEEPER, deep-starvation-only defect

**Telemetry first: pairing is now deterministic in EVERY regime** — `consume_rtframe − armRt = 0` for 100/100 frames
on the starved missing_texture run AND the starved blinking run. Every arm is consumed on exactly the present it
registered for. Post-fix pixel results:

| run | regime | ratio | pre-fix | post-fix |
|---|---|---|---|---|
| R1 | game@30 paced, full-fid | 1.000 | exact | **exact — 0/100** (labels; pattern/cadence match m20) |
| R8 | game@60 paced | 1.052 | (untested) | **exact — 0/100** |
| R5 | pace-off @30 | 0.393 | **−1 uniform** | **exact — 0/100 ← FIXED** |
| R3 | game@240 paced | 2.984 | −1 uniform | first ~1.5 bursts exact, then a ONE-TIME slip → content −1 thereafter |
| R6 | wall@240 + DELIVERY | 4.023 | −1 uniform | same signature as R3 (event 0 exact, slip mid-burst-1) |
| R7 | blinking @240 | 3.230 | (n/a) | annotation matches game state; **pixels never show the hide at all** |

**The residual, precisely:** in R3/R6 the run *starts* content-aligned, then a single mid-run event (consistently
around idx 16–22) permanently shifts presented content one frame behind its index — with the pairing telemetry still
perfect. Ordering cannot fix content the present never contained: **under deep starvation the presented backbuffer can
carry a stale scene.** R7 sharpens it: an 8-tick `SetActorHiddenInGame` window (game-thread state proven hidden by the
annotation, which is sampled from that state) **never appears in any presented frame** — visually confirmed on
`frame_00006.png` — so render-state changes (hide/unhide) can go stale by MORE than one frame while material swaps
slip by exactly one. The staleness is change-type-dependent, which no arm-side pairing can ever repair.

**Honest gate accounting vs the brief:** G2 (ratio≈1) GREEN; G3 (pace-off) GREEN — the headline repair; G4
(clock-independence) GREEN (game/wall identical at both 30 and 240, pre- and post-fix); G6 at ratio≈1 GREEN (m20
validations stand; this fix does not alter that regime). **G1/G5 (deep starvation, ratio ≳3) NOT green** — materially
improved but a residual scene-vs-present slip remains, now proven to live BELOW the pairing layer. Deep-starved
captures remain unreliable and must not ship (they are self-identifying — see below).

## Ship guidance (unchanged in spirit, now sharper and larger)

`run_summary.json.speed_ratio` is the per-session trust check:
- **ratio ≤ ~1.05, paced** → frame-exact (proven at 1.000 and 1.052).
- **pace OFF** → now also frame-exact (was −1) — but keep pacing ON anyway; it is what makes real-time content play at
  the correct speed (m11).
- **ratio ≳ 1.1–3+** → do not trust the session: lower `IAI.Capture.Fps` until ratio ≈ 1 and re-capture. Audit any
  already-delivered session for `speed_ratio > 1.02`.

## Proposed m22 (not started): scene-identity marker

The residual needs the present to be verifiable against the SCENE it carries, not just against arm order. Shape: a
minimal scene-view-extension (the long-planned Stage-3 SVE machinery; CaptureBench already proved an SVE compiles on
stock 5.1) whose `BeginRenderViewFamily` records `GFrameCounter` and whose render-thread hook publishes
"latest-rendered-scene = tick K" to the capturer; the present hook then consumes arm(N) only when the presented scene
is N — and on mismatch marks the frame rather than mislabeling it. Also in scope for m22: why the one-time slip
happens (present-without-fresh-scene vs scene-without-present) and the hide-propagation staleness R7 exposed.

## Files touched

Plugin: `AnomalyCapture/Private/AnomalyFrameCapturer.h` (+1 field), `AnomalyCapture/Private/AnomalyFrameCapturer.cpp`
(registration via render command + telemetry in the consume log). Docs: this journal, `gotchas.md` (G82),
`CLAUDE.md`. **Untouched:** m18's label stamp, m20's ranges/subtype, the view ring/L=0, phase timing, settle-K, the
sync path, the m19 tee logic (it inherits the fix through the shared class), IAnomaly/injector/anomalies. No new
dependency; catalog stays 8. Dashboard untouched.

## State

Code + docs written; validation matrix run in a local package. **No commit.** On owner review: commit as
`fix(capture)` + tag `m21`, with m22 (scene-identity marker + deep-starvation reliability) as the follow-up.
