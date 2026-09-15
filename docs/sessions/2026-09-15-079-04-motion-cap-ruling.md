# 2026-09-15 — 079-04 — `MOTION_CAP` ruled to the margin bound; re-proved; frozen for Codex review

**VERDICT: GREEN.** P1, P2, P3, P5, P6 and P8 all read as predeclared.
**One constant changed and nothing else in the tool.** `measure_label_offset.py` byte-unchanged;
`Source/`, `Shaders/`, fixtures, schema, harness untouched; no capture, build or cook.
**Nothing merged** — this is the freeze point for Codex's merge-gate review.

**Predictions:** `docs/predictions/2026-09-15-verifier-field-validity.md` **AMENDMENT 3**, committed
**`f064f0f`** before the constant was touched and before any re-run.
**Evidence:** `_bench_sessions_bank\M079_VERIFIER_EVIDENCE\079-04\`.

---

## 1. The rulings, and what they rest on

### 1.1 `MOTION_CAP = 0.040` — the margin bound, not the minimum

`P4′` produced **two** bounds. 079-03 shipped their minimum; chat rules the margin bound:

| | value | source |
|---|---|---|
| (a) largest real `M_med` with FULL recovery of an injected ±1 | **0.0049** | `A2L_LEGA` |
| (b) ≥ 2× margin below the first INCOMPLETE session | **0.040** | `LYRA_SMOKE_01` 0.0801 ÷ 2 |

🔑 **(a) rests on the single value it was derived from.** A 0.0001 measurement drift would refuse the
very session that set the cap — and `A2L_LEGA`'s own `M_med` moved between 0.0043 and 0.0049 across
the three shift runs, so that drift is **observed, not hypothetical**. (b) carries a safety factor
and still refuses every session that failed to recover. **At 0.040, `A2L_LEGA` sits ~8× inside the
cap instead of exactly on it.**

The header carries the provenance verbatim:

```
cap=0.040 (2x margin below first incomplete recovery, LYRA_SMOKE_01 M_med=0.0801;
only full recovery observed at 0.0049, A2L_LEGA; ruling 079-04)
```

### 1.2 No middle tier — and the argument is the client's own complaint

079-03 measured **zero false shifts up to `M_med` 0.085** (the gate does not invent disagreements)
and proposed trusting `PASS` higher than `SHIFT`. **Rejected:**

> **A `PASS` from a gate that cannot read back a one-frame shift is not evidence of alignment — it is
> the client's `F1` complaint restated.** If the instrument provably cannot see a one-frame error at
> that motion level, "no error found" carries no information about whether one is there.

The measurement is kept **as a fact** (below, and in the tool header) and as the **stated reason
there is no middle tier**. Above the cap the answer is `NOT-MEASURABLE(camera motion: …)`, full stop.

### 1.3 The synthetic ladder is non-binding, and the header says so

It recovers to `M_med` **0.75** only because a uniform scroll has **no parallax**. It stays in the
selftest as the *"a shift is still readable under motion at all"* proof and **sets nothing** — the
cap is set by real sessions only.

### 1.4 The known-answer method is settled

079-03's finding is accepted: mask-derived onsets are **circular** (`m49` A1 defines
`affected_frames` as the observable subset, so the in-span mask onset *is* the label onset, and `m44`
writes no mask outside the span). **`CaptureBench/tools/m079_shift_recovery.py` is the known-answer
method for this gate from here on** — SHA-256 `9FA26D4F4172F887…`.

⚠ **One coherence fix to that checker, and the reason to make it:** it still printed
`CAP = 0.0049`, its own 079-03 `min()` rule, which now **contradicts the shipped constant**. It now
prints **both bounds**, names ruling 079-04, and echoes `verify_capture.MOTION_CAP` so the two can
never silently disagree. A calibration tool that reports a different number from the thing it
calibrated is a trap for the next reader.

---

## 2. The readings

| | prediction | reading | |
|---|---|---|---|
| **P1** | selftest 17/17 `OK` | **17/17, `SELFTEST: OK`**, no `*** BROKEN ***` | ✅ |
| **P2** | six bench legs byte-identical to 079-03 | **`PASS 4 / SHIFT 0 / NOT-VISIBLE 0 / NOT-MEASURABLE 0`**, `tau=0.0040`, `M_med=0.0005`, `cap=0.0400` on all six | ✅ |
| **P3** | `A2L_LEGA` full 6/6 × 3 | **6/6 at δ = 0, +1, −1** | ✅ |
| **P5** | `M50L_LG9` unchanged, zero NOT-VISIBLE / SHIFT | **6 refusals, 0 / 0** | ✅ |
| **P6** | batch == individual | **identical** | ✅ |
| **P8** | `LYRA_SMOKE_01` + `A1L_LEGA` all refused | **0 PASS / 0 SHIFT / 0 NOT-VISIBLE** | ✅ |

### 2.1 P8's reason split — exactly the departure AMENDMENT 3 declared in advance

```
A1L_LEGA        idx0..4 camera motion   idx5 manifested-false-or-empty
LYRA_SMOKE_01   idx0..5 camera motion
```

The brief's wording said *every* event would read `(camera motion …)`. It does not, and **that is
correct behaviour, not a miss**: `A1L_LEGA idx=5` (`lod_popping`) is `manifested: false`, and the
manifested check sits at the top of the event loop, ahead of region / baseline / threshold / motion.
**Declared in AMENDMENT 3 §A3.5 before the run**, with the load-bearing predicate stated as
zero PASS / zero SHIFT / zero NOT-VISIBLE.

### 2.2 `M50L_LG9` — unchanged, three distinct honest refusals

```
idx0 threshold unsatisfiable   idx1 camera motion   idx2 camera motion
idx3 region covers the picture idx4 camera motion   idx5 threshold unsatisfiable
```

### 2.3 The whole set

**Zero NOT-VISIBLE and zero SHIFT across all ten banked sessions; every session `VERDICT PASS`,
exit 0.** `A2L_LEGA` unaffected at `M_med 0.0049` against `cap 0.0400`: `PASS 6 / NOT-MEASURABLE 1`.

---

## 3. The fact that is kept rather than acted on

**Zero false shifts were measured up to `M_med` 0.085** — at δ = 0, `A2L_LEGA`, `A1L_LEGA` and
`LYRA_SMOKE_01` gave **15 decided events, 15 `PASS`, no spurious shift**. What degrades with motion
is **localisation**, not fabrication: at ~0.08 the gate still finds an edge but misplaces it by 1–2
frames or inverts the sign; at 0.349 it finds nothing and falls back into the `G259` false-fail.

⛔ **This is recorded as a measurement and deliberately NOT turned into policy** (§1.2). It is the
thing a future session will be tempted to build a middle tier on, so the reason it was refused is
written down beside it.

## 4. Standing limits, unchanged

- **A cap at 0.040 still refuses most real gameplay footage.** Rule (a) — never call a correct label
  NOT-VISIBLE — is met. **Rule (b) — be useful on that footage — is not.** The tool is honest and
  largely silent there, and that trade is deliberate.
- **Only ONE real session has ever been observed to recover fully**, so the cap rests on a thin base
  and should be re-derived whenever more masked moving-camera sessions exist.
- **`NOT-MEASURABLE` is an unread surface**, not a statement that the labels are wrong.
- **`M50L_LG9` is still not a known-good session** (`observability_measured: false`,
  `bbox_source: "projected"` on all six).

## 5. Owed

**The office re-run** on the M2 Concorde captures: `--label-pixel-gate --report-only` (or `--all`),
transcribing the summary lines, the header's `M_med`/`cap`, and each event's `bbox=… obs=…`.
**Predeclared: zero NOT-VISIBLE.** ⚠ If those sessions read `M_med` above 0.040 — likely, for real
gameplay — they will be refused for camera motion, and **that is rule (b) failing on the real data**,
which is the decision chat already has in front of it.
