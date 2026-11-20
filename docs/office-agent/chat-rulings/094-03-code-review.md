# 094-03 — Claude Code review of Codex's Europa Phase A A1/A2 resume (W3: written after Codex exited)

**Date:** 2026-10-16, 17:54–18:20 IST · **Reviewer:** Claude Code (mailbox brief 094-03-europa-phase-a-a1-a2-resume.md)
**Job:** resume of thread `<uuid>` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`,
gpt-6-astra / max / danger-full-access (rollout `turn_context` 10:08Z, 10:41Z, 11:34Z), 15:38:07–17:54:06 IST, 8,159 s,
exit 0, one try. Codex's verdict: `GREEN | A1/A2 complete; protected state unchanged; both UBT matrices and blacklist
can-fails pass; A3 not started; no push`. Codex's report: `_reviews\094-03-codex-report.md`. My receipts:
`…\code-receipts-094-03\` (plus my JsonExport re-run receipts in `D:\IA_Europa\Receipts\CodeReview09403\`).

## Summary
- **A1 and A2 are done as ruled, and I reproduce the key results independently.** P9 is unchanged (my re-hash
  19,290/19,290). Both engine trees and the Paris original had 0 in-window writes. My own JsonExport re-run through the
  approved tooling matches Codex's exports module-object for module-object on all 23 exports, mutants included.
- **No port-attributable write outside D: except the 4.25 `XmlConfigCache` rewrite** (an EA4 exception, same 190 B).
  Codex's 19,944 B C: charge is CrashReportClient activity of the owner's UE 5.7 editors, charged conservatively.
- **Three GAPs, none blocking A1/A2:** EA3 (the 5.1 copies disable engine plugins their content needs), EA12 (the doc
  line is spliced mid-sentence), EB1 (the drive-root scope cannot see top-level directories). **One OPEN:** EA2 (C/E seams
  named, not yet built).
- **C: audit:** the port is not what eats C:. Today the consumers were Visual Studio Installer's background update
  download into `%LOCALAPPDATA%\Temp`, Windows/driver updates, and Windows shadow copies at their size limit.
- Counts: **16 OK · 3 GAP (EB1, EA3, EA12) · 1 OPEN (EA2) · 0 CONFLICT.**

## 1. Rulings EB1–EB5 and EA1–EA15
| Ruling | Evidence | Status |
|---|---|---|
| EB1 targeted write proof | `write_ledger.py`: 18 ops, before/after path/size/mtime of the EA4 exception paths and the declared scopes, budgets enforced, child receipts with D: TEMP/TMP, `uebp_*`, DOTNET/NuGet and W4, floors only, no ETW/USN/elevation. **Method gap:** its `c_root`/`e_root` scopes record top-level *files* only (`mode=='top'` never appends directories), so a new or deleted top-level folder on C:/E: is invisible to it. It missed the owner's deletion of `C:\Dev` (10.1 GiB). My root listings (dirs included) show no new top-level folder on C: or E:. | **GAP** (non-blocking) |
| EB2 headroom | My gate: GO at 15:28 (C 26.71 / E 54.91) and 15:37 (C 26.11 / E 54.85). Ledger op 01 enforces C≥13 / E≥52. No floor breach; E: fell to 50.57 at 16:36 (foreign: `E:\<personal-folder>`), then the owner freed space (E: 105.6 at 16:45). | OK |
| EB3 resume scope | Same thread. Entry proof = inventories + 25,866 hashes + git state, no full re-hash. A1 then A2, stopped at A2. | OK |
| EB4 C: audit | §6. | OK |
| EB5 process notes | No new ETW/logman/USN activity; RoomGenerator fix not repeated. | OK |
| EA1 accepted design | Base d7eee97, `feat/europa-port`, four new commits (no cherry-picks), 4.25 host `D:\IA_Europa\StylizedParisStreet`. Shim (C1) belongs to A3. | OK |
| EA2 full-parity seams | F: `FAnomalyControlTransportState` exists (opaque, native 5.1 / empty 4.25). C: the opaque `TUniquePtr` Async/PreviewTee/RunLog members already exist at d7eee97; the "native factories" Codex names do not exist yet. E: named facades + `AnomalyCapabilities.h` only. | OPEN (A3 must show C/E interfaces) |
| EA3 hosts | Layout, `/XJ` copies and CaptureBench are all correct (§3). **But the 5.1 copies disable EnhancedInput, ControlRig, Metasound, FullBodyIK and GameplayInsights.** StackOBot content references the first three: `BP_Bot`, `PC_InGame`, `ABP_Bot`, `CR_MovingPlatform`, 6 SFX. `GlobalDefaultGameMode=GM_InGame` references `PC_InGame`/`BP_Bot`, `CB_GateLevel` has no override, and `DefaultInput.ini` names `/Script/EnhancedInput` classes. **So every map, the gate level included, would load assets of disabled plugins: the copies are not the qualified configuration.** Codex followed the request's "disable any other plugin" literally. EA3's examples were project plugins (RoomGenerator, UnrealMCPython), and the engine plugins' binaries exist (EA8 allows them). | **GAP** (blocks A5/A6, not A1–A3) |
| EA4 exceptions/budgets | Charged C 19,944 B / E 0. The charged paths are CrashReportClient logs and analytics whose names carry the PIDs of the owner's UE 5.7 editors (2840, 34468, 26300, 35244), so they are foreign. `XmlConfigCache-E+EpicGames+UE_4.25.bin` was rewritten at 17:30 at the same size (4.25 UBT; an EA4 exception). `%LOCALAPPDATA%\UnrealBuildTool` and E: BuildRules: 0 changes. | OK |
| EA5 arbiter/denominator | In the committed predictions (AA-off, paired TAA/TSR, per-world eligible denominator). Unchanged since `ac81ecb`. | OK |
| EA6 idle gate, W3/W4 | Idle gate in `tools/europa/idle_gate.py`; my re-run 45/45 PASS, same case list as Codex's run. W3: I wrote mid-job notes only to `code-receipts-094-03\` and never steered Codex. W4: launch receipt shows neither date variable; every child receipt records both null; author = committer time on all 4 commits. | OK |
| EA7 inert shells | Bench/Shaders compiled-out logs, Control transport state empty on 4.25, `ANOMALY_*=0` exported on 4.25 (verified in matrices). | OK (A2 scope) |
| EA8 P9 + 5.1 tree | Codex's entry/exit pass; mine in §2. 0 build actions in all 7 UBT logs (JsonExport only; the only compile was each copy's own rules DLL under D:). | OK |
| EA9 archive state | Unchanged. | OK |
| EA10 job split | In predictions. | OK |
| EA11 UHT-safe headers | My re-run: port worktree and ReGate51 6 reflected headers / 0 forbidden; Codex's physical mutant `ProbeMutant.h` → FAIL, exit 1; release d7eee97 → 12 (unchanged from 094-02). | OK |
| EA12 doc lines | Keys renamed correctly. **But the explanation line is inserted mid-sentence** in `docs/PRE-DELIVERY-CHECKLIST.md:291–292` ("…so a / UE 5.1 reads these legacy… / Shipping build compiles…", which also breaks the checklist item's indentation) and in `docs/architecture.md:26–27` ("…none of the plugin; the / UE 5.1 reads… / `ANOMALY_CAPTURE=0`… branches are kept…"). Both sentences are garbled. | **GAP** (trivial) |
| EA13 descriptor Level 1 | Codex's matrices: 5.1 base/port 8 pairs, 4.25 5 pairs, both mutants change Shipping by exactly `AnomalyShaders`, both restored exports clean. Reproduced by me (§5). | OK |
| EA14 worktrees/commits | All three `worktree add` commands use `--git-dir=…AnomalyInjector/.git`; commits only on `feat/europa-port`; no push (71 refs unchanged + `feat/europa-port`). | OK |
| EA15 stop rules | No P9 difference, floor breach, engine/Paris write, protected-ref change or setting change. D2: two harness problems, ≤2 attempts each (op 04 classifier; op 09 reader). No editor, game, UHT or UAT child. | OK |

## 2. Independent P9, engine trees, protected git
- **P9 re-hash** (`p9_rehash.py`, seed 9403): all 19,270 protected exe/dll/pak/ucas/utoc + 20 random protected files =
  **19,290 OK, 0 hash, 0 size, 0 missing, 0 mtime**, 32.1 GB, 79 s, against `full-sha256.jsonl` (SHA-256 `db884f1e…`,
  re-verified).
- **My snapshots** (15:29 vs 17:56): 5.1 `Engine\Binaries\Win64` 1,607 files, all 498 engine-plugin Binaries,
  BuildRules, `Programs\UnrealBuildTool`, `Engine\Saved`: identical. **Full 4.25 install listing (179,502 files):
  identical.** J2 host binaries (69) and staging key files (82) identical by SHA-256; Paris original 1,511 files identical
  by SHA-256; StackOBot `.uproject`/Config/Source/Content identical.
- **In-window scan 15:38:07–17:54:30** (write or creation time): `E:\EpicGames\UE_4.25` 0/179,502 ·
  `D:\UESource\UnrealEngine` 0/339,409 · Paris 0/1,511 · `%LOCALAPPDATA%\UnrealBuildTool` 0. All three TEMP roots
  (`C:\ClaudeTemp`, `%LOCALAPPDATA%\Temp`, `C:\Windows\Temp`): no UE-named entry written.
- **Git:** refs 71 → 72 (only `refs/heads/feat/europa-port 4c6c1d2`). The seven protected worktrees have the same heads and
  0 tracked changes; `m51` 53bf725 clean. The new worktrees are clean: Paris `4c6c1d2` [feat/europa-port], Baseline51
  `d7eee97` detached, ReGate51 `2ee5cac` detached. In-window writes under `D:\IntrusiveAnomalies\StackOBot` are only the
  common repo's `.git` objects/worktrees/refs/logs.

## 3. EB1 routing and foreign writes (my scans vs Codex's ledger)
| Location (in window) | Hits | Attribution |
|---|---:|---|
| `%LOCALAPPDATA%\UnrealEngine` | 113 | 108 UE 5.7 `Common/Zen`; 4 `Editor\*.json` written 17:08–17:28 (UE 5.x editors; the port launched none); 1 `XmlConfigCache-…UE_4.25.bin` (port 4.25 UBT; EA4 exception, size unchanged) |
| `C:\ProgramData\Epic` | 20 | EOS updater run 17:24:22–34 (logs, InstalledItems, `NotAllowedUnattendedBugReports`), Zen state 17:08, launcher catalog 17:38: foreign |
| C: root (dirs incl.) | 2 + 1 deletion | `Windows`, `System Volume Information` (15:59:48 shadow copy); `C:\Dev` deleted by the owner (no mention in Codex's events or command index); no new folder |
| E: root (dirs incl.) | 1 | `E:\<personal-folder>` (owner's UE 5.7 project, launched by the owner's Codex desktop app): foreign |
| D: root | — | `IA_Europa` (port), `pagefile.sys` (system-managed) |

Codex's ledger and my scans agree on every port path. Its 1,555 recorded changes: 1,159 foreign · 386 tooling · 10
`port_C_exception` (the conservative CrashReportClient/XmlConfigCache charges). Tooling overhead: `.claude` (C:) +31.6 MB
across ops; `.codex` is a junction to `E:\CodexData\home` (since 2026-09-18) and grew 0.14 GB on E:; `C:\ClaudeTemp` flat.
**Note for chat:** the ruling's "`.codex` 24.70 → 24.71 GB" C: line in 094-02 was on E:.

## 4. Hosts
- **Layout:** `D:\IA_Europa\{StylizedParisStreet, Baseline51\StackOBot, ReGate51\StackOBot}`, plain folders, no junctions.
  Plugins: AnomalyInjector worktree in all three; CaptureBench only in the two 5.1 hosts.
- **Copies vs sources:** Content 2,743 / 2,743 / 1,453 files, 0 missing, 0 size differences. Exactly three deliberate edits
  per host: `.uproject` (plugin set), `DefaultEngine.ini` (D: DDC graphs `Europa425DDC` / `Europa51BaseDDC` /
  `Europa51PortDDC`, also set as Installed/Default graphs), and the game module `Build.cs`. The `Build.cs` edit is a
  JsonExport-only descriptor probe (`EuropaDescriptorProbe`), inert unless `EUROPA_DESCRIPTOR_PROBE` points under
  `D:\IA_Europa`; identical in both 5.1 hosts. It persists into the A4b builds of both hosts.
- **Sources unchanged** (Paris by SHA-256, StackOBot as §2).
- **CaptureBench:** my `git archive e64d6f9` SHA-256 equals Codex's tar (`752f196e…`, 870,400 B); in both hosts
  104/104 files match the commit's blobs, nothing extra, no Binaries.
- **Plugin set:** see EA3 GAP (§1). My content scan: `/Script/EnhancedInput` in 10 assets, `/Script/ControlRig` in 18,
  `/Script/MetasoundEngine` in 6; FullBodyIK, GameplayInsights, UnrealMCPython, RoomGenerator and PythonScriptPlugin
  in 0.
- **J2-era fixtures:** Codex lists 666 fixture-related paths per 5.1 copy (466 external actors, 197 CaptureBenchTexCorrupt,
  2 CaptureBenchGate, 1 AnomalyFixtures), none named Review093/J2_; generation stays with A5.

## 5. Commits and A2
- **Commits** (author = committer time, local only): `ac81ecb` 16:32:37 predictions + tools · `fe74543` 16:39:30 A2 source ·
  `2ee5cac` 16:53:51 reader fix **plus** the toolchain-dir log in `AnomalyInjector.Build.cs` (the subject names only
  the reader) · `4c6c1d2` 17:34:37 results, CLAUDE.md status. **First UBT executable 16:47:19 IST**, from the child
  receipts; the predictions are unchanged since `ac81ecb`. Source, descriptor and matrix tooling are unchanged from
  `2ee5cac` to `4c6c1d2`.
- **`git diff --stat d7eee97..feat/europa-port`:** 35 files, +1,321 / −163. Production: descriptor, five Build.cs, five reflected
  headers, 4 Compat headers, control transport state + `.cpp`, Shaders/Bench module stubs, Bench actor. Plus CLAUDE.md,
  two EA12 docs, routing doc, predictions, session journal, nine `tools/europa` files.
- **Read every source diff:**
  - The C# preserves 5.1 behaviour except the declared items: the exact 4.25/5.1 `BuildException`; C++17 only on 4.25;
    the G233-safe `#if UE_5_1_OR_LATER && !UE_5_2_OR_LATER` around the Renderer private include; `AnomalyInjector`
    moved from private to public dependency in Capture and ControlServer; new public defines; and a
    `Console.WriteLine` in every module's rules.
  - The control `.cpp` change is a mechanical move of 13 fields to `Transport->` (no collisions), plus out-of-line
    default and `FVTableHelper` constructors that allocate the state.
  - Header `#if __has_include` blocks moved into non-reflected Compat headers.
  - **Comment-free: 0 Python comment tokens in `tools/europa`; no comments in C#/C++ diffs.**
  - Style: new C# in tab-indented files uses 4-space indents.
