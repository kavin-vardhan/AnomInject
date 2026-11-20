<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, danger-full-access resume, exit 0, 10,494 s, 16:23:04-19:17:58 IST). Body below is byte-identical to report.md, sha256 89C6E1E18B8439A5D620EEBD13BEA73072E1B9FB000F28F69365931E307ADC57. Stage 1's checkpoint report is runs\...\report-stage1.md. -->
Stage 1 passed and Code pushed **`c5d0919e5074404d26da8c16ccf845b360bde383`**. Stage 2 stopped on the camera-clipping D4 gate: **914 unlabelled frames differ from the control**. Its D3D12 legs remain unrun. The final local commit is **`c706b7cb0fc50ef02461ff611413800dc54ee87d`**; Codex pushed nothing.

The accepted 4.25 materials remain unchanged: checker SHA256 `5222b3eeb7cdd1f5b58b5975d568b699c7430949b024c67e6f5b1ae6b88a7bec`, pink `b88d88615286a6c9a3648a5c80ab3889ca8f6c648c378e8984ba3d01e79c552a`. GG1's readback retains six usage flags, no Nanite flag, opaque/two-sided/Default Lit, pink (1,0,1), and the release checker graph. The 5.1 assets are hash-unchanged. The fixture's SkeletalCube and three-instance mesh retain GF4's IoU 1.0 on 1,896 frames, with zero new magenta; EA_Core has no skeletal target.

The texture implementations retain `d7eee97` behavior through compile-only compatibility changes: [missing_texture](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:72) and [corrupted_texture](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp:72). This resume changes their bench conditions, not either anomaly.

GM2's [bench helper](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/EuropaTextureBench09444e.cpp:83) applies a runtime blue BasicShapeMaterial MID, Color (0.05,0.2,0.5,1), to all four targets including EA_Floor. It turns EA_Light down to zero and adds six shadowless fills at intensity 3. It holds EA_Camera at (-550,0,480), pitch -12 degrees, FOV 50. The helper is restricted to non-Shipping, `-IAIBench`, EA_Core sessions; its host Source copy is byte-equal. No host asset or config is saved.

The complete calibration selects **-1.875 EV**, the brightest passing exposure; all 15 brighter grid values fail. Bloom is off, eye-adaptation quality is 2/method 3, and physical-camera exposure is off. Every scored `-game` session requalifies before firing. Target means are 0.4081–0.6168 of full scale, saturated pixels zero, checker gaps 25.532–57.988, and pink hue passes. Both swaps change 100% of every target silhouette. The floor, cubes and LOD target cover 23.492%, 8.506% each and 12.871%; centres are inside the frame. No target exclusion or explained-pixel fallback is used.

Stage 1 results:

| Leg | Frames / drops | Checker / pink fires | Minimum whole-frame IoU | Dirty timing edges | Revert residue | Minimum LOD changed pixels |
|---|---:|---:|---:|---:|---:|---:|
| D3D11 1280×720 | 2352 / 0 | 22 / 13 | 1.000000 | 0 / 140 | 0 / 35 | 68036 |
| D3D12 1280×720 | 2352 / 0 | 22 / 13 | 1.000000 | 0 / 140 | 0 / 35 | 68036 |
| D3D12 1279×719 | 2352 / 0 | 22 / 13 | 1.000000 | 0 / 140 | 0 / 35 | 67886 |

Each leg passes H4 and texture G1–G3/G5: 69 events, 780 masks, all 443 texture frames at strict IoU 1.0, zero changed pixels outside masks, every LOD event visibly changed, zero census errors and `unavailable=0`. Frame pairing, held view, pre-UI can-fails and schema checks pass. G5 records `r.CustomDepth` 1/1/1, zero enabled/tagged/polluted components, zero host stencil writes and clean restoration.

The accepted fixture texture proof remains: checker fires 13 static/9 skeletal times and pink 5 static/8 skeletal times; G2 changes every mask pixel and G3 is 1.0. G4 has zero new magenta on 2,352 paired frames, covering checker 169 static/117 skeletal and pink 65 static/92 skeletal frames. G5 has three readings of 1 and 35 residue-zero reverts. The usage-flag mutant changes zero pixels on 365 frames; the early-label reader mutant yields IoU 0. Stage 1's early/late-edge mutants also fail. Example frames: [checker](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/frame-000743.png), [pink](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/frame-000719.png).

Code independently recomputed Stage 1, scanned it and confirmed the push in [STAGE1-PUSHED.txt](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/STAGE1-PUSHED.txt). Read-only remote verification matched the full SHA before Stage 2 began. The sealed [Stage 1 report](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/report-stage1.md) lists every session, preview, settings receipt and reader; its statement that review was pending records its earlier checkpoint. Code's [scan](D:/IntrusiveAnomalies/_reviews/094-45r-stage1-prepush-scan.md) and [office card](D:/IntrusiveAnomalies/_reviews/094-45r-office-card-europa.md) cover the pushed texture stage.

