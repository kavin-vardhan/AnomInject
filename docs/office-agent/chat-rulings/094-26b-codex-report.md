<!-- Collected by Claude Code (mailbox brief 094-26b) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-30 after Codex exited (thread 01a14334, relay tries 1, exit 0, duration 2,852 s); body below is byte-identical, sha256 6E7EC135B81B0910AD024B7BF72AFAC1208AEF0DA7F472B7BACA598F9E076E99, 36019 bytes -->
**094-26b: NEEDS-DECISION.** FD2 and FD3 are fixed, tested, rehearsed and committed; the kit was re-armed for 09426b with no dry runs. The real Smoke admitted the stock UBT validation child, kept the Python fixture module alive, and exited normally with code 0 after **23.169 seconds**. It found two unavailable 4.25 properties: `PostProcessSettings.override_anti_aliasing_method` and `anti_aliasing_method`. FC3 stopped progression. **EA_Core and EA_Travel never ran; no package was created.** Protected-state checks passed; neither log contains upload evidence and no crash was recorded. The canary scan has four unresolved OS-protected read gaps. Another run requires an authorized compatible recipe correction and fresh arming.

Evidence root: [Receipts/09426b](D:/IA_Europa/Receipts/09426b). [Committed receipts index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426b-authoring-receipts.json) binds 38 immutable receipts. This report records the later audit and documentation tail. Chat’s request and rulings supply authority; review observations were not treated as new instructions.

