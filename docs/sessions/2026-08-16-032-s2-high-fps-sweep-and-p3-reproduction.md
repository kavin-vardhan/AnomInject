# 2026-08-16 — 032 — S2: the high-VideoFps sweep, and the FIRST REPRODUCTION (P3: labeled hides that never manifest)

Base: plugin `777135f` (docs only this session). Bench: `CaptureBench` `163dd12`, **zero probe edits —
third consecutive turn**. **Production `AnomalyInjector` / `AnomalyCapture` BYTE-UNCHANGED.** Production
still captures via the **backbuffer**. **No S3 work.** No fix attempts.

Companion: journal 031 (I10 game + render legs, both clean at `VideoFps 30`). This journal is where the
hunt finally caught something.

---

## 1. Headline

At **`VideoFps` 120 and 240**, hide-type anomaly windows are **labelled in `annotation.json` but never
appear in the captured pixels** — **49 of 49 events across four legs, zero manifested.** Every one of
those labelled-positive frames is a training example that says "anomaly here" over a picture with no
anomaly in it. That is dataset poisoning, and it is the first defect this project has reproduced on
demand.

⚠ **It is NOT the mechanism we expected, and three measurements say so** (§5). ⚠ **It is `VideoFps`-scoped,
not ratio-scoped** — the same target, seed, anomaly and binary are perfectly aligned at 30 fps across
twelve legs (§6).

---

## 2. Three phenomena, tracked separately from here on

The project has been treating one label — "the −1" — as one bug. It is at least three, and conflating
them is what made the m21 residual unfixable.

| | signature | status |
|---|---|---|
| **P1** | the client's **one-frame shift** at ratio ≈1.2, `VideoFps 30` | **NOT reproduced** — 12 legs, both levers, nominal→deep→pacing-off (journal 031) |
| **P2** | **stale / duplicate present** — the presented backbuffer carries an old scene | **signature ABSENT here**: 0/149 byte-identical *and* 0/149 near-identical adjacent frames on all four legs |
| **P3** | a **labelled hide window never manifests** in any frame | **REPRODUCED, 49/49 events**, `VideoFps`-dependent (this journal) |

---

## 3. The high-`VideoFps` calibration sweep

A44 header: package `Builds\BenchGate`, compile stamp 2026-08-16 11:53:58, three-symbol scan PRESENT,
no rebuild. A32: `CB_GateLevel` verified **by name** every point, 1280×720, exposure pinned, **targeted
zero-match** (A36 — calibration wants a clean pacing signal with no fires), marker ON, A45 validity, A46
hygiene, pacing ON. **120 frames/point**, same at both fps so they compare; at 240 fps 60 frames is only
a quarter-second of video timeline and the ratio is computed over the armed-frame wall span.

**A48 config echo — four independent read-backs per point, none of them the value requested:** the
engine's own `IAI.Capture.Fps: N` log, the `Capture run STARTED … fps=N` line, `run.json target_fps`,
`run_summary.json target_fps`. **All four agreed at 30 / 120 / 240 on every point.** `VideoFps` is
`FMath::Clamp(InFps, 1, 240)` (`AnomalyCaptureSubsystem.cpp:442`), so **240 is exactly the ceiling and
lands intact — not clamped, not refused.**

| point | fps | gMs | ratio | frame (ms) | sustained |
|---|---|---|---|---|---|
| N030 | 30 | 0 | 1.0000 | 33.33 | 30.00 |
| N120 / N120b / N120c | 120 | 0 | **2.1317 / 1.5213 / 1.8197** | 17.76 / 12.68 / 15.16 | 56.29 / 78.88 / 65.95 |
| N240 / N240b / N240c | 240 | 0 | **3.1767 / 4.0641 / 4.0565** | 13.24 / 16.93 / 16.90 | 75.55 / 59.05 / 59.16 |
| G120_22 | 120 | 22 | 2.8287 | 23.57 | 42.42 |
| G240_24 | 240 | 24 | 6.1726 | 25.72 | 38.88 |
| G120_60 | 120 | 60 | 7.3836 | 61.53 | 16.25 |
| G240_60 | 240 | 60 | 14.7952 | 61.65 | 16.22 |

### The structural finding — `speed_ratio` stops being a dial

```
frame_time ≈ max( 1/VideoFps , natural_frame_time , stall + ~1.4 )
```

