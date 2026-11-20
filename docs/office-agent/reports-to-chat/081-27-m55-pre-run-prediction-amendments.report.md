# REPORT for 081-27-m55-pre-run-prediction-amendments.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-27-m55-pre-run-prediction-amendments.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-25 12:28:24 | finished: 2026-09-25 12:47:24 | duration: 19 min | exit code: 0
context at last turn: 445792 tokens (45% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-27-m55-pre-run-prediction-amendments.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

## Plain summary for the owner
- I applied the three test-check corrections that chat approved. All three are fixed before any of the affected tests has run.
- Each correction passes on real saved results and fails on deliberately broken copies: 64 of 64 checks behaved as expected.
- The lock file for the next PC session is updated, and its start-up check passes offline for c1, c2 and c3. The full dry run passes 150 of 150.
- Nothing ran on the PC: no game, no editor, no build.

## Amendment proof (`_reviews\081-27-evidence\amendment-proof.json`, 64/64)

| amendment | stand-ins that pass | mutations that fail |
|---|---|---|
| **PQ-A**: `view_rejected` is 0 except G8, G2 and G15, which must be ≥ 1 (33 checks) | 15 checks, all real saved results: <br>• Stage-1 V6 G2 N/S and G15 N/S on StackOBot, Lyra V6 G15 N/S and V5 G2 N/S (value 1) <br>• Stage-1 G7 N/S (value 0) <br>• the build-3 non-gate legs c0 S1 and 081-22 A (value 0) <br>• `C1_G8_N_A1` as G8 <br>• the wiring inside `measure_eval`, using `C1_G8_N_A1` as G2 and as G15 | 18 checks: <br>• value 2 judged as a non-gate leg and as G1, 3, 7, 9, 10 and 13 <br>• the full `measure_eval('g9')` on `C1_G8_N_A1` <br>• G2 and G15 with 0, missing, `true` or -1 <br>• a `C1_G8_N_A1` copy with 0, judged as G2 and G15 |
| **PQ-B**: one `HIGH-WATER-PEAK` line per closure (1 + `change_epoch_resets`), the last equal to the run summary, every census closed (19 checks) | 7 checks: <br>• five real single-closure build-3 legs: `C1_G8_N_A1`, c0 S1, L1, L4, 081-22 A <br>• a two-closure synthetic, on its own and through the full `measure_eval('g8')` | 12 checks: <br>• first or last line missing <br>• the two lines in the wrong order <br>• last line's bytes or census differ from the summary <br>• four versions of an unclosed first-closure census <br>• 3 lines for 1 reset, and 2 lines for 0 or 2 resets |
| **G270**: a pair refused as `empty_region` is honest; `closure_timeout` or `budget_exceeded` still stops (12 checks) | 4 checks: <br>• c0 L1 now **PASSes**: 50 events, 179 measured + 14 `empty_region` of 193, oracle exit 0 <br>• c0 L4 <br>• 081-17 G3_N_A2 <br>• an unmodified copy of L1 | 8 checks, all NEEDS-DECISION: <br>• one L1 pair relabelled `closure_timeout` <br>• relabelled `budget_exceeded`, both with a broken census and with a closed census plus its log line (that one stops on the memory branch) <br>• relabelled `current_undelivered` <br>• the pair row and its phase disagreeing, in either direction <br>• a phase one pair short <br>• L4 with a `BUDGET-EXCEEDED` line added |

- No saved build-3 log has two closures; I scanned the whole bank. So PQ-B's pass case is a faithful synthetic: `C1_G8_N_A1`'s real log with one extra closure line that carries the log's own real, closed 14,745,600-byte census. `C1_G15_N` will be its first real test.
- Pre-run is confirmed: the ledgers hold only the two `C1_G8_N` rows, and c2/c3 are empty.

## Boundary diff and dry-run totals
- **Boundary** (`081-27-evidence\boundary-diff.json`):
  - `feature_head` `148050f…` → `0d411ca…`
  - `081-23-lib.py` `ce8184d4…` → `87a5b5c1…`
  - also changed: `utc`, and the `reissued` block (the 081-26 block is kept verbatim inside it as `previous_reissue`)
  - the other 33 harness hashes are unchanged, and a check confirms only these keys differ
  - the re-issue script inherited from 081-26 printed `reissued` as newly "added", which is wrong; I corrected that entry in the diff file.
- **Offline preflight replay:** c1, c2 and c3 all exit 0 with no problems (`boundary-verify.json`).
- **Dry run:** 150/150 as expected.
  - It is the 134 checks from 081-26 with identical outcomes, plus 16 new ones.
  - The only detail change is the reworded `why` text on the 081-17 G270 PASS.
  - 081-23 and 081-26 evidence are byte-unchanged; a hash guard covered both trees.
  - I copied the 081-26 wrapper into `081-27-evidence\dryrun-081-27.py`, because running the original writes into `081-26-evidence\dryrun\`.

## Commits and files changed
- **Pushed** `feat/m55-change-evidence` `148050f..0d411ca` (docs only): predictions AMENDMENT 2 (pre-run), journal 081-27, CLAUDE.md status. No tag. The scratch worktree is removed.
- Main checkout is still `m51` at `53bf725` with no tracked edits. `master` is `031a103`.
- **Harness files changed:**
  - `_reviews\081-23-lib.py` (+32/−10, CRLF kept): new `view_rejected_error`, `peak_lines_errors` and `g270_phase_accounted`, plus the `lyra_g270_eval` branch
  - `_reviews\081-23-evidence\prep-boundary.json`
- **New files, all under `_reviews\081-27-evidence\`:** proof script, JSON and log; `predictions-amendment.md`; dry-run wrapper and its output; boundary re-issue and verify scripts and results; pre-edit copies of the lib and the boundary; commit message.
- No plugin source, CaptureBench, `ToCodex\`, tag or container was touched.

## NEEDS-DECISION items
None blocking. One point for chat, which I did not change: the amended G270 rule has no minimum number of measured pairs. A leg where every required pair is refused as `empty_region` would PASS. The oracle would compare 0 rows, the audit's "no measured pair" error is filtered out for G270, and the vacuity guard only checks for zero events or phases. The old wording could not reach that case. c0 L1 measured 179 of 193, so the risk is low. A one-line pre-run guard (at least one measured pair) would close it; it only affects C3.

## Wall-clock
About 45 minutes: reading ~8, lib edits ~5, proof ~10, dry run ~5, docs, commit and push ~8, boundary and replay ~4, wrap-up ~5.
api-equivalent cost (not billed on Max): $9.04 | turns: 64 | running total across all briefs: $2,174.98
