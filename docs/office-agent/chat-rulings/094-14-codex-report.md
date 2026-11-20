<!-- Collected by Claude Code (mailbox brief 094-14) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 8C39B79C445B1B585551B7AA0A2FFF8FCB76BBF9DDE114DFEBACA725DA7164F1, 13927 bytes; thread 01a14334, resume 20:50:44-21:49:24 IST, 3520 s, exit 0; body below is byte-identical -->
# 094-14 — Europa A5 partial result

**NEEDS-DECISION. A5 is incomplete.** EN1 passes, and the first 4.25 probe Editor build now returns **0 errors / 0 C++ warnings**, restoring its DLL, PDB and library. The new action-log reader failed twice, exhausting D2. No third attempt or further build ran.

- Final feature head: `cd5b9ad`; compiled probe source: `35eec14`. Production plugin source/content remain unchanged.
- P9 v2 entry and exit PASS; no protected-state differences. C:/E:/engine budget charges are **0 B**.
- All 110 prior artifacts are present: 108 byte-equal; only the 4.25 probe DLL/PDB changed.
- Five Development rebuilds, complete action evidence, EI4, A5 manifest binding and the owner kit remain pending.
- No runtime launch, push, tags, global/per-user setting change, or foreign process termination. Elapsed approximately 51 minutes.

## Entry, exit and toolchains

Both EA8 proofs use v2 SHA-256 `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`, re-derived byte-for-byte from v1 and the removal manifest. Each inventories 472,553 files, admits the same 1,023 inherited EG1 records, and verifies **23,033 full binary/config hashes (104,794,989,456 B)**. Original-host manifests and protected git state match. The m51 checkout retains its exact 15 untracked entries and index hash. The five prunable registrations remain untouched.

Paris is clean on `feat/europa-port` at `cd5b9ad`; ReGate51 is clean and detached at `35eec14`; Baseline51 is clean and detached at `d7eee97`. Entry free space was C/D/E **18.97/82.92/236.62 GiB**; exit was **18.96/82.50/235.45 GiB**. No warning or floor was crossed.

Compiler identity checks pass: 4.25 directory `14.16.27023`, compiler product `14.16.27051.0`, SDK `10.0.17763.0`; 5.1 directory `14.38.33130`, product `14.38.33144.0`, SDK `10.0.22621.0`. Actual 4.25 responses confirm the first selection; no 5.1 compile ran.

Evidence: [entry](D:/IA_Europa/Receipts/09414/P9/entry/summary.json), [exit](D:/IA_Europa/Receipts/09414/P9/exit/summary.json), [git state](D:/IA_Europa/Receipts/09414/P9/exit/git-state.json), [compiler identity](D:/IA_Europa/Receipts/09414/toolchains/exit-identity.json).

## EN1 boundaries and EL2 canary

The base contained the real canary and `CLAUDECODE`; their positive controls matched the launch receipt. **26 strict boundary tests and 16 native-toolchain tests pass**, including exact-prefix acceptance and rejection of a missing base PATH entry, foreign prefix and canary. A fresh-child control preserves the non-blocked SDK variables.

All **51 recorded child environments** contain no blocked name. The current 4.25 makefile decodes **98 pairs**: no blocked name or nonempty secret-named variable. Its PATH is the unchanged base suffix preceded by SDK `10.0.17763.0/x64`, MSVC `HostX64/x86`, and `HostX64/x64`. Only `VC_COMPILER_DIR` and `VC_COMPILER_PATH` are added, within the selected toolchain. All other non-blocked base variables are preserved. The complete six-build makefile proof remains pending.

The final literal-canary scan covers new/changed files under `D:/IA_Europa` and this run folder. Its PASS receipt is linked below; no value is printed or stored. Names and masked values only are recorded. No additional historical secret redaction was needed.

Evidence: [tests](D:/IA_Europa/Receipts/09414/environment/en1-tests.json), [all child boundaries](D:/IA_Europa/Receipts/09414/closeout-final/all-child-boundaries.json), [native environment](D:/IA_Europa/Receipts/09414/closeout-final/native-environment-425.json), [makefile proof](D:/IA_Europa/Receipts/09414/closeout-final/makefiles.json), [final canary scan](D:/IA_Europa/Receipts/09414/closeout-final/canary-final.json).

## Builds, warnings and the STOP

Both runs used `StylizedParisStreetEditor Win64 Development`, the explicit 4.25 UBT executable and owned project, `-NoEngineChanges -NoBuildUHT -NoHotReload -NoHotReloadFromIDE -NoXGE -MaxParallelActions=4 -Verbose`, with scoped TEMP/TMP and an absolute per-run log. Full argument arrays and effective environments are retained in each invocation/child receipt.

| Run / SHA | Native exit | Duration, including memory admission | Actions | Result |
|---|---:|---:|---|---|
| `425-editor` / `290a7d2` | 6 | 54.71 s | 3 project compiles executed; 6 planned; engine/plugin 0 | Probe compile errors; first action-reader failure |
| `425-editor-final` / `35eec14` | 0 | 64.37 s | 5 project: 2 compiles, 2 links, 1 metadata; engine/plugin 0 | Native compile/link success; second reader failure |

