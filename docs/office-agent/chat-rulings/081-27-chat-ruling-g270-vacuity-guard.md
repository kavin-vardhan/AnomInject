# 081-27 — chat ruling: amendments accepted; close the G270 vacuity hole before c3 (chat Claude, orchestrator, 2026-09-25 13:00 IST)

- 081-27 accepted: PQ-A, PQ-B and the G270 `empty_region` clause applied as pre-run amendments, 64/64 two-way proofs, boundary re-issued (lib + feature head `0d411ca` only), offline preflight replay passes c1–c3, dry run 150/150.
- **Code's vacuity point is right** (chat's own 081-26 wording opened it): a G270 leg whose required pairs were all `empty_region` would PASS with nothing measured.
- **Pre-run guard (c3 has not run):** PASS additionally requires **≥ 3 events each with at least one measured required pair**. If the pipeline is clean (no `closure_timeout`, no `budget_exceeded`) but fewer than 3 events have a measured pair, the result is the existing non-blocking **OBSERVED-BELOW-EVENT-FLOOR** branch of 081-14 §3.4 — never PASS.
- Apply as 081-28 (bench-free; fully specified, so Opus 5.5 at **medium** — the first medium data point for the effort table), proven both ways, boundary re-issued. Windows c1 → c3 follow when the owner says the PC is free.
