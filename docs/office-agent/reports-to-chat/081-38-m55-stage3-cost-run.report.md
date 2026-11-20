# REPORT for 081-38-m55-stage3-cost-run.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-38-m55-stage3-cost-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-26 05:21:19 | finished: 2026-09-26 05:35:34 | duration: 14 min | exit code: 0
context at last turn: 392941 tokens (39% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-38-m55-stage3-cost-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner**
- At 720p the feature costs nothing measurable on the game thread: −0.08 ms per frame, and even the worst case is below 0 ms, far under the 1 ms limit. Image writing did not slow down. **720p PASS.**
- At 1080p the run stopped after the first feature-on leg, so there is **no 1080p answer yet**. With pacing off the image writer falls far behind, and the feature hit its 256 MiB memory cap. It then refused half its measurements, as designed, and too few were left to count as a real test.
- A second surprise: with pacing off, the feature measured only about 1 in 5 of the pairs it normally measures, even at 720p. So the 720p pass covers that lighter workload.
- Nothing crashed or dropped frames, and the PC stayed quiet throughout. Chat now needs to decide how to test 1080p.

**1. The gate per resolution** (read exactly as declared)

| res | game-thread Δ (B−A), ms/engine frame | upper bound | clause | writer Δ PNG/s | upper bound | half-width vs 5 % limit | clause | verdict |
|---|---|---|---|---|---|---|---|---|
| 720p | −0.0827 (A 3.607, B 3.525) | **−0.0024** | PASS (≤ +1.000) | +1.234 (A 68.06, B 69.29) | **+2.513** | 1.279 ≤ 3.403 | no drop, resolved → PASS | **PASS** |
| 1080p | — | — | — | — | — | — | — | **UNRESOLVED** (only 1 A and 0 B legs counted; 4+4 needed) |

- Overall, by the declared rule: **UNRESOLVED**.
- `HELD-FPS-PRIOR` did not trigger (720p arm rates were 68.3–71.1 fps).

**2. Per-leg table**
- Every leg had no person evidence and no invalid attempt.
- "Foreign CPU" is load from other processes, as cores (mean / worst 1-second interval).

| leg | status | arm fps | GT ms | writer PNG/s | foreign CPU | foreign GPU / IO |
|---|---|---|---|---|---|---|
| 720 A1 | VALID | 68.86 | 3.578 | 67.33 | 0.92 / 1.13 | 0.03 % / 0.56 MB/s |
| 720 B1 | VALID | 69.89 | 3.516 | 69.76 | 0.86 / 1.11 | 0.03 / 0.65 |
| 720 B2 | VALID | 71.06 | 3.434 | 70.67 | 0.83 / 1.31 | 0.04 / 0.55 |
| 720 A2 | VALID | 68.43 | 3.652 | 68.05 | 0.86 / 1.04 | 0.01 / 0.65 |
| 720 A3 | VALID | 69.29 | 3.575 | 68.65 | 1.00 / 1.39 | 0.07 / 0.82 |
| 720 B3 | VALID | 69.25 | 3.536 | 68.87 | 0.80 / 1.00 | 0.04 / 0.56 |
| 720 B4 | VALID | 68.25 | 3.613 | 67.87 | 0.93 / 1.29 | 0.07 / 1.62 |
| 720 A4 | VALID | 68.71 | 3.625 | 68.20 | 0.87 / 1.14 | 0.05 / 0.58 |
| 1080 A1 | VALID | 58.12 | 5.676 | 32.83 | 0.79 / 0.93 | 0.08 / 0.59 |
| 1080 B1 | **LEG-FAILURE** | 57.18 | 5.652 | 32.30 | 0.69 / 0.89 | 0.14 / 0.58 |

- Both discard legs (720 and 1080) ran and were not counted, as declared.
- **Load context:** with pacing off, the game itself saturates the CPU. The system was about 98–100 % busy, and the game used about 12.5–13.3 cores, roughly 10 of them PNG-encode workers.
- The per-process GPU counter reads **0.0 % for the game** on every leg, so the game's GPU load was not captured. The cause is not established.

**3. Readings**
- **m55 worker time (B legs):**
  - 720p: 1.16–1.33 ms per captured frame, 30–35 ms per measured pair.
  - 1080p: 2.18 ms per captured frame.
- **CPU by thread class (cores):** game 0.29–0.39; render 0.76–0.81; RHI 0.68–0.80; background workers 9.95–10.65. The A and B legs are indistinguishable.
- **Memory high-water, with the paced priors 17.5 / 19.4 / 39.4 / 49.8 / 66.4 MB kept beside it:**
  - 720p: 36.9 / 36.9 / 37.8 / 46.1 MB.
  - **1080p: 267.6 MB**, which is 99.7 % of the 256 MiB cap.
- **Writer backlog:** 19–29 frames at 720p; 141 frames at 1080p.
- **Drops:** 0 everywhere. No rows dropped, no late results, no closure timeouts.
- **Frame-time tails:**
  - Engine frame p95/p99 at 720p: 18.5–20.0 / 20.7–24.2 ms.
  - Writer latency p50 at 720p: 196–223 ms. At 1080p: about 2.5 s, with p99 about 3.7 s.
- **Required-pair yield:**
  - 720p: 20, 23, 24 and 21 measured out of 120. Unmeasured windows were `view_mismatch` 60–68, `unsupported_delivery` 22–28 and `mask_payload_missing` 5–10.
  - 1080p: 8 out of 60. Unmeasured windows were `budget_exceeded` 30, `view_mismatch` 9, `mask_payload_missing` 6, `unsupported_delivery` 5 and `predecessor_missing` 2.
  - The paced priors on this recipe measured 20 of 20, with neither `view_mismatch` nor `unsupported_delivery`.
  - On the `view_mismatch` rows the camera delta is exactly 0. The receipts show the mask was served from the view family one earlier than the colour frame. This is the G295 shape. The mechanism is not established.
- **Colour-completion latency (B legs):** 720p p50 200–227 ms (about 18 frames); 1080p p50 2276 ms (about 92 frames).
- **`census_cycles`:** 81–86 at 720p; 43–44 at 1080p.
- **Oracle:** exit 0 on all four 720p B legs.
- **c0 long-run reading, carried forward:** no measurable ON/OFF effect; the stage uses about 0.12 logical cores.
- **Declared predictions that missed:**
  - required-pair yield (declared 1.0);
  - worker ms per frame (lower than declared);
  - 1080p high-water (declared 40–110 MB);
  - 1080p writer throughput (32.3–32.8 PNG/s against a declared 36–44).
- The UNEXERCISED block is carried verbatim in section 8 of the report.

**4. Diagnostic pair vs the FPS-21.5 prior**
- **Not run.** The campaign halted before the diagnostic block.
- With the run log off, no counted leg had a stall inside its measurement window.

**5. Deviations**
- The run stopped at the declared halting class. Per the brief, I did not resume it or re-run any leg.
- I committed the docs after the halt. That moves the feature head past `ae56e17`, so any further window run needs a boundary re-issue. Any continuation already needs a ruling and probably a predictions amendment.
- Harness output is in `_reviews\081-37-evidence\cost\`, the default location. My files are under `_reviews\081-38-evidence\`:
  - `stage3-cost-report.md`, the Stage 3 cost report;
  - the read-only analysis scripts;
  - the console log;
  - two extracted copies of the m55 change-evidence predictions file (at `2125159` and `ae56e17`, used for the UNEXERCISED block).

**6. Commits pushed**
- `b25bab5` on `feat/m55-change-evidence`: the journal `docs/sessions/2026-09-26-081-38-code-m55-stage3-cost-run.md`, gotcha G301, and the CLAUDE.md status block.
- I used a scratch worktree and removed it afterwards.
- Main checkout is still `m51` `53bf725`. `master`, tags, containers and CaptureBench are untouched. Postflight had 0 problems, and the staged Game exe 002805CF was verified.

**7. NEEDS-DECISION**
1. **1080p cannot pass the B vacuity rule with pacing off on this recipe**, because the writer saturates and the feature hits its cap. Options:
   - run 1080p paced;
   - reshape or shorten the 1080p legs so the payload stays under the cap;
   - accept 1080p as UNRESOLVED on this evidence.
   Any of these needs a predictions amendment and a boundary re-issue.
2. **Does the 720p PASS stand at the workload actually reached?** Yield was 0.17–0.20, not the declared 1.0. The declared rule reads PASS. The game-thread margin is wide, but the workload assumption behind the prediction did not hold.
3. **Product finding for m51/M3:** with pacing off, yield collapses because the mask is served from the previous view family (`view_mismatch`) or not delivered (`unsupported_delivery`, stage mask). This extends G295.

**8. Wall-clock:** 05:22 to 05:35 local, about 13 minutes. Bench time was about 7 minutes, and no wait budget was used.
api-equivalent cost (not billed on Max): $6.10 | turns: 40 | running total across all briefs: $2,304.30
