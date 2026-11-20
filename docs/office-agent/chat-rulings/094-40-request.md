094-40 — FINISH THE 4.25 CAPTURE PORT, TAKE 4. In short:
- the bench seeds the injector once per session;
- auto lod_corruption forces a LOD different from the one drawn;
- rebuild with a fresh budget;
- the EA_Core smoke;
- if GREEN, FV6's redaction.
Code then scans, pushes and writes the office card. Under chat's ruling 094-39 (FX1–FX6), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-02.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-39-ruling.md (FX1–FX6). It is new. In short:
   - FX2: the bench driver starts and seeds the auto-injector once per session, never once per hold.
   - FX3: auto-fired lod_corruption forces a LOD different from the one currently drawn.
     - Force the coarsest LOD; if the coarsest is already drawn, force LOD0.
     - Predict the drawn LOD on the game thread from screen size, as the engine does (cite the 4.25 source).
     - Keep FW3's 2-LOD guard.
     - Log it as a deliberate 4.25 addition.
   - FX4: the shipped selection floor stays at 6 % (5.1). The 0.5 % floor is for EA_Core bench runs only. Note which floor each session used.
   - FX6, the smoke:
     - EA_Core at the 8 GiB start line, with the in-session stop at 4 GiB; two slots.
     - Gates: FV5's EA_Core set (masks on affected frames and none on clean ones; annotation.json at schema 2.1 with the sets agreeing, 5.1's far plane and a valid coverage_pct; census not exhausted or lost; custom depth restored with the game's own actors untouched; the player's view; the self-check lines measured; no crash and no upload).
     - Plus: each anomaly fires at least 3 times, and every LOD event changes target pixels.
     - The window for real sessions ends at 07:00 IST on 11-02.
   - FX6, if GREEN: FV6's redaction of the drive-root listing.
     - docs/europa/09434-live-capture-receipts.json is now in 25 commits since 47a6972.
     - Replace the folder names with counts and a hash.
     - Keep a local-only backup ref.
     - Prove that each rewritten commit's tree differs only in that file.
   - Code does the scan, the push and the card afterwards. Not your task: you never push.
2. Kept: 094-38 (FW2–FW5), 094-37 (FV5, FV6), 094-36 (FU3), 094-35 (FT2), 094-33b (FQ3), 094-32 (FO2), 094-31 (FM1, with a fresh budget of 3 attempts per target), 094-31b (FN2), 094-27c (FJ1), 094-27d (FK1), and the rulings these keep by name.
3. 094-39's results: _reviews\094-39-codex-report.md, _reviews\094-39-code-review.md.

STARTING POINT
- feat/europa-port at 0621027.
- Both targets are built with FW3 and FW4.
- The bench driver restarts and re-seeds the injector per hold.

ORDER
1. Entry: FJ1's fast checks.
2. FX2 and FX3. Test only the changed code (FJ1).
3. Rebuild both targets under FM1, with a fresh budget. Re-bind the manifest.
4. The EA_Core smoke. Check every gate. If a gate fails and a quick, testable fix exists, use the retry slot.
5. If the smoke passes: FV6's redaction.
6. Exit: FJ1's fast checks. Commit with W4. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in step 3, and for a retry's rebuild.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. Maps are loaded, never saved. Binaries change only in the builds.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push by you. No tags.
- No security, paging, power or machine-setting changes. No elevation. Delete nothing except by receipt.
- FV6's rewrite is limited to feat/europa-port commits after 4f46a9c, with a backup ref.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report.md, covering:
- the summary;
- FX2 and FX3, with citations;
- the build;
- the smoke against every gate, with per-anomaly event and mask counts and each LOD event's changed pixels;
- the self-check lines, verbatim;
- the selection floor used;
- FV6 (the commits rewritten, the backup ref, the tree-diff proof);
- any crash, upload sign or protected change (or "none");
- the final commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>". GREEN means every gate passes and FV6 is done.
