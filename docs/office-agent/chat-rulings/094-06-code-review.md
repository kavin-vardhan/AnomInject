# 094-06 — Claude Code review of Codex's Europa A3 recovery (W3: written after Codex exited)

**Date:** 2026-10-24, 10:43–11:05 IST · **Reviewer:** Claude Code (mailbox brief 094-06-europa-a3-recovery.md)
**Job:** resume of thread `<uuid>` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`,
gpt-6-astra / max / danger-full-access (rollout `turn_context` 2026-10-24T04:38:48Z), 10:05:43–10:42:21 IST, 2,198 s, exit 0,
one try. Codex's verdict: `NEEDS-DECISION | A3 stopped at EF2 on inherited 5.1 engine metadata outside ED4; protected
binary/config hashes match; no repository edits, cleanup, compile or push in 094-06; final compile pair pending`.
Codex's report: `_reviews\094-06-codex-report.md`. My receipts: `…\code-receipts-094-06\` and `D:\IA_Europa\Receipts\CodeReview09406\`.

## Summary
- **The STOP is correct, and it is about 094-05, not 094-06.** The interrupted 5.1 ReGate51 run (op `06-51-working`) left
  1,023 files changed in the 5.1 engine tree. They total 1,913,531 B and were all written 2026-10-22 17:05:55–17:06:04 IST.
  - 622 are UBT/UHT metadata under `Engine\Intermediate\Build`. ED4 allows that class only for a run that "succeeds".
  - 400 are under `Engine\Plugins\**\Intermediate\Build`, outside ED4's allowed root.
  - 1 is a leftover `Engine\Intermediate\UbtRuns\42980_…` marker.
  - ED4: "Any other write in the 5.1 engine tree is still a STOP." Codex stopped before EF1, cleaning, the include fixes and
    any compile, and widened no allowlist. My independent scan reproduces the 1,023 files exactly (§5).
- **Nothing protected changed, and 094-06 itself wrote nothing to either engine.** P9 v1 still applies: the owner's hand
  delete has not happened.
  - P9: my re-hash is 19,290/19,290 OK; Codex's entry and exit hashes are 25,866/25,866.
  - The 4.25 engine and the Paris original: 0 writes since 10-22 15:52.
  - My JsonExport re-run equals Codex's 094-05 set on all 21 exports, with all three mutants.
- **What 094-05 left that is verified:** the 4 commits (`90377bf`, `88b01f1`, `e421441`, `5bd368b`) are clean and local;
  `git fsck` and the open ledger's integrity both pass. The 4.25 Editor compile at `5bd368b` has 0 errors and 0 C++ warnings
  (C2855 9→0, C5043 20→0, no suppression). The 5.1 compile at `5bd368b` was cut off at action 2/29, with 8 first-header
  errors.
- **Counts: 17 OK · 2 GAP · 6 OPEN · 1 STOP (ED4, 5.1) · 0 CONFLICT.** Plus one deviation of my own (§11).

## 1. Rulings
| Ruling | Evidence | Status |
|---|---|---|
| EF1 memory gate | Not implemented, because entry stopped first. Memory-gate log: no waits, no starts, no stops. Codex took one read-only reading at 10:09 (7.76 GiB physical / 42.03 GiB commit free). Mine: 09:52 at 8.99 / 40.62 GiB; the lowest during the job was 5.18 / 36.91 GiB (10:15). | OPEN (blocked by STOP) |
| EF2 recovery | EA8 entry vs v1: all protected hashes equal; STOP only for the inherited metadata. `git fsck --connectivity-only` exit 0 (mine: 56 dangling, 0 missing/broken). `PRAGMA integrity_check` on `09405\ledger\current.sqlite` = `ok` (mine, read-only: 792,255 rows, size and mtime unchanged by the check). No journal/WAL/SHM residue. 4.25 outputs: the 17:01 receipt hashed only compiler response files, never DLLs (mine: 0 DLL hashes in `builds\425-01\*.json`). So EF2's hash check cannot be done and its fallback applies: re-run 4.25. The five plugin DLLs date from inside 425-01's window (16:57:38–16:58:05) and equal Codex's reported current hashes. ReGate51 cleaning not done. | OK (checks) / OPEN (cleaning, 4.25 re-run) |
| EF3 cleanup / P9 | Owner has not deleted (§3). v1 applies; keep list, junctions, Content and BankOverflow all verified. | OK (owner step pending) |
| EE1 UHT C: classes | 094-05 window: `%LOCALAPPDATA%\UnrealHeaderTool` changed only in the ten `Config\Windows\*.ini` files (16:55) and a `CrashReportClient\UE4CC-Windows-*` folder (16:54). Codex charged 112 B of UHT crash-report config plus 2,072 B of foreign Adobe logging. 094-06: no change (my before = after). | OK |
| EE2 Target.cs probe rooting | ReGate51 and Baseline51 `StackOBot.Target.cs` / `StackOBotEditor.Target.cs` hash-identical (mine). Codex: all six differ from the originals only in the probe block. | OK |
| EE3 4.25 PCH policy | `Rules.PCHUsage = NoPCHs` sits inside `if (Is425(Target))` (diff). 4.25 export: `PCHUsage=NoPCHs` for the plugin modules. 425-01: 0 errors (C2855 9→0). 5.1 module objects equal release apart from the accepted A2 surface (§10). | OK |
| EE4 C5043 | 20 → 0 after EE3; no suppression added (425-01 log: 0 C++ warnings). | OK (not needed) |
| EE5 5.1 pass-through | Diff level (`668c075..5bd368b`): on 5.1, `SupportsAnomaly` returns true; `Register` adds unconditionally; the five constructor finders are back to unconditional inside `#if ANOMALY_ENGINE_51`; `ApplyAnomaly` / `TryFireSpecific` / `SetAnomalyEnabled` / pool / selector filters are all `#if ANOMALY_ENGINE_425`; the 5.1 selector cycle/choice expressions equal `d7eee97` verbatim. `ANOMALY_ENGINE_51` comes from `Compat/AnomalyCapabilities.h` (→ `AnomalyEngineVersion.h`, with an `#error` if neither engine), included second in the Injector `.cpp`. Codex's 12 token comparisons are empty. Runtime parity is A9's. | OK (diff level) |
| EE6 tooling | Env receipt case-insensitive, module parser from `Source\<Module>\`, cumulative budget suite revived, UHT `-UserDir` removed. My re-runs: resume 16/16, delta 9/9, budget 5/5 (the 30 Codex cites). | OK |
| EE7 cache cleared | Not done (094-05 hook refusal); owner list pending (§3). | OPEN (owner) |
| ED1 disk | Lowest seen in 094-06: C 18.97, D 138.38, E 53.80 GiB; never below a warning. | OK |
| ED2 working compiles | 425-01 at `5bd368b`: StylizedParisStreetEditor Win64 Development, `-NoEngineChanges … -MaxParallelActions=1`, 20 actions (project 1 / plugin 19 / engine 0 / engine-plugin 0), exit 0, 249.9 s. 51-01 at `5bd368b`: same flags, 29 admitted (9 / 20 / 0 / 0), interrupted at [2/29], 8 first-header errors. **No final pair.** | OPEN |
| ED3 toolchain | Unchanged (§2 of the context): VS 2022 17.12.35707.178, MSVC 14.38.33130 (cl 14.38.33144) and BuildTools 2017 14.16.27023 (cl 14.16.27051), SDK 17763 + 22621, no 14.44. Logs: 4.25 "VS2017 14.16.27051 … 10.0.17763.0"; 5.1 "VS2022 14.38.33144 (…14.38.33130) and 10.0.22621.0". | OK |
| ED4 engine trees | 5.1: the inherited 1,023-file set, outside ED4's root/classes and from a run that never finished (§5, §12). 4.25: 0 writes (179,502 files). 094-06 added nothing. | **STOP** (5.1) / OK (4.25) |
| ED5 ledger | 094-06 ledger 217,221,046 B, one initial inventory plus deltas, root folders included; 0 access errors. Delta 9/9 (mine). | OK |
| ED6 profile line | Unchanged since 094-04; 5.1 export still equals release (§10). | OK |
| ED7 probe | Mutants reproduced on both engines (§10). | OK |
| EC1 plugin set | Carried. My export membership equals Codex's on all 21. | OK |
| EC5 A3 risks | Item 3 (5.1 include reach): 8 first-header errors surfaced and are not yet fixed; cl never got past action 2. | OPEN |
| EB1 write proof | Codex's per-op ledger: C 0 / E 0 / engine 0 in 094-06. My before/after snapshots are equal on every engine, C: exception and staging location (§6). | OK |
| EA8 P9 / 0 engine actions | P9 hashes unchanged (§4). 5.1: 0 engine and 0 engine-plugin actions admitted, yet metadata was written (the STOP). | OPEN (5.1) |
| EA11 headers | Mine at `5bd368b`: 7 reflected / 0 violations in both the Paris and ReGate51 roots; the mutant FAILs (line 13 `#if ANOMALY_CAPTURE`). | OK |
| EA14 commits | No 094-06 commit. The 4 094-05 commits have author time = committer time; refs 72 → 72; `feat/europa-port` not on origin. | OK |
| EA15 stop rules | STOP obeyed; no engine repair, no allowlist change. | OK |
| Receipts (ED2 fields) | 425-01 recorded no produced-binary hashes, so the EF2 check was impossible. The 5.1 admission recorded no `/std` flags (unquoted `@response` arguments; Codex recovered `/std:c++17` from all 12 response files). | GAP |
| Self-test suites | `tools/europa/tooling_self_tests.py` (A1/A2-era, last changed `ac81ecb`) is stale. It loops Baseline51 through build mode, the ED2 guard added at `9fdc8f0` raises, and without `--output` it writes under the default `Receipts\09405`. It is not one of the 30 Codex cites. | GAP (minor) |

