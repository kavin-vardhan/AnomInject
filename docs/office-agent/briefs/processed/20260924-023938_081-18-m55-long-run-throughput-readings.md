# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 081 m55 long-run throughput (Code, Opus 5.5)

# 081-18 — m55 build 2: long-run throughput readings, evidence ON vs OFF (readings only — no build, no source change)

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. Captures outlive the tool timeout: run them in the background and poll; **never end the run while a background task is still running**. If the Bash tool stalls on this box, use the PowerShell tool. This does not override the rules on destructive actions below.

## Context
m55 "measured change evidence", branch `feat/m55-change-evidence` (head `fcfde07`, built source `9c75abe`, Game **8E64FA39**, Lyra modules archive `m55-stage2-r17-lyra-48b9e048`). The previous round (081-17R, a different Code session) built this candidate and passed every leg. On Lyra G270 at 1080p the census showed the stage cursor falling behind the newest issued index — 12 → 28 → 40 → 53 frames over the 300-frame capture — with the head pending and waiting on nothing at the peaks, 197 MB held (cap 256 MiB), and issue→colour latency p50 1.19 s (StackOBot: 0.30–0.39 s). The stage worker and the PNG writer share the engine thread pool. A 300-frame run cannot tell a plateau from slow growth. **This round measures that, on long runs, with evidence ON vs OFF, so chat can decide whether a scheduling fix is needed before the requalification campaign.**

## Read first
1. `D:\IntrusiveAnomalies\_reviews\081-17-chat-ruling-build2-accepted-throughput-readings.md` (full).
2. `_reviews\081-17-code-m55-build2-report.md` (full) and `_reviews\081-17-evidence\` — `final-ledger.json`, `lyra-g270-readings.json`, `stack1080-readings.json`, `postflight.json`, and the harness you reuse: `_reviews\081-17-*.py` / `*.ps1` (derived from Codex's 081-15 scripts; `081-17-lyra.py` for the Lyra fixture sequence).
3. On the feature branch (`git show feat/m55-change-evidence:<path>`; the live checkout is `m51`): `docs/change-evidence-fields.md` (the census and latency fields), journal `docs/sessions/2026-09-24-081-17-code-m55-build2.md`, gotchas G272–G282.
4. The engine source for the thread pool the stage worker uses (`Async(EAsyncExecution::ThreadPool)` in `AnomalyChangeStage.cpp`) and the writer's pool (`AnomalyAsyncWriter.cpp`) — read-only, to explain what you measure.

## Hard rules
- **No build, no source change, no commit to source.** Docs/journal commits on the feature branch only. No merge, tag, cook; never touch `master`, `m51`, cooked containers, CaptureBench, `ToCodex\`, the owner's untracked files.
- **One bench.** At start verify: main checkout `m51` @ `53bf725` with 8E64FA39 staged (built = staged = archive); Lyra detached @ `caa68c6` with its original nine modules; no `UnrealEditor` / `StackOBot` / `LyraGame` / `UnrealBuildTool` process running. A process that is not yours: **never kill it** — wait for it (check every 5 min, up to 60 min), then stop and report if it is still there. Never kill anything by name; stop only PIDs you started.
- Restore the boundary exactly as 081-17R did and prove it in `_reviews\081-18-evidence\postflight.json`.
- Codename discipline: codenames only (Concorde, Bates, Europa); never a real game title.
- Predictions (what you expect to see, and your plateau/growth rule) are written to `_reviews\081-18-evidence\predictions.md` **before the first capture** and committed with the journal at the end. Missed predictions are reported, never fitted.

## The legs (readings only; order fixed)
All at 1920×1080, same settings as the 081-17R recipes except frame count and the evidence switch. Label every leg uniquely.
| # | Host / recipe | Frames | Evidence |
|---|---|---|---|
| L1 | Lyra G270 recipe (081-17R `LYRA_G3_N_A2` CLI) | 1800 | ON (compiled default) |
| L2 | same | 1800 | OFF (`IAI.Capture.ChangeEvidence 0`) |
| L3 | same | 1800 | OFF |
| L4 | same | 1800 | ON |
| S1 | StackOBot `solid1080` recipe (081-17R `B_SOLID1080_N_A1` CLI), natural order | 1800 | ON |
| S2 | same | 1800 | OFF |

Lyra legs use the same fixture sequence as 081-17R (input lock, settle, L2 placement, L1 check, foreground). A host respawn or pose drift **during** a long capture does not invalidate a reading — record it with its frame index. Fixture attempts: up to 3 per leg for a failure *before* capture; after that record the leg as NOT-RUN and continue with the next.

## What to report per leg
- **Throughput:** `sustained_wall_fps`, `speed_ratio`, ticks per captured frame, frames written, every writer drop/late/queue counter the run summary carries, total wall time.
- **Evidence (ON legs):** issued / measured / refused by reason — overall **and per 10 % of the run (deciles by session index)**; first session index of any `budget_exceeded`; high-water and peak census; every `BUDGET-EXCEEDED` and `HIGH-WATER` census line reduced to (time, cursor, latest_index, lag, colours pending, head state/waits_on); tail outcome (any `closure_timeout`, `retained` bytes at close); `change_worker_ms_total`.
- **Latency trend:** issue→colour completion ms and frames, p50/p95/max overall and **per decile** (is the writer falling behind over time?).
- **ON vs OFF:** the throughput numbers side by side (L1+L4 vs L2+L3; S1 vs S2) with the difference and your uncertainty.
- **Thread pool facts:** the engine thread-pool size on this machine (from the log or source + CPU count), and which pool the stage worker and the writer use.

## Your analysis (short, evidence-backed)
1. Plateau or growth? Apply the rule you predeclared; give the lag and latency trend numbers it rests on.
2. Does evidence ON change writer throughput or add drops vs OFF, beyond run-to-run noise?
3. Your best explanation of the head sitting "ready but unprocessed", with its confidence level — label anything you have not demonstrated as a hypothesis.
4. The **smallest** change you would propose if a fix is needed (e.g. where the stage worker should run), and what it would risk. **Do not implement it.**

## Do NOT start
Any build or source change; the requalification inventory; Stage 3.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED` — GREEN when every leg ran (or is honestly NOT-RUN with its reason) and the boundary is restored; the readings themselves are not a pass/fail.
2. **Plain summary for the owner** — 4–6 short lines, no jargon.
3. Leg table — leg, attempts, fixture notes, wall time, sustained fps, frames written, drops, ON-only: measured/refused (by reason), first budget refusal index, high-water, worker ms.
4. Trends — lag series and latency per decile (compact tables).
5. ON vs OFF comparison with uncertainty.
6. Analysis (1–4 above).
7. Deviations and harness changes.
8. Boundary proof — postflight verdict line.
9. NEEDS-DECISION items — or "none".
10. Wall-clock per step, for the owner's implementer comparison.
