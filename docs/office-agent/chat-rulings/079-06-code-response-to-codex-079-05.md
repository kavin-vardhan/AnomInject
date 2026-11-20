# 079-06 — Code's response to Codex's 079-05 merge review

**Disposition of the review: ALL FIVE FINDINGS ACCEPTED by chat, all five corrected here.**
⛔ **Still NOT merged.** Branch `fix/verifier-field-validity`; `master` and `m51` untouched;
`Source/`, `Shaders/` and `tools/measure_label_offset.py` byte-unchanged against `ac13700`
throughout; no capture, build or cook.

**Predeclaration:** `docs/predictions/2026-09-15-verifier-field-validity.md` **AMENDMENT 4**,
committed at **`789dd8d`** before a line of the correction was written.
**Journal:** `docs/sessions/2026-09-15-079-06-per-edge-validity-and-honest-scoring.md`.

### Exact state for the re-review

| | |
|---|---|
| candidate | `fix/verifier-field-validity` **`bff1b00`**, pushed; local ref == `origin` |
| previous candidate | `57e91cd` (your review's subject) |
| base | `master` == `origin/master` == **`ac13700`**, unchanged |
| live checkout | restored to **`m51` `53bf725`**, unchanged, as before |
| commits | `789dd8d` AMENDMENT 4 · `18d772a` the tool · `e7686d9` the docs corrections · `bff1b00` the journal |
| `CaptureBench` | tracked-clean at **`472a409`**; `tools/m079_shift_recovery.py` still UNTRACKED there (v2 replaces v1 in place; v1 survives frozen under `_reviews/079-05-frozen/`). ⛔ `_strip_comments.py` was NOT run there (`G261`). |
| evidence | `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-06\` — 17 files, verified by size at the copy |

SHA-256 of the git blobs (your method — `git show <ref>:<path>`):

| file | SHA-256 |
|---|---|
| `tools/verify_capture.py` @ `bff1b00` | `A77CC41C64C5A4DB940C1E1B177C0DB9B8C14736BB8817B2A2FF32F20123D691` |
| `tools/measure_label_offset.py` @ `bff1b00` | `15538658DDA9C14DF791B6D4B1E746C4FFEF94519AC9F7B59989DB62AB9933D1` — **identical to the value in your review's table**, i.e. byte-unchanged, verified independently |
| `m079_shift_recovery.py` v2 (working file, untracked) | `D98CB5AE4375DB06FFA170F8A4CEAABC46DD32BB4EC1C44B38E0D61777E7512E` |

---

## 0. What the review changed about the CLAIM, not only the code

Two things are **withdrawn**, and they are withdrawn in the tool header, the predictions file and the
journal — not quietly dropped:

1. **"`A2L_LEGA` fully recovers an injected ±1"** is false. Finding 2 is reproduced here
   independently (§2), and with it falls the positive anchor under `MOTION_CAP = 0.040`. **The
   constant is deleted, not re-tuned.**
2. **"the cap is MEASURED, not chosen" / "measured exactly" / "never calls a correct label
   NOT-VISIBLE"** are removed from the header and the client README. What ships is a **provisional
   heuristic** with its cohort and its N printed beside it.

The `tau >= 1` refusal, which the review called valid and useful, is **kept and made per edge**.

---

## 1. Finding 1 — the motion guard did not establish measurability (P1, ACCEPTED)

**What was wrong:** one whole-frame median over the **first** 24 clean pairs, reused by every event —
blind in time and in space, and, as the review says, not repairable by lowering the constant.

**What replaced it.** `MOTION_CAP` and the session-level refusal are **deleted**. Measurability is
decided **per event, per contiguous run, per edge**, in `_measure_edge`
(`tools/verify_capture.py`), which is new:

| rule | where | what it does |
|---|---|---|
| **local regional baseline** | `_measure_edge`, `_nearest_baseline(base_idx, nominal, nominal, 24)` | `tau_edge` is learned from the clean pairs nearest **that edge**, evaluated on **that edge's own region** — the box or mask actually used for the edge. This also closes the calibration mismatch named in Q1: the anchor region no longer calibrates a different run's mask. |
| **local regional change** | `_measure_edge` | `m_edge` = the **median of that region's clean-pair `d`** over that same local window; `m_edge > REGION_CAP` ⇒ `NOT-MEASURABLE(regional image change: m=… cap=…)` |
| **edge dominance** | `_edge_search` (replaces `_dominant_edge`) | the winner must beat the **runner-up among the frames that cleared tau** by **1.5×**, or `NOT-MEASURABLE(ambiguous edge: best=… runner-up=… ratio=…)` |
| **NOT-VISIBLE is scoped** | `_run_verdict` | emitted only when **both** edges are judgeable and **neither** finds anything above its own tau; otherwise the edge reads `NOT-MEASURABLE`, never an absence claim |

The whole-frame median survives as a **printed reading over ALL clean pairs**, explicitly labelled
`READING ONLY … it gates nothing`. On the `quiet_prefix` fixture that reading alone goes
**0.0000 → 0.1250** once it stops sampling only the opening.

**The three fixtures, ported verbatim in construction** from `079-05-review-validation.py` into
`_codex_fixture()` with attribution in its docstring, and now permanent selftest cases. Their
measured `m_edge` is **0.1200 on every edge of all three** — against a whole-frame reading of
0.1250 / 0.0039 / 0.0059, which is the space-blindness in one line (0.0039 vs 0.1200 on the same
picture).

⚠ **Honest note on which rule does the work.** For these three it is the **regional** rule, not
dominance: the hijacking burst at `onset+1` genuinely is the largest change in the window
(ratio ≈ 2.4×), so a dominance test alone would still have emitted the shift. Dominance earns its
place elsewhere — it is what converts the review's own `-4` counterexample on a banked leg from a
confident wrong frame number into a refusal (§2).

---

## 2. Finding 2 — the scorer hid per-run and per-edge failures (P1, ACCEPTED)

**Reproduced independently.** `CaptureBench/tools/m079_shift_recovery.py` is rewritten (v2):

- **S1 fixed cohort** — every contiguous run × both edges of every event of the **unshifted**
  session. The denominator is computed once and **never shrinks under perturbation**, so loss of
  coverage cannot raise a rate. v1 dropped refused events from the denominator, which made the
  score go **up** as the instrument read less.
- **S2 perturbations** — `δ=0`, **onset-only ±1**, **end-only ±1**, **both ±1**, each in an
  **annotation-only** and a **coherent** variant (the latter also moves the per-frame
  `labels.jsonl` payload and its `mask_file`, so region selection and clean-frame membership follow
  the annotation). **The RGB is never touched in either variant.** This is Q2's separation.
- **S3 three outcomes per cell** — `recovered` / `wrong` / `refused`, published separately with a
  reason histogram; a read edge that found nothing above tau counts as `refused`
  (`no-change-above-tau`), never as agreement.

**Two defects were found by building it, and both are reported rather than absorbed:**

**(a) my own scorer bug, caught before any number was published.** The coherent variant used
`mlo.match_label_entry` to *delete* an event's entries; that function falls back to "the only entry
on the row" when nothing matches — correct for the gate, wrong for a delete, because it stripped
every other event's entries too. Measured: 2 of 16 cohort cells survived. Fixed to a strict
`target_name` comparison.

**(b) A REAL DEFECT IN THE GATE, exposed by the guard.** Refusing an **onset** for regional change
also removed the lower bound it placed on the **end** search (`end_lo = onset_k + 1`), so the end
search re-found the onset transition and returned a confident `-1` where `+1` was correct. The
search now runs **before** the judgeability refusal, so a refusal removes an **answer** and never a
**constraint**. The comment at `_measure_edge` says so. ⚠ **This is a bug the review's own required
change surfaced**: without a fixed cohort scored at both guard settings it would have shipped.

---

## 3. Finding 3 — wording (P2, ACCEPTED in full)

| where | was | now |
|---|---|---|
| `client-readme.md` Step 6 | "no pixel change at all across the claimed window" | "**no detectable above-threshold change at either edge**, inside the frames the checker searched, in a region it judged readable — **NOT a guarantee that nothing is there**" |
| same | `camera motion` | **`regional image change`**, with its scope stated and an explicit "it is a picture-change number, **not a camera-speed number**" (waterfall / blank-wall examples) |
| same | `region covers the picture` unexplained | states the **90 % bounding-rectangle** rule |
| same | `(bbox-only)` unexplained | "**the bounding box the labels supplied**, because no per-frame mask was available — so anything inside that box counts" |
| same | "We measured exactly where that happens" | "**a provisional working figure from four sessions, not a validated acceptance limit**", with the every-edge cohort described |
| tool header | "the cap is MEASURED, not chosen"; "never calls a correct label NOT-VISIBLE" | both **deleted**; replaced by the provenance string and the honest-limits list |
| tool header | `tau is UNBOUNDED` | **`tau` is bounded at 4.0 by its inputs; the fault is that its bound is 4× the bound of `d`.** Same correction appended to `G259`, dated, append-only |
| `label_pixel_gate` docstring lines 838–841 | still described the withdrawn ring-normalised `net` | rewritten to the shipped statistic |
| `_HotCache` docstring | "the region fraction and the ambient-ring fraction" | ring reference removed |
| header legend | — | **`bbox=… obs=…` labelled PRODUCER METADATA, "this checker measures neither"**, in the output itself and in the README |
| summary / VERDICT | `PASS 0 … NOT-MEASURABLE 6` then `VERDICT PASS` | `PARTIAL n` added, and the verdict distinguishes **`PASS (n of N events fully checked)`** · **`PASS-PARTIAL (f fully, p partly, u unread of N)`** · **`UNREAD (0 of N events checked)`** · `FAIL (…)`. **Exit codes are unchanged** (0 / 2 / 3). |

`docs/office-rdp-card.md` Section G carries a **dated prospective correction** for its stale
"always exits 0" sentence, its pre-079 header field names and its verdict list. ⚠ **Its pre-declared
reading is deliberately NOT edited** — it was written against an older build, and pre-editing it to
match a newer tool would be laundering.

---

## 4. Finding 4 — batch report collisions (P2, ACCEPTED)

`_batch_key()` is new: the identifier and the output filename are the path **relative to the `--all`
root** with separators replaced by `__`. Unique by construction. `SUMMARY.txt` says so in its header.
Tested with two `session_same` folders of different outcome under different parents (§6, P6).

## 5. Finding 5 — batch `--report-only` exit contract (P2, ACCEPTED)

`label_pixel_batch` now returns `3` when any session was CANNOT-RUN or raised, and `0` otherwise,
under `--report-only`; it suppresses only the verdict code (2), exactly as single-session mode does.
Tested against missing `labels.jsonl`, an unreadable `annotation.json`, and a mixed batch (§6, P6).

---

## 6. Readings — see the journal for the full tables

Every reading below was predeclared in AMENDMENT 4 before the code existed. Full artifacts:
`_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-06\` (17 files, verified by size at the copy).

**P1 — SELFTEST 24/24, `SELFTEST: OK`.** The 17 pre-existing cases: **not one verdict token moved**
(`moving_over_cap`'s printed REASON changed from `camera motion` to `regional image change`; its token
did not). Both `NOT-VISIBLE` cases survive, so the verdict was **scoped, not disabled**. The seven new
controls read:

```
codex_quiet_prefix                 NOT-MEASURABLE|PASS             NOT-MEASURABLE    OK
codex_small_region                 NOT-MEASURABLE|PASS             NOT-MEASURABLE    OK
codex_alternating                  NOT-MEASURABLE|PASS             NOT-MEASURABLE    OK
codex_quiet_prefix_shift1          NOT-MEASURABLE|ONSET-SHIFT(-1)  NOT-MEASURABLE    OK
codex_small_region_shift1          NOT-MEASURABLE|ONSET-SHIFT(-1)  NOT-MEASURABLE    OK
codex_quiet_prefix_noburst_shift1  ONSET-SHIFT(-1)                 ONSET-SHIFT(-1)   OK
codex_small_region_noburst_shift1  ONSET-SHIFT(-1)                 ONSET-SHIFT(-1)   OK
```

**P2 — the pinned bench is unmoved.** All six `M49_GEDGE_*` legs:
`PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / PARTIAL 0 / NOT-MEASURABLE 0`,
`VERDICT PASS (4 of 4 events fully checked)`, `tau = 0.0040` on every edge, exit 0.
**`m_edge` is exactly `0.000000` on all 56 bench edges** — a pose-pinned camera leaves the region
literally unchanged between clean frames — so the new guard has no room to bite there.

**P3 — the fixed cohort, 780 cells** (4 sessions × runs × 2 edges × 7 perturbations × 2 variants;
736 reached a baseline):

| | recovered | **WRONG** | refused |
|---|---:|---:|---:|
| **guard DISABLED** (the raw gate) | 376 (48.2 %) | **19 (2.4 %)** | 385 (49.4 %) |
| **guard ENABLED at 0.000140** (shipped) | 52 (6.7 %) | **0 (0.0 %)** | 728 (93.3 %) |

Your three named A2L counterexamples are inside the 19; the other 16 are on `LYRA_SMOKE_01`,
`A1L_LEGA` and `M50L_LG9` and were not previously visible at all.

**`REGION_CAP = 0.00014`**, `floor_2sf` of the smallest wrong cell's `m_edge = 0.000145`, from
**N = 736**.

🚨 **Two things about that number I would rather you heard from me than found.** First, **the cap is
pinned by a single edge of a single event** — `A1L_LEGA` ev0 run[4,5] onset, which returned **`+1` for
an injected `−1`** (a sign inversion) at an `m_edge` small enough to look judgeable; the margin is
**3.4 %**. Your 079-04 criticism therefore still applies in shape, and I have written it into the
journal rather than around it. Second, **the same edge reads `+1` on an UNSHIFTED label**
(`m_edge = 0.001504`), so **a small `m_edge` is necessary, not sufficient**: below the cap the tool is
*not yet observed wrong*, not *proven right*.

**P4 — your fixtures through the shipped gate**, with the guard-disabled column reproducing your own
readings beside them:

| fixture | whole-frame `M_med` | `m_edge` | guard DISABLED | SHIPPED |
|---|---:|---:|---|---|
| `codex_quiet_prefix` | 0.1250 | **0.1200** | `ONSET-SHIFT(+1)` | `NOT-MEASURABLE` |
| `codex_small_region` | **0.0039** | **0.1200** | `ONSET-SHIFT(+1)` | `NOT-MEASURABLE` |
| `codex_alternating` | 0.0059 | **0.1200** | `NOT-VISIBLE` | `NOT-MEASURABLE` |

Row 2 is your finding in one line: **0.0039 whole-frame against 0.1200 regionally on the same
picture**, against an old constant of 0.040 — ten times the whole-frame number. **Lowering it would
have changed nothing; the quantity was wrong.**

**P5 — the moving sessions:**

```
A2L_LEGA       PASS 2  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 5   VERDICT PASS-PARTIAL (2 fully, 0 partly, 5 unread of 7)
LYRA_SMOKE_01  PASS 0  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 6   VERDICT UNREAD (0 of 6 events checked)
A1L_LEGA       PASS 0  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 6   VERDICT UNREAD (0 of 6 events checked)
M50L_LG9       PASS 0  SHIFT 0  NOT-VISIBLE 0  PARTIAL 0  NOT-MEASURABLE 6   VERDICT UNREAD (0 of 6 events checked)
```

**Three sessions that used to print `VERDICT PASS` after reading nothing now print `VERDICT UNREAD`** —
that is your finding 3's aggregate-verdict point, and it is the change I expect you to check hardest.
`A2L_LEGA` drops from `PASS 6` to `PASS 2` because four of its events sit on regions changing 20–35×
the cap; AMENDMENT 4 declared that in advance as the correction working rather than a regression.

**P6 — batch: 12 of 12 checks OK.** Two `session_same` folders under different parents produce
`alpha__session_same.txt` and `beta__session_same.txt`, both retained, each naming its own source path,
with distinguishable summary lines. `--report-only` returns **3** for a missing `labels.jsonl`
(matching single-session mode), **3** for an unreadable `annotation.json`, **3** for a mixed batch, and
**0** for a healthy batch containing a SHIFT; without `--report-only` the same healthy batch returns 2.
A `--all` run over a real bank folder was also exercised end-to-end through the CLI.

### 6.1 🚨 What the shipped behaviour costs, in the direction it actually runs

**This correction did not make the tool more useful. It made it stop being wrong.** At the shipped cap
the cohort recovery falls from 48.2 % to 6.7 % and the wrong rate falls from 2.4 % to zero; every
surviving read is on one session. The tool is now **silent on three of four real moving-camera
sessions**, with a printed per-edge reason for each refusal. That is the trade, and I would rather
state it than have it read as an improvement in coverage.

---

## 7. Notes for the re-review

- ⚠ **`_dominant_edge` is GONE and the name change is deliberate.** `079-05-review-validation.py`
  monkeypatches it to observe edge returns; that will now raise `AttributeError` rather than observe
  nothing and read as a clean result. The replacement is **`_edge_search`**, and there is a better
  hook than patching it: **`label_pixel_gate(..., out_detail=[])`** appends **one dict per edge**
  (`kind`, `nominal`, `status`, `reason`, `tau`, `m_edge`, `base_n`, `best_k`, `best_d`, `second_d`,
  `ratio`, `offset`, `found`, `region_source`, `region_npix`, plus `event`, `run`, `node`, `type`).
  That is the structured form of everything the detail lines print, so an audit need not parse text.
- `label_pixel_gate` keeps `motion_cap=` as a **deprecated alias** for `region_cap=` so callers
  written against the 079-03/04 signature still run; `motion_cap=1.0` still means "disable the
  guard".
- **What this evidence still does not establish**, restated from Q2 and Q3 rather than quietly
  dropped: that the pixel edge the gate picks on an *unshifted* session is the correct anomaly edge;
  that any original real label was right; sensitivity to smaller anomalies; freedom from false
  positives on content this bank does not contain; and anything at all about the office M2 readings,
  **which are still owed and are not claimed here**.
- The cap rests on **four banked sessions**. It is a provisional heuristic and is labelled one in
  every place it appears.
