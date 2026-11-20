# 094-27 — Claude Code review of the 4.25 Phase A runtime kit, its rehearsal and arming, and the real run that stopped at the smoke session (W3: written after Codex exited)

**Date:** 2026-10-30, 08:39–09:05 IST · **Author:** Claude Code (mailbox brief 094-27) · **Inputs:**
- Codex's report `_reviews\094-27-codex-report.md` (collected byte-identical with one header line; body 30,073 B, `33D40E11…`). Thread `01a14334`, resumed 07:07:10 – 08:38:55 IST, 5,505 s, relay tries 1, no capacity retry, exit 0, runner stderr 0 B. Ledger row `094-01-europa-phase-a-plan-resume29`, written once by the relay. **Codex's verdict: `NEEDS-DECISION | Runtime kit prepared; real Smoke stopped on an incorrect cleanup reader; all runtime gates and pictures remain unproved.`**
- Chat's `094-27-request.md` (FB1–FB7) and the rulings it binds.

**My receipts:** `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-27\` (`step0\`, `protect\`, `review\`, `preserved\`, `README-receipts.txt`). Owned scratch: `D:\IA_Europa\Receipts\CodeReview09427\canfail\` and `D:\IA_Europa\Temp\CodeReview09427\` (`py27\`, `scripts\`, `canfail\`, `canary-control\`). All plain folders, 0 reparse points. No account identifier is reproduced anywhere I wrote.

## Summary
- **I agree with NEEDS-DECISION.** The kit was built (`599ef70`), tested, rehearsed (5 stand-in cases), armed for 09427 (18 single-use legs, deadline 2026-10-31T00:30Z) and dry-run on all 18 legs with 0 launches. The live reading was clean. After FA5's idle pre-check and three quiet gate polls, the one real smoke session (editor PID 2344, 08:08:27–08:08:42 IST, exit 0) found all 31 Python bindings and shut down normally. **The armed reader then failed it on a schema it can never see.** As FB7 says, a failed smoke ends the run: no runtime leg started, so A3, A5, A6, A7 and A8 are all NOT RUN, and there are no pictures.
- **The stop cause is confirmed from source and from the real files (G1).** `runtime_readers.py:26` requires a `world_snapshot_skipped` row with `label="cleanup"`. The probe's cleanup hook (`EuropaPhaseAWorldProbe.cpp:211-225`) emits `world_cleanup` with `actor_snapshot="skipped"` and `skip_reason`, and never calls `Snapshot(…, "cleanup")`. So the reader could not pass any real leg. On a copy of the real smoke slot, every other `clean_exit` predicate passes (my G4).
- **Two further GAPs for chat:** the crash reporter respawns itself outside the editor's process tree, so the kit's owned tree and deadline never see it (G2, from 4.25 source); and every runtime argv now carries `-unattended`, against EX4, with no ruling cited (G3).
- **Nothing protected changed:** no crash, no upload evidence, no Content, Config, descriptor, `.uproject` or binary change (my own before and after lists). The canary is absent: 0 candidates in 2,011 files plus 25 decoded archives, and my planted control was found 3 of 3 times. m51 matched P9 three times. P9 v2 is unchanged and all 110 bound files match.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | FB1 scope | `runtime_policy.py` LEGS: Smoke, Load (A3), WorldsPIE + WorldsGame (A5), GC (A6), 4 A7 positives + 8 NoApply/SkipRestore controls (A7), Admission (A8). Host Paris 4.25, fixtures `EA_Core`/`EA_Travel` unchanged (hashes `c2986f9a…`, `51c7fe5c…`, `bbd24a79…`, before and after). EV6: the fixtures hold no Paris glass, and the ROIs are fixed (design :109-117). No real leg ran | OK as designed; NOT RUN |
| 2 | FB2 kit | Derived from the retained machinery (`Guard`, `write_ledger.run_child`, `fixture_wait`, `fixture_precheck`). Scope flags cook/commandlet/packaging/build/save are false (my A9 refuses cook=True). Content/Config/build changes between legs STOP (my C1–C4); in-session writes STOP (D1–D4, H4). Outputs are receipts only. Limits 300/600/900 s (design :14-23). One session per leg and no retry (A8; `check_unused_launch`). Failed evidence kept (H3, the real Smoke slot). Binds only the 27-file 4.25 manifest (`manifest_check`) | OK · G2 · O1 |
| 3 | FB3 in-session | `Monitor.check` acts on free commit below 6 GiB, the drive floors, the deadline and 60 s unreadable. Input, foreign GPU and physical memory are recorded. `validity()` marks windows with input, foreign GPU >15 %, gaps >250 ms, unreadable rows or missing coverage (my E0–E5), and an invalid event is not counted (E6, `runtime_readers.py:101`) | OK · O2 · O5 |
| 4 | FB4 design note | `docs/europa/09427-runtime-design.md`: I read 16 cited 4.25 ranges and all say what the note claims (`review\cites.txt`). These cover AutomationEditorCommon :635-649, :773-794; EditorAutomationTests :185-209; PlayLevel :821-838, :1050-1059, :2453-2458; EditorPythonExecuter :145-153, :185-195; PythonScriptPlugin :560-566; SceneView :845-858; EngineAnalytics :105-122 (-game excluded at :108); GameEngine :658-663; PythonScriptPlugin.uplugin :26-28; ModuleDescriptor :494-499; probe driver :210-215; AnomalyLod :85-90. Nothing under Content can change: LoadLevel only, no save, `-nowrite`, transient PIE settings, plus the protection checks. **Omitted:** EX4/`-unattended` and the crash-reporter respawn | OK · G2 · G3 |
| 5 | FB4 tests, can-fails | Codex: 12 suites PASS (`report.md` table). Mine: 56/56 as expected (`Receipts\CodeReview09427\canfail\summary.json`): A0–A9 arming and leg refusals, B0–B6 argv, C0–C4 protection between legs, D1–D6 in-session (incl. the FF2 control), E0–E6 validity, F0–F7 FE3/FD2, G1–G5 real smoke rows, H0–H5 the deadline through the real `execute`/`run_child` (detached grandchild stopped, sibling alive, evidence kept) | OK · G1 |
| 6 | FB4 rehearsal | `scratch\rehearsal-attempt1\summary.json`: pass, fail (exit 7, evidence kept), deadline (detached child stopped), Content write STOP, UBT build STOP; sibling survived in all 5. No Unreal | OK · G1 (stand-in copied the reader's schema) |
| 7 | FB4 arming | `Launch\Europa425-Runtime.json` `a71a8216…` (source `599ef70`, 09427, 18 slots, deadline 00:30Z), `.bat` `b20dc8c5…`. Older armings refused (my A3). Only the Smoke slot and `global` state exist | OK |
| 8 | FB4 dry runs | `scratch\dry\run-result.json`: DRY_RUN_PASS, 18/18 legs ARMED_DRY_RUN_PASS, 0 launches | OK |
| 9 | FB4 live reading | Codex 08:02: reasons [], 0 launches. Mine 08:43–08:44, 3 polls: same 10 exempt launcher PIDs, foreign [], idle/physical/commit equal. The owner was at the PC, so both read `input_idle` (`review\live-compare.json`) | OK |
| 10 | FB4 two-failure rule / D2 | No FB4 prerequisite failed twice. Two pre-child wrapper refusals and an index-correction commit were disclosed | OK |
| 11 | FB5 real run | Outer idle pre-check passed 08:06:10, Smoke pre-check passed 08:07:07 (27,998 s idle). Gate WAIT, WAIT, CLEAR at 08:07:20.6, :49.6, 08:08:18.9 (GPU 4 %). Smoke 08:08:27.417–08:08:42.719 IST (15.30 s wrapper, 13.44 s child), exit 0. Reader FAIL ⇒ run STOP, 17 legs unstarted | STOPPED (no crash) |
| 12 | FB6 evidence | `gate-verdicts.json`: all five NOT_RUN. `contact-sheet-index.json`: 0 sheets, 0 pictures, explicitly. Lean exit present (canary, audit, EA8 exit, ledger) | OK (no pictures) |
| 13 | FB7 Python 2.7 | Mine, under `E:\EpicGames\UE_4.25\…\python.exe` 2.7.14, recording stub, output under my scratch (two path edits in a wrapper, `py27\patch-receipt.json`): PASS, 3 editor scripts (Smoke/Load/WorldsPIE), 15 native game scripts, runpy-dict mutant refused, 0 saves. No `.pyc` written (the 3 in `__pycache__` date from 10-22) | OK |
| 14 | FB7 smoke | `driver.json` SMOKE_PASS, 31/31 bindings, module alive. Runtime.log: Python 2.7.14, `QUIT_EDITOR` (:711), `LogExit: Exiting.` (:1138), 0 Error/ensure/Fatal lines. Same editor argv family as the editor legs | OK (reader FAIL = G1) |
| 15 | FB7 FC4 switches | Both `-ini:EditorSettings…` switches in the real argv and on every leg's bound argv, the game legs included (B6); a missing one is refused (B2/B3). Logs: only `CrashClient.log:57 DataRouterUrl` (configuration). The -game leg is unproven at runtime | OK · O3 |
| 16 | FB7 FC5 / FF2 classes | 33 host Saved/Intermediate changes: `PackageRestoreData.json` mtime; 21 `AssetRegistryCache` + 9 coalesced configs + `CachedAssetRegistry.bin` mtime-only; + `AssetRegistryCache\ec29d66a.bin` (44 B). Equal to Codex's `managed-changes.json` (33) | OK |
| 17 | FB7 FD2 + FE3 | Real child: UBT 13868 `-Mode=ValidatePlatforms -allplatforms` (+ conhost 39276), 28 polls, no compiler, linker or UHT. Unreadable argv, implicit TargetList, Clean/Execute/Deploy, a build triple and dotnet-hosted Clean all stop (F1–F7) | OK |
| 18 | FB7 FD3 lines | Start 8 GiB commit / 6 GiB physical; session stop below 6 GiB commit (FB3 read as superseding FD3's 4). Smoke min 10.7 GiB | OK · O4 |
| 19 | FB7 FE1 AA | `-ExecCmds=r.PostProcessAAQuality 0` (SceneView.cpp:845-858 ⇒ AAM_None at quality ≤0). Runtime.log :242/:349 scalability sets 4 at startup; :700-701 `r.PostProcessAAQuality = "0"` at frame 1. No config edit | OK · O6 |
| 20 | FF1 | `final-state.json` `FF1_quarantine_equal: true` | OK |
| 21 | FF2 | D3/D6 (a Saved\Config ini stops; a WorldState json is recorded). Saved\Config 24 files hash-equal before and after | OK |
| 22 | FF5 | One full EA8 entry proof; `quick_check` between legs; full exit proof | OK |
| 23 | FF3, FF4 | 094-26d only | N/A |
| 24 | FE2 | 6 OS-denied event logs excluded as a class, recorded | OK |
| 25 | FE4, FE5, FD1, FD4–FD6, FC1, FC3, FC7–FC9 | Superseded or carried, as the request says | Carried |
| 26 | FC2 | `runtime_scripts.launcher` uses `imp.load_source`; the mutant fails | OK |
| 27 | FC6 | Row 17; Binaries 16/11 and plugin Intermediate 179 unchanged in name, size and time | OK |
| 28 | FA1 | 0 reparse points (Codex `final-state.json`; mine for CodeReview09427 and the Paris host) | OK |
| 29 | FA5 idle rule | Row 11 | OK |
| 30 | FA6 lean | Start and after quick checks only; no EK1 pre-scan, no P9 re-hash; Codex's full EA8 ran at entry and exit | OK |
| 31 | FA2–FA4, FA7, FA8 | FA7: 094-27 was pre-staged and launched after 094-26d's GREEN; others carried | Carried |
| 32 | EZ1 roots | C:/D:/E: 60/113/70 at start and after, names equal; 10 polls `roots=same`; Codex 0 attributions | OK |
| 33 | EZ2–EZ6 | Not exercised | N/A |
| 34 | EY4 / EV3 / EU2 disk | D: 70.48 → 70.0 GiB, E: 291.6 → 290.9, C: 30.97 → 30.8; no pause | OK |
| 35 | EY5 4.25 only | No 5.1 launch, build or write; 5.1 manifests 39/39 and 44/44 | OK |
| 36 | EY1, EY2, EY3, EY6–EY8 | Rehearsal per FB4; others carried | Carried |
| 37 | EX4 `-unattended` off | Present on every runtime argv (`runtime_policy.py command()`; real argv) | **GAP (G3)** |
| 38 | EX1–EX3, EX5, EX6 | EX2's note is now FB3 (row 3) | Carried |
| 39 | EW1 | 27/39/44 = 110/110 at start and after; the 4.25 manifest `c501895c…` is unchanged, 27 files | OK |
| 40 | ET2 EditorRuns | Only `31124`, 0 B, created 2026-10-26T18:43:59.9006068Z, empty hash, at start and after | OK |
| 41 | EA4 / EG1 / ET3 ledger | Charged C/E/5.1 = 0/0/0 (limits 250/50/100 MB). runtime-run: 20 `port_C_exception` (11 CrashReportClient `WindowsNoEditor\*.ini`, XmlConfigCache, NVIDIA DXCache, protect `Diagnostic.log`, 6 event logs), 1 foreign (Epic catcache), 54 port_D, 6 tooling | OK |
| 42 | EU1 command audit | Mine from `events.jsonl` after offset 53,608,463: 176 command starts, 175 via `filtered09427.ps1`, 1 direct read-only `Get-Content .\filtered09426d.ps1`, 0 direct write-capable; 30 file edits. Codex: 176 = 173 boundary + 2 pre-child refusals + 1 direct read-only | OK |
| 43 | EL2 canary | Fresh value (`canary.txt`, `641d19e1…`, equal to the job config's `canary_sha256_utf8`). Scan from the launch second: 2,011 files, 589 MB, 25 `.gz` decoded, 0 candidates, 0 matches, 0 errors, 0 child receipts naming `IA_CANARY_TOKEN`. Control: 3/3 matches (plain, UTF-16LE, gz), decoy not matched. Codex: 1,905 files, 0 matches. The respawned crash reporter's environment is unrecorded (G2) | OK · G2 |
| 44 | EA8 / P9 v2 | Codex entry and exit PASS (472,553 inventory, 1,023 EG1, 23,033 hashes, 104,794,989,456 B, protected git and source manifests equal). P9 v2 file `8e3ed095…` unchanged (mine) | OK |
| 45 | m51 / protected | m51 PASS ×3 (start 07:02, prelaunch 07:07, after 08:39) plus final 08:55; protected PASS start and after (release 8/8, BankOverflow 0 lost / 0 changed, StackOBot Content and cache Binaries equal) | OK |
| 46 | Launches | Exactly 1 `UE4Editor.exe` child receipt among 78 (`ledger\runtime-run\child-001.json`); 0 in dry runs and rehearsal; 0 UE-class processes after. My 10-minute polls never caught the 15-s session | OK |
| 47 | Commits / W4 | `599ef70` 07:55:51, `500e423` 08:22:00, `ccc861b` 08:27:52; author date = committer date = real time; no `origin/feat/europa-port`, no tags; worktree clean; 0 email, canary-shaped or token patterns in the diffs | OK |
| 48 | Crash folders | Paris `Saved\Crashes` unchanged (the two 10-27 folders); `%LOCALAPPDATA%\CrashReportClient\Saved\Crashes` and the engine's `Saved\Crashes` absent; no `ue4-crc-pid-*` left behind | OK |
| 49 | EN5 | No hard STOP reached; no exception used to pass the failed reader | OK |

**Count (49 rows):** 40 OK (rows 2–6, 14, 15, 18, 19 and 43 carry a GAP or note; row 12 has no pictures) · 1 OK as designed but NOT RUN (row 1) · 1 STOPPED (row 11) · 2 N/A · 4 Carried · 1 GAP (row 37 = G3). G1 and G2 sit in rows 4–6, 14 and 43. No ruling row is a FAIL; G1 is the kit's own blocking defect, and it stopped the run as FB7 requires. No protected-state change, no crash, no upload.

## 2. The real run, per leg
| Leg | Gate wait (IST) | Start – end (IST) | Duration | Exit | Result |
|---|---|---|---|---|---|
| Smoke | 08:07:20.6 – 08:08:18.9, 3 quiet polls (62.5 s) | 08:08:27.417 – 08:08:42.719 | 15.30 s (child 13.44 s) | 0 | Driver SMOKE_PASS (31/31); **reader FAIL "No skipped cleanup proof"**; run STOP |
| Load, WorldsPIE, WorldsGame, GC, 12 A7 legs, Admission | — | — | — | — | NOT RUN; slots never created |

Probe: 41 rows. Row 38 is `world_cleanup`, `actor_snapshot=skipped`, `skip_reason=engine_exit`; row 40 is `gc_restore_skipped`; row 41 is the single `probe_stop` (`root_release=skipped`, no outstanding screenshot). There are no PIE rows (world_type 3). Console lines match the catalog: `fixture_precheck.py:37`, `fixture_wait.py:45` and `:60`, and `runtime_runner.py`'s three prints.

## 3. Gates
| Gate | Reader | Mine |
|---|---|---|
| A3 editor load + compile-out | NOT_RUN | Agree. The smoke's `startup()` predicates pass on the real log (G5), but the Load leg never ran |
| A5 worlds and lifecycle (PIE and -game) | NOT_RUN | Agree |
| A6 reflected GC | NOT_RUN | Agree |
| A7 per anomaly (valid / applied / reverted) | NOT_RUN: blinking 0/0/0, lighting_mismatch 0/0/0, lod_corruption 0/0/0, missing_object 0/0/0 | Agree: no A7 probe rows exist |
| A8 non-package | NOT_RUN | Agree |

**Pictures:** none. `D:\IA_Europa\Receipts\09427\contact-sheet-index.json` lists 0 sheets and 0 pictures, and says so. The synthetic PNGs under `scratch\` are reader-test artifacts.

## 4. GAPs and observations
- **G1 (blocking, confirmed): the runtime reader's cleanup schema is invented.**
  - `runtime_readers.py:26` looks for `kind=='world_snapshot_skipped' and label=='cleanup'`.
  - The probe emits `world_cleanup` with `actor_snapshot="skipped"` (`EuropaPhaseAWorldProbe.cpp:211-225`). `Snapshot()` (`:293-301`) emits `world_snapshot_skipped` only for its own labels (`initialized`, `cleanup_before`, …), never `cleanup`.
  - So every real leg would have failed `clean_exit`. The retained `fixture_leg.py:32` already reads the correct shape.
  - The blind spot was structural: `runtime_standin.py:35` and `runtime_reader_tests.py:74` both reproduce the reader's own schema. This is G96's shape: no known answer from the real probe was used, although 094-26d's two real `probe.jsonl` files existed.
  - My G1–G4 show that on the real smoke files, the cleanup predicate is the only one that fails.
  - **Fix shape (for chat):** test every runtime reader against real probe files (094-26d's two, and this Smoke). Add Codex's missing, duplicate and unsafe-stop mutations, then a fresh arming. The spent 09427 kit must not be restarted.
- **G2 (GAP): the crash reporter is outside the owned tree.**
  - In 4.25 (`WindowsPlatformCrashContext.cpp:493-536`, `WITH_EDITOR`), the editor starts `CrashReportClientEditor`. That process respawns itself "to sever the link with the Editor process group", writes its PID to `%TEMP%\ue4-crc-pid-<editor pid>` (the editor deletes it), and exits.
  - Neither CRC process appears in any of the 38 owned-process polls. The respawned monitor (log PID 45840, `-MONITOR=2344`) logged `Editor/ExitCode:0` and exited on its own.
  - **Consequences:**
    - a deadline stop does not reach the respawned CRC, so the rehearsal's "including crash reporter children" holds only for a direct child;
    - its argv and environment are never recorded, so the canary-by-name check cannot cover it;
    - `UE4Editor.exe -game` is still a `WITH_EDITOR` binary, so the game legs will do the same.
  - **A candidate for chat, not a claim it works:** adopt any process whose command line names `-MONITOR=<an owned editor PID>`.
- **G3 (GAP): `-unattended` on every runtime argv, against EX4.**
  - EX4 ruled it off for the authoring kit. It appears in `runtime_policy.command()` and in the real argv. The design note neither cites nor justifies it.
  - **A plausible reason:** with `-ExecCmds=py`, EditorPythonExecuter's `GIsRunningUnattendedScript` is never set (`EditorPythonExecuter.cpp:145`), so dialogs could block. That reason is not established.
  - Chat should rule whether runtime legs may carry it.
- **O1: one failed leg stops the whole run.** Any reader or gate failure ends it (`runtime_runner.py`, reader raise ⇒ STOP; design :25). FB5 names only a crash as a STOP for the remaining legs, and the request's NEEDS-DECISION wording expects a gate to fail while others run. In the fixed order, one A7 failure leaves A8 unrun.
- **O2: the event budget is fixed.** Each A7 positive leg attempts exactly 6 events and needs ≥3 counted, plus 6 applies, 6 reverts and ≥2 refusals. FB3 lets a leg take further events within its limit; the kit never does. Four input-contaminated windows fail the leg.
- **O3: the -game opt-outs are unproven at runtime.** They are bound (B6). EngineAnalytics excludes `-game` at `:108`.
- **O4: the 6 GiB commit stop may bite.** FB3's 6 GiB in-session stop is adopted. The smoke's minimum was 10.7 GiB; two PIE clients and screenshots will use more, and the owner's normal free commit is about 12–14 GiB.
- **O5: an unreadable observation invalidates an event.** The smoke's last event observation failed with `CalledProcessError` at monitor close. Inside an A7 window, such a row marks the event invalid (E4).
- **O6: AA-off must be echoed per picture.** Scalability set `r.PostProcessAAQuality` to 4 at startup, and the ExecCmds override set 0 at frame 1. A7 pictures need the effective value echoed per picture (the design promises a native echo).
- **O7: `report.md` differs from `last-message.txt`.** They are 30,073 B and 29,857 B (`collect-receipt.txt`). I reviewed `report.md`.
- **O8: my canary control script crashed after the scan.** `canary_control27.py` (inherited from 26d) crashed on its own summary key after the scanner had written `canary-control.json`. I read the result directly: 3/3.

## 5. Top 5 risks for Phase B (six basic anomalies with 4.25-authored materials)
1. **Readers proven only by stand-ins (G1's class).** Every Phase B reader must be qualified on real 4.25 probe files and real pictures (a known positive and a known negative), never on rows written by the same author as the reader.
2. **Materials authored in 4.25 can fail silently.** A material missing a usage flag (skeletal, static lighting, instanced) renders default grey and still "applies" (G49 and G157 in this project's history). Each material needs a real-session pixel check on each mesh class. The plugin's 5.1 Content cannot load in 4.25 (FB1).
3. **The picture pipeline is unproven.** The screenshot delegate, the 1280×720 size, the effective AA-off echo, the fixed ROIs (predicted from coordinates, not measured) and blinking's phase selection all have their first real test ahead. A black or unlit frame reads like a null (G151). A calibration or luma leg before counted legs is cheap insurance.
4. **The run is fragile.** The runner stops on any failure (O1), the event budget is fixed (O2), and the window is effectively the night. One contaminated leg can cost a whole night of legs.
5. **The ownership gaps carry forward.** G2 (CRC respawn) and G3 (`-unattended`) carry into every editor and game leg, and the 6 GiB commit stop (O4) is close to this PC's normal headroom.
