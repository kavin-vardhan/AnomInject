<!-- Collected by Claude Code (mailbox brief 094-28) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-30 after Codex exited (thread 01a14334, relay tries 2, exit 0, duration 20,359 s); body below is byte-identical, sha256 C97E664EBDCDF0B2EA1694336142F20E0DB1F9B71F29F4CE9371D26AE2D372D9, 36794 bytes -->
**NEEDS-DECISION.** The fixes, Python 2.7 tests, retained suites, stand-in rehearsals, re-arm and dry runs passed. The real Smoke then stopped because my UBT guard admitted only `QueryTargets` and rejected the editor’s stock `ValidatePlatforms` command. The kit stopped its owned tree after 9.547 seconds; neither recipe started and no package was created. Protected files are unchanged. A complete real Smoke, clean shutdown and authoring remain unproved; no retry or runtime fix followed the STOP.

094-28 evidence: [run report](<D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/report.md>), [receipt index](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09428-authoring-receipts.json>) and [Smoke slot 01](<D:/IA_Europa/Receipts/09428/fixture-attempts/Smoke/01>). The final local head is `dce872d2668a49087457f2bfd12e5f676af59220` on `feat/europa-port`; the kit still binds `c2e73180fdf39f3906ff9a0773ab860834bdf7c0`. No push or tags.

