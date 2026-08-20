# 2026-08-20 — 047 — `m28`: capture output resolution as a DOWNSCALE ON WRITE

**Stage 0 (survey S0) → Stage 1 (plan + pre-declared gates) → Stage 2 (build).**
Gates NOT YET RUN — the owner presses Play. This journal covers everything up to and
including the build; the gate results belong in a later entry.

---

## 1. What this session did, in one read

`m28` adds ONE capture knob — a target output **HEIGHT** — implemented as a resample at the
**write step**. The render stays native. It was preceded by a read-only survey (S0) whose job
was to produce the real list of surfaces an output-resolution change touches, rather than a
guess at that list.

**The survey is the reason the design is shaped the way it is.** Its most decision-relevant
finding was that the `m26`/`m27` stencil mask counts at the view's render resolution and
**never sees the capture output buffer**, so a write-time downscale **structurally cannot
reach the veto**. That is what made this shape safe, and it was established FROM SOURCE
before any code was written.

---

## 2. Survey S0 — the findings that shaped the milestone

### 2.1 THE MASK CANNOT BE REACHED BY A WRITE-TIME DOWNSCALE — SOURCE-READ

`AnomalyMaskSceneViewExtension.cpp`: the mask RT is allocated at `SceneColor.ViewRect.Size()`
at the Tonemap pass, the fullscreen pass runs over that rect, and the per-tag count is the
pixel loop over that RT. `AnomalyMaskMeasure.cpp` records `ViewportPixels` from the same rect
and assigns the tri-state from `MaxCount`. **No read of `Job.Width`, `Frame.Width` or any
write-step dimension exists anywhere in the mask files.** The write step runs on a thread pool
from a different data path.

Cross-checked against banked evidence: the `m27` play-gate log carries
`M23 PASS … viewRect=1280x720` on every armed frame against `RESOLUTION DELTA … grabbed
1280x720`. Equal today, on 2 legs, at one resolution.

### 2.2 THREE GRAB PATHS, THREE DIFFERENT DIMENSION SOURCES

| path | dimensions come from |
|---|---|
| SVE (default since `m25`) | `SceneColor.ViewRect` at the **VisualizeDepthOfField** pass |
| backbuffer (`IAI.Capture.SVE 0`) | the **Slate arranged geometry** of the `SViewport` widget, clamped to the backbuffer |
| sync fallback / `IAI.Capture.Shot` | `FViewport::GetSizeXY()` |

They are **not the same number by construction**. The code already knew this and already
instruments it — the RESOLUTION DELTA (3-rect) line, whose own text warns that a non-zero
`dW_vp` means the reported resolution disagrees with the delivered pixels. On both banked
`m27` legs all three read 1280×720.

### 2.3 THE FIELD CLASSIFICATION — the point of the survey

Everything the projection pipeline produces is computed in **normalised [0,1] screen space**
(`ProjectBoxToNormalizedRect`) and therefore **already resolution-invariant**: `bbox_norm`,
`bbox_valid`, `coverage_ratio`, `coverage_pct`, `visible_positive`.

**Pure geometry**, rescaling by multiplication: `labels.jsonl` `width`/`height`,
`bbox_px` (`[x, y, WIDTH, HEIGHT]`, clamp-then-scale, and the clamp bound is the same `W,H` it
scales by, so the rescale is exact), `annotation.video.resolution`, `run.json` `viewport`.

**Measurements** — values produced by counting or sampling: `mask.provided`, `vetoed_events`,
`translucent_vetoes`, `translucency_unknown_vetoes`, the `mask_*_discards` counters, **and the
event SET in `annotation.json` itself** since `m26` slice 3.

🚨 **THE ONE CHANNEL BY WHICH A PIXEL DIMENSION REACHES THE INVARIANT FIELDS:**
`FAnomalyViewInfo::AspectRatio` is `GetViewportSize().X / .Y` (`AnomalyViewport.cpp`) and it
feeds the projection matrix. **An aspect-preserving downscale leaves every invariant field
correct; a non-preserving one silently invalidates all of them.** This finding is why `D1`
exists.

### 2.4 `video.resolution` WAS DESCRIBING THE WRONG THING

It came from `GetViewportSize()` read once at `StartRun`, not from any written frame. And
**in delivery mode `labels.jsonl` is not written at all**, so a delivered dataset contained
**no artifact recording the true dimensions of its own pixels**. That is the defect `m28`
fixes, and it is why `GATE I` was later added.

