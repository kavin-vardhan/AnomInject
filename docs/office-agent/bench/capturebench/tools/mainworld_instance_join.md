# MainWorld instance join — ASSET namespace <-> RUNTIME namespace

**Two namespaces name the same actors, and a claim proven in one is UNPROVEN in the other until they
are joined by an exact key.** This table is that join. It exists because the gap produced a wrong
answer on 2026-08-19: `G-1` established `Trigger` status **per external actor FILE**, a runtime motion
leg then targeted a **runtime UAID name**, and the two had never been connected — so the leg fired at a
**trigger-BOUND** Stomper while the finding under test was about the **unbound** ones. A
trigger-bound Stomper standing still with nobody on the pressure plate is the *expected* result and
tests nothing; reporting it would have been a false HALT. → **G122**.

## The exact key

Every World Partition one-file-per-actor `.uasset` contains its own object path:

```
/Game/StackOBot/Maps/MainWorld.MainWorld:PersistentLevel.<Class>_C_UAID_<HEX>_<N>
```

and `<Class>_C_UAID_<HEX>_<N>` is exactly what `IAI.ListActors` prints at runtime and what
`IAI.Apply` / `IAI.Capture.Start`'s target argument matches with `=<name>`. **That substring is the
join key. Nothing else is** — file basenames are opaque GUIDs and actor *labels* do not exist in a
cooked build (`(no-label)`), so neither can be used.

## Trigger status, and what it means

- `BP_Stomper` / `BP_MovingPlatform` carry a **scalar `Trigger`** property.
- ⚠ `BP_Fan` carries **`Triggers` (an ARRAY)** plus `TriggersActive` / `TriggersNeeded` — **keying a Fan
  on the scalar name reads every instance as empty and contradicts its own reference signal.**
- UE serialises only **non-default** property values, so **`EMPTY` below means the property is absent
  from the instance file**, i.e. left at `None`. `BOUND` means it is serialised, and in every case
  here the instance also references a `BP_PressurePlate` or `BP_Button`.

## Motion column — the point of this artifact

**`OBSERVED`** = measured moving at runtime. **`REFUTED`** = measured, and did not move. **`UNTESTED`**
= never measured. ⛔ **`UNTESTED` is NOT "presumed moving."** The asset comment
*"when no trigger is referenced it move constantly"* is an **asset fact** that has already been
**refuted at runtime for one unbound Stomper**, so it does not transfer to untested instances.

---

## BP_Stomper — 7 placed

| runtime UAID name | asset file | Trigger | runtime motion |
|---|---|---|---|
| `BP_Stomper_C_UAID_B42E9936F5429ADA00_2086831169` | `0E5JK19NZI4C74C00ZZ7N` | **BOUND** | **REFUTED** — static across a 10-frame static-camera window; *expected for a bound instance, tests nothing* |
| `BP_Stomper_C_UAID_B42E9936F5429ADA00_2086831170` | `SKQSVQZV1BKERLJ6Y2C7I` | **BOUND** | UNTESTED |
| `BP_Stomper_C_UAID_B42E9936F542F9D500_2012109606` | `RZM1H7X7G5A97WSJD4IGH` | EMPTY | **REFUTED** — 21 samples at two incommensurate cadences (spacing 12 and 7), one position `(8962.014, 8754.727, 2599.993)` |
| `BP_Stomper_C_UAID_B42E9936F542F9D500_2012110608` | `MY6AJ2QD906BZO5OSF9S0` | EMPTY | UNTESTED |
| `BP_Stomper_C_UAID_B42E9936F542F9D500_1484112590` | `9HW09I31E0N4BEZJKVCR7` | EMPTY | UNTESTED |
| `BP_Stomper_C_UAID_B42E9936F542F9D500_2012104599` | `2UMQBMC097K0DJ9SFW0I9` | EMPTY | UNTESTED |
| `BP_Stomper_C_UAID_B42E9936F542F9D500_2012105600` | `832VJD4HBF9KNK1DTJD50` | EMPTY | UNTESTED |

## BP_MovingPlatform — 7 placed

