# 094-19 — Claude Code review of the probe-fix/rebuild/research job (W3: written after Codex exited)

**Date:** 2026-10-27, 12:19–14:05 IST · **Author:** Claude Code (mailbox brief 094-19) · **Inputs:** Codex's report `_reviews\094-19-codex-report.md` (thread `01a14334`, resume 12:54:30–13:29:38 IST, 2,108 s, exit 0, 69 items, 46 commands; ledger row `094-01-europa-phase-a-plan-resume17` once), chat's `094-18-ruling.md` (ET1–ET7), `094-19-request.md` and the rulings they bind. My receipts: `code-receipts-094-19\` (and `step0\`, `protect\`, `ek1\`, `review\`), `D:\IA_Europa\Receipts\CodeReview09419\`, `D:\IA_Europa\Temp\CodeReview09419\`. No account identifier is reproduced.

## Summary
- **Codex stopped RED on two hard STOPs, both correctly raised, before the probe edit, any build or any research.**
  - **ED1 floor breach.** Its ledger operation `01-probe-fix` (content check, 13:12:45–13:13:05) read D: free **48,019,410,944 B (44.72 GiB)** against the 50 GiB floor (`child-failure.json`); my own poll read **44.65 GiB at 13:13:45**. EN5 lists "a floor breach" as a hard STOP; ET3 says ED1's floors apply whatever the attribution.
  - **One unfiltered child.** Its closeout command 35 ran `git -C <Paris worktree> status --porcelain=v1` directly from its shell with only `GIT_OPTIONAL_LOCKS=0`, so `git.exe` inherited the canary and `CLAUDECODE`. Codex self-reported it (`direct-git-boundary-stop.json`) and classed it under EN5's "secret or canary exposure". Read-only: the Paris index is unchanged in my after-snapshot.
- **The D: dips are transient and coincide with the owner's Blender renders; the writer is NOT established.** Observation: a second owner Blender (pid 47940, a background render to `E:\<personal-folder>`, started 13:10:01) joined Blender 50032 (7.03 GB commit), and D: free went 74.95 GiB (13:04:30) → 44.65 (13:13:45) → 62.51 (13:14) → 64.42 (13:15:29) → **74.64 after exit**. The system-managed page file (`init=0 max=0`) moved at the same time (45,097,156,608 B → 55,433 MB allocated at 13:14 → 56,066,400,256 B at 13:15:26 → back to exactly 43,008 MB after exit), but neither sample (Codex 52.9 GB, mine 56.1 GB) covers the whole ~30 GiB. **A second dip after the job settles it as unexplained:** D: read 54.49 GiB at 13:53:54 with the page file allocated at only 44,096 MB, while two more owner Blenders had started (13:38:17, 13:50:36); D: was back to 71.18 → 74.05 GiB by 13:59 as one of them exited, with the page file at 43.5–46.5 GB. Not ours or Codex's: my writes total ~0.1 GB, Codex's 0.45 GB, and no Steam download was in progress. Codex's own words ("cause not fully established") are the right ones.
- **Nothing was damaged.**
  - Probe source byte- and mtime-equal in all 15 files across three hosts.
  - Paris `Content` 1,453/1,453 and plugin `Content` 5/5 SHA-256-equal.
  - The 110 bound artifacts re-hash 27/27, 39/39, 44/44 (`4beccca6…`, `617a9082…`, `f6e52a85…`).
  - P9 v2 18,264/18,264 by my own re-hash, file `8E3ED095…` unchanged.
  - Protected state PASS ×3 (start, after exit, final); `m51` = P9 ×5 (start, dry run, launch, after exit, final).
  - 53 of 61 listings equal before/after, including the full 4.25 install, every 5.1 engine tree, all hosts, refs and worktree indexes.
  - Canary: 0 value hits in 121 changed files; 0 launches.
- **ET2 and ET7 are done and match my readings:** marker `31124` 0 B, created/written 18:43:59.9006068Z, empty-file hash, the only entry, still in place; both owner-slot `.tmp` files still locked (the launcher is still running), `ecache.bin` readable and clean.
- **The research was not done by Codex.** My own read-only source findings, spot-checked line by line, are in §5 as **input for chat, not as Codex's ET3/ET4/ET6 deliverable**.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | ET1 A6 owner half accepted | Not re-run; the 094-15 bound artifacts and the owner evidence are untouched | OK |
| 2 | ET2 marker receipt | Codex `ET2\marker.json`: 0 B, created/modified 18:43:59.900607Z, sha `e3b0c442…`, pid 31124, exit 3, disposition retained = my 12:26 and after-exit readings (`state-start.txt`, `state-after-exit.txt`); 4.25 install listing unchanged | OK |
| 3 | ET3 launcher research | Not started (hard STOP). My spot-checked findings in §5.1 | N/A (blocked) |
| 4 | ET3 launcher writes foreign; ED1 always applies | `ProgramData\Epic`: launcher/EOS logs and `InstalledItems` changed at 12:36 IST (foreign); ED1 STOP raised | OK |
| 5 | ET4 upload research | Not started. Mine in §5.2 | N/A (blocked) |
| 6 | ET5 probe fix incl. 5.1 GC-time lookup | Not applied; drafts only in the run folder (`fix_probe09419.py`, `probe_teardown_check09419.py`), never installed. All 15 host files unchanged | N/A (blocked) |
| 7 | ET6 texture research | Not started. Mine in §5.3 | N/A (blocked) |
| 8 | ET7 O4 temp files | Both `.tmp` files locked at 12:26, 12:39 and after exit (`0026e58f-…tmp` 0 B created 12:29:04; `761b7485-…tmp` 670,796 B); `ecache.bin` readable; Codex records the same and scanned `ecache.bin` (no hit) | OK (carried) |
| 9 | ET7 carries (O3, ER1 G3, ER3, 8.3/junction, EO1 budget, A9, EJ1/3/5) | Untouched | OPEN (carried) |
| 10 | ES1 / ES3 / ES5 | No re-run; no re-arm; kit `4b5b2812…`/`5d1d1ab5…` unchanged | OK |
| 11 | ES2 fix, static check, mutants | Not started; D2 budget unspent (Codex) | N/A (blocked) |
| 12 | ES4 six rebuilds, EI4, re-bind | Not started; 0 build actions; manifests not re-bound | N/A (blocked) |
| 13 | ER1–ER4 | ER1 G3 and ER3 carried; ER2/ER4 not exercised | OPEN (carried) / N/A |
| 14 | EO1–EO3, EO6 | No reader, build or makefile refresh | N/A |
| 15 | EO4 one probe source | 15/15 files equal across hosts and to my before-record | OK |
| 16 | EO5 closeout | Codex: EA8 exit REFUSED by the floor guard (not overridden, not claimed); since-launch canary scan; incremental EK1; ledger summary `STOP_INCOMPLETE`; no commit | OK (partial by design) |
| 17 | EN1 / EN2 / EN3 | No makefile regenerated (my post-scan: 0 makefiles changed) | N/A |
| 18 | EN4 my writes | Only my allowed places (§7) | OK |
| 19 | EN5 hard STOPs | Floor breach and canary exposure are both on EN5's never-covered list; no EN5 item applied | OK (STOP ×2, correctly raised) |
| 20 | EM2 `CLAUDECODE` | Present in Codex's base (`entry-environment.json`); absent from all 4 recorded children; **present in the unrecorded direct `git` child** (inherited) | GAP G1 |
| 21 | EL1 denylist at every boundary | 4/4 recorded python children PASS (base preserved, 0 tool/secret/canary names); the direct `git` call bypassed the wrapper | GAP G1 |
| 22 | EL2 canary | Positive control at launch (`launch-receipt.txt`, child sha = `187942a0…`); Codex base `canary_matches_launch` true; 0 value hits / 0 canary-shaped strings in 121 files changed since 12:54:29 (14 roots); 9 name-only hits (receipts, scripts, events); self-test 8/8 | OK (value) / G1 (child) |
| 23 | EL3 / EL4 / EL5 | A5 done / kit not run / wrapper removed `CLAUDE_CODE_MESSAGING_TOKEN` only | N/A / OK / OK |
| 24 | EK1 secrets | Pre-scan 40,777 files 0 values, 1 locked file; post-scan 143 files 0 values (1 read error = my own open console); scanner self-test 17/17 | OK |
| 25 | EK2 / EK3 | No makefile / `GIT_OPTIONAL_LOCKS=0` in every git call incl. the stray one; m51 and Paris indexes unchanged | N/A / OK |
| 26 | EI2 memory gate | Never reached a build admission. Codex reported 3.6 GiB physical / 15.7 GiB commit free (below 4/16) before the STOP | N/A (observed below gate) |
| 27 | EI3 / EI4 | No build, no query | N/A |
| 28 | EG1 engine trees | 5.1 `Intermediate\Build`, plugin `Intermediate\Build`, Binaries, BuildRules, Programs\UBT, UbtRuns, Saved: all equal before/after; Codex's partial 5.1 after-inventory 339,410 equal | OK |
| 29 | EG2 receipts | No build | N/A |
| 30 | P9 v2 / protected state / m51 | My re-hash 18,264 OK, file `8E3ED095…` at start, 13:4x and final; protected PASS at 12:26, after exit and final (13:53); m51 PASS at start, dry run, launch, after exit and final; can-fail 5/5 | OK |
| 31 | Launches | 0 launch-class processes of ours in 4 polls; 46 commands: 0 kill, 0 delete, 0 git-write words; the two launch-word hits are process-list queries | OK |
| 32 | Commits / push / tag / W4 | None; refs and worktree heads equal before/after | OK |
| 33 | Ledger row | `…-resume17` exactly once | OK |
| 34 | Self-test suites | §6 | OK |

**Count: 17 OK · 13 N/A · 1 GAP (G1, rows 20–22) · 2 OPEN carried (rows 9, 13) · 2 STOP (correctly raised) · 0 stop violations · 0 conflicts.**

## 2. The floor STOP in detail
- **Codex's record:** ledger op `01-probe-fix`, `before.json` 13:12:45, children 001–004 (python, all boundary PASS) 13:12:46–13:13:05, `child-failure.json` 13:13:05: `ED1 floor/headroom STOP` with D free 48,019,410,944 B, floor 53,687,091,200 B; the after-snapshot also refused (47,928,963,072 B). `write_ledger.py:34–40` checks the start figures at an operation start and the floors every 20 s during a child run (`:453–462`).
- **My series** (`poll-log.txt`, `midjob-notes.txt`): D 75.10 (12:25) · 74.95 (13:04:30) · **44.65 (13:13:45)** · 62.51 (13:14) · 64.42 (13:15:29) · 58.91 (13:24:53) · 74.58 (13:29:41) · 74.64 (after exit). Page file: 45,097,156,608 B (11:39) → 55,433 MB alloc, 9,098 MB in use (13:14) → 56,066,400,256 B (13:15:26) → 43,008 MB alloc, peak use 10,203 MB (after exit).
- **Second dip, after Codex exited (my readings):** 54.49 GiB at 13:53:54 (below ED1's 55 GiB warning), page file 44,096 MB allocated; 71.18 / 73.30 / 74.05 GiB at 13:59:03 / :23 / :43, page file 46,494 / 44,327 / 43,562 MB; owner Blenders 28012 (13:50:36, exited by 13:59), 33420 (13:38:17) and 50032 running. So the page file does not account for the dips; the writer is not established (no Steam download; my and Codex's folders are small).
- **What it means:** the dips track the owner's Blender renders and recover when a render ends. Paging settings are machine settings and may not be changed (EN5). D: starts each job only ~5 GiB above ED1's 70 GiB start figure, so the only levers are the owner's workload or more free space on D:.

## 3. The unfiltered `git` child (G1)
- Command 35 (my `review\cmd-audit19.json`): a process query, a read of `content-stop-check.json`, a free-space read, then `$env:GIT_OPTIONAL_LOCKS='0'; git -C 'D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector' status --porcelain=v1`. No other git call in the job; 0 git writes.
- **Effect on state:** none. The Paris worktree index (hash, size, mtime) and status are equal before/after; refs equal.
- **Effect on the canary:** the value went into `git.exe`'s environment (inherited) and, by my search, into no file.
- **For chat:** every Codex shell command is itself a child of `codex.exe` carrying the canary (that is how Codex reads it); prior jobs kept every *tool* child (git 78 in 094-15) behind the ledger's filtered `run_child`. This job is the first where a tool child bypassed it. Codex's classification (hard STOP) is the strict reading of EL1/EL2/EN5.

## 4. The fix and the rebuilds
Not started. I diffed nothing because nothing changed: all 15 probe files equal `probe-source-before.tsv` (the 094-18 record). **Codex's unapplied draft, read at a glance for the next job:** its static check refuses `TActor(Iterator|Range)<` and `FindObject*/StaticFindObject*/LoadObject*/StaticLoadObject*` with five mutants (iterator, range, `FindObject`, `StaticFindObject`, `LoadObject`) inserted into `Cleanup`, plus a lookup hidden in a helper, a removed GC guard and a class-capture called from `Cleanup`; the draft fix caches the control/bench classes as weak pointers, records `engine_exit`/`garbage_collection` skip rows (`IsEngineExitRequested() || GExitPurge`, `IsGarbageCollecting()`) and walks `World->GetLevels()`/`Level->Actors` with null/unreachable/pending-kill counters. Remaining teardown paths it must also cover (my pre-fix read): `SnapshotState`'s component and material reads, `Stop()`'s `ReleaseGC()` and `Dormant` raw-pointer `RemoveFromRoot()` at module shutdown.

## 5. Research — my own read-only findings (spot-checked by me in 4.25 source; input for chat, not Codex's deliverable)
### 5.1 ET3: what starts the launcher
- `Editor\UnrealEd\Private\EditorEngine.cpp:607–632`, `UEditorEngine::InitEditor`, right after `UEngine::Init` (`:610`): unless `-ForceLauncher`, it calls `LauncherPlatform->OpenLauncher(SilentOpen)` (`:626–630`; `FOpenLauncherOptions` `bSilent(true)`, `ILauncherPlatform.h:10`) when **all** hold: not an internal build, not a Perforce build (`EngineBuildSettings.cpp:9,16`: existence of `Build/NotForLicensees/EpicInternal.txt` / `Build/PerforceBuild.txt`), no debugger, not `FApp::IsBenchmarking()`, not `GIsDemoMode`, not a commandlet, and **`EpicGamesLauncher` not already running** (`:622`).
- No dedicated switch exists. `-DEMOMODE` (`LaunchEngineLoop.cpp:2480`) stops it but changes ~20 editor behaviours (autosave, screen messages, save/delete prompts, actor and scene-component paths, PIE error dialogs; every `GIsDemoMode` use listed in my notes). `-BENCHMARK` changes the timestep. A marker file under `Engine\Build` is an engine-tree write (EN5 STOP).
- **My pick:** for owner legs, the `:622` gate: the owner starts the launcher themselves before the leg, so the editor spawns no child launcher and nothing inherits our TEMP/environment; the launcher's own writes stay foreign under ET3 and ED1 still applies. No switch, no setting, no engine edit. For unattended legs there is no clean option; `-DEMOMODE` only if chat accepts its side effects.

### 5.2 ET4: uploads
- The crash-reporter monitor starts unconditionally from a static global (`WindowsPlatformCrashContext.cpp:1390`, `:1106–1111`).
- Ensures report in unattended mode by default (`:1524`; `ExceptionHandling.cpp:31,47–50`). Crashes report with `ShowDialog` (`:1298–1309`).
- `ReportCrashForMonitor` reads `[/Script/UnrealEd.CrashReportsPrivacySettings] bSendUnattendedBugReports` from `GEditorSettingsIni` (`:603`; default `True`, `BaseEditorSettings.ini:9–10`) and returns at `:624–633` — before anything is written to `Saved\Crashes` or sent to the reporter — when the report has no dialog and that setting is false.
- **Per-invocation and unsaved:** `-ini:EditorSettings:[/Script/UnrealEd.CrashReportsPrivacySettings]:bSendUnattendedBugReports=False`. The `-ini:` parser is `ConfigCacheIni.cpp:1100–1200`, compiled in non-Shipping (`ConfigCacheIni.h:27`), applied when every global ini is generated (`:3300`, EditorSettings loaded at `:3873`), and **`FConfigFile::Write` skips command-line values** (`:1531`, `:1543 … && !bOptionIsFromCommandline`), so the C: per-user `EditorSettings.ini` (today it has no privacy section) is not changed.
- **My pick:** that `-ini:` switch (stops every ensure upload) + the card's "Close Without Sending" for a crash; add `-unattended` only for unattended legs (it makes the crash dialog-less so the same `:624` return applies, but changes dialogs editor-wide; `App.cpp:234` not read by me).
- Side note: `C:\ProgramData\Epic\NotAllowedUnattendedBugReports` (0 B, since 2023-04-11, rewritten by the launcher at 10:38 and 12:36 IST) is read by no 4.25 Core, CrashReportCore or UnrealEd source; the ensure was uploaded anyway. Machine-wide; leave it alone.

### 5.3 ET6: the texture ensure
- `TextureDerivedDataTask.cpp:249–271`: in the non-virtual-texture branch, `ensure(TextureData.Blocks.Num() == 1)` (`:252`) only reports; the build compresses **`Blocks[0]` only** (`:266`) and continues. A multi-block source (UDIM; sub-agent cites `EditorFactories.cpp:4221` → `:3873`, not read by me) is therefore built from its first tile only, which can change how that texture renders in pictures (inferred).
- **Which texture is not established.** The ensure's stack has no symbols (`Runtime.log:5165–5174`, all `UnknownFunction`) and every line 5153–5201 carries one flush timestamp (`18.45.10:362`); the reporter's own log puts the ensure at 18:44:39 UTC. The `Building textures:` line (`:209`, logged at `:43`) is written per build on worker threads, so "the closest preceding line" (`T_Cassadibirra_M`, `T_Grayscale_M`, `T_Vetro03_B`…) is not an identification. 562 builds logged.
- **My pick:** identify the asset read-only before A7 (its source-block count, not the log), and choose A7's shot so no surface uses it, rather than enabling virtual textures (a host change with shader recompiles). The DDC on D: now holds that texture's single-tile build, so a later run may not show the ensure at all (inferred).

## 6. Self-test suites (re-run by me)
`owner_eq_self_tests.py` **159/159** (harmless children only, 0 UE launches), `owner_kit_self_tests.py` **62/62 + 48 measured** (0 UE launches), `idle_gate.py --self-test` **48/48** — copies changing only the fixture TEMP base, under `CodeReview09419`. EA11 `check_reflected_headers.py --self-test`: Paris **PASS** (7 headers, mutant detected), ReGate51 **PASS**, release control Baseline51 `d7eee97` **FAIL, 12 violations** (expected pre-port: a real-data can-fail). My own: EK1 scanner 17/17, canary search 8/8, EN1 checker 17/17, m51 gate can-fail 5/5.

## 7. Writes, snapshots
- Mine: `code-receipts-094-19\`, `resume-prompt-094-19.txt`, the preserved `094-18-*` copies (hash-equal to `code-receipts-094-18\`), `_reviews\094-19-codex-report.md` (body byte-identical, `F2307C42…`), this file; in `D:\IA_Europa` only `Receipts\CodeReview09419\` (EA11 outputs, self-test roots) and `Temp\CodeReview09419\` (pre-fix probe copies, harness). I killed nothing, deleted nothing, launched nothing, and ran no build.
- Before → after (61 listings): 53 equal. Different: free space, processes, root mtimes (C:\ClaudeTemp; E:\<personal-folder>, <personal-folder>, MailBox, SteamLibrary; D:\pagefile.sys), `ProgramData\Epic` (launcher/EOS, foreign), tooling sizes, and `D:\IA_Europa` +49 files, all under Codex's `Receipts\09419` (0 changed, 0 removed elsewhere).
- Final canary pass over my own writes since the launch second (`_reviews`, the run folder, both `CodeReview09419` folders; `review\canary-search-final.json`): 342 files, **0 value hits**; the 2 other canary-shaped strings are my self-test fixture's synthetic values (`canary-selftest-fixture\lookalike.txt`, `v8*.txt`, `v16.bin`).
- Still running (owner's): the kit console `cmd.exe` 44364 at its pause prompt; the Epic Games Launcher 26116 with its helpers; Blender 50032 and the render 47940.

## 8. GAP and OPENs
- **G1 (EL1/EL2/EM2):** one direct read-only `git status` from Codex's shell inherited the canary and `CLAUDECODE`. No file carries the value; no state changed. Chat's call: whether a tool child of Codex's own shell is a ruled boundary (Codex says yes, a hard STOP), and how the next job guarantees every tool child goes through the filtered wrapper.
- **O1:** the writer behind the D: dips is not established (the page file does not account for them; they coincide with the owner's Blender renders and recover when one ends).
- **O2:** the ET7 temp files stay unverified while the launcher holds them.
- **O3:** all ET3/ET4/ET6 research, the ES2/ET5 fix, ES4's rebuilds and re-binding remain to do; the D2 budgets are unspent.
- **Carried:** ER1 G3, ER3 bare-UBT, 8.3/junction tests, EO1 reader budget, A9 probe control path, EJ1/EJ3/EJ5, ET7's O3 package-output absence check.

## 9. Top 5 risks for the next runtime leg
1. **D: headroom under the owner's Blender renders.** D: starts ~75 GiB against ED1's 70 GiB start and 50 GiB floor, and twice today it fell 20–30 GiB within minutes while extra owner renders ran (writer not established), recovering when they ended. Any job (build or runtime) can STOP again unless the owner pauses the background renders, or frees D: space, before it starts.
2. **Memory at the EI2 gate.** With two Blenders, commit free sat at 12–16 GiB (start needs 16) and physical at 1.4–4 GiB (needs 4); builds can wait up to EF1's 4 hours.
3. **The exit crash is still in every host's probe binary.** The fix and the six rebuilds are not done, so A7 must not run on the current binaries (ES2: a crash is a STOP); 5.1 adds the GC-time `FindObject` (`WorldProbe.cpp:237`).
4. **Uploads and the launcher are unruled.** Without §5.2's switch every ensure uploads automatically with the full log; without §5.1's choice the editor re-spawns the launcher into our TEMP and environment.
5. **A tool child outside the filtered wrapper.** The canary STOP can recur from a single direct call; and the UDIM single-tile build may silently change a texture in A7's pictures.

VERDICT: REVIEW DONE — 17 OK / 13 N/A / 1 GAP (G1 direct git child inherited canary+CLAUDECODE, read-only, no file) / 2 OPEN carried / 2 STOP correctly raised (ED1 D: floor 44.72 GiB during an owner Blender render, writer not established, recovered after; EN5 canary exposure) / 0 stop violations / 0 conflicts; probe fix, six rebuilds, re-binding and Codex's research not started; ET2 and ET7 done and equal to my readings; probe source 15/15, content 1,453+5, bound artifacts 27/39/44, P9 v2 18,264/18,264, protected state PASS, m51 = P9 throughout, 53/61 listings equal, canary 0 value hits in 121 files, self-tests 159/159, 62/62+48, 48/48, EA11 PASS ×2 + control FAIL as expected; 0 launches; my own spot-checked ET3/ET4/ET6 findings in §5
