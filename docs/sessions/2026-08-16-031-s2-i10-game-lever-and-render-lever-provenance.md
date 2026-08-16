# 2026-08-16 — 031 — S2: the A43 pre-flight, the I10 game-lever legs (the defect does NOT reproduce), and the render lever diagnosed as a stale binary

Base: plugin `fbf8ad1` (this session adds docs only). Bench: `CaptureBench` `163dd12`, **frozen for
the duration of the I10 game legs** — zero probe edits between calibration and measurement.
**Production `AnomalyInjector` / `AnomalyCapture` remain BYTE-UNCHANGED.** Production still captures
via the **backbuffer**. **No S3 work has started, and the clean I10 result does not license any.**

> **Numbering.** The chat track runs a numbered amendment series. A34–A43 are carried by
> `docs/CHAT-HANDOFF-s2-gate-env-and-i10-setup.md`; **A44–A47 are new this session and are written
> out in full in §7**, so a cold reader never needs to resolve a dangling number.

---

## 1. What this session was for

Two things, in this order, both settled:

1. **A43 pre-flight** — the marker oracle had only ever been decoded from CaptureBench's *own*
   capturer. Every I10 leg decodes it from **production capture** PNGs, and production capture has
   deliberately kept a DrawDebug visual out of saved frames before (the poll-radius sphere). If that
   suppression caught the DrawDebug family the oracle would be blind in exactly the captures I10
   needs.
2. **I10 game-lever legs** — does the client's `−1` reproduce on the **current backbuffer path**
   under calibrated game-thread starvation?

A third result arrived unbidden: the dead render-thread lever is **diagnosed**, and it was diagnosed
without touching the probe, because establishing *which binary the legs would run on* is a
prerequisite of the freeze anyway.

---

## 2. A43 — THE MARKER SURVIVES PRODUCTION CAPTURE

One production capture in `CB_GateLevel`, marker on, decoded from the production PNGs.
**MEASURED: it decodes.** 60/60 frames returned a value; frames 5..59 gave a strictly increasing
series 8..78. Production capture does **not** suppress the DrawDebug-family marker. Proceeded to the
legs in the same turn, as A43 specified.

### The edge the pre-flight exposed — and it is sharper than the rule we had

Frames 0..4 of that run carried **no marker at all** (the player controller / camera are not ready
that early), and the decoder returned a **confident wrong answer** on every one of them: value `0`,
row `105`, spread `95.3` — comfortably past its own `spread >= 40` gate. It had latched onto ordinary
scene contrast.

The standing rule was "marker decode success is not proof the scene rendered." It is now stronger and
more specific: **a successful decode is not proof a marker was drawn.** Validity requires a
**strictly increasing decoded series across the analysis window** — never a decode count. This is
**A45** (§7) and it is built into the leg validity check.

**Free retro-finding.** Re-decoding the banked game-thread calibration session `SW/G30` gives the
identical false signature — constant `0` at row `105` on all 60 frames. **The banked calibration legs
were run with the marker OFF** and therefore carry no frame-identity evidence of their own. The ratio
table itself is unaffected (§6 finding E reproduces two of its points on the same binary), but this is
a live input to the A17/A19 retroactive audit's second axis ("did anyone verify the frames had
content?").

---

## 3. THE RENDER LEVER — DIAGNOSED, WITHOUT EDITING THE PROBE

### The measurement

MEASURED, at byte level, on the package the render legs actually ran against:

```
Builds\BenchGate\...\Binaries\Win64\StackOBot.exe   LastWriteTime 2026-08-06 01:33:46
                                                    (never rebuilt since)

UTF-16 string scan of that exe:
  "CaptureBench.Stall.RenderMs"   PRESENT     <- the CVar exists, so the knob "works"
  "CaptureBench.Marker"           PRESENT
  "PROBE,postactortick"           PRESENT
  "PROBE,MISS"                    PRESENT
  "CaptureBenchRenderStall"       ABSENT      <- the ENQUEUE_RENDER_COMMAND
  "PROBE,renderstall"             ABSENT
  "PROBE,stallcounters"           ABSENT      <- the execution counters
```

