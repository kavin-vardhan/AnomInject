# 2026-07-15 — 025 — m19: preview backbuffer tee + new targeting defaults

*(Owner bundled two strands into m19: the preview backbuffer re-plumb, and three targeting-default changes. The
defaults strand is documented at the end of this journal; the preview strand is everything above it.)*

## Part 2 (bundled) — new targeting defaults: coverage 6%, poll radius 18 m, pool = blinking + missing_texture

**Step 1 — mechanism report (what these were BEFORE):** all three were **hardcoded engine constants**, none ini-backed,
and the engine is the only source of truth:
- `AnomalyViewport.cpp:30` — `float GPollRadius = 0.0f;` → **cull OFF**. Units: **centimetres**.
- `AnomalyViewport.cpp:34` — `float GMinScreenCoveragePct = 0.0f;` → **cull OFF**. Units: **percent 0–100** (not 0–1).
- `AnomalyAutoInjectorSubsystem.cpp:50-53` — `Initialize` enabled **every** id in `GAutoPool`
  (`{missing_object, blinking, missing_texture}`).
- **Parity check — answered: they are NOT set in two places.** The dashboard defines **no defaults** for any of the
  three; its sliders and pool checkboxes are pure snapshot mirrors (`session.pollRadius`, `session.minScreenCoverage`,
  `auto.pool[id]` ← `Auto->IsAnomalyEnabled`, `ControlSnapshot.cpp:189`), `store.ts:115` initialises `snapshot: null`,
  and `SessionBar` only renders after `everConnected` (`App.tsx:31`). So the cleanest change is **engine constants
  only** — and it is also the only *correct* one: adding a dashboard-side default would create a second source of truth
  that could drift from the engine. (The `?? 0` in `SessionBar.tsx:92-93` is a pre-first-snapshot placeholder.)

**Step 2 — applied (engine only; cleanest form = flip the constants; no new ini key):**
- `GPollRadius = 1800.0f` (18 m in cm) and `GMinScreenCoveragePct = 6.0f`; both console help strings corrected —
  they advertised "(default OFF)" and would otherwise have been lying. *(Coverage was first applied as 12% and the
  owner reduced it to 6% after the measurement below showed 12% culls the test scene's hero character.)*
- New `GAutoPoolDefaultEnabled[] = { blinking, missing_texture }`, consumed by `Initialize`. `GAutoPool` is unchanged,
  so `missing_object` stays **selectable, just not enabled by default**, and `SetAllAnomaliesEnabled(true)` still means
  all of `GAutoPool` (an explicit action, not a default). `Initialize` now logs the default pool.
- Ranges/behaviour/m13 optimism untouched. Both values land exactly on a dashboard slider step (poll `step=100`,
  coverage `step=1`).
- **Not ini-backed** — flagged as an available follow-up if per-title tuning is wanted (the console + dashboard already
  give per-session overrides).

**Step 3 — validation (fresh PACKAGED session, no dashboard — the authoritative check):**
```
IAI.SetPollRadius: current radius = 1800.0 cm (cull ON)
IAI.SetMinScreenCoverage: current = 6.00% (cull ON)
IAI.Auto.Status ->  enabled pool (2): blinking, missing_texture
AutoInjector subsystem initialized ... Default pool: blinking, missing_texture.
```
Dashboard parity (same session, snapshot the UI binds to): `session.pollRadius = 1800` → slider shows **18 m**;
`session.minScreenCoverage = 6` → **6%**; `auto.pool` = `[x] blinking, [x] missing_texture, [ ] missing_object`
(+ `[ ] lod_corruption`, `[ ] lod_popping`, which are catalog entries outside `GAutoPool` — pre-existing, unchanged).
**PASS with zero dashboard changes.** Controls still drive the engine (verified by toggling both culls to 0 and back
over the WS surface during the check).

**Why 6% and not 12% — measured, not guessed.** Coverage swept on StackOBot's MainMenu with the poll cull OFF (so
coverage is isolated):

| coverage | eligible targets | Bot (`SkeletalMeshActor_3`) in set? |
|---|---|---|
| 0 % (pre-m19) | 15 | yes |
| **6 % (shipped)** | **5** | **yes** |
| 10 % | 4 | no |
| 12 % (first proposal) | 4 | no |

The Bot occupies **9.98%** of the viewport (corroborated by the m19 capture's `annotation.json`
`coverage_ratio: 0.10026` for that exact actor), so 10% or 12% **culls the hero character of the test scene**; 6%
keeps it with ~1.7× headroom. The owner reduced 12→6 on this evidence.

