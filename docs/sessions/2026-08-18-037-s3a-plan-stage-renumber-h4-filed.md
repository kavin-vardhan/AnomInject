# 2026-08-18 — 037 — S3 starts: stage renumbering, H4 filed, and the S3a implementation plan

**Plan-only turn for S3a — no capture code written.** What *was* written this turn: the
`make_gate_level.py` frozen-level guard (its own commit in `CaptureBench`, `8dad64e`) and the docs
below. Probe untouched; `CaptureBench` probe sources unchanged.

---

## 1. Rulings recorded this turn

### 1.1 A47 — AMENDED

**Original bbox ruling UNCHANGED:** per-leg settled bbox + settle window; no design may assume a
fixed bbox across legs.

**Amendment:** the bifurcation is in camera **ROTATION**, not position. Measured across **369**
gate-level event camera samples: position invariant at `(-1500, 0, 260)` — `CB_PlayerStart` — on
**369/369**; rotation modal `(0,0,0)` on **278/369 (75.3 %)** with the remainder scattered across 41
other yaw/pitch values.

**New clause:** *inter-actor occlusion is invariant across the bifurcation*, because it depends only
on the eye position and static geometry, and rotation changes only frustum membership.

⚠ **DO NOT GENERALISE.** This holds **because** the gate-level targets are all
`ComponentMobility.STATIC` **and** the player start is fixed. It **fails in any level with motion** —
a moving occluder, a moving target, or a driven camera each break it independently. The clause is a
property of `CB_GateLevel`, not of the engine.

### 1.2 H2 — RETIRED-UNKNOWN

The hypothesis ledger now reads:

| # | Hypothesis | Status |
|---|---|---|
| **H1** | GPU-load starvation shape | OPEN, untested — no lever exists |
| **H2** | *(unknown)* | **RETIRED-UNKNOWN** — appears nowhere in this repo; history unrecoverable. **Never re-mint this number.** The entry exists solely so nobody reclaims it |
| **H3** | Auto-exposure active | OPEN, likely, unconfirmed |
| **H4** | Occlusion-blind labelling | **OPEN, NAMED, NOT ADOPTED** — see §2 |

### 1.3 P6 — WIDENS, NO NEW NUMBER (A61 applied to phenomena, not just verdict buckets)

**P6 = "annotation.json field-contract defects"**, three instances:

| instance | status |
|---|---|
| `node.bounds` — whole-actor union admits an editor-only 1010 frustum cube | **SETTLED** (m23-era). Mechanism proven; contract ruling locked (render-relevant bounds via `IsRenderableComponent`); implementation parked as a milestone candidate |
| `camera.path` — carries the **view-target actor** path, not a camera path | **OPEN** — naming/contract question |
| `coverage_pct` = 0 while `coverage_ratio` > 0 | **OPEN — PREDICTED FROM SOURCE, NOT MEASURED** |

**Third instance, status stated precisely because it matters:** this is a **source read**, not an
observation. `EvaluateSelectionProvenance` (`AnomalyViewport.cpp:540-570`) early-outs when
`CollectRenderableVisibleUnion` — which is occlusion-aware — returns null, leaving
`Out.CoveragePct` at its default 0, while `coverage_ratio` comes from the occlusion-blind projector
and stays non-zero. **No artifact exhibiting it has ever been seen.** It manifests **only** under
H4's exact condition (a fully occluded labelled target), so it is a *prediction contingent on an
unconfirmed hypothesis*, and it ships to the client in delivery mode if H4 ever occurs there.

### 1.4 Gotcha G99

The level-authoring script is destructive by default and the asset it destroys is the frozen gate
instrument. Guarded this turn; verified three ways (default refuses, override yields, sibling
passes). See `docs/gotchas.md`.

---

## 2. H4 — FILED. Design recorded before the instrument exists. NOT RUNNING.

**What a path-(b) result licenses — ruling, recorded verbatim in intent:**

