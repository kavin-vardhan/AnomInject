# 081-22 predictions — m55 build 3 in the engine, Part 1 (Claude Code, Opus 5.5)

Written 2026-09-24 ~04:45 IST, after the preflight (0 mismatches, 2 declared external updates) and before any
leg of this round. No build, no source change. Build under test: build 3 = source `85c99b3` (+ oracle `c27024d`),
Game exe **002805CF** (built = staged = `_binary_baselines\StackOBot.exe.m55-build3-002805CF`), Lyra modules
`m55-build3-lyra-cf0b38cb`. Pre-m55 comparison binary: `_binary_baselines\StackOBot.exe.m52-veto-arm-window-0844220E`
(source identical to master `031a103`). Oracle: `tools/verify_capture.py --change-oracle` at `94a56a9` (scratch
worktree). Part 2 reuses `_reviews\081-18-evidence\predictions.md` unchanged (sha256 `225cb4df…2d05`, re-checked
before Part 2); nothing here amends it. Missed predictions are reported as missed, never refitted.

## 0. Quiet PC and validity rules (fixed now, before any result)

- A leg starts only after two consecutive quiet checks (no `UnrealEditor.exe`/`UnrealEditor-Cmd.exe` of any
  engine, no StackOBot/Lyra game process, no UBT/ShaderCompileWorker of ours). Waiting is by tool-call polls of
  <= 9 min, logged to `environment-events.json`, 4 h cumulative budget from the first busy check (04:34 IST).
- Every runner re-checks for an `UnrealEditor` that is not the leg's own process at startup, immediately before
  `capture_start`, and once per second during capture. A hit stops the leg (our PID only) and marks it
  **INVALID-FOREIGN** (not failed); the leg is retried after the PC is quiet again. Such an attempt counts toward the
  usual 3 attempts per leg.
- A pre-capture fixture failure (focus, lock/placement, B1/L1/L2, startup) is INVALID-PRECAPTURE and retried.
  Anything after `capture_start` is a reading of that attempt, never a reason to retry.
- **Stop rule:** any fixture-valid failure of a Part 1 gate below ⇒ NEEDS-DECISION, Part 2 is not started.

## (a) A_SOLID1080_N — build-3 StackOBot 1080p `solid1080` natural smoke

