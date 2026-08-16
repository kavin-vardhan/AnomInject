# 2026-08-06 — 030 — S2: render-thread keying design (B′ LOCKED) + building a trustworthy gate environment

Base: plugin `2365808` → `fbf15b1` (docs only). **Production `AnomalyInjector` / `AnomalyCapture` are
BYTE-UNCHANGED for the whole of S2.** Production still captures via the **backbuffer**. **No S3 work has
started.** All instrumentation lives in `Plugins/CaptureBench/` (own local-only git repo, never delivered).

> **Note on numbering.** The chat track ran a numbered amendment series (A1…A41, H1–H3, I1–I10). Those
> numbers exist only in chat. Everything still load-bearing is restated here as plain standing rules so a
> cold reader needs no dangling references. The chat handoff docs
> (`docs/CHAT-HANDOFF-m22-and-sve-s1.md`, `docs/CHAT-HANDOFF-s2-keying-design.md`) carry the narrative.

---

## 1. What S2 was for

Labels today key to a game-thread `GFrameCounter` stamped at **arm** time, and the arm is paired to a
present. An SVE has **no arming step** and runs on the render thread, so the mapping from "this rendered
frame" back to "the game-thread anomaly state snapshot that produced it" had to be **designed, not ported**.

The session split cleanly in two. The first half designed and measured the keying model and succeeded. The
second half discovered that **every packaged validation this project had ever run was in a menu**, and was
spent building a gate environment worth trusting.

---

## 2. THE KEYING DESIGN — B′, LOCKED

**B′ = frame-number-keyed ring, key-only.** The game thread publishes at **`BeginRenderViewFamily`**; the
render-thread pass looks up by **`View.Family->FrameNumber`**.

- **Only the identifier crosses threads.** The label snapshot never leaves the game thread. Matching stays
  in `ProcessCompletedFrames` on the game thread — the same shape as today's backbuffer path.
- **All three state-fill points stay exactly where they are, in order.** A snapshot is currently written by
  three points across two ticks — `CaptureCurrentFrame` mid-tick N (`AnomalyCaptureSubsystem.cpp:966`),
  `FinalizeArmedLabel` at end of tick N (`:1105`, the m18 fix), `SampleDeferredHidden` at top of tick N+1
  (`:1139`, the m20 fix). The migration changes the KEY and the GRAB POINT, **not** that ordering.
- **A lookup miss is loud** — counted, warned, frame dropped, never written with a guessed label. Silent
  failure was the disqualifying property for every rejected option.
- Rejected: **A** (payload on the view family — no sanctioned slot on stock 5.1) and **D** (arm-style,
  render-thread ordered — still pairs by *order*, so it re-inherits the m21 bug class silently).

### The finding that decided the hook — and nearly went the other way

`SetupViewFamily` reports `ViewFamily.FrameNumber == 4294967295` (`UINT_MAX`, its initialised value) —
the number is **not assigned yet**. It is set at `SceneRendering.cpp:4561`, and `BeginRenderViewFamily` runs
at `:4590`, *after* it, on the game thread in the same function. **MEASURED order, every frame:**

```
PROBE,tickstart,            gfc=N
PROBE,postactortick,        gfc=N
PROBE,setupviewfamily,      gfc=N, familyFrame=4294967295   <- NOT set yet
PROBE,beginrenderviewfamily,gfc=N, familyFrame=<real>       <- usable
```

⚠ **Picking the earlier, more obvious hook would have keyed every frame to `UINT_MAX`, silently.** This is
the single most valuable result of S2.

### Other measured facts behind B′

- `familyFrame − GFrameCounter` = **constant 2**, and `familyFrame − GFrameCounterRenderThread` = constant 2,
  across all regimes → the two counters advance **1:1**. (The offset is a startup constant; B′ never uses
  it — matching is by identity.)
- Key ring round-trip: **100% hit** (41/41, 40/40, 41/41) under nominal, game-stall and render-stall.
- Loud failure **proven**: forcing a miss gave **26/26 misses with 26 explicit warnings**, no frame
  labelled by guess.
- Views: `views=1, gameViews=1, isGameView=1, isSceneCapture=0`. ⚠ Measured in a MENU — re-measure in the
  gate level; splitscreen/stereo scoped out.
- TAA jitter is present and **sub-pixel** (max ≈0.375 px). 5.1 exposes `GetProjectionNoAAMatrix()`
  (`SceneView.h:345`) — **use the NoAA matrix for bbox projection.**