`natural_frame_time` on this box at 1280×720 with PNG writing is **12.7–17.8 ms**. At 30 fps the budget
(33.3 ms) sits comfortably above it, so the pacer dominates and the ratio is a clean, controllable
1.000 — *that is why the 30-fps table was so well behaved*. At 120 fps the budget is 8.33 ms and at 240
it is 4.17 ms, **both below natural cost**, so natural cost dominates and **the ratio degrades from a
dial into a readout of natural starvation** — and the readout is noisy (±20% wander), against the ~1–2 ms
noise seen at 30 fps.

⚠ **Measured, not modelled** (A9: the 30-fps table is never scaled arithmetically to other fps):
**at 120 fps the natural floor is ratio 1.52–2.13; at 240 fps it is 3.18–4.06 — already in the deep band
with zero induced load.** The nominal / mild / client bands are **unreachable at 120 and 240 on this
box**. That is a **measured property of the instrument, explicitly not a coverage failure** — A40's
required coverage belonged to the 30-fps I10 question and was discharged there.
⚠ Dev-box numbers. Not portable (A32).

**Correction on the record:** the first sample pair looked like 120 fps running *slower per frame* than
240 fps, which would have been a real anomaly. The repeats show it was noise — natural frame time
wanders across the same 12.7–17.8 ms band at both. No mechanism was written for it, because there is
none to write.

**The pre-declared sweep tripwire did not trigger:** identity was checked on every zero-match point as it
came in — **1315 / 1315 decoded frames at `label.frame_index − marker_gfc == 0`**, up to ratio 14.7952.
Note precisely what that is: the identity instrument on runs with **no fires**. It shows the presented
frame carries the marker of the tick its label claims. It does **not** exercise anomaly-state surfaces.

---

## 4. I10-HF — the legs

**A49 — regime windows replace A40 bands for this set.** A40's nominal/client/deep coverage answered the
30-fps question and is discharged; the high-fps question is different (*does the known historical shape
reproduce at the fps where it was seen*), so the legs carry pre-declared **regime windows** as validity
condition 1.

**Prediction, restated verbatim before any result, with its pre-declared sharpening:**

> "High-VideoFps (120/240, pacing ON) legs: PREDICTED TO REPRODUCE frame↔label misalignment (nonzero
> per-event deltas and/or identity diffs). That is why the legs exist — it is the known historical
> shape. If they come back CLEAN, that is a MAJOR RESULT: the known failure no longer manifests on the
> current path and instrument, and the hunt narrows to H1, the delivery gap, and out-of-model causes —
> reported as such, not as a partial failure."
>
> Sharpening: **ABSENT-classified events count as reproduction** alongside SHIFTED and identity diffs.

Method identical to I10-render except as stated: targeted single-anomaly `blinking` on
`StaticMeshActor_49` (A36 — auto-pool would mix `missing_texture` into the same series), seed 777,
**150 frames/leg**, marker ON, delivery **OFF** + `content_clock wall` (**these legs do not emulate
delivery mode**), exposure pinned, 1280×720, `FocusGate` **ON** (see §7).

| leg | fps | gMs | achieved | **declared window** | frame (ms) | events | ALIGNED | SHIFTED | **ABSENT** |
|---|---|---|---|---|---|---|---|---|---|
| HF1 | 120 | 0 | **1.6916** | [1.30–2.40] ✓ | 14.10 | 13 | 0 | 0 | **13** |
| HF2 | 240 | 0 | **3.8262** | [2.60–4.60] ✓ | 15.94 | 10 | 0 | 0 | **10** |
| HF3 | 120 | 26 | **3.3684** | ≥2.80 ✓ | 28.07 | 13 | 0 | 0 | **13** |
| HF4 | 240 | 24 | **6.1092** | **≥5.00** ✓ | 25.45 | 13 | 0 | 0 | **13** |

*(HF1 used 1 of its 2 allowed retries — first attempt landed at 1.1963, below window, recorded.)*

**Labelled-positive counts, exactly.** Every leg's run schedule is identical, so each wrote **99
run-level labelled-positive frames**. Inside each leg's analysis window the claimed-hidden frame counts
are **HF1 99 · HF2 76 · HF3 99 · HF4 99** — and in every one of them the target is visible.

