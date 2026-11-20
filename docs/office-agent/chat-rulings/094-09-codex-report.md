<!-- Collected by Claude Code (mailbox brief 094-09) on 2026-10-25 from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md (sha256 7411BBFCAEA7B579A3B22250D52999A5187582E5FB6649409AA23DED2AEA8461, 35,672 B); thread 01a14334, gpt-6-astra / max, 2026-10-24T22:24:29 to 2026-10-25T04:16:33 IST, 21,123 s, exit 0, 1 try. Content below is Codex's report, unchanged. -->
# 094-09 - Europa A4b report

**A4b is complete.** Release control **d7eee97** and candidate **1c4cce7** pass 5.1 Editor and monolithic Game Development. Candidate Shipping excludes IA and probe code. R1 warning, generated-definition and tick-pin evidence passes; the real strict pass and exact-restoration rebuilds pass on both targets.

- Candidate production source and descriptor remain identical to **489c828**. Result HEAD **d30cbe0** contains only the final documentation/reach update.
- All builds use **MSVC 14.38.33130 / product 14.38.33144**, SDK **10.0.22621.0**, `-NoEngineChanges` and `-MaxParallelActions=4`. Engine and engine-plugin tree actions: **0**. Engine-source work writes only into owned hosts.
- No new or worse candidate warnings. Shipping has no IA/probe action, BuildProducts receipt entry or new compiled product. Strict covers **77 headers + 64 ordinary sources + native UHT code** per target, with plugin PCH/unity off.
- P9 v2 entry/exit and EA11 pass. C: charge **854,797 B**; E: **0 B**; EG1 bookkeeping **6,225,337 B**. No floor breach or unresolved STOP.
- A4a's **27-file** 4.25 manifest remains exact for A6-A8. A5 probe behavior and all runtime/staged/delivery gates remain pending. No runtime launch, cook, UAT, push, tag, global/per-user change or foreign process stop.

Evidence: [09409 receipts](<D:/IA_Europa/Receipts/09409>). Final artifacts: [Baseline51](<D:/IA_Europa/Receipts/09409/qualification/final-baseline51-binaries.json>) and [ReGate51](<D:/IA_Europa/Receipts/09409/qualification/final-regate51-binaries.json>).

## Entry, exit and toolchains

P9 v2 was re-derived byte for byte from v1 and the approved removal manifest. The retained manifest is 472,552 lines, SHA-256 `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`; the 235,223 removed lines hash to `dd970a071b603e8a08cd38d69cecdea31e0b3ebc111b7ed29584b91a7764d063`. The approved cache deletion was completed before this job. Deleted roots are outside v2; retained release binaries, junction targets, source, refs, indices and protected worktrees remain covered. Five deleted worktree registrations remain prunable and were not pruned.

| Proof | Inventory paths | Selected full hashes | Bytes hashed | Protected git/source |
| --- | --- | --- | --- | --- |
| [entry](<D:/IA_Europa/Receipts/09409/P9/entry/summary.json>) | 472553 | 23033 | 104,794,989,456 | equal |
| [exit](<D:/IA_Europa/Receipts/09409/P9/exit/summary.json>) | 472553 | 23033 | 104,794,989,456 | equal |

The full baseline was not re-hashed. The inventory admits the already-ruled EG1 bookkeeping plus this job's receipted bookkeeping. Original Paris and original host sources/config/content stayed unchanged. Connectivity and prior-ledger integrity checks passed.

Read-only inspection found VS Community 2022 17.12.35707.178 and Build Tools 2017 15.9.34031.82; MSVC directories 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023, with no 14.44. SDKs 10.0.17763.0 and 10.0.22621.0 remain installed. Fresh 5.1 JsonExport and actual compiler/response evidence selected 14.38.33130, compiler product 14.38.33144 and SDK 10.0.22621.0. The 4.25 toolset remains 14.16.27051 in directory 14.16.27023. No installation, repair or fallback occurred.

## Scoped tooling, controls and cleaning

EI6 is implemented through a hashed per-job configuration: job/temporary roots, the five permitted build pairs, strict runs, the single Baseline51 Editor query, memory thresholds, parallelism and the 14-hour boundary. All build entry points delegate to that policy. Build/ledger evidence stays under Receipts/09409 and Temp/09409.

Synthetic tests cover EI2 admission boundaries, consecutive low-commit stops, physical-only pressure, timeout and owned-PID stop handling; wrong target/config/toolset/SDK, engine-tree output and dependency writes; missing actual responses/coverage; PCH and unity; wrong feature and tick-pin definitions; and cumulative/delta-stored C/E budgets, one-byte overruns and new root folders. No can-fail created a C:/E: folder. Native 5.1 response-only actions, monolithic definition locations, module provenance and UHT short names have explicit controls. Code can reproduce them in the recorded D: test folders.

Controls: [memory/bookkeeping](<D:/IA_Europa/Receipts/09409/self-tests/memory-bookkeeping/results.json>), [A4b/native names](<D:/IA_Europa/Receipts/09409/self-tests/native-short-names/results.json>), [strict native reader](<D:/IA_Europa/Receipts/09409/self-tests/strict-native-schema/result.json>), [budgets](<D:/IA_Europa/Receipts/09409/self-tests/budgets/results.json>), [delta budgets](<D:/IA_Europa/Receipts/09409/self-tests/delta-budgets/results.json>), [environment](<D:/IA_Europa/Receipts/09409/self-tests/environment/results.json>).

