# 095-11 — Independent review of m57 (lod_popping spatial trigger) and m59 (forced-LOD refusals)

**Date:** 2026-11-14 · **Reviewer:** Claude Code (Opus 5.5, xhigh), headless side session · **Mode:** read-only. Nothing
built, launched, edited, committed or checked out. No Codex.
**Refs read (git objects / exported copies):** m57 `feat/m57-lod-spatial-trigger` `f77deed` vs `integ/m56-m58` `876c719`;
m59 `fix/m59-forced-lod-refusals` `af1b08f` vs `master` `ebc34ed`; CaptureBench `m57-gates` `9eb8ca6` vs `m56-gates`
`1978cd8`; UE 5.1.1 source. Specs: `095-08-ruling.md` (with addendum), `095-08-plan-B-…`, `095-00-ruling.md` §3/§6,
reports 095-09 / 095-09b / 095-10, journal 095-10, frozen `predictions-095-10.txt`.
**Receipts:** `_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-11\` (exported trees, diffs, sub-reviewer
probes, `receipts.md` with the arithmetic behind B1 and B2).
**Marks:** **[V]** = I read it myself in the cited source and, where stated, did the arithmetic. **[R]** = reported by a
read-only sub-reviewer (three were used: m59, readers, bench scripts); not fully re-read by me.

## Verdicts
| Unit | Verdict | One line |
|---|---|---|
| m57 | **READY AFTER FIXES** | The per-frame label path is sound in both tick orders; but with the as-built arm rules and bench paths, almost every look-first clip and gate leg would produce no event (B1, B2), and the internal reader is loosened on every 2.3 session (B3). |
| m59 | **READY FOR GATES** | A strict improvement; no regression found. Three coverage gaps and readme errors to fix before the client drop (S9–S11). |
| bench (CaptureBench `m57-gates`) | **NOT READY** | The null-pair check reads the label box wrongly and can pass vacuously; the clip script cannot build a clip. |

Counts: **6 BLOCKER, 11 SHOULD-FIX, 14 NOTE.**

---

## Findings

### BLOCKER

**B1 — m57 (bench path + arm rule). The turn paths never arm a view-entry fire under the default burst schedule, in
native order. [V]**
- Where: `Source/AnomalyBench/Private/AnomalyBenchModule.cpp:72-81` (`LodPoseAt` turn paths: yaw −100 until si 30, then
  +2°/frame); `Source/AnomalyInjector/Private/AnomalyLodTrigger.cpp:85-113` (yaw rate over 8 ring samples back);
  `AnomalyCaptureSubsystem.cpp:5596-5621` (a refused fire is a zero-match burst; the phases still run, so the next
  `BeginFire` is a full cycle later).
- Facts used: tickables run at `LevelTick.cpp:1606`, the camera cache updates at `:1621`, tick end at `:1814`. So at the
  capture's `BeginFire` in tick t the view is pose t−1, and the ring's newest sample (native order, injector after
  capture) is pose t−2. The bench applies pose t at tick t (`AnomalyBenchModule.cpp:262`).
- Scenario, default config (pre 4, settle 2, positives 8, post 4): `BeginFire` runs at si 3, 19, 35, 51, 67.
  - `CB_LodFix_N` (850, −340), radius 230: it is less than 20° outside the view from the start and enters the view at
    si 41. At si 35 the 8-sample yaw rate is (−92 − (−100)) / 0.300 s = 26.7°/s < 30°/s → `not_turning_toward`. At si 51
    it is `already_in_view`. Arming is possible only at si 36–40, between two `BeginFire` ticks.
  - `CB_LodFix_D` (850, +340): arming possible only at si 53–62. At si 51 it is 22.3° outside (> 20°); at si 67 it is in view.
  - `turn_away` (L4): at si 35 the rate is 26.7°/s; by si 51 the camera is turning away.
- Effect: C1, C2, L1 (turn legs), L4, L4b and L12a get no armed fire. L4 would "pass" (0 labels, no event) for the wrong
  reason, and its can-fail lever (`LodSkipOnScreen`) cannot fire because nothing is armed. In synthesized order si 35
  reads exactly 30.0°/s, a float coin flip, so L10 could also disagree between orders.
- Smallest fix (bench): make each arming window longer than one burst cycle plus the motion lag, e.g. hold yaw −130 and
  start turning at si 4, or turn at 1°/frame; pin `IAI.Capture.Config` in the predictions and amend them before any leg.

**B2 — m57 (product). The distance arm looks ahead to the contrast switch, but the arm timeout waits for the first
positive frame, which needs coverage ≥ 7 %. On content whose engine switch is far beyond the coverage distance, every
distance fire times out with no event. [V]**
- Where: `AnomalyLodTrigger.cpp:267-293` and `AnomalyLodTriggerRule.h:138-168` (frames-ahead uses `ReleaseScreenSize` =
  contrast switch × 1.1; coverage is not in it); `AnomalyCaptureSubsystem.cpp:1196-1207` (pre-roll until the fire is
  labelled; subvisible frames are not labelled; timeout 90 by `LodPoppingArmTimeoutFrames`).
- Scenario C3 (`approach`, `=IAIBenchLodCopyN`, defaults; FOV 90, 1280×720, copy radius 50, screen sizes [1.0, 0.05]):
  - Armed at si 51 (distance ≈ 20.8 m, 82 frames ahead). The engine draws LOD0 below 17.8 m.
  - The first positive needs ≈ 3.0 m at 7 % coverage (≈ 7.2 m at the bench override of 1 %).
  - The timeout fires at si ≈ 143 (≈ 13.9 m): no event, id in `lod_untriggered_event_ids`. The next burst refuses
    `at_finest_lod`.
  - Even without the timeout: the path stops at x = 1402.5, so the nearest distance is ≈ 303.5 cm, where coverage is
    ≈ 6.9 % (never positive at 7 %). The release also needs 10 % more screen size than the first positive, which the
    path never reaches.
- The second pair does not rescue it: `CB_LodFix_N2/D2` sit at y = ±200, so the nearest distance on the paths is ≈ 358 cm,
  while their engine switch is ≈ 296 cm. The engine never draws LOD0 there.
- Effect: C3, C4, L1 (approach legs) and L5's "checked > 0 on legs with labelled frames" all come back empty. Labels stay
  correct (the wrong frames are flagged `lod_subvisible` and the release frame shows the right LOD), so this is a yield
  and gate defect, not a mislabel. It also caps yield on any host where LOD switches happen at small screen sizes.
- Smallest fix: (a) in the distance arm, look ahead to the larger of the contrast release size and the screen size at
  which coverage reaches `C_vis`; and/or (b) start the arm timeout at the first wrong frame (subvisible or positive),
  not at the arm. Bench: move the copies and N2/D2 onto the path's end with room for the +10 % margin. Re-freeze the
  predictions.

**B3 — m57 (tools). `verify_capture.py --label-rule` loosens the by-id veto check on every schema-2.3 session. [V]**
- Where: `tools/verify_capture.py:3900`: `transition_detail.get("minor") == 2`. The writer now always emits minor 3
  (`AnomalyLabelWriter.cpp:1632`), so the code falls to `:3906`, where any veto in the session excuses every labelled
  frame that is missing from `annotation.json`.
- Scenario (sub-reviewer probe P1, consistent with the code): a kept event loses one labelled frame from `annotation.json`
  and the session has one unrelated m26 veto. Minor 2 → `LABELLED-EXTRA`, exit 1. Minor 3 → exit 0.
- Fix: `in (2, 3)`; add a 2.3 copy of the m56 veto cases to the selftest (all 13 new lod cases use the legacy schema).

**B4 — bench. `m57_null_pair.py` reads `bbox_px` as two corners; the plugin writes `[x, y, w, h]`. [V]**
- Where: CaptureBench `tools/m57_null_pair.py:98-99`; plugin `AnomalyLabelWriter.cpp:221`.
- Effect: the "target region" for L1, L2 and L4b is wrong. Sub-reviewer probes: a correct session FAILS with 0 frames
  differing; a session with the onset labelled one frame late PASSES. The selftest builds corner boxes, so it shares the bug.
- Fix: `box = (x, y, x + w − 1, y + h − 1)`; selftest writes `[x, y, w, h]`.

**B5 — bench. `m57_null_pair.py` judges one event's window only and drops frames the null leg lacks; it can pass a
session where nothing happened. [V window/drop, R vacuous pass]**
- Where: `m57_null_pair.py:102-103` (window = that event's first..last entry, filtered by `si in rows_n`); vacuous pass at
  `:118-148` [R].
- Scenarios [R, probes C–F]: never held and nothing labelled → PASS; every differing frame flagged subvisible → PASS; null
  leg truncated before the recorded swap → PASS; picture still differs after the event → PASS (that is also L6's
  "after the run, frames equal the null", which no script checks).
- Fix: refuse (exit 3) unless both legs have the same session indices; judge every common frame; require ≥ 1 labelled
  frame and an up-swap per positive run; report L4b "not exercised" when no held off-screen frame exists.

**B6 — bench. `m57_make_clip.py` can never build a clip. [V]**
- Where: `m57_make_clip.py:43,46` encodes `annotated/frame_%05d.png`; `verify_capture.py:5195` writes
  `frame_NNNNN_annotated.png`, and only for frames that carry a box (gaps).
- Effect: every C1–C6 assembly fails at the ffmpeg step. Nothing is destroyed.
- Fix: encode the raw frames with an overlay, or rename/fill the annotated set; run the real tool in the selftest. Also
  write under `--out`, not inside the session folder [R].

### SHOULD-FIX

**S1 — m57. The render-side record cannot see dithering, streaming or MinLOD, and it is not taken on the swap frame. [V]**
- No probe when `lod.held` is false (`AnomalyCaptureSubsystem.cpp:6537-6540`), so the swap frame and the frames after it
  are never checked.
- `MarkLodUnconfirmed` compares `EngineLod` only; the render thread's temporal samples are recorded and ignored
  (`AnomalyLabelWriter.cpp:96`).
- `MinLod` and `RenderData` come from the game thread (`AnomalyLod.cpp`, `GetRenderProbeInputs`), so the render check
  recomputes the same formula on the same inputs and can only detect a different view. Plan risk 1 ("the record
  catches any miss" of the first-resident clamp) does not hold.
- The game-thread temporal prediction reads render-thread-owned state, one render frame stale: `SceneView.cpp:999-1009`
  reads `State->GetTemporalLODState()`, updated by the renderer at `SceneVisibility.cpp:4014`.
- Scenario: the D variant (or any dithered static mesh). The release lands while the render's two temporal samples
  disagree → crossfade on frames labelled negative, no flag.
- Fix: probe the swap frame and the following `lod.TemporalLag` window; compare the temporal samples; read the first
  resident LOD on the render thread; flag `lod_unconfirmed` on disagreement.

**S2 — m57. A wrong LOD seen through a see-through occluder with collision is a plain negative, not flagged. [V]**
- `AnomalyLodTriggerRule.h:119` (`!bOnScreen` → Negative); on-screen = frustum and ≥ 1 of 9 `ECC_Visibility` traces to
  bounds points clear (`AnomalyViewport.cpp:234-274`).
- Scenario: the held target is behind glass, a fence or foliage with visibility collision. All 9 traces are blocked, so
  the frames are negative and unflagged while the coarse LOD is visible.
- Fix: in frustum + held + engine finer + trace-occluded → transition-only (reuse `lod_subvisible` or a new reason), never
  Negative.

**S3 — m57 and m59. Label box, mask and observability cover the whole actor while only one component is forced. [V]**
- m57 forces one component (deviation 1); m59 refuses some components of mixed actors (e.g. skeletal body + static prop).
  Neither calls `AnomalyViewport::SetEventComponentScope` (only `Anomaly_TexCorrupt.cpp:1204` does).
- Scenario: an actor with two meshes; the smaller one is off screen or unchanged. `bbox_px` and the mask include it;
  `observable` can be true from the unchanged mesh; the event node's `asset_name` may name the wrong component.
- Fix: set the scope to the forced components at Apply, clear it at Revert, as TexCorrupt does.

**S4 — m57 (tools). A vetoed spatial lod event that carried `lod_subvisible` / `lod_unconfirmed` makes the reader and the
client kit refuse the whole session. [V reader line, R kit line]**
- `verify_capture.py:3392` excuses a vetoed id only if its reasons are exactly `{settle}`; the writer skips vetoed ids in
  `annotation.json` (`AnomalyCaptureSubsystem.cpp:11012`). The kit calls the same check (`label_sync_check.py:2422` [R]).
- Fix: on minor 3, also excuse a vetoed `lod_popping@…` id whose reasons ⊆ {settle, lod_subvisible, lod_unconfirmed}.

**S5 — m57 (kit, docs). The client kit never judges a spatial lod event, and says so nowhere. [V readme, R kit output]**
- Readme §3 "It cannot judge" (`docs/client-readme.md:341`) lists camera_clipping only; `OFFICE-CHECK.md` unchanged [R].
- The kit counts them as not-judgeable but prints no reason; every category reads 0 [R, probe P5]. `KIT_VERSION` 1.6 is
  unchanged though behaviour changed [R].
- Since `periodic` is bench-only, a client can never get a judged `lod_popping` event: their own check cannot find a
  lod label defect.
- Fix: print "not judgeable (spatial lod)" with the reason; bump the kit version; say it in the readme. See question 2.

**S6 — m57 (docs). Schema 2.3 and the two new reasons are missing from the readme's reference tables. [R]**
- The `label_schema_minor` row lists 1 and 2; no 2.2→2.3 changelog; no reason-table rows for `lod_subvisible` /
  `lod_unconfirmed`; the per-anomaly "reasons" cell still says none; several run_summary keys and the burst extension are
  undocumented; "every event that has labelled frames stays" omits m26 vetoes.

**S7 — bench. `m57_lod_readout.py` can print PASS with nothing checked. [R]**
- No expectation flag → PASS (probe R1: 5 render mismatches, PASS). `--expect no_events` passes with no ARMED line and a
  missing `lod_untriggered` key. L5 does not read `label_transition_lod_unconfirmed_entries`. L6 does not pair REVERT lines
  with fires, and the plugin prints no REVERT line for a destroyed mesh (`Anomaly_LodPopping.cpp:643-658`, `if (Mesh)`) [V].

**S8 — bench. `make_lod_fixture.py` options. [V reachability, R the rest]**
- The second pair at y = ±200 never switches LOD on the `lod_v1` paths (see B2) [V].
- Not atomic: `SM_LodFix_N2` is saved before later refusal checks run; a failure leaves an orphan that blocks every
  re-run [R]. Running `second_pair` then `instanced` as two runs fails the second read-back ("found 4 of 2") [R].
- It deletes nothing and never touches `CB_GateLevel`, `CB_LodFix_N/D` or `SM_LodFix_N/D` [R]. Unsaved changes in an
  interactive editor's open map would be discarded [R].

**S9 — m59 (gap, pre-existing). An explicit `lod_corruption <target> 1` forces LOD 0 and can change nothing. [V]**
- `Anomaly_LodCorruption.cpp:31-34, 84, 91`; `AnomalyLod.cpp:152-160` (`ForcedLodModel` 1 = LOD 0). A target already at
  LOD 0 is labelled with nothing changed. The refusal log says the guard applies "with or without a lod-index", which
  reads as broader than it is.
- Fix: refuse when the resolved forced LOD is not coarser than the current LOD (`AnomalyLod::GetCurrentLod`); at minimum
  refuse index 1.

**S10 — m59 (gap). MinLOD or stripped LODs make a multi-LOD mesh effectively single-LOD; it is not refused. [V engine]**
- The proxy clamps a forced LOD to `ClampedMinLOD + 1` (`StaticMeshRender.cpp:2361`, `ClampedMinLOD` at `:288`), while
  `GetWorstLod` counts `LODResources` (`AnomalyLod.cpp:34-46`). A 2-LOD mesh with MinLOD 1 always draws LOD 1.
- Scenario: `lod_corruption` (or `periodic` `lod_popping`) on such a mesh labels frames with nothing changed. (m57's
  spatial modes are safe: the predictor applies MinLOD, so no contrast exists and the arm refuses.)
- Fix: count usable LODs as N − max(MinLOD override or mesh MinLOD, first LOD with vertices); same for skinned meshes.

**S11 — m59 (docs). The readme states the skeletal switch wrongly. [V]**
- `docs/client-readme.md:1257`: single-LOD skeletal meshes are "still refused as `single_lod` whatever this setting
  says". At switch 0 the reason is `skeletal_unmeasured` (`AnomalyLodPolicy.h:14-18`).
- `:1255`: with the switch at 1 "the behaviour is then exactly as in earlier builds". False for a single-LOD skeletal
  mesh, which earlier `lod_corruption` forced and which is now refused. The console help repeats it [R].
- The H4 filter, if added for the client, must not rely on the first node's `component_class` alone: it is the first
  visible mesh component with an asset, not the forced one, and game subclasses of HISM have other names [R].

### NOTE
- **N1 [V] m57.** The commit in the next tick reuses the tick-end evaluation only through a cache key (exact camera pose,
  bounds origin, held, frame). On a host that updates the camera cache between tick end and the next `TickObjects`
  (a manual `UpdateCamera`, a controller with no camera manager) the commit re-evaluates with a different camera, and
  the tracker can count a positive the label never wrote (or the reverse). Simpler and order-proof: store the last
  tick-end evaluation and commit it unconditionally.
- **N2 [V] m57.** If the camera retreats after a distance arm on a mesh with ≥ 3 LODs, the held LOD becomes finer than
  the engine's. Those frames are plain negatives, and the revert release is a visible coarsening pop on an unflagged
  frame boundary. Release (or flag) when the engine coarsens past the held LOD.
- **N3 [V] m57.** `CollectArmableActors` runs on every auto-pool round while `lod_popping` is enabled: a
  `TObjectIterator<UStaticMeshComponent>` over the process, then per multi-LOD candidate within 100 m a scene-view
  prediction, up to 9 traces and a directional-light iterator. Unmeasured; the plan's `lod_predict_ms_*` readout was not
  built.
- **N4 [V] m57.** The shadow test uses the first shadow-casting directional light only and the AABB of the swept box: it
  over-refuses on real content and ignores point/spot light shadows. `ProjectBoxToNormalizedRect` drops corners behind
  the camera, which can miss a swept box that crosses the view.
- **N5 [V] m57.** The lod candidate path bypasses the census, the translucent-only exclusion, the coverage cull and the
  poll radius that every other id goes through. The m26 veto still removes zero-pixel targets.
- **N6 [V] m57.** `lod_unconfirmed` frames keep their mask PNG (deviation 7). That breaks m44's "mask only on labelled
  frames"; document it.
- **N7 [V] m57.** A hold-cap swap is inferred at the next frame even when the run ends or hits the frame cap right after
  the cap-reached frame; then `lod_swaps` names a frame that was never captured and the kind reads `hold_cap_only`
  instead of `no_swap`.
- **N8 [V] m57.** A lod event whose labelled frames were all dropped by the writer is listed as untriggered.
- **N9 [V] m57.** On the backbuffer path (`IAI.Capture.SVE 0`) no render record exists; `lod_unconfirmed` can never fire.
- **N10 [V] m57.** `HandleLodTriggerSetting` prints the ini key where it means the console command (cosmetic).
- **N11 [V] m57.** The positive rule has no contrast term (per plan): a held LOD with less than 4× triangle contrast to
  the engine's LOD is labelled positive.
- **N12 [V] m57.** The selector now injects `lod_popping` only while the camera moves; a refusal is shown as
  "0 matched" [R for the selector text].
- **N13 [R] m57 tools.** The untriggered-id excuse does not check the `lod_popping@` prefix (it can only mask an event
  with no labelled frames).
- **N14 [R] m59.** The mid-run switch read-back pairs the latched value with the new source; outside a capture the switch
  applies at once, not "at the next capture run" as the help says.

---

## What I checked and found correct (against the brief's 16 points)

1. **Positive rule timing [V].** Held / on screen / engine finer / coverage are all evaluated at `OnWorldTickEnd`
   (`SampleDeferredActiveState` → `IsFireLabelledThisFrame` → `EvaluateNow`) with the camera cache that
   `UpdateCameraManager` (`LevelTick.cpp:1621`) wrote for this frame, i.e. the render's camera. The held state at tick
   end is after this tick's `DecideTick`, which is what the render shows (forced-LOD change recreates the proxy in the
   same frame). The mid-tick pre-roll query reads the previous frame's cached evaluation in native order and a
   consistent hybrid in synthesized order; it moves only the window start, not labels. First frame: Apply frame is
   evaluated at tick end and committed next tick; camera cut and view-target change take effect at the same
   `UpdateCamera`, so label and render agree. Residual: N1.
2. **Engine-LOD prediction [V].** Calls the engine's own `ComputeStaticMeshLOD` / `ComputeTemporalStaticMeshLOD` with
   `r.StaticMeshLODDistanceScale` as `FactorScale` (the function multiplies by `LODDistanceFactor` itself, as the main
   pass does), MinLOD from component or mesh plus the first resident LOD, `r.ForceLOD` refused. `FLodViewScope` builds
   a family with no view extensions, so no `SetupView` side effects; the family deletes its view. Gaps: temporal state
   staleness (S1), split screen / second view not modelled (first player only).
3. **Swap frame [V].** Release at tick N → the frame rendered at N shows the engine's LOD; the swap is recorded on frame N
   (telemetry at tick end, commit at N+1 with the same input). The trigger reading the previous frame's camera delays
   the release by one frame but never mislabels: frame N−1 really showed the held LOD. The `LodReleaseLag` lever is a
   real can-fail for L2.
4. **Render-side record lifetime [V/R].** Probes are keyed by request id under a lock, taken once on the render thread,
   cleared at reset. The raw `FStaticMeshRenderData*` is read in the render of the same frame; mesh destruction is
   render-fenced by the engine [R]. Unrendered requests leave a map entry until run end (bounded). `lod_unconfirmed`
   cannot fire on a correct frame except on a view mismatch (safe direction); it is silent in the S1 cases.
5. **`lod_subvisible` [V].** Applied in the drain to telemetry `lod.subvisible`; transition-only, never labelled, no mask;
   counted. Holes: S2, N2.
6. **Hooks and burst extension [V].** Defaults (false / −1) leave every other anomaly's schedule byte-identical:
   `BurstHoldsEventOpen` is false unless a live lod fire holds open, and `ResolveBurstDeferredOnsetTimeout` returns the
   global 30 for `stuck_low_mip`. `stuck_mip_hold_timeouts` still counts stuck_low_mip only. Frame cap is checked before
   the phase switch; a 3000-frame safety bound exists; target lost → `Revert` and `IsAnomalyHoldingEventOpen` is false
   once inactive.
7. **Releases [V].** Visible releases are the trigger (recorded spatial), the hold cap (inferred `hold_cap`) or a revert
   while subvisible (frames flagged). Off-screen and matching releases are invisible. Revert restores `SavedForcedLod`,
   always 0 because a present forced LOD is refused. World teardown and target lost revert. Exceptions: N2, S1 (dither).
8. **Q4 [V].** No event with a labelled frame is removed except by the m26 veto; untriggered ids skip only the event, and
   their transition frames stay in the root union (`A.Transitions` is copied whole). Kinds and `lod_spatial_swap_visible`
   are right for spatial swap, turned away (no_swap), hold cap (hold_cap_only), never triggered (no event), run end
   (no_swap; edge N7). The hold-cap inference is sound in both tick orders.
9. **m26 with view entry [V].** Veto arms wait for labelled frames (deferred onset), so an off-screen armed target is never
   measured zero. A labelled target with zero drawn pixels is still vetoed, correctly (deviation 13).
10. **Selection and draw [V].** Candidates are sorted by name and drawn with the same seeded stream; deterministic for a
    deterministic camera. Shadow rule: N4. One component per fire on multi-mesh actors: S3.
11. **m56 interplay [V/R].** Pre-roll and extension frames are captured every tick; a swap is never a settle edge (settle
    roles come only from the settle phases); settle placement checks accept extension frames and reject settle on a swap
    frame [R, probe P4]. Reader faults: B3, S4. Kit: S5.
12. **`periodic` [V].** Reached only by name or a numeric second argument, refused without the bench gate
    (`periodic_bench_only`), never passed by the auto pool (it sends `=Target` only) or the selector. Behaviour equals
    the old timer apart from the declared keys (`lod.trigger`, `lod.state`, subtype `periodic`).
13. **Bench levers and camera path [V].** The five `IAI.Bench.Lod*` levers are inside `#if !UE_BUILD_SHIPPING` and carry the
    `IAI.Bench.` prefix that the bench gate masks (`AnomalyBenchGate.cpp:10, 84`). `lod_v1` lives in `AnomalyBench`
    (stripped from delivery), needs `-IAIBenchFixture`, the map allow-list and the input lock.
