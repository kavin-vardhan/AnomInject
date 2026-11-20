# 094-22 — Claude Code review of the armed A6 fixture-authoring kit (W3: written after Codex exited)

**Date:** 2026-10-28, 07:30–08:25 IST · **Author:** Claude Code (mailbox brief 094-22) · **Inputs:** Codex's report `_reviews\094-22-codex-report.md` (thread `01a14334`, resume 2026-10-28 04:53:35 – 07:29:49 IST, 9,374 s, exit 0, ledger row `094-01-europa-phase-a-plan-resume20` once), chat's `094-21-ruling.md` (EW1–EW4), `094-22-request.md` and the rulings they bind. My receipts: `code-receipts-094-22\` (and `step0\`, `protect\`, `ek1\`, `review\`), `D:\IA_Europa\Receipts\CodeReview09422\`, `D:\IA_Europa\Temp\CodeReview09422\`. No account identifier is reproduced.

## Summary
- **Codex ended GREEN in 2.6 h of its 9 h.** Local commits on `feat/europa-port` (no upstream, no tags, author = committer time): `6e46b9f` (kit), `572f5c0` (entry messages), `8abb79e` (mid-session gate; the kit binds this commit), `1a9afc1` (docs). Kit: `D:\IA_Europa\Launch\Europa425-FixtureAuthoring.bat` `4c5ad14e…` → `fixture_entry.py` → `disk_pause.py` → `fixture_leg.py`; config `Europa425-FixtureAuthoring.json` `42c89db2…`.
- **The design note checks out against 4.25 source, line by line** (§2), and needs no `.uproject` or config change. `-unattended` is correctly omitted under EW3's rule.
- **One BLOCKING GAP (G4), found only by reading the live gate:** with the Epic Games Launcher running, as EU3/EW3 require, its own child processes `EOSOverlayRenderer-Win64-Shipping.exe` (5 live, direct children of `EpicGamesLauncher.exe`) match the measured profile's game-image rule and **hold the gate**. The real kit would print "Waiting: foreign Unreal process PID 39596 to close; …" for 4 hours and stop. Codex's dry runs could not see it: a dry run must use synthetic readings, and Codex's synthetic process lists contained only `EpicGamesLauncher.exe`. Fail-safe (no editor starts), but the real run cannot pass while the launcher runs. Needs a tooling fix and a re-arm.
- **One predicted failure needing a ruling (D2):** the mid-session gate (EA6 mid-leg rule, commit `8abb79e`) re-reads system-wide free memory (≥ 6 GiB) and summed GPU use (≤ 15 %) with **our own editor running**. Free memory was 6.9–9.7 GiB before any editor today, and 094-18's 4.25 session ran 13 ShaderCompileWorkers on 2,715 startup shaders. A trip stops the editor and spends the recipe with no retry.
- **Everything else I checked agrees with Codex.** My dry run of the real `.bat`: both recipes `ARMED_DRY_RUN_PASS`, 0 launches, real slots absent. Self-tests re-run 89/89, 17/17, 159/159, 62/62 + 48, 48/48, 12/12, 33/33 ×4. My can-fails 44/46 (the 2 failures are G4); refusal reasons 8/8 through the real `.bat`; O2 mutants 37/37. 110/110 bound files, Paris Content 1,453 + 5 byte-identical, canary 0, EK1 0 values, EU1 204/1/0, P9 v2 18,264/18,264, m51 = P9, protected PASS.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | EW1 the 09421 binaries are the bound set; kits re-bound | Kit config `manifests` = the three 09421 manifests (`c501895c…`/`b05e6f8c…`/`3865fe0c…`), `fixture_policy.MANIFESTS` hard-codes the same three, `verify_manifests` requires 110 files. Re-hashed 27/39/44 at start and after exit. 09417 kit unchanged (`4b5b2812…`/`5d1d1ab5…`), its state still holds `launched.json` | OK |
| 2 | EW2 scope: EA_Core + EA_Travel only, new packages by exact path in one fixture folder, no overwrite, Content re-hashed | `runtime_scope` = 425 / fixture-authoring / 2 recipes / 2 sessions, 1 per recipe / 1,800 s / new packages only / no overwrite, PIE, cook, commandlet, packaging, production or config edit. Declared: `Content\EuropaPhaseA\SM_EA_TwoLod.uasset`, `EA_Core.umap`, `EA_Travel.umap`. Preflight refuses an existing destination or sidecar; EA_Core needs an empty folder; postflight requires added files == declared and every one of 1,458 existing Content files unchanged. `Content\EuropaPhaseA` absent | OK |
| 3 | EW2 clean exit; no PIE, cook, packaging, production change; no `.uproject`/config change | `probe_exit` requires a contiguous sequence, ≥ 1 `world_cleanup` with `actor_snapshot=skipped`, `skip_reason=engine_exit`, exactly one final `probe_stop` with `root_release=skipped`, and no `world_type 3`. These names match the probe source (`EuropaPhaseAWorldProbe.cpp:62/176/216–225`). Config snapshot (`.uproject`, `Config`, `Saved\Config`, 42 files) compared before and after; `-nowrite` | OK (runtime unproven: O1) |
| 4 | EW2 Python plugin enabled on the command line only | 4.25 `PluginManager.cpp:567–603` parses `-EnablePlugins=` and configures it ahead of the project's list, with no descriptor write on that branch (the only descriptor writes are the missing-plugin prompts at `:1119–1135`; both plugins are present and bound) | OK |
| 5 | EW3 measured profile | 300 s idle / 6 GiB / GPU ≤ 15 % / no foreign Unreal / three spaced polls: `idle_gate.reasons` + `Window`; measured suite 48/48; my 4 boundary cases (299 s, 6 GiB − 1, 15.01 %, GPU unreadable) all wait | OK |
| 6 | EW3 / EU3 the launcher must already run; plain wait; never started | Absent launcher → "Waiting: Epic Games Launcher already running." and the 4 h give-up (my case A, 241 messages); no kit code starts the launcher | OK |
| 7 | EW3 / EU3 the launcher's own processes don't hold the gate; recorded | `EpicGamesLauncher`, `EpicWebHelper`, `UnrealCEFSubProcess` and `EpicOnlineServicesUserHelper` pass. **`EOSOverlayRenderer-Win64-Shipping.exe` (direct children of `EpicGamesLauncher.exe`) holds the gate, synthetic and live** (§4) | **GAP G4 (blocking)** |
| 8 | EW3 / EU4 switch on every argv; refusal without it | `validate_argv` requires the exact string once and the argv equal to the bound template. My 11 argv mutants refuse: missing, `=True`, twice, lowercase extra, other section, `-unattended`, no `-nowrite`, `-game`, `-ExecCmds=PlayInEditor`, other slot's script, `UE4Editor-Cmd.exe` | OK |
| 9 | EW3 `-unattended` rule | Omitted. With the switch, uploads are refused (`WindowsPlatformCrashContext.cpp:598/603/624–633`, `:768/782/801–803`), so condition 1 holds. Conditions 2 and 3 are not established: `MessageDialog.cpp:84–89` defaults `YesNoYesAllNoAllCancel` to Yes, and the direct `MessageBoxExt` "missing target" prompt (`LaunchEngineLoop.cpp:5496–5499`) is not suppressed. The fallback is right (§2, O3) | OK |
| 10 | EW3 time limit; stops only our tree, crash reporter included | `run_child(runtime_guard)` keeps handles (`OwnedTree`) and loops while any owned handle lives. My cases: a detached grandchild orphaned when its parent exits is stopped at the deadline (6.1 s); a 3-level tree is stopped; a sibling survives and exits by itself | OK |
| 11 | EW3 G1 metadata out of build/runtime children | `EUROPA_BOOTSTRAP_PROOF` in 0 of 112 recorded child environments. The 18 execution (EW3-policy) children carry 0 `EUROPA_*`. My synthetic makefiles (425 / 51port / 51base) built from `child_env(execution=True)` pass the strict raw-byte test and EN1's checker. Can-fail: a reinserted proof is caught by both. A runtime child's real process environment has no metadata. The six existing makefiles are byte-unchanged (6/6) | OK (note O4) |
| 12 | EW3 O2 the 8 sibling calls | `probe_teardown_check` PASS on 3 hosts + templates, 33/33 each. **My 16 mutants (2 per call, placed at function end, other functions and spellings) 16/16 detected**; my 21 earlier mutants still 21/21 | OK · OPEN O2b |
| 13 | EW3 / ER1 G3 each refusal shows its reason | Through the real `.bat`, 8/8 show their own `Stopped:` line, none "unexpected error": unarmed, old arming job, widened scope, wrong manifest, extra output, owner profile, launched lock, all three attempts used | OK |
| 14 | EW3 / ER3 bare UBT and dotnet UBT hold the gate | `UnrealBuildTool.exe`, `UnrealHeaderTool.exe` and mixed-case forms hold; dotnet + `UnrealBuildTool.dll` holds; `dotnet build` does not | OK |
| 15 | EW3 8.3 and junction aliases | Codex's self-test made a real junction in its throwaway temp, refused it, and removed only the link. D: has 8.3 names disabled, so the literal `~1` form is refused instead. Mine: `CODERE~1`, `..` forms, a path outside the root, and 2 existing D: junctions (`StackOBot\Saved`, `_bench_sessions_bank`) all refused | OK |
| 16 | EW3 attempts: fresh job root, launched lock, one session per recipe, per-session limit | 3 prelaunch slots per recipe, an existing slot folder is skipped, `launch-reserved.json`/`launched.json` lock the recipe (my H cases, incl. a reservation with no launch); Travel's lock leaves Core usable; 1,800 s | OK |
| 17 | EW3 no launch; dry run on throwaway slots | Codex `dry-run-final` and my `bat-dry-run` (§5) | OK |
| 18 | EW4 carried | EO1 budget, A9, EJ1/EJ3/EJ5, O3 (A8), EV6 glass, O3 memory | OPEN (carried) |
| 19 | EV1 no edit inside a pause | 0 pauses: D: never below 71.53 GiB (156 readings at 60 s) | OK |
| 20 | EV2 | 2.6 h | N/A |
| 21 | EV3 D: 40/30/25 | Job template C 13/11/10, D 40/30/25, E 25/20/15; `idle_gate.FLOORS` D 25 GiB; my launch hold at 40 GiB | OK |
| 22 | EV4 | No build | N/A |
| 23 | EV5 | Probe source 15 files identical across hosts and unchanged since start; static check row 12 | OK |
| 24 | EV6 / EV7 | EV6 carried to A7; EV7's items are built here (rows 6–16), except row 7 | OK · GAP G4 |
| 25 | EU1 command audit | My audit from the launch offset: **205 commands — 204 through the wrapper, 1 direct read-only (`Get-Content` of rulings/entry files), 0 direct write-capable**; equal to Codex | OK |
| 26 | EU2 foreign dips | 0 pauses; each recipe runs under `disk_pause.py --kind runtime` | OK |
| 27 | EU4 / EU5 / EU6 | EU4 row 8; EU5 closed by EV6; EU6 done | OK |
| 28 | ET1 / ET2 / ET3 | ET2 marker `31124` unchanged (0 B, empty hash, only entry); `write_ledger` admits only zero-byte numeric `EditorRuns` markers. ET3: `ProgramData\Epic` changes are launcher-family log rotation (24 added, 14 removed, 8 changed) | OK |
| 29 | ET5 / ET6 / ET7 | ET5 unchanged; ET6 closed; owner-slot `.tmp` files unchanged (my scanner still can't open them: 2 read errors) | OK |
| 30 | ER2 / ER4 | Not exercised | N/A |
| 31 | EQ1 / EQ2 / EQ3 / EQ4 | EQ1: unreadable readings wait (159/159, 62/62). EQ2: the post-create lock holds (Codex's self-test; my H cases). EQ3: unchanged apart from ER3. EQ4: the 09417 arming stays spent | OK |
| 32 | EP1–EP3 | A6's owner evidence stays bound to the 094-15 binaries (EW1) | N/A |
| 33 | EO1 / EO2 | EO1 budget carried; EO2 unused | OPEN (carried) / N/A |
| 34 | EO3 makefiles | None regenerated; 6/6 byte-unchanged | OK |
| 35 | EO4 one probe source | Row 23 | OK |
| 36 | EO5 closeout | EA8 entry (corrected, with 094-21's accepted history; the uncorrected first comparison is kept) and exit PASS (Codex); report collected byte-identical (`082AE19F…`); ledger row once; W4 on all 4 commits | OK |
| 37 | EO6 | Not touched | OK |
| 38 | EN1 | Row 11; no makefile regenerated | OK |
| 39 | EN2 / EN3 | Not exercised | N/A |
| 40 | EN4 my writes | Only my allowed folders (§7) | OK |
| 41 | EN5 | No hard STOP fired. Codex's one interpretation (same-session writes to a new declared package) is cited to `EditorLevelLibrary.cpp:529–549/635–679` and I confirm it: `NewLevel` saves the new map at once, then `SaveCurrentLevel` saves it again | OK · ratify (D3) |
| 42 | EM2 `CLAUDECODE` | In the launch base (positive control); absent from all 112 child environments | OK |
| 43 | EL1 | 115 child receipts: 112 with environments, all contract PASS, 0 tool/secret names; 3 without (creation-failure tests) | OK |
| 44 | EL2 canary | Positive control at launch (child sha = generated). By value from 04:53:34 IST, 14 roots: **1,115 files, 0 value hits, 0 other canary-shaped**; by name in child environments: 0 | OK |
| 45 | EL3 / EL4 / EL5 | EL4: kit path as above, refusing before any launch; EL5: my wrapper removed only `CLAUDE_CODE_MESSAGING_TOKEN` | OK |
| 46 | EK1 secrets | Pre-scan 43,430 files, full scan after exit **44,425 files: 0 secret values**, live match 0, git dates 0; 2 read errors (the owner-slot `.tmp` files) | OK |
| 47 | EK2 / EK3 | Makefiles decode 6/6; `GIT_OPTIONAL_LOCKS=0` | OK |
| 48 | EI2 / EG1 / EG2 | No build. 4.25 install and every 5.1 engine tree byte-equal in my before/after listings | N/A / OK / N/A |
| 49 | EA6 mid-leg rule | Enforced by `fixture_session_gate`; EW2's one-session limit is taken over EA6's rerun (Codex's precedence, to ratify). It counts our own editor's load | **D2** |
| 50 | P9 v2 / protected / m51 | P9 v2 re-hash 18,264/18,264, file `8e3ed095…` unchanged; protected PASS at start and after exit; m51 PASS at start, both launch gates, after exit and final; its 5 can-fails pass | OK |
| 51 | Launches | 18 polls `launch_class_ours=0`, no sightings file; no UE-class process at exit; my dry run descendants python/cmd only | OK |
| 52 | Commits / push / tags | 4 local commits; no upstream; no tags; only `refs/heads/feat/europa-port` moved | OK |

**Count (52 rows, primary status): 42 OK · 6 N/A · 2 OPEN (carried) · 1 GAP (G4, blocking) · 1 decision (D2) · 0 STOP · 0 stop violations.** Also: D3 ratification on row 41, OPEN O2b on row 12.

## 2. The design note against 4.25 source (all read by me)
- **Mechanism:** `UE4Editor.exe <Paris .uproject> /Engine/Maps/Entry … -EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities -ExecutePythonScript=<attempt>\script\author.py …`.
  - `EditorPythonExecuter.cpp:203–222` parses the switch and refuses non-editor and commandlet use.
  - `:156–171` waits for a world and a finished asset registry, then runs `PY <file>` (`PythonScriptPlugin.cpp:450–466` runs a `.py` token as a file).
  - `:150–152` / `:185–195` defer `QUIT_EDITOR` to the next tick. `QUIT_EDITOR` → `UUnrealEdEngine::CloseEditor` (`EditorServer.cpp:5734–5737`, `UnrealEdEngine.cpp:878–893`) → `RequestEngineExit`, with **no save prompt**.
  - The generated script `runpy.run_path`s the committed recipe and calls `author(recipe, receipt)`. My dry run's two scripts and the recipe compile under the bound Python 2.7.14 (`patchlevel.h:23–30`).
- **Outputs:**
  - `NewLevel` (`EditorLevelLibrary.cpp:506–549`) refuses an existing package and saves the new map through `SaveMap` (`FileHelpers.cpp:4225–4238`).
  - `SaveCurrentLevel` (`:635–679`) saves the level (+ MapBuildData if any; none without a lighting build).
  - `SaveLoadedAsset` (`EditorAssetLibrary.cpp:1111–1132`) and `SetLodFromStaticMesh` (`EditorStaticMeshLibrary.cpp:242`).
  - `NewMap` (`EditorServer.cpp:2238–2299`) has no save prompt.
- **No `.uproject`/config change:**
  - `-nowrite` returns before any `FConfigFile::Write` (`ConfigCacheIni.cpp:1476–1481`).
  - The `-ini:EditorSettings:` override is parsed by base file name (`:1151–1199`) and skipped on save (`:1531/1543`).
  - `-SkipCompile` (`LaunchEngineLoop.cpp:5438–5442`); `-stdout` returns instead of the rebuild prompt (`:5473–5477`). The `MessageBoxExt` missing-target prompt (`:5496–5499`) is avoided only because the bound `.target` exists.
  - `-UserDir`/`-ShaderWorkingDir` (`Paths.cpp:1557/1585`).
- **`-unattended`:** omitted, per EW3's rule (row 9). **Observation O3:** `FExecuterTickable` sets `GIsRunningUnattendedScript = true` at construction and never resets it (`EditorPythonExecuter.cpp:145`). So from module startup every `FMessageDialog::Open` returns its default (`MessageDialog.cpp:93/118`), including the implicit Yes for `YesNoYesAllNoAllCancel`, whether or not `-unattended` is passed. In 4.25 UnrealEd that type is used only by the import-overwrite (`Factory.cpp:353`), factory (`EditorFactories.cpp:5339`) and FBX merge prompts, none of which the recipe reaches; the recipe also refuses any existing asset itself. Codex's note says the same thing more briefly.
- **Footprint:**
  - DDC `D:\IA_Europa\Caches\425`; TEMP in the attempt's D: slot; logs and probe in the attempt folder; stock `Saved` in the owned host.
  - `EditorRuns\<pid>` is created at start and removed at clean exit (`HotReload.cpp:389–434`).
  - In 094-18 the 4.25 editor also spawned `UnrealBuildTool.exe` twice and `CrashReportClientEditor.exe` twice as its own children. These are owned, not in the kit's refusal list (`cl`/`link`/UHT/UAT/UnrealPak/`-Cmd`), and their writes fall to the ledger's classes.
- **Time limit:** 1,800 s from process creation, independent of the gate collector (Codex's blocked-collector test; my orphan test).

## 3. The tooling (`7039df7..8abb79e` + docs `1a9afc1`; 19 files, +1,307/−98)
- New: `fixture_policy.py` (scope, argv, path canonicalisation, Content/config snapshots), `fixture_entry.py` (entry, throwaway dry-run configs, per-recipe `disk_pause` wrapper), `fixture_leg.py` (preflight, P9, gate, launch, postflight), `fixture_attempts.py` (slots and locks on the owner Guard), `fixture_wait.py` and `fixture_session_gate.py` (start and mid-session gates), self-test suites.
- Changed:
  - `idle_gate.py`: launcher requirement and family exclusion; bare UBT/UHT hold the gate; D: floor 25.
  - `environment.py`: `execution_env` strips `EUROPA_*` from execution children.
  - `write_ledger.py`: `runtime_guard` deadline with owned tree, `running_check`, refusal of compile/cook children, EditorRuns marker class.
  - `probe_teardown_check.py` (O2); `author_425_fixture.py` (split into one recipe per call); `owner_p9.py` (094-21's accepted history).
- 0 comments in the new tools (the convention).

## 4. G4 — the launcher's overlay processes hold the measured gate (blocking)
- **Live, 07:40 IST**, the kit's own collector and `reasons()` with the launcher running (`review\live-gate-now1.json`): `foreign_process:39596, 35088, 44568, 40516, 4420`, all `EOSOverlayRenderer-Win64-Shipping.exe`. Every other condition passed.
- **Ancestry** (`review\launcher-family-ancestry.txt`): 39596 is a direct child of `EpicGamesLauncher.exe` 26116, started 1 s after it; the other four are its children. The image is under `C:\Program Files (x86)\Epic Games\Epic Online Services\managedArtifacts\…\`.
- **Cause:**
  - `process_record` marks any `.+-Win64-(Shipping|Development|Test)` image as `blocked_runtime`, and `reasons()` excludes only `epicgameslauncher.exe`, `unrealcefsubprocess.exe` and `epicwebhelper.exe` (EU3's three names).
  - EW3 states the principle: the launcher's own processes don't hold the gate.
- **Effect:** the real kit waits 4 h, then refuses with "Stopped: no measured idle window within four hours" and uses a prelaunch slot. The Waiting line tells the reader to close a launcher process. No editor starts.
- **Why the dry runs passed:** `fixture_entry` requires synthetic gate readings for any dry run. Codex's samples listed only `EpicGamesLauncher.exe`; my dry run included an overlay sample and recorded it waiting (`idle.jsonl` poll 2: `foreign_process:700010, 700011`).
- **Fix options:** see D1.

## 5. My dry run of the real entry point (throwaway slots)
- **Command:** `cmd /d /c D:\IA_Europa\Launch\Europa425-FixtureAuthoring.bat --dry-run --dry-run-root D:\IA_Europa\Receipts\CodeReview09422\bat-dry-run --dry-run-temp D:\IA_Europa\Temp\CodeReview09422\bat-dry-run --dry-run-samples …\mycanfails\bat-samples.json`.
- **Samples:** launcher absent → launcher + 2 overlay processes → low memory + bare UBT → GPU unreadable → dotnet UBT → good ×3 (launcher family present).
- **Result:** exit 0, 07:47:58–08:02:56. For each recipe:
  - real P9 entry PASS;
  - the polls waited for `launcher`, `foreign_process` (the overlay), `physical_memory` + UBT, `gpu:unknown` and dotnet UBT, then CLEAR at 170 s;
  - `ARMED_DRY_RUN_PASS`, launches 0.
- Real slot roots, `fixture-runs` and `Content\EuropaPhaseA` are all absent afterwards.
- **Console, verbatim:**
```text
Europa fixture authoring: checking the EW2 arming.
EA_Core: attempt 1 of 3; checking protected files, Content, binaries and routing.
EA_Core: protected files, Content, binaries and routing passed.
Waiting: Epic Games Launcher already running.
Waiting: GPU use at or below 15 percent (reading unavailable).
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
EA_Core: armed dry run passed. No editor was started; real slots remain unused.
EA_Travel: … (same six lines) …
Both recipe dry runs finished. No editor was started.
```
- Waiting lines print at most once a minute, so the overlay, memory and UBT polls appear only in `idle.jsonl`.
- **Codex's 15 console templates** (report) match the literals in `fixture_entry.py`, `fixture_leg.py` and `fixture_wait.py`. In a live run the G4 line would read "Waiting: foreign Unreal process PID <n> to close; …".

## 6. Self-tests
- **Codex's suites, re-run by me** (`review\rerun-selftests22.json`; outputs under `CodeReview09422`):
  - `fixture_self_tests` **89/89** (33 static, 48 measured, 0 Unreal launches);
  - `fixture_session_tests` **17/17**;
  - `owner_eq_self_tests` **159/159** and `owner_kit_self_tests` **62/62 + 48**, on copies changing only the fixture TEMP base;
  - `idle_gate --self-test` **48/48**; `disk_pause --self-test` **12/12**;
  - `probe_teardown_check --self-test` **33/33** on Paris, Baseline51, ReGate51 and the templates.
  - These suites removed their own test junction link and stopped their own harmless children (code under test, in my throwaway folders).
- **Mine:**
  - can-fails **44/46** (2 = G4);
  - refusals **8/8**;
  - static mutants **37/37** (+5 extra shapes, 4 surviving: O2b);
  - G1/EN1 **16/16**;
  - m51 gate can-fail 5/5;
  - canary, EK1 pre/post/full, P9 re-hash, kit/manifest re-hash.

## 7. Writes, snapshots
- **Mine:**
  - `code-receipts-094-22\`;
  - `resume-prompt-094-22.txt`;
  - the preserved `094-21-*` copies (hash-equal to `code-receipts-094-21\`);
  - `_reviews\094-22-codex-report.md` (body byte-identical, `082AE19F…`) and this file;
  - in `D:\IA_Europa` only `Receipts\CodeReview09422\` and `Temp\CodeReview09422\`.
  - I launched nothing, built nothing, deleted nothing, and stopped no process.
- **Before → after (61 listings): 52 equal**, including the 4.25 install, every 5.1 tree, all three hosts' Binaries/Intermediate/Source/Config/Content, both 5.1 plugins, StackOBot, the release baseline, the cache and `%LOCALAPPDATA%`.
- **The 9 that differ, all expected:**
  - free space and processes;
  - the C:\ root (`ClaudeTemp` mtime);
  - `ProgramData\Epic` (launcher);
  - tooling sizes;
  - refs, worktree heads and porcelain (`feat/europa-port` `7039df7` → `1a9afc1`);
  - `D:\IA_Europa`: +890 files (`Receipts\09422` 870, `Temp\09422` 7, the kit's `.bat`/`.json`, 11 new tools/docs) and 8 changed (`CLAUDE.md` + 7 tools); 0 removed.

## 8. GAPs, OPENs, observations
- **G4 (blocking):** §4.
- **O1 (carried):** the clean exit is still proven only from source. 094-18's exit did produce a `world_cleanup` at exit (then a pre-fix snapshot), so the row the qualification needs does fire on this path.
- **O2b:** four shapes outside the 8 calls survive the pattern check: `MakeWeakObjectPtr(World)`, `X.Get()->`, `(*It)->` in `Stop`, and a dereference in `Cleanup`. None occurs in the probe.
- **O3:** row 9 / §2. The executor's process-wide unattended-script flag means `FMessageDialog` defaults apply with or without `-unattended`.
- **O4:** non-execution (orchestration) children pass `EUROPA_*` through when the parent has them. In Codex's job they carried `EUROPA_JOB_CONFIG`, `EUROPA_OPERATION_DIR` and `EUROPA_DISK_SUPERVISOR_CREATED`/`_PID` (functional), never the bootstrap proof. `fixture_entry` strips all `EUROPA_*` at entry. EW3's "build and runtime child" is met.
- **O5:** dry runs cannot read the live gate, which is how G4 escaped. A read-only live-collector check belongs in the next dry run.
- **O6:** the live config's `job_template.authority` reads "preparation and dry-run only", and it is copied into every real slot's `job-config.json`. Cosmetic, but misleading in a real run.
- **O7:** a stock clean exit deletes `EditorRuns\<own pid>` (`HotReload.cpp:422–433`). An editor that happened to get PID 31124 would remove the ET2 marker. Negligible.
- **Carried:** EW4's list.

## 9. Top 5 risks for the first automated run
1. **G4:** with the launcher running, the gate never clears. Fix before any real run.
2. **Mid-session self-trip (D2):**
   - The session gate counts our editor's own memory and GPU, with a 75 s staleness rule.
   - Today's free memory before any editor was 6.9–9.7 GiB against a 6 GiB floor, and 094-18 compiled 2,715 shaders on 13 workers at startup.
   - A trip kills the editor mid-authoring and spends the recipe.
3. **Partial output blocks every retry:**
   - `NewLevel` writes `EA_Core.umap` before the actors exist.
   - Any later failure (exit-row mismatch, crash, gate trip, deadline) leaves a file that the EA_Core preflight ("folder must be empty") refuses forever.
   - Chat needs a rule for partial packages before the run.
4. **First real clean exit:** qualification needs the exact `engine_exit` skipped cleanup and one `probe_stop`. The fix (EV5) has never run, and a correct authoring session that crashes at exit still fails.
5. **The time budget:** 1,800 s includes startup, a possible DDC/shader miss, and two editor-spawned UBT runs (094-18). The deadline is a hard stop that spends the recipe.

## 10. Decisions for chat
- **D1 (G4), required before any real run.** Options:
  - **(a) Recommended:** exclude, and record, the launcher's process tree. That is every descendant of the running `EpicGamesLauncher.exe`, which covers `EOSOverlayRenderer-Win64-Shipping.exe`, `EpicWebHelper.exe` and `UnrealCEFSubProcess.exe`. The same images outside that tree keep holding the gate. Then re-arm, and add a read-only live-collector reading to the dry run with the launcher running (O5).
  - (b) Exclude `EOSOverlayRenderer-Win64-Shipping.exe` by name and install path only.
  - (c) Keep the rule. The real kit cannot clear while the launcher runs, which EU3 requires, so this blocks the leg.
  - Either fix changes bound tooling hashes, so it needs an arming job (no launch).
- **D2 (EA6 mid-session gate).** Options:
  - (a) mid-session, check only human idle, foreign processes (our tree excluded) and the disk floors;
  - (b) also check memory and GPU, excluding our tree's share, or with a lower in-session floor;
  - (c) keep it as built and accept a likely trip.
  - Also ratify Codex's precedence: one session per recipe, so a lost gate spends the recipe rather than rerunning it.
- **D3 (EN5 interpretation):** ratify that the same session may write the new declared package more than once (`NewLevel`'s first save, then `SaveCurrentLevel`). Also rule what happens to a partial package left by a failed session (risk 3).

VERDICT: REVIEW DONE — 42 OK / 6 N/A / 2 OPEN (carried) / 1 GAP G4 BLOCKING (the launcher's own EOSOverlayRenderer-Win64-Shipping.exe children hold the measured gate; live 5 PIDs; real run cannot clear while the launcher runs) / D2 mid-session self-trip decision / D3 EN5 ratification / 0 STOP / 0 stop violations. Design citations verified in 4.25 source; no .uproject/config change; -unattended correctly omitted. Kit scope = EW2; real slots 0/15 present; my real-.bat dry run ARMED_DRY_RUN_PASS ×2, 0 launches. Self-tests 89/17/159/62+48/48/12/33×4; my can-fails 44/46 (2 = G4), refusals 8/8, O2 37/37, G1/EN1 16/16. 110/110 bound files; Paris Content 1,453+5 identical; canary 0 in 1,115 files; EK1 0 values (44,425); EU1 204/1/0; P9 v2 18,264/18,264; m51 = P9; 0 launches.
