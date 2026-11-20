Implemented engine-projection geometry and repaired the office reader; J2 remains unqualified.
PF-1 now has zero false labels, but four off-screen fires still apply, triggering the D2 product stop.
Nothing was pushed; hosts and staging are restored, and the candidate is archived as UNQUALIFIED.

**1. Outcome and evidence.** Worked in `E:/IA_BuildCache/_r933/AnomalyInjector` on `fix/followup-capture`. The only new engine leg was Lyra selected-viewport PF-1. It wrote 360/360 frames, with zero labelled or visible-positive rows, zero dropped/stale results, four applied fires and zero zero-match bursts. Independent engine corners remain at X **[-79.365341, -21.432146]**. Four subsequent `MEASURED_ZERO` vetoes remove the annotations; they do not satisfy the frozen no-fire/refusal requirement.

The retained f608d56 negative is rejected by the same checker: 96 false rows and four fires. The new candidate is rejected solely for the four fires. `BeginFire` calls `TryFireSpecific`, which finds the named actor and reaches `ApplyAnomaly` without an on-screen query. This path was unchanged. No product repair, reframing, threshold change or further engine leg followed the failure.

Evidence: [stop review](E:/IA_BuildCache/_r937/stop-review.json), [new PF-1 assessment](E:/IA_BuildCache/_r937/legs/lyra_P_pf1/projection-assessment.json), [retained can-fail](E:/IA_BuildCache/_r937/pf1-retained-control.json). Raw new session: `E:/IA_BuildCache/_r937/legs/lyra_P_pf1/capture/session_20261014-135608`. Prior 093-06 report and `_r936` evidence remain unchanged.

**2. Part A: route, audit and files.** [Design note](E:/IA_BuildCache/_r933/AnomalyInjector/docs/sessions/2026-10-14-093-07-engine-projection.md) and [predictions](E:/IA_BuildCache/_r933/AnomalyInjector/docs/predictions/093-07-engine-projection.md) were committed first, at `a28c72f`, before product edits.

The projector resolves a unique capture-world local player and calls public `ULocalPlayer::GetProjectionData` for its real viewport. Capture refreshes GT geometry at world-tick end, after camera update. The existing view extension freezes the actual serving view's unjittered matrices and rects with the request; validated owner/family identity precedes publication. Frozen world bounds map through normalized coordinates of the copied rect to written image dimensions. No FOV/aspect reconstruction remains in this targeting/label projector. Configured `fovDeg` keeps its meaning.

Timing limitation remains explicit: an early selector query can still see the last completed camera cache before that frame's camera update. Serving-frame labels use the later frozen record. The required camera-switch proof is unrun, so this does not establish full K3 timing compliance.

The design note's complete FOV/aspect audit, using baseline file positions, is:

| Product use | Disposition |
|---|---|
| Viewport BuildProjectionMatrix/BuildViewProjectionMatrix, frustum/coverage/bbox callers | Saved engine matrices replace scalar reconstruction. |
| GetActiveViewInfo | Resolved owner projection; configured FOV and viewport aspect retained as metadata. |
| ComputeBoundsScreenSizeForActiveView | Engine projection supplied to screen-size helper. |
| EvaluateNearClipSlab and confirmation rays | Engine projection coefficients and off-centre geometry; existing distance/trace policy retained. |
| AnomalyNearClipSlab scalar geometry | FOV reconstruction removed; pure SAT/trace logic retained. |
| CameraClipping cache | Engine matrix/rect identity added; FOV is only a cache invalidator. |
| TestVisibility | Resolved engine geometry; obsolete synthetic scalar arguments cannot create a fallback. |
| FAnomalyViewInfo fields | Metadata retained; engine geometry added as values. |
| LabelWriter view.fovDeg/aspect | Existing configured-FOV/viewport-aspect metadata meanings retained. |
| Capture/annotation camera fields | Metadata only, no reconstruction. |
| ChangeStage FOV difference | Camera-change diagnostic only. |
| ControlSnapshot | Camera metadata only. |
| CaptureLetterbox | Bench-only camera setter/restoration and predicted-rect log; engine computes actual rect. |
| SceneViewExtension | Serving view supplies unjittered geometry. |
| Output-height/label sizing | Image resampling from picture dimensions, not camera projection. |
| LOD, stuck-mip, texture, ChangeCase and TexCorruptTree callers | Consume the common engine record. |

