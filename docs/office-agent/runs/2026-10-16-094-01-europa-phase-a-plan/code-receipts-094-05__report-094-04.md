# 094-04 — Europa Phase A job A3: shared C++ closure

**NEEDS-DECISION. A3 is incomplete.** The first 4.25 working compile passed UHT but failed C++. Its write ledger then found non-exempt UHT configuration writes on C:, triggering the required STOP. No second build or compiler fix followed; no 5.1 C++ compile ran.

- **Protected state:** entry and exit both match the accepted P9 baseline: 707,775 file inventories, 25,866 binary/config full hashes, protected refs/worktrees and pause receipts. Both engine trees are unchanged.
- **Completed:** EC1 host plugin restoration; ED3 pinned toolchain checks; 8 paired 5.1 and 5 expected 4.25 JsonExports; all three restored mutants; closure edits and backend declarations; EA11; ancestry audit; compact ledger tests; strict-tool adaptation. The closure still needs compiler fixes.
- **Working result:** `9fdc8f0`, 4.25 Editor Development, UHT PASS, C++ exit 6. Admitted actions: project 11 / plugin 24 / engine 0. Partial host/probe DLLs only.
- **STOP:** UHT replaced one 112-byte crash config and rewrote ten two-byte ini files under `%LOCALAPPDATA%/UnrealHeaderTool/Saved/Config`. Zero net growth does not exempt the location. No C: restore or deletion was attempted.
- **Current branch:** `feat/europa-port` at `668c075233b755001766ac48003183fb6af4dc68`; clean and unpushed. ReGate51 remains clean at compiled source `9fdc8f0`; Baseline51 remains at `d7eee97`. The later feature commits contain documentation only.
- **Decision needed:** admit a narrowly bounded UHT configuration exception, or approve a demonstrated routing design before another UHT invocation. Then resume the recorded compiler closure work. A4 is not ready to start.

Evidence root: `D:/IA_Europa/Receipts/09404`. Report date: 2026-10-22. Only the requested A3 scope was attempted. No editor/game/commandlet/cook/UAT, Game/Shipping build, Baseline51 build, strict run, clean/rebuild, engine repair, toolset installation, network, push or tag operation ran.

## Entry, exit and disk proof

The accepted full baseline remains `Receipts/P9/baseline/full-sha256.jsonl`, SHA-256 `db884f1e9c3debd7b9ed678db08036509959b1fc5a0d27799b10ecccb0625cce`. I did not repeat its full-file hash pass. Each per-job proof enumerated protected path/size/mtime/file identity and fully hashed the required binaries/configs: 119,771,817,645 bytes across 25,866 files. Both inventories contain 707,775 files and zero differences. Git refs, protected heads, index hashes/status, staging and the 11:00 pause receipts also compare equal. The permitted feature ref and three owned worktrees are recorded separately.

The original Paris and selected StackOBot copy sources still equal their A1 before-copy manifests. The J2 installed candidate remains installed and frozen; nothing was restored or substituted. The approved absent partial archive state remains absent. These P9 results do not erase the separate C: UHT write STOP.

Receipts: `P9/entry/`, `P9/exit/`, `entry-preflight.json`, `entry-completed.json`, `exit-completed.json`. Enumeration failures are fatal on protected paths. No unapproved J2 evidence was opened for feature work.

Resolved volumes are physical C:, D: and E:; the owned root resolves to physical D:. ED1 limits are C start/warn/floor 13/11/10 GiB, D 70/55/50, E 25/20/15. The entry was 26.26/129.66/33.45 GiB. No observed reading breached a warning or floor. Free space is a floor monitor, not write attribution.

| Operation | C GiB before → after | D GiB before → after | E GiB before → after | Ledger |
|---|---|---|---|---|
| 01-hosts-toolchains | 26.21 → 25.89 | 108.89 → 108.68 | 33.38 → 31.84 | PASS |
| 02-descriptor-matrices | 25.67 → 25.61 | 108.68 → 123.61 | 31.45 → 30.91 | PASS |
| 03-cpp-closure | 25.61 → 25.49 | 123.61 → 123.13 | 30.85 → 30.42 | PASS |
| 04-build425-01 | 25.48 → 25.48 | 123.13 → 121.72 | 30.40 → 30.37 | STOP |
| 05-stop-evidence | 25.48 → 25.48 | 121.72 → 121.47 | 30.32 → 28.85 | PASS |

EA8 exit free space: C: 25.48 GiB, D: 121.66 GiB, E: 30.21 GiB. Every operation also records its resolved paths and thresholds; running children check floors every 20 seconds.

## Toolchains, hosts, plugin set and descriptor proof

ED3 passed before the working build. Read-only vswhere reports VS Community 2022 **17.12.35707.178** and VS Build Tools 2017 **15.9.34031.82**. Installed MSVC directories are 14.36.32532, 14.38.33130 and 14.42.34433 under VS2022, and 14.16.27023 under VS2017. No 14.44 directory was found. SDK Include/Lib/bin trees for 10.0.17763.0 and 10.0.22621.0 exist. Nothing was installed or changed.

| Engine | Fresh UBT selection | Compiler file/product | SDK / standard |
|---|---|---|---|
| 5.1 | VS2022 directory 14.38.33130 | cl file 19.38.33144; product/UBT 14.38.33144 | 10.0.22621.0; module CppStandard Default |
| 4.25 | VS2017 directory 14.16.27023 | cl file 19.16.27051; product/UBT 14.16.27051 | 10.0.17763.0; plugin Cpp17 |

Receipts: `toolchains/installed.json`, `vswhere.json`, `51base-selection.json`, `51port-selection.json`, `425-selection.json`. The exact profile line is Console.WriteLine output in the receipted child stdout, not the UBT file log. The working 4.25 UBT log independently confirms the compiler and SDK. No 5.1 fallback was selected.

Hosts remain `D:/IA_Europa/StylizedParisStreet`, `D:/IA_Europa/Baseline51/StackOBot` and `D:/IA_Europa/ReGate51/StackOBot`, with their plugin worktrees under `Plugins/AnomalyInjector`. No new host copy was required. Existing copy manifests remain in `Receipts/09403/copies`; this job's source-equality proofs are under `P9/{entry,exit}`. CaptureBench remains the A1 git-archive export of `e64d6f9a43b7aa2e5eb041b7dab14e5285bb911b` from its own repository, never from a J2 installed copy.

EC1 restores EnhancedInput, ControlRig, FullBodyIK, Metasound and GameplayInsights in both 5.1 copies. Effective plugin sets equal the original except disabled, uncopied RoomGenerator and UnrealMCPython. AnomalyInjector is explicitly enabled in the copies and default-enabled in the original, so that spelling is not an effective-set change. Each 5.1 export union has **140 enabled plugins**; **239 Editor Development engine-plugin binary paths** per host exist. The proof uses equal explicit flags plus unchanged engine defaults/descriptor graph and the copied hosts' UBT logs; no UBT invocation was made against the protected original. The original-source inventory and engine binary hashes remain unchanged.

