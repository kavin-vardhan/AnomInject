# `m41` — CENSUS ON BY DEFAULT — PRE-DECLARED GATES AND PREDICTIONS

**Written and committed BEFORE any `m41` source change.** Plan: journal 069 §1 (`47acfe6`).
Approval + five rulings: session 069 brief 2.

⛔ **This file is never amended after a measurement exists.** A defective wording is annotated in the
journal, not edited here (the laundering shape). ⛔ **Any gate that fails: report the measurement and
STOP. Do not iterate in the same turn.**

---

## 0. WHAT `m41` CHANGES, AND THE TWO THINGS IT MUST NOT

`m41` makes the m36/m37 selection census the **compiled default**, flips the **mask** compiled default
with it, closes the **translucent custom-depth loophole**, adds a **host post-process preflight**, makes
**verdict expiry cycle-relative**, and adds a **coverage assertion** (candidates the census never saw).

⛔ **It must not change a rendered pixel.** ⛔ **It must not move `annotation.json`** — `P6` stays at 48
keys, measured both ways.

**`run_summary.json` key delta, census-effective only: 12 → 15.**
Existing 12: `census_frames` · `census_cycles` · `census_candidates` · `census_zero` ·
`census_below_floor` · `census_above_ceiling` · `census_excluded_translucent` ·
`census_fires_fallback_all` · `census_unmeasurable_nanite` · `census_unmeasurable_tag_failed` ·
`census_unmeasurable_hidden` · `census_unmeasurable_not_yet_measured`.
**New 3:** `census_host_pp_customdepth_readers` · `census_fires_partial_fallback` ·
`census_fires_unseen_candidates`.
📌 The record said "+11"; `m37` had already made it 12 (`census_above_ceiling`). The correction lands in
the docs commit.

**The five rulings this file gates against:** mask compiled default flips too · B's fixture is a
bench-only runtime spawn · C-G1b defers to the next cook (C-G1a runs now) · `Window = max(knob,
LastCompletedCycleTicks + LostAfterTicks)` with knob as **floor** · `census_fires_unseen_candidates` is
a **fire** count.

---

## 1. `A-G1` — INERT WHEN OFF (`P-C7` RE-ANCHOR)

**Leg:** `m41` build, `IAI.Capture.Census 0`, everything else at the bench standard, pose-matched
against a census-OFF control.

**PREDICTED:**
- `run_summary.json` contains **NO key beginning `census_`**.
- `frame_indices` identical to the control; `labels.jsonl` **0 row diffs** when keyed by
  `session_index`.
- `run_summary` differs from the control only by the declared run-unique fields.
- The log contains **no** `HOST-PP CUSTOM-DEPTH READERS` line and **no** `Auto.Fire: census consulted=`
  line.

**WHY IT CAN BE TRUSTED:** the provider is registered only inside the `bCensusEffective` guard
(`AnomalyCaptureSubsystem.cpp:1834`) and cleared at `:3322`, so with the census off `CensusQuery` is
null and the selection loop is the pre-m36 code. **Structural, then measured.**

**FAILURE BRANCH:** any `census_*` key present, or any row diff ⇒ **the inert path is broken and `m41`
is not shippable.** Report and STOP.

---

## 2. `A-G2` — PROVENANCE ECHO, BOTH DIRECTIONS (`G96`)

**Leg (a):** no ini key, no console. **Leg (b):** `IAI.Capture.Census 0`.

**PREDICTED (a):** the census `EFFECTIVE FOR THIS RUN` line reads census **ON**, requested **on**, from
**`COMPILED DEFAULT (on)`**; the mask line reads **ON** from **`COMPILED DEFAULT (on)`**.
**PREDICTED (b):** census **off**, from **`IAI.Capture.Census (console)`**.

🚨 **A DISJUNCTION IN EITHER BRANCH IS THE DEFECT, NOT A COSMETIC MISS.** The pre-`m41` string
`"COMPILED DEFAULT (off) or IAI.Capture.Census"` is *false* once the compiled default is ON, and it is
`G139`'s own failure mode living inside `G139`'s fix. Each branch must name **exactly one** source.

