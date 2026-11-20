# 081-39 — Chat ruling on 081-38 NEEDS-DECISION: pacing-off results recorded as stress readings; a declared PACED block becomes the headline cost gate at 720p and 1080p

Date: 2026-09-26 05:45 IST · Author: chat Claude (orchestrator) · Build 3 · Feature head `b25bab5` · cost predictions `docs/predictions/2026-09-26-m55-stage3-cost.md`

## What 081-38 established (accepted as measured)
- **720p, pacing off: PASS as declared.**
  - Game thread: Δ −0.083 ms, upper bound −0.002 ms.
  - Writer: +1.2 PNG/s, no drop, resolved.
  - **But the workload was light.** Required-pair yield was 0.17–0.20: `view_mismatch` 60–68 and `unsupported_delivery` 22–28 per leg. The paced prior on this recipe is 20/20.
- **1080p, pacing off: UNRESOLVED.**
  - The writer is saturated (backlog 141, latency p50 ~2.5 s).
  - m55's retained payload reached 267.6 MB, which is 99.7 % of the 256 MiB cap. `budget_exceeded` refused 30 of 60, and B1 failed vacuity (8 measured).
  - This is the memory envelope working as designed: refusals are honest and no wrong numbers were produced.
- **0 drops everywhere, and the oracle exit 0 on all four 720p B legs.**

## Ruling 1 — pacing-off results stand as STRESS readings, not the headline
**Q2: the 720p PASS stands.** It is the declared rule, read literally. It is labelled **"pacing-off stress, measured-pair yield 0.17–0.20"**, and it is not presented as m55's cost at full workload.

**1080p pacing-off stays UNRESOLVED**, recorded with its cause:
- writer saturation;
- payload cap;
- honest `budget_exceeded` refusals.

It is **not re-run** and not reshaped. Shortening legs to squeeze under the cap would measure an artificial workload.

Client-doc consequence: with pacing off at 1080p, expect `budget_exceeded` refusals. **Use paced capture with m55.**

## Ruling 2 — AMENDMENT 1: a PACED block is the headline cost gate (declared before it runs)
The accepted production-like mode, and every accepted recipe in this campaign, is **paced**: `IAI.Capture.Pace 1`, `IAI.Capture.Fps 30`. The cost question that matters to a client is the cost at the workload m55 actually does in that mode.

1. **Block P.** For each resolution, 720p then 1080p: one declared discard leg (evidence OFF), then A1 B1 B2 A2 and A3 B3 B4 A4.
   - Same recipe, and still only one A/B difference.
   - Pacing **ON at 30**, run log OFF, bench gate 0.
   - Leg length is Code's call from the bank. It must be long enough for a steady window after the 60-frame warm-up, and must fit a ≤ 45-min bench total including the diagnostic pair.
2. **Gate P (headline), with the same statistic** (one-sided 95 % bound, pooled A/A and B/B):
   - **Game thread:** PASS only if the upper bound is ≤ +1.000 ms. Otherwise UNRESOLVED.
   - **Writer:** the same literal throughput rule as 081-37/38. A drop beyond uncertainty → UNRESOLVED, and the 5 % resolution guard stays. Under pacing, both sides should run at the capture rate. Also required:
     - **0 drops**;
     - **pacing held**: B's arm rate is not below A's beyond uncertainty.

     No new tolerance is introduced.
   - **Vacuity:** each B leg needs **measured-pair yield ≥ 0.9** of required pairs. The paced prior is 1.0. A B leg below that is LEG-FAILURE. That is a stop and a finding, not a retry.
3. **Readings, not gates:**
   - writer latency p50/p95/p99 and backlog, with a trend over the leg;
   - m55 worker ms per captured frame and per pair;
   - CPU by thread class;
   - high-water against the 256 MiB cap and the priors;
   - yield with reasons;
   - `census_cycles`;
   - load context, including the unexplained game-process GPU-counter 0.0 % (an instrument limitation; name it).
4. **Diagnostic pair.** `S3_DIAG_A/B` runs **after** block P, exactly as declared in 081-37 (pacing off, 720p, run log ON). It stays outside every gate. It answers the FPS-21.5 prior.
5. **The pacing-off blocks already run are not re-run.** Resume must skip them.

## Ruling 3 — product finding recorded (m51 scope)
With pacing off, yield collapses:
- `view_mismatch`: the mask is served from the view family one before the colour frame, with camera Δ 0;
- `unsupported_delivery`.

m55 correctly **refuses** these pairs instead of measuring wrongly paired frames. That is the identity contract doing its job. The mispairing itself is the m51 mask/colour pairing defect, first seen in 078 under load, with the G295 shape. Record it as a gotcha and cross-reference it from the m51 notes. It is FUTURE (m51 pairing), not m55.

## Sequencing
- **081-39**, bench-free, xhigh:
  1. AMENDMENT 1 into the cost predictions (dated, cites this ruling);
  2. harness: block P, gate P, the vacuity yield rule, resume skipping the done pacing-off blocks;
  3. proofs, both ways: gate P reads PASS/UNRESOLVED on synthetic paced data; vacuity yield 0.89 → LEG-FAILURE and 0.90 → valid; a drop, or pacing not held → UNRESOLVED; resume launches block P first and never re-launches a done leg;
  4. docs committed **before** the boundary re-issue;
  5. dry run and preflight replay.
- **081-40**, bench, high: block P, then the diagnostic pair, then the Stage 3 cost report (both pacing modes, with the headline from P).
- **081-41**, bench-free, high: client docs.
