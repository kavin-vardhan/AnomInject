# 094-20 — Claude Code review of the disk-pause / probe-fix / rebuild job (W3: written after Codex exited)

**Date:** 2026-10-28, 00:47–01:15 IST · **Author:** Claude Code (mailbox brief 094-20) · **Inputs:** Codex's report `_reviews\094-20-codex-report.md` (thread `01a14334`, resume 2026-10-27 15:37:13 – 2026-10-28 00:46:32 IST, 32,959 s, exit 0, 536 items, 191 commands; ledger row `094-01-europa-phase-a-plan-resume18` once), chat's `094-19-ruling.md` (EU1–EU6), `094-20-request.md` and the rulings they bind. My receipts: `code-receipts-094-20\` (and `step0\`, `protect\`, `ek1\`, `review\`), `D:\IA_Europa\Receipts\CodeReview09420\`, `D:\IA_Europa\Temp\CodeReview09420\`. No account identifier is reproduced.

## Summary
- **Codex ended NEEDS-DECISION at its 9-hour limit. What it delivered:**
  - **EU2 is built and proven**, committed at `1722348` on `feat/europa-port` (local, unpushed): `tools/europa/disk_pause.py`, `disk_recovery.py`, and a 4-line checkpoint hook in `write_ledger.floors()`.
  - **The probe fix is built and statically proven** in the same commit. It is identical in all three hosts and **uncompiled**.
- **What it did not deliver: 0 of 6 rebuilds, no EI4, no re-binding, no EU5, no full EK1.**
  - **The rebuilds never started because of the machine, not the code.** EI2's commit gate (≥ 16 GiB free on three polls) never passed in about 2.5 h of waiting; the only two build admissions were interrupted by foreign D: dips before UBT was created (the UBT child receipts carry no PID). Separately, D: dipped below its 50 GiB floor 17 times, about 2.6 h of EU2 pauses in total. **No compile error occurred and no compile D2 attempt was spent.**
- **No STOP fired and I found no stop violation.**
  - P9 v2: 18,264/18,264, file `8E3ED095…` unchanged.
  - `m51` = P9 at start, at both pre-launch gates and after exit.
  - Protected state PASS at start and after exit.
  - Bound manifests unchanged: 27/27, 39/39, 44/44 (`4beccca6…`, `617a9082…`, `f6e52a85…`).
  - Engine trees unchanged.
  - Content: Paris 1,453 and plugin 5 files equal.
  - 0 launches.
  - Canary: 0 value hits in 567 files new or changed since the launch second.
  - My EU1 audit agrees with Codex's: 4 direct read-only, 187 filtered, 0 direct write-capable.
- **Codex discloses one EU2 procedural violation (my G1).** At 17:42:12 IST it edited a run-folder helper, `commit_probe09420.py`, through the file-edit API while pause 05 (detected 17:40:30, resumed 17:46:54) was active. EU2 says only the pause receipt is written while paused. The edit touched no host source, binary, content, engine file or setting.
- **EU5, done by me read-only as input for chat (not Codex's deliverable):** the only multi-block (UDIM) texture source among 698 Paris textures is **`Content\ParisStreet\Textures\T_Vetro02_N.uasset`**.
  - The owner-leg log corroborates it: `Building textures: T_Vetro02_N (BC5, 2048X2048)` at line 5157, then the `TextureData.Blocks.Num() == 1` ensure at line 5162, in the same flush.
  - **It is referenced by `MM_Glass` and six glass instances.**

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | EU1 read-only direct = deviation; command audit | My audit (`review\eu1_audit20.py` over `events.jsonl` from the launch offset): 191 commands. Items 1–4 (15:37–15:44, before `filtered09420.ps1` existed) are direct and read-only (`Get-Content`, `Get-Date`, `rg --files`). The other 187 go through `filtered09420.ps1`. **0 direct write-capable.** This equals Codex's terminal audit (4 + 187). The wrapper checks the canary's hash against `canary.txt`, removes every blocked name from its own process, sets `GIT_OPTIONAL_LOCKS=0`, then starts the child. The 40 file-edit API items are outside a command audit (see G1). | OK |
| 2 | EU2 foreign dip pauses | Diff read. **My can-fails**, driving the real `supervise()` with a scripted D: reading, a stubbed footprint, a fake clock for the 4 h case and harmless python sleepers as the "build" tree, all under `Temp\CodeReview09420\eu2\`: **A** foreign drop → PAUSE → three clear polls → `RESTART_REQUIRED` (75) with the pending flag written · **B** port growth 25 GiB → STOP (2) · **C** no recovery → TIMEOUT "no disk window" at 14,400 s (76) · **D** a drop during the "build" stopped exactly the child and grandchild while my control process lived, and a sub-warning reading reset the count (54 → 56 ×3: consecutive 0, 1, 2, 3, RESUMED at 180 s). **Mutant** (port growth zeroed): B then PAUSEs → detected. Codex's self-test re-run 12/12. **Real pauses:** all 17 judged foreign, with the port-growth upper bound on D: ≤ 138.6 MiB against ~20–30 GiB dips; my 1-minute series shows D: < 50 GiB inside 13 of the 17 windows (often to the third decimal of Codex's reading); the other 4 dips were shorter than my sampling (window minima 51.8–61.6). One pause ended TIMEOUT at the 9-hour limit. D2: attempt 1 failed (missed a dip during the baseline inventory; Codex stopped its own tree by PID), attempt 2 passed. | OK (built and proven) · **GAP G1** (pause-time write) |
| 3 | EU3 launcher design | For the arming job; not built here. The launcher is still the old instance (pid 26116, from 10:38) | N/A (carried) |
| 4 | EU4 uploads switch | For the arming job; no editor run | N/A (carried) |
| 5 | EU5 texture identification | **Codex: NOT STARTED.** Mine, read-only (`review\eu5_header_scan20.py`, package-summary and name-map parse, ue4 version 518): 883 packages, 698 texture-like, 0 errors. **Only `T_Vetro02_N.uasset` (1,836,172 B, sha `8677850B63FA…`) carries `TextureSourceBlock`/`Blocks`/`BlockX`/`BlockY`** (`T_Vetro02_B` and the other 696 do not). Log corroboration as in the summary. Referencers (`review\eu5-referencers.json`): `MM_Glass`, `MI_Vetro01`, `MI_Vetro02`, `MI_VetroSX02`, `MI_VetroSX03`, `MI_TabacchiGlass`, `MI_Piano06`. | **GAP G2** (Codex) · my finding as input |
| 6 | EU6 order | EU2 → fix → static check done, in order. Rebuilds, EI4, re-bind and EU5 not reached; the EU1 audit was done | OPEN (incomplete) |
| 7 | ET1 A6 owner half | Not re-run | OK |
| 8 | ET2 `EditorRuns` marker | `31124`, 0 B, 18:43:59.9006068Z, empty-file hash, the only entry: identical at start and after exit. 4.25 full listing equal before/after | OK |
| 9 | ET3 launcher writes foreign; ED1 applies | `ProgramData\Epic` changes are launcher/EOS (foreign) | OK |
| 10 | ET4 | No runtime leg | N/A |
| 11 | ET5 fix incl. 5.1 GC-time lookup | Both `FindObject`s moved to `CacheClasses()`, which only `Initialized()` calls, behind engine-exit/`GExitPurge`/GC guards, with weak pointers. `TActorIterator` replaced by `GetLevels()`/`Level->Actors` + `IsInspectable`. `Cleanup` records `actor_snapshot: skipped` + `skip_reason`. **OPEN O1:** `Stop()` still calls `RemoveFromRoot()` on raw `Dormant` and GC-field pointers after the exit purge (§3) | OK (static) · OPEN O1 |
| 12 | ET6 → EU5 | See row 5 | GAP G2 |
| 13 | ET7 owner-slot temp files | Codex read 1 byte using `FileShare.ReadWrite\|Delete` (my earlier reads failed only because they omitted `Delete`). **Mine, in memory, nothing copied** (`review\et7-owner-slot-scan.json`): `761b7485…tmp` 670,796 B, `0026e58f…tmp` 0 B, `ecache.bin` 90,897 B. 0 secret-name tokens, 0 live-value hits, 0 canary strings | OK (closed) |
| 14 | ET7 carries (O3, ER1 G3, ER3, 8.3/junction, EO1 budget, A9, EJ1/3/5) | Untouched | OPEN (carried) |
| 15 | ES1 / ES3 / ES5 | Not re-run; kit `4b5b2812…`/`5d1d1ab5…` unchanged; not run | OK |
| 16 | ES2 one source, `ENGINE_MAJOR_VERSION`, no production edit | 5/5 files SHA-equal across Paris, Baseline51 and ReGate51. The 3 changed files equal their committed templates byte for byte; the 2 unchanged ones differ from their templates only in CRLF, as before. `ENGINE_MAJOR_VERSION` in `IsInspectable`, `LifetimeProbe` and `Snapshot`. `git diff b20e66e 1722348 -- Source Content *.uplugin *.Build.cs *.uproject` is empty | OK |
| 17 | ES2/ET5 static check + mutants | Re-run on all 3 hosts + templates: PASS, 10/10 self-tests each (real + 9 mutants). **My mutants:** `TActorRange` in `Stop()`, `StaticFindObject` in `Cleaned()`, `FindObject` in `ReleaseGC()`, `TActorIterator` in `EndFrame()`, guard removed from `SnapshotState` — **5/5 detected**. **Not detected:** `LoadClass`, `StaticLoadClass`, `TObjectIterator<AActor>`, `FindFirstObject` in `Cleanup` (none occurs in the probe today) | OK · **GAP G3** (coverage) |
| 18 | ES4 six rebuilds, EI4, re-bind | 0/6. EI2 never admitted; the two 4.25 Editor admissions were interrupted before UBT creation. Manifests deliberately left bound to `550e9b7` binaries | OPEN |
| 19 | ER1–ER4 | ER1 G3 and ER3 carried; ER2/ER4 not exercised | OPEN (carried) / N/A |
| 20 | EO1 / EO2 / EO6 | No reader, no build | N/A |
| 21 | EO3 makefile refresh | Codex removed only the target-owned 4.25 Editor `Makefile.bin` by receipt: 713,387 B, `3f0fb3a2…`, equal to my before-snapshot. It is now absent; the other 5 makefiles are unchanged | OK |
| 22 | EO4 one probe source | Row 16 | OK |
| 23 | EO5 closeout | EA8 entry and exit PASS (Codex). Ledger closed; commit with W4 dates unforced (author = committer time). Terminal canary refresh blocked by the floor guard and not bypassed; my own search covers it (row 31). Terminal report handling ran 00:37–00:46, 9 min past the limit, with no implementation work | OK (with note) |
| 24 | EN1 / EN2 / EN3 | No makefile regenerated; nothing to prove | N/A |
| 25 | EN4 my writes | Only my allowed places (§6). Disclosure: 4 read-only scratch copies under `Temp\CodeReview09420\src-1722348\` were written at 20:27 while the job ran | OK |
| 26 | EN5 hard STOPs | None fired; no exception claimed | OK |
| 27 | EM2 `CLAUDECODE` | Absent from all 47 recorded child environments. Present only in the shells of the 4 direct read-only commands (EU1 class) | OK |
| 28 | EL1 denylist at every boundary | 49 child receipts: 47 PASS (29 python, 16 git, 2 UBT never created) with 0 tool/`CLAUDECODE`, 0 secret-named and no canary name. The 2 others are a failure receipt and a memory-budget receipt, not children | OK |
| 29 | EL2 canary | Launch positive control PASS (child python sha = generated). Search by value from the launch second (`review\canary-search-postjob.json`, 14 roots): 567 files, **0 value hits, 0 other canary-shaped strings**, 5 name-only hits | OK |
| 30 | EL3 / EL4 / EL5 | — / kit not run / wrapper removed only `CLAUDE_CODE_MESSAGING_TOKEN` | N/A / OK / OK |
| 31 | EK1 secrets | Codex's full scan INCOMPLETE (interrupted by a dip). Mine: pre-scan 14:41–15:19, 41,320 files, 0 values, 0 live matches. Post-scan of files changed since the launch second: 462 files, 0 values, 0 live matches; the 3 hit files are names only (`AUTHORITY`, an engine header guard, names inside `events.jsonl`, my launch receipt's empty `GIT_AUTHOR_DATE=[]`). Window 15:19–15:37 holds only my own pre-launch receipts | OK (mine) · **GAP G4** (Codex's) |
| 32 | EK2 / EK3 | No makefile / `GIT_OPTIONAL_LOCKS=0` set by the wrapper; m51 and Paris indexes as expected | N/A / OK |
| 33 | EI2 memory gate | Held correctly. Commit free 1.2–17.2 GiB. Largest holders at 19:17 with no Blender running: `wlanext.exe` 8.96 GB (8.47 → 9.03 GB over the day), WSL `vmmem` 4.0 GB, the launcher 2.3 GB; at 20:07 also another Claude session's `asr.py` at 9.57 GB. All foreign | OK |
| 34 | EI3 / EI4 | No build; no EI4 query | N/A / OPEN |
| 35 | EG1 engine trees | 4.25 full install, 5.1 `Intermediate\Build`, Binaries, BuildRules, Programs\UBT, UbtRuns, Saved, plugin trees: all equal before/after | OK |
| 36 | EG2 build receipts | No build | N/A |
| 37 | P9 v2 / protected / m51 | P9 v2 re-hash 18,264 OK, 0 mtime diffs; m51 PASS ×5 (start, dry run, launch, after exit, final); protected PASS ×3 | OK |
| 38 | Launches | 0 launch-class processes of ours across 66 polls; 0 launch/kill/delete/git-write words in 191 commands; 4.25 and 5.1 trees unchanged | OK |
| 39 | Commits / push / tags | `1722348` local only; no remote `feat/europa-port` ref; ReGate51 checkout moved `550e9b7 → 1722348` (clean; `550e9b7..1722348` is docs/tools only); no tags | OK |
| 40 | Self-test suites | §5 | OK |

**Count (primary status per row, 40 rows): 25 OK · 9 N/A · 2 GAP · 4 OPEN · 0 STOP · 0 stop violations.** GAPs overall: G1–G4 (G1, G3 and G4 sit on OK rows). OPEN: O1 plus the carried items.

## 2. EU2 in practice
- **17 pauses, about 9,345 s (2.6 h) in total.** Detected D: readings were 44.07–44.47 GiB every time. 16 resumed; the last timed out at the 9-hour limit.
- **Each interruption restarts an inventory-heavy operation from scratch.** Combined with roughly 2.5 h of EI2 waiting, that consumed the window.
- **The writer behind the dips is not established (G120).**
  - D: free + page-file allocation stays at about 116.9 GiB for most of the series, but **only 10 of 124 readings below 55 GiB are explained by page-file growth**; 111 are not.
  - The minima repeat at an identical ~44.3 GiB, i.e. a ~22 GiB allocation that comes and goes.
  - Dips occurred with and without an owner Blender running.
  - None is ours: the port's growth bound was ≤ 139 MiB, and my own and Codex's writes are small.
  - Data: `code-receipts-094-20\space-series.txt` and `review\pause-vs-series.json`.

## 3. The fix: remaining teardown path (O1)
- **The exit order, in source, both engines:**
  - `FEngineLoop::Exit()` → `AppPreExit()` → `FCoreDelegates::OnExit.Broadcast()` → `StaticExit()`. This marks every non-`UField` object unreachable, sets `GExitPurge`, and purges rooted objects too.
    - 4.25: `LaunchEngineLoop.cpp:4218, 5716`; `Obj.cpp:4340, 4452–4466, 4476–4483`.
    - 5.1: `LaunchEngineLoop.cpp:4592, 6286`; `Obj.cpp:4744, 4867, 4877`.
  - **Only after that** does `UnloadModulesAtShutdown()` call our `ShutdownModule()` (4.25 `:4249`, 5.1 `:4650`).
- **Our side:** `ShutdownModule()` → `Probe.Stop()`, which calls `ReleaseGC()` (`Field.Original->RemoveFromRoot()`) and `RemoveFromRoot()` on every raw `Dormant` pointer (`EuropaPhaseAWorldProbe.cpp:157–159`; `EuropaPhaseAProbeDriver.cpp:374–380`).
- **Effect:** safe only while both arrays are empty at exit. An exit inside the 3-frame dormant window or a GC probe would touch freed objects.
- **Scope:** not one of ET5's two named families, so the static check does not look for it.

## 4. EU5 detail
- **Scanner:** it parses each package's summary (legacy −7, file version 518, custom versions, `TotalHeaderSize`, name map), reads only the header bytes, and looks for the name-map entries the 4.25 `FTextureSource` serializer adds when `Blocks` is non-empty.
- **The discriminator discriminates:** 1 of 698 textures carries those names.
- **Engine behaviour:** `TextureDerivedDataTask.cpp:252` ensures and `:266` builds `Blocks[0]` only (094-19 ruling EU5). So every surface drawn with these seven glass materials may show the first tile only.
- **Practical note:** `MM_Glass` is the master, so any other instance may inherit the texture as a default parameter, and shop-window glass is likely common in a street scene.

## 5. Self-tests (re-run by me)
- `owner_eq_self_tests.py`: **159/159** (11 harmless children, 0 UE launches).
- `owner_kit_self_tests.py`: **62/62 + 48 measured**.
- `idle_gate.py --self-test`: **48/48**.
  - These three ran on copies changing only the fixture TEMP base, under `CodeReview09420`.
- `disk_pause.py --self-test`: **12/12**.
- `probe_teardown_check.py --self-test`: **10/10 ×4** (three hosts + templates).
- EA11 `check_reflected_headers.py --self-test`: Paris **PASS**, ReGate51 **PASS**; release control Baseline51 `d7eee97` **FAIL, 12 violations** (expected pre-port; a real-data can-fail).
- My own:
  - EU2 end-to-end can-fails A–D + mutant;
  - static mutants 5/5 on the ruled families;
  - EU5 header scan;
  - canary search (567 files);
  - EK1 pre-scan and post-scan;
  - m51 gate.

## 6. Writes and snapshots
- **Mine:**
  - `code-receipts-094-20\`;
  - `resume-prompt-094-20.txt`;
  - the preserved `094-19-*` copies (hash-equal to `code-receipts-094-19\`);
  - `_reviews\094-20-codex-report.md` (body byte-identical, `1513754B…`);
  - this file.
  - In `D:\IA_Europa` only `Receipts\CodeReview09420\` (static, EA11 and disk self-test outputs, self-test roots) and `Temp\CodeReview09420\` (pre-fix probe copies, 1722348 scratch copies, `eu2\` harness roots).
  - **The only processes stopped were my harness's own harmless python sleepers,** stopped by the code under test as the can-fail requires. I killed nothing else, deleted nothing, launched nothing and ran no build.
  - **I held the launch from 15:20 to 15:37** until D: read ≥ 70 GiB on two polls (`prelaunch-headroom.txt`).
- **Before → after (61 listings): 46 equal.**
  - **Expected differences:**
    - refs / worktree porcelain / heads (`feat/europa-port` and ReGate51 at `1722348`);
    - the probe source in the 3 hosts (Paris `Build.cs`/`Module.cpp` mtime only, bytes equal);
    - 1 Paris Intermediate file removed (the EO3 makefile);
    - `D:\IA_Europa` +323 / −1: 293 Codex `Receipts\09420`, 18 ReGate51 checkout, 4 Paris new tools, 8 mine;
    - free space; processes; drive-root mtimes; `ProgramData\Epic` (launcher, foreign); tooling sizes.
- **Still running (owner's):** the Epic Games Launcher (26116, the old instance) with its helpers. The old kit console is gone (closed by the owner).

## 7. GAPs and OPENs
- **G1 (EU2):** one file-edit-API write (`commit_probe09420.py`, run folder) during pause 05. Disclosed by Codex, no EN5 exception claimed. Chat's call: accept as a recorded deviation, and whether the next job's tooling should block edits while a pause receipt is open.
- **G2 (EU5):** not done by Codex. My identification (`T_Vetro02_N`, 7 glass materials) is input, not its deliverable.
- **G3 (static check coverage):** `LoadClass`, `StaticLoadClass`, `TObjectIterator<AActor>` and `FindFirstObject` pass the check. Absent from the probe today.
- **G4 (EK1):** Codex's full scan is incomplete. My pre-scan and post-scan show 0 values.
- **O1:** `Stop()` raw `RemoveFromRoot()` after the exit purge (§3).
- **Carried:** ES4/EI4/re-bind/EN1 (the next job), ER1 G3, ER3 bare UBT, 8.3/junction tests, EO1 reader budget, A9, EJ1/EJ3/EJ5, ET7's O3 package-output absence check, EU3, EU4.

## 8. Top 5 risks for the arming job and the next runtime leg
1. **The machine, not the code, decides whether the six rebuilds can run.** D: dips (~22 GiB, unexplained, 17 in 9 h) and commit pressure (EI2 never passed; `wlanext.exe` alone holds ~9 GB and grows) together ate this job's window. A rebuild job started under the same conditions will likely end the same way.
2. **Every host binary still carries the exit crash.** The fix is uncompiled, so no runtime or A7 leg may run until all six rebuilds pass with EI4 and the re-bound manifests. And O1 remains a narrow exit-time use-after-free in both engines.
3. **EU5's remedy may not be "out of frame".** `T_Vetro02_N` sits under `MM_Glass` and six glass instances; a street shot without shop-window glass may be hard to frame. A7 needs an explicit check of the visible materials, or a different remedy.
4. **The static check can pass a new teardown loader** (G3). A later edit could re-open the crash class with every gate green.
5. **Arming prerequisites are unmet.** The launcher is still the old editor-spawned instance (EU3 needs a clean, already-running launcher); EU4's source proof for automated legs is not done; and the Paris Editor makefile is now absent, so the next build regenerates it and must re-prove EN1.

VERDICT: REVIEW DONE — 25 OK / 9 N/A / 2 GAP rows, 4 GAPs overall (G1 pause-time edit, G2 EU5 not done by Codex, G3 static-check coverage, G4 Codex EK1 incomplete) / 4 OPEN rows (plus O1 Stop() RemoveFromRoot after exit purge) / 0 STOP / 0 stop violations. EU2 built and proven (Codex 12/12; my A–D + mutant). Fix static-proven (10/10 ×4; my 5/5 ruled mutants; 4 unnamed-family mutants survive). 0/6 rebuilds (EI2 never admitted; 17 foreign D: pauses, writer not established). Manifests 27/39/44 unchanged. EU5 by me: T_Vetro02_N (7 glass materials). EU1 audit 4 read-only / 187 filtered / 0 write-capable. Canary 0 in 567 files. P9 v2 18,264/18,264. m51 = P9. 0 launches.
