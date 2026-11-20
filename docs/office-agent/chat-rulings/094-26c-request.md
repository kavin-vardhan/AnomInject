094-26c — FIX the fixture recipe's anti-aliasing item for 4.25 (FE1), RE-ARM for 09426c, then RUN the A6 fixture authoring for real (smoke session, EA_Core, EA_Travel) before morning — one job, under chat's ruling 094-26b (FE1–FE5). Requested by chat Claude (the orchestrator) on 2026-10-30.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-26b-ruling.md (FE1–FE5). It is new.
   - FE1: the recipe drops override_anti_aliasing_method and anti_aliasing_method on 4.25; re-bind the recipe and its A5 manifest; a dated prediction addendum; the Python 2.7 harness.
   - FE2: OS-protected Windows event logs are excluded from the canary scan as a class (count and folder recorded).
   - FE4: this job, its order and its window.
2. _reviews\094-28-ruling.md (FD1–FD6): FD2's UBT rule and FD3's memory lines stand as built (09e776b); FD5's no-dry-run reasoning applies again.
   _reviews\094-26-ruling.md (FC1–FC9): FC2–FC5 stand; FC3's smoke session runs first.
3. _reviews\094-25-ruling.md (FA1–FA8): FA5's idle rule, FA6's lean checks.
4. _reviews\094-24-ruling.md (EZ1–EZ6); _reviews\094-23-ruling.md (EY1–EY8, with EY7's evidence, stops and verdict meanings; EY5: 4.25 only); _reviews\094-22-ruling.md (EX1–EX6) as amended; _reviews\094-21-ruling.md (EW1–EW4); and the earlier rulings they bind, down to _reviews\094-12-request.md (STOP RULES, DELIVERABLE).
5. 094-26b's results: _reviews\094-26b-codex-report.md and _reviews\094-26b-code-review.md (the smoke receipt lists the 51 bindings that exist and the 2 that don't). If either shows a protected-state change, a crash or an upload, stop at entry with NEEDS-DECISION.

STARTING POINT
- feat/europa-port at 094-26b's final commit (the guard and memory fixes are in 09e776b). The 09426b arming: the smoke session is spent (it exited cleanly and failed on the 2 missing bindings); EA_Core and EA_Travel were never started. This job supersedes the arming.
- The declared outputs are still absent. Paris Content, Config, the descriptors and the .uproject are unchanged.
- The 4.25 manifest is unchanged: 27 files (c501895c…). The 31124 marker stays in place (ET2).
- The Epic Games Launcher is open. The owner is asleep; the PC should stay idle until the morning.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space; the ED3 toolchains; 094-28's results (BINDING 5).
2. FE1: remove the two anti-aliasing bindings from the recipe's post-process item, and nothing else; re-bind the recipe and its A5 manifest; commit a dated prediction addendum; run the Python 2.7 harness (FC2) and the suites that cover the changed files. The smoke check must now list every remaining binding as present (from 094-26b's receipt). D2 applies: two attempts.
3. FE2: exclude OS-protected Windows event logs from the canary scan as a class, recording the count and folder.
4. Re-arm for job 09426c: fresh slots for the smoke session, EA_Core and EA_Travel; EW2's scope otherwise; 09426b and older armings refused. No dry runs (FD5). One live, observe-only gate reading.
5. The real run (FE4), under FA5's idle rule (the PC idle for 10 minutes before each start; the kit's gate decides each session). The window: until 07:45 IST on 10-30; a session already started may finish. The order:
   a. the smoke session (FC3). A failed smoke ends the run; no recipe slot is used;
   b. EA_Core;
   c. EA_Travel.
6. Evidence, per recipe, as 094-23 EY7 lists it (a–g), with FC5's classes; for the smoke session, its receipt, its clean exit and its upload check; the UBT children's recorded argv.
7. Exit (FA6, lean; a milestone): the canary scan of new and changed files from the launch second; the EU1 command audit; the EA8 exit proof against v2; the ledger summary; commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3, EY4, EZ1, EZ2, FA6, FC5, FD2 and FE2. EN5's other hard STOPs are unchanged.
- Only the armed kit launches the editor. Its own UnrealBuildTool (FD2), ShaderCompileWorker and crash-client children belong to its owned tree. Any other launch is a STOP.
- Any change to Content (outside the declared outputs), Config, a descriptor, the .uproject, a bound binary or a manifest is a STOP.
- An upload of any kind is a STOP (EY7, FC4).
- A crash is a STOP: no further session; quarantine per EX3; report.
- Foreign activity never ends the job (FA6).
- The work limit is FE4's window. Report-only work after it is allowed.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the recipe change, its re-binding and the prediction addendum, and the self-test table;
- the arming and live-reading receipts;
- the real run's timeline: the smoke session, then each recipe (gate wait, start, end, duration, exit);
- the console, verbatim;
- the smoke receipt, and evidence items a–g per recipe, with receipt paths;
- the outputs table: path, size, sha256;
- the upload check for every session, and the UBT argv list;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
