# 081-37 — Chat ruling on 081-36 GREEN: build 3 requalification complete; Stage 3 released (cost harness design first, then the cost run, then docs)

Date: 2026-09-26 04:35 IST · Author: chat Claude (orchestrator) · Build 3 (`85c99b3`, Game 002805CF) · Branch head `2125159`

## Ruling 1 — requalification accepted
The c0–c3 requalification of build 3 is **complete and accepted**, as recorded in 081-36's 92-leg ledger:
- 6 c0 readings;
- 19 c1 stack PASS, including the first runtime proofs of the refusal census and G10;
- 4 legacy groups PASS, with CB-N and CB-S PASS-REEVALUATED under AMENDMENTS 5–6;
- 42 c2 PASS;
- 5 c3 PASS, with Lyra G270 on the PASS branch.

Over the run, AMENDMENTS 1–6 were declared before use, proven in both directions and checked for no verdict flips. Each corrected an encoding against an already-ruled class, or admitted a pre-m55 mechanism under signature and a cap. No stack predicate, threshold or m55 measurement was loosened.

**Pre-existing product findings (not m55), queued for m51/M3:**
- **G295 (coalesced-arm yield).** When a target-mask render slips a frame, the next frame comes out unmeasured.
  - The label is honest, so this costs yield, not correctness.
  - Fix: a per-arm pixel readback.
- **G298 (m47 packaged-build false positive).** A run-wide shader-readiness marker flagged 90 frames that no incomplete material could have affected. A client could discard good frames because of it.
  - Fix: tie the marker to the materials in use; log which material is incomplete; correct the "structurally zero" claim.
- **`coverage_pct`** is timing-sampled. G297 singletons on it appear on both binaries. The fix is the existing FUTURE item: compute it from the frame's own mask.

## Ruling 2 — Stage 3 released (per 081-03 §Stage 3, with every carry-forward)
The oracle is already done (081-19). What remains is **cost** and **docs**. Order:
- **081-37, bench-free, xhigh.** Cost harness design, predictions, the FPS 21.5 prior, and a dry run.
- **081-38, bench, high.** The cost run.
- **081-39, bench-free, high.** Docs.

### Cost gate (081-03, unchanged)
- A/B/B/A with pacing off, at **720p and 1080p**, on **StackOBot only** (Lyra was scoped out in 081-07).
- **A = build 3 with evidence OFF, B = build 3 with evidence ON.** The same binary for both, so only m55's stage differs.
- **Metrics:**
  - game-thread ms;
  - m55 worker ms;
  - writer throughput;
  - queue and byte high-water, against the logged bound;
  - drops;
  - tails (p95/p99);
  - **required-pair yield** (measured / required) with reasons;
  - the **latency distribution**;
  - `census_cycles`, as a reading.
- **Gate:** ≤ **+1 ms game thread** AND writer throughput unchanged within uncertainty. Anything else is **UNRESOLVED (not PASS)**.
- The uncertainty method, the statistic, the leg count and the durations must be **declared in a predictions file before any leg runs**. The A/A and B/B spread is the natural noise floor; PASS needs the bound, not just the point estimate, to be inside +1 ms.

### FPS 21.5 prior (carried since 081-05)
Evidence-OFF MAIN4 read 21.5 fps against 30 elsewhere. Any cost leg at that FPS needs an explanation before the gate is read.
- First, explain it bench-free from the banked MAIN4 legs: log, settings, pacing, resolution, fixture.
- If the bank cannot explain it, add a declared diagnostic pair to the cost run. That pair must not feed the gate.

### Harness
- Reuse the 081-23 infrastructure: preflight/postflight, quiet gate, **person-present gate** (including ENV-INTERRUPTED), hash-locked boundary, budget tag, resume-safe ledger, stop at first failure.
- Performance is sensitive to a busy PC. Record each leg's CPU/GPU load context and any foreign process. A leg with person evidence or foreign load is invalid and gets re-run, never averaged in.
- Total bench time ≤ 60 min, fitted to one window.

### Carry into the Stage 3 report
- the UNEXERCISED block verbatim;
- the high-water priors (17.5 / 19.4 / 39.4 / 49.8 / 66.4 MB), kept beside the new readings;
- the c0 long-run reading (no measurable ON/OFF effect; stage ≈ 0.12 logical cores).

### Docs (081-39)
- **Field reference**, covering all new fields, the enum and the sidecar.
- **Client readme**:
  - "no verdict in v1";
  - the delay=3 paragraph;
  - the **memory envelope** per 081-15/16 (m55's admitted payload only; hard cap 256 MiB; closure-bound formula; 1080p ≈ 110 MB covered; 1440p ≈ 195 MB unexercised; 4K ≈ 440 MB → expect `budget_exceeded`; refusals are honest);
  - the `positive_frames` caveat;
  - oracle usage.
- **Catalogue note** for the bench twins.
- **Gotchas** and the **CLAUDE.md** status block.
- **Known pre-existing limits** (G295, G298, `coverage_pct`) in a limits section.

## After Stage 3
Codex reviews every Code-authored change (build 2 onward, the oracle, the requalification harness, Stage 3). The owner pastes that request to Codex. Then the comment stripper, the merge (by Code), and the tag.