- **EA11:** §1.
- **Shared C# rules compiled by both UBTs:** rules DLLs under each host's own `Intermediate\Build\BuildRules`
  (4.25 log: "Compiling … StylizedParisStreetModuleRules.dll"); my re-run left their hashes unchanged.
- **Toolset, from the ledger child logs** (`Anomaly profile:` goes to stdout, not the `-Log` file):
  - 5.1: `version=14.38.33144`, `toolchain=…\MSVC\14.38.33130`, SDK 10.0.22621.0, `cppStandard=Default`.
  - 4.25: `version=14.16.27051` (directory `14.16.27023`), SDK 10.0.17763.0, `Cpp17`.
  - The 5.1 logs also list installed 14.36 and 14.42 toolsets.
- **JsonExport re-run (mine):** I ran the approved code paths (`write_ledger` → `run_json_batch` → `descriptor_matrix`
  `verify`/`mutant`) with only the receipts root redirected to `D:\IA_Europa\Receipts\CodeReview09403`, so none of
  Codex's receipts were overwritten. The ledger op `cr09403-json` passed (C 0 / E 0 charged).
  - 4.25: 5 pairs. 5.1 base and port: 8 pairs each. Mutants: 4.25 Shipping 278→279 and 5.1 Shipping 291→292, adding
    only `AnomalyShaders`.
  - **All 23 exports equal Codex's, module object for module object; native predicates identical.**
  - Descriptors byte-restored (`7ea025bf…`, `d8dddf16…`); worktrees clean afterwards.