**⚠ The MainMenu eligible set is 1 with both culls on — and that is the POLL RADIUS, not coverage, and it is a
MENU-MAP ARTIFACT rather than a defect.** Isolated: coverage-6%-only → 5 targets (Bot included); poll-18m-only → 1
target. The poll cull is measured from the **player pawn** (G34), and a menu map's pawn is nowhere near the menu
vignette's camera, so everything on screen is >18 m from it. (Proof: the camera sits at `[3837, 2525, 1700]`, ~3 m
from the Bot — were the poll origin the camera, the Bot would pass easily.) The lone survivor, `StaticMeshActor_0` at
122 m, escapes only because the cull subtracts the bounds sphere radius, so huge actors are never distance-culled. In
a real gameplay level the pawn IS the player under the camera and 18 m is meaningful — **so the poll-radius default
cannot be judged in this menu map, and is worth an owner sanity check in an actual gameplay level / on Concorde.**
Targeted capture is unaffected either way (visibility-independent, G61). Recorded as G80.

---

# Part 1 — preview backbuffer tee (fix the packaged black preview; retire the synchronous preview read)

Base: plugin `4559c8c` (tag `m18`, in sync with origin), dashboard `f978f1b` (**untouched — no dashboard change needed**).
Plan approved in the preceding turn; this journal covers the implementation. **No commit this turn** — owner review first.

## Goal

The dashboard preview is black in ANY packaged build (reproduced on the local StackOBot package). Captures are fine
because the capture path reads the BACKBUFFER. Re-plumb the preview onto that same backbuffer stream, throttled to
preview cadence and JPEG-encoded off-thread, replacing the viewport `ReadPixels` read.

## Two premise corrections (recorded so the record is accurate)

The approved fix DIRECTION was right; two stated facts about the bug were not, and one of them would have let a
broken build pass its own gate. Both were re-checked against artifacts before implementing:

1. **The preview path was NOT editor-only gated.** There is no `WITH_EDITOR` (or equivalent) guard anywhere in
   `AnomalyPreviewCapture.{h,cpp}` or its call site. The only guard is `#if ANOMALY_CONTROL_SERVER`
   (`AnomalyControlServerSubsystem.cpp:14…`), which is `=1` for every configuration except Shipping
   (`AnomalyControlServer.Build.cs:24-30`). The packaged Development build therefore COMPILED AND RAN the preview.
   ⇒ There was no editor-only guard to remove, and nothing "replaces" it: the not-Shipping guard is correct and stays.
2. **Frames were NOT "never generated" (counter stuck at 0).** Measured live on the packaged build: 117 frames in
   20 s (~5.8/s), frameId incrementing 233→349, every JPEG **exactly 15027 bytes**, decoding to a valid 1280×720
   image with mean brightness **0.00**. Frames were generated, encoded and SENT — they were BLACK. (Packaged viewport
   has no render target → `GetRenderTargetTexture()` null → `FD3D12DynamicRHI::RHIReadSurfaceData` zero-fills and
   returns success → `ReadPixels` reports true. The "succeeds-all-black" variant.)
   ⇒ **Gate consequence:** "frame counter increments" PASSED on the broken build. Every m19 gate is pixel-based.

## What was built

**Preview tees off the same `OnBackBufferReadyToPresent` stream as capture, with its own arm list.** It cannot share
capture's *grab*: m16 suppression makes preview and capture mutually exclusive in time (when capture is grabbing,
preview must be silent — so there is never a capture frame to tee from), and `FAnomalyFrameCapturer`'s arm-matching
(`AnomalyFrameCapturer.cpp:67-72`) and completed queue (`:176-186`) are single-consumer — one shared instance would
have the two eating each other's frames across a capture-start boundary. So the preview owns a **second
`FAnomalyFrameCapturer` instance** bound to the same delegate; `FAnomalyFrameCapturer` itself is **unmodified**, which
is what keeps capture byte-identical. Both bindings early-out when their own arm list is empty; in practice only one
is ever armed, because of the m16 exclusivity.

- **`Private/AnomalyPreviewTee.{h,cpp}` (NEW)** — `FAnomalyPreviewTee`: owns the preview capturer + a shared,
  CS-protected "latest JPEG" slot. `Arm()` (game thread), `Pump(bSuppressed)` (game thread: `EnqueueDrain`,
  `PopCompleted`, dispatch encode), `PollJpeg()`, `IsBusy()`, `DiscardReady()`. Logs the backbuffer format once per
  session (the G5 diagnostic).
- **`Public/AnomalyCaptureSubsystem.h` + `Private/AnomalyCaptureSubsystem.cpp`** — owns the tee (created lazily on
  first arm; reset in `Deinitialize`) and exposes a 3-method game-thread facade: `PreviewPump()`,
  `PreviewArm(ViewEpoch)`, `PreviewPoll(Jpeg, W, H, Epoch)`. All render/RHI stays inside `AnomalyCapture`; the control
  server gains **no render dependencies** — module DAG (core ← AnomalyCapture ← ControlServer) preserved.
