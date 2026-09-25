# 081-36 — m55 build 3: c1 resumed at `R23_MAINON_N1` under AMENDMENTS 5–6 and the person-present gate; MAIN-N and MAIN-S PASS; c2 42/42 PASS; c3 5/5 PASS (Lyra G270 on the PASS branch). Requalification c0–c3 COMPLETE (Claude Code, Opus 5.5, 2026-09-26)

## Goal
Brief 081-36: resume window c1 at `R23_MAINON_N1` (after 081-35's AMENDMENT 6 and the CB-S PASS-REEVALUATED row), then c2, then c3,
stopping at the first window that exits non-zero for a measurement or gate reason; then the docs. Build 3 is `85c99b3`, Game
002805CF. Boundary 081-35: lib `f5876b05…`, window runner `56e86f2e…`, feature head `03adc99` (local == origin at launch).

## What happened
All three windows used `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\081-23-window.py <cN> 540 081-36` (one budget tag),
run in the background from `_reviews\`, console redirected to `_reviews/081-36-evidence/cN-console.txt`.

| window | UTC | legs launched | result | chunk / postflight |
|---|---|---|---|---|
| c1 (resume) | 22:19:03–22:24:37 (5.6 min) | 8 legacy (MAINON_N1–N4, MAINON_S1–S4) | 8 READING on attempt 1; MAIN-N **PASS**, MAIN-S **PASS** | 0 / PASS |
| c2 | 22:24:43–22:44:24 (19.7 min) | 42 stack | 42 PASS, all on attempt 1 | 0 / PASS |
| c3 | 22:44:30–22:53:40 (9.2 min) | 2 nohold + 3 Lyra | 5 PASS (NOHOLD_N on attempt 2) | 0 / PASS |

- **Resume verified:** `chunk1-summary.json` lists the 19 decided stack legs and `legacy_CB_N` / `legacy_CB_S` as done; the only
  `command-*.json` files written in c1 this run were `r23_mainon_n1`…`s4`. No decided leg or group was re-launched.
- **Quiet gate:** 2 checks per window (31 s each), only the shared `UnrealTraceServer` present. Budget `081-36`: 31.1 s of 2700 s per window.
- **Person-present gate:** 9 (c1) + 43 (c2) + 7 (c3) = 59 checks, all clear, **0 s waited**. Checks with idle < 60 s were cleared by
  the harness-input exclusion (the last input lay inside the previous attempt's own focus window, which carried no person evidence).
  **ENV-INTERRUPTED 0; `person_evidence=false` on every attempt.**
- **Postflights:** main `m51` `53bf725`, Lyra `caa68c6` with its 9 original modules (c3 staged `c27024d` + 11 build-3 modules and
  restored, removing 2 build-3-only files), staged exe 002805CF, every binary/container/harness file unchanged.

## Legacy groups (c1)
Evaluator files `_reviews/081-23-evidence/c1/legacy-MAIN-N-comparison.json`, `legacy-MAIN-S-comparison.json`; rows in `legacy-results.json`.

- **MAIN-N** (native, `MAINON_N1`/`N4` pre-m55 0844220E, `N2`/`N3` build 3 evidence ON): 4 cross pairs, 0 extras, 0 gated-unequal;
  key presence clean, change blocks present on both ON legs. Timing-sampled tick counters differed on N1–N2 and N3–N4 only.
- **MAIN-S** (synth, same layout): 4 cross pairs, 0 extras, 0 gated-unequal; `coverage_pct` (timing-sampled) differed on three pairs.
- **AMENDMENT 5 live:** A — no coalesced frame excused; B — every tag map the identity (no π); C — no historical excusal needed.
- **AMENDMENT 6 live:** `m47_excused` empty in both groups (no m47 keys on any leg). Campaign cap unchanged at 1 build-3 leg (CB_S2).
- **G297 reading:** MAIN-N none. MAIN-S two self-excused singletons, both on timing-sampled `coverage_pct`, not in the historical
  run-unique set: `anomalies[2]` on **MAINON_S1 (pre-m55)** 8.6927 vs 8.6518, and `anomalies[6]` on **MAINON_S2 (build 3, ON)**
  9.25045 vs 9.24997. **Tripwire does not fire** — the field also has pre-m55 singletons (081-22 D_N1/D_N4 and MAINON_S1).
- **`census_cycles`:** MAIN-N N1 69 (pre) · N2 76 (b3 ON) · N3 74 (b3 ON) · N4 76 (pre); MAIN-S S1 74 (pre) · S2 78 (b3 ON) ·
  S3 67 (b3 ON) · S4 77 (pre). No separation by binary.

## c3
- **NOHOLD_N:** A1 INVALID-FIXTURE (`fixture: MainWorld full-row camera precondition failed`, predeclared fixture rule, no person
  evidence), A2 PASS. **NOHOLD_S:** A1 PASS.
- **C3_LYRA_G270_N — PASS branch:** 9 events, 7 with a measured required pair (≥ 3 floor met), 32 pairs = 28 measured + 4 honest
  `empty_region`, budget refusals 0, invariants 0, oracle 28/28 matched; latency p50 541 ms / p95 628 ms / max 676 ms (16/19/21 frames).
- **C3_LYRA_NULL720_N / SOLID720_N:** PASS, 20/20 each.

## Requalification ledger — every leg of c0–c3 on build 3

| window | leg | result | attempts / note | evidence |
|---|---|---|---|---|
| c0 | L1 Lyra ON 1800 | VALID READING | 081-24 | `081-23-evidence/c0/` |
| c0 | L2 Lyra OFF 1800 | VALID READING | attempt 3 | `c0/` |
| c0 | L3 Lyra OFF 1800 | VALID READING | | `c0/` |
| c0 | L4 Lyra ON 1800 | VALID READING | | `c0/` |
| c0 | S1 Stack ON 1800 | VALID READING | | `c0/` |
| c0 | S2 Stack OFF 1800 | VALID READING | | `c0/` |
| c1 | C1_G8_N | PASS-REEVALUATED | 081-25 A1, re-evaluated 081-26 | `c1/C1_G8_N_A1` |
| c1 | C1_G8_S, C1_G9_N, C1_G9_S, C1_G2_N | PASS ×4 | 081-29 A1 each | `c1/` |
| c1 | C1_G2_S | PASS | A1–A3 (081-29) ENV-VOIDED (ruling 081-30); A4 PASS (081-31) | `c1/C1_G2_S_A4` |
| c1 | C1_G15_N / _S | PASS / PASS | 081-31 A1 | `c1/` |
| c1 | C1_LOW_N / _S | PASS / PASS | 081-31 A1; `ChangeMaxBytes 1` refusal census, all 90 refused | `c1/` |
| c1 | C1_G10_N / _S | PASS / PASS | 081-31 A1 | `c1/` |
| c1 | C1_COUNT_FAIL_N / _S | PASS / PASS | 081-31 A1 | `c1/` |
| c1 | C1_WALL_FAIL_N / _S | PASS / PASS | 081-31 A1 | `c1/` |
| c1 | C1_NULL1080_N / _S | PASS / PASS | 081-31 A1 | `c1/` |
| c1 | C1_SOLID1080_S | PASS | 081-31 A1 | `c1/` |
| c1 | legacy CB-N (CB_N1–N6) | PASS-REEVALUATED (group) | 081-31 readings A1; AMENDMENT 5, committed 081-33 | `c1/legacy-CB-N-comparison.json` |
| c1 | legacy CB-S (CB_S1–S6) | PASS-REEVALUATED (group) | 081-34 readings A1 (FAIL row kept); AMENDMENT 6, committed 081-35 | `c1/legacy-CB-S-comparison.json` |
| c1 | legacy MAIN-N (MAINON_N1–N4) | **PASS (group)** | 081-36 A1 each | `c1/legacy-MAIN-N-comparison.json` |
| c1 | legacy MAIN-S (MAINON_S1–S4) | **PASS (group)** | 081-36 A1 each | `c1/legacy-MAIN-S-comparison.json` |
| c2 | C2_NULL, SOLID, DELAY, HIDE, HIDE_DEPTH, EMPTY, BLINK, SHORT, DROP, RUNEND, TEARDOWN × N/S | PASS ×22 | 081-36 A1 each | `c2/stack-results.json` |
| c2 | C2_MOVING_NULL_N, C2_MOVING_SOLID_N | PASS ×2 | A1 | `c2/` |
| c2 | C2_G1, G3, G4, G5, G6, G7, G11, G12, G13 × N/S | PASS ×18 | A1 each; G7 at `speed_ratio` 2.395 | `c2/` |
| c3 | R23_NOHOLD_N | PASS | A1 INVALID-FIXTURE (camera precondition), A2 PASS | `c3/nohold-results.json` |
| c3 | R23_NOHOLD_S | PASS | A1 | `c3/nohold-results.json` |
| c3 | C3_LYRA_G270_N | PASS (branch PASS) | A1; 7 measured events, 4 `empty_region` | `c3/lyra-results.json` |
| c3 | C3_LYRA_NULL720_N / SOLID720_N | PASS / PASS | A1 each | `c3/lyra-results.json` |

**Totals: 92 legs.** 6 c0 readings · 19 c1 stack PASS · 4 legacy groups PASS (two re-evaluated) covering 20 legacy legs · 42 c2 PASS ·
5 c3 PASS. **Nothing dropped, nothing below a floor, no FAIL outstanding.** ENV-INTERRUPTED 0 in 081-31, 081-34 and 081-36; the only
voided attempts in the campaign are C1_G2_S A1–A3 (081-30). Bank: `_bench_sessions_bank/M55B3R23_*`.

## Deviations
- New files only, under `_reviews/081-36-evidence/` (three consoles). No harness file, ledger or boundary edited by hand.
- This docs commit moves `feat/m55-change-evidence`'s head; the prep boundary still pins `03adc99`, so **any further bench window
  needs a boundary re-issue first**. None is planned inside m55's requalification — c0–c3 are complete.
- No new gotcha: every reading this run had a predeclared rule.

## State / hand-off
- **Build 3 (`85c99b3`, Game 002805CF) has passed the whole c0–c3 requalification.** NOT MERGED, NOT TAGGED, no build.
- Next is chat's: the m55 release decision for build 3.