- **LICENSES a MECHANISM claim:** the label path emits positives for fully occluded targets. The
  projector reads the current view and the actor's bounds; it has **no memory of how the target
  became occluded**, so (a)-vs-(b) history is irrelevant *to the projector*.
- **DOES NOT LICENSE an INCIDENCE claim:** that H4 causes the client's complaint. Incidence is
  entirely a question about (a), and (b) is silent on how often (a) occurs. **Never write (b) up as
  evidence of incidence.**
- **THE ASYMMETRY THAT MAKES (b) WORTH RUNNING:** a **negative** (b) **refutes H4 in one packaged
  run**, and (a) never needs building.

**Design, pre-declared:**

| | |
|---|---|
| **TARGET** | any of the ≥94 % on-screen occluded set — `StaticMeshActor_11 / _22 / _24 / _33 / _100 / _139`. **NOT `_5`** — 89.2 % sits under A56's 90 % line |
| **CONFIG** | `IAI.SetViewportScoping` **OFF**, effective value **echoed back (A48)**. A scoping-ON run selects nothing and reads as a clean null for the wrong reason |
| **SIGNATURE — symptom** | A54 → **ABSENT**. Shared with P3; **alone it discriminates nothing** |
| **SIGNATURE — cause** | `selection_provenance.json` → **`valid:false`, 0/0 samples**. Unique to H4: a P3-style non-manifestation has a *visible* target, so its provenance reads **9/9 passed, `valid:true`** |
| **THE PAIRING IS THE POINT** | neither signature certifies alone |
| **CONTROL (A53)** | same run, same config, a known-**visible** target must return A54 **ALIGNED** and provenance **9/9 `valid:true`** — bless and condemn |
| **PREDICTION** | label **IS** emitted; A54 **ABSENT**; provenance `valid:false` 0/0; `coverage_pct` 0 with `coverage_ratio` > 0 |
| **REFUTATION BRANCH, pre-declared as COUNTING** | **no label emitted ⇒ H4 REFUTED for path (b)** — *provided* the scoping echo confirms the gate was not the cause. **A refutation is a result, not a failed run** |

**Sequenced AFTER S3a. Do not run it. Do not build for it.**

---

## 3. Stage renumbering — LOUD, because a number moved

```
BEFORE                                   AFTER
  S3  B' colour + matrix                   S3  B' colour + matrix          (unchanged)
  S4  depth                                S4  backbuffer -> UI-on option  (WAS S5)
  S5  backbuffer demotion                  --  DEPTH: PARKED, UNNUMBERED
```

- **S3** — B′ into `AnomalyCapture` behind a **default-OFF** switch, **colour only**, full
  ratio × config matrix on the **real paced path**. **This is where ratio-independence is
  DISCHARGED.**
- **S4 (was S5)** — backbuffer demoted to the **UI-on option**, defaults flipped, client config.
- **DEPTH — PARKED, UNNUMBERED.** `SceneDepthTexture` in `PrePostProcessPass_RenderThread`, FP32,
  plus the typed FP16/FP32 path. **Parked, not deleted** — revivable if the ML side wants it, or if
  the H4/stencil lane needs a cheap instrument.

⚠ **CORRECTION — READ THIS IF YOU HOLD AN OLDER DOC.** **"S4" now means the backbuffer demotion, not
depth.** The number **moved down**; there is **no hole at S4**. Any earlier text calling depth "S4"
or the demotion "S5" is superseded by this entry. (Numbers are not reused for *different* things
silently — this shift is stated rather than left to be discovered, which is the m22 renumber hazard.)

### 3.1 Two things stated now so they cannot drift

- **S3 SPLITS INTO TWO GATED TURNS. Structural, not a suggestion.** **S3a** = implementation;
  **S3b** = the full ratio × config matrix. Bundling them means a validation miss halts a turn that
  also holds uncommitted code — different failure modes, different turns.