Corroborated from git and the filesystem:

- `163dd12` *"A41 render-lever rebuild + execution counters"* is dated **2026-08-16 11:59:18**.
  `GameStallFires` / `RenderStallFires` / `LogStallCounters` exist at `163dd12` and at **no earlier
  commit** (checked `54bf5a1`, `88d2bc4`, `eef0019` — all empty).
- The render-stall legs `RSW/r20..r110` wrote their sessions at **11:54:29 .. 11:57:20** on
  2026-08-16 — *before that commit existed* — into that same package's `Saved` tree.

### The cause — and the sharper version, found when the rebuild ran

**The render command is never issued, because the code that issues it is not in the binary under
test.** In that binary the render stall still lives only inside CaptureBench's SceneViewExtension —
the original dead configuration. This is candidate (a) from journal 030 §7 in its strongest form.
Candidate (b) (counter/log gating) is refuted by the same evidence: there is no counter in that
binary to gate wrongly.

⚠ **REFINED after the rebuild, and the refinement is the actually dangerous trap.** The monolithic
game binary **had been compiled** — `D:\IntrusiveAnomalies\StackOBot\Binaries\Win64\StackOBot.exe` is
dated **2026-08-16 11:53:58**, i.e. *36 seconds before the first render leg started*, and it contains
all three symbols. What never ran was the **stage/archive** step, so `Builds\BenchGate` went on
serving the 2026-08-06 exe. The re-run of `BuildCookRun` this session **compiled nothing** for the
`StackOBot` target (only 4 actions, all for `StackOBotEditor`) and merely re-staged — the archived
exe now carries that same 11:53:58 timestamp.

So it was never "forgot to build". It was **built, seen to succeed, and never staged** — which is
worse, because the developer has a green build in hand. It also means **A44's timestamp half would
have been misleading here in the other direction**: the archived exe inherits the *compile* time, not
the archive time. **The string scan is the load-bearing half of A44.** → **G92**.

### The correction to the record — measurement wins

The handoff doc states *"STALL_FIRED = 0 at all four values"* and *"the execution counter (A41)
caught it on first use."* **The counter was not in the binary, so it cannot have reported 0.**

- The **conclusion was right** and the four `ratio 1.000` readings are real. The lever genuinely
  never fired.
- The **catch was not made by the counter.** It was made by ratio arithmetic: at 110 ms a firing
  render stall would have forced `ratio ≈ 3.3` by the concurrency model, and the ratio did not move.

Standing phrasing adopted so this cannot be misremembered: **"a counter that never printed is not a
counter that printed 0."** The A41 principle stands — it is simply **unpaid-for** until its first
real catch, which is now scheduled.

Note the A41 rule *does* have teeth for the **game** lever, by stronger evidence than a counter: a
busy-wait that never ran cannot move frame time from 33.3 ms to 100.1 ms.

### Why this made the freeze stronger, not weaker

The A42 freeze exists so the headline measurement does not ride on fresh code. The package predating
`163dd12` means the I10 legs ran the **byte-identical binary that produced the banked calibration
table**. §6 finding E confirms it reproduces that table.

---

## 4. I10 — THE GAME-LEVER LEGS

### 4.1 Conditions (A32), pinned and recorded

| | |
|---|---|
| Scene | `CB_GateLevel` — **verified by NAME, not picture**: exactly one `LogLoad: LoadMap: /Game/CaptureBenchGate/CB_GateLevel` per leg log, zero MainMenu loads, checked on all six |
| Rate / size | `VideoFps 30` (A9), 1280×720, format `png` |
| Exposure | pinned: `r.DefaultFeature.AutoExposure 0` + `r.EyeAdaptationQuality 0` |
| Mode | **TARGETED, single anomaly, hide-type: `blinking` on one actor** (A36). *Why targeted:* auto-pool mixes `missing_texture` into the same series, and the oracle assumes one hide-type target with one bbox |
| Target | `StaticMeshActor_49` — chosen by **measurement**: `IAI.DumpCoverage` ranks it largest on screen at **7.798%** (next 6.868%), and it is independently the actor the earlier `T2` auto-pool run selected |
| Schedule | seed 777, 90 frames/leg, `K=2 pre=4 positive=8 post=4` |
| Delivery | **OFF**, `content_clock=wall` (default). **I10-game did NOT emulate delivery mode** — the oracle needs the projected bbox from `labels.jsonl`, which delivery suppresses |
| Machine | dev box; package `Builds\BenchGate`, exe built 2026-08-06 01:33:46 |
| Marker | ON, every leg |