### 2.5 THE DASHBOARD FORMAT CONTROL — RECOLLECTION CONFIRMED, AND IT WORKS

It is a real capture-panel control that sends `format` in `capture_start`, and the server and
engine writer both honour it end to end. **Not a silent no-op.** Two wrinkles fell out, filed
as `W1` and `W2` — see §5.

---

## 3. The locked design (`D1`–`D10`), and the two collisions found in it

`D1` no width parameter · `D2` `0` = native, snap even, never upscale · `D3` derive per-frame
from the frame in hand · `D4` `video.resolution` from the first written frame, no new delivered
fields · `D5` area/box resample · `D6` one resample site · `D7` four-level precedence with an
unconditional echo · `D8` dashboard control · `D9` fix `W1` · `D10` file `W2`.

### 3.1 ⚖ `D7` CONTRADICTED `D3`, AND THE RULING WAS TO **CUT**

`D7` asked the `StartRun` line to name native `WxH` and output `WxH`. **At `StartRun` no frame
has been grabbed**, so the only available source for native `WxH` is `GetViewportSize()` — the
exact source `D3` forbids and the exact source §2.4 found to be describing the wrong thing.

⚖ **OWNER RULING: the `StartRun` line carries REQUESTED HEIGHT AND PROVENANCE ONLY.** No
`WxH`, no predicted pair. **The reasoning is worth keeping: the viewport-versus-frame
disagreement is ALREADY instrumented** by the RESOLUTION DELTA (3-rect) line, which read
`dW_vp=0` on both `m27` legs. **A second predictor of the same quantity is duplication, not
evidence.** The authoritative pair is measured from the first written frame and logged there.

📌 **The proposal I brought was to ADD a labelled prediction. The ruling was to REMOVE. The
removal is better and the reason is transferable: when a log line cannot honestly answer a
question, deleting the question beats hedging the answer.**

### 3.2 ⚖ `GATE B` WAS RE-AIMED AT THE MEASURED LINE (amendment, before any leg ran)

`GATE B` originally graded `frame_00000`'s file header against "the output size named in the
`StartRun` echo". With §3.1 that number no longer exists. It now grades against the
**first-frame MEASURED line**. **This is a TIGHTENING** — it tests the whole chain from
grabbed frame to written file, and that pair is the one `labels.jsonl`, every `bbox_px` and
`video.resolution` are computed from. `GATE E` is unaffected because the `StartRun` line
stays UNCONDITIONAL.

### 3.3 ⚖ `IAI.Capture.Start` HAD NO FREE POSITIONAL SLOT — the `oh=` token

