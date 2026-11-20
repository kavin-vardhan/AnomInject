Recovered the bench with byte-identical candidate binaries; PF-3, PF-4 and PF-5 passed.
Native mask-picture pairing failed: all 56 decidable samples read NEITHER.
J2 remains unqualified; hosts and staging are restored, and nothing was pushed.

**1. Recovery and stop.** Work used E:/IA_BuildCache/_r933/AnomalyInjector on fix/followup-capture. The [crash-recovery receipt](E:/IA_BuildCache/_r939/recovery09310/crash-recovery.json) records absent GIT_AUTHOR_DATE/GIT_COMMITTER_DATE variables, 23 dead-run engine process identities checked with none surviving, clean candidate host source trees, and four saved-editor configuration files restored. Candidate banks and installed binaries matched the eff797c manifests: both game EXEs, ten product DLLs, four Review helper DLLs, two CaptureBench fixture DLLs and the Editor EXE. Original content matched; three additional Lyra assets were verified against candidate Git blobs. All 5,764 live-staging hashes matched. No foreign process was stopped.

The recovered worktree was at 571f15a with four modified tracked documentation files and no uncommitted product changes. Their exact patch was banked before closure. The process and command-line inventory showed all inspected editor, build and Codex processes started after the 04:34:05 IST boot; none belonged to the dead run. Scratch harness scripts and receipts under _r939 were retained and audited.

Under X1, the completed PF-1, twelve PF-2 PIE cases, archived 0.813 replay and reader self-tests stand pending independent review. No product rebuild was needed. PF-3 repeated with the original 1280x992 viewport as its first harness repair attempt.

The stop is [stack_P_pair_native_100](E:/IA_BuildCache/_r939/legs/stack_P_pair_native_100/pairing-assessment.json): r=80, w=80, CURRENT=0, PREVIOUS=0, NEITHER=56, same-position=7, no-data=17. Ownership dropped/unpaired/pending counts are zero; admission counters are 0/0. The permanent predicate requires PREVIOUS=NEITHER=0. At sample 2 the reader reports picture centroid 435.6 and mask centroid 355.8 against the unchanged 40 px tolerance. Both images are 1280x992.

Read-only inspection found a full mask silhouette and a smaller visible magenta region. Motion blur was enabled in the log, but no experiment establishes it as the cause. **Cause is not established.** No proven harness defect justified a retry; no pairing repair, threshold change or product change was attempted. D2 stopped later engine work immediately. [Diagnosis and retained hashes](E:/IA_BuildCache/_r939/recovery09310/stop-diagnosis.json) distinguish the observation from possible explanations.

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

The recovery continuation itself committed only CLAUDE.md, architecture, gotchas and the 093-09/093-10 journals. G483 was checked across all 71 refs immediately before commit. Earlier documentation also corrected the readme to require Play (PIE or a game build) for object anomalies, and replaced the two literal question marks identified in the review.

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

The supplemental console no-projection probe remains specified but unrun after the stop. Its expected refusal and subsequent healthy-event floors were not changed.

**5. Gates and can-fails.** r/w means requested/written; v means independently visible events; e is maximum measured edge error. O/N means targeted_refused_off_screen / targeted_refused_no_projection. NR means not run after D2. Synthetic alignment certifies identity/membership; native companions certify RGB edges.

| Gate | PIE result | Staged result | Can-fail |
|---|---|---|---|
| PF-1, exact Lyra 942x869 Y off-left arrangement | PASS: 360/360, zero visible rows, zero texture/generic applications, O/N=4/0 | Not specified separately | Retained 093-07 four-fire session and 093-06 96-false-row session reject; non-texture apply mutant rejects |
| PF-2 | PASS 12/12; each 360/360, v4, e0, 96 measured paired rows, O/N=0/0 | NR: four required cases | Per-axis 0.96 mutants reject; old Stack box reading passes, old Lyra 0.967 and measured 0.813 reject |
| PF-3 UV and proxy | PASS: each 1152/1152, v4, e0, 192 measured rows, O/N=0/0; events/targets/edges/visible_positive identical; bbox maximum difference 0.0 px | NR | One-pixel box and visible-onset mutants reject |
| PF-4 letterbox | PASS: 360/360, v4, e0, n96, b1000, O/N=0/0; viewport 1280x720, copied rect [0,92,1280,536], aspect 2.39 | Not specified | X/Y scale and full-window mapping mutants reject |
| PF-5 locked FOV | PASS: 360/360, v4, e0, n96, b1000, O/N=0/0; all labels 70 | Not specified | Observed old-route 90 rejects locked 70; per-axis box mutants reject |
| Camera cut/blend, both fixtures | NR | NR | Identity-self and stale-projection controls prepared, not run |
| Supplemental admission probes, both fixtures | NR | NR | Counter/application mutants prepared, not run |
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

