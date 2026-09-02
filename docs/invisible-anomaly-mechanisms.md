# Invisible-anomaly mechanisms — the ledger

**One symptom, several causes.** *"A labelled anomaly that cannot be seen in the delivered frames"* has
been produced by more than one mechanism, and they are **DISTINCT**, with **potentially DISTINCT
CURES**.

> 🚨 **NO SINGLE FIX IS KNOWN TO ADDRESS ALL OF THEM.** `feature/stencil-capture`'s premise — report
> actual pixel contribution before hiding — is the cure for **H4**. It has **not** been shown to cure
> **H5**, and H5's two classes may not share a cure with each other. **Do not treat any one branch as
> "the fix for invisible anomalies."**

Each row states whether it is **MEASURED** or **SOURCE-READ**, because that distinction has repeatedly
been the difference between a finding and a foreclosure (**G120**).

---

## 1. `m23` / `P3` — hide-type identity taken from the sampling OUTCOME

| | |
|---|---|
| **status** | ✅ **FIXED, SHIPPED** (`m23`) |
| **what it is** | The positive frame set was derived from *whether any frame sampled the target hidden*, so an event that never manifested could still be labelled positive on every on-screen frame. |
| **evidence** | **MEASURED.** `IsHideTypeAnomaly` is now an explicit table; a non-manifesting hide writes `manifested:false` and **zero** positive frames, and logs a warning naming the count it used to emit. |
| **pointer** | journals 031–034, the `m23` tag |
| **cure** | shipped |

## 2. `H4` — the target WOULD draw, but is BLOCKED

| | |
|---|---|
| **status** | ✅ **SUPPORTED (path b), MECHANISM ONLY** |
| **what it is** | `ProjectActorBoundsToScreenRect` runs **no occlusion trace**, while selection (`IsComponentRenderableVisibleInternal`) does. A target on-screen but fully occluded projects successfully and is labelled positive while contributing no pixels. |
| **evidence** | **MEASURED.** `bbox_valid` on 59/59 rows and provenance `valid:false` + `0/0` at all 8 anchors — the **C4 divergence**, never seen in 780 prior banked records. Raw in-bbox series ≤ **2.0×10⁻⁴** against a control hide's **0.1023–0.1116**. |
| **pointer** | journal 045 PART TWO |
| ⛔ **limits** | **NOT CONFIRMED** — the A54 leg of the original signature is unobtainable (**G117**). **NO INCIDENCE CLAIM** — path (b) only; path (a) is **PARKED, NOT REFUTED**. **n = 1 leg.** |
| **cure** | **`feature/stencil-capture`** — its premise is exactly this. ⛔ **UNTOUCHED, NOT REBASED.** |

## 3. `H5` class (ii) — AGGREGATE BOUNDS, and every guard collapses together

