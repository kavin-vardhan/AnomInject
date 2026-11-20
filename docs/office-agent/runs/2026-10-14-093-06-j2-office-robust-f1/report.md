The drop-aware camera controls and all 42 fresh PIE ledger cases passed on unchanged product 9781031.  
Qualification stopped at the dedicated TSR setup reader: it confuses target-mask serials with colour sample indices.  
Hosts and staging are restored; cook, staged gates and push remain unrun, and the candidate remains UNQUALIFIED.

**Preflight, scope and commits.** Work used `E:/IA_BuildCache/_r933/AnomalyInjector`, branch `fix/followup-capture`, initially clean at `0bc6e9f46cc2d15ddee44548db62f31757d54783`. Both candidate banks and their archived copies rehashed identically: both game EXEs, ten product DLLs, four helper DLLs and both fixture DLLs. Lyra's relinked fixture hash is recorded below. Original host files/content and all 5,764 live-staging files matched the preceding restoration receipt. Git author/committer date overrides were absent. The preceding report remains preserved as `report-093-13.md`. [Preflight](E:/IA_BuildCache/_r9314/preflight.json).

| Local commit | Time, IST on 2026-10-16 | Purpose |
|---|---|---|
| `741e8a1836754116fd890ed095542d46cf185011` | 03:14:27 | Freeze AB1 donor, AB2 complete PIE run list and AB3-C log reading |
| `334d9033c5166bf4e63b593aef487245b6751e31` | 06:34:01 | Results, scope stop, journal, architecture, status and G487 |
| `0e50b56df4baf516ec815f433a93084388e2cf05` | 06:48:07 | Archive destination routing repair and retained partial-attempt receipt |

The retained Stack camera reassessment began **03:15:18.604**, and the first fresh engine leg began **03:23:50.336**, after the freeze. The product remains `978103163c0a1839bd6948614818b73f07674c72`; no product, fixture-content or office-reader source changed. **Heads pushed: none.** Remote remains `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`. [Predictions](E:/IA_BuildCache/_r933/AnomalyInjector/docs/predictions/093-14-camera-donor-full-pie.md), [journal](E:/IA_BuildCache/_r933/AnomalyInjector/docs/sessions/2026-10-16-093-14-camera-donor-full-pie.md).

**The stop and required decision.** The first dedicated TSR50 leg, Stack PIE session `062725`, wrote **360/360** pictures. All **96/96 labelled rows** have measured paired masks. Box agreement passes with **X=0.9961088900, Y=0.9944751381, n=96 each**; both 0.96 axis controls reject. All pictures, masks and label dimensions are **1280x720**. Every one of the **294 logged mask-pass lines** reports internal **640x360**, with zero bad extents.

The full frozen gate nevertheless **FAILS**. `assess_z2.reduced` expects `(1<<61)|(session_index+1)` for every colour row. Product `ArmTargetMaskOwn` instead allocates `GTargetMaskRequestBit | (++TargetMaskOwnSerial)` only when it arms a target mask, then records the actual request-to-session mapping. This ordinary capture has serials **1-96**, with serial **1 at sample 29**, not sample 0. The reader therefore reports **264 missing expected IDs**, nominally samples 96-359. Pairing's probe forces an arm per sample, so its serial/sample coincidence does not generalize to this fixture.

This is an evidenced harness assumption, not a measured wrong box, inconsistent output size or new crash. The failed outcome remains unchanged. AB1 authorizes only the camera donor repair; correcting this separate receipt matcher would change a reader beyond the authorized scope. No retry, altered predicate or further engine leg followed. **NEEDS-DECISION: authorize a defined reduced-render receipt proof for the dedicated sparse-mask TSR fixture, preserving the frozen estimator, per-axis n>=20 and 1% limits.** [Failed assessment](E:/IA_BuildCache/_r9314/legs/stack_P_tsr50_pf2/tsr-box-assessment.json), [source/log diagnosis](E:/IA_BuildCache/_r9314/stop-readings09314.json).

**Camera repair and results.** AB1 attempt **1 of 2** selects the last written sample before each switch from that leg's receipts, requiring distance <=10. It does not choose a more convenient labelled donor. Missing/unusable donors stop the control. CPU checks covered actual donors, the inclusive ten-sample boundary, eleven samples and no donor. The original unavailable-control result and both retained Stack sessions rehash unchanged.

The only reader edit is the scratch [camera_assess.py](E:/IA_BuildCache/_r9314/camera_assess.py), recorded in [the AB1 diff](E:/IA_BuildCache/_r9314/camera-donor-ab1.patch): old SHA-256 `d8e6d854d9b0b684fe2bbc654404f44becd8a8e30d79f4504c8d6e40ff55073d`, new `bd5892efb1977830615652e8d4c0057f40cc664abf247a9e5280aae70652569e`. No estimator or tolerance changed.