**Entry and exit proofs.** [Prior-result entry check](D:/IA_Europa/Receipts/09426b/prior-results-entry.json) found no protected-state change, crash or upload in 094-28, satisfying BINDING 5. Both [EA8 entry](D:/IA_Europa/Receipts/09426b/P9/entry/summary.json) and [EA8 exit](D:/IA_Europa/Receipts/09426b/P9/exit/summary.json) passed against v2: 472,553 inventory files, 1,023 EG1-admitted entries and 23,033 required binary/config hashes covering 104,794,989,456 bytes; protected Git state and source manifests equal. P9 v2 stayed SHA-256 `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. m51 remained `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`, with its baseline index/status preserved.

ED1 free space at entry initialization and final verification, in GiB:

| Drive | Entry | Final | Start / warn / floor |
| --- | --- | --- | --- |
| C | 31.04 | 31.04 | 13 / 11 / 10 |
| D | 73.47 | 72.69 | 40 / 30 / 25 |
| E | 291.63 | 291.62 | 15 / 12 / 10 |

[ED3 toolchains](D:/IA_Europa/Receipts/09426b/toolchains.json) were unchanged: VS 2022 17.12.35707.178 and VS 2017 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; SDK 10.0.17763.0 and 10.0.22621.0. No MSVC 14.44, build or toolchain installation. [Final state](D:/IA_Europa/Receipts/09426b/final-state.json) rechecked 1,864 unique bound hashes and Content/config/build snapshots; the worktree is clean.

**Fixes and tests.** [FD2](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:32) admits editor-owned non-build UBT modes without a mode allowlist, while recording argv/read errors. Explicit `-Mode=Build`, target/platform/configuration triples (including quoted, split `-Target` and combined forms), compiler/linker/UHT children, build-output writes and changed bound binaries remain STOPs. The watcher checks size and nanosecond mtime every second and after child return, including same-byte rewrites; existing SHA-256 checks remain. Sources: [BuildMode.cs:45](E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Modes/BuildMode.cs:45), [TargetDescriptor.cs:156](E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Configuration/TargetDescriptor.cs:156), [TargetDescriptor.cs:209](E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Configuration/TargetDescriptor.cs:209), [TargetPlatformManagerModule.cpp:996](E:/EpicGames/UE_4.25/Engine/Source/Developer/TargetPlatform/Private/TargetPlatformManagerModule.cpp:996) and [ValidatePlatformsMode.cs:35](E:/EpicGames/UE_4.25/Engine/Source/Programs/UnrealBuildTool/Modes/ValidatePlatformsMode.cs:35).

[FD3](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:45) sets the measured start gate to **6 GiB free physical / 8 GiB free commit** and the in-session commit stop to **below 4 GiB**. The Waiting line is `Waiting: 8 GiB free commit.` Other in-session observations, the 60-second unreadable-reading stop and disk thresholds remain unchanged. These changes were prescribed before testing, not tuned after measurements. [Design and predictions](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426b-authoring-amendments.md) records the narrow scope and predicted native-property risk.

| Final check | Result | Receipt |
| --- | --- | --- |
| FD2/FD3 cases, including every requested can-fail | 43/43 PASS | [FD summary](D:/IA_Europa/Receipts/09426b/tests/fd-final/summary.json) |
| FC retained suite | 48/48 PASS | [FC](D:/IA_Europa/Receipts/09426b/tests/affected-final/fc/summary.json) |
| EY retained suite | 46/46 PASS | [EY](D:/IA_Europa/Receipts/09426b/tests/affected-final/ey/result.json) |
| Session suite | 57/57 PASS | [Session](D:/IA_Europa/Receipts/09426b/tests/affected-final/session/result.json) |
| Measured gate suite | 50/50 PASS | [Gate](D:/IA_Europa/Receipts/09426b/tests/affected-final/gate.json) |
| Environment-read checks | 19 self-tests PASS; 69 helpers / 17 reads | [Environment](D:/IA_Europa/Receipts/09426b/tests/affected-final/environment.json) |
| Changed-check stand-in rehearsal | 6/6 PASS; independent sibling survives each | [Rehearsal](D:/IA_Europa/Receipts/09426b/rehearsal-summary.json) |

All suites passed. Final reruns followed added split-Target parser coverage before commitment; no failed suite required a third attempt and no gate was weakened after a result. The six stand-in cases were QueryTargets and ValidatePlatforms passing, and Build mode, build triple, output write and commit below 4 GiB stopping only the owned tree. Plain Python stand-ins and synthetic files stayed under `Receipts/09426b/fd-rehearsal-final` and `Temp/09426b/fd-rehearsal/fd-rehearsal-final`. No Unreal or actual compiler was launched by tests. FC2 in-editor scripts, launcher, A5 recipes and privacy implementation were unchanged, so the conditional Python 2.7 harness rerun was not triggered. The native Smoke supplied new launcher/API evidence.

**Arming and live reading.** [Arming receipt](D:/IA_Europa/Receipts/09426b/arming/receipt.json) binds tooling `09e776bfb2467a1429cd469058ee745acb7df33c`, 78 tooling files, 198 host files, 7 engine files, 1,458 Content files, 41 config files and 110 manifest artifacts. The 09428 and older armings refuse. All 18 fresh real paths were absent at arming. [FD5](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:68) was followed: **zero dry-run sessions**. [Exact argv](D:/IA_Europa/Receipts/09426b/arming/exact-argv.json) and [all console lines](D:/IA_Europa/Receipts/09426b/arming/console-catalog.json) are retained.

Real paths are `D:/IA_Europa/Receipts/09426b/fixture-state/{global,Smoke,EA_Core,EA_Travel}`, `fixture-attempts/Smoke/01`, `fixture-attempts/EA_Core/01–03` and `fixture-attempts/EA_Travel/01–03`, with attempt TEMP paths mirrored under `D:/IA_Europa/Temp/09426b/fixture-attempts`. Smoke permits one session capped at 300 seconds; each recipe permits at most one session capped at 1,800 seconds. The three prelaunch slots do not permit three editor sessions. EW2 outputs remain the same three packages.

The [live observe-only reading](D:/IA_Europa/Receipts/09426b/live-gate.json) at 03:45:00 IST recorded 12,270.750 seconds idle, 9.11 GiB physical, 14.47 GiB commit, GPU 0%, and no blocking reason. Eleven launcher-family processes were exempt by ancestry and install folder: launcher 25528; web helpers 17788, 35008, 42640, 43120, 43916; overlays 7080, 20180, 38440, 40848, 43160. No blocker was misread. The Waiting line requested three passing polls. Windows remained on the 10-29 13:16 boot, with the owner-started launcher instance from 18:08; this job started no launcher.

Armed BAT: 161 bytes, SHA-256 `4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41`. Armed JSON: 530248 bytes, SHA-256 `3396fc9823796a09d053b6b8c3bcfe751365a3a5d49b2a7caacd5589b90af67c`. Both remain unchanged after the run. The old 09417 and 09425 spent states were preserved.

**Real timeline — 2026-10-30, IST.** The single FA5 pre-check passed at 03:46:18 after 12,348.531 seconds idle, with 8.92 GiB physical, 14.46 GiB commit, GPU 0% and no blockers. The window ended at 07:30; failed Smoke ended runtime work earlier.

| Stage | Start → end | Outcome |
| --- | --- | --- |
| Real BAT, once | 03:46:18 → 03:55:20 | Exit 2; no retry |
| Smoke measured gate | 03:52:06 → 03:53:04 | Three passing polls; no blocking reasons; 62.031 s elapsed |
| Smoke editor PID 29516 | 03:54:04 → 03:54:27 | 23.168696 s; normal exit 0 |
| EX3 quarantine | 03:54:35 → 03:54:38 | Zero outputs/moves; lock remains spent |
| EA_Core | Not started | No recipe state or attempt used |
| EA_Travel | Not started | No recipe state or attempt used |

**Console, verbatim.** [Original console](D:/IA_Europa/Receipts/09426b/entrypoint-01-console.log); SHA-256 `0ed23c9192aa78433bf0866ccb633deec5008543004602ac49928bd7a374269e`.

```text
Europa fixture authoring: checking the EW2 arming.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
Smoke: attempt 1 of 1; checking protected files, Content, binaries and routing.
Smoke: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
Smoke: starting the one authorized smoke session; limit 300 seconds. Output is recorded in D:\IA_Europa\Receipts\09426b\fixture-attempts\Smoke\01\session.
Smoke: 0 declared partial outputs quarantined by receipt. A retry requires chat re-arm.
Stopped: Author script did not report the requested recipe complete. No further session will be started.
Stopped: Smoke did not pass. No later recipe or automatic retry will start.
```

**Smoke and recipe evidence a–g.** [smoke.json](D:/IA_Europa/Receipts/09426b/fixture-attempts/Smoke/01/smoke.json) reports `module_alive=true`, 51/53 checks present, zero creates/loads/saves and the two missing properties. Twenty-six native callable checks could not determine arity through Python 2.7 inspection; their binding documentation is retained, not claimed as full call-execution proof. The unchanged [authoring script:88](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/author_425_fixture.py:88) sets the missing properties. This corroborates the review’s O1 observation; it was not used as authority to alter the accepted recipe.

| Evidence | Result and receipt |
| --- | --- |
| a. Clean exit / errors / ensures | Smoke: [probe.jsonl](D:/IA_Europa/Receipts/09426b/fixture-attempts/Smoke/01/session/probe/probe.jsonl) has 25 rows, exactly one skipped world cleanup and one probe_stop, root release skipped, no PIE. Runtime.log:664–665 records QUIT_EDITOR / UUnrealEdEngine::CloseEditor. Exit 0, no forced termination. The only error-pattern candidate is line 636 reporting 0 errors and 0 warnings; engine informational, not an error/ensure. Plugin/probe errors and actual ensures: zero. Core/Travel: no session. |
| b. Outputs / existing protected files | All 1,458 Content files and 41 bound config files, descriptors, .uproject, 110 artifacts and manifests unchanged. No BuiltData or undeclared package. [Collected comparisons](D:/IA_Europa/Receipts/09426b/result-evidence.json). |
| c. A5 authored content | No authoring receipt; every recipe operation below is unreached. Smoke inspects bindings only. |
| d. No uploads | Both required editor privacy overrides present in prepared and recorded runtime argv. Smoke Runtime.log and CrashClient.log checks pass with no send/POST/upload evidence, through normal shutdown. No crash/ensure report folder. Core/Travel: no session to check. |
| e. EditorRuns | 31124 remains the sole final zero-byte marker with unchanged creation/write time and empty SHA-256; no manual deletion. Endpoint checks do not claim observation of every transient marker. |
| f. Ledger / FC5 | C/E/5.1 classes retained; no charged exception growth. Only host managed delta is Saved/Autosaves/PackageRestoreData.json mtime, unchanged size/hash. Build outputs unchanged. |
| g. Failed session | Smoke [quarantine receipt](D:/IA_Europa/Receipts/09426b/fixture-attempts/Smoke/01/quarantine-receipt.json): zero declared outputs, zero moves, retry_authorized=false, launch lock retained. Both recipe states and all their slots remain absent. |

Outputs under `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/`:

| Package | Size | SHA-256 |
| --- | --- | --- |
| SM_EA_TwoLod.uasset | Absent | Not applicable |
| EA_Core.umap | Absent | Not applicable |
| EA_Travel.umap | Absent | Not applicable |

A5 item-by-item status against the unchanged [committed recipe](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09411-fixture-recipes.md):

| Item | EA_Core | EA_Travel |
| --- | --- | --- |
| Declared package names / no overwrite | No packages | No packages |
| Sphere LOD0 / Cube LOD1; at least two render LODs | Not reached | Mesh reuse not reached |
| EA_Floor; tagged EA_HideCube, EA_BlinkCube, EA_LOD | Not reached | Not reached |
| Movable light, intensity 5000 / radius 1800 / shadows | Not reached | Not reached |
| Camera transform / FOV 50 | Not reached | Not reached |
| Unbound post-process: exposure 1/1, no motion blur or AA | Not reached | Not reached |
| GameModeBase / PlayerStart / level save / actor inventory | Not reached | Not reached |
| AUTHORED_NOT_QUALIFIED receipt | Absent | Absent |

**Uploads and UBT argv.** [Upload check](D:/IA_Europa/Receipts/09426b/fixture-attempts/Smoke/01/upload-check.json) and the independent evidence check found no uploads in either completed Smoke log. Runtime.log: 106067 bytes, SHA-256 `d9768935fa5a01ff686db29bb3f40587fdd3f5fdc4a5b0bce8a9f81fa95356ce`; CrashClient.log: 6643 bytes, SHA-256 `4ca05e5b879e5b5c88c30287bd54d538493d8d5a0199aa9fffcbf1489073b68b`. CrashClient.log:57 is a configured DataRouterUrl, not a request; lines 64–77 record editor exit 0, NoSessionFound, NoTempCrash and normal monitor shutdown. This is log evidence, not network packet capture. EA_Core/EA_Travel have no logs or upload claim.

The sole real UBT child was PID 27872, owned by editor 29516:

```text
"E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms
```

It completed without triggering the guard. No compiler, linker, UHT, UAT or ShaderCompileWorker was recorded. Editor 29516, crash monitors 27784 and 3360, UBT 27872 and conhost 45764 all ended; [identity-based closeout](D:/IA_Europa/Receipts/09426b/process-closeout.json) verified all five. One editor argv read became unavailable during exit; the earlier complete argv remained recorded. No UBT argv was missing and no foreign process was stopped.

FC4’s unchanged command-line overrides are `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False` and `-ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False`. Sources: [EngineAnalytics.cpp:105](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:105), [AnalyticsPrivacySettings.h:18](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Classes/Analytics/AnalyticsPrivacySettings.h:18), and the full source chain in [accepted FC4 design](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09428-authoring-amendments.md). Crash-monitor children inherit the engine’s privacy context; their argv/logs are retained rather than asserting that the editor switches appear literally in every child argv.

Marker `E:/EpicGames/UE_4.25/Engine/Intermediate/EditorRuns/31124` remains 0 bytes, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`, creation/write `2026-10-26T18:43:59.9006068Z`. Four of 18 armed paths now exist for Smoke/global state; all 14 recipe paths remain absent. No retry followed Smoke’s STOP.

