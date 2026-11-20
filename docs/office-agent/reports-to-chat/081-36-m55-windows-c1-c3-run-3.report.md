# REPORT for 081-36-m55-windows-c1-c3-run-3.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-36-m55-windows-c1-c3-run-3.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-26 03:48:02 | finished: 2026-09-26 04:26:20 | duration: 38 min | exit code: 0
context at last turn: 371807 tokens (37% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-36-m55-windows-c1-c3-run-3.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

Build 3 (`85c99b3`, Game 002805CF) passed windows c1, c2 and c3, so the whole c0–c3 requalification is complete. Every window exited 0 with postflight PASS. The docs are pushed.

## Plain summary for the owner
- c1 picked up where it stopped: only the 8 remaining old-vs-new comparison runs were launched. Both comparison groups (MAIN-N and MAIN-S) passed.
- c2: all 42 runs passed on the first try. c3: all 5 passed; one needed a second try after a predeclared camera-setup check failed.
- The PC stayed free throughout. The "is someone using the PC" check ran 59 times and never had to wait, and no attempt was voided.
- Nothing was excused under the new rules (AMENDMENTS 5–6), and the early-warning check did not trip.
- Build 3 is ready for chat's release decision. It is not merged and not tagged.

## Per-window table

| window | legs run | passed | invalid | ENV-INTERRUPTED | dropped / below-floor | person-present waits | wall time | postflight |
|---|---|---|---|---|---|---|---|---|
| c1 (resume) | 8 comparison legs (MAINON_N1–N4, S1–S4) | 8 READING on attempt 1; MAIN-N **PASS**, MAIN-S **PASS** | 0 | 0 | 0 / 0 | 9 checks, 0 s | 22:19:03–22:24:37 UTC (5.6 min) | PASS |
| c2 | 42 | 42 PASS, all on attempt 1 | 0 | 0 | 0 / 0 | 43 checks, 0 s | 22:24:43–22:44:24 UTC (19.7 min) | PASS |
| c3 | 5 (6 attempts) | 5 PASS | 1 (R23_NOHOLD_N A1) | 0 | 0 / 0 | 7 checks, 0 s | 22:44:30–22:53:40 UTC (9.2 min) | PASS |

- **Resume check:** in c1, only `command-r23_mainon_*.json` files were written. No decided leg or group was re-launched.
- **Budget:** tag `081-36` used 31.1 s of 2700 s in each window. All of it was the quiet-gate wait at window start; only the shared `UnrealTraceServer` was present.
- **The invalid attempt:** R23_NOHOLD_N A1 failed the fixture rule `fixture: MainWorld full-row camera precondition failed`, with `person_evidence=false`. A2 passed.

## Failures
None.

## Notable readings
- **Refusal census (`ChangeMaxBytes 1`) and G10:** these are c1 stack legs, already decided in 081-31 (all 90 frames refused, measured 0). They were not re-run, by the resume design.
- **Legacy vs pre-m55:**
  - MAIN-N and MAIN-S were built 2 pre-m55 legs plus 2 build-3 evidence-ON legs each. There were no OFF legs in these groups.
  - All 8 cross pairs showed 0 extras and 0 gated-unequal fields. Key presence was clean, and both ON legs carried their change block.
  - The OFF-leg comparisons are CB-N and CB-S, both PASS-REEVALUATED earlier.
- **Lyra G270:** reached the **PASS** branch.
  - 9 events, 7 of them with a measured required pair (the floor is 3).
  - 32 pairs: 28 measured, 4 honest `empty_region`; 0 budget refusals, invariants 0.
  - The oracle matched 28 of 28. Latency: p50 541 ms, p95 628 ms, max 676 ms.
  - NULL720 and SOLID720 passed with 20 of 20 each.
- **Person-present gate:** fired 59 times, 0 s total wait.
  - Some checks read idle time under 60 s. Each was cleared because the last input fell inside the previous attempt's own focus window, which is the harness's input exclusion working as designed.
- **AMENDMENT 5–6 excusals in this run:** none.
  - A: no coalesced frame excused.
  - B: every tag map was the identity, so no π was applied.
  - C: no historical excusal was needed.
  - M47: `m47_excused` is empty in both groups. The campaign's m47 cap still stands at 1 (CB_S2 from 081-34).
- **G297 singleton reading:**
  - MAIN-N: none.
  - MAIN-S: two singletons, both on the timing-sampled field `coverage_pct`, which is not in the historical run-unique set:
    - `anomalies[2]` on **MAINON_S1 (pre-m55)**: 8.6927 vs 8.6518;
    - `anomalies[6]` on **MAINON_S2 (build 3, ON)**: 9.25045 vs 9.24997.
  - **The tripwire did not fire.** The same field also shows singletons on pre-m55 legs (081-22 D_N1/D_N4 and MAINON_S1).
- **`census_cycles`:**

  | group | pre-m55 legs | build-3 ON legs |
  |---|---|---|
  | MAIN-N | N1 69, N4 76 | N2 76, N3 74 |
  | MAIN-S | S1 74, S4 77 | S2 78, S3 67 |

  The values do not separate by binary.

## Deviations
- The consoles went to `_reviews\081-36-evidence\c{1,2,3}-console.txt`. These are new files only. No harness file, ledger or boundary was edited by hand.
- No new gotcha: every reading in this run fell under an existing predeclared rule.
- The docs commit moved the feature head to `2125159`, but the boundary still pins `03adc99`. **Any further bench window needs a boundary re-issue first.**

## Commits pushed
`feat/m55-change-evidence` `03adc99..2125159`, docs only, committed from a scratch worktree that has since been removed:
- the journal `docs/sessions/2026-09-26-081-36-code-m55-requal-c1-c3-green.md`, with the full 92-leg c0–c3 ledger:
  - 6 c0 readings;
  - 19 c1 stack PASS;
  - 4 comparison groups PASS (CB-N and CB-S re-evaluated), covering 20 legs;
  - 42 c2 PASS;
  - 5 c3 PASS;
- the CLAUDE.md status block.

The main checkout is still `m51` at `53bf725`. `master` is `031a103`, same as origin. No build, merge or tag.

## NEEDS-DECISION
None. Build 3 now waits on chat's release decision.

## Wall-clock
- **c1:** 5.6 min.
- **c2:** 19.7 min.
- **c3:** 9.2 min, including the Lyra staging and restore.
- **Session:** about 40 min.

Several connectors need authorising in their connector settings, and the PDF viewer failed to connect. None were needed here.
api-equivalent cost (not billed on Max): $5.38 | turns: 34 | running total across all briefs: $2,275.50
