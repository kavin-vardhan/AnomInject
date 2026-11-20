<!-- Collected by Claude Code (mailbox brief 094-13) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md; sha256 8F4ED60FB89F79F2CDAA2D9FE9C2B2B3066EDD6A4945370EE08CD81638561961, 13417 bytes; thread 01a14334, resume 18:21:58-19:17:39 IST, 3341 s, exit 0; body below is byte-identical -->
094-13 — Europa A5 partial result, 2026-10-25

**A5 is incomplete. EL1 stopped the first 4.25 Editor action-plan query because stock UBT changes `PATH`. No compile or link action executed.**

- EM1 is verified; full P9 v2 entry and exit pass.
- EL1/EM2 filtering, 26 synthetic tests and the real fresh-child test pass. Native SDK module comparisons and all three Shipping mutants pass.
- No blocked name or canary reached the recorded port child environments or the regenerated makefile. Exact native preservation fails on `PATH` only.
- UBT query cleanup removed the old 4.25 probe PDB and its Intermediate import library. The other 109 prior manifest files remain hash-equal; no restoration or A5 re-binding was performed.
- Six rebuilds, the clean-engine query and the owner kit remain unfinished. No runtime launch, engine edit, global setting change, push or tag.

Evidence root: [09413 receipts](<D:/IA_Europa/Receipts/09413>). Full report: [report.md](<D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/report.md>).

**Entry, exit and toolchains.** Both EA8 proofs inventoried 472,553 protected files, admitted the same 1,023 inherited EG1 metadata entries, and fully hashed 23,033 protected binaries/configs (104,794,989,456 bytes). Source-copy inventories, protected refs, indices and statuses match. m51 has the baseline 15 untracked entries; its branch and index are unchanged. The five prunable registrations remain untouched. V2 was re-derived byte for byte: `8e3ed0956a28e51c4205e334a5f9d81bbedd8c3afb606ed7234e83c111a32c74`. This was the scoped per-job proof, not a new full protected-file hash. [Entry](<D:/IA_Europa/Receipts/09413/P9/entry/summary.json>); [exit](<D:/IA_Europa/Receipts/09413/P9/exit/summary.json>).

Toolchain checks retain VS 2022 17.12.35707.178 and Build Tools 2017 15.9.34031.82; MSVC directories 14.36.32532, 14.38.33130, 14.42.34433 and 14.16.27023. No 14.44. Native exports select 5.1 MSVC 14.38.33130 (cl 19.38.33144), and 4.25 MSVC 14.16.27051 with SDK 10.0.17763.0. No component or configuration was changed.

**EL1/EM2 and EL2.** The denylist drops the four orchestration prefixes, `CLAUDECODE` and all EK1 secret-named variables, then adds declared process-local routing. The launch environment contained both the fresh canary and CLAUDECODE as positive controls. The 26 tests include SDK/unknown-variable preservation, dotted names, letter-valued length bytes, missing/changed variables and blocked-name mutants. A separate fresh child preserves the real non-blocked environment, including `VULKAN_SDK`, `VK_SDK_PATH` and `LINUX_MULTIARCH_ROOT`. [Tests](<D:/IA_Europa/Receipts/09413/environment/self-tests.json>); [fresh child](<D:/IA_Europa/Receipts/09413/environment/real-child.json>).

All 66 recorded port-child launch boundaries pass; values are masked and only non-blocked fingerprints are retained. The native reader finds no missing or blocked name, but `PATH` differs in both Makefile.bin and the exported compile environment. Every other inherited variable matches. UBT adds `VC_COMPILER_PATH` and `VC_COMPILER_DIR`. The one action-plan-generated makefile has no blocked names; it is not the requested six post-rebuild proof. The final literal-canary scan is [canary-final09413.json](<D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/canary-final09413.json>); no literal value is printed or stored.

The stock 4.25 code calls `SetEnvironmentVariables` in `UEBuildTarget.cs:1596`; `Platform/Windows/VCEnvironment.cs:141–184` prepends the selected SDK x64, MSVC HostX64 x86 and MSVC HostX64 x64 directories. `Modes/BuildMode.cs:513–516` then serializes that environment. Diagnosis confirms exactly those three entries and a byte-identical original PATH suffix. Equivalent 5.1 source at `VCEnvironment.cs:149–180` predicts compiler/SDK augmentation there too; no 5.1 build was attempted. [Value-free diagnosis](<D:/IA_Europa/Receipts/09413/stop/native-path-diagnosis.json>).

**Module comparison.** All 8 Baseline51, 8 ReGate51 and 5 stock-4.25 pairs match the accepted pre-A5 module objects except the intended probe dependency change. Earlier accepted A2 differences from literal release remain explicitly recorded. The plugin Shipping-blacklist mutant on 5.1 and probe blacklist mutants on both engines changed the expected membership, then restored byte-exactly and re-exported. [Matrix summary](<D:/IA_Europa/Receipts/09413/matrices/step3-summary.json>).

