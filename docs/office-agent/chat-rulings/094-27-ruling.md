# 094-27 — Chat ruling: the runtime kit and the real smoke session worked, but our own checker misread one row and stopped the run. Fix it against real files, keep going after non-crash failures, and run again today (094-27b)

**Date:** 2026-10-30, about 10:20 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Codex's report:** `_reviews\094-27-codex-report.md` (07:07:10–08:38:55 IST, 5,505 s, exit 0). Its verdict: `NEEDS-DECISION | Runtime kit prepared; real Smoke stopped on an incorrect cleanup reader; all runtime gates and pictures remain unproved.`
- **Claude Code's review:** `_reviews\094-27-code-review.md`. 49 rows: 40 OK · 1 not run · 1 stopped · 2 N/A · 4 carried · 1 GAP. Also G1–G3 and O1–O8.
- **Mailbox report 094-27:** NEEDS-DECISION, 114 min. The watcher is idle.

## Verdict
**The kit is accepted as built** (`599ef70`):
- 18 single-use legs: smoke, load, two PIE legs, GC, 12 A7 legs and admission, with limits of 300, 600 or 900 s;
- Code checked 16 of 16 of the design note's 4.25 citations;
- Codex's 12 suites passed, as did Code's 56 of 56 can-fails, the 5 rehearsals, and 18 dry runs with 0 launches.

**The real smoke session worked:**
- PID 2344, 08:08:27–08:08:43 IST, 15.3 s, exit 0;
- all 31 Python bindings were found, and there was no error, no ensure and no upload sign;
- exactly 1 launch, and nothing protected changed;
- canary 0 hits, with the control found 3 of 3;
- the EA8 exit proof passed, and `m51` matched P9 four times.

**The STOP was our checker's fault (Code's G1):**
- the reader (`runtime_readers.py:26`) expected a `world_snapshot_skipped` row labelled `cleanup`;
- the probe writes `world_cleanup` with `actor_snapshot="skipped"` (`EuropaPhaseAWorldProbe.cpp:211-225`), which the older `fixture_leg.py:32` already reads correctly;
- the stand-in and the reader test were both written from the reader's own expectation, so the rehearsal passed a wrong reader.

**Part of this is mine:** FB4 asked for stand-in rehearsals, but it never required the readers to be tested on the real probe files we already had (094-26d's two sessions). FH2 makes that a standing rule.

**Codex was right** not to turn the FAIL into a pass by hand after the fact.

## Rulings

### FH1. 094-27 is accepted as evidence
- The 09427 kit is spent and stays spent.
- Its smoke files are kept, and they become real test files for FH2.

### FH2. G1: fix the cleanup reader, and test every reader on real files (a standing rule)
**The fix:** the cleanup check reads `world_cleanup` with `actor_snapshot="skipped"`, as the probe writes it.

**The real-file rule, from now on:**
- **(a) Every reader requirement names its emitter.** Every row kind and field a reader requires is cited to the probe source line that writes it. A requirement with no emitter is a defect. Code checks this line by line.
- **(b) Real files first.** Every reader runs against every real probe file kept so far:
  - 094-26c's EA_Core session;
  - 094-26d's two sessions;
  - 094-27's smoke.

  Where a reader's rows occur in those files, it must pass on the real file and fail on its mutations: a row missing, duplicated or out of order, and an unsafe stop.
- **(c) Stand-ins are mutated copies of real files** wherever a real file exists. They are never written from the reader's own expectation.
- **(d) Rows that no real session has written yet** (PIE worlds, travel, GC, anomaly apply and revert, screenshots) rely on (a) until the first real leg writes them. FH4 covers a reader that then turns out wrong.

**The proof for this fix:** the fixed reader passes 094-27's real smoke files, the exact files the old one failed.

### FH3. A failed leg no longer ends the run (O1; amends FB5, FB7's smoke rule and FG3)
**The run ends early only for:**
- **a crash of our editor or game:** it ends abnormally, not through its normal exit path (FB5);
- **a protected change:** Content, config, a descriptor, the `.uproject`, a bound binary or a manifest;
- **any upload sign, or a canary hit;**
- **a smoke session whose editor** fails to start, exits non-zero, or lacks a binding. Then every leg would fail the same way;
- **the end of the window.**

**Everything else is recorded, its evidence is kept, and the run goes on to the next leg:**
- a reader or gate FAIL;
- a leg reaching its session limit;
- an in-session memory or drive stop. The next leg waits at the admission gate as usual (EZ2);
- too few valid events;
- **a new crash or ensure report folder when the process exited normally.** That fails its leg (this amends FG3).

**Why:** the legs are separate sessions, each with its own evidence. Stopping at the first checker bug cost us 17 legs, and finding one bug per night is too slow.

### FH4. Verdicts may be re-read from kept evidence
- Each leg's raw evidence (probe rows, logs, screenshots) is hashed when its session ends.
- **If a reader is later found wrong,** a fixed reader re-reads the same files, after FH2's tests and Code's review. That verdict stands without a new session, as long as the files still match their hashes.
- A FAIL is never re-labelled by hand.

### FH5. G2: adopt the crash monitor by its `-MONITOR` PID
**The 4.25 behaviour:** the editor starts `CrashReportClientEditor`, which restarts itself outside the editor's process group (`WindowsPlatformCrashContext.cpp:493-536`). The kit never saw it. `UE4Editor.exe -game` does the same.

**The rule:**
- **A process whose command line names `-MONITOR=<PID of an owned editor or game>` is ours.** It is adopted by that exact PID match, never by name.
- **Record** its PID, parent, start time and command line. Also record whether its environment holds the canary variable by name, if that can be read without elevation; otherwise record "not readable".
- **It joins the leg's owned tree.** At a session-limit stop, stop the adopted monitor first and then the editor, so a deliberate stop is not reported as a crash.
- **After a normal exit,** it leaves on its own, as it did at 08:08 after logging `Editor/ExitCode:0`. If it is still there 60 s after the editor exits, stop it as owned and record it.
- **`%TEMP%\ue4-crc-pid-<pid>`** is engine-managed: recorded, never a STOP.
- **G2 is never a STOP by itself.**
- **Citations:** Codex cites how 4.25 passes `-MONITOR=`, and Code checks the citation.

### FH6. G3: `-unattended` is allowed on the runtime legs (amends EX4 for this kit only)
**Why:**
- 4.25's `-ExecutePythonScript` closes the editor on the next tick (Codex's 094-27 finding), so the runtime legs use the `PY` console entry instead;
- that entry never sets `GIsRunningUnattendedScript` (`EditorPythonExecuter.cpp:145`), so without `-unattended` a single modal dialog could hang a leg until its limit;
- EX4's analysis still holds: the only 4.25 prompts that default to Yes are import or overwrite prompts, and runtime legs never import or save;
- EU4's `bSendUnattendedBugReports=False` stays on every argv, so unattended crash sends stay off.

