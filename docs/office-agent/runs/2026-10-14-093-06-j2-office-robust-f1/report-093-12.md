All historic PIE pairings now pass, including the fresh TSR 50% proofs and shifted-mask controls.
Qualification stopped when the Stack camera reference crashed after its first cut in the product exposure-exclusion path.
J2 remains unqualified; hosts and staging are restored, and nothing was pushed.

**1. Preflight, freeze and decision.**

Work used E:/IA_BuildCache/_r933/AnomalyInjector on fix/followup-capture, initially clean at 3d09293391cf3a7ce0b0e8810804e5cafaeaf189. The compiled candidate remains eff797cf8dae0c886556e742cae4fe98f59458e1. Both game EXEs, ten product DLLs, four Review helper DLLs and both CaptureBench fixture DLLs rehashed identically to the frozen banks and 093-11 archive. Stack/Lyra hosts matched 201/183 original files and 2748/2879 content files at 23098fa/caa68c6. All 5764 live-staging hashes matched. No prior receipt-owned engine process survived. Both Git date overrides were absent. The 36 completed J2 cases, PF-1 through PF-5, measured 0.813 replay and office-reader tests stand under Z3/X2. [Preflight receipt](E:/IA_BuildCache/_r9312/preflight.json).

The earlier shutdown recovery remains at [crash-recovery.json](E:/IA_BuildCache/_r939/recovery09310/crash-recovery.json): 23 dead-run identities absent, four saved-editor config files restored, unchanged candidate binaries and original content/staging restored. This continuation required no product rebuild.

The Z2 freeze is **cdef048e35cb25bc5c4d16d34ea278be477b720c**, `docs(capture): freeze TSR half-resolution pairing proof`, committed **2026-10-15 20:47:54 +05:30**. The first fresh leg launched **20:57:18.620 IST**, after the freeze and a passing idle gate. [Predictions](E:/IA_BuildCache/_r933/AnomalyInjector/docs/predictions/093-12-tsr-half-resolution.md) contain Z3's stop rules verbatim. No product, fixture, permanent reader or tolerance changed.

The historic recipe applies to every pairing case: no arbiter overrides for AA, Lumen diffuse indirect, dynamic GI or reflections. Host values remain in effect; the frozen exposure, screen-percentage, marker and other commands are unchanged. The pairing reader SHA-256 is **54928b2ecce48d97a1af4bea16b00184ca091e8b22670fe68e8e3165671f3525**, tolerance **40 px**, PREVIOUS=NEITHER=0, decidable floor **1**. Recipe attempts are closed at 2/2; packaging metadata and marker budgets are spent.

Z2 measures half-size internal rendering on every captured mask-pass record, consistent picture/mask/label dimensions, the unchanged PF-2 estimator per axis, and unchanged pairing plus its shifted-mask negative. An axis gates only at n>=20; smaller samples are readings. The 100% log is the reduced-render negative, and a 0.96 mutant must reject on any gating axis. The previous stopped Stack native50 remains **“size assertion encoded AA-off”**, assessed only as a reading. It is not substituted for the fresh gate. No AA-off 50% leg is planned; its original size predicate would still apply.

The preceding one-factor controls remain relevant. C1 restored lighting and read 52/0/0, shifted 4/47/0 rejected, median probe coverage 0.6846932587496334 and speed_ratio 1.3308544555985475. C2 retained arbiter lighting and added exactly `r.MotionBlurQuality 0`: 0/0/60, coverage 0.05146484375, speed_ratio 1.4967484182760686. The failed unlit leg was 0/0/56, coverage 0.05107421875, speed_ratio 1.3608307584742956. Thus restoring lighting repaired that instrument precondition; motion-blur removal did not. C2 is a reading, not an attempt. Later amended-recipe Lyra native100 read 49/0/9, using the second recipe attempt; the historic recipe then passed Lyra 50/0/0 and Stack 55/0/0, with shifted copies rejected. The amended Lyra cause is not established by C1. No positive engine pairing has read PREVIOUS.

Coverage is the unchanged reader's bright-magenta pixels inside the current tag-255 mask divided by its area, median over decidable frames on the reader's sampling grid. speed_ratio is m51's wall-span/game-span reading. Both are readings, not gates. No separate historic passing bank was found during the prior documented search; the two retained historic 100% cases now provide coverage readings. Original source images, masks and metadata remain unchanged.


The new gate stop is the Stack camera identity reference, launched 22:41:59.337 IST after a passing idle gate. The cut occurs at sample 60/game frame 226; the editor exits 3 with an array assertion at 22:42:36.447 IST. The fault is in the product AnomalyCapture module, not a packaging or barcode failure. No camera retry, product repair, cook or staged gate follows. [Stop receipt](E:/IA_BuildCache/_r9312/gate-stop.json).


**2. Inherited J1 closure.** Part 0 was completed before J2 in 59b2fa19aa82f6c0ebbf2db1f223ba849b796c9f. Copies of PIE and staged J1 UV captures relabelled the first 48-frame event from StaticMeshActor_0 to StaticMeshActor_12 with that object's measured mask. RGB was unchanged and mask overlap was zero; both visibility checks rejected the wrong object. The original captures rehashed unchanged. The blanket claim that every J1 can-fail ran was corrected. G479 and the readme record that event-state silhouette masks omit pixels cut away by corrupted alpha, while edges remain zero frames off; the future item is the union of pre-event and event silhouettes. Both staged proxy runs had three events because attempt 0 was refused as texture_uniform:pending. [Part 0 evidence](E:/IA_BuildCache/_r936/part0-r1/result.json).

**3. Projection, admission and metadata as built.** Selection and admission use ULocalPlayer::GetProjectionData for the resolved capture owner's viewport on the game thread. Capture sampling updates after the camera/world tick; L3 allows selection/admission to use the last completed camera. Labels, visibility and masks use the frozen serving record, including the owner view's unjittered matrices and rects. Projection coordinates are normalized through the same copied rect and mapped to actual picture dimensions. Screen percentage, letterboxing and resampling do not introduce FOV/aspect reconstruction.

Ownership activation decides participation only. GT resolves a unique local player/display before requests; there is no unknown-target fallback. Each colour, mask, census and probe request freezes the run epoch, owner generation, scene, real target and local-player/view identity. A unique family/view binding is required before dequeue and again on RT. Viewport changes invalidate pending work and baselines; single-view scope, same-sample expiry and dropped-result accounting remain enforced.

The [design note](E:/IA_BuildCache/_r933/AnomalyInjector/docs/sessions/2026-10-14-093-07-engine-projection.md) and predictions preceded product changes. Its complete FOV/aspect inventory is:

