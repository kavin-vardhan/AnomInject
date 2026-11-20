# REPORT for 094-22-europa-a6-arm-automated-authoring.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-22-europa-a6-arm-automated-authoring.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-28 04:32:23 | finished: 2026-10-28 08:09:38 | duration: 217 min | exit code: 0
context at last turn: 747403 tokens (75% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-22-europa-a6-arm-automated-authoring.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION — the kit is armed exactly to EW2 and both dry runs passed with 0 launches, but one blocking GAP (G4) means the real run could never start while the Epic launcher is running.

**2. Plain summary for the owner**
- Codex built the overnight kit that will create the two test levels (EA_Core, EA_Travel). Nothing was launched and nothing protected changed.
- I found that the kit treats the Epic launcher's own overlay programs as "another Unreal program", so it would wait 4 hours and give up. A small fix and a re-arm are needed before the real run.
- Chat also needs to decide whether the in-session safety check should count our own editor's memory and GPU use. As built, it will probably stop the session partway through.

**3. Start state (04:33–04:51 IST)**
- **Disk:** C: 20.38, D: 73.64, E: 33.45 GiB. Launch hold met: D: at or above 40 GiB on two polls a minute apart.
- **Memory:** 9.70 of 23.63 GiB physical free, 26.05 GiB commit free. No blender running.
- **Toolchains:** unchanged (VS 2022 17.12, VS 2017 BuildTools 15.9, MSVC 14.16/14.36/14.38/14.42, SDK 17763 and 22621). Both mailbox watchers running.
- **m51:** matches P9 (`53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries in order). Its 5 can-fail cases pass.
- **Protected state:** PASS — release baseline 8/8, junctions resolve, StackOBot Content identical, BankOverflow 0 lost, cache Binaries identical.
- **EK1 pre-scan:** 43,430 files, 0 secret values.
- **Recorded:**
  - feature branch at `7039df7`;
  - manifests `c501895c…` 27/27, `b05e6f8c…` 39/39, `3865fe0c…` 44/44;
  - the launcher still the old instance (pid 26116);
  - Paris Content hash list: 1,453 + 5 files, equal to 094-18's.
- **Before-snapshot vs 094-21's after-snapshot:** 55 of 61 listings equal.

**4. The Codex job**
- **Run:** `runs\2026-10-16-094-01-europa-phase-a-plan`, thread `01a14334`, resumed once.
- **Timing:** 04:53:35 – 07:29:49 IST, 9,374 s, exit 0. Ledger row `…-resume20` appears once.
- **Pauses:** 0. D: never went below 71.53 GiB.
- **Commits:** `6e46b9f`, `572f5c0`, `8abb79e` (the commit the kit binds), `1a9afc1` (docs). All local, W4 dates correct, no push, no tags.
- **Its verdict line:** `VERDICT: GREEN | EW2 fixture-authoring kit armed and dry-run; bindings, P9, scans and gates pass; zero Unreal launches`

**5. The design note**
- **Mechanism:** the ordinary `UE4Editor.exe` opens the Paris project with `-EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities` and `-ExecutePythonScript=<attempt>\script\author.py`. That generated script calls the committed `author_425_fixture.py` for exactly one recipe. The editor closes itself on the next tick, with no save prompt.
  - Checked in 4.25 source: `PluginManager.cpp:567–603`, `EditorPythonExecuter.cpp:139–222`, `EditorServer.cpp:5734`, `UnrealEdEngine.cpp:878–893`.
- **Outputs:** exactly `Content\EuropaPhaseA\SM_EA_TwoLod.uasset` and `EA_Core.umap` (EA_Core), and `EA_Travel.umap` (EA_Travel). Any existing destination is refused, EA_Core requires an empty folder, and all 1,458 existing Content files are re-hashed before and after.
- **No `.uproject` or config change:** `-nowrite` blocks config writes (`ConfigCacheIni.cpp:1476`), `-SkipCompile` and `-stdout` stop the rebuild prompt, and the config files are compared before and after.
- **`-unattended`:** omitted, under EW3's fallback. Without it there is still no upload, but source doesn't show that every prompt has a safe default: `MessageDialog.cpp:84–89` defaults one prompt type to Yes.
  - The Python runner switches on unattended-script mode for the whole editor process anyway (`EditorPythonExecuter.cpp:145`), so default answers apply either way. The only 4.25 prompts that default to Yes are import/overwrite prompts the recipe never reaches.
- **Time limit:** 1,800 s from process creation. At the limit only our editor's process tree is stopped, crash reporter included.
- **Clean exit:** requires the probe's skipped `engine_exit` cleanup row and exactly one `probe_stop`, and no PIE.

**6. The kit**
- **Files:** `D:\IA_Europa\Launch\Europa425-FixtureAuthoring.bat` (`4c5ad14e…`) and `Europa425-FixtureAuthoring.json` (`42c89db2…`). It cites `094-21-ruling.md#EW2` and #EW3, and its runtime scope equals EW2.
- **Real slots** (all 15 paths absent):
  - attempts: `Receipts\09422\fixture-attempts\EA_Core\01–03` and `EA_Travel\01–03`;
  - TEMP: the same layout under `Temp\09422\fixture-attempts`;
  - state: `Receipts\09422\fixture-state\EA_Core`, `\EA_Travel`, `\global`.
- **Gate:** the full measured profile, the launcher must already be running, and the profile is re-checked about every 25 s during the session.
- **Every line the run can show:**
  ```text
  Europa fixture authoring: checking the EW2 arming.
  {recipe}: attempt {n} of 3; checking protected files, Content, binaries and routing.
  {recipe}: protected files, Content, binaries and routing passed.
  Waiting: three passing polls, 25 seconds apart.
  Waiting: {each unmet condition joined by ; }.
  Measured gate passed: three quiet polls; the launcher is already running.
  Stopped: no measured idle window within four hours. No editor was started.
  {recipe}: armed dry run passed. No editor was started; real slots remain unused.
  {recipe}: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in {path}.
  {recipe}: clean exit and output checks passed; this recipe is now spent.
  {recipe}: previously completed; its launch authorization remains spent.
  Stopped: {recipe} did not pass. No later recipe or automatic retry will start.
  Stopped: {reason} No further session will be started.
  Both recipe dry runs finished. No editor was started.
  Both recipe sessions finished. The two launch authorizations are spent.
  ```
- **Waiting conditions:** mouse and keyboard idle for 300 seconds; 6 GiB free physical memory; GPU use at or below 15 percent; a readable running-program list; Epic Games Launcher already running; foreign Unreal process PID n to close; disk X: above its floor. Each can carry "(reading unavailable)".

**7. My review** (`_reviews\094-22-code-review.md`): 52 rows — 42 OK, 6 N/A, 2 OPEN (carried), 1 GAP, 1 decision, 0 STOP.
- **G4 (blocking):** the launcher's own `EOSOverlayRenderer-Win64-Shipping.exe` children (5 running now, direct children of `EpicGamesLauncher.exe`) hold the measured gate. I saw this live; the dry runs missed it because they must use synthetic readings.
- **D2:** the in-session gate counts our own editor's memory and GPU use. Free memory today was 6.9–9.7 GiB before any editor, against a 6 GiB floor.
- **O1:** the clean exit is still proven from source only.
- **O2b:** four extra code shapes outside the 8 ruled calls still pass the static check. None of them occurs in the probe.
- **O3:** the Python runner's unattended-script mode makes default dialog answers apply with or without `-unattended`.
- **O4:** non-build helper processes can inherit `EUROPA_*` variables from their parent; the kit's entry point strips them.
- **O5:** dry runs can't read the live gate.
- **O6:** the live config's job template says "preparation and dry-run only", and real slots would copy that wording.
- **O7:** a clean editor exit deletes its own EditorRuns PID marker, so an editor that happened to get PID 31124 would remove the ET2 marker. The odds are negligible.
- **Design:** every 4.25 citation is verified; no `.uproject` or config change is needed; outputs are exact paths and nothing is overwritten.
- **Self-tests re-run:** 89/89, 17/17, 159/159, 62/62 + 48, 48/48, 12/12, and 33/33 on each of the four sources.
- **My can-fails:** 44/46 — the 2 failures are G4. Refusal reasons 8/8 through the real `.bat`. O2 mutants 16/16, plus 21/21 earlier ones. G1 and the EN1 synthetic-makefile test 16/16. The six existing makefiles are unchanged.
- **My dry run of the real `.bat`:** both recipes passed, 0 launches, real slots still absent.
- **Unchanged after the job:** 110/110 bound files, all three manifests, and Paris Content byte-identical. The 09417 kit is unchanged and still spent.
- **Canary:** 0 hits by value in 1,115 files from the launch second, and 0 in 1,584 files in the final search. 0 by name in child environments.
- **Scans and audits:** EK1 found 0 secret values in 44,425 files. EU1: 204 commands through the wrapper, 1 direct read-only, 0 direct write-capable.
- **Protected state:** P9 v2 18,264/18,264; m51 matches P9; protected-state check PASS; 0 launches.

**8. Owner actions**
- After this job, as the 094-21 ruling says: restart the PC, double-click `start_watcher.bat`, then start the Epic Games Launcher and leave it running.
- Do not double-click `Europa425-FixtureAuthoring.bat` until chat re-arms the kit after G4. As it stands it would wait 4 hours and use a slot.
- **For the real run, the PC should be:** idle overnight, with the launcher open and nothing else Unreal running (no editors or <other-home-project> builds). Pause the E:\DynamicContents watcher. Close Blender and heavy apps. Keep D: at 40 GiB or more free and at least 8–10 GiB of physical memory free.

**9. NEEDS-DECISION**
- **D1 (G4):** extend the launcher exclusion so it covers the whole running `EpicGamesLauncher.exe` process tree. I recommend that over excluding the overlay by name and install path. Then re-arm, and add a live gate reading to the dry run. This needs an arming job with no launch.
- **D2:** decide what the in-session gate checks:
  - (a) only human idle, foreign processes and disk floors;
  - (b) memory and GPU too, excluding our own share or with a lower floor;
  - (c) keep it as built and accept a likely trip.
  - Also ratify that one session per recipe takes precedence over EA6's rerun.
- **D3:** ratify Codex's EN5 reading that the same session may save its new package more than once. Also rule what happens to a partial package left by a failed session — today it would block every retry.

Separately: several MCP connectors (Google Calendar, GitHub, Slack and others) need authorizing in claude.ai or `/mcp`, and the pdf-viewer server failed to connect. None was needed for this job.
api-equivalent cost (not billed on Max): $29.81 | turns: 199 | running total across all briefs: $5,029.31
