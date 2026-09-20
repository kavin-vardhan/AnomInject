# m55 — measured change evidence: make `observable` mean measured visible change

**PLAN ONLY. No source, shader, tool, fixture or schema change was made. No build, no cook, no capture,
no leg. Branch `feat/m55-change-evidence` off `master` `031a103`; `m51` `53bf725` untouched and the main
checkout was never moved (this unit was authored in a worktree outside the StackOBot tree, shared-tree
rule 2).**

**This document is written to be reviewed BEFORE implementation and it is not self-approving.** Section
§13 carries the questions a reviewer must answer first; §2.6 carries a correction to the brief's own
control design that a reviewer should rule on explicitly, because building the brief as literally worded
would repeat a refutation this project already has on the record (`G260`).

---

## §0 Why this unit exists

Three independent findings converge on one gap:

- **`G263` (079-07, with its 079-09 dated correction).** The verifier can say *that* pixels changed and,
  with masks, *which* pixels — it cannot say *what caused* the change. It deliberately issues no
  label-correctness verdict. Attribution has to come from the side that knows what it injected.
- **`G270` (m52, 080-04).** The `stuck_low_mip` perceptibility ratio is a *size* test. On Lyra, events at
  ratio **26.76 / 26.76 / 31.59** — three to four times the 8.0 gate — read `NO-TRACE`, because a
  low-frequency paint or normal map is imperceptible at any achievable mip. No predicate computed from
  bounds and mip sizes can see what a texture *contains*.
- **`m49` A1.** For texture-class anomalies `observable` is `labelled && held && target_pixels >= N`
  (`AnomalyCaptureSubsystem.cpp:4016`). Every term is about **presence**: the fire is labelled, the
  anomaly's condition is held, and the target drew front-most pixels. Nothing in it asks whether the
  picture *changed*.

The client's F2 complaint — *labelled, nothing visible* — is only closed when the producer measures, per
frame, how much the target's own pixels changed. `m55` is that measurement.

⛔ **What `m55` cannot do, stated first so it is not claimed later.** `G263`'s correction is not repealed
by moving the measurement in-engine. A mask identifies pixels, not causes; lighting, occlusion and
animation move the same pixels. `m55` makes `observable` mean *the target's own pixels measurably
changed when the anomaly engaged*. It does **not** make `observable` mean *the anomaly caused the
change*, and no wording in the schema, the client docs or a commit message may say that it does.

---

## §1 D1 — what to measure, and out of which buffer

### 1.1 Which colour buffer the capture actually reads (the brief asks this explicitly)

| | hook | `EPostProcessingPass` index | what it sees |
|---|---|---|---|
| mask pass (`m26`/`m34`/`m49b`) | `Tonemap` | 2 | post-tonemap, **pre-FXAA** scene colour |
| colour capture (the delivered PNG) | `VisualizeDepthOfField` | 4 | post-tonemap, **post-FXAA** scene colour |

- `AnomalyMaskSceneViewExtension.cpp:97-101` subscribes the mask to `EPostProcessingPass::Tonemap`.
- `AnomalySceneViewExtension.cpp:72` subscribes the colour capture to
  `EPostProcessingPass::VisualizeDepthOfField`.
- The enum order is `SSRInput, MotionBlur, Tonemap, FXAA, VisualizeDepthOfField, MAX`
  (`Engine/Source/Runtime/Engine/Public/SceneViewExtension.h`).

⇒ **the mask is produced two subscribable passes before the pixels the client receives.** That is not a
defect today — the mask never reads colour — but it decides where `m55` must run (§7).

### 1.2 What m49 reads (the brief asks this too): no colour at all

`target_pixels` and `target_drawn_pixels` are derived entirely from **custom stencil + custom depth +
scene depth** (`AnomalyVisibleMask.usf:31, 39-40, 44, 51`), reduced per tag by
`AnomalyMaskReduce.usf:37-52`. There is no colour sample anywhere in that chain.

⇒ **`m55` would be the first colour-derived label evidence in the producer.** The one existing
colour-derived quantity is `m48`'s exposure dip, and it is a *CPU*, *whole-frame*, *subsampled* mean
luminance taken from the already-read-back delivered bytes
(`AnomalyCaptureSubsystem.cpp:4061-4063`) — neither per-target nor per-pixel. Nothing existing is being
extended; this is a new kind of evidence and it needs its own can-fail gates.

### 1.3 The quantity

For each captured frame `N` carrying a target mask, and for each reserved tag `t` present in that
frame's mask:

```
d(p)          = max over colour channels of | cur(p) - prev(p) |, normalised to [0,1]
change_px(t)  = count of p where mask_N(p) == t and d(p) > tau_px
change_sum(t) = sum over p where mask_N(p) == t of d(p)
change_frac_target(t) = change_px(t) / target_pixels(t)
change_mean_target(t) = change_sum(t) / target_pixels(t)
```

`prev` is the **previous captured frame's delivered colour** — session index `N-1`, pinned by identity,
never by adjacency (§5.3).

The denominator is the **current** frame's `Count[t]` from RT0 — i.e. the same `target_pixels` the row
already publishes, from the same render. It is not recomputed and cannot disagree.

**Why `max abs channel` and not luminance.** `corrupted_texture` swaps a surface to solid magenta; a
green→magenta swap can be near-neutral in luminance while being the most visible change the catalogue
produces. A luma-only metric would under-read exactly the anomaly it most needs to see. Chroma-aware is
the conservative choice and costs one `max` per pixel.

**Why a count *and* a sum.** `change_px` answers *how much of the target changed*; `change_mean` answers
*by how much*. A mip-blur that shifts every pixel by a little and a decal that flips a few pixels a lot
are different events, and `G270`'s Lyra class is precisely the "many pixels, tiny amplitude" corner. One
number could not separate them. `change_mean` is also **the statistic the verifier already prints as
`d=`**, which is what makes §6's cross-check a comparison of like with like rather than of two different
measurements.