**Two approved deviations, both recorded as such.**

1. **`StaticMeshActor_49` instead of "a named CB_ target".** The spec error was **chat-side**: actor
   **labels are editor-only** (our own standing invariant), so `CB_Target_NN` never exists at
   runtime; the reachable names are `StaticMeshActor_NN`. Coverage-ranked selection preserves the
   intent (targeted, single, known, prominent).
2. **90 frames/leg** rather than the calibration table's 60 — event-count driven; it yields 5–8 hide
   events per leg after the settle window, against the ≥3 requirement.

Also recorded: the A31 content checks were **recomputed in Python over the production PNGs**. The
engine-side checks live in `FCaptureBenchCapturer` and do not run during a *production* capture. This
is functionally equivalent — the checker reads the same PNGs the oracle reads.

### 4.2 The oracle

Per-frame metric = **mean luminance INSIDE the target bbox** (A35), divided by the whole-frame mean.
The normalisation is not cosmetic: whole-frame luminance ramps **34 → 111** over the first ~16
captured frames, which swamps a raw in-bbox reading.

Two-cluster split by largest gap, **fitted inside the leg's settled window**, then applied to all
frames. Hidden/visible orientation is decided by the majority of annotation-clean frames; with 4
hidden of every 12, a ±1 shift cannot flip it, so the orientation is **not circular**.

**Positive control, built in.** Every leg reports mismatches at shift −2..+2. A one-frame shift costs
**19–30 mismatches** on every leg. The oracle would have seen a `−1`. That control is what makes the
null below a real null rather than a blind instrument.

**A27 eyes step performed, not skipped.** Leg L6 index 39 vs 40: the cube at bottom-left is present on
39 and gone on 40, at exactly the boundary `annotation.json` claims; the marker strip differs between
the two frames.

### 4.3 Predictions — restated verbatim, before results

> nominal band: clean · CLIENT band: shows the −1 · DEEP band: −1 or worse ·
> pacing-OFF: shows the −1 · **if the CLIENT band is CLEAN, that is a major result meaning her
> defect has a different mechanism — reported as such, not as a partial failure.**

Render legs: **UNPREDICTED, now and forever.** None was written.

### 4.4 Results

| leg | stall | pace | achieved ratio | A40 band | window | events | shift-0 mismatches | per-event deltas |
|---|---|---|---|---|---|---|---|---|
| L1 | 0 | on | **1.0000** | nominal | 16..89 | 7 | **0 / 74** | all 0 |
| L2 | 34 | on | **1.0558** | mild | 0..89 | 8 | **0 / 90** | all 0 |
| L3 | 39 | on | **1.2148** | **client** | 30..89 | 5 | **0 / 60** | all 0 |
| L6 | 40 | on | **1.2342** | **client** | 0..89 | 8 | **0 / 90** | all 0 |
| L4 | 99 | on | **3.0027** | **deep** | 0..89 | 8 | **0 / 90** | all 0 |
| L5 | 0 | **OFF** | **0.3312** (`paced:false`) | pacing-off | 0..89 | 8 | **0 / 90** | all 0 |

Sign convention: `delta = (first index where the target disappears in the PIXELS) − (first index
annotation.json claims)`. `0` = aligned.

**44 hide events, 494 frames compared, ZERO misaligned frames anywhere.**
Required coverage complete: nominal, client (×2), deep, pacing-off. **Mild came in free at 1.0558** —
A40 had it as uncovered; it is now covered, and clean.

Per-leg validity numbers (window), so the record carries numbers and not just PASS:

