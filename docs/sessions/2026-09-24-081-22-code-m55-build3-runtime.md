# 081-22 — m55 build 3 in the engine: Part 1 runtime legs; Part 2 not started (stop rule)

Implementer: Claude Code (Opus 5.5), headless, 2026-09-24. Governing ruling:
`_reviews/081-21-chat-ruling-build3-accepted-bench-when-quiet.md`. Evidence: `_reviews/081-22-evidence/`
(scripts `_reviews/081-22-*.py|ps1`). Predictions: `_reviews/081-22-evidence/predictions.md` (copied
byte-identical to `docs/predictions/2026-09-24-m55-081-22-build3-runtime.md`), written and hashed before any
leg (`e73932cc…`), plus **AMENDMENT 1** appended before any leg (`fa3071f4…`, a tightening). No build, no
source change, no merge, tag or cook.

## 1. Entry state and the wait

Main checkout `m51` `53bf725`, feature `94a56a9` == origin, master `031a103`, Lyra `caa68c6` with nine entry
modules, staged exe **002805CF** (build 3) = built = archive. Preflight: 180 binaries, 2,387 prior artifacts,
14,031 bank files — 0 mismatches after two declared external updates: chat's scorecard
`_reviews/081-17-codex-vs-code-comparison.md` (edited 04:33 IST, after 081-21's postflight), and 081-21's
`postflight.log`, whose manifest hash is the SHA-256 of **zero bytes** (081-21's `close.py` hashed its own
stdout log before anything reached it; no non-empty prefix of the file matches).

Quiet PC = no `UnrealEditor` of any engine, no foreign game process, two consecutive quiet checks. Four UE 5.7
HeistCrewUE editors (PIDs 153756, 96312, 76640, 104420; started 01:32–02:31 IST, ~1 core each, one window
titled "Error") ran from the first check (04:34 IST) until they exited ~07:09–07:13. Polls 1–18 (≤ 9 min
each) were NOT-QUIET; poll 18 once crashed with "Thread failed to start" in a child PowerShell (commit
charge ~71.7 GB of 85 GB at the time) — the check now retries and counts a failed check as not quiet.
**Quiet at 07:13:27 IST after 2 h 39 min of waiting.** Part 1 ran 07:13:34–07:18:06. A new HeistCrewUE editor
(PID 97980, `/Engine/Maps/Templates/OpenWorld`) started at **07:18:28**, 22 s after the last leg ended; no leg
overlapped a foreign editor.

## 2. Part 1 results (all legs accepted on their first attempt)

| Leg | Result | Key numbers |
|---|---|---|
| (a) `A_SOLID1080_N` — 1080p `solid1080` smoke | **PASS** | 20/20 pairs measured, 5 events, onset gt8 fraction 1.0 ×5 (control 0.009–0.013), budget 0/0/0, invariants 0, census self-check clean, 71 frozen masks verified; oracle **exit 0**, 20/20/20/0 (refs 20/20/20/0); `change_multi_view_families` 0, `change_unsupported_completion` 0, `change_view_rejected` 0, no `BENCH-GATE-REFUSED`/`STAGE-DISCARDED`; EFFECTIVE `gate=14(from console)`; high-water **66,392,064 B** (build 2: the same figure); latency p50/p95/max 386/430/436 ms; speed_ratio 1.0006; 90 PNGs; foreground 79/79 |
| (b) `F1_CANCEL` — stop before focus | **PASS** | window minimised, foreground never the game (51 samples, 0 violations); `capture_status` running=true frames=0; the session folder existed before the stop (`Actual_Frames` created, 0 files); one `CANCELLED before focus` line; one `STAGE-DISCARDED … issued=0` line; **no session folder and zero files 5 s after the stop and after the process ended** (the empty capture root remains); no STARTED banner |
| (c) `F2_TWOVIEW` — split screen (`DebugCreatePlayer 1`) | **EXERCISED, PASS** | delivered frames are 1920×540 (one half of a horizontal split); build 3: `change_multi_view_families` **120**, `change_view_rejected` 150, all 30 m55 pair rows `unsupported_delivery`, 0 measured, oracle exit 0 (nothing compared); legacy equals pre-m55 on every compared field (frames 90, index set, per-index anomaly tuples, camera rows, onsets, key structures, event ranges, MASK-TIE, mask occupancy); SVE-WANT-SUMMARY identical on both binaries: marks 90, wanted 90, **submits 180** (the pre-m55 multi-view double submit, reproduced exactly), frames 90, `pendingWantedAtEnd` 0, `maxPendingDepth` 1; key presence OK |
| (d) native order — evidence OFF vs pre-m55 `0844220E`, MainWorld | **FAIL (declared deep-diff gate)** | masks byte-identical and the whole 081-12 field set equal on all six pairs; OFF read back on build 3 (echo `"0"`, no sidecar, no `change_*` key, no EFFECTIVE line); key presence OK; **9 JSON paths differ build 3 vs pre-m55 on all four cross pairs and on neither same-binary pair**; 3 of them lie outside the historical run-unique set too |
| (d) synthetic order | **PASS** | all rules, 100 run-unique paths, key presence OK |

### 2.1 The (d) native-order difference, stated as observed

| Path | pre-m55 D_N1 / D_N4 | build 3 D_N2 / D_N3 | all four synthetic legs (both binaries) | 081-12 bank (six m55 legs) |
|---|---|---|---|---|
| `annotation/anomalies[2]/coverage_pct` | 8.6927 / 8.6927 | 8.6518 / 8.6518 | 8.6518 | 8.6518 |
| `annotation/anomalies[6]/coverage_pct` | 9.25045 / 9.25045 | 9.24997 / 9.24997 | 9.24997 | 9.2500 |
| `annotation/anomalies[7]/coverage_pct` | 9.25495 / 9.25495 | 9.25479 / 9.25479 | 9.25479 | 9.2548 |
| `run_summary/end_frame`, `capture_game_ticks` | 122 / 122 | 121 / 121 | 122 | 121 or 122 within one binary |
| `key_ring_published`/`consumed` | 121 / 121 | 120 / 120 | 121 | 120 or 121 within one binary |
| `key_ring_wrapped`, `ticks_per_captured_frame` | 57, 1.3556 | 56, 1.3444 | 57, 1.3556 | vary within one binary |

- The pre-m55 pair also differs **with itself** at events 3 and 5 (9.0584 vs 9.0717; 9.2369 vs 9.2354), so
  `coverage_pct` is run-variable on that binary in native order; events 2, 6, 7 differ identically in both
  pre-m55 legs. Anchor indices (27/75/87), occlusion samples (5/9) and poll distance (863.91) are identical on
  every leg; labels' camera rows are identical on every leg.
- Source fact (read, not measured): `coverage_pct` comes from `EvaluateSelectionProvenance`, called in the
  capture drain when the anchor frame's completed record is processed, against the **live** world and view at
  that moment — not the arm-time view the label rows carry. m55 changes `CaptureCurrentFrame` and
  `ProcessCompletedFrames`. ⛔ **A timing difference at processing time is a CANDIDATE, not established**
  (`G120`); the tick-counter split is n = 2 per binary on counters already known to vary within one binary.
- `AEBD09EA` (the 081-12 "predecessor") is the **m55 stage-1** build (source `3dc1dda`), not pre-m55: this is
  the first time a pre-m55 binary ran the MainWorld legacy recipe. Every m55 build on record (stage 1, stage 2,
  build 3) reads 8.6518 at event 2.
- The declared gate failed as written; it is not reinterpreted. `coverage_pct` ships in `annotation.json` in
  both delivery modes, so this is a client-visible difference of at most 0.47 % relative.

### 2.2 The comparator, proven before use (AMENDMENT 1)

A known-answer run on 081-12's banked MainWorld legs showed the declared subset rule absorbs a difference
present in only one build-3 leg (one OFF leg swapped for an evidence-ON leg passed). Two tightenings were fixed
before any leg: **key presence** (no added/removed JSON path) and, for native order, a **strict rule** whose
run-unique set is measured on non-build-3 same-binary pairs (this round's pre-m55 pair + three 081-12 pairs,
19 path patterns). Known answers: 081-12 pre vs m55-OFF passes all three rules; one-leg-ON is caught by key
presence and the strict rule; both-legs-ON by all three. An n = 2 same-recipe pair alone under-samples noise
(`end_frame` 121/121/122/121 on the known-good set) — `G286`.

## 3. Stop, and what did not run

The brief's stop rule ("any fixture-valid failure in Part 1 ⇒ NEEDS-DECISION, do not start Part 2") fired on
(d) native. **Part 2 (L1–L4 Lyra, S1–S2 StackOBot, 1800 frames, ON/OFF) was NOT started**; its harness is
derived and syntax-checked (`081-22-lyra-run.ps1`, `081-22-measure.ps1`, `081-22-part2.py`, stage/restore,
analyzer re-validated on 081-17R known answers) and the 081-18 predictions are unchanged
(`225cb4df…`). The PC was busy again from 07:18:28 in any case.

## 4. Harness

Runners derived from 081-18's by asserted single-occurrence replacements (`harness-derivation.json`): evidence
root and bank prefix `M55B3R22_`; a foreign-editor check at startup, before `capture_start` and once per
second during capture (a hit stops our PID and marks the leg INVALID-FOREIGN); the Lyra runner records a
post-capture L2 failure instead of throwing (081-18's driver would have retried a post-capture host respawn,
against its own brief). New: `081-22-f1.ps1` (minimised-window cancel), `081-22-legacy.py` (c/d ABBA with
staging, readings, comparator), quiet-wait and pre/postflight scripts. `PYTHONDONTWRITEBYTECODE` set so no
`__pycache__` lands in CaptureBench.

## 5. Boundary

Postflight `_reviews/081-22-evidence/postflight.json`: main `m51` `53bf725`, Lyra `caa68c6` with its nine
modules (never staged this round), staged exe 002805CF after every swap to the pre-m55 archive, every binary,
container, prior artifact and bank file byte-identical to preflight, scratch worktree removed.

## 6. Next

Chat decides on the (d) native-order difference: accept it as a timing-sensitive provenance field (and declare
`coverage_pct` run-variable in the legacy comparison), or have it diagnosed (e.g. more pre-m55 native legs, or
sampling provenance from the arm-time view), before Part 2 and the requalification inventory.