Recipe = 081-17R `B_SOLID1080_N_A1` exactly (CB_GateLevel, targeted `solid_swap` on actor49, Config `2 4 16 4 0`,
order 0, 90 frames, 1920x1080, input lock + placement + pre-capture B1, gate 14 with `-IAIBenchFixture`; gate 14
is inert in build-3 source: no `Gate(14…)` call exists). Runner `081-22-measure.ps1` (derived from 081-18's).

Gates (all must hold on the fixture-valid attempt):
- G-a1 fixture: whole-leg foreground, lock/place/release 1/1/1 with restoration, captured-label B1, no handled ensure.
- G-a2 invariants 0 (the 081-08 audit: retained bytes 0; denominator/histogram/ref-onset mismatch, persist_failed,
  late_record_mutations, rows_dropped all 0; pair/event structure rules).
- G-a3 required pairs **20/20** measured, exactly 5 final events, onset gt8 fraction > 0.9 on every onset.
- G-a4 `change_reason_budget_exceeded` 0, no `BUDGET-EXCEEDED` line, no `budget_census` row; EFFECTIVE
  `maxBytes=268435456(from compiled)`; census self-check clean (peak = high-water, unaccounted 0, one PEAK line).
- G-a5 oracle `--change-oracle` **exit 0**, SUMMARY `claimed 20 compared 20 matched 20 mismatched 0`.
- G-a6 build-3 specifics on a single-view bench: `change_multi_view_families` 0, `change_unsupported_completion` 0,
  `change_view_rejected` 0, no `BENCH-GATE-REFUSED` and no `STAGE-DISCARDED` line, EFFECTIVE line reads
  `gate=14(from …)`, and no event record carries a `late_results` key (F6).

Readings predicted (not gates): high-water 60–80 MB (build 2: 66.4 MB); colour latency p50 330–450 ms (build 2:
387); speed_ratio 1.000–1.010; 90 PNGs and 90 label rows.

## (b) F1_CANCEL — start, then stop before the game window has focus

Recipe: staged build 3, CB_GateLevel, 1280x720 windowed, `IAI.Server.Start` only plus AE-off/SVE/pace/fps 30 —
no bench flag, no fixture, evidence at its compiled default (ON). After world-ready the runner **minimises** the
game window and never gives it focus; it verifies the foreground PID is not the game, sends `capture_start`
(blinking, actor49, 90 frames, png, seed 777) to a fresh bank folder, samples the foreground at >= 20 Hz for
~3 s, reads `capture_status`, sends `capture_stop`, waits 5 s, then ends the process (our PID) and re-checks.

Validity (decided before the folder is read): the foreground PID is never the game from `capture_start` to
`capture_stop`; else INVALID-PRECAPTURE and retried.
Gates: `capture_status` before stop reads `running=true frames=0`; the log carries `=== Capture run CANCELLED
before focus` exactly once and `Capture(m55): STAGE-DISCARDED` exactly once with `issued=0`; **no `session_*`
directory and zero files** anywhere under the capture folder, 5 s after the stop and again after the process
ended; no handled ensure. Reading: whether the (empty) capture root folder itself exists.
Discriminator: build 2 (pre-F1) re-created the deleted session folder from its stage; that shape is a FAIL.

## (c) F2_TWOVIEW — a real two-view family (split screen via `DebugCreatePlayer 1`)

Recipe (081-12 CB legacy recipe at 1080p): CB_GateLevel, 1920x1080 windowed (B1 not applicable by the harness's
own scope rule), targeted `blinking` on actor49, seed 777, Config `2 4 8 4 0`, 90 frames, AA 0, AE off, plus
`DebugCreatePlayer 1` in the startup commands; harness `081-22-legacy-run.ps1` (081-12's A63 runner + foreign
check). Order: C1 build 3 (evidence compiled ON), C2 pre-m55, C3 pre-m55, C4 build 3.

Exercise check (before any comparison): the log has no `Failed to DebugCreatePlayer`, and build 3's
`change_multi_view_families` > 0. If not, (c) is **UNEXERCISED** with that reason — not a failure — and the
remaining (c) launches are not run.
Gates if exercised:
- G-c1 build 3: `change_multi_view_families` > 0; zero measured m55 rows; `change_reason_unsupported_delivery` > 0.
  Prediction (reading): every pair row's reason is `unsupported_delivery`.
- G-c2 legacy delivery equals pre-m55 under the control-pair rule: for each compared field (Actual_Frames count
  and index set; the 081-12 CB field set — per-index anomaly tuples, camera rows, onsets, label key structure,
  annotation key structure, non-`change_*` summary keys, event ranges, MASK-TIE tuples without the run-local tag,
  mask occupancy; the SVE-WANT-SUMMARY counters), if both same-binary pairs (C1/C4, C2/C3) agree, all four
  cross pairs must agree. A field that differs within a same-binary pair is a run-to-run reading, not gated.
- G-c3 pending arms do not grow: build 3's `pendingWantedAtEnd` = pre-m55's and `maxPendingDepth` <= pre-m55's
  (predicted 0 and 1 on both); no handled ensure; build-3 oracle exit 0 (nothing to compare is exit 0).

## (d) Legacy byte-identity, evidence OFF, build 3 vs pre-m55, MainWorld, both tick orders

Recipe = 081-12 MAIN legacy comparison: MainWorld, `blinking` on `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803`
(SM_rock_02), seed 777, Config `2 4 8 4 0`, 90 frames, 1280x720, AA 0, AE off, `IAI.Capture.ChangeEvidence 0`
issued to both binaries (unknown command on pre-m55, as in 081-12). Harness `081-22-legacy-run.ps1`.
Native order D_N1 pre, D_N2 b3, D_N3 b3, D_N4 pre; synthetic order (`IAI.Bench.SynthTickOrder 1`) D_S1 pre,
D_S2 b3, D_S3 b3, D_S4 pre. Staged exe restored to 002805CF and re-hashed after the last leg.

Declared before running:
- **Must be byte-identical build 3 (OFF) vs pre-m55, native order:** every `target_mask/*.png` file (bytes) and
  the mask file set; and the full 081-12 MAIN field set on all six pairs (per-index anomaly tuples, camera rows,
  onsets, label key structure, annotation key structure, run_summary key set, event ranges, MASK-TIE tuples).
- **Full deep diff, both orders:** every JSON path of `labels.jsonl` (rows keyed by `session_index`),
  `annotation.json`, `run_summary.json` and `run.json` that differs between the two binaries must also differ
  within a same-binary pair (run-unique set, measured, e.g. `t_wall`, wall-derived fps, session id). Any extra
  differing path FAILS.
- **Keys:** with evidence OFF build 3 must add **no** key anywhere: no `change_*` summary key, no sidecar, no new
  label/annotation/run key. The allowed key difference is **none**; the OFF state is read back (engine echo
  `IAI.Capture.ChangeEvidence = "0"`, no EFFECTIVE line, no `change_evidence.jsonl`).
- Synthetic order: the same rules; G230 says pixels are not reproducible run to run in that order, so any field
  the control pairs show varying is a reading there. Prediction: cross ⊆ run-unique holds in both orders; the
  native-order mask files are byte-identical on all 6 pairs.
- Colour frames (`Actual_Frames/*.png`): reading only — byte-identical fraction per pair.

## AMENDMENT 1 — written ~04:58 IST, before any leg of this round (a tightening, not a loosening)

A known-answer test of the (c)/(d) comparator on 081-12's banked MainWorld legs
(`legacy-comparator-selftest.json`) showed the rule declared above ("a cross-binary difference must also differ
within a same-binary pair") has a hole: a difference present in only ONE build-3 leg (tested by swapping one OFF
leg for an evidence-ON leg) lands in the build-3 control pair and is absorbed as "run-unique". The gates are
therefore tightened, all fixed before any leg:
- **Key presence (c and d):** the set of JSON paths (array indices normalised) in every build-3 leg must equal the
  union over the pre-m55 legs and vice versa — no added and no removed key. (c) excepts only `run_summary/change_*`
  (evidence is ON there by design). Catches the one-leg case.
- **(d) native order, strict rule:** every cross-binary differing path must lie in the run-unique set measured on
  binaries other than build 3: this round's pre-m55 pair **plus** three same-binary pairs from 081-12's bank
  (`legacy-historical-run-unique.json`, 19 path patterns: session ids, `t_wall`, wall-derived fps/ratio, tick and
  key-ring counters, census cycles/frames, shader prewarm ms, two m55 counters). A same-recipe pair of n = 2 alone
  under-samples run-to-run noise (measured: `end_frame` 121/121/122/121 on the known-good set), which is why the
  historical pairs are included; they are measured, not chosen.
- The declared rule stays as written. Known answers: 081-12 pre vs m55-OFF passes all three rules; one-leg-ON is
  caught by key presence and the strict rule; both-legs-ON is caught by all three. (The strict rule's known-good pass
  is partly circular — its reference includes the same bank — which is stated, not hidden.)