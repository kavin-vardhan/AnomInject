# 2026-09-15 — verifier field validity — FIX PROPOSAL (design only; nothing built)

> 2026-09-20 dated pointer: 079-09 withdraws label-truth verdicts. Current predictions
> are in [2026-09-20-verifier-consistency-semantics.md](2026-09-20-verifier-consistency-semantics.md).
> This file is preserved as the historical predeclaration and amendments.

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

---

# AMENDMENT 1 — 2026-09-15 (079-02), written BEFORE a line of the fix was implemented

Chat ruled §7's open items: the contract entry lands on `m51` as a docs-only commit; **no home
capture and no camera lever** — the banked sessions are the field surrogate; blast radius is
**`verify_capture.py` only**, `measure_label_offset.py` byte-unchanged; the §2 diagnosis is
**ACCEPTED** as the mechanism.

This amendment fixes the readings **P1–P6** in advance. ⛔ **If P2 or P3 fails, STOP and report
NEEDS-DECISION with the numbers. Do NOT tune constants to pass.** Tuning the *selftest fixture* so
that it poses a fair question is legitimate and will be stated; tuning `K_SIGMA`, `SIGNAL_FLOOR` or
any gate threshold to move a verdict is the laundering shape and is forbidden.

## Constants chosen now, so they are not chosen after seeing a result

| constant | value | why |
|---|---|---|
| `K_SIGMA`, `SIGNAL_FLOOR` | **6.0, 0.0040 — UNCHANGED** | they are finally applied to the statistic they were calibrated for (`net`). Changing them would be tuning against the symptom. |
| `MIN_RING_PX` | **2000** | inherited from `mlo.RING_MIN_PX`; one source of truth, and `ring_mean` already enforces it |
| `RING_MAX_REGION_FRAC` (gate) | **0.90** | per the brief. Deliberately looser than `mlo`'s 0.60 — the gate prefers to *attempt* a read and let F-B catch it, rather than refuse early |
| `MIN_BASELINE_FRAMES` | **3** | unchanged from today's `len(base_vals) < 3` |
| `BASELINE_MAX_FRAMES` | **24** | new cap; makes "nearest" meaningful and bounds cost on long field sessions |

## The predeclared readings

**P1 — SELFTEST.** `--label-pixel-gate --selftest` prints `SELFTEST: OK`. The **7 existing static
cases read exactly as today** (`PASS`, `PASS`, `ONSET-SHIFT(-1)`, `ONSET-SHIFT(+1)`,
`END-SHIFT(-1)`, `END-SHIFT(+1)`, `NOT-VISIBLE`). New moving cases read:

| case | expected |
|---|---|
| `moving_clean` (pan 2) | `PASS` |
| `moving_fast` (pan 8) | `PASS` |
| `moving_label_late_1` | `ONSET-SHIFT(-1)` |
| `moving_label_early_1` | `ONSET-SHIFT(+1)` |
| `moving_end_late_1` / `moving_end_early_1` | `END-SHIFT(-1)` / `END-SHIFT(+1)` |
| `moving_blank_region` | **`NOT-VISIBLE`** — the gate must STILL be able to fail |
| `moving_fullframe_region` | `NOT-MEASURABLE(no ambient ring…)` — F-C |
| `moving_unsat` (stripes, pan = stripe width ⇒ every pixel flips, `d_ring = 1.0`) | `NOT-MEASURABLE(threshold unsatisfiable…)` — F-B |

🔑 **`moving_blank_region` is the load-bearing selftest case.** If the ring fix made NOT-VISIBLE
unreachable it would have *disabled* the verdict rather than *scoped* it — a gate that cannot fail is
not a gate (`G96`).

**P2 — THE FIX IS INERT ON A PINNED CAMERA.** All six `M49_GEDGE_*` legs
(`{BL,CT,MO,MT}_NAT`, `{BL,MT}_SYN`) read **`tau = 0.0040`** on every event and
**`PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / NOT-MEASURABLE 0`**, `VERDICT PASS`, exit 0.
⚠ **ONE DIFFERENCE IS DECLARED IN ADVANCE AND IS NOT A REGRESSION: the printed `base=` count will
drop from `37` to at most `24`** — that is `BASELINE_MAX_FRAMES` (F-D), chosen above, not a change in
what the gate concluded. On a pinned camera `d_ring ≈ 0`, so `net ≈ raw` and `tau` must still land on
the floor. **Any movement in a bench VERDICT is a FAIL of the fix.**

**P3 — THE REPRODUCTION FLIPS.** `M50L_LG9`: **zero NOT-VISIBLE, zero SHIFT**; every event either
`PASS` or `NOT-MEASURABLE` with a printed, numeric reason. (079-01 read NOT-VISIBLE 6/6, VERDICT
FAIL, `tau` up to 1.1938.)
⚠ **P3 is judged against an EYE-CHECK, produced BEFORE the gate is re-run**: for each of its 6
events, overlay `start−1`, `start`, `start+1` with the bbox drawn, look at them, and record whether
the anomaly is visibly present inside the box at `start` and absent at `start−1`.
🚨 **An event whose eye-check CONTRADICTS its label is a FINDING ABOUT `m50`'s LABELS and is reported
as such, loudly — not hidden, and not used to excuse a gate reading.**

**P4 — THE OTHER MOVING-CAMERA LEGS DO NOT REGRESS.** `A1L_LEGA`, `LYRA_SMOKE_01`, `A2L_LEGA`:
PASS count **≥** 079-01's (4, 5, 6 respectively), **no new NOT-VISIBLE**, `VERDICT PASS` on each.
Any verdict that moves is listed with before/after **and the numbers behind it**.

**P5 — BATCH MODE AGREES WITH THE INDIVIDUAL RUNS.** `--all` over the bank folder holding those
sessions prints one line per session whose counts are **identical** to the per-session runs above.
A session lacking `labels.jsonl` prints `CANNOT RUN`, never a crash and never a pass.

**P6 — OFFICE (owner errand, only after this brief is GREEN).** Re-run the new tool on the same M2
Concorde captures. Predeclared: **mostly PASS; remaining events NOT-MEASURABLE with printed reasons;
ZERO NOT-VISIBLE on the events the owner verified by eye.** ⚠ **P6 is the only reading that touches
the actual field data — P1–P5 are home evidence and cannot substitute for it.**

## Failure branches, predeclared

- **P2 moves** ⇒ the fix is not inert on a pinned camera. **STOP**, NEEDS-DECISION.
- **P3 still shows NOT-VISIBLE** ⇒ ring normalisation is insufficient. **STOP**, NEEDS-DECISION;
  the second-order candidate (§2.3) is next. ⛔ Do not relax F-B/F-C to manufacture a PASS.
- **P3 reads NOT-MEASURABLE on all six** ⇒ rule (a) met, rule **(b) NOT met** — honest and useless.
  Report as such; do not call it GREEN.
- **The eye-check contradicts a label** ⇒ report it as an `m50` labelling finding; it does not
  change the gate's verdict either way.

## RESULT (appended 2026-09-15 after the run — see `docs/sessions/2026-09-15-079-02-verifier-ring-normalised-gate.md`)

**P1 PASS (16/16) · P5 PASS · P2 FAIL · P3 FAIL · P4 FAIL ⇒ NEEDS-DECISION, the predeclared stop.**
No constant was tuned. `tau` still read `0.0040` on every pinned bench leg and `base=` dropped
37 → 24 exactly as declared, so **the failures are not threshold drift** — they refute **F-A (ring
SUBTRACTION)** specifically:

