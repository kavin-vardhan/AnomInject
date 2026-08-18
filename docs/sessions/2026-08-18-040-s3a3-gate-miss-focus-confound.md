# 2026-08-18 — 040 — S3a-3: the dropped-positive gate PASSES; the C1 subset re-run MISSES on an uncontrolled variable

**HALTED. Code held off `master`** on `s3a-3-GATE-MISS-focus-confound` (`8f57a62`, pushed).
`master` stays at `515a869` (S3a-2, certified). No same-turn fix, no re-run for a green.

---

## 1. What was built (compiles clean, editor + game, exit 0)

- **`run_summary`**: `capture_path: "sve"` + `key_ring_published/consumed/missed/wrapped/corrupted`,
  emitted **only when the switch is ON** (C1).
- **`IAI.Capture.SVE.ForceMissPhase`** — corruption fires when `((serial + phase) mod N) == 0`.
- **FIX 2 as ruled**: `LastRunDir` set at `StartRun`, returned by `GetStatus`; `RunDir` cleared in
  `FinishRun`'s lifecycle reset. The reporting read is now structurally separated from the
  destructive path.
- **Banner fixed**: `DescribeGrabPoint()` reports the grab point (`sve/scene-colour` /
  `async/backbuffer` / `sync`) instead of `bAsyncCapture`.

---

## 2. THE DROPPED-POSITIVE GATE — **PASS**, and it needed a corrected metric first

### 2.1 My own overlap metric was ambiguous. Correcting it.

In journal 039 I reported "a dropped POSITIVE frame was never exercised" from
`overlap = missing ∩ claimed = 0`. **That metric is ambiguous: zero overlap is *also* the PASS
condition.** If the label path drops a positive frame correctly, the frame vanishes from the files
**and** from the claims, so the intersection is empty either way.

**The sound discriminator is whether the claimed set SHRANK against the clean run.** Under it:

| phase | drop offsets (mod 12) | claimed offsets (mod 12) | positives dropped |
|---|---|---|---|
| 0 | 1, 7, 11 | 4, 5, 9, 10 (full) | **none** — journal 039's reading stands |
| 1 | 1, 7, 11 | 4, 5, 9, 10 (full) | none |
| 2 | **0, 6, 10** | **4, 5, 9** | **YES — offset 10** |
| 3 | 1, 7, 11 | 4, 5, 9, 10 (full) | none |

So the phase parameter is **coarse but real**: three of four phases reproduce the same lock, and
**phase 2 breaks it**. The whole N=4 phase space was exhausted rather than stopping at the first
success or the first failure.

### 2.2 The gate, on the artifact

**Phase 2 dropped 7 positive frames — `[10, 22, 34, 46, 58, 70, 82]`, one per burst.** Proven
non-zero *before* the result was read, as required.

| check | result |
|---|---|
| every dropped positive absent from **disk** | **yes** |
| every dropped positive absent from **claims** | **yes** |
| **claimed indices with no file behind them** | **`[]` — empty** |
| files ↔ label rows, both directions | **68 ↔ 68, 1:1** |
| `session_index` == image number, every row | **yes** |
| `run_summary` | `capture_path: sve`, published 2691 · consumed 2018 · missed 673 · corrupted 673 |

**The pre-declared prediction held: the label path drops the frame cleanly, and no positive is
claimed without a file behind it. No label-fabrication defect in the SVE path.** The P3b failure mode
does not arrive by this route.

---

## 3. THE GATE MISS — C1 subset re-run, 16 extra fields

`SUBSET TEST: fields differing in TEST but NOT in CONTROL: 16` ⇒ **FAIL by the pre-fixed rule.**

The extras: `engine/ticks_msec` on all 8 events, `run.json/start_frame`,
`run_summary/end_frame`, and six line-level `labels.jsonl` fields.

**Cause — environmental, not code:**

| leg | `start_frame` | `end_frame` | `ticks_msec[0..3]` |
|---|---|---|---|
| m23 leg 1 | 1 | 122 | 134, 667, 1201, 1734 |
| m23 leg 2 | 1 | 122 | 134, 667, 1201, 1734 |
| S3a-2 OFF | 1 | 122 | 134, 667, 1201, 1734 |
| **S3a-3 OFF** | **2559** | **2681** | **30270, 30804, 31337, 31870** |

`30270 ms` is the **30-second focus-gate safety timeout**. The S3a-3 leg never got window focus, sat
in `ArmedPending` for the full timeout, then started anyway — so every absolute frame/time field
shifted. The log confirms it: `Capture armed — still waiting for game-window focus (2s/4s/6s…)`.

**The finding that matters more than the miss:**

> **The gate has an uncontrolled environmental variable — whether the game window acquires focus —
> and it can flip the result in either direction.**

My control pair was **n = 2 and both halves got prompt focus**, so it never sampled that variance.
And the S3a-2 re-gate in journal 039 **passed partly because all four of its legs happened to get
prompt focus** — that was not designed, it was luck. A control pair that does not span the
environment's real variance is a control pair that certifies the wrong thing. This is the
bracket-vs-contain problem (**A57**) reappearing in the harness rather than in an oracle.

**I did not re-run to get a prompt-focus leg.** That would be selecting a favourable run, and the
result would be a green obtained by discarding a red.

**What I cannot claim from this data:** that S3a-3 with the switch OFF *is* subset-identical. The
comparison that would establish it — same focus condition on both sides — has not been run. The miss
is explained, **not** dismissed.

**Chat-side call (gate design, not mine to make mid-turn):** pin the variable — e.g. run every leg
with `IAI.Capture.FocusGate 0` (⚠ **G93**: only safe at 30 fps; keep the gate ON above that), or make
equal `start_frame` a precondition of the comparison and re-run when it does not hold. Either is a
change to the gate, and gates are not edited after seeing results.

---

## 4. NOT PERFORMED — FIX 2's dashboard verification

The ruling required verifying, not assuming, that `capture_stop` / `capture_status` /
`ControlSnapshot` still surface the right path via `LastRunDir`. **It was not performed** — the halt
came first. It needs a WebSocket client against the control server *after* a run ends, and
`-ExecCmds` only issues commands at startup, so the packaged harness cannot reach a post-run status
query without one. **`LastRunDir` is therefore implemented but UNVERIFIED at runtime**, and must not
be treated as certified.

---

## 5. State

- `master` — **`515a869`**, untouched, still the certified S3a-2.
- `s3a-3-GATE-MISS-focus-confound` — **`8f57a62`**, pushed, **do not merge**.
- `s3a-2-GATE-FAILED-do-not-merge` — `087f4d9`, unmerged, still evidence.
- Staged package — carries the **S3a-3** binary (A44-verified, both encodings). The pre-S3 binary is
  preserved as `StackOBot.exe.m23-baseline`. ⚠ **The staged exe is now ahead of `master`** — restore
  the baseline or re-stage before any leg that must run certified code.
- New legs on disk (unbanked pending the verdict): `S3A3_OFF`, `S3A3_P1`, `S3A3_P2`, `S3A3_P3`.
