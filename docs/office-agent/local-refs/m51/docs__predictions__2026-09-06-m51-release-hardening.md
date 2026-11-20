# m51 - release hardening - PREDICTIONS

> # DRAFT - NOT APPROVED FOR EXECUTION
> Written 2026-09-06 in brief 078-01, BEFORE any implementation and BEFORE any leg.
> **No empirical result is claimed in this file.** Every number below is a PREDICTION.
> It becomes executable only when the adversarial reviewer has responded in writing under
> `_reviews\` and a follow-up brief releases execution. Amendments after that point are
> append-only dated AMENDMENT blocks, never edits (`P-C2` route).

**Source revision at authoring:** `master` = `ac13700`. **Source material:** `fix/capture-label-integrity`
= `be16977` (base `21c1e0d`, now backed up on origin). **Rulings:** R1-R8 as amended,
`_reviews\2026-09-06-review-of-fix-capture-label-integrity.md` sections 3 and 5.

---

## 0. What this file is predicting

m51 imports the useful half of `be16977` onto post-Phase-B `master`, applies rulings R1-R8, and
proves the result against the project gate bar. The predictions below are the **expected readings**.
A reading outside its stated band is a **STOP**, reported red, not reinterpreted in the same turn
(`G253`).

Three standing rules govern every entry:

- **Unknown is never zero.** `-1` / `null` mean NOT MEASURED. No gate may accept a `0` where the
  prediction says `-1`, and no gate may accept `-1` where it says `0`.
- **A can-fail lever must bypass the mechanism under test, not exercise it** (`G252`).
- **A leg certifying with fewer than 3 counted events is INVALID, not failed** - re-run, never grade
  on the reduced set.

---

## 1. Scope boundary - what m51 does NOT change

Declared here so that a reviewer can check the diff against it.

- **Shaders are untouched.** MEASURED: `git diff --stat 21c1e0d..be16977 -- Shaders` is EMPTY, and
  Phase B's shader changes (`AnomalyMaskReduce.usf` +36, `AnomalyVisibleMask.usf` +10) live only on
  `master`. **m51 therefore needs NO COOK** (`G103`/`G129`) and the Phase B container quartet
  `67EA1FE0`/`2CEFB8F4`/`E03C6610` + `A16A18A8`/`C70ECDAA` stays byte-unchanged. If any m51 commit
  makes a shader or shader-parameter-struct change, that is a SCOPE BREAK: stop, price the cook and
  the re-baseline, and obtain an explicit scope release before proceeding.
- **No client-facing meaning changes.** `label_schema` 3 = schema 2 + additive keys only (R1).
  `anomaly_present` keeps "an injection was present". `affected_frames` keeps "observable subset,
  with the zero-measured fallback". Renegotiating either is the M3 contract, not m51.
- **`target_drawn_pixels` and `frames_drawn_unexpected` remain DIAGNOSTICS and never label
  predicates** (`G258`). Any m51 commit that makes `DrawnCount` a label input is a scope break.

---

## 2. Truth table - measurement disposition per (frame, fire)

This is the contract every consumer must agree on. `TP` = `target_pixels`, `TDP` =
`target_drawn_pixels`, `OBS` = `observable`, `MS` = `mask_state`, `RS` = `render_state`.

| # | condition | TP | TDP | OBS | MS | RS | counts as | commits? |
|---|---|---|---|---|---|---|---|---|
| A | mask not requested (knob off) | -1 | -1 | null | unmeasured | - | Unmeasured | yes |
| B | mask requested, NOT effective (F1: `OutputHeight != 0`, or mask-measure off) | -1 | -1 | null | unmeasured | - | Unmeasured | yes, **immediately - no hold** |
| C | mask armed, pass invalid (F9: `CustomDepthMode != 3`, bad stencil extent, unassigned reserved tag, owner violation) | -1 | -1 | null | unmeasured | - | Unmeasured | yes |
| D | target not measurable (all-Nanite, R2) | -1 | -1 | null | unmeasured | - | Unmeasured | yes |
| E | this FIRE's swap material incomplete (R5) | -1 | -1 | null | unchanged | `shaders_pending` | Unmeasured **for this fire only** | yes |
| F | mask valid, texture-class, condition held, count > 0 | N>0 | - | true | present | - | Observable | yes |
| G | mask valid, texture-class, count == 0 | 0 | - | false | empty | - | Measured, not observable | yes |
| H | mask valid, hide-class, labelled, silhouette in view, `DrawnCount == 0` | N>=0 | 0 | true | present/empty | - | Observable | yes |
| I | mask valid, hide-class, labelled, `DrawnCount > 0` | N | >0 | **true** | present | - | Observable; `frames_drawn_unexpected`++ | yes |
| J | mask PNG encode/write FAILED (R6) | -1 | -1 | null | unmeasured | - | Unmeasured; `masks_dropped`++ | **yes - colour frame still commits, `mask_file: null`** |
| K | image encode/write FAILED | - | - | - | - | - | `writer_drops`++ | **NO** |
| L | label row append FAILED | - | - | - | - | - | `writer_drops`++ | **NO** |
| M | readback cap exceeded at arm (R7) | - | - | - | - | - | `readback_drops`++ | **NO** |
| N | armed tick received no render (R8) | - | - | - | - | - | `unrendered_ticks`++ | **NO - snapshot cancelled immediately** |

**Row I is the Phase B ruling and is load-bearing.** `DrawnCount > 0` is NOT evidence of presence
(`G258`); it must not veto `observable`. A build in which row I reads `OBS: false` has reintroduced
the defect Phase B refuted on pixels, and it deletes true positive labels.

**Rows J vs K/L is the distinction the brief demands.** A mask artifact failure downgrades the
measurement but keeps the picture; an image or label failure means there is no frame.

### 2.1 Session completeness

`capture_complete` is **false** if and only if at least one of
`writer_drops`, `readback_drops`, `unrendered_ticks`, `masks_dropped` is non-zero, i.e. an artifact
that was supposed to exist does not.

`capture_complete` stays **true** for rows A-I: an intentionally unavailable measurement is not an
artifact failure. **This is the line the brief asks to be declared explicitly** - a Nanite-heavy
host, a mask-off run and a downscaled run all produce complete sessions full of honest `-1`s.

> **FORK F-a (see journal section 7):** whether `masks_dropped > 0` alone should set
> `capture_complete:false`. Proposed YES (a promised artifact failed). The cost is that the R6
> mask-failure lever leg then cannot pass `--integrity-only`, which is correct for a failure leg but
> must be declared per leg in section 6.

---

## 3. Truth table - event derivation (R1)

Counted over the event's INJECTED frames. "Measured" = the frame produced a disposition in
{F,G,H,I} for that fire; rows A-E are Unmeasured.

| measured | observable | `affected_frames` | `observability_measured` | `observability_complete` | in file? |
|---|---|---|---|---|---|
| 0 | 0 | **= `injected_frames`** (FALLBACK) | false | false | yes |
| >0, < injected | k > 0 | the observable subset | true | false | yes |
| >0, < injected | 0 | `[]`, `start_frame`/`end_frame` = **-1** | true | false | yes |
| == injected | k > 0 | the observable subset | true | true | yes |
| == injected | 0 | `[]`, start/end = **-1** | true | true | yes |

- The fallback fires **only at ZERO measured frames** (R1 as amended). A partially measured event
  keeps the observable subset and never falls back.
- `observability_measured` keeps Phase B's meaning (`Measured > 0`) - **unchanged**, so
  "nothing you read changes meaning" holds. `observability_complete` is the NEW key.
- Zero-observable events STAY in the file (m49 ruling, unchanged).
- **`start/end = -1` on an empty window is a VALUE CHANGE in an existing key**, not a new key.
  Declared here rather than hidden behind "additive": a v2 consumer reading `start_frame` on an
  empty window previously saw `0`. It is called out in the v2->v3 changelog in the client's own
  words.

### 3.1 The m26 event veto - the unreconciled overlap

MEASURED at `AnomalyCaptureSubsystem.cpp:4948-5000` on master: the m26 veto deletes an entire event
whose separate event-level `FAnomalyMaskRecord` reads `MEASURED_ZERO`, **without consulting the
per-frame record at all**. m49 simultaneously rules that zero-observable events STAY in the file.
Both rules are live in the shipped binary and they contradict each other.

**Proposed resolution (FORK F-b, journal section 7): re-point the veto's INPUT, do not delete the
veto.** Veto iff the event has at least one measured frame AND every measured frame reads
`target_pixels == 0`. Predictions:

- `vetoed_events` stays a LIVE counter (it is a `run_summary` key the client already has) and stays
  can-fail-able; deleting the veto would silently retire it to a permanent 0 (`G96` shape).
- On the four StackOBot hide/texture legs: `vetoed_events` **unchanged from the m49 Phase B
  baseline** (predicted 0 on the gate fixture).
- No event that has per-frame observable evidence is ever deleted. Can-fail: a target measured at
  zero across every measured frame still vetoes.

---

## 4. Lifetime and ownership - arm to terminal outcome

Every accepted request reaches **exactly one** terminal outcome. This table is the contract the F5
and R8 gates test.

| stage | thread | identity carried | may transition to |
|---|---|---|---|
| **Armed** | game (`OnWorldTickEnd`) | `(Generation, SessionIndex, RequestId)`; frozen condition/identity/geometry | Published, Cancelled, Dropped(cap) |
| **Published** | render (`BeginRenderViewFamily`) | + `(World, Viewport, Family.FrameNumber)` | Rendered, Cancelled |
| **Rendered** | render (`AfterPass`) | sub-rect copy enqueued for readback | ReadbackPending |
| **ReadbackPending** | render -> game | `RequestId` | Measured, Cancelled |
| **Measured** | game (`ServiceTargetMask`) | joined by `(Generation, RequestId)`; mask outcome or explicit unmeasured | Encoding |
| **Encoding** | worker pool (N=2 default) | receipt handle | Committed, Failed |
| **Committed** | game (receipt) | final disposition incl. downgrades | terminal |
| **Cancelled** | game | reason: unrendered / stale generation / session end | terminal |
| **Dropped** | game | reason: readback cap | terminal |
| **Failed** | game (receipt) | reason: image / label / mask write | terminal |

Rules the implementation must satisfy, each with its gate in section 6:

1. **Capacity is released on every terminal outcome, exactly once.** A cancellation retires the
   `PendingSnapshots` entry and its mask/view bookkeeping **immediately**, not at finalize (R8, the
   defect Codex added). An obsolete completion arriving later must neither resurrect the entry nor
   decrement capacity a second time - it is discarded by `Generation`.
2. **`Generation` is checked at every boundary**, so a result from session N can never bind to a
   request in session N+1 even when serials repeat (F5).
3. **Backpressure waits at most ONE configured capture-frame period**, and that wait must not hold
   any lock a worker needs to make progress. Then the frame is a counted drop (R6). Never a 10 s
   game-thread stall.
4. **Finalization joins all accepted work before deriving anything.** Annotation and `run_summary`
   are derived from terminal committed receipts only.
5. **Ordering is not required.** `labels.jsonl` rows may be appended in completion order; the
   client contract already joins by `session_index` and forbids sorting on `frame_index`
   (`G162`, client README). The serialized section is the append + receipt publication, which must
   not serialize the encodes themselves and must not block on an index that was cancelled.

### 4.1 Writer sizing - declared before measurement

- `N` workers, **default 2**, ini `[AnomalyCapture] WriterWorkerCount`.
- Byte budget default **256 MiB**, ini `WriterQueueBytes`, and it **includes in-service jobs**, not
  only queued ones - a budget that ignores the job a worker is holding under-counts by up to N
  frames.
- Accounting covers raw BGRA + mask buffer + encoded output.
- Cost at the delivery config, 1920x1080: raw BGRA = 1920*1080*4 = **7.91 MiB/frame**; mask (8-bit)
  = **1.98 MiB/frame**; PNG output bounded by raw. Worst case per in-flight job ~**17.8 MiB**, so
  256 MiB admits ~14 frames in flight. At a high-BPP layout (`PF_A2B10G10R10` / `PF_FloatRGBA`
  intermediate, 8 B/px) raw = **15.8 MiB** and the same budget admits ~7.
- **These defaults are NOT throughput-proven.** They are declared here so the throughput gate
  (section 6, gate T) can refute them. A gate that merely passes at N=2 does not establish that 2 is
  right; the can-fail arm is what makes the number a reading.
- Readback cap default **32**, ini `ReadbackPendingCap` (R7).

---

## 5. Integration plan and conflict map

### 5.1 Measured conflict surface

`git diff --stat` at authoring time. Phase B (`21c1e0d..master`) and the branch
(`21c1e0d..be16977`) overlap on exactly six files:

| file | Phase B | branch | conflict |
|---|---|---|---|
| `AnomalyCaptureSubsystem.cpp` | +267 | +502/-... | **HEAVY** - drain loop, finalize, sampling |
| `AnomalyLabelWriter.cpp` | +56 | +97 | moderate - row emission, decode |
| `AnomalyLabelWriter.h` | +10 | +28 | moderate - snapshot struct |
| `AnomalyMaskSceneViewExtension.cpp` | +62 | +73 | moderate - arm/publish/consume |
| `AnomalyMaskTypes.h` | +3 | +1 | small |
| `Public/AnomalyCaptureSubsystem.h` | +18 | +9 | small |
| `Shaders/*.usf` | +46 | **0** | **NONE - measured** |
| `Public/AnomalyMaskReduceShader.h` | +1 | **0** | **NONE - measured** |

**Consequence, stated because it is the trap:** `git diff master..be16977` shows Phase B's own
additions as DELETIONS (gotchas -159, predictions -412, journal -559, plus the shader and
`DrawnCount` work), because the branch predates Phase B. **Applying that diff, or merging the branch
as a unit, DELETES Phase B.** m51 must port hunk by hunk against `master`, never apply a whole-file
or whole-diff replacement of the six overlapping files.

### 5.2 Commit sequence

Each ruling is a separately named commit so the reviewer can read them independently. Imported work
that carries reversed semantics is **never committed in its reversed form**: the ruling is applied
in the same commit that imports it.

| # | commit | contents |
|---|---|---|
| 1 | `feat(capture): m51 F1 - resolve intentionally-unavailable masks immediately` | per-snapshot await flag from `bTargetMaskEffective`, per-snapshot wait counter; retire the subsystem-wide `TargetMaskHoldTicks` in the same commit so it is never left vestigial |
| 2 | `feat(capture): m51 F2 - freeze geometry, identity and provenance at the frame boundary` | `FTargetGeometry` by value; label writer and event accumulation stop dereferencing live actors; destroyed target keeps its row |
| 3 | `feat(capture): m51 F3+R2 - release event stencil tags when the fire ends` | history separated from live lease; `NaniteOnly == Renderable` PRESERVED (no flip) |
| 4 | `feat(capture): m51 F4+F5+R8 - bind render submissions to one view and one session generation` | world/viewport/single-view/no-stereo filters; `Generation`; **immediate cancellation retiring capacity**; `unrendered_ticks` |
| 5 | `feat(capture): m51 F9+R5 - one shared validity assessment, shader-pending per fire` | shared validity predicate; per-fire attribution |
| 6 | `feat(capture): m51 F6+R1 - derive events from committed per-frame evidence, schema 3` | additive keys; fallback at zero measured only; `observability_complete`; veto re-pointed per FORK F-b |
| 7 | `feat(capture): m51 F7+F8+R6+R7 - committed-write receipts and bounded writer` | receipts, per-session writer, bounded pool, byte cap, counted drops, `ensure` not `check` |
| 8 | `feat(capture): m51 F10+F14 - global membership and sync parity at one sampling boundary` | globals sampled with membership; sync path uses `IsLogicallyHidden` and includes globals; **the two session-global counters stay incremented** |
| 9 | `feat(capture): m51 F11 - project from the rendered view's own matrices` | actual projection/rect; near-plane clipping |
| 10 | `feat(injector): m51 F12+F13+F16 - strong material ownership, preview completion, format gate` | `TStrongObjectPtr`; every preview request terminates; unsupported layouts refused, never black |
| 11 | `feat(control): m51 F15+R3+R4 - targeting paths and mutation accounting` | add path matching + ambiguity log (name-OR-class KEPT); WebSocket refusal kept; console allowed and counted as `external_mutations`, distinguished from auto-injector operations |
| 12 | `fix(capture): m51 - Published batch eviction, not silent clear` | FIFO eviction with a counter, mirroring the key ring |
| 13 | `feat(tools): m51 - integrity auditor on the amended schema` | `verify_integrity.py` rewritten to R1/R5; per-leg expected exits |
| 14 | `docs(m51): ...` | client README section 8, changelog, checklist, gotchas, journal |

---

## 6. Gate matrix

Every leg: **both tick orders** (natural, `IAI.Bench.SynthTickOrder`), **both fixtures** (StackOBot
bench, Lyra worktree) unless marked otherwise, on **ONE binary per fixture** whose hash is recorded,
A-side archived byte-identical, **>= 3 counted events** to certify.

**Binary provenance (brief item 10).** StackOBot and Lyra cannot share a host executable - they are
different games. The one-binary discipline is therefore **per fixture**: one StackOBot exe +
container quartet, and one LyraEditor build, both produced from the **same plugin source revision**,
both hashes recorded, and every leg of that fixture's campaign runs on its own frozen binary. No
after-the-fact substitution; every discard banked and classified under `A63`/`A47`.

### 6.1 New legs, one per finding

| gate | finding | fixture / config | prediction | can-fail lever | can-fail signature | stop condition |
|---|---|---|---|---|---|---|
| **M-F1** | F1 | StackOBot, masks requested + `IAI.Capture.OutputHeight 360` | **90 requested = 90 written**, `TARGET MASK NEVER RESOLVED` count **0**, every row `mask_state:"unmeasured"`, `target_pixels -1`, `capture_complete:true` | revert commit 1 (A-side binary) | A-side drops most of the batch and logs NEVER RESOLVED repeatedly | any written < requested, or any `target_pixels: 0` |
| **M-F2a** | F2 | StackOBot, moving-target lever, camera static | `bbox_px` **invariant under readback latency** (see 6.2) | A-side binary | A-side `bbox_px` moves with latency | any bbox varies with latency on B-side |
| **M-F2b** | F2 | StackOBot, target destroyed mid-window | the frame's row is **KEPT** with its frozen bbox; the index stays in `injected_frames` | A-side binary | A-side loses the index from `affected_frames` | row missing on B-side |
| **M-F3** | F3 | StackOBot, > 55 sequential measurable events in one session | **all events measured**, `TAG-POOL EXHAUSTED` **0**, `unmeasurable_targets_admitted` **0** from exhaustion | `IAI.Bench.TagPoolLimit 8` (existing) | exhaustion fires and events ship NOT_MEASURED | any exhaustion on the natural leg |
| **M-F4** | F4 | StackOBot + Lyra | mask and colour `RenderFrame` **equal on every row** | second eligible view (see 6.3) | request consumed by the foreign view | any row where they differ |
| **M-F5** | F5 | StackOBot, stop then start with in-flight readbacks | **zero cross-session results**; session 2 rows all carry generation 2 | force in-flight at stop | A-side binds an old result to a new request | any generation mismatch accepted |
| **M-F6** | F6/R1 | StackOBot + Lyra | section 3 truth table holds row for row; `vetoed_events` matches the Phase B baseline | zero-drawn target | veto fires on a measured-zero event | an event with observable evidence deleted |
| **M-F7** | F7/F8 | StackOBot, write-failure lever (read-only dir / locked file) | `capture_complete:false`, `writer_drops > 0`, **no dangling `mask_file`**, no annotation row without a frame | the lever IS the can-fail | without the lever, all zero | a failed write reported as success |
| **M-F9** | F9 | StackOBot, omit-depth lever (`IAI.Bench.HideOmitDepthPassSilencing`) | rows read **`target_pixels -1`**, never `0` | the lever IS the can-fail | without it, measured counts | any `0` where the pass did not run |
| **M-F10** | F10 | StackOBot, `camera_clipping` entering mid-capture, BOTH orders | entering transition present in both orders; both session-global counters **non-zero** | synth order | membership differs between orders | counters read 0 (they are live on master - a 0 means m51 orphaned them) |
| **M-F11** | F11 | StackOBot, constrained-aspect fixture (the 067 `bDefaultConstrainAspectRatio` device) | projected box uses the rendered rect; near-plane case clipped, not discarded | unconstrained control | boxes differ between the two | inverted or out-of-rect boxes |
| **M-F12** | F12 | StackOBot, `obj gc` during a texture anomaly | original material restored **by pointer identity**, not by mesh default | A-side binary | A-side falls back to default | identity not restored |
| **M-F13** | F13 | StackOBot, forced preview drop then resize | preview recovers; `Outstanding` returns to 0 | forced drop IS the lever | A-side freezes permanently | preview does not recover |
| **M-F14** | F14 | StackOBot, SYNC path (`IAI.Capture.Async 0`), hide-class | sync rows agree with async rows on `anomaly_present` and hidden set; session globals present | async control leg | pre-fix sync reports non-manifested for every hide | any disagreement |
| **M-F15** | F15/R4 | StackOBot | console mutation during capture **succeeds** and `external_mutations` increments with its session index; WebSocket mutation **refused** with `capture_active`; auto-injector operations **do not** increment | issue both kinds | a no-op cannot masquerade as success - attempts and outcomes both logged | console blocked (breaks bench levers), or auditor failing on `external_mutations` |
| **M-F16** | F16 | StackOBot, unsupported layout forced | **refusal**, counted; **no black frame written** | forced layout IS the lever | supported layout writes normally | a black frame counted as success |
| **T** | throughput | **1920x1080 native PNG**, mask + census ON, both orders, both fixtures | zero `writer_drops`, zero `readback_drops`, zero missing labels; `speed_ratio` within the existing ship band; queue depth and max wait recorded | saturation lever (tiny byte budget) | drops appear and are COUNTED, session marked incomplete | any natural drop, or an uncounted drop |
| **A** | auditor | every banked schema-3 session | `--integrity-only` **PASS** on natural legs; the expected non-zero exit on each declared failure leg | corrupt a copy | auditor fails the corruption | a failure leg passing, or a natural leg failing |

**Throughput leg duration.** The existing bar is a full standard leg (90 armed frames, `A47`(vii)),
which is what every certifying leg in this project uses; the 640x360 smokes behind `be16977` are not
a precedent for anything. m51 additionally runs **one extended leg per fixture at 1080p sized to
exceed the writer byte budget by at least 3x in flight** - with the section 4.1 arithmetic that is
> 100 armed frames - because a 90-frame leg at 256 MiB may never reach steady-state pressure and
would report a vacuous zero (`G146`).

### 6.2 M-F2a - why "frozen bbox == drawn bbox" is REJECTED as the test

**A projected actor AABB does not equal a visible silhouette bbox, and the project has measured
that.** `m30` recorded the MainWorld rock at **11.83 % bounds coverage while DRAWING 2.78 %** - a
~4x over-read (`G149` amendment). The AABB of a sphere projects to a square; its silhouette is the
inscribed circle. Requiring `bbox_px == bbox_drawn_px` would therefore fail on a **correct** static
build, and tuning a tolerance until it passed would be fitting the gate to the answer.

**The test used instead needs no pixel reference at all.** F2 is a *determinism* claim: after the
fix, `bbox_px` for session index N is a function of the frame-boundary state ONLY, so it must be
**invariant to how long readback takes**. Run the same seed/target/config at two readback latencies
and compare `bbox_px` row for row:

- **B-side (fixed): identical on every row**, both latencies.
- **A-side (can-fail): rows diverge, and the divergence grows with target speed** - that is the
  defect, measured as a differential rather than against a synthetic truth.

Latency is varied with the existing `IAI.Capture.ViewLag` knob and/or the mask hold path; if neither
gives a clean two-point separation on the bench, **a latency lever must be built** and is listed in
section 6.5 as a tool gap. A secondary, weaker reading (reported, not gating): on a purpose-built
unoccluded convex target the **centroid** of `bbox_px` should track the centroid of `bbox_drawn_px`
within a band **measured from a STATIC control leg on the same fixture** - centroids are far more
robust than extents to the AABB/silhouette mismatch, and the static leg is what justifies the band.

### 6.3 M-F4 - the second-view lever

`IsActiveThisFrame_Internal` (`AnomalySceneViewExtension.cpp:43-47`) currently returns
`Cap->IsActive()` with **no world and no viewport test**, so any eligible family in any world
consumes a pending request. The lever must produce a second eligible family concurrently with
capture. Candidates, in order of preference: a second PIE world; a scene-capture component that is
NOT flagged `bIsSceneCapture` (the existing exclusion at `:57-58` would otherwise mask it); a second
editor viewport in the editor-target leg. **If none can be produced on either fixture, the gate is
reported UNRUNNABLE, not passed** - and the world/viewport filter is then evidenced only by the
static reading that the filter exists and rejects a synthesised foreign context in a unit test,
which is explicitly weaker and must be labelled so.

### 6.4 R8 - the decoupled-order gate, and the arm that does NOT exist here

**MEASURED, and it changes the gate design:** `tickpin_compiled = false` on **both** fixtures -
StackOBot banked leg `PB1G_BL_NAT` and Lyra `LYRA_SMOKE_01` both report it, because both run stock
UE 5.1 and the tick pin compiles only against a decoupled-tick fork
(`#if ANOMINJECT_FW_TICKPIN`). Therefore:

- **The "tick pin applied" arm of R8's gate is STRUCTURALLY UNRUNNABLE on both available
  fixtures.** It is declared unrunnable and reported as such, never as passed, and never replaced by
  a unit test presented as the gate.
- **`SynthTickOrder` alone does NOT test decoupling.** It relocates the injector's dispatch within a
  tick; it does not produce a tick that gets no render. Claiming otherwise would be the `G96` shape.
- **`ticks_per_captured_frame > 1` is NOT evidence of missed renders.** Banked legs read 108 ticks /
  90 frames (StackOBot) and 122 / 90 (Lyra); that is lead-in, settle and post-gap, all of which
  correctly capture nothing.

**The runnable arm, and it exercises exactly the defect:** `IAI.Capture.SVE.ForceMiss N` corrupts
key-ring publishes so the render-thread lookup misses. An armed frame then receives no completion -
**the same bookkeeping shape as a tick with no render**: the `PendingSnapshots` entry is left
behind. Gate M-R8:

- Force misses for a burst **longer than the readback cap** (32), then turn the lever off.
- **B-side prediction:** each missed arm is cancelled immediately, `unrendered_ticks` counts them,
  capacity is released, and **capture resumes and completes** once the lever is off; the session is
  marked `capture_complete:false` with `unrendered_ticks` equal to the forced count.
- **A-side (can-fail):** the cap fills and capture does **not** recover after the lever is off.
- Report `unrendered_ticks`, `writer_drops` and `readback_drops` **separately** - they are three
  different failures and collapsing them hides which one fired.
- **Named limitation carried into the client README:** the difference between "a real decoupled host
  loses arms" and "our lever synthesises the same bookkeeping" is a difference in CAUSE. The lever
  proves the recovery path; it does not prove behaviour on a decoupled host, which no fixture here
  can supply.

### 6.5 Tools and levers that do NOT yet exist

Named so that nobody converts an unavailable gate into a pass.

| needed for | tool / lever | exists? |
|---|---|---|
| M-F1 | `OutputHeight` + mask-requested leg recipe | knobs exist; leg script new |
| M-F2a | readback-latency A/B lever | `IAI.Capture.ViewLag` exists; **two-point separation unproven** |
| M-F2b | moving target + destroy-during-window lever | **NEW** (`m45`/`OBS-2` teleport lever is the nearest precedent, and `OBS-2` measured that a STATIC-mobility target silently refuses to move - `G245`; the lever must READ BACK the achieved state) |
| M-F3 | > 55 sequential measurable events in one session | **NEW leg recipe** (existing legs produce ~8) |
| M-F4 | second eligible view/world | **NEW - may be unobtainable, see 6.3** |
| M-F5 | stop/start with forced in-flight readbacks | **NEW** |
| M-F7 | write-failure lever | **NEW** (read-only dir or held file handle) |
| M-F13 | forced preview drop | **NEW** |
| M-F16 | forced unsupported pixel layout | **NEW** |
| T | writer saturation lever (tiny byte budget) | **NEW** (ini knob makes it cheap) |
| M-R8 | `IAI.Capture.SVE.ForceMiss` | **EXISTS** |
| auditor | `verify_integrity.py` on the amended schema | branch version exists but implements the **superseded** schema - see section 7 |

### 6.6 Regression set - unchanged bar

Run in full, both orders, both fixtures: **ONSET**; **PAIRING** 100 % and 50 % screen percentage;
**G7**; **MASK-TIE**; **TARGET-PIXELS TIE**; **G-EDGE x8**; **OBS-1..4**; **m50 G1/G2/G6**;
**PB-1..PB-8** including **PB-5 in BOTH directions** (AE-ON still firing, texture class suppressed -
Phase B's target-excluded exposure detector must remain EFFECTIVE, not merely present);
**black-frame**; **A44** both encodings; **P-C7 v3**; the **Lyra LG set**; verifier report-only.

**P-C7 v3 (brief item 9).** Declared per leg, not globally:

- `run_summary` **68 -> 68 + |additive m51 keys|**, enumerated exactly:
  `capture_complete`, `requested_frames`, `writer_drops`, `readback_drops`, `unrendered_ticks`,
  `masks_dropped`, `external_mutations`, `unmeasurable_targets_admitted` (existing), plus the
  section 3 event keys in `annotation.json`.
- **Absolute counters keep the single-constant-delta rule; `mask_value` keeps the bijection rule**
  (`P-C7 v2`, and the 077R-02 correction that comparing them for EQUALITY is forbidden).
- `frame_indices` **identical value for value** on every pair **except** where a ruling changes
  membership - and each such case is declared **per leg in advance**, with its finding named
  (F2's destroyed/offscreen target and F6's veto re-pointing are the two candidates). A membership
  change that was not declared in advance reads as a regression and is a STOP.

### 6.7 Expected auditor exits per leg

Stated so the two obligations cannot contradict each other.

| leg class | `--integrity-only` |
|---|---|
| every natural certifying leg, both fixtures, both orders | **PASS (exit 0)** |
| M-F1, M-F3, M-F9, M-F14, M-F15, M-F16 | **PASS** - these produce complete sessions full of honest `-1`s |
| M-F7 (write failure) | **FAIL, non-zero** - `capture_complete:false` is the correct output |
| M-R8 (forced misses) | **FAIL, non-zero** - `unrendered_ticks > 0` |
| M-F5 (stop/start) | session 2 **PASS**; session 1 may legitimately fail |
| T saturation arm | **FAIL, non-zero** |
| every banked pre-m51 session | **NOT APPLICABLE** - schema < 3, the auditor must say so and exit distinctly, never silently pass |

---

## 7. Declared premise corrections

Recorded here because a prediction built on a false premise is worse than no prediction.

1. **The three "cleanup" items are BRANCH-INTRODUCED, not master defects.** MEASURED:
   `SessionGlobalPositive/NegativeFrames` **are incremented on master** (`AnomalyCaptureSubsystem.cpp:4252,4256,4383,4387`)
   and are incremented **nowhere** on `be16977`; `TargetMaskHoldTicks` is **live** on master
   (`:3804-3813`) and vestigial only on the branch; the silent `Published` hard-reset is
   `be16977`'s `AnomalyMaskSceneViewExtension.cpp:113` (`if (Published.Num() >= 64) { Published.Reset(); }`)
   and **master has no such map at all** - master's key ring does proper FIFO eviction with a
   `Wrapped` counter (`AnomalySveKeyRing.cpp:120-123`). **The m51 work item is therefore "do not
   orphan / do not import", not "fix existing staleness".**
2. **The branch auditor implements the SUPERSEDED schema and would FAIL a correct m51 build.**
   `verify_integrity.py` asserts `anomaly_present == observable` (L151) - reversed under R1 - applies
   `shaders_pending` **per row** rather than per fire (L116-117) - reversed under R5 - and asserts
   `affected_frames == observable set` (L168), which breaks under R1's zero-measured fallback. It
   must be rewritten, not adopted.
3. **R8's tick-pin arm is unrunnable on both fixtures** - section 6.4.
4. **F6 is an unreconciled contradiction inside the shipped binary**, not merely a review finding -
   section 3.1.

## 8. Open forks carried into review

**F-a** - does `masks_dropped > 0` alone set `capture_complete:false`? Proposed YES.

**F-b** - the m26 veto: re-point its input (proposed) or delete it (the branch's choice)?
Proposed **re-point**, because deleting it retires a live client-facing counter to a permanent zero
and re-admits a class of events the client's dataset has never contained.

**F-c** - M-F4's second-view lever may be unobtainable on both fixtures; if so the gate is reported
UNRUNNABLE and F4's evidence is weaker than the others. Accepted or not?

---

*End of the 078-01 DRAFT text. **EVERYTHING ABOVE THIS LINE IS SUPERSEDED BY AMENDMENT 1 WHEREVER
THEY DISAGREE** and is retained only as the reviewed snapshot. No leg may run against any part of
this file until the reviewer has approved AMENDMENT 1 in writing and a follow-up brief releases
execution.*

---
---

# AMENDMENT 1 - 2026-09-06 (brief 078-02) - THE ACTIVE SPECIFICATION

> **STATUS: DRAFT - NOT APPROVED FOR EXECUTION.** Written after two written reviews, both
> **CHANGES REQUIRED**: `_reviews\078-01-codex-preflight-review.md` (eleven objections) and
> `_reviews\078-01-chat-predictions-review.md` (agrees with all eleven, adds four blockers).
> **This amendment is the active specification.** Sections 0-8 above are the reviewed snapshot and
> are superseded wherever they disagree. Still no empirical result is claimed: every source line
> cited was re-read at `ac13700` in this session, and a source reading is not a runtime pass.

**Refs.** `master` = `ac13700`; `m51` = `b532ddd` (parent); source material `be16977` (base
`21c1e0d`, backed up on origin). Reviewed draft SHA-256
`4A676AD2B08F859C3A5F147CD75C42FA29DAA69DF83086F6217C4F656BC2BDED`.

---

## A1.1 Provenance corrections to the 078-01 report and journal

The 078-01 journal and report are history and are **not rewritten**. These four claims are corrected
here; the corrected statement governs.

| # | 078-01 said | correction |
|---|---|---|
| P1 | "All sixteen findings **reproduce**" | **Static confirmation only.** Every finding was re-located at `ac13700` and the cited code paths exist as described. No capture ran. Read every F-row as *confirmed present in source*, never as *observed at runtime*. |
| P2 | F1 "ships about a quarter of its frames"; "reachable with two shipped compiled-default settings" | **Both overstated.** The fraction is arithmetic from the drain structure, **not a measurement**. And the trigger is **not** a shipped default: `client-delivery.md:529` / `client-readme.md:715` only *document* that a non-zero `OutputHeight` refuses the mask, and the host `Config/DefaultGame.ini` `[AnomalyCapture]` block sets **no `OutputHeight` key**, so the compiled default `0` (native) holds. F1 is a **configuration-dependent** P1: severity on firing is unchanged; reachability requires an operator-set non-zero `OutputHeight`, mask-measure off, or async off. |
| P3 | "`F2`'s BRIEFED GATE IS REJECTED AND REPLACED" | **Wrong attribution.** Brief 078-01 §3 item 9 *already* required "an independent same-frame projection reference, or a purpose-built unoccluded fixture with justified tolerance, not a universal false equality". The equality gate came from the earlier `_reviews` chat review §4, not the brief. Nothing in the brief was refuted; the draft agreed with it and mis-credited itself. |
| P4 | "THREE PREMISES REFUTED ... the cleanup items are branch-introduced" | **Overstated as a refutation.** Brief 078-01 §3 item 2 listed the three items inside the *implementation sequence*, without asserting they were master defects. The measurement (which ref each lives on) stands and is useful; the framing "refuted premise" does not. |

Two further corrections required by both reviewers:

- **Git.** A three-way merge uses the merge base and retains non-conflicting changes from both sides;
  it does **not** necessarily delete Phase B. The dangerous operations are *applying the endpoint
  diff* `master..be16977` or *whole-file replacement*, both of which would revert Phase B. The
  authorized strategy is unchanged: a deliberate hunk-by-hunk port onto `master`.
- **Cook.** The branch's empty shader diff supports **no cook for the port**. It is not evidence
  that the whole milestone needs no cook: a new fixture asset or a later shader / shader-parameter
  change would cross the fence. The fence stands and a crossing is a STOP for explicit release.

---

## A1.2 Disposition of every review item

| item | disposition | where |
|---|---|---|
| Codex 1 - observable predicate | ACCEPTED in full | A1.3 |
| Codex 2 / Chat 1 - reject fork F-b, delete the veto | ACCEPTED; F-b resolved DELETE | A1.4 |
| Codex 3 / Chat 3 - ForceMiss != stale arm; real no-render device | ACCEPTED; `WithholdWorldRender` specified, ForceMiss kept separate | A1.8.1, A1.8.2 |
| Codex 4 / Chat 4 - M-F9 wrong lever | ACCEPTED; `r.CustomDepth 0` window specified | A1.8.3 |
| Codex 5 / Chat 5 - F2 needs real delay + independent reference | ACCEPTED; `ReadbackDelayTicks` + `BENCH_GEOM` + offline oracle | A1.8.4, A1.8.5 |
| Codex 6 / Chat 6 - writer accounting + sustained load | ACCEPTED; accounting policy replaced, 600/60 leg | A1.7, A1.10 |
| Codex 7 / Chat 7 - completeness inconsistent; F-a redundant | ACCEPTED; reconciliation replaces the four-counter iff; F-a removed as a fork | A1.5 |
| Codex 8 / Chat 8 - five writer boundaries | ACCEPTED | A1.9 M-W1..M-W5 |
| Codex 9 / Chat 9 - lifecycle, GT/RT, per-request terminal, leases | ACCEPTED | A1.6 |
| Codex 10 / Chat 10 - matrix, fixtures, commands, P-C7, provenance | ACCEPTED | A1.9, A1.11, A1.13 |
| Codex 11 / Chat 11 - provenance claims | ACCEPTED | A1.1 |
| Chat blocker 1 - M-F4 editor-viewport route | ACCEPTED **with a correction**: a packaged exe cannot host PIE | A1.9 M-F4 |
| Chat blocker 2 - Phase B exposure asserted numerically | ACCEPTED **with a correction**: the row-field demand is unsatisfiable | A1.8.6, A1.9 M-PB5 |
| Chat blocker 3 - row B `bSveCapture`; row N ForceMiss separate | ACCEPTED, both | A1.3 rows B, N, N2 |
| Chat blocker 4 - auditor exit 3 NOT APPLICABLE | ACCEPTED | A1.12 |
| Chat §2 - `start/end = -1` declared value change | ACCEPTED, own changelog line | A1.4 |
| Chat §2 - `vetoed_events` 0 + `zero_observable_events` | ACCEPTED | A1.4 |
| Chat §2 - F1 severity verified against delivery ini | ACCEPTED, **claim corrected** | A1.1 P2 |
| fork F-a | **CLOSED - already YES in the brief.** Not a fork. | A1.5 |
| fork F-b | **CLOSED - DELETE the veto.** | A1.4 |
| fork F-c | **NOT a waiver.** M-F4 runs the editor route; only a measured failure to produce an eligible family makes it OPEN | A1.9 M-F4 |

---

## A1.3 Truth table - per (frame, fire) - CORRECTED

**The predicate is master's own, re-read this session at `AnomalyCaptureSubsystem.cpp:3872-3880`:**

```
Px = TargetPixels[i]                                  (:3872-3873)
if (Px == GTargetPixelsUnmeasured) -> Unmeasured; continue   (:3874-3878)
bObservable = bLabelled && bHeld && Px >= ObservableMinPixels (:3880)
```

`DrawnCount` appears only at `:3887` and increments `FramesDrawnUnexpected`; **it never touches
`Snap->Observable[i]`.** The same predicate governs both hide and texture classes - there is no
class branch in it. `ObservableMinPixels` is a knob (compiled default 1); the threshold is **never**
replaced by a hardcoded `> 0`.

| # | condition | TP | TDP | OBS | MS | RS | commits? |
|---|---|---|---|---|---|---|---|
| A | mask not requested (knob off) | -1 | -1 | null | unmeasured | - | yes |
| B | mask requested, **not effective** | -1 | -1 | null | unmeasured | - | yes, **immediately, no hold** |
| C | mask armed, pass invalid (F9 class) | -1 | -1 | null | unmeasured | - | yes |
| D | target not measurable (all-Nanite, R2) | -1 | -1 | null | unmeasured | - | yes |
| E | **this fire's** swap material incomplete (R5) | -1 | -1 | null | unchanged | `shaders_pending` | yes |
| F | measured, `labelled && held && TP >= threshold` | >=thr | any | **true** | present | - | yes |
| G | measured, `TP == 0` | 0 | any | **false** | empty | - | yes |
| H | measured, `0 < TP < threshold` | <thr | any | **false** | present | - | yes |
| I | measured, **condition lost** (`!held`) | any | any | **false** | present/empty | - | yes; `frames_condition_lost`++ |
| J | measured, **not labelled** this frame | any | any | **false** | present/empty | - | yes |
| K | hide-class, labelled, `DrawnCount > 0` | per F-J | >0 | **per F-J, unchanged** | - | - | yes; `frames_drawn_unexpected`++ |
| L | mask PNG encode/write FAILED (R6) | -1 | -1 | null | unmeasured | - | **yes** - colour frame commits, `mask_file:null`, `masks_dropped`++, **row AND event observation downgraded** |
| M | image or label-row write FAILED | - | - | - | - | - | **NO**; `writer_drops`++ |
| N | armed tick received **no render** (R8) | - | - | - | - | - | **NO**; `unrendered_ticks`++, snapshot cancelled immediately |
| N2 | family rendered, **key lookup failed** (ForceMiss) | - | - | - | - | - | **NO**; `key_misses`++ - a *rendered-but-unreadable* frame, **a different branch from N** |
| O | readback cap exceeded at arm (R7) | - | - | - | - | - | **NO**; `readback_drops`++ |

**Row B, corrected per Chat blocker 3.** "Not effective" is, on master
(`AnomalyCaptureSubsystem.cpp:2965`): `bTargetMask && bMaskMeasure && bAsyncCapture &&
(EffectiveOutputHeight == 0)`. The port adds `bSveCapture` (branch `:2901`), so the m51 predicate is
`bTargetMask && bMaskMeasure && bAsyncCapture && bSveCapture && (EffectiveOutputHeight == 0)`, and
**the backbuffer path joins the intentionally-unavailable set** with its own reason string (branch
`:2935`). All five conditions are named in the `StartRun` echo.

**Row K is the G258 ruling and is load-bearing.** A build in which `DrawnCount > 0` changes `OBS`
has reintroduced the defect Phase B refuted on pixels and deletes true positive labels.

**Row E is per FIRE (R5).** A pending swap material on fire 1 makes fire 1 unmeasured and leaves a
co-active fire 2 at its own measured value.

**Row aggregates.** `anomaly_present` = **any injection present on the frame** (v2 meaning,
unchanged). `visible_positive` = **any fire observable-true**. A frame with fire 1 unknown and fire 2
true is `anomaly_present:true`, `visible_positive:true`, with per-fire evidence differing.

**The shared mask artifact stays distinct from per-fire evidence.** One `mask_file` serves the
frame; a per-fire `-1` does not retract it and does not change `mask_state` for the other fires.
Worked example, to be reproduced verbatim by M-R5: frame with fire 1 = `missing_texture` whose swap
material is incomplete, fire 2 = a valid `blinking` hide. Expect `mask_state:"present"`,
`mask_file` non-null, fire 1 `target_pixels:-1` / `observable:null` / `render_state:"shaders_pending"`,
fire 2 `target_pixels:N` / `observable:true`, frame `visible_positive:true`.

**Globals and sync.** Session-global anomalies (`camera_clipping`) have no target actor and no mask:
rows read `target_pixels:-1`, `observable:null`, `bbox_source:"projected"`, whole-frame extent. After
F14 the sync path emits the same fire set (globals included) and uses `IsLogicallyHidden`, so its
rows follow the same table; the sync path is `bAsyncCapture == false` and therefore always row B.

---

## A1.4 Event derivation (R1) - CORRECTED, veto DELETED

Counted over the event's **injected** frames. "Measured" = a disposition in {F,G,H,I,J} for that fire
on a **committed** frame.

| measured | observable | `affected_frames` | `observability_measured` | `observability_complete` | in file? |
|---|---|---|---|---|---|
| 0 | 0 | **= `injected_frames`** (fallback) | false | false | **yes** |
| >0, < injected | k > 0 | observable subset | true | false | **yes** |
| >0, < injected | 0 | `[]`, start/end **-1** | true | false | **yes** |
| == injected | k > 0 | observable subset | true | true | **yes** |
| == injected | 0 | `[]`, start/end **-1** | true | true | **yes** |

**The m26 event veto is DELETED (fork F-b resolved DELETE).** `AnomalyCaptureSubsystem.cpp:4948-4999`
removes a whole event whose separate event-level record reads `MEASURED_ZERO`. Both reviewers ruled
that re-pointing it still deletes events, which operating contract §3 and the m49 ruling forbid.
Consequences, declared:

- **`vetoed_events` is retained as a compatibility key and reads `0`.** It is **not** repurposed. The
  v2->v3 changelog carries one line: *"`vetoed_events` is retained for compatibility and is no longer
  incremented; the event veto was removed in schema 3. Events that would previously have been deleted
  now appear with an empty `affected_frames` and are counted by `zero_observable_events`."*
- **NEW `zero_observable_events`** = the count of **retained** events that have **at least one
  committed injected-frame measurement** and **no observable injected frame**. A fully unmeasured
  event (fallback row 1) is **excluded** - it is not a measured negative. This is stated because the
  two are easy to conflate and the conflation would manufacture false negatives.
- **`start/end = -1` on an empty window is a declared bug-fix VALUE CHANGE**, not an additive key: a
  v2 consumer previously saw `0`, which is a valid frame index. Its own changelog line.
- **P-C7 impact is declared, not discovered:** on the legs where the m26 veto used to fire, the event
  set **grows** and `vetoed_events` falls to 0. Those legs are named per-leg in A1.11; on every other
  leg the event set must be identical.

---

## A1.5 `capture_complete` - reconciliation, not a counter conjunction

The four-counter "if and only if" of §2.1 is **withdrawn**: a lost callback or a failed final JSON
write would leave all four at zero and publish `capture_complete:true`.

```
capture_complete :=
      committed_receipts.index_set == requested_indices
   && every receipt's REQUIRED artifacts written (image, label row; mask iff armed)
   && writer_drops == 0 && readback_drops == 0 && unrendered_ticks == 0
   && masks_dropped == 0 && key_misses == 0
   && every accepted request reached a terminal outcome
   && labels.jsonl, annotation.json and run_summary.json all written successfully
```

- **`requested_frames` and `written_frame_indices` are both emitted**, so the hole is visible
  independently of any counter. `requested_indices` is the set of indices the capture FSM **armed**;
  it is never rewritten to match what committed. Erasing requested history to make the arithmetic
  agree is forbidden - the hole is the finding.
- **Injection windows are over REQUESTED frames.** `injected_frames` keeps its v2 meaning: the frames
  on which the injection existed, whether or not the frame committed. `affected_frames` (observable)
  is necessarily over **committed** frames, because observability requires a measurement. **These are
  two ledgers and both are preserved.** A dropped frame therefore stays in `injected_frames` and is
  absent from `affected_frames`, which is exactly the pre-existing meaning of the pair; no membership
  semantics change.
- **Publication order (no circular dependency).** `run_summary.json` is written **last**, to a
  temporary file, then renamed. Its `capture_complete` is computed from the reconciliation above,
  which is complete *before* it is serialised, so the field never depends on its own write. If any of
  the three final writes fails, the run emits **no success notification**, logs
  `Capture(m51): SESSION PUBLICATION FAILED`, and leaves whatever succeeded on disk.
- **If `run_summary.json` itself is absent**, the auditor treats its absence as **FAILED**, not as
  "older schema" (A1.12).
- **Orphan policy, declared (Chat 8 asked for a pick).** On a **label-row append failure the image is
  RETAINED**, the receipt is terminal-`Failed` with reason `label_append`, and the index is absent
  from `written_frame_indices`. Deleting the image would add a delete/retry race to a path that is
  already failing. **The auditor MUST detect the orphan** (a file in `Actual_Frames/` with no row and
  no annotation reference) and fail. This is chosen over cleanup precisely because it is auditable.
- **No successful JSONL row without its RGB.** The image write is checked before the row is appended.

**Not incompleteness:** rows A-E. A Nanite-heavy host, a mask-off run, a backbuffer run and a
downscaled run all produce **complete** sessions full of honest `-1`s.

---

## A1.6 Lifecycle, ownership and leases - CORRECTED

| stage | thread | identity | may transition to |
|---|---|---|---|
| **Armed** | **game** (`OnWorldTickEnd`) | `(Generation, SessionIndex, RequestId)` + frozen condition/identity/geometry | Published, Cancelled, Dropped(cap) |
| **Published** | **GAME** (`BeginRenderViewFamily`) | + `(World, Viewport, Family.FrameNumber)` | Rendered, Cancelled |
| **Rendered / readback submitted** | **RENDER** (`AfterPass_RenderThread`) | RequestId | ReadbackPending |
| **ReadbackPending** | render -> game | RequestId | Measured, Cancelled |
| **Measured** | game (`ServiceTargetMask`) | joined by `(Generation, RequestId)` + the snapshot's frozen `MaskValues` | Encoding |
| **Encoding** | worker pool | receipt handle | Committed, Failed |
| **Committed / Cancelled / Dropped / Failed** | game | terminal | - |

**GT/RT corrected (Codex 9).** `BeginRenderViewFamily` is a **game-thread** callback
(`SceneViewExtension.h:136-141`); the engine calls it at `GameViewportClient.cpp:1707` **before**
enqueuing the family. Only `AfterPass_RenderThread` and the readback submission are render-thread.
The draft's table had this wrong.

**Per-request terminal state, in addition to `Generation`.** A generation check cannot reject a late
completion for a request already cancelled **within the same session**. Each request carries a
terminal flag; a completion arriving for a terminal request is discarded, logged, and **releases
capacity zero further times**. Capacity is released **exactly once**, at the transition into any
terminal state.

**Stale unpublished arms - expiry is explicit (brief ruling 3).** Measured this session:
`ConsumeWantedForPublish` (`AnomalySveCapturer.cpp:45+`) pops `PendingWanted[0]` **FIFO with no
expiry**. If nothing renders, nothing is consumed, and `PendingWanted` and `PendingSnapshots` both
grow. **Waiting for the next `BeginRenderViewFamily` is therefore NOT sufficient** - the cap can fill
during the withholding burst, and on resumption each new family would consume a 40-tick-old request
id, **mis-pairing a fresh render with a stale snapshot**. That is worse than a drop.

⇒ **A game-thread sweep runs before admitting the next capture request and before the cap check.** It
retires unpublished arms older than `StaleArmMaxTicks` (knob, default = readback cap = 32 ticks),
counts `unrendered_ticks`, releases their capacity, and marks their snapshots terminal-`Cancelled`.
**Already-published arms are untouched**, so in-flight GPU work is never orphaned. Synchronisation:
the sweep takes the same `StateCS` that `ConsumeWantedForPublish` takes, and removes only entries
whose publish has not begun; a publish that wins the lock first simply consumes normally and the
sweep sees a shorter list. Because both run on the game thread in this engine, the lock contends only
with the capturer's own internal use.

**Tag leases (Codex 9, Chat 9).** `EventClaimed` (`AnomalyMaskMeasure.cpp:93`, cleared only at
`:71`) is released when **both**: the fire has ended, **and** `ArmsIssued == ArmsResolved` for that
record - i.e. every submitted rendering/readback user has reached a terminal outcome. A CPU
cancellation is **not** GPU retirement: a cancelled request whose readback was already submitted
still counts as unresolved until its readback completes or the final `BlockUntilGPUIdle` retires it.
The historical record (value -> event, for `mask_map.json`) is kept for the whole session; only the
**live lease** is released. Each submission freezes its `(tag -> event)` identity in the snapshot, so
a later reuse of the same value cannot re-attribute an in-flight result. Census (`CensusClaimed`) and
host reservations (`HostReserved`) remain excluded from `IsFree` unchanged.

**`Published` eviction.** Master has no such map (its key ring does FIFO eviction with a `Wrapped`
counter, `AnomalySveKeyRing.cpp:120-123`); the branch adds one that clears wholesale
(`AnomalyMaskSceneViewExtension.cpp:113`). The port uses FIFO eviction **and** every evicted batch
issues an explicit `Cancelled` terminal outcome per affected consumer, releasing capacity exactly
once, plus an eviction counter. Silent dropping is forbidden.

**Stereo / multi-view.** Declined explicitly where detectable: the extension refuses to admit
requests when the family reports stereo or more than one view, logging
`Capture(m51): UNSUPPORTED VIEW TOPOLOGY - capture declined`. If topology changes **mid-run**, the
run terminates as `capture_complete:false` with reason `view_topology_changed`; accepted work is
joined and published first. This is a declared disposition, not an unexplained hole.

---

## A1.7 Writer accounting - policy chosen and justified

**Per-pixel facts, corrected** (`Core/Private/Misc/PixelFormat.cpp:44,52`): `PF_A2B10G10R10` = **4
B/px**; `PF_FloatRGBA` = **8 B/px**; `PF_B8G8R8A8` = 4 B/px. At 1920x1080 = 2,073,600 px:

| buffer | 8-bit / packed-10 | half-float RGBA |
|---|---|---|
| source colour | 7.91 MiB | 15.82 MiB |
| mask (8-bit) | 1.98 MiB | 1.98 MiB |
| resampled colour (only when `OutputHeight != 0`) | <= source | <= source |

**PNG output is NOT bounded by the raw size** - incompressible input plus container overhead can
exceed it. The draft's implicit "encoded <= raw" is withdrawn, and a **1.1x cap is rejected**: capping
is a rejection *policy* masquerading as a memory *bound*, and rejecting would create a frame hole for
a compression accident.

**Chosen policy - reservation accounting with measured reserve and honest overrun reporting:**

- Admission charges `reservation = source + mask + resampled + (EncoderReserveFactor * source)`.
- `EncoderReserveFactor` is an ini knob. Its **default is not asserted here**: it is set from the
  measured maximum `encoded/raw` ratio observed on the throughput calibration leg (A1.10), and the
  predictions file is amended with that number before the gate legs run.
- A job whose **actual** encoded output exceeds its reservation is **completed, never truncated**,
  and increments `writer_reserve_overruns`; the byte high-water reading reflects the real peak.
- **Too-large job:** a single job whose reservation exceeds the whole budget is **admitted alone**
  (budget momentarily exceeded) and counted in `writer_oversize_admissions`. Rejecting it would turn
  a configuration property into a frame hole; deadlocking is not an option.
- **Setting validation** at `StartRun`: `WriterWorkerCount` in `[1,16]`, `WriterQueueBytes` >= one
  admissible job at the run's own resolution, `ReadbackPendingCap` in `[1,256]`. Out of range =
  **REFUSED with the value named**, not clamped (the m21/`A48` convention).
- **Deadline:** the backpressure wait is `min(one capture-frame period, WriterMaxWaitMs)`. If the
  capture rate is zero or invalid, the period is undefined, so the wait falls back to
  `WriterMaxWaitMs` (default 40 ms) and the substitution is logged.
- **Lock ownership:** the wait is on a condition variable released before any worker-needed lock is
  taken; the game thread never holds the queue lock while sleeping. Never a 10 s stall.
- **Commit publication is serialised, encodes are not.** N workers encode concurrently; a single
  commit mutex orders receipt publication and the `labels.jsonl` append. Row order is not sorted -
  the client contract joins by `session_index` and forbids sorting on `frame_index` (`G162`).
  Publication for a cancelled index is skipped without blocking.
- **Same-generation terminal check** on every receipt; **no writes and no counter mutation after the
  final completion is published** (M-W5 tests exactly this).
- `check(Pending == 0)` becomes `ensure` + log.

Proposed defaults `N = 2` and `256 MiB` are **proposed, not proven**; gate T (A1.10) is what can
refute them.

---

## A1.8 Devices - what must be built, and what each one actually proves

Each device is a bench-only, console-only, default-OFF lever, loudly echoed, never in a client
payload, and each **reads back its achieved effect** rather than logging its intent (`G245`).

### A1.8.1 `IAI.Bench.WithholdWorldRender <ticks>` - the real no-render device

**Mechanism, verified in the local engine this session:** `GameViewportClient.cpp:1696-1707` gates
`GetRendererModule().BeginRenderingViewFamily(...)` on `!bDisableWorldRendering`;
`bDisableWorldRendering` is a public bitfield at `GameViewportClient.h:98`. Setting it via
`World->GetGameViewport()` suppresses the family entirely. **No engine-source modification is
authorized or assumed** - this is a public engine member the plugin already has access to.

- **Semantics.** `N > 0` sets the flag at the **end of the next capture tick after the command**, and
  clears it at the end of the Nth subsequent capture tick. Start and end tick indices are **logged as
  `session_index` values**, so the burst is bounded in the artifact, not in prose.
- **Capture keeps running:** game ticks, arms and `session_index` advance through the burst. The FSM
  is not paused.
- **Evidence the owned family was absent:** the SVE's own publish counter must not advance during the
  burst (`published` delta == 0 across the logged window), and `ConsumeWantedForPublish` must not be
  entered. That is an independent observation, not an inference from the flag.
- **Restoration:** the prior flag value is saved and restored on normal completion, on `StopRun`, on
  `FinishRun` and in `Deinitialize`. A run that ends mid-burst still restores. Restoration is
  verified by reading the flag back and logging it.
- **N = 40** (> cap 32), then resume.

**B-side (candidate) prediction.** The A1.6 sweep retires unpublished arms older than
`StaleArmMaxTicks` = 32. During a 40-tick burst nothing publishes, so the arms of ticks 1..N-32 are
swept as they age past the threshold and the last <=32 are swept on resumption or expire in turn.
⇒ **`unrendered_ticks == 40` is NOT automatic and is not predicted as a bare constant**: the
prediction is `unrendered_ticks == (number of capture ticks that armed during the burst and never
published)`, which the leg logs directly as its own counted opportunity total, and the two must be
**equal**. Capacity returns to zero pending, capture resumes, **>= 3 counted events are captured
AFTER resumption**, `capture_complete:false`, and a **complete per-index reconciliation** is emitted:
every requested index is either committed or terminal-`Cancelled(unrendered)`, with no index in
neither set.

**A-side (frozen `B8AC65D8`) prediction - CORRECTED.** Chat's "cap fills, no recovery" is **wrong for
this binary**: measured this session, `ac13700` has **no `PendingSnapshots` admission cap at all**
(only the drain-loop bound `Iter < 8` at `:4040` and the finalize sweep at `:4082-4086`); the cap
`if (Async->PendingSnapshots.Num() >= 8)` is **added by the branch** at its `:4072`. A binary cannot
fail by filling a cap it does not have. What `B8AC65D8` will do instead, from
`ConsumeWantedForPublish`'s FIFO-with-no-expiry: `PendingWanted` and `PendingSnapshots` grow through
the burst, and **on resumption each new family consumes the OLDEST pending arm**, pairing a fresh
render with a ~40-tick-old snapshot - **mass mis-pairing**, then a bulk drop at finalize. Expected
A-side signature: `pendingAtDrainEntry` >> 0, a large `"did not resolve by run end (dropped)"` count,
and label rows whose content belongs to earlier ticks. **This is the honest A-side reading and it is
a different failure from the branch's.**

**For the new cap/cancellation regression specifically**, an A-side cannot help: neither the cap nor
the cancellation exists in `B8AC65D8`, and **a console lever absent from an archived executable
cannot be "applied" to it** - an unknown command is not a device. ⇒ **in-candidate falsifier
`IAI.Bench.NoStaleArmSweep 1`** disables the A1.6 sweep on the candidate binary, alongside the
healthy control on the same binary. Expected: sweep ON = recovery as above; sweep OFF = the cap fills
at 32, `readback_drops` rises, and capture does **not** recover after the burst. Distinct signatures,
same binary, both readings from the same leg pair.

**OPEN, not converted:** this proves the **recovery path**. It does not prove behaviour on a real
decoupled host, and it is **not** the applied-tick-pin arm - `tickpin_compiled` reads **false** on
both fixtures (banked `PB1G_BL_NAT`, `LYRA_SMOKE_01`), so that arm stays OPEN and gets a client-README
sentence. `SynthTickOrder` does not substitute: it relocates dispatch within a tick and produces no
missing render.

### A1.8.2 ForceMiss stays, as a different failure

`AnomalySveKeyRing.cpp:16-28`, re-read: ForceMiss **corrupts published keys**; the family renders and
the render-thread lookup fails. `1` = every key, `N>1` = every Nth, with `ForceMissPhase` for phase.
⇒ a **rendered-but-unreadable** frame, counted as **`key_misses`**, never as `unrendered_ticks`.
Because the argument is a periodicity, the leg brackets it with an explicit tick-bounded on/off
interval logged by `session_index`.

### A1.8.3 F9 - `r.CustomDepth 0` window

`HideOmitDepthPassSilencing` is the **wrong** lever: re-read at
`AnomalyInjectorSubsystem.cpp:861-868`, it omits the **depth-pass half** of the m45 hide
(`bRenderInDepthPass` stays true, main pass off) so the target occludes while drawing nothing. The
custom-depth measurement pass still runs and Phase B's non-zero `DrawnCount` there is **correct**
(PB-4). It stays as the Phase B / identity-arbiter diagnostic.

**F9 device:** set `r.CustomDepth 0` for a tick-bounded window, restore afterwards. Because the
plugin sets this cvar itself at `BeginRun` (and could reassert it), the device must:
(a) record the **effective** value **at the pass point** every frame (master already logs
`rCustomDepthModeAtPass`), (b) set with a priority that a plugin write cannot silently outrank, and
(c) **verify by read-back** that the window achieved mode != 3 on the frames it claims - if it did
not, the leg is **VOID by a pre-declared rule**, never a pass.

Controls: **valid-zero** = a fully occluded tagged target, pass valid, `target_pixels: 0`,
`mask_state:"empty"`, `observable:false` (row G). **Invalid window** = `target_pixels: -1`,
`observable: null`, `mask_state:"unmeasured"`, with the recorded reason. Requiring `-1` where a valid
pass would give `0` is the whole point of F9.

### A1.8.4 F2 - `IAI.Bench.ReadbackDelayTicks <N>`

`ViewLag` is the legacy sync **projection-view** lag (`AnomalyCaptureSubsystem.cpp:5927-5930`);
moving the view sample is a confound, not a latency manipulation. The device instead defers the
**game-thread consumption of completed readbacks** by N ticks. It touches nothing under test: the
render, the readback and the frozen snapshot are unchanged; only the drain is later. N must stay
**below the healthy queue and readback caps** so the leg does not accidentally become a saturation
test - with cap 32, `N = 6` is used and the pending high-water is reported to prove headroom.

### A1.8.5 F2 - `BENCH_GEOM` sidecar + independent offline oracle

A latency-invariance check alone is insufficient: a permanently constant or wrong box is also
invariant. The independent reference is a bench-only log line emitted at the frame boundary:

```
BENCH_GEOM si=<n> actor=<path> origin=<x,y,z> extent=<x,y,z> view_origin=<x,y,z>
           view_rot=<p,y,r> fov=<deg> aspect=<f> rect=<x0,y0,x1,y1> proj=<16 floats>
```

- **The oracle is independent**: a new `CaptureBench/tools/m51_bbox_oracle.py` recomputes the
  projection **from the sidecar numbers only**. It must **not** call the implementation's projection
  helper and must **not** read the frozen label snapshot - otherwise it tests the code against
  itself. A constant or wrong box **fails** the oracle; that is verified by feeding it a
  deliberately-perturbed copy before any verdict is read (`G243`/`A53`).
- **The matrices must be the RENDER view's.** `OnWorldTickEnd` alone is insufficient where the render
  view can differ from the sampled view; where they can differ, `proj`/`rect` are captured from the
  rendered `FSceneView` and the sidecar records which source it used.
- **Conventions declared:** rects are half-open integer pixel ranges; NDC->pixel uses
  `floor` for min and `ceil` for max; near-plane-crossing edges are clipped, not dropped. **Tolerance
  = 1 px per edge**, justified as the maximum disagreement two independent implementations of that
  rounding can produce on the same inputs, and verified on a static control leg before the moving leg
  is graded.
- **Cross-run equality is conditional.** Two runs' `bbox_px` are compared **only if** their
  independently recorded `BENCH_GEOM` actor and view series are equal row for row. A shared seed
  establishes no such thing.
- **Cases:** moving target (N=0 vs N=6); **destroyed after render, before drain** (row kept, index in
  `injected_frames` **and** in the event's observations); **orthographic**; **constrained/letterboxed
  rect** (the 067 `bDefaultConstrainAspectRatio` device); **near-plane crossing**.
- **Both fixtures.** Targets are **runtime-spawned** bench actors on StackOBot and Lyra, so no frozen
  asset is modified and neither fixture is quietly waived. If a case is genuinely impossible on one
  fixture, it is marked **OPEN for that fixture**, never waived.

### A1.8.6 Phase B exposure - the row-field demand is corrected

Chat blocker 2 asks for `exposure_dip_scope: "frame_minus_targets"` **on every labelled row of a
zero-dip leg**. That is **unsatisfiable without a schema change**: re-read this session,
`AnomalyLabelWriter.cpp:156-162` emits **both** `exposure_dip` and `exposure_dip_scope` **only inside
`if (bExposureDip)`**, and `AnomalyCaptureSubsystem.cpp:3958-3959` sets the scope only when a dip is
detected. Requiring the field on non-dip rows would change legacy conditional-emission behaviour -
exactly what R1 forbids. **Conditional emission is preserved.**

The behavioural assertion is met instead by bench diagnostics that prove the excluded operand was
**evaluated**, which is what the demand was really about. The detector is an AND of both operands
(`:3956` `if (bDipAll && bDipExcl)`), so:

- per labelled frame, log `LumaAll`, `LumaExcl`, rolling `MeanAll`, `MeanExcl`, `bExclusionUsable`
  and the usable-exclusion pixel count;
- `frames_exposure_dip_suppressed` (already emitted - banked `PB1G_BL_NAT` reads 3) must be **> 0** on
  a leg containing a **known whole-frame drop caused by a hide**, proving `bDipAll && !bDipExcl`
  fired, i.e. the exclusion actively suppressed a false dip;
- AE-ON smoke leg still yields `frames_exposure_dip > 0` in **both** orders;
- pinned legs read `frames_exposure_dip == 0` **with** the diagnostics showing `MeanExcl` computed
  and `bExclusionUsable` true on labelled frames.

**No blanket injection-free-frame baseline guard** may be imported if it turns the detector off on
labelled frames; `exposure_dip_scope` present but ineffective is a FAIL.

### A1.8.7 Other devices

`IAI.Bench.NoTagRetire 1` (retirement-bypass falsifier for M-F3) · `IAI.Bench.WriterFailAt
<image|mask|label|all> <index>` (deterministic write failure) · `IAI.Bench.ForceUnsupportedLayout`
(F16) · `IAI.Bench.PreviewDropNext` (F13) · a saturation arm via `WriterQueueBytes`.

---

## A1.9 Gate matrix

Common: **both tick orders** (natural, `IAI.Bench.SynthTickOrder`); **both fixtures** unless the row
says otherwise; **one frozen binary per fixture**; **>= 3 counted events to certify** (a leg below 3
is INVALID, re-run, never graded on the reduced set); every attempt banked and classified under
`A63`/`A47`. Harness `CaptureBench/tools/run_leg.ps1` (StackOBot) and `lyra_leg.ps1` (Lyra).

| gate | finding | fixture / config | expected reading | falsifier | stop condition | auditor |
|---|---|---|---|---|---|---|
| **M-F1** | F1 | both; masks requested + `IAI.Capture.OutputHeight 360` | requested == written; `NEVER RESOLVED` 0; every row row-B (`-1`/unmeasured); `capture_complete:true` | frozen A-side | any written < requested, or any `target_pixels: 0` | PASS |
| **M-F1b** | F1 | both; `IAI.Capture.SVE 0` (backbuffer) | same as M-F1; reason string names the backbuffer | A-side | any hold, any `0` | PASS |
| **M-F2a** | F2 | both; moving spawned target; `ReadbackDelayTicks` 0 vs 6 | `bbox_px` identical row-for-row **and** equal to the offline oracle within 1 px | A-side | divergence on candidate; oracle mismatch | PASS |
| **M-F2b** | F2 | both; target destroyed after render, before drain | row KEPT; index in `injected_frames` **and** event observations | A-side | row or membership missing | PASS |
| **M-F11** | F11 | both; ortho / constrained rect / near-plane | oracle agreement within 1 px in all three | unconstrained control | inverted or out-of-rect box | PASS |
| **M-F3** | F3 | both; > 55 sequential measurable events | all measured; `TAG-POOL EXHAUSTED` 0; achieved live-lease occupancy reported | `IAI.Bench.NoTagRetire 1` | any exhaustion on the natural leg | PASS |
| **M-F3b** | F3 | both; delayed work + tag reuse (`ReadbackDelayTicks 6` + reuse) | no result re-attributed across a reused value | NoTagRetire | any cross-attribution | PASS |
| **M-F4** | F4 | **editor target**, PIE capture + editor level viewport at Realtime, both fixtures | mask and colour `RenderFrame` equal on every row; foreign-family consumptions counted **0** | in-candidate `IAI.Bench.NoViewBinding 1` | any row differing; any foreign consumption | PASS |
| **M-F5** | F5 | both; stop/start with in-flight readbacks | zero cross-session results; session 2 all generation 2 | A-side | any generation mismatch accepted | s2 PASS |
| **M-F6** | F6/R1 | both | A1.4 table row for row; `vetoed_events == 0`; `zero_observable_events` matches the hand count | zero-drawn target | an event deleted; a fully-unmeasured event counted as zero-observable | PASS |
| **M-R5** | R5 | both; mixed fire (pending texture + valid hide) | the A1.3 worked example verbatim | force-pending both fires | a valid fire made unknown by its neighbour | PASS |
| **M-F9** | F9 | both; `r.CustomDepth 0` window + occluded control | window rows `-1`/null/unmeasured with reason; control row G `0`/empty | window itself | any `0` where the pass did not run; **read-back shows mode 3 in the window ⇒ VOID** | PASS |
| **M-F10** | F10 | both; `camera_clipping` entering mid-capture, both orders | entering transition present in both; both session-global counters **> 0** | synth order | membership differs; counters read 0 (they are live on master, so a 0 means m51 orphaned them) | PASS |
| **M-F12** | F12 | both; `obj gc` during a texture anomaly | original restored **by pointer identity** | A-side | identity not restored | PASS |
| **M-F13** | F13 | both; forced preview drop + resize | preview recovers; `Outstanding` returns to 0 | the drop itself | no recovery | PASS |
| **M-F14** | F14 | both; sync path, hide class | sync rows agree with async on `anomaly_present` and hidden set; globals present | async control | any disagreement | PASS |
| **M-F15** | F15/R4 | both | console mutation succeeds and `external_mutations`++ **with session index**; WebSocket refused `capture_active`; auto-injector ops **do not** increment | issue both kinds | console blocked; auditor failing on `external_mutations`; an attempt logged without its outcome | PASS |
| **M-F16** | F16 | both; forced unsupported layout | refusal counted; **no black frame**; the requested index is a **hole** ⇒ `capture_complete:false` | the lever | a black frame counted as success | **FAIL** |
| **M-R8** | R8 | both; `WithholdWorldRender 40` | A1.8.1 in full; >= 3 events after resumption; full per-index reconciliation | `IAI.Bench.NoStaleArmSweep 1` | no recovery on the swept leg; any index in neither set | **FAIL** |
| **M-R8b** | R8 | both; ForceMiss bracketed window | `key_misses` > 0; `unrendered_ticks == 0`; recovery | ForceMiss itself | a lookup failure counted as an unrendered tick | **FAIL** |
| **M-W1** | F7/R6 | both; mask write fails | row commits; row **and** event observation downgraded; `masks_dropped` > 0; `mask_file:null` | `WriterFailAt mask` | a dangling `mask_file` | **FAIL** |
| **M-W2** | F7 | both; RGB write fails | **no** commit; `writer_drops`++; hole | `WriterFailAt image` | a row without its image | **FAIL** |
| **M-W3** | F7 | both; label append fails | no commit; **image RETAINED as an orphan**; receipt reason `label_append` | `WriterFailAt label` | orphan not detected by the auditor | **FAIL** |
| **M-W4** | R6 | both; saturated enqueue | counted drop after <= one frame period; **no 10 s stall** | tiny `WriterQueueBytes` | an uncounted drop; a wait beyond the deadline | **FAIL** |
| **M-W5** | F8 | both; worker delayed across `FinishRun` and a next `StartRun` | no counter mutation or write after publication; next session uses a fresh writer; concurrent `StartRun` refused/queued by the declared policy until the join completes | delayed worker | any late mutation | s2 PASS |
| **M-PB5** | Phase B | both; pinned + AE-ON | A1.8.6 in full, **both directions** | AE-ON leg | `frames_exposure_dip_suppressed == 0` on the known-drop leg | PASS |
| **T** | throughput | **1080p native PNG**, mask+census ON, both orders, both fixtures | A1.10 | tiny byte budget | any natural drop or hole | PASS / saturation arm **FAIL** |
| **A** | auditor | every banked schema-3 session | A1.12 | corrupt a copy | a failure leg passing or a natural leg failing | per A1.12 |

**Encode vs OS-write.** M-W1/M-W2 each run twice where the paths differ: an **encoder** failure
(malformed input to the image wrapper) and an **OS write** failure (read-only directory / held
handle). Their receipts must carry different reasons.

**Full existing regression set, unchanged and required:** ONSET · PAIRING 100 % and 50 % · G7 ·
MASK-TIE · TARGET-PIXELS TIE · G-EDGE x8 · OBS-1..4 · m50 G1/G2/G6 · PB-1..PB-8 · black-frame · A44
both encodings · P-C7 v3 · the Lyra LG set · `verify_capture.py` report-only.
Scripts: `m44_gates.py`, `m44_pairing_probe.py`, `m49b_gates.py`, `m49b_pc7v3.py`, `m50_tag_owner.py`,
`m50_readout.py`, `a54_oracle.py`, `check_pose.py`, `eval_leg.py`, `subset_gate.py`,
`m36_a44_scan.ps1`, plus new `m51_bbox_oracle.py` and `m51_reconcile.py`.

---

## A1.10 Throughput

- **600 requested frames** per natural extended leg; **the first 60 are warm-up for TIMING METRICS
  ONLY** - they still count for artifact completeness, holes and drops.
- 1920x1080 **native** PNG, mask + census ON, both orders, both fixtures.
- **Capture rate and speed-ratio band.** The leg runs at the shipped `VideoFps 30`. The acceptance
  band is the **existing ship band `speed_ratio <= 1.05`** (m21, demoted to telemetry for label
  correctness but still the throughput criterion). ⚠ **Conflicting older prose is resolved here:**
  m33's note that `speed_ratio` is vacuous applies to the **pinned decoupled fork**, not to this
  bench; on StackOBot and Lyra the ratio is a live reading and 1.05 is the number. No table entry
  says "existing band" without the number.
- **Recorded:** queue-depth and byte high-water, the **total memory budget scope** (which buffers are
  charged - A1.7), **maximum deadline overshoot** (max enqueue wait minus the deadline), zero natural
  `writer_drops` / `readback_drops`, zero requested-frame holes, and the measured max `encoded/raw`
  ratio that sets `EncoderReserveFactor`.
- **Saturation arm:** `WriterQueueBytes` set **below the computed admissible single job at 1080p**
  (A1.7 gives ~9.9 MiB + reserve for the 8-bit case), so saturation is guaranteed by arithmetic
  rather than hoped for; if the revised accounting makes that value inadmissible under the
  oversize-admission rule, a **slow-worker device** is used instead. ⚠ **The draft's "3x the byte
  budget in flight" claim is withdrawn** - cumulative work is not concurrency.

---

## A1.11 P-C7 v3 - per-leg allowlists

Rules preserved: absolute counters compared as a **single constant delta**; `mask_value` by
**bijection**, never equality; `frame_indices` identical value-for-value **except** where a ruling
changes membership, declared **per leg in advance**.

**`run_summary` additions (all legs):** `capture_complete`, `requested_frames`,
`written_frame_indices`, `writer_drops`, `readback_drops`, `unrendered_ticks`, `key_misses`,
`masks_dropped`, `external_mutations`, `zero_observable_events`, `writer_reserve_overruns`,
`writer_oversize_admissions`, `observability_complete`.
**Event additions:** `event_id`, `injection_id`, `observability_basis`, `observability_complete`,
`frame_observations`, `condition_requested`. **Row additions:** `schema_version`.
**Value changes (declared, not additive):** `label_schema` 2 -> 3; empty-window `start_frame` /
`end_frame` 0 -> **-1**; `vetoed_events` -> **0**.

| leg class | `frame_indices` | event-set change | why |
|---|---|---|---|
| G-EDGE x8, ONSET, MASK-TIE, TARGET-PIXELS TIE, PAIRING, black-frame | **IDENTICAL** | none | no ruling touches them |
| OBS-1..4 | **IDENTICAL** | none | observability predicate unchanged from master |
| PB-1..PB-8 | **IDENTICAL** | none | Phase B behaviour preserved |
| legs where the m26 veto previously fired (the `SM_Ramp2` / spline-class texture and hide legs) | identical per surviving event | **event set GROWS**; `vetoed_events` 8 -> 0; `zero_observable_events` > 0 | A1.4, declared |
| M-F2b (destroyed target) | **membership CHANGES**: the post-destruction index is retained | none | F2 fix, declared |
| M-R8, M-W1..W5, M-F16 | **not comparable** - deliberately incomplete | n/a | failure legs are not P-C7 subjects |
| Lyra LG set | **IDENTICAL** | none | R2 unchanged (`NaniteOnly == Renderable`) |

---

## A1.12 Auditor - `tools/verify_integrity.py`, rewritten

The branch's version implements the **superseded** schema and would fail a correct m51 build:
`L151` asserts `anomaly_present == observable` (reversed by R1); `L116-117` applies
`shaders_pending` **per row** (reversed by R5); `L168` asserts `affected_frames ==` the observable set
(breaks R1's fallback); `L38-39` has no provision for a deliberately incomplete session or an older
schema. It is rewritten, not adopted.

**Exit codes:** `0` valid complete schema-3 · `1` schema-3 **integrity failure** (incomplete session,
hole, orphan, dangling `mask_file`, annotation index without a committed frame, membership
contradiction) · `2` **malformed / missing** artifact (unparseable JSON, absent `run_summary.json`,
absent `label_schema`) · **`3` NOT APPLICABLE** - a *valid* session of an *older* schema (1 or 2).
**2 and 3 are distinct**: "I cannot read this" is not "this is old". Silent success on either is
forbidden.

`verify_capture.py` remains the gate for v1/v2 sessions and the separate visual/label-pixel check;
**the auditor does not replace it**, and integrity consistency is **not** evidence that any anomaly
class was visible - only the verifier's pixel gate speaks to that. The pre-delivery checklist gains
the auditor box beside, not instead of, the verifier box.

**Known-answer checker cases (proven both ways before any verdict is read, `A53`/`G96`):** malformed
JSON; missing `run_summary.json`; a v2 session (must exit 3); a dangling `mask_file`; an orphan image
with no row; a truncated `written_frame_indices`; a pending-fire row (`shaders_pending` with
`observable:null`) which must **pass**.

**Unsupported pixel layout** is **incomplete** (exit 1), not a complete session of unmeasured masks -
the requested RGB never existed.

---

## A1.13 Provenance anchors

**m51 runtime A-side = `B8AC65D8` + the Phase B container.** Verified byte-identical between the
staged bench set and the archive this session.

| role | path | SHA-256 | bytes |
|---|---|---|---|
| staged exe (= A-side) | `…\Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe` | `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8` | 241,400,320 |
| archived A-side exe | `_binary_baselines\StackOBot.exe.m49b-gate-B8AC65D8` | same as above | 241,400,320 |
| container `.utoc` | `…\Content\Paks\StackOBot-Windows.utoc` | `67EA1FE0A15ACF2429516C461C5FD85989285B3C93B24C42855AE4959386257A` | 294,235 |
| container `.ucas` | `…\StackOBot-Windows.ucas` | `2CEFB8F43D048E2534471BC1D9532B77A870FAF46F14C9C2AFAE813B99B6C4D3` | 364,596,784 |
| container `.pak` | `…\StackOBot-Windows.pak` | `E03C661084D8BC2BB6EF0F51F2DECB162853B2188B733B1DBD22B96CA974CDB7` | 10,115,711 |
| `global.ucas` | `…\global.ucas` | `A16A18A8FFB5151D2E850BB52841162F6844A0361818F8D1F158E8377DFFBA9E` | 1,833,008 |
| `global.utoc` | `…\global.utoc` | `C70ECDAA232DB8CF77BEEAB47B7226C48FCC91FF705526D507500CD14CF21C51` | 539 |
| container archive | `_binary_baselines\m49b-postcook-container-2AFD2DDB\` | all five **identical** to the staged set | - |
| cooked exe (archive only) | `…\m49b-postcook-container-2AFD2DDB\StackOBot.exe` | `2AFD2DDB95B5791962CD4BDFB0852D69876338F0C6A9365F82E108B600C3FF19` | 241,399,296 |

**Historical anchor, still load-bearing (m50 A-side / exposure BEFORE picture) - preserved, not
reused as m51's A-side:**

| role | path | SHA-256 | bytes |
|---|---|---|---|
| exe | `_binary_baselines\StackOBot.exe.m50-A159EDFD` | `A159EDFD14F7D7AA47819AE1597DF701EDC01112844E3DD6931D7280A67FA96E` | 241,378,304 |
| m46 `.utoc` | `_binary_baselines\m46-container-m50exe-A159EDFD\StackOBot-Windows.utoc` | `EF8EB23C0871A0AC9E45523139F08DE6F0C9A9FF78B1E8AE5B201B924614A295` | 294,235 |
| m46 `.ucas` | same dir | `A8BFFF880B61E4CE668FDBFF6E6226FE4023858228008FCE6336EB5397FBFFE7` | 364,596,784 |
| m46 `.pak` | same dir | `3C026A8D565B6F67E7A69A826EE3F0C1BB7ABDF3D807268485D9D929D5BD908B` | 10,115,711 |
| `global.*` | same dir | `A16A18A8…` / `C70ECDAA…` | 1,833,008 / 539 |

**Lyra baseline is a MODULE SET, not a plugin SHA** (a checkout SHA does not establish which binary an
editor loads). Worktree detached `4a4bfcc2cdbade6bbeb99494e540952847ae9b63`, clean. Loaded binaries
resolve through `UnrealEditor.modules`:

| file (under `E:\UEProjects\LyraStarterGame\Plugins\AnomalyInjector\Binaries\Win64\`) | SHA-256 (first 16) | bytes | mtime |
|---|---|---|---|
| `UnrealEditor.modules` | `5AA9B9769AA5422C` | 316 | 2026-09-06 10:39:23 |
| `UnrealEditor-AnomalyCapture.dll` | `B258412F3C5CB67A` | 1,299,456 | 2026-09-06 10:39:22 |
| `UnrealEditor-AnomalyControlServer.dll` | `13F57646EB4E254E` | 251,904 | 2026-09-06 10:39:22 |
| `UnrealEditor-AnomalyShaders.dll` | `7EB5C6F0CB718182` | 64,512 | 2026-09-06 10:38:12 |
| `UnrealEditor-AnomalyInjector.dll` | `1FE44ECBF887DD5C` | 710,144 | **2026-09-04 18:01:30** |

⚠ **`AnomalyInjector.dll` predates the other three by two days.** That is consistent with Phase B
touching only `AnomalyCapture`/`AnomalyShaders`, but it means the Lyra baseline **must be frozen as
the five-file set with the manifest**, never as one timestamp or one SHA. Full 64-char hashes are
recorded at the freeze step.

**Freeze step (later, not now).** m51's candidate binaries do not exist. When the port is built, ONE
plugin source revision produces: one StackOBot exe (+ the unchanged Phase B container) and one
LyraEditor module set; both are hashed, archived and recorded here **before** the first certifying
leg. **No candidate hash is invented in advance.** The two games cannot share a host executable, so
the one-binary rule is **per fixture**; within a fixture there is no substitution mid-campaign.

---

## A1.14 OPEN arms - never converted to passes

- **R8 applied-tick-pin arm** - `tickpin_compiled == false` on both fixtures. OPEN.
- **Real decoupled-host behaviour** - `WithholdWorldRender` proves the recovery path, not a host.
  OPEN; one client-README sentence.
- **Mixed-Nanite actors (R2)** - future Lyra leg. OPEN.
- **M-F4** - if the editor-viewport route measurably fails to produce a second eligible family, F4's
  runtime evidence is OPEN and the in-candidate `NoViewBinding` falsifier is reported as weaker.

A gate set containing an OPEN arm **cannot be summarised as "all required gates pass"**.

---

## A1.15 Commit sequence - dependency-ordered

The draft's order let receipt-derived aggregation land before receipts existed. Corrected: the
**writer/receipt commit precedes the aggregation commit**.

1. F1 - resolve intentionally-unavailable masks immediately (retire the subsystem-wide hold counter
   in the same commit; add `bSveCapture` to the effective predicate).
2. F2/F11 - freeze geometry, identity and provenance at the frame boundary; render-view matrices;
   near-plane clipping; `BENCH_GEOM`.
3. F4/F5 - world/viewport/single-view binding, `Generation`, per-request terminal state.
4. R8 - stale-arm sweep, immediate cancellation with capacity release, `unrendered_ticks`,
   `WithholdWorldRender`, `NoStaleArmSweep`.
5. F9/R5 - one shared validity assessment; per-fire shader attribution.
6. **F7/F8/R6/R7 - committed-write receipts, bounded writer, reconciliation fields.**
7. **F6/R1 - schema 3, veto deleted, `zero_observable_events`** (depends on 6).
8. F3/R2 - tag lease release on fire-end **and** arms-resolved; Nanite rule preserved.
9. F10/F14 - global membership and sync parity at one sampling boundary; both global counters stay
   incremented.
10. F12/F13/F16 - strong material ownership, preview completion, layout gate.
11. F15/R3/R4 - path matching + ambiguity log; console counted, WebSocket refused.
12. `Published` FIFO eviction with explicit per-consumer cancellation.
13. Auditor rewritten to A1.12.
14. Docs.

**Future work, explicitly separated from this documentation commit:** the mixed-Nanite Lyra leg, the
Lyra drawn-residual mechanism chase (`G258`, CANDIDATE ONLY), Europa E1, and Codex's second tranche
(evidence levels, control-server responses, shipping configuration, private renderer dependencies).

---

## A1.16 Self-review of this amendment

Checked before commit: no truth-table row contradicts another (rows F-J are one predicate; K is
orthogonal; L/M/N/N2/O are disjoint terminal outcomes) · every knob named is either existing
(`ObservableMinPixels`, `OutputHeight`, `SVE`, `ForceMiss`, `ForceMissPhase`, `TagPoolLimit`,
`SynthTickOrder`, `HideOmitDepthPassSilencing`) or explicitly listed as **to be built** (A1.8) · no
lever is sent to the frozen A-side that does not exist in it (A1.8.1 states this and substitutes an
in-candidate falsifier) · every gate row names both fixtures or says why not · A1.14 lists the OPEN
arms and A1.15 forbids the "all pass" summary while they stand · completeness (A1.5) and the auditor
(A1.12) agree row for row with the gate matrix's auditor column · no source reading in this file is
described as a runtime pass (A1.1 P1).

**No separate gate manifest file is added.** The matrix in A1.9 is the single source; a second file
would be a second source of truth for the same rows.

---

*End of AMENDMENT 1. It is a DRAFT. No leg may run until the reviewer approves this exact snapshot in
writing and a follow-up brief releases execution.*

> **SUPERSEDED IN PART.** AMENDMENT 2 below replaces A1.5's `capture_complete` conjunction, A1.6's
> stale-arm expiry, A1.7 in its entirety, A1.8.1's falsifier scope, A1.9's saturation device and
> three of its falsifiers, A1.9's "no separate gate manifest file", A1.11's leg classes and A1.13's
> deferred Lyra hashes. Everything else in AMENDMENT 1 stands.

---

# AMENDMENT 2 - 2026-09-06 (brief 078-03) - BOUNDED CORRECTIONS

Scope: the four corrections C1-C4 of brief 078-03, nothing else. No implementation, no leg, no
source edit. The execution inventory referenced throughout is
**`docs/predictions/2026-09-06-m51-gate-manifest.json`** (40 recipes, 147 fixture/order expansions).

---

## A2.1 (C1) Stale arms expire by RENDER IDENTITY, not by readback capacity

**`StaleArmMaxTicks` is DELETED.** A1.6 set it to "= readback cap = 32", but 32 is a pending-readback
*capacity*, not an allowance to render an older snapshot. An unpublished arm from an earlier game
frame is *already* stale for a later scene, whatever the queue depth.

**Measured.** `ArmWanted` stores only the RequestId (`TArray<uint64> PendingWanted`,
`AnomalySveCapturer.h`) and `ConsumeWantedForPublish` (`AnomalySveCapturer.cpp:45+`) pops
`PendingWanted[0]` FIFO with **no frame comparison at all** - `FamilyFrameNumber` is used only for
logging. There is no publish-time identity check today; A1.6 spoke of "keeping" one that does not
exist. The identity is sound because the publish is game-thread:
`ISceneViewExtension::BeginRenderViewFamily` (`SceneViewExtension.h:141`, *"Called on game thread when
view family is about to be rendered"*) is invoked at `SceneRendering.cpp:4590`, while `GFrameCounter++`
is at `LaunchEngineLoop.cpp:5568`, **after** `GEngine->Tick()` at `:5363` => a healthy arm and its
publish share one `GFrameCounter`, and a later family necessarily carries a larger one.

**The rule:**

1. Each arm carries `ArmGameFrame = GFrameCounter` at `OnWorldTickEnd`, beside
   `(Generation, SessionIndex, RequestId)`.
2. **Publish-time rejection.** `ConsumeWantedForPublish` compares the head entry's `ArmGameFrame`
   against `GFrameCounter`. Not equal => not consumed: retired terminal-`Cancelled(unrendered)`,
   `unrendered_ticks` increments, capacity released, loop advances. The family then consumes a
   **fresh** arm of its own game frame, or none.
3. **Game-thread sweep before admission and before the cap check**, retiring every unpublished arm
   whose `ArmGameFrame != GFrameCounter`. Needed because a family may never arrive at all, in which
   case rule 2 never runs and capacity never returns.
4. **Published GPU work is NEVER expired by this sweep** - only entries whose publish has not begun.
   Synchronisation is the existing `StateCS`; a publish that wins the lock consumes normally and the
   sweep then sees a shorter list.
5. **Same-session terminal guard**: a generation check cannot reject a late completion for a request
   cancelled *within the same session*, so each request carries a terminal flag and a completion for
   a terminal request is discarded and logged.
6. **Capacity is retired EXACTLY ONCE**, at the transition into any terminal state, by either route.

**A1.8.1's falsifier is corrected.** It predicted `NoStaleArmSweep` alone prevents recovery. With rule
2 retained that does not follow - publish-time rejection would retire the backlog as soon as rendering
resumed, and the leg would recover for the wrong reason. => **`IAI.Bench.NoStaleArmSweep 1` must
bypass ALL stale-request capacity-retirement routes for the test interval** (rules 2 and 3) **while
RETAINING rule 2's identity guard**, so no leg ever deliberately labels a later render with an older
snapshot. ON: pending stays at the cap after resumption, `readback_drops` rises, no recovery,
`unrendered_ticks` does not account for the backlog. OFF: pending returns to zero, `>= 3` counted
events after resumption, every requested index committed or terminal-`Cancelled(unrendered)` with none
in neither set. **Normal cleanup is restored at teardown** so an intended failure does not hang
`FinishRun`.

**Unchanged from A1.8.1:** `WithholdWorldRender` is the real no-render device (`bDisableWorldRendering`
gates `BeginRenderingViewFamily` at `GameViewportClient.cpp:1707`, and the plugin's own
`BeginRenderViewFamily` is downstream of that gate, so nothing publishes) with independently observed
capture opportunities, a restored prior flag and `>= 3` counted events after resumption. `ForceMiss`
stays a **different** failure - `key_misses` counts accepted session capture requests whose published
key was corrupted, never unrelated non-wanted keys or preview families. `B8AC65D8` has neither the new
levers nor an admission cap and is not asked to run them.

**New recipe `M-R8c`** isolates identity from capacity: a **3-tick** withhold burst, far below any
limit, in which the three arms must still be retired *by identity*.

---

## A2.2 (C2) Deterministic admission - A1.7 is replaced entirely

**Rejected from A1.7:** the measured-`encoded/raw` `EncoderReserveFactor`; over-budget completion with
`writer_reserve_overruns`; the oversize-admission exception with `writer_oversize_admissions`. A
measured maximum compression ratio is not a bound on future content, over-budget admission reverses
R6, and a release default may not depend on a calibration capture that has not been run.

**Defaults:** `WriterWorkerCount N = 2`, `WriterQueueBytes = 256 MiB`, **charged including jobs in
service**, not merely queued.

**Deterministic reservation, from the local implementation.** A job's plugin-owned buffers are known
exactly at admission from source format and dimensions: `Job.RawBytes` = `W*H*BPP(SrcFormat)`, moved
into the job and held for its life (`AnomalyAsyncWriter.h`); `Pixels` = `W*H*4` from
`ConvertTightToBGRA` (`AnomalyLabelWriter.cpp:248`, colour jobs only); `Scaled` = `OutW*OutH*4` from
`ResampleAndEncodeBGRA` (`:347`, only when `OutputHeight != 0`); `Job.Record` charged a flat 64 KiB.

`reservation_colour = W*H*(BPP + 4) + [OutW*OutH*4 if resampling] + 65536`
`reservation_mask   = W*H + 65536` (`EncodeGray8Png` calls `SetRaw` with `ERGBFormat::Gray` and does
**no** BGRA conversion, so a mask job has no second buffer).

At 1920x1080: 8-bit/packed-10 colour native **15.88 MiB**, `PF_FloatRGBA` **23.79 MiB**, 8-bit +
resample to 360p **16.76 MiB**, mask **2.04 MiB**. Against 256 MiB that is 16 charged 8-bit colour
jobs, so **`N = 2` binds and the byte cap is headroom** at every resolution either fixture uses. The
first single 8-bit colour job to exceed 256 MiB needs `W*H > 33,546,240 px`: 7680x4320 fits
(253.2 MiB), 8192x4320 does not.

**There is NO oversize admission exception.** Invalid configuration is **REFUSED AT `StartRun` with
the value named** (m21/`A48`): `WriterWorkerCount` outside `[1,16]`, `WriterQueueBytes` smaller than
one admissible job at the run's own resolution and format, `ReadbackPendingCap` outside `[1,256]`. An
**accepted** request whose job cannot fit - reachable mid-run, since frame dimensions can change
mid-session and `AnomalyAsyncWriter::NoteWrittenSize` already detects exactly that - is a **counted
failure**: `writer_oversize_failures` increments, the index is a hole, `capture_complete` is false.
Never silently admitted over budget. An encoder or output-limit breach is likewise a **controlled
failure** - the job fails, the receipt records the reason, the index is a hole. Never truncation,
never overrun-and-success.

**Deadline.** The enqueue wait is `min(one capture-frame period, WriterMaxWaitMs)` - **33.33 ms** at
the shipped `VideoFps 30`. If the capture rate is zero or invalid the period is undefined and the wait
falls back to `WriterMaxWaitMs` (default 40 ms), logged. The wait is on a condition variable released
before any worker-needed lock, so the game thread never holds the queue lock while sleeping. (Today's
`FlushPending` bounds, 2.0 s at `:601` and 5.0 s at `:4089`, are *flush* timeouts and are unchanged.)

**What the byte cap bounds, honestly.** It bounds **plugin-owned raw, converted and resampled buffers
exactly**. It does **not** bound codec-internal allocation, the encoded output, GPU memory, the
readback staging buffers (`ReadbackPendingCap` governs those), the engine allocator, or process RSS.
**No hard cap on process/GPU/allocator memory is claimed anywhere.**

**Codec scratch - the exact source reason, and an OPEN prerequisite.**
`IImageWrapper::GetCompressed` (`Engine/Source/Runtime/ImageWrapper/Public/IImageWrapper.h:164`) is
`virtual TArray64<uint8> GetCompressed(int32 Quality = 0) = 0;` - a whole-buffer return by value with
no size hint, no maximum, no chunked or streaming variant, no allocator injection point, and no way to
query the compressed size beforehand. **An a-priori byte bound on encoded output is not obtainable
through this interface**, and no `EncoderReserveFactor` is invented to stand in for one. Worse,
`AnomalyPreviewCapture.cpp` binds the returned temporary to a const reference then `Memcpy`s it into
`OutBytes`, so the encoded image is resident **twice** in plugin-visible memory.

Codec scratch is therefore bounded **structurally**, not by bytes: at most `N` concurrent encodes,
each with an input no larger than the maximum admissible reservation - a worker-count / input-size
contract. Two follow-ons, both **OPEN implementation prerequisites, not solved design**: **(a)** drop
the redundant copy by handing the returned array straight to the file write (a strict improvement,
available today, no API change); **(b)** a true byte bound needs an encoder that streams to disk
through a caller-provided buffer, which `IImageWrapper` in UE 5.1 does not offer. **Until (b) exists
the design is incomplete on this axis and is recorded as such.**

**Saturation device.** `IAI.Bench.WriterWorkerDelayMs <ms> <ticks>` with **valid** settings: `N=2`,
256 MiB, 1080p native PNG, `VideoFps 30`, **250 ms** per-job delay for **600 ticks**. Demand 30
jobs/s against service 8 jobs/s fills the 16 charged slots in ~0.64 s; thereafter every arrival waits
at most 33.33 ms and is dropped and counted - over 600 requested frames, order ~150 committed and
~450 drops. **The gate is on the invariants** (`writer_drops > 0`, every drop counted, no wait beyond
the deadline, no 10 s stall, no uncounted hole) because the count depends on measured encode time.
A1.9's "tiny `WriterQueueBytes`" arm is **withdrawn** - that value is now refused at `StartRun`, and a
configuration refusal is not an in-capture backpressure test; it survives as `M-W4v`.

**Throughput unchanged:** 600 requested frames, first 60 warm-up **for timing metrics only** (still
counting for completeness, holes and drops), 1080p native, `VideoFps 30`, `speed_ratio <= 1.05`. The
measured max `encoded/raw` ratio is a **reading only** and sets nothing. The bar is fixed before the
leg and is never lowered after seeing a result.

---

## A2.3 (C3) Completeness, the two ledgers, and the membership sampler

**(a) `mask_write_required`, not "mask iff armed".** A1.5's "mask iff armed" fails every legitimately
empty frame. Measured: `Snap.MaskFileRel` is assigned **at arm** when `bTargetMaskEffective`
(`AnomalyCaptureSubsystem.cpp:4196-4198`) - that is *requested*; the effective artifact policy is
*"the PNG exists if and only if it has content - no all-zero PNG is ever written"* (`:3011`, `:6472`),
with `mask_state` Present/Empty/Unmeasured (`:1279-1292`).

```
mask_requested      := bTargetMaskEffective at arm
mask_write_required := (outcome == Present AND the reduced mask has >= 1 non-zero pixel)
```

`mask_write_required` is computed from the effective policy **before the write is attempted** and
recorded on the receipt, distinct from requested/armed status. Required and the write fails =>
`masks_dropped++`, `mask_file:null`, row **and** event observation downgraded (R6), receipt reason
`mask_required_but_absent`, **`capture_complete:false`** - **downgrading the row does not clear the
obligation**. Not required (Empty, Unmeasured, no content) => `mask_file:null` is correct; **no
promised artifact exists, so it is not an artifact failure** and does not touch `capture_complete`.

**(b) Prepublication readiness is separated from publication success.** A1.5's conjunction included
`run_summary.json` written successfully, which is self-referential.

```
candidate_complete := committed_receipts.index_set == requested_indices
                   && every receipt's REQUIRED artifacts written (mask per (a))
                   && writer_drops == readback_drops == unrendered_ticks == 0
                   && masks_dropped == key_misses == writer_oversize_failures == 0
                   && every accepted request terminal
                   && labels.jsonl closed OK && annotation.json written OK
publication_ok     := candidate_complete && run_summary.json written OK
```

The serialised `capture_complete` is **`candidate_complete`** - computed on the game thread from
reconciled receipts, complete before serialisation, making **no claim about its own write**.
`labels.jsonl` and `annotation.json` are observable because both precede it (annotation `:5002`, run
summary `:5240`, so **run_summary is already last today**); m51 adds tmp+rename and the two success
checks, and the success notification is emitted **only after all required metadata commits succeed**.
An absent or failed `run_summary.json` is a **FAILED session to the auditor** (exit 2) - that is what
supplies the last link, not a field asserting its own write.

**(c) The two ledgers, and A1.5's blanket claim corrected.** A1.5 said *"a dropped frame stays in
`injected_frames` and is absent from `affected_frames`"*. Measured, that is wrong **twice**.
`AccumulateFrameEvents` is called from the **drain** (`:3998`) and the sync path (`:4282`) - only for
frames that produced a readback result - so an **armed-but-never-drained** frame is in **neither**
ledger, which is why `requested_frames` must come from the FSM's armed set and not from
`injected_frames`. And the writer is fire-and-forget, with accumulation **before** it, so a
**drained-but-write-failed** frame *is* in `injected_frames` and carries an observability verdict with
no image on disk. The R1 fallback (`:5641-5670`) then sets `bObservabilityMeasured = (Measured > 0)`,
a dropped index has no `ObservableByIndex` entry and counts Unmeasured, and at zero measured
`affected_frames` is left equal to `injected_frames` - **so on a zero-measured incomplete session
`affected_frames` contains indices with no image.**

Disposition, preserving both ledgers and R1: `injected_frames` and `affected_frames` keep their agreed
meanings - **the requested ledger is not erased and no schema meaning is flipped**;
`observability_basis` in {`measured`, `fallback_injected`} distinguishes **observable membership from
fallback membership** (a fallback window is injection-labelled data, not verified visible ground
truth - contract section 3); `observability_complete` is false when any injected index lacks a
committed receipt with all required artifacts; a member index whose receipt is not committed reads
**unknown/incomplete** in `frame_observations`, never as a successful visible annotation. **The
auditor fails the incomplete session on the RECONCILIATION** (`capture_complete:false`, hole, orphan),
and when `observability_basis == "fallback_injected"` it **does not apply the observable-only equality
rule** - applying it there is what would manufacture an R1 contradiction.

**(d) The membership sampler, named from code.**
`UAnomalyCaptureSubsystem::IsFireLabelledThisFrame` (`AnomalyCaptureSubsystem.cpp:4620`), sampled once
per fire per armed frame at `OnWorldTickEnd` into `Snap->FireLabelled` (`:4580-4584`) and **frozen in
the snapshot**: `ActorHidden` -> `AnomalyHiddenClass::IsLogicallyHidden(FActor)`; `AnomalyState` ->
`Injector->IsAnomalyCurrentlyAnomalous(F.Id)`; default (`FireWindow`) -> `true`. The observable
predicate at `:3880` consumes exactly that frozen bit. **m51 preserves this sampler unchanged**; the
vague "an injection exists" is **not** the condition and must not replace it.

**(e) A second simultaneous capture context is declined explicitly.** `AnomalySveKeyRing` is a
**namespace singleton** (`PublishKey`/`LookupKey`/`Reset`/`GetCounters` are free functions,
`AnomalySveKeyRing.h`) - one ring per process - and `AnomalyStencilTag` likewise applies tags directly
to actor components through free functions. Two contexts share both irreducibly and FIFO consumption
would cross-attribute => a second `StartRun` while one is active is **REFUSED at `StartRun`** with
reason `capture_context_active`, **before any pixel is captured**; interference is never merely
noticed afterwards. Recipe `M-F5b`.

---

## A2.4 (C4) The gate/provenance manifest

**A1.9's "no separate gate manifest file is added" is withdrawn**, and with it A1.11's leg classes and
A1.13's deferred Lyra hashes: those were uncompleted deliverables, not reviewer questions.

**`docs/predictions/2026-09-06-m51-gate-manifest.json` is the single execution inventory** - 40
recipes, 147 fixture/order expansions, 16 harnesses whose CLI contracts were read out of the scripts
this session, and the full provenance block. Per recipe: unique ID, fixture, target type, exact
absolute script path with verified arguments, capture config, expected event validity, expected
artifacts/counters, auditor exit, falsifier, stop condition, and either the declared P-C7 deltas or
the reason it is not a P-C7 subject. A1.9's table remains the narrative rationale; the manifest is
authoritative for what is runnable. Only the corrections are restated here.

**Nothing is pretended to exist.** `m51_bbox_oracle.py`, `m51_reconcile.py` and the rewritten
`tools/verify_integrity.py` are `planned, not implemented`, each with a proposed CLI and the commit it
depends on; thirteen levers likewise. (The auditor exists on `be16977` only; its `--reference` and
pixel arguments are dropped - that is `verify_capture.py`'s job.) Every recipe whose devices are
absent carries `"runnable_now": false` and its `blocked_on` list.

**Three A1.9 falsifiers were wrong, not merely missing** - an ordinary input variation is not
automatically a falsifier. `NoTagRetire` *holds* leases and so prevents the very reuse M-F3b needs; it
cannot manufacture cross-attribution, so M-F3b needs `IAI.Bench.ForceEarlyTagRetire` (release at
fire-end ignoring `ArmsIssued == ArmsResolved`) and `NoTagRetire` stays **M-F3's** exhaustion
falsifier. Forcing **both** fires pending makes both unknown under either the per-fire or the
frame-wide rule and cannot discriminate; M-R5 needs `IAI.Bench.ForceShaderPendingForFire <index>`.
Synth tick order is **not** a bypass of M-F4's view binding; that is `IAI.Bench.NoViewBinding 1`. Two
devices are marked **MISSING** rather than dressed up: M-F11 has no runtime defect injector (its
can-fail is the oracle's own `--perturb-px` selftest) and M-F10 has none for orphaned counters.

**No lever is sent to an archived executable that lacks it.** `ReadbackDelayTicks`, the destroy-target
device, the writer devices and the sweep bypass are all absent from `B8AC65D8`: M-F2a/M-F2b use the
in-candidate `NoGeometryFreeze` bypass; M-F4's A-side is **OPEN** (a packaged exe cannot host PIE, and
no editor-target archive exists for either fixture - a named prerequisite, not a fabricated baseline);
M-F5's A-side arm is timing-dependent and is reported **NOT EXERCISED** if it does not reproduce.
`ForceMiss` and `obj gc` *do* exist there, so M-R8b and M-F12 run real A-side arms.

**P-C7 allowlists are per leg and include what A1.11 omitted:** `visible_positive`,
`bbox_px`/`bbox_norm`/`bbox_drawn_px`, `mask_file`, `target_pixels`/`observable`/`mask_state`, and
per-fire `render_state`. **No global "allow all changes" entry exists.** `writer_reserve_overruns` and
`writer_oversize_admissions` are **removed** (no referent under A2.2); `writer_oversize_failures`
replaces the latter and counts a refusal, not an over-budget admission.

**Provenance - every hash computed this session, not copied.** All eight StackOBot current-A-side
values reproduce A1.13 exactly and staged == archive byte-identical; the m46 container and the Phase B
post-cook archive are added at full length. **Lyra is a five-file module set plus its manifest**, now
at all 64 characters, with `UnrealEditor.modules` resolved to the actual DLL names, its `BuildId`, the
four `.pdb` companions, worktree `4a4bfcc`, the project path and engine GUID
`{B34F356C-4AE7-256A-F0E1-318A632BB902}`. `AnomalyInjector.dll` still predates the other three by two
days, which is why the baseline is the set plus the manifest and never one SHA. **No candidate hash is
recorded in advance**, and no unqualified numeric claim such as `vetoed_events 8 -> 0` appears without
the leg class it belongs to.

---

## A2.5 Self-check of this amendment, against the document

Checked in the text, not by running anything:

1. **Prior-game-frame unpublished arm expires before new admission** - A2.1 rules 2-3; the sweep
   precedes the cap check, and `M-R8c` isolates it at N=3 where capacity is never in play.
2. **Published delayed readback survives** - A2.1 rule 4: only entries whose publish has not begun are
   removed, so in-flight GPU work is never orphaned.
3. **Oversize job cannot be admitted over cap** - A2.2: refused at `StartRun` as a configuration
   property, counted `writer_oversize_failures` and holed mid-run. No admission exception exists here.
4. **Zero measured + failed RGB keeps fallback semantics but fails integrity** - A2.3(c): the event
   keeps `affected_frames = injected_frames` with `observability_basis: fallback_injected`, the missing
   index reads unknown/incomplete, and the session fails on the **reconciliation**, not on membership.
5. **Armed invalid mask has no invented PNG requirement** - A2.3(a): `mask_write_required` is false for
   Empty and Unmeasured, so `mask_file:null` is correct there and not an artifact failure.
6. **`NoStaleArmSweep` alone is not claimed to prevent every other cancellation route** - A2.1 says
   publish-time retirement would recover on its own, and widens the falsifier's scope accordingly.
7. **Every existing hash is full length** - all 64 characters, computed this session.
8. **Every runnable recipe has an actual or proposed executable command and a classified falsifier** -
   40 recipes with `command_*`, `runnable_now`, `blocked_on`, and a falsifier `class` from
   {`defect_injector`, `both_ways_control`, `frozen_a_side`, `MISSING`}.

**Not claimed:** that every m51 requirement is now met. A2.2(b) leaves a bounded encoded-output
guarantee **OPEN**; M-F11 and M-F10 have **no defect injector**; M-F4's A-side is **OPEN**; and
AMENDMENT 1's four open arms (applied tick pin, real decoupled host, mixed-Nanite, M-F4) stand. A gate
set containing an OPEN arm may not be summarised as "all required gates pass".

---

*End of AMENDMENT 2. It is a DRAFT. No leg may run, and no implementation may start, until the
reviewer approves in writing and a follow-up brief releases execution.*

---

## DISPOSITION 2026-09-06 (brief 078-04) - F1 RELEASED, EVERYTHING ELSE HELD

The reviewer's 078-03 review returns **CHANGES REQUIRED on the plan** and **APPROVED for a bounded
F1 phase**. This is a disposition, not a third amendment: AMENDMENT 2 is unchanged.

**Released:** F1 implementation (commit 1 only), the M-F1 manifest record made executable, and the
M-F1 legs on StackOBot in both tick orders, A-side and B-side, plus an ONSET/PAIRING/G7 smoke on the
B-side. **Held, unexecuted and unmodified:** every other manifest record, commits 2-14, the auditor,
`m51_pc7.py`, `m51_reconcile.py`, `m51_bbox_oracle.py`, all bench levers, and all other legs.

**This candidate is NOT m51 schema 3.** It emits no `capture_complete`, `requested_frames`,
`written_frame_indices`, `schema_version`, `event_id` or `observability_basis`, and `label_schema`
stays **2**. Completeness for these legs is read from the existing artifact - rows, image filenames,
`run_summary.json` and the arm/NEVER-RESOLVED/finalize log lines. A recipe that demands a schema-3
key of this binary is a defect in the recipe.

**Two review proposals are NOT adopted here and neither blocks F1.**
- **Table-row 6, `injected_frames := RequestedIndices ∩ committed`:** REJECTED for this phase. It
  changes an already-agreed client-facing meaning; **R1 and A2.3(c)'s two-ledger disposition remain
  in force**, and `observability_complete` keeps its A2.3 semantics. Any alternative is a separately
  agreed migration, not a side effect of a drain fix.
- **The `deflateBound` PNG reservation:** a PROPOSAL to verify against the actual local encoder in
  the later writer tranche. A2.2's item (b) stays **OPEN**; nothing here treats it as a proved bound.

**One premise in the F1 brief is CORRECTED at source, and the correction is the conservative one.**
The brief asks that "awaiting" additionally require `bSveCapture`, on the premise that a backbuffer
run cannot produce a mask. **Source says otherwise.** The target mask is a SEPARATE scene view
extension (`FAnomalyMaskSceneViewExtension`), created whenever `bMaskMeasure`
(`AnomalyCaptureSubsystem.cpp:3712`), active whenever it has a pending arm
(`AnomalyMaskSceneViewExtension.cpp:49-52`), and armed with its **own** request id
(`GTargetMaskRequestBit | ++TargetMaskOwnSerial`, `:1159`) that is never the colour path's id. It
does not consult `bSveCapture` anywhere. ⇒ **adding `bSveCapture` to the effective predicate would
stop ARMING masks on a path where they work**, silently downgrading every backbuffer run to
`unmeasured` - a behaviour regression on a shipped path, justified by an unverified mechanism, which
is precisely what this project's own invariant forbids.
⚠ **It is also unmeasured in both directions:** every banked backbuffer leg
(`M31_*_BB`, `M35_GM7_*`, `RESCUE_S43_backbuffer`) **predates `m43`** and carries no `target_mask_*`
keys at all, so backbuffer × target-mask **has never been run**. ⇒ `bTargetMaskEffective` is left as
it is; **M-F1b stays UNRELEASED and OPEN**, and it is the leg that would settle it. Reported as a
**NEEDS-DECISION**, not silently resolved either way.

**Impossible arming is already prevented and was not re-guarded.** Target-mask arming is gated on
`bTargetMaskEffective` at both its entry points (`:830` `bCapturedThisTick`, `:1170`
`ServiceTargetMask`), so no mask is armed when the mask is ineffective. F1's defect was never the
arm - it was that the DRAIN waited on the *requested* flag. Adding a redundant guard would have
implied a second mechanism where one already holds.

**What F1 changes, in one sentence:** the drain now waits only for a snapshot that can actually
receive an outcome (`bAwaitingTargetMask`, frozen from `bTargetMaskEffective` at arm), each waiting
snapshot carries **its own** bounded counter instead of one subsystem-wide counter that any resolving
frame could reset, and a requested-but-not-awaiting snapshot is initialised honestly as unmeasured
and committed immediately.
