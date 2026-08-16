# 2026-08-16 — 033 — P3 diagnosed: the blink clock (P3a) and the label fallback (P3b)

Base: plugin `2e0b7a8` (docs only). Bench: `CaptureBench` `163dd12`, **zero probe edits — fourth
consecutive turn**. **Production `AnomalyInjector` / `AnomalyCapture` BYTE-UNCHANGED.** No fix code was
written this turn; the fix plan is a separate deliverable.

Companion: journal 032 (P3 reproduced), journal 031 (I10 at 30 fps, clean).

---

## 1. Headline

P3 — *a labelled hide window that never appears in the pixels* — is **two independent defects stacked**:

- **P3a, the timing defect.** At high `VideoFps` the blink toggle never fires inside an event window, so
  the actor is **never actually hidden**. The pixels are correct; the scene genuinely contains no anomaly.
- **P3b, the labelling defect.** When zero frames sample hidden, the annotation writer **silently
  substitutes the frames where the actor was merely on screen**, converting a non-event into a
  full-window block of positive labels.

P3a is what makes the anomaly not happen. **P3b is what turns "nothing happened" into 99 poisoned
training labels**, and it is **anomaly-agnostic** — it will do the same for any hide-type event that
fails to manifest for any reason.

**The SVE migration would not have fixed either** (D-B, §4.2).

---

## 2. The read-only survey (evidence, with citations)

### Blink's clock

```
Anomaly_Blinking.cpp:58   Tick(float DeltaSeconds)
                    :65     Accumulator += DeltaSeconds        <- accumulated tick dt, no timer
                    :66-69   while (Accumulator >= HalfPeriodSeconds) { bHiddenPhase = !bHiddenPhase; }
                    :76      Actor->SetActorHiddenInGame(bHiddenPhase)
                    :44     HalfPeriodSeconds = 0.5f / Hz
Anomaly_Blinking.h  :28-29  DefaultHz = 5.0f, MaxHz = 60.0f    => default half-period 0.100 s
Anomaly_Blinking.cpp:26-43  rate IS configurable via an optional 2nd arg [hz] (usage :17)
                            — but the capture path passes no args
AnomalyInjectorSubsystem.cpp:160-175  forwards its own tickable DeltaTime to each active anomaly (:173)
Anomaly_Blinking.cpp:85-98  Revert() resets Accumulator, bHiddenPhase and bActive
```

### The label path for blink windows

```
AnomalyCaptureSubsystem.cpp:1139-1164  SampleDeferredHidden
                          :1162          FireHidden.Add((FActor && FActor->IsHidden()) ? 1 : 0)
                          :1427-1429     AccumulateFrameEvents -> Ev->HiddenByIndex[SessionIndex]
                          :1466          const bool bHideType = HiddenIdx.Num() > 0;
                          :1467          FrameIndices = bHideType ? HiddenIdx : Ev.AffectedFrames;
```

Per-frame hidden labels **are sampled from real actor state** (`:1162`) — they are not derived from
intent. The defect is one level up, at `:1466-1467`.

### Fixed timestep

```
AnomalyCaptureSubsystem.cpp:442  VideoFps = FMath::Clamp(InFps, 1, 240)
                          :672  FApp::SetFixedDeltaTime(1.0 / VideoFps)   in StartRun
```
⇒ the `DeltaSeconds` blink accumulates **is** `1/VideoFps`.

### `missing_object`'s route

```
Anomaly_MissingObject.cpp:35  SetActorHiddenInGame(true)  in Apply
                         :53  SetActorHiddenInGame(false) in Revert;  NO Tick override
```
Same state route and same `IsHidden()` sampling as blink; **different timing**.

---

## 3. Mechanism of record

### P3a — TIMING (adopted)

