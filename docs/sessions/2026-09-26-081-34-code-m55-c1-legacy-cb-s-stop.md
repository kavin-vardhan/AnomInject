# 081-34 — m55 build 3: c1 resumed at R23_CB_S1 under AMENDMENT 5; legacy group CB-S FAILS on key presence (the m47 shader-pending keys on one build-3 OFF leg); MAIN-N, MAIN-S, c2 and c3 not run (Claude Code, Opus 5.5, 2026-09-26)

## Goal
Brief 081-34: resume requalification window c1 at `R23_CB_S1` under AMENDMENT 5 (081-32 + 081-33 A.6) and the person-present gate,
then c2 and c3; stop at the first window that exits non-zero for a measurement or gate reason; then the docs. Build 3 is `85c99b3`,
Game 002805CF. Boundary 081-33: lib `9901be63…`, window runner `56e86f2e…`, feature head `50cc235` (all verified before launch).

## What happened
- **c1**: `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\081-23-window.py c1 540 081-34`, 21:07:16–21:11:51 UTC
  (02:37–02:41 IST). Console `_reviews/081-34-evidence/c1-console.txt`.
  - Preflight PASS; quiet after 2 checks (31 s), only the shared UnrealTraceServer present.
  - **Resume verified:** the chunk summary lists the 19 decided stack legs and `legacy_CB_N` as done; the only `command-*.json` files
    written were `r23_cb_s1` … `r23_cb_s6`. No decided leg or group was re-launched.
  - Person-present gate: 7 checks (window start + 6 attempts), all clear, 0 s waits. ENV-INTERRUPTED 0. `person_evidence=false` on all six.
    Budget `081-34`: 31.1 s of 2700 s.
  - **Legacy CB-S: six READING legs on attempt 1; group FAIL.** `Stop(1)`, chunk exit 1, postflight PASS (main `m51` `53bf725`, Lyra
    `caa68c6` with 9 original modules, staged exe 002805CF, every binary, container and harness file unchanged).
  - The harness recorded `LEGACY_GROUP_CB_S FAIL` in `legacy-results.json`, so a re-run stops again (G290).
  - MAIN-N and MAIN-S not run; c2 and c3 not started.
- **AMENDMENT 5 excusals live: none.** No coalesced frame (A), every tag map the identity (B). C: `census_cycles` 27/26/27/27/27/28
  (S1–S6) is in the historical set; the cross pairs listed no historical excusal because S2/S5 already taught it as run-unique.

## The CB-S failure
Cause read: `_reviews/081-34-evidence/c1-legacy-cb-s-cause-read.md`. Evaluator: `_reviews/081-23-evidence/c1/legacy-CB-S-comparison.json`.

- **Only problem: key presence.** `CB_S2` (build 3, evidence OFF, synth order) carries `anomaly_materials_incomplete`, `render_state`,
  `shader_jobs_pending` on all 90 label rows. All 12 cross pairs: 0 extras, 0 gated-unequal.
- **What it is:** the m47 shader-readiness mark. `render_state="shaders_pending"`, `shader_jobs_pending 0`,
  `anomaly_materials_incomplete 1` on si 0..89; run_summary `shader_prewarm_incomplete 1`, `frames_shaders_pending 90`. Log l.15:
  `PREWARM shaders materials=2 incomplete=1 waited=0.0ms pendingBefore=0 pendingAfter=0` — on a packaged build, where the same line
  says the count is "STRUCTURALLY ZERO". **That m47 claim is refuted by this measurement.**
- **Everything else is identical to pre-m55 CB_S1:** masks, `mask_value`, `target_pixels`, bboxes, events, onsets, occupancy. The
  anomaly is `blinking`, which swaps no material, so the mark cannot describe these frames' pixels.
- **Incidence:** first packaged StackOBot occurrence in 972 banked sessions carrying the m47 fields (the only other non-zero is
  `LYRA_SMOKE_01`, prewarm_incomplete 2, 0 frames). 0 on all other 11 legacy legs of this requalification and on the 8 older pre-m55
  legacy legs.
- **Source:** `git diff 031a103 85c99b3 -- Source` touches no m47 line (no `Prewarm`, `ShadersPending`, `render_state`,
  `shader_jobs_pending`, `anomaly_materials_incomplete`, `IsComplete`). Evidence was OFF. **Mechanism NOT ESTABLISHED.**
