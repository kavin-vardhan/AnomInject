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

## 8.6a 🔺 **SUPERSEDED — OWNER REPRODUCED `P9` ON BATES WITH NO FLAGS, 2026-09-02.**

> 🚨 **THIS BLOCK REPLACES THE BENCH CLOSE-OUT THAT PRECEDED IT. DO NOT APPLY THE OLD ONE.** The
> earlier close-out framed `P9` as "not reproduced on the bench, next evidence is a typed read". The
> owner then reproduced it **directly, on Bates, with every plugin flag off**, and measured a
> **per-frame opacity ladder**. Everything below is that evidence. ⛔ **NO MECHANISM IS CLAIMED FOR
> EITHER PHENOMENON.**

---

# 🏁🏁 `P9` **(B) IS CLOSED.** `m40` VALIDATED ON BATES, 2026-09-02.

> 🎯 **READ THIS FIRST. Everything below it is the investigation that got here, and is kept as
> history.** `P9` (A) closed on 2026-09-02 by the menu-off test (temporal accumulation). **`P9` (B) —
> the phase displacement — is now CLOSED by a fix validated on the host it was found on.**

**The validating run.** Owner, 2026-09-02 ~16:10 box clock, Bates **editor** build at `2f16bf7`
(carrying `m37` + `m38` + `m40`); the packaged build on that box untouched. Card `SECTION D`,
prerequisites exactly as `C-3`: fresh editor · AA + motion blur off via the title's menu ·
`blinking` ticked · `Log LogAnomaly Verbose` · config `2 4 8 4 0` ·
`IAI.Capture.Start "" png 4242 90 blinking`.

| read | value |
|---|---|
| `frame_indices` (c) | `blink StaticMeshActor_155` **`{4, 5, 6, 10}`** · `blink StaticMeshActor_1246` **`{28, 29, 30, 34}`** |
| index map (b) | 26→41859 · **27→41860** · **28→41863** · 29→41864 … 34→41869 · 35→41870 · 36→41873 |
| toggle lines (a), event 2 | **`[863]` HIDDEN · `[866]` VISIBLE · `[869]` HIDDEN** |
| toggle lines, event 1 | `[831]` · `[834]` · `[837]` — same shape at `n = 4` |
| eye (owner) | `StaticMeshActor_1246` **gone at 28, 29, 30, 34**, visible otherwise — *"labels match the disappearance"* |

**THE JOIN, event 2 (`n = 28`):**

| flip | toggle tick | = `frame_index` of | code says | eye says | label says |
|---|---|---|---|---|---|
| 1 — first hide | `41863` | **`28` = `n`** | hidden from 28 | gone 28 ✅ | in `frame_indices` ✅ |
| 2 — mid-event show | `41866` | **`31` = `n+3`** | visible from 31 | visible 31 ✅ | absent ✅ |
| 3 — second hide | `41869` | **`34` = `n+6`** | hidden at 34 | gone 34 ✅ | in `frame_indices` ✅ |

⇒ **`labels {28,29,30,34}` == `code` == `eye`. THREE-WAY AGREEMENT, frame for frame.**

## ✅ `D-0` PASS — AND IT IS THE **STRONG ROW**

**`apply → first toggle` still reads `Δ = +3`** (`n−1` = frame 27 = tick 41860; first toggle 41863),
and **the first toggle still lands ON `n`, a captured tick.** ⇒ 🎯 **BATES' TICK ORDER IS
UNCHANGED — the injector still ticks first there, exactly as at `C-3` — AND THE LABELS ARE NOW RIGHT
ANYWAY.** That is the strongest of the two admissible outcomes pre-declared at card `D-0`: the
disorder is still present and no longer reaches the labels. **`Δ = +2` would also have passed but
would not have exercised the fix; it did not occur.**

📌 **What `m40` did, stated precisely: it REMOVED THE DEPENDENCY rather than confirming the cause.**
The label's active bit is now sampled at `FWorldDelegates::OnWorldTickEnd`, after every tickable and
before the draw, so it is what the renderer will draw for that frame **whatever order the subsystems
ticked in**. ⛔ **THE TICK ORDER ON BATES WAS NEVER OBSERVED DIRECTLY, AND IT DOES NOT NEED TO BE.**
Every *"consistent with, not asserted"* line below **STANDS AS WRITTEN AND IS KEPT AS HISTORY** — the
one-tick-offset reading fitted every number on both hosts with no free parameters and nothing
contradicted it, but it was never promoted to a mechanism and it is not promoted now. **A fix that
makes the question moot is a better outcome than a fix that needed the answer.**

⚠ **A FINDING EN ROUTE, AND `m38`'s LOUD-INERT ECHO IS WHY IT COST MINUTES INSTEAD OF A RE-VISIT:**
`anomaly_log.txt` was **ABSENT on the first attempt** — **the Bates project runs DELIVERY MODE, so
the `m38` run log is auto-OFF there** (its default mirrors `run.json`). **The `Capture(runlog)` echo
said so in its own words**, the owner forced it with **`IAI.Capture.RunLog 1`** and re-ran. ⇒ `G210`,
and card `D-2` now carries the line. 📌 **This is also a live data point for the still-open question
of what the run log's CLIENT default should be** — on a delivery-shaped build it had to be forced.

**Consequences, executed:**
- ⛔→✅ **THE STANDING MITIGATION IS LIFTED. `blinking` is back in the Bates pool**, and card `C-(e)`
  is superseded. It lifted because the fix is *validated there*, not because it shipped.
- ✅ **Nothing further is owed to Bates for `P9`.**
- ⛔ **`m40` remains certified at one configuration** (`2 4 8 4 0`, 30 fps) on two hosts, and
  ⛔ **`IAI.Bench.SynthTickOrder` is still never typed on a host.**
- ⛔ **The sync-fallback path is still one tick stale by decision** — unchanged by this closure.

🧭 Implementation and the four bench legs: journal 068 §11. This validation: journal 068 §12.

---

### It is NOT a census or mask phenomenon

**Reproduced with `census OFF` and `mask OFF` — plain `blinking` + capture, no flags.**
⇒ ⛔ **`P9` IS NOT A CENSUS PHENOMENON AND NOT A MASK PHENOMENON.** The source read at §6 — that the
census never touches the fired target — **stays on the record but is MOOT for `P9`**: it answers a
question `P9` turned out not to depend on.

### Incidence — deterministic, not occasional

**Every reviewed `blinking` event, on both legs.** ⚠ The earlier *"3 instances"* was **the full
reviewed population, not a sample** — so the correct reading is **deterministic on Bates**, not
intermittent. That is a materially different phenomenon from the one the first ledger entry
described.

### The label cadence is the fixed, certified shape

`frame_indices` follow **(n, n+1, n+5, n+6)** — the 30 fps **sampled** shape, and it is
**identical on both hosts**. ⚠ **The labels are host-INDEPENDENT.**

### The per-frame opacity ladder — Bates, owner eye

⚠ **This supersedes the earlier hidden-set transcription where the two disagree. BOTH ARE KEPT.**

| frame | what the eye sees |
|---|---|
| **n** | ≈ **20 %** opacity |
| **n+1** | ≈ **10 %** opacity |
| **n+2** | **fully gone** |
| **n+3** | **still gone** |
| **n+4** | *not recorded* |
| **n+5** | **FULLY VISIBLE** |
| **n+6** | ≈ **20 %** opacity |
| **n+7** | fully visible |

