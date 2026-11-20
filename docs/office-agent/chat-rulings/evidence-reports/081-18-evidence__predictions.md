# 081-18 predictions — m55 build 2 long-run throughput, evidence ON vs OFF (Claude Code, Opus 5.5)

Written 2026-09-24 before the first 081-18 capture, after the preflight and after an analyzer self-test on
banked 081-17R data. Readings only: no build, no source change. Build under test: source `9c75abe`,
Game 8E64FA39 (packaged StackOBot), Lyra modules `m55-stage2-r17-lyra-48b9e048` (UnrealEditor.exe -game).
Missed predictions will be reported as missed, never refitted.

## 1. Legs (order fixed)

| # | Label stem | Host / recipe | Frames | Evidence |
|---|---|---|---|---|
| L1 | L1_LYRA_ON | Lyra `L_ShooterGym`, 081-17R `LYRA_G3_N_A2` CLI (auto-pool `stuck_low_mip`, Config `2 4 8 4 0`, 1920x1080) | 1800 | ON (compiled) |
| L2 | L2_LYRA_OFF | same + `IAI.Capture.ChangeEvidence 0` | 1800 | OFF |
| L3 | L3_LYRA_OFF | same | 1800 | OFF |
| L4 | L4_LYRA_ON | same as L1 | 1800 | ON |
| S1 | S1_STACK_ON | StackOBot `CB_GateLevel`, 081-17R `B_SOLID1080_N_A1` CLI (targeted `solid_swap`, Config `2 4 16 4 0`, order 0) | 1800 | ON |
| S2 | S2_STACK_OFF | same + `IAI.Capture.ChangeEvidence 0` | 1800 | OFF |

Only changes from the 081-17R runners: frame count, the evidence switch, evidence root/bank prefix `M55L18_`,
capture wait bound 150 s → 600 s, two QPC/UTC anchors, and a 1 Hz passive process sampler (game-process CPU
time, working set, private bytes, thread count; ShaderCompileWorker count/CPU; `GetSystemTimes`).
Pre-capture fixture failures get up to 3 attempts per leg; after that the leg is NOT-RUN. Anything that
happens after `capture_start` (host respawn, pose drift, foreground loss, flush timeout) is recorded with its
index, never retried. The switch state is read back, not assumed: ON requires the
`EFFECTIVE enabled=1(from compiled) maxBytes=268435456(from compiled)` line plus sidecar and `change_*` keys;
OFF requires the engine echo `IAI.Capture.ChangeEvidence = "0"`, no EFFECTIVE line, no sidecar, no `change_*`
key. A mismatch stops the round as a harness fault.

## 2. Source facts this reading rests on (source-read, not measured)

- Machine: i7-12650H, 10 physical / 16 logical cores (`LogInit: Cores=10`), 23.6 GB RAM, NVMe.
- `NumberOfWorkerThreadsToSpawn()` = 16 − 2 = 14; `GUseNewTaskBackend` = 1 (5.1 default); task graph starts
  14 workers = 2 foreground + 12 background (`TaskGraph.cpp:1919-1922`, `GNumForegroundWorkers` = max(⌈14/21⌉, 2)).
- Packaged StackOBot (`WITH_EDITOR` 0): `GThreadPool` = `FQueuedLowLevelThreadPool` on that scheduler
  (`LaunchEngineLoop.cpp:2350`). Lyra via UnrealEditor.exe (`WITH_EDITOR` 1): `GThreadPool` =
  `FQueuedThreadPoolWrapper(GLargeThreadPool, 14)` with `GLargeThreadPool` a `FQueuedLowLevelThreadPool`
  (`:2305-2331`); the wrapper keeps a per-priority queue and dequeues the oldest first (`ThreadingBase.cpp:910-915`).
- `Async(EAsyncExecution::ThreadPool, …)` enqueues at `EQueuedWorkPriority::Normal` (`Async.h:354`), which maps to
  `LowLevelTasks::ETaskPriority::BackgroundNormal` (`QueuedThreadPoolWrapper.h:460`) — runnable only on the 12
  background workers.
- The m55 stage worker (`ScheduleLocked` → `Async(ThreadPool, Work)`, `AnomalyChangeStage.cpp:370-376`) and every
  PNG writer job (`FAnomalyAsyncWriter::Enqueue`, `AnomalyAsyncWriter.cpp:23`) therefore share one Normal-priority
  queue. The worker is re-dispatched only when inactive, and it drains every ready index each time it runs.
- "Colour completion" is `Change->Colour()`, called after `EncodeAndWriteFrame` has converted, PNG-encoded and
  written the frame and appended its labels row (`AnomalyAsyncWriter.cpp:70-80`). So colour-completion latency is
  issue → PNG written.
- Run end: `BeginClosure` (5 s wall bound) then `FlushPending(5.0)` (`AnomalyCaptureSubsystem.cpp:5221, 4274`).
- ⛔ Hypothesis **H-Q, NOT established:** the head sits "ready but unprocessed" because the worker's dispatch
  waits in that FIFO behind queued PNG jobs, so its wait grows with the writer backlog.

## 3. Instruments

