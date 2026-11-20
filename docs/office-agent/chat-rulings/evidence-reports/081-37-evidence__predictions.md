# m55 Stage 3 — the cost campaign on build 3: predictions, statistic and validity rules (declared 2026-09-26, 081-37, before any leg)

Author: Claude Code (Opus 5.5), brief 081-37 (bench-free). Governing ruling: `_reviews/081-37-chat-ruling-requal-complete-stage-3-cost.md`
(cost gate of 081-03 §Stage 3, unchanged). Build 3 = `85c99b3`, Game exe **002805CF** (unchanged; no build, no source change).
Harness: `_reviews/081-37-cost-window.py`, `081-37-cost-lib.py`, `081-37-cost-sampler.py`, plus the reused, hash-locked 081-23 runner
`081-23-measure.ps1`. Evidence of the run: `_reviews/081-37-evidence/cost/`. This file is declared **before** any cost leg runs; a byte-identical
copy is `_reviews/081-37-evidence/predictions.md` and the committed file's SHA-256 is pinned in `_reviews/081-37-evidence/cost-boundary.json`.

## 0. The one-line gate (081-03, unchanged)

Per resolution (720p and 1080p, StackOBot only): **PASS iff the one-sided 95 % upper bound of the game-thread delta (B − A) is ≤ +1.0 ms
per engine frame AND writer throughput is unchanged within uncertainty.** Anything else is **UNRESOLVED**, never PASS. Overall PASS iff both
resolutions PASS. A = build 3 with evidence OFF (`IAI.Capture.ChangeEvidence 0`); B = build 3 with evidence ON (compiled default). Same binary,
same launch; the ExecCmds differ by exactly that one token.

## 1. The FPS 21.5 prior (carried since 081-05) — classified **PARTIAL**, bench-free

**What the number is — ESTABLISHED** (bank `M55S1_M55S1V6_MAIN4_E0`, `session_20260921-144844`, evidence OFF, paced, 720p, MainWorld):
- `sustained_wall_fps` 21.535 = 30 / `speed_ratio` 1.3930, a **whole-session** ratio (wall span 5.526 s over a 3.967 s game span).
- The whole excess (1.558 s) sits in **one start-of-capture stall across session_index 0→4**: engine-frame deltas 883.3, 111.4, 238.0, 526.9 ms
  (the last spans 3 ticks). Every later frame (si 5–89) has the same cadence as its three siblings MAIN1–3 (median 33.4–33.5 ms; the same
  15 settle gaps of ~97–100 ms at the same indices). It is **not a sustained rate**.
- It is **unique in the bank**: of 340 banked m55 sessions of ≥ 10 frames, 22 carry a > 150 ms/engine-frame stall; 19 are deliberate bench
  delays (G7/G15 at si 10, G8/G13 at si 9), 2 are 160–180 ms blips in the 1800-frame `S1_STACK_ON` leg, and MAIN4 is the only start stall.
- **Not m55** (evidence was OFF on that leg). **Not shader compilation**: `Capture(m47): SHADERS pending=0 incomplete=0` at si 0–3 and
  `shader_prewarm_ms` 0.0018 (its siblings 0.0017–0.0020).

**Where the time went — ESTABLISHED (timestamps and source), cause of the slowness NOT ESTABLISHED:**
- In MAIN4's run log the game thread took **6–215 ms per log line** from 09:18:44.48 to 09:18:46.50 UTC — on the configuration echo lines
  StartRun emits back-to-back with trivial work between them, which MAIN1 emits within ≤ 2 ms of each other. After 09:18:46.50 the gaps return
  to ≤ 1 ms. Census `CYCLE 1 COST tagBlockMs=329.04` vs MAIN1's 0.87 (that timer encloses three log lines).
- Source: the m38 run log (`FAnomalyRunLog`, `CanBeUsedOnAnyThread() = true`) writes **every line with `Writer->Flush()`**
  (`AnomalyRunLog.cpp` `WriteLineLocked`) → `FArchiveFileWriterGeneric::Flush` → `IFileHandle::Flush` → **`FlushFileBuffers`**
  (`WindowsPlatformFile.cpp:755-758`) — a synchronous disk flush **on the thread that logs**, the game thread for StartRun's lines.
  The legs ran delivery OFF, so the run log was ON (`auto, from delivery=off`).
- NOT ESTABLISHED: what made the process/disk slow for ~2 s (Stage-1 legs recorded no load or disk context); and whether each gap was the
  flush itself or the game thread being descheduled (timestamps exist only at line granularity). Candidates, none measured: write-back of the
  previous leg's bank copy, an anti-virus scan of fresh files, a foreign process burst.