| Camera leg | Result | r=w+drops | Measured labelled masks | Moving X/Y, n per axis | O/N |
|---|---|---|---|---|---|
| Stack PIE reference, retained | PASS, no rerun | 990=984+6 | 714 | Reference check PASS | 0/0 |
| Stack PIE active, reassessed | PASS | 990=984+6 | 714/714 | 1.000729985 / 1.000127131, n44 | 0/0 |
| Lyra PIE reference, fresh 032444 | PASS | 990=984+6 | 714 | Reference check PASS | 0/0 |
| Lyra PIE active, fresh 032822 | PASS | 990=984+6 | 714/714 | 1.000097912 / 0.999884435, n44 | 0/0 |
| Stack staged reference/active | NR | NR | NR | NR | NR |
| Lyra staged reference/active | NR | NR | NR | NR | NR |

`O/N` means `targeted_refused_off_screen / targeted_refused_no_projection`; `NR` means not run, not zero. Each active camera leg has four visible events and **zero onset/recovery error**. Full-frame PF-2 reads approximately 1.000/1.000, n714 per axis. Cut-window positive scales are Stack **1.001439/1.001198**, Lyra **0.999267/1.002353**, n44 each. The identity-self and stale-projection controls reject.

| Fixture/switch | Last written donor; distance | Stale-copy X/Y | Control |
|---|---|---|---|
| Stack cut60 | 56; 4 | 0.920863 / 1.077844 | Reject |
| Stack blend300 | 296; 4 | 1.044657 / 0.960920 | Reject |
| Lyra cut60 | 56; 4 | 0.937729 / 1.058824 | Reject |
| Lyra blend300 | 296; 4 | 1.033333 / 0.970948 | Reject |

Each control has 44 qualifying rows per axis. Both reference and active runs drop **three pending samples per switch**: 57-59 at the cut, 297-299 at the blend, all named `owner_render_missing`. Unexplained drops, written/dropped overlap, pending-at-stop and accepted stale/frame/world mismatches are **0**. In each fixture, the event active at the cut and the event active at the blend both continue: cut events end at sample208, blend events at448, with annotation `pie_end_settle:1`; no event-ID-specific end-log match was found. All four revert receipts per active leg report exact restoration, zero residue. Both reference and active legs on both fixtures record **16 change-stage extent_mismatch pairs**, including four before the cut. AB3-D remains the post-J2 open item to check whether m55 change evidence validates under camera motion; it is not a J2 gate.

**PF gates and the full PIE/staged matrix.** The new binary ran **64 fresh engine legs**, plus the retained Stack camera reassessment. All **42 fresh candidate PIE ledger cases** pass; the four exact archived-DLL PIE controls are the only carried matrix results. Thus the 84-case ledger is **46 PIE PASS / 38 staged NR**. This does not override the separate TSR setup failure. Identity inheritance is withdrawn under AB2.

| Gate | Fresh PIE result | Staged result | Can-fail |
|---|---|---|---|
| PF-1, exact Lyra off-left arrangement | PASS: 360/360, visible rows0, generic/texture applies0, O/N4/0 | Not separately specified | Retained four-fire and 96-false-row captures reject; generic-apply mutant rejects |
| PF-2, both fixtures x X/Y x three sizes | 12/12 PASS: each360/360, v4/e0, n96/axis, O/N0/0 | Four1280x720 cases NR | Both0.96 axis mutants reject on every leg; old Lyra0.967/0.813 reject, Stack passes |
| PF-3, Stack UV/proxy | PASS: each1152/1152, v4/e0, identical events/targets/visible_positive, maximum box delta0.0px; O/N0/0 | NR | One-pixel and onset/visibility mutants reject |
| PF-4, Stack letterbox | PASS: 360/360, v4/e0, n96/axis, O/N0/0 | Not separately specified | Full-window mapping and scale mutants reject |
| PF-5, Stack FOV lock70 | PASS: every fovDeg70;360/360,v4/e0, box agreement; O/N0/0 | Not separately specified | Observed old-route cache90 rejects locked70; scale mutants reject |
| Dedicated TSR50 Stack | **FAIL setup**, box check PASS: X n96/.9961088900; Y n96/.9944751381; O/N0/0 | NR | Both0.96 copies and retained100% log reject |
| Dedicated TSR50 Lyra | NR after stop | NR | NR |

Ordinary PF-2 axis scales are approximately **1.000000/.99999998** at942x869, **1.000356/.998770** at1123x870 and **.99999994/1.000000** at1280x720, on both fixtures and constraints. PF-3 retains the original1280x992 viewport setup; its 0.5px threshold never changed. PF-4 records the constrained copied rect [0,92,1280,536], camera aspect2.39. The exact archived f608d56 replay remains **X=.8130053613/Y=.8130053403, n96, b813**, rejected; it was re-read without another engine replay or input changes.