14. **m59 refusals [V/R].** Order HISM → skeletal (switch off) → single LOD; foliage derives from HISM, plain ISM is not
    refused [R]. No half-applied target: `lod_corruption` forces only accepted components and returns false only when
    none were; `lod_popping` forces nothing in Apply. Latch set at `BeginActualRun`, released at `FinishRun`; a run
    cancelled before focus never latches [R]. Other anomalies' targets and the seeded draw are unchanged [R].
15. **What m59 does not cover [V].** Single-LOD plain ISM: refused as `single_lod`. Visually identical LODs: not covered,
    no field to filter on. Streaming / MinLOD clamp: S10. Explicit index 1: S9. Nanite: refused by the default Nanite
    policy, not by m59 [R].
16. **Bench scripts [V/R].** B4–B6, S7, S8. `make_lod_fixture.py` deletes nothing [R].

**Not checked:** the 4.25 port (out of scope); the dashboard repo (whether any shipped surface still sends a numeric
second argument to `lod_popping`, which would now be refused as `periodic_bench_only`); I ran no selftest myself (the
sub-reviewers ran the Python ones: label rule 107, kit 103, annotation transitions 15, bench scripts 5/11/4, all OK).

---

## The 13 deviations (journal 095-10 §3)
1. **One component per spatial fire** — acceptable for v1, but only with S3 (scope the box, mask and observability).
2. **Release margin on the later of the two thresholds** — matches plan §3.1. It needs the camera to keep approaching
   after the first positive; the bench path does not (B2).