ReGate51 moved to the committed candidate and was cleaned before its Editor build: **171 files**, **43 directories**, **514,823,734 B**. Each file was hashed before removal; resolved paths and reparse checks confined deletion to ReGate51's host Binaries/Intermediate Build and AnomalyInjector/CaptureBench build outputs. Probe outputs were included. Baseline51 and engine outputs were not cleaned.

Cleaning receipt: [manifest and result](<D:/IA_Europa/Receipts/09409/cleaning/ReGate51-before-qualification-result.json>).

## Build commands and results

Each build ran sequentially through the scoped wrapper and ledger. The executable was `D:/UESource/UnrealEngine/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe`. Each command used its named target, `Win64`, configuration and host `.uproject`, with:

```text
-Log=<build-receipt>/UBT.log -NoEngineChanges -NoBuildUHT
-NoHotReload -NoHotReloadFromIDE -NoXGE -MaxParallelActions=4 -Verbose
```

The preceding action query appended `-WriteOutdatedActions=<build-receipt>/actions-1.json`. Include controls used the recorded disposable `-SingleFile` path. Strict temporarily patched only candidate plugin rules. Full literal argv/Windows command lines, response hashes, toolchain lines, `/std`, PCH flags and produced binary hashes are in every linked result and the consolidated index.

| Receipt/target leg | SHA | Project/plugin/engine-source-in-host/tree | Executed | Exit | Status | Wrapper seconds | Errors/warnings |
| --- | --- | --- | --- | --- | --- | --- | --- |
| [builds/baseline-editor](<D:/IA_Europa/Receipts/09409/builds/baseline-editor/result.json>) | d7eee97 | 9/20/0/0 | 29 | 0 | PASS | 203.2 | 0/2 |
| [builds/baseline-game](<D:/IA_Europa/Receipts/09409/builds/baseline-game/result.json>) | d7eee97 | 217/0/548/0 | 0 | - | REFUSED before compile | 77.7 | 0/0 |
| [builds/baseline-game-2](<D:/IA_Europa/Receipts/09409/builds/baseline-game-2/result.json>) | d7eee97 | 52/7/706/0 | 765 | 0 | PASS | 4603.7 | 0/2 |
| [builds/candidate-editor](<D:/IA_Europa/Receipts/09409/builds/candidate-editor/result.json>) | 1c4cce7 | 9/20/0/0 | 29 | 0 | PASS | 211.3 | 0/2 |
| [builds/candidate-game](<D:/IA_Europa/Receipts/09409/builds/candidate-game/result.json>) | 1c4cce7 | 52/7/706/0 | 765 | 0 | PASS | 4656.3 | 0/2 |
| [builds/candidate-shipping](<D:/IA_Europa/Receipts/09409/builds/candidate-shipping/result.json>) | 1c4cce7 | 37/1/668/0 | 706 | 0 | PASS | 3617.9 | 0/2 |
| [strict/attempt1/include_positive](<D:/IA_Europa/Receipts/09409/strict/attempt1/include_positive/result.json>) | 1c4cce7 | 0/1/0/0 | 1 | 0 | PASS | 123.5 | 0/0 |
| [strict/attempt1/include_negative](<D:/IA_Europa/Receipts/09409/strict/attempt1/include_negative/result.json>) | 1c4cce7 | 0/1/0/0 | 1 | 6 | EXPECTED_FAILURE | 86.5 | 1/0 |
| [strict/attempt1/strict_StackOBotEditor](<D:/IA_Europa/Receipts/09409/strict/attempt1/strict_StackOBotEditor/result.json>) | 1c4cce7 | 0/163/0/0 | 163 | 0 | PASS | 725.3 | 0/0 |
| [strict/attempt1/strict_StackOBot](<D:/IA_Europa/Receipts/09409/strict/attempt1/strict_StackOBot/result.json>) | 1c4cce7 | 2/152/0/0 | 154 | 0 | PASS | 681.1 | 0/0 |
| [strict/attempt1/normal_StackOBotEditor](<D:/IA_Europa/Receipts/09409/strict/attempt1/normal_StackOBotEditor/result.json>) | 1c4cce7 | 0/11/0/0 | 0 | - | REFUSED before compile | 57.4 | 0/0 |
| [strict/restoration-retry/normal_StackOBotEditor](<D:/IA_Europa/Receipts/09409/strict/restoration-retry/normal_StackOBotEditor/result.json>) | 1c4cce7 | 0/17/0/0 | 17 | 0 | PASS | 174.9 | 0/0 |
| [strict/restoration-retry/normal_StackOBot](<D:/IA_Europa/Receipts/09409/strict/restoration-retry/normal_StackOBot/result.json>) | 1c4cce7 | 2/6/0/0 | 8 | 0 | PASS | 215.6 | 0/0 |
| [builds/clean-engine-query](<D:/IA_Europa/Receipts/09409/builds/clean-engine-query/result.json>) | d7eee97 | 0/0/0/0 | 0 | 0 | PASS | 43.4 | 0/0 |

