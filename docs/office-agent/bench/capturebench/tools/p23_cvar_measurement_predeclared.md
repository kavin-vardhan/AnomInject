# PART TWENTY-THREE — `M-1`..`M-3`: PRE-DECLARED BRANCHES

**Written and committed BEFORE any measurement.** ⛔ **MEASUREMENT ONLY. NO FIX — there is no
same-turn fix to a validity instrument, and the mask IS this milestone's validity instrument.**

**The last unmeasured link.** Source has exonerated every other link (`F-1` the proxy timing, the
post-Tonemap pass point, the cvar priority). **This one is measured, not reasoned about.**

---

## The branches — restated VERBATIM from the brief

> **MODE 3 AT PASS TIME AND CUSTOM DEPTH PRODUCED** ⇒ the cvar is exonerated, the fault is elsewhere,
> and source-only diagnosis has now failed on every link. **That is a HARD result and it changes the
> approach — report it, do not improvise a next candidate in the same turn.**
>
> **MODE NOT 3, OR CUSTOM DEPTH NOT PRODUCED** ⇒ the mechanism is established and the fix is whatever
> makes it 3 at pass time. **Report the cause; do not apply the fix in the same turn.**
>
> **THEY DISAGREE (mode 3 but not produced)** ⇒ **a separate finding, and the more interesting one.
> Report it.**

⚠ **A fourth outcome must be named or it will be mistaken for one of the above:**

| # | outcome | meaning |
|---|---|---|
| **B4** | **the instrument does not report** — no lines, or the leg fails for a HOW-IT-RAN reason | ⛔ **NOT a result.** Discard, bank, re-run (A63) |

---

## `M-1` — the EFFECTIVE mode, read WHERE IT IS CONSUMED

**Read `r.CustomDepth` on the RENDER THREAD, at the pass point our mask runs, on the frames we arm.**
⛔ **NOT via `-ExecCmds`** — that is startup-only and would report the default (**1**), which is
precisely the value under suspicion, so a console read would look like evidence and be none.

`GetCustomDepthMode()` maps **1 → `Enabled`**, **2 → `Enabled`**, **3 → `EnabledWithStencil`**,
anything else → `Disabled`.

## `M-2` — the DOWNSTREAM FACT, reported directly

**`bCustomDepthProduced` is the quantity that actually selects the dummy**
(`SceneTextures.cpp:959`). **The cvar is the cause; this is the effect. Both are reported.**

✅ **Direct discriminator, decided in advance: the engine's `StencilDummy` is a `1 × 1` texture
(`SystemTextures.cpp:250`). The real custom-stencil texture is view-sized.** ⇒ **logging the bound
`CustomStencilTexture`'s EXTENT settles it without inference: `1x1` = dummy = not produced.**

**Corroborating, from data the reduction already computes:** the pixel count carrying the unassigned
tag. **Uniform across the frame ⇒ a 1×1 dummy sampled everywhere** (`F-5`'s discriminator).

## `M-3` — the value OVER TIME, not at one instant

`StartRun` sets 3. **Report the game-thread value at `BeginRun`, at every arm, and at `FinishRun`**,
so a level load, a scalability apply, or the run lifecycle resetting it is visible rather than
assumed.

---

## ⚠ `F-4`'s COUPLING MUST NOT CONFOUND THIS

**Tags persist to `FinishRun`** — so a later event measures under conditions an earlier one created,
and that is already known to have changed a result (the one clean event in `P20_M26S1_RAMP` saw no
255 because earlier tags had made custom depth render).

⇒ **THE INSTRUMENT REPORTS PER ARMED FRAME, NOT PER EVENT, AND NOTHING IS AVERAGED.** If the mode or
the produced-flag changes across a run, that change must be visible in the trace. **A per-event
summary would hide exactly the effect `F-4` predicts.**

---

## Conditions

- **LOG ONLY.** No artifact field, no behaviour change, no veto. `mask{provided}` stays `false`.
- ⛔ **The stencil range stays 200/255.** ⛔ **No tag/arm separation** — withdrawn, refuted at source.
- ⛔ **NO H5 LEGS** until both controls read NON-ZERO (Ruling 2).
- **A63**: bank every attempt. **B1** where on-calibration, **declared NOT APPLICABLE** otherwise
  (`G117`). No A54 verdict on MainWorld targets.
- A code-only change ⇒ **exe hot-swap, no cook** (`G103`) — the shader is already in the container
  from the `m26` cook, and **`G131` does not re-apply because no shader is being added.** ⚠ **The
  boot still has to be checked: a build that cannot start reports nothing.**

*(Written 2026-08-20, PART TWENTY-THREE, before any measurement.)*
