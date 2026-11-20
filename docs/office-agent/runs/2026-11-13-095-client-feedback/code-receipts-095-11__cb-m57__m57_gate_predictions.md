095-10 predictions for m57 (lod_popping spatial trigger) - FROZEN 2026-11-14 before any engine leg
Branch feat/m57-lod-spatial-trigger (written, Editor-compiled, pure and Python tests only). No leg has run.
Source of truth: _reviews\095-08-plan-B-lod-spatial-trigger.md section 9, AMENDED for ruling 095-08 (Q4 keep-and-mark,
G1-G4, H3) and for the as-built defaults below. A leg that disagrees is reported as it reads; no gate is re-cut after it.

0. Common recipe
- Packaged 1280x720, 30 fps paced, native tick order, CB_LodFixture, the m57 exe by code-only swap on top of 095-07's cook
  (which must carry the two fixture options of section 6).
- Pixel gates at the AA-off identity arbiter (r.AntiAliasingMethod 0, Lumen/GI/reflections off). A luma premise on the
  first fixture leg (G151, G402) before any reading.
- -IAIBench -IAIBenchFixture; IAI.Bench.InputLock 1; IAI.Bench.CameraSchedule lod_v1 <path>; every attempt banked.
- Pose read-back: every 'IAI-CC-SCHEDULE FRAME' line on a lod_v1 leg has err_cm < 0.5 and err_deg < 0.1, or the leg is
  INVALID (A63 shape), never FAILED.
- Defaults as built: armTimeout 90, holdCap 150, entryFrames 4, downPop 1, coverage_min 7.0 %, contrast ratio 4,
  release margin 10 % on the later of (contrast switch, first positive screen size), hysteresis 0.85, closing speed
  >= 50 cm/s within 30 deg, view-entry margin 20 deg at >= 30 deg/s, ahead <= 90 frames.
- Targets: turn paths =CB_LodFix_N / =CB_LodFix_D (scale 4.6 at (850, -/+340)); distance paths =IAIBenchLodCopyN /
  =IAIBenchLodCopyD (runtime scale-1 copies at (1700, -/+60)) or =CB_LodFix_N2 / =CB_LodFix_D2 (second pair, LOD1 at
  screen size 0.3, at (1700, -/+200), once the cook carries it). Camera start x=-600 on distance paths, x=0 on turns.

1. Look-first clips (G4, before any gate) - predictions for what the owner will see
C1 turn_in, =CB_LodFix_N view_entry: one event, kind spatial_swap, lod_trigger view_entry; the blue cube enters from the
   right edge; frames while partly in view with coverage < 7 % carry lod_subvisible; exactly 4 positive frames, then the
   swap frame shows the orange sphere; lod_swaps = [{cause view_entry, direction up, on_screen true}].
C2 turn_in, =CB_LodFix_D view_entry: the same; no crossfade at the snap (forced LOD disables dithering; temporal samples
   agree at a fixed distance).
C3 approach, =IAIBenchLodCopyN distance_threshold: armed during the first moving burst (frozen at LOD1, invisible); a
   long lod_subvisible stretch from ~17 m to ~3 m; positives from coverage 7 % (~3 m) for ~3-5 frames (the 10 % margin
   on the first-positive size at 7.5 cm/frame), then the swap. With =CB_LodFix_N2 (second pair): the engine switch is
   near 3 m, so the positive span starts with the switch and the subvisible stretch is short.
C4 approach_retreat, =IAIBenchLodCopyN distance_threshold, IAI.Capture.Config 2 4 120 14 0 (a window that spans the
   retreat): an up-swap (distance, up), then on retreat past 0.85 of the swap screen size a down-swap (distance, down)
   followed by positive frames until the engine coarsens back to LOD1, then an invisible release (no third swap).
   With IAI.Anomaly.LodDownPop 0: the up-swap only.
C5 still, =IAIBenchLodCopyN distance_threshold: no event; every burst refuses with lod_not_armable:not_approaching
   (no fire, so lod_untriggered 0 = fires 0).
C6 approach, =IAIBenchLodCopyN periodic 8 (bench gate open): today's timer, subtype periodic, kind periodic.

2. Gates (plan section 9 table, amended)
L0  null vs null per path: 0 frames differ at AA-off.
L1  m57_null_pair.py on turn_in N and D, approach N (copy) and N2, approach_retreat N: labelled frames == frames where
    anomaly != null inside the target box, ignoring lod_subvisible; onset 0, offset 0. Can-fail: the tool's selftest
    (label shifted by one fails); and IAI.Anomaly.LodMinCoverage 100 on turn_in N gives 0 positives and 0 events
    (untriggered, its id listed).
L2  m57_null_pair.py: every pixel-edge swap frame is the first null-equal frame after a labelled run. Can-fail:
    IAI.Bench.LodReleaseLag 1 fails L2 (the recorded swap is one frame before the picture changes).
