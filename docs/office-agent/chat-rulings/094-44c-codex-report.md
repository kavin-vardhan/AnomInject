<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 6,952 s, 07:53:54-09:49:46 IST). Body below is byte-identical to report.md, sha256 533F3CA8ED57903E577DBFFD4D1326DA8EB222A370309B783E61412D4C78640A. -->

094-44c stops at G5 with NEEDS-DECISION. The fixture correction, texture firing counts, pixel agreement and both runtime negatives pass. The live `r.CustomDepth` proof is incomplete: one value reply was logged; three were required. The two posted-query receipts are not measurements. No subsequent session or build was run. [Full measurements](<E:/IA_Fixtures/Outline425/Receipts/09444c/stop-result.json>).

The accepted materials remain unchanged: checker SHA256 `5222b3eeb7cdd1f5b58b5975d568b699c7430949b024c67e6f5b1ae6b88a7bec`; pink `b88d88615286a6c9a3648a5c80ab3889ca8f6c648c378e8984ba3d01e79c552a`. GG1’s readback remains applicable: six usage flags, no Nanite flag, two-sided/opaque/Default Lit, pink `(1,0,1)`, checker graph equal to `d7eee97`. Both 5.1 assets and all nine fixture assets are unchanged. No authoring session was needed. [Material receipt](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09444-material-receipts.json>).

GH2 was frozen before any anomaly fire at exposure **−2.1875**, bloom off and fixed manual exposure. Untouched target means were **51.48/51.68** of 255; checker contrasts **40.18/40.18** levels; pink means approximately **(214,0,202)**. The runtime pose uses the existing cube-based static target and SkeletalCube; visible shares are **7.1491%/6.6850%**, below the unchanged 25% ceiling. GF4’s accepted skeletal/instanced proof remains 1.000 IoU over 1,896 frames with zero new magenta. [Frozen settings and readbacks](<E:/IA_Fixtures/Outline425/Receipts/09444c/freeze.json>).

In 4.25, quality 0 disables the eye-adaptation show flag and forces compensation to 1. The working bench uses quality 2, method 3, and physical-camera exposure off. [ShowFlags.cpp](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/ShowFlags.cpp:367>), [manual exposure and compensation](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/PostProcess/PostProcessEyeAdaptation.cpp:240>), [disabled-show-flag branch](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/PostProcess/PostProcessEyeAdaptation.cpp:307>).

Both texture implementations retain `d7eee97` behaviour: the diff is the compat include and two override-material pointer reads through `AnomalyCompat::OverrideMaterial`; apply, revert and installed-state logic are otherwise unchanged. The private pass still obtains the current component proxy every frame. [missing_texture](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:72>), [corrupted_texture](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp:72>), [compat helper](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Compat/AnomalyEngineCompat.cpp:19>), [fresh proxy](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyPrivatePass425.cpp:367>). No native plugin behaviour changed in this resume.

The outline-off positive run saved **2,352 paired frames, zero drops**, seed 4242, with normal exit. G1–G3 pass:

| Anomaly | Static / skeletal fires | Affected frames | Minimum whole-frame IoU | Clean edge frames | Residue-zero reverts |
|---|---:|---:|---:|---:|---:|
| missing_texture | 13 / 9 | 286 | 1.000 | 44 | 22 |
| corrupted_texture | 5 / 8 | 157 | 1.000 | 26 | 13 |

Every affected frame changes exactly its mask: **65,886 static pixels** or **61,609 skeletal pixels**, with **zero changed pixels outside**. All 70 edge frames have zero changes; no early/late frame. Examples: [checker frame 743](<E:/IA_Fixtures/Outline425/Receipts/09444c/textures/texture-off/01/plugin-frames/epoch01/frame-000743.png>) and [pink frame 719](<E:/IA_Fixtures/Outline425/Receipts/09444c/textures/texture-off/01/plugin-frames/epoch01/frame-000719.png>).

