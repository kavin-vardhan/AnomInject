# 2026-07-29 — 028 — CaptureBench S1 re-run (blocked) + production traceability quick-fix plan

Base: plugin `ed2b851`, tree **CLEAN**. **No production code touched this session.** Measurement +
planning only, per brief.

## Context recorded this session (load-bearing)

1. **NEW REQUIREMENT — label correctness must be ratio-independent.** A real client session at
   `speed_ratio` **1.2** showed a confirmed **−1 label lag** on a `missing_object` event boundary
   (effect at frame 50, annotation starts 51) — the **1.1–1.5 band we never measured**. m21 validated
   ratio ≈1.0 (exact) and ratio ≳3 (residual), so this band sat between the two measured points.
   Clients capture on their own hardware and **will not be asked to discard or re-capture sessions**.
   ⇒ The internal ship rule (`speed_ratio ≤ ~1.05`) is **DEMOTED from correctness gate to internal
   telemetry**, and retires entirely when the SVE migration lands.

2. **Decisions from chat Claude (final) on the CaptureBench evaluation report:**
   - **Q1 YES** — UI on/off ships as a **grab-point choice**: SVE = UI-free, backbuffer = UI-on. No
     UI-isolation work (SDR has no isolated UI layer; `GetCompositeUIRenderTarget()` is HDR-gated —
     `SlateRHIRenderer.cpp:980-991`). **Ground-truth contract AMENDED: UI presence is per-run config;
     proposed delivered default = UI-off, pending client sign-off.**
   - **Q2** — 8-bit delivered color stands. The typed FP16/FP32 path lands **with depth (S3)**, not before.
   - **Q3** — SVE migration **opens now**; it **SUPERSEDES the parked m22 scene-identity-marker**
     proposal (same problem, stronger solution).
   - **Q4** — CaptureBench is **kept permanently** as a non-shipping perf-regression harness (own
     plugin, gated, never ships).
   - Staging **S1→S4 approved**. Standing note for S2/S4 gate design: label gates must include
     **starved regimes** AND the **wall + delivery** client config.

3. **DOC ACCURACY FLAG:** `CLAUDE.md` "Current status" still says m21 is *built but NOT committed*.
   Verified false — **m21 is committed as `a2c3127`, tagged, and is an ancestor of HEAD `ed2b851`**;
   the tree is clean. The status block predates the dashboard M1–M3 / delivery-docs work that has since
   landed. Cold-start bootstrapping is currently misleading on this point.

## TASK A — S1 expanded re-run: **COMPLETE (measured)**

All rows: **100% coverage, 0 gaps, 0 dropped, content-identity probe conclusive (distinct == captured,
0 consecutive duplicates) in EVERY row** — Standalone, packaged, and all starve legs, both methods.

| Leg (W0 = capture-only) | fps | SVE hook mean/max ms | BB hook mean/max ms | ratio (mean) |
|---|---|---|---|---|
| Standalone 720p W1 | 54.6 / 46.8 | 0.0063 / 0.0384 | 0.7089 / 2.2101 | 113x |
| Standalone 720p W0 | 101.8 / 104.9 | 0.0044 / 0.0224 | 0.6477 / **7.4088** | 147x |
| **Packaged** 720p W1 | **40.3 / 13.7** | 0.0062 / 0.3132 | 0.4978 / 7.2667 | 80x |
| **Packaged** 720p W0 | 107.9 / 107.8 | 0.0038 / 0.0142 | 0.6568 / **11.2844** | 173x |
| Starve 1080p (74 fps) | 74.3 / 74.1 | 0.0065 / 0.0316 | 0.8731 / **18.2330** | 134x |
| Starve SP170 (41 fps) | 40.7 / 41.1 | 0.0051 / 0.0208 | 0.6583 / 1.3893 | 129x |
| Starve SP320 (14.6 fps) | 14.6 / 14.8 | 0.0042 / 0.0059 | 0.6525 / 2.0236 | 155x |

**Headlines.** (1) SVE hook cost is ~**80–173× cheaper** and, more importantly, **flat**: its max never
exceeded **0.31 ms** in any run, while the backbuffer spiked to **7.3 / 7.4 / 11.3 / 18.2 ms**. At 105 fps
(9.5 ms budget) a 7.4 ms spike eats ~78% of a frame; 18.2 ms is longer than a whole 60 fps frame. That is
the measured mechanism behind "smoother". Cause: the backbuffer path calls `EnqueueCopy` on the **immediate**
command list inside the present hook; the SVE appends a deferred `AddEnqueueCopyPass` to the render graph.
(2) **Throughput is a tie** at W0 (101.8 vs 104.9; 107.9 vs 107.8; 14.6 vs 14.8) — the SVE advantage is
**not** throughput. (3) **NEW, packaged-only, observed once:** at W1 (inline PNG on) the packaged backbuffer
row collapsed to **13.70 fps vs SVE 40.28**, with mean encode **863 ms vs 288 ms** — i.e. under the CPU
contention that inline encoding creates, the backbuffer hook degrades ~3× worse. Standalone showed only a
~15% gap. **Single observation — needs a repeat before being leaned on.**

