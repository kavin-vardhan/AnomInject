The exposure-exclusion repair is implemented; both new Stack camera runs survived the cut and blend.  
The full camera gate stopped because its frozen negative control requires sample 299, which W2 correctly dropped.  
J2 remains unqualified: hosts and staging are restored, an unqualified archive is retained, and nothing was pushed.

**Preflight and commits.** Work used `E:/IA_BuildCache/_r933/AnomalyInjector`, branch `fix/followup-capture`, initially clean at `7049614542edb868458b1223f6397d74a3024b59`. The eff797c baseline banks and archive rehashed identically, including both game EXEs, ten product DLLs and the helpers. Hosts, content and all 5,764 live-staging files matched the preceding restoration receipt. Both Git date overrides were absent. The preceding report was preserved as `report-093-12.md`. [Preflight receipt](E:/IA_BuildCache/_r9313/preflight.json).

| Local commit | Time, IST on 2026-10-16 | Purpose |
|---|---|---|
| `ef03f36e07bb68c2e5ffd9c3f28fcb5820896836` | 00:11:01 | Design and predictions, before product edits |
| `978103163c0a1839bd6948614818b73f07674c72` | 00:14:14 | Sole product commit: `fix(capture): reset exposure exclusion state on owner changes` |
| `0bc6e9f46cc2d15ddee44548db62f31757d54783` | 01:33:38 | Closing journal, architecture, status and G486 |

The first new engine leg started at **01:13:04.463 IST**, after the freeze, fix, builds, audits and idle gate. No commit was pushed. Remote remains `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`.

**Scoped repair and CPU proof.** The [design](E:/IA_BuildCache/_r933/AnomalyInjector/docs/sessions/2026-10-16-093-13-exposure-exclusion-design.md) inventories construction, run initialization, ownership invalidation and fold allocation. The only product changes are [AnomalyCaptureSubsystem.cpp](E:/IA_BuildCache/_r933/AnomalyInjector/Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp) and [its header](E:/IA_BuildCache/_r933/AnomalyInjector/Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h): **17 insertions, 10 deletions**.

`ResetExposureExclusion()` resets the mask, folded-event set, width, height and pixel count together. Every reset site uses it. The fold preserves shape-change clearing, including a new shape with the same pixel count, and allocates whenever actual `Num() != W*H`. An emptied same-size mask cannot bypass allocation. Dimension/payload guards, event union and reserved probe-tag exclusion are unchanged. No engine edit, dependency, private header, label field or summary field was added.

The existing standalone harness hosted a CPU test by compiling the actual extracted old/new function bodies and owner-reset statements with container shims. [Results](E:/IA_BuildCache/_r9313/exposure-selftest-r1/result.json):

| CPU case | eff797c | 9781031 |
|---|---|---|
| Same-size owner change, then fold | Rejects/fails | PASS |
| Owner reset clears all five members | Rejects/fails | PASS |
| Allocation follows actual element count | Rejects/fails | PASS |
| Repeated-event suppression | PASS | PASS |
| Shape change with equal pixel count | PASS | PASS |

The retained 093-12 engine assertion is the real-engine can-fail and was **not rerun**. That J2 block was absent from pushed J1 and the delivered release.

**Builds and audits.** All eight builds passed with **0 errors and 0 warnings**:

| Fixture | Normal Editor | Strict Editor | Normal Game | Strict Game |
|---|---|---|---|---|
| StackOBot | PASS 0/0 | PASS 0/0 | PASS 0/0 | PASS 0/0 |
| Lyra | PASS 0/0 | PASS 0/0 | PASS 0/0 | PASS 0/0 |

Each strict target compiled 71 standalone header TUs; build settings restored exactly. The 69 pure-suite steps, nine family cases and five ownership mutants passed their expected outcomes. Binary/lever audits passed, including 11 runtime and 16 Editor helper commands. Both-encoding string scans left zero unresolved additions; injected ASCII and UTF-16 unknowns rejected. Lyra's raw `pnullptr` finding was a verified filename/literal byte boundary, not a new product string. Comment stripping changed 0 of 182 files; whitespace and scope checks passed. [Build/audit receipt](E:/IA_BuildCache/_r9313/closing-audit09313.json).

**New camera results and stop.** Both new specs are unchanged from the retained camera specifications: cut at sample 60, blend at 300 over 1.5 seconds. The projection helper sets 1280x720; the new summaries report that actual output. Both runs completed and produced final summaries; exit 1 in their launch receipts is owned-PID teardown after completion, not an engine crash.

