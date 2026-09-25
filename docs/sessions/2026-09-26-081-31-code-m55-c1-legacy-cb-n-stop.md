# 081-31 — m55 build 3: c1 resumed at C1_G2_S with the person-present gate; all 14 remaining stack legs pass; the first legacy group (CB-N) FAILS its native rules; c2 and c3 not run (Claude Code, Opus 5.5, 2026-09-26)

## Goal
Brief 081-31: run requalification windows c1 (from `C1_G2_S`), then c2 and c3, with the person-present gate from 081-30. Stop at the
first window that exits non-zero for a measurement or gate reason, then write the docs. Build 3 is `85c99b3`, Game 002805CF.
Boundary 081-30: lib `bb4fbb9d…`, window runner `56e86f2e…`, feature head `66976ea`.

## What happened
- **c1**: `081-23-window.py c1 540 081-31`, run 19:03:26–19:13:30 UTC (00:33–00:43 IST).
  - Console: `_reviews/081-31-c1-console.txt`.
  - Preflight PASS; quiet after 2 checks (31 s), with only the shared UnrealTraceServer present.
- **Resume verified.** The first launch was `C1_G2_S_A4`: after the ENV-VOIDED row it started a fresh budget. No decided leg was re-launched.
- **The person-present gate** ran 21 times: window start plus 20 attempts.
  - Every check was clear with a 0 s wait, each excused by the previous attempt's own ALT tap (081-30 design).
  - ENV-INTERRUPTED: 0. `person_evidence=false` on all 20 attempts.
  - Budget: 31.1 s of 2700 s spent.
- **All 14 stack legs PASS on the first attempt they were given.**
- **Legacy group `CB-N`: all six legs are READING on attempt 1, and the group comparison FAILS (22 problems).**
  - Stop: `Stop(1) legacy CB-N FAIL`, chunk exit 1, postflight PASS (main `m51` `53bf725`, Lyra `caa68c6` with 9 original modules,
    staged exe 002805CF, every binary, container and harness file unchanged).
  - `CB-S`, `MAIN-N` and `MAIN-S` were not run.
  - c2 and c3 were not started.

## The CB-N failure — three independent phenomena
Full read with log lines and source references: `_reviews/081-31-evidence/c1-legacy-cb-n-cause-read.md`.
Evaluator output: `_reviews/081-23-evidence/c1/legacy-CB-N-comparison.json`.

- **A — `CB_N2` (build 3, evidence OFF): the frame-29 target mask is UNAVAILABLE.**
  - Observed:
    - `target_pixels -1`, `mask_state unmeasured`;
    - event 2's observable subset is [28,33,34] instead of [28,29,33,34] (`injected_frames` unchanged).
  - This one frame accounts for all the "field set not identical" lines, the 20-path strict set on N2's pairs, and the key-presence line.
  - Log: one mask render served three arms (the si 28 target, a census arm and the si 29 target), then
    `TARGET MASK UNAVAILABLE … si 29 … pixels=0`.
  - Source: the drain gives pixels to the **first** pixel-wanting arm only. That rule is **identical in pre-m55 `master` and build 3**,
    and build 3's addition there is guarded by a null-when-OFF issue pointer.
  - Why frame 28's mask render slipped one frame: **not established**. N2's speed_ratio is 1.000913, against about 1.000003 on N3–N5.
  - Not attributable to m55 by source; there is no measured control.
- **B — `CB_N5` (build 3, evidence OFF): bijective stencil-tag relabelling.**
  - 89 strict paths, all `mask_value` or mask-file hashes. The tag map is one-to-one (247→227, 248..254→247..253).
  - Occupancy, MASK-TIE and all counts are equal.
  - It is the run-to-run census relabel class. The other OFF leg matches pre-m55.
  - Looks like a **predicate-encoding gap**: the raw tag values are compared, while the stable CB field set is tag-normalised and passes.
- **C — `census_cycles` 26/26/27/27/26/26** (27 on both ON legs).
  - It is in the historical run-unique set, so the native strict rule already excuses it.
  - The cross-pair rule (rule 1) checks only this group's three control pairs, which happened to agree.
  - It looks like a **predicate-encoding gap** (rule 1 ignores the historical set). ON = 27 is an association (n = 2 vs 4), with no mechanism.
- → **NEEDS-DECISION.** Nothing was re-run, edited or reclassified.

## Requalification ledger — every leg of c0–c3 on build 3

