094-31 — C1: plugin FRAMES on 4.25. Implement the Phase C plan's C1 file list, build the Paris host's two 4.25 targets, and pass the C1 gate in two real sessions. Under chat's ruling 094-30 (FL1–FL6) and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-10-31.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

THE OWNER'S DECISIONS
- Capture is the priority (10-31).
- Lean profile (FJ1): no checks or tests beyond what the port needs.
- Label sync is the most important part of the plugin; it is C2's job, next.

BINDING
1. _reviews\094-30-ruling.md (FL1–FL6). It is new.
   - FL1: your plan, docs/europa/PHASEC-capture-port-plan.md (4f46a9c), is the route.
   - FL3: the readback adapter drops and counts a 0×0 map, or one whose size or pitch doesn't match the request, and never writes it. Each session records its RHI and the dropped count.
   - FL5: renumber the new gotcha G488, in your first commit.
   - FL6: this job.
   - FL2 and FL4 bind C2 and C3 later. Note them, don't build them.
2. _reviews\094-27c-ruling.md (FJ1: the lean profile) and _reviews\094-27d-ruling.md (FK1: physical memory 4 GiB at the start). Also the rulings FJ1 keeps by name: FI2–FI4, FH4–FH6, FE1, FD3 as FK1 amends it, FF2/FG2, EU4/FC4, EY5 and EZ1.
3. Your own A3–A5 build practice for the 4.25 targets: the toolchain admission and the build memory gate. FJ1 drops the heavy proofs.

STARTING POINT
- feat/europa-port at 4f46a9c. Phase A is done: the four smoke anomalies are counted on 4.25.
- On 4.25, ANOMALY_CAPTURE, CONTROL_SERVER and SHADERS are all 0.
- The Paris host's binaries are bound in the runtime kit's 27-file manifest, built at 40a8eaf (094-21).

ORDER
1. Entry: FJ1's fast checks.
2. Implement C1:
   - the plan's C1 file list, with capture compiled in on 4.25 for non-Shipping builds; SHADERS and CONTROL_SERVER stay 0;
   - FL3's adapter rule;
   - the 5.1 paths stay intact behind their guards;
   - the keyword-Rotator check, and the engine's Python 2.7 harness for the new capture_425_driver/reader scripts;
   - FL5's renumbering.
3. Build the Paris host's Editor Development and Game Development targets.
   - Start UnrealBuildTool at below-normal process priority, and keep the build memory gate.
   - Report the first build's compile errors, counted per module. That is the real measurement the plan didn't have.
   - Fix and rebuild. If the same error survives two fix attempts, stop that stage and report it.
   - This job is authorized to replace the host's plugin and host binaries. Then re-bind the runtime kit's manifest to the new hashes.
4. The negative test: a padded, undersized or 0×0 readback is refused (FL3).
5. The C1 gate, in two real sessions under FJ1's gate:
   a. Editor PIE on EA_Core;
   b. standalone -game on EA_Travel.
   Each requests 60 plugin PNGs at native 1280×720 within 90 s. Each must show:
   - the exact count, with no missing or duplicate index;
   - no black frame and no unknown format;
   - the correct pixels in the check region;
   - the RHI and the dropped count recorded;
   - a normal exit.
   If a session fails, diagnose it, fix it if the fix is quick and testable, and use one more fresh slot for that session.
6. Exit: FJ1's fast checks; commit, with W4. No push.
The window for real sessions: until 07:00 IST on 11-01.

STOP RULES
- Only the armed kit launches the editor or the game. The build (UnrealBuildTool and its compiler children) is authorized for step 3 only.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. The binaries may change in step 3, and only there.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags.
- No security, paging, power or machine setting changes. No elevation. Delete nothing except by receipt.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report (report.md):
- a summary;
- the change list;
- the first-build error counts per module, and the fixes;
- the final build results;
- the negative test;
- both sessions' gate results (count, indices, black and format, check region, RHI, dropped, exit);
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
GREEN means both sessions pass the C1 gate.
