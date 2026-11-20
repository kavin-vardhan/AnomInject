# 094-04 — Claude Code review of Codex's Europa Phase A job A3 (W3: written after Codex exited)

**Date:** 2026-10-22, 14:58–15:30 IST · **Reviewer:** Claude Code (mailbox brief 094-04-europa-phase-a-a3.md)
**Job:** resume of thread `<uuid>` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`,
gpt-6-astra / max / danger-full-access (rollout `turn_context` 07:01Z, 07:05Z, 07:53Z, 08:33Z, 09:17Z), 12:31:18–14:57:53 IST,
8,795 s, exit 0, one try. Codex's verdict: `NEEDS-DECISION | A3 incomplete; 4.25 UHT passed but C++ failed; non-exempt UHT C:
config writes triggered STOP; P9 and both engines unchanged; no 5.1 compile or push`. Codex's report:
`_reviews\094-04-codex-report.md`. My receipts: `…\code-receipts-094-04\` and `D:\IA_Europa\Receipts\CodeReview09404\`.

## Summary
- **The STOP was correct, and it was declared before the compile, not made up afterwards.** Codex's pre-compile admission
  (`Receipts\09404\precompile\425-write-routing.json`, 13:43) said UHT's `Saved/Config` would be routed to D: by a
  process-local `UE-CmdLineArgs -UserDir=…`, and that any write to the C: location would be a STOP. The first 4.25 Editor
  working compile (`9fdc8f0`, 14:09:20) ran UHT. UHT rewrote 10 two-byte inis under
  `%LOCALAPPDATA%\UnrealHeaderTool\Saved\Config\Windows`, created a new `CrashReportClient\UE4CC-Windows-A17EAF…` folder, and
  removed the previous `UE4CC-Windows-4E7D…` folder. That location is not an EA4 exception. The ledger fired, and nothing ran
  after it: no compiler fix, no second build, no 5.1 compile.
- **Done as ruled:**
  - EC1 (plugin set), EC2 (doc lines), EC3 (root folders), EC4 (seams), ED3 (toolchain), ED6 (once per target);
  - ED7 (probe skeleton and mutants), except for one host-file gap;
  - the C1–C5 closure source as designed (it does not compile yet);
  - EA11, the ancestry audit, and the ED5 ledger.
- **Independent checks:**
  - P9 re-hash 19,290/19,290;
  - 0 writes in either engine tree or the Paris original;
  - my JsonExport re-run equals Codex's on all 21 exports, mutants included, module object for module object;
  - EA11 and its mutant; ancestry with 0 consumers;
  - ED5 delta 9/9 and idle gate 45/45. **The retained A1/A2 budget suite now crashes.**
- **Counts: 23 OK · 2 GAP (ED5, ED7) · 3 OPEN (ED2, EC5, EA8) · 2 STOP (ED4 C: half, EA4: one event) · 3 n/a · 0 CONFLICT.**

## 1. Rulings
| Ruling | Evidence | Status |
|---|---|---|
| ED1 disk limits | Entry 26.26/129.66/33.45 GiB. Lowest seen in the job: C 23.42 (14:58), D 108.68 (12:57; `D:\pagefile.sys`, system-managed, grew 51.5→72.0 GiB under the owner's UE 5.7 editor commit, 20.3 GiB), E 28.72 (14:58). Never below a warning. | OK |
| ED2 working compiles | Allowed pairs enforced twice (`process_wrapper.command`, `working_build.build_command`). One 4.25 run, `-NoEngineChanges -NoBuildUHT`, commit-bound (`9fdc8f0`); all fields recorded. 0 errors not reached; 5.1 not run (STOP). No Game/Shipping/Baseline51/strict/clean/hot-reload. | OPEN (blocked by STOP) |
| ED3 toolchain | Mine (vswhere): VS 2022 17.12.35707.178 + VS 2017 BuildTools 15.9.34031.82; MSVC 14.36/14.38.33130/14.42 and 14.16.27023; no 14.44. Fresh 5.1 JsonExport logs 14.38.33144 from `14.38.33130`, SDK 10.0.22621.0. 4.25 compile log: "Visual Studio 2017 14.16.27051 toolchain (…\14.16.27023) and Windows 10.0.17763.0 SDK". VS 17.14 (MSVC 14.44) is downloaded, not installed. | OK |
| ED4 engine trees | 5.1 and 4.25: my before/after/review-end listings identical (incl. 5.1 `Engine\Intermediate\Build`, 19,768 files); in-window scan 0/339,409 and 0/179,502; 5.1 metadata 0 B (no 5.1 compile). 4.25 source routing shown before compiling (caches go to the project). **C: half failed: UHT wrote a non-exempt C: location.** | OK (engines) / **STOP** (C:) |
| ED5 ledger | Root scopes include folders. One initial full inventory, then gzip deltas with snapshot hashes. Compile-child observer. 0.388 GiB (≤ 2 GiB). Earlier ledgers untouched (09403, P9, CodeReview09403: 0 writes in window). Delta self-tests 9/9 (my re-run). **GAP:** the retained `ledger_budget_self_tests.py` now raises `ValueError` (its `snapshot` stub returns 1 value; `finish()` unpacks 2), so its `cumulative_C` per-job case is no longer exercised anywhere. | GAP (minor) |
| ED6 profile line | Once per target per UBT run: 4 lines / 5 pairs (4.25), 7 / 8 (5.1), 1 for the plugin mutant. Content unchanged; `AnomalyInjector.Build.cs` re-indented to its tabs. The other Build.cs files are each internally consistent. 5.1 port export = release (§8). | OK |
| ED7 probe | Empty `EuropaPhaseAProbe` (Core only, `IMPLEMENT_MODULE`, no Compat include) in all three hosts; 5.1 sources identical; legacy `BlacklistTargetConfigurations: ["Shipping"]` in each `.uproject`; mutants on both engines put it into Shipping and restore (reproduced). **GAP:** all six host `Target.cs` files gained a `ProjectDescriptor` loop adding the probe to `ExtraModuleNames`, because a `.uproject` declaration alone does not root a project module. Necessary for the probe to build, and recorded in a dated addendum. But it is beyond ED7's text, and against plan C4's "4.25 targets remain byte-identical". It also changes the Baseline51 control's targets (identically in both 5.1 hosts). | GAP (ratify) |
| EC1 plugin set | Mine: 5.1 copies = original list except `UnrealMCPython` (disabled) and `RoomGenerator` (disabled, not copied); `AnomalyInjector` explicit. Editor DLLs exist for EnhancedInput/ControlRig/FullBodyIK/Metasound/GameplayInsights/Python/WebSocket (built 2026-07-29). Codex: 140 enabled plugins, 239 engine-plugin binaries in the export. | OK |
| EC2 doc lines | Both lines now follow their sentences. | OK |
| EC3 root folders | In the ledger root scopes; synthetic tests for new C:/D:/E: root folders; no test folder created. My C:/E: root listings: no new folder. | OK |
| EC4 seams | Reflected surface (mine, at `9fdc8f0`): Injector 6 UPROPERTY + 1 UFUNCTION (`OnWatchedTargetEndPlay`), bench actor 3 UPROPERTY, the other five reflected headers none; no version-specific type is reflected. C and E declared as non-reflected interfaces with `CreateCaptureBackend425` / `CreateTextureAdapter425` (declared, not defined, not wired). Landing files named in `docs/europa/phase-a-a3-closure.md` (code stays comment-free). | OK |
| EC5 A3 risks | (1) and (4) verified; (2) UHT passed, C++ failed; (3) 5.1 include reach untested (no 5.1 compile); (5) idle gate still never run for real. | OPEN |
| EC6 C: record | Port C: charge this job: 0 B of allowed growth; the UHT writes are a location breach, not budget. | n/a |
| EB1 write proof | Per-op inventories, in-window scans (mine agree, §3), child receipts (casing defect: `UE-CmdLineArgs` missing from env receipts), no ETW/USN/elevation. It caught the breach. | OK |
| EB2 headroom | Superseded by ED1. | OK |
| EB3 resume scope | Same thread; entry proof only (707,775 inventories, 25,866 hashes). | OK |
| EB4 / EB5 | Not in this job's scope. | n/a |
| EA1 shim contract | Code matches C1 and the 5.1 engine class line by line (§6). | OK |
| EA2 seams | C/E declared, F existing. | OK |
| EA3 hosts | Unchanged layout; EC1 applied. | OK |
| EA4 exceptions | `%LOCALAPPDATA%\UnrealHeaderTool\Saved\Config` is not an exception: one breach event (11 writes + 1 folder removal), 0 B net. XmlConfigCache 4.25 rewrite: an exception, same size. | **STOP** |
| EA5 arbiter/denominator | Predictions addenda add only EC1/ED7/ED2/toolchain expectations; no threshold retuned. | OK |
| EA6 W3/W4 | My notes only in `code-receipts-094-04\`; no steering. W4: launch receipt shows neither date variable; author = committer time on all 6 commits. | OK |
| EA7 inert shells | Disabled TUs and facades as designed; tick audit non-Shipping, `-IAIBench`, default off, one branch. | OK |
| EA8 P9 | Codex entry/exit 0 differences; mine 19,290/19,290 (§2). 4.25 compile: 0 engine actions (35 planned, all 57 produced items inside the host). **5.1 "0 engine and 0 engine-plugin actions" is still unproven** (no 5.1 build, 5 engine plugins re-enabled). | OPEN (5.1) |
| EA9 archive | Unchanged. | OK |
| EA10 split | A3 only; A4 not started. | OK |
| EA11 headers | 7 reflected headers / 0 forbidden (Codex's and my checker, port worktree and ReGate51); release 12; physical mutant FAILs. | OK |
| EA12 docs | See EC2. | OK |
| EA13 descriptor proof | Reproduced (§8). | OK |
| EA14 commits | 6 local commits on `feat/europa-port`, no push; refs 72 → 72 (only `feat/europa-port` moved). | OK |
| EA15 stop rules | STOP fired and was obeyed. D2: five harness issues, each fixed once. | OK |

## 2. Independent P9, engine trees, git
- **P9 re-hash** (seed 9404): all 19,270 protected exe/dll/pak/ucas/utoc plus 20 random protected files = **19,290 OK**, 0 hash, 0 size,
  0 missing, 32.1 GB, 186 s, against `full-sha256.jsonl` (`db884f1e…`).
- **My snapshots** (12:24 before / 14:58 after / 15:20 review-end): identical across all three, except where noted below:
  - 5.1 `Binaries\Win64`, all engine-plugin Binaries, `Intermediate\Build`, BuildRules, `Programs\UnrealBuildTool`, `Saved`;
  - the full 4.25 listing (179,503 lines);
  - J2 host binaries (69) and staging key files (82) by SHA-256;
  - the Paris original by SHA-256;
  - StackOBot `.uproject`/Config/Source/Content;
  - `%LOCALAPPDATA%\UnrealBuildTool`.

  The before-snapshot was also identical to my 094-03 exit snapshot, except `%LOCALAPPDATA%\UnrealBuildTool`, which the
  owner's UE 5.7 UBT rewrote on 10-21.
- **Git:** `feat/europa-port` `4c6c1d2` → `668c075`. ReGate51 followed each commit: `eb7b8e0` 12:55, `2e062ea` 13:24, `71e8d47`
  13:26:25 (before the 13:26:41 export), `9fdc8f0` 13:55. Baseline51 stays at `d7eee97`. All three are clean. `m51` `53bf725` is
  clean. No other ref or worktree changed. The only in-window repository writes are `.git` objects, refs and the three port
  worktree admin folders.

## 3. EB1 routing in build mode (in-window scan 12:31:18–14:58:00)
| Location | Hits | Attribution |
|---|---:|---|
| `E:\EpicGames\UE_4.25` / `D:\UESource\UnrealEngine` / Paris original | 0 / 0 / 0 | — |
| `%LOCALAPPDATA%\UnrealHeaderTool` | 11 (+1 folder removed) | **Port (4.25 UHT, 14:09:57–14:10:03): the STOP** |
| `%LOCALAPPDATA%\UnrealEngine` | 59 | 1 `XmlConfigCache-E+EpicGames+UE_4.25.bin` 14:10:05 (port, EA4 exception); 58 UE 5.7 `Common\Zen`/`Editor`/`5.7\Saved` (owner's editor, foreign) |
| `%LOCALAPPDATA%\UnrealBuildTool` | 0 | `-Log=` redirect held |
| `C:\ProgramData\Epic` | 22 | EOS updater 12:47–12:48, launcher manifests 14:56: foreign |
| C: root / E: root (folders included) | 1 / 0 | `C:\pagefile.sys` (Windows paging); no new folder |
| TEMP roots (UE-named) | 0 | — |

Codex's ledger and my scans agree on every port path. Its 12 UHT deltas = my 11 writes plus the removed old CRC file. Its
port-attributed allowed-exception growth: C 0 B, E 0 B. ED5's root-folder scope works: the delta self-test rejects new
C:/D:/E: root folders, and my root listings show none.

## 4. Toolchain (every compile and export log)
- 4.25 compile `425-01`: VS2017 14.16.27051 (dir 14.16.27023), SDK 10.0.17763.0, plugin responses `/std:c++17`, shared PCHs at
  the default standard.
- 5.1 exports (Codex's and mine): VisualStudio2022 14.38.33144 from `…\MSVC\14.38.33130`, SDK 10.0.22621.0,
  `cppStandard=Default`.
- No fallback toolset was selected.

## 5. EC1
§1. My plugin-list diff and the binary existence check agree with Codex's `hosts\effective-plugins-and-binaries.json`.

## 6. The closure (source review)
- `git diff --shortstat d7eee97..feat/europa-port`: 96 files, +4,174/−294.
- `2ee5cac..feat/europa-port`: 77 files, +3,135/−352 (58 of them production Source files; 69 since d7eee97).
- `9fdc8f0..668c075` changes no production file. No C#, descriptor or `.uproject` changed after `71e8d47`, so step 3 still
  covers the compiled source. No comment tokens were added anywhere.
- **Shim:** matches C1 and engine 5.1 `UTickableWorldSubsystem`:
  - `ShouldCreateSubsystem`: Super first, then `Cast<UWorld>`, Game/PIE only. On 5.1 that equals the removed overrides
    (default Game/Editor/PIE ∩ Game/PIE).
  - Tick type: template → Never, otherwise the base.
  - `IsTickable`: 4.25 `!IsTemplate() && bInitialized`; 5.1 delegates to the base.
  - `IsAllowedToTick() const final`, without `override`.
  - `Initialize`/`Deinitialize` check-and-set with no added chain; `Tick` has the engine's `checkf`; `GetStatId` has the
    `PURE_VIRTUAL` fallback.
  - Default and `FVTableHelper` constructors; one `FTickableGameObject` base.
  - All five `DoesSupportWorldType` overrides are removed (leaving empty `protected:` blocks; cosmetic).
- **Nine raw pointers:** Injector 5 scalars plus `TArray<UObject*> TexCorruptStrongRefs`, bench actor 3. Names and specifiers
  are kept, scalars are null-initialized, everything stays under `UPROPERTY` (GC traversal kept). No weak pointers.
- **Compat:** each 5.1 branch is the original expression verbatim (`GetSkinnedAsset`, `GetRenderData`, `GetCameraCacheView`,
  `GetFinalPerspectiveNearClipPlane`, `PerspectiveNearClipPlane > 0`, `bCastContactShadow`, `ChaosTriMeshes`). The 4.25
  branches follow the plan, plus PhysX `TriMeshes`, which is recorded reach. `ANOMALY_ENGINE_51` comes from a header that
  every guarded `.cpp` includes first, so the guards evaluate correctly on 5.1.
- **Capability policy (diff level):**
  - `GAutoPool` (9 keys), `GAutoPoolDefaultEnabled` and `GAnomalyChoices` are byte-unchanged from `d7eee97`, and every ID is
    in the 5.1 supported list, so the 5.1 eligible set, draw order and defaults are unchanged.
  - Unsupported IDs are filtered before the draw (`TryFireOnce`), so they never consume one.
  - 4.25: blinking/lighting_mismatch/lod_corruption, plus missing_object only with `-IAIBench`; others fail closed with
    `unsupported_engine_phase_a`.
  - The 5.1 list's 14 IDs match every registered `GetId()` exactly.
- **4.25 inert closure:** complete disabled `FAnomalyPreviewTee`, `FAnomalyRunLog` and preview exports. The 4.25-only
  unsupported-texture facade never reports applied, ready or measured. The 4.25 stock Nanite probe returns false; `AllowNanite`
  is untouched. The 4.25 constructor runs no asset finder (fields stay null).
- **5.1-visible changes beyond the N class, all small:**
  - new warning texts for unknown or unsupported IDs (`ApplyAnomaly`, `TryFireSpecific`, `SetAnomalyEnabled`);
  - `Register` now also filters by the list (see risk 4);
  - `PhaseABackends().TextureAdapters = ANOMALY_ENGINE_51 && !UE_BUILD_SHIPPING` now gates the 5.1 asset finders. This is
    harmless only because all five modules are Shipping-blacklisted.
- **Ancestry (mine):** 745 text files, 57 `TickableWorldSubsystem` hits: 7+7 includes, 5+5 shim parents, 14+14 shim
  definitions (two candidate worktrees), and 5 release parents (Baseline51). 0 Cast/IsA/IsChildOf/StaticClass/
  GetDerivedClasses/GetSubsystemArray lines on subsystem types. 6,368 `.uasset`/`.umap` byte-scanned (ASCII, UTF-16 LE/BE):
  0 hits. No asset was loaded.
- **EA11:** §1.

## 7. Working compile (`425-01`)
- **Run:** commit `9fdc8f0`; StylizedParisStreetEditor Win64 Development;
  `-NoHotReload -NoEngineChanges -Verbose -NoBuildUHT -NoHotReloadFromIDE -NoXGE -MaxParallelActions=1`; UHT exit 0 (7.5 s).
  35 actions (project 11 / plugin 24 / engine 0; all 57 produced items inside the host); UBT exit 6.
- **Errors:** 9 × C2855 (`/std:c++17` against shared PCHs at the default standard). The other ~125 are mostly the
  `AnomalyCaptureSubsystem.cpp` cascade from :1448 (unguarded writer/mask/stencil helpers), plus `TFunction::Reset`, C4099
  `FStaticMeshRenderData` class/struct, `GetValue`/`GetPredictedLODLevel`, the 4-argument `GetViewFrustumBounds`, axis constants
  and `UE_KINDA_SMALL_NUMBER`.
- **Warnings:** 20 × C5043 (operator new/delete exception specification; 4 per plugin module), all new port warnings.
- **Fixes:** none attempted; each proposed fix stays inside C1–C5.
- **Outputs:** two partial host/probe DLLs left on D:.

## 8. JsonExport re-run (mine, through the approved `write_ledger` → `process_wrapper` → `descriptor_matrix`, receipts redirected)
- **Scope:** ledger op `cr09404-json` PASS (C 0 / E 0 / engine51 0, no stops); 217 MB.
- **Hosts:** 4.25 5 pairs, 5.1 base 8, 5.1 port 8.
- **Mutants:** plugin-blacklist mutant (Shipping 302 → 303, `AnomalyShaders`), and probe mutants on 5.1 (302 → 303) and 4.25
  (278 → 279, `EuropaPhaseAProbe`). All restored to none.
- **Comparison with Codex:** all 21 exports equal Codex's in membership and in every module object; native predicates are
  equal.
- **Base vs port 5.1, after normalising host paths:** they differ only in the five IA modules (`ANOMALY_BENCH`/
  `ANOMALY_PROFILE_BENCH` defines; `AnomalyInjector` private → public in Capture/ControlServer). Shipping is identical.
- **Restoration:** descriptors byte-restored; the three worktrees are clean.
- **Writes the re-run made outside my receipts:** the three mutated descriptors were rewritten at 15:17:34–39 IST (ReGate51
  `AnomalyInjector.uplugin` and `StackOBot.uproject`, Paris `StylizedParisStreet.uproject`). Their bytes equal the pre-run
  hashes in `CodeReview09404\pre-rerun-hashes.txt` and `post-rerun-hashes.txt`; only their mtimes moved.

## 9. Self-tests
- ED5 delta suite: 9/9 PASS.
- Idle gate: 45/45 PASS.
- Codex's EA11 self-test: PASS.
- A1/A2 budget suite: **crashes** (§1, ED5).

## 10. The UHT STOP, for chat
- **The location is the 4.25 installed-engine UHT program's own user Saved folder.** The log says `Installed Engine Build: 1`
  and reads `<USERPROFILE>/AppData/Local/UnrealHeaderTool/Saved/Config/Windows/Engine.ini`. The ten inis there were created
  2023-09-14, and the owner's own 4.25 build on 10-05 rewrote them. So every 4.25 UHT run writes there, including A4a's
  Editor/Game builds and the strict pass.
- **The CrashReportClient config is written in the first second of the run** (14:09:57, before AppInit). That fits Codex's
  reading that the `UE-CmdLineArgs` merge comes too late. 4.25 UBT fixes UHT's argv, and I see no native-argv route without an
  engine edit or a per-user setting, both of which are forbidden.
- **My recommendation (chat decides):** admit an exact, narrow EA4 class:
  `%LOCALAPPDATA%\UnrealHeaderTool\Saved\Config\Windows\*.ini` and `…\Saved\Config\CrashReportClient\UE4CC-Windows-*\`.
  Name the replace-the-previous-CRC-folder behaviour explicitly, keep the 250 MB C: budget, and keep per-op path/size/mtime/hash
  evidence. It is the same kind as EA4's "UBT and UHT logs that cannot be redirected".

## 11. E: audit (sizes only, at start; `E:\<personal-folder>` not measured)
- **E: free went from about 102 GiB (10-16, 094-03 exit) to 33.7 GiB this morning.** Files written on E: since then:
  - **`E:\<personal-folder>` 66.1 GiB:** the PickAFight "svs-epic" Unreal 5.7 project 53.4 GiB, red-blue-explainer renders 6.6 GiB,
    three other PickAFight folders 3.3 GiB, an archive 1.1 GiB;
  - **`E:\CodexData` 7.7 GiB:** the Codex desktop app's home, sessions and runtimes;
  - `E:\EpicGames` 0.08 GiB.
- **None of it is the port.** The port's E: charge was 0 B again, and `E:\IA_BuildCache` has had no new file since 10-16.
- **By 15:20, E: free space was back to 91.1 GiB**, which looks like the owner freeing space; the audit did not re-measure
  folders.

## 12. Top 5 risks for A4a and A4b
1. **UHT C: writes block every 4.25 build until chat rules (§10).** A4a (Editor/Game/Shipping), the strict pass and
   any re-run of `425-01` all execute 4.25 UHT. The wrapper's engine-action gate cannot prevent it, because UHT runs during the
   `-WriteOutdatedActions` plan invocation, before admission. It can only be detected.
2. **The C++17/shared-PCH fix and the 4.25 API closure.**
   - C2855 needs a 4.25-only PCH or standard policy. If it touches C#, the full descriptor set and all three mutants must
     re-run.
   - The 20 new C5043 warnings need a fix or an accepted-warning ruling.
   - The Capture subsystem helper guards are the bulk of the errors. Watch them for reflected-surface or renderer reach.
3. **5.1 has never compiled this closure.**
   - 58 production files changed in A3 (69 since d7eee97), a new public dependency was added, and C4099 will also warn on 5.1.
   - EA8's "0 engine and 0 engine-plugin actions" is unproven now that five engine plugins are re-enabled (EC1).
   - The downloaded VS 17.14 / MSVC 14.44 update could be installed at any time and would stop 5.1 under ED3.
4. **The 5.1 capability list duplicates the registry.** `SupportsAnomaly` hard-codes 12 IDs + 2 bench twins and now gates
   `Register`, so a future 5.1 anomaly missing from the list would be silently absent from the 5.1 registry. The 5.1 asset
   finders now depend on `!UE_BUILD_SHIPPING` through `PhaseABackends()`. A9's catalogue/registry gate should assert both.
5. **Tooling defects to fix before A4 evidence counts:**
   - the environment receipt omits `UE-CmdLineArgs` (Windows key casing);
   - the diagnostics module parser mislabels modules;
   - the A1/A2 budget can-fail suite is dead (cumulative case unexercised);
   - the host `Target.cs` probe rooting (ED7 GAP) is carried into A4's Game/Shipping builds and the Baseline51 control;
   - the idle gate has still never run for real.

VERDICT: REVIEW DONE — 23 OK / 2 GAP (ED5 old budget suite broken, ED7 host Target.cs rooting) / 3 OPEN (ED2, EC5, EA8-5.1) / 2 STOP (ED4 C:, EA4: the UHT Saved/Config write, correctly fired) / 3 n/a / 0 CONFLICT; P9 unchanged (Codex and independent 19,290/19,290); 0 engine/Paris writes; JsonExport reproduced exactly on 21 exports incl. 3 mutants; E: space went to the owner's E:\<personal-folder> (66 GiB) and Codex app data (8 GiB), not the port
