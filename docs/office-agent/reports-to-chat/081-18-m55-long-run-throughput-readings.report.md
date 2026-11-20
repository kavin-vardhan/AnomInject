# REPORT for 081-18-m55-long-run-throughput-readings.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-18-m55-long-run-throughput-readings.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-24 01:07:15 | finished: 2026-09-24 02:39:38 | duration: 92 min | exit code: 0
context at last turn: 668495 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-18-m55-long-run-throughput-readings.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION: no long-run leg ran. Four <other-home-project> editors (UE 5.7, not ours) started on this machine at 01:32 IST, the same second as our first launch, and were still running at the brief's 60-minute bound. I never touched them, and the boundary is restored and proven.

## 2. Plain summary for the owner
- None of the long 1800-frame tests ran. Another project's Unreal editors (four of them, <other-home-project>) started on this machine and were still running after the 60-minute wait the brief allows, so I stopped.
- The first Lyra attempt had already been invalid before recording: the game respawned the player after placement, as happened last round.
- Everything is ready for a rerun: the test scripts, the predictions (frozen before anything ran) and a timing method checked against known answers.
- Re-reading last round's short 300-frame Lyra recording shows the frame writer already falling behind: 23.2 frames/s went in, 21.7 frames/s came out. The evidence stage's waiting grew in step with it; StackOBot stayed flat. This does not answer the long-run question.
- To get the answer, the <other-home-project> work needs to stop or be scheduled around, then this brief reruns unchanged.

## 3. Leg table

| Leg | Attempts | Fixture notes | Wall | Sustained fps | Frames written | Drops | ON: measured/refused · first budget refusal · high-water · worker ms |
|---|---|---|---|---|---|---|---|
| L1 Lyra ON | 1 of 3 used | A1 invalid before capture: L1 passed (0.115 cm, 0°); pre-capture L2 failed (`GA_AutoRespawn` after placement, 20:03:26Z). No `capture_start` was sent. | 87 s launch | — | 0 | — | — |
| L1 (attempt 2 onward), L2 OFF, L3 OFF, L4 ON, S1 ON, S2 OFF | 0 | NOT-RUN: foreign editors present from 01:33:29 to the stop at 02:33:35 | — | — | — | — | — |

## 4. Trends
None from 081-18. The only numbers I have are an offline re-read of 081-17R's banked 300-frame legs, labelled as such. Writer latency here means frame armed → PNG written, per frame.

| 081-17R leg | Armed fps | PNGs completed fps | Writer latency decile medians (ms) | Rule R1 | Ready-head wait (census lines) |
|---|---|---|---|---|---|
| Lyra G270 | 23.23 | 21.72 | 672, 792, 958, 1034, 1171, 1210, 1337, 1314, 1447, 1528 (2.8 ms/frame) | GROWTH | 0 → 801 ms over 11.7 s |
| Stack solid N | — | 24.3 | 378 … 395 | PLATEAU | 0–1 ms |
| Stack null N | — | 25.0 | 290 … 282 | PLATEAU | 0–1 ms |

- **Lyra lag series:** lag 12 → 53 while pending colours went 1 → 21. At 18 of the 20 census lines the head was pending with nothing to wait on.
- **Correlation:** the head's wait tracks the head's own writer latency, Spearman ρ = 0.95.
- **Armed rate:** frames are armed at about 23–25 fps, not 30, because there are 1.22–1.26 game ticks per captured frame.

## 5. ON vs OFF comparison
Not measured: the OFF legs never ran, so I have no uncertainty estimate.

## 6. Analysis
1. **Plateau or growth?** Unanswered for long runs. Applied to the banked 300-frame data, the predeclared rules read Lyra GROWTH (both writer latency and stage retention) and StackOBot PLATEAU. Extrapolating linearly to 1800 frames gives about 6 s writer latency and the 256 MiB cap reached near index 400. That is a projection, not a reading.
2. **Does evidence ON slow the writer or add drops?** Not measured; that is exactly what L2/L3 were for. The stage's own CPU is small (1.1 s of worker time across the 300-frame Lyra run, about 40–57 ms per measured pair), but the ON/OFF comparison itself did not happen.
3. **Best explanation (hypothesis, medium confidence; not demonstrated):**
   - *Measured:* on Lyra 1080p the PNG writer completes fewer frames than are armed, so its backlog grows.
   - *Read from engine source:* the stage worker and every PNG job are queued at the same Normal priority in one oldest-first queue, served by the 12 background task workers (14 task workers on this 16-thread CPU: 2 foreground, 12 background).
   - *Consequence:* each time the worker is re-dispatched it waits behind the writer's backlog. That fits "head ready but unprocessed", ρ = 0.95, and StackOBot's 0–1 ms.
   - *Caveats:* this hypothesis was formed from that same data, so the fit is consistency, not a test. It also cannot separate queueing from general CPU saturation.