| leg | lum mean | sd | clip % | cluster gap (% of range) | threshold | hidden side |
|---|---|---|---|---|---|---|
| L1 | 109.65–114.69 | 104.25–106.87 | 5.74–16.45 | 0.1723 (66.5%) | 1.5638 | high |
| L2 | 111.39–115.66 | 106.45–106.95 | 13.70–16.95 | 0.1958 (70.4%) | 1.5477 | high |
| L3 | 125.67–128.74 | 103.44–104.87 | 15.90–18.92 | 0.0360 (34.6%) | 1.6756 | **low** |
| L4 | 110.05–114.82 | 106.45–106.95 | 13.67–16.93 | 0.1750 (64.3%) | 1.5646 | high |
| L5 | 112.81–117.70 | 106.18–106.73 | 13.70–16.95 | 0.1714 (60.3%) | 1.5270 | high |
| L6 | 110.06–115.63 | 106.38–106.88 | 13.69–16.94 | 0.1666 (59.6%) | 1.5612 | high |

Thresholds for reference: luminance floor ≥ 2.0, flatness floor sd ≥ 5.0, clip ceiling ≤ 35%. All six
pass with wide margin. **L3's "hidden = low" is not an anomaly** — its camera rested elsewhere (§6
finding A), so its bbox frames a different patch of scene and hiding the target *darkens* the crop
instead of brightening it. L3 being clean **with a completely different crop and inverted polarity**
is an incidental cross-check on the oracle.

### 4.5 The independent identity cross-check — accepted as the stronger instrument

For every captured frame, the marker's decoded `GFrameCounter` (drawn in the game tick, *present in
the pixels*) was compared against that frame's own `labels.jsonl` `frame_index` (the arm-time
counter):

```
label.frame_index - marker_gfc  ==  0   on  532 / 534 decoded frames
```

per leg: L1 85, L2 90, L3 87, L4 90, L5 90, L6 90. **The only two exceptions are pre-window
warm-up frames in L3 with no marker drawn** — the §2 false-decode signature, outside every leg's
analysis window.

This holds in **every** regime, deep starvation (ratio 3.0) and pacing-off (0.33) included. It is the
property the m21 residual violates: a **stale presented scene would decode an EARLIER counter than
the label claims**. It does not, anywhere. This goes into the record **beside** the pixel oracle as
the stronger frame-identity instrument.

### 4.6 Verdict against the predictions

| band | predicted | measured | |
|---|---|---|---|
| nominal | clean | CLEAN | prediction held |
| **CLIENT** | shows the −1 | **CLEAN, twice** | **prediction FAILED** |
| DEEP | −1 or worse | CLEAN | prediction FAILED |
| pacing-off | shows the −1 | CLEAN | prediction FAILED |

By the pre-declared rule, **the CLIENT band coming back clean is a MAJOR RESULT, not a partial
failure**: under game-thread starvation on the current backbuffer path, the client's defect does not
reproduce.

⚠ **The pacing-off prediction was in tension with our own record before it was written.** m21 reports
pace-off **FIXED, 0/100**. A prediction of "shows the −1" contradicted a shipped, measured fix. The
leg is therefore a *regression check that passed*, and the prediction still **counts as failed** —
recorded rather than quietly reclassified, because the interesting fact is that the frozen prediction
set disagreed with the project's own m21 record and nobody noticed at declaration time.

---

## 5. What this licenses — and what it explicitly does not

**Licensed, exactly one claim:** *the defect does not reproduce under **GAME-THREAD** starvation, on
this box, in this level, at `VideoFps 30`, on the current backbuffer path.* **No mechanism claim.**

**NOT licensed, and not drifting toward "probably fine":**

- **Deep starvation stays OPEN.** Ratio 3.0 was reached by a **game-thread** stall, and G89's own
  concurrency model says a game-thread stall replaces frame time while the render thread runs
  **concurrently** — the renderer is never starved. The m21 residual is a **present-side**
  phenomenon. L4 *could not have tested it.*
- **The shape the residual was actually observed in is still owed**: high `VideoFps` (120/240,
  pacing ON).
