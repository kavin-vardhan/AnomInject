Targeted object admission is implemented at `41c1a0c`; it remains unqualified in engine.  
A confirmed inherited locked-FOV metadata regression requires a projector change outside 093-08's scope; no new engine leg ran.  
Normal/strict builds and hygiene passed; hosts are restored, staging hashes are unchanged, and nothing was pushed.

1. Work and scope stop

Worktree: `E:/IA_BuildCache/_r933/AnomalyInjector`, branch `fix/followup-capture`.
Initial local head `9514242790f71b480eb947b2ec98b8d0e14903e9`; evidence `E:/IA_BuildCache/_r938`.
Design and predictions were committed first at `7f4329c` before product edits.
No natural-cause code, engine edit, new module dependency, additional private header, merge or tag.

Code's parallel review R8 identifies an inherited 093-07 change: `fovDeg` now receives
`Camera.FOV`. The previous path called `PlayerCameraManager::GetFOVAngle()`, which
returns a positive `LockedFOV` override when present. UE 5.1's own GetViewPoint also
uses GetFOVAngle. I independently checked all three source paths. For unequal
locked/cache values, exported metadata changes; engine-derived boxes do not.
The projector file is byte-identical between f9b9328 and this admission candidate.

The still-binding 093-07 Part A.3 prohibits changing a label key's meaning.
Restoring that behavior requires AnomalyViewport.cpp, outside 093-08 Part A.3's
admission/two-counter scope. I recorded the scope stop at 16:47 UTC before the
first engine leg. **Code calls R8 minor and says it does not invalidate these
legs; Code did not order this stop.** This is my application of the scope rule,
not a failed engine prediction or an automatic approval rejection.
Evidence: `_r938/scope-stop.json`, `fov-metadata-source-evidence.json`, and the
read-only review copy. No repair was made outside scope.

2. Admission and projection as built

The check runs in `UAnomalyInjectorSubsystem::ApplyAnomaly` before the registered
anomaly's Apply can mutate the world. It resolves the unique capture-world local
player/real viewport through the existing engine projector, extracts its frustum,
and intersects each matched actor's renderable bounds. Any off-screen member
refuses the whole query. There is no occlusion/coverage/radius gate or scalar
FOV reconstruction. A transient reason and one counter increment accompany each
refused call; retry cadence and active-effect records are preserved.

| Named-target path found | Common check |
|---|---|
| BeginFire -> TryFireSpecific -> ApplyAnomaly | Before target mutation |
| TryFireOnce -> ApplyAnomaly | Re-check after visible/census selection |
| Selector UI/key/IAI.SelectorInject -> InjectSelected | Re-check cached selection |
| Console IAI.Apply | Before dispatch |
| Dashboard control-server inject | Before dispatch |
| Public C++ ApplyAnomaly | Same guard |

Object-scope anomalies, including bench null_effect/solid_swap, share the guard.
Frame-wide time_dilation/camera_clipping and existing component lighting eligibility
remain unchanged. Internal proxy/material Apply calls implement an already-admitted
parent. Empty/no-match queries keep their existing errors.

Both world-local counters reset at capture start and appear only at the root of
run_summary.json: `targeted_refused_off_screen`, `targeted_refused_no_projection`.
They count calls, not actors/samples. No label or annotation key was added.

New product files:

- `Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp`: admission and transient reasons.
- `Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h`: counter state/reset/accessors.
- `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`: reset and summary export.

Only public `ConvexVolume.h` was newly included. The source tree is
`5363827c7ac678d185a4ceecc9cd071870ed9c01`; shaders remain `64e18bf3d238c0c10817c34ed03fcb24f5903f06`.
Docs: 093-08 design/journal and predictions, architecture, status block, G482.
G482 was checked across 71 refs and rechecked at closure. The earlier two literal
question-mark documentation defects were already repaired in 093-07; no new
replacement was needed.