- **S3 GOING GREEN DOES NOT CLOSE P1.** P1 has never been reproduced, and you cannot demonstrate a
  fix for something you cannot summon. A clean matrix proves the new path does not carry **the old
  race**. It is **not** evidence it cures her defect. **P1 stays OPEN after S3**, leads unchanged:
  **H1** (GPU load, no lever exists) and the **delivery-mode gap**.
- **If the matrix goes red, that is a DESIGN FAILURE of B′, not a bug — it means REDESIGN, not
  patch.** Said now so nobody is tempted to paper over it later.

**PREDICTION, PRE-DECLARED BEFORE THE MATRIX EXISTS:** ratio-independence **HOLDS** — clean at every
ratio including deep and pacing-off. B′ keys by **identity**, not by order, so the arm→present race
that produced the −1 has **no positional step left to fail on**.

---

## 4. S3a — file-by-file plan. NO CODE THIS TURN.

### 4.0 The seam, and why S3a is narrower than it looks

Today's async path keys frame ↔ state by **`RequestId = GFrameCounter`**, stamped at arm time:

```
CaptureCurrentFrame()          Snap.FrameCounter = GFrameCounter
                               Async->PendingSnapshots.Add(GFrameCounter, Snap)
                               Capturer->ArmForCapture(GFrameCounter, Window, Rect)   <- the race
ProcessCompletedFrames()       PendingSnapshots.Find(Frame.RequestId)
```

B′ removes the arm→present pairing entirely: the game thread publishes
`(ViewFamily.FrameNumber → GFrameCounter)` at `BeginRenderViewFamily`, and the render-thread pass
recovers the `GFrameCounter` by looking up `View.Family->FrameNumber`. **That recovered value *is*
the `RequestId` the existing map is already keyed by.**

⇒ **The seam is `FAnomalyCapturedFrame::RequestId`.** S3a swaps the **producer** of that id and
touches **no consumer**: `PendingSnapshots`, `BuildLabelRecordForSnapshot`, `AccumulateFrameEvents`,
`FAnomalyAsyncWriter`, `labels.jsonl` and `annotation.json` are all untouched by design.

### 4.1 New files — `Source/AnomalyCapture/Private/`

**1. `AnomalySveKeyRing.h` / `.cpp`** — the B′ ring, production-shaped.
- `PublishKey(uint32 FamilyFrameNumber, uint64 GameFrameCounter, bool bWanted)` — game thread.
- `LookupKey(uint32 FamilyFrameNumber, FKeyEntry& Out)` — render thread.
- `Reset()`, capacity constant, and four counters: **published / consumed / missed / wrapped**.
- **`ForceMiss` lives here** (see 4.3) — it is a ring property, not a wiring property.
- Mirrors `CaptureBenchProbe`'s proven ring; drops all probe logging.

**2. `AnomalySceneViewExtension.h` / `.cpp`** — `FAnomalySceneViewExtension : FSceneViewExtensionBase`.
- `BeginRenderViewFamily` → `PublishKey(...)`. **The only hook where `ViewFamily.FrameNumber` is
  assigned** — `SetupViewFamily` still reports `UINT_MAX` (measured in S2; do not move it).
- `IsActiveThisFrame_Internal` → switch ON **and** capture active.
- `SubscribeToPostProcessingPass` → `EPostProcessingPass::VisualizeDepthOfField`, the bench's proven
  point: post-tonemap, **pre-Slate** (hence UI-free by construction).
- `AfterPass_RenderThread` → lookup by `View.Family->FrameNumber`; skip
  `bIsSceneCapture`/`bIsReflectionCapture`; on **hit + wanted**, `AddEnqueueCopyPass` into an
  `FRHIGPUTextureReadback`; **on MISS: count, warn loudly, DROP the frame — never label by guess.**

**3. `AnomalySveCapturer.h` / `.cpp`** — the SVE-side producer.
- In-flight readbacks, `EnqueueDrain()`, `PopCompleted(FAnomalyCapturedFrame&)`,
  `NumPendingApprox()` — **the same public surface as `FAnomalyFrameCapturer`** minus
  `ArmForCapture`, which is replaced by `MarkWanted(uint64 GameFrameCounter)`.
