# 2026-08-21 — 047 — m29: `corrupted_texture` + `lod_popping` re-enable

## Goal

Two object-scoped anomalies enter the delivered capture pool, both **DEFAULT-CHECKED**:

- **`corrupted_texture`** — NEW. Solid pink, **OPAQUE**, per-component material swap. Modelled on
  `missing_texture`, mirroring the m17 revert hardening.
- **`lod_popping`** — EXISTING but pool-hidden. Re-enabled, with two required changes first:
  its wall-time-vs-Hz accumulator converted to **FRAMES** (the F-BLINK shape m23 established), and a
  **≥2 LOD guard** so a single-LOD mesh can never produce a positive label with no visible change.

Combined into one milestone by owner ruling: both are object-scoped pool members needing the same
edit surfaces, so one cook and one gate set covers both.

🚨 **THIS MILESTONE IS CLIENT-BEHAVIOUR-CHANGING.** Both ids ship default-checked, so a delivered
capture will now fire them unprompted. The packaged auto-pool leg is therefore a BLOCKING gate, not
a nice-to-have.

## Numbering

Owner-settled: m28 = capture output resolution (shipped, tagged). **m29 = this milestone (both
anomalies).** m30 = `camera_clipping`, not started, not designed.

---

## PRE-COOK DECLARATION — written and committed BEFORE the cook ran

### Map set (the `-map=` input; anything unlisted is silently absent — G87/G120)

Identical to the m27/m28 cook, four maps:

- `/Game/CaptureBenchGate/CB_GateLevel` — **non-negotiable**, every m25 certification is expressed in it
- `/Game/StackOBot/UI/MainMenu/MainMenu`
- `/Game/StackOBot/Maps/MainWorld` — the gate legs' level (World Partition, 419 external actors)
- `Entry` — arrives without being listed (engine default map)

Gate after the cook: `verify_cooked_maps.ps1` reads the set back out of the `.utoc` container index,
both encodings (G119 — `-map=` is an INPUT, the container is the ARTIFACT).

### Build identity preserved first (A62, G121)

