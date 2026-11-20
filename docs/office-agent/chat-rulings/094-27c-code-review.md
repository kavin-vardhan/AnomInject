# 094-27c — Claude Code review: camera fix, A6 re-authoring, and the full 18-leg 4.25 runtime run (W3: written after Codex exited)

**Date:** 2026-10-31, 02:05–02:40 IST · **Author:** Claude Code (mailbox brief 094-27c) · **Inputs:**
- Codex's report `_reviews\094-27c-codex-report.md` (collected byte-identical with one header line; body 51,286 B, `CC904650…`). Thread `01a14334`, resumed 15:46:49 – 02:04:36 IST, 37,066 s, relay tries 1, no capacity retry, exit 0. Ledger row `094-01-europa-phase-a-plan-resume31`, written once by the relay. **Codex's verdict: `RED | Two authoring sessions; 18 runtime sessions; A3 PASS, A5 FAIL, A6 FAIL, A7 FAIL, A8 FAIL; evidence retained; no push.`**
- Chat's `094-27b-ruling.md` (FI1–FI6), `094-27c-request.md`, and the rulings they bind.

**My receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-27c\` (`step0\`, `protect\`, `review\`, `preserved\`, `midjob-notes.txt`, `disk-sightings.txt`, `memory-sightings.txt`, `real-files-{start,after}.tsv`, `a6-packages-start.tsv`, `paris-lists-{before,after}.json`). Owned scratch: `D:\IA_Europa\Receipts\CodeReview09427c\` (`rotator-scan*.json`, `canfail-fi4-fi5.json`, `suites\`) and `D:\IA_Europa\Temp\CodeReview09427c\` (`scripts\`, `py27\`, `old\`, `canary-control\`). Plain folders, 0 reparse points. No account identifier is reproduced anywhere I wrote.

## Summary
- **I disagree with RED; by this brief's definitions the run is NEEDS-DECISION.** None of the RED conditions occurred: both authoring sessions exited 0; there was exactly one runtime crash (GC); nothing protected changed; Content changed only at FI2's three declared paths; no upload sign; the canary is absent. The run had one runtime crash and four failing gates, which the brief classes as NEEDS-DECISION.
- **The fixes worked in real sessions.** Both maps were re-authored with the camera read back as pitch −12, yaw 0, roll 0 before any save. The Load leg read back 42 of 42 saved values on each map, the camera included, with one `MAP LOAD` per map and one `QUIT_EDITOR`. **The new busy guard ignored 10 re-entrant ticks during those loads**, so 094-27b's re-entry (my predecessor's G1) is now observed in a real session.
- **A kit-versus-probe file conflict probably cost up to six legs (G1, blocking).** The kit's in-session monitor tails `probe.jsonl` while the probe appends to it with an exclusive handle. Three legs were stopped by the kit's own `PermissionError`, and the pattern on three more fits the probe silently disabling itself after a failed write. Codex recorded the three stops as "controlled owned stops" without a cause.
- **The GC crash is our probe's defect, confirmed** (`NewObject<UObject>` on an abstract class in a `-game` session). A6 cannot pass until that C++ line changes.
- Only lighting_mismatch reached three valid visible events (5). lod_corruption had 6 valid windows but no visible change; missing_object had 6 valid windows rejected for clean-frame noise.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | FI1 | 09427b kit refused: the runtime arming is `09427c` (source `7073697`) and `runtime_policy.validate` refuses any other job (`runtime_policy.py:52-53`); runtime suites re-run by me pass (below) | OK |
| 2 | FI2 recipe | `author_425_fixture.py:53-68` (at `7073697`): `unreal.Rotator(pitch=-12.0, yaw=0.0, roll=0.0)`, normalized read-back within ±0.01°, receipt `checked_before_any_save`, failure path saves nothing and sends one `QUIT_EDITOR` | OK |
| 3 | FI2 static rule | My own AST scan (engine Python 2.7.14) of all 93 `tools\europa` scripts: **0 positional `Rotator` calls in any script either kit runs**; keyword calls only at `author_425_fixture.py:55, :104`. The only positional calls are in the 5.1 stress recipe `author_51_stress_fixture.py:33, :35`, which neither kit runs (hash-bound only). My scan fires on the old recipe (`0a02987` lines 80 and 92). Codex's `rotator_contract.py` covers the five executed scripts | OK · O3 |
| 4 | FI2 stub | `fixture_python27_harness.py:84` and `runtime_python27_tests.py:49`: `def Rotator(roll=0.0,pitch=0.0,yaw=0.0)` | OK |
| 5 | FI2 can-fail | My run of the author harness: case `old_positional_camera` gives actual **roll −12, pitch 0**, 0 saves, 1 exit request, EXPECTED_FAIL | OK |
| 6 | FI2 quarantine | `09427c\quarantine\receipt.json`: the three old packages moved by receipt, size and SHA-256 equal at source and destination (`bbd24a79…`, `c2986f9a…`, `51c7fe5c…`); `final-state.json` FI2 and FF1 quarantines equal | OK |
| 7 | FI2 re-authoring | Both recipes AUTHORING_PASS (section 2); the authoring arming is `09427c` and refuses 09426d | OK · O2 |
| 8 | FI2 Content | My before/after lists: **only the three declared paths changed**; 1,453 other Content files, plugin Content, Config (18), the descriptor and the `.uproject` hash-equal | OK |
| 9 | FI3 driver | `runtime_editor_425.py` (at `7073697`): `BUSY` guard (`tick`), `terminal` state, `finish` records then unregisters then quits once; `console` refuses after terminal. My run of the runtime harness: `failing_camera_single_exit` (one load, one unregister, one quit, no EA_Travel load), `reentrant_load_single_exit` (1 re-entrant tick ignored), `callback_exception_once`, all PASS | OK |
| 10 | FI3 cause | Codex's citations check out: 094-27b `Load\…\Runtime.log` lines 711, 713, 714 (three `MAP LOAD` in frame 10 within 29 ms), quits 732/751/770; the preserved old driver loads at `:75`, raises at `:104-105`, `finish` at `:32-44`. Codex reports the Niagara crash cause **unconfirmed**; I agree. **New evidence:** this run's Load driver records `reentrant_ticks_ignored=10`, so load_level does re-enter the post-tick callback | OK |
| 11 | FI3 camera check | Normalized comparison within ±0.01° in both the recipe and the Load read-back | OK |
| 12 | FI4 | `runtime_evidence.outcome` (`:48-51`) counts a crash only on a nonzero exit without a deliberate owned stop; `runtime_runner.crash_policy` / `record_leg` end the run on the second. **My own can-fails against the kit's functions, 15/15 PASS:** exit 3 = crash and continues; a session-limit stop is not a crash; first crash continues, a pass after it continues, a limit stop does not count, the second crash ends the run (`FI4 second runtime crash`); an upload sign still ends it on the first leg. **Live:** GC exit 3 counted 1, run continued, `post_crash_owned_stop` stopped monitor 42360 (`-MONITOR=9556`) | OK |
| 13 | FI5 | `canary_eventlogs.exclusion_class`: errors 32/33 under `C:\Windows` → FI5 class; my can-fails confirm 32 and 33 classed, `..` paths, locks outside `C:\Windows`, and access-denied `.etl`/`.cnpf` under `C:\Windows` not classed | OK · O1 |
| 14 | FI6 order and window | Entry 15:46–16:05, fixes committed 16:28, quarantine 16:29, authoring 19:11 and 19:28, runtime 19:48 → 01:42 IST; every real session started before 06:00 | OK |
| 15 | FH2 real files | `runtime_real_files` re-run by me: 7 real files, 138 mutations, all rejected; the kept real files re-hash unchanged after the job (21 of 21) | OK |
| 16 | FH3 | Non-crash failures continued (12 failing legs, run went on); only GC counted as a crash | OK |
| 17 | FH4 | My re-hash of every leg's `raw-evidence.json` file list: 18 legs, 0 changed | OK |
| 18 | FH5 | 21 adopted-monitor rows, one per session (two for lighting SkipRestore: the pre-respawn child and the respawned one, both `-MONITOR=17604`). **My polls caught two live**: 31008 (parent 9172, `-MONITOR=5632`, missing_object SkipRestore) and 24752 (parent 23080, `-MONITOR=2104`, Admission); both match Codex's table | OK |
| 19 | FH6 | `-unattended` on all 18 runtime argvs; not on the authoring argvs (EX4 kept) | OK |
| 20 | FH7 | Lines unchanged (8/6 start, 4 stop). WorldsGame stopped in session at free commit < 4 GiB under the owner's load | OK · O6 |
| 21 | FH8 | O3: the `-game` legs ran with both privacy switches and their logs have 0 upload lines; O6: every counted picture's AA echo is 0 | OK |
| 22 | FH9 order | Smoke, Load, WorldsPIE, WorldsGame, GC, A7 (positive, NoApply, SkipRestore), Admission | OK |
| 23 | FB1 / A3–A8 | Section 4 | See §4 |
| 24 | FB2 kit | Limits as armed; one session per leg; 18 slots; failed evidence kept | OK |
| 25 | FB6 evidence | Gate verdicts, contact-sheet index (1 counted, 3 diagnostic marked NOT COUNTED) | OK |
| 26 | FB7 Python 2.7 | Both harnesses re-run by me under 2.7.14: author PASS (Smoke, EA_Core 90 calls, EA_Travel 87 calls, three expected-fail mutants), runtime PASS (3 editor scripts, 15 native game scripts, runpy mutant refused); no `.pyc` written into the tools | OK |
| 27 | FB7 smoke / FC3 | Smoke 19:48:42, 12.8 s, exit 0, reader PASS. No authoring smoke, as FI2 ruled | OK |
| 28 | FC4 / EU4 | Both switches on all 20 argvs; 0 upload/POST/datarouter lines in all 20 Runtime.logs and CrashClient.logs | OK |
| 29 | FD2 / FC6 | 5 UnrealBuildTool children, every one `-Mode=ValidatePlatforms -allplatforms` (EA_Core, EA_Travel, lod NoApply, blinking SkipRestore, lod SkipRestore); 0 compiler, linker or UHT; ShaderCompileWorker seen as an owned child | OK |
| 30 | FF2 / FG2 / FC5 | Saved: `WorldState\1202994220.json` and `2530609069.json` (mtime only), `Autosaves\PackageRestoreData.json`; **0 `*.ini` changed under `Saved\Config`**. Intermediate: 20 new and 32 rewritten `AssetRegistryCache` files | OK |
| 31 | FG3 / FI4 crash folders | No new folder in any of the five crash roots (start = after); no `ue4-crc-pid-*` left | OK |
| 32 | FG4 O5 | The first read-back that could catch the camera now passes on both maps | OK |
| 33 | FE2 / EL2 canary | My scan from the launch second: 3,230 files, 1.5 GB, 24 archives decoded, **0 candidates, 0 matches**, 0 read errors, 0 child receipts naming `IA_CANARY_TOKEN`; planted control 3 of 3, decoy not matched. Codex: 0 matches, GAP on 4 unreadable `C:\Windows` files (O1) | OK · O1 |
| 34 | FA1 | 0 reparse points in my scratch and under the Paris host (before and after) | OK |
| 35 | FA5 | Every leg admitted only after three gate samples with ≥ 10 min idle (lowest 622.7 s, A7 lighting SkipRestore) | OK |
| 36 | FA6 lean | Quick checks only; no EK1 pre-scan; no P9 re-hash of mine | OK |
| 37 | EZ1 roots | C:/D:/E: 60/113/70 at start and after, names equal; every poll `roots=same` | OK |
| 38 | EV3 / EY4 / EU2 | D: dipped to 32.1 GiB at 16:41 and 26.8 at 20:15 (page file 42.8 → 49.5 GB under commit pressure; my stat-only writer scan found nothing else large), recovered; Codex's gate held | OK · O7 |
| 39 | EY5 | No 5.1 launch, build or write; 5.1 manifests 39/39 and 44/44 | OK |
| 40 | EX3 / EX4 | Quarantine by receipt; authoring argv without `-unattended` | OK |
| 41 | EW1 | 27/39/44 = 110/110 at start and after; 4.25 manifest `c501895c…`, 27 files | OK |
| 42 | ET2 EditorRuns | `31124` unchanged; `48368` (094-27b's) unchanged; no new marker | OK |
| 43 | EA4 / EG1 / ET3 | Charged C:/E:/5.1 = 40,000,666 / 0 / 0 B (limits 250 / 50 / 100 MB) | OK |
| 44 | EU1 | Mine from `events.jsonl` after offset 58,585,165: 510 starts, 509 through `filtered09427c.ps1`, 1 direct read-only (`Get-Content .\filtered09427b.ps1`), 0 direct write-capable; 58 file edits. Equal to Codex's | OK |
| 45 | EA8 / P9 v2 | Codex entry and exit PASS (472,553 inventory, 1,023 EG1, 23,033 hashes, 104,794,989,456 B); P9 v2 file `8E3ED095…` unchanged (mine) | OK |
| 46 | m51 / protected | m51 PASS at start, before launch (wrapper) and after; protected PASS at start and after (release 8/8, BankOverflow 0 lost/0 changed, StackOBot Content and cache Binaries equal) | OK |
| 47 | Launches | Exactly 20 `UE4Editor.exe` child receipts: 2 authoring, 18 runtime (15 `-game`), one per slot; 0 UE-class processes after | OK |
| 48 | Commits / W4 | `7073697` 16:28:01, `2e2766f` 01:56:17; author = committer = real time; no `origin/feat/europa-port`, no tags; 0 comment lines added, the 4 my predecessor flagged removed | OK |
| 49 | D2 | Author harness attempt 1 refused the still-existing outputs, attempt 2 passed; other suites first time | OK |
| 50 | Runtime arming binds new packages | `Europa425-Runtime.json` `protected.content` = EA_Core 23,168 B `3c9c34dc…`, EA_Travel 21,124 B `6b7c9dd6…`, SM_EA_TwoLod 137,553 B `c4fb3976…` | OK |
| 51 | Kit in-session monitor vs probe file | Three legs stopped by the kit's own read of `probe.jsonl`; three more fit a silenced probe | **GAP (G1)** |

**Count (51 rows):** 49 OK (rows 3, 7, 13, 20, 33, 38 with a note) · 1 GAP (row 51) · row 23 in §4.

## 2. The authoring
| Recipe | Gate wait | Start – end (IST) | Duration | Exit | LogExit / probe_stop | Camera read-back (before any save) |
|---|---|---|---|---|---|---|
| EA_Core | 72.8 s | 19:11:46 – 19:12:23 | 36.6 s | 0 | line 1143 / 1 (2 skipped cleanups) | pitch −12.0, yaw 0.0, roll 0.0, passed |
| EA_Travel | 536.2 s | 19:28:26 – 19:28:39 | 12.4 s | 0 | line 1118 / 1 | pitch −12.0, yaw 0.0, roll 0.0, passed |

Both: 0 error or ensure lines, 0 upload lines, no PIE rows, privacy switches present, one `ValidatePlatforms` UBT child each.

| New output (my hash) | Bytes | SHA-256 |
|---|---|---|
| `Content\EuropaPhaseA\SM_EA_TwoLod.uasset` | 137,553 | `C4FB39761A37EDFC…` |
| `Content\EuropaPhaseA\EA_Core.umap` | 23,168 | `3C9C34DCE6336EF6…` |
| `Content\EuropaPhaseA\EA_Travel.umap` | 21,124 | `6B7C9DD632955D4F…` |

## 3. The runtime run, per leg
Idle at admission is the last of three gate samples; all were ≥ 600 s.

| Leg | Admitted idle | Start – end (IST) | Duration | Exit | Reader |
|---|---|---|---|---|---|
| Smoke | 3,165 s | 10-30 19:48:42 – 19:48:55 | 12.8 s | 0 | PASS |
| Load (A3) | 3,301 s | 19:50:58 – 19:51:13 | 15.1 s | 0 | PASS (42/42 + 42/42) |
| WorldsPIE | 3,444 s | 19:53:22 – 19:54:36 | 74.6 s | 0 | FAIL: missing clean probe stop |
| WorldsGame | 815 s | 20:08:35 – 20:09:03 | 27.3 s | 125 | FAIL: FH7 commit stop |
| GC | 702 s | 20:20:55 – 20:21:02 | 6.7 s | **3 (crash)** | FAIL |
| A7 blinking positive | 829 s | 20:23:03 – 20:23:15 | 12.7 s | 125 | FAIL: kit `PermissionError` on probe.jsonl |
| A7 lighting positive | 957 s | 20:25:09 – 20:25:34 | 25.0 s | 0 | **PASS** (5 counted) |
| A7 lod positive | 713 s | 20:37:45 – 20:38:13 | 27.5 s | 0 | FAIL: 0 visible of 6 valid |
| A7 missing_object positive | 734 s | 20:50:55 – 20:51:25 | 29.7 s | 0 | FAIL: 0 of 6 valid (clean-frame noise) |
| A7 blinking NoApply | 723 s | 21:03:46 – 21:04:13 | 26.8 s | 0 | FAIL: control event invalid (coverage) |
| A7 lighting NoApply | 1,707 s | 21:33:01 – 21:33:27 | 25.5 s | 0 | FAIL: control event invalid (coverage) |
| A7 lod NoApply | 706 s | 21:45:32 – 21:45:52 | 20.5 s | 0 | PASS |
| A7 missing_object NoApply | 707 s | 21:57:58 – 21:58:18 | 20.6 s | 0 | PASS |
| A7 blinking SkipRestore | 713 s | 22:10:31 – 22:10:55 | 24.5 s | 0 | FAIL: control event invalid (coverage) |
| A7 lighting SkipRestore | 685 s | 23:55:09 – 23:55:46 | 37.0 s | 125 | FAIL: kit `PermissionError` on probe.jsonl |
| A7 lod SkipRestore | 2,624 s | 10-31 01:13:34 – 01:13:49 | 15.3 s | 125 | FAIL: kit `PermissionError` on probe.jsonl |
| A7 missing_object SkipRestore | 2,738 s | 01:15:27 – 01:20:31 | 303.5 s | 125 | FAIL: session limit (probe silent ~290 s) |
| Admission (A8) | 723 s | 01:32:00 – 01:42:00 | 600.4 s | 125 | FAIL: session limit (probe silent ~593 s) |

Probe contract on the raw files (my run): PASS on the 11 legs that ended with `probe_stop`; "start/stop ordering" on the 7 without one. Console lines match the kit's catalog; 0 error lines except GC's assertion block.

## 4. Gates
| Gate | Reader | Mine |
|---|---|---|
| A3 editor load + compile-out | PASS | **Agree.** Load: one load per map, 42/42 checks each, camera (−12, 0, 0) raw and normalized, one exit; Smoke PASS |
| A5 worlds and lifecycle | FAIL | **Agree.** WorldsPIE has no `probe_stop`: its last probe row is frame 793 while the editor ran to frame 979 and exited cleanly (fits G1). WorldsGame was stopped by FH7 under the owner's load |
| A6 reflected GC | FAIL | **Agree.** Confirmed probe defect: `Runtime.log:715` `Assertion failed: GIsEditor \|\| bCreatingCDO \|\| !InClass->HasAnyClassFlags(CLASS_Abstract)`, stack `:727` in `GCProbe()`; `EuropaPhaseAProbeDriver.cpp:437` `NewObject<UObject>(GetTransientPackage())`; stock `Object.h:49` declares UObject `CLASS_Abstract`; `UObjectGlobals.cpp:2282` checks it when `GIsEditor` is false (`-game`) |
| A7 per anomaly | FAIL | **Agree.** My counts from the raw `command_result` rows equal the reader's on all 12 legs; my own window-validity recomputation agrees on every event. lighting_mismatch: attempted 8, applied 6, reverted 6, refused 2 (the invalid-target and unsupported-ID controls), valid 5, invalid 1 → PASS. lod_corruption: 8/6/6/2, valid 6, but the reader saw no visible active picture in any of them. missing_object: 8/6/6/2, valid 6, rejected as unqualified clean-frame noise. blinking: 0 attempted (stopped at 12.7 s). NoApply PASS for lod and missing_object; blinking and lighting NoApply, and blinking SkipRestore, invalid on observation coverage; the other three SkipRestore legs never reached an event |
| A8 non-package | FAIL | **Agree.** Admission reached its 600 s limit with the probe silent for its last ~593 s |

## 5. The pictures
- **Index:** `D:\IA_Europa\Receipts\09427c\contact-sheet-index.json`: 1 counted sheet (lighting_mismatch) and 3 diagnostic sheets marked NOT COUNTED (lod_corruption, missing_object, and blinking from its SkipRestore control).
- **The counted sheet** (`runtime-attempts\A7_-lighting_mismatch-positive\01\lighting_mismatch-contact-sheet.png`, 1920×1920, hash equal): 15 pictures, events 0–4 before/during/after; every picture exists, opens as a 1280×720 PNG, matches its recorded hash, belongs to the event its row names, and has an AA echo of 0.
- **My look:** the view is level (horizontal floor edge) and slightly downward (the cube tops and the floor are seen from above), so the camera fix shows. Before and after frames match, and "during" is clearly dimmed. **The lit frames are heavily blown out to white**, with the cube faces saturated; that may hide subtler anomalies such as the LOD swap.

## 6. GAPs and observations
- **G1 (blocking for any re-run): the kit's monitor and the probe fight over `probe.jsonl`.**
  - **Source:** the probe appends each row with `FFileHelper::SaveStringToFile(…, FILEWRITE_Append)` (`EuropaPhaseAWorldProbe.cpp:106`), without `FILEWRITE_AllowRead`. Stock 4.25 then opens the file with share mode 0, so nothing else may hold it (`FileManagerGeneric.cpp:108`, `WindowsPlatformFile.cpp:1026-1032`). The kit's monitor opens the same file to tail it on every check during the session (`runtime_monitor.py:160-166`, called at `:210`).
  - **One side of the collision is observed:** when the monitor opens it during a probe write, Python raises `PermissionError` [Errno 13]. That propagates out of `check()`, the kit stops the owned tree and the leg ends with exit 125. This happened on A7 blinking positive, A7 lighting SkipRestore and A7 lod SkipRestore.
  - **The other side is a strong candidate, not observed:** if the probe's own write fails because the monitor holds the file, the probe sets `bEnabled = false` (`:107-109`) and writes nothing more, silently. That fits three legs:
    - WorldsPIE: last probe row at frame 793, editor to frame 979 and a clean exit, no `probe_stop`;
    - missing_object SkipRestore: probe silent ~290 s before its session limit;
    - Admission: probe silent ~593 s before its session limit.
  - **Why nothing caught it:** the stand-ins are written by Python, which shares files, so the rehearsals cannot collide. Codex recorded the three kit stops without a cause.
- **G2 (blocking for A6): the GC probe constructs an abstract `UObject`** (§4). It is a C++ change to the probe and needs a build.
- **G3 (needs chat): A7's picture result for lod_corruption and missing_object.** Both had 6 valid, applied and reverted events and still counted 0. Whether the LOD swap is visible at this camera and exposure is unknown, and the lit frames are saturated (§5).
- **G4 (kit design): control events marked invalid on "observation coverage missing"** (blinking and lighting NoApply, blinking SkipRestore). The event window starts before the first observation covering it, so a correct control fails.
- **O1: the canary scan's 4 unreadable files** are access-denied (5), not locked: three `C:\Windows\System32\SMI\Store\Machine\SCHEMA.DAT{…}.cnpf` and `winevt\Logs\Microsoft-Windows-CodeIntegrity%4Verbose.etl`. They are outside both FE2 (denied `.evtx`) and FI5 (errors 32/33). My scan of our own write roots is clean, and its control works.
- **O2: the recipe now calls `new_blank_map(False)` and `save_map`** instead of `new_level` and `save_current_level`. FI2 said the bindings stay the same; Codex changed them so that nothing is saved before the camera check, because `new_level` saves the map (`EditorLevelLibrary.cpp:534-545`). The new calls run the same `GEditor->NewMap()` and `UEditorLoadingAndSavingUtils::SaveMap` that `new_level` used internally (`FileHelpers.cpp:4225-4260`), and they worked in both real sessions. They had no smoke session first.
- **O3: the 5.1 stress recipe still has positional rotators** (`author_51_stress_fixture.py:33, :35`). It is outside EY5 and neither kit runs it, but it would break FI2's rule if it were ever run.
- **O4: the four fixture suites** (`fixture_fc/fe/ff/self_tests`) need an explicit job config, so I did not re-run them. Codex's qualification shows them PASS, and my author-harness run covers the recipe.
- **O5: no new EditorRuns marker and no crash folder** after the GC crash. The `-game` crash left only exit 3 and the monitor's log.
- **O6: WorldsGame was stopped by FH7** (free commit < 4 GiB) while the owner's jobs held memory. That is the environment, not the kit.
- **O7: D: dipped twice** (to 32.1 GiB at 16:41 and 26.8 at 20:15) as `D:\pagefile.sys` grew under the owner's load; both dips recovered.

## 7. Top 5 risks for Phase B (the six basic anomalies, with 4.25-authored materials)
1. **Probe and kit I/O races (G1).** Any engine-side writer that opens files exclusively will collide with a tailing reader. Use `FILEWRITE_AllowRead` (or one long-lived shared handle) in the probe, and make the monitor retry on sharing errors. A silent self-disable must become a loud row or log line.
2. **Exposure and saturation.** The fixture's lit frames are blown out, so a material swap or LOD change can be invisible in pixels while the anomaly "applies". Each anomaly class needs a pixel check calibrated on a frame that is not saturated.
3. **`-game`-only engine checks.** GC showed that code legal in the editor (a warning) is fatal in `-game`. Phase B's anomaly code must be exercised in `-game` sessions, not only in PIE.
4. **Material authoring traps** (missing usage flags rendering default grey; G2/G49 history) remain unproven on 4.25. Every authored material needs a real-session read-back and a picture.
5. **Windows for real legs.** Physical memory and commit sat near the gate lines all evening under the owner's work; one long leg (missing_object SkipRestore, Admission) can burn its whole limit. Session limits plus G1 cost two legs ~15 minutes.
