<!-- Collected by Claude Code (mailbox brief 094-07) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 65,434 B, sha256 239D83BFC5745F134933B7DDE5F38A03A2E090CDC1A250A3BBF98B77C6BEA9EE, written 2026-10-24 15:21:17 IST by Codex thread 01a14334 (gpt-6-astra / max), job 13:46:29-15:29:32, exit 0. Verbatim below this line. -->
**094-07 — Europa A3 finish report · 2026-10-24**

**A3 is complete.** Both authorized Editor Development targets pass on `204724a7d089463767518f28723b077f7f058ab1`, with zero errors and zero engine/engine-plugin actions. The Baseline51 action-plan query finds zero outdated engine actions. These are working builds; A4 final-build, strict, runtime, picture and delivery gates remain outstanding.

P9 v2 re-derives byte for byte. Entry and exit each verify 23,033 protected binary/config hashes. Protected state matches v2 plus EG1-authorized bookkeeping; original sources are unchanged. EF1 memory supervision, EG1 bookkeeping receipts and EG2 product/flag receipts are active. No editor/game/commandlet/cook/UAT, Game/Shipping build, push, tag, toolset change or foreign-process termination occurred.

Evidence root: `D:/IA_Europa/Receipts/09407`. [Saved report](<D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/report.md>). The two corrected harness problems are retained, with one correction each. Earlier job evidence was not rewritten.

**Entry, recovery and P9 v2**

The v1 manifest is `db884f1e9c3debd7b9ed678db08036509959b1fc5a0d27799b10ecccb0625cce`. Streaming v1 through the declared normalized root exclusions reproduces both files exactly, in order:

| Manifest | Lines | File bytes | SHA-256 |
|---|---:|---:|---|
| v1 | 707,775 | 231,535,810 | `db884f1e9c3debd7b9ed678db08036509959b1fc5a0d27799b10ecccb0625cce` |
| v2 | 472,552 | 159,596,346 | `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74` |
| removed | 235,223 | 71,939,464 | `dd970a071b603e8a08cd38d69cecdea31e0b3ebc111b7ed29584b91a7764d063` |

Entry inventories 472,553 files, admitting exactly 1,023 inherited EG1 differences by exact size/mtime/hash receipt. Protected hashes cover 104,794,989,456 bytes per sweep. The first scan omitted eight retained RoomGenerator assets that v1 had reached through an excluded E: junction; all eight existed on D:. The second attempt added that physical target and passed. This was a traversal bug, not a protected-file deletion or a broader exclusion.

All protected refs, main and retained worktree heads/index hashes/status match. Removed-root worktrees are excluded under EG5, while their branch refs remain protected. The three port worktrees are clean; Paris and ReGate51 bind to the final SHA, and Baseline51 remains detached at d7eee97. `git fsck --connectivity-only` exits 0 (dangling objects noted, no repair); the interrupted ledger passes read-only SQLite `integrity_check`. The 4.25 install and Paris original inventories match 09406; original Paris and StackOBot copy-source manifests still match A1.

EF2 removed **100 files / 66,986,117 bytes**, only inside `D:/IA_Europa/ReGate51/StackOBot`. The saved action list, affected Intermediate trees, UHT output directories and explicit products determine the deletion list. Each file was hashed and its resolved path checked before deletion. Source, Config, Content and engine trees were excluded. [Cleaning receipt](<D:/IA_Europa/Receipts/09407/recovery/EF2-cleaning-result.json>).

The interruption lost the 5.1 build result, closing ledger and final report. It did not lose the committed source. Code’s event-log evidence identifies a Start-menu restart under memory pressure, rather than a kernel crash. The 17:01 4.25 receipt lacked binary hashes, so its historical zero-error result was retained as history and 4.25 was rebuilt under EG2. No inherited engine bookkeeping was restored or deleted by this job.

| Earlier A3 working run | SHA | Project/plugin/engine actions | Result | Duration |
|---|---|---|---|---:|
| 09404 4.25 | 9fdc8f0 | 11/24/0 | Exit 6; 144 recorded error lines, including 9 C2855; 20 C5043 warnings; UHT exception STOP | 126.45 s |
| 09405 4.25 | 5bd368b | 1/19/0 | Exit 0; zero errors/C++ warnings after NoPCHs and closure fixes; binary hashes absent | 249.91 s |
| 09405 5.1 | 5bd368b | Planned 9/20/0 | Eight first-include diagnostics; the first two compile actions were logged before the interruption; no complete result | Unavailable |

Both historical 4.25 runs selected 14.16.27051 / SDK 10.0.17763.0 / C++17; the interrupted 5.1 run selected 14.38.33144 / SDK 10.0.22621.0 with the existing shared PCHs. Original diagnostics retain module/file:line detail: [09404 result](<D:/IA_Europa/Receipts/09404/builds/425-01/result.json>), [09405 result](<D:/IA_Europa/Receipts/09405/builds/425-01/result.json>). These are history, not substitutes for the completed pair below.

**Toolchains, hosts and descriptor proof**

Read-only vswhere: VS Community 2022 17.12.35707.178; Build Tools 2017 15.9.34031.82. Installed MSVC directories are 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023; no 14.44. SDKs 10.0.17763.0 and 10.0.22621.0 exist. Fresh JsonExport confirms:

- `Anomaly profile: engine=4.25 compiler=VisualStudio2017 version=14.16.27051 SDK=10.0.17763.0 cppStandard=Cpp17 toolchain=C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.16.27023`.
- `Anomaly profile: engine=5.1 compiler=VisualStudio2022 version=14.38.33144 SDK=10.0.22621.0 cppStandard=Default toolchain=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130`.