The complete [84-case J2 ledger](E:/IA_BuildCache/_r939/qualification09310.json) has **32 PASS, one FAIL, 51 NR**. The cells below report the case reader result; a positive reader result does not substitute for an unrun archived control.

| Case | Stack PIE | Lyra PIE | Stack staged | Lyra staged | Can-fail / note |
|---|---|---|---|---|---|
| Delayed viewport | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | NR | NR | Exact J1 delayed engine controls NR; owner-state CPU mutants reject |
| Healthy single view | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | NR | NR | Three forced NotGameView negatives below pass |
| New window + realtime editor view | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A (PIE case) | N/A (PIE case) | Wrong-view=0 and non-owner rejection>0; exact release engine controls NR |
| Selected viewport | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A (PIE case) | N/A (PIE case) | World/target binding mutants reject; old release control NR |
| Two-client, client0 | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A (PIE case) | N/A (PIE case) | Independent world/frame markers; cross-world binding mutants reject |
| Two-client, client1 | PASS 360/360, v4, e0, O/N=0/0 | PASS 360/360, v4, e0, O/N=0/0 | N/A (PIE case) | N/A (PIE case) | Independent world/frame markers; cross-world binding mutants reject |
| Resize | PASS 360/358, v4, e0, O/N=0/0 | PASS 360/358, v4, e0, O/N=0/0 | NR | NR | Generation and named-drop receipt mutants reject (CPU) |
| Two same-world displays | PASS 360/0, O/N=0/4 | PASS 360/0, O/N=0/4 | NR | NR | Forced ambiguity refuses; first-wins owner-selection mutant rejects |
| Split-screen | PASS 360/0, O/N=0/4 | PASS 360/0, O/N=0/4 | NR | NR | Forced multiple views refuse; multi-view predicate mutant rejects |
| Stereo | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR | Forced stereo refuses |
| Negative controller ID | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR | Specific NotGameView sub-reason counted |
| Cleared game-thread view flag | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR | Specific NotGameView sub-reason counted |
| Cleared Game show flag | PASS 360/0, O/N=0/0 | PASS 360/0, O/N=0/0 | NR | NR | Specific NotGameView sub-reason counted |
| Native UV regression | PASS 1152/1152, v4, e0, O/N=0/0 | PASS 1152/1152, v4, e0, O/N=0/0 | NR | NR | Ten per-leg defects reject; Part0 wrong-object proof retained |
| Native proxy regression | PASS 1152/1152, v4, e0, O/N=0/0 | PASS 1152/1152, v4, e0, O/N=0/0 | NR | NR | Ten per-leg defects reject; Part0 wrong-object proof retained |
| Synthetic UV alignment | PASS 1152/1152, identity events=4, O/N=0/0 | NR | NR | NR | Completed Stack mask/label/frame identity copies reject; Lyra NR |
| Synthetic proxy alignment | PASS 1152/1152, identity events=4, O/N=0/0 | NR | NR | NR | Completed Stack mask/label/frame identity copies reject; Lyra NR |
| Native pairing 100% | FAIL 80/80, C/P/N=0/0/56, O/N=0/0 | NR | NR | NR | Positive failed before shifted-mask control |
| Native pairing 50% | NR | NR | NR | NR | Shifted-mask control NR |
| Synthetic pairing 100% | NR | NR | NR | NR | Shifted-mask control NR |
| Synthetic pairing 50% | NR | NR | NR | NR | Shifted-mask control NR |
| Exact J1 delayed control | NR | NR | NR | NR | NR |
| Pre-F1 wrong-view control | NR | NR | NR | NR | PIE multi/staged wrong-target controls NR |

