<!-- Collected by Claude Code (brief 094-06): run folder _relay\runs\2026-10-16-094-01-europa-phase-a-plan (resume of thread <uuid>) | model gpt-6-astra, effort max, sandbox danger-full-access (rollout turn_context 04:38:48Z on 2026-10-24) | 2026-10-24 10:05:43-10:42:21 IST, duration 2,198 s, 1 try | exit 0 | source report.md sha256 4970C154D4D43870CA92F391BCD096593856E4E11AAB4BEFAD2547DEF5E0C463, content unedited below -->
**094-06 — Europa A3 recovery report · 2026-10-24**

**NEEDS-DECISION. A3 stopped during EF2 entry recovery, before source edits, cleanup or a build.** The interrupted 5.1 run left **1,023 engine-tree differences**, including metadata outside ED4’s allowed paths/classes. All timestamps fall within the interrupted run. I preserved them and made no engine repair or exception change.

P9 v1 remains authoritative. Entry and exit each verified **25,866 protected binary/config SHA-256s**; all match. Protected refs, indexes, pause receipts and original host manifests match. The 1,023 metadata differences remain a STOP in both proofs; there are no additional protected differences during this job. Git connectivity and the unfinished SQLite ledger pass. `feat/europa-port` and ReGate51 remain at `5bd368b`; all three port checkouts are clean.

The earlier 4.25 compile log still records 0 errors and 0 C++ warnings. Its receipt did **not** contain binary output hashes, so historical output hash equality cannot be proved and EF2 requires a fresh compile. The 5.1 build remains interrupted, with eight first-header errors. **No final compile pair exists.** EF1, EF2 cleanup and the include fixes remain pending. No commit, push, runtime launch or toolset/settings change occurred in 094-06.

Full evidence root: `D:/IA_Europa/Receipts/09406`. Earlier receipts remain untouched. This report also records the unfinished 094-05 work because that run produced no report.

**The engine-write STOP and the decision needed**

The complete engine comparison covers 339,410 files, against 339,409 in the unfinished ledger. It found 1,022 changed files and one addition, totaling **1,913,531 B** after the writes. The earlier 622-file observation covered only `Engine/Intermediate/Build`; it omitted plugin intermediate folders and the UBT run marker. This is additional evidence, not a change to a ruling.

| Location and class | Files | Bytes after | Content equal to v1 | ED4 status |
|---|---:|---:|---:|---|
| Engine/Intermediate/Build: ActionHistory.bin | 1 | 746,231 | 0 | Named class; interrupted run did not satisfy the success condition |
| Engine/Intermediate/Build: LiveCodingInfo.json | 414 | 447,896 | 414 | Class not explicitly allowed |
| Engine/Intermediate/Build: UHT Timestamp | 207 | 367,363 | 135 | Named class; success condition unresolved |
| Engine/Plugins/**/Intermediate/Build: LiveCodingInfo.json | 239 | 168,581 | 239 | Outside allowed root |
| Engine/Plugins/**/Intermediate/Build: UHT Timestamp | 161 | 183,375 | 93 | Outside allowed root |
| Engine/Intermediate/UbtRuns/42980_aaa272f4fb41c5e47d20430ffecddefb | 1 | 85 | New file | Outside allowed root |

All 653 LiveCodingInfo files have identical bytes to v1. Of the 368 UHT Timestamp files, 228 have identical bytes and 140 differ. ActionHistory also differs. The marker contains the 5.1 UBT executable path and names the interrupted PID 42980. Modification times are **2026-10-22 17:05:55–17:06:04 IST**, preceding this recovery job. No engine binary, generated C++ output, Source, Content, Config, Shaders, Programs or other engine file difference was found.

ED4 says “Any other write in the 5.1 engine tree is still a STOP.” I did not reinterpret its root restriction, expand the ledger allowlist or treat an interrupted run as successful. Both P9 comparisons retain every difference without an exemption. [Exact path/size/mtime/hash comparison](<D:/IA_Europa/Receipts/09406/stop/engine-differences.json>).

The configured 5.1 UBT source explains why zero engine compile/link actions did not prevent these writes:

- [UEBuildModuleCPP.cs:745](<D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/Configuration/UEBuildModuleCPP.cs:745>) writes LiveCodingInfo whenever the target supports Live Coding. This does not show that a Live Coding session was launched.
- [ExternalExecution.cs:855](<D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/System/ExternalExecution.cs:855>) writes UHT Timestamp files for modules not marked read-only, including engine-plugin intermediate directories.
- [UnrealBuildTool.cs:596](<D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/UnrealBuildTool.cs:596>) creates the UbtRuns marker before mode execution; its `finally` removes it at line 710. Session termination explains the leftover. Normal successful invocations can create and remove this file between snapshots.
- [TargetRules.cs:1094](<D:/UESource/UnrealEngine/Engine/Source/Programs/UnrealBuildTool/Configuration/TargetRules.cs:1094>) marks `bWithLiveCoding` as requiring a unique build environment. Changing it is not an established safe routing fix for this prebuilt engine.