- **Nothing here shows the SVE migration fixes the client's defect.** It shows the current path is
  well-behaved under one class of starvation. Those are different claims and only the second matters.

**Consequence, per A42's conditional, now resolved: THE RENDER LEVER IS THE CRITICAL PATH.** The
client's shape may be render-side, and that is the only untested hypothesis standing.

---

## 6. Gate-environment and harness findings (all MEASURED this session)

**A. The gate level's camera is NOT static, and NOT deterministic across legs.** G89 says the
stationary `PlayerStart` makes camera motion "a variable we introduce deliberately rather than
inherit." **False in practice:** the pawn spawns above the floor, falls, and settles. Measured:

- the projected bbox moves for the first ~0.5–1 s (L3: **11 distinct bboxes**, settled only at
  frame 30);
- whole-frame luminance ramps **34 → 111** over the first ~16 captured frames;
- **the settled camera differed between legs.** L1/L2/L4/L5/L6 rest with the target at **306×235 px**;
  L3 rests at **178×291 px**. L3 was stall 39 and L6 stall 40 — near-identical input, different rest
  position. A **run-to-run bifurcation**, not a function of the stall.

Not fatal (each leg uses its own settled bbox and window), but G89's "deterministic" claim must be
qualified: **content-deterministic, not camera-deterministic**. This is **A47**.

**B. The focus gate makes leg start time nondeterministic.** m16's 30 s focus-gate timeout fired on
some legs and not others, so some began capture cold with warm-up and camera-settle *inside* the
capture (L1, L3) and some began fully warmed (L2, L4, L5, L6). L2 sat 31 s before `start_frame 857`.
Harmless to this result, but legs are not condition-uniform. Next round should set
`IAI.Capture.FocusGate 0` or insert a fixed warm-up delay before starting.

**C. Harness trap — it already produced one wrong number.** `Builds\BenchGate\Windows\StackOBot.exe`
is a **217 KB launcher**; the real 240 MB game is a **separate process**. `Start-Process -PassThru`
returns the launcher, so killing it leaves the game running and the **next** leg is measured against
a still-running previous instance. The first nominal attempt read **ratio 1.483 at stall 0** for
exactly this reason; with kill-by-name plus an idle-box assertion the same leg reads **1.0000**.
→ **G90**, and **A46**.

**D. `TryFireSpecific` prepends `=` itself** (`AnomalyAutoInjectorSubsystem.cpp:283`), so the targeted
actor token must **not** include one. `"=StaticMeshActor_49"` becomes `"==StaticMeshActor_49"` and
**silently zero-matches** — the run completes normally with `positive_frames: 0`. Note the banked
calibration legs passed `"=CB_NO_SUCH_ACTOR"`, which became `"==CB_NO_SUCH_ACTOR"` and still
zero-matched, so their intent held either way. → **G91**.

**E. Instrument continuity confirmed.** Same binary as the banked table, and it reproduces it:

| stall | banked | measured this session |
|---|---|---|
| 99 ms | 3.004 | **3.0027** |
| 39 ms | 1.204 | **1.2148** |
| 34 ms | 1.239 | **1.0558** |

The 34 ms row is the knee-region noise G89 already documents, observed a second time.

---

## 7. New standing rules (A44–A47), written out in full

- **A44 — binary provenance.** No leg is interpreted until the change under test is proven **PRESENT in
  the artifact under test**, by **symbol/string scan** (or an embedded version stamp). **Build
  timestamps are ADVISORY ONLY and mislead in both directions** — a stale binary can sit behind a green
  build (staging skipped), and a freshly staged one can carry an old timestamp (the archive inherits the
  compile time). ***"The harness ran" is never "the change ran."*** → **G92** for the trap and the
  one-liner.
- **A45 — marker validity.** A valid marker read is a **strictly increasing decoded series over the
  analysis window**. A lone successful decode is never evidence a marker was drawn; the decoder
  confidently misreads markerless frames.
- **A46 — harness process hygiene.** Kill by process **NAME** and assert a **zero-instance idle box**
  before every launch.
