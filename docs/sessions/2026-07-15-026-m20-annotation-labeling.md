# 2026-07-15 — 026 — m20: annotation.json labeling — blinking hidden-state staleness, order-independent subtype, subtype mirror

Base: plugin `c8aa3fa` (tag `m19`, in sync with origin), dashboard `f978f1b` (untouched). **No commit this turn.**

## Summary — what the three reported bugs actually were

The investigation turn measured all three against pixel ground truth in a local package. **One of the three was not a
bug, and the other two had different mechanisms than the brief's locked diagnosis.** The fixes below are the ones the
evidence supports; the gates the brief specified (G1–G3, pixel-exact) are what prove it.

| Reported | Locked diagnosis | What it actually is |
|---|---|---|
| **A** missing_texture range shifted one earlier | annotation is a SEPARATE path m18 didn't reach; re-anchor it | **NOT A BUG — annotation is already pixel-exact.** No code change. |
| **B** blinking drops the last hidden frame | range END off-by-one (exclusive vs inclusive) | **Blinking's hidden state was sampled ONE GAME TICK STALE.** No range-end exists to fix. |
| **C** blinking "flicker"; missing_texture "" | same root as B; B's fix resolves it | **Two independent causes**, neither shared with B: order-dependent transition counting, and an unset subtype. |

## Bug A — NOT A BUG. Deliberately no code change.

**Claim:** annotation.json's non-hide `affected_frames` is computed on a separate path from the labels.jsonl span that
m18 corrected, and still carries a whole-range one-earlier shift.

**Refuted, twice over:**
1. **Code:** `ProcessCompletedFrames` calls `BuildLabelRecordForSnapshot(*Snap, …)` (labels.jsonl) at
   `AnomalyCaptureSubsystem.cpp:800` and `AccumulateFrameEvents(Snap->Fires, Snap->FireHidden, …)` (annotation) at
   `:802` — **adjacent lines, same function, same m18-corrected `Snap`**. There is no separate path and no independent
   range computation; one accumulator feeds one writer.
2. **Measurement (m19, session_20260715-162507), per frame, pixels decoded from the PNGs:** annotation
   `frame_indices [3..10]` == pixels-corrupted `[3..10]` == labels present `[3..10]`, **zero disagreement on any
   frame**; annotation index 3 ↔ `frame_00003.png` ↔ visibly checkered.

**Prime explanation for the field observation: 0-based vs 1-based frame numbering.** The whole pipeline is 0-based —
images are `frame_%05d.png` from `00000`, and `host-tools/encode_watcher.py`'s docstring (:17-18) states "Frame
numbering is session-local 0-based … so the ffmpeg image2 demuxer globs them directly". A video player that labels the
first frame "1" shows annotation index 15 as "frame 16" — exactly the reported `actual 16..20 vs annotation 15..19`.
**The tell:** *pre*-m18 the annotation ran one frame LATE, which accidentally MATCHED a 1-based reading; m18 made it
0-based-pixel-exact, which BREAKS that reading. So this symptom appearing right after m18 is m18 working correctly.

**Why we did not "fix" it:** shifting the range +1 would re-introduce the exact bug m18 fixed, in the client
deliverable, and would make the brief's own G1 gate fail. **G1 already passes on m19 with zero code change.**

**Open question for the owner (blocking any change here):** how was "actual frames 16..20" determined? Decisive
10-second test on the affected session: open `frame_00015.png` and `frame_00016.png` — whichever is the FIRST frame
showing the corruption is the correct `start_frame`. If it is `frame_00015.png`, annotation is right and the
comparison was 1-based. If the client's tooling is 1-based, making the indices 1-based is a **spec change** (all
indices + `start_frame`/`end_frame` + docs + the slicer contract), not a bug fix — and it must be done at the emit
layer, never by shifting the sample.

## Bug B — blinking's hidden state was ONE GAME TICK STALE (not a range end)

**There is no range end to correct:** `frame_indices` is emitted verbatim from `E.FrameIndices`
(`AnomalyLabelWriter.cpp:345-355`); no exclusive/inclusive comparison exists anywhere on this path.

