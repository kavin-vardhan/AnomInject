# 094-33 — Chat ruling: 4.25 frames are now UI-free, and C1 and C2 pass again. C3's masks crashed on one wrong texture flag; fix it and run C3 now (094-33b)

**Date:** 2026-11-01, about 03:35 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-33:** NEEDS-DECISION. It ran 02:03–03:10 IST.
- **Codex's report:** `_reviews\094-33-codex-report.md`.
- **Code's review:** `_reviews\094-33-code-review.md`.

## Verdict
**FO2 is done.**
- **The new capture point:** colour is copied in `PostRenderViewFamily_RenderThread`. 4.25 calls it from `SceneRendering.cpp:3044`, inside `RenderFinish`. The HUD, the debug canvas and stats are drawn after that (`GameViewportClient.cpp:1528` → `:1624` / `:1672` / `:1695`).
- **The heartbeat:** 0 heartbeat pixels in all 612 frames, against 1,358–1,377 in the controls.
- **Nothing else moved:** frames are byte-identical to 094-32's outside the old text box.
- **The gates passed again on the new path:**
  - **C1:** PIE and `-game`, 60 frames each, format 18, D3D11, 0 dropped;
  - **C2:** 480 frames, 0 mismatches, and every one-frame shift negative fails.
- **4.25 frames are now clean the way 5.1's are.**

**C3: the shaders compiled and loaded at boot, but no masks came out.**
- **The cause:** the mask textures set the shader-read flag in `Flags`, while 4.25 checks `TargetableFlags` (`RendererInterface.h:109-110`, `ShaderParameterStruct.cpp:496-503`).
- **What happened:** the session asserted at frame 93 (`Runtime.log:638`, on the fatal-error path), then hung until the kit stopped it at its 180 s limit.
- **The safety record:** no crash folder, no upload sign, no Paris change, and `m51` equal to P9.

Code's classification is right: this is NEEDS-DECISION, the run's first FI4 crash, not RED.

## Rulings

### FP1. FO2 is accepted as done

### FP2. The C3 crash is 094-33's first and only FI4 crash
- 094-33b starts its own count.

### FP3. 094-33b: fix the flag, rebuild, and run C3 now
**The fix:** set the mask textures' shader-read flag in `TargetableFlags`, per 4.25's `RendererInterface.h:109-110`, citing the source.
- Test it with a 4.25-shaped descriptor check: a descriptor with the flag only in `Flags` must be refused, and one with it in `TargetableFlags` accepted.
- Look over the other C3 descriptors for the same mistake.

**The build:** one new build pair under FM1, with three attempts per target.

**The C3 session:**
- **one session,** plus **one retry slot,** to be used only if the first run shows a quick, testable fix;
- **the gates:** FO3's frozen per-anomaly metrics and both negatives;
- **the same session's labels must also pass the C2 reader** with zero mismatches. That is a free extra check that masks didn't disturb the colour or the labels.

**The rest:**
- FJ1's lean profile applies;
- **the window for real sessions:** until 12:00 IST on 11-01.

## For the owner
- **Nothing to do.**
- **Frames on 4.25 are now clean like on 5.1.**
- **Masks need a one-line fix,** which is running now.