G5’s measured component state passes: all **28** before/after records match, and enabled/tagged/unexpected/polluted counts remain **0 on all 2,352 frames**. All 35 texture reverts report residual 0. However, the log contains only startup `r.CustomDepth = "1"`; requests posted at frames 80 and 2300 produced no command/value replies. **G5 therefore fails for missing evidence**, and the three-reading requirement was retained. [G5 result](<E:/IA_Fixtures/Outline425/Receipts/09444c/textures/texture-off/01/texture-metrics.json>), [query receipts](<E:/IA_Fixtures/Outline425/Receipts/09444c/textures/texture-off/01/console-probe.jsonl>).

Both runtime can-fails pass. The fixture-only no-skeletal-usage mutant renders the baseline grey: **29 skeletal events / 365 affected frames**, nonempty masks, **0 changed target pixels**, and three default-material fallback log lines; G2 rejects it. Forty real static checker masks shifted one frame early have IoU 0. The positive run supplies another **35 early-label mutants across both textures**, all IoU 0. Mutant asset SHA256: `0e5172773c260935646128a3b82c6324030474d9efeba980c9c900e637b93466`. [Runtime negative evidence](<E:/IA_Fixtures/Outline425/Receipts/09444c/runtime-negatives.json>), [both-texture early negatives](<E:/IA_Fixtures/Outline425/Receipts/09444c/early-positive.json>).

G4’s outline-on comparison, the host’s D3D11/D3D12/1279×719 regressions, and camera_clipping’s port, slab spans, control, restoration and examples were **not run**, under the failed-gate stop order. Local defaults remain blinking, lod_corruption, missing_texture and corrupted_texture; origin remains at `8df75de` with blinking + lod_corruption. The local default change is not qualified for push. The existing office card remains unchanged.

The fixture Editor and Game each used **3/3 FM1 attempts, all successful**, BelowNormal with at most two actions. Editor times: **19.24 / 8.20 / 11.80 s**; Game: **33.27 / 34.88 / 30.76 s**. These compile fixture calibration helpers; the fixture plugin remains detached at `8e59a56`. No host build or launch occurred. Python 2.7/keyword-Rotator, camera-settle, scoped-kit 3-positive/3-negative and pixel-reader negatives pass. All archived binaries rehash correctly. [Build and test receipts](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09444c-calibration-receipts.json>).

All bench changes were runtime-only and ended with their processes: exposure −2.1875; bloom/AA/motion blur/dynamic resolution 0; screen percentage 100; eye-adaptation quality 2/method 3; physical-camera exposure off; point light intensity 0; six movable white, shadowless directional lights at intensity 3; FOV 30, pitch −12°, distance 1,300, lateral offsets ±125, skeletal/static scale 5/1.3; baseline material swaps; outline blendable off; mutant override only in its leg. C5 used seed 4242 once, interval 0.8 s, hold 0.4 s, one concurrent event, fixed 1/30 s and pacing 1, restored at Stop. Positive routes were the four local defaults; the mutant requested only missing_texture. All launches used the armed kit, `-IAIBench` and `-nowrite`. No Config, asset or machine setting was saved.

Recorded deviations/setup failures: five complete no-fire calibration protocols (303 previews/2,424 frames) were retained, including four nonqualifying results; thresholds were never lowered. One standalone Python setup had no frames because the module is Editor-only. One early PIE setup read a stale camera and was stopped; subsequent setup waits for the updated POV. Both roots exited 0. That PIE editor also auto-started launcher children: 12 verified owned descendants were closed by identity/PID after stop intents; foreign processes were untouched. Later launches used `-BENCHMARK` to suppress automatic launcher startup. [Owned-child receipt](<E:/IA_Fixtures/Outline425/Receipts/09444c/textures/preflight-pie/01/owned-descendant-cleanup.json>), [engine launch condition](<E:/EpicGames/UE_4.25/Engine/Source/Editor/UnrealEd/Private/EditorEngine.cpp:615>). PIE’s incidental pre_ui diagnostic was not claimed as qualified; full host H4 remained pending. No product gate was weakened or rerun after G5 failed.

