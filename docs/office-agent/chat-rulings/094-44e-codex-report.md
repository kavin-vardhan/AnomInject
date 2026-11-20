<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 2,516 s, 11:10:06-11:52:02 IST). Body below is byte-identical to report.md, sha256 19688539B70DB4975F50D9A553CB9A0C36C95AEF3551B7B4C1AF7B4233813B1B. -->

094-44e ends **NEEDS-DECISION at the GJ3 pre-fire qualification**. None of the 65 tested exposures qualifies all three EA_Core targets. The armed calibration completed **195 previews / 1,560 frames / 0 drops**, with no auto-injector fires. The failed-gate stop rule was applied before any scored regression. [Measurements](<D:/IA_Europa/Receipts/09444e/calibration-stop-summary.json>).

The host-only helper is compiled out of Shipping and requires both `-IAIBench` and exact EA_Core. It records runtime bloom 0, eye-adaptation quality 2, method 3, and physical-camera exposure off. It previews the original, checker and pink materials at each exposure from 0 to −8 EV in frozen 0.125 steps, eight frames each. The thresholds were declared before launch and were not changed. [Helper](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/EuropaTextureBench09444e.cpp:29>), [prediction](<D:/IA_Europa/Receipts/09444e/calibration-prediction.json>), [actual settings](<E:/IA_BuildCache/09444e-evidence/Calibration/01/settings.json>).

The measured pink-qualification intervals do not overlap:

| Target | Tested EV values passing full-region pink colour | Values passing all checks |
|---|---|---:|
| EA_HideCube | −3.0 through −3.625 | 0 |
| EA_BlinkCube | −3.0 through −3.625 | 0 |
| EA_LOD | −5.25 through −6.0 | 0 |

At −5.5 EV, all three baseline means are within 0.2–0.8 and have no saturated pixels, but the small cubes' pink means are approximately **127/21/127** and **128/21/128**, below R/B≥200. EA_LOD reads **234/59/220**, but checker contrast is **27.56**, below 40. At −6.375 EV its checker contrast reaches **41.84**, while its baseline falls to **0.19585**. At −3.625 EV the small cubes have qualifying pink but saturated baseline pixels; EA_LOD's pink G is approximately 173. These are measurements on the declared grid, not a claim about every possible scene arrangement. Examples: [checker at −3.625](<E:/IA_BuildCache/09444e-evidence/Calibration/01/preview-088/frame-000000.png>), [pink at −5.5](<E:/IA_BuildCache/09444e-evidence/Calibration/01/preview-134/frame-000000.png>).

| Required regression | Status |
|---|---|
| D3D11 1280×720 | Not run: pre-fire qualification failed |
| D3D12 1280×720 | Not run |
| D3D12 1279×719 | Not run |

There are no new scored event/mask counts, H4 results or texture G1–G5 results. No exposure was frozen for scoring. The whole-frame G3 IoU≥0.98 rule remains intact. Local defaults remain blinking, lod_corruption, missing_texture and corrupted_texture, **unqualified for push**. Camera_clipping was outside GJ5's scope and remains deferred.

Previously accepted evidence is unchanged: both materials retain their verified usage flags/constants and release graphs; GF4 skeletal/instanced IoU is 1.000 on 1,896 frames. Fixture checker fires static/skeletal 13/9 and pink 5/8, with G3 IoU 1.000; G4 has 0 new magenta on 2,352 paired frames; G5 has direct depth readings 1/1/1 and 35 residue-zero reverts. Accepted can-fails remain 365 unchanged mutant frames and early-label IoU 0. These were not rerun. [Accepted fixture evidence](<E:/IA_Fixtures/Outline425/Receipts/09444d/g4-g5.json>).

Neither texture implementation nor the private pass changed in this resume. Plugin Source/Shaders/descriptor are byte-identical to the compiled `8e59a56` state; the existing release `d7eee97` parity remains applicable. [Missing texture](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:72>), [pink texture](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp:72>). The exposure implementation follows 4.25's manual-method and bias handling: [method/bias](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/PostProcess/PostProcessEyeAdaptation.cpp:146>), [physical-camera branch](<E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/PostProcess/PostProcessEyeAdaptation.cpp:248>).