Changed product files: `AnomalyViewport.h/.cpp`, `AnomalyNearClipSlab.h`, `Anomaly_CameraClipping.h/.cpp`, `AnomalyInjectorSubsystem.cpp`; capture's `AnomalyCaptureSubsystem.cpp`, `AnomalyFrameCapturer.h`, `AnomalySceneViewExtension.cpp`, `AnomalyMaskSceneViewExtension.cpp`, `AnomalySveCapturer.h/.cpp`, `AnomalyLabelWriter.h/.cpp`, and `AnomalyCaptureOwnership.h`. [Exact paths and header audit](E:/IA_BuildCache/_r937/scope-audit.json) record all 15 files. No engine edit, additional private engine header, module dependency, Build.cs change, shader change or natural-cause code.

Tools changed: `office_checkpoint_i.py`, its Markdown doc, `test_office_checkpoint_i.py`, `camera_clipping_slab_selftest.cpp` and `capture_ownership_selftest.cpp`. Docs: architecture, G481, predictions, journal, CLAUDE status and the prior J2 journal's literal-question-mark correction. G481 was checked across 71 refs before the final docs commit; no collision.

**3. Part B: reader and self-tests.** Removed `accepted_ambiguous` from ownership diagnostics and `a=` from readings. Null `mask_file` contributes to `m` without crashing. The reader writes a local 20-frame contact sheet with frame numbers; no inspection input or manual edge-pair input remains. The owner appends ` g=0` after inspecting it. Host 1 means the first office host; host 2 means the second office host. Nothing from an office host was imported.

Formats:

```text
I<h>.<case> PASS v=<visible events> c=<0 X,1 Y,2 major> b=<scale x1000> #<check>
I<h>.<case> FAIL r=<r> w=<w> v=<v> e=<e> x=<x> m=<m> c=<c> b=<b> #<check> I-fail=<counters>
B<h> n=<rows used> b=<scale x1000> #<check>
```

Machine PASS requires r=w=360, at least three visible events, e=x=m=0, measured paired masks, complete ownership/projection records, no pending/dropped/unpaired/invalid results, and both scale axes within [0.990,1.010]. Final office acceptance also needs the owner's g=0. `x` is a machine view-record check, not independent proof of picture origin.

The frozen PF-2/box estimator uses labelled non-transition rows with finite valid boxes and actual matching masks/counts/bounds/dimensions. Both boxes must be at least one pixel inside each picture edge. Per axis, P and D are projected and drawn box centres relative to the picture centre; use |D|>=5% of that dimension, median(P/D), at least 20 distinct rows per axis, no outlier removal. `b` is round-half-up of 1000 times the worse axis deviation; X wins ties. Insufficient data gives -1. `n` counts distinct rows used on either axis. I check digit weights host, case, pass-bit, r,w,v,e,x,m,c,b by 1..11 modulo 10; B uses `(host+2*n+3*b)%10`.

Actual retained-session outputs were identical on **Python 3.10.11** (numpy 1.26.4/Pillow 9.5.0) and **3.13.1**. Minimum 3.10 is documented and exercised; product reader uses streamed hashing, not file_digest. No new office package is required. Original session hashes match.

| Required self-test | Actual line / status |
|---|---|
| Healthy I PASS | **NOT RUN after PF-1 stop; no PASS line is claimed.** |
| Retained null-mask Lyra FAIL | `I1.11 FAIL r=360 w=360 v=0 e=-1 x=0 m=96 c=-1 b=-1 #9 I-fail=360,360,385,0,0,3,0,0,0,0,0,0,0,0,0,0` |
| Retained StackOBot f608d56 box PASS, exit 0 | `B1 n=96 b=999 #0` |
| Retained Lyra f608d56 box FAIL, exit 1 | `B1 n=96 b=967 #4` |

StackOBot raw axes are 0.999703/0.998737; Lyra axes are 0.967177/0.967742. The generated null-mask contact sheet was visually inspected. [Self-test receipts](E:/IA_BuildCache/_r937/reader-selftests-retained/results.json). The required fourth result remains outstanding, not replaced with a synthetic PASS.

**4. Gate results and can-fails.** U means not run after the first product failure. No old passing engine case is counted as qualification of this head.

