# 081-39 — m55 Stage 3 cost: AMENDMENT 1 declares a paced block P as the headline gate; harness amended and proven both ways; bench-free

Date: 2026-09-26 · Author: Claude Code (Opus 5.5), headless, brief 081-39 (bench-free, xhigh) · Build 3 `85c99b3`, Game exe 002805CF
(unchanged; no build, no plugin source change) · Ruling `_reviews/081-39-chat-ruling-cost-paced-block.md` (on the 081-38 NEEDS-DECISION).

## 1. Goal

Chat's ruling says:
- The 081-38 pacing-off results stand as **stress readings**: 720p PASS at a measured-pair yield of 0.17–0.20; 1080p UNRESOLVED, with
  writer saturation → the payload cap → `budget_exceeded` refusals. Neither is re-run.
- A declared **paced block P** (30 fps) becomes the headline cost gate, with the same statistic and no new tolerance. A P B leg needs a
  measured-pair yield ≥ 0.9.
- The diagnostic pair runs after P.

This brief turns that into AMENDMENT 1 of the cost predictions, amends the harness, proves it both ways without the bench, commits the docs,
re-issues the cost boundary and replays the preflight offline.

**Bench-free:** no game, window or leg was launched. The only processes started were synthetic Python programs of mine (a replica of the
m11 pacer, §3.1).

## 2. What was declared — AMENDMENT 1 (`docs/predictions/2026-09-26-m55-stage3-cost.md`)

- **A1.1 Stress.**
  - 720p pacing OFF: PASS, labelled "pacing-off stress, measured-pair yield 0.17–0.20".
  - 1080p pacing OFF: UNRESOLVED, with its cause: writer saturation → m55 payload at the 256 MiB cap → honest `budget_exceeded`
    refusals → B vacuity LEG-FAILURE.
  - All 18 pacing-off legs are **retired**: never scheduled, and refused by name. Their ledger rows are unchanged. The ruled LEG-FAILURE
    row no longer halts.
  - Client-doc consequence (081-41): use paced capture with m55.
- **A1.2 Block P.**
  - Per resolution (720p, then 1080p): a declared discard (evidence OFF), then `A1 B1 B2 A2`, then `A3 B3 B4 A4`.
  - Pacing ON at 30 comes from the runner's unchanged base line. P appends only `IAI.Bench.ChangeGate 0, IAI.Capture.RunLog 0`, and A adds
    `IAI.Capture.ChangeEvidence 0`.
  - Then the diagnostic pair, exactly as §1.6.
- **A1.3 Durations.** 600 frames at both resolutions:
  - The paced solid recipe captures at 24.75–24.95 fps, so a leg is ≈ 24 s of capture with a 540-frame steady window.
  - That is 30 events against the tag pool's 55.
  - Paced, the 1080p writer keeps up, so the 300-frame limit that pacing OFF needed does not apply.
  - Expected ≈ 18–19 min. The campaign cap is 42 min (2520 s), so the whole window stays inside 45 min.
- **A1.4 Gate P**, per resolution, same statistic:
  1. game-thread upper bound ≤ +1.000 ms;
  2. writer drop beyond uncertainty → UNRESOLVED, with the 5 % guard;
  3. 0 drops on every counted leg;
  4. **pacing held**: B's arm rate is not below A's beyond uncertainty.

  The P mode check requires `paced: true`, the Pace echo ON, the Fps echo 30, `target_fps` 30, gate 0 and the run log OFF. The headline is
  PASS iff both resolutions PASS.
- **A1.5 Vacuity.** Measured-pair yield ≥ 0.9, tested in integers. Below it → LEG-FAILURE. The 20-pair floor is kept; the diagnostic pair
  keeps §5.
- **A1.6 Readings.** Latency and backlog **trend** (four quarters plus slopes), pacing, worker ms, high-water as % of the cap, yield with
  reasons, `census_cycles`, and load. The game's per-process GPU counter reading 0.0 % is named as an instrument limitation.
- **A1.7 The metric under pacing** — §3.1.
- **A1.8 Expected values** from the bank, with three declared risks:
  1. the pacing-held and writer clauses are literal while the pacer makes the arm rate nearly constant (c0's ON leg was 0.55 % slower with
     the run log ON);
  2. `S3_DIAG_B` sits at the pacing-off yield of 20–24 of 120, against a floor of 20;
  3. the busy-wait could hide up to ~1 ms of a sub-millisecond change on a tighter-waking host.