## 2. The 17:11 crash (read-only System event log)
- **It was a restart from the Start menu, after Windows ran out of commit, not a kernel crash.**
  - Event 2004 ("low virtual memory condition") was logged at 16:32:20, 16:49:19 and 17:10:22. Each time it names the
    owner's three `blender.exe` as top consumers; at 17:10:22 they held 22.21 / 22.18 / 20.01 GB of virtual memory.
  - 17:11:32: event 1074, `StartMenuExperienceHost.exe` restarted the PC on behalf of the owner's account
    ("Other (Unplanned)").
  - Boot at 17:12:18. There is no event 41, no 6008, no BugCheck 1001 and no WHEA-Logger event.
  - The restart ended Codex's console processes (exit 0x40010004 on its last op) and Claude Code's.
- **10-18 was different:** event 41 plus 6008 ("previous system shutdown at 15:13:14 on 10/18/2026 was unexpected"). That
  is an unclean stop (a hang, a hard reset or power), with no BugCheck or WHEA either.
- **10-15:** planned Windows Update restarts at 04:29–04:33, then one owner Start-menu restart at 11:33.
- **Today:** load 61 %; 8.99 GiB physical and 40.62 GiB commit free; no Blender or UE 5.7 running. The D: page file is
  system-managed at 43,008 MB.

