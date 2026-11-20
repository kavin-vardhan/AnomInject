094-27c — FIX the A6 recipe's camera (keyword-only rotators, G410) and the runtime driver's failure path. QUARANTINE and RE-AUTHOR EA_Core and EA_Travel with the A6 kit. RE-ARM the runtime kit on the new packages, then RUN all 18 runtime legs when the PC is idle. One job, under chat's ruling 094-27b (FI1–FI6). Requested by chat Claude (the orchestrator) on 2026-10-30.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-27b-ruling.md (FI1–FI6). It is new.
   - FI1: the 09427b runtime kit is spent. FH2–FH8 and FH5's monitor adoption are proven in real sessions.
   - FI2: the camera fix and the re-authoring.
     - The recipe spawns the camera with unreal.Rotator(pitch=-12.0, yaw=0.0, roll=0.0). Before saving, it reads the rotation back and requires pitch -12, yaw 0, roll 0 within ±0.01°. If that fails, it saves nothing and requests exit once.
     - A standing rule: every unreal.Rotator(...) with arguments in any in-editor or in-game script passes pitch, yaw and roll by keyword. A static check enforces it in every kit's tests.
     - The Python 2.7 recording stub models Rotator(roll=0.0, pitch=0.0, yaw=0.0). The old recipe line must fail the camera-angle assertion.
     - Quarantine SM_EA_TwoLod.uasset, EA_Core.umap and EA_Travel.umap by receipt, then author both recipes fresh with the A6 kit armed for 09427c. No authoring smoke.
     - The authorized Content change is only those three declared outputs at their exact paths.
   - FI3: the driver's failure path is single-shot.
     - In order: record every actual and expected value; unregister every callback; request exit exactly once; act on nothing after that.
     - Establish the 094-27b Load crash cause from its log and the driver source, with line citations, and report it as confirmed or unconfirmed.
     - The camera check compares normalized angles within ±0.01°.
   - FI4: in the runtime kit, the first crash fails its leg and the run goes on. These end the run: a second crash; any crash in an authoring session (quarantine per EX3); an upload sign or canary hit; a protected change; a broken smoke.
   - FI5: files under C:\Windows\ that the OS holds locked join FE2's OS-protected class: recorded by name and size, never a coverage gap.
   - FI6: this job, its order and its window.