The frozen PF-2 estimator uses paired measured masks on labelled non-transition rows with valid dimensions/counts/bounds and finite boxes at least one pixel inside the picture. Each axis requires a mask-centre displacement of at least5% of its picture dimension. Its estimate is the median projected-centre/mask-centre displacement ratio about the picture centre, without trimming or refitting. Both axes need at least20 rows and raw scale within[.990,1.010].

Matrix cells give `r/w; v/e; O/N`, or C/P/N for pairing. `NA` means the counter is absent in the archived build; `N/A` means the case is inapplicable. Refusal cases intentionally write zero. Synthetic alignment tests identity/membership; native companions test RGB visibility and zero edges. Each native UV/proxy leg also passes ten rejecting controls with unchanged originals. The can-fail column states the control; all completed cases' controls meet their expected rejection.

| Case | Stack PIE | Lyra PIE | Stack staged | Lyra staged | Can-fail specification |
|---|---|---|---|---|---|
| Delayed viewport | PASS 360/360; v4/e0; 0/0 | PASS 360/360; v4/e0; 0/0 | NR | NR | Exact J1 completeness negative |
| Healthy single view | PASS 360/360; v4/e0; 0/0 | PASS 360/360; v4/e0; 0/0 | NR | NR | Wrong-view/world/frame controls |
| Second realtime editor view | PASS 360/360; v4/e0; 0/0 | PASS 360/360; v4/e0; 0/0 | N/A | N/A | Exact pre-F1 wrong-view control |
| Selected viewport | PASS 360/360; v4/e0; 0/0 | PASS 360/360; v4/e0; 0/0 | N/A | N/A | Cross-world binding negative |
| Two-client client0 | PASS 360/360; v4/e0; 0/0 | PASS 360/360; v4/e0; 0/0 | N/A | N/A | Cross-world binding negative |
| Two-client client1 | PASS 360/360; v4/e0; 0/0 | PASS 360/360; v4/e0; 0/0 | N/A | N/A | Cross-world binding negative |
| Resize | PASS 360/358; v4/e0; 0/0 | PASS 360/358; v4/e0; 0/0 | NR | NR | Missing reason/receipt and overlap mutants |
| Same-world ambiguity | PASS 360/0; v-/e-; 0/4 | PASS 360/0; v-/e-; 0/4 | NR | NR | Forced ambiguity refused/count |
| Split-screen | PASS 360/0; v-/e-; 0/4 | PASS 360/0; v-/e-; 0/4 | NR | NR | Forced split refused/count |
| Stereo | PASS 360/0; v-/e-; 0/0 | PASS 360/0; v-/e-; 0/0 | NR | NR | Forced stereo refused/count |
| Negative controller ID | PASS 360/0; v-/e-; 0/0 | PASS 360/0; v-/e-; 0/0 | NR | NR | Controller sub-reason |
| Non-game-thread view | PASS 360/0; v-/e-; 0/0 | PASS 360/0; v-/e-; 0/0 | NR | NR | Game-thread-view sub-reason |
| Game show flag off | PASS 360/0; v-/e-; 0/0 | PASS 360/0; v-/e-; 0/0 | NR | NR | Show-flag sub-reason |
| Native UV regression | PASS 1152/1152; v4/e0; 0/0 | PASS 1152/1152; v4/e0; 0/0 | NR | NR | Timing/mask/marker/drop/pixel defects |
| Native proxy regression | PASS 1152/1152; v4/e0; 0/0 | PASS 1152/1152; v4/e0; 0/0 | NR | NR | Timing/mask/marker/drop/pixel defects |
| Synthetic UV alignment | PASS 1152/1152; events4; 0/0 | PASS 1152/1152; events4; 0/0 | NR | NR | Mask/event/donor-barcode mutants |
| Synthetic proxy alignment | PASS 1152/1152; events4; 0/0 | PASS 1152/1152; events4; 0/0 | NR | NR | Mask/event/donor-barcode mutants |
| Native pairing 100% | PASS 80/80; C56/P0/N0; 0/0 | PASS 80/80; C57/P0/N0; 0/0 | NR | NR | Shifted-mask copy |
| Native pairing 50% | PASS 80/80; C55/P0/N0; 0/0 | PASS 80/80; C59/P0/N0; 0/0 | NR | NR | Shifted mask, 100% log, wrong size |
| Synthetic pairing 100% | PASS 80/80; C57/P0/N0; 0/0 | PASS 80/80; C56/P0/N0; 0/0 | NR | NR | Shifted-mask copy |
| Synthetic pairing 50% | PASS 80/80; C62/P0/N0; 0/0 | PASS 80/80; C57/P0/N0; 0/0 | NR | NR | Shifted mask, 100% log, wrong size |
| J1 delayed negative | PASS retained 360/0; v-/e-; NA/NA | PASS retained 360/0; v-/e-; NA/NA | NR | NR | Archived binary fails completeness |
| Pre-F1 wrong-view negative | PASS retained 360/360; v0/e-1; NA/NA | PASS retained 360/360; v0/e-1; NA/NA | NR | NR | Archived binary fails view identity |

