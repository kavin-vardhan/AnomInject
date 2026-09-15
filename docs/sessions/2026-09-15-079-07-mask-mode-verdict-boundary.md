# 2026-09-15 — 079-07 — a verdict needs target attribution, so masks or nothing

**Branch `fix/verifier-field-validity`, four commits on top of `bff1b00`. ⛔ NOT MERGED.**
`master` and `m51` untouched. `Source/`, `Shaders/` and `tools/measure_label_offset.py`
byte-identical to `master`. No build, capture, cook or tag.

| commit | what |
|---|---|
| `0e7d96c` | **AMENDMENT 5**, written and committed **before a line of the correction existed** |
| `d7f4e40` | the fix — `tools/verify_capture.py`, +782/−160 |
| `0a87825` | docs: `client-readme.md` Step 6, Section G, `G263`, the two appended prediction corrections |
| *(this)* | the journal |

Scorer `m079_shift_recovery.py` → **v2.1**, still untracked in CaptureBench, which stays
tracked-clean at `472a409`.

---

## 1. What this session is

Codex's **second** independent merge review of the verifier returned CHANGES REQUIRED on eight
findings; chat accepted all eight and re-scoped the first. This session implements that, measures
it, and hands it back. It is the fourth round of one workstream, and the interesting thing about it
is that **the first three all reached for a threshold and this one stopped doing that.**

---

## 2. The finding that mattered, and why no number could have fixed it

Codex built the minimal case. A 160×120 picture, static except a 50×50 window onto a scrollable
crop. A white 20×20 patch present on frames **60..67** — the real anomaly. And **one** unrelated
10 px background scroll at frame **61**, inside the labelled window. Every clean baseline pair
unchanged.

| labels | required | `bff1b00` read |
|---|---|---|
| 60..67, **correct** | alignment or refusal | **`ONSET-SHIFT(+1)`, FAIL, exit 2** |
| 61..67, one frame **late** | `ONSET-SHIFT(-1)` or refusal | **`PASS`, exit 0** |

I rebuilt it from their construction and confirmed both before writing anything. The burst changes
**0.4960** of the region against the real onset's **0.1600** and wins by **3.1×**.

🔑 **Every guard in that gate was satisfied.** The learned `tau` asks *is this bigger than the
region's own noise* — it is, hugely. `m_edge` asks *is this region still when nothing is happening*
— it is, **exactly zero**. `DOMINANCE` asks *was the contest close* — it was not, 3.1×. All three
pass and the answer is still wrong, and I confirmed Codex's other reading too: **the false shift
survives at `REGION_CAP = 0`.**

**Because the missing quantity is not a magnitude.** Those are all SEPARATION tests, and the
question is IDENTITY. A frame-difference gate can say THAT something changed; inside a bounding box
it cannot say WHAT changed, because the box contains the target and the floor behind it and whatever
the camera swept past.

🔻 **So the `_edge_search` docstring's claim that dominance is *"THE DIRECT CURE FOR SOMETHING ELSE
WON THE ARGMAX"* is WITHDRAWN in the source** — replaced by a paragraph stating what dominance does
and does not decide, with the fixture's numbers in it.

### 2.1 The fix is a boundary, not a tolerance

**Verdicts are given only in MASK MODE**, where every edge of the run took its region from the
delivered per-frame mask. A bbox-only run prints `READING` lines carrying every number it measured
and the session ends `UNREAD-BBOX-ONLY` at **exit 0**. Exit 2 is now reachable only from a mask-mode
`SHIFT` or `NOT-VISIBLE`.

**No new region code was written.** `_build_region` has preferred the mask over the bbox since m49;
what changed is what the gate is allowed to CONCLUDE from a region that is not one.

**The cost, measured before it was designed rather than discovered after:**

| session | edges | mask-attributed | consequence |
|---|---:|---:|---|
| six pinned `M49_GEDGE_*` | **64** | 64 | untouched |
| `A2L_LEGA` | 16 | 16 | untouched |
| `LYRA_SMOKE_01` | 16 | 16 | untouched |
| `A1L_LEGA` | 12 | 12 | untouched |
| `M50L_LG9` | 16 | **0** | **leaves the verdict surface** |

⚠ **64, not the 56 my 079-06 report stated.** Codex's correction is right and is now in the source,
the header and the amendment.

**No run anywhere in that set is mixed**, which is what made the boundary affordable — and also why
the mixed case needed a manufactured fixture rather than a reading (§4.2).

---

## 3. The other seven, each with the reading that proves it

**F2 — constraints survive refusal.** Both of Codex's reproductions confirmed on the old bytes:
a whole-frame onset box refused before searching left the end unbounded, which re-found the onset
transition and returned **`END-SHIFT(-2)`** on aligned labels; and a first end that was refused
*or correctly read* still released the second run's onset, which re-used the first run's clear and
returned **`ONSET-SHIFT(-2)`** on an aligned onset. The search now runs before every refusal that
can still be searched, at `SIGNAL_FLOOR` where `tau` is unusable, marked `bound_only` — and by
construction every `bound_only` path ends in a refusal, so it can never become an answer.

