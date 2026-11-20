094-45r Stage 1 passes the implementer gates. All three EA_Core regressions pass H4, texture G1–G3 and G5. Every texture frame uses strict whole-frame IoU; the GM2 explained-pixel fallback is unused. Code’s scan and push are pending. Stage 2 (`camera_clipping`) has not started.

The GM2 helper runs only in the home host, outside Shipping, with `-IAIBench` on EA_Core. It holds EA_Camera at (−550,0,480), pitch −12°, FOV 50. Before C5 it installs a runtime BasicShapeMaterial MID with linear Color (0.05,0.2,0.5,1) on all four targets, including EA_Floor; switches the point light to 0; and adds six shadowless fills at intensity 3. Original slots, colour readback, light states and every setting are receipted. Nothing is saved and no target is excluded. Source: [bench helper](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/EuropaTextureBench09444e.cpp:83), byte-equal to the host Source copy.

The no-fire calibration includes the retained −5 EV probe and a 65-value 0 to −8 EV sweep in 0.125 steps. The new lighting requires recalibration: **−1.875 EV** is the brightest passing exposure; all 15 brighter grid values fail. There are 198 colour previews (1,584 frames), each preceded by a 32-frame geometry probe (6,336 frames), with zero drops. Every scored session independently requalifies the same exposure, view and materials before any fire. Bloom is 0; eye-adaptation quality 2/method 3; physical-camera exposure is off.

Calibration at the selected exposure:

| Target | Visible pixels | Share | Mean /255 | Saturated pixels | Checker gap | Minimum swap visibility |
|---|---:|---:|---:|---:|---:|---:|
| EA_Floor | 216502 | 23.4920% | 0.6168 | 0 | 25.532 | 100.00% |
| EA_HideCube | 78390 | 8.5059% | 0.4081 | 0 | 57.988 | 100.00% |
| EA_BlinkCube | 78390 | 8.5059% | 0.4082 | 0 | 57.841 | 100.00% |
| EA_LOD | 118616 | 12.8707% | 0.4319 | 0 | 46.654 | 100.00% |

All target centres are inside the frame and every drawn share remains inside 0.5–25%. Pink hue passes GK2 on every target. The same-session pre-fire evidence checks both textures over all eight frames, with 100% target-pixel changes in the calibration.

| Leg | Frames/drops | Checker fires | Pink fires | Minimum strict IoU | Timing edges / dirty | Reverts / residual | Minimum LOD changed pixels |
|---|---:|---:|---:|---:|---:|---:|---:|
| D3D11-1280 | 2352/0 | 22 | 13 | 1.000000 | 140/0 | 35/0 | 68036 |
| D3D12-1280 | 2352/0 | 22 | 13 | 1.000000 | 140/0 | 35/0 | 68036 |
| D3D12-1279 | 2352/0 | 22 | 13 | 1.000000 | 140/0 | 35/0 | 67886 |

Every texture fire changes its target; every texture frame has zero changed pixels outside its mask. Both frames before each fire and both frames after each revert are checked against the saved frame immediately before the fire. The early/late-edge mutants fail. The explained-pixel reader also rejects unexplained unchanged pixels and spill; no fallback is selected in these runs.

H4 retains exact frame pairing, masks on every affected frame and none on clean frames, schema/coverage provenance, the held player view, paired pre-UI can-fail checks, every LOD event changing target pixels, and census exhaustion/lost/no-pass/pollution counts of zero. `unavailable=0`. EA_Core has no skeletal candidate; accepted fixture coverage remains the skeletal proof. The ruled EA_Core wall-strip and >55 census waivers remain. G5 records `r.CustomDepth` 1/1/1, zero enabled or tagged components, zero host stencil writes, unchanged component state and zero revert residue.

