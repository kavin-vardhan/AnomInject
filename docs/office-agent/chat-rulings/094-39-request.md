094-39 — FINISH THE 4.25 CAPTURE PORT, TAKE 3: a ≥2-LOD guard for auto lod_corruption, a per-frame pre_ui proof, labelled constants, the firing schedule listed; rebuild with a fresh budget; the smoke (street map at 12 GiB, or EA_Core); then the push prep (FV6 redaction; Code's scan and office card). Under chat's ruling 094-38 (FW1–FW6) and 094-37 (FV5, FV6), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-02.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-38-ruling.md (FW1–FW6). It is new. In short:
   - FW2: keep the pool extension on this branch. No new keys. Log it in gotchas.
   - FW3: auto-fired lod_corruption picks only meshes with at least 2 LODs, using the same guard as lod_popping. Keep 5.1's coarsest-LOD apply. Log it as a deliberate addition.
   - FW4: the self-check proves pre_ui frame by frame. Record the frames on which the marker was drawn, and show that those exact captures lack it. Label tick= and tick_type as build constants. tick_calls stays as the measured read.
   - FW5: list the capture driver's firing schedule (0.8 s, 0.4 s, 1 concurrent) in the sweep document as a 4.25-only choice.
   - FW6, this job:
     - a fresh FM1 budget (3 attempts per target);
     - the smoke as FV5: the street map at 12 GiB free commit (in-session stop at 4 GiB);
     - fallback: EA_Core at 8 GiB if the street map isn't admitted within 45 minutes of the build finishing, or by 03:00 IST;
     - two slots;
     - gates: FV5's, plus FW3 (no single-LOD target) and FW4 (the per-frame pre_ui proof);
     - report each LOD event's visible change (information only);
     - the window for real sessions ends at 07:00 IST on 11-02.
2. _reviews\094-37-ruling.md:
   - FV5's gates (from FU5): each anomaly fires at least 3 times; every affected frame has a mask and clean frames none; annotation.json at schema 2.1 with the sets agreeing, 5.1's far plane and a valid coverage_pct; the census (on the street map: more than 55 drawn, no exhaustion, no lost batch); custom depth restored exactly, with the game's own actors untouched; the wall strip unchanged (street map only); the view equals the player's view; the self-check lines present, sane and measured; no crash, no upload, the Paris host unchanged.
   - FV6, only if the smoke passes:
     - redact the drive-root listing from the unpushed history since 4f46a9c. It is in docs/europa/09434-live-capture-receipts.json, unchanged in 22 commits from 47a6972 to the tip;
     - replace the names with counts and a hash;
     - keep a local-only backup ref;
     - prove that each rewritten commit's tree differs only in that file.
3. Kept: 094-36 (FU3), 094-35 (FT2), 094-33b (FQ3), 094-32 (FO2), 094-31 (FM1), 094-31b (FN2), 094-27c (FJ1), 094-27d (FK1), and the rulings these keep by name.
4. 094-38's results: _reviews\094-38-codex-report.md, _reviews\094-38-code-review.md.

STARTING POINT
- feat/europa-port at 8bdcb4b.
- The Game target is built at 4dbcf9b. The Editor target's last build failed at 5971792; its fix is in 4dbcf9b.

ORDER
1. Entry: FJ1's fast checks.
2. FW3, FW4 and FW5. Test only the changed code (FJ1).
3. Rebuild both targets under FM1, with a fresh budget. Re-bind the manifest.
4. The smoke (FW6), with the fallback if needed. Check every gate. If a gate fails and a quick, testable fix exists, use the retry slot.
5. If the smoke passes: FV6's redaction.
6. Exit: FJ1's fast checks. Commit with W4. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in step 3, and for a retry's rebuild.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. Maps are loaded, never saved. Binaries change only in the builds.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags.
- No security, paging, power or machine-setting changes. No elevation. Delete nothing except by receipt.
- FV6's rewrite is limited to feat/europa-port commits after 4f46a9c, with a backup ref.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report.md, covering:
- the summary;
- FW3, FW4 and FW5;
- the build;
- the smoke against every gate, with per-anomaly event and mask counts and each LOD event's visible change;
- the self-check lines, verbatim;
- the map used, and why;
- FV6 (the commits rewritten, the backup ref, the tree-diff proof) or why it was skipped;
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means every smoke gate passes and FV6 is done.
