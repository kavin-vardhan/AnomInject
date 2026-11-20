# 093-07: engine projection and J2 requalification

## Design committed before product changes

Binding: 093-06 ruling K3-K7, 093-07 request, 093-06 review GAPs 1/2 and
observations A-D, and 078-16 C.3. Worktree is
`E:/IA_BuildCache/_r933/AnomalyInjector`, branch `fix/followup-capture`, initial
head `bb9e741`, previously frozen product `f608d56`. Evidence for this continuation
is `E:/IA_BuildCache/_r937`; `_r936` is preserved evidence. No natural code,
engine edits, new module dependencies or additional engine-private headers.

### Route, ownership and timing

Use `ULocalPlayer::GetProjectionData` for the uniquely resolved capture-world
local player's actual viewport. Store the returned projection matrix,
`ComputeViewProjectionMatrix()`, view origin, constrained and unconstrained
rects as values in `FAnomalyViewInfo`. These are unjittered engine matrices;
`GetProjectionData` includes the axis constraint, projection mode, constrained
rect, off-centre projection and `SetupViewProjectionMatrix` hooks. No product
projection is reconstructed from camera FOV/aspect scalars.

All UObject access is on the game thread. Resolve the player from the world's
game instance, require uniqueness, and use its viewport client. Capture's frozen
owner is checked against that player/viewport; never substitute a global editor
viewport or a first controller from a different world. A missing/ambiguous view
is invalid geometry, not a guessed camera. Existing F1 owner/family refusal
and lifetime rules remain authoritative.

UE 5.1 `LevelTick.cpp` calls tickable subsystems before `UpdateCameraManager`,
then broadcasts `OnWorldTickEnd`. Capture's early arm is not its geometry
sample. Refresh the pending request's view at world-tick end, after the camera
update, before freezing target bounds and evaluating per-frame on-screen state.
This update is also required in synthetic order. Keep request identity, frame,
owner generation, matrices and target bounds together through publication.
No readback-time UObject lookup may determine a saved frame's geometry.

Target eligibility uses the engine projection of the currently resolved owner
when the selector asks; before a camera update that is the last completed
camera state. It is a selection query, not the authority for a later frame's
label. The latter always uses its post-camera frozen record. Do not advance the
host camera from the plugin or change anomaly duration/tick scheduling.

The serving colour view also supplies its unjittered matrix and actual rects
through the existing view extension/readback value record. Publication checks
the already-bound owner/family and maps the frozen world bounds using that
same serving geometry. This closes the gap between a GT query and a host's
later view setup, without touching render state from the writer thread.

### Rect mapping

For each world-bound corner, divide engine clip X/Y by W and map NDC to
normalised coordinates of the engine's constrained view rect. Capture copies
`SceneColor.ViewRect` into an origin-zero colour texture; it does not copy the
whole window or its letterbox bars. The constrained view maps to that copied
rect: `p_source = copied_min + uv * copied_size`, then
`p_written = (p_source - copied_min) / copied_size * written_size`.
Thus `bbox_norm` is that same `uv` and `bbox_px` uses actual written dimensions.
Internal screen percentage, a nonzero copied origin, temporal upsampling and
OutputHeight resampling cannot introduce an extra aspect/offset scale.
Missing or inconsistent geometry is refused/counted; it is never filled with
a scalar-derived matrix. Orthographic engine matrices are valid for boxes;
camera-clipping's existing non-perspective refusal remains explicit.

### Complete camera FOV/aspect use audit

Search baseline: case-insensitive FOV/aspect/projection uses throughout Source,
plus all `GetActiveViewInfo` callers. Positions below refer to `bb9e741`.