- **`Private/AnomalyLabelWriter.{h,cpp}`** — `ConvertTightToBGRA` promoted from a file-local anonymous namespace into
  `namespace AnomalyLabel` + declared in the header. **Pure move, no behavior change.** This is what makes the preview
  inherit capture's proven format handling instead of owning a second format model.
- **`Private/AnomalyControlServerSubsystem.cpp`** — `Tick` calls `Cap->PreviewPump()` each tick. `PushFrames` keeps
  the m16 suppression gate and the subscriber check EXACTLY, and replaces the synchronous
  `CaptureGameViewportJpeg` with poll-ready-JPEG → send → arm-next.
- **`AnomalyPreviewCapture.{h,cpp}`** — `CaptureGameViewportJpeg` DELETED (the preview was its only caller; no dead
  code). `EncodePixels` kept (used by the tee and the label writer); `CaptureGameViewportRaw`/`Encoded` kept — still
  used by the SYNC capture fallback via `CaptureLabeledShot`.
- **Dashboard: NO CHANGE.** The AIF1 wire format (magic + frameId + epoch + w/h + JPEG) is unchanged.

### Suppression (m16 / G73) — the gate sits at the ARM, not just the send

`PushFrames` still early-returns on `IsCaptureActive()` (no SEND). Additionally `PreviewArm()` refuses while
`IsCaptureActive()`, so during a capture there is **no new grab, no readback, no encode, no send** — gating only the
send would have left the tee grabbing and encoding during capture, i.e. fighting capture. `Pump()` still DRAINS while
suppressed (so an in-flight readback armed just before a capture start is reclaimed and cannot leak) but discards the
result instead of encoding it; `PreviewPoll` also discards any ready frame while active. Suppression is KEPT after
m19 by choice: concurrent preview readbacks would compete with capture's for readback bandwidth and the worker pool
and could perturb m11 pacing, violating "capture byte-identical".

### Throttle / threading

Arm cadence is unchanged: the existing `FrameIntervalSec` from the `subscribe` message (clamped 0.5–10 Hz; the
dashboard asks for 6). The hook fires per present but early-outs with no arm pending; there is no per-present encode
or send. Extra guards at the arm: authed frame-subscriber exists, capture inactive, and `IsBusy()` (no pile-up).
Threading mirrors capture exactly — render thread does only `EnqueueCopy` and the drain memcpy; the game thread pops
and dispatches; a **background task** does `ConvertTightToBGRA` + `EncodePixels(JPEG q60)` into the slot; the game
thread polls and sends (the WS socket is not thread-safe). **No `FlushRenderingCommands` anywhere in the new path.**
Cost: preview frames land ~1 cadence period after their arm — invisible at 6 Hz.

### Refinement carried from the plan

`ViewEpoch` is now stamped at **arm** time and carried through the tee, instead of read at send time. With async
latency a resize between arm and send would otherwise attach a NEW epoch to an OLD-size image, and the dashboard's
`frame.epoch !== snapshot.epoch` overlay guard (`PreviewCanvas.tsx:33`) would wrongly draw overlays on a stale frame.

## Gates — in a local package (`Builds\MidRepro\Windows`), headless over the WS surface

- **G1 PACKAGED PREVIEW WORKS — GREEN (the headline).** Pixel-based, not counter-based. 87 frames in 15.1 s
  (5.77/s), frameId 1→87, JPEGs ~70.7–71.4 KB and **all 60/60 distinct** in a 10 s sample — versus the pre-fix
  signature of a **constant 15027 B**, mean 0.00. The decoded frame shows the real scene (Bot, environment, menu UI).
  Artifact: `scratchpad/first_frame.jpg`. The `Ensure condition failed: InRHITexture` that fingerprinted the old
  path is **gone from the log**.
- **G2 SUPPRESSION HOLDS — GREEN.** Bucketing preview frames by the snapshot's own `capture.running`: **13.2 s with
  capture RUNNING → 0 preview frames**; 8.8 s idle → 51 frames (5.8/s). Preview resumes after the run.
- **G3 EDITOR (PIE) — OWNER-PENDING.** Cannot be run headless in a package; needs the editor. Risk is low but
  unproven for the preview specifically: the tee uses the *same* hook, the same `ComputeGameViewportCapture` rect
  logic and the same (unmodified) capturer class that the async CAPTURE path already exercises in PIE across
  m10–m16, and the new path contains no editor-only calls. Owner recipe: open PIE, `IAI.Server.Start`, connect the
  dashboard, confirm a live (non-black) preview and that overlay boxes still align.
- **G4 CAPTURE REGRESSION — GREEN (byte-identical to m18).** Same targeted capture, m18 baseline vs m19: frames
  100/100, positives 65/65, negatives 35/35, identical label pattern
  (`...PPPPPPPP....PPPPPPPP....` ×8), identical gframe cadence `[0,1,2,3,6,7,8,9,10,11,12,13,16,17,18,19]`,
  identical `annotation` `start=3 end=10 count=8 indices=[3..10]`, 9 events both. No pacing warnings.
