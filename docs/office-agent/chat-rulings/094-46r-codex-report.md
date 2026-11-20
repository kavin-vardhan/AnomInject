<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, danger-full-access resume, exit 0, 6,503 s, 09:37:15-11:25:39 IST). Body below is byte-identical to report.md, sha256 3B10C93E9A182D854467D03314EE471CFDA80380BF1AFADF7BA7A5964CF9413A. -->
094-46r stopped with **NEEDS-DECISION**. D3D11 D4 passes under GO2. D3D12 D4 fails on **6 of 2,496 unlabelled frames**: each has **255 pixels at a 2-level difference**, exceeding its sealed **240-pixel** limit. No pixel exceeds the 2-level amplitude limit. Further launches stopped; both odd-width legs and GO3 remain unrun.

GO2's `IAICameraFarOnly` switch is implemented in [AnomalyCameraSession425.cpp](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyCameraSession425.cpp:19). It requires non-Shipping, `-IAIBench`, EA_Core, the requested camera route, `-IAICameraSlabBench` and `-IAICameraRegression`. It holds the far view for the complete 2,352-frame default section and omits the near-wall tail. Both new far-only sessions label **0** camera frames. The ordinary 2,640-frame combined schedule, anomaly implementation, slab algorithm and GN4 counter remain unchanged.

Each near-plane pair uses all 2,352 matching indices against control A's default-section prefix. D3D11 reuses the accepted 094-46 control and positive; its new floor binary differs only by the guarded schedule change and additive `far_only` receipt. All new D3D12 sessions use identical binaries. No accepted D3D11 session was rerun.

| RHI | Session floor: delta / count | Near-plane floor: delta / count | D4 unlabelled failures |
|---|---:|---:|---:|
| D3D11 1280×720 | 1 / 560,155 | 2 / 255 | 0 / 2,496 |
| D3D12 1280×720 | 1 / 559,838 | 2 / 240 | 6 / 2,496 |

The [reader](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/camera_reader09446r.py:87) records every frame's full 256-bin distribution and counts above each level. The selected count belongs to the larger delta; the lower-level count is never transferred. D3D11's near-plane floor was sealed at **04:37:58 UTC**, before re-comparison at **04:40:23 UTC**. D3D12's near-plane floor was sealed at **05:28:23 UTC**, before the positive started at **05:29:42 UTC**. Both floor files and their hashes are preserved in the [receipt index](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/docs/europa/09446r-receipts.json).

D3D11's maximum unlabelled count is **254**, within 255. D3D12 fails at frames **1156, 1163, 1996, 2003, 2087 and 2094**, each at count 255. Both positives label exactly **2352–2399, 2448–2495 and 2544–2591**: 144 labelled frames, each changing all **921,600 pixels** above both floors. Near 100 restores to 10 at Stop; baseline controls label zero. The shifted-label can-fail detects six boundary mismatches on each RHI. See [D3D11 rejudgment](D:/IA_Europa/Receipts/09446r/D3D11-rejudged-full.json), [D3D12 paired pixels](D:/IA_Europa/Receipts/09446r/H4/D3D12-1280-positive/01/paired-pixels.json) and [the six failures](D:/IA_Europa/Receipts/09446r/stop.json). Opened example: D3D12 frame 2352 [control](E:/IA_BuildCache/09446r-evidence/D3D12-1280-control-a/01/plugin-frames/epoch01/frame-002352.png) / [positive](E:/IA_BuildCache/09446r-evidence/D3D12-1280-positive/01/plugin-frames/epoch01/frame-002352.png).

All five new sessions pass **H4, strict texture G1–G3 and G5**: 0 drops, 780 object-mask frames, 22 checker and 13 pink fires, whole-frame IoU **1.000** on 286 checker and 157 pink frames, 0 spill, 140 clean edge frames and 35 residue-free reverts per session. Every LOD event changes target pixels; the minimum event peak is 68,061 pixels. `unavailable=0`, `polluted=0`, enabled/tagged custom-depth components 0, stencil writes 0, and `r.CustomDepth=1/1/1`. GN4's counter is 0 and agrees with annotation entry counts. No explained-pixel fallback was used. EA_Core has no skeletal candidate; accepted fixture skeletal and material proofs remain unchanged.