- **P2:** `MT_NAT`/`MT_SYN` PASS → NOT-VISIBLE. At the onset `d_region = 0.02286` but
  `d_ring = 0.02872` ⇒ `net = -0.00586`. The anomaly's GI bounce lands in a ring that is **less than
  half the silhouette's area** (28,272 vs 66,837 px, clamped by a corner-hugging bbox), so the ring's
  *fraction* exceeds the region's. **`m26`'s `A35` already measured that these anomalies change
  pixels outside their own bbox**, and journal 071 named this exact leg as the thinnest margin in the
  set and "the one to watch".
- **P3/P4:** ring subtraction does **not** cancel camera motion — it is parallax/content-weighted, so
  `tau` on `net` still reads 0.18–0.44 on `M50L_LG9`.
- 🔻 **P3's PREMISE IS WITHDRAWN:** all six `M50L_LG9` events carry `observability_measured: false`
  and `bbox_source: "projected"` — **the producer declares it has no pixel evidence**. 079-01's
  "known-good labels" was an over-claim, corrected in the 079-02 journal §4.

**F-B, F-C, F-D, F-E, F-F, F-G, F-H are not implicated and should survive. Only F-A is refuted.**

---

# AMENDMENT 2 — 2026-09-15 (079-03), written BEFORE the revert and BEFORE any run

Chat ruled: **F-A withdrawn**, statistic returns to raw `d(region)`; motion estimate becomes **global**,
not ring-based; a **motion escape** replaces ring subtraction; F-B..F-H stand; the
`observability_measured` / `bbox_source` pair is an **annotation, not a verdict rule**.

## A2.0 🚨 BLOCKING FINDING — §1's MASK GROUND TRUTH IS CIRCULAR, AND I AM NOT RUNNING A TEST THAT CANNOT FAIL

The brief asks for **MASK ONSET = the first frame in the event's span with `observable == true`**, used
as ground truth the gate never sees. **Measured before predeclaring anything, it cannot disagree with
the label by construction:**

- **`m49` A1 DEFINES `affected_frames` AS THE OBSERVABLE SUBSET.** So "the first frame of
  `affected_frames` where `observable == true`" is **`min(affected_frames)`** — the label's own onset.
  Re-deriving the label from the label.
- **The data agrees.** In `A1L_LEGA` and `A2L_LEGA` **every in-span anomaly entry reads
  `observable: True`**. The `False` entries exist (10 in `A1L_LEGA`) but sit **outside** the span, in
  `injected \ affected`: event 0 has `injected [4,5,9,10]` against `affected [4,5]`.
- **And no mask evidence exists outside the span at all** — `m44`'s rule is that masks appear only on
  frames labelled for that event, so the mask can never place an edge the label did not already claim.

**Two of the four nominated sessions do not even carry the keys:** `LYRA_SMOKE_01` and the
`M49_GEDGE_*` legs have **no `target_pixels` / `observable`** on their anomaly entries (they pre-date
`m49` A1). Per §1's own instruction — *"say so and drop it from the set — do not substitute"* —
**`LYRA_SMOKE_01` is DROPPED.** That leaves `A1L_LEGA` and `A2L_LEGA`, and for those the test is
vacuous.

