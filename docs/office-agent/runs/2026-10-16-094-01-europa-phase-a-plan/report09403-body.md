# 094-03 — Europa A1/A2 implementation

{{SUMMARY}}

## P9 baseline, entry and exit

The accepted full baseline remains `D:\IA_Europa\Receipts\P9\baseline\full-sha256.jsonl`: 707,775 files, 231,535,810-byte manifest, SHA-256 `db884f1e9c3debd7b9ed678db08036509959b1fc5a0d27799b10ecccb0625cce`. This job reused it; it did not repeat the full protected-tree hash. Each entry/exit proof checked the protected inventory and fully hashed 25,866 protected binary/config files, totaling 119,771,817,645 bytes. Both comparisons report zero differences, including the pause receipts, protected refs, worktree heads, index hashes and status. J2's installed candidate remains the baseline; no restore was performed. EA9's post-pause archive state remains accepted.

The only Git additions are `feat/europa-port` and the three authorized worktree registrations. The 71 pre-existing refs and seven protected worktrees are unchanged. No push, tag operation, stash, protected-branch edit, global XML edit, security change, process kill or network action occurred.

Evidence: `D:\IA_Europa\Receipts\09403\P9\entry\` and `P9\exit\`, including `exit-summary.json`, `inventory-comparison.json`, `binary-config-sha256.jsonl`, `pause-comparison.json`, and the exit's authorized-ref/worktree receipts. `09403-report-data/whole-job-engine-paris-continuity.json` compares the first and final ledger inventories, including the intervals between operations. Both engines and the Paris original have zero differences.

## Hosts, copies and CaptureBench

`D:\IA_Europa` resolves to physical D:. All three hosts are plain directories. The common repository was addressed directly for worktree creation, as EA14 requires.

| Host | Plugin worktree | Copy before local routing edits |
|---|---|---:|
| `D:\IA_Europa\StylizedParisStreet` | `Plugins\AnomalyInjector`, `feat/europa-port` | 1,479 files; 2,960,638,517 B |
| `D:\IA_Europa\Baseline51\StackOBot` | `Plugins\AnomalyInjector`, detached `d7eee97` | 2,756 files; 2,167,688,640 B |
| `D:\IA_Europa\ReGate51\StackOBot` | `Plugins\AnomalyInjector`, detached tested `2ee5cac` | 2,756 files; 2,167,688,640 B |

The Paris source is `E:\Unreal Projects\StylizedParisStreet`. The two 5.1 sources are the authorized original StackOBot `.uproject`, `Config`, `Content` and `Source`. Robocopy used `/XJ`; nested bare `/XD` names and reparse points were checked first. Binaries, Intermediate, Saved, Builds, DDC, Plugins and unrelated top-level files were excluded. Copy manifests compare path, size, mtime and full SHA-256. Source-before/source-after and job-exit comparisons are equal. Target.cs files remain byte-identical to their sources, retaining the 4.25 pins and 5.1 include order.

Manifests and exact copy commands are under `Receipts\09403\copies\425`, `51base`, `51port`, and ledger operations 02–04. Each 5.1 copied-content manifest lists 666 fixture-related paths: 466 external-actor paths, 197 CaptureBenchTexCorrupt, two CaptureBenchGate and one AnomalyFixtures path. No Review093/J2_ named path was found. Names alone do not establish provenance; A5 must generate the identical declared fixtures from the archived tools before qualification.

Both 5.1 hosts use CaptureBench from `git archive e64d6f9a43b7aa2e5eb041b7dab14e5285bb911b`, named by release receipt `docs/sessions/2026-09-30-089-02b-m53-rebench-prep.md:76` and confirmed by EB5. No installed J2 copy was used. Archive: `D:\IA_Europa\Receipts\09403\CaptureBench-e64d6f9.tar`, 870,400 B, SHA-256 `752f196ef20a7f95cdfef8febecafe34a2ae1e2d3c280ecf2facb76a3b50235b`, re-hashed at closeout.

## Routing and EB1 write proof

Each copied `DefaultEngine.ini` has its own named DDC graph plus project-local default/installed graphs: `Europa425DDC`, `Europa51BaseDDC`, `Europa51PortDDC`; private caches are `D:\IA_Europa\Caches\425`, `51base`, `51port`. There is no shared-cache or environment override in these graphs. Only AnomalyInjector is explicitly enabled on 4.25. The 5.1 copies enable AnomalyInjector, CaptureBench and PythonScriptPlugin; other explicit plugins are disabled. The required 5.1 Python/WebSocket engine plugin binaries already existed and were hashed before admission.

`tools/europa/process_wrapper.py` names the engine roots explicitly. It runs the existing UBT executable directly for JsonExport. Child receipts record D:-scoped TEMP/TMP, DOTNET_CLI_HOME, NuGet and `uebp_EngineSavedFolder`, `uebp_LogFolder`, `uebp_FinalLogFolder`, plus absolute `-Log=`/JSON output paths. Prepared editor/game/UAT recipes include `-UserDir`, `-ShaderWorkingDir`, `-DDC`, `-abslog`, crash-client logging, staging and package paths on D:. Those runtime recipes were not executed or qualified here. `-UserDir` and shader/DDC arguments do not apply to a JsonExport process.

`write_ledger.py` inventories the EB1 locations before and after each operation, stores path/size/mtime in SQLite, records changed paths and classifies foreign/tooling activity separately. No ETW, USN or elevation was attempted. Every direct child command/environment has a receipt. Free space is used only to enforce floors. UBT invocations omitted `-ExecCodeGenActions`; no UHT, C++ compiler, editor, game, cook, shader worker or packaging leg ran.

{{LEDGER}}

The per-operation CSVs in `09403-report-data/` give free space, D: logical file totals, C:/E: charges, exception-scope totals, foreign paths and tooling overhead. The original SQLite inventories preserve every measured path. `scope-inventories.csv` includes the EA4 4.25 EngineUserDir, XmlConfigCache and installed BuildRules subsets; `outside-D-changes.csv` lists each observed changed path and its classification. Full command receipts remain in each ledger directory; `commands.json` indexes them.

These are targeted observations, not system-wide write tracing. OS/tooling scans report 191 access-denied entries per snapshot (51 Windows, 85 WER, 55 ClaudeTemp); they are retained as errors, never presented as empty directories. Protected engines/Paris and P9 enumerations have zero access errors and are error-fatal. Windows, Epic, shared UE 5.7/Zen/editor settings and GPU activity are foreign in this no-renderer job. `.codex` resolves to `E:\CodexData\home`; `.claude` and `C:\ClaudeTemp` are on C:. Their changes are tooling overhead, not port budget.

## Predictions, commits and self-tests

{{COMMITS}}

The frozen prediction file is `docs/predictions/094-europa-phase-a.md`. It covers A0–A8/R0–R9, the AA-off zero-edge arbiter, paired release-default TAA/TSR characterization, per-world eligible-update denominator, EA3 StackOBot/CaptureBench fixtures and EA10 split. It is unchanged after the first commit. No runtime predicate has been declared passed here.

The port idle gate's 45 synthetic cases pass, including each refusal condition, three consecutive polls, loss of admission and timeout. It enforces 300 s human idle, 6 GiB free RAM, GPU at most 15%, no foreign UE/game/UAT/cook/shader process, and the disk floors. Its real collector was not used for A1/A2. Routing/classification tests: 31 pass. Five synthetic tests exercise the production ledger's exact C:/E: limits, one-byte overruns and cumulative C: growth; overruns cause STOP. These tests create SQLite metadata only on D:, with no synthetic C:/E: writes.

EA11 scans all six reflected headers, including the bench actor: zero forbidden directives. The five subsystem headers now use non-reflected optional-include helpers; the control header has no capability guard in its reflected region. A physical synthetic header with `#if ANOMALY_CONTROL_SERVER` makes the checker exit 1 with one violation. Final real scan passes. This is static evidence, not a UHT compile.

