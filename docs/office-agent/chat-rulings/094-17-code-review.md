# 094-17 — Claude Code review of Codex's A6 kit fix and re-arm (W3: written after Codex exited)

**Date:** 2026-10-26, 06:56–07:40 IST · **Author:** Claude Code (mailbox brief 094-17) · **Inputs:** Codex's report `_reviews\094-17-codex-report.md` (thread `01a14334`, resume 05:20:26–06:56:06 IST, 5,740 s, exit 0, 210 items, ledger row `094-01-europa-phase-a-plan-resume15` once), chat's `094-16-ruling.md` (EQ1–EQ5) and the rulings it binds. My receipts: `code-receipts-094-17\` (and `review\`), `D:\IA_Europa\Receipts\CodeReview09417\`, `D:\IA_Europa\Temp\CodeReview09417\`.

## Summary
- **G1 and G2 are fixed, and I proved both independently.** G1: every reading error or timeout is now a "cannot read …" wait that resets the three-pass count (my 34/34 injections through the real wait loop; 320 live collector calls between Codex and me, 5 GPU read failures, 0 uncaught exceptions). G2: the launched mark is written in the first callback after `Popen`; my re-run of the 094-16 post-Popen reproduction now leaves the mark and the next attempt is refused, and 11/11 of my injection cases behave as ruled (creation failure leaves slot 2 usable; mark-storage failure keeps the reservation locked).
- **Re-armed exactly as EQ4 says:** config `5d1d1ab5…` (arming 09417, citation `094-15-ruling.md#EP1` + amendment `094-16-ruling.md#EQ4`, scope = EP1 field for field), three real slots and the state folder absent, the 09416 arming refused, `.bat` unchanged (`4b5b2812…`). My own dry run of the real `.bat` on throwaway slots: `ARMED_DRY_RUN_PASS`, real P9 preflight PASS, 0 launches.
- **Bound binaries/manifests unchanged** (27/39/44, `4beccca6…`/`617a9082…`/`f6e52a85…`, re-hashed at exit and final). P9 v2 18,264/18,264, file `8e3ed095…` unchanged; `m51` = P9 at start, launch, exit and final; protected state PASS ×3; canary 0 value hits in 4,627 new/changed files; 0 launches.
- **One new GAP, non-blocking: G3 — every refusal now prints the generic `Stopped: unexpected error; details saved to <file>. No editor was started.`** The plain reasons ("All three attempts have been used", "Another kit attempt is running. Keep its console open", "already started", "superseded", …) are only in the details file. Fail-safe (no editor starts in any case), but it goes beyond EQ1's "any **other** unexpected error", and chat's card must not list the old specific `Stopped:` lines.
- Codex's raw ledger STOP (six linker temp files removed under `C:\ClaudeTemp\UnrealBuildTool\044a3114`) is a foreign UE 5.7 DCDemo build; I verified the native UBT log independently. EB1 governs; not an EN5 hard-STOP class.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | EQ1 G1: each reading (memory, GPU, programs, C/D/E) that errors or times out → "cannot read", not a pass, resets the count, keeps waiting | `review\eq1-canfails17.json` 34/34: `CalledProcessError`, `TimeoutExpired`, `ValueError`, `OSError` injected into each of the 6 readings via `collect(readers)` inside the real `wait_idle` loop: the bad poll has `consecutive 0`, `<name>:unknown`, the error recorded by type only (no stderr, no command text), the "Waiting: cannot read …" line, then exactly three fresh passes clear; never a traceback. Whole-collector `CalledProcessError`/`TimeoutExpired`/`OSError`: 45 lines, `TIMEOUT` at exactly 2,700 s. Known disk breach still stops | OK |
| 2 | EQ1 live proof (≥ 200 by Codex, ≥ 100 by me) | Codex 200 calls (`live-collector-final.jsonl` 200 rows re-read by me): 5 × GPU `CalledProcessError` (2.5 %), 0 uncaught, 4.1–10.3 s. Mine (`collector-live17.json`): 120 calls, owner profile, 0 reading errors, 0 uncaught, 3.85–4.89 s (no foreign Unreal process ran during my window) | OK |
| 3 | EQ1 safety net: "any **other** unexpected error" → one plain line + details file | The line, the details file in the attempt folder, redaction and "no traceback on the console" all hold (my forced `KeyError` after the claim: one line, details in `attempt-01`, fake secret `<redacted>`). **But `main()` now catches `BaseException` for every error, so the 28 plain refusal messages reach only the details file** (G3, §2) | **GAP G3** |
| 4 | EQ2 G2: mark right after process creation; later failures keep it; cancel only on creation failure | `review\eq2-canfails17.json` 11/11, harmless `python -c sleep` child: no fault; my 094-16 reproduction (receipt write after Popen); PID receipt; observer import; observer start; console thread start; console log open; poll-loop floor breach; final receipt → `launched.json` exists, child gone, next attempt "already started". Mark storage fails → reservation kept, next "incomplete receipt". Creation fails (`FileNotFoundError`) → reservation removed by receipt, next attempt claims slot 2 | OK |
| 5 | EQ3 O1: trace server and version selector recorded, not holding the owner gate; editors/commandlets/games/build tools/SCW still do; measured unchanged | `review\eq3-canfails17.json`: both images (any case) ignored in owner-preview and kept in the sample; 15 other Unreal images still block, owned descendants exempt; 9 images give identical measured results at `303d02d` and now | OK (O1 below) |
| 6 | EQ3 O2: dotnet running UnrealBuildTool counts in both profiles | 6 command-line forms × 2 image casings block both profiles, `command_line` not kept; at `303d02d` the measured profile did not block them; `dotnet build`, `…UnrealBuildToolHelper.dll`, `MyUnrealBuildTool.dll`, `--info` do not block; dotnet AutomationTool blocks | OK |
| 7 | EQ3 O3: a dry-run config naming a real slot or state path is refused | 32 path variants for 09416 and 09417 refused: exact slot/temp/state, parents, children, `..` aliases, case, forward slashes, trailing dot, trailing space, `\\?\` prefix. Throwaway config valid; live config must name exactly the 09417 paths (other slot/state refused); real config with `--dry-run` refused | OK (O5 below) |
| 8 | EQ4 re-arm for 09417, EP1 scope, citation as amended | `review\config-check17.json`: `arming_job 09417`, `authorized_by_chat true`, citation `094-15-ruling.md#EP1`, amendment `094-16-ruling.md#EQ4`, `dry_run_only false`, owner-preview, `human_idle false`, `runtime_scope` == EP1, no build/query pairs, slots `Receipts\09417\owner-attempts\01–03` + `Temp\09417\owner-attempts\01–03`, state `Receipts\09417\owner-state`; 43/43 tooling and 30/30 host files hash-equal; every module `owner_leg.py` imports transitively (11) is bound | OK |
| 9 | EQ4 09416 arming superseded and unusable; its slots absent | `validate()` on the 09416 config (from `kit-before.json`) → "This arming has been superseded…", live and dry; through `main()` too. All seven 09416 paths and all seven 09417 paths absent at exit and final | OK |
| 10 | EQ4 dry run on throwaway slots, real slots unused, 0 launches | Codex `Receipts\09417\dry-run\attempt-01` (768 s) and mine `CodeReview09417\bat-dry-run\attempt-01` (801 s, §5): both `ARMED_DRY_RUN_PASS`, launches 0 | OK |
| 11 | EQ4 bound binaries and manifests unchanged | 27/27, 39/39, 44/44 at start, after exit and final; manifest hashes equal | OK |
| 12 | D2: two attempts for the EQ1–EQ3 tooling | Events: suite runs attempt1 PASS (156), attempt2 PASS (unchanged repeat), EQ3 alias gap reproduced, one repair, PASS (159). No failing suite run; the killed live observer was a harness counter bug (disclosed) | OK |
| 13 | EQ5 the card | Chat's job; facts in §5 and §8 | N/A |
| 14 | EP1 one owner-started 4.25 EditorPreview session | Scope above; argv opens the editor only (no `-game`, no `-run=`), routing on D: (§5) | OK |
| 15 | EP2 slots, lock, plain waiting, 45 minutes, owner gate, measured gate | All re-verified; plain waiting and give-up lines intact; refusals see G3 | OK |
| 16 | EP3 evidence job | 094-18 | N/A |
| 17–19 | EO1/EO2 reader, EO3 makefile refresh | No build, no UBT; host Intermediate listings equal before/after | N/A |
| 20 | EO4 one probe source | Host Source listings equal; ReGate51 head `550e9b7` | OK |
| 21 | EO5 closeout | Codex: EA8 exit, canary from the launch second, EK1, ledger summary, commit with W4 | OK |
| 22 | EO6 repaired self-test | `resume_self_tests.py` not in the diff | OK |
| 23 | EN1 toolchain environment | No makefile regenerated; six A5 makefiles decode (my EK1 pre-scan) | OK |
| 24 | EN2/EN3 | Not exercised | N/A |
| 25 | EN4 Code's writes | Only my allowed folders (§6) | OK |
| 26 | EN5 | Codex applied none; agreed. The foreign TEMP removal is not an EN5 hard-STOP class and EB1 covers it | OK |
| 27 | EM2 `CLAUDECODE` blocked | Present in the launch base (positive control); absent from all 97 child receipts | OK |
| 28 | EL1 denylist | 97 child receipts under `Receipts\09417` (24+42 python, 30 git, 1 cmd): contract PASS, 0 tool/secret names, base preserved | OK |
| 29 | EL2 canary | Positive control at launch (child python sha = generated `4be1f396…`); 0 value hits in 4,627 new/changed files since 05:20:25 (1 foreign pytest temp dir vanished mid-scan); 0 child environments carry the name; the job config stores only its SHA | OK |
| 30 | EL3 | A5 finished | N/A |
| 31 | EL4 the kit | `.bat` → `owner_leg.py` → `write_ledger.run_child`; refuses before any launch on every preflight failure; D: routing | OK |
| 32 | EL5 Code's deviation | My wrappers removed only `CLAUDE_CODE_MESSAGING_TOKEN` (names-only receipts) | OK |
| 33 | EK1 secrets | Pre-scan 38,716 files, post-scan 1,179 files, Codex 39,353 files: 0 values, 0 live matches; `AUTHORITY` hits hold the public citation | OK |
| 34 | EK2 | No makefile regenerated | N/A |
| 35 | EK3 `GIT_OPTIONAL_LOCKS=0` | In child routing; Baseline51/ReGate51 heads and indexes unchanged | OK |
| 36 | EG1 engine trees | 5.1 engine listings and the full 4.25 install equal before/after | OK |
| 37 | EB1 foreign write, op `03-documentation` raw STOP | Native `%LOCALAPPDATA%\UnrealBuildTool\Log.txt` (sha `1D40BFA1…`, = Codex's) started 06:11:30 IST: DCDemo UE 5.7 Shipping, line 11 "Setting temp directory to 'C:\ClaudeTemp\UnrealBuildTool\044a3114'"; files created 06:11:47; op 03 children python/git only; my polls saw 0 build processes of ours | OK |
| 38 | W4, no push, no tags | Author = committer date on `c11843e`, `5922ce6`, `23adabf`, `b20e66e`; no upstream; only `refs/heads/feat/europa-port` moved | OK |
| 39 | No-comments convention | 0 comments, 0 docstrings in the 7 changed tools | OK |
| 40 | P9 v2 | 18,264/18,264 re-hashed, 0 mismatches; file `8e3ed095…` at start and final | OK |
| 41 | `m51` | `53bf725`, index `526aba7f`, 15 untracked = P9 at start, launch, exit, final | OK |
| 42 | Protected state | PASS at start, after exit, final | OK |
| 43 | Launches | Poll log 10 polls, `launch_class_ours 0`; no sightings file; Codex children python/git/cmd; my dry run descendants python only | OK |
| 44 | Ledger row | `…-resume15` exactly once | OK |

**Count: 35 OK · 8 N/A · 1 GAP (G3) · 7 OPEN · 0 STOP violations · 0 conflicts.**

## 2. The tooling (`303d02d..b20e66e`, 16 files, +560/−66)
- `idle_gate.collect()` now reads each condition in its own `try`; errors become `reading_errors[name] = {type, returncode | timeout_seconds}` and the field stays `None`, which `reasons()` turns into `<name>:unknown`. Memory now comes from `build_memory.sample()` (GlobalMemoryStatusEx), processes from one PowerShell call whose command lines are classified in memory and dropped (`process_record`). `owner_wait.wait_idle` also catches `SubprocessError`/`OSError` from the whole collector.
- `write_ledger.run_child(…, before_create, on_creation_failed)`: reservation → `Popen` (creation failure → cancel) → `on_started` first → receipt, observer, console thread inside the `try`; any later exception terminates the owned child and re-raises.
- `owner_leg.main()` catches `BaseException` → `stop_unexpected()`: details (redacted traceback) in the claimed slot, or `Receipts\09417\refused-attempts\<uuid>\` before a claim; one console line, suffix "No editor was started." or "An editor may have started. This kit remains locked; tell chat before trying again."
- `owner_attempts`: `ARMING_JOB 09417`, `AMENDMENT`, superseded refusal, resolved-path checks against both generations, exact live paths.

**G3 detail (`review\main-cases\*.json`, run through `owner_leg.main()` in fresh processes on throwaway configs):** all slots used, launched marker, reservation marker, lock held by another process, unarmed, superseded 09416 — every one printed only `Europa owner preview: checking authorization.` then `Stopped: unexpected error; details saved to …\refused-attempts\<uuid>\unexpected-error-<uuid>.txt. No editor was started.`; the details file's last line is the old plain reason. No editor, no real path touched. The removed lines include "Keep its console open" (a second double-click now looks like a fault) and "Keep this console open and send the message to chat. Do not accept rebuild, convert, update or resave prompts." Fix size: catch `RuntimeError` first and print `'Stopped: '+str(error)`; any fix changes a bound tooling hash and needs a re-arm.

## 3. Self-test suites
- Codex's suites re-run by me in `CodeReview09417` (copies changing only the fixture TEMP base): `owner_eq_self_tests.py` **159/159** (13 harmless children), `owner_kit_self_tests.py` **62/62** + 48 measured, `idle_gate.py --self-test` **48/48**.
- Mine: EQ1 34/34, EQ2 11/11, EQ3/EQ4 126/130 — the 4 "fails" are my premise check (O1), main-path cases 7/7 as described in G3, EK1 self-test 17/17, canary self-test 8/8.

## 4. Armed kit and slots
- `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat` 298 B `4b5b2812196e0d5256ec086f7d695ee46827c5b2bb65eb8e2b62973fa7dddd74` (unchanged since 094-16).
- `D:\IA_Europa\Launch\Europa425-OwnerLeg.json` 19,066 B `5d1d1ab55abcf166ea42a3dc7d724f6b165af04cfac9e14088cbd3572c59e9f3`, mtime 05:55:48.
- Real slots: `D:\IA_Europa\Receipts\09417\owner-attempts\01`, `\02`, `\03`; TEMP `D:\IA_Europa\Temp\09417\owner-attempts\01`, `\02`, `\03`; state `D:\IA_Europa\Receipts\09417\owner-state` — all absent. 09416's seven paths absent. `Receipts\09417\refused-attempts` absent.
- Feature `b20e66e`; tools `idle_gate.py 3cc42607…`, `owner_attempts.py f03db813…`, `owner_leg.py 3adc7073…`, `owner_wait.py 5243dc6f…`, `write_ledger.py d7271e4f…`, `owner_eq_self_tests.py 4998ebbb…` (= Codex's table).

## 5. My dry run of the real `.bat`
Throwaway config differing only in `dry_run_only`, `state_root`, `attempt_slots`, `activation`; synthetic readings: low memory, good, GPU unreadable, good with `UnrealTraceServer.exe`, dotnet UBT, good ×3. Exit 0, 806 s wall (preflight 801 s), descendants python ×2 only, `ARMED_DRY_RUN_PASS`, P9 entry-owner PASS. Streak: 0, 1, 0 (`gpu:unknown`), 1 (trace server passes), 0 (`foreign_process:4712`), 1, 2, CLEAR at 175 s. Argv: `UE4Editor.exe <Paris .uproject> -UserDir=D:/… -ShaderWorkingDir=D:/IA_Europa/Caches/425/… -DDC=Europa425DDC -abslog/-AbsCrashReportClientLog/-EuropaProbeOut` into the attempt folder. Console, verbatim:
```text
Europa owner preview: checking authorization.
Attempt 1 of 3: checking protected files, binaries and routing. This can take several minutes.
Protected files, binaries and routing passed. Checking for a quiet start; you may use the mouse and keyboard.
Waiting: free memory is below 6 GiB. Leave this window open.
Waiting: cannot read GPU use. Leave this window open.
Waiting: another Unreal or game program is running (PID 4712). Leave this window open.
Quiet-start checks passed three times.
Dry run complete: the armed path passed. No editor was started; real attempts were not used.
```

## 6. Owner-visible console lines (Codex's 25, each verified against source: literal or composed from literals)
```text
Europa owner preview: checking authorization.
Attempt 1 of 3: checking protected files, binaries and routing. This can take several minutes.
Attempt 2 of 3: checking protected files, binaries and routing. This can take several minutes.
Attempt 3 of 3: checking protected files, binaries and routing. This can take several minutes.
Protected files, binaries and routing passed. Checking for a quiet start; you may use the mouse and keyboard.
Waiting: free memory is below 6 GiB. Leave this window open.
Waiting: GPU use is above 15%. Leave this window open.
Waiting: another Unreal or game program is running (PID <n>). Leave this window open.
Waiting: cannot read free memory. Leave this window open.
Waiting: cannot read GPU use. Leave this window open.
Waiting: cannot read running programs. Leave this window open.
Waiting: cannot read free space on C. Leave this window open.
Waiting: cannot read free space on D. Leave this window open.
Waiting: cannot read free space on E. Leave this window open.
Waiting: quiet-start checks 1 of 3. Leave this window open.
Waiting: quiet-start checks 2 of 3. Leave this window open.
Quiet-start checks passed three times.
No quiet start within 45 minutes. No editor was started. Close heavy jobs, then double-click again to use the next attempt.
Starting the one authorized editor session. Do not open Play. Answer No to any rebuild prompt. Close with File > Exit and save nothing.
Editor closed. Tell chat what happened. This kit will not start another editor session.
Dry run complete: the armed path passed. No editor was started; real attempts were not used.
Stopped: unexpected error; details saved to <file>. No editor was started.
Stopped: unexpected error; details saved to <file>. An editor may have started. This kit remains locked; tell chat before trying again.
Press any key to continue . . .   (Windows' own pause prompt, localized; not after --dry-run)
```
Several unmet conditions share one `Waiting:` line, joined by `; `. **No longer shown on the console (G3):** every specific `Stopped: <reason>` line from 094-16, including "Another kit attempt is running. Keep its console open…", "All three attempts have been used…", "The authorized editor session has already started…", "A disk is below its safety floor…", "P9 owner preflight failed." and "Keep this console open and send the message to chat. Do not accept rebuild, convert, update or resave prompts."

## 7. Writes, snapshots
- Mine: `code-receipts-094-17\`, `resume-prompt-094-17.txt`, the preserved `094-16-*` copies, `_reviews\094-17-codex-report.md` (body byte-identical, `F03F9BBF…`), this file; in `D:\IA_Europa` only `Receipts\CodeReview09417` and `Temp\CodeReview09417` (359 files, 0 changed, 0 removed). I killed only my own lock-helper child (by handle); I deleted nothing.
- Before → after (61 listings): 49 equal. Ours: `feat/europa-port` (ref, Paris head/index), `D:\IA_Europa` (651 added under `Receipts\09417` and 3 new Paris tools/docs, 14 changed = the config, 6 tools, 7 docs; 0 removed). Foreign/volatile: LocalAppData UnrealEngine and UnrealBuildTool (DCDemo builds 05:41 and 06:11, trace servers), ProgramData\Epic (`NotAllowedUnattendedBugReports`, Zen), C:\ and E:\ roots, processes, free space, tooling sizes. 094-16 after-review → 094-17 before: 55 of 61 equal.

## 8. OPENs (none blocking)
- **O1** The brief's "but still hold the measured gate" is false: the measured profile never blocked `UnrealTraceServer.exe`/`UnrealVersionSelector.exe` (identical at `303d02d`). The ruling's "measured profile unchanged" holds.
- **O2** Pre-existing: the measured profile ignores a bare `UnrealBuildTool.exe`/`UnrealHeaderTool.exe` image (only dotnet UBT now counts); the owner profile blocks them through its `unreal|ue4` rule.
- **O3** A refusal before a slot is claimed writes `Receipts\09417\refused-attempts\<uuid>\unexpected-error-*.txt`; 094-18 should expect it. (My harness patched `owner_leg.ROOT` to keep these inside CodeReview09417.)
- **O4** Owner profile: `UnrealCEFSubProcess.exe` (Epic Games Launcher) blocks the gate; `CrashReportClient(Editor).exe` and `EpicGamesLauncher.exe` do not.
- **O5** Not tested: 8.3 short-name aliases (none exist on D:) and junction aliases (would leave a reparse point I may not delete).
- **O6** Any internal failure after the editor starts (receipt, observer, console reader, ED1 floor every 20 s) now terminates the editor and locks the kit; the console says "An editor may have started…".
- **O7** Carried: EO1's reader budget is spent; the 5.1 probe control-row path (A9); EJ1/EJ3/EJ5.

## 9. Top 5 risks for the owner leg
1. **G3:** any refusal, including a second double-click while the first window runs, reads "Stopped: unexpected error". The card should say: double-click once; if a second window shows Stopped, close only that window and leave the first open; screenshot any Stopped and tell chat.
2. **The foreign DCDemo workload** (UnrealEditor-Cmd at 01:44/03:01/05:23, UBT at 05:41/06:11, trace servers): its editor and dotnet UBT now correctly hold the gate, which can run the full 45 minutes and use a slot. Pause its watcher; close the Epic Games Launcher (O4), Blender and heavy jobs. GPU counter failures (5/200 while DCDemo ran, 0/120 when quiet) now just wait.
3. **Time and slots:** ~13–14 min of checks (768 s Codex, 801 s mine), then up to 45 min waiting; the window must stay open. A refusal after the claim uses that slot; three in total.
4. **First real 4.25 editor start** (carried): tens of minutes of shaders, a possible "rebuild modules?" prompt (No), never convert/update/resave, never open plugin Content; the five 5.1 packages' predicted rejection is unobserved. ED1 floors are checked every 20 s (C: 17.8 GiB, floor 10) and a breach or internal failure now closes the editor (O6).
5. **One session only:** once the editor process exists the kit locks (correctly) for every outcome, including a crash in the first second; any retry needs chat to re-arm.

VERDICT: REVIEW DONE — 35 OK / 8 N/A / 1 GAP (G3 every refusal prints the generic "Stopped: unexpected error" line; plain reasons only in the details file; fail-safe) / 7 OPEN / 0 STOP violation / 0 CONFLICT; G1 fixed (my 34/34 injections through the real wait loop; 320 live calls, 5 GPU read errors, 0 uncaught); G2 fixed (094-16 reproduction now keeps the mark and refuses the next attempt; 11/11 injections; creation failure leaves slot 2); O1–O3 as ruled (126 checks; brief premise on the measured gate false, ruling holds); re-armed 09417 citing 094-15 EP1 + 094-16 EQ4, scope = EP1, three real slots and state absent, 09416 refused; Codex 159/159 + 62/62 + 48/48 reproduced; my real-.bat dry run ARMED_DRY_RUN_PASS with real P9, 0 launches; manifests 27/39/44 unchanged; P9 v2 18,264/18,264; m51 = P9 throughout; protected state PASS ×3; canary 0 value hits in 4,627 files; EK1 0 values; foreign ledger STOP verified as DCDemo UE 5.7