[Complete build, command, flag and binary-hash index](<D:/IA_Europa/Receipts/09409/qualification/all-builds.json>). Counts are admitted actions; Executed comes from UBT action lines. Wrapper time includes queries and admission waits. Predicted include-negative failures and the pre-compile reader failure remain visible.

## R1 warnings, defines, tick pin and Shipping

| Target | Code | Location | Category | Baseline | Candidate | Cause |
| --- | --- | --- | --- | --- | --- | --- |
| editor | C4996 | `<host>/Source/StackOBot/MidReproActor.cpp:110` | compiler_or_build | 1 | 1 | Existing host fixture uses SetMasterPoseComponent, deprecated in 5.1 in favor of SetLeaderPoseComponent; host sources are byte-identical. |
| editor | C4996 | `<host>/Source/StackOBot/MidReproActor.cpp:153` | compiler_or_build | 1 | 1 | Existing host fixture uses SetMasterPoseComponent, deprecated in 5.1 in favor of SetLeaderPoseComponent; host sources are byte-identical. |
| game | C4996 | `<host>/Source/StackOBot/MidReproActor.cpp:110` | compiler_or_build | 1 | 1 | Existing host fixture uses SetMasterPoseComponent, deprecated in 5.1 in favor of SetLeaderPoseComponent; host sources are byte-identical. |
| game | C4996 | `<host>/Source/StackOBot/MidReproActor.cpp:153` | compiler_or_build | 1 | 1 | Existing host fixture uses SetMasterPoseComponent, deprecated in 5.1 in favor of SetLeaderPoseComponent; host sources are byte-identical. |

The comparison normalizes only the two host roots and retains diagnostic code, file:line, category, text and multiplicity. No warning was suppressed or hidden. The raw collection flag new_port_warning is conservative; the baseline join in warnings-comparison.json determines whether a warning is actually new or worse. Full occurrences remain in qualification/warnings.json.

Generated definitions are archived and hashed from the actual target outputs, and their forced-include references are joined to compile responses. Capture, ControlServer and Shaders are 1 in both non-Shipping controls and candidates; candidate bench/profile macros are 1. The release predates those explicit bench macros: its enabled AnomalyBench module, dependencies and compiled fixtures are the control, as stated in the frozen addendum before UBT. ANOMINJECT_FW_TICKPIN equals baseline. This is compile evidence; capture/control runtime positives remain later gates.

| Target | Build | Tick pin | Generated values |
| --- | --- | --- | --- |
| editor | baseline | ['0'] | {"ANOMALY_CAPTURE": ["1"], "ANOMALY_CONTROL_SERVER": ["1"], "ANOMALY_SHADERS": ["1"], "ANOMINJECT_FW_TICKPIN": ["0"]} |
| editor | candidate | ['0'] | {"ANOMALY_BENCH": ["1"], "ANOMALY_CAPTURE": ["1"], "ANOMALY_CONTROL_SERVER": ["1"], "ANOMALY_PROFILE_BENCH": ["1"], "ANOMALY_SHADERS": ["1"], "ANOMINJECT_FW_TICKPIN": ["0"]} |
| game | baseline | ['0'] | {"ANOMALY_CAPTURE": ["1"], "ANOMALY_CONTROL_SERVER": ["1"], "ANOMALY_SHADERS": ["1"], "ANOMINJECT_FW_TICKPIN": ["0"]} |
| game | candidate | ['0'] | {"ANOMALY_BENCH": ["1"], "ANOMALY_CAPTURE": ["1"], "ANOMALY_CONTROL_SERVER": ["1"], "ANOMALY_PROFILE_BENCH": ["1"], "ANOMALY_SHADERS": ["1"], "ANOMINJECT_FW_TICKPIN": ["0"]} |

[Generated definitions and response references](<D:/IA_Europa/Receipts/09409/qualification/defines-and-tick-pin.json>).

Shipping's plan, actual action list, target BuildProducts and newly written compiled products contain no IA or EuropaPhaseAProbe module. The single plugin action is the separate CaptureBench fixture: its original descriptor has no Shipping blacklist. The ruled original plugin set remains unchanged; this bench-host build is not a delivery package, and fixture removal remains required before delivery. Existing Development files beside Shipping are excluded from the Shipping product denominator. AnomalyInjector.uplugin may remain as a descriptor staging dependency; packaged/content exclusion remains A11a. Baseline51 Shipping was not built.

[Shipping proof](<D:/IA_Europa/Receipts/09409/builds/candidate-shipping/shipping-exclusion.json>).

## Strict includes and restoration

| Target | Exit | Errors | Warnings | Header TUs | Ordinary + header sources | Lever findings |
| --- | --- | --- | --- | --- | --- | --- |
| StackOBotEditor | 0 | 0 | 0 | 77 | 141 | 0 |
| StackOBot | 0 | 0 | 0 | 77 | 141 | 0 |

Each target covers 152 C++ compiles: 64 ordinary sources, 77 independent headers and 11 UHT-generated sources. Plugin responses have no PCH creation/use and no unity source; generated headers and private headers are included. Archived response files and UHT manifests determine coverage. Zero coverage, a missing response or a missing generated action rejects. qualification/strict-coverage.json records the exact source list and counts.