## A2 file changes and 5.1 effect

The old europa-e1 switch was reimplemented as ruled. The reviewed 22-file draft was applied, then the selected toolchain-directory log was added. Here **N** means changed but intended to preserve 5.1 behavior; **C** means deliberately changed behavior/output. No modified file is claimed byte-identical. Enabled C++ bodies retained inside new guards are not a runtime-neutrality proof.

| File, relative to plugin root | Change | 5.1 effect |
|---|---|---|
| `AnomalyInjector.uplugin` | Legacy WhitelistTargets/BlacklistTargetConfigurations keys | N; actual UBT matrix equals release |
| `Source/AnomalyInjector/AnomalyInjector.Build.cs` | Shared exact 4.25/5.1 validation, profile policy, C++17 only on 4.25, selected toolchain log | N for compilation policy; C diagnostic output |
| `Source/AnomalyCapture/AnomalyCapture.Build.cs` | Exact enabled/disabled dependencies/defines; C# preprocessing protects 5.1-only API | N |
| `Source/AnomalyControlServer/AnomalyControlServer.Build.cs` | Exact transport/dependency profile; public Injector dependency | N |
| `Source/AnomalyShaders/AnomalyShaders.Build.cs` | Exact shader capability; Core-only disabled shell | N |
| `Source/AnomalyBench/AnomalyBench.Build.cs` | Exact bench capability/target filter; disabled dependency set | N |
| `Source/AnomalyInjector/Public/Compat/AnomalyEngineVersion.h` | Exact version constants and unsupported-profile error | N on supported 5.1 |
| `Source/AnomalyInjector/Public/Compat/AnomalyCapabilities.h` | Non-reflected Phase A backend capabilities | N; additive seam |
| `Source/AnomalyInjector/Public/Compat/AnomalyStatsIncludes.h` | Optional stats includes outside reflection | N |
| `Source/AnomalyInjector/Public/Compat/AnomalySharedPointerIncludes.h` | Optional shared-pointer include outside reflection | N |
| `Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h` | Replace reflected optional include guards with helper | N |
| `Source/AnomalyInjector/Public/AnomalyAutoInjectorSubsystem.h` | Same | N |
| `Source/AnomalyInjector/Public/AnomalySelectorSubsystem.h` | Same | N |
| `Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h` | Stats/shared-pointer helpers | N |
| `Source/AnomalyControlServer/Public/AnomalyControlServerSubsystem.h` | Opaque transport ownership; constructors/destructor out of line; remove reflected-region guard | N intended; allocation/lifetime/ABI reach requires later gates |
| `Source/AnomalyControlServer/Private/Compat/AnomalyControlTransportState.h` | Native 5.1 state and complete empty 4.25 state | N intended |
| `Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp` | Route existing fields through opaque state; define ownership lifetime | N intended; runtime proof pending |
| `Source/AnomalyShaders/Private/AnomalyShadersModule.cpp` | Guard functional startup, add disabled startup log | N |
| `Source/AnomalyBench/Private/AnomalyBenchModule.cpp` | Guard bench body and add inert startup log | N |
| `Source/AnomalyBench/Private/AnomalyBenchTwoPartActor.cpp` | No component setup in disabled bench shell | N |
| `docs/PRE-DELIVERY-CHECKLIST.md` | EA12 legacy names and 5.1 silent fallback line | C documentation |
| `docs/architecture.md` | Same | C documentation |