**Entry and exit proofs.** [EA8 entry](<D:/IA_Europa/Receipts/09428/P9/entry/summary.json>) and [full EA8 exit](<D:/IA_Europa/Receipts/09428/P9/exit/summary.json>) both passed against P9 v2: 472,553 inventory files, 23,033 selected binary/config hashes and 104,794,989,456 bytes hashed; protected git and source manifests equal. P9 v2 SHA-256 remains `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. `m51` remains `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67` with its baseline index/status.

Disk start/warn/floor lines were C: 13/11/10, D: 40/30/25 and E: 15/12/10 GiB. Entry free space was C: 30.42, D: 62.30 and E: 299.09 GiB. The [post-commit state check](<D:/IA_Europa/Receipts/09428/final-state.json>) passed 1861 unique bound hashes, Content/config/build snapshots and the unchanged kit, with a clean feature worktree. Final free space was C: 31.06 GiB, D: 73.49 GiB, E: 291.64 GiB. No actual EU2 pause occurred. [ED3 toolchains](<D:/IA_Europa/Receipts/09428/toolchains.json>) remained VS 2022 17.12.35707.178, VS 2017 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; SDK 10.0.17763.0 and 10.0.22621.0.

**Fixes and source evidence.** FC2 replaces the temporary `runpy` namespace with a retained module loaded by `imp.load_source`. The launcher retains the module while calling the unchanged A5 authoring script. Installed Python 2.7.14 [runpy.py](<E:/EpicGames/UE_4.25/Engine/Binaries/ThirdParty/Python/Win64/Lib/runpy.py:40>) removes its temporary module; lines 272–276 return a copied dictionary. [CPython 2.7.14 import.c](https://github.com/python/cpython/blob/v2.7.14/Python/import.c) implements the retained `sys.modules` entry and module execution (lines 588–617, 654–697 and 2967–2983). The actual engine interpreter, rather than a Python 3 approximation, passed syntax, imports, the generated launcher and both complete recorded recipe sequences. The old runpy-dictionary mutant failed.

FC4 requires exactly one of each process-only override, and refuses missing or contradictory values:

```text
-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False
-ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False
```

The 4.25 [command-line config override parser](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/ConfigCacheIni.cpp:1151>) applies these values; final config loading is at line 3300. [AnalyticsPrivacySettings.h](<E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Classes/Analytics/AnalyticsPrivacySettings.h:18>) declares the EditorSettings class, and [EditorServer.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/EditorServer.cpp:1235>) reads the usage-data setting. [EngineAnalytics.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:105>) guards provider creation; lines 169–181 also guard the editor-session summary writer/sender. [GenericPlatformCrashContext.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/GenericPlatform/GenericPlatformCrashContext.cpp:363>) transfers both privacy values to shared crash context. [WindowsPlatformCrashContext.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Windows/WindowsPlatformCrashContext.cpp:779>) adds `-NoAnalytics` when usage data is disabled. Its line 1106 still starts the monitor: a CrashClient.log alone is not an upload. The installed tree lacks the CrashReportClient program implementation; the real session logs remain necessary evidence, and no packet-capture claim is made.

FC5 records, rather than binds, only `Saved/Config/CrashReportClient/UE4CC-*`, `Saved/Autosaves/PackageRestoreData.json`, host Intermediate asset-registry caches and editor logs. Project Config, other bound config, descriptors, .uproject, Content, binaries and manifests remain protected. [Stock crash-config purge](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/GenericPlatform/GenericPlatformCrashContext.cpp:909>) supports the explicit FC5 amendment. FC6 records owned-child PID, parent, creation time and argv at each process poll. A target-discovery query is permitted; an unreadable/unknown/build argv, compiler, linker or UHT is refused. [DesktopPlatformBase.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Developer/DesktopPlatform/Private/DesktopPlatformBase.cpp:882>) supplies the stock `-Mode=QueryTargets` path. Build-output hashes remain a separate check.

The new Smoke session uses the real launcher and fixture module, checks every recipe API by name and signatures where Python exposes them, writes its receipt and exits. Its script makes no Unreal call that creates, loads or saves an asset. Stub tests do not prove native API availability; that is the purpose of the real Smoke. The kit enforces Smoke success before either recipe, the 600-second FA5 pre-check before each real entry, the unchanged measured session gate and the FC7 deadline.

**Self-tests.** All retained suites passed on source commit `c2e73180fdf39f3906ff9a0773ab860834bdf7c0`, before arming. Expected negative controls remain failures of their tested gate, not suite failures. [Final suite binding](<D:/IA_Europa/Receipts/09428/final-suites.json>).

| Evidence | Result |
| --- | --- |
| [Engine Python 2.7 harness](<D:/IA_Europa/Receipts/09428/python27-attempt2/summary.json>) | PASS: Smoke 0 Unreal calls; Core 92 and Travel 89 ordered calls; runpy and missing-API mutants rejected; zero asset writes. |
| [FC checks](<D:/IA_Europa/Receipts/09428/fc-tests-corrected/summary.json>) | 46 PASS: privacy argv, managed classes, UBT query/build/unknown, old POST positive controls, argv reader and admission conditions. |
| [Whole retained suite run](<D:/IA_Europa/Receipts/09428/suites-corrected2/summary.json>) | 21 PASS, including fixture, session, EY, EQ/owner, measured gate, environment reads, disk pause, root attribution, ledger, readers, static checks and prior unit suites. |
| [Real-branch rehearsal](<D:/IA_Europa/Receipts/09428/rehearsal-summary.json>) | All three scenarios PASS on the final source commit; zero Unreal launches. |
| [Real entry-point dry runs](<D:/IA_Europa/Receipts/09428/dry-run-result.json>) | Smoke, EA_Core and EA_Travel ARMED_DRY_RUN_PASS; zero launches; all 18 real paths unused. |

**Rehearsal and D2 history.** The final three stand-in scenarios exercised the actual BAT → entry → non-dry leg → reservation/marker → child → postflight/lock/quarantine path on owned scratch: all three sessions passing; Smoke failing with both recipes unstarted; and a synthetic owned UBT build action stopping the tree while its sibling survived. The latter two correctly returned STOP/exit 2. Every stand-in deadline was 30 seconds; no Unreal executable ran. [Rehearsal summary](<D:/IA_Europa/Receipts/09428/rehearsal-summary.json>) binds the qualifications and closed-ledger archive receipts.

The first rehearsal was stopped by owned PID/creation-time receipt before any session because inspection found `smoke_fail` included in two deadline-mode tuples. Commit `c2e7318` corrected those tuples. A scratch-path precheck first used `/attempt`, then passed with the required `/fixture-attempts/Smoke/01` shape; nested child receipts are one failed invocation. The corrected rehearsal then lost its orchestration turn to the recorded “Selected model is at capacity” platform error after Smoke/Core had passed and during Travel preflight. No Travel completion or exit code is invented. [Interruption evidence](<D:/IA_Europa/Receipts/09428/rehearsal-interruption-2.json>) records absent owned processes, unchanged source and unused real slots. The fresh `2-resume1` run repeated the complete three-scenario rehearsal on the same commit. No runtime gate was weakened or re-tuned.

**Arming, dry run and live reading.** [Arming receipt](<D:/IA_Europa/Receipts/09428/arming/receipt.json>) binds source `c2e73180fdf39f3906ff9a0773ab860834bdf7c0`, 1,458 existing Content files, 41 config files and all 110 EW1 artifacts. The retired 09425/09424/09423/09422 configurations refuse. The old 09425 Core slot and 09417 owner kit stay spent. Probe source remains `40a8eaf983518fe1992e172434aa8caf347654f1`. The 5.1 manifests remain frozen records.

Real roots are `D:/IA_Europa/Receipts/09428/fixture-state/{global,Smoke,EA_Core,EA_Travel}`; Smoke uses `fixture-attempts/Smoke/01`; each recipe has prelaunch slots `fixture-attempts/EA_Core/01–03` and `fixture-attempts/EA_Travel/01–03`, with TEMP mirrored under `D:/IA_Europa/Temp/09428`. Those are 18 paths. They authorize one Smoke and at most one session per recipe, not three recipe sessions. Smoke has a 300-second session cap and no output package; recipes retain 1,800 seconds. [Exact argv](<D:/IA_Europa/Receipts/09428/arming/exact-argv.json>); [console catalog](<D:/IA_Europa/Receipts/09428/arming/console-catalog.json>).

Dry runs used throwaway `Receipts/09428/dry-run` and `Temp/09428/dry-run` roots with explicitly synthetic samples, while exercising real protected-state, content, binary, routing and config checks. [Step 4 completion](<D:/IA_Europa/Receipts/09428/step4-complete.json>) preceded the real pre-check. The [observe-only live reading](<D:/IA_Europa/Receipts/09428/live-gate.json>) at 00:31:41–00:31:46 IST on 10-30 recorded 676.359 seconds idle, about 9.19 GiB physical and 10.95 GiB commit free, GPU 0%, zero reading errors and zero misread launcher blockers. Eleven launcher/helper PIDs were exempt by install folder; the sole reason was `commit_memory`, with “Waiting: 12 GiB free commit.” The PC had not restarted during this job; boot remained 13:16:14 IST on 10-29 and the owner’s launcher instance began 18:08:07. [Restart/launcher receipt](<D:/IA_Europa/Receipts/09428/restart-state.json>).

Armed BAT SHA-256: `4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41`. Armed JSON: 529,207 bytes, SHA-256 `54cfaa61c1cc0829b503449a80e333374a5125c89d4ef7183a66f4edf7e3f2e6`. The committed design is [09428-authoring-amendments.md](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09428-authoring-amendments.md>).

The 21 retained suites on the final tooling commit were:

| Suite | Result |
| --- | --- |
| tooling_self_tests.py | PASS |
| ledger_delta_self_tests.py | PASS |
| ledger_budget_self_tests.py | PASS |
| resume_self_tests.py | PASS |
| finish_self_tests.py | PASS |
| historical_suite09428.py | PASS |
| uht_reader_self_tests.py | PASS |
| reader_self_tests.py | PASS |
| root_attribution_tests.py | PASS |
| fixture_admission_tests.py | PASS |
| action_reader_tests.py | PASS |
| owner_eq_self_tests.py | PASS |
| fixture_self_tests.py | PASS |
| fixture_session_tests.py | PASS |
| fixture_ey_tests.py | PASS |
| environment_reads.py | PASS |
| disk_pause.py | PASS |
| idle_gate.py | PASS |
| probe_teardown_check.py | PASS |
| check_reflected_headers.py | PASS |
| unit_suites09425.py | PASS |

**Real timeline — 2026-10-30, IST.** [FA5 readings](<D:/IA_Europa/Receipts/09428/idle-precheck-01.jsonl>) contain 105 polls. The final pre-check passed at 02:24:06: 7,416.922 seconds idle, 9.30 GiB physical and 14.88 GiB commit free, GPU 0%, no blockers. Before that, commit headroom was the waiting condition. The FC7 start deadline was 06:30 IST; this failed Smoke ended the run earlier as FC3 requires.

| Stage | Start → end | Result |
| --- | --- | --- |
| Observe-only pre-check | 00:33:44 → 02:24:06 | WAIT, then PASS; no launch before PASS |
| Real BAT, once | 02:24:06 → 02:32:49 | Exit 2; no retry |
| Smoke measured gate | 02:30:00 → 02:30:57 | Three passing polls; last idle 7,828.109 s, physical 9.23 GiB, commit 14.60 GiB, GPU 0% |
| Smoke editor PID 2476 | 02:31:55 → 02:32:05 | 9.547 s; controlled termination, exit 125 |
| EX3 quarantine | 02:32:12 → 02:32:14 | Zero files/moves; Smoke lock remains spent |
| EA_Core | Not started | All recipe paths unused |
| EA_Travel | Not started | All recipe paths unused |

**Console, verbatim.** [Original console](<D:/IA_Europa/Receipts/09428/entrypoint-01-console.log>); SHA-256 `1253d58ac902dd4ff9b56d7696687bf77e28a721f2a189a97f93617ba95fbf98`.

```text
Europa fixture authoring: checking the EW2 arming.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
Smoke: attempt 1 of 1; checking protected files, Content, binaries and routing.
Smoke: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
Smoke: starting the one authorized smoke session; limit 300 seconds. Output is recorded in D:\IA_Europa\Receipts\09428\fixture-attempts\Smoke\01\session.
Smoke: 0 declared partial outputs quarantined by receipt. A retry requires chat re-arm.
Stopped: FC6 UBT build action or unknown mode; only QueryTargets is permitted No further session will be started.
Stopped: Smoke did not pass. No later recipe or automatic retry will start.
```

**Smoke result and stop attribution.** [attempt-result.json](<D:/IA_Europa/Receipts/09428/fixture-attempts/Smoke/01/attempt-result.json>) is STOP. The retained UBT argv is:

```text
"E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms
```

[TargetPlatformManagerModule.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Developer/TargetPlatform/Private/TargetPlatformManagerModule.cpp:996>) launches exactly this SDK-status command; [ValidatePlatformsMode.cs](<E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Modes/ValidatePlatformsMode.cs:35>) checks SDK availability and prints VALID/INVALID status. The recorded command is not evidence of a build. My guard’s `QueryTargets`-only implementation was too narrow. No compiler, linker, UHT, UAT or ShaderCompileWorker child was recorded, and build outputs stayed equal. The original STOP is retained; no gate was changed after the real result.

The kit stopped editor PID 2476, UBT 44428, crash-monitor child 48728 and conhost 41720 by retained identity. The initial crash-monitor PID 39980 had already exited. All five were independently absent at [process closeout](<D:/IA_Europa/Receipts/09428/process-closeout.json>). No foreign process was stopped. `smoke.json` and `probe.jsonl` were never produced, so there is no skipped cleanup row, no `probe_stop` and no clean-exit proof. No PIE evidence appears in the retained log. The Runtime.log ends during engine initialization; native recipe API availability and the launcher inside a completed editor session remain unproved.

**Evidence a–g.** [Collected evidence](<D:/IA_Europa/Receipts/09428/result-evidence.json>) and [assessment](<D:/IA_Europa/Receipts/09428/assessment.json>) retain raw observations and classifications.

| Item | Smoke / EA_Core / EA_Travel |
| --- | --- |
| a. Exit, errors and ensures | Smoke was stopped by the kit, not a recorded crash. Runtime.log has zero error/ensure/fatal candidates; CrashClient.log also has none. Plugin/probe errors: none recorded. Clean probe exit is missing. Core/Travel: no session or logs. |
| b. Outputs and protected files | Zero new/changed/missing Content files; all 1,458 existing files, 41 bound config files, descriptors, .uproject, 110 artifacts and manifest hashes equal. No BuiltData. FC5 managed and Saved deltas are empty. |
| c. A5 recipe evidence | No smoke or authoring receipt. Every authoring item below is unreached; neither recipe ran. |
| d. Uploads | Both editor privacy overrides are present in prepared/runtime argv and Runtime.log:145. No send/POST/upload found in either Smoke log; the forced early stop leaves completed-session/shutdown proof open. Core/Travel: not run. |
| e. EditorRuns | 31124 is unchanged and the sole final marker; no manual deletion. Endpoint checks do not claim every transient marker was observed. |
| f. Ledger | Runtime: 4 port_C_exception, 13 port_D and 1 tooling changes; zero charged C/E/5.1 growth. No 5.1 launch/build/write operation. |
| g. Failed-session quarantine | Smoke: zero declared outputs, zero moves, lock retained, retry_authorized=false. Both recipe states and all their slots remain absent. |

**Outputs.** Paths below are under `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/`.

| Path | Size | SHA-256 |
| --- | --- | --- |
| SM_EA_TwoLod.uasset | Absent | Not applicable |
| EA_Core.umap | Absent | Not applicable |
| EA_Travel.umap | Absent | Not applicable |

**A5 item-by-item status.** The [committed recipe](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09411-fixture-recipes.md>) and [authoring script](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/author_425_fixture.py>) are unchanged. Both recipe columns below mean no runtime authoring occurred; stub tests are separate evidence.

| Recipe item | EA_Core | EA_Travel |
| --- | --- | --- |
| Exact declared packages; no overwrite | No packages | No packages |
| Sphere LOD0 / Cube LOD1, at least two real render LODs | Not reached | Mesh reuse not reached |
| EA_Floor; tagged EA_HideCube, EA_BlinkCube and EA_LOD | Not reached | Not reached |
| Movable light: intensity 5000, radius 1800, shadows | Not reached | Not reached |
| EA_Camera transform and FOV 50 | Not reached | Not reached |
| Unbound post-process: exposure 1/1, no motion blur or AA | Not reached | Not reached |
| GameModeBase, PlayerStart, saved level, actor/target inventory | Not reached | Not reached |
| AUTHORED_NOT_QUALIFIED receipt | Absent | Absent |

**Upload check for every session.** Smoke’s [upload-check.json](<D:/IA_Europa/Receipts/09428/fixture-attempts/Smoke/01/upload-check.json>) and independent check found zero send/POST hits in Runtime.log (43,356 bytes, SHA-256 `d98c9f46024f1351b7f5835eb4373b62218844d9b6aaafa09af844d0ecf28e60`) and CrashClient.log (5,540 bytes, `ac8d969f6d95786b7a9a15ae3cf01107f7ba21d5fd7d026087b9c9fec32584ba`). CrashClient.log:57 names a configured DataRouter URL; it does not record a request. Other candidates are argv, config and monitor lifecycle lines. No crash/ensure report folder was created. This short, interrupted log check is not packet capture or proof of normal shutdown behavior. EA_Core and EA_Travel have no session and no upload proof.

Marker `E:/EpicGames/UE_4.25/Engine/Intermediate/EditorRuns/31124` remains zero bytes, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`, creation/write time `2026-10-26T18:43:59.9006068Z`. [Quarantine receipt](<D:/IA_Europa/Receipts/09428/fixture-attempts/Smoke/01/quarantine-receipt.json>) preserves the spent Smoke authorization. Four of the 18 real paths now exist for Smoke/global state; all 14 recipe paths remain absent. The 09417 and old 09425 spent states were preserved.