**Measured mechanism (session_20260715-183542):** `annotation(gframe G) == pixels(gframe G-1)` for every verifiable
frame — annotation hidden `{4,5,6,10}` vs pixels hidden `{4,5,9,10}`; **both** blink edges late by exactly one, in
every burst. Blinking toggles inside the **injector** subsystem's tick (`AnomalyInjectorSubsystem.cpp:169-173`,
`Pair.Value->Tick(DeltaTime)`) — a *different* `UTickableWorldSubsystem` that ticks **after** the capture subsystem —
so m18's `FinalizeArmedLabel` sampled `IsHidden()` before blinking had toggled for that frame, while the frame then
rendered *with* the toggle. This is exactly the risk flagged as unmeasured in m18 / G78; it is now measured.
`missing_object` is immune because its hide runs in `Apply` ← `BeginFire`, **inside** our own Tick, before the sample
(m18 G3: 0/100 mismatches). ⇒ hide-type splits: **Apply/Revert-driven = correct; self-ticking = one tick stale.**
At a burst tail the uniformly-late edge drops the final hidden frame out of the window — the reported "tail clip".

**Fix:** defer the hidden sample by one tick. `FinalizeArmedLabel` now records only `Fires`/`FirePos` and flags
`bHasDeferredHidden`; the new **`SampleDeferredHidden()`** fills `FireHidden` at the **top of the next Tick** — the
first moment the world state equals what the previous frame rendered (every subsystem has ticked). It runs *before*
`ProcessCompletedFrames` (which is the earliest a frame armed at N can be drained), plus at the top of `FinishRun`
(before `RevertAllLiveFires`) to cover a `StopRun` arriving between ticks.

**Blast radius is provably zero on the m18-validated path:** `FireHidden` never reaches labels.jsonl (it is not a
parameter of `BuildFrameLabelRecord`); it feeds **only** annotation's hidden set and transition count. `Fires`,
`FirePos`, `AffectedFrames`, coverage and phase timing are untouched — confirmed by G5.

## Bug C — two independent causes, neither shared with B

The brief's "B and C share a root; fixing B's boundary math resolves C" is not the case: **a uniform stale shift
preserves the transition count**, so B's fix changes C by nothing.

**C1 — the transition count was ORDER-DEPENDENT.** It was accumulated incrementally via `LastHidden` in *arrival*
order, but frames arrive out of session order: `Drain_RenderThread` fills `Completed` iterating `InFlight` in
**reverse** (`AnomalyFrameCapturer.cpp:128` → `:164`) while `PopCompleted` is FIFO (`:184`), so any drain with ≥2
ready frames — routine at DrainTail and under hitches — hands them newest-first and manufactures spurious
transitions → inflated → "flicker". **Fix:** the accumulator now stores `TMap<int32,uint8> HiddenByIndex` and
`WriteSessionAnnotationFile` derives **both** the hidden set and the transition count from the **sorted** key
sequence. Order cannot matter, and the hidden set + transitions now come from one source that cannot drift.
The derivation stays data-driven and unchanged (≤2 → `disappear_reappear`, ≥3 → `flicker`) — G3 proves it still
discriminates.
*Note:* the reported "single vanish → flicker" did **not** reproduce locally: `DefaultHz = 5.0f`
(`Anomaly_Blinking.h:28`) gives a 6-frame period, so the default 8-frame positive window genuinely contains ~2 hidden
blocks (3 transitions) and "flicker" is **correct** there. The order-dependence above is the credible mechanism for a
genuine single-vanish being misclassified; it is now impossible by construction.

**C2 — missing_texture subtype `""`.** `MapAnomalyToClient`'s else branch simply never set one
(`OutSubtype = FString()`). Now `OutSubtype = Id.ToString()` (owner-chosen: mirror the type).

## Files touched

Plugin: `AnomalyCapture/Public/AnomalyCaptureSubsystem.h` (`SampleDeferredHidden()` + 2 members),
`AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` (`FSessionEventAccum` → `HiddenByIndex`; `MapAnomalyToClient`
else-branch subtype; `Tick` + `FinishRun` call `SampleDeferredHidden()`; `FinalizeArmedLabel` splits;
`AccumulateFrameEvents` records per-index state; `WriteSessionAnnotationFile` computes order-independently).
Docs: this journal, `gotchas.md` (G81), `architecture.md`, `CLAUDE.md`.
**Untouched:** `IAnomaly`/injector/anomalies, `AnomalyLabelWriter` emit logic, `FAnomalyFrameCapturer`, the gating
rule, phase timing, settle-K, L=0, the preview tee. No new dependency; catalog stays 8. Dashboard untouched.

