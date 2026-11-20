# 095-08 — Plan B: `lod_popping` gets a spatial trigger (m57, `feat/m57-lod-spatial-trigger`)

**Author:** Claude Code (Opus 5.5, xhigh), headless, 2026-11-14. **Status:** PLAN ONLY. Nothing built, run, edited or committed.
**Base:** `integ/m56-m58` = `9bf6c7e` (master + m58 + m56). Every plugin `file:line` below is on that ref.
**Binding inputs:** `_reviews\095-00-ruling.md` §3 (B1–B6) and §6; `_reviews\095-02-plan-A1-capture-continuity.md` §1;
`_reviews\095-06-ruling.md`. After m56 every engine tick in a run is a captured frame; this plan is written on that base.

**Marks.** **[V]** = I re-read it in source on `integ/m56-m58`, `feat/europa-port` (git objects) or UE 5.1.1.
**[R]** = reported by a read-only sub-reviewer or an older record, not re-read by me.

**Short names.** `LP` = `Source/AnomalyInjector/Private/Anomalies/Anomaly_LodPopping.cpp`. `LOD` = `Source/AnomalyInjector/Private/AnomalyLod.cpp`.
`VP` = `Source/AnomalyInjector/Private/AnomalyViewport.cpp`. `AUTO` = `Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp`.
`SUB` = `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`. `SYNC` = `Source/AnomalyInjector/Public/AnomalyLabelSync.h`.
`BENCH` = `Source/AnomalyBench/Private/AnomalyBenchModule.cpp`. `UE` = `D:\UESource\UnrealEngine\Engine\Source`.

