# 094-03 — Europa A1/A2 implementation

**GREEN for A1/A2 only.** Three physical-D hosts are ready; the exact-version C# rules, EA11 static check and both engines' descriptor Level 1 proofs pass. The 5.1 candidate matches release across eight valid pairs; 4.25 matches five expected pairs. Both real Shipping blacklist mutants fail the normal gate and both restored exports pass.

P9 entry/exit: zero differences. Both engines and the Paris original: zero inventory differences. EB1 budget charge: C 19,944 B / 250,000,000 B; E 0 B / 50,000,000 B. No floor breach or unresolved STOP. The accepted full baseline was not re-hashed.

Branch `feat/europa-port`, final `4c6c1d2f51f635aa5a2bc28057bbf2c4148f5b3b`; tested production source/descriptor `2ee5cac9de6f0cf34d15c65fd900f7c934901c34`. Four local commits; no push. Predictions precede the first UBT invocation. No editor/game/UHT/C++ build/cook/bench run. A3 has not started.

Receipts: `D:\IA_Europa\Receipts\09403`. Full operation/tooling tables and command index: `09403-report-data/` beside this report. Owner action: numeric read card may proceed; later A6/A7 owner legs remain pending.

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

All 18 operation ledgers pass. Total conservative charges are **C 19,944 B** and **E 0 B**. C charges are positive growth on the allowed CrashReportClient monitor paths; there were no port runtime children, so these are conservative budget reservations, not proven port causation. They are retained rather than reclassified away. E: BuildRules and both engines have zero changes.

Free space is GiB before→after; “D files” is the logical total under D:\IA_Europa excluding ledger files, not a volume-delta attribution. C/E charge is bytes. F is the number of changed paths classified foreign.

| Operation | C free | D free | E free | D files GiB | C/E charge B | F |
| --- | --- | --- | --- | --- | --- | --- |
| 01-entry | 26.09→21.50 | 158.75→158.51 | 53.54→51.12 | 0.489→0.548 | 0/0 | 697 |
| 02-copy425 | 23.21→23.20 | 158.34→155.40 | 50.77→50.77 | 0.548→3.306 | 0/0 | 42 |
| 03-copy51base | 23.20→23.20 | 155.22→153.02 | 50.77→50.73 | 3.306→5.327 | 0/0 | 9 |
| 04-copy51port | 31.36→41.54 | 152.84→150.64 | 50.68→50.63 | 5.327→7.348 | 0/0 | 32 |
| 05-worktrees | 41.54→41.53 | 150.46→150.25 | 50.63→50.63 | 7.348→7.382 | 0/0 | 9 |
| 06-routing | 41.51→41.51 | 150.07→149.89 | 50.61→50.59 | 7.382→7.382 | 0/0 | 10 |
| 07-predictions | 41.51→41.51 | 149.71→149.53 | 50.58→50.57 | 7.382→7.382 | 0/0 | 18 |
| 08-a2-source | 41.51→41.51 | 149.36→149.18 | 97.10→105.74 | 7.382→7.383 | 8,167/0 | 71 |
| 09-json51base | 41.48→41.48 | 149.00→148.81 | 105.37→105.18 | 7.383→7.401 | 0/0 | 9 |
| 10-reader-fix | 41.47→41.44 | 148.63→148.45 | 104.79→104.42 | 7.401→7.401 | 0/0 | 28 |
| 11-json51port | 41.45→41.45 | 148.27→148.08 | 104.40→104.30 | 7.401→7.420 | 0/0 | 6 |
| 12-json425 | 41.44→41.43 | 147.90→146.95 | 103.87→103.47 | 7.420→7.424 | 0/0 | 12 |
| 13-mutant51 | 41.44→41.44 | 146.49→146.31 | 103.31→103.05 | 7.424→7.425 | 8,197/0 | 68 |
| 14-mutant425 | 41.43→41.42 | 146.13→137.18 | 102.75→102.57 | 7.425→7.425 | 0/0 | 10 |
| 15-restored51 | 41.42→41.42 | 143.25→124.83 | 102.49→102.32 | 7.425→7.427 | 0/0 | 53 |
| 16-restored425 | 41.42→41.42 | 145.42→145.24 | 102.29→102.28 | 7.427→7.427 | 3,580/0 | 59 |
| 17-final-results | 41.41→41.41 | 145.07→144.89 | 102.10→102.07 | 7.427→7.427 | 0/0 | 4 |
| 18-exit | 41.41→41.36 | 144.71→144.47 | 102.04→101.98 | 7.427→7.487 | 0/0 | 22 |

