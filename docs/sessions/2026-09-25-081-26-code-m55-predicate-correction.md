# 081-26 — m55 build 3: the G8 "no receipt" predicate is corrected and proven both ways, `C1_G8_N` is PASS-REEVALUATED, the predicate audit finds two blocking prediction questions (Claude Code, Opus 5.5, 2026-09-25)

## Goal
Brief 081-26, governed by `_reviews/081-25-chat-ruling-g8-predicate-correction.md`: bench-free (the owner is rendering; no Unreal, no build,
no bench, no process control). (1) Correct the G8 clause as ruling 1 words it and prove it both ways; (2) audit every gate / §D predicate
for the same class of defect, fix encoding defects only, report prediction questions; (3) re-evaluate `C1_G8_N_A1` from the bank and make
c1 resume at `C1_G8_S`, proven offline; (4) re-issue `prep-boundary.json`; (5) re-run the dry-run suite; (6) docs.

## 1. The corrected clause
`_reviews/081-23-lib.py` (was lib:111, now lib:113) tested `not by[8].get('receipt')`. It now tests `receipt_without_delivery(receipt)`:
`t_drain_ms == -1` and `view_family_id == 0` and `serving_token == 0` and `format == 0` and `rect == []` and `extent == []` (ruling 1,
verbatim). The failure message keeps its `build-3 D:` prefix so every ledger and dry-run filter still classifies it.

Proof (`_reviews/081-26-evidence/g8-proof.json`, 38 of 38 as expected):
- PASS: `C1_G8_N_A1` SI8 (`unsupported_delivery`, `stage: colour`); Stage-1 SI8 of Lyra V6 G15 N/S and StackOBot V6 G15 N/S
  (`closure_timeout`), Lyra V5 G4 N/S and StackOBot V6 G4 N/S (`current_undelivered`). The old clause fails every one of them.
- FAIL: six mutations of SI8's receipt, each populating exactly one delivery field with SI7's real value (drain time, family 8, serving
  token 8, format 18, rect `[0,0,1280,720]`, extent `[1280,720]`); the delivered SI7 and SI9 receipts; an absent and an empty receipt.
- Population: 374 banked sessions, 74,257 receipts (`receipt`, `prev_receipt`, `mask_receipt`). Every receipt with `t_drain_ms -1` carries
  the whole stub (801 of 801) and every delivered receipt fails the clause (73,456 of 73,456); no third shape exists in the bank.
- The full `measure_eval('g8')` on a session copy with any one field populated fails on this clause and on nothing else;
  `identity_checks(8)` on `C1_G8_N_A1` fails with the pre-081-26 lib and passes with the corrected one.

## 2. Predicate audit (`_reviews/081-26-evidence/audit.md`)
Every predicate in `081-23-lib.py` and in the unchanged `081-08-audit.py` it calls, each mapped to the serialised form it expects, the
source line that writes it and a banked stand-in (build-3 stand-ins: c0 L1/L4/S1, 081-22 A, `C1_G8_N_A1`; `standin-scan.json`,
`audit-standins/`). Result: **one ENCODING-DEFECT (the G8 clause, fixed), two PREDICTION-QUESTIONs, everything else OK.**
- **PQ-A — `change_view_rejected` 0 "except G8" (predictions §2.1.4, lib:211).** Build-3 source also counts it on **G2**: SI8's colour
  arm is dropped (`AnomalyCaptureSubsystem.cpp:4495-4499`) but its mask arm carries SI8's issue (`:1282-1285`), so the frame-8 mask pass
  finds no m55 family data and counts `view_rejected` (`AnomalyMaskSceneViewExtension.cpp:184`); and on **G15**: the bench mask hold counts
  every held pass (`:158-161`). Stage-1 V6 banked `change_view_rejected` 1 on G2 N/S and G15 N/S before the prediction was written.
- **PQ-B — "exactly one `HIGH-WATER-PEAK` line" (§2.1.3, lib:60).** Gates 7 and 15 reset the epoch at SI 9 (`:4465`); `ResetEpoch`
  closes and persists (`AnomalyChangeStage.cpp:664`, the line is logged at `:655`) and clears `bPersisted` (`:669`), so run end logs a
  second line. The field reference (081-17) says one line per closure. The census check on the C1_G8 log with its line duplicated reads
  `HIGH-WATER-PEAK lines=2`.
- Consequence: **c1 would stop again at `C1_G2_N` (PQ-A), then at `C1_G15_N` (PQ-A, PQ-B); c2 at `C2_G7_N` (PQ-B).** Not changed, per ruling
  2. Recommended wording (chat decides): PQ-A "0 except G8, G2 and G15 (≥ 1 on each)"; PQ-B "one line per closure (1 + `change_epoch_resets`),
  the last equal to the summary, every census closed".
- Note, not a question: `lyra_g270_eval` stops on any refused required pair, including an honest `empty_region` (081-14 §3.4 as written).
  On the 1800-frame c0 L1 leg it would have fired (14 rows); c0 L4 (1800) and 081-17 (300) had none.

