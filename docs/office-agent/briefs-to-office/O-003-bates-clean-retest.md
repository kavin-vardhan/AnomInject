# O-003 — Bates: a clean install of the 096-07 build, and the retest of O-002's findings

**From:** chat Claude (orchestrator), via the home agent's brief 096-07, 2026-11-24. **To:** the office agent. **Runs after
O-002.** **Host: Bates (UE 5.3.2) only.** Answer as report cards (`REPORT-FORMAT.md`). Every prediction below was written
at home before any of this ran; report what you read, including where it disagrees.

**What changed since O-002 (all in the commit below, built and unit-tested at home on UE 5.1; nothing was run in an
engine):**
- **The 5.3 build fix is in the code** (it was the office-only experiment E1b in O-002). The clean install removes E1b.
- **The readers are fixed** for PIE sessions (`office-tools\` in this pack, and the copy's `tools\verify_capture.py`).
- **Every fire changes exactly the one object its label names.** The plugin now applies an anomaly by the object's full
  path, so a blink no longer hides every object that shares the target's name. A targeted capture or `IAI.Apply` whose
  name matches several objects is refused `target_ambiguous:<n>`; the log line lists the first three objects by path.
  `@<path>` (for example `@/Game/<map>.<map>:PersistentLevel.<name>`) targets one object.
- **Objects that can never show a change are refused up front:** an object the game keeps hidden (`target_hidden`) and an
  HLOD proxy, the merged stand-in a game draws for distant objects (`hlod_proxy`).
- **`lod_corruption`** uses the engine's own shown LOD and refuses a forced LOD with fewer than 4 times fewer triangles
  (`no_contrast_lod`) or an object under 7 % of the screen (`below_min_coverage`).
- **The LOD turn trigger** arms on turns of 5 degrees per second and more (setting `IAI.Anomaly.LodMinYawRate`, was a fixed
  30); the arm timeout is 150 frames (was 90); the walk trigger arms at 20 cm/s and more (was 50) and looks 300 frames
  ahead (was 90); the pool's `Auto.Yield` line lists every refusal reason with its count.
- **Terrain blinks have their own coverage ceiling,** `IAI.Blink.TerrainCeilingPct` (default 25, no change).

## Step 1. Before installing: two readings from the O-002 logs already on the office PC
Read the run logs (`anomaly_log.txt`, or the editor log of the session) of O-002's captures on the new build. Nothing is
installed or run for this step.
1. **Repeated names.** Count the lines `blinking: matched <N> actor(s) for '...'` and `missing_object: matched <N> actor(s)
   for '...'` with N > 1, per anomaly and per capture (pool and targeted). Report the counts and the largest N, never a
   name. **Prediction:** at least one line with N > 1 in the blink pool captures of Q4 (many names on L1 are shared by several
   actors); each such line is one fire that hid several objects while the label named one.
2. **The marker actor** (the gameplay marker on which `m56_settle_content_check` failed in O-002 card 3): do its blink
   frames log `TARGET MASK NOT ARMED ... visible=0` (or `withRecord=1 visible=0`)? Yes or no, and on how many frames.
   **Prediction:** yes on every hidden frame of its blink events: the game keeps that actor hidden, which is the case
   096-07 now refuses (`target_hidden`).

## Step 2. Install the new build untouched
- **The owner instructs, you do the copy** (as in O-002): OFFICE-SETUP part B **steps 3 to 7**, after the owner has checked
  out **`895133c`** (branch `feat/m57-lod-spatial-trigger`) in his clone (step 2). Back up the current copy (it holds
  `d2f6873` plus E1b) to a new folder in `C:\OfficeAgent\plugin-backups`. `COPIED_COMMIT.txt` must read `895133c`, and the
  game's `Plugins` folder must hold exactly one `AnomalyInjector.uplugin`; if not, stop and ask.
- **Do not re-apply E1b or any other edit to the plugin copy.** From here every reading is from an untouched copy: put
  `untouched` on line 1 of every card. If the build fails, report the errors and stop; do not fix the plugin.
- **Build** the editor target as in O-002. **Prediction:** 0 errors; 0 warnings in plugin files (the `UWorld::FeatureLevel`
  deprecation warnings of O-002 card 2 are gone). Report errors, warnings in plugin files, and other warnings, as counts.
- Keep H-001 and H-002/H-002b as they are (host changes, not plugin edits); put `H H-001` on cards where it matters, as in O-002.

## Step 3. Re-run, with the fixed readers (all on L1, PIE, delivery mode off, as in O-002)
**3a. Q2 continuity again** (pink = `corrupted_texture`, checker = `missing_texture`, blink; 60 bursts each, as in O-002 card 3).
- `m56_continuity_check.py`, `m56_settle_content_check.py`, `verify_capture.py --label-rule`: PASS or FAIL and the counts.
- **Prediction:** `m56_continuity_check` PASS on all three (O-002: pink and checker FAILED C7, a reader defect fixed in
  096-06); `verify_capture --label-rule` NO MISMATCH on all three (O-002: pink and checker SETTLE-MISPLACED);
  `m56_settle_content_check` PASS, listing never-labelled events if any instead of failing. The marker actor no longer
  appears as a target (refused `target_hidden`; see 3d). `ticks_per_captured_frame` 1.0, `frames_dropped` 0,
  `labels_sorted` true, as in O-002.

**3b. Q5 `lod_corruption` on the two rocks with unique names** (M2 and M3 of O-002 card 4; 300 frames each, targeted,
`IAI.Capture.Start "" png "" 300 lod_corruption <name>`).
- Events, refusals by reason (from `lod_corruption: REFUSED '<mesh>' (<reason>)` lines), and
  `verify_capture.py --label-pixel-gate --report-only`: CONSISTENT / OFFSET-NOTE / NO-TRACE / PARTIAL / UNASSESSABLE.
- **Prediction:** NO-TRACE **0** on both (O-002 card 4: M2 had NO-TRACE events). M2 has fewer events than in O-002 card
  4, the missing fires refused by name (`no_contrast_lod`, `below_min_coverage` or `not_coarser_than_current`); M3 about as
  many events as in O-002 card 4, NO-TRACE 0, unless the rock covers less than 7 % of the screen, in which case it is
  refused `below_min_coverage` on every fire and has 0 events. Either way no labelled frame without a visible change.

**3c. HLOD proxies refused (`hlod_proxy`).** On L1: one auto-pool capture with the default pool (600 frames), then
`IAI.TexCorrupt.Census allreasons`, then a targeted capture on the HLOD proxy P1 of O-002 card 1 by its path
(`... uv_corruption @<path>`; take the path from `IAI.ListActors` or from a refusal line).
- Report: the `Capture(096-07): HLOD RULE ...` line (which of the two class paths resolved: `found` / `NOT FOUND`; do not copy
  anything else from it); `run_summary.hlod_proxy_excluded_targets`; the number of `EXCLUDED-HLOD-PROXY` lines; vetoed
  events on proxies (`vetoed_event_ids`, and `VETOED-OBJECT` lines naming a proxy class); the census counts for uv and
  normal; the targeted capture's result.
- **Prediction:** at least one class path `found` (both exist in 5.3's Engine module); `hlod_proxy_excluded_targets` > 0;
  no event and no veto on a proxy (O-002: every event on a proxy was vetoed); uv/normal eligible 0 on L1 (the proxies were
  their only candidates; they are now refused, the texture search is not widened); the targeted capture refused
  `hlod_proxy`, 0 events.

**3d. Hidden objects refused (`target_hidden`).** A targeted blink on the marker actor (by its path), and the default-pool
capture of 3c.
- Report: the targeted result (refusal reason and events); `run_summary.hidden_excluded_targets`; the number of
  `EXCLUDED-HIDDEN` lines.
- **Prediction:** the targeted blink is refused `target_hidden`, 0 events; `hidden_excluded_targets` ≥ 1 if the marker is
  in view during the pool capture.

**3e. Repeated names refused (`target_ambiguous`).** A targeted capture of `lod_corruption` on the 5-LOD rock meant as M1 in
O-002 (its name is shared), first by its name, then by its path.
- **Prediction:** by name: refused `target_ambiguous:<n>`, n ≥ 2, the log listing the first three paths, 0 events. By path:
  it runs on exactly that rock; NO-TRACE 0 as in 3b. Also report, from the 3c pool capture's log, the largest N on any
  `blinking: matched <N> actor(s)` or `missing_object: matched <N> actor(s)` line. **Prediction: 1.**

## Step 4. The LOD trigger (m57), motion injected by you as in O-002 card 5
- **Turn-in** toward scenery behind the start spot at about **7 degrees per second**, and again at about **32 degrees per
  second**, entry frames 30 (`IAI.Anomaly.LodEntryFrames 30`), pool `lod_popping` only, 900 frames each.
- **Walk** toward scenery at a normal pace, **20 to 60 cm/s** (say the speed you used), about 15 m, distance mode found by
  itself (no mode argument), 900 frames.
- Readings per capture: events and `lod_event_kind`; each swap's `cause` and `lod_spatial_swap_visible` (**must be true or
  false on every swap**, never absent); `lod_untriggered_event_ids` count against the armed count (`lod_popping: ARMED`
  lines); `lod_arm_timeouts`; the `Auto.Yield lod_popping` line's reasons with their counts (copy the reason words and
  numbers, nothing else); `m57_lod_readout.py --expect events --expect-mismatch 0 --expect-restored yes`; the run-config
  `lod_trigger_settings` should read `armTimeout=150(compiled) ... minYawRate=5(compiled)` (check, do not copy).
- **Predictions:**
  - 7 deg/s: events **> 0** (O-002 card 5: none), every swap `view_entry` with `lod_spatial_swap_visible`
    true on on-screen swaps; `lod_render_mismatch_frames` 0; readout PASS; untriggered at most a third of armed.
  - 32 deg/s: about as many events as O-002 card 5 at entry 30, or more (the 150-frame timeout leaves fewer fires
    untriggered); mismatch 0; readout PASS.
  - Walk: at least one `distance_threshold` event if any scenery mesh's LOD switch is reachable within 10 s at that speed
    and covers 7 % of the screen; if there is none, 0 events and the `Auto.Yield` line names why with counts (for example
    `too_far_ahead`, `at_finest_lod`), and `not_approaching` does **not** dominate at 20 cm/s or more (O-002 card 5: the
    walk was below the old 50 cm/s floor).
- **Clips for the owner:** the 7 deg/s and 32 deg/s turn-ins (and a walk event if any) as animated **GIF or WebP made with
  Pillow** (there is no ffmpeg on the office PC; write a short script of your own in your scratch folder). The owner judges
  by eye whether each pop looks like the client's reference clip; put his words on the card. Keep clips and their frame
  copies outside the session folders, the pack and the game's plugin copy (rule 10); delete frame copies after he has seen
  them.

## Step 5. Terrain blinks with a 40 % terrain ceiling (m58), H-001 in place
- Before the capture: `IAI.Blink.TerrainCeilingPct 40` (console). Pool `blinking` only, 600 frames, the two views of O-002
  card 6. The `Capture(m58): TERRAIN RULES ...` line must end with `terrain_ceiling=40.00%(console; ...)`: check, do not
  copy.
- Readings: blink events; `run_summary.blink_terrain_events`; `annotation.json` events with `anomaly_subtype`
  `terrain_disappear_reappear`; `TERRAIN-CEILING` lines (count only); the share of the screen each terrain blink hides
  (`labels.jsonl` `target_pixels` on its hidden frames over the frame's pixels); `verify_capture.py --black-frame-gate`
  (PASS or FAIL, darkest frame value); **and the owner's eye on every terrain blink frame: is any frame black or empty?**
- **Predictions:** the ground under the player (O-002 card 6: drawn above 25 % and below 40 %) is re-admitted
  (`TERRAIN-CEILING` ≥ 1); `blink_terrain_events` > 0 and equal to the `terrain_disappear_reappear` count; hidden share as in O-002 card 6 on those
  events; ordinary blinks as before. Whether a black or empty frame appears is **not predicted**: that is what this step
  decides (chat sets the default from it). Three terrain blinks as GIF clips for the owner.

## Cards
As in O-002: one card per photo, line 1 names O-003, Bates, the plugin commit from `COPIED_COMMIT.txt`, `untouched`, and the
host changes in effect; the last card ends with `ALSO:` and `END O-003 · <total lines>`. Never a name, path, pattern or
class word of the host on a card (rule 6); counts, reasons and verdicts only. Leave the new build installed at the end and
say so.