**Ledger and foreign activity.** [Ledger rollup](<D:/IA_Europa/Receipts/09428/ledger-summary.json>) contains 7 completed operation summaries and their admission records: no ledger STOP, no new/removed root attribution and no actual EU2 pause. The runtime attempt STOP remains separate and is not erased by these filesystem-ledger passes. Charged growth across completed operations: C 0 bytes, E 0, 5.1 engine 0; below EA4/EG1 caps. The four runtime C rows are one NVIDIA cache file and the three event logs listed below. Protected enumeration has zero access errors; 239 denied OS/tooling metadata paths remain listed. Foreign disk/memory changes and root mtimes were recorded; foreign contents were not opened or processes controlled. No new reparse point was created; retained alias tests used simulated attributes on owned plain paths.

**Canary and command audit.** The fresh launch control was non-vacuous, and recorded child environments contain no blocked name or canary, including the editor’s 92 names. [Final launch-second scan](<D:/IA_Europa/Receipts/09428/canary-final.json>) is **GAP**: 3,259 files, 3,281,540,473 bytes, plus 3,668,124,608 decoded archive bytes; zero value matches, 3 read errors and six passing scanner controls. It covers new/changed files since `2026-10-29T15:54:29.239568Z` under D:/IA_Europa and the run folder, plus exact ledger-attributed C:/E:/engine files. No full EK1 scan was run, per FA6. The following OS-protected files remain unproved; no permission or security workaround was attempted:

| Path | Result |
| --- | --- |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-PushNotification-Platform%4Operational.evtx | Access Denied (5) |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-StorageManagement%4Operational.evtx | Access Denied (5) |
| C:\Windows\System32\winevt\Logs\Security.evtx | Access Denied (5) |

FC9 accepted the earlier run’s OS-protected event-log gaps. These are the current three paths, so the report preserves GAP explicitly rather than claiming they were scanned. All readable files had zero matches. Final report bytes receive their own hash-based canary check.

[EU1/EV1 audit](<D:/IA_Europa/Receipts/09428/command-audit-final.json>) passed through its recorded event boundary: 244 filtered invocations, 1 initial direct read-only deviation, zero direct write-capable commands and zero pause-window violations. One exact wrapper receipt-sharing refusal occurred before its child started; its evidence is retained. Failed inspection commands and the failed documentation parser attempt remain in the audit. The initial read-only deviation established the filtered boundary; later writers used it. No runtime retry occurred.

**Deviations and corrections.** The rehearsal corrections and platform-capacity interruption are detailed above. The first documentation commit guard mistook Git’s exact LF/CRLF warning lines for filenames; it failed before `git add` or `git commit`. The report-only parser then filtered only that warning; its second attempt committed the reviewed seven documentation files. No Git setting was changed. The first report-draft command had a bracket syntax error and wrote no report; its corrected second run passed. Exit hashing preceded the final canary/audit so those checks include its new receipts. Final hash checks and the final canary scan cover the documentation tail; no full P9 re-hash or full EK1 repeat was added. The three denied event logs remain GAP. Most importantly, the real FC6 guard refused a stock validation mode: no fix, re-arm or second Smoke was attempted after that result.