Hosts remain Paris on D:, plus `D:/IA_Europa/Baseline51/StackOBot` and `D:/IA_Europa/ReGate51/StackOBot`. CaptureBench is the archived e64d6f9a43b7aa2e5eb041b7dab14e5285bb911b source, never J2’s installed copy. EC1 retains the original effective engine-plugin set except disabled, uncopied RoomGenerator and UnrealMCPython: the accepted export union has 140 enabled plugins and 239 existing engine-plugin DLLs per 5.1 host. Their existence was rechecked; engine binaries are also covered by the entry/exit hashes. Both 5.1 probe sources/targets remain identical.

| Applicable full JsonExport set | Result |
|---|---|
| 5.1 release and port, 8 pairs each | Game Debug/DebugGame/Development/Test/Shipping; Editor Debug/DebugGame/Development; equal module membership |
| 4.25, 5 pairs | Game DebugGame/Development/Shipping; Editor DebugGame/Development; expected membership |
| 5.1 plugin Shipping-blacklist mutant | AnomalyShaders enters Shipping; restoration removes it |
| 5.1 and 4.25 probe mutants | EuropaPhaseAProbe enters Shipping; each restoration removes it |

The full set and all three mutants are the accepted 09405 results. No C#, descriptor or .uproject changed since that set, so the conditional full rerun was unnecessary; this job reran both Editor Development toolchain exports. Raw 5.1 module objects retain only the EC-accepted capability definitions and public Injector dependency in Capture/Control, with no additional difference. The exports do not replace packaged descriptor proof.

**Memory gate, tooling and working compiles**

EF1 requires >=6 GiB physical and >=12 GiB free commit on three polls 20 seconds apart. During UBT, two consecutive 5-second readings below 1/4 GiB stop only retained-handle-owned UBT descendants. Parent lifetime and creation times protect against PID reuse; mspdbsrv is excluded. Stopped working outputs are cleaned by receipt before the next wait. Each wait has a four-hour limit; MaxParallelActions=1 stays set.

Self-tests: 75 initial EF1/EG1/EG2 cases, expanded to 82 with retained response and strict command-wiring checks; 16 EE1/EE6 cases, 9 delta-storage cases, 5 cumulative budget cases and 31 routing cases pass. Missing `--output` returns 2 without creating an old job folder. Synthetic engine/engine-plugin actions and forbidden metadata paths fail. The stop callback is tested with synthetic memory readings; no actual starvation or process termination was used. Strict wiring uses a fake child runner; no strict build was launched.

| UBT invocation memory log | Wait polls | Admission physical/commit GiB | Stops |
|---|---:|---|---:|
| 06-51-working/child-008-memory.jsonl | 24 | 7.77/34.00 | 0 |
| 06-51-working/child-009-memory.jsonl | 2 | 7.03/33.20 | 0 |
| 07-425-working/child-008-memory.jsonl | 18 | 8.68/35.01 | 0 |
| 07-425-working/child-009-memory.jsonl | 2 | 8.57/34.99 | 0 |
| 09-51-final/child-018-memory.jsonl | 2 | 8.10/35.77 | 0 |
| 09-51-final/child-019-memory.jsonl | 2 | 8.06/35.76 | 0 |
| 10-425-final/child-008-memory.jsonl | 2 | 8.08/35.83 | 0 |
| 10-425-final/child-009-memory.jsonl | 2 | 8.05/35.77 | 0 |
| 11-baseline-query/child-008-memory.jsonl | 2 | 7.90/35.68 | 0 |

The first launch harness failed before UBT because an import had been renamed unnecessarily; its second attempt uses the unchanged compiler-identity helper and succeeds. The initial 5.1 wait correctly refused insufficient physical headroom while Blender was running. No foreign process was stopped.

| Working result / query | SHA | Project/plugin/engine/engine-plugin actions | Exit / errors / new port warnings | Seconds |
|---|---|---|---|---:|
| 07-425-working 425 | `286f2ee0ca6f` | 1/3/0/0 | 0 / 0 / 0 | 438.68 |
| 10-425-final 425 | `204724a7d089` | 0/0/0/0 | 0 / 0 / 0 | 80.91 |
| 06-51-working 51port | `286f2ee0ca6f` | 9/20/0/0 | 0 / 0 / 0 | 743.71 |
| 09-51-final 51port | `204724a7d089` | 0/0/0/0 | 0 / 0 / 0 | 84.87 |
| 11-baseline-query 51base query | `d7eee9773181` | 9/20/0/0 | 0 / 0 / 0 | 53.24 |

Every working command carries `-NoEngineChanges -NoBuildUHT -NoHotReload -NoHotReloadFromIDE -NoXGE -MaxParallelActions=1`, scoped `-Project` and `-Log`, with process-local TEMP/TMP, dotnet and UAT caches on D:. Action plans precede execution and reject engine/engine-plugin or foreign-host products. Exact command lines, actual child environment, compiler descendants, duration, diagnostics and operation links are retained in each receipt. The Baseline51 query executes no compile or link action.

Working builds executed 29 actions on 5.1 and four incremental actions on 4.25 at 286f2ee. Later commits changed tooling and documentation only. The final-HEAD pair is an up-to-date confirmation, with zero actions; Source is byte-identical and every host DLL/PDB hash equals the working-pair inventory. Retained response hashes match the actual earlier compiler responses and are labelled as retained evidence, not new compiler execution. Durations include memory waits. [Final source/binary binding](<D:/IA_Europa/Receipts/09407/source/final-source-binding.json>).

4.25 uses C++17 and NoPCHs for all five plugin modules. 5.1 keeps its original default standard/PCH policy; the actual compiler responses show C++17. C2855 fell 9→0 and C5043 20→0 after EE3, without suppression. The eight own-header-first diagnostics are resolved while keeping the outer 5.1 guards. The only remaining 4.25 warnings are the pre-existing user XML entries at BuildConfiguration.xml:16 (bAllowUBAExecutor) and :22 (MemoryPerActionBytes), repeated in plan/execution. No global XML was changed.