`FAnomaly_Blinking` accumulates forwarded tick dt (`Tick` :58-69) against
`HalfPeriodSeconds = 0.5/Hz` (:44, default 5 Hz ⇒ 0.100 s). Under capture, dt = `1/VideoFps`
(`SetFixedDeltaTime` :672, clamp :442). **When the active window's accumulated ticks × (1/fps) never
reach the half-period, the toggle never fires, `bHiddenPhase` stays false, and the actor never hides
(:76)** — scene-level invisibility, confirmed by D-B. `Revert` resets the accumulator (:85-98), so no
phase carries between events. `missing_object` (Apply-based hide, no `Tick`) is **immune** — confirmed
by D-C.

### P3a threshold bookkeeping — **NOT adopted**, pre-declared banked-data test

Candidate arithmetic is ~**9 available ticks per event** (apply tick + 8 window ticks):
`9/90 = 0.1000… ≥ 0.100` under float ⇒ toggles on the final tick(s); `9/120 = 0.075 < 0.100` ⇒ never
toggles. **Predicted signatures, stated before the read:** at **90 fps** events sample hidden **only at
the window tail** (last ~1 frame); at **60 fps** events sample hidden in **mid-window blocks** (toggle
every ~6 ticks). A mismatch leaves this bookkeeping OPEN and **does not block the fix**, because the fix
removes the seconds-vs-frames race entirely.

### P3b — LABELLING (adopted outright)

`AnomalyCaptureSubsystem.cpp:1466` infers `bHideType` from `HiddenIdx.Num() > 0` — **event type from
sampling OUTCOME** — and `:1467` **silently substitutes `Ev.AffectedFrames`** when zero frames sampled
hidden, converting non-manifestation into full-window positives.

**Fingerprint on the record:** at 30 fps claimed sets are **GAPPED** (`[4,5,9,10]` — the sampled shape);
at 120/240 they are **8-CONSECUTIVE** (`[3..10]`, `[15..22]` — the `AffectedFrames` shape).

This is the dataset-poisoning amplifier, and it is **anomaly-agnostic**. → **G94**.

---

## 4. The three discriminators

### 4.1 D-A — fps bisection: a THRESHOLD, not a gradient

**A survey-derived expectation was stated before the run so that it could be wrong:** *"the transition
sits between 60 and 90 fps, because an 8-frame positive window is 0.089 s at 90 fps, under the 0.100 s
half-period."* ⚠ **FALSIFIED** — 90 fps manifests. On the record as a failed prediction, not quietly
dropped.

| leg | ratio | ALIGNED | SHIFTED | ABSENT |
|---|---|---|---|---|
| 60 fps | 0.9999 | **12** | 0 | 1 (truncated) |
| 90 fps | 1.2904 | **6** | 0 | 1 (truncated) |
| 120 fps (HF1) | 1.6916 | 0 | 0 | **13** |

**The cliff is between 90 and 120 fps, and there are ZERO SHIFTED events anywhere** between clean-30 and
absent-120. It goes straight from fully aligned to fully absent.

**A50 ADDENDUM — TRUNCATED.** The single ABSENT at 60 and 90 fps is the *same* final event
`[147,148,149]`: the run hits its 150-frame cap mid-burst before the hide phase. **Events truncated by
the frame cap are classified TRUNCATED and excluded from taxonomy counts.**

### 4.2 D-B — grab-point test: the locked prediction CONFIRMED

**True dual capture, one run:** production backbuffer + CaptureBench SVE simultaneously. S1 rig verified
functional at `163dd12` (SVE 900 frames, 1280×720, `fmt=18`, **100.00% coverage, 0 gaps**). 150/150
production frames matched to an SVE twin **by decoded marker `GFrameCounter`**, then measured with **one
oracle**:

| grab point | baseline | MAD | frames flagged hidden | dev on claimed-hidden frames |
|---|---|---|---|---|
| **BACKBUFFER** | 1.4513 | 0.01190 | **NONE** | −3.3 … +2.5 σ (n=99) |
| **SVE (SceneColor)** | 1.4515 | 0.01190 | **NONE** | −3.3 … +2.5 σ (n=99) |