⇒ **P3 as written would report 100 % agreement while testing nothing** (`G146`'s vacuous-pass shape),
and **P4's cap would inherit that vacuity.** I am therefore not running it.

## A2.1 THE REPLACEMENT — an INJECTED known answer, which cannot be circular

🚨 **DEVIATION FROM THE BRIEF, DECLARED IN ADVANCE AND IN WRITING. Chat ratifies or rejects.**
Instead of reading a known answer out of the producer's own labels, **we inject one ourselves** using
the machinery already in the tool (`_shifted_copy_of`: copies a real session's LABELS ONLY, shifts
every window by `delta`, references the frames in place, never writes to the source).

> **A gate is validated by whether it can RECOVER A KNOWN LABELLING ERROR. The envelope question is
> then exactly: at how much camera motion does that ability disappear?**

This is strictly **stronger** than the specified test — the answer is ours, not the producer's, so it
can genuinely fail — and it runs on **real moving-camera content with real parallax**.

**P3′ — recovery of an injected ±1 shift.** For `A2L_LEGA` (light motion) and `A1L_LEGA` (strong):
run the gate on `delta = 0`, `+1`, `−1`. Required: at `delta 0` **no SHIFT** on decided events; at
`delta +1` the decided events read **`ONSET-SHIFT(-1)`**; at `delta −1`, **`ONSET-SHIFT(+1)`**.
Tabulate per event: `M_med | delta | gate verdict | recovered? `. Events read NOT-MEASURABLE are
listed with their reason and **excluded from the recovery rate, never counted as agreement.**

**P4′ — setting `MOTION_CAP`, procedure fixed now.**
1. **Synthetic ladder** (fully known answer, no parallax ⇒ **optimistic by construction**):
   `_synth_session(shift=+1, pan=p, blocks=16)` for `p ∈ {0,1,2,4,6,8,12,16}`. Record `M_med` and
   whether `ONSET-SHIFT(-1)` is recovered. `M_synth` := the largest `M_med` still recovered.
2. **Real** (with parallax): the P3′ results. `M_real` := the largest session `M_med` with **100 %**
   recovery on decided events.
3. `MOTION_CAP := min(M_synth, M_real)`, **rounded DOWN to two significant figures**.
4. **If any session/rung FAILS recovery at `M_fail`, additionally require `MOTION_CAP ≤ M_fail / 2`**
   (the brief's 2× margin).
5. If nothing fails anywhere, the cap is set **just above the highest `M_med` observed**, and is
   flagged **`provisional — no disagreeing session observed`**.
6. The cap and this provenance go in the tool header. ⛔ **No other constant is tuned.**

## A2.2 The other readings, unchanged from the brief

- **P1** selftest: all existing cases **re-based on raw `d`**, plus one **heavy-motion** case whose
  known answer is `NOT-MEASURABLE(camera motion)` ⇒ `SELFTEST: OK`.
- **P2** 🔑 **THE INERTNESS PROOF F-A FAILED.** All six `M49_GEDGE_*`: `tau = 0.0040`, **`PASS 4 /
  SHIFT 0 / NOT-VISIBLE 0 / NOT-MEASURABLE 0`**, verdict lines identical to **079-01's** run apart
  from the new annotation columns and `base=24`. **`MT_NAT` and `MT_SYN` must be PASS 4** — that is
  **P7** as well.
- **P5** `M50L_LG9` (`M_med` measured **0.2649** globally in 079-01; the gate's own header read
  **0.3491** over its clean subset — both are reported): **every event `NOT-MEASURABLE(camera
  motion)`, zero NOT-VISIBLE, zero SHIFT**, *provided the cap lands below that value*. If the cap
  lands above it, the escape does not fire and **I say so rather than forcing it.**
- **P6** batch: counts identical to the individual runs.
- **P7** the 079-02 regression is gone (subsumed by P2).

## A2.3 My prediction for `A1L_LEGA`, stated before the run

079-01 (raw `d`, base=30) read **PASS 4 / NOT-MEASURABLE 2** — `idx=0` NOT-MEASURABLE with
`CONTAMINATED=7`, `idx=5` `manifested-false-or-empty`. 079-02 (ring) turned `idx=0` into
`ONSET-SHIFT(+1)`. **With F-A reverted I predict `A1L_LEGA` returns to PASS 4 / NOT-MEASURABLE 2**,
i.e. the 079-02 SHIFT was a **gate error introduced by ring subtraction**, not a label defect. ⚠ F-D's
nearest-24 baseline (vs 30) may still move `idx=0`; if a SHIFT survives the revert, **P3′'s
`delta = 0` run is what decides whether it is real**, and it is reported either way.

## A2.4 Constants

`K_SIGMA = 6.0`, `SIGNAL_FLOOR = 0.0040`, `MIN_BASELINE_FRAMES = 3`, `BASELINE_MAX_FRAMES = 24`,
`RING_MAX_REGION_FRAC = 0.90` — **all unchanged**. `M_med` is computed at **full resolution, no
downsampling** (stated per §0.2; the cache makes it cheap). `MOTION_CAP` is the **only** new constant
and it is set solely by A2.1's procedure.

## A2.5 Stop rules

**If P2 or P7 fails, STOP — NEEDS-DECISION with the numbers.** If P3′ shows the gate cannot recover an
injected shift even on `A2L_LEGA` (light motion), **STOP** — the statistic is not fit for moving
content at all and no cap can rescue it.

---

# AMENDMENT 3 — 2026-09-15 (079-04), chat's rulings and the predeclared re-readings

Written before the constant is changed and before any re-run.

## A3.1 `MOTION_CAP = 0.040` — ruled, not re-derived

P4′ produced **two** constraints and 079-03 took their minimum:

| constraint | value |
|---|---|
| largest `M_med` with FULL recovery of an injected ±1 | **0.0049** (`A2L_LEGA`) |
| ≥ 2× margin below the first INCOMPLETE session | **0.040** (`LYRA_SMOKE_01` 0.0801 ÷ 2) |

**Chat rules the cap sits at the MARGIN BOUND, 0.040.** The reason is the fragility 079-03 reported:
a cap resting on the single value it was derived from means a **0.0001 measurement drift refuses the
very session that set it**. The margin bound is the constraint that carries a safety factor, and it
still refuses both incomplete sessions (0.0801, 0.0852) and `M50L_LG9` (0.3491).

The header provenance line must read **exactly**:

```
cap=0.040 (2x margin below first incomplete recovery, LYRA_SMOKE_01 M_med=0.0801;
only full recovery observed at 0.0049, A2L_LEGA; ruling 079-04)
```

## A3.2 NO TWO-TIER POLICY — and the reason is the client's own complaint

079-03 measured **zero false shifts up to `M_med` 0.085** and proposed that `PASS` might be trusted
higher than `SHIFT`. **Chat rejects the middle tier**, and the argument is the decisive one:

> 🔑 **A `PASS` from a gate that cannot read back a one-frame shift is not evidence of alignment — it
> is the client's `F1` complaint restated.** If the instrument provably cannot see a one-frame error
> at that motion level, then "no error found" carries no information about whether one is there.

⇒ Above the cap the answer stays `NOT-MEASURABLE(camera motion: M_med=… cap=0.040)`. The measurement
is recorded as a **fact** in the journal and as the **stated reason there is no middle tier** in the
tool header.

## A3.3 The synthetic ladder stays NON-BINDING

It recovers to `M_med` 0.75 because a uniform scroll has **no parallax**. It stays in the selftest as
the *"the gate can still read a shift under motion"* proof, and the header must say that **the real
cap is set by real sessions only.**

## A3.4 The known-answer method is now settled for this gate

079-03's §1 finding is accepted: mask-derived onsets are **circular** (`m49` A1 defines
`affected_frames` as the observable subset). **`CaptureBench/tools/m079_shift_recovery.py` — move real
labels by a known ±1 and require the gate to report it back — is the known-answer method for this gate
from here on.**

## A3.5 Predeclared re-readings

- **P1** selftest **17/17 OK**.
- **P2** the six `M49_GEDGE_*` legs: **verdict lines byte-identical to 079-03** — `PASS 4 / SHIFT 0 /
  NOT-VISIBLE 0 / NOT-MEASURABLE 0`, `tau=0.0040`, `M_med=0.0005`. Unaffected: 0.0005 ≪ 0.040.
- **P3** `A2L_LEGA` (0.0049) shift recovery: **full 6/6 at δ = 0, +1, −1**, as in 079-03.
- **P5** `M50L_LG9` (0.3491): unchanged — **six refusals, zero NOT-VISIBLE, zero SHIFT**; the three
  `camera motion` lines now print `cap=0.040`.
- **P8** `LYRA_SMOKE_01` (0.0801) and `A1L_LEGA` (0.0852): **zero PASS, zero SHIFT, zero
  NOT-VISIBLE — every event NOT-MEASURABLE.**
  ⚠ **ONE DEPARTURE FROM THE BRIEF'S WORDING, DECLARED IN ADVANCE:** the brief says *every* event
  reads `(camera motion …)`. It will not, and that is correct behaviour, not a miss —
  **`A1L_LEGA` `idx=5` (`lod_popping`) is `manifested: false`**, and the manifested check sits at the
  top of the event loop, before the region, baseline, threshold and motion checks. It will read
  `NOT-MEASURABLE(manifested-false-or-empty)`. Expected split: **`A1L_LEGA` 5 × camera motion + 1 ×
  manifested-false-or-empty; `LYRA_SMOKE_01` 6 × camera motion.** The load-bearing predicate is
  **zero PASS / zero SHIFT / zero NOT-VISIBLE**, and that is what P8 is judged on.
- **P6** batch counts **identical** to the individual runs.

⛔ **Nothing else in the tool changes.** `K_SIGMA`, `SIGNAL_FLOOR`, `MIN_BASELINE_FRAMES`,
`BASELINE_MAX_FRAMES`, `MAX_REGION_FRAC`, `ATTAINABLE_MAX` all unchanged; `measure_label_offset.py`
byte-unchanged; no `Source/` or `Shaders/` change; no capture, build or cook.

---

# AMENDMENT 4 — 2026-09-15 (079-06), written BEFORE a line of the correction was implemented

Codex's independent merge review (`_reviews/079-05-codex-merge-review.md`) returned **CHANGES
REQUIRED** on five findings. **Chat accepts all five.** This amendment fixes the corrected design and
the readings **before** the code exists, exactly as Amendments 1–3 did.

## A4.0 What is WITHDRAWN from the 079-03/079-04 record

🔻 **"`A2L_LEGA` FULL RECOVERY" IS WITHDRAWN AS A CLAIM.** `m079_shift_recovery.py` v1 scored the
**event token** — the fourth whitespace field of each `idx=` line — and called a session FULL when
every non-refused event token matched. Codex re-ran the same shifts and read the **per-run, per-edge**
returns underneath that token:

| label delta | published event score | correct ONSET offsets | correct END offsets |
|---|---|---:|---:|
| 0 | 6/6 | 8/8 runs | 8/8 runs |
| +1 | 6/6 | 8/8 runs | **7/8 runs** |
| −1 | 6/6 | **6/8 runs** | **6/8 runs** |

Named counterexamples, all on `A2L_LEGA/session_20260904-180418`: **δ=+1 run[29..30]** returned
`(−1, +2)` where `(−1, −1)` was correct; **δ=−1 run[8..9]** returned `(−2, −1)` where `(+1, +1)` was
correct; **δ=−1 run[27..28]** returned `(−4, −1)` where `(+1, +1)` was correct. In the last two the
detailed run reads `shift beyond the measurable range` while the EVENT reports the expected
`ONSET-SHIFT(+1)` **from its other run** — an unread run hidden behind a correct-looking token.

⇒ **the only session ever observed to "recover fully" does not recover every run and edge.**
**`MOTION_CAP = 0.040`'s positive anchor therefore falls**, and the constant is withdrawn with it.
⛔ The 079-03/079-04 numbers are **kept as history with a dated correction**, never edited away.

## A4.1 The corrected design — measurability becomes PER EDGE, local in time AND in region

**F1 accepted:** one whole-frame median taken over the **first** 24 clean pairs and reused by every
event is not a measurability test. It is blind in **time** (a quiet opening authorises a later moving
scene) and in **space** (a small region can change strongly while the whole frame barely moves).

⇒ **`MOTION_CAP` and the session-level refusal are DELETED.** The whole-frame `M_med` survives only
as a **printed reading**, computed over **all** clean pairs rather than the opening 24, and labelled
as a reading in the output. **Judgeability is decided per event, per contiguous run, per edge:**

| # | rule | refusal |
|---|---|---|
| **V1** | **Local regional baseline.** `tau_edge`/`base_edge` are learned from the `BASELINE_MAX_FRAMES` clean pairs NEAREST THAT EDGE, evaluated on **that edge's own region** — the box/mask actually used for the edge, not the event's anchor region. | `NOT-MEASURABLE(baseline: only N clean frame(s) near <edge frame>, need 3)` |
| **V2** | **Local regional change.** `m_edge` := the MEDIAN of that region's clean-pair `d` over that same local window. | `NOT-MEASURABLE(regional image change: m=… cap=… at <edge>)` |
| **V3** | **Edge dominance.** The chosen frame's `d` must exceed `tau_edge` **and** exceed the second-largest `d` **among the frames the argmax actually ranges over** (i.e. those with `d > tau_edge`) by **`DOMINANCE = 1.5`×**. | `NOT-MEASURABLE(ambiguous edge: best=… at k, second=… at k, ratio=… needs >=1.5x)` |
| **V4** | **NOT-VISIBLE only from a judgeable region.** Emitted only when V1–V3 hold for BOTH edges of the run, the end is not truncated, and NO frame in either searched window exceeds its own `tau_edge`. | otherwise the edge reads `NOT-MEASURABLE(no change above tau at this edge)` |
| **V5** | **Event verdict = worst run/edge, with EVERY run and EVERY edge printed.** | a run with one read edge and one refused edge prints both and makes the event `PARTIAL` |
| **V6** | **Unsatisfiable threshold and whole-frame region stay** — now evaluated PER EDGE. | unchanged wording, per edge |

**`DOMINANCE = 1.5` and the reason it is 1.5, stated before any reading:** a competitor within two
thirds of the chosen edge's magnitude is not distinguishable from it by this statistic, so the argmax
is a coin flip and the honest answer is a refusal rather than a frame number. **The second-largest is
taken over the ELIGIBLE set (`d > tau_edge`) and not over the whole window**, because that is the set
`_dominant_edge` actually chooses from — a frame that never cleared the detection threshold was never
a candidate. With fewer than two eligible frames the chosen edge is dominant by construction.

**Event-token severity order (worst first):** `NOT-VISIBLE` → `SHIFT` → `PARTIAL` → `NOT-MEASURABLE`
→ `PASS`. ⛔ **Exit codes are UNCHANGED**: `bad = SHIFT + NOT-VISIBLE`, exit 2 when `bad > 0`, else 0;
`3` for CANNOT-RUN. **`PARTIAL` is not a failure.**

## A4.2 The honest scorer — `m079_shift_recovery.py` v2

**F2 accepted.** The unit of scoring becomes the **(contiguous run × edge × perturbation) CELL**.

- **S1 FIXED COHORT.** The denominator is every contiguous run × both edges of every event of the
  **UNSHIFTED** session. **It never shrinks under perturbation**, so losing coverage can never raise a
  recovery rate.
- **S2 PERTURBATIONS.** `δ = 0`; **onset-only ±1**; **end-only ±1**; **both ±1** — each in two
  variants, reported separately: **annotation-only** (as today) and **coherent**, which also moves the
  per-frame `labels.jsonl` payload so region selection and clean-frame membership follow the
  annotation. **The RGB frames are never touched in either variant.**
- **S3 PER CELL:** `recovered` (the returned offset equals the expected one for that edge),
  `wrong` (an offset was returned and it is not the expected one), `refused` (NOT-MEASURABLE, with the
  reason). Recovery, wrong and refusal rates are published **separately**, with a reason histogram.
- **S4** runs `A2L_LEGA`, `LYRA_SMOKE_01`, `A1L_LEGA`, `M50L_LG9`, **guard DISABLED** (the raw gate)
  and **ENABLED** (the shipped behaviour), recording `m_edge` for every cell.
- **S5 `REGION_CAP` :=** the largest `m_edge` at which the **guard-disabled** cohort shows **ZERO
  `wrong` cells across all real sessions**, floored to two significant figures, and reduced by one
  unit in the last significant place if the floor would not be strictly below the smallest `wrong`
  cell's `m_edge`. Printed as **`provisional heuristic — from N real edges`** in header and lines.
  **If no cell is ever wrong**, the cap is the largest observed `m_edge`, flagged
  `provisional — no disagreeing cell observed`. **If the smallest-`m_edge` cell is already wrong**,
  say so plainly and set the cap so everything above the signal floor is refused — honesty over
  usefulness.
- ⛔ **`REGION_CAP` IS THE ONLY CONSTANT SET BY MEASUREMENT, AND ONLY BY THIS PROCEDURE.**
  `K_SIGMA`, `SIGNAL_FLOOR`, `MIN_BASELINE_FRAMES`, `BASELINE_MAX_FRAMES`, `MAX_REGION_FRAC`,
  `ATTAINABLE_MAX`, `DOMINANCE` are fixed here and are **not** to be moved by a result.

## A4.3 Wording, batch and the card (F3, F4, F5 — all accepted)

- `NOT-VISIBLE` is redefined everywhere as **"no detectable above-threshold change at either edge
  within the searched windows of a judgeable region; NOT a guarantee of absence."**
- `(bbox-only)` is explained: **the supplied box was used; no per-frame mask.**
- `camera motion` → **`regional image change`**, with its sampled scope stated. It is an
  image-change proxy, never a camera-displacement measurement.
- `region covers the picture` states the configured **90 % bounding-rectangle** rule.
- The cap is a **provisional heuristic** with its cohort provenance. ⛔ **"measured exactly"** and
  **"never calls a correct label NOT-VISIBLE"** are removed; the stale ring-normalised docstring at
  `verify_capture.py:838-841` is corrected.
- `bbox=… obs=…` is labelled **producer metadata** in the header legend.
- Summary gains **`PARTIAL n`**; the VERDICT line distinguishes
  **`PASS (n of N events fully checked)`** · **`PASS-PARTIAL (n fully, p partially, u unread)`** ·
  **`UNREAD (0 of N events checked)`** · **`FAIL (…)`**.
- **Batch:** identifier and output filename become the **relative path from the `--all` root** with
  separators replaced by `__`; `--report-only` **preserves CANNOT-RUN/error status** (returns 3 if any
  session read 3 or raised) and suppresses only the verdict-failure code.
- `docs/office-rdp-card.md` Section G gets a **prospective dated correction**: the stale header names,
  the stale verdict list, and the **"always exits 0"** sentence.
- `G259` gets a **dated append**: `tau` is bounded by its inputs (`d ∈ [0,1]` ⇒ `tau ≤ 4.0`), so the
  fault is that **`tau` can exceed `d`'s ceiling**, not that `tau` is mathematically unbounded. The
  home reproduction explains the two banked unsatisfiable events and the mechanism class — **not every
  office reading; those are still owed.**

## A4.4 THE PREDECLARED READINGS

**P1 — SELFTEST.** All 17 existing cases still pass, **plus** the V7 controls ported from
`_reviews/079-05-review-validation.py` with attribution:

| new case | cap | expected |
|---|---|---|
| `codex_quiet_prefix` (aligned; quiet opening, motion from frame 36, background burst at onset+1) | **shipped** | `NOT-MEASURABLE` or `PASS` — ⛔ never `SHIFT`, never `NOT-VISIBLE` |
| `codex_small_region` (aligned; 50×50 moving region in a static 320×240 picture) | **shipped** | `NOT-MEASURABLE` or `PASS` |
| `codex_alternating` (aligned; same region, alternating 1/3 px scroll; both true edges 10 % of region) | **shipped** | `NOT-MEASURABLE` or `PASS` |
| `codex_quiet_prefix_shift1` (label +1, hijack burst PRESENT) | **shipped** | `NOT-MEASURABLE` or `ONSET-SHIFT(-1)` — ⛔ never `PASS`, never `NOT-VISIBLE` |
| `codex_small_region_shift1` (label +1, hijack burst PRESENT) | **shipped** | `NOT-MEASURABLE` or `ONSET-SHIFT(-1)` |
| `codex_quiet_prefix_noburst_shift1` (label +1, burst REMOVED ⇒ the true edge IS dominant) | **off** | **`ONSET-SHIFT(-1)`** |
| `codex_small_region_noburst_shift1` (label +1, burst REMOVED) | **off** | **`ONSET-SHIFT(-1)`** |

🔑 **The last two are the load-bearing negative controls.** Without them V2/V3 could have *disabled*
the verdict rather than *scoped* it, and the suite would pass by refusing everything (`G96`/`G146`).

**Predicted existing-case changes: NONE of the 17 verdict TOKENS moves.** The two `NOT-VISIBLE` cases
survive — `blank_region` has `m_edge = 0` and `moving_blank_region` runs with the judgeability guard
**explicitly disabled**, which is an operator override and is printed as one. `moving_over_cap`'s
printed REASON changes from `camera motion` to `regional image change`; the token does not. ⚠ **Any
token that does move is listed in the result with the rule that moved it and why.**

**P2 — THE PINNED BENCH IS UNMOVED.** All six `M49_GEDGE_*` legs read **`PASS 4 / SHIFT 0 /
NOT-VISIBLE 0 / NOT-MEASURABLE 0 / PARTIAL 0`**, `VERDICT PASS (4 of 4 events fully checked)`,
`tau = 0.0040` on every edge, exit 0. Every edge measurable and dominant; `m_edge ≈ 0.0005`.
⛔ **If any bench event drops to `PARTIAL` or `NOT-MEASURABLE`, STOP — NEEDS-DECISION with the
numbers. Do not move `DOMINANCE` or `REGION_CAP` to recover it.**

**P3 — THE SCORER v2 TABLES.** For each of the four real sessions, guard **off** and **on**: the fixed
cohort size, and per perturbation cell the `recovered` / `wrong` / `refused` counts with the reason
histogram, for **both** the annotation-only and the coherent variant. Then `REGION_CAP` and the **N**
of real edges behind it. **Predicted, and this is the one that matters: the guard-disabled cohort
shows at least one `wrong` cell on `A2L_LEGA`** — Codex already named three. A table with zero wrong
cells anywhere would mean the scorer did not reproduce the review and is itself a **STOP**.

**P4 — CODEX'S FIXTURES THROUGH THE SHIPPED GATE.** The three aligned fixtures read **`PASS` or
`NOT-MEASURABLE` only**. ⛔ **A `SHIFT` or a `NOT-VISIBLE` on any of them is a FAIL of the correction —
STOP, NEEDS-DECISION with the numbers.**

**P5 — THE MOVING SESSIONS.** `M50L_LG9`, `A1L_LEGA`, `LYRA_SMOKE_01`: **zero `NOT-VISIBLE`, zero
`SHIFT`**. Their VERDICT lines read `UNREAD` or `PASS-PARTIAL` as the data dictate — **which one is
NOT predicted**, because per-edge judgeability may now read some edges that the session-level cap
refused wholesale. `A2L_LEGA` is likewise not predicted to stay `PASS 6`: per-edge refusal may turn
events `PARTIAL`, and **that is the correction working, not a regression.**

**P6 — BATCH.** Two sessions named `session_same` under different parents produce **two retained
reports** with distinct names and two distinguishable summary lines. `--report-only` over a batch
containing a session with no `labels.jsonl` returns **3**, matching single-session mode; a batch of
healthy sessions returns **0**; an unreadable session returns **3**.

## A4.5 Stop rules

- **P2 fails** ⇒ the correction has broken the pinned reference. **STOP.**
- **P4 fails** ⇒ the correction does not close finding 1. **STOP.**
- **P3 shows zero `wrong` cells with the guard disabled** ⇒ the scorer is not reproducing the review.
  **STOP.**
- ⛔ **No constant is moved to clear any of these.** Tuning a fixture so that it poses a fair question
  is legitimate and is stated; moving `DOMINANCE`, `K_SIGMA`, `SIGNAL_FLOOR` or a derived cap to move
  a verdict is the laundering shape and is forbidden.
- ⛔ **NO MERGE** either way. `Source/`, `Shaders/` and `measure_label_offset.py` stay byte-unchanged;
  no capture, build or cook.

---

# AMENDMENT 5 — 2026-09-15 (079-07), written BEFORE a line of the correction was implemented

Codex's second independent merge review (`_reviews/079-07-codex-merge-rereview.md`) returned
**CHANGES REQUIRED** on eight findings. **Chat accepts all eight**
(`_reviews/079-07-chat-disposition.md`). This amendment fixes the corrected design and the readings
**before** the code exists, exactly as Amendments 1–4 did.

## A5.0 WHAT IS WITHDRAWN AND WHAT IS RE-SCOPED

**F1 is ruled a SCOPE BOUNDARY, not a threshold problem, and that ruling is the whole of this
amendment.** Codex built a fixture in which a 20×20 patch appears on frames 60..67 and an unrelated
10 px background scroll happens once at frame 61, inside the labelled window, with every clean
baseline pair unchanged (`m_edge = 0`, `tau = 0.0040`). At the shipped cap the candidate reads
**`ONSET-SHIFT(+1)` FAIL on CORRECT labels**, and **`PASS` on a deliberately late label**. The burst
changes 0.4960 of the region against the real onset's 0.1600 and wins dominance by 3.1×.
**Reproduced here before this amendment was written; both readings confirmed.**

🔻 **WITHDRAWN: the `_edge_search` docstring's claim that dominance is "THE DIRECT CURE FOR
'SOMETHING ELSE WON THE ARGMAX'".** It is not. Dominance decides whether the argmax was CLOSE; it
does not decide whether the winner belongs to the TARGET. Lowering the cap does not help either —
the false shift survives at cap 0, because the baseline is already perfectly still.

🔑 **THE RULING: a frame-difference gate has no way to attribute an in-window transition to the
target without a per-frame mask.** Therefore:

| mode | when | what it may emit |
|---|---|---|
| **MASK MODE** | every edge of the run took its region from the delivered per-frame mask | `PASS` / `ONSET-SHIFT(n)` / `END-SHIFT(n)` / `NOT-VISIBLE` / `NOT-MEASURABLE` / `PARTIAL` |
| **BBOX-ONLY** | any edge of the run fell back to the supplied bounding box | **`READING` lines only. NEVER a verdict.** |

⛔ **THIS IS NOT A TUNING. It removes the F1 class outright for bbox-only data** — the region is no
longer asked a question it cannot answer — and it makes the client sentence true: verification is
real on M3 datasets and bench captures, and on bbox-only captures the tool points a human at named
frames.

⚠ **THE COST IS STATED, NOT DISCOVERED: `M50L_LG9` leaves the verdict surface entirely.** Measured
before this amendment: of the ten nominated bank sessions, the six pinned `M49_GEDGE_*` legs
(**64 edges**, correcting the 56 in 079-06's report), `A2L_LEGA` (16), `LYRA_SMOKE_01` (16) and
`A1L_LEGA` (12) take **100 %** of their edge regions from masks; `M50L_LG9` takes **0 of 16** and is
bbox-only on every edge. **No run anywhere in that set is mixed**, so the per-run rule has no
awkward case in the banked data.

**Mode is decided PER RUN, not per session or per edge, and the reason is a coupling, not a
convenience:** F2's ordering constraints run BETWEEN the two edges of a run (an onset's detected
frame bounds its own end; a run's detected clear bounds the next run's onset). An unattributed
onset's `best_k` can be any transition in the picture, so letting it bound an attributed end would
import exactly the ambiguity this ruling removes. A run is therefore in mask mode only if **both**
its edges are mask-attributed.

## A5.1 THE CORRECTED DESIGN — F1 to F8

### F1 — R1/R2/R3/R4

- **R1** In mask mode the region for every edge is the target's mask silhouette for that frame.
  This is **already the m49 path**: `_build_region` (`verify_capture.py:606-623`) prefers
  `row["mask_file"]` and only falls back to `mlo.bbox_from_label_entry` when there is no mask;
  `_region_from_mask` (`:563-603`) returns a binary silhouette crop, and `_region_frac` (`:671-712`)
  multiplies the frame's hot-pixel crop by that binary (`:708-711`). **No new region code is
  written; what changes is what the gate is ALLOWED TO CONCLUDE from a region that is not one.**
- **R2** In bbox-only mode every edge prints a READING line and the event token is `READING`; the
  summary counts `READINGS n`; the session VERDICT is
  `UNREAD-BBOX-ONLY (n events; readings printed - bbox-only sessions cannot be verified against
  pixels, a reviewer must look at the named frames)`; **exit 0**. Exit 2 remains reachable only from
  a mask-mode `SHIFT` or `NOT-VISIBLE`; exit 3 is unchanged. `--report-only` is unchanged.
- **R3** Mixed sessions are handled per run; READING runs count as **unread** for F7's coverage.
- **R4** Wording: the dominance "direct cure" claim is removed; the FAIL line becomes
  `labels not confirmed by pixels on n event(s)`; header, `client-readme.md` Step 6 and
  `office-rdp-card.md` Section G all carry *"verdicts are given only where per-frame masks identify
  the target's pixels (M3 datasets and bench captures); without masks the tool reports readings for
  human review and never a verdict."*

### F2 — constraints survive refusal

The search runs **before every refusal that has a region and a scannable window**, so a refused edge
still yields a `best_k` usable purely as an ORDERING CONSTRAINT. Where `tau` could not be learned or
is unsatisfiable, the search runs at `SIGNAL_FLOOR` and the result is marked bound-only; it is never
promoted to an answer. The dependent window is then bounded by the **DETECTED** edge if one exists,
**else by the LABELLED** edge:

- end of run `i`: `end_lo >= (onset.best_k + 1)` if detected, else `>= (rs + 1)`;
- onset of run `i+1`: `on_lo >= (prev_end.best_k + 1)` if detected, else `>= (prev_re_ + 2)`.

🔑 **DETECTED-ELSE-LABELLED, NOT `max(detected, labelled)`** — and the difference is load-bearing.
On a one-frame run whose labels are LATE by one, the true clear sits at the labelled onset frame
itself; a labelled floor of `rs + 1` would exclude the correct answer in exactly the case the gate
exists to catch. The brief's parenthetical — *"previous run's detected clear if found, else its
labelled end"* — is the operative rule and is the sound one; the looser `max(...)` phrasing beside
it is not adopted, and this deviation is declared here rather than discovered in a re-review.

If a bound empties the window ⇒ `NOT-MEASURABLE(window emptied by the preceding edge: …)`.

### F3 — coverage before judgement

`end_hi` is clamped to the session's last frame (it was not; that clamp changes **no** search result,
because `_edge_search` already skips absent frames — it makes the coverage denominator meaningful).
An edge is `read` only if **every frame pair of its FINAL window was scannable**; any absent frame ⇒
`NOT-MEASURABLE(frames missing in window: …)`.