5.1 reports C4996 at host Source/StackOBot/MidReproActor.cpp:110 and :153 for SetMasterPoseComponent. This copied fixture is byte-identical to the original and Baseline51 (SHA-256 `8fd561694c301f2de912f4706c8cd6e5bd4aa4647d75f72ee0852c1875039ffb`). The initial parser conservatively flagged both as new pending review; this origin proof classifies them as existing host warnings. No port C++ warning remains. [Warning origin proof](<D:/IA_Europa/Receipts/09407/source/host-warning-origin.json>).

| Engine | Selected compiler / SDK | Effective plugin compiler flags |
|---|---|---|
| 4.25 | 14.16.27051, directory 14.16.27023 / 10.0.17763.0 | `/std:c++17`; no `/Yc`, `/Yu` or `/Fp` in the nine retained plugin compile responses |
| 5.1 | 14.38.33144, directory 14.38.33130 / 10.0.22621.0 | `/std:c++17`; `/Yu` and `/Fp` reference existing Engine/UnrealEd PCHs; the probe uses Core PCH |

**Write ledger and protected state**

ED1 start/warning/floor GiB: C 13/11/10; D 70/55/50; E 25/20/15. All readings below are resolved volumes. Budgets remain C 250 MB, E 50 MB, EG1 bookkeeping 100 MB. Free space is a floor monitor, never a write attribution estimate.

| Operation | C/D/E free before → after GiB | D logical bytes before → after | C/E/EG1 charge B | Foreign/tooling changes |
|---|---|---|---|---|
| 01-entry | 18.87/137.84/53.79 → 18.86/137.53/53.79 | 19,363,245,371 → 19,482,676,843 | 0/0/0 | 9/18 |
| 02-entry-retry | 18.86/137.53/53.79 → 18.85/137.41/53.79 | 19,482,676,843 → 19,609,273,511 | 0/0/0 | 17/15 |
| 03-tooling | 18.85/137.41/53.79 → 18.86/137.41/53.79 | 19,609,273,511 → 19,609,656,458 | 0/0/0 | 4/5 |
| 04-recovery-source | 18.86/137.41/53.78 → 18.86/137.46/53.77 | 19,609,656,458 → 19,548,442,458 | 0/0/0 | 11/2 |
| 05-51-working | 18.85/137.46/53.78 → 18.85/137.46/53.77 | 19,548,442,458 → 19,548,442,458 | 0/0/0 | 3/4 |
| 06-51-working | 18.84/137.46/53.72 → 18.80/136.98/53.53 | 19,548,442,458 → 20,064,464,723 | 40960/0/1167215 | 4/22 |
| 07-425-working | 18.78/136.98/53.53 → 18.29/136.98/53.46 | 20,064,464,723 → 20,064,619,356 | 12288/0/0 | 4/23 |
| 08-final-source | 18.28/136.98/53.45 → 18.28/136.98/53.45 | 20,064,619,356 → 20,065,032,606 | 0/0/0 | 5/5 |
| 09-51-final | 18.28/136.98/53.45 → 18.28/136.98/53.45 | 20,065,032,606 → 20,065,101,893 | 0/0/550738 | 2/14 |
| 10-425-final | 18.28/136.98/53.45 → 18.28/136.98/53.45 | 20,065,101,893 → 20,065,129,844 | 0/0/0 | 0/4 |
| 11-baseline-query | 18.28/136.98/53.45 → 18.28/136.94/53.45 | 20,065,129,844 → 20,102,420,441 | 0/0/1913446 | 1/11 |
| 12-exit | 18.27/136.94/53.45 → 18.27/136.83/53.44 | 20,102,420,441 → 20,229,077,141 | 0/0/0 | 16/16 |

One full inventory per scope plus hashed operation deltas is retained. Protected enumeration is error-fatal; denied OS/tooling paths are explicitly recorded, not treated as empty. Root-folder additions are checked and their synthetic mutant stops. Tooling overhead inventories `.claude` and `C:/ClaudeTemp` on C:, and the `.codex` junction on physical E:, outside port budgets. [Per-path exceptions, foreign writes and tooling totals](<D:/IA_Europa/Receipts/09407/report-data/exceptions-foreign-tooling.json>).

| EG1 operation | Changed paths | Charged bytes | Classes |
|---|---:|---:|---|
| 06-51-working | 1021 | 1,167,215 | LiveCodingInfo.json=653, Timestamp=368 |
| 09-51-final | 368 | 550,738 | Timestamp=368 |
| 11-baseline-query | 1022 | 1,913,446 | ActionHistory.bin=1, LiveCodingInfo.json=653, Timestamp=368 |

Cumulative charges: **C 53,248 B; E 0 B; EG1 3,631,399 B**. Ledger evidence is 0.20 GiB, below 2 GiB. All operation ledgers pass. No engine binary/generated C++/Source/Content/Config/Shaders/Programs write occurred; 4.25 engine writes stayed within the unchanged BuildRules policy. EE1 UHT paths have per-operation hashes. [Bookkeeping paths, hashes and classes](<D:/IA_Europa/Receipts/09407/report-data/engine-bookkeeping.json>).

**Closure contract and backend seams**

| Shim contract | Implementation |
|---|---|
| UWorldSubsystem plus one FTickableGameObject registration | Public/Compat/AnomalyTickableWorldSubsystem.h:10 |
| Super ShouldCreateSubsystem first, then Game or PIE only | Private/Compat/AnomalyTickableWorldSubsystem.cpp:20 |
| CDO Never, otherwise base tick type; no Always override | cpp:30 |
| 4.25 IsTickable = !IsTemplate() && bInitialized | cpp:35 |
| 5.1 base IsTickable; exact engine IsAllowedToTick expression, final | cpp:40/44; header:23 |
| False→true→false initialization checks and initialized Tick check | header:30; cpp:49/58/67 |
| World and stats mapping | header:20/25 |
| Default-off, non-Shipping IAIBench-only audit; off path no allocation/lock | cpp:10/75 |