Additional A1/evidence files are `CLAUDE.md`, `docs/europa/phase-a-routing.md`, the predictions and session journal, and the nine files under `tools/europa/`: DescriptorProbe template, reflected-header checker, descriptor reader, idle gate, process wrapper, routing self-tests, UBT admission check, write ledger and budget self-tests. These deliberately change tooling/documentation (C), with no production runtime claim. Copied host `.uproject`/DDC config/Build.cs observer edits are outside the plugin commits; baseline and candidate use the same observer and fixture inputs. The observer invokes the actual UBT parser/predicates only when a process-local D: receipt path is set; it changes no target pin or dependency.

## UBT rules, matrices and can-fails

{{MATRICES}}

Both UBTs compiled the shared C# rules. The actual 5.1 selected toolchain directory/family is `14.38.33130`; its compiler binary revision is `14.38.33144`. The actual 4.25 compiler revision is `14.16.27051`, installed under directory `14.16.27023`. Logs show SDKs `10.0.22621.0` and `10.0.17763.0`, and CppStandard `Default` and `Cpp17`, respectively. No selector, target or global XML was changed to manufacture these readings. Existing global XML contains newer unsupported options that 4.25 warns about; the file remains untouched.

The engine and per-user XML inputs also match their pre-UBT size, mtime and SHA-256 receipts (`09403-report-data/xml-settings-unchanged.json`).

