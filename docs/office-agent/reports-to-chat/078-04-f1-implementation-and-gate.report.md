# REPORT for 078-04-f1-implementation-and-gate.md
verdict: GREEN (derived) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 078-04-f1-implementation-and-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-06 16:48:18 | finished: 2026-09-06 17:08:05 | duration: 20 min | exit code: 0
context at last turn: 893503 tokens (89% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\078-04-f1-implementation-and-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
The build is linking. While it finishes, here's where the campaign stands:

**F1 implemented and committed** (`6713f51`, 3 files, +41/−11, confined to F1). **Predictions committed before any leg** (`53d42c2`), plus a pre-declared amendment (`09c636f`) after a premise failed.

**Two premise corrections, both recorded before any verdict was read:**

1. **`bSveCapture` must NOT enter the effective predicate.** The target mask is a *separate* scene view extension with its own request id (`AnomalyCaptureSubsystem.cpp:1159`), created on `bMaskMeasure` alone (`:3712`), active whenever armed. It never consults the colour grab point. Adding `bSveCapture` would stop *arming* masks on the backbuffer path where they work — a silent regression on a shipped path, justified by an unverified mechanism. And it is unmeasured in both directions: every banked backbuffer leg predates `m43`, so backbuffer × target-mask has **never run**. Left as-is; **M-F1b stays OPEN**.

2. **The released `OutputHeight 360` recipe cannot pass B1.** It halves the written frame to 640×360, and `bbox_px` lives in written-frame pixel space while `CALIB_BBOX` is frozen at 1280×720. The runner's own discriminator printed ratio `(None, 0.5, 0.4998, 0.5)` with `modal_rot (0,0,0)`, `distinct=1` — **resolution scope, not pose** (`G117`). I did not weaken B1. I kept that arm as a reported reading with B1 declared NOT APPLICABLE, and pre-declared a second trigger — `TargetMask 1` + `Mask 0` — which reaches the identical F1 code path at native resolution, so every gate stays live.

**A-side is fully characterised — 10 runs, all identical:**

| | value |
|---|---|
| frames written | **33 of 90 armed** |
| dropped at finalize | **57** (33+57=90 exactly) |
| events | **3** |
| `NEVER RESOLVED` warnings | 33 |
| `target_mask_frames_unavailable` | 33 (== written, as derived) |

Successive holds are **exactly 4 engine frames apart** (`[8]`, `[12]`, `[16]`) — the subsystem-wide counter holding 4 ticks, releasing one frame, resetting. Mechanism confirmed to the tick, in both orders and at both resolutions.

⚠ Two things I'll flag rather than smooth: my derived band was **18–30 written frames and the measurement is 33** — the mechanism was exact, the rate arithmetic was pessimistic because the FSM spends more engine ticks than armed frames. And the A-side artifact is *internally* consistent (no holes, no orphans, no duplicates) — you can only see the loss from the log's "57 dropped" line. That is precisely why this defect could ship unnoticed.

Still to run: the candidate build, the four B-side legs, and the ONSET/PAIRING/G7 smoke. No merge-readiness claim attaches to any of this.
api-equivalent cost (not billed on Max): $34.44 | turns: 73 | running total across all briefs: $1,166.59