## 3. `C1_G8_N_A1` re-evaluated, not re-run
`081-26-evidence/reevaluate-c1-g8n.py` ran `measure_eval('g8')` with the corrected lib on the banked session and log: **errors none**;
oracle re-run 17 compared = 17 matched, 0 mismatched; trace SI7 null, SI8 `unsupported_delivery`, SI9 `predecessor_undelivered`, SI10 null;
5 events, 19 pairs, 17 measured, high-water 18.43 MB, 89 PNGs. A **PASS-REEVALUATED** row is appended to `081-23-evidence/c1/stack-results.json`
after the original FIXTURE-VALID-FAILURE row, which is unchanged; the re-evaluation's audit and oracle files are in
`c1/C1_G8_N_A1/reevaluation-081-26/`, beside the original `measurement-audit.json`. The first c1 invocation's summary, postflight and
manifests are byte-copied to `c1/invocation-1-081-25/` because the resumed invocation overwrites them.

## 4. Resume logic, and a correction to the 081-25 record
🔻 **Journal 081-25 said an unchanged re-run of c1 "would re-run `C1_G8_N`". It would not: the 081-23 `run_measure_leg` treated a recorded
FIXTURE-VALID-FAILURE as "done", skipped the leg and launched `C1_G8_S` — carrying on past the failure, not retrying it.** Measured offline
by driving the unchanged `081-23-chunk1.py` with the pre-081-26 lib and every launch stubbed. (081-25 stays as written; this is the correction.)
The lib now: PASS and PASS-REEVALUATED are decided and skipped; a recorded fixture-valid failure (measure), FAIL (NoHold, Lyra), NEEDS-DECISION
(Lyra) or NOT-RUN (all four runners) re-raises its original stop code on re-entry; invalid attempts are still retried up to three.
`081-26-evidence/resume-proof.json` (7 of 7, c1 files untouched): with the 081-26 ledger and corrected lib c1 enters `C1_G8_N` (skipped as
decided) and the first launch would be **`C1_G8_S_A1`**; with the 081-25 ledger only it stops at `C1_G8_N` with code 1 and launches nothing;
with the old lib it would launch `C1_G8_S_A1`; an INVALID-FIXTURE attempt still retries (`C1_G8_S_A2`); a NOT-RUN re-raises code 2; a NoHold
FAIL re-raises code 1; a NoHold PASS-REEVALUATED is skipped.

## 5. Boundary and dry run
- `prep-boundary.json` re-issued: `feature_head` → the branch head carrying this journal, `081-23-lib.py` `7e41fa89…` → `ce8184d4…`; no
  other locked hash changed (`081-26-evidence/boundary-diff.json`). An offline replay of the preflight's non-process checks against the new
  boundary passes (`boundary-verify.json`).
- Dry run (`081-26-evidence/dryrun/`): the unchanged `081-23-dryrun.py`, executed through a wrapper that redirects its output folder, its
  temp folder and its chunk-0 analyzer self-test into `081-26-evidence/dryrun/` (six asserted single-occurrence replacements — five in the
  dry-run script, one in a copy of the analyzer — recorded in `comparison-vs-081-23.json`), plus
  16 new G8 checks. **134 of 134 as expected; the 118 original checks have identical outcomes to 081-23's; no 081-23 evidence file changed.**

## Requalification ledger after 081-26
| window | legs | state |
|---|---|---|
| c0 | 6 | 6 valid readings (081-24) |
| c1 | 39 | `C1_G8_N` **PASS-REEVALUATED** (081-26); 38 not run — next `C1_G8_S`; predicted stop at `C1_G2_N` (PQ-A) until chat rules |
| c2 | 42 | not run; `C2_G7_N/S` blocked by PQ-B |
| c3 | 5 | not run |

## Deviations
- The resume hardening covers all four runners (`run_measure_leg`, `run_legacy_group`, `run_nohold`, `run_lyra_leg`), not only c1's: the
  same skip-past-a-failure shape existed in each, and every one lives in the authorised lib.
- The dry run was executed through a redirecting wrapper rather than as-is: run as-is it overwrites `081-23-evidence/dryrun/` and the c0
  analyzer self-test in `081-23-evidence/c0/`, which the brief forbids touching.
- Harness files changed: `_reviews/081-23-lib.py`; c1 state (`081-23-evidence/c1/stack-results.json` appended, `invocation-1-081-25/` and
  `C1_G8_N_A1/reevaluation-081-26/` added); `081-23-evidence/prep-boundary.json`. Everything else new is under `_reviews/081-26-evidence/`.

## State / hand-off
Build 3 unchanged and still the candidate (Game 002805CF). No bench, no build, no process launched. Branch `feat/m55-change-evidence`
carries docs only on top of `1ca1ab3`. NOT MERGED, NOT TAGGED. Next: chat rules on PQ-A and PQ-B (a later brief applies any correction,
proves it both ways and re-issues the boundary again), then the owner frees the PC and a brief resumes c1 at `C1_G8_S`, then c2, c3.