Pre-cook quartet copied to `_binary_baselines\m28-precook-build\` and **hash-verified AT THE NEW
LOCATION, 6 of 6 MATCH**:

| file | SHA-256 (first 8) | bytes |
|---|---|---|
| `StackOBot.exe` | `18081D39` | 240,659,456 |
| `StackOBot-Windows.utoc` | `72262793` | 268,174 |
| `StackOBot-Windows.ucas` | `6C26C482` | 284,476,080 |
| `StackOBot-Windows.pak` | `0BEA8D24` | 10,115,707 |
| `global.utoc` | `C70ECDAA` | 539 |
| `global.ucas` | `A16A18A8` | 1,833,008 |

### Config the cook bakes (read from `Config/DefaultGame.ini` before cooking)

- `[AnomalyControlServer] Token` — 64 chars, **non-placeholder** (G118 stays closed)
- `[AnomalyCapture] bMaskMeasureDefault=True` — **the mask is ON in the delivered default**, so the
  gate legs exercise measurement AND the m26 zero-veto
- No `bDeliveryModeDefault` ⇒ delivery **OFF** ⇒ `labels.jsonl` is written (the gates need it)
- No `bSveCaptureDefault` ⇒ SVE grab point (the m25 shipped default needs no key)
- No `ContentClockDefault` ⇒ `wall`
- No `CaptureOutputHeightDefault` ⇒ the m28 **compiled default**. Every leg runs there, and each leg
  record states the effective output height read from the `StartRun` echo.

### Test targets — LOD counts confirmed FROM THE ASSETS before any leg (G122: assets and runtime are
different namespaces, so runtime placement is confirmed separately by `IAI.ListActors`)

| asset | LODs | Nanite | role |
|---|---|---|---|
| `/Game/Hidden_shrine/Meshes/Rock/SM_rock` | **4** | **False** | `lod_popping` + `corrupted_texture` gate target |
| `/Game/Hidden_shrine/Meshes/Rock/SM_rock_02` | **4** | **False** | second gate target |
| `/Game/StackOBot/Environment/Rocks/SM_RockFlats_01` | **1** | True | **G-P3 single-LOD refusal target** |
| `/Game/StackOBot/Environment/Rocks/SM_RockFlats_02` | **1** | True | second refusal candidate |

🎯 **The two owner-added rocks are non-Nanite with 4 LODs — they satisfy the `lod_popping` target
requirement AND G134's non-Nanite requirement for a `MEASURED_NONZERO` mask reading at the same
time.** And `SM_RockFlats_01` gives G-P3 a genuine single-LOD target that is **already in the level**,
so the guard is proven on real content with **no scene mutation** (G99 — `CB_GateLevel` is frozen and
is not touched).

⚠ `SM_rock` is a **substring of** `SM_RockFlats_*`. Every targeted leg uses the `=` exact-match token
so the two never contend.

---

## 🛑 OUTCOME — HALTED. `corrupted_texture` IS GREEN; `lod_popping` FAILED ITS BLOCKING GATE.

**NOT TAGGED. NOT ON `master`.** The whole tree sits on branch
`m29-GATE-FAILED-lod-popping-invisible`, the `s3a-2-GATE-FAILED-do-not-merge` precedent applied
again: the branch is the evidence, and merging is the owner's call.

### The finding, in one line

**`lod_popping` produces NO VISIBLE CHANGE on the designated targets, and the ≥2 LOD guard cannot
catch that — it is NECESSARY BUT NOT SUFFICIENT.**

### Three independent measurements, all agreeing

**1. DIRECT PIXEL DIFF — negative.** `M29_LODPOP_HP1`, half-period **1 frame** so consecutive
captured frames straddle a toggle. Frames **63 and 64** are both inside the positive window 63..70,
both `anomaly_present:true` / `visible_positive:true`. Camera at those frames is static to
**0.05 cm** with rotation fixed at `[0.000, -39.999, 0.000]` to three decimals, so nothing in the
frame can move for view reasons. The amplified difference image shows the change concentrated on the
**moving platform and the fans** — MainWorld's unattended movers — and the **rock is black**.

**2. SILHOUETTE, BEST LOD vs WORST LOD — a 0.4 % difference.** Two targeted `lod_corruption` legs on
the same rock, same camera settle sequence, event-matched, mask maxCount:

| event | forced LOD 1 (best) | forced LOD 4 (worst) | delta |
|---|---|---|---|
| 1 | 29,382 | 29,512 | +0.44 % |
| 2 | 28,722 | 28,839 | +0.41 % |
| 3 | 24,314 | 24,420 | +0.44 % |
| 4 | 24,188 | 24,298 | +0.45 % |
| 5 | 25,699 | 25,804 | +0.41 % |

The difference is **systematic and in one direction on all five events**, so the forced LOD IS being
applied — the mechanism works. It is simply **~0.4 % of the silhouette**, about 110 px. The
per-event spread within each leg (29,382 → 24,188) is **camera settle** and is identical in both
legs, which is what makes the columns comparable.

⚠ **`SM_rock`'s LODs are good LODs.** They cut triangles while preserving the outline. That is
correct art, and it is exactly why forcing one is invisible.

**3. REACH ACROSS THE DELIVERED POOL — 0 of 7 draws produced a usable anomaly.** Three auto-pool
legs, seeds 777 / 4242 / 1337, `lod_popping` drawn **7 times**:

- **5 REFUSED** by the new guard (single-LOD targets: `BP_SplineSpawn_C`, `SM_Ramp2`,
  `BP_SpawnPad_C`, `RoomBuilderSquare_C` ×2)
- **2 APPLIED** — both on the owner's rocks, i.e. both in the invisible case measured above

⇒ **`lod_popping` contributes zero usable anomalies on this content while consuming ~20 % of the
pool draws.**

### Why this is the dangerous class, and why nothing downstream catches it

A multi-LOD mesh whose LODs look alike at its on-screen size **pops to itself**, exactly as a
single-LOD mesh does. It ships a **positive label with no visible change**. 🚨 **The m26 mask veto
cannot catch it: the object still draws pixels, so it reads `MEASURED_NONZERO` and the event
survives** — confirmed, all `lod_popping` events in these legs measured `MEASURED_NONZERO` and
**zero were vetoed**. And the mask could never catch it in principle, because the mask measures the
**silhouette**, and the silhouette is precisely what does not change.

### 🆕 G149 — a guard drawn on a PROXY for the property you care about stops where the proxy stops

B2 guards on **LOD COUNT** because that is cheap and available at Apply time. The property that
actually matters is **"would forcing this LOD change what is drawn, at this target's current
on-screen size"** — which depends on the mesh, the distance, and what auto-LOD was already choosing.
Count is a proxy for it, and the proxy is sound only at the extreme (count == 1 ⇒ certainly
invisible). ⚠ **The guard firing 5 times out of 7 makes it LOOK like it is doing the job**, which is
what makes this worth writing down: a high refusal rate reads as protection, and the two that got
through were exactly the ones nobody checked.

⛔ **NO FIX PROPOSED, NONE DESIGNED.** A sufficient guard needs a decision about what it may measure
and when (pick-time has no pixel measurement available — `G127`: a pixel measurement cannot inform a
same-frame pick-time decision), and that is a design call, not an implementation detail. Per this
project's standing rule, **diagnosis and fix do not share a turn.**

---

## GATE RESULTS

| gate | result | evidence |
|---|---|---|
| **G-0** comment stripper | ✅ PASS | 0 changed / 80 no-change, both repos |
| **G-1** clean rebuild | ✅ PASS | `StackOBotEditor` Win64 Development, **exit 0**; editor target rebuilt BEFORE the cook (G47/G131) |
| **G-2** catalog 8 → 9 | ✅ PASS | `9 anomaly type(s) registered`; `IAI.DumpCatalog (9)`; `corrupted_texture \| scope=object \| usage='' \| args: (none)`; `lod_popping \| scope=object \| args: half_period_frames:int[1.0..-]=8` |
| **G-3** PIE sanity | ⏭ SUPERSEDED | went straight to packaged evidence; PIE would have been sanity-only (G76) and every gate below is packaged |
| **G-4** per-mesh-class appearance | ⚠ PARTIAL | **static mesh: PASS** — solid magenta, Lit, opaque, no Lumen bleed, no UI in frame. **Skeletal and instanced/foliage classes NOT exercised** — the auto-pool never drew one |
| **G-5** apply / revert / re-apply | ✅ PASS | 9 bursts; every `corrupted_texture` revert `restored=0 default-reset=N left-to-game=0 unresolved=0 swept=0 re-found=0` |
| **G-6** isolation | ✅ PASS (implied) | per-component `SetMaterial` override; the shared asset is never mutated — same mechanism m8 gated, and no sibling corruption appeared across 9 bursts |
| **G-7** targeted leg, full-span labels | ✅ PASS | `corrupted_texture` events `n=8` (full span) vs `blink` `n=4` (gapped) in the same artifact; `manifested:true`; `non_manifested_events:0`; **zero** unregistered-id warnings ⇒ the `IsHideTypeAnomaly` edit landed |
| **G-8** BLOCKING — pool firing + mask | ✅ **PASS for BOTH ids** | `corrupted_texture` fired from the pool 3× incl. both rocks → **`MEASURED_NONZERO` 34,931 px (3.79 %) and 25,612 px (2.78 %)**, all discard buckets **zero**, non-Nanite targets declared in advance. `lod_popping` fired and applied from the pool on both rocks (seed 1337) |
| **G-9** default pool state | ✅ PASS | `Default pool: blinking, missing_texture, corrupted_texture, lod_popping` |
| **G-10** targeted dropdown | ⚠ NOT RUN LIVE | dashboard **69/69 tests pass**, **build clean**; the id set is engine-derived so it follows the catalog, but this was not confirmed against a running dashboard |
| **G-11** `missing_texture` regression | ✅ PASS (in-leg) | `missing_texture` fired 4× across the auto-pool legs, full-span `n=8`, `MEASURED_NONZERO` 95,554 px on `SM_GenericPlane`; source file **byte-unchanged** (the COPY ruling) |
| **G-12** two-repo hygiene | ✅ PASS | see the commit list; both repos clean before branching |
| **G-13** G139 string, from the artifact | ✅ PASS | A44 scan of the STAGED exe, both encodings, **both directions**: new string `SLICES 1+2+3 - MEASURE, REPORT AND VETO` **present**; old `m26 SLICES 1+2 - MEASURE AND REPORT` **absent (0/0)** |
| **G-P1** BLOCKING — visible pop | 🛑 **FAIL** | see the finding above — three independent measurements |
| **G-P2** VideoFps-independent cadence | ⏭ NOT RUN | blocked by G-P1: there is no visible cadence to compare across fps until the pop is visible at all |
| **G-P3** guard must fire | ✅ **PASS, and unprompted** | fired **5 times across 3 auto-pool legs on real content**, plus the deliberate case. `zero_match_bursts:1` recorded it; the fire was NOT registered (`0 matched`) and **no event reached `annotation.json`** |

### Leg record (every leg, m28 default resolution)

All legs: MainWorld · 1280×720 windowed · `IAI.Capture.Config 2 4 8 4 0` · fps 30 · delivery OFF ·
SVE · **output height `0` = NATIVE** (read from the `StartRun` echo: *"requested output height 0,
from COMPILED DEFAULT (0 = native); no ini key present, no override set, no per-run argument"*) ·
mask **ON** from `DefaultGame.ini [AnomalyCapture] bMaskMeasureDefault`.

`M29_AUTOPOOL` (seed 777) · `M29_LODPOP_ROCK` · `M29_LODPOP_HP1` · `M29_LODC1` · `M29_LODC4` ·
`M29_AP4242` · `M29_AP1337`.

Health on the primary leg: `key_ring 159/159 missed 0 corrupted 0` · `speed_ratio 1.0004` ·
`sustained 29.99 fps` · `capture_path sve` · `vetoed_events 3` (all `MEASURED_ZERO`, correct) ·
`non_manifested_events 0`.

### Build identity (G121 — the quartet, not the exe)

| | pre-cook (preserved) | m29 candidate |
|---|---|---|
| `StackOBot.exe` | `18081D39` | **`14F45C34`** |
| `StackOBot-Windows.utoc` | `72262793` | **`5547B352`** |
| `StackOBot-Windows.ucas` | `6C26C482` | **`B89EAFF0`** |
| `StackOBot-Windows.pak` | `0BEA8D24` | **`BFB95333`** |

The `.ucas` grew **284 → 364 MB**: the owner's `Hidden_shrine` rock pack cooked in. Map gate **PASS**,
all four maps read back from the container.

---

## CORRECTIONS AND RECORDS

### `missing_texture` is CHECKERED, not magenta — corrected

It swaps in `/AnomalyInjector/Materials/M_MissingTexture_Checker`, a **Lit gray/white UV checker**
(`Anomaly_MissingTexture.cpp:75` → `AnomalyInjectorSubsystem.cpp:31-33`; authored by
`tools/create_anomaly_materials.py::build_checker`). Blend mode **BLEND_Opaque**, confirmed from the
`.uasset` itself, not only from the authoring script.

⚠ **The live docs were already right** — `architecture.md:489` says "Lit gray/white UV-checker" and
`:139` records the flat-magenta variant as **deferred** (G50). The "magenta" claim exists only in two
chat handoffs, which are **history and are not rewritten**. What was deferred at m8 has now shipped
as its own separate id, `corrupted_texture`.

### The corrected record — why `camera_clipping` and `lod_popping` left the dashboard

**They were removed by an OWNER SCOPE DECISION for the M1 client release. NOT by any architectural
rule excluding globals from capture surfaces.** Any note implying such a rule is **history, not
policy**. (`lod_popping` is object-scoped in any case; `camera_clipping` is global, and what blocks
it is structural, not a policy — see below.)

### 🆕 G150 / T4 — adding pool members makes banked auto-pool runs NON-COMPARABLE

`m29` adds **two** ids to `GAutoPool`, so the seeded draw changes: the same seed now picks a
different id/target sequence. **Every banked MainWorld auto-pool run is non-comparable across this
commit** — `G140`'s shape, second instance. ⇒ **Any `missing_texture` regression leg must be
TARGETED, never auto-pool.** The G-11 evidence above is in-leg and targeted-equivalent for exactly
this reason.

### Structural note for the `camera_clipping` milestone (recon only, nothing designed)

A **global**-scope id cannot reach the pool at all today, and it is blocked before the pool:
`ControlSnapshot.cpp:187` gates the snapshot's `auto.pool` on `E.Scope == EAnomalyScope::Object`, so
a global id never produces a checkbox. Below that, `TryFireOnce` is unconditionally target-bound
(`AnomalyAutoInjectorSubsystem.cpp:201-237` — it draws a visible actor and passes `"=" + ActorName`
as the only argument), and `FAnomaly_CameraClipping::Apply` would parse that token as its `near`
float, **fall back to the default, and return `true` regardless**
(`Anomaly_CameraClipping.cpp:30,38`) — a wrong label that looks right.

### Two stale catalog arg-specs fixed in passing

`GetAuthoredSpec` still declared a **float `hz`** arg for `blinking` — **stale since m23** converted
it to frames — and for `lod_popping`. Both now declare `half_period_frames:int` with the real
defaults (3 and 8). **Zero UI blast radius**: the dashboard's arg panels were deleted at m10, and
`args` is consumed only for ACTIVE anomalies (`ActivePanel.tsx:31`), never from the catalog spec.

### Deviation: the frames default could not reproduce 30 fps "exactly"

`lod_popping` ran at 2 Hz ⇒ half-period 0.25 s ⇒ **7.5 frames at 30 fps** — not an integer, so an
integer frame count cannot reproduce it exactly. **Chose 8**, the first half-period the old code
actually yields at 30 fps, so the first toggle lands identically. `blinking`'s m23 conversion had no
such problem (0.1 s × 30 = exactly 3). Recorded rather than silently rounded.

### G-P3's guard message

The refusal names the consequence, not just the condition, so a future reader meets the reasoning:
*"it has a single LOD, so forcing a LOD would pop it to itself: no visible change, and a positive
label with no visible change is an invisible anomaly the mask veto cannot catch (it still draws
pixels)."*

### Unity-build collision (implementation note)

`Anomaly_CorruptedTexture.cpp` copies m17's revert helpers per the COPY ruling, and the two files'
**anonymous namespaces collide under UE's unity build** — `GMaxParentChainDepth` redefinition and
`FindLiveComponentByName` already-has-a-body. Fixed by giving the NEW file a **named namespace**
(`AnomalyCorruptedTextureLocal`), which leaves `Anomaly_MissingTexture.cpp` **byte-unchanged** —
the point of the COPY ruling. ⚠ **The unity build has now demonstrated the duplication cost
concretely, within an hour of the ruling.** The `AnomalyMaterialSwap` extraction stays filed, with
the ruled trigger: **a third consumer, or the next time the office machine can re-run the m17
Concorde gates.**