`hosts/plugin-reference-diff.json` and `hosts/effective-plugins-and-binaries.json` contain the full sets and paths. `stop/host-final-manifests.json` hashes each host's .uproject and Source files. The two 5.1 probe source sets are identical.

ED7 adds only an empty Core-dependent EuropaPhaseAProbe module, its Build.cs and IMPLEMENT_MODULE class. No reflected probe class, behavior, Compat include or shim-audit access was needed. Each .uproject uses legacy `BlacklistTargetConfigurations: ["Shipping"]`. Each Game/Editor Target.cs roots the module through UBT's own `ModuleDescriptor.IsCompiledInConfiguration` predicate. The initial export showed that a declaration alone did not root the module; the dated addendum and target correction were committed before retesting. Native predicate admission lets the Shipping exclusion mutant actually change the export.

The successful matrices below include the probe. “IA5” means AnomalyInjector, AnomalyCapture, AnomalyControlServer, AnomalyShaders and AnomalyBench; “none” means neither IA nor probe. All pairs are Win64. These are JsonExports, not Game/Shipping builds.

| Comparison | Target | Config | Total modules | IA/probe | Result |
|---|---|---|---|---|---|
| 5.1 port = release | StackOBot | Debug | 336 | IA5 + probe | PASS |
| 5.1 port = release | StackOBot | DebugGame | 336 | IA5 + probe | PASS |
| 5.1 port = release | StackOBot | Development | 336 | IA5 + probe | PASS |
| 5.1 port = release | StackOBot | Shipping | 302 | none | PASS |
| 5.1 port = release | StackOBot | Test | 330 | IA5 + probe | PASS |
| 5.1 port = release | StackOBotEditor | Debug | 742 | IA5 + probe | PASS |
| 5.1 port = release | StackOBotEditor | DebugGame | 742 | IA5 + probe | PASS |
| 5.1 port = release | StackOBotEditor | Development | 742 | IA5 + probe | PASS |
| 4.25 expected | StylizedParisStreet | DebugGame | 305 | IA5 + probe | PASS |
| 4.25 expected | StylizedParisStreet | Development | 305 | IA5 + probe | PASS |
| 4.25 expected | StylizedParisStreet | Shipping | 278 | none | PASS |
| 4.25 expected | StylizedParisStreetEditor | DebugGame | 585 | IA5 + probe | PASS |
| 4.25 expected | StylizedParisStreetEditor | Development | 585 | IA5 + probe | PASS |

Full 5.1 module memberships equal d7eee97 for all eight pairs, not just the IA subset. Exact-version rule branches compile in both UBTs. ED6 emits seven profile lines for eight 5.1 targets and four for five 4.25 targets: Shipping has no included IA module to emit one. No target emits a second line in the same UBT run. Reproductions and raw matrices: `matrices/51base-v2`, `51port-profile-fix`, `425-v2`, `51-comparison.json`, `step3-summary.json`.

No C#, descriptor or .uproject changed after the successful step-3 set. Later changes were C++/tooling/documentation, so another descriptor set was not required before the STOP. The proposed PCH fix is unapplied; if it changes C#, it will require the full set again.

| Mutant | Shipping change | Restore/re-export |
|---|---|---|
| 5.1 plugin blacklist removed for AnomalyShaders | AnomalyShaders appears | none; original bytes restored |
| 5.1 probe exclusion removed | EuropaPhaseAProbe appears | none; original bytes restored |
| 4.25 probe exclusion removed | EuropaPhaseAProbe appears | none; original bytes restored |

Mutation/restore exports are in `matrices/*mutant*` and `*restored*`; original bytes and hashes are retained in `mutants/`. No mutant was compiled or left installed.

## Closure and actual reach manifest

The source is committed but **not a compiling closure**. The actual changed-path join is 96/96 against `git diff --name-only d7eee97..HEAD`, with zero unassigned paths; 29 planned/audited retained files are byte-identical. `docs/europa/phase-a-reach.json` and `closure/final-reach-join.json` bind the join to the final SHA. A2 files are included because the requested base is d7eee97; they are not all new A3 edits.

Effect classes: **N** changed but intended neutral on 5.1, pending qualification; **C** deliberately changed diagnostic/tool/host/document behavior; **B** byte-identical. R1 is builds/strict/toolchain, R2 descriptor, R3 lifetime/ticks/GC, R4 alignment, R5 pixels/labels, R6 speed, R7 renderer/material/state, R8 control/teardown, R9 catalogue/UI/bench. R0 covers environment/evidence. The JSON adds each file's reached-behavior description; the table gives every changed file, actual change/reason and covering gates.