🔑 **One deviation from the brief's wording, declared in AMENDMENT 5 before implementation.** The
brief said `max(previous labelled edge, previous DETECTED edge)`; I implemented its own
parenthetical, **detected-if-found-else-labelled**. The `max` form is unsound in exactly the case
the gate exists for: **on a one-frame run whose labels are LATE by one, the true clear sits at the
labelled onset frame itself**, so a labelled floor of `rs + 1` excludes the correct answer even when
the onset was detected correctly at `rs − 1`.

**F3 — coverage before judgement.** An empty search set `status="read"` and reported *"0 of 0 frames
above tau"*, and two of those combined into `NOT-VISIBLE` and a FAIL. An edge is now read only if
every frame pair of its **FINAL** window was scannable.

⚠ **"FINAL" is load-bearing and was measured first.** The banked edges scan **3–8** pairs of a
nominal 9 — because the window is narrowed by the run's own extent, the ordering bounds, the
foreign-event exclusion and the session's ends. **Applying coverage to the nominal ±4 window would
have refused every edge in the bank, including all 64 pinned-bench edges.** Missing files and
foreign exclusions are counted separately; only the first refuses.

**F4 — the denominator.** `shadow()` returned `None` for failure **and the destination path for
success**, so an unsupported perturbation skipped a whole session/variant. The sense is now
inverted — `None` means success, and every failure carries a reason — and the cohort is published
before scoring. Codex's fixture: **26 cells, 8 unsupported.**

**F5 — no-signal is not a refusal.** Five classes now. Re-classifying the banked 079-06 table
offline reproduces Codex exactly: guard-off **385 refusals = 255 real + 130 no-signal**, guard-on
728 with **0**. 🆕 And **118 of those 130 are `M50L_LG9`**, so F1's boundary removes them by
construction rather than by re-labelling. The FAIL line becomes **`labels not confirmed by
pixels`** — a single changed pixel in a 50×50 mask is `d = 0.0004` against `tau = 0.0040`, so
`NOT-VISIBLE` is a detection limit and the old wording claimed more than it could carry.

**F6 — the zero boundary.** `floor_2sf(0)` returned a bare float where the caller unpacked a pair,
so the exact boundary the procedure exists for crashed. Fixed; and the second half matters more:
`NO ADMISSIBLE ENVELOPE` is now a state the gate accepts and honours, and a numeric `0.0` is not
refuse-all. **Proven both directions on a pinned leg where every `m_edge` is 0.000000:** cap `0.0`
→ `PASS 4`; sentinel → `NOT-MEASURABLE 4`.

**F7 — coverage aggregation.** The event token comes from coverage over all its edges, not the worst
run. A `SHIFT` still wins outright, deliberately: it is a positive detection on an attributed,
fully-observed, dominant edge and stands on its own, whereas `PASS` and `NOT-VISIBLE` are claims
about the whole event. **AMENDMENT 4's V5 severity rule is corrected by a dated AMENDMENT 5, not
rewritten in place.**

**F8 — collision-safe ids.** `__` substitution cannot be injective when `__` is legal inside a path
component. The id gains 8 hex of `sha1(relative path)` — and because a hash is an argument about
likelihood rather than a proof, **uniqueness is asserted across the batch before anything is
written**. Codex's collision now retains both reports with their opposite verdicts and each line
names its own path.

---

## 4. Two things I got wrong, both caught by the fixtures

### 4.1 🔻 My predeclared expectation for one case forbade the fixture's own known answer

`c07_previous_end_control_masked` was declared *"PASS / PARTIAL / NOT-MEASURABLE — never a SHIFT"*.
It reads **`END-SHIFT(+1)`**, and that is **correct**: the fixture's RGB carries its first patch on
10..12 while its labels claim 10..11, so its first end really is one frame early — and Codex's own
prediction for it says *"first end +1, second onset 0"*. **A defect in the PRE-DECLARATION, not in
the build**, the same shape as journal 073's `A1-ONSET-JOIN` conjunct. Corrected by appending
(**A5.4**); the table was not edited.

🔑 **And the repair is a strengthening, because the event TOKEN could not have answered the question
that case exists to ask.** `_event_token` returns the first run's shift, so `END-SHIFT(+1)` would
have printed identically whether the SECOND run's onset was right or wrong — `G262`'s exact shape,
a token hiding a per-unit failure, **inside the correction for it**. The five F1/F2 cases are now
judged on per-edge offsets:

| case | per-edge | before |
|---|---|---|
| `c07_burst_aligned_masked` | `+0, +0` | — |
| `c07_burst_late_masked` | `-1, +0` | — |
| `c07_onset_early_refusal_masked` | `refused, +0` | end read **−2** |
| `c07_previous_end_refusal_masked` | `+0, refused, +0, +0` | second onset **−2** |
| `c07_previous_end_control_masked` | `+0, +1, +0, +0` | second onset **−2** |

**The constraint removed an answer only where the answer was unfounded.** The fixture's genuine `+1`
survives.

### 4.2 A latent inconsistency, fixed before any result was read