**PREDICTED, the reworded inactive warning:** on a default leg it must **not appear at all**. It appears
only when the mask was turned off by console or ini, and its text must say so.

---

## 3. `A-G3` — KEY-SET SUBSET · `A-G4` — `P6` UNMOVED

**`A-G3` PREDICTED:** census-ON leg vs a pre-`m41` census-OFF control ⇒ the `run_summary` key delta is
**exactly the 15 `census_*` keys named in §0**, nothing else added, nothing removed.
**`A-G4` PREDICTED:** `annotation.json` **48 keys**, added 0, removed 0, on both a census-ON and a
census-OFF leg.

**FAILURE BRANCH:** a 16th key, or any non-`census_` addition ⇒ report the name and STOP.

⚠ **DECLARED IN ADVANCE: `A` CANNOT BE CERTIFIED ON THIS BENCH.** Maximum drawn coverage here is ≈6 %,
so the 25 % ceiling never bites and floor 0.5 admits nearly everything measured non-zero. Selection may
be **near-identical** to a census-OFF leg. That is **expected and is not evidence the flip works** —
**Bates (Section E) is the instrument.** A bench leg showing no selection change is PASS for A-G1..G4
and says nothing about A's product effect.

---

## 4. `B-G1` — THE TRANSLUCENT LOOPHOLE, BOTH DIRECTIONS

**Fixture:** bench-only `IAI.Bench.SpawnTranslucentProbe 1` (console-only, default absent, never in a
client payload). It spawns a probe in front of the settled camera and prints a **candidate table** —
every material it considered, with that material's **blend mode** and **`IsTranslucencyWritingCustomDepth()`**.

**PREDICTED, knob `IAI.Capture.CensusTranslucentWriters` OFF (the new default):** the probe appears in
the `CYCLE n NOT-MEASURED` listing as **`EXCLUDED(translucent)`**; `census_excluded_translucent` ≥ 1; it
never appears in an `Auto.Fire: CENSUS '<name>' -> Eligible` line.

**PREDICTED, knob ON:** the same probe reads **`MEASURED_NONZERO`** with a plausible `drawnPct`, and
`census_excluded_translucent` is one lower.

🚨 **PRE-DECLARED HONEST-ABSENCE BRANCH (ruling 2).** The ON direction requires a material that is
**translucent-blend AND opts into custom-depth writes**. That flag is a compile-time `UMaterial`
property; it cannot be created at runtime and a `UMaterialInstanceDynamic` inherits it from its parent.
**If the staged container holds no such material, the ON direction is UNOBTAINABLE on the bench** — the
lever must say so by name, the candidate table is the evidence, and **B-G1's ON direction rides the next
cook alongside `C-G1b`**. ⛔ That is a declared limitation, **not** a pass, and **not** a failure —
report it as UNOBTAINABLE with the table.

**FAILURE BRANCH:** the probe reads `EXCLUDED(translucent)` with the knob **ON** ⇒ the fixture is wrong,
not the code. Report `GetBlendMode()` and the opt-in flag from the candidate table. **Do not adjust the
fixture until the cause is named.**

---

## 5. `C-G1a` — THE HOST-PP PREFLIGHT MECHANISM (runs now)

**`C-G1b` (an authored CustomDepth-sampling post-process material, both directions) is DEFERRED to the
next cook by ruling 3, and becomes a REQUIRED pre-delivery gate there.**

**`C-G1a` PREDICTED, on a standard census-ON bench leg:**
- exactly one `Capture(census): HOST-PP CUSTOM-DEPTH READERS = N (scanned V volume(s), C camera
  blend(s), M material(s))` line;
- `N = 0` at **Log** verbosity on this bench (no known CustomDepth-reading post-process here);
- 🔑 **the `scanned` counts are the gate.** A `= 0` with `scanned 0/0/0` is **BLINDNESS, NOT A CLEAN
  READ** and is a FAILURE. At least the camera-blend or material count must be non-zero, proving the
  enumeration and the shader-map query ran.