Injector, AutoInjector, Selector, Capture and ControlServer use the same shim; their DoesSupportWorldType overrides are removed. CDO/lifecycle/world behavior still requires the A5/A6 runtime probes. The independent per-world denominator excludes TimeOnly/paused updates as predicted; the probe is still an empty host module with no Compat dependency or shim-audit access, rooted through the ratified target block and excluded from Shipping.

Nine raw UPROPERTY fields retain names/specifiers and strong GC ownership: MissingTextureChecker, CorruptedTexturePink, TexCorruptUv, TexCorruptNormal, TexCorruptNoise, TexCorruptStrongRefs, Root, PartEligible, PartIneligible. Eight scalars start null; the array starts empty. No weak-pointer substitution occurred.

| Reflected owner | Unchanged annotation | Portable declaration |
|---|---|---|
| `Source\AnomalyBench\Private\AnomalyBenchTwoPartActor.h` | `UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")` | `USceneComponent* Root = nullptr;` |
| `Source\AnomalyBench\Private\AnomalyBenchTwoPartActor.h` | `UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")` | `UStaticMeshComponent* PartEligible = nullptr;` |
| `Source\AnomalyBench\Private\AnomalyBenchTwoPartActor.h` | `UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnomalyBench")` | `UStaticMeshComponent* PartIneligible = nullptr;` |
| `Source\AnomalyInjector\Public\AnomalyInjectorSubsystem.h` | `UFUNCTION()` | `void OnWatchedTargetEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason);` |
| `Source\AnomalyInjector\Public\AnomalyInjectorSubsystem.h` | `UPROPERTY()` | `UMaterialInterface* MissingTextureChecker = nullptr;` |
| `Source\AnomalyInjector\Public\AnomalyInjectorSubsystem.h` | `UPROPERTY()` | `UMaterialInterface* CorruptedTexturePink = nullptr;` |
| `Source\AnomalyInjector\Public\AnomalyInjectorSubsystem.h` | `UPROPERTY()` | `UMaterialInterface* TexCorruptUv = nullptr;` |
| `Source\AnomalyInjector\Public\AnomalyInjectorSubsystem.h` | `UPROPERTY()` | `UMaterialInterface* TexCorruptNormal = nullptr;` |
| `Source\AnomalyInjector\Public\AnomalyInjectorSubsystem.h` | `UPROPERTY()` | `class UTexture2D* TexCorruptNoise = nullptr;` |
| `Source\AnomalyInjector\Public\AnomalyInjectorSubsystem.h` | `UPROPERTY(Transient)` | `TArray<UObject*> TexCorruptStrongRefs;` |

The shared capability policy admits blinking, lighting_mismatch and lod_corruption on 4.25, plus missing_object only with IAIBench. Unsupported/unknown requests fail closed without consuming a successful draw; the 4.25 constructor does not load plugin-authored assets. The stock component probe sets no global AllowNanite. On 5.1, capability admission passes every registry ID; 12 EE5 function token comparisons preserve constructor/registry/unknown-ID, auto and selector paths, including the nine keys, arrays, order and defaults. Runtime equivalence is still a gate obligation.

Capture’s 18 implementation TUs use ANOMALY_CAPTURE: added outer guards for Census, ChangeMeasurement, ChangeStage, FrameCapturer, LabelWriter, MaskSceneViewExtension, Measurability, PreviewCapture, RunLog, SceneViewExtension and SveCapturer; existing sufficient guards for AsyncWriter, CaptureLetterbox, ChangeFamilyData, MaskMeasure, PreviewTee, StencilTag and SveKeyRing. Complete disabled Async/PreviewTee/RunLog holders cover normal and FVTableHelper destruction. Disabled helpers/preview exports are inert. Control opens no listener. Thirteen advanced Injector TUs are exact-5.1-only; the eight corrected own headers remain portable or guarded. AnomalyUnsupportedTexture returns false/unavailable, empty modes and zero measurements and never claims applied, ready or measured.

Compat covers skinned-asset identity, static RenderData, static/skeletal LOD access, camera POV/near clip, contact shadows, collision/frustum construction, constants, optional scene-capture flags and TFunction reset. The 5.1 branches retain original expressions. Capture has no UPROPERTY/UFUNCTION surface; its frame/snapshot references and owning holders are native/opaque. Injector’s six reflected material/texture fields and OnWatchedTargetEndPlay(AActor*, EEndPlayReason::Type), and Bench’s three component fields, are portable. No reflected engine-specific RHI, skinned-asset or transport type remains.

| Phase | Non-reflected seam | Future 4.25 implementation |
|---|---|---|
| C | IAnomalyCaptureBackend / CreateCaptureBackend425, Async owner | Source/AnomalyCapture/Private/Compat/AnomalyCaptureBackend425.cpp |
| E | IAnomalyTextureAdapter / CreateTextureAdapter425, IAnomaly facade | Source/AnomalyInjector/Private/Compat/AnomalyTextureAdapter425.cpp |
| F | FAnomalyControlTransportState opaque holder | Source/AnomalyControlServer/Private/Compat/AnomalyControlTransport425.cpp |

C/E remain declarations only; no 5.1 backend was rewired. Later backend implementation slots do not require another reflected declaration change for these seams.

Final ancestry audit: 853 source/tool/host files; 6,378 assets scanned for ASCII/UTF-16 TickableWorldSubsystem, with zero matches and no asset loads. Source matches are retained for review; baseline engine-parent declarations are expected. EA11 passes 7 reflected headers and its forbidden-directive mutant; the shim-specific mutant also fails as intended. Asset-registry ancestry remains deferred to the first editor launch.

