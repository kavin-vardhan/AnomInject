# `m43` — TARGET ID MASK — PRE-DECLARED GATES AND PREDICTIONS

**Committed BEFORE any `m43` source change.** Plan: journal 069 §3 (`bcd9bb6`) §3.8 + the four rulings
in session 069 brief 6. Bootstrap `HEAD == origin/master == db2f49b`, verified.

⛔ **Never amended after a measurement exists** (`P-C2` route: annotate forward). ⛔ **Any gate FAIL:
report the measurement and STOP, no iteration.**

---

## 0. WHAT `m43` IS, AND THE ONE THING IT MUST NOT DO

Per captured frame, an **8-bit grayscale PNG** at `target_mask/frame_NNNNN.png` whose non-zero values
are the **stencil tags of the anomaly targets visible in that frame**, 0 elsewhere. Same shader, same
pass, same RT as the `m26` visible mask — **no new shader, no new render pass.**

🚨 **THE INVARIANT THAT MATTERS MOST (`R1`): `m26`'s measurement and the VETO must be untouched.**
`m43` gets its own arm budget and its own `RequestId` space; `MaxArmsPerEvent`, `Records`,
`FramesContributed` and every veto input stay exactly as they are. **Gate (v) proves it by comparison,
not by assertion.**

**How one arm serves both** — stated here because it is the load-bearing design decision: the post-tonemap
callback consumes **one** pending arm per frame, so two arms in one frame would desynchronise the mask
from the frame. Therefore, on a captured frame `m43` **reuses `m26`'s arm when `m26` armed this tick**
(adding only a pixel readback, which cannot change the counts — those come from the GPU table either
way) and **arms its own id only when `m26` did not**. One arm per frame, always.

**Numbering:** mask PNGs are numbered by **`session_index`** so they sort with `Actual_Frames`.
⛔ **Never by `frame_index`** (`G161`, and `client-readme.md` already forbids joining on it).

**Per captured frame, exactly one of three outcomes:**
| condition | outcome | counter |
|---|---|---|
| target mask effective **and** ≥1 live event target not hidden | mask armed → PNG from the readback | `target_mask_frames_measured` |
| target mask effective **and** every live event target hidden (or none live) | **explicit all-zero PNG**, no arm (`LOCK-1`) | `target_mask_frames_hidden_blank` |
| target mask not effective, or the readback never arrived | **no file**, `mask_file: null` | `target_mask_frames_unavailable` |

🔑 **blank ≠ null.** A blank PNG means *"measured, no target visible"* — real ground truth, and for a
hide-type anomaly the most informative frame in the set. `null` means *"not measured"*. This is
`m26`'s `MEASURED_ZERO` ≠ `NOT_MEASURED` distinction recurring at the frame level.

---

## 1. `(i)` COVERAGE

**PREDICTED**, healthy async leg, 90 captured frames:
`target_mask_frames_measured + target_mask_frames_hidden_blank == 90` ·
`target_mask_frames_unavailable == 0` · **90 files** in `target_mask/`, each at the **picture size**
(1280×720 on the standard bench leg) · every `labels.jsonl` frame row carries a non-null `mask_file`.

**FAILURE BRANCH:** a sum below 90, or any `unavailable`, means a readback did not arrive — report the
count and the frame indices; do not re-run.

## 2. `(ii)` THE BIT-EXACT TIE — the load-bearing gate

For **every measured frame** and **every live target** on it, the number of pixels equal to that
target's `mask_value` in the PNG **==** the reduce table's `Count` for that tag on that same frame.

🔑 **This is a tie between two INDEPENDENT reductions of the same RT** — the GPU compute reduce (the
table the veto already reads) and a CPU count over the delivered PNG. **It is what makes the shipped
mask provably the same silhouette the labels were judged on.**

**PREDICTED: equal on 100 % of (frame, target) pairs; violations 0.**
**FAILURE BRANCH:** any mismatch ⇒ the delivered mask is not the measured silhouette. **STOP.** Report
the frame, the tag, both counts. ⛔ Do not "explain" a mismatch by resampling or rounding — there is
neither in this path.

## 3. `(iii)` HIDDEN FRAMES ARE BLANK, NOT ABSENT

Bench cadence (`m40`/`m41` legs): a `blinking` event's hidden set is `{n, n+1, n+5, n+6}`-shaped —
`m41`'s own legs measured e.g. `28-29-33-34`.

**PREDICTED:** on every `blinking`-hidden and `missing_object` frame the PNG **exists and is all
zero**; on the frame after a revert the target's value is **present again**; `hidden_blank` ≥ the
number of such frames.
**FAILURE BRANCH:** a *missing file* on a hidden frame means `R2` did not take effect — that is the
`069-05` D6 defect and it is a FAIL, not a variation.

