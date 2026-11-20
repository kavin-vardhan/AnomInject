# REPORT for 081-39-m55-cost-amendment-1-paced.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-39-m55-cost-amendment-1-paced.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-26 05:37:59 | finished: 2026-09-26 06:11:25 | duration: 33 min | exit code: 0
context at last turn: 618062 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-39-m55-cost-amendment-1-paced.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner**
- The new headline cost test is written down: it runs the capture at its normal paced 30 fps, at 720p and 1080p, with the same pass/fail maths as before and no new tolerance.
- The earlier pacing-off results are kept as labelled stress readings and will not be re-run: 720p passed, 1080p is unresolved.
- The test harness is updated and every check the ruling asked for behaves as intended. All of it ran without the game.
- Everything is committed and pushed, the lock is re-issued, and the offline pre-check is clean. The next bench run (081-40) should take about 18–19 minutes.

**3. Block P design** (AMENDMENT 1 in `docs/predictions/2026-09-26-m55-stage3-cost.md`)

*Legs*
- For each resolution, 720p then 1080p: `S3_P_<res>_DISC` (a declared discard with evidence OFF, never counted), then `A1 B1 B2 A2`, then `A3 B3 B4 A4`.
- Then the diagnostic pair `S3_DIAG_A/B`, exactly as declared in 081-37 (pacing off, 720p, run log ON), outside every gate.
- Same recipe as 081-37. The only A/B difference is still `IAI.Capture.ChangeEvidence 0` on A.
- Pacing ON at 30 fps comes from the runner's own base line (`IAI.Capture.Pace 1, IAI.Capture.Fps 30`). Block P appends only `IAI.Bench.ChangeGate 0, IAI.Capture.RunLog 0`.
- The mode check reads each leg's own log and summary: paced, Pace echo ON, Fps echo 30, `target_fps` 30, gate 0, run log OFF. Any mismatch is a HARNESS-FAIL.

*Duration: 600 frames at both resolutions*
- The paced recipe captures 24.75–24.95 fps in the bank, so a leg is about 24 s of capture with a 540-frame steady window.
- That is 30 events, well under the 55-event tag-pool limit (the c0 1800-frame leg measured exactly the first 55 events).
- Paced, the 1080p writer keeps up, so the 300-frame limit that pacing-off needed does not apply.

*Gate P*
- Same statistic as before.
- Game thread: PASS only if the upper bound is ≤ +1.000 ms.
- Writer: UNRESOLVED on a drop beyond uncertainty; the 5 % resolution guard stays.
- Every counted leg must have 0 drops.
- Pacing held: B's arm rate must not be below A's beyond uncertainty.
- A B leg with measured-pair yield below 0.9, or with no required pairs, is a LEG-FAILURE. The yield test uses integer maths (10 × measured ≥ 9 × required), and the old 20-pair floor is kept.

*Expected values (from the paced bank)*

| reading | P 720p | P 1080p |
|---|---|---|
| verdict | PASS | PASS |
| game-thread Δ (B−A) | within ±0.3 ms, upper bound ≤ +0.4 | same |
| game-thread ms per engine frame | 2.5–5.0 | 3.5–7.0 |
| arm rate (captured fps) | 24.6–25.0 | 24.5–25.0 |
| writer throughput | arm rate ±1 % | arm rate ±1 % |
| writer latency p50 | 140–260 ms | 300–450 ms |
| backlog high-water | ≤ 12 frames | ≤ 20 frames |
| latency trend | no growth | no growth |
| m55 high-water | 15–40 MB | 55–90 MB |
| B yield | 1.0 (120 of 120) | 1.0 (120 of 120) |
| drops | 0 | 0 |

*Declared risks*
1. The pacing-held and writer clauses have no tolerance, and pacing makes the arm rate nearly constant. c0's evidence-ON leg ran 0.55 % slower than its OFF leg, but with the run log ON. The prediction is that this gap disappears with the run log off.
2. `S3_DIAG_B` keeps the 20-pair floor, and pacing-off 720p B legs measured only 20–24 of 120 pairs. It could stop the run with a LEG-FAILURE, but only after block P is complete.
3. Under pacing, the game-thread metric includes the pacer's short wait loop (see the pacer proof below).

*Bench time*
- Each paced leg is about 43–46 s; with gates, about 50–53 s per launch.
- Expected total is about 18–19 minutes. Campaign time is capped at 42 minutes (2520 s), so the whole window stays inside 45.