Non-owner scene rejections in the two multi-view positives were 2,476 for Stack and 2,502 for Lyra, with zero wrong-view frames. Both ambiguity cases counted 730 ambiguous-owner checks; both split cases counted 366 multiple-view checks; both stereo cases counted 728 stereo checks. The controller sub-reason counted 1,067 for Stack and 1,068 for Lyra; the game-thread and show-flag negatives each counted 1,068 on both fixtures. All six refusal cases per fixture wrote zero frames, counted 360 dropped/unpaired samples, and finished with zero pending samples. These counters count checks, not unique frames.

Each native UV/proxy regression has ten rejecting controls: onset/offset ±1, missing mask, blank mask, metadata count mismatch, erased marker, corrupt dropped-summary, and a physical one-pixel mask deletion. Originals rehash unchanged. Part 0 supplies the wrong-object can-fail. The two completed Stack synthetic alignment legs reject copied mask identity, label identity and donor-frame identity defects. The failed pairing leg stopped before its shifted-mask control.

Resize accounting:

| Leg | r | w | Named drops | Missing indices | Owner updates | Stale-generation rejections | Stale/mismatched accepted | O/N |
|---|---:|---:|---|---|---:|---:|---:|---|
| Stack PIE resize | 360 | 358 | owner_render_missing:2 | 98,99 | 1 | 6 | 0 | 0/0 |
| Lyra PIE resize | 360 | 358 | owner_render_missing:2 | 98,99 | 1 | 6 | 0 | 0/0 |
| Both staged resize cases | NR | NR | NR | NR | NR | NR | NR | NR |
| Both fixtures' PIE/staged camera switches | NR | NR | NR | NR | NR | NR | NR | NR |

Both completed resize legs have v4/e0, 96 measured paired rows, b1000, zero unexplained drops/overlap/pending samples, and only 960x720 pictures after sample 100. No camera-switch drop or event-continuation reading is claimed. The CPU accounting self-test includes pending pre-switch indices 59,60,299; removed reasons/receipts and written/dropped overlap reject.

Every measured candidate leg's O/N values are in the PF and J2 tables. The superseded six original Stack PF-2 captures and first PF-3 UV setup also had 0/0. The old f608d56 replay has no admission fields; its counters are unavailable, not zero. All unrun cases have no readings.

**6. Office reader.** [office_checkpoint_i.py](E:/IA_BuildCache/_r933/AnomalyInjector/tools/office_checkpoint_i.py) and [its documentation](E:/IA_BuildCache/_r933/AnomalyInjector/tools/office_checkpoint_i.md) remove accepted_ambiguous/a=, count null measured masks as m, write a 20-frame numbered contact sheet beside the session, and measure edges. No handwritten inspection input is required. The owner appends g=0 after inspecting the local contact sheet. Host 1 is the first office host; host 2 is the second office host. These self-tests use home sessions, not office data.

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

Expected exits are 0,1,0,1. The healthy line comes from the new marker-corrected Stack PF-2 session. The null-mask failure is the retained 093-06 Lyra session; box PASS/FAIL use retained f608d56 Stack/Lyra sessions. Eight copied-input controls reject: null mask, wrong mask count, missing ownership, missing projection, invalid view, X-only/Y-only 4% scaling and shifted onset. Originals rehash unchanged. The home contact sheet was inspected; all 20 frames show the fixture. This is not an office owner's g reading. The additional scheduled J2-session reader repeat was not reached.

**7. Diagnostics.** Example actual projection block from the passing letterbox session; this is in run_summary.json only, never labels:

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

**8. Builds, audits, heads and archive.** The unchanged candidate retains all eight normal/strict Editor/Game builds on both fixtures, each with zero errors/warnings. Strict builds compiled 71 standalone header TUs per target and restored build settings. All 69 CPU steps, nine family-test steps and five ownership mutants have expected outcomes. Source lever audit covers 53 registrations and 12 self-tests; binary audits verify both configurations and the rebuilt helpers, including 11 runtime and 16 Editor commands with Lyra Switches present. String scans have zero unresolved additions, with unknown ASCII/UTF-16 sentinels rejected. Comment stripping changed 0 of 182 files; diff checks passed.