**Proposed ruling, not applied:** admit only these demonstrated metadata classes under both engine and engine-plugin intermediate roots, plus wrapper-PID UbtRuns markers; retain the 100 MB metadata cap, exact per-operation receipts, protected binary/config hashes, `-NoEngineChanges`, and zero engine/engine-plugin actions. The ruling also needs to state how metadata written before a compiler failure, EF1 stop or restart is handled. EF1 intentionally permits stopping a run, whereas ED4 currently requires success. Any stale marker deletion needs explicit authority because the current deletion scope is D:/IA_Europa only. I propose no engine or global-setting edit.

**Interruption and recovery**

Code’s event-log receipts identify an owner-initiated Start-menu restart at 17:11:32, followed by boot at 17:12:18. They record resource-exhaustion events and no kernel-crash/BugCheck evidence for this restart. Thus the current evidence supports session termination under memory pressure; the ruling’s earlier “probably crashed or froze” description was provisional. No leftover port build process was reported at launch, and I stopped no process.

| Recovery check | Result |
|---|---|
| Common-repository `git fsck --connectivity-only` | Exit 0; dangling objects reported, no connectivity error; no repair or pruning |
| Open ledger `09405/ledger/current.sqlite` | `PRAGMA integrity_check` = `ok`; 197,279,744 B; opened read-only |
| Other databases open at interruption | None in the operation directory; no journal/WAL/SHM residue recorded |
| Port worktrees | Paris feature and ReGate51: 5bd368b; Baseline51: d7eee97; all clean |
| 4.25 action outputs | 34/34 match the prior size/mtime inventory; current hashes saved; no historical binary hashes available |
| 5.1 working result | UHT passed; 29 actions admitted; logs show two compile starts; completion/action totals and final result unavailable |
| Lost proof | 5.1 final result, after-operation ledger, exit proof, final pair, report and exit code |
| EF2 cleanup | Not performed because entry STOP fired; the interrupted host outputs remain in place |

The 094-05 receipt hashes are hashes of compiler response files, not produced DLLs. Current output hashes are retained in [425-output-check.json](<D:/IA_Europa/Receipts/09406/stop/425-output-check.json>); I do not present them as 17:01 hashes. A future final 4.25 compile is required even if the include fixes happen to leave its binary inputs unchanged.

**Entry, exit, disk and write evidence**

No cleanup occurred, no v2 directory exists, and v2 re-derivation is inapplicable. The unchanged v1 manifest is `db884f1e9c3debd7b9ed678db08036509959b1fc5a0d27799b10ecccb0625cce`, 707,775 baseline entries. Entry and exit inventory 707,776 files, retaining the added UbtRuns marker. Each hashes 119,771,817,645 B across the required 25,866 binary/config files; the full 707,775-file hash was not repeated. Protected ref/index/status and pause comparisons pass, including J2, m51, master, release and tags. The original Paris manifest (1,511 files) and selected StackOBot source manifest (2,756 files) match their copy receipts.

The 4.25 install inventory is unchanged across all 179,502 files. The original Paris inventory is unchanged across 1,511 files. No current-job engine write was detected. P9 overall remains STOP solely for the inherited 5.1 metadata set. [Entry proof](<D:/IA_Europa/Receipts/09406/P9/entry/exit-summary.json>); [exit proof](<D:/IA_Europa/Receipts/09406/P9/exit/exit-summary.json>).

Resolved-volume entry headroom: C **19.36 GiB**, D **138.89 GiB**, E **53.81 GiB**. ED1 start/warning/floor limits remain C 13/11/10, D 70/55/50, E 25/20/15 GiB. No warning or floor was crossed in the recorded operations.

| Operation | C GiB before→after | D GiB before→after | E GiB before→after | D inventory growth, B | C/E/engine charge, B |
|---|---:|---:|---:|---:|---:|
| 01-stop-entry | 19.14→19.12 | 138.71→138.44 | 53.81→53.80 | 66204609 | 0/0/0 |
| 02-stop-exit | 19.12→19.12 | 138.44→138.44 | 53.80→53.80 | 8678 | 0/0/0 |
| 03-stop-exit-proof | 19.12→19.12 | 138.44→138.38 | 53.80→53.80 | 66307677 | 0/0/0 |

Operation 02’s evidence collector rejected a new documentation file absent from the older reach manifest. Its ledger passed; the child did not. The collector was corrected to classify docs separately from production files, and its second attempt in operation 03 passed. This consumed one harness fix; no compiler attempt was made. Entry bootstrap receipts precede the first full ledger inventory and contain only read-only recovery results on D:.

