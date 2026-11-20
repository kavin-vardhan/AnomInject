# 079-10 — Delta spec for CODEX after 079-09 (chat Claude, orchestrator, 2026-09-20)

079-09 accepted as implemented; NEEDS-DECISION resolved below. Same ground rules as 079-09 (branch only, from `7d5b6e9`; no m51/master/Source; predictions first as AMENDMENT 1 of `2026-09-20-verifier-consistency-semantics.md`; report to `_reviews/079-10-codex-delta-report.md`; end with a verdict line; record wall time).

## R1 — Boundary masks (resolves P2/P4)
The producer writes masks only on labelled (in-span) frames (m44; confirmed 079-03). Rule: when a frame in an edge window or in the baseline has NO mask for the target, use the mask of the NEAREST labelled frame of the same run (onset frame for pre-span frames, end frame for post-span frames). Constraints: extrapolation distance ≤ `edge_window + N_BASE` frames from the span; every observation computed on an extrapolated mask carries `(mask extrapolated from frame s, Δ=n)`; the run line summarises `masks: in-span actual, edges extrapolated ≤Δmax`. A frame INSIDE the span with no mask is still `UNASSESSABLE(mask missing at frame t)` and still blocks NO-TRACE (unchanged). NO-TRACE with extrapolated edge pairs is permitted and its line adds `edge pairs use extrapolated masks`. This is not 079-08 F2's boundary-mask reuse: there, real per-frame masks existed and were ignored; here, none exist. State exactly that in the header and G-note.
Then re-run: pinned bench ×6 (expected: every edge observed, `tau=0.0040`, all 4 events CONSISTENT, `NO FAILURE FOUND (4 consistent …)`); the three masked Lyra sessions (print all lines; no NO-TRACE expected); scorer v3 sweep (cells now score; cap re-derived or `NO ADMISSIBLE ENVELOPE` — whatever the data says, no tuning).

## R2 — Lighting coincidence (resolves P1)
CONSISTENT stands when a zero-delta transition coincides with a whole-region change. Print the caveat on the RUN line as well: `CONSISTENT (caveat: edge coincides with a whole-region change — inspect frames k−1..k+1)`. Counts: `CONSISTENT n (c with caveat)`. AMENDMENT 1 records that P1's "never CONSISTENT" expectation was wrong and why.

## R3 — NO-TRACE is thresholded absence (resolves P6)
Wording: `NO-TRACE — no change above the noise floor (tau=…) on this target across frames s..e; the label claims a visible change`. README and header say the same and give the detection limit as a fraction of the target's pixels. P6's "wrong NO-TRACE" class is redefined: a NO-TRACE where any in-span or edge-window pair exceeded tau on the target. `c07_below_tau_masked` keeps NO-TRACE as its known answer.

## R4 — Event aggregation from run outcomes only
An UNASSESSABLE run is unassessable at the event level regardless of its edges; event token = worst-of {NO-TRACE > OFFSET-NOTE > PARTIAL > CONSISTENT} over runs, with UNASSESSABLE/READING runs counted in the coverage numbers only. Fix the `blank_missing_interior` expectation accordingly (event UNASSESSABLE).

## R5 — Keep
Class allowlist as a producer contract (documented as such); inclusive label-only bounds (coverage cost accepted and printed); RGB-channel differencing; endpoint-extent handling; batch/exit contracts.

## Report
Per-P readings with exact lines (P2 bench, P3 Lyra, P4 scorer tables + cap or NO ADMISSIBLE ENVELOPE, P1 self-test count), the diff summary, commit SHAs, wall time. `VERDICT: GREEN` if the bench reads as R1 predicts and the self-test passes; `NEEDS-DECISION` otherwise.
