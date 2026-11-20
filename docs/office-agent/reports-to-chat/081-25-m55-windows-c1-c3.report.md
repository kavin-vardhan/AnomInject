# REPORT for 081-25-m55-windows-c1-c3.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-25-m55-windows-c1-c3.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-25 11:39:07 | finished: 2026-09-25 11:46:48 | duration: 8 min | exit code: 0
context at last turn: 362433 tokens (36% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-25-m55-windows-c1-c3.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

Window c1 stopped at its first leg, so c2 and c3 did not start. The build did what was predicted. The failure is a bug in my own 081-23 checking code: one check tests the wrong thing. Nothing was edited, fixed or re-run, and the docs are pushed.

## 2. Plain summary for the owner
- The PC was free, and the first test of window c1 ran cleanly.
- The build did exactly what was predicted for that test. Every check passed except one.
- That one check is wrongly written: it looks for a missing entry where the program writes an entry with empty values. This is a mistake in my test scripts, not in the build.
- By the rules the run stops at the first failure, so the other 85 tests have not run yet.
- Chat needs to approve fixing that check before the tests can continue (about 1.5 h of PC time is still left).

## 3. Per-window table

| window | legs run | passed | invalid | dropped | wall time | postflight |
|---|---|---|---|---|---|---|
| c1 | 1 of 39 | 0 (1 fixture-valid failure) | 0 | 0 | 11:40:01–11:42:18 (2 min 17 s; quiet after 31 s; leg 26 s) | PASS |
| c2 | not started | — | — | — | — | — |
| c3 | not started | — | — | — | — | — |

## 4. Failure
- **Leg:** `C1_G8_N_A1` (StackOBot, gate 8, native order). It was fixture-valid: B1-VALID, foreground 89/89, runner exit 0.
- **Rule:** §D says "SI8 carries no receipt" (`081-23-lib.py:111`, `need(not by.get(8,{}).get('receipt'), …)`).
- **Evaluator output:** `FIXTURE-VALID-FAILURE C1_G8_N_A1: ['case: build-3 D: SI8 carries no receipt']`.
- **Everything else on the leg passed:**
  - The shared measurement checks passed: audit, census, zero `budget_exceeded`, high-water 18.43 MB.
  - The oracle exited 0, with 17 compared, 17 matched, 0 mismatched.
  - SI8 was `unsupported_delivery` (colour) and SI9 was `predecessor_undelivered`.
  - `view_rejected` ≥ 1, one throwaway family was constructed, and SI8's legacy PNG is absent (89 PNGs).
- **Cause read** (full note in `_reviews\081-25-evidence\c1-g8-cause-read.md`):
  - SI8's `receipt` holds only the issue side: token 9 and the submit time. Every delivery field is at its "none" value: `t_drain_ms -1`, `view_family_id 0`, `serving_token 0`, `format 0`, empty `rect` and `extent`.
  - In the source, `ChangeReceiptJson` (`AnomalyChangeStage.cpp:492`) always writes the issue fields, and fills the delivery fields only when a receipt was actually built. So "no receipt built" always looks like this stub; the key is never missing.
  - Banked rows that earlier rounds already called "no receipt" have exactly the same shape: Stage-1 Lyra G15 SI8 `closure_timeout` (journal 081-05) and G4 SI8 `current_undelivered`.
  - So the build behaved as §D predicted, and the check is wrong. The dry run could not catch this: there was no banked build-3 G8 run to test the check on, and the known-bad G8 case failed on other checks first. Recorded as gotcha **G289**.
  - Second slip of this kind in my 081-23 test scripts, after G288.

## 5. Notable readings
- The refusal census at runtime (`ChangeMaxBytes 1` and G10) has **still never run**; it was not reached.
- The legacy vs pre-m55 comparisons were not reached, and neither was the Lyra G270 leg.

## 6. Deviations
- I wrote the console transcript to a new file, `_reviews\081-25-c1-console.txt`.
- My first commit attempt failed because PowerShell split the commit message on its quotes. The push in that same command still went out and carried only the c0 patch commit, `26ac6aa`. I then committed 081-25 from a message file.
- Following the brief, I applied the held c0 patch after the stop. This moves the feature head off `a745ee5`, so every window's preflight will now refuse until `prep-boundary.json` is issued again. Fixing the lib check changes a hash-locked file, so the boundary needs re-issuing anyway.

## 7. Commits pushed
To `feat/m55-change-evidence`, `a745ee5..1ca1ab3`, no tag:
- `26ac6aa` — the held 081-24 c0 docs patch.
- `1ca1ab3` — the 081-25 journal with the full c0–c3 requalification ledger (6 valid readings, 1 failed leg, 85 not run), the CLAUDE.md status block, and G289.

The scratch worktree is removed. The main checkout is still `m51` `53bf725`, master is `031a103`, and the staged exe is 002805CF. No game or editor process is running; only the shared UnrealTraceServer is up.

## 8. NEEDS-DECISION items
1. **The G8 "no receipt" check.** My recommendation: change it to test whether a receipt was built, i.e. `t_drain_ms == -1` with no family, format or rect. Chat would then re-issue `prep-boundary.json` with the new lib hash and feature head `1ca1ab3`. Under that reading, `C1_G8_N_A1` passes on the evidence already banked. Chat can accept that by re-evaluating from the banked evidence, without re-running the leg.
2. **How to resume.** `chunk1-summary.json` records `"steps": []`, so running c1 again unchanged would retry the failed leg. The rules say a fixture-valid failure is never retried. Resuming needs a ruling: either re-evaluate `C1_G8_N` from its banked evidence and start at `C1_G8_S`, or re-run all of c1.
3. Separately noticed, not acted on: the feature-branch source carries code comments (for example `AnomalyChangeStage.cpp:314`, `AnomalyChangeFamilyData.h:16`). That breaks the no-comments rule in the project instructions.

## 9. Wall-clock (IST)

| step | time |
|---|---|
| Reading | 11:38–11:40 |
| c1 (preflight, 31 s quiet wait, 1 leg, postflight) | 11:40:01–11:42:18 |
| Cause read | 11:42–11:50 (~8 min) |
| Docs, commits, push, clean-up | 11:50–11:58 (~8 min) |
| **Total** | **~20 min** |

For the owner: the Gmail, Calendar and other connectors need authorizing in claude.ai settings, and the PDF viewer failed to connect. None of them were needed this run.
api-equivalent cost (not billed on Max): $5.45 | turns: 36 | running total across all briefs: $2,150.67