| Operation | .claude C: bytes before→after | .codex physical E: bytes before→after | C:/ClaudeTemp bytes before→after | Foreign / tooling changed rows |
|---|---:|---:|---:|---:|
| 01-stop-entry | 10617888945→10617973152 | 27808577557→27809866666 | 5084617384→5084617384 | 34 / 16 |
| 02-stop-exit | 10617973152→10617987299 | 27813074836→27812833029 | 5084617384→5084617919 | 4 / 10 |
| 03-stop-exit-proof | 10617987299→10617996028 | 27812917664→27813144156 | 5084617919→5084617919 | 18 / 9 |

Current-job exception charges are **0 B C:, 0 B E:, 0 B engine metadata**. Tooling overhead is outside the port budget. Windows/owner changes remain separately classified; free-space movement is not attributed to the port. The ledger is 217,221,046 B, below 2 GiB. It retains one initial inventory and hashed per-operation deltas, including drive-root folders. Protected enumeration had zero access errors; 203 denied OS/WER/tool-temp paths are disclosed, not treated as empty. UHT C: paths retain full before/after hashes and show no current-job change.

The completed 094-05 ledger operations charged 2,184 B C: and 0 B E:. That C: figure conservatively includes 2,072 B of foreign Adobe logging plus 112 B of UHT crash-report configuration. The interrupted operation 06 has no closing ledger; I did not fabricate a retrospective PASS across the two-day gap. This recovery preserves its before snapshot and records the present differences separately.

**Toolchains, memory gate and working builds**

Fresh file/version reads and vswhere confirm Community 2022 **17.12.35707.178**, Build Tools 2017 **15.9.34031.82**, MSVC directories **14.36.32532, 14.38.33130, 14.42.34433**, and 2017 directory **14.16.27023**. No 14.44 directory exists. The pinned 5.1 compiler has product version **14.38.33144**; the 4.25 compiler has product version **14.16.27051**. SDK Include/Lib/bin directories **10.0.17763.0** and **10.0.22621.0** exist. No compiler was executed for this discovery. Fresh JsonExport selection is deferred by the STOP; the earlier selection logs remain evidence only for their own runs.

**EF1 was not implemented or exercised:** entry stopped before that step. Memory-gated starts, waits, timeouts and process stops are all zero. One read-only observation at 10:09 IST showed about 7.76 GiB free physical and 42.03 GiB free commit; this is not the required three-poll admission. The next run still needs the 6/12 GiB three-sample start gate, 20 s spacing, 5 s monitoring, two-sample 1/4 GiB stop thresholds, PID-owned descendants excluding mspdbsrv, four-hour wait limit, output cleaning after an EF1 stop, and synthetic threshold/stop tests.

| Historical working leg | SHA | Toolset / SDK / standard | Project / plugin / engine / engine-plugin actions | Result |
|---|---|---|---|---|
| 09405 425-01 | 5bd368b | 14.16.27051 / 17763 / C++17, NoPCHs | 1 / 19 / 0 / 0 | Exit 0, 0 errors, 0 C++ warnings; 249.909 s; ledger 05-streaming-425 |
| 09405 51-01 | 5bd368b | 14.38.33130 directory, compiler 14.38.33144 / 22621 / Default; recovered response files say /std:c++17 | Planned 9 / 20 / 0 / 0 | Interrupted; eight first-header errors; no completed duration/result; ledger 06-51-working unfinished |
| 09406 final pair | None | None | No actions | Not run |

The 4.25 XML warnings are pre-existing: `BuildConfiguration.xml:16` (`bAllowUBAExecutor`) and `:22` (`MemoryPerActionBytes`) are unsupported by 4.25. They appear in plan and execution logs, four diagnostic lines total. No global XML was changed. C2855 fell **9→0** and C5043 **20→0** after EE3 NoPCHs. **No C5043 suppression was added.** C4099 was fixed by matching the render-data forward declaration. New port warnings in the completed 4.25 leg: zero.

The eight unresolved 5.1 errors are all `Source/AnomalyInjector/Private/Anomalies/Anomaly_<name>.cpp:1`, for **CameraClipping, ChangeCase, CorruptedTexture, LodPopping, MissingTexture, StuckLowMip, TexCorrupt and TimeDilation**. UBT expected each corresponding header first. The proposed ordinary fix is to put the portable/version-guarded own header before the version include, preserving the outer 5.1 implementation guard. No fix was applied here. The 5.1 log cannot be reported as zero-error just because the action-export process returned 0.

One additional receipt gap needs a harness fix before the next working pair: the 5.1 action admission recorded no standard response flags. A read-only recovery reader found the unquoted `@...response` command arguments and all 12 compile response files contain `/std:c++17`; their current hashes are recorded. Its first attempt recognized only quoted arguments; the second handled the actual unquoted form. The production wrapper was not changed after the STOP.

