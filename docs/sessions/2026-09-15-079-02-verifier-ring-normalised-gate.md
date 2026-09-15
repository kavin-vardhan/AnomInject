# 2026-09-15 — 079-02 — ring-normalised label-pixel gate: BUILT, GATED, **REFUTED BY ITS OWN GATES**

**VERDICT: NEEDS-DECISION.** `P1` (selftest) and `P5` (batch) pass. **`P2`, `P3` and `P4` FAIL.**
The predeclared stop fired: *"If P2 or P3 fails, STOP and report NEEDS-DECISION with the numbers;
do not tune constants to pass."* No constant was tuned. Nothing is merged.

**Branch:** `fix/verifier-field-validity`. **Predictions:** `docs/predictions/2026-09-15-verifier-field-validity.md`
**AMENDMENT 1**, committed `b4ccd11` **before a line of the fix was written**.
**Scope honoured:** `tools/verify_capture.py` only; `measure_label_offset.py` **byte-unchanged**
(`git diff` confirms); `Source/`, `Shaders/`, fixtures, schema, harness untouched. No capture, build
or cook. Evidence: `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\` (20 files).

⛔ **THE IMPLEMENTATION MUST NOT BE MERGED AS IT STANDS.** It is committed on the branch for
reproducibility, in the house precedent of `s3a-2-GATE-FAILED-do-not-merge`.

---

## 1. The readings

| | prediction | reading | |
|---|---|---|---|
| **P1** selftest | 16/16, moving cases too | **16/16 OK** | ✅ |
| **P2** pinned bench inert | 6 legs `PASS 4`, `tau=0.0040` | **4 legs PASS; `MT_NAT` and `MT_SYN` → `NOT-VISIBLE 4`** | ❌ |
| **P3** `M50L_LG9` flips | 0 NOT-VISIBLE | **4 NOT-VISIBLE, VERDICT FAIL** | ❌ |
| **P4** other Lyra legs | no new NOT-VISIBLE, PASS ≥ before | **`A1L_LEGA` +1 SHIFT; `LYRA_SMOKE_01` +1 NOT-VISIBLE** | ❌ |
| **P5** batch agrees | counts identical, `CANNOT RUN` handled | **identical; handled** | ✅ |

`tau` still lands on `SIGNAL_FLOOR = 0.0040` on every pinned bench leg, and `base=` dropped 37 → 24
exactly as AMENDMENT 1 declared. **The failures are not threshold drift.**

---

## 2. Failure 1 (P2) — ring subtraction DESTROYS a real small signal, and this project had already
## measured why

`M49_GEDGE_MT_NAT`, event 0, measured frame by frame (`diag-MT_NAT-ring-swamps-signal.txt`):

```
frame 1280x720   region = mask(sole v223) 66837px   bbox=(0,485,306,720)   ring area = 28272px
    k   d_region     d_ring        net
    2    0.00000    0.00195   -0.00195
    3    0.02286    0.02872   -0.00586   <== LABEL ONSET
    4    0.00000    0.00209   -0.00209
   11    0.02280    0.02833   -0.00553   (clear)
```

The anomaly turns **1,528** target pixels hot **and 812 ring pixels** hot. The silhouette is
66,837 px and the ring only **28,272 px** — the target sits in the **bottom-left corner**, bbox
`(0,485,306,720)`, so the 48 px band is clamped away on two sides. A smaller denominator makes the
ring's *fraction* larger than the region's, so **`net` goes NEGATIVE at the very frame the anomaly
starts** and no edge can ever be found.

🚨 **THE RING'S PREMISE IS THAT THE ANOMALY IS CONFINED TO THE REGION, AND THIS PROJECT MEASURED
THAT FALSE THREE MILESTONES AGO.** `m26` ruling `A-4`/`A35`: on `SM_Ramp2` the largest change from
hiding the target was **OUTSIDE its own bbox — peak-OUT 0.2955 vs peak-IN 0.1785.** `G258` is the
same family. A texture swap changes bounce light on the floor beside it; that light lands in the
ring, and subtraction charges it against the anomaly.

🔑 **AND JOURNAL 071 NAMED THIS EXACT LEG AS THE ONE THAT WOULD BREAK.** Verbatim: *"`missing_texture`
**0.0229** at AA-off vs 0.2627 delivered, i.e. **5.7×τ, the thinnest margin in the whole gate set**
… **the one to watch if the threshold, target or arbiter recipe changes.**"* The measured
`d_region` here is **0.02286**. The prediction fired on the nose, four milestones later, against a
statistic change rather than a threshold change.

---

## 3. Failure 2 (P3/P4) — ring subtraction does NOT cancel camera motion

If the ring cancelled motion, `net` on clean frames would sit near zero and `tau` would fall to the
floor. On `M50L_LG9` (`M_med = 0.349`) it does not: `tau` on `net` reads **0.1787 / 0.2485 / 0.3535 /
0.3746 / 0.4426**.

🔑 **BECAUSE CAMERA MOTION IS NOT A GLOBAL SCALAR — IT IS PARALLAX- AND CONTENT-WEIGHTED.** A region
full of near, detailed geometry changes far more under a given camera move than a ring containing
flat distant wall. `d_region − d_ring` therefore carries a large content-dependent variance, and
`6·MAD` keeps `tau` high.

⚠ **THIS CORRECTS A DOCUMENTED CLAIM IN THE SIBLING MODULE.** `measure_label_offset.py:92-93` says
*"a whole-frame change lifts the ring as much as the region and cancels."* That is true for a
**genuinely global** change — exposure, a fade — and **false for camera motion with parallax**. (The
module survives it because it *also* carries the `ambient > AMBIENT_FACTOR·T ⇒ UNMEASURABLE` escape
at `mlo:126`; it does not rely on cancellation alone. **The gate copied the subtraction and not the
escape** — the mirror image of 079-01's finding, where it copied the constants and not the ring.)

---

## 4. Failure 3 — 🔻 **079-01 OVER-CLAIMED, AND I AM WITHDRAWING IT: `M50L_LG9` IS NOT A KNOWN-GOOD
## SESSION**

079-01 called it *"a session whose labels are independently established as correct"* and the brief
inherited that as *"the known-good-called-NOT-VISIBLE case"*. **Read from the artifact, all six
events say otherwise:**

```
0..5  observability_measured = False    bbox_source = "projected"    affected == injected
```

**The producer itself declares it has NO pixel evidence for any of these labels.** They are the
`m49` R1 fallback: observability could not be measured, so the *injected* set is reported, with a
**bounds-projected** box. Region sizes: **11.7 %, 20.0 %, 30.4 %, 43.5 %, 79.6 % and 100 % of the
frame** — the `G124`/H3 pathology.

**Where the over-claim came from, so the mistake is legible:** journal 076 records `LG9` as `m50`'s
headline success — `vetoed_events` 6 → 0, `anomalies: []` → **6 events delivered**. That success was
*shipping honest events instead of nothing*, **explicitly flagged `observability_measured: false`.**
It was never a claim of pixel accuracy. I read "m50's success" as "known-good labels" and carried it
into 079-01.

### 4.1 And the pixels agree there is nothing to find

Event 2, per-frame `net` across its window (`diag-M50L_LG9-series.txt`):

```
   36 0.1138   37 0.1405   38 0.0933   39 0.0695   40 0.0585 <== LABEL ONSET   41 0.0143