Ledger evidence itself occupies 6,848,872,325 B (6.379 GiB) on D:, separately from that D-files total. Common Git objects and this run folder are also outside the D-files subtotal. Large drive-free-space movements are not attributed to the port or to a specific foreign process. E: entered below its 55 GiB warning margin but above EB2 admission and its 50 GiB floor; C: stayed above its 11 GiB warning margin.

Tooling overhead, logical bytes before→after (outside the port exception budget):

| Operation | .claude (C:) | .codex (physical E:) | ClaudeTemp (C:) |
| --- | --- | --- | --- |
| 01-entry | 8,451,659,668→8,451,715,473 | 26,617,023,897→26,619,179,822 | 5,149,483,368→5,149,483,368 |
| 02-copy425 | 8,451,746,650→8,451,746,650 | 26,628,045,815→26,628,250,867 | 5,149,483,368→5,149,483,368 |
| 03-copy51base | 8,451,746,650→8,451,764,176 | 26,629,091,160→26,631,222,573 | 5,149,483,368→5,149,483,368 |
| 04-copy51port | 8,451,764,176→8,452,396,264 | 26,636,614,028→26,645,343,709 | 5,149,483,368→5,149,483,368 |
| 05-worktrees | 8,452,531,229→8,452,587,304 | 26,660,670,354→26,667,701,399 | 5,149,483,368→5,149,483,368 |
| 06-routing | 8,452,600,944→8,452,600,944 | 26,668,698,972→26,678,655,930 | 5,149,483,368→5,149,483,368 |
| 07-predictions | 8,452,600,944→8,452,600,944 | 26,685,742,782→26,705,774,480 | 5,149,483,368→5,149,483,368 |
| 08-a2-source | 8,452,610,791→8,452,610,791 | 26,726,347,987→26,729,848,343 | 5,149,483,368→5,149,483,368 |
| 09-json51base | 8,452,626,470→8,452,626,470 | 26,730,648,141→26,731,098,643 | 5,149,483,368→5,149,483,368 |
| 10-reader-fix | 8,453,308,345→8,455,527,848 | 26,733,304,353→26,733,400,292 | 5,149,483,368→5,149,483,368 |
| 11-json51port | 8,455,880,434→8,456,195,541 | 26,733,835,690→26,733,892,890 | 5,149,483,368→5,149,483,368 |
| 12-json425 | 8,456,505,535→8,458,263,373 | 26,737,206,294→26,744,415,350 | 5,149,483,368→5,149,483,368 |
| 13-mutant51 | 8,458,500,546→8,460,811,168 | 26,746,523,860→26,746,757,740 | 5,149,483,368→5,149,483,858 |
| 14-mutant425 | 8,467,167,446→8,470,335,152 | 26,747,177,288→26,747,368,494 | 5,149,483,858→5,149,483,858 |
| 15-restored51 | 8,475,286,216→8,475,536,543 | 26,747,948,034→26,748,061,287 | 5,149,483,368→5,149,483,368 |
| 16-restored425 | 8,475,747,895→8,475,883,952 | 26,748,487,266→26,748,596,897 | 5,149,483,368→5,149,483,368 |
| 17-final-results | 8,478,066,861→8,480,491,638 | 26,748,888,720→26,754,204,871 | 5,149,483,368→5,149,483,368 |
| 18-exit | 8,481,203,929→8,483,296,759 | 26,756,768,814→26,756,944,020 | 5,149,483,368→5,149,483,368 |

The per-operation CSVs in `09403-report-data/` give free space, D: logical file totals, C:/E: charges, exception-scope totals, foreign paths and tooling overhead. The original SQLite inventories preserve every measured path. `scope-inventories.csv` includes the EA4 4.25 EngineUserDir, XmlConfigCache and installed BuildRules subsets; `outside-D-changes.csv` lists each observed changed path and its classification. Full command receipts remain in each ledger directory; `commands.json` indexes them.

These are targeted observations, not system-wide write tracing. OS/tooling scans report 191 access-denied entries per snapshot (51 Windows, 85 WER, 55 ClaudeTemp); they are retained as errors, never presented as empty directories. Protected engines/Paris and P9 enumerations have zero access errors and are error-fatal. Windows, Epic, shared UE 5.7/Zen/editor settings and GPU activity are foreign in this no-renderer job. `.codex` resolves to `E:\CodexData\home`; `.claude` and `C:\ClaudeTemp` are on C:. Their changes are tooling overhead, not port budget.

## Predictions, commits and self-tests