- **G5 FORMAT — GREEN on StackOBot; Concorde is an OWNER POST-PUSH CHECK (documented limitation).** The tee logs
  `Preview(tee): first backbuffer frame (fmt=18, bpp=4, rect=1280x720)` — `PF_B8G8R8A8`, one of the four handled
  formats, and the preview renders correctly. **Not claimed as universal:** see the limitation section below.
- **G6 THREADING / FRAME-TIME — GREEN structurally; honest numbers.** No render-thread stall introduced (encode and
  send are off the render thread; the only render-thread work is `EnqueueCopy` + the drain memcpy — capture's
  existing pattern). Measured on the local menu scene: ~91.3 fps with no subscriber vs ~86.7 fps with the preview
  subscribed at 6 Hz. **That ~5% is inside the noise floor** — the no-subscriber baseline itself ranged 67.9–105.4
  fps across intervals — so it is reported as "no measurable regression", not as a number to trust.

## HONEST PERF NOTE (accepted correction 1) — this does NOT speed up capture

m16's suppression already stops preview generation entirely from arm→finish, so the preview could not have been
dragging capture before this change. **m19 therefore delivers no capture-time speedup, and none is claimed.** What it
delivers: (a) a working preview in packaged builds — the headline; (b) removal of the ~6 Hz game-thread
`FlushRenderingCommands` + game-thread JPEG encode that the old `FRenderTarget::ReadPixels` path performed
**outside** capture, i.e. during normal play with the dashboard open (the frozen-dashboard/perf-debt half). The (b)
benefit is **structural** — verified in code — and was **not measurable on the local light menu scene** (see G6
noise). A loaded scene (the office box) is where it would be felt. Do not restate this as "improves capture
performance".

## KNOWN LIMITATION (accepted correction 2) — Concorde / HDR format is a post-push owner check

The tee reuses `AnomalyLabel::ConvertTightToBGRA` (`AnomalyLabelWriter.cpp`), which handles `PF_B8G8R8A8`,
`PF_R8G8B8A8`, `PF_A2B10G10R10`, `PF_FloatRGBA`, and **returns BLACK from its `default:` branch for anything else**.
- **Validated:** StackOBot packaged (`fmt=18` = `PF_B8G8R8A8`) — preview correct. **This is NOT universal coverage.**
- **Structural safety net:** the preview now shares the capture path's conversion, so any title whose captured PNGs
  are correct has a handled format and will preview correctly. Concorde's captures are good ⇒ its format is one of
  the four ⇒ the preview *should* be correct. That is an inference, not a measurement.
- **Genuinely open, owner post-push check on the office box (mirrors the m17 pattern):** whether Concorde's real
  swapchain (HDR / exotic format) previews correctly. Two failure shapes: an unhandled format → BLACK preview (would
  also blacken their captures); a true HDR `PF_FloatRGBA` swapchain → the naive linear×255 clamp
  (`AnomalyLabelWriter.cpp`, the `PF_FloatRGBA` case) has no tonemap → washed/dark but NOT black, matching how their
  captured PNGs already look.
- **EXACT PLACE TO ADD A CONVERSION STEP IF NEEDED:** `AnomalyLabel::ConvertTightToBGRA` — add the new `case` (or a
  tonemap in the `PF_FloatRGBA` branch). Because capture and preview now share this one function, fixing it there
  fixes **both**. The one-line `Preview(tee): first backbuffer frame (fmt=…)` log makes identifying the format a
  5-second check on the box.

## Invariants held

`IAnomaly`/injector/anomalies BYTE-CLEAN. `FAnomalyFrameCapturer` and `FAnomalyAsyncWriter` unmodified. Capture's
grab/labeling/pacing/delivery and the 1:1 mapping unchanged (G4 proves it). No new module dependency; the control
server gains no render deps. No editor-only calls in the new path. Catalog stays 8. Source comment-free.

## Still open / not addressed

- **The SYNC capture path still writes BLACK frames in a package** (`IAI.Capture.Async 0`; same `ReadPixels` root —
  it keeps `CaptureGameViewportRaw`/`Encoded` alive). m19 does not fix it; async remains the only working packaged
  capture path. Proposed cheap guard (owner call): a one-time Warning at `StartRun` when `!bAsyncCapture && !GIsEditor`.
- The async **view/bbox** staleness under camera motion (m18's open half) and the bbox-from-live-bounds-at-drain-time
  issue are untouched here.

## State

Code + docs written; G1/G2/G4/G5/G6 green in a local package, G3 owner-pending (PIE). **No commit.** On acceptance:
strip comments (already clean), commit as one `feat(capture)` or `fix(capture)` + tag `m19`.
