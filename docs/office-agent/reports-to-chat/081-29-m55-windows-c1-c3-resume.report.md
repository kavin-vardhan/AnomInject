# REPORT for 081-29-m55-windows-c1-c3-resume.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-29-m55-windows-c1-c3-resume.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-25 23:31:18 | finished: 2026-09-25 23:41:55 | duration: 11 min | exit code: 0
context at last turn: 363741 tokens (36% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-29-m55-windows-c1-c3-resume.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

c1 resumed at `C1_G8_S` and four legs passed. Then `C1_G2_S` used up all three attempts because a person was using the PC. No gate failed on build 3. c2 and c3 were not run, and the harness will not continue past `C1_G2_S` without a ruling.

## 2. Plain summary for the owner
- The run picked up where it stopped last time (`C1_G8_S`). The earlier leg was not re-run.
- Four tests passed cleanly: `C1_G8_S`, `C1_G9_N`, `C1_G9_S` and `C1_G2_N`.
- During the next test someone used the PC. The mouse moved, then the Start menu and the desktop took over the screen. The game lost focus, so all three tries of `C1_G2_S` were invalid.
- This was not a build problem. The test code has no check for a person at the keyboard, and it retries straight away.
- Chat needs to allow `C1_G2_S` another set of tries, and ideally add an idle check. After that, c1 → c2 → c3 must run with nobody touching the PC (about 1.5 h).

## 3. Per-window table

| window | legs run | passed | invalid | dropped / below-floor | wall time | postflight |
|---|---|---|---|---|---|---|
| c1 | 5 of 38 still owed (G8_S, G9_N, G9_S, G2_N, G2_S) | 4 (+ `C1_G8_N` PASS-REEVALUATED from before) | 1 leg, 3 attempts (`C1_G2_S`), recorded NOT-RUN | 0 | 18:02:11–18:08:09 UTC (6 min; quiet after 31 s) | PASS |
| c2 | not started | — | — | — | — | — |
| c3 | not started | — | — | — | — | — |

## 4. Failure (environmental, not a measurement or gate result)
- **Leg:** `C1_G2_S`, attempts A1, A2 and A3.
- **Rule:** fixture check "whole-leg foreground failed". After three such attempts the harness wrote a `NOT-RUN` row and raised `Stop(2) C1_G2_S INVALID-FIXTURE-EXHAUSTED`. Chunk exit was 2.
- **Evidence of a person at the PC:**

| attempt | game in foreground | what held the foreground | cursor |
|---|---|---|---|
| A1 | 82 of 107 samples | `SearchHost.exe` (Start / search) for the rest | moved through 58 positions |
| A2 | 0 of 480 | `explorer.exe` (desktop shell) | parked on the taskbar row (1434,1077) |
| A3 | 0 of 485 | `explorer.exe` | parked on the taskbar row |

  - The four passing legs all had a stationary cursor and full foreground.
  - Input idle time read **0.34 s** after postflight, so the PC was being used while the run finished.
- **No gate predicate was evaluated on `C1_G2_S`.** All three attempts were B1-VALID on pose. The build is not implicated.
- **Why I stopped instead of re-running:** because of the `NOT-RUN` row, `stop_on_recorded` halts every new invocation before it launches anything (the G290 hardening, working as designed). The brief's "re-running resumes" applies to NOT-QUIET and to a foreign editor mid-leg, not to a used-up attempt budget.
- **What continuing would take:** either a hand edit of the harness-written ledger, or a change to the hash-locked lib plus a boundary re-issue. I did neither.
- Cause read: `_reviews\081-29-evidence\c1-g2s-cause-read.md`. Console output: `_reviews\081-29-c1-console.txt`.

## 5. Notable readings
- **New passes on build 3**, oracle compared = matched = 17 on each unless noted, speed ratio about 1.0:

| leg | events | pairs | measured | high-water | PNGs |
|---|---|---|---|---|---|
| `C1_G8_S` | 5 | 19 | 17 | 15.67 MB | 89 |
| `C1_G9_N` | 5 | 19 | 17 | 18.43 MB | 90 |
| `C1_G9_S` | — | — | 17 | 16.59 MB | — |
| `C1_G2_N` | 5 | 20 | 19 | 24.88 MB | — |

- The refusal census at runtime (`ChangeMaxBytes 1`) and G10 have **still never run**; they are not reached yet.
- Legacy vs pre-m55 (evidence ON and OFF) was not reached.
- The Lyra G270 leg was not reached.

## 6. Deviations
- I wrote the console output to new files `_reviews\081-29-c1-console.txt` and `.err.txt`, and made a new folder `_reviews\081-29-evidence\` holding the cause read and a ledger snapshot.
- I did not edit any harness file, the ledger or the boundary.

## 7. Commits pushed
- `feat/m55-change-evidence` `69c9b30..070013f` (docs only):
  - journal `docs/sessions/2026-09-25-081-29-code-m55-c1-resume-stop.md`, with the complete c0–c3 ledger;
  - CLAUDE.md status block;
  - gotcha **G292**: the quiet gate sees processes, not a person, and a retry loop with no idle check lets one interaction use up all three attempts.
- The ledger totals are 92 legs:
  - 6 c0 readings;
  - 5 c1 passes;
  - 1 environmental exhaustion;
  - 80 not run.
- The scratch worktree is removed. The main checkout is still `m51` `53bf725`, `master` is `031a103`, the staged exe is 002805CF, and there is no tag.

## 8. NEEDS-DECISION items
1. **How to resume `C1_G2_S`.** It needs one of:
   - a ruling that voids the three environmental attempts and grants a fresh budget (a ledger annotation in the style of PASS-REEVALUATED);
   - a lib change, which needs a boundary re-issue.
2. **Recommended alongside (1): a pre-attempt check** on input idle time and on whether the game owns the foreground, so that a person at the PC becomes a NOT-QUIET wait instead of burning attempts. It is a lib change plus a boundary re-issue, and it could be dry-run on the banked A1–A3 records.
3. **Owner:** the next c1 → c3 run needs the PC left completely untouched for about 1.5 h. The quiet gate cannot detect a person, just as it cannot detect a Blender render.

## 9. Wall-clock
- **c1:** 6 min. Preflight and quiet wait took 44 s; legs ran 18:03:00–18:06:31; postflight followed.
- **c2 and c3:** not run.
- **Session total:** about 20 min, including the cause read, the docs and the push.
api-equivalent cost (not billed on Max): $5.18 | turns: 32 | running total across all briefs: $2,184.72
