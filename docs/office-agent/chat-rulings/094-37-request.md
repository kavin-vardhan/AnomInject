094-37 — FINISH THE 4.25 CAPTURE PORT FOR THE OFFICE. In short:
- D: cleanup by receipt (step 0);
- build the pending fixes;
- 5.1's far plane and lost-batch timeout;
- a parity read of the four smoke anomalies;
- a one-line capture self-check;
- a short street-map smoke;
- then Code's pre-push scan and the office capture card.
Under chat's ruling 094-36 (FU1–FU6), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-01.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-36-ruling.md (FU1–FU6). It is new. In short:

   FU1 — The capture path is accepted for the office smoke:
   - census parity;
   - no tagging interference;
   - the exact restore;
   - the player view;
   - the schema.

   FU2 — Real-scene label-free grading is off the port's path. D1–D5 are FUTURE dataset-quality items; don't work on them here.

   FU3 — Finish the port:
   - build 05c55e2 (G500) and 2b9dc19 (coverage_pct);
   - write 5.1's 1,000,000 for an infinite far plane in annotation.json;
   - port 5.1's lost-batch timeout (LostAfterTicks 8) and its no-pass and polluted discard;
   - keep the event-adopts-census-value deviation, and log it in gotchas;
   - parity read, citing 5.1, of each smoke anomaly's apply and revert path:
     - blinking's hide and reveal calls;
     - lighting's change and any ramp;
     - LOD's chosen model;
     - missing_object's hide.
     Fix any 4.25 difference. Otherwise state that it matches.
   - a one-line capture self-check, logged at capture start and again after 300 frames, reporting:
     - the viewport class and size;
     - the backbuffer format;
     - the RHI;
     - PostRenderViewFamily fired (with a count), and whether that was before the UI;
     - the tick mode;
     - the census state, and any lost readback;
   - a fresh build budget: FM1's 3 attempts per target.

   FU4 — D: cleanup, owner-approved, as step 0:
   - Scope: D:\IA_Europa\Receipts\09403 … 09433c and CodeReview09403 … CodeReview09433c.
   - Remove:
     - picture and frame data;
     - ledger databases;
     - logs and archives;
     - any other file of 1 MiB or more.
   - Keep:
     - P9;
     - 09434 onward, with their CodeReview folders;
     - every path the current kits, tools or tests reference (search the kit scripts, tools/europa, the launch kits and the relay run folder first);
     - FH2's real probe files;
     - 09427e's contact sheets;
     - 09427b's crash analysis;
     - .json, .md, .txt and .log files under 1 MiB.
   - Receipt: D:\IA_Europa\Receipts\_cleanup-094-37\removed.tsv (path, size, modified time), plus a summary (space freed; D: free before and after).
   - Touch nothing outside this scope.

   FU5 — Smoke:
   - one street-map session: the player's view, seeded auto-injection 4242, the four smoke anomalies, 1280×720;
   - no label-free grading.
   - Gates:
     - each anomaly fires at least 3 times;
     - every affected frame has a mask, and clean frames have none (FQ3);
     - annotation.json: schema 2.1, sets agree with the rows, the far plane as in 5.1, a valid coverage_pct;
     - the census: more than 55 drawn actors at some pose, no exhaustion, no lost batch (with a focused test of the timeout);
     - custom depth restored exactly, and the game's own actors untouched;
     - the wall pose's strip unchanged with the census on;
     - the view equals the player's view;
     - the self-check line is present and sane;
     - no crash, no upload, the Paris host unchanged.
   - One retry slot. The window for real sessions ends at 07:00 IST on 11-02.

   FU6 — Code's part after the smoke: the pre-push scan and the office card. Not your task.

2. Kept rulings:
   - _reviews\094-35-ruling.md (FT2's parity, and the game's own custom-depth actors);
   - _reviews\094-34-ruling.md (FS2's changes);
   - _reviews\094-33b-ruling.md (FQ3);
   - _reviews\094-32-ruling.md (FO2);
   - _reviews\094-31-ruling.md (FM1);
   - _reviews\094-31b-ruling.md (FN2);
   - _reviews\094-27c-ruling.md (FJ1);
   - _reviews\094-27d-ruling.md (FK1);
   - the rulings these keep by name.

3. 094-36's results:
   - _reviews\094-36-codex-report.md;
   - _reviews\094-36-code-review.md (§1 lists the 5.1 gaps).

STARTING POINT
- feat/europa-port at df5dfdc.
- The built source is 9a785e0. 05c55e2 and 2b9dc19 are committed but unbuilt.
- The grain cannot be turned off on 4.25 (RemapPermutation forces it). Phase-matched references exist if a bench needs them.

ORDER
1. Entry: FJ1's fast checks.
2. FU4, the cleanup by receipt.
3. FU3's changes and parity read, citing 5.1. Test only the changed code (FJ1).
4. Rebuild under FM1 (3 attempts per target). Re-bind the manifest.
5. FU5's smoke session under FJ1's gate. Check every FU5 gate. If a gate fails and a quick, testable fix exists, use the retry slot.
6. Exit: FJ1's fast checks. Commit with W4. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in step 4, and for a retry's rebuild.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. The street map is loaded, never saved. Binaries change only in the builds.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags.
- No security, paging, power or machine-setting changes. No elevation.
- Delete nothing except FU4's scope, by its receipt.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report.md, covering:
- the summary;
- the cleanup: files removed, space freed, D: before and after, what was kept and why;
- the changes, with the 5.1 citations;
- the parity read per anomaly (matches, or what was fixed);
- the build;
- the smoke, against every FU5 gate, with per-anomaly event and mask counts;
- the self-check line, verbatim;
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means every FU5 gate passes.
