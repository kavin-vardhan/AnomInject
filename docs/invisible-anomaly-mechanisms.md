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
