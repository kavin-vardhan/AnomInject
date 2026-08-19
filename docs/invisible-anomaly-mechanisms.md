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
| **status** | ✅ **SUPPORTED, REPRODUCED HERE** |
| **what it is** | Selection requires a renderable component — **a TYPE test, not a DRAWING test**. `UInstancedStaticMeshComponent` derives from `UStaticMeshComponent`, so aggregates pass trivially while their `Bounds` cover the whole cluster. The label describes the container; the pixels are a small subset of it. |
| **evidence** | **MEASURED.** `InstancedFoliageActor_0_0_0`: `bbox_px (0,0,1280,720)` on 59/59 rows — **the entire frame** — `coverage_ratio 1.0`, `coverage_pct 100`, `manifested true`, while whole-frame change was **~0.0069** and an 8×8 grid put it in **4 of 64 cells**. |
| **pointer** | journal 045 PART NINE · **G124** |
| 🚨 **generalises** | **MEASURED and NOT foliage-specific.** **3 of 13** non-foliage selectable actors carry a **NEGATIVE `poll_distance`** — `BP_SpawnPad_C` **−114.8** (a **plain `StaticMeshComponent`**), `BP_SplineSpawn_C` **−19405.5**, `RoomBuilderSquare_C` **−1737.8**. A negative value means the bounds sphere already contains the poll origin, so **the 1800 cm cull can never fire from anywhere in the level**. |
| ⛔ **limits** | **MECHANISM ONLY, NO INCIDENCE CLAIM.** One instance measured end-to-end; the owner's `InstancedMeshActor` is **not** this actor. |
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

## What is NOT established

- **Which mechanism accounts for the client's cases**, in what proportion. **No incidence claim exists
  for any row above.**
- Whether `H5` (i) occurs in this project at all — StackOBot is a polished sample and may simply not
  contain the pattern. **That would be a property of THIS PROJECT, not evidence against class (i) in
  the client's game.**
- Whether any two rows share a cure.

*(Maintained alongside `docs/gotchas.md`. Referenced from `CLAUDE.md`. Created 2026-08-19.)*
