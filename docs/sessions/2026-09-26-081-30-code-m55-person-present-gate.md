# 081-30 — m55 requalification harness: `C1_G2_S` voided as environmental, the person-present gate added and proven both ways, boundary re-issued (Claude Code, Opus 5.5, 2026-09-26, bench-free)

## Goal
Brief 081-30, ruling `_reviews/081-30-chat-ruling-person-present-gate.md`. 081-29 stopped at `C1_G2_S`: a person used the PC, all three
attempts were lost to the foreground, and no gate predicate was evaluated (G292). Implement ruling 1 (ENV-VOIDED) and ruling 2 (the
person-present gate, items (a)–(d)) in the hash-locked lib and the window runner, prove the eight required items both ways, re-issue the
boundary, re-run the full dry run. **Bench-free:** no game, window, chunk or leg was launched; no build; no plugin source; the proofs run on
banked records and synthetic cases. Evidence: `_reviews/081-30-evidence/`.

## What the runners do with input, focus and the cursor (read first; the design rests on it)
- All four runners (`081-23-measure.ps1`, `081-23-lyra-run.ps1`, `081-23-nohold-run.ps1`, `081-22-legacy-run.ps1`) have **one**
  input-injecting call: `Force(h)` = `ShowWindow(h, SW_RESTORE)` + **`keybd_event` ALT down/up** + `SetForegroundWindow` +
  `BringWindowToTop`. They also call WScript.Shell `AppActivate`, which activates a window and injects no input.
- **None of them moves the cursor.** There is no `SetCursorPos`, `mouse_event`, `SendInput` or `ClipCursor` in any runner; measure and
  Lyra only read it (`GetCursorPos`).
- `Force`/`AppActivate` run (i) unconditionally during startup and the settle loop (measure 5 s, Lyra 45 s), and (ii) during snapshot
  sampling and capture **only on a sample where the foreground is not the game**. Inside the steady-state span the harness therefore
  injects input only right after a lost-focus sample.
- The game may move the cursor when it regains focus (UE re-captures the mouse). That is a consequence of the harness's focus action and
  is treated as the harness's, not a person's.
- On the banked c0/c1 records the cursor is **constant across the whole sampled span of every passing leg**. On three passing legs
  (`L1_LYRA_ON_A1`, `S1_STACK_ON_A1`, `C1_G8_N_A1`) `cursor_before` — read before the game launches — differs from the settled cursor.
  That is **outside** the evidence span (startup); cause not established (G120).