**4. Proof table** (all under `_reviews\081-39-evidence\`)

| proof | expected | actual | evidence |
|---|---|---|---|
| gate P, +0.4 ms with a tight spread | PASS | PASS | `dryrun-post-lock\dryrun-results.json` |
| gate P, +1.4 ms | UNRESOLVED | UNRESOLVED | same |
| gate P, +0.6 ms with a wide spread | UNRESOLVED | UNRESOLVED (tight-spread control reads PASS) | same |
| writer drop beyond uncertainty | UNRESOLVED | UNRESOLVED | same |
| pacing not held (B 24.80 vs A 25.00 fps) | UNRESOLVED | UNRESOLVED | same |
| any drop on a counted leg | UNRESOLVED | UNRESOLVED; end to end, a flush timeout halts the leg and gate P reads UNRESOLVED | same |
| yield 0.89 (89 of 100) | LEG-FAILURE | LEG-FAILURE; the row says "89/100 = 0.89 < 0.9" | same |
| yield 0.90 (90 of 100) | valid | VALID and counted | same |
| resume on a copy of the real 081-38 ledger | `S3_P_720_DISC` first; no finished leg re-launched | `S3_P_720_DISC_T1` first; exactly 20 launches (P, then diagnostic); 0 pacing-off legs launched; all 12 rows from 081-38 byte-identical; a re-run launches 0; a stop mid-1080p re-launches nothing already done | same (R1–R4) |
| stress readings re-read from the real ledger | 720p PASS; 1080p UNRESOLVED with its cause | 720p PASS (Δ −0.0827, upper −0.0024, yield 0.17–0.20); 1080p UNRESOLVED with its halting row | same |
| paced bank legs reproduce published values | yield, latency, high-water match | 7 of 7: c0 S1 331.5 ms / 24.739 PNG/s / 66,392,064 B / 220 of 360 (published 333 / 24.71 / 66.39 MB / 220 of 360); c2 20 of 20 at 18.43 / 15.67 MB; c1 20 of 20 at 62.24 MB; c2/c1 writer latency 164.3 / 351.7 ms against the legs' own 165 / 352 | `proof-readings-paced.json` |
| pacer replica (a synthetic process, not the game) | +1.4 ms of work reads ≥ ~1 ms | +1.281 (unpaced control +1.294); 6 of 7 checks as expected | `proof-pacer.json` |
| dry run, before and after the lock | all pass | 112 / 112 and 112 / 112 | `dryrun\`, `dryrun-post-lock\` |
| window walk and offline preflight replay | all green | 11 / 11; replay 0 problems | `window-walk.json`, `preflight-replay-final.json` |

The one pacer check that missed was my own prediction of the mechanism. Reading the source, I expected the pacer's final busy-wait to absorb small increases in game-thread work. Measured on the replica, `Sleep` overshoots by 1.6–2.0 ms, so the busy-wait is a constant ~0.2 ms per frame and absorbs nothing. The cycle metric therefore tracks work changes the same way paced and unpaced, and gate P can fail on a real 1.4 ms cost. Recorded as G303.

**5. Boundary**
- Feature head `ae56e17` → `805f317`.
- Predictions `fe52e4fa` → `6230506a`; the byte-identical copy is `081-39-evidence\predictions.md`.
- Cost lib `cfe4cd1f` → `e5f0d9a2`; window runner `90ada58f` → `3a9d5fa4`. The other 12 harness files are unchanged.
- The previous boundary is kept at `081-39-evidence\cost-boundary.before-081-39.json`.
- Dry run after the lock: 112 of 112. Preflight replay: 0 problems.
- 081-40 invocation: `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\081-37-cost-window.py 081-40`
  - budget tag `081-40`;
  - same ledger folder, `_reviews\081-37-evidence\cost\`;
  - 081-38's generated outputs are kept as `*.before-081-40.json` before they are regenerated.

**6. Deviations**
- The campaign cap is 42 minutes, not 45, so postflight, oracle and analysis still fit inside the 45-minute window.
- Pace and Fps are not appended again for block P, because the runner's base line already sets them. The mode check verifies the values that actually took effect.
- Additions the brief did not ask for:
  - the pacer-replica proof and gotcha G303;
  - the window now keeps 081-38's outputs before overwriting them, and does not re-run the oracle on the retired pacing-off legs.
- My first banked-leg checker compared two different populations (every frame vs. pair rows only). It was replaced with a like-for-like check before any verdict was read, and the first console output is kept.
- The m51 branch is off-limits, so the requested cross-reference from the m51 notes lives in G295's m51 FUTURE note on the feature branch, pointing to G302.
- My first commit attempt passed the message as arguments and committed nothing. I recommitted from a message file.

**7. Commits pushed**
- `805f317` on `feat/m55-change-evidence` (local == origin): AMENDMENT 1, the 081-39 journal, G302 and G303, the G295 cross-reference, and the CLAUDE.md status block.
- The scratch worktree is removed. The main checkout is still `m51` `53bf725`.
- `master`, tags, containers, CaptureBench and `ToCodex\` are untouched. The real 081-38 evidence folder was verified unchanged against its own manifest.

**8. NEEDS-DECISION**
None. The three risks in section 3 are declared and do not block 081-40.
api-equivalent cost (not billed on Max): $16.45 | turns: 114 | running total across all briefs: $2,320.75