**Hosts, descriptors and prior EE proofs**

The layout remains `D:/IA_Europa/StylizedParisStreet`, `D:/IA_Europa/Baseline51/StackOBot`, and `D:/IA_Europa/ReGate51/StackOBot`, each with `Plugins/AnomalyInjector`. The 5.1 CaptureBench archives are commit **e64d6f9a43b7aa2e5eb041b7dab14e5285bb911b**, the release qualification receipt’s source; J2-installed copies were not used. Original-source/copy manifests remain under `Receipts/09403/copies`, with fresh equality results in this job’s P9 proof.

The unchanged 094-05 EC1/EE2 receipts show both 5.1 effective plugin sets equal the original except disabled, uncopied RoomGenerator and UnrealMCPython; 140 enabled plugins in the export union and 239 existing engine-plugin DLLs per host. Both 5.1 host sources are identical. All six Target.cs files differ from their originals only in the ratified probe-rooting block. EuropaPhaseAProbe is still an empty module, has no Compat dependency or audit access, and is excluded from Shipping with the legacy key.

| Last full JsonExport set, 09405 operation 02 | Pairs / mutant result |
|---|---|
| 5.1 release and port | Each: Game Debug/DebugGame/Development/Test/Shipping; Editor Debug/DebugGame/Development — 8 pairs |
| 4.25 | Game DebugGame/Development/Shipping; Editor DebugGame/Development — 5 pairs |
| Plugin Shipping-blacklist mutant, 5.1 | AnomalyShaders entered Shipping; restored and re-exported |
| Probe Shipping mutant, 5.1 | EuropaPhaseAProbe entered Shipping; restored and re-exported |
| Probe Shipping mutant, 4.25 | EuropaPhaseAProbe entered Shipping; restored and re-exported |

There was no additional 5.1 module-object delta against the EC-accepted A2 comparison. Literal raw object equality has the already-accepted exceptions: new capability defines and the public Injector dependency in Capture/ControlServer. I do not erase those differences by calling every raw object identical. C#, descriptors and .uproject files have not changed since that set; no export ran in this recovery job.

EE1/EE6 work committed in 094-05 removed UHT `UE-CmdLineArgs` UserDir, matched environment keys case-insensitively, parsed module names from Source paths, and revived cumulative budget checks. Its **30 tests passed**: 16 resume cases, 9 delta-ledger cases and 5 budget cases, including one-byte overflow/new-file/new-root-folder failures. The ledger’s streaming comparison replaced the memory-heavy SQL join. These are historical self-tests, not EF1 tests.

EE5’s recorded **12 function token comparisons** against d7eee97 have empty diffs for the 5.1 constructor, registry/ApplyAnomaly, auto-injector and selector paths. Original Shipping guards remain; the 5.1 capability policy returns true. The guarded Capture-helper enabled tokens match 668c075. [EE5 proof](<D:/IA_Europa/Receipts/09405/source/EE5-token-proof.json>).

**Closure contract and pending validation**

No production file changed in 094-06. The following is the committed source contract at 5bd368b, not a newly qualified A3 result.

| Shim contract | Code |
|---|---|
| Literal UWorldSubsystem + one FTickableGameObject base | Public/Compat/AnomalyTickableWorldSubsystem.h:10 |
| Super first, then only Game or PIE | Private/Compat/AnomalyTickableWorldSubsystem.cpp:20 |
| CDO Never; others retain base tick type; no Always | cpp:30 |
| 4.25 IsTickable = !IsTemplate() && bInitialized | cpp:35 |
| 5.1 base IsTickable; engine expression IsAllowedToTick, final | cpp:40/44; header:23 |
| False→true→false checked lifecycle, initialized tick check | header:30; cpp:49/58/67 |
| World and stats | header:20/25 |
| Default-off, non-Shipping, IAIBench-only audit | cpp:10/75; no allocation/lock on off path |

Injector, AutoInjector, Selector, Capture and ControlServer share that parent; their DoesSupportWorldType overrides are gone. The independent per-world update denominator and runtime/CDO/world legs remain A5/A6 work.

The nine raw UPROPERTY replacements are **MissingTextureChecker, CorruptedTexturePink, TexCorruptUv, TexCorruptNormal, TexCorruptNoise, TexCorruptStrongRefs, Root, PartEligible and PartIneligible**. Names/specifiers and strong GC references remain; eight scalars initialize null and the array initializes empty. No weak-pointer substitution occurred.

