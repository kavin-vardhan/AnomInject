# SESSION: new
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 081 m55 independent review of Codex Stages 1-2 (Code, Opus 5.5)

# 081-20 — independent review of the Codex-authored m55 source (Stages 1–2 + completion clock), read-only, bench-free

You are Claude Code, running **headless** from the GDP mailbox as an **independent reviewer**. You did not write this code. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) the review below is done, (b) a stop condition fires, or (c) something you need is deliberately protected from you. Every final message follows the structure at the end and starts with a `VERDICT:` line. Never end the run while a background task is still running. If the Bash tool stalls on this box, use the PowerShell tool.

## What you are reviewing
m55 "measured change evidence" on `feat/m55-change-evidence`. **Scope: the Codex-authored source, `git diff 031a103 5649a6d -- Source/`** (25 files, ≈ +1,979 / −30, 40 commits incl. docs): the identity/lifetime layer (issue tokens, receipts, epochs/cuts, view owner, closure watermark), the ordered CPU stage and its measurement arithmetic (count > 8, 8-bin histogram, mean, control = complement of all masks), phases/events and the sidecar records, the 14-value refusal enum, the bench twins (`null_effect` / `solid_swap`), the bench-only `AnomalyBench` module and fixture commands, and the completion clock (`5649a6d`). **Out of scope:** `9c75abe` (build 2, authored by another Code session — Codex reviews that later), the Python tools, the harness scripts under `_reviews\`.

## The contract to review against
- `docs/predictions/2026-09-20-m55-change-evidence.md` on the branch (read with `git show feat/m55-change-evidence:<path>`; the live checkout is `m51`) — the three contracts (numeric, identity-lifetime, temporal), the refusal enum, the amendments at the top.
- `docs/change-evidence-fields.md`, gotchas G272–G283, `CLAUDE.md` conventions (game-agnostic: Core/CoreUObject/Engine only, no host-game types; no comments in code; stripper).
- Chat rulings `D:\IntrusiveAnomalies\_reviews\081-04 … 081-19-chat-*.md` tell you what was decided and why. **Codex's own reports are claims, not evidence** — verify anything you rely on.
- Evidence you may use: banked captures under `D:\IntrusiveAnomalies\_bench_sessions_bank\` (m55 rounds), `_reviews\081-*-evidence\`, and the new oracle `tools/verify_capture.py --change-oracle` (081-19; 1,234 banked rows matched).

## What to hunt for (not exhaustive — follow the code)
1. **Wrong number published as measured** — a pair joined to the wrong frame, mask, view or generation; a measured value surviving an epoch/cut reset; arithmetic that differs from the numeric contract; histogram/mean/denominator errors; control region leaks.
2. **Missed refusals** — any path where a pair that should be refused (or indeterminate) is emitted as measured, or where a missing onset could read as "absent".
3. **Lifetime and threading** — locks, `MoveTemp`/reallocation hazards while unlocked, the shared-owner counting of colour buffers, release on every path (finalize, closure, teardown, epoch reset), use-after-release, the async worker's scheduling (081-18: it shares the engine thread pool with the PNG writer), races between the game thread, render thread and writer workers.
4. **The completion clock** (`5649a6d`): completion counting beyond `Phase.Last` / the watermark, the 5 s backstop, the out-of-order rule, precedence (`bAwaitingClosure`) — can it hang, starve, or finalize too early?
5. **Memory accounting** — every reservation released exactly once; bytes never double-counted or leaked; behaviour at the cap.
6. **Bench isolation** — bench twins cannot enter the auto pool or apply outside the bench; the bench module cannot ship into a client build; fixture commands inert without the bench flag.
7. **Game-agnostic and convention violations** — any host-type dependency; comments (the repo convention forbids them; count them per file, P3).
8. **Hot-path cost** — per-pair pixel loops at 1080p on a pool thread; anything that scales badly with resolution, events or run length.

## Method
- Before reading deeply, write your review questions and what evidence would settle each to `_reviews\081-20-evidence\review-plan.md`.
- Static reading plus offline reasoning; where a claim can be tested without Unreal (a Python model of a C++ predicate, a replay against banked sidecars/logs, an oracle run), test it and keep the script in `_reviews\081-20-evidence\`.
- For every finding: severity — **P1** (a wrong number or wrong pair published as measured; crash; hang; unbounded growth; data loss), **P2** (honest refusal where a measurement was possible; real perf risk; a latent defect likely to bite), **P3** (convention/clarity); `file:line` on commit `5649a6d`; a concrete failure scenario (inputs/state → wrong output); your confidence; whether banked evidence already exercises it.
- A finding you cannot make concrete is a question, not a finding — list those separately.

## Hard rules
- **Read-only.** No source edits, no commits, no build, no Unreal launch, no bench, no process control. Do not touch the live checkout's working tree (read with `git show` / a scratch worktree you remove afterwards), `master`, `m51`, tags, CaptureBench, `ToCodex\`, or any existing file under `_reviews\` — write new files only, under `_reviews\081-20-evidence\` and your report `_reviews\081-20-code-review-of-codex-m55.md`.
- Another project's editors may be running on this PC — irrelevant, you need no engine.
- Codename discipline: codenames only (Concorde, Bates, Europa).
- Time bound ≈ 3 h; if exceeded, stop with what is reviewed and the remainder listed.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED` — GREEN when the review is complete and found **no P1**; NEEDS-DECISION if any P1 exists or the review could not complete.
2. **Plain summary for the owner** — 4–6 short lines, no jargon.
3. Findings table — id, severity, file:line, one-line defect, failure scenario, confidence, evidence status.
4. Tested claims — what you tested offline and the results.
5. Open questions (could not be made concrete).
6. Coverage — what you read fully, skimmed, or did not reach.
7. Wall-clock per step, for the owner's implementer/reviewer comparison.