| Gate | StackOBot PIE | Lyra PIE | StackOBot staged | Lyra staged | Can-fail status |
|---|---|---|---|---|---|
| PF-1 original selected framing | N/A | **FAIL: 0 false rows, 4 fires** | N/A | N/A | Retained 96-false-row/four-fire session rejected. |
| PF-2 X/Y x 942x869,1123x870,1280x720 | U, 6 legs | U, 6 legs | U, X/Y at 1280x720 | U, X/Y at 1280x720 | Retained Stack box PASS and Lyra .967 rejection established. .813 archived-binary replay and per-axis .96 mutants unrun. |
| PF-3 native-X UV/proxy comparison | U | N/A | U | N/A | Retained self-comparison/1px mutant preflight passes; new-candidate comparison and onset control unrun. |
| PF-4 constrained-aspect 2.39 | U | N/A | N/A | N/A | Full-window mapping mutant unrun. |
| Camera cut at 60, blend at 300 | U | U | U | U | Identity-copy RGB and stale-projection controls unrun. Drops/event continuation/reasons: **no readings**. |

The retained 942x869 negative contains no measured target masks, so the .813 mask-centre estimate cannot be computed from it. This was disclosed in the design before legs; an exact archived-binary replay with an on-screen target was prepared, preserving the estimator, but remains unrun.

Full 093-06 matrix on this candidate: **0/84 run**. Each cell below is unrun; counts include the explicitly planned old-build controls.

| J2 case group | Stack PIE | Lyra PIE | Stack staged | Lyra staged | Planned can-fail/control |
|---|---|---|---|---|---|
| Delayed viewport | U | U | U | U | J1 delayed-owner build; controls below. |
| Healthy owner | U | U | U | U | Wrong-view/frame markers and refusal negatives. |
| Second realtime editor view | U | U | N/A | N/A | Release wrong-view build. |
| Selected viewport, engine-framed positive | U | U | N/A | N/A | Original framing retained separately as PF-1. |
| Two-client, client 0 and client 1 | U x2 | U x2 | N/A | N/A | Wrong-world marker/ownership controls. |
| Resize/reopen generation | U | U | U | U | Stale-generation predicate mutant and stale-result accounting. |
| Same-world ambiguity | U | U | U | U | First-wins predicate mutant. |
| Split-screen and stereo | U x2 | U x2 | U x2 | U x2 | Forced multi-view/stereo and predicate mutants. |
| NotGameView: controller/view/show flag | U x3 | U x3 | U x3 | U x3 | Each forced negative must increment its own reason. |
| J1 UV/proxy and label-pixel regression | U x2 | U x2 | U x2 | U x2 | Shifted edges, missing/blank/wrong-count masks, erased marker, corrupt summary. |
| Synthetic alignment UV/proxy | U x2 | U x2 | U x2 | U x2 | Mask/label identity and frame-barcode mutations. |
| Pairing native/synthetic x 100%/50% | U x4 | U x4 | U x4 | U x4 | Previous-frame mask copies. |
| J1 delayed-build control | U | U | U | U | Exact PIE DLLs; separately hashed rebuilt staged executable. |
| Release wrong-view/target control | U | U | U | U | Exact PIE DLLs; staged wrong-target counterpart. |
| Total | **23 U** | **23 U** | **19 U** | **19 U** | [All 84 case names](E:/IA_BuildCache/_r937/expected-engine-cases.json). |

Normal and strict Editor/Game builds pass on both fixtures with zero errors/warnings; each strict target compiled 71 standalone header TUs. All 69 CPU runner steps matched expected results, including negative controls; additional ownership/family mutants and four reader unit tests on both interpreters pass. Comment stripper changed zero files; diff checks pass. Product source lever audit is 53/53; product binary audits pass for both configurations/hosts. The separate Lyra Editor helper audit is incomplete because one command is missing, detailed below.

String scans: StackOBot 1,788 additions versus J1, Lyra 1,630 versus its f608d56 binary; all attributed, with unknown ASCII/wide sentinels rejected. Cook, containers and staged proof were not produced after the stop. Source/header audit found no out-of-scope dependency or private-header addition.

**5. Actual projection diagnostic.** From the new PF-1 `run_summary.json`; no projection block was added to labels:

```json
{
  "projection": {
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
}
```

Ownership readings for that session: owner scene/target known at start, generation 1, updates 0; requested=sampled=written=colour_served=360, mask_served=384, scene_mismatch=3; dropped, unpaired, pending, stale generation and all three NotGameView sub-reasons are zero. These do not substitute for the failed fire gate.

**6. Heads, binaries, archive and restoration.** Pushed commits: **none**. Remote `fix/followup-capture` remains `1bf7054f5b8664f057282ed863a62ab5dbbd60e3`. Local head is `9514242790f71b480eb947b2ec98b8d0e14903e9`, clean; product remains `f9b93280a3c692cecae377c3ee72c186bdb34852`. Only this feature ref changed. No merge, tag or delivery branch change.