| Use | Disposition |
|---|---|
| Viewport projection builders, frustum, coverage, bounds and bbox | Saved engine matrices |
| GetActiveViewInfo | Resolved local-player engine projection; FOV and viewport aspect remain metadata |
| ComputeBoundsScreenSizeForActiveView | Engine projection passed to the engine helper |
| Near-clip slab and confirmation rays | Engine inverse projection, including off-centre geometry |
| AnomalyNearClipSlab scalar reconstruction | Removed; SAT/trace policies retained |
| CameraClipping cache key | FOV is only an invalidator; engine geometry identity also participates |
| Injector TestVisibility | Resolved engine geometry; no scalar-camera fallback |
| FAnomalyViewInfo | Metadata plus copied engine geometry values |
| LabelWriter fovDeg/aspect | Existing metadata meanings retained |
| Capture/annotation camera fields | Metadata only |
| ChangeStage FOV difference | Diagnostic camera-change reading |
| ControlSnapshot camera fields | Metadata only |
| CaptureLetterbox | Bench aspect setter/restoration; engine computes rect |
| Scene-view extension projection | Unjittered serving-view matrices frozen with the sample |
| Output-height and written-size code | Image resampling only |
| LOD, stuck-mip, missing/corrupt texture, ChangeCase and TexCorruptTree callers | Common engine record; no anomaly-specific camera reconstruction |

W1 restored the metadata accessor in [AnomalyViewport.cpp](E:/IA_BuildCache/_r933/AnomalyInjector/Source/AnomalyInjector/Private/AnomalyViewport.cpp):

~~~cpp
// Previous metadata expression:
OutView.HorizontalFOVDeg = Camera.FOV;
// Restored expression:
OutView.HorizontalFOVDeg = Player->PlayerController->PlayerCameraManager->GetFOVAngle();
~~~

These explanatory comments are report text, not source changes. Boxes and visibility remain on engine projection. PF-5 observed requested=70, GetFOVAngle=70, cache=90; every fovDeg label was 70. The exact f9b9328 expression would report the observed 90 and fails the same locked-70 predicate.

Named-target admission converges at ApplyAnomaly before mutation. The inventory covers BeginFire → TryFireSpecific, selector fires, console IAI.Apply, dashboard and public single-target calls. It tests renderable bounds against the owner's engine frustum; occlusion remains with the mask veto. Off-screen and unavailable-projection attempts are refused and counted in run_summary.json only, with existing retry cadence. Frame-wide eligibility remains separate. No natural-cause code, engine edit, new private header or module dependency was introduced.

Cumulative product and reader files changed since pushed J1:

~~~text
Source/AnomalyCapture/Private/AnomalyCaptureFamily.h
Source/AnomalyCapture/Private/AnomalyCaptureOwnerState.h
Source/AnomalyCapture/Private/AnomalyCaptureOwnership.h
Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp
Source/AnomalyCapture/Private/AnomalyCensus.cpp
Source/AnomalyCapture/Private/AnomalyCensus.h
Source/AnomalyCapture/Private/AnomalyChangeFamilyData.cpp
Source/AnomalyCapture/Private/AnomalyChangeFamilyData.h
Source/AnomalyCapture/Private/AnomalyChangeStage.cpp
Source/AnomalyCapture/Private/AnomalyChangeStage.h
Source/AnomalyCapture/Private/AnomalyFrameCapturer.h
Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp
Source/AnomalyCapture/Private/AnomalyLabelWriter.h
Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp
Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.h
Source/AnomalyCapture/Private/AnomalyMaskTypes.h
Source/AnomalyCapture/Private/AnomalySceneViewExtension.cpp
Source/AnomalyCapture/Private/AnomalySveCapturer.cpp
Source/AnomalyCapture/Private/AnomalySveCapturer.h
Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h
Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.cpp
Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.h
Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp
Source/AnomalyInjector/Private/AnomalyViewport.cpp
Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h
Source/AnomalyInjector/Public/AnomalyNearClipSlab.h
Source/AnomalyInjector/Public/AnomalyViewport.h
tools/camera_clipping_slab_selftest.cpp
tools/capture_ownership_selftest.cpp
tools/office_checkpoint_i.md
tools/office_checkpoint_i.py
tools/test_office_checkpoint_i.py
~~~

**4. Predictions retained or re-frozen before legs.**

| Amendment | Prediction and reason |
|---|---|
| L2 PF-1 | Zero visible rows and applied fires, positive off-screen refusal, no no-projection refusal: admission must reject before mutation |
| L2 missing/ambiguous owner | No application without projection; no-projection refusals are retryable |
| Ambiguous/split negatives | 360/0 with ownership rejections, zero applications and positive no-projection counters |
| Render-only stereo/NotGameView negatives | Retain 360/0 and specific rejection counters; GT admission need not refuse a render-only negative |
| Delayed/other healthy cases | At least three visible events, zero edges, measured masks and completeness; an early invalid attempt may be refused |
| PF-3 | Same events, targets, edges and visible_positive; bbox difference at most 0.5 px. Stable framing resolves before firing |
| W1 PF-5 | Lock FOV to 70 before framing; metadata must read 70, with ordinary pixel/mask/PF-2 floors. Use the observed cache value for the old-route negative |
| W2 resize/camera | r=w+named drops, exactly one retirement per missing sample, no overlap/unexplained drop/pending sample, zero stale or mismatched acceptance. Other healthy cases retain w=r |
| W5 PF-1 checker | Count generic IAI.Apply applied receipts as well as texture-specific fires, so non-texture applications cannot escape the check |
| X1 PF-3 setup | Restore retained 1280x992 viewport; unchanged 0.5 px comparator. This is a harness repair, not a relaxed prediction |

Y2 exhausted both pairing recipe attempts in 093-11. Z2 re-states only the TSR half-resolution proof before any 093-12 leg: measure the internal rect, consistent output sizes, per-axis PF-2 with at least 20 qualifying rows to gate, and unchanged pairing with its shifted-mask control. A non-gating axis is reported as a reading. Z3 keeps historic settings and closes the recipe, metadata and marker budgets. No product, fixture content or permanent reader changed in 093-12.

**5. Gates and can-fails.** r/w is requested/written; v is independently visible events; e is maximum measured edge error; O/N is targeted_refused_off_screen/targeted_refused_no_projection. NR means not run. Negative-control PASS means the deliberately faulty input/build was rejected. Synthetic alignment certifies identity/membership; native companions certify RGB visibility and edges.

| Gate | PIE result | Staged result | Can-fail |
|---|---|---|---|
| PF-1, exact Lyra 942x869 Y off-left arrangement | PASS: 360/360, zero visible rows, zero texture/generic applications, O/N=4/0 | Not specified separately | Retained 093-07 four-fire session and 093-06 96-false-row session reject; non-texture apply mutant rejects |
| PF-2 | PASS 12/12; each 360/360, v4, e0, 96 measured paired rows, O/N=0/0 | NR: four required cases | Per-axis 0.96 mutants reject; old Stack box reading passes, old Lyra 0.967 and measured 0.813 reject |
| PF-3 UV and proxy | PASS: each 1152/1152, v4, e0, 192 measured rows, O/N=0/0; events/targets/edges/visible_positive identical; bbox maximum difference 0.0 px | NR | One-pixel box and visible-onset mutants reject |
| PF-4 letterbox | PASS: 360/360, v4, e0, n96, b1000, O/N=0/0; viewport 1280x720, copied rect [0,92,1280,536], aspect 2.39 | Not specified | X/Y scale and full-window mapping mutants reject |
| PF-5 locked FOV | PASS: 360/360, v4, e0, n96, b1000, O/N=0/0; all labels 70 | Not specified | Observed old-route 90 rejects locked 70; per-axis box mutants reject |
| Camera cut/blend, both fixtures | FAIL: Stack identity reference asserts after cut; active Stack and both Lyra legs NR | NR | Identity-self and stale-projection controls remain unrun |
| Supplemental admission probes, both fixtures | PASS: each 360/360, v4, e0, wrong-view 0, O/N=0/1 | NR | Zero-counter and applied-after-refusal copies reject on both fixtures |
| Office reader | Required four self-tests PASS as tests; new healthy session and eight negative controls verified | No separate staged reader claim | Details below |