| Changed file (relative to plugin) | 5.1 | Change / reason | Reach gates |
|---|---|---|---|
| AnomalyInjector.uplugin | N | A2 legacy descriptor aliases preserve module membership; real matrix and mutant rerun in A3 | R1,R2,R8,R9 |
| CLAUDE.md | C | Record A3 status, failed working compile and write STOP; no production change | R0,R1,R2 |
| Source/AnomalyBench/AnomalyBench.Build.cs | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect) | R1,R2,R8,R9 |
| Source/AnomalyBench/Private/AnomalyBenchModule.cpp | N | A2 disabled 4.25 bench implementation and module diagnostic; enabled body retained | R1,R2,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyBench/Private/AnomalyBenchTwoPartActor.cpp | N | A2 disabled component construction on 4.25; 5.1 constructor retained | R1,R2,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyBench/Private/AnomalyBenchTwoPartActor.h | N | Three null raw reflected component pointers; names, specifiers and GC traversal retained | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyCapture/AnomalyCapture.Build.cs | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect) | R1,R2,R8,R9 |
| Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp | N | Literal shim parent and portable shell includes; complete owned disabled types, but remaining capture helper guards fail the first 4.25 compile | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyCapture/Private/AnomalyCensus.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyChangeMeasurement.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyChangeStage.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyFrameCapturer.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyFrameCapturer.h | N | Disabled strict-header TU must not parse renderer or PixelFormat includes | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyLabelWriter.h | N | Disabled strict-header TU must not parse renderer or PixelFormat includes | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyMeasurability.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyPreviewCapture.cpp | N | Disabled preview exports clear outputs and return unavailable; 5.1 encoding untouched | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyPreviewTee.h | N | Complete disabled type for default, FVTableHelper and destructor TUniquePtr paths | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalyRunLog.cpp | N | Complete inert run-log ownership and all virtual methods; no registration or writes at 0 | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Private/AnomalySveCapturer.cpp | N | Move all capture implementation/includes behind availability; enabled 5.1 body retained | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyCapture/Public/Compat/AnomalyCaptureBackend.h | N | Phase C native interface/factory declarations only; no 5.1 rewiring | R1,R4,R5,R6,R7,R8 |
| Source/AnomalyControlServer/AnomalyControlServer.Build.cs | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect) | R1,R2,R8,R9 |
| Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp | N | Remove derived world-type override; preserve existing lifecycle call placement | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyControlServer/Private/Compat/AnomalyControlTransportState.h | N | A2 opaque non-reflected transport ownership; complete disabled type | R1,R2,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyControlServer/Private/ControlProtocol.cpp | N | No protocol/snapshot work or unavailable dependencies on the disabled transport path | R1,R8,R9 |
| Source/AnomalyControlServer/Private/ControlSnapshot.cpp | N | No protocol/snapshot work or unavailable dependencies on the disabled transport path | R1,R8,R9 |
| Source/AnomalyControlServer/Public/AnomalyControlServerSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/AnomalyInjector.Build.cs | C | A2 exact-version rules remain intended neutral; ED6 deliberately changes profile logging from per module to once per target, preserving its content | R1,R2,R8,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R4,R5,R6,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.h | N | Self-contained disabled native header in strict 4.25 TUs | R1,R4,R5,R6,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_ChangeCase.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R4,R5,R6,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R4,R5,R6,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_LodPopping.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R4,R5,R6,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R4,R5,R6,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.h | N | Self-contained disabled native header in strict 4.25 TUs | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.h | N | Self-contained disabled native header in strict 4.25 TUs | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_TimeDilation.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R4,R5,R6,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptBench.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptCore.h | N | Self-contained disabled native header in strict 4.25 TUs | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptState.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Anomalies/TexCorruptUniform.cpp | N | Exclude unavailable implementation and includes on 4.25; keep enabled 5.1 tokens | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp | N | Filter unsupported admission before random draw; preserve all nine 5.1 keys, arrays and draw order | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Private/AnomalyHiddenClass.cpp | N | Contact-shadow API boundary; other hide and restore flags retained | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/AnomalyInjectorModule.cpp | N | Stock 4.25 explicit false Nanite probe; no AllowNanite override; 5.1 probe ownership unchanged | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp | N | Retain 5.1 implementation; unavailable 4.25 facade fails closed | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Private/AnomalyLod.cpp | N | Preserve LOD conventions and 5.1 getter | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/AnomalySelectorSubsystem.cpp | N | Selector uses common admission; no working choice for unsupported IDs; unchanged 5.1 ordering | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Private/AnomalyViewport.cpp | N | Portable asset identity, defining HitResult include and exact cached POV/near-clip semantics | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Compat/AnomalyCapabilities.cpp | N | Explicit 4.25 supported set; 5.1 catalogue admission unchanged; unknown IDs fail closed | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Compat/AnomalyEngineCompat.cpp | N | Exact 5.1 API operations; declared stock 4.25 API substitutions | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Private/Compat/AnomalyTickableWorldSubsystem.cpp | N | Single FTickable registration; 4.25 IsTickable lifetime gate; default-off audit only | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Private/Compat/AnomalyUnsupportedTexture.cpp | N | Complete 4.25 portable texture facade; never applied/eligible/ready/measured | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Public/AnomalyAutoInjectorSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h | N | Literal shim parent; five null raw reflected asset pointers and one empty raw-pointer array, with names/specifiers retained | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Public/AnomalySelectorSubsystem.h | N | Reparent to literal shim; move world eligibility to shared Super-first policy | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Public/Compat/AnomalyCapabilities.h | N | Single capability admission policy for registry, auto pool and selector | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Public/Compat/AnomalyEngineCompat.h | N | Portable identities and accessors; no reflected engine-specific types | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Public/Compat/AnomalyEngineVersion.h | N | A2 exact engine predicates and unsupported-version diagnostic | R1,R2,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Public/Compat/AnomalySharedPointerIncludes.h | N | A2 portable defining shared-pointer includes outside reflected UHT parse | R1,R2,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Public/Compat/AnomalyStatsIncludes.h | N | A2 defining stats includes outside reflected UHT parse | R1,R2,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyInjector/Public/Compat/AnomalyTextureAdapter.h | N | Phase E native adapter/factory declarations only; existing 5.1 facade unchanged | R1,R3,R4,R5,R7,R9 |
| Source/AnomalyInjector/Public/Compat/AnomalyTickableWorldSubsystem.h | N | Literal shared reflected ancestry and engine-equivalent 5.1 lifetime/tick contract | R1,R3,R4,R5,R6,R7,R8,R9 |
| Source/AnomalyShaders/AnomalyShaders.Build.cs | N | A2 exact-version rules, portable shared C# and dependency/availability policy; Injector adds once-per-target profile diagnostic in A3 (C diagnostic effect) | R1,R2,R8,R9 |
| Source/AnomalyShaders/Private/AnomalyShadersModule.cpp | N | A2 disabled 4.25 shader registration and diagnostic; 5.1 mapping retained | R1,R2,R3,R4,R5,R6,R7,R8,R9 |
| docs/PRE-DELIVERY-CHECKLIST.md | C | EC2 moves the legacy-descriptor explanation after the complete existing sentence | R0,R1,R2 |
| docs/architecture.md | C | EC2 repairs the spliced legacy-descriptor explanation without changing the rule | R0,R1,R2 |
| docs/europa/09404-working-result.md | C | Record exact failed commit, diagnostics, unauthorized UHT writes and proposed resume work | R0,R1,R2 |
| docs/europa/phase-a-a3-closure.md | C | Document shim contract, reflected surface, backend landing sites and closure boundaries | R0,R1,R2 |
| docs/europa/phase-a-reach.json | C | Join all release-to-feature changed paths to effect classes, reach and future gates | R0,R1,R2 |
| docs/europa/phase-a-routing.md | C | A1/A2 scoped host/cache routing record; runtime routing remains unqualified | R0,R1,R2 |
| docs/predictions/094-europa-phase-a.md | C | Frozen A/R predicates plus dated EC1/ED7 module and probe-root addenda before affected runs | R0,R1,R2 |
| docs/sessions/2026-10-16-094-03-europa-a1-a2.md | C | Retain accepted A1/A2 session record, already committed before this job | R0,R1,R2 |
| docs/sessions/2026-10-22-094-04-europa-a3.md | C | A3 journal, commit/build ordering and incomplete STOP status | R0,R1,R2 |
| tools/europa/DescriptorProbe.cs.txt | C | A2 portable C# descriptor-admission probe template; no runtime module | R0,R1,R2 |
| tools/europa/check_reflected_headers.py | C | EA11 UHT-safe reflected-header directive scan and synthetic forbidden-directive mutant | R0,R1,R2 |
| tools/europa/compile_children.py | C | Read-only PID descendant observations for compiler-child attribution, with self-test | R0,R1,R2 |
| tools/europa/descriptor_matrix.py | C | A2 JsonExport membership collection/comparison used for the restored A3 matrices | R0,R1,R2 |
| tools/europa/host_probe/EuropaPhaseAProbe.Build.cs.txt | C | Empty host probe and native TargetRules descriptor admission templates; actual host copies are separately manifested | R0,R1,R2 |
| tools/europa/host_probe/EuropaPhaseAProbeModule.cpp.txt | C | Empty host probe and native TargetRules descriptor admission templates; actual host copies are separately manifested | R0,R1,R2 |
| tools/europa/host_probe/TargetRules.fragment.txt | C | Empty host probe and native TargetRules descriptor admission templates; actual host copies are separately manifested | R0,R1,R2 |
| tools/europa/idle_gate.py | C | Accepted A1/A2 runtime idle gate retained; 45 prior synthetic tests, no runtime use in A3 | R0,R1,R2 |
| tools/europa/ledger_budget_self_tests.py | C | Retain A1/A2 budget can-fail cases; ED5 adds separate delta reconstruction tests | R0,R1,R2 |
| tools/europa/ledger_delta_self_tests.py | C | ED5 synthetic delta/hash tests for exact budgets, one-byte overruns, new files/folders and engine writes | R0,R1,R2 |
| tools/europa/process_wrapper.py | C | Add only the two ED2 Editor Development working-build routes to scoped child launching | R0,R1,R2 |
| tools/europa/tooling_self_tests.py | C | A2 orchestration can-fails retained; no runtime module | R0,R1,R2 |
| tools/europa/ubt_admission.py | C | A2 approved UBT entry/admission checks retained for project-local operations | R0,R1,R2 |
| tools/europa/working_build.py | C | Commit-bound action admission, zero engine actions and compiler diagnostic receipts; two reporting defects remain noted | R0,R1,R2 |
| tools/europa/write_ledger.py | C | ED1 floors, ED4 build attribution, ED5 root folders and compact initial-inventory/delta evidence | R0,R1,R2 |
| tools/strict_include_build.py | C | Explicit roots, UE4Editor, version-safe strict settings, actual TU action coverage and receipted restoration; not executed until A4 | R0,R1,R2 |