**Rebuild status.** The planned source was `8c4a1c36547ef3197e0ab0665319f66412f992f6`. The first UBT action-plan invocation exited 0 in 1.916 seconds (UBT reports 1.73); the environment wrapper exited 1 after 42.077 seconds including admission. `result.json` has a null build exit because the build stage never started.

| Host | Editor Development | Game Development |
|---|---|---|
| 4.25 Paris | Action-plan only; EL1 STOP | Not run |
| Baseline51 | Not run | Not run |
| ReGate51 | Not run | Not run |

Command actually run:

```text
E:\EpicGames\UE_4.25\Engine\Binaries\DotNET\UnrealBuildTool.exe StylizedParisStreetEditor Win64 Development -Project=D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject -Log=D:\IA_Europa\Receipts\09413\builds\425-editor\UBT.log -NoEngineChanges -NoBuildUHT -NoHotReload -NoHotReloadFromIDE -NoXGE -MaxParallelActions=4 -Verbose -WriteOutdatedActions=D:\IA_Europa\Receipts\09413\builds\425-editor\actions-1.json
```

Actual selection: `Visual Studio 2017 14.16.27051 ... Windows 10.0.17763.0 SDK`; profile `cppStandard=Cpp17`. The exported plan contains 6 project actions (3 compile, 2 link, 1 metadata), 0 plugin, 0 engine-source-in-host, 0 engine-tree and 0 engine-plugin actions. All executed action counts are zero. Three planned compile responses have `/std:c++17` and no PCH flags; they were not executed and are not compiler qualification. No binary was produced. The two existing user XML warnings are at `BuildConfiguration.xml:16` (`bAllowUBAExecutor`) and `:22` (`MemoryPerActionBytes`); no C++ warning assessment is possible without compilation. [Build receipt](<D:/IA_Europa/Receipts/09413/builds/425-editor/result.json>).

EI2 admitted after three polls, 20 seconds apart: physical/commit GiB = 8.44/32.04, 8.38/31.94, 8.45/32.00. Total wait 40.019 seconds; no in-run memory stop or process kill. [Memory log](<D:/IA_Europa/Receipts/09413/ledger/04-425-editor/child-007-memory.jsonl>).

**Artifact binding and query side effects.** Stock 4.25 `BuildMode.cs:321` deletes stale `DeleteItems` before its export branch at `:341–348`; `ActionGraph.cs:650–665` implements the deletion. UBT.log:539–540 records the two owned deletions. This is query preparation, despite zero executed compile/link actions. The first diagnostic inventory stopped on the missing PDB; the second recorded the absence explicitly. No retry of UBT and no automatic restore occurred.

| Existing set | Hash-equal | Result |
|---|---:|---|
| 4.25 A4a | 26/27 | Old probe PDB missing; manifest incomplete |
| Baseline51 | 39/39 | Preserved |
| ReGate51 | 44/44 | Preserved |

Missing: `D:/IA_Europa/StylizedParisStreet/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.pdb`, prior 3,461,120 bytes, SHA-256 `41db342a455e34d637a7c1c411f52245f59ed983c0b3f644df0a3b53a9afd28b`. The second deleted item is `Intermediate/Build/Win64/UE4Editor/Development/EuropaPhaseAProbe/UE4Editor-EuropaPhaseAProbe.lib`, outside that 27-file manifest. Every retained EXE/DLL is unchanged. The following receipts list every artifact path and full current/prior hash; they are preservation checks, not new qualification:

- [425 artifact hashes](<D:/IA_Europa/Receipts/09413/stop/425-prior-binaries.json>) — manifest SHA-256 `552815c5c734e2ea7e4aef584282621af9514dc11017a7a419ba3047320ae269`.
- [51base artifact hashes](<D:/IA_Europa/Receipts/09413/stop/51base-prior-binaries.json>) — manifest SHA-256 `e50fd7eaa81002d84b588eed653bdc90da7810ed83b1e0f88c3d7c62e5204e86`.
- [51port artifact hashes](<D:/IA_Europa/Receipts/09413/stop/51port-prior-binaries.json>) — manifest SHA-256 `07ccb8e62fbd566335b13d5abc3c3760bf6e1c55be3851c662acacaa43c9e779`.

**Changes, predictions and checks.** `environment.py`, `write_ledger.py`, `a4_build.py`, `idle_gate.py` and `prepare_runtime.py` implement the environment policy and evidence checks (tooling changes, intended neutral to 5.1 runtime). The requested docstrings were removed from six tools before qualification. Three identical 161-byte duplicate ED7 `Private/EuropaPhaseAProbeModule.cpp` files were deleted by hash receipt, leaving the intended A5 implementation once per host. All three five-file probe source sets match. This host-only correction is uncompiled. Production plugin Source, Content and descriptor are byte-identical to the starting commit.

