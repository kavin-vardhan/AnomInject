094-26d — WIDEN the editor-managed class to the host's Saved\ folder (FF2), RE-ARM for 09426d, then RUN the A6 fixture authoring for real (EA_Core, then EA_Travel; no smoke, FF3) when the PC is idle — one job, under chat's ruling 094-26c (FF1–FF5). Requested by chat Claude (the orchestrator) on 2026-10-30.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-26c-ruling.md (FF1–FF5). It is new.
   - FF1: EA_Core's 094-26c packages stay in quarantine; both recipes are authored fresh.
   - FF2: everything the editor writes under the host's Saved\ is recorded, never a STOP, except any *.ini under Saved\Config\; the project's Config\, descriptors and .uproject stay protected.
   - FF3: no smoke session in this job.
   - FF4: this job, its order and its window.
2. _reviews\094-26b-ruling.md (FE1–FE5): FE1's recipe and FE2's canary class stand as built.
   _reviews\094-28-ruling.md (FD1–FD6): FD2 and FD3 stand; FD5's no-dry-run reasoning applies again.
   _reviews\094-26-ruling.md (FC1–FC9): FC2, FC4 and FC5 stand.
3. _reviews\094-25-ruling.md (FA1–FA8): FA5's idle rule, FA6's lean checks.
4. _reviews\094-24-ruling.md (EZ1–EZ6); _reviews\094-23-ruling.md (EY1–EY8, with EY7's evidence, stops and verdict meanings; EY5: 4.25 only); _reviews\094-22-ruling.md (EX1–EX6) as amended; _reviews\094-21-ruling.md (EW1–EW4); and the earlier rulings they bind, down to _reviews\094-12-request.md (STOP RULES, DELIVERABLE).
5. 094-26c's results: _reviews\094-26c-codex-report.md and _reviews\094-26c-code-review.md. If either shows a protected-state change, a crash or an upload, stop at entry with NEEDS-DECISION.

STARTING POINT
- feat/europa-port at 094-26c's final commit (aecff71 or later). The 09426c arming: the smoke passed 51/51; EA_Core is spent (it authored both packages, then the kit stopped on Saved\Config\WorldState\1202994220.json and quarantined them); EA_Travel was never started. This job supersedes the arming.
- The declared outputs are absent again (quarantined by receipt in 09426c's EA_Core attempt).
- The declared outputs are still absent. Paris Content, Config, the descriptors and the .uproject are unchanged.
- The 4.25 manifest is unchanged: 27 files (c501895c…). The 31124 marker stays in place (ET2).
- The Epic Games Launcher is open. The owner may use the PC in the day; the run waits for idle (FA5).
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space; the ED3 toolchains; 094-28's results (BINDING 5).
2. FF2, with self-tests and can-fails: a WorldState json, a Backup map, an Autosave and a log under Saved\ are recorded; an *.ini under Saved\Config\ and any file in the project's Config\ stop. Re-run the suites that cover the changed code. D2 applies: two attempts.
3. Re-arm for job 09426d: fresh slots for EA_Core and EA_Travel (no smoke, FF3); EW2's scope otherwise; 09426c and older armings refused. No dry runs. One live, observe-only gate reading.
4. The real run (FF4), under FA5's idle rule (the PC idle for 10 minutes before each start; the kit's gate decides each session). The window: until 06:30 IST on 10-31; a session already started may finish. The order:
   a. EA_Core;
   b. EA_Travel.
5. Evidence, per recipe, as 094-23 EY7 lists it (a–g), with FF2's classes; the UBT children's recorded argv.
6. Exit (FA6, lean; a milestone): the canary scan of new and changed files from the launch second; the EU1 command audit; the EA8 exit proof against v2; the ledger summary; commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3, EY4, EZ1, EZ2, FA6, FC5, FD2, FE2 and FF2. EN5's other hard STOPs are unchanged.
- Only the armed kit launches the editor. Its own UnrealBuildTool (FD2), ShaderCompileWorker and crash-client children belong to its owned tree. Any other launch is a STOP.
- Any change to Content (outside the declared outputs), Config, a descriptor, the .uproject, a bound binary or a manifest is a STOP.
- An upload of any kind is a STOP (EY7, FC4).
- A crash is a STOP: no further session; quarantine per EX3; report.
- Foreign activity never ends the job (FA6).
- The work limit is FF4's window. Report-only work after it is allowed.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the FF2 change, and the self-test table;
- the arming and live-reading receipts;
- the real run's timeline per recipe (gate wait, start, end, duration, exit);
- the console, verbatim;
- evidence items a–g per recipe, with receipt paths, and every Saved\ change recorded under FF2;
- the outputs table: path, size, sha256;
- the upload check for every session, and the UBT argv list;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