Planned or audited files requiring no A3 edit follow. These are B; being unchanged does not remove their dependent runtime gates. Package-checker alias/extraction work remains for its later authorized job. Probe behavior/fixtures remain A5. Separate TickAudit files were superseded by EA7's shim-local audit.

| Retained file | 5.1 | Reason |
|---|---|---|
| Content/Materials/M_CorruptTex_Normal.uasset | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Content/Materials/M_CorruptTex_UV.uasset | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Content/Materials/M_CorruptedTexture_Pink.uasset | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Content/Materials/M_MissingTexture_Checker.uasset | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Content/Textures/T_CorruptTex_NoiseN.uasset | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Shaders/Private/AnomalyMaskReduce.usf | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Shaders/Private/AnomalyTexStats.usf | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Shaders/Private/AnomalyVisibleMask.usf | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyCapture/Private/AnomalyAsyncWriter.cpp | B | Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable |
| Source/AnomalyCapture/Private/AnomalyCaptureLetterbox.cpp | B | Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable |
| Source/AnomalyCapture/Private/AnomalyCaptureModule.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyCapture/Private/AnomalyChangeFamilyData.cpp | B | Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable |
| Source/AnomalyCapture/Private/AnomalyMaskMeasure.cpp | B | Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable |
| Source/AnomalyCapture/Private/AnomalyPreviewTee.cpp | B | Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable |
| Source/AnomalyCapture/Private/AnomalyStencilTag.cpp | B | Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable |
| Source/AnomalyCapture/Private/AnomalySveKeyRing.cpp | B | Existing ANOMALY_CAPTURE boundary already excludes all work; outer includes portable |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_Blinking.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_LightingMismatch.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_LodCorruption.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingObject.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyInjector/Public/AnomalyDefaults.h | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyShaders/Private/AnomalyMaskReduceShader.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyShaders/Private/AnomalyTexStatsShader.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyShaders/Private/AnomalyVisibleMaskShader.cpp | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyShaders/Public/AnomalyMaskReduceShader.h | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyShaders/Public/AnomalyTexStatsShader.h | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| Source/AnomalyShaders/Public/AnomalyVisibleMaskShader.h | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| tools/check_package_descriptor.py | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |
| tools/verify_capture.py | B | Existing algorithm, guard, asset or reader needs no A3 edit; runtime/packaged qualification remains pending |

Host changes are outside the plugin git diff and separately manifested: all three .uproject files (C: probe declaration; the 5.1 copies also restore EC1 plugins), each host's Game and Editor Target.cs (C: native probe admission), and each host's `Source/EuropaPhaseAProbe/EuropaPhaseAProbe.Build.cs` plus `Private/EuropaPhaseAProbeModule.cpp` (C: empty host module). All other host Source files remain unchanged. These affect R0/R1/R2 now and R3–R9 when A5 adds behavior. The two 5.1 hosts use equal probe sources and target logic.

EC2 moved the two legacy-key explanation lines after their original sentences in PRE-DELIVERY-CHECKLIST.md and architecture.md. ED6 retains existing C# indentation and profile/toolchain content; its intentional output-frequency change is class C. Ordinary C2 reach also required the existing Viewport complex-collision access to enter Compat (PhysX TriMeshes on 4.25, ChaosTriMeshes on 5.1). No renderer dependency or reflected declaration outside the recorded surface was added.

## Shim contract and nine reflected raw pointers

Code map: `Source/AnomalyInjector/Public/Compat/AnomalyTickableWorldSubsystem.h` and `Private/Compat/AnomalyTickableWorldSubsystem.cpp`. These replace the engine parent of Injector, AutoInjector, Selector, Capture and ControlServer. Each derived DoesSupportWorldType override is removed; concrete GetSubsystem access remains unchanged.

| Contract | Implementation / evidence |
|---|---|
| Literal reflected base on both engines | Header :10, UWorldSubsystem + one FTickableGameObject inheritance |
| One registration; no Always tick on 4.25 | Single native tickable base; cpp :30 uses Never for templates, otherwise base default |
| Super first; Game/PIE only | ShouldCreateSubsystem cpp :20–28; no derived world-type override |
| Initialized state | Header bInitialized=false; cpp :49 check false then true; :58 check true then false |
| 5.1 initialization chain | Matches engine implementation: no added UWorldSubsystem Initialize/Deinitialize call |
| 4.25 tick admission | IsTickable cpp :35 returns !IsTemplate() && bInitialized |
| 5.1 tick admission | IsTickable delegates to FTickableGameObject; IsAllowedToTick is final and exactly !IsTemplate() && bInitialized |
| CDO and world | GetTickableTickType Never for templates; GetTickableGameObjectWorld returns GetWorld |
| Tick assertion and stat fallback | Tick cpp :67 retains engine initialization check; abstract GetStatId has engine-style PURE_VIRTUAL fallback |
| Audit | Non-Shipping, -IAIBench gated, IAI.Bench.TickAudit default 0; off path one branch, no audit allocation/lock; on records instance/class/world/frame/world-time/init/template |
| Qualification limit | 4.25 UHT passed. C++ compile failed elsewhere; no runtime registration, cadence, pause, multi-world or GC claim. A5 supplies independent per-world tick denominator. |