## 3. The cleanup and P9
- **Not done.** Two inventories of `E:\IA_BuildCache` 5 min 42 s apart (09:54:00, 09:59:42) are identical to each other and
  to 094-05's before-inventory: 432,470 files, 397,028,291,180 B, 10,509 folders, 41 groups, 3 reparse points, 5 worktree
  `.git`, 49,686 hard-linked inodes.
- **P9 v1 applies** (`full-sha256.jsonl` `db884f1e…`, 707,775 lines). No `baseline-v2` folder was written. Codex
  re-derived nothing (inapplicable) and ran entry and exit against v1, so we agree.
- **Keep list:**
  - release baseline 8/8 size- and hash-equal;
  - all six D: junctions into the cache resolve;
  - `D:\IntrusiveAnomalies\StackOBot\Content` (a real folder) is byte-identical to its 094-05 listing (2,744 lines);
  - `D:\IA_BankOverflow`: 0 missing and 0 changed of v1's 89,583 files. `09102-final-binaries.zip` (2026-10-03) lies
    outside v1's roots and predates v1.
- **E:** 53.81 GiB, before and after. It was 91.08 GiB on 10-22; the drop is outside the cache.

## 4. P9, checked independently
- **Re-hash** (seed 9406) of all 19,270 protected exe/dll/pak/ucas/utoc files plus 20 random others: **19,290 OK**, 0
  hash, 0 size and 0 missing mismatches, 32.1 GB, 68 s.