The 4.25 policy permits blinking, lighting_mismatch and lod_corruption, plus missing_object only under IAIBench. Unsupported/unknown IDs fail closed before a successful draw. Plugin-authored asset finders are absent from the 4.25 constructor and the fields remain null. The stock component probe does not set AllowNanite globally. On 5.1, the catalogue, nine pool keys, arrays, draw order, asset finders and unknown-ID responses retain release behavior by the EE5 token proof; runtime parity remains unqualified.

The 18 Capture implementation units retain the ANOMALY_CAPTURE boundary: new outer guards cover Census, ChangeMeasurement, ChangeStage, FrameCapturer, LabelWriter, MaskSceneViewExtension, Measurability, PreviewCapture, RunLog, SceneViewExtension and SveCapturer; AsyncWriter, CaptureLetterbox, ChangeFamilyData, MaskMeasure, PreviewTee, StencilTag and SveKeyRing already had guards. Disabled complete holders cover normal and FVTableHelper construction/destruction. The 094-05 helper fixes add 13 void guards, four false-return guards, a zero-return reader, and inert PreviewPump/Arm/Poll exports. The control subsystem creates no listener on 4.25. Thirteen advanced Injector units remain exact-5.1-only. AnomalyUnsupportedTexture returns unavailable/false, no draw modes and zero measurements; it never claims ready/applied/measured.

The Compat layer covers skinned-asset identity, static render data and LOD screen size, skeletal predicted LOD, camera POV/near clip, contact shadows, collision data, frustum construction, axis/small-number constants, optional scene-capture flags and TFunction reset. The 5.1 branches preserve the original expressions; the working 5.1 compile is still needed to close include reach.

EC4 reflected-surface inventory: UAnomalyCaptureSubsystem has no reflected members or methods. Its native frame/snapshot references and Async/PreviewTee/RunLog ownership are opaque. Injector’s six properties use portable UObject/material/texture types, and its reflected callback is `OnWatchedTargetEndPlay(AActor*, EEndPlayReason::Type)`; Bench’s three component properties are portable too. No engine-specific renderer, skinned-asset or WebSocket type requires another reflected change for the declared seams:

| Later phase | Existing non-reflected seam | Planned 4.25 implementation |
|---|---|---|
| C | IAnomalyCaptureBackend / CreateCaptureBackend425, native Async owner | Source/AnomalyCapture/Private/Compat/AnomalyCaptureBackend425.cpp |
| E | IAnomalyTextureAdapter / CreateTextureAdapter425, IAnomaly facades | Source/AnomalyInjector/Private/Compat/AnomalyTextureAdapter425.cpp |
| F | FAnomalyControlTransportState opaque transport | Source/AnomalyControlServer/Private/Compat/AnomalyControlTransport425.cpp |

C/E are declarations only, with no 5.1 backend rewiring. Historical ancestry evidence scans 432 source/tool/host files and 6,368 assets with zero asset byte-string matches for TickableWorldSubsystem; no asset was loaded. The expanded final scan remains pending after the source changes. The current EA11 scan passes on **7 reflected headers**, including the shim, and its in-memory forbidden-directive mutant fails as required. Asset-registry ancestry checks remain deferred to the first authorized editor launch.

The strict tool already accepts explicit roots, uses UE4Editor paths on 4.25, and avoids 4.25 IncludeOrderVersion. It has not run and is not qualified. A4 must exercise its can-fail and final strict builds. The checked-in closure/reach prose still contains pre-EE status and an obsolete UHT UserDir explanation; those documentation updates were not made after the entry STOP. This report and the current receipt manifest explicitly correct those historical claims.

**Commits and artifacts**

There are **no 094-06 commits or pushes**. The surviving 094-05 commits, all on feat/europa-port and unpushed, are:

| Commit | Author/committer time, IST | Subject |
|---|---|---|
| 90377bf02996b46f06bc7b5adef63751da91c6c3 | 2026-10-22 16:04:48 | fix(europa): repair build receipts and admit ruled UHT outputs |
| 88b01f1ff771db993d6d631dc5cd21dc82010d64 | 2026-10-22 16:08:35 | fix(europa): isolate 4.25 C++17 modules from shared PCHs |
| e421441ceba890ec23f8b953d491cc7551ac55da | 2026-10-22 16:19:54 | fix(europa): close disabled capture and shared 4.25 APIs |
| 5bd368b207c4fe430ebfbd5208c3a41fb6f2eb06 | 2026-10-22 16:54:14 | fix(europa): bound ledger comparison memory |

W4 date variables are absent. No exe/DLL or archive was produced in this job; no bench lever was changed. Current hashes of DLLs from the earlier 4.25 action list are below, solely as recovery readings:

The initial predictions commit ac81ecb at 2026-10-16 16:32:37 IST preceded the first UBT invocation at 16:47:19, as accepted by EC. No prediction was revised here; the current prediction history is saved with this report’s evidence.