W4 date recreation preserved trees, messages and author identity, with both date environment variables absent. New author/committer timestamps are 2026-10-15 00:40:53 +05:30 for the first two and 00:40:54 for the third. The old chain is retained in a bundle; nothing was deleted or pushed.

| Old commit | Recreated commit |
|---|---|
| 7f4329c462aa779c1703420876d9492b367aaef8 | 06740850141876fea2b57801a084fd39c85a879b |
| 41c1a0c9d54c807b62bdf55a905783a7ea9e26d0 | d37ab4995562ae261591c1fed8b820f670a20425 |
| 1071ca13f4f4e00d744e394203f1063fe772cab8 | 36fec5fef2ab267648544bc5a40d06998d5e76a2 |

Local metadata fix: 3dc0c98d1ee932bac1672d5fd832495dda77d7a3. Compiled snapshot: eff797cf8dae0c886556e742cae4fe98f59458e1. Pre-crash documentation head: 571f15a067d9f9d078ec77e887b22cb550540866. New closing documentation commit: **7d5e5a45ccf27c57d1615bb3780a7c5145b32c15**, docs(capture): record J2 crash recovery and native pairing stop.

**Commits pushed: none.** Remote fix/followup-capture remains 1bf7054f5b8664f057282ed863a62ab5dbbd60e3. No merge, tag, force-push or natural-cause code. The worktree is clean. Source, Shaders and tools at the closing head equal the compiled candidate. Source tree: 2f639a77861feb8dce828ff15a9d768733c94cb6; shader tree: 64e18bf3d238c0c10817c34ed03fcb24f5903f06.

[Unqualified archive](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-recovery-09310-eff797c-57F83797-C6D74FED-UNQUALIFIED) contains both candidate banks, fixture DLLs, compiled-source and closing-head snapshots, the old-history bundle, scripts/receipts, and the complete failed pairing session. Files rehashed successfully; a one-byte mutant was rejected and its original remained unchanged. Raw remaining captures stay under E:/IA_BuildCache/_r939.

Archive source hashes:

~~~text
product-source.zip       9c03f16e9b1c4a9a738a061b4207acb57548f764216ed4b825dc7ae513a31e6d
qualification-head.zip   468769e078a069d3a59dd3bb32f3a194cfb1c78383119f99cc2e989d41e54f85
pre-recreation.bundle    08a66ed19e208216523d82c22934c6992598a29c09eaf9126dae789a17156a02
~~~

EXE/DLL SHA-256 values:

| Fixture / file | SHA-256 |
|---|---|
| Stack / StackOBot.exe | 57f83797ffaa878689a593ba2d959a8601ca6b959ec5838d6c1f8921f76913a3 |
| Stack / UnrealEditor-AnomalyBench.dll | 1eb5ffee75b4efb4a4322df34899cfcbe5e3a4abeb4b088d71bdaa6757d7e235 |
| Stack / UnrealEditor-AnomalyCapture.dll | f8ebc8b8ed06b4ac9333f43e002fa3d4d8e79e810479a53687c5a587a1854abd |
| Stack / UnrealEditor-AnomalyControlServer.dll | 719483cf2d49270015b2889f6cc1b9358a59e17b532e328555acc15d2a9805bc |
| Stack / UnrealEditor-AnomalyInjector.dll | fdc74d5d55e123c7c25c3d3a6d2d454d133e7346d8e9a6c594c821399f2f9af4 |
| Stack / UnrealEditor-AnomalyShaders.dll | 193b7a1322180fbf8fbc60933ce5d205f389233d3aefafc971353a381cff4977 |
| Stack / UnrealEditor-Review0936.dll | f0b8d1544d326c6a77c1d4fc5002ac28cb77be26846646f2cf2767f9796bd89a |
| Stack / UnrealEditor-Review0936Runtime.dll | 62e34b62c9d8010eab90c9f992655fc817f1a773a28310c12aa7ea37fd704c5a |
| Lyra / LyraGame.exe | c6d74fed3e3d5d697e3f3215745f08198cd7676892616556ca9318c6ac48ee6f |
| Lyra / UnrealEditor-AnomalyBench.dll | 72d86c188ca8a6e666576b7961db1dcde111282fc9a84c85d2f02a28e852d1d1 |
| Lyra / UnrealEditor-AnomalyCapture.dll | 719ed0f96d3e184d4cb6cd82e80032a05f9516b816dc87999df99011f3d4ca48 |
| Lyra / UnrealEditor-AnomalyControlServer.dll | e8a0368c48d741ff15075345140e2a5bb7ba632ca93fc0f4f339838cfd52c4cc |
| Lyra / UnrealEditor-AnomalyInjector.dll | 3ecf5a96bb26f2f46e51fe5b8e882af9cdeeb1dd4dc1114f4e6aaaff81b144e9 |
| Lyra / UnrealEditor-AnomalyShaders.dll | 1ee16db89b38f9f20212793196f2b084e11317b06815aff9cb69f7f3756fbac6 |
| Lyra / UnrealEditor-Review0936.dll | 3dd1c1bacbb9f6052990d15de6dc86e25d57d107fe9e1386e80b5e57b6b06c9d |
| Lyra / UnrealEditor-Review0936Runtime.dll | f7be05d2eef8f7e9c6d567cf21e46c8644ebcbe890350af6e4a8959944320cfa |
| stack / UnrealEditor-CaptureBench.dll | a7443041103affbcd06b724a1ee1cd66f30c636d7ba246d2b34e661b0e89fb51 |
| lyra / UnrealEditor-CaptureBench.dll | cbd93ae77e1a566ae6d6b10e8dd2ec183dbc2a7eaef46d709ca0951e28b6a97c |
| UE5.1 UnrealEditor.exe | 5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b |
| Lyra f608 replay / UnrealEditor-AnomalyBench.dll | 14fbc492b742173fc7da0df6815f765c92a97be236f246a1975c1f0840557132 |
| Lyra f608 replay / UnrealEditor-AnomalyCapture.dll | 9f0155f89528f6d20f070fe591b2019bdc54f7ffaecf17a99a0a6f95210b04aa |
| Lyra f608 replay / UnrealEditor-AnomalyControlServer.dll | 42c93430c786e775f293750eec50ec2ac8f924669a145d79e3e035ad8b335ca2 |
| Lyra f608 replay / UnrealEditor-AnomalyInjector.dll | 9f48c8bb1ce0fd8521e42671066df06f1ea2d5be1e5b88676db782b3307e6751 |
| Lyra f608 replay / UnrealEditor-AnomalyShaders.dll | 7804202021e708d80234762a4d3cb9acecf83eb449ee32a46291be6ee45e002d |

The five replay product DLLs come from the [f608d56 unqualified archive](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-09306-f608d56-4BB0F074-B0F2B30C-UNQUALIFIED). Its historical helper DLLs were not loaded; current helpers above were used. The game EXEs were built and archived but no staged game leg ran. The J1 E41ABF93 and release 218BA4C7 engine controls remain unrun in this qualification.

**9. Harness fixes, settings and restoration.**

| Item | Action and outcome |
|---|---|
| PF-3 viewport | First repair: restore 1280x992; both repeats have exact 0.0 px deltas. Original failed setup and unchanged comparator retained |
| PF-2 marker setting | First repair honors marker=false; repeat all six Stack cases. Original Y/1280 failure retained; marker overlap supplied 88.12% of clean-frame RGB difference |
| Marker/camera geometry | Second and final marker repair prepared camera marker retention/top placement; completed decoder calibration from fixed fixture geometry and axis constraint. 60 retained-image preflight checks pass; camera runtime validation remains unrun |
| Idle telemetry | First repair treats empty GPU samples as unavailable, preserving the original four-hour deadline and failed prelaunch evidence |
| Owned-process teardown | First repair closes only verified owned PIDs after atomic capture completion/report receipts and five seconds. Later unexplained input timestamps still enforce the full idle gate |
| Archived-control manifest | First repair distinguishes actually loaded current helpers from historical archive helper metadata |
| PF-5 framing | Lock before framing, committed before its leg; old spec retained |
| Pre-leg support fixes | Copied the unchanged missing mask-count helper; normalized own scratch line endings; corrected empty-unresolved-list string reviewer handling; attributed two Lyra PE-boundary constant/pointer strings |
| Recovery content membership | Verified three extra candidate-tracked Lyra assets against Git blobs; retained the initial diagnostic |
| Pairing failure | No repair attempted; cause unresolved. Reader and 40 px tolerance unchanged |
| Final reporting | Complete 84-case ledger includes controls whose launch specs were never created; the completed 0.813 negative is recorded from its swap receipt |