**SceneColor also shows the object VISIBLE during labelled windows. P3 is scene-level and
grab-point-independent — the SVE migration would NOT cure it.** The two grab points agree to four
decimal places on the same frames.

**Two failed attempts, recorded rather than hidden:** (1) budgets did not overlap — the SVE captured
`gfc 1..900` while the focus gate held production until frame 2156; (2) with a 4000-frame budget the
SVE's 720 KB/frame write load **starved production's async writer and it wrote 0 PNGs**. Fixed by forcing
the game window foreground so the gate released at once and both captures overlapped (production frames
133..337 inside the SVE's 1..795). → **G95**.

### 4.3 D-C — anomaly-type probe: `missing_object` MANIFESTS

`missing_object`, 120 fps, same target and method: ratio 1.6966, **8 ALIGNED, 0 SHIFTED, 0 ABSENT**.
At the very same `VideoFps`, a hide-type anomaly that hides in `Apply` is frame-exact while the one that
toggles in `Tick` never manifests. **This pins P3a on the toggle clock.**

⚠ **P4-CANDIDATE, evidence-only, designation deferred:** this leg shows **41/96 shift-0 mismatches while
all 8 event edges are ALIGNED** — a tail-length disagreement between the claimed and pixel hidden sets,
**distinct from P3 and from P1**. Not conflated with either anywhere. Characterisation is a banked read;
no mechanism reading is offered here.

---

## 5. Oracle change, and the rule it produced

The luminance-settle heuristic was confounded by `missing_object`, which moves whole-frame luminance
itself — it collapsed D-C's analysis window to 3 frames. Settle is now computed on **annotation-clean
frames only**, plus an empty-baseline guard. Analysis-side only; **zero probe edits**.

**A53 (new standing rule): any oracle/analysis change must be re-verified against one known-ALIGNED and
one known-ABSENT control before its results are used.** Receipt for this change: banked R1 still returns
**8 ALIGNED / 0 mismatches**, banked HF1 still returns **13 ABSENT**.

---

## 6. Fix direction (constraints; the plan is a separate deliverable)

- **F-LABEL** — first, independent, and the guard. Hide-type events with **zero** sampled-hidden frames
  emit **zero** positive `frame_indices`. The event row is **kept**, with an additive
  `manifested: false`, a loud per-event warning, and a session-summary non-manifested counter.
  **Hide-type identity must come from the same existing routing knowledge that sends an event into
  `SampleDeferredHidden` — never inferred from sample outcomes.** `IAnomaly` stays **LOCKED**.
- **F-BLINK** — half-period defined in **FRAMES** (integer), **default 3**, chosen to reproduce today's
  30 fps cadence exactly (0.100 s × 30 = 3 frames) so the shipped-default dataset shape at 30 fps is
  unchanged. **Known and accepted consequence: 60/90 fps cadence changes versus today** — both are
  valid, and frame-space consistency is the point, because the dataset is frames and a detector sees
  frame sequences.
- **Milestone: `m23`** — the next free number in the repo's own tag ledger (highest existing tag is
  `m22` at `5fef60e`; `m13` was never a plugin tag). Looked up, never assumed — that is why the
  milestone is named **P3-fix** and the number is resolved from the ledger.
- **SEQUENCING RULING: P3-fix lands and validates BEFORE S3 starts.** Client-facing dataset poisoning
  outranks an internal migration, and **D-B proves the SVE is orthogonal to P3**, so nothing technical
  forces the other order. S3's matrix then validates on an already-fixed path.

---

## 7. Scope and what stays open

**Adopted:** P3a (timing) and P3b (labelling) as above.
**Not adopted:** the P3a threshold arithmetic (§3, pre-declared banked test).
**Unchanged and untested:** **H1** (GPU-load starvation — both levers are CPU busy-waits), the
**delivery-mode gap** (every I10/HF leg ran delivery OFF), **A47**, and **P1** — the client's one-frame
shift at ratio ≈1.2, 30 fps — which remains **NOT reproduced** across twelve legs and is **not** claimed
to share a root with P3.
