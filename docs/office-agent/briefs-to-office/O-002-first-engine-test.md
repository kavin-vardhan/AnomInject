# O-002 — first engine test of the four client-feedback units, on the real hosts

**From:** chat Claude (orchestrator), 2026-11-22. **To:** the office agent. **Runs after O-001,** whose release-build
readings are the "before" side of every comparison here. Answer as report cards (`REPORT-FORMAT.md`), one set per host.

**Hosts: Concorde and Bates** (UE 5.1). **Not Europa:** these units are not yet ported to 4.25.

**What is being tested.** Four units, written at home, reviewed, never run in an engine:
- **m56, capture continuity** (client topic 1): every rendered frame is written; the frames at each anomaly edge carry
  transition reason `settle`; every row has `tick_index`; `run_summary` reports dropped frames honestly.
- **m57, LOD spatial trigger** (client topic 2): `lod_popping` holds a coarse LOD and releases it when the player turns
  toward the object (`view_entry`) or walks toward it (`distance_threshold`), instead of on a timer.
- **m58, terrain blink subtype** (client topic 3): a blink on terrain is labelled subtype `terrain_disappear_reappear`.
- **m59, forced-LOD refusals:** no LOD label where forcing the LOD changes nothing on screen.

**Setup** (`<game>` below is the game project folder you were started in)
- **The owner puts the test version in, not you.** He checks out the pushed commit **`d8d3a7e`** (branch
  `feat/m57-lod-spatial-trigger`) in his clone, moves the game's old plugin copy out to a backup, copies `d8d3a7e`
  into `<game>\Plugins\AnomalyInjector` and writes `d8d3a7e` into its `COPIED_COMMIT.txt` (`OFFICE-SETUP.md`, part B).
  You do not run git. Confirm that `COPIED_COMMIT.txt` reads `d8d3a7e` and that the game's `Plugins` folder holds
  exactly one `AnomalyInjector.uplugin`; if either is wrong, stop and ask. Then build.
- Build the host the way it is normally built for captures (the new copy has no build files, so the plugin compiles
  from scratch). Use the same levels, paths and settings as O-001 so that every reading has a before and an after.
- Q5 and Q7 also need release-build captures on the same path. Take them while the game still holds the delivered
  build (before the owner copies `d8d3a7e` in), or ask the owner to put the delivered build back for them (he keeps
  it as his backup). Never switch versions yourself. Say on the card which version each reading came from.
- Set the terrain name pattern for this host as described in the plugin readme (`docs/client-readme.md` §8.4.2): in
  the game's console type `IAI.SetTerrainPatterns <word> [word...]` before the first capture (a packaged build ignores
  an edited loose ini file; in the editor, or in a build cooked with it, `[AnomalyInjector] +TerrainNamePatterns=<word>`
  in `DefaultGame.ini` does the same). Each capture's log prints a `Capture(m58): TERRAIN RULES ...` line with the list
  in effect and where it came from: check it, do not copy it onto a card. **Never report the pattern.**
- **Capture with delivery mode off:** type `IAI.Capture.Delivery 0` before each `IAI.Capture.Start`. With delivery
  mode on the plugin writes no `run.json` and no run log (`anomaly_log.txt`), and the readers below cannot judge.
  Delivery mode does not change the labels.