Full per-launch commands and PID identities are retained. The actual changed setting groups and values were:

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
| IAI.Bench.Review938.Projection | 0 0 0 none; 1123 870 0 StaticMeshActor_0; 1123 870 0 none; 1123 870 1 StaticMeshActor_0; 1280 720 0 StaticMeshActor_0; 1280 720 0 StaticMeshActor_0 2.39; 1280 720 1 StaticMeshActor_0; 942 869 -1 StaticMeshActor_0; 942 869 0 StaticMeshActor_0; 942 869 1 StaticMeshActor_0; 942 869 1 none |
| IAI.Bench.Review939.LockFOV | 70 |
| IAI.Bench.SynthTickOrder | 1 |
| IAI.Capture.Config | 0 12 12 12 4; 0 192 48 192 4; 0 30 24 60 4 |
| IAI.Capture.Delivery | 0 |
| IAI.Capture.FocusGate | 0 |
| IAI.Capture.Fps | 30 |
| IAI.Capture.Mask | 1 |
| IAI.Capture.Pace | 1 |
| IAI.Capture.RunLog | 1 |
| IAI.Capture.SVE | 1 |
| IAI.Capture.Start | (session path) png 777 1400 stuck_low_mip StaticMeshActor_77; (session path) png 777 1400 uv_corruption StaticMeshActor_0 tile; (session path) png 777 360 uv_corruption StaticMeshActor_0 tile; (session path) png 777 80 blinking StaticMeshActor_49 |
| IAI.Capture.TargetMask | 1 |
| r.AntiAliasingMethod | 0 |
| r.DefaultFeature.AutoExposure | 0 |
| r.DynamicGlobalIlluminationMethod | 0 |
| r.DynamicRes.OperationMode | 0 |
| r.DynamicRes.TestScreenPercentage | 0 |
| r.Editor.Viewport.OverridePIEScreenPercentage | 0 |
| r.EyeAdaptationQuality | 0 |
| r.Lumen.DiffuseIndirect.Allow | 0 |
| r.ReflectionMethod | 0 |
| r.ScreenPercentage | 100 |
| r.ScreenPercentage.MaxResolution | 0 |
| r.ScreenPercentage.MinResolution | 0 |
| r.ScreenPercentage.Mode | 0 |
| r.SecondaryScreenPercentage.GameViewport | 100 |

PIE modes were window, multi, selected and client 2; ExecClient selected 0/1. Launches used -IAIBench, -unattended and -NoLiveCoding; Lyra's GameModeBase override was process-local. Native PIE viewport 1280x992, the three PF-2 sizes, PF-4 aspect 2.39, resize 960x720 and locked FOV 70 are all recorded above. The prepared camera Marker.Top/Switches and admission-probe commands were not executed. Render constraints were never written to host config. Archived replay DLL swaps were reversed and rehashed.

Final restoration returned StackOBot to 23098fa7415f148e935acbd4a97e18da04e6c46c and Lyra to caa68c6799db023f7a24c0d6cfc56124ce27b542, with 201/183 original files rehashed, both source trees clean and original content matching. All 5,764 live-stage hashes match. Reviewed job-created files and five owned helper/content roots were removed by exact-path/hash receipts; every new crash-client config GUID matched an owned launch log. No owned engine process remains; no foreign process or security setting was changed. [Final restoration receipt](E:/IA_BuildCache/_r939/finish-checks.json).

The remaining PIE cases, camera readings, cook, staged matrix and push are omitted solely because the native pairing prediction failed. Completed home evidence is not office qualification.

**10. NEEDS-DECISION.** Resolve whether the native pairing mismatch is a fixture/reader problem or a product defect, authorize the appropriate repair, and rule on continuation from this stop while preserving the permanent pairing predicate and K7 order. J2 cannot be qualified or pushed from these partial results.

VERDICT: NEEDS-DECISION
