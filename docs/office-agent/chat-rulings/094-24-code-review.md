# 094-24 — Claude Code review of the EY1–EY4 candidate and its rehearsal (W3: written after Codex exited)

**Date:** 2026-10-28, 21:37–23:10 IST · **Author:** Claude Code (mailbox brief 094-24) · **Inputs:**
- Codex's report `_reviews\094-24-codex-report.md`: thread `01a14334`, resumed 17:49:47–21:36:56 IST, 13,629 s over 2 tries, exit 0. Try 1 ended at ~20:52 on "Error running remote compact task: Selected model is at capacity"; the relay waited 10 min and resumed at 21:02:27. Ledger row `094-01-europa-phase-a-plan-resume22`, recorded once. **Codex's verdict: `NEEDS-DECISION | ED5 STOP at E:\OBSVideos; 09424 not re-armed; zero Unreal launches.`**
- Chat's `094-23-ruling.md` (EY1–EY8), `094-24-request.md` and the rulings they bind.

**My receipts:** `code-receipts-094-24\` (with `step0\`, `protect\`, `ek1\`, `review\`), `D:\IA_Europa\Receipts\CodeReview09424\` and `D:\IA_Europa\Temp\CodeReview09424\`. No account identifier is reproduced.

## Summary
- **Codex stopped on ED5 before re-arming.** At 19:42:43 a new top-level folder, `E:\OBSVideos`, appeared between two of its ledger operations (`D:\IA_Europa\Receipts\09424\ledger\04-rehearsal-resume\between-operations.json`: 58 changes foreign, 1 `STOP`, basis "ED5 new or removed root folder"). Codex recorded the writer as not established, stopped implementation, and closed out. **No 09424 arming exists; the unchanged 09423 config is still at `Launch\Europa425-FixtureAuthoring.json` (`b1daf567…`).**
- **The writer is the owner's OBS Studio (my read-only evidence, `review\obsvideos-writer.txt`):** `obs64.exe` pid 21492, parent `explorer.exe`, started 19:35:14 IST from `E:\Program Files\obs-studio`; its profile reads `FilePath=E:/OBSVideos`; the folder holds three `.mp4` recordings created 19:43:37, 19:44:29 and 19:49:08. Not Codex, not me, not the kit. ED5's can-fail makes any new root folder a STOP regardless of writer, so Codex's STOP follows the rule as written (D1).
- **What Codex did build is uncommitted** in the Paris worktree on `feat/europa-port` at `97de9d4`: 11 modified and 4 new tooling files (my patch `review\diff-97de9d4-worktree.patch`, 172+/57− plus `environment_reads.py`, `fixture_ey_tests.py`, `fixture_rehearsal.py`, `fixture_standin.py`). No commit, push or tag. Because the bound tooling changed in place, the 09423 config's 11 tooling rows no longer match, and Codex's full EA8 exit refused by design (clean-owned-checkout).
- **On the candidate itself, my independent checks agree with Codex and go further:**
  - **EY1 (G1) is fixed.** The variable is gone; `fixture_leg` derives `<state>\authoring-started.json` from the guard. Through the **real `.bat`** in my own scratch, the supervisor recognised each recipe's marker (`EX2_session_floor_policy` recorded, marker `wrapper_pid` = the helper), and no environment carries the name.
  - **My rehearsal, 5 of 5 cases PASS through the real branch** (§3), including the two Codex never completed (missing `probe_stop`, deadline) and a crash-reporter-named child.
  - **EY2 live:** on 3 polls the kit's folder-based exemption equals my independent walk (9 PIDs: launcher 26116, web helpers 43312/43852/46296, overlays 39596/35088/44568/40516/4420; folders `…\Epic Games\Launcher` and `…\Epic Games\Epic Online Services`), 0 blockers. EY2/EY3 can-fails 29/29.
  - **EY3** 60 s rule 12/12 (clock-injected, mine); **EY4** E: start 15 GiB passes at exactly 15 GiB and stops at 15 GiB − 1 B.
- **One GAP for the re-arm (G2):** the retained 09417 EQ suite (`owner_eq_self_tests.py`) now fails under the candidate: its synthetic reader dict lacks the new `commit_memory` reader, so `collect()` records an extra reading error (`<key>_other_readings_retained`). Codex did not run this suite on the candidate. Test-side, not a live defect, but the suite must pass before any re-arm.
- **Everything protected is unchanged:** 110/110 bound files (`c501895c…`/`b05e6f8c…`/`3865fe0c…`), Paris Content 1,453 + 5 byte-equal to my start list, probe source 15 files, 31124 marker, P9 v2 18,264/18,264 (file `8e3ed095…`), m51 = P9 (start, prelaunch, after exit, final), protected state PASS (start, after exit, final). 0 Unreal launches. Canary: 0 value hits; 0 by name in child environments.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | EY1 G1 fix: drop `EUROPA_AUTHORING_MARKER`, derive the marker | Diff: `fixture_leg.py` reads `canonical(guard.state/'authoring-started.json')`; `disk_pause.supervise` no longer sets the variable and still validates the same path from `--authoring-marker`. Through the real `.bat` (§3, pass-both): `EA_Core-disk.json`/`EA_Travel-disk.json` record `EX2_session_floor_policy` with the derived marker path; markers authority `094-22-ruling EX2`, phase `authoring_started`, `wrapper_pid` = helper; no environment holds the name | OK |
| 2 | EY1 environment-read check + mutant | `environment_reads.py`: 56 helpers, 16 named reads, PASS; its 11 self-tests pass; `fixture_leg.py` now has 0 named reads. My mutants on a copy of `fixture_leg.py` (`review\envread-mutants24.json`): restoring the G1 line, `.get`, `from os import environ`, a constant name and a dynamic name all FAIL the check (5/5); a surviving-name control PASSES. **But four forms escape** (copy-then-read `env=dict(os.environ); env[N]`, `os.environ.copy()[N]`, `getattr(os,'environ')[N]`, `os.environ.__getitem__(N)`). No kit helper reads a named variable through a copied mapping today | OK · O1 |
| 3 | EY1 real-branch rehearsal, passing and failing stand-ins | Codex: 4 cases complete (pass-both, partial-core, partial-travel, extra-core); missing-stop refused once at D: 37.31 GiB, then ED5 stopped its resume; deadline never ran. Mine (§3): pass-both, extra-core, missing-stop, deadline+sibling, crash-name child — **5/5 PASS** | OK |
| 4 | EY1 rehearsal confinement | `fixture_rehearsal.activate()` admits only `Receipts\{09424,CodeReview09424}\rehearsal\<case>` with `Temp\<same>\host`, synthetic manifests and host/Content/config rows under that TEMP, a 2–30 s deadline; `validate()` refuses rehearsal roots outside test mode and a rehearsal process switching to the real kit. My `.bat --rehearsal` against the live config is not needed: the live config has no `rehearsal` key, so `activate()` refuses it. Writes: Codex's under `Receipts\09424`/`Temp\09424`, mine under `CodeReview09424`; Paris host listings (Binaries, Intermediate, Source, Config, Content, uproject) equal before → after; real 09423 and 09424 slot paths absent | OK |
| 5 | EY2 exemption by install folder | `idle_gate.launcher_folders()` derives the Launcher folder from the live launcher image (`…\Portal\Binaries\Win64`) and adds the sibling `Epic Online Services` folder; `launcher_tree()` exempts only images under one of them, with ancestry to the launcher, not in `BLOCKING_IMAGES`, not a game. Live 3/3 polls equal to my independent walk, 0 blockers (`review\ey2-check24.json`). Can-fails 15/15 (editor under the launcher with engine or Launcher-folder path; editor or overlay under a look-alike launcher at a non-launcher path; unreadable `None`/empty/relative path; `..` escape; look-alike folder prefix; game exe under the Launcher folder; overlay outside the tree; unreadable launcher path makes overlays block; web helper with unreadable path stays recorded-only) | OK · O5 |
| 6 | EY3 60 s unreadable acting reading | `SessionGate`: per-reading timer for commit and C/D/E, ≥ 60 s ⇒ stop; reader exception or non-dict ⇒ unreadable. Mine 12/12 (`review\ey3-60s24b.json`; the 10/12 file `ey3-60s24.json` was my harness bug, kept): 59.9 s no stop; stop at exactly 60 s (commit, C, wrong `resolved_drive`, NaN, negative, raising reader); a readable reading resets; alternating gaps do not stop; short gaps written as `record` | OK |
| 7 | EY3 UnrealPak / UnrealLightmass block; six images recorded only | Both block outside the tree and inside the Launcher folder and tree; CrashReportClient(Editor), UnrealInsights, UnrealTraceServer, UnrealVersionSelector, UnrealCEFSubProcess, EpicWebHelper never block (mine, in the 29) | OK |
| 8 | EY3 12 GiB free-commit start line + Waiting line | `reasons(profile='measured')`: 12 GiB − 1 B ⇒ `commit_memory`, "Waiting: 12 GiB free commit."; unreadable ⇒ "Waiting: 12 GiB free commit (reading unavailable)."; not applied to the owner-preview profile. Template memory now 6/12/None/6, accepted by `job_scope` only for EY1–EY4 fixture authoring | OK |
| 9 | EY4 E: 15/12/10 in kit and entry checks | `disk_pause.LIMITS` and `write_ledger.floors()` E (15,12,10); `idle_gate.FLOORS` E 10 GiB. `floors(start=True)`: E = 15 GiB PASS, 15 GiB − 1 B / 12 GiB / 10 GiB − 1 B STOP (`review\ey4-floors24.json`) | OK |
| 10 | EY5 4.25-only scope | No build; frozen 5.1 hosts untouched (worktrees clean at `d7eee97`/`40a8eaf`, listings equal); manifests unchanged | OK |
| 11 | EY6 order: fix, rehearse, re-arm 09424, dry runs, live reading | Fix and part of the rehearsal done; **re-arm, armed dry runs and Codex's live reading not reached (ED5 STOP)**; no commit | **NOT DONE (STOP)** |
| 12 | EY6 re-arm scope = EW2; 09424 slots unused; 09423 refused | No arming written. 09424 real `fixture-state`/`fixture-attempts` absent; 09423's 15 real paths absent. My real `.bat` dry run on throwaway roots: `Stopped: Fixture arming is not the 09424 EW2/EW3 arming amended by EX1-EX5 and EY1-EY4. No further session will be started.` (exit 2, 0 launches) | Partly N/A · OK |
| 13 | EY7 094-25 readiness | Not ready: no armed kit | N/A |
| 14 | EY8 carried | EO1 budget, EV6 glass, EJ5, EX2 note, O1 (clean exit at runtime), O2b | OPEN (carried) |
| 15 | EX1–EX5 as amended | EX2 owned-tree stops (commit, E: floor, deadline) re-proven on the candidate in my 094-23 set; EX3 quarantine proven in every failing rehearsal case; EX4 argv mutants and EX5 wording in my sets. Expected deltas only (§4) | OK |
| 16 | EX6 / EW1–EW4 | EW1 manifests 27/39/44, all bound files equal; EW2 scope unchanged in source (`SCOPE`); EW3 argv/locks/aliases 42/46 in my 094-22 set, the 4 deltas from EY2/EY3 sample shape (§4) | OK |
| 17 | EV1–EV7 | EV1: 0 EU2 pauses; EV2: 3.8 h; EV3 D: lines unchanged; EV5 probe source 15 files identical across 3 hosts; EV6 carried | OK |
| 18 | EU1 command audit | 118 commands: 117 through the wrapper, 1 direct read-only (`Get-Content` of the ruling and wrapper scripts), 0 direct write-capable; 19 file-edit items (`review\eu1-audit.json`). Codex reports "118 filtered, 1 direct read-only" — a one-count labelling difference, same conclusion | OK · O4 |
| 19 | EU2 foreign dips | D: dipped to 37.1–43 GiB 19:15–19:35 (foreign) and recovered; E: fell to 12.69 GiB at 20:47 (owner, <personal-folder>/SteamLibrary/OBSVideos), then rose to 313 GiB at 22:00. No floor breached, no pause | OK |
| 20 | EU3–EU6 | EU4 switch in the stand-in argv (`--mode … UPLOAD`); others not exercised | OK / N/A |
| 21 | ET1–ET7 | ET2 marker `31124` unchanged (0 B, only entry); ET3 `ProgramData\Epic` rotation foreign; ET7 owner-slot `.tmp` unchanged | OK |
| 22 | ER1–ER4 | ER1: the 09423 refusal names its reason; every failing rehearsal case printed its own `Stopped:`/reason | OK |
| 23 | EQ1–EQ5 | EQ1: unreadable commit at start waits. **The EQ suite itself fails under the candidate (G2)** | OK · **GAP G2** |
| 24 | EP1–EP3 | — | N/A |
| 25 | EO1–EO6 | EO3 6 makefiles decode, unchanged; EO4 static 33 × 4; EO5 Codex's entry PASS, full exit refused (dirty candidate) | OK |
| 26 | EN1–EN5 | EN5: Codex applied no exception; ED5 kept as a STOP | OK |
| 27 | EM2 | `CLAUDECODE` in the launch base; 0 tool names in 54 child receipts | OK |
| 28 | EL1–EL5 | 54 child receipts, 0 bad, 0 `EUROPA_*` in execution children, 7 supervisor→helper environments with no secret-class name (G1's echo gone); my wrapper removed only `CLAUDE_CODE_MESSAGING_TOKEN` | OK |
| 29 | EL2 canary | Positive control: child sha = generated `e1489f2e…`. By value from the launch second: 1,374 files after Codex's exit and 2,652 at the end, 0 hits (§7). By name in child environments: 0 | OK |
| 30 | EK1–EK3 | Full scan 48,023 files, 0 secret values, 0 live-value matches, 0 git date values, makefiles 6/6, 4 read errors (2 ET7 `.tmp`, 2 locks held open by my running rehearsal). Name-hit files 338 → 343, all 5 added are my review files (names only) | OK |
| 31 | EI2 / EG1 / EG2 | No build; 4.25 install and every 5.1 tree equal before → after | N/A / OK |
| 32 | ED5 new root folder | STOP raised on `E:\OBSVideos`; writer attributed by me to the owner's OBS (D1) | **STOP (foreign) · D1** |
| 33 | P9 v2 / protected / m51 | P9 v2 18,264/18,264; protected PASS ×3; m51 = P9 ×4 | OK |
| 34 | Launches | 0 Unreal processes during the job (22 polls, `launch_class_ours=0` and `foreign_launch_class=0` on all, no sightings file). My review started harmless stand-ins only (Python, and a `PING.EXE` copy named `CrashReportClient.exe` in my scratch) | OK |

**Count (34 rows):** 28 OK (some partly N/A) · 2 N/A · 1 OPEN (carried) · 1 NOT DONE · 1 STOP (foreign, D1) · 1 GAP (G2) · 0 protected-state changes.

## 2. The candidate (diff) in brief
- **EY1:** variable removed in `disk_pause` and `fixture_leg`; `environment_reads.py` added; `fixture_ey_tests` asserts the name appears in neither file.
- **Rehearsal hooks in the production path:** `--rehearsal` on `fixture_entry`, `fixture_leg` and the `disk_pause` routing check; `fixture_policy.REHEARSAL` switches HOST, OUTPUTS, manifests, command, P9 preflight and `check_ddc` to scratch. They activate only with a config that carries the EY1 scratch contract.
- **EY2/EY3** in `idle_gate.py` and `fixture_session_gate.py`; **EY4** in `disk_pause`, `write_ledger`, `idle_gate`.
- `write_ledger.run_child` no longer calls `floors()` while the runtime session monitor is active, and `disk_pause` stops reading drives once the session has started: in-session disk limits now live only in `SessionGate` (consistent with EX2).
- New arming constants: `ARMING_JOB='09424'`, amendment citations EX1–EX5 + EY1–EY4, session policy with `start_commit_GiB 12` and `unreadable_seconds 60`, template memory 6/12/None/6.

## 3. My real-branch rehearsal (`review\rehearsal-cr24-all.json`)
Driver `review\rehearse_cr24.py`: Codex's case builder with every root moved under `Receipts\CodeReview09424\rehearsal\<case>` and `Temp\CodeReview09424\rehearsal\<case>`, template read-only from `Receipts\09424\job-config.json`. Cases 1–4 run the real `cmd /d /c Europa425-FixtureAuthoring.bat --config … --rehearsal` (entry → `disk_pause` → non-dry `fixture_leg.main()`).
| Case | Result | Key evidence |
|---|---|---|
| pass-both (Codex's, re-run) | PASS, exit 0, 1,589 s | Both `AUTHORING_PASS`, both "clean exit and output checks passed; this recipe is now spent.", one `probe_stop`, markers + `success.json`, both locks spent, 0 `EUROPA_*` in the child |
| extra-core | PASS, exit 2 | `Authoring wrote missing or undeclared Content files.`; declared outputs quarantined by receipt; EA_Travel not started; lock spent |
| missing-stop | PASS, exit 2 | `Clean exit needs an engine-exit skipped world_cleanup and exactly one final probe_stop.`; quarantined; EA_Travel not started |
| deadline (3 s) + sibling | PASS, exit 2 | `Session limit reached; stopped only the owned editor tree, including crash reporter children.`; stopped 5800 and its detached child 4132 (gone afterwards); sibling 8928 alive |
| crash-reporter-named child | PASS, exit 2 | Real `fixture_leg.main()` non-dry branch in a fresh process (not the `.bat`), stand-in replaced by mine: it spawned a detached `PING.EXE` copy named `CrashReportClient.exe` (18420), stopped with the tree; an identically named sibling outside the tree (42584) survived, so the stop is by tree, not by image name |

Not exercised by any rehearsal (disclosed by Codex too): the real P9 admission (`owner_p9 entry-fixture`), the real argv (`prepare_runtime`), `check_ddc`, and live start/acting readings (synthetic in rehearsal). The printed "limit 1800 seconds" line is fixed text even when the rehearsal deadline is shorter (O2).

## 4. Self-tests and my sets
- **Codex's suites re-run on the candidate** (`review\rerun-selftests24.json`): EY 46/46; environment reads PASS (56 helpers, 16 reads, 11 self-tests); fixture 89/89 (33 static); owner kit 62 + 50 measured; idle_gate PASS; disk_pause PASS; static 33 × 4. Session suite failed one `EX2_*_not_delayed_by_collector` (< 8 s) check while my scans loaded the disk; **57/57 on a quiet re-run** (`session-tests-retry24b.json`) (O3). **EQ suite: FAIL (G2).**
- **My 094-23 set:** 43/57; all 14 deltas expected: 11 EX1 cases built from samples without image paths or a commit reading (EY2/EY3 now make those launcher children non-exempt and commit "unknown"), CrashReportClientEditor now recorded-only (EY3), and `EX5_live_config_scalars` reads the unchanged 09423 config. Every EX2 and EX3 case passes.
- **My 094-22 set:** 42/46; the 4 deltas are the same sample shape (no commit reading, no paths). All argv, alias/junction, lock, orphan-deadline and refusal cases pass.

## 5. Writes and snapshots
- **Mine:** `code-receipts-094-24\`; `resume-prompt-094-24.txt`; `094-23-*` copies (hash-equal to the run folder); `_reviews\094-24-codex-report.md` (body byte-identical) and this file; `D:\IA_Europa\Receipts\CodeReview09424\` and `Temp\CodeReview09424\` only. I built, committed and deleted nothing. I stopped one process: my own hung background poll task (a Claude Code tool task, 21:23).
- **Before → after:** 61 listings, 52 equal. The 9 that differ: free space; processes; worktree heads (Paris worktree now 11 tracked changes); C:\ root (`ClaudeTemp`); E:\ root (`OBSVideos` new, `<personal-folder>`/`<personal-folder>` mtimes — owner); D:\ root (`pagefile.sys`); `ProgramData\Epic` (launcher rotation); tooling sizes; `D:\IA_Europa` +1,369 files (616 `Receipts\09424`, 598 `Temp\09424`, my CodeReview files, 4 new tools), 11 tool files changed, 0 removed.

## 6. GAPs, OPENs, observations
- **STOP (Codex) / D1:** ED5 on `E:\OBSVideos`, writer = owner's OBS (my evidence).
- **G2 (blocks a re-arm, not this closeout):** the EQ suite needs the `commit_memory` reader in its synthetic reader dict (or `collect()` must tolerate a missing reader); it must pass before the next arming.
- **O1:** the environment-read check misses copy-based reads (4 forms); no current instance.
- **O2:** rehearsal console prints "limit 1800 seconds" with a 2–30 s rehearsal deadline (cosmetic).
- **O3:** the session suite's < 8 s collector-delay check is load-sensitive.
- **O4:** EU1 count labelling differs by one (117 + 1 mine vs "118 + 1" Codex).
- **O5:** EY2 exempts nothing unless the launcher runs from `…\Portal\Binaries\Win64|Win32`; with an unreadable or unusual launcher path the overlays block and the gate waits (fail-safe).
- **Carried:** O1 (094-23: clean exit at runtime), O2b, EY8's list.

## 7. Final canary search
From the launch second (17:49:46 IST) to 22:58 IST over D:\IA_Europa, the run folder, `_reviews`, C:\ClaudeTemp, %LOCALAPPDATA% UBT/UnrealEngine, ProgramData\Epic, the 4.25 install, the m51 checkout and the mailbox (`review\canary-search-final.json`): 2,652 new or changed files, 3.43 GB, 0 read errors, **0 value hits, 0 other canary-shaped strings**; 31 files carry the name only (scanners, wrappers, tool sources and receipts). Hash compared: `e1489f2e…` (canary.txt).

## 8. Top 5 risks for the real run (094-25, when re-armed)
1. **ED5 again:** any new or removed top-level folder on C:, D: or E: during the run (OBS to a new folder, a download, a Steam library, a new project) is a STOP. EU2 does not cover it.
2. **Memory at start and in session:** free commit read 7.9–16.2 GiB this evening against the new 12 GiB start line and the 6 GiB in-session stop; physical free 1.75–8.8 GiB, mostly below the 6 GiB start line. Blender, Chrome, OBS or an idle editor will hold the gate (4 h limit) or stop a session.
3. **Disk swings:** D: dipped to 37 GiB (below its 40 GiB start line) this evening; a start-line refusal stops that attempt and uses a prelaunch slot (3 per recipe).
4. **First real clean exit and time budget (carried O1):** the engine-exit skipped cleanup and one `probe_stop` have never run on the real editor; 1,800 s includes startup, DDC and shader work.
5. **Paths the rehearsal substitutes:** real P9 admission, real argv, `check_ddc`, live start readings and live acting limits meet for the first time in the real run (each is covered separately by dry runs and unit tests).

## 9. Decisions for chat
- **D1 (ED5 on `E:\OBSVideos`).** (a) Accept the attribution (owner's OBS) as foreign and re-issue the remainder of 094-24: Codex completes missing-stop and deadline through its rehearsal (or chat accepts §3 as that evidence), fixes G2, runs the supplemental checks, re-arms 09424, dry-runs, takes the live reading and commits. ED5 unchanged, plus an owner instruction: no new top-level folders on C:, D: or E: during a job. (b) Amend ED5 so a new root folder with an established foreign writer is recorded, not a STOP (needs an attribution method within the no-ETW/no-USN rule, such as the process and configuration read I used).
- **D2 (the uncommitted candidate).** The next job starts with a dirty `feat/europa-port` worktree (15 files, hashes in `Receipts\09424\stop-closeout.json`). Chat should say whether it resumes from those exact bytes (verified by hash) and commits after re-qualification, or discards and rebuilds.

VERDICT: REVIEW DONE — Codex NEEDS-DECISION (ED5 STOP on new root folder E:\OBSVideos; writer = owner's OBS Studio by my evidence; 09424 not re-armed; candidate uncommitted). EY1 G1 fixed and proven through the real .bat (supervisor saw the derived marker); my rehearsal 5/5 (pass-both, extra file, missing probe_stop, deadline+sibling, crash-reporter-named child). EY2 live = my walk on 3 polls (9 PIDs, 0 blockers), can-fails 29/29; EY3 60 s 12/12; EY4 E 15 GiB inclusive. GAP G2: retained EQ suite fails under the candidate (missing commit reader). Escapes O1 (copy-based env reads). 110/110 bound; Paris Content unchanged; P9 v2 18,264/18,264; m51 = P9; canary 0; EK1 0 values; EU1 118/1/0; 0 Unreal launches.
