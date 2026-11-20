# REPORT for 081-30-m55-person-present-gate.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-30-m55-person-present-gate.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-25 23:48:45 | finished: 2026-09-26 00:31:18 | duration: 43 min | exit code: 0
context at last turn: 734804 tokens (73% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-30-m55-person-present-gate.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

Both rulings are in the harness and all 8 required proofs came out as expected. The boundary is re-issued, the full dry run passes 182/182, and the docs are pushed. Nothing was launched: no game, window, chunk, leg or build.

## Plain summary for the owner
- The three `C1_G2_S` tries spoiled by someone using the PC are now voided. The next c1 run starts that test again as attempt 4, with 3 fresh tries.
- Before every try, the test harness now checks that nobody has touched the keyboard or mouse for a full minute. If someone has, it waits instead of burning tries.
- That waiting shares the existing 45-minute limit per window. When the limit runs out, the window stops cleanly and nothing is marked as a failure.
- A try that loses the game window while there are signs of a person is not counted, up to 5 per test. Test results themselves are never changed.
- A PC-free stretch of about 1.5 hours is still needed for c1 → c2 → c3.

## What the runners do with input, focus and the cursor, and how the harness excludes it
- **What I found.** All four runners (stack, legacy, nohold, Lyra) inject exactly one kind of input: an ALT key tap inside `Force()`, used to take the foreground back. None of them moves the cursor; they only read its position.
  - Outside startup, `Force` fires only right after a sample where the game is not in the foreground.
  - When the game gets focus back it may move the cursor itself. I count that as the harness's action, not a person's.
- **Pre-attempt gate.** It waits for ≥ 60 s of input idle, read with `GetLastInputInfo`.
  - Every runner call is recorded as a harness window (its start and end times).
  - If the latest input falls inside a runner window whose attempt showed no person evidence, that input was the runner's own ALT tap, and the gate lets the next attempt go.
  - This is safe because every runner was released by a gate that had already seen 60 s of human idle.
  - If that attempt did show person evidence, the gate waits 60 s from its last input.
  - It polls every ≤ 15 s and logs `person-present-wait` with the idle reading.
- **Budget.** The gate's waits and the window runner's existing wait for other programs to close are booked in one file per window (`wait-budget.json`, keyed by a per-brief tag). When 45 min are spent, the window exits 4 and never records NOT-RUN.
- **Person evidence.** It is judged only inside the steady part of the attempt (after snapshot sampling starts and before the capture ends). Any one of these counts:
  - a shell process (SearchHost, StartMenuExperienceHost, ShellExperienceHost, explorer) held the foreground;
  - the cursor moved, and not within ±0.75 s of a focus action;
  - an input registered away from any focus action.
- **Live data.** A read-only monitor records input, cursor and foreground every 0.25 s during every attempt.

## Proof table

| # | expected | actual | evidence (`_reviews\081-30-evidence\`) |
|---|---|---|---|
| 1 | banked `C1_G2_S` A1–A3 → ENV-INTERRUPTED, with reasons | A1: SearchHost held the foreground on 43 samples, and the cursor moved 36 times. A2: explorer on 570 samples. A3: explorer on 575 samples. Both process IDs are confirmed as alive since boot. Replaying A1–A3 through the real `run_measure_leg` gives 3× ENV-INTERRUPTED, then the next try (`A4`) passes, with no NOT-RUN. | `pp-proof.json`, `pid-attestation.json` |
| 2 | no person evidence on the four 081-29 passes or any banked c0/c1 record | 0 of 13 records flagged (the 4 passes included); the dry run adds 3 NoHold legs, also 0 | `pp-proof.json`, `dryrun\` |
| 3 | foreground failure without person evidence stays INVALID-FIXTURE | two banked 081-11 legs (focus lost to another process, cursor still) stay INVALID-FIXTURE. A synthetic case does too. Three replays still use up the budget: NOT-RUN, stop code 2. | `pp-proof.json` |
| 4 | gate: 0.3 s waits; ≥ 60 s proceeds; harness's own input is not a person; budget runs out → exit 4, no NOT-RUN | waits 15/15/15/14.7 s, then proceeds at 60 s. 75 s idle proceeds at once. Own ALT tap → proceeds, and both control cases wait. A person who never leaves → 2700 s, exit 4, nothing launched, ledger empty. Budget shared with the quiet wait: 10 s left → exit 4. Window start: 6/6. | `pp-proof.json`, `pp-walk.json` |
| 5 | 5-cap → exit 4, no NOT-RUN | 5 ENV-INTERRUPTED → exit 4, 0 tries counted. Re-running → exit 4 before any launch. The Lyra and nohold/legacy classifiers give the expected results. | `pp-proof.json` |
| 6 | fixture-valid FAIL with the flag → verdict unchanged, sent to chat as NEEDS-DECISION | stays FIXTURE-VALID-FAILURE with `needs_decision` set, and the stop message shows `person_evidence=true`. Controls: a flagged PASS stays PASS; an unflagged FAIL has no flag text. | `pp-proof.json` |
| 7 | ENV-VOIDED clears with a fresh budget of 3; NOT-RUN alone halts; ENV-VOIDED does not clear a FAIL | the real ledger resumes at `C1_G2_S_A4`, and a copy of it runs A4–A6 then NOT-RUN. The ledger without ENV-VOIDED halts with code 2. After FIXTURE-VALID-FAILURE, FAIL or NEEDS-DECISION it halts with code 1. The row writer refuses misuse. | `resume-proof.json`, `pp-proof.json`, `env-void-c1-g2s.json` |
| 8 | boundary re-issued, full dry run all green | dry run 182/182: the 153 existing checks unchanged, plus 29 new. All 86 remaining legs walked: gate, then quiet check, then launch, for every leg kind. Offline preflight passes for c1, c2 and c3. | `dryrun\comparison-vs-081-28.json`, `pp-walk.json`, `boundary-verify.json` |

## Boundary
- **Lib:** `7cb583f6…` → `bb4fbb9d…`
- **Window runner:** `63837074…` → `56e86f2e…`
- **Feature head:** `69c9b30` → `66976ea`. 081-29's docs commit had moved the head without a re-issue, so the next preflight would have refused anyway.
- The other 32 pins are unchanged. Details are in `boundary-diff.json`.
- **Dry run:** 182/182.

## Deviations
- I added **AMENDMENT 4** to the requalification predictions. Its §0 retry rule would otherwise be untrue. It is marked as scheduling only; no prediction changed.
- `081-23-window.py` takes an optional budget tag and books its quiet waits into the budget.
- I copied 081-29's c1 files to `c1\invocation-2-081-29\` before appending the ENV-VOIDED row, the same way 081-26 did.
- **G293.** My first `window.py` called `L.budget_left()`, which the lib did not define, even though the 42 lib-level proofs passed. On the bench it would have killed every window at start. Only running the real `window.py` caught it. It was fixed and every proof was re-run.
- The first commit subject picked up a BOM (G141 again). It was amended before pushing.

## Commits pushed
- `feat/m55-change-evidence` `070013f..66976ea`, docs only:
  - journal `docs/sessions/2026-09-26-081-30-code-m55-person-present-gate.md`;
  - G292 marked mitigated, plus the new G293;
  - AMENDMENT 4;
  - CLAUDE.md status block.
- Unchanged: main checkout `m51 53bf725`, `master` `031a103`, no tag, staged exe `002805CF`, CaptureBench and `ToCodex\`. The scratch worktree is removed.

## NEEDS-DECISION items
None. Three non-blocking notes for chat:
- **How to start 081-31:** `081-23-window.py c1 540 081-31`. The tag gives each window a fresh 45-minute budget.
- **Unexplained cursor moves at startup.** On 3 passing legs the cursor moved before the game launched. That is outside the evidence window, so it is not treated as a person; the cause is not established.
- **Limits of the gate:**
  - It cannot see a person who is present but touches nothing.
  - The input check has never seen real bench data; 081-31 is its first run.
  - If Explorer took the foreground with nobody there, the cap stops the window at exit 4, not NOT-RUN.

Unrelated: the claude.ai connectors (and others listed) need authorizing in their settings, and the PDF viewer failed to connect. None were needed here.
api-equivalent cost (not billed on Max): $19.53 | turns: 87 | running total across all briefs: $2,204.25
