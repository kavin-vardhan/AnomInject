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

## 9. State after the BUILD stage (superseded by §16)

| | |
|---|---|
| plugin | implementation committed and pushed; **NOT TAGGED** — the owner smokes first |
| tags | `m26` → `d6bee7a`, `m27` → `4a92962`. **NEITHER MOVED.** |
| `feature/stencil-capture` | **UNTOUCHED at `76cac74`**, never checked out |
| AnomDash | `CapturePanel.tsx` + rebuilt `dist/`, committed and pushed |
| gates | **NONE RUN** at that point. |
| PIE | not started by me at any point |

---

## 10. HOW THE GATES WERE ACTUALLY RUN — the M0 split, exercised properly

⚖ **The owner asked whether Code could run the legs itself. It can, and `G136` is precisely why
that was not obvious:** `G136` established that **none of the bridge's 62 endpoints starts Play
or Simulate** — but that is a statement about STARTING PIE, not about driving it. **The M0 gate
split has always been "the owner presses Play; Code drives everything after that."**

**As run:** the owner pressed Play twice (once per PIE session) and did nothing else. Code issued
every console command through `unreal-mcpython`'s `util_execute_python` →
`SystemLibrary.execute_console_command(get_game_world(), …)`, read every log line and artifact
from disk, and drove the two `GATE G` wire sends through a **stdlib-only WebSocket client**
written for the purpose (`ws_send.py` — raw handshake + masked text frame, no dependencies).

🚨 **BINARY IDENTITY WAS ESTABLISHED BEFORE ANY LEG, AND THE FIRST ATTEMPT AT IT WAS WORTHLESS.**
The check was `get_console_variable_int_value(name) is not None` — which is true for **every**
string, including a garbage one, so it could not fail. The log proved it: *"Failed to find console
variable 'IAI.Capture.OutputHeight'"* for all of them, because these are console **commands**, not
variables. **Re-done as a positive observation with a negative control:** issuing
`IAI.Capture.OutputHeight` with no args produced its usage line; `IAI.Capture.ThisCommandDoesNotExist`
produced nothing. **That is what proved the editor had the new module and not a stale DLL.**

### 10.1 ⚠ LEG CONDITIONS — NOT DEFAULTS, AND NOTHING BELOW CHANGED A SHIPPED DEFAULT

Recorded loudly because a future reader must not infer either of these is the product's behaviour.

| condition | why | shipped default |
|---|---|---|
| `IAI.Capture.FocusGate 0` | so unattended bridge-driven runs begin deterministically instead of waiting on window focus | **ON, unchanged** |
| `IAI.SetMinScreenCoverage 0` | see §12 — at 6 % the auto-pool selects nothing in an 875×869 PIE panel | **6 %, unchanged** |

⛔ **NEITHER DEFAULT WAS EDITED IN CODE OR INI.** Both were session-scoped console overrides, and
both were **identical across every compared pair**, so neither can explain any result. ⚠ **A 6 %
threshold is NOT thereby shown to be wrong, and the focus gate is NOT off in a delivered build.**

---

## 11. GATE RESULTS — ALL NINE PASS

