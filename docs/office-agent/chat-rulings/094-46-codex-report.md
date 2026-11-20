<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, danger-full-access resume, exit 0, 4,706 s, 08:01:06-09:19:32 IST). Body below is byte-identical to report.md, sha256 D19B8CB41F5F5EEEA0223BDC5E5197632153ECA90B5688F242E04B17844BEDED. -->
094-46 stopped with **NEEDS-DECISION**. GN4 is implemented and compiled. The combined D3D11 run passed H4, texture G1–G3 and G5, but **D4 failed the sealed GN2 noise floor on 2,440 of 2,496 unlabelled frames**. Further launches stopped. The cause of the extra differences is not established.

GN4 now writes label_transition_camera_clipping_unconfirmed_entries by summing the existing camera_clipping_unconfirmed transition-entry counts. This preserves d7eee97’s name and meaning (AnomalyLabelWriter.cpp:1350; CountEntryReasons), including multiple entries in one frame. See the [counter helper](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyTransitionCounts425.h:5) and [summary writer](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp:330). The counter is 0 in all three sessions and agrees with their annotation entry sums. The nonzero unit test passed; replacing accumulation with assignment failed as predicted. The accepted session_global_* compatibility keys are documented in [09446-camera.md](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09446-camera.md). The anomaly implementation and slab algorithm remain unchanged from the accepted port.

The combined bench preserves the original 2,352 texture-regression frames, then appends six alternating 48-frame near/far holds. This permits one held camera session with the complete object regression. The new IAICameraRegression switch requires a non-Shipping build, IAIBench, EA_Core, IAICameraSlabBench and the requested camera route. The original camera-only schedule remains unchanged without that switch. See [AnomalyCameraSession425.cpp](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyCameraSession425.cpp:24).

Two baseline-near control sessions used the same binaries, seed 4242, D3D11, 1280×720, settings and pose schedule as the positive. The reader recorded each frame’s 256-bin distribution of maximum absolute RGB-channel difference per pixel, plus counts above every level. The [floor receipt](D:/IA_Europa/Receipts/09446/floors/D3D11-1280.json) was sealed at **03:22:02.800511 UTC**, before the positive launched at **03:23:29.798393 UTC**. Its SHA-256 is 79d01f09c03a3cad3b98bf7a0da3a4ced4be071897018760e30a1ae36816a57d. Neither threshold was changed.

| D4 measurement | Result |
|---|---:|
| Sealed maximum absolute channel difference | 1 level |
| Sealed maximum count at that level | 560,155 pixels |
| Baseline controls’ camera-labelled frames | 0 / 0 |
| Positive spans, inclusive | 2352–2399; 2448–2495; 2544–2591 |
| Labelled frames | 144 |
| Minimum pixels above the floor on each labelled frame | 921,600 |
| Unlabelled frames exceeding the delta bound | 2,440 / 2,496 |
| Maximum unlabelled delta | 2 levels |
| Maximum pixels above the floor on an unlabelled frame | 254 |
| Maximum count at level 1 | 296,540 |
| Count-bound failures | 0 |
| Near plane at Stop, all sessions | 10, restored |

The baseline-near can-fail labelled zero frames. Reader can-fails rejected excessive delta, excessive count and a zero-floor violation; the shifted-label test found six boundary mismatches. The previously accepted 154-check slab self-test was unchanged and was not repeated under FJ1. Example frame 2352: [baseline control](E:/IA_BuildCache/09446-evidence/D3D11-1280-control-a/01/plugin-frames/epoch01/frame-002352.png) and [positive](E:/IA_BuildCache/09446-evidence/D3D11-1280-positive/01/plugin-frames/epoch01/frame-002352.png).

All three D3D11 sessions produced **2,640 frames, 0 drops and 780 object-mask frames**. Each had 15 blink, 19 LOD, 22 checker and 13 pink fires. Every LOD event changed target pixels. Checker and pink reached **strict whole-frame IoU 1.000** on 286 and 157 labelled frames respectively, with 0 spill, 140 clean edge frames and 35 residue-free reverts. No explained-pixel fallback was used. Texture fires were on static targets; EA_Core supplied no skeletal target. The accepted fixture skeletal proof remains unchanged. G5 recorded r.CustomDepth **1/1/1**, polluted 0, enabled/tagged custom-depth components 0, stencil writes 0 and unavailable 0.

**GN3 remains incomplete.** D3D12 1280×720, D3D12 1279×719, the default-only additive comparison against c5d0919 and the office-card read-command check are unrun because D4 failed. Defaults remain blinking, lod_corruption, missing_texture and corrupted_texture; camera_clipping remains opt-in.