- Emits `FAnomalyCapturedFrame` with `RequestId` = the **ring-recovered** `GFrameCounter`.

### 4.2 Modified — build

**4. `AnomalyCapture.Build.cs`** — inside the existing non-Shipping block only:
`PrivateDependencyModuleNames += "Renderer"` and
`PrivateIncludePaths.Add(Path.Combine(GetModuleDirectory("Renderer"), "Private"))`.
**Sanctioned**: `architecture.md` deferred `Renderer`/Renderer-private to **Stage 3**, and this is
Stage 3. `AnomalyInjector` core is untouched, so the game-agnostic invariant holds.
⚠ **RISK R1:** a Renderer-*private* include path is engine-layout-specific (5.1). It is compiled out
of Shipping, but an engine bump can break the module's compile. Known and accepted; recorded here so
it is not rediscovered.

### 4.3 Modified — subsystem

**5. `AnomalyCaptureSubsystem.h`** — `bSveCapture` switch; SVE extension + SVE capturer handles
alongside the existing `Capturer` in `FAnomalyCaptureAsyncState`; a render-thread-safe "is this
`GFrameCounter` wanted" accessor.

**6. `AnomalyCaptureSubsystem.cpp`**
- **`IAI.Capture.SVE <0|1>`**, default **0**, **mid-run guarded** (mirrors the `ContentClock` guard).
- **Packaged default** via GConfig `[AnomalyCapture] bSveCaptureDefault`, read at `Initialize`
  beside the three existing keys (`:222-238`) — same proven mechanism, no new dependency.
- **`IAI.Capture.SVE.ForceMiss <0|1>`** — the forced-miss route, **planned in from the start**, not
  discovered later (that omission cost m23 a scope amendment).
- `StartRun` — switch ON ⇒ create/register the SVE + SVE capturer **instead of**
  `RegisterBackbufferHook()`.
- `CaptureCurrentFrame()` — switch ON ⇒ build and store the snapshot **exactly as today**, then
  `MarkWanted(GFrameCounter)` in place of `ArmForCapture(...)`. `ComputeGameViewportCapture` is not
  consulted: **the SVE's `ViewRect` is authoritative**.
- `FinishRun` — unregister; fold the four ring counters into the run summary.

**7. `AnomalyLabelWriter.{h,cpp}`** — `run_summary.json` gains `capture_path` (`"backbuffer"` |
`"sve"`) and, when SVE is on, the four ring counters. **`annotation.json` is untouched.**

### 4.4 What S3a explicitly does NOT do

No depth, no stencil. No defaults flip and no client-config change (that is **S4**). The backbuffer
path is **kept and remains the default**. No `ViewRing` / `ViewLagFrames` deletion — that stays
blocked on **A4 Condition 1 (VP equality), still UNSATISFIED**. No H4 work.

### 4.5 Behavioural differences to expect when the switch is ON — flagged now, not discovered later

- **R4 — resolution/rect.** The SVE captures the **view rect**; the backbuffer path captured the
  Slate window rect. `annotation.video.resolution` may therefore differ between paths. Expected, not
  a defect, and **not** a gate-1 failure (gate 1 is switch-**OFF** only).
- **R5 — UI.** The SVE grab is **pre-Slate ⇒ UI-free by construction**. That is the intended S4
  grab-point choice, but it means flipping the default later **changes delivered image content**.
- **R2 — hook point.** `VisualizeDepthOfField` is proven on `CB_GateLevel`; real content with a
  different post-process chain is an S3b question.
- **R3 — activation.** If `IsActiveThisFrame_Internal` returns false the callback never subscribes;
  the capture-active state must be readable safely from the render thread.

### 4.6 S3a gates — pre-declared

