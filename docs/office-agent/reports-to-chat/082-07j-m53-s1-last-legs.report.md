# REPORT for 082-07j-m53-s1-last-legs.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07j-m53-s1-last-legs.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-28 05:08:16 | finished: 2026-09-28 05:21:45 | duration: 13 min | exit code: 0
context at last turn: 387005 tokens (39% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07j-m53-s1-last-legs.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

S1 stopped on its own stop rule at leg `G4_CAN_UV`, which the harness recorded as NOT-RUN (stop code 2). The feature behaved correctly: the test assumes the game window is not yet focused when capture starts, but our own test runner forces focus before that point, so the test can never pass as written. Postflight passed and the docs are pushed.

**2. Plain summary for the owner**
- **Is S1 complete?** No. 158 of 193 legs are done; 4 passed this run and then S1 stopped at a test that can never pass as written. 35 legs have not run.
- **Did "undo" and failure rollback leave everything clean?** A normal apply followed by undo restored the original material exactly. The failure-rollback tests are among the 35 unrun legs, so that is not yet proven.
- **Does texture corruption work on the StackOBot main level's floor once loaded?** Not answered yet. Those legs are also after the stop.

**3. S1 gate table, final across 082-07b / 07e / 07h / 07j** (live rows only)

| gate | class | legs / rows | result | key reading |
|---|---|---|---|---|
| G-ID | Q | 14 | 14 PASS | identity 0; thinnest wrong copy is U-C5 sRGB-twice at 90 |
| PRE-CLIP MASKED | PRE | 1 | PASS | 2,048 px on 57 of 57 frames |
| G-ID-M | Q | 29 | 29 PASS | identity ≤ 1; thinnest is M-D-MAG texelshift at 34 against a floor of 16 |
| G-ID-M-D | D | 6 | READING | — |
| G3 | Q | 43 | 43 PASS | — |
| G-RD | Q | 2 | PASS | drift 0 of 92 |
| G-BIND (1) / (3) / (4) | Q / Q / D | 3 | PASS / PASS / READING | ring 4, far 1 |
| G-REASON (Q rows) | Q | 17 | 17 PASS | new this run: A5 `below_size_policy:min_64`, A6 `map_set_over_cap:9_of_8`, V2 `over_budget` (step V2) |
| G-REASON T10 | D | 0 | FIXTURE-CANNOT-EXERCISE (premise) | recorded this run as declared |
| G-REASON S3O / A2 / S8 | — | — | UNRUN | — |
| G6 | Q | 1 | PASS | — |
| G4 LEVEL-CHANGE / RECREATED | Q | 2 | UNEXERCISED | declared |
| G4 FIN-UV | Q | 1 | **PASS** | 1 apply, 1 revert, ledger balanced, `M_TC_UC1` restored exactly |
| G4 CAN-UV | Q | 1 | **NOT-RUN (stop)** | section 4 |
| G4, the other 24 rows | Q | — | UNRUN | — |
| G-COLL F-SYN / F-MW, BAND/G-ID/G3 FMW, MW-FLOOR, G3-COUNT | — | — | UNRUN | — |
| TRIP / G11 | Q | 158 | 158 PASS | — |
| LUMA / BAND F-SYN, FIXTURE-NAMES, G0 ×2 | — | — | PASS / VALID / PASS / READING | — |

**4. The failure**
- **Gate and rule:**
  - `G4_CAN_UV` was INVALID on 3 of 3 attempts with `protocol: CANCEL-LINE-NOT-SEEN`, then recorded NOT-RUN.
  - The harness refuses to re-run a NOT-RUN leg without a ruling (`082-07-lib.py:477`), so I did not launch `082-07j-2`.
- **Output of each attempt:**
  - `Capture run STARTED` at frame 1, then `FINISHED … 0 frame(s)` 7 ms later.
  - `fires_applied=0`, with no m53 APPLIED / REFUSED / LEDGER line.
  - No `CANCELLED before focus` line.
  - Runner exit 1.
- **Cause, from source and the attempt records:**
  - `StartRun` waits for focus only when the window is **not** focused (`AnomalyCaptureSubsystem.cpp:3362-3373`). Otherwise it starts synchronously, and a Stop in the same batch finishes the run instead of cancelling it.
  - `082-07-run.ps1:59-68` (`Hold-Focus`) forces the window to the foreground from 400 ms after launch. `launch.json` shows focus at 0.9 / 1.3 / 1.2 s; frame 1 came at about 2.3 s.
  - **The gate's premise is wrong, not the feature.** This is G320 / G328's shape on the focus axis, now filed as gotcha G332. The stub proofs could not see it because the stub cannot model focus.
  - `G4_CAN_NM` has the same definition and would fail the same way.
- **Evidence:**
  - `_reviews\082-07-evidence\run\M53S1_G4_CAN_UV_A{1,2,3}\` (`game.log`, `launch.json`, `command.json`);
  - `_reviews\082-07j-evidence\window-082-07j.out.txt`.

**5. The owed in-engine proofs**
- **P2-1 and P2-3:** CONFIRMED over the 156 accepted m53 capture legs, read-only. The live-file hashes were unchanged before and after.
- **P2-2, P2-5 and P2-6:** NOT OBTAINED; their legs are after the stop. G4 FIN-UV shows a clean apply and revert with a balanced ledger, but that is not the FailStep path.

**6. Readings**
- **MW-FLOOR:** not obtained, because all six floor legs are unrun. The MainWorld "zero yield" statement stays UNVERIFIED.
- **G-COLL F-SYN / F-MW and the F-MW common-event counts:** unrun.
- **G0 against §R14:** unchanged. F-SYN reads 77 targets and 0 findings; F-MW reads 10 targets and 6 findings (ruled). A5, A6 and V2's finals at the decision frame equal their census finals, as predicted.
- **Other readings:**
  - `REASON_A5` needed a second attempt (A1 was `whole-leg foreground failed`, a validity condition).
  - The CAN attempts carry `person_evidence: true` (1–2 input events). A candidate cause, not established, is the runner's own synthetic Alt key in `[M53Fg]::Force`. It did not decide their status.

**7. Deviations**
- None to the run. I did not launch `082-07j-2`, because the lib forbids re-running a NOT-RUN leg.
- The gate-table, ledger and readings generators are adapted copies of 082-07h's, in `_reviews\082-07j-evidence\`.
- **Boundary `456f4146` is now stale:** the docs commit moves the feature head.

**8. Commits pushed**
- `848651e` on `feat/m53-uv-normal-corruption` (`570352e..848651e`, local = origin). It carries:
  - journal `2026-09-28-082-07j-code-m53-s1-last-legs-stop.md`, with the full S1 ledger of 174 attempts plus the NOT-RUN record across 07b / 07e / 07h / 07j, superseded attempts and T10 marked;
  - the status block;
  - gotcha G332.
- No tag. `m51` is still `53bf725` and `master` is still `b5f15a3`.

**9. NEEDS-DECISION**
- **CHAT-DECISION REQUIRED: the G4 CAN premise.** Candidates, none tested:
  - **(a)** Re-encode CAN as "stop before the first fire": accept a FINISH with 0 frames, `fires_applied=0` and no m53 line. The declared cost is that the cancel-before-focus branch goes unexercised.
  - **(b)** A runner mode for the cancel legs that does not force focus. This is not deterministic.
  - **(c)** Declare CAN-UV/NM class D FIXTURE-CANNOT-EXERCISE (premise), owed to S2 with a bench-only "treat as unfocused" lever. That needs a build.
- **Whatever the ruling, before the remaining 35 legs can resume:** a supersede act for the three CAN attempts and the NOT-RUN record, an amendment, and a boundary re-issue.

**10. Wall-clock:** about 13 min (05:08:46 → 05:21 IST). The window itself took 7.7 min, and the wait budget used was 177 s of 2,700.
api-equivalent cost (not billed on Max): $6.77 | turns: 50 | running total across all briefs: $2,797.56
