# 094-30 Code review: Phase C capture port plan (short, FJ1)

Claude Code, 2026-10-31, about 09:15 IST. These are observations for chat, not instructions to Codex.
Subject: `feat/europa-port` 4f46a9c (one local commit on abb240b, docs only: the plan +106, CLAUDE.md +2, gotchas +4, session +11). Codex report: `_reviews\094-30-codex-report.md`, collected byte-identical. Codex ran 1,434 s with exit 0.

**Protection checks (mine):**
- m51 matches P9 at start, prelaunch and after: 53bf725, index 526aba7f, 0 tracked changes, 15 untracked. master b5f15a3, release d7eee97, and the 30 tags are unchanged.
- The Paris host is hash-equal before and after (`paris-diff.json`): Content 1,456 files, plugin Content 5, Config 18, host Binaries 16, plugin Binaries 11, the descriptor and the .uproject. Host and plugin Intermediate are unchanged by size and mtime.
- No push: no `origin/feat/europa-port` exists. W4 held: the child saw neither date variable, and both commit dates are the real 09:04:15.
- My 20 s process sampling, across three polls, saw no UE4Editor, game, UBT, cl or link process.

## 1. Spot-check of seven different/missing rows against 4.25 source (`E:\EpicGames\UE_4.25`): all confirmed
| Row | What I checked | Result |
|---|---|---|
| 05 | `RHIGPUReadback.h:99` `Lock(uint32)` only; `.cpp:128-136` maps into local `BufferWidth/Height` and returns only the pointer. `MapStagingSurface(…, int32& OutWidth, int32& OutHeight)` at `RHICommandList.h:4443` | Confirmed |
| 06 | `RHICreateTexture2D(…, FRHIResourceCreateInfo&)` at `RHICommandList.h:5179`; 0 hits for `FRHITextureCreateDesc` in RHI/Public | Confirmed |
| 07 | `TransitionResource(EResourceTransitionAccess, FRHITexture*)` at `:3390`; 0 hits for `FRHITransitionInfo`/`ERHIAccess` | Confirmed |
| 12 | 0 hits for `AddEnqueueCopyPass` in Runtime `*.h/*.cpp` | Confirmed |
| 13 | 0 hits for `ExtentionData`/`ExtensionData` in Engine+Renderer; `FSceneViewFamily` (`SceneView.h:1406`) carries `RenderTarget`, `Scene`, `FrameNumber` (`:1522-1540`) | Confirmed |
| 15 | 0 hits for `OnWorldTickEnd`. `TickObjects` is at `LevelTick.cpp:1630`, then the camera update at `:1634ff` and `TG_PostUpdateWork` at `:1677`, then `OnWorldPostActorTick` at `:1694` inside `if (bDoingActorTicks)` at `:1690`. The Phase A shim returns `GetWorld()` from `GetTickableGameObjectWorld`, so the injector ticks at `:1630`, before the anchor | Confirmed |
| 26 | 0 hits for `CalcSceneCustomStencil` in Shaders; the swizzled stencil load is at `DeferredShadingCommon.ush:635,704` | Confirmed |

I also checked two more rows, and both hold: row 10 (0 hits for `SubscribeToPostProcessingPass`/`PrePostProcessPass_RenderThread`) and row 25 (`SceneTexturesConfig.h` is absent; `FSceneTexturesUniformParameters` is at `SceneRenderTargetParameters.h:19`). The writers the plan keeps also check out: the `SaveArrayToFile(const TArray64<uint8>&…)` overload exists (`FileHelper.h:123`), and the `FormatLogLine` signature matches what `AnomalyRunLog.cpp:110` uses. The 58-file module count matches (42 + 7 + 9).

**Two facts the plan should carry, in the C1 adapter:**
- 4.25's own `EnqueueCopy` allocates the staging texture at the full **source** size and copies by `CopyToResolveTarget(Rect)` (`RHIGPUReadback.cpp:112-123`). This is the full-source layout m35 designed around. Keep m35's approach: copy the view sub-rect into an owned W×H texture at (0,0).
- On D3D12, `RHIMapStagingSurface` can return `OutWidth/OutHeight = 0` when the map arrives before the copy (`D3D12RenderTarget.cpp:1465-1470`, an engine comment that admits it). The adapter must treat 0 or short dimensions as "not ready: drop the frame and count it", never as data. On D3D11, the Paris host's default (its Config sets no `DefaultGraphicsRHI`), `OutWidth = RowPitch/Bpp` and `OutHeight = DepthPitch/RowPitch` (`D3D11RenderTarget.cpp:817-818`).