- `run_summary.census_host_pp_customdepth_readers == 0`.

**Mechanism proof without an authored fixture:** the scan additionally reports, for at least one
enumerated material, whether it uses **`PPI_PostProcessInput0`** — a texture id essentially every
post-process material reads. **PREDICTED: at least one enumerated material reports it true**, which
proves `FMaterialShaderMap::UsesSceneTexture` is being read correctly and that a `CustomDepth` false is
a reading rather than a dead code path.

**FAILURE BRANCH:** `scanned 0/0/0`, or no material reports any texture id true ⇒ the query path is
dead; a shipped `N = 0` would be blindness. Report and STOP.

**STATED LIMITS, which the gate does not test and the line's own text must carry:** it detects a
material *sampling* CustomDepth/CustomStencil, not that the sample changes a pixel; it cannot see a host
reader outside the material system (a host C++ scene-view extension, a custom pass, Niagara, UMG, a
decal); it is a StartRun snapshot and misses a mid-run blendable. ⛔ **`N = 0` does NOT mean "nothing on
this host reads custom depth."**

---

## 6. `D-G1` / `D-G2` — CYCLE-RELATIVE EXPIRY

`Window = max(MaxVerdictAgeTicks, LastCompletedCycleTicks + LostAfterTicks(8))`, knob = **floor**, the
existing `[0,600]` clamp is the **runaway ceiling only** (ruling 4).

🚨 **PRE-DECLARED CARVE-OUT, and it protects a shipped gate lever.** `IAI.Capture.CensusMaxAge 0` is
documented as *"0 expires everything and is the `P-C11` loud-inert control"*. Under a bare `max()` a
knob of 0 would yield a window of `cycleTicks + 8` and **`P-C11`'s lever would silently stop working**.
**`m41` therefore special-cases knob `<= 0` to a window of 0.** **PREDICTED: `IAI.Capture.CensusMaxAge 0`
still produces the all-fallback WARNING on every fire and `census_fires_fallback_all > 0`.**

**`D-G1` — A and B side ON ONE BINARY.** Bench-only `IAI.Bench.CensusBatchCap <n>` caps the per-batch
size so a cycle stretches well past 12 ticks; bench-only `IAI.Bench.CensusFixedExpiry <0|1>` forces the
pre-`m41` fixed window. 🚨 **Both levers ship in `m41` precisely so the A-side does not need a
second binary — that is `m40`'s L2 lesson applied in advance.**

- **A-side** (`CensusBatchCap 2`, `CensusFixedExpiry 1`): **PREDICTED** `expired > 0` on at least one
  fire; the per-fire line prints `window=12`.
- **B-side** (`CensusBatchCap 2`, `CensusFixedExpiry 0`): **PREDICTED** `expired = 0` on every fire, and
  `window ≈ cycleTicks + 8` — a value **strictly greater than 12**.
- **BOTH sides:** the `Auto.Fire: census consulted=… eligible=… excluded=… fallback=… expired=…
  unseen=… (window=…)` line present on **every** fire.