⚠ **"FINAL window" is load-bearing and is declared now: the window is deliberately narrowed by the
run's own extent, by ordering bounds, by the foreign-event exclusion and by the session's ends.
Measured before this amendment, the banked edges scan 3–8 pairs of a nominal 9 because of those
bounds, not because of gaps.** Applying coverage to the nominal ±4 window would refuse every edge
in the bank, including all 64 pinned-bench edges. Foreign-event exclusions are reported separately
and are not "missing"; if they leave the window empty the edge refuses with that reason.

`NOT-VISIBLE` requires both edges read, i.e. both fully covered.

### F4 — immutable denominator (scorer)

The eligibility cohort — session × run × edge × perturbation × variant — is published **before**
scoring. An unsupported perturbation (a run removed or merged, a coherent payload that cannot be
constructed) no longer skips the session/variant: its cells are retained with
`outcome = "unsupported"` and an explicit reason. `derive_cap` ignores them (their `m_edge` is
`None`).

### F5 — no-signal is not a refusal (scorer) and the FAIL wording

Outcome classes become **`recovered` / `wrong` / `refused(<reason>)` / `no-signal` / `unsupported`**.
`no-signal` is `status=read, found=False` — the edge WAS read and nothing cleared `tau`. The 079-06
guard-off table's **385 "refused" cells contain 130 no-signal cells**; they are re-published split.
**"Zero wrong returned offsets" is not "zero wrong verdicts"** and the report says so.