| New leg | r / w | Named drops | Measured masks | PF-2 X / Y | Admission O/N | Full result |
|---|---|---|---|---|---|---|
| Stack PIE identity-mode reference, session 011352 | 990 / 984 | 6 | 714 labelled rows | Reference check PASS | 0/0 | Reference PASS, no crash |
| Stack PIE active, session 011816 | 990 / 984 | 6 | 714/714 | 0.9999999404 / 1.0000000000, n714 each | 0/0 | **FAIL: negative-control donor absent** |
| Lyra PIE reference/active | NR | NR | NR | NR | NR | Stopped before launch |
| Stack staged reference/active | NR | NR | NR | NR | NR | Cook/gates not reached |
| Lyra staged reference/active | NR | NR | NR | NR | NR | Cook/gates not reached |

`O/N` means `targeted_refused_off_screen / targeted_refused_no_projection`. `NR` means not run, not zero.

Both Stack runs reconcile **990 = 984 + 6**. Missing samples are **57, 58, 59, 297, 298, 299**, each with exactly one `owner_render_missing` retirement. Written/dropped overlap, unexplained drops, pending-at-stop and accepted stale/frame/world mismatches are all **0**. The reference/active stale-generation rejection readings are 14/17; these are rejected checks, not accepted stale frames. Each generation reached 3 with two owner updates.

The active run has four independently visible events at [29,208], [269,448], [509,688] and [749,928]. All four onset and recovery errors are **0 frames**, with no labelled row missing its measured mask. Moving samples 301–344 read **X=1.0007299849, Y=1.0001271308, n=44 per axis**. Annotation identity has zero errors. The identity-self negative rejects unchanged pictures.

| Switch | Pending pre-switch drops, both runs | Active event outcome | Recorded reason/readings |
|---|---|---|---|
| Cut, 60 | 3: 57–59 | `uv_corruption@175`, target StaticMeshActor_0, continued; last sample 208 | Annotation `pie_end_settle:1`; no event-ID-specific end-log match |
| Blend, 300 | 3: 297–299 | `uv_corruption@415`, same target, continued; last sample 448 | Annotation `pie_end_settle:1`; no event-ID-specific end-log match |

All four generic revert receipts report exact restoration and zero residue. The identity reference has **16 change-stage `extent_mismatch` pairs**, including four before the cut at 29–32; this is AA6-D's reading, not a gate. Both final DRAWN SUMMARY logs report exposure exclusion **6,050 px**.

The frozen camera reader hardcodes **299** as the stale-projection donor. It is absent because it was correctly retired. Sample **296** is the last written pre-blend sample. The reader therefore emits **`stale-projection can-fail has no pre-blend sample`**. No substitute donor was used, no replacement control was run, and neither the reader nor the fix was retried. This is a **harness scope stop**, not a measured new label/picture disagreement. AA5 stops the new-binary camera failure, and this request expressly forbids reader changes beyond AA4. [Unmodified gate failure](E:/IA_BuildCache/_r9313/legs/stack_P_camera/camera-assessment.json), [stop diagnosis](E:/IA_BuildCache/_r9313/stop-readings09313.json).

**Reach, identity and dedicated TSR qualification.** The fix can affect initial allocation/folding, delayed owner resolution, resize and same-size camera changes; consumers include target-excluded luminance and exposure diagnostics. The frozen reach includes camera, delayed and resize on both fixtures, plus healthy/UV identity. All remaining legs below are **NR on 9781031** because the camera stop came first.

| Reach/identity leg | Stack PIE | Lyra PIE | Comparison/control |
|---|---|---|---|
| Delayed viewport | NR | NR | Original completeness/identity floors; exact J1 negative |
| Resize | NR | NR | Original W2 named-drop accounting; missing-receipt/overlap negatives |
| Healthy identity vs eff797c | NR | NR | PF-3, masks byte-identical, all label fields identical, protected summary fields identical |
| UV identity vs eff797c | NR | NR | Same requirements; mask-byte, label-field, summary-count and PF-3 mutants |

The identity comparator freezes literal label fields, including clocks and IDs; it performs no rebasing. Pictures are a byte-identity reading. The only allowed summary differences are the design's enumerated timing/throughput, completion-latency, async-residency, drain-tick/ring, background-census/mask and non-owner-check telemetry. Counts/outcomes, admission, masks, exposure, projection, errors and caps remain exact. A timing-only summary mutant is the allowed-difference control. None of these identity comparisons or controls was reached. **The retained eff797c PIE passes have not been inherited by the new candidate.**

