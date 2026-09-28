# 086-02 — camera_clipping: the per-frame label becomes a view-slab test on render geometry; the B-CC camera-schedule lever

Date: 2026-09-29 (IST, the session started 2026-09-28 late). Brief `086-02` (Code, headless). Branch `fix/m52-label-timing`,
edited and built in place on the warm scratch host `E:\IA_BuildCache\_r84_host`. **Nothing was launched, staged or cooked.**
Inputs: `_reviews\086-01-label-sync-audit.md` §5.3 and the B-CC row of §7; ruling `_reviews\086-01-chat-ruling-audit.md`.

## 0. Summary

| item | result |
|---|---|
| Label rule | camera_clipping is positive on a frame iff a **rendered** primitive (pawn and its meshes included, FX excluded) has render geometry in the **view slab** between the baseline and the anomalous near plane, **inside the frustum** |
| Test | broad phase: world render bounds vs the slab AABB; narrow phase: exact SAT of the **oriented** render box (and the world AABB) against the truncated pyramid; ISM/HISM/foliage per instance |
| Timing | decided at `OnWorldTickEnd`, **after** `UpdateCameraManager`; the old decision read the previous frame's camera (G41 recurrence) |
| Old rule | kept as the diagnostic `camera_clipping.sphere_proxy` on every frame while camera_clipping is held |
| Cost | measured in the selftest: exact SAT **~60 ns** per box, AABB broad phase **~4 ns**, slab build **~50 ns**; in-engine enumeration **unmeasured** (no launch), instrumented in `run_summary` |
| Lever | `IAI.Bench.CameraSchedule cc_v1|off`, AnomalyBench, registered only under `-IAIBench`, compiled out of Shipping |
| Unit tests | `tools/camera_clipping_slab_selftest.cpp` **93/93** (`/W4 /WX`); legacy sphere model wrong on **9 of 17** constructed cases, slab test on **0** |
| Build | Editor exit 0 (13 actions, 47 s), Game exit 0 (6 actions, 72 s), **0 warnings**; exe **`E9FF019A`**, `_binary_baselines\m52fix-E9FF019A\` |
| Commit | source `f908b71`, pushed; no tag, no merge |

## 1. Bootstrap (the two lines)

- The label was `IsGeometryWithinNearClipRadius`: a Visibility-channel sphere overlap of radius `GNearClippingPlane` at the
  view origin with the pawn ignored. That answers "is there collision near the lens", not "was rendered geometry cut out".
- AnomalyBench had one-shot placement, input lock and a CB scene fixture only; there was no per-frame camera schedule.

## 2. The label fix, as built

**Where:** `FAnomaly_CameraClipping::IsCurrentlyAnomalous` → `AnomalyViewport::EvaluateNearClipSlab`; pure maths in
`Source/AnomalyInjector/Public/AnomalyNearClipSlab.h` (UE-free, the same header the selftest compiles).

1. **Camera.** The slab is built from `PlayerCameraManager->GetCameraCacheView()`, i.e. the POV the renderer uses: location,
   rotation, FOV, `bConstrainAspectRatio`/`AspectRatio`, the local player's `AspectRatioAxisConstraint`, the viewport size and
   `r.UseLegacyMaintainYFOVViewMatrix`, mirroring `FMinimalViewInfo::CalculateProjectionMatrixGivenView`
   (`CameraStackTypes.cpp`). A per-camera `PerspectiveNearClipPlane > 0` or an ortho camera makes the slab **empty**
   (`r.SetNearClipPlane` has no effect there), reported as `camera_clipping.near_overridden`.
2. **Slab.** Depth `[baseline, anomalous]` along the view axis, lateral `|x| ≤ tanH·d`, `|y| ≤ tanV·d`. Baseline = the near
   plane captured at `Apply`; anomalous = the argument (default 100). `anomalous ≤ baseline` ⇒ empty (the null leg).
3. **Which primitives.** Every `UPrimitiveComponent` of every actor, plus level BSP (`ULevel::ModelComponents`), that the main
   view would draw: registered, scene proxy present, `IsVisible()` (covers hidden-in-game), owner not hidden,
   `bRenderInMainPass` (so the m45 hide is excluded), not `bVisibleInSceneCaptureOnly`, owner-see rules evaluated against the
   view target exactly as `FPrimitiveSceneProxy::IsShown` (`PrimitiveSceneProxy.cpp:1249-1258`), outside `MinDrawDistance`.
   **The pawn is not ignored.** `UFXSystemComponent` is excluded (see limits).
4. **Test.** World `Bounds` AABB vs the slab AABB, then the exact separating-axis test (5 slab face axes, 3 box axes,
   6×3 edge crosses) of the **oriented** render box — `CalcBounds(FTransform::Identity)` × `BoundsScale` × the component
   transform — AND of the world AABB (a point in the geometry is in both boxes, so requiring both is still conservative
   and tighter than either). If the oriented box is implausible (zero local extent with a non-zero world extent, or its
   centre outside the world AABB), the world AABB alone is used and counted (`camera_clipping_box_fallbacks`).
   ISM/HISM/foliage: `GetInstancesOverlappingBox(slab AABB)` then one oriented box per instance from the mesh bounds.
5. **Occlusion does not matter, by construction.** If some geometry lies in the slab along a pixel ray, the first surface
   along that ray at depth ≥ baseline is at depth < anomalous (anything nearer than the baseline is already clipped at
   baseline), so that pixel changes. No trace is needed, and none is made.
6. **Timing (a G41 recurrence, fixed).** The async path appended the session-global fire in `FinalizeArmedLabel`, which
   runs among the tickables (`LevelTick.cpp:1606`) **before** `UpdateCameraManager` (`:1621`); `GetPlayerViewPoint` there
   returns the previous frame's camera cache. Now `FinalizeArmedLabel` skips camera_clipping (`bViewGlobalPending` on the
   snapshot) and `SampleDeferredActiveState` (at `OnWorldTickEnd`, `:1814`) decides it, appends the entry at the **end** of
   `Fires` and extends `FirePos` / `Trailing` / `RenderSettled` when they are populated. The session-global
   positive/negative counters move with it. Other session globals keep the old point; the **sync** path is unchanged (its
   picture is the previous frame, which matches the previous camera it reads). Nothing reads the pending snapshot between
   the two hooks (`OnWorldTickEndMask` does not touch snapshots).
7. **Cache.** The anomaly caches the evaluation per `GFrameCounter` + POV location/rotation/FOV + both planes, so every
   caller in the label path shares one evaluation per captured frame.
8. **Diagnostics (additive, only while camera_clipping is held; every other row and summary is byte-identical).**
   Frame keys: `camera_clipping.slab` (= the label), `camera_clipping.sphere_proxy` (the m30 rule, computed at the same
   pose), `camera_clipping.slab_primitives`, `camera_clipping.eye_inside_box` (hits whose box contains the eye — the
   hollow-mesh class), `camera_clipping.near_overridden` (only when true). `run_summary`: `camera_clipping_label_rule`
   (`view_slab_render_bounds_v1`), both planes, frames evaluated, slab / sphere-proxy positive frames, slab-only and
   proxy-only frames, eye-inside-box frames, override frames, box fallbacks, FX-excluded max, enumerated and candidate
   means, instances tested, eval ms total / µs mean / µs max, and the top 16 first-hit primitives with frame counts.
   One `Capture(camera_clipping) RUN SUMMARY` log line.

## 3. What the chosen test can still get wrong (stated for the B-CC oracle to judge)

**Over-label (label positive, nothing visibly cut):**
1. **Conservative bounds.** The oriented render box is a superset of the triangles. Hollow or concave meshes — a sky-dome
   mesh, a room shell, an arch, a landscape component whose height range spans the eye — can reach the slab with no
   triangle there. `eye_inside_box` counts the worst sub-class. CB_GateLevel has no sky mesh (SkyAtmosphere).
2. **Transparent pixels.** Masked or translucent geometry in the slab whose pixels are empty (alpha-cut foliage cards).
3. **A sliver at the frustum edge.** A primitive whose slab intersection is sub-pixel is labelled; its picture effect
   may be below any threshold (the ruling's 50 % rule judges that).
4. **Frustum edge approximations.** `OffCenterProjectionOffset`, split-screen view rects and engine forks with custom
   projections are not modelled.

**Under-label (visibly cut, label negative):**
5. **Geometry outside its render bounds.** WPO / vertex animation / wind beyond `BoundsScale`, skinned vertices outside
   the physics-asset bounds, displacement. The engine culls these wrongly too.
6. **FX systems are excluded** (particles, ribbons, beams at the lens). Chosen because FX bounds are often fixed and large
   (the G33 reason); the opposite choice would over-label.
7. **Primitives the enumeration does not reach:** components not owned by an actor in the world's actor list and not level
   BSP (world-owned line batchers, primitives with a non-actor outer). Deferred decals and fog are not primitives.
8. **A camera changed after `UpdateCameraManager`** (late camera updates, XR late update) is not seen.

**Not a label error, but it will show in ON-vs-null pixels (G352, source-read):** the near plane is the start of every
directional-light shadow cascade (`DirectionalLightComponent.cpp:762`), so pushing it shifts shadows across the whole frame
even when nothing is clipped. The oracle must take its floor from ON-vs-null on designed-negative frames.

## 4. Cost

- **Measured (selftest, `/O2`, this box, 2,000,000 calls on 4,096 random boxes):** exact SAT box-vs-slab **58.8–62.6 ns**,
  AABB broad phase **4.0–4.5 ns**, slab build **~50 ns**.
- **In-engine per captured frame (not measured; no launch):** one pass over every primitive component of every actor (flag
  reads + one AABB test each), `CalcBounds(Identity)` for broad-phase survivors, per-instance tests for ISM candidates. By
  arithmetic, a few µs on CB_GateLevel, tens of µs on MainWorld (432 actors), and ~0.5 ms for ~10,000 primitives.
  Only when camera_clipping is held. **B-CC reads the real figure** from `camera_clipping_eval_us_mean` / `_max` /
  `_primitives_enumerated_mean` / `_candidates_mean`.

## 5. The lever: `IAI.Bench.CameraSchedule`

- **Registration:** only when `-IAIBench` is on the command line; the AnomalyBench module is denylisted for Shipping.
- **`cc_v1` preconditions** (refused by name otherwise): `-IAIBenchFixture`, map CB_GateLevel, capture not active, the
  module's own `IAI.Bench.InputLock 1` held, not already armed, no `IAI.Bench.SceneFixture` configured, `StaticMeshActor_49`
  loaded (its Cube mesh and material are reused, as SceneFixture does), an active view and a view owner.
- **Fixture (transient, movable, cube mesh):**
  - wall — centre (-1395, 0, 260), scale (0.1, 12, 8), near face at x = -1400, Visibility-blocking;
  - `beside` prop — 20 cm cube at (-1700, -370, 260), Visibility-blocking;
  - `prop_noncolliding` — 20 cm cube at (-1750, 500, 260), NoCollision;
  - pawn probe — only if `PC->GetPawn()` is non-null: a 20 cm Visibility-blocking `UStaticMeshComponent` owned by the
    possessed pawn, absolute transform at (-1750, -700, 260). If no pawn is possessed, the `pawn_mesh` segment runs
    without it (designed negative) and the ARMED line says `pawn_mesh=0`.
- **Drive:** at `OnWorldPreActorTick`, pose(k) with k = the capture's `GetSessionFrameIndex()` (0 before capture) is applied
  like `IAI.Bench.PlaceView` (control rotation + view-owner teleport with the camera offset recorded at arm time). The frame
  armed in that tick gets index k (it is assigned and incremented at arm time), so the pose is deterministic per frame
  index in every leg. After the camera update, one `IAI-CC-SCHEDULE FRAME k=… seg=… designed=… want=… got=… err_cm=…
  err_deg=…` line per captured frame. Disarms (and destroys the fixture) at run end, on `off`, on input release, world
  cleanup and module shutdown.
- **Schedule (200 captured frames, camera height 260, pitch 0):**

| k | segment | pose | designed clip | old proxy (predicted) |
|---|---|---|---|---|
| 0–29 | control_start | (-1700, 0) yaw 180 | no | no |
| 30–39 | wall_far | 150 cm from the wall, yaw 0 | no | no |
| 40–59 | wall_in | d = 147.5 − 5j, yaw 0 | **yes for k 50–59** (d < 100) | same |
| 60–69 | wall_near | d = 50 | **yes** | yes |
| 70–89 | wall_out | d = 52.5 + 5j | **yes for k 70–79** | same |
| 90–99 | wall_far_2 | d = 150 | no | no |
| 100–119 | behind | wall 50 cm **behind**, yaw 180 | no | **yes (wrong)** |
| 120–139 | beside | (-1700, -300) yaw 180, collider 70 cm to the side | no | **yes (wrong)** |
| 140–159 | prop_noncolliding | (-1700, 500) yaw 180, prop at depth 50 | **yes** | **no (wrong)** |
| 160–179 | pawn_mesh | (-1700, -700) yaw 180, pawn probe at depth 50 | **yes if a pawn is possessed** | **no (wrong)** |
| 180–199 | control_end | (-1700, 0) yaw 180 | no | no |

No wall frame lands within 2.5 cm of the 100 cm plane.

## 6. Unit tests (`tools/camera_clipping_slab_selftest.cpp`, 93/93)

The selftest compiles the production header with plain `cl /std:c++17 /EHsc /O2 /W4 /WX` (recipe
`E:\IA_BuildCache\_r84_selftest\cc_build.bat`). Picture truth on the constructed cases comes from an independent
brute-force oracle (41³ samples of the box tested against the slab, plus 41³ samples of the slab tested against the box),
and each case's designed truth is asserted against that oracle first.

| case | truth | legacy sphere | new slab |
|---|---|---|---|
| wall face 150 in front | 0 | 0 | 0 |
| wall face 50 in front | 1 | 1 | 1 |
| wall face 97.5 (edge in) | 1 | 1 | 1 |
| wall face 102.5 (edge out) | 0 | 0 | 0 |
| **wall 50 behind** | 0 | **1** | 0 |
| **collider 70 beside** | 0 | **1** | 0 |
| **non-colliding prop at 50** | 1 | **0** | 1 |
| **pawn-owned mesh at 50** | 1 | **0** | 1 |
| **first-person arms (only-owner-see) at 40** | 1 | **0** | 1 |
| pawn body (owner-no-see) at 50 | 0 | 0 | 0 |
| **hidden collider at 50** | 0 | **1** | 0 |
| **box closer than the baseline plane** | 0 | **1** | 0 |
| **box above the vertical extent** | 0 | **1** | 0 |
| **rotated rod (AABB overlaps, OBB does not)** | 0 | **1** | 0 |
| FX system at 50 (excluded) | 0 | 0 | 0 |
| null leg (near = baseline) | 0 | 0 | 0 |
| camera yaw 180, wall 50 ahead | 1 | 1 | 1 |

- **Both ways, as the brief asks:** the legacy model fails the behind-camera and non-colliding cases, and the slab test
  passes them (explicit checks).
- **The new test can fail:** with a world-AABB-only narrow phase it gets the rotated rod wrong (1 disagreement); with the
  frustum removed it gets 5 wrong. Both mutations are caught.
- **Random cross-check:** 20,000 random oriented boxes. SAT hits 3,746; **0** cases where a sample point proves an
  intersection that SAT missed; 6 SAT hits unconfirmed at 61³ sampling, and their largest minimum SAT overlap is
  **0.366 cm** (grazing contacts thinner than the sampling pitch).
- Also covered: projection extents for MaintainX / MaintainY (current and legacy) / MajorAxis landscape and portrait /
  constrained aspect; primitive classification (owner-see both ways, hidden owner, main pass off, no proxy,
  scene-capture-only, FX, min draw distance); an empty slab for `anomalous ≤ baseline`.
- ⚠ The legacy model is a **model** of `OverlapAnyTestByChannel` (a ball vs boxes with a collision flag, pawn ignored),
  not the engine call.

## 7. Build and archive

- Warm host, edited in place: Editor `StackOBotEditor Win64 Development` exit 0, 13 actions, 47.1 s; Game `StackOBot Win64
  Development` exit 0, 6 actions, 72.4 s. **0 compiler warnings** in both logs (the only `warning` match is UHT's
  `-WarningsAsErrors` flag). Comment stripper: 0 changed of 118.
- Exe SHA-256 **`E9FF019AC23EC0EC…`**, 242,042,368 B, archived `D:\IntrusiveAnomalies\_binary_baselines\m52fix-E9FF019A\`
  (exe + both logs + README, hash re-verified at the archive). A44 (UTF-16): every new token present; controls
  `IAI.Bench.PlaceView` 1, `IAI.Capture.TargetMask` 6. Code-only; pairs with container `67EA1FE0`. **Not staged.**

## 8. B-CC leg spec (for 084-05b's harness)

**Binary and fixture:** exe `E9FF019A` + pre-m53 container `67EA1FE0`, CB_GateLevel, 1280×720 windowed, `VideoFps 30`,
exposure pinned (harness default), **delivery OFF** (labels.jsonl is needed), **marker OFF** (`-Marker 0`: the marker encodes
`GFrameCounter`, which differs between runs, G125), focus gate ON (G93). Command line adds **`-IAIBench -IAIBenchFixture`**.
**Pose gate B1 and `-RequireModalRotZero` are NOT APPLICABLE and must be off** (the camera moves by design; declared here,
before any leg).

**Recipe (ExecCmds, in order):**
1. `IAI.Bench.InputLock 1`
2. `IAI.Bench.CameraSchedule cc_v1` — check the `IAI-CC-SCHEDULE ARMED` line (and `pawn_mesh=`) before capture.
3. `IAI.Anomaly.CameraClipTriggerRadius 100000` (keeps the targeted form pushed along the whole path; max 1,000,000)
4. `IAI.Capture.Config 2 4 8 14 0`
5. `IAI.Capture.Start "" png 4242 200 camera_clipping StaticMeshActor_49 <near>` — targeted **session-global** form;
   **`<near>` = 100 for ON, 10 for null**. It is the only difference between an ON leg and its null.

**Legs (7, ~2 min each):**

| leg | near | order | AA | role |
|---|---|---|---|---|
| CC_ON_NAT | 100 | native | delivered | the judged leg |
| CC_NULL_A_NAT | 10 | native | delivered | matched null |
| CC_NULL_B_NAT | 10 | native | delivered | null-vs-null noise floor |
| CC_ON_AAOFF | 100 | native | AA-off arbiter recipe (G184 banked datum) | TAA-free edges |
| CC_NULL_AAOFF | 10 | native | AA-off | its null |
| CC_ON_SYN | 100 | `IAI.Bench.SynthTickOrder 1` | delivered | tick-order independence (standing rule) |
| CC_NULL_SYN | 10 | synth | delivered | its null |

**Validity (per leg, before any verdict):** exactly 200 rows with `session_index` 0..199 and 200 PNGs; one
`IAI-CC-SCHEDULE FRAME` line per k with `err_cm ≤ 0.5` and `err_deg ≤ 0.1`; the ARMED line present; `camera_clipping.slab`
present on every row; on null legs **0** rows carry a camera_clipping entry (the slab is empty); ON/null pairs have identical
k→segment maps and the same `pawn_mesh` value. A failure is INVALID (re-run up to 3 times), never FAILED.

**Pixel oracle (ON vs its null, at the same k):**
- `D_k` = pixels whose maximum channel difference exceeds 8/255 (the m29 strong-difference convention), whole frame.
- Floors: `N_k` = null A vs null B; `S_k` = ON vs null on the designed-negative segments `control_start`, `wall_far`,
  `wall_far_2`, `control_end` (G352: pushed-plane shadow shifts live here).
- Signal: `P` = the minimum `D_k` over the designed-positive holds `wall_near` and `prop_noncolliding` (plus `pawn_mesh`
  when `pawn_mesh=1`).
- Threshold `θ` = the geometric midpoint between `F = max(max N_k, max S_k)` and `P`. **The leg is UNJUDGEABLE if
  `P ≤ 2F`**, with both margins reported. Per frame: `CLIPPED = D_k > θ`.
- Edges: for frames next to a designed transition, also report `D_k` as a fraction of the adjacent steady segment's median
  (strict, 10 %, 50 %; the ruling's release threshold is the 50 % majority).

**Gate:**
1. The label (`camera_clipping.slab`, equivalently the presence of a camera_clipping entry) equals `CLIPPED` on every judged
   frame of CC_ON_NAT, CC_ON_AAOFF and CC_ON_SYN at the 50 % rule; strict and 10 % counts are reported beside it.
2. **Can-fail proof on the same pixels:** `camera_clipping.sphere_proxy` must DISAGREE with `CLIPPED` on `behind` and
   `beside` (predicted 20 + 20 false positives) and on `prop_noncolliding` (20 false negatives), plus `pawn_mesh` when a pawn
   is possessed. If the proxy agrees there, the oracle is blind to those cases and the gate is not evidence.
3. The designed verdicts (table in §5) and the oracle agree. A disagreement is reported, not re-labelled; it is where §3's
   limits would show.
4. Report the cost keys and `camera_clipping_first_hits` per leg.
5. Run the 086-01 evaluator beside it where it applies (whole-frame event, no mask).

## 9. Deviations from the brief, stated

- **Registration and execution gates.** The command registers only under `-IAIBench` as asked; it also needs
  `-IAIBenchFixture` to execute, like every other AnomalyBench fixture command (shared precondition code).
- **The pawn segment on CB_GateLevel.** The `-unattended` fixture may have no possessed pawn. The lever then runs the
  segment without the probe and says so; it never improvises a pawn. The probe is a stand-in for a player mesh (a
  pawn-owned, Visibility-blocking component), which is exactly what the old proxy ignored.
- **Scope added: the timing fix (§2.6).** The brief asked for the geometric test. The one-frame camera lag was found while
  wiring it, and a moving-camera oracle would read it as a +1 desync on every wall edge, so it was fixed in the same change.
  Other session globals and the sync path are unchanged.
- **The diagnostic key.** `camera_clipping.sphere_proxy` is a frame-level key on every frame while camera_clipping is held
  (not an entry key), because the comparison needs it on negative frames too, where there is no entry.

## 10. Hand-off

- 084-05b builds the harness for B-CC from §8. The Tue-night bench runs it on `E9FF019A` (staging is the bench's job).
- A Codex source review of 084-05a + 086-02 is next per the 084-05a hand-off.
- Nothing is merged or tagged; `m51`, `master`, `ToCodex\` and the m53 files were not touched.