**`D-G2` — no regression at normal cycle length.** Standard bench leg, no levers.
**PREDICTED: `window = 12`** (the bench's short cycles put `lastCycleTicks + 8` under the knob),
`expired = 0`, `census_fires_partial_fallback` as observed, and the leg otherwise matches `A-G3`'s
census-ON control. ⇒ **byte-inert where the cycle is short, which is every host measured so far.**

**FAILURE BRANCH:** A-side `expired = 0` ⇒ the lever did not stretch the cycle (read `CYCLE n DONE
ticks=`); that is an instrument failure, not a pass. B-side `expired > 0` ⇒ the window is not being
applied. Either way report the two `window=` values and STOP.

---

## 7. `E-G1` / `E-G2` — PREFILTER ⊇ FIRE-VISIBLE-SET

**Established from source before the gate (journal 069 §1.5): the two predicates are NESTED, not
mismatched** — prefilter is renderable ∧ poll ∧ frustum; the fire set adds occlusion and screen
coverage, through the same `IsRenderableComponent` chokepoint, the same poll origin/radius and an
identical frustum construction. **An unseen candidate is therefore TEMPORAL** (an actor that entered the
view after the cycle's prefilter snapshot), not a predicate bug.

**`E-G1` PREDICTED:** on two standard bench legs (settled camera, static level)
**`census_fires_unseen_candidates = 0`**. ⚠ **A non-zero here is a FINDING, not a fault** — most likely
`Prefiltered.Num() == 0` on the fire's own tick. Report verbatim; do not adjust.

**`E-G2` — prove-it-can-fail, REQUIRED (`G96`).** Bench-only `IAI.Bench.CensusDropEntry <n>` omits every
*n*th actor from the prefilter list at `StartCycle`; those actors are still seen by the fire path and
have no census entry. **PREDICTED at `n=2`: `unseen` is roughly half the `consulted` count on the
per-fire line, and `census_fires_unseen_candidates > 0`.**

⛔ **"0 on the bench, detector unproven" IS NOT A PASS.** This counter's whole job is to be believed
when it reports zero on a client host.

**FAILURE BRANCH:** `E-G2` produces `unseen = 0` ⇒ the counter cannot report a non-zero and its zero is
blindness. Report and STOP.

---

## 8. CARRIED GATES — nothing `m41` touches may move

| gate | PREDICTED |
|---|---|
| `m38` run log | opens and closes cleanly on every leg; the close marker present |
| `m40` L4-shape | with the census OFF, `labels.jsonl` **byte-identical** (0 row diffs by `session_index`) to the `m40` control — `m40`'s order-independent sampling is undisturbed |
| `P6` | `annotation.json` **48/48** on every leg |
| `A44`, both encodings | every new string present in the STAGED exe: `SpawnTranslucentProbe`, `CensusFixedExpiry`, `CensusDropEntry`, `CensusBatchCap`, `HOST-PP CUSTOM-DEPTH READERS`, `CensusTranslucentWriters` |
| container | **UNCHANGED** — code-only hot-swap, `G103`, no cook |

---

## 9. BATES — `office-rdp-card.md` SECTION E, `E-0` PASS CONDITIONS

Pre-declared here so the card cannot drift from them:

(a) the StartRun echo reads census **ON** with source **`COMPILED DEFAULT (on)`** and mask **ON** with
source **`COMPILED DEFAULT (on)`**;
(b) the `HOST-PP CUSTOM-DEPTH READERS =` line is present **with non-zero `scanned` counts**;
(c) `aboveCeiling >= 1` with an `ABOVE-CEILING` line naming the landscape-class actor;
(d) the fog-card actor appears in the `NOT-MEASURED` listing as **`EXCLUDED(translucent)`**;
(e) an `Auto.Fire: census consulted=…` line on **every** fire;
(f) the eye list shows the anomaly visible at **≥ the m36 leg-2 rate (~90 %)**, with **no repetition of
the same 2–3 targets** and **no pitch-black frames**.

⚠ **(c) and (d) can come back different and still be RESULTS, not failures.**
`aboveCeiling = 0` ⇒ the landscape actor's drawn coverage moved (a different window/letterbox) — report
the histogram verbatim.
The fog card reading `MEASURED_NONZERO` ⇒ its material is **not** translucent-blend (most likely Masked),
so `B`'s rule does not reach it — **a finding that changes `B`'s scope, not a failed gate.**

**FURTHER PREDICTED ON BATES:** eligible-set size between m36 leg-2's ~8 and that minus the fog card and
the landscape actor · `vetoed_events` **0** (as on both m36 legs) · **`census_fires_unseen_candidates >
0`** — unlike the bench, because the PIE camera moves; that is the counter's first real reading and it
is **expected, not a fault** · `census_fires_partial_fallback > 0` for the same reason.