**Two thresholds, accumulated in the same pass.** `change_px` is accumulated at **two** predeclared
per-pixel thresholds, `tau_lo` and `tau_hi`, at no extra dispatch. This is deliberate: it lets the class
floors of §9 be derived **offline from banked legs** rather than by re-running the campaign once per
candidate threshold, and it makes the metric's sensitivity visible in the artifact instead of buried in a
compiled constant.

### 1.4 The control, and the whole-frame confound

The same dispatch accumulates, at a reserved slot and over **every** pixel including untagged ones:
`changed_all_lo`, `changed_all_hi`, `sum_all`. With `W*H` known on the CPU:

```
change_frac_control = (changed_all_lo - change_px(t)) / (W*H - target_pixels(t))
```

— the frame outside the target, on the same frame pair, from the same render.

### 1.5 The whole-frame control is a REFUSAL input, not a subtrahend — a correction to the brief

The brief asks for `change_frac_target - change_frac_control` to be *"what the label carries"*.
**That specific step should not be built, and the reason is on this project's own record.**

`G260` (079-02/03) built exactly that subtraction, gated it, and had it **refuted by its own gates in
both directions at once**:

- **It does not cancel camera motion.** Motion is parallax- and content-weighted: a region of near,
  detailed geometry changes far more under a given camera move than flat distant wall. The difference
  carries a large content-dependent variance instead of cancelling.
- **It destroys real signal on a still camera, which is worse.** On a pinned bench leg the onset read
  `d_region = 0.02286` against `d_ring = 0.02872`, so the subtracted quantity went **negative at the
  exact frame the anomaly started**.
- **Its premise is false for these anomalies.** The effect leaves the region: `m26`'s `A35`/ruling `A-4`
  measured hiding `SM_Ramp2` changing **more outside its own bbox than inside** (peak-OUT `0.2955`
  against peak-IN `0.1785`). `G258` is the same family.

Neither failure is a property of *where* the measurement is taken. Both are properties of the **scene**.
Moving the measurement in-engine fixes the attribution of *which pixels* (which is real and valuable) and
fixes nothing about parallax weighting or about bounce light landing outside the silhouette. The brief's
justification — *same frame pair, same render, no attribution claim* — removes a pairing confound that
`G260` was never about.

⇒ **`m55` publishes `change_frac_target` and `change_frac_control` as two separate measured fields and
subtracts neither.** `G260`'s own stated rule is followed instead: *where the surroundings cannot be a
control, MEASURE the confound and REFUSE above a threshold.* The control becomes a refusal input —
above a predeclared `change_frac_control` cap the frame's change evidence is `unmeasured`, which
**admits** (§4).

📌 **This costs the reviewer nothing.** Both fields ship, so a subtractive rule remains exactly one
arithmetic step away in any downstream consumer if chat rules for it — without a code change, without a
re-capture, and with the banked evidence to test it on. What is refused here is *baking the subtraction
into the label*, which is the irreversible half.

⛔ A **spatial ring** (dilated bbox minus bbox) was also considered and is rejected for the same reason
plus one more: the reduce learns the tag's bounds *in the same pass* that would need them, so a ring
needs either a second dispatch or a one-frame-stale bbox. It buys nothing `G260` has not already refuted.

---

## §2 D2 — what change evidence means per anomaly class

The catalogue is **10** (`GAutoPool` is 7, default-enabled 4 —
`AnomalyAutoInjectorSubsystem.cpp:22-30`). The active-source taxonomy the capture already uses is at
`AnomalyCaptureSubsystem.cpp:281-292`.

| anomaly | active source | in default pool | has a target mask? | change evidence | `observable` under m55 |
|---|---|---|---|---|---|
| `blinking` | ActorHidden | yes | yes (silhouette survives; `m45` keeps custom depth) | onset transition **and** `target_drawn_pixels == 0` thereafter | m49 rule ∧ onset evidence |
| `missing_object` | ActorHidden | no (in pool) | yes | same | m49 rule ∧ onset evidence |
| `missing_texture` | FireWindow | yes | yes | onset transition over the silhouette | m49 rule ∧ onset evidence |
| `corrupted_texture` | FireWindow | yes | yes | onset transition over the silhouette | m49 rule ∧ onset evidence |
| `stuck_low_mip` | AnomalyState (deferred onset) | no | yes | unheld→held transition at the `080-06` window start | m49 rule ∧ onset evidence |
| `lod_popping` | AnomalyState | yes | yes | **repeated** transitions, one per toggle | m49 rule ∧ onset evidence |
| `lod_corruption` | FireWindow | no (not in pool) | yes | one-shot forced LOD ⇒ onset transition only | m49 rule ∧ onset evidence |
| `lighting_mismatch` | FireWindow | no (not in pool) | **no** — see §2.3 | whole-frame reading only | **`null`, unchanged** |
| `camera_clipping` | AnomalyState | yes | **no** — global, no target | whole-frame reading only | **`null`, unchanged** |
| `time_dilation` | FireWindow | no (hidden) | **no** — global | whole-frame reading only | **`null`, unchanged** |

### 2.1 Hide class (`blinking`, `missing_object`)

`m45` hides by dropping the main and depth passes **while keeping `bRenderCustomDepth`**, so a hidden
target still has an RT0 silhouette and a denominator. The onset frame pair is (last visible) → (first
hidden) and the change is large over exactly those pixels.

