# 095-06 — Independent read-only review: m56 capture continuity, m58 terrain blink subtype, CaptureBench m56-gates

**Date:** 2026-11-13 · **Reviewer:** Claude Code (Opus 5.5, xhigh), fresh side session, headless · **Mode:** read-only.
Nothing built, launched, committed, checked out or stashed. No Codex. Primary checkout left on
`fix/m56-capture-continuity` (`b4017ea`), clean apart from the owner's untracked files.

**Reviewed:** `git diff master...fix/m56-capture-continuity` (tip `b4017ea`), `git diff master...feat/m58-terrain-blink-subtype`
(tip `a948046`), both from `master` = `ebc34ed`; CaptureBench `master...m56-gates` (`1d1e119`).
**Specifications read:** `095-00-ruling.md` §2/§4/§6, `095-02-ruling.md`, `095-05-ruling.md`, both 095-02 plans, the 095-04e /
095-05 / 095-05b reports, the frozen predictions `code-receipts-095-05\predictions-095-05.txt`, `m56_gate_predictions.md`.

**Marks.** **[V]** = I re-checked it in source on the branch (and, where stated, with an offline probe). **[R]** = reported or
inferred, not re-checked. Line numbers are on each branch's tip. `SUB` = `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`,
`AW` = `AnomalyAsyncWriter.cpp`, `VC` = `tools/verify_capture.py`, `KIT` = `tools/label_sync_check.py`.

---

## Verdict

| Unit | Verdict | BLOCKER | SHOULD-FIX | NOTE |
|---|---|---|---|---|
| m56 (plugin + readers) | **READY AFTER FIXES** | 0 | 5 | 11 |
| m58 (plugin) | **READY FOR GATES** | 0 | 0 | 7 |
| CaptureBench m56-gates | **READY AFTER FIXES** | 1 | 1 | 1 |

- No defect found that shifts a positive or negative label on a non-settle frame at release defaults.
- The settle mechanism, `tick_index`, the drop accounting, the stamp and the carried `6713f51` read correct.
- The SHOULD-FIX items are about event bookkeeping, a held-global mode, a reader hole, a stall bound, and override coverage.
- The bench BLOCKER is the kill-by-name in `run_leg.ps1`, which 095-07 would execute.

---

## Findings

### BLOCKER

**B1 — bench — `run_leg.ps1` kills processes by name before and after every attempt. [V]**
- Where (CaptureBench `m56-gates`, `tools/run_leg.ps1`):
  - `:230` `Get-Process -Name "StackOBot*" -EA SilentlyContinue | Stop-Process -Force` (before each attempt, "A46");
  - `:232-233` throws if any `StackOBot*` process is still alive;
  - `:354` the same kill after each attempt.
- Scenario: the owner (or another brief) has any `StackOBot*.exe` running — a delivered build, the MidRepro build, a manual
  check. 095-07's first leg kills it without warning. That breaks the standing "never kill by name" rule.
- Smallest fix (PID-based):
  1. Keep a harness-owned list of launched processes: `$p = Start-Process ... -PassThru`, record `$p.Id`, `$p.StartTime`, path.
  2. Before an attempt: list `Get-Process -Name StackOBot*`. If any is not in the owned list, **do not kill**: throw
     "A46: foreign StackOBot PID(s) n present; not touching them; leg not started". Owned leftovers are killed only after
     re-checking `Id`, `StartTime` and `Path` (guards PID reuse).
  3. After an attempt: `if (-not $p.HasExited) { $p.Refresh(); if ($p.Path -eq $staged -and $p.StartTime -eq $t0)
     { Stop-Process -Id $p.Id -Force; $p.WaitForExit(15000) } }`. `$staged` is the real game binary (`:166`), not the
     launcher stub, so `$p` is the game process itself; no child sweep is needed.

### SHOULD-FIX

**S1 — m56 — Every vetoed event comes back into `annotation.json` as a `manifested:false` shell. [V]**
- Where: the m26 veto removes the event only from `SessionEvents` (`SUB:9291`). `SessionTransitions` keeps its key.
  `WriteSessionAnnotationFile` then adds an event for every transition key not in the event list (`SUB:10580-10595`,
  `MapAnomalyToClient` at `:10588`).
- Why m56 makes it routine: before m56 a vetoed event rarely had a transition entry (release defaults 0/0/0; PIE-only
  `pie_end_settle` aside). m56 gives every event settle entries: the after-onset ones on its normal entries, the
  after-offset ones as transition-only entries.