### Two corrections to earlier reporting
- **Readback latency:** PIE showed SVE 0.00 vs BB 1.00 frames and I floated it as an SVE advantage. **PIE
  artifact.** At real frame rates both sit at 1.73–2.00 frames in every leg. No readback-depth advantage.
- **Resolution semantics:** I had warned SVE captures at `SceneColor.ViewRect` so screen-percentage would
  desync it from output res, and used that to reject `r.ScreenPercentage` as an unfair starve lever.
  **Measured false** — at SP 170 and SP 320 the SVE frame stayed exactly 1920×1080, identical to the
  backbuffer. The grab point (after `VisualizeDepthOfField`) is **after the upsample**, i.e. at OUTPUT
  resolution. So the lever was fair, and that migration caveat is **withdrawn** for this grab point.

### ⚠ Limitation that bounds what A2 proves
**CaptureBench free-runs — no fixed timestep, no m11 pacer.** Production's `speed_ratio` regime is defined by
the pacer + fixed timestep against the wall clock. A2's "starvation" is frame-time overrun of a free-running
game, which is **not** that regime. ⇒ **The client's 1.2-band −1 lag can be neither reproduced nor refuted by
this harness**; ten green rows must NOT be read as "the 1.2 band is fine." That question belongs to the
S2/S4 gates driven through the **production** capture path with pacing engaged. Also honest: the ≥3× target
was **not reached** — `r.setres` is silently clamped to the desktop resolution (2560×1440 → 1920×1080), and
SP 320 bottomed out at 14.6 fps = **2.05×** overrun.

### Method notes
- `A1(ii)` packaged ran via **game-target build + exe hot-swap** into `Builds\Windows` (G76), no re-cook —
  confirmed correct: the package is monolithic and CaptureBench ships no content.
- Starve lever ended up as `r.ScreenPercentage` (170 / 320) after `r.setres` proved desktop-clamped.

## TASK A — original blocked state (superseded above, kept as the record)

Everything needed to run it unattended is **staged and written**, but the machine could not build.

**Blocker (environmental, not code):** two `UnrealEditor` processes were resident (~6.0 GB), leaving
**~500 MB free of 24 GB**. UBT therefore capped itself to **one parallel action** (`Requested 1.5 GB
free memory per action, 1.18 GB available: limiting max parallel actions to 1`) for a **2517-action**
rebuild — hours of wall time, and the link would then have failed on the loaded
`UnrealEditor-CaptureBench.dll` (the same Live-Coding class of failure hit earlier in the day). Build
was stopped deliberately rather than burn the session.

**Unblock:** fully exit both editor/game processes, then rebuild. Nothing else is outstanding.

### What was built for A (CaptureBench only — throwaway/harness plugin, never ships)

- **`CaptureBench.Matrix [frames] [exitWhenDone] [startDelaySec] [label]`** — runs the whole A/B
  unattended: `sve/W1 → backbuffer/W1 → sve/W0 → backbuffer/W0`, resolving the game world per row,
  waiting out each run, and optionally calling `RequestExit` when done. This makes both the Standalone
  and packaged legs **headless and log-parseable** (`-ExecCmds` + `-abslog`), which is what A1 needs.
- **Content-identity detection (A3)** — a sparse per-frame pixel hash (~8k samples, negligible RT cost)
  in `Drain_RenderThread`, reporting **distinct frames** and **consecutive duplicates**. This is the
  cheap empirical probe for "backbuffer presents stale content under starvation". It **self-reports
  INCONCLUSIVE when `distinct <= 1`** (static scene = nothing to detect), so it cannot produce a false
  positive result. Judged trivial (~20 lines) and therefore in scope per the brief.
- Extended CSV row: `mode,W,H,renderSpan,captured,coverage,gapEvents,gapFrames,maxGap,wall,renderFps,
  meanHookMs,maxHookMs,meanLat,meanCopyMs,write,distinct,dupes`.

### Planned run design (unchanged, ready to execute)

- **A1(i) Standalone:** `UnrealEditor.exe <uproject> -game -windowed -resx=1280 -resy=720
  -ExecCmds="CaptureBench.Matrix 300 1 8 std" -abslog=<log>`.
- **A1(ii) Packaged Development:** the package at `Builds\Windows` is **monolithic** (verified: no
  per-module DLLs; `Anomaly*.dll` absent) and does **not** contain CaptureBench. Per **G76** this needs
  a **game-target build + exe hot-swap** (`StackOBot Win64 Development` → copy
  `Binaries\Win64\StackOBot.exe` into the package), **not** a full re-cook — CaptureBench ships no content.