FJ1 entry and exit pass. Full `owner_p9.main()` passed in **366.79 s**. All **27** host-bound binaries, protected assets/configuration, m51, protected refs and spike branches are unchanged; only local feat moved. Exit free space: **D: 55.70 GiB; E: 87.26 GiB**. **Runtime crashes: 0; upload signs: 0; protected changes: 0.** Build Makefile invalidations have receipts; no evidence or asset was deleted. [Exit receipt](<D:/IA_Europa/Receipts/09444c/exit.json>).

Commits created on `feat/europa-port`:

- `7c3303e5b44fb77f7a44b83434674cea2fde1116` — scoped calibration kit and tests.
- `9788fd645a02e5ec0a4b6a98e5d83229057900c9` — frozen GH2 calibration evidence.
- `90d4275aad88ad25a313dc18e3058e8b13adf376` — final G5 stop evidence, status, journal and G536.

**Commits pushed: none. Tags: none.** The fixture remains outside the plugin repository. Its runtime authoring/pose script is [texture_pose09444c.py](<E:/IA_Fixtures/Outline425/Scripts/texture_pose09444c.py>), SHA256 `40648e6aa8b47218d16b5ed706e8c010aab4a04ebecde4d64e833f221b02034f`.

The complete executable/DLL archive inventory follows. Roots **A/B/C** are respectively `E:/IA_Fixtures/Outline425/Receipts/09444c/binary-archive/`, `manual-binary-archive/`, and `uniform-binary-archive/`; **T** is `E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/`. Each table path is relative to its stated root(s); A+B+C lists all three archived copies. [Full absolute paths and rehash receipt](<D:/IA_Europa/Receipts/09444c/exe-dll-inventory.json>).

| Archive(s) / relative file | SHA256 |
|---|---|
| A / `Binaries/Win64/Outline425.exe` | `a451bf9ac51a9e7cb464f2c49e72f349574246e9a406797ab47820a4b6df571d` |
| A / `Binaries/Win64/UE4Editor-Outline425.dll` | `40545b0d0b92648b69698cdf8803227c0a693ca056a4a909b69e19f44885594c` |
| A+B+C / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll` | `0a74f35993eddb1406901e4b240360d67c5239cfc5a80ac0d6bcd38517b25e12` |
| A+B+C / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `eb9900526b37c559634be99b6b848b77218b34dd97aa296835c31ee45f3a9602` |
| A+B+C / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll` | `b84088b69fe7b66adcd79086941bd8374cbb9beebdc63f959113fb24fdc13b40` |
| A+B+C / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll` | `0ead5c69edabc96a301dc3059cca824696a9736799ab2df10e2446ffdbdd2eb7` |
| A+B+C / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll` | `9ae9b60976505af41e9cdb547380954610bc10ef687ee71083f2240265f578c4` |
| B / `Binaries/Win64/Outline425.exe` | `47a869b07c85c80ae648697f7e0939c5e21136919efd942e092e9b8d841c5ebe` |
| B / `Binaries/Win64/UE4Editor-Outline425.dll` | `31cf9f9d50b30517bf6a63c232a0c5f95eba16b7fcbd2973cff66c9fd20a3b29` |
| C / `Binaries/Win64/Outline425.exe` | `e3aac41dbd05b3c1d9cb31f89a13e8acf6dbbbfb6623f506de0a64fe0e9a68a1` |
| C / `Binaries/Win64/UE4Editor-Outline425.dll` | `67614a21922a9516f56504fbead6a4c0ff1a10bf6f2ec83101671d767c7c9032` |
| T / `UE4Editor.exe` | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` |
| T / `CrashReportClientEditor.exe` | `ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7` |
| T / `UnrealBuildTool.exe` | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` |
| T / `ShaderCompileWorker.exe` | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` |

VERDICT: NEEDS-DECISION | G1–G3 and both negatives pass; G5 lacks live CVar replies, so G4, host regressions and camera_clipping remain pending.
