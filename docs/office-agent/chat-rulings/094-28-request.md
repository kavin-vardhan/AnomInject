094-28 — FIX the authoring launcher (Python 2.7) and the analytics uploads, RE-ARM for 09428, SMOKE-TEST in the real editor, then RUN the A6 fixture authoring for real (EA_Core, then EA_Travel) when the PC is idle — one job, under chat's ruling 094-26 (FC1–FC9). Requested by chat Claude (the orchestrator) on 2026-10-29.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-26-ruling.md (FC1–FC9). It is new.
   - FC2: the launcher fix, and the Python 2.7 test harness for every in-editor script.
   - FC3: one real-editor smoke session before any recipe slot.
   - FC4: the analytics opt-out, proven from 4.25 source.
   - FC5: engine- and editor-managed files are recorded, not bound.
   - FC6: the editor's own UnrealBuildTool child.
   - FC7: this job, its order and its window.
2. _reviews\094-25-ruling.md (FA1–FA8): FA5's idle rule, FA6's lean checks.
3. _reviews\094-24-ruling.md (EZ1–EZ6); _reviews\094-23-ruling.md (EY1–EY8, with EY7's evidence, stops and verdict meanings; EY5: 4.25 only); _reviews\094-22-ruling.md (EX1–EX6) as amended; _reviews\094-21-ruling.md (EW1–EW4); and the earlier rulings they bind, down to _reviews\094-12-request.md (STOP RULES, DELIVERABLE).
4. 094-26's evidence: _reviews\094-26-codex-report.md, _reviews\094-26-code-review.md, and the EA_Core slot-01 session under D:\IA_Europa\Receipts\09425\fixture-attempts\EA_Core\01.

STARTING POINT
- feat/europa-port = 97aec94 (docs only over 83d614f). The kit's tooling equals a8d3425.
- The 09425 arming: EA_Core is spent (slot 01, 0 partial outputs); EA_Travel was never started. This job supersedes the arming.
- The declared outputs are still absent. Paris Content, Config, the descriptors and the .uproject are unchanged.
- The 4.25 manifest is unchanged: 27 files (c501895c…). The 5.1 manifests are frozen records (EY5).
- The 31124 marker stays in place (ET2).
- The owner's PC: Windows restarted at 13:16 IST on 10-29; the Epic Games Launcher has been open since 18:08. The owner may use the PC this evening.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space (EV3's D: lines, EY4's E: lines); the ED3 toolchains.
2. The fixes, with self-tests and can-fails:
   - FC2: the launcher keeps the fixture module alive; the harness runs every in-editor script under E:\EpicGames\UE_4.25\Engine\Binaries\ThirdParty\Python\Win64\python.exe (2.7.14) with a recording stub of `unreal` (syntax, imports, the launcher, each recipe's full call sequence, nothing saved); a runpy-dict mutant must fail.
   - FC4: the analytics opt-out switches, each with its 4.25 source citation; an argv without them refuses. If no command-line opt-out exists, stop NEEDS-DECISION with the options.
   - FC5: the managed-file classes.
   - FC6: the UBT child in the owned tree, its argv recorded, and no build allowed.
   D2 applies: two attempts.
3. A rehearsal of the changed parts with stand-ins on owned scratch (the launcher path, the smoke-session path, a STOP on a build action by the UBT child).
4. Re-arm for job 09428: fresh slots for EA_Core and EA_Travel, plus one smoke session; EW2's scope otherwise; the 09425 and older armings refused. Then dry-run every session on throwaway slots, and take one live, observe-only gate reading.
5. The real run (FC7), under FA5's idle rule (the PC idle for 10 minutes before each start; the kit's gate decides each session). The window: from the end of step 4 until 06:30 IST on 10-30; a session already started may finish. The order:
   a. the smoke session (FC3). A failed smoke ends the run; no recipe slot is used;
   b. EA_Core;
   c. EA_Travel.
6. Evidence, per recipe, as 094-23 EY7 lists it (a–g), with FC5's classes; plus, for the smoke session, its receipt, its clean exit and its upload check.
7. Exit (FA6, lean; a milestone):
   - the canary scan of new and changed files from the launch second;
   - the EU1 command audit;
   - the EA8 exit proof against v2;
   - the ledger summary;
   - commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3, EY4, EZ1, EZ2, FA6 and FC5. EN5's other hard STOPs are unchanged.
- Only the armed kit launches the editor. Its own UnrealBuildTool (FC6), ShaderCompileWorker and crash-client children belong to its owned tree. Any other launch is a STOP.
- Any change to Content (outside the declared outputs), Config, a descriptor, the .uproject, a bound binary or a manifest is a STOP.
- An upload of any kind is a STOP (EY7, FC4).
- A crash is a STOP: no further session; quarantine per EX3; report.
- Foreign activity never ends the job (FA6).
- The work limit is FC7's window. Report-only work after it is allowed.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the fixes, their 4.25 and Python 2.7 citations, and the self-test table;
- the rehearsal, arming, dry-run and live-reading receipts;
- the real run's timeline: the smoke session, then each recipe (gate wait, start, end, duration, exit);
- the console, verbatim;
- the smoke receipt, and evidence items a–g per recipe, with receipt paths;
- the outputs table: path, size, sha256;
- the upload check for every session;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