| | |
|---|---|
| **status** | ✅ **SUPPORTED, REPRODUCED HERE — and 🆕 MEASURED BY THE CURE ITSELF ON BOTH INSTANCES (2026-08-20, journal PART TWENTY-EIGHT §195).** `BP_SplineSpawn_C`: label claims **22.89 %** of frame, mask measures **ZERO surviving pixels**, 8 of 8 events. `InstancedFoliageActor_0_0_0`: label claims **100 % / `bbox_px (0,0,1280,720)`**, mask measures **0.62–1.45 %** — it draws **~1.4 % of what it claims**. **Both with every integrity bucket clean** (`framesContributed = arms`; discarded/residual/unconfirmed/noPass/probe/collisions/tagFailed all 0; 29/29 armed frames view-sized). 🆕 **AND SINCE SLICE 2 (PART TWENTY-NINE) THE MEASUREMENT REACHES THE ARTIFACT:** `annotation.json`'s already-shipping `mask{provided}` carries the tri-state's bool — **`NOT_MEASURED` → `false` (never measured, MUST BE ADMITTED); `MEASURED_ZERO` and `MEASURED_NONZERO` → `true`** — derived from `State` alone by one function that never sees a magnitude, so the two zeros cannot collapse. Verified in both directions on banked known answers (`SM_Ramp2` → `false`; `BP_SplineSpawn_C`'s measured zero → `true`). 🆕 **AND SINCE SLICE 3 (PART THIRTY-ONE, `65deadc`) THE CURE ACTS:** an event is **removed from `annotation.json` IF AND ONLY IF it manifested AND its target was MEASURED at ZERO drawn pixels** — counted in `run_summary.vetoed_events`. **`NOT_MEASURED` is never vetoed; a measured NON-ZERO count is never vetoed however small a fraction of its claimed extent it is. NO RATIO, NO THRESHOLD** (reasoning recorded verbatim, journal §209). Gated: `BP_SplineSpawn_C` 8/8 vetoed · the foliage at ~1.4 % of its claim **NOT** vetoed · `SM_Ramp2`'s `NOT_MEASURED` events **all kept**. 🚨 **THE ACCEPTED COST: `m26` is a PARTIAL cure for `H5` — it removes the zero-contribution case and LEAVES THE OVER-CLAIM CASE** (the foliage ships as a valid label). ⛔ **NO INCIDENCE CLAIM.** |
| **what it is** | Selection requires a renderable component — **a TYPE test, not a DRAWING test**. `UInstancedStaticMeshComponent` derives from `UStaticMeshComponent`, so aggregates pass trivially while their `Bounds` cover the whole cluster. The label describes the container; the pixels are a small subset of it. |
| **evidence** | **MEASURED.** `InstancedFoliageActor_0_0_0`: `bbox_px (0,0,1280,720)` on 59/59 rows — **the entire frame** — `coverage_ratio 1.0`, `coverage_pct 100`, `manifested true`, while whole-frame change was **~0.0069** and an 8×8 grid put it in **4 of 64 cells**. |
| **pointer** | journal 045 PART NINE · **G124** |
| 🚨 **generalises** | **MEASURED and NOT foliage-specific.** **3 of 13** non-foliage selectable actors carry a **NEGATIVE `poll_distance`** — `BP_SpawnPad_C` **−114.8** (a **plain `StaticMeshComponent`**), `BP_SplineSpawn_C` **−19405.5**, `RoomBuilderSquare_C` **−1737.8**. A negative value means the bounds sphere already contains the poll origin, so **the 1800 cm cull can never fire from anywhere in the level**. |
| ⛔ **limits** | **MECHANISM ONLY, NO INCIDENCE CLAIM.** One instance measured end-to-end; the owner's `InstancedMeshActor` is **not** this actor. |
| **cure direction** | 🆕 **`C-1` (surviving-pixel count via a mask) — OWNER-RULED 2026-08-19.** ⛔ **NOT A REVIVAL OF `feature/stencil-capture`: mine it, do not resume it.** Its blacklist must not survive; its `USkeletalMeshComponent` narrowing manufactures the defect it detects; its reduction is unpriced. 🚨 **MEASURED + SOURCE-READ 2026-08-20 (journal PART TWENTY-SIX, `G133`/`G134`): C-1 is STRUCTURALLY BLIND TO NANITE GEOMETRY on UE 5.1** — `Nanite::FSceneProxy::GetViewRelevance` never sets `bRenderCustomDepth` and the 5.1 custom-depth pass has no Nanite path, so a Nanite target is selectable, taggable, verifiable **and permanently unmeasurable**. ✅ **The false-zero half is CLOSED (PART TWENTY-SEVEN, `3beb3ba`): the extent precondition means such a target lands `NOT_MEASURED` ⇒ ADMITTED, never vetoed.** ✅ 🎯 **AND THE SCOPE IS SETTLED: both `H5` instances are NON-NANITE** (`SM_Bush`, `SM_GenericPlane`), so **the cure reaches the cases that motivated it.** ⚠ **But StackOBot's authored structural geometry is overwhelmingly Nanite — the limit is the COMMON CASE on this very title, measured not projected — so `m26` neither detects nor mitigates `H5` on Nanite geometry.** |
| **cure SHAPE** | 🆕 **(c) DEFERRED VETO, WITH (b)'s REPORTING — OWNER-RULED 2026-08-19.** Measure during the window · record in the already-shipping `mask{}` slot · **invalidate the event in the in-memory accumulator before `FinishRun` writes `annotation.json`.** ⛔ **Shape (a) pre-flight veto is REJECTED PERMANENTLY** on two independent blockers: **selection→fire is ZERO frames**, and a re-picking veto **destroys the seeded draw protocol** and breaks `m22`'s shipped byte-identical gate. **Accepted limits `L1`–`L3` — journal PART FOURTEEN §104.1.** **NOT IMPLEMENTED.** |
| **cure PLAN** | 🆕 **`m26`, PLANNED NOT APPROVED, NOTHING IMPLEMENTED — journal PART FIFTEEN §110-§118.** File by file, gates with thresholds, risk. 🚨 **The negative branch is a SHIP GATE, not a test case:** it must FIRE on `InstancedFoliageActor`/`BP_SplineSpawn_C`, **NOT over-fire on `StaticMeshActor_49` and `SM_Ramp2`**, ADMIT when blind, and be LOUD when blind. 🚨 **Riskiest item is `P-2`:** a hide-type target is hidden during the positives, so **a naive measurement reads zero and would invalidate EVERY hide-type event** — survivable only because *no qualifying frame* lands in `NOT_MEASURED`, never `MEASURED_ZERO`. |
| **the ONE definition** | 🆕 **`AnomalyViewport::IsRenderableComponent`** — the **already-locked** `G33`/`P6` ruling, not a new choice. **Masking can adopt it TODAY** (public `ANOMALYINJECTOR_API`; `AnomalyCapture` already depends on `AnomalyInjector`), which alone removes the `UPoseableMeshComponent` narrowing. ⚠ **Labelling CANNOT adopt it today — that is the locked-but-unimplemented `P6` bounds ruling, and `P6` DOES NOT MOVE.** 🚨 **`mask.provided` false/true carries *never measured* vs *measured* — so a cure can be forbidden to invalidate what it never measured, with NO new field.** |
| **n** | **2 measured instances** — `InstancedFoliageActor_0_0_0` and **`BP_SplineSpawn_C`** (22.9 % of frame claimed, peak in-bbox change **0.0175** against the control's **0.5515**). ⚠ **`BP_SplineSpawn_C` was excluded from an earlier candidate list on the inference "it clearly draws", from its rect size — which is CLAIMED extent, the very quantity H5 says is untrustworthy. Measurement overturned it.** |
| **cure** | ⛔ **UNKNOWN, and NOT designed.** ⚠ **A CLASS BLACKLIST IS NOT A FIX** — measured, it breaks `RoomBuilderSquare_C`, a legitimate target with a 75:1 in/out change ratio. |

### 3.1 CURE MEASUREMENT — what a threshold can and cannot do (journal 045 PART ELEVEN)

**7 targets measured marker-off; 5 GOOD, 2 BAD. n is small and stated as small.**

⛔ **THE SELECTION BOUNDS EXTENT IS NOT RECORDED IN ANY ARTIFACT.** The guards use SM/SK
`Component->Bounds`; `node.bounds` is the whole actor (`P6`); `poll_distance` is one equation in two
unknowns. **The quantity every guard is computed from cannot be read back.**

**The only clean separator is POST-hide** — change per unit claimed area, BAD {0.00603, 0.00838} vs
GOOD {0.0303 … 0.498}, a 3.6× gap with no overlap. ⛔ **A cure must decide BEFORE hiding, so this is
not usable as a rule.**

**Among PRE-hide fields: `cov_pct` and rect % DO NOT SEPARATE AT ALL** — BAD holds both the highest
value (100.00) and nearly the lowest (3.86). ⇒ 🚨 **THE CURE NEEDS A NEW MEASUREMENT, NOT A NEW
THRESHOLD.**

**What naive rules cost, measured:**

| rule | BAD caught | GOOD broken |
|---|---|---|
| `poll_distance < 0` | 2/2 | **2/5** — `RoomBuilderSquare_C`, `BP_SpawnPad_C` |
| `cov_pct > 90` | 1/2 | 0/5 *(misses `BP_SplineSpawn_C`)* |
| `cov_pct > 3` | 2/2 | **4/5** |
| blacklist ISM/HISM/Foliage | 2/2 | **1/5** — `RoomBuilderSquare_C` |
| bounds-volume ÷ rect | — | ⛔ **not computable** |

⚠ **One PRE-hide quantity did separate in this sample — the occlusion sample count (BAD ≤ 3/9,
GOOD ≥ 5/9).** n=2 vs n=5 on a 9-valued integer, two GOOD targets one step from the boundary, and the
mechanism offered for it is post-hoc. ⛔ **An observation, NOT a proposed threshold.**

### 3.2 ⛔ RULING — `SM_Ramp2` IS AN A35 CASE AND IT CONSTRAINS ANY FUTURE CURE

**Measured, marker-off, on a LEGITIMATE target:** `SM_Ramp2` peak change **OUT** of its own bbox
**0.2955** exceeds peak change **IN** **0.1785**. **The largest visual effect of hiding it is outside
the rect it claims.**

🚨 **THIS IS A CONSTRAINT ON THE CURE, NOT A CURIOSITY.** A cure that measures drawn contribution
**only inside the claimed rect** scores this target low and would reject it. Shadows are the named
instance; A35 covers the general case. **Any candidate whose measurement is bbox-scoped inherits this
failure mode and must state it.**

### 3.3 ⛔ RULING — THE `H4`/`H5` SHARED-CURE QUESTION IS A HYPOTHESIS, NOT A FINDING

Recorded verbatim, so the resemblance is never later treated as established:

> **"`H4`'s cure (report actual pixel contribution before hiding) and `H5`'s required new measurement
> RESEMBLE each other. Whether ONE measurement serves BOTH is UNESTABLISHED. It is attractive
> precisely because it would collapse two mechanisms into one fix, and that is the reason to test it
> rather than assume it. `H4` is a target that WOULD draw and is BLOCKED; `H5` is a target that
> contributes far less than it claims. A measurement answering 'is this target currently occluded' is
> not obviously the same as one answering 'how much of what this label claims is actually drawn'."**

⛔ **`feature/stencil-capture` STAYS UNTOUCHED AND UNREBASED.**

## 4. `H5` class (i) — the target WOULD NOT DRAW ANYWAY

| | |
|---|---|
| **status** | ⚠ **ENUMERATED, NOT OBSERVED** |
| **what it is** | A mesh component that passes the filter while drawing nothing: `bRenderInMainPass=false`, a null `StaticMesh`, a material that renders nothing, an owner-hidden actor (`IsVisible()` does **not** consult the owner's `bHidden`), or an SMC carried as an editor gizmo. |
| **evidence** | **SOURCE-READ ONLY.** The filter checks exactly two things — `Component->IsVisible()` and, for ISM, `GetInstanceCount() > 0`. **ABSENT:** owner `bHidden`, `bRenderInMainPass`, `GetStaticMesh() != nullptr`, section count, material presence, `WasRecentlyRendered()`. Owner-observed instance: **`BP_LocalVolumetricFog`** — **not reproducible here**, the client runs her own game. |
| **pointer** | journal 045 PART NINE §64, PART TEN |
| **cure** | ⛔ **UNKNOWN, and NOT designed.** |

## 5. TRACEABILITY DEGRADATION — ⚠ **NOT A CAUSE. This is what prevents ATTRIBUTING the others.**

> 🚨 **DO NOT FILE THIS AS A FOURTH CAUSE.** It produces no invisible anomalies. It is why the
> remaining invisible cases in the client's delivered data **cannot be assigned to a mechanism.**

| | |
|---|---|
| **status** | ⚠ **OWNER-OBSERVED; CHARACTERISED HERE** |
| **what it is** | The owner reports that in **builds**, delivered nodes read `StaticMeshActor_xxx` for most objects, while the same inspection in the **editor** shows proper names. |
| **evidence** | **MEASURED (banked sweep, 1267 node entries / 109 legs) — and it REFINES the observation.** `asset_name` and `component_class` are populated on **15 of 15** distinct node identities. **They do NOT degrade in builds.** What degrades is **`node.name`**, which is `AActor::GetName()` — the internal object name — and is **identical in editor and build**. The editor *displays* `GetActorLabel()`, which **does not exist in a cooked build** (measured: `IAI.ListActors` printed `(no-label)` for all 432 MainWorld actors). |
| **pointer** | journal 045 PART TEN, PART ELEVEN |
| **cure** | ⛔ **NOT designed.** Any change to how these fields are populated **touches the `annotation.json` contract ⇒ `P6` TERRITORY / MILESTONE CANDIDATE.** ⛔ **`P6` DOES NOT MOVE.** |

### 5.1 THE CLIENT-FACING ANSWER — recorded verbatim

> **Attribution of an invisible anomaly in a delivered session: PARTIALLY POSSIBLE, TODAY, WITH NO
> CHANGES. `asset_name` and `component_class` ARE populated in packaged builds (15/15 across 109
> banked legs, 1,267 node entries, zero empty) and identify the culprit CLASS. `node.name` is
> `AActor::GetName()`, the internal object name, NOT the editor label — a script-spawned actor reads
> `StaticMeshActor_<n>` in both editor and build, and the editor's friendlier display comes from
> `set_actor_label()`, which does not exist in a packaged build (G91). This is NOT a
> field-population defect. CANNOT be done today: distinguishing two instances of the same class, or
> determining which selection predicate admitted an actor."**

### 5.2 ⚠ `P6`'s THIRD OBSERVATION — and it NARROWS the answer above

**`ResolveNodeIdentity` takes the FIRST *VISIBLE* mesh component**, so the identity fields are
**NON-DETERMINISTIC for any actor that toggles component visibility at runtime.**

**Measured on the same actor, `BP_SpawnPad_C_UAID_B42E9936F54253D500_1492231360`:**

| leg | `asset_name` | `component_class` |
|---|---|---|
| `MW_MW_Q3_PAD` | **`SM_SpawnPad_Base`** | `StaticMeshComponent` |
| `H5i_H5i_SPAWNPAD` | **`Plane`** | `StaticMeshComponent` |

The Blueprint contains `SetVisibility` / `bVisible`, so which component is visible at the anchor
frame varies between runs, and `ResolveNodeIdentity` reports whichever it reaches first.

🚨 **CONSEQUENCE, and it must be read next to §5.1: a delivered session can name THE SAME ACTOR TWO
DIFFERENT WAYS. `asset_name` identifies a CLASS reliably and an INSTANCE unreliably.** That narrows
*"partially possible"* further: an analyst grouping delivered cases by `asset_name` will split one
actor across two groups without any indication that they are the same object.

⛔ **`P6` DOES NOT MOVE. Not fixed.**

---

## 6. CURE OPTIONS — the costed space (journal 045 PART TWELVE)

⛔ **SOURCE READING AND COSTING ONLY. NOTHING IMPLEMENTED, NOTHING PROTOTYPED, NO CANDIDATE PICKED —
that is the owner's call and it is the next brief.**

### 6.0 Two architectural facts that price every candidate

**(1) THE FILTER AND THE PIXELS LIVE IN DIFFERENT MODULES, AND ONE OF THEM DOES NOT EXIST IN SHIPPING.**
`IsRenderableComponent`, `IsUnoccluded`, the selector and every anomaly live in **`AnomalyInjector`**,
whose deps are `Core`/`CoreUObject`/`Engine`/`InputCore` and which builds in **every** configuration.
Every pixel measurement lives in **`AnomalyCapture`**, which is `ANOMALY_CAPTURE=0` in Shipping and
holds all render deps (`Renderer`, `RHI`, `RenderCore`, plus the Renderer **private** include path,
`AnomalyCapture.Build.cs:24-43`). ⇒ **a pixel-based cure either is non-Shipping-only, or
`AnomalyInjector` gains render dependencies.** *(Not forbidden — CLAUDE.md's invariant contemplates
those deps — but it is a plugin-shape decision, not an implementation detail.)*

**(2) A PIXEL MEASUREMENT CANNOT INFORM A SAME-FRAME PICK-TIME DECISION.** The SVE runs on the
**render thread**, after post-processing, and its result returns by **async GPU readback**
(`AnomalySveCapturer::Drain_RenderThread` polls `IsReady()` and skips when not ready). The stencil
branch budgets **12 frames** for that round trip before it gives up on a mask
(`HeldAges[i] > 12`). ⇒ **any pixel-derived cure is a PRE-FLIGHT — arm, wait, then decide — not a
predicate the selector can call.**

### 6.1 The five candidates

| | measures | where it can run | cost | cannot see |
|---|---|---|---|---|
| **C-1** stencil / ID-buffer pixel count | **surviving pixel COUNT + pixel AABB** per tagged target, occlusion-correct | render thread, `SubscribeToPostProcessingPass` | full-screen PS + R8 RT + readback **per armed frame**, plus a **W×H CPU scan on the render thread** per frame; mutates target render state and forces `r.CustomDepth 3` | anything outside the tagged silhouette — **shadows (A35 / `SM_Ramp2`)**; anything not tagged |
| **C-2** depth comparison | target's **expected** depth vs scene depth at its rect | render thread, `PrePostProcessPass_RenderThread` *(exists; currently unused)* | one depth read; **cheaper than C-1** — no tagging, no per-target pass | ⛔ **its reference depth comes from the SAME bounds `H5` says are untrustworthy**; cannot separate "target is here" from "something else is at that depth" |
| **C-3** `WasRecentlyRendered()` | **binary**: did this primitive render recently | game thread, free, anywhere | ~free — one float compare | ⛔ **HOW MUCH** it drew. Also **TRUE for a shadow-only contributor** (see `G126`) |
| **C-4** per-instance / per-section bounds | real extent of **each instance** instead of the cluster | game thread, pick time | O(instances); `GetInstanceTransform` + mesh bounds, pure CPU, no render deps | ⛔ **a PLAIN component whose bounds exceed its mesh** — i.e. `BP_SpawnPad_C` (−114.8, plain SMC) |
| **C-5** render-relevant bounds (the LOCKED `P6` ruling) | **WHICH components** count as "the object" | game thread, pick time | negligible; reuses `IsRenderableComponent` | ⛔ **how big one renderable component's bounds are** — an ISM's `Bounds` stay the whole cluster |

### 6.2 What each would have returned on the 7 banked targets

⛔ **Stated as DERIVABLE or NOT DERIVABLE. Nothing is estimated.**

| candidate | derivable? | result |
|---|---|---|
| **C-1** | ⛔ **NOT DERIVABLE** | No banked artifact carries a per-target pixel count. The nearest banked quantity, whole-frame `\|d\|`, is a **contrast-weighted** change, not an **area** — substituting it is exactly the named error family. **One bound IS derivable:** on the foliage leg the change occupied **4 of 64** grid cells against a **100 %** claim, so a count would have been far below the claimed area. That bounds *where change occurred*, not the count. |
| **C-2** | ⛔ **NOT DERIVABLE** | Same reason, plus the reference depth is not recorded either. |
| **C-3** | ✅ **DERIVABLE** | **TRUE on all 7 → 0/2 BAD caught, 0/5 GOOD broken.** Every one of the 7 passed selection (`IsVisible()` ∧ frustum ∧ ≥1 of 9 clear rays) and every hide produced a **non-zero, localized** change (min `0.00192`), so all 7 were drawing. **C-3 separates NOTHING on this set** — as expected: it targets class (i), and this set contains **no** class (i) instance. |
| **C-4** | ⚠ **PARTIAL** | Applies to **3 of 7** (the instanced ones): `RoomBuilderSquare_C` GOOD, `InstancedFoliageActor` BAD, `BP_SplineSpawn_C` BAD. **Leaves the 4 plain-SMC targets untouched — including `BP_SpawnPad_C`, whose −114.8 is the whole point of `G124`'s generalisation.** Whether it would **break `RoomBuilderSquare_C`** is ⛔ **NOT DERIVABLE** — that depends on the reduction rule (union? largest? nearest?), which no candidate specifies. Exact per-instance transforms are runtime state, absent from every artifact. |
| **C-5** | ✅ **DERIVABLE** | **0/2 BAD caught, 0/5 GOOD broken — a NO-OP on this set.** All 7 were selected *through* a component that already passed `IsRenderableComponent`, so `IsVisible()` was already true for it; C-5 changes **which** components are unioned, never **how big** a renderable component's bounds are. ⇒ **C-5 is a correctness fix for `P6`'s bounds defect, NOT an `H5` cure.** |

🆕 **A REAL DEFECT C-5 *WOULD* FIX, found in source this turn:** the **label** path
(`ProjectActorBoundsToScreenRect`, `AnomalyViewport.cpp:653-685`) unions components on a **TYPE-ONLY**
test with **no `IsVisible()` gate**, while **selection** (`IsRenderableComponent`, `:493`) **does**
check `IsVisible()`. **The label rect and the selection set already disagree about what "the object"
is** — an invisible mesh component enlarges the box but cannot be selected through. C-5 makes them
agree. *(This is `G33`'s "one definition of the object" applied to the box; it does not touch `H5`.)*

### 6.3 Question A — which class does each address?

| candidate | class (i) *non-drawing* | class (ii) *over-claiming* |
|---|---|---|
| **C-1** stencil pixel count | ✅ zero surviving pixels | ✅ count ≪ claimed area |
| **C-2** depth | ⚠ partial (a null mesh writes no depth) | ⛔ **no** — reference depth is derived from the bad bounds |
| **C-3** `WasRecentlyRendered` | ✅ **only** | ⛔ **no** — binary |
| **C-4** per-instance bounds | ⛔ no | ⚠ **the AGGREGATE half only** |
| **C-5** render-relevant bounds | ⛔ no | ⛔ no |

⇒ **C-1 is the only candidate that addresses BOTH.**

### 6.4 Question B — which could ALSO answer `H4`? (HYPOTHESIS, per §3.3)

| candidate | hypothesis | uncertainty |
|---|---|---|
| **C-1** | **Plausible, and it is the branch's own premise** — the shader's test *is* an occlusion test (`customDepth >= sceneDepth - bias`), so zero surviving pixels covers *blocked* and *draws nothing* alike | ⚠ **The TIMING differs between the two uses.** `LOCK-1` on that branch takes the mask **while the target renders** and falls back to a projected/last-known box once it is hidden. `H4` path (b) fires at an **already-occluded** target, so the mask is zero **with nothing to fall back to**. One measurement, two different moments — **UNESTABLISHED** |
| **C-2** | **Plausible for `H4`** — depth comparison is precisely an occlusion test | ⛔ **and it is the candidate that does NOT serve `H5`.** A cure chosen for `H4` alone |
| **C-3** | **Plausible, and by far the cheapest** — component `LastRenderTime` is bumped only when `PrimitiveDefinitelyUnoccludedMap` is set (`SceneVisibility.cpp:2491`), i.e. it is **genuinely occlusion-aware** | ⚠ **`G126`** — the shadow path bumps it too (`ShadowSetup.cpp:1672,1909`), so a shadow-only contributor reads "rendered". Also latent: render thread ≥1 frame behind, plus occlusion-query buffering |
| **C-4 / C-5** | ⛔ **no** — neither reads occlusion | — |

### 6.5 Question C — DELIVERY MODE. ⚠ LOAD-BEARING, answered for every candidate.

🚨 **THE PREMISE NEEDS CORRECTING FIRST, FROM SOURCE: DELIVERY MODE GATES *REPORTING*, NOT
*MEASUREMENT*.** `EvaluateSelectionProvenance` is called **unconditionally**
(`AnomalyCaptureSubsystem.cpp:1599`); only the **sidecar file** is suppressed (`:1720`); and
`coverage_pct` — a field of that same struct — **already reaches `annotation.json` in BOTH modes**
(`:1691`, written at `AnomalyLabelWriter.cpp:404`).

⇒ **NO CANDIDATE IS BLOCKED BY DELIVERY MODE.** All five can run and all five can report, because
`annotation.json` is written in both modes.

| candidate | runs in delivery? | real constraint |
|---|---|---|
| **C-1** | ✅ | ⚠ **SHIPPING**, not delivery — `AnomalyCapture` is compiled out of Shipping. Already true of capture itself, so a client capturing at all is on a non-Shipping build |
| **C-2** | ✅ | same |
| **C-3** | ✅ **most delivery-safe** | none — game thread, engine-only, needs no artifact at all if used as a gate |
| **C-4** | ✅ | none |
| **C-5** | ✅ | none |

### 6.6 Does it move `P6`?

🆕 **`annotation.json` ALREADY CARRIES THE SLOTS.** Every event object emits
**`mask: {provided: false}`** and **`depth: {provided: false}`**, hardcoded, today, in every delivered
artifact (`AnomalyLabelWriter.cpp:452-459`).

| candidate | moves `P6`? |
|---|---|
| **C-1** | ⚠ **VALUE, not SHAPE** — `mask.provided` flips to `true` in a slot that already ships; `bbox_norm` changes value. **Adding sub-fields under `mask` WOULD be a shape change** |
| **C-2** | ⚠ same, via the `depth` slot |
| **C-3** | ✅ **NONE** if used purely as a pick-time gate — no field added, removed, renamed or recomputed |
| **C-4** | ⚠ **VALUE only** — `coverage_pct`, `coverage_ratio`, `bbox_*` change value; no field changes |
| **C-5** | ⚠ **VALUE only** — this *is* the locked `node.bounds` ruling; no field changes |

### 6.7 What `feature/stencil-capture` already implements (READ-ONLY inspection, `C-1`)

**Tip `76cac74`; the stencil work is `468ed6b` → `b39c0d0` → `76cac74`. NOT checked out, NOT merged,
NOT rebased, NOT modified.** Diff vs `master`: **21 files, +1283/−48**.

✅ **It got a long way.** A real global shader (`Shaders/Private/AnomalyVisibleMask.usf`) tests
`customStencil ∈ reserved ∧ customDepth ≥ sceneDepth − bias` and writes a tag-valued R8 mask; the SVE
runs it after **Tonemap**, reads back async, and reduces to **`FAnomalyMaskAABB{MinX,MinY,MaxX,MaxY,
Count}` per tag** — **`Count` IS the surviving-pixel count `C-1` needs, and it already exists.**
Tagging saves and restores each primitive's prior `{bRenderCustomDepth, CustomDepthStencilValue}`
exactly and refcounts `r.CustomDepth 3`.

⚠ **THREE THINGS A COSTING MUST CARRY:**
1. 🚨 **Its last commit "excludes `InstancedFoliageActor`" — the class blacklist the owner has RULED
   IS NOT A FIX** — and the code comment justifying it says *"standalone ISM/HISM crates have sane
   bounds"*, which is now **MEASURED FALSE** (`BP_SplineSpawn_C` −19405.5 is a standalone ISM;
   `BP_SpawnPad_C` −114.8 is a plain SMC). **The branch's own scoping rests on a premise `H5`
   refuted.**
2. 🚨 **A NARROWING DEFECT:** its `IsRenderableMesh` tests **`USkeletalMeshComponent`**, while
   `master`'s selector tests the **base `USkinnedMeshComponent`**. A `USkinnedMeshComponent` that is
   not a `USkeletalMeshComponent` (e.g. `UPoseableMeshComponent`) is **selectable but never tagged**
   ⇒ **mask count 0 on a target that draws — a false "contributes nothing", failing in the dangerous
   direction.**
3. ⚠ Its reduction is a **W×H single-threaded CPU scan on the render thread** per armed frame
   (921,600 byte reads at 1280×720). Not measured; flagged as the cost nobody has priced.

---

## 7. `P6`'s FOURTH OBSERVATION — label geometry and selection geometry DISAGREE

⛔ **RECORDED, NOT FIXED. `P6` DOES NOT MOVE.** Same family as `C-5`, which is a **measured no-op on
`H5`** (§6.2) — so this is **not** `H5`'s cure and must not be mistaken for it.

| path | test | `IsVisible()` gate? |
|---|---|---|
| **selection** — `IsRenderableComponent` (`AnomalyViewport.cpp:493`) | `IsVisible()` ∧ ISM instance count ∧ type | ✅ **YES** |
| **label rect** — `ProjectActorBoundsToScreenRect` (`:653-685`) | **type ONLY** — `IsA<UStaticMeshComponent>() \|\| IsA<USkinnedMeshComponent>()` | ⛔ **NO** |
| **`node.bounds`** — `GetComponentsBoundingBox(true)` | every `UPrimitiveComponent`, incl. collision + gizmos | ⛔ **NO** |

⇒ **an invisible mesh component ENLARGES the label box while being unable to carry a selection.**
Three code paths, three different answers to *"what is the object?"*

🚨 **ADJACENCY TO THE CURE — NOTED, NOT ACTED ON.** If a mask measures *"drawn pixels of the target"*,
that is a **FOURTH** definition. **A cure that adds a fourth definition is a defect generator.** Any
cure design must state which single definition selection, labelling and masking all share.

---

## What is NOT established

- **Which mechanism accounts for the client's cases**, in what proportion. **No incidence claim exists
  for any row above.**
- Whether `H5` (i) occurs in this project at all — StackOBot is a polished sample and may simply not
  contain the pattern. **That would be a property of THIS PROJECT, not evidence against class (i) in
  the client's game.**
- Whether any two rows share a cure.

*(Maintained alongside `docs/gotchas.md`. Referenced from `CLAUDE.md`. Created 2026-08-19.)*

SCOPE CORRECTION — what m26 actually does (source-derived, not measured)

m26 is recorded as "the H5 cure". That name is narrower than the shipped behaviour.

The veto switches on the mask state enum alone. MEASURED_ZERO carries no cause. The
code therefore deletes ANY event whose target was measured to draw zero pixels in
that view, whatever produced the zero. Candidate producers include H5-shaped targets
that cannot manifest, occluded targets (H4-shaped), targets that left the view rect,
and targets not rendered in that frame for any other reason.

This is a statement about what the code does, read from source. It is NOT a claim
that the cure catches H4 — that remains an open hypothesis, deliberately unclaimed.

Which causes actually produce MEASURED_ZERO rather than NOT_MEASURED is OPEN and is
the subject of investigation I11.

The m26 tag message under-describes this scope. The tag is NOT being rewritten. This
entry is the correction of record.

SAFETY-PROPERTY CORRECTION — the admit bias is sound at the enum and unsound at the
assignment (MEASURED, I11-A)

m26's safety property is recorded as structural: MaskStateProvidesMeasurement and
MaskStateVetoes switch on the state enum alone, MaxCount is never read, and "there is
no code path on which a magnitude can move an event between the two zeros."

THAT SENTENCE REMAINS TRUE AND IS NOT WITHDRAWN. No magnitude moves an event between
the two zeros.

WHAT IS WRONG IS THE SAFETY ARGUMENT BUILT ON IT. The argument assumed that an event
reaching MEASURED_ZERO had been measured. I11-A measured that it need not have been.

bPassRan tests Mask.CustomStencilExtent — a VIEW-LEVEL property, "was custom depth
produced at all this frame" — and uses it as a PER-TARGET precondition. When any
primitive in the scene writes custom depth the extent goes view-sized, and a target
that contributed no evidence whatever passes bPassRan and contributes a clean
Count = 0.

MEASURED, I11-A, five legs, two independent routes, every gate passed:
  lever OFF  framesNoPass=4 framesContributed=0            -> NOT_MEASURED -> ADMITTED
  lever ON   framesNoPass=0 framesContributed=4 maxCount=0 -> MEASURED_ZERO -> VETOED
Same target, same map, same seed, same session shape, pose matched to 0.175 deg. The
only change was one boolean on an unrelated lamp.

CONSEQUENCE: a target the instrument CANNOT SEE can be deleted as though it had been
seen and found empty. On UE 5.1 that includes every Nanite target (G134) — 244 of
MainWorld's 350 static-mesh actors by direct property read.

SCOPE, SO IT IS NOT OVERREAD: PIE only (G76) — MECHANISM CLAIM ONLY. NO INCIDENCE
CLAIM. The lever was constructed deliberately. Whether the shipping path supplies its
own writer is OPEN and is investigation I11-B. The m26 tag does not carry this. THE TAG
IS NOT BEING REWRITTEN. This entry is the correction of record.

UPDATE, I11-B STAGE 1, 2026-08-20: the shipping path SUPPLIES ITS OWN WRITER. Branch
Y-1. In the owner's play-gate smoke auto-pool run, with nothing constructed, the extent
was view-sized on 26 armed frames from the plugin's OWN accumulated tags, and both
MEASURED_ZERO targets were wholly Nanite and on screen. H6 does not need an external
lever.

H6 — DOCUMENTED, NOT FIXED. OWNER DECISION, 2026-08-20.

THE DEFECT: bPassRan tests a VIEW-LEVEL property and uses it as a PER-TARGET
precondition. A target contributing no evidence about itself can therefore reach
MEASURED_ZERO and be vetoed as though it had been measured and found empty.
MEASURED, I11-A (five legs, two routes) and I11-B Stage 1 (unaided, shipping path).

IT IS NOT A NANITE-ONLY DEFECT. FIVE routes are now named, FOUR of them PROVEN:
  (a) NANITE — the target cannot write custom depth at all on 5.1 (G134). HIGH HARM: a
      fully visible, drawing target is deleted.
  (b) OFF-SCREEN — proven with SM_GratIng, NON-Nanite. LOWER HARM: deleting an
      off-screen target's label is arguably the correct outcome reached by an unsound
      route.
  (c) FULLY OCCLUSION-CULLED — removed from the visible set, so relevance never runs and
      no custom depth is written. LOWER HARM: deletion is what H4 says SHOULD happen.
      Right outcome, unsound route. SOURCE-READ.
  (d) DEGENERATE GEOMETRY — zero sections/triangles, or an ISM whose instances are all
      culled while GetInstanceCount() stays > 0. LOWER HARM: this is H5 class (i), the
      case the cure exists to catch. Right outcome, unsound route. SOURCE-READ.
  (e) TRANSLUCENT MATERIAL — 🚨 HIGH HARM, AND THE RESULT IS SIMPLY WRONG. VERIFIED FROM
      5.1 SOURCE. FCustomDepthPassMeshProcessor::UseDefaultMaterial sets
      bIgnoreThisMaterial for a translucent blend mode unless the material opts in
      (CustomDepthRendering.cpp:310-338, whose own comment reads "ignore translucent
      materials without allowing custom depth writes"), and TryAddMeshBatch then
      `return true` with NO draw command added (:359-364). The opt-in is
      AllowTranslucentCustomDepthWrites (Material.h:833-835, read at Material.cpp:6253-
      6256), an author-ticked box under Translucency > Advanced with NO initialiser and
      NO constructor assignment anywhere in the engine — DEFAULT OFF.
      ⚠ AND IT IS SELF-SUFFICIENT, unlike every other route: the target still TAGS
      (IsRenderableComponent has no material test, AnomalyViewport.cpp:493-510) and its
      relevance still sets bRenderCustomDepth with no blend-mode test
      (StaticMeshRender.cpp:1936, PrimitiveSceneProxy.h:598), so IT SUPPLIES ITS OWN
      bPassRan. A SINGLE-TARGET run on a translucent object is enough. A plainly
      visible, fully drawing object has its label DELETED.

🚨 NONE OF (b)–(e) DEPENDS ON NANITE. Occlusion culling, degenerate geometry and
translucent materials bite with Support Nanite OFF exactly as they would with it on.
THE SUPPORT-NANITE ARGUMENT DOES NOT COVER THEM, AND NEITHER DOES THE PRE-DELIVERY
CHECKLIST BOX THAT ASKS ABOUT IT.

✅ ONE ROUTE IS CLOSED, AND IT IS CLOSED STRUCTURALLY, WHICH IS WORTH AS MUCH AS A
MEASUREMENT: bTagFailed (no renderable component to tag) ADMITS. In ArmIfMeasurable the
`continue` at AnomalyMaskMeasure.cpp:180-185 precedes ArmMask (:188), ++ArmsIssued (:189)
and the ArmedRequestToRecord insert (:190); CollectResults builds its entire work list
from that map (:302-304); and State initialises to NotMeasured (AnomalyMaskMeasure.h:22)
and is written only on the clean contribute path (:437). A tag-failed event therefore
never reaches the bPassRan test at all, so ANOTHER ACTOR'S TAG CANNOT PULL IT INTO A
MEASUREMENT. Confirmed empirically too: tagFailed = 0 on all eight play-gate smoke
events.

THE SET OF ROUTES IS STILL NOT CLOSED. Five are named. Others may exist and have not been
looked for.

THE CURE'S REACH IS VIEW-DEPENDENT AND CAN BE LOW — MEASURED, owner play-gate smoke,
2026-08-20. A FINDING, NOT A LOG NOTE.

Two runs ELEVEN MINUTES APART, on the SAME BUILD, in REAL GAMEPLAY on MainWorld:

    session_20260820-211024    1 of 8 events NOT_MEASURED    mask_nopass_discards  4
    session_20260820-211345    5 of 8 events NOT_MEASURED    mask_nopass_discards 20

Those events were ADMITTED BECAUSE NOTHING WAS MEASURED, not because they were measured
to draw. On 211345 the m26/m27 cure was effectively INERT FOR FIVE OF EIGHT EVENTS. All
five carried framesNoPass=4 with framesContributed=0 — every armed frame failed the
custom-depth pass.

THIS IS THE DESIGNED SAFE DIRECTION AND IS NOT A DEFECT. NOT_MEASURED => ADMIT. Nothing
was wrongly deleted. IT IS THE ADMIT BIAS WORKING, and the run is internally consistent:
211345 produced ZERO MEASURED_ZERO events, so vetoed_events = 0 is the CORRECT output
rather than a silent failure. The full breakdown was 5 NOT_MEASURED / 3 MEASURED_NONZERO
/ 0 MEASURED_ZERO, and mask.provided matched the tri-state on all eight rows.

NO MECHANISM IS CLAIMED (G120). framesNoPass is NOT a Nanite counter — frustum culling,
occlusion culling and any other route by which the target is absent from the view's
relevant set reach the same counter. ONE of the five IS established: SM_Ramp2 is the
KNOWN-NANITE CONTROL and MUST read NOT_MEASURED every time (G134), so its appearance here
is the control behaving correctly. THE OTHER FOUR — BP_Stomper_C, BP_MovingPlatform_C,
BP_PressurePlate_C, RoomBuilderSquare_C — ARE NOT ESTABLISHED AND WERE NOT CHASED.

WHY IT WAS INVISIBLE UNTIL NOW, AND THIS IS THE TRANSFERABLE PART: THE BENCH LEGS
STRUCTURALLY COULD NOT HAVE SHOWN IT. They run unattended with a settled camera and read
notMeasured = 0. A moving camera with real World-Partition streaming swung the rate from
1-in-8 to 5-in-8 between two runs minutes apart. That is G135's shape again — an
instrument environment that cannot exhibit the case, with the blindness presenting as a
CLEAN PASS. AN OWNER-PLAYED RUN IS A DIFFERENT INSTRUMENT FROM A BENCH LEG AND SEES
THINGS THE BENCH CANNOT. It is also the same axis as the m26 smoke's Finding 3, "the
measurement is VIEW-DEPENDENT" — that finding was about one target measuring differently
in two views; this one is about HOW MANY events get measured at all.

CONSEQUENCE, DECISION-RELEVANT AND NOT YET DECIDED: HOW MUCH OF H5 THE CURE CATCHES IN A
CLIENT CAPTURE IS VARIABLE AND CAN BE SMALL. Any future statement to a client about what
the cure does must carry that. NOT a defect, NOT queued, NO number minted.

ALSO RECORDED SO IT IS NOT RE-DISCOVERED AS NEW: SM_Ramp2 logged collisions=2 with
"unassigned reserved tag 255 observed, cause not established". That is G133's known
single-pixel detector, cause not established, UNCHANGED. Not a new observation.

FOLIAGE EXCLUDED FROM SELECTION — m27, 2026-08-20. THE INSTANCE GOES; THE MECHANISM STAYS.

InstancedFoliageActor is excluded from selection because ITS LABEL IS UNUSABLE, not
because the anomaly fails to occur.

THE HIDE DOES MANIFEST. Measured, journal 045 Parts Nine/Ten (post marker-contamination
correction): hiding InstancedFoliageActor_0_0_0 changed the frame by a whole-frame mean
of 0.0059, concentrated in 4 of 64 grid cells, peak cell 0.1242 — localised exactly where
the bushes are. If the hide did nothing that number is zero.

WHAT FAILS IS LOCALISATION. coverage_pct reads 100 and bbox_px reads the entire frame
while roughly 1.4% of it changes — wrong by two orders of magnitude. A label that boxes
the whole frame teaches a detector that unchanged pixels are anomalous. That is the same
failure mode as a labeled-but-invisible sample and it is worse than no label at all.

CORRECTION — DISCARD THIS: "HISM ignores SetActorHiddenInGame, so foliage never actually
hides." NOT SUPPORTED. Our own banked measurement points against it, and Actor.cpp:4556
shows SetActorHiddenInGame calls SetHidden() + MarkComponentsRenderStateDirty() with no
foliage opt-out found in source. The July 2026 observation is NOT called wrong — it may
have been a different build, a different actor, or an effect small enough to read as
absent — but it is NOT ESTABLISHED and must never again be written as the reason.

THE EXCLUSION IS A STOPGAP, NOT A CURE, AND MAY NOT BE PERMANENT. It removes a target
whose label cannot be localised with the tools that exist today. A per-instance bbox would
make foliage a legitimate target again. That path is UNBUILT and UNQUEUED — recorded as a
possibility, not a plan.

WHY m23's GUARD DOES NOT ALREADY CATCH THIS — VERIFIED FROM SOURCE, m27:
the manifested guard keys on a FLAG at every link, so it agrees with the actor and stays
silent. AnomalyCaptureSubsystem.cpp:1492 fills FireHidden from FActor->IsHidden(); :2020
and :2022 carry it into HiddenByIndex; :2053 derives HiddenIdx from it; and bManifested is
HiddenIdx.Num() > 0. Both hide anomalies set exactly that flag
(Anomaly_MissingObject.cpp:35, Anomaly_Blinking.cpp:78). The guard is not broken — it
answers a different question.

THE COST, OWNER-ACCEPTED AND STATED AT ITS REAL SIZE: delivered datasets will contain NO
foliage anomalies at all, permanently, until per-instance foliage handling exists.
⚠ AND IT IS NOT A ROUNDING ERROR ON THIS MAP. Foliage is 2 of ~350 mesh-owning actors,
but the SELECTOR'S pool is far narrower — MainWorld's settled view offers about SIX
selectable actors, and the play-gate smoke tagged exactly six, two of them foliage. THE
EXCLUSION REMOVES A THIRD OF THE SELECTABLE VARIETY IN THAT VIEW. The cost paragraph
should not read cheaper than it is.

🚨 AND THE MECHANISM IS NOT CURED BY THIS. THE AUGUST RULING STANDS, SCOPED NOT REVERSED:
a class blacklist is NOT a fix for H5, because it hides one instance while OVERSIZED
BOUNDS stay open for every other actor. That argument was about CURING H5 and it is
untouched. What it never addressed is whether to keep firing at a target whose label
cannot be localised, which is a separate question and the one m27 answers.
  H5 CLASS (ii) REMAINS OPEN for every actor whose bounds exceed its drawn footprint.
  BP_SpawnPad_C IS THE NAMED EXAMPLE: a plain UStaticMeshComponent — not instanced, not
  foliage — with poll_distance −114.8, which no foliage blacklist touches and which G124
  generalised from precisely to stop this being read as a foliage problem.
  THE OVER-CLAIM RULE REMAINS THE CURE AND REMAINS UNBUILT — and it is now UNBLOCKED:
  SM_GratIng is the complex-silhouette NON-Nanite control whose absence journal §209 gave
  as the reason a ratio could not be calibrated. It exists, in the shipped level, and it
  is measurable.

⚠ WHY NO BENCH LEG WOULD EVER HAVE CAUGHT (e) — G135 AGAIN: a scan of the live MainWorld
found ZERO translucent material slots across 350 mesh actors, 593 static/skinned
components and 1,055 slots. The calibration content cannot exhibit the defect class.
⛔ THAT NUMBER IS ABOUT THE WRONG PROJECT. It says nothing about the ship target, whose
translucent-asset count is UNMEASURED, like its Nanite count. And the scan has stated
weaknesses (G136): it read the BASE material's blend mode, so a MATERIAL INSTANCE
OVERRIDING THE BLEND MODE would read as opaque — the engine warns about exactly this at
MaterialShared.cpp:1758 — and it does not cover dynamic instances or static-switch
permutations.

MITIGATION IN THE CODE: MaxCount is a MAX across contributing frames, so ONE real
non-zero reading survives any number of phantom zeros. The exposed case is a target
contributing no real evidence on EVERY armed frame.

WHY IT IS NOT BEING FIXED: the near-term ship target is Concorde, where Nanite support
is DISABLED at project level (owner-verified 2026-08-20), which removes route (a) as
configured. Route (b) remains and is accepted at its stated lower harm.

OWNER DECISION 2026-08-20, AND THE DISAGREEMENT IS RECORDED SO IT IS NOT RELITIGATED:
THE MASK SHIPS ON (m27), AND ROUTE (e) IS ACCEPTED AS A COST.
  OPTION A, RULED: turn the mask on in delivered builds and accept that translucent
  targets can have good labels deleted in delivered captures.
  OPTION C, RECOMMENDED BY CHAT AND NOT TAKEN: fix route (e) before shipping the mask
  on, on the grounds that ITS FAILURE IS INVISIBLE TO THE CLIENT — she receives fewer
  labels with no signal that anything went wrong, which is the one failure mode this
  project has consistently treated as worse than a loud one.
  THE OWNER RULED A. The reasoning that carried it: with the mask OFF a delivered build
  behaves as m25, which is the invisible-anomaly labelling the client complained about
  in the first place. Shipping it off is a KNOWN defect affecting all geometry; shipping
  it on is an ACCEPTED defect affecting translucent geometry of unmeasured extent.
⚠ ROUTE (e) IS NOT BEING FIXED IN m27, AND NO FIX FOR ANY ROUTE IS DESIGNED, PROPOSED OR
QUEUED. m27 is a DEFAULTS AND PACKAGING change, not a repair.
📌 m27 CARRIES A DIAGNOSTIC FOR THE ACCEPTED COST: each vetoed event logs whether any
material slot on its tagged components is translucent, so the population nobody has
measured becomes measurable from a delivered session. It is a READOUT — it does not feed
the veto and must never become a filter.

THE CONDITION THIS RESTS ON, AND IT IS ONE CHECKBOX:
  Project Settings > Engine > Rendering > SUPPORT NANITE, currently UNTICKED.
  IF IT IS EVER TICKED, ROUTE (a) GOES LIVE ACROSS EVERY NANITE-FLAGGED MESH AT ONCE,
  WITH NO CHANGE TO THE PLUGIN. Concorde's Nanite-flagged asset count is UNKNOWN — the
  5.1 Content Browser has no Nanite filter and no census was run.

WHAT WOULD REOPEN THIS: Nanite support enabled in any host title · a host that already
writes custom depth (outlines, highlights, post-process masks) — NEVER ASSESSED, and it
supplies the precondition permanently, independent of Nanite · a third route found ·
any evidence on incidence in a delivered capture.

NOT CLAIMED: no incidence claim anywhere. All evidence is PIE (G76). The four vetoes in
the play-gate smoke are NOT attributed to H6 — H6 was present and active in that run;
that is not the same as having caused them (G120).

WHY DISABLING SUPPORT NANITE MAKES A NANITE-FLAGGED MESH MEASURABLE — VERIFIED FROM 5.1
SOURCE, AND IT IS CONDITIONAL. The chain, cited:
  1. the project checkbox is r.Nanite.ProjectEnabled (RendererSettings.h:560), backed by
     GNaniteProjectEnabled, default 1 (RenderUtils.cpp:22-25).
  2. DoesPlatformSupportNanite returns FALSE outright when it is 0
     (RenderUtils.cpp:1727-1734), and UseNanite (RenderUtils.h:759) reaches it through
     DoesRuntimeSupportNanite.
  3. UStaticMeshComponent::ShouldCreateNaniteProxy is therefore FALSE
     (StaticMeshComponent.cpp:1719-1736), so NO Nanite::FSceneProxy is created.
  4. 🚨 THE FALLBACK IS GATED ON A SECOND CVAR: r.Nanite.ProxyRenderMode
     (GNaniteProxyRenderMode, StaticMeshRender.cpp:127-140). At its DEFAULT 0 the
     component falls through to a conventional FStaticMeshSceneProxy
     (StaticMeshRender.cpp:2459-2477). At 1 or 2 the code RETURNS NULLPTR and the mesh
     RENDERS NOTHING — its own comment says "just make the mesh invisible instead".
     Same shape for ISM (InstancedStaticMesh.cpp:2362-2374) and HISM
     (HierarchicalInstancedStaticMesh.cpp:3179-3186).
  5. The conventional proxy DOES set the flag the mask needs:
     FStaticMeshSceneProxy::GetViewRelevance does Result.bRenderCustomDepth =
     ShouldRenderCustomDepth() (StaticMeshRender.cpp:1936) — exactly what
     Nanite::FSceneProxy::GetViewRelevance never does (G134,
     NaniteResources.cpp:941-1010).
⇒ TRUE, ON THE STATED CONDITION: with SUPPORT NANITE unticked AND
  r.Nanite.ProxyRenderMode at its default 0, a Nanite-flagged mesh renders through the
  conventional path, can set bRenderCustomDepth, and IS measurable by the m26 mask —
  so route (a) is INERT in Concorde as configured.
  ⚠ THE CONDITION IS NOT ONE CHECKBOX BUT TWO. r.Nanite.ProxyRenderMode is a
  SCALABILITY cvar and can be set by an ini, a device profile or a scalability group
  without anyone touching project settings. If it is non-zero, Nanite-flagged meshes do
  not render AT ALL — a louder failure than a measurement one, but it is a different
  failure and it is not the state this decision assumes.
  ⚠ The same fallback governs Nanite being unsupported for ANY other reason — the
  r.Nanite cvar, missing 64-bit atomics, or forward shading (RenderUtils.h, UseNanite /
  DoesRuntimeSupportNanite). The decision rests on the fallback, not on the checkbox.

---

# 8. `P9` — the blinking hidden set the label claims is not the one the eye saw

> 🔢 **THE NUMBER IS `P9`, NOT `P8`, AND THE DEVIATION IS DELIBERATE.** The instruction that opened
> this entry said *"P8"*. **`P8` has been taken since 2026-08-18** — TAU is not camera-pose
> invariant — and phenomenon numbers are **NEVER reused**. `P9` was minted in journal 066 §3 and is
> already carried in `CLAUDE.md`'s phenomenon ledger, so this entry completes that mint rather than
> opening a second one. The two chat handoffs that say "P8 — blinking label offset" are referring
> to **this** entry; they are wrong about the number only.

| | |
|---|---|
| **status** | 🔴 **OPEN.** Owner-observed. **Two bench reproduction attempts, 2026-09-02.** v1 (`MainWorld`) returned `UNDECIDABLE` on all five legs — a **fixture** failure, not a `P9` result (journal 067 §11, `G206`). **v2 (a letterboxed `CB_GateLevel`, settled camera) GRADED 16 EVENTS ACROSS FOUR LEGS: ALL ALIGNED, `k=0`, both differences empty, ZERO `P9`-SHAPE**; 8 further events UNDECIDABLE (journal 067 §12). ⛔ **That is still NOT the pre-declared "NOT REPRODUCED"**, which required *every* event graded. **Neither reproduced nor refuted. Nothing here is upgraded and nothing is discarded.** |
| **class** | **Labelling ↔ manifestation mismatch at hide BOUNDARIES.** `blinking` only. |
| **scope** | ⚠ **`missing_object` is NOT reported affected** — that is the owner's report, not a measured exclusion. |
| **evidence** | **OWNER-OBSERVED (eye), Bates, over RDP.** **3 instances**, spread across **both** m36 Section B legs (floor 6.0 and floor 0.5). |
| **mechanism** | ⛔ **NONE CLAIMED.** See §8.4. |

## 8.1 The run configuration it was seen under

Both Section B legs, per the RDP card's payload and the StartRun echo typed back:

| axis | value |
|---|---|
| mask | **ON** (`IAI.Capture.Mask 1`) |
| census | **ON**; `excludeTranslucent=1`, `reservation=1`, `maxVerdictAgeTicks=12` |
| floor | leg 1 **6.0** (compiled default) · leg 2 **0.5** (console) |
| view rect | **letterboxed, non-zero `Rect.Min.Y`** — `(0,138)` and `(0,69)` |
| `tickpin_compiled` | **false** |
| burst schedule | `IAI.Capture.Config 2 4 8 4 0`, 90-frame cap, auto-pool |

📌 **THE OWNER-OBSERVATION RULE APPLIES IN FULL: the observation STANDS regardless of what the bench
later shows.** A bench that fails to reproduce it has failed to reproduce it — that is a fact about
the bench, not a retraction of what was seen on the host. This project has been wrong in that
direction before (`G135`: the bench legs structurally could not exhibit the case, and the blindness
presented as a clean pass).

## 8.2 The one instance that was transcribed frame by frame

```
annotation.json  frame_indices (claimed hidden) : { 42, 43, 47, 48 }
owner, watching the frames (observed hidden)    : { 42, 43, 44, 48 }
```

Two differences, **in opposite directions**, inside one event window:

- **44** is observed hidden and is **absent** from the claim.
- **47** is observed visible and is **present** in the claim.

**47 − 44 = 3**, and **3 is exactly one half-period at the blinking default** — `DefaultHalfPeriodFrames = 3`,
source-verified at `Source/AnomalyInjector/Private/Anomalies/Anomaly_Blinking.h:28`, with a
`static_assert` binding it to `AnomalyDefaults::BlinkingHalfPeriodCompiled`
(`Anomaly_Blinking.cpp:27-30`) so the echoed number and the used number cannot silently disagree.
⚠ **That arithmetic is RECORDED AS AN ARITHMETIC COINCIDENCE OF THE OBSERVED NUMBERS. It is not
offered as a mechanism, and "one half-period" names no code path in this entry.**

🚨 **THIS IS NOT `P1`'s SHAPE.** `P1` is a **constant** shift — every claimed frame off by the same
amount. No single shift maps `{42,43,47,48}` onto `{42,43,44,48}`: the first two frames agree
exactly, so the set is not displaced, it is **wrong in one position in each direction**. Filing this
under `P1` would merge two different signatures.

⚠ **THE OTHER TWO INSTANCES WERE NOT TRANSCRIBED FRAME BY FRAME.** Recorded as *same shape, frames
not recorded*. ⛔ **No numbers are invented for them.**

## 8.3 What is NOT known

- ⛔ **Whether the LABELS are wrong or the PIXELS are wrong.** Both remain open. A mislabelled frame
  and a mis-rendered frame produce the same complaint from the eye.
- ⛔ Whether the **bench reproduces it at all.** ⚠ **STILL OPEN AFTER THE 2026-09-02 ATTEMPT** — the
  legs returned `UNDECIDABLE`, so the question was not answered in either direction. The blocker is
  a **fixture** one, not a `P9` one: a non-zero view-rect origin forces `MainWorld`, whose intro
  camera moves during capture, and a moving camera defeats every bbox-scoped pixel oracle this
  project owns (journal 067 §11.6).
- ⛔ Whether it depends on **pacing**, on the **letterbox**, or on the **census** being on. ✅
  **DELIVERY MODE IS ELIMINATED** as a factor — measured, leg C (journal 067 §11.8). ✅ The **tick
  ratio** is eliminated as a *discriminator* — 1.3556 reproduces exactly on this bench (§11.4).

## 8.4 Mechanism — NONE CLAIMED

**Frame-saga discipline: measure, then design.** Candidate mechanisms exist in this repo's record —
`m20`'s one-game-tick-stale hidden state (**FIXED**), the `m31` arm→present pairing family — and
**not one of them is asserted here** (`G120`). No mechanism goes into any brief, plan or handoff for
this phenomenon before a measurement exists. The measurement plan is
`docs/predictions/2026-09-02-p9-blinking-boundary-repro.md`, pre-declared before any leg runs.

## 8.5 An UNEXPLAINED CO-OBSERVATION — not a cause

`ticks_per_captured_frame` **is** `capture_game_ticks / total_frames` **by construction**
(`AnomalyLabelWriter.cpp:546-548`) — on Bates, **122/90**. So the ratio itself is not the open
question. **The unmeasured quantity is the 32 surplus game ticks**, enumerated in
`docs/predictions/2026-09-02-p9-blinking-boundary-repro.md` §5 and **not attributed**.

It is recorded **beside** `P9`, not **under** it: it was observed on the same runs and nothing
connects the two. ⛔ **Do not write it as a cause, a lead, or a discriminator until a leg says it is
one.**

## 8.6 Mitigation AVAILABLE NOW

**Untick `blinking` on Bates runs until `P9` closes.** Every other anomaly this plugin ships is
single-state, so no other anomaly has a hide *boundary* inside its window for a boundary defect to
land on. That yields a clean Bates dataset immediately and costs one anomaly type on one host. It is
a **mitigation, not a fix**, and it is on the RDP card as a standing item (Section C-(e)).

## 8.6a 🔻 BENCH CAMPAIGN CLOSE-OUT — 2026-09-02/03. **STATUS STAYS `OPEN`.**

Two bench reproduction attempts ran. **Neither reproduced `P9` and neither refuted it.**

**v1 — `MainWorld`, letterboxed by the lever.** `UNDECIDABLE` on **all five legs, every event**.
⛔ **Not a `P9` result at all:** the fixture could not satisfy the conjunction the measurement
needed — a non-zero view-rect origin forced `MainWorld`, whose intro camera **moves during capture**
(32 distinct origins over 90 frames), so the per-event bbox changed every frame and `A56` collapsed
to modal 1-in-8. → **`G206`**. Journal 067 §11.

**v2 — a letterboxed `CB_GateLevel`, via the R3 zero-cook route, on the SAME exe `D2BB25A5`.** The
fixture satisfies both halves: settled camera (`distinct=1` bbox at `modal 100 %`) **and**
`rect=(0,92)-(1280,628)`. The reader passed **three in-regime controls on the leg's own data** before
anything was graded.

> **16 of 16 readable events, across 4 legs and both pacings, read `ALIGNED` with `k = 0` and BOTH
> DIFFERENCES EMPTY. `P9`-SHAPE appeared ZERO times. 8 further events were `UNDECIDABLE` on
> separation.**

⛔ **THE PRE-DECLARED `NOT REPRODUCED` IS *NOT* SATISFIED, AND IT WAS *NOT* LOOSENED.** That verdict
required **every** blinking event `ALIGNED`; 8 were not graded. The amendment rule holds — the
discriminator stands as written and no measurement was rounded up to meet it.

**Two caveats, stated because they bound what the negative is worth:**

1. ⚠ **Events below the separation floor are UNREADABLE, so "`P9` lives only in low-contrast events"
   is NOT excluded by this campaign.** ✅ **But the Bates instances were owner-EYE-VISIBLE, i.e.
   high-contrast — so the readable class is the diagnostic one**, and it is the class that came back
   clean.
2. ⚠ **The `SHIFTED` division of labour remains untested on real shifted pixels.** It is proven at
   `d-unit` on literal sets and refused correctly on synthetic shifts; no leg has yet produced a
   genuinely displaced label set for it to catch. → **`A65`**.

📌 **The owner-observation rule stands in full: the Bates observation is unaffected by any of this.**
**Next evidence is the Bates one-event typed read** (journal 067 §5; RDP card Section C item **(h)**),
not another bench leg.

## 8.7 Related entries

| | |
|---|---|
| **`P1`** | client's one-frame shift at ratio ≈ 1.2, 30 fps — **OPEN, never reproduced.** A **constant** shift; §8.2 says why `P9` is not it. |
| **`P3`** | a labelled hide that never manifests — **FIXED at `m23`.** Same family (labelling vs manifestation), already cured, and the reason the `manifested` flag and the zero-positive-frame guard exist. |
| **`P5`** | single-frame alignment undecidable ≥ 90 fps — **queued.** ⚠ Bates ran at **30 fps**, so `P5` is **not in play here unless a leg measures it into play**. |