- Whether the ALT tap registers in `GetLastInputInfo` was **not measured** (that would mean injecting input on the owner's desktop). The
  design assumes it does, the safe assumption for an exclusion.

## Design, as built (`_reviews/081-23-lib.py`, `_reviews/081-23-window.py`)

### (a) The pre-attempt gate, `pp_gate`
- Called before every attempt of every leg kind — `run_measure_leg`, `run_legacy_group` (per runner invocation), `run_nohold`,
  `run_lyra_leg` — immediately before the quiet check and the launch; and at window start in `081-23-window.py`, after preflight and the
  process-quiet wait.
- Reads `GetLastInputInfo` (ctypes, user32) against `GetTickCount`. Clear when input idle ≥ 60 s.
- **Excluding the harness's own input.** Every runner invocation is recorded as a *harness window* `[launch, exit]` in
  `<window>/harness-input-windows.json`, with that attempt's `person_evidence`. If the latest input lies inside a harness window (± 1 s)
  whose attempt carried **no** person evidence, it is the runner's own focus action and the gate is clear. Why that is sound: every
  runner is released by a gate that proved ≥ 60 s of human idleness, and the latest input lies inside that runner's window, so no human
  input came after it or between that gate and it. A window whose attempt **did** carry person evidence is not excused: the gate waits
  60 s from the last input in it.
- While not clear: poll every ≤ 15 s, log `person-present-wait` with the idle reading (environment-events, and stdout), and add the wait to
  **`<window>/wait-budget.json`**. The window runner adds its process-quiet waits (`quiet-wait`) to the same file, so the 45-min per-window
  NOT-QUIET budget is one mechanical number. Exhausted → `Stop(4)` / window exit 4, NOT-QUIET class, with the idle reading in the message.
  **Never a NOT-RUN.**
- Budgets are keyed by a tag: `081-23-window.py <w> [limit] [tag]` (default `default`); the chunk inherits it as `IAI_R23_BUDGET_TAG`.
  A new brief uses a new tag to get a fresh 45 minutes for the same window.

### (b) Classifying an attempt that failed on foreground
- During every runner invocation a `PersonMonitor` thread samples every 0.25 s: input tick and idle, cursor, foreground PID and its image
  name (`QueryFullProcessImageNameW`). Read-only. The record is banked as `<window>/person-<label>.json` with the evidence result.
- **Evidence span.** Measure / Lyra: `sampling_started_utc` → `anchor_end.utc` (the game is up and settled; it is killed after the span).
  NoHold / legacy: each banked try's `_focus.json` frame span (`first/last_frame_wall` ± 0.05 s). UE's `t_wall` is QPC seconds + 16777216,
  the same clock as the runner's samples and Python's `perf_counter` (verified live).
- **Person evidence** is any of:
  1. *shell foreground* — a runner or monitor sample in the span whose foreground is `SearchHost.exe`, `StartMenuExperienceHost.exe`,
     `ShellExperienceHost.exe` or `explorer.exe`. Names come from the monitor; otherwise a PID is resolved live **only if** that process was
     created before the attempt started (so the PID cannot have been reused); banked records use an attestation.
  2. *cursor moved while the harness was not moving it* — a change between consecutive samples of one stream in the span that is **not
     next to a focus action** (timed samples: no lost-focus sample within ± 0.75 s; the untimed preflight samples: none within ± 15
     samples, ≈ 0.75 s at the runner's 20 Hz).
  3. *non-harness input* — an input-tick change whose time (`t − idle`) lies in the span and is not next to a focus action (same rule; if
     any preflight sample lost focus, the whole preflight phase counts as a focus action).
- **ENV-INTERRUPTED** iff the attempt would otherwise be INVALID-FIXTURE / INVALID-PRECAPTURE (legacy: HALT-FIXTURE-3) **and every fixture
  error is a foreground error** (`whole-leg foreground failed`, `whole-leg foreground`, `fixture: whole-leg foreground`; legacy: all three
  internal tries banked with `_focus.json valid=false`) **and** there is person evidence. No runner has a pre-capture *focus* failure
  (their pre-capture failures are protocol, B1, L1 and L2), so INVALID-PRECAPTURE cannot qualify in practice — stated, not assumed.
- ENV-INTERRUPTED rows do not count against the 3-attempt budget; labels keep counting physically (`…_A4`). **Cap 5 per leg:** the 5th
  raises `Stop(4)`; re-entry with 5 raises `Stop(4)` before any launch; never a NOT-RUN. A foreground failure **without** person evidence
  stays an ordinary invalid attempt.

### (c) Fixture-valid attempts are never reclassified
- `person_evidence` and `person_reasons` are recorded on every attempt row of all four ledgers (INVALID-FOREIGN and HARNESS-FAIL rows too).
- A PASS keeps its verdict; a `person-evidence-on-valid-attempt` event is logged. A fixture-valid FIXTURE-VALID-FAILURE / FAIL /
  NEEDS-DECISION keeps its verdict, gains `needs_decision`, and its stop message appends
  `NEEDS-DECISION: person_evidence=true (<reasons>); verdict unchanged, not auto-voided (081-30 ruling 2c)`. A legacy group FAIL names the
  flagged legs.

### (d) Unchanged
Every evaluator, predicate, threshold, prediction and PASS rule is byte-identical (`measure_eval`, `identity_checks`, the census checks,
`fixture_errors_measure`, `legacy_reading`, `legacy_compare`, `nohold_eval`, `lyra_fixture`, `lyra_g270_eval`, `lyra_twin_eval`). The diff
touches only the import line, `stop_on_recorded`, the four runner loops and the new block. The dry run's 153 prior checks are identical.

### Ruling 1 — ENV-VOIDED
- `stop_on_recorded` treats a NOT-RUN followed by an ENV-VOIDED row for the same leg as cleared, and the attempt budget then counts only
  attempts after the latest ENV-VOIDED row. ENV-VOIDED never clears FAIL, FIXTURE-VALID-FAILURE or NEEDS-DECISION.
- `record_env_voided` refuses a leg that carries one of those, or whose latest row is not a NOT-RUN.

## Ruling 1 applied (`_reviews/081-30-evidence/env-void-c1-g2s.json`)
- 081-26's mechanism: the c1 files the 081-29 invocation wrote (`chunk1-summary`, `postflight`, `evidence-manifest`, `bank-manifest`,
  `stack-results`) were byte-copied to `081-23-evidence/c1/invocation-2-081-29/` first, then one dated row was appended through the lib:
  `C1_G2_S ENV-VOIDED`, recorded 2026-09-25T18:49:56Z, `voided` = `C1_G2_S_A1..A3`, `clears` = NOT-RUN, citing the ruling, lib hash of the
  attempts `7cb583f6…` and of the recording `bb4fbb9d…`. The three attempt rows and the NOT-RUN row are unchanged (the ledger is the
  pre-081-30 ledger plus one row, checked).
- Resume proof on the real ledger (`resume-proof.json`): the unchanged `081-23-chunk1.py`, every launch stubbed, skips the five decided legs,
  gates `C1_G2_S`, and would launch **`C1_G2_S_A4`**. With the pre-081-30 ledger it still halts with code 2 (G290 preserved).

## Proofs (`pp-proof.json` 42/42, `pp-walk.json` 8/8, `resume-proof.json` 2/2)

| # | required | result |
|---|---|---|
| 1 | banked `C1_G2_S` A1–A3 → ENV-INTERRUPTED with the reasons | A1: SearchHost.exe on 43 samples + 36 cursor moves (899,856 → 1249,562); A2: explorer.exe on 570; A3: explorer.exe on 575. PIDs attested (`pid-attestation.json`: 17416 SearchHost.exe, 14080 explorer.exe, both created at boot, before the attempts). A replay through the real `run_measure_leg` gives A1–A3 ENV-INTERRUPTED, then `C1_G2_S_A4` PASS; no NOT-RUN. |
| 2 | no person evidence on the four 081-29 passes and every banked c0/c1 record | 0 of 13 c0/c1 records (incl. `C1_G8_S`, `C1_G9_N`, `C1_G9_S`, `C1_G2_N`); the dry run adds 3 banked NoHold legs, also 0. |
| 3 | foreground failure without person evidence stays INVALID-FIXTURE | banked 081-11 `V2_RUNEND_N_A1` and `V2_TEARDOWN_S_A1` (capture foreground lost to PID 32940, cursor still) stay INVALID-FIXTURE; a synthetic non-shell owner stays INVALID-FIXTURE; three such replays still exhaust the budget (NOT-RUN, Stop 2). |
| 4 | gate: 0.3 s waits; ≥ 60 s proceeds; own injection is not a person; budget exhaustion exits 4 without NOT-RUN | waits 15/15/15/14.7 s then proceeds at 60 s; 75 s proceeds at once; the last input inside a clean runner window proceeds at once, and both controls wait; an in-attempt ALT tap and cursor warp next to a focus loss read "not a person", while an input, a cursor move or an explorer.exe sample away from it read "person"; a person who never leaves → 180 polls, 2700 s, exit 4, nothing launched, ledger empty; 2690 s already spent on the quiet wait → 10 s, exit 4; window start 6 of 6. |
| 5 | 5-cap exits 4, no NOT-RUN | five ENV-INTERRUPTED → exit 4, counted attempts 0; re-entry → exit 4 before launch; interleaved 2 ENV + 3 ordinary → NOT-RUN on the 3 ordinary; NoHold/legacy and Lyra classifiers ENV-INTERRUPTED on mutated banked telemetry, clean on the originals. |
| 6 | fixture-valid FAIL with the flag → verdict unchanged, NEEDS-DECISION | FIXTURE-VALID-FAILURE unchanged, `needs_decision` set, Stop 1 message carries the flag; controls: PASS with the flag stays PASS, FAIL without it carries no flag text. |
| 7 | ENV-VOIDED clears with a fresh budget of 3; NOT-RUN alone halts; ENV-VOIDED on a FAIL does not clear | a copy of the real ledger launches A4, A5, A6 then NOT-RUN; the real ledger without ENV-VOIDED halts 2; ENV-VOIDED after FIXTURE-VALID-FAILURE / FAIL / NEEDS-DECISION halts 1; the writer refuses both misuses. |
| 8 | boundary re-issued, full dry run green | dry run **182/182** (the 153 of 081-28 identical + 29 new); every remaining leg walked: **86/86** (c1 39, c2 42, c3 5; all four kinds) show gate → quiet check → launch; boundary re-issued after this commit. |

## Boundary and dry run
- **Dry run** (`dryrun-081-30.py`, the 081-28 wrapper lineage redirected to `_reviews/081-30-evidence/dryrun/`): **182 of 182 as expected**,
  0 outcome changes and 0 detail changes against 081-28, 29 new checks (classifier on banked records, ENV-VOIDED semantics). 081-23, 081-26,
  081-27 and 081-28 evidence byte-unchanged by it.
- **Boundary:** re-issued after this commit for exactly `081-23-lib.py` (`7cb583f6…` → `bb4fbb9d…`), `081-23-window.py`
  (`63837074…` → `56e86f2e…`) and `feature_head`; offline preflight replay for c1–c3. Details: `_reviews/081-30-evidence/boundary-diff.json`.

## Findings
- 🆕 **G293.** The first version of the window runner called `L.budget_left()`, which the lib did not define. The 42 lib-level proofs passed;
  only executing the real `081-23-window.py` source (the window-start walk) raised the `AttributeError`. On the bench it would have
  failed every window at start, after preflight. Fixed before anything ran (lib `b7307a65…` → `bb4fbb9d…`, re-applied from the pristine
  copy); every proof was re-run on the fixed lib, and the walk now covers the window runner.
- The walk's own first run flagged all 20 legacy legs: the checker assumed the launch is the last traced step, but `run_legacy_group`'s
  `finally` re-checks the process state after it. The checker was wrong, not the harness (G142's shape); corrected and re-run.
- A 0-byte untracked `081-23-lib.py` sits in the plugin repo root (created 2026-09-25 12:32:45 IST, during 081-27). Not this session's; left
  as found.
- The owner was using the PC during this session (input idle 0 ms, a browser in the foreground). Nothing was launched.

## Limits (stated, not solved)
- A person who is present but touches nothing is invisible to an input gate. The evidence prongs catch disruption, not presence.
- Keyboard-only interference that switches to a **non-shell** app is masked by the runner's own ALT tap in the same instant; that attempt
  stays an ordinary invalid attempt (the status-quo direction).
- Input during a clean runner's startup phase is excused by design; if the person stays, their next input outside the window makes the
  next gate wait.
- If `explorer.exe` held the foreground without a person (a focus-lock failure), the attempt would read ENV-INTERRUPTED; the cap bounds
  that at exit 4, not NOT-RUN.
- The monitor is new: banked records carry runner telemetry only (no input ticks), so the input prong has not yet seen real bench data.
  081-31 is its first run.

## Deviations
- **AMENDMENT 4** appended to the requalification predictions: §0's retry rule, scheduling only; no prediction changed.
- The window runner gains an optional third argument (the budget tag) and books its process-quiet waits into the budget file.
- Beyond the eight required proofs: the per-leg walk, the window-start scenarios and the resume proof on the real ledger.

## State / hand-off
- Build 3 unchanged and still the candidate. Feature branch: this docs-only commit. NOT MERGED, NOT TAGGED, no build.
- **081-31:** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\081-23-window.py c1 540 081-31` resumes at `C1_G2_S_A4` with a fresh
  3-attempt budget, then c2 and c3 with the same tag. On exit 4, re-run without extra sleeping: the gate and the quiet wait book their
  time into `wait-budget.json`, and the window refuses once 45 minutes are spent.