2. _reviews\094-27-ruling.md (FH1–FH9) as amended by FI4; _reviews\094-27-request.md (FB1–FB7) as amended by FH3–FH8 and FI4.
3. The A6 authoring kit's rulings, for step 4:
   - _reviews\094-26d-ruling.md (FG1–FG4);
   - _reviews\094-26c-ruling.md (FF1–FF5);
   - _reviews\094-26b-ruling.md (FE1–FE5; FE1's recipe stands);
   - _reviews\094-28-ruling.md (FD1–FD6);
   - _reviews\094-26-ruling.md (FC1–FC9);
   - _reviews\094-22-ruling.md (EX1–EX6; EX3's quarantine; EX4 as ruled for the authoring kit);
   - _reviews\094-21-ruling.md (EW1–EW4; EW2's scope).
4. _reviews\094-25-ruling.md (FA1–FA8); _reviews\094-24-ruling.md (EZ1–EZ6); _reviews\094-23-ruling.md (EY1–EY8; EY5: 4.25 only); and the earlier rulings they bind, down to _reviews\094-12-request.md (STOP RULES, DELIVERABLE).
5. Your Phase A plan, _reviews\094-01-codex-europa-phase-a-plan.md: §E2's gates as FB1 scopes them; §E3; §H's A6 and A7 rows. The A5 fixture manifest, whose camera is pitched -12.
6. 094-27b's results: its report, and _reviews\094-27b-codex-report.md and _reviews\094-27b-code-review.md when present. If either shows a protected-state change or an upload, stop at entry with NEEDS-DECISION.

STARTING POINT
- feat/europa-port at 094-27b's final commit (0a02987 or later), with a clean worktree. The 09427b kit is spent: Smoke PASS; Load FAIL (crash); 16 legs never ran.
- EA_Core and EA_Travel exist as 094-26d authored them, with their cameras rolled -12° (G410). SM_EA_TwoLod is unaffected, but EA_Core's recipe authors it, so it is re-authored too.
- Paris Content outside those three packages, Config, the descriptors and the .uproject are unchanged. The 4.25 manifest has 27 files (c501895c…). The 31124 marker stays in place (ET2).
- The owner may use the PC in the day; real sessions wait for idle (FA5). The Epic Games Launcher is open.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space; the ED3 toolchains; 094-27b's results (BINDING 6).
2. The fixes, now: FI2's recipe change, static check and stub; FI3's driver and camera check; FI4's crash counting; FI5's class.
   - Run the engine's Python 2.7.14 harness on every changed script, with the can-fails:
     - the old camera line fails;
     - a positional Rotator anywhere fails the static check;
     - a failing check gives exactly one exit request and no load after it;
     - a repeating callback that raises is unregistered on its first raise;
     - a first crash stand-in continues the run, and a second ends it.
   - Re-run the suites that cover changed code.
   - Qualify everything in the runtime kit that does not depend on the new packages.
   - D2 applies: two attempts.
3. The rehearsal: the runtime kit's eight cases plus FI3's and FI4's new cases, with stand-ins copied from real files (FH2c).
4. Authoring:
   - quarantine the three packages by receipt;
   - re-arm the A6 kit for 09427c, with fresh EA_Core and EA_Travel slots; 09426d and older armings are refused;
   - one live, observe-only gate reading;
   - under FA5's idle rule, author EA_Core, then EA_Travel. Evidence per EY7, with FF2's and FG2's classes.
5. Runtime:
   - after both maps are authored, bind the new package hashes and arm the runtime kit for 09427c: fresh single-use slots for all 18 legs, the smoke included; 09427b and older armings are refused;
   - dry runs of every leg on throwaway slots;
   - one live gate reading;
   - then the real run under FA5's idle rule, in FH9's order: smoke; A3 load; A5 PIE, then -game; A6 GC; A7; A8.
   If any of steps 2–5 fails twice, stop with NEEDS-DECISION before the next real launch.
6. Evidence per FB6:
   - each gate's reader verdict, with its receipts;
   - the Load leg's read-back of every saved value, the camera included;
   - for A7, one contact sheet per anomaly and the index, with the per-picture AA echo.
7. Exit (FA6, lean; a milestone): the canary scan of new and changed files from the first launch second; the EU1 command audit; the EA8 exit proof against v2; the ledger summary; commit, with W4.
The window for real sessions: until 06:00 IST on 10-31. A session already started may finish. Report-only work after it is allowed.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3, EY4, EZ1, EZ2, FA6, FC5, FD2, FE2, FF2, FG2, FH3, FI4 and FI5. EN5's other hard STOPs are unchanged.
- Only the two armed kits launch the editor or the game. Their own UnrealBuildTool (FD2), ShaderCompileWorker and crash-client processes belong to their owned trees, including the -MONITOR process adopted under FH5. Any other launch is a STOP.
- Content: only FI2's three declared outputs may change (the quarantine, then the fresh authoring). Any other change to Content, Config, a descriptor, the .uproject, a bound binary or a manifest is a STOP.
- An upload of any kind is a STOP.
- A crash in an authoring session is a STOP (quarantine per EX3). In the runtime kit, the second crash is a STOP (FI4).
- Foreign activity never ends the job (FA6).
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the changes (FI2–FI5): the static-check result, the stub, the can-fail table, and the crash-cause analysis with its citations (confirmed or unconfirmed);
- the rehearsal, both armings, the dry runs and the live readings;
- the quarantine receipts, and the authoring timeline per recipe (gate wait, start, end, duration, exit). Also the in-session camera read-back, and the new outputs table: path, size, sha256;
- the runtime timeline per leg: gate wait, start, end, duration, exit and reader verdict;
- each gate's reader verdict: A3 with the read-back values; A5; A6; A7 per anomaly, with attempted, valid, applied, reverted, refused and invalid; A8;
- the contact-sheet index;
- the adopted crash-monitor table, and any crash with its leg;
- the upload check for every session, the -game legs included, and the UBT argv list;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