The real positive UStaticMeshComponent member-call TU compiled. Removing only Components/StaticMeshComponent.h failed at that disposable source with the predicted incomplete-type/member error. PCH and unity synthetic mutants also reject. All five Build.cs files restore exact original bytes and hashes; generated test sources are removed by receipt and tracked status restores exactly. Both normal Editor/Game rebuilds then pass. Each normal rebuild has six fresh C++ actions with normal PCH use; the five definitions files referenced by those actions contain no strict marker. Unreferenced strict intermediate headers remain recorded separately. qualification/normal-restoration-flags.json binds that check. Temporary strict changes did not become permanent C#/descriptor changes.

The first normal Editor action plan retained the pre-strict normal unity objects and scheduled only eleven relink/metadata actions. The unchanged zero-C++ guard refused it before execution. The strict compiles and restoration remained valid. One recovery hashed and removed twelve cached normal IA object files (six per target), selected from the qualified native action lists and confined to ReGate51; synthetic path controls rejected other hosts, engines, Shipping and non-object outputs. Fresh normal Editor and Game builds then passed. No predicate was weakened, no strict result was overwritten, and no production source changed. The joined summary records the original summary hash and both recovery receipts. This resolved the second harness problem within D2.

[Joined strict summary](<D:/IA_Europa/Receipts/09409/strict/restoration-retry/summary.json>); [Missing-include failure](<D:/IA_Europa/Receipts/09409/strict/attempt1/include_negative/result.json>); [Exact restoration](<D:/IA_Europa/Receipts/09409/strict/attempt1/restoration.json>); [Normal-object cleanup](<D:/IA_Europa/Receipts/09409/strict/restoration-retry/cleanup-before.json>).

## Changes, compiler evidence and final artifacts

Production Source, descriptor and host sources/projects are unchanged from A4a: **B for this job**. No 4.25 recompile was required; all 27 A4a artifacts re-hash equal for A6-A8. No permanent C#/descriptor/.uproject change triggered another full JsonExport/mutant set. Fresh 5.1 selection and exact strict-rule restoration are recorded.

Tooling is **C** (changed, intended neutral to runtime): job_scope/run_job parameterize roots and allow-lists; build_memory applies EI2; write_ledger records the scoped policy; a4_build/process_wrapper/working_build enforce commit-bound entry points and classify native monolithic output/provenance; a4_evidence/a4b_evidence read compiler flags, generated definitions and tick pins; recovery_cleanup confines stopped-output cleanup; strict_include_build reads 5.1 native response actions; the self-test files exercise the controls. Run-folder recovery/verification/report helpers are C and retained with receipts; recover_strict_normal09409.py records its own hash and exact cleanup plan. Predictions/status/session/result/reach documents and appended G462-G463 are C. No new production-file reach was introduced.

The first baseline Game query succeeded, but its reader expected definitions under plugin Intermediate. Monolithic definitions were under host Intermediate, so the wrapper stopped before compiling. The failed receipt remains. One correction added both native locations, followed by synthetic and retained-plan revalidation. Native UHT ShortName aliases corrected four generated-module classifications before the retry. The repeated baseline Game build uses the corrected classification. No compiler-error fix attempt was consumed.

[Reach manifest](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/phase-a-reach.json>); [A4a artifacts unchanged](<D:/IA_Europa/Receipts/09409/qualification/A4a-artifacts-unchanged.json>).

| Build | C/C++ responses | /std counts | PCH responses | Compiler product | SDK |
| --- | --- | --- | --- | --- | --- |
| baseline_editor | 12 | ['/std:c++17']:12 | 12 | 14.38.33144.0 | 10.0.22621.0 |
| baseline_game | 654 | ['/std:c++17']:652; []:2 | 650 | 14.38.33144.0 | 10.0.22621.0 |
| candidate_editor | 12 | ['/std:c++17']:12 | 12 | 14.38.33144.0 | 10.0.22621.0 |
| candidate_game | 654 | ['/std:c++17']:652; []:2 | 650 | 14.38.33144.0 | 10.0.22621.0 |
| shipping | 609 | ['/std:c++17']:608; []:1 | 606 | 14.38.33144.0 | 10.0.22621.0 |

The responses without /std compile C sources: MiMalloc.c and xxhash.c in both Development Game builds, and MiMalloc.c in Shipping. All C++ responses use /std:c++17. This preserves the 5.1 default policy; no C++ standard override was added.


Final artifacts below bind A9-A11 after strict restoration. The complete manifests also contain symbols, module manifests, target receipts and runtime dependencies. Per-build binary hashes, including superseded products, remain in the linked build index; no binary archive or staging/install swap was made.


[baseline51 full manifest](<D:/IA_Europa/Receipts/09409/qualification/final-baseline51-binaries.json>) (39 files).

