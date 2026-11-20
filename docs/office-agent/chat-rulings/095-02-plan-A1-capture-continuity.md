# 095-02 — Plan A-1: capture continuity (m56, `fix/m56-capture-continuity`)

**Author:** Claude Code (Opus 5.5, xhigh), headless, 2026-11-13. **Status:** PLAN ONLY. Nothing built, run or changed.
**Base:** `master` = `ebc34ed` (= release `d7eee97` + one docs commit). Every `file:line` below is on `master`.
**Binding inputs:** `_reviews\095-00-ruling.md` §2 (unit A) and §6; `_reviews\095-01-ruling.md` (no build yet);
`_reviews\095-00-code-capture-review-2026-11-11.md` (its line numbers were on `fix/followup-capture`; all re-derived here).

**Marks.** **[V]** = I re-checked it against `master` source, a banked session, or UE 5.1.1 source.
**[R]** = reported (by a sub-reviewer or an older record) and not re-checked by me.

**Short names.** `SUB` = `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`.
`SUBH` = `Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h`. `LW` = `Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp`.
`SYNC` = `Source/AnomalyInjector/Public/AnomalyLabelSync.h`. `AW` = `Source/AnomalyCapture/Private/AnomalyAsyncWriter.cpp`.

---

## 0. Summary

- **The defect, in one line.** Two engine ticks after every anomaly onset and two after every offset are rendered but never
  captured. The frame files are numbered without gaps, so the hole is invisible in the file names. It sits exactly on the
  label edges. [V]
- **The change.** The burst state machine keeps deciding labels but no longer decides whether a frame exists. The two
  settle phases capture a frame on every tick. Those frames carry the schema-2.1 transition flag with a new reason
  `settle`. Nothing else about any existing frame's label or mask changes.
- **Continuity becomes checkable from the data alone.** Each row gains `tick_index` and `t_wall_run`.
  `run_summary` gains one honest drop count (`frames_dropped`), the missing tick indices and per-cause counts.
- **The fps stamp follows captured frames.** On a healthy run `ticks_per_captured_frame` reads exactly 1.0.
- **Also in A-1:** the minimum writer change so no frame is lost silently at run end, and the m51 mask-wait fix
  `6713f51` as its own commit.
- **Not in A-1:** throughput work (A-2), the 4.25 forward-port, security items (A7).
- **Sub-reviewers used** (read-only): reader inventory, terrain path, settle history + drop paths + `6713f51`. Every claim of
  theirs that a plan step depends on was re-checked and is marked [V]; the rest is marked [R].

---

## 1. How it works today on `master`

### 1.1 The burst state machine [V]

- Phases: `SUBH:231-233` (`SettleAfterFire`, `SettleAfterRevert` among them). Defaults `SUBH:468-472`:
  settle K = 2, pre = 4, positive = 8, post = 4, bursts = 0 (until stop).
- Set only from the console: `IAI.Capture.Config <settleK> <pre> <positive> <post> <bursts>`
  (`SUB:2020-2033`, registered `SUB:10435-10442`). No ini key. The dashboard's `capture_start` does not send it. [R for the
  dashboard half; the plugin half is V]
- One capture `Tick` (`SUB:1018-1233`), in order:
  1. `++CaptureGameTicks` once the run has begun (`SUB:1038-1040`).
  2. Focus wait while `ArmedPending` (`SUB:1051-1071`).
  3. `PaceThisTick` (`SUB:1075`, body `SUB:8589-8620`).
  4. `ProcessCompletedFrames` drains finished readbacks (`SUB:1081`, body `SUB:4599-5000`).
  5. Frame-cap check (`SUB:1089-1102`).
  6. The phase switch (`SUB:1104-1229`).
  7. `FinalizeArmedLabel` (`SUB:1231`, body `SUB:5645-5708`).
  Later in the same world tick, `OnWorldTickEnd` runs the mask service and the label sample
  (`SUB:1302-1328`, `SampleDeferredActiveState` `SUB:6037-6189`).
- **Every tick inside a run that passes without a capture:**

  | Phase | Where | Captures? | Inside the frame span? |
  |---|---|---|---|
  | `TexCorruptWarm` | `SUB:1106-1131` | no | no (before frame 0) |
  | `LeadIn` | `SUB:1133-1136` | yes; on its last tick it also calls `BeginFire` after the capture | yes |
  | **`SettleAfterFire`** | **`SUB:1138-1141`** | **no — decrements only** | **yes** |
  | `Positives` | `SUB:1143-1176` | yes (also during the deferred-onset pre-roll); last tick calls `BeginRevert` after the capture | yes |
  | **`SettleAfterRevert`** | **`SUB:1178-1181`** | **no — decrements only** | **yes** |
  | `PostGap` | `SUB:1183-1205` | yes; its last tick calls `BeginFire` after the capture | yes |
  | `DrainTail` | `SUB:1207-1216` | no | no (after the last frame) |
  | `RestoreTrail` | `SUB:1218-1225` | yes (stuck_low_mip restore; replaces `SettleAfterRevert` when a trail opens, `SUB:5334-5344`) | yes |

- Entry into settle: `BeginFire` `SUB:5328-5329` (and `SUB:5299-5300` for a held global), `BeginRevert` `SUB:5346-5347`.
  K = 0 already skips the phases (G457 fix, present on `master`).
- Edge configs that also skip a tick: pre = 0 (LeadIn fires on its first tick with no capture), positive = 0,
  post = 0. Not used by any default; A-1 leaves them as they are and the continuity gate would flag them.
- **Steady state: 16 engine ticks per 12 written frames.** Onset frames sit at session index ≡ 3 (mod 12).
- **Banked evidence** (a 90-frame m47-era leg on `CB_GateLevel`,
  `D:\IntrusiveAnomalies\_m47_legs\AE1_CT_AEON\session_20260903-182615`, read today): 90 rows; `frame_index` runs 1..120;
  consecutive deltas are 1 on 74 pairs and **3 on 15 pairs**, at session 3→4, 11→12, 15→16, 23→24 and so on; `t_wall`
  steps 93–100 ms at those pairs against 28–38 ms elsewhere; `anomaly_present` on 3–10, 15–22, …;
  `run_summary.ticks_per_captured_frame` = 1.344 (121 / 90). [V]
- **So a delivered `labels.jsonl` already shows the gap** through `frame_index` and `t_wall`, even though the file names
  hide it. Since 052 `labels.jsonl` is written in delivery mode by default.

### 1.2 How a frame gets its index, and where the fps stamp comes from [V]

- `CaptureCurrentFrame` (`SUB:5350-5586`) allocates the frame:
  `session_index` = `SessionFrameIndex` (`SUB:5371`, incremented `SUB:5453` async / `SUB:5578` sync).
  It names the PNG and the label row.
  `frame_index` = `GFrameCounter` at the arm (`SUB:5370`). `t` = world seconds (`SUB:5372`).
  `t_wall` = `FPlatformTime::Seconds()` (`SUB:5380`) — **an absolute clock, not "since the run started"**.
  The client readme says "since the run started" (`docs/client-readme.md:883`). The banked rows read ~16.8 M s. Doc defect.