The inherited projection route is ULocalPlayer::GetProjectionData on GT, refreshed
after the camera update, then the serving view's unjittered matrices/rects carried
through readback. L3 permits last-completed-camera selection/admission. Labels,
masks and visibility use the frozen serving record. Normalized coordinates use
the same copied view rect, then actual written dimensions, avoiding extra scale
from screen percentage or resampling.

The design's complete FOV/aspect inventory remains:

| Use | Disposition |
|---|---|
| Viewport projection/frustum/coverage/bbox and screen-size helpers | Engine matrices |
| GetActiveViewInfo | Engine projection; metadata has the R8 locked-FOV defect |
| Near-clip slab and confirmation rays | Engine coefficients, including off-centre terms |
| Camera-clipping cache | Geometry identity; FOV only invalidates cache |
| TestVisibility and anomaly/selector targeting callers | Shared engine record |
| FAnomalyViewInfo scalars | Metadata only |
| LabelWriter fovDeg/aspect, annotation/capture camera fields | Metadata; R8 remains unresolved |
| ChangeStage FOV difference and ControlSnapshot | Diagnostic metadata |
| Bench letterbox setter | Engine computes projection; IAIBench/Shipping guards retained |
| Scene-view extension | Frozen unjittered serving geometry |
| Output-height/label size | Image resampling, not camera projection |

3. Re-frozen predictions and office reader

| Prediction amendment | Reason |
|---|---|
| PF-1: 0 visible/labelled rows, 0 applied fires, off-screen refusals >=1, no-projection=0 | Admission now precedes Apply |
| Missing/ambiguous projection attempts refuse and retry | No guessed owner or early off-view mutation |
| Ambiguous/split negative captures: 0 fires and positive no-projection count | GT owner is non-unique; deliberate 360/0 refusal cases |
| Render-only NotGameView/stereo negatives need their capture rejection, not a fire count | Their GT projection may still resolve |
| Delayed/other healthy cases retain >=3 visible events, zero edges and measured masks | An early unavailable-owner attempt may now refuse; it cannot excuse a failed floor |
| PF-3 retains identical events/targets/edges/visible_positive and <=0.5px boxes | Stable X-constrained framing should be unaffected |
| All healthy positives require w=r, including resize/camera | L5; copied older completeness exception removed before any leg, making acceptance stricter |

PF-2's estimator and thresholds are unchanged: per-axis median of projected/mask
box-centre displacement about view centre; declared interior-mask inclusion rules;
>=20 distinct contributing rows per axis; both raw scales 0.990..1.010; `b` is
the worst-axis scale x1000, rounded half up. No outlier trimming or retuning.
Camera timing remains cut60, blend300/1.5s, Config 0 30 180 60 4, expected990;
pending pre-switch drops are readings and are never counted as written.

Reader code/doc are inherited unchanged from 093-07. Python minimum 3.10;
only existing numpy/Pillow/verify_capture, no hashlib.file_digest. Formats:

```text
I<h>.<case> PASS v=<v> c=<0 X,1 Y,2 major> b=<scale x1000> #<check>
I<h>.<case> FAIL r=<r> w=<w> v=<v> e=<e> x=<x> m=<m> c=<c> b=<b> #<check> I-fail=<counters>
B<h> n=<rows> b=<scale x1000> #<check>
```

PASS requires r=w=360, v>=3, e=x=m=0, paired measured masks and PF-2 bounds.
The reader writes 20 numbered distributed contact frames locally. The owner
appends `g=0` for complete office acceptance. No manual input file/edge pairs;
no `a=` or accepted_ambiguous field. Host1/2 mean the first/second office host.

Four requested self-test readings:

```text
Healthy I PASS: NOT RUN - scope stop before a new engine session.
I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0
B1 n=96 b=999 #0
B1 n=96 b=967 #4
```

The three actual lines match on Python 3.10.11 and 3.13.1. Retained originals
rehash unchanged. Four reader unit tests also pass on actual Python 3.10;
the full home suite has 55 passing Python tests. No healthy line/check digit
is fabricated. Machine B success/failure is conveyed by exit0/exit1.

