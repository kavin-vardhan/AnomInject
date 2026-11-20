# 094-18 — Claude Code review of the A6 owner-leg evidence job (W3: written after Codex exited)

**Date:** 2026-10-27, 10:14–12:05 IST · **Author:** Claude Code (mailbox brief 094-18) · **Inputs:** Codex's report `_reviews\094-18-codex-report.md` (thread `01a14334`, resume 10:45:59–11:38:32 IST, 3,153 s, exit 0, 95 items, ledger row `094-01-europa-phase-a-plan-resume16` once), chat's `094-17-owner-leg-ruling.md` (ES1–ES5), `094-15-ruling.md` EP3 and the rulings they bind. My receipts: `code-receipts-094-18\` (and `review\`, `ek1\`, `protect\`), `D:\IA_Europa\Receipts\CodeReview09418\`, `D:\IA_Europa\Temp\CodeReview09418\`. Account identifiers seen in the crash files are not reproduced anywhere.

## Summary
- **Codex stopped with NEEDS-DECISION before the probe fix and the rebuilds, and the STOP is correctly raised.** The owner's editor left `E:\EpicGames\UE_4.25\Engine\Intermediate\EditorRuns\31124` (0 B, created 2026-10-26 18:43:59.900 UTC, SHA-256 `E3B0C442…` = empty). EA4 (094-01-ruling, line 115: on E: only `UE_4.25\Engine\Intermediate\Build\BuildRules\`; "a write anywhere else on C: or E: is a STOP") and EN5's hard-STOP list ("engine-tree writes outside EG1"; EG1 is 5.1-only) leave no room for the intent clause. The kit's own ledger already raised it at 10:09:51 (`EB1/ED4/ED5 ledger STOP`). Origin, verified: stock 4.25 `HotReload.cpp:389–417` creates `EngineIntermediateDir/EditorRuns/<pid>` at module startup (`:516`) and `:422–434` deletes it at shutdown (`:543`); the crash skipped the delete. `-NoHotReload` did not stop the module from loading.
- **No Paris or plugin content changed** (my hashes before and after the job): Paris `Content` 1,453 files / 2,960,507,507 B equal in path, size and mtime to my 094-17 after-review listing and SHA-256-equal 1,453/1,453 to the E: original; plugin `Content` 5 files equal and git-blob-equal to `b20e66e`. The only Paris host change outside Intermediate/Saved is `Config\DefaultEditor.ini` re-written at 10:07:04 with identical bytes (mtime only).
- **ES3, established by me read-only (Codex left it "not determined"): two crash reports WERE uploaded to Epic, and there was no restart.** An engine ensure (`TextureDerivedDataTask.cpp:252`, `TextureData.Blocks.Num() == 1`) was reported automatically at 00:14:44–46 IST (`bSucceeded=1`), with no owner action; the exit crash was uploaded at 10:07:40–41 IST (`All uploads done`: CrashContext, ini, the 721,693 B editor log, a 1,171,291 B minidump). Crash folders are on D: (`StylizedParisStreet\Saved\Crashes\…_0000`, `_0001`); outside D: the crash reporter wrote 11 tiny `%LOCALAPPDATA%\CrashReportClient\Saved\Config\WindowsNoEditor\*.ini` (58 B) and `%LOCALAPPDATA%\UnrealEngine\4.25\Saved\crash-reports\pending-reports.json` (52 B), both EA4 exception classes. No WER or CrashDumps entry.
- **Root cause confirmed in source** (§4): `OnWorldCleanup → Cleanup():157 → Snapshot():281 → SnapshotState():279 TActorIterator<AActor> / :282 It->Tags`; 4.25 `EngineUtils.h:248` and 5.1 `EngineUtils.h:250` both `checkf(!CurrentActor->IsUnreachable())`. A second, 5.1-only teardown hazard exists in the same path (OPEN O2).
- **Probe evidence survived up to the crash** (§3): 1,841,488 rows, strictly increasing, 0 torn lines; the A6 owner-leg EditorPreview half is met; only the exit-time rows are lost.
- **Protected state:** P9 v2 18,264/18,264 binaries + 20 sampled files re-hashed, file `8E3ED095…` unchanged; protected state PASS ×3; `m51` = P9 at start, launch, after exit and final; manifests 27/39/44 unchanged; canary 0 value hits in 168 files; 0 launches.
- **Two GAPs need chat (§6):** the editor-spawned Epic Games Launcher's C: writes (+933 MB under `C:\ProgramData\Epic\EpicGamesLauncher`) are classed "foreign" by the kit ledger; and automatic ensure-report upload is not covered by the card's "Close Without Sending".

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | ES1 leg done; re-run only if evidence lost | Probe stream, Runtime.log, CrashClient.log, ledger, idle log all present and parse; lost rows are only the exit-time ones (§3) | OK |
| 2 | ES2 root cause | Source lines read in all three pre-fix copies (identical, `probe-source-before.tsv`) and both engines (§4) | OK |
| 3 | ES2 fix, static check + mutant, one source | Not started (Codex hard STOP before step 3); probe source byte-unchanged in all 3 hosts | N/A (blocked) |
| 4 | ES2 other paths (5.1, PIE end) | 5.1 has the same iterator assert and an extra GC-time `FindObject` (O2); PIE-end cleanup runs before GC (not proven by a run) | OPEN |
| 5 | ES3 upload / restart / processes / files outside D: | Uploads YES ×2, restart NO, 0 editor/CRC processes, C: files listed (Summary). Codex: "not determined" | GAP G1 (Codex) · facts OK |
| 6 | ES4 rebuilds, EI4, re-bind | Not run; 27/27, 39/39, 44/44 re-hashed at start, after exit and final | N/A (blocked) |
| 7 | ES5 no re-arm | `.bat` `4b5b2812…`, config `5d1d1ab5…` unchanged; slot 01 only; `launched.json` present; slots 02/03 and `refused-attempts` absent | OK |
| 8 | EP3 P9 v2, protected state | P9 v2 18,264/18,264 + 20 random, 0 mismatches; protected PASS start/after-exit/final; m51 PASS ×4 | OK |
| 9 | EP3 engine trees (EG1 + runtime classes) | 4.25 install: +1 file only (`EditorRuns\31124`) between 094-17 after-review and now; 5.1 engine listings equal before/after the job | STOP (inherited, correct) |
| 10 | EP3 C:/E: charges (EA4) | Kit ledger: C charged 25,172,775 B, E 0; but 258+149 Epic launcher/EOS files classed foreign although the launcher is a child of the editor | GAP G2 |
| 11 | EP3 content admission vs prediction | Mount line only; 0 LogLinker/load/save/thumbnail lines for the five packages; bytes unchanged; rejection is silent in 4.25 so only absence is observable | OK (absence-only, O3) |
| 12 | EP3 no Paris content changed | 1,453/1,453 + 5/5 equal, before and after the job | OK |
| 13 | EP3 logs: prompts, warnings, errors | 0 message dialogs, 0 rebuild prompts; 3 Warning lines (MagicLeap, MapCheck 0/0, exit); errors = the ensure + the assert | OK (O5 ensure) |
| 14 | EP3 DDC and shader routing | `-DDC=Europa425DDC`; Local DDC `D:/IA_Europa/Caches/425/Local: Writable`; Boot.ddc 84 MB on D:; ShaderWorkingDir cleaned and used on D:; shaders 2,715→6,194 left, done within the first hour; 23 lines at the preview | OK |
| 15 | ER1 G3 | The kit ended through the generic unexpected-error path (ledger STOP → RuntimeError); fix still owed | OPEN (carried) |
| 16 | ER2 | Not exercised | N/A |
| 17 | ER3 bare UBT gap | New evidence: the editor itself started `UnrealBuildTool.exe` twice at startup (00:07:44, 00:07:51) | OPEN (carried) |
| 18 | ER4 owner leg | Ran once, slot 01 | OK |
| 19 | EQ1 owner gate | 23 readings, 16 GPU waits, CLEAR at 644 s after 3 passes, 0 reading errors | OK |
| 20 | EQ2 launched mark | `launched.json` at launch (18:37:38.54Z), kit locked | OK |
| 21 | EQ3/EQ5 | Not exercised / chat's card | N/A |
| 22 | EQ4 armed kit | 09417 config used, citation EP1 + EQ4 | OK |
| 23 | EO1–EO3, EO6 | No reader, build or makefile refresh | N/A |
| 24 | EO4 one probe source | Pre-fix source identical in 3 hosts (5 files) | OK |
| 25 | EO5 closeout | Codex: EA8 exit, canary scan, EK1, ledger summary; nothing to commit | OK |
| 26 | EN1 | No makefile regenerated; six makefiles decode (my EK1 pre-scan) | OK |
| 27 | EN2/EN3 | Not exercised | N/A |
| 28 | EN4 my writes | Only my allowed folders (§8) | OK |
| 29 | EN5 | Codex applied none; correct, an engine-tree write is on the hard-STOP list | OK |
| 30 | EM2 `CLAUDECODE` | Present in the launch base (positive control), absent from all 7 child receipts | OK |
| 31 | EL1 denylist | 7 child receipts (python), contract PASS, 0 blocked/secret names | OK |
| 32 | EL2 canary | Positive control at launch (child sha = generated `c2dfc699…`, dry run) and live launch; 0 value hits in 168 new/changed files since 10:45:58; 0 child environments carry the name | OK |
| 33 | EL3 | A5 finished | N/A |
| 34 | EL4 kit | Worked as built: preflight PASS, gate, launch, lock, ledger STOP at finish | OK |
| 35 | EL5 my deviation | Wrapper removed `CLAUDE_CODE_MESSAGING_TOKEN` only (names-only receipt) | OK |
| 36 | EK1 secrets | Pre-scan 40,211 files 0 values (2 unreadable owner-slot temp files); post-scan 182 files 0; uploaded crash folders had 0 hits | OK (O4) |
| 37 | EK2 / EK3 | No makefile regenerated / `GIT_OPTIONAL_LOCKS=0` in my and Codex's git calls | N/A / OK |
| 38 | EI2–EI4 | No builds, no query | N/A |
| 39 | EG1 | 5.1 bookkeeping class not involved; the 4.25 marker is outside it | OK (see #9) |
| 40 | EG2 receipts | Codex's `engine-stop.json`: path, size, mtime, hash = mine | OK |
| 41 | No commit / push / tag, W4 | feat `b20e66e`, refs and worktree heads equal before/after | OK |
| 42 | Launches | 7 polls `launch_class_ours 0`, no sightings file; Codex's 58 commands contain no launch, kill, delete or git-write words | OK |
| 43 | Ledger row | `…-resume16` exactly once | OK |

**Count (table, row 37 counted OK): 29 OK · 8 N/A · 2 GAP (G1, G2) · 3 OPEN · 1 STOP (inherited, correctly raised) · 0 stop violations · 0 conflicts. Plus G3 in §6, and O1–O7 in §9 (O2 and O7 repeat table rows 4, 15 and 17): 3 GAP and 7 OPEN in total.**

## 2. The owner leg's evidence (read-only)
**Timeline (IST):** claim 23:47:25 → preflight PASS 23:55:57 → owner gate CLEAR 00:07:36 (644 s) → editor launched 00:07:38 (pid 31124) → CRC monitor 00:07:40 → editor-spawned UBT ×2 00:07:44/51 → shaders done within the first hour → engine ensure + automatic report upload 00:14:34–46 → editor-spawned `EpicGamesLauncher.exe` 29984 at 00:14:17 (then 48016) → map `AssetsOverview` loaded 00:16:32 (128 s) → **editor idle ~9.8 h** → asset editor for `SM_WindowHouse3` opened 10:05:18.9 → `CloseEditor()` 10:07:07.0 → assert 10:07:20.7 → crash report uploaded 10:07:40–41 → CRC window closed 10:07:42, `Editor/ExitCode:3` → kit ledger STOP 10:09:51. **The preview stayed open 108 s** (opening to exit). The crash came 13 s after Exit, not a minute.

**Content admission against `docs/europa/09411-content-admission.md`:** `Mounting plugin AnomalyInjector` (line 118); `AnomalyInjector module started.` (one line); asset discovery 521 s; 0 lines naming any of `M_CorruptedTexture_Pink`, `M_CorruptTex_Normal`, `M_CorruptTex_UV`, `M_MissingTexture_Checker`, `T_CorruptTex_NoiseN`; 0 `LogLinker`, 0 `LogSavePackage`, 0 thumbnail lines; the two `LogCook` lines are the editor's cook-settings banner (00:14:20), not a cook. In 4.25 the rejection is silent (`PackageReader.cpp:78–83` returns VersionTooNew/TooOld without a log; `AssetDataGatherer.cpp:864–873` returns false, retry only for CustomVersionMissing), so the evidence is absence plus unchanged bytes. The new `Intermediate\AssetRegistryCache\*.bin` files are 44-byte stubs and name no package at all; no full registry cache was written (the crash preceded shutdown).

**Paris host changes:** Intermediate +34 (`AssetRegistryCache`); `Saved\Crashes` two folders; `Config\DefaultEditor.ini` mtime only. `%LOCALAPPDATA%\UnrealEngine\4.25\Saved\Config\Windows\Editor*.ini` (3), `pending-reports.json`, `XmlConfigCache-E+EpicGames+UE_4.25.bin` changed (EA4 classes). D:\IA_Europa +4,119 files (mostly `Caches\425`). Two empty, older folders `%LOCALAPPDATA%\Temp\UnrealBuildTool` (2026-10-20) and `UnrealShaderWorkingDir` (2026-10-23) disappeared between 094-17 and now; not attributed.

## 3. The probe's observations and the A6 gates
`owner-leg\probe\probe.jsonl`: 586,604,193 B, 1,841,488 rows, sequence strictly increasing, 0 unparsed. Kinds: probe_start 1, world_created 6, pre_initialize 6, initialized 6, world_snapshot 10, component_state 9, world_cleanup 3, world_cleaned 3, update begin/end 920,722 each. Snapshots: Editor (type 2) `initialized` ×2 + `cleanup_before` ×1 (`Untitled`, `AssetsOverview`); EditorPreview (type 4) `initialized` ×4 + `cleanup_before` ×3 (`World_0`–`World_3`). Every snapshot has the five subsystem classes once each, `registered_count == len(instances) == 0` (non-game worlds), CDO `cdo_allowed false`, tick type 2, `cdo_should_create false`. Update rows: 920,722 matched pairs, 0 repeated begins, 0 unmatched ends, 0 eligible, 0 paused, 0 unclosed, eligibility rule consistent (`review\probe-summary18.json`).

**A6 gates met (owner-leg scope):** positive Editor and positive EditorPreview world observations; correct non-game admission and CDO state for all five classes; a monotone, complete stream up to the crash; preview ≥ 60 s (`World_2` lived 101.6 s on the probe clock). **Not in this leg's scope:** Game/PIE, pause, eligible-update denominators, two PIE worlds, GC controls. **Lost:** the last snapshot's `component_state` (the crashing walk), `World_3`'s `world_cleanup`/`world_cleaned`, the `AssetsOverview` exit cleanup, `World_0`'s cleanup and `probe_stop`. **Not met:** a clean exit (ES2 defers it to the next runtime leg).

## 4. Root cause
- `EuropaPhaseAWorldProbe.cpp:79` registers `FWorldDelegates::OnWorldCleanup` → `Cleanup()`; `:157` `Snapshot(World, "cleanup_before")`; `:281` `SnapshotState(World, Label)`.
- `EuropaPhaseAProbeDriver.cpp:279` `for (TActorIterator<AActor> It(World); It; ++It)`; `:282` `It->Tags` dereferences.
- 4.25 `EngineUtils.h:200–222` (WITH_EDITOR, class `AActor`) fills the iterator from `Level->Actors` with no reachability filter; `operator++` (`:288–323`) filters level/world/suitability only; `GetActorChecked()` `:247–248` `checkf(!CurrentActor->IsUnreachable(), "%s", GetFullName())` → the logged `WorldSettings None.None:None.None`.
- 5.1 `EngineUtils.h:202/222` same path, `:247–250` same `checkf`; `SkipPendingKill` (`:403`) uses `IsValid()` only. **The assert reproduces on 5.1.**
- The frames below the delegate (Engine → UnrealEd → CoreUObject) are the final GC's world teardown, consistent with the ruling.

## 5. The fix (not started) and what a fix must also cover
- **Remaining teardown-unsafe patterns in the pre-fix source (static read):** (a) the `TActorIterator<AActor>` in `SnapshotState` from `Cleanup`; (b) **5.1-only:** `Snapshot()` calls `FindObject<UClass>(…"/Script/AnomalyControlServer.AnomalyControlServerSubsystem")` (`WorldProbe.cpp:237`) and `check(ControlClass)`; 5.1 `StaticFindObjectFast` is Fatal while `IsGarbageCollectingAndLockingUObjectHashTables()` (`UObjectGlobals.cpp:331–334/354–357`), and 4.25 `StaticFindObject` is Fatal while `IsGarbageCollectingOnGameThread()` (`:318–321`, not reached on 4.25 because that branch is 5.1-only). Whether exit-time `OnWorldCleanup` runs under the 5.1 lock is not established. (c) `ObserveSubsystem`/`GetSubsystem`/catalogue reads on subsystems that may be unreachable at that point (no assert in either engine's source path I read). The other actor iterator (`Driver.cpp:219`, `ACameraActor`, scripted `prepare`) is not a cleanup path.
- **PIE end:** `EndPlayMap` cleans the PIE world before GC marks it, so the iterator assert is not expected there; not proven by a run.

## 6. GAPs
- **G1 (Codex's report):** ES3 (upload, restart, C: file list), the content-admission lines, prompts, DDC/shader routing and the probe observations are reported as "not determined/unqualified". Stopping was correct; the read-only answers are in §2–§3 here.
- **G2 (EA4 attribution):** the kit observer recorded `EpicGamesLauncher.exe` 29984 as a child of `UE4Editor` 31124 (00:14:17 IST); the launcher (48016, then 26116 after a 10:38 restart) is **still running**. During the session `C:\ProgramData\Epic\EpicGamesLauncher` grew by 933,348,877 B (258 files) and `EpicOnlineServices` changed 149 files; the kit ledger classed all of it **foreign**. `ecache.bin` in the owner slot's D: Temp was created 00:15:10 (51 s after the launcher started) and that folder is still being written (a 670,796 B `.tmp` locked since 10:38–10:39). If attributed to the port, EA4's 250 MB C: cap and "write anywhere else on C:" apply. Chat's call.
- **G3 (ES3 scope):** the first upload was an **ensure** report sent with no click; "Close Without Sending" on future cards does not cover it. Uploaded files included the full editor log and minidumps.

## 7. Self-test suites (re-run by me in CodeReview09418)
`owner_eq_self_tests.py` **159/159** (13 harmless children, 0 UE launches), `owner_kit_self_tests.py` **62/62 + 48 measured**, `idle_gate.py --self-test` **48/48** (copies changing only the fixture TEMP base). EA11 `check_reflected_headers.py --self-test`: Paris worktree PASS 7 headers, mutant detected; ReGate51 PASS; **release control Baseline51 `d7eee97` FAIL, 12 violations** (expected for pre-port code: a real-data can-fail). EK1 scanner self-test 17/17; canary search self-test 8/8.

## 8. Writes, snapshots
- Mine: `code-receipts-094-18\`, `resume-prompt-094-18.txt`, the preserved `094-17-*` copies (hash-equal), `_reviews\094-18-codex-report.md` (body byte-identical, `FC5F1207…`), this file; in `D:\IA_Europa` only `Temp\CodeReview09418` (probe-source copies, EA11 outputs, harness) and `Receipts\CodeReview09418` (self-test roots). I killed nothing and deleted nothing.
- Before → after the job (61 listings): 54 equal, including the full 4.25 install, all 5.1 engine trees, all three hosts' trees, refs, worktrees, m51, StackOBot. Differences: free space, processes, C:/E: root mtimes, ProgramData\Epic (launcher, −261/+57), tooling sizes, and D:\IA_Europa (+93 = Codex's `Receipts\09418` and my CodeReview09418; plus the owner-slot Temp churn in G2).
- Still open on the desktop: the kit console `cmd.exe` 44364 at its pause prompt.

## 9. OPENs (none blocking on their own)
- **O1** The STOP's disposition (chat): e.g. a receipted 4.25 runtime-bookkeeping class for `Engine\Intermediate\EditorRuns\<pid>`, analogous to EG1. A clean exit creates and deletes the marker inside the session, so a before/after inventory would not see it.
- **O2** The 5.1 GC-time `FindObject` in `Snapshot()` (§5b).
- **O3** Content admission is provable only by absence in 4.25; A8's cook needs a package-output absence check (the prediction already says so).
- **O4** Two owner-slot temp files were unreadable to both scans (held by a live process); contents unverified.
- **O5** Engine ensure `TextureDerivedDataTask.cpp:252` during texture DDC building (which texture: not established).
- **O6** The editor sat idle ~9.8 h between map load and preview.
- **O7** Carried: ER1 G3 fix, ER3 bare UBT (the editor spawns it at startup), 8.3/junction alias tests, EO1 reader budget, 5.1 probe control path (A9), EJ1/EJ3/EJ5.

## 10. Top 5 risks for the next runtime leg
1. **The exit assert recurs** on every host until the fix lands, and on 5.1 the `FindObject` path may fail first (O2). ES2 makes a crash there a STOP.
2. **Every 4.25 editor start writes `Engine\Intermediate\EditorRuns\<pid>`**; without a ruled class any crash is a STOP again, and `-NoHotReload` does not prevent it.
3. **Automatic uploads to Epic**: ensures are sent without a click, crash reports on one click; the uploaded payload contains the full editor log and a minidump.
4. **The editor-spawned Epic Games Launcher** inherits the session environment, writes ~0.9 GB on C:, outlives the session and keeps writing into our D: Temp (G2); it also loads the machine and can hold the idle gate.
5. **Editor-spawned bare UBT at startup** (ER3 gap) plus first-start shader compilation (~1 h here) and long idle sessions: measured legs need the bare-UBT gate fix and a session time limit.

VERDICT: REVIEW DONE — 29 OK / 8 N/A / 3 GAP (G1 Codex left ES3 and admission facts undetermined; G2 editor-spawned Epic launcher's +933 MB C: writes classed foreign; G3 ensure reports upload with no click) / 7 OPEN / 1 STOP inherited and correctly raised (4.25 EditorRuns\31124, HotReload.cpp:389–434) / 0 stop violations / 0 conflicts; Paris 1,453/1,453 and plugin 5/5 content unchanged before and after; 2 crash reports uploaded (00:14:46 ensure, 10:07:41 crash), no restart, no editor/CRC process; root cause confirmed in 4.25 EngineUtils.h:248 and 5.1 :250; probe stream intact to the crash, EditorPreview half of A6 met; fix and rebuilds not started; manifests 27/39/44 unchanged; P9 v2 18,264/18,264; m51 = P9 throughout; protected state PASS ×3; canary 0 value hits in 168 files; self-tests 159/159, 62/62+48, 48/48; 0 launches
