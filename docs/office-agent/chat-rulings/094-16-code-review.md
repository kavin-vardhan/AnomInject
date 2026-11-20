# 094-16 — Claude Code review of Codex's A6 kit arming (W3: written after Codex exited)

**Date:** 2026-10-26, 04:05–04:50 IST · **Author:** Claude Code (mailbox brief 094-16) · **Inputs:** Codex's report `_reviews\094-16-codex-report.md` (thread `01a14334`, resume 02:51:57–04:04:02 IST, 4,325 s, exit 0, ledger row `094-01-europa-phase-a-plan-resume14` once), chat's `094-15-ruling.md` (EP1–EP3) and the rulings it binds. My receipts: `code-receipts-094-16\` (and `review\`), `D:\IA_Europa\Receipts\CodeReview09416\`, `D:\IA_Europa\Temp\CodeReview09416\`.

## Summary
- **The kit is armed exactly as EP1 says, the three real slots are unused, and nothing launched.** Feature HEAD `303d02d` (commits `27e6b3c`, `d751456`, `303d02d`, local only, W4 clean). Bound manifests 27/39/44 re-hashed by me: every file equal, manifest hashes `4beccca6…`, `617a9082…`, `f6e52a85…` unchanged. P9 v2 18,264/18,264 and its file hash `8e3ed095…` unchanged; `m51` = P9 at start, before launch, after exit and at the end; protected state PASS ×3; engine trees untouched; canary 0 value hits in 784 new/changed files; 0 launches.
- **Codex's EP2 tests reproduce:** 62/62 new cases plus 48/48 measured-gate regressions (re-run from a copy whose only change is the TEMP base, so my writes stay in `CodeReview09416`). **My own can-fails: 61/61**, including every one the brief named. **My own end-to-end dry run of the real `.bat`** on throwaway slots: `ARMED_DRY_RUN_PASS`, real P9 owner preflight PASS (472,553 paths, 23,033 hashes, 104.8 GB), synthetic gate, 0 launches, no launch markers.
- **Two GAPs, neither of which launches anything unintended in the normal path, but both against EP2's text — chat decides whether they must be fixed before the owner leg:**
  - **G1 — the live quiet-start sampler fails about 4 % of the time, and the failure is not handled.** `Get-Counter '\GPU Engine(*)\…'` raised `CounterApiError` on **3 of 70** live samples (`CalledProcessError`). `owner_leg.main()` catches only `RuntimeError/OSError/ValueError/AssertionError`, so the owner would see a **Python traceback**, the attempt would be used up, and no editor would start. At ~35 s per poll, a 3-poll quiet start fails ~12 % of the time, and **a 45-minute wait almost surely ends in a traceback (~97 %) instead of EP2's plain give-up message**.
  - **G2 — the one-session lock has a narrow hole between `Popen` and the start callback.** If anything raises there (the child receipt write, the compile-child observer import/thread), `owner_leg` removes the launch reservation with the receipt reason "Popen did not start an editor" although the child is running, writes no `launched.json`, and **the next double-click can claim slot 2 and start a second editor**. Reproduced with a harmless Python child and a forced write failure (`review\post-popen-gap16.json`).
- Six OPENs (below), none blocking.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | EP1 armed scope = exactly one owner-started 4.25 EditorPreview session, mesh `/Game/ParisStreet/Meshes/SM_WindowHouse3` 60 s, File > Exit, no save; no PIE/cook/commandlet/packaging/automated input; one editor | Real config `runtime_scope` equals that field for field; `build_pairs`/`query_pairs` empty; host 425; can-fail `real_config_scope_is_exactly_EP1` | OK |
| 2 | EP1 authorization record cites 094-15 EP1 | `authorized_by_chat: true`, `authorization_citation` and `job_template.authority` = `D:/IntrusiveAnomalies/_reviews/094-15-ruling.md#EP1`; `runtime_authorized: true` only inside that scope; 18 unarmed/widened mutants refused | OK |
| 3 | EP2 three slots, first unused taken, refused attempt leaves next usable | Codex 62/62 reproduced; mine: two/three pre-existing folders, a file at a slot path, a slot with an old config not reused | OK |
| 4 | EP2 launched lock refuses every later attempt | Codex + mine: second, third, fourth attempt after `launched.json` refused and create no slot; marker alone refuses | OK, see **G2** |
| 5 | EP2 one plain waiting line a minute naming each unmet condition | Synthetic: 45 lines in 2,700 s, every condition named (11 conditions through `wait_idle` itself), changing conditions tracked | OK synthetic; **G1** live |
| 6 | EP2 45-minute give-up, no launch | Synthetic exact 2,700 s, timeout line, 0 launches | OK synthetic; **G1** live |
| 7 | EP2 owner gate drops human idle; keeps ≥ 6 GiB, GPU ≤ 15 %, no foreign Unreal, three polls | `profile='owner-preview'` skips only `input_idle`; disk floors also kept (stricter); clears at 3 spaced polls with recent input | OK |
| 8 | EP2 measured legs keep the full gate | `Window(profile='measured', human_idle=False)` raises; default profile = measured; measured window never clears with recent input; config with `gate_profile` measured / `human_idle` true or missing refused | OK |
| 9 | EP2 self-tests with the seven can-fails | All present in `owner_kit_self_tests.py` and passing on my re-run | OK |
| 10 | EP2 dry run on throwaway slots, real slots unused, 0 launches | Codex's `Receipts\09416\dry-run\attempt-01` and mine `CodeReview09416\bat-dry-run\attempt-01`, both `ARMED_DRY_RUN_PASS`; real slot/TEMP/state paths absent | OK |
| 11 | EP2 bound binaries/manifests unchanged (STOP otherwise) | 27/27, 39/39, 44/44 at 02:29 and after exit; manifest hashes equal | OK |
| 12 | EP3 (094-17, after the owner says done) | Not this job. The kit routes `-abslog`, crash log and `-EuropaProbeOut` into the claimed attempt folder on D:, so EP3's evidence will exist | N/A |
| 13 | EO1/EO2 action-log reader | No build ran | N/A |
| 14 | EO3 makefile refresh | No UBT run; all three hosts' Intermediate SHA-256 listings equal before/after, so the six makefiles are unchanged | N/A |
| 15 | EO4 one probe source | Host Source listings equal; probe source stays `550e9b7` | OK |
| 16 | EO5 closeout with the accepted tools | Codex: EA8 exit, canary from the launch second (328 files), EK1, ledger summary; EA11 not applicable (no reflected header touched) | OK |
| 17 | EO6 repaired self-test | `resume_self_tests.py` not in the diff | OK |
| 18 | EN1 toolchain environment | No new makefile; the six A5 makefiles unchanged and decode (my EK1 pre-scan) | OK |
| 19 | EN2/EN3 | Not exercised | N/A |
| 20 | EN4 Code's writes | Only `CodeReview09416` folders, my receipts, the two review files | OK |
| 21 | EN5 intent clause | Codex applied none; I agree none was needed (G1 is unhandled stock flakiness, not a rule conflict) | OK |
| 22 | EM2 `CLAUDECODE` blocked | Present in Codex's base (positive control); absent from all 47 child environments | OK |
| 23 | EL1 denylist | 47 children (23 python, 23 git, 1 cmd = Codex's `.bat` dry run): contract PASS, base preserved, 0 tool/secret names, SDK variables present | OK |
| 24 | EL2 canary | Positive control matched at launch (`08f03e45…`); 0 value hits in 784 new/changed files since 02:51:56; 0 child environments carry the name; only my synthetic fixtures are other canary-shaped strings | OK |
| 25 | EL3 | A5 finished in 094-15 | N/A |
| 26 | EL4 the kit | `.bat` → `owner_leg.py` → `write_ledger.run_child` (approved wrapper); refuses on any preflight failure; D: routing; P9 preflight inside a ledger operation | OK |
| 27 | EL5 Code's deviation | My wrapper removed only `CLAUDE_CODE_MESSAGING_TOKEN` (names-only receipt) | OK |
| 28 | EK1 secrets | Codex: 38,200 files, 0 values. Mine: pre-scan 37,954 files and post-scan 559 files with the live-value check active, 0 values | OK |
| 29 | EK2 | No makefile regenerated | N/A |
| 30 | EK3 `GIT_OPTIONAL_LOCKS=0` | In every child's routing; Baseline51/ReGate51 heads and indexes unchanged | OK |
| 31 | EI2 memory/parallelism | No build | N/A |
| 32 | EG1 engine trees | All 5.1 engine listings and the full 4.25 install equal before/after: 0 engine writes | OK |
| 33 | EG2 build receipts | No build | N/A |
| 34 | W4, no push, no tags | Author = committer date on all three commits; only `refs/heads/feat/europa-port` moved | OK |
| 35 | No-comments convention | 0 comment/docstring lines in the four new or rewritten tools | OK |
| 36 | P9 v2 | 18,264/18,264 binaries re-hashed, 0 mismatches; file `8e3ed095…` at start and end | OK |
| 37 | `m51` | `53bf725`, index `526aba7f`, 15 untracked = P9 at 02:28, before launch, after exit, final | OK |
| 38 | Protected state | PASS at start, after exit, final | OK |
| 39 | Real slots unused | `Receipts\09416\owner-attempts\01–03`, `Temp\09416\owner-attempts\01–03`, `owner-state` all absent | OK |
| 40 | Launches | Poller: 0 launch-class processes of ours in 8 polls; Codex's children python/git/cmd only; my dry run none | OK |

**Count: 30 OK · 10 N/A · 2 GAP (G1, G2, against rows 4–6) · 6 OPEN · 0 STOP violations · 0 conflicts.**

## 2. EP2, the tooling
**Diff `3d20b5c..303d02d`:** 16 files, +679/−130. Code: `owner_attempts.py` (new: `validate`, `Guard` with a Windows byte-range lock on `owner-state\active.lock`, atomic `mkdir(exist_ok=False)` slot claims, `launch-reserved.json` / `launched.json`, `cancel_unstarted` with a hash receipt), `owner_wait.py` (new: `wait_idle` with one notice a minute, 2,700 s limit, disk-floor stop, plain labels), `owner_leg.py` (rewritten around the guard; slow inventories now precede the gate; `--owner-started` required for live), `idle_gate.py` (`profile` = `measured` | `owner-preview`; owner skips only input idle and also counts any image named `unreal`/`ue4`; window limit 2,700 s for owner), `write_ledger.py` (`on_started` callback after `Popen`), `.bat` (adds `--owner-started`), `owner_kit_self_tests.py` (new). Docs: CLAUDE.md, architecture, onboarding, runbook, gotcha G465, the prediction addendum, a session note, `phase-a-reach.json`.

**Re-runs (`review\rerun-codex-selftests16.json`):** `owner_kit_self_tests.py` 62/62 PASS, 45 waiting lines, 48 measured regressions; `idle_gate.py --self-test` 48/48. The harness copy differs from the source (`5545c591…`) in one string: the synthetic fixtures' TEMP base, because the test writes TEMP scratch under `D:\IA_Europa\Temp\<name>-scratch` (Codex's run left `Temp\ep2-tests-attempt1-scratch`).