4. Gate evidence and counters

**Zero new engine legs; 0/84 final-candidate J2 cases qualified.** K7 was stopped
before its first PIE positive. No cook/staged or camera leg ran. `NR` below means
not run because of that scope stop, never PASS. New admission counter readings
are unavailable for every NR leg, not zero.

PF-1 retained direct can-fail: `093-07 REJECT rows=360 visible=0 fires=4`;
both admission counters are absent in that older build. The 093-06 control
also rejects: rows360, false-labelled96, fires4. Originals rehash unchanged.
The candidate PF-1 rerun line is **NOT RUN**, so no off-screen-counter positive
or zero-fire claim is made for the admission build.

| Gate | StackOBot PIE | Lyra PIE | StackOBot staged | Lyra staged | Can-fail evidence |
|---|---|---|---|---|---|
| PF-1 exact selected942x869 Y | N/A | NR | N/A | N/A | Both retained failures rejected |
| PF-2 X/Y x 942x869,1123x870,1280x720 | 6 NR | 6 NR | 2 NR at1280x720 | 2 NR at1280x720 | Retained Stack999 PASS/Lyra967 FAIL; .813 replay and per-axis mutants NR |
| PF-3 native X UV/proxy comparison | 2 NR | N/A | 2 NR | N/A | Shifted edge/1px-box copies NR |
| PF-4 constrained-aspect2.39 | NR | N/A | N/A | N/A | Full-window/cropped-rect mutant NR |
| Camera cut and blend | NR | NR | NR | NR | Identity RGB/stale geometry/mask-shift controls NR |
| Missing-owner admission probe | NR | NR | NR | NR | Zero-counter/applied-receipt copies prepared, NR |

Camera readings for each of the four legs: pending/pre-switch drops, subsequent
drops, active-event continuation/end and recorded reasons are **unavailable**.
Only the reader's chronological synthetic test ran: cut60 includes retired si59
and si60; blend300 includes si299; the old window-only reader misses si59.
This is a harness test, not camera product qualification.

Full J2 matrix retained for continuation (both fixtures, applicable modes):

| Cases | PIE | Staged | Planned can-fail |
|---|---|---|---|
| Delayed viewport / healthy | Both NR | Both NR | J1 E41ABF93 delayed all-drop control |
| New window plus second realtime editor view | Both NR | Standalone wrong-target counterpart NR | Release218BA4C7 wrong-view control |
| Selected viewport, clients0/1 | Both NR | N/A | View/world/frame-marker corruption |
| Resize/reopen | Both NR | Both NR | Stale generation/picture-size evidence |
| Same-world ambiguity / split / stereo | Both NR | Both NR | Forced view arrangements and first-wins mutant |
| NotGameView controller / GT-view / Game flag | Both NR | Both NR | Forced negative sub-reasons versus healthy view |
| UV / proxy regressions | Both NR | Both NR | Wrong mask/count, invisible pixels, shifted edges |
| UV / proxy alignment | Both NR | Both NR | Shifted label/barcode membership |
| Native/synthetic mask pairing,100%/50% | Both NR | Both NR | Previous-frame mask and wrong sample |
| J1/release binary controls | Both NR | Both NR | Archived editor binaries; separately hashed staged links |

All original zero-edge/measured-mask/visible-event floors remain. No accepted
ambiguous result or wrong-view count is claimed from unrun cases. Admission
counter values per candidate leg are NR throughout; retained controls predate
those counters. The .813 replay and healthy I PASS remain explicitly unfinished.

5. Projection example

This is the **retained f9b9328 PF-1 summary**, not a new candidate reading.
It illustrates the existing summary-only schema; no projection block is added
to labels by this job.

```json
{
  "version": 1,
  "axis_constraint": 1,
  "camera_aspect": 1.3333300352096558,
  "viewport_size": [
    942,
    869
  ],
  "view_rect": [
    0,
    0,
    942,
    869
  ],
  "unconstrained_view_rect": [
    0,
    0,
    942,
    869
  ],
  "copied_view_rect": [
    0,
    0,
    942,
    869
  ],
  "mode": "perspective",
  "source": "owner_view_unjittered",
  "served_frames": 360,
  "invalid_refused": 0
}
```