4. **Smallest change, if a fix is needed (not implemented):** in `ScheduleLocked`, replace `Async(ThreadPool, …)` with `AsyncPool(*GThreadPool, …, nullptr, EQueuedWorkPriority::High)`. The worker then queues ahead of Normal writer jobs. Risks:
   - It does not fix the writer's own shortfall: writer memory and latency still grow, and end-of-run flush/closure timeouts remain.
   - The worker's pixel scans would run ahead of PNG jobs.
   - On Lyra it counts against the 14-job concurrency cap.
   
   A dedicated worker thread is the larger alternative.

## 7. Deviations and harness changes
- **Preflight attempt 1:** flagged one changed file, chat's scorecard `081-17-codex-vs-code-comparison.md`, edited 01:07 after 081-17R closed. I recorded it as a declared external update with both hashes and re-ran: 0 mismatches.
- **Runners:** derived from 081-17R's by asserted single-occurrence replacements. Changes: evidence root, bank prefix `M55L18_`, capture wait 600 s, QPC/UTC anchors, and a 1 Hz passive CPU/memory sampler.
- **New scripts:** drivers, analyzer, comparer and postflight. The analyzer passed a known-answer self-test on banked data (latency residual −1.6 to +1.2 ms against the stage's own figure; every refusal total reproduced exactly).
- **Restore/postflight checks narrowed after the stop:** they now refuse only on processes that can touch our folders and record foreign ones. The launch-time check is unchanged.
- **Docs commit** `5ab5e61` on `feat/m55-change-evidence`, made from a scratch worktree that has since been removed:
  - journal `docs/sessions/2026-09-24-081-18-code-m55-long-run-throughput.md`
  - predictions copied byte-identical to `docs/predictions/2026-09-24-m55-081-18-long-run-throughput.md`
  - CLAUDE.md status block
- **Memory:** saved a note about the foreign editors.

## 8. Boundary proof
`_reviews/081-18-evidence/postflight.json`: **"PASS: boundary restored (main m51 53bf725, Lyra caa68c6 with original 9 modules, all binaries byte-identical to preflight)"**, `problems: []`. It covers:
- 180 binaries, 1,782 prior artifacts and 14,031 bank files re-verified;
- CURRENT.md unchanged, predictions hash intact;
- remote feature `5ab5e61`, master `031a103`;
- no process of ours running; the four foreign editors are listed separately.

## 9. NEEDS-DECISION items
- **The bench.** The <other-home-project> workload (PIDs 153756, 96312, 76640, 104420) has to be stopped or scheduled around; this is the second round in a row it has blocked. Then rerun 081-18 as written: predictions and harness are unchanged, and L1 has 2 pre-capture attempts left, or starts fresh.
- **Or decide without the rerun.** The alternative is to rule on a build-3 scheduling change from the 300-frame evidence alone. That evidence cannot show the ON-vs-OFF writer effect or the long-run shape.

## 10. Wall-clock per step (IST)

| Step | Window | Duration |
|---|---|---|
| Reading and source reads | start not logged → 01:22 | — |
| Preflight (2 runs) | 01:22–01:24 | ≈ 2 min |
| Harness, analyzer self-test, predictions | 01:25–01:31:39 | ≈ 7 min |
| Lyra staging and L1 attempt 1 | 01:31:47–01:33:27 | ≈ 2 min |
| Waiting on foreign editors | 01:33:29–02:33:35 | 60 min |
| Restore, re-read, docs, postflight | 02:34–02:38 | ≈ 5 min |

Active work was about 16 min after the reading phase, plus the 60-minute wait. There was 1 physical launch and 0 capture starts.
api-equivalent cost (not billed on Max): $18.67 | turns: 119 | running total across all briefs: $2,040.49