- **Finer 5.1 check than Codex's** (Codex compares module membership only): per-module objects, base vs port, differ
  **only** in the five IA modules: `ANOMALY_BENCH`/`ANOMALY_PROFILE_BENCH` defines, and `AnomalyInjector` moving from
  private to public dependency in Capture and ControlServer. No game, CaptureBench or engine module differs in any
  pair; Shipping is identical.
- **Idle gate:** re-run 45/45 (§1).

## 6. C: audit (EB4, sizes only, nothing deleted)
- **Start (15:28–15:34, free 26.99 GiB), largest consumers (logical sizes):**

  | Folder | GiB | Main contents |
  |---|---:|---|
  | `<USERPROFILE>` | 130.3 | AppData\Local 71.8 (Packages 16.5, which includes the Claude desktop VM disk `rootfs.vhdx` 8.76; uv 10.9; Google 9.5; Microsoft 6.2; NVIDIA 4.75); `.claude` 7.9; `.cache` 7.8 |
  | `C:\Windows` | 47.9 | WinSxS 20.1 (hard-link-inflated) |
  | `C:\ProgramData` | 33.8 | Microsoft 11.4; Epic 10.5, of which launcher VaultCache 8.2; NVIDIA 5.3 |
  | `C:\Program Files` | 31.5 | Visual Studio 9.7 |
  | `C:\Program Files (x86)` | 20.3 | |
  | `C:\Dev` | 10.1 | |

  Other system files: pagefile 0.50, swapfile 0.27, no hiberfil; `$Recycle.Bin` 0 visible, 3 denied.