### F6 — the zero boundary

`floor_2sf(0)` returns the same `(value, unit)` shape as the non-zero path (it returned a bare float
and `derive_cap` unpacked a pair — `TypeError`). A wrong cell at `m_edge = 0` yields the explicit
result **`NO ADMISSIBLE ENVELOPE`**, not a number. That state is a distinct sentinel the gate
accepts, prints in its header, and honours by **refusing every edge**.
⚠ **A numeric cap of `0.0` is NOT refuse-all and must never be described as one** — the test is
`m_edge > cap`, so a perfectly still region (`m_edge = 0`, which is **every one of the 64 pinned
bench edges**) still passes it. The header states that distinction explicitly.

### F7 — coverage aggregation, and a correction to AMENDMENT 4

🔻 **A4.1's V5 severity order (`NOT-VISIBLE → SHIFT → PARTIAL → NOT-MEASURABLE → PASS`, worst
run wins) is CORRECTED here by a dated amendment rather than rewritten in place.** It conflicts with
A4.1's own `PARTIAL` definition and with the client README's statement that `UNREAD` means nothing
could be read: a fully read and aligned run beside an unread run printed `NOT-MEASURABLE` for the
event and `UNREAD` for the session. The event token now comes from **coverage over all the event's
edges**:

```
R = edges that are status=read AND in a mask-attributed run;  E = all the event's edges
any run verdict is a SHIFT            -> that SHIFT          (a positive detection on a read,
                                                              dominant, attributed edge stands alone)
|R| == 0 and no run attributed        -> READING
|R| == 0                              -> NOT-MEASURABLE
|R| == |E| and any run NOT-VISIBLE    -> NOT-VISIBLE
|R| == |E|                            -> PASS
otherwise                             -> PARTIAL
```

