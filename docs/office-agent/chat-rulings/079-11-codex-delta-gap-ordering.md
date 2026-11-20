# 079-11 — Delta for CODEX: ordered assignment of transitions in inter-run gaps (chat Claude, orchestrator, 2026-09-20)

079-10 accepted; P2 miss resolved by R6 below. Same rules (branch only from `e6351ce`; AMENDMENT 2 in the predictions file first; report `_reviews/079-11-codex-delta-report.md`; verdict line; wall time).

## R6 — Gap ordering (replaces "claimed by two edges" ONLY inside inter-run gaps of the same target)
For two consecutive labelled runs A=[a0..a1] and B=[b0..b1] of the same target (any mode), the gap G = (a1, b0). Edges facing each other: A.end and B.onset. Rule:
1. Collect the dominant above-threshold transitions inside G ∪ both facing windows (each transition frame counted once).
2. If ≥ 2 transitions: A.end takes the EARLIEST, B.onset takes the LATEST; any transitions between them are ignored for these two edges (printed as `unassigned transition k in gap`). Both edges become normal TRANSITION observations with their deltas (so OFFSET-NOTE/CONSISTENT apply).
3. If exactly 1 transition: both facing edges `UNASSESSABLE(gap holds 1 transition for 2 labelled edges)`.
4. If 0: both facing edges search their own windows as before; NO-TRANSITION applies.
5. Non-facing edges (A.onset, B.end) unchanged. The "claimed by two edges" rule still applies everywhere else (different targets overlapping in one window, or a single run's own onset and end competing for one frame — a one-frame run whose windows overlap keeps the existing rule).
Disclose on the run line: `gap ordering applied (k1→A.end, k2→B.onset)`.

## Predeclare (AMENDMENT 2)
P2′: both blink legs `M49_GEDGE_BL_NAT` / `BL_SYN` — every edge observed, 4 events CONSISTENT (caveat as the data says), `tau=0.0040`; the other four legs byte-identical to 079-10. Self-test: add a two-run blink fixture (runs [4..5],[8..9], true transitions at 6 and 8) ⇒ CONSISTENT; the same with the second onset labelled late (9) ⇒ OFFSET-NOTE(−1 on B.onset); the same with only ONE transition in the gap ⇒ both facing edges UNASSESSABLE; the 079-07 `15→16` fixture keeps its answer (state which branch of R6 it falls under and why the answer does not change — if it does change, say so and predeclare the new one). Bank re-run for the three masked Lyra sessions (blink events there, if any, are the ones to watch); scorer sweep re-run; the cap may move — report it, don't tune it.

## Report
Per-P readings, diff summary, SHAs, wall time, `VERDICT: GREEN` if P2′ and the self-test read as predeclared; `NEEDS-DECISION` otherwise.