**EN5 applications and limits.** No intent-clause override was used. [EN5](<D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54>) retains hard stops for protected state, secret/canary exposure, disallowed engine writes and machine/security/global settings. This job applies explicit amendments as follows:

| Item and source | Application |
| --- | --- |
| [FC5](<D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:68>); [crash-config purge](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/GenericPlatform/GenericPlatformCrashContext.cpp:909>) | Only the four named managed classes are recorded instead of bound. No general config exception. |
| [FC6](<D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:77>); [target discovery](<E:/EpicGames/UE_4.25/Engine/Source/Developer/DesktopPlatform/Private/DesktopPlatformBase.cpp:882>) | Owned UBT query argv is recorded; builds and changed build outputs remain STOPs. |
| [FC4](<D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:61>); [analytics guard](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:105>) | Process-only source-proven opt-out; uploads remain STOPs. No persistent config/security change. |
| [ET2](<D:/IntrusiveAnomalies/_reviews/094-18-ruling.md:54>) | Engine-managed EditorRuns lifecycle is recorded; marker 31124 is preserved. |
| [EU2](<D:/IntrusiveAnomalies/_reviews/094-19-ruling.md:46>); [FA6](<D:/IntrusiveAnomalies/_reviews/094-25-ruling.md:127>) | Foreign activity is recorded/waited out; a floor breach invokes pause, with no edits in an open pause. |
| [EZ1/EZ2](<D:/IntrusiveAnomalies/_reviews/094-24-ruling.md>) | Root changes require attribution without opening foreign contents; admission between floor and start waits. |
| [FC9](<D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:112>) | Only the previously accepted three OS-protected event-log read gaps may be identified as such; other gaps remain explicit. |