PF-2 per-case scale readings:

| Fixture | Axis | Viewport | n | b | Raw X / Y | O/N |
|---|---|---|---:|---:|---|---|
| Stack | X | 942x869 | 96 | 1000 | 1.000000052 / 0.999999979 | 0/0 |
| Stack | X | 1123x870 | 96 | 999 | 1.000356226 / 0.998769611 | 0/0 |
| Stack | X | 1280x720 | 96 | 1000 | 0.999999940 / 1.000000000 | 0/0 |
| Stack | Y | 942x869 | 96 | 1000 | 1.000000052 / 0.999999979 | 0/0 |
| Stack | Y | 1123x870 | 96 | 999 | 1.000356226 / 0.998769611 | 0/0 |
| Stack | Y | 1280x720 | 96 | 1000 | 0.999999940 / 1.000000000 | 0/0 |
| Lyra | X | 942x869 | 96 | 1000 | 1.000000052 / 0.999999979 | 0/0 |
| Lyra | X | 1123x870 | 96 | 999 | 1.000356226 / 0.998769611 | 0/0 |
| Lyra | X | 1280x720 | 96 | 1000 | 0.999999940 / 1.000000000 | 0/0 |
| Lyra | Y | 942x869 | 96 | 1000 | 1.000000052 / 0.999999979 | 0/0 |
| Lyra | Y | 1123x870 | 96 | 999 | 1.000356226 / 0.998769611 | 0/0 |
| Lyra | Y | 1280x720 | 96 | 1000 | 0.999999940 / 1.000000000 | 0/0 |

PF-2 estimator is unchanged: use labelled, non-transition rows with finite boxes and paired measured masks matching dimensions/count/bounds; boxes must be at least one pixel inside the image. For each axis, require mask-centre displacement at least 5% of picture dimension, take the median projected/mask centre-displacement ratio about picture centre, with no trimming or refitting, and require at least 20 distinct rows and raw scale in [0.990,1.010]. b is the worse axis ×1000, rounded half up, with X breaking ties.

The archived f608d56 Lyra replay at 942x869 Y produced n=96, scales 0.8130053613451103 / 0.8130053403348766, b=813, and was rejected. It used the exact five archived product DLLs with current job helpers; all five candidate DLLs were restored and rehashed. [Replay receipt](E:/IA_BuildCache/_r939/controls/lyra_P_f608_pf2_y_942x869/swap-receipt.json).

| Case | Stack PIE | Lyra PIE | Stack staged | Lyra staged |
|---|---|---|---|---|
| Delayed viewport | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | NR | NR |
| Healthy view | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | NR | NR |
| Second realtime editor view | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A | N/A |
| Selected viewport | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A | N/A |
| Two-client client0 | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A | N/A |
| Two-client client1 | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A | N/A |
| Resize | PASS 360/358, v4, e0, O/N=0/0 | PASS 360/358, v4, e0, O/N=0/0 | NR | NR |
| Same-world ambiguity | PASS 360/0, O/N=0/4 | PASS 360/0, O/N=0/4 | NR | NR |
| Split-screen | PASS 360/0, O/N=0/4 | PASS 360/0, O/N=0/4 | NR | NR |
| Stereo | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR |
| Negative controller ID | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR |
| Non-game-thread view | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR |
| Game show flag off | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR |
| Native UV regression | PASS 1152/1152, v4, e0, O/N=0/0 | PASS 1152/1152, v4, e0, O/N=0/0 | NR | NR |
| Native proxy regression | PASS 1152/1152, v4, e0, O/N=0/0 | PASS 1152/1152, v4, e0, O/N=0/0 | NR | NR |
| Synthetic UV alignment | PASS 1152/1152, events=4, O/N=0/0 | PASS 1152/1152, events=4, O/N=0/0 | NR | NR |
| Synthetic proxy alignment | PASS 1152/1152, events=4, O/N=0/0 | PASS 1152/1152, events=4, O/N=0/0 | NR | NR |
| Native pairing 100% | PASS 80/80, O/N=0/0 | PASS 80/80, O/N=0/0 | NR | NR |
| Native pairing 50% | PASS 80/80, O/N=0/0 | PASS 80/80, O/N=0/0 | NR | NR |
| Synthetic pairing 100% | PASS 80/80, O/N=0/0 | PASS 80/80, O/N=0/0 | NR | NR |
| Synthetic pairing 50% | PASS 80/80, O/N=0/0 | PASS 80/80, O/N=0/0 | NR | NR |
| J1 delayed negative | PASS 360/0, O/N=unavailable/unavailable | PASS 360/0, O/N=unavailable/unavailable | NR | NR |
| Pre-F1 wrong-view negative | PASS 360/360, v0, e-1, O/N=unavailable/unavailable | PASS 360/360, v0, e-1, O/N=unavailable/unavailable | NR | NR |

The [84-case J2 ledger](E:/IA_BuildCache/_r9312/qualification09312.json) has **46 PASS, 0 FAIL, 38 NR**: all PIE matrix cases pass; all staged cases remain unrun. The separate mandatory camera requirement fails. This ledger count is not an overall GREEN verdict. The 36 retained cases stand after identical rehash; six new historic pairings and four exact old-build negatives add ten passes. Both supplemental admission probes also pass outside that 84-case count.

- Exact J1 DLL controls on both fixtures request 360 frames and write 0, proving the delayed-owner negative. Exact pre-F1 DLL controls write 360/360 with **360 wrong-view frames** on each fixture. All four five-DLL swaps restore and rehash the candidate. Their unavailable admission fields are not treated as zero. Staged controls remain NR.
- Candidate multi-view PIE has wrong-view count 0 and non-owner rejections Stack=2476, Lyra=2502. Selected/two-client frames pass world/frame identity; cross-world binding mutants reject.
- Ambiguity and split-screen refuse all 360 requests, with 730/366 counted checks on each fixture and O/N=0/4. Stereo counts 728 checks. Controller sub-rejections are Stack=1067, Lyra=1068; game-thread view and Game-show-flag sub-rejections are 1068 each. Those render-only negatives have O/N=0/0. These are intentional refusal cases.
- Every completed native UV/proxy regression rejects ten copied defects: onset/recovery shifts, missing/blank masks, wrong mask counts, erased markers, wrong drop counts and a physical pixel deletion. Part 0 independently rejects the wrong object. Synthetic alignment on both fixtures rejects mask identity, event identity and donor-frame barcode mutants; native companions establish RGB edges.
- All eight historic PIE pairings have PREVIOUS=NEITHER=0 and reject shifted masks. Z2's 100% log and wrong-width negative reject on all fresh 50% cases. Their PF-2 axes have n=0, so no gating-axis 0.96 mutant is applicable; this is an explicit reading limit, not measured scale agreement. The normal PF-2 matrix's per-axis 0.96 mutants reject.
- Resize accounting negatives reject missing reasons/receipts and written/dropped overlap. The camera identity-self and stale-projection negatives were not reached because the reference crashed. The 57 partial pre-cut frame/world markers decode correctly; no marker repair was made.