| Reflected owner | Property | Current type / initialization | Specifiers |
|---|---|---|---|
| Injector | MissingTextureChecker | UMaterialInterface* = nullptr | UPROPERTY() |
| Injector | CorruptedTexturePink | UMaterialInterface* = nullptr | UPROPERTY() |
| Injector | TexCorruptUv | UMaterialInterface* = nullptr | UPROPERTY() |
| Injector | TexCorruptNormal | UMaterialInterface* = nullptr | UPROPERTY() |
| Injector | TexCorruptNoise | UTexture2D* = nullptr | UPROPERTY() |
| Injector | TexCorruptStrongRefs | TArray<UObject*>; default empty | UPROPERTY(Transient) |
| BenchTwoPartActor | Root | USceneComponent* = nullptr | VisibleAnywhere, BlueprintReadOnly, Category=AnomalyBench |
| BenchTwoPartActor | PartEligible | UStaticMeshComponent* = nullptr | same |
| BenchTwoPartActor | PartIneligible | UStaticMeshComponent* = nullptr | same |

Names/specifiers are retained; no weak-pointer substitution. The array has no scalar null state. Eight scalar fields start null. Runtime GC proof remains pending.

## Disabled implementation, capability policy and C/E/F seams

The 18 planned Capture implementation units are covered as follows. New outer `ANOMALY_CAPTURE` guards precede renderer includes in **AnomalyCensus, AnomalyChangeMeasurement, AnomalyChangeStage, AnomalyFrameCapturer, AnomalyLabelWriter, AnomalyMaskSceneViewExtension, AnomalyMeasurability, AnomalyPreviewCapture, AnomalyRunLog, AnomalySceneViewExtension and AnomalySveCapturer**. Existing sufficient guards are retained in **AnomalyAsyncWriter, AnomalyCaptureLetterbox, AnomalyChangeFamilyData, AnomalyMaskMeasure, AnomalyPreviewTee, AnomalyStencilTag and AnomalySveKeyRing**. All names are .cpp under Source/AnomalyCapture/Private. The subsystem implementation is additional and remains incomplete: its unguarded target-mask/observability helpers caused this compile's errors.

FrameCapturer.h and LabelWriter.h exclude disabled renderer declarations. PreviewTee has a complete disabled class. Async state is complete at the Capture default constructor, FVTableHelper constructor and destructor; RunLog has complete inert methods and FArchive destruction visibility. Control's A2 opaque transport remains complete and its disabled Start/Stop/status paths stay inactive; ControlProtocol.cpp and ControlSnapshot.cpp are excluded with ANOMALY_CONTROL_SERVER. No listener or preview was tested.

Thirteen Injector implementation units are excluded by ANOMALY_ENGINE_51: Anomaly_StuckLowMip, Anomaly_TexCorrupt, TexCorruptBench, TexCorruptDraw, TexCorruptState, TexCorruptTree, TexCorruptUniform, Anomaly_TimeDilation, Anomaly_LodPopping, Anomaly_CameraClipping, Anomaly_MissingTexture, Anomaly_CorruptedTexture and Anomaly_ChangeCase. Native headers TexCorruptCore, Anomaly_TexCorrupt, Anomaly_StuckLowMip and Anomaly_CameraClipping have corresponding boundaries.

The 4.25-only `Private/Compat/AnomalyUnsupportedTexture.cpp` implements the portable TexCorrupt/StuckMip facade with unavailable eligibility, empty modes, failed warm start (-1), zero measurement work and empty statistics. It never reports a successful application, readiness or measurement; 5.1 contributes no duplicate implementation from this file. Stock 4.25 Injector startup installs a component Nanite probe returning false and removes its own probe on shutdown. AllowNanite is not changed; 5.1 retains Capture's original probe ownership.

| Engine | Admission policy | Preserved / pending |
|---|---|---|
| 4.25 | blinking, lighting_mismatch, lod_corruption; missing_object only with -IAIBench | All other and unknown IDs fail closed. No 5.1 plugin asset finder runs; five asset fields stay null. |
| 5.1 | Existing 12 delivered registry entries plus two gated bench twins | Nine auto-pool keys, arrays, draw order and defaults retained in source; runtime sequence/catalogue gate still required. |

Registry registration, ApplyAnomaly, auto-pool enable/pre-draw filtering and selector choices use `AnomalyCapabilities`. Unsupported IDs are filtered before a successful draw. The existing pool is filtered, not enlarged: manual light/LOD requests remain supported on 4.25 even when their IDs are not in the original nine-key auto array. No unsupported route is made eligible by a stub.

EC4 reflected-surface inventory (`audits-final-all-files/reflected-surface.json`) covers all seven reflected headers. Capture has **no UPROPERTY or UFUNCTION**; its Async, PreviewTee and RunLog members are native opaque owners. Injector's reflected users of the texture facades are precisely the six properties above and `OnWatchedTargetEndPlay(AActor*, EEndPlayReason::Type)`. The bench actor contributes the other three properties. AutoInjector, Selector, ControlServer and the new shim have no additional reflected members/methods. These reflected types are shared engine types; no RHI, SkinnedAsset, WebSocket or version-specific ticker type remains in this surface. The native frame/label-snapshot parameters are not reflected. No further reflected holder move was necessary after A2's opaque transport and the raw-pointer conversion.

The declaration-only seams, with backend landing notes in `docs/europa/phase-a-a3-closure.md`, are:

- **C:** `AnomalyCapture/Public/Compat/AnomalyCaptureBackend.h`, IAnomalyCaptureBackend RequestFrame/PollFrame/Stop and CreateCaptureBackend425; implementation lands in `AnomalyCapture/Private/Compat/AnomalyCaptureBackend425.cpp`, owned behind Async.
- **E:** `AnomalyInjector/Public/Compat/AnomalyTextureAdapter.h`, IAnomalyTextureAdapter Supports/Create and CreateTextureAdapter425; implementation lands in `AnomalyInjector/Private/Compat/AnomalyTextureAdapter425.cpp`, behind the native texture facades.
- **F:** existing native FAnomalyControlTransportState; 4.25 backend lands in `AnomalyControlServer/Private/Compat/AnomalyControlTransport425.cpp` behind that owner.

No 5.1 backend is rewired. Native interfaces may evolve without another reflected declaration change; implemented backend parity remains future work.

## Ancestry, EA11 and strict-tool status

Final audit: **845 source/tool/host text files**, 283 pattern matches, 57 explicit tickable-name matches. Those explicit matches are the five unchanged Baseline51 parent declarations plus candidate shim definitions/includes/parents in the two candidate worktrees. No Cast/IsA/IsChildOf/GetDerivedClasses/GetSubsystemArray consumer or serialized class-path dependency on the old tickable base was found. All tracked Source/tools files, host Source/Config/.uproject files, and host plugin source/tools were included; no filename-extension exclusion remained in the final pass.

Byte scans of **6,368 .uasset/.umap files** in plugin Content and all three hosts' Content found zero TickableWorldSubsystem strings in ASCII/UTF-8 or UTF-16 LE/BE. No asset loaded. Asset-registry confirmation still waits for the first authorized editor launch. Receipts: `audits-final-all-files/ancestry-source.json` and `ancestry-assets.json`.

EA11 final source check passes all **seven reflected headers**, including the shim, with zero violations. Its in-memory `#if ANOMALY_CONTROL_SERVER` mutant fails at synthetic/ProbeMutant.h:5; allowlisted and malformed/continued-directive controls also pass their expectations. No production header was mutated. Receipt: `closure/ea11-final.json`.