**Handling in the cost run (declared now):**
1. **The gated legs run with the run log OFF** (`IAI.Capture.RunLog 0`, A and B alike). That removes the only per-line synchronous disk
   flush on the game thread — the mechanism the stall ran through — and it is the client configuration (delivery mode turns the run log off).
2. **The game-thread metric counts CPU cycles**, so a thread blocked in a flush or descheduled is not charged (§3.1).
3. **Warm-up exclusion**: the first 60 captured frames of every leg are outside the steady window (MAIN4's stall sat at si 0–4).
4. **Stall census on every leg**: every engine frame > max(150 ms, 3 × the leg median), with its session_index, is recorded beside the leg's
   load context.
5. **FPS-prior hold**: if any counted leg's steady-window arm rate is below **0.75 ×** the median of its own condition at that resolution, the
   verdict for that resolution is **`HELD-FPS-PRIOR`** — the gate is **not read** until that leg is explained (081-05's carry-forward,
   literally). Predicted: no leg is held.
6. **Diagnostic pair (does NOT feed the gate)** — because the cause is not established: `S3_DIAG_A` (evidence OFF) and `S3_DIAG_B`
   (evidence ON), 720p, pacing OFF, **run log ON** (the Stage-1 configuration), 600 frames each, after the gated blocks, with the full
   load/disk/thread sampling. Readings: engine ms and game-thread cycles against the gated 720p legs of the same condition (the run log's wall
   and CPU cost on the game thread), the m55 log-line count (evidence ON adds one `MASK-SERVED` Log line per served mask on the render thread,
   flushed per line when the run log is ON), stall census, run-log line count. Predicted: game-thread **cycles** within ±0.3 ms of the gated
   legs of the same condition; engine ms per frame higher by 0.2–10 ms (wide: per-line flush latency on this disk is unmeasured); no stall.

## 2. The campaign

### 2.1 Fixture and recipe — chosen from the accepted c2 legs

Map `CB_GateLevel`, target `StaticMeshActor_49`, anomaly **`solid_swap`**, `IAI.Capture.Config 2 4 16 4 0`, native tick order, seed 777,
PNG, SVE grab, delivery OFF, the 081-23 fixture (input lock, placed view, pre-capture B1, captured-label B1). This is the `C2_SOLID` recipe
(PASS at 720p in c2, 081-36) and the 1080p twin recipe (`C1_SOLID1080`, PASS in c1) and the c0 long-run recipe (S1/S2, 1800 frames).
**Why this one:** it is the only accepted recipe exercised at **both** resolutions, so its 1080p full yield (20/20) and high-water (66.4 MB)
are known priors; `solid_swap` makes every measured pair a real full-magnitude change (non-degenerate arithmetic) through the production m55
path; it is fixed-camera and B1-gated, so A and B see the same pixels. **Stated limit:** it computes 4 required pairs per ~20-frame event
(≈ 0.2 pairs per captured frame); the accepted `C2_BLINK` recipe computes ≈ 0.48 and has never run at 1080p. The cost measured here is at the
solid recipe's pair density; per-pair worker ms is reported so it can be scaled (any scaling is an extrapolation, labelled as one).

### 2.2 Cost-specific settings (identical on A and B; the only A/B difference is `IAI.Capture.ChangeEvidence 0` on A)

Appended through the runner's `-ExtraExecCmds` (appended after, so they override the base line): `IAI.Capture.Pace 0` (pacing OFF, the
ruling), `IAI.Bench.ChangeGate 0` (the base line sets bench gate 14, which adds a CRC and a log line per frozen mask — a bench device, not
product), `IAI.Capture.RunLog 0` (§1). Everything else is the accepted recipe byte-for-byte.

### 2.3 Legs, order, durations

| block | legs (in order) | frames per leg |
|---|---|---|
| 720p | `S3_720_DISC` (declared discard, evidence OFF, never counted), then `A1 B1 B2 A2`, then `A3 B3 B4 A4` | 600 |
| 1080p | `S3_1080_DISC`, then `A1 B1 B2 A2`, then `A3 B3 B4 A4` | 300 |
| diagnostic | `S3_DIAG_A`, `S3_DIAG_B` (720p, run log ON; not in the gate) | 600 |

- **n = 4 A and 4 B legs per resolution**, in two ABBA blocks (linear drift cancels inside each block; G186).
- **600 frames at 720p** = 30 events (≤ 55: the m50 tag pool ran out at event 55 on the 1800-frame c0 legs, so the B workload is constant).
- **300 frames at 1080p**: pacing OFF saturates the 1080p writer (bank: 38–43 PNG/s against a 53–66 fps arm rate), and the run-end writer
  flush waits at most **5.0 s** (`AnomalyCaptureSubsystem.cpp:4283`). 300 frames keeps the predicted tail at 2–4 s on A and B.