The second action breakdown is explicitly a **manual review of retained log descriptions and native graph**, not a repaired automated gate. Its wrapper exits 1. Actual probe responses use `/std:c++17`, without `/Yc` or `/Yu`. Both runs used the required 14.16/17763 toolchain.

Compile fixes, applied identically in all three hosts: add `Policies/CondensedJsonPrintPolicy.h`; replace TArray `IsEmpty` with `Num`; explicitly construct `TPair<FString,int32>`. Errors at WorldProbe.cpp:45 and Driver.cpp:45,73,142,208-211 clear after one fix. The final run has no C++ error/warning. Two existing UBT warnings remain at user `BuildConfiguration.xml:16` (`bAllowUBAExecutor`) and `:22` (`MemoryPerActionBytes`); they were recorded, not changed.

The reader first rejected `EuropaPhaseAWorldProbe.cpp`. Its one correction admitted bare compiler status when `bShouldOutputStatusDescription=false`. The second run rejected `UE4Editor-EuropaPhaseAProbe.lib`, whose flag is true. UE4 `ParallelExecutor.cs:207-213,265-276` emits the bare status for that case too. The defect is mine; EN5 does not reset its D2 budget. No third parser attempt ran.

The combined closeout script was also abandoned after two failures: it initially asserted away the accepted XML warnings, then invoked EA11 on the non-reflected-only host probe. The independent existing EA8 exit tool completed protection checks afterward. An earlier invalid `--kind tooling` argument was rejected before an operation; `setup` was used. All failed attempts and partial receipts remain preserved.

Evidence: [first run](D:/IA_Europa/Receipts/09414/builds/425-editor/result.json), [first-run recovered diagnostics](D:/IA_Europa/Receipts/09414/recovery/425-first-compile/result.json), [second native result](D:/IA_Europa/Receipts/09414/builds/425-editor-final/result.json), [read-only second-run observations](D:/IA_Europa/Receipts/09414/closeout-final/native-build-observation.json).

## EI2 memory admission

Each build passed three polls about 20 seconds apart. Both ran with at most four actions; neither needed an in-run stop. No foreign process was touched.

| Operation | Admission wait | Physical free, GiB | Commit free, GiB | Stops |
|---|---:|---:|---:|---:|
| 02-425-editor | 40.01 s | 8.51 | 31.54 | 0 |
| 04-425-editor-final | 40.01 s | 8.36 | 31.24 | 0 |

Per-poll receipts are the `child-*-memory.jsonl` files in the corresponding ledger operations.

## Source changes, module comparison and EA11

| File(s), relative to the port | Change | 5.1 effect |
|---|---|---|
| `tools/europa/environment.py` | Optional native-toolchain comparison policy; strict wrapper boundary retained | C, tooling |
| `tools/europa/toolchain_environment.py` | Selected prefix and variable checks plus self-tests | C, tooling |
| `tools/europa/makefile_actions.py` | Read-only native graph reader and executed-log join; join currently blocked | C, tooling |
| `tools/europa/a5_build.py` | Rebuild before query, receipt-based owned makefile refresh, native evidence capture | C, tooling |
| `tools/europa/host_probe/EuropaPhaseAWorldProbe.cpp.txt` and `EuropaPhaseAProbeDriver.cpp.txt`, plus each host's corresponding `.cpp` | The three compatibility fixes above | C, isolated host, intended neutral |
| `docs/predictions/094-europa-phase-a.md`, session journal, result document, `CLAUDE.md` | Prediction, evidence and exact stop state | C, documentation |
| Production `Source`, `Content`, descriptor; host C# and `.uproject` | No edit in this job | B |

All five probe source files are identical across the three hosts. The accepted 094-13 module comparison and Shipping mutants are carried forward; no C#, descriptor or `.uproject` changed, so no additional JsonExport set ran. EA11 passes on all seven plugin reflected headers and detects its mutant. The probe's header is non-reflected; no host reflected declaration was added.

The dated prediction addendum was committed in `290a7d2` before the first native build. No prediction was weakened after a result. The previously accepted reader can-fails, fixture recipes, runtime readiness and content-admission analysis were not rerun or changed.

## Artifacts, EI4 and owner kit

All 27 4.25 files are present, with 25 prior hashes equal and only the probe DLL/PDB changed. Baseline51 remains 39/39 equal; ReGate51 remains 44/44 equal. The missing probe PDB and library were recreated. Full current hashes and source labels are preserved in [observed binary hashes](D:/IA_Europa/Receipts/09414/closeout-final/observed-binary-hashes.json) and [prior-manifest comparison](D:/IA_Europa/Receipts/09414/closeout-final/prior-manifest-comparison.json). These are **not final A5 manifests**.