### Still open on the design

- **Latch lifetime rule (required, not yet built):** the latch must store the `GFrameCounter` it was set in
  and be consumed only if it matches the current one; mismatch ⇒ discard + count + warn. Otherwise a tick
  with no `BeginRenderViewFamily` leaves a latch that binds to the WRONG frame — the arm→present bug class
  relocated one step earlier, and silent.
- **ViewRing / `ViewLagFrames` deletion** is approved *in principle* but **contingent** on proving the
  captured view's matrices are the ones the pixels used. **That check has NOT been done.** Nothing gets
  deleted until it passes, and it must ship behind a bisect switch.
- **Deep starvation remains OPEN** and does not drift toward "probably fine": the m21 residual was never
  reproduced, so nothing has been shown about whether reading SceneColor mid-render is immune to it.

---

## 3. THE GATE ENVIRONMENT — the second half, and why it took so long

**Every packaged validation before this session ran in MainMenu** (`GameDefaultMap` — G87), because
StackOBot actively bounces `MainWorld` back to the menu. Both escape routes failed: a command-line map
override and a deferred `OpenLevel` both travel and get pulled back; a loose `Config/DefaultEngine.ini`
beside the package is silently ignored (G88).

Resolution: a **synthetic, owned, deterministic bench level** — `CB_GateLevel`, authored by script,
cooked into a bench-only package at `Builds\BenchGate`, reached by command-line map argument so the host
project config is **never touched**. Full operational detail in **G89**.

The saga produced four corrections worth carrying:

1. **The level rendered BLACK** (mean luminance 0.000) and benchmarked at 141.52 fps — *shading nothing is
   cheap*. Cause: script-authored levels are never opened in the editor, so lighting is never baked.
   Fix: fully movable lights + `SkyAtmosphere`.
2. **Auto-exposure was active** (frame-to-frame spread 5.752 → 0.108 when pinned) — but it was **NOT** the
   cause of the overexposure. Two problems that look like one.
3. **A "converged 23.7 ms" result was withdrawn** — measured with PNG encoding running. Clean natural cost
   is **8.52 ms**. Encode contention is not scene cost.
4. **The heavy-gate-level premise was wrong** — `frame_time ≈ stall + 1.3 ms`; a game-thread stall runs
   concurrently with rendering and the scene never enters. The knee sits at ~30–34 ms **regardless of scene
   weight**. The 18–25 ms cost target is **retired**; cost is a recorded property, not a bar.

### Gate level pass conditions (current)

1. **Legible by eye** — targets distinguishable from floor and sky. This is the one that always mattered:
   the oracle is pixel-based and the client's #1 complaint is invisible anomalies.
2. Passes the three content checks (luminance floor / flatness floor / clip ceiling — G89).
3. Deterministic and reproducible from script.
4. Natural cost **recorded, not targeted** — currently 8.52 ms.

The level as it stands meets all four. **Stop tuning it.**

---

## 4. CALIBRATION — game-thread lever (BANKED)

Conditions, all pinned: scene **`CB_GateLevel`** (152 CB_ actors), **VideoFps 30**, output **1280×720**,
exposure pinned (`r.DefaultFeature.AutoExposure 0` + `r.EyeAdaptationQuality 0`), **targeted zero-match**
(clean pacing signal, no fires), 60 frames/leg, dev box.

| stall (ms) | speed_ratio | frame (ms) | band |
|---|---|---|---|
| 0 – 30 | 1.000 | 33.3 | nominal |
| 34 | 1.239 | 41.3 | client band |
| 39 | 1.204 | 40.1 | client band |
| 76 | 2.314 | 77.1 | — |
| 85 | 2.589 | 86.3 | — |
| 99 | **3.004** | 100.1 | deep |

**Knee: 30 → 34 ms, and SHARP.** ⚠ Non-monotonic across 34/39 (1.239 vs 1.204) — run-to-run variance is
~1–2 ms of frame time, the same size as the effect near the knee.
⚠ **The mild band (1.02–1.10] is effectively UNREACHABLE by choosing a stall** — its window sits inside that
noise, and a leg that lands at 1.000 while labelled "1.05" is a silently passing leg.