| Final binary | Bytes | SHA-256 |
| --- | --- | --- |
| boost_atomic-vc142-mt-x64-1_70.dll | 9728 | `ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765` |
| boost_chrono-vc142-mt-x64-1_70.dll | 35840 | `17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26` |
| boost_iostreams-vc142-mt-x64-1_70.dll | 50688 | `7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441` |
| boost_program_options-vc142-mt-x64-1_70.dll | 396288 | `dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5` |
| boost_python39-vc142-mt-x64-1_70.dll | 195072 | `9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df` |
| boost_regex-vc142-mt-x64-1_70.dll | 824832 | `387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64` |
| boost_system-vc142-mt-x64-1_70.dll | 9728 | `805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4` |
| boost_thread-vc142-mt-x64-1_70.dll | 84480 | `bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48` |
| D3D12Core.dll | 5247408 | `40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2` |
| d3d12SDKLayers.dll | 9217448 | `1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052` |
| OpenImageDenoise.dll | 49771408 | `11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a` |
| StackOBot.exe | 242784256 | `4e69082a23f443fe4f8b87a24a13018f0d129db994a8c2cbd02da3e53b19ae1b` |
| tbb.dll | 383488 | `6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f` |
| tbb12.dll | 293376 | `de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d` |
| UnrealEditor-EuropaPhaseAProbe.dll | 43008 | `e47152170bb22d823063ca652174447ff804d32ea1a0e06d4a27c8954ab9b1eb` |
| UnrealEditor-StackOBot.dll | 137728 | `4092eb194804229dd2e4e85dec4c50eb96cefd03491b5f7038c084365e43ec19` |
| UnrealEditor-AnomalyBench.dll | 155648 | `6a8a770188e44660e0d0ba790d14515b600df498eb657bd24af07245dee211d8` |
| UnrealEditor-AnomalyCapture.dll | 2054656 | `9ab3578e0cdab25d9cacf93b226d8d54af2acfda6af0631c5da76f3d5eed9d94` |
| UnrealEditor-AnomalyControlServer.dll | 259072 | `9a41d9a8cb649f0dbd962082ea0b535f6162b2f611146b20d202f2fd457a4b57` |
| UnrealEditor-AnomalyInjector.dll | 1758720 | `91d14a4af7711c3563dccb30589d013648d9a23b5a7f33563e67f0d06443974e` |
| UnrealEditor-AnomalyShaders.dll | 68096 | `284c64cd7936ea260e780a1a010916a72312a77acb2282a0d413b4196b584b0d` |
| UnrealEditor-CaptureBench.dll | 309248 | `e2ef940d6df3ca2d8075015498da127f32540df20b844d1c23e9ffe1de3ce5d8` |

[regate51 full manifest](<D:/IA_Europa/Receipts/09409/qualification/final-regate51-binaries.json>) (44 files).

| Final binary | Bytes | SHA-256 |
| --- | --- | --- |
| boost_atomic-vc142-mt-x64-1_70.dll | 9728 | `ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765` |
| boost_chrono-vc142-mt-x64-1_70.dll | 35840 | `17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26` |
| boost_iostreams-vc142-mt-x64-1_70.dll | 50688 | `7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441` |
| boost_program_options-vc142-mt-x64-1_70.dll | 396288 | `dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5` |
| boost_python39-vc142-mt-x64-1_70.dll | 195072 | `9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df` |
| boost_regex-vc142-mt-x64-1_70.dll | 824832 | `387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64` |
| boost_system-vc142-mt-x64-1_70.dll | 9728 | `805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4` |
| boost_thread-vc142-mt-x64-1_70.dll | 84480 | `bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48` |
| D3D12Core.dll | 5247408 | `40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2` |
| d3d12SDKLayers.dll | 9217448 | `1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052` |
| OpenImageDenoise.dll | 49771408 | `11213c268e6c67262d48e7351e1f6c196acbb91a9a1cf059d4d758c34368f08a` |
| StackOBot-Win64-Shipping.exe | 108477952 | `010ab98e78e1a4a04c4dc1fb3edf3841336b3bc6f4d8a6db5be24841b6845fb4` |
| StackOBot.exe | 242792448 | `2422f0872a12e3433a80db9391fec9aa7b607a0e4b02cb7685c66696e6db7c2e` |
| tbb.dll | 383488 | `6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f` |
| tbb12.dll | 293376 | `de25bc26f71c5fa53b585116db11d643dc63373fab094e5d786a2ef7eb50786d` |
| UnrealEditor-EuropaPhaseAProbe.dll | 43008 | `d05546c99b4cd04cc12a0530f8375f380363a1cc375c73ed1c67b6294ea5710f` |
| UnrealEditor-StackOBot.dll | 137728 | `741b766316baa00dac0a63631dd407b7262ce47fb5f3c677f8d135054117cc0f` |
| UnrealEditor-AnomalyBench.dll | 155136 | `128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c` |
| UnrealEditor-AnomalyCapture.dll | 2054656 | `0e14f06a515844ad27174789fe455284c798a64e8fffe3a8bd12f251e4f162e6` |
| UnrealEditor-AnomalyControlServer.dll | 259072 | `3e740f3b9bcf76cc060a25e3eb6602e5a34b375c9e336b86bd065dac129a8a80` |
| UnrealEditor-AnomalyInjector.dll | 1772544 | `88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5` |
| UnrealEditor-AnomalyShaders.dll | 68096 | `f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c` |
| UnrealEditor-CaptureBench.dll | 309248 | `89032f62c2ff43f9f55cbce60a3abd8e68378287bf8ec7fad24f3cec0c11d7f6` |

EA11 passes on the final reflected surface and rejects its mutant. [EA11 result](<D:/IA_Europa/Receipts/09409/qualification/EA11.json>).

## Memory and operation ledgers