## 2. Scratch compile: none ran
No compile happened, and that agrees with the unchanged Intermediate listings and the empty process sightings. Codex's reason holds in substance: the enable switches are hard-coded, and the qualified target writes the bound host. Its UBT XML-cache point is weak, because that cache lives in the user's AppData and is not protected. An isolated E: copy of the host was possible, so "not cheap" is a lean-profile judgement, not a blocker. As a result, the C1 file list is derived from the inventory, not from compile errors. The first C1 build is the measurement.

## 3. Stages and gates
- **C1 is concrete,** measured in two sessions (PIE on EA_Core, `-game` on EA_Travel), each with 60 frames at 1280×720. Two wording fixes:
  - "60 distinct … PNGs" must mean distinct indices and files. A static scene with a fixed camera and AA off can legitimately produce byte-identical frames, so content-distinctness would false-fail.
  - "No black frames" needs a named threshold: reuse m47's derived 6.0 whole-frame mean, or derive one from the Phase A sheets.
- **C2's comparison is strong:** it covers every apply, revert and blink-toggle edge, in both tick orders, with independent pixel classes, and an ambiguous edge fails. It catches a uniform one-frame offset at nominal pacing. Two cheap additions would make it catch the two historical forms of the defect:
  - **(a) Make the can-fail mutation per edge class.** Shift only the revert edges, then only the blink interior toggles; each must fail. A whole-label shift is caught by the apply edges alone, so it does not prove the reader resolves interior toggles, which is where P9 lived: {n,n+1,n+5,n+6} against {n,n+1,n+2,n+6}.
  - **(b) Add one `IAI.Capture.Pace 0` block** to the same session. On the backbuffer path, the pacer is what made the old arm→present pairing look correct. Before m21, pace-off read −1 while ratio≈1 was exact (journal 027). A stock-4.25 regression of m21's render-command-ordered arm would pass a paced-only C2.
- **C3's single threshold (IoU ≥0.90, recall ≥0.95, ≤1 % spill on every counted frame) does not fit two of the four anomalies:**
  - For lod_corruption, the visible change is the cube/sphere difference, not the target silhouette (m29 recorded the same: only edges change). A correct mask will score low IoU.
  - For lighting_mismatch, the target is a light: its stencil mask is empty or undefined, and its change is a lighting footprint. The plan's own rule, "an empty reference fails", collides with this.
  - **Proposed fix:** IoU and recall for blinking and missing_object; change-inside-dilated-mask recall plus spill for LOD; and declare lighting_mismatch as mask-not-provided, or define its mask. Otherwise C3 forces either a false RED or a post-hoc retune, which the plan itself forbids.

## 4. C1 file list
It is complete against the inventory: every row maps to a listed file (05–08 → FrameCapturer + Readback425; 09–14 → SVE isolation; 15 → Subsystem; 18 → ViewMetadata425; 31–33 → Measurability, Subsystem, compat). Three things are missing or unstated:
- **(i)** The C1 request must authorize replacing the Paris host's bound plugin binaries (`Plugins\AnomalyInjector\Binaries\Win64\UE4Editor-Anomaly*.dll`), and re-arming the launch kit's binding manifest (`D:\IA_Europa\Launch\Europa425-*.json`). The plan mentions this only as "later authorization".
- **(ii)** The new capture driver needs a kit leg definition, not only the "affected harness" line.
- **(iii)** Housekeeping: the new gotcha is numbered **G475**, which collides with `feat/vt-blur-stacks`'s G475 (pushed). Across refs, the next free number was G488. The commit is local and unpushed, so renumber it in a follow-up commit before D-lite.

## 5. My top three risks (they differ from Codex's in emphasis)
1. **A label offset passes C2.** C2 runs only paced, and its can-fail is a uniform shift. Additions 3(a) and 3(b) close this.
2. **Readback on D3D12 or Europa's fork.** A map can return 0×0 before the copy lands, and the home gate on D3D11 never exercises that path. The adapter's refusal must be tested with a synthetic zero or short map (the plan already names one bounds negative), and the office smoke must record the RHI.
3. **C3 is not evaluable for LOD or lighting as written.** This needs per-anomaly metrics before the predictions are frozen.

Codex's risk list (pitch and format, old-family pairing, closure leaks, stencil and resolution, overclaiming) stands. Its estimate is C1 2–3, C2 2–4, C3 3–5 days, 7–12 in total.