| Commit (feat/europa-port; local only) | Author time | Committer time | Subject |
| --- | --- | --- | --- |
| ac81ecbfb65380623c1fc16b07b3851b181429d9 | 2026-10-16T16:32:37+05:30 | 2026-10-16T16:32:37+05:30 | docs(europa): freeze Phase A predictions and scoped setup |
| fe745437deaabd76a5658689e54636a35b48a855 | 2026-10-16T16:39:30+05:30 | 2026-10-16T16:39:30+05:30 | feat(europa): add exact-version availability and legacy module filters |
| 2ee5cac9de6f0cf34d15c65fd900f7c934901c34 | 2026-10-16T16:53:51+05:30 | 2026-10-16T16:53:51+05:30 | fix(europa): distinguish target exports from UBT event logs |
| 4c6c1d2f51f635aa5a2bc28057bbf2c4148f5b3b | 2026-10-16T17:34:37+05:30 | 2026-10-16T17:34:37+05:30 | docs(europa): record A2 descriptor proofs and budget controls |

First UBT: **2026-10-16T16:47:19.230647+05:30**, after the predictions commit at 2026-10-16T16:32:37+05:30. All seven UBT invocations exited 0; the separate first baseline reader failed as described below. W4 receipts show GIT_AUTHOR_DATE and GIT_COMMITTER_DATE absent. Final Source, descriptor, predictions, reader, process wrapper and ledger are unchanged from tested `2ee5cac`; ReGate51 remains there intentionally.

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

| Host | Target | Configuration | IA modules | All modules | Result |
| --- | --- | --- | --- | --- | --- |
| 51base | StackOBot | Debug | 5 | 324 | PASS |
| 51base | StackOBot | DebugGame | 5 | 324 | PASS |
| 51base | StackOBot | Development | 5 | 324 | PASS |
| 51base | StackOBot | Shipping | 0 | 291 | PASS |
| 51base | StackOBot | Test | 5 | 318 | PASS |
| 51base | StackOBotEditor | Debug | 5 | 721 | PASS |
| 51base | StackOBotEditor | DebugGame | 5 | 721 | PASS |
| 51base | StackOBotEditor | Development | 5 | 721 | PASS |
| 51port | StackOBot | Debug | 5 | 324 | PASS |
| 51port | StackOBot | DebugGame | 5 | 324 | PASS |
| 51port | StackOBot | Development | 5 | 324 | PASS |
| 51port | StackOBot | Shipping | 0 | 291 | PASS |
| 51port | StackOBot | Test | 5 | 318 | PASS |
| 51port | StackOBotEditor | Debug | 5 | 721 | PASS |
| 51port | StackOBotEditor | DebugGame | 5 | 721 | PASS |
| 51port | StackOBotEditor | Development | 5 | 721 | PASS |
| 425 | StylizedParisStreet | DebugGame | 5 | 304 | PASS |
| 425 | StylizedParisStreet | Development | 5 | 304 | PASS |
| 425 | StylizedParisStreet | Shipping | 0 | 278 | PASS |
| 425 | StylizedParisStreetEditor | DebugGame | 5 | 584 | PASS |
| 425 | StylizedParisStreetEditor | Development | 5 | 584 | PASS |

Non-Shipping IA set: AnomalyInjector, AnomalyCapture, AnomalyControlServer, AnomalyShaders and AnomalyBench. Shipping: empty. The full 5.1 candidate module memberships, including engine dependencies, equal release in every row (zero differences). 4.25 exports the four capability defines as 0; 5.1 candidate exports them as 1.

Each host also exercised all 25 combinations through the actual UBT PluginDescriptor parser and IsCompiledInConfiguration: five target types × five configurations. Non-Shipping Client/Server exclude AnomalyBench; Program and Shipping exclude all five. These are supplemental native predicates, not builds of absent Client/Server/Program targets. Source 5.1 Editor Test/Shipping and installed 4.25 unsupported configurations are explicitly excluded from target exports.

Both removed-blacklist mutants changed the real Game Shipping export from zero IA modules to AnomalyShaders alone. The normal predicate therefore fails; native predicate rows also changed as predicted. Each descriptor was byte-restored and a fresh real Shipping export returned to zero IA modules. Their cross-worktree raw hashes differ only by Git line endings.

New artifacts are C# rule assemblies only; hashes (also in `A2-level1-results.json`):

| Rules DLL | Bytes | SHA-256 |
| --- | --- | --- |
| D:\IA_Europa\StylizedParisStreet\Intermediate\Build\BuildRules\StylizedParisStreetModuleRules.dll | 20480 | 48755fa8a646b38768f088b89b5b293f6660b3e7ed2c3487c58f1955ad8c634d |
| D:\IA_Europa\Baseline51\StackOBot\Intermediate\Build\BuildRules\StackOBotModuleRules.dll | 17920 | 3b69c3f32e5359c950b19928744799c5843de46f66934c50b779462e50218ac7 |
| D:\IA_Europa\ReGate51\StackOBot\Intermediate\Build\BuildRules\StackOBotModuleRules.dll | 19456 | 512de20146956581e926993a5c40fde9aa4649a869941bd13e20d140c9a1b2de |

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
