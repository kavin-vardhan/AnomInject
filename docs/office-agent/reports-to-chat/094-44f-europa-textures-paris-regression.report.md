# REPORT for 094-44f-europa-textures-paris-regression.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-44f-europa-textures-paris-regression.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-04 12:02:46 | finished: 2026-11-04 12:36:23 | duration: 34 min | exit code: 0
context at last turn: 460939 tokens (46% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-44f-europa-textures-paris-regression.live.txt

---- FINAL MESSAGE (Claude Code) ----
**VERDICT: NEEDS-DECISION**

**For the owner:** Paris hasn't been tested yet with the textures on by default. The new brightness setting is correct, but on the first Paris test the game's camera pointed away from the test objects, so the test stopped before anything was scored. Nothing was pushed and the office card is unchanged.

**The EA_Core fix (GK2)**
- Codex re-read 094-44e's 65-exposure sweep under GK2 and chose **−5.0 EV**, the brightest exposure that passes; all 40 brighter values fail. My own reader of the same raw frames also gives −5.0 EV.
- At −5.0 EV, by Codex's readings:
  - No untouched target has a pixel at 255.
  - The untouched target means are 151.9, 152.4 and 101.6, i.e. 0.60, 0.60 and 0.40 of full scale.
  - EA_LOD's pink reads (245.9, 90.6, 232.0), which passes the hue check.
  - The checker gaps are 50.65, 49.73 and 21.09.
- The check that decides it is EA_LOD's pink hue. At −4.875 its B/G is 2.36, under the 2.5 floor.
- ⚠ EA_LOD's checker gap is only 1 level above the 20 floor on Codex's front-face patch. My whole-silhouette reading gives 40.2.
- The settings are bench-only and runtime-only, and no fill lights were used:
  - bloom off;
  - eye adaptation quality 2, method 3;
  - exposure −5;
  - physical-camera exposure off;
  - `r.CustomDepth` read as 1 and never set;
  - the game ran with `-nowrite`.
- The Paris host's Content, Config, plugin Content, descriptor and `.uproject` are hash-equal before and after. The launch reserve is 3 GiB. There was one FE3b child, UBT `-Mode=ValidatePlatforms`.

**The Paris regression: no numbers**
- **D3D11 1280×720:** launched at 12:17:42 IST. Its own preflight (3 previews, 24 frames, 0 drops) projected all three targets outside the frame: centres around (2556, 638), (2254, 1438) and (3019, 1272), against the sweep's (502, 436), (778, 436) and (640, 384).
  - I looked at both frames. The leg's frame is black apart from a floor wedge on the right; the sweep frame shows the three cubes centred.
  - The reader refused, so nothing fired, C5 never started, and there are no masks or annotation.
  - The two launch command lines differ only in their output paths. The cause of the different view isn't established.
- **D3D12 1280×720 and D3D12 1279×719:** not run.
- So there are no new H4, texture G1–G3, `unavailable` or G5 per-row numbers.
- **Defaults on local `983f429`:** blinking, lod_corruption, missing_texture and corrupted_texture. camera_clipping is untouched and still compiled out.

**Push and card:** nothing was pushed and the card wasn't updated. origin is still `8df75de` (checked with `ls-remote`). Local feat is now `983f429` (`786a168` and `983f429`: tools and docs only). Every commit after `8df75de` sits on `8e59a56`, which turns on the default routes, so none of them can go out on its own (GH4).

**Stop times:** no build ran. The last launch was at 12:17:42 IST and the game exited at 12:18:19. None of our processes were alive at 12:23 or later. Both the 13:10 and 13:35 limits were kept with a wide margin.

**Safety:** no real crash, by my reading.
- The kit's result counts "1 crash", but that was the kit stopping its own game after the reader refused. It recorded an empty `AssertionError` reason, and `runtime_exit.py:16` labels any empty-reason exit as a crash. The log has 0 fatal lines, and no new crash, WER or dump files appeared. The upload check passed.
- m51 matches P9 at start, prelaunch, exit and final.
- The fixture, both 4.25 materials and both 5.1 `.uasset`s are unchanged.
- Nothing was deleted.
- C: free space dropped 24.1 → 21.5 GiB during the game session. That drive isn't protected, and I haven't found the cause.

**Decisions chat must make:**
1. **The camera view.** Should the bench helper check that the targets are inside the frame before its previews, and retry or stop if not? Should it also lock the view explicitly, for example to the level's `EA_Camera`?
2. **The crash label.** Should `runtime_exit.py` stop counting an empty-reason owned stop as a crash? The fix would need its can-fail tests, and this run's receipt stays as recorded.
3. **The rerun after 15:15.** Run the three Paris legs with −5.0 EV kept, or recalibrate in run mode first?

Files are in `D:\IntrusiveAnomalies\_reviews\`:
- 094-44f-code-review.md
- 094-44f-codex-report.md
api-equivalent cost (not billed on Max): $9.85 | turns: 79 | running total across all briefs: $5,722.90