Within the hold there is nothing to compare — two consecutive hidden frames are identical. The brief's
own formulation is adopted: **onset-frame change, plus `target_drawn_pixels == 0` thereafter**, and the
second half already exists (`m49` Phase B). `G258` governs the asymmetry: `drawn == 0` is strong evidence
of absence; `drawn > 0` establishes nothing. `m55` does not change that and must not be read as
reinstating the veto the 077 ruling removed.

### 2.2 Texture class (`corrupted_texture`, `missing_texture`, `stuck_low_mip`, and a future UV-corruption `m54`)

The silhouette persists and is fully drawn, so the denominator is large and stable; the change is a
**content** change inside it. This is the class `G270` is about and the one `m55` exists to close: a
swap that changes nothing visible reads `change_frac_target ≈ change_frac_control ≈ 0` and is no longer
called observable on size alone.

`stuck_low_mip` is the one with a **deferred onset**, and `080-06` already put the labelled window at the
first measurably-held frame. That is a gift: the frame pair straddling the window start is exactly
unheld→held, so the transition is *in* the measured pair by construction rather than by luck.
⚠ **Risk, named: the streamer's mip arrival can be gradual**, spreading the transition over several
frames and leaving no single pair above a floor. §4.3's first-K latch is the mitigation and §10's `G-5`
is the gate that would expose it.

### 2.3 Light class (`lighting_mismatch`, and a future `m53`) — what the mask pass can and cannot give

**It can give nothing, and this is structural rather than a gap to be filled by tuning.**

`lighting_mismatch` matches `ULightComponent` by substring across the world
(`Anomaly_LightingMismatch.cpp:48`). The renderable-visible set the selector and the mask both work from
is static-or-skinned meshes only (`G33`), and the stencil tag is written onto
`UPrimitiveComponent`s. A light actor carries no primitive component, so `AnomalyStencilTag::TagActor`
tags nothing, `ArmTargetMaskOwn` finds no visible taggable actor
(`AnomalyCaptureSubsystem.cpp:1185-1197`) and the frame lands `Unmeasured`.

⇒ **`lighting_mismatch` already ships `target_pixels: -1` and `observable: null` today.** `m55` changes
nothing for it and must not pretend to. The brief's suggested regions — the attenuation sphere projected
to a screen bbox, or the census of lit primitives — are both real options, and both are a **new region
source**, not a new measurement: the first re-introduces a bounding box, which `G263` says carries no
verdict; the second needs a per-frame set of lit primitives that nothing computes today.

📌 **Filed, not designed, and deliberately out of `m55`'s scope.** It is also not urgent: `lighting_mismatch`
is not in `GAutoPool`, so it cannot reach a client dataset by the auto path. A future `m53` that adds a
flicker/lighting anomaly **must bring its own region source with it**, and that is the finding to carry
forward rather than a blocked item to re-discover.

### 2.4 Global class (`camera_clipping`, `time_dilation`)

No target, no tag, no mask; `bWholeFrameExtent` and `bbox_norm 0,0,1,1`. The whole-frame change fraction
is the only available statistic and **it has no control** — there is nothing outside the region. It ships
as a `run`-level reading. `observable` stays `null`, which is what it reads today, and the admit bias is
preserved by doing nothing.

### 2.5 LOD class

`lod_popping` is the only catalogue member with **repeated in-window transitions** (one per toggle), so
it is the one class where a per-frame change series is informative for its whole span rather than only at
its edges. It is therefore the best natural positive control for the per-frame field, and §10 uses it
that way. ⚠ At the compiled `LodMaxDistance` default of 200 cm this bench produces **zero**
`lod_popping` events (`m30`, and `080-06`'s `G-B` had to pass `IAI.Anomaly.LodMaxDistance 50000` to fire
at all) — so any `lod_popping` leg must declare that lever **in the predictions, before the leg**, or it
is vacuous (`G146`).

---

## §3 D3 — how `observable` changes

### 3.1 The finding that shapes the whole rule: a difference measures transitions, not states

`observable` is a **per-frame** field. A temporal difference between consecutive frames is a **transition**
measurement. For every steady-state class in the catalogue — which is all of them except `lod_popping` —
consecutive frames *inside* the hold are identical again once the anomaly has engaged, so
`change_frac_target` returns to ≈ 0 on every held frame after the first.

⇒ **`observable = current rule AND (this frame's change ≥ floor)` would mark the onset frame observable
and every subsequent held frame NOT observable.** It would empty `affected_frames` down to a single
frame for every texture- and hide-class event in the dataset. That is catastrophic and it is not what
the brief intends; the brief half-states it for hide class (*"after onset there is nothing to compare"*)
and the same is true for every other class.

**Therefore change evidence is an EVENT-level property, evaluated at the event's transitions, and
consumed per frame.** The per-frame `change_*` fields ship as measurements and gate nothing on their own.

### 3.2 No lookahead is available, and none is needed

`labels.jsonl` rows are emitted progressively from the drain
(`AnomalyCaptureSubsystem.cpp:3997-4048` computes `observable`, and the row is built and handed to the
async writer in the same pass). A row cannot be retro-edited, and `L3` already records that the veto does
not retro-edit it either. So a per-frame `observable` may not depend on a *later* frame's measurement.

It does not have to. **The onset is the first labelled frame, and the onset's change is measured on that
very frame** — the pair is (previous captured frame) → (this frame), and the previous frame is by
definition already past. So the evidence is available at the exact moment the first labelled row is
built. It is **latched onto the event's record** and every subsequent frame of that event reads the latch.

### 3.3 The proposed rule

```
per frame i of event e:

  change_measured(i)  = prev frame resolved && identity pinned && control below cap && denominator > 0
  change_evidence(e)  = PRESENT   once any of the first K labelled frames of e read
                                  change_frac_target >= floor(class)
                      = UNMEASURED if none of those frames was change_measured
                      = ABSENT     otherwise

  observable(i) = (m49 A1 rule)  AND  change_evidence(e) != ABSENT
```

