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