The newly observed SDK-validation path is also documented with [TargetPlatformManagerModule.cpp:996](<E:/EpicGames/UE_4.25/Engine/Source/Developer/TargetPlatform/Private/TargetPlatformManagerModule.cpp:996>) and [ValidatePlatformsMode.cs:35](<E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Modes/ValidatePlatformsMode.cs:35>). EN5 was not used to bypass the failed Smoke or authorize a retry. FC3’s stop remains binding.

**Commits, settings and hashes.** Local commits on `feat/europa-port`:

| Commit | Change |
| --- | --- |
| 88052fd1d18a38263bb76816be6882ed46b65978 | Python 2.7 launcher, privacy, Smoke, managed-file/UBT guards, tests and design |
| c2e73180fdf39f3906ff9a0773ab860834bdf7c0 | Two stand-in mode-tuple corrections; final tested/rehearsed/armed tooling |
| dce872d2668a49087457f2bfd12e5f676af59220 | Documentation and receipts index only; W4 author/committer dates 2026-10-30T02:47:25+05:30 / 2026-10-30T02:47:25+05:30 |

Commits pushed: **none**. Tags: **none**. The final documentation files are CLAUDE.md, docs/architecture.md, docs/gotchas.md, docs/onboarding.md, docs/setup-runbook.md, docs/sessions/2026-10-29-094-28-authoring-fixes.md and docs/europa/09428-authoring-receipts.json. Whitespace and exact staged-file checks passed; the final worktree is clean. No persistent bench lever, machine/security setting or project config was changed. The kit supplied process-only privacy flags, `-IAIBench`, `-EuropaPhaseA`, Python/EditorScriptingUtilities, 1280×720, DDC/UserDir/ShaderWorking/log routing, `-nowrite`, `-SkipCompile` and the 300-second Smoke cap. The owned process tree has ended. Rehearsal-only limits and stand-ins stayed on scratch.