**Ledger and foreign activity.** [Final ledger summary](D:/IA_Europa/Receipts/09426b/ledger-summary.json) records 6 completed operations, no ledger/admission STOP, no new or removed drive-root attribution and no actual EU2 pause. Runtime classification: 17 port_C_exception, 47 port_D and 3 tooling changes. The 17 C rows are 11 crash-client config files, one NVIDIA cache, one Unreal XML config cache and four Windows event logs; charged growth is zero. Total charged C/E/5.1-engine growth: **0 / 0 / 0 bytes**, below EA4/EG1 caps. The runtime Smoke STOP remains separate from passing filesystem ledgers. Protected enumeration had zero access errors; 239 OS/tooling metadata access errors remained explicitly listed. Foreign root timestamp/log changes were recorded; foreign contents were not opened. No 5.1 launch/build or new reparse point occurred.

**Canary and command audit.** The fresh launch canary control was non-vacuous. Every filtered boundary removed its name/value and blocked identifiers; the editor’s 92 recorded environment names contain no forbidden name. [Final launch-second scan](D:/IA_Europa/Receipts/09426b/canary-final.json) is **GAP**: 678 readable files / 997,202,458 bytes plus 407,902,962 decoded archive bytes, zero matches and six passing scanner controls. It covers new/changed files since `2026-10-29T21:56:57.678170+00:00` under D:/IA_Europa and this run folder, plus exact ledger-attributed C/E/engine outputs. No full EK1 scan was run, per FA6. The following files could not be read:

