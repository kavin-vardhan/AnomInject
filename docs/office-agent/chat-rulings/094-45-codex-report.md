<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, danger-full-access resume, exit 0, 3,159 s, 15:15:19-16:07:58 IST). Body below is byte-identical to report.md, sha256 A7BD30AEC90402EC9EA385D84E147A39759AFF7881D800E7CF4151FF0F1E0BE3. -->
094-45 stopped at Stage 1 with **NEEDS-DECISION**. D3D11 completed 2,352 frames with zero drops and passed the earlier H4 checks, but texture G3 failed on 261 of 443 affected frames. GL2 also remains incomplete: the preflight qualified three cubes, while C5 selected the unqualified `EA_Floor`. Both D3D12 legs and Stage 2 (`camera_clipping`) are unrun. No `STAGE1-READY.txt` was written; no push or tag was made.

GL3 is implemented in [runtime_exit.py](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/runtime_exit.py:13). Nine tests pass, including empty-reason ownership, missing intent and stale PID. Replaying the original 094-44f receipt classifies it as owned-stop; its bytes remain unchanged.

GL2 now explicitly holds `EA_Camera` at (−550, 0, 480), pitch −12°, FOV 50, with automatic camera management disabled. A bench-only private-census probe measures 32 frames before each preview. The three cubes pass the same-session −5 EV qualification: drawn shares 8.5059%, 8.5059%, 12.8707%; minimum baseline means 143.866, 144.425, 99.807; zero saturated pixels; checker gaps 49.936, 48.880, 21.037; pink hue passes. However, the [helper’s actor list](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa/EuropaTextureBench09444e.cpp:146) omits `EA_Floor`, which draws 23.492% and receives seven checker and four pink events. Its required material qualification was not performed.

Both targets pass FM1 attempt 2, with zero compiler/link errors, BelowNormal priority and at most two actions. Attempt 1 also compiled successfully. Its first runtime probe stalled because it returned before setting `Prepared=Next`; zero frames, previews or events ran. A recorded intent followed by a PID/image/creation-time-matched kit close ended that setup normally. The [readiness correction](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyCaptureLabels425.cpp:112) was tested and rebuilt, then only the second session slot was used. The original incomplete FAIL receipt remains intact. This setup retry is explicitly recorded; no measured G1–G5/H4 failure was retried. Editor build times were 51.50/49.83 seconds; Game 65.22/62.84 seconds. Both 27-file archives rehash.

The accepted materials and skeletal/instanced fixture evidence remain unchanged. Checker SHA256 is `5222b3eeb7cdd1f5b58b5975d568b699c7430949b024c67e6f5b1ae6b88a7bec`; pink is `b88d88615286a6c9a3648a5c80ab3889ca8f6c648c378e8984ba3d01e79c552a`. The accepted readback covers six usage flags, opaque/two-sided/Default Lit, pink (1,0,1), and the release-line checker graph. The 5.1 assets are unchanged. Prior GF4 remains IoU 1.0 on 1,896 frames for skeletal and instanced targets. No anomaly implementation changed in this resume: the retained texture apply/revert paths are [missing_texture](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_MissingTexture.cpp:72) and [corrupted_texture](D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyInjector/Private/Anomalies/Anomaly_CorruptedTexture.cpp:72), ported from `d7eee97`. Camera clipping remains compiled out on 4.25.

The D3D11 1280×720 result is:

| Route | Events | Affected/masked frames |
|---|---:|---:|
| blinking | 15 | 90/90 |
| lod_corruption | 19 | 247/247 |
| missing_texture | 22 | 286/286 |
| corrupted_texture | 13 | 157/157 |

G1 and G2 pass: every texture fire changes its target. G3 fails: checker minimum IoU **0.406495** (221/286 frames fail), pink **0.813152** (40/157 fail). All 443 texture frames have zero changed pixels outside the mask. Up to 128,495 checker pixels and 40,453 pink pixels inside the mask change by no more than two RGB levels. These are measured silhouette disagreements; no further mechanism is claimed. All 70 edge frames are clean. Examples: [checker frame 743](E:/IA_BuildCache/09445-evidence/D3D11-1280/02/plugin-frames/epoch01/frame-000743.png), [pink frame 719](E:/IA_BuildCache/09445-evidence/D3D11-1280/02/plugin-frames/epoch01/frame-000719.png).

G4 retains the accepted fixture result: zero new magenta on 2,352 paired frames. G5 passes here: `r.CustomDepth` reads 1/1/1; all 14 recorded component states agree; enabled/tagged/polluted counts and host stencil writes are zero; all 35 reverts have residual zero. The accepted usage-flag negative (365 unchanged frames) and early-label reader negative (IoU 0) were not repeated.

Earlier H4 checks pass: all 780 affected frames have masks and clean frames none; schema 2.1 sets, coverage values and far plane agree; player view matches; per-frame pre-UI proof passes; census draws four objects at every pose with no exhaustion, lost batch or missing pass; `unavailable=0`. EA_Core has no skeletal target; the accepted fixture covers it. Every one of the 19 LOD events changes target pixels (per-event minima 69,404–69,409; maxima 69,413–69,415). Each event’s values are retained in the result summary. Wall-strip and >55-drawn checks are waived for EA_Core as ruled. D3D12 1280×720 and 1279×719 are **NOT RUN** after the failure.

Self-check lines, verbatim:

```text
Capture425 SELF_CHECK stage=start frames=1 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=5 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=4 ui_controls=4 ui_positive=4 ui_checked=1 ui_hits=0 ui_paired=1 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1211 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=bench_off batches=0 lost=0 no_pass=0 polluted=0
Capture425 SELF_CHECK stage=300 frames=300 size=1280x720 format=18 RHI=D3D11 PostRenderViewFamily=318 synth=0 viewport=SceneViewport client=GameViewportClient viewport_size=1280x720 backbuffer_format=18 slate=1 pie=0 pre_ui=1 ui_draws=8 ui_controls=8 ui_positive=8 ui_checked=300 ui_hits=0 ui_paired=8 tick=OnWorldPostActorTick425 tick_source=build_constant tick_type=2 tick_type_source=build_constant tick_calls=1524 fixed_step=1 fixed_dt=0.033333333 fixed_rate=0 fixed_fps=30.000000 smoothing=0 census=on batches=0 lost=0 no_pass=0 polluted=0
```

Runtime-only bench settings were bloom 0, eye-adaptation quality 2/method 3, exposure −5, physical-camera exposure off, the held camera, 0.5% bench selection floor and seed 4242. Preview material slots were restored before C5; capture timing and injector settings restored at Stop. Process-only view/post-process settings expired with the closed session; nothing was saved. The shipped selection floor remains 6%. Defaults remain blinking, lod_corruption, missing_texture and corrupted_texture on the local branch; their push is withheld. Camera clipping is not in the auto pool and Stage 2 did not start.

C: before/after was 20.4616→20.4682 GiB for setup and 20.4528→20.4486 GiB for the scored session. Neither drop reached 1 GiB. Cache-size readings are retained. Exit free space is C: 20.43, D: 53.64, E: 83.24 GiB. The 3 GiB launch reserve was retained. Entry full P9, exit fast checks, protected refs/assets and archive rehash pass. Runtime crashes, upload signs and protected changes: **none**. All owned runtime processes are closed. No machine settings changed. No cleanup deletion occurred; build-tool removals are recorded in the build receipts.

Evidence and readers:

- Raw sessions: `D:/IA_Europa/Receipts/09445/H4/D3D11-1280/01` and `/02`; captures: `E:/IA_BuildCache/09445-evidence/D3D11-1280/01` and `/02`.
- [Gate details, each LOD event and self-checks](D:/IA_Europa/Receipts/09445/result-summary.json), [full reader result](D:/IA_Europa/Receipts/09445/H4/D3D11-1280/02/reader.json), [target qualification audit](D:/IA_Europa/Receipts/09445/selected-target-audit.json), [floor pixels](D:/IA_Europa/Receipts/09445/floor-audit.json).
- Readers in `D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/`: `geometry_reader09445.py`, `calibration_reader09445.py`, `host_reader09445.py`, `result_summary09445.py`, `floor_audit09445.py`; the first three are also committed in `tools/europa`.
- [Complete artifact hashes and archive paths](D:/IA_Europa/Receipts/09445/exe-dll-inventory.json), [final exit audit](D:/IA_Europa/Receipts/09445/exit.json). The runtime launcher is disabled and rebound to the current build.

Executable/DLL SHA256 values follow. Archive roots: **A1** = `E:/IA_BuildCache/09445-evidence/binary-archive`, **A2** = `E:/IA_BuildCache/09445-evidence/binary-archive-02`, **M** = `E:/IA_Fixtures/Outline425/Receipts/09444/executable-archive`. Shipping and unchanged engine artifacts were retained, not rebuilt.

| Archive root / relative path | SHA256 |
|---|---|
| A1,A2 / `Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe` | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| A1 / `Binaries/Win64/StylizedParisStreet.exe` | `ea6ed0823052223a969f1bab356190bbf95da25e2b3f0719bee86ee2ce8d6ac3` |
| A1,A2 / `Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll` | `0d61f309bfd916c35689d014cbcdf2a652f97aa8cd7e12c3ca4da67c4f8df03d` |
| A1,A2 / `Binaries/Win64/UE4Editor-StylizedParisStreet.dll` | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| A1,A2 / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll` | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| A1 / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `b216591a86d6c4b0a30067124087e0aaa5512ffeaeec970460d1cc244cc4b18b` |
| A1,A2 / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll` | `0b0dc378b5fa47f65eff3246eebd7c03d4b4bcf9bce4515f993ec9f253bc772b` |
| A1,A2 / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll` | `5bb246f0299f2409c62655005663972ee4900f99386b4c5f3b9eeea01cf84701` |
| A1,A2 / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll` | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |
| A2 / `Binaries/Win64/StylizedParisStreet.exe` | `6a1e0a4c9056d004cf8bc47eecd7b8d47dc7ff054cd7061d558a09f52a0cec4a` |
| A2 / `Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `0376ba82c8edf89cd58a28e539eb0cda06f8ccf928eef8ccf5425e55199682c6` |
| M / `UE4Editor.exe` | `4a6d83264e68403534aa917e92f741e0d09add5e8cf20c19b76d2cfe4558c387` |
| M / `CrashReportClientEditor.exe` | `ccd60dfe533f605b1ad8c8d550121be3eda3073b846f3e0df51a478a17a380e7` |
| M / `UnrealBuildTool.exe` | `07c1a6fbc30271b96679253143eb620c49db5b2919a1cbb7430e8fc6bdab9bd0` |
| M / `ShaderCompileWorker.exe` | `6805ce2c9cea663cd356cfc463b470390f227b08ef05e80a12d400363cdf60e6` |

Local commits this job: `96d8f01`, `93e7435`, `f82a5c8`, `422ac34`, `ea3ea68`, `2d93e67`. Final commit: **`2d93e6763dc3ddc606d5ec8a36cceca38edc38a9`** on `feat/europa-port`, clean. Commits pushed by Codex: **none**. Stage 1 handshake and Stage 2 remain unrun.

VERDICT: NEEDS-DECISION | D3D11 texture G3 failed and EA_Floor was not pre-qualified; D3D12 and camera_clipping remain unrun.