L3  m57_lod_readout.py --expect no_events on still (targeted, and auto-pool with only lod_popping enabled): 0 lod events;
    auto-pool: every round reads lod_not_armable in the yield tally, 0 fires. Can-fail: IAI.Bench.LodForceTrigger 1 on
    still gives >= 1 event (--expect events).
L4  turn_away, =CB_LodFix_N view_entry: armed off screen, never enters: 0 labelled frames, no event, its id in
    lod_untriggered_event_ids, lod_arm_timeouts 1 per armed burst. Can-fail: IAI.Bench.LodSkipOnScreen 1 gives
    labelled off-screen frames.
L4b m57_null_pair.py on the L4 leg and on the armed-off-screen stretch of turn_in: anomaly == null over the whole frame.
L5  every leg: run_summary lod_render_mismatch_frames 0, lod_render_checked_frames > 0 on legs with labelled lod frames,
    no lod_unconfirmed entry. Can-fail: IAI.Bench.LodPredictStaleView 1 on approach gives mismatches > 0.
L6  every leg plus a destroy leg (IAI.Bench.DestroyTarget while armed): every 'lod_popping: REVERT' line reads RESTORED
    (forced_lod_model read back equal to the saved value). Can-fail: IAI.Bench.LodSkipRestore 1 reads NOT RESTORED.
L7  periodic 8 on CB_LodFix_N static camera, m57 exe vs integ exe, both orders: m56 non-settle comparator, every field
    equal except the declared m57 keys (entry lod.trigger, lod.state; event anomaly_subtype periodic, lod_* event keys;
    label_schema_minor 3; run_summary lod_* block and the two label_transition_lod_* keys).
L8  blinking and missing_texture targeted on CB_GateLevel, m57 exe vs integ exe: the same comparator; no lod key on their
    entries or events; run_summary differs only by label_transition_lod_subvisible_entries 0 and
    label_transition_lod_unconfirmed_entries 0 (no lod_* block: no lod fire ran).
L9  from L1: every labelled frame masked; MASK-TIE 0 mismatches; no mask on swap frames.
L10 L1 legs in synthesized order (label gate only, G230): m56 continuity check passes with the extension frames; labels
    equal native order frame for frame.
L11 L1 legs at delivered AA (TSR): reported, not gated - frames of residue after each swap.

3. Q4 (ruling 095-08) - kept and marked, never dropped
L12a turn_in N with IAI.Anomaly.LodEntryFrames 30 and IAI.Anomaly.LodHoldCap 10: one event KEPT in annotation.json with
     ~10 labelled frames, lod_event_kind hold_cap_only, lod_spatial_swap_visible false, lod_swaps = one hold_cap swap at
     the frame after its last lod frame; run_summary lod_events_by_kind.hold_cap_only 1, lod_swaps_by_cause.hold_cap 1;
     verify_capture.py --label-rule LOD-KEYS passes.
L12b no_swap (visibly wrong, then unseen before any release): no lod_v1 path produces it (turn_away never enters);
     covered offline only (label-rule selftest lod_kept_no_swap_event_marked_clean). Stated, not claimed.
L12c every spatial lod event in every leg: verify_capture.py --label-rule passes LOD-KEYS and ANNOTATION-TRANSITIONS
     (untriggered ids excused only for settle / lod_subvisible / lod_unconfirmed entries).

4. Untouched (L7, L8 read-backs predicted)
- No other anomaly's membership, telemetry, timing or events change; the Positives phase is byte-identical when no live
  fire holds the event open (every anomaly but lod_popping returns false); the pre-roll timeout for stuck_low_mip is
  unchanged (it returns -1); stuck_mip_hold_timeouts counts stuck_low_mip only.
- The swap frame is never a settle frame: m56 flags only the K ticks after a burst fire or revert.

5. Yield (reading, not gated)
- StackOBot MainWorld auto-pool, 600 frames, unattended (camera settles): lod_popping fires 0 (no motion), every round
  lod_not_armable (not_approaching or not_turning_toward); other enabled ids fill the bursts.

6. Fixture options the heavy brief runs BEFORE its cook (CaptureBench m57-gates, make_lod_fixture.py, editor commandlet)
- IAI_LODFIX_ADD=second_pair: SM_LodFix_N2 / SM_LodFix_D2 (screen sizes [1.0, 0.3]) and actors CB_LodFix_N2 /
  CB_LodFix_D2 at (1700, -/+200, 200), scale 1, no shadow. Predicted read-back: 2 actors, names equal labels.
- IAI_LODFIX_ADD=instanced: CB_LodFix_ISM (one InstancedStaticMeshComponent instance of SM_LodFix_N, scale 2, at
  (1200, -700, 600)) and CB_LodFix_HISM (one HierarchicalInstancedStaticMeshComponent instance, at (1200, 700, 600)).
  Predicted read-back: each actor one component of the named class with one instance. m59 leg L59-5: lod_corruption on
  =CB_LodFix_HISM refuses hism_ignores_forced_lod; on =CB_LodFix_ISM it applies and the picture changes.
- Both are additive: the content diff is exactly the new assets plus CB_LodFixture.umap.