6. Builds, heads, hashes and archive

Both hosts passed normal and strict Editor/Game builds, zero errors/warnings,
71 standalone header translation units per target. All 69 CPU runner steps
passed, including 154 slab checks/20,000 cross-check boxes and 47 ownership
checks. Separately, 20 family predicates, eight family mutants and five
ownership mutants pass.
Source/binary lever audits and all helper command-name audits pass; the rebuilt
Lyra Editor helper contains Switches and the 09308 version stamp. New probe
code stays helper-only, IAIBench-gated and Shipping-denied.
String additions versus the archived f9b9328 candidate are fully attributed:
stack: 1509 additions, 0 unresolved; lyra: 1507 additions, 0 unresolved. Unknown-string controls reject. Comment strip and diff
checks pass. No product lever/default was changed.

**Pushed commits: none.** Remote remains
`1bf7054f5b8664f057282ed863a62ab5dbbd60e3`. Local commits created in this job:

```text
7f4329c462aa779c1703420876d9492b367aaef8 docs(capture): freeze targeted admission design and predictions
41c1a0c9d54c807b62bdf55a905783a7ea9e26d0 fix(injector): refuse targeted fires outside the owner frustum
1071ca13f4f4e00d744e394203f1063fe772cab8 docs(capture): record admission scope stop and build receipts
```

Final local head `1071ca13f4f4e00d744e394203f1063fe772cab8` preserves compiled product `41c1a0c9d54c807b62bdf55a905783a7ea9e26d0`.
The executable/DLL SHA-256 values below are the archived candidate bytes.

| Binary | SHA-256 |
|---|---|
| stack / StackOBot.exe | `5f75eca808a001430ba643c12f7520807c3182d6900e014773d8f93fdce38ccb` |
| stack / editor/UnrealEditor-AnomalyBench.dll | `777c230669425608fc0af68c96c0963cda4ad4bc3f09a9dbe69fd2d15aaeb053` |
| stack / editor/UnrealEditor-AnomalyCapture.dll | `298fb4eb16e6b635d2184a790c5634bb646dc23bd901da5a3eb7deb703522ebb` |
| stack / editor/UnrealEditor-AnomalyControlServer.dll | `e8e966a07d4680b1ff0f7b068a61c27ccd584c941a07626c3e39a55b824fc2aa` |
| stack / editor/UnrealEditor-AnomalyInjector.dll | `16fbd166dde45d3938902b1cb1bd1d4ca7183443570e27ea3371dc7ae69f81ac` |
| stack / editor/UnrealEditor-AnomalyShaders.dll | `47088a38eee22981626ea24e59ac77362f004bda8bef815d2762d64c7d6f7558` |
| stack / harness/UnrealEditor-Review0936.dll | `4f9058d6951a68a75cb0a026ad84074a904e4b0ff96654bc8b317a27df0961a1` |
| stack / harness/UnrealEditor-Review0936Runtime.dll | `d32e365ed750270a43bc63c6608ce903efcf54e40dc673ac5eee76d389754b23` |
| lyra / LyraGame.exe | `ad40da9909a115e7a6c99b3d253632c6d860910e2bc7df2e7625536468b29779` |
| lyra / editor/UnrealEditor-AnomalyBench.dll | `8b6b0ff573308fb781ba7ef09cce635357119d90b06028a1f0bbc6a7978171cf` |
| lyra / editor/UnrealEditor-AnomalyCapture.dll | `992ba6d247132565982b17d4c2dde95e76ca5de0648e6396d29e814b124767c8` |
| lyra / editor/UnrealEditor-AnomalyControlServer.dll | `d7ea4a4be94aab0f291cb819b2e961c068860b4fad464c3b40583b6eec5fbb72` |
| lyra / editor/UnrealEditor-AnomalyInjector.dll | `b9832b92c6e87ea9caf9fda22491b83a6f85d632bd2846ef0e9a1ed5b580b23f` |
| lyra / editor/UnrealEditor-AnomalyShaders.dll | `738516ca3eb381e0d102e539b0665fb3c6fb18c2e5a46b71429c54d7e6ebab2c` |
| lyra / harness/UnrealEditor-Review0936.dll | `f32598b49ce7853b69b297a5ce4a249ca40151c7d9b6b23e6255d280c887f289` |
| lyra / harness/UnrealEditor-Review0936Runtime.dll | `2296c52da1aa9286c59899c1cf11050d4754f6f30743df16a1504ee3f34939c2` |