**G-S3a-1 — SWITCH-OFF INERT, DEMONSTRATED NOT ASSERTED.**
Production is no longer byte-unchanged (retired at m23), so the gate is **behavioural identity of
output**. Same seed, same config, switch OFF vs the pre-S3 binary ⇒ **byte-identical
`annotation.json`**, **byte-identical `labels.jsonl`**, **identical frame identity** (decoded
marker ↔ `frame_index` series, frame count and cadence).
⚠ **`run_summary.json` is EXPECTED to differ** — it now records `capture_path`. It is compared
field-by-field with the new fields excluded, and is **not** part of the identity set.
**Sequencing, because G92 wipes `Saved`:** take the baseline on the **currently staged m23 binary
first**, **re-bank**, *then* stage S3a. Do not rely on an older banked leg unless its `run.json`
config matches exactly.

**G-S3a-2 — THE LOUD-MISS GUARD FIRES ON THE PRODUCTION PATH, PROVEN BOTH WAYS.**
26/26 forced misses on the bench **does not carry** — a guard that has never fired *here* is not a
guard (m23's own ruling).
- *Fires:* SVE ON + `ForceMiss 1` ⇒ every lookup misses; `missed == published`; a loud warning is
  emitted; **zero frames written**; **no frame labelled by a guessed key**; the run ends cleanly.
- *Sleeps:* same binary, `ForceMiss 0` ⇒ `missed == 0`.
- **Gate on the ARTIFACT** (`run_summary` counters + the written file set), not on "a counter
  incremented" — m19's lesson.

**G-S3a-3 — THE SVE PATH PRODUCES A CORRECT SESSION** at nominal paced 30 fps on the gate level:
expected frame count written, `PendingSnapshots` fully consumed (no orphans), annotation cadence
matching the known gapped `[4,5,9,10]` shape, and **A10** (frame_index ↔ decoded `GFrameCounter`,
1:1) on that paced leg.

**Free-run debts — which S3a can discharge and which must wait:**

| debt | verdict |
|---|---|
| **A10** — frame-number ↔ `GFrameCounter` 1:1, paced | **S3a discharges it**, scoped to the nominal paced leg. The matrix extends it across regimes |
| **key-ring round-trip under stall** | **S3b** — requires the stall levers, which are matrix regimes |
| **A11** — one clean non-zero ring-counter observation | **S3b.** ⚠ `ForceMiss` produces non-zero counters but they are **synthetic**; A11 wants a *clean* observation, i.e. the ring genuinely wrapping or missing under load. S3a cannot honestly discharge it |

### 4.7 Commit slicing, with gates between slices

S3a is too big for one commit. Three slices, each independently revertable:

| slice | contents | gate before the next slice |
|---|---|---|
| **S3a-1** `feat(capture)` | ring + SVE + SVE capturer + `Build.cs` dep + both console vars registered. **Nothing selects the SVE path** — `StartRun` still always registers the backbuffer hook | compiles clean (5.1 Dev-Editor, exit 0); **inert BY CONSTRUCTION** — the strongest form of G-S3a-1, since no call site exists; ring round-trip exercised headlessly |
| **S3a-2** `feat(capture)` | wire `StartRun` / `CaptureCurrentFrame` / `FinishRun` to the switch | **G-S3a-1** (demonstrated) and **G-S3a-3** |
| **S3a-3** `feat(capture)` | `run_summary` `capture_path` + ring counters | **G-S3a-2**, both ways |

**Packaged-run hygiene on every leg:** **A44** symbol/string scan of the staged binary; **G92**
re-bank **before** staging; **A48** echo the *effective* config values, never the requested ones.

**Comment stripper runs before every commit** (source must stay comment-free).

**No tag at S3a.** The milestone tag belongs after S3b's matrix.

---

## 5. State

- Plugin `master`: docs-only this turn. **No capture code written.**
- `CaptureBench`: **`8dad64e`** — the frozen-level guard. **Probe untouched**; this is a tools edit,
  not a probe edit (same ruling as the auditor).
- Bank: 74 session dirs.
- **Next turn: S3a implementation**, slices as above, on approval of this plan.