**My can-fails (`review\own-canfails16.json`, 61/61):**
- **Fourth attempt after a launched marker:** second, third and fourth attempts refused with "already started", no slot folder created; with all slots also used, the launch message still wins.
- **A slot folder that already exists:** two existing → slot 3; three → "All three attempts have been used"; a file at the slot path is skipped; an old `job-config.json` is never reused.
- **An unarmed config:** 18 mutants refused (authorization false / `"true"` / missing, citation missing or another ruling, `runtime_authorized` false, two sessions, PIE, commandlet, automated input, save, extra scope key, a build pair, host 51, two slots, a slot outside Receipts, state inside a slot, a slot on C:). The real config refuses a dry run before any claim, and validates for the live owner run.
- **A measured-leg profile that drops human idle:** refused at `Window`, `reasons` and config level (4 + 4 cases).
- **A waiting message for each unmet condition, through `wait_idle` itself:** low memory, GPU, foreign UE4Editor / UnrealEditor-Cmd / UnrealBuildTool / a `-Win64-Shipping` game / ShaderCompileWorker, unreadable memory / GPU / processes / disk: each 45 lines naming it and the timeout line; never a mouse/keyboard line.
- **My dry run of the real `.bat`** (`review\bat-dry-run16.txt`, config differing from the real one only in `dry_run_only`, `state_root`, `attempt_slots`, `activation`): exit 0 in 950 s; the console shows exactly the lines in §5 for my sample schedule; argv routes everything to D:; state folder holds only `active.lock`. My first try failed in my own harness before the kit ran (cmd quoting); it is receipted as try 1.