- **A47 — per-leg bbox.** Camera rest position bifurcates run-to-run. Every leg computes its **own**
  settled bbox and settle window; no design may assume a fixed bbox across legs.

---

## 8. THE RENDER LEVER, REVIVED AND CHARACTERISED

### 8.1 The rebuild and its A44 verification

`BuildCookRun` re-staged `Builds\BenchGate` from CaptureBench `163dd12`. **BUILD SUCCESSFUL, 3m 00s**
(compile was already done — see §3 — so this was a cook/stage/archive pass).

**A44 verification of the fresh package** (`...\Windows\StackOBot\Binaries\Win64\StackOBot.exe`,
240 497 664 B, compile stamp 2026-08-16 11:53:58):

```
must now be PRESENT (were ABSENT):        controls (were already present):
  CaptureBenchRenderStall   PRESENT         CaptureBench.Stall.RenderMs  PRESENT
  PROBE,renderstall         PRESENT         CaptureBench.Marker          PRESENT
  PROBE,stallcounters       PRESENT         PROBE,MISS                   PRESENT
                                            PROBE,postactortick          PRESENT
```

⚠ Before staging the archived sessions were moved out to `D:\IntrusiveAnomalies\_bench_sessions_bank`
(1347.2 MB, all 10 directories: CAL, CAL2, CaptureBench, Config, I10, Logs, RSW, SW, T2, T2C). The
archive step wipes the package tree, and that tree held every banked session including the I10 legs
and the raw evidence the A17/A19 audit will need.

### 8.2 The smoke test — it fires, and ZERO probe edits were needed

`CaptureBench.Stall.RenderMs 40`, production capture, targeted **zero-match** (A36: clean pacing
signal, no fires — the same shape the game table was calibrated with).

```
PROBE,renderstall,fired=60,ms=40.0
PROBE,renderstall,fired=120,ms=40.0
...
PROBE,renderstall,fired=780,ms=40.0        <- 13 log lines, 780 executions
speed_ratio = 1.4069   (was 1.000 dead)
```

**The three-point logging (registration / issue / execution) was not written, deliberately.** The
execution point already exists at `163dd12` and it is the strongest of the three — if execution logs,
registration and issue are proven by implication. Writing diagnostic code for a lever that turned out
to work would have been an edit for its own sake. The `0 ms-not-plumbed` edge case is likewise dead:
the log line carries `ms=40.0`.

**The A41 counter has now been paid for**: `RenderStallFires` produced the `fired=N` series above,
which is a real counter making a real observation. Negative control also holds — with
`RenderStallMs 0` the counter logs **zero** lines (§8.4).

### 8.3 The sweep — `speed_ratio` is NOT blind to render-side starvation

Conditions identical to the game table (A32): `CB_GateLevel` verified by name, `VideoFps 30`,
1280×720, exposure pinned, targeted zero-match, 60 frames/leg, pacing ON, rebuilt binary.
`frame_ms = 1000 / sustained_wall_fps`.

| render stall (ms) | ratio | frame (ms) | `fired` |
|---|---|---|---|
| 0 | 1.0000 | 33.3 | **0** (negative control) |
| 10 | 1.0001 | 33.3 | 2100 |
| 20 | 1.0000 | 33.3 | 1320 |
| 30 | 1.1161 | 37.2 | 960 |
| 40 | 1.4069 | 46.9 | 780 |
| 70 | 2.3076 | 76.9 | 480 |
| 110 | 3.5065 | 116.9 | 360 |

**The metric responds to render-side starvation exactly as it does to game-side starvation.** The
"speed_ratio is blind to render starvation" hypothesis — the one the dead lever nearly certified as a
major finding — is **refuted by measurement**.

**One model covers both levers:**

```
frame_time ≈ max( 1/VideoFps , stall + residual )
      residual = 1.3 ms  (GAME lever)      knee = 33.3 − 1.3 = 32.0 ms
      residual = 6.9 ms  (RENDER lever)    knee = 33.3 − 6.9 = 26.4 ms
```