FM1 builds passed: **Editor attempt 2, 23.120 s; Game attempt 1, 42.645 s**, with two actions maximum and BelowNormal priority. Editor attempt 1 failed once with C4668 because the new helper read `UE_BUILD_SHIPPING` before including CoreMinimal; the include order was fixed. No fixture build or launch occurred. The new helper compiled directly without PCH; binary marker checks and all 27 artifact archive hashes pass. Scope/deadline/argv tests and the pixel reader's positive plus four negatives pass. [Builds and hashes](<D:/IA_Europa/Receipts/09444e/builds-and-archive.json>), [audit](<D:/IA_Europa/Receipts/09444e/build-audit.json>).

Full P9 passes in 386.877 s: 382,970 inventory files and 22,628 binary/config rehashes. Entry and final fast checks pass. **Crashes 0; upload signs 0; protected changes 0.** Content, Config, descriptors, the fixture, 5.1 materials, m51 and protected refs remain unchanged. Build outputs match the post-build armed snapshot. The only explicit deletions were three receipted Makefile.bin cache refreshes during the authorized builds. Final free space is **D: 53.91 GiB; E: 85.64 GiB**. [Exit](<D:/IA_Europa/Receipts/09444e/exit.json>).

Runtime settings included bloom 0, eye quality 2/method 3, exposure grid 0..−8, physical-camera exposure off, pitch −12, AA/motion blur/dynamic resolution 0, screen percentage 100, 30 FPS cap and the existing bench-only 0.5% selection floor. `r.CustomDepth` was read as 1, never set. Original material slots were restored by the helper before exit; all transient settings ended with the process. No settings or assets were saved. Evidence was placed under authorized E: scratch to retain **GJ4's unchanged 3 GiB D: reserve**. The default launcher is disabled; its build-only manifest records the rebuilt binaries. [Restoration receipt](<D:/IA_Europa/Receipts/09444e/kit-restore.json>).

The runtime exited at **11:39:49 IST**, the pixel reader finished **11:40:51**, and the PID/creation-time audit at **11:42:46** found all five owned process identities gone. The blackout deadlines were respected. Foreign activity was recorded and untouched. [Process audit](<D:/IA_Europa/Receipts/09444e/owned-processes-closed.json>).

Local commits on `feat/europa-port`: `cfe8e06` (bench helper), `ee56edc` (include fix), `cc8c2d5` (build binding/preflight), and **d33689f5b48ed7f73ea30b46e6713fa62a853918** (stop evidence, W4 status/journal and G539). **Pushed commits: none. Tags: none. Final worktree: clean.**

Executable/DLL archives: **B** = `E:/IA_BuildCache/09444e-evidence/binary-archive/`; **T** = `E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/`. All entries rehash equal; unchanged binaries are included. [Absolute inventory](<D:/IA_Europa/Receipts/09444e/exe-dll-inventory.json>).

| Archive | Relative path | SHA256 |
|---|---|---|
| B | Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| B | Binaries/Win64/StylizedParisStreet.exe | `98056a06aed14538fffd0f5dd26d2a029e95e6220c631626214e72d707b826d6` |
| B | Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll | `4d5fedfb93f330d4ae99619b6f498783d9ac16c63fec775de7e6a3dc58523981` |
| B | Binaries/Win64/UE4Editor-StylizedParisStreet.dll | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| B | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| B | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | `ea6dcbdaa7849dae68b850b9ca86240973eebce75810a77da5745d7d28f383ce` |
| B | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| B | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | `5bb246f0299f2409c62655005663972ee4900f99386b4c5f3b9eeea01cf84701` |
| B | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |
| T | UE4Editor.exe | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` |
| T | CrashReportClientEditor.exe | `ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7` |
| T | UnrealBuildTool.exe | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` |
| T | ShaderCompileWorker.exe | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` |

VERDICT: NEEDS-DECISION | EA_Core pre-fire qualification failed; all three scored regressions remain unrun.