| Rebuilt probe artifact | Bytes | SHA-256 |
|---|---:|---|
| `UE4Editor-EuropaPhaseAProbe.lib` | 2,074 | `3db365cd42557d1ceac9b6b27614824b5d98134ba043a9a0365f008376a0f373` |
| `UE4Editor-EuropaPhaseAProbe.dll` | 371,200 | `720b4301fd261a2df41aa3d72d26759b699064e78941cd093f91c394554eb477` |
| `UE4Editor-EuropaPhaseAProbe.pdb` | 23,932,928 | `fbcbadb959a8d15ba88ae39e13079abf8facb269a64e21a8e5bb8d6ac879453c` |

No executable was rebuilt. Shipping artifacts remain byte-equal. No archive was created, and nothing was pushed.

EI4 did not run: Baseline51 has not been rebuilt with the probe, and EN2 requires rebuilding before querying. The five unrun builds are 4.25 Game and both Development targets in Baseline51/ReGate51. No completed `Europa425-Editor-OwnerLeg.bat`, dry-run receipt or `094-14-owner-card-draft.md` is delivered. Uninstalled drafts in the run folder are not qualified launch tooling.

## Ledger and EN5

All eight operation safety ledgers PASS, including operations whose child failed. Port exception growth is **C: 0 B; E: 0 B; 5.1 engine bookkeeping: 0 B**. Both engine-tree inventories remain unchanged. The allowed C: changes have zero growth; no new exception class was used. [Exception-path before/after records](D:/IA_Europa/Receipts/09414/closeout-final/exception-ledger.json) retain each size, mtime and hash. Ledger evidence is 226,010,776 B, below 2 GiB. Protected enumeration has zero errors; the same 205 inaccessible OS/tooling entries are retained as explicit access errors, not empty inventories.

Free space columns are resolved **C/D/E GiB**. D delta is the inventoried port-tree logical-size change, not an IO-byte counter. Tooling overhead is outside the port budget; exact per-folder before/after values and paths are in each summary, including `.codex` resolving to E:. Between-operation deltas are retained separately.

| Operation | Free space before → after | Port D net bytes | Foreign changes | Tooling net bytes |
|---|---|---:|---:|---:|
| 01-en1-tooling | 18.97/82.72/236.50 → 18.97/82.52/236.47 | +54,914 | 3 | +67,388 |
| 02-425-editor | 18.97/82.52/236.43 → 18.97/82.50/236.25 | +16,173,508 | 2 | +100,586 |
| 03-probe-compile-fixes | 18.97/82.50/236.10 → 18.97/82.50/235.96 | +78,160 | 20 | +25,978 |
| 04-425-editor-final | 18.97/82.50/235.86 → 18.97/82.43/235.72 | +75,026,232 | 1 | +1,412,910 |
| 05-closeout | 18.96/82.43/235.63 → 18.96/82.43/235.61 | +34,671 | 6 | +144,969 |
| 06-closeout-final | 18.96/82.43/235.59 → 18.97/82.43/235.57 | +128,025 | 5 | +39,655 |
| 07-stop-docs | 18.96/82.43/235.55 → 18.96/82.43/235.53 | +0 | 2 | -35,645 |
| 08-exit-proof | 18.96/82.43/235.51 → 18.96/82.31/235.43 | +126,653,222 | 8 | +88,347 |

**One EN5 item applied:** remove only the owned target `Makefile.bin`, after a size/mtime/SHA-256 receipt, before each fresh native attempt. Stock UBT restores the cached environment at 4.25 `Engine/Source/Programs/UnrealBuildTool/Modes/BuildMode.cs:530-534` and 5.1 `:788-792`. Refreshing this regenerable cache preserves current routing and all non-blocked base variables. The original prediction cited the corresponding save sites; the journal records the restore citations. Two deletion receipts and resulting native makefiles are retained. No other EN5 reading was used.

## Commits and exact resume point

All commits are local on `feat/europa-port`, with author/committer date overrides absent:

| Commit | IST, 2026-10-25 | Purpose |
|---|---|---|
| `290a7d247024c8cfe5b8978c0889fa38560e1141` | 21:05:17 | EN1 and rebuild-before-query tooling, predictions |
| `35eec142f91ef3f440c1c1adf6186232a93bc9ec` | 21:13:23 | Probe compile compatibility; first reader correction |
| `cd5b9ad3e6a65711740c0bed494579a06695568c` | 21:35:06 | Committed stop state and resume point |

Chat needs to rule on a fresh action-reader harness budget. Proposed correction: follow the documented bare-status behavior for both boolean states and metadata, then exercise both engines' retained logs without a pre-build query. It is not implemented. Do not rerun the abandoned combined closeout script.

Then finish the 4.25 Editor evidence, the remaining five builds, all native makefile proofs, EI4, source-bound manifests and owner-kit dry run. A6 still needs its approved idle windows (300 s human idle, at least 6 GiB physical free, GPU at most 15%, no foreign barred Unreal processes, three passing polls), owner preview/eye leg, and content-admission safeguards. The owner card must say **answer No to any rebuild-modules prompt**. No owner launch is ready or requested by this result.

VERDICT: NEEDS-DECISION | A5 incomplete: D2 action-log reader exhausted; 4.25 Editor native build passed; five builds pending