### 🚨 TWO PHENOMENA, RECORDED SEPARATELY

**(A) BOUNDARY SMEAR.** Partial opacity on the first frames after a disable (**n** ≈20 %, **n+1**
≈10 %) and again at **n+6** ≈20 %. A **temporal-accumulation signature**. ⛔ **CAUSE NOT
ESTABLISHED.**

**(B) PHASE DISPLACEMENT — this is `P9` proper.** **Fully gone at n+2 and n+3 where the labels say
visible; fully visible at n+5 where the labels say hidden.**

🎯 **(A) CANNOT PRODUCE (B), AND THAT IS THE LOAD-BEARING SEPARATION: history blending cannot hold a
DISABLED object at FULL opacity, nor erase an ENABLED one.** A smear moves opacity toward its
neighbours; it does not invert presence. So the two are on different axes and must not be collapsed.

### ✅ Overlay semantics — RESOLVED FROM CODE. Only the anchor is open.

**RED = the frame IS in `annotation.json`'s `frame_indices`. AMBER = the frame has a `labels.jsonl`
row for that fire and is NOT in `frame_indices`** (`overlay_watcher.py:18-19`; the plugin says the
same from the other side at `AnomalyCaptureSubsystem.cpp:3281-3286`).

🎯 **A ONE-FRAME DRIFT BETWEEN THE TWO IS IMPOSSIBLE BY CONSTRUCTION.** Both are stamped from the
**same `Snap->SessionIndex`**, one line apart — the labels row at `:1917`/`:1919`, the
`frame_indices` accumulation at `:1921-1922`. There is no second counter and no second stamping
site. ⇒ Red and amber are **two readings of one comparison**, not two artifacts, so *"the two
artifacts disagree"* is **not available as an explanation.**

⚠ **THE ONLY OPEN ITEM IS WHICH EVENT ANCHORED THE OWNER'S `n`** — the red set is `frame_indices` by
definition, so the reported `{n+1, n+2, n+5, n+6}` against the recorded cadence
`{n, n+1, n+5, n+6}` is an anchor question, not an artifact question. **`C-3(c)` prints the array and
settles it.** ⛔ **Never by re-asking the owner.**

🚨 **AND THE CONSEQUENCE IS LOAD-BEARING: THE OVERLAY READING ALONE EVIDENCES (B), IN BOTH
DIRECTIONS, INDEPENDENT OF THE ANCHOR QUESTION.** A **RED** box sits on `n+5`, which the eye saw
**FULLY VISIBLE** — a frame the labels call hidden. An **AMBER** box sits on `n+3`, which the eye saw
**FULLY GONE** — a frame the labels do not call hidden. **Both directions, from the overlay alone,
whatever `n` is anchored to.** ⇒ **(B) does not depend on the transcription being right.**

### The owner's config test, recorded with what it did and did not reach

`r.AntiAliasingMethod 2` · motion blur `0` · FSR3 upscaler off ⇒ **no change.**

⚠ **Recorded with its scope: method `2` is `TAA`, which RETAINS HISTORY** (`SceneView.cpp:209-218`:
`0` off · `1` FXAA · `2` TAA · `3` MSAA · `4` TSR, engine default). **FSR3 frame interpolation is a
SEPARATE switch from the FSR3 upscaler.** ⇒ **THE TEST MOVED FROM ONE TEMPORAL METHOD TO ANOTHER AND
NEVER REACHED A NON-TEMPORAL STATE** (`0` or `1`). ⛔ It therefore does **not** clear temporal
accumulation for (A). **Decisive leg queued: RDP card `C-1`.**

### 🔺 THE MENU-OFF TEST — 2026-09-02, owner. **(A) IS CLOSED. (B) SURVIVES, CLEAN.**

**All anti-aliasing and motion blur disabled via the title's own settings menu**, then the same read:

1. ✅ **THE PARTIAL-OPACITY FRAMES ARE GONE.**
2. 🔴 **THE DISPLACEMENT PERSISTS**, and is now a **clean binary** read:
   **observed hidden `{n, n+1, n+2, n+6}` vs claimed `{n, n+1, n+5, n+6}`.**

📌 **That is the SAME STRUCTURE as the original transcription** — `{42,43,44,48}` observed against
`{42,43,47,48}` claimed maps onto it exactly at `n = 42`. The first report and this one describe one
phenomenon.

**(A) BOUNDARY SMEAR — ✅ CLOSED. ATTRIBUTED TO TEMPORAL ACCUMULATION**, by `C-1`'s **pre-declared**
discriminator: *partials vanish ⇒ (A) is temporal accumulation on Bates' pipeline.* They vanished.

⚠ **CAVEAT, RECORDED RATHER THAN GLOSSED: the settings were changed through the title's MENU, so the
EFFECTIVE cvar values were never captured.** The attribution rests on the menu labels doing what they
say. **`C-3(e)` collects the actual values for the record.** ⛔ Not a reason to reopen (A); a reason
not to call it measured to the cvar.

⚠ **AND ONE EARLIER READING IS SUPERSEDED:** the AA-on ladder recorded **`n+3` still gone**; the AA-off
binary read has **`n+3` VISIBLE**. **The difference is filed under (A)** — that frame's darkness was
smear, not absence. **Both readings are kept.**

**(B) PHASE DISPLACEMENT — 🔴 STILL OPEN**, and now cleaner than when it was found: **deterministic
across events, present with AA ON and with AA OFF.**

### 🎯 THE AXIS TABLE FOR (B) — what is excluded, and what is left

| axis | status | on what evidence |
|---|---|---|
| **AA / temporal method** | ⛔ **EXCLUDED, BOTH DIRECTIONS** | bench runs **TSR ON** (measured: `r.AntiAliasingMethod = 4`, LastSetBy Constructor) and reads **16/16 ALIGNED**; Bates is **displaced with AA ON *and* AA OFF** |
| census + mask | ⛔ EXCLUDED | the no-flags reproduction — census OFF, mask OFF |
| delivery mode | ⛔ EXCLUDED | v1 leg **C**, `delivery_mode = True`, identical structure to leg A |
| pacing | ⛔ EXCLUDED | both pacings ALIGNED on the bench; Bates is paced |
| tick ratio | ⛔ EXCLUDED | closed as a non-finding — 1.3556 reproduces exactly here |
| letterbox / non-zero view-rect origin | ⛔ EXCLUDED | the v2 fixture **is** letterboxed and reads ALIGNED |
| 🔴 **Bates host build + content** | **REMAINING** | everything else is eliminated |

### TRANSITION ARITHMETIC — arithmetic of the observed sets only. ⛔ NO MECHANISM.

**Four flips per event.** Writing the claimed set `{n, n+1, n+5, n+6}` and the observed set
`{n, n+1, n+2, n+6}` as transitions:

| flip | labels say | eye says | |
|---|---|---|---|
| 1 — **first hide** | at **n** | at **n** | ✅ **match** |
| 2 — mid-event show | at **n+2** | at **n+3** | ⚠ **+1 late in pixels** |
| 3 — second hide | at **n+5** | at **n+6** | ⚠ **+1 late in pixels** |
| 4 — **final show** | at **n+7** | at **n+7** | ✅ **match** |

Three readings follow from that and nothing else:

- 🎯 **THE OUTER PAIR MATCHES EXACTLY, ON BOTH OWNER OBSERVATIONS (AA on and AA off).**
- ⚠ **THE INTERIOR PAIR IS EACH +1 FRAME LATE IN PIXELS.**
- ✅ **HIDDEN-FRAME COUNT IS CONSERVED — 4 against 4 ⇒ NO DROPPED FRAMES.**
- ✅ **OUTER-PINNED ⇒ NO WHOLE-EVENT OFFSET** (which is also why this was never `P1`).

⛔ **WHICH CODE PATH DRIVES EACH FLIP IS THE NEXT SESSION'S SOURCE READ, NOT THIS ENTRY'S CLAIM.**

### Localization — arithmetic only, no mechanism

**Labels are host-independent** (the same (n, n+1, n+5, n+6) cadence on both hosts).
**Pixels are host-dependent** (bench **16/16 ALIGNED**; Bates **100 % displaced**).
⇒ **THE DISCRIMINATOR LIES ON THE HOST PATH FROM "visibility set on tick t" TO "pixels in captured
frame f".** ⛔ That is a statement about where to look, **not** a mechanism, and it names no step.

> 🔻 **SUPERSEDED BY `C-3`, 2026-09-02 — read the block below before acting on the line above.** `C-3`
> measured the toggle call itself and found **code == eye, frame for frame, on Bates**. ⇒ **the path
> from "visibility set on tick t" to "pixels" is CLEAN on both hosts**; the divergence is between the
> **toggle call and the LABEL SAMPLE**, i.e. one step earlier than this line points. ⛔ **The line is
> kept, not deleted — it was the correct reading of the evidence that existed when it was written.**

### What the bench campaigns are still worth

v1 and v2 stay recorded exactly as taken. They established the **matched-axis clearance**: with
letterbox, census, pacing, tick ratio and delivery all matched or excluded, the bench's labels and
pixels agree 16/16. ⇒ **None of those axes is the discriminator**, which is what makes the
localization line above sayable at all.

### 🔴🔴 THE `C-3` RESULT — Bates, owner, 2026-09-02, run at 13:57. **THE DIVERGENCE IS LOCATED.**

> 🚨 **`C-3` IS DONE AND IT ANSWERED.** The pre-declared three-way comparison fired its **row 2**. The
> toggle log **sides with the EYE on both interior flips**; the **labels are the outlier**. ⛔ **NO
> MECHANISM IS ASSERTED HERE.** What is recorded is a measurement plus the arithmetic that follows
> from it. Source read at journal 068 §8; the fix options are a PLAN only, journal 068 §9.

**The bundle, as measured.** Prefix **PRESENT** (`[timestamp][GFrameCounter%1000]`), so the exact-frame
join was available and no fallback was needed. ⚠ `SVE-WANT-TRACE arm … gameFrame=` returned **0 hits**
on that build — the backup anchor does not exist there; **its absence is expected and is not a fault.**

| what | value |
|---|---|
| apply line `blinking: matched` | `[35]` |
| toggle lines | **HIDDEN `[38]` · VISIBLE `[41]` · HIDDEN `[44]`** (~100 ms apart ⇒ paced 30 fps, half-period 3) |
| `session_index → frame_index` | 26→30034 · **27→30035** · **28→30038** · 29→30039 … 35→30045 · 36→30048 |
| `frame_indices` (labels) | **`{28, 29, 33, 34}`** — the certified `(n, n+1, n+5, n+6)` cadence at `n = 28` |
| eye, AA OFF | **gone `28, 29, 30, 34`** · visible `27, 31, 32, 33, 35, 36` |
| overlay | **AMBER on 27** (`n−1`) · **NO box on 35** (`n+7`) |
| event 1 of the same run | apply `[3]`, toggles `[6] · [9] · [12]` — the same pattern |

**THE JOIN, and it is exact:**

| flip | toggle tick | = `frame_index` of | eye |
|---|---|---|---|
| 1 — first hide | `30038` | **`session_index 28` = `n`** | gone from 28 ✅ |
| 2 — mid-event show | `30041` | **`31` = `n+3`** | visible from 31 ✅ |
| 3 — second hide | `30044` | **`34` = `n+6`** | gone at 34 ✅ |
| 4 — final show | revert on `n+7`'s tick | **`35`** | visible at 35 ✅ |

⇒ **CODE == EYE, frame for frame. LABELS `{28,29,33,34}` ARE THE OUTLIER.**
✅ **Two free cross-checks from journal 068 §1 both PASSED on Bates: AMBER on `n−1`, and NO box at
`n+7`.** The label pipeline's *shape* is intact on that host; only its *phase* differs.

**THE ARITHMETIC — the apply tick is not counted.** ⛔ Arithmetic only.

- The model (journal 068 §1.1.1) predicts the injector counts the **apply tick itself**, so with
  `half-period 3` the first toggle lands on `apply + 2` — here `30035 + 2 = 30037`, an **uncaptured
  settle tick**.
- **Measured on Bates: `30035 → 30038` = `apply + 3`.** The injector counted ticks **30036, 30037,
  30038 — NOT the apply tick.** Same on event 1 (`[3]` → `[6]`).
- ⇒ **all three toggles are exactly ONE TICK LATE relative to the arm**, which moves flip 1 off the
  uncaptured settle tick and **onto `n`'s own arm tick**.

