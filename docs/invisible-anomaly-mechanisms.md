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
| **cure** | ⛔ **UNKNOWN, and NOT designed.** ⚠ **A CLASS BLACKLIST IS NOT A FIX** — `BP_SpawnPad_C` is a plain SMC and would be untouched by one, while looking closed. |

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
| **pointer** | journal 045 PART TEN |
| **consequence** | **Attribution IS possible from `asset_name` + `component_class` even when `node.name` is generic.** |
| **cure** | ⛔ **NOT designed.** Any change to how these fields are populated **touches the `annotation.json` contract ⇒ `P6` TERRITORY / MILESTONE CANDIDATE.** ⛔ **`P6` DOES NOT MOVE.** |

---

## What is NOT established

- **Which mechanism accounts for the client's cases**, in what proportion. **No incidence claim exists
  for any row above.**
- Whether `H5` (i) occurs in this project at all — StackOBot is a polished sample and may simply not
  contain the pattern. **That would be a property of THIS PROJECT, not evidence against class (i) in
  the client's game.**
- Whether any two rows share a cure.

*(Maintained alongside `docs/gotchas.md`. Referenced from `CLAUDE.md`. Created 2026-08-19.)*
