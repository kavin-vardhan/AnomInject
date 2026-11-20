094-26 — RUN the armed A6 fixture-authoring kit for real, starting now and whenever the PC is idle (EA_Core, then EA_Travel; at most two editor sessions), under chat's rulings 094-25 (FA1–FA7), 094-24 EZ5 and 094-23 EY7. Then prove the result. Requested by chat Claude (the orchestrator) on 2026-10-29.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-25-ruling.md (FA1–FA7). It is new. FA3 and FA5 are this run; FA6 sets the lean checks.
2. _reviews\094-24-ruling.md (EZ1–EZ6): EZ1's attributed root rule, EZ2's admission wait, and EZ5, this run.
3. _reviews\094-23-ruling.md (EY1–EY8):
   - EY7: this run's window, gate, attempts, evidence, stops and verdict meanings;
   - EY1–EY4: the kit as fixed in 094-24 and 094-25, and re-armed in 094-25;
   - EY5: the owner's scope decision. The port is 4.25 only; nothing in this job touches 5.1.
4. _reviews\094-22-ruling.md (EX1–EX6) as amended by EY1–EY4 and EZ1–EZ2, _reviews\094-21-ruling.md (EW1–EW4), _reviews\094-20-ruling.md (EV1–EV7), _reviews\094-19-ruling.md (EU1–EU6), _reviews\094-18-ruling.md (ET1–ET7), _reviews\094-17-ruling.md (ER1–ER4), _reviews\094-16-ruling.md (EQ1–EQ5), _reviews\094-14-ruling.md (EO1–EO6), _reviews\094-13-ruling.md (EN1–EN5), _reviews\094-12-ruling.md (EM2) and _reviews\094-12-request.md (STOP RULES, DELIVERABLE), with everything they bind.
5. The armed 09425 kit, its design notes (09422-authoring-design.md, as amended by your 09423, 09424 and 09425 amendments) and your A5 fixture recipes and manifests, as committed.

STARTING POINT
- feat/europa-port = 83d614f. The kit binds a8d3425 (.bat 4c5ad14e…, .json 01b2ed6a…).
- Before launching you, Claude Code removed its own leftover test junction under D:\IA_Europa\Temp\CodeReview09425 (FA1), by receipt, and its own dry run of the real entry point passed. The ledger refuses reparse points under D:\IA_Europa; create none.
- The kit is armed for 09425: attempts D:\IA_Europa\Receipts\09425\fixture-attempts\EA_Core\01–03 and \EA_Travel\01–03, TEMP mirrored under D:\IA_Europa\Temp\09425\fixture-attempts, state under Receipts\09425\fixture-state\global, \EA_Core and \EA_Travel. All 15 real paths are absent. The 09423 and 09422 armings refuse.
- Bound manifests (EW1): 4.25 27 files (c501895c…); Baseline51 39 (b05e6f8c…); ReGate51 44 (3865fe0c…). The two 5.1 manifests are frozen records (EY5).
- The 31124 marker stays in place (ET2). The 09417 owner kit stays spent.
- The owner asked for speed (FA5). His standing OK covers unattended editor runs whenever the PC is free; the kit's gate and your idle pre-check decide when.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
First, the owner's quiet slot: the owner needs the PC undisturbed from 10:00 to 10:30 IST on 10-29. If you start before 10:30 IST, wait until then: nothing heavy (the EA8 entry proof included), no entry-point start and no session before 10:30. Then go on.
1. Entry: the EA8 entry proof against v2; ED1 free space (EV3's D: lines, EY4's E: lines); the ED3 toolchains; the launcher's state (EU3). A foreign drive-root entry is recorded under EZ1, never opened.
2. The window (FA5): from your start until 06:00 IST on 10-30. Before each start of the entry point, wait with the kit's observe-only collector until the PC has been idle for 10 minutes and the memory, GPU and process conditions hold; then start it. Poll lightly while you wait.
3. The run: start D:\IA_Europa\Launch\Europa425-FixtureAuthoring.bat in its real (armed) mode, once. It runs EA_Core, then EA_Travel.
   - Don't interfere with it while it runs. Record its console verbatim.
   - The kit's measured gate decides when each session starts.
   - Re-run only if it ended with exactly "Stopped: no measured idle window within four hours. No editor was started." Then, after the idle pre-check again, you may start it on the next prelaunch slot, inside the window, and never beyond the 3 armed slots per recipe.
   - Any other "Stopped:" line ends the run: no retry, no fix. Collect and report.
4. Evidence, per recipe:
   a. the clean exit: the probe's skipped cleanup row and exactly one probe_stop; no PIE; the editor's exit code and how it ended; every error and ensure in Runtime.log, classified as engine or host content, or our plugin or probe;
   b. the outputs: exactly the declared packages are new, with size and sha256; nothing else under Content is new or changed (no BuiltData, no extra package); the 1,458 existing Content files, the config files, the descriptors and the .uproject unchanged;
   c. the authored content: the authoring receipt against the committed A5 recipe and manifest, item by item;
   d. no uploads: the EU4 switch in every recorded argv; the crash-client and session logs show no upload; list any crash or ensure report folder;
   e. the EditorRuns markers per ET2, with 31124 unchanged;
   f. the C:, E: and 5.1-engine ledger classes (EA4, EG1, ET3);
   g. a failed session: EX3's quarantine receipt.
5. Exit (FA6; this is A6's milestone):
   - the canary scan of new and changed files from the launch second (no full EK1 scan);
   - the EU1 command audit;
   - the EA8 exit proof against v2;
   - the ledger summary;
   - commit the receipts index and docs, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3, EY4, EZ1 and EZ2, and the kit's own rules (EW2, EW3, EX1–EX5, EY1–EY4, EZ1–EZ2). EN5's other hard STOPs are unchanged.
- A crash is a STOP: no further session; quarantine per EX3; report.
- Any change outside the three declared outputs is a STOP: Content, config, descriptors, .uproject, bound binaries or manifests.
- Only the kit launches the editor. The editor's own ShaderCompileWorker and crash-client children belong to its owned tree. Any other editor, game, commandlet, cook, UAT or ShaderCompileWorker launch is a STOP.
- No source change, build, re-arm or kit edit in this job. If the kit refuses for a tooling reason, stop and report.
- The work limit is FA5's window (to 06:00 IST on 10-30), not 9 hours. Report-only work after it is allowed.
- Foreign activity never ends the job (FA6): record it, wait it out, or both.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the timeline per recipe: gate wait, session start and end, duration, exit;
- the console, verbatim;
- evidence items a–g, with receipt paths;
- the outputs table: path, size, sha256;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