**Then:**
- the design note cites these reasons, and Code checks them;
- the authoring kit keeps EX4 as ruled.

### FH7. O4: FD3's memory lines apply to every editor and game leg
- **The lines:**
  - free commit of 8 GiB to start a leg;
  - physical memory of 6 GiB at the start;
  - a stop when free commit falls below 4 GiB in session.
- **Why the change:** FB3's "below 6 GiB" was my drafting error. FB3 was written before FD3 and never updated.

### FH8. Small items
- **O2:** the fixed budget of 6 events per A7 leg is accepted. A leg with fewer than 3 valid events fails and the run goes on (FH3). A later run repeats only that leg.
- **O3:** the `-game` legs' opt-outs are proven when those legs run.
  - Their logs get the same upload check as the editor's. Cite EngineAnalytics skipping `-game` at `:108`.
  - An upload sign is a STOP.
- **O5:** if the `CalledProcessError` at monitor close is a race with the editor's exit, fix the race. An unreadable reading inside an event window still invalidates that event.
- **O6:** every A7 picture records the effective `r.PostProcessAAQuality`. A picture where it is not 0 does not count.
- **O7 and O8:** noted.

### FH9. 094-27b, now: fix, re-arm for 09427b, then run when the PC is idle, all in one job
**In order:**
1. **Entry:** the EA8 entry proof against v2, ED1 space, the ED3 toolchains, and 094-27's results.
2. **Changes, with tests and can-fails:** FH2–FH8, plus the engine's Python 2.7.14 harness for every changed in-editor or in-game script. The new can-fails:
   - the readers pass the real files and fail their mutations;
   - a stand-in that restarts itself with `-MONITOR=<pid>` outside the tree is adopted, and is stopped at the limit;
   - a failing leg is followed by a passing one, and the second still runs;
   - an ensure folder after a normal exit fails its leg, while an abnormal exit ends the run;
   - FD3's lines.
3. **Rehearsal:** 094-27's five cases plus the three new ones, with stand-ins copied from real files (FH2c).
4. **Re-arm for job 09427b:**
   - fresh single-use slots for all 18 legs. That includes the smoke, because the monitor changes are new in-session code;
   - 09427 and older armings are refused;
   - dry runs of every leg on throwaway slots;
   - one live, observe-only gate reading.

   D2 applies: if any of steps 2–4 fails twice, stop with NEEDS-DECISION before any real launch.
5. **The real run, under FA5's idle rule:**
   - **the window:** from the end of step 4 until 06:00 IST on 10-31;
   - **the order:** smoke, then A3 load, A5 (PIE, then `-game`), A6 GC, A7, and A8.
6. **Evidence and contact sheets** per FB6, then a lean exit (FA6; this is a milestone).

## For the owner
- **Nothing to do.**
- The fix starts now, and it doesn't need Unreal.
- The real tests run whenever your PC has been idle for 10 minutes, today or tonight.
