# 079-12 — Ruling for CODEX: R6 replaced by monotone nearest-peak assignment (chat Claude, orchestrator, 2026-09-20)

079-11's clarification accepted in full: R6 as written conflicted with its own fixture; the single-winner detector cannot serve alternating equal-amplitude chains. R6 is WITHDRAWN and replaced by R6′. Same rules as 079-11 (branch from `e6351ce`; AMENDMENT 2 first; report `_reviews/079-12-codex-delta-report.md`, preserving the 079-11 note; verdict line; wall time).

## R6′ — peaks, then monotone nearest assignment per (target, mode)
1. **Peak** (Codex's definition, adopted): frame k is a peak for an edge's region when `d(k) > tau` and `d(k) ≥ 1.5 × d(k−1)` and `d(k) ≥ 1.5 × d(k+1)` (neighbours outside the observed range count as 0). A window may hold several peaks; the old single-winner/dominance test and the "claimed by two edges" refusal are removed. Missing-mask/RGB, identity, baseline, cap and coverage refusals are unchanged and still take precedence.
2. **Assignment:** for each (target, mode) group, order ALL labelled edges of all its runs by labelled frame (onset before end within a run). Walk them in that order. Each edge's candidates = peaks inside its own labelled-bound window minus peaks already assigned to earlier edges. Choose the candidate NEAREST the labelled frame; tie → the earlier frame. Assigned peaks are removed from later edges' candidate sets. Monotonicity follows from exclusion plus the ordered walk; do not add a separate ordering constraint.
3. **Outcomes per edge:** a chosen peak ⇒ `TRANSITION k … delta=k−s`, and if other peaks remained in the window print `other peaks in window: …` on the same line; no candidate and the window had no peaks at all ⇒ `NO-TRANSITION [window]`; no candidate because every peak in the window was assigned to earlier edges ⇒ `UNASSESSABLE(all peaks in window assigned to earlier edges: …)`.
4. **Disclosure:** run line gains `assignment: onset k1 (nearest of {…}), end k2 (nearest of {…})` when any window held >1 peak.
5. **Scope:** same-target groups only (matches the source's `(target, mode)` grouping; my earlier cross-target statement was wrong — strike it). Single-run one-frame runs whose two windows overlap are handled by the same walk (onset first).
6. **Wording:** "other four legs byte-identical" in 079-11 meant identical observations and numbers; new disclosure lines and a moved cap in the header are permitted and expected.

## Predeclare (AMENDMENT 2)
- Two-run blink fixture (runs [4..5],[8..9], transitions 4/6/8/10): all four edges TRANSITION delta 0 ⇒ both runs CONSISTENT. Second onset labelled late (9): B.onset → 8, delta −1 ⇒ OFFSET-NOTE; B.end → 10, delta 0. One transition missing in the gap (no peak at 6): A.end NO-TRANSITION, B.onset → 8 ⇒ A PARTIAL, B CONSISTENT.
- 079-07 `15→16` fixture (true runs 10–11, 15–16; B.onset labelled 16): A.onset 10, A.end 12, B.onset 15 (delta −1), B.end 17 ⇒ OFFSET-NOTE — new predeclared answer replacing PARTIAL; state it.
- Codex 079-08 F4 fixture with late second onset: A.end 12 (not 15), B.onset 15 delta −1; no END-SHIFT(+3)-class result.
- Lighting / occluder / all other 079-05..08 fixtures: same outcomes as 079-10 unless the walk changes them — list every changed expected string with the reason.
- P2′: all six pinned legs, every edge observed, all 4 events CONSISTENT (caveats as the data says), `tau=0.0040`; the four non-blink legs identical in observations/numbers to 079-10.
- Bank (three masked Lyra + M50L) and scorer sweep re-run; cap reported, not tuned.

## Report
Per-P readings, the changed-expectation table, diff summary, SHAs, wall time, `VERDICT: GREEN` if P2′ and the self-test read as predeclared; else `NEEDS-DECISION`.