🎯 **AND THE BENCH IS THE OTHER HALF OF THE SAME MEASUREMENT — SAME LINE PAIR, SAME CONFIG, OPPOSITE
ANSWER.** From the `m38` gate-(v) session (`M38_G5_VERBOSE\session_20260902-183933`, `run.json`
`settle 2 / pre 4 / positive 8 / post 4`, `frame_cap 90`, targeted `blinking` — **byte-for-byte
`C-3`'s config**), whose `anomaly_log.txt` carries the toggle lines *with* the prefix:

> **apply → first toggle = `+2` on ALL EIGHT bursts of the bench run, against `+3` on Bates.**

| burst | `n` | apply tick | = `frame_index(n−1)` | first toggle | on | second toggle | = `frame_index(n+2)` | third toggle | = `frame_index(n+5)` | revert | = `frame_index(n+7)` |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 1 | 4 | 4 | ✅ 4 | 6 | **uncaptured settle tick** | 9 | ✅ 9 | 12 | ✅ 12 | 14 | ✅ 14 |
| 2 | 16 | 20 | ✅ 20 | 22 | ✅ uncaptured | 25 | ✅ 25 | 28 | ✅ 28 | 30 | ✅ 30 |
| 3 | 28 | 36 | ✅ 36 | 38 | ✅ uncaptured | 41 | ✅ 41 | 44 | ✅ 44 | 46 | ✅ 46 |
| 4 | 40 | 52 | ✅ 52 | 54 | ✅ uncaptured | 57 | ✅ 57 | 60 | ✅ 60 | 62 | ✅ 62 |
| 5 | 52 | 68 | ✅ 68 | 70 | ✅ uncaptured | 73 | ✅ 73 | 76 | ✅ 76 | 78 | ✅ 78 |
| 6 | 64 | 84 | ✅ 84 | 86 | ✅ uncaptured | 89 | ✅ 89 | 92 | ✅ 92 | 94 | ✅ 94 |
| 7 | 76 | 100 | ✅ 100 | 102 | ✅ uncaptured | 105 | ✅ 105 | 108 | ✅ 108 | 110 | ✅ 110 |
| 8 (cap-truncated) | 88 | 116 | ✅ 116 | 118 | ✅ uncaptured | 121 | **no captured frame** | — | — | 122 | — |

🚨 **THE BENCH MATCHES JOURNAL 068 §1.1.1 EXACTLY, ON EVERY FULL BURST — and Bates does not. Same
plugin, same config, same two log lines; the ONLY thing that differs is whether the apply tick is
counted.** ⇒ the loud alternative is **excluded by measurement**: the bench does **not** show
`n / n+3 / n+6`, so this is not a plugin-wide property that the bench had simply never been asked
about.

📌 **Journal 068 §6 (`R1`) predicted this shape before the bundle arrived, and it is why the result is
readable at all: `frame_indices` is EXACTLY invariant under a one-tick shift of the toggle relative to
the arm, while the PIXELS move by one frame, and the two OUTER flips are insensitive** (flip 1 lands
on an uncaptured tick either way; flip 4 fires inside the capture `Tick` either way). **That is
precisely the observed signature — outer pair matches, interior pair each +1 late in pixels,
hidden-frame count conserved, labels host-identical.** ⇒ **the labels being host-identical was never
evidence that the label path is healthy; it is what a shift of this family predicts.**

**RESTATED IN `R5`'s TERMS, which is how it must travel.** The pre-declared row 2 said *"the
SAMPLING / LABELLING side is the outlier"*. **That naming is superseded** (journal 068 §6.5): the
divergence lies **in the INTERVAL BETWEEN THE TOGGLE CALL AND THE LABEL SAMPLE**, and on this
evidence the interval opens at the **apply tick / toggle-relative-to-arm** end, not inside the
sampling code — the deferred sampler at `AnomalyCaptureSubsystem.cpp:591` behaves identically on both
hosts and is order-compensating by construction. ⛔ **Do not write "the sampler is broken on Bates".**

**What this closes, and what it does not.**

| | |
|---|---|
| ✅ **PIXELS, AA, capture path, overlay semantics** | **CLEARED.** Code and eye agree frame for frame; the overlay's `n−1` / `n+7` checks both pass |
| ✅ **the divergence is LOCATED** | between the toggle call and the label sample, at the toggle-relative-to-arm end |
| 🔴 **WHY the apply tick is counted here and not there** | **OPEN — this is the tick-order question.** Source read: journal 068 §8. ⛔ **"consistent with", never asserted** |
| ⛔ **no mechanism is entered in the axis table above** | the `Bates host build + content` row stands as written |

📌 **`(B)` IS NO LONGER "OPEN WITH NOTHING TO LOOK AT". It is open with a located interval, a measured
two-host discriminator (`apply → first toggle`: bench `+2`, Bates `+3`) that costs two log lines to
read on any host, and a fix plan awaiting a ruling.**
⛔ **Nothing further is needed from Bates for `P9` until a fix build exists** — the box is sealed and
`C-3` got everything the card asked for.

### 🏁 FIX LANDED — **`m40`**, 2026-09-02. **BATES VALIDATION PENDING.**

> ✅ **BUILT, GATED AND SHIPPED.** Staged bench exe **`C0AD3F91`**; container unchanged, no cook.
> 🎯 **`P9` (B) IS NOW REPRODUCIBLE ON THE BENCH AND THE FIX REMOVES IT.** With the bench-only lever
> ON against the SHIPPED sampler, leg `L2` read **`P9-SHAPE` on 7 of 7 counted events** with the
> **exact Bates sets** — claimed `{n, n+1, n+5, n+6}` against observed `{n, n+1, n+2, n+6}`, missing
> `[n+5]` / extra `[n+2]` — and `apply → first toggle` at **`+3`**. With the fix and **the lever still
> ON**, leg `L3` read **7/7 ALIGNED** at `{n, n+1, n+2, n+6}` while the toggle lines **still read
> `+3`** ⇒ **the LABEL SAMPLE moved, not the toggles.** Leg `L4` (fix, lever OFF) is
> **byte-identical** to the pre-fix control: `frame_indices` identical, `labels.jsonl` 0 row
> differences, `P6` 48/48.
> ⛔ **NO MECHANISM IS ASSERTED FOR BATES. The lever synthesises the SYMPTOM by a different mechanism
> (a delegate), so "consistent with, not asserted" — the language of this entry — STANDS UNCHANGED
> until an `m40` build is validated on that host.**
> ✅ **`m40` LANDED ON THE BENCH (`0864e7a`, exe `C0AD3F91`) AND IS NOW VALIDATED ON BATES
> (2026-09-02, card `SECTION D`) — `D-0` PASS on the STRONG ROW.** Pre-declared pass condition was
> **`frame_indices` equals the eye whether `apply → first toggle` reads `+2` or `+3`**; it read
> **`+3`**, so the disorder is still present there and the labels are right anyway. **See the
> closure block at the head of §8.6a.** ✅ **The `blinking` mitigation on Bates is LIFTED.**
> ⛔ **`IAI.Bench.SynthTickOrder` is still NEVER typed on a host.**
> 🧭 Implementation and all leg results: journal 068 §11.

### ✅ FIX APPROVED — option 2, milestone **`m40`** (2026-09-02). *(the plan, kept as the record)*

**`m40` = order-independent label sampling.** The per-frame label's active bit moves from *"the top of
the next capture `Tick`"* to **`FWorldDelegates::OnWorldTickEnd`** (`LevelTick.cpp:1814`) — **after
every tickable (`:1606`) and still before the draw** — so the sampled bit is what the renderer will
draw for that same frame **whatever order the subsystems ticked in**.
🎯 **It changes NO rendered pixel on any host, adds NO artifact field and NO client-facing setting**,
and it **removes** the dependency rather than pinning it. ⛔ Pinning the tick order was considered and
**rejected**: it would change what one host renders (journal 068 §9).
🧭 **Plan: journal 068 §10. Pre-declared gates:
`docs/predictions/2026-09-02-m40-order-independent-label-sampling.md`.**
🔑 **Prove-it-can-fail is a BENCH-ONLY, DEFAULT-OFF lever** (`IAI.Bench.SynthTickOrder`, console only,
no ini key, echoed at `StartRun` whether on or off) that relocates the injector's dispatch to
`OnWorldPreActorTick` and so **synthesises the SYMPTOM — not the cause — of the other order.** Four
pre-declared legs: control · **bench reproduction of `P9` (B)** · fix-with-lever-still-on · inertness.
⚠ **The ledger's language stays "consistent with, not asserted" until an `m40` build is validated on
Bates**, and that validation's pass condition is pre-declared: **the labels equal the eye whether
`apply → first toggle` reads `+2` or `+3`.**
⛔ **KNOWN LIMITATION CARRIED BY `m40` BY DECISION:** the **sync-fallback** capture path
(`AnomalyCaptureSubsystem.cpp:2439`) samples inline and **remains one tick stale**. No gate exercises
it today and one variable at a time. **Detect it from `SVE-WANT-SUMMARY`: `marksIssued` below
`framesWritten`.** ⚠ Its own fallback notice is on `LogAnomalyCapture`, so a `LogAnomaly`-only Verbose
run will not show it.

---

## 8.6b 🔻 BENCH CAMPAIGN RECORD — superseded as the close-out, kept as the measurement

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

⚠ **SUPERSEDED AS THE "WHAT NEXT" LINE.** This block said the next evidence was a typed read. The
owner has since **reproduced `P9` directly on Bates with no flags** and measured the opacity ladder
— see **§8.6a**, which is the current entry. The next evidence is the **`C-1` AA-off leg**, and the
typed bundle now rides `C-3`.

## 8.7 Related entries

| | |
|---|---|
| **`P1`** | client's one-frame shift at ratio ≈ 1.2, 30 fps — **OPEN, never reproduced.** A **constant** shift; §8.2 says why `P9` is not it. |
| **`P3`** | a labelled hide that never manifests — **FIXED at `m23`.** Same family (labelling vs manifestation), already cured, and the reason the `manifested` flag and the zero-positive-frame guard exist. |
| **`P5`** | single-frame alignment undecidable ≥ 90 fps — **queued.** ⚠ Bates ran at **30 fps**, so `P5` is **not in play here unless a leg measures it into play**. |

---

# 9. `m41` — three entries the census work put on this ledger (2026-09-03, session 069)

## 9.1 ⛔ PERSIST-TAGS: the "TSR shimmer" motivation is **NOT SUPPORTED** — correction, not a finding

The named optimisation **PERSIST-TAGS** (keep candidates tagged across census batches and rotate
stencil VALUES in place, instead of flipping `bRenderCustomDepth` per batch) was, during `m41`
planning, promoted from a **cost** item to a **correctness** item on this reasoning: *"each tag flips
the render proxy → motion vectors reset for a frame → a one-frame TSR ghost/shimmer."*

**That mechanism does not survive a 5.1 source read.**

- ✅ **The cost half is CONFIRMED.** `UPrimitiveComponent::SetRenderCustomDepth` →
  `MarkRenderStateDirty()`, **and only when the value actually changes**
  (`PrimitiveComponent.cpp:4075-4082`) — a full render-state recreate.
  `UPrimitiveComponent::SetCustomDepthStencilValue` → `SceneProxy->SetCustomDepthStencilValue_GameThread`
  (`:4084-4097`) → an `ENQUEUE_RENDER_COMMAND` writing **one scalar** on the proxy
  (`PrimitiveSceneProxy.cpp:964-984`). **No recreate.** Rotating values really is cheap; flipping the
  flag really is not.
- ❌ **The correctness half is NOT SUPPORTED.** Previous-transform / velocity state lives in
  **`FSceneVelocityData`**, keyed by **`FPrimitiveComponentId`** — stable across a recreate — and the
  class's own comment reads: *"Tracks primitive transforms so they will be persistent across rendering
  state recreates."* (`ScenePrivate.h:2410-2413`; `GetComponentPreviousLocalToWorld:2425`,
  `UpdateTransform:2443`). `UpdateTransform` is additionally guarded by
  `check(Proxy->HasDynamicTransform())` — **most census candidates are STATIC and have no velocity to
  lose in the first place.** And `m26`'s banked `F-1` finding already established that the deferred
  recreate is flushed **inside the same frame's** `BeginRenderingViewFamilies`, so the primitive is
  **not missing** from the frame.

