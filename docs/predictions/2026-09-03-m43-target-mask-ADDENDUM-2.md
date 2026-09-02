# `m43` PREDICTIONS — **ADDENDUM 2**: one mask render serves every pending arm

**Committed BEFORE the source change it describes.** Parent: `docs/predictions/2026-09-03-m43-target-mask.md`
(`2691f8c`), which is **NOT edited** — gates (i)–(x) there stand as written and are re-run here.
Authorised by session 069 brief 8, ruling **(a)**.

---

## 1. THE MEASUREMENT THAT MOTIVATES THIS — taken BEFORE the change, on banked `m41` legs

Per `m26` arm: the tick it was armed on (its `RequestId`, which is `GFrameCounter`) versus the tick the
mask pass actually served it (the `M23 PASS` line's own `[GFrameCounter % 1000]` engine prefix).

| banked leg | census arms | `m26` arms | served | **UNSERVED** | lag min/max/mean |
|---|---|---|---|---|---|
| `M41_M41_ON_A` (census **ON** — the shipped `m41` default) | 78 | 24 | 22 | **2** | 1 / **3** / 1.86 |
| `M41_M41_OFF` (census OFF) | 0 | 24 | 24 | **0** | 1 / 1 / 1.00 |
| `M41_M41_OFF_B` (census OFF) | 0 | 24 | 24 | **0** | 1 / 1 / 1.00 |

🚨 **THE LATENT DEFECT IS REAL AND IT IS IN SHIPPED `m41`.** The mask pass renders once per frame and
consumes exactly one pending arm (`RequestId = PendingArms[0]`), so `m26`'s ≤4 arms per event queue
behind ~0.8 census arms per frame in the same FIFO. With the census OFF every arm is served on the very
next render; with the census ON **2 of 24 arms were never served at all** and the rest were served up to
**3 frames** late. `framesContributed` — a veto input — therefore depends on census cycle length.

⚠ **STATED PRECISELY: this is LATENT, not observed to have changed an outcome.** On both `m41` legs the
event sets were identical and `vetoed_events` was 0. ⛔ **Do not write that the veto was wrong.** What is
established is that a veto input is coupled to an unrelated subsystem's cadence, which is a defect in
the mechanism regardless of whether it has bitten.

## 2. WHY OPTION (a) IS SEMANTICALLY EXACT — the argument, so a reader can check it

The mask RT's content depends **only on which actors are tagged at render time** — not on who asked for
it. Every consumer already filters the result by its own tag set: `m26` reads its record's tag, the
census reads its batch's tags, the target mask reads the event set. **Serving all pending `RequestId`s
from one render is therefore not an approximation; it is the same answer delivered to each asker.**

**Implementation shape:** `FMaskInFlight` carries a **list** of `RequestId`s. One render, one GPU reduce,
one table. The per-tag table is small and is **copied** per consumer; the ~900 KB pixel buffer is
**moved to the single consumer that asked for pixels** and left empty for the others, so (a) adds no
large copies. Exactly **one** mask render per frame, never more.

---

## 3. GATES

### `A` — ONE RENDER PER FRAME
**PREDICTED:** `M23 PASS` count **==** the number of frames that had ≥1 pending arm, and **never more
than one pass per frame**. Against attempt 2's 106 passes for 90 frames, the count should **fall**.
**FAILURE:** two passes on one frame ⇒ (a) is not implemented as specified. STOP.

### `B` — `m26` UNDER CENSUS **OFF** (the strict control)
**PREDICTED:** vs the `m41` control at census OFF — `P-C7 v2` PASS (labels byte-identical bar `t_wall`);
`annotation.json` identical; and **`m26`'s counters IDENTICAL**: arms, `framesContributed`, every event's
`MEASURED_ZERO`/`MEASURED_NONZERO` verdict, `vetoed_events`, and the `VETOED-OBJECT` line set.
**Rationale:** with no census there was never any contention, so (a) must be a no-op here. **Any
difference is a regression.** STOP.

### `C` — `m26` UNDER CENSUS **ON** (where the fix must show)
**PREDICTED:** the **verdict SET is identical** to the `m41` census-ON leg (same events, same
`MEASURED_*`, same `vetoed_events`); **`framesContributed` per event is ≥ before** (earlier service can
add frames, never remove them); and the arm→serve lag table reads **max ≤ 1 for every arm, UNSERVED 0**,
against before's max 3 / unserved 2.
⚠ **`framesContributed` rising is the FIX WORKING, not a regression** — it is pre-declared here so it
cannot be misread later. ⛔ A verdict that *changes* (e.g. `NOT_MEASURED` → `MEASURED_ZERO`) is a
different matter: report it, do not wave it through, because that direction can reach the veto.

### `D` — THE CENSUS UNDER (a)
**PREDICTED:** `framesPolluted` **0** · `batchesLost` **0** · `tagOvertaken` unchanged or lower · cycle
length **≤** before · the `CYCLE DONE` verdict histogram equal within the band already known stable on
this bench (`zero=13 nonzero=64 belowFloor=49` on the `m41` leg).
**Why pollution must still pass:** the target mask's self-tags are drawn from `m26`'s own record tags, so
they are already inside `EventTags` (`BuildBaseTagSet`) and are in the census's allowed set by
construction. **If `framesPolluted > 0`, that argument is wrong — report it.**

### `E` — `m43` GATES (i)–(x), AS PRE-DECLARED IN `2691f8c`
Unchanged in wording. `(i)` coverage `measured + hidden_blank == captured`, `unavailable == 0`, one file
per captured frame. `(ii)` the bit-exact tie, **now against the shared table for that frame**. `(iii)`
hidden blanks. `(iv)` no leakage. `(v)` OFF inertness **including `m26`/veto identical**. `(vi)` key sets.
`(vii)` letterbox. `(viii)` output-height refusal, both directions. `(ix)` cost. `(x)` A44 / `m38` /
graceful shutdown.

### `F` — `m40` L4-SHAPE
**PREDICTED:** census OFF, mask ON, target mask OFF ⇒ `labels.jsonl` byte-identical to the `m40`-shaped
control bar `t_wall`. **The label sampler must be untouched by all of this.**

---

## 4. WHAT THIS ADDENDUM DOES NOT CLAIM

- ⛔ It does **not** claim the veto ever produced a wrong answer. §1's defect is **latent**.
- ⛔ It does **not** change any consumer's filtering rule, any artifact field, or the veto's rule.
- ⚠ It **does** change the shared mask pass, which `m26` and the census both depend on. That is why
  `B`, `C` and `D` exist and why `B` demands byte-identity rather than "close enough".