## 3. Proofs

### 3.1 The game-thread metric under pacing — `081-39-evidence/proof-pacer.json` (6 of 7 as expected)

Under pacing, the game-thread cycle count includes the m11 pacer's final `SwitchToThread` loop (source: `PaceThisTick`, margin 1.5 ms;
UE `Sleep` truncates to whole ms; `timeBeginPeriod(1)`).
- **Derivation (declared before running):** the loop would absorb sub-millisecond work changes.
- **Replica** (a synthetic process with the same calls; 600 frames per condition; the 081-37 sampler): `Sleep(n)` returned after
  n + 1.6–2.0 ms, and the loop averaged 0.20–0.23 ms per frame.

| condition | Δ cycles, ms/frame | expected |
|---|---|---|
| paced, +0.4 ms of work, tight jitter | **+0.335** | [−0.15, +1.15] ✓ |
| paced, +1.4 ms, tight jitter | **+1.281** | ≥ 0.85 ✓ (gate P can fire) |
| paced, +1.4 ms, 1 ms jitter | +1.244 | [1.0, 1.8] ✓ |
| unpaced control, +1.4 ms | +1.294 | [1.1, 1.6] ✓ |
| A/A repeat | −0.007 | ≤ 0.25 ✓ |
| pacer holds 30 fps | 30.02–30.04 | [29.8, 30.05] ✓ |
| busy-wait moves opposite to +0.4 ms of work | **−0.030** | [−0.55, −0.25] or [0.45, 0.75] ✗ — **refuted** |

The derivation's mechanism is refuted by measurement, and it is reported as not-as-expected. The consequence is favourable: on this box the
paced metric tracks work changes like the unpaced one (→ G303).

### 3.2 Banked paced legs through the P readings — `081-39-evidence/proof-readings-paced.json` (7 of 7)

| leg | reproduced | published |
|---|---|---|
| c0 `S1_STACK_ON_A1` (1080p, 1800 frames) | writer latency p50 331.5 ms, throughput 24.739 PNG/s, high-water 66,392,064 B, yield 220/360 | 333 ms, 24.71, 66.39 MB, 220/360 (081-24) |
| c0 `S2_STACK_OFF_A1` | 330.5 ms, 24.876 PNG/s | 331 ms, 24.84 |
| c2 `C2_SOLID_N_A1` / `_S_A1` (720p, 90 frames) | yield 20/20, high-water 18,432,000 / 15,667,200 B; writer latency p50 164.3 ms vs the leg's own colour-completion p50 165 | 20/20, same bytes |
| c1 `C1_SOLID1080_S_A1` | yield 20/20, high-water 62,238,720 B; writer latency 351.7 vs 352 | 20/20, same bytes |

Also shown:
- the P mode check **accepts** the paced legs' pacing (and still rejects their gate 14 and run log ON);
- it **rejects** the 081-38 pacing-off legs (`paced=False`, Pace echo OFF);
- the P vacuity rule fires on c0 `S1` (yield 0.611 — the tag-pool tail of an 1800-frame leg) and not on the 20/20 legs.