`_event_token` counted **attributed EDGES** individually while mask mode is a property of the
**RUN**, so a run with one masked and one unmasked edge could have contributed a verdict the run
itself had refused to give. No banked session exercises that path — which is why it needed a
fixture. `c07_mixed_runs` (masks on the first run only) was predeclared **`PARTIAL`** at **A5.5**
before it existed; it reads `PARTIAL`.

---

## 5. The readings

**P1 — `SELFTEST: OK`, 46 cases.** The seventeen synthetic ones gain masks, because under R1/R2 a
bbox-only fixture yields readings and **a suite of readings cannot prove the gate is still able to
FAIL** (`G96`). Their tokens do not move. Five 079-05 Codex fixtures become `READING`, with two new
**masked** twins carrying their negative-control role. Eighteen new 079-07 cases. Plus the
real-session half against a banked leg: `real+1` → `ONSET-SHIFT(-1)` on **4 of 4** events,
`real-1` → `ONSET-SHIFT(+1)` on **4 of 4**.

**P2 — the pinned bench is unmoved.** All six legs
`PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / PARTIAL 0 / NOT-MEASURABLE 0 / READINGS 0`,
`VERDICT PASS (4 of 4 events fully checked)`, `tau = 0.0040` on **all 64 edges**, every one
mask-attributed at `m_edge = 0.000000`, exit 0.

**P3 — the bank. Zero `SHIFT` and zero `NOT-VISIBLE` on all four.**

| session | counts | verdict |
|---|---|---|
| `A2L_LEGA` | PASS 2 · NM 5 | `PASS-PARTIAL (2 fully, 0 partly, 5 unread of 7 events …)` |
| `LYRA_SMOKE_01` | NM 6 | `UNREAD (0 of 6 events checked …)` |
| `A1L_LEGA` | NM 6 | `UNREAD (0 of 6 events checked …)` |
| `M50L_LG9` | **READINGS 6** | **`UNREAD-BBOX-ONLY (6 events; readings printed …)`**, exit 0 |

**P4 — scorer v2.1 on the mask-mode cohort.** 44 original run-edges × 13 combinations = **572
planned, 572 scored.**

| guard | recovered | wrong | refused | no-signal | unsupported |
|---|---:|---:|---:|---:|---:|
| OFF | 368 (64.3 %) | **17** | 175 | 12 | 0 |
| ON at `0.00014` | 52 (9.1 %) | **0** | 520 | 0 | 0 |

**`REGION_CAP` re-derives to `0.00014000` — unchanged — pinned by the cell Codex named**:
`A1L_LEGA` event 0, run[4..5], onset, annotation-only both +1, expected −1, **read +1**,
`m_edge = 0.00014459`. **Retained positive support: 4 original edges, `A2L_LEGA` events 4 and 5 —
which the scorer now prints itself** rather than leaving to a reviewer to reconstruct.
`REGION_CAP_N` is corrected 736 → 572 and the header says what N is: perturbation cells that reached
a baseline, not independent edges and not validations.

**P5 — batch.** Collision fixture retains two reports with distinguishable lines; missing-label under
`--report-only` → 3; healthy → 0; failing without `--report-only` → 2.

---

## 6. What this does not establish

- **The cap no longer licenses a verdict, and it never should have.** A still baseline says nothing
  about whether an in-window change is ATTRIBUTABLE — the F1 fixture has `m_edge = 0` at both edges
  and still had its onset hijacked. Masks license the verdict; the cap only decides whether a
  mask-mode edge is worth reading.
- Agreement with an injected offset is a **perturbation test**. It does not establish that the
  original label was right, that the selected RGB edge was the anomaly's, or anything about
  sensitivity or false positives on content this bank does not contain.
- **52 / 0 / 520 is a statement about three banked Lyra sessions**, and `REGION_CAP` still says
  `PROVISIONAL HEURISTIC`.
- ⚠ **A consequence for Section G, now on the card: if the M2 Concorde captures carry no
  `target_mask` folder, that step cannot reach its pre-declared reading at all** — it prints readings
  and stops. That is a result to transcribe, not a pass.
- 📌 **Filed, not acted on:** the comment stripper would change `tools/verify_capture.py` (−472 bytes
  of `#` comments; docstrings survive), so it has evidently not been run on this file across the 079
  series. Running it now would delete explanatory comments that 079-03..06 added and that Codex's
  reviews cite by line. **My own new code uses docstrings only**, so it neither adds to the
  inconsistency nor destroys anything. Chat's call.

---

## 7. Evidence and state

`_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-07\` — 17 files: `P1_selftest.txt`, ten
`gate_<session>.txt`, `P4_scorer_v21.txt` + `cohort_v21.json` (572 cells), `F5_079-06_resplit.txt`,
`P5_batch.txt`, `P2P3_summary.txt`, `m079_shift_recovery_v21.py`.

Codex's answer, finding by finding, is `_reviews\079-07-code-response-to-codex.md`.
New gotcha **`G263`**. ⛔ No client, Section G, campaign or `m51` hold is released by any of this.

**Next: Chat's checkpoint, then whatever independent re-review Chat commissions. ⛔ Do not merge.**
