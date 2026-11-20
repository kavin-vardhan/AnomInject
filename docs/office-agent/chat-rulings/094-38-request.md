094-38 — FINISH THE 4.25 CAPTURE PORT, TAKE 2: the LOD parity fix and an auto-injector parity sweep, a self-check line that measures every field, 5.1's far plane, a smoke with a memory line that fits (with an EA_Core fallback), then the push prep (redact the drive-root listing; Code's scan and office card). Under chat's ruling 094-37 (FV1–FV6), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-01.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-37-ruling.md (FV1–FV6). It is new. In short:

   FV2 — LOD PARITY AND THE SWEEP
   - lod_corruption uses 5.1's default selection exactly: WorstLodSentinel, the coarsest LOD (Anomaly_LodCorruption.cpp:29-32, AnomalyLod.cpp:144-152).
     It replaces the 4.25-only LOD0 choice at AnomalyAutoInjectorSubsystem.cpp:630-637.
   - Diff the whole 4.25 auto-injection path against 5.1 and list every 4.25-only line.
     Each line is either justified as a 4.25 API difference, or removed.
   - In the smoke, report each LOD event's visible change. This is information only, not a gate.

   FV3 — THE SELF-CHECK MEASURES EVERY FIELD
   - viewport: the runtime class or type of the game viewport and its client;
   - tick mode: read from engine state (fixed time step, fixed frame rate, smoothing, the hook actually used);
   - pre_ui: a pixel test. Draw a small marker on the UI or canvas layer for a few frames, and report pre_ui=1 only if no captured frame contains it;
   - keep the format, RHI, size, hook count, census state, and the lost, no-pass and polluted counts;
   - emit the line at the first resolved frame and at frame 300.

   FV4 — Match 5.1's far plane exactly, the finite case included (cite).

   FV5 — THE SMOKE
   - Street map:
     - start at 12 GiB free commit and 6 GiB physical;
     - the in-session stop stays at 4 GiB;
     - two new slots (one, plus a retry).
   - Fallback: if the street map can't be admitted by 03:00 IST, run on EA_Core at the 8 GiB start line. That drops only the census-over-55 and wall-strip gates.
   - Gates: FU5's (094-36 ruling), plus:
     - FV3's measured fields;
     - 5.1's far plane;
     - a valid coverage_pct.
   - The window for real sessions ends at 07:00 IST on 11-02.

   FV6 — PUSH PREP (only if the smoke passes)
   - Redact the drive-root listing from the unpushed history:
     - in every commit since 4f46a9c that carries one (docs/europa/09434-live-capture-receipts.json at least; check the other receipts JSONs), replace the folder names with counts and a hash;
     - keep a local-only backup ref;
     - verify that each rewritten commit's tree differs only in the redacted files;
     - skip this step if the owner has said "leave it" (chat will add a line to the brief).
   - Then Code writes the pre-push scan and the office card. That is not your task.

2. Kept:
   - _reviews\094-36-ruling.md (FU3's fixes, FU5's gates);
   - _reviews\094-35-ruling.md (FT2);
   - _reviews\094-33b-ruling.md (FQ3);
   - _reviews\094-32-ruling.md (FO2);
   - _reviews\094-31-ruling.md (FM1, with a fresh budget of 3 attempts per target);
   - _reviews\094-31b-ruling.md (FN2);
   - _reviews\094-27c-ruling.md (FJ1);
   - _reviews\094-27d-ruling.md (FK1);
   - the rulings these keep by name.

3. 094-37's results:
   - _reviews\094-37-codex-report.md;
   - _reviews\094-37-code-review.md (§2 has the LOD finding and the self-check's fixed-text fields).

STARTING POINT
- feat/europa-port at 07c853e. The built source is cedb505 (Source is identical at 07c853e).
- The census timeout and discard, coverage_pct, G500 and the far plane (infinite case) are built.
- The kit change 0284a0d moved the gate wait to after setup.

ORDER
1. Entry: FJ1's fast checks.
2. FV2's fix and sweep, FV3 and FV4, each citing 5.1. Test only the changed code (FJ1).
3. Rebuild under FM1. Re-bind the manifest.
4. The FV5 smoke, with the fallback if needed. Check every gate. If a gate fails and a quick, testable fix exists, use the retry slot.
5. If the smoke passes: FV6's redaction, then the exit checks.
6. Exit: FJ1's fast checks. Commit with W4. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in step 3 and for a retry's rebuild.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. Maps are loaded, never saved. Binaries change only in the builds.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags.
- No security, paging, power or machine-setting changes. No elevation. Delete nothing except by receipt.
- FV6's history rewrite is limited to feat/europa-port commits after 4f46a9c, with a backup ref.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report.md, covering:
- the summary;
- the LOD fix and the sweep's list (each line justified or removed);
- FV3's fields and how each is measured;
- the far plane;
- the build;
- the smoke, against every gate, with per-anomaly event and mask counts and each LOD event's visible change;
- the self-check lines, verbatim;
- the map used, and why;
- FV6's redaction (the commits rewritten, the backup ref, and the tree-diff proof), or why it was skipped;
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means every smoke gate passes and FV6 is done or explicitly skipped.