3. **Label at call time, commit next tick** — sound in both orders given `LevelTick.cpp` 1606 < 1621 < 1814. Prefer
   committing the stored tick-end evaluation unconditionally (N1).
4. **Hold-open sampled at tick end** — sound; makes the extension order-independent.
5. **Hold cap from the first positive frame** — fine; documented semantics.
6. **Hold-cap swap inferred by the capture** — sound in normal endings; edge N7.
7. **`lod_unconfirmed` frames keep a mask** — acceptable only if documented (N6).
8. **View ring fed from `DispatchAnomalyTicks`** — fine; it adds a one-frame lag to the motion estimate in native order,
   which contributes to B1.
9. **No `lod_refused_by_reason`** — acceptable; the yield tally carries it.
10. **Office kit NOT-JUDGEABLE** — acceptable only if the kit prints it and the readme says it (S5).
11. **Bench copies at (1700, ±60), start x = −600** — reject as built: the geometry makes C3/C4 unproducible (B2).
12. **m45 frame reader copied, not imported** — fine; the copy is not the problem (B4 is).
13. **m26 veto unchanged** — correct. Q4 is about events with no visible swap, not zero-pixel targets. Readme must say
    "except m26-vetoed events" (S6).

---

## Questions for chat
1. **B1/B2: bench retune only, or a product change too?** Recommendation: product change for B2 (look ahead to the
   coverage size, and start the arm timeout at the first wrong frame), plus a bench retune for B1 and B2, plus a
   predictions amendment before 095-07's m57 legs.
2. **The client kit cannot judge any lod_popping event.** Recommendation: say so in the readme now (S5); later, add a kit
   path that judges the recorded swap edge from pixels (the swap frame is a pixel edge even when the onset is not).
3. **Dithered meshes (S1).** Recommendation: flag in the safe direction first (probe and compare the swap-frame
   window), and only then consider waiting for temporal agreement before a release.
4. **m59 gaps S9/S10: fix in m59 now or a follow-up?** Recommendation: a small follow-up on the m59 branch before the
   client drop (they are pre-existing, but they are the same "label with no visible change" class m59 exists to remove).
5. **See-through occluders (S2).** Recommendation: flag those frames transition-only rather than negative.