The pairing table retains the earlier failed unlit leg, C2 reading and amended-recipe history. These are not substituted for current historic gates. The old historic Stack native50 is a Z2 reading only; its separate fresh leg is the gate. Unrun staged rows carry no results.

**6. Pairing, drop accounting and camera readings.**

| Evidence / leg | C/P/N | Decidable / floor | Same / no data | Coverage | speed_ratio | O/N | Shifted-mask C/P/N; rejected |
|---|---|---|---|---|---|---|---|
| _r9311/failed09310 | 0/0/56 | 56/1 | 7/17 | 0.051074219 | 1.3608307584742956 | 0/0 | NR |
| _r9311/lyra_P_pair_historic_native_100 | 50/0/0 | 50/1 | 13/17 | 0.687368611 | 1.7648565948386736 | 0/0 | 6/43/0; yes |
| _r9311/lyra_P_pair_native_100 | 49/0/9 | 58/1 | 3/19 | 0.686985926 | 1.4332142274213744 | 0/0 | NR |
| _r9311/stack_P_pair_c2_100 | 0/0/60 | 60/1 | 4/16 | 0.051464844 | 1.4967484182760686 | 0/0 | NR |
| _r9311/stack_P_pair_historic_native_100 | 55/0/0 | 55/1 | 8/17 | 0.681289692 | 1.7526797464565387 | 0/0 | 5/50/0; yes |
| _r9311/stack_P_pair_historic_native_50 | 56/0/0 | 56/1 | 8/16 | 0.687995799 | 1.6717415699098683 | 0/0 | 6/52/0; yes |
| _r9311/stack_P_pair_native_100 | 52/0/0 | 52/1 | 13/15 | 0.684693259 | 1.3308544555985475 | 0/0 | 4/47/0; yes |
| _r9311/stack_P_pair_native_50 | 64/0/0 | 64/1 | 0/16 | 0.553435147 | 1.4826194805246364 | 0/0 | 6/56/0; yes |
| _r9311/stack_P_pair_synth_100 | 55/0/0 | 55/1 | 8/17 | 0.685937500 | 1.6826494561531875 | 0/0 | 5/50/0; yes |
| _r9311/stack_P_pair_synth_50 | 64/0/0 | 64/1 | 0/16 | 0.553822466 | 1.631710101154786 | 0/0 | 6/56/0; yes |
| _r9312/lyra_P_pair_historic_native_50 | 57/0/0 | 57/1 | 4/19 | 0.692690879 | 1.90546165821673 | 0/0 | 8/49/0; yes |
| _r9312/lyra_P_pair_historic_synth_100 | 52/0/0 | 52/1 | 10/18 | 0.686543290 | 1.8354248731762548 | 0/0 | 8/44/0; yes |
| _r9312/lyra_P_pair_historic_synth_50 | 56/0/0 | 56/1 | 6/18 | 0.692943975 | 1.8737517344423487 | 0/0 | 8/47/0; yes |
| _r9312/stack_P_pair_historic_native_50 | 56/0/0 | 56/1 | 8/16 | 0.688305011 | 1.7944412654902362 | 0/0 | 7/49/0; yes |
| _r9312/stack_P_pair_historic_synth_100 | 60/0/0 | 60/1 | 3/17 | 0.682945833 | 1.826303469700904 | 0/0 | 6/55/0; yes |
| _r9312/stack_P_pair_historic_synth_50 | 61/0/0 | 61/1 | 3/16 | 0.689513767 | 1.8847957215731657 | 0/0 | 7/55/0; yes |
| stack_S_pair_historic_native_100 | NR | NR / 1 | NR | NR | NR | NR | NR |
| stack_S_pair_historic_native_50 | NR | NR / 1 | NR | NR | NR | NR | NR |
| stack_S_pair_historic_synth_100 | NR | NR / 1 | NR | NR | NR | NR | NR |
| stack_S_pair_historic_synth_50 | NR | NR / 1 | NR | NR | NR | NR | NR |
| lyra_S_pair_historic_native_100 | NR | NR / 1 | NR | NR | NR | NR | NR |
| lyra_S_pair_historic_native_50 | NR | NR / 1 | NR | NR | NR | NR | NR |
| lyra_S_pair_historic_synth_100 | NR | NR / 1 | NR | NR | NR | NR | NR |
| lyra_S_pair_historic_synth_50 | NR | NR / 1 | NR | NR | NR | NR | NR |

| 50% leg | Role | Internal/output; lines; covered requests | Pictures/masks; size | Measured labelled rows | X n/scale | Y n/scale | Controls | Result |
|---|---|---|---|---|---|---|---|---|
| stack_P_pair_historic_native_50 | retained reading | [[640, 496, 1280, 992]]; 82; 80 | 80/69; PASS | 18/18 | 0/unavailable (reading) | 0/unavailable (reading) | 100% log reject; width reject; 0.96 N/A: no gating axis | PASS |
| lyra_P_pair_historic_native_50 | fresh gate | [[640, 496, 1280, 992]]; 82; 80 | 80/63; PASS | 18/18 | 0/unavailable (reading) | 0/unavailable (reading) | 100% log reject; width reject; 0.96 N/A: no gating axis | PASS |
| lyra_P_pair_historic_synth_50 | fresh gate | [[640, 496, 1280, 992]]; 80; 80 | 80/65; PASS | 18/18 | 0/unavailable (reading) | 0/unavailable (reading) | 100% log reject; width reject; 0.96 N/A: no gating axis | PASS |
| stack_P_pair_historic_native_50 | fresh gate | [[640, 496, 1280, 992]]; 80; 80 | 80/68; PASS | 18/18 | 0/unavailable (reading) | 0/unavailable (reading) | 100% log reject; width reject; 0.96 N/A: no gating axis | PASS |
| stack_P_pair_historic_synth_50 | fresh gate | [[640, 496, 1280, 992]]; 81; 80 | 80/69; PASS | 18/18 | 0/unavailable (reading) | 0/unavailable (reading) | 100% log reject; width reject; 0.96 N/A: no gating axis | PASS |
| stack_S_pair_historic_native_50 | gate | NR | NR | NR | NR | NR | NR | NR |
| stack_S_pair_historic_synth_50 | gate | NR | NR | NR | NR | NR | NR | NR |
| lyra_S_pair_historic_native_50 | gate | NR | NR | NR | NR | NR | NR | NR |
| lyra_S_pair_historic_synth_50 | gate | NR | NR | NR | NR | NR | NR | NR |

| Leg | r | w | Counted drops by reason | Missing indices | Stale/mismatched accepted | O/N |
|---|---:|---:|---|---|---:|---|
| stack_P_resize | 360 | 358 | {"owner_render_missing": 2} | 98,99 | 0 | 0/0 |
| stack_S_resize | NR | NR | NR | NR | NR | NR |
| lyra_P_resize | 360 | 358 | {"owner_render_missing": 2} | 98,99 | 0 | 0/0 |
| lyra_S_resize | NR | NR | NR | NR | NR | NR |

