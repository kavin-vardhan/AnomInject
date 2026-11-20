# 094-33b — Code's short review (both descriptor fixes hold; masks sit on the right objects; lighting and LOD fail FO3 coverage because the picture also changes away from the objects)

**Date:** 2026-11-01, about 04:20 IST · **Author:** Claude Code (mailbox launcher and reviewer) · **These are observations, not rulings.**

**Inputs:**
- Codex's report, collected byte-identical as `_reviews\094-33b-codex-report.md` (sha256 `5FFE8B8F…`). Codex exited 0 after 1,825 s (03:36:27 → 04:06:52 IST). Its verdict is **NEEDS-DECISION**.
- My receipts: `code-receipts-094-33b\` and `D:\IA_Europa\Receipts\CodeReview09433b\`. My scripts: `D:\IA_Europa\Temp\CodeReview09433b\` (they use tools copied out at `d43635d` with read-only git).

## 1. The fix
**The citations match 4.25 source.**
- `RendererInterface.h:109-110` stores `Flags` and `TargetableFlags` separately.
- `ShaderParameterStruct.cpp:496` checks `TargetableFlags & TexCreate_ShaderResource`, and only logs an Error.
- For the second fix: `D3D11Commands.cpp:2320` is the `BUF_SourceCopy` ensure, `RHIGPUReadback.cpp:71` is the `CopyToStagingBuffer` call, and `RenderGraphResources.h:560` gives `CreateBufferDesc` `Static|UAV|SRV` only.

**New finding (an inference, not proven):** the old descriptor also breaks a fatal check, `check(!(Flags & TexCreate_ShaderResource))` at `RHIUtilities.h:603`.
- `Graph.Execute` reaches that check through `RenderTargetPool.cpp:350-357` for any UAV-targetable texture.
- That is the likeliest unnamed assert behind 094-33's fatal path, because the `:496` line only logs. The 094-33 log has no assert text, so this is not proven.
- The new descriptor (`Flags` None, `TargetableFlags` SR|UAV) passes `RHIUtilities.h:601-603/609/612` and `RenderTargetPool.cpp:343`.

**My descriptor test passes.** It models those 4.25 checks on the committed sources (`descriptor-test.json`):

| Version | Result | Why |
|---|---|---|
| `8d1b2e4` (old) | refused | the `:496` Error, the `:603` fatal check and the `:2320` ensure |
| `e129eba` | refused | the `:2320` ensure only |
| `d43635d` (final) | accepted | — |
| five mutants | all refused | — |

**Codex's own test functions, run on the real sources, agree:** the texture check refuses `8d1b2e4`, and the buffer check refuses `8d1b2e4` and `e129eba`. Their test matches the C++ text with a regex; mine adds the engine checks.

**Other descriptors:**
- **On the 4.25 C3 path there is no further instance.** The path has three RDG textures and the reduction table, both fixed, and the CPU-readback staging texture (`AnomalyReadback425.cpp:8-9`). That texture is created directly by the RHI, so the RDG flag rule does not apply to it.
- **The two other buffer/readback sites are 5.1-only and not compiled on 4.25** (`TexCorruptUniform.cpp:207`, `AnomalyMaskSceneViewExtension.cpp:344`).
- ⚠ **Codex's first descriptor review missed the copy-source rule** (`descriptor.json`: "table … SRV|UAV"). Slot 01's log is what found it.

## 2. C3 (session slot 02; slot 01 is byte-identical: frames, masks, evidence and tables 240/240 each)
**Integrity checks, all mine:**
- The mask PNG equals the evidence mask channel on 240/240 frames.
- The GPU table equals my recompute on 240/240.
- All 72 edge frames (steps 0–2 and 17–19 of each event) have an empty mask.

**Per-anomaly metrics.** I recomputed every number from the raw files, and each equals Codex's to 4 decimals:

| Anomaly / state | Frames | IoU | Spill | Coverage (≥0.95) | Result |
|---|---:|---|---|---|---|
| blinking drawn | 21 | 1.000 | 0 | – | PASS |
| blinking hidden | 21 | 1.000 | 0 | – | PASS |
| missing_object | 42 | 1.000 | 0 | – | PASS |
| lod_corruption | 42 | 1.000 | 0 | 0.0493–0.0494 | **FAIL** |
| lighting_mismatch | 42 | – | 0; outside receivers 0 | 0.5645–0.5647 | **FAIL** |

**The coverage failure is not a threshold artifact.** At stricter change levels (>16, >32 and >64), lighting reaches only 0.58, 0.66 and 0.80, and LOD only 0.07, 0.10 and 0.24.

**Why it fails** (my sheet, `c3-mask-sheet.png`, plus pixel counts):
- **Every mask sits exactly on its object.**
- **Lighting (frame 63):** all 344,769 uncovered changed pixels are untagged background. By eye it is the glow over the dark backdrop around the saturated objects (it looks like bloom; not measured), which dims when the light changes.
- **LOD (frame 123):** of 322,615 uncovered pixels, 43,750 are the removed cube's footprint, 31,721 are floor shadow, and the rest is the same glow.

**What IoU 1.000 does and does not show:** the reference comes from the same stencil and custom-depth readback the shader uses. So IoU proves the shader equals a CPU recompute of the same input. It does not prove the mask matches the picture.

**The pixel-side signs:**
- blinking's drawn frames change 0 pixels against the clean frame;
- on hidden and missing frames, the change sits where the masked cube vanished;
- but only about 23% of mask pixels change by more than 8 levels, because a white cube sits on a saturated white background.

**The negatives, mine, on the real masks:**
- **One-frame shift, late and early:** fails 6 frames per anomaly, always at event edges.
- **Wrong target** (another anomaly's mask at the same step): fails 42/42 frames for blinking, missing_object and LOD.
- ⚠ **For lighting, the wrong target passes the geometry checks (0/42).** Lighting has no IoU term, and every tagged object lies inside the receivers. Only coverage could catch it, and coverage already fails on the real masks. Codex scoped its claim the same way.

**Masks I opened, one per anomaly:**
- **blinking** (frames 6 and 4): tag 128, 78,390 px, on the right-hand cube.
- **lighting** (frame 63): tags 128–131 = the floor and three cubes.
- **LOD** (frame 123): tag 129, 69,520 px, on the sphere, not on the old cube.
- **missing_object** (frame 183): tag 130, 78,390 px, where the left-hand cube was.

**C2 on the same session:**
- **The reader (C3 profile):** PASS, 240/240 frames and 0 mismatches on both slots. Its three label-shift negatives fail.
- **My label-free classifier:** 0 mismatches and 0 foreign labels.
  - Clean frames have 0 changed pixels; active frames have at least 116,498.
  - Its edges equal the labels' edges.
  - Every blink-shift negative fails.
- **The reader change only adds a 240-frame profile.** The 480-frame default is untouched.

⚠ **For chat:** on blinking's drawn frames (3–5 and 10–13 of each event), the mask carries tag 128 while the label says no anomaly.
- FO3 asks for exactly this (drawn-frame IoU).
- On 5.1, `m44` writes masks only on frames labelled for the event.
- So the rule the 4.25 port ships needs a ruling before any office capture.

**The retry slot** fixed the ensure, not coverage. Codex predicted coverage would still fail, and it did.

## 3. Safety
- **Crash: none.**
  - The kit recorded `crash_count 0` on both slots, and both exits were normal.
  - No new `Saved\Crashes` folder exists since 10-27.
  - Slot 01 logged one handled ensure (frame 94, `BUF_SourceCopy`). The process continued, and slot 02 has no ensure.
- **Upload: none.**
  - The session ran `-unattended` with `bSendUnattendedBugReports=False`, so 4.25 returns before contacting the crash client (`WindowsPlatformCrashContext.cpp:624-634`).
  - The CRC logs show only the startup DataRouterUrl print and `Editor/ExitCode:0`.
  - `upload-check` passes on both slots.
- **The Paris host:** Content (1,456 files), plugin Content (5), Config (18), the descriptor and the `.uproject` are hash-, size- and mtime-equal, before against after, from my own lists.
- **Binaries:**
  - Only the host exe/pdb and the editor `AnomalyCapture` dll/pdb changed, all inside build windows (`bin-timeline.txt`).
  - The manifest is 32/32 equal.
  - Builds ran BelowNormal with `-MaxParallelActions=2`. Editor 01 passed, 02 failed (C2678) and 03 passed; game 01 and 02 passed.
- **Git:**
  - `m51` equals P9 at start, at launch and at exit. master, release and the 30 tags are unchanged.
  - Four feature commits, ending `d43635d`. There is no origin branch and nothing was pushed.
  - G491 and G492 are unique across all refs.
- **My poll ran without gaps,** 03:37–04:07 IST.

## 4. What's left before an office capture smoke on Europa's fork
1. **A chat decision on lighting and LOD C3.** The masks are right, but FO3's coverage cannot pass on this glowing, saturated fixture. One option is a bloom- and exposure-off arbiter, like 5.1's AA-off one.
2. **Rule whether masks may appear on frames labelled clean** (blinking's drawn frames), then the real firing path (FO1).
3. **Then a read of Europa's fork:** its viewport, render target, `PostRenderViewFamily` and tick mode.
