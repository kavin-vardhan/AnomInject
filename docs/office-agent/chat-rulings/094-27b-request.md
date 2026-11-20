094-27b — FIX the runtime kit (FH2–FH8), RE-ARM it for 09427b, then RUN it for real when the PC is idle: A6's runtime legs, A7 (the four smoke anomalies, with pictures) and A8's non-package part. One job, under chat's ruling 094-27 (FH1–FH9). Requested by chat Claude (the orchestrator) on 2026-10-30.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-27-ruling.md (FH1–FH9). It is new.
   - FH1: the 09427 kit is spent. Its smoke files are kept as real test files.
   - FH2: the cleanup reader reads world_cleanup with actor_snapshot="skipped" (EuropaPhaseAWorldProbe.cpp:211-225). The real-file rule, standing from now on:
     (a) every row kind and field a reader requires is cited to the probe line that emits it;
     (b) every reader runs against every kept real probe file: 094-26c's EA_Core session, 094-26d's two sessions and 094-27's smoke. It passes on them and fails their mutations (a row missing, duplicated or out of order; an unsafe stop);
     (c) stand-ins are mutated copies of real files wherever one exists, never written from the reader's own expectation.
   - FH3: only these end the run early:
     - a crash of our editor or game (it ends abnormally, not through its normal exit path);
     - a protected change;
     - an upload sign or a canary hit;
     - a smoke session whose editor fails to start, exits non-zero or lacks a binding;
     - the end of the window.
     Everything else is recorded, its evidence is kept, and the run goes on to the next leg. A new crash or ensure report folder after a normal exit fails its leg (FG3 as amended).
   - FH4: each leg's raw evidence is hashed at session end. A later, reviewed reader fix may re-read it. A FAIL is never re-labelled by hand.
   - FH5: adopt the crash monitor by an exact -MONITOR=<owned editor or game PID> match, never by name.
     - Record its PID, parent, start time and command line, and the canary variable by name in its environment if readable without elevation.
     - An owned-tree stop reaches it first, then the editor.
     - It must leave within 60 s of a normal exit, or it is stopped as owned and recorded.
     - %TEMP%\ue4-crc-pid-<pid> is engine-managed.
     - It is never a STOP by itself. Cite how 4.25 passes -MONITOR=.
   - FH6: -unattended is allowed on the runtime legs. The design note cites why: the PY console entry never sets GIsRunningUnattendedScript (EditorPythonExecuter.cpp:145); runtime legs never import or save; EU4's bSendUnattendedBugReports=False stays on.
   - FH7: FD3's memory lines for every editor and game leg:
     - to start a leg: free commit 8 GiB and physical memory 6 GiB;
     - in session: a stop when free commit falls below 4 GiB.
   - FH8:
     - O2's fixed 6-event budget is accepted;
     - O3: the -game legs' logs get the same upload check, citing EngineAnalytics :108;
     - O5: fix the close-time CalledProcessError race;
     - O6: echo the effective r.PostProcessAAQuality for every A7 picture. A picture where it is not 0 does not count.
   - FH9: this job, its order and its window.
2. _reviews\094-26d-ruling.md (FG1–FG4): FG2 (the crash-client config ini files are engine-managed) applies, and FG3 applies as amended by FH3.
   _reviews\094-27-request.md (FB1–FB7): the scope, the kit's rules, and the evidence and pictures, as amended by FH3–FH8.
3. _reviews\094-26c-ruling.md (FF1–FF5): FF2's Saved\ class, and FF5's one full preflight per run.
   Also: _reviews\094-26b-ruling.md (FE1–FE5); _reviews\094-28-ruling.md (FD1–FD6); _reviews\094-26-ruling.md (FC1–FC9); _reviews\094-25-ruling.md (FA1–FA8).
4. _reviews\094-24-ruling.md (EZ1–EZ6); _reviews\094-23-ruling.md (EY1–EY8; EY5: 4.25 only); _reviews\094-22-ruling.md (EX1–EX6) as amended, with EX4 amended by FH6 for the runtime kit only; _reviews\094-21-ruling.md (EW1–EW4); and the earlier rulings they bind, down to _reviews\094-12-request.md (STOP RULES, DELIVERABLE).
5. Your Phase A plan, _reviews\094-01-codex-europa-phase-a-plan.md: §E2's gates A3, A5, A6, A7 and A8 as FB1 scopes them; §E3; §H's A6 and A7 rows.
6. 094-27's results: _reviews\094-27-codex-report.md and _reviews\094-27-code-review.md. If either shows a protected-state change, a crash or an upload, stop at entry with NEEDS-DECISION.