Second-view wrong-view counts are zero; Stack/Lyra non-owner rejection readings are **2479/2504**. Each fixture counts ambiguity730, split366 and stereo728. NotGameView sub-counts are Stack controller/game-thread/show-flag **1067/1067/1067**, Lyra **1067/1067/1068**. Exact J1 controls write0/360; pre-F1 controls show the wrong view on360/360. Their archived-DLL receipts and inputs were rehashed, not rerun.

Both fresh resize legs reconcile **360=358+2 owner_render_missing**, samples98-99, with zero stale/mismatched acceptance, overlap or unexplained loss. After sample100, pictures are960x720, with no old-size result accepted. Four visible events remain zero frames off; PF-2 has n96 per axis. Stack scales are **.999999837/.999999867**, Lyra **.999997529/.999997529**. Both DRAWN SUMMARY logs read **exposure exclusion2304px**; this is a log reading, not a summary field. Missing-receipt/reason and overlap mutants reject. Every other healthy matrix leg keeps w=r. Both supplemental PIE admission probes pass360/360,v4/e0,O/N0/1, with zero-counter/application controls rejected; both staged probes are NR.

**Pairing and Z2.** Reader SHA-256 remains `54928b2ecce48d97a1af4bea16b00184ca091e8b22670fe68e8e3165671f3525`, tolerance40px, decidable floor1, PREVIOUS=NEITHER=0. Historic host AA/TSR, Lumen/GI/reflection settings remain in use. Coverage is median bright-magenta pixels inside the current mask divided by mask area on the reader grid; speed_ratio is the existing wall-span/game-span reading.

| Fixture/mode/type/% | C/P/N; decidable/floor | Shifted C/P/N; result | Coverage; speed_ratio | Z2 50% parts 1/2/4 | O/N |
|---|---|---|---|---|---|
| stack/P/native/100 | 56/0/0; 56/1 | 6/51/0; reject | 0.682903; 1.724386 | N/A | 0/0 |
| stack/P/native/50 | 55/0/0; 55/1 | 7/48/0; reject | 0.688416; 1.750797 | PASS; 80 lines/80 samples; 80 pictures/68 masks | 0/0 |
| stack/P/synth/100 | 57/0/0; 57/1 | 6/51/0; reject | 0.681583; 1.751763 | N/A | 0/0 |
| stack/P/synth/50 | 62/0/0; 62/1 | 7/56/0; reject | 0.689737; 1.776731 | PASS; 80 lines/80 samples; 80 pictures/69 masks | 0/0 |
| lyra/P/native/100 | 57/0/0; 57/1 | 8/48/0; reject | 0.687197; 1.815675 | N/A | 0/0 |
| lyra/P/native/50 | 59/0/0; 59/1 | 8/50/0; reject | 0.693059; 1.835268 | PASS; 81 lines/80 samples; 80 pictures/65 masks | 0/0 |
| lyra/P/synth/100 | 56/0/0; 56/1 | 8/47/0; reject | 0.686931; 1.825061 | N/A | 0/0 |
| lyra/P/synth/50 | 57/0/0; 57/1 | 8/48/0; reject | 0.695056; 1.896440 | PASS; 81 lines/80 samples; 80 pictures/65 masks | 0/0 |
| stack/S/native/100 | NR | NR | NR | NR | NR |
| stack/S/native/50 | NR | NR | NR | NR | NR |
| stack/S/synth/100 | NR | NR | NR | NR | NR |
| stack/S/synth/50 | NR | NR | NR | NR | NR |
| lyra/S/native/100 | NR | NR | NR | NR | NR |
| lyra/S/native/50 | NR | NR | NR | NR | NR |
| lyra/S/synth/100 | NR | NR | NR | NR | NR |
| lyra/S/synth/50 | NR | NR | NR | NR | NR |

Every fresh50% pairing leg covers all80 captured samples with internal640x496/output1280x992, uniform picture/mask/label dimensions and rejecting100%-log, wrong-width and shifted-mask controls. Each has18 measured labelled rows. Pairing PF-2 part3 is retired under AA4; no vacuous box pass is claimed. The dedicated TSR leg above supplies the non-vacuous box measurement but fails its separate setup matcher.

Historical readings remain retained: unlit failed leg0/0/56, coverage.051074219, speed1.360830758; C1 amended52/0/0, coverage.684693259, speed1.330854456, shifted4/47/0 rejected; C2 exact failed recipe plus `r.MotionBlurQuality 0` reads0/0/60, coverage.051464844, speed1.496748418. The second recipe attempt adopted historic settings; the recipe budget remains closed. The stopped093-11 native50 leg remains a reading labelled **size assertion encoded AA-off**:56/0/0, shifted6/52/0 rejected, coverage.687995799, speed1.671741570;82 half-render lines cover80 requests, with80 pictures/69 masks. These are historical readings, not new legs or recipe retries.