⛔ **This is NOT a refutation of the SYMPTOM.** GPU-Scene and static-draw-list churn on a recreate are
real, and their per-frame pixel effect **has never been measured here**. Per the observation-vs-mechanism
invariant (`G120`): **the concern stands as UNMEASURED; the mechanism does NOT stand as stated.**

🎯 **CONSEQUENCE — `m42` IS MEASUREMENT-FIRST.** Its first task is not an implementation: *does a census
flag flip on a captured frame move a pixel?* Two legs on **one** binary, census-ON vs
census-ON-with-flips-suppressed, at a matched pose, read with the grid/luma instrument under `G125`'s
marker discipline (strict cross-run byte identity is known-unobtainable here). ⛔ **Building a
tag-lifetime redesign on an unmeasured correctness premise is the `ReservedStencilMax` mistake in a new
place.** Two further reasons `m42` is not `m41`: it reverses `m36`'s closed tag-lifetime ruling (with
tags persisted, `IsAnyComponentTagged` — the `HeldElsewhere` guard — degenerates to always-true), and it
makes §9.3's hazard **permanent instead of intermittent**, so **§9.3 must read `= 0` on the target host
before `m42` is even safe to consider.**

## 9.2 🔴 OPEN — the fog-card actor on Bates: something on it draws into custom depth, and we do not know what

**Owner observation (eyeball-level, real, NOT explained):** with the census and mask ON, an actor whose
material is **surface-translucent WITHOUT custom-depth writes** was nonetheless **selected and
annotated**, and nothing visible changed. The owner is mitigating **by name exception** for now.

⚠ **That combination should not happen under `m41`'s rule.** A translucent-only candidate is
`EXCLUDED(translucent)` regardless of the opt-in, and a candidate that writes no custom depth cannot be
measured non-zero by the mask. **So something on that actor IS drawing into custom depth** — a second
component, a second material slot, an opaque or masked slot alongside the translucent one, or a writer
we have not enumerated.

⛔ **NO MECHANISM IS CLAIMED, and none should be written into a brief before it is measured.** The
candidate list above is a list of things to look at, not an explanation.

🔎 **THE READ THAT SETTLES IT, and it is small:** the actor's **`DRAWN-COVERAGE` histogram entry** (is it
`MEASURED_NONZERO`, and at what drawn %?) **plus its component / material-slot list**. Those two
together say whether the census measured a real silhouette and which slot produced it. Card `SECTION E`
carries it with **no expected value stated**, deliberately — an expected value here would bias the read
of the one observation we have.

📌 **Recorded OPEN.** It is not `H5`, not `H6` and not `P9`; it is a census-selection question about one
actor class on one host.

## 9.3 🚨 THE HOST POST-PROCESS HAZARD, and the defect found while instrumenting it

**The hazard (why the preflight exists):** the census tags candidates with **custom depth on captured
frames**. A host post-process that **reads** `CustomDepth` or `CustomStencil` would therefore tint or
outline census-tagged objects **in frames the labels call CLEAN** — an unlabelled artifact that **no
counter can see**. Bates is known to write host custom stencil on some actors, so such a reader is not
hypothetical.

**The instrument:** at `StartRun`, with the census effective, enumerate the engine's **three** own
post-process sources (`LocalPlayer.cpp:866-881`) — volumes · the camera manager's cached blends ·
**the view target's `// CAMERA OVERRIDE`** — resolve each blendable material through its **serialized
`UsedSceneTextures` bitmask** (`FMaterialShaderMap::UsesSceneTexture`, works in a cooked build, no
editor-only data) and print one loud line either way, carrying its own `scanned` counts.

🚨 **THE DEFECT, AND HOW IT WAS CAUGHT.** The first cut scanned only sources 1 and 2 and **missed the
CAMERA OVERRIDE — which is where a `UCameraComponent`'s `PostProcessSettings` actually arrive, i.e. the
most ordinary way a host applies a full-screen effect.** It was caught because the gate demanded
`scanned` counts alongside the verdict: a `= 0` with `scanned 0/0/0` was pre-declared as **BLINDNESS,
NOT A CLEAN READ**, the gate failed, the campaign stopped, and the diagnosis found the missing source.
📌 **A preflight printing only `READERS = 0` would have been green on every level and would have shipped
it.** ⇒ **`G96` is usually about proving a detector CAN fire; here it caught a detector looking in the
wrong place.**

**Stated limits — these travel with every `= 0`:** it detects a material that **samples** the texture,
not that the sample changes a pixel; it **cannot see a reader outside the material system** (a host
scene-view extension, a custom pass, Niagara, UMG, a decal); it is a **`StartRun` snapshot**, so a
blendable added later is missed. ⛔ **`N = 0` DOES NOT MEAN "nothing on this host reads custom depth."**