- Scenario: a MainWorld auto-pool capture with 3 vetoes (the m27 owner smoke had 3 and 4). `annotation.json` lists 3 extra
  events, each `manifested:false` with empty `frame_indices` and `transition_frames` = its 2 onset-settle and 2 offset-settle
  frames. `run_summary` reads `vetoed_events 3`, `non_manifested_events 0`. The readme and the C++ log line ("a vetoed event
  leaves NO trace in annotation.json") are now false. The dashboard Library and any event count show phantom events.
  After merging m58, a vetoed terrain blink shell also counts in `blink_terrain_events`.
- No label is wrong: the shell has no positive frames, and its frames are settle frames the client drops anyway.
- 095-05b's G575 rests on the opposite premise ("the veto removes the event from annotation.json"). On real m56 output the
  shell exists, so the reader tolerance (S2) never fires.
- Smallest fix: collect the vetoed event ids in the veto loop; skip them in the transition-only loop. Their row-level settle
  frames stay in the root `transition_frame_count` through `SettleFrameSet`. Write `vetoed_event_ids` to `run_summary` so
  readers can excuse exactly those ids.

**S2 — m56 readers — The new veto tolerance lets a session that lost a real event pass. [V, offline probe]**
- Where: `VC:3374-3377` (`annotation_transition_check`): unlisted transition events are excused whenever their count is at most
  `vetoed_events`. The office kit uses the same function (`KIT:2411`). It also applies to schema 2.1 sessions.
- The pre-existing LABELLED-EXTRA rule (`VC:3733-3738`) already excuses every unlisted labelled entry whenever
  `vetoed_events > 0`. The annotation check was the last guard; the tolerance removes it.
- Probe (`code-receipts-095-06\run56\probe_095_06.py`, output `probe_095_06.out.txt`), built on the selftest's own
  session writer:
  - case D: one vetoed event written as a shell (the real m56 output), **plus a second, non-vetoed event missing from
    `annotation.json`**, `vetoed_events 1` → `--label-rule` **exit 0, no fail**;
  - case E (control): the same missing event with `vetoed_events 0` → exit 1, ANNOTATION-TRANSITIONS + LABELLED-EXTRA.
- Smallest fix: excuse only the ids listed in `vetoed_event_ids` (S1), and only on `label_schema_minor 2`. If chat keeps the
  shells instead, delete the tolerance: real m56 sessions never need it.

**S3 — m56 — A targeted capture of a held global anomaly flags settle frames where nothing starts or stops, and the
label-rule reader fails it. [V, offline probe]**
- Where: `BeginFire`'s held-global branch (`SUB:5403-5408`) still sets `SettleAfterFire`; `BeginRevert` still sets
  `SettleAfterRevert`. In this mode the global is held for the whole run (`SUB:5938-5941`), the per-burst phases change nothing,
  and `ZeroMatchBursts` is not incremented.
- Scenario: a targeted `camera_clipping` capture (the m30 gate shape, reachable from the dashboard's targeted mode).
  4 of every 16 frames are flagged `settle` and dropped by the client's rule for no reason. `verify_capture --label-rule`
  finds no onset to anchor them (`_lr_settle_placement` skips session globals, `VC:3419`) and fails every one as a stray
  (`VC:3469-3473`). Probe case A: exit 1, SETTLE-MISPLACED. (`m56_continuity_check.py` passes it, by schedule.)
- Smallest fix: in held-global mode capture the settle-phase ticks with role `Normal`. Alternative: the reader treats a
  held-global targeted run like a zero-match run.

**S4 — m56 — The writer back-pressure cap is 120 s per frame, not per run, so a stuck writer freezes the game for hours. [V]**
- Where: `AW:53-87` (`WaitForCapacity`, cap at `AW:71`), called on the game thread for every colour frame (`SUB:5065`).
  After 120 s it enqueues anyway; the next frame waits another 120 s.
- Scenario: a writer that stops making progress (a hung network or USB target, an antivirus lock) with 80 frames left:
  about 2.7 h of frozen game or editor. Stop cannot run (it needs the game thread). PIE end and teardown add up to
  2 × 120 s more (`SUB:5160`, `SUB:1018`).
- A full disk does not trigger it (writes fail fast). Normal slow-disk back-pressure is correct and costs wall time only.
- Smallest fix: once the cap trips, turn back-pressure off for the rest of the run, log it once, and report it in
  `run_summary` (for example `writer_backpressure_abandoned: true`). Keep the end-of-run flush cap.

**S5 — m56 — Optional temporal windows now cover K fewer ticks, so existing frames lose their `temporal_aa` flag. [V]**
- Where: the fire-window, proxy-blur and hide transition tracks count captured frames by `session_index`
  (`SUB:8089`, `SUB:8191`; `AnomalyLabelSync.h` `FEventTransitionTrack::Observe`). The settle frames are now inside those counts.
- Scope: only with the client-documented optional overrides (readme §8.7: `IAI.Label.TransitionOnFrames` / `OffFrames` /
  `HideFrames`). Release defaults are 0/0/0 and are unaffected.
- Scenario: under TSR with `TransitionOnFrames 3`, the release flagged ticks 0, 3, 4 after an onset (ticks 1–2 were not
  captured). m56 flags ticks 0, 1, 2 (which are also settle). Ticks 3 and 4 exist in both builds and are now unflagged
  labelled positives, while their pixels may still be under 50 %. The off and hide windows shrink the same way.
- G5 runs at defaults, so it cannot see this.
- Smallest fix: let the windows skip settle frames when counting; or document in §8.5a and §8.7 "from 2.2 a window counts
  ticks; add K to a value chosen on an older build".

### SHOULD-FIX (bench and predictions)

**S6 — bench — 095-07's frozen predictions and the G5 comparator will fail correct m56 output. [V]**
- (a) `exposure_dip` / `exposure_dip_scope` on non-settle frames can change. The window is the previous 8 `session_index`
  values (`SUB:4949-4958`), which now include settle frames. Plan §9 risk 8 calls this intended. But P11 and
  `m56_nonsettle_compare.py` allow only `tick_index`, `t_wall_run`, renumbering keys and the control pair's run-unique set.
  A hide-type G5 leg that marks a dip reads FAIL, and the stop rule ends the campaign.
- (b) P19 says `run_summary` gains "exactly" its list, which omits `orphan_completions`, `writer_pending_count_floor`,
  `writer_pending_bytes_peak`, `writer_backpressure_waits`, `writer_backpressure_seconds` and `writer_flush_complete`
  (accepted by ruling 095-05).
- Smallest fix: amend the predictions before the first leg; declare `exposure_dip*` as an m56 difference that the
  comparator reports separately and does not fail on.

---

## NOTES

### m56
- **N1 [V] Mixed async and sync frames undercount `FramesWritten`.** On the backbuffer path a tick whose viewport rect cannot be
  resolved falls back to a sync grab (`SUB:5604`). `ProcessCompletedFrames` (`SUB:5076`) and `DrainAsyncToCompletion`
  (`SUB:5178`) overwrite `FramesWritten` with the writer's count, dropping the sync frames. m56 now derives the fps stamp and
  `ticks_per_captured_frame` from it: such a run reads ratio > 1, a lossy stamp, the "CONTINUITY ACCOUNTING DISAGREES" warning
  and a reader DROP-COUNT fail, while the tick-based `frames_dropped` reads 0. The overwrite is pre-existing; SVE (the default)
  never reaches it. Fix: add `Async->SyncFramesWritten`.
- **N2 [V] A timed-out writer can corrupt the next run's accounting.** The writer lives across runs (`SUB:4629`;
  `ResetCounters` at `SUB:3745`, `AW:276`). Late jobs of a run whose flush timed out add to `FailedSessions` and remove from
  `InFlightSessions` after the next run's reset, so the next run can mark one of its own written frames `write_failed` and drop
  it from `annotation.json`. Needs a 120 s timeout first. Fix: tag jobs with a run generation.
- **N3 [V]** A `writer_timeout` frame can still appear on disk (image and `labels.jsonl` row) after the summary is written.
- **N4 [V]** On a lossy run the three target-mask counts can exceed `total_frames` (the residual guard only adds,
  `SUB:9477-9480`), against the readme's "they add up to the frame count".
- **N5 [V]** After-offset settle entries take geometry frozen at `BeginRevert` (`SUB:5446-5452`, mid capture tick), not at the
  tick-end sample. They are transition-only, so no label box is affected.
- **N6 [V]** No reader checks the label content of settle frames: the kit excuses them in both directions (`KIT:1077`, `:1103`),
  the continuity check reads only the flags. P6, P10 and P12 (labelled-and-flagged counts, masks on onset-settle frames) need a
  scripted count in 095-07.
- **N7 [V]** The label rule excuses every stray settle row once `zero_match_bursts > 0` (`VC:3470-3473`). Narrow to 2K rows per
  zero-match burst at schedule positions.
- **N8 [V]** a54: claimed frames now include the labelled onset-settle frames, which can carry TSR residue. The flank rule rarely
  triggers for fire-window types. The file already says to re-gate margins on m56 legs.
- **N9 [V]** `m56_continuity_check.py` C8 checks that frames exist, not that they are non-empty (`run_leg.ps1` checks zero-byte
  files separately). Under m56 `run_summary.json` is written after the writer flush, so the zero-byte race is mostly gone.
- **N10 [V]** `annotation.json` carries only a count of row-only settle frames (zero-match bursts), not their indices. A delivery
  client with `labels.jsonl` off cannot drop them; they are clean frames, so this is harmless.
- **N11 [R]** The P9 blink hidden sets in tick space (native +{2,3,4,8,9}, synth +{3,4,5,9}) were not re-derived here.

### m58
- **M1 [V]** `IAI.SetTerrainPatterns clear` returns to the cooked ini list, not to "no patterns" (`AnomalyDefaults.cpp:485`).
  C-G3's can-fail ("clear reads `disappear_reappear`") holds on the bench (no ini key) but not on a host with a cooked pattern.
- **M2 [V]** No guard against an over-broad pattern (for example `SM_` makes every mesh blink terrain). The banner echoes the
  list; consider a warning when `blink_terrain_events` equals all blink events.
- **M3 [V]** The sync path never freezes event anchors (only `FreezeSampleGeometry` in the async tick-end sample does), so a
  sync-only run writes `disappear_reappear` for terrain. The sync path is unsupported for delivery; the anchor gap pre-exists.
- **M4 [V]** The readme puts the new subtype row in the **v1 → v2** changelog table. After the merge it belongs with 2.2.
- **M5 [V]** The refusal reaches the auto pool only on a Nanite landscape with `IAI.Targets.AllowNanite 1` (its Nanite component
  is a static mesh component). The draw then becomes a zero-match burst. C-G5 holds at defaults.
- **M6 [V]** "IsA LandscapeProxy" is a class-name match over the class chain (`AnomalyTerrainRule.h` `ChainHas`); the
  `FindObject` lookup (`AnomalyTerrain.cpp:74`) only decides ACTIVE or INERT. Equivalent for engine classes.
- **M7 [R]** `FindObject<UClass>(nullptr, "/Script/Landscape.LandscapeProxy")` is the 5.1 idiom; only C-G2 will prove it.

### CaptureBench
- **CB-N1 [V]** `m56_nonsettle_compare.py` compares `labels.jsonl` only, not `annotation.json` frame lists or mask pixels, so it
  would not see S1. State that scope in the G5 reading.

---

## What I checked and found correct (against the 15 points)

1. **Off-by-one in frames and ticks — correct. [V]**
   - `tick_index` = `CaptureGameTicks − TicksAtFirstArm`, stamped after `StampArmWallClock` on the async path (`SUB:5509-5512`),
     so the first row is 0. The sync path sets 0 before its first arm and the same formula after (`SUB:5678`).
   - One arm per capture tick in every phase (`SUB:1151-1243`); settle frames are armed before the decrement, so the phase
     lengths and the `BeginFire` / `BeginRevert` ticks are unchanged.
   - Span = last arm tick − first arm tick + 1; ticks before the first arm (warm-up, focus wait, `pre = 0`) and after the last
     (drain tail) are outside it.
   - `frames_dropped` walks every span tick: written ticks are armed sessions with no drop cause; the rest get the session's
     cause, else the unarmed-tick cause, else `unknown` (`SUB:9918-9988`). Every `PendingSnapshots` removal records a cause
     (`SUB:5062`, `:5157`, `:5554`, `:5583`).
   - K = 2 and K = 1 give K flagged frames per edge. **K = 0:** `BeginFire` goes straight to `Positives`, `BeginRevert` to
     `PostGap`; nothing flagged, nothing skipped.
   - `pre = 0`: outside the span, no false drop. `positive = 0` and `post = 0` skip one tick per cycle, counted `unknown` (honest,
     and the plan says the gate flags them).
2. **What a settle frame says — correct, with S1 and S3. [V]**
   - After onset: the normal entry keeps its label and gains `settle` (`StepSettleFlags`, `SUB:8248-8278`); masks arm as for any
     labelled frame.
   - After offset: transition-only entries built from the fires copied at `BeginRevert` with geometry frozen there
     (`SUB:5444-5453`, `AddSettleRevertedFires` `SUB:8224-8246`), never from the live actor at readback.
   - Copy lifetime: cleared by `BeginFire` (`SUB:5397-5401`) and by the per-run reset; a fire still live is skipped; a zero-match
     burst copies nothing; a `RestoreTrail` replaces the after-offset settle phase, so the copy is never used; a deferred-onset
     timeout goes through `BeginRevert` normally; a run ending inside a settle phase keeps what was captured.
   - Held global: S3. Vetoed event: S1.
   - Every settle frame carries the row-level flag, also with no entry (`AnomalyLabelWriter.cpp:157-171`).
3. **Non-settle frames — unchanged at release defaults, apart from `exposure_dip` (S6a) and the override windows (S5). [V]**
   - The labelled test never reads reason bits (`AnomalyLabelWriter.cpp:878-890`, `:290-296`); `anomaly_present` follows the
     emit mode only.
   - `FinalizeArmedLabel`, the tick-end sample, membership, `FillAnnotationInputs` and the transition steps are unchanged; the
     new code acts only on settle roles.
   - The m26 veto arms run every tick-end whether or not a frame is captured.
4. **Both tick orders — no new mid-tick label read. [V]** The role comes from the phase, set inside the capture tick. The only
   new mid-tick reads are the `BeginRevert` copy (N5, transition-only) and the sync path (unsupported).
5. **Carried `6713f51` — correct. [V]** The hold test sits after ORDER HOLD (`SUB:4755-4803`), so an order-held frame does not
   spend its mask budget. The budget is per snapshot; an ineffective mask commits at once as unmeasured, and the residual
   (`SUB:9477`) counts those frames unavailable. The mask pass serves every pending arm per render (m43), so 16 frames per
   cycle add readbacks, not contention.
6. **Writer threads and shutdown — correct apart from S4 and N2. [V]**
   - `PendingBytes`, `InFlightSessions` and `FailedSessions` are under one lock; counters are atomic; `Pending` drops after the
     job's bookkeeping, so a completed flush means final counts.
   - Counts are read after the flush (`SUB:5160-5185`), and the summary is written after that.
   - No writer job waits on the game thread (m55 `Colour` / `CompleteMask` take a lock and return), so back-pressure cannot
     deadlock.
7. **Fps stamp and `ticks_per_captured_frame` — correct. [V]** Continuous run: `bLossy` is false, so the stamp is the old value
   (target, or sustained wall fps) and the ratio is exactly 1.0. Lossy run: target × (N−1)/(S−1), or (N−1)/wall span
   (`SUB:8952-8995`). `speed_ratio` is unchanged (tick-based). N1 is the one way to read a false lossy run.
8. **Schema honesty — correct. [V]** `tick_index` and `t_wall_run` on async and sync rows; row-level settle on both; every
   continuity key written on every run (async state always exists, `SUB:719`); `label_schema_minor` 2 in the one place it is
   written; delivery mode still writes `labels.jsonl` (default on), `run_summary.json` and `annotation.json`.
9. **Readers — `settle` is excused only on flagged frames, and edges stay guarded; the veto tolerance is S2. [V]**
   - The onset frame and the revert frame stay unflagged, so a label one frame early or late at either edge still fails the
     kit; selftests prove it (`settle_label_late_1`, `_early_1`).
   - Placement is checked structurally (`VC` SETTLE-MISPLACED, continuity C7). Holes: S2, S3, N6, N7.
   - Selftests re-run on copies: label rule 83 OK, annotation tests 8 OK, exclusion gate OK, continuity 9 OK, comparator 6 OK.
10. **m58, terrain decided while alive, transition-only path — correct. [V]** Decided once in `FreezeEventAnchor` at the first
    tick-end sample (key format equals the event id). Both annotation call sites look up the anchor (`m58 SUB:10291`).
11. **m58 class lookup and refusal scope — correct. [V]** Lookup failure reads INERT in the banner and refuses nothing. The
    refusal is only in `Anomaly_Blinking::Apply` (`Anomaly_Blinking.cpp:72`). Selection, census and viewport code are untouched;
    see M5 for the one auto-pool edge.
12. **m58 patterns — correct for empty patterns, case and fields; no over-broad guard (M2). [V]** Empty patterns are skipped,
    matching folds ASCII case, fields are actor name, actor own class, and component name / own class / mesh asset. The
    excluded-target ini reader refactor keeps its behaviour.
13. **Merge** — see below.
14. **Shipping and bench gating — correct. [V]** m56 levers: capture module (denied in Shipping), runtime-checked with
    `AnomalyBenchGate::IsEnabled()`. m58 levers: `#if !UE_BUILD_SHIPPING`, same pattern as existing commands such as
    `IAI.Bench.SynthTickOrder`. All four fall under the `IAI.Bench.` prefix mask (`AnomalyBenchGate.cpp:42-84`).
    `IAI.SetTerrainPatterns` is a product setting, ungated like `IAI.SetExcludedTargets`.
15. **Bench scripts — no pass on a broken session found; B1 and S6 must be fixed. [V]**
    - The continuity check cross-checks `tick_index` against `frame_index` (C4), so a wrong tick stamp cannot hide a gap; C6
      fails any drop; C7 anchors on the data and the schedule.
    - The comparator fails a missing partner, a release row on a settle frame, an unpartnered non-settle m56 row and any field
      change not measured as run-unique; A64 pose precondition first.

---

## Merge result (`git merge-tree --write-tree`, no refs changed)

- **Exit 1: conflicts in 5 files.** Output: `code-receipts-095-06\merge-tree-m56-m58.txt` (tree `5210e4c`).
  - `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`, `AnomalyLabelWriter.cpp`, `AnomalyLabelWriter.h`: **one
    conflict, three copies**. Both branches append parameters to `WriteRunSummary` (m56 `const FContinuityTelemetry*`, m58
    `BlinkTerrainEvents, TerrainUnlabelableRefusals`). Mechanical: keep all three, in the same order at the declaration,
    definition and call.
  - `CLAUDE.md` (status blocks) and `docs/gotchas.md` (both append): mechanical. Gotcha numbers do not collide
    (m58 G574, m56 G575–G578).
- **Auto-merged and read:** the run banner (m58's TERRAIN RULES line after m56's widened STARTED line), `StartRun` resets,
  `WriteSessionAnnotationFile` (m58's lambda before m56's drop-removal block), `client-readme.md`, `architecture.md`.
- **Semantic clashes that merge cleanly:**
  - S1 × m58: vetoed terrain blink shells get the terrain subtype and count in `blink_terrain_events`.
  - m58's refusal makes a zero-match burst; m56 flags its settle frames row-only and the reader allows them (consistent).
  - Readme: m58's row sits in the v1 → v2 table while m56 adds a 2.1 → 2.2 table (M4).
  - `run_summary` keys: no name collision.

---

## Questions for chat

1. **Vetoed events (S1, S2):** restore m26's "no trace" (skip vetoed ids, write `vetoed_event_ids`, reader excuses exactly those),
   or keep and document the shells? **Recommend restore.**
2. **Held global (S3):** capture the settle-phase ticks as normal frames in held-global mode, or add a reader exception?
   **Recommend the C++ change** — nothing transitions there.
3. **Override windows (S5):** skip settle frames in the window count, or document "add K"? **Recommend the doc fix** (defaults are
   0/0/0 and the readme names no value); switch to the code fix if a client uses the overrides.
4. **095-07 build plan:** gate each unit on its own exe from the one cook (G103 code-only swap), not on a merged exe. m58's C-G4 and
   C-G5 compare against the release, and m56 changes bursts per frame cap. **Recommend separate exes.**
5. **Merge order after green gates:** **recommend m58 first** (smaller), then m56 with the mechanical `WriteRunSummary` resolution.
6. **a54 flank (095-05b's open question):** keep the skip limited to `settle`. **Recommend keep** (old readings stay byte-identical).
7. **S4 and B1 before 095-07:** both are small; **recommend fixing them in a short brief before the cook**, with S1–S3 and S6.

---

## Receipts (`D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-06\`)

- `m56-source.diff`, `m56-tools.diff`, `m56-readme.diff`, `m58-source.diff`: the reviewed diffs.
- `src56\`, `src58\`, `srcmaster\`, `cb\`: source copies read for context.
- `merge-tree-m56-m58.txt`, `merged_*.cpp/.h`: the merge-tree result and conflict hunks.
- `run56\probe_095_06.py`, `probe_095_06.out.txt`: the S2 and S3 reader probes (copies of the branch tools; temp dirs only).
- Selftests run on copies with bytecode writing off: label rule 83 OK, annotation tests 8 OK, exclusion gate OK,
  continuity 9 OK, comparator 6 OK.
