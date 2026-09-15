# 2026-09-15 — verifier field validity — FIX PROPOSAL (design only; nothing built)

**Status: PROPOSAL. NO CODE WAS WRITTEN.** `tools/verify_capture.py` and
`tools/measure_label_offset.py` are byte-unchanged as of this file. Every prediction below is
written **before** the fix exists.

**Diagnosis this rests on:** `docs/sessions/2026-09-15-079-01-verifier-field-validity.md`.
**One-line statement of the defect:**

> `label_pixel_gate` learns `tau = median + 6·MAD` over a statistic `d` that is **bounded in [0,1]**,
> while `tau` itself is **unbounded**. Under camera motion the clean-frame `d` rises toward 1 and its
> MAD grows, so `tau` climbs past what `d` can ever reach — measured at **1.0950** and **1.1938** on a
> real session — making `d > tau` unsatisfiable and NOT-VISIBLE unconditional. The constants
> `K_SIGMA`/`SIGNAL_FLOOR` were calibrated in `measure_label_offset.py` for `net` — a
> **ring-subtracted, motion-compensated** quantity — and are applied here to a raw, uncompensated one.

---

## 1. Acceptance rules (chat's, fixed — restated so the fix is judged against them)

- **(a)** The gate must **NEVER** return NOT-VISIBLE for a region it cannot judge. Camera motion or
  baseline contamination ⇒ `NOT-MEASURABLE(<reason with numbers>)`, never a FAIL.
- **(b)** It must actually judge moving-camera sessions to a usable degree, or it is not a client tool.
- **(c)** Dense schedules: per-event baseline from the nearest clean frames of THAT region, explicit
  floor; below the floor ⇒ NOT-MEASURABLE with the count printed.
- **(d)** Selftest gains the complementary half (moving-camera cases).

---

## 2. THE PRIMARY FIX — ring-normalised change (`net`), with an unsatisfiable-threshold assertion

**Chosen primary, with the reasoning from §4 of the journal: subtract an ambient ring, exactly as
`measure_label_offset.py` already does for its own statistic.** This is **restoration of existing
prior art in this repo**, not a new invention — `mlo:92-93` states in its own words that ring
subtraction is what stops camera motion faking a manifestation, and `mlo:126` already uses ambient to
declare UNMEASURABLE rather than to fail.

### 2.1 The metric

```
raw(k) = frac of REGION pixels differing > thresh from frame k-1          (today's d, unchanged)
amb(k) = frac of RING   pixels differing > thresh from frame k-1
         RING = region dilated by RING_DILATE_PX (48), minus the region   (mlo:483-491)
net(k) = raw(k) - amb(k)
```

`tau` is then learned **on `net`**, with the same robust form and the same constants:
`tau = max(median(net_base) + K_SIGMA·MAD(net_base), SIGNAL_FLOOR)`, and the edge test becomes
`net(k) > tau`.

**Why this removes the defect rather than retuning around it.** Camera motion is a *global* change:
it lifts `raw` and `amb` together, so `net ≈ 0` with small spread ⇒ `tau` falls back to
`SIGNAL_FLOOR` and **the headroom is restored**. An anomaly is a *local* change: it lifts `raw` and
not `amb` ⇒ `net` spikes. The quantity being thresholded becomes **local excess over global motion**,
which is what "the anomaly is visible here" actually means.

⛔ **No constant is retuned.** `K_SIGMA = 6.0` and `SIGNAL_FLOOR = 0.0040` keep their values — they
are finally applied to the quantity they were calibrated for. **A fix that instead lowered `K_SIGMA`
would be tuning against the symptom and would still saturate at enough motion.**

### 2.2 The three invalidity escapes (acceptance rule (a)) — all NOT-MEASURABLE, never NOT-VISIBLE

| # | condition | verdict |
|---|---|---|
| **E1** | **No usable ring**: region ≥ `RING_MAX_REGION_FRAC` (0.60) of the frame, or ring area < `RING_MIN_PX` (2000) | `NOT-MEASURABLE(no-ring: region NNNpx = P% of frame — motion cannot be separated)` |
| **E2** | **Motion dominates**: `median(amb_base) > AMBIENT_FACTOR · tau` | `NOT-MEASURABLE(camera-motion: ambient=X.XXXX vs tau=Y.YYYY)` |
| **E3** | **Threshold unsatisfiable**: `tau ≥ max attainable value of the statistic` | `NOT-MEASURABLE(threshold unsatisfiable: tau=X.XXXX)` |