The 283 broad source matches include 57 TickableWorldSubsystem references: 52 are the two candidate copies' shim declarations, definitions and includes; five are the unchanged baseline parent declarations. There is no remaining cast, class-path or subsystem-array consumer of the old parent in the candidate source or host sources. The source lever audit passes 54 registrations and four lookups, including the new default-off TickAudit lever.

**File-by-file 5.1 reach**

These 103 paths join exactly to `git diff --name-only d7eee97..204724a7d089463767518f28723b077f7f058ab1`. N = changed but intended neutral; C = changed tooling/diagnostic/host surface; B = byte-identical retained planned file. N is not a runtime-parity claim. [Full reach manifest](<D:/IA_Europa/Receipts/09407/report-data/reach.json>).

| File | Class | Change / reach / covering gates |
|---|---|---|
| `AnomalyInjector.uplugin` | N | A2 legacy descriptor aliases preserve module membership; real matrix and mutant rerun in A3; reaches Module inclusion, startup and package exclusion; gates R1, R2, R8, R9 |
| `CLAUDE.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `Source/AnomalyBench/AnomalyBench.Build.cs` | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect); reaches Compiler, standard, dependency closure, module selection and startup; gates R1, R2, R8, R9 |
| `Source/AnomalyBench/Private/AnomalyBenchModule.cpp` | N | A2 disabled 4.25 bench implementation and module diagnostic; enabled body retained; reaches Shared compile/include/ownership and feature initialization; enabled-route timing and lifetime regression; gates R1, R2, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyBench/Private/AnomalyBenchTwoPartActor.cpp` | N | A2 disabled component construction on 4.25; 5.1 constructor retained; reaches Shared compile/include/ownership and feature initialization; enabled-route timing and lifetime regression; gates R1, R2, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyBench/Private/AnomalyBenchTwoPartActor.h` | N | Three null raw reflected component pointers; names, specifiers and GC traversal retained; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyCapture/AnomalyCapture.Build.cs` | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect); reaches Compiler, standard, dependency closure, module selection and startup; gates R1, R2, R8, R9 |
| `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | N | Shim lifecycle and portable shell; complete disabled ownership and preview symbols; renderer-dependent helper bodies guarded; enabled Capture token stream preserved; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyCapture/Private/AnomalyCensus.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyChangeMeasurement.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyChangeStage.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyFrameCapturer.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyFrameCapturer.h` | N | Disabled strict-header TU must not parse renderer or PixelFormat includes; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyLabelWriter.h` | N | Disabled strict-header TU must not parse renderer or PixelFormat includes; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyMeasurability.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyPreviewCapture.cpp` | N | Disabled preview exports clear outputs and return unavailable; 5.1 encoding untouched; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyPreviewTee.h` | N | Complete disabled type for default, FVTableHelper and destructor TUniquePtr paths; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalyRunLog.cpp` | N | Complete inert run-log ownership and all virtual methods; no registration or writes at 0; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Private/AnomalySveCapturer.cpp` | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h` | N | Reparent to literal shim; move world eligibility to shared Super-first policy; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyCapture/Public/Compat/AnomalyCaptureBackend.h` | N | Phase C native interface/factory declarations only; no 5.1 rewiring; reaches Enabled capture, labels/masks, frame association, readiness and object lifetime; gates R1, R4, R5, R6, R7, R8 |
| `Source/AnomalyControlServer/AnomalyControlServer.Build.cs` | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect); reaches Compiler, standard, dependency closure, module selection and startup; gates R1, R2, R8, R9 |
| `Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp` | N | Remove derived world-type override; preserve existing lifecycle call placement; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyControlServer/Private/Compat/AnomalyControlTransportState.h` | N | A2 opaque non-reflected transport ownership; complete disabled type; reaches Shared compile/include/ownership and feature initialization; enabled-route timing and lifetime regression; gates R1, R2, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyControlServer/Private/ControlProtocol.cpp` | N | No protocol/snapshot work or unavailable dependencies on the disabled transport path; reaches Transport/snapshot protocol and teardown; gates R1, R8, R9 |
| `Source/AnomalyControlServer/Private/ControlSnapshot.cpp` | N | No protocol/snapshot work or unavailable dependencies on the disabled transport path; reaches Transport/snapshot protocol and teardown; gates R1, R8, R9 |
| `Source/AnomalyControlServer/Public/AnomalyControlServerSubsystem.h` | N | Reparent to literal shim; move world eligibility to shared Super-first policy; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/AnomalyInjector.Build.cs` | C | Exact shared rules; 4.25-only C++17/NoPCHs; unchanged 5.1 rule objects apart from accepted A2 surface; ED6 once-per-target diagnostic; reaches Compiler, standard, dependency closure, module selection and startup; gates R1, R2, R8, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Time, camera, LOD popping, material swaps and bench twins; cadence, pixel labels and restoration; gates R1, R4, R5, R6, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.h` | N | Self-contained disabled native header in strict 4.25 TUs; reaches Time, camera, LOD popping, material swaps and bench twins; cadence, pixel labels and restoration; gates R1, R4, R5, R6, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_ChangeCase.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Time, camera, LOD popping, material swaps and bench twins; cadence, pixel labels and restoration; gates R1, R4, R5, R6, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Time, camera, LOD popping, material swaps and bench twins; cadence, pixel labels and restoration; gates R1, R4, R5, R6, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_LodPopping.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Time, camera, LOD popping, material swaps and bench twins; cadence, pixel labels and restoration; gates R1, R4, R5, R6, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Time, camera, LOD popping, material swaps and bench twins; cadence, pixel labels and restoration; gates R1, R4, R5, R6, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.h` | N | Self-contained disabled native header in strict 4.25 TUs; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.h` | N | Self-contained disabled native header in strict 4.25 TUs; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_TimeDilation.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; EG6 own header first, portable or guarded on 4.25, outer implementation guard retained; reaches Time, camera, LOD popping, material swaps and bench twins; cadence, pixel labels and restoration; gates R1, R4, R5, R6, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/TexCorruptBench.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/TexCorruptCore.h` | N | Self-contained disabled native header in strict 4.25 TUs; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/TexCorruptState.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp` | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp` | N | 4.25 allowlist before draw; 5.1 pool, ordering, unknown-ID response and admission tokens equal release; native reset adapter; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Private/AnomalyHiddenClass.cpp` | N | Contact-shadow API boundary; other hide and restore flags retained; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/AnomalyInjectorModule.cpp` | N | Stock 4.25 explicit false Nanite probe; no AllowNanite override; 5.1 probe ownership unchanged; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp` | N | Shim lifecycle; 4.25 registry/asset boundary; 5.1 constructor, Register and Apply tokens equal release; native reset adapter; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Private/AnomalyLod.cpp` | N | Route static render data, screen thresholds and predicted skeletal LOD through native version adapters; same conventions; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/AnomalySelectorSubsystem.cpp` | N | 4.25 skips unsupported choices; 5.1 selection and HUD tokens equal release; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Private/AnomalyViewport.cpp` | N | Portable camera/data/contact/collision/frustum/axis/constant/visibility access; unchanged 5.1 operations; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Compat/AnomalyCapabilities.cpp` | N | Explicit allowlist and bench-only missing_object on 4.25; unconditional pass-through on 5.1; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Compat/AnomalyEngineCompat.cpp` | N | Native skinned/static data, camera/near clip/contact/collision/LOD/frustum/axis/optional flag adapters; original 5.1 expressions; stock 4.25 substitutions; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Private/Compat/AnomalyTickableWorldSubsystem.cpp` | N | Single FTickable registration; 4.25 IsTickable lifetime gate; default-off audit only; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Private/Compat/AnomalyUnsupportedTexture.cpp` | N | Complete 4.25 portable texture facade; never applied/eligible/ready/measured; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Public/AnomalyAutoInjectorSubsystem.h` | N | Reparent to literal shim; move world eligibility to shared Super-first policy; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h` | N | Literal shim parent; five null raw reflected asset pointers and one empty raw-pointer array, with names/specifiers retained; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Public/AnomalySelectorSubsystem.h` | N | Reparent to literal shim; move world eligibility to shared Super-first policy; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Public/Compat/AnomalyCapabilities.h` | N | Single capability admission policy for registry, auto pool and selector; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Public/Compat/AnomalyEngineCompat.h` | N | Portable native adapters and correct class forward declaration; TFunction reset and constants preserve 5.1 operations; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Public/Compat/AnomalyEngineVersion.h` | N | A2 exact engine predicates and unsupported-version diagnostic; reaches Shared compile/include/ownership and feature initialization; enabled-route timing and lifetime regression; gates R1, R2, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Public/Compat/AnomalySharedPointerIncludes.h` | N | A2 portable defining shared-pointer includes outside reflected UHT parse; reaches Shared compile/include/ownership and feature initialization; enabled-route timing and lifetime regression; gates R1, R2, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Public/Compat/AnomalyStatsIncludes.h` | N | A2 defining stats includes outside reflected UHT parse; reaches Shared compile/include/ownership and feature initialization; enabled-route timing and lifetime regression; gates R1, R2, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyInjector/Public/Compat/AnomalyTextureAdapter.h` | N | Phase E native adapter/factory declarations only; existing 5.1 facade unchanged; reaches Injector admission, targets, materials, LOD/camera/hide state and enabled anomaly routes; gates R1, R3, R4, R5, R7, R9 |
| `Source/AnomalyInjector/Public/Compat/AnomalyTickableWorldSubsystem.h` | N | Literal shared reflected ancestry and engine-equivalent 5.1 lifetime/tick contract; reaches Reflected ancestry, lifetime, tick cadence, GC and dependent injector/capture/control behavior; gates R1, R3, R4, R5, R6, R7, R8, R9 |
| `Source/AnomalyShaders/AnomalyShaders.Build.cs` | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect); reaches Compiler, standard, dependency closure, module selection and startup; gates R1, R2, R8, R9 |
| `Source/AnomalyShaders/Private/AnomalyShadersModule.cpp` | N | A2 disabled 4.25 shader registration and diagnostic; 5.1 mapping retained; reaches Shared compile/include/ownership and feature initialization; enabled-route timing and lifetime regression; gates R1, R2, R3, R4, R5, R6, R7, R8, R9 |
| `docs/PRE-DELIVERY-CHECKLIST.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/architecture.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/europa/09404-working-result.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/europa/09407-working-result.md` | C | Scoped tooling, falsification checks or explicit evidence documentation; reaches Build/write admission and evidence correctness; gates R0, R1, R2 |
| `docs/europa/phase-a-a3-closure.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/europa/phase-a-reach.json` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/europa/phase-a-routing.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/predictions/094-europa-phase-a.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/sessions/2026-10-16-094-03-europa-a1-a2.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/sessions/2026-10-22-094-04-europa-a3.md` | C | Scoped setup, build/evidence tooling or explicit port documentation; no runtime-neutrality claim; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `docs/sessions/2026-10-22-094-05-europa-a3-resume.md` | C | Scoped tooling, falsification checks or explicit evidence documentation; reaches Build/write admission and evidence correctness; gates R0, R1, R2 |
| `docs/sessions/2026-10-24-094-07-europa-a3-finish.md` | C | Scoped tooling, falsification checks or explicit evidence documentation; reaches Build/write admission and evidence correctness; gates R0, R1, R2 |
| `tools/europa/DescriptorProbe.cs.txt` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/build_memory.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Build/write admission and evidence correctness; gates R0, R1, R2 |
| `tools/europa/check_reflected_headers.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/compile_children.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/descriptor_matrix.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/finish_self_tests.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Build/write admission and evidence correctness; gates R0, R1, R2 |
| `tools/europa/host_probe/EuropaPhaseAProbe.Build.cs.txt` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/host_probe/EuropaPhaseAProbeModule.cpp.txt` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/host_probe/TargetRules.fragment.txt` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/idle_gate.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/ledger_budget_self_tests.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/ledger_delta_self_tests.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/process_wrapper.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/recovery_cleanup.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Build/write admission and evidence correctness; gates R0, R1, R2 |
| `tools/europa/resume_self_tests.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Build/write admission and evidence correctness; gates R0, R1, R2 |
| `tools/europa/tooling_self_tests.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/ubt_admission.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/working_build.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/europa/write_ledger.py` | C | Scoped proof tooling; current EG1 bookkeeping, EF1 memory supervision, EG2 product/flag receipts, EG3 mandatory output and deterministic can-fails where applicable; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |
| `tools/strict_include_build.py` | C | Explicit owned host/engine roots, UE4Editor paths and no 4.25 IncludeOrderVersion; EE1 environment override removed and both UBT calls use EF1; synthetic command wiring only, actual strict lever deferred to A4; reaches Host/evidence selection, source restore, build admission and claim completeness; gates R0, R1, R2 |