- `StampArmWallClock` (`SUB:8622-8634`) records the first and last arm wall time and the tick counts at those arms.
- `speed_ratio` = wall span / ((`TicksAtLastArm` − `TicksAtFirstArm`) / fps) (`SUB:8636-8697`). It is tick-based.
- Stamped fps (`SUB:8703-8728`): game clock → the target fps; wall clock → `fps / speed_ratio` when the ratio exceeds 1.02,
  else the target. `annotation.video.fps` = the stamp (`SUB:10052`); `video.total_frames` = frames written (`SUB:10050`).
- `run_summary.ticks_per_captured_frame` = `capture_game_ticks / total_frames` (`LW:1302-1304`). `capture_game_ticks` counts
  **every** tick since the run began (`SUB:4198`, `SUB:1040`), including warm-up and drain ticks.
- **Net effect:** a healthy paced run writes 12 frames per 16 ticks and stamps 30 fps. The assembled video is about 25 %
  shorter than real time.

### 1.3 How a frame's label row is decided [V]

1. Arm (`CaptureCurrentFrame`): a snapshot keyed by request id (`SUB:5437`).
2. End of the capture `Tick` (`FinalizeArmedLabel`, `SUB:5645-5708`): the live fires, read **after** any `BeginFire` /
   `BeginRevert` in the same tick (m18).
3. `OnWorldTickEnd` sample (`SampleDeferredActiveState`, `SUB:6037-6189`): active and labelled bits, frozen geometry,
   install state, Nanite gate, proxy-blur and PIE-end-settle transitions, telemetry, mask values (m40).
4. Drain (`ProcessCompletedFrames`, `SUB:4599-5000`): mask outcome (`SUB:4734-4792`), observability (`SUB:4800-4866`),
   exposure dip, `FillAnnotationInputs` (`SUB:4950`, `8553-8567`), hide and fire-window transitions (`SUB:4951-4952`),
   the row (`LW:716-728` → `LW:115-384`), transition and event accumulation (`SUB:4967-4971`, `9890-9999`).
- Membership rule per policy: `SYNC:53-80` (fire window = on screen and installed; hidden types = logically hidden;
  anomaly-state types = currently anomalous). `injected_frames` = members; `affected_frames` = the observable subset
  (`SUB:10089-10153`).
- **What a settle frame's row would contain if today's code simply captured it:**
  - **After the onset:** the fire is live and installed. The entry is a normal labelled entry, listed in `annotation.json`,
    and its target mask is armed (`SUB:1358-1370`). Release defaults have no temporal transition windows (0/0/0,
    `SYNC:13-15`). **So it would silently become an extra positive.** For `blinking` it is labelled only if the
    target is hidden on that tick.
  - **After the offset:** `BeginRevert` has already called `RevertAllLiveFires` (`SUB:5338`), so the fire is gone from the
    live list. With release defaults no transition track emits an entry for it, so **it would be a plain negative**.
    The PIE-only `pie_end_settle` flag lands on the first frame after the label run, which is the revert-tick frame that
    already exists today, so it does not move.
  - `stuck_low_mip`: render-truth membership decides each frame; when a restore trail opens, `RestoreTrail` replaces the
    after-offset settle and already captures every tick.

### 1.4 The schema-2.1 transition machinery, and where `settle` fits [V]

- Reason bits `SYNC:17-26` (9 reasons, bits 0–8, stored in `uint16`); JSON names `SYNC:28-43` (`temporal_aa`,
  `hide_return`, `partial`, `camera_clipping_unconfirmed`, `unresolved`, `effect_interrupted`, `nanite_unmaskable`,
  `capture_unpaired`, `pie_end_settle`); legacy coercion `SYNC:45-51`.
- Per entry: an emit mode (normal, suppress, transition-only) and a reason bitmask. Normal + bits = "labelled but
  uncertain"; transition-only = an unlabelled entry that sets no `anomaly_present`.
- Fires that are no longer live get transition-only entries through `AddTransitionFire` (`SUB:7856-7879`).
- Row level: `transition_present` (`LW:152-155`); a row-level `transition_reason` exists today only for
  `capture_unpaired` (`LW:156-160`). Entry level: `transition: 1` and `transition_reason` (`LW:273-278`).
- Counts: `CountEntryReasons` (`SUB:7742-7775`) → `run_summary.label_transition_<reason>_entries` (`LW:1347-1355`).
- `annotation.json`: per event `transition_frames` and `transition_reasons`, root `transition_frame_count`
  (`LW:740-764`, `LW:1463-1468`, `LW:1486-1496`); `label_schema` 2, `label_schema_minor` 1 (`LW:1460-1461`).
- **`settle` fits as bit 9 (512).** `NumReasons` 9 → 10; `DescribeReasonBit(9)`; add it to the `ReasonsOrLegacy` mask;
  a new `run_summary.label_transition_settle_entries`. `uint16` has room.
- **Name risk:** the existing `pie_end_settle` contains the substring `settle`. Any reader that matches by substring would
  confuse them. See §3 and Questions for chat.

### 1.5 The two tick orders [V]

