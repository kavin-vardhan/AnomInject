094-31b — FINISH C1 (plugin frames on 4.25) from the committed C1 source: build both 4.25 targets under FM1's lighter incremental gate, then the negative test and the two C1 sessions. Under chat's rulings 094-31 (FM1–FM2) and 094-30 (FL1–FL6), and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-10-31.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-31-ruling.md (FM1–FM2). It is new.
   - FM1: UnrealBuildTool runs with -MaxParallelActions=2, at below-normal priority. It starts on one reading of at least 8 GiB of free commit. If free commit falls below 3 GiB during the build, stop the owned build tree by PID and retry when the start line holds again; at most 3 attempts per target. This replaces the 16 GiB build gate for this job.
   - FM2: this job.
2. _reviews\094-31-request.md: its ORDER steps 3–6, its STOP RULES and its DELIVERABLE, as amended by FM1. _reviews\094-30-ruling.md (FL1–FL6).
3. _reviews\094-27c-ruling.md (FJ1) and _reviews\094-27d-ruling.md (FK1), with the rulings they keep by name.

STARTING POINT
- feat/europa-port carries 094-31's committed C1 source, its tests and the tightened arming check. No build has run since 40a8eaf.
- 094-31 was ended by the owner restarting the watcher, after waiting about 7.5 h on an unpassable 16 GiB gate. Its receipts are under the run folder and D:\IA_Europa\Receipts. No real session or build ran.

ORDER
1. Entry:
   - If a 094-31 Codex or kit process is still alive, stop it by its recorded PID, never by name, and record it.
   - FJ1's fast checks.
   - Confirm that the C1 commit is the tip, or name the tip.
2. Build the Paris host's Editor Development and Game Development targets under FM1.
   - Report the first build's compile errors, counted per module.
   - Fix and rebuild. If the same error survives two fix attempts, report it.
   - Then re-bind the runtime kit's manifest to the new hashes.
3. The negative test (FL3): a padded, undersized or 0×0 readback is refused.
4. The two C1 sessions under FJ1's gate, as 094-31-request.md ORDER 5 defines them:
   - Editor PIE on EA_Core and standalone -game on EA_Travel;
   - each requests 60 plugin PNGs at native 1280×720 within 90 s;
   - exact count, no missing or duplicate index;
   - no black frame, no unknown format;
   - correct check region;
   - the RHI and the dropped count;
   - a normal exit.
5. Exit: FJ1's fast checks; commit, with W4. No push.
The window: until 07:00 IST on 11-01.

DELIVERABLE
As 094-31-request.md, plus each build attempt's start reading and any in-build stop.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
GREEN means both C1 sessions pass.