- Steady window: session_index **60 … N−1** of each leg (540 / 240 frames).
- **Bench time:** ~20 launches × 35–45 s ≈ **12–15 min** expected; hard cap **60 min** of campaign time (the runner refuses to start a leg
  that would cross it: exit 6, resume-safe). Quiet/person/load waits come from a separate 45-min budget per tag.

### 2.4 Resume rule

A block (4 legs, or the diagnostic pair) counts only if all its legs are VALID within **one invocation**. On re-invocation, a block that was
incomplete has its earlier VALID legs marked `VOID-RESUME` (kept, never counted) and re-runs whole, so drift always cancels inside one
contiguous ABBA block. Every invocation re-runs the discard leg of each resolution it enters. A recorded `LEG-FAILURE`, `HARNESS-FAIL` or
`NOT-RUN` halts every later invocation up front until a ruling.

## 3. Metrics — source, window, warm-up

All times: UE `t_wall − 2^24` is the harness QPC clock (checked per leg: the first arm must fall 0–30 s after the runner's capture-start QPC
anchor, else HARNESS-FAIL). UTC for PNG mtimes comes from the runner's `anchor_start` {QPC, UTC} pair.

1. **Game-thread ms per engine frame (GATED).** External, read-only sampler at 10 Hz: `QueryThreadCycleTime` on the game's thread whose
   description is **`GameThread`** (UE sets it, `WindowsPlatformProcess.cpp:2117-2119`), converted at the registry TSC rate (**2688 MHz**),
   over the steady window, divided by the engine frames (`frame_index` interpolated at the two sample instants).
   Why not the CSV profiler: its header row is written at capture **end** (`CsvProfiler.cpp:2338`), and the runner force-kills the game.
   Why not `GetThreadTimes`: it is tick-sampled (15.625 ms) — on this box it misread a 1 ms-slice thread by 5–29 % across three runs and a
   4 ms-slice thread by −3 to +9 %, while the cycle counts reproduced the programmed load (081-37 `proof-sampler.json`).
   Cross-check reported per leg: cycles ÷ OS accounting over the window.