The engine Editor executable used as the build-host reference (not launched)
hashes `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b`.

Archive: `D:\IntrusiveAnomalies\_binary_baselines\followup-capture-J2-admission-09308-41c1a0c-5F75ECA8-AD40DA99-UNQUALIFIED`.
Status **UNQUALIFIED**; 436 payload files / 535545573
bytes rehashed, archive-byte mutant rejected, original bytes unchanged.
Raw evidence stays at `_r938`; the archive includes source, binaries, harness,
checks and receipts. The final report/docs are added under its closure folder
with a separate hash manifest. The older unqualified/J1/release archives remain
unchanged and were not substituted for final-candidate proof.

Hosts restored by receipt:

- StackOBot: detached `23098fa7415f148e935acbd4a97e18da04e6c46c`, 201 original files rehashed, content unchanged; 0 newly generated files removed by receipt.
- Lyra: detached `caa68c6799db023f7a24c0d6cfc56124ce27b542`, 183 original files rehashed, content unchanged; 19 newly generated files removed by receipt.
- Live staging: all 5764 file hashes unchanged. No new staging/cook or engine launch.

Only the receipt-owned helpers/fixture copies and build changes were installed;
their roots were removed/restored by receipt. Planned axis/framing/camera/probe
settings were never executed. No host config setting, security setting, foreign
process or owner configuration was changed. Disk floors held throughout.
The strict-build tool temporarily set bUseUnity=false, PCHUsage=NoPCHs,
IncludeOrderVersion=Latest and bEnforceIWYU=true, then restored the original
Build.cs files and removed its 71 generated header TUs before normal builds.
Both restore_ok receipts pass. No engine bench lever/CVar was executed.

7. Harness fixes and deviations

- Missing 093-07 Lyra Switches helper: fresh source timestamps, version stamp,
  rebuilt both hosts; all runtime/editor command names verified. First repair attempt.
- Camera reader: count logged pending pre-switch retirements and de-duplicate
  against subsequent missing rows; chronological self-test and old-reader negative pass.
- Preflight decoded an existing Python BOM as a character: use utf-8-sig,
  matching Python's loader. First repair attempt; no engine leg affected.
- PowerShell stdin changed an em dash in the gotcha scanner: use a Unicode
  escape; G482 collision check passes across 71 refs. First repair attempt.
- Copied resize/camera completeness exception: removed before any engine leg
  to enforce L5 w=r. No observed result was retuned or made easier to accept.
- The VC environment script printed a vswhere lookup diagnostic; it still
  selected MSVC14.38 and all CPU compile/test commands passed. No environment
  or toolchain setting was changed.

Required engine work is unfinished solely because of the documented scope stop.
The build driver already running at the stop was allowed to finish; subsequent
actions were audits, preservation and restoration. There was no failed engine
prediction, no qualifying replay, no delivery claim, and no push.

8. NEEDS-DECISION

Authorize restoring fovDeg's pre-existing GetFOVAngle/LockedFOV behavior in
AnomalyViewport.cpp and a focused locked-FOV metadata regression check, then
resume K7 from PIE PF-1 on the resulting candidate. The admission implementation
is reviewable but unqualified. L3's camera-timing point is already settled;
it is not being reopened.

VERDICT: NEEDS-DECISION