| Product use | Disposition |
|---|---|
| `AnomalyViewport.cpp:196-218`, BuildProjectionMatrix/BuildViewProjectionMatrix; all BuildFrustum, coverage, bounds and bbox callers | Replace reconstruction with the saved engine matrices. |
| `AnomalyViewport.cpp:683-731`, GetActiveViewInfo | Resolve the local player/viewport and obtain engine projection; configured FOV and viewport aspect remain metadata. |
| `AnomalyViewport.cpp:983-996`, ComputeBoundsScreenSizeForActiveView | Feed the engine projection matrix to the engine screen-size helper. |
| `AnomalyViewport.cpp:1034-1113,1313`, EvaluateNearClipSlab and confirmation-ray geometry | Replace ComputeTanExtents(FOV,aspect,axis) with engine inverse-projection geometry; preserve near-distance/confirmation policies. Include off-centre ray geometry, never re-derive a FOV. |
| `AnomalyNearClipSlab.h:33-73` scalar reconstruction | Remove from the product path; adapt its geometry input and meaningful CPU tests. Pure SAT/trace policies remain unchanged. |
| `Anomalies/Anomaly_CameraClipping.cpp:146-180`, camera cache key | FOV is only a cache invalidator, not projection. Include engine geometry identity so same-frame constraint/rect changes cannot reuse a stale eligibility result. |
| `AnomalyInjectorSubsystem.cpp:589-598`, TestVisibility | This direct projector caller must consume resolved engine geometry. Supplied synthetic camera scalars cannot create a production projection fallback. Retain a diagnostic explanation for obsolete scalar arguments. |
| `AnomalyViewport.h:17,19`, FAnomalyViewInfo | Retain configured-FOV and viewport-aspect metadata; add engine geometry as values, no new module dependency. |
| `AnomalyLabelWriter.cpp:374-375`, view.fovDeg/aspect | Metadata only. `fovDeg` stays configured camera FOV; aspect keeps viewport-aspect meaning. |
| `AnomalyCaptureSubsystem.cpp:10230-10231,10466-10467` and LabelWriter annotation camera fields | Event camera metadata only; no projection reconstructed from these fields. |
| `AnomalyChangeStage.cpp:564-573`, camera FOV difference | Diagnostic camera-change reading only, not a projector. |
| `AnomalyControlServer/ControlSnapshot.cpp:92-93` | Existing camera metadata only; no projection calculation. |
| `AnomalyCaptureLetterbox.cpp` | Bench-only camera-aspect setter/restoration and predicted-rect log; the engine computes the rect. Keep behind IAIBench/Shipping guards. |
| `AnomalySceneViewExtension.cpp:131` | Existing copied engine projection for a view; preserve engine provenance, take the unjittered serving geometry for saved labels. |
| Capture output-height/LabelWriter size code | Derives written width from image dimensions only; resampling, not camera projection. |
| LOD, stuck-mip, missing/corrupt texture, ChangeCase and TexCorruptTree callers of GetActiveViewInfo | Receive the common engine record; do not add anomaly-specific camera math. |

Planned product files: the public/private AnomalyViewport projector; its
near-clip pure geometry adapter and camera-clipping cache caller; the
TestVisibility direct caller; capture snapshot/frame/SVE/readback plumbing and
LabelWriter summary plumbing. K5 additionally removes accepted_ambiguous from
ownership diagnostics and fixes `tools/office_checkpoint_i.py` and its doc.
Any need for a broader mechanism change is a scope stop.

### Summary diagnostics and office reader

Add `projection` to run_summary only: version, effective axis constraint
(0 X, 1 Y, 2 major), camera aspect, viewport size, constrained/unconstrained
view rect, projection mode, source and invalid/refused geometry counts.
Keep label keys/meanings unchanged. Remove the unreachable accepted_ambiguous
summary field and reader a=. Ambiguity refusals/first-wins can-fail remain.

The reader has no inspection input. It writes a local 20-frame contact sheet
with frame numbers. Its PASS is the machine result; the complete office
reading is accepted only when the owner appends `g=0` after viewing the sheet.
`x` counts machine-detectable wrong/invalid-view evidence, not a claim that
counter zeros identify a game picture; `g` is the independent visual check.
Missing masks count as m, never interrupt other measurements. The exact scale
estimator and check digit are frozen in the predictions. Use Python 3.10+
with only numpy, Pillow and existing verify_capture; test with a real 3.10
interpreter, not only syntax parsing.