- **Predicate:** looks like an encoding gap: m47's frame keys are conditional emission by design, and the key-presence rule reads their
  appearance as a schema change. It is also a real pre-m55 diagnostic reading, so what to do with it is a scope question for chat.
- **Structural observation (G297):** the cross clause learned `CB_S2`'s own differences as run-unique from the S2/S5 control pair, and
  synth order has no strict reference-pair clause. Key presence was the only rule that could see this, and it fired correctly.

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
| c1 | C1_G2_S | PASS | A1–A3 (081-29) ENV-VOIDED by ruling 081-30; A4 PASS (081-31): 19/19 |
| c1 | C1_G15_N / C1_G15_S | PASS / PASS | 081-31; 16/16 each, speed_ratio 2.395 |
| c1 | C1_LOW_N / C1_LOW_S | PASS / PASS | 081-31; `ChangeMaxBytes 1` refusal census: all 90 refused, measured 0 |
| c1 | C1_G10_N / C1_G10_S | PASS / PASS | 081-31; one budget refusal at si 8; 17/17 |
| c1 | C1_COUNT_FAIL_N / _S | PASS / PASS | 081-31; 28/28 |
| c1 | C1_WALL_FAIL_N / _S | PASS / PASS | 081-31; measured 0 |
| c1 | C1_NULL1080_N / _S | PASS / PASS | 081-31; 20/20 |
| c1 | C1_SOLID1080_S | PASS | 081-31; 20/20 |
| c1 | legacy CB-N (CB_N1–N6) | PASS-REEVALUATED (group) | 081-31 readings, attempt 1 each; AMENDMENT 5 re-evaluation committed 081-33 (A: CB_N2 si 29; B: CB_N5 π; C: `census_cycles`) |
| c1 | legacy CB-S (CB_S1–S6) | **FAIL (group)** | 081-34; 6 × READING on attempt 1; key presence on CB_S2 (m47 keys); NEEDS-DECISION |
| c1 | legacy MAIN-N (MAINON_N1–N4), MAIN-S (MAINON_S1–S4) | NOT RUN | fail-fast |
| c2 | 42 legs | NOT RUN | c1 did not exit 0 |
| c3 | NOHOLD_N/S, C3_LYRA_G270_N, C3_LYRA_NULL720_N, C3_LYRA_SOLID720_N | NOT RUN | G270 branch not reached |

**Totals: 92 legs.** 6 c0 readings · 19 c1 stack legs decided PASS · 6 legacy readings in the PASS-REEVALUATED group CB-N · 6 legacy
readings in the FAILED group CB-S · 55 not run (8 legacy c1, 42 c2, 5 c3). **No stack-gate failure on build 3. No leg dropped or
below-floor. ENV-INTERRUPTED 0 across 081-31 and 081-34.**

Evidence: `_reviews/081-23-evidence/c1/` (`legacy-results.json`, `legacy-CB_S*/`, `person-R23_CB_S*.json`, `postflight.json`); bank
`_bench_sessions_bank/M55B3R23_CB_S*`; `_reviews/081-34-evidence/` (console, cause read, `scan_m47.py`, `diff_cbs.py`,
`cb-s-pairwise-diff.txt`).

## Deviations
- New files only, under `_reviews/081-34-evidence/` (console redirected there instead of `_reviews/` root).
- No harness file, ledger or boundary was edited.

## State / hand-off
- Build 3 unchanged and still the candidate. This docs-only commit moves the feature head, so **the next window needs a boundary re-issue**.
- NOT MERGED, NOT TAGGED, no build.
- Next: chat rules on CB-S. The open questions:
  1. Are the m47 conditional keys (`render_state`, `shader_jobs_pending`, `anomaly_materials_incomplete`, and run_summary
     `frames_shaders_pending` / `shader_prewarm_incomplete`) in legacy-identity scope, or a timing/environment-sampled diagnostic class
     (like `exposure_dip`)?
  2. If in scope with an excusal: signature-bound (the leg's own `PREWARM … incomplete=N` line) and capped, as ruling A was?
  3. Does G297's structural blind spot (synth order: a single leg's value-only deviation is self-excused by its control pair) need
     a predicate change before MAIN-S?
  Then: re-evaluate CB-S from the bank (081-26 precedent) or amend before MAIN-N/MAIN-S, re-issue the boundary, resume c1 at `MAINON_N1`.
