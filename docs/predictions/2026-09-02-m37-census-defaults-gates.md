# `m37` — census selection defaults: gates and results

> 🔢 **PROVENANCE OF THE PRE-DECLARATION, STATED HONESTLY BECAUSE IT MATTERS.** These gates were
> **pre-declared in journal 067 §14 and committed at `3105297`, before ANY `m37` source existed.**
> That commit is the pre-declaration. **THIS FILE POST-DATES THE MEASUREMENT** and records the gates
> beside their results — it is a **RESULTS file**, not a predictions file, and it is named and read
> as one. ⛔ Nothing here was invented after the fact: every gate below appears in `3105297` in the
> same words.

**Binary:** `6C80E872` (from `D2BB25A5`). **Container UNCHANGED** — `utoc 2A66CA57` ·
`ucas A7EF9B12` · `pak D8009AD7` — **exe-only swap** (`G103`), no cook, **no tag**.

## What `m37` is

- `CensusMinDrawnCoveragePct` compiled default **6.0 → 0.5**
- **NEW** `CensusMaxDrawnCoveragePct`, compiled default **25.0**, **INCLUSIVE**: eligible **iff**
  `floor ≤ coverage ≤ ceiling`
- `MEASURED_NONZERO` above the ceiling → **EXCLUDED categorically.** At scenery scale **the LABEL is
  unusable — that is not the same as the anomaly failing.**
- New counter `census_above_ceiling` (`run_summary`, the `CYCLE n DONE` line, the `SUMMARY` line) and
  one greppable per-exclusion token
- **Ceiling ≤ 0 ⇒ DISABLED, stated in the `StartRun` echo** so a disabled ceiling can never read like
  a healthy one
- **Census compiled default stays OFF** ⇒ a defaults change inside a compiled-OFF feature is
  **client-inert**

---

## THE GATES — ✅ **ALL FOUR PASS**

### (a) `P-C7` census-OFF byte-identity, RE-ANCHORED at the new build boundary ✅

`M36_M36_MERGE_INERT_MASTER` (on `D2BB25A5`) vs `M37_M37_GA_PC7` (on `6C80E872`), same config:
`CB_GateLevel`, `blinking`, `StaticMeshActor_49`, paced, delivery OFF, `MaskReduce both`, **no census
command** (compiled default OFF).

```
== KNOWN-ANSWER PROOF 1: A vs A must PASS ==     self-comparison clean: PROOF 1 OK
== KNOWN-ANSWER PROOF 2: A vs perturbed-A must FAIL ==
                                     perturbed comparison reported 2 difference(s): PROOF 2 OK
== P-C7 VERDICT COMPARISON: A vs B ==
[pair] annotation keyset IDENTICAL (48 keys)
[pair] event set IDENTICAL (8 events)
[pair] run_summary keyset IDENTICAL (48 keys)
[pair] no census_* keys in either run_summary (S1 contract)
[pair] run_summary values identical outside the declared run-unique set
[pair] frame count identical (90)
P-C7 PASS: census-OFF artifacts identical outside the declared run-unique set     exit=0
```

✅ **The checker proved itself in BOTH directions before its verdict was read** (`G96`). ✅ **The
client-inert claim now rests on a measurement at THIS binary**, not on the previous one.
📌 *"no `census_*` keys in either `run_summary`"* also confirms the **new** `census_above_ceiling`
key is absent when the census is off — the key set did not move for a client build.

### (b) MECHANISM control for the ceiling ✅

`M37_M37_GB_CEIL5` — console ceiling **5.0**, deliberately **below** a known candidate.

```
Capture(census): EFFECTIVE FOR THIS RUN - census ON (...), floor=0.50%(from COMPILED DEFAULT (0.5)),
  ceiling=5.00%(from IAI.Capture.CensusCeiling (console))
  [band is INCLUSIVE: eligible iff floor <= coverage <= ceiling], ...

Census: ABOVE-CEILING 'StaticMeshActor_0' drawn=41655px (6.071%) > ceiling 5.00% - EXCLUDED
  (label unusable at scenery scale, not a failed anomaly).          [43 such lines this run]

Census: CYCLE 8 DONE ... belowFloor=45(floor 0.50%) aboveCeiling=2(ceiling 5.00%) ...

run_summary: census_above_ceiling = 2
```