**G1 detail (`review\collector-stress16-a.json`, `collector-stress16.json`):** 70 live `collect()` calls, 3 × `CalledProcessError` from `Get-Counter` (`CounterApiError`, GPU-engine instances appearing/disappearing), mean 9.7–10.5 s per call against a 20 s timeout (a `TimeoutExpired` would take the same uncaught path). `wait_idle` does not catch collector errors; `run()` re-raises them; `main()` does not catch `SubprocessError`. Fail-closed (no editor), but the owner sees a traceback, the `.bat` pauses, and the slot is consumed.

**G2 detail:** in `write_ledger.run_child`, `dump(path, receipt)`, the compile-child observer import/thread (`EUROPA_BUILD_CHILDREN=1` on the live path) and the console thread run after `Popen` but before the `try:` that calls `on_started`; an exception there neither terminates the child nor marks the launch, and `owner_leg`'s `finally` then calls `cancel_unstarted`. A fix is small (call `on_started` first after `Popen`, or treat any exception after `reserve_launch` with a live `Popen` as launched).

## 3. Armed scope and slots
- `D:\IA_Europa\Launch\Europa425-OwnerLeg.json` (18,629 B, `1f328ec6…`): schema 2, arming job 09416, `dry_run_only: false`, `gate_profile: owner-preview`, `human_idle: false`, state `D:\IA_Europa\Receipts\09416\owner-state`, 42 tooling files (42/42 hash-equal now, including every owner module, `owner_p9.py` and both `.bat` copies), 30 host source/routing files (30/30), manifest `4beccca6…` (equal).
- **Real slots, all absent after the job and after my review:** `D:\IA_Europa\Receipts\09416\owner-attempts\01`, `\02`, `\03`; TEMP `D:\IA_Europa\Temp\09416\owner-attempts\01`, `\02`, `\03`; state `D:\IA_Europa\Receipts\09416\owner-state`.
- What the kit cannot enforce: PIE and saving are console/card instructions, not technical blocks; the argv itself opens the editor only (no `-game`, no `-run=`), with the probe flags reviewed in 094-15.

