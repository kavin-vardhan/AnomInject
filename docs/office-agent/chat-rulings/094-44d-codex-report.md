<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, gpt-6-astra, max, danger-full-access resume, exit 0, 2,472 s, 10:09:02-10:50:14 IST). Body below is byte-identical to report.md, sha256 75199CC42C09813906B8843762D83F1463A965E988372A8B9816F7A5898FBD0E. -->

094-44d ends **NEEDS-DECISION**. Fixture G4 and G5 pass. The first host regression, D3D11 1280×720, fails texture G3 on all 443 affected frames. Its minimum whole-frame IoU is **0.095590 checker / 0.087897 pink**, below 0.98. The failed-gate stop rule was applied: no D3D12 launch or retuning. [Measurements](<D:/IA_Europa/Receipts/09444d/stop-result.json>).

The outline-on fixture run reused the frozen binaries, exposure −2.1875, bloom off, manual exposure, six shadowless fills and FOV 30. Its setup and pose receipts equal the kept outline-off run. Both use seed 4242 and the four default routes. **All 2,352 frames pair; new magenta is 0 on every frame; drops 0.**

| Texture/target | Fires | Affected frames | New magenta |
|---|---:|---:|---:|
| Checker/static | 13 | 169 | 0 |
| Checker/skeletal | 9 | 117 | 0 |
| Pink/static | 5 | 65 | 0 |
| Pink/skeletal | 8 | 92 | 0 |

G5 now has direct in-process `r.CustomDepth=1` readings at startup and saved indices 81/2300, engine frames 2140/5968/8212. All 28 component states match before/after; enabled, tagged, unexpected, polluted and host-stencil-write counts remain 0. The Python observer only reads; it sends no window input. Python 2.7, keyword-Rotator, missing/changed-reading negatives, scoped-policy and deadline tests pass. [G4/G5 evidence](<E:/IA_Fixtures/Outline425/Receipts/09444d/g4-g5.json>), [direct readings](<E:/IA_Fixtures/Outline425/Receipts/09444d/textures/texture-on/01/depth-readings.jsonl>), [observer](<E:/IA_Fixtures/Outline425/Scripts/capture_depth_read09444d.py>).

GG1/GI1's accepted materials, readback and G1–G3 remain applicable. Checker SHA256 `5222b3eeb7cdd1f5b58b5975d568b699c7430949b024c67e6f5b1ae6b88a7bec`; pink `b88d88615286a6c9a3648a5c80ab3889ca8f6c648c378e8984ba3d01e79c552a`. Six usage flags, no Nanite, two-sided/opaque/Default Lit, pink (1,0,1), and the release checker graph are unchanged. The accepted fixture G3 has IoU 1.000, all 70 timing edges clean and 35 residue-zero reverts. GF4's skeletal/instanced proof remains IoU 1.000 on 1,896 frames. Accepted negatives remain: usage mutant 0 changed pixels on 365 frames; 35 early-label mutants IoU 0. Examples: [checker](<E:/IA_Fixtures/Outline425/Receipts/09444d/textures/texture-on/01/plugin-frames/epoch01/frame-000743.png>), [pink](<E:/IA_Fixtures/Outline425/Receipts/09444d/textures/texture-on/01/plugin-frames/epoch01/frame-000719.png>).

No native implementation, material or shader changed in this resume. Both texture ports retain `d7eee97` behaviour, with only the compat include and override-material pointer access differing. [Missing texture](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:72>), [corrupted texture](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp:72>), [compat helper](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Compat/AnomalyEngineCompat.cpp:19>), [fresh per-frame proxy](<D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyPrivatePass425.cpp:367>).

The host D3D11 `-game` session exited normally with 2,352 frames and 0 drops:

| Route | Fires | Affected/masked frames | Result |
|---|---:|---:|---|
| blinking | 15 | 90 | H4 passes |
| lod_corruption | 19 | 247 | Every event changes 14,455–14,475 target pixels |
| missing_texture | 22 | 286 | G1/G2 pass; G3 IoU 0.095590–0.183899 |
| corrupted_texture | 13 | 157 | G1/G2 pass; G3 IoU 0.087897–0.134274 |

Every affected frame has a mask; all 1,572 clean frames have none. Schema 2.1, affected/injected sets, far plane, all 69 coverage values, measured self-checks, framewise pre-UI proof and player view pass. Census peak 4, exhaustion/lost/no-pass/polluted 0; unavailable 0 across five verified static components. All 14 component states restore exactly, host stencil writes 0, and all three actual host `r.CustomDepth` replies are 1. EA_Core has no skeletal actor. [Host reader](<D:/IA_Europa/Receipts/09444d/H4/D3D11-1280/01/reader.json>).

The G3 failure is measurable outside the target: checker changes 20,127–234,868 outside-mask pixels; pink 109,936–263,454. Some saturated inside-mask pixels also remain unchanged. Every fire changes target pixels, all 70 edges are clean, and all 35 reverts report residual 0. Saved frames show a saturated scene with visible halos; no new diagnostic run isolated the rendering effect. The whole-frame 0.98 bar, mask scope and host settings were retained. [Before](<D:/IA_Europa/Receipts/09444d/H4/D3D11-1280/01/plugin-frames/epoch01/frame-000718.png>), [pink failure](<D:/IA_Europa/Receipts/09444d/H4/D3D11-1280/01/plugin-frames/epoch01/frame-000719.png>), [checker failure](<D:/IA_Europa/Receipts/09444d/H4/D3D11-1280/01/plugin-frames/epoch01/frame-000743.png>).