- **End (17:56–18:07, free 39.66 → 38.41 GiB):**
  - **Removed by the owner during the job (≈23.5 GiB):** `C:\Dev` 10.1, Unity LocalLow 5.3 + Roaming 2.9, GitHubDesktop
    0.6, Microsoft PIX 0.3 (plus `%LOCALAPPDATA%\Temp` 12.6 before the job).
  - **Grew during the job:**
    - `%LOCALAPPDATA%\Temp` +3.76: folder `ghtija0r`, 3.68 GiB, 900 .vsix/.msi/.cab files. This is Visual Studio
      Installer's background update download ("BEGIN: Background Download" at 15:35:16; VC 14.44 CRT, IntelliCode,
      Copilot, .NET 9 SDK packages), still running at 18:05.
    - `C:\Windows` +1.85 (System32 +1.83; driver store, catroot).
    - `C:\Program Files\Piriform` +0.76 (CCleaner, which backed up the Intel UHD driver at 15:48).
    - `.claude` +0.04.
  - **Port: nothing** beyond the 190 B XmlConfigCache rewrite.
- **Invisible to file scans:** the `volsnap` log shows C: shadow copies at their user-defined size limit:
  - 12:16 the oldest was deleted to make room;
  - 12:49 copies were aborted because storage could not grow;
  - **15:59:48 a new copy was taken** (oldest deleted). This matches `System Volume Information`'s timestamp and follows
    CCleaner's driver install.

  Shadow-copy sizes need admin (`Win32_ShadowStorage` and `Get-ComputerRestorePoint` both denied; not pursued).
