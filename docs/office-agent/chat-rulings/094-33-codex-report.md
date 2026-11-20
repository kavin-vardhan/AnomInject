<!-- collected by Claude Code (mailbox brief 094-33) from D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\report.md, 5581 B, sha256 B7B82A807BF63CD8562DF048A01972F657E69936FAD534F56DEDF2BE364B394C, at 2026-11-01T03:02:47.230524+05:30 -->
FO2 passed: 4.25 frames are UI-free, and both C1 sessions and C2 passed again. C3 compiled, but its first mask reduction stalled; the kit stopped its owned tree at the session limit. No C3 mask metrics were produced.

Colour capture now uses `PostRenderViewFamily_RenderThread`. Stock 4.25 invokes it at [SceneRendering.cpp:3040](/E:/EpicGames/UE_4.25/Engine/Source/Runtime/Renderer/Private/SceneRendering.cpp:3040); viewport canvas/debug/UI follows the queued scene render at [GameViewportClient.cpp:1525](/E:/EpicGames/UE_4.25/Engine/Source/Runtime/Engine/Private/GameViewportClient.cpp:1525). The 5.1 path stays guarded. No heartbeat suppression or backbuffer-format override was used.

The frozen heartbeat-region check found **zero heartbeat pixels in all 612 C1/C2 images**. Kept controls contained 1,186–1,200 and failed as required. Focused hook/reader/argv and keyword-Rotator checks passed. No in-editor Python script changed. [FO2 tests](/D:/IA_Europa/Receipts/09433/tests/fo2.json).

| FO2 gate | Result |
|---|---|
| C1 PIE, EA_Core | PASS: 60 frames, indices 0–59, capture 64.135 s |
| C1 standalone, EA_Travel | PASS: 60 frames, indices 0–59, capture 9.131 s |
| C2 | PASS: 480 primary plus 12 travel frames; zero mismatches or ambiguous frames |

Both C1 sessions produced native 1280×720 images with correct check-region pixels, no black/unknown-format frames, **D3D11, PF_A2B10G10R10 (18), AA 0 and zero drops**, then exited normally. C2 completed three apply/revert events per anomaly in each tick order, including Pace 0; six interior blink toggles per order matched. Apply-only, revert-only and blink-toggle one-frame label shifts each failed independently. [Session receipts](/D:/IA_Europa/Receipts/09433/session-summary.json).

Both targets finally built on `b80b971`. All six starts used BelowNormal priority, two actions and ≥8 GiB free commit; none hit the 3 GiB build stop.

| Build | Start / minimum commit GiB | Result and compile errors |
|---|---:|---|
| Editor 01 | 10.14 / 8.71 | FAIL: 1 AnomalyCapture ternary-type error |
| Editor 02 | 10.12 / 8.93 | PASS after explicit RHI-reference correction |
| Game 01 | 10.07 / 7.83 | PASS |
| Game 02 | 10.12 / 7.93 | FAIL: 5 AnomalyCapture diagnostics from unavailable `Create2D` |
| Game 03 | 10.10 / 8.86 | PASS using 4.25 `Create2DDesc` |
| Editor 03 | 10.06 / 4.81 | PASS |

Other modules had zero compile errors. [Build evidence](/D:/IA_Europa/Receipts/09433/build-summary.json). The rebound **27-file manifest matches disk**; [artifact-hashes.json](/D:/IA_Europa/Receipts/09433/artifact-hashes.json) lists every executable/DLL hash. No binary archive was created.

C3 enables the scoped mask and reduction shaders, temporary stencil tags 128–131, and paired raw stencil/custom-depth/scene-depth evidence. Its binding smoke compiled the new shaders, reached zero pending shaders and exited normally. Before launch, the synthetic one-frame mask shift failed at both edges; the wrong-target negative produced IoU 0, coverage 0 and spill 1. [C3 tests](/D:/IA_Europa/Receipts/09433/tests/c3.json).

The C3 session ran **02:52:04–02:55:05 IST**. At its first reduction, [Runtime.log:636](/D:/IA_Europa/Receipts/09433/c3-attempts/C3/01/session/Runtime.log:636) reported missing shader-resource access for both mask textures. The confirmed defect is [AnomalyMasks425.cpp:99](/D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/Compat/AnomalyMasks425.cpp:99): it places `TexCreate_ShaderResource` in `Flags`, while [4.25 validation](/E:/EpicGames/UE_4.25/Engine/Source/Runtime/RenderCore/Private/ShaderParameterStruct.cpp:496) requires it in `TargetableFlags`. The exact blocking/assertion site remains unconfirmed.

| C3 anomaly | Completed events / measured frames | IoU, coverage, spill | Verdict |
|---|---:|---|---|
| blinking | 0 / 0 | Unavailable | NOT MEASURED |
| missing_object | 0 / 0 | Unavailable | NOT MEASURED |
| lod_corruption | 0 / 0 | Unavailable | NOT MEASURED |
| lighting_mismatch | 0 / 0 | Unavailable | NOT MEASURED |

FO3 thresholds were not lowered. No real-data mask negatives or distributions can be claimed. The required descriptor correction is recorded but unapplied: **FM1’s three starts per target and the single C3 slot are spent**. [C3 diagnosis](/D:/IA_Europa/Receipts/09433/c3-diagnosis.json).

The owned deadline stopped monitor **44120 before game 29756**, returning 125 after 181.457 s. The original kit receipt marks a crash/compile failure because its shader-error matcher also catches runtime validation errors; that receipt remains unchanged. No independent engine exception, boot shader compilation failure or new crash folder was confirmed. Six other sessions exited normally.

Upload signs and protected changes: **none**. Entry/exit checks preserve m51, protected refs, Content, Config, descriptors and project files; no drive-root entries changed. C3’s `r.CustomDepth=3` and stencil changes were process-local; normal restoration was not reached before forced teardown. No machine settings changed. [Exit proof](/D:/IA_Europa/Receipts/09433/exit.json).

FO4 restored the original 094-31b paragraph verbatim and appended correction G489; G490 records the descriptor failure. Commits on `feat/europa-port`: `dcdf379`, `818d883`, `10b271f`, `b80b971`, and **`8d1b2e49b322738278ee3715807012791062504c`**. W4 overrides were absent; worktree clean. No push or tags.

VERDICT: RED | UI-free C1 and C2 pass; C3 failed before producing masks and remains unqualified.
