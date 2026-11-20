# REPORT for 081-33-m55-am5-colour-fix-release.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-33-m55-am5-colour-fix-release.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-26 01:55:02 | finished: 2026-09-26 02:32:06 | duration: 37 min | exit code: 0
context at last turn: 536613 tokens (54% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-33-m55-am5-colour-fix-release.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

## Summary for the owner
- The colour check is fixed the way chat ruled. The old-output check now forgives a frame whose mask was lost only if that frame's picture file exists, at the right size and format. It never compares pixels.
- Every required test gave the expected result. CB-N is now officially recorded as passing, re-checked from the saved data with nothing re-run.
- The harness is unlocked again: the next bench window starts at `CB_S1`.
- Nothing was launched or built.

## Proofs (evidence under `_reviews\081-33-evidence\`)

| Proof | Expected | Actual | Evidence |
|---|---|---|---|
| 1: colour (A.6) | missing file → FAIL; different size → FAIL; present and matching → excused | as expected. The missing and resized (1279×720) files FAIL with nothing excused. A file with every pixel painted magenta but the same 1280×720 format is excused: this was 081-32's bad test, now read correctly | `a6-proof.json` |
| 1: colour controls | the 081-32 lib (no A.6) PASSes the same missing and resized files | as expected, so A.6 is what makes them fail | `a6-proof.json` |
| 1: colour, extra cases | wrong pixel format, zero-byte file, a comparison leg missing its file → FAIL | 3 of 3 | `a6-proof.json` |
| 1: derived field off by 2 | FAIL | 6 of 6, one per field. Each fails on exactly its own field while the frame stays excused, so it fails for its own reason | `a6-proof.json` |
| 1: the 8 earlier negatives, 2 frequency controls, 5 symmetric cases | same verdicts as 081-32 | 8/8, 2/2, 5/5; each case now also checks which frame was excused | `a6-proof.json` |
| 2–5 | unchanged | A positive 2/2 (A.6 holds against all 5 other legs); B 8/8; C 10/10. **Proofs 1–5 total 49 of 49** | `a6-proof.json`, `proof\cb-n-a6.json` |
| 6: no flips | earlier verdicts unchanged | 16 comparisons. Against the recorded verdicts, the only change is CB-N FAIL → PASS. Against the 081-32 lib: no changes and identical problem lists. Readings identical on 30/30 legs | `proof6-no-flips.json` |
| 7: CB-N | PASS-REEVALUATED | committed. The c1 ledger went from 6 to 7 rows; the row cites both rulings and is dated 02:17 IST. The saved data reproduced every 081-31 file hash, colour frames included | `cb-n-reevaluation\`, `commit-receipt.json` |
| resume | c1 resumes at `R23_CB_S1` | 5 of 5, run on the real committed ledger | `resume-proof.json` |

One problem was caught before it produced a false result. 081-32's test copies left out the colour frames, so under A.6 every negative would have failed because of the missing picture rather than for its stated reason. The copies now include the colour frames.

I also checked something I expected to break and was wrong about. A frame that is not excused still FAILs in synth order, through the rule that flags fields present on one binary only. So "the group fails by the ordinary rules" holds in both tick orders on this data (`a6-synth-order-observation.json`, a reading, not a gate).

## Boundary
- Lib `bb4fbb9d` → **`9901be63`**. The 081-32 draft `c36a60c2` was never pinned.
- Feature head `66976ea` → **`50cc235`**.
- Window runner unchanged at `56e86f2e`, and the other 33 pinned harness files are unchanged.
- `prep-boundary.json` `7204caed` → **`b034d5c5`** (`boundary-diff.json`).
- **Preflight replay:** c1, c2 and c3 all pass with no problems, both with process checks stubbed and with the real process list. The box is currently quiet (`preflight-replay.json`).
- **Full dry run:** 202 of 202 before the re-issue and 202 of 202 after. That is 081-32's 198 unchanged plus 4 new A.6 cases, and no earlier evidence folder changed.
- **081-34 command:** `python D:\IntrusiveAnomalies\_reviews\081-23-window.py c1 540 081-34`
  - Budget tag `081-34`, which gets a fresh 45-minute wait budget.
  - It starts at `R23_CB_S1`, then runs CB-S, MAIN-N and MAIN-S.

## Deviations
- **A.6 checks against every other leg in the group, not one.** If a comparison leg itself has no colour file, the frame is not excused. This is the cautious reading, and a test case covers it.
- **Extra test cases:** three more colour cases, and the off-by-2 test run once for each of the six fields instead of once.
- **Test copies now include the colour frames.** The changes made to each copy are byte-for-byte 081-32's.
- **I ran a 081-33 copy of `reevaluate-cb-n.py`** so the row cites both rulings and 081-32's evidence isn't overwritten. The archive went under `081-32-evidence\c1-invocation-3-081-31\`, as the ruling accepted.
- **Order changed:** docs were committed and pushed before the boundary re-issue, not last. Committing after it would move the pinned head and block 081-34. So the journal cannot contain the final boundary hashes; they are in the evidence and in this report.
- **A slip I fixed:** rewriting the memory index through PowerShell double-encoded its em-dashes. I repaired and verified it (4 restored, none left). No committed file was affected; the commit's diff was 205 insertions and 3 deletions, all intended.

## Commits pushed
`feat/m55-change-evidence` `ef2ffca..50cc235`, docs only, from a scratch worktree that has since been removed:
- the 081-33 journal;
- predictions AMENDMENT 5 marked **released**, citing 081-33 for A.6 and the six fields;
- the CLAUDE.md status block;
- G296 marked resolved (plus the test-copy trap), and G294 marked in effect.

The main checkout is still `m51 53bf725`; `master` is `031a103`, same as origin. No build, merge or tag. Harness-side changes (not in git): the lib, `prep-boundary.json`, the c1 ledger row, and the new evidence folder.

## NEEDS-DECISION
None.

The PDF-viewer connector failed to connect, and several other connectors need authorising in their settings. None were needed for this run.
api-equivalent cost (not billed on Max): $13.85 | turns: 94 | running total across all briefs: $2,245.45