Planned files needing no edit:

- B — `Content/Materials/M_CorruptTex_Normal.uasset`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Content/Materials/M_CorruptTex_UV.uasset`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Content/Materials/M_CorruptedTexture_Pink.uasset`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Content/Materials/M_MissingTexture_Checker.uasset`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Content/Textures/T_CorruptTex_NoiseN.uasset`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Shaders/Private/AnomalyMaskReduce.usf`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Shaders/Private/AnomalyTexStats.usf`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Shaders/Private/AnomalyVisibleMask.usf`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyCapture/Private/AnomalyAsyncWriter.cpp`: Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable.
- B — `Source/AnomalyCapture/Private/AnomalyCaptureLetterbox.cpp`: Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable.
- B — `Source/AnomalyCapture/Private/AnomalyCaptureModule.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyCapture/Private/AnomalyChangeFamilyData.cpp`: Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable.
- B — `Source/AnomalyCapture/Private/AnomalyMaskMeasure.cpp`: Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable.
- B — `Source/AnomalyCapture/Private/AnomalyPreviewTee.cpp`: Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable.
- B — `Source/AnomalyCapture/Private/AnomalyStencilTag.cpp`: Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable.
- B — `Source/AnomalyCapture/Private/AnomalySveKeyRing.cpp`: Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable.
- B — `Source/AnomalyInjector/Private/Anomalies/Anomaly_Blinking.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyInjector/Private/Anomalies/Anomaly_LightingMismatch.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyInjector/Private/Anomalies/Anomaly_LodCorruption.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingObject.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyInjector/Public/AnomalyDefaults.h`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyShaders/Private/AnomalyMaskReduceShader.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyShaders/Private/AnomalyTexStatsShader.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyShaders/Private/AnomalyVisibleMaskShader.cpp`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyShaders/Public/AnomalyMaskReduceShader.h`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyShaders/Public/AnomalyTexStatsShader.h`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `Source/AnomalyShaders/Public/AnomalyVisibleMaskShader.h`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `tools/check_package_descriptor.py`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.
- B — `tools/verify_capture.py`: Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending.

**Commits, artifacts and A4 handoff**

All commits are local Conventional Commits on feat/europa-port, with both date variables absent. Nothing was pushed. Author and committer times are recorded below.

- `c0dc592d6ad6c0f6807c006f1a397b1d70501098 2026-10-24T14:10:51+05:30 2026-10-24T14:10:51+05:30 fix(europa): gate working builds on memory and complete receipts`.
- `286f2ee0ca6ffc9e9e1fbd8c3caaefa01c8a91fd 2026-10-24T14:14:17+05:30 2026-10-24T14:14:17+05:30 fix(europa): put anomaly headers before disabled implementation guards`.
- `59fbf11c72cca93df8a453fd206a0d521bbf52ba 2026-10-24T14:50:15+05:30 2026-10-24T14:50:15+05:30 fix(europa): finish build receipts and record the A3 gate handoff`.
- `204724a7d089463767518f28723b077f7f058ab1 2026-10-24T14:54:32+05:30 2026-10-24T14:54:32+05:30 docs(europa): clarify audit evidence and 4.25 admission scope`.

Initial predictions ac81ecb at 2026-10-16 16:32:37 IST preceded first UBT at 16:47:19. The 09407 addendum was committed in 286f2ee at 14:14:17 IST before fresh exports and working builds. No gate threshold was retuned after a result.

| Final host binary | SHA-256 |
|---|---|
| 425: `D:\IA_Europa\StylizedParisStreet\Binaries\Win64\UE4Editor-EuropaPhaseAProbe.dll` | `cf76232d9289134d5709f00fde8b3ab3685ce4ce84570bd5c2b44f6c1e5f8d7c` |
| 425: `D:\IA_Europa\StylizedParisStreet\Binaries\Win64\UE4Editor-EuropaPhaseAProbe.pdb` | `00e9d2e94a89d1fb630d04c3aa7b1caeacbf2c7a745867b1094ce864c5e0833a` |
| 425: `D:\IA_Europa\StylizedParisStreet\Binaries\Win64\UE4Editor-StylizedParisStreet.dll` | `c391bde91d6d3a9d2ab91e3dbe47137fbce60fb4d7769bd113b6f34d1a71e27d` |
| 425: `D:\IA_Europa\StylizedParisStreet\Binaries\Win64\UE4Editor-StylizedParisStreet.pdb` | `3754955d918b6e18a3b4be209eb227ebe08b5a61d752d7b6982612c4bad1db78` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyBench.dll` | `45a59ddc9b59ca8be29e34f28fd562adb43d4ea7d0269c823ce0905dc3fc376c` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyBench.pdb` | `18dacec20d1da028010a1bf853d95ee26b16af3ae594084e4dcbe6cec55aa1d3` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyCapture.dll` | `259efa7d07cb09f9fce44ab7098426813240bc97a231968da18be9db04ea21c9` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyCapture.pdb` | `7e1570384d7a17678a67f1d614f84a80c9e50f46f93e89488f24d473d6de1974` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyControlServer.dll` | `294a5199a0673bf369d65ec3bc27b99f1cf7d1f4818e26e67653510b9d4658e5` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyControlServer.pdb` | `206cecb109f39e8f26d567a6ae918ec4b5e1f80cb181f140c3705dfad9fa61e8` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyInjector.dll` | `287e5622d9bb25d35d93370154cf0bd60380a4a921f1210d538d0937533ffa09` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyInjector.pdb` | `6b6c8b1fa0f24968520d70e8a7b61186d0957dc248061ad313a77c2a19a2a30e` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyShaders.dll` | `24639a1005ca5cd6b71e94562449cbca604f64aba2f136b98e10e9a37bf6a491` |
| 425: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyShaders.pdb` | `04e7f24f441733e5ed7ff83d2a307791b5d3b30d6d04bb23baa77fc89c538aff` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Binaries\Win64\UnrealEditor-EuropaPhaseAProbe.dll` | `c32b52eee966dec07c30512e30dffa7a2c2be4c8aecebd78dc5faf72189e70d0` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Binaries\Win64\UnrealEditor-EuropaPhaseAProbe.pdb` | `b6aecd32c6da1e74bd57965584244725e824067264c7773f6c3497d0bdc9ba0a` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Binaries\Win64\UnrealEditor-StackOBot.dll` | `376a04c07330d3c9db8a29b27bf073a2e6a76f49ad7549607e29bcb9f7aa265a` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Binaries\Win64\UnrealEditor-StackOBot.pdb` | `4c6a850851b42c633ae3dd6875a5a330f9b5ef9dbba0f697d5a9fd278d455dc6` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyBench.dll` | `981c3d78a10545a943db788fe3ae6dd7429b7b1071babf8fcaf332b05a8204cc` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyBench.pdb` | `31dda0c57a69035d51ad8ce769eafa132d78349beeb9e52b138f4b2c11eaa664` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyCapture.dll` | `6947b1b2055046a6c7af501b87071bdc3bd972eccb9821d609344934e1979b74` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyCapture.pdb` | `afb093eca7f3f77201c52f6e86a626061b80714a5eadf08ca83456fad63d5def` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyControlServer.dll` | `72d774476679676c9bb4809a411b8743ff35a6cc2f9265554011275d7314a37b` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyControlServer.pdb` | `c0cfbbaeeccde8f2f99674c5a6e931e1e379f60bd6100d0376945bfb1e1a850e` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyInjector.dll` | `75e546e279ea87b94d65a6adf5e7c544bdb394f809a0e4147708424f59c78456` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyInjector.pdb` | `e756668df5939cb4f15326100984915af3d458adfa386a828dbbcc0ecc34adb7` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyShaders.dll` | `8b6eee6e705cd7edf5d9e48d91dc6f84794af65cf001509cc40d7962407122ac` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Binaries\Win64\UnrealEditor-AnomalyShaders.pdb` | `cd4982676f67e85ea64773b93480c3e9567a88b3c8900a7fe2da01de57a27021` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\CaptureBench\Binaries\Win64\UnrealEditor-CaptureBench.dll` | `a21dfe35f503d0e8de3fb371132278f23958514243f8e5b8791058a63a7b49b0` |
| 51port: `D:\IA_Europa\ReGate51\StackOBot\Plugins\CaptureBench\Binaries\Win64\UnrealEditor-CaptureBench.pdb` | `61584caba2ac7bde37288d5c1b70872433a0c91d6e5abcc5b262f93a9176e3be` |

