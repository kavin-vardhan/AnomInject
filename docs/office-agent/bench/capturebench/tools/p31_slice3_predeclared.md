# P31 — `m26` SLICE 3 (THE VETO). Pre-declared BEFORE implementation and before any leg.

Session 2026-08-20, journal PART THIRTY-ONE.

## THE RULE, AS RULED — and nothing more

> **VETO IF AND ONLY IF `State == MEASURED_ZERO`** (i.e. `mask.provided == true` AND the measured
> pixel count is 0), **AND `manifested == true`** (`A-1`).

**NOT VETOED, under any circumstance:** `NOT_MEASURED` (`provided:false`) — never measured, MUST
ADMIT · `MEASURED_NONZERO` — **any count ≥ 1 pixel, however small a fraction of the claimed area**
· `manifested == false` events (`A-1` precedence: `m23` logic byte-unchanged, evaluated first).

⛔ **NO RATIO. NO THRESHOLD. NO COMPARISON AGAINST CLAIMED AREA.** The implementation tests the
**enum state only** — not `MaxCount`, not a percentage, not a constant. **No constant is introduced
that could later become a threshold, and no identifier is named as though a ratio is coming.**

## Predicted results, per leg

| leg | target | measured state | **vetoed?** | events in `annotation.json` | `vetoed_events` |
|---|---|---|---|---|---|
| **A** `P31_S3_SPLINE` | `BP_SplineSpawn_C` | `MEASURED_ZERO` ×8 | 🎯 **YES ×8 — IT FIRES** | **0** (all removed) | **8** |
| **B** `P31_S3_CTRL49` | `StaticMeshActor_49` | `MEASURED_NONZERO` ×8 | **NO** | 8 | 0 |
| **C** `P31_S3_CYL73` | `StaticMeshActor_73` | `MEASURED_NONZERO` | **NO** | = measured events | 0 |
| **D** 🚨 `P31_S3_FOLIAGE` | `InstancedFoliageActor_0_0_0` | `MEASURED_NONZERO` ×8 (5,689–13,342 px vs a 921,600 px claim) | 🚨 **NO — NOT VETOED** | 8 | **0** |
| **E** `P31_S3_RAMP` | `SM_Ramp2` (known-Nanite) | `NOT_MEASURED` ×8 | **NO — admits when blind** | 8 | 0 |
| **F/G/H** `P31_S3_G9_{OFF_A,OFF_B,ON}` | `StaticMeshActor_49` | `MEASURED_NONZERO` | **NO** | 8 | 0 |

🚨 **LEG D IS THE RULE'S OWN GUARD.** The foliage draws ~1.4 % of what it claims and **is NOT
vetoed**, because the rule is zero-only. **If leg D shows any veto, A RATIO HAS CREPT IN SOMEWHERE
AND THAT IS A FAIL** — no matter how sensible the resulting number looks.

### ⚠ A CONSEQUENCE THAT MUST BE STATED IN ADVANCE, NOT DISCOVERED

**Leg A vetoes all 8 of its events, so its `annotation.json` will contain ZERO anomalies.** That is
**the veto working**, and it is simultaneously **below `G-11`'s 3-event validity floor**. The two
readings are reconciled by the leg's ROLE, fixed here before it runs:

- **Leg A is a DEMONSTRATION leg (`G-4` "it fires"), NOT a certifying leg.** Its purpose is to show
  the veto removes what it should. **It is not graded by `A54` and certifies nothing about
  alignment.**
- **`G-11`'s floor applies to CERTIFYING legs** — legs whose results are used as evidence about
  something other than the veto itself. **A certifying leg dropping below 3 counted events after
  the veto is INVALID and must be REPORTED as invalid, never silently graded on the reduced set.**
- ⛔ **An empty `anomalies[]` must not crash or silently mislead any downstream reader.** Checked
  explicitly on leg A.

## GATES

| # | gate | pass condition |
|---|---|---|
| **G-4** | **IT FIRES** | leg A: events **absent** from `anomalies[]`, `vetoed_events == 8` |
| **G-5** | **IT DOES NOT OVER-FIRE** | legs B, C **not vetoed** · 🚨 **leg D not vetoed** |
| **G-7** | **ADMITS WHEN BLIND** | leg E: `NOT_MEASURED` ⇒ not vetoed, `vetoed_events == 0` |
| **G-8** | **THE BLIND CASE IS LOUD** | absence logged AND in the artifact (`mask.provided:false` + `mask_nopass_discards`) |
| **G-9** | **DELIVERY ORTHOGONALITY, RE-RUN AT SLICE 3** | pose-matched OFF/ON pair (route (a)); **EXTRAS = 0**; invariant core identical **including `vetoed_events` and the event set** |
| **G-10** | `A54` still certifies on the control | leg B: ≥3 counted events after the veto |
| **G-11** | **COUNTED EVENTS BEFORE AND AFTER, AS TWO NUMBERS** | logged and in `run_summary`; below 3 on a CERTIFYING leg ⇒ that leg INVALID, reported |
| **P6** | field SET unchanged, both modes | `annotation.json` **48/48**; `run_summary` **+4** (`vetoed_events` added) |
| **A-1 DISJOINT** | the two counters never double-count | `vetoed_events` and `non_manifested_events` disjoint **by construction** — `manifested` is evaluated FIRST and a non-manifested event is never vetoed |

## FAILURE BRANCHES

| # | reading | action |
|---|---|---|
| **F-1** | 🚨 leg D (foliage) shows ANY veto | **a ratio has crept in ⇒ HALT** |
| **F-2** | 🚨 leg E (`NOT_MEASURED`) shows ANY veto | **the admit bias is broken ⇒ HALT.** This is the data-destroying direction |
| **F-3** | leg A vetoes fewer than 8 | the rule is not firing where it must ⇒ report, attribute nothing, HALT |
| **F-4** | any annotation key added/removed, or a sub-field under `mask` | **`P6` MOVED ⇒ HALT** |
| **F-5** | `vetoed_events` + `non_manifested_events` double-count any event | `A-1` violated ⇒ HALT |
| **F-6** | G-9 EXTRAS ≠ 0 | delivery orthogonality broken by the veto ⇒ HALT |
| **F-7** | mask OFF ⇒ any veto | not inert when off ⇒ HALT |
| **F-8** | empty `anomalies[]` crashes or silently misleads a reader | ⇒ report and HALT |

## LIMITS `L1`–`L3` — to be written into the docs, not discovered later

**`L1`** the frames are **already on disk** and are **NOT un-written** — the veto removes the EVENT,
not the PNGs. **`L2`** a post-`m26` event count is **NOT comparable** with a pre-`m26` one;
`vetoed_events` carries the delta. **`L3`** `labels.jsonl` (delivery OFF) is **prebuilt and cannot
be corrected**, so **delivery OFF and ON WILL DISAGREE on event content** — stated, not reconciled.

⛔ **Constraints: no ratio, no threshold, no constant that could become one · `P6` does not move
(no sub-fields under `mask`; `vetoed_events` is `run_summary`, which is not `P6`) · stencil range
200/255 · CB_GateLevel untouched (`G99`) · `feature/stencil-capture` READ-ONLY · NO TAG (the owner
smoke gate comes first).**