- **Worktree heads and index hashes** are equal before and after 094-06. ReGate51's index mtime moved at 10:34; its hash
  is the same.
- **Refs** equal (72). `m51` is at `53bf725` with 0 tracked changes. The three port worktrees each have 0 status lines.

## 5. Engine trees
- **My scan of everything written since 2026-10-22 15:52:**
  - 5.1: 339,410 files scanned; **1,023 hits, 1,913,531 B, mtimes 17:05:55–17:06:04 only.**
    - `Engine\Intermediate\Build`: 414 `LiveCodingInfo.json`, 207 UHT `Timestamp`, 1 `ActionHistory.bin`;
    - `Engine\Plugins\**\Intermediate\Build`: 239 `LiveCodingInfo.json`, 161 `Timestamp`;
    - `Engine\Intermediate\UbtRuns`: 1.
  - 4.25: 0 of 179,502. The Paris original: 0 of 1,511.
  - That matches Codex's table class for class.
- **My snapshots (Binaries\Win64, every engine plugin's Binaries, BuildRules, Programs\UnrealBuildTool, Saved, full 4.25
  listing):** equal between 094-05-before and 094-06-after. The one exception is `Engine\Intermediate\Build`, whose 622
  files are the same set.
- **UBT logs:** both 094-05 runs passed `-NoEngineChanges -NoBuildUHT -NoHotReload -NoXGE -MaxParallelActions=1`, and
  each admission counted 0 engine and 0 engine-plugin actions.

## 6. EB1 routing in 094-06 (my before 10:03 / after 10:44)
- **Equal:**
  - `%LOCALAPPDATA%\UnrealEngine`, `UnrealBuildTool` and `UnrealHeaderTool`;
  - `ProgramData\Epic`;
  - the UE-named TEMP entries;
  - the D: and E: roots;
  - the 4.25 BuildRules;
  - staging and host binaries (SHA-256);
  - StackOBot's `.uproject`, Config, Source and Content.
- **C: root:** only `C:\ClaudeTemp`'s mtime changed.
- **`D:\IA_Europa`:** the only changes were Codex's `Receipts\09406` and `Temp\09406`.
- **Codex's ledger agrees:** exception charges C 0 / E 0 / engine 0.

## 7. The closure (`git diff --stat 668c075..5bd368b`: 22 files, +436/−128; `d7eee97..5bd368b`: 98 files, +4,488/−300)
- **Capture (`e421441`):** `#if ANOMALY_CAPTURE` guards around the bodies of 19 helper functions, with `#else` returning
  false or 0, plus the disabled `PreviewPump/Arm/Poll` exports. When enabled, the bodies are byte-unchanged.
- **Compat additions:** `StaticLodScreenSize`, `PredictedLod`, `ViewFrustumWithoutFarPlane`, `AxisVector`,
  `IsSceneCaptureOnly`, `NearClipSmallNumber` and `ResetFunction`.
  - Each 5.1 branch is the original expression verbatim: `.GetValue()`, `GetPredictedLODLevel()`,
    `GetViewFrustumBounds(…, true, false)`, `FVector::X/Y/ZAxisVector`, `bVisibleInSceneCaptureOnly != 0`,
    `UE_KINDA_SMALL_NUMBER` (returned as a double: a widening of a float, so the comparison is unchanged) and
    `TFunction::Reset()`.
  - The 4.25 frustum builds near, left, right, top and bottom and leaves out the far plane, which is the same plane set.
- **C4099:** `class FStaticMeshRenderData` forward declaration.
- **The empty `protected:` blocks** in five headers were removed (EE5 allows it).
- **The C3 rule** holds: 4.25 compiled with 0 errors.
- **Cosmetic:** the re-indented loop in `SetAllAnomaliesEnabled` and a stray blank line in two loops. `NearClipSmallNumber`
  is a misnomer for a box-volume epsilon.