The accepted material readback and assets remain unchanged: checker SHA256 `5222b3eeb7cdd1f5b58b5975d568b699c7430949b024c67e6f5b1ae6b88a7bec`, pink `b88d88615286a6c9a3648a5c80ab3889ca8f6c648c378e8984ba3d01e79c552a`. Six usage flags, opaque/two-sided/Default Lit, pink (1,0,1), and the release-line checker graph are retained; the 5.1 assets are untouched. Accepted GF4 remains IoU 1.0 on 1,896 skeletal/instanced frames. Fixture texture G1–G3 remains IoU 1.0; G4 remains zero new magenta on 2,352 paired frames; G5 remains startup/frame81/frame2300 readings of 1 and 35 residue-zero reverts. The accepted usage-flag negative changes zero pixels on 365 frames; the early-label reader negative yields IoU 0.

GL3 remains the accepted GM1 fix:9 classifier tests pass, three review mutants fail, and the old empty-reason receipt replays as owned-stop without changing its bytes. No anomaly behavior changed in this resume. The retained `d7eee97` apply/revert logic is in [missing_texture](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:72) and [corrupted_texture](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp:72), using the existing compat override lookup and 4.25 material finders. Defaults are blinking, lod_corruption, missing_texture and corrupted_texture. Camera clipping remains compiled out on 4.25 and outside the auto pool.

Editor and Game each pass fresh FM1 attempt 1, in 32.817 and 44.658 seconds, with zero compiler/link errors, BelowNormal priority and at most two actions. The native change is confined to the home-host helper; production plugin native source and assets are unchanged. The existing carried engine-warning path remains. All 27 archived files rehash.

Execution deviation: Calibration slot01 retained the inherited 900-second scored-run bound, too short for 198 geometry-plus-preview steps. It was closed by a matching PID/time/image owned-stop intent before any C5 or measured anomaly gate. Its incomplete raw FAIL receipt is retained. Only the remaining Calibration slot02 received a bounded 2,700-second allowance, with an exact no-fire/normal-exit/intent/frozen-evidence retry predicate; all scored legs keep 900 seconds. Slot02 passed. No measured gate was weakened or retried.

Runtime-only changes expire with each closed session: base material, preview swaps, fills/point-light intensity, bloom/manual exposure, held camera, AA/motion-blur pins, bench 0.5% selection floor, seed 4242 and capture pacing. Anomaly reverts restore the blue session base; capture settings restore at Stop. The host files remain hash-equal. No machine settings changed; no cleanup deletion occurred. Build-tool removals are recorded by receipt. The runtime launcher is disabled at this checkpoint.

C: readings (GiB):

- H4\Calibration\01: 20.4301 → 20.4288; drop 0.0012.

- H4\Calibration\02: 20.4281 → 20.4019; drop 0.0261.

- H4\D3D11-1280\01: 20.4400 → 20.4738; drop -0.0338.

- H4\D3D12-1279\01: 20.4384 → 20.4359; drop 0.0025.

- H4\D3D12-1280\01: 20.4548 → 20.4531; drop 0.0017.

No C: drop reached 1 GiB. Cache-size observations are retained. The D: floor and 3 GiB launch reserve, E: floor and C: 10 GiB floor remain. Foreign activity was recorded without interference. Full entry P9 and exit fast checks pass; no runtime crash, upload sign or protected change. All owned runtime processes are closed.

Absolute evidence paths and readers:

- D3D11-1280: session `D:\IA_Europa\Receipts\09445r\H4\D3D11-1280\01`; complete gate result `D:\IA_Europa\Receipts\09445r\H4\D3D11-1280\01\result.json`; reader `D:\IA_Europa\Receipts\09445r\H4\D3D11-1280\01\reader.json`; immutable file manifest `D:\IA_Europa\Receipts\09445r\H4\D3D11-1280\01\raw-evidence.json`; frames `E:\IA_BuildCache\09445r-evidence\D3D11-1280\01\plugin-frames\epoch01`; preview/geometry/GK2/visibility/approval files `E:\IA_BuildCache\09445r-evidence\D3D11-1280\01\attempt-01`; base/fill/settings receipt `E:\IA_BuildCache\09445r-evidence\D3D11-1280\01\settings.json`.