D3D12 1280×720 and 1279×719 are **unrun**. Local defaults remain blinking, lod_corruption, missing_texture and corrupted_texture, but are **not qualified for push**. Origin remains `8df75de` and its earlier defaults/card remain valid. Camera_clipping is excluded by GI7 and deferred to 094-45.

No native builds ran: installed targets' Source/Shaders/descriptor match compiled `8e59a56`; GI6's frozen fixture binaries were retained. All 27 bound artifacts and 20 executable/DLL archive entries rehash equal. Prior strict-build/string/lever evidence remains applicable by byte equality. Full P9 entry passes in 393.94 s; fast entry/exit pass. Final free space: D: 54.09 GiB, E: 86.72 GiB. **Crashes 0; upload signs 0; protected changes 0; no explicit deletions.** m51, protected refs, spikes and 5.1 assets are unchanged. [Exit](<D:/IA_Europa/Receipts/09444d/exit.json>).

Runtime-only settings ended at Stop/process exit: fixture exposure −2.1875, bloom/AA/motion/dynamic-resolution 0, screen 100, eye quality 2/method 3, physical-camera exposure off, point light 0, six shadowless fills at 3, FOV 30/pitch −12, distance 1300, offsets ±125, skeletal/static scales 5/1.3, baseline material assignment and outline blendable on. Host used its existing presentation, eye quality 0, AA/motion/dynamic-resolution 0 and screen 100. Both used seed 4242 once, 0.8 s interval, 0.4 s hold, one concurrent event, fixed 1/30 s, pace 1, and the bench-only 0.5% floor; ceiling 25% and shipped 6% floor are unchanged. The staged default launcher/manifest was restored by receipt. No Config or machine setting was saved.

Recorded adjustment: before any host launch, its capacity reserve was set to 2 GiB instead of the inherited 3 GiB, against a kept maximum 1.470 GiB per session; the 50 GiB floor stayed enforced. No product gate changed. Fixture launches required `-BENCHMARK -nowrite`; host stayed `-game -nowrite`. The last runtime root exited 10:32:53 IST; the read-only PID/creation-time audit at 10:41:10 found 0 live owned processes. The host gate reader finished at 10:37:43 IST. The 13:10 launch and 13:35 closure deadlines were respected. Foreign processes were recorded and untouched. [Restoration](<D:/IA_Europa/Receipts/09444d/kit-restore.json>), [owned-process audit](<D:/IA_Europa/Receipts/09444d/owned-processes-closed.json>).

Local commits on `feat/europa-port`:

- `fa207fb4cb6acab76c313a70a10e5239b50dc278` — direct fixture readings and blackout checks.
- `035ec20c58f7ed477af6a693be6381cff52f4e93` — passing G4/G5 evidence and host kit.
- `4cc8b4596313b5438b08e8cc6fb7e99920be85d3` — G3 stop evidence, W4 status/journal and G538.

**Commits pushed: none. Tags: none. Final worktree: clean.**

Executable/DLL hashes and archives follow. **F** = `E:/IA_Fixtures/Outline425/Receipts/09444c/uniform-binary-archive/`; **H** = `E:/IA_Fixtures/Outline425/Receipts/09444r/texture-host-binary-archive/`; **T** = `E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/`. Paths are relative to these roots. [Absolute inventory and rehash](<D:/IA_Europa/Receipts/09444d/exe-dll-inventory.json>).

| Archive | Relative path | SHA256 |
|---|---|---|
| F | Binaries/Win64/Outline425.exe | `e3aac41dbd05b3c1d9cb31f89a13e8acf6dbbbfb6623f506de0a64fe0e9a68a1` |
| F | Binaries/Win64/UE4Editor-Outline425.dll | `67614a21922a9516f56504fbead6a4c0ff1a10bf6f2ec83101671d767c7c9032` |
| F | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | `0a74f35993eddb1406901e4b240360d67c5239cfc5a80ac0d6bcd38517b25e12` |
| F | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | `eb9900526b37c559634be99b6b848b77218b34dd97aa296835c31ee45f3a9602` |
| F | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | `b84088b69fe7b66adcd79086941bd8374cbb9beebdc63f959113fb24fdc13b40` |
| F | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | `0ead5c69edabc96a301dc3059cca824696a9736799ab2df10e2446ffdbdd2eb7` |
| F | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | `9ae9b60976505af41e9cdb547380954610bc10ef687ee71083f2240265f578c4` |
| H | Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| H | Binaries/Win64/StylizedParisStreet.exe | `46d4946a5df34bbd700d09ca009d4c999f4d292f617a209a7c9ba86e2b737eee` |
| H | Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll | `6037924a08166344146f337b2f0b19ba35a5daf3dc99715db15ef6c332864a89` |
| H | Binaries/Win64/UE4Editor-StylizedParisStreet.dll | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| H | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| H | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll | `ea6dcbdaa7849dae68b850b9ca86240973eebce75810a77da5745d7d28f383ce` |
| H | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| H | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll | `5bb246f0299f2409c62655005663972ee4900f99386b4c5f3b9eeea01cf84701` |
| H | Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |
| T | UE4Editor.exe | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` |
| T | CrashReportClientEditor.exe | `ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7` |
| T | UnrealBuildTool.exe | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` |
| T | ShaderCompileWorker.exe | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` |

VERDICT: NEEDS-DECISION | Fixture G4/G5 pass; host D3D11 texture G3 fails; D3D12 and push remain pending.