The retained PIE resize legs each satisfy **360 = 358 + 2 owner_render_missing drops**, at indices 98 and 99. Each has generation 2, one owner update, six stale-generation rejection checks, zero stale/mismatched accepted frames, v4/e0, 96 measured masks and b1000. No missing receipt, written/dropped overlap or pending sample remains. Staged resize is NR.

Each of the six new positive pairing sessions writes 80/80 with zero dropped/unpaired/pending and O/N=0/0. Both admission probes write 360/360 with zero drops, O/N=0/1. Both release controls write 360/360; the old J1 controls deliberately write 0/360. Old builds do not supply current admission diagnostics.

**Camera reference crash readings:** planned r=990, but only **62 arm requests** were issued. Exactly **57 rows/pictures** at indices 0..56 were written, **3 named owner_render_missing retirements** account for 57/58/59, and **2 requests at 60/61 remain unfinalized**. Thus 62=57+3+2 describes the crash state; it does not satisfy completed W2 accounting. All three named retirements are pending pre-switch samples and count toward the cut reading. A complete per-switch drop total is unavailable. No final run_summary.json or annotation.json exists, so final admission totals are unavailable, not zero.

The partial frames are all pre-cut: 28/28 labelled rows have measured masks, X/Y each n=28 with scales 0.9999999403953554/1.0, and the frozen frame/world decoder finds zero mismatches in 57 written frames. No post-cut acceptance claim is possible. One identity-mode event applied at game frame 195 and remained tagged at samples 60/61. Its change-evidence phase records finalized_by=epoch_reset and reason=extent_mismatch; there is no normal event-end/revert receipt before the crash. The blend at 300 was never reached. Candidate camera edges, active-event continuation through the full cut/blend and moving-frame PF-2 remain unqualified. [Partial readings](E:/IA_BuildCache/_r9312/stop-readings09312.json).

The portable stack's first RVA, 0x579b3, is inside PE function interval 0x56fe0..0x57f42, whose entry is exported as FoldExposureExclusion. Callers map to ServiceTargetMask, ProcessCompletedFrames and Tick. Source inspection at AnomalyCaptureSubsystem.cpp:4732 clears the exclusion mask/folded set but retains width/height/count. The fold at :1486 allocates only on dimension change and indexes the array at :1524. This supports the stale-dimensions diagnosis for the index-206410/size-0 assertion. No repaired-run claim is made; Source and Shaders remain unchanged.

stack_P_camera: NR.

stack_S_camera: NR.

lyra_P_camera: NR.

lyra_S_camera: NR.

Admission values are recorded per leg in the PF, matrix and pairing tables. Both new supplemental probes have O/N=0/1, before=0/after=0, reason=targeted_refused_no_projection and restored=1 at sample 10; both copied counter/application controls reject. All six fresh pairings have 0/0. Retained PF-1 has 4/0 and zero generic/texture applications; ambiguity/split have 0/4. Other completed healthy candidate cases have 0/0. The archived f608/J1/release sessions and crashed reference lack the summary fields: unavailable. Camera active legs and every staged leg are NR.

**7. Office reader.** [office_checkpoint_i.py](E:/IA_BuildCache/_r933/AnomalyInjector/tools/office_checkpoint_i.py) and [its documentation](E:/IA_BuildCache/_r933/AnomalyInjector/tools/office_checkpoint_i.md) remove accepted_ambiguous/a=, count null measured masks as m, write a 20-frame numbered contact sheet beside the session, and measure edges. No handwritten inspection input is required. The owner appends g=0 after inspecting the local contact sheet. Host 1 is the first office host; host 2 is the second office host. These self-tests use home sessions, not office data.

Formats:

~~~text
I<h>.<case> PASS v=<v> c=<0 X|1 Y|2 major> b=<scale x1000> #<check>
I<h>.<case> FAIL r=<r> w=<w> v=<v> e=<e> x=<x> m=<m> c=<c> b=<b> #<check> I-fail=<counters>
B<h> n=<rows> b=<scale x1000> #<check>
~~~

Machine PASS requires r=w=360, at least three visible events, e=x=m=0, paired measured masks and both raw scale axes within 1%; the final owner reading also requires g=0. Box mode uses the same PF-2 estimator. Minimum Python is 3.10; actual Python 3.10.11 and 3.13.1 produced identical outputs using only the existing NumPy/Pillow kit and repository reader code, with no hashlib.file_digest.

Required self-test lines, quoted verbatim:

~~~text
I1.11 PASS v=4 c=0 b=1000 #0
I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0
B1 n=96 b=999 #0
B1 n=96 b=967 #4
~~~

Expected exits are 0,1,0,1. B19 correction: the healthy line came from the original marker-ON Stack PF-2 session, session_20261015-020500. It also holds on marker_r1, session_20261015-033612; 093-11 reran both on Python 3.10 with unchanged input hashes. The null-mask failure is the retained 093-06 Lyra session; box PASS/FAIL use retained f608d56 Stack/Lyra sessions. Eight copied-input controls reject: null mask, wrong mask count, missing ownership, missing projection, invalid view, X-only/Y-only 4% scaling and shifted onset. Originals rehash unchanged. The home contact sheet was inspected; all 20 frames show the fixture. This is not an office owner's g reading. The required new-engine-session PASS is already established; no redundant J2-session repeat is claimed. Y4 adds the instruction: do not resize the window during a checkpoint session. This continuation changes no office Python code.

**8. Diagnostics.** Example actual projection block from the passing letterbox session; this is in run_summary.json only, never labels:

~~~json
{
  "projection": {
    "version": 1,
    "axis_constraint": 0,
    "camera_aspect": 2.390000104904175,
    "viewport_size": [
      1280,
      720
    ],
    "view_rect": [
      0,
      92,
      1280,
      536
    ],
    "unconstrained_view_rect": [
      0,
      0,
      1280,
      720
    ],
    "copied_view_rect": [
      0,
      92,
      1280,
      536
    ],
    "mode": "perspective",
    "source": "owner_view_unjittered",
    "served_frames": 360,
    "invalid_refused": 0
  }
}
~~~

capture_ownership retains owner-known-at-start, owner updates, reason/sub-reason rejection counts, unpaired/dropped and stale-generation telemetry. accepted_ambiguous was removed as K5 required. The two targeted-refusal counters are also summary-only. Rejections count request/family checks, not unique pictures.

Actual ownership excerpt from Stack PIE resize; stale_generation counts rejected checks, while the accepted stale-frame count is zero:

~~~json
{
  "capture_ownership": {
    "owner_generation": 2,
    "owner_scene_known_start": 1,
    "owner_target_known_start": 1,
    "owner_updates": 1,
    "requested": 360,
    "written": 358,
    "unpaired": 2,
    "dropped": 2,
    "stale_generation": 6,
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
      "stale_generation": 6,
      "missing_family_binding": 0,
      "owner_render_missing": 0
    }
  }
}
~~~

Y4 scope: owner_render_missing in the rejections map counts family checks, not every retired sample. The two resize retirements are named in the log and counted in dropped/unpaired/capture_unpaired_dropped even though this map entry is zero. G484 and the run-summary documentation record the scope; the next product change must count the drop reason there or remove the key. No product change was made for this diagnostic.