AA3 mentions `ExposureExclusionPx` in run_summary; inspection found it only in the existing log. This state-only repair adds no writer key. A new JSON field would exceed scope. The resize log reading is unrun.

AA4's frozen dedicated fixture uses the PF-2 target well inside the frame, historic TSR/lighting defaults, 50% screen percentage, output 1280x720, Stack X/Lyra Y constraint, and 360 requested frames. The unchanged estimator must have n>=20 and scale [0.990,1.010] **on each axis**, with each 0.96 mutant rejected. Fewer rows is a stop.

| Dedicated TSR50 leg | X n / scale | Y n / scale | 0.96 controls | O/N |
|---|---|---|---|---|
| Stack PIE | NR / NR | NR / NR | NR | NR |
| Lyra PIE | NR / NR | NR / NR | NR | NR |
| Stack staged | NR / NR | NR / NR | NR | NR |
| Lyra staged | NR / NR | NR / NR | NR | NR |

**Retained PF gates and full J2 matrix.** The following results belong to **eff797c**, preserved as evidence; they are not new-binary passes. Every new staged gate is unrun. The prior 84-case matrix contains 46 PIE PASS and 38 staged NR, with the separate camera requirement stopping overall qualification. [Ledger](E:/IA_BuildCache/_r9312/qualification09312.json).

| Gate | Retained PIE result | Staged status | Can-fail result |
|---|---|---|---|
| PF-1, exact Lyra 942x869 Y/off-left | 360/360; visible rows=0; generic/texture applications=0; O/N=4/0 | Not separately specified | Old four-fire and 96-false-row captures reject; generic-apply mutant rejects |
| PF-2, both fixtures × X/Y × three sizes | 12/12 PASS; each 360/360, v4, e0, 96 measured rows, O/N=0/0 | Four 1280x720 cases NR, both fixtures × X/Y | Each 0.96 axis mutant rejects; archived Lyra 0.967 and 0.813 reject; Stack passes |
| PF-3, Stack UV/proxy | Each 1152/1152, v4, e0, 192 measured rows, O/N=0/0; same events/targets/visible_positive; box delta=0.0 px | NR | One-pixel and onset/visibility mutants reject |
| PF-4, Stack letterbox | 360/360, v4, e0, n96, b1000, O/N=0/0; copied rect [0,92,1280,536], aspect 2.39 | Not separately specified | Scale/full-window mutants reject |
| PF-5, Stack locked FOV | 360/360, v4, e0, b1000, O/N=0/0; every fovDeg=70 | Not separately specified | Observed old-route cache=90 rejects locked 70; box mutants reject |

The archived f608d56 0.813 replay measured **X=0.8130053613, Y=0.8130053403, n96 each, b813**, and rejected. It used the exact archived binary and restored all five DLLs afterward. PF-3's original 1280x992 setup was restored in 093-10; the 0.5 px threshold never changed.

In the matrix, `r/w; v/e; O/N` gives requests/writes, visible events/maximum edge error, and admission counters. Refusal cases intentionally write zero. Synthetic alignment certifies identity/membership; native companions certify RGB visibility. All new PIE matrix reruns are NR after the camera stop.