Example corrupted_texture: [frame](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/frame-000719.png), [mask](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/mask-000719.png), [reference](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/frame-000718.png).

Example missing_texture: [frame](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/frame-000743.png), [mask](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/mask-000743.png), [reference](E:/IA_BuildCache/09445r-evidence/D3D11-1280/01/plugin-frames/epoch01/frame-000742.png).

- D3D12-1280: session `D:\IA_Europa\Receipts\09445r\H4\D3D12-1280\01`; complete gate result `D:\IA_Europa\Receipts\09445r\H4\D3D12-1280\01\result.json`; reader `D:\IA_Europa\Receipts\09445r\H4\D3D12-1280\01\reader.json`; immutable file manifest `D:\IA_Europa\Receipts\09445r\H4\D3D12-1280\01\raw-evidence.json`; frames `E:\IA_BuildCache\09445r-evidence\D3D12-1280\01\plugin-frames\epoch01`; preview/geometry/GK2/visibility/approval files `E:\IA_BuildCache\09445r-evidence\D3D12-1280\01\attempt-01`; base/fill/settings receipt `E:\IA_BuildCache\09445r-evidence\D3D12-1280\01\settings.json`.

Example corrupted_texture: [frame](E:/IA_BuildCache/09445r-evidence/D3D12-1280/01/plugin-frames/epoch01/frame-000719.png), [mask](E:/IA_BuildCache/09445r-evidence/D3D12-1280/01/plugin-frames/epoch01/mask-000719.png), [reference](E:/IA_BuildCache/09445r-evidence/D3D12-1280/01/plugin-frames/epoch01/frame-000718.png).

Example missing_texture: [frame](E:/IA_BuildCache/09445r-evidence/D3D12-1280/01/plugin-frames/epoch01/frame-000743.png), [mask](E:/IA_BuildCache/09445r-evidence/D3D12-1280/01/plugin-frames/epoch01/mask-000743.png), [reference](E:/IA_BuildCache/09445r-evidence/D3D12-1280/01/plugin-frames/epoch01/frame-000742.png).

- D3D12-1279: session `D:\IA_Europa\Receipts\09445r\H4\D3D12-1279\01`; complete gate result `D:\IA_Europa\Receipts\09445r\H4\D3D12-1279\01\result.json`; reader `D:\IA_Europa\Receipts\09445r\H4\D3D12-1279\01\reader.json`; immutable file manifest `D:\IA_Europa\Receipts\09445r\H4\D3D12-1279\01\raw-evidence.json`; frames `E:\IA_BuildCache\09445r-evidence\D3D12-1279\01\plugin-frames\epoch01`; preview/geometry/GK2/visibility/approval files `E:\IA_BuildCache\09445r-evidence\D3D12-1279\01\attempt-01`; base/fill/settings receipt `E:\IA_BuildCache\09445r-evidence\D3D12-1279\01\settings.json`.

Example corrupted_texture: [frame](E:/IA_BuildCache/09445r-evidence/D3D12-1279/01/plugin-frames/epoch01/frame-000719.png), [mask](E:/IA_BuildCache/09445r-evidence/D3D12-1279/01/plugin-frames/epoch01/mask-000719.png), [reference](E:/IA_BuildCache/09445r-evidence/D3D12-1279/01/plugin-frames/epoch01/frame-000718.png).

Example missing_texture: [frame](E:/IA_BuildCache/09445r-evidence/D3D12-1279/01/plugin-frames/epoch01/frame-000743.png), [mask](E:/IA_BuildCache/09445r-evidence/D3D12-1279/01/plugin-frames/epoch01/mask-000743.png), [reference](E:/IA_BuildCache/09445r-evidence/D3D12-1279/01/plugin-frames/epoch01/frame-000742.png).

- Calibration session `D:\IA_Europa\Receipts\09445r\H4\Calibration\02`, raw frames `E:\IA_BuildCache\09445r-evidence\Calibration\02\attempt-01`, complete sweep `D:\IA_Europa\Receipts\09445r\calibration-result.json`, frozen choice `D:\IA_Europa\Receipts\09445r\freeze.json`.