The strict tool now requires explicit engine, plugin and project roots; permits only the three owned physical-D hosts; selects UE4Editor paths on 4.25; inserts no 4.25 IncludeOrderVersion; validates actual compile actions/TU coverage; restores receipted changes and requires a normal rebuild. It was **not run**, as required. Its build route also depends on the unresolved UHT write routing and is not operationally certified. The ledger's mixed-case environment receipt bug and diagnostic-module parser issue below must be corrected before future build/strict evidence is trusted. The source lever audit passed (54 names/registrations, 48 static and 6 runtime, zero findings); it does not substitute for runtime can-fails.

## Working compile, diagnostics and ED4 findings

There was exactly one working compile attempt. The wrapper's action-plan invocation successfully ran UHT before exporting the actions. The subsequent compile used the same committed source and no engine-changing action. Both invocations passed -NoEngineChanges and -NoBuildUHT. UHT is the allowed header generator, not an editor/commandlet launch.

| Field | Recorded value |
|---|---|
| Commit / target | 9fdc8f0c6ea2027f9f7810b8f9fc244351c4b0a3 / StylizedParisStreetEditor Win64 Development |
| Operation / receipt | 04-build425-01 / builds/425-01/result.json |
| Selection | VS2017 14.16.27051 (directory 14.16.27023), SDK 10.0.17763.0 |
| C++ standard | Plugin .response files /std:c++17; shared PCHs retain default standard, causing C2855 |
| Action admission | 35 planned: project 11, plugin 24, engine 0; includes own-host PCH output under project Intermediate |
| Execution | 24 action entries started: project 10, plugin 14, engine 0; remaining dependent links/metadata skipped after failures |
| Timing | Action-plan 44.619 s; compile 81.587 s; wrapper measured 126.448 s (outer process 127.065 s) |
| Outcome | UHT exit 0, reflection generated in 7.488 s; C++/UBT exit 6; no retry/fix attempt |
| 5.1 working compile | Not run after write STOP, although ED3 and EC1 passed |
| Engine writes | Full before/after inventories: 0 paths changed in either engine; 0 B 5.1 metadata; 0 B 4.25 BuildRules growth |
| C: writes | 12 non-exempt UHT config path deltas; zero net bytes. Allowed-exception growth charged 0 B. Location STOP. |

Exact command recipe, issued through `tools/europa/process_wrapper.py --host 425 --mode build --target StylizedParisStreetEditor --configuration Development --output D:/IA_Europa/Receipts/09404/builds/425-01 --execute`:

```text
E:\EpicGames\UE_4.25\Engine\Binaries\DotNET\UnrealBuildTool.exe
StylizedParisStreetEditor Win64 Development
-Project=D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject
-Log=D:\IA_Europa\Receipts\09404\builds\425-01\UBT.log
-NoHotReload -NoEngineChanges -Verbose -NoBuildUHT -NoHotReloadFromIDE
-NoXGE -MaxParallelActions=1
```

The action-plan invocation additionally passed `-WriteOutdatedActions=D:/IA_Europa/Receipts/09404/builds/425-01/outdated-actions.json`. Every mutation path in that list is under the owned host. Compile-child Toolhelp observations record descendant PIDs (UBT, UHT, cl-filter, cl, link and supporting processes); they use no ETW/USN/elevation and may miss children shorter than the sampling interval. Command/action logs provide the complementary argv evidence.

Representative failures and the next proposed fixes, **not applied**, are:

| Failure | Location | Proposed next source work |
|---|---|---|
| C2855 /std:c++ inconsistent with PCH (9) | Nine plugin unity/generated TUs | 4.25-only NoSharedPCHs or another proven matched-standard policy; preserve 5.1; rerun matrices if C# changes |
| Missing Async Writer/mask types; cascade to C1003 | AnomalyCaptureSubsystem.cpp:1448 onward | Complete disabled helper guards; audit preview disabled exports and all common callers |
| TFunction has no Reset | AutoInjector:685–687; InjectorSubsystem:366,1156 | Portable empty-function reset adapter, exact 5.1 path retained |
| C4099 class/struct mismatch | Compat/AnomalyEngineCompat.h:10 | Match FStaticMeshRenderData class declaration on both engines |
| GetValue / GetPredictedLODLevel missing | AnomalyLod.cpp:117,135 | 4.25 per-platform screen-size and skeletal LOD access adapters |
| Four-argument GetViewFrustumBounds missing | AnomalyViewport.cpp:227,1580,1657,1906 | Equivalent no-far-plane construction on 4.25; retain 5.1 native overload |
| Axis constants, scene-only flag, UE_KINDA_SMALL_NUMBER missing | AnomalyViewport.cpp:608–610,1166,1238 | Compatible constants/optional-feature access inside existing Compat reach |

The raw diagnostics are retained, including cascades; no errors were suppressed. No error received a fix attempt before the STOP. There was no out-of-memory failure. These proposed fixes appear within C1–C5's compatibility design; any discovered renderer dependency, new reflected surface or ruled-behavior change still returns to Chat.

Warnings are unresolved. Each of the five module sites emitted four C5043 warnings (operator new, new[], delete, delete[] exception specification):

| Module | File:line | Warnings | New port warning |
|---|---|---|---|
| AnomalyBench | AnomalyBenchModule.cpp:507 | 4 × C5043 | yes, surfaced with C++17/PCH mismatch |
| AnomalyCapture | AnomalyCaptureModule.cpp:30 | 4 × C5043 | yes |
| AnomalyControlServer | AnomalyControlServerModule.cpp:24 | 4 × C5043 | yes |
| AnomalyInjector | AnomalyInjectorModule.cpp:40 | 4 × C5043 | yes |
| AnomalyShaders | AnomalyShadersModule.cpp:34 | 4 × C5043 | yes |
| UBT/user XML | BuildConfiguration.xml:16,22 | bAllowUBAExecutor and MemoryPerActionBytes unknown to 4.25 | no; already present in JsonExports, settings untouched |

`stop/normalized-diagnostics.json` supplies every diagnostic's module and file:line. The original wrapper parser matched the plugin-root name AnomalyInjector before the real module component, so its original module labels and unclassified warning flags are not authoritative; the raw logs are preserved and the derived report corrects this without changing build results. That parser needs repair in the resumed tooling work.

Source routing evidence is `precompile/425-write-routing.json` (24 numbered excerpts with source hashes). Relevant 4.25 paths: UBT SourceFileMetadataCache.cs:222, CppDependencyCache.cs:168, ActionHistory.cs:226 and TargetMakefile.cs:664 route installed-project caches to project Intermediate; ExternalExecution.cs:328,945,1063,1147 protects read-only engine generated headers/timestamps and puts manifest, UHT log, .deps and .uhtpath beside project output; BuildMode.cs:295 checks NoEngineChanges before action execution. XmlConfig.cs:65 uses the approved installed-engine C: cache. These cache predictions matched observed engine inventories.