### Initial evidence limitation, before predictions/legs

The retained Lyra 942x869 stop has zero measured target masks and null drawn
boxes on all 96 positive rows. A mask-centre estimator cannot return 0.813
from it. Keep this input as PF-1's negative and the required null-mask FAIL;
report scale unavailable rather than invent pixels. Its independently saved
engine-projection metadata shows the approximately 0.813 mismatch. Supplement
the existing 1123x870 mask-based 0.967 can-fail with an archived-f608d56 binary
replay at 942x869 and a genuinely on-screen target, so the same frozen PF-2
mask estimator can measure the approximately 0.813 error. No gate threshold
or original failed evidence is changed by that additional control.

## Execution journal

Initial source/refs/remote and disk readings are in `_r937/start.json`.
The registered Python 3.10 launcher points at a missing executable; use an
isolated runtime under job scratch for the minimum-version proof, without
changing user configuration. No host mutation or engine leg has occurred.

Implementation checkpoint: product/reader `5af1899` follows the design commit.
The serving geometry is installed before ComputeRenderMembership; world-tick-end
refresh precedes frozen annotation inputs. Both named hosts were backed up by hash
receipt and installed detached at the implementation candidate. No engine leg has
run yet. Initial compile exposed protected `ULocalPlayer::GetViewPoint` on UE 5.1;
use the public camera cache for configured metadata, keeping projection itself on
the public GetProjectionData API. No engine/private-header/dependency change.

All existing CPU suites pass, including 154 slab checks, 47 ownership checks,
20 family predicates and their mutants, 51 Python tests and the 53-lever audit.
Four new reader tests pass under Python 3.10.11 and 3.13. The initial standalone
unit invocation used Python's default temporary directory; subsequent job runners
set TEMP/TMP to `_r937/temp`. No retained capture is changed. The first stripper
invocation used a nonexistent worktree-local path; rerunning the canonical script
at `D:/IntrusiveAnomalies/_strip_comments.py` changed no source comments.

Retained reader tests, both interpreters, original session hashes unchanged:

```
I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0
B1 n=96 b=999 #0
B1 n=96 b=967 #4
```

The new healthy PASS remains pending its engine session. The generated 20-frame
null-mask contact sheet was visually checked: all frame numbers and the local-only
instruction are legible. PF-1's unchanged checker rejects all 96 false rows and
four applied fires in the retained session, with engine X [-79.365,-21.432].

The read-only UE 5.7 headers also show an optional camera-level axis override.
The projection API already applies it; summary diagnostics now select that
override when the member exists, otherwise the local-player setting (UE 5.1).
This uses overload detection in the projector, with no version guess or new
header/dependency. The camera-switch schedule and matched identity-copy RGB
control are frozen in the predictions before any engine leg.

## Final candidate and D2 stop

Product frozen at `f9b93280a3c692cecae377c3ee72c186bdb34852`, Source tree
`2e91fe78728265e381a5cfb7b359ac4a66006a9e`, Shaders tree
`64e18bf3d238c0c10817c34ed03fcb24f5903f06`. Normal and strict Editor/Game
builds pass on both hosts with zero errors/warnings and 71 standalone headers
per strict target. Source/module-scope audit confirms public added headers,
no Build.cs or shader change. All 69 CPU runner steps meet their expected exit,
including negative controls; ownership/family mutants and both Python reader
interpreters pass. The two literal question marks from observation C are replaced.

The first engine leg passed the idle gate after 69 seconds (three samples,
GPU 0/1/0 percent, about 11.5 GiB available physical memory). It ran only
Lyra selected-viewport PF-1, exact 942x869 MaintainYFOV original framing.
Engine-projected target X is [-79.365341,-21.432146], wholly off the left edge.

| PF-1 measurement | Retained f608d56 can-fail | New f9b9328 |
|---|---:|---:|
| requested/written | 360/360 | 360/360 |
| falsely labelled / visible-positive rows | 96 / 96 | 0 / 0 |
| applied fires | 4 | 4 |
| zero-match bursts | 0 | 0 |
| checker | rejects | rejects: off-screen target fired |

