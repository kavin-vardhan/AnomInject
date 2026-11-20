# 093-14 predictions: drop-aware camera controls and full J2 rerun

Authority: request 093-14 and ruling 093-13 AB1-AB5. Product is frozen at
978103163c0a1839bd6948614818b73f07674c72. No product or fixture content change.
Worktree E:/IA_BuildCache/_r933/AnomalyInjector; new evidence E:/IA_BuildCache/_r9314.
This freeze is committed before any new assessment or engine leg.

## AB1 camera control donor, attempt 1 of 2

For each scripted switch, use the last written sample before the switch, taken
from this leg's label/image membership receipts. The donor is max(written indices
< switch), not a fixed offset and not the last labelled row selected to help the
test. Require a matching written image and a donor within the inclusive range
[switch-10, switch-1]. No written donor in that window is a STOP: control unavailable
must never become PASS. If the selected written row cannot supply the projected
box, that control is also unavailable and stops; do not search earlier boxes.

For each cut/blend control, copy the measured geometry and replace projected
boxes in the existing 44-sample post-switch window (switch+1 through switch+44)
with the donor box. This applies the existing blend-control construction to each
switch as AB1 requires. Use the unchanged office PF-2 estimator and thresholds.
Both cut and blend stale-projection controls must reject; missing data is not a
substitute for a constructed control. Record donor index, distance, image/row
receipts, mutated row count and per-axis counts/scales. Preserve input captures
and the original _r9313 control-unavailable result byte-for-byte.

Re-assess the retained Stack active/reference sessions, without an engine rerun.
Write results and copied-control geometry under _r9314, never over _r9313. All
positive camera checks remain unchanged: >=3 independently visible events,
zero onset/recovery errors, measured masks on every labelled row, full/moving
PF-2 agreement, correct frame/world identity, annotation identity and W2 drops.
The identity-self negative must still reject. CPU donor-selector examples cover
pending-three drops, a donor exactly 10 samples back, and no donor within 10;
they do not replace the retained-session or in-engine controls.

## AB2 order and run list

1. Re-assess retained Stack PIE camera with AB1; then Lyra PIE reference and active,
   same 60 cut / 300 blend / 1.5-second spec. Every Lyra leg uses the relinked
   fixture DLL 017ca90bdffd2b79b30fa7a1e262c9f747c0070aece0608ba76177e85455edb3.
2. PF-1 (exact Lyra 942x869 Y off-left): zero visible rows and generic/texture
   applications, >=1 off-screen refusal, known projection; old four-fire and
   96-false-row captures and generic-apply mutant reject.
3. PF-2: both fixtures x X/Y x 942x869,1123x870,1280x720. Original marker-OFF
   repaired specs, process-local constraint only. 360/360, >=3 visible events,
   zero edges, all labelled masks measured, per-axis [0.990,1.010], .96 mutants.
   Archived f608 box negatives remain the controls; no candidate result is inherited.
4. PF-3: Stack UV/proxy at the retained 1280x992 viewport recipe, comparator to
   f608 sessions unchanged: same events/targets/edges, identical visible_positive,
   bbox <=0.5 px. Retain one-pixel/onset mutants. Original viewport_r1 specs apply.
5. PF-4 Stack letterbox: correct measured boxes or counted refusal; mapping/scale
   negatives unchanged. PF-5 Stack locked70: fovDeg70, usual PF-2/mask/edge floors,
   observed old cache-route90 negative. These retain their original fixture scope.
6. All 42 candidate PIE cases from the 84-case ledger, listed below, using original
   predicates/can-fails and the already-adopted historic pairing aliases.
7. Supplemental admission probes on both fixtures and office-reader self-tests.
8. Dedicated TSR50 PF-2 on both fixtures, the frozen 093-13 arrangement/estimator:
   well-inside target, historic TSR, 1280x720, Stack X/Lyra Y, marker OFF, 360/360;
   >=20 qualifying rows per axis and scale [0.990,1.010], per-axis .96 rejection.
9. Cook; then every staged gate on both fixtures, including cameras, all 38 staged
   ledger cases/controls, four PF-2 cases, two dedicated TSR50 cases, staged PF-3
   comparisons and admission probes. -nodebuginfo only with staged EXE/DLL hashes
   equal to the 9781031 candidate manifest. Push feature branch only if all green.

AA3 item-4 identity inheritance is superseded, not relaxed or silently skipped.
No byte/field identity comparison is used to transfer old candidate passes.
Only the four exact archived-DLL PIE controls below stand: they never load the
candidate product. The retained Stack camera is re-assessed as explicitly ordered.

