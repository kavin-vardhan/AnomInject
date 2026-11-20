<!-- Collected by Claude Code (mailbox brief 094-26c) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-30 after Codex exited (thread 01a14334, relay tries 1, exit 0, duration 3,321 s); body below is byte-identical, sha256 D7EA2D22B5D7CFD26D0F961BE4C41653FCA86F229AF4A265989C835BAA2F8E04, 40598 bytes -->
**094-26c: RED.** FE1 and FE2 are implemented and committed; the 09426c kit was re-armed without dry runs. The real Smoke passed **51/51 bindings**. EA_Core authored its two packages and exited normally, but Unreal added `Saved/Config/WorldState/1202994220.json`, which is outside FC5’s admitted classes. The kit applied the Config STOP, quarantined both packages and refused EA_Travel. **A6 remains incomplete. No retry followed.** Both editors exited 0; no crash or upload evidence was found.

Evidence: [Receipts/09426c](D:/IA_Europa/Receipts/09426c). The [committed receipt index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426c-authoring-receipts.json) binds 59 immutable receipts/files. This report adds the final documentation, audit and canary tail. Chat’s rulings supplied authority; Code’s reviews were treated as observations.

**Entry and exit proofs.** [Prior-results check](D:/IA_Europa/Receipts/09426c/prior-results-entry.json) found no protected-state change, crash or upload in 094-26b. ORDER 1’s “094-28” reference was read with BINDING 5’s explicit 094-26b results. [EA8 entry](D:/IA_Europa/Receipts/09426c/P9/entry/summary.json) and [EA8 exit](D:/IA_Europa/Receipts/09426c/P9/exit/summary.json) passed against v2: 472,553 inventory files, 1,023 EG1 entries and 23,033 binary/config hashes covering 104,794,989,456 bytes; protected Git and source manifests equal. These P9 proofs do **not** waive the newly created host Config path. P9 v2 remained SHA-256 `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. m51 remained `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`, with its baseline index and 15 status entries preserved.

Free space, GiB:

| Drive | Entry | Final | Start / warn / floor |
| --- | --- | --- | --- |
| C | 30.98 | 31.00 | 13 / 11 / 10 |
| D | 72.69 | 71.60 | 40 / 30 / 25 |
| E | 291.62 | 291.59 | 15 / 12 / 10 |

[ED3 toolchains](D:/IA_Europa/Receipts/09426c/toolchains.json): VS 2022 17.12.35707.178; VS 2017 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; SDK 10.0.17763.0 and 10.0.22621.0. No 14.44, installation or build. [Final state](D:/IA_Europa/Receipts/09426c/final-state.json) verifies 1869 original bound hashes, unchanged kit and preserved quarantine. It explicitly records the Config STOP, rather than an overall protected-state pass.

**Recipe change and tests.** [FE1](D:/IntrusiveAnomalies/_reviews/094-26b-ruling.md) authorized removal of exactly `override_anti_aliasing_method` and `anti_aliasing_method` from [the recipe post-process loop](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/author_425_fixture.py:85). The other six exposure/motion-blur entries and all other authoring behavior remain unchanged. The smoke checker now inspects the 51 retained bindings; all were present in the prior native receipt and in this run. `AntiAliasingMethod.AAM_NONE` itself still exists and remains checked. AA control for capture is deferred to A7/094-27, not added to this recipe.

The [recipe document](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09411-fixture-recipes.md), [A5 fixture manifest](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426c-a5-fixture-manifest.json) and [dated prediction addendum](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426c-authoring-amendments.md) were re-bound and committed before the real run. [Re-binding receipt](D:/IA_Europa/Receipts/09426c/recipe-rebinding.json) records the prior and new hashes. The binary manifests were not edited.

| Binding | SHA-256 |
| --- | --- |
| Recipe | f55aff4ddc6e239907d3b278ad2bb0a032f79fa24e502d32fa32ac84297a43dd |
| Recipe document | 7050e652be5899d72dfab9baa4b9af044b754f1e864584d67e1110b70a5e761d |
| A5 fixture manifest | 686cbd56eea0afa663c6113832b7dcfaad5f32d33ee6088e51f15b48261d19ac |
| Smoke checker | d27a3a8ea6cda131a4a8ba293ab4541a3c55c864546369be6005d8e9905f02cc |

| Check | Result | Receipt |
| --- | --- | --- |
| Actual engine Python 2.7.14 harness | PASS: three scripts; Smoke 0, Core 90, Travel 87 recorded calls; zero asset writes; runpy-dict and missing-API mutants fail | [Python 2.7](D:/IA_Europa/Receipts/09426c/python27-attempt1/summary.json) |
| FE recipe/bindings/event-log checks | 14 PASS, including seven FE2 cases | [Affected suites](D:/IA_Europa/Receipts/09426c/tests/affected-attempt2/summary.json) |
| FC / EY suites | 49 / 46 PASS | [Final suite binding](D:/IA_Europa/Receipts/09426c/final-suites.json) |
| Session / measured gate suites | 57 / 50 PASS | [Affected suites](D:/IA_Europa/Receipts/09426c/tests/affected-attempt2/summary.json) |
| Environment-read checks | 19 self-tests PASS; 71 helpers, 17 reads | [Affected suites](D:/IA_Europa/Receipts/09426c/tests/affected-attempt2/summary.json) |
| Real Smoke | 51/51 present; module alive; creates/loads/saves all 0 | [smoke.json](D:/IA_Europa/Receipts/09426c/fixture-attempts/Smoke/01/smoke.json) |

D2: the first FE suite attempt failed because the new test passed a generator to `verify_files`, which requires a sized list. The test was corrected to a list; the second affected-suite attempt passed. The recipe and Python 2.7 harness passed their first attempt. No third attempt or post-result gate relaxation occurred. FD2, FD3 and the FC2 retained-module launcher were hash-verified unchanged. No stand-in rehearsal or session dry run was added; FD5 applied. The recording stub checks syntax/imports/full call order, while native Smoke verifies availability; 26 native callable signatures could not expose arity under Python 2.7, so Smoke alone is not proof of executing every API.

FE2 is implemented by [canary_eventlogs.py](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/canary_eventlogs.py): only access-denied `.evtx` files directly under `C:/Windows/System32/winevt/Logs` qualify. Readable logs are scanned. Other folders, extensions, path traversal, sharing errors and readable files are not excluded. The denied count/folder is recorded below.

**Arming and live reading.** [Arming receipt](D:/IA_Europa/Receipts/09426c/arming/receipt.json) binds tooling `9f5ce85a562fa162d3570aef74292cb48d50b03c`, 83 tooling/document files, 198 host files, 7 engine files, 1,458 Content files, 41 config files and 110 binary-manifest artifacts. The 09426b, 09428, 09425, 09424, 09423 and 09422 armings refuse. All 18 fresh real paths were absent at arming. **Zero dry-run sessions.** [Exact argv](D:/IA_Europa/Receipts/09426c/arming/exact-argv.json) and [console catalog](D:/IA_Europa/Receipts/09426c/arming/console-catalog.json) are retained.

Slots: `D:/IA_Europa/Receipts/09426c/fixture-state/{global,Smoke,EA_Core,EA_Travel}`; `fixture-attempts/Smoke/01`; `fixture-attempts/EA_Core/01–03`; `fixture-attempts/EA_Travel/01–03`. Attempt TEMP paths mirror these under `D:/IA_Europa/Temp/09426c/fixture-attempts`. Smoke permits one 300-second session; each recipe permits one 1,800-second session. Three prelaunch slots do not permit three editor sessions. Seven paths now exist for global/Smoke/Core state and slot 01; the other eleven, including every Travel path, remain absent.

The [observe-only reading](D:/IA_Europa/Receipts/09426c/live-gate.json) at 04:54:13 IST recorded 16423.562 seconds idle, 8.41 GiB physical, 14.47 GiB commit and GPU 5%, with no blocking reason. Ten launcher-family processes were exempt by ancestry and install folder, including five overlays and four web helpers: 7080, 20180, 25528, 35008, 36968, 38440, 40848, 43120, 43160, 43916. No blocker was misread. The launcher remained the owner-started PID 25528 from 10-29 18:08; this job did not start it. FD3 remains 6 GiB physical / 8 GiB commit at admission and a 4 GiB in-session commit stop.

Armed BAT: 161 bytes, SHA-256 `4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41`. Armed JSON: 531903 bytes, SHA-256 `f846d37ffb459819d5298254fd43cf39a8330ba382121f2e1d3d518d449190c9`. Both remain unchanged after the run. The old spent owner and recipe kits were preserved.

**Real timeline — 2026-10-30, IST.** The FE4 deadline was 07:45. Runtime ended earlier at the Core STOP. FA5 pre-checks passed before the BAT and before each session: 16,488.531 / 16,492.703 / 17,054.375 seconds idle, respectively, with all memory/GPU/process conditions passing. Each measured gate then obtained three quiet polls.

| Stage | Start → end | Result |
| --- | --- | --- |
| Real BAT, once | 04:55:18 → 05:14:09 | Exit 2; no retry |
| Smoke measured gate | 05:01:06 → 05:02:04 | 61.895 s; three passing polls |
| Smoke editor PID 34352 | 05:03:05 → 05:03:35 | 29.310721 s; normal exit 0 |
| EA_Core measured gate | 05:10:56 → 05:11:54 | 62.046 s; three passing polls |
| EA_Core editor PID 43724 | 05:12:55 → 05:13:08 | 12.804029 s; normal exit 0 |
| EX3 Core quarantine | 05:13:24 → 05:13:26 | Two packages moved; retry forbidden |
| EA_Travel | Not started | No slot or state used |

**Console, verbatim.** [Original console](D:/IA_Europa/Receipts/09426c/entrypoint-01-console.log), SHA-256 `50cae564a8c6927b7e024684b1a5c444bdcc0a8d3e426b62ef4fc545e2b77378`.

```text
Europa fixture authoring: checking the EW2 arming.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
Smoke: attempt 1 of 1; checking protected files, Content, binaries and routing.
Smoke: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
Smoke: starting the one authorized smoke session; limit 300 seconds. Output is recorded in D:\IA_Europa\Receipts\09426c\fixture-attempts\Smoke\01\session.
Smoke: clean exit and output checks passed; this recipe is now spent.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
EA_Core: attempt 1 of 3; checking protected files, Content, binaries and routing.
EA_Core: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
EA_Core: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in D:\IA_Europa\Receipts\09426c\fixture-attempts\EA_Core\01\session.
EA_Core: 2 declared partial outputs quarantined by receipt. A retry requires chat re-arm.
Stopped: Config or descriptor changed during the session. No further session will be started.
Stopped: EA_Core did not pass. No later recipe or automatic retry will start.
```

**Smoke and recipe evidence a–g.** [Smoke receipt](D:/IA_Europa/Receipts/09426c/fixture-attempts/Smoke/01/smoke.json) confirms the retained module and 51 available bindings without creating, loading or saving assets. [Core authoring receipt](D:/IA_Europa/Receipts/09426c/fixture-attempts/EA_Core/01/authoring.json) reports `AUTHORED_NOT_QUALIFIED`, two LODs, eight actor labels and four anomaly-target mappings. This authoring receipt does not override Core’s failed postflight.

| Evidence | Smoke / EA_Core / EA_Travel |
| --- | --- |
| a. Clean exit | Smoke: 25 probe rows, one skipped cleanup, exactly one probe_stop. Core: 37 rows, two skipped cleanup rows, exactly one probe_stop. Both root releases skipped, no PIE and normal exit 0; no forced termination. [Smoke probe](D:/IA_Europa/Receipts/09426c/fixture-attempts/Smoke/01/session/probe/probe.jsonl); [Core probe](D:/IA_Europa/Receipts/09426c/fixture-attempts/EA_Core/01/session/probe/probe.jsonl). Travel not started. |
| b. Outputs and protected state | Core created exactly its mesh and map under Content, then both were quarantined. All 1,458 pre-existing Content files and existing config, descriptor/.uproject, binary and manifest hashes remain equal. No BuiltData or extra Content package. One new bound Saved/Config/WorldState JSON caused STOP; Saved/Backup addition recorded separately. [Comparisons](D:/IA_Europa/Receipts/09426c/result-evidence.json). |
| c. Authored content | Core completed the committed recipe; item-by-item limits below. Smoke performed binding inspection only. Travel unreached. |
| d. Uploads | Both Runtime/CrashClient log pairs pass the upload check; required editor privacy argv present. No crash/ensure report folder found. Travel has no session/log claim. |
| e. EditorRuns | 31124 is the sole final marker, unchanged size/hash/times. No manual deletion; endpoint checks do not claim every transient marker was observed. |
| f. Ledger / FC5 | C/E/5.1 classes and managed deltas retained. Bound build contents unchanged; nine zero-byte Intermediate config files have mtime-only deltas. Details below. |
| g. Failed session | Core [quarantine receipt](D:/IA_Europa/Receipts/09426c/fixture-attempts/EA_Core/01/quarantine-receipt.json) binds both moved packages, absent source paths and retry_authorized=false. Smoke passed; Travel unused. |

Every error/ensure-pattern candidate in Runtime.log is listed here; both are engine informational summaries, not errors or ensures. Actual engine/host errors, plugin/probe errors and ensures: zero.

| Session / line | Verbatim candidate | Classification |
| --- | --- | --- |
| Smoke / 636 | [2026.10.29-23.33.30:883][  0]MapCheck: Map check complete: 0 Error(s), 0 Warning(s), took 0.126ms to complete. | Engine; zero-error map-check summary |
| EA_Core / 636 | [2026.10.29-23.43.03:681][  0]MapCheck: Map check complete: 0 Error(s), 0 Warning(s), took 0.218ms to complete. | Engine; zero-error map-check summary |

Output paths below are under `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/`. The two hashes describe quarantined files, **not accepted outputs remaining in Content**.

| Package | Final disposition | Bytes | SHA-256 |
| --- | --- | --- | --- |
| SM_EA_TwoLod.uasset | Quarantined | 137553 | 090e5f049b834cf103233be5af06421e5c984ac06688814b2367a92af06f8327 |
| EA_Core.umap | Quarantined | 21114 | b786610b6e9787cffa1a3aae4898af7852aa5690aaca50ebaf426d673c08df3c |
| EA_Travel.umap | Never created | — | — |

Quarantine root: [EA_Core/01/quarantine](D:/IA_Europa/Receipts/09426c/fixture-attempts/EA_Core/01/quarantine). No package was restored or reopened.

A5 manifest comparison, item by item:

| Committed item | EA_Core evidence | EA_Travel |
| --- | --- | --- |
| Owned physical project only; no overwrite | Owned-project/no-overwrite guards completed | Not reached |
| Engine sphere LOD0 / cube LOD1; two LODs | Native receipt: LOD count 2; mesh saved | Not reached |
| Floor (700,0,-50), scale (18,18,1) | Bound call sequence completed; actor inventory recorded; no independent saved-property readback | Not reached |
| Hide (600,-260,110), blink (600,260,110), scales (2,2,2) | Bound call sequence completed; actor inventory recorded; no independent saved-property readback | Not reached |
| LOD actor (800,0,160), scale (3,3,3) | Bound call sequence completed; actor inventory recorded; no independent saved-property readback | Not reached |
| Movable point light (450,0,650), intensity 5000, radius 1800, shadows | Bound call sequence completed; actor inventory recorded; no independent saved-property readback | Not reached |
| Camera (-550,0,480), rotation (-12,0,0), FOV 50 | Bound call sequence completed; actor inventory recorded; no independent saved-property readback | Not reached |
| Unbound post-process volume; fixed exposure and motion blur off | Bound call sequence completed; actor inventory recorded; no independent saved-property readback | Not reached |
| GameModeBase; PlayerStart (-550,0,500) | Bound call sequence completed; actor inventory recorded; no independent saved-property readback | Not reached |
| Save only new declared packages; AUTHORED_NOT_QUALIFIED; no PIE | Receipt completed; two packages later quarantined; no PIE | Not reached |

Core actor labels: `EA_BlinkCube`, `EA_Camera`, `EA_Floor`, `EA_HideCube`, `EA_LOD`, `EA_Light`, `PlayerStart`, `PostProcessVolume`. Target mappings: missing object→EA_HideCube; blinking→EA_BlinkCube; LOD corruption→EA_LOD; lighting mismatch→EA_Light. Cold reopen, shader/streaming readiness, 20-frame noise, ROI freeze, three events per route, travel/GC and owner preview remain unperformed; runtime qualification is false.

**Config STOP and managed changes.** The new file is [WorldState/1202994220.json](D:/IA_Europa/StylizedParisStreet/Saved/Config/WorldState/1202994220.json), 50 bytes, SHA-256 `cfd1637a188c41a6accb22bbc76f3a84d5762b4e6b21ea8ad2e87816a3d8bbc4`. The UTF-16 package-name CRC32 for `/Game/EuropaPhaseA/EA_Core` is 1202994220 and the JSON contains `Folders`. [EditorActorFolders.cpp:23](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/EditorActorFolders.cpp:23) selects the path; line 49 registers PostSaveWorld; [line 132](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/EditorActorFolders.cpp:132) writes it directly. `-nowrite` is checked by the separate [ConfigCacheIni writer](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/ConfigCacheIni.cpp:1478). This explains the engine origin. It does not authorize a Config exception: FC5 does not list WorldState and EN5 excludes protected-state differences. The file remains preserved.

Also preserved: [Saved/Backup map](D:/IA_Europa/StylizedParisStreet/Saved/Backup/EA_Core_2026-10-30-05-13-05.umap), 7,549 bytes, SHA-256 `7cecfa84b4b4f685f732ce3893823aa61f23f725f6151fe5a29c778b7c2a87c3`, outside Content. [PackageBackup.cpp:36](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/PackageBackup.cpp:36) constructs the dated backup and copies the prior map version. The recipe’s new-level/save sequence accounts for a backup before the final 21,114-byte map; no restoration or extra launch was used to inspect it.

FC5 managed deltas: 23 files, comprising 21 small asset-registry cache timestamp changes, one `Intermediate/CachedAssetRegistry.bin` content change (7,781,835→7,784,444 bytes), and `Saved/Autosaves/PackageRestoreData.json` mtime only. Cache hashes before/after are in [the assessment](D:/IA_Europa/Receipts/09426c/assessment.json). Source: [AssetDataGatherer.cpp:412](E:/EpicGames/UE_4.25/Engine/Source/Runtime/AssetRegistry/Private/AssetDataGatherer.cpp:412) and [PackageAutoSaver.cpp:47](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/PackageAutoSaver.cpp:47). Nine zero-byte `Intermediate/Config/CoalescedSourceConfigs/*.ini` files changed mtime, retaining empty hashes: Crypto, Encryption, InternationalizationExport, LocalizationServiceSettings, SourceControlSettings, TranslationPickerSettings, UnitTest, UnitTestStats and UnrealInsightsSettings. [ConfigCacheIni.cpp:1908](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/ConfigCacheIni.cpp:1908) documents these generated backups. No compiled output changed.

**Uploads and UBT argv.** Both recorded editor argv arrays include `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False` and `-ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False`. Sources: [EngineAnalytics.cpp:105](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/EngineAnalytics.cpp:105), [AnalyticsPrivacySettings.h:18](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Classes/Analytics/AnalyticsPrivacySettings.h:18) and [FC4 source chain](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09428-authoring-amendments.md). Crash-monitor children inherit the engine privacy context; their argv are retained without claiming the editor switches appear literally in every child argv.

| Session / log | Bytes | SHA-256 | Upload evidence |
| --- | --- | --- | --- |
| Smoke / Runtime.log | 106069 | abf118a4d5740a450e079278cd232d4816f48cbb5bad2758b4e2b255e27716ea | None |
| Smoke / CrashClient.log | 6644 | 237509b156081ad1d97367cf4e6e816cda40d0a437b6629b32a2a7bfccaf8aa6 | None |
| EA_Core / Runtime.log | 111674 | d40e0f3a1138907cc81af4204a6ce46ad7f15c7383ed603722203fa9393a718b | None |
| EA_Core / CrashClient.log | 6645 | 1ec1859cbf24e6912bec1d9a439046de1cd8e96b626d479bdeb0a287d916087d | None |

Checks: [Smoke upload check](D:/IA_Europa/Receipts/09426c/fixture-attempts/Smoke/01/upload-check.json); [Core upload check](D:/IA_Europa/Receipts/09426c/fixture-attempts/EA_Core/01/upload-check.json). The DataRouterUrl configuration and normal monitor lifecycle are not POST/send events. This is source-and-log evidence, not packet capture. Travel did not run.

Every real UBT child:

| Session | PID / editor parent | Recorded command line |
| --- | --- | --- |
| Smoke | 12860 / 34352 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |
| EA_Core | 2332 / 43724 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |

Both were stock non-build validation. No compiler, linker, UHT, UAT or ShaderCompileWorker was recorded. [Identity-based process closeout](D:/IA_Europa/Receipts/09426c/process-closeout.json) verified all ten owned processes ended: two editors, four crash monitors, two UBT children and two conhosts. No foreign process was stopped. Exact process trees and argv remain in the runtime child receipts.

Marker `E:/EpicGames/UE_4.25/Engine/Intermediate/EditorRuns/31124`: 0 bytes, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; creation/write `2026-10-26T18:43:59.9006068Z`, unchanged.

**Ledger, canary and command audit.** [Ledger summary](D:/IA_Europa/Receipts/09426c/ledger-summary.json) records 8 completed operations, no ledger/admission STOP, no new/removed drive-root attribution and no actual EU2 pause. The runtime Config STOP is separate from passing filesystem-routing ledgers. C/E/5.1-engine charged exception growth: **0 / 0 / 0 bytes**; caps remain 250/50/100 MiB. Classified C rows include crash-client metadata, NVIDIA cache, Unreal XML config cache and Windows event logs; full paths/classes remain in the summary. Protected enumeration has no access errors; 239 OS/tooling metadata access errors are listed, not treated as empty. Foreign timestamp/log activity was recorded; foreign root contents were not opened. No 5.1 launch/build or reparse point was created.

The fresh launch canary control was non-vacuous. Filtered boundaries dropped the canary and blocked identifiers; both editor environment receipts contain zero forbidden names. [Final launch-second scan](D:/IA_Europa/Receipts/09426c/canary-final.json) passed: 574 readable files, 1,339,155,856 bytes plus 612,046,767 decoded archive bytes; zero matches, zero unresolved read errors, six passing scanner controls. Scope: new/changed files since `2026-10-29T23:10:25.017090+00:00` under D:/IA_Europa and the run folder, plus exact ledger-attributed C/E/engine paths. No full EK1 scan, per FA6. FE2 excluded 1 access-denied event log(s) under `C:\Windows\System32\winevt\Logs`; readable event logs were scanned.

| FE2 excluded path | Reason |
| --- | --- |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-GroupPolicy%4Operational.evtx | OS access denied (5) |

[EU1/EV1 audit](D:/IA_Europa/Receipts/09426c/command-audit-final.json) passed through its recorded event boundary: 112 filtered invocations, 1 direct read-only deviation, 1 boundary-log lock refusal before child start, 19 recorded file-edit events, zero direct write-capable commands and zero pause-window violations. The finalizer itself is included as a filtered invocation. The report bytes receive a separate hash-based canary check.

**Deviations and corrections.** The initial direct read-only command read the prior filtered wrapper and new ruling to establish this run’s boundary; EU1 records it as a deviation. One parallel wrapper append met a log-file lock and refused before starting its child; wrapper starts were then serialized. Read-only missing-helper/look-up failures remain in the command record. The first FE test failed on a generator/list mismatch and passed on its second attempt. During report-only assessment, UTF-8 parsing of the engine’s UTF-16 WorldState JSON failed; byte-aware JSON decoding then passed. These did not add runtime attempts. The unexpected Config and Saved/Backup writes are preserved and disclosed; no FC5 widening or repair followed the STOP. Exit hashing preceded final canary/audit to include its receipts. No full EK1 or dry run was performed because FA6/FD5 prescribe their omission. No strict-include build was run because this job does not authorize builds. FE3’s later guard work remains outside this job.

**EN5 items and source citations.** [EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54) was not used to bypass the Config failure.

| Item / source | Application |
| --- | --- |
| Protected-state difference — [EN5:66](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:66); [FC5](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:68) | WorldState is outside admitted classes: hard STOP, RED, quarantine, no Travel or retry. Source attribution is not a waiver. |
| Generated host caches — [FC5](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:68); [AssetDataGatherer](E:/EpicGames/UE_4.25/Engine/Source/Runtime/AssetRegistry/Private/AssetDataGatherer.cpp:412); [ConfigCacheIni](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/ConfigCacheIni.cpp:1908) | Named asset-registry/autosave deltas recorded; zero-byte Intermediate config mtime rewrites recorded narrowly. No extension to project Config or WorldState. |
| Editor UBT — [FD2](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:32); [TargetPlatformManagerModule](E:/EpicGames/UE_4.25/Engine/Source/Developer/TargetPlatform/Private/TargetPlatformManagerModule.cpp:996) | Owned ValidatePlatforms argv retained; no build action or compiler/linker/UHT child. FD2 implementation unchanged. |
| Memory — [FD3](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:45) | 6 GiB physical, 8 GiB commit start, 4 GiB commit stop retained; no runtime tuning. |
| Secrets/base environment — [EL1/EL2](D:/IntrusiveAnomalies/_reviews/094-11-ruling.md); [EM2](D:/IntrusiveAnomalies/_reviews/094-12-ruling.md) | No blocked names or secrets in child records; nonblocked base variables preserved by contracts. Fresh canary scan passed. |
| Event-log exclusion — [FE2](D:/IntrusiveAnomalies/_reviews/094-26b-ruling.md) | Only denied OS .evtx class, exact folder/count recorded; no security or permission workaround. |
| Uploads — [FC4](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:61) | Process-only privacy overrides retained; no upload evidence; uploads remain STOPs. |
| Engine markers — [ET2](D:/IntrusiveAnomalies/_reviews/094-18-ruling.md:54); [EG1](D:/IntrusiveAnomalies/_reviews/094-06-ruling.md:24) | 31124 preserved; engine/C/E changes classified, no engine write outside admitted classes identified. |
| Commands/disk — [EU1/EU2](D:/IntrusiveAnomalies/_reviews/094-19-ruling.md:30); [EV1/EV3](D:/IntrusiveAnomalies/_reviews/094-20-ruling.md); [EY4](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md) | Read-only deviation recorded; writers filtered; no pause-window edit or floor breach; prescribed disk lines unchanged. |
| Foreign roots/admission — [EZ1/EZ2](D:/IntrusiveAnomalies/_reviews/094-24-ruling.md); [FA6](D:/IntrusiveAnomalies/_reviews/094-25-ruling.md:110) | No new/removed root entry; foreign activity recorded without opening foreign contents or ending work. Lean scans applied. |
| Launch authority — [FE4](D:/IntrusiveAnomalies/_reviews/094-26b-ruling.md); [FC3](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:46); [FD5](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:68) | Only armed kit launched Smoke/Core after idle checks; no Travel, retries or dry runs. |
| Other EN5 hard stops | No P9 difference, discovered secret exposure, machine/security/global setting change or actual floor breach. This was an authorized launch job. The new protected Config path was not excused. |

**Commits, settings and inventories.** Local commits on `feat/europa-port`:

| Commit | Change / W4 actual time |
| --- | --- |
| 9f5ce85a562fa162d3570aef74292cb48d50b03c | FE1/FE2 tooling, tests, recipe bindings and prediction; 2026-10-30 04:53:04 +05:30 |
| aecff71f87181ea33e7d3b85702740bf717bccbb | Receipts index and documentation; 2026-10-30T05:25:57+05:30 / 2026-10-30T05:25:57+05:30 |

Commits pushed: **none**. Tags: **none**. The documentation commit updates CLAUDE.md, G472, architecture, setup-runbook, onboarding, journal and receipt index. Exact staged-file/whitespace checks passed; the feature worktree is clean. No source/kit change followed the real-run STOP.

Retained settings: only the authorized recipe AA-item removal and FE2 scan policy, plus the fresh arming. No persistent machine/security or bench lever was changed. Both editors used process-only privacy flags, `-IAIBench`, `-EuropaPhaseA`, Python/EditorScriptingUtilities, 1280×720, routed UserDir/DDC/ShaderWorking/logs, `-nowrite` and `-SkipCompile`; those processes ended. Test readings and mutant scripts stayed on owned scratch. Unexpected WorldState/backup and admitted managed deltas remain recorded; no restoration is claimed. No files in E:/IA_BuildCache or D:/IA_BankOverflow were deleted or moved.

Unchanged binary manifests; both 5.1 records remain frozen:

| Manifest | Files | SHA-256 |
| --- | --- | --- |
| 425 | 27 | c501895c9856638dd76e1c2091be8726f6797014bcb0714e306fd9d6ebd98df6 |
| baseline51 | 39 | b05e6f8c9ce12fa1c5b4dbd142a59383f737bb8a3b7885b7219d1bcdb6c1fc37 |
| regate51 | 44 | 3865fe0c6ed039d5bad2bc54d1a6a89f11c403e98237b4c6caf74ca661ea12e2 |

All 110 artifact paths/sizes/hashes are in [binary-hashes.tsv](D:/IA_Europa/Receipts/09426c/binary-hashes.tsv). Every bound EXE/DLL is listed below. Prefixes under `D:/IA_Europa`: **425 = StylizedParisStreet**, **B51 = Baseline51/StackOBot**, **R51 = ReGate51/StackOBot**. These are unchanged records, not new builds.

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

Engine, recorded runtime-tree and interpreter hashes (engine Python 2.7 was launched as the required plain-interpreter harness):

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

No compiled binary or binary archive was created. [Archive/image inventory](D:/IA_Europa/Receipts/09426c/archive-image-inventory.json) binds every compressed receipt archive. No EXE/DLL-named test files remain under this job’s Receipts/Temp roots.

Every compressed archive path follows, relative to `D:/IA_Europa/Receipts/09426c/`; the inventory above gives each SHA-256:

| Archive path |
| --- |
| fixture-attempts/EA_Core/01/ledger/fixture-preflight/after-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-preflight/before-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-preflight/classified-changes.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-runtime/after-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-runtime/before-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-runtime/classified-changes.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/initial.sqlite.gz |
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

VERDICT: RED | Smoke passed; EA_Core created a bound WorldState Config file, so both Core packages were quarantined and EA_Travel never started.