```

**The labelled onset frame has the LOWEST local excess in its entire neighbourhood — lower than the
four clean frames before it.** No first-order statistic can place an edge there.

Amplified difference images (`diff_ev2_onset_vs_control.png`): the clean→clean control `38→39` is
**indistinguishable** from the label onset `39→40` — both are whole-frame camera-motion edge
structure across walls, floor grid, characters and VFX. The eye-check composites
(`eyecheck_ev*.png`) show the same: **a 30 %-of-frame red box in which a human cannot see anything
appear or disappear either.**

⇒ **On these events there is no recoverable signal, and the honest verdict is NOT-MEASURABLE.** The
gate reaching NOT-VISIBLE is the wrong *label* for a correct *observation*.

---

## 5. What still stands, and what does not

⛔ **NOT weakened: 079-01's diagnosis and `G259`.** The saturation mechanism is unchanged and was
measured (`tau` 1.0950 / 1.1938 against a statistic capped at 1.0). What 079-02 refutes is the
**chosen remedy**, not the disease.

🚨 **But the home surrogate is now known to conflate THREE things**, and only the first is what the
field complaint is about:

| | |
|---|---|
| **H1** threshold saturation under motion | the field defect; real; `G259` |
| **H3** projected bboxes covering 30–100 % of frame | `G124`; makes localisation impossible in principle |
| **unmeasured labels** (`observability_measured: false`) | nothing to verify against |

**Seven of the eight implemented elements are NOT implicated in any failure and should survive:**

| | status |
|---|---|
| **F-A** ring **subtraction** | 🔴 **REFUTED** — §2 and §3 |
| **F-B** unsatisfiable-threshold assertion | ✅ fires correctly in selftest; would turn the field's NOT-VISIBLE into an honest NOT-MEASURABLE |
| **F-C** no ring ⇒ no read | ✅ correctly caught the 100 %-of-frame event |
| **F-D** per-event nearest baseline | ✅ no harm observed; `base=24` as declared |
| **F-E** `(bbox-only)` tag | ✅ |
| **F-F** header envelope + `M_med` | ✅ made §3 diagnosable in one line |
| **F-G** `--all` batch | ✅ P5; selftest-first refusal works |
| **F-H** moving-camera selftest | ✅ **and it is now the thing that would catch this class** |

---

## 6. Recommendations for 079-03 (chat's call — nothing here is adopted)

1. **Use the ring as a VALIDITY TEST, not a SUBTRACTION.** Keep raw `d` as the decision statistic and
   add `mlo`'s own escape: `ambient > AMBIENT_FACTOR · tau ⇒ NOT-MEASURABLE`. With **F-B** this turns
   the field's unconditional NOT-VISIBLE into an honest NOT-MEASURABLE **and leaves the pinned bench
   byte-inert** — no small signal is destroyed, because nothing is subtracted.
   ⚠ **Be honest about what that buys: acceptance rule (a) YES, rule (b) probably NOT** on
   heavy-motion sessions. §4.1 suggests there may be **no recoverable signal at all** there, in which
   case no tool can deliver rule (b) and the right answer is to say so.
2. **Make `observability_measured: false` + `bbox_source: "projected"` NOT-MEASURABLE by
   construction.** Schema-derived, not tuned: the gate is being asked to verify a label whose
   producer already says it has no pixel evidence, against a bounds-derived box.
3. **A better surrogate is needed.** One with `observability_measured: true`, **drawn** (not
   projected) bboxes, and real camera motion. `A1L_LEGA` / `LYRA_SMOKE_01` are closer.
   🎯 **And the cheapest way to find out whether the FIELD data is that shape is the owner's
   transcribed readings, which have still not arrived.** The two fields to ask for are
   `observability_measured` and `bbox_source`; if the M2 captures are also `false`/`projected`, then
   the field complaint is partly unanswerable and that is the finding.
4. **Do not merge this branch as it stands.**
