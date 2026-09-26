# 081-18 — m55 build 2 long-run throughput, evidence ON vs OFF: STOPPED before the first capture (Claude Code)

2026-09-24 ~01:10 → 02:45 IST. Implementer: Claude Code (Opus 5.5), headless, brief 081-18. Authority:
`_reviews/081-17-chat-ruling-build2-accepted-throughput-readings.md`. Readings only: no build, no source
change. Build under test: source `9c75abe`, Game 8E64FA39, Lyra modules `m55-stage2-r17-lyra-48b9e048`.

## 1. Result in one paragraph

**No long-run leg ran.** The preflight passed, the harness was built and proven on banked data, the
predictions were frozen (SHA-256 `225cb4df…2d05`, 20:01:39Z), Lyra was staged, and L1 attempt 1 failed
**before capture** on the pre-capture L2 audit (host `GA_AutoRespawn` after the placement — the same shape as
081-17R's A1). In the same second that attempt launched (01:32:00 IST) a **foreign UE 5.7 `UnrealEditor`
for HeistCrewUE** started, and three more followed (01:47, 02:05, 02:31). Under the brief's rule the driver
refused to launch, checked every 5 minutes, and stopped at the 60-minute bound (02:33:35 IST) with **four**
foreign editors still running. None was touched. Lyra was restored; the boundary is proven by postflight.
The only readings in this journal are an **offline re-analysis of 081-17R's banked 300-frame legs** (§5),
labelled as such: they do not answer the long-run question.

## 2. Timeline (IST)

| Time | Event |
|---|---|
| 01:22–01:24 | Preflight. Attempt 1 flagged one change: `_reviews/081-17-codex-vs-code-comparison.md` (chat's scorecard) edited 01:07:42, after 081-17R's postflight. Recorded as a declared external update with both hashes; re-run: **0 mismatches** over 180 binaries, 1,782 prior artifacts, 14,031 bank files, remote refs and the Lyra module set. |
| 01:25–01:33 | Harness derived and self-tested (§3). Predictions frozen 01:31:39. Lyra staged 01:31:47 (`9c75abe` + 11 archived modules). |
| 01:32:00 | L1_LYRA_ON_A1 launched. Foreign HeistCrewUE editor PID 153756 started the same second (launcher parent already exited). |
| 01:33:27 | L1 A1 INVALID before capture: L1 PASS (0.115 cm / 0°), L2 pre-capture FAIL (`GA_AutoRespawn` FinishingRestart / RequestingPlayerRestartNextFrame / OnPawnAvatarSet at 20:03:26Z, after placement). Fixture attempt 1 of 3. No `capture_start` was sent. |
| 01:33:29 | Driver refused attempt 2: foreign bench-class process present. Waiting, 5-minute checks. |
| 01:47:42, 02:05:15, 02:31:07 | Foreign editors PIDs 96312, 76640, 104420 (same UE 5.7 install, same HeistCrewUE project, other maps). Combined ≈ 3 cores and 3–4 GB each. |
| 02:33:35 | 60-minute bound reached with 4 foreign editors present → stop (`environment-events.json`). |
| 02:34 | Lyra restored to detached `caa68c6` with its original 9 modules (two candidate-only AnomalyBench files removed). |

## 3. Harness (ready for a rerun, all under `_reviews/081-18-*`)

- `081-18-start.py` preflight rooted at 081-17R's postflight; `081-18-lyra-stage.py` / `-lyra-restore.py`;
  `081-18-lyra.py` / `-stack.py` drivers (fixed order L1 ON, L2 OFF, L3 OFF, L4 ON, S1 ON, S2 OFF; ≤ 3
  pre-capture fixture attempts; anything after `capture_start` recorded, never retried; ON/OFF read back from
  the leg's own log, a mismatch stops the round); `081-18-analyze.py`; `081-18-compare.py` (rules R1–R4,
  scores P1–P20); `081-18-close.py` postflight.
- Runners derived from 081-17R's by asserted one-occurrence string replacements (`harness-derivation.json`):
  evidence root, bank prefix `M55L18_`, capture wait 150 → 600 s, QPC/UTC anchors, a 1 Hz passive sampler
  (game-process CPU/working set/private bytes/threads, ShaderCompileWorker count/CPU, `GetSystemTimes`).
- **Instrument proven against a known answer before any 081-18 capture:** writer latency per frame = PNG
  `LastWriteTime` − arm time (labels `t_wall` mapped to UTC through the frame's own `Capture(m47): SHADERS …`
  log line). On three banked 081-17R legs its residual against the stage's own
  `colour_completion_latency_ms` is −1.6 … +1.2 ms (n = 60 rows), and it reproduces the Lyra summary
  percentiles (1190.9 / 1550.2 / 1628.8 vs 1191 / 1551 / 1629 ms). The per-index refusal reconstruction
  reproduces every `change_reason_*` total and `change_pairs_identity_valid` on all three.
- Declared harness change after the stop was foreseeable: the restore and postflight idle checks now refuse
  only on bench-class processes that can touch our trees (engine path under `D:\UESource`, or a
  Lyra/StackOBot command line) and record foreign ones separately. The launch-time idle check is unchanged.
- The frozen predictions (`docs/predictions/2026-09-24-m55-081-18-long-run-throughput.md`, byte-identical to
  `_reviews/081-18-evidence/predictions.md`) were written before any capture and remain valid for a rerun of
  the same legs.

## 4. Source facts (source-read, UE 5.1, this machine)

- i7-12650H, 10 physical / 16 logical (`LogInit: Cores=10`). `NumberOfWorkerThreadsToSpawn()` = 14;
  `GUseNewTaskBackend` = 1 ⇒ 14 task workers = **2 foreground + 12 background** (`TaskGraph.cpp:1919-1922`).
- Packaged StackOBot: `GThreadPool` = `FQueuedLowLevelThreadPool` on that scheduler (`LaunchEngineLoop.cpp:2350`).
  Lyra via `UnrealEditor.exe -game`: `GThreadPool` = `FQueuedThreadPoolWrapper(GLargeThreadPool, 14)` over a
  `FQueuedLowLevelThreadPool` (`:2305-2331`); the wrapper dequeues the oldest work first (`ThreadingBase.cpp:910-915`).
- `Async(EAsyncExecution::ThreadPool)` enqueues at `EQueuedWorkPriority::Normal` (`Async.h:354`) →
  `ETaskPriority::BackgroundNormal` (`QueuedThreadPoolWrapper.h:460`): only the 12 background workers run it.
- The stage worker (`ScheduleLocked`, `AnomalyChangeStage.cpp:370-376`) and every PNG writer job
  (`AnomalyAsyncWriter.cpp:23`) therefore share **one Normal-priority FIFO**. The worker is re-dispatched only
  when inactive and drains every ready index when it runs. "Colour completion" is `Change->Colour()`, called
  after the PNG is encoded and written (`AnomalyAsyncWriter.cpp:70-80`).
- `AsyncPool(*GThreadPool, …, nullptr, EQueuedWorkPriority::High)` (`Async.h:393-400`) would enqueue the worker
  at `BackgroundHigh`, ahead of Normal writer jobs. Named as a candidate lever only.

## 5. Offline re-analysis of 081-17R's banked 300-frame legs (NOT the long-run reading)

| Leg (081-17R) | Armed fps | PNG completion fps (same window) | Writer latency decile medians, ms | R1 | Ready-head wait at census lines |
|---|---|---|---|---|---|
| LYRA_G3_N_A2 | 23.23 | 21.72 | 672, 792, 958, 1034, 1171, 1210, 1337, 1314, 1447, 1528 (2.8 ms/frame) | GROWTH | 0 → 801 ms over 11.7 s; 18/20 lines pending with empty `waits_on`; Spearman ρ vs the head's writer latency 0.95 |
| B_SOLID1080_N_A1 | — (90 frames) | 24.3 overall | 378 … 395, flat | PLATEAU | 0–1 ms |
| B_NULL1080_N_A1 | — | 25.0 overall | 290 … 282, flat | PLATEAU | 0–1 ms |

Armed frames arrive at ≈ 23–25 fps, not 30 (ticks per captured frame 1.22–1.26). On Lyra 1080p the writer
completed ≈ 1.5 fps fewer frames than were armed, so its backlog and latency grew for the whole 300 frames,
and the stage's ready head waited longer as the writer's latency grew. ⚠ **This is consistency, not a test:**
hypothesis H-Q (the worker's dispatch waits in the writer's FIFO) was formed from this same census, and the
correlation cannot separate a shared cause (CPU saturation) from queueing. ⛔ **The long-run question (plateau
or growth over 1800 frames, ON vs OFF writer throughput) is unanswered.** A linear extrapolation of these
rates to 1800 frames (≈ 6 s writer latency, the 256 MiB cap reached near si ≈ 400) is a projection, not a
reading.

## 6. What a rerun needs

An idle bench: no foreign `UnrealEditor` / game / UBT / ShaderCompileWorker. The four HeistCrewUE editors
belong to another workload on this machine; whoever runs it has to stop it or schedule around it. Then the
same legs run unchanged from the frozen predictions: stage Lyra, `081-18-lyra.py`, restore, `081-18-stack.py`,
`081-18-analyze.py`, `081-18-compare.py`, `081-18-close.py` — into a fresh evidence folder or as a declared
continuation of this one (L1 has used 1 of its 3 pre-capture attempts).

## 7. State

Main checkout `m51` `53bf725` (build-2 binaries in place, as 081-17R left them); Lyra detached `caa68c6`
with its original nine modules; every binary byte-identical to the preflight; no bank session created;
`master`, `m51`, tags, CaptureBench, `ToCodex\`, cooked containers and the owner's untracked files untouched.
Proof: `_reviews/081-18-evidence/postflight.json`.