Build under test: plugin `c25bf59`, editor build, DLL 23:53:36. Map `CB_GateLevel`, PIE panel,
**native 875×869**, seed 7 on every leg. Banked at `_bench_sessions_bank\M28_GATES\`.

| gate | result | the number that decides it |
|---|---|---|
| **A** | PASS | native `resamples_performed = 0`; downscale `30 == 30` framesWritten. Exact both ways, and `12/12` on the three `F` legs |
| **B** | PASS | `frame_00000.png` IHDR read off disk = **362×360** = the MEASURED line's output size |
| **C** | PASS | see §11.1 — the divergence IS the proof |
| **D** | PASS, **non-vacuously** | `bbox_norm` rows differing **0**; `bbox_px` rows differing **19**; 19 valid bboxes/leg. See §12 |
| **E** | PASS | four distinct provenance strings, each naming the right level |
| **F** | PASS | `361→364×362` (snapped up), `1080→native` (no upscale), `0→native` with **PER-RUN** provenance |
| **G** | PASS | `"jpg"`→JPEG; `"banana"`→PNG **and the warning demonstrably fired** |
| **H** | DONE | native PNG + native JPEG pair, same seed/map/session, both with content. **Banked, ungraded** |
| **I** | PASS | delivery ON, `544×540` on both sides, `labels.jsonl` absent |

### 11.1 `GATE C` — and why only one of its two legs is evidence

| leg | IHDR (ground truth) | `video.resolution` | `run.json viewport` |
|---|---|---|---|
| native | 875×869 | 875×869 | 875×869 |
| downscale `oh=360` | 362×360 | **362×360** | **875×869** |

🚨 **The native leg proves nothing — the two fields would have agreed before `m28` too, for the old
and wrong reason.** The downscale leg is the whole gate: `video.resolution` follows the pixels while
`viewport` stays with the window. **Pre-declared as `P-B`, and it held.**

### 11.2 `GATE E` — the four readings, verbatim from the echo

```
height=0    from COMPILED DEFAULT (0 = native); no ini key present, no override set, no per-run argument
height=540  from DefaultGame.ini [AnomalyCapture] CaptureOutputHeightDefault
height=720  from IAI.Capture.OutputHeight (between-runs override)      <- ini still 540
height=360  from PER-RUN ARGUMENT (dashboard outputHeight / console Start oh=)  <- ini 540 AND override 720 still set
```

📌 **`GATE F`'s zero leg is the sentinel proof:** `oh=0` reports height **0** from **PER-RUN
ARGUMENT**, against the compiled-default leg's height **0** from **COMPILED DEFAULT**. *"Nobody
asked"* and *"deliberately asked for native"* are separable. **`m27`'s FINDING 3 disjunction does
not recur.**

### 11.3 `GATE D`'s sample row — the design in one line

```
native    875x869  norm 0.16666672521210724,0.53051225208472552,0.31818185127166354,0.64917101019199153
                   px   145.83338456059383,461.01514706162646,132.57573530211178,103.11446079521414
downscale 362x360  norm 0.16666672521210724,0.53051225208472552,0.31818185127166354,0.64917101019199153
                   px    60.33335452678282,190.98441075050118,54.848475633559381,42.717152918615767