| Case | Retained Stack PIE | Retained Lyra PIE | New Stack staged | New Lyra staged | Can-fail evidence |
|---|---|---|---|---|---|
| Delayed viewport | 360/360; 4/0; 0/0 | 360/360; 4/0; 0/0 | NR | NR | Exact J1 writes 0/360 |
| Healthy single view | 360/360; 4/0; 0/0 | 360/360; 4/0; 0/0 | NR | NR | Wrong-view/J1 controls; world/frame negatives |
| Second realtime editor view | 360/360; 4/0; 0/0 | 360/360; 4/0; 0/0 | N/A | N/A | Exact pre-F1 wrong view: 360 frames each |
| Selected viewport | 360/360; 4/0; 0/0 | 360/360; 4/0; 0/0 | N/A | N/A | Cross-world binding negative |
| Two-client client0 | 360/360; 4/0; 0/0 | 360/360; 4/0; 0/0 | N/A | N/A | Cross-world binding negative |
| Two-client client1 | 360/360; 4/0; 0/0 | 360/360; 4/0; 0/0 | N/A | N/A | Cross-world binding negative |
| Resize | 360/358; 4/0; 0/0 | 360/358; 4/0; 0/0 | NR | NR | Missing-reason/receipt and overlap mutants reject |
| Same-world ambiguity | 360/0; —; 0/4 | 360/0; —; 0/4 | NR | NR | Forced ambiguous displays refused and counted |
| Split-screen | 360/0; —; 0/4 | 360/0; —; 0/4 | NR | NR | Forced split view refused and counted |
| Stereo | 360/0; —; 0/0 | 360/0; —; 0/0 | NR | NR | Forced stereo refused and counted |
| Negative controller ID | 360/0; —; 0/0 | 360/0; —; 0/0 | NR | NR | Controller sub-reason exercised |
| Non-game-thread view | 360/0; —; 0/0 | 360/0; —; 0/0 | NR | NR | Game-thread-view sub-reason exercised |
| Game show flag off | 360/0; —; 0/0 | 360/0; —; 0/0 | NR | NR | Show-flag sub-reason exercised |
| Native UV regression | 1152/1152; 4/0; 0/0 | 1152/1152; 4/0; 0/0 | NR | NR | Ten timing/mask/marker/drop/pixel defects reject |
| Native proxy regression | 1152/1152; 4/0; 0/0 | 1152/1152; 4/0; 0/0 | NR | NR | Same ten copied defects reject |
| Synthetic UV alignment | 1152/1152; 4 events; 0/0 | 1152/1152; 4 events; 0/0 | NR | NR | Mask/event/donor-barcode mutants reject |
| Synthetic proxy alignment | 1152/1152; 4 events; 0/0 | 1152/1152; 4 events; 0/0 | NR | NR | Mask/event/donor-barcode mutants reject |
| Native pairing 100% | 80/80; 0/0 | 80/80; 0/0 | NR | NR | Shifted-mask copies reject |
| Native pairing 50% | 80/80; 0/0 | 80/80; 0/0 | NR | NR | Shifted masks, 100% log, wrong width reject |
| Synthetic pairing 100% | 80/80; 0/0 | 80/80; 0/0 | NR | NR | Shifted-mask copies reject |
| Synthetic pairing 50% | 80/80; 0/0 | 80/80; 0/0 | NR | NR | Shifted masks, 100% log, wrong width reject |
| J1 delayed negative | 360/0; O/N unavailable | 360/0; O/N unavailable | NR | NR | Exact E41ABF93 build fails completeness |
| Pre-F1 wrong-view negative | 360/360; O/N unavailable | 360/360; O/N unavailable | NR | NR | Exact 218BA4C7 build fails view identity |

The retained second-view cases have zero wrong-view frames and Stack/Lyra non-owner rejection counts 2476/2502. Ambiguity/split count 730/366 checks on each fixture; stereo counts 728. Controller sub-rejections are 1067/1068, and game-thread/Game-show-flag sub-rejections are 1068 each. The exact pre-F1 controls show the wrong view on all 360 frames; J1 delayed controls write zero of 360. All old-build swaps were reversed and rehashed. Unavailable old-build admission fields are not zeros.

Both retained supplemental admission probes pass 360/360, v4/e0, O/N=0/1; the unavailable-projection attempt is refused before mutation and restored at sample 10. Counter/application mutants reject. Both new staged probes are NR.

**Pairing evidence and Z2.** No pairing leg was launched in 093-13. The reader remains SHA-256 `54928b2ecce48d97a1af4bea16b00184ca091e8b22670fe68e8e3165671f3525`, tolerance 40 px, PREVIOUS=NEITHER=0, decidable floor 1. Historic recipe means host AA/TSR, Lumen, GI and reflection values; the recipe budget is closed. Coverage is median bright-magenta pixels inside the current mask divided by mask area on the reader's grid. speed_ratio is the existing wall-span/game-span reading.

| Retained historic PIE gate | C/P/N | Decidable/floor | Coverage | speed_ratio | Shifted C/P/N, rejected | O/N |
|---|---|---|---|---|---|---|
| Stack native100 (_r9311) | 55/0/0 | 55/1 | 0.681289692 | 1.752679746 | 5/50/0 | 0/0 |
| Stack native50 (_r9312) | 56/0/0 | 56/1 | 0.688305011 | 1.794441265 | 7/49/0 | 0/0 |
| Stack synthetic100 (_r9312) | 60/0/0 | 60/1 | 0.682945833 | 1.826303470 | 6/55/0 | 0/0 |
| Stack synthetic50 (_r9312) | 61/0/0 | 61/1 | 0.689513767 | 1.884795722 | 7/55/0 | 0/0 |
| Lyra native100 (_r9311) | 50/0/0 | 50/1 | 0.687368611 | 1.764856595 | 6/43/0 | 0/0 |
| Lyra native50 (_r9312) | 57/0/0 | 57/1 | 0.692690879 | 1.905461658 | 8/49/0 | 0/0 |
| Lyra synthetic100 (_r9312) | 52/0/0 | 52/1 | 0.686543290 | 1.835424873 | 8/44/0 | 0/0 |
| Lyra synthetic50 (_r9312) | 56/0/0 | 56/1 | 0.692943975 | 1.873751734 | 8/47/0 | 0/0 |