**Builds, implementation and diagnostics.** All eight retained9781031 normal/strict Editor/Game builds remain0 errors/0 warnings, with71 standalone header TUs per target. Identical source and binaries support retaining those builds and CPU tests:69 pure-suite steps,9 family cases,5 ownership mutants and the exposure self-test. Fresh binary/lever audits pass, including11 runtime and16 Editor helper commands. Both-encoding string scans have zero unresolved additions; ASCII/UTF-16 unknown controls reject. Comment stripping changes0/182 files; source scope and whitespace checks pass. No rebuild was necessary in093-14. [Audit](E:/IA_BuildCache/_r9314/closing-audit09314.json).

The prior design commit `ef03f36e07bb68c2e5ffd9c3f28fcb5820896836` at00:11:01 preceded fix `978103163c0a1839bd6948614818b73f07674c72` at00:14:14. That sole product fix changes AnomalyCaptureSubsystem.cpp/header,17 insertions/10 deletions: one helper resets mask, folded-event set, width, height and pixel count; allocation follows actual array length versus W*H while preserving shape-change clearing. Extracted old code fails same-size owner-reset/fold, reset completeness and actual-count allocation; new code passes all five cases, including repeated-event suppression and equal-area shape change. The retained093-12 crash remains the engine can-fail and was not rerun.

Selection/admission use the resolved owner's engine projection on GT; L3 permits the last completed camera. Labels, visibility and masks use frozen serving-view unjittered matrices/rects, normalized through the same copied rect to actual output pixels. No targeting/label FOV reconstruction remains. The design inventory is unchanged:

| Product use | Disposition |
|---|---|
| Projector/frustum/coverage/bounds/bbox and TestVisibility | Engine geometry; no scalar fallback |
| GetActiveViewInfo/FAnomalyViewInfo | Resolved engine geometry plus metadata |
| ComputeBoundsScreenSizeForActiveView | Engine projection passed to engine helper |
| Near-clip slab and confirmation rays | Engine inverse projection; SAT/trace policy retained |
| CameraClipping cache | FOV invalidates cache; engine geometry identity also participates |
| LabelWriter/capture/annotation/ControlSnapshot | FOV/aspect metadata only |
| ChangeStage FOV difference | Camera-change diagnostic |
| CaptureLetterbox | Bench aspect setter/restoration; engine computes rect |
| Scene-view extension | Frozen unjittered serving matrices |
| Written-size/output-height code | Resampling only |
| LOD/stuck-mip/missing or corrupt texture/ChangeCase/TexCorruptTree | Common engine geometry |

W1 restored `OutView.HorizontalFOVDeg` from `Camera.FOV` to `PlayerCameraManager->GetFOVAngle()` in AnomalyViewport.cpp. PF-5 measured locked/effective70 and cache90; the f9b9328 expression would return that observed90. Named-target paths, including BeginFire -> TryFireSpecific, selection, console IAI.Apply, dashboard and public single-target calls, converge on frustum admission in ApplyAnomaly before mutation. Off-screen/no-projection refusals increment the two summary-only counters and retry on existing cadence. Occlusion stays with the mask veto; frame-wide eligibility is unchanged. The readme requires Play/PIE or a game build, not Simulate.

Ownership requests freeze epoch/generation/scene/target/player/view and bind one family/view before dequeue and again on RT. Owner changes retire pending work/baselines; ambiguous, split/stereo and NotGameView cases refuse/count. There is no first-wins unknown-target route. The obsolete accepted_ambiguous field is removed. Actual summary excerpts, never label fields:

```json
{
  "capture_ownership": {
    "version": 1,
    "counter_scope": "rejections count request/family checks or resolution changes; served counts colour requests and all mask arms",
    "run_epoch": 1,
    "owner_generation": 3,
    "owner_scene_known_start": 1,
    "owner_target_known_start": 1,
    "owner_updates": 2,
    "world_type": 3,
    "net_mode": 0,
    "requested": 990,
    "sampled": 990,
    "written": 984,
    "colour_served": 988,
    "mask_served": 1478,
    "context_world_null": 0,
    "target_fallback_accepts": 0,
    "unpaired": 6,
    "dropped": 6,
    "stale_generation": 16,
    "pending_at_stop": 0,
    "rejections": {
      "owner_scene_unknown": 0,
      "owner_target_unknown": 0,
      "scene_mismatch": 3,
      "target_mismatch": 0,
      "not_game_view": 0,
      "not_game_view_show_flag": 0,
      "not_game_view_game_thread": 0,
      "not_game_view_controller_id": 0,
      "scene_capture_or_reflection": 0,
      "owner_player_unknown": 0,
      "ambiguous_owner": 0,
      "multiple_views": 0,
      "stereo_view": 0,
      "local_player_view_mismatch": 0,
      "stale_generation": 16,
      "missing_family_binding": 0,
      "owner_render_missing": 0
    }
  },
  "projection": {
    "version": 1,
    "axis_constraint": 1,
    "camera_aspect": 1.3333300352096558,
    "viewport_size": [
      1280,
      720
    ],
    "view_rect": [
      0,
      0,
      1280,
      720
    ],
    "unconstrained_view_rect": [
      0,
      0,
      1280,
      720
    ],
    "copied_view_rect": [
      0,
      0,
      1280,
      720
    ],
    "mode": "perspective",
    "source": "owner_view_unjittered",
    "served_frames": 984,
    "invalid_refused": 0
  },
  "targeted_refused_off_screen": 0,
  "targeted_refused_no_projection": 0
}
```