| Existing DLL | Current SHA-256; not a historical receipt |
|---|---|
| UE4Editor-AnomalyBench.dll | 45a59ddc9b59ca8be29e34f28fd562adb43d4ea7d0269c823ce0905dc3fc376c |
| UE4Editor-AnomalyCapture.dll | 259efa7d07cb09f9fce44ab7098426813240bc97a231968da18be9db04ea21c9 |
| UE4Editor-AnomalyControlServer.dll | 294a5199a0673bf369d65ec3bc27b99f1cf7d1f4818e26e67653510b9d4658e5 |
| UE4Editor-AnomalyInjector.dll | 69cf17a850552bcfbad636ab8545593c59954d38b97cf801de125d8a70953c77 |
| UE4Editor-AnomalyShaders.dll | 24639a1005ca5cd6b71e94562449cbca604f64aba2f136b98e10e9a37bf6a491 |

**Exact resume point / A4 handoff**

A3 is incomplete and A4a/A4b are not admitted by this report. Resume depends on chat’s ruling on the demonstrated engine metadata writes and interrupted-run accounting. Then:

1. Re-run recovery/P9 against the explicitly ruled baseline/exception record; preserve protected binary/config hashes.
2. Implement and commit EF1 with synthetic start/stop/ownership/timeout tests before any UBT build-mode child; also fix standard-response capture for the unquoted 5.1 response arguments.
3. Clean only the interrupted ReGate51 host outputs identified by the saved action list/UHT receipts, by receipt; clean again after any EF1 stop. Do not delete engine files under the current authority.
4. Fix the eight first includes while keeping disabled 4.25 units free of UE5 expressions. Commit before compiling; move ReGate51 to that SHA.
5. Complete the 5.1 working compile and a fresh 4.25 compile on one final SHA, with pinned toolsets, one worker, NoEngineChanges, no engine/engine-plugin actions, diagnostics and produced-binary hashes.
6. Update the stale closure/reach/session prose; finish ancestry/EA11/exit evidence and the report. Re-export the complete matrix and mutants if C#, descriptors or .uproject files change.

The memory wait limit and compiler retry limits were not consumed here. The report collector and the read-only response reader each used one correction within two attempts. The earlier interruption remains excluded from D2 under EF2. Recovery fsck/integrity and engine-delta reads preceded the full EA8 hash sweep; all were read-only and neither P9 comparison exempts the discovered differences. After the STOP, work was limited to read-only verification and report/evidence generation. Implementation awaits the engine-write ruling.

**File-by-file reach at the unchanged HEAD**

The following **98 changed paths** join exactly to `git diff --name-only d7eee97..5bd368b`. N = changed but intended neutral on 5.1; C = changed tooling/diagnostics/documentation; B = byte-identical planned file. N is an intended effect, not runtime qualification. The complete reasons and reached behaviors/gates are also in [reach.json](<D:/IA_Europa/Receipts/09406/report-data/reach.json>).

