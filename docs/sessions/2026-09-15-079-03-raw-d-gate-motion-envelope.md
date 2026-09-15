# 2026-09-15 — 079-03 — raw-`d` gate + a MEASURED camera-motion envelope

**VERDICT: GREEN.** P1–P7 all read as predeclared. **`tools/verify_capture.py` only;
`measure_label_offset.py` byte-unchanged; `Source/`, `Shaders/`, fixtures, schema, harness
untouched. No capture, build or cook.** Nothing merged — 079-04 merges after chat reads this and
Codex reviews at the merge gate.

**Predictions:** `docs/predictions/2026-09-15-verifier-field-validity.md` **AMENDMENT 2**, committed
**`8b1ee53`** *before* the revert and before any run.
**Evidence:** `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-03\` (14 files).
**New checker:** `Plugins/CaptureBench/tools/m079_shift_recovery.py`, SHA-256 `3BDC47D8678B3BEF…`.

---

## 1. 🚨 The brief's ground truth was CIRCULAR, and it was replaced rather than run

§1 asked for **MASK ONSET = the first frame in the event's span with `observable == true`**, as
ground truth the gate never sees. **It cannot disagree with the label, by construction:**

- **`m49` A1 DEFINES `annotation.affected_frames` as the OBSERVABLE SUBSET.** So the first in-span
  frame with `observable == true` is `min(affected_frames)` — the label's own onset.
- **Measured:** every in-span anomaly entry in `A1L_LEGA` and `A2L_LEGA` reads `observable: True`.
  The `False` entries exist (10 in `A1L_LEGA`) but sit **outside** the span, in
  `injected \ affected` — event 0 has `injected [4,5,9,10]` against `affected [4,5]`.
- **And `m44` writes masks only on frames labelled for that event**, so no mask evidence exists
  outside the span at all; a mask can never place an edge the label did not already claim.
- **Two of the four nominated sessions do not carry the keys**: `LYRA_SMOKE_01` and every
  `M49_GEDGE_*` leg have no `target_pixels` / `observable` (pre-`m49`-A1). Per §1's own rule,
  `LYRA_SMOKE_01` was **dropped, not substituted**.

⇒ Running it would have reported **100 % agreement while testing nothing** (`G146`'s vacuous-pass
shape). **Declared in AMENDMENT 2 before any measurement, with the replacement below.**

### 1.1 The replacement: INJECT the known answer

> **A gate is validated by whether it can RECOVER A KNOWN LABELLING ERROR — so we create the error
> ourselves.** Move every annotated window of a REAL session by a known `delta` (labels only; frames
> junctioned, never copied or written), and require the gate to report that shift back with the
> opposite sign.

Nothing about the answer comes from the artifact under test, so **the test can genuinely fail** — and
the envelope question becomes measurable: *at how much camera motion does the ability to see a
one-frame labelling error disappear?* That number is `MOTION_CAP`.

🔻 **A defect in my own checker, found and fixed before the verdict was read.** The first scoring pass
counted `NOT-VISIBLE` at `delta 0` as a success ("no unexpected shift"). **A NOT-VISIBLE on an
UNSHIFTED label is exactly the false failure this whole unit exists to remove.** It made
`M50L_LG9` read "3 decided, 3 recovered" when the truth was **3 of 3 correct labels called
NOT-VISIBLE**. Corrected to require `PASS` at `delta 0`, and both runs are banked.

---

## 2. The readings

### P1 — SELFTEST: **17/17 OK**

Seven original static cases unchanged; ten moving cases. All three refusals print distinguishably:

```
moving_blank_region       NOT-VISIBLE                  <- the gate can STILL fail under motion
moving_fullframe_region   NOT-MEASURABLE(region ...)
moving_unsat              NOT-MEASURABLE(threshold ...)
moving_over_cap           NOT-MEASURABLE(camera ...)
```

⚠ **Each case declares its own motion cap, and that is stated in the code rather than left to be
discovered.** The scrolling cases exist to exercise the *statistic*, so the escape is disabled for
them — the synthetic backdrop's own scroll rate (`pan/blocks` = 0.125 at pan 2) sits far above the
shipped cap, and leaving it on would short-circuit every one of them to NOT-MEASURABLE, **testing
nothing**. One case runs at the shipped cap and is the proof the escape fires.

### P2 / P7 — THE INERTNESS PROOF F-A FAILED. **All six legs PASS.**

```
M49_GEDGE_BL_NAT  PASS 4  SHIFT 0  NOT-VISIBLE 0  NOT-MEASURABLE 0 | tau=0.0040 M_med=0.0005
M49_GEDGE_CT_NAT  PASS 4  ... | tau=0.0040 M_med=0.0005
M49_GEDGE_MO_NAT  PASS 4  ... | tau=0.0040 M_med=0.0005
M49_GEDGE_MT_NAT  PASS 4  ... | tau=0.0040 M_med=0.0005      <- 079-02 read NOT-VISIBLE 4 here
M49_GEDGE_BL_SYN  PASS 4  ... | tau=0.0040 M_med=0.0005
M49_GEDGE_MT_SYN  PASS 4  ... | tau=0.0040 M_med=0.0005      <- 079-02 read NOT-VISIBLE 4 here
```

`tau` back on `SIGNAL_FLOOR`, `base=24` as declared, `bbox=n/a obs=n/a` on these v1-era sessions.
**P7 satisfied: the 079-02 regression is gone.**

### P3′ — injected ±1 recovery on real moving-camera sessions (cap disabled)

| session | `M_med` | δ | decided | recovered | not-meas | misses |
|---|---|---|---|---|---|---|
| `A2L_LEGA` | 0.0049 | 0 | 6 | **6** | 1 | – |
| `A2L_LEGA` | 0.0048 | +1 | 6 | **6** | 1 | – |
| `A2L_LEGA` | 0.0043 | −1 | 6 | **6** | 1 | – |
| | | | | | | **FULL RECOVERY** |
| `A1L_LEGA` | 0.0852 | 0 | 4 | 4 | 2 | – |
| `A1L_LEGA` | 0.0995 | +1 | 3 | 2 | 3 | `idx=0` read `ONSET-SHIFT(+1)`, wanted `(-1)` |
| `A1L_LEGA` | 0.0852 | −1 | 4 | 4 | 2 | – |
| | | | | | | **INCOMPLETE** |
| `LYRA_SMOKE_01` | 0.0801 | 0 | 5 | 5 | 1 | – |
| `LYRA_SMOKE_01` | 0.0767 | +1 | 5 | 5 | 1 | – |
| `LYRA_SMOKE_01` | 0.0830 | −1 | 6 | 4 | 0 | `idx=0` read `END-SHIFT(-1)`; `idx=2` read `ONSET-SHIFT(-2)` |
| | | | | | | **INCOMPLETE** |
| `M50L_LG9` | 0.3491 | 0 | 3 | **0** | 3 | **3 of 3 read `NOT-VISIBLE`, wanted `PASS`** |
| `M50L_LG9` | 0.3491 | +1 | 3 | 0 | 3 | 3 × `NOT-VISIBLE` |
| `M50L_LG9` | 0.3579 | −1 | 2 | 0 | 4 | 2 × `NOT-VISIBLE` |
| | | | | | | **NONE** |

🔑 **Two separable properties, and the distinction matters for what the tool is worth:**

- **The gate does not INVENT disagreements.** At `delta 0`, `A2L_LEGA`, `A1L_LEGA` and
  `LYRA_SMOKE_01` are clean — 15 decided events, 15 `PASS`, **zero false shifts at up to
  `M_med` 0.085**.
- **What degrades with motion is LOCALISATION.** At ~0.08 the gate still sees an edge but misplaces
  it by 1–2 frames or inverts the sign. At 0.349 it sees nothing and falls back into the `G259`
  false-fail — **three correct, unshifted labels called NOT-VISIBLE.**

### P4′ — setting the cap

Synthetic scroll ladder (uniform scroll, **no parallax — optimistic by construction**):

| pan | 0 | 1 | 2 | 4 | 6 | 8 | 12 | 16 |
|---|---|---|---|---|---|---|---|---|
| `M_med` | 0.0000 | 0.0625 | 0.1250 | 0.2500 | 0.3750 | 0.5000 | 0.7500 | 1.0000 |
| recovered | yes | yes | yes | yes | yes | yes | **yes** | no |

**Synthetic still recovers at `M_med` 0.75 — and is deliberately NOT the binding number**, precisely
because a uniform scroll has no parallax. Real content fails at 0.0801.

```
largest fully-recovered M_med, real = 0.0049   smallest INCOMPLETE = 0.0801
cap = floor_2sf( min(0.7500, 0.0049, 0.0801/2) ) = 0.0049
```

### **`MOTION_CAP = 0.0049`**

### P5 — `M50L_LG9`: **zero NOT-VISIBLE, zero SHIFT**, six NOT-MEASURABLE with numeric reasons

```
idx=0  NOT-MEASURABLE(threshold unsatisfiable: tau=1.0950 but d can never exceed 1.0 ...)
idx=1  NOT-MEASURABLE(camera motion: M_med=0.3491 cap=0.0049 ...)
idx=2  NOT-MEASURABLE(camera motion: ...)
idx=3  NOT-MEASURABLE(region covers the picture: 2073600px = 100.0% of the 2073600px frame ...)
idx=4  NOT-MEASURABLE(camera motion: ...)
idx=5  NOT-MEASURABLE(threshold unsatisfiable: tau=1.1938 ...)
```

every line carrying `bbox=projected obs=unmeasured` (§0.4's annotation — **reported, never a verdict
input**). **The exact session that read `NOT-VISIBLE 6/6, VERDICT FAIL` in 079-01 now reads `VERDICT
PASS` with six honest refusals.**

### P6 — batch over all ten sessions: counts **identical** to the individual runs

```
A1L_LEGA          | exit 0 | PASS 0 SHIFT 0 NOT-VISIBLE 0 NOT-MEASURABLE 6 | VERDICT PASS
A2L_LEGA          | exit 0 | PASS 6 SHIFT 0 NOT-VISIBLE 0 NOT-MEASURABLE 1 | VERDICT PASS
LYRA_SMOKE_01     | exit 0 | PASS 0 SHIFT 0 NOT-VISIBLE 0 NOT-MEASURABLE 6 | VERDICT PASS
M49_GEDGE_* (x6)  | exit 0 | PASS 4 SHIFT 0 NOT-VISIBLE 0 NOT-MEASURABLE 0 | VERDICT PASS
M50L_LG9          | exit 0 | PASS 0 SHIFT 0 NOT-VISIBLE 0 NOT-MEASURABLE 6 | VERDICT PASS
```

**Zero NOT-VISIBLE and zero SHIFT across the entire bank set.**

---

## 3. ⚠ What GREEN does and does not buy — read this before shipping the tool

**Acceptance rule (a) is MET.** No correct label is called NOT-VISIBLE anywhere in the set; every
refusal is NOT-MEASURABLE with its numbers printed.

🚨 **ACCEPTANCE RULE (b) IS NOT MET, AND THE CAP IS WHY.** At `0.0049` the tool refuses
`A1L_LEGA` (0.085), `LYRA_SMOKE_01` (0.080) and `M50L_LG9` (0.349) **entirely** — and the M2 field
captures are real gameplay, so they will almost certainly be refused too. The tool becomes **honest
and largely silent** on exactly the footage the complaint came from. That is a better failure than
the current one (confident wrong answers), but it is not a working field check, and it should not be
sold as one.

**Two facts chat should weigh, both measured, neither adopted here:**

1. **The cap sits EXACTLY on `A2L_LEGA`'s own `M_med` (0.0049)**, because that is the only fully
   agreeing session. A run-to-run drift of 0.0001 would refuse the session the cap was derived from.
   **The brief's own second constraint — a 2× margin below the first disagreement — permits anything
   up to `0.0400`**, which would keep `A2L_LEGA` comfortably inside and still refuse both 0.08
   sessions. I shipped **0.0049** because that is what the predeclared rule computes; **0.0400 is the
   defensible alternative within the same rule and it is chat's call, not mine.**
2. **`PASS` is more robust than `SHIFT`.** Zero false shifts were observed up to `M_med` 0.085, while
   shift *localisation* failed there. A **two-tier policy** — trust `PASS` up to ~0.08, trust a
   `SHIFT`'s magnitude only below the cap — is supported by the data. **Not implemented:** it needs
   its own predeclaration and its own known-answer run.

⛔ **Nothing here says a refused session's labels are wrong.** NOT-MEASURABLE is an unread surface.
⛔ **`M50L_LG9` is still not a known-good session** (`observability_measured: false`,
`bbox_source: "projected"` on all six) — 079-02's correction stands.

---

## 4. What changed in the tool

| | |
|---|---|
| **F-A ring subtraction** | **REVERTED.** `ring_mean` / `RING_DILATE_PX` / `RING_MIN_PX` are no longer referenced at all. Statistic is raw `d(region)`, as on `master`. |
| **Motion estimate** | **global** — median whole-frame changed-pixel fraction over clean frames, **full resolution, no downsampling**. |
| **Motion escape** | `M_med > MOTION_CAP` ⇒ `NOT-MEASURABLE(camera motion: …)`, new. |
| **F-B** | kept; `attainable` is now simply `ATTAINABLE_MAX = 1.0` (no ring needed). Asserted, never clamped. |
| **F-C** | kept, rephrased on region size alone: `>= MAX_REGION_FRAC` (0.90) of frame. |
| **F-D/E/F/G/H** | kept as built. |
| **Provenance** | `bbox=<source> obs=<measured\|unmeasured\|n/a>` on every event line. **Annotation only** — chat rejected it as a verdict rule, and the reason is in the code: the field case *is* projected boxes. |
| **CLI** | `--motion-cap` added; `--all` unchanged. |

`K_SIGMA`, `SIGNAL_FLOOR`, `MIN_BASELINE_FRAMES`, `BASELINE_MAX_FRAMES`, `MAX_REGION_FRAC` **all
unchanged**. `MOTION_CAP` is the only new constant and it is set solely by §2's procedure.

**Docs:** tool module header (envelope, the cap's provenance table, and the corrected
`measure_label_offset.py:92-93` note); `client-readme.md` **Step 6** in plain words; **`G260`** in
`gotchas.md`.

## 4.1 🔻 A mistake I made and repaired in the same turn — the comment stripper is REPO-SCOPED

The no-comments invariant applies to **this** repo, and `_strip_comments.py` is run against **its**
root. Having added one checker to the **CaptureBench** plugin, I ran the stripper against CaptureBench
too. **It modified 26 tracked files** — among them **`tools/a54_oracle.py`, which `A53` explicitly
protects** (*"stays untouched — any edit re-triggers `A53`"*), and `check_pose.py`, `m44_gates.py`
and the `pc7v3` comparators.

**Caught by reading the stripper's own summary line** (`changed: 26` where the intended answer was
`0`) and reverted immediately with `git -C <CaptureBench> checkout -- tools/`.

✅ **Fully recovered, verified:** CaptureBench is **tracked-clean at `472a409`**, exactly as
`CLAUDE.md` records it. The pre-existing **untracked** files there still carry their original mtimes
(01-09, 02-09, 03-09), so the stripper did not reach them and nothing was unrecoverable. My new
checker's hash is unchanged (`3BDC47D8678B3BEF…`) because it carries docstrings and no comments.

🔑 **The transferable part: CaptureBench is a DIFFERENT REPO WITH A DIFFERENT CONVENTION — its tools
carry comments on purpose.** A house rule scoped to one repo must be applied with that scope, and the
cheapest guard is the one that caught it: **read the tool's own summary line and stop when the count
is not the count you intended** (`G115`'s habit, on a different tool).

## 5. Owed

**P6 (office)** — the owner re-runs `--label-pixel-gate --report-only` (or `--all`) on the M2
Concorde captures and transcribes the summary lines only. **Predeclared: zero NOT-VISIBLE; most
events NOT-MEASURABLE with printed reasons.** ⚠ **If they are all refused for camera motion, that is
rule (b) failing on the real data and chat's §3 decision becomes the deciding one.** The two fields
worth transcribing alongside are the header's `M_med` and each event's `bbox=… obs=…`.