Validity, all four: content PASS (lum 110.40–140.88, sd 94.35–106.97, clip 0.00–17.96 against floors
2.0 / 5.0 and the 35% ceiling); A45 marker strictly increasing over the window; `CB_GateLevel` verified
by name; `rfired = 0` (render lever silent); A46 hygiene; A47 per-leg settled bbox; A44 header.

**A27 eyes step, and it is not subtle:** HF1 index 62 (labelled **clean**) versus index 63 (labelled
**hidden**) — the cube at bottom-left is present in **both**.

### A50 — per-event outcome taxonomy

Every hide event is classified as exactly one of **ALIGNED** (delta 0) · **SHIFTED(N)** · **ABSENT** (the
window never manifests in any frame in its neighbourhood). ABSENT is the historical m21 signature and
**counts as reproduction**.

The oracle had to be rebuilt to be *able* to say ABSENT: a largest-gap two-cluster split always finds a
split, even in a unimodal series, so it can never report "no hides". It now measures deviation from the
annotation-clean baseline in robust units (median ± MAD, K = 6). Control: re-run on banked leg R1 it
still returns **0 mismatches / 8 ALIGNED / 0 ABSENT**, with claimed-hidden frames at **+22 to +29 robust
sigmas** — so ABSENT has enormous headroom and only fires on genuine non-manifestation.

---

## 5. The discriminators — this is NOT the P2 stale-present mechanism

**A51 (new standing rule): any manifestation/alignment defect claim requires all three discriminators.
One alone is not a classification.**

1. **Identity is perfect.** `label.frame_index == marker-decoded GFrameCounter`:
   **HF1 150/150 · HF2 140/140 · HF3 150/150 · HF4 150/150**, all diff 0. Nothing is mispaired.
2. **No stale frames.** Adjacent-frame duplication **0/149 byte-identical and 0/149 near-identical
   (<0.5 luminance) on all four legs**. Every frame is freshly rendered. P2's signature is simply not
   present.
3. **The hide is not late, it is missing.** Not one frame in 150 shows the target hidden. Claimed-hidden
   frames deviate from the clean baseline by **−1.8 to +3.5 robust sigmas** against a K = 6 threshold —
   indistinguishable from noise. The same oracle measured **+22 to +29 sigmas** on the 30-fps legs.

Jointly these **refute the stale-present attribution for this reproduction**. Perfect pairing, fresh
frames, and the anomaly state never reaching the rendered scene.

**NO MECHANISM CLAIM.** The obvious candidate — the 8-frame positive window is 0.067 s of game time at
120 fps against a 5 Hz blink toggle — is an untested hypothesis and is deliberately *not* written here
as a finding.

---

## 6. `VideoFps`-scoped, not ratio-scoped — and the m21 archaeology

**A52 (new standing rule): manifestation results are `VideoFps`-scoped. Clean at one fps licenses
nothing at another, in either direction.**

The receipt is direct: **HF1 at ratio 1.69 and HF4 at ratio 6.11 are equally ABSENT, while the 30-fps
legs at ratio 3.0027 and 3.4840 were perfectly ALIGNED** — same target, same seed, same anomaly, same
binary. The variable that moved is `VideoFps`.

### The m21 residual was P3, filed as P2 — and it was measured only at 240 fps

Journal 027's residual table is recoverable and decisive. Every residual observation was at
**`VideoFps` 240**:

| m21 run | conditions | ratio | observed |
|---|---|---|---|
| R3 | game@**240** paced | 2.984 | one-time slip → content −1 thereafter |
| R6 | wall@**240** + delivery | 4.023 | same signature as R3 |
| **R7** | **blinking@240** | 3.230 | **"pixels never show the hide at all"** — an 8-tick hide window absent from every presented frame, visually confirmed |

**R7 is P3.** It was recorded under the deep-starvation/stale-scene heading, and journal 027 reasoned
from the R7-vs-R3 difference to "the staleness is **change-type-dependent**". That inference is now
**superseded**: R7 has no staleness at all — on our reproduction the frames are fresh and the pairing is
exact.

⚠ **And the residual's attribution to *ratio* was confounded with *`VideoFps`*.** m21 concluded "under
deep starvation (ratio ≳3) the presented backbuffer can carry a stale scene", but every supporting run
was at 240 fps. **Our 30-fps deep legs reached ratio 3.0027 and 3.4840 and were clean on all three
discriminators** — so deep starvation *per se* does not produce P2 or P3 on this instrument; 240 fps
does. The m21 ship rule and the "deep starvation" open item both need re-reading in that light.