- **No comment tokens were added.**

## 8. Working compiles
See ED2. The 4.25 log shows exactly 20 actions, all inside the host. The only warnings are the 2 user-XML
`BuildConfiguration.xml` lines (pre-existing). The 5.1 errors are all `…\Anomalies\Anomaly_<X>.cpp(1): error: Expected
Anomaly_<X>.h to be first header included.`, for CameraClipping, ChangeCase, CorruptedTexture, LodPopping,
MissingTexture, StuckLowMip, TexCorrupt and TimeDilation. **No 5.1 cl result exists.**

## 9. EF1 and EF2 in this job
EF1 has no log and no self-tests, because it was never reached. EF2's cleaning receipt does not exist, and the interrupted
ReGate51 outputs are still in place. Both wait for the ED4 ruling (§12).

## 10. JsonExport re-run (mine, ledger op `cr09406-json` → `process_wrapper` → `descriptor_matrix`, Paris tools at `5bd368b`)
- **Ledger:** PASS, no stops, exception charges C 0 / E 0 / engine 0; about 5 min.
- **All 21 exports equal Codex's 094-05 set** (`Receipts\09405\descriptor-sets\initial\matrices`) in membership and in
  every module object, with native predicates equal: 4.25 5 pairs; 5.1 base 8; 5.1 port 8.
- **Mutants:**
  - plugin blacklist on 5.1 (Shipping 302 → 303) and restored;
  - probe on 5.1 (302 → 303) and on 4.25 (278 → 279), each restored.
- **Base vs port 5.1, after normalising host paths:** they differ only in the five IA modules: the bench/profile defines,
  and `AnomalyInjector` moving from private to public in Capture and ControlServer. Shipping is identical.
- **The three mutated descriptors** were rewritten at 10:51:01–05 and are byte-equal to their pre-run hashes. All three
  worktrees are clean.

## 11. Self-tests, and a deviation of mine
- **Results:** delta 9/9, budget 5/5, resume 16/16, idle gate 45/45: PASS. EA11 and its mutant behave as expected (§1).
  Ancestry: 746 text files, 57 `TickableWorldSubsystem` name hits, 0 ancestry lines on subsystem types; 6,368 assets, 0
  hits.
