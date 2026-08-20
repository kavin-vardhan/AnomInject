# 2026-08-21 — 047 — m29: `corrupted_texture` ships; `lod_popping` deferred to m30

## Outcome in one line

**m29 = `corrupted_texture`**, a new 9th anomaly, object-scoped, default-checked in the delivered
pool, fully gated. **`lod_popping`'s timing conversion and its ≥2-LOD guard also ship** — they are
correct and gated — **but its POOL MEMBERSHIP is deferred to m30**, because the proximity gate it
needs could not be calibrated in one pass. That deferral is the owner's pre-authorised contingency,
not a judgement about the anomaly.

🚨 **`lod_popping` IS A CLIENT REQUIREMENT AND WILL SHIP. Nothing here says otherwise.**

---

## ⛔ TWO CORRECTIONS TO THIS SESSION'S OWN EARLIER REPORT — read these before anything else

### CORRECTION 1 — the "not viable on well-authored content" verdict is STRUCK

An earlier report from this session concluded that `lod_popping` "produces no visible change" and
"contributes zero usable anomalies on this content". **That conclusion is WITHDRAWN and must not be
carried forward.** It was drawn from a single test condition that structurally could not contain the
effect: every leg ran under the shipped 18 m pawn-anchored poll radius only, with the rock at
~3 % of frame drawn. **The owner's original requirement — "if the player is NEAR a LOD-enabled
object, LOD popping happens" — was never in the test conditions.**

This is **`G135`'s shape and the m27 owner-play-gate lesson repeating**: a bench leg that cannot
exhibit the effect returns a clean negative, and a clean negative reads like a finding.

### CORRECTION 2 — the first calibration pass was VOID, and the owner caught it

The owner reported *"the game keeps launching to a black screen"*. He was right, and it invalidated
a measurement I had already drawn a conclusion from.

`CB_LodCalib`, the synthetic calibration level authored for this work, rendered **100 % BLACK**:
`mean_luma = 0.0000`, **zero non-zero pixels of 921,600**. On that level I had measured LOD 1 vs
LOD 4 as **byte-identical** and read that as "coverage does not separate visible from invisible".
**It was black-vs-black.**

🚨 **AND IT RECONCILED A CONTRADICTION I WAS ACTIVELY CHASING.** The mask reported a small but
systematic ~0.2 % difference between LOD 1 and LOD 4 while the colour frames showed nothing. Both
readings were correct: **custom depth does not need lighting, so the mask saw the real geometry
change; the colour frames carried no light at all.** I had the discriminator in hand and read it as
noise.

⇒ **`m19`'s lesson, third instance: GATE ON PIXELS.** I gated on a mask count and on a
"zero difference" number, and neither can tell a black frame from a null result. **A luma check is
now the first thing run against any new capture environment, before any measurement is trusted.**
→ **G151**.

Isolation was verified rather than assumed: on the *same* build, MainWorld read `mean_luma 107.95,
99.14 % non-zero` while `CB_LodCalib` read `0.0000`. The build was healthy; only the synthetic level
was dark. Cause: it was authored without the movable point lights `make_gate_level.py` spawns and
without the directional light flagged as the atmosphere sun light. Fixed, re-authored, re-cooked,
**and gated on luma before re-use** — `mean 83.74, 67.61 % non-zero, max 255`.

---

## WHAT THE CORRECTED MEASUREMENTS ACTUALLY SHOW

With the calibration level lit, LOD 1 vs LOD 4 on `SM_rock`, two legs per rung, camera identical
(`dpos 0.0000 drot 0.0000`), whole-frame pixels differing by ≥8/255:

| rung | distance | bounds coverage | strong-diff px | verdict |
|---|---|---|---|---|
| A | 1000 cm | **33.04 %** | **66,615** | VISIBLE |
| B | 2331 cm | **9.35 %** | **12,489** | VISIBLE |
| C | 3780 cm | *unmeasurable* | **14** | not visible |
| D | 5720 cm | *unmeasurable* | **8** | not visible |

**Three orders of magnitude separate rung B from rung C.** `lod_popping` is plainly visible at close
range. **The owner is right and the earlier verdict was wrong.**