EA11 scans 7 reflected headers across candidate production and the three probes, with zero violations; its in-memory forbidden-conditional mutant is detected. The 094-11 accepted probe/readers (69 can-fails), fixture recipes, 15-minute observe-only collection and static content-admission analysis are retained. No runtime gate or owner leg is claimed. The 094-13 prediction addendum was committed before native exports or the action-plan query and explicitly predicted this PATH stop; no threshold or assertion was weakened after observing it.

Commits, author and committer times (W4 variables absent; feature branch only):

- `9674fa557d85dd85113e3dbbe65d097391dd1b0f 2026-10-25T19:04:44+05:30 2026-10-25T19:04:44+05:30 docs(europa): record A5 native PATH contract stop`
- `8c4a1c36547ef3197e0ab0665319f66412f992f6 2026-10-25T18:39:28+05:30 2026-10-25T18:39:28+05:30 fix(europa): preserve child environments under EL1 and EM2`

Paris and ReGate51 are clean at `9674fa557d85dd85113e3dbbe65d097391dd1b0f`; Baseline51 stays at d7eee97. The final commit is documentation only. Pushed commits: none. New archives: none. Runtime bench levers changed: none. Mutants are restored.

**Ledger and disks.** All seven operation ledgers pass; a child command failure in operations 04/05 is separate from the write-proof result. New charged growth: C: 0 B / 250 MB, E: 0 B / 50 MB; new EG1 bookkeeping: 0 files, 0 B / 100 MB. There were C: exception overwrites, so zero charge does not mean zero C: writes. Neither engine tree changed. Ledger evidence is 226,140,835 bytes, below 2 GiB. Protected enumeration had zero errors; 205 denied OS/tooling entries per inventory are listed, never treated as empty.

| Operation | C before→after GiB | D before→after GiB | E before→after GiB |
|---|---:|---:|---:|
| 01-denylist-install | 18.87→18.88 | 83.40→83.19 | 241.70→241.60 |
| 02-denylist-tests | 18.88→18.81 | 83.19→83.19 | 241.43→241.33 |
| 03-matrices | 18.81→18.81 | 83.19→83.14 | 240.11→240.11 |
| 04-425-editor | 18.81→18.81 | 83.14→83.14 | 240.10→240.10 |
| 05-stop-diagnosis | 18.81→18.80 | 83.14→83.14 | 240.10→240.10 |
| 06-stop-diagnosis-complete | 18.80→18.80 | 83.14→83.14 | 240.09→240.09 |
| 07-closeout | 18.80→18.80 | 83.14→83.02 | 240.09→240.05 |

All readings are resolved volumes and remain above ED1 warning levels. Per-operation D: totals, exception path/size/mtime/hash changes, foreign writes and C:/E: tooling overhead are in [ledger summary](<D:/IA_Europa/Receipts/09413/stop/ledger-summary.json>). `.codex` resolves to E:/CodexData/home; `.claude` and ClaudeTemp resolve to C:. Tooling overhead is outside the port budget.

**Decision and exact resume point.** Chat needs to decide whether EL1 may admit only the exact stock toolchain/SDK PATH prefix while requiring the original PATH suffix and every other base value to remain byte-identical. Keep all blocked-name/canary checks. This exception is proposed only; no engine edit, global PATH change or pre-seeding was applied. EL1 attempt 1 reached the native contract stop; no second native attempt was spent. The separate diagnostic inventory used two attempts to handle the query-deleted PDB transparently.

After a ruling, resume at the six Development rebuilds, validate all regenerated native environments, run the Baseline51 clean-engine query, and re-bind touched artifacts including the missing 4.25 PDB. Then finish and dry-run the EL4 owner launcher and plain card. Neither `Europa425-Editor-OwnerLeg.bat` nor the 094-12 owner-card draft was produced by this job. A6 still needs those deliverables, an idle window, the owner preview/eye steps and the frozen content-admission checks. No owner launch is ready or authorized from this result.

For A6, EA6 requires input idle >=300 seconds, free physical memory >=6 GiB, GPU <=15%, no foreign listed Unreal/game/UAT/cook/shader-worker process, and resolved disk floors, on three passing polls about 25 seconds apart. The wait limit is four hours; loss of the gate invalidates a leg. The owner must wait for chat and the completed kit; that card must name the exact Paris mesh and preview duration, warn of first-run shader compilation, forbid conversion/resaving and plugin Content access, and explain No/Cancel plus exit on unexpected dialogs. Those live content and preview predictions remain untested.

VERDICT: NEEDS-DECISION | A5 incomplete: stock UBT PATH augmentation fails EL1; 0 compile actions; 109 prior artifacts equal, 4.25 probe PDB missing after query cleanup