Each historic gate's decidable count is its C value; all shifted copies reject. All eight staged pairing cases are NR, including their C/P/N, coverage, speed, admission and controls.

The preceding controls remain: failed unlit leg **0/0/56**, coverage **0.051074219**, speed **1.360830758**; C1 amended lighting **52/0/0**, coverage **0.684693259**, speed **1.330854456**, shifted **4/47/0 rejected**; C2 exact failed recipe plus **`r.MotionBlurQuality 0`** gives **0/0/60**, coverage **0.051464844**, speed **1.496748418**. The later amended Lyra leg was 49/0/9; the authorized second recipe attempt adopted historic settings. These are retained controls/readings, not reruns or new candidate qualification.

The stopped 093-11 historic Stack native50 remains a reading labelled **“size assertion encoded AA-off”**: C/P/N=56/0/0, shifted=6/52/0 rejected, coverage=0.687995799, speed=1.671741570. Its 82 log lines cover all 80 requests with internal640x496/output1280x992; all 80 pictures/69 masks have consistent dimensions. The four fresh historic 50% gates also cover every request at that internal/output ratio, have uniform picture/mask/label sizes and reject 100%-log/wrong-width and shifted-mask controls.

| Retained fresh 50% leg | Mask-pass lines / requests covered | Pictures / masks | Labelled measured rows | Old PF-2 X; Y |
|---|---|---|---|---|
| Stack native | 80 / 80 | 80 / 68 | 18 / 18 | n0, unavailable; n0, unavailable |
| Stack synthetic | 81 / 80 | 80 / 69 | 18 / 18 | n0, unavailable; n0, unavailable |
| Lyra native | 82 / 80 | 80 / 63 | 18 / 18 | n0, unavailable; n0, unavailable |
| Lyra synthetic | 80 / 80 | 80 / 65 | 18 / 18 | n0, unavailable; n0, unavailable |

AA4 retires that vacuous pairing part 3. Pairing now retains Z2 parts **1, 2 and 4**; dedicated TSR box legs above carry the mandatory per-axis n>=20 proof. No n0 result is represented as box agreement. Z2's prior freeze `cdef048` was committed 2026-10-15 20:47:54 IST before its first leg at 20:57:18.620.

**Projection, ownership and admission as built.** Selection/admission use the resolved owner's engine projection on GT; L3 permits the last completed camera. Serving-view unjittered matrices/rects are frozen with each sample for labels, visibility and masks. Normalizing through the same copied rect maps geometry to actual picture pixels. No FOV/aspect reconstruction remains in targeting/labels. The original inventory remains:

| Use | Disposition |
|---|---|
| Projector/frustum/coverage/bounds/bbox; TestVisibility | Engine geometry, no scalar fallback |
| GetActiveViewInfo; FAnomalyViewInfo | Resolved engine geometry plus metadata |
| ComputeBoundsScreenSizeForActiveView | Engine projection supplied to engine helper |
| Near-clip slab and confirmation rays | Engine inverse projection; scalar reconstruction removed, SAT/trace policy retained |
| CameraClipping cache | FOV only invalidates cache; engine geometry identity also participates |
| LabelWriter, capture/annotation, ControlSnapshot | FOV/aspect metadata only |
| ChangeStage FOV difference | Camera-change diagnostic only |
| CaptureLetterbox | Bench aspect setter/restoration; engine computes rect |
| Scene-view extension | Frozen unjittered serving matrices |
| Written-size/output-height code | Resampling only |
| LOD, stuck-mip, missing/corrupt texture, ChangeCase, TexCorruptTree | Common engine geometry; no per-anomaly reconstruction |

W1 restored `OutView.HorizontalFOVDeg` from `Camera.FOV` to `Player->PlayerController->PlayerCameraManager->GetFOVAngle()` in AnomalyViewport.cpp. The locked-70 test observed GetFOVAngle=70/cache=90; the f9b9328 expression would return that observed 90. Boxes remain on engine projection.