EI2 uses >=16 GiB free commit and >=4 GiB physical for three polls about 20 seconds apart. In-run stop is commit below 6 GiB on two consecutive five-second samples; physical pressure alone does not stop. The four-hour wait limit and owned-tree stop/receipted-cleanup rules remain in force. MaxParallelActions=4 applies to every build; UBT may lower that cap.

Recorded admissions: **25**. Sum of admission waits: **1000.4 s**; longest individual wait **40.1 s**. Non-routine memory events: **0**; details are retained in the memory index. No foreign process was stopped.

| Operation/log | Admission UTC | Physical GiB | Commit GiB | Wait seconds |
| --- | --- | --- | --- | --- |
| 04-baseline-editor/child-006-memory.jsonl | 2026-10-24T17:18:26.007164+00:00 | 6.26 | 25.89 | 40.0 |
| 04-baseline-editor/child-007-memory.jsonl | 2026-10-24T17:19:11.270051+00:00 | 6.91 | 27.59 | 40.0 |
| 05-baseline-game/child-006-memory.jsonl | 2026-10-24T17:25:04.546978+00:00 | 8.49 | 34.63 | 40.0 |
| 07-baseline-game-retry/child-014-memory.jsonl | 2026-10-24T17:35:19.779048+00:00 | 7.05 | 31.18 | 40.0 |
| 07-baseline-game-retry/child-015-memory.jsonl | 2026-10-24T17:36:17.078117+00:00 | 8.42 | 34.89 | 40.0 |
| 09-candidate-editor/child-007-memory.jsonl | 2026-10-24T18:59:44.584873+00:00 | 9.62 | 28.85 | 40.0 |
| 09-candidate-editor/child-008-memory.jsonl | 2026-10-24T19:00:44.094972+00:00 | 9.45 | 28.77 | 40.0 |
| 10-candidate-game/child-007-memory.jsonl | 2026-10-24T19:06:32.699861+00:00 | 10.67 | 34.00 | 40.0 |
| 10-candidate-game/child-008-memory.jsonl | 2026-10-24T19:07:46.078106+00:00 | 10.51 | 33.99 | 40.0 |
| 11-candidate-shipping/child-007-memory.jsonl | 2026-10-24T20:28:44.266529+00:00 | 9.50 | 30.11 | 40.1 |
| 11-candidate-shipping/child-008-memory.jsonl | 2026-10-24T20:29:48.792587+00:00 | 9.68 | 29.87 | 40.0 |
| 12-strict/child-006-memory.jsonl | 2026-10-24T21:32:50.236956+00:00 | 11.81 | 34.14 | 40.0 |
| 12-strict/child-007-memory.jsonl | 2026-10-24T21:33:41.736586+00:00 | 11.67 | 34.05 | 40.0 |
| 12-strict/child-011-memory.jsonl | 2026-10-24T21:35:13.672318+00:00 | 11.61 | 34.00 | 40.0 |
| 12-strict/child-012-memory.jsonl | 2026-10-24T21:35:56.662495+00:00 | 11.23 | 34.50 | 40.0 |
| 12-strict/child-016-memory.jsonl | 2026-10-24T21:37:01.628256+00:00 | 11.68 | 34.65 | 40.0 |
| 12-strict/child-017-memory.jsonl | 2026-10-24T21:37:52.333182+00:00 | 9.99 | 29.99 | 40.0 |
| 12-strict/child-021-memory.jsonl | 2026-10-24T21:49:28.442416+00:00 | 11.79 | 34.72 | 40.0 |
| 12-strict/child-022-memory.jsonl | 2026-10-24T21:50:38.664021+00:00 | 10.04 | 29.79 | 40.0 |
| 12-strict/child-026-memory.jsonl | 2026-10-24T22:01:13.236476+00:00 | 12.16 | 34.51 | 40.0 |
| 13-strict-normal-recovery/child-008-memory.jsonl | 2026-10-24T22:08:11.493831+00:00 | 9.08 | 29.60 | 40.0 |
| 13-strict-normal-recovery/child-009-memory.jsonl | 2026-10-24T22:08:54.750832+00:00 | 11.24 | 34.27 | 40.0 |
| 13-strict-normal-recovery/child-013-memory.jsonl | 2026-10-24T22:11:28.689747+00:00 | 11.08 | 34.52 | 40.0 |
| 13-strict-normal-recovery/child-014-memory.jsonl | 2026-10-24T22:12:22.014817+00:00 | 11.10 | 33.81 | 40.0 |
| 14-clean-engine-query/child-006-memory.jsonl | 2026-10-24T22:19:26.978305+00:00 | 9.37 | 29.94 | 40.0 |

[Memory log index and hashes](<D:/IA_Europa/Receipts/09409/qualification/memory-index.json>). All in-run samples, waits and any event details are retained.

Every operation ledger passed. Resolved-volume free space is shown before -> after, in GiB. Port D delta is logical inventory change, not physical free-space attribution. Foreign writes and orchestration overhead are recorded separately. Exception paths have before/after inventories, delta snapshot hashes and command/environment redirect receipts.