G484 documents that `rejections.owner_render_missing` counts a narrower check path than retirement receipts; it can be0 while dropped/unpaired and six named receipts account for camera retirement. Stale-generation rejection counters are not accepted stale frames.

**Office reader.** No Python source change here. Minimum Python3.10 is proven with3.10.11 and3.13.1; only existing NumPy/Pillow are used, without hashlib.file_digest. Formats remain `I<h>.<case> PASS v=… c=… b=… #…`, full-number FAIL plus I-failure counters, and `B<h> n=… b=… #…`. PASS requires r=w=360,e=x=m=0,paired masks and |b-1000|<=10; null masks increment m without stopping other computations. The owner counts non-game images on the20-frame numbered contact sheet and appends `g=<count>`; final PASS requires g=0. No resize during a checkpoint.

Here c=0/1/2 means X/Y/major-axis constraint; b is the worst-axis scale times1000. The actual self-test lines, identical on both Python versions, are:

```text
I1.11 PASS v=4 c=0 b=1000 #0
I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0
B1 n=96 b=999 #0
B1 n=96 b=967 #4
```

The healthy line comes from fresh9781031 engine session033532; the FAIL is the093-06 null-mask Lyra session, and the box lines use f608d56 Stack/Lyra sessions. All eight fresh reader mutants reject; originals rehash unchanged. The fresh contact sheet was inspected locally. These are home self-tests, not an office checkpoint or an owner's office g reading. Historical B19 provenance remains corrected: the093-09 healthy line came from original marker-ON and also holds on marker_r1.

**Earlier closure and date history.** Part0 remains complete at `59b2fa19aa82f6c0ebbf2db1f223ba849b796c9f`: copied PIE/staged UV events claimed StaticMeshActor_12 instead of StaticMeshActor_0, with unchanged RGB and zero mask overlap; both wrong-object checks rejected and originals rehashed unchanged. The blanket can-fail claim was corrected. G479/readme retain the masked-alpha limitation: event-state silhouettes omit pixels cut away by corrupted alpha, though edges stay0; future work is the pre/event silhouette union. Both staged proxy runs produced three events because attempt0 was refused as `texture_uniform:pending`.

The prior date recreation preserved trees, messages and author identity, with no deletion or push:

| Old hash | Correct-date replacement |
|---|---|
| `7f4329c462aa779c1703420876d9492b367aaef8` | `06740850141876fea2b57801a084fd39c85a879b` |
| `41c1a0c9d54c807b62bdf55a905783a7ea9e26d0` | `d37ab4995562ae261591c1fed8b820f670a20425` |
| `1071ca13f4f4e00d744e394203f1063fe772cab8` | `36fec5fef2ab267648544bc5a40d06998d5e76a2` |

Those dates are2026-10-15 00:40:53/53/54 IST; old objects remain preserved. The prior shutdown recovery receipt remains [crash-recovery.json](E:/IA_BuildCache/_r939/recovery09310/crash-recovery.json). No new shutdown occurred. Admission re-freezing, W2 drop accounting and PF-1 generic apply checks remain as approved; no threshold changed after a result.

**Hashes, cook and archive.** All candidate SHA-256 values below rehash identically before/after the run. No cook ran, so staged-versus-candidate equality and every staged gate remain NR. The prepared nodebuginfo/storage/control-build scripts were not executed.