```

`bbox_norm` **identical to 17 significant digits**; `bbox_px` scaled by exactly `362/875`. **Nothing
reached the projection path.**

---

## 12. 🚨 THE VACUITY FINDING — `GATE D` PASSED ON NOTHING, AND THE GATE COULD NOT TELL

**This is the most important thing in the session and it is a defect in the PRE-DECLARATION, not in
`m28`.**

The first native/downscale pair returned `GATE D`'s pass condition — **`bbox_norm` rows differing:
0** — and it was meaningless. Both legs contained **zero anomalies**: `positive_frames=0`,
`annotation.anomalies=0`, every burst logging *"fired nothing (zero-match / empty)"*. **Comparing two
empty sets returns equal. THE EMPTIEST POSSIBLE RUN PRODUCED THE GATE'S CLEANEST PASS.**

**CAUSE OF THE EMPTINESS — MEASURED, and it is an environment property, not a defect:**
`IAI.DumpCoverage` reported **69 renderable-visible actors** while `IAI.DumpVisible` reported **0**.
In an 875×869 PIE panel no single actor reaches the default **6 %** screen-coverage threshold, so the
auto-pool had nothing to select. With `IAI.SetMinScreenCoverage 0` the visible set returned to 69 and
the re-run pair carried **19 valid bboxes per leg**.

**WHAT CAUGHT IT WAS NOT PRE-DECLARED.** An added counter-check: *`bbox_px` rows differing MUST be
> 0, else nothing was actually rescaled.* `bbox_px` reading **zero** differences **while
`width`/`height` plainly differed** is what exposed the hole.

⚖ **OWNER RULING `C2` — `GATE D` IS NOT AMENDED RETROACTIVELY.** The amendment rule forbids touching
a prediction once the instrument exists, and by then it did. **The record stands exactly as measured:
`GATE D` passed on its written terms BOTH times, and only the added counter-check separates the real
pass from the empty one. Its written form remains vacuously satisfiable.** The counter-check is
recorded in the gate file as an **ADDITION made during the run**, dated and attributed, deleting
nothing.

⚖ **OWNER RULING `C1` — THE STANDING RULE, now `G146`:**

> **A gate whose pass condition is an EQUALITY needs a companion condition that FAILS ON EMPTY INPUT.
> Otherwise the emptiest possible run is its cleanest pass.**

⚠ **THIRD INSTANCE OF THE ORACLE SHAPE** — `G106` (the A54 oracle existed only in prose), `G142` (two
defects in a verification script, found while reporting a pass), now this. **All three share: the
INSTRUMENT was wrong while the PRODUCT was fine, and every time the wrongness presented as a CLEAN
RESULT rather than an error.** Nothing about a pass invites a second look, which is why each has cost
a session.

---

## 13. 🆕 TWO INSTRUMENTATION SCARES — BOTH MINE, NEITHER A DEFECT

Recorded because each read exactly like a build defect for several minutes, and `G142` says the first
hypothesis on a surprising reading is that the CHECK is wrong.

**SCARE 1 — "three `MEASURED` lines for two legs."** `util_get_output_log` **writes its own output
back into the log** as a `LogPython` line, so a later grep re-matched leg 1's text embedded inside an
earlier dump. **The checker polluted the very artifact it was reading.** Filtering `LogPython` gives a
clean 1:1. *(This is `G142` defect 1's shape — a whole-log grep that cannot tell one run's evidence
from another's.)*

**SCARE 2 — "26 echo lines for 13 runs", an exact doubling.** The pattern `EFFECTIVE FOR THIS RUN`
also matches **`m27`'s MASK echo**, which uses the identical phrase (`Capture(mask): EFFECTIVE FOR
THIS RUN — mask ON…`). With `m28`-specific patterns the counts are exactly **13/13/13/13** and
**3/3/3/3** (runs / echo / measured / summary). ⚠ **A log-line convention reused across milestones
makes every loose grep ambiguous** — anchor on the `Capture(mN)` prefix, never on the shared phrase.

---

## 14. ⚖ CLOSING RULINGS

*(Numbered `C1`–`C6` here, corresponding to the six rulings issued at milestone close.)*

**`C1` VACUITY COMPANION** — new standing rule, written as `G146` **and** as a dated design note in
the gate file, because the moment it is most needed is while someone is drafting the NEXT gate table.

**`C2` `GATE D` NOT AMENDED RETROACTIVELY** — see §12.

**`C3` `dimMismatches` STAYS UNPROVEN AND SHIPS.** It read **0 on all 16 runs**, so the mid-run
size-change warning has never fired. ⛔ **A contrived trigger was explicitly REFUSED: it would prove
the warning compiles, not that it protects anything.** It is a **DIAGNOSTIC, not a correctness
guard** — if it never fires, nothing is wrong. 🚨 **NAMED AS UNPROVEN IN THE TAG so silence is never
later read as evidence.** *(Note this is a deliberate, reasoned exception to `G96`'s "a guard that has
never fired is not a guard" — `G96` applies to guards that gate correctness; this one only reports.)*

**`C4` `G145` WIDENS** — it was never a git gotcha. Rewritten for **any native command invoked from
PowerShell**, with the third instance (a WebSocket JSON payload, nothing to do with git) and the fix
shape: **pass the payload in a FILE, not as an inline argument.**

**`C5` LEG CONDITIONS ARE NOT DEFAULTS** — see §10.1. In the journal and in the tag.

**`C6` NO ALIGNMENT CLAIM IS EXTENDED** — `m25` certifies **1280×720 and 1281×721 only**. Nothing
here certifies 875×869; alignment was not tested at any size. In the tag scope.

---

## 15. OPEN / NOT DONE, DELIBERATELY

* **`dimMismatches` unproven** — `C3`, shipped as a named limitation.
* **The JPEG pair is BANKED AND UNGRADED.** No comparison, no quality claim, no threshold proposed.
  Grading is a separate instrument that does not exist yet.
* **`GATE D`'s written form remains vacuously satisfiable** — `C2`. Not rewritten.
* ⚠ **Every gate leg ran with `SetMinScreenCoverage` forced to 0**, so **nothing in the gate set
  exercised `m28` under the shipped selection behaviour.** Closed by the owner smoke (§16).
* **`H6` REMAINS DOCUMENTED, NOT FIXED. `P6` DID NOT MOVE. NO RATIO OR THRESHOLD EXISTS ANYWHERE IN
  THE CODE OR THE DOCS.** `m28` touches none of that line of work — by design, and `GATE D` is the
  control that proves it.

---

## 16. THE OWNER SMOKE — PASSED, ON `MainWorld`, UNDER SHIPPED SELECTION

**This closes the gap §15 named.** Every gate leg ran with `SetMinScreenCoverage` forced to 0; this
did not. Banked at `_bench_sessions_bank\M28_OWNER_SMOKE\` (192 files, manifest, log).

**Preconditions, verified before any run:** `MainWorld`; **`IAI.SetMinScreenCoverage` at its 6 %
DEFAULT, cull ON**; `IAI.DumpVisible` = **5** real selectable actors; ini key absent (compiled
default 0); mask ON from the ini; delivery off. Focus gate off — the one leg condition, for
unattended driving.

| leg | requested | measured | resamples/written | events |
|---|---|---|---|---|
| **S1** console, seed 11, 60f | `0` COMPILED DEFAULT | 875×869 → **875×869**, native | **0** / 60 | 1 anomaly, 40 positives, **4 vetoed** |
| **S2** console, seed 11, 60f | `540` PER-RUN | 875×869 → **544×540**, YES | **60** / 60 | 1 anomaly, 40 positives, **4 vetoed** |

🎯 **THE STRONGEST SINGLE RESULT IN THE MILESTONE: the EVENT OUTCOME IS IDENTICAL ACROSS THE PAIR —
same kept anomaly, same 40 positive frames, and the SAME 4 VETOED EVENTS — with the `m26`/`m27` veto
LIVE and firing.** That is the `m28` premise demonstrated in real content: **a write-time downscale
does not reach the veto.** The gate legs argued it from source and from an artificial pair; this shows
it with the cure actually deleting events.

**`GATE D`'s property re-checked on this real pair, with `G146`'s companion applied:**
`rows 60 = 60` · `distinct view origins 1 and 1, identical` · **40 valid bboxes** ·
**`bbox_norm` rows differing = 0** · **`bbox_px` rows differing = 40** · labels `875×869` vs `544×540`.
Artifacts: `IHDR == annotation.video.resolution` on both legs.

### 16.1 THE DASHBOARD CONTROL — VISUALLY VERIFIED, and it was the last unverified surface

Driven by Code in a real browser against the live server (`dist/` served on 5180, real WebSocket
connection, real clicks), **not** simulated:

* The **`size`** select exists **beside `format`**, defaults to **"native (as rendered)"**, offering
  1080p / 720p / 540p / 360p, with the explanatory tooltip.
* Selecting **540p** and clicking **Start capture** produced
  *"requested output height 540, from **PER-RUN ARGUMENT** (dashboard outputHeight…)"* → 544×540,
  `resamples 30/30`.
* Leaving it on **native** produced *"requested output height 0, from **COMPILED DEFAULT** … **no
  per-run argument**"* → **proving the field is OMITTED, not sent as 0**, which is `D8`'s
  fall-through requirement and the `-1`/`0` sentinel working across the wire.

### 16.2 🚨 A REAL REGRESSION THE SMOKE CAUGHT — THE START BUTTON WAS UNREACHABLE

**Found only because the UI was actually driven.** Adding the fourth control to `.cap-row` wrapped it
from two lines to three, and the capture panel — a flex child with the default `flex-shrink: 1` —
**was squashed instead of allowed to grow**: measured `clientHeight 209` against `scrollHeight 290`,
which put **`Start capture` at y=406 while the panel ended at y=353.** The button was in the DOM,
enabled, and **painted outside the visible column.** `read_page` reported it as present; only the
screenshot and a geometry measurement showed it was unusable.

**FIX — one line, and it repairs a LATENT fragility rather than my symptom:**
```css
.col.right > .panel { flex-shrink: 0; }
```
`.col.right` already had `overflow: auto`; it never got to scroll because the panels shrank first.
Verified live before editing source (panel 209→310 = its full content height, button inside the
column box, column scroll engaged), then applied, rebuilt, and re-verified in the browser.

⚠ **THE LESSON, and it is why the owner's "you also run the smoke" instruction mattered:** the wire
path was already proven by `GATE E`'s fourth leg, and a wire-only verification would have shipped a
dashboard **whose Start button could not be clicked**. **`read_page` saying an element exists is not
evidence a human can reach it.** *(I had flagged this exact file in the Stage 1 plan as "styles.css —
cosmetic, may end up empty". It did not end up empty.)*

---

## 17. State at milestone close

| | |
|---|---|
| plugin | `m28` **TAGGED**; all work pushed |
| AnomDash | CSS regression fix pushed |
| tags | `m26` → `d6bee7a`, `m27` → `4a92962`, **NEITHER MOVED**; `m28` new |
| `feature/stencil-capture` | **UNTOUCHED at `76cac74`**, never checked out |
| gates | **A–I ALL PASS**, banked at `_bench_sessions_bank\M28_GATES\` |
| owner smoke | **PASSED** on `MainWorld` under shipped selection, banked at `_bench_sessions_bank\M28_OWNER_SMOKE\` |
| `DefaultGame.ini` | restored to its prior content; **no shipped default changed** |