| Operation | C GiB | D GiB | E GiB | D delta MiB | C/E/EG1 bytes | Foreign changes | Tool overhead MiB |
| --- | --- | --- | --- | --- | --- | --- | --- |
| [01-entry](<D:/IA_Europa/Receipts/09409/ledger/01-entry/summary.json>) | 18.70 -> 18.70 | 133.30 -> 132.98 | 271.84 -> 271.83 | 120.79 | 0/0/0 | 25 | 8.76 |
| [02-tooling](<D:/IA_Europa/Receipts/09409/ledger/02-tooling/summary.json>) | 18.70 -> 18.70 | 132.98 -> 132.98 | 271.82 -> 271.82 | 0.37 | 0/0/0 | 2 | 0.22 |
| [03-preflight](<D:/IA_Europa/Receipts/09409/ledger/03-preflight/summary.json>) | 18.70 -> 18.70 | 132.98 -> 132.98 | 271.82 -> 271.77 | 4.30 | 0/0/0 | 4 | 0.17 |
| [04-baseline-editor](<D:/IA_Europa/Receipts/09409/ledger/04-baseline-editor/summary.json>) | 18.70 -> 18.69 | 132.98 -> 132.53 | 271.63 -> 271.23 | 460.55 | 139340/0/1296969 | 5 | 3.74 |
| [05-baseline-game](<D:/IA_Europa/Receipts/09409/ledger/05-baseline-game/summary.json>) | 18.69 -> 18.69 | 132.53 -> 132.36 | 271.01 -> 270.87 | 152.84 | 0/0/0 | 1 | -7.15 |
| [06-monolithic-reader](<D:/IA_Europa/Receipts/09409/ledger/06-monolithic-reader/summary.json>) | 18.69 -> 18.69 | 132.36 -> 132.35 | 270.86 -> 270.86 | 2.33 | 0/0/0 | 1 | 0.47 |
| [07-baseline-game-retry](<D:/IA_Europa/Receipts/09409/ledger/07-baseline-game-retry/summary.json>) | 18.69 -> 18.77 | 132.35 -> 116.18 | 270.84 -> 268.40 | 16553.18 | 415452/0/0 | 6 | 24.49 |
| [08-clean-candidate](<D:/IA_Europa/Receipts/09409/ledger/08-clean-candidate/summary.json>) | 18.77 -> 18.77 | 116.18 -> 116.66 | 268.35 -> 268.30 | -490.85 | 0/0/0 | 0 | 3.00 |
| [09-candidate-editor](<D:/IA_Europa/Receipts/09409/ledger/09-candidate-editor/summary.json>) | 18.77 -> 18.77 | 116.66 -> 116.19 | 268.25 -> 268.06 | 492.43 | 87212/0/1913446 | 1 | 0.07 |
| [10-candidate-game](<D:/IA_Europa/Receipts/09409/ledger/10-candidate-game/summary.json>) | 18.77 -> 18.74 | 116.18 -> 99.85 | 267.96 -> 265.03 | 16691.54 | 167689/0/0 | 2 | 32.48 |
| [11-candidate-shipping](<D:/IA_Europa/Receipts/09409/ledger/11-candidate-shipping/summary.json>) | 18.74 -> 18.73 | 99.85 -> 86.81 | 265.00 -> 263.44 | 13352.91 | 43032/0/0 | 2 | 0.29 |
| [12-strict](<D:/IA_Europa/Receipts/09409/ledger/12-strict/summary.json>) | 18.73 -> 18.38 | 86.81 -> 85.34 | 263.44 -> 262.23 | 1491.76 | 0/0/1167215 | 3 | 10.70 |
| [13-strict-normal-recovery](<D:/IA_Europa/Receipts/09409/ledger/13-strict-normal-recovery/summary.json>) | 18.23 -> 18.72 | 85.34 -> 85.07 | 262.23 -> 261.61 | 275.93 | 2072/0/550738 | 3 | -1.04 |
| [14-clean-engine-query](<D:/IA_Europa/Receipts/09409/ledger/14-clean-engine-query/summary.json>) | 18.72 -> 18.72 | 85.07 -> 85.07 | 261.56 -> 261.46 | 1.86 | 0/0/1296969 | 1 | 2.93 |
| [15-final-evidence](<D:/IA_Europa/Receipts/09409/ledger/15-final-evidence/summary.json>) | 18.72 -> 18.72 | 85.07 -> 85.07 | 261.43 -> 261.43 | 2.56 | 0/0/0 | 4 | 0.26 |
| [16-exit](<D:/IA_Europa/Receipts/09409/ledger/16-exit/summary.json>) | 18.72 -> 18.70 | 85.07 -> 84.95 | 261.43 -> 261.43 | 120.79 | 0/0/0 | 11 | 6.34 |
| [17-results-commit](<D:/IA_Europa/Receipts/09409/ledger/17-results-commit/summary.json>) | 18.68 -> 18.65 | 84.95 -> 84.95 | 261.42 -> 261.42 | 0.01 | 0/0/0 | 1 | 1.27 |

Totals: C **854,797 B / 250 MB**, E **0 B / 50 MB**, EG1 **6,225,337 B / 100 MB**. Ledger evidence **230,141,415 B**, below 2 GiB. Start/warning/floor limits were C 13/11/10, D 70/55/50 and E 25/20/15 GiB. No floor breach occurred.

