# Anomaly Injector — Handoff: m7 (Labeled Frame-Capture + 2D BBox Labeling) shipped

**Purpose.** Cold-reader bridge for what shipped in **m7** and what carries forward. Read alongside `CLAUDE.md`,
`architecture.md`, `gotchas.md` (G35–G43), and `docs/sessions/2026-06-20-012-frame-capture-labeling.md`.

---

## 1. What m7 is
A capture/labeling layer that turns a LIVE auto-injection run into an **ML-friendly labeled image sequence**:
per captured frame, the game-viewport image + a JSONL label record (temporal `anomaly_present` + per-object 2D
bbox) sourced from the auto-injector's **own ground truth** (L1 — we fire deliberately, so we know id/actor/frame;
no replay diff). Catalog unchanged at **7** (infrastructure, not a new anomaly). VersionName **0.8.0**.

## 2. Where it lives + the three sanctioned core exposures
Housed in the **`AnomalyControlServer`** module (`UAnomalyCaptureSubsystem` + `AnomalyLabelWriter`), reusing that
module's game-viewport capture primitive + ImageWrapper; gated by `ANOMALY_CONTROL_SERVER` (compiled out of
Shipping — dataset capture runs in a packaged Development/Test build; never retail Shipping). The **only**
`AnomalyInjector` changes (everything else — `IAnomaly`, injector core, anomalies, leaf helpers, `=` match,
`GetVisibleRenderableActors` — is byte-clean):
1. `AnomalyViewport::ProjectActorBoundsToScreenRect` — the 2D-bbox projection (type-only bounds union, NOT
   `IsVisible`-gated, so a hidden actor still projects — G38).
2. `FAutoLiveFireInfo` widened with `TWeakObjectPtr<AActor> TargetActor` + `uint64 StartFrame`.
3. `UAnomalyAutoInjectorSubsystem::RevertAllLiveFires()` exposed (keeps `GetLiveFires()` accurate during bursts).

## 3. How a run works (the load-bearing facts)
- **Capture-driven deterministic bursts:** `[pre] → FireOnce → [settle K] → [positives P] → RevertAllLiveFires →
  [settle K] → [post]`, looped. Drives the m6 core via `TryFireOnce`/`RevertAllLiveFires` (never `AdvanceTime`).
- **Settle-K is SYMMETRIC** at both boundaries (a game-thread mutation reaches the rendered frame >=1 frame later,
  `r.OneFrameThreadLag`; default K=2 — G37).
- **Same-tick alignment:** image + `GetLiveFires()` snapshot in one game-thread call, stamped one `GFrameCounter` (G36).
- **View-lag L default 0 is CORRECT, not "zero lag":** the capture subsystem ticks before `UpdateCameraManager`
  (LevelTick.cpp:1606 vs 1621), so `GetActiveViewInfo` already returns the previous frame's POV = the view that
  rendered the captured pixels; the two 1-frame lags cancel. FPS-invariant. Knob (`IAI.Capture.ViewLag`) stays for
  the async path (G41).
- **`visible_positive` = `anomaly_present && (≥1 bbox_valid)`** is the detection-relevant positive. Under motion a
  fired actor can leave the viewport mid-hold (`present=true` + no box) — KEPT as a hard negative, not dropped (G42).
- **Output:** `run_<seed>_<timestamp>/` = `frame_<GFrameCounter>.png` (opaque, native res — G39) + `labels.jsonl` +
  `run.json` + `run_summary.json`. `tools/verify_capture.py` overlays boxes + tallies. Console: `IAI.Capture.Shot/
  Config/ViewLag/Start/Stop/Status`. Reproducible at the decision level (seed + fixed vantage), not pixel-level (S4).

## 4. Commit structure (Plan A — two commits)
The m7 cold-boot read the control server's capture primitive from the **working tree** and treated it as committed
— it was the parallel dashboard track's **uncommitted Slice-1 WIP** (G43). History was verified clean (reflog:
nothing orphaned). Resolved by promoting Slice-1 as its own commit first, then m7 path-scoped on top:
- **`ff1be3c`** `feat(control-server): Slice-1 dashboard capture primitive + WS snapshot/protocol` — the shared
  capture primitive (incl. the raw-BGRA + PNG/JPEG + opaque-alpha generalization) + the WS subsystem rewrite +
  `ControlProtocol`/`ControlSnapshot` + `spike-client.html` + Build.cs. **No tag.**
- **`<m7 sha>`** the m7 milestone, **tagged `m7`**, VersionName 0.8.0.

**→ For the parallel dashboard track:** `ff1be3c` promoted your uncommitted Slice-1 verbatim from the working tree.
Reconcile your dashboard journal/handoff against that SHA when you resume. (The Slice-1 work is otherwise unchanged.)

## 5. Forward / deferred
- **Async capture path** (`OnBackBufferReadyToPresent` + `FRHIGPUTextureReadback`) is the documented superseder of
  the synchronous `ReadPixels` flush — REQUIRED before **framerate-bug anomalies** enter the pool (the flush would
  corrupt the framerate label) and for **exact-under-motion** view-matching. Re-derive L there; do NOT assume L=0
  carries over (G40/G41).
- **Pixel-accurate masks** (custom-depth/stencil) + the **occluded-on-screen** label refinement
  (`GetLastRenderTimeOnScreen`, G22) are deferred — now also motivated by per-frame visible-positive accuracy.
- **Backlog (non-blocking):** flicker on `InstancedFoliageActor` projects a box over the foliage actor's full
  (huge) bounds — a loose/low-value label; flag for a future target-curation pass.
- Next per the roadmap: high-priority new visual bug types (born viewport-aware + auto-injectable + now
  capturable/labelable).