**Watch item for a real host (`D-G1`'s residual):** census verdict freshness is
`max(knob, lastCompletedCycleTicks + LostAfterTicks(8))`. On a synthetic 41–47-tick cycle the `8` margin
does **not** fully cover cycle-to-cycle variance, and some verdicts still expire. **Deliberately NOT
tuned on a bench regime.** ⇒ **if a real host reports `expired > 0` with `window > 12` on the
`Auto.Fire: census consulted=` line, that margin is the first knob to look at.**

---

# 10. The shared mask pass — a latent defect in shipped `m41`, and `m43`'s limitations

**2026-09-03, session 069.** Found while building `m43`; **it is a defect in code that was already
delivered**, not in the new feature.

## 10.1 🚨 `m26`'s ARMS WERE STARVED BY THE CENSUS THROUGH A ONE-SLOT FIFO

The `m26` visible-mask pass renders **once per frame** and consumed **exactly one** pending arm
(`RequestId = PendingArms[0]`, `AnomalyMaskSceneViewExtension.cpp`). Since `m41` shipped the census ON
by default, two consumers shared that slot: `m26`'s ≤4 arms per event and the census's ~0.8 arms per
frame.

**Measured, per arm, as the tick armed vs the tick the pass served it (the `M23 PASS` line's own
`[GFrameCounter % 1000]` prefix):**

| leg | census arms | `m26` arms | served | **UNSERVED** | lag min/max/mean |
|---|---|---|---|---|---|
| `m41`, census **ON** (the shipped default) | 78 | 24 | 22 | **2** | 1 / **3** / 1.86 |
| `m41`, census **OFF** | 0 | 24 | 24 | 0 | 1 / 1 / 1.00 |
| `m41`, census **OFF** (second leg) | 0 | 24 | 24 | 0 | 1 / 1 / 1.00 |
| **`m43`**, census **ON** | 97 | 24 | **24** | **0** | **1 / 1 / 1.00** |

🔑 **The census-OFF pair is the control and it is clean: with no census every arm is served on the very
next render. With the census ON, 2 of 24 arms were NEVER SERVED and the rest ran up to 3 frames late.**

⇒ **`framesContributed` — an input to the `m26` measurement and therefore to the ZERO-ONLY VETO — was
coupled to census cycle length.** Measured effect on one event: `StaticMeshActor_49@116`
**`framesContributed` 2 → 4** once fixed.

⚠ **LATENT. NO VERDICT WAS EVER OBSERVED TO CHANGE.** Both `m41` legs produced identical event sets and
`vetoed_events` 0, and every event read `MEASURED_NONZERO` on both. ⛔ **"The veto was wrong" is NOT
established and must not be written.** What is established is the coupling.

**The fix (`m43`): one render serves every pending arm.** Semantically exact rather than a
prioritisation — the RT's content depends only on which actors are tagged at render time, and each
consumer already filters the result by its own tag set, so one render is the *same answer* delivered to
each asker. Verified: **max one mask pass per frame across 103 frames**, `servedArms` 1/2/3.

📌 **Why it was invisible for two milestones:** nothing counts arms against passes. Every counter the
mask keeps is per-event or per-batch, and a starved arm simply never appears. It surfaced only because a
third consumer made the contention bad enough to notice. → `G217`.

## 10.2 `m43`'s named limitations

- **Anomaly targets only** — not a mask of every object in the scene.
- **Translucent-only targets never appear** (excluded at selection, and cannot write custom depth);
  **Nanite targets never appear** (`G134`), the same blind spot the `m26` measurement has.
- **Multi-target frames UNVERIFIED** (`D7`) — no leg has yet shown two distinct non-zero values in one
  PNG. Client docs say "one value per anomaly target present in the frame".
- **Output height ≠ 0 refuses outright** — a label mask must never be filtered. Nearest-neighbour mask
  resampling is a named follow-up, not built.
- ⚠ **Per-frame tag/restore churn on a live target** queues a deferred render-proxy recreate.
  **Measured `tagFlips = 0` on every bench leg** — `m26` already had the live target tagged whenever the
  target mask armed — so the churn is **zero on this bench**, and **its effect on pixels is UNMEASURED,
  not shown harmless**. **`m42` (persist tags, rotate values in place) is its fix and is
  measurement-first** (see §9.1: `m42`'s own TSR-shimmer motivation is NOT SUPPORTED by 5.1 source and
  stays unmeasured).

## 10.3 Census `tagOvertaken` — PASS-WITH-READING, and a mechanism of mine refuted

With the target mask ON the census's `tagOvertaken` reads **1** where `m41` read **0**. Attributed by
control: **the same `m43` binary with the target mask OFF reads 0.** Everything the counter guards
stayed put — `framesPolluted` **0**, `batchesLost` **0**, cycle histogram identical
(`zero=13 nonzero=64 belowFloor=49`), verdict set identical.

⚖ **Ruled PASS-WITH-READING (`P-C2` precedent):** the counter is the one the census built for exactly
this class, its own text calling a re-tag by the event mask *"the expected case"*. The gate predicate
*"unchanged or lower"* was **over-strict for an observation counter**; the corrected predicate is
*"`tagOvertaken` may rise; `framesPolluted`, `batchesLost` and the verdict set must not move."*

⚠ **I attributed the rise to the target mask's tag/restore cycle opening windows. The `tagFlips` counter
added afterwards reads 0 on every leg, so THAT MECHANISM DOES NOT STAND** — the perturbation comes from
the extra arms changing census batch timing. **Recorded as refuted rather than dropped** (`G120`).

---

# §11 — `m44`: THE TARGET MASK'S ONSET, AND WHAT THE FOUR BATES OBSERVATIONS BECAME

**2026-09-03. Shipped on `master` as `m44`.** Journal 069 §7–§12.

## 11.1 The four observations, and where each one went

| | observation | outcome |
|---|---|---|
| **O1** | first mask frame is one after the labelled frame | ✅ **FIXED** — tag ownership (§11.2). Gate `G1` 4/4 delta 0, both tick orders |
| **O2** | hidden-class frames carry no mask | ⛔ **NOT FIXED — it is `m45`.** Those frames are now honestly `mask_state: "unmeasured"`, and the client docs say so in as many words |
| **O3** | blank PNGs everywhere | ✅ **FIXED** — a file exists iff it has content; 61 of 90 blanks became 0 |
| **O4** | CorruptedTexture's effect starts at `n+1` | ❌ **NOT REPRODUCED** — the picture already differs at `n` by 5.9–8.2 % against a ~0.5 % baseline. Shipped as **documentation**: temporal AA settles over the following frames, so the first labelled frame can look subtle while the pixels have already moved |

⚠ **A fifth defect was found that nobody reported:** masks were being written on frames the labels call
clean (`blinking`'s visible in-between frames). Gate `G7` now forbids it; 0 stray in both orders.

## 11.2 The mechanism, in one sentence, with its numbers

**An actor under a live fire belongs to its event.** `ArmTargetMaskOwn` tested
`IsAnyComponentTagged` and treated "somebody has tagged this" as "it is tagged for me", so on an
event's first labelled frame the target could still carry a foreign stencil value while the reduce
filtered on the event's own tag. `m26`'s `ArmIfMeasurable` never had the hole — it asserts its value
unconditionally. **Two consumers of one shared attribute, only one of them asserting ownership**
(`G227`).

**Measured, on exactly the four `+1` events and no other armed frame (4 of 27):**

| session_index | event tag | value on the actor | owner |
|---|---|---|---|
| 27 | 222 | 204 | census |
| 51 | 224 | 242 | census |
| 63 | 226 | **224** | the previous event on that same actor |
| 87 | 229 | **226** | the previous event on that same actor |

⇒ **two independent sources.** Turning the census off cured 2 of 4 and would have shipped a half fix.

## 11.3 ⛔ THE STENCIL POOL WAS NOT PARTITIONED — do not re-propose it

Splitting the pool into disjoint census and event ranges was considered and **refused with numbers**:
the assignable range is **55 values (`200..254`)** and the census tagged **77 candidates in a single
90-frame leg**. A split trades a fixed bug for **tag exhaustion**. Ownership is the fix; capacity was
never the problem.

## 11.4 `census tagOvertaken` 0–1 → 2–3 — PASS-WITH-READING

The `m43` gate-`D` precedent, cited deliberately. It is the ownership rule **made visible**: the target
mask now takes back an actor the census had tagged, and it lands in the counter the census built for
exactly this class. `framesPolluted 0`, `batchesLost 0`, the verdict histogram, the event set, every
`manifested` and `positive_frames` all unchanged. ⛔ **Not "the census got worse".**

## 11.5 Three refuted hypotheses, one line each — every stop was correct

- **`IAI.Bench.SynthTickOrder`** — refuted: the `+1` is identical in both orders, with the lever proven
  engaged (it moved the blink hidden set).
- **The missing frame handshake** — refuted: `PREVIOUS = 0` on every decidable frame of four legs and
  `r.OneFrameThreadLag 0` changes nothing. ⚠ **The gap is real and is deferred as hygiene** — the mask
  SVE has no frame key where the capturer has one (`m31`).
- **Internal-vs-output resolution** — refuted *as this cause* (rects equal on all 51 passes at the
  bench default) but **CONFIRMED as a separate real defect**: at `r.ScreenPercentage 50` the probe
  reads 0 correct of 26. Fix built and unvalidated on `m44-f1-resolution-mapping-UNVALIDATED`,
  pending a cook. **Until then masks, the census and `m26` are correct only at 100 % screen
  percentage** (`G225`).

## 11.6 And one finding was retracted — it was the instrument (`G226`)

A probe using stencil tag `250` (inside the allocator's range) and the same magenta material
`corrupted_texture` swaps to produced a confident, detailed and **entirely false** *"the mask carries
content the picture does not on a quarter of frames"*. Corrected probe: **40/40 correct, both orders.**
---

# §12 — `m45`: HIDDEN-CLASS MASKS. What shipped, and the two levers it took to prove it

**2026-09-03, on `master`.** Journal 069 §13–§15.

## 12.1 What shipped

`blinking` and `missing_object` no longer call `SetActorHiddenInGame`. They drop the target from the
**main and depth passes** and silence shadows, Lumen, distance fields, ray tracing and decals, **while
keeping `bRenderCustomDepth`** — so the target still writes custom depth and the existing `m26` compare
against scene depth yields **the would-be-visible region**, occlusion-aware, with no new shader.

🔑 **The design point that is easy to miss: the labels' notion of "hidden" was `AActor::IsHidden()`.**
Stop calling `SetActorHiddenInGame` and `blinking`'s hidden set silently empties — the labels break
while the pixels stay right. A **logical-hidden registry** is now the single source of that fact for
the two label paths, `m26`'s `LOCK-1` guard and its three siblings, and the census.

**Measured:** `target_mask_frames_measured` **27 → 35** and `_unavailable` **63 → 55** — **+8 / −8, and
the eight are exactly the eight `blink` hidden frames (4+4)**. Mask files == labelled frames exactly
(`35 = 35`, stray 0). Every veto counter, the event set, `manifested` and `positive_frames` unchanged.

**`M45-G3`, the would-be-silhouette check** — hidden-frame mask against the same actor's mask while
visible, same run, camera delta exactly zero: **blinking IoU 0.9987 · missing_object IoU 0.9969**, both
tick orders. 48,590 vs 48,591 pixels.

## 12.2 The shadow lever that could not fire, and why that mattered

The first prove-it-can-fail lever omitted **shadow** silencing. It engaged (echoed in the engine log)
and the picture still did not move — **`CB_GateLevel`'s target casts no shadow that reaches the
frame**, so the fixture could not exhibit the class. ⛔ **That is not a passing gate**; it is `G135`'s
shape, and it is why `m45` did not merge on its first attempt.

**The replacement lever is fixture-independent:** `IAI.Bench.HideOmitDepthPassSilencing` leaves
`bRenderInDepthPass` true while the main pass is off, so the target **still writes the depth prepass
and occludes what is behind it while drawing nothing itself**. It fires anywhere: **20 of 60 frames,
worst 4.69 %**.

## 12.3 The arbiter, and the honest limit

| leg (AA-off, native) | frames differing |
|---|---|
| CONTROL old-vs-old | **0 of 60** — a zero floor is what makes the reading mean anything |
| TEST old-vs-new hide | **0 of 60** |
| CAN-FAIL depth-omitted | **20 of 60** |

⚠ **At the DELIVERED configuration this comparison is impossible: two runs of the same build differ by
~9 % of pixels** (`G228`). **Identity is therefore proven at the AA-off arbiter and no pixel claim is
made at the delivered configuration.** ⚠ **`SynthTickOrder` cannot host a pixel arbiter either** — its
own old-hide control differs on 60 of 60 frames (`G230`).

## 12.4 What is still not covered

⛔ **Nanite targets get no hidden-class mask** — `Nanite::FSceneProxy::GetViewRelevance` never sets
`bRenderCustomDepth` (`G134`). Unchanged by `m45` and stated in the client docs.
---

# §13 — `m46`: the mask pass maps through the internal view rect

**2026-09-03, on `master`, on a freshly cooked container.** Journal 069 §16.

The mask pass runs **after tonemap**, so `SvPosition` is in **OUTPUT** space, while the scene textures
it samples are at **INTERNAL** resolution. It sampled them with unscaled coordinates. At 100 % screen
percentage the two spaces coincide and nothing looks wrong; at any other ratio every sample lands in
the wrong place, and the region beyond the internal rect reads whatever the pooled texture still holds.

**Measured, with the banked prove-it-can-fail leg as the A-side** (`r.ScreenPercentage 50`, internal
640×360 against output 1280×720):

| | mask-picture pairing |
|---|---|
| BEFORE | **CURRENT 0 of 26 decidable, NEITHER 25** |
| AFTER | **CURRENT 35 of 35, NEITHER 0, PREVIOUS 0** |

100 % unchanged (33 of 33). ⇒ **masks, the census and `m26` are now correct under dynamic resolution,
any screen percentage and any temporal upsampler** — all ordinary shipped-game settings.

⚠ **RETIRES THE NEAREST-NEIGHBOUR MASK-RESAMPLING FOLLOW-UP.** The mapping is nearest by construction
(integer, rounded, clamped), so there is no separate resample step to add and none should be proposed.

📌 **`P-C7 v2` reading, and the control that settled it:** 5 of 35 mask silhouettes differ against the
pre-`m46` control — **and a SAME-BUILD control reproduces it exactly** (same 5 frames, same frame 52,
same 23,198-pixel symmetric difference with the two values merely swapped). It is **census run-to-run
variance, not `m46`** (`G169`). Every veto counter, `positive_frames` and the three `target_mask_*`
counters are identical.

⚠ **It could not ship with `m44` because a global shader parameter-struct change is fatal against a
stale cooked container (`G129`).** It needed a full cook, which is an owner-sequenced operation.

---

# 11. AUTO-EXPOSURE — the game's own rendering, measured, and it is NOT editor-only

**2026-09-03, session 069 brief 26 (`m47b`). Eight legs, editor AND packaged, both texture-swap
anomaly types. Pre-declaration: `docs/predictions/2026-09-03-m47b-auto-exposure.md`, written before
any leg ran. ⛔ NO SOURCE CHANGE — docs and harness only.**

This entry exists because `m47` left the owner's symptoms **(2) "target renders BLACK"** and
**(3) "whole picture black for a burst, recovers"** UNEXPLAINED, and named auto-exposure as the
cheapest untested candidate (§17.6 of journal 069). It is now measured.

## 11.1 THE STANDING ASYMMETRY — every bench leg forced AE OFF; the delivered build runs it ON

Established by reading, before any leg: the plugin never touches exposure (one log string, no code);
StackOBot's `DefaultEngine.ini` and the engine's `BaseEngine.ini` set **no exposure key at all**; so
the delivered configuration is the engine default, **`r.DefaultFeature.AutoExposure = 1`**
(`SceneView.cpp:165-170`), Histogram method, Bias 1.0, legacy luminance range, with
**`Min 0.03 < Max 8.0`** (`Scene.cpp:468-469`) — genuinely adaptive, not the `Min == Max` fake-manual
degenerate case. **Confirmed at runtime by an A48 echo in BOTH the editor and the packaged build:**
`AutoExposure = "1" LastSetBy: Constructor`, `EyeAdaptationQuality = "2" LastSetBy: Scalability`.

**And it reaches the written pixels**: eye adaptation is applied inside the tonemapper and the capture
SVE subscribes at `EPostProcessingPass::VisualizeDepthOfField` (`AnomalySceneViewExtension.cpp:72`),
which is **after `Tonemap`**. → **`G233`.**

## 11.2 THE MEASUREMENT — matched pairs, only the two cvars differing

| leg | build | anomaly | AE | lum min | lum max | spread | max dip | target lum |
|---|---|---|---|---|---|---|---|---|
| AE1 | editor | `corrupted_texture` | **ON** | 72.662 | 103.920 | **31.258** | **8.61 %** | 90.6 – 117.5 |
| AE2 | editor | `corrupted_texture` | OFF | 100.490 | 106.240 | 5.750 | 1.73 % | 123.4 – 124.7 |
| AE3 | **packaged** | `corrupted_texture` | **ON** | 72.419 | 104.439 | **32.020** | **9.01 %** | 90.5 – 117.8 |
| AE4 | **packaged** | `corrupted_texture` | OFF | 99.552 | 106.652 | 7.100 | 2.04 % | 123.7 – 128.1 |
| AE5 | editor | `missing_texture` | **ON** | 74.524 | 103.923 | **29.398** | **7.27 %** | 115.7 – 149.3 |
| AE6 | editor | `missing_texture` | OFF | 102.739 | 106.240 | 3.501 | 0.97 % | 148.4 – 149.9 |
| AE7 | editor | `corrupted_texture`, 40-frame window | **ON** | 73.094 | 103.924 | **30.830** | 8.61 % | 90.8 – 117.5 |
| AE8 | editor | `corrupted_texture`, 40-frame window | OFF | 100.138 | 106.240 | 6.103 | 2.39 % | 123.4 – 173.1 |

`AE-LIVE` positive control (`G96`) **PASSES decisively** on the matched packaged pair: mean luminance
**77.907 vs 102.488**, spread **32.020 vs 7.100**. The AE-ON "dip" readings are evidence, not blindness.

🔑 **The dominant effect is a SESSION-START CONVERGENCE TRANSIENT, not an event-locked dip.** Whole-frame
luminance falls monotonically **103.9 → ~74 over the first ~30-40 frames (≈ 1.0-1.3 s)** and then holds.
The steady-state per-event sawtooth is only **1.4-2.1 %**. The long-window leg (AE7) bounds the
recovery: after a revert at `si=43` the frame rises 75.06 → 77.44 over **39 frames and is still
rising**, which is `SpeedDown = 1.0` (τ = 1 s, `Scene.cpp:479`) behaving exactly as specified.

## 11.3 THE VERDICTS — and one of them is a REFUTATION

- **Symptom (3) "whole picture black for a burst"** — **`AE-PARTIAL`.** Auto-exposure is confirmed as a
  large, real, previously-unmeasured effect on captured pixels in **both** editor and packaged. ⛔ **But
  nothing on this fixture approaches black**: the darkest frame is **72.4 of 255**, and the deepest
  single-frame drop is **9.01 %**. AE **did not reproduce** the symptom at this stimulus size.
- **Symptom (2) "target renders BLACK"** — 🚨 **AUTO-EXPOSURE IS REFUTED AS A STANDALONE EXPLANATION,
  and it is refuted structurally rather than by a null.** Eye adaptation is a **global** operator; it
  cannot darken one object while leaving the frame alone. A dark target with no corresponding
  whole-frame dip is therefore **not** AE, whatever the fixture. **`DARK FIRST FRAMES = 0` on every leg.**
- ⚠ **AND A CONFOUND WAS FOUND FOR ANY ONSET READING.** Under AE the first event of a session fires
  while exposure is still converging, so its target reads **117.8 against a session mean of 95.0** —
  brightest first, falling monotonically. **That looks like an onset effect and is not one.** It is
  where in the session the event fired. The brief predicted "first frame darker than the event mean";
  the measurement gives the **opposite sign**, and the pre-declaration called that direction in
  advance — though **for the wrong reason**, which is recorded rather than smoothed: §1 argued from
  "the swap is brighter than the scene", and a pinned-exposure measurement then showed the swap is
  **DARKER** (`corrupted_texture` **−17.7 %**, `missing_texture` **−3.5 %**). **Right prediction, wrong
  premise; the real driver is session position, not per-event response.**
- ⛔ **NO MECHANISM IS ASSERTED FOR THE SIGN OF THE PER-EVENT SWING** (`G120`). At pinned exposure the
  swap darkens the region, yet under AE the whole frame reads *darker* during positives rather than
  brighter. The histogram response was **not** derived and is not claimed. What is claimed is only what
  was measured: the excursion grows ~3-5× and the whole level drops 24 %.

## 11.4 ⚠ THE `G135` GUARD — WHAT THIS NULL DOES NOT MEAN

The target is **≈ 7.2-7.8 % of frame**. A 7 % stimulus may simply not move the 10th-90th percentile
log-luminance histogram far. ⇒ **`AE-PARTIAL` for symptom (3) DOES NOT EXCLUDE auto-exposure on the
owner's content**, where coverage and the brightness gap can both be far larger and the exposure
excursion scales with them. This is `G135` in its exact shape and it was pre-declared in §4 of the
prediction file, before the number existed. **Do not upgrade this into "auto-exposure is excluded".**

## 11.5 IT IS NOT A CAPTURE DEFECT

The dip is the **game's own rendering**, correctly captured. The dataset should look like the game.
`verify_capture.py --black-frame-gate` **PASSES** on the deepest AE-ON leg (`min 72.419` against
threshold `6.0`, a 12× margin, **0 black frames, 0 dark first frames**), and its `--selftest` passes a
grey session and fails an all-black one, so that PASS is a reading and not blindness. **The m47
threshold survives the regime change — but by luck of direction, not because AE was considered when it
was derived.**