New local commits, all unpublished:

- `a28c72f4fced02269566948f456978d5dbbab382` — docs(capture): freeze engine projection design and qualification predictions
- `5af18992ce81d3c9f5236b9bb7f4cafc418bfee9` — fix(capture): freeze engine projection and repair office readings
- `51cb0d6e9e176a042a88fd95ecbce98c1398a509` — fix(viewport): read camera metadata through the public cache
- `f9b93280a3c692cecae377c3ee72c186bdb34852` — fix(viewport): report camera axis overrides when available
- `7a8810fc416c18af48bf9361531f225bd1571ea2` — docs(capture): record projection PF-1 qualification stop
- `9514242790f71b480eb947b2ec98b8d0e14903e9` — docs(capture): correct PF-1 callsite name

Source tree: `2e91fe78728265e381a5cfb7b359ac4a66006a9e`; unchanged shader tree: `64e18bf3d238c0c10817c34ed03fcb24f5903f06`. The final docs commit changes no Source, Shaders or tools. Prior six unpublished 093-06 commits remain intact, listed in [closure receipt](E:/IA_BuildCache/_r937/closure.json).

Frozen product/helper SHA-256 values follow. DLL rows use the exact `UnrealEditor-<name>.dll` filename; executable names are explicit. These are unqualified candidates. No control executable/DLL was launched in 093-07.

| Binary | StackOBot SHA-256 | Lyra SHA-256 |
|---|---|---|
| StackOBot.exe / LyraGame.exe | `7a69b4ee572d8b33ff0aa5766d8d85e01205605d77724f7fc574128752711158` | `7a49973117abf3836e81e76b9f39c6024de2ffa4f3c1173f8ac19c42ebf98551` |
| UnrealEditor-AnomalyBench.dll | `15550c935a0bb80a353552c36482ed1a4dd50f962dda7b1d7511e10fc4f96e6e` | `d007a4c4281a4b57cca3ce96df25afb27d3c2cd2730c16906bfb76abf3f7494b` |
| UnrealEditor-AnomalyCapture.dll | `965dd45c7b6c8f8bf6df33bfaf247dd4db43c740cb9b87adbcff5d2dbd45cea5` | `893a6462d9c1fad1a87b60b497d7fff43491bfff43469a86c955c8221863b1b5` |
| UnrealEditor-AnomalyControlServer.dll | `1bf6e019029f368449f0a43557b39962a09b580038a6e44972d5318b22a5c232` | `cfd4efff1b72cc262494bcd7166bfb6c47abce0759341ba6a34377feb28b0777` |
| UnrealEditor-AnomalyInjector.dll | `3a628c70503d8ab3f4aedf39579ca7790400f7b74df6c1474ba5f8fff5eb8b3b` | `87d4cb8c5b3dc064edd78e0dfd0690fc4b3517e92dde5a9b10fccd949d235e25` |
| UnrealEditor-AnomalyShaders.dll | `dcd45202db539bfa2c31d8b10bf1d08760e4ee653a27516d2be02d88a5464fa4` | `ab41976cae5540bd8e78619303180085469629d59042a50d59867b07fb021367` |
| UnrealEditor-Review0936.dll | `a37d231c3792beccfb4fe3467ccc76a5d407dfd542c4976e40dd3cefa7672c81` | `e77c9009bfb0af7f09e1a30672ce5a3e1a4971f9a00c02ada42bc159a7f5f537` |
| UnrealEditor-Review0936Runtime.dll | `cc44c9b84d88d56b06bacfef51d3a36355df5277c5e15a82921f8760ff596197` | `25e908575d877e610ce8e3de188b28ccfdd42a9c9c263f033672742eaf1ba5d3` |

Launched engine `D:/UESource/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe`: `5fcbbf4fe39adae10d1d67a3b21e998723ae07ed258c0fc7fa4103b029b6381b`. Only the Lyra Editor candidate was exercised in-engine. The retained camera-helper source SHA is `3803e8e115c50c22f5312c0f50f7d107f033a91997f8f1ce6ffa07da795de544`.

Archive: [followup-capture-J2-projection-09307-f9b9328-7A69B4EE-7A499731-UNQUALIFIED](D:/IntrusiveAnomalies/_binary_baselines/followup-capture-J2-projection-09307-f9b9328-7A69B4EE-7A499731-UNQUALIFIED). Frozen payload **541,545,448 bytes**; all files rehash, byte-corruption control rejected, original unchanged. Contains candidate manifests/binaries, frozen source, harnesses, logs and receipts. Raw capture PNGs remain at the session path above. Final report/docs are added as a separately hashed closure supplement. Existing archive index and older archives were not edited.