Receipts: `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-08\`
(`engine-facts-095-08.md`, `europa-port-lod-hunks.diff`, `europa-port-diff-touched-files.txt`, `receipts.md`).

---

## 0. Summary

- **Mechanism (recommended): plugin-timed.** The plugin holds a coarse LOD with `ForcedLodModel` while the target is
  armed, and releases it on a spatial trigger. A forced-LOD change recreates the proxy inside the same frame's render and
  never crossfades [V]. The engine has no safe per-component lever that moves its own switch points in 5.1 [R].
- **Two triggers.**
  - `distance_threshold`: freeze the target at the coarse LOD it already shows (no visible change when armed). As the
    camera approaches, the frozen LOD becomes wrong. Release when the camera crosses a screen-size threshold where the swap
    is clearly visible. Re-hold on retreat, with hysteresis.
  - `view_entry`: hold a coarse LOD on a target just outside the view, on the side the camera is turning toward. When it
    enters the view it shows the coarse LOD for a few frames (default 4), then snaps.
- **Labels.** A frame is positive when the target is on screen, shown at the held LOD, the engine would draw a finer LOD
  for that exact view, and the target is large enough on screen for the difference to show (the m30 7 % gate). A wrong but
  too-small frame carries a new transition-only reason `lod_subvisible`. The swap frame is the first negative; it is
  recorded.
- **Lifecycle on the burst machine.** Arming reuses the deferred-onset pre-roll (captured negatives, then a timeout). A new
  hook stops the burst from ending while a target is visibly wrong, so every pop is caused by the camera, never by the
  schedule. No movement means no event.
- **Old timer.** Kept by name as mode `periodic`, out of the default pool.
- **Bench.** A scripted camera path in the bench-only `AnomalyBench` module, on the existing `CB_LodFixture`, with runtime
  copies of its two fixture meshes. Code-only on top of 095-07's cook. Look-first clips before any gate.
- **Scope of v1.** Plain static mesh components only. Skeletal, ISM and HISM are refused by name (each for a measured
  reason, §10).

---

## 1. How `lod_popping` works today (on `integ/m56-m58`)

### 1.1 The anomaly [V]
- Class: `Anomaly_LodPopping.h:9-41`. Id `lod_popping`; usage `<substring> [half_period_frames]`; `IsCurrentlyAnomalous()` =
  `bActive && bPoppedPhase` (`:20`). Defaults: half-period 8 frames (`:36`), clamp 1..600 (`:37-38`), coverage minimum 7.0 %
  (`:40`).
- **Apply** (`LP:10-236`):
  1. Half-period: arg 2, else `AnomalyDefaults::GetHalfPeriodFrames` (ini key `LodPoppingHalfPeriodFramesDefault`, console
     `IAI.Anomaly.LodHalfPeriod`) (`LP:35-59`).
  2. Targets: every static or skinned mesh component matching the substring (`AnomalyLod::ResolveLodComponents`,
     `LOD:11-30`); the optional viewport-scoping filter (`LP:63-71`).
  3. Gates (auto-pool selection only; a targeted fire bypasses all but the first), per component:
     - multi-LOD: runtime LOD count ≥ 2 (`LP:114-121`, `LOD:32-49`);
     - proximity: `poll_distance_cm` ≤ `LodPoppingMaxDistanceCm` (compiled **200 cm**; metric = sphere-approx bounds distance
       from the **player pawn**, `VP:950-985`, `VP:156-169`) (`LP:139-163`);
     - coverage: bounds-projected screen coverage ≥ 7.0 % at pick time (`VP:1526-1563`) (`LP:165-172`);
     - highest LOD: the predicted current LOD must be 0 (`LP:174-197`, `LOD:76-141`).
  4. Each qualifying component gets `BaselineLod` = its current `ForcedLodModel` (normally 0 = auto) and `PoppedLod` = its LOD
     count, i.e. the 1-based forced value of the coarsest LOD (G408) (`LP:209-213`).
- **Tick** (`LP:238-263`): counts ticks; every `HalfPeriodFrames` it flips `bPoppedPhase` and writes `ForcedLodModel` =
  popped or baseline on every target. It starts in the baseline phase, so the first pop lands 8 ticks after Apply.
- **Revert** (`LP:265-279`): writes `BaselineLod` back on every live target.
- **LOD helpers** (`LOD:51-74`): static → `UStaticMeshComponent::SetForcedLodModel`, skinned → `USkinnedMeshComponent::SetForcedLOD`.
  `GetCurrentLod` (`LOD:76-141`) predicts the static LOD from `RenderData->ScreenSize[]` and the plugin's own projection
  (`VP:987-996`, `VP:196-206`); it ignores `r.StaticMeshLODDistanceScale`, `View.LODDistanceFactor`, `MinLOD` and LOD streaming.

### 1.2 Selection [V]
- Pool: `AUTO:35-46`; default-enabled set `AUTO:50-56` = `blinking`, `missing_texture`, `corrupted_texture`, **`lod_popping`**.
- `TryFireOnce` (`AUTO:257-620`): eligible ids → `GetVisibleRenderableActors` (`AUTO:358`, body `VP:1565-1592`: frustum,
  poll radius, ≥ 1 of 9 occlusion rays clear `VP:234-269`, coverage cull) → seeded draw `TexCorruptPure::DrawAttempt`
  (`TexCorruptPure.h:1533-1560`): id first, then target among that id's candidates, then hold. **If the drawn id has no
  candidate, the burst fires nothing** (`EDrawOutcome::NoCandidates`, `AUTO:533-567`).
- Apply runs under `SetAutoPoolSelection(true)` with the single arg `=TargetName` (`AUTO:573-592`), so an auto-pool fire
  always takes the default half-period.
- Catalog spec exposes one arg `half_period_frames` default 8 (`Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:176-180`).

### 1.3 Label membership and mask [V]
- `lod_popping` is `AnomalyState` (`Source/AnomalyCapture/Private/AnomalyActiveSource.h:26`). Membership for that policy is
  **the active bit alone, with no on-screen term** (`SYNC:96-99`).
- Per frame the bit is sampled at `OnWorldTickEnd` (`SampleDeferredActiveState`, `SUB:6294-6447`): `ComputeFireActive`
  (`SUB:8904-8922`) and `IsFireLabelledThisFrame` (`SUB:6449-6473`) both call `IsAnomalyCurrentlyAnomalous`.
- Readme semantics today: entry present on every burst frame; labelled on the popped frames; "active but not labelled" on the
  un-forced phases (`docs/client-readme.md:1002`, `:1285`).
- Observability (m49): `observable` = the target drew ≥ 1 masked pixel on that frame; `affected_frames` is the observable
  subset of `injected_frames` [R, m49 record].
- Masks: armed only on frames labelled for the event (m44 rule) [R]. m26 veto arms: the first ticks after the record is
  created, **unless** `DoesAnomalyHaveDeferredOnset` is true, in which case arms wait for labelled frames
  (`SUB:1621`, `SUB:1636`; `Source/AnomalyCapture/Private/AnomalyMaskMeasure.cpp:475`) [V].
- Event subtype: `MapAnomalyToClient` gives `lod_popping`/`lod_popping` (`SUB:677-689`). A per-event override exists:
  `Ev->Subtype`, filled from an anomaly's telemetry (texcorrupt's `texcorrupt.mode`, `SUB:10359-10370`) and written over the
  default (`SUB:10543-10546`).

### 1.4 How the capture burst drives it [V]
- Phase switch `SUB:1140-1275`: `LeadIn` → `BeginFire` (`SUB:5445-5497`, which calls `TryFireOnce` or `TryFireSpecific`) →
  `SettleAfterFire` (2 captured frames, flagged `settle`, m56) → `Positives` (8 frames; with a deferred-onset fire it first
  captures a pre-roll until the fire is labelled, timeout `DeferredOnsetTimeoutFrames` = 30, `SUB:1184-1217`,
  `SUB:5263-5320`) → `BeginRevert` (`SUB:5499-5525`, reverts every live fire) → `SettleAfterRevert` (2, flagged) → `PostGap` (4)
  → `BeginFire` … At defaults a cycle is 16 ticks = 16 frames.
- With the timer at half-period 8, Apply at tick T, first pop at T+8: of the 10 frames after Apply (2 settle + 8 positives),
  the popped ones are roughly the last 3 before the revert. The pop has no spatial cause: the client's complaint.

### 1.5 Evidence on record [R]
- m30: on `CB_LodCalib` the forced-worst vs proper difference was 12,489 px at 9.35 % bounds coverage and 14 px at 3.90 %;
  the 7.0 % gate sits between (journal 048). On MainWorld, 8 draws → 2 survived; refusals were mostly single-LOD content.
- 052: the 200 cm proximity gate gave 0 `lod_popping` fires on the bench.
- 084-08b / 089-03: `CB_LodFixture` gate legs PASS 23/23 at +0/+0, both orders, AA on and off, 0 fade edges on the dithered
  variant (static camera).
- G149: on the MainWorld rock, LOD 1 vs LOD 4 differ by ~0.4 % of the silhouette.
- Client readme today (`:1392`): "on ordinary scenery the pop is too faint to find in the pixels".

---

## 2. How Unreal 5.1 picks a LOD, and the two ways to cause a pop

### 2.1 Static meshes
- **Formula** [V `UE/Runtime/Engine/Private/SceneManagement.cpp:608-630`]: walk from the coarsest LOD; LOD i is used when
  `(ScreenSize[i] × scale × LODDistanceFactor × 0.5)² > ScreenRadius²`. The result is clamped up to `MinLOD`.
  - Main pass scale = `r.StaticMeshLODDistanceScale × View.LODDistanceFactor` [V `SceneVisibility.cpp:2122`].
  - `ScreenSize[i]` = the mesh asset's per-platform screen size (`RenderData->ScreenSize[i].GetValue()`) [R].
  - `MinLOD` / `bOverrideMinLOD` / `ClampedMinLOD`: baked into the proxy at construction; no public setter on the
    component [R].
  - LOD streaming: the result is clamped to the first resident LOD (`ClampToFirstLOD(CurFirstLODIdx)`) [R].
  - `r.ForceLOD` (non-Shipping/Test only) overrides everything when the LOD show flag is on [V `SceneVisibility.cpp:2120`].
  - Dithered transition: only for non-movable proxies with a dither material [V `StaticMeshRender.cpp:700-701`]; it blends
    the LODs chosen from two camera positions `lod.TemporalLag` = 0.5 s apart [V `SceneManagement.cpp:21-25`].
- **`ComputeStaticMeshLOD` and `ComputeTemporalStaticMeshLOD` are `ENGINE_API`** [V `SceneManagement.h:3239`, `:3250`].
- The per-view drawn LOD lives in `FViewInfo::PrimitivesLODMask`, a private Renderer class [V `SceneRendering.h:1212`].
- **Forced LOD**: `SetForcedLodModel` marks render state dirty [V `StaticMeshComponent.cpp:2270-2277`]; the proxy is
  recreated in `SendAllEndOfFrameUpdates`, which runs inside `BeginRenderingViewFamilies` [V `SceneRendering.cpp:4528`], so
  the change is in the same frame's render. The forced proxy submits one LOD and disables dithering
  [V `StaticMeshRender.cpp:1165-1167`, `:1068-1070`]. A return to auto has no per-primitive LOD history [R]; it can only
  dither if the camera itself straddles a LOD threshold (G372's source reading agrees).
- **ISM**: forced LOD applies to the whole component; auto LOD is chosen once from combined bounds [R].
  **HISM ignores `ForcedLodModel`** [V: zero references in `HierarchicalInstancedStaticMesh.cpp`].
- **Nanite** meshes get a Nanite proxy; discrete LODs and forced LOD do not apply [R].

### 2.2 Skeletal meshes
- `SetForcedLOD` does **not** mark render state dirty [V `SkinnedMeshComponent.cpp:3393-3403`]. The value is used by
  `UpdateLODStatus_Internal` in the component's own tick (`TG_PrePhysics`) [V `:3544-3546`]. Set from the plugin's tickable
  (which runs after actor ticks), it reaches the render **one frame later**.
- Auto LOD: `MeshObject->MinDesiredLODLevel` is double-buffered by the render thread [V `SkeletalRender.cpp:145-154`]; the
  LOD drawn in frame N was chosen from frame N-2's view or older [R]. Leader pose and `bSyncAttachParentLOD` copy another
  component's LOD [R].

### 2.3 The two ways to cause a pop

| | (i) Plugin-timed: hold a forced LOD, release on the plugin's trigger | (ii) Engine-timed: move where the engine switches |
|---|---|---|
| Public API, no host asset touched | Yes: `SetForcedLodModel` (static), `SetForcedLOD` (skeletal) [V] | **Only `UPrimitiveComponent::SetBoundsScale`** [R]. No per-component LOD scale, bias or distance factor exists [R]. Editing `ScreenSize[]` dirties the shared mesh asset (forbidden). Global cvars change every mesh at once (unlabelled pops everywhere). |
| Clean revert | Yes: write the saved `ForcedLodModel` back (today's revert). | Bounds scale reverts, but it also changes frustum and occlusion culling, shadow culling, texture streaming, and every plugin box and coverage figure (all computed from `Bounds`) [R]. A shrunk bound can make a visible object vanish at a screen edge: a second, unlabelled anomaly. |
| Swap frame measurable from the render side | Static: deterministic (proxy recreated the same frame [V]); cross-checked per captured frame by a render-thread record (§5.4). Skeletal: one frame late unless the plugin also re-runs the component's LOD update. | Static: the main-pass choice is private (`PrimitivesLODMask`); dither materials crossfade over up to `lod.TemporalLag` = 0.5 s, so the "swap frame" is a span. Skeletal: N-2 lag. |
| 4.25 | `ForcedLodModel` / `SetForcedLOD` exist [R, used by the 4.25 `lod_corruption`]. | `BoundsScale` exists [R]; same side effects. |

**Recommendation: (i), plugin-timed, for static meshes.** It is the only route with a clean revert, an instantaneous swap
and a known swap frame. The *timing* still follows the engine: every threshold is computed with the engine's own LOD
function for the exact view (§5.1), so a distance pop lands where an engine with shifted thresholds would pop.
No split: (ii) is rejected for both triggers.

---

## 3. The two triggers

Common terms (all computed at `OnWorldTickEnd` for the view the frame is rendered with, §5.1):
- `S` = the engine's LOD screen measure for the target (the quantity `ComputeStaticMeshLOD` compares).
- `L_eng` = the LOD the engine would draw now if the target were not held. `L_h` = the held LOD (0-based).
- `C` = bounds coverage of the target's on-screen part (the m30 quantity; clamped to the screen). `C_vis` = 7.0 % (the m30
  calibration; ini `LodPoppingMinCoveragePct`, console `IAI.Anomaly.LodMinCoverage`, both exist).
- Contrast test: `L_h − L_eng ≥ 1` and `tris(L_h) ≤ tris(L_eng) / R_tri` (`R_tri` default 4; triangle counts from
  `FStaticMeshRenderData::LODResources[i]`).
- "Moving": the view origin's closing speed toward the target (distance trigger) or yaw rate (view entry), averaged over the
  last 8 ticks from a small ring of tick-end view samples the anomaly keeps itself.

### 3.1 Distance crossing (`distance_threshold`)
- **Metric: the engine's screen measure `S`**, not raw distance. It is what makes UE switch LOD, and it folds in object size,
  FOV and the LOD scale. Camera distance (to the bounds sphere) is recorded beside it for the client.
- **Arm** (candidate rules, all at once):
  1. In view (the existing visible set), plain static mesh, ≥ 2 LODs, not Nanite, not excluded by pattern.
  2. **Freeze is invisible:** `L_h = L_eng` now, `L_h ≥ 1`, and both temporal LOD samples agree (not mid-dither).
  3. Approaching: closing speed ≥ `v_min` (default 50 cm/s) and the target within 30° of the motion direction.
  4. A release threshold exists: the smallest `S_rel` where the engine would draw a LOD passing the contrast test **and**
     `C ≥ C_vis`, times `(1 + m)` (`m` default 0.10 = "clearly visible").
  5. Just ahead: at the current closing speed, predicted frames until `S ≥ S_rel` ≤ `T_ahead` (default 90 frames).
- **What the client sees:** the object keeps its coarse LOD as the camera approaches. Once the engine would have switched,
  the frame is wrong; while `C < C_vis` it is flagged `lod_subvisible`; once `C ≥ C_vis` it is positive. At `S_rel` the
  hold is released and the object snaps to the engine's LOD.
- **Hysteresis (the reverse on retreat):** after a release, if the camera moves away until `S < h × S_rel` (`h` default
  0.85) **and** the engine still draws a finer LOD than `L_h`, the hold is re-applied: a down-pop at a distance crossing,
  then positive frames until the next up-pop or until the engine's own LOD coarsens back to `L_h` (then the hold matches and
  is released invisibly). No flicker: the up and down crossings are 15 % apart in `S`.
- **Why not "hold the worst LOD from far away"**: forcing a coarser LOD than the engine shows on a visible target is itself a
  pop with no cause. The freeze rule makes arming invisible by construction.

### 3.2 View entry (`view_entry`)
- **Candidate:** a plain static multi-LOD mesh, not Nanite, **outside the frustum** but within `θ_margin` (default 20°) of the
  frustum edge on the side the camera is yawing toward; yaw rate ≥ `ω_min` (default 30°/s); predicted entry ≤ `T_ahead`
  frames; and, evaluated at its current distance as if in view: `C ≥ C_vis` and the contrast test passes for
  `L_h` = its coarsest LOD (or the coarsest LOD that passes the contrast test).
- **Prepared while unseen:** the hold is set at Apply. Off-screen frames are not labelled (the label needs on-screen).
- **Shadows and lighting:** a held LOD also changes the object's shadow and its indirect light. An off-screen object can cast
  a shadow into view, which would be an unlabelled visible change. Rule: refuse a candidate whose bounds, swept along the
  main directional light's direction for the shadow distance, intersect the frustum (`lod_shadow_in_view`). Indirect light
  (Lumen) is expected to be below the noise; the bench measures it (gate L4b).
- **Entry detection:** per frame, the existing visibility tests (frustum + ≥ 1 of 9 rays clear, `VP:228-269`). The first
  on-screen frame with `C ≥ C_vis` is the first positive; partly entered frames with `C < C_vis` are flagged `lod_subvisible`.
- **How long the wrong LOD shows:** `N_entry` captured frames counted from the first positive frame (default **4**, console
  `IAI.Anomaly.LodEntryFrames`, ini `LodPoppingEntryFrames`, range 1..30). Then release; the next frame is the swap. The
  owner picks the number from the clips (§8.3).
- **Never seen:** if the target does not enter before the arm timeout, the hold is released while it is still off-screen:
  invisible, no label, no event (B2).
- **Turned away before the snap:** release while unseen; the event had visible wrong frames but no visible swap (§4.3).

---

## 4. Lifecycle, mapped onto the burst state machine

### 4.1 States (inside the anomaly)
| State | Hold | Frame label | Leaves by |
|---|---|---|---|
| CANDIDATE | none | — | chosen at `BeginFire` |
| ARMED | set, invisible (held = engine LOD, or off-screen) | negative (entry present, not labelled) | becomes wrong on screen → LABELLED; arm timeout → TIMED_OUT |
| LABELLED | set, visibly wrong | positive (`C ≥ C_vis`) or `lod_subvisible` (`C < C_vis`) | trigger → SWAPPED; wrong becomes right again (retreat) or leaves view → ARMED; cap → CAPPED |
| SWAPPED | released | negative | retreat past hysteresis with engine finer than `L_h` → re-hold → LABELLED (distance only) |
| RELEASED | released at the burst's revert (always invisible by construction) | — | — |
| TIMED_OUT | released invisibly | — | no event (B2) |
| CAPPED | released at the cap (the one non-spatial pop) | — | event dropped (§4.3, Question 4) |

### 4.2 Mapping onto the capture
- **`BeginFire` → `TryFireOnce`.** `lod_popping` is eligible only if at least one candidate can be armed **now** (a new
  pre-check beside the partner exclusion, `AUTO:301-319`). Otherwise it is excluded for this round with reason
  `lod_not_armable:<why>` (counted through `NoteYieldRound`), and the draw picks another enabled id. No burst is wasted.
- **Candidates** come from a new provider (`AnomalyLodTrigger::CollectCandidates`), not from the visible set, because view-entry
  candidates are off-screen. They are sorted by name and drawn with the same seeded stream, so the draw protocol is unchanged.
- **`SettleAfterFire`**: the 2 frames are flagged `settle` exactly as m56 does. Arming changed nothing visible.
- **`Positives` with deferred onset.** `HasDeferredOnset()` returns true in the spatial modes. The capture already handles it
  (`SUB:1184-1217`): pre-roll frames are captured as negatives until the fire is labelled; then the 8-frame window starts.
  - New hook `IAnomaly::GetDeferredOnsetTimeoutFrames()` (default −1 = the global 30). `lod_popping` returns
    `LodPoppingArmTimeoutFrames` (default **90** = 3 s at 30 fps). `stuck_low_mip` is unchanged.
  - Timeout → `NoteDeferredOnsetTimeout` → `BeginRevert` → invisible release → TIMED_OUT. The counter and log line become
    per-anomaly (today they say `stuck_mip_hold_timeouts`, `SUB:5295-5320`).
  - Side effect wanted: the m26 veto arms wait for labelled frames [V `AnomalyMaskMeasure.cpp:475`], so an armed off-screen
    target is never measured as zero and vetoed.
- **Positives cannot end on a visibly wrong target.** New hook `IAnomaly::IsHoldingEventOpen()`: true while LABELLED. When the
  8-frame window runs out, the phase keeps capturing (one frame per tick, counted `lod_window_extension_frames`) until no
  live fire holds the event open, or until `LodPoppingHoldCapFrames` (default **150**). This is the only change to the burst
  machine; every other phase is untouched.
- **`BeginRevert`**: `RevertAllLiveFires` → the anomaly writes back the saved `ForcedLodModel` on any target still held. By the
  rule above that release is invisible, except at the cap.
- **`SettleAfterRevert`, `PostGap`**: unchanged (flagged `settle`, then clean frames).
- **Frame cap and run end:** the cap check (`SUB:1125-1138`) wins over the extension; `FinishRun` reverts after the last frame,
  so nothing is seen. An event cut by the cap is TRUNCATED (A50 addendum).
- **Target destroyed while armed:** `WantsTargetLostNotification()` → `OnTargetLost` releases and ends the event (TIMED_OUT if
  never labelled; otherwise as §4.3).
- **What the capture does while armed:** it captures every tick (m56). Those frames are negatives with an unlabelled
  `lod_popping` entry (today's "active but not labelled" row shape), or `lod_subvisible` frames.

### 4.3 What reaches `annotation.json`
- **TIMED_OUT (never visibly wrong):** no event. Recommended (Question 3): write **no** event and list the id in a new
  `run_summary.lod_untriggered_event_ids`, the S2 pattern; readers accept its orphan `settle` entries by id.
- **No visible swap (CAPPED, or turned away before the snap):** recommended (Question 4): the same treatment, listed in
  `lod_unswapped_event_ids` with the reason. B5 asks for visible contrast **at the moment of the pop**; these events have no
  pop on screen. Their already-written `labels.jsonl` rows keep their entries (L3 precedent); the id lists name them.
- **Normal:** one event per fire; `frame_indices` = labelled frames; the swap frame is recorded (§5.3). A distance event can
  contain more than one pop (hysteresis); each swap is listed.

---

## 5. Labels

### 5.1 The per-frame rule (sampled at `OnWorldTickEnd`, order-independent)
- **Positive** iff: target held **and** on screen (frustum + ≥ 1 of 9 rays clear) **and** `L_eng < L_h` for this view **and**
  `C ≥ C_vis`.
- **Transition-only, reason `lod_subvisible`** iff: held, on screen, `L_eng < L_h`, `C < C_vis`. Excluded from positives and
  negatives by the client's existing rule.
- **Negative** otherwise (off-screen while held; held LOD equals the engine's; released).
- `IsCurrentlyAnomalous()` implements the positive rule; the result is cached per `GFrameCounter` because the capture asks
  several times per tick. The policy stays `AnomalyState`; the on-screen term lives in the anomaly, so `SYNC:96-99` does not
  change for any other anomaly.
- **Engine-exact `L_eng` on the game thread:** a new `AnomalyLod::PredictEngineLod(World, Component)` builds the frame's
  `FSceneView` with `ULocalPlayer::CalcSceneView` and calls `ComputeStaticMeshLOD` with `FactorScale` =
  `r.StaticMeshLODDistanceScale`, `MinLOD` from the component or mesh, and the first-resident-LOD clamp; it also evaluates
  both `ComputeTemporalStaticMeshLOD` samples (dither straddle test). **This is the 4.25 port's `PredictDrawnLod425` pattern**
  [V `europa-port-lod-hunks.diff`], with the 5.1 scale semantics (multiply, [V `SceneVisibility.cpp:2122`]; the 4.25 code
  passes the inverse). If `r.ForceLOD ≥ 0`, every spatial fire is refused (`force_lod_cvar_active`).
- The camera at `OnWorldTickEnd` is this frame's (the 086-02 `camera_clipping` precedent) [R].

### 5.2 Onset and offset, 0 frames off, from measured render state
- **Render-side record (new, cheap, Engine API only).** The plugin's colour SVE pass already takes a per-request render watch
  for `stuck_low_mip` (`AnomalySceneViewExtension.cpp:193-196`) [V]. For each held `lod_popping` target the tick-end sample
  passes `{RenderData*, bounds origin/radius, MinLOD, held value}`; the render thread computes `ComputeStaticMeshLOD` and
  both temporal samples on the **real** `FSceneView` of the captured family and attaches `lod.engine_lod_render` to the
  completed frame.
- **Agreement check in the drain:** if the render-side `L_eng` differs from the tick-end one on any labelled or flagged frame,
  that frame's entry becomes transition-only with reason `lod_unconfirmed` and `run_summary.lod_render_mismatch_frames` is
  incremented (safe direction; Question 10). The prediction: 0 on every leg.
- **Swap frame:** the first frame whose tick-end sample reads "released" after a LABELLED frame. Static forced-LOD changes reach
  the same frame's render [V]; the record confirms the held value per frame.
- **Gate truth:** onset and offset are judged from pixels against a null leg on the same scripted path (§9 L1). The positive
  run must equal the frames where the anomaly leg differs from the null leg, outside `lod_subvisible` frames.
- **Trigger timing vs label timing:** the trigger is evaluated in the anomaly's `Tick`, before `OnWorldTickEnd`, in both tick
  orders. It reads the previous frame's camera, so a release can land one frame after the crossing. The label never depends on
  that: it reads the state at tick end. The recorded trigger distance is the one at the swap frame.

### 5.3 Subtypes and additive keys
- `anomaly_type` stays `lod_popping`. `anomaly_subtype` = `distance_threshold`, `view_entry` or `periodic`, from telemetry key
  `lod.trigger` through the existing `Ev->Subtype` path (`SUB:10359-10370`, `:10543-10546`).
- **`labels.jsonl` entry (telemetry, only on `lod_popping` entries):** `lod.trigger`, `lod.state`
  (`armed|labelled|subvisible|swapped`), `lod.held_lod`, `lod.engine_lod`, `lod.engine_lod_render`, `lod.screen_size`,
  `lod.distance_cm`, `lod.coverage_pct`, `lod.swap` (true on a swap frame only).
- **`annotation.json` event (additive):** `lod_swap_frames` (session indices), `lod_swap_ticks` (`tick_index`),
  `lod_trigger_screen_size`, `lod_trigger_distance_cm` (at the first swap), `lod_held_lod`, `lod_swap_to_lod`.
  `label_schema_minor` 2 → 3 (Question 12).
- **`run_summary`:** `lod_armed`, `lod_labelled_events`, `lod_untriggered`, `lod_untriggered_event_ids`, `lod_unswapped`,
  `lod_unswapped_event_ids`, `lod_capped`, `lod_window_extension_frames`, `lod_subvisible_frames`,
  `lod_render_mismatch_frames`, `lod_refused_by_reason` (object), `label_transition_lod_subvisible_entries`,
  `label_transition_lod_unconfirmed_entries`.
- **Two new reason bits** in `SYNC:17-53`: `lod_subvisible` (1024), `lod_unconfirmed` (2048); `NumReasons` 10 → 12; both in
  the legacy mask; `uint16` has room.

### 5.4 The m56 `settle` flag around the swap
- m56 flags only frames after a real burst fire or revert (095-06 S3). The swap is neither, so it is **not** flagged.
- Reason: a static forced-LOD release is a proxy recreate in the same frame with dithering disabled while forced [V]; at AA-off
  the swap frame should equal the null leg exactly. The bench measures this (gate L2); if it does not hold, chat rules a flag.
- Under TSR the swap frame carries history residue like every other edge; the optional temporal windows (release default
  0/0/0) cover it when a client turns them on. Unchanged behaviour.

### 5.5 Masks
- Unchanged rules: a mask on every labelled frame, none on unlabelled ones.
- On positive frames the mask is the held LOD's silhouette, which is what is drawn (custom depth uses the same submitted LOD) [R].
- **The swap frame is not labelled, so it has no mask.** The last positive frame before it has the held silhouette.
- `lod_subvisible` frames: transition-only, no mask (like other transition-only entries).

---

## 6. Yield

- **Host properties that decide it:** the number of plain static meshes with ≥ 2 LODs in reach that are not Nanite, not HISM
  or foliage, and whose LODs differ strongly; how often the player walks toward or turns toward them.
- **On our hosts today [R]:**
  - StackOBot MainWorld: two rocks (`SM_rock`, 4 LODs) are the only multi-LOD, non-Nanite, non-foliage targets; the Bot is
    single-LOD (G20); foliage is excluded (m27); most structural geometry is Nanite or single-LOD (m30, 084-08). The rock's
    LOD contrast is faint (G149).
  - Lyra ShooterGym: the one `lod_popping` draw on record hit a single-LOD cube (072).
  - So on both bench hosts, free-play yield is about zero. The bench uses the fixture (§8).
- **Unattended runs and standing still:** no movement → no candidate → `lod_popping` is excluded from the round → the draw picks
  another enabled anomaly. A capture is never left with empty bursts because of LOD; it simply has no LOD events while the
  player stands still (B2).
- **Rough estimate for a host rich in multi-LOD props (not measured; for planning only):** opportunities while walking every
  5–10 s (a qualifying prop ahead within 30°, or a turn past one); with four ids enabled and a burst every ~0.5 s, an
  opportunity lasting 2–3 s gets drawn with probability ~75 %; ~60–80 % then trigger before the timeout ⇒ **about 3–6 LOD
  events per minute of active movement, 0 when still.** Today's timer gives one event per LOD-drawn burst whenever a
  qualifying target is visible, whatever the camera does.
- **Cost to other anomalies:** a LOD event can lengthen its burst by the pre-roll (≤ 90 frames) and the extension (≤ 150).
  Both are counted, so the readout shows it.
- **Option for chat (Question 11):** draw `lod_popping` first whenever it is armable (opportunities are rare and short).
  Recommendation: equal draw in v1, measure, decide later.

---

## 7. The old timer mode (B4)

- **Kept as mode `periodic`**, by explicit argument only: `IAI.Apply lod_popping <substring> periodic [half_period_frames]`,
  and a bare number as the second argument still means periodic, so existing bench scripts keep working. The auto pool always
  passes `=Target` with no mode → spatial (`auto` picks the trigger the candidate qualified for). Recommended (Question 7):
  `periodic` refused unless the bench gate is open (`-IAIBench`), reason `periodic_bench_only`.
- **Every place that assumes the timer:**
  - `Anomaly_LodPopping.h:13-14` (description, usage), `:31-38` (half-period members); `LP:16-59` (parsing), `LP:238-263` (tick).
  - `AnomalyDefaults.h:8,13,22`, `AnomalyDefaults.cpp:262-276` (key and describe), `:1415-1419` and `:1938-1944`
    (`IAI.Anomaly.LodHalfPeriod`): keep, document as periodic-only.
  - `AnomalyDefaults` gates that do not fit the spatial modes (Question 8): `LodPoppingMaxDistanceCm` (200 cm, `:857-929`,
    `:1354-1367`, `:1473-1497`, `:1879-1890`) and `LodPoppingRequireHighestLod` (`:1026-1097`, `:1525-1549`, `:1810-1825`):
    apply to `periodic` only. `LodPoppingMinCoveragePct` (7 %) becomes `C_vis` in the spatial label rule.
  - Capture run-config echo `SUB:4406-4409` (prints `lodHalf`, proximity, coverage, highest-LOD): reword, add the trigger
    settings.
  - Catalog spec `AnomalyInjectorSubsystem.cpp:176-180` (`half_period_frames`): becomes `mode` enum
    {`auto`, `distance_threshold`, `view_entry`, `periodic`} plus `params`.
  - `AUTO:41`, `:55` (pool and default-on): unchanged ids; the default-on entry now means the spatial modes.
  - Selector `AnomalySelectorSubsystem.cpp:33`: the selected actor is passed with no mode → spatial auto; a refusal reason is
    shown when it cannot arm.
  - Docs: `docs/client-readme.md:204`, `:388`, `:534`, `:1002`, `:1285`, `:1368`, `:1392`; `docs/architecture.md` (24 lines);
    `docs/setup-runbook.md:94`, `:115-118`, `:272`, `:285`; `docs/onboarding.md` (2); `docs/PRE-DELIVERY-CHECKLIST.md` (lod
    boxes, if any).
  - Tools: `tools/verify_capture.py:193`, `:1026`, `:3224`, `:3946`, selftest cases `:4243-4245`, `:4334-4335`;
    `tools/label_sync_check.py:47` and its per-type rules; `tools/measure_label_offset.py:176`.
  - CaptureBench: the 084-09 `BLODF` legs (`LodHalfPeriod 8`, `2 4 16 14 0`) stay valid only as `periodic` regression legs.
  - **Dashboard repo** (`anomaly-dashboard`, `master`): `src/anomalies.ts:12` blurb ("Object flips to a low-detail version")
    → "An object shows a low-detail version, then pops to full detail as you approach or turn toward it"; mock data
    `src/mock/*.ts`; the pool list comes from the engine snapshot (m19), so no default lives there; `src/store.ts:10` hidden
    ids unchanged. Re-bundle after the readme changes.
  - **The m30 gates:** G-P2′ (half-period in frames at 30 and 60 fps) applies to `periodic` only; G-P4 (coverage gate fires)
    becomes the `C_vis` rule's can-fail in L1; the 200 cm gate is retired for spatial modes.

---

## 8. The bench

### 8.1 Where the scripted camera path lives
- **Recommended (Question 9): the `AnomalyBench` module**, which is in the plugin repo but is never delivered: Shipping-denied in
  `AnomalyInjector.uplugin`, removed from the staged descriptor and source by `tools/stage_plugin_delivery.py:41-42` (ruling F9),
  and lever-audited. It already holds the B-CC camera schedule `cc_v1` (`BENCH:313-375`) with exactly the machinery needed:
  input lock, poses keyed by `session_index` applied at `OnWorldPreActorTick` (`BENCH:176-184`, `:146-156`), per-frame pose
  read-back with `err_cm`/`err_deg` (`BENCH:157-175`), runtime fixture spawns (`BENCH:300-311`), bench gate, map allow-list
  (`BENCH:109-118`).
- Alternative, as the brief named it: the CaptureBench plugin's own C++ module. Against it: its repo has no remote, and its
  module is the frozen probe.
- New schedule `IAI.Bench.CameraSchedule lod_v1 <path>` with paths `approach`, `approach_retreat`, `turn_in`, `turn_away`,
  `still`; `CB_LodFixture` added to the allow-list.

### 8.2 The fixture (B6: look first)
- **`/Game/AnomalyFixtures/CB_LodFixture` is the only fixture with multi-LOD meshes at approachable distances [R 084-08b,
  089-02a]:** `SM_LodFix_N` (no dither) and `SM_LodFix_D` (dither on): LOD0 orange sphere (960 tris), LOD1 blue cube (48 tris),
  screen sizes [1.0, 0.05], not Nanite, static, no shadow; sun pitch −40 and three 400 cd point lights; PlayerStart (0, 0, 200)
  facing +X; the two targets at (850, ±340, 200), scale 4.6 (bounds radius 230).
- **View entry: usable as it is.** Start yawed 90° away from the targets, turn toward +X at ~60°/s: each target is prepared at LOD1
  off-screen, enters as a blue cube, snaps to the orange sphere after `N_entry` frames.
- **Distance: needs smaller copies, spawned at run time (no authoring, no cook).** At scale 4.6 the engine keeps LOD0 out to
  ~82 m (screen size 0.05), so the freeze rule never finds a coarse target on the floor. `lod_v1` spawns copies of the two
  fixture meshes at scale 1.0 (natural LOD1 beyond ~17.8 m) along the +X lane at 20–25 m, plus the path `approach` from x = 0.
  Static mobility for the D copy (dithering needs it).
- **Content property to state, not hide:** with this mesh the engine's own LOD1 switch is ~7× farther than the 7 % coverage
  distance, so most of the approach is flagged `lod_subvisible` and positives start at ~2.5–3 m. The clips show it with the
  default `C_vis` and with a lower bench override (`IAI.Anomaly.LodMinCoverage 1`). If the owner finds the approach
  unrepresentative, the fix is a second mesh pair with LOD1 at screen size 0.3 (a `make_lod_fixture.py` option) and one cook.
- **MainWorld rock (optional seventh clip):** a scripted approach to `SM_rock` shows how faint a real-content pop is.

### 8.3 Look-first clips for the owner (before any gate)
- **Prerequisites:** 095-07's cook (its map set already lists `CB_LodFixture`) and the m57 exe by code-only swap; E: space;
  a luma premise on the first fixture leg (G151, G402).
- **Legs** (packaged, 1280×720, 30 fps paced, native order, delivered AA (TSR), 300 frames, targeted `lod_popping`, each with a
  null twin on the same path):

| Clip | Path | Anomaly args | Shows |
|---|---|---|---|
| C1 | `turn_in` | `=CB_LodFix_N view_entry` | blue cube enters, holds 4 frames, snaps |
| C2 | `turn_in` | `=CB_LodFix_D view_entry` | the same on the dithered material |
| C3 | `approach` | `=<copy N> distance_threshold` | coarse LOD while approaching, pop at the threshold |
| C4 | `approach_retreat` | `=<copy N> distance_threshold` | up-pop, then down-pop on retreat (hysteresis) |
| C5 | `still` | `=<copy N> distance_threshold` | nothing happens (B2) |
| C6 | `approach` | `=<copy N> periodic 8` | today's timer on the same path, for contrast |

- **Assembly with existing tools:** `tools/verify_capture.py --dir <session> --out <session>\annotated` draws the label boxes
  (red = labelled, amber = transition-only); `ffmpeg -framerate 30 -start_number 0 -i frame_%05d.png` (the same call the
  dashboard's `encode_watcher.py` makes) gives one mp4 per leg; one `hstack` call puts anomaly | null side by side. A
  20-line CaptureBench script wraps the three steps; no new image tool.
- **What the owner judges:** does each pop read as the real thing; is 4 frames right for view entry (variants 2 and 8 can be
  produced from C1 by re-running with `IAI.Anomaly.LodEntryFrames`); is the hysteresis down-pop wanted.
- **Size:** 12 legs × ~216 MB (0.72 MB/frame [R m56 plan]) + annotated copies ≈ **5 GB**; ~25 min of legs plus assembly.

---

## 9. Gates and can-fails (label-sync standard)

Common recipe: `CB_LodFixture` with `lod_v1`, packaged, 1280×720, 30 fps paced, **AA-off identity arbiter** for pixel gates
(`r.AntiAliasingMethod 0`, Lumen/GI/reflections off, native order; G228, G230), every attempt banked, predictions frozen in one
file before the first leg. Pose read-back from `lod_v1` must show `err_cm` ≈ 0 on every frame, or the leg is INVALID (A63 shape).

| Gate | Legs | Pass rule | Can-fail |
|---|---|---|---|
| **L0 control** | null vs null, each path | 0 frames differ at AA-off | — (it is the control) |
| **L1 edges from pixels** | `turn_in` N, D; `approach` N, D; `approach_retreat` N; each with its null; native | the labelled frames equal the frames where anomaly ≠ null in the target region, ignoring `lod_subvisible` frames; onset and offset **0 frames off** | offline mutant with the label shifted by 1 frame fails; a leg with `IAI.Anomaly.LodMinCoverage 100` must have 0 positives (the `C_vis` rule fires) |
| **L2 swap frame** | from L1 | recorded `lod_swap_frames` = first frame equal to null after a positive run; the swap frame equals the null exactly at AA-off | bench lever `IAI.Bench.LodReleaseLag 1` (release one tick after the recorded swap) must fail |
| **L3 no movement, no event** | `still` with targeted lod; auto-pool with only `lod_popping` enabled, `still`, 300 frames | 0 lod events; `lod_untriggered` = fires; auto-pool: `lod_not_armable` rounds counted, 0 wasted bursts | lever `IAI.Bench.LodForceTrigger` on `still` must produce events (the "no event" reading is not blindness) |
| **L4 no label unseen** | `turn_away` (target armed off-screen, never enters) | 0 labelled frames on the target; no event | lever `IAI.Bench.LodSkipOnScreen` (drops the on-screen term) must produce labelled off-screen frames |
| **L4b unseen means unchanged** | from L4 and L1 armed stretches | anomaly = null outside the target region while armed off-screen (shadow, indirect light) | none planned; a fixture with a shadow-casting off-screen copy is the follow-up if L4b ever fails |
| **L5 render record** | every leg | `lod_render_mismatch_frames` = 0; no `lod_unconfirmed` entry | lever `IAI.Bench.LodPredictStaleView` (game-thread prediction uses last frame's camera) must produce mismatches on moving legs |
| **L6 revert leaves nothing** | every leg; plus a destroy leg (`IAI.Bench.DestroyTarget` while armed) | every touched component's `ForcedLodModel` read back equal to its pre-arm value (one log line per revert); after the run, frames equal the null | lever `IAI.Bench.LodSkipRestore` must fail the read-back |
| **L7 periodic unchanged by name** | `CB_LodFix_N` static camera, `periodic 8`, m57 exe vs integ exe, both orders | m56's non-settle comparator: every field equal except the declared new keys | the comparator's doctored pair fails |
| **L8 others untouched** | `blinking`, `missing_texture` targeted, `CB_GateLevel`, m57 exe vs integ exe | the same comparator, no new key on their entries | the same |
| **L9 masks** | from L1 | every labelled frame masked; MASK-TIE 0; no mask on swap frames | — |
| **L10 continuity and order** | L1 legs again in synthesized order (label gate only, G230) | m56 continuity check passes (extension frames included); labels equal native order frame for frame | — |
| **L11 TSR reading** | L1 legs at delivered AA | reported, not gated: frames of residue after each swap | — |

- **Totals:** ~34 legs, ~7.5 GB, ~2–2.5 h of bench plus evaluation (~1 min per 300-frame leg + startup ~40 s each).
- **New check scripts (CaptureBench):** `m57_null_pair.py` (L1, L2, L4b; imports `m45_identity.py`'s frame reader),
  `m57_lod_readout.py` (L3, L5, L6 from logs and `run_summary`). Each proven both ways on doctored copies first (G96).
- **Stop rule:** a failed gate is reported and stops; no same-turn fix.

---

## 10. Risks

1. **Label sync around an engine-timed swap.** Avoided by design: the plugin times the release. The remaining engine-timed
   moment is the onset of "wrong" (the engine's own boundary); it is computed with the engine's function on the frame's view
   and cross-checked on the render thread (§5.2). Mesh LOD streaming can clamp `L_eng` coarser than predicted; the predictor
   applies the first-resident clamp and the record catches any miss.
2. **Dithered materials.** A release while the camera straddles a natural boundary would crossfade for up to 0.5 s. Rule:
   release only when both temporal samples agree; otherwise wait (counted, `lod_release_deferred_dither`). The D variant is in
   every gate.
3. **Temporal AA residue** on the swap frame: same as every edge; gated at AA-off, reported under TSR.
4. **Skeletal meshes: refused in v1** (`skeletal_not_supported`). `SetForcedLOD` reaches the render one frame after a tickable
   writes it [V], auto LOD lags two frames [R], leader-pose sync copies LODs [R]. ⚠ **Finding for chat:** today's timer has the
   same one-frame latency on skeletal targets in native order, so its labels could lead its pixels by a frame. Derived from
   source, never measured (the only skeletal target on our bench is single-LOD).
5. **Instanced meshes: refused in v1** (`instanced_not_supported`). ISM: one LOD for the whole component from combined bounds.
   **HISM ignores forced LOD [V]**. ⚠ **Finding for chat:** today's timer accepts a HISM target outside foliage, forces nothing,
   and still labels the "popped" frames: positives with no visible change. Proposed: `periodic` refuses HISM too (one line).
6. **Nanite:** refused regardless of `IAI.Targets.AllowNanite` (`nanite_no_discrete_lods`); forced LOD does nothing there.
7. **Streaming / world partition:** a candidate can stream out while armed → `OnTargetLost` path.
8. **Target destroyed while armed:** handled (§4.2), gated (L6).
9. **Two tick orders:** labels are sampled at tick end (order-independent). The trigger runs in the anomaly `Tick`, before tick
   end in both orders. `BurstAwaitsDeferredOnset` reads state in the middle of the capture `Tick` (`SUB:1188`), so the window may
   open one frame later in one order; labels do not move (same as `stuck_low_mip`). Gated by L10.
10. **`lod_corruption`** also forces LODs (`FireWindow`, not in the default pool). The one-anomaly-per-actor rule keeps them off
    the same actor. A component already carrying a non-zero `ForcedLodModel` at arm time is refused (`forced_lod_present`).
11. **Census:** distance candidates are in view and get census opinions; view-entry candidates are off-screen and get none
    (the census has no verdict on unseen actors). The census tags custom depth; a held LOD changes the drawn silhouette it
    measures, which is the truth.
12. **Off-screen arming changes shadows and indirect light** (§3.2 rule; gate L4b).
13. **Cost:** one `CalcSceneView` per held target per tick, cached per frame. Measured on the bench (`lod_predict_ms_*` in
    `run_summary`).
14. **Exposure dip (m48):** a large swap can trip the luminance mark; marks only, not labels.
15. **A host with no qualifying content** gets no LOD events at all. That is the honest outcome; the readout says why
    (`lod_refused_by_reason`, `lod_not_armable`).

---

## 11. Commit breakdown, briefs and bench hours

On `feat/m57-lod-spatial-trigger`, cut from the integration tip after 095-07 (or from `master` once m56/m58 are merged):
1. `feat(lod): m57 engine-exact LOD prediction` — `AnomalyLod::PredictEngineLod` (CalcSceneView + `ComputeStaticMeshLOD` +
   temporal samples + clamps), triangle-ratio helper, pure selftest `tools/lod_trigger_selftest.cpp` (threshold, hysteresis,
   freeze-invisible rule, contrast test), mutants.
2. `feat(lod): m57 lod_popping spatial modes` — modes, candidate provider `AnomalyLodTrigger`, arm/trigger/release state machine,
   positive and subvisible rule, telemetry, refusals (skeletal, ISM, HISM, Nanite, `r.ForceLOD`, forced LOD present, shadow in
   view), `periodic` kept and HISM-refusing, ini and console settings with echoes.
3. `feat(capture): m57 deferred-onset timeout per anomaly, burst extension, subtypes and keys` — `GetDeferredOnsetTimeoutFrames`,
   `IsHoldingEventOpen`, the Positives extension and cap, per-anomaly timeout counter, `Ev->Subtype` from `lod.trigger`, event
   and summary keys, reason bits, untriggered and unswapped id lists, auto-pool eligibility pre-check.
4. `feat(capture): m57 render-side LOD record` — SVE pass record, drain agreement check, `lod_unconfirmed`.
5. `feat(bench): m57 lod_v1 camera schedule and levers` — paths, fixture copies, the five levers of §9, `lever_audit.py`.
6. `chore(tools): m57 readers learn the LOD subtypes, reasons and keys` — `verify_capture.py` (reason list, per-type table:
   the onset of a `lod_popping` event is a flag boundary, not a pixel edge; the swap is the pixel edge), `label_sync_check.py`,
   `annotation_transition_check.py`, `measure_label_offset.py`, their selftests.
7. `docs: m57` — readme, architecture, runbook, checklist, status block, journal, gotchas.

Outside the plugin: CaptureBench (clip script, `m57_null_pair.py`, `m57_lod_readout.py`, predictions); dashboard (blurb, mocks,
re-bundle).

**Briefs:**
- (i) implementation and offline tests, both targets 0 errors / 0 warnings (Editor and Game), `cl /Zs` where builds are
  blocked (~5–6 h of Code);
- (ii) look-first clips (needs 095-07's cook and E: space; ~1 bench hour with assembly) → owner judges;
- (iii) parameter changes from the owner's verdict, if any (short);
- (iv) gate campaign (~2.5 bench hours);
- (v) independent read-only review side session (~1 h);
- (vi) docs, merge, dashboard re-bundle (~1–2 h).

**Bench total ≈ 4 hours,** after 095-07 and after E: is freed.

---

## 12. What the client will see change

- **LOD pops now happen because the player moved.** A pop is either (a) an object that stays at a low-detail version as you
  walk toward it and snaps to full detail at a distance where the change is clearly visible, or (b) an object that comes into
  view at a low-detail version when you turn, and snaps to full detail a few frames later.
- **No movement, no LOD event.** If the player stands still, the capture contains no LOD pops; other anomalies fill those bursts.
- **The positive frames are the frames where the low-detail version is clearly visible** before the snap. Frames where it is
  wrong but too small to see carry a new flag, `lod_subvisible`, and are dropped by the existing rule for flagged frames.
- **The snap frame is recorded** (`lod_swap_frames`), with the distance and on-screen size at which it happened.
- **New subtypes:** `distance_threshold` and `view_entry`. The old timed pop (`periodic`) is no longer used in captures.
- **Fewer LOD events, every one meaningful.** How many depends on how many objects in the game have several detail levels
  (Nanite objects have none) and on how much the player moves.
- **Unchanged:** the id `lod_popping`, the mask rules, every other anomaly.

---

## 13. Questions

### For chat (forks, each with a recommendation)
1. **Mechanism:** plugin-timed forced LOD (recommended) vs engine-timed via `SetBoundsScale` (culling, shadow and box side
   effects; dithered crossfades; no clean swap frame).
2. **v1 scope:** plain static meshes only; skeletal and ISM later, each with its own latency fix. **Recommended: yes.**
3. **An armed target that never triggers:** write no event and list its id (recommended; B2 says it does not count) vs today's
   `manifested:false` event (no reader change, but it still appears as an event).
4. **An event without a visible swap** (cap reached, or the player turned away before the snap): treat as Question 3
   (recommended; B5 asks for contrast at the moment of the pop) vs keep it with a `lod_release` marker.
5. **Wrong-but-too-small frames:** new transition reason `lod_subvisible` (recommended; the safe direction, client rule
   unchanged) vs arming only targets whose wrong span starts above `C_vis` (no new reason, much lower yield).
6. **Hysteresis down-pop on retreat:** on by default (recommended; it is in B1) vs off until the owner sees C4.
7. **`periodic` access:** bench gate only (recommended; "tests only") vs any non-Shipping console.
8. **The 200 cm proximity gate and the highest-LOD gate:** `periodic` only; the spatial modes use the trigger's own visibility
   and contrast rules (recommended; a 2 m release distance would make distance pops nearly impossible and contradicts the freeze).
9. **Camera-path home:** the `AnomalyBench` module (recommended; it already holds `cc_v1` and is staged out of every delivery)
   vs the CaptureBench plugin as the brief named it.
10. **Label source:** tick-end game-thread prediction with the engine's function, confirmed per frame by a render-thread record,
    disagreements flagged `lod_unconfirmed` (recommended) vs m52-style labels taken from the render record in the drain
    (heavier; touches the mask arm path).
11. **Draw weight when armable:** equal (recommended for v1) vs LOD first.
12. **Schema:** `label_schema_minor` 3 for the new event keys (recommended) vs fold into 2 if m57 ships in the same release as m56.

### For the owner or the client (one line each)
- What does the client's reference LOD clip show: an approach pop, a turn pop, or both, and for how many frames is the low detail visible?
- Are most static meshes in the client's game Nanite (then `lod_popping` will yield almost nothing there)?
- Do they want LOD pops on characters (skeletal meshes), or props only?
- In their capture sessions, does the player mostly walk and turn, or stand and watch?
- Is the down-pop on retreat (C4) something they want in the dataset?

---

## 14. 4.25 note

- **`lod_popping` is not built on 4.25 today.** On `feat/europa-port` `2828f6a` the whole of `Anomaly_LodPopping.cpp` is inside
  `#if ANOMALY_ENGINE_51`, its registration is guarded the same way, and the 4.25 default pool uses `lod_corruption` instead
  [V `git show`/`git diff`, receipt `europa-port-lod-hunks.diff`].
- **The predictor exists there:** `AnomalyLod::PredictDrawnLod425` builds a `FSceneView` with `CalcSceneView` and calls
  `ComputeStaticMeshLOD` with MinLOD, the first-resident clamp, `r.ForceLOD` and `r.StaticMeshLODDistanceScale` (passed as the
  inverse) [V]. m57's 5.1 predictor follows the same shape; the scale semantics must be checked per engine when porting.
- Touched-file differences (`integ/m56-m58` vs `feat/europa-port`, receipt `europa-port-diff-touched-files.txt`; this diff
  also contains m56/m58, which the 4.25 branch lacks): `Anomaly_LodPopping.cpp` +4 (the guard), `AnomalyLod.cpp/.h` +60
  (compat calls and the 4.25 predictor), `AnomalyViewport.cpp` +18/−15 (compat calls), `AnomalyAutoInjectorSubsystem.cpp`
  +104/−8, `AnomalyInjectorSubsystem.cpp` +58/−6, `AnomalyBenchModule.cpp` +16, `AnomalyCaptureSubsystem.cpp` (mostly m56/m58).
  `Anomaly_LodPopping.h`, `IAnomaly.h`, `AnomalyActiveSource.h`, `AnomalyViewport.h` are identical.
- **Mechanisms on 4.25:** `ForcedLodModel` / `SetForcedLodModel` exist [R, used by the 4.25 `lod_corruption`];
  `ComputeStaticMeshLOD` is callable [V, the port compiles a call to it]; temporal LOD dithering and `lod.TemporalLag` [R, not
  re-read]. The render-side record needs the 4.25 capture hook (the port captures through its own pre-UI path, not the 5.1
  SVE), so it must be re-designed there. **The 4.25 forward-port of m57 is a separate unit (M5); it adds `lod_popping` to
  4.25 for the first time.**

---

## Unplanned / open
- Exact default numbers (`v_min`, `θ_margin`, `ω_min`, `T_ahead`, `R_tri`, `m`, `h`, `N_entry`, the timeout and the cap) are
  starting values; the look-first clips and the gate predictions fix them.
- The rough yield estimate (§6) is not measured; the first owner-played capture on a content-rich host is the real reading.