| New session | Frames | C: before → after, GiB |
|---|---:|---:|
| D3D11 near-plane floor | 2,352 | 23.662 → 23.660 |
| D3D12 control A | 2,640 | 23.272 → 23.270 |
| D3D12 control B | 2,640 | 23.252 → 23.252 |
| D3D12 near-plane floor | 2,352 | 23.226 → 23.225 |
| D3D12 positive | 2,640 | 23.228 → 23.229 |

**Unrun:** D3D12 1279×719 combined, D3D12 1279×719 default-only, GO3's output-key/type comparison and office-card read commands. Defaults remain `blinking`, `lod_corruption`, `missing_texture`, `corrupted_texture`; `camera_clipping` remains opt-in. No threshold was retuned and no measured gate was retried.

Entry/exit FJ1 checks pass. Full P9 passed in **352.716 seconds**, covering 382,970 inventory files and 22,628 rehashed files. Normal Editor and Game builds each pass **attempt 1**, in **52.624 / 66.890 seconds**, with 0 link errors. Focused scope/floor/schema tests and their can-fails pass, including rejection of an inflated lower-level count. Source/comment/lever audits, binary-marker checks and archive rehash pass. A separate strict-include rebuild was not run in this job; no further build was started after the D4 stop. The accepted 154-check slab self-test is unchanged and was not repeated. The existing Shipping binary is retained; no Shipping rebuild is claimed.

Bench settings were runtime-only: `-game`, `-nowrite`, `-IAIBench`, `-IAICaptureFallback`, `-IAICameraSlabBench`, `-IAICameraRegression`; controls additionally used `-IAICameraBaseline`, and the two floor sessions used `-IAICameraFarOnly`. The frozen bench retained the blue base (0.05,0.2,0.5), EA_Light 5000→0, six shadowless fills at intensity 3, bloom 5→0, eye quality 0→2, method −1→3, exposure 0→−1.875, physical-camera exposure off, AA/motion blur off, screen percentage 100, dynamic resolution off and pacing 30. Far view: (−550,0,480), pitch −12, FOV 50; near view: (600,0,160). The 0.5% bench selection floor and pacing restoration were receipted. Near-plane restoration occurred at Stop; temporary scene/view/exposure changes ended with process exit. No assets or config were saved.

**Safety:** 0 runtime crashes, 0 upload signs, 0 protected changes; all owned runtime/build processes ended and the launcher is disabled. Foreign activity was recorded and untouched. No file was deleted; no security, paging, power or machine setting changed. No session lost 1 GiB on C:. Final free space: **C: 23.227, D: 53.012, E: 54.894 GiB**. Protected refs, fixture assets and both material generations passed [exit comparison](D:/IA_Europa/Receipts/09446r/exit.json).

Implementation choices/deviations: the complete default-section prefix provides the far-only pair without redundant baseline launches; the D3D11 binary difference is disclosed above. Completed text receipts created by this job were losslessly compressed, with hashes and mtimes unchanged, to retain the 3 GiB launch reserve. One premature kit invocation occurred before arming completed; it launched no engine and consumed no runtime attempt, then was invoked after ARMED. These are recorded in the receipt index. The failed count gate was preserved unchanged.

Evidence roots are **D:/IA_Europa/Receipts/09446r** and **E:/IA_BuildCache/09446r-evidence**. Each leg has `H4/<leg>/01/result.json`, `reader.json`, `raw-evidence.json` and C: receipts; frames/previews/settings are under `<leg>/01` in the evidence root. Accepted D3D11 evidence remains in the corresponding `09446` roots. [result-summary.json](D:/IA_Europa/Receipts/09446r/result-summary.json) and the receipt index list the readers and exact files.

Local commits on `feat/europa-port`:

- `cd2d60c8889e589bba118655ebff2d8eb7f2420f` — guarded far-only schedule and tests.
- `15cea3826e1c700d7719a56196ebe1377c123e71` — kit/readers; all five new sessions ran at this tip.
- **`886e1a0a28d2521559b4122ac0610e7be803d665`** — final W4 evidence; worktree clean.

