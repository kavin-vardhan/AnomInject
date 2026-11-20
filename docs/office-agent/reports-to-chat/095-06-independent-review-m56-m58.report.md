# REPORT for 095-06-independent-review-m56-m58.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-06-independent-review-m56-m58.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-13 18:07:24 | finished: 2026-11-13 18:34:43 | duration: 27 min | exit code: 0
context at last turn: 815974 tokens (82% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-06-independent-review-m56-m58.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

1. **For the owner (3 lines, plain words)**
   - The terrain change (m58) is ready for the test run. The frame-continuity change (m56) works as designed, but 5 smaller problems should be fixed first.
   - None of them makes a normal training label wrong. They affect event bookkeeping, one targeted capture mode, a check-tool loophole, a possible long freeze if file saving hangs, and an optional setting.
   - The bench script must stop killing programs by name before the test run. Two prediction details also need updating, or the test run will stop on a false failure.

2. **Verdict per unit** (review file: `_reviews\095-06-independent-review.md`)

   | Unit | Verdict | BLOCKER | SHOULD-FIX | NOTE |
   |---|---|---|---|---|
   | m56 | READY AFTER FIXES | 0 | 5 | 11 |
   | m58 | READY FOR GATES | 0 | 0 | 7 |
   | CaptureBench `m56-gates` | READY AFTER FIXES | 1 | 1 | 1 |

3. **Every BLOCKER and SHOULD-FIX** ([V] = I re-checked it in source; S2 and S3 also confirmed with a small offline probe)
   - **B1 (bench) [V]:** `run_leg.ps1:230` and `:354` kill every `StackOBot*` process by name, before and after each attempt. The fix is to kill only the PID the script itself started, after checking its start time and path, and to refuse to start a leg if any other StackOBot process is running.
   - **S1 (m56) [V]:** every vetoed event comes back into `annotation.json` as an empty event marked `manifested:false`. The veto removes it from the event list (`SUB:9291`), but the loop for transition-only events adds it back (`SUB:10580-10595`). The docs and the log say a vetoed event leaves no trace.
   - **S2 (m56 readers) [V, probe]:** the new veto tolerance (`verify_capture.py:3374-3377`, also used by the client check kit) lets a session pass that lost a real, non-vetoed event, as long as one other event was vetoed. Probe: exit 0 with the tolerance, exit 1 without the veto.
   - **S3 (m56) [V, probe]:** a targeted `camera_clipping` capture still flags 4 of every 16 frames `settle`, although nothing starts or stops (`SUB:5403-5408`). The label-rule check then fails it with SETTLE-MISPLACED.
   - **S4 (m56) [V]:** the writer's 120 s wait limit applies to each frame, not to the whole run (`AnomalyAsyncWriter.cpp:53-87`). If file saving hangs, the game thread can freeze for hours (80 frames × 120 s ≈ 2.7 h).
   - **S5 (m56) [V]:** with the optional `IAI.Label.Transition*Frames` settings that the readme offers clients, each window now covers K fewer ticks. Frames that exist in both builds lose their `temporal_aa` flag. The release defaults (0/0/0) are not affected.
   - **S6 (bench) [V]:** 095-07's predictions and the G5 comparator do not allow the intended `exposure_dip` change on non-settle frames, so a likely false G5 failure would stop the gate run. P19 also omits 6 `run_summary` keys that the 095-05 ruling accepted.

4. **The merge result:** `git merge-tree` reports conflicts in 5 files, all mechanical.
   - The three source files hold one conflict: both branches add parameters to `WriteRunSummary`. Keep all of them, in the same order everywhere.
   - `CLAUDE.md` and `gotchas.md` are append conflicts. The gotcha numbers do not collide (G574 vs G575–G578).
   - Clean-merging overlaps: vetoed terrain blinks would count in `blink_terrain_events` (follows from S1), and m58's readme row sits in the v1→v2 changelog table.
   - I recommend gating each unit on its own exe from the one cook, then merging m58 first.

5. **Safety:** none.
   - Nothing was built, launched, committed, checked out or stashed, and no process was touched.
   - I ran only the tools' Python selftests and my probe, on copies in the receipts folder: all selftests passed (83, 8, OK, 9, 6).
   - The plugin checkout is still on `fix/m56-capture-continuity` at `b4017ea`, clean; CaptureBench is on `master`, clean.
   - Receipts are in `code-receipts-095-06\`.
api-equivalent cost (not billed on Max): $20.18 | turns: 96 | running total across all briefs: $6,132.11