| Path | Result |
| --- | --- |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-Audio%4PlaybackManager.evtx | Access denied (5) |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-GroupPolicy%4Operational.evtx | Access denied (5) |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-PushNotification-Platform%4Operational.evtx | Access denied (5) |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-Store%4Operational.evtx | Access denied (5) |

The scanner’s failed assertion is preserved. FC9 accepted earlier specific event-log gaps; this run has four, three different from that accepted set. Their contents remain unproved and require review. GAP is not converted to PASS. No permission, security or access workaround was attempted. The final report bytes receive a separate hash-based canary-value check.

[EU1/EV1 command audit](D:/IA_Europa/Receipts/09426b/command-audit-final.json) passed through its recorded event boundary: 93 filtered invocations, 1 direct read-only deviation, 1 parameter-binding refusal before a child, zero direct write-capable commands, 25 recorded file-edit events and zero pause-window violations. The finalizer itself is included as a filtered invocation. No edits occurred during an actual pause.

**Deviations and corrections.** The initial direct read-only inspection established the filtered boundary and attempted to read a missing prior helper; it wrote nothing. A wrapped `rg -e` call was rejected by PowerShell parameter binding before any child; subsequent positional-pattern search worked. A wrapped regex search failed without writes. The initial entry-only scope used the new memory values before the old validator accepted them; it refused before the entry ledger/proof, then the non-runtime entry scope used its existing policy. Runtime arming uses FD3. The first edit-driver call omitted the disk-check argument and failed before edits; the corrected call passed. An event-audit input reader first used the wrong text encoding and failed read-only, then passed with UTF-8. These setup/inspection failures are retained; they did not consume extra runtime attempts. Exit hashing preceded the final canary/audit to include its receipts. Documentation and final hashes cover the later reporting tail. No in-editor script, A5 recipe or gate changed after the real Smoke result. The two missing properties and four canary read gaps remain unresolved.