| Manifest | Files | SHA-256 |
| --- | --- | --- |
| 425 | 27 | c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6 |
| baseline51 | 39 | b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37 |
| regate51 | 44 | 3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2 |

All 110 artifact paths, sizes and hashes are in [binary-hashes.tsv](<D:/IA_Europa/Receipts/09428/binary-hashes.tsv>). Every bound EXE/DLL follows; paths are relative to D:/IA_Europa (425 = StylizedParisStreet; B51 = Baseline51/StackOBot; R51 = ReGate51/StackOBot). These are unchanged binaries, not new builds.

| Bound EXE/DLL | SHA-256 |
| --- | --- |
| 425/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe | 6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861 |
| 425/Binaries/Win64/StylizedParisStreet.exe | a2db7a028ebdbfc9d5d74600a35a2951421f8def703ca90f5e307ed460eeb1ac |
| 425/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll | 9458cb952db4c24adc13211118222f1754a7aacaedbc9bd027505a2a382f824f |
| 425/Binaries/Win64/UE4Editor-StylizedParisStreet.dll | 8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | 2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940 |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | 9627b803fdb4e94c4e80678176c7478435c092b1c7f993b3024cc936bb50c36c |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | 0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | 9c9bb82daf01ed85900b6794d75570ea38d447e911285c989153e5ec01700f0a |
| 425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | c3db764fc6566f01ca1f425d3bde0469ca8d1ae88ceb01bfb8f5a82f5e8cfd96 |
| B51/Binaries/Win64/boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| B51/Binaries/Win64/boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| B51/Binaries/Win64/boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| B51/Binaries/Win64/boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| B51/Binaries/Win64/boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| B51/Binaries/Win64/boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| B51/Binaries/Win64/boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| B51/Binaries/Win64/boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| B51/Binaries/Win64/D3D12/D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| B51/Binaries/Win64/D3D12/d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| B51/Binaries/Win64/OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| B51/Binaries/Win64/StackOBot.exe | b6def734b67b3d03636397ec9d43999458ab86e6baa3db9f21382d394fa2d3d0 |
| B51/Binaries/Win64/tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| B51/Binaries/Win64/tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| B51/Binaries/Win64/UnrealEditor-EuropaPhaseAProbe.dll | ac3b1ca44fc45b61cc3a6e4f1fc7271c383d337fa86ab1ba45eddce4e2d78012 |
| B51/Binaries/Win64/UnrealEditor-StackOBot.dll | 4092eb194804229dd2e4e85dec4c50eb96cefd03491b5f7038c084365e43ec19 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyBench.dll | 6a8a770188e44660e0d0ba790d14515b600df498eb657bd24af07245dee211d8 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyCapture.dll | 9ab3578e0cdab25d9cacf93b226d8d54af2acfda6af0631c5da76f3d5eed9d94 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll | 9a41d9a8cb649f0dbd962082ea0b535f6162b2f611146b20d202f2fd457a4b57 |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyInjector.dll | 91d14a4af7711c3563dccb30589d013648d9a23b5a7f33563e67f0d06443974e |
| B51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyShaders.dll | 284c64cd7936ea260e780a1a010916a72312a77acb2282a0d413b4196b584b0d |
| B51/Plugins/CaptureBench/Binaries/Win64/UnrealEditor-CaptureBench.dll | e2ef940d6df3ca2d8075015498da127f32540df20b844d1c23e9ffe1de3ce5d8 |
| R51/Binaries/Win64/boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| R51/Binaries/Win64/boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| R51/Binaries/Win64/boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| R51/Binaries/Win64/boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| R51/Binaries/Win64/boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| R51/Binaries/Win64/boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| R51/Binaries/Win64/boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| R51/Binaries/Win64/boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| R51/Binaries/Win64/D3D12/D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| R51/Binaries/Win64/D3D12/d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| R51/Binaries/Win64/OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| R51/Binaries/Win64/StackOBot-Win64-Shipping.exe | 010ab98e78e1a4a04c4dc1fb3edf3841336b3bc6f4d8a6db5be24841b6845fb4 |
| R51/Binaries/Win64/StackOBot.exe | 5b0fb171a9c65dced68cfd52b8c62540c0ecb29714f30091717b9d055d5f2298 |
| R51/Binaries/Win64/tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| R51/Binaries/Win64/tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| R51/Binaries/Win64/UnrealEditor-EuropaPhaseAProbe.dll | f8cab87d9a3e0bb93a8fc92dd20c22d25851c83051f3cd28a0e97da0c6af1b21 |
| R51/Binaries/Win64/UnrealEditor-StackOBot.dll | 741b766316baa00dac0a63631dd407b7262ce47fb5f3c677f8d135054117cc0f |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyBench.dll | 128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyCapture.dll | 0e14f06a515844ad27174789fe455284c798a64e8fffe3a8bd12f251e4f162e6 |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll | 3e740f3b9bcf76cc060a25e3eb6602e5a34b375c9e336b86bd065dac129a8a80 |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyInjector.dll | 88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5 |
| R51/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyShaders.dll | f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c |
| R51/Plugins/CaptureBench/Binaries/Win64/UnrealEditor-CaptureBench.dll | 89032f62c2ff43f9f55cbce60a3abd8e68378287bf8ec7fad24f3cec0c11d7f6 |