Evidence is retained in [result-summary.json](D:/IA_Europa/Receipts/09446/result-summary.json), [paired-pixels.json](D:/IA_Europa/Receipts/09446/H4/D3D11-1280-positive/01/paired-pixels.json), and the [receipt index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09446-receipts.json). For each suffix control-a, control-b and positive, session receipts are under D:/IA_Europa/Receipts/09446/H4/D3D11-1280-&lt;suffix&gt;/01, and raw frames, previews and settings are under E:/IA_BuildCache/09446-evidence/D3D11-1280-&lt;suffix&gt;/01. The readers are [camera_reader09446.py](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/camera_reader09446.py), [host_reader09446.py](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/host_reader09446.py) and [h4_reader09446.py](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/h4_reader09446.py).

Entry and exit FJ1 checks passed. Full P9 passed in **344.016 seconds**: 382,970 inventory files and 22,628 rehashed files. Editor and Game builds each passed **attempt 1**, in **76.263 / 72.496 seconds**, with no linker errors. The strict-include path, focused tests, string scan, comment/lever audit and archive rehash passed. The existing Shipping binary was retained; no Shipping rebuild is claimed.

Bench settings were runtime-only: -game, -nowrite, IAIBench, IAICaptureFallback, IAICameraSlabBench and IAICameraRegression; controls additionally used IAICameraBaseline. The four-target blue material (0.05, 0.2, 0.5), EA_Light 5000→0, six shadowless fills of intensity 3, bloom 5→0, eye quality 0→2, method −1→3, exposure 0→−1.875 and physical-camera exposure off were reused. The far view was EA_Camera at (−550,0,480), pitch −12, FOV 50; near holds used (600,0,160). AA and motion blur were off, screen percentage 100, dynamic resolution off and frame cap/capture pacing 30. The bench selection floor was 0.5%; normal selection/pacing restoration was receipted. Near 100 restored to 10; controls held 10. All session-only material/light/view/exposure changes expired with normal process exit, with no saved asset/config changes. r.CustomDepth was read only. The kit was restored to runtime-disabled status.

C: before→after was **23.8147→23.8110**, **23.7899→23.7886** and **23.7794→23.7772 GiB**; no session lost 1 GiB. Final free space was C: **23.773**, D: **53.084**, E: **64.100 GiB**. There were **0 runtime crashes, 0 upload signs and 0 protected changes**. All owned runtime/build processes ended; foreign processes were observed and untouched. No security, paging, power or machine settings changed. Protected refs, fixture assets, both material generations and the host’s protected files passed exit comparison.

Scope adjustments: the guarded combined pose schedule replaces the camera-only timing only for this regression; no production behaviour changed. A single odd-width default-only compatibility run was planned under GN3’s singular “a default run” to preserve disk reserve; it never launched. Remaining gates were stopped, not weakened or retried. No floor was retuned.

Local commits on feat/europa-port:

- 1e65b6fc9437e90d141f31d3191f06c052ba808f — GN4 and combined bench schedule.
- afa3ea08d63e6f4d134ced83b97b4bd4d7cd9b0b — tested kit/readers and sealed-floor workflow.
- **eea2acdae04b933fe3188fb4bc0d160e3581ed47** — final W4 results/evidence; worktree clean.

**Commits pushed: none. Tags created: none.** Origin remains c5d0919e5074404d26da8c16ccf845b360bde383. All sessions ran at afa3ea08; the final commit changes documentation only.

Every EXE/DLL hash and retained archive path is listed below. Archive roots: **A** = E:/IA_BuildCache/09446-evidence/binary-archive/; **B** = E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/; **C** = E:/IA_BuildCache/09446-evidence/selftests/. The [full inventory](D:/IA_Europa/Receipts/09446/exe-dll-inventory.json) also records companion build files.

| Archive path relative to root | SHA-256 |
|---|---|
| A/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe | 6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861 |
| A/Binaries/Win64/StylizedParisStreet.exe | b3dd0a5ee29e55a788be1cd42265a7b17d90b43ba2ce20b845e730fba4ee8333 |
| A/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll | d2d14a80460102591c01ed4c7a5a6bdbf475d3ab97dcc9c14cb4e77bc3aa9e04 |
| A/Binaries/Win64/UE4Editor-StylizedParisStreet.dll | 8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c |
| A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | 2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940 |
| A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | 9c5a4846cdc17435168fe0b26344982e36431953c8c647a3e3c00f0ae17ba6e6 |
| A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | 0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b |
| A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | 00b2e8f49f0f5394eab3b8c027ec908d7a0af9efbc574e625ffb905b2e6a256d |
| A/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | 3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb |
| B/UE4Editor.exe | 4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387 |
| B/CrashReportClientEditor.exe | ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7 |
| B/UnrealBuildTool.exe | 07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0 |
| B/ShaderCompileWorker.exe | 6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6 |
| C/counter.exe | 6da0bb04752fc859b037f7baa91a9c4a445f94a1b6f87575ebf00d96ac476f5c |
| C/counter-mutant.exe | de65d091c96eeaca12d6e665aad28f438d87aead4a1ccba29c8f155a22012429 |

VERDICT: NEEDS-DECISION | D4 exceeded the sealed noise floor; remaining qualification stopped, evidence committed locally.