| Binary | Stack SHA-256 | Lyra SHA-256 |
|---|---|---|
| Game EXE (StackOBot.exe / LyraGame.exe) | `d8d3095167a7addb6aa756674746183e78a8ceb766a4abdff4966c4312806c1b` | `dfe6f1643bfb5bf1be41ac3fce565e85d7f49960186ff9032ba11a1f914b229a` |
| UnrealEditor-AnomalyBench.dll | `e9d1754f132a1ab9bbecb6583ee87b1e7f62ffa98b0a4a62406d03455cf1daf3` | `c11d1b17b07efc7da43c7006a3b9d1bf9ea828548f315652641f8bfba60ac686` |
| UnrealEditor-AnomalyCapture.dll | `a820c0d60d8df6789729fe40228bc04cc6babc36f03d867765c251b89cf3fa49` | `53e7ec2e7655210af9fc4fa928392b93430ad627e7e73f9402c79d2155ee3871` |
| UnrealEditor-AnomalyControlServer.dll | `e6dc5176f11cbd620e0d93c53c74a23194a371a248e0a12536393f0064565aee` | `782fbdda64d7160211b0d0a6452fd7f2d615906e2012c35099ac2f0afe13de98` |
| UnrealEditor-AnomalyInjector.dll | `4cf9648912569625472d23c119be01aeead259d6c074a27a87a1f8042f0e6efc` | `1c20d2985bf521737c9984f11cf982498e3fc3dd5f9321d02fc39625588199d6` |
| UnrealEditor-AnomalyShaders.dll | `923eb3104c1f66769893e5bfb0f2498f9febce5da486028a67f2963886e5cecc` | `5ad00b823cbe7b0fd0b7920a32a57ddd0b7114212284faa057d0f5d971c71119` |
| UnrealEditor-Review0936.dll | `bd785eb52a986cf832a735692c9653240e4bc02b3e77ccd84af30a57c939b7d0` | `b5a80a89325e91574ac7737230b12e6f5fb797e572aec107085e09642578453b` |
| UnrealEditor-Review0936Runtime.dll | `ed999affe8802ee63b7e8237ab94d5f88a10526a1e7d722f17a631120bb2bb2b` | `6261de224ced26b1e894ce872caba49a09f8e9e0e1b54cc1728fd9a3cd0ee660` |
| UnrealEditor-CaptureBench.dll | `a7443041103affbcd06b724a1ee1cd66f30c636d7ba246d2b34e661b0e89fb51` | `017ca90bdffd2b79b30fa7a1e262c9f747c0070aece0608ba76177e85455edb3` |
| Shared UnrealEditor.exe | `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b` | Same |

The [UNQUALIFIED archive](D:/IA_BankOverflow/followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-OVERFLOW-UNQUALIFIED) retains candidate/fixture banks, source and closing-head snapshots, captures, scripts, controls, build/audit evidence and restoration receipts. It is stored on physical D: and linked under the archive index using the same name. Initial payload: **88,139 files, 63,237,183,076 logical bytes, 16,896,045,743 unique payload bytes**. Equal payloads share hardlinks only inside this new archive, never to source evidence. Every payload rehashes; a one-byte DLL mutant rejects with the original unchanged. Closing report/verification receipts are added and rehashed in the final manifest. The archive index is append-only; prior archives remain unchanged. [Final archive rehash](E:/IA_BuildCache/_r9314/archive-final-rehash.json).

**Harness repairs, settings and restoration.** Donor repair attempt1 succeeds; the original failure is retained. The audit wrapper initially selected J1 rather than the exact eff797c baseline frozen in093-13; one input repair restored the frozen baseline, with scanner/classifier/unknown-string controls unchanged. The stop-reading writer's Windows default decoding failed on UTF-8 source; one explicit UTF-8 read repaired this reporting script. Neither repair changed a gate or triggered an engine retry. The new TSR matcher problem was diagnosed but not repaired because it exceeds AB1 scope. Pairing, marker and packaging budgets were not reopened.

The first archive attempt checked logical D: capacity but the archive root is a junction onto E:. Only the verified archiver PID42388 was stopped before the floor; its26,576 partial files were inventoried and retained as explicitly incomplete. Routing repair attempt2 resolves the destination volume, reserves2GiB metadata space and writes the separate physical-D archive above. The original script, partial receipt and inventory remain available. This storage repair caused no engine retry or evidence change.

Process-local settings and bench levers used/restored are listed here; exact per-leg commands remain in every launch receipt:

Union of actual commands; each launch receipt preserves its grouping and output path. Historic pairing/TSR omit the AA/Lumen/GI/reflection overrides.