Restored StackOBot to detached `23098fa7415f148e935acbd4a97e18da04e6c46c`, with 201/201 original files matching; Lyra to detached `caa68c6799db023f7a24c0d6cfc56124ce27b542`, with 183/183 matching. Original content matches. Twenty Lyra files created by this job and the receipt-owned helper/fixture roots were removed by checked paths/receipts. [Restoration rehash](E:/IA_BuildCache/_r937/receipts/restored-rehash.json). All 5,764 live staging hashes match; no staging write or cook occurred. The launched editor PID 5004 exited normally; no owned engine process remains. Disk space remains above both 50 GiB floors.

The only launched leg used these process-local settings, all ended with that process; original host Config/Saved-Config bytes were restored:

```text
r.DefaultFeature.AutoExposure=0; r.EyeAdaptationQuality=0; CaptureBench.Marker=1
IAI.Capture.SVE=1; Delivery=0; Pace=1; Fps=30; Mask=1; TargetMask=1; RunLog=1; FocusGate=0
r.AntiAliasingMethod=0; r.Lumen.DiffuseIndirect.Allow=0
r.DynamicGlobalIlluminationMethod=0; r.ReflectionMethod=0
IAI.Capture.Config 0 30 24 60 4
IAI.Bench.Review937.Projection 942 869 1 none
IAI.Bench.Review936.Observe; IAI.Bench.Review936.PIE selected
IAI.Capture.Start <owned-output> png 777 360 uv_corruption StaticMeshActor_0 tile
-IAIBench -NoLiveCoding -unattended -nosplash; process-local Lyra GameModeBase override
```

Exact launch arguments, owner PID identity, commands and idle readings are in [launch receipt](E:/IA_BuildCache/_r937/legs/lyra_P_pf1/launch.json). Idle gate passed after 69 seconds: three GPU readings 0/1/0%, about 11.5 GiB free physical memory, no foreign editor/game. No foreign process or security setting was changed. Strict-build rule substitutions and generated TUs were restored.

**7. Harness fixes and deviations.** No gate, prediction or threshold was weakened.

| Problem | Correction / retained status |
|---|---|
| UE 5.1 GetViewPoint is protected | One pre-gate implementation correction: public camera-cache metadata; geometry still GetProjectionData. |
| Preliminary workers absent with interrupted strict files | One recovery: restore five Build.cs backups, remove 71 exact receipt-listed TUs, restart full final-head strict/normal builds. Interruption cause not established. |
| CRLF backup compared to normalized git bytes | One correction to path-filtered git hash verification; failed assertion preceded mutation. |
| Initial unit invocation used default TEMP | It auto-cleaned; subsequent commands explicitly used owned `_r937/temp`. Write-location deviation disclosed. |
| Wrong initial stripper path | Corrected to canonical script; zero comment changes. |
| UTF-16 PowerShell preflight JSON | Corrected read encoding; actual idle gate consumes captured stdout. |
| Lyra Editor helper missing Switches | Diagnostic audit found 13/14 commands; Game helper has 9/9. No rebuild attempt before PF-1 product stop. Source contains the command; interrupted incremental-build overlap is only a hypothesis. PF-1 uses Projection, not Switches. |
| Lyra `.uedbg` executable section | One audit correction: attribute by verified PE executable/code flags and surrounding bytes. Unknown-string controls still reject. |

The prepared, unrun camera reader currently counts missing samples only after each switch; a continuation should include pending pre-switch samples when reporting drops. No camera reading is claimed. Minor shell quoting/path and duplicate-patch construction errors occurred before mutation and were corrected; they produced no engine result or gate change.

The requested .813 mask can-fail cannot be read from null-mask evidence; the predeclared supplemental archived-binary replay is unrun. The healthy I PASS, later PIE gates, cook and all staged gates are omitted solely because the PF-1 product stop occurred first. The report deliberately does not substitute old qualification or synthetic output for those missing results.

**8. NEEDS-DECISION.** Authorize a continuation to repair the targeted-fire admission path with a counted engine-projection off-screen refusal, then restart the prescribed qualification order. Also resolve the early-selector/post-camera timing requirement before claiming K3 complete. Rebuild and verify the stale Lyra Editor helper before camera tests. The corrected per-frame labels are evidence of progress; J2 is not qualified or ready to push.

VERDICT: NEEDS-DECISION