**Cook and staged binary condition.** No cook ran: K7 stops at the camera product assertion. The prepared -nodebuginfo route therefore has no staged EXE/DLL rehash result, and no staged hash-equivalence or qualification is claimed. Both game EXEs, ten product DLLs, four helpers and both fixture DLLs remain identical to the eff797c manifests. Exact metadata/support restoration is disclosed below; it did not compile or relink code. Live staging was never replaced.

**9. Builds, audits, heads and archive.** The unchanged candidate retains all eight normal/strict Editor/Game builds on both fixtures, each with zero errors/warnings. Strict builds compiled 71 standalone header TUs per target and restored build settings. All 69 CPU steps, nine family-test steps and five ownership mutants have expected outcomes. Source lever audit covers 53 registrations and 12 self-tests; binary audits verify both configurations and the rebuilt helpers, including 11 runtime and 16 Editor commands with Lyra Switches present. String scans have zero unresolved additions, with unknown ASCII/UTF-16 sentinels rejected. Comment stripping changed 0 of 182 files; diff checks passed.

W4 date recreation preserved trees, messages and author identity, with both date environment variables absent. New author/committer timestamps are 2026-10-15 00:40:53 +05:30 for the first two and 00:40:54 for the third. The old chain is retained in a bundle; nothing was deleted or pushed.

| Old commit | Recreated commit |
|---|---|
| 7f4329c462aa779c1703420876d9492b367aaef8 | 06740850141876fea2b57801a084fd39c85a879b |
| 41c1a0c9d54c807b62bdf55a905783a7ea9e26d0 | d37ab4995562ae261591c1fed8b820f670a20425 |
| 1071ca13f4f4e00d744e394203f1063fe772cab8 | 36fec5fef2ab267648544bc5a40d06998d5e76a2 |

Metadata fix: 3dc0c98d1ee932bac1672d5fd832495dda77d7a3. Compiled snapshot: eff797cf8dae0c886556e742cae4fe98f59458e1. Prior closing head: 3d09293391cf3a7ce0b0e8810804e5cafaeaf189. Z2 freeze: cdef048e35cb25bc5c4d16d34ea278be477b720c. Closing documentation commit: **7049614542edb868458b1223f6397d74a3024b59**.

**Commits pushed: none.** Remote fix/followup-capture remains 1bf7054f5b8664f057282ed863a62ab5dbbd60e3.