```text
CaptureBench.Marker 0
CaptureBench.Marker 1
CaptureBench.Marker.Top 1.0
IAI.Anomaly.StuckMipRoute auto
IAI.Bench.MaskPairingProbe 1
IAI.Bench.Review936.Observe
IAI.Bench.Review936Runtime.Ambiguous
IAI.Bench.Review936Runtime.LateStart <leg-output> png 777 360 uv_corruption StaticMeshActor_0 tile
IAI.Bench.Review936Runtime.Mode controller
IAI.Bench.Review936Runtime.Mode gameview
IAI.Bench.Review936Runtime.Mode showflag
IAI.Bench.Review936Runtime.Mode stereo
IAI.Bench.Review936Runtime.ResizeAt 100 960 720
IAI.Bench.Review936Runtime.Split
IAI.Bench.Review938.AdmissionProbe 10 StaticMeshActor_0
IAI.Bench.Review938.Projection 0 0 0 none
IAI.Bench.Review938.Projection 1123 870 0 StaticMeshActor_0
IAI.Bench.Review938.Projection 1123 870 1 StaticMeshActor_0
IAI.Bench.Review938.Projection 1280 720 -1 StaticMeshActor_0
IAI.Bench.Review938.Projection 1280 720 0 StaticMeshActor_0
IAI.Bench.Review938.Projection 1280 720 0 StaticMeshActor_0 2.39
IAI.Bench.Review938.Projection 1280 720 1 StaticMeshActor_0
IAI.Bench.Review938.Projection 942 869 -1 StaticMeshActor_0
IAI.Bench.Review938.Projection 942 869 0 StaticMeshActor_0
IAI.Bench.Review938.Projection 942 869 1 StaticMeshActor_0
IAI.Bench.Review938.Projection 942 869 1 none
IAI.Bench.Review938.Switches 60 300 1.5
IAI.Bench.Review939.LockFOV 70
IAI.Bench.SynthTickOrder 1
IAI.Bench.TexCorruptIdentity 1
IAI.Capture.Config 0 12 12 12 4
IAI.Capture.Config 0 192 48 192 4
IAI.Capture.Config 0 30 180 60 4
IAI.Capture.Config 0 30 24 60 4
IAI.Capture.Delivery 0
IAI.Capture.FocusGate 0
IAI.Capture.Fps 30
IAI.Capture.Mask 1
IAI.Capture.Pace 1
IAI.Capture.RunLog 1
IAI.Capture.SVE 1
IAI.Capture.Start <leg-output> png 777 1100 uv_corruption StaticMeshActor_0
IAI.Capture.Start <leg-output> png 777 1100 uv_corruption StaticMeshActor_0 tile
IAI.Capture.Start <leg-output> png 777 1400 stuck_low_mip StaticMeshActor_77
IAI.Capture.Start <leg-output> png 777 1400 uv_corruption StaticMeshActor_0 tile
IAI.Capture.Start <leg-output> png 777 360 uv_corruption StaticMeshActor_0 tile
IAI.Capture.Start <leg-output> png 777 80 blinking StaticMeshActor_49
IAI.Capture.TargetMask 1
r.AntiAliasingMethod 0
r.DefaultFeature.AutoExposure 0
r.DynamicGlobalIlluminationMethod 0
r.DynamicRes.OperationMode 0
r.DynamicRes.TestScreenPercentage 0
r.Editor.Viewport.OverridePIEScreenPercentage 0
r.EyeAdaptationQuality 0
r.Lumen.DiffuseIndirect.Allow 0
r.ReflectionMethod 0
r.ScreenPercentage 100
r.ScreenPercentage 50
r.ScreenPercentage.MaxResolution 0
r.ScreenPercentage.MinResolution 0
r.ScreenPercentage.Mode 0
r.SecondaryScreenPercentage.GameViewport 100
```

PIE modes: window, multi, selected, client2. ExecClient wrappers select clients0/1. Each launch uses -IAIBench, -NoLiveCoding, -unattended, -nosplash and its own plan/log paths. Fixture maps are CB_TexCorruptLevel and Proxy0912/Detail/CB_ProxyDetail. The first PF-3 setup is1280x992; camera cuts/blends and resize remain process-local.

All overrides ended with owned processes. No host config override, fixture-content change, security change or foreign-process intervention occurred. Prepared capture/evidence relocation and packaging-routing scripts were never executed; only the closing archive uses overflow storage. No cook or staging metadata repair occurred. Normal/strict build settings remain restored by their original receipts.

Restoration verified Stack head `23098fa7415f148e935acbd4a97e18da04e6c46c`,201 files/2748 content files; Lyra head `caa68c6799db023f7a24c0d6cfc56124ce27b542`,183 files/2879 content files. Only33/34 reviewed job-created files and five installed roots were removed, by receipts. All **5,764 live-stage hashes match**. No owned engine remains; no foreign process was stopped. Disk floors remained satisfied. [Restoration](E:/IA_BuildCache/_r9314/receipts/restored.json), [final verification](E:/IA_BuildCache/_r9314/final-verification09314.json).

No natural-cause code, release/delivery merge, tag, force-push or branch deletion occurred. Local head is `0e50b56df4baf516ec815f433a93084388e2cf05`; remote is unchanged at `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`; **no commits were pushed**. The remaining work is Lyra dedicated TSR, the cook, every staged gate and the final feature push, after a ruling on the dedicated TSR receipt reader.

Automatic approval review rejected removal of this job's [partial archive](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-UNQUALIFIED) as "blocked by policy." No deletion was retried. The partial attempt remains on E:; the completed archive is on physical D:.

VERDICT: NEEDS-DECISION