- **094-02's unexplained −2.6 GiB:** not established. In 094-02, `%LOCALAPPDATA%\Temp` (12.65 GB) was flat between 14:13
  and 14:46, but its growth before 14:13 was never measured. The candidates that fit today's evidence are:
  - the VS background downloader writing into that Temp folder;
  - shadow-copy copy-on-write growth, which a file scan cannot see;
  - driver/Windows updates.

  Correction: `.codex` was never on C:.

## 7. Top 5 risks for A3 (and the jobs it feeds)
1. **Host plugin set (EA3 GAP).** Fix before the first 5.1 launch (A5/A6), or both baseline and candidate run a
   non-qualified input/animation/audio stack. A baseline that then fails its own predicates would read as a port
   problem.
2. **First real UHT/C++ on 4.25 (A3→A4a).**
   - The control subsystem's user-declared default + `FVTableHelper` constructors with `TUniquePtr` to an incomplete
     type.
   - The other `TUniquePtr`-to-incomplete members (Capture ×3).
   - The A3 shim reparenting of five subsystems.
   - EA11 is static evidence only.
3. **5.1 neutrality beyond membership.** The public `AnomalyInjector` dependency widens include reach; R6/strict-include
   (A4b) must confirm no behaviour change. The `Anomaly profile:` `Console.WriteLine` prints once per module per target in
   every 5.1 build.
4. **Environment drift during A4.**
   - VS Installer is downloading an update that includes MSVC 14.44 components. If it installs, re-verify that 5.1 still
     selects 14.38.33130 on every A4b build.
   - Shadow copies are cycling on C:.
   - The owner's Codex desktop app keeps relaunching UE 5.7 editors (pids 39672, 2840, 34468, 26300, 35244 in 2 h), and
     free physical memory was 1.25 GiB. The idle gate will refuse most windows.
5. **Ledger and idle-gate method limits before runtime jobs.**
   - Drive-root scopes ignore directories (EB1 GAP).
   - The foreign-attribution rules assume "no renderer/editor child" (Codex says so too).
   - The idle gate exempts only listed PIDs (no descendants, e.g. ShaderCompileWorkers of an owned editor), sums all
     GPU-engine counters, and its real collector has never run.
   - Each ledger op stores ~380 MB of SQLite on D: (6.4 GiB this job).

VERDICT: REVIEW DONE — 16 OK / 3 GAP (EB1 root dirs, EA3 host plugin set, EA12 doc splice) / 1 OPEN (EA2) / 0 CONFLICT; P9 unchanged (Codex and independent 19,290/19,290); 0 engine/Paris writes; JsonExport reproduced exactly incl. both mutants; C: consumers are VS Installer background download, Windows/driver updates and shadow copies, not the port
