<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, danger-full-access resume, exit 0, 1,621 s, 12:05:34-12:32:35 IST). Body below is byte-identical to report.md, sha256 901D3DF91F97ED0EAB7246AC47AE7C3CF60EA68381020134BC5859CBE1CF6C5F. -->
094-44f ends **NEEDS-DECISION**. GK2 selects **-5.0 EV** from the accepted sweep, but the fresh D3D11 preflight used a different camera view. All three target centres projected outside the capture, so the kit stopped before C5. No scored regression passed and nothing was pushed. [Stop evidence](<D:/IA_Europa/Receipts/09444f/preflight-stop-diagnostic.json>).

The retained 65-exposure sweep was re-read under GK2 without changing its raw files. All 40 brighter tested values fail; -5.0 EV is the brightest common passing value on the 0.125-EV grid. Bloom is off, every untouched target has zero channel-255 pixels, and all baseline, hue and checker checks pass:

| Target | Baseline mean (0-255) | Minimum pink R/G/B means | Checker contrast |
|---|---:|---|---:|
| EA_HideCube | 151.89 | 147.73 /27.80 /147.86 | 50.65 |
| EA_BlinkCube | 152.44 | 148.31 /27.81 /148.40 | 49.73 |
| EA_LOD | 101.56 | 245.91 /90.63 /231.97 | 21.09 |

These are calibration measurements, not scored anomaly results. [Frozen measurements](<D:/IA_Europa/Receipts/09444f/freeze.json>), [complete exposure table](<D:/IA_Europa/Receipts/09444f/calibration-result.json>), [checker preview](<E:/IA_BuildCache/09444e-evidence/Calibration/01/preview-121/frame-000000.png>), [pink preview](<E:/IA_BuildCache/09444e-evidence/Calibration/01/preview-122/frame-000000.png>).

The armed D3D11 launch wrote **3 previews, 24 frames, zero drops**. Its target centres were **(2556.38,637.99), (2254.40,1437.80), (3018.92,1272.43)** in a 1280x720 capture. Every checker sampling patch contained **0 pixels**; the reader assertion at line 52 therefore refused approval. The accepted sweep's centres were (502.02,435.56), (777.98,435.56), (640,383.87). The helper sets control rotation once but does not verify the complete camera pose before previews. The cause of the changed view is **not established**. [Failed preview](<E:/IA_BuildCache/09444f-evidence/D3D11-1280/01/preview-000/frame-000000.png>), [reader](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/calibration_reader09444f.py:52>), [helper](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/EuropaTextureBench09444e.cpp:69>).

| Required leg | Result |
|---|---|
| D3D11 1280x720 | Fresh preflight failed; C5 not started |
| D3D12 1280x720 | Not run after the stop |
| D3D12 1279x719 | Not run after the stop |

There were **0 auto-injector fires and no C5 masks or annotations**. H4 and texture G1-G3/G5 remain unmeasured in this resume. No failed result was retuned or overwritten; all 73 sealed evidence files rehash equal. The whole-frame G3 threshold remains IoU>=0.98 with exact clean edges. Local defaults remain blinking, lod_corruption, missing_texture and corrupted_texture, pending their host gates. Camera_clipping was outside GK3's scope.

Previously accepted work remains unchanged: both 4.25 materials retain their verified graphs/constants/flags; GF4 skeletal/instanced coverage reached IoU 1.000 on 1,896 frames. Fixture checker fires static/skeletal 13/9 and pink 5/8, with G3 IoU 1.000; G4 has 0 new magenta on 2,352 paired frames; G5 reads depth 1/1/1 with 35 residue-zero reverts. Accepted can-fails remain 365 unchanged mutant frames and early-label IoU 0. These gates were not rerun. [Fixture evidence](<E:/IA_Fixtures/Outline425/Receipts/09444d/g4-g5.json>).

**No native build ran.** Plugin Source, Shaders, Content and descriptor remain unchanged from entry; the accepted host helper and both target builds were reused. Thus the existing behaviour parity against release `d7eee97` is unchanged. GK2 tests pass 2 positives/6 negatives; capture-log segmentation passes 1 positive/4 negatives. Scope, memory, deadline and four argv cases pass. Comment stripping changes 0 of 408 files; diff checks pass. [Tests](<D:/IA_Europa/Receipts/09444f/reader-tests.json>), [build archive](<D:/IA_Europa/Receipts/09444e/builds-and-archive.json>).

**Crash accounting discrepancy:** the immutable kit result reports **1 crash**. It recorded a stop intent for running PID 23160 at 06:48:19.631UTC, then terminated the root at 06:48:19.829UTC. The reason was an empty `AssertionError` string; `classify_exit` rejects an empty reason and labels the exit a crash. An in-memory diagnostic changing only that reason to `AssertionError` yields `owned-stop`. There is no new crash-report folder. The raw count remains 1; neither the classifier nor the receipt was changed. [Original result](<D:/IA_Europa/Receipts/09444f/H4/D3D11-1280/01/result.json>), [classifier](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/runtime_exit.py:16>).

Full P9 passes in 358.797 s: 382,970 inventory files and 22,628 binary/config rehashes. Final fast checks, all 27 bound artifacts and archive rehashes pass. **Upload signs 0; protected changes 0; deletions 0.** Protected refs, fixture files and both 5.1 materials remain unchanged. Final free space: **D: 53.80 GiB; E: 85.63 GiB**. [Exit receipt](<D:/IA_Europa/Receipts/09444f/exit.json>).

Runtime settings were bloom 0, eye quality 2/method 3, exposure -5, physical-camera exposure off, AA/motion blur/dynamic resolution 0, screen percentage 100, 30 FPS cap and bench coverage floor 0.5%. The requested pitch was -12; the actual view mismatch is recorded above. `r.CustomDepth` was read as 1, never set. The helper restores original material slots before writing prefire.json; transient settings ended with process termination. No asset or Config was saved. The default launcher was restored to its disabled build-only state, with a receipt. [Settings](<E:/IA_BuildCache/09444f-evidence/D3D11-1280/01/settings.json>), [restoration](<D:/IA_Europa/Receipts/09444f/kit-restore.json>).

The root ended at **12:18:19 IST**; the identity audit at **12:24:16 IST** found **0 active owned processes**. All launches and pixel analysis finished before the blackout limits. Foreign activity was recorded and untouched. [Process audit](<D:/IA_Europa/Receipts/09444f/owned-processes-closed.json>).

Local commits on `feat/europa-port`: `786a1682b5cc183ada372c45941802e8d3554888` (GK2 reader/kit and tests), and **983f429961b258270c8878c3d1161d1dae057452** (W4 stop evidence, status/journal and G540-G541). **Pushed commits: none; tags: none; worktree: clean.** The regression shortfall is explicit; no further launch followed the failed preflight.

Executable/DLL archives are reused and rehashed: **B**=`E:/IA_BuildCache/09444e-evidence/binary-archive/`; **T**=`E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/`. [Full absolute inventory](<D:/IA_Europa/Receipts/09444f/exe-dll-inventory.json>).

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

VERDICT: NEEDS-DECISION | GK2 calibration passes on retained evidence; the fresh-view preflight failed and all three scored regressions remain unrun.
