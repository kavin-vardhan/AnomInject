---
name: capture-method-evaluation
description: "SVE vs backbuffer capture A/B — MEASURED results, client wants UI excluded, SVE recommended"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-07-29T09:22:14.534Z
---

Evaluating moving capture off the **backbuffer-readback** method (`FAnomalyFrameCapturer`:
`OnBackBufferReadyToPresent` + async `FRHIGPUTextureReadback`, armed per-frame from the game thread)
to the **SceneViewExtension** method used by `ExportTextures`/`r.DumpML` (WhiteMoon Dreams).

**CLIENT REQUIREMENT (2026-07-13, owner-confirmed): the client wants a way to DISABLE UI from the
capture.** This resolves the earlier open question. SVE color = SceneColor grabbed BEFORE the Slate UI
composite = **inherently UI-free**, so SVE is now the aligned method. (Backbuffer = UI always baked in.)
Corollary worth keeping: in **SDR there is no isolated UI layer** — `GetCompositeUIRenderTarget()` is
HDR-gated (`SlateRHIRenderer.cpp:980-991`: needs `GRHISupportsHDROutput && r.HDR.UI.CompositeMode != 0
&& bSceneHDREnabled`). So a UI **on/off toggle** is cheapest as "SVE = UI off, backbuffer = UI on" —
keep BOTH grab points rather than trying to isolate UI. ExportTextures' `_sui.tif` came from HDR games.

**Owner confirmed the roadmap covers MANY buffers** (color → depth/UI → GBuffers). Backbuffer is a dead
end for depth/GBuffers (they only exist mid-pipeline) → an SVE is required regardless.

**`Plugins/CaptureBench/` A/B test plugin BUILT + RUN (verified clean Dev-Editor compile+link on 5.1).**
Throwaway, separate from AnomalyInjector, non-shipping (`CAPTURE_BENCH`). Commands:
`CaptureBench.Start <sve|backbuffer> [maxFrames] [outDir]`, `.Stop`, `.Status`, `.Write <0|1>`
(0 = capture+readback only, skips PNG encode to isolate hook cost).

**S1 RE-RUN COMPLETE (2026-07-29, journal 028, plugin `b210a52`): 10 rows — Standalone + PACKAGED (exe
hot-swap per G76) + 3 starve legs. ALL rows 100% coverage / 0 gaps / 0 stale-content dupes, both methods.**
SVE hook **80–173x cheaper** and FLAT (max never >0.31ms) vs backbuffer spikes of **7.3 / 7.4 / 11.3 / 18.2 ms**
(at 105fps the budget is 9.5ms → a 7.4ms spike eats 78% of a frame). **Throughput is a TIE** at capture-only
(101.8 vs 104.9; 107.9 vs 107.8) — the SVE win is hook cost + spike absence, NOT throughput. NEW (packaged,
**observed once, needs repeat**): with inline PNG on, backbuffer collapsed to **13.70 fps vs SVE 40.28**
(encode 863ms vs 288ms) — it degrades ~3x worse under the CPU contention encoding creates.
**TWO CORRECTIONS:** (1) PIE's "SVE readback latency 0 frames" was a **PIE ARTIFACT** — both are 1.7–2.0
frames at real rates; no readback-depth advantage. (2) "SVE captures at ViewRect so screen-percentage
desyncs it from output res" is **MEASURED FALSE** — the grab point is AFTER the upsample, both captured
identical 1920x1080 at SP170/SP320; that migration caveat is **withdrawn**.
**⚠ HARD LIMITATION: CaptureBench FREE-RUNS (no fixed timestep, no m11 pacer) → it CANNOT reproduce
production's `speed_ratio` regime. The client's 1.2-band −1 lag can be neither reproduced nor refuted by this
harness** — that belongs to S2/S4 gates on the production path. ≥3x starve NOT reached (2.05x max; `r.setres`
is silently desktop-clamped, use `r.ScreenPercentage`).

**Original PIE run (superseded, kept for the correction record):**
- Coverage 100%, 0 gaps, 300/300 drained — in ALL FOUR runs. Neither method drops frames.
- **RT hook cost mean: SVE 0.0047-0.0051 ms vs Backbuffer 0.4603-0.4732 ms (~95-100x cheaper).**
- **RT hook cost MAX: SVE 0.011-0.014 ms vs Backbuffer 1.53-5.11 ms** ← the "smoothness" difference.
  Cause: backbuffer calls `EnqueueCopy` on the IMMEDIATE command list inside the present hook; SVE
  appends a deferred `AddEnqueueCopyPass` to the RDG graph. (The 5.11ms max coincided with DDC
  maintenance = partly environmental, but the clean run's 1.53ms is still ~100x the SVE max.)
- Copy-out ~equal (0.42-0.47 ms both). Readback latency SVE 0 fr / BB 1 fr (both non-blocking).
- FPS 13.2-14.8 in ALL runs — **the ~15fps PIE ceiling is NOT capture-caused** (Write 0 only moved
  13.6→14.8). Capture is not the bottleneck; something else caps this PIE session.
- Inline PNG encode = **128-136 ms/frame**. Harmless at 15fps, would saturate CPU at 60fps
  (60x128ms ≈ 8 core-sec/sec) → keep offloading video to host ffmpeg; never inline-PNG per frame.
- NOTE: source is **10-bit** (PF_A2B10G10R10); the shared writer truncates to 8-bit BGRA (`>>2`).

**BIGGEST MIGRATION ARGUMENT (beyond perf): an SVE structurally eliminates the m21/m22 bug class.**
The SVE grab point IS the frame being rendered — there is no arm→present matching, so the "next present
wins" undefined-ness ([[m21-frame-pairing-race]]) and the deep-starve stale-scene residual cannot occur.
But it also means the m21 pairing model must be REPLACED (frame↔state keying moves from game-thread
`GFrameCounter` arm to a render-thread `GFrameNumberRenderThread` snapshot) — the single biggest
migration risk given m18/m20/m21 were all label-alignment work.

**Costs/caveats for the migration decision:** SVE needs the Renderer **PRIVATE** include path
(`GetModuleDirectory("Renderer")/Private` for `FPostProcessMaterialInputs`/`FScreenPassTexture`, plus a
`class FViewInfo;` fwd-decl) — private headers can shift between engine versions (maintenance cost;
`Renderer` itself is already on the CLAUDE.md allowed-dep list). SVE color rect = `SceneColor.ViewRect`,
so screen-percentage/dynamic-res makes it differ from output res. NOT yet tested: Standalone/packaged,
high fps, HDR, concurrent anomaly injection, depth, pixel-equivalence. Overlay suppression (G54) may
partly become unnecessary (Canvas/HUD is post-scene → absent from SceneColor; but DrawDebug* line-batcher
primitives ARE scene geometry → would still appear) — needs verification.
Related: [[anomaly-injector-state]], [[m21-frame-pairing-race]].