All invocations use existing engine UBT executables, `-Mode=JsonExport`, `-NoEngineChanges`, `-NoHotReload`, `-Verbose`, a D: `-Log=`, and one `-Target=<target> Win64 <configuration> -Project=<D:project> -OutputFile=<D:json>` argument per pair. The exact Windows command lines/environment are in ledger child receipts and each matrix's `invocation.json`. No full Editor/Game build ran. Rule-assembly SHA-256 values are in `A2-level1-results.json`; there are no newly built game/plugin executables or C++ DLLs.

## Later backends and A3

Full parity remains the target. Phase C can add render/readback, labels and mask implementations behind the existing opaque capture async/preview/run-log state and native factories. Phase E can add texture/streaming/readiness implementations behind native Compat/adapters and the existing subsystem facade; A3 performs the one-time raw-UPROPERTY migration. Phase F can replace/add transport internals behind `FAnomalyControlTransportState` without another reflected-region capability guard. The new capability profile records current support, not a completed future backend.

A3 remains the approved shared C++ closure: introduce the literal shim and reparent five subsystems, preserve 4.25 initialized/CDO and exact 5.1 tick semantics, move world filtering into ShouldCreateSubsystem, migrate raw reflected pointers, audit ancestry consumers, complete Compat/includes and disabled capture/advanced-texture symbols, and adapt strict_include_build.py for explicit roots/UE4Editor/no 4.25 IncludeOrderVersion. The opaque control lifetime and UHT-safe includes are already in A2. No other A3 closure was attempted. A4a/A4b still own full/strict C++ builds; A8/A11a still owe descriptor Level 2 packages. Mounted 5.1-authored plugin assets remain an admission concern before the first 4.25 editor launch.

The wrappers currently permit execution only for JsonExport. A later authorized build/runtime job must enable its operation mode and bind the runtime idle gate. Before runtime, the ledger must also replace its explicit A1/A2 “no renderer/editor child” foreign-attribution assumptions with classifications suitable for the real launch, then verify actual redirect behavior. Prepared command-line tests are not a runtime routing qualification.

The owner may complete the numeric Europa read card now. No owner runtime action was needed here. The first owner 4.25 editor/preview leg remains A6 and the eye check A7, after chat authorizes those jobs.

## Harness issues, limits and decision items

Two harness issues used at most two attempts each. Operation 04 initially classified a shared `UnrealEngine/Editor/FilterBar.json` mtime change as STOP. Its recorded children were only source-copy tooling; no port editor was launched. Under EB1, the repaired classification is foreign. The original failure, input inventories and adjudication remain; the copy was not repeated. Operation 09's UBT succeeded, but reader attempt 1 parsed UBT's JSON-lines event log as a target JSON document. Attempt 2 restricted target filenames and verified the existing eight exports and native predicates; no UBT rerun or predicate change was needed. These were harness failures, not hidden product failures or protected-state restores.

No unresolved EA15 STOP or product/scope decision remains for A1/A2. GREEN here means only the authorized setup, rules/static checks and descriptor Level 1 are complete. It does not mean the 4.25 C++ port builds, loads, ticks, captures, transports control, packages or matches pictures. Those claims remain subject to the frozen A/R gates and later chat rulings. There was no push and no runtime lever/setting to restore; both descriptor mutants were restored and independently re-exported.

VERDICT: GREEN | A1/A2 complete; protected state unchanged; both UBT matrices and blacklist can-fails pass; A3 not started; no push