- **The readers** are in `C:\OfficeAgent\pack\office-tools\`; read its `README.md` first and run each reader's
  `--selftest` before using it. `verify_capture.py` is in the game's plugin copy:
  `<game>\Plugins\AnomalyInjector\tools\verify_capture.py`; run it with `python -B` so Python writes no `__pycache__`
  into the copy. Readers only read the session folders.
- Capture commands (game console): an automatic-pool capture of N frames is `IAI.Capture.Start "" png "" N`; limit the
  pool first with `IAI.Auto.Pool all 0` and then `IAI.Auto.Pool <id> 1` for each anomaly wanted (the default pool is
  `blinking`, `missing_texture`, `corrupted_texture`, `lod_popping`). A capture of one anomaly on one named object is
  `IAI.Capture.Start "" png "" N <anomaly> <object name>`, where `<object name>` is the actor name as
  `IAI.ListActors` prints it (editor labels do not exist in a cooked build).
- Leave the test version in place at the end and say so; chat will say when the owner switches back.

**Q1. Build.** Does the plugin compile on this host, with how many warnings in plugin files? If it does not, give our
`file:line` and the error in your own words; you may prove a fix with a throwaway experiment (rule 3).

**Q2. No frames skipped (m56).** Same level and path as O-001 Q3: one capture per default anomaly set, at least 300
frames each, default settings (delivery mode off, as above).
- From `run_summary.json`: `ticks_per_captured_frame` (**expected exactly 1.0**; O-001 on the release build should
  read about 1.33), `frames_dropped` (**expected 0**), `frames_dropped_by_cause`, `capture_span_ticks`.
- From `labels.jsonl`: does `tick_index` step by exactly 1 on every row? Count of rows flagged `settle` per anomaly edge
  (**expected 2 at default settings**).
- Readers: `python m56_continuity_check.py <session>` (expected PASS; run it on an O-001 release session too: expected
  FAIL), `python m56_settle_content_check.py <session>`, and
  `python -B <game>\Plugins\AnomalyInjector\tools\verify_capture.py --dir <session> --label-rule`. PASS or FAIL each,
  and the count of findings.
- Does the assembled video now last as long as the capture took?

**Q3. The texture stall, again.** Repeat O-001 Q3 exactly on this build: the wall time of the frame where each pink
(`corrupted_texture`) and each checkered (`missing_texture`) texture corruption starts, the two frames after it and the
frame where it ends, against the run's median frame (from `labels.jsonl`: this build's rows carry `t_wall_run`, seconds
since the capture's first frame; the release rows carry only `t_wall`); first event against later ones; blink as the
control. Same, better or worse than the release build?

**Q4. Terrain blinks (m58).** Captures with blink enabled on two levels with a lot of ground (`IAI.Auto.Pool all 0`,
`IAI.Auto.Pool blinking 1`, then `IAI.Capture.Start "" png "" 600`; the pool id is `blinking`, the event type in
`annotation.json` is `blink`).
- `run_summary`: `blink_terrain_events`, `terrain_unlabelable_refusals`, total blink events.
- `annotation.json`: how many blink events carry `anomaly_subtype` `terrain_disappear_reappear`. Check 10 of them by
  eye: is each really ground? Check 10 ordinary blinks: is any of them ground that was missed?
- Did the run end with the "every blink was terrain" warning (log line
  `Capture(m58): EVERY blink event of this run (<n> of <m>) was labelled terrain_disappear_reappear ...`, printed only
  when the run had five or more blink events)?
- Typical share of the screen a terrain blink hides (`labels.jsonl` `target_pixels` on its hidden frames over the
  frame's pixel count; the event's `coverage_ratio` in `annotation.json` is the box, not the shape).

**Q5. No LOD label without a visible change (m59).** One capture each of `lod_corruption` and of `lod_popping`, on a
level with plenty of scenery, on **both** this build and the release build, same path.
- `lod_corruption` is not in the automatic pool: a capture fires it on one named object,
  `IAI.Capture.Start "" png "" 300 lod_corruption <object name>`. Do one capture per object for a few objects of
  different kinds (made of instanced or hierarchical-instanced meshes; a mesh with one LOD; a mesh with several LODs; a
  character), the same objects on both builds. `lod_popping`: `IAI.Auto.Pool all 0`, `IAI.Auto.Pool lod_popping 1`,
  then an automatic-pool capture.
- This build: refusals by reason (`hism_ignores_forced_lod`, `single_lod`, `skeletal_unmeasured`,
  `not_coarser_than_current`, `current_lod_undetermined`), and events applied. They are in the run log:
  `lod_corruption: REFUSED '<mesh>' (<reason>)` per refused mesh; for `lod_popping`, `lod_popping: NOT ARMABLE '<mesh>'
  (<reason>)` per mesh and `lod_not_armable:<reason>` on the `Auto.Yield` line (the spatial trigger adds reasons of its
  own, such as `not_approaching`; count those separately).
- `python -B <game>\Plugins\AnomalyInjector\tools\verify_capture.py --dir <session> --label-pixel-gate --report-only`
  on both (on the release build, its copy's own `verify_capture.py`): how many
  complete events read NO-TRACE (a label with no visible change)? **Expected: 0 on this build;** some on the release
  build if this host has instanced foliage or single-LOD meshes.

**Q6. LOD pops with a spatial cause (m57). The owner judges these by eye.**
- The owner plays: (a) stands still and turns slowly toward scenery that was behind him; (b) walks toward scenery from a
  distance; (c) stands still for 30 seconds without turning. A capture runs with `lod_popping` in its spatial modes:
  on this build a capture fires `lod_popping` only on a spatial cause, choosing `view_entry` (turning toward an object
  just outside the view) or `distance_threshold` (walking toward it) by itself; the old timed pop (`periodic`) is
  refused in a capture. Use `IAI.Auto.Pool all 0`, `IAI.Auto.Pool lod_popping 1`, then `IAI.Capture.Start "" png "" 900`
  (30 s at 30 fps).
- Readings: `lod_swaps` per event with `cause`, `lod_event_kind`, `lod_spatial_swap_visible`; events never triggered
  (`lod_untriggered_event_ids`); the flagged-entry counts `label_transition_lod_subvisible_entries` and
  `label_transition_lod_unconfirmed_entries` in `run_summary.json`; `m57_lod_readout.py`
  (`--expect events --expect-mismatch 0 --expect-restored yes` for (a) and (b),
  `--expect no_events --expect-mismatch 0` for (c)). (c) should give no LOD event.
- Encode the turn-in with `IAI.Anomaly.LodEntryFrames` at 15, 30 and 60 (set it before each capture), using
  `m57_make_clip.py` without `--null` (a played path has no clean twin, so the capture is encoded alone):
  `python m57_make_clip.py --verify <game>\Plugins\AnomalyInjector\tools\verify_capture.py --anomaly <session> --out <folder> --name <name>`,
  with `<folder>` in the office captures area beside the sessions: never inside a session folder, and never in the
  pack (`C:\OfficeAgent\pack`), the game's plugin copy, a repo, a synced folder, a network share or removable media
  (rule 10). The clip tool's `<name>_overlay\` and `<name>_frames\` are working frame copies: delete them once the
  owner has watched the clips; the clips may stay. **The owner watches them at the office.** Put his verdict on the card in his words: does the pop look like
  the client's reference clip, and which length looks most like a real game?
- How many objects in view could pop at all, and how many did? (The run log names every mesh tried:
  `lod_popping: ARMED '<mesh>' ...` or `lod_popping: NOT ARMABLE '<mesh>' (<reason>) ...`.)

**Q7. Cost.** Median and 95th-percentile frame time and capture throughput on this build against the release build,
same path. Any writer back-pressure or stall: `run_summary` `writer_backpressure_waits`, `writer_backpressure_seconds`,
`writer_stalled`, or the log line `Capture(m56): the frame writer stalled ...`?

**ALSO:** anything that looked wrong on screen or in the data that these questions did not ask about.
