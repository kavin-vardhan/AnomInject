# REPORT for 086-02-camera-clipping-label-fix.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 086-02-camera-clipping-label-fix.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 23:51:22 | finished: 2026-09-29 00:29:51 | duration: 38 min | exit code: 0
context at last turn: 648333 tokens (65% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\086-02-camera-clipping-label-fix.live.txt

---- FINAL MESSAGE (Claude Code) ----
I've replaced camera_clipping's per-frame label and built the B-CC camera-schedule lever. Both targets build clean, the new selftest passes 93/93, and everything is pushed. Nothing was launched, staged or cooked, so the new rule has no runtime or pixel evidence yet; B-CC is its first test.

```
VERDICT: GREEN

1. PLAIN SUMMARY FOR THE OWNER
camera_clipping now labels a frame only when something actually drawn on screen sits between the normal and the pushed near plane, inside the view, and it is decided with the camera of that same frame.
A bench tool now moves the camera through a fixed 200-frame path, so Tuesday's bench can check the labels against the pictures; nothing was run today.

2. BOOTSTRAP (two lines)
- Old label: a Visibility-collision sphere of radius GNearClippingPlane at the view origin, pawn ignored (AnomalyViewport.cpp:857-885). It asks "is collision near the lens", not "was rendered geometry cut out".
- AnomalyBench had one-shot placement, input lock and a CB scene fixture, but no per-frame camera schedule.

3. THE LABEL FIX: TEST, COST, LIMITS
Rule: a frame is positive iff a rendered primitive (pawn and its meshes included, FX excluded) has render geometry in the view slab [baseline near, anomalous near] inside the frustum.
- Test: world render bounds against the slab's bounding box first, then an exact separating-axis test of each object's rotated render box (and its world box) against the slab. Foliage and other instanced meshes are tested per instance.
- Which primitives count mirrors the renderer: scene proxy, IsVisible, owner not hidden, main pass, owner-see rules against the view target, MinDrawDistance.
- The slab is built from the camera cache the renderer uses, with the same projection maths. A per-camera near-plane override or an ortho camera gives an empty slab.
- Occlusion needs no trace: any geometry in the slab makes the first visible surface along that pixel's ray fall inside the slab.
- Timing, which I added beyond the brief: the old decision ran among the tickables (LevelTick.cpp:1606), before UpdateCameraManager (:1621), so it read the PREVIOUS frame's camera. That would show as a +1 desync on every edge of a moving-camera leg. It is now decided at OnWorldTickEnd. Other session globals and the sync path are unchanged.
- Old rule kept for comparison: frame key camera_clipping.sphere_proxy, plus camera_clipping.slab / .slab_primitives / .eye_inside_box / .near_overridden, on every frame while camera_clipping is held.
- run_summary gains camera_clipping_* keys (rule, counts, slab-only and proxy-only frames, cost, first hits) and one RUN SUMMARY log line. All other rows and summaries are byte-identical.
- Cost measured in the selftest: exact test ~60 ns per box, first-pass box check ~4 ns, slab build ~50 ns. The in-engine cost walks every primitive once per captured frame and is NOT measured (no launch); by arithmetic µs on CB_GateLevel, ~0.5 ms at 10k primitives. B-CC reads it from camera_clipping_eval_us_mean/_max.
- Limits, over-label: hollow meshes whose box surrounds the eye (sky-dome meshes, room shells, landscape tiles; counted by eye_inside_box); alpha-cut or translucent pixels in the slab; sub-pixel slivers at the frustum edge; off-centre and split-screen projections.
- Limits, under-label: vertex motion outside render bounds (WPO, wind, skin outside its physics bounds); FX systems excluded; primitives not owned by an actor in the world list and not level BSP; camera changes after UpdateCameraManager.
- G352 (read from source, not measured): the near plane is the start of every directional-light shadow cascade (DirectionalLightComponent.cpp:762). ON vs null will differ even on frames where nothing is clipped.

4. THE LEVER
- IAI.Bench.CameraSchedule cc_v1|off, in AnomalyBench. Registered only under -IAIBench; the module is compiled out of Shipping.
- To run it also needs -IAIBenchFixture, CB_GateLevel, InputLock and no capture active; it is refused by name otherwise.
- It spawns a wall (face at x=-1400), a colliding prop beside the lens path, a non-colliding prop, and a pawn-owned probe if a pawn is possessed. If none is, the pawn segment runs without it and logs pawn_mesh=0.
- It drives 200 poses keyed by the capture session frame index, applied at OnWorldPreActorTick:
  control 0-29 · wall 150 cm 30-39 · in 40-59 (clipped from k 50) · 50 cm 60-69 · out 70-89 (clipped to k 79) · 150 cm 90-99 · behind 100-119 · beside 120-139 · non-colliding prop 140-159 · pawn mesh 160-179 · control 180-199.
- No wall frame lands within 2.5 cm of the 100 cm plane.
- Logs: ARMED, SEGMENT, and one FRAME line per captured frame with requested vs actual pose.

5. UNIT TESTS, BOTH WAYS
- tools/camera_clipping_slab_selftest.cpp compiles the production header (cl /W4 /WX /O2): 93 checks, 0 failures.
- Each constructed case's truth comes from an independent 41³ brute-force point-sampling oracle.
- Legacy sphere model wrong on 9 of 17 cases; slab test wrong on 0.
  · It fails behind-camera (false positive) and non-colliding (false negative), where the slab test passes, as the brief asked.
  · It also fails beside, pawn mesh, first-person arms, hidden collider, closer-than-baseline, above-frustum and the rotated rod.
- The new test can fail too: a world-box-only narrow phase fails the rotated rod, and removing the frustum fails 5 cases.
- Random cross-check, 20,000 boxes: 0 cases where a sample point proves an intersection the test missed. 6 hits unconfirmed by sampling, all grazing contacts (≤0.366 cm overlap).

6. BUILD
- Warm host E:\IA_BuildCache\_r84_host. Editor 13 actions / 47 s, exit 0; Game 6 actions / 72 s, exit 0; 0 compiler warnings. Comment stripper: 0 changed of 118.
- exe SHA-256 E9FF019AC23EC0EC5D922EAB140AAF2CEAA235CFDFD3D7E7531B3A1F81409120, 242,042,368 B.
- A44 string scan (UTF-16): every new token present; controls PlaceView 1, TargetMask 6.
- Archived at _binary_baselines\m52fix-E9FF019A\ (exe, both logs, README; hash re-verified there). Code-only, pairs with container 67EA1FE0. NOT staged.

7. B-CC LEG SPEC (journal §8)
- Setup: E9FF019A + 67EA1FE0, CB_GateLevel 1280x720, fps 30, exposure pinned, delivery OFF, -Marker 0, focus gate ON, -IAIBench -IAIBenchFixture. B1 and -RequireModalRotZero declared NOT APPLICABLE.
- ExecCmds: IAI.Bench.InputLock 1 · IAI.Bench.CameraSchedule cc_v1 · IAI.Anomaly.CameraClipTriggerRadius 100000 · IAI.Capture.Config 2 4 8 14 0 · IAI.Capture.Start "" png 4242 200 camera_clipping StaticMeshActor_49 <near>
- <near> = 100 for ON and 10 for null; that is the only difference between a leg and its null.
- 7 legs: ON_NAT, NULL_A_NAT, NULL_B_NAT (delivered AA) · ON_AAOFF, NULL_AAOFF · ON_SYN, NULL_SYN (SynthTickOrder).
- Validity: 200 rows and 200 PNGs; FRAME lines with err ≤0.5 cm / 0.1°; 0 camera_clipping entries on null legs; identical segment maps; ≤3 invalid attempts per leg.
- Oracle: D_k = pixels whose max channel differs by >8/255, ON vs null at the same k.
  · Floor F = max of null-vs-null and ON-vs-null on the designed-negative segments (G352).
  · Signal P = min D over the wall_near and prop holds (plus pawn if present).
  · Threshold θ = geometric midpoint of F and P; the leg is UNJUDGEABLE if P ≤ 2F. Edges report strict / 10% / 50%.
- Gate:
  (1) Label == CLIPPED on every judged frame of the three ON legs at the 50% rule.
  (2) Can-fail: sphere_proxy must be wrong on behind, beside and prop (predicted 20+20 false positives, 20 false negatives; pawn too if present), or the oracle is blind.
  (3) Report where designed verdicts and the oracle disagree.
  (4) Cost keys and first_hits.
  (5) The 086-01 evaluator beside it.

8. COMMITS PUSHED (fix/m52-label-timing, ce84394..f51f124, local == origin)
- f908b71 fix(camera_clipping): view slab on render bounds, decided after the camera update; B-CC lever (+ selftest)
- f51f124 docs(camera_clipping): journal, status block, client-readme label definition, architecture, G352
No tag, no merge. m51 53bf725, master b5f15a3, ToCodex, E:\<personal-folder> and the m53 files untouched. Nothing staged or launched.

9. NEEDS-DECISION (none blocks 084-05b)
1. Hollow meshes around the eye. On a host with a sky-dome mesh this test may label EVERY frame positive, which is the failure m30 was meant to prevent. CB_GateLevel cannot show it: it uses SkyAtmosphere, not a dome mesh (the G135 pattern). I did not add a heuristic: dropping boxes that contain the eye would also drop the real case of the camera pushed into a wall. Recommend one extra MainWorld or Lyra ON/null pair on Tuesday to read camera_clipping_eye_inside_box_frames and first_hits before Thursday's decision.
2. FX exclusion. Particles are excluded because their bounds are often fixed and large (the G33 reason). The cost is that a sliced particle is never labelled. Override if you prefer the opposite trade.
```
api-equivalent cost (not billed on Max): $20.74 | turns: 162 | running total across all briefs: $3,198.73
