# REPORT for 081-28-m55-g270-vacuity-guard.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-28-m55-g270-vacuity-guard.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: medium
started: 2026-09-25 12:53:09 | finished: 2026-09-25 13:03:18 | duration: 10 min | exit code: 0
context at last turn: 351249 tokens (35% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-28-m55-g270-vacuity-guard.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

## Summary
- A Lyra G270 leg can no longer PASS on nothing. It now needs at least 3 events that each have a measured required pair. A clean leg below that count gets OBSERVED-BELOW-EVENT-FLOOR, which does not stop the campaign. Every stop branch is checked before this new floor. All 12 proof cases came out as expected.
- The boundary is re-issued for exactly the new lib hash and the new feature head. The offline preflight replay passes for c1, c2 and c3. The dry run is 153/153. Nothing ran on the PC: no Unreal, no build, no bench.

## Proof table (`_reviews\081-28-evidence\guard-proof.json`, 12/12)

| case | expected | got |
|---|---|---|
| c0 L1, with the oracle run: 50 events, 45 with a measured pair | PASS | PASS (oracle 179/179 matched) |
| c0 L4, with the oracle run: 42 of 42 events measured | PASS | PASS |
| 081-17 LYRA_G3_N_A2 (build 2), 5 of 5 events measured | PASS | PASS |
| control: L1 copy with its content unmodified | PASS | PASS |
| L1 copy with every required pair relabelled `empty_region` (rows and phases) | OBSERVED-BELOW-EVENT-FLOOR | OBSERVED-BELOW-EVENT-FLOOR (0 measured events) |
| L1 copy where only 2 events keep a measured pair | OBSERVED-BELOW-EVENT-FLOOR | OBSERVED-BELOW-EVENT-FLOOR |
| extra case: L1 copy where exactly 3 events keep a measured pair | PASS | PASS |
| 081-27 mutation: one pair relabelled `closure_timeout` | NEEDS-DECISION | NEEDS-DECISION |
| 081-27 mutation: one pair relabelled `budget_exceeded`, no census | NEEDS-DECISION | NEEDS-DECISION (census) |
| 081-27 mutation: `budget_exceeded` with a closed census and its log line | NEEDS-DECISION | NEEDS-DECISION (memory gate) |
| 081-27 mutation: L4 with a `BUDGET-EXCEEDED` line added | NEEDS-DECISION | NEEDS-DECISION |
| extra case: all-`empty_region` copy plus one `closure_timeout` | NEEDS-DECISION | NEEDS-DECISION (the stop is checked before the floor) |

## Boundary diff and dry run
- **`081-23-lib.py`:** `87a5b5c1…` → `7cb583f6…`. The pre-edit copy reconstructs `87a5b5c1` byte-exact, and the file keeps its CRLF line endings.
- **`feature_head`:** `0d411ca` → `69c9b30`.
- **Also changed:** `utc`, and the `reissued` block. The 081-27 block is kept inside it as `previous_reissue`.
- **Unchanged:** the other 33 harness hashes.
- **Cosmetic flaw in the diff file:** the copied diff script still lists `reissued` among the "unchanged other keys", even though it changed. `prep-boundary.json` itself is correct.
- **Offline preflight replay:** c1, c2 and c3 each exit 0 with no problems (`boundary-verify.json`).
- **Dry run: 153/153 as expected.** It is the 150 checks from 081-27 with identical outcomes, plus 3 new ones. The only wording change is in two G270 PASS `why` texts, which now include the measured-event count. The 081-23, 081-26 and 081-27 evidence folders are byte-unchanged.

## Commits and files
- **Pushed** `feat/m55-change-evidence` `0d411ca..69c9b30` (docs only): predictions AMENDMENT 3 (pre-run), journal `2026-09-25-081-28-code-m55-g270-vacuity-guard.md`, CLAUDE.md status block. Committed from a scratch worktree, which is now removed.
- The local branch ref was moved to match origin. The main checkout is still `m51` `53bf725` with no tracked edits; `master` is `031a103`. No tag.
- **Harness files changed:** `_reviews\081-23-lib.py` (a `measured_events` count plus the new PASS / OBSERVED-BELOW-EVENT-FLOOR branches, with `measured_events` added to the result) and `_reviews\081-23-evidence\prep-boundary.json`.
- **New files, all under `_reviews\081-28-evidence\`:** proof script, JSON and log; `predictions-amendment.md`; dry-run wrapper, its log and the `dryrun\` folder; boundary re-issue and verify scripts, plus `boundary-diff.json` and `boundary-verify.json`; the pre-edit lib copy; the pre-081-28 boundary copy; `commit-message.txt`.
- No plugin source, CaptureBench, `ToCodex\`, container, `master` or `m51` was touched.

## NEEDS-DECISION
None.

## Wall-clock
About 25 minutes: reading ~4, lib edit ~2, proof ~6 (2 of them the oracle), dry-run wrapper and run ~6, docs, commit and push ~4, boundary and replay ~2.
api-equivalent cost (not billed on Max): $4.56 | turns: 26 | running total across all briefs: $2,179.54
