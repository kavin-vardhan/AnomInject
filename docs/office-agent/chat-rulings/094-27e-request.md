094-27e — RUN the three remaining Phase A sessions under the lean profile: smoke, blinking positive, lod_corruption positive, with 094-27d's fixes. Under chat's ruling 094-27d (FK1–FK4) and 094-27c (FJ1–FJ4). Requested by chat Claude (the orchestrator) on 2026-10-31.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

THE OWNER'S DIRECTIVE STANDS: fast, with no checks or tests beyond FJ1's lean profile.

BINDING
1. _reviews\094-27d-ruling.md (FK1–FK4). It is new.
   - FK1: physical memory at least 4 GiB to start. The commit lines stay: 8 GiB to start, and a stop below 4 GiB in session.
   - FK2: 2 idle minutes before a start.
   - FK3: this job.
   - FK4: the probe's write self-disable is deferred.
2. _reviews\094-27c-ruling.md (FJ1–FJ4): the lean profile and Phase A's exit (FJ2). The named rulings it keeps still apply: FI2–FI4, FH4–FH6, FE1, FD3 (as FK1 amends it), FF2/FG2, EU4/FC4, FB1/FB6, EY5 and EZ1.
3. 094-27d's results: _reviews\094-27d-codex-report.md and _reviews\094-27d-code-review.md.

STARTING POINT
- feat/europa-port at cda5cdd (fixes 9360c18).
- The 09427d kit is spent; its three armed slots never ran.
- Counted so far: lighting_mismatch 5 (094-27c) and missing_object 6 (094-27d re-read).
- lod_corruption: forced model 1 (the sphere) in place of 2, untried.
- blinking: the probe file is read once after exit, untried.

ORDER
1. Entry: FJ1's fast checks.
2. Re-arm for 09427e: fresh single-use slots for smoke, blinking positive and lod_corruption positive. 09427d and older armings are refused. One dry run of the three slots.
3. Run them in that order under FJ1's gate, with FK1 and FK2.
4. Contact sheets and one index for all four anomalies (lighting and missing_object as counted), each counted event with before, during and after, and its AA echo.
5. Exit: FJ1's fast checks; commit, with W4.
If blinking or LOD still fails, diagnose it from the session's pictures and rows, and report the cause with the evidence. Fix it in this job only if the fix is in tooling or the driver and can be tested quickly; then use one more fresh slot for that anomaly.
The window: until 07:00 IST on 11-01.

STOP RULES
- Only the armed kit launches the editor. Its own ShaderCompileWorker, UnrealBuildTool and crash-monitor processes belong to its owned tree (FH5). Any other launch is a STOP.
- Any change to the Paris host's Content, Config, a descriptor, the .uproject or a bound binary is a STOP.
- An upload sign is a STOP.
- A second crash ends the run (FI4).
- m51, master, release/2026-10-05 and the tags stay untouched. No push. No tags.
- No security, paging, power or machine setting changes. No elevation. Delete nothing except by receipt.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report (report.md):
- a summary;
- per anomaly: attempted, applied, reverted, counted, and the reader verdict;
- the contact-sheet index with its paths;
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
GREEN means all four anomalies have at least 3 counted events (Phase A done).
