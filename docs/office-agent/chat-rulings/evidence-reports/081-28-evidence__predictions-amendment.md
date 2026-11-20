
## AMENDMENT 3 — 081-28, 2026-09-25: the G270 vacuity guard (dated, pre-run)

Authority: `_reviews/081-27-chat-ruling-g270-vacuity-guard.md` (chat Claude, 2026-09-25 13:00 IST). **Written before C3's Lyra G270 leg
ran on build 3**: `081-23-evidence/c3` is empty and the campaign ledgers hold no C3 row. A pre-run amendment, not a refit. Everything not
named here is unchanged, including AMENDMENT 2's G270 pair rule and §3.4's stop branch.

- **G270 — adds to AMENDMENT 2's PASS rule.** PASS **additionally requires ≥ 3 events each with at least one measured required pair**
  (an event counts when the sum of `pairs_measured` over its phases is > 0). If the pipeline is clean — no `closure_timeout`, no
  `budget_exceeded`, no other refusal, phase accounting complete, invariants 0, census closed, oracle clean — but fewer than 3 events
  have a measured required pair, the result is the existing **non-blocking OBSERVED-BELOW-EVENT-FLOOR** branch of 081-14 §3.4, **never
  PASS**. Why: under AMENDMENT 2 alone a leg whose required pairs were all `empty_region` would PASS with nothing measured (the oracle
  compares 0 rows, and the audit's "no measured pair" error is filtered for G270) — `G146`'s vacuous zero. Every stop branch still
  precedes this floor. Encoded in `081-23-lib.lyra_g270_eval`; the result gains `measured_events` as a reading.

**Proof both ways** (`_reviews/081-28-evidence/guard-proof.json`, 12 of 12 as expected): PASS on c0 L1 (50 events, 45 with a measured
pair, oracle exit 0 mismatched 0), c0 L4 (42 of 42), 081-17 LYRA_G3_N_A2 (build 2, 5 of 5), an unmodified-content L1 copy, and an L1 copy
with exactly 3 events keeping a measured pair (the boundary); OBSERVED-BELOW-EVENT-FLOOR on an L1 copy with every required pair relabelled
`empty_region` (rows and phases, 0 measured events) and on a copy where only 2 events keep a measured pair; NEEDS-DECISION on the 081-27
`closure_timeout` and `budget_exceeded` mutations (with and without a closed census and its log line), on L4 with a `BUDGET-EXCEEDED`
line, and on the all-`empty_region` copy plus one `closure_timeout` (the stop precedes the floor).

**Dry run** (081-27 wrapper redirected to `_reviews/081-28-evidence/dryrun/`): **153 of 153 as expected** — the 150 checks of 081-27 with
identical outcomes (two G270 PASS `why` strings gain the measured-event count) plus 3 new; 081-23, 081-26 and 081-27 evidence
byte-unchanged. **Boundary** re-issued for exactly `081-23-lib.py` and `feature_head`: `_reviews/081-28-evidence/boundary-diff.json`.
