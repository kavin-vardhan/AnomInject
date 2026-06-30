# 2026-06-30 · 015 · Stencil-capture milestone — Stage 1 (module split + async backbuffer capture)

Branch `feature/stencil-capture` off `master` `d4a77db`. First stage of a multi-stage milestone:
"Occlusion-correct stencil bounding boxes + async unified capture." This stage delivers ONLY the
quarantined module split + the async, UI-inclusive, non-blocking frame grab (color only). The
stencil tagging and the occlusion-correct box are later stages.

## The milestone (banked plan)
Replace the m7 label box (projected 3D bounds → 2D AABB) with a GPU-truth box from the rendered
silhouette: tag the target with a reserved Custom Stencil value, read custom-stencil + custom-depth +
scene-depth in a Scene View Extension, keep the pixels where `stencil==tag AND customDepth<=sceneDepth`,
and take the 2D AABB of the surviving pixels (occlusion-correct, pixel-tight, animation-correct).
Folds in the async-backbuffer capture milestone. Stages: 1 async color capture (this) · 2 stencil
tagging · 3 stencil/depth → box · 4 multi-actor + docs + tag.

## What was done (Stage 1)
- **New quarantined module `AnomalyCapture`** (gated `ANOMALY_CAPTURE`, compiled out of Shipping;
  render/`RHI`/`RenderCore`/`Slate`/`SlateCore`/`ApplicationCore` + a `bBuildEditor`-only `UnrealEd`
  dep, all only when not Shipping). The m7 capture moved out of `AnomalyControlServer`:
  `UAnomalyCaptureSubsystem`, `AnomalyLabelWriter`, `AnomalyPreviewCapture` relocated; own log cat
  `LogAnomalyCapture`. `AnomalyControlServer` now privately depends on `AnomalyCapture` (its only use:
  `AnomalyPreview::CaptureGameViewportJpeg` for the live preview + resolving `UAnomalyCaptureSubsystem`).
  DAG: **core ← AnomalyCapture ← AnomalyControlServer.**
- **Async, UI-inclusive, non-blocking capture** (`FAnomalyFrameCapturer`): hooks
  `FSlateRenderer::OnBackBufferReadyToPresent` (render thread, post-Slate = the real player frame with
  game UI), clipped to the game-viewport rect (FFrameGrabber pattern: `GetGameViewportWidget` →
  `FindWidgetWindow` → `TargetWindowPtr` + arranged-widget `CaptureRect`; callback ignores other
  windows → no editor chrome in docked PIE). Stages an `FRHIGPUTextureReadback`; the render thread does
  ONLY the IsReady-gated lock + a stride-removed memcpy of the raw native bytes; a per-tick render
  command drains ready readbacks. Replaces m7's synchronous `ReadPixels` flush.
- **Off-thread writer** (`FAnomalyAsyncWriter`, thread pool): convert + PNG/JPEG encode + image write +
  `labels.jsonl` append, all off the game thread (append serialized by a lock; atomic counters mirrored
  back). The label RECORD is built on the game thread (it projects the target actor's bounds = UObject
  access). ImageWrapper preloaded on the game thread so workers never module-load.
- **Frame↔state carry** unchanged in principle: snapshot {`GFrameCounter`, time, view, live fires} at
  submit, keyed by `GFrameCounter`; the resolved frame writes `frame_<N>` + the label from snapshot N.
- **`IAI.Capture.Async <0|1>`** (default ON) — falls back to the legacy synchronous `ReadPixels` path
  for A/B (note: sync = scene + canvas HUD, may MISS UMG; backbuffer is the faithful UI-inclusive one).
- **Our-overlay suppression** (core, additive): generalized `AnomalyViewport::SetDebugSphereSuppressed`
  → `SetOverlaysSuppressed`/`AreOverlaysSuppressed`, checked by the poll-radius sphere, the selector
  `DrawHUD` + selection `DrawDebugBox`, the auto `DrawHUD`, and the injector heartbeat (which is
  ACTIVELY evicted via `RemoveOnScreenDebugMessage` each tick while suppressed — G54). The game's UI is
  never touched. PIE mouse-control-label disabled per-PIE-session at subsystem `Initialize` (G55).
- **`DrainTail` FSM phase** — clean burst-count runs drop ZERO frames (G56).

## Problems → resolutions (the three eyeball rounds)
1. **Grab point wrong (after-tonemap dropped game UI).** Owner clarified the contract = real player
   frame WITH game UI, minus only OUR overlays. Moved the grab from the SVE after-tonemap (pre-HUD) to
   `OnBackBufferReadyToPresent` (post-Slate) + viewport clip (G52). The SVE returns at Stage 3 for the
   stencil mask; color + stencil are now two grab points joined by frame id.
2. **Game-thread stall + animation judder.** The encode + file write were on the game thread (G53).
   Moved them to a thread-pool worker; render thread keeps only the lock-copy-out. Owner confirmed
   smooth.
3. **Lead-in overlay leaks + 1 dropped frame.** Heartbeat: a 2.5 s-lifetime message lingered past the
   stop-new gate → actively evict each tick (G54). Mouse label: `ShowMouseControlLabel` is a one-shot
   show-gate + self-fading widget, not a live toggle → disable at `Initialize` (G55). Dropped frame: the
   last arm presents next frame, after `FinishRun`'s flush → `DrainTail` phase (G56). All confirmed.

## Deviations / decisions
- **P2 module split** (new module) over P1 (extend ControlServer) — owner-blessed; render deps isolated,
  core byte-identical. **Renderer/Renderer-private deferred to Stage 3** (Stage-1 backbuffer color path
  doesn't need them; already proven to compile). **Tagging helper home = AnomalyCapture** (decided, not
  yet built). **Mouse-label scope = per-PIE-session** (owner-blessed trade; the only reliable fix).
- Adaptive non-unity build exposed a missing transitive include in the relocated writer (G57) — fixed
  to be self-contained.

## State / hand-off
- Clean 5.1 StackOBotEditor/Development/Win64 compile (exit 0), all 3 modules link; core dep set
  unchanged; `IAnomaly` untouched; catalog stays 8. Owner re-eyeball GREEN: game UI in, viewport-clipped
  (no editor chrome), no overlays/heartbeat/mouse-label on any frame incl. lead-in, smooth (no judder),
  0-drop clean run, carry intact, colors correct (`fmt=18`).
- Committed on the branch as one atomic `refactor(capture)` (no tag, no version bump).
- **Next: Stage 2** — custom-stencil tagging of the target (`r.CustomDepth 3` at runtime, set/restore
  the target's prior custom-depth flags), gated on a stencil-buffer-dump eyeball. Then Stage 3
  re-introduces the SVE (Renderer/Renderer-private deps re-added) for the stencil/depth mask + the
  occlusion-correct pixel-AABB box, joined to the backbuffer color by frame id (watch mask-rect →
  color-rect mapping / DPI).