The UHT UserDir prediction did not hold. LaunchEngineLoop.cpp:5275 appends UE-CmdLineArgs during AppInit; Paths.cpp:1549 caches its first UserDir parse. The preflight did not prove that every earlier path consumer had avoided that cache. Native UHT argv has no UserDir; UHT.txt:46 names the C: ini path; no requested D UHT user folder appeared. The exact cause is unresolved. The wrapper populated a process-local UE-CmdLineArgs environment value, but Windows normalized the environment key's casing and the exact-case receipt filter omitted it. This is a receipt defect, not proof that the child did or did not consume the value. `stop/routing-erratum.json` supersedes the original admission for UserDir. No engine/UHT experiment followed the STOP.

The ten rewritten Windows config filenames were Compat.ini, DeviceProfiles.ini, Engine.ini, Game.ini, GameUserSettings.ini, Hardware.ini, Input.ini, Lightmass.ini, RuntimeOptions.ini and Scalability.ini. The other two deltas were the removed old CrashReportClient.ini and its replacement under a new UE4CC directory. `stop/unauthorized-writes.json` records all twelve exact paths, sizes, mtimes and available hashes.

Partial outputs retained on D:, never staged, swapped, launched or archived:

| File | Bytes | SHA-256 |
|---|---|---|
| D:\IA_Europa\StylizedParisStreet\Binaries\Win64\UE4Editor-EuropaPhaseAProbe.dll | 89088 | cf76232d9289134d5709f00fde8b3ab3685ce4ce84570bd5c2b44f6c1e5f8d7c |
| D:\IA_Europa\StylizedParisStreet\Binaries\Win64\UE4Editor-StylizedParisStreet.dll | 91648 | c391bde91d6d3a9d2ab91e3dbe47137fbce60fb4d7769bd113b6f34d1a71e27d |

No new executable or completed Anomaly plugin DLL exists from this attempt. A4 may decide on cleaning only after A3 resumes and completes. No archive paths or pushed commits exist for this job.

## ED5 ledger, routing and per-operation writes

The ledger uses one initial full SQLite inventory per scope, then ordered gzip before/after deltas and a canonical SHA-256 for each snapshot. Earlier jobs' ledgers are untouched. Root scopes now include top-level folders and reparse targets, not just files. C: UnrealHeaderTool was added to the scan, which caught the real failure. Enumeration is fatal for engines/protected paths. Each operation records 198 denied OS/tooling entries (Windows/WER/ClaudeTemp); they are disclosed, not treated as empty. This is EB1's targeted inventory proof, not a claim of omniscient filesystem tracing.

The nine synthetic delta tests passed: C/E exact limit; one-byte overruns rejected; a new non-exempt scanned file rejected; a new root folder on each C/D/E rejected; an engine binary write rejected. Every delta reconstructed the expected snapshot hash. No C:/E: test folder was created. Four build whitelist controls reject Baseline51, Game, Shipping and mismatched target requests before execution. The process observer self-test passed. The accepted A1/A2 idle gate remains unchanged with its 45 prior synthetic tests; it was not used here because no runtime leg ran.

For build windows, compiler descendants count as port children. Windows/WER growth is conservatively charged; GPU/owner UE5.7/shared editor activity and orchestration folders are separately classified. The 5.1 metadata allowance requires a successful NoEngineChanges run with zero engine actions, recognized cache paths and a 100 MB cap; it was unused. No 4.25 engine exception beyond BuildRules was added.

TEMP/TMP, uebp log/saved/final folders, DOTNET_CLI_HOME and NuGet are process-local D: paths. UBT -Log and UHT -abslog reached D:. Project DDC graphs and ShaderWorkingDir remain prepared, with no renderer/DDC runtime leg this job. The UserDir environment route failed its measured obligation and is blocked. No registry, KnownFolder, machine or user environment was changed.

The following D: totals are logical bytes in the owned tree excluding the current ledger directory; they include earlier hosts/evidence. The reader copy used to reconstruct exception subtotals is explicitly included in the final ledger-evidence size. Free-space swings were not charged to the port.

| Operation | D bytes before → after | C / E allowed growth B | Foreign / tooling delta rows | Result |
|---|---|---|---|---|
| 01-hosts-toolchains | 15,379,024,326 → 15,379,067,955 | 0 / 0 | 23 / 17 | PASS |
| 02-descriptor-matrices | 15,379,067,955 → 15,452,143,468 | 0 / 0 | 36 / 44 | PASS |
| 03-cpp-closure | 15,452,143,468 → 15,452,835,177 | 0 / 0 | 49 / 34 | PASS |
| 04-build425-01 | 15,452,835,177 → 16,973,020,968 | 0 / 0 | 5 / 10 | STOP |
| 05-stop-evidence | 16,973,020,968 → 17,235,850,769 | 0 / 0 | 23 / 51 | PASS |

The 04 STOP remains in the job result; a passing read-only exit operation does not clear it. Every changed path, size, mtime and classification is in `ledger/<operation>/classified-changes.jsonl.gz`. Engine changes are empty for every operation. Exception-path before/after inventories were reconstructed from the full initial inventory and deltas in `stop/exception-path-totals.json`, with the snapshot hash for each side. Logical byte totals follow; unchanged totals can still contain forbidden rewrites.

| Scanned exception/location | 01 B→A | 02 B→A | 03 B→A | 04 B→A | 05 B→A |
|---|---|---|---|---|---|
| C: UBT logs | 8,646,023 → 8,646,023 | 8,646,023 → 8,646,023 | 8,646,023 → 8,646,023 | 8,646,023 → 8,646,023 | 8,646,023 → 8,646,023 |
| C: UHT Saved/Config (NOT EXEMPT) | 132 → 132 | 132 → 132 | 132 → 132 | 132 → 132 | 132 → 132 |
| C: UHT logs | 163,952 → 163,952 | 163,952 → 163,952 | 163,952 → 163,952 | 163,952 → 163,952 | 163,952 → 163,952 |
| C: UnrealEngine/4.25 | 72,086,004 → 72,086,004 | 72,086,004 → 72,086,004 | 72,086,004 → 72,086,004 | 72,086,004 → 72,086,004 | 72,086,004 → 72,086,004 |
| C: UnrealEngine/XmlConfigCache-* | 1,859 → 1,859 | 1,859 → 1,859 | 1,859 → 1,859 | 1,859 → 1,859 | 1,859 → 1,859 |
| C: Windows paging/crash root files | 553,672,704 → 553,672,704 | 687,890,432 → 687,890,432 | 687,890,432 → 687,890,432 | 687,890,432 → 687,890,432 | 687,890,432 → 687,890,432 |
| C: crashclient | 755,761 → 755,761 | 755,761 → 755,761 | 755,761 → 755,761 | 755,761 → 755,761 | 755,761 → 755,761 |
| C: gpu_dx | 2,469,888 → 2,469,888 | 2,469,888 → 2,469,888 | 2,469,888 → 2,469,888 | 2,469,888 → 2,469,888 | 2,469,888 → 2,469,888 |
| C: gpu_nvidia | 4,675,742,321 → 4,675,742,321 | 4,675,742,321 → 4,675,742,321 | 4,675,742,321 → 4,675,742,321 | 4,675,742,321 → 4,675,742,321 | 4,675,742,321 → 4,675,742,321 |
| C: wer_system | 56,700,215 → 56,700,215 | 56,700,215 → 56,700,215 | 56,700,215 → 56,700,215 | 56,700,215 → 56,700,215 | 56,700,215 → 56,700,215 |
| C: windows | 53,937,827,211 → 53,937,827,211 | 53,937,427,395 → 53,937,439,683 | 53,937,439,683 → 53,938,866,082 | 53,938,866,082 → 53,938,866,082 | 53,938,878,370 → 53,938,880,442 |
| E: 4.25 BuildRules | 4,259,817 → 4,259,817 | 4,259,817 → 4,259,817 | 4,259,817 → 4,259,817 | 4,259,817 → 4,259,817 | 4,259,817 → 4,259,817 |

