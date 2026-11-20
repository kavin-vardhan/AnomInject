# REPORT for 081-24-m55-window-c0-long-run.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-24-m55-window-c0-long-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-25 10:40:13 | finished: 2026-09-25 11:13:55 | duration: 34 min | exit code: 0
context at last turn: 544896 tokens (54% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-24-m55-window-c0-long-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

All six long-run legs ran and gave valid readings, the oracle passed on every evidence-ON session, and the setup is restored and proven. One harness bug, found during this window: the analysis step crashed after the legs, so I re-ran the analysis myself without changing any locked file (details in §7). I also held back the docs commit so the next windows can still start (§7).

## 2. Plain summary for the owner
- The six long tests (1800 frames each: four on Lyra, two on StackOBot) all ran. No other project's editor appeared.
- Memory use and picture-writing delay stayed level for the whole run on both games. The earlier worry, a backlog that keeps growing, did not happen on build 3.
- Turning the new evidence feature on did not slow picture-writing and did not lose any frames. All six runs wrote every one of their 1800 pictures.
- **No build 4 is needed.** The remaining three test windows (c1–c3) can run on build 3 as it is.
- A small bug in the test scripts stopped the automatic analysis. I re-ran the same analysis by hand and got the numbers.

## 3. Legs
Window 10:41:57–10:59:53 IST. The PC was quiet on the first two checks.

| leg | attempts / fixture | runner / capture time | sustained fps | speed_ratio | frames / drops | measured / required (refusals) | first budget refusal | high-water | worker ms |
|---|---|---|---|---|---|---|---|---|---|
| L1 Lyra ON | 1 | 2:50 / 72.1 s | 29.383 | 1.0210 | 1800 / 0 | 179/193 (14 `empty_region`) | none | 64.3 MB (si 855) | 8,367 |
| L2 Lyra OFF | 3 (A1, A2: host respawn between placement and capture start) | 2:34 / 74.9 s | 29.291 | 1.0242 | 1800 / 0 | — | — | — | — |
| L3 Lyra OFF | 1 | 2:28 / 72.6 s | 29.314 | 1.0234 | 1800 / 0 | — | — | — | — |
| L4 Lyra ON | 1 | 2:29 / 74.7 s | 29.315 | 1.0234 | 1800 / 0 | 168/168 | none | 134.8 MB (si 550) | 8,403 |
| S1 Stack ON | 1 | 1:44 / 73.0 s | 29.688 | 1.0105 | 1800 / 0 | 220/360 (140 `mask_payload_missing`) | none | 66.4 MB (si 17) | 9,084 |
| S2 Stack OFF | 1 | 1:30 / 72.6 s | 29.848 | 1.0051 | 1800 / 0 | — | — | — | — |

- **Drops:** no flush timeout, no failed or unresolved frame, no zero-byte or missing PNG on any leg.
- **Lyra host events:** bots kill and respawn the player during every Lyra capture (up to 965 cm camera moves). These are recorded, not treated as invalid, per the brief.
- **Per-pair outcomes, ON legs:**
  - L1: 311 valid; refused 1465 `mask_payload_missing`, 23 `empty_region`, 1 `first_frame`.
  - L4: 294 valid; refused 1501 `mask_payload_missing`, 2 `out_of_order_timeout`, 2 `predecessor_missing`, 1 `first_frame`.
  - S1: 875 valid; refused 924 `mask_payload_missing`, 1 `first_frame`.
  - Frames outside any labelled window carry no mask by design, which is why `mask_payload_missing` dominates on Lyra.
- **S1's 140 unmeasured pairs** are the known stencil tag-pool ceiling: 55 tag values, one kept per event, 90 events. There are 35 `TAG-POOL EXHAUSTED` lines, identical on S1 and S2.
- **Oracle (frozen `--change-oracle`):** exit **0** on L1 (179/179), L4 (168/168) and S1 (220/220). Compared = matched = measured, 0 mismatched.

## 4. Trends
Writer latency (arm to PNG written), median ms per decile:

| leg | d1 | d2 | d3 | d4 | d5 | d6 | d7 | d8 | d9 | d10 | slope ms/frame | R1 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| L1 ON | 491 | 517 | 502 | 527 | 534 | 546 | 579 | 536 | 587 | 569 | +0.05 | PLATEAU |
| L4 ON | 734 | 874 | 836 | 811 | 723 | 708 | 751 | 723 | 722 | 812 | −0.07 | PLATEAU |
| L2 OFF | 645 | 599 | 565 | 640 | 648 | 702 | 708 | 768 | 1047 | 1612 | +0.55 | GROWTH |
| L3 OFF | 635 | 630 | 572 | 705 | 667 | 640 | 709 | 706 | 731 | 791 | +0.10 | INDET. |
| S1 ON | 366 | 344 | 343 | 341 | 337 | 333 | 326 | 326 | 321 | 325 | −0.02 | PLATEAU |
| S2 OFF | 354 | 346 | 345 | 337 | 333 | 331 | 326 | 327 | 324 | 322 | −0.02 | PLATEAU |

Stage census (logged only when the high-water rises):

| leg | lag (latest − cursor) | pending colours | last census line | peak census |
|---|---|---|---|---|
| L1 | 10–17 | 1–5 | si 516 | si 855 |
| L4 | 11–32 | 1–12 | si 548 | si 550 |
| S1 | 7–12 | 0–3 | si 10 | si 17 |

- After those points, held memory never exceeded the peak (L1: 6 colours + 7 masks; L4: 15 + 5; S1: 5 + 12). So lag is bounded there, not sampled.
- The issue-to-colour lag on window rows is flat per decile: L1 15–18 frames, L4 22–28, S1 10–11.

## 5. ON vs OFF
**How I estimated noise:**
- Lyra ran in ABBA order with two legs per condition. The noise is the spread between two legs of the same condition. The predeclared rule says a difference is real only if the ON and OFF ranges do not overlap.
- StackOBot has one leg per condition. The noise band for latency is the within-leg spread of the d2–d10 medians (24 ms). Rate differences cannot be quantified at n = 1.

| metric | Lyra ON (L1, L4) | Lyra OFF (L2, L3) | ON − OFF | same-condition spread | verdict |
|---|---|---|---|---|---|
| sustained fps | 29.383, 29.315 | 29.291, 29.314 | +0.05 | 0.07 / 0.02 | ranges miss by 0.001 fps, and ON is the faster one |
| PNG rate | 25.08, 24.30 | 24.19, 24.95 | +0.12 | 0.78 / 0.76 | not distinguishable |
| latency p50 ms | 535, 763 | 688, 675 | −32 | 228 / 13 | not distinguishable |
| game CPU cores | 12.7, 13.3 | 13.1, 13.5 | −0.3 | 0.5 / 0.4 | not distinguishable |
| drops | 0, 0 | 0, 0 | 0 | — | — |

- **StackOBot:** latency p50 333 vs 331 ms (inside the 24 ms band); PNG rate 24.71 vs 24.84.
- **StackOBot speed_ratio** 1.0105 vs 1.0051. This comes from 3 vs 1 stalls over 100 ms; the median per-frame excess is about 0 on both. It cannot be attributed at n = 1.
- **Stage worker cost:** about 0.12 logical cores (4.7–5.0 ms per frame).

## 6. Analysis
**1. Plateau or growth: PLATEAU on both hosts, by the predeclared rules R1 and R2.**
- Writer latency is flat on L1, L4 and S1.
- The high-water was reached in the first half of each run and never exceeded afterwards.
- Zero `budget_exceeded`, zero `closure_timeout`.
- L2's GROWTH (evidence OFF) sits in deciles 9–10. There, other programs on the PC rose from about 2 to 3.4–3.9 cores with the system at 100 % busy, the game dropped to 12.2 cores, and PNG size stayed at 2.71 MB. That is an association with outside load; with evidence OFF it cannot be the stage.

**2. Does evidence ON change writer throughput or drops? No, beyond noise.**
- Sustained fps is "distinguishable" only by the letter of the rule (a 0.001 fps gap), and ON is the faster side.
- Every other metric overlaps. Drops are 0 everywhere.

**3. `budget_exceeded`, `closure_timeout` and late results on the long ON runs:**
- `budget_exceeded`: none. `closure_timeout`: none.
- L4 has 2 late results, at si 430 and si 1548. Those PNGs finished after 7 and 4 later frames (file times), so the stage refused them as `out_of_order_timeout`, as designed. Each successor got `predecessor_missing`, and the late colour was logged as a `LATE-REJECT` diagnostic.
- Neither index was in a measured window, so no required pair was lost.

**4. Explanation and change:**
- **Source (read):** the writer (`AnomalyAsyncWriter.cpp:23`) and the stage worker (`AnomalyChangeStage.cpp:413`) are both `Async(ThreadPool)` at Normal priority. That maps to BackgroundNormal (`QueuedThreadPoolWrapper.h:460`), which runs only on the 12 background workers of this 16-logical-core machine (`Cores=10` physical).
- **Hypothesis H-Q, NOT SUPPORTED by rule R4:** the "ready but unprocessed" head is the worker waiting in that queue behind PNG jobs.
  - L4 fits it: 11 of 13 census lines show a head with all inputs in, wait median 185 ms (max 438), ρ 0.99 against the head's own writer latency.
  - L1 does not: 4 of 6 lines (0.67, below the 0.70 threshold), median wait 9 ms.
- **Why the plateau:** stage memory follows the writer backlog. Today the writer kept pace with the arm rate on every Lyra ON leg, so the backlog stayed at 15–25 frames.
- **Build 2 comparison:** the 300-frame build-2 run (081-17R) armed 23.2 fps but wrote only 21.7 PNG/s and grew from 672 to 1528 ms. The writer code is identical between build 2 and build 3, and today's first 300 frames show no such growth. **Cause not established.** 081-17R had no process sampler and ran 5 minutes after a three-target build.
- **Change: none needed.** If stage memory ever has to be decoupled from the writer backlog, the smallest change is one call at `AnomalyChangeStage.cpp:413`: `AsyncPool(*GThreadPool, …, nullptr, EQueuedWorkPriority::High)`.
  - It would remove the head's queue wait, but not the head's own writer latency.
  - Risk: about 5 ms per frame of stage arithmetic, run under the stage lock, would jump ahead of PNG jobs on an already saturated CPU.
  - Not implemented.

**Predictions (081-18 P1–P20 on build 3): 7 of 20 met** (P2, P9, P13, P14, P16, P18, P19).
- The Lyra-growth family missed: P1, P3–P7, P10, P11.
- Also missed:
  - P8: L1 had 46 events, above the predicted 45.
  - P12: sustained fps counts as "distinguishable" by the rule.
  - P15: the tag-pool ceiling.
  - P17: speed_ratio 1.0105, above the 1.010 bound.
  - P20: R4 above.

## 7. Deviations
1. **The c0 analyzer crashed on the first OFF leg.** `081-23-c0-analyze.py:210` calls `.pop` on `a['evidence']`, which is the string `"OFF"` on OFF legs; its self-test only covered ON legs. `chunk0.py` throws away the analyzer's exit code, so the window still reported exit 0.
   - I left the locked files untouched. `_reviews\081-24-evidence\081-24-analyze-driver.py` imports the analyzer unchanged (hash-checked) and only replaces its main loop. The comparator runs from a copy with one checked edit (its input folder), recorded in `analysis\derivation.json`.
   - Written up as gotcha G288. c1–c3 go through `run_sequence`, where a crash does exit non-zero, so they don't have this problem.
2. **Docs commit held back.** Every window's preflight requires the feature branch head to be `a745ee5`, so pushing would have made c1–c3 refuse to start. The commit (journal, CLAUDE.md status block, G288) is saved as `_reviews\081-24-evidence\docs-pending\0001-docs-m55-081-24-window-c0-long-run-on-build-3-both-h.patch`; `git apply --check` exits 0 against `a745ee5`. The branch was put back to `a745ee5`, which equals origin, and nothing was pushed.
3. **Oracle and extra readings.** The c0 harness has no oracle step, so I ran the frozen copy myself (`081-24-oracle.py`). The extra analyses (`081-24-deep.py`, `081-24-hitch.py`) are readings only.
4. **Waiting.** The Monitor tool's bash has no `grep` here, so I waited on the legs with PowerShell loops instead.

## 8. Boundary proof
- `_reviews\081-23-evidence\c0\postflight.json`: **"PASS: main m51 53bf725, Lyra caa68c6 with 9 original modules, staged exe 002805CF, every binary/container/harness file unchanged"**, `problems: []`.
- Re-checked after my analysis in `_reviews\081-24-evidence\boundary-final.json`: **PASS**, `problems: []`.
  - All 76 c0 evidence files and all 13,823 bank files are unchanged since postflight.
  - Feature branch and origin at `a745ee5`; master `031a103`; m51 `53bf725`; scratch worktree removed.
- Still running: the per-user UnrealTraceServer (PID 33660, started with our first launch at 10:42:38, shared across engines). The harness treats it as harmless and it was not stopped.

## 9. NEEDS-DECISION
None. For chat's ruling:
- c1–c3 on build 3 unchanged, as recommended in §6.
- When to apply the held docs patch: after c3, or after chat updates the prep boundary.

## 10. Wall-clock (IST)

| step | time |
|---|---|
| Reading | 10:40–10:42 (~2 min) |
| Preflight and quiet wait | 10:41:57–10:42:28 (31 s) |
| Lyra legs, incl. stage, two invalid L2 attempts, restore | 10:42:30–10:55:29 (~13 min) |
| StackOBot legs | 10:55:30–10:58:46 (~3 min) |
| Harness analysis crash and postflight | 10:58:46–10:59:53 |
| Re-analysis, oracle, extra readings | 11:00–11:07 (~7 min) |
| Docs patch, boundary re-proof, memory | 11:07–11:13 (~6 min) |
| **Total** | **~33 min** |

Not needed this run, but for the owner: several MCP connectors (Gmail, Calendar and others) need authorizing, and the PDF viewer failed to connect.
api-equivalent cost (not billed on Max): $14.61 | turns: 102 | running total across all briefs: $2,145.22
