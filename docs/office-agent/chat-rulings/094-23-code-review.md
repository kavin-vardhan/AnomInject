# 094-23 — Claude Code review of the re-armed A6 fixture-authoring kit (W3: written after Codex exited)

**Date:** 2026-10-28, 15:55–16:55 IST · **Author:** Claude Code (mailbox brief 094-23) · **Inputs:**
- Codex's report `_reviews\094-23-codex-report.md`: thread `01a14334`, resumed 10:47:59–15:55:14 IST, 18,435 s over 2 tries, exit 0. Try 1 ended at ~13:33 on "Selected model is at capacity"; the relay waited 10 min and resumed. Ledger row `094-01-europa-phase-a-plan-resume21`, recorded once.
- Chat's `094-22-ruling.md` (EX1–EX6), `094-23-request.md` and the rulings they bind.

**My receipts:** `code-receipts-094-23\` (with `step0\`, `protect\`, `ek1\` and `review\`), `D:\IA_Europa\Receipts\CodeReview09423\` and `D:\IA_Europa\Temp\CodeReview09423\`. No account identifier is reproduced.

## Summary
- **Codex ended GREEN.** It made two local commits on `feat/europa-port`, with no upstream and no tags, and author time equal to committer time:
  - `8c4da78`: the EX1–EX5 tooling; the kit binds this commit;
  - `97de9d4`: CLAUDE.md and the journal only.
- **The re-armed kit:**
  - `D:\IA_Europa\Launch\Europa425-FixtureAuthoring.bat` is unchanged (`4c5ad14e…`);
  - the config is now `b1daf567…` (424,634 B), with arming job `09423`;
  - the job-template wording reads "09423 armed for EW2 and EW3, as amended by 094-22-ruling EX1-EX5".
- **EX1 is right, and proven live (§2).** On three polls, the kit's own collector excluded exactly the set my independent downward walk from `EpicGamesLauncher.exe` produced. That set is 9 PIDs: the launcher 26116, its 3 web helpers and the 5 EOS overlays. Both foreign-process sets were empty, and both equal Codex's reading at 12:48. 094-22's G4 is fixed.
- **EX2, EX3 and EX5 are each right as units (§3–§5).** My EX can-fails pass 56 of 57. The one failure is my own expectation, and the behaviour behind it predates this job (O3).
- **One BLOCKING GAP (G1), found by driving the real entry function (§6):**
  - Codex's new variable `EUROPA_AUTHORING_MARKER` contains "AUTH", so the kit's own EK1/EL1 filter treats it as a secret-class name.
  - `fixture_leg.main()` replaces its environment with `filtered(os.environ)` before `run()`, which drops it.
  - In a real (non-dry) session, `fixture_leg.py:129` (`os.environ['EUROPA_AUTHORING_MARKER']`) therefore raises `KeyError` right after printing "starting the one authorized authoring session".
  - EA_Core stops before any editor launch. It uses one prelaunch slot and prints `Stopped: 'EUROPA_AUTHORING_MARKER' …`. EA_Travel never runs.
  - The dry run returns at line 125, before that line, so no dry run (Codex's or mine) can see it.
  - Reproduced through the real `main()` with `run()` stubbed (`review\marker-env23.json`).
- **One decision for chat (D1):** EX1's "every descendant" also exempts an editor or game *started from the launcher*. Codex's test `EX1_every_descendant_recorded` codifies that a `UE4Editor.exe` under the launcher is excluded.
- **My `.bat` dry run was refused at its first preflight** by ED1/EV3's start figures. E: had 19.17 GiB free against 25, after foreign E: use during the day. That refusal is correct, but it means the downstream half of my real-entry dry run could not run (§7). Today's E: (19.1 GiB) also blocks any real run.
- **Everything else agrees with Codex:**
  - self-tests re-run: 89/89, 57/57, 159/159, 62/62 + 48, 48/48, disk_pause PASS, and 33/33 × 4;
  - my 094-22 can-fails 46/46; refusals through the real `.bat` 12/12; O2 mutants 37/37; G1/EN1 16/16;
  - 110/110 bound files; Paris Content 1,453 + 5 byte-identical to my start list;
  - canary: 0 value hits in 1,275 files;
  - EK1: 0 secret values;
  - EU1: 118 commands through the wrapper / 1 direct read-only / 0 direct write-capable;
  - P9 v2: 18,264/18,264; m51 = P9; protected PASS; 0 launches.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | EX1 launcher tree excluded by ancestry, re-read each poll, recorded | `idle_gate.launcher_tree()` walks each process's parent chain to an `EpicGamesLauncher.exe` root, with a PID-reuse guard: a child created before its parent breaks the walk. `reasons()` skips the excluded set; `collect()` records `launcher_tree_excluded` with each PID's ancestry. The old image-name exemption list is removed. Live: §2. My synthetic polls 1–6 pass (§2) | OK · D1 |
| 2 | EX1 anything outside the tree still blocks | Outside the tree, these block: UnrealEditor, UE4Editor, EOSOverlayRenderer, a `-Win64-Shipping` game, UBT, UHT, dotnet UBT and ShaderCompileWorker (my cases). A cycle and a self-parent end the walk and block | OK · O3 |
| 3 | EX1 a launcher child with foreign Unreal children is judged by ancestry each poll | Tree present: excluded, with ancestry [grandchild, child, launcher]. Launcher exits: the same processes block. A new launcher does not adopt orphans. A reused launcher PID younger than the child is not an ancestor. Re-parenting is re-checked. The same tree later is excluded again, with no cached state | OK |
| 4 | EX1 launcher absent: the kit waits | Synthetic: "Waiting: Epic Games Launcher already running; …", 241 messages, NEEDS-DECISION after 4 h. The kit never starts the launcher | OK |
| 5 | EX1 live, observe-only gate reading in the dry run | Codex's reading (12:48 IST): only `input_idle`; 9 excluded. Mine, at the same moment as an independent reading on three polls (16:03–16:05): excluded sets equal, foreign sets equal and empty, both equal to Codex's. Three more live polls (16:05–16:06): 0 blockers | OK |
| 6 | EX2 start gate unchanged (full measured profile) | `reasons(profile="measured")`, the 300 s / 6 GiB / 15 % / 3 polls rules and the 4 h limit are unchanged in the diff. My 094-22 boundary cases still pass (46/46) | OK |
| 7 | EX2 in session: only the disk floors, commit below 6 GiB and 1,800 s act; the rest is recorded | `SessionGate.check()` reads the commit and drive floors at most once a second and raises → `run_child` stops the owned tree. The observe-only collector records idle, memory, GPU, launcher and foreign processes with `action=record_only`. My cases: exact floors don't act; −1 B on commit or on C/D/E acts; idle 0, 1 B memory, GPU 100 %, foreign UnrealEditor and absent launcher do not act | OK · O2 |
| 8 | EX2 stops only our tree, crash reporter included; no pause | Commit, E: floor and deadline each stopped child + grandchild + detached orphan (≥ 3 PIDs) in 2.2/2.4/3.2 s, despite a blocked collector. A sibling survived. The recipe is then spent. `floors()` after start raises an ED1 STOP and never enters the EU2 checkpoint (and does enter it before start, as the can-fail shows). "Crash reporter included" is covered by descendant tracking; I deliberately did not fake a CrashReportClient image | OK |
| 9 | EX2 phase handoff to the in-session monitor | `disk_pause.authoring_started()`: absent, wrong authority, wrong phase and wrong PID all refuse; valid accepts. The supervisor's direct child PID equals the helper's `os.getpid()` (`C:\Python313\python.exe`, no launcher shim). **But the helper never receives the variable it needs (G1)** | **GAP G1 (blocking)** |
| 10 | EX2 one session per recipe, no automatic retry | Recipe spent after every stop; `claim()` refuses on two tries; `fixture_entry` stops the later recipe | OK |
| 11 | EX3 repeated saves; partial outputs quarantined by receipt; nothing else changes; retry needs re-arm | Synthetic host: only the declared partial moved, sha checked, to `<attempt>\quarantine\EA_Core.umap`. Receipt `retry_authorized=false`, `launch_lock_unchanged=true`, `original_paths_absent=true`. The only changed path is the declared one; an undeclared sidecar and the other recipe's output are untouched. Refused: a second call, a hard-linked output (nothing moved), changed existing Content (refused before any move). Retry refused ×2 | OK |
| 12 | EX4 `-unattended` stays off | The argv mutant `unattended_added` is refused (in my 46/46) | OK |
| 13 | EX5 O6 wording; O2b; O4; O7 | Template wording passes. O2b: the same four shapes survive. O4: execution children 0 `EUROPA_*` (88 receipts); G1/EN1 16/16. O7 accepted | OK |
| 14 | EX6 dry-run only | 0 launches (§9) | OK |
| 15 | Re-arm: EW2 scope exactly; 09423 slots unused; 09422 refused | `runtime_scope` identical to EW2 (2 recipes, 1 each, 1,800 s, new packages only, no PIE/cook/commandlet/packaging/production/config edit). Real 09423 paths 0/15; old 09422 paths 0/15. `validate()` and the real `.bat` refuse the preserved 09422 config (`Receipts\09423\prior-arming09422.json`, `42c89db2…`, hash-equal to the config found at start) | OK |
| 16 | EW1 manifests bound | Config `manifests` = `c501895c…` / `b05e6f8c…` / `3865fe0c…`; re-hashed 27/39/44 at start and after exit | OK |
| 17 | EW2 / EW3 retained (argv switch, aliases, locks, 8 sibling calls, G1 metadata) | My 094-22 can-fails 46/46 (incl. 11 argv mutants, alias/junction refusals, launched/reserved locks, orphan deadline); O2 37/37; G1/EN1 16/16 | OK |
| 18 | EW4 carried | EO1 budget, A9, EJ1/EJ3/EJ5, O3 (A8), EV6 glass, O3 memory | OPEN (carried) |
| 19 | EV1 no edit inside a pause | 0 pauses (Codex's ledger; my series: C ≥ 20.24, D ≥ 39.98, E ≥ 20.81 GiB from 10:49 to 15:55) | OK |
| 20 | EV2 9 h | 5.1 h wall, including the capacity wait | OK |
| 21 | EV3 D 40/30/25 (C 13/11/10, E 25/20/15) | Kit start figures and floors as ruled; my dry run was refused at E start (§7) | OK |
| 22 | EV4 / EV6 / EV7 | No build; EV6 carried; EV7 rows 1–17 | N/A / OK |
| 23 | EV5 one probe source | 15 files, identical across 3 hosts, start = after | OK |
| 24 | EU1 command audit | 119 commands: 118 through the wrapper, 1 direct read-only (`Get-Content`), 0 direct write-capable; 20 file-edit items. Equal to Codex | OK |
| 25 | EU2 foreign dips | Three foreign D: dips of ~20 GiB (lowest 39.98 GiB); none below a floor, so no pause. E: fell from 33.4 to 19.1 GiB through foreign writes | OK · risk 2 |
| 26 | EU3 / EU4 / EU5 / EU6 | Rows 4, 12, 17 | OK |
| 27 | ET1–ET7 | ET2 marker `31124` unchanged (0 B, only entry). ET3: launcher-family log rotation in `ProgramData\Epic` (foreign). ET7: owner-slot `.tmp` files unchanged (still unreadable to my scanner) | OK |
| 28 | ER1 every refusal shows its reason | 12/12 through the real `.bat`, each with its own `Stopped:` line, none "unexpected error". **G1's stop would read only `Stopped: 'EUROPA_AUTHORING_MARKER' …`** | OK · G1 |
| 29 | ER2 / ER4; ER3 | Not exercised; ER3 bare and dotnet UBT block (rows 2, 17) | N/A / OK |
| 30 | EQ1–EQ5 | EQ1: the start gate waits on an unreadable reading (live `gpu:unknown` waited once). EQ2 lock holds. EQ4: the 09417 arming stays spent (`active.lock`, `launch-reserved.json`, `launched.json`) | OK · O2 |
| 31 | EP1–EP3 | — | N/A |
| 32 | EO1–EO6 | EO1 carried. EO3: 6 makefiles decode, none regenerated. EO4 row 23. EO5: EA8 entry/exit PASS (Codex), report collected byte-identical (`A4B330E9…`). EO6 not touched | OK |
| 33 | EN1–EN5 | EN1 16/16; EN2/EN3 not exercised; EN4: my writes are in allowed folders only; EN5: no new interpretation (EX3 ratified the repeated save) | OK |
| 34 | EM2 `CLAUDECODE` | In the launch base (positive control); in 0 of 88 child receipts | OK |
| 35 | EL1 | 88 child receipts, 0 with tool/secret/canary names, 0 `EUROPA_*` in EW3 execution children. **The supervisor → helper hop carries the secret-class name `EUROPA_AUTHORING_MARKER` (value masked)**, the same item as G1 | OK · G1 |
| 36 | EL2 canary | Positive control: the child sha equals the generated hash `d93c7348…`, and Codex's `job_template.canary_sha256_utf8` equals it. By value from 10:47:58: 773 files after Codex's exit and 1,275 at the end, 0 value hits, 0 other canary-shaped strings. By name in child environments: 0 | OK |
| 37 | EL3 / EL4 / EL5 | My wrapper removed only `CLAUDE_CODE_MESSAGING_TOKEN` | OK |
| 38 | EK1 / EK2 / EK3 | Full scan 45,724 files, 0 secret values, 0 live-value matches, 0 git date values, 2 read errors (ET7 files). Name-hit files 312 → 320: the only new name outside my tooling is `EUROPA_AUTHORING_MARKER` (masked). Makefiles 6/6 decode. `GIT_OPTIONAL_LOCKS=0` | OK |
| 39 | EI2 / EG1 / EG2 | No build; 4.25 install and every 5.1 tree byte-equal before/after | N/A / OK |
| 40 | P9 v2 / protected / m51 | P9 v2 re-hash 18,264/18,264, file `8e3ed095…` unchanged. Protected PASS (start, after exit, final). m51 = P9 (start, both launch gates, after exit, final; can-fails 5/5) | OK |
| 41 | Launches | 33 polls with `launch_class_ours=0`, no sightings; child images only `cmd`/`git`/`python` (plus a synthetic `missing.exe` creation-failure test); no UE-class process at the end | OK |

**Count (41 rows, primary status): 35 OK · 4 N/A or partly N/A · 1 OPEN (carried) · 1 GAP (G1, blocking) · 1 decision (D1, on row 1) · 0 STOP · 0 stop violations.** Also O2 (rows 7, 30), O3 (row 2), and G1's echoes on rows 28 and 35.

## 2. EX1 — the launcher tree (diff, live, can-fails)
- **The diff** (`idle_gate.py` +33/−11):
  - `launcher_tree()` handles absent parents, cycles and PID reuse (a `created_utc` comparison).
  - `process_record()` and `read_processes()` now carry `created_utc`.
  - The three-name exemption (`epicgameslauncher`, `unrealcefsubprocess`, `epicwebhelper`) is gone, so an image name alone exempts nothing.
- **Live** (`review\live-compare.json`, `review\live-gate-now1.json`):
  - The launcher is still the 10-27 instance, 26116; no restart happened (boot 2026-10-22 17:12 IST).
  - On every poll the kit excluded [4420, 26116, 35088, 39596, 40516, 43312, 43852, 44568, 46296], which equals my downward walk.
  - `EpicOnlineServicesUserHelper.exe` 44340/3228 (parent 16704, not running) is outside the tree and matches no image rule, so it does not block.
  - The remaining reasons were `input_idle` and `physical_memory` (owner present, Blender open), and `gpu:unknown` once.
  - The line was "Waiting: mouse and keyboard idle for 300 seconds; 6 GiB free physical memory."
- **D1:**
  - A Unreal editor or game whose ancestry reaches `EpicGamesLauncher.exe` is excluded too. That is what EX1 says ("every descendant"), and Codex tests it (`UE4Editor.exe` under the overlay → not blocking).
  - Opening a project or game from the launcher normally makes it a launcher child. That is expected rather than measured here: no banked process list holds such an editor (`review\launcher-parent-history.json`).
  - The measured profile still requires GPU ≤ 15 %, 6 GiB free memory and 300 s idle, so an active editor would still hold the gate; an idle one would not.

## 3. EX2 — the in-session rules
- **Built in three places** (`fixture_session_gate.py`, `write_ledger.py`, `disk_pause.py`):
  - **`SessionGate`:** the acting limits run inside `run_child`'s loop, and a separate observe-only collector thread runs every 25 s.
  - **`write_ledger.floors()`:** raises an ED1 STOP instead of an EU2 checkpoint once `AUTHORING_STARTED` is set.
  - **`disk_pause`:** skips its own pause after a valid `authoring-started.json` (authority EX2, phase `authoring_started`, PID = the helper).
- **Unit behaviour is right** (row 7–10).
- **O2:** an unreadable commit or drive reading inside the session does not stop it (fail-open). Recorded in `review\mycanfails-ex23.json`.

## 4. EX3 — the quarantine
- **`fixture_quarantine.py`:**
  - requires the attempt's `prepared.json` with the exact declared list;
  - refuses a declared path that existed before;
  - re-verifies all pre-existing files;
  - plans each move with size, mtime, sha and file id, writes the receipt first, moves by `os.rename` on D:, and re-verifies;
  - refuses aliases, links and reused folders.
- **`fixture_leg`** calls it in `finally` whenever a process was created and the result is not `AUTHORING_PASS`, and again if the ledger close raises.
- **Rows 11, 15.**

## 5. EX5, the armed scope and the console
- **Scalars:**
  - `arming_job 09423`;
  - `amendment_citations` = EX1–EX5 anchors;
  - `session_policy` as ruled;
  - `source_commit 8c4da78`;
  - `design docs/europa/09423-authoring-amendments.md` (`d9442e31…`);
  - the 09422 design kept as accepted base.
- **Bound rows:** 61 tooling, 199 host, 7 engine, 1,458 existing Content and 42 config; every row re-hashed equal (0 bad).
- **The 15 real paths:**
  - `D:\IA_Europa\Receipts\09423\fixture-state\{global,EA_Core,EA_Travel}`;
  - `…\Receipts\09423\fixture-attempts\{EA_Core,EA_Travel}\{01,02,03}`;
  - `…\Temp\09423\fixture-attempts\{EA_Core,EA_Travel}\{01,02,03}`.
  - All are absent.
- **Console:** Codex's catalog (`Receipts\09423\arming\console-catalog.json`) has 191 expressions, and its 16 ordinary templates match the source. With G1, a real run would show this after the gate clears:
```text
EA_Core: starting the one authorized authoring session; limit 1800 seconds. Output is recorded in <attempt>\session.
Stopped: 'EUROPA_AUTHORING_MARKER' No further session will be started.
Stopped: EA_Core did not pass. No later recipe or automatic retry will start.
```

## 6. G1 — the helper never receives its phase marker (blocking)
- **Chain:**
  1. `fixture_entry` → `disk_pause --authoring-marker <state>\authoring-started.json` → `env['EUROPA_AUTHORING_MARKER']=…` (`disk_pause.py:304`, added after `filtered()`) → `python -B fixture_leg.py`.
  2. `fixture_leg.main()`, at `fixture_leg.py:193–194`, does `clean=filtered(os.environ); os.environ.clear(); os.environ.update(clean)`.
  3. `filtered()` drops every `forbidden_child_name`, and `secret_name()` matches `AUTH` in `AUTHORING` (`environment.py:7–22`).
  4. In the non-dry branch, `fixture_leg.py:128–129` runs `ledger.begin(...)` and then `canonical(os.environ['EUROPA_AUTHORING_MARKER'])` → `KeyError`.
- **Proof** (`review\marker-env23.json`):
  - the variable is present before `main()` and absent inside `run()`;
  - `EUROPA_JOB_CONFIG` and `EUROPA_DISK_SUPERVISOR_PID` survive;
  - the lookup raises `KeyError "'EUROPA_AUTHORING_MARKER'"`.
- **Effect:**
  - fail-safe: no launch, no reservation and no quarantine;
  - but every real attempt stops after a P9 entry and a full gate wait, and spends a prelaunch slot. After three, the recipe needs a re-arm.
- **Why nobody saw it:**
  - Dry runs return at `fixture_leg.py:122–125`.
  - Codex's session tests exercise `authoring_started()`, `SessionGate` and `run_child` separately, never `fixture_leg.main()` on its real branch.
  - Codex's EK1 qualification read the name as "masked orchestration phase metadata", which is correct about the value and silent about the filter.
- **Fix options** (D2).

## 7. My dry run of the real entry point
- **Command:** `cmd /d /c D:\IA_Europa\Launch\Europa425-FixtureAuthoring.bat --dry-run --dry-run-root D:\IA_Europa\Receipts\CodeReview09423\bat-dry-run --dry-run-temp D:\IA_Europa\Temp\CodeReview09423\bat-dry-run --dry-run-samples …\mycanfails\bat-samples.json`, at 16:16–16:18.
- **Result:**
  - exit 2, 0 launches;
  - "EA_Core: attempt 1 of 3; checking protected files, Content, binaries and routing." then `Stopped: ED1 floor/headroom STOP: {… "E": {"free": 20585660416, "start": 26843545600 …}}`.
- **Correct by EV3:** E: start is 25 GiB.
- **Not re-tried:** E: stayed at about 19.1 GiB through the end of the review.
- **Coverage instead:**
  - Codex's two `.bat` dry runs (11:3x and 12:1x IST, E: 26–28 GiB) passed both recipes;
  - the overlay-clearing samples pass in my synthetic wait and refusal sets;
  - P9, bound-file and Content checks are re-done independently above.

## 8. Self-tests and my own sets
- **Codex's suites, re-run by me** (`review\rerun-selftests23.json`):
  - fixture 89/89 (33 static, 48 measured);
  - session 57/57 (EX1 13, EX2 29, EX3 10, arming 5);
  - EQ 159/159; kit 62/62 + 48; idle_gate 48/48; disk_pause PASS;
  - static 33/33 on Paris, Baseline51, ReGate51 and the templates.
  - Codex's 33 command-audit tests were not re-run; my own EU1 audit replaces them.
- **My sets:**
  - EX can-fails 56/57 + 4 observations (`review\mycanfails-ex23.json`);
  - the 094-22 set 46/46, where the two former G4 failures now pass;
  - refusals 12/12, including the preserved 09422 config and three EX5 mutants;
  - O2 mutants 37/37 (O2b: the same 4 survivors);
  - G1/EN1 16/16; m51 can-fails 5/5;
  - `marker-env23`, `live-compare`, `old-image-rules` and `children23`.

## 9. Writes, snapshots
- **Mine:**
  - `code-receipts-094-23\`, `resume-prompt-094-23.txt`, and the preserved `094-22-*` copies (hash-equal to `code-receipts-094-22\`);
  - `_reviews\094-23-codex-report.md` (body byte-identical) and this file;
  - in `D:\IA_Europa`, only `Receipts\CodeReview09423\` and `Temp\CodeReview09423\`.
  - I launched nothing, built nothing, deleted nothing and stopped no process.
  - One `Remove-Item -ErrorAction SilentlyContinue` named a file in my receipts that did not exist, so nothing was removed.
- **Before → after:** 61 listings, **50 equal**, including:
  - the 4.25 install and every 5.1 tree;
  - all three hosts' Binaries, Intermediate, Source, Config and Content, and both 5.1 plugins;
  - StackOBot, the release baseline, the cache and `%LOCALAPPDATA%`.
- **The 11 that differ, all expected:**
  - free space and processes;
  - refs, worktree heads and porcelain (`feat/europa-port` `1a9afc1` → `97de9d4`);
  - foreign drive-root mtimes on C:, E: and D: (`ClaudeTemp`, `Program Files`, `renders`, `System Volume Information`, two owner folders on E:, `SteamLibrary`, `pagefile.sys`);
  - `ProgramData\Epic` (launcher);
  - tooling sizes;
  - `D:\IA_Europa`: +583 files (572 `Receipts\09423`, 8 `Temp\09423`, 3 committed docs/tools), 12 changed (the kit config, CLAUDE.md, 10 committed tools), 0 removed.

## 10. GAPs, OPENs, observations
- **G1 (blocking):** §6.
- **D1:** §2.
- **O2:** in-session unreadable commit or drive readings don't stop the session (fail-open; EQ1 governs only the start gate).
- **O3:** outside the tree, `CrashReportClient(Editor)`, `UnrealPak`, `UnrealLightmass`, `UnrealInsights`, `UnrealTraceServer`, `UnrealVersionSelector`, `UnrealCEFSubProcess` and `EpicWebHelper` do not block. This is unchanged since `1a9afc1` (`review\old-image-rules.json`). It is a classification question against EX1's "any Unreal … process".
- **O4:** the PID-reuse guard is skipped when a creation time is missing. Live readings carry it (Codex's receipt and mine).
- **O5:** Codex's first dry-run harness hit its own 1,800 s aggregate timeout during the Travel preflight. It re-ran on fresh roots with 3,600 s; the kit's 1,800 s session limit is unchanged.
- **O6:** the first full EK1 pass died with the capacity failure; Codex retained it and re-ran.
- **Carried:** O1 (clean exit proven only from source), O2b, and EW4's list.

## 11. Top 5 risks for the first automated run
1. **G1:** every real attempt stops before launch, and the console says "starting …" first.
2. **Disk at start and during the session:**
   - E: is 19.1 GiB against a 25 GiB start figure;
   - D: had three ~20 GiB foreign dips today (lowest 39.98 GiB, below the 40 start figure);
   - a dip during a session to under 25 GiB on D: or 15 GiB on E: stops the editor and spends the recipe.
3. **Commit memory:**
   - free commit fell to 4.88 GiB at 15:49 with the owner's Blender and Chrome open;
   - under EX2 that stops a running session and spends the recipe;
   - the start gate checks physical memory only.
4. **D1:** an editor or game started from the launcher and left idle is not a foreign process.
5. **The first real clean exit and time budget** (carried):
   - qualification needs the exact `engine_exit` cleanup and one `probe_stop`, never yet run;
   - 1,800 s includes startup, any DDC/shader work and the editor's own UBT children.

## 12. Decisions for chat
- **D2 (G1), required before any real run.** Each option changes bound tooling hashes, so each needs a re-arm (no launch).
  - **(a) Recommended:** drop the variable. `fixture_leg` already knows the expected path (`guard.state/'authoring-started.json'`) and `disk_pause` validates the same path from its own argument. Then add an integration test that drives `fixture_leg.main()` down the real branch with `ledger.run_child` stubbed, so no editor starts.
  - (b) Rename it so it doesn't match the EK1 secret-name pattern (e.g. `EUROPA_SESSION_PHASE_FILE`), plus the same test.
  - (c) Pass it as a `fixture_leg` argument, plus the same test.
- **D1 (EX1 scope).** Choose one:
  - (a) keep "every descendant";
  - (b) exempt launcher descendants except editor, game and build images (UnrealEditor/UE4Editor/ShaderCompileWorker/UBT/UHT/AutomationTool, or any image under a project or engine folder). The EOS overlay stays exempt by its install folder or its image;
  - (c) exempt only images under the Epic launcher and Epic Online Services install folders.
- **O2 / O3:** rule whether in-session unreadable readings act, and whether the O3 images count as "Unreal processes".

VERDICT: REVIEW DONE — 35 OK / 4 N/A / 1 OPEN (carried) / 1 GAP G1 BLOCKING (EUROPA_AUTHORING_MARKER is dropped by fixture_leg.main()'s own EK1 name filter, so the real authoring branch raises KeyError before launch; reproduced through main()) / D1 EX1 scope decision / 0 STOP / 0 stop violations. EX1 live reading equal to my independent walk on 3 polls and to Codex's (9 PIDs, 0 blockers). EX2/EX3/EX5 correct as units; scope = EW2; 09423 real paths 0/15; 09422 refused. My real-.bat dry run refused at the E: start figure (19.17 < 25 GiB). Self-tests 89/57/159/62+48/48/disk_pause/33×4; my can-fails 56/57 + 46/46, refusals 12/12, O2 37/37, G1/EN1 16/16. 110/110 bound; Paris Content identical; canary 0 in 1,275 files; EK1 0 values; EU1 118/1/0; P9 v2 18,264/18,264; m51 = P9; 0 launches.