## 4. `(iv)` NO LEAKAGE

Census-ON leg. The mask RT carries census batch tags in the same frames; `m43` zeroes every value not
in that frame's **event-tag set**.

**PREDICTED:** every non-zero value in every PNG ∈ that frame's event-tag set; **violations 0**;
no value < `ReservedStencilBase` and no host-reserved value ever appears.

## 5. `(v)` OFF INERTNESS **AND** THE `R1` PROOF

- `IAI.Capture.TargetMask 0` vs the `m41` control → **`P-C7 v2` PASS** (deltas one constant, pose
  identical, everything else byte-identical), **no `target_mask/` directory**, **no new keys** in
  `labels.jsonl` or `run_summary`.
- 🚨 **THE `R1` PROOF:** on the `m43` binary, the mask/veto counters with the knob **ON** must be
  **identical** to the same leg with the knob **OFF** — `vetoed_events`, `translucent_vetoes`,
  `translucency_unknown_vetoes`, `mask_probe_arms`, `mask_residual_discards`, `mask_nopass_discards`,
  and the `VETOED-OBJECT` lines. **PREDICTED: identical.**
  **FAILURE BRANCH:** any difference ⇒ `m43` reached into `m26`'s measurement. **STOP.**

## 6. `(vi)` KEY SETS

**PREDICTED:** `labels.jsonl` delta vs the `m41` control = **exactly `{mask_file, mask_value}`**
(`mask_file` on the frame row, `mask_value` on each anomaly row) · **`annotation.json` delta = NONE,
48 keys, added 0 removed 0** · `run_summary` delta = **exactly the three counters**.

## 7. `(vii)` LETTERBOXED FIXTURE

Zero-cook route (`Set PlayerCameraManager bDefaultConstrainAspectRatio true` +
`DefaultAspectRatio 2.39`), bench device only.
**PREDICTED:** the PNG's dimensions **==** the picture dimensions (the letterboxed view rect, e.g.
`1280×536`), and gate `(ii)` still holds on that leg.

## 8. `(viii)` OUTPUT-HEIGHT REFUSAL — both directions

`R3`: the mask RT is view-rect sized while `m28` resamples the written frame, so at a non-zero output
height the two would disagree in size. ⛔ **A label mask must never be filtered** — bilinear would
invent stencil values that were never assigned to anything.

**PREDICTED, height non-zero:** the StartRun echo names the refusal · **no `target_mask/` directory** ·
`target_mask_frames_unavailable == captured frames` · every `mask_file` is **null**.
**PREDICTED, same leg at height 0:** masks written normally.
⚠ Nearest-neighbour resampling is a **named follow-up, not built**.

## 9. `(ix)` COST — reported, not graded

**PREDICTED:** a per-frame delta is measurable — `m43` re-introduces a full `W×H` `PF_R8_UINT` surface
readback (~0.9 MB at 720p) **per captured frame**, which is the cost `m34` removed, plus one grayscale
PNG encode on the worker. ⛔ **NO THRESHOLD.** Report `t_wall` per captured frame and `speed_ratio`
against the `m41` control. ⚠ At the shipped paced 30 fps the pacer may absorb it entirely — that would
be **headroom, not free**, and `speed_ratio` is the instrument that says so on a client box.

## 10. `(x)` CARRIED GATES

**PREDICTED:** `A44` both encodings — every new string present in the STAGED exe (`TargetMask`,
`target_mask`, `mask_map.json`, `bTargetMaskDefault`, `mask_file`, `mask_value`), ascii 0 / utf16
non-zero, with a positive and a negative control · `m38` run log opens and closes cleanly on every leg ·
**graceful-shutdown leg**: exit code 0, `Object subsystem successfully closed.`, zero Fatal/Assertion/
Ensure lines, **and the `target_mask/` folder is deletable afterwards** (no leaked file handle — the
mask jobs must flush like frame jobs) · container **UNCHANGED**, code-only hot-swap, **no cook**.

---

## 11. WHAT `m43` DOES NOT CLAIM

- ⛔ **It is not a full instance mask of the scene** — only **anomaly targets** appear. Everything else
  is background, by design.
- ⛔ **Translucent-only targets never appear**: they are excluded at selection (`m41` item B) and they
  cannot write custom depth, so they can never be in the mask.
- ⛔ **Nanite targets are invisible to this mask** (`G134`) — the same limit the `m26` measurement has.
- ⚠ **Multi-target frames are UNVERIFIED** (`D7`): one arm per tick and tags restored after collection
  make one value per frame the common case. Client docs say *"one value per anomaly target present in
  the frame"*. **If a bench leg shows two distinct non-zero values in one PNG, say so; do not claim it
  otherwise.**