No merge, tag, force-push or natural-cause code. Source and Shaders equal the compiled candidate; tools/*.py is unchanged. Source tree 2f639a77861feb8dce828ff15a9d768733c94cb6; shader tree 64e18bf3d238c0c10817c34ed03fcb24f5903f06. The worktree is clean.

[Unqualified archive](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-tsr-09312-eff797c-57F83797-C6D74FED-UNQUALIFIED) contains both candidate banks, fixture DLLs, source and closing-head snapshots, the retained date-history bundle, scripts/readings/receipts, packaging metadata, exact runtime support DLLs, all new pairing sessions, the partial camera capture and its crash XML/minidump. The old failed unlit session remains in the 093-10 archive. Other raw captures remain in _r9312, _r9311 and _r939. Every archived payload rehashed; a one-byte mutant was rejected with its original unchanged.

| Archived source | SHA-256 |
|---|---|
| product-source.zip | 9c03f16e9b1c4a9a738a061b4207acb57548f764216ed4b825dc7ae513a31e6d |
| qualification-head.zip | 0dc5b7b4c6c4f55cd1c36890a2a215d009df100547ab7142e558e59b3f8a64a1 |
| pre-recreation.bundle | 08a66ed19e208216523d82c22934c6992598a29c09eaf9126dae789a17156a02 |

| Binary / role (identical hashes grouped) | SHA-256 |
|---|---|
| stack / StackOBot.exe | 57f83797ffaa878689a593ba2d959a8601ca6b959ec5838d6c1f8921f76913a3 |
| stack / UnrealEditor-AnomalyBench.dll | 1eb5ffee75b4efb4a4322df34899cfcbe5e3a4abeb4b088d71bdaa6757d7e235 |
| stack / UnrealEditor-AnomalyCapture.dll | f8ebc8b8ed06b4ac9333f43e002fa3d4d8e79e810479a53687c5a587a1854abd |
| stack / UnrealEditor-AnomalyControlServer.dll | 719483cf2d49270015b2889f6cc1b9358a59e17b532e328555acc15d2a9805bc |
| stack / UnrealEditor-AnomalyInjector.dll | fdc74d5d55e123c7c25c3d3a6d2d454d133e7346d8e9a6c594c821399f2f9af4 |
| stack / UnrealEditor-AnomalyShaders.dll | 193b7a1322180fbf8fbc60933ce5d205f389233d3aefafc971353a381cff4977 |
| stack / UnrealEditor-Review0936.dll | f0b8d1544d326c6a77c1d4fc5002ac28cb77be26846646f2cf2767f9796bd89a |
| stack / UnrealEditor-Review0936Runtime.dll | 62e34b62c9d8010eab90c9f992655fc817f1a773a28310c12aa7ea37fd704c5a |
| lyra / LyraGame.exe | c6d74fed3e3d5d697e3f3215745f08198cd7676892616556ca9318c6ac48ee6f |
| lyra / UnrealEditor-AnomalyBench.dll | 72d86c188ca8a6e666576b7961db1dcde111282fc9a84c85d2f02a28e852d1d1 |
| lyra / UnrealEditor-AnomalyCapture.dll | 719ed0f96d3e184d4cb6cd82e80032a05f9516b816dc87999df99011f3d4ca48 |
| lyra / UnrealEditor-AnomalyControlServer.dll | e8a0368c48d741ff15075345140e2a5bb7ba632ca93fc0f4f339838cfd52c4cc |
| lyra / UnrealEditor-AnomalyInjector.dll | 3ecf5a96bb26f2f46e51fe5b8e882af9cdeeb1dd4dc1114f4e6aaaff81b144e9 |
| lyra / UnrealEditor-AnomalyShaders.dll | 1ee16db89b38f9f20212793196f2b084e11317b06815aff9cb69f7f3756fbac6 |
| lyra / UnrealEditor-Review0936.dll | 3dd1c1bacbb9f6052990d15de6dc86e25d57d107fe9e1386e80b5e57b6b06c9d |
| lyra / UnrealEditor-Review0936Runtime.dll | f7be05d2eef8f7e9c6d567cf21e46c8644ebcbe890350af6e4a8959944320cfa |
| UE5.1 UnrealEditor.exe | 5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b |
| stack / UnrealEditor-CaptureBench.dll | a7443041103affbcd06b724a1ee1cd66f30c636d7ba246d2b34e661b0e89fb51 |
| lyra / UnrealEditor-CaptureBench.dll | cbd93ae77e1a566ae6d6b10e8dd2ec183dbc2a7eaef46d709ca0951e28b6a97c |
| Lyra f608 replay / UnrealEditor-AnomalyBench.dll | 14fbc492b742173fc7da0df6815f765c92a97be236f246a1975c1f0840557132 |
| Lyra f608 replay / UnrealEditor-AnomalyCapture.dll | 9f0155f89528f6d20f070fe591b2019bdc54f7ffaecf17a99a0a6f95210b04aa |
| Lyra f608 replay / UnrealEditor-AnomalyControlServer.dll | 42c93430c786e775f293750eec50ec2ac8f924669a145d79e3e035ad8b335ca2 |
| Lyra f608 replay / UnrealEditor-AnomalyInjector.dll | 9f48c8bb1ce0fd8521e42671066df06f1ea2d5be1e5b88676db782b3307e6751 |
| Lyra f608 replay / UnrealEditor-AnomalyShaders.dll | 7804202021e708d80234762a4d3cb9acecf83eb449ee32a46291be6ee45e002d |
| lyra_P_j1_delayed / UnrealEditor-AnomalyBench.dll; stack_P_j1_delayed / UnrealEditor-AnomalyBench.dll | 2d014241ec9f65ced693d6836494ab6e85f173dc22fb24e6153263d99efaf5bc |
| lyra_P_j1_delayed / UnrealEditor-AnomalyCapture.dll; stack_P_j1_delayed / UnrealEditor-AnomalyCapture.dll | e176b8e0b4ccb2efe5089f6929fbb830147bc61abe381df47bf9602640263425 |
| lyra_P_j1_delayed / UnrealEditor-AnomalyControlServer.dll; stack_P_j1_delayed / UnrealEditor-AnomalyControlServer.dll | 10c64f857e8c271d0f44c43e8c63beaac4ee67e2cde348f918f5ad0dd4195a70 |
| lyra_P_j1_delayed / UnrealEditor-AnomalyInjector.dll; stack_P_j1_delayed / UnrealEditor-AnomalyInjector.dll | 5171ba2fbfd180ea8a7fe6618334b73655726a93db9b223f4c70f4844e35a35f |
| lyra_P_j1_delayed / UnrealEditor-AnomalyShaders.dll; stack_P_j1_delayed / UnrealEditor-AnomalyShaders.dll | 589602b04bc775caeef7ccf8b1f21eee7cb39a91de95935ed57c5249e1d4f75e |
| lyra_P_release_multi / UnrealEditor-AnomalyBench.dll; stack_P_release_multi / UnrealEditor-AnomalyBench.dll | 9f967665827cd1358b5f914466c36ecc45e47e5e972c749aa0221283bc92dfce |
| lyra_P_release_multi / UnrealEditor-AnomalyCapture.dll; stack_P_release_multi / UnrealEditor-AnomalyCapture.dll | 470ebe5f6c5d8fa11d9872392035c62c2bab2480aee462a4d4d58ac59e8a5be3 |
| lyra_P_release_multi / UnrealEditor-AnomalyControlServer.dll; stack_P_release_multi / UnrealEditor-AnomalyControlServer.dll | 0c9c329bedf22be975a09042eb242a21cda726fcab63553240a46685b1dd8cf1 |
| lyra_P_release_multi / UnrealEditor-AnomalyInjector.dll; stack_P_release_multi / UnrealEditor-AnomalyInjector.dll | 45a1512f0c7b59e321d8de97dd03e23b9bee175f503f974fb5a200f99b5a1b90 |
| lyra_P_release_multi / UnrealEditor-AnomalyShaders.dll; stack_P_release_multi / UnrealEditor-AnomalyShaders.dll | 43d78d878cd1e8cd41b23db1fb747fc5c6bd6a103cb5089bb4fb984b1afc47db |
| Lyra restored support / boost_atomic-vc142-mt-x64-1_70.dll | ef693ab6dfd013104b662104d2b02e7b56aacd3b50d9d2288217abc3a0dc8765 |
| Lyra restored support / boost_chrono-vc142-mt-x64-1_70.dll | 17d6e4f679da378aa77d4a9432a0867edbc520b23ee53869374d4baf8f712f26 |
| Lyra restored support / boost_iostreams-vc142-mt-x64-1_70.dll | 7ecefd98c99abd9dd67bd8df7d17aa79464b0a4a75eef60777686aa851dfa441 |
| Lyra restored support / boost_program_options-vc142-mt-x64-1_70.dll | dcc8dff55bef77d28a0cddad029964db22263b06b6ebda7dd5c8ec76c4fb83a5 |
| Lyra restored support / boost_python39-vc142-mt-x64-1_70.dll | 9a02a7a4bc4bf3f22c7f5ad6277a3ded0ee34eb53d13a2f40151800ccadd53df |
| Lyra restored support / boost_regex-vc142-mt-x64-1_70.dll | 387dac498bbe06288e78e8f1e6013452e58084cda1e13bef0c7ceb496bc83e64 |
| Lyra restored support / boost_system-vc142-mt-x64-1_70.dll | 805efa1df34d9d7bb2903e7d7a73858b06ab765e6be345f2e58067a17cfc55f4 |
| Lyra restored support / boost_thread-vc142-mt-x64-1_70.dll | bc174c62e68c4c988386fc9cefd8facce81e2465cbe99cf0c25f596ee1ef4e48 |
| Lyra restored support / tbb.dll | 6fbb72516c91fd1085e0e9df6988c53b3e75cff5e42214920493356d8ba8745f |
| Lyra restored support / D3D12Core.dll | 40c8ab6800e0d107b73564c3e7a53f0d307b601ad15055445a31ba0ff3a793a2 |
| Lyra restored support / d3d12SDKLayers.dll | 1c271d5c7f1606447fcf5dca848436723da1a34a9ee71f0f513749819a21b052 |
| Lyra restored support / tbb.pdb | e2dc5c1533d5dbfe27920c3eb8d9ff051b4c00bcdc205f6fcad8ce17e0b0ac4d |

**10. Harness fixes, settings and restoration.**

This continuation changes tracked documentation and scratch orchestration only. Product source, office Python, permanent pairing/camera readers, fixture content, lights, materials and frozen thresholds remain unchanged. Earlier failures and reports remain retained.

| Item | Attempts and outcome |
|---|---|
| Z2 wrapper null masks | One dry-run repair. The first wrapper incorrectly demanded masks on unlabelled rows with null mask_file. It now checks every existing mask and non-null reference, while requiring a measured mask on every labelled row. The original script and failed reading are retained. No engine leg was retried; no permanent reader or gate changed |
| Admission negative-log encoding | Repair 1 adds explicit UTF-8 to the scratch checker's output-copy write after Windows cp1252 rejected an existing replacement character. The admission predicate AST is identical; the raw source log rehashed unchanged. Reassessment of the same Stack capture passes both negatives, with no engine rerun. The original script, empty output, error and failed ordered step are retained |
| Historic pairing | Both recipe attempts were used in 093-11. All new legs use the adopted historic recipe. The 093-11 stopped native50 stays a reading; its fresh replacement is explicitly authorized by Z2 |
| Exact packaging artifacts | Reinstalled the archived .target receipts and eleven support DLLs by hash, without UBT regeneration. A read-only pre-cook check found tbb.pdb also listed as RequiredResource, which -nodebuginfo does not omit. Restored its exact 6,230,016-byte payload from the engine, matching the retained _r939 receipt: e2dc5c1533d5dbfe27920c3eb8d9ff051b4c00bcdc205f6fcad8ce17e0b0ac4d. Target receipt and executable bytes remained unchanged. This completes artifact restoration before cook; it is not a metadata repair attempt or cook retry |
| Retained packaging attempts | The first UBT receipt-byte comparison failed because WriteMetadata generated a new monolithic BuildId. The second reconstructed the archived target metadata and restored exact support files. Both attempts remain spent |
| Retained PF-3 repair | Restored the original 1280x992 viewport on repair 1; both regressions have 0.0 px bbox difference. Failed setup remains banked |
| Retained marker repairs | First honored marker=false and repeated six Stack PF-2 cases. Second/final repaired camera marker retention/top placement and decoder geometry, with 60 retained-image checks. No further marker repair is authorized |
| Other retained harness repairs | Idle telemetry preserves each original deadline; owned-PID teardown waits for capture/report receipts; archived control receipts distinguish loaded helpers; PF-5 locks before framing; the missing mask-count helper and scratch line endings were repaired. Two Lyra PE-boundary string additions were attributed. Prior restoration ownership and gotcha Unicode lookups were repaired on their second attempts. All receipts remain in the preceding evidence roots |

The report helpers consolidate receipts only; they do not alter any gate. All process-local commands actually used across the retained qualification and this continuation are listed below. Full arguments, commands, times and PID identities remain in the launch receipts. Historic pairing omits all four arbiter overrides; their zero values in the table belong to other legs. C2 alone adds motion blur off. Projection constraints and FOV locks are process-local and never written to host configuration.


The camera reference is a product stop, not a harness repair attempt. Its launch record says session=null because the runner requires a finalized summary; the actual partial session is explicitly banked and archived. A read-only diagnostic script uses the frozen geometry/marker readers on the 57 partial frames and maps the crash RVA through the binary's export/runtime-function tables. It does not change a predicate or turn the partial result into a PASS. The raw XML, minidump and capture are retained. The remaining K7 work is unrun solely because this stop fired. No third pairing, packaging or marker attempt occurred.


| Command | Values used |
|---|---|
| CaptureBench.Marker | 0; 1 |
| IAI.Anomaly.StuckMipRoute | auto |
| IAI.Bench.MaskPairingProbe | 1 |
| IAI.Bench.Review936.Observe | (no arguments) |
| IAI.Bench.Review936Runtime.Ambiguous | (no arguments) |
| IAI.Bench.Review936Runtime.LateStart | (session path) png 777 360 uv_corruption StaticMeshActor_0 tile |
| IAI.Bench.Review936Runtime.Mode | controller; gameview; showflag; stereo |
| IAI.Bench.Review936Runtime.ResizeAt | 100 960 720 |
| IAI.Bench.Review936Runtime.Split | (no arguments) |
| IAI.Bench.Review938.AdmissionProbe | 10 StaticMeshActor_0 |
| IAI.Bench.Review938.Projection | 0 0 0 none; 1123 870 0 StaticMeshActor_0; 1123 870 0 none; 1123 870 1 StaticMeshActor_0; 1280 720 -1 StaticMeshActor_0; 1280 720 0 StaticMeshActor_0; 1280 720 0 StaticMeshActor_0 2.39; 1280 720 1 StaticMeshActor_0; 942 869 -1 StaticMeshActor_0; 942 869 0 StaticMeshActor_0; 942 869 1 StaticMeshActor_0; 942 869 1 none |
| IAI.Bench.Review938.Switches | 60 300 1.5 |
| IAI.Bench.Review939.LockFOV | 70 |
| IAI.Bench.SynthTickOrder | 1 |
| IAI.Bench.TexCorruptIdentity | 1 |
| IAI.Capture.Config | 0 12 12 12 4; 0 192 48 192 4; 0 30 180 60 4; 0 30 24 60 4 |
| IAI.Capture.Delivery | 0 |
| IAI.Capture.FocusGate | 0 |
| IAI.Capture.Fps | 30 |
| IAI.Capture.Mask | 1 |
| IAI.Capture.Pace | 1 |
| IAI.Capture.RunLog | 1 |
| IAI.Capture.SVE | 1 |
| IAI.Capture.Start | (session path) png 777 1100 uv_corruption StaticMeshActor_0 ; (session path) png 777 1400 stuck_low_mip StaticMeshActor_77; (session path) png 777 1400 stuck_low_mip StaticMeshActor_77 ; (session path) png 777 1400 uv_corruption StaticMeshActor_0 tile; (session path) png 777 360 uv_corruption StaticMeshActor_0 tile; (session path) png 777 80 blinking StaticMeshActor_49; (session path) png 777 80 blinking StaticMeshActor_49  |
| IAI.Capture.TargetMask | 1 |
| r.AntiAliasingMethod | 0 |
| r.DefaultFeature.AutoExposure | 0 |
| r.DynamicGlobalIlluminationMethod | 0 |
| r.DynamicRes.OperationMode | 0 |
| r.DynamicRes.TestScreenPercentage | 0 |
| r.Editor.Viewport.OverridePIEScreenPercentage | 0 |
| r.EyeAdaptationQuality | 0 |
| r.Lumen.DiffuseIndirect.Allow | 0 |
| r.MotionBlurQuality | 0 |
| r.ReflectionMethod | 0 |
| r.ScreenPercentage | 100; 50 |
| r.ScreenPercentage.MaxResolution | 0 |
| r.ScreenPercentage.MinResolution | 0 |
| r.ScreenPercentage.Mode | 0 |
| r.SecondaryScreenPercentage.GameViewport | 100 |

Final restoration returned StackOBot to **23098fa7415f148e935acbd4a97e18da04e6c46c** and Lyra to **caa68c6799db023f7a24c0d6cfc56124ce27b542**: 201/183 original files rehashed, both source trees clean and original content unchanged. All **5764/5764** live-stage hashes match. Seven Stack and 21 Lyra receipt-owned extra files, plus five installed helper/content roots, were removed by exact path/hash or own-launch GUID receipts. The exact tbb.pdb sidecar was included in that restoration. No owned engine process remains. Foreign processes and security settings were untouched. [Restoration receipt](E:/IA_BuildCache/_r9312/finish-checks.json).

All console overrides ended with their owned processes; host config files were restored. Exact archived J1/pre-F1 swaps on both fixtures and the retained f608 replay were reversed and rehashed. The longer Lyra control idle wait lasted 2933 seconds under its original four-hour deadline; the camera reference waited 352 seconds. Every engine launch followed a passing gate. D:/E: stayed above 50 GiB; before final restoration the readings were 186.79/97.13 GiB free.

Tracked changes in this continuation are the Z2 predictions, G485 extension, architecture, journal and CLAUDE.md status block. G485's heading is rechecked across all refs at commit. Comment stripping changes 0/182 files; diff checks pass. Product source and office Python are unchanged. The inherited build/test/string/lever evidence stands for the rehashed candidate; it does not override the new camera failure. Home partial qualification does not establish either office host's checkpoint.

**11. NEEDS-DECISION.** Authorize a product repair for exposure-exclusion state invalidation on owner/view changes, then prescribe renewed camera and remaining K7 qualification. The observed assertion is in FoldExposureExclusion; the source clears its backing mask while retaining the dimensions used to decide allocation. Any such product edit is explicitly outside 093-12. The 50% proof itself now passes under Z2 with its n=0 per-axis limitation reported. Keep the historic pairing recipe, fixed reader/tolerance and spent recipe/metadata/marker budgets. No further engine retry or push was made.

VERDICT: NEEDS-DECISION
