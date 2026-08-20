# 2026-08-21 — 048 — m30: lod_popping proximity gate + camera_clipping

## Outcome

**m30 = two pool members, both default-checked.** `lod_popping` finishes m29's deferral with a
bounds-coverage proximity gate stacked on its ≥2-LOD guard. `camera_clipping` becomes the first
**Global-scoped** pool member: held for the whole capture session, and — the design call — labelled
per frame **only when geometry is actually within the near-clip radius**.

Delivered pool: **blinking, missing_texture, corrupted_texture, lod_popping, camera_clipping.**

---

## PART A — the proximity gate

### The bracket completed, and why m29 could not complete it

m29 stalled trying to read coverage out of `annotation.json` and hit the `-1` sentinel and an
inverted rect. **That was the wrong instrument.** The gate computes coverage itself, from bounds, at
pick time — so that computation is now instrumented and logged on every leg, and the label
projector is not in the loop at all. Its inverted-rect bug is untouched and irrelevant here.

`CB_LodCalib`, four rungs, coverage read from the gate's own computation:

| rung | distance | **bounds_coverage_pct** (gate's own number) | strong-diff px (LOD1 vs LOD4) | verdict |
|---|---|---|---|---|
| A | 1000 cm | **33.0365** | 66,615 | VISIBLE |
| B | 2331 cm | **9.3453** | 12,489 | VISIBLE |
| C | 3780 cm | **3.9045** | 14 | not visible |
| D | 5720 cm | **1.5246** | 8 | not visible |

✅ **THEY BRACKET.** Last visible **9.3453**, first invisible **3.9045**, with the visible signal
collapsing by **three orders of magnitude** between them.

🔍 **Cross-check that the instrument is sound, not merely self-consistent:** on the two rungs where
`annotation.json` also produced a number, the gate's own computation agrees exactly — **33.0365 vs
33.04** and **9.3453 vs 9.35**. Two independent routes to the same quantity.

### Threshold: 7.0 %, and its margins (A55)

- Above the invisible anchor: **7.0 / 3.9045 = 1.79×**
- Below the visible anchor: **9.3453 / 7.0 = 1.34×**

**Biased deliberately toward the safe direction.** The failure being guarded is *a positive label
with no visible change* — dataset poisoning. Refusing a borderline-visible target costs one missed
sample; admitting an invisible one poisons data. So the threshold sits nearer the visible anchor.

⚠ **ONE QUANTITY THROUGHOUT (A2).** Both anchors, the threshold and the runtime gate are all
**bounds-projected screen coverage at pick time**. Drawn extent is a different, smaller number —
the same MainWorld rock reads **11.83 % bounds** while **drawing 2.78 %** of frame, ~4×. The two are
never mixed, and every recorded number names the quantity.

### Shape

Stacked **on top of** the ≥2-LOD guard, which is unchanged and already gated. Below threshold ⇒
`Apply` returns false via the existing AMB-2 matched-zero convention ⇒ **no fire, no label**. Same
refusal path, same log shape, same zero-match-burst counter — no second refusal shape was invented.
Bounds only, no pixel read (**G127-safe**).

### A6 — FIRE RATE, REPORTED NOT TUNED

Four packaged auto-pool legs on MainWorld, **8 `lod_popping` draws: 2 survived the gate, 6 refused**
(25 %). The refusals are dominated by the **single-LOD** guard, not the new coverage gate —
MainWorld's structural geometry is largely single-LOD/Nanite. **Nothing was loosened.** This is a
number for the owner: if 25 % is too thin in real play, that is a pool-composition decision, not a
threshold decision.

---

## PART B — camera_clipping

### B1 — snapshot surface

`ControlSnapshot` now keys `auto.pool` on `Scope == Object || Scope == Global`, so `camera_clipping`
renders in the existing checkbox list with **no dashboard layout work**. ✅ **`time_dilation` also
became eligible and is VERIFIED still hidden** by `HIDDEN_ANOMALY_IDS` — checked, not assumed.

### B2 — the session-global path

A Global-scoped pool member **never routes through `TryFireOnce`**. `TryFireOnce` now skips ids whose
catalog scope is Global, so **the `"=ActorName"` token is never constructed for them** — the
misparse is removed structurally, not guarded against.

Applied in `BeginActualRun`, reverted in `FinishRun`. ⚠ **Ordering gated:** `StartRun`'s clean-slate
`RevertAllActive()` runs first and `BeginActualRun` runs after it, so the clean slate cannot revert
what was just applied. Measured every leg: `Baseline near-clip was 10.000, it is now 100.000` at
start and `Near-clip is now 10.000` at finish — **the baseline is asserted from the log, not assumed.**

### B3 — the label, and why it is not "the whole session is positive"

**A frame is labelled `camera_clipping`-positive ONLY when geometry is within the anomalous
near-clip radius of the camera.** Per captured frame: a sphere overlap at the camera at radius =
active near clip. Bounds/overlap only — **no pixel read, no same-frame pixel dependency (G127)**.

🚨 **WHY.** The near plane being wrong is not the same as the viewer seeing anything wrong. Labelling
a whole session positive would put thousands of frames showing nothing into the client's dataset —
her original complaint at scale — and **the m26 mask veto cannot catch it: no target ⇒ no mask.**

**The existing event shape expresses this with NO new field. `P6` DOES NOT MOVE** — verified: the
event key set is byte-identical (`anomaly_type, anomaly_subtype, affected_frames, manifested,
coverage_ratio, coverage_pct, affected_objects, camera, engine, mask, depth`) and `run_summary`'s key
set is unchanged. Whole-frame is carried as **`coverage_ratio = 1`** and a per-frame
**`bbox_norm = 0,0,1,1` with `bbox_valid = true`**; `asset_name` is empty because there is no target
object. `coverage_pct` stays at its `-1` sentinel, which is correct — it comes from selection
provenance, and a global anomaly has no selected actor to evaluate.

### G-C2 — the query separates, both directions

| pose | frames positive | frames negative |
|---|---|---|
| **close** — wall inside the near-clip radius | **60** | 0 |
| **open space** — MainWorld settled pose | 0 | **120** |

Same build, categorical, no tolerance.

⚠ **HONEST LIMIT: "SAME SESSION" WAS NOT ACHIEVABLE AND IS NOT CLAIMED.** Both directions are proven
on the same build across two sessions. A single session cannot show both because **no cooked level
gives camera motion relative to nearby geometry** — MainWorld's pawn settles to a fixed pose and
`CB_LodCalib`'s camera is static by construction. Stated rather than papered over.

⚠ **A NEAR-VACUOUS TEST, CAUGHT.** B4's non-interference gate was specified against a TARGETED leg —
but session globals are deliberately skipped in targeted mode, so that test would have **passed
because the condition never occurs** (`G96`'s shape). Non-interference is therefore evidenced where
camera_clipping is actually **positive on every frame** (the close-pose leg): `corrupted_texture`
`3..10 n=8` and `missing_texture` `39..46 n=8`, `51..58 n=8` — the canonical burst cadence, unchanged.

### Two iterations before the visual was worth showing (G151 in practice)

The first near-wall sat at X=315, outside the 100-unit radius — the query's `0 positive` there was
**correct**, my geometry was simply too far. The second filled the view but was **unlit**, so the
"before" was a black rectangle: technically a valid diff, useless as an eyeball artifact. Third
iteration added a dedicated wall light. **Every intermediate frame was luma-checked before being
trusted** — `cc_off mean_luma 137.46 / 100 % non-zero`, `cc_on 90.69 / 72.9 %`.

---

## GATE RESULTS

| gate | result | evidence |
|---|---|---|
| stripper · rebuild · catalog | ✅ | 0 changed / 80 no-change; editor **then** game target, both exit 0; **9 anomaly type(s) registered** |
| **G-P1** visible pop | ⏳ **OWNER** | pair sent: MainWorld rock, LOD0 vs LOD3, **2,090 strong px in-bbox** vs a 3,406 px out-of-bbox control channel (the level's movers) |
| **G-P2′** fps-independent cadence | ✅ **PASS, categorical** | `half_period_frames=8` **and 5 toggles** at **both** 30 and 60 fps. Under the old seconds design, doubling fps halves the per-frame cadence |
| **G-P4** gate must fire | ✅ **PASS** | rung D at **1.7681 %** < 7.0 ⇒ REFUSED, `Applying nothing`, **0 `lod_popping` events in `annotation.json`** |
| **G-C1** visible slice | ⏳ **OWNER** | pair sent: wall fills the view OFF, clipped away entirely ON, **54.65 % of frame differs** |
| **G-C2** query separates | ✅ **PASS both ways** | 60/0 close, 0/120 open |
| **G-C3** session lifecycle | ✅ PASS | applied after clean slate, reverted at finish, near-clip **10 → 100 → 10**, baseline read from the log |
| **G-C4** checkbox functional | ✅ PASS | `IAI.Auto.Pool camera_clipping 0` **ACCEPTED** (not silently rejected — the R1 trap, tested positively); `time_dilation` still hidden |
| **G-9′** default pool | ✅ PASS | `blinking, missing_texture, corrupted_texture, lod_popping, camera_clipping` |
| **G-8′** both new members | ✅ PASS | `lod_popping` draws from the pool and is correctly gated; `camera_clipping` active session-wide with per-frame labels gated by the query |
| **G-R** regression, TARGETED | ✅ PASS | `corrupted_texture` and `missing_texture` both 5 events, all manifested, canonical spans `3..10 / 15..22 / 27..34 / 39..46 / 51..58`, all `n=8` |
| **hygiene** map gate | ✅ PASS, **exit 0** | `CB_LodCalib` **EXCLUDED from the shipping cook**; the gate is clean with its expected set **untouched** — it was not silenced |

**Every leg:** 1280×720 · `Config 2 4 8 4 0` · fps 30 (except the G-P2′ pair) · delivery OFF · SVE ·
**output height 0 = NATIVE (m28 default, from the `StartRun` echo)** · mask ON from the ini.

**Build quartet (G121):** exe **`99AE7526`** · utoc **`3D4C02D9`** · ucas **`D15236B2`** · pak
**`BFB95333`**.

⚠ **The gates needing `CB_LodCalib` (G-P4, G-C1, G-C2, the calibration) ran on the gating build,
which differed from the shipping build ONLY by the presence of that level.** The code is identical.
Stated rather than glossed.

---

## CORRECTED

The pool coverage predicate default is **6 %** (`GMinScreenCoveragePct`), not 12 %. The new
`lod_popping` minimum (7 %) is a **separate, stricter, per-anomaly** threshold that stacks on it.
