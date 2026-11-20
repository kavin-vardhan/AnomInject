# REPORT for 081-40-m55-stage3-cost-run-paced.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-40-m55-stage3-cost-run-paced.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-26 06:12:55 | finished: 2026-09-26 06:40:17 | duration: 27 min | exit code: 0
context at last turn: 431458 tokens (43% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-40-m55-stage3-cost-run-paced.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner**
- At normal paced 30 fps capture, m55 costs **no measurable game-thread time** at 720p or 1080p. The worst-case bounds are −0.07 ms and +0.11 ms per frame, against a 1 ms budget. Both resolutions **PASS**.
- It **does not slow image writing**: PNG output keeps up with capture at about 25 frames per second on both sides.
- It **does its full workload**: every B leg measured 120 of 120 required pairs, with 0 dropped frames.
- Memory stays small: about 20 MB at 720p and about 68 MB at 1080p, against its 256 MiB budget.
- The earlier pacing-off results stay as labelled stress readings. At 1080p with pacing off it runs out of room and refuses pairs honestly, so the advice is to use paced capture.

**3. Gate P per resolution** (declared statistic: 4 A + 4 B legs, one-sided 95 %)

| res | game-thread Δ (B−A) ms/engine frame | upper vs bound | clause | writer Δ PNG/s | upper (≥ 0) · half-width vs 5 % limit | clause | pacing held: arm Δ, upper | drops | B yield | **verdict** |
|---|---|---|---|---|---|---|---|---|---|---|
| 720p | −0.1154 | **−0.0718** ≤ +1.000 | PASS | +0.0347 | +0.0782 · 0.0435 ≤ 1.2488 | PASS | −0.0003, +0.0013 → PASS | 0 on all 8 legs | 120/120 on each B leg | **PASS** |
| 1080p | −0.0371 | **+0.1100** ≤ +1.000 | PASS | −0.0137 | +0.0291 · 0.0427 ≤ 1.2493 | PASS | −0.0023, +0.0033 → PASS | 0 on all 8 legs | 120/120 on each B leg | **PASS** |

Headline overall: **PASS**. No FPS-prior hold fired.

**4. Per-leg table** (steady window si 60–599)

All 16 counted legs were VALID on attempt 1, with a clean mode check, no person evidence and 0 drops. Foreign CPU was 1.04–1.28 cores, foreign GPU ≤ 0.07 %, foreign I/O ≤ 2.1 MB/s.

| leg | arm fps | GT ms/eng frame | writer PNG/s | lat p50 ms | backlog high-water | system busy % | game cores |
|---|---|---|---|---|---|---|---|
| P_720 A1/A2/A3/A4 | 25.068–25.070 | 3.780/3.779/3.772/3.715 | 24.975–24.977 | 162.8–163.1 | 6–7 | 34.2–35.9 | 3.27–3.39 |
| P_720 B1/B2/B3/B4 | 25.067–25.070 | 3.634/3.606/3.677/3.666 | 24.974–25.069 | 162.8–163.2 | 6–7 | 34.8–35.9 | 3.32–3.43 |
| P_1080 A1/A2/A3/A4 | 25.062–25.071 | 5.941/6.068/5.981/6.131 | 24.928–25.025 | 318.4–330.0 | 11–14 | 64.4–66.0 | 7.57–7.88 |
| P_1080 B1/B2/B3/B4 | 25.062–25.070 | 5.950/5.835/6.099/6.087 | 24.969–24.977 | 322.2–334.5 | 12–15 | 64.2–67.7 | 7.69–8.12 |
| DIAG_A / DIAG_B | 62.665 / 61.945 | 4.594 / 4.513 | 62.549 / 61.715 | 200.5 / 201.2 | 16 / 16 | 95.7 / 95.1 | 12.06 / 11.90 |

**5. Readings**
- **Worker time** (m55, off the game thread):

  | | per captured frame | per measured pair |
  |---|---|---|
  | 720p B | 2.46–2.52 ms | 12.3–12.6 ms |
  | 1080p B | 7.22–7.42 ms | 36.1–37.1 ms |

- **CPU by thread class:**
  - game thread 0.11 cores at 720p and 0.18 at 1080p;
  - render 0.21 / 0.28, RHI 0.18 / 0.24;
  - bg_worker, B minus A: +0.07 cores at 720p (2.69 vs 2.61) and +0.23 at 1080p (7.01 vs 6.78).
- **m55 high-water against the 256 MiB cap:**
  - 720p 19.35–20.28 MB (7.2–7.6 %);
  - 1080p 56.02–68.47 MB (20.9–25.5 %);
  - priors: 17.5 / 19.4 / 39.4 / 49.8 / 66.4 MB;
  - for comparison, the pacing-off 1080p stress leg reached 267.6 MB.
- **Writer backlog and trend:** high-water 6–7 frames at 720p and 11–15 at 1080p. No growth on any P leg: the largest latency slope was +0.64 ms/s, and every backlog slope was ≤ 0.018 frames/s.
- **Drops:** 0 on every leg.
- **Latency percentiles:**

  | | writer p95 | writer p99 | engine-frame ms p50 / p95 / max |
  |---|---|---|---|
  | 720p | 168–170 ms | 171–174 ms | 33.33 / 35.6–35.8 / ≤ 38.0 |
  | 1080p | 351–375 ms | 366–388 ms | 33.33 / 42.2–43.9 / ≤ 56.9 |

  No stall on any leg.
- **Required-pair yield:** 120/120 on all 8 B legs. The only row reasons are `first_frame` 1 and `mask_payload_missing` 119–120 on non-required rows. There were 0 `view_mismatch`, `unsupported_delivery`, `budget_exceeded`, late, dropped or closure-timeout rows.
- **Colour-completion latency:** p50 164 ms (5 frames) at 720p and 324–336 ms (10 frames) at 1080p.
- **`census_cycles`:** 132–135 at 720p and 119–123 at 1080p, per 600 frames.
- **Load context:** in the table above. The game-process GPU counter read **0.0 % again on every leg**. This is an instrument limitation; the cause is not established.
- **c0 long-run reading (081-24, carried):** no measurable effect; the stage takes about 0.12 cores. Block P is consistent with that, now at full yield.
- **UNEXERCISED block:** verbatim in report §9. The source file at `805f317` is byte-identical to its `2125159` version.
- **Oracle (a54):** exit 0 on all 13 B legs.

**6. Diagnostic pair vs the FPS-21.5 prior**
- **No stall** anywhere in either leg: the stall census is empty from si 0, and the largest engine frame was 93 ms. MAIN4's 1.56 s start stall **did not reproduce (n = 2)**. That neither confirms nor refutes the run log as its cause.
- **The declared cycles prediction MISSED.** It said within ±0.3 ms of the run-log-OFF stress legs; the reading was **+0.987 ms (A) and +0.989 ms (B)**.
- **Engine time** rose +1.19 / +1.48 ms per frame, which hits the declared +0.2…+10 ms.
- **DIAG B − A game thread:** −0.081 ms, n = 1 each.
- **Run log volume:** 7,279 lines (A) and 7,684 lines (B, of which 454 were `MASK-SERVED`).
- **DIAG_B yield:** 38 of 120, which clears the floor of 20, so declared risk 2 did not fire.
- Recorded as **G304**: turning the run log on costs about 1 ms of game-thread CPU per frame, and the cause is not established. The gated block P ran with the run log off, as the client configuration does.

**7. Deviations**
- **Console log location.** I tee'd the window's console output to a new file in the `_reviews` root, then moved it to `_reviews\081-40-evidence\window-run-1.console.txt`. No existing file was touched.
- **Report generator.** `make_report.py` is new, adapted from 081-38's. Its first draft had two wrong sentences, both fixed before commit:
  - it said the ledger held 18 unchanged pacing-off rows; it holds 12;
  - it said the stress bound stays the sharpest; the paced bound is tighter.
- **Arm-rate prediction (A1.8) missed.** Declared 24.5–25.0; read 25.06–25.07. The band came from whole-session ratios, while the gate reads the steady-window rate. Reported as a miss, not refitted; it has no verdict effect.
- **The feature head now sits past the cost boundary** (`ab634f5` vs `805f317`). Any further bench window needs a boundary re-issue first.

**8. Commits pushed**
- `ab634f5` on `feat/m55-change-evidence` (local == origin): the 081-40 journal, G304 and the CLAUDE.md status block.
- The scratch worktree is removed. The main checkout is still `m51` `53bf725`, and `master` is unchanged at `031a103`.
- Tags, containers, CaptureBench, `ToCodex\` and all 15 harness files (hashes verified) are untouched.
- Evidence: `_reviews\081-40-evidence\{stage3-cost-report.md, make_report.py, window-run-1.console.txt}` and `_reviews\081-37-evidence\cost\`.

**9. NEEDS-DECISION**
None. One item for chat to note: the diagnostic miss (G304) is a reading outside the gate and does not affect gate P.

**10. Wall-clock**
- Session from 06:13 to 06:40 IST, about 27 minutes.
- Bench window from 06:13:59 to 06:33, about 20 minutes.
- No wait budget spent.
api-equivalent cost (not billed on Max): $7.16 | turns: 45 | running total across all briefs: $2,327.91
