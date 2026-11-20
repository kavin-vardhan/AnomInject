---
name: h5-selector-admits-non-drawing-objects
description: "H5 — the selector's renderable test is a TYPE test, not a DRAWING test; class (ii) (aggregate/instanced actors) reproduced and SUPPORTED on 2026-08-19"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-08-19T14:35:08.191Z
---

**`H5` — THE SELECTOR ADMITS OBJECTS THAT CANNOT MANIFEST A VISIBLE HIDE.** Minted 2026-08-19 (number
verified free: `H1`/`H3`/`H4` minted, **`H2` RETIRED-UNKNOWN**, no `H5`–`H9`). Plugin `4d91133`, bench
`873862a`, **no tag, zero production code**. Journal
`docs/sessions/2026-08-19-045-h4-cook-and-h5-mainworld-arc.md` **PART NINE**.

**Triggered by OWNER OBSERVATION** (eyeball-level, not measured): the client's invisible anomalies were
**not partially hidden — they could not be found at all**; culprits named in her data were
**`InstancedMeshActor`** and **`BP_LocalVolumetricFog`**; the rest unattributable because every actor
reads `StaticMeshActor_xxx`. **That evidence outranked a chat-side inference, which was WITHDRAWN.**

⛔ **`H4` ≠ `H5`. H4 is a target that WOULD draw but is BLOCKED; H5 is one that WOULD NOT DRAW ANYWAY.
Same symptom, different mechanism, DIFFERENT CURE — `feature/stencil-capture` is H4's cure and may not
be H5's.** Path (a) is **PARKED, NOT REFUTED** — a *priority* decision, not a scope one (G120).

**THE FILTER (source):** `IsRenderableComponent` checks **only** `Component->IsVisible()` and, for ISM,
`GetInstanceCount() > 0`. **ABSENT:** the owner actor's `bHidden` (`IsVisible()` doesn't consult it),
`bRenderInMainPass`, `GetStaticMesh() != nullptr`, section count, material presence,
`WasRecentlyRendered()`, and **any distinction between ISM/HISM and a plain SMC** — they *derive* from
`UStaticMeshComponent`. Every companion predicate is computed on **BOUNDS**; none reads a pixel.

**CLASS (ii) REPRODUCED** on `InstancedFoliageActor_0_0_0`: label `bbox_px (0,0,1280,720)` on **59/59
rows — the whole frame**, `coverage_ratio 1.0`, `coverage_pct 100`, `manifested true` — while the
measured whole-frame change was **~0.0069** against a proper hide's **0.1023–0.1116**, and an 8×8 grid
put the change in **4 of 64 cells**. ⇒ **the label claims 100 % of the frame; the change lives in ~6 %.**

🚨 **CURE MEASUREMENT (2026-08-19, journal PART ELEVEN, plugin `b909475`): THE CURE NEEDS A NEW
MEASUREMENT, NOT A NEW THRESHOLD.** Every artifact field — `cov_pct`, `coverage_ratio`, `bbox_px`,
`poll_distance`, `node.bounds` — describes **CLAIMED extent; none describes DRAWN extent**. The one
plausible proxy `cov_pct` **does not separate at all** (BAD holds both 100.00 and 3.86). ⛔ **And the
SELECTION BOUNDS EXTENT cannot be read back from any artifact** (`node.bounds` is the whole actor;
`poll_distance` is one equation in two unknowns). Naive-rule costs, measured over 5 GOOD / 2 BAD:
`poll_distance<0` breaks **2/5 GOOD**; `cov_pct>3` breaks **4/5**; the ISM/HISM blacklist breaks
`RoomBuilderSquare_C` (a 75:1 in/out ratio). **No `cov_pct` value catches both BAD without breaking
GOOD.** ⚠ Occlusion count separated here (BAD ≤3/9, GOOD ≥5/9) — **n=2 vs 5, post-hoc mechanism,
OBSERVATION not a threshold.**
🆕 **A SECOND class-(ii) instance and it is NOT foliage: `BP_SplineSpawn_C`** — claims 22.9 % of frame,
peak-in change **0.0175** vs the control's **0.5515**. ⚠ **I had excluded it on the inference "it
clearly draws" from its RECT SIZE — claimed extent, the very quantity H5 distrusts.**
🔧 **My harness was truncating its own captures:** `run_leg.ps1` killed the process the instant
`run_summary.json` appeared, mid async-writer flush → **7 zero-byte PNGs while every counter read
clean**. Fixed with a flush-wait, proven both ways. **Always check for zero-byte frames.**
⚠ **DISK: 12.9 GB free; bank 15.7 GB / 148 dirs, exe-side leg dirs 6.4 GB / 83.** The exe-side dirs
are mostly duplicates of banked sessions and are the obvious thing to prune.

**📒 THE LEDGER IS `docs/invisible-anomaly-mechanisms.md` — read it first.** Five rows: `m23`/`P3`
FIXED · `H4` SUPPORTED (cure = `feature/stencil-capture`) · `H5`(ii) SUPPORTED · `H5`(i)
**ENUMERATED-NOT-OBSERVED** · **traceability degradation, marked NOT A CAUSE**. **No single fix is
known to address all of them.**

⚠ **CORRECTION (2026-08-19, `G125`): a HARNESS MARKER contaminated the first foliage grid.** Two of the
"four cells" I published were the `CaptureBench` frame-identity marker, which changes every frame *by
construction*. Caught by comparing grids across legs — the same two cells within 0.006 across **three
levels and three targets**. Fixed at source (`-Marker 0`), not masked. Corrected: whole-frame mean
**0.0059** (was 0.0069), grid peak **0.1242** (was "0.1800"). **The finding strengthened.**
⛔ **Turn the marker OFF for any pixel measurement.**

✅ **`G124` GENERALISES (measured): 3 of 13 non-foliage selectable actors have a NEGATIVE
`poll_distance`** — `BP_SpawnPad_C` **−114.8 (a PLAIN `StaticMeshComponent`)**, `BP_SplineSpawn_C`
−19405.5, `RoomBuilderSquare_C` −1737.8. **The mechanism is OVERSIZED BOUNDS, not aggregation — a
foliage/instanced blacklist would miss `BP_SpawnPad_C`.**

📌 **TRACEABILITY, characterised (1,267 nodes / 109 packaged legs): `asset_name` and `component_class`
are 15/15 populated and DO NOT degrade in builds.** Only **`node.name`** degrades — it is
`AActor::GetName()`, identical in editor and build; the **editor displays the LABEL**, which does not
exist in a cooked build. ⇒ **attribution to a culprit CLASS is possible today** via
`asset_name`+`component_class`; distinguishing two instances of one class is not.

**How to apply:** → **[[g124-aggregate-bounds-defeat-every-guard]]**. When measuring a suspected H5
case, **use the GRID, not the bbox** — a bbox-only reading (A35's rule) reports a real change and calls
it a manifest hide. `P-a1`…`P-a5` remain **UNTESTED** (the live `1/9` is corroboration at the boundary,
not a test — provenance is computed *after* selection). ⚠ **Traceability now BLOCKS characterising the
defect from client data, and the `B1` name collides** (pose gate vs the shipped m22 traceability work);
`asset_name`/`component_class` **already exist** and named the culprit here — **ask the owner which
fields her copy actually shows** before adding any. Any new field is **`P6` territory / milestone**.