## 4. The `.bat`, line by line (298 B, `4b5b2812…`, CRLF, no BOM, byte-equal to the tracked copy)
1. `@echo off` · 2. `setlocal` · 3. `"C:\Python313\python.exe" -B "…\tools\europa\owner_leg.py" --config "D:\IA_Europa\Launch\Europa425-OwnerLeg.json" --owner-started %*` · 4. keep `ERRORLEVEL` · 5. `pause` unless the first argument is `--dry-run` · 6. `exit /b` with it.
- **Approved wrapper:** the editor can start only via `write_ledger.run_child` inside the `owner-leg-launch-admission` runtime operation, after the lock, a fresh slot, the scope check, ED1 start floors, manifest/binary/source/tooling hashes, the DDC check, routing validation and the P9 preflight, then the gate, then a second hash pass.
- **Refuses when the preflight fails:** every failure raises before `reserve_launch` (G1's exception type aside, which still launches nothing).
- **D: routing:** `-UserDir`, `-ShaderWorkingDir`, `-DDC=Europa425DDC` (graph checked), `-abslog`, `-AbsCrashReportClientLog`, `-EuropaProbeOut` into the attempt folder; child TEMP/TMP/UAT/DotNet/NuGet into `D:\IA_Europa\Temp\09416\owner-attempts\0N\…`.
- **Launched nothing** (Codex's dry run and mine).

## 5. Owner-visible console lines (verified against source; all 24 of Codex's list are literals or composed from literals)
```text
Europa owner preview: checking authorization.
Attempt 1 of 3: checking protected files, binaries and routing. This can take several minutes.
Attempt 2 of 3: checking protected files, binaries and routing. This can take several minutes.
Attempt 3 of 3: checking protected files, binaries and routing. This can take several minutes.
Protected files, binaries and routing passed. Checking for a quiet start; you may use the mouse and keyboard.
Waiting: free memory is below 6 GiB. Leave this window open.
Waiting: GPU use is above 15%. Leave this window open.
Waiting: another Unreal or game program is running (PID <n>). Leave this window open.
Waiting: cannot read free memory; cannot read GPU use; cannot read running programs; cannot read free space on C; cannot read free space on D; cannot read free space on E. Leave this window open.
Waiting: quiet-start checks 1 of 3. Leave this window open.
Waiting: quiet-start checks 2 of 3. Leave this window open.
Quiet-start checks passed three times.
No quiet start within 45 minutes. No editor was started. Close heavy jobs, then double-click again to use the next attempt.
Starting the one authorized editor session. Do not open Play. Answer No to any rebuild prompt. Close with File > Exit and save nothing.
Editor closed. Tell chat what happened. This kit will not start another editor session.
Stopped: This kit is not armed by chat. No editor was started.
Stopped: All three attempts have been used. Ask chat to prepare more; no editor was started by this attempt.
Stopped: The authorized editor session has already started. Ask chat before trying again.
Stopped: Another kit attempt is running. Keep its console open; no editor was started by this attempt.
Stopped: A prior launch has an incomplete receipt. Ask chat before trying again.
Stopped: A disk is below its safety floor. No editor was started.
Keep this console open and send the message to chat. Do not accept rebuild, convert, update or resave prompts.
Press any key to continue . . .   (Windows' own pause prompt, localized; not in Codex's list)
```
Several unmet conditions share one line, joined by `; `. Plain and accurate, with four card notes:
- "several minutes" measured **12.5 min (Codex) and 15.8 min (mine)**; say "about 10–15 minutes".
- The 45 minutes start **after** that check, so a give-up can come ~60 min after the double-click.
- Not in the list but reachable: `Stopped: P9 owner preflight failed.`, `Stopped: Bound owner-leg file changed: <path>`, `Stopped: ED1 floor/headroom STOP: {…JSON…}`, and (G1) a Python traceback. 17 further configuration-integrity messages exist and are unreachable on the armed kit.
- "another Unreal or game program" also fires for helpers such as `UnrealTraceServer.exe` (O1).

## 6. Bound binaries, manifests, P9 v2, protected state, snapshots
- Manifests and all 110 bound files: equal at 02:29 and after exit (`kit-hashes-start2.json`, `kit-hashes-after-exit.json`).
- P9 v2: full binary re-hash 18,264/18,264; file hash equal at 02:31 and final.
- Snapshots (61 listings): 094-15 after-review → 094-16 before: 53 equal, 8 foreign/volatile. Before → after: 49 equal; ours changed only `feat/europa-port` (ref, Paris worktree head/index) and `D:\IA_Europa` (281 new files in `Receipts\09416`, the two Launch files, Paris worktree tools/docs; 0 removed); the rest is the foreign DCDemo UE 5.7 workload (LocalAppData UnrealEngine/UBT, ProgramData\Epic\Zen, `E:\DynamicContents`), processes, free space and tooling sizes. After-review: see `review\after-vs-afterreview.txt`.

## 7. Observations (OPEN, none blocking)
- **O1** The owner profile counts any process image containing `unreal`/`ue4` as foreign, including `UnrealTraceServer.exe` and `UnrealVersionSelector.exe`. The foreign DCDemo workload (its own mailbox watcher, UE 5.7 `UnrealEditor-Cmd` runs at 01:44 and 03:01 IST, UBT at 02:38) can hold the gate.
- **O2** A UE 5.x UnrealBuildTool runs as `dotnet.exe …UnrealBuildTool.dll`; the collector's `blocked_runtime` command-line match does not include it, so a foreign 5.x build is not reported as an Unreal program (memory and GPU still gate).
- **O3** `validate()` would accept a throwaway (`dry_run_only: true`) config that names the real slot paths; only an operator could build one, and the double-click path uses the real config, which refuses dry runs.
- **O4–O6 carried from 094-15:** EO1's reader budget is spent; the 5.1 probe control-row path (A9); EJ1/EJ3/EJ5.

## 8. Top 5 risks for the owner leg
1. **G1:** the sampler can fail mid-wait and show a traceback, using up an attempt. Best mitigation without a fix: close Blender and other heavy or GPU-using apps first, so the gate clears in three polls (~1–2 min). The card should say "if red error text appears, take a screenshot, close the window, double-click again".
2. **Time:** ~10–15 min of checks before the quiet-start gate, then up to 45 min of waiting; the console must stay open throughout.
3. **The foreign UE 5.7 workload** (E:\DynamicContents) can start Unreal processes at any time and hold the gate (O1); its watcher may need pausing for the leg.
4. **First real 4.25 editor start** (from 094-15): tens of minutes of shaders, a possible "rebuild modules?" prompt (No), never accept convert/update/resave, never open plugin Content; the five 5.1 packages' predicted rejection is still unobserved. ED1 floors are re-checked every 20 s during the session and a breach terminates the editor (C: 18.0 GiB now, floor 10).
5. **G2 plus only three slots:** a one-session hole in a narrow failure window; and once the editor starts, any further try needs chat to re-arm.

VERDICT: REVIEW DONE — 30 OK / 10 N/A / 2 GAP (G1 live sampler failure ~4 %/poll uncaught → traceback, 45-min plain give-up practically unreachable; G2 post-Popen exception releases the launch reservation with a child running) / 6 OPEN / 0 STOP violation / 0 CONFLICT; kit armed exactly to EP1 citing 094-15 EP1; three real slots and state absent; Codex 62/62 + 48/48 reproduced; my 61/61 can-fails; my own real-.bat dry run ARMED_DRY_RUN_PASS with real P9 preflight, 0 launches; manifests 27/39/44 and 110 bound files unchanged; P9 v2 18,264/18,264; m51 = P9 throughout; protected state PASS ×3; engine trees untouched; canary 0 value hits in 784 files, 0 child environments; EK1 0 values; 0 launches