- **Native order:** the injector's anomaly dispatch runs in its own `Tick` (`Source/AnomalyInjector/Private/
  AnomalyInjectorSubsystem.cpp:482-494`, `DispatchAnomalyTicks` `:520-536`). On this box it runs **after** the capture
  `Tick` [R, journal 068 §8 measurement].
- **Synthesised order** (bench lever `IAI.Bench.SynthTickOrder`): dispatch moves to `OnWorldPreActorTick`
  (`:538-545`, `:737-765`), i.e. **before** the capture `Tick`.
- **Code that depends on the order:**
  - the anomalies' own per-tick work (blink toggles, `lod_popping` swaps, `TickAlways` restore polling) lands before or
    after the capture `Tick`'s `BeginFire` / `BeginRevert` and arm;
  - `BurstAwaitsDeferredOnset` (`SUB:5109`) reads anomaly state in the middle of the capture `Tick`;
  - the sync fallback grab samples during the `Tick` (it is marked `capture_unpaired` and unsupported).
- **Order-independent by design:** every label bit is sampled at `OnWorldTickEnd` (m40), and the picture is that same
  tick's render (m18, m21).
- **What A-1 changes here:** nothing in the ordering. It adds frames on the ticks where an order difference can show,
  for example a blink toggle that lands on the first settle tick. The anomalies' clocks already count ticks, so after
  A-1 the frame pattern equals the tick pattern.

---

## 2. The change, file by file

The smallest change that meets the standard: **capture on the settle ticks, flag those frames, and account for every tick.**

### 2.1 `SUBH` (header)
- A per-snapshot frame role: normal, after-onset settle, after-offset settle (a `uint8` on the snapshot is enough).
- Members: the reverted fires of the last `BeginRevert` with their frozen geometry; a session → tick map of armed frames;
  a session → drop-cause map; counters for settle frames armed and written.
- `SettleFrames` stays, default 2.

### 2.2 `SUB` — the state machine (about 6 changed lines)
- `SettleAfterFire` (`SUB:1138-1141`) and `SettleAfterRevert` (`SUB:1178-1181`): call `CaptureCurrentFrame` with the
  settle role before the decrement. The phase lengths, the transition ticks and `BeginFire` / `BeginRevert` are unchanged.
- Result per cycle at defaults: post 4 + settle 2 + positive 8 + settle 2 = **16 ticks, 16 frames**.
- `LeadIn`, `Positives`, `PostGap`, `RestoreTrail`, the deferred-onset pre-roll and m18's "the frame armed on a transition
  tick is labelled after the transition" all stay exactly as they are.

### 2.3 `SUB` — arm, sample and drain
- `CaptureCurrentFrame` (both paths): write the role, `tick_index` (= `CaptureGameTicks − TicksAtFirstArm`, after
  `StampArmWallClock`) and `t_wall_run` (= arm wall time − first arm wall time) into the snapshot; record the armed tick.
- `BeginRevert`: before `RevertAllLiveFires`, copy the live fires and freeze their geometry (game thread, actors alive).
  `BeginFire` clears the copy.
- At the tick-end sample, for an after-offset settle frame: add each copied fire that is no longer live as a
  transition-only entry with reason `settle`, using the frozen geometry. This keeps R4's rule that a label's box comes from
  the tick-end sample, never from the live actor at readback.
- In the drain (after `StepFireWindowTransitions`, before the row is built) and in the sync path (`SUB:5512-5518`):
  a new `StepSettleFlags` ORs `settle` into every emitted entry of a settle frame. Normal entries stay labelled (the same
  treatment the schema already gives temporal onset frames). Transition-only entries stay transition-only.
  The row also gets row-level `transition_present: true` and a row-level `transition_reason` containing `settle`, so a
  settle frame with no entry (a zero-match burst, a suppressed render-truth entry) is still dropped by the client's rule.
- `AccumulateTransitions` and the session union: settle frames count in each event's `transition_frames` where the event
  has an entry, and always in the root `transition_frame_count`.
- **Masks:** no new code. Labelled after-onset settle frames already arm the target mask through
  `EnsureMaskRecordsForCapturedFrame` / `ArmTargetMaskOwn` (`SUB:1358-1370`).
- **Untouched:** the m26 veto arms (they run every tick-end whether or not a frame is captured, `SUB:1355`), the census
  tick (`SUB:1380-1387`), pacing, the temporal defaults 0/0/0, every membership rule, and the label and mask meaning of
  every non-settle frame.

### 2.4 The console settle setting and its echo
- `IAI.Capture.Config` keeps K. Its meaning changes from "ticks after each edge that are not captured" to "captured frames
  after each edge that carry `transition_reason: settle`". K = 0 means no flagged frames; nothing is ever skipped.
- The echo strings change wording only: `SUB:2032-2033`, the run banner `SUB:4327-4334`, the idle status
  `SUB:4417-4418`, the help text `SUB:10436-10442`. `run.json` keeps `settle_frames` (`LW:1056`) with the new meaning,
  documented.
- The default stays 2 (ruling (a): the flag is the safe direction). A later ruling that drops the flag is a one-line
  default change to 0, decided by the measurement in §8.

### 2.5 Fps stamp and the tick ratio (ruling A3)
- Let N = frames written and S = span ticks = `TicksAtLastArm − TicksAtFirstArm + 1`. Healthy: N = S.
- Stamp: game clock → target × (N − 1) / (S − 1); wall clock → (N − 1) / wall span when the pacing ratio exceeds the
  tolerance, else the game-clock formula. **On a continuous run both equal today's formulas exactly**; they differ only when
  a frame is lost, and then the video length still matches the time it covers.
- `speed_ratio` stays tick-based (it measures pacing health, not frame count). `game_clock_speed_ratio` unchanged.
- `ticks_per_captured_frame` is redefined as S / N (exactly 1.0 when continuous). `capture_game_ticks` keeps its meaning;
  a new `capture_span_ticks` = S. (Question for chat 2.)

### 2.6 `SYNC` (`AnomalyLabelSync.h`)
- `ReasonSettle = 512`, `NumReasons = 10`, the name, the legacy mask. This header is included by the pure C++ selftests
  under `tools/`; they must still compile, and one case is added.

### 2.7 `LW` (`AnomalyLabelWriter.cpp/.h`)
- Row: `tick_index`, `t_wall_run`, the row-level settle flag.
- `run_summary`: the accounting keys of §3, `label_transition_settle_entries`, `capture_span_ticks`,
  the redefined `ticks_per_captured_frame`.
- `annotation.json`: `label_schema_minor` 2.
- Check the result of the `labels.jsonl` append (`LW:419-423` sync path and the writer's path; today the result is
  ignored).

### 2.8 Writer and capturers
- See §5 (writer) and §4 (drop causes reported back from the capturers).

### 2.9 Carry `6713f51`
- Its own commit. See §6.

### 2.10 Docs and tools
- `docs/client-readme.md` (row and summary tables, the reason table §8.6a, a 2.1 → 2.2 changelog row, the `t_wall` fix),
  `docs/architecture.md`, `docs/capture-fps.md`, `docs/PRE-DELIVERY-CHECKLIST.md`, the status block, a journal, gotchas.
- The readers in §3 that need an update.

---

## 3. New output keys, exactly, and every reader

### 3.1 Keys

| File | Key | Type, unit | Meaning |
|---|---|---|---|
| `labels.jsonl` row | `tick_index` | int | Engine ticks since the run's first captured frame (0 on the first row). Consecutive rows (sorted by `session_index`) differ by exactly 1 on a healthy run. |
| `labels.jsonl` row | `t_wall_run` | number, seconds | Wall time since the first captured frame's arm. |
| `labels.jsonl` row | `transition_present` | `true` | Existing key; now also on every settle frame. |
| `labels.jsonl` row | `transition_reason` (row level) | array of string | Existing key (today only `capture_unpaired`); now also `settle` on every settle frame. |
| `labels.jsonl` entry | `transition`, `transition_reason` | existing | Gains the value `settle`. |
| `annotation.json` root | `label_schema_minor` | int | 1 → 2. |
| `annotation.json` event | `transition_frames`, `transition_reasons` | existing | Include settle frames and a `settle` count. |
| `annotation.json` root | `transition_frame_count` | existing | Includes every settle frame. |
| `run_summary.json` | `frames_dropped` | int | Span ticks with no written frame (S − N). 0 on a healthy run. |
| `run_summary.json` | `frames_dropped_ticks` | array of int | The `tick_index` of every missing frame, ascending. Capped at 4096 entries. |
| `run_summary.json` | `frames_dropped_ticks_truncated` | bool | True if the cap was hit. |
| `run_summary.json` | `frames_dropped_by_cause` | object, cause → int | Sums to `frames_dropped`. Causes in §4. |
| `run_summary.json` | `frames_armed` | int | Frames allocated a `session_index`. |
| `run_summary.json` | `capture_span_ticks` | int | S. |
| `run_summary.json` | `ticks_per_captured_frame` | number | **Redefined** as S / N. |
| `run_summary.json` | `settle_frames` | int | Frames written in a settle phase. |
| `run_summary.json` | `label_transition_settle_entries` | int | Entries carrying `settle` (matches the existing per-reason family). |
| `run_summary.json` | `label_rows_failed` | int | Frames whose image was written but whose `labels.jsonl` row append failed. |

- No key is removed or renamed. `stamped_fps` and `video.fps` keep their names; only the formula changes on lossy runs.

### 3.2 Readers

Effect codes: **ok** = ignores the change; **update** = needs a change; **BREAK** = fails loudly; **SILENT** = would misjudge
a session without saying so. "Dashboard" = `D:\IntrusiveAnomalies\anomaly-dashboard`, ref `master` `7ea2e51` (the 5.1
delivery; `feat/desktop-v2` is its ancestor; `feat/europa-425` = master + one catalog commit) [R: ancestry and dates only].

| Reader | What it reads | Effect | What it needs |
|---|---|---|---|
| `tools/verify_capture.py` annotation check (`:3250-3305`) | schema, per-event transitions | **BREAK**: `:3260` rejects minor ≠ 1 [V] | accept minor 2 |
| `tools/verify_capture.py` label-rule check (`:3155-3192`, `:3309-3660`) | reason list [V], per-type reason table, PIE rules | **BREAK**: `settle` is an unknown reason (`:3455`); adding it to the list without the per-type table crashes at `:3461` [R] | add `settle` to the list, the per-type table and the labelled/unlabelled lists; keep it out of the temporal-only list |
| `tools/verify_capture.py` overlay (`:328-342`, `:4343-4398`) | `session_index`, event frame lists | ok; offset-settle entries draw AMBER "outside subset"; the readme's amber table goes stale [R] | readme numbers |
| `tools/verify_capture.py --label-pixel-gate` (`:1148+`) | event runs, masks, pixels | ok; wider clean gaps help. If onset-settle frames were left out of `frame_indices` the edge would read an offset note [R]. They stay in (labelled), so no change. | none |
| `tools/annotation_transition_check.py`, `tools/test_annotation_transitions.py` | schema 2.1; fixtures pinned to 9 reasons, minor 1 | **BREAK** (exit 1); tests still pass and prove nothing about the new data [R] | minor 2, a `settle` fixture |
| `tools/label_sync_check.py` (office kit, ships to the client) | `KNOWN_REASONS` `:62-63` [V], refusal `:2376-2381` [V], transition gate `:1041-1113`, engine-frame-jump logic `:620-641` | **BREAK**: refuses every new session (via the annotation check). **SILENT** once that is fixed: it excuses frames only for `temporal_aa` / `hide_return` (`:1057-1078`), so a changing `settle` frame becomes a false FAIL; its `frame_index` jump censoring stops firing [R] | learn `settle` as an excusing reason in both directions; minor 2 |
| `tools/measure_label_offset.py` (`:1075-1093`) | baseline from frames outside every window + 2-frame guard | **SILENT**: when its guard relaxes on dense schedules, settle frames enter the baseline and inflate the threshold, hiding offsets [R] | exclude `transition_frames` from the baseline |
| `tools/exclusion_gate.py:16` | an m52 entry overlaps if labelled or any `transition` | update: m52 `settle` entries become overlap frames; the C++ overlap counter must use the same rule [R] | align the rule |
| `tools/r4_pairing_gate.py` | C++ source anchors in the transition functions | update: re-run after the change [R] | re-run |
| `tools/m52_readout.py` | `total_frames`, `t_wall` deltas | ok; the max frame-time reading drops [R] | none |
| `tools/OFFICE-CHECK.md` (`:11-13`, `:72-74`) | config, sample counts, "schema 2.1" | update: wording and sample counts [R] | docs |
| `WebClient/spike-client.html:162` | live snapshot fps | ok [R] | none |
| C++ `MapAnomalyToClient` | — | (unit C) | — |
| Dashboard `src-tauri/src/sessions.rs` `describe` (`:243-300`) | `video.total_frames`, `fps`, event `injected_frames` | ok; timeline counts change [R] | none |
| Dashboard `sessions.rs` `settled` (`:349-385`) | `run_summary.total_frames` must equal the PNG count | update **if** `total_frames` ever counted dropped ticks: the session would stall 90 s [R]. **This plan keeps `total_frames` = frames written.** | none, by design |
| Dashboard `video.rs` (`:1009-1024`) [V partially] | frames by directory listing | **SILENT**: a numbering gap is filled by repeating the previous frame (counted, not shown) — against the no-duplication rule [R] | report gaps instead of filling (Question 6) |
| Dashboard `host-tools/encode_watcher.py` (`:248-256`) [V] | `video.fps`; `ffmpeg -start_number 0 -i frame_%05d` | **SILENT**: ffmpeg's numbered input stops at the first missing number, so one dropped frame truncates the mp4 without a warning [R for ffmpeg behaviour] | check `frames_dropped` / numbering before encoding; warn loudly (Question 6) |
| Dashboard `overlay.rs`, `anomalies.ts`, `LibraryView.tsx`, `Timeline.tsx` | `session_index`, ids, boxes, masks; type keys | ok [R] | none |
| Dashboard `host-tools/bundle_manifest.txt` | copies the plugin's readme and tools at bundle time | update: every tool fix reaches the client only through a re-bundle [R] | re-bundle |
| `docs/client-readme.md` (`:137-146`, `:845`, `:883`, `:915`, `:957-958`, `:991-1004`), `docs/client-delivery.md:167-172`, `docs/PRE-DELIVERY-CHECKLIST.md:676` ("run_summary key count is 68" — a literal that will fail) | — | update [R; `:883` V] | docs |
| CaptureBench `run_leg.ps1:220` | hard-codes `IAI.Capture.Config 2 4 8 4 0`, 90 frames | update: per-leg event predictions change (~7 → ~5 events per 90 frames) [R] | new predictions |
| CaptureBench `a54_oracle.py` (`:299-323`) | flank frames min(C)−1, max(C)+1 | **SILENT** under temporal AA: max(C)+1 becomes an offset-settle frame with residue, biasing toward ABSENT [R] | skip flagged flank frames, or AA-off only |
| CaptureBench comparators (`subset_gate.py`, `m50_pc7v3.py`, …) | cross-binary field sets | update: old-vs-new is not comparable by `session_index`; declare the new keys [R] | the new comparator of §7 G5 |
| CaptureBench `m44_gates.py`, `m44_pairing_probe.py`, `p9_hidden_set.py` | onset, mask placement, blink sets | ok for logic; the P9 expected shapes are 12-frame artefacts and must be re-derived [R] | new predictions |
| Loose `D:\IntrusiveAnomalies\host-tools\*` | stale copies, in no bundle | ok [R] | none |

- **Name collision `settle` / `pie_end_settle`:** every reader matches reason names exactly; none uses substrings
  (`verify_capture.py:3157` [V], `label_sync_check.py:62-63` [V], `:496`, `:506` [R]). `settle` is safe as a name.

---

## 4. Every way an armed frame can fail to be published on `master`

Common facts [V]: the async arm increments `session_index` unconditionally (`SUB:5453`), so any later loss leaves a missing
file number. The sync path increments only on success (`SUB:5578`), so its failure is a tick with no frame and no number.
`run_summary.total_frames` and `annotation.video.total_frames` (`SUB:10050`) are frames written; there is no armed or
dropped count on `master`. `pending_at_stop` does not exist on `master` (it is a follow-up-branch key) [V].

The A-1 accounting: `frames_dropped` = span ticks − frames written. Every missing tick gets exactly one cause. Causes are
recorded where the loss happens; anything left is `unknown`, and the gate requires `unknown` = 0.

| # | Loss path on `master` | Where | Counter today | Reaches `run_summary` today? | A-1 cause | How the gap shows after A-1 |
|---|---|---|---|---|---|---|
| D0 | **Settle tick, not captured** (the main one) | `SUB:1138-1141`, `SUB:1178-1181` [V] | none | no | (removed by the change) | — |
| D1 | Snapshot never resolved by run end (8 flush rounds, +16 for render-truth runs) | `SUB:5018-5040`, `SUB:5069-5074` [V] | warning only | no | `unresolved_at_end` | missing tick in `frames_dropped_ticks`, missing file number |
| D2 | Mask hold when the mask is requested but cannot exist (C2 defect; output height ≠ 0, or `Mask 0` with `TargetMask 1`); frames pile up and end in D1 | `SUB:4734-4746`, effective flag `SUB:3750` [V] | `target_mask_frames_unavailable` only for frames that get through | partly | (removed by `6713f51`, §6) | — |
| D3 | `DrainTail` budget (≥ 10 ticks) runs out | `SUB:1095`, `SUB:1193`, `SUB:1207-1211` [V] | none | no | feeds `unresolved_at_end` | as D1 |
| D4 | Completed frame with no snapshot ("CAP-PAIR-DROP") | `SUB:4708-4716` [V] | warning | no | `pair_drop` (if its request id maps to an armed index; else counted separately as `orphan_completions`) | as D1 |
| D5 | Change-evidence epoch reset cancels other-generation arms (owner scene/target/level change) | `SUB:5412-5426` [V] | `pending_colour_cancelled` diagnostic | as a `change_*` key [R] | `cancelled_epoch` | as D1 |
| D6 | Change-evidence bench gate 2 drops an arm (only with `-IAIBenchFixture`) | `SUB:5440-5444` [V] | `capture_arm_dropped` | `change_*` [R] | `bench_gate` | as D1 |
| D7 | SVE: no key for the view family; empty rect; extent clamp; readback guard; readback lock fails; arm never meets a publish | `AnomalySceneViewExtension.cpp:176-185, 216-236`; `AnomalySveCapturer.cpp:501-507, 548-552` [R]; log lines [V] | `key_ring_missed` (in `run_summary`), others log-only | partly | `sve_no_key`, `sve_rect`, `sve_clamp`, `sve_guard`, `sve_lock` (reported back through the completed queue with the request id), else `unresolved_at_end` | as D1 |
| D8 | Backbuffer path: invalid backbuffer, window mismatch, empty rect, clamp, texture failure, guard, lock | `AnomalyFrameCapturer.cpp:242-253, 263-280, 313-320, 390-396, 420-423` [R]; log lines [V] | log-only counters | no | `bb_*` causes as D7 | as D1 |
| D9 | Writer encode or save fails | `AW:74-102` [V] | `Dropped`, end-of-run warning `SUB:5079-5083` [V] | no | `write_failed` | as D1; the frame is also **removed from `annotation.json` frame lists** (today it stays listed) |
| D10 | `labels.jsonl` row append fails (result ignored) | `LW:419-423` and the writer path [R] | none | no | not a dropped frame (the PNG exists) → `label_rows_failed` | the row is missing; counted |
| D11 | Writer flush times out (5 s at run end, 2 s at teardown) and the summary is finalised with jobs pending | `SUB:5076`, `SUB:1000`, `AW:140-153` [V] | warning | no | removed by §5 (complete flush); if the hard cap is ever hit: `writer_timeout` | as D1 |
| D12 | Sync grab fails: no index, no counter | `SUB:5530-5585` [V] | none | no | `not_armed` | a tick in the span with no row |
| D13 | Frame cap counts armed frames | `SUB:1089` [V] | — | — | (kept; drops are now visible) | a capped run delivers cap − drops, and says so |

- Mask PNG write failures (`AW:61`, `MasksDropped` never read) are not frame losses; A-1 wires them into
  `target_mask_frames_unavailable` as the writer's own warning already claims.
- Bench levers for the can-fail (new, behind the bench gate, compiled out of Shipping, `lever_audit.py` updated):
  `IAI.Bench.DropFrameAt <session_index>` (discards one completed frame before the writer → cause `bench_drop`) and
  `IAI.Bench.SkipArmAt <tick_index>` (one tick passes with no arm → cause `not_armed`).

---

## 5. The minimum writer change A-1 needs

Today [V]: one `Async(ThreadPool)` job per frame (`AW:22-32`), no queue limit, `GetPending()` unused, failures only counted
(`AW:99-102`), and run end waits at most 5 s (`SUB:5076`) before finalising counts.

**In A-1 (ruling (c): blocking is accepted; never drop by design):**
1. **Run end completes every job.** `FlushPending` waits until the pending count is 0, logging progress every 2 s. A hard cap
   (proposed 120 s) exists only so a broken host cannot hang forever; if it is ever hit, every unfinished frame is counted as
   `writer_timeout` and the run summary says so. Teardown (`SUB:1000`) gets the same rule.
2. **Counts are read after the flush, never before.** `FramesWritten` and the failure list are final when the summary and the
   annotation are written.
3. **Per-frame failure accounting.** The writer records the `session_index` of each failed frame (thread-safe list), with
   the cause. The annotation drops those indices from its frame lists.
4. **The `labels.jsonl` append result is checked** and counted (`label_rows_failed`).
5. **A simple in-flight cap with blocking.** Before enqueueing, the drain waits while more than N jobs are pending
   (proposed N = 32). Under the forced fixed timestep this costs wall time only, never a frame or game time. Reason it is
   in A-1: A-1 raises the frame rate by a third on the same hosts (16 frames per 16 ticks instead of 12), and an unbounded
   queue on a slow host ends in memory exhaustion, which is not a graceful outcome. (Question 5 if chat prefers A-2.)

**Left for A-2 (measured, one commit each):** dedicated writer threads instead of the shared pool, PNG compression level,
keeping `labels.jsonl` open, ordered rows, readback pooling, census period, snapshots during capture, run-log flushing,
per-tick anomaly walks.

---

## 6. Carrying `6713f51` (the m51 mask-wait fix)

- **What it does** [R, sub-reviewer read of `git show 6713f51`; stat 3 files +41/−11]: each snapshot freezes
  `bAwaitingTargetMask` from the *effective* mask flag at arm time and counts its own held ticks. The drain waits only for a
  snapshot that is awaiting a mask. A requested-but-ineffective mask commits immediately as unmeasured (`target_pixels −1`,
  `mask_state: unmeasured`) with a one-shot warning. The subsystem-wide hold counter goes away.
- **Does it apply to `master`?** `git apply --check` fails on one hunk (the drain hunk): its leading context expects the
  CAP-PAIR-DROP block directly above `if (Snap->bTargetMask)`, and `master` has the m52 ORDER HOLD block there
  (`SUB:4719-4732`). With reduced context (`-C2`) the whole patch applies (drain hunk lands at `SUB:4737`). The removed lines
  are byte-identical on `master`. [R; the anchors below are V]
- **Plan:** `git cherry-pick -x 6713f51` on the unit branch, resolve the one hunk by placing the new wait test after the
  ORDER HOLD block, and keep it as its own commit. If the cherry-pick drags anything else, re-implement on these anchors:
  effective flag `SUB:3750`; hold and reset `SUB:4734-4746`; "NEVER RESOLVED" `SUB:4755`; `GTargetMaskMaxHoldTicks = 4`
  `SUB:563`; arm `Snap.bTargetMask = bTargetMask` `SUB:5392`; the label-contract use of the requested flag `SUB:6131` stays.
- **When the defect is live on `master`:** output height ≠ 0, or `IAI.Capture.Mask 0` with the target mask on. Release
  defaults do not trigger it. The dashboard's Setup panel exposes an output-height control [R, 11-11 review].
- **Gates (ruling (b)), from the m51 record** [R, `git show m51:CLAUDE.md`]:
  - pre-fix build reproduces the drop in both trigger shapes (`TargetMask 1` + `Mask 0`; `OutputHeight 360`) — the m51
    measurement was 33 of 90 written before, 90 of 90 after;
  - fixed build: requested = written, "NEVER RESOLVED" 0, rows unmeasured, no hole;
  - **M-F1b** (never run): the same on the backbuffer path, `IAI.Capture.SVE 0`.
  - Note from m51: the `OutputHeight 360` leg cannot pass the B1 pose gate (its calibration box is in pixels), so it is a
    loss-count leg, not a pixel leg.

---

## 7. Gate plan

**Preconditions (none met today):** E: freed (095-01 D3); the primary host rebuilt (editor + game); **one cook** of
`master` content with the bench maps `CB_GateLevel`, `CB_LodFixture`, `CB_TexCorruptLevel`, `MainWorld`, `MainMenu`,
`Entry` (checked with `verify_cooked_maps.ps1`). No staged package exists today (`StackOBot\Builds` is empty) and the
release archive `_binary_baselines\release-2026-10-05-23098fa-218BA4C7` holds binaries only [V]. A-1 changes no shader, no
shader parameter and no content, so that one cook serves both the release exe (A-side) and the A-1 exe (B-side) by
code-only swap (G103).

**Which gates need no cook:** every PIE leg (needs only the rebuilt editor), the offline mutants, and the reader updates.
Staged legs need the one cook.

**Common recipe:** 1280×720, `IAI.Capture.Fps 30`, A63 harness `CaptureBench/tools/run_leg.ps1` (its hard-coded config and
predictions updated), pose gate where the target is `StaticMeshActor_49`, every attempt banked, both tick orders where
listed. Disk per frame measured at **0.72 MB all-in** (a banked 90-frame bench session is 64.9 MB) [V]; MainWorld assumed
~1 MB/frame [R].

| Gate | Legs | Readers and thresholds | Disk | Time |
|---|---|---|---|---|
| **G1 continuity** | `CB_GateLevel`, auto-pool (default four) seed 4242, 300 frames; {PIE, staged} × {paced, unpaced} × {native, synth} = **8**; plus **1** load leg: `MainWorld` staged, auto-pool, unpaced, 600 frames, delivered AA | new `m56_continuity_check.py` (CaptureBench): sort by `session_index`; `tick_index` delta = 1 on every pair; `frame_index` delta = 1 (engine cross-check); `ticks_per_captured_frame` = 1.000; `frames_dropped` = 0; `capture_span_ticks` = written frames; settle flag on exactly the frames armed in settle phases | 1.7 + 0.6 GB | ~45 min |
| **G2 edges from pixels** | AA-off arbiter. Per type: staged native, staged synth, PIE native = 3 legs × 8 types = **24**: `blinking`, `missing_object`, `missing_texture`, `corrupted_texture` (`CB_GateLevel`, `StaticMeshActor_49`, 120 frames); `lod_popping` (`CB_LodFix_N`); `uv_corruption`, `normal_corruption` (`CB_TexCorruptLevel`); `stuck_low_mip` (the B9 bench recipe) | `verify_capture.py --label-pixel-gate` (every edge `delta=+0`); `m44_gates.py` ONSET and G7; `label_sync_check.py` after it learns `settle` (0 unexcused frames); the frozen E1 reader for `stuck_low_mip`; `m56_continuity_check.py` again | 2.0 GB | ~2 h |
| **G3 masks** | from the G2 legs | every labelled frame, including after-onset settle frames, has `mask_state` present (or empty where the hide makes it so); MASK-TIE 0 mismatch; TARGET-PIXELS TIE 0 mismatch; `target_mask_frames_unavailable` 0 | — | — |
| **G4 can-fails** | (a) release exe on the G1 staged paced native leg = **1** (+1 PIE with the archived release editor DLLs); (b) `IAI.Bench.DropFrameAt 40` and `IAI.Bench.SkipArmAt 40` on A-1 = **2**; (c) shifted-label and moved-flag mutants = offline copies of a G2 session; (d) `6713f51`: pre-fix in both trigger shapes, fixed in both, M-F1b backbuffer = **5** | (a) continuity check must FAIL (predicted: `ticks_per_captured_frame` ≈ 1.33, 15 `frame_index` jumps of 3 per 90 frames); (b) `frames_dropped` = 1 with the right cause and tick, the file number missing, the encode step warns; (c) every reader in G2 must FAIL; (d) as §6 | 0.7 GB | ~45 min |
| **G5 non-settle frames unchanged** | pose-matched pairs, release exe vs A-1 exe, same seed and target, native: `blinking` and `missing_texture` × {AA-off, delivered} × 2 repeats = **8** | new `m56_nonsettle_compare.py`: join rows by tick offset (`frame_index` − first `frame_index`), every release row must equal its A-1 row on every label, mask and box field; allowed differences only `tick_index`, `t_wall_run` and the run-unique set measured from a same-binary control pair (A64 pose precondition first). Proven both ways on a doctored pair | 0.6 GB | ~40 min |
| **G6 settle exactness** (reported, not gated) | one null leg per type at AA-off, staged native = **8**; the G2 legs supply the rest | §8 | 0.6 GB | ~40 min |

- **Totals:** about **58 legs, ~6.2 GB of sessions, ~5.5 h of bench time**, plus the rebuild (~1–1.5 h, ~14.5 GB on E:) and
  one cook (~45–75 min, several GB). The E: floor makes the disk the first blocker.
- **Predictions to freeze before the first leg** (one predictions file, committed first): frames = cap; tick deltas all 1;
  `ticks_per_captured_frame` 1.000; `frames_dropped` 0; 2K settle frames per complete cycle; per event 8 unflagged + 2
  flagged labelled frames; onset frames at session ≡ 3 (mod 16) at defaults; about 18 events per 300-frame leg; the release
  can-fail numbers above; `blinking`'s hidden set derived in tick space from `Anomaly_Blinking.cpp` (no hand-copied shapes
  from the 12-frame era); the dashboard and encode-step behaviour on the drop leg.
- **Stop rule:** a failed gate is reported and stops; no same-turn fix.

---

## 8. Measuring whether settle frames are pixel-exact

Goal: give chat evidence, per delivered anomaly, to rule later whether the `settle` flag can be dropped (K default → 0).

- **What "exact" means.**
  - An **after-onset** settle frame is exact if its target region already matches the steady anomalous state: its
    difference from the median of that event's unflagged positive frames is within the null-noise floor, and its difference
    from the pre-onset clean frames exceeds the edge threshold `tau`.
  - An **after-offset** settle frame is exact if it already matches the clean steady state: its difference from the median
    of the following unflagged post frames is within the noise floor (no residue of the anomaly).
- **Region.** The event's own target mask where it exists (the after-onset frames have one); the frozen label box
  otherwise. Whole-frame reading reported beside it, never instead of it.
- **Noise floor.** A same-build null leg at the same seed and pose with the anomaly suppressed (the bench `null_effect`
  anomaly, already registered behind `-IAIBench`), at the same session indices.
- **Configuration.** The identity arbiter: `r.AntiAliasingMethod 0` with Lumen, GI and reflections off, native tick order
  (the m45 rule; synthesised order cannot host a pixel arbiter, G230). A delivered-config (TSR) reading is reported but never
  decides: two runs of one build differ on ~9 % of pixels there (G228).
- **Instrument.** Extend the existing `tools/verify_capture.py --label-pixel-gate` edge machinery (it already scores
  per-frame change against a `tau`) with a per-settle-frame table, or a small CaptureBench script that imports it.
  Prove it both ways first (G96): a doctored session whose settle frame is a copy of the previous clean frame must read
  NOT exact; a doctored copy of a positive frame must read exact.
- **Coverage.** Every delivered anomaly with a burst edge: `blinking`, `missing_object`, `missing_texture`,
  `corrupted_texture`, `lod_popping`, `uv_corruption`, `normal_corruption`, `stuck_low_mip` (both routes).
  `camera_clipping` is a whole-session hold with no burst edge; it is out of this table.
- **Output, per anomaly:** settle frames scored, exact / not exact, the worst residual, AA-off and TSR columns, PIE and
  staged columns, ≥ 8 events each.
- **Proposed decision rule (for the later ruling, not used in A-1):** drop the flag for a type only if 100 % of its settle
  frames are exact at AA-off over ≥ 16 events in PIE and staged. Known non-exact candidates to expect: `stuck_low_mip`
  (the streamer takes ticks to change mips) and anything with temporal history under TSR.

---

## 9. Risks to label sync

**What history established** [R unless marked; quotes from `docs/gotchas.md` via the sub-reviewer]:
- **G37 (m7)** created settle for the old synchronous grab: a fire or revert "does not appear in the captured pixels for
  ~1 frame", so K = 2 = one frame of lag plus one of margin.
- **G78 (m18)** refuted that premise for the async path: "a state change made at the end of tick N lands in frame N's OWN
  render"; settle ticks "capture nothing", and phase timing and K were left byte-unchanged.
- **G82 (m21)** made arm→present pairing deterministic in every regime (the deep-starvation stale-scene residual stays open).
- **m40** samples every label bit at `OnWorldTickEnd`, independent of tick order [V: `SUB:1319-1327`].
- **G457 (090-12)**: with `Config 2` the proxy-blur reader had to censor edges because "two uncaptured engine frames"
  followed bind and revert; `Config 0` then produced continuous AA-off captures with 0/0 edges. The K = 0 fix is on
  `master` [V: `SUB:5328-5329`, `5346-5347`]. Under TSR the same records show a decaying tail of +7..8 frames.
- ⇒ No remaining reason exists to skip the settle ticks on the async path. The remaining reason to *flag* them is that
  every gate on record except 090-12's proxy legs ran with K = 2.

**Where a label could shift by a frame, and why A-1 should not:**
1. **The onset frame and the revert frame** are captured today and labelled after the transition (m18). A-1 does not
   touch them, `BeginFire`, `BeginRevert` or `FinalizeArmedLabel`.
2. **Anomaly clocks count ticks**, not captured frames (blink half-period, `lod_popping` swap period). After A-1 their
   pattern in `session_index` space changes shape (it becomes regular). That is a change of *frames*, not of labels; every
   expected set from the 12-frame era (G-EDGE `{4,5,9,10}`, the P9 shapes) must be re-derived, not compared.
3. **m26 veto arms** run every tick-end whether or not a frame is captured (`SUB:1355`) and keep the same ticks because K and
   the phase lengths do not change. ⚠ If a later ruling sets K = 0, `missing_object`'s post-revert arm window shrinks from
   6 ticks to 4, which is exactly its budget (G351, F-3); that ruling must re-check m26.
4. **Temporal AA tails.** Release defaults have no temporal flags (0/0/0). The two after-offset settle frames carry the
   TSR residue that today is skipped; the `settle` flag covers exactly those two. Frames beyond them already show the tail
   unflagged today (pre-existing, unchanged by A-1).
5. **The m47 first-use cost** was measured on the onset frame at K = 2; A-1 keeps the onset frame identical.
6. **The m53 bench levers** count engine frames (G407: effect "1 + max(0, D − 1 − K)" captured frames). With settle captured
   their effect in captured frames becomes D. Any m53 lever prediction must be re-derived.
7. **Mask load:** the target mask now arms on 16 frames per cycle instead of 12. Carry `6713f51` first so the hold logic is
   per-snapshot.
8. **Exposure dip** (m48) uses the previous 8 captured frames; the window becomes 8 contiguous ticks (more correct).

**What would prove it safe:** G2 (0/0 edges with settle frames present, both orders, AA-off), G5 (every non-settle row
identical to the release build after the tick-offset join), G3 (masks on every labelled frame), and the §8 table.

---

## 10. What the client will see change

Plain words, for chat's client note:

- **No more hidden jumps.** Today two engine frames are silently missing right after every anomaly appears and right after
  it disappears. Motion jumps there, exactly where the label changes. After A-1 every frame is present.
- **More frames per anomaly cycle.** With the default settings a cycle is 16 frames instead of 12: 4 clean, 2 flagged,
  8 positive, 2 flagged. A capture with a frame limit therefore holds about a quarter fewer anomaly events for the same
  limit (a 90-frame capture: about 5–6 events instead of 7–8).
- **Flagged frames.** The 2 frames after each onset are labelled with the anomaly and flagged `settle`; the 2 frames after
  each offset carry a `settle`-flagged entry and no label. The client's existing rule ("drop frames with a transition flag
  from both positives and negatives") already removes them. No new rule is needed.
- **Video duration.** The assembled video plays in real time. Today it is about 25 % shorter than the time it covers.
- **New keys** (all additive): `tick_index` and `t_wall_run` on every row; `frames_dropped` with the missing tick indices
  and causes in `run_summary.json`; `label_schema_minor` becomes 2. On a healthy capture `frames_dropped` is 0 and
  `ticks_per_captured_frame` is exactly 1.
- **Blink patterns.** A blinking object now hides and shows on a regular rhythm in the frame sequence. Today's irregular
  pattern was caused by the missing frames.
- **A documentation fix.** `t_wall` was described as "seconds since the run started"; it is an absolute clock. The new
  `t_wall_run` is the run-relative one.
- **Unchanged:** the meaning of every existing label, mask and box on every frame that exists today.

---

## 11. Commit breakdown and estimate

On `fix/m56-capture-continuity` off `master`:
1. `fix(capture): carry m51 F1 - wait only for a mask outcome that can exist` — `git cherry-pick -x 6713f51`, one hunk
   placed after the ORDER HOLD block.
2. `fix(capture): m56 capture every settle tick and flag it settle` — `SYNC` bit, the two phase cases, `StepSettleFlags`,
   reverted-fire transition entries, row-level flag, annotation minor 2, counters, echo wording.
3. `feat(capture): m56 tick index, run wall time and one honest drop count` — span accounting, causes (including capturer
   drop reports), the stamp formula, `ticks_per_captured_frame`, annotation filtering of failed frames.
4. `fix(capture): m56 writer finishes every job at run end and counts failures` — §5, plus the two bench levers and the
   `lever_audit.py` update.
5. `chore(tools): m56 readers learn settle and schema 2.2` — `verify_capture.py`, `label_sync_check.py`,
   `annotation_transition_check.py`, `measure_label_offset.py`, `exclusion_gate.py`, their tests.
6. `docs: m56 capture continuity` — readme, architecture, capture-fps, checklist, status block, journal, gotchas (next G574+).

Outside the plugin repo (later briefs): AnomDash — encode-step gap warning, gap reporting instead of frame repeat, re-bundle;
CaptureBench — `run_leg.ps1` predictions, `a54_oracle.py` flank rule, the two new check scripts and the comparator.

**Estimate:** five briefs.
- (i) the deferred rebuild and baseline (already queued; needs E:);
- (ii) implementation and offline tests, both targets building with zero warnings (~4–5 h of Code);
- (iii) cook and gates (~6–7 bench hours; may split into two windows);
- (iv) an independent read-only review side session (~1 h);
- (v) docs, merge to `master`, dashboard re-bundle (~1–2 h).

About **8 bench hours** in total, after disk is available.

---

## Questions for chat

1. **The 675 → 676 office read (A5).** At the default config, onset frames sit at `session_index` ≡ 3 (mod 12), and
   675 = 56 × 12 + 3 [V arithmetic; verified on a banked session's onsets 3, 15, 27, …]. So frame 675 is exactly where an
   onset frame and then a two-tick settle gap fall, *if* the session used defaults and the client counts from 0.
   **Recommendation:** add to the owner's office read, numbers only: in that session's `labels.jsonl`, the `frame_index` and
   `t_wall` of rows `session_index` 674–677. A `frame_index` step of 3 at 675→676 means the C1 gap; a step of 1 means the
   other family (a wall-clock hitch). No mechanism is claimed until then.
2. **`ticks_per_captured_frame`:** redefine it as span ticks / frames written (exactly 1.0 on a healthy run, as A3 asks)
   and add `capture_span_ticks`, or keep the old meaning and add a new key? **Recommendation: redefine and changelog it**;
   its documented purpose is exactly this reading, and the old value counted warm-up and drain ticks that are outside the
   frame span.
3. **Settle frames with no event entry** (a zero-match burst; a suppressed `stuck_low_mip` entry): flag them at row level
   anyway? **Recommendation: yes.** Every settle frame carries the row-level flag, so the client rule needs no exception.
4. **A frame that failed to write:** remove its index from `annotation.json` frame lists (it does not exist) and count it,
   or keep it listed? **Recommendation: remove and count.**
5. **The writer in-flight cap (§5 item 5):** in A-1 or in A-2? **Recommendation: A-1**, because A-1 adds a third more
   frames on the same hosts.
6. **Gaps in the encode step (dashboard repo, a later brief):** ffmpeg truncates silently at the first missing number;
   the dashboard's own encoder repeats the previous frame. **Recommendation:** both report the gap loudly and never duplicate
   a frame; encode the frames that exist and name the missing indices in a sidecar and in the UI.
7. **Interim advice before A-1 ships:** `IAI.Capture.Config 0 4 8 4 0` already gives continuous capture on the release
   (the G457 fix is on `master`), but unflagged, and only the 090-12 proxy legs were gated that way.
   **Recommendation: do not advise the client to change it;** use it only as an office bisect reading.
8. **`label_schema_minor` order:** unit C lands first. **Recommendation:** C adds a value to an existing string field and does
   not bump the minor; A-1 bumps it to 2.

---

## 4.25 note

Read-only diff `master` vs `feat/europa-port` for the files A-1 touches (receipt
`code-receipts-095-02\europa-port-diff-touched-files.txt`) [V]:

| File | Diff on `feat/europa-port` |
|---|---|
| `AnomalyCaptureSubsystem.cpp` | +204 / −43. One single-line hunk at the top of `Tick` (`master` line 1021, a guard) and two single-line hunks in `FinishRun` (`master` 8817, 9725); **none** in the phase switch, `BeginFire`/`BeginRevert`, `CaptureCurrentFrame`, `ProcessCompletedFrames`, `DrainAsyncToCompletion`, the transition steps or `MapAnomalyToClient`. The rest are engine-version guards, the 4.25 capture hooks, `StartRun`/`StopRun`, mask and census code (receipt `europa-port-subsystem-hunks.txt`) |
| `AnomalyCaptureSubsystem.h` | +14 / −15 |
| `AnomalyLabelWriter.cpp/.h` | +5 / −1, +4 |
| `AnomalyAsyncWriter.cpp/.h` | +2 / −1 each |
| `AnomalySveCapturer.cpp/.h` | +5 / −1, +2 / −1 |
| `AnomalyFrameCapturer.cpp` | **+114 / −5** (the 4.25 pre-UI capture path; its drop causes D8 must be re-derived there) |
| `AnomalyLabelSync.h`, `verify_capture.py`, `label_sync_check.py`, `annotation_transition_check.py`, `client-readme.md` | identical |

The forward-port (M5) should merge cleanly except around `AnomalyFrameCapturer.cpp`. The 4.25 capture path has its own
drop paths and must be gated on its own.
