# 095-05 — m56 capture continuity: plugin commits 1 to 4 (written, Editor-compiled, selftests passed)

**Date:** 2026-11-13 · **Session:** Claude Code (Opus 5.5, xhigh), headless, light brief (095-03b E4).
**Branch:** `fix/m56-capture-continuity` off `master` `ebc34ed`. **Spec:** `_reviews\095-02-plan-A1-capture-continuity.md`
(§2–§6, §9, §11) as changed by `_reviews\095-02-ruling.md` (Unit A-1). **Receipts:**
`D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-05\`.

## State in one paragraph

Commits 1–4 of plan §11 are on the branch. Every commit compiled on the plugin's Editor target with 0 errors and 0
warnings, and the pure selftests pass. **No engine leg has run. The Game target was not built. The Python readers are
not updated yet (plan commit 5, next brief): `verify_capture.py`, `label_sync_check.py` and
`annotation_transition_check.py` will reject sessions written by this build (schema minor 2, reason `settle`).** No cook,
no launch, no merge, no tag.

## Commits

| # | Hash | Subject |
|---|---|---|
| 1 | `b2b5116` | fix(capture): m51 F1 - wait only for a mask outcome that can exist (`cherry-pick -x 6713f51`) |
| 2 | `26e5f4b` | fix(capture): m56 capture every settle tick and flag it settle |
| 3 | `2ef529c` | feat(capture): m56 tick index, run wall time and one honest drop count |
| 4 | `28fa39e` | fix(capture): m56 writer finishes every job at run end and counts failures |

**Commit 1.** `git cherry-pick -x 6713f51` hit one conflict, the drain hunk, exactly as plan §6 predicted. Resolved by
keeping `master`'s m52 ORDER HOLD block and placing the new not-awaiting / awaiting branches after it. The resolved
diff's changed lines equal `6713f51`'s 47 changed lines one for one (`Compare-Object` empty). Nothing else came along.
The commit keeps the original author date.

**Commit 2 (§2.1–§2.4, §2.6).** `SettleAfterFire` / `SettleAfterRevert` capture with a frame role
(`EFrameRole::SettleAfterOnset` / `SettleAfterOffset`) before the decrement. Reason bit 9 `settle` (512),
`NumReasons` 10. `DecideSettleEntry` never changes an entry's emit mode. `StepSettleFlags` ORs `settle` into every
emitted entry of a settle frame (drain after the fire-window step; sync path after `MarkCaptureUnpaired`).
`BeginRevert` copies the live fires and freezes their geometry before `RevertAllLiveFires`; on an after-offset settle
frame the tick-end sample adds every copied fire that is no longer live as a transition-only `settle` entry with that
frozen geometry. Row-level `transition_present` and a `transition_reason` containing `settle` on every settle frame,
with or without an entry. `annotation.json` `label_schema_minor` 2; the root `transition_frame_count` includes every
settle frame. `run_summary`: `label_transition_settle_entries`, `settle_frames` (counted by the writer on success).
Echo wording for K in the console reply, run banner, idle status and help text.

**Commit 3 (§2.5, §2.7, §3.1, §4).** Rows gain `tick_index` and `t_wall_run`. `run_summary` gains
`capture_span_ticks`, `frames_armed`, `frames_dropped`, `frames_dropped_ticks` (cap 4096) + `_truncated`,
`frames_dropped_by_cause` (known causes always present), `orphan_completions`. `ticks_per_captured_frame` redefined as
span / written; `capture_game_ticks` unchanged. Causes recorded at the loss: `unresolved_at_end`, `cancelled_epoch`,
`bench_gate`, `not_armed`, else `unknown`. The fps stamp follows written frames (game: target × (N−1)/(S−1); wall:
(N−1)/wall span above the tolerance); when N = S both take today's code path, so a continuous run stamps exactly what
it stamps today. A frame armed but never written leaves every `annotation.json` frame list and is counted.

**Commit 4 (§5).** `FlushPending` runs to completion (progress every 2 s, 120 s hard cap → `writer_timeout`,
`writer_flush_complete`); teardown uses the same cap. Per-frame `write_failed` from the writer; mask PNG write failures
move from measured to unavailable. The `labels.jsonl` append result is checked (`label_rows_failed`, both paths).
In-flight cap: 1 GiB of pending pixel bytes with a floor of 4 jobs; the drain blocks before enqueueing a colour frame;
echoed in the run banner (`writerCap=1024MiB(min 4 jobs in flight; blocks, never drops)`) and in `run_summary`
(`writer_pending_byte_cap`, `_count_floor`, `_bytes_peak`, `writer_backpressure_waits`, `_seconds`). Bench levers
`IAI.Bench.DropFrameAt <si>` (cause `bench_drop`, after the label row is built so the annotation filter is exercised)
and `IAI.Bench.SkipArmAt <tick>` (cause `not_armed`), bench-gated, in the Shipping-excluded capture module.

## Compile (Editor target, Development Win64, `-MaxParallelActions=2`, memory watchdog)

| After | Start free | Min free | Samples < 1 GiB | Actions | Time | Errors / warnings |
|---|---|---|---|---|---|---|
| commit 2 | 5.15 GiB | 2.01 GiB | 0 of 20 | 14 | 197 s | 0 / 0 |
| commit 3 | 4.41 GiB | 0.95 GiB | 1 of 15 | 11 (UBT dropped to 1 process) | 145 s | 0 / 0 |
| commit 4 | **2.47 GiB** | 0.68 GiB | 1 of 21 | 11 | 206 s | 0 / 0 |

⚠ **Commit 4's compile started below the brief's 4 GiB launch floor.** My sample about 30 s earlier read 5.29 GiB; the
watchdog's first sample at launch read 2.47. The script now re-checks at launch (waits up to 20 min, else skips). No dip
under 1 GiB lasted longer than one 10 s sample; the 2-minute kill rule never triggered.

**Lever audit:** source PASS (55 lever names, was 53; both new levers `scope=static shipping=excluded gate=swept`),
`--selftest` 12 cases OK, `--binary` on the five fresh Editor DLLs PASS (55 in binary = 55 in source, missing 0).
`tools/lever_audit.py` needed no edit (it discovers levers from source by rule).

## Selftests (pure C++ under `tools/` that include `AnomalyLabelSync.h`)

- With the new header and the old pins: exactly the three `NumReasons == 9` pins failed, nothing else
  (`pie_end_settle_selftest`, `r4_capture_pairing_selftest`, `m52_window_selftest`).
- Pins moved 9 → 10; `pie_end_settle_selftest` gained `TestSettleReason` (9 checks: bit, name, disjoint from every
  reason incl. `pie_end_settle`, never legacy-coerced, roles, emit modes kept, labelled entry still labelled, a
  non-settle frame untouched).
- Results at the head: `pie_end_settle` 29/0, `r4_capture_pairing` 46/0, `install_state` 27/0, `m52_window` 316/0.
- Can-fail: a header copy whose `DecideSettleEntry` forgets the bit fails 2 of 29 checks.

## Read-back against the plan's "untouched" list (§2.3 last bullet, §9 items 1–8)

Hunk headers of `git diff -U0 master..HEAD -- Source` (receipt `untouched-hunks.txt`):
- **No hunk** in `FinalizeArmedLabel`, `OnWorldTickEndMask`, `OnWorldTickEndSample`, `PaceThisTick`,
  `ServiceTargetMask`, `ArmTargetMaskOwn`, `EnsureMaskRecordsForCapturedFrame`, `StepFireWindowTransitions`,
  `StepHideTransitions`, `FillAnnotationInputs`, `ComputeFireActive`, `IsFireLabelledThisFrame`, `AccumulateFrameEvents`.
- **Byte-unchanged files:** `Source/AnomalyInjector/Private/**` (every anomaly, the injector, auto-injector, viewport),
  `AnomalyMaskMeasure.cpp` (m26 arms and veto), `AnomalyCensus.cpp`, `AnomalySveCapturer.cpp`, `AnomalyFrameCapturer.cpp`.
- 1. Onset frame and revert frame: unchanged. **`BeginFire` / `BeginRevert` gained bookkeeping only** (clear / copy the
  reverted-fire list and its frozen geometry, as plan §2.3 prescribes); their phase, fire and revert behaviour is
  unchanged. `FinalizeArmedLabel` untouched.
- 2. Anomaly clocks: no anomaly file touched; they still count ticks.
- 3. m26 veto arms: same ticks (K and phase lengths unchanged; arming code untouched).
- 4. Temporal defaults 0/0/0 (`AnomalyLabelSync.h` lines 13–15) unchanged.
- 5. m47 onset-frame first-use: onset frame unchanged.
- 6. m53 bench levers: code unchanged (their captured-frame predictions must be re-derived, as the plan says).
- 7. Mask load: the target mask now arms on 16 frames per cycle (expected); `6713f51` went first.
- 8. Exposure dip: code unchanged; its 8-frame window becomes 8 contiguous ticks.
- Membership rules (`SYNC:53-80` region) unchanged. Labelled membership does not read the reason bits
  (`IsSnapshotEntryLabelled`), so a `settle` bit cannot change a label.

## Predictions

Frozen before any leg: `code-receipts-095-05\predictions-095-05.txt` (P1–P20: plan §7 plus a tick-space blink
derivation — native hidden si = onset + {2,3,4,8,9}, synth onset + {3,4,5,9}; both reproduce today's banked
{4,5,9,10} / {4,5,6,10} when ticks n+1, n+2 are left uncaptured).

## Deviations from the plan

1. **Capturer drop causes not split** (§4 D7/D8 `sve_*` / `bb_*`): a frame a capturer never completes is counted
   `unresolved_at_end`; the capturers' existing log lines still name the specific reason. Every missing tick still gets
   exactly one named cause. Splitting needs edits in both capturers and the completed queue.
2. **`orphan_completions`** is emitted as its own `run_summary` key (plan §4 names it; §3.1's table does not).
3. **Writer keys beyond `writer_pending_byte_cap`**: count floor, peak bytes, back-pressure waits/seconds and
   `writer_flush_complete` are also emitted, so the bound and its effect are readable from the artifact.
4. **`lever_audit.py` unchanged** (see above).
5. **The selftest pins** `NumReasons == 9` in three files moved to 10 (they pin the reason count by design).
6. **Mask-PNG failure wiring** moves a failed mask from `target_mask_frames_measured` to `_unavailable` so the three
   still sum to the frame count.

## Not proven / left

- No engine leg: continuity, edges from pixels, masks, can-fails, non-settle identity and the settle-exactness table
  are all owed (095-07, after a Game build and the cook).
- Plan commit 5 (readers) and commit 6 (docs, gotchas) are the next brief. The client kit must not receive this build
  before the readers and the dashboard brief land (ruling A-R4).
- The 4.25 forward-port is untouched.