Missing GPU/WER/temp roots or empty UE-named TEMP selections contribute zero; access denials remain listed. UHT Saved/Config is shown explicitly as non-exempt. Its 132-byte total hid twelve path/mtime changes, which the delta gate correctly rejected.

Tooling overhead, outside the port budget; .codex resolves to **E:/CodexData/home**, while .claude and C:/ClaudeTemp are on C:.

| Operation | .claude bytes before → after | .codex bytes before → after | ClaudeTemp bytes before → after |
|---|---|---|---|
| 01-hosts-toolchains | 10,097,169,311 → 10,099,025,162 | 26,696,207,516 → 26,698,564,741 | 5,149,485,731 → 5,149,485,731 |
| 02-descriptor-matrices | 10,100,434,750 → 10,107,322,767 | 26,694,606,166 → 26,700,911,495 | 5,149,485,731 → 5,149,485,731 |
| 03-cpp-closure | 10,109,705,909 → 10,121,277,176 | 26,701,270,223 → 26,704,121,807 | 5,149,485,731 → 5,149,485,731 |
| 04-build425-01 | 10,121,283,025 → 10,121,283,025 | 26,704,458,202 → 26,706,531,379 | 5,149,485,731 → 5,149,485,731 |
| 05-stop-evidence | 10,121,301,957 → 10,122,208,329 | 26,707,606,068 → 26,705,058,938 | 5,149,485,731 → 5,149,485,731 |

Final ledger plus its read-only initial-inventory reader copy: **416,371,787 bytes (0.388 GiB)**, below ED5's 2 GiB limit. Allowed exception growth charged across the job: C **0 B**, E **0 B**. Non-exempt UHT path writes are a separate STOP, not a budget exemption.

## Commits, prediction ordering and deviations

All commits are local Conventional Commits on feat/europa-port. GIT_AUTHOR_DATE and GIT_COMMITTER_DATE were absent; author and committer timestamps agree. No push, tag, protected branch or unrelated index change occurred.

| Commit | IST author/committer time | Subject |
|---|---|---|
| eb7b8e091b7b21af5377b74f661698ff71d66878 | 2026-10-22T12:54:53+05:30 | chore(europa): prepare A3 hosts and compact write ledger |
| 2e062ea592423360eb7380c4728a6a5349e36717 | 2026-10-22T13:24:29+05:30 | fix(europa): root host probe through descriptor filters |
| 71e8d4734c6b6bc0705a4ee2acbe9e8141d2926d | 2026-10-22T13:26:24+05:30 | fix(europa): format target profile keys as strings |
| 9fdc8f0c6ea2027f9f7810b8f9fc244351c4b0a3 | 2026-10-22T13:55:36+05:30 | feat(europa): add shared subsystem and disabled backend closure |
| 61e7cb469a4beda47685b495ae99d45e7553554f | 2026-10-22T14:22:34+05:30 | docs(europa): record A3 compile failure and UHT write stop |
| 668c075233b755001766ac48003183fb6af4dc68 | 2026-10-22T14:32:04+05:30 | docs(europa): clarify incomplete closure reach and speed gates |

The A3 prediction addendum was committed at 12:54:53, before the first new JsonExport. The probe-root correction addendum was committed at 13:24:29 before the corrected matrix run. The C++ source was committed at 13:55:36 before UBT at 14:09:20. No pixel threshold, AA arbiter, paired TAA/TSR requirement or per-world denominator was retuned. Post-STOP commits record status and reach only; production source remains identical to the failed compile SHA.

Bounded harness issues retained in evidence: (1) initial probe declaration absent from export, corrected once through native target admission; (2) toolchain reader looked only in UBT.log instead of the receipted Console.WriteLine output, corrected once; (3) C# FileReference addition rejected the profile key, corrected once with String.Format; (4) an initial inline whitelist-test command had shell quoting syntax failure, replaced once with a script; (5) a source-excerpt range exceeded Target.cs length, corrected once. No harness problem exceeded D2. The UserDir write failure and environment-receipt/parser defects received no post-STOP engine/build retry. Remaining raw compile errors received zero fix attempts.

Only status/evidence documentation and read-only exit checks followed the STOP. C: UHT configuration was not restored, no process was killed, no installed/staged binary was moved, and no toolset/security/global setting was changed. UHT's own out-of-scope config mutations are explicitly the exception to the intended write confinement, not an accepted deviation.

## Exact resume point and A4 handoff

**Chat decision required before another UHT/build operation.** The smallest proposed exception is the exact UHT `Saved/Config/Windows` and `Saved/Config/CrashReportClient` output classes, retaining the existing C: budget and path/size/mtime/hash evidence. This report does not authorize that exception. Alternatively, establish a process-local native-argument routing mechanism that acts before cached path selection; no supported forwarding option was identified in the reviewed 4.25 UBT path. Do not modify the engine or a global/per-user setting to obtain it.

After a ruling, resume from feat/europa-port's final clean HEAD, with production source at 9fdc8f0. Recheck P9, ED1 and the pinned toolsets. Repair the receipt casing and diagnostic module parsing before trusting later runs. Resolve the first compile's C++17/PCH mismatch and incomplete disabled capture/API closure within C1–C5; commit first and repeat the 4.25 Editor Development working build. If C# or a host descriptor changes, rerun the complete descriptor set and all three mutants. Stop if an identical compiler error survives two fixes. Only after 4.25 succeeds and the ruling permits writes should ReGate51 move to that committed source and run its permitted Editor Development build.

A4a/A4b must know: no successful plugin C++ closure exists yet; 5.1 has no working compile; the two retained host/probe DLLs are partial outputs; strict adaptation is unexecuted and shares the blocked routing; ED7 probe behavior and independent tick denominator are still A5; AA-off pixel truth, TAA/TSR characterization, worlds/GC, control, masks/labels, performance, staged descriptor proof and owner preview/eye legs are all outstanding. No runtime or final-build gate is green from this job. No owner action is requested during this relay run.

VERDICT: NEEDS-DECISION | A3 incomplete; 4.25 UHT passed but C++ failed; non-exempt UHT C: config writes triggered STOP; P9 and both engines unchanged; no 5.1 compile or push
