094-27d — FINISH A7, lean: diagnose 094-27c's failed picture legs from the kept evidence, fix them, re-read or re-run only what still fails, and give the owner contact sheets for all four smoke anomalies. One job, under chat's ruling 094-27c (FJ1–FJ4: the lean profile). Requested by chat Claude (the orchestrator) on 2026-10-30.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

THE OWNER'S DIRECTIVE (10-30, 22:20 IST): get the port done fast; no long checks or tests that aren't directly relevant to the porting. FJ1 turns that into rules. Where any older ruling asks for more checking than FJ1, FJ1 wins.

BINDING
1. _reviews\094-27c-ruling.md (FJ1–FJ4). It is new.
   - FJ1, the lean profile:
     - Dropped until the D-lite push: the full EA8 proofs, the canary and EK1 scans, the EU1 audit, and charged-growth accounting.
     - Dropped unless the launcher or kit core changes: rehearsals, dry-run sweeps, live observe-only readings, and the smoke.
     - Tests cover only the changed code: its unit tests, the Python 2.7 harness for changed in-editor scripts, the keyword-Rotator check, and one negative test per fix.
     - The gate takes one reading. It waits only on FD3's memory lines, D: at 25 GiB to start (15 GiB floor), and 5 idle minutes. GPU, input and coverage are recorded, but they never block a start or invalidate an event.
     - A picture event counts when its before, during and after screenshots exist and pass the picture check.
     - Deferred out of Phase A: A5, A6 GC, A8, and all NoApply and SkipRestore controls.
     - What stays: m51 and the protected refs by read-only git; the Paris Content, Config, descriptors and .uproject hashed before and after each session; the upload switches and the log check; one session per leg with its limit and owned-tree stop (FH5); FI4.
   - FJ2: Phase A closes when each of A7's four anomalies has at least 3 events that visibly apply and revert, by the picture check or by the owner's eye check.
   - FJ3: this job, its order and its window.
   - FJ4: remove the three comment lines in tools/europa; every driver step runs once behind a guard, and the harness models a multi-tick load.
2. The rulings that still bind, by name only:
   - FI2: keyword-only rotators;
   - FI3: the single-shot driver;
   - FI4: the crash rule;
   - FH4: re-reading kept evidence;
   - FH5: adopting the crash monitor;
   - FH6: -unattended on runtime legs;
   - FE1: anti-aliasing off at capture, through the command line;
   - FD3: the memory lines;
   - FF2 and FG2: the Saved\ classes;
   - EU4 and FC4: the upload switches;
   - FB1's A7 scope and FB6's contact sheets;
   - EY5: 4.25 only;
   - EZ1: foreign drive-root entries are recorded, not acted on.
   Each is in _reviews\ under its ruling file. Read only these.
3. 094-27c's results: its report, and _reviews\094-27c-codex-report.md and _reviews\094-27c-code-review.md. If either shows a protected-state change or an upload, stop at entry with NEEDS-DECISION.

STARTING POINT
- feat/europa-port at 094-27c's final commit, with the 09427c runtime kit spent.
- EA_Core and EA_Travel were re-authored in 094-27c. Their cameras read back at pitch −12, yaw 0, roll 0.
- 094-27c's A7 results:
  - lighting_mismatch PASS, with pictures;
  - lod_corruption FAIL: 6 applies and 6 reverts, 0 events passing the picture check ("No visible active picture");
  - missing_object FAIL: 6 applies and 6 reverts, all failing the clean-frame noise check;
  - blinking FAIL: a controlled stop on a sharing violation reading the live probe file.
- 094-27c's raw evidence is hashed (FH4).
- The owner may use the PC in the day; real sessions wait for FJ1's gate.

ORDER
1. Entry: FJ1's fast checks; ED1 free space; 094-27c's results (BINDING 3).
2. Diagnose from the kept 094-27c pictures and probe rows, with no new session:
   a. lod_corruption: why no change is visible. Check the forced LOD value (1-based), LOD1's visual difference at this camera, and the picture region.
   b. missing_object (and blinking SkipRestore's noise): what changes between clean frames. Measure global brightness drift (auto exposure) against local change.
   c. blinking: the sharing violation on the live probe read.
   Report each cause, with the picture or row that shows it.
3. Fix, in this order of preference:
   - a checker or region that was wrong: fix it, then re-read the kept evidence (FH4). No new session for an anomaly whose re-read passes.
   - the live probe read retries on a sharing violation.
   - brightness drift: lock exposure for picture sessions with a command-line override proven from 4.25 source. No config edit (as FE1).
   - no visible LOD change in the pictures: fix the anomaly's parameters or target in the driver. No re-authoring.
   Remove the three comment lines (FJ4). Test only the changed code (FJ1).
4. Re-arm for job 09427d only the A7 positive legs that still fail, with fresh single-use slots, and run them under FJ1's gate. If the launcher or kit core changed, add the smoke.
5. Contact sheets for all four anomalies, with one index for the owner: lighting_mismatch's from 094-27c, plus the new or re-read ones. Each counted event shows before, during and after, with its AA echo.
6. Exit: FJ1's fast checks; commit, with W4.
The window for real sessions: until 07:00 IST on 10-31. If a failure can't be explained or fixed without re-authoring, report it with its pictures as NEEDS-DECISION and still finish the others.

STOP RULES
- Only the armed kit launches the editor or the game. Its own ShaderCompileWorker, UnrealBuildTool and crash-monitor processes belong to its owned tree (FH5). Any other launch is a STOP.
- Any change to the Paris host's Content, Config, a descriptor, the .uproject or a bound binary is a STOP.
- An upload sign in any session log is a STOP.
- A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05 and the tags stay untouched. No push. No tags.
- Never change a security, paging, power or machine setting. No elevation. Delete nothing except by receipt, under the job's own receipts root.
- Foreign activity never ends the job (FA6); record it (EZ1).

DELIVERABLE
A short report in your run folder (report.md):
- a summary first;
- the entry checks;
- the three diagnoses, with evidence;
- the fixes and their tests;
- re-reads and re-runs, per anomaly: attempted, applied, reverted, counted, and the reader verdict;
- the contact-sheet index;
- any crash, upload sign or protected change (or "none");
- the exit checks and the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
GREEN means all four anomalies have at least 3 counted events and the contact sheets exist.