Stage 2 enables the release [camera-clipping class](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.cpp:28) through the existing CameraCachePOV compatibility helper. The [session adapter](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyCameraSession425.cpp:20) holds it from Start to Stop, evaluates the original slab predicate after the camera, emits whole-frame rows without an object mask, and restores the near plane. [Capture integration](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp:107) serves C4/C5. Tests prove class/header equality to `d7eee97` after compile-only normalization, an exact release accumulator block and all 55 row/summary keys. The unchanged slab self-test passes **154 checks**; legacy/AABB/frustum mutants fail. Three pixel-reader mutants catch early labels, unlabelled changes and unchanged positives. [Test evidence](D:/IA_Europa/Receipts/09445r/Stage2/selftests.json), [reader tests](D:/IA_Europa/Receipts/09445r/Stage2/reader-selftests.json).

The camera bench uses seed 4242 and the same held pose schedule in both runs: 624 far frames, then six 288-frame near/far holds. The near eye is (600,0,160), 50 cm before EA_LOD's front bounds. `-IAICameraSlabBench` and `-IAICameraBaseline` are non-Shipping, `-IAIBench`, EA_Core-only switches. The baseline holds near=10; the positive holds near=100.

| Camera D3D11 result | Baseline control | Positive |
|---|---:|---:|
| Frames / drops | 2352 / 0 | 2352 / 0 |
| Confirmed labelled frames | 0 | 864 |
| Distinct positive spans | 0 | 3 |
| Unconfirmed frames | 0 | 0 |
| Near plane restored at Stop | 10 | 10 |
| `r.CustomDepth` readings | 1 / 1 / 1 | 1 / 1 / 1 |

Positive spans are frames **624–911, 1200–1487 and 1776–2063**. Every labelled frame changes at least **902,398 pixels** against its same-index control. The baseline session labels zero frames, satisfying the runtime can-fail. State/schema/view checks pass, with zero custom-depth components, tags, pollution, stencil writes, event masks or unavailable components.

**D4 fails:** of 1,488 unlabelled frames, 574 have zero changed pixels, 390 have one, 401 have two, and 123 have three above the frozen greater-than-two-RGB-level threshold. Thus **914 unlabelled frames fail**. Raw any-level differences are retained separately, reaching 383,768 pixels on an unlabelled frame. No region was excluded and no threshold was changed. The rendering cause is not established. The failed-gate rule stopped further work; camera D3D12 is unrun. Example: [positive frame 746](E:/IA_BuildCache/09445r-evidence/Stage2/D3D11-positive/01/plugin-frames/epoch01/frame-000746.png), [baseline frame 746](E:/IA_BuildCache/09445r-evidence/Stage2/D3D11-control/01/plugin-frames/epoch01/frame-000746.png).

Camera evidence: [paired per-frame results](D:/IA_Europa/Receipts/09445r/Stage2/legs/D3D11-positive/01/paired-pixels.json), [summary](D:/IA_Europa/Receipts/09445r/Stage2/result-summary.json), [committed reader](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/camera_reader09445r.py:78), and [evidence index with absolute paths/hashes](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09445r-stage2-receipts.json). Raw sessions are under `D:\IA_Europa\Receipts\09445r\Stage2\legs\D3D11-{control,positive}\01`; raw frames are under `E:\IA_BuildCache\09445r-evidence\Stage2\D3D11-{control,positive}\01\plugin-frames\epoch01`.

FM1 build accounting: Stage 1 Editor/Game attempt 1 passed in 32.817/44.658 seconds. Stage 2 Editor attempt 2 failed because `ToSharedRef()` was called on a `TSharedRef`; the one-line compile correction passed on attempt 3 in 44.843 seconds. Stage 2 Game attempt 2 passed in 83.521 seconds. Successful builds have zero compiler/link errors; the accepted engine-warning path remains. Both 27-file archives rehash. The changed-source audit passes all 12 files; binary string checks confirm the camera implementation in the Game executable and capture DLL. The archived Shipping executable is unchanged and has no new camera bench switch. [Build/archive evidence](D:/IA_Europa/Receipts/09445r/Stage2/binary-archive.json), [source/string audit](D:/IA_Europa/Receipts/09445r/Stage2/source-audit.json).

Execution deviation: the first unscored Stage 1 calibration used the inherited 900-second bound, which was insufficient for 198 geometry-plus-preview steps. Its owned-stop receipt is preserved. Only the remaining calibration slot received 2,700 seconds under the recorded no-fire retry predicate; scored legs retained 900 seconds. No measured gate was retried or weakened. Stage 2 stopped immediately after its failed measured gate.