1. **Writer latency, every frame, ON and OFF:** PNG `LastWriteTime` minus the frame's arm time. Arm time = labels
   `t_wall` (`FPlatformTime::Seconds`) mapped to UTC by the median offset to the same index's
   `Capture(m47): SHADERS … si=N` log line; the harness QPC anchor is a cross-check.
   **Known answer already passed on banked 081-17R data:** residual vs the stage's own
   `colour_completion_latency_ms` median +0.04 / −0.28 / +0.08 ms, range −1.6…+1.2 ms, n = 60 rows over three
   legs; the per-frame p50/p95/max reproduce the summary's (1190.9/1550.2/1628.8 vs 1191/1551/1629 ms).
   **Per-leg gate (ON legs):** |median residual| ≤ 5 ms and max |residual| ≤ 50 ms, else that leg's PNG-derived
   latency is not used. OFF legs use the instrument validated on the same host's ON legs plus an anchor spread ≤ 5 ms.
2. **Census lines:** every `HIGH-WATER` and `BUDGET-EXCEEDED` line reduced to time, trigger, si, cursor,
   latest, lag, bytes, colour split, masks, head state / waits_on, and `head_wait_ms` = line time − max(head
   colour PNG mtime, head mask PNG mtime) when the head is pending with nothing to wait on.
3. **Per-index refusal reconstruction** (first_frame / budget line / closure line / predecessor after a
   non-admitted colour / mask state). Used per decile **only if it reproduces every `change_reason_*` total and
   `change_pairs_identity_valid` exactly** (self-test: exact on three 081-17R legs). Otherwise per-decile
   refusals are reported only for the logged classes (budget, closure) and window rows.
4. **Process sampler:** game-process CPU (logical cores) and system busy % over the arm window; private bytes.

Deciles = 10 equal bins of session index over the armed range.

## 4. Decision rules (fixed now)

**R1 writer latency (per leg).** m_d = median latency of decile d.
GROWTH if m10 − m2 ≥ max(250 ms, 0.5·m2) and ≥ 6 of the 8 steps m(d+1) − m(d), d = 2…9, are positive.
PLATEAU if |m10 − m2| ≤ max(100 ms, 0.25·m2) and max − min of m6…m10 ≤ the same band. Else INDETERMINATE.

**R2 stage retention (ON legs).** GROWTH if any `budget_exceeded`, or if a `HIGH-WATER` line is logged at
si ≥ 0.7·N and the high-water ≥ 1.5 × the largest held bytes logged before 0.3·N.
PLATEAU if zero `budget_exceeded` and no `HIGH-WATER` line (lines are throttled to one colour of rise) at
si ≥ 0.5·N. Else INDETERMINATE.
**Answer to chat's question** = PLATEAU only if R1 and R2 are both PLATEAU on that host; GROWTH if either is GROWTH.

**R3 ON vs OFF.** Lyra (two legs each, ABBA order): for sustained_wall_fps, speed_ratio, PNG completion rate
(same arm window), latency slope and decile-10 median, DISTINGUISHABLE only if the ON range and the OFF range do
not overlap; the difference of means is reported either way. StackOBot (one leg each): median-latency
difference is DISTINGUISHABLE only if it exceeds the larger within-leg spread of m2…m10; rate differences are
stated but not quantifiable at n = 1. "Adds drops" = any failed/unresolved/missing frame on ON with none on OFF.

**R4 H-Q support (reported as support, never as proof).** Supported if on the Lyra ON legs (a) ≥ 70 % of census
lines show the head pending with empty `waits_on`, (b) Spearman ρ(head_wait_ms, head writer latency) ≥ 0.5,
and (c) StackOBot ON census head waits have median ≤ 150 ms. Otherwise not supported.

## 5. Numeric predictions

Lyra ON (L1, L4):
- P1 R1 = GROWTH; OLS slope d2–d10 1.5–5.0 ms/frame; m10 3.0–9.0 s.
- P2 armed 1800; 1800 PNGs on disk after the tail, no zero-byte, none missing.
- P3 ≥ 1 `budget_exceeded`; first at si 250–800; R2 = GROWTH; high-water ≥ 241.6 MB (≥ 90 % of the cap).
- P4 decile 10: ≥ 40 % of issued indices refused as `budget_exceeded` or `predecessor_missing`.
- P5 ≥ 70 % of census lines head-pending with empty waits_on; decile-10 median head_wait ≥ 1000 ms.
- P6 `pairs_measured` < `pairs_required` (at least one phase not fully measured).
- P7 run end: a `writer flush timed out` line with 1–100 jobs; `total_frames` < 1800; `late_results` ≥ 1;
  `closure_timeout` 0–30.
- P8 15–45 annotation events. P9 speed_ratio 1.00–1.06.
- P10 game process ≥ 11 logical cores over the arm window; system busy ≥ 75 %; private bytes +≥ 0.8 GB from the
  first sample to the last arm.

Lyra OFF (L2, L3):
- P11 R1 = GROWTH with slope within 0.7–1.3 × the ON legs' mean slope; a flush-timeout line; switch read back OFF.
- P12 R3: writer completion rate and sustained fps NOT DISTINGUISHABLE between ON and OFF.

StackOBot ON (S1):
- P13 R1 = PLATEAU; overall p50 250–500 ms; |slope| ≤ 0.3 ms/frame.
- P14 zero `budget_exceeded`, zero `closure_timeout`, zero late results; R2 = PLATEAU; high-water 50–130 MB.
- P15 85–90 events; every phase fully measured. P16 no flush timeout; `total_frames` 1800.
- P17 speed_ratio 1.000–1.010; game process 5–12 logical cores; private bytes growth < 0.5 GB.
- P18 `change_worker_ms_total` 8–25 s.

StackOBot OFF (S2):
- P19 R1 = PLATEAU; R3 median-latency difference vs S1 NOT DISTINGUISHABLE.

H-Q:
- P20 R4 = supported.