| runtime UAID name | asset file | Trigger | runtime motion |
|---|---|---|---|
| `BP_MovingPlatform_C_UAID_B42E9936F542EBDB00_1649270448` | `CCB5ZSO36QT7XLBOWXMHG` | EMPTY | ✅ **OBSERVED** — **PingPong on Z**, `1658.4 ↔ 2393.1` (**734.7 cm**), **126.67 cm/s**, period ≈ **12.1 s** (bottom→top→bottom, measured f2→f198), X/Y fixed at `(3980, 2380)`. **Repeats.** |
| `BP_MovingPlatform_C_UAID_B42E9936F542F9D500_2012115617` | `27JOXY2U0MG955JQVJLEJ` | EMPTY | ✅ **OBSERVED** — **Z**, `3239.0 ↔ 3715.0` (476.0 cm), **210.00 cm/s**, at `(9433, 7843)` |
| `BP_MovingPlatform_C_UAID_B42E9936F542F9D500_2012117619` | `J9RE3Z62T3WYD9UH90822` | EMPTY | ✅ **OBSERVED** — **HORIZONTAL**, mostly Y: `8387 → 9139` (752.5 cm) + X 132.7, Z flat, at `(≈9270, ·, 3726)` |
| `BP_MovingPlatform_C_UAID_B42E9936F542F9D500_2012105601` | `AKFKMYJOHWFZVI751P8XZ` | EMPTY | ✅ **OBSERVED** — **HORIZONTAL**, mostly X: `9793 → 10789` (995.6 cm) + Y 175.5, Z flat, at `(·, ≈9395, 3720)` |
| `BP_MovingPlatform_C_UAID_B42E9936F542F9D500_1484110587` | `DPPV0T1MIPE25BFQ6JED2` | **BOUND** | UNTESTED |
| `BP_MovingPlatform2_UAID_B42E9936F5429ADA00_2086823146` | `ZOYP8IHEJZ6NHZTT6XZDK` | **BOUND** | UNTESTED |
| `BP_MovingPlatform_C_UAID_B42E9936F54257D500_1526715066` | `UEN3HLQTVIGNOULP1TP29` | **BOUND** | UNTESTED |

## BP_Fan — 4 placed

⚠ **Keyed on `Triggers` (the ARRAY), not on the scalar `Trigger`.** Both signals agree at this key.

| runtime UAID name | asset file | Triggers | runtime motion |
|---|---|---|---|
| `BP_Fan_C_UAID_B42E9936F5429ADA00_2086843187` | `B0BU3QBIIDKKYHOG86CMN` | **BOUND** | UNTESTED |
| `BP_Fan_C_UAID_B42E9936F542F9D500_1484105580` | `XMCN2LVUE6ZI1S2WK5H8S` | **BOUND** | UNTESTED |
| `BP_Fan_C_UAID_B42E9936F542F9D500_2012101598` | `XVTNFI4353GBB6LXHGOC4` | EMPTY | UNTESTED |
| `BP_Fan_C_UAID_B42E9936F542F9D500_2012116618` | `X6JFL2CSAQ3J88Y9Y5CBO` | EMPTY | UNTESTED |

## Key sanity — verified in BOTH directions (G96)

`mainworld_join.ps1` cross-checks the two independent signals — *`Trigger`/`Triggers` serialised* and
*references a `BP_PressurePlate`/`BP_Button`* — and reports any row where they disagree. **They agree
on all 18 rows.** ⚠ **That check is what found the Fan's ARRAY key:** keyed on the scalar `Trigger`,
Fans read **4-of-4 EMPTY while 2 of 4 referenced a PressurePlate** — a disagreement, and the tool
prints exactly that case as its worked example. A key that has never disagreed with an independent
signal has not been shown to be the right key.

**Census: `BP_Stomper` 5 EMPTY / 2 BOUND · `BP_MovingPlatform` 4 EMPTY / 3 BOUND · `BP_Fan` 2 EMPTY /
2 BOUND.**

---

## Regenerate

`mainworld_join.ps1` in this directory rebuilds the Stomper/MovingPlatform rows from
`Content/__ExternalActors__/StackOBot/Maps/MainWorld/`. The motion column is **hand-maintained from
measured legs** and must never be inferred.