And on **real content**, MainWorld's `SM_rock` at bounds coverage 11.83 %, measured with the same
two-leg instrument restricted to the target's own bbox: **2,133 strong pixels in-bbox**, against an
out-of-bbox control channel of 3,335 px carrying the level's moving platform and fans.
⚠ **This also refutes my earlier MainWorld reading.** That one compared *adjacent frames within one
leg* at half-period 1 and assumed they straddled a toggle. They evidently did not. **The two-leg
LOD 1 vs LOD 4 method is the sound instrument; the adjacent-frame method is not, and its result is
withdrawn.**

## 🛑 WHY THE PROXIMITY GATE IS NOT IN m29

`D3` forbids proposing a threshold until **both** anchors exist on the coverage axis. After the
corrected pass:

- **VISIBLE side — measured.** Rung A 33.04 %, rung B 9.35 %.
- **INVISIBLE side — NOT measurable.** Rungs C and D produced **no trustworthy coverage number**.
  `annotation.json` gave the `-1` sentinel for rung C, and the label projector returned **inverted
  rects** on this level — `bbox_px [945,205,335,257]` (right < left), `[14,271,245,147]`
  (bottom < top). A coverage figure derived from a degenerate rect is not a measurement.
- **MainWorld cannot supply the invisible anchor either**: its only multi-LOD targets are the two
  rocks, at 11.83 % and 16.73 %, and both are on the visible side. Every other pool target is
  single-LOD and is refused before it can be measured.

⇒ **The bracket has one end. A threshold placed on it would be invented, not calibrated** — exactly
what `D3` forbids. **HALTED, and the pre-authorised contingency invoked: ship `corrupted_texture`,
defer `lod_popping`'s pool membership to m30.**

📌 **WHAT m30 INHERITS, so the next pass is short:** the calibration level exists and is cooked
(`CB_LodCalib`, authored by `CaptureBench/tools/make_lod_calib_level.py`, luma-gated); the two-leg
strong-diff instrument exists; the visible-side anchors are measured. **m30 needs the invisible-side
coverage, which means fixing the label projector's inverted rect on that level (or reading coverage
by a route that does not depend on it) and adding one or two farther rungs.**

⚠ **A REAL FINDING FOR THAT WORK, and it is why the threshold matters more than it looks:** on
MainWorld the rock reads **11.83 % bounds coverage while drawing only 2.78 % of frame** — the bounds
proxy over-reads by ~4×. A bounds-coverage threshold is a proxy for a proxy, and `G149`'s warning
applies to it directly.

---

## G149 — AMENDED (append; nothing deleted)

- **STANDS**: LOD COUNT is a proxy. Count 1 ⇒ certainly invisible. Count ≥ 2 is **not** certainly
  visible.
- **CORRECTED**: the missing variable is the target's **ON-SCREEN SIZE**, not LOD authoring quality.
  A good LOD preserves the silhouette **at the size it was authored for**; close enough, the
  difference is plainly visible — measured at **66,615 strong pixels** at 33 % coverage. **The 0.4 %
  best-vs-worst delta is a reading AT THAT DISTANCE, not a property of the mesh.**
- **STANDS**: nothing downstream catches it. The `m26` mask measures the **silhouette** and reads
  `MEASURED_NONZERO` either way. **The gate must be at PICK TIME.**

---

## GATE RESULTS (final binary)

