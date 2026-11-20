# 094-09 — Claude Code review of Codex's Europa A4b, and the EI7 build-cache delete (W3: written after Codex exited)

**Date:** 2026-10-25, 04:17–04:45 IST · **Reviewer:** Claude Code (mailbox brief 094-09-europa-a4b-51-builds)
**Job:** resume of thread `<uuid>` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`,
gpt-6-astra / max / danger-full-access, 2026-10-24 22:24:29 → 2026-10-25 04:16:33 IST, 21,123 s (5.87 h), exit 0, one try.
Codex's verdict: `GREEN | A4b baseline/candidate builds, R1 and 5.1 strict complete on 1c4cce7; P9 v2 and ledgers pass; A4a artifacts unchanged; no push`.
Codex's report: `_reviews\094-09-codex-report.md` (report.md sha256 `7411BBFC…`, 35,672 B). Ledger row `094-01-europa-phase-a-plan-resume7` (appended by the relay).
My receipts: `…\code-receipts-094-09\` (cleanup, protect, snapshots, `review\`) and `D:\IA_Europa\Receipts\CodeReview09409\` (self-tests, EA11).

## Summary
- **EI7 is done.** I deleted the build cache before launch: 29 roots, 432,433 files, 394.09 GB logical, 0 errors. E: free went
  **48.04 → 271.87 GiB (+223.8 GiB)**. The 3 junctions were removed link-only, and their targets are intact and equal to 094-05.
  The protected state passed at start, after the cleanup and after Codex exited. No delete was refused by my safety hook.
- **A4b holds up on my independent reading.** All five ruled builds pass (Baseline51 Editor and Game Development at `d7eee97`;
  ReGate51 clean Editor, Game Development and Shipping at `1c4cce7`):
  - exit 0 and 0 errors;
  - MSVC 14.38.33130 (cl 14.38.33144) and SDK 10.0.22621.0 in every one of 2,913 archived compile responses;
  - 0 actions write under either engine tree.
- **Production source is unchanged.** `489c828`, `1c4cce7` and `d30cbe0` have the same `Source` tree object (`5b4cbbf1…`) and the
  same `.uplugin` blob. All 22 changed files are tools or docs. No 4.25 re-compile or JsonExport was due.
- **R1 holds:**
  - the only warnings are the same two host C4996 lines (`MidReproActor.cpp:110/153`) on both sides; nothing new or worse;
  - the generated defines `ANOMALY_CAPTURE`, `ANOMALY_CONTROL_SERVER` and `ANOMALY_SHADERS` are 1, and the bench macros are 1 on the candidate;
  - `ANOMINJECT_FW_TICKPIN` is 0 on both.
- **Shipping excludes every IA and probe module:**
  - 0 actions, 0 BuildProducts and 0 UHT modules;
  - 0 hits for 11 IA/probe needles in the Shipping exe, in both encodings, while both Development exes hit every needle.
  - The CaptureBench fixture module does compile into Shipping (OPEN 1).
- **The strict pass is complete on both targets:**
  - 64 `.cpp`, 77 header TUs and 11 generated sources;
  - 152 responses, all C++17, with 0 PCH flags and 0 unity sources;
  - the mutant fails with C2027;
  - exact `Build.cs` restoration, and the normal rebuilds pass.
- **The EI4 clean-engine query** (Baseline51 Editor, action plan only) returned 0 actions.
- **P9 v2 is unchanged:** 18,264/18,264 by my re-hash. Engine bookkeeping is EG1-only: 1,022 files, 1.91 MB distinct (Codex's
  per-operation sum is 6.23 MB; the cap is 100 MB). The 4.25 install is equal before and after.
- **Counts: 36 OK · 0 GAP · 7 OPEN · 0 STOP · 0 CONFLICT.**

## 1. The cleanup (EI7)
Receipts: `code-receipts-094-09\cleanup\` (`cleanup-result.json`, `receipts\delete-*.json`, `junction-*.json`).

| Step | Result |
|---|---|
| Inventory first | 22:10. 432,470 files, 397,028,291,180 B, 10,509 dirs, 3 reparse points, equal to 094-05's in all 41 groups (`inventory-compare-094-05.json`). The owner had not started deleting. |
| Safety pre-checks | 0 processes used the delete set. 49,686 hard-linked inodes: 0 with a link outside the cache, 0 sharing an inode with a kept file. |
| Junctions (one at a time) | Each was checked as a directory junction (tag `0xA0000003`) with the expected readlink target, then removed with `os.rmdir` (non-recursive). Targets before = after = 094-05: StackOBot Content 2,743 files `91b3ebf3…`; RoomGenerator Content 8 `a863308c…`; BankOverflow donor 88,147 `8370ab1a…`. |
| Re-scan | 29 roots, 432,433 files, 0 reparse points, 0 errors. |
| Deletes | One root at a time, against an allow-list, each pre-scanned to refuse any reparse point. Files deleted with `os.unlink` and folders bottom-up with `os.rmdir`, using the long-path prefix. No `Remove-Item -Recurse`, no recursive shell delete. 14 `_r*` folders (399,839 files); 12 `_binary_baselines` items (11,980); the contents of `StackOBot\Builds`, `Intermediate` and `Saved` (20,614; the folders themselves kept). **0 errors.** |
| Kept | `release-2026-10-05-23098fa-218BA4C7` (8/8 SHA-256 equal); `StackOBot\Binaries` (26 files, listing hash equal); the 3 loose `.py` files; the 6 junction-target folders. The cache now holds 37 files and 0 reparse points. |
| E: free | 48.044 → 271.870 GiB. 094-05 estimated 223.2 GiB unique; the measured gain is 223.8. |
| Protected state | PASS at 22:09, 22:17 and 04:17. |
| Hook | **No delete was refused.** One later call that only wrote the receipt was blocked because its text named a recursive shell delete command. Nothing in it ran; I rewrote the receipt with the file tool. |
| Side effect | The 5 cache worktrees' git registrations are now "prunable" and still hold their branches as checked out (OPEN 4). Not pruned. |
| Receipt note | `delete-_r52p_src.json`'s `unique_inode_bytes` is invalid: `scandir` leaves `st_ino` 0 on Windows. I removed the field for the other 28 roots. |

## 2. Rulings
| Ruling | Evidence | Status |
|---|---|---|
| EI1 scope | `builds\` holds exactly the 5 pairs plus the query; job config `build_pairs` = the 5, `query_pairs` = Baseline51 Editor; no Baseline51 Shipping folder or action list. | OK |
| EI1 `-NoEngineChanges` | 25 of 25 UBT command lines in 214 job logs (`review\scope-review.json`), including the JsonExport and both refused plans. | OK |
| EI1 toolchain from responses | 2,913 archived responses (2,259 without the refused first Baseline51 Game plan), sha256-equal to Codex's records. All contain `MSVC\14.38.33130` and `10.0.22621.0`; 0 contain 14.36, 14.42, 14.16 or 17763. Every compile action's `CommandVersion` is 14.38.33144. Every log reads `Using Visual Studio 2022 14.38.33144 toolchain (…\14.38.33130) and Windows 10.0.22621.0 SDK`. | OK |
| EI1 builds pass | My log scan (`review\builds-review.json`): `[n/N]` lines reach N for every compile (29, 765, 29, 765, 706, 163, 154, 17, 8); 0 error lines; exit 0 in every result. The include-negative expected failure is exit 6. | OK |
| EI1 R1 warnings | Re-derived from the raw `UBT.log`s by code, file:line and category (`review\r1-review.json`): Editor, Game and Shipping each have exactly `C4996 <host>\Source\StackOBot\MidReproActor.cpp:110` and `:153` on both sides. New or worse in the candidate: none. 0 linker warnings. | OK |
| EI1 R1 defines | Read from the generated `Definitions.*.h` files themselves. Candidate Editor and Game (11 files each): CAPTURE, CONTROL_SERVER and SHADERS = 1, BENCH = 1, PROFILE_BENCH = 1. Baseline Editor and Game (6 each): the same three = 1 (the release predates the bench macros). Shipping: only `CAPTURE_BENCH 0`. | OK |
| EI1 tick pin | `ANOMINJECT_FW_TICKPIN 0` in every non-Shipping candidate and baseline file. Equal. | OK |
| EI1 Shipping exclusion | 0 IA/probe actions. The target receipt's BuildProducts are the exe/pdb and third-party DLLs only. The Shipping UHT manifest (132 modules) has no Anomaly, probe or CaptureBench module. No IA/probe Shipping folder under host or plugin Intermediate. Exe scan of `StackOBot-Win64-Shipping.exe` (`010ab98e…`): 0/0 for all 11 IA and probe needles; the positive controls (ReGate51 `2422f087…` and Baseline51 `4e69082a…` Development exes) hit every needle. One CaptureBench action: OPEN 1. | OK |
| EI1 strict pass | `review\strict-review.json`: git at `1c4cce7` has 77 `.h` and 64 `.cpp`; the 77-entry header-TU map covers exactly those 77. Per target: 152 archived responses, all `/std:c++17`, 0 `/Yu`, `/Yc` or `/Fp"`, 0 unity sources; all 64 `.cpp`, 77 header TUs and 11 `.gen.cpp` compiled. | OK |
| EI1 strict can-fails | Include-negative: `SIC_MissingDefine09408.cpp(3): error C2027 … 'UStaticMeshComponent'`, exit 6; include-positive compiles. Codex's synthetic PCH and unity mutants are in `strict-native-schema` (29 cases); I re-ran that suite (29 PASS). | OK |
| EI1 strict restoration | The 5 `Build.cs` files equal `buildcs_backup` by hash. ReGate51 has 0 status lines (untracked included) and no `SIC_` or `StrictIncludeCheck*` file. | OK |
| EI1 normal rebuild after | The first normal Editor plan held only 11 relink actions; Codex's zero-compile guard refused it. Recovery removed 12 cached IA objects inside ReGate51 (confined, `review\cleaning-review.json`). Then fresh normal Editor (17 actions) and Game (8) builds passed, 6 compiles each, with normal PCH and unity. | OK |
| EI2 memory gate | Job config: start 4 GiB physical and 16 GiB commit, stop at commit 6, parallelism 4. 25 admissions, each after 3 qualifying polls (40 s); 0 stops; 2,780 in-run samples. Lowest free commit during a compile 14.9 GiB; lowest free physical 0.11 GiB (OPEN 5). `-MaxParallelActions=4` on every build; UBT logs `Building N actions with 4 processes`. | OK |
| EI3 engine-tree actions | 0 ProducedItems or DeleteItems under `D:\UESource` or `E:\EpicGames` in all 14 action lists, and 0 outside the build's own host. | OK |
| EI3 engine-source-in-host count | My module-provenance classifier gives 706 compile/link actions on engine modules in each Development Game build and 668 in Shipping, equal to Codex's. Codex files a further 45 (Shipping 31) `BuildProject` ISPC header-generation actions for engine modules under "project" (OPEN 2). | OK |
| EI4 clean-engine query | `builds\clean-engine-query`: `-NoEngineChanges -WriteOutdatedActions`, query only, `actions-1.json` holds 0 actions (0 engine, 0 engine-plugin), 2.9 s. | OK |
| EI5 time limit | 5.87 h of 14; no limit stop. | OK |
| EI6 tooling scope | `Receipts\09409\job-config.json` holds roots, allow-lists, memory policy, parallelism and the 14 h boundary; `job_scope.py` refuses unruled pairs, policies and Baseline Shipping. I reproduced every suite against my own copy of that config rooted at `CodeReview09409` (lists unchanged): tooling 31, memory-gate `finish_self_tests` **80/80**, A4b 36, delta 9, budget 5, resume 16, UHT 29, idle gate PASS. Codex's counts are equal (memory-bookkeeping 80, native-short-names 36, strict-native-schema 29). | OK |
| EI7 cleanup | §1. | OK |
| EG1 bookkeeping | `review\engine-window-51.json` (22:24 → 04:17): 339,410 engine files scanned. 1,022 written: 414 + 239 `LiveCodingInfo.json` (all byte-equal to v2), 207 + 161 UHT `Timestamp`, 1 `ActionHistory.bin`. 1,913,446 B; 0 non-EG1; 0 v2 files missing or resized. Engine `Binaries\Win64` (1,608), engine-plugin Binaries (2,469), BuildRules, `Programs\UnrealBuildTool`, `Engine\Saved` and UbtRuns (17) are equal before and after. | OK |
| EG2 receipts | Every `result.json` has `binary_hashes`, `host_binary_inventory`, `actual_compile_responses` with sha256, `toolchain_lines` and `/std` and PCH evidence. | OK |
| EG5 P9 v2 | §3. | OK |
| EG5 protected state | §3. | OK |
| EF1 (amended) | No wait came near 4 h (the longest was 40 s); no in-run stop. The two refused-before-compile plans and the zero-compile refusal are not D2 attempts; their receipts are retained. | OK |
| D2 / two-attempt rule | 0 compiler-error fix attempts; 2 harness corrections (monolithic definitions location, native UHT short names) and 1 guard recovery, all in tooling. | OK |
| ED1 disk | Lowest free during the job (Codex's ledgers): C 18.23, D 84.95, E 260.6 GiB. Now C 18.43 / D 85.10 / E 260.62. | OK |
| ED2 / EH5 final-SHA re-runs | Not triggered: no production change (same `Source` tree object). All 27 A4a artifacts are unchanged (my Paris host and plugin Binaries and Intermediate listings are equal before and after). | OK |
| ED3 toolchain | Unchanged at start and end: VS 2022 17.12.35707.178, BuildTools 2017 15.9.34031.82, MSVC 14.36/14.38/14.42 + 14.16.27023, SDK 17763 + 22621, no 14.44. | OK |
| EH2 clean by receipt | `cleaning\ReGate51-before-qualification-result.json`: 171 files, 514,823,734 B, all inside ReGate51's host `Binaries`, `Intermediate\Build`, and the AnomalyInjector and CaptureBench `Binaries`/`Intermediate`. All 156 files in my hashed before-snapshot are in it, with equal hashes; the other 15 are CaptureBench outputs. | OK |
| EH6 scope | 185 child receipts: python 47, git 109, vswhere 1, UnrealBuildTool 26. 0 editor, game, commandlet, cook or UAT. | OK |
| EA8 entry/exit | Codex: 472,553 inventory paths and 23,033 full hashes, equal on each side. Mine: §3. | OK |
| EA11 | Approved checker on ReGate51 and Paris `Source`: PASS, 7 reflected, 0 violations, mutant detected. My independent checker agrees. | OK |
| EA14 / W4 commits | 5 local commits (`6fe993e`, `410acbc`, `b36ac4a`, `1c4cce7`, `d30cbe0`), author time = committer time on each; the launch receipt shows the child saw neither date variable. `feat/europa-port` is not on origin. Refs 72 → 72, with only `feat/europa-port` moved (`55764fd` → `d30cbe0`). | OK |
| Prediction addendum before UBT | Committed in `6fe993e` at 22:41:05. The first UBT call (fresh JsonExport) is at 22:44:32; the first build at 22:48:26. | OK |
| Comment invariant | 0 added comment lines in the changed `.py` files. | OK |
| Ledger row | `094-01-europa-phase-a-plan-resume7`, exit 0, GREEN, appended once by the relay. | OK |

## 3. P9 v2 and protected state
- **P9 v2:** `8e3ed095…2c74`, 472,552 lines, unchanged. My re-hash (`review\p9_rehash_v2.py`, seed 9409): 18,244 protected
  exe/dll/pak/ucas/utoc files plus 20 random v2 files = **18,264 OK**, 0 mismatches, 0 mtime differences (17.1 GB in 64 s).
- **Worktrees:** Baseline51 `d7eee97` (index unchanged), ReGate51 `1c4cce7` (detached; the qualified SHA), Paris `d30cbe0` on
  `feat/europa-port`, Lyra `9781031`. All have 0 tracked and 0 untracked changes. `m51` is at `53bf725` with 0 tracked changes.
- **Paris original** `E:\Unreal Projects\StylizedParisStreet`: hashed inventory, 1,512 lines, equal.
- **Protected state** (`protect\protect-{start,after-cleanup,after-exit}.txt`), all PASS:
  - the release baseline 8/8;
  - the six D: junctions resolve;
  - StackOBot Content byte-identical to its 094-05 snapshot (2,744 lines);
  - `D:\IA_BankOverflow` 0 missing and 0 changed against v1 (the one extra file is the pre-v1 `09102-final-binaries.zip`);
  - `E:\IA_BuildCache\StackOBot\Binaries` equal to the 094-08 after-snapshot.
- **`D:\IA_Europa` changes** were only in the two hosts' build outputs, `Receipts\09409`, and the Paris worktree's 20 tools/docs
  files; nothing disappeared. **C: root:** only `ClaudeTemp` (my TEMP) and `renders` (owner, 22:11, before launch) moved.
  ProgramData\Epic, LocalAppData UE/UBT/UHT, UE TEMP entries, E: root, the Lyra binaries and the 4.25 install are equal.

## 4. OPEN items
1. **The CaptureBench fixture compiles into the candidate Shipping build.** It is one action (`Module.CaptureBench.cpp`) with
   `CAPTURE_BENCH 0`, and one UTF-16 "CaptureBench" string in the Shipping exe. The target receipt also stages
   `CaptureBench.uplugin` and `AnomalyInjector.uplugin` as RuntimeDependencies. It is not an IA or probe module, so it is not a
   STOP. Baseline51 Shipping was not built, so it is unknown whether the release did the same. A6/A11a must prove packaged
   exclusion and remove the fixture before any delivery.
2. **EI3's count is definitional.** Codex reports 706/706/668 engine-source actions in host and puts 45/45/31 ISPC
   `BuildProject` actions on engine modules under "project", although their module rules are engine. By module provenance the
   count is 751/751/699. Both write only inside the host. Chat to say which definition the ledger should use.
3. **5.1 runtime parity and A5–A11 are unproven** (carried). Builds certify neither label/picture sync nor 5.1 equivalence.
4. **Five stale git worktree registrations after EI7** (`.git\worktrees\AnomalyInjector1/2/3/4/7`). They still hold
   `feat/m52-proxy-blur`, `feat/m53-uv-normal-corruption`, `release/2026-10-05` and `fix/followup-capture` as checked out, so
   `git worktree add` or `checkout` of those branches elsewhere fails until `git worktree prune`. No refs changed. Not pruned.
5. **Physical memory reached 0.11 GiB free** during the Baseline51 Game build, and 0.48 GiB in the candidate Game build and
   strict pass, while free commit stayed at 14.9 GiB or more. EI2 allows it (paging, no crash), but it is the margin A6's
   editor launches will start from.
6. **UBT makefile environment carry-over.** Builds that reuse a cached makefile export the environment captured when that
   makefile was created, and UBT probably restores it for its children:
   - Baseline51 Editor and the clean-engine query show TEMP `D:\IA_Europa\Temp\09407\11-baseline-query`;
   - the cached environment also stores a local Claude Code messaging token in those `actions-1.json` receipts.
   TEMP still resolved to D:, and no files changed under `D:\IA_Europa\Temp`, so the budgets are unaffected. But Codex's
   per-child "environment redirect" receipts describe the wrapper's environment, not necessarily the compiler's.
7. **My one write outside `CodeReview09409`:** the approved tooling's child routing created two empty folders,
   `D:\IA_Europa\Temp\CodeReview09409\op` and `\resume`, during my self-test re-runs (its `temp_root` must lie under
   `D:\IA_Europa\Temp`). Left in place, since I may not delete.

## 5. Build manifests for A9–A11 (re-hashed by me)
- `qualification\final-baseline51-binaries.json`: 39 of 39 size and sha256 equal (StackOBot.exe `4e69082a…`).
- `qualification\final-regate51-binaries.json`: 44 of 44 equal (StackOBot.exe `2422f087…`, StackOBot-Win64-Shipping.exe
  `010ab98e…`, UnrealEditor-AnomalyInjector.dll `88f288ee…`).
- The 4.25 A6–A8 manifest is still A4a's 27 files, unchanged.

## 6. Top 5 risks for A5 and A6
1. **A5 puts the first runtime code into the probe.** It is still the empty A3 skeleton. Its readers and fixtures must keep the
   Shipping exclusion (descriptor blacklist plus module rules) and pass strict, EA11 and the descriptor matrices on both engines.
   Re-run the Shipping exe scan after A5.
2. **A6's first 4.25 editor launch** (Paris, the 27-file manifest) is the first time the port's modules load at runtime:
   - LoadingPhase ordering (the PostConfigInit shader module);
   - global-shader source mapping on 4.25;
   - the cost of a first shader compile (ShaderCompileWorker processes);
   - all of it with physical memory already near zero under compile load (OPEN 5) and the owner's Blender and other jobs
     resident. An editor plus SCW is the commit-heavy mix EI2 was written for, and it needs its own gate.
3. **Editor launches write where no ruling covers yet:** engine `Saved`, DerivedDataCache, shader caches and the 4.25 install's
   `Engine\Saved`. EG1 covers build bookkeeping only. A6 needs a ruled class and a clean-engine proof for editor writes before
   the first launch, or P9/ED4 will STOP it.
4. **Packaged content.** Build-level exclusion is proven, but the Shipping receipt stages both plugin descriptors and the
   CaptureBench module is compiled in (OPEN 1). A6/A11a must inspect the packaged pak and the plugin enablement on both
   engines, not the build tree.
5. **Provenance drift.** Any rebuild (including an incremental one after A5) changes binary hashes (G201) and may reuse cached
   makefiles with old environments (OPEN 6). A6–A8 must bind to the 27-file manifest and A9–A11 to the 39/44-file manifests,
   or re-qualify. The stale worktree registrations (OPEN 4) will block any step that checks out `release/2026-10-05` or
   `fix/followup-capture` in a new worktree.

VERDICT: REVIEW DONE — 36 OK / 0 GAP / 7 OPEN (CaptureBench compiles into Shipping and both descriptors stage; EI3 engine-source count definitional, 45/31 ISPC actions filed as project; runtime parity A5–A11 unproven; 5 stale worktree registrations after EI7; physical memory 0.11 GiB min during compiles; UBT makefile environment carry-over in receipts; 2 empty Temp folders from my self-test re-runs) / 0 STOP / 0 CONFLICT; EI7 cleanup done (432,433 files, E: 48.04 → 271.87 GiB, junction targets intact, protected state PASS ×3); A4b complete on 1c4cce7 with production source equal to 489c828 (5 builds exit 0, 0 errors, 14.38.33130/22621 in 2,913 responses, 0 engine-tree actions, 706/706/668 engine-source-in-host reproduced); R1 warnings 2×C4996 on both sides, defines 1, tick pin 0=0; Shipping 0 IA/probe by actions, receipt, UHT and exe strings; strict 64+77+11 per target, PCH/unity off, C2027 mutant, exact restoration; EI4 0 actions; EG1 1.91 MB; P9 v2 18,264/18,264; self-tests reproduced (80/80 memory gate)