Fit for the render side above the knee is essentially exact: 30 → +7.2, 40 → +6.9, 70 → +6.9,
110 → +6.9. The larger residual is expected and physical — the render thread must still render the
scene and service the capture readback, and the recorded natural cost of this level is 8.52 ms;
the game thread carries only 1.3 ms of non-stall work.

⚠ **Corollary that matters for reading client telemetry:** because both levers move `speed_ratio`
through the same functional form, **a client's `speed_ratio` of 1.2 says frame time ≈ 40 ms and says
NOTHING about which thread was starved.** The metric cannot attribute. That is now measured, not
assumed.

### 8.4 Cross-binary instrument continuity (finding-E style, on the new binary)

| game stall | banked (old binary) | §6 re-measure (old binary) | anchor (REBUILT binary) |
|---|---|---|---|
| 0 ms | 1.000 / 33.3 ms | 1.0000 | **1.0000 / 33.3 ms** |
| 99 ms | 3.004 / 100.1 ms | 3.0027 | **3.0242 / 100.8 ms** |

The instrument survived the rebuild. Both anchors verified `CB_GateLevel` by name and logged
`renderstall` **0 times**, confirming the render lever stays silent when it is not asked for.

⚠ **Recorded gap, not fixed:** `gameStallFired` / `renderStallFired` can only be dumped via
`CaptureBench.Probe.Report`, and `-ExecCmds` runs at startup, so the *totals* cannot be printed after
a run without a delayed-exec path. The render lever is unaffected (its periodic log *is* the counter);
the game lever's execution rests on ratio arithmetic, which is the stronger evidence anyway. A future
probe change could log both counters at capture finish — deliberately **not** done here under the
prefer-zero-edits rule.

---

## 9. Next

**No render I10 legs were run, and their bands are not declared here.** Per the same A40 discipline
that made the game legs interpretable, the bands must be **pre-declared chat-side from this sweep**
before any render I10 leg runs. What the sweep supplies for that decision, from the model
`ratio ≈ (stall + 6.9) / 33.33`:

| A40 band | required ratio | render stall that lands there |
|---|---|---|
| nominal | [1.00–1.02] | 0–20 (measured flat) |
| mild | (1.02–1.10] | ≈27.1–29.8 — a 2.7 ms window; wider than the game lever's but still best-effort |
| **client** | [1.15–1.35] | **≈31.4–38.1** (suggest two legs near 33 and 36) |
| **deep** | ≥2.80 | **≥86.4** (110 measured 3.5065) |
| pacing-off | own category | `Pace 0` with a render stall |

Then, still owed and not covered by this session: the high-`VideoFps` (120/240, pacing ON) legs that
reproduce the shape the m21 residual was actually observed in; the A17/A19 retroactive audit (paper
only, both axes — with §2's marker-OFF finding as a new input); the I2 view-count re-measure.

✅ **A20 item 4 is DISCHARGED, and had been for ten days** — the pre-cook `GameDefaultMap` check landed
in `fbf8ad1` as the `Config/DefaultEngine.ini → GameDefaultMap` bullet in
`docs/PRE-DELIVERY-CHECKLIST.md` §1. That commit's message calls it the "delivery-checklist guard";
the debt was then carried forward wrongly by the handoff doc and by this journal's first draft. Struck.

**FRAMING CORRECTION — the migration is NOT gated on reproducing the client's defect.** S3's
ratio × config matrix discharges **ratio-independence on its own terms**; that is the requirement. A
reproduction is the strongest available *validation asset* — with one, S3 can show the defect **dies**
rather than merely that gates pass — which is why the hunt continues. But it is not a logical
precondition, and earlier phrasings in this repo that read "nothing is proven until the defect
reproduces" overstate it. What remains true: **nothing shows the migration fixes HER defect**, and
that specific claim needs a reproduction before it can be made.

*(Environment note, no action: the machine's `HKLM` UE 5.1 registry path is stale; the 5.1 engine this
project builds against is the source build at `D:\UESource\UnrealEngine`, resolved via the uproject's
`EngineAssociation` GUID. A running `UnrealEditor.exe` on this box belongs to an unrelated 5.7
project and was left alone.)*