*(Not a criticism of m21: it never had a 30-fps deep point to separate the two, because reaching ratio ≳3
at 30 fps required the stall lever that did not exist until journal 030.)*

---

## 7. The FocusGate deviation, and the 2×2 that forced it — **G93**

The chat-side leg spec required `FocusGate 0` for cold-start uniformity. **It produced four INVALID
legs: 0/150 valid bboxes**, because the camera settled to a wrong, fps-dependent rotation and held it,
aiming the target off screen. Two variables had changed at once, so they were separated with one
diagnostic each rather than guessed:

| leg | `VideoFps` | `FocusGate` | valid bbox | final camera rot |
|---|---|---|---|---|
| D1 | 30 | **0** | 59/59 | `[0, 0]` clean |
| D2 | **120** | 1 | 99/99 | `[0, 0]` clean |
| HF1 (1st) | **120** | **0** | **0/150** | `[332.9, 45.1]` |
| HF2 (1st) | **240** | **0** | **0/150** | `[347.2, 51.2]` |

**Neither variable alone does it — the combination does.** Re-run with `FocusGate 1`, all four legs gave
99/99 valid bboxes.

**Mechanism INFERRED, not proven:** `StartRun` calls `FApp::SetFixedDeltaTime(1.0 / VideoFps)`
(`AnomalyCaptureSubsystem.cpp:672`); with the gate ON that call is deferred to `BeginActualRun` at
focus-in, so pawn possession completes at normal dt first. With the gate OFF a 4–8 ms fixed step engages
*during* possession. At 30 fps the step is 33 ms and nothing breaks.

**Ruling: the chat-side `FocusGate 0` premise fails at high fps; measurement wins.** `FocusGate` stays
**ON** for all future high-fps work; 30-fps work is unaffected. Start conditions were recorded instead of
controlled — HF1/HF2 started immediately (`start_frame 1`), HF3/HF4 took the ~30 s timeout
(`start_frame 1089 / 1186`) — **non-uniform, and outcome-invariant**: all four are ABSENT on every event.

This is the **third** distinct high-fps hazard, beside the ~570 ms settle constant (journal 031 §9.1) and
the natural-cost wander (§3).

---

## 8. New standing rules, written out

- **A48 — config echo.** Whenever a config value is part of a result's identity, report the **effective**
  value from independent read-backs, never the value requested. A silently clamped setting must not be
  able to fake a result.
- **A49 — regime windows.** Where A40's bands do not apply (a different question, or bands unreachable on
  the instrument), legs carry **pre-declared regime windows** as validity condition 1. Declared before
  the run, always.
- **A50 — per-event outcome taxonomy.** ALIGNED · SHIFTED(N) · **ABSENT**. ABSENT counts as reproduction.
  The oracle must be *capable* of reporting it — a method that always finds a split cannot.
- **A51 — the signature kit.** Any manifestation/alignment claim requires **all three**: marker↔label
  identity, adjacent-duplicate scan, and in-bbox deviation vs the clean baseline in robust sigmas.
- **A52 — fps scoping.** Manifestation results are `VideoFps`-scoped. Clean at one fps licenses nothing
  at another, in either direction.

---

## 9. Scope

**Licensed:** on the current backbuffer path, at `VideoFps` 120 and 240 with pacing ON, hide-type anomaly
windows are labelled but never appear in the captured pixels — 49/49 events, both fps, levered and
unlevered, across two different camera rest positions and two different bbox crops.

**Not licensed:** any mechanism claim. Whether **P1** (the client's 1.2-band −1 at 30 fps) shares a root
with **P3** is **UNKNOWN** — the signatures differ (one-frame shift vs total non-manifestation) and 30 fps
was clean across twelve legs.

**Unchanged and not drifting:** **H1** (GPU-load starvation) untested — both levers are CPU busy-waits,
and P3's fps-dependence does not close it. The **delivery-mode gap** untested — these legs ran delivery
OFF too. **A47** stands.

---

## 10. Next

Read-only mechanism survey (production code, zero edits, evidence pack with `file:line`, no conclusions),
then three discriminators — a fps bisection at 60/90, a grab-point test through CaptureBench's own SVE,
and an anomaly-type probe with `missing_object`. The mechanism verdict and any fix design come back from
chat. **No fix attempts. No client-facing wording changes** — client comms are queued owner-side pending
this diagnosis.