**EN5 applications, with sources.** No intent-clause override was used to bypass a failure. [EN5:54–72](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54) requires no blocked names/secrets, no loss of base variables, no writes outside allowed places and no protected-state change. The environment contracts, owned-scratch receipts and entry/exit proofs cover those requirements. Its hard stops remain subject only to explicit amendments below:

| Item / source | Application |
| --- | --- |
| [FD2](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:32); [stock SDK validation](E:/EpicGames/UE_4.25/Engine/Source/Developer/TargetPlatform/Private/TargetPlatformManagerModule.cpp:996) | Editor-owned non-build UBT admitted; argv retained. Build actions/children/output writes remain STOPs. |
| [FD3](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:45) | Only the prescribed 8/4 GiB commit lines changed; physical 6 unchanged. |
| [FC5](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:68); [crash-config lifecycle](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/GenericPlatform/GenericPlatformCrashContext.cpp:909) | Only four named engine-managed classes are recorded instead of bound; no general Config exception. Autosave timestamp recorded. |
| [FC4](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:61); [analytics source](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:105) | Process-only privacy overrides; uploads remain STOPs. |
| [ET2](D:/IntrusiveAnomalies/_reviews/094-18-ruling.md:54) | EditorRuns lifecycle recorded; 31124 preserved. |
| [EU1/EU2](D:/IntrusiveAnomalies/_reviews/094-19-ruling.md:30); [EV1/EV3](D:/IntrusiveAnomalies/_reviews/094-20-ruling.md) | Read-only deviation recorded; all writers filtered; no pause-window edits; D thresholds retained. |
| [EY4](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md); [EZ1/EZ2](D:/IntrusiveAnomalies/_reviews/094-24-ruling.md) | E thresholds and admission wait retained; root attribution without foreign-file reads. No root additions/removals this job. |
| [FA6](D:/IntrusiveAnomalies/_reviews/094-25-ruling.md:110) | Foreign activity recorded, not used to end the job; milestone full entry/exit proofs and changed-file canary only. |
| [FC9](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:112) | Four current denied event logs remain GAP; no extension of the prior acceptance. |
| [FC3](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:46); [FD4/FD5](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:54) | Only kit Smoke launched after idle gates; failed Smoke ended runtime; no recipes or dry runs. |
| EN5 hard stops | No P9/protected difference, disallowed engine write, discovered secret exposure, machine/security/global setting change or actual floor breach. This was an explicitly authorized launch job. No security settings changed. |