2. **Writer throughput (GATED).** Colour PNGs whose file mtime falls inside the steady window ÷ window seconds (081-24's definition).
   At 1080p this is the saturated writer's capacity; at 720p, where the writer keeps up, it tracks the arm rate.
3. **m55 worker ms** — `change_worker_ms_total` per captured frame and per measured pair; plus CPU cores by thread class (game, render, RHI,
   background/foreground workers) from the same sampler.
4. **Queue and byte high-water** — `change_bytes_high_water` and the peak census against the logged cap (EFFECTIVE `maxBytes=268435456`);
   writer backlog high-water (armed minus completed PNGs, from arm times and mtimes). Priors kept beside: **17.5 / 19.4 / 39.4 / 49.8 / 66.4 MB**.
5. **Drops** — integrity (below): every frame armed, labelled and written; no flush timeout, unresolved frame, failed encode/write, failed mask
   write, `key_ring_missed`, `change_rows_dropped`, `change_persist_failed`.
6. **Tails** — per-engine-frame wall time p50/p95/p99/max over consecutive captured frames one tick apart in the window; writer latency
   (PNG mtime − arm) p50/p95/p99/max.
7. **Required-pair yield** (B) — Σ `pairs_measured` / Σ `pairs_required` over the sidecar's event phases, with every unmeasured window row's
   reason.
8. **Latency distribution** (B) — `change_colour_completion_latency_*` from the summary, and p99 from the sidecar rows.
9. **`census_cycles`** — a reading.
10. **Load context (every leg)** — 1 Hz: CPU of every process (game / harness tree / OS-induced / foreign), PDH GPU 3D-engine utilisation
    by pid, process I/O, `PhysicalDisk(_Total)` idle %, sec/write and queue; stored raw per leg.

## 4. The statistic (declared)

Per resolution, over the counted legs (4 A, 4 B): Δ = mean(B) − mean(A); pooled within-condition SD
s_p = √[(Σ_A (x − x̄_A)² + Σ_B (x − x̄_B)²) / 6] — **the A/A and B/B spread is the noise floor**; SE = s_p·√(1/4 + 1/4); t = 1.943 (one-sided
95 %, 6 df); upper = Δ + t·SE, lower = Δ − t·SE.
- **Game thread:** PASS clause iff **upper ≤ +1.000 ms**. The bound, not the point estimate.
- **Writer throughput:** a drop is *beyond uncertainty* iff **upper < 0** (the whole one-sided interval lies below zero) → UNRESOLVED.
  **Resolution guard (G146):** if t·SE > **5 %** of the A mean, the clause is **UNRESOLVED (under-resolved)** — "unchanged within
  uncertainty" is not evidence when the uncertainty could hide a 5 % drop. An increase is never a failure.
- **Incomplete** (fewer than 4 + 4 counted legs) → UNRESOLVED. **FPS-prior hold** (§1.5) → HELD-FPS-PRIOR, gate not read.

## 5. Validity (a leg is counted only if VALID; invalid legs are re-run, never averaged in)

| class | condition | effect |
|---|---|---|
| INVALID-PERSON | the 081-30 person-evidence classifier finds a person in the attempt span (any prong) | not counted, re-queued (**stricter than 081-30 ruling 2c for cost legs, per 081-37**) |
| INVALID-LOAD | over the steady window: foreign CPU mean **> 2.0** logical cores, or any 1-s interval **> 3.5**, or foreign GPU 3D **> 5 %**, or foreign I/O **> 20 MB/s**, or load not measurable | not counted, re-queued |
| INVALID-FOREIGN | a foreign editor appears (runner `Check-Foreign`) | not counted, re-queued behind the quiet gate |
| INVALID-FIXTURE / -PRECAPTURE | pre-capture B1, whole-leg foreground, lock/place/release, captured-label B1 | 3 counted attempts, then NOT-RUN (exit 2) |
| HARNESS-FAIL | mode check: pacing not OFF, gate ≠ 0, run log not as declared, evidence switch not as intended (A: echo `0`, no EFFECTIVE, no sidecar, no `change_` keys; B: `EFFECTIVE enabled=1(from compiled) maxBytes=268435456(from compiled) gate=0(from console)`, sidecar, keys), wrong resolution, clock join, no GameThread samples | exit 3, halts |
| LEG-FAILURE | integrity (§3.5) or B vacuity (< 20 measured pairs) | exit 1 — **stop at first failure** |

"Foreign" = every process that is not the game, not in the harness's process tree, and not OS-induced (System, Registry, Memory Compression,
dwm, csrss, Defender, the search indexer, audiodg). Measured idle baseline on this box, 2026-09-26: **1.16 foreign cores** (Windows Widgets
0.9), foreign GPU 0.02 %, foreign I/O 0.6 MB/s — the limits sit ~1 core above it. A 5-s pre-launch load check precedes every attempt.
Environment caps: 5 environment-invalid attempts per leg → exit 4 (NOT-QUIET, no NOT-RUN). Every attempt records its load context.

## 6. Expected readings (declared; misses are reported, never refitted)

| reading | 720p | 1080p |
|---|---|---|
| verdict, game-thread clause | **PASS** (Δ within [−0.2, +0.3] ms; the stage worker runs off the game thread; c0: ≈ 0.12 cores, 4.7–5.0 ms of worker time per frame) | **PASS** (same band) |
| verdict, writer clause | **PASS** (writer keeps up; Δ ≈ 0) | **at risk — not predicted with confidence.** Saturated writer: the stage worker and B's canonical colour copy share the pool, predicted drop **1–3 %**; whether it is *beyond uncertainty* depends on the realised spread |
| arm rate (captured fps) | 45–90 | 40–65 |
| game-thread ms / engine frame | 2–12 (no prior on this box) | 2–14 |
| writer throughput (PNG/s) | = arm rate ± 2 % | 36–44 |
| tail, last arm → last PNG | ≤ 2 s | 1.5–4.5 s |
| writer latency p50 | 0.15–0.5 s | 0.5–4 s |
| m55 high-water | 15–50 MB | 40–110 MB (≤ 256 MiB cap) |
| required-pair yield (B) | 1.0 (≈ 120 required), zero `budget_exceeded` / `closure_timeout` | 1.0 (≈ 60 required), same |
| worker ms per captured frame (B) | 3–8 | 4–10 |
| drops | 0 | 0 |
| stall census in window | none | none |
| person / foreign / load invalid attempts | 0 expected | 0 expected |

## 7. Carried into the Stage 3 report (081-38/081-39), unchanged

- The UNEXERCISED lines of `docs/predictions/2026-09-20-m55-change-evidence.md`, verbatim (every line marked UNEXERCISED at `2125159`).
- High-water priors 17.5 / 19.4 / 39.4 / 49.8 / 66.4 MB, beside the new readings.
- The c0 long-run reading: evidence ON vs OFF, no measurable effect on sustained fps, PNG rate, writer latency, CPU or drops; stage ≈ 0.12
  logical cores (081-24).