| Path | 5.1 class | Change/reason |
|---|---|---|
| AnomalyInjector.uplugin | N | A2 legacy descriptor aliases preserve module membership; real matrix and mutant rerun in A3 |
| CLAUDE.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| Source/AnomalyBench/AnomalyBench.Build.cs | N | Exact-version rules; 4.25 C++17/NoPCHs and availability; 5.1 settings retained. |
| Source/AnomalyBench/Private/AnomalyBenchModule.cpp | N | A2 disabled 4.25 bench implementation and module diagnostic; enabled body retained |
| Source/AnomalyBench/Private/AnomalyBenchTwoPartActor.cpp | N | A2 disabled component construction on 4.25; 5.1 constructor retained |
| Source/AnomalyBench/Private/AnomalyBenchTwoPartActor.h | N | Three null raw reflected component pointers; names, specifiers and GC traversal retained |
| Source/AnomalyCapture/AnomalyCapture.Build.cs | N | Exact-version rules; 4.25 C++17/NoPCHs and availability; 5.1 settings retained. |
| Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp | N | Shared shim and disabled ownership types; EE compile fixes guard helper bodies and provide inert preview exports without changing enabled Capture tokens. |
| Source/AnomalyCapture/Private/AnomalyCensus.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalyChangeMeasurement.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalyChangeStage.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalyFrameCapturer.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalyFrameCapturer.h | N | Disabled strict-header TU must not parse renderer or PixelFormat includes |
| Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalyLabelWriter.h | N | Disabled strict-header TU must not parse renderer or PixelFormat includes |
| Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalyMeasurability.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalyPreviewCapture.cpp | N | Disabled preview exports clear outputs and return unavailable; 5.1 encoding untouched |
| Source/AnomalyCapture/Private/AnomalyPreviewTee.h | N | Complete disabled type for default, FVTableHelper and destructor TUniquePtr paths |
| Source/AnomalyCapture/Private/AnomalyRunLog.cpp | N | Complete inert run-log ownership and all virtual methods; no registration or writes at 0 |
| Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Private/AnomalySveCapturer.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained |
| Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy |
| Source/AnomalyCapture/Public/Compat/AnomalyCaptureBackend.h | N | Phase C native interface/factory declarations only; no 5.1 rewiring |
| Source/AnomalyControlServer/AnomalyControlServer.Build.cs | N | Exact-version rules; 4.25 C++17/NoPCHs and availability; 5.1 settings retained. |
| Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp | N | Remove derived world-type override; preserve existing lifecycle call placement |
| Source/AnomalyControlServer/Private/Compat/AnomalyControlTransportState.h | N | A2 opaque non-reflected transport ownership; complete disabled type |
| Source/AnomalyControlServer/Private/ControlProtocol.cpp | N | No protocol/snapshot work or unavailable dependencies on the disabled transport path |
| Source/AnomalyControlServer/Private/ControlSnapshot.cpp | N | No protocol/snapshot work or unavailable dependencies on the disabled transport path |
| Source/AnomalyControlServer/Public/AnomalyControlServerSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy |
| Source/AnomalyInjector/AnomalyInjector.Build.cs | C | Exact-version rules; 4.25 C++17/NoPCHs and availability; 5.1 settings retained. Once-per-target profile diagnostic is C. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.h | N | Self-contained disabled native header in strict 4.25 TUs |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_ChangeCase.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_LodPopping.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.h | N | Self-contained disabled native header in strict 4.25 TUs |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.h | N | Self-contained disabled native header in strict 4.25 TUs |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_TimeDilation.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptBench.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptCore.h | N | Self-contained disabled native header in strict 4.25 TUs |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptState.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp | N | Exact-5.1 implementation guard; disabled 4.25 TU. First-header fixes remain for the eight named files. |
| Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp | N | Shim and 4.25 admission; 5.1 pool/list/draw tokens match release; versioned TFunction reset. |
| Source/AnomalyInjector/Private/AnomalyHiddenClass.cpp | N | Contact-shadow API boundary; other hide and restore flags retained |
| Source/AnomalyInjector/Private/AnomalyInjectorModule.cpp | N | Stock 4.25 explicit false Nanite probe; no AllowNanite override; 5.1 probe ownership unchanged |
| Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp | N | Shim/ownership changes; EE5 restores release constructor, Register and unknown-ID tokens on 5.1; 4.25 rejects unsupported IDs before use. |
| Source/AnomalyInjector/Private/AnomalyLod.cpp | N | Static screen-size and skeletal predicted-LOD access use version adapters. |
| Source/AnomalyInjector/Private/AnomalySelectorSubsystem.cpp | N | Shim and 4.25 selector filter; EE5 retains release 5.1 list/cycle/draw behavior. |
| Source/AnomalyInjector/Private/AnomalyViewport.cpp | N | Camera, near-clip, frustum, axis, scene-capture and collision adapters keep 5.1 expressions. |
| Source/AnomalyInjector/Private/Compat/AnomalyCapabilities.cpp | N | EE5: exact 4.25 allowlist; 5.1 admits every registered ID. |
| Source/AnomalyInjector/Private/Compat/AnomalyEngineCompat.cpp | N | Exact 5.1 API operations; declared stock 4.25 API substitutions |
| Source/AnomalyInjector/Private/Compat/AnomalyTickableWorldSubsystem.cpp | N | Single FTickable registration; 4.25 IsTickable lifetime gate; default-off audit only |
| Source/AnomalyInjector/Private/Compat/AnomalyUnsupportedTexture.cpp | N | Complete 4.25 portable texture facade; never applied/eligible/ready/measured |
| Source/AnomalyInjector/Public/AnomalyAutoInjectorSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy |
| Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h | N | Literal shim parent; five null raw reflected asset pointers and one empty raw-pointer array, with names/specifiers retained |
| Source/AnomalyInjector/Public/AnomalySelectorSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy |
| Source/AnomalyInjector/Public/Compat/AnomalyCapabilities.h | N | Single capability admission policy for registry, auto pool and selector |
| Source/AnomalyInjector/Public/Compat/AnomalyEngineCompat.h | N | 4.25 API adapters for asset/LOD/render data/camera/contact/collision/frustum/constants/optional flags and TFunction reset; original 5.1 expressions preserved. |
| Source/AnomalyInjector/Public/Compat/AnomalyEngineVersion.h | N | A2 exact engine predicates and unsupported-version diagnostic |
| Source/AnomalyInjector/Public/Compat/AnomalySharedPointerIncludes.h | N | A2 portable defining shared-pointer includes outside reflected UHT parse |
| Source/AnomalyInjector/Public/Compat/AnomalyStatsIncludes.h | N | A2 defining stats includes outside reflected UHT parse |
| Source/AnomalyInjector/Public/Compat/AnomalyTextureAdapter.h | N | Phase E native adapter/factory declarations only; existing 5.1 facade unchanged |
| Source/AnomalyInjector/Public/Compat/AnomalyTickableWorldSubsystem.h | N | Literal shared reflected ancestry and engine-equivalent 5.1 lifetime/tick contract |
| Source/AnomalyShaders/AnomalyShaders.Build.cs | N | Exact-version rules; 4.25 C++17/NoPCHs and availability; 5.1 settings retained. |
| Source/AnomalyShaders/Private/AnomalyShadersModule.cpp | N | A2 disabled 4.25 shader registration and diagnostic; 5.1 mapping retained |
| docs/PRE-DELIVERY-CHECKLIST.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/architecture.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/europa/09404-working-result.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/europa/phase-a-a3-closure.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/europa/phase-a-reach.json | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/europa/phase-a-routing.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/predictions/094-europa-phase-a.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/sessions/2026-10-16-094-03-europa-a1-a2.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/sessions/2026-10-22-094-04-europa-a3.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| docs/sessions/2026-10-22-094-05-europa-a3-resume.md | C | Port rules, predictions, evidence or session documentation; some A3 result prose awaits the final update. |
| tools/europa/DescriptorProbe.cs.txt | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/check_reflected_headers.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/compile_children.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/descriptor_matrix.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/host_probe/EuropaPhaseAProbe.Build.cs.txt | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/host_probe/EuropaPhaseAProbeModule.cpp.txt | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/host_probe/TargetRules.fragment.txt | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/idle_gate.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/ledger_budget_self_tests.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/ledger_delta_self_tests.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/process_wrapper.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/resume_self_tests.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/tooling_self_tests.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/ubt_admission.py | C | Scoped port proof, fixture or harness tooling; no runtime behavior claim. |
| tools/europa/working_build.py | C | ED2 target/action admission; EE6 module diagnostic parsing and removal of ineffective UHT UserDir attempt. EF1 memory gate is still pending. |
| tools/europa/write_ledger.py | C | ED5 delta ledger/root folders, EE1 exact UHT classes, EE6 child environment receipt and bounded-memory streaming comparison. |
| tools/strict_include_build.py | C | Explicit roots, UE4Editor, version-safe strict settings, actual TU action coverage and receipted restoration; not executed until A4 |