**⚠ THIS TABLE IS A DEV-BOX INSTRUMENT, NOT A PORTABLE SPEC.** It does not transfer to another machine and
must never be quoted as one. It is how we *reach* regimes on this box — ratio-independence is the
requirement, and this is the tool for testing it, not a claim about anyone else's hardware.

### Leg classification (declared in advance, so binning can never become flexible after the fact)

`nominal [1.00–1.02]` · `mild (1.02–1.10]` best-effort · `client [1.15–1.35]` · `deep ≥2.80` ·
`pacing-off` (own category, ratio recorded not banded).
A leg is **valid** if it passes the content checks **and** its achieved `speed_ratio` is recorded; it is then
**classified by the band it actually landed in**. A valid leg in a gap is kept as data but fills no band.

### Render-thread lever — NOT WORKING

Legs at 20/40/70/110 ms all returned `ratio 1.000` with **`stall_fired = 0`**. The stall originally lived in
CaptureBench's SceneViewExtension, which is inactive during production capture; it was moved to an
unconditional `ENQUEUE_RENDER_COMMAND` from the probe's world tick **and still reports `fired=0`**. Cause
not identified.
⚠ Without the execution counter this would have been reported as "**speed_ratio is blind to render-side
starvation**" — a major, wrong finding. **A lever that never fired proves nothing about the metric.**

---

## 5. Standing rules established this session

- **Measure, then design.** No confident mechanism enters a design without measurement behind it. This bug
  family has been misdiagnosed repeatedly from plausible untested theories.
- **Eyes, then number, then benchmark.** The luminance floor correctly passed a 58%-blown, unusable frame.
  The visual step is not redundant with the numeric one.
- **Marker decode success is NOT proof the scene rendered.** The decoder thresholds cells against its own
  reference cells, so it decodes perfectly on a fully black frame. Permanent property of the oracle.
- **A lever must prove it executed.** Execution counters on any injected stall.
- **State targeted vs auto-pool per leg, and why.** Auto-pool fires concurrent anomaly types and will
  silently mix them into any measurement that assumes one at a time.
- **Levels: check the NAME, never the picture.** MainMenu is a full 3D scene and looks like gameplay.
- **Report READ / MEASURED / INFERRED.** If a measurement contradicts a standing decision, the measurement
  wins, and say so directly.

## 6. Findings parked for other workstreams

- **Shadow leakage (for the invisible-anomaly / stencil track).** Hiding an object changes pixels **outside**
  its bbox — the cast shadow goes with it. Quantified: event-frame whole-frame luminance rises **+1.66 mean,
  4.67 max**. Consequence adopted here: **I10's oracle keys on pixels INSIDE the target's bbox.** Consequence
  for that track: a hidden object whose shadow also vanishes is *more* visible than its bbox implies; one
  with no shadow contribution is *less*. Directly adjacent to the client's #1 complaint.
- **`Builds\BenchGate` is not deliverable.** Bench-only map, restricted cook. Client artifact is
  `Builds\Windows`.

## 7. Hand-off — the immediate next task

1. **Diagnose why the render-thread stall does not fire** (`stall_fired=0` after the rebuild). Ranked
   candidates: (a) the render command is never issued — log unconditionally at *issue* time, not only inside
   the lambda; (b) the counter/log gating is itself the bug (`FThreadSafeCounter::Increment()` return
   semantics vs the `%60` filter) rather than the stall; (c) something in the capture path bypasses or
   flushes it.
2. Calibrate the render table — **or**, if the stall provably fires and the ratio still does not move, that
   is a major finding (`speed_ratio` blind to render-side starvation) → **stop and report**, do not work
   around it.
3. Retroactive audit of banked results, paper only, two axes: *was it menu-bound?* and *did anyone verify the
   frames had content?* Default classification **WEAKENED** — each result argues its way out. Include the m22
   gates and anything client-facing validated only in a package.
4. Re-measure the view-count fact in the gate level.
5. **I10 — reproduce the client's defect on the CURRENT backbuffer path**, both levers, targeted
   single-anomaly hide-type legs, marker on every frame, oracle inside the target bbox, every leg recording
   achieved `speed_ratio`. Then the high-`VideoFps` legs (120/240, pacing ON) that reproduce the shape m21's
   residual was actually observed in.

**Nothing about the migration is proven until I10 shows the defect reproducing and then not reproducing.**
Everything so far shows the *new path is well-behaved*; nothing yet shows it **fixes the reported defect**.
