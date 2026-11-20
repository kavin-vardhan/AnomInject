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

@@MATRIX@@

Second-view wrong-view counts are zero; Stack/Lyra non-owner rejection readings are **2479/2504**. Each fixture counts ambiguity730, split366 and stereo728. NotGameView sub-counts are Stack controller/game-thread/show-flag **1067/1067/1067**, Lyra **1067/1067/1068**. Exact J1 controls write0/360; pre-F1 controls show the wrong view on360/360. Their archived-DLL receipts and inputs were rehashed, not rerun.

Both fresh resize legs reconcile **360=358+2 owner_render_missing**, samples98-99, with zero stale/mismatched acceptance, overlap or unexplained loss. After sample100, pictures are960x720, with no old-size result accepted. Four visible events remain zero frames off; PF-2 has n96 per axis. Stack scales are **.999999837/.999999867**, Lyra **.999997529/.999997529**. Both DRAWN SUMMARY logs read **exposure exclusion2304px**; this is a log reading, not a summary field. Missing-receipt/reason and overlap mutants reject. Every other healthy matrix leg keeps w=r. Both supplemental PIE admission probes pass360/360,v4/e0,O/N0/1, with zero-counter/application controls rejected; both staged probes are NR.

**Pairing and Z2.** Reader SHA-256 remains `54928b2ecce48d97a1af4bea16b00184ca091e8b22670fe68e8e3165671f3525`, tolerance40px, decidable floor1, PREVIOUS=NEITHER=0. Historic host AA/TSR, Lumen/GI/reflection settings remain in use. Coverage is median bright-magenta pixels inside the current mask divided by mask area on the reader grid; speed_ratio is the existing wall-span/game-span reading.

@@PAIRINGS@@

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
@@DIAGNOSTICS@@
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

@@HASHES@@

The [UNQUALIFIED archive](D:/IA_BankOverflow/followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-OVERFLOW-UNQUALIFIED) retains candidate/fixture banks, source and closing-head snapshots, captures, scripts, controls, build/audit evidence and restoration receipts. It is stored on physical D: and linked under the archive index using the same name. Initial payload: **@@ARCHIVE_FILES@@ files, @@ARCHIVE_BYTES@@ logical bytes, @@ARCHIVE_UNIQUE@@ unique payload bytes**. Equal payloads share hardlinks only inside this new archive, never to source evidence. Every payload rehashes; a one-byte DLL mutant rejects with the original unchanged. Closing report/verification receipts are added and rehashed in the final manifest. The archive index is append-only; prior archives remain unchanged. [Final archive rehash](E:/IA_BuildCache/_r9314/archive-final-rehash.json).

**Harness repairs, settings and restoration.** Donor repair attempt1 succeeds; the original failure is retained. The audit wrapper initially selected J1 rather than the exact eff797c baseline frozen in093-13; one input repair restored the frozen baseline, with scanner/classifier/unknown-string controls unchanged. The stop-reading writer's Windows default decoding failed on UTF-8 source; one explicit UTF-8 read repaired this reporting script. Neither repair changed a gate or triggered an engine retry. The new TSR matcher problem was diagnosed but not repaired because it exceeds AB1 scope. Pairing, marker and packaging budgets were not reopened.

The first archive attempt checked logical D: capacity but the archive root is a junction onto E:. Only the verified archiver PID42388 was stopped before the floor; its26,576 partial files were inventoried and retained as explicitly incomplete. Routing repair attempt2 resolves the destination volume, reserves2GiB metadata space and writes the separate physical-D archive above. The original script, partial receipt and inventory remain available. This storage repair caused no engine retry or evidence change.

Process-local settings and bench levers used/restored are listed here; exact per-leg commands remain in every launch receipt:

@@SETTINGS@@

All overrides ended with owned processes. No host config override, fixture-content change, security change or foreign-process intervention occurred. Prepared capture/evidence relocation and packaging-routing scripts were never executed; only the closing archive uses overflow storage. No cook or staging metadata repair occurred. Normal/strict build settings remain restored by their original receipts.

Restoration verified Stack head `23098fa7415f148e935acbd4a97e18da04e6c46c`,201 files/2748 content files; Lyra head `caa68c6799db023f7a24c0d6cfc56124ce27b542`,183 files/2879 content files. Only33/34 reviewed job-created files and five installed roots were removed, by receipts. All **5,764 live-stage hashes match**. No owned engine remains; no foreign process was stopped. Disk floors remained satisfied. [Restoration](E:/IA_BuildCache/_r9314/receipts/restored.json), [final verification](E:/IA_BuildCache/_r9314/final-verification09314.json).

No natural-cause code, release/delivery merge, tag, force-push or branch deletion occurred. Local head is `0e50b56df4baf516ec815f433a93084388e2cf05`; remote is unchanged at `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`; **no commits were pushed**. The remaining work is Lyra dedicated TSR, the cook, every staged gate and the final feature push, after a ruling on the dedicated TSR receipt reader.

Automatic approval review rejected removal of this job's [partial archive](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-donor-09314-9781031-D8D30951-DFE6F164-UNQUALIFIED) as "blocked by policy." No deletion was retried. The partial attempt remains on E:; the completed archive is on physical D:.

VERDICT: NEEDS-DECISION
