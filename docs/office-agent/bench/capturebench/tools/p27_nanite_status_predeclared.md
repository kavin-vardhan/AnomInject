# P27 / T-1 — Nanite status of the target set, pre-declared BEFORE any asset is read

**Written and committed before the first read.** Session 2026-08-20, journal PART TWENTY-SEVEN.
Follows the `p18`/`p21`/`p23`/`p24`/`p26` discipline.

## The question, and why it outranks the three rulings

`G134` established that C-1 cannot see Nanite geometry on UE 5.1. **If the two `H5` instances that
motivated the cure are themselves Nanite, the cure cannot measure the very instances it exists
for.** That is a scope-level fact about `m26`, not a defect, and it must be settled before the
replacement-control search (T-3) or the scope statement (T-4) can mean anything.

## Targets to classify

| # | actor | why it is in the set |
|---|---|---|
| 1 | `InstancedFoliageActor_0_0_0` (MainWorld) | `H5` class (ii) instance #1 |
| 2 | `BP_SplineSpawn_C` (MainWorld) | `H5` class (ii) instance #2 — not foliage |
| 3 | `StaticMeshActor_49` (CB_GateLevel) | 🚨 **THE DISCRIMINATOR.** It MEASURED SUCCESSFULLY all milestone. If it is non-Nanite, that is WHY, and the explanation closes |
| 4 | `SM_Ramp2` (MainWorld) | confirm PART TWENTY-SIX's signature read |
| 5 | CB_GateLevel's target set generally | does the whole calibration bench sit on non-Nanite geometry? |

## Method, declared in advance, with its confidence claim

**M-A (primary, offline, deterministic): `.uasset` name-table signature.**
UE tagged-property serialisation writes a property's NAME only when its value differs from the
class default. `FMeshNaniteSettings::bEnabled` defaults to **false**. So on a `UStaticMesh` asset:
`NaniteSettings` **and** `bEnabled` both present in the package name table ⇒ the struct was
serialised with a non-default sub-field, and `bEnabled` was one of them.
⚠ **STATED WEAKNESSES, so the claim is not overstated:** (1) `NaniteSettings` can serialise
because a DIFFERENT sub-field is non-default while `bEnabled` stays false; (2) `bEnabled` is a
generic name and could belong to another object in the same package. **Neither weakness survives a
DIFFERENTIAL result** — if some meshes carry the pair and others do not, and the split matches the
measured behaviour (`StaticMeshActor_49` measurable, `SM_Ramp2` not), the signature is doing real
work. **Confidence to be stated as EVIDENCE, never as a measurement.**

**M-B (corroboration, only if available): the editor over the MCP bridge.**
⚠ **`G97`/`A59` GOVERN IT ABSOLUTELY: no bridge reading is attributed to this project until
`Paths.project_dir()` AND `get_engine_version()` have been read back and printed beside the
result.** The bridge has previously answered for a different game on a different engine version.
If identity fails, **M-B is ABANDONED, not worked around**, and M-A stands alone with its stated
confidence.

⛔ **READ-ONLY. No runs, no legs, no asset modified, nothing opened for write.**

## PRE-DECLARED BRANCHES — restated verbatim from the ruling

> **BOTH H5 TARGETS NON-NANITE** => the cure applies to the instances that motivated it. `m26`
> proceeds with a stated Nanite limit. Continue to T-2.
> **EITHER OR BOTH NANITE** => the cure cannot measure the `H5` instances on this bench. That is a
> SCOPE-LEVEL finding, not a defect. Report it, complete T-2 (it is a correctness fix regardless),
> and HALT before T-3 and T-4. The decision that follows is mine.
> **MIXED** => report which, and treat the Nanite one as the second branch.

## Additional pre-declared readings (so a surprise cannot be reframed after the fact)

| # | reading | what it would mean |
|---|---|---|
| **N-1** | `StaticMeshActor_49`'s mesh carries NO Nanite signature | the discriminator holds; measurability tracks the signature; `G134`'s explanation CLOSES |
| **N-2** | 🚨 `StaticMeshActor_49`'s mesh DOES carry the signature | **the signature does NOT predict measurability** — `G134`'s mechanism stands (it is source-read) but the asset-side test is refuted as a classifier, and every classification below is void. Report and HALT |
| **N-3** | an `H5` target resolves to a mesh that cannot be located on disk | classification UNAVAILABLE for it — say so; do not substitute a sibling asset |
| **N-4** | `SM_Ramp` does not reproduce PART TWENTY-SIX's signature | that finding was wrong; correct it in place |

## T-2 (Ruling 1) predictions — declared now, before the precondition is written

| leg | prediction after the extent precondition |
|---|---|
| **L5 `SM_Ramp2`** | 🚨 `MEASURED_ZERO` → **`NOT_MEASURED`** on every event, including event 1 (the clean false zero). `framesContributed=0`; a new `framesNoPass` bucket carries the 29 |
| **L1 blinking** | **UNCHANGED** — same `maxCount` figures as this turn (66516/66331/66321/66446/66852-class values), 4/4 contributed, `framesNoPass=0` |
| **L2 `missing_texture`** | **UNCHANGED** — 8/8 events 4/4, same maxCounts |
| **L3 `missing_object`** | **UNCHANGED** — 0 in-window + 4 post-revert, capped final event `NOT_MEASURED` |
| **L4 probe** | probe frame now discarded by the EXTENT precondition (it is a dummy frame) — still bucketed PROBE, still fires the 255 detector, `mask_probe_arms=1`; the probed event still `MEASURED_NONZERO` from its 3 clean frames |
| **the 255 detector** | **DEMOTED TO A SECONDARY SIGNAL** — say so in the record; the extent is now primary. Still proven live by L4 |
| **F-T2-A** | ⛔ any L1–L4 `maxCount` that MOVES ⇒ the precondition changed a passing leg ⇒ that is not the approved change ⇒ HALT |
| **F-T2-B** | ⛔ L5 still `MEASURED_ZERO` anywhere ⇒ the precondition does not close `G133` ⇒ HALT |
