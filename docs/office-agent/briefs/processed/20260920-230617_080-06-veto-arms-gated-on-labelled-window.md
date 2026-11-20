# SESSION: new-default
# NAME: 080 m52 follow-up: veto arm gating
# MODEL: claude-opus-5
# EFFORT: high

# 080-06 — gate the m26 veto's arms on the labelled window for deferred-onset anomalies; log the reduce table. Small unit, then merge.

## 0. Bootstrap
CLAUDE.md on master `ce4d1f4`; journal 080-05 §2 (the diagnosis with citations); `AnomalyMaskMeasure.{h,cpp}`, `AnomalyCaptureSubsystem.cpp` at the cited lines. Branch `fix/veto-arm-window` off master. `m51` untouched.

## 1. Change (as sized in 080-05)
- `ArmIfMeasurable`: for records whose anomaly `HasDeferredOnset()`, arms are issued only on ticks inside the event's labelled window (first held frame onward), never in the pre-roll; `MaxArmsPerEvent` budget counts from the first eligible tick. Non-deferred anomalies unchanged (byte-identical behaviour — prove with the A/B below).
- Logging: when the veto reads `R.MaxCount`, print one line per event: `M26 VETO-READ tag=… arms=… resolved=… maxCount=… counts=[per-arm counts]` so a future zero is interpretable (the reduce-table contents Code found logged nowhere).
- Bench-only can-fail lever `IAI.Bench.VetoArmUngated 1` restoring the old ungated arms.

## 2. Gates (predictions first, in the journal)
- G-A/B: `blinking` and `lod_popping` legs on the new binary vs the archived `5588F6FB`: `M26 VETO-READ` arm ids and `vetoed_events` identical (non-deferred path untouched).
- G-C: `stuck_low_mip` auto-pool leg (same recipe as 080-04 G11): every `M23 ARM` id for each stuck-mip event lies inside that event's `injected_frames`; `vetoed_events 0` where every row has `target_pixels > 0`; `NO-TRACE 0` on the verifier.
- G-D can-fail: lever ON reproduces a pre-roll arm set (ids in the 4 ticks after fire) — and if a veto recurs, the new log line shows its per-arm counts.
- G-E: both tick orders identical on G-C.
Build/stage/hash/archive (predecessor `5588F6FB` verified at its archive). Then `--no-ff` merge to master, push, no tag; CLAUDE.md status line; gotcha (deferred-onset anomalies and any per-event evidence budget: the budget must start at onset, not at fire); journal. `VERDICT: GREEN` alone on the last line.
