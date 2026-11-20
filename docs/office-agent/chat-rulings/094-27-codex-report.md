<!-- Collected by Claude Code (mailbox brief 094-27) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-30 after Codex exited (thread 01a14334, relay tries 1, exit 0, duration 5,505 s); body below is byte-identical, sha256 33D40E117D218CCDC313F02B68C23C5A0BF218C26A2DB0747498E39B935F6F15, 30073 bytes -->
094-27 is **NEEDS-DECISION**. The 4.25 runtime kit was implemented, rehearsed, armed and dry-run for all 18 sessions. Its only real Smoke editor exited normally with code 0 and found all 31 Python bindings. The new cleanup reader then rejected a valid probe row because it expected the wrong schema, so the kit stopped before all 17 runtime legs. A respawned crash monitor also lacks complete process-tree evidence. No protected-state change, crash or upload evidence was found. A3, A5, A6, A7 and A8 remain incomplete; there are no real A7 pictures or contact sheets.

The armed tooling remains unchanged at `599ef70b18f55206143699c611ce151d1d761737`. The final feature commit is `ccc861b4d2590a057b797f6c6379c400ae507918`; the worktree is clean. No real retry, re-arm after the stop, build, push or tag occurred. The immutable evidence is indexed in [09427-runtime-receipts.json](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427-runtime-receipts.json>). The complete run evidence is under `D:/IA_Europa/Receipts/09427`.