**Commits and settings.** Local commits on `feat/europa-port`:

| Commit | Change / W4 actual time |
| --- | --- |
| 09e776bfb2467a1429cd469058ee745acb7df33c | FD2/FD3 tooling, tests and design; 2026-10-30 03:43:29 +05:30 |
| c1ef76b7c5f16bb23478e478ad3704e333f81402 | Receipt index and documentation only; 2026-10-30T04:05:51+05:30 / 2026-10-30T04:05:51+05:30 |

Commits pushed: **none**. Tags: **none**. Final documentation updates cover CLAUDE.md, architecture, gotchas G471, onboarding, setup-runbook, the session journal and receipts index. Whitespace/exact-stage checks passed and the worktree is clean. No persistent bench lever, project Config, machine or security setting was changed. FD3’s committed kit limits remain as requested. The one editor used process-only privacy flags, `-IAIBench`, `-EuropaPhaseA`, Python/EditorScriptingUtilities, 1280×720, routed UserDir/DDC/ShaderWorking/logs, `-nowrite`, `-SkipCompile` and a 300-second Smoke cap. Those processes have ended. Test-only synthetic readings/deadlines stayed on scratch; no Shipping behavior changed.

Manifest hashes, unchanged; the two 5.1 records remain frozen:

| Manifest | Files | SHA-256 |
| --- | --- | --- |
| 425 | 27 | c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6 |
| baseline51 | 39 | b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37 |
| regate51 | 44 | 3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2 |

All 110 artifact paths, sizes and hashes are in [binary-hashes.tsv](D:/IA_Europa/Receipts/09426b/binary-hashes.tsv). Every bound EXE/DLL is listed below. Prefixes resolve under `D:/IA_Europa`: **425 = StylizedParisStreet**, **B51 = Baseline51/StackOBot**, **R51 = ReGate51/StackOBot**. These are unchanged records, not new builds.

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

Engine, recorded runtime-tree and interpreter hashes (the engine Python interpreter was bound but not launched separately this job):

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

No binary archive or compiled binary was created. [Complete archive/image inventory](D:/IA_Europa/Receipts/09426b/archive-image-inventory.json) binds all compressed receipt archives and synthetic files. The eight test files below are harmless text fixtures with DLL suffixes, not compiled binaries:

| Owned test path | SHA-256 |
| --- | --- |
| D:\IA_Europa\Receipts\09426b\tests\fd-attempt1\managed\Binaries\Win64\test.dll | e35b2252583cefc6c764da00830c86b8d929c925703a88ebcb622c8f0caf24f9 |
| D:\IA_Europa\Receipts\09426b\tests\fd-attempt1\write-36\Binaries\Win64\test.dll | a20b5aefd656f49207afc83c0ff34fc7b9031bca95b2ce78522bc741429c9936 |
| D:\IA_Europa\Receipts\09426b\tests\fd-attempt1\write-39\Plugins\AnomalyInjector\Binaries\Win64\test.dll | a20b5aefd656f49207afc83c0ff34fc7b9031bca95b2ce78522bc741429c9936 |
| D:\IA_Europa\Receipts\09426b\tests\fd-final\managed\Binaries\Win64\test.dll | e35b2252583cefc6c764da00830c86b8d929c925703a88ebcb622c8f0caf24f9 |
| D:\IA_Europa\Receipts\09426b\tests\fd-final\write-37\Binaries\Win64\test.dll | a20b5aefd656f49207afc83c0ff34fc7b9031bca95b2ce78522bc741429c9936 |
| D:\IA_Europa\Receipts\09426b\tests\fd-final\write-40\Plugins\AnomalyInjector\Binaries\Win64\test.dll | a20b5aefd656f49207afc83c0ff34fc7b9031bca95b2ce78522bc741429c9936 |
| D:\IA_Europa\Temp\09426b\fd-rehearsal\fd-rehearsal-final\write\host\Binaries\Win64\synthetic.dll | 2416597dbc9ee692881a97856d8e2ba6205c9e860e375457a595f6b885279766 |
| D:\IA_Europa\Temp\09426b\fd-rehearsal\write\host\Binaries\Win64\synthetic.dll | 2416597dbc9ee692881a97856d8e2ba6205c9e860e375457a595f6b885279766 |

Every compressed archive path follows, relative to `D:/IA_Europa/Receipts/09426b/`; SHA-256 values are in the inventory linked above:

| Archive path |
| --- |
| fixture-attempts/Smoke/01/ledger/fixture-preflight/after-delta.jsonl.gz |
| fixture-attempts/Smoke/01/ledger/fixture-preflight/before-delta.jsonl.gz |
| fixture-attempts/Smoke/01/ledger/fixture-preflight/classified-changes.jsonl.gz |
| fixture-attempts/Smoke/01/ledger/fixture-runtime/after-delta.jsonl.gz |
| fixture-attempts/Smoke/01/ledger/fixture-runtime/before-delta.jsonl.gz |
| fixture-attempts/Smoke/01/ledger/fixture-runtime/classified-changes.jsonl.gz |
| fixture-attempts/Smoke/01/ledger/initial.sqlite.gz |
| ledger/closeout/after-delta.jsonl.gz |
| ledger/closeout/before-delta.jsonl.gz |
| ledger/closeout/classified-changes.jsonl.gz |
| ledger/documentation/after-delta.jsonl.gz |
| ledger/documentation/before-delta.jsonl.gz |
| ledger/documentation/classified-changes.jsonl.gz |
| ledger/entry/after-delta.jsonl.gz |
| ledger/entry/before-delta.jsonl.gz |
| ledger/entry/classified-changes.jsonl.gz |
| ledger/initial.sqlite.gz |
| ledger/tooling/after-delta.jsonl.gz |
| ledger/tooling/before-delta.jsonl.gz |
| ledger/tooling/classified-changes.jsonl.gz |

Nothing in E:/IA_BuildCache or D:/IA_BankOverflow was deleted or moved. No cleanup of foreign files was attempted.

VERDICT: NEEDS-DECISION | FD2/FD3 passed; Smoke found two unavailable 4.25 properties, so both recipes remain unstarted; four canary-read gaps remain.