Args 6+ are forwarded to the anomaly VERBATIM (`m23`'s contract). Inserting a positional
output-height at slot 6 would have been a **silent breaking change** to a documented signature.

⚖ **OWNER RULING: a NON-POSITIONAL token `oh=<n>`, scanned across the argument list and
STRIPPED before the anomaly args are collected.** ⚠ **`h=` was explicitly REJECTED** as too
likely to collide with an anomaly's own argument. **This is a narrow, deliberate exception to
verbatim forwarding: exactly one token prefix is consumed by the capture command, and it is
recorded here so it is not later discovered as drift.**

### 3.4 🆕 `GATE I` WAS ADDED — the fix must reach delivery mode

Every other leg runs delivery OFF because `GATE D` needs `labels.jsonl`. Without a delivery-ON
leg, **the fix would never be tested in the mode client captures actually use** — see §2.4.
Appended in the same amendment commit, with its failure branch written before any result
existed: a fail means `video.resolution` is correct only in the mode nobody ships, which is
the original defect surviving inside its own fix (`G139`'s shape).

---

## 4. `m28` AS-BUILT

### 4.1 The single resample site, and the report `D6` demanded

`D6` said *"if they do not converge on a single site, REPORT THAT rather than duplicating the
resample in three places."* **They do not converge.** Paths 2 and 3 already share
`CaptureLabeledShot`; path 1 does not join them. The nearest common function is
`AnomalyPreview::EncodePixels` — **and it is contaminated**: `AnomalyPreviewTee.cpp` calls it
too, so resampling there would silently downscale the live preview, which is the coordinate
frame the dashboard's click-to-select maps against.

**As built: ONE implementation (`AnomalyLabel::ResampleAndEncodeBGRA`), TWO invocation points,
and the preview untouched BY CONSTRUCTION.** `AnomalyPreview::CaptureGameViewportEncoded` is
now unreferenced inside the plugin; it is public API and was **left in place**, not deleted in
the same change that adds a resample.

### 4.2 One derived pair, four consumers, no new scaling code

`AnomalyLabel::DeriveOutputSize` is the only place `D2`'s rules exist. Called at the two
"frame in hand" points. The pair then reaches the resampler, `labels.jsonl` `width`/`height`,
**every `bbox_px`**, and `video.resolution`. 🎯 **`bbox_px` needed NO new code** — it is
already computed from the same `W,H` — which is the entire reason the pair is threaded rather
than re-derived. Two derivations would be two chances to disagree.

### 4.3 `GATE A`'s counter — the ordering that matters

`resamples_performed` increments **only on the successful-write branch**, in lockstep with
`FramesWritten`. ⚠ **Incrementing it before the encode would have broken `GATE A`'s exact
equality on a dropped frame** — precisely the near-miss the gate exists to catch. It is
internal/log only and is deliberately **not** in `run_summary.json`.

### 4.4 Files changed

**Plugin:** `AnomalyLabelWriter.{h,cpp}` (derivation + resample + `CaptureLabeledShot` +
`EncodeAndWriteFrame` + `IAI.Capture.Shot`) · `AnomalyAsyncWriter.{h,cpp}` (job pair, counters,
first-written + mismatch warning) · `AnomalyCaptureSubsystem.{h,cpp}` (ini read, precedence,
echo, per-frame derivation on both paths, resample summary, `video.resolution` source,
`IAI.Capture.OutputHeight`, the `oh=` token) · `AnomalyControlServerSubsystem.cpp`
(`outputHeight` field + the `W1` fix).
**Dashboard:** `CapturePanel.tsx` only — `AnomalyClient.captureStart` already spreads opts, and
`protocol.ts`/`validate.ts` type only INBOUND messages.
**NOT ONE LINE:** `AnomalyViewport.*`, `AnomalyMaskMeasure.*`,
`AnomalyMaskSceneViewExtension.*`. That is `m28`'s premise and `GATE D`'s control.

### 4.5 Build

`StackOBotEditor Win64 Development` — **clean, 7 actions, zero errors, zero warnings.**
Dashboard `tsc && vite build` clean; **69/69 vitest pass.** The comment stripper reports
`changed: 0` over 78 files. Diffstat read before every commit (`G115`).

⚠ **THE DASHBOARD CONTROL WAS NOT VISUALLY VERIFIED BY ME.** `App.tsx` returns `ConnectScreen`
until a WebSocket connection to a running game succeeds, so `CapturePanel` is unreachable
without PIE, and **the owner presses Play**. It is in his runbook instead. Stated rather than
implied.

---

## 5. `W1` FIXED, `W2` FILED

**`W1` — FIXED (`D9`).** The server chose format with
`bPng = !Format.Equals("jpeg")`. Three defects in one line: `"jpg"` silently gave **PNG** while
the console accepted both spellings; every unrecognised string became PNG **with no warning**;
and the failure is **invisible in the artifact** because `run.json` faithfully records `"png"`,
the value actually used. Now an explicit three-way with a **loud** no-match branch naming the
value and the fallback. **Absent/empty stays silent** — a caller omitting the field is taking
a default, not making an error. → **`G144`**.

**`W2` — FILED, NOT FIXED (`D10`).** The format is never echoed back: no field in
`ControlSnapshot` and none in `capture_stopped`, so the dashboard's select is fire-and-forget.
It IS in `run.json` afterwards, so this is a latency of feedback, not a lost record. ⚖ **Filed
as a second addendum under `G139`, NOT in `docs/invisible-anomaly-mechanisms.md`** — that
ledger is for invisible-anomaly MECHANISMS only, and a missing echo is `G139`'s own subject.
Owner-ruled.

---

## 6. 🆕 GOTCHAS MINTED — `G143`, `G144`

**`G143`** — `git tag -l --format='%(objectname:short)'` prints the **TAG OBJECT's** hash for
an annotated tag, not the commit's. The cold-start instruction sheet carried that form, so
`m26`/`m27` read `4328961`/`1756f52` against the true `d6bee7a`/`4a92962`. ⚠ **The cost is not
the confusion, it is the HALT** — bootstrap contracts correctly say to stop on a mismatch, so a
defect in the CHECK spends the session's most expensive response on a non-event. `G142`'s shape
one level up. Use `git rev-parse --short <tag>^{commit}`. **Folded into `CLAUDE.md`'s workflow
rules by owner ruling**, so a cold session gets the right form without reading the gotcha.

**`G144`** — a parser that maps "everything else" to a default converts a typo into a silent
behaviour change. Full analysis at the gotcha; the transferable rule is **never write
`bFlag = !(x == "oneValue")`**, and **distinguish ABSENT from WRONG**. ⚠ **`G96` again: the
fallback branch existed for the whole life of the feature and had never once been observed to
fire, because firing it produced no output.** `GATE G` therefore requires the warning to be
*demonstrated* firing, not merely present in source.

---

## 7. Declared residuals and scope limits

⚠ **THE ASPECT RESIDUAL, DECLARED IN ADVANCE (`P-C`).** Because `H_out` is snapped even and
`W_out` derived then snapped even, the delivered image's aspect can differ from the render's
by up to about **one part in a thousand**. **Labels stay EXACTLY consistent with the delivered
pixels** — `bbox_px`, `labels` `width`/`height` and `video.resolution` all use the SAME derived
pair. What drifts is only the render projection aspect versus the output image aspect.
**Sub-pixel, stated before measurement, not gated.**

⛔ **NO ALIGNMENT CLAIM IS EXTENDED.** `m25` certified alignment at **1280×720 and 1281×721
only** (`B1`'s pixel `CALIB_BBOX` — an inherited gap). `m28` can produce sizes outside that set
and **certifies nothing at them.** This must appear in the tag scope statement.

📌 **A PARTIAL CORRECTION TO MY OWN S0 "UNDECIDABLE".** S0 recorded screen-percentage behaviour
as undecidable. The **S4-1 resolution matrix already measured it** for the capture rect: legs
`M4a`/`M4b` at `r.ScreenPercentage` **50** and **170** both delivered 1280×720 with `dW/dH 0/0`.
**What remains genuinely open is narrower than I stated:** whether the mask's **Tonemap**-stage
view rect equals the SVE's **VisualizeDepthOfField**-stage view rect under a non-100% screen
percentage. The mask did not exist at S4-1, so that leg has never been run. **The narrower
question is the honest one; the broader one was over-cautious.**

⚠ **A zero-frame session now writes `video.resolution: [0,0]` plus a warning.** This is a
deliberate behaviour change: it does **not** fall back to `GetViewportSize()`, because that is
the quantity §2.4 found does not describe the delivered image, and a fallback would reinstate
the exact wrongness in the one case nobody checks. Owner-ruled.

📌 **`G-9` DELIVERY-MODE ORTHOGONALITY IS PREDICTED, NOT PROVEN, UNTIL `GATE I` RUNS.**

---

## 8. Encoder copies — REPORTED, NOT FIXED

Three copies of `encode_watcher.py`. `anomaly-dashboard/host-tools/` and `_M2Smoke/host-tools/`
are **byte-identical**. **`D:\IntrusiveAnomalies\host-tools\` — the one the owner runs — is an
older fork**: it has a hardcoded `CAPTURES_ROOT = r"E:\AnomalyCaptures"` as the `--root`
default (the canonical copy makes `--root` **required**, the `m21`-era hardening), and it lacks
`touch_heartbeat` / `--heartbeat` entirely, so `Run.bat`'s self-check cannot distinguish "the
watcher is running" from "its window is open but the process died". **No encoding behaviour
differs** — both carry the identical ffmpeg line including the even-pad filter. `m28` is
unaffected by which copy runs. **Deliberately not fixed; out of scope this session.**

---

## 9. State after this session

| | |
|---|---|
| plugin | implementation committed and pushed; **NOT TAGGED** — the owner smokes first |
| tags | `m26` → `d6bee7a`, `m27` → `4a92962`. **NEITHER MOVED.** |
| `feature/stencil-capture` | **UNTOUCHED at `76cac74`**, never checked out |
| AnomDash | `CapturePanel.tsx` + rebuilt `dist/`, committed and pushed |
| gates | **NONE RUN.** All of A–I await the owner's play session. |
| PIE | **not started by me at any point** |

⛔ **`H6` REMAINS DOCUMENTED, NOT FIXED. `P6` DID NOT MOVE. NO RATIO OR THRESHOLD EXISTS
ANYWHERE IN THE CODE OR THE DOCS.** `m28` touches none of that line of work — by design, and
`GATE D` is the control that proves it.
