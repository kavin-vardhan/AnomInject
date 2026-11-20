# CaptureBench — capture-method A/B benchmark

A **throwaway test plugin** to answer one question: is the *SceneViewExtension* capture
method (the one the `ExportTextures` / `r.DumpML` plugin uses) actually faster / smoother /
gap-free compared to our `AnomalyCapture` **backbuffer-readback** method?

It captures **final color only** (PNG to disk) two different ways, behind an **identical**
measurement harness, so the numbers are directly comparable. It does **not** touch the
`AnomalyInjector` / `AnomalyCapture` plugin.

Non-shipping only (gated by `CAPTURE_BENCH`; the module is inert in Shipping).

## The two methods

| Mode | Hook | What it grabs | Equivalent to |
|------|------|---------------|---------------|
| `sve` | `FSceneViewExtension::SubscribeToPostProcessingPass` (after `VisualizeDepthOfField`) → `AddEnqueueCopyPass` (async RDG readback) | post-tonemap/FXAA scene color (no HUD) | ExportTextures / DumpML |
| `backbuffer` | `FSlateRenderer::OnBackBufferReadyToPresent` → `FRHIGPUTextureReadback::EnqueueCopy` (async) | final presented backbuffer (HUD composited) | our `FAnomalyFrameCapturer` |

Both feed the **same** pipeline: async GPU→CPU readback → render-thread lock+copy (drained
via a game-thread ticker, off the graph) → **thread-pool PNG encode+write**. The write path
is deliberately identical so the only variable is the **capture hook**.

Key difference under test: the SVE hook fires **once per rendered frame from inside the
render pipeline** (no game-thread arming), which is the likely reason the other games felt
"gap-free". This plugin captures **every rendered frame** in *both* modes, so if a mode still
shows gaps, the gap is inherent to that hook — not to arming cadence.

## Commands

```
CaptureBench.Start <sve|backbuffer> [maxFrames] [outDir]
CaptureBench.Stop
CaptureBench.Status
```

- `maxFrames` — stop automatically after N frames (`0` / omitted = until `.Stop`).
- `outDir` — defaults to `Saved/CaptureBench/<mode>_<timestamp>/`.
- Frames land as `frame_000000001.png`, …

### Run the A/B (do this in a real Play session)

```
CaptureBench.Start sve 300
```
… play for a bit; it auto-stops at 300 frames and logs a summary. Then:
```
CaptureBench.Start backbuffer 300
```
Compare the two `LogCaptureBench` summary blocks (and the `CSV,` lines).

## Where to run each mode

- **`sve`** works in **PIE (editor)** and Standalone — it hooks the game scene's
  post-process chain regardless of the window.
- **`backbuffer`** filters to a real **GameWindow**, so run it **Standalone**
  (`Play → New Editor Window` won't match; use *Standalone Game* or a packaged Dev build).

## Reading the summary

```
================ CaptureBench summary ================
Method                 : SVE(PostProcess)
Frame size             : 1920 x 1080 (fmt=..., 4 bpp)
Wall time              : 5.002 s
Rendered frames (span) : 300  (rtframe 10234..10533)
Captured frames        : 300
Frame coverage         : 100.00 %  (gaps: 0 events, 0 frames missed, max run 0)   <-- "no frame gaps"
Captured FPS           : 59.98   (render FPS ~ 59.98)
RT hook cost / frame   : mean 0.0100 ms, max 0.0400 ms                             <-- render-thread cost (smoothness)
GPU->CPU readback lat. : mean 2.00 frames, max 3 frames                           <-- async pipeline depth
RT copy-out / frame    : mean 0.30 ms, max 1.10 ms
Disk written / dropped : 300 / 0   (612.0 MB, 122.3 MB/s, mean encode 3.10 ms)
======================================================
```

The metrics that answer the question:

- **Frame coverage / gaps** → *"did it leave frame gaps?"* 100% coverage, 0 gaps = every
  rendered frame captured. Any `frames missed` > 0 means the hook could not keep 1:1 with the
  renderer.
- **RT hook cost / frame** → *"is it smooth?"* This is how much render-thread time the capture
  steals each frame. Lower = less chance of a hitch. Compare the two modes directly.
- **Captured FPS vs render FPS** → whether capturing dragged the frame rate down.
- **GPU→CPU readback latency** → how many frames behind the async readback lands (both methods
  are non-blocking; this shows the pipeline depth, not a stall).
- **Disk MB/s + mean encode** → the write path (identical across modes; a control variable).

The `CSV,<method>,<renderSpan>,<captured>,<coverage>,<gapEvents>,<gapFrames>,<maxGap>,<wall>,<meanHookMs>,<maxHookMs>,<meanLat>,<meanCopyMs>,<written>,<MB>`
line is for pasting into a sheet.

### Comparing against the real `AnomalyCapture`

To compare against the *production* path (which arms one frame at a time from the game
thread), run a normal `IAI.Capture.Start` session of a known length and compare its
frames-written / wall-time against this plugin's **Captured FPS** and **coverage**. If
production shows lower coverage than `CaptureBench sve`, the gap is in the game-thread arming
cadence, not the readback.

## Notes / limits

- Final color only. Depth/UI/GBuffers are future work — the SVE hook is the right place to add
  them (that's the whole point of this test), but this plugin stays color-only.
- PNG (BGRA8) is written for every frame — real capture would batch/encode differently; the
  point here is a **fair, identical** write path across the two hooks, not peak throughput.
- `sve` grabs after `VisualizeDepthOfField` (post-tonemap, **no HUD**); `backbuffer` includes
  the HUD. Framing differs by design — the benchmark is about capture cadence/cost, not pixel
  parity. Dimensions are printed so it's transparent.