All port build outputs remained on D:. C: compile-child writes stayed within ruled exceptions; no new exception class was introduced. Tooling .codex resolves to E:/CodexData/home; .claude and C:/ClaudeTemp are on C: and measured as tooling overhead outside port budgets. Foreign Windows/owner changes are retained separately. The targeted scan retains OS/tooling enumeration denials explicitly; protected enumeration is error-fatal and had no errors. No ETW, USN or elevation was attempted.

| Operation | EG1 scope | Bookkeeping class | Paths | After bytes |
| --- | --- | --- | --- | --- |
| 04-baseline-editor | engine | ActionHistory.bin | 1 | 746231 |
| 04-baseline-editor | engine | Timestamp | 207 | 367363 |
| 04-baseline-editor | engine-plugin | Timestamp | 161 | 183375 |
| 09-candidate-editor | engine | ActionHistory.bin | 1 | 746231 |
| 09-candidate-editor | engine | LiveCodingInfo.json | 414 | 447896 |
| 09-candidate-editor | engine | Timestamp | 207 | 367363 |
| 09-candidate-editor | engine-plugin | LiveCodingInfo.json | 239 | 168581 |
| 09-candidate-editor | engine-plugin | Timestamp | 161 | 183375 |
| 12-strict | engine | LiveCodingInfo.json | 414 | 447896 |
| 12-strict | engine | Timestamp | 207 | 367363 |
| 12-strict | engine-plugin | LiveCodingInfo.json | 239 | 168581 |
| 12-strict | engine-plugin | Timestamp | 161 | 183375 |
| 13-strict-normal-recovery | engine | Timestamp | 207 | 367363 |
| 13-strict-normal-recovery | engine-plugin | Timestamp | 161 | 183375 |
| 14-clean-engine-query | engine | ActionHistory.bin | 1 | 746231 |
| 14-clean-engine-query | engine | Timestamp | 207 | 367363 |
| 14-clean-engine-query | engine-plugin | Timestamp | 161 | 183375 |

Every changed EG1 path has size, mtime and hash receipts. [Bookkeeping detail](<D:/IA_Europa/Receipts/09409/qualification/engine51-bookkeeping.json>). No engine/engine-plugin binary, generated C++, source or other forbidden tree write occurred.

## Clean-engine query, commits and handoff

The final Baseline51 StackOBotEditor Development query used NoEngineChanges and WriteOutdatedActions, with **0 engine and 0 engine-plugin actions**. It was action-plan-only: **0 executed actions**. Result: [query receipt](<D:/IA_Europa/Receipts/09409/builds/clean-engine-query/result.json>).

| Local feature commit | Author time | Committer time | Subject |
| --- | --- | --- | --- |
| 6fe993e6ed1aa8a723058b6c38d4c3be4ffaa5af | 2026-10-24T22:41:05+05:30 | 2026-10-24T22:41:05+05:30 | test(europa): scope 5.1 qualification and commit memory admission |
| 410acbcbceb4cda65b87b713a580b57e0a12c07e | 2026-10-24T22:44:23+05:30 | 2026-10-24T22:44:23+05:30 | test(europa): route build entry points through the job policy |
| b36ac4ae7773117fe71ba1d17c04b79d24b938bb | 2026-10-24T23:00:13+05:30 | 2026-10-24T23:00:13+05:30 | fix(europa): read monolithic definitions and module provenance |
| 1c4cce7023081c64f5c747688ea7ef8f69143f7b | 2026-10-24T23:04:30+05:30 | 2026-10-24T23:04:30+05:30 | fix(europa): classify generated modules through native short names |
| d30cbe03da39dabf69c8bda09b3dcdaec8314ae6 | 2026-10-25T04:09:20+05:30 | 2026-10-25T04:09:20+05:30 | docs(europa): record A4b baseline and candidate qualification |

All commits are local on feat/europa-port, with GIT_AUTHOR_DATE and GIT_COMMITTER_DATE absent. Nothing was pushed or tagged. The frozen A4b addendum preceded the first UBT invocation. Final result HEAD d30cbe03da39dabf69c8bda09b3dcdaec8314ae6 differs from qualified 1c4cce7023081c64f5c747688ea7ef8f69143f7b only in documentation/reach; ReGate51 remains detached at the compiled SHA and Baseline51 at d7eee97.

A5 must add the probe's behavior/readers/fixtures: it remains the empty A3 skeleton. A6-A8 must use A4a's unchanged exact 27-file manifest. A9-A11 use this job's final Baseline51/ReGate51 manifests after strict restoration. Build-level Shipping exclusion does not replace packaged extraction/load proof. Runtime world/tick/GC, pixel/label alignment, AA characterization, capture/control and owner legs remain pending. No runtime bench lever was changed; disposable strict settings and source controls restored exactly.

No STOP or NEEDS-DECISION item remains from A4b. Both pre-compile harness refusals were resolved and retained; no production fix or warning waiver was needed. A separate read-only evidence inspection was refused because it omitted the job configuration; repeating it through the job-scope shim passed, with no UBT or mutation involved. This report submits A4b evidence for chat's ruling and makes no delivery claim.

VERDICT: GREEN | A4b baseline/candidate builds, R1 and 5.1 strict complete on 1c4cce7; P9 v2 and ledgers pass; A4a artifacts unchanged; no push