| Fresh PIE case | Source spec |
|---|---|
| stack_P_delayed | _r9314/specs/stack_P_delayed.json |
| stack_P_healthy | _r9314/specs/stack_P_healthy.json |
| stack_P_multi | _r9314/specs/stack_P_multi.json |
| stack_P_selected | _r9314/specs/stack_P_selected.json |
| stack_P_client0 | _r9314/specs/stack_P_client0.json |
| stack_P_client1 | _r9314/specs/stack_P_client1.json |
| stack_P_resize | _r9314/specs/stack_P_resize.json |
| stack_P_ambiguous | _r9314/specs/stack_P_ambiguous.json |
| stack_P_split | _r9314/specs/stack_P_split.json |
| stack_P_stereo | _r9314/specs/stack_P_stereo.json |
| stack_P_controller | _r9314/specs/stack_P_controller.json |
| stack_P_gameview | _r9314/specs/stack_P_gameview.json |
| stack_P_showflag | _r9314/specs/stack_P_showflag.json |
| stack_P_reg_uv | _r9314/specs/stack_P_reg_uv.json |
| stack_P_reg_proxy | _r9314/specs/stack_P_reg_proxy.json |
| lyra_P_delayed | _r9314/specs/lyra_P_delayed.json |
| lyra_P_healthy | _r9314/specs/lyra_P_healthy.json |
| lyra_P_multi | _r9314/specs/lyra_P_multi.json |
| lyra_P_selected | _r9314/specs/lyra_P_selected.json |
| lyra_P_client0 | _r9314/specs/lyra_P_client0.json |
| lyra_P_client1 | _r9314/specs/lyra_P_client1.json |
| lyra_P_resize | _r9314/specs/lyra_P_resize.json |
| lyra_P_ambiguous | _r9314/specs/lyra_P_ambiguous.json |
| lyra_P_split | _r9314/specs/lyra_P_split.json |
| lyra_P_stereo | _r9314/specs/lyra_P_stereo.json |
| lyra_P_controller | _r9314/specs/lyra_P_controller.json |
| lyra_P_gameview | _r9314/specs/lyra_P_gameview.json |
| lyra_P_showflag | _r9314/specs/lyra_P_showflag.json |
| lyra_P_reg_uv | _r9314/specs/lyra_P_reg_uv.json |
| lyra_P_reg_proxy | _r9314/specs/lyra_P_reg_proxy.json |
| stack_P_align_uv | _r9314/specs/stack_P_align_uv.json |
| stack_P_align_proxy | _r9314/specs/stack_P_align_proxy.json |
| stack_P_pair_historic_native_100 | _r9314/specs/stack_P_pair_historic_native_100.json |
| stack_P_pair_historic_native_50 | _r9314/specs/stack_P_pair_historic_native_50.json |
| stack_P_pair_historic_synth_100 | _r9314/specs/stack_P_pair_historic_synth_100.json |
| stack_P_pair_historic_synth_50 | _r9314/specs/stack_P_pair_historic_synth_50.json |
| lyra_P_align_uv | _r9314/specs/lyra_P_align_uv.json |
| lyra_P_align_proxy | _r9314/specs/lyra_P_align_proxy.json |
| lyra_P_pair_historic_native_100 | _r9314/specs/lyra_P_pair_historic_native_100.json |
| lyra_P_pair_historic_native_50 | _r9314/specs/lyra_P_pair_historic_native_50.json |
| lyra_P_pair_historic_synth_100 | _r9314/specs/lyra_P_pair_historic_synth_100.json |
| lyra_P_pair_historic_synth_50 | _r9314/specs/lyra_P_pair_historic_synth_50.json |

Archived-DLL exceptions: stack_P_j1_delayed, stack_P_release_multi, lyra_P_j1_delayed, lyra_P_release_multi.


## Unchanged floors, readings and stop rules

Healthy cases retain v>=3, zero frame edge error, every labelled row with measured
masks, and w=r. Intentional refusal cases retain their original zero-write/count
predicates. Resize/camera use W2: r=w+named drops, one retirement per missing sample,
no written/dropped overlap, no unexplained/pending result and zero accepted stale
or mismatched frame/world samples. Other original pixel, mask and box floors stay.
Record both admission counters per leg; no field is added or changed.

AB3-C: ExposureExclusionPx is only the existing DRAWN SUMMARY log reading on each
resize leg. It is not a run_summary key. No JSON writer change is authorized.
AB3-D open item after J2: check whether m55 change evidence validates under camera
motion. Record extent_mismatch counts on reference and active camera legs; this
reading is not a J2 gate and its correction is outside this job.

Pairing uses the historic recipe, unchanged 54928b2e reader, 40 px tolerance,
decidable floor 1 and PREVIOUS=NEITHER=0, with shifted-mask rejection. Z2 parts 1,
2 and 4 remain for 50% pairing; dedicated AA4 legs carry non-vacuous box agreement.
Capture every request at half internal extent, consistent output picture/mask/label
sizes; the 100%-log and wrong-size controls reject. Report coverage and speed_ratio.

Pairing recipe: closed. PREVIOUS > 0 is a STOP. NEITHER > 0 is a STOP, after taking
the Z3 spill readings on that same leg. Marker: spent. Packaging metadata: spent.
Camera control donor: at most 2 attempts. A control that cannot be built is a STOP,
never a pass. A camera-switch product failure is a STOP. A TSR half-resolution leg
with fewer than 20 qualifying rows on an axis is a STOP. Otherwise D2 is unchanged.
No product/fixture change, reader/tolerance/predicate change beyond AB1, or repeated
leg until it passes. No new leg before the idle gate; all restoration by receipt.
