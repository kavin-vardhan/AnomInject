# Session 012 — Labeled Frame-Capture + 2D BBox Labeling (m7) (2026-06-20)

## Goal
Capture N frames from a running game; TEMPORAL-label which frames contain an anomaly; SPATIAL-label each
affected object with a 2D bounding box. Output = an ML-friendly labeled image sequence. v1 = a LIVE single
labeled-capture stream during an **auto-injection** run; the record-replay clean-vs-injected harness stays
deferred. Built on m6 + the (uncommitted-at-start) control-server Slice-1 capture primitive.

## Locked design (chat-Claude rulings, all approved)
- **L1** — labels come from the INJECTOR'S OWN GROUND TRUTH (we fire deliberately, so we know id/actor/frame),
  not a replay diff. v1 snapshots the auto-injector's live-fire set per captured frame.
- **L2** — 2D bbox = project the affected actor's 3D bounds (AABB corners) to screen space via the SAME
  reversed-Z VP path `AnomalyViewport` already uses. Pixel-accurate stencil masks = deferred.
- **L3** — temporal label is exact iff the ground-truth snapshot is on the SAME frame as the image; tag both
  with a shared frame index.
- **L4** — `IAnomaly` + the injector core stay LOCKED; the only sanctioned growth is a structured readback on
  the auto-injector **shell**.
- **L5** — game-agnostic, public UE APIs, ships-as-a-build (no editor). Dataset capture runs in a packaged
  Development/Test build, where the control-server module is compiled in (`ANOMALY_CONTROL_SERVER=1`).

## Q-rulings (scoping turn)
- **Q1 frame grab** = REUSE the control server's game-viewport `FViewport::ReadPixels` path (real displayed
  frame, packaged-viable, Engine+ImageWrapper). Synchronous-is-a-feature for v1 (exact same-tick alignment).
  Async (`OnBackBufferReadyToPresent` + `FRHIGPUTextureReadback`) = deferred upgrade. SceneCapture2D / MRQ rejected.
- **Q2** — (a) widen the auto-injector readback with the target actor; (b) HOUSE the writer in
  `AnomalyControlServer` (reuses the capture primitive + ImageWrapper + snapshot; no Shipping-capture requirement).
- **Q3** = capture-driven deterministic bursts. **Q4** = PNG-lossless default + JSONL sidecar + run.json. **Q5**
  = defer render-time occlusion. **Q6** = same-tick capture + `GFrameCounter` stamp.

## Three sanctioned core exposures (the only AnomalyInjector touches — IAnomaly/injector/anomalies/leaf helpers/
## `=`-match/`GetVisibleRenderableActors` stay byte-clean)
1. `AnomalyViewport::ProjectActorBoundsToScreenRect(View, Actor, OutMin, OutMax)` — public projection (L2),
   built on the existing private `BuildViewProjectionMatrix` + a new unclamped projector. Unions the actor's
   SM/SK bounds **by type only, not `IsVisible()`** (so hidden `missing_object`/`flicker` actors still project —
   G38); returns the unclamped normalized rect; false only behind-camera/off-screen.