Entry and exit EA8 proofs both passed against P9 v2: **472,553 inventory entries, 1,023 EG1-admitted entries, 23,033 binary/config hashes and 104,794,989,456 bytes hashed** on each proof. Protected git and source-manifest comparisons passed, including the m51 baseline. P9 v2 remained SHA-256 `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. See [entry proof](<D:/IA_Europa/Receipts/09427/P9/entry/summary.json>), [exit proof](<D:/IA_Europa/Receipts/09427/P9/exit/summary.json>) and [final state](<D:/IA_Europa/Receipts/09427/final-state.json>).

The entry review of 094-26d found no blocking GAP and verified all three authored fixtures. Final checks retain all 1,461 Content files, 41 protected config/project records, one descriptor, 332 build files, 198 bound host files, eight engine files and 96 tooling files. The kit binds only the unchanged 27-file 4.25 manifest (`c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6`); the frozen 5.1 hosts were not built or run. Marker 31124 and the two quarantined 09426c packages remain unchanged. [Bound entry](<D:/IA_Europa/Receipts/09427/bound-entry.json>); [result evidence](<D:/IA_Europa/Receipts/09427/result-evidence.json>).

| Drive | Entry free GiB | Final free GiB | Start / warning / floor GiB |
|---|---|---|---|
| C | 30.97 | 30.82 | 13 / 11 / 10 |
| D | 70.48 | 70.01 | 40 / 30 / 25 |
| E | 291.6 | 290.88 | 15 / 12 / 10 |

ED3 recorded Visual Studio 2022 17.12.35707.178 and VS 2017 Build Tools 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; Windows SDK 10.0.17763.0 and 10.0.22621.0. No 14.44 toolchain, compile or export ran. [Toolchain receipt](<D:/IA_Europa/Receipts/09427/toolchains.json>).

The committed [design and dated predictions](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09427-runtime-design.md>) define immutable fixture use, one session per leg, exact argv/routing, failed-evidence retention, measured admission, process ownership, deadlines and readers. The runtime kit derives its environment, ledger, root attribution, locks and gate from the retained authoring machinery. FE3 closes unreadable UBT argv, implicit TargetList and Clean/Execute/Deploy gaps; FD2 build triples, explicit Build, compiler/linker/UHT children and build writes remain stops. FF2 records Saved files while every Saved/Config INI remains protected. New crash folders independently stop the run.

The design’s key 4.25 source citations and their practical limits are:

| Design point | Source and status |
|---|---|
| PIE and three restart cycles | [AutomationEditorCommon.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/Tests/AutomationEditorCommon.cpp:635>) and [EditorAutomationTests.cpp](<E:/EpicGames/UE_4.25/Engine/Plugins/Tests/EditorTests/Source/EditorTests/Private/UnrealEd/EditorAutomationTests.cpp:185>) select System.Maps.PIE; PlayLevel.cpp:821,1050,2453,2494 supplies transient settings, no save and in-process clients. Real worlds legs did not run. |
| Retained Python callback | [PythonScriptPlugin.cpp](<E:/EpicGames/UE_4.25/Engine/Plugins/Experimental/PythonScriptPlugin/Source/PythonScriptPlugin/Private/PythonScriptPlugin.cpp:560>) executes PY; [EditorPythonExecuter.cpp](<E:/EpicGames/UE_4.25/Engine/Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Private/EditorPythonExecuter.cpp:148>) explains why ExecutePythonScript is unsuitable for a retained callback. The imp-loaded module stayed alive in Smoke. |
| Standalone driver | [bound native probe driver](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAProbeDriver.cpp:149>) implements the JSON commands; travel clears the script at line 210. PythonScriptPlugin is Editor-only; no game Python script is installed. No real game leg ran. |
| Analytics and crash privacy | [EngineAnalytics.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:105>) gates editor analytics and excludes -game; EditorServer.cpp:1235 reads usage privacy, ConfigCacheIni.cpp:1100 implements -ini, and GenericPlatformCrashContext.cpp:389 reads crash privacy. Both opt-outs are mandatory in each root session argv. |
| AA and screenshots | [SceneView.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/SceneView.cpp:845>) maps r.PostProcessAAQuality 0 to AAM_None. Bound probe Driver.cpp:245 records engine screenshot request/completion; WindowsPlatformTime.h:23 supplies the QPC clock. No actual A7 picture was taken. |
| Cleanup schema defect | [EuropaPhaseAWorldProbe.cpp](<D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe/EuropaPhaseAWorldProbe.cpp:211>) emits world_cleanup/actor_snapshot=skipped. [runtime_readers.py](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/runtime_readers.py:26>) incorrectly expects world_snapshot_skipped/label=cleanup. |

Admission uses 10 minutes human idle, 6 GiB free physical memory, 8 GiB free commit, GPU at most 15%, the owner-started launcher and three quiet polls. **FB3 takes precedence for the session commit stop: below 6 GiB**, rather than the carried FD3 4 GiB line. Disk floors, deadlines and 60 seconds of unreadable acting data also stop a session. Input, foreign GPU and physical memory are recorded; input/GPU-contaminated A7 windows do not count. The fixed ROIs and glass exclusion, image noise/restoration thresholds and six attempted positive events per anomaly are predictions that remain unqualified by real pictures.

The qualification tests returned the following results before arming. These are synthetic/harness results, not real gate verdicts.

| Suite | Checks | Recorded result |
|---|---|---|
| Runtime scope/argv/guards | 120 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/runtime-tests-final/summary.json>) |
| Runtime readers and pixel/control can-fails | 20 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/runtime-reader-tests-final/summary.json>) |
| Engine Python 2.7.14 harness | 3 editor paths + runpy-dict mutant; 15 native JSON scripts | [PASS](<D:/IA_Europa/Receipts/09427/scratch/python27-attempt1/summary.json>) |
| Retained A/R readers | 69 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/reader-tests/self-tests.json>) |
| Environment reads | 19 tests; 86 helpers / 18 reads | [PASS](<D:/IA_Europa/Receipts/09427/scratch/environment-reads-final.json>) |
| Owner EQ and gate regressions | 159 + 62 owner + 50 measured | [PASS](<D:/IA_Europa/Receipts/09427/scratch/owner-eq-tests>) |
| Session protections | 57 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/session-tests/result.json>) |
| EY regressions | 46 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/ey-tests/result.json>) |
| Ledger deltas | 9 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/ledger-delta-tests/results.json>) |
| Admission | 24 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/admission-tests/result.json>) |
| Retained tooling | 31 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/tooling-tests/tooling-self-tests.json>) |
| Finish checks | 78 | [PASS](<D:/IA_Europa/Receipts/09427/scratch/finish-tests/results.json>) |

The Python harness used the engine’s own 2.7.14 interpreter with a recording unreal stub and no asset saves. It exercised Smoke, Load and WorldsPIE and rejected the runpy-returned-dictionary mutant. Game legs use the already-bound native JSON interpreter, with all 15 generated scripts validated. Relevant can-fails covered missing opt-outs, old armings/unknown legs, protected file writes, UBT build actions, 6 GiB commit and 60-second unreadability, contaminated event windows and image/control failures. The prelaunch suites passed without two failures of an FB4 prerequisite. The real Smoke exposed a blind spot: both the stand-in and reader tests copied the reader’s incorrect cleanup schema; their PASS did not demonstrate compatibility with the actual probe.

The [five-case rehearsal](<D:/IA_Europa/Receipts/09427/scratch/rehearsal-attempt1/summary.json>) used the real runtime runner/ledger/monitor with harmless Python stand-ins under owned scratch. Passing execution, exit-7 failure with retained evidence, a deadline with a detached child, Content-write STOP and UBT-build STOP all matched predictions; the unrelated sibling survived. No Unreal, real slot or Paris Content was touched. The cleanup-reader portion shares the schema limitation above.

Arming bound tooling `599ef70b18f55206143699c611ce151d1d761737`, 18 single-use sessions and deadline `2026-10-31T00:30:00+00:00` (31 October 06:00 IST). The real [runtime config](<D:/IA_Europa/Launch/Europa425-Runtime.json>) has SHA-256 `a71a821634ae71a5824ae7fdf00fe6d53f2593b9ccfcae7bfc52782a09efcba9`; the [entry batch file](<D:/IA_Europa/Launch/Europa425-Runtime.bat>) has SHA-256 `b20dc8c57b694d5852b0ca7f3dc10059683c753d817a31fc4793a8bdc4871f58`. Prior armings are refused. Smoke is spent; every other slot remains absent, and the global stopped result prevents reuse. [Slot closeout](<D:/IA_Europa/Receipts/09427/armed-slots-closeout.json>).

For each slug below, the exact slot paths are `D:/IA_Europa/Receipts/09427/runtime-attempts/<slug>/01`, `D:/IA_Europa/Temp/09427/runtime-attempts/<slug>/01` and `D:/IA_Europa/Receipts/09427/runtime-state/<slug>`. Global state is `D:/IA_Europa/Receipts/09427/runtime-state/global`.

| Leg / slug | Limit s | Real result |
|---|---|---|
| Smoke | 300 | Smoke reader FAIL; slot spent |
| Load | 300 | NOT_RUN; slot unused |
| WorldsPIE | 600 | NOT_RUN; slot unused |
| WorldsGame | 300 | NOT_RUN; slot unused |
| GC | 300 | NOT_RUN; slot unused |
| A7_-blinking-positive | 900 | NOT_RUN; slot unused |
| A7_-lighting_mismatch-positive | 900 | NOT_RUN; slot unused |
| A7_-lod_corruption-positive | 900 | NOT_RUN; slot unused |
| A7_-missing_object-positive | 900 | NOT_RUN; slot unused |
| A7_-blinking-NoApply | 300 | NOT_RUN; slot unused |
| A7_-lighting_mismatch-NoApply | 300 | NOT_RUN; slot unused |
| A7_-lod_corruption-NoApply | 300 | NOT_RUN; slot unused |
| A7_-missing_object-NoApply | 300 | NOT_RUN; slot unused |
| A7_-blinking-SkipRestore | 300 | NOT_RUN; slot unused |
| A7_-lighting_mismatch-SkipRestore | 300 | NOT_RUN; slot unused |
| A7_-lod_corruption-SkipRestore | 300 | NOT_RUN; slot unused |
| A7_-missing_object-SkipRestore | 300 | NOT_RUN; slot unused |
| Admission | 600 | NOT_RUN; slot unused |

All 18 actual-entry-point dry runs passed on throwaway roots `Receipts/09427/scratch/dry` and `Temp/09427/scratch/dry`, with zero editor/game launches. The dry arming hash is `edc4e023fe66fff554c6ef6d3f1a34e9754938fee2e19e519b3a8bb4a77f0bf1`. [Dry result](<D:/IA_Europa/Receipts/09427/scratch/dry/run-result.json>); [dry console](<D:/IA_Europa/Receipts/09427/scratch/dry/entry-console.txt>). FB4 completed at 2026-10-30T08:02:37.491+05:30.

The [live observe-only reading](<D:/IA_Europa/Receipts/09427/live-gate.json>) had no blockers: 27684.922 seconds idle, 6.956 GiB free physical and 12.532 GiB free commit. It launched nothing. The [outer idle precheck](<D:/IA_Europa/Receipts/09427/entry-idle-precheck.jsonl>) passed at 08:06:10 IST and the [Smoke idle precheck](<D:/IA_Europa/Receipts/09427/Smoke-precheck.jsonl>) at 08:07:07 IST. Three gate samples at 08:07:20.569, 08:07:49.596 and 08:08:18.871 each had GPU 4% and over 28,010 seconds idle; final gate elapsed time was 62.518 seconds.

The real timeline contains exactly one session:

| Session | Start IST | End IST | Duration / exit | Outcome |
|---|---|---|---|---|
| Smoke, PID 2344 | 2026-10-30T08:08:27.417+05:30 | 2026-10-30T08:08:42.719+05:30 | 15.301968 s wrapper / exit 0 | Python SMOKE_PASS; reader FAIL |
| All 17 runtime legs | — | — | 0 launches | Blocked by FB7 |

The child boundary measured 13.440499300006195 seconds. The Python callback issued `QUIT_EDITOR`; Runtime.log records CloseEditor/EngineExit and normal shutdown. Active samples reached a minimum 10.718 GiB free commit; no acting failure fired. There was no PIE, crash, or new crash folder. The previous two host crash folders and their eight files are retained and indexed. [Execution](<D:/IA_Europa/Receipts/09427/runtime-attempts/Smoke/01/execution.json>); [31-binding smoke receipt](<D:/IA_Europa/Receipts/09427/runtime-attempts/Smoke/01/driver.json>).

The exact real entry-point console follows, copied from [entry-console.txt](<D:/IA_Europa/Receipts/09427/entry-console.txt>). The outer precheck also printed `Idle pre-check passed: ten minutes idle and all measured conditions hold.` before starting the entry point.

```text
LEDGER before 2026-10-30T02:36:20.042993+00:00
{"scope": "engine51", "files": 339410, "folders": 0, "errors": 0}
{"scope": "engine425", "files": 179533, "folders": 0, "errors": 0}
{"scope": "paris_original", "files": 1511, "folders": 0, "errors": 0}
{"scope": "unreal_engine", "files": 1271, "folders": 0, "errors": 0}
{"scope": "ubt", "files": 43, "folders": 0, "errors": 0}
{"scope": "uht", "files": 26, "folders": 20, "errors": 0}
{"scope": "crashclient", "files": 81, "folders": 0, "errors": 0}
{"scope": "epic", "files": 4657, "folders": 0, "errors": 0}
{"scope": "c_root", "files": 16, "folders": 44, "errors": 0}
{"scope": "d_root", "files": 35, "folders": 78, "errors": 0}
{"scope": "e_root", "files": 5, "folders": 65, "errors": 0}
{"scope": "windows", "files": 197246, "folders": 0, "errors": 52}
{"scope": "wer_system", "files": 60, "folders": 0, "errors": 132}
{"scope": "gpu_nvidia", "files": 93, "folders": 0, "errors": 0}
{"scope": "gpu_dx", "files": 67, "folders": 0, "errors": 0}
{"scope": "tool_claude", "files": 16649, "folders": 0, "errors": 0}
{"scope": "tool_codex", "files": 12590, "folders": 0, "errors": 0}
{"scope": "tool_temp", "files": 7774, "folders": 0, "errors": 55}
{"scope": "port", "files": 71219, "folders": 0, "errors": 0}
{"scope": "ue_temp_0", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_1", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_2", "files": 0, "folders": 0, "errors": 0}
Idle pre-check passed: ten minutes idle and all measured conditions hold.
Smoke: checking Content, config, binaries, tooling and routing.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
Smoke: starting its one session; limit 300 seconds.
Stopped: No skipped cleanup proof. No further session will be started.
LEDGER after 2026-10-30T02:38:51.404906+00:00
{"scope": "engine51", "files": 339410, "folders": 0, "errors": 0}
{"scope": "engine425", "files": 179533, "folders": 0, "errors": 0}
{"scope": "paris_original", "files": 1511, "folders": 0, "errors": 0}
{"scope": "unreal_engine", "files": 1271, "folders": 0, "errors": 0}
{"scope": "ubt", "files": 43, "folders": 0, "errors": 0}
{"scope": "uht", "files": 26, "folders": 20, "errors": 0}
{"scope": "crashclient", "files": 81, "folders": 0, "errors": 0}
{"scope": "epic", "files": 4657, "folders": 0, "errors": 0}
{"scope": "c_root", "files": 16, "folders": 44, "errors": 0}
{"scope": "d_root", "files": 35, "folders": 78, "errors": 0}
{"scope": "e_root", "files": 5, "folders": 65, "errors": 0}
{"scope": "windows", "files": 197246, "folders": 0, "errors": 52}
{"scope": "wer_system", "files": 60, "folders": 0, "errors": 132}
{"scope": "gpu_nvidia", "files": 93, "folders": 0, "errors": 0}
{"scope": "gpu_dx", "files": 67, "folders": 0, "errors": 0}
{"scope": "tool_claude", "files": 16649, "folders": 0, "errors": 0}
{"scope": "tool_codex", "files": 12590, "folders": 0, "errors": 0}
{"scope": "tool_temp", "files": 7774, "folders": 0, "errors": 55}
{"scope": "port", "files": 71240, "folders": 0, "errors": 0}
{"scope": "ue_temp_0", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_1", "files": 0, "folders": 0, "errors": 0}
{"scope": "ue_temp_2", "files": 0, "folders": 0, "errors": 0}
{"result": "PASS", "classification_counts": {"port_C_exception": 20, "foreign": 1, "port_D": 54, "tooling": 6}, "exception_growth_charged": {"C": 0, "E": 0, "engine51": 0}, "cumulative_job_exception_growth": {"C": 0, "E": 0, "engine51": 0}, "stops": [], "root_attributions": [], "operation": "runtime-run", "kind": "runtime", "time_utc": "2026-10-30T02:39:45.587214+00:00", "disks_before": {"C": {"resolved": "C:\\", "free": 33198497792, "start": 13958643712, "warning": 11811160064, "floor": 10737418240}, "D": {"resolved": "D:\\", "free": 75105419264, "start": 42949672960, "warning": 32212254720, "floor": 26843545600}, "E": {"resolved": "E:\\", "free": 312979718144, "start": 16106127360, "warning": 12884901888, "floor": 10737418240}}, "disks_after": {"C": {"resolved": "C:\\", "free": 33213743104, "start": 13958643712, "warning": 11811160064, "floor": 10737418240}, "D": {"resolved": "D:\\", "free": 75104354304, "start": 42949672960, "warning": 32212254720, "floor": 26843545600}, "E": {"resolved": "E:\\", "free": 312977932288, "start": 16106127360, "warning": 12884901888, "floor": 10737418240}}, "snapshot_before_sha256": "c0ffd72a4c24cdc7ccce31a4a197246482b11f0e23cf3a0339603734a2d3f52a", "snapshot_after_sha256": "5301ec6a6d343d389fc1702cf1817c232b57e9cfcdb075470d60bf807e40f12f", "port_D_bytes_before": 107596210910, "port_D_bytes_after": 107596955309, "tooling_before": {"tool_claude": {"path": "<USERPROFILE>\\.claude", "resolved": "<USERPROFILE>\\.claude", "present": true, "files": 16649, "folders": 0, "bytes": 11885427684}, "tool_codex": {"path": "<USERPROFILE>\\.codex", "resolved": "E:\\CodexData\\home", "present": true, "files": 12590, "folders": 0, "bytes": 32400025628}, "tool_temp": {"path": "C:/ClaudeTemp", "resolved": "C:\\ClaudeTemp", "present": true, "files": 7774, "folders": 0, "bytes": 718331976}}, "tooling_after": {"tool_claude": {"path": "<USERPROFILE>\\.claude", "resolved": "<USERPROFILE>\\.claude", "present": true, "files": 16649, "folders": 0, "bytes": 11888554895}, "tool_codex": {"path": "<USERPROFILE>\\.codex", "resolved": "E:\\CodexData\\home", "present": true, "files": 12590, "folders": 0, "bytes": 32400242754}, "tool_temp": {"path": "C:/ClaudeTemp", "resolved": "C:\\ClaudeTemp", "present": true, "files": 7774, "folders": 0, "bytes": 718331976}}, "scan_access_errors_before": 239, "scan_access_errors_after": 239, "scope_note": "One initial full inventory, canonical SHA-256 per snapshot, before/after deltas per operation. Error-fatal protected enumeration; listed denied OS/tooling paths are not treated as empty.", "ledger_bytes": 225483489}
```

The preserved [probe JSONL](<D:/IA_Europa/Receipts/09427/runtime-attempts/Smoke/01/session/probe/probe.jsonl>) has 41 rows: row 38 is `world_cleanup`, `actor_snapshot="skipped"`, `skip_reason="engine_exit"`; row 41 is the unique `probe_stop`, `root_release="skipped"`, with no outstanding screenshot. The armed reader instead required `world_snapshot_skipped` and `label="cleanup"`. Its [FAIL verdict](<D:/IA_Europa/Receipts/09427/runtime-attempts/Smoke/01/reader.json>) remains unchanged. Postmortem interpretation does not convert it to a pass or authorize another launch. The required correction is to test the reader against these real rows, with missing/duplicate/unsafe-stop mutations, before a fresh authorized arming.

Both session logs have zero error/ensure/fatal findings and no POST/upload/send evidence. The crash log contains a DataRouter URL configuration line, not a request, and records `Editor/ExitCode:0`, `MTBF/NoSessionFound`, `MTBF/NoTempCrash` and normal exit. [Upload check](<D:/IA_Europa/Receipts/09427/smoke-upload-check.json>); [error/ensure and shutdown evidence](<D:/IA_Europa/Receipts/09427/errors-ensures.json>). This is log-based evidence, not a packet capture. The exact editor argv retains EU4 `bSendUnattendedBugReports=False`, FC4 `bSendUsageData=False`, `-nowrite` and AA quality zero. No actual -game upload behavior was exercised.

The owned UBT child was PID 13868, parent 2344, with argv `"E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms`; its conhost was PID 39276. No compiler/linker/UHT child or build-output change was observed. The respawned crash-monitor log targets `-MONITOR=2344`, but its process is absent from the retained owned list. Thus complete crash-monitor parent/handle lifetime coverage is **not proven**; the stand-in deadline test does not resolve this real respawn gap. Read-only closeout found zero UE-class processes. [Process closeout](<D:/IA_Europa/Receipts/09427/process-closeout.json>); [owned process and argv receipt](<D:/IA_Europa/Receipts/09427/ledger/runtime-run/child-001.json>).

Protected outputs remained byte-identical. This job produced receipts only; its three existing fixtures are:

| Fixture under host Content/EuropaPhaseA | Bytes | SHA-256 |
|---|---|---|
| SM_EA_TwoLod.uasset | 137553 | bbd24a79a2b46efeea8a8e3d23c65ebb1eef73b16ea1af97e04ff8bb7cec714a |
| EA_Core.umap | 21114 | c2986f9aee65dbb5d5b1af0016759f7ef881d3636400de19938206fb05284a14 |
| EA_Travel.umap | 21124 | 51c7fe5c212b2c7996fa5af4beebdf79bf0b01ceaf59d0a041ceb34f39fd70d0 |

FF2 recorded one Saved change: `Saved/Autosaves/PackageRestoreData.json` had a timestamp change with identical size/hash. FC5 recorded 21 asset-registry-cache timestamp changes, added `Intermediate/AssetRegistryCache/ec29d66a.bin`, and changed `Intermediate/CachedAssetRegistry.bin`. All are enumerated with before/after values in [managed changes](<D:/IA_Europa/Receipts/09427/managed-changes.json>). Existing WorldState JSON, Backup maps and crash folders stayed unchanged. No partial runtime Content existed to quarantine; failed session evidence remains in its original Smoke slot. The prior authoring quarantine was neither restored nor changed.

| Gate | Reader verdict / remaining evidence |
|---|---|
| A3 | NOT_RUN: cold load/property readback and full compile-out qualification remain. |
| A5 | NOT_RUN: two PIE worlds, cycles, pause/travel, preview and standalone remain. |
| A6 | NOT_RUN: reflected GC remains. |
| A7 | NOT_RUN: all four anomaly positives, both controls, refusals and pictures remain. |
| A8 non-package | NOT_RUN: admission, paired seeded sequence, registry, content admission and residue remain. |

The [gate-verdict file](<D:/IA_Europa/Receipts/09427/gate-verdicts.json>) marks every gate incomplete. The [contact-sheet index](<D:/IA_Europa/Receipts/09427/contact-sheet-index.json>) explicitly contains zero real pictures/contact sheets. Synthetic PNGs under scratch are test artifacts and are not presented for the owner’s eye check. There is no A7 milestone pass.

The five completed main-job ledger operations all passed: entry/setup, runtime tooling, the real runtime run, documentation and metadata closeout. C:, E: and 5.1-engine charged growth is **0 / 0 / 0 bytes**, against 250,000,000 / 50,000,000 / 100,000,000-byte limits. Runtime classification included 20 C: exception metadata changes with zero charged growth; there were no engine/root STOPs, no new or removed root attribution and no EU2 pause. The 239 already-denied OS/tooling enumeration paths remained listed, not treated as empty. [Ledger summary](<D:/IA_Europa/Receipts/09427/ledger-summary.json>) retains EA4/EG1/ET3 classes and all canonical before/after hashes. Foreign activity was recorded without intervention.

The final EU1 audit passed with 173 filtered shell entries, one direct read-only bootstrap inspection deviation, two boundary refusals before a child could start, 30 file-edit events, and zero pause-window violations. The direct inspection read the previous wrapper so the new denylist boundary could be established. No direct write-capable shell ran. The two pre-child refusals were a bootstrap receipt lock collision and an ambiguous wrapper parameter. Ordinary read-command errors (path guesses, a JSON-list inspection and quoting) are retained in the audit, not treated as evidence passes. [Command audit](<D:/IA_Europa/Receipts/09427/command-audit-final.json>).

The canary positive control was present in the base environment, hash-matched without recording its value, and removed at recorded child boundaries together with EM2 blocked names. The final launch-second scan passed: 1905 files, 641322710 raw bytes, zero matches and zero unexplained read gaps. FE2 excluded 6 OS-access-denied event logs under `C:/Windows/System32/winevt/Logs`; readable logs were scanned, and the excluded names/count are recorded. Gzip/ZIP contents were decoded where applicable. No full EK1 scan ran. [Canary receipt](<D:/IA_Europa/Receipts/09427/canary-final.json>).

EN5 was applied with the following source citations and limits. No new exception was used to bypass the failed smoke reader or a hard STOP.

| EN5 item / carried amendment | Application |
|---|---|
| 094-13 EN5:55–58, documented stock behavior | Source-backed command-line settings and existing managed classes were used; no new benign-behavior waiver was needed. The cleanup defect was reported and the run stopped. |
| EN5:61; EL1/EL2, EM2 | Secret/canary and blocked names removed at recorded boundaries; positive control and final scan passed. Crash-monitor lifetime coverage remains separately incomplete. |
| EN5:62; EL1/EN1 | Non-blocked base variables preserved by the execution contract; no broad environment dump. |
| EN5:63; EA4/EG1/ET3 | D: routing and scoped exceptions recorded; zero charged C:/E:/5.1 growth. |
| EN5:64,67; EA8/P9 | Entry/exit P9 v2 and final protected hash checks passed; Content/config/descriptors/binaries unchanged. |
| EN5:68; EG1/ET2 | No unapproved engine-tree change; marker 31124 retained unchanged. |
| EN5:69; EK1/EL2/FE2 | No canary match; only denied Windows event logs excluded as a class, with count/folder. |
| EN5:70 | No machine, security, service, global setting or owner-process changes. |
| EN5:71; FB1/FB2/FB7 | This is a launch-authorized job. Only one armed Smoke editor was started; runtime legs were blocked. No cook, commandlet, UAT or packaging. |
| EN5:72; EU2/EV1/EV3/EY4/EZ2 | Disk lines unchanged; no actual floor breach or pause. Admission waits and acting stops were tested. |
| EZ1/FA6 | No root changes required attribution; foreign workload was never opened or terminated. |
| FF2/FC5/FF5 | Saved/Intermediate managed changes recorded; all Saved/Config INI protected. One full entry proof, fast per-leg checks, full exit proof. |
| FD2/FE3; FD3 as amended by FB3 | Stock ValidatePlatforms admitted with argv; build-action checks retained. Start 8 GiB commit / 6 GiB physical; session stop below 6 GiB commit. |
| FC4/FE1/FB7 | Crash/analytics opt-outs and AA command-line override; no config edit. No upload evidence; actual game and capture paths unrun. |
| FB4/FB7; D2; W4 | Preparations passed; the one failed real Smoke ended the run. No retry. Documentation-only index correction removed two mutable ledger entries; a CRLF warning normalization refusal was corrected before its commit. No timestamp override, push or tag. |

Commits made on `feat/europa-port` are:

| Commit | Purpose |
|---|---|
| 599ef70b18f55206143699c611ce151d1d761737 | Runtime tooling/design/tests; armed and exercised unchanged. |
| 500e4238d7173f93876d31a75a39e515e6f72506 | Stopped-run evidence index and documentation. |
| ccc861b4d2590a057b797f6c6379c400ae507918 | Exclude mutable ledger current.sqlite and scratch-removals.jsonl from the immutable index; frozen operation evidence remains bound. |

The two documentation commits used actual W4 timestamps 2026-10-30T08:22:00+05:30 and 2026-10-30T08:27:52+05:30. The final index verifies 1686 immutable receipts. The first final-index check found the mutable-ledger mistake; the corrected check passed. [Index correction](<D:/IA_Europa/Receipts/09427/index-correction.json>). No production C++ change or build was made; strict-include build and build archive creation were inapplicable. Scoped bench/CVar/ini overrides ended with the editor; no persistent setting was changed.

All existing executable/DLL hashes and the complete 27-artifact binding are listed in [binary-hashes.json](<D:/IA_Europa/Receipts/09427/binary-hashes.json>) and [binary-hashes.tsv](<D:/IA_Europa/Receipts/09427/binary-hashes.tsv>). No executable, DLL or binary archive was created. The exact paths and re-hashes of every compressed receipt archive and test-named executable/DLL are in [archive inventory](<D:/IA_Europa/Receipts/09427/archive-image-inventory.json>). They are evidence/test artifacts, not new builds.

The remaining work is concrete: correct the cleanup reader against the preserved real probe schema, prove crash-monitor respawn ownership, then qualify and freshly authorize the runtime run. All 17 legs and the owner’s picture review remain outstanding. The current spent kit must not be restarted.

VERDICT: NEEDS-DECISION | Runtime kit prepared; real Smoke stopped on an incorrect cleanup reader; all runtime gates and pictures remain unproved.
