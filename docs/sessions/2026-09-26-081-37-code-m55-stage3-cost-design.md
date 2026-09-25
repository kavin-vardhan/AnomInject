# 081-37 — m55 Stage 3: the FPS 21.5 prior explained (PARTIAL), the cost campaign designed, predicted, built, proven and dry-run (Claude Code, Opus 5.5, 2026-09-26, bench-free)

## Goal
Brief 081-37, ruling `_reviews/081-37-chat-ruling-requal-complete-stage-3-cost.md` (Stage 3 released; cost gate of 081-03 unchanged). Explain
the FPS 21.5 prior from the bank; design the A/B/B/A pacing-off cost campaign at 720p and 1080p on StackOBot; declare its predictions,
statistic and validity rules before anything runs; build the harness on the 081-23 infrastructure; prove it both ways; dry-run it; lock it.
**Bench-free:** no game, window or leg was launched; no build; no plugin source change. The only processes started were two synthetic
Python programs of mine (the sampler's known-answer loads). Evidence: `_reviews/081-37-evidence/`.

## 1. The FPS 21.5 prior — PARTIAL
Full text in the predictions file §1. In short:
- **Established (bank):** `M55S1V6_MAIN4_E0` read 21.535 because `sustained_wall_fps` is a whole-session ratio. The entire 1.558 s excess
  is one stall across session_index 0→4 (883 / 111 / 238 / 527 ms). Frames 5–89 match MAIN1–3 to the millisecond. It is the only such start
  stall among 340 banked m55 sessions; the other 21 > 150 ms stalls are deliberate bench delays or two ~170 ms blips.
- **Excluded:** m55 (evidence was OFF) and shader compilation (`pending=0 incomplete=0` at si 0–3; prewarm 0.0018 ms).
- **Established (log + source):** during the stall the game thread took 6–215 ms per run-log line, against ≤ 2 ms in the sibling legs. The
  run log flushes to disk on every line, on the logging thread (`FlushFileBuffers`) → G300.
- **Not established:** why the disk or the process was slow for ~2 s. No load context exists for Stage-1 legs.
- **Handling:** gated legs run with the run log OFF; the game-thread metric counts CPU cycles; the first 60 frames are excluded; every leg
  carries a stall census and its load context; a `HELD-FPS-PRIOR` rule applies; and a **diagnostic pair** with the run log ON is declared
  (outside the gate).

## 2. Design (predictions `docs/predictions/2026-09-26-m55-stage3-cost.md`)
- **Recipe:** the accepted `C2_SOLID` / `C1_SOLID1080` / c0 recipe — `CB_GateLevel`, `StaticMeshActor_49`, `solid_swap`,
  `2 4 16 4 0`, native order, with the 081-23 fixture. It is the only accepted recipe exercised at both resolutions.
- **A/B difference:** exactly `IAI.Capture.ChangeEvidence 0` on A. Both sides also get `IAI.Capture.Pace 0`, `IAI.Bench.ChangeGate 0`
  (the base line's gate 14 is a bench device: a CRC and a log line per frozen mask) and `IAI.Capture.RunLog 0`.
- **Legs:** per resolution, one declared discard, then ABBA × 2 → 4 A + 4 B counted. 600 frames at 720p (30 events, under the m50 tag-pool
  limit of 55); 300 frames at 1080p. The run-end writer flush waits at most 5 s, and pacing-off saturates the 1080p writer (bank: 38–43 PNG/s
  against a 53–66 fps arm rate). Then the diagnostic pair.
- **Budget:** ~12–15 min expected, hard cap 60 min (exit 6, resume-safe). Waits come from the 45-min per-tag budget.
- **Game-thread ms:** the 10 Hz `QueryThreadCycleTime` of the thread described `GameThread`, at the 2688 MHz TSC rate, per engine frame, over
  session_index 60…N−1. The CSV profiler was rejected: its header row is written at capture end, and the runner force-kills the game.
  `GetThreadTimes` was rejected as tick-sampled (G299).
- **Writer throughput:** PNG completions per second in the steady window (081-24's definition).
- **Statistic:** Δ = mean B − mean A. The pooled A/A-and-B/B SD is the noise floor, t = 1.943 (one-sided 95 %, 6 df).
  - Game-thread clause: PASS iff the upper bound ≤ +1.000 ms.
  - Writer clause: UNRESOLVED if the drop is beyond uncertainty (upper bound < 0), or if under-resolved (t·SE > 5 % of the A mean — the
    G146 guard).
  - HELD-FPS-PRIOR if a leg's arm rate is below 0.75 × its condition median.
- **Validity:** any person evidence → INVALID-PERSON (stricter than 081-30 ruling 2c, as 081-37 orders for cost). Foreign load > 2.0 cores
  mean / 3.5 cores in any second / > 5 % GPU / > 20 MB/s I/O → INVALID-LOAD. A foreign editor → INVALID-FOREIGN. All three are re-queued,
  never averaged. Fixture-invalid attempts are capped at 3 → NOT-RUN; environment-invalid attempts at 5 → exit 4.
  - Mode → HARNESS-FAIL; integrity or vacuity → LEG-FAILURE. Both stop at the first failure.
  - An incomplete ABBA block is voided and re-run whole on resume.
- **Load baseline measured today:** 1.16 foreign cores at idle (Windows Widgets ~0.9); the limits sit about one core above it.

## 3. Harness (`_reviews/081-37-*`)
- `081-37-cost-sampler.py` — per-thread cycle sampler (Toolhelp, `OpenThread`, `QueryThreadCycleTime`, thread descriptions) and a 1 Hz load
  sampler (every process's CPU and I/O, PDH GPU 3D engine by pid, PhysicalDisk). All reads; nothing is injected or killed.
- `081-37-cost-lib.py` — imports the 081-23 lib unchanged and redirects its evidence root. It reuses `pp_gate`, `PersonMonitor`,
  `pp_evidence_attempt`, `fixture_errors_measure`, the budget file and `run_oracle`; the runner is the hash-locked `081-23-measure.ps1`,
  driven through `-ExtraExecCmds`. It adds the quiet and load gates, the leg runner, readings, mode/integrity/vacuity checks, the ledger,
  block voiding, the statistic and the verdict.
- `081-37-cost-window.py <tag>` — preflight against `081-37-evidence/cost-boundary.json`, gates, campaign, postflight, analysis, and oracle
  readings on the B legs.

## 4. Proofs (all as expected)
| proof | result | evidence |
|---|---|---|
| cycle counter is TSC-rate | long-slice trials 1.00–1.04 × registry per OS CPU-second; wall-based calibration is wrong (thread descheduled 21–25 % on an idle box) | `proof-cyclecounter.json` |
| game-thread sampler, known load | 4 ms busy / 6 ms idle → **3.745** ms/frame; 1 ms / 9 ms → **0.970**; worker 0.93 cores; thread found by `GameThread` description; the two loads separated by > 2.5 ms | `proof-sampler.json` (4/4) |
| analysis on real banked legs | c0 S1/S2: writer latency p50 331.5 / 330.5 ms (081-24: 333 / 331), throughput 24.74 / 24.88 PNG/s (24.71 / 24.84), high-water 66.39 MB, S1 yield 0.611 (the 55-of-90 tag-pool limit); QPC clock join 18 ms; the mode check **rejects** both (paced, gate 14, run log ON) | `proof-readings.json` (2/2) |
| walk, 12 scenarios + 13 gate units + 6 load-rule units | **64 of 64** | `dryrun/dryrun-results.json` |

Gate units, as the brief requires:
- +0.4 ms with a tight spread → PASS; +1.4 ms → UNRESOLVED; +0.6 ms with a wide spread (bound 1.15) → UNRESOLVED (control: tight → PASS).
- Writer 40 → 38 PNG/s → UNRESOLVED (drop); 40 → 39.95 inside the spread → PASS; noisy → UNRESOLVED (under-resolved); an increase → PASS.
- Boundaries: exactly +1.000 → PASS, +1.001 → UNRESOLVED. A leg at 0.6 × median → HELD-FPS-PRIOR. 3 A legs → UNRESOLVED (incomplete).
- A −0.5 % writer drop with a very tight spread → UNRESOLVED: the literal rule, flagged to chat.

End-to-end scenarios:
- **S1** full size (600/300 frames): 20 launches in order, every attempt gated, +0.4 ms recovered at both resolutions, PASS.
- **S2:** a foreign-load leg, a person-evidence leg and a foreign-editor leg are each INVALID, excluded from the statistic and re-run as T2.
- **S3 resume:** a stop before `B2` voids A1/B1, and the second invocation re-runs block 1 whole.
- **S4–S8** each halt and stay halted: writer flush timeout, evidence not effective, a vacuous B leg, wrong resolution, fixture exhausted.
- **S9:** 5 load-invalid attempts → exit 4, no NOT-RUN. **S10:** bench budget → exit 6. **S11 / S12:** UNRESOLVED end to end (+1.4 ms;
  writer drop).

**Found and fixed before use:**
- The walk showed the discard leg's condition was derived from its name (`'D'`), so it would have launched **evidence ON**. It is now
  declared evidence OFF.
- The sampler cached a thread's description at first sight and missed names set a moment later (G299).

## 5. Lock order (081-33 pattern)
The docs were committed and pushed from a scratch worktree outside the StackOBot tree. After that, `cost-boundary.json` was issued, pinning
that pushed head, the committed predictions' SHA-256 and every harness file. The preflight was then replayed offline, the window runner was
walked with a stubbed campaign, and the dry run was repeated. Those results are in the 081-37 report and evidence, not here, because this
commit is the head they pin. The 081-23 `prep-boundary.json` is untouched (still `03adc99`); the c0–c3 windows are closed.

## 6. For chat (does not block 081-38)
At 1080p the writer is saturated, and B's stage worker and canonical colour copy share its pool. A small real drop (predicted 1–3 %) may
therefore be *beyond uncertainty* under the literal rule and read UNRESOLVED. If chat wants a practical tolerance, it must be ruled
**before** 081-38 (it changes the lib → boundary re-issue). Without a ruling, the declared rule stands.

## State
Build 3 unchanged. Feature branch: this docs-only commit. NOT MERGED, NOT TAGGED. Main checkout `m51` `53bf725` untouched. Scratch worktree
removed at close.