| gate | result | evidence |
|---|---|---|
| G-0 stripper | ✅ PASS | 0 changed / 80 no-change |
| G-1 clean rebuild | ✅ PASS | editor **and** game targets, exit 0 |
| G-2 catalog 8 → 9 | ✅ PASS | `9 anomaly type(s) registered`; `corrupted_texture \| scope=object \| usage='' \| args: (none)` |
| G-4 static pink | ✅ PASS | solid magenta, Lit, opaque, no Lumen bleed, no UI |
| **G-4S skeletal pink** | ✅ **PASS (blocking) — OWNER-CONFIRMED BY EYE, 2026-08-21** | `CharacterMesh0 -> pink on 2 slot(s)` + `Jetpack -> pink on 1`; `SKM_Bot` / `SkeletalMeshComponent`, full-span `n=8`. **Renders solid pink, NOT default-gray, out of the cooked artifact** ⇒ `used_with_skeletal_mesh` survived the cook (G49). 🎯 **The owner judged the delivered frame directly — "pink confirmed" — which is the instrument this gate was designed around: pink-vs-gray needs no threshold, and no numeric colour test was invented for it.** |
| G-5 apply/revert/re-apply | ✅ PASS | every revert `restored/default-reset/left-to-game=0/unresolved=0/swept=0` |
| G-6 isolation | ✅ PASS | per-component override; shared asset never mutated |
| G-7 full-span labels | ✅ PASS | `corrupted_texture` `n=8` beside `blink`'s gapped `n=4`; zero unregistered-id warnings |
| **G-8 pool + mask** | ✅ **PASS (blocking)** | fired 3× from the pool; `MEASURED_NONZERO` **104,300 px (11.32 %)** and **25,609 px (2.78 %)**; the third measured `MEASURED_ZERO` and was **correctly vetoed** ⇒ the m26 cure reaches the new id |
| G-9 default pool | ✅ PASS | `Default pool: blinking, missing_texture, corrupted_texture` — `lod_popping` absent, **0 occurrences in the whole log** |
| G-10 dropdown | ⚠ NOT RUN LIVE | 69/69 dashboard tests, clean build |
| G-11 `missing_texture` regression | ✅ PASS | fires, full-span, `MEASURED_NONZERO`; source **byte-unchanged** |
| G-12 two-repo hygiene | ✅ PASS | see commits |
| G-13 G139 string from artifact | ✅ PASS | A44 both encodings, **both directions** |
| G-P3 single-LOD guard | ✅ PASS | fired 5× unprompted on real content |
| **G-P1 visible pop** | 🚫 **NOT CLAIMED** | visibility is now **demonstrated** (66,615 px at 33 % coverage), but `lod_popping` is not in m29's pool, so the gate does not apply to what ships |
| G-P2′, G-P4 | ⏭ DEFERRED TO m30 | both gate the proximity gate, which is not in m29 |

**Leg conditions, every leg:** 1280×720 windowed · `Config 2 4 8 4 0` · fps 30 · delivery OFF · SVE ·
**output height 0 = NATIVE (m28 compiled default, read from the `StartRun` echo)** · mask **ON** from
`DefaultGame.ini`.

**Build identity (G121, the quartet).** Final: exe **`1ABB8E3C`** · utoc **`FB7F958A`** ·
ucas **`A359878A`** · pak **`65C060A3`**. Preserved beforehand and hash-verified 6/6 at the new
location: `_binary_baselines\m28-precook-build\` and `_binary_baselines\m29-gate1-build-14F45C34\`.
⚠ The cooked container now also carries `CB_LodCalib`; the map gate reports it as an **unexpected
entry (exit 2)** and that is **expected for this build** — it was deliberately added and deliberately
not written into the gate's expected set, so the gate keeps an independent voice.

---

## ALSO IN m29

- **`lod_popping` timing converted to FRAMES**, mirroring F-BLINK. ⚠ 2 Hz at 30 fps is **7.5 frames**,
  so "reproduce exactly" was arithmetically impossible; chose **8**, the first half-period the old
  code yields. Recorded rather than rounded silently.
- **`lod_popping`'s ≥2-LOD guard**, proven firing 5× unprompted on real content.
- **G139 `IAI.Capture.Mask` help string** corrected — clears m27 RULING 2 on the cook it required.
- **Two stale catalog arg-specs**: `blinking` still declared a float `hz` arg, **stale since m23**.
- **`PRE-DELIVERY-CHECKLIST`** gains a **categorical** catalog box plus a delivered-pool box;
  **`setup-runbook`** stops asserting *"seven"*, which it had since m3 while the catalog was 8 from m8.

## CORRECTED RECORDS

- **`missing_texture` is CHECKERED, not magenta.** The live docs were already right
  (`architecture.md`); only two chat handoffs said magenta, and **historical handoffs are not
  rewritten**. The flat-magenta variant deferred at m8 (`G50`) ships here as its own id.
- **`camera_clipping` and `lod_popping` left the dashboard by an OWNER SCOPE DECISION for the M1
  release, NOT by any architectural rule excluding globals from capture surfaces. Any note implying
  such a rule is HISTORY, NOT POLICY.**

## KNOWN AND ACCEPTED — owner aware, not a blocker

At high coverage the label bbox covers the whole target while only the silhouette edge changes — an
**over-claiming label**, the same accepted class as the foliage over-claim. Recorded, not fixed.

## 🆕 G150 / T4

Adding a pool id re-rolls the seeded draw ⇒ **banked auto-pool runs are non-comparable across a
pool-membership change** (`G140`'s shape). Any regression leg for an existing anomaly must be
**TARGETED**. m29 adds one id (`corrupted_texture`), so this applies to it.