- **My error, recorded:** running the stale `tooling_self_tests.py` without `--output` created **10 empty folders** at
  10:54:32 (`D:\IA_Europa\Receipts\09405\routing\` and nine `prepared\…` children), before the suite raised.
  - My ledger op and the resume self-test also left two empty folders: `Temp\09405\cr09406-json` and
    `Temp\09405\resume-selftest`.
  - There are no files in any of them, and nothing in 09405 was modified.
  - They are outside my allowed write list and were not removed (Code may not delete). Receipt:
    `code-receipts-094-06\review\own-out-of-scope-write.txt`.
- **A second slip, caught by the mutant:** I first ran my EA11 checker on file paths instead of plugin roots. It then read
  "0 reflected" and passed the mutant. Re-run on roots, it gives the results above.

## 12. The ED4 STOP, for chat
- **The writes are structural, not an accident of the interruption.** In 5.1 UBT:
  - `UEBuildModuleCPP.cs:745` writes `LiveCodingInfo.json` for every module whenever the target supports Live Coding;
  - `ExternalExecution.cs:855` writes UHT `Timestamp` files for every module not marked read-only, and in a source-built
    engine that includes enabled engine plugins;
  - `UnrealBuildTool.cs:596/710` creates a `UbtRuns\<PID>_<hash>` marker at start and deletes it in `finally`.
  - So **every** 5.1 build-mode run, including a clean successful one, rewrites the `Engine\Intermediate\Build` and
    `Engine\Plugins\**\Intermediate\Build` metadata. Only the marker is left behind by an interrupted run.
  - The 4.25 install is an installed engine, so its plugins are read-only and it wrote 0.
- **Without an amended ED4, no 5.1 compile can pass:** not A3's final pair, not A4b's Game builds, not the strict pass and
  not the Baseline51 control.
- **The content is metadata.** All 653 `LiveCodingInfo.json` files are byte-identical to v1 (only their mtimes moved); 140
  of the 368 `Timestamp` files and `ActionHistory.bin` differ. No binary, generated code, Source, Content, Config, Shaders
  or Programs file changed.
- **My recommendation (chat decides), close to Codex's:**
  - Admit, for 5.1 build mode only, exactly `LiveCodingInfo.json` and UHT `Timestamp` under `Engine\Intermediate\Build\**`
    and `Engine\Plugins\**\Intermediate\Build\**`, plus `ActionHistory.bin` and makefiles under `Engine\Intermediate\Build`.
    Add `Engine\Intermediate\UbtRuns\<wrapper-owned PID>_*`.
  - Keep the 100 MB cap, the per-op path, size, mtime and hash receipts, `-NoEngineChanges`, and 0 engine and 0
    engine-plugin actions.
  - Say that metadata from an interrupted or EF1-stopped run counts against the same cap and is accepted when the
    protected hashes match.
  - Rule on the 85-byte stale marker for dead PID 42980. Either accept and record it in place, or authorise its removal
    by receipt (today no one may delete in the engine tree).
  - **Source fact for that ruling** (`Engine\Source\Developer\DesktopPlatform\Private\DesktopPlatformBase.cpp:756–`):
    `FDesktopPlatformBase::IsUnrealBuildToolRunning()` walks `Engine\Intermediate\UbtRuns`. For each file it checks whether
    the named PID's application path hashes to the recorded MD5, and **deletes the file when it does not**.
  - So the first 5.1 editor that makes that check (the owner's StackOBot editor, or A5's first editor launch) will remove
    the marker itself. That removal will appear as an engine-tree change that the port did not make at that time.
  - Whatever chat rules should therefore also say that this engine-side removal is expected. A stale marker is otherwise
    harmless: it reads "running" only if PID 42980 is reused by the same UBT executable path.

## 13. Top 5 risks for A4a and A4b
1. **The ED4 metadata class (§12)** blocks every 5.1 build-mode run until chat rules: the A3 pair, A4b Game/Shipping, the
   strict pass and Baseline51.
2. **Memory.** The owner's Blender renders took the PC out of commit three times in 40 minutes on 10-22.
   - EF1 is still unbuilt.
   - A4's Game, Shipping and strict builds are longer than the Editor Development legs, so the 4-hour wait could keep
     timing out while renders run.
   - The 10-18 stop (event 41/6008) shows unclean stops happen here too.
3. **5.1 has never compiled the closure past action 2.**
   - The 8 first-header fixes touch 8 guarded Injector TUs. Each must put its own header first while keeping that header
     self-contained and free of UE5 expressions in the disabled 4.25 TU (C3).
   - More 5.1-only cl errors are likely behind them. 69 production files differ from `d7eee97`.
4. **Receipt and tooling gaps** to fix before A4 evidence counts:
   - no produced-binary hashes in ED2 receipts;
   - no `/std` capture for unquoted `@response` arguments;
   - the stale `tooling_self_tests.py`, which writes under the default `Receipts\09405`;
   - the empty folders from §11.
5. **EE5 holds at diff level only.** The 5.1 asset finders and every enabled 5.1 body sit behind function-like macros
   (`ANOMALY_ENGINE_51`, `ANOMALY_CAPTURE`). A future TU that misses the include compiles the 5.1 code out silently,
   because an undefined macro evaluates to 0 with no error. A9 must assert the registry, catalogue and asset finders at
   runtime on 5.1.

VERDICT: REVIEW DONE — 17 OK / 2 GAP (ED2 receipts lack binary hashes and /std capture; stale tooling_self_tests.py) / 6 OPEN (EF1, EF2 cleaning + 4.25 re-run, EE7 owner delete, ED2 final pair, EC5 5.1 include reach, EA8 5.1) / 1 STOP (ED4: 1,023 inherited 5.1 engine metadata files, 1.91 MB, correctly fired) / 0 CONFLICT; P9 v1 unchanged (Codex 25,866 hashes; mine 19,290/19,290); 0 writes to either engine in 094-06; JsonExport reproduced on 21 exports + 3 mutants; 17:11 was an owner Start-menu restart under Blender commit exhaustion, not a kernel crash