Named-target paths, including BeginFire → TryFireSpecific, selection, console `IAI.Apply`, dashboard and public single-target calls, converge on admission in `ApplyAnomaly` before mutation. Bounds/frustum refusal counts `targeted_refused_off_screen`; unavailable geometry counts `targeted_refused_no_projection`. Existing cadence retries refusals; occlusion remains with the mask veto, and frame-wide anomalies retain their eligibility. Both counters are summary-only. Readme dev notes require Play/PIE or a game build, not Simulate.

Ownership activation controls participation only. GT resolves a unique real display; requests freeze epoch, generation, scene, target and local-player/view, and bind a unique family/view before dequeue and again on RT. Changes retire pending work/baselines; ambiguity, split/stereo and NotGameView sub-reasons are counted. There is no unknown-target first-wins path. The obsolete accepted_ambiguous field is removed.

Actual new active-run summary excerpts, never label fields:

```json
{
  "projection": {
    "version": 1,
    "axis_constraint": 0,
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
  "capture_ownership": {
    "owner_scene_known_start": 1,
    "owner_target_known_start": 1,
    "owner_generation": 3,
    "owner_updates": 2,
    "requested": 990,
    "written": 984,
    "target_fallback_accepts": 0,
    "unpaired": 6,
    "dropped": 6,
    "stale_generation": 17,
    "pending_at_stop": 0,
    "rejections": {
      "scene_mismatch": 3,
      "not_game_view_show_flag": 0,
      "not_game_view_game_thread": 0,
      "not_game_view_controller_id": 0,
      "ambiguous_owner": 0,
      "stale_generation": 17,
      "owner_render_missing": 0
    }
  }
}
```

`rejections.owner_render_missing=0` does not contradict six retirement receipts: that counter counts the narrower request/family-check path, while `unpaired/dropped` and the named receipts account for retirement. G484 documents this distinction and the follow-up; no counter semantics were changed here. `stale_generation=17` counts rejected checks, not accepted stale pictures.

**Office reader.** No `.py` change in 093-13. Minimum Python remains 3.10, with existing NumPy/Pillow and no `hashlib.file_digest` dependency. PASS format is `I<h>.<case> PASS v=… c=… b=… #…`; FAIL includes r,w,v,e,x,m,c,b and I-failure counters. Box mode is `B<h> n=… b=… #…`. Null masks increment m without crashing. PASS requires r=w=360, e=x=m=0, paired masks and |b−1000|<=10; the owner adds `g=0` after checking the 20-frame numbered contact sheet. No window resize during a checkpoint.

Retained self-tests agree on Python 3.10.11 and 3.13.1, exits 0/1/0/1:

```text
I1.11 PASS v=4 c=0 b=1000 #0
I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0
B1 n=96 b=999 #0
B1 n=96 b=967 #4
```

The healthy I line came from the new 093-09 engine session with the original marker ON and also holds on marker_r1, as B19 corrected. The FAIL is the retained null-mask Lyra session; box lines use f608d56 Stack/Lyra sessions. These are home self-tests, not an office checkpoint or a claimed office g reading.

**Earlier closure and date history.** J1 Part 0 remains complete at `59b2fa19aa82f6c0ebbf2db1f223ba849b796c9f`: copied PIE/staged UV events were relabelled from StaticMeshActor_0 onto StaticMeshActor_12's target/mask, with unchanged RGB and zero overlap; both wrong-object controls rejected and originals rehashed unchanged. The blanket can-fail claim was corrected. G479/readme retain the masked-alpha limitation: event-state silhouettes omit pixels cut away by corrupted alpha, though edges stay 0; future work is unioning pre/event silhouettes. Both staged proxy runs produced three events because attempt 0 was refused as `texture_uniform:pending`.

The earlier date recreation preserved trees, messages and author identity; nothing was deleted or pushed. [Mapping receipt](E:/IA_BuildCache/_r939/date-recreation-verification.json):

| Old hash | Correct-date replacement |
|---|---|
| `7f4329c462aa779c1703420876d9492b367aaef8` | `06740850141876fea2b57801a084fd39c85a879b` |
| `41c1a0c9d54c807b62bdf55a905783a7ea9e26d0` | `d37ab4995562ae261591c1fed8b820f670a20425` |
| `1071ca13f4f4e00d744e394203f1063fe772cab8` | `36fec5fef2ab267648544bc5a40d06998d5e76a2` |