🔑 **E3 IS THE LOAD-BEARING ONE AND IT MUST SHIP EVEN IF THE RING WORKS PERFECTLY.** It is a
**structural assertion that the observed failure class cannot recur silently**, independent of
whether the ring logic is right. A threshold no measurement could ever clear is a **broken
instrument**, and an instrument that knows it is broken must say so rather than emit a verdict. Today
that state produced six confident FAILs. ⛔ **Do not implement E3 as a clamp** (`tau = min(tau, 1.0)`)
— clamping converts an impossible test into a merely absurd one that still FAILs, and hides it.

**E1 directly catches H3:** `M50L_LG9`'s `labels_bbox_px/2073600px` region **is the whole 1920×1080
frame**; there is no outside, so no ring, so nothing is separable — that event is *unjudgeable* and
must say so.

### 2.3 Rejected alternatives, with reasons

- **Second-order (change-of-change) onset detection** — rejected as primary. It is scale-free, which
  is attractive, but it detects *any* acceleration in the change signal, and a moving camera supplies
  plenty (turn onset, occluder crossings). No prior art here; the ring has both prior art and a
  stated mechanism. **Keep as a fallback only if the ring proves insufficient at high motion.**
- **View-delta compensation as PRIMARY** (`view.rot`/`view.origin` from `labels.jsonl`) — rejected as
  primary. ⚠ Two measured reasons: the field is **schema v1** and the presence of those fields there
  is **unverified**; and on `A1L_LEGA` Lyra's camera **translated 1214 cm with rotation constant**
  (journal 072), so a rotation-keyed compensator would have read "no motion" on a session measuring
  `M_med = 0.0884`. **Ship it as a printed diagnostic, never as the gate's dependency.**
- **Lowering `K_SIGMA` / raising `SIGNAL_FLOOR`** — rejected; see §2.1.
- **Whole-frame normalisation instead of a ring** — weaker than the ring (a large target is part of
  the frame, so it partially cancels itself) and it has no prior art here. The ring is local.

---

## 3. Dense schedules (acceptance rule (c))

Today `base_idx` is computed **once, globally** (`verify_capture.py:703-714`) and shared by every
event; the guard relaxes 2 → 1 → 0 for the whole session at once. Change to **per-event**: take the
nearest clean frames **to that event's own window**, evaluated **on that event's own region**, up to
a cap, with floor `BASELINE_MIN = 3`. Below the floor ⇒ NOT-MEASURABLE **with the count printed**
(the existing `:782` message already does this — keep it, and add the guard actually used and the
distance of the frames chosen).

**Also report, never gate:** the measurable ceiling `±(min_clean_gap // 2)` is already printed; add
the per-event gap so a reader can see *which* event is starved rather than only the session minimum.

---

## 4. Selftest — the complementary half (acceptance rule (d))

`_synth_session` gains a **`pan=` parameter** that translates the background a fixed number of pixels
per frame (and scrolls a textured backdrop, so the motion produces real per-pixel change rather than
flat-on-flat). New cases, **all with `pan` non-zero**:

| case | expect |
|---|---|
| `moving_clean` | **PASS** |
| `moving_label_late_1` (shift +1) | **ONSET-SHIFT(-1)** — right sign under motion |
| `moving_label_early_1` (shift -1) | **ONSET-SHIFT(+1)** |
| `moving_end_late_1` / `moving_end_early_1` | **END-SHIFT(-1)** / **END-SHIFT(+1)** |
| `moving_blank_region` (motion, **no anomaly** in the region) | **NOT-MEASURABLE or PASS — NEVER NOT-VISIBLE** |
| `moving_region_is_frame` (region = whole frame) | **NOT-MEASURABLE(no-ring)** — E1 |
| `moving_extreme` (pan large enough that today's `tau` > 1.0) | **NOT-MEASURABLE**, never NOT-VISIBLE — E2/E3 |

⚠ **The existing static `blank_region` → NOT-VISIBLE case is KEPT.** NOT-VISIBLE must remain
reachable on a *pinned* camera, or the fix has simply disabled the verdict instead of scoping it.
**A gate that can no longer fail is not a gate** — the selftest must still prove it fails where
failing is correct.

---

## 5. PREDECLARED VALIDITY EVIDENCE — the four readings the fix brief must produce

Written now, before the fix exists. Each is pass/fail on a fixed rule.

**V1 — SELFTEST.** `--label-pixel-gate --selftest` passes **all** cases: the seven existing static
ones **unchanged**, plus every §4 moving case. Any moving case reading NOT-VISIBLE ⇒ **FAIL**.

**V2 — THE REPRODUCTION FLIPS, WITH NO LABEL CHANGES.** `M50L_LG9` goes from **NOT-VISIBLE 6/6,
VERDICT FAIL** to **PASS on the events whose labels are known-good**, with:
- ⛔ **zero edits to `annotation.json`, `labels.jsonl` or any frame** — the session is read-only;
- events that are genuinely unjudgeable (E1 — the full-frame-bbox ones) reading **NOT-MEASURABLE with
  the region size printed**, which is a correct reading, not a dodge;
- **NOT-VISIBLE count = 0.**
- `A1L_LEGA`, `LYRA_SMOKE_01`, `A2L_LEGA` stay **VERDICT PASS**, with their NOT-MEASURABLE events
  either unchanged or improved.

**V3 — THE HISTORICAL PASSES ARE PRESERVED, AND THE FIX IS INERT ON A PINNED CAMERA.** All six
`M49_GEDGE_*` legs still read **PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / NOT-MEASURABLE 0**.
🔑 **Stronger form, and it is the one to hold to: on a pinned camera `amb ≈ 0`, so `net ≈ raw` and
`tau` must still print `0.0040`.** Verdicts **byte-identical** to §3.2's table. Any movement in a
bench verdict is a **FAIL of the fix**, not a new reading.

**V4 — THE OFFICE RE-RUN.** The owner re-runs `--label-pixel-gate --report-only` on the M2 Concorde
captures and transcribes the summary lines only (no office data leaves the box). Expected: **mostly
PASS; remaining events NOT-MEASURABLE with their reasons and numbers printed; NOT-VISIBLE on
eye-verified-correct labels = 0.** ⚠ **V4 is the only one of the four that tests the actual field
data; V1–V3 are home evidence and cannot substitute for it.**

### 5.1 Failure branches, predeclared

- **If `M50L_LG9` flips to PASS but a bench leg moves** ⇒ the fix is not inert on a pinned camera;
  **stop**, do not accept.
- **If `M50L_LG9` reads NOT-MEASURABLE on all six** ⇒ acceptance rule (a) is met but **(b) is not** —
  the tool is honest and useless. Report it as such; the ring is then insufficient and the
  second-order candidate (§2.3) is next. ⛔ **Do not relax E1/E2 to manufacture a PASS.**
- **If the field re-run (V4) still reads mass NOT-VISIBLE** ⇒ the home reproduction was not the field
  mechanism after all; **stop and report** — the ranking in the journal would then be wrong and chat
  decides the next step.

---

## 6. Instrumentation (§3.3 of the brief) — still wanted, scoped down

`--debug-events` prints, per event: `raw`, `amb`, `net` and the **per-frame series** across the
window ±edge; `tau`, `median`, `MAD`, the baseline frames used **and why**; the region source and
size; and a **camera-motion estimate per frame** — both the whole-frame changed-pixel fraction
(`M`, the proxy used in the journal) **and** the `view.rot`/`view.origin` deltas from `labels.jsonl`
when present.

⚠ **Scoped down because the banked reproduction already ranked the hypotheses without it** — `tau`,
`base`, the per-edge `d` and the region source/size are already printed by the existing non-`--quiet`
output. It is needed for **building and defending the fix**, not for the diagnosis.

---

## 7. Open items for chat

1. 🚨 **`docs/OPERATING-CONTRACT.md` does not exist on `master`** — it is m51-resident and m51 is
   HELD. The §0 orchestration entry is **owed** and is recorded meanwhile in the 079-01 journal §0
   and `_mailbox/README.md`. **Where should the contract entry land?**
2. **Is a home moving-camera StackOBot capture still wanted?** **Recommendation: no.** The banked set
   already spans `M_med` 0.0005 → 0.2649 on two hosts and contains a full reproduction under
   field-matching conditions (bbox-only, real game). Requiring one would force inventing a
   camera-motion lever, which **reopens the §3.2-vs-§4 `Source/` contradiction** this diagnosis
   avoided.
3. ✅ **`G156`'s correction is APPENDED (done this session, append-only).** Its "delivery mode does
   not write `labels.jsonl`" mechanism was superseded by session 052 the day after it was written;
   its measured table and its "cannot self-verify" conclusion stand. Journal §6.1.
4. **Scope check:** `measure_label_offset.py` is used by Section G and by
   `--label-pixel-gate` alike. The fix as proposed touches **only `verify_capture.py`** and needs
   **no change to `measure_label_offset.py`** (it only *calls* `ring_mean`, already public). Confirm
   that is the intended blast radius before implementation.