The new labels satisfy PF-1's visibility part. The no-fire/refusal part does
not: four applies occur, and four later MEASURED_ZERO vetoes remove their
annotations. Read-only source inspection locates the missing admission query:
`AnomalyCaptureSubsystem::BeginFire` calls `TryFireSpecific`; that method
finds the actor, checks existing live/partner/Nanite rules and then calls
`ApplyAnomaly`, without consulting on-screen geometry. This path was not changed
by the projection patch. No product fix, framing change or gate relaxation was
made after the failed prediction. D2 stops the qualification sequence here.

PF-2 (12 PIE/4 staged), PF-3, PF-4, all 84 J2 cases, camera switches and the cook
are NOT RUN. No camera-switch drop/continuation reading exists. The three retained
reader lines above are actual outputs under Python 3.10.11 and 3.13.1; the fourth,
new healthy I PASS, remains unrun. The retained 942x869 input has no measured mask,
so its b remains unavailable; the planned archived-binary .813 replay is unrun.

Product lever scans pass on both hosts/configurations. StackOBot's 1,788 new
string readings versus J1 and Lyra's 1,630 versus its f608d56 candidate are fully
attributed; unknown ASCII/wide sentinels are rejected. Lyra has an executable
`.uedbg` section: its one leftover scanner fragment is instruction bytes, verified
by the PE executable/code flags and surrounding instructions, not a product literal.

The additional helper audit found Lyra's frozen Editor helper missing
`IAI.Bench.Review937.Switches` (13/14 commands); its Game helper has all nine runtime
commands, and StackOBot passes both configurations. The final source includes the
command. An interrupted preliminary compilation overlapping the source edit is a
possible cause of the stale object, not an established mechanism. PF-1 did not use
Switches. The product stop occurred before a repair/rebuild attempt; retain these
bytes as unqualified, and rebuild the helper before a continued camera proof.
The prepared, unrun camera reader's next-46-samples drop reading also needs to
include pending pre-switch samples when attributing drops; no reading is claimed.

Other harness/compile corrections (one corrective attempt each): public camera
cache for protected GetViewPoint; restored five Build.cs backups and removed 71
receipt-listed strict TUs after interrupted workers; normalized git-blob verification
instead of raw CRLF backup comparison; explicit job TEMP/TMP after one auto-cleaned
default-TEMP unit invocation; canonical stripper path; UTF-16 preflight JSON read.
The string scanner attribution was extended for verified executable sections.
No gate or threshold was changed. Receipts and failed attempts remain in `_r937`.

Hosts restored by receipt: StackOBot detached `23098fa`, 201 original files;
Lyra detached `caa68c6`, 183 original files and 20 job-created build/config files
removed. Original host content matches. Only receipt-owned helper/content roots
were removed. All 5,764 live staging files rehash unchanged; no cook or staging
write occurred. The launched editor exited normally and no owned process remains.
Process-local projection/CVar/PIE changes ended with that editor; original config
bytes are restored. No owner security setting or foreign process was touched.

Unqualified archive:
`D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-projection-09307-f9b9328-7A69B4EE-7A499731-UNQUALIFIED`.
The archive records both candidate manifests, frozen source, scripts, receipts and
failure evidence; raw PNGs remain under `_r937/legs/lyra_P_pf1`. Archive rehash and
byte-mutant results are in `_r937/archive-receipt.json`. The report is in the shared
093-06/093-07 run folder. Nothing was pushed; remote remains
`1bf7054f5b8664f057282ed863a62ab5dbbd60e3`. No merge, tag or natural-cause code.

NEEDS-DECISION: authorize a continuation to add a counted engine-projection
off-screen refusal to the targeted path and then restart the prescribed gate order.
The post-camera timing limitation of early selector queries described in the design
also remains explicit; serving-frame label geometry is authoritative. J2 is unqualified.