**Commits pushed: none. Tags: none.** Origin remains `c5d0919e5074404d26da8c16ccf845b360bde383`, confirmed by `ls-remote`. The final commit changes documentation only.

Every EXE/DLL hash and archive path is below. “Current” is this job's archive; “reused” is the accepted D3D11 archive. Matching hashes share a row. The [full inventory](D:/IA_Europa/Receipts/09446r/exe-dll-inventory.json) includes companion build files, and the [reused archive rehash](D:/IA_Europa/Receipts/09446r/reused-binary-rehash.json) verifies the accepted binaries.

| Archived executable or DLL | SHA-256 |
|---|---|
| [Current Home-Shipping.exe](E:/IA_BuildCache/09446r-evidence/binary-archive/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe); [Reused Home-Shipping.exe](E:/IA_BuildCache/09446-evidence/binary-archive/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe) | 6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861 |
| [Current Home.exe](E:/IA_BuildCache/09446r-evidence/binary-archive/Binaries/Win64/StylizedParisStreet.exe) | bb1a650c7eba9cfe2fd7d69e5df4f98e75e06519d0b32416b8ea8a1708b2b8a8 |
| [Current UE4Editor-EuropaPhaseAProbe.dll](E:/IA_BuildCache/09446r-evidence/binary-archive/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll); [Reused UE4Editor-EuropaPhaseAProbe.dll](E:/IA_BuildCache/09446-evidence/binary-archive/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll) | d2d14a80460102591c01ed4c7a5a6bdbf475d3ab97dcc9c14cb4e77bc3aa9e04 |
| [Current UE4Editor-Home.dll](E:/IA_BuildCache/09446r-evidence/binary-archive/Binaries/Win64/UE4Editor-StylizedParisStreet.dll); [Reused UE4Editor-Home.dll](E:/IA_BuildCache/09446-evidence/binary-archive/Binaries/Win64/UE4Editor-StylizedParisStreet.dll) | 8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c |
| [Current UE4Editor-AnomalyBench.dll](E:/IA_BuildCache/09446r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll); [Reused UE4Editor-AnomalyBench.dll](E:/IA_BuildCache/09446-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll) | 2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940 |
| [Current UE4Editor-AnomalyCapture.dll](E:/IA_BuildCache/09446r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll) | dd44fd0be5248725c8433aa8239c5c5d1fd13b52db30b27f32497a02b562a40a |
| [Current UE4Editor-AnomalyControlServer.dll](E:/IA_BuildCache/09446r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll); [Reused UE4Editor-AnomalyControlServer.dll](E:/IA_BuildCache/09446-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll) | 0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b |
| [Current UE4Editor-AnomalyInjector.dll](E:/IA_BuildCache/09446r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll); [Reused UE4Editor-AnomalyInjector.dll](E:/IA_BuildCache/09446-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll) | 00b2e8f49f0f5394eab3b8c027ec908d7a0af9efbc574e625ffb905b2e6a256d |
| [Current UE4Editor-AnomalyShaders.dll](E:/IA_BuildCache/09446r-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll); [Reused UE4Editor-AnomalyShaders.dll](E:/IA_BuildCache/09446-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll) | 3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb |
| [Current UE4Editor.exe](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/UE4Editor.exe) | 4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387 |
| [Current CrashReportClientEditor.exe](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/CrashReportClientEditor.exe) | ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7 |
| [Current UnrealBuildTool.exe](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/UnrealBuildTool.exe) | 07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0 |
| [Current ShaderCompileWorker.exe](E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive/ShaderCompileWorker.exe) | 6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6 |
| [Reused Home.exe](E:/IA_BuildCache/09446-evidence/binary-archive/Binaries/Win64/StylizedParisStreet.exe) | b3dd0a5ee29e55a788be1cd42265a7b17d90b43ba2ce20b845e730fba4ee8333 |
| [Reused UE4Editor-AnomalyCapture.dll](E:/IA_BuildCache/09446-evidence/binary-archive/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll) | 9c5a4846cdc17435168fe0b26344982e36431953c8c647a3e3c00f0ae17ba6e6 |

VERDICT: NEEDS-DECISION | D3D12 exceeded its sealed pixel-count floor; odd-width qualification and GO3 remain unrun.
