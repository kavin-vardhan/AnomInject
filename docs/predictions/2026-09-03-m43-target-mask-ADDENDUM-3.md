# `m43` PREDICTIONS — **ADDENDUM 3**: liveness from the label's own source; the own-arm always

**Committed BEFORE the source change it describes.** Parents: `2026-09-03-m43-target-mask.md`
(`2691f8c`) and `…-ADDENDUM-2.md` (`50463c2`) — **neither is edited**. Authorised by session 069
brief 9, rulings 1–3.

---

## 1. DEFECT 1 — MEASURED FIRST, AS RULED. It is a peek/take RACE, and it is one cause for all three.

The three unavailable frames on attempt 3 were `session_index` **29, 43, 89** (`frame_index` 40, 58,
120). All three have `anomalies = 1` in their label row, and the log shows **`M23 ARM` on each of their
ticks** — so `m26` armed, the target mask took the **shared** path, and the render happened.

**The cause is the shared path itself.** Each tick the mask block runs
`EnqueueDrain()` → `ServiceTargetMask()` (peek, `bRemove = false`) → `CollectResults()` (take, removes).
`EnqueueDrain` only *enqueues* a render command; the result is published by the render thread at an
arbitrary moment. **If a result lands AFTER `ServiceTargetMask` has peeked but BEFORE `CollectResults`
takes it in the same tick, the target mask never sees it and the entry stays pending forever** — then
it is counted `unavailable` at run end.

⇒ **Not end-of-run residue, not a lost readback, not a writer drop. A race, sporadic by nature, which
is exactly why it hit 3 scattered frames** (29 and 43 mid-run, 89 at the end).

**The fix removes the race rather than timing around it:** ⚠ **the target mask stops sharing `m26`'s
`RequestId` and ALWAYS arms its own.** Since ADDENDUM 2's one-render-serves-all is already in, a second
id on the same frame costs **no extra render, no extra readback** — the pixels are simply delivered to
the id that asked for them. The peek disappears, `m26`'s result is untouched, and the two consumers
stop sharing a slot they were never designed to share.

## 2. DEFECT 2 — liveness comes from the label row's own source

`m26`'s records **outlive their fire window** (it keeps arming to fill its 4-arm budget), so
"`m26` armed" is not "a target is live". The target mask now takes both decisions from
**`Auto->GetLiveFires()`** — **the same call the label row is built from** in `FinalizeArmedLabel`.

🔑 **Why that is identical by construction and does not rely on delegate order:** both reads happen in
the same world tick at `OnWorldTickEnd`, and `LiveFires` is mutated only in the injector's own `Tick`,
which has already run. ⛔ **No dependence on the `OnWorldTickEnd` multicast order** — that is the
assumption `P9` was made of, and it is not repeated here.

- **measure** iff ≥1 live fire has a valid, **not hidden** target actor;
- **blank** iff there is no live fire, or every live fire's target is hidden (`R2`), or the live fires
  are session-globals with no actor (a global has no silhouette);
- the **write filter** is **this frame's live-fire tag set**, not `BuildBaseTagSet()` (every record tag
  ever). An ended event's lingering tag is therefore zeroed.

---

## 3. GATES ADDED OR TIGHTENED

### `(iv)` — TIGHTENED
**PREDICTED:** every non-zero value in every PNG ∈ **that frame's LIVE-FIRE tag set** (was: the event
set). Violations **0**.

### `(iv-b)` — NEW, and it is the one attempt 3 would have failed
**PREDICTED:** every captured frame whose label row has **`anomalies = 0`** has an **all-zero** mask
(or no file). **Violations 0.**
⚠ Attempt 3 had **2** (`session_index` 23 and 47, both immediately after a fire ended). **A mask that
contradicts its own labels is worse than a missing mask** — this gate exists so that can never ship.

### `(iii)` — PROVEN BY IDENTITY, per ruling 3
**PREDICTED:** after ruling 1 there is **no route to blank other than "no live fire" or "all live
targets hidden"**, so a blank on a fire-active frame must correspond to the game-side hidden state.
Shown by the count identity **`measured + hidden_blank + unavailable == captured`** together with
`(iv-b)`, plus the blinking cadence check.

### `(i)` — the coverage prediction, restated with ruling 2's escape
**PREDICTED:** `measured + hidden_blank == captured` and **`unavailable == 0`**. With the race removed
there is no known route to a pending entry on a healthy async leg. ⚠ **If a residue remains it must be
end-of-run only** (`session_index` == the last captured index), which ruling 2 pre-authorises; **a
mid-run unavailable is a FAIL and must be reported with its index.**

---

## 4. WHAT IS UNCHANGED AND MUST STAY SO

`m26` no longer requests pixels at all, and nothing else about its path moves — gates **B** (census OFF,
byte-identical counters) and **C** (census ON, verdict set identical, `framesContributed` ≥ before, lag
≤ 1) stand exactly as written in ADDENDUM 2. The census's filtering, the veto's rule, `annotation.json`
and the label sampler are all untouched; gates **D** and **F** stand.
