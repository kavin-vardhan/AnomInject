# O-001 — first groundwork on the real hosts

**From:** chat Claude (orchestrator), 2026-11-20. **To:** the office agent. **Answer as report cards** (`REPORT-FORMAT.md`): numbered answers `A1...`, at most 40 lines a card. Nothing identifying a title, and no host code, in any card.

**Why:** three changes are written at home and not yet proven anywhere: every captured frame is kept (m56), LOD pops get a spatial cause (m57), terrain blinks get their own subtype (m58). Four facts about the real hosts decide how we finish them. Answer for **every host you can run**, one set of cards per host. The plugin on the hosts is the delivered release; none of the new units is in it, and you do not need them for this brief. Scratch scripts are fine (in `C:\OfficeAgent\scratch`).

**Setup:**
- **The plugin in the game:** the game keeps the plugin version it already has (the delivered build); the owner copies nothing in for this brief. Read `Plugins\AnomalyInjector\COPIED_COMMIT.txt`. If it is missing, identify the version from the files and say how on the card, for example: the top status entry of the copy's `CLAUDE.md`, the newest journal in `docs\sessions\`, whether `Source\AnomalyInjector\Public\AnomalyTerrain.h` exists (it arrives with m58, so the delivered release does not have it). The `.uplugin` `VersionName` reads `0.9.0` on both the release and the m56–m59 commits, so it does not tell them apart. Put the version on line 1 of every card (`REPORT-FORMAT.md`).
- **Capture with delivery mode off:** type `IAI.Capture.Delivery 0` in the game console before each `IAI.Capture.Start` (ruling 096-02 R3). The readers of O-002 need each session's `run.json` and run log (`anomaly_log.txt`), which delivery mode does not write; this makes O-001's sessions comparable with O-002's. Delivery mode does not change the labels.

**Q1. What is the ground made of?** In two or three typical playable levels:
- (a) Is the ground the player stands on the engine's landscape system, ordinary static meshes, or both? Rough share of each.
- (b) For mesh ground: is there a naming or class pattern that picks out the ground pieces? **Do not report the pattern.** Report how many tokens it needs, how many ground pieces it matches out of how many, and how many non-ground objects it wrongly matches.
- (c) Roughly what share of the screen does one ground piece cover when it is hidden, at a normal camera height: under 10 %, 10 to 25 %, over 25 %?

**Q2. How much of the scenery can show a LOD pop?** In the same levels, for static mesh components within about 50 m of the player path:
- (a) the count; how many have two or more usable LODs (count only LODs the engine can draw: at or above the minimum LOD, with geometry);
- (b) how many are instanced or hierarchical-instanced, and how many are Nanite;
- (c) for those with two or more LODs: the quartiles of the screen size at which LOD 0 gives way to LOD 1;
- (d) how many of them cover at least 7 % of the screen at the moment of that switch when approached at walking speed. (Our distance-triggered pop needs that; our turn-in pop does not.)
- (e) how you counted: editor script, runtime walk, or reading assets.

**Q3. Does the game stall when a corrupted texture appears?** The client saw a frame jump only after the two texture-corruption anomalies (the one that turns a surface pink and the one that turns it checkered), never after other anomalies. Their games advance on the wall clock, so one long frame shows as a jump even when no frame is dropped.
- (a) With the delivered build, run one capture per anomaly with at least ten events of it, and one with a blink as the control, same level, same path, default settings (delivery mode off, as in Setup).
- (b) From `labels.jsonl`: the wall-time length of the frame where each event starts, of the two frames after it, and of the frame where it ends, against the run's median frame. First event of the process against later ones.
- (c) `MEASURED:` does applying either texture corruption stall the frame? By how many milliseconds? Only the first time, or every time? Does the blink do the same?
- (d) `INFERRED:` from our plugin's code, what causes it (our `file:line`).
- (e) Separately: `ticks_per_captured_frame` and `capture_game_ticks` from each run's `run_summary.json` (the delivered build has no `frames_dropped`; that field arrives with m56), and whether `frame_index` jumps by more than one anywhere, and by how much, at the start and end of events.

**Q4. The reviewed session.** The client's review of 9 September was of a session captured on this PC with the M2 build.
- (a) Does that session still exist here?
- (b) If yes: `frame_index` and `t_wall` for rows 674 to 677; which event starts at or near 675 and its anomaly type; whether the same jump recurs at every event start and end or only at some; `ticks_per_captured_frame`; for terrain events, whether `component_class` reads as landscape or as a mesh class.
- (c) If no: say so, and whether any other M2 session of the same host exists that shows the same pattern.

**ALSO:** anything on these hosts you think will break one of the three changes. You can read the hosts' code; chat cannot.