- Results `D:\IA_Europa\Receipts\09445r\result-summary.json`; focused tests `D:\IA_Europa\Receipts\09445r\focused-tests.json`; edge tests `D:\IA_Europa\Receipts\09445r\edge-reader-tests.json`; build/audit `D:\IA_Europa\Receipts\09445r\builds-and-archive.json` and `D:\IA_Europa\Receipts\09445r\build-audit.json`; final exit `D:\IA_Europa\Receipts\09445r\exit.json`; closed-process audit `D:\IA_Europa\Receipts\09445r\owned-processes-closed.json`; launcher restoration `D:\IA_Europa\Receipts\09445r\kit-restore.json`.

- Committed readers: `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\geometry_reader09445r.py`, `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\calibration_reader09445r.py`, `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\host_reader09445r.py`, `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\texture_reader09445r.py`, `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\capture_phaseb_h4_reader.py`, `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\capture_texture425_reader.py`, `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\europa\capture_c4_reader.py`. The run-folder copies of the first four are byte-equal to those used by the kit. Aggregator `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\result_summary09445r.py`.

Executable/DLL hashes and exact archive paths (complete 27-file plus engine inventory: `D:\IA_Europa\Receipts\09445r\exe-dll-inventory.json`):

| Archive path | SHA256 |
|---|---|
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Binaries\Win64\StylizedParisStreet-Win64-Shipping.exe` | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Binaries\Win64\StylizedParisStreet.exe` | `bc4864d33cdb81228b3b2298c1aecc92c3189507084a19327125a4dc72b63a53` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Binaries\Win64\UE4Editor-EuropaPhaseAProbe.dll` | `927462edd2a3fa243e1249a92b7fd3955f45cd9c2bb5fbf52c4075dc1bf58b11` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Binaries\Win64\UE4Editor-StylizedParisStreet.dll` | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyBench.dll` | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyCapture.dll` | `0376ba82c8edf89cd58a28e539eb0cda06f8ccf928eef8ccf5425e55199682c6` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyControlServer.dll` | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyInjector.dll` | `5bb246f0299f2409c62655005663972ee4900f99386b4c5f3b9eeea01cf84701` |
| `E:\IA_BuildCache\09445r-evidence\binary-archive\Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-AnomalyShaders.dll` | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |
| `E:\IA_Fixtures\Outline425\Receipts\09444\executable-archive\UE4Editor.exe` | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` |
| `E:\IA_Fixtures\Outline425\Receipts\09444\executable-archive\CrashReportClientEditor.exe` | `ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7` |
| `E:\IA_Fixtures\Outline425\Receipts\09444\executable-archive\UnrealBuildTool.exe` | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` |
| `E:\IA_Fixtures\Outline425\Receipts\09444\executable-archive\ShaderCompileWorker.exe` | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` |

Local commits this resume: `c356b7eda776242f9a6a23432e0cdfa48979609f`, `7a224e6b75a83c0693a4814bf85b229cdf2798e3`, `968fc7dacdfe353764a7019ee373a0c17d8d12a3`, `45bc97ab709d372bc1e7df9bab1e637166610b73`, `c5d0919e5074404d26da8c16ccf845b360bde383`. Stage 1 tip: **`c5d0919e5074404d26da8c16ccf845b360bde383`**, clean on `feat/europa-port`. Commits pushed by Codex: **none**. No tags or spike changes. Stage 2 is gated on Code’s matching push confirmation.

Additional diagnostic readers and audit scripts, with absolute paths/hashes, are listed in `D:\IA_Europa\Receipts\09445r\evidence-index.json`. The retained unscored setup session and stop intent are in `D:\IA_Europa\Receipts\09445r\H4\Calibration\01`; its retry predicate is `D:\IA_Europa\Receipts\09445r\H4-retry-Calibration.json`. The partial-preview observations were diagnostic only; the frozen full calibration reader made the choice.