Engine, recorded runtime-tree and harness-interpreter executable/DLL hashes:

| Path | SHA-256 |
| --- | --- |
| C:\Python313\python.exe | 03573716a7fcbd8b7fed8fe0163bbfbd3a852546920d6a666123f307c8dac7d9 |
| C:\Windows\System32\conhost.exe | c86146188b78e484f32cc29e2331689659943b47689a85570342044f499938ac |
| E:\EpicGames\UE_4.25\Engine\Binaries\DotNET\UnrealBuildTool.exe | 07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0 |
| E:\EpicGames\UE_4.25\Engine\Binaries\ThirdParty\Python\Win64\python.exe | 52b4dead44802e263f54881d434f6dec5eab9ff3bd69a9d0c1408065543c3e55 |
| E:\EpicGames\UE_4.25\Engine\Binaries\Win64\CrashReportClientEditor.exe | ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7 |
| E:\EpicGames\UE_4.25\Engine\Binaries\Win64\UE4Editor.exe | 4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387 |
| E:\EpicGames\UE_4.25\Engine\Plugins\Editor\EditorScriptingUtilities\Binaries\Win64\UE4Editor-EditorScriptingUtilities.dll | 39800186398b4f7de8fc5c7b29e954f0d31c9fa3341244c06b25d9cbf00fe5b1 |
| E:\EpicGames\UE_4.25\Engine\Plugins\Experimental\PythonScriptPlugin\Binaries\Win64\UE4Editor-PythonScriptPlugin.dll | b62d70e6c367674cc21ad8d294a63e32788074cd967fa89eba514709b2a91378 |

No binary archive was created. [Complete archive/image inventory](<D:/IA_Europa/Receipts/09428/archive-image-inventory.json>) lists every one of the 135 compressed receipt archive paths and hashes, and the 3 owned synthetic .exe/.dll-named test files. Those filenames do not imply a build or an Unreal launch. Closed rehearsal databases were compressed and removed only by owned-path/hash receipts under `rehearsal/pass-all-2-resume1/closed-ledger-archives.json`, `rehearsal/smoke-fail-2-resume1/closed-ledger-archives.json` and `rehearsal/ubt-build-2-resume1/closed-ledger-archives.json`. Nothing in E:/IA_BuildCache or D:/IA_BankOverflow was deleted or moved.

VERDICT: NEEDS-DECISION | Smoke stopped on stock UBT SDK validation; both recipes remain unstarted and no packages were created.