with, unchanged from `m49` A1: `target_pixels == -1` ⇒ `observable` is `null` and the rule is not
reached at all.

Three properties, each deliberate:

1. **`UNMEASURED` admits.** Where the change could not be measured — no previous frame, a dropped frame,
   a resolution change, a control above the cap — the event keeps `m49`'s answer. This is `m26`'s admit
   bias and the 077 ruling in the same direction: *deleting a true positive is dataset loss*.
2. **The first `K` labelled frames are provisionally admitted.** Until the latch resolves, frames read
   the `m49` rule. `K` is a small predeclared constant (proposed **4**, matching the veto's per-event arm
   budget). This is what protects `stuck_low_mip`'s gradual hold and any class whose transition is
   spread. From frame `K+1` on, an event that never showed a change reads `observable: false`.
3. **`ABSENT` is the only state that removes anything**, and it removes an event's *observable frames*,
   not the event: an event with an empty observable set **stays in the file** (`m49` ruling, unchanged),
   ships `affected_frames` empty, and is visible to the client as such.

⚠ **The direction of the risk, stated rather than discovered.** `ABSENT` on a real anomaly is a true
positive losing its positive frames — the dangerous direction. Every default above is set so that the
only way to reach `ABSENT` is a *successful* measurement that found no change. That is why the floors of
§9 are derived conservatively (low) rather than tuned to reject the most events.

### 3.4 Schema

**Additive only. `annotation.json`'s root and per-event key sets DO NOT MOVE — `P6` is intact —
and `label_schema` stays `2`** (`m51` owns the bump to 3; two units must not both move it).

Per-row, inside an anomaly entry (the `stuck_mip.*` precedent: emitted only where meaningful):

| key | type | meaning |
|---|---|---|
| `change_measured` | bool | a sound previous-frame pair existed for this row |
| `change_px` | int | target-mask pixels differing by more than `tau_lo`; `-1` = unmeasured |
| `change_frac_target` | float \| null | `change_px / target_pixels` |
| `change_mean_target` | float \| null | mean per-pixel difference over the target's mask pixels |
| `change_frac_control` | float \| null | the same fraction over the frame outside the target |
| `change_px_hi` | int | the same count at `tau_hi`; `-1` = unmeasured |