2. `FAutoLiveFireInfo` widened with `TWeakObjectPtr<AActor> TargetActor` + `uint64 StartFrame` (the actor for
   bounds projection + the fire's start `GFrameCounter`).
3. `UAnomalyAutoInjectorSubsystem::RevertAllLiveFires()` — exposed (was the private `RevertAllLive`): reverts via
   the injector AND clears the tracking list, so `GetLiveFires()` stays accurate. Capture drives burst reverts
   through this (NOT the injector's `RevertAll`, which would leave the list stale — S2).

## Stage 1 — foundation + one correctly-labeled frame (GATE 1 PASSED)
- Generalized the capture primitive to `CaptureGameViewportRaw` (BGRA, opaque-alpha — G39) + `CaptureGameViewportEncoded`
  (PNG/JPEG, native resolution, no downscale); old `…Jpeg` is a thin wrapper. *(This generalization physically
  lives in the control-server **Slice-1** commit `ff1be3c` — it is shared infra; see "Entanglement" below.)*
- New `AnomalyLabelWriter` + `IAI.Capture.Shot` — one frame: same-tick `GetLiveFires()` snapshot + capture +
  per-fire bbox projection → one PNG + one appended JSONL record, stamped `GFrameCounter`.
- **A1 finding:** `ReadPixels` returns the already-rendered RT (no fresh draw) + flush → trails the game thread
  >=1 frame. Stage-1's `Shot` is a separate console command from `FireOnce` (different frames), so state is
  on-screen by Shot time — the gate is naturally clean. Owner eyeball: box lands on the object, incl. the
  hidden missing_object "where the hole is" case. Bug found + fixed at the gate: opaque-alpha (G39).

## Stage 2 — burst orchestration + run management (GATE 2 PASSED)
- New `UAnomalyCaptureSubsystem` (Game+PIE, dormant): burst state machine
  `[pre M] -> FireOnce -> [settle K] -> [positives P] -> RevertAllLiveFires -> [settle K] -> [post M]`, looped
  (post-roll doubles as next pre-roll — S3), `BurstCount 0` = until Stop. **Symmetric K-frame settle at both
  boundaries (S1/G37).** Drives the m6 deterministic core via `TryFireOnce`/`RevertAllLiveFires`, never
  `AdvanceTime`. A2 Run-off warn-not-block; A6 zero-match → negatives-only burst. `run.json` manifest at start,
  `run_summary.json` at stop. Console `IAI.Capture.Start/Stop/Status/Config`.
- GATE 2: bounded 3-burst run — correct frame counts (settle frames not written), `present` false→true→false at
  both boundaries (the revert→negative side specifically), boxes land, `run.json` correct, decision-level
  reproducibility across two same-seed fixed-vantage runs (S4 — identical fired id/target, NOT pixel-identity).

## Stage 3 — bbox correct UNDER CAMERA MOTION (GATE 3 PASSED at L=0)
- Per-tick view ring + `IAI.Capture.ViewLag <L>` (distinct from settle-K). Each captured frame projects with the
  view from L ring-entries ago. **Validated default L=0.**
- **The L=0 contradiction, resolved at source (G41):** Stage-3 predicted L=1 (1-frame render lag). Empirically
  L=0 matches, L=1 over-corrects (box trails). Cause: the capture subsystem (a `FTickableGameObject`) ticks at
  `LevelTick.cpp:1606` BEFORE `UpdateCameraManager` at `:1621`, so `GetActiveViewInfo` at the capture tick
  already returns the PREVIOUS frame's camera POV — exactly the view that rendered the `ReadPixels` frame (N-1,
  `r.OneFrameThreadLag=1`). The two 1-frame lags cancel → L=0. "L=0" = "one render-frame back" by tick-order
  convention, NOT zero lag. L↔K coherent (same ~1-frame lag); FPS-invariant (frame-count relationship) → no
  low-FPS re-test. Not pixel-exact under frame-time variance — the async path (G40) supersedes.
- **visible_positive contract (Gate-3 moving-run addendum, G42):** under motion a fired actor can leave the
  viewport mid-hold → `present=true` + all `bbox_valid=false`. KEEP those frames (hard negatives), don't drop.
  Added top-level `visible_positive = present && (any bbox_valid)`; `verify_capture.py` tallies present /
  visible-positive / present-but-off-screen. Occluded-on-screen sub-case = deferred G22.

## Entanglement + Plan A (the commit story)
The cold-boot read the control server's capture primitive from the **working tree** and treated it as committed
— it was the parallel dashboard track's **uncommitted Slice-1 WIP** (committed HEAD `323de4b` had a different
Slice-0 class). m7 was built on it, entangling the milestone (G43). History was verified clean (reflog: nothing
orphaned; Slice-1 only in the working tree). Resolved via **Plan A — two commits:**
- **Commit 1 `ff1be3c`** `feat(control-server): Slice-1 dashboard capture primitive + WS snapshot/protocol` —
  the shared primitive (incl. the raw-BGRA + PNG/JPEG generalization) + the rewritten WS subsystem +
  `ControlProtocol`/`ControlSnapshot` + `spike-client.html` + Build.cs. **No tag.** The parallel track reconciles
  its own dashboard journal against this SHA when it resumes.
- **Commit 2 (m7, tagged)** — the three core exposures + `AnomalyCaptureSubsystem` + `AnomalyLabelWriter` +
  `visible_positive` + ViewLag + `tools/verify_capture.py` + docs + `.uplugin` 0.8.0.
Standalone-buildability (commit 1 has no m7-symbol forward-reference; m7 symbols confined to `AnomalyLabelWriter`)
is what validated the two-commit order.

## Output format (Q4)
Per run: `run_<seed>_<timestamp>/` with `frame_<GFrameCounter>.png` (native res, opaque) + `labels.jsonl`
(one record/line) + `run.json` (manifest at start) + `run_summary.json` (at stop). Record fields: `frame_index`,
`t`, `image`, `width/height`, `anomaly_present`, `visible_positive`, `anomalies[{id, target_name, bbox_px[x,y,w,h]
(clamped), bbox_norm (unclamped), bbox_valid, seconds_remaining, start_frame}]`, `view`. `tools/verify_capture.py`
overlays boxes onto frames + prints a per-frame table + tallies (Pillow).

## Gates
- Clean Development-Editor compile on 5.1 (exit 0) at every step; commit 1 and commit 2 each build exit 0.
- GATE 1 (still): one labeled frame, box on the object incl. hidden case — owner eyeball GREEN.
- GATE 2 (still, bounded 3-burst): frame counts, both-boundary transitions, boxes land, run.json,
  decision-level reproducibility — GREEN.
- GATE 3 (moving): boxes track the object under walk+turn at L=0 (L=1/L=2 over-correct); still-case regression
  auto-preserved (zero motion → identical ring views → L is a no-op); temporal transitions clean under motion —
  owner eyeball GREEN.

## State
- Catalog unchanged at **7** (capture/labeling is infrastructure over the existing catalog — no new anomaly).
  Core deps unchanged (`Core/CoreUObject/Engine/InputCore`); control-server deps unchanged (Json/ImageWrapper/
  WebSocketNetworking). VersionName **0.8.0**. `IAnomaly`/injector core/anomalies/leaf helpers/`=`-match/
  `GetVisibleRenderableActors` byte-clean. Capture is housed in `AnomalyControlServer`, gated by
  `ANOMALY_CONTROL_SERVER` (compiled out of Shipping).

## Hand-off
- m7 closed: commit 1 `ff1be3c` (Slice-1, no tag) + commit 2 `<m7 sha>` tagged **`m7`**. No push (owner owns push).
- The untracked `docs/2026-06-20-011-handoff-vfx-removal-poll-radius.md` was left untouched (pre-existing).
- **For the parallel dashboard track:** Slice-1 was promoted verbatim in `ff1be3c`; reconcile that track's
  journal/handoff against it on resume.
- Next: per the roadmap, the high-priority new visual bug types (born viewport-aware + auto-injectable +
  now capturable/labelable). The async backbuffer capture path (G40) is the prerequisite for framerate-bug
  anomalies and for exact-under-motion view-matching.
