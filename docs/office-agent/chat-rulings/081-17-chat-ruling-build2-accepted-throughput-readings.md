# 081-17R — chat ruling: build 2 accepted; before the 70-leg requalification, one long-run throughput reading (chat Claude, orchestrator, 2026-09-24 01:15 IST)

## Disposition — GREEN accepted
- **Build 2:** source `9c75abe`, Game **8E64FA39**, Lyra modules `m55-stage2-r17-lyra-48b9e048`; one build set, all exit 0; postflight PASS (`problems: []`), boundary back on `m51`.
- **Stack 1080p twins 4/4 PASS on first attempt:** 20/20 required pairs each, zero `budget_exceeded`, invariants 0; null onsets 0.00004–0.00023 vs control 0.0044–0.0048; solid onsets 1.0 ×10 vs control 0.009–0.013. High-water 51.9 / 64.3 / 66.4 / 70.5 MB with closed census (0 unaccounted bytes).
- **Lyra G270 (300 frames) PASS branch:** first fixture-valid capture (attempt 2; attempt 1 fixture-invalid on a host respawn), 5 events, 20/20 pairs, zero refusals/timeouts.
- **Conduct:** a foreign UE 5.7 editor (<other-home-project>) appeared mid-round; Code refused at its idle check, never touched it, waited 39 min. Correct. Missed predictions reported plainly as readings. Correct.
- **Noted for merge:** the Stage 1–2 source carries comments; the repo's comment-stripper convention must run before merge (merge checklist item). Refusal-time census (diagnostic i) is unexercised at runtime — the G10 and `ChangeMaxBytes 1` legs must show it. Legacy comparisons will see exactly one added summary key, `change_bytes_peak_census` — predeclare it.
- **Product reading, not a gate (for the later calibration unit and the owner's stuck-mip default-on call):** Lyra `stuck_low_mip` onset target vs whole-frame control = 0.584/0.530, 0.027/0.069, 0.00009/0.0013, 0.0011/0.0042, 0.1015/0.0976 — at or below the frame's own background change in four of five events.

## Why one more reading before the requalification
Chat's read of the Lyra census lines (they sample only record-setting moments): cursor lag behind the newest issued index went 12 → 28 (≈ 2 s) → 40 (≈ 5.5 s) → 53 (≈ 11 s), still rising at the end; the head was pending with nothing to wait on at the peaks; issue→colour latency p50 1.19 s on Lyra vs 0.30–0.39 s on StackOBot. A 300-frame run cannot tell a plateau from slow steady growth, and real capture sessions run for thousands of frames. If lag keeps growing, a long 1080p session on a heavy host will start refusing on budget within about a minute, and the stage worker may be taking thread-pool time from the PNG writer. If that needs a scheduling fix, it must land **before** the 70 requalification legs, not after them. So: readings first, no source change.

## Next — brief 081-18 (Code, Opus 5.5, xhigh, fresh session: context was 65 %)
Long-run readings on build 2, evidence ON vs OFF (`IAI.Capture.ChangeEvidence`), no build, no source change, no gates. Chat decides from the readings: (a) plateau and writer unaffected → requalification on build 2; (b) growth or writer slowdown → a scheduling fix as build 3, then requalification.
