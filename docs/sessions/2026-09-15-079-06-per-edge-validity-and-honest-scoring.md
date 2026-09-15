# 2026-09-15 — 079-06 — per-edge validity, an honest cohort scorer, and two withdrawn claims

**Codex's independent merge review (`_reviews/079-05-codex-merge-review.md`) returned CHANGES
REQUIRED on five findings. Chat accepted all five. All five are corrected here.**

⛔ **NOT MERGED.** Branch `fix/verifier-field-validity`; `master` and `m51` untouched. `Source/`,
`Shaders/` and `tools/measure_label_offset.py` **byte-unchanged against `ac13700`** throughout — no
capture, no build, no cook, no push of anything but this branch.

**Predeclared:** `docs/predictions/2026-09-15-verifier-field-validity.md` **AMENDMENT 4**, committed
**`789dd8d`** before a line of the correction existed.
**Response to the reviewer:** `_reviews/079-06-code-response-to-codex-079-05.md`.
**Evidence:** `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-06\`.

---

## 0. Two claims are WITHDRAWN, and that is the headline

### 0.1 🔻 "`A2L_LEGA` fully recovers an injected ±1" is FALSE

079-03 reported it and 079-04 built `MOTION_CAP = 0.040` on it. The score came from the **event
token** — the fourth whitespace field of each `idx=` line. Codex re-ran the identical shifts and read
the per-run, per-edge returns underneath:

| label delta | published event score | correct ONSET edges | correct END edges |
|---|---|---:|---:|
| 0 | 6/6 | 8/8 runs | 8/8 runs |
| +1 | 6/6 | 8/8 runs | **7/8 runs** |
| −1 | 6/6 | **6/8 runs** | **6/8 runs** |

with **δ=+1 run[29..30]** returning `(−1, +2)` for `(−1, −1)`, and **δ=−1 run[8..9]** and
**run[27..28]** returning `(−2, −1)` and `(−4, −1)` for `(+1, +1)`. In the last two the run itself
printed `shift beyond the measurable range` while the EVENT printed the expected `ONSET-SHIFT(+1)`
**from its other run**.

⇒ **`MOTION_CAP = 0.040` loses its positive anchor and is DELETED, not re-tuned.** The 079-03/079-04
numbers stay in their journals as history; the correction is appended, never edited over. → **`G262`**.

### 0.2 🔻 "the cap is MEASURED, not chosen" / "measured exactly" / "never calls a correct label NOT-VISIBLE"

All three are removed from the tool header and `client-readme.md`. What ships is a **PROVISIONAL
HEURISTIC** with its cohort, its N and its smallest-disagreeing cell printed beside it, and an
explicit *"not a validated client acceptance envelope"*.

---

## 1. The correction — measurability is now PER EDGE, local in time AND in region

**Finding 1, accepted.** One whole-frame median over the **first** 24 clean pairs, reused by every
event, is blind twice over and — as the review says — **not repairable by lowering the constant**:

| blindness | reproduced as | reading |
|---|---|---|
| **in TIME** | a fixture still for 36 frames, then scrolling, with the labelled window in the moving part | whole-frame median over the opening **0.0000**; the value local to that window **0.1200** |
| **in SPACE** | a 50×50 region scrolling inside a static 320×240 picture | whole-frame **0.0039**; regional **0.1200** — a factor of **31** on the same picture |

⇒ `MOTION_CAP` and the session-level refusal are **gone**. `_measure_edge()` decides each edge on its
own:

- **V1** `tau_edge` from the 24 clean pairs **nearest that edge**, on **that edge's own region** (the
  box or mask actually used there — which also closes the anchor-vs-run calibration mismatch Codex
  named in Q1).
- **V2** `m_edge` = the **median of that region's clean-pair `d`** over the same local window;
  `> REGION_CAP` ⇒ `NOT-MEASURABLE(regional image change: m=… cap=…)`.
- **V3** the winner must beat the **runner-up among the frames that cleared tau** by **1.5×**, or
  `NOT-MEASURABLE(ambiguous edge: …)`. An argmax always returns something and never says the contest
  was close.
- **V4** `NOT-VISIBLE` only when **both** edges are judgeable and **neither** finds anything.
- **V5** every run and every edge is printed; an event with some edges read and some refused is
  **`PARTIAL`**, counted separately from PASS and from NOT-MEASURABLE.
- **V6** the `tau >= 1` and whole-frame-region refusals survive, now per edge.

The whole-frame median remains as a **printed reading over ALL clean pairs**, labelled
`READING ONLY … it gates nothing`. Even as a reading it improves: on the quiet-prefix fixture it goes
**0.0000 → 0.1250** once it stops sampling only the opening.

⚠ **Which rule actually does the work on Codex's three fixtures: the REGIONAL one, not dominance.**
All three have `m_edge = 0.1200` on every edge. Their hijacking burst at `onset+1` genuinely is the
largest change in the window (ratio ≈ 2.4×), so **a dominance test alone would still have emitted the
shift.** Dominance earns its place elsewhere — §2.2.

---

## 2. The honest scorer, and the two defects it found

**Finding 2, accepted.** `CaptureBench/tools/m079_shift_recovery.py` v2 scores the
**(run × edge × perturbation) CELL**:

- **fixed cohort** from the **unshifted** session — it never shrinks, so lost coverage cannot raise a
  rate. v1 dropped refusals from the denominator, so **the harder the case, the better it scored**.
- **seven perturbations** — `δ=0`, onset-only ±1, end-only ±1, both ±1 — each in an
  **annotation-only** and a **coherent** variant (the latter also moves the per-frame `labels.jsonl`
  payload and its `mask_file`). **The RGB is never touched in either.** That is Codex Q2's separation.
- **three outcomes**, published separately with a reason histogram; a read edge that found nothing
  counts as **refused**, never as agreement.

### 2.1 My own scorer bug, caught before any number was published

The coherent variant used `mlo.match_label_entry` to **delete** an event's entries. That function
falls back to *"the only entry on the row"* when nothing matches — right for the gate, wrong for a
delete: it stripped every **other** event's entries too, leaving **2 of 16 cohort cells alive**.
Fixed to a strict `target_name` comparison. 🔑 **A matcher written to be generous is dangerous when
reused to remove things.**

### 2.2 🚨 A REAL DEFECT IN THE GATE, exposed by the guard

Refusing an **onset** for regional change also removed the lower bound it placed on the **end**
search (`end_lo = onset_k + 1`). The end search then re-found the onset transition and returned a
confident **`−1` where `+1` was correct** — a cell that was right with the guard OFF and wrong with it
ON. The search now runs **before** the judgeability refusal:

> **A refusal must remove an ANSWER, never a CONSTRAINT.**

⚠ **Nothing in the brief asked for this; it surfaced only because the same fixed cohort was scored at
both guard settings.** Without that, it would have shipped.

---

## 3. `REGION_CAP` — derived, and what derived it

**The cohort: 780 cells** — 4 sessions × (every contiguous run × both edges) × 7 perturbations × 2
label variants, fixed from each unshifted session. **736 of them reached a baseline** and therefore
carry an `m_edge`.

### 3.1 GUARD DISABLED — the raw gate, and the only place a cap may be derived from

```
TOTAL cells 780   recovered 376 (48.2%)   WRONG 19 (2.4%)   refused 385 (49.4%)
```

**All nineteen wrong cells, sorted by `m_edge` — this list IS the derivation:**

| `m_edge` | session | variant | perturbation | cell | expected | READ |
|---:|---|---|---|---|---:|---:|
| **0.000145** | `A1L_LEGA` | annotation | both +1 | ev0 run[4,5] **onset** | −1 | **+1** |
| 0.000434 | `A1L_LEGA` | annotation | onset +1 | ev0 run[4,5] onset | −1 | +0 |
| 0.000434 | `A1L_LEGA` | coherent | onset +1 | ev0 run[4,5] onset | −1 | +0 |
| 0.001504 | `A1L_LEGA` | annotation | **both 0** | ev0 run[4,5] onset | +0 | **+1** |
| 0.004539 | `A2L_LEGA` | annotation | both −1 | ev2 run[28,29] end | +1 | −1 |
| 0.004557 | `A2L_LEGA` | coherent | both −1 | ev2 run[28,29] end | +1 | −1 |
| 0.008547 · 0.008552 · 0.009075 ×2 | `LYRA_SMOKE_01` | both | both −1 / end −1 | ev5 run[87,89] end | +1 | −1 |
| 0.038920 · 0.038944 | `LYRA_SMOKE_01` | both | both −1 | ev2 run[40,41] end | +1 | −1 |
| 0.215142 … 0.226289 (×7) | `M50L_LG9` | both | onset +1 / end ±1 | ev1 run[27,34] both edges | 0 / −1 / +1 | +1 / +0 / +2 |

⇒ **`REGION_CAP = 0.00014`** — `floor_2sf(0.000145)`, strictly below the smallest wrong cell, from
**N = 736** real edges.

🚨 **THE SMALLEST WRONG CELL IS A SIGN INVERSION, AND IT SETS THE CAP ON ITS OWN.** `A1L_LEGA`
event 0 (`blink` on `Cube7`, the event journal 073 recorded as walking off the right edge of the
frame) returned **`+1` for an injected `−1`** at `m_edge = 0.000145` — a region quiet enough to look
judgeable. **A small `m_edge` is therefore NECESSARY, NOT SUFFICIENT**, and the cap is pinned by a
single edge of a single event. Said here rather than left to be discovered.
⚠ **The same edge reads `+1` on an UNSHIFTED label** (`both 0`, `m_edge = 0.001504`). At the shipped
cap both cells are refused, so it never reaches a verdict — but it is a real reading and it is not
hidden.

### 3.2 GUARD ENABLED at 0.000140 — what the shipped tool does

```
TOTAL cells 780   recovered 52 (6.7%)   WRONG 0 (0.0%)   refused 728 (93.3%)
```

**Zero wrong cells, and the price is 86 % of the recovery.** Every surviving read is on `A2L_LEGA`
(4 of 16 cells in every perturbation — the two `Cube2_8` events, `m_edge ≤ 0.0001`). `LYRA_SMOKE_01`,
`A1L_LEGA` and `M50L_LG9` read **nothing at all**.

🔑 **That is the trade, stated in the direction it actually runs: this correction did not make the
tool more useful. It made it stop being wrong, and the honest consequence is that it is now silent on
three of four real moving-camera sessions.** The refusal reasons are printed per edge, so the silence
is legible rather than blank.

---

## 4. The readings — every one predeclared in AMENDMENT 4 before the code existed

| | prediction | reading | |
|---|---|---|---|
| **P1** | all 17 existing selftest cases + 7 new V7 controls OK | **24/24, `SELFTEST: OK`**, no `*** BROKEN ***` | ✅ |
| **P2** | six pinned bench legs `PASS 4`, `tau 0.0040`, every edge dominant, `m_edge ≈ 0.0005` | **`PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / PARTIAL 0 / NOT-MEASURABLE 0`**, `VERDICT PASS (4 of 4 events fully checked)`, `tau = 0.0040`, exit 0, on all six | ✅ |
| **P3** | cohort tables both guard settings; ≥1 wrong cell on `A2L_LEGA` with the guard off | **780 cells; guard off 376/19/385; guard on 52/0/728**; `A2L_LEGA` contributes 2 of the 19 | ✅ |
| **P4** | Codex's three aligned fixtures read `PASS` or `NOT-MEASURABLE` only | **`NOT-MEASURABLE` on all three**, with the guard-disabled column reproducing Codex's own `ONSET-SHIFT(+1) ×2` / `NOT-VISIBLE ×1` side by side | ✅ |
| **P5** | `M50L_LG9`, `A1L_LEGA`, `LYRA_SMOKE_01`: zero NOT-VISIBLE, zero SHIFT | **0 / 0 on all three**, `VERDICT UNREAD`; `A2L_LEGA` `PASS-PARTIAL (2 fully, 0 partly, 5 unread of 7)` | ✅ |
| **P6** | batch collision + exit contract | **all 12 checks OK** | ✅ |

### 4.1 P1 — the selftest, both halves

**The 17 pre-existing cases: NOT ONE VERDICT TOKEN MOVED**, exactly as predicted. `blank_region` and
`moving_blank_region` still read `NOT-VISIBLE`, so the verdict was **scoped, not disabled**.
`moving_over_cap`'s printed REASON changed from `camera motion` to `regional image change`; its token
did not.

**The seven new controls, ported from `_reviews/079-05-review-validation.py` with attribution:**

```
codex_quiet_prefix                 NOT-MEASURABLE|PASS             NOT-MEASURABLE    OK
codex_small_region                 NOT-MEASURABLE|PASS             NOT-MEASURABLE    OK
codex_alternating                  NOT-MEASURABLE|PASS             NOT-MEASURABLE    OK
codex_quiet_prefix_shift1          NOT-MEASURABLE|ONSET-SHIFT(-1)  NOT-MEASURABLE    OK
codex_small_region_shift1          NOT-MEASURABLE|ONSET-SHIFT(-1)  NOT-MEASURABLE    OK
codex_quiet_prefix_noburst_shift1  ONSET-SHIFT(-1)                 ONSET-SHIFT(-1)   OK
codex_small_region_noburst_shift1  ONSET-SHIFT(-1)                 ONSET-SHIFT(-1)   OK
```

🔑 **The last two are what stops this being a suite that passes by refusing everything.** Same
regional motion, hijack burst removed so the true edge dominates, guard off — and the gate still
reports the injected shift with the right sign (`G96`).

### 4.2 P4 — the counterexamples, before and after, in one reading

| fixture | whole-frame `M_med` | `m_edge` | guard DISABLED | SHIPPED |
|---|---:|---:|---|---|
| `codex_quiet_prefix` | 0.1250 | **0.1200** | `ONSET-SHIFT(+1)` on a CORRECT label | `NOT-MEASURABLE` |
| `codex_small_region` | **0.0039** | **0.1200** | `ONSET-SHIFT(+1)` on a CORRECT label | `NOT-MEASURABLE` |
| `codex_alternating` | 0.0059 | **0.1200** | `NOT-VISIBLE` on a CORRECT label | `NOT-MEASURABLE` |

**Row 2 is the whole argument in one line: 0.0039 whole-frame against 0.1200 regionally, on the same
picture.** The old constant was 0.040 — ten times the whole-frame number, so lowering it would have
changed nothing for this fixture. **The quantity was wrong, not the threshold.**

### 4.3 P5 — the moving sessions, and what `PARTIAL` buys

```
A2L_LEGA       PASS 2  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 5   VERDICT PASS-PARTIAL (2 fully, 0 partly, 5 unread of 7)
LYRA_SMOKE_01  PASS 0  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 6   VERDICT UNREAD (0 of 6 events checked)
A1L_LEGA       PASS 0  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 6   VERDICT UNREAD (0 of 6 events checked)
M50L_LG9       PASS 0  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 6   VERDICT UNREAD (0 of 6 events checked)
```

⚠ **`A2L_LEGA` was `PASS 6 / NOT-MEASURABLE 1` at 079-04 and is now `PASS 2 / NOT-MEASURABLE 5`.**
AMENDMENT 4 declared that in advance and declared it **not a regression**: four of those events sit on
regions changing 20–35× the cap, and the 079-04 reading was a session-level cap waving them through.
🚨 **Three sessions now print `VERDICT UNREAD` where they used to print `VERDICT PASS`.** That is
finding 3's aggregate-verdict correction doing exactly its job — the old line said `PASS` after
reading nothing at all.

`M50L_LG9` still exhibits `G259` in the raw: `tau = 1.0950` and `1.1938` on two events, now refused
**per edge** as `threshold unsatisfiable` before any verdict is formed.

---

## 5. Honest limits, stated rather than discovered later

1. **`REGION_CAP` is pinned by ONE EDGE of ONE EVENT** (`A1L_LEGA` ev0 run[4,5] onset,
   `m_edge = 0.000145`), and the margin between it and the cap is **3.4 %**. 079-04 was criticised
   for a cap resting on a single value; **this one rests on a single value too.** The difference is
   that it now rests on a *disagreement* (a cap below which nothing was wrong) rather than on an
   *agreement* (a level at which something looked right), which is the safer direction — but it is
   thin and it will move when more sessions exist.
2. **A small `m_edge` is NECESSARY, NOT SUFFICIENT.** The cell that sets the cap inverted the sign of
   an injected shift while its region looked quiet. Below the cap the tool is not *proven* right; it
   is *not yet observed wrong* on 736 edges.
3. **The tool is now silent on three of four real moving-camera sessions** and reads 4 of 16 cells on
   the fourth. Rule (a) — never call a correct label NOT-VISIBLE — holds. **Rule (b) — be useful on
   real footage — is further from holding than it was**, and that is a consequence of removing a
   false permission, not a new defect.
4. **`DOMINANCE = 1.5` was declared before implementation and is not derived from anything.** Its
   justification is an argument (a competitor within two thirds of the winner is a coin flip), not a
   measurement. It refused 2–8 cells per perturbation on `A2L_LEGA` with the guard off — visible in
   the reason histograms — and its effect at the shipped cap is small because the regional rule fires
   first on almost everything.
5. **What this evidence still does not establish** (restated from Codex Q2/Q3, not quietly dropped):
   that the pixel edge the gate picks on an *unshifted* session is the correct anomaly edge; that any
   original real label was right; sensitivity to smaller anomalies; freedom from false positives on
   content this bank does not contain.
6. **Nothing here measures the owner's M2 field captures.** `G259`'s dated append now says so: the
   home reproduction explains **two banked unsatisfiable events and a mechanism class**, not the field
   readings. **The office re-run is still owed.**
7. **The coherent variant's scope:** it moves the per-frame payload of the **annotated** frames only.
   For hide-type anomalies `labels.jsonl` also carries entries on the un-hidden in-between frames
   (`m44`'s rule), and those are left where they are. Stated because it bounds what the variant tests.

---

## 6. What was NOT done

- ⛔ **No merge, no push to `master` or `m51`, no tag.**
- ⛔ **No ring subtraction, no provenance veto, no new physical campaign** — Codex explicitly
  recommended none of these follow from the findings, and none was attempted.
- ⛔ **`docs/office-rdp-card.md`'s PRE-DECLARED READING was NOT edited.** It was written on 2026-09-04
  against an older build of this checker; whether it still applies is exactly what a run would decide,
  and pre-editing it to match a newer tool is the laundering shape. The card gains a **dated
  prospective correction** for its three genuinely stale items only.
- ⛔ **The office M2 re-run is still OWED.** Nothing here measures the owner's field captures, and
  `G259`'s dated append now says so in as many words.
- ⛔ **`_strip_comments.py` was NOT run in `CaptureBench`** (`G261`). That repo keeps its comments.