| window | leg | result | note |
|---|---|---|---|
| c0 | L1 Lyra ON 1800 | VALID READING | 081-24 |
| c0 | L2 Lyra OFF 1800 | VALID READING | attempt 3 |
| c0 | L3 Lyra OFF 1800 | VALID READING | |
| c0 | L4 Lyra ON 1800 | VALID READING | |
| c0 | S1 Stack ON 1800 | VALID READING | |
| c0 | S2 Stack OFF 1800 | VALID READING | |
| c1 | C1_G8_N | PASS-REEVALUATED | 081-25 A1 re-evaluated in 081-26 |
| c1 | C1_G8_S | PASS | 081-29 A1 |
| c1 | C1_G9_N | PASS | 081-29 A1 |
| c1 | C1_G9_S | PASS | 081-29 A1 |
| c1 | C1_G2_N | PASS | 081-29 A1 |
| c1 | C1_G2_S | **PASS** | A1–A3 (081-29) ENV-VOIDED by ruling 081-30; **A4 PASS**: 5 events, 20 pairs, 19 measured, oracle 19/19; HW 25.80 MB; 89 PNGs |
| c1 | C1_G15_N / C1_G15_S | **PASS** / **PASS** | 16/16 each; HW 19.35 / 18.43 MB; speed_ratio 2.395 on both (the G15 recipe) |
| c1 | C1_LOW_N / C1_LOW_S | **PASS** / **PASS** | `ChangeMaxBytes 1` runtime refusal census, first run: all 90 indices refused, measured 0, HW 0, oracle compared 0 |
| c1 | C1_G10_N / C1_G10_S | **PASS** / **PASS** | G10 first runtime proof: one budget refusal at si 8; 17/17 matched; HW 19.35 / 18.43 MB |
| c1 | C1_COUNT_FAIL_N / _S | **PASS** / **PASS** | 15 events, 30 pairs, 28 measured, 28/28; HW 40.55 / 41.47 MB |
| c1 | C1_WALL_FAIL_N / _S | **PASS** / **PASS** | 15 events, 30 pairs, measured 0; HW 27.65 MB |
| c1 | C1_NULL1080_N / _S | **PASS** / **PASS** | 20/20; HW 66.39 MB |
| c1 | C1_SOLID1080_S | **PASS** | 20/20; HW 62.24 MB |
| c1 | legacy CB-N (CB_N1–N6) | **FAIL (group)** | 6 × READING on attempt 1; comparison FAIL, see above; NEEDS-DECISION |
| c1 | legacy CB-S (CB_S1–S6) | NOT RUN | fail-fast |
| c1 | legacy MAIN-N (MAINON_N1–N4), MAIN-S (MAINON_S1–S4) | NOT RUN | fail-fast |
| c2 | 42 legs | NOT RUN | c1 did not exit 0 |
| c3 | NOHOLD_N/S, C3_LYRA_G270_N, C3_LYRA_NULL720_N, C3_LYRA_SOLID720_N | NOT RUN | G270 branch not reached |

**Totals: 92 legs.**
- 6 c0 readings.
- 19 c1 stack legs decided PASS: 18 on attempt, plus the re-evaluated `C1_G8_N`. `C1_G2_S` passed on its fresh-budget A4.
- 6 legacy readings in one FAILED group.
- 61 not run: 14 legacy c1, 42 c2, 5 c3.
- **No stack-gate failure on build 3. No leg dropped or below-floor by a predeclared rule. ENV-INTERRUPTED 0.**

Evidence:
- `_reviews/081-23-evidence/c1/`: `stack-results.json`, `legacy-results.json`, `legacy-CB_N*/`, `person-*.json`, `postflight.json`.
- Bank: `_bench_sessions_bank/M55B3R23_*`.
- Snapshots: `_reviews/081-31-evidence/`.

## Deviations
- New files only:
  - `_reviews/081-31-c1-console.txt` (+ `.err.txt`);
  - `_reviews/081-31-evidence/` (cause read and two ledger snapshots).
- No harness file, ledger or boundary was edited.

## State / hand-off
- Build 3 is unchanged and still the candidate.
- The feature branch gains this docs-only commit. NOT MERGED, NOT TAGGED, no build.
- **The next commit on the feature branch moves the head, so the next window needs a boundary re-issue** (081-30 already hit this).
- Next: chat rules on CB-N. The open questions:
  1. Is phenomenon A (a pre-existing coalescing rule, trigger not established) disqualifying for a legacy-identity gate, or is it
     scoped/declared?
  2. Should rules 1/3 compare `mask_value` / CB mask hashes modulo bijection, as `P-C7 v2/v3` do?
  3. Should rule 1 also consult the historical run-unique set (`census_cycles`)?

  Any predicate change is a pre-run amendment for **CB-S / MAIN-N / MAIN-S only** or a re-evaluation of CB-N from the bank (081-26
  precedent), plus a boundary re-issue. c1 then resumes at `CB_S1`, followed by c2 and c3.