⛔ Exit codes unchanged: `bad = SHIFT + NOT-VISIBLE`, exit 2 when `bad > 0`; `PARTIAL` and `READING`
are not failures.

### F8 — collision-safe identifiers

The batch identifier becomes `<flattened relative path>__<first 8 hex of sha1(relative path)>`, and
**uniqueness is asserted across the batch before anything is written**; a collision refuses with
exit 3 and lists the offenders. The summary prints the relative path beside the identifier.

### The wording corrections also required (Codex Q5)

- Section G card: *"exit 3 = NOTHING WAS READ"* → *"exit 3 = at least one session could not run;
  others may have been read."*
- The header's `7×2` shorthand → the real cohort definition; **N** explained as PERTURBATION CELLS
  that reached a baseline, never independent real edges; the retained positive support stated as it
  is (079-06: four original edges of two events in one session, i.e. static regions between events).
- Pinned-bench edge count **56 → 64**.

## A5.2 THE PREDECLARED READINGS

⛔ **No constant is tuned outside P4's procedure. `K_SIGMA`, `SIGNAL_FLOOR`, `MIN_BASELINE_FRAMES`,
`BASELINE_MAX_FRAMES`, `MAX_REGION_FRAC`, `ATTAINABLE_MAX`, `DOMINANCE` are fixed here and are not
to be moved by a result. `REGION_CAP` is re-derived by A4.2's S5 procedure, unchanged, on the
MASK-MODE cohort only** (bbox-only cells are readings, not verdicts, so they leave the cohort).