Runtime settings were receipted: blue base and preview swaps, fills/point-light intensity, bloom/manual exposure, held view, AA/motion-blur pins, 0.5% bench selection floor, seed 4242 and capture pacing. Texture reverts restore the blue session base. Capture settings restore at Stop; runtime-only actor/light changes expire with each closed process. Camera Stop restores near=10 and the prior camera state. The launcher is disabled and all owned runtime processes are closed. No machine settings changed and no cleanup deletion was performed; build-tool removals are recorded by receipt.

C: before/after readings (GiB), recorded per session:

| Session | Before | After |
|---|---:|---:|
| Stage 1 calibration 01 | 20.4301 | 20.4288 |
| Stage 1 calibration 02 | 20.4281 | 20.4019 |
| Stage 1 D3D11 1280 | 20.4400 | 20.4738 |
| Stage 1 D3D12 1280 | 20.4548 | 20.4531 |
| Stage 1 D3D12 1279 | 20.4384 | 20.4359 |
| Camera control | 20.4175 | 20.4162 |
| Camera positive | 20.4015 | 20.4006 |

No session drop reached 1 GiB. Final free space is C:20.397, D:53.255, E:69.771 GiB. Full entry P9 and final FJ1 checks pass. Foreign activity was recorded without interference. **Runtime crashes: none; upload signs: none; protected changes: none.** Protected refs, fixture assets, preserved fixture config and both material generations remain unchanged. [Final exit receipt](D:/IA_Europa/Receipts/09445r/Stage2/exit.json).

The defaults remain **blinking, lod_corruption, missing_texture, corrupted_texture**. Camera clipping is opt-in and remains locally unqualified. Stage 1's pushed commit stays intact. Local Stage 2 commits are `896b641`, `24a120e`, `0ec5bec` and final **`c706b7cb0fc50ef02461ff611413800dc54ee87d`**, with W4 and a clean worktree. Commits pushed by Codex: **none**. Code pushed `feat/europa-port` through `c5d0919e5074404d26da8c16ccf845b360bde383`. No tags or spike changes.

EXE/DLL SHA256 hashes and archive links follow. “1” and “2” identify the Stage 1 and Stage 2 archives; identical hashes share a row. The [complete inventory](D:/IA_Europa/Receipts/09445r/Stage2/exe-dll-inventory.json) also lists all non-executable build artifacts. The unchanged slab test executable is listed separately.

| Binary | SHA256 | Archives |
|---|---|---|
| Europa Shipping (unchanged) | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe), [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe) |
| Europa Game | `bc4864d33cdb81228b3b2298c1aecc92c3189507084a19327125a4dc72b63a53` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Binaries/Win64/StylizedParisStreet.exe) |
| UE4Editor-EuropaPhaseAProbe.dll | `927462edd2a3fa243e1249a92b7fd3955f45cd9c2bb5fbf52c4075dc1bf58b11` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll), [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll) |
| Europa host module | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Binaries/Win64/UE4Editor-StylizedParisStreet.dll), [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Binaries/Win64/UE4Editor-StylizedParisStreet.dll) |
| UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll), [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll) |
| UE4Editor-AnomalyCapture.dll | `0376ba82c8edf89cd58a28e539eb0cda06f8ccf928eef8ccf5425e55199682c6` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll) |
| UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll), [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll) |
| UE4Editor-AnomalyInjector.dll | `5bb246f0299f2409c62655005663972ee4900f99386b4c5f3b9eeea01cf84701` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll) |
| UE4Editor-AnomalyShaders.dll | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` | [1](E:/IA_BuildCache/09445r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll), [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll) |
| Europa Game | `99063c8ff0b655b8b56ca6c06cdb7b95931351d7107c2010bc979d1b2c068574` | [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Binaries/Win64/StylizedParisStreet.exe) |
| UE4Editor-AnomalyCapture.dll | `e167eea3d7100d6adb67460c66765280dfbedc499b41e183aa7e86330470d9aa` | [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll) |
| UE4Editor-AnomalyInjector.dll | `00b2e8f49f0f5394eab3b8c027ec908d7a0af9efbc574e625ffb905b2e6a256d` | [2](E:/IA_BuildCache/09445r-evidence/Stage2/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll) |
| UE4Editor.exe | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` | [engine](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/UE4Editor.exe) |
| CrashReportClientEditor.exe | `ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7` | [engine](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/CrashReportClientEditor.exe) |
| UnrealBuildTool.exe | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` | [engine](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/UnrealBuildTool.exe) |
| ShaderCompileWorker.exe | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` | [engine](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/ShaderCompileWorker.exe) |
| Slab self-test | `041ea9503b583dbbd148882546e96cfbfb19a79e7bcc9184459f91a3d202b738` | [test executable](E:/IA_BuildCache/09445r-evidence/Stage2/selftests/slab.exe) |

VERDICT: NEEDS-DECISION | Stage 1 pushed; camera-clipping D4 fails on 914 unlabelled frames; D3D12 unrun.