⚠ **A checker defect was caught and fixed before the verdict was read (G142's shape).** The first version compared the leg summary's
colour-completion p50 (every issued frame) with the p50 of the sidecar pair rows (a subset), which are different populations. It "failed"
S1 and C1. It was replaced by the like-for-like check (writer latency from PNG mtimes against the leg's own published p50). The lib was not
changed for it. First-run console kept.

### 3.3 Dry run — `081-39-evidence/dryrun/dryrun-results.json` (112 of 112, pre-lock)

The full-size end-to-end run gives 20 launches in the declared order (P720 discard+8, P1080 discard+8, diagnostic A, B). Every attempt was
gated. +0.4 ms was recovered at both resolutions, the arm rate was ≈ 25 fps with the writer tracking it, B yield was 120/120, drops were 0,
and the trend and pacing readings were present. Headline PASS, from P only.

Gate P units, as the ruling asks:

| case | verdict |
|---|---|
| +0.4 ms, tight spread | PASS |
| +1.4 ms | UNRESOLVED |
| +0.6 ms, wide spread | UNRESOLVED |
| +0.6 ms, tight spread (control) | PASS |
| exactly +1.000 ms | PASS |
| +1.001 ms | UNRESOLVED |
| writer drop beyond uncertainty | UNRESOLVED |
| writer drop inside uncertainty | PASS |
| writer under-resolved | UNRESOLVED |
| **pacing not held** (B 24.80 vs A 25.00, writer unchanged) | **UNRESOLVED** |
| pacing inside the noise | PASS |
| **any drop** on a counted leg | **UNRESOLVED** |
| drops not read | UNRESOLVED |
| a sub-0.9 B leg | UNRESOLVED |
| FPS-prior hold | HELD-FPS-PRIOR |
| incomplete | UNRESOLVED |

Vacuity:

| yield | result |
|---|---|
| **0.89 (89/100)** | **LEG-FAILURE** |
| **0.90 (90/100)** | **valid** |
| 108/120 | valid |
| 107/120 | LEG-FAILURE |
| 0 required | LEG-FAILURE |
| 18/20 (below the kept floor) | LEG-FAILURE |

End to end:
- a 0.89 B leg halts the campaign with the yield in the row, and a 0.90 leg is counted;
- a writer flush timeout or an encode failure → LEG-FAILURE, and gate P reads UNRESOLVED;
- pacing OFF on a P leg, Fps 60 on a P leg, evidence not effective, or the wrong resolution → HARNESS-FAIL;
- fixture exhaustion → NOT-RUN;
- five load-invalid attempts → exit 4;
- the 2520-s cap → exit 6;
- +1.4 ms, a writer drop, or pacing not held → headline UNRESOLVED.

**Resume on the real 081-38 ledger** (a copy; the real one is not touched):
- the first launch is `S3_P_720_DISC_T1`, and the launches are exactly block P, then the diagnostic pair;
- no pacing-off leg is launched, and all 12 ledger rows from 081-38 are byte-identical;
- the ruled row `S3_1080_B1_T1` is named in the campaign state;
- stress 720p re-reads PASS with 081-38's numbers (Δ −0.0827, upper −0.0024, "yield 0.17-0.20"), and stress 1080p reads UNRESOLVED with its
  cause;
- a re-invocation after completion launches nothing, and a retired leg is refused by name;
- a stop mid-P1080 → the next invocation re-launches nothing from P720, re-runs the P1080 discard and block 1 whole, and never re-launches a
  counted label.

## 4. Harness changes (`_reviews/081-37-cost-lib.py`, `081-37-cost-window.py`; pre-081-39 copies in `081-39-evidence/`)

**Lib:**
- adds the `P_720` / `P_1080` resolutions (600 frames, paced) and the paced extras;
- sets `BENCH_BUDGET_S` to 2520;
- adds the retired and active leg sets, and schedules P then the diagnostic pair;
- halting is read from active legs only, and a retired leg is refused;
- adds the paced mode check, per-leg `drops` / `drops_total`, `p_vacuity`, the trend reading, pacing readings and high-water % of the cap;
- adds `verdict_P` (gate P);
- the analysis gives the headline from P, and `stress` from the retired blocks (with their label, yields, halting row and cause).

**Window:**
- before regenerating them, keeps the previous `cost-verdict`, `oracle-readings`, `bank-manifest` and `evidence-manifest` as
  `*.before-<tag>.json`;
- runs the oracle only on active counted B legs, merged with the readings it already has;
- prints the headline, stress and diagnostic sections separately.

**Unchanged:** the sampler; the 081-37 statistic and `verdict_for` (they read the stress blocks); every validity class.

## 5. Lock order (the 081-33 / 081-37 pattern)

This docs commit (the amendment, this journal, G302/G303 with the G295 cross-reference, and the status block) is pushed from a scratch
worktree outside the StackOBot tree. After it:
1. the cost boundary `_reviews/081-37-evidence/cost-boundary.json` is re-issued, pinning this head, the committed predictions' SHA-256 (with
   a byte-identical copy at `081-39-evidence/predictions.md`), and every harness file;
2. the dry run is repeated;
3. the window runner is walked with a stubbed campaign;
4. the preflight is replayed offline.

Those results are in the 081-39 report and evidence, not here, because this commit is the head they pin.

## 6. State

- Build 3 unchanged. Feature branch: this docs-only commit. NOT MERGED, NOT TAGGED.
- Main checkout `m51` `53bf725` untouched; `master`, tags, containers, CaptureBench and `ToCodex\` untouched.
- **Next — 081-40 (bench):** block P, then the diagnostic pair, then the Stage 3 cost report with both pacing modes and the headline from
  P.