## D2 — every audited path, re-confirmed against the audit's "affected" set

| Emitter | Affected? | Action |
|---|---|---|
| labels.jsonl (per-frame span) | correct since m18 | **untouched** — byte-identical (G5) |
| annotation `affected_frames.frame_indices` NON-HIDE | **correct already** (Bug A refuted) | no change |
| annotation `affected_frames.frame_indices` HIDE-TYPE | **yes — one tick stale** | **FIXED** (B) |
| annotation `anomaly_subtype` (blinking) | **yes — order-dependent count** | **FIXED** (C1) |
| annotation `anomaly_subtype` (non-blinking) | **yes — empty** | **FIXED** (C2) |
| annotation `start_frame`/`end_frame` | derived from frame_indices | inherits the B fix |
| annotation `coverage_ratio` | from `Fires` | no change |
| run.json manifest | no ranges (StartFrame only) | no change |
| run_summary.json | `positive_frames` from `Fires` (m18-correct) | no change |
| `_debug`-derived indices | **does not exist** (grep = 0) | n/a |
| mp4 (encode_watcher.py) | 1:1, 0-based | no change (owns the numbering convention — see Bug A) |
| slicer (Stage 5) | not built; will read annotation.json | inherits all fixes |
| verify_capture / overlay_watcher | read labels.jsonl | no change (G6 green) |

## Gates — all in a local package (`Builds\MidRepro\Windows`), pixel-confirmed

- **G1 (Bug A) — GREEN, no change required.** missing_texture: annotation `frame_indices [3..10]` == pixel-corrupted
  `[3..10]` == labels `[3..10]`, zero disagreement per frame. Already true on m19; still true on m20.
- **G2 (Bug B) — GREEN.** Blinking, per frame: annotation hidden `{4,5,9,10}` == pixels hidden `{4,5,9,10}`;
  **zero "in pixels but not in annotation", zero "in annotation but not in pixels"**, across all bursts. The final
  hidden frame is included. Pre-fix the same capture gave `{4,5,6,10}` vs pixels `{4,5,9,10}`.
- **G3 (Bug C) — GREEN, derivation intact.** Single-vanish window (`IAI.Capture.Config 2 4 3 4 0`): **14/14 events =
  `disappear_reappear`**. Genuine multi-toggle (default 8-frame window, 5 Hz blink): `flicker`. missing_texture
  subtype = `missing_texture`.
- **G4 (every audited path) — GREEN**; see the table above. Only the paths the audit marked affected changed.
- **G5 (regression) — GREEN.** labels.jsonl m19 vs m20: identical label pattern
  (`...PPPPPPPP....` ×8), identical gframe cadence `[0,1,2,3,6,7,8,9,10,11,12,13,16,17,18,19]`, positives 65/65,
  frames 100/100. **The m18 correction is undisturbed**; settle-K / L=0 / phase timing untouched (not in the diff).
- **G6 (overlay) — GREEN.** `verify_capture.py`: 100 frames, 65 boxes, `present=True: 65 | visible-positive: 65 |
  present-but-off-screen: 0`.

## PARKED — future work (recorded, deliberately NOT done this milestone)

**Shared range-builder refactor.** Rationale: a single source of truth for every frame range so this off-by-one class
cannot recur per-path. **Deferred post-delivery by owner decision (D1)** — it touches every consumer including the
m18-validated labels.jsonl path, and the delivery needs surgical, low-blast-radius correctness. *Note from this
milestone: the annotation and labels paths already share one snapshot and one accumulator, so the practical drift risk
is lower than feared — the refactor is about consolidating the emit layer, not rescuing divergent computations.*

**Also parked (found, not fixed, not owner-approved):** `frame_count` is a **span, not a count** —
`Count = End - Start + 1` (`AnomalyLabelWriter.cpp:349`). For gapped hide-type sets it misreports: measured
`frame_count: 7` against 4 `frame_indices`. Contiguous non-hide sets hide it. Fix = `FrameIndices.Num()`, or rename
to `frame_span` if the span is what the client wants. **Owner decision — it ships in the client deliverable.**

## State

Code + docs written; G1–G6 green in a local package. **No commit.** On acceptance: strip comments (already clean),
commit as one `fix(capture)` + tag `m20`.