### P1 — SELFTEST: `SELFTEST: OK`, every case as tabled below

**EVERY EXPECTED-STRING CHANGE, WITH ITS REASON.** The seventeen 079-06 cases are all built by
`_synth_session`, which writes masks only when asked, so under R1/R2 all but one were bbox-only and
would become `READING` — which would delete the suite's ability to prove the gate can fail (G96).
**The fixtures therefore gain masks; the expectations do not move.** The mask is written as the
LABEL's own rectangle, so `blank_region` and `fullframe_region` keep lying about the target in
exactly the way they did.

| # | case | build change | cap | expected | change vs 079-06 and why |
|---|---|---|---|---|---|
| 1 | `clean` | +mask | off | `PASS` | token unchanged; masked so a verdict is still permitted |
| 2 | `clean_masked` | — | off | `PASS` | unchanged |
| 3 | `label_late_1` | +mask | off | `ONSET-SHIFT(-1)` | token unchanged; masked |
| 4 | `label_early_1` | +mask | off | `ONSET-SHIFT(+1)` | token unchanged; masked |
| 5 | `end_late_1` | +mask | off | `END-SHIFT(-1)` | token unchanged; masked |
| 6 | `end_early_1` | +mask | off | `END-SHIFT(+1)` | token unchanged; masked |
| 7 | `blank_region` | +mask (= the label's box) | off | `NOT-VISIBLE` | token unchanged; masked |
| 8 | `moving_clean` | +mask | off | `PASS` | token unchanged; masked |
| 9 | `moving_fast` | +mask | off | `PASS` | token unchanged; masked |
| 10 | `moving_label_late_1` | +mask | off | `ONSET-SHIFT(-1)` | token unchanged; masked |
| 11 | `moving_label_early_1` | +mask | off | `ONSET-SHIFT(+1)` | token unchanged; masked |
| 12 | `moving_end_late_1` | +mask | off | `END-SHIFT(-1)` | token unchanged; masked |
| 13 | `moving_end_early_1` | +mask | off | `END-SHIFT(+1)` | token unchanged; masked |
| 14 | `moving_blank_region` | +mask | off | `NOT-VISIBLE` | token unchanged; masked |
| 15 | `moving_fullframe_region` | +mask (= whole frame) | off | `NOT-MEASURABLE` | token unchanged; masked |
| 16 | `moving_unsat` | +mask | off | `NOT-MEASURABLE` | token unchanged; masked |
| 17 | `moving_over_cap` | +mask | **shipped** | `NOT-MEASURABLE` | token unchanged; **this is the case that proves the per-edge regional guard still FIRES in mask mode** |

| # | 079-05 Codex case | cap | expected | change and why |
|---|---|---|---|---|
| 18 | `codex_quiet_prefix` | shipped | **`READING`** | was `NOT-MEASURABLE\|PASS`. Bbox-only; under R2 the whole class is unattributable, which is STRICTLY STRONGER than the per-edge guard it used to test |
| 19 | `codex_small_region` | shipped | **`READING`** | as above |
| 20 | `codex_alternating` | shipped | **`READING`** | as above |
| 21 | `codex_quiet_prefix_shift1` | shipped | **`READING`** | was `NOT-MEASURABLE\|ONSET-SHIFT(-1)`; bbox-only |
| 22 | `codex_small_region_shift1` | shipped | **`READING`** | as above |
| 23 | `codex_quiet_prefix_noburst_shift1` | off | **`READING`** | was `ONSET-SHIFT(-1)`; bbox-only. Its can-still-fail role moves to case 24 |
| 24 | `codex_quiet_prefix_noburst_shift1_masked` | off | `ONSET-SHIFT(-1)` | **NEW.** The load-bearing negative control, carried into mask mode so refusing everything still cannot pass the suite |
| 25 | `codex_small_region_noburst_shift1` | off | **`READING`** | was `ONSET-SHIFT(-1)`; bbox-only |
| 26 | `codex_small_region_noburst_shift1_masked` | off | `ONSET-SHIFT(-1)` | **NEW**, same role as 24 |

| # | 079-07 Codex case (all NEW) | cap | expected | what it proves |
|---|---|---|---|---|
| 27 | `c07_burst_aligned` | shipped | `READING` | **F1/R2.** The false `ONSET-SHIFT(+1)` becomes a reading |
| 28 | `c07_burst_aligned_masked` | shipped | `PASS` | **F1/R1.** Chat's stated requirement |
| 29 | `c07_burst_late` | shipped | `READING` | **F1/R2.** The false `PASS` becomes a reading |
| 30 | `c07_burst_late_masked` | shipped | `ONSET-SHIFT(-1)` | **F1/R1.** Chat's stated requirement |
| 31 | `c07_onset_early_refusal` | shipped | `READING` | **F2/R2** |
| 32 | `c07_onset_early_refusal_masked` | shipped | `PASS` / `PARTIAL` / `NOT-MEASURABLE` — ⛔ **never a SHIFT, never NOT-VISIBLE** | **F2.** A refused onset must still bound its own end |
| 33 | `c07_previous_end_refusal` | shipped | `READING` | **F2/R2** |
| 34 | `c07_previous_end_refusal_masked` | shipped | `PASS` / `PARTIAL` / `NOT-MEASURABLE` — ⛔ **never a SHIFT, never NOT-VISIBLE** | **F2 across runs** |
| 35 | `c07_previous_end_control_masked` | shipped | `PASS` / `PARTIAL` / `NOT-MEASURABLE` — ⛔ **never a SHIFT** | **F2** with the prior end READ, Codex's follow-up |
| 36 | `c07_partial_multirun_masked` | shipped | `PARTIAL` | **F7.** One read+aligned run beside one refused run |
| 37 | `c07_partial_multirun_bbox` | shipped | `READING` | **R2/R3** |
| 38 | `c07_missing_edge_frames_masked` | shipped | `NOT-MEASURABLE` | **F3.** Never `NOT-VISIBLE` from zero observations |
| 39 | `c07_missing_edge_frames` | shipped | `READING` | **F3/R2** |
| 40 | `c07_below_tau_masked` | shipped | `NOT-VISIBLE` | **F5.** The detection limit, named beside the README sentence |
| 41 | `c07_zero_cap_static_masked` | **0.0** | `PASS` | **F6.** A numeric cap of 0 is not refuse-all |
| 42 | `c07_zero_cap_burst_masked` | **0.0** | `PASS` — ⛔ **never a SHIFT** | **F6** on the F1 burst |
| 43 | `c07_no_envelope_masked` | **NO ADMISSIBLE ENVELOPE** | `NOT-MEASURABLE` | **F6.** The sentinel really does refuse everything |

Plus the two `real±1` cases against `--dir`, unchanged in intent.

⛔ **A case reading outside its declared set is a FAIL. The tables are not edited to match a result;
NEEDS-DECISION is reported with the numbers.**

### P2 — THE PINNED BENCH IS UNMOVED

All six `M49_GEDGE_*` legs: **`PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / PARTIAL 0 / NOT-MEASURABLE 0 /
READINGS 0`**, `VERDICT PASS (4 of 4 events fully checked)`, `tau = 0.0040` on every edge, exit 0,
**64 detailed edges**, every one mask-attributed with `m_edge = 0.000000` (measured before this
amendment).
⛔ **If any bench event drops to `PARTIAL`, `NOT-MEASURABLE` or `READING`, STOP — NEEDS-DECISION
with the numbers. Do not move `DOMINANCE` or `REGION_CAP` to recover it.**

### P3 — THE BANK SESSIONS

- `A2L_LEGA`, `LYRA_SMOKE_01`, `A1L_LEGA` (mask mode on 100 % of their edges): **mask-mode readings.
  Zero `SHIFT` and zero `NOT-VISIBLE` on all three.** Which of `PASS` / `PASS-PARTIAL` / `UNREAD` each
  prints is **NOT predicted** — F2's new bounds and F3's coverage rule may read or refuse edges that
  079-06 did the other way, and that is the correction working.
- `M50L_LG9` (bbox-only on 16 of 16 edges): **`UNREAD-BBOX-ONLY`, six `READING` event lines,
  exit 0.**
- 🚨 **If a mask-mode edge IS fully read AND dominant AND disagrees, the frames and the numbers are
  printed and it is reported as a FINDING about that session's labels — not as a failure of this
  brief, and not smoothed away.**

### P4 — SCORER v2.1 ON THE MASK-MODE COHORT

Guard off and guard on, over `A2L_LEGA`, `LYRA_SMOKE_01`, `A1L_LEGA` only. The immutable denominator
is published FIRST. Five classes reported separately. `REGION_CAP` re-derived by S5, or
`NO ADMISSIBLE ENVELOPE`, with provenance in the new wording. The 079-06 780-cell tables are
additionally re-published from the banked `cohort.json` with the 130 no-signal cells split out.
**Predicted: the guard-disabled mask-mode cohort still shows at least one `wrong` cell** — Codex
named the binding one (`A1L_LEGA` event 0, run[4..5], onset, annotation-only both +1, expected −1,
returned +1, `m_edge = 0.00014459`) and `A1L_LEGA` is a masked session, so it stays in the cohort.
**A table with zero wrong cells anywhere would mean the scorer did not reproduce the review and is
itself a STOP.**

### P5 — BATCH

Codex's collision fixture (`a__b/session_same` and `a/b__session_same`) retains **two** reports with
distinct names and two distinguishable summary lines; a batch containing a session with no
`labels.jsonl` returns **3** under `--report-only`; a mixed batch returns 3 while still reading the
others.

## A5.4 CORRECTION TO A5.2's P1 TABLE, appended 2026-09-15 the moment the fixture answered

🔻 **CASE 35's PREDECLARED EXPECTATION SET WAS DEFECTIVE AS WRITTEN, AND THE BUILD IS NOT AT
FAULT.** `c07_previous_end_control_masked` was declared `PASS / PARTIAL / NOT-MEASURABLE — never a
SHIFT`. It reads **`END-SHIFT(+1)`**, and that is the CORRECT answer: the fixture's RGB carries its
first patch on frames 10..12 while its labels claim 10..11, so its first end really is one frame
early. Codex's own prediction for this control says so in as many words — *"first end +1, second
onset 0"*. **I forbade the fixture's own known answer**, which would have made the case fail
whatever the tool did. This is the same shape as journal 073's `A1-ONSET-JOIN` conjunct: **a defect
in the PRE-DECLARATION, not in the build.**

⛔ **The table above is NOT edited.** The correction rides here, dated and appended.

**AND THE REPAIR IS A STRENGTHENING, NOT A RELAXATION — because the event TOKEN could not have
answered the question this case exists to ask.** `_event_token` returns the FIRST run's shift, so
`END-SHIFT(+1)` would have printed identically whether the SECOND run's onset was right or wrong —
which is `G262`'s exact shape, a token hiding a per-unit failure, inside the correction for it.
The five F1/F2 cases therefore gain a **PER-EDGE** expectation checked against `out_detail`, and
that is now what they are judged on:

| case | expected per-edge offsets (`r` = refused) | what it pins |
|---|---|---|
| `c07_burst_aligned_masked` | `+0, +0` | the burst does not become an onset |
| `c07_burst_late_masked` | `-1, +0` | the real onset is still found through the burst |
| `c07_onset_early_refusal_masked` | `r, +0` | a REFUSED onset still bounds its own end — it read `-2` before |
| `c07_previous_end_refusal_masked` | `+0, r, +0, +0` | a REFUSED end still bounds the NEXT run's onset — `-2` before |
| `c07_previous_end_control_masked` | `+0, +1, +0, +0` | a READ end bounds it too, and the real `+1` is still reported |

**Measured: all five hold.** The two `-2` readings Codex reproduced are gone, and the fixture's
genuine `+1` survives — the constraint removed an ANSWER only where the answer was unfounded.

## A5.5 ONE CASE ADDED AFTER THE AMENDMENT, ITS EXPECTATION FIXED BEFORE IT RAN

**`c07_mixed_runs` — expected `PARTIAL`, at the shipped cap. Written here before the fixture
existed.** A5.1's **R3** says mixed sessions are handled per run and that READING runs count as
UNREAD for F7's coverage, and nothing in the P1 table tested it: **every banked session is wholly
masked or wholly bbox-only, so the mixed case has no natural example.** The fixture is Codex's
`partial_multirun` geometry with masks written on the FIRST run's frames only — run `[10..11]`
attributed and readable, run `[25..26]` bbox-only. Required: the event reads **`PARTIAL`** and the
session **`PASS-PARTIAL`**; ⛔ **never `PASS`** (which would credit the unattributable run) and
⛔ **never `UNREAD`** (which would discard the run that WAS read).

⚠ **It also guards a latent inconsistency found while writing the correction and fixed before any
result was read: `_event_token` counted ATTRIBUTED EDGES individually while mask mode is a
property of the RUN**, so a run with one masked and one unmasked edge could have contributed a
verdict that the run itself had refused to give. The token now counts only the edges of runs that
are in mask mode. **No banked session exercises that path**, which is precisely why it needed a
fixture rather than a reading.

## A5.3 STOP RULES

- **P2 fails** ⇒ the correction has broken the pinned reference. **STOP, NEEDS-DECISION.**
- **P1 case 28 or 30 fails** ⇒ R1 does not close F1. **STOP.**
- **P1 cases 24/26 fail** ⇒ the suite can no longer prove the gate fails. **STOP.**
- **P4 shows zero `wrong` cells with the guard disabled** ⇒ the scorer is not reproducing the review.
  **STOP.**
- ⛔ **No constant is moved to clear any of these.** Tuning a fixture so that it poses a fair question
  is legitimate and is stated; moving a threshold to move a verdict is forbidden.
- ⛔ **NO MERGE either way.** `Source/`, `Shaders/` and `measure_label_offset.py` stay byte-unchanged;
  no capture, build or cook.