Dates were 2026-10-15 00:40:53/53/54 IST. The preserved bundle retains old objects. The preceding shutdown recovery remains documented in [crash-recovery.json](E:/IA_BuildCache/_r939/recovery09310/crash-recovery.json); no new shutdown occurred here.

The prior admission re-freeze requires no application without owner projection, positive no-projection refusals for ambiguous/split cases, and the original >=3-visible-events/zero-edges/measured-masks floors for healthy runs. Render-only negatives retain their specific rejection counts. W2 allows named drops only for resize/camera; other healthy runs retain w=r. PF-1 includes generic application receipts. AA2 changes state reset/allocation; AA4 alone moves reduced-resolution boxes to a proper fixture. No threshold was changed after observing a result.

**New binary hashes and cook status.** All values below are SHA-256 from the new manifest and were checked before each launch. Product source is `9781031`; the closing documentation head has identical Source/Shaders/tools. No cook ran, so no staged-versus-candidate equality or staged gate is claimed. `-nodebuginfo` was prepared but not executed.

| Bank | EXE/DLL | SHA-256 |
|---|---|---|
| stack | StackOBot.exe | `d8d3095167a7addb6aa756674746183e78a8ceb766a4abdff4966c4312806c1b` |
| stack | editor/UnrealEditor-AnomalyBench.dll | `e9d1754f132a1ab9bbecb6583ee87b1e7f62ffa98b0a4a62406d03455cf1daf3` |
| stack | editor/UnrealEditor-AnomalyCapture.dll | `a820c0d60d8df6789729fe40228bc04cc6babc36f03d867765c251b89cf3fa49` |
| stack | editor/UnrealEditor-AnomalyControlServer.dll | `e6dc5176f11cbd620e0d93c53c74a23194a371a248e0a12536393f0064565aee` |
| stack | editor/UnrealEditor-AnomalyInjector.dll | `4cf9648912569625472d23c119be01aeead259d6c074a27a87a1f8042f0e6efc` |
| stack | editor/UnrealEditor-AnomalyShaders.dll | `923eb3104c1f66769893e5bfb0f2498f9febce5da486028a67f2963886e5cecc` |
| stack | harness/UnrealEditor-Review0936.dll | `bd785eb52a986cf832a735692c9653240e4bc02b3e77ccd84af30a57c939b7d0` |
| stack | harness/UnrealEditor-Review0936Runtime.dll | `ed999affe8802ee63b7e8237ab94d5f88a10526a1e7d722f17a631120bb2bb2b` |
| lyra | LyraGame.exe | `dfe6f1643bfb5bf1be41ac3fce565e85d7f49960186ff9032ba11a1f914b229a` |
| lyra | editor/UnrealEditor-AnomalyBench.dll | `c11d1b17b07efc7da43c7006a3b9d1bf9ea828548f315652641f8bfba60ac686` |
| lyra | editor/UnrealEditor-AnomalyCapture.dll | `53e7ec2e7655210af9fc4fa928392b93430ad627e7e73f9402c79d2155ee3871` |
| lyra | editor/UnrealEditor-AnomalyControlServer.dll | `782fbdda64d7160211b0d0a6452fd7f2d615906e2012c35099ac2f0afe13de98` |
| lyra | editor/UnrealEditor-AnomalyInjector.dll | `1c20d2985bf521737c9984f11cf982498e3fc3dd5f9321d02fc39625588199d6` |
| lyra | editor/UnrealEditor-AnomalyShaders.dll | `5ad00b823cbe7b0fd0b7920a32a57ddd0b7114212284faa057d0f5d971c71119` |
| lyra | harness/UnrealEditor-Review0936.dll | `b5a80a89325e91574ac7737230b12e6f5fb797e572aec107085e09642578453b` |
| lyra | harness/UnrealEditor-Review0936Runtime.dll | `6261de224ced26b1e894ce872caba49a09f8e9e0e1b54cc1728fd9a3cd0ee660` |
| stack | UnrealEditor-CaptureBench.dll | `a7443041103affbcd06b724a1ee1cd66f30c636d7ba246d2b34e661b0e89fb51` |
| lyra | UnrealEditor-CaptureBench.dll | `017ca90bdffd2b79b30fa7a1e262c9f747c0070aece0608ba76177e85455edb3` |
| shared engine | UnrealEditor.exe | `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b` |

**Harness repairs and settings.** Each new harness repair used its first attempt, retained the original failure, and changed no gate threshold:

1. The CPU FString shim lacked unary dereference; adding the shim operator fixed its C2100 compile failure.
2. The scratch lever source bundle omitted its `.uplugin`; exact product/helper metadata repaired audit input before binary inspection.
3. The closing source audit compared normalized extraction bytes with CRLF checkout bytes; it now compares the committed Git blob and normalized checkout. The product was unchanged, and CPU results were not rerun or retuned.

The existing CPU bootstrap emitted `vswhere.exe not recognized`; the explicit MSVC compiler resolved and all compilations succeeded. No compiler/security setting was changed. The missing camera donor was **not repaired**, because reader changes beyond AA4 are prohibited. Pairing, marker and packaging budgets were not reopened. Prior harness repairs remain documented in the preserved reports; no failed engine leg was rerun in this job.

The complete process-local bench commands used by the two new legs were:

```text
r.DefaultFeature.AutoExposure 0
r.EyeAdaptationQuality 0
CaptureBench.Marker 1
IAI.Capture.SVE 1
IAI.Capture.Delivery 0
IAI.Capture.Pace 1
IAI.Capture.Fps 30
IAI.Capture.Mask 1
IAI.Capture.TargetMask 1
IAI.Capture.RunLog 1
IAI.Capture.FocusGate 0
r.AntiAliasingMethod 0
r.Lumen.DiffuseIndirect.Allow 0
r.DynamicGlobalIlluminationMethod 0
r.ReflectionMethod 0
IAI.Capture.Config 0 30 180 60 4
IAI.Bench.Review938.Projection 1280 720 -1 StaticMeshActor_0
IAI.Bench.Review938.Switches 60 300 1.5
IAI.Bench.Review936.Observe
IAI.Capture.Start (session path) png 777 1100 uv_corruption StaticMeshActor_0
IAI.Capture.Start (session path) png 777 1100 uv_corruption StaticMeshActor_0 tile
IAI.Bench.TexCorruptIdentity 1
```

`TexCorruptIdentity 1` was reference-only; active used `tile`. Both launches used the retained fixture map, window PIE, `-IAIBench`, `-NoLiveCoding`, unattended/no-splash and per-leg plan/log paths. No screen-percentage, axis, motion-blur, fixture-lighting/content or additional marker change was made beyond those recorded camera specs. Strict-build PCH/unity/IWYU settings were restored by their receipts. Console overrides ended with the owned processes; no host config override persisted.

**Restoration, archive and push.** Owned editor PIDs 39696/29028 were closed only after capture completion and identity verification. No owned engine remains and no foreign process was touched. Stack/Lyra original heads are `23098fa7415f148e935acbd4a97e18da04e6c46c` / `caa68c6799db023f7a24c0d6cfc56124ce27b542`; all 201/183 recorded files and 2748/2879 content files match. Twenty-one new build/crash-config files and five installed roots were removed only by reviewed ownership receipts. All **5,764/5,764** live-stage hashes match. [Restoration](E:/IA_BuildCache/_r9313/receipts/restored.json), [stage rehash](E:/IA_BuildCache/_r9313/receipts/staging-after.json), [final verification](E:/IA_BuildCache/_r9313/final-verification09313.json).

The [UNQUALIFIED archive](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-exposure-09313-9781031-D8D30951-DFE6F164-UNQUALIFIED) contains both candidate banks, helper/fixture DLLs, source and closing-head snapshots, both complete new camera captures, tests, scripts, logs and receipts. The initial 4,011 bank/evidence files total 2,074,844,086 bytes; closing reports/receipts are added to its manifest. Every archived payload is rehashed, and a one-byte DLL mutant rejects with the original unchanged. The archive index is appended; no earlier archive is modified. Baseline identity evidence remains in the separate eff797c UNQUALIFIED archive and `_r939`/`_r9311`/`_r9312`.

Disk floors remained satisfied. Both engine launches passed the idle gate; security settings and foreign processes were untouched. No natural-cause code, delivery merge, tag, force-push or branch deletion occurred. **Heads pushed: none.** Local head is `0bc6e9f46cc2d15ddee44548db62f31757d54783`; remote remains `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`.

**NEEDS-DECISION:** the stale-projection negative needs an authorized drop-aware donor rule, or a specified alternative control under W2. Sample 299 is a counted drop; substituting 296 would change the frozen reader. No such substitution was made. The remaining reach/identity/TSR, cook, staged qualification and push remain unfinished because of this scope stop.

VERDICT: NEEDS-DECISION