Planned files needing no edit, confirmed absent from that diff:

| Path | 5.1 class | Reason |
|---|---|---|
| Content/Materials/M_CorruptTex_Normal.uasset | B | Existing asset, algorithm, guard or reader retained. |
| Content/Materials/M_CorruptTex_UV.uasset | B | Existing asset, algorithm, guard or reader retained. |
| Content/Materials/M_CorruptedTexture_Pink.uasset | B | Existing asset, algorithm, guard or reader retained. |
| Content/Materials/M_MissingTexture_Checker.uasset | B | Existing asset, algorithm, guard or reader retained. |
| Content/Textures/T_CorruptTex_NoiseN.uasset | B | Existing asset, algorithm, guard or reader retained. |
| Shaders/Private/AnomalyMaskReduce.usf | B | Existing asset, algorithm, guard or reader retained. |
| Shaders/Private/AnomalyTexStats.usf | B | Existing asset, algorithm, guard or reader retained. |
| Shaders/Private/AnomalyVisibleMask.usf | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalyAsyncWriter.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalyCaptureLetterbox.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalyCaptureModule.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalyChangeFamilyData.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalyMaskMeasure.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalyPreviewTee.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalyStencilTag.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyCapture/Private/AnomalySveKeyRing.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_Blinking.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_LightingMismatch.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_LodCorruption.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingObject.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyInjector/Public/AnomalyDefaults.h | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyShaders/Private/AnomalyMaskReduceShader.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyShaders/Private/AnomalyTexStatsShader.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyShaders/Private/AnomalyVisibleMaskShader.cpp | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyShaders/Public/AnomalyMaskReduceShader.h | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyShaders/Public/AnomalyTexStatsShader.h | B | Existing asset, algorithm, guard or reader retained. |
| Source/AnomalyShaders/Public/AnomalyVisibleMaskShader.h | B | Existing asset, algorithm, guard or reader retained. |
| tools/check_package_descriptor.py | B | Existing asset, algorithm, guard or reader retained. |
| tools/verify_capture.py | B | Existing asset, algorithm, guard or reader retained. |

VERDICT: NEEDS-DECISION | A3 stopped at EF2 on inherited 5.1 engine metadata outside ED4; protected binary/config hashes match; no repository edits, cleanup, compile or push in 094-06; final compile pair pending
