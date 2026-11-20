# PART TWENTY-ONE — DIAGNOSIS `D-1`..`D-5`: PRE-DECLARED IMPLICATIONS

**Written and committed BEFORE any measurement.** ⛔ **DIAGNOSIS ONLY — NO FIX THIS TURN.** The mask
**is** the validity instrument for `m26`, and this project does not repair a validity instrument in
the same turn it diagnoses it.

**The leading candidate, from PART TWENTY, held explicitly as NOT ESTABLISHED:**

> `ReservedStencilMax = 255` puts our reserved range on top of the value an **unpopulated /
> default custom-stencil read** returns, so our tag never appears (count 0) and 255 appears instead.

🚨 **THE TRAP THIS PRE-DECLARATION EXISTS TO AVOID (owner's ruling): if the shader is not reading our
stencil AT ALL, moving the range to 100–155 changes NOTHING — every read still returns whatever the
unpopulated buffer returns. A repair that appears to work for the wrong reason is worse than one that
fails.**

---

## What would REFUTE the `ReservedStencilMax` candidate

⚠ **A hypothesis with no refuting observation is not being tested. These are the refuters, declared
in advance:**

| # | observation | effect on the candidate |
|---|---|---|
| **R1** | **Our assigned tag (200–207) appears in the mask with a non-zero count anywhere, in any event** | ⛔ **REFUTED as the whole story** — the read path works, so 255 is a *second* thing, not the cause of our zeros |
| **R2** | `r.CustomDepth` reads back **≠ 3** at run time in the packaged build | ⛔ **REFUTED / SUPERSEDED** — the buffer is not being produced at all, which is a different fault with a different fix, and moving the range would not touch it |
| **R3** | The custom-stencil texture is provably **bound and populated** at the pass point, by source | ⛔ weakens "unpopulated"; forces the cause toward a **read/format/swizzle** fault |
| **R4** | 255 occupies only a **geometry-shaped subset** of the frame rather than ~all of it | ⛔ **REFUTED** — a default/clear read is uniform; a shaped region means something is genuinely writing 255 |
| **R5** | An event with `collisions=0` (property verified intact, no unassigned tag) **still** returns count 0 | ⚠ **does NOT refute**, but it **exonerates the write side** and moves the fault to the read side |

✅ **CONFIRMING (not proving) observations:** 255 uniform across ~the whole frame · custom-stencil
texture unbound or cleared to 0xFF at this pass point · our tag never present in any event.

---

## `D-1` — IS THE SHADER READING THE STENCIL AT ALL?

**Establish from source:** which buffer `CalcSceneCustomStencil` samples, through which binding, and
whether that binding is populated at `EPostProcessingPass::Tonemap` where our pass runs.

| answer | implication |
|---|---|
| binding populated at this pass point | the read path is live ⇒ fault is in the **value** (format/swizzle/range) |
| binding NOT populated here | ⇒ **the pass point is wrong**, and no range change fixes it |
| binding populated only when custom-depth geometry exists | ⇒ ties `D-1` to `D-4`; order matters |

## `D-2` — IS OUR TAG BEING WRITTEN? *(separate the write from the read)*

`VerifyActorStillTagged` already checks the **game-thread** side and only counts a collision when the
component property does **not** read back as ours.

| answer | implication |
|---|---|
| an event with **`collisions=0`** still returns **count 0** | ✅ **WRITE SIDE EXONERATED** — property verified intact and the mask still never saw it ⇒ **read-side fault** (`R5`) |
| property read-back fails | ⇒ write-side fault; the tag is being cleared before the frame renders |

## `D-3` — WHAT IS 255, EXACTLY?

**Always 255, never 254, both levels, 25 occurrences.** Three candidates, each with a different fix:

| candidate | discriminator |
|---|---|
| **(a)** unpopulated buffer returning its clear/default value | uniform across ~the whole frame; independent of scene content |
| **(b)** format / swizzle — reading the wrong channel or an unconverted depth | value tracks something scene-shaped; changes with view |
| **(c)** something genuinely writes 255 | a **geometry-shaped** region; would differ between two unrelated levels |

⚠ **(c) is already weak: StackOBot is a sample project, and one constant value across two unrelated
levels is not a host-game signature.** ⛔ **But it is not excluded, and the detector must stop
asserting it (Task 2).**

## `D-4` — `r.CustomDepth` EFFECTIVE STATE, read back (A48), not trusted from the `Set`

| answer | implication |
|---|---|
| **3** (EnabledWithStencil) at run time | the enable is fine ⇒ fault is downstream |
| **0/1/2** | 🚨 **the buffer is not being produced with stencil at all** ⇒ `R2`, a different fault, and **the range candidate is superseded** |

⚠ **`EnableCustomStencil()` is called from `FAnomalyMaskMeasure::BeginRun()`, i.e. at capture start —
NOT at module load. Whether that is early enough for the render thread's pass is part of this
question.**

## `D-5` — THE 16th EVENT

15 of 16 showed `arms=4 resolved=4`; one showed `arms=1 resolved=1`.

| answer | implication |
|---|---|
| it is the **last** event, truncated by the 90-frame cap | ✅ benign, fully explained, no second fault |
| anything else | 🚨 **a second, unexplained fault — name it** |

---

## Conditions

- ⛔ **NO H5 LEGS until a control reads NON-ZERO.** A zero there is what the bug produces and would be
  indistinguishable from the cure working (`G96`).
- ⛔ **DO NOT MOVE THE STENCIL RANGE.** That is the repair, and the cause is not established.
- ⛔ **NO fix in this turn**, beyond Task 2's detector **message** (which changes no logic).
- **A63** applies to any leg; **B1** where on-calibration, **declared NOT APPLICABLE** otherwise
  (`G117`); no A54 verdict on MainWorld targets; bank every attempt.
- What already stands and is **not** re-opened by this fault: **`LOCK-1` is PROVEN**
  (`skippedHidden=3..4` on every event), the **tag → mask → readback plumbing round-trips**, the
  **module**, the **gates**, the **new quartet**.

*(Written 2026-08-20, PART TWENTY-ONE, before any measurement.)*