🚨 **THIS TESTS THE MECHANISM, NOT THE DEFAULT — AND THE PLAN SAID SO BEFORE IT RAN. VERBATIM FROM
JOURNAL 067 §14, COMMIT `3105297`:**

> *"THIS TESTS THE MECHANISM, NOT THE DEFAULT. The bench map's largest candidate is ~6.06 %, so
> nothing on it exceeds 25 % and **THE DEFAULT IS UNTESTABLE HERE** — a bench that cannot exhibit the
> case would give a clean pass that means nothing (`G96` / `G135`). **The default 25 is
> Bates-validated LATER**, on the host whose landscape reads 34 %."*

⇒ **`m37` ships a default this bench CANNOT validate, and that is stated rather than papered over.**
The Bates validation is the RDP card's optional "ceiling validation on Bates" line.

### (c) Floor-behaviour spot-check at 0.5 ✅ — **the decisive one for the floor**

| leg | exe | floor source | candidates | zero | belowFloor |
|---|---|---|---|---|---|
| `P9V2_A` | `D2BB25A5` | **console** 0.5 | 77 | 13 | **45** |
| `M37_GD_CEILOFF` | `6C80E872` | **COMPILED DEFAULT** 0.5 | 77 | 13 | **45** |

✅ **Identical, across two different binaries** — the new compiled default reproduces exactly what the
old console-set value produced. **A default that silently failed to take is the failure mode this
gate exists for, and it did take.** Provenance confirmed in the echo:
`floor=0.50%(from COMPILED DEFAULT (0.5))`.

### (d) Disabled-ceiling echo check ✅

`M37_M37_GD_CEILOFF` — console ceiling **0**.

```
ceiling=DISABLED (<=0; NO upper bound is applied and scenery-scale targets ARE eligible)
  (from IAI.Capture.CensusCeiling (console))

IAI.Capture.CensusCeiling: 0.00 - the ceiling is DISABLED (<= 0). NO upper bound is applied and
  scenery-scale targets ARE eligible. This is a real setting, not an error, and StartRun says so
  out loud so a disabled ceiling can never read like a healthy one.

ABOVE-CEILING token count: 0        HISTOGRAM NOTE count: 0        census_above_ceiling: 0
```

✅ **Loud in both directions** (`G96`): the off state announces itself in the echo, in a warning at
set time, and by the total absence of exclusions and of the histogram note.

---

## The three effective render settings — the owed read-back, DISCHARGED

Read bare from the console on the gate-(d) leg:

```
r.AntiAliasingMethod = "4"      LastSetBy: Constructor
r.MotionBlurQuality  = "4"      LastSetBy: Scalability
r.ScreenPercentage   = "100"    LastSetBy: Scalability
```

✅ **This CONFIRMS BY MEASUREMENT what was previously only a source read:** the bench runs
`r.AntiAliasingMethod = 4 = TSR`, the engine default (`SceneView.cpp:209-218`), set by the
**Constructor** — i.e. no ini and no scalability profile overrides it, exactly as the silent
`DefaultEngine.ini` implied. ⚠ **Recorded as an axis reading only. It is NOT an argument about `P9`'s
(A) or (B).**

---

## Comparability

⚠ **A census-ON defaults change is a NEW `G140`-family baseline boundary.** The same seed now selects
from a different eligible set, so **census-ON legs banked before `6C80E872` are NOT comparable to
census-ON legs after it.** ✅ **Census-OFF legs stay comparable — that is exactly what gate (a)
proves.** **Boundary binary: `6C80E872`.**

## Not done

⛔ No cook, no tag, no container change, no selection/veto/mask/cadence change, no ini keys for any
host (they land only when the census ships ON, a delivery-precondition lane).
