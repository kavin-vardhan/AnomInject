<!-- Collected by Claude Code (mailbox brief 094-26d) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md at 2026-10-30 after Codex exited (thread 01a14334, relay tries 1, exit 0, duration 3,115 s); body below is byte-identical, sha256 5A7FEAA9C5F4BA2AFEB19769E41593D71077C09E968AAB3AB998D0A10EEBE16F, 36626 bytes -->
**094-26d: GREEN.** EA_Core and EA_Travel were authored fresh by the armed 4.25 kit, each in one editor session with exit 0. Exactly three declared packages remain. FF2 records Saved bookkeeping while protecting Saved/Config INI files and project Config. No protected-state change, crash, upload evidence or build was found. The earlier Core packages remain quarantined. **No smoke, dry run, retry, push or tags.** A6 authoring is complete; A7 runtime and picture qualification remains pending.

Evidence: [Receipts/09426d](D:/IA_Europa/Receipts/09426d); [committed receipt index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426d-authoring-receipts.json). Chat’s rulings supplied authority; Code’s reviews supplied observations.

**Entry and exit proofs.** [Prior-results check](D:/IA_Europa/Receipts/09426d/prior-results-entry.json) applies BINDING 5 to 094-26c. Its known WorldState STOP is explicitly reclassified by FF1/FF2; neither prior report identifies another protected change, crash or upload. ORDER 1’s “094-28” reference was read with BINDING 5’s explicit 094-26c inputs. [EA8 entry](D:/IA_Europa/Receipts/09426d/P9/entry/summary.json) and [EA8 exit](D:/IA_Europa/Receipts/09426d/P9/exit/summary.json) passed against P9 v2: 472,553 inventory files, 1,023 EG1 entries, 23,033 binary/config hashes and 104,794,989,456 bytes. Protected Git and source manifests are equal. v2 SHA-256: `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. m51 remains `53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67`, with its baseline index and 15 status entries preserved.

| Drive | Entry GiB | Final GiB | Start / warn / floor GiB |
| --- | --- | --- | --- |
| C | 30.98 | 30.98 | 13 / 11 / 10 |
| D | 71.6 | 70.48 | 40 / 30 / 25 |
| E | 291.6 | 291.6 | 15 / 12 / 10 |

[ED3 toolchains](D:/IA_Europa/Receipts/09426d/toolchains.json): VS 2022 17.12.35707.178 and VS 2017 15.9.34031.82; MSVC 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; SDK 10.0.17763.0 and 10.0.22621.0. No 14.44, installation or build. [Final state](D:/IA_Europa/Receipts/09426d/final-state.json) verifies 1870 distinct bound hashes and the unchanged spent kit.

**FF2 change and tests.** [FF2](D:/IntrusiveAnomalies/_reviews/094-26c-ruling.md:29) is implemented in [fixture_policy.py](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/fixture_policy.py:251). All host Saved files are recorded; any `.ini` under Saved/Config takes precedence and remains protected, including crash-client INI files. Project Config, descriptors, .uproject and Content protection remain. The original config comparison is extracted into a callable used at both admission and postflight, so the can-fails exercise the same gate. FF3 removes Smoke from this arming and from both recipe prerequisites. Full per-recipe preflight remains; FF5 belongs to 094-27.

The [dated prediction](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426d-authoring-amendments.md) and tooling were committed before arming. The FE1 recipe, A5 manifest, in-editor scripts, FC2 launcher, FD2 guard, FD3 thresholds and FE2 scanner class are hash-equal to their accepted versions. The existing native smoke evidence remains 51/51; no redundant smoke or Python 2.7 rerun was added for unchanged scripts.

| Affected check | Result | Receipt |
| --- | --- | --- |
| FF classification / scope | 83 PASS, including new/changed/removed Saved files; INI/project Config refusals; descriptor bindings; old-arm and Smoke refusals | [FF checks](D:/IA_Europa/Receipts/09426d/tests/affected-attempt1/ff/summary.json) |
| FE / FC / EY | 14 / 47 / 46 PASS | [Affected suites](D:/IA_Europa/Receipts/09426d/tests/affected-attempt1/summary.json) |
| Session / measured gate | 57 / 50 PASS | [Final suite binding](D:/IA_Europa/Receipts/09426d/final-suites.json) |
| Environment-read checker | 19 self-tests PASS; 72 helpers, 17 reads | [Environment checks](D:/IA_Europa/Receipts/09426d/tests/affected-attempt1/environment.json) |

All seven affected suites passed on their first attempt. D2 helper corrections: the entry helper’s first exact list comparison used string ordering instead of Windows Path ordering; [all 42 path/size/hash records matched](D:/IA_Europa/Receipts/09426d/entry-config-comparison.json), and its second check passed. The audit helper’s first self-test exposed wrapper normalization that could mistake an inner shell for an outer shell; preserving the filtered invocation fixed it, and the second attempt passed the retained checks. Neither correction changed a runtime gate or consumed an editor slot.

**Arming and live reading.** [Arming receipt](D:/IA_Europa/Receipts/09426d/arming/receipt.json) binds tooling `759a7baf28c95284ec25e5297175c8d8dbffb0a7`, 84 tooling/doc files, 198 host files, 7 engine files, 1,458 existing Content files, 41 protected config files and 110 EW1 artifacts. The 09426c, 09426b, 09428, 09425, 09424, 09423 and 09422 armings refuse. All 15 new real paths were absent at arming. No dry-run session was performed.

Slots: `D:/IA_Europa/Receipts/09426d/fixture-state/{global,EA_Core,EA_Travel}` and `fixture-attempts/{EA_Core,EA_Travel}/01–03`; TEMP mirrors attempts under `D:/IA_Europa/Temp/09426d/fixture-attempts`. Both recipes used slot 01; 02/03 remain unused. There is no Smoke slot. Each recipe permits one editor session, up to 1,800 seconds; prelaunch slots do not authorize extra sessions. [Exact argv templates](D:/IA_Europa/Receipts/09426d/arming/exact-argv.json) and [console/refusal catalog](D:/IA_Europa/Receipts/09426d/arming/console-catalog.json) are retained.

The [live observe-only reading](D:/IA_Europa/Receipts/09426d/live-gate.json) at 06:12:35 IST passed: 21125.578 seconds idle, 8.22 GiB physical, 14.29 GiB commit and GPU 4%. No blockers were misread. Launcher-folder exemptions: 7080, 20180, 25528, 35008, 36968, 38440, 40848, 43120, 43160, 43916. The launcher stayed owner-started PID 25528. FD3 remains 6 GiB physical / 8 GiB commit at admission, and a 4 GiB in-session commit stop.

| Kit file | Bytes | SHA-256 |
| --- | --- | --- |
| Europa425-FixtureAuthoring.bat | 161 | 4c5ad14eeb8fd22f0ebcd6ec2349ad28a874c21826cfda36df03c99b45982c41 |
| Europa425-FixtureAuthoring.json | 535238 | e79de5f49bd6ad41de4f568227b05896868a6cd834a944f91ea63cfb48af0ead |

**Real timeline — 2026-10-30, IST.** The FF4 deadline is 06:30 IST on 10-31. The BAT ran once, 06:14:01–06:32:44, exit 0. Each FA5 pre-check passed with at least ten minutes idle and the memory/GPU/process conditions; each session then passed three measured polls. [Entry-point receipt](D:/IA_Europa/Receipts/09426d/entrypoint-01.json).

| Stage | Start–end IST | Duration / result |
| --- | --- | --- |
| EA_Core gate | 06:19:53–06:20:51 | 62.199 s; three quiet polls |
| EA_Core editor PID 28984 | 06:21:52–06:22:07 | 14.978 s; normal exit 0 |
| EA_Travel gate | 06:29:20–06:30:18 | 62.149 s; three quiet polls |
| EA_Travel editor PID 34672 | 06:31:28–06:31:40 | 12.607 s; normal exit 0 |

| Idle pre-check receipt | Passing idle seconds |
| --- | --- |
| idle-precheck-01.jsonl | 21211.5 |
| fixture-runs/b2d7792e16074ab383f4d6b6f1769a34/EA_Core-idle-precheck.jsonl | 21215.719 |
| fixture-runs/b2d7792e16074ab383f4d6b6f1769a34/EA_Travel-idle-precheck.jsonl | 21766.516 |

**Console, verbatim.** [Original console](D:/IA_Europa/Receipts/09426d/entrypoint-01-console.log), SHA-256 `d597c7e182c8d53b18dbeafad093dffbdbeef242ad380c226c805cfb94fa9779`.

```text
Europa fixture authoring: checking the EW2 arming.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
EA_Core: attempt 1 of 3; checking protected files, Content, binaries and routing.
EA_Core: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
EA_Core: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in D:\IA_Europa\Receipts\09426d\fixture-attempts\EA_Core\01\session.
EA_Core: clean exit and output checks passed; this recipe is now spent.
Idle pre-check passed: ten minutes idle and all measured conditions hold.
EA_Travel: attempt 1 of 3; checking protected files, Content, binaries and routing.
EA_Travel: protected files, Content, binaries and routing passed.
Waiting: three passing polls, 25 seconds apart.
Measured gate passed: three quiet polls; the launcher is already running.
EA_Travel: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in D:\IA_Europa\Receipts\09426d\fixture-attempts\EA_Travel\01\session.
EA_Travel: clean exit and output checks passed; this recipe is now spent.
Both recipe sessions finished. The two launch authorizations are spent.
```

**Evidence a–g per recipe.** All attempt-relative paths below are under `D:/IA_Europa/Receipts/09426d/fixture-attempts/<recipe>/01/`. [Full comparisons and raw evidence index](D:/IA_Europa/Receipts/09426d/result-evidence.json).

**EA_Core.**

| Item | Evidence |
| --- | --- |
| a. Clean exit | 2 skipped engine-exit cleanup row(s), exactly one probe_stop, skipped root release, no PIE; normal LogExit and exit 0. [Probe](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Core/01/session/probe/probe.jsonl); [runtime receipts](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Core/01/ledger/fixture-runtime) |
| b. Outputs / protection | 2 declared packages newly present, no BuiltData or sidecar. Existing Content, protected config, descriptor/.uproject and binary hashes unchanged. [Attempt result](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Core/01/attempt-result.json) |
| c. A5 recipe | AUTHORED_NOT_QUALIFIED; two LODs, eight actors, four expected target labels; item-by-item comparison below. [Authoring receipt](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Core/01/authoring.json) |
| d. Uploads | Both exact privacy switches in editor argv; Runtime.log and CrashClient.log pass the kit and independent upload checks. No new crash/ensure report folder. [Upload check](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Core/01/upload-check.json) |
| e. EditorRuns | 31124 remains the sole final marker, 0 bytes, original creation/mtime and empty hash. No new lingering marker. |
| f. Ledger | P9 preflight and runtime ledgers PASS; C/E/5.1 classes retained. [Preflight](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Core/01/ledger/fixture-preflight/summary.json); [runtime ledger](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Core/01/ledger/fixture-runtime/summary.json) |
| g. Failed session / quarantine | Not applicable: session passed. No new quarantine; the FF1 prior packages remain untouched. |

**EA_Travel.**

| Item | Evidence |
| --- | --- |
| a. Clean exit | 1 skipped engine-exit cleanup row(s), exactly one probe_stop, skipped root release, no PIE; normal LogExit and exit 0. [Probe](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Travel/01/session/probe/probe.jsonl); [runtime receipts](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Travel/01/ledger/fixture-runtime) |
| b. Outputs / protection | 1 declared packages newly present, no BuiltData or sidecar. Existing Content, protected config, descriptor/.uproject and binary hashes unchanged. [Attempt result](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Travel/01/attempt-result.json) |
| c. A5 recipe | AUTHORED_NOT_QUALIFIED; two LODs, eight actors, four expected target labels; item-by-item comparison below. [Authoring receipt](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Travel/01/authoring.json) |
| d. Uploads | Both exact privacy switches in editor argv; Runtime.log and CrashClient.log pass the kit and independent upload checks. No new crash/ensure report folder. [Upload check](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Travel/01/upload-check.json) |
| e. EditorRuns | 31124 remains the sole final marker, 0 bytes, original creation/mtime and empty hash. No new lingering marker. |
| f. Ledger | P9 preflight and runtime ledgers PASS; C/E/5.1 classes retained. [Preflight](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Travel/01/ledger/fixture-preflight/summary.json); [runtime ledger](D:/IA_Europa/Receipts/09426d/fixture-attempts/EA_Travel/01/ledger/fixture-runtime/summary.json) |
| g. Failed session / quarantine | Not applicable: session passed. No new quarantine; the FF1 prior packages remain untouched. |

Every actual error/ensure in Runtime.log was reviewed; both counts are zero. Keyword matches were only engine zero-error MapCheck summaries:

| Session | Log line | Classification |
| --- | --- | --- |
| EA_Core | 636: [2026.10.30-00.52.02:702][  0]MapCheck: Map check complete: 0 Error(s), 0 Warning(s), took 0.101ms to complete. | Engine informational summary; zero errors and warnings, not an error or ensure. |
| EA_Travel | 636: [2026.10.30-01.01.36:313][  0]MapCheck: Map check complete: 0 Error(s), 0 Warning(s), took 0.104ms to complete. | Engine informational summary; zero errors and warnings, not an error or ensure. |

The [A5 manifest](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09426c-a5-fixture-manifest.json) stays SHA-256 `686cbd56eea0afa663c6113832b7dcfaad5f32d33ee6088e51f15b48261d19ac`; recipe `f55aff4ddc6e239907d3b278ad2bb0a032f79fa24e502d32fa32ac84297a43dd`; recipe document `7050e652be5899d72dfab9baa4b9af044b754f1e864584d67e1110b70a5e761d`. Both successful receipts and the bound executed recipe support each item below. This is authoring evidence, not independent saved-property readback or A7 picture qualification.

| A5 item, both recipes | Evidence / limit |
| --- | --- |
| Owned host / no overwrite | Owned project and absent-destination guards passed; Core fresh, Travel requires Core success. |
| Sphere LOD0 / cube LOD1 | Core duplicates the engine sphere and installs cube LOD1; both receipts report lod_count 2; Travel loads that same bound mesh. |
| Floor / hide / blink / LOD actors | Receipts list EA_Floor, EA_HideCube, EA_BlinkCube, EA_LOD; bound recipe sets the manifest positions and scales. |
| Movable point light | EA_Light present; bound recipe sets (450,0,650), intensity 5000, radius 1800 and shadows. |
| Camera | EA_Camera present; bound recipe sets (-550,0,480), rotation (-12,0,0), FOV 50. |
| Post-process | Unbound volume; six retained exposure/motion-blur fields execute. FE1’s two unsupported AA setters remain absent. |
| World / start | GameModeBase and PlayerStart (-550,0,500) set by the bound recipe; eight actors recorded. |
| Targets / save | Both receipts match missing_object→EA_HideCube, blinking→EA_BlinkCube, lod_corruption→EA_LOD, lighting_mismatch→EA_Light; only new declared packages saved; no PIE. |

**Every Saved change under FF2.** Paths are relative to `D:/IA_Europa/StylizedParisStreet/`. Each row is the per-session before/after comparison, including identical-content timestamp rewrites. Full sizes, hashes and timestamps are in each attempt’s `managed-before.json` / `managed-after.json`, and [the combined Saved inventory](D:/IA_Europa/Receipts/09426d/result-evidence.json). Protected Saved/Config INI files have zero changes.

| Session | Saved path | Change | Bytes after | SHA-256 after |
| --- | --- | --- | --- | --- |
| EA_Core | Saved/Backup/EA_Core_2026-10-30-06-22-04.umap | added | 7549 | 3d1907d29d8df33b21df7898fe0369e1070e04fe39d3547be4dcd80e978526b3 |
| EA_Core | Saved/Autosaves/PackageRestoreData.json | mtime only | 96 | fdc2d5bebf9cdef10fe7088228558cb57ff12b0ea92ea114809e20fd2add5bb6 |
| EA_Core | Saved/Config/WorldState/1202994220.json | mtime only | 50 | cfd1637a188c41a6accb22bbc76f3a84d5762b4e6b21ea8ad2e87816a3d8bbc4 |
| EA_Travel | Saved/Backup/EA_Travel_2026-10-30-06-31-37.umap | added | 7559 | a3b4b2a1163d1ec9afdd07604c681bc9a81c26c0a03634c099e9bdbc3f8ceb08 |
| EA_Travel | Saved/Config/WorldState/2530609069.json | added | 50 | cfd1637a188c41a6accb22bbc76f3a84d5762b4e6b21ea8ad2e87816a3d8bbc4 |
| EA_Travel | Saved/Autosaves/PackageRestoreData.json | mtime only | 96 | fdc2d5bebf9cdef10fe7088228558cb57ff12b0ea92ea114809e20fd2add5bb6 |

FF2 permits these files directly. WorldState is written by [FActorFolders::OnWorldSaved](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/EditorActorFolders.cpp:132); Core/Travel UTF-16 path CRC32 values are 1202994220 / 2530609069. [PackageBackup.cpp](E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/PackageBackup.cpp:231) explains re-save backups. FC5 Intermediate cache changes and zero-byte coalesced-config timestamp rewrites are separately recorded in [assessment.json](D:/IA_Europa/Receipts/09426d/assessment.json); build-output hashes remain equal. Old Saved/Crashes folders are unchanged; no new crash/ensure report folder was created.

**Outputs.** Paths relative to `D:/IA_Europa/StylizedParisStreet/Content/EuropaPhaseA/`:

| Package | Bytes | SHA-256 |
| --- | --- | --- |
| SM_EA_TwoLod.uasset | 137553 | bbd24a79a2b46efeea8a8e3d23c65ebb1eef73b16ea1af97e04ff8bb7cec714a |
| EA_Core.umap | 21114 | c2986f9aee65dbb5d5b1af0016759f7ef881d3636400de19938206fb05284a14 |
| EA_Travel.umap | 21124 | 51c7fe5c212b2c7996fa5af4beebdf79bf0b01ceaf59d0a041ceb34f39fd70d0 |

FF1 quarantine remains under `D:/IA_Europa/Receipts/09426c/fixture-attempts/EA_Core/01/quarantine/`, hash-equal to entry:

| Quarantined package | Bytes | SHA-256 |
| --- | --- | --- |
| EA_Core.umap | 21114 | b786610b6e9787cffa1a3aae4898af7852aa5690aaca50ebaf426d673c08df3c |
| SM_EA_TwoLod.uasset | 137553 | 090e5f049b834cf103233be5af06421e5c984ac06688814b2367a92af06f8327 |

**Uploads and UBT argv.** Each editor argv contains exactly `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False` and `-ini:EditorSettings:[/Script/UnrealEd.AnalyticsPrivacySettings]:bSendUsageData=False`. The unchanged 4.25 source chain is in [FC4 source citations](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09428-authoring-amendments.md). Both Runtime/CrashClient log pairs show no POST/send/upload; DataRouter URL configuration alone is not a request. Engine-created child argv are retained verbatim. This is source-and-log evidence, not packet capture.

| Recipe | UBT PID / parent | Recorded argv |
| --- | --- | --- |
| EA_Core | 14852 / 28984 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |
| EA_Travel | 37404 / 34672 | "E:/EpicGames/UE_4.25/Engine/Binaries/DotNET/UnrealBuildTool.exe" -Mode=ValidatePlatforms -allplatforms |

FD2 admitted only the observed ValidatePlatforms actions; no compiler, linker, UHT, cook or UAT ran in either owned tree, and no build output changed. [Process closeout](D:/IA_Europa/Receipts/09426d/process-closeout.json) confirms all 10 owned processes ended. FD2/FD3 were not altered during the run.

The Travel root process record retains one transient command-line-length read error. Its exact launch argv is retained in the child receipt and corroborated by Runtime.log; both UBT child argv were readable and retained. This did not change the gate or trigger a retry.

**Ledger and canary.** [Ledger summary](D:/IA_Europa/Receipts/09426d/ledger-summary.json) contains 8 completed operations, all PASS, zero admission/operation stops, 0 real pause windows, and charged growth C/E/5.1 = {'C': 0, 'E': 0, 'engine51': 0} bytes. EA4 limits remain 250/50/100 MiB. Stock EditorRuns and admitted cache/config bookkeeping are classified under ET2/EG1/ET3; foreign launcher/OS activity is recorded. Metadata enumeration denials are listed, not treated as empty protected trees. Foreign root attributions: 0; no foreign contents were opened.

The fresh canary control was non-vacuous. Filtered boundaries dropped the canary and blocked identifiers while retaining fingerprints of nonblocked base variables; both editor environments contain zero forbidden names. [Final launch-second scan](D:/IA_Europa/Receipts/09426d/canary-final.json) passed: 616 readable files, 1,340,079,059 bytes and 612,499,719 decoded archive bytes; zero matches, zero unresolved read errors, six passing controls. Selection starts at `2026-10-30T00:27:19.371222+00:00` under D:/IA_Europa and the run folder plus exact ledger-attributed C/E/engine files. No full EK1 scan, per FA6. FE2 excluded 2 access-denied `.evtx` file(s) under `C:\Windows\System32\winevt\Logs`; readable logs were scanned.

| FE2 excluded path | Reason |
| --- | --- |
| C:\Windows\System32\winevt\Logs\Microsoft-Windows-StorageManagement%4Operational.evtx | OS access denied (5) |
| C:\Windows\System32\winevt\Logs\Security.evtx | OS access denied (5) |

**Command audit and deviations.** [EU1/EV1 audit](D:/IA_Europa/Receipts/09426d/command-audit-final.json) passed: 79 filtered invocations, 2 direct read-only bootstrap deviations, 18 file-edit events, zero direct write-capable commands and zero pause-window violations. Its finalizer invocation is included. Initial wrapper/ruling reads established the fresh boundary; no secret value was printed or passed to a writer. One progress read preceded creation of its target summary and returned file-not-found. The entry ordering and audit-normalization corrections each passed their second attempt; no runtime retry occurred. Exit hashing precedes the final scan/audit to include those receipts. FF3/FD5/FA6 explicitly omit smoke, dry runs and full EK1. No strict-include build or binary rebuild was run because this job changes only Python kit tooling and does not authorize a build. No new EN5 waiver or scope change was inferred.

**EN5 application and citations.** [EN5](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:54) retains every hard stop. FF2 is explicit authority, not an inferred exception.

| Item / source | Application |
| --- | --- |
| [FF2](D:/IntrusiveAnomalies/_reviews/094-26c-ruling.md:29) / [EN5:66](D:/IntrusiveAnomalies/_reviews/094-13-ruling.md:66) | Saved bookkeeping recorded; INI/project Config, descriptors, Content and binaries protected; no protected-state difference. |
| [FC5](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:68); [AssetDataGatherer](E:/EpicGames/UE_4.25/Engine/Source/Runtime/AssetRegistry/Private/AssetDataGatherer.cpp:412); [ConfigCacheIni](E:/EpicGames/UE_4.25/Engine/Source/Runtime/Core/Private/Misc/ConfigCacheIni.cpp:1908) | Asset-registry cache deltas and zero-byte Intermediate config timestamp rewrites recorded; no project Config exemption. |
| [FD2](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:32); [TargetPlatformManagerModule](E:/EpicGames/UE_4.25/Engine/Source/Developer/TargetPlatform/Private/TargetPlatformManagerModule.cpp:996) | Owned ValidatePlatforms argv recorded; no build action or compiler/linker/UHT child. |
| [FD3](D:/IntrusiveAnomalies/_reviews/094-28-ruling.md:45) | 6 GiB physical / 8 GiB commit start; 4 GiB commit stop, unchanged. |
| [EL1/EL2](D:/IntrusiveAnomalies/_reviews/094-11-ruling.md); [EM2](D:/IntrusiveAnomalies/_reviews/094-12-ruling.md) | No blocked child names or secrets; unrelated base variables preserved by boundary contracts; fresh canary scan PASS. |
| [FE2](D:/IntrusiveAnomalies/_reviews/094-26b-ruling.md) | Only denied Windows event logs excluded; folder/count recorded; no permission/security workaround. |
| [FC4](D:/IntrusiveAnomalies/_reviews/094-26-ruling.md:61) | Process-only privacy overrides retained; uploads remain STOPs; no upload evidence. |
| [ET2](D:/IntrusiveAnomalies/_reviews/094-18-ruling.md:54); [EG1](D:/IntrusiveAnomalies/_reviews/094-06-ruling.md:24) | 31124 unchanged; engine/C/E writes classified within admitted classes. |
| [EU1/EU2](D:/IntrusiveAnomalies/_reviews/094-19-ruling.md:30); [EV1/EV3](D:/IntrusiveAnomalies/_reviews/094-20-ruling.md); [EY4](D:/IntrusiveAnomalies/_reviews/094-23-ruling.md) | Read-only deviations recorded; all writers filtered; no pause edit or floor breach; disk lines retained. |
| [EZ1/EZ2](D:/IntrusiveAnomalies/_reviews/094-24-ruling.md); [FA6](D:/IntrusiveAnomalies/_reviews/094-25-ruling.md:110) | Foreign activity recorded; no foreign folder contents opened; lean checks applied. |
| [FF1/FF3/FF4](D:/IntrusiveAnomalies/_reviews/094-26c-ruling.md) | Prior quarantine preserved; only armed Core and Travel launched, after idle gates, within the window; no smoke or dry run. |
| Remaining hard STOPs | No P9 difference, secret exposure, engine write outside EG1, global/security setting change or actual floor breach. This was an authorized editor-launch job. |

**Commits and settings.** Local commits on `feat/europa-port`:

| Commit | Change / actual W4 time |
| --- | --- |
| 759a7baf28c95284ec25e5297175c8d8dbffb0a7 | FF2/FF3 tooling, tests and prediction; 2026-10-30 06:11:10 +05:30 |
| 1686c4de345923b09fca149ab1cef2843c5eef82 | Receipt index and docs; author 2026-10-30T06:43:46+05:30, committer 2026-10-30T06:43:46+05:30 |

Commits pushed: **none**. Tags: **none**. The feature worktree is clean. The docs commit updates CLAUDE.md, journal, G473, architecture, setup-runbook, onboarding and receipt index; it does not change the armed tooling.

Retained changes: FF2 policy and the fresh, now-spent 09426d arming. No persistent machine/security or bench lever was changed. Process-only settings were privacy opt-outs, `-IAIBench`, `-EuropaPhaseA`, Python/EditorScriptingUtilities, 1280×720, routed UserDir/DDC/ShaderWorking/logs, `-nowrite` and `-SkipCompile`; the processes ended. Test readings and mutants stayed in owned scratch. Saved/Intermediate changes remain recorded; no restoration is claimed. No E:/IA_BuildCache or D:/IA_BankOverflow file was deleted or moved.

**EXE/DLL hashes and archives.** No executable, DLL or binary archive was built. [All 110 EW1 artifacts](D:/IA_Europa/Receipts/09426d/binary-hashes.tsv) and [archive/image inventory](D:/IA_Europa/Receipts/09426d/archive-image-inventory.json) retain the full paths and sizes. All bound and runtime EXE/DLL hashes follow; the two 5.1 sets are frozen records, never launched.

Path prefixes: `H = D:/IA_Europa/StylizedParisStreet`, `B = D:/IA_Europa/Baseline51`, `R = D:/IA_Europa/ReGate51`, `E = E:/EpicGames/UE_4.25/Engine`.

| EXE/DLL path | SHA-256 |
| --- | --- |
| C:/Python313/python.exe | 03573716a7fcbd8b7fed8fe0163bbfbd3a852546920d6a666123f307c8dac7d9 |
| C:/Windows/System32/conhost.exe | c86146188b78e484f32cc29e2331689659943b47689a85570342044f499938ac |
| B/StackOBot/Binaries/Win64/D3D12/D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| B/StackOBot/Binaries/Win64/D3D12/d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| B/StackOBot/Binaries/Win64/OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| B/StackOBot/Binaries/Win64/StackOBot.exe | b6def734b67b3d03636397ec9d43999458ab86e6baa3db9f21382d394fa2d3d0 |
| B/StackOBot/Binaries/Win64/UnrealEditor-EuropaPhaseAProbe.dll | ac3b1ca44fc45b61cc3a6e4f1fc7271c383d337fa86ab1ba45eddce4e2d78012 |
| B/StackOBot/Binaries/Win64/UnrealEditor-StackOBot.dll | 4092eb194804229dd2e4e85dec4c50eb96cefd03491b5f7038c084365e43ec19 |
| B/StackOBot/Binaries/Win64/boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| B/StackOBot/Binaries/Win64/boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| B/StackOBot/Binaries/Win64/boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| B/StackOBot/Binaries/Win64/boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| B/StackOBot/Binaries/Win64/boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| B/StackOBot/Binaries/Win64/boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| B/StackOBot/Binaries/Win64/boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| B/StackOBot/Binaries/Win64/boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| B/StackOBot/Binaries/Win64/tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| B/StackOBot/Binaries/Win64/tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| B/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyBench.dll | 6a8a770188e44660e0d0ba790d14515b600df498eb657bd24af07245dee211d8 |
| B/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyCapture.dll | 9ab3578e0cdab25d9cacf93b226d8d54af2acfda6af0631c5da76f3d5eed9d94 |
| B/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll | 9a41d9a8cb649f0dbd962082ea0b535f6162b2f611146b20d202f2fd457a4b57 |
| B/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyInjector.dll | 91d14a4af7711c3563dccb30589d013648d9a23b5a7f33563e67f0d06443974e |
| B/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyShaders.dll | 284c64cd7936ea260e780a1a010916a72312a77acb2282a0d413b4196b584b0d |
| B/StackOBot/Plugins/CaptureBench/Binaries/Win64/UnrealEditor-CaptureBench.dll | e2ef940d6df3ca2d8075015498da127f32540df20b844d1c23e9ffe1de3ce5d8 |
| R/StackOBot/Binaries/Win64/D3D12/D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| R/StackOBot/Binaries/Win64/D3D12/d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| R/StackOBot/Binaries/Win64/OpenImageDenoise.dll | 11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a |
| R/StackOBot/Binaries/Win64/StackOBot-Win64-Shipping.exe | 010ab98e78e1a4a04c4dc1fb3edf3841336b3bc6f4d8a6db5be24841b6845fb4 |
| R/StackOBot/Binaries/Win64/StackOBot.exe | 5b0fb171a9c65dced68cfd52b8c62540c0ecb29714f30091717b9d055d5f2298 |
| R/StackOBot/Binaries/Win64/UnrealEditor-EuropaPhaseAProbe.dll | f8cab87d9a3e0bb93a8fc92dd20c22d25851c83051f3cd28a0e97da0c6af1b21 |
| R/StackOBot/Binaries/Win64/UnrealEditor-StackOBot.dll | 741b766316baa00dac0a63631dd407b7262ce47fb5f3c677f8d135054117cc0f |
| R/StackOBot/Binaries/Win64/boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| R/StackOBot/Binaries/Win64/boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| R/StackOBot/Binaries/Win64/boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| R/StackOBot/Binaries/Win64/boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| R/StackOBot/Binaries/Win64/boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| R/StackOBot/Binaries/Win64/boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| R/StackOBot/Binaries/Win64/boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| R/StackOBot/Binaries/Win64/boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| R/StackOBot/Binaries/Win64/tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| R/StackOBot/Binaries/Win64/tbb12.dll | de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d |
| R/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyBench.dll | 128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c |
| R/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyCapture.dll | 0e14f06a515844ad27174789fe455284c798a64e8fffe3a8bd12f251e4f162e6 |
| R/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll | 3e740f3b9bcf76cc060a25e3eb6602e5a34b375c9e336b86bd065dac129a8a80 |
| R/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyInjector.dll | 88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5 |
| R/StackOBot/Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyShaders.dll | f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c |
| R/StackOBot/Plugins/CaptureBench/Binaries/Win64/UnrealEditor-CaptureBench.dll | 89032f62c2ff43f9f55cbce60a3abd8e68378287bf8ec7fad24f3cec0c11d7f6 |
| H/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe | 6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861 |
| H/Binaries/Win64/StylizedParisStreet.exe | a2db7a028ebdbfc9d5d74600a35a2951421f8def703ca90f5e307ed460eeb1ac |
| H/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll | 9458cb952db4c24adc13211118222f1754a7aacaedbc9bd027505a2a382f824f |
| H/Binaries/Win64/UE4Editor-StylizedParisStreet.dll | 8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c |
| H/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | 2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940 |
| H/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | 9627b803fdb4e94c4e80678176c7478435c092b1c7f993b3024cc936bb50c36c |
| H/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | 0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b |
| H/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | 9c9bb82daf01ed85900b6794d75570ea38d447e911285c989153e5ec01700f0a |
| H/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | c3db764fc6566f01ca1f425d3bde0469ca8d1ae88ceb01bfb8f5a82f5e8cfd96 |
| E/Binaries/DotNET/UnrealBuildTool.exe | 07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0 |
| E/Binaries/ThirdParty/Python/Win64/python.exe | 52b4dead44802e263f54881d434f6dec5eab9ff3bd69a9d0c1408065543c3e55 |
| E/Binaries/Win64/CrashReportClientEditor.exe | ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7 |
| E/Binaries/Win64/UE4Editor.exe | 4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387 |
| E/Plugins/Editor/EditorScriptingUtilities/Binaries/Win64/UE4Editor-EditorScriptingUtilities.dll | 39800186398b4f7de8fc5c7b29e954f0d31c9fa3341244c06b25d9cbf00fe5b1 |
| E/Plugins/Experimental/PythonScriptPlugin/Binaries/Win64/UE4Editor-PythonScriptPlugin.dll | b62d70e6c367674cc21ad8d294a63e32788074cd967fa89eba514709b2a91378 |

Every compressed receipt archive path, relative to `D:/IA_Europa/Receipts/09426d/` unless absolute (each hash is in the inventory above):

| Archive path |
| --- |
| fixture-attempts/EA_Core/01/ledger/fixture-preflight/after-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-preflight/before-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-preflight/classified-changes.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-runtime/after-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-runtime/before-delta.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/fixture-runtime/classified-changes.jsonl.gz |
| fixture-attempts/EA_Core/01/ledger/initial.sqlite.gz |
| fixture-attempts/EA_Travel/01/ledger/fixture-preflight/after-delta.jsonl.gz |
| fixture-attempts/EA_Travel/01/ledger/fixture-preflight/before-delta.jsonl.gz |
| fixture-attempts/EA_Travel/01/ledger/fixture-preflight/classified-changes.jsonl.gz |
| fixture-attempts/EA_Travel/01/ledger/fixture-runtime/after-delta.jsonl.gz |
| fixture-attempts/EA_Travel/01/ledger/fixture-runtime/before-delta.jsonl.gz |
| fixture-attempts/EA_Travel/01/ledger/fixture-runtime/classified-changes.jsonl.gz |
| fixture-attempts/EA_Travel/01/ledger/initial.sqlite.gz |
| ledger/documentation/after-delta.jsonl.gz |
| ledger/documentation/before-delta.jsonl.gz |
| ledger/documentation/classified-changes.jsonl.gz |
| ledger/entry/after-delta.jsonl.gz |
| ledger/entry/before-delta.jsonl.gz |
| ledger/entry/classified-changes.jsonl.gz |
| ledger/exit/after-delta.jsonl.gz |
| ledger/exit/before-delta.jsonl.gz |
| ledger/exit/classified-changes.jsonl.gz |
| ledger/initial.sqlite.gz |
| ledger/tooling/after-delta.jsonl.gz |
| ledger/tooling/before-delta.jsonl.gz |
| ledger/tooling/classified-changes.jsonl.gz |

VERDICT: GREEN | Both A6 recipes authored fresh with clean exits and three declared packages; FF2 bookkeeping recorded, protection and canary checks passed.