Per-event (inside `labels.jsonl`'s event entry, **not** `annotation.json`):

| key | type | meaning |
|---|---|---|
| `change_evidence` | `"present"` \| `"absent"` \| `"unmeasured"` | the latched verdict |
| `change_frac_onset` | float \| null | the value at the first labelled frame |
| `change_frac_max` | float \| null | max over the event's labelled frames |
| `change_frac_median` | float \| null | median over the event's labelled frames |

`run_summary` (currently **86** keys) gains a small block: `change_frames_measured`,
`change_frames_unmeasured`, `change_events_present`, `change_events_absent`,
`change_events_unmeasured`, `change_refused_control`, `change_refused_identity`, `change_tau_lo`,
`change_tau_hi`, and the effective per-class floors with their provenance (`A48` — echo the **effective**
value and **where it came from**, never the value requested).

---

## §4 D4 — cost, and every way this can be wrong

### 4.1 A FULL COOK IS REQUIRED, and it is the largest single cost in this unit

Any GPU option here adds or changes a global shader parameter struct, and `G129` is unambiguous: a new
global shader cannot ride the code-only hot-swap, and a parameter-struct change is fatal against a stale
container (`m46`'s cook exists for exactly this). **This is true of the extend-the-existing-reduce option
as well as of the new-pass option — there is no GPU path that avoids it.**

Consequences to sequence deliberately, not discover:

- The container quintet (`67EA1FE0`/`2CEFB8F4`/`E03C6610` + `A16A18A8`/`C70ECDAA`) has been
  **byte-unchanged since `m49` Phase B**. A cook retires it, and with it every binary↔container pairing
  in `_binary_baselines\README.md`.
- `G121`: build identity is **exe hash + container identity**. Both halves move.
- `G92`: the archive step can wipe `Saved`. Bank first.
- `G119`/`G118`: read the cooked map set and the enforced config **back out of the artifact** afterwards.
- The cook must be its own sequenced operation, **never inside a measurement turn**.

⇒ **a reviewer may reasonably decide the cook is not affordable now.** §7.4's CPU fallback is costed for
exactly that case and is a real option, not a strawman.

### 4.2 GPU cost and memory

One additional compute dispatch per captured frame with a mask, `[numthreads(8,8,1)]` over the output
view rect — the same shape and domain as the existing `FAnomalyMaskReduceCS`, which the pacing gates have
never been able to resolve above noise. Per pixel: two texture loads, one `max`-of-channels, two
groupshared `InterlockedAdd`s on changed pixels plus one for the sum. Readback: **4 KB**
(256 tags × 4 uints), against the existing reduce's 6,144 B.

Retained memory: **one** frame of delivered colour at the view rect —
1280×720×4 B = **3.69 MB**, 1920×1080×4 B = **8.29 MB**. One texture, plugin-owned, released at
`FinishRun` and at teardown.

The declared budget is `<= +1 ms at 1080p`, measured as §10's `G-8` specifies — **pacing OFF**, because a
paced leg structurally cannot measure a sub-budget hook (`m35` finding, `G186`'s `A,B,B,A` order after a
declared discard). `G169` governs the reading: a difference not larger than the within-build spread is
**below the resolution of this instrument**, never *no cost*.

### 4.3 Identity: the previous colour must be the previous CAPTURED frame

This is the failure this project has met four times (`m21`, `m31`, `m44`, and `080-06`'s arm window) and
it is the one to design against first.

- The colour capture arms with `RequestId = ++CaptureRequestSerial` and stamps
  `Snap.SessionIndex = SessionFrameIndex` (`AnomalyCaptureSubsystem.cpp:4369-4372`), and the mask arm for
  the same frame is bound at the same tick by
  `TargetMaskArmedTick = GFrameCounter; TargetMaskArmedSessionIndex = Snap.SessionIndex` (`:4397-4398`),
  consumed at tick-end under `TargetMaskArmedTick == GFrameCounter` (`:891`). **Colour and mask for one
  session index are the same rendered frame**, and the mask result is already keyed to a session index
  through `TargetMaskPendingSessionIndex` (`:1249`).
- ⚠ **The mask pass does not run on every rendered frame** — the SVE is active only while arms are
  pending (`AnomalyMaskSceneViewExtension.cpp:49-53`). Retention must therefore be driven by the
  **captured-frame** cadence, not the arm cadence. The colour SVE's callback runs exactly on captured
  frames (`AnomalySceneViewExtension.cpp:107-110`, gated on `Entry.bWanted`), which is why §7 puts the
  retention there.
- **The rule: pin, do not tolerate.** The retained frame carries its `RequestId`; the subsystem resolves
  both request ids to session indices through `PendingSnapshots` and requires exactly
  `SessionIndex(prev) == SessionIndex(cur) - 1`. Anything else ⇒ `change_measured: false`. Frames *are*
  dropped in the wild — the extent clamp drops one (`AnomalySceneViewExtension.cpp:122-134`) and a
  key-ring miss drops one (`:96-105`) — and after a drop the indices skip and this check correctly
  refuses. **Refuse, never guess**, which is the key ring's own stated discipline.
- Resolution or letterbox change mid-run ⇒ retained extent differs ⇒ refuse and re-seed.
- `IAI.Capture.OutputHeight != 0` ⇒ `m43` already refuses the target mask entirely, so `m55` inherits the
  refusal and never has to reason about a resampled write.

### 4.4 The `m51` interaction, stated because it is a cross-branch fact

**`6713f51` is not an ancestor of `master`** (verified: `git merge-base --is-ancestor` exits 1), and
`bAwaitingTargetMask` / `TargetMaskWaitTicks` do not exist in `master`'s
`AnomalyCaptureSubsystem.cpp`. ⇒ **`m51`'s `F1` held-frame fix is NOT on `master`**, and `m55` built here
does not interact with it *as code*.

It does interact with it as a **design**, twice, and both need reconciling at whatever merge brings the
two together:

1. `m55` adds a **second readback per captured frame**. `m51`'s `R7` writer-byte-bound work is about
   exactly that budget. 4 KB per frame is small, but it is a new consumer of a bound that is still open
   on that branch.
2. `F1`'s subject is *waiting for a mask outcome that can exist*. `m55` adds a second outcome
   (the change table) that a frame can be waiting on. The same rule must hold for it: **wait only for an
   outcome that can exist**, and commit honestly unmeasured otherwise.

⛔ Neither is a blocker for `m55` on `master`. Both are recorded so the merge cannot silently orphan them.

### 4.5 Every route to `change_measured: false`

no previous captured frame (first frame of the run) · previous frame dropped or its session index is not
`N-1` · retained extent differs from this frame's · no target mask this frame (`target_pixels == -1`) ·
`Count[t] == 0` ⇒ no denominator · `change_frac_control` above its cap · the change readback was not
ready at the final bounded drain. **Every one of them admits** (§3.3 property 1).

### 4.6 Two measurement limitations, declared in advance

- **A moving target under-reads.** The change is accumulated over the **current** frame's mask, so pixels
  the target vacated between `N-1` and `N` are outside the region and are not counted. On the settled
  bench this is inert; on a moving-camera host it is real. `change_frac_control` and the existing
  whole-frame motion reading are what expose it, and the control cap is what refuses it.
- **The delivered picture is measured, so the delivered picture's own noise is measured too.** Temporal
  AA, dither and auto-exposure all move pixels with no anomaly present. That is precisely why the
  measurement ships with a paired control and why `tau_px` is derived rather than chosen (§9), and why
  §10's `G-1` (a null-effect anomaly must read at the control) is the load-bearing can-fail gate.

---

## §5 D5 — verifier interplay

The 079 tool stays **independent**: it differences delivered PNGs on disk over the delivered masks, and
`m55` changes nothing it reads. Its outcome vocabulary (`CONSISTENT` / `OFFSET-NOTE` / `NO-TRACE` /
`PARTIAL` / `UNASSESSABLE` / `READING`, `tools/verify_capture.py:479-483`) does not change and **no new
verdict is added**.

It gains **one reading line per event**:

```
CHANGE-EVIDENCE event=<id> producer onset=<change_frac_onset> mean=<change_mean> evidence=<present|absent|unmeasured>
                tool d=<measured d at the selected onset transition> tau=<tau>  AGREE | DIVERGE(<delta>)
```

Rules, each a consequence of something already ruled:

- Agreement **strengthens** a `CONSISTENT` line and is reported as such — it is two instruments, on two
  sides of the pipeline, measuring the same statistic (`change_mean_target` and the tool's `d` are the
  same quantity, §1.3) on the same buffers.
- Divergence is an **`OFFSET-NOTE`-class reading for a human**, never a failure and never a change of
  exit code. `NO-TRACE` remains the only failing outcome and its definition is untouched.
- ⛔ **The producer's numbers are never an input to the tool's own measurement.** 079 already states that
  *"M3 observable flags are producer evidence and are not independently verified by this tool"*; the same
  applies here. If the tool ever used `change_*` to choose a transition, the cross-check would become
  circular and the tool would stop being a second opinion.
- ⛔ And per `G263`'s correction, **agreement does not establish cause.** Two instruments agreeing that
  the target's pixels changed at frame `k` is still not evidence that the injection is what changed them.

📌 **The most valuable single use of this line is on the `G270` Lyra events.** Those read `NO-TRACE` from
the tool at ratio 26–31. If the producer independently reads `change_frac_onset ≈ 0` on the same events,
`m55` has closed the gap the ratio could not see — measured from the inside, on the exact class that
motivated the unit. That is §10's `G-3`.

---

## §6 Field list — one table

*(consolidated in §3.4; nothing here is in `annotation.json` and `P6` does not move.)*

---

## §7 The mechanism, and what was rejected

### 7.1 Chosen: a new change-reduce pass at the colour hook, consuming the mask published at the Tonemap hook

```
Tonemap  callback (mask SVE, existing):
    ... existing mask PS + reduce CS, byte-unchanged ...
    publish { ConvertToExternalTexture(MaskRT), View.Family->FrameNumber } into a shared holder

VisualizeDepthOfField callback (colour SVE, existing, runs only on captured frames):
    OwnTexture = existing W*H sub-rect copy at (0,0)          <- already built, AnomalySceneViewExtension.cpp:139-145
    if holder.FrameNumber == View.Family->FrameNumber
       and Prev.PooledRT valid and Prev.Extent == this extent:
        Mask = RegisterExternalTexture(holder.MaskRT)
        Prev = RegisterExternalTexture(Prev.PooledRT)
        dispatch FAnomalyChangeReduceCS(Mask, OwnTexture, Prev) -> 256x4 uint table
        AddEnqueueCopyPass(table readback)
    Prev = { ConvertToExternalTexture(OwnTexture), Entry.RequestId, extent }   <- retention, no extra copy
```

Why this shape:

- **It measures the delivered pixels.** `OwnTexture` *is* what becomes the PNG. Measuring post-tonemap
  pre-FXAA instead would measure a picture the client never receives, and would make §5's cross-check a
  comparison of two different images.
- **Retention is free.** `OwnTexture` already exists for the readback; extracting it costs one refcount,
  not a copy.
- **The cadence is already exactly right.** That callback runs on captured frames and no others
  (`Entry.bWanted`), so no SVE's activation pattern changes and nothing in the heavily-gated mask arm
  accounting is touched.
- **Cross-frame state is pooled-RT based** (`TRefCountPtr<IPooledRenderTarget>`, refcounted and
  frame-independent) — never a raw `FRDGTextureRef`, which is allocated from the graph's own allocator
  and would dangle across frames. The within-frame mask handoff is guarded by
  `View.Family->FrameNumber` equality — identity, not adjacency, the same discipline as the key ring.
- **The existing reduce is not touched at all**, so `MASK-TIE`, `TARGET-PIXELS TIE`, `target_pixels`,
  `target_drawn_pixels`, `bbox_drawn_px`, ONSET and pairing are unchanged **by construction** rather than
  by measurement. That is the `m49` Phase B lesson (*"indices 0..4 DID NOT MOVE"*) taken one step
  further.
- The new shader must **not** skip `V == 0` the way `AnomalyMaskReduce.usf:38` does — the whole-frame
  control needs the untagged pixels. That structural difference is a second reason not to fold it into
  the existing shader.

`ConvertToExternalTexture` (`RenderGraphBuilder.h:273`) is the correct call and returns the pooled target
immediately at construction time; `QueueTextureExtraction` (`:259`) completes only after execution and is
too late for same-frame use. `TryRegisterExternalTexture` (`RenderGraphUtils.h:263`) returns null rather
than asserting, which is what the first-frame case needs.

### 7.2 Rejected — extend the existing `FAnomalyMaskReduceCS` at the Tonemap hook

Brief's option (a). Rejected on three counts: it measures **pre-FXAA**, i.e. not the delivered picture;
retention at that hook requires the mask SVE to be **active on every captured frame**, which changes when
the post-process chain designates that callback its final writer (`overrideOutput`) and is therefore a
pixel-affecting change needing the `m45` identity arbiter to clear it; and it would have to abandon the
`V != 0` skip that the existing shader's structure depends on. It saves one dispatch and costs a
correctness margin. **It does not save the cook.**

### 7.3 Rejected — `FViewInfo::PrevViewInfo` / the TAA history as "the previous frame"

The TAA history is pre-tonemap, in history space, reprojected, and does not exist at all with AA off —
which is the arbiter configuration this project measures pixels in. It is not the previous delivered
frame under any configuration.

### 7.4 Costed fallback, not rejected — CPU differencing of the two delivered crops

`m48` already reads colour on the CPU from `Frame.RawBytes` in the drain
(`AnomalyCaptureSubsystem.cpp:4061-4063`), and the per-frame mask **is** on the CPU as well
(`Result.MaskPixels`). So a CPU implementation is genuinely available and it is the only option that
**needs no cook**.

Its costs, stated fairly: a ~1 MP scan per captured frame on the **game thread** inside the drain, which
is the shape of the render-thread CPU scan `m34` removed; retention of the previous frame's full
`RawBytes` in system memory, competing with the writer's byte budget (`m51` `R7`); and no access to
anything the GPU pass gets for free. Against that: no shader, no parameter struct, no container churn,
and every binary↔container pairing in `_binary_baselines` survives.

⇒ **If a reviewer rules the cook unaffordable this cycle, this is the build, with the same fields, the
same per-class rules, the same floors and the same gates.** The measurement's semantics do not depend on
where it runs. §13 Q2 asks for that ruling explicitly.

---

## §8 Floor derivation — the procedure, not the numbers

⛔ **No floor and no `tau` is chosen in this document, and none may be chosen from a leg's outcome.**

1. `tau_lo` / `tau_hi` come from the **noise floor of a NEGATIVE frame pair**: consecutive captured frames
   from a leg with **no anomaly firing**, on both fixtures, at the delivered configuration and at the
   AA-off arbiter. `tau_lo` sits above the highest per-pixel difference those pairs produce; `tau_hi` is
   set an order above it. Two-sided, the `m48` method: every quiet pair below it, every known-anomalous
   pair above it, with both margins reported.
2. The **class floors** come from banked legs whose visibility is already known from the outside:
   the six pinned bench legs and the three masked Lyra sessions the verifier scores, plus `080-04`'s
   Lyra `CO8` leg — the one carrying the `G270` events at ratio 26–31 that read `NO-TRACE`.
   A floor is admissible only if it separates *known-visible* from *known-invisible* on that set **with
   the band between them reported**, and it is set on the **conservative side** of the band, because
   `ABSENT` is the dataset-losing direction (§3.3).
3. **The known answer must be injected, not read from the artifact under test.** `G260`'s corollary: an
   onset read from the delivered masks is circular, because `m49` A1 *defines* `affected_frames` as the
   observable subset. The anchors here are the verifier's independent `NO-TRACE`/`CONSISTENT` readings
   and the client-side visibility judgements, not the producer's own labels.
4. The two-threshold accumulation of §1.3 means steps 1–2 are **offline sweeps over banked legs**, not a
   re-run per candidate value.
5. ⚠ **Derivation travels; the number may not** (`m48`'s standing caveat). The floors are compiled
   defaults with console overrides and an `A48` provenance echo, and the derivation is written down so
   another title can repeat it.

**All floors and `tau` values are predeclared in an amendment to this file BEFORE the implementation
gate, and are not touched afterwards on the strength of a reading.**

---

## §9 Gates

Every gate runs in **both tick orders** (native and `IAI.Bench.SynthTickOrder`) unless noted, and every
pixel-identity reading is taken at the AA-off arbiter (`G228`/`G230`).

| # | gate | passes when |
|---|---|---|
| `G-0` | both build targets, incl. the **modular editor** target (the only one that catches a missing `MODULE_API`) | exit 0, **zero warnings**, twice |
| `G-1` | **CAN-FAIL, load-bearing.** A control anomaly with a null visual effect — `IAI.Bench.StuckMipNoHold 1`, whose `held:false` on 51/51 is already proven — fires with the mask armed | `change_frac_target` reads **at the control**, not above it; `change_evidence: absent`; and the same leg with the lever off reads `present` |
| `G-2` | texture class on **both fixtures**, shipped configuration | `corrupted_texture` and `missing_texture` read `change_evidence: present` with `change_frac_onset` well clear of the floor; `observable` sets unchanged from the A-side |
| `G-3` | **the `G270` class.** `080-04`'s banked Lyra `CO8` leg recipe, `MaxCoAffected 8` | the three ratio-26–31 events read **low** `change_frac_onset` and `change_evidence: absent`, agreeing with the verifier's independent `NO-TRACE` on the same events |
| `G-4` | hide class | onset frame reads a large `change_frac_target`; every later held frame reads `target_drawn_pixels == 0`; `observable` unchanged from `m49` |
| `G-5` | `stuck_low_mip` deferred onset | the first held frame (the `080-06` window start) is `change_measured` and its pair straddles unheld→held; if the transition is spread, the first-`K` latch resolves `present` within `K` |
| `G-6` | `lod_popping` per-frame series (declare `IAI.Anomaly.LodMaxDistance 50000` **in advance**) | repeated in-window transitions appear in the per-frame `change_frac_target` series at the toggle cadence |
| `G-7` | **identity.** `IAI.Bench.ChangeForceStale 1` forces the retained frame to be one index older | `change_measured:false` on every row, `change_refused_identity` non-zero, **no row silently measured against the wrong frame** |
| `G-8` | **cost.** Pacing **OFF**, `A,B,B,A` after a declared discard, 1280×720 and 1920×1080 | ms per captured frame and per engine frame reported both ways; `<= +1 ms` at 1080p, or reported as **below the resolution of this instrument** (`G169`) if inside the within-build spread |
| `G-9` | **schema / `P6`.** A leg with no change-bearing event and one with | `annotation.json` root **4 → 4** and per-event **16 → 16**; `label_schema` **2**; `labels.jsonl` row keys unchanged; anomaly keys grow by exactly the declared `change_*` set and only inside an eligible entry; `run_summary` **86 → 86 + exactly the declared block** |
| `G-10` | **A-side identity.** Cross-binary difference set vs a same-binary control pair (`080-06`'s `G-A`/`G-B` method) | `EXTRAS 0` |
| `G-11` | **light and global classes are untouched** | `lighting_mismatch`, `camera_clipping`, `time_dilation` read `observable: null` exactly as on the A-side |
| `G-12` | verifier interplay | the `CHANGE-EVIDENCE` line prints on every event; exit codes unchanged on all six CLI cases; selftest and contracts still green; **no new verdict** |
| `G-13` | `A44` on the staged artifact, both encodings | new symbols present, **pre-existing controls non-zero** (sound, not blind) and invented symbols absent (discriminating) |
| `G-14` | post-cook | `verify_cooked_maps.ps1` passes; the cook log names `FAnomalyChangeReduceCS`; **and the build boots** (`G129`/`m46`: the boot is the evidence, not a string scan) |

`G-1` is the one that decides whether anything else is worth reading: without it, a `change_evidence:
absent` anywhere is indistinguishable from an instrument that cannot fire (`G96`).

---

## §10 File-by-file

**New**

- `Shaders/Private/AnomalyChangeReduce.usf` — the change-reduce CS. Loads mask + current + previous,
  accumulates per value (**including 0**) `{ChangeLo, ChangeHi, SumAbsDiff255}` into a groupshared table,
  one global atomic merge per group per present value. Overflow checked: 1920×1080 × 255 = 5.29e8,
  inside `uint32`.
- `Source/AnomalyShaders/{Public,Private}/AnomalyChangeReduceShader.{h,cpp}` — `FAnomalyChangeReduceCS`
  declaration and `IMPLEMENT_GLOBAL_SHADER`, alongside the two existing ones. ⚠ The module is
  `PostConfigInit` for the reason `G131` records; nothing about that changes.
- `Source/AnomalyCapture/Private/AnomalyChangeTypes.h` — `FAnomalyChangeTagResult`,
  `FAnomalyChangeResult` (mirroring `AnomalyMaskTypes.h`), and the `EAnomalyChangeEvidence` tri-state.

**Changed**

- `AnomalyMaskSceneViewExtension.{h,cpp}` — publish `{pooled MaskRT, family frame number}` into a shared
  holder at the end of the Tonemap callback. **No change to the mask PS, the reduce CS, the table
  stride, the readback, or `FAnomalyMaskResult`.**
- `AnomalySceneViewExtension.cpp` — the block of §7.1 inside the existing `AfterPass_RenderThread`, after
  `OwnTexture` is built and its readback enqueued.
- `AnomalySveCapturer.{h,cpp}` — carry the change-table readback beside the colour readback in `FInFlight`
  and surface the decoded table through the existing drain, keyed by the same `RequestId`. This is where
  the retained-frame holder lives and where it is released on `Reset()`.
- `AnomalyCaptureSubsystem.{h,cpp}` — join the change table to the frame's mask outcome at the session
  index; the identity check of §4.3; the per-event latch of §3.3; the extended `observable` rule at
  `:4016`; the counters; the `A48` provenance echo; `ObservableMinPixels`' sibling console knobs for
  `tau` and the floors, **all mid-run guarded**.
- `AnomalyLabelWriter.{h,cpp}` — emit the per-row and per-event keys of §3.4 and the `run_summary` block.
  ⛔ `annotation.json` untouched.
- `tools/verify_capture.py` — the single `CHANGE-EVIDENCE` reading line of §5, plus its selftest fixtures
  and a contract pinning that it **cannot** change an exit code.
- `docs/client-readme.md` §8, `docs/client-delivery.md`, `docs/PRE-DELIVERY-CHECKLIST.md`,
  `docs/architecture.md`, `docs/gotchas.md`.

**Untouched by construction:** `IAnomaly.h` · every anomaly · the selector, auto-injector and census ·
`AnomalyMaskReduce.usf` · `AnomalyVisibleMask.usf` · `AnomalyMaskMeasure.*` (the `m26` veto) ·
`annotation.json`'s shape.

Estimated size: ~450 lines of C++/HLSL plus the gate campaign and the cook.

---

## §11 Risks

1. **The cook.** §4.1. The single largest cost and the one a reviewer may refuse. Fallback at §7.4.
2. **`ABSENT` on a real anomaly** — a true positive losing its positive frames. Mitigated by the admit
   bias, the first-`K` latch, and conservative floor derivation; exposed by `G-2` and `G-4`.
3. **Delivered-picture noise** (TAA, dither, auto-exposure) inflating `change_frac_target` and making
   `absent` unreachable — the *opposite* failure, and the one that makes the whole unit vacuous. `G-1` is
   the gate that would catch it; a `change_evidence` that is `present` on a null-effect control means the
   metric is measuring the renderer, not the anomaly.
4. **Moving targets and moving cameras** under-read (§4.6). Both bench fixtures are settled; Lyra is not,
   which is why `G-3` runs there.
5. **Two instruments, one number.** §5's cross-check is valuable exactly because the tool is independent.
   Any future change that lets the producer's numbers steer the tool destroys it silently.
6. **`m51` merge surface** (§4.4).
7. **A new evidence field invites a new veto.** It must not become one: the 077 ruling removed a veto
   built on `target_drawn_pixels` for precisely this reason, and `G258` is the standing warning.
   `change_*` is a measurement and an `observable` input; it is **never** an input to the `m26` veto.

---

## §12 Questions for chat — none of these is decided here

- **Q1 — the subtraction (§1.5).** The brief asks for `change_frac_target - change_frac_control` to be
  what the label carries; `G260` built that, gated it and refuted it in both directions. This plan
  publishes both fields and subtracts neither, using the control as a refusal input per `G260`'s own
  stated rule. **Confirm, or overrule with the reason recorded.** Either way both fields ship, so the
  choice is reversible in a consumer without a re-capture.
- **Q2 — the cook (§4.1).** Every GPU route needs a full cook and retires a container that has been
  byte-unchanged since `m49` Phase B. Ruling needed: **GPU + cook now**, or **the CPU build of §7.4**
  with identical semantics and no cook.
- **Q3 — does `observable` actually change in `m55`?** §3.3 proposes it does, gated on an event-level
  latch. The conservative alternative is that `m55` ships the measurements and the per-event
  `change_evidence` verdict while `observable` keeps the `m49` A1 rule verbatim, and a follow-up flips it
  once the floors have been derived from real distributions. The second is slower and strictly safer.
- **Q4 — `K`, the provisional-admission window (§3.3).** Proposed 4, matching the veto's arm budget.
- **Q5 — the light class (§2.3).** `lighting_mismatch` has no mask and no region and this plan
  deliberately does not invent one. Confirm that a future `m53` is expected to bring its own region
  source rather than `m55` growing one.
- **Q6 — `stuck_low_mip`'s default-on decision.** `080-05` parked it *pending `m55`*. This plan does not
  flip `GAutoPoolDefaultEnabled` and does not assume the answer; `G-3` produces the reading that decides
  it.
