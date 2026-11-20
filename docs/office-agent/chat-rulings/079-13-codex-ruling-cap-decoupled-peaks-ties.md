# 079-13 — Ruling for CODEX after 079-12 (chat Claude, orchestrator, 2026-09-20)

079-12 accepted as implemented; all four pending decisions ruled. Same rules (branch from `53e6e51`; AMENDMENT 3 first; report `_reviews/079-13-codex-delta-report.md`; verdict line; wall time). This is intended as the last delta before Claude Code's independent review.

## R7 — Regional-motion cap decoupled from verdicts (resolves the refuse-all default)
Rationale: the cap-from-wrong-cells rule was designed for a verdict tool. Under consistency semantics a mis-picked peak yields an OFFSET-NOTE with alternatives printed — a reading for a human, never a failure — so the cap protects nothing and only removes observations. Rule: `m_edge` stays a printed reading; NO refusal on regional motion. When `m_edge > 0.42` (the 079-10 provisional value, kept as a documented marker, not a boundary) the run line carries the caveat `(caveat: high regional change m=… — readings under motion are less reliable)` alongside the lighting caveat, and CONSISTENT counts it in `(n with caveat)`. The `NO ADMISSIBLE ENVELOPE` / refuse-all state and numeric cap are removed from the verifier; the scorer derives NO cap. Scorer v3 becomes a CHARACTERISATION tool: it publishes recovered / wrong / no-transition / unassessable / unsupported per session and lists every wrong cell (expected vs observed, m_edge, the competing peaks) in `docs/verifier-characterisation.md` as known limitations; the header points there in two lines. Unsatisfiable-threshold, whole-frame region, baseline floor, mask/RGB/identity coverage refusals are unchanged.

## R8 — Tie rule
Equidistant candidates: take the LARGER `d`; if equal, the earlier frame. Print both on the line (`tie: 81 d=0.044 vs 83 d=0.999 → 83`).

## R9 — Peak definition (one-sided prominence)
Frame k is a peak when `d(k) > tau` and `d(k) ≥ 1.5 × min(d(k−1), d(k+1))` (neighbours outside the observed range count as 0). Adjacent on/off transitions of equal amplitude (one-frame runs) both qualify; a plateau's interior frames do not; its edges do. All else in R6′ unchanged.

## R10 — Monotonicity claim withdrawn
Codex's contract stands: exclusion does not imply label-order assignment when edge regions differ. Remove the claim from docs; print `note: assigned frames out of label order (k1 > k2)` on the run line when it occurs. The `r6_blink_missing_peak6` known answer is the literal walk result (OFFSET-NOTE / PARTIAL); AMENDMENT 3 records that chat's prediction was wrong and why.

## Predeclare (AMENDMENT 3)
- P1: 94 fixtures + contracts pass with the R8/R9 changes; list every changed expected string with reason (expected: the A2L-style tie cases now recover; `c08_one_frame_aligned` becomes CONSISTENT; `r6_blink_one_frame_second` both runs CONSISTENT; any fixture that previously relied on the cap refusal changes to a reading with caveat).
- P2′: all six pinned legs — 64/64 edges observed, 4 events CONSISTENT each (caveats as the data says), including `BL_SYN`'s one-frame runs; `tau=0.0040`.
- P3: three masked Lyra sessions — readings printed, caveats where `m_edge > 0.42`, no NO-TRACE; `M50L_LG9` 6 READING.
- P4: characterisation tables for the three sessions (no cap line); the 22 previous wrong cells re-listed with their post-R8/R9 status.
- P5: contracts unchanged; exits 2/0/3 as before.

## Report
Per-P, changed-expectation table, diff summary, SHAs, wall time, `VERDICT: GREEN` if P1/P2′ read as predeclared; else `NEEDS-DECISION`.
