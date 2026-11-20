# REPORT for 095-08-plan-m57-lod-spatial-trigger.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-08-plan-m57-lod-spatial-trigger.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 08:14:06 | finished: 2026-11-14 08:39:50 | duration: 26 min | exit code: 0
context at last turn: 728741 tokens (73% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-08-plan-m57-lod-spatial-trigger.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner (3 lines, plain words)**
1. A LOD pop will only happen when the player moves. Either an object stays rough-looking as you walk toward it and snaps to full detail at a distance where you can see the change, or an object comes into view rough-looking as you turn and snaps to full detail about 4 frames later.
2. If the player stands still there are no LOD pops; other anomalies fill those bursts.
3. Before any testing you get six short clips from the LOD test level, each shown beside the same camera path with no anomaly: turn-in (two variants), walk-up, walk-up then back away, standing still, and today's timed pop for comparison.

**The plan in twelve lines**
1. **Mechanism: the plugin times the pop.** It holds a coarse LOD with `ForcedLodModel` and releases it on a trigger. In UE 5.1 the change lands in the same frame and never crossfades (checked in source).
2. **Why not let the engine time it:** 5.1 has no per-object setting that moves where the engine switches LOD. The only option, `SetBoundsScale`, also breaks culling, shadows and every label box.
3. **Distance trigger:** freeze a visible object at the coarse LOD it already shows, so arming changes nothing on screen. Release it when the camera crosses an on-screen-size threshold where the swap is clearly visible. On retreat it re-holds with 15 % hysteresis.
4. **View-entry trigger:** hold a coarse LOD on an object just outside the view, on the side the camera is turning toward. When it enters it shows the coarse LOD for N frames (default 4), then snaps. Objects whose shadow reaches the view are refused.
5. **Lifecycle:** arming reuses the existing deferred-onset pre-roll, with a 90-frame timeout for LOD. One new hook stops a burst from ending while an object is visibly wrong (cap 150 frames), so the schedule never causes a pop.
6. **Labels:** a frame is positive when the object is on screen, at the held LOD, the engine would draw a finer LOD for that exact view, and it covers at least 7 % of the screen (the m30 visibility threshold). A wrong but smaller frame gets a new drop flag, `lod_subvisible`.
7. **Swap frame and checking:** the snap frame is recorded. The engine's LOD is computed on the game thread with the engine's own function, the same way the 4.25 port's predictor does it, and re-checked per frame on the render thread. A disagreement is flagged `lod_unconfirmed`.
8. **Keys:** subtypes `distance_threshold`, `view_entry` and `periodic` (the old timer, by name and bench-only). Additive keys record the swap frames, the trigger distance and on-screen size, and the LODs.
9. **Scope of v1:** plain static meshes only. Skeletal, instanced (ISM), hierarchical-instanced (HISM) and Nanite meshes are refused by name, each for a reason checked in source.
10. **Yield:** about zero on StackOBot MainWorld (two faint rocks) and Lyra ShooterGym, so the bench needs the fixture. My unmeasured estimate for a game rich in multi-LOD props is about 3–6 LOD events per minute of movement, 0 when still. When nothing can arm, LOD drops out of the draw and no burst is wasted.
11. **Bench:** a scripted camera path `lod_v1` in the bench-only `AnomalyBench` module, on the existing `CB_LodFixture`, plus small copies of its two meshes spawned at run time. No new content and no new cook: it is a code-only swap onto 095-07's cook.
12. **Estimate:** 7 commits and 6 briefs. About 1 bench hour for the clips and 2.5 for the gates (34 legs, ~7.5 GB). All of it waits on 095-07 and on E: being freed.

The plan is at `D:\IntrusiveAnomalies\_reviews\095-08-plan-B-lod-spatial-trigger.md`.

**Two problems found in today's timer mode** (read from source, never measured):
- **Skeletal meshes:** a forced LOD set by the plugin reaches the picture one frame late, so its labels may lead the pixels by a frame. Our only skeletal bench target has one LOD, so this has never been exercised.
- **HISM components outside foliage:** HISM ignores forced LOD, so the timer would label "popped" frames on which nothing changes. The plan has `periodic` refuse HISM (one line).

**Questions for chat (my recommendation first)**
1. Mechanism: plugin-timed forced LOD, not `SetBoundsScale`.
2. v1 covers plain static meshes only; skeletal and ISM come later. Yes.
3. An armed object that never triggers: write no event and list its id in the run summary (`lod_untriggered_event_ids`, the vetoed-event pattern), not a `manifested:false` event.
4. An event with no visible snap (cap reached, or the player turned away first): handle it the same way as question 3.
5. Wrong-but-small frames: add the new `lod_subvisible` flag, rather than restricting arming (no new flag, but far fewer events).
6. The down-pop on retreat: on by default, since it is in B1.
7. `periodic` mode: bench gate only.
8. The 200 cm proximity gate and the highest-LOD gate apply to `periodic` only.
9. Camera path in the `AnomalyBench` module rather than the CaptureBench plugin the brief named. `AnomalyBench` already holds `cc_v1` and is stripped from every delivery; CaptureBench has no remote.
10. Labels from the game thread at tick end, cross-checked by the render thread, rather than m52-style render-record labels.
11. Equal draw weight for LOD in v1.
12. `label_schema_minor` 3.

**Questions for the owner or the client (one line each)**
- Does the client's reference LOD clip show an approach pop, a turn pop or both, and for how many frames is the low detail visible?
- Are most static meshes in their game Nanite? If so, `lod_popping` will yield almost nothing there.
- Do they want LOD pops on characters, or on props only?
- In their capture sessions does the player mostly move, or stand and watch?
- Do they want the down-pop on retreat in the dataset?

**Safety:** none. All repos were read only; the primary checkout stayed on `integ/m56-m58` `9bf6c7e`. Nothing was built or launched, and no process was touched. One read-only sub-reviewer read the engine source; I re-read the claims the plan depends on, and the plan marks them [V]. I wrote only the plan file and the receipts folder `code-receipts-095-08\`.
api-equivalent cost (not billed on Max): $20.62 | turns: 62 | running total across all briefs: $6,202.73