- **A2 starved regimes:** lever = **`r.setres`**, deliberately **not** `r.ScreenPercentage`.
  Screen percentage changes SVE's `SceneColor.ViewRect` but **not** the backbuffer size, so the two
  methods would be capturing different pixel counts and the copy-out comparison would be unfair.
  `r.setres` scales **both** equally. Targets expressed as overrun vs a nominal 30 fps budget:
  **~1.2×** (≈25 fps) to sit in the client's reported band, and **≥3×** (≤10 fps) for deep starve.

## TASK B — production quick-fix plan (file-by-file, awaiting approval)

Capture-method-independent; survives the SVE migration. **No code written.** Full plan ferried to chat
in this session's report. Summary of the three items and their real costs:

- **B1 traceability** (`AnomalyLabelWriter.{h,cpp}` + `AnomalyCaptureSubsystem.cpp`): add mesh **asset
  name**, **component class**, and **world bounds** to `affected_objects.nodes[]`. Straightforward —
  `FSessionNode` gains fields, `AccumulateFrameEvents` fills them at the existing anchor-frame branch
  (`:1333-1349`), `WriteSessionAnnotation` emits them (`:366-373`).
- **B2 selection provenance** (adds `AnomalyViewport.{h,cpp}`): record screen-coverage %, occlusion
  pass-rate, poll distance per fired event. **⚠ Non-trivial:** coverage and distance are cheap
  (computed then discarded), but **an occlusion "pass-rate" does not exist today** — the trace
  early-outs on the first unblocked sample (`AnomalyViewport.cpp:134-140`), so a rate requires removing
  that short-circuit = up to 9× the traces on the hot targeting path. Plan proposes making the full
  count **opt-in**, defaulting to the current cheap boolean.
- **B3 — CANCELLED, removed from scope** (owner correction). The client session folder lives on a different
  machine and cannot be brought here. **Client-session inspection is OFFICE-MACHINE-ONLY work.** Do not
  re-ask for the path.

- **B3' local subtype regression smoke — RUN, PARTIAL PASS (report-only, nothing fixed).** Two packaged
  captures (`IAI.Capture.Start … blinking`, 120 frames, seed 12345), analysing `annotation.json`: count
  contiguous runs in `affected_frames.frame_indices` and compare against the emitted `anomaly_subtype`.
  - **PASS (flicker direction): 2/2 blink events, `runs=2 → "flicker"`, matching in both runs.** Indices
    `[4,5,9,10]` and `[40,41,45,46]` — note `[4,5,9,10]` reproduces the m20 G2 gate case exactly. **The m20
    Bug C fix is intact: no regression.** No mismatch between derived transitions and emitted subtype in any
    observed event.
  - **NOT EXERCISED (disappear_reappear direction): could not drive a single-run event.** `blinking` is fixed
    at `DefaultHz = 5.0` (0.1 s half-period ≈ 3 frames at 30 fps) and **the capture entry point cannot pass
    anomaly args**, so Hz is not settable through `IAI.Capture.Start`. `IAI.Capture.Config 2 2 4 2` vs
    `2 2 8 2` produced **byte-identical hidden index sets**, so the positive-window knob did not change the
    captured pattern either. ⇒ the ≤2-transitions branch was not re-proven this session (m20 G3 did prove it,
    14/14). **To close this properly the capture path needs a way to pass anomaly args (e.g. Hz) — flagged,
    not built.**
  - **Consequence for the client complaint:** as far as this evidence goes, the derivation is *correct* —
    2 hidden runs genuinely is multi-toggle, and "flicker" is the right label for it. That supports the
    reading that the client's complaint is **VOCABULARY, not a labeling bug**, which is what the pending
    flat-taxonomy proposal addresses. Stated as support, not proof: the other branch is unverified here.
  - **Two incidental re-confirmations.** (1) **G81 still ships:** `frame_count: 7` alongside only **4**
    `frame_indices` — the span-vs-count defect is live in the client-facing file, unchanged and still
    owner-gated. (2) The packaged build boots into the **MainMenu** map and the only blink target was
    `StaticMeshActor_0` there — this is exactly the m19/G80 menu-map artifact, so B3' did **not** run in a
    real gameplay level.

## Hand-off / next action

1. Close both `UnrealEditor` processes → rebuild → run A1/A2 (fully automated via `CaptureBench.Matrix`).
2. Approve/adjust the Task B plan (note the B2 occlusion-cost fork).
3. Supply the client session folder path to close B3.
4. Consider refreshing `CLAUDE.md` Current status (stale m21 claim, item 3 above).
