# 094-33 — Code's short review (FO2 UI-free frames pass; C3 crashed before writing any mask)

**Date:** 2026-11-01, about 03:10 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- **Codex's report:** collected byte-identical as `_reviews\094-33-codex-report.md` (body 5,581 B, sha256 `B7B82A80…`).
- **The run:** Codex exit 0 after 3,166 s (02:09:09 → 03:01:55 IST). Codex's own verdict is **RED**.
- **My receipts:** `code-receipts-094-33\` and `D:\IA_Europa\Receipts\CodeReview09433\`.
- **My scripts:** `D:\IA_Europa\Temp\CodeReview09433\`. Each reads committed tools copied out at `10b271f` / `8d1b2e4` with read-only git.

## 1. FO2: the pre-UI hook, the heartbeat check, and the C1/C2 re-runs

### The hook
**It matches 4.25 source.** `AnomalyViewMetadata425::PostRenderViewFamily_RenderThread` copies `Family.RenderTarget->GetRenderTargetTexture()` over the single view's `UnscaledViewRect`. It refuses multiple views, a missing texture or a rect mismatch, and it is guarded by `ANOMALY_ENGINE_425`.

**Where 4.25 calls it, and what comes after:**
- **The hook call:** `SceneRendering.cpp:3044`, inside `FSceneRenderer::RenderFinish` (`:2735`).
- **The viewport draws:** `GameViewportClient.cpp:1528` queues `BeginRenderingViewFamily`.
- **UI drawn after that:** the HUD at `:1624`, the debug canvas at `:1672` and stats at `:1695`. Slate draws later still.
- **Codex's citations** (`:3040`, `:1525`) are the adjacent block and comment lines.
- **One exception to keep in mind:** `SceneRendering.cpp:3024-3037` still draws `FVisualizeTexturePresent` before the hook, but only when `vis` debug output is on.

### The heartbeat check, and why it is stronger than a colour test
**The new frames are byte-identical to 094-32's frames at the same index, everywhere outside the old heartbeat-text box:**

| Session | Frames identical outside the box |
|---|---|
| C1 PIE | 60 / 60 |
| C1 Game | 60 / 60 |
| C2 epoch 1 | 480 / 480 |
| C2 epoch 2 (travel) | max difference 1 level, which is the tonemapper's grain |

**Inside the box:**
- the green text is gone (green drops 153–158 levels on the old text pixels);
- my detector (`g > r+30 and g > b+30`, whole frame for C2) finds **0 pixels in all 612 frames**;
- the controls read 1,358–1,377.

**By eye:** I opened frames 0/30/59 (C1) and 0/240 plus travel frame 5 (C2), against the 094-32 controls. The text is gone and the scene behind it is intact (`fo2-heartbeat-sheet.png`).

### C1 re-run, from the raw files: PASS
| | PIE (EA_Core) | Game (EA_Travel) |
|---|---|---|
| Frames | 60, indices 0–59 | 60, indices 0–59 |
| Distinct images | 7 (grain period), 0 consecutive equal | 7, 0 consecutive equal |
| Black frames | 0 | 0 |
| Format | 18, D3D11, 0 dropped | 18, D3D11, 0 dropped |
| Check region | mean 0.497, fraction 0, against the Phase A reference | mean 0.478, fraction 0, against 094-32's same-map engine read |
| Exit | normal | normal |

No argv or logged command line carries a backbuffer override.

### C2 re-run: PASS
- **The reader,** re-run by me: PASS, 0 mismatches.
- **My label-free classifier** (per-event baseline): 0 mismatches and 0 foreign labels.
  - Its edges equal the reader's.
  - Minimum clean/active separation is 132.8×.
  - Per anomaly and order: 3 apply and 3 revert edges, plus 6 blink toggles for blinking.
  - The Pace 1 and Pace 0 blocks both ran.
- **My six shifts** (apply, revert and blink-interior, each early and late): **all fail**, each at exactly the shifted indices.
- **Codex's three kept mutated label files:** the reader refuses them, and so does my classifier.

## 2. FO3: C3 produced no masks, and the session crashed

### What did work
- **The builds:** AnomalyShaders built in on 4.25 (editor-03 and game-03 PASS on `b80b971`).
- **The C3 smoke:** it compiled 2 shader jobs at boot with no compile error, reached `Shaders left to compile 0`, and exited normally.
- **The C3 session found its shader:** it resolved `FAnomalyMaskReduceCS` from the global shader map, because 4.25's parameter validation names it.
- **So no boot compile failure occurred.** The kit's `shader_compile_failure: true` comes from its broad matcher on the line "LogShaders: Error", as Codex discloses.

### The failure
**The descriptor defect is confirmed against source.**
- The code (`AnomalyMasks425.cpp:99-100`) puts `TexCreate_ShaderResource` in `Flags`.
- `Create2DDesc` stores `Flags` and `TargetableFlags` separately (`RendererInterface.h:109-110`).
- The check (`ShaderParameterStruct.cpp:496-503`) reads `TargetableFlags`, and only logs an Error.

**Where I differ from Codex: this was a crash, not only a stall.** The log's last line (`Runtime.log:638`, frame 93) is `LogWindows: Windows GetLastError: … (0)`.
- 4.25 prints that line only in `FWindowsErrorOutputDevice::Serialize` (`WindowsErrorOutputDevice.cpp:30-46`), which is the fatal-error and assert path.
- So the engine **entered its fatal-error path** right after the two validation errors.
- The process then hung until the kit's 180 s stop (181.457 s, exit 125), and no crash folder was finalized.
- The assert text and its site are not in the log.
- The kit counted it: `crashed true`, `crash_count 1`.
- **Under FI4 this is the run's first crash.** The leg fails and the run continues. **It is not a second crash.**

### The masks and metrics
- **0 masks, 0 frames, 0 labels.** I had nothing to recompute against the frozen thresholds, and no mask to open.
- **The reader's thresholds match FO3:** IoU 0.90, coverage 0.95, spill 0.01.
- **Notes on its references (for when it runs):**
  1. lod_corruption's IoU compares the shader's mask with a CPU recompute of the same stencil and custom-depth readback. The pixel-independent term there is coverage of changed RGB pixels.
  2. lighting's "receivers" are every tagged fixture object (stencil 128–131).
  3. blinking uses the clean pre-apply silhouette for both drawn and hidden frames, which is fine on this static fixture.
  4. The real-data mutants in `read()` have never run.

### The negatives, run by me
- **Codex's synthetic test:** PASS. The shift fails at indices 1 and 3; the wrong target scores IoU 0 and spill 1.
- **Mine, with the reader's `metrics()` and my own IoU/spill, on regions taken from the kept C2 frames** (blinking event 0, 57,966 px; missing_object event 9, 57,930 px; overlap 0):
  - the correct sequence passes;
  - a one-frame shift fails at frames 6, 10, 14 and 17 (late) and 5, 9, 13 and 16 (early);
  - the wrong target fails with IoU 0 and spill 1.
- These show the metric can fail. **They are not C3 evidence.**

## 3. FO4: PASS
- `docs/gotchas.md` at `8d1b2e4` is `32c5bf5`'s 10,297 lines verbatim, plus 21 appended lines (G489, the correction, and G490, the descriptor finding).
- The restored 094-31b line is byte-equal to the original.
- G489 and G490 appear on no other ref (`git log --all -S`).

## 4. Safety
- **Crash:** one, C3, as above. **No upload sign:**
  - all seven `upload-check` results PASS;
  - the CrashClient logs show only the startup DataRouterUrl print;
  - host `Saved\Crashes` is unchanged since 10-27.
- **The Paris host:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal from 02:06 to 03:03 (`paris-diff.json`). Host `Source` is unchanged.
- **m51:** HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries, at start, at launch and at exit. master `b5f15a3`, release `d7eee97` and the 30 tags are unchanged.
- **Binaries:**
  - All 13 changed files have mtimes inside Codex's game-03 window (02:41:29–02:42:20) or its editor-03 window (02:42:33–02:45:05).
  - The manifest is 27/27 equal.
  - The probe DLL relinked in editor-03; its source did not change.
  - All six builds used `-MaxParallelActions=2`. Minimum free commit was 4.81 GiB, above the 3 GiB stop.
  - ⚠ **My poll was not running from 02:39 to 02:51,** so I saw neither the three C3-window builds nor the C3 smoke live. Their priority is Codex's statement, and the argv comes from the receipts.
- **Feature branch:** `feat/europa-port` has five commits, ending `8d1b2e4`. The tree is clean, there is no origin branch and no tags, and nothing was pushed.

## 5. What's left before an office capture smoke on Europa's fork
1. **Fix C3's texture flags** (`TexCreate_ShaderResource` into `TargetableFlags`), then run one new authorized build pair and one C3 session to get the per-anomaly metrics and the real-data negatives.
2. **Then the real firing path** (FO1), instead of the bench driver.
3. **Then a read of Europa's fork:** its viewport, render-target and `PostRenderViewFamily` behaviour, and its tick mode.

FO2's pre-UI colour capture and labels already carry over as-is.
