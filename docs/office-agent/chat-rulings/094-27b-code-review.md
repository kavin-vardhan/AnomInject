# 094-27b — Claude Code review of the fixed 4.25 runtime kit, its re-arming for 09427b, and the real run that crashed at the A3 Load leg (W3: written after Codex exited)

**Date:** 2026-10-30, 15:26–16:05 IST · **Author:** Claude Code (mailbox brief 094-27b) · **Inputs:**
- Codex's report `_reviews\094-27b-codex-report.md` (collected byte-identical with one header line; body 37,172 B, `BD3976DD…`). Thread `01a14334`, resumed 10:43:25 – 15:25:53 IST, 16,949 s, relay tries 1, no capacity retry, exit 0. Ledger row `094-01-europa-phase-a-plan-resume30`, written once by the relay. **Codex's verdict: `RED | 2 real sessions; A3 FAIL, A5 NOT_RUN, A6 NOT_RUN, A7 NOT_RUN, A8 NOT_RUN; evidence retained, no retry or push.`**
- Chat's `094-27-ruling.md` (FH1–FH9), `094-27b-request.md`, and the rulings they bind.

**My receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-27b\` (`step0\`, `protect\`, `review\`, `preserved\`, `midjob-notes.txt`, `disk-sightings.txt`, `memory-sightings.txt`, `real-files-start.tsv`). Owned scratch: `D:\IA_Europa\Receipts\CodeReview09427b\` (`canfail\`, `suites\`) and `D:\IA_Europa\Temp\CodeReview09427b\` (`scripts\`, `py27\`, `canfail\`, `canary-control\`). All plain folders, 0 reparse points. No account identifier is reproduced anywhere I wrote.

## Summary
- **I agree with RED.** The fixes held and the kit behaved as ruled: the fixed reader passed the new real Smoke, both crash monitors were adopted by exact `-MONITOR` PID, and the crash ended the run as FH3 requires. **But our editor crashed:** the A3 Load session (PID 48368, 14:43:40–14:43:52 IST) exited with code 3 after `EXCEPTION_ACCESS_VIOLATION reading address 0xffffffff` with `UE4Editor-Niagara.dll` at the top of the stack, during shutdown (`Runtime.log:775` "Preparing to exit", `:779-801`). Sixteen legs never started; there are no A7 pictures.
- **Two defects of ours came first, both from the Load leg's own files (my G1, G2).** (1) The editor driver re-entered itself: three `MAP LOAD` commands were issued in one frame before any finished (`Runtime.log:711, :713, :714`), then three loads, three checks and three `QUIT_EDITOR` (`:732, :751, :770`). (2) The first-ever read-back of the authored A6 fixture failed: `Saved camera rotation differs`. From 4.25 source, the Python `unreal.Rotator` takes its positional arguments as roll, pitch, yaw (`NoExportTypes.h:454` names `MakeRotator` as the native make, and `KismetMathLibrary.h:2142-2146` orders it Roll, Pitch, Yaw), so the recipe's `unreal.Rotator(-12,0,0)` (`author_425_fixture.py:80`) most likely saved a −12° **roll**. That is a strong candidate, not an observation: no receipt kept the saved angles.
- **The crash's own cause is not established (G3).** No crash report folder was written (`CrashClient.log:64` `Editor/ExitCode:3`, `:67-68` `MTBF/NoSessionFound`, `MTBF/NoTempCrash`), there are no symbols, and whether it happens without the re-entry is unknown.
- **Nothing protected changed, no upload, no canary.** Content, Config, Saved\Config, the descriptor and the `.uproject` are unchanged by my own before and after lists. Neither session's logs show an upload, send or POST. My canary scan found 0 candidates in 3,201 files; my planted control was found 3 of 3. m51 matched P9 at start, before launch and after. P9 v2 is unchanged and all 110 bound files match.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | FH1 | The 09427 kit stayed spent: arming `Europa425-Runtime-09427-spent.json` preserved; `runtime_tests` (121, re-run by me) refuses `09427` and older armings. The 094-27 smoke files are kept, and still hash-equal after the job (`real-files-start.tsv`, 10 files, rechecked) | OK |
| 2 | FH2 fix | `runtime_readers.py:28` and `runtime_probe_contract.py` read `world_cleanup` with `actor_snapshot=='skipped'` and a known `skip_reason`. **The fixed reader passes a copy of the exact 094-27 smoke files the old one failed** (my canfail `FH2_fixed_reader_passes_real_094-27_smoke_copy`), and passed the new real Smoke (reader.json PASS) | OK |
| 3 | FH2(a) emitters | My `review\emitters27b.py` checked all 26 contract rows: **every kind and field literal is present inside its cited probe line range** (`review\emitters.json`). The remaining reader keys are reader-internal, script-supplied labels (`runtime_scripts.py:17-55`) or kit receipts (`driver.json`, `monitor.jsonl`), as Codex's note says. Also confirmed: the `snapshot` command emits `world_snapshot` and then `component_state` with the same label (`EuropaPhaseAWorldProbe.cpp:358-359`, `ProbeDriver.cpp:170, :303-310`) | OK |
| 4 | FH2(b) real files | Codex: 5 files, 115 mutations rejected. Mine: the contract passes all five kept real streams and rejects every mutation I made (missing, duplicate, out of order, unsafe stop ×2, no skipped cleanup, missing stop, missing field); `startup()` passes the three with a Runtime.log. Suites re-run by me on my scratch: `runtime_real_files` PASS (5 files, 115) | OK |
| 5 | FH2(c) stand-ins | 10 `real-file-provenance.json` receipts in `rehearsal-attempt2`; the stand-ins copy the 094-27 smoke files | OK |
| 6 | FH2(d) | No PIE, travel, GC, apply/revert or screenshot rows exist yet; they rely on row 3 | Carried |
| 7 | FH3 | `runtime_evidence.outcome` (`:40-64`) ends the run only on a protected change, upload sign, canary hit, abnormal exit, hard stop, or a smoke start/exit/binding failure; `runtime_runner.record_leg` (`:91-96`) continues otherwise. My can-fails: a failing leg then a passing leg (both recorded, second runs), an ensure folder after a normal exit fails only its leg, exit 3 ends the run, a limit stop continues, a smoke reader FAIL continues, a missing binding / upload / protected change / canary hit end it. **Live:** Load's exit 3 ended the run, exactly as ruled | OK |
| 8 | FH4 | `raw-evidence.json` for both legs: 11 files each, re-hashed by me, 0 changed; canary-hash checked at seal | OK |
| 9 | FH5 | `runtime_owned.py` adopts only `values==[str(root_pid)]` of `-MONITOR=`, with creation time ≥ root and a retained handle; `stop()` stops monitors first (`:123-135`); `write_ledger.py:505-506` stops a monitor still alive 60 s after root exit. My live can-fail: a `-MONITOR=<root>` stand-in started by a broker outside the tree was adopted, and was stopped before the root; decoys (another PID, a doubled `-MONITOR`, the name `CrashReportClientEditor.exe` without `-MONITOR`) were not adopted and survived. **Real sessions:** Smoke 14924 → monitor 22808 (parent 36064), Load 48368 → 17244 (parent 17448), both matching the crash-client logs (`CrashClient.log:14, :62`), environment readable, canary name absent; both left on their own (no 60 s stop) | OK · O5 |
| 10 | FH5 citations | 4.25: `WindowsPlatformCrashContext.cpp:446` appends `-MONITOR=%u` with the current PID; `:502-537` (WITH_EDITOR) waits for the respawn, reads and deletes `%TEMP%\ue4-crc-pid-<pid>` (`:521-528`). `UE4Editor.exe -game` is the same WITH_EDITOR binary (`review\cites-fh.txt`) | OK |
| 11 | FH6 | `-unattended` on both argvs. 4.25: `EditorPythonExecuter.cpp:145` sets `GIsRunningUnattendedScript` only in `FExecuterTickable` (the `-ExecutePythonScript` route); the PY console path (`PythonScriptPlugin.cpp:560-566` → `:443-447`) builds a default `FPythonCommandEx`, so `RunString` (`:1014`) leaves it false. EU4 `bSendUnattendedBugReports=False` on both argvs. No import or save in either session | OK |
| 12 | FH7 | Start: `idle_gate.py:103-104` (6 GiB physical, 8 GiB commit); session stop: `runtime_monitor.py:24` (below 4 GiB). Armed `memory` equals the policy. My can-fails: 8.0 admits, 8 GiB−1 waits; 6.0 admits, 6 GiB−1 waits; 4.0 continues, 3.99 stops; 5.5 continues (the old 6 GiB line is gone). Live pre-checks: Smoke 6.02 GiB physical / 12.87 commit, Load 6.10 / 12.46 | OK |
| 13 | FH8 | O2: six-event budget unchanged (no A7 leg ran). O3: no `-game` leg ran, so the game opt-outs are still unproven live; both editor logs got the upload check. O5: the close race is separated after a recorded root exit (`runtime_fh_tests` 35, re-run by me). O6: `runtime_scripts.py` inserts `prepare` before every screenshot and the reader requires a 0 echo (`runtime_readers.py:90-93`); no picture was taken | OK · NOT RUN |
| 14 | FH9 order and window | Entry, fixes, rehearsal, re-arm, then the real run from 14:39 IST; Smoke then Load; run ended by the crash, well inside the 06:00 window | OK (STOP on crash) |
| 15 | FG2 | Saved\Config 24 files hash-equal before and after; no `UE4CC-*` folder under the host's `Saved\Config\CrashReportClient` | OK |
| 16 | FG3 as amended | No new crash or ensure folder in any of the five crash roots (`step0\crash-folders-*.tsv`); the crash ended the run by its exit code | OK |
| 17 | FG4 O5 (read-back) | **The first read-back failed:** `Saved camera rotation differs` (G2) | **GAP (G2)** |
| 18 | FB1 scope | Only Smoke and Load ran. Fixtures unchanged (`bbd24a79…`, `c2986f9a…`, `51c7fe5c…`) | NOT RUN beyond A3 |
| 19 | FB2 kit | Limits 300/600/900 s as armed; one session per leg; failed evidence kept in its slot; binds only the 27-file 4.25 manifest | OK |
| 20 | FB5 crash STOP | Load exit 3 ⇒ `Stopped: abnormal editor/game exit. No further session will be started.` (`entry-console.txt`) | OK |
| 21 | FB6 evidence | `gate-verdicts.json`: A3 FAIL, A5–A8 NOT_RUN; contact-sheet index: 0 sheets, 0 pictures | OK (no pictures) |
| 22 | FB7 Python 2.7 | Mine, under the engine's 2.7.14 with output under my scratch (`py27\run_harness27b.py`, two path edits): PASS, 3 editor scripts, 15 native game scripts, runpy-dict mutant refused, 0 saves, no `.pyc` written into the tools. **Limit:** the stub's `Rotator(*args)` (`fixture_python27_harness.py:75-77`) and an instant `load_level` cannot catch G1 or G2 | OK · see G1/G2 |
| 23 | FB7 smoke | Smoke PID 14924, 14:41:06–14:41:40 IST, exit 0, `SMOKE_PASS` 31/31 bindings, module alive, 0 error lines, probe contract PASS | OK |
| 24 | FC4 / EU4 switches | Both `-ini:EditorSettings…` switches on both argvs; 0 upload, send or POST lines in either Runtime.log or CrashClient.log (only `DataRouterUrl` configuration) | OK · O6 |
| 25 | FD2 / FC6 UBT | **No UnrealBuildTool child in either session** (35 process polls show only `UE4Editor.exe` and `CrashReportClientEditor.exe`); 094-27's smoke had `ValidatePlatforms` in 28 polls. 0 compiler, linker or UHT | OK · O1 |
| 26 | FF1, FF5 | Quarantine untouched; one full EA8 entry proof, quick checks between legs | OK |
| 27 | FE2 / EL2 canary | My scan from the launch second: 3,201 files, 633 MB, 28 archives decoded, 0 candidates, 0 matches, 0 errors, 0 child receipts naming `IA_CANARY_TOKEN`; control 3/3, decoy not matched. Codex's final scan: GAP, 3 locked Windows Update files outside FE2 | OK · O2 |
| 28 | FD3, FC2, FC5 | FD3 = row 12; FC2 = row 22; FC5: 32 Intermediate files mtime-only, 1 Saved file mtime-only | OK |
| 29 | FA1 | 0 reparse points in my scratch and under the Paris host (before and after) | OK |
| 30 | FA5 idle rule | Outer pre-check passed 14:19 IST; Smoke pre-check 1,811 s idle, Load 1,979 s; three quiet gate polls before each | OK |
| 31 | FA6 lean | Quick checks at start and after; no EK1 pre-scan; no P9 re-hash of mine; Codex's EA8 entry and exit | OK |
| 32 | EZ1 roots | C:/D:/E: 60/113/70 at start and after, names equal; every poll `roots=same` | OK |
| 33 | EV3 / EY4 / EU2 disk | D: fell to 28.5 GiB at 12:14 (below warn 30) when the system-managed `D:\pagefile.sys` grew by 19.5 GB under the owner's load, then recovered by 12:24; no session was running | OK · O7 |
| 34 | EY5 | No 5.1 launch, build or write; 5.1 manifests 39/39 and 44/44 | OK |
| 35 | EX4 as amended by FH6 | Row 11 | OK |
| 36 | EW1 | 27/39/44 = 110/110 at start and after; 4.25 manifest `c501895c…`, 27 files, unchanged | OK |
| 37 | ET2 EditorRuns | `31124` unchanged. **New `48368`** (0 B, created and written 2026-10-30T09:13:47.1196570Z, the Load editor's PID), left by the crash: inside ET2's class. Codex's receipt has path, size and hash, not times or process | OK · O3 |
| 38 | EA4 / EG1 / ET3 ledger | Charged C:/E:/5.1 = 53,248 / 0 / 0 B (limits 250 / 50 / 100 MB). C: = the crash client's 11 `WindowsNoEditor\*.ini`, XmlConfigCache, an NVIDIA DXCache file, and Windows' own logs and Update store; E: = the 48368 marker; 3 foreign (ClaudeTemp, Epic catcache, EOS lock) | OK |
| 39 | EU1 command audit | Mine from `events.jsonl` after offset 55,851,110: 314 command starts, 313 through `filtered09427b.ps1`, 1 direct read-only (`Get-Content .\filtered09427.ps1`), 0 direct write-capable; 39 file edits. Equal to Codex's | OK |
| 40 | EA8 / P9 v2 | Codex entry and exit PASS (472,553 inventory, 1,023 EG1, 23,033 hashes, 104,794,989,456 B, protected git and source manifests equal). P9 v2 file `8e3ed095…` unchanged (mine) | OK |
| 41 | m51 / protected | m51 PASS at start (10:38), before launch (10:43, by the wrapper) and after (15:28); protected PASS at start and after (release 8/8, BankOverflow 0 lost / 0 changed, StackOBot Content and cache Binaries equal) | OK |
| 42 | Launches | Exactly 2 `UE4Editor.exe` child receipts (runtime-run 001, 002); 0 in rehearsals and dry runs; 0 UE-class processes after | OK |
| 43 | Commits / W4 | `0886769` 11:23:19, `ecfb3ce` 11:53:31, `0a02987` 15:16:18; author date = committer date = real time; no `origin/feat/europa-port` (only the old `origin/europa-e1`), no tags; worktree clean; 0 email-, canary- or token-shaped added lines | OK · O4 |
| 44 | D2 | One prelaunch defect (observed hard stops could be lost at closeout) found by Codex, reproduced synthetically, fixed once (`ecfb3ce`), re-armed as a second generation; 0 entry starts before the fix | OK |
| 45 | EN5 | The one hard STOP reached is the crash; no exception used to pass it | OK |

**Count (45 rows):** 38 OK (rows 9, 13, 22, 24, 25, 27, 33, 37 and 43 carry a note) · 1 GAP (row 17 = G2) · 1 NOT RUN (row 18) · 1 OK with STOP (row 14) · 1 Carried (row 6) · rows 13 and 21 partly NOT RUN. The kit itself passed every ruling it was asked to meet; the run is RED because our editor crashed.

## 2. The real run, per leg
| Leg | Pre-check / gate (IST) | Start – end (IST) | Duration | Exit | Reader |
|---|---|---|---|---|---|
| Smoke | pre-check passed 14:39:17 (1,811 s idle, 6.02 GiB physical, 12.87 GiB commit); gate 82.2 s, three quiet polls | 14:41:06.5 – 14:41:40.7 (root); wrapper 14:41:45.7 | 33.0 s | 0 | **PASS** (31/31 bindings; probe contract PASS; 1 skipped cleanup, 1 `probe_stop`) |
| Load (A3) | pre-check passed 14:42:05 (1,979 s idle, 6.10 / 12.46 GiB); gate 70.8 s | 14:43:40.4 – 14:43:52.0 | 10.6 s | **3** | **FAIL** ("Nonzero or stopped session"); driver `DRIVER_FAIL: Saved camera rotation differs`; run STOP |
| WorldsPIE … Admission (16 legs) | — | — | — | — | NOT RUN |

Console lines match the runner's catalog (`runtime_runner.py:126, :155, :161, :163`; `entry-console.txt`). My 10-minute polls (14:36, 14:46) fell either side of both sessions, so I never saw them live.

## 3. Gates
| Gate | Reader | Mine |
|---|---|---|
| A3 editor load + compile-out | FAIL | **Agree.** `clean_exit` fails on exit 3. The compile-out half alone would pass: `startup()` on the Load files is PASS (all five module lines once, defines 0) |
| A5 worlds and lifecycle (PIE, -game) | NOT_RUN | Agree |
| A6 reflected GC | NOT_RUN | Agree |
| A7 per anomaly | NOT_RUN: blinking, lighting_mismatch, lod_corruption, missing_object all attempted 0 / valid 0 / applied 0 / reverted 0 / refused 0 / invalid 0 | Agree: no A7 probe row exists |
| A8 non-package | NOT_RUN | Agree |

**Pictures:** none. `D:\IA_Europa\Receipts\09427b\contact-sheet-index.json` lists 0 sheets and says so; `all-picture-aa.json` lists 0 native picture requests.

## 4. GAPs and observations
- **G1 (blocking for the next run): the editor driver re-enters itself during `load_level`.**
  - **Observation:** three `Cmd: MAP LOAD FILE="/Game/EuropaPhaseA/EA_Core.EA_Core" … SHOWPROGRESS=1` lines in frame [10] within 29 ms (`Load\…\Runtime.log:711, :713, :714`), then three complete loads, each followed by its own `QUIT_EDITOR` (`:732, :751, :770`); `driver.json` lists `QUIT_EDITOR` three times; the probe holds 5 `world_cleanup` rows, 3 of them skipped at engine exit.
  - **Candidate mechanism (not established):** the Slate post-tick callback that drives the leg calls `load_level`, and loading with progress pumps Slate again, so the callback runs again before the first call returns. Codex also calls re-entry a hypothesis.
  - **Not caught earlier because** the Smoke loads nothing, and the Python 2.7 stub's `load_level` returns at once.
- **G2 (GAP, needs chat): the authored EA_Core camera most likely has a −12° roll, not a −12° pitch.**
  - **Observation:** the Load check (`runtime_editor_425.py:102-105`) expects pitch, yaw, roll = (−12, 0, 0) and raised. The driver receipt did not keep the actual angles.
  - **Source:** `FRotator`'s Python wrapper uses `HasNativeMake="Engine.KismetMathLibrary.MakeRotator"` (`NoExportTypes.h:454`), whose parameters are Roll, Pitch, Yaw (`KismetMathLibrary.h:2142-2146`). So `unreal.Rotator(-12,0,0)` in `author_425_fixture.py:80` means roll −12. The 5.1 stress recipe (`author_51_stress_fixture.py:33`) has the same shape.
  - **Consequence if confirmed:** an A6 output (FG1) carries a wrong value, so EA_Core would need re-authoring, and the A7 picture ROIs (`runtime_readers.py:13-14`) were predicted from the intended view. A read-back that logs the angles before raising would settle it in one session.
- **G3 (OPEN): the shutdown crash.** `EXCEPTION_ACCESS_VIOLATION reading address 0xffffffff`; callstack `UE4Editor-Niagara.dll` ×4, then Engine, UnrealEd, CoreUObject, UE4Editor.exe (no symbols); after `LogExit: Preparing to exit.` and after three nested loads of the same map. No crash folder was written. **Cause not established**, and nothing shows whether a single clean load would also crash.
- **O1: FD2's guard was not exercised live.** No UnrealBuildTool child ran in either session. The only 4.25 call path I found is `Invalidate()` → `SetupSDKStatus()` (`TargetPlatformManagerModule.cpp:141-145`); what triggered it in 094-27 and not here is not traced.
- **O2: the canary's final coverage GAP.** Codex could not read three Windows Update files (`DataStore.edb`, `.jfm`, `edb.log`; sharing violation). They are written by the OS update service and lie outside FE2's event-log class. My scan of our own write roots is clean and its control works. Chat may want to widen FE2 to these OS files.
- **O3: ET2's receipt for `48368` lacks its times and process.** I recorded them (state-after.txt); the name is the Load editor's PID. A later stock editor start may remove it under ET2.
- **O4: three comment lines** were added to `tools/europa` Python (`runtime_fh_tests.py`, `runtime_owned.py:124`, `runtime_probe_contract.py:8`), against the plugin repository's no-comments convention.
- **O5: my polls never caught a session live.** FH5's real-session check rests on the kit's `adopted-monitors.json`, compared with both crash-client logs instead.
- **O6: the `-game` legs' opt-outs are still unproven at runtime** (FH8 O3); no `-game` leg ran.
- **O7: D: dipped to 28.5 GiB at 12:14** from page-file growth (`D:\pagefile.sys` 26.7 → 46.1 GB allocated) under the owner's load (four `kz_s3_post.py` render jobs, DaVinci Resolve), and recovered by 12:24. Physical memory was the main reason for the wait, from 11:43 to 14:39.

## 5. Top 5 risks for Phase B (six basic anomalies with 4.25-authored materials)
1. **Engine API semantics that stubs cannot see (G1, G2).** The Python 2.7 harness proves syntax and call names, not argument order or blocking behaviour. Every authored value needs a real-session read-back that logs the value it found, and every driver needs a re-entrancy guard proven in a real session.
2. **The shutdown crash (G3).** If the Paris host crashes in Niagara on a normal exit, every editor leg risks a crash, and FH3 stops the run at the first one. A single-load characterisation session (with the G1 guard) should come before more legs.
3. **Authored fixtures and materials can be silently wrong.** G2 shows an A6 output may already be wrong. 4.25-authored materials also fail silently (missing usage flags render default grey and still "apply"). Each needs a real-session pixel check per mesh class.
4. **The picture pipeline is still unproven.** The screenshot path, the 1280×720 size, the per-picture AA echo and the fixed ROIs (predicted from the intended camera, which G2 may have changed) have had no real test. A black or unlit frame reads like a null.
5. **Daytime windows are scarce.** Physical memory sat near the 6 GiB start line all day under the owner's work, and the page file can push D: below its start line. Real legs effectively run only when the owner is away, so one crash or defect can cost a whole night.