STARTING POINT
- feat/europa-port is at 094-27's final commit (599ef70 or later).
- The 09427 kit is spent: its Smoke slot ran (exit 0, 31 of 31 bindings, reader FAIL on G1), and its 17 other slots never ran. This job supersedes it.
- EA_Core, EA_Travel and SM_EA_TwoLod exist as 094-26d authored them. Paris Content, Config, the descriptors and the .uproject are unchanged.
- The 4.25 manifest is unchanged: 27 files (c501895c…). The 31124 marker stays in place (ET2).
- The owner may use the PC in the day; the run waits for idle (FA5). The Epic Games Launcher is open.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space; the ED3 toolchains; 094-27's results (BINDING 6).
2. FH2–FH8, with tests and can-fails, and the engine's Python 2.7.14 harness for every changed in-editor or in-game script. Re-run the suites that cover the changed code. The new can-fails:
   - the readers pass the real files and fail their mutations;
   - a stand-in that restarts itself with -MONITOR=<pid> outside the owned tree is adopted, and is stopped at the limit;
   - a failing leg is followed by a passing one, and the second still runs;
   - an ensure folder after a normal exit fails its leg, while an abnormal exit ends the run;
   - FD3's lines.
   D2 applies: two attempts.
3. The rehearsal: 094-27's five cases plus the three new ones (the self-restarting -MONITOR stand-in; a failing leg followed by a passing one; an ensure folder versus an abnormal exit). Use stand-ins copied from real files (FH2c).
4. Re-arm for job 09427b:
   - fresh single-use slots for all 18 legs, the smoke included;
   - 09427 and older armings refused;
   - dry runs of every leg on throwaway slots;
   - one live, observe-only gate reading.
   If any of steps 2–4 fails twice, stop with NEEDS-DECISION before any real launch.
5. The real run (FH9), under FA5's idle rule: the PC idle for 10 minutes before each start, and the kit's gate decides each leg.
   - The window: from the end of step 4 until 06:00 IST on 10-31. A leg already started may finish.
   - The order: smoke; A3 load; A5 PIE, then -game; A6 GC; A7; A8.
6. Evidence per FB6:
   - each gate's reader verdict, with its receipts;
   - for A7, one contact sheet per anomaly, plus the index;
   - the per-picture AA echo (FH8).
7. Exit (FA6, lean; a milestone): the canary scan of new and changed files from the launch second; the EU1 command audit; the EA8 exit proof against v2; the ledger summary; commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3, EY4, EZ1, EZ2, FA6, FC5, FD2, FE2, FF2, FG2 and FH3. EN5's other hard STOPs are unchanged.
- Only the armed kit launches the editor or the game. Its own UnrealBuildTool (FD2), ShaderCompileWorker and crash-client processes belong to its owned tree, including the -MONITOR process adopted under FH5. Any other launch is a STOP.
- Any change to Content, Config, a descriptor, the .uproject, a bound binary or a manifest is a STOP.
- An upload of any kind is a STOP.
- A crash of our editor or game is a STOP for the remaining legs. Other leg failures don't end the run (FH3).
- Foreign activity never ends the job (FA6).
- The work limit is FH9's window. Report-only work after it is allowed.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the changes (FH2–FH8), with three tables:
  - the emitter table (FH2a): every reader requirement with the probe line that emits it;
  - the real-file test table (FH2b);
  - the can-fail table;
- the rehearsal, arming, dry-run and live-reading receipts;
- the real run's timeline, per leg: gate wait, start, end, duration, exit and reader verdict;
- each gate's reader verdict: A3, A5, A6, A7 and A8. For A7, per anomaly: attempted, valid, applied, reverted, refused and invalid;
- the contact-sheet index;
- the adopted crash-monitor table (FH5);
- the upload check for every session, the -game legs included, and the UBT argv list;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
