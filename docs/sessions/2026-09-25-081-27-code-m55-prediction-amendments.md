# 081-27 — m55 build 3: three pre-run prediction amendments (PQ-A, PQ-B, G270 `empty_region`) applied, proven both ways, boundary re-issued (Claude Code, Opus 5.5, 2026-09-25)

## Goal
Brief 081-27, governed by `_reviews/081-26-chat-ruling-prediction-amendments-pre-run.md`: bench-free (the owner is rendering; no Unreal,
no build, no bench, no process control). (1) Apply rulings 1–3 exactly as worded, in `_reviews/081-23-lib.py` and as a dated pre-run
amendment in the requalification predictions; (2) prove each both ways; (3) re-issue `prep-boundary.json` for exactly the changed hashes
and replay the preflight offline for c1–c3; (4) re-run the dry run through the 081-26 redirecting wrapper; (5) docs.

## 1. Pre-run, verified
The campaign ledgers hold two rows, both `C1_G8_N`; `C1_G2`, `C1_G15`, `C2_G7` and C3 G270 have none, and `081-23-evidence/c2` and `c3`
are empty. The amendments are therefore pre-run (predictions AMENDMENT 2), not post-hoc.

## 2. What changed in `081-23-lib.py` (+32/−10, CRLF kept; pre-image `_reviews/081-27-evidence/081-23-lib.pre-081-27.py`)
- **PQ-A:** lib:211's `if g!=8: view_rejected == 0` becomes `view_rejected_error(g,s)`: G2 and G15 must read an integer ≥ 1, G8 keeps its
  own `identity_checks` clause, every other leg must read 0 (message unchanged for that case).
- **PQ-B:** `peak_census_check` keeps the summary-peak checks and calls the new `peak_lines_errors`: line count = 1 +
  `change_epoch_resets`; each line's census closed at its own bytes (`bytes_held` = `high_water` = line bytes, plus `census_entry_errors`);
  the last line equals the summary (bytes and census). Messages for the single-closure case keep their 081-23 wording.
- **G270:** `lyra_g270_eval` excludes `empty_region` rows from the refused set and replaces "pairs_measured == pairs_required" with
  `g270_phase_accounted` (reasons ⊆ {`empty_region`} and measured + `empty_region` = required). The budget and structural stop branches run
  first and are untouched; the result carries an `empty_region` count.

## 3. Proof (`_reviews/081-27-evidence/amendment-proof.json`, 64 of 64 as expected)
| amendment | stand-ins that pass | mutations that fail |
|---|---|---|
| PQ-A (33) | 15: Stage-1 V6 G2 N/S, G15 N/S, Lyra V6 G15 N/S, V5 G2 N/S (1 each); Stage-1 G7 N/S (0); c0 S1, 081-22 A (0); `C1_G8_N_A1` as G8; `measure_eval` wiring on `C1_G8_N_A1` as G2/G15 | 18: view_rejected 2 judged as non-gate and G1/3/7/9/10/13; full `measure_eval('g9')` on `C1_G8_N_A1`; G2/G15 at 0, missing, `true`, -1; `C1_G8_N_A1` copy at 0 as G2/G15 |
| PQ-B (19) | 7: single-closure build-3 legs `C1_G8_N_A1`, c0 S1/L1/L4, 081-22 A; the two-closure synthetic, alone and through `measure_eval('g8')` | 12: first or last line missing; order swapped; last line bytes / census unequal; first census with unaccounted bytes, unowned colour, `bytes_held` or `high_water` off its line; 3 lines for 1 reset; 2 lines for 0 or 2 resets |
| G270 (12) | 4: c0 L1 (14 `empty_region`, oracle exit 0), c0 L4, 081-17 G3_N_A2, an unmodified L1 copy | 8: one L1 pair re-labelled `closure_timeout`, `budget_exceeded` (census broken; and with a closed census + line → memory branch), `current_undelivered`; row-only and phase-only disagreement; phase one pair short; L4 with a `BUDGET-EXCEEDED` line |

⚠ **No banked build-3 log has two closures** (bank-wide scan: zero logs with two `HIGH-WATER-PEAK` lines; Stage-1 G7/G15 predate the
peak census and print none). PQ-B's pass case is therefore a **faithful synthetic**: `C1_G8_N_A1`'s real log with a first-closure line
inserted after its real `HIGH-WATER bytes=14745600` line, carrying that line's real, closed census, and `change_epoch_resets` 1 in a copied
summary. Its first real exercise will be `C1_G15_N`.

## 4. Boundary and dry run
- Dry run: `_reviews/081-27-evidence/dryrun-081-27.py` = the 081-26 wrapper redirected to `081-27-evidence/dryrun/` (the 081-26 wrapper
  itself writes into `081-26-evidence/dryrun/`, which would overwrite that turn's evidence), carrying 081-26's G8 prelude plus 16 new
  checks. **150 of 150 as expected**: the 134 checks of 081-26 with identical outcomes, one `why` string reworded (081-17 G270 PASS);
  081-23 and 081-26 evidence byte-unchanged (hash guard over both trees).
- Boundary: re-issued after this commit for `081-23-lib.py` and `feature_head` only; `_reviews/081-27-evidence/boundary-diff.json`;
  offline preflight replay for c1, c2, c3 in `boundary-verify.json`.

## 5. Observation for chat (not changed)
The amended G270 rule, as worded, has no floor on measured pairs: a leg whose every required pair is refused `empty_region` would PASS
(the oracle compares 0 and mismatches 0; the 081-08 audit's `no measured window pair` is filtered for G270; the vacuity guard tests only
zero events or zero phases). The old wording could not reach that case. c0 L1 measured 179 of 193. A one-line pre-run guard (≥ 1 measured
pair) would close it if chat wants it; it touches only C3.

## 6. State
`feat/m55-change-evidence` gains this docs commit; `master`, `m51`, tags, cooked containers, CaptureBench, `ToCodex\` untouched; no plugin
source change. Harness files changed: `_reviews/081-23-lib.py`, `_reviews/081-23-evidence/prep-boundary.json`; everything else new under
`_reviews/081-27-evidence/`. NO BUILD, NOT MERGED, NOT TAGGED. Next: windows c1 → c3 when the owner frees the PC.