Produced intermediate PDBs, response hashes and complete flags are retained in the build receipts; no binary archive or staging swap was made. No bench lever/CVar was changed, and no runtime profile was launched.

A4a/A4b must use the final source SHA and their own final normal/strict builds: 4.25 Editor/Game/Shipping and 5.1 baseline/candidate, including the two monolithic Game builds when authorized. Recheck 14.38 selection before every 5.1 build. Keep EF1, scoped routing, P9 v2 and EG1 accounting; A4 decides any further cleaning. The strict tool has explicit roots, UE4Editor paths and no 4.25 IncludeOrderVersion. Its obsolete UHT environment override is removed; both UBT calls use EF1 and synthetic wiring checks pass. Its real lever, restoration, per-action receipts and header-TU proof have not run. The host probe needs behavior in A5; asset-registry, world/CDO, owner preview/eye, AA-off/TAA/TSR, pixel, packaged descriptor and delivery qualification remain outstanding.

Strict-tool readiness limit: its EF1 stop currently propagates through source restoration and exits; A4 must connect stopped-output cleaning/retry and complete binary receipts before running strict legs. The working-build wrapper already implements that loop. A4 must also select new receipt/temp roots and preserve 09407 evidence. No strict leg was attempted here.

No unresolved A3 design or STOP item remains. Working build success does not certify label/picture synchronization or 5.1 runtime equivalence. A3 stops here; no A4 job was started.

VERDICT: GREEN | A3 shared closure complete; final 4.25/5.1 working pair and Baseline51 clean-engine query pass; P9 v2 checks pass; A4 gates pending; no push
