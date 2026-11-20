# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 081 m55 build 3 runtime + long-run (Code, Opus 5.5)

# 081-22 — m55 build 3 in the engine: wait for a quiet PC, then the owed runtime legs and the long-run ON/OFF readings

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. **Waiting for a quiet PC is part of the task, not a reason to stop.** End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. Builds, legs and waits outlive the tool timeout: run them in the background and poll; never end the run while a background task is still running. If the Bash tool stalls on this box, use the PowerShell tool.

## Context
m55 on `feat/m55-change-evidence` (head `94a56a9`; **build 3** = source `85c99b3`, Game **002805CF**, editor `m55-build3-main-editor-c27024d`, Lyra `m55-build3-lyra-cf0b38cb`). Build 3 fixed the review's findings and passed every offline check, but has **not run in the engine**. The long-run ON/OFF throughput readings (081-18) never ran either. Both were blocked because another project's UE 5.7 editors (<other-home-project>) held the PC all night.

## Read first
1. `D:\IntrusiveAnomalies\_reviews\081-21-chat-ruling-build3-accepted-bench-when-quiet.md` (the governing ruling).
2. `_reviews\081-21-…` evidence and report (`_reviews\081-21-evidence\`, the journal on the branch), including the four owed legs (a)–(d) as specified in the 081-21 brief: `_mailbox\processed\*081-21-m55-build3-review-fixes.md` §Verification.
3. The 081-18 brief (`_mailbox\processed\*081-18-m55-long-run-throughput-readings.md`): its legs L1–L4 (Lyra ABBA, 1800 frames, evidence ON/OFF) and S1–S2 (StackOBot AB), what to report, and the analysis questions. Its predictions (`_reviews\081-18-evidence\predictions.md`) and harness (`_reviews\081-18-*`, drivers under `_reviews\081-18-evidence\`) stand — note that the binary is now build 3.
4. `_reviews\081-17-code-m55-build2-report.md` and `_reviews\081-18-evidence\partial-081-17R-reanalysis.json` (the harness lineage and the offline writer-deficit reading).

## Quiet PC — the gate for every leg
- **Quiet** = no `UnrealEditor.exe` of any engine, and no game process that is not yours, running.
- Check at start; if not quiet, wait with one long-poll tool call at a time (≤ 9.5 min each, logging each check to `_reviews\081-22-evidence\environment-events.json`), **up to 4 hours in total**. Never kill or touch a foreign process.
- A foreign editor that appears during a leg makes **that leg invalid** (not failed): stop the leg cleanly, record it, go back to waiting (inside the same 4 h budget), then retry the leg (3 attempts per leg as usual).
- If the 4 h budget runs out, restore the boundary and stop with whatever ran.

## The legs, in this order
**Part 1 — build 3 runtime (the 081-21 owed legs):**
- (a) StackOBot 1080p `solid1080` natural smoke: 20/20 required pairs, zero `budget_exceeded`, invariants 0, oracle `--change-oracle` exit 0 on the session.
- (b) F1: start a run and stop it before the game window has focus → no session folder, nothing written.
- (c) F2: a real two-view family if you can create one (e.g. the engine's debug command that adds a second local player) → legacy colour and masks delivered as pre-m55, m55 rows `unsupported_delivery`, `change_multi_view_families` > 0, no growth of pending arms. If no real two-view family can be made, UNEXERCISED with the reason.
- (d) Legacy byte-identity, evidence **OFF** on build 3 vs the pre-m55 master binary `StackOBot.exe.m52-veto-arm-window-0844220E` (source = `031a103`), on the MainWorld legacy-comparison recipe the 081-12 comparisons used, both orders; declare before running which outputs must be byte-identical and which keys may differ (e.g. added `change_*` summary keys).
- **Stop on any fixture-valid failure in Part 1** (NEEDS-DECISION) — do not start Part 2.

**Part 2 — the 081-18 long-run readings, on build 3:** L1 Lyra ON, L2 OFF, L3 OFF, L4 ON (1800 frames, G270 recipe), S1 StackOBot ON, S2 OFF (1800 frames, `solid1080` recipe). Readings only; report and analyse exactly as the 081-18 brief asks (throughput and writer counters ON vs OFF with uncertainty, per-decile latency, lag series, budget refusals, tail outcome, thread-pool facts, plateau-vs-growth by the predeclared rule, the smallest change you would propose — **not implemented**). Run the oracle on every ON session.

## Hard rules
- **No build, no source change.** Docs/journal commits on the feature branch only (scratch worktree, as before). One bench; never kill anything that is not yours, never by name. Restore the boundary and prove it in `_reviews\081-22-evidence\postflight.json`. Never touch `master`, `m51`, tags, cooked containers, CaptureBench, `ToCodex\`, or existing `_reviews\` files. Codenames only.
- Predictions for Part 1 written to `_reviews\081-22-evidence\predictions.md` before the first leg; Part 2 reuses the 081-18 predictions.

## Do NOT start
The requalification inventory, Stage 3 cost legs or docs, any build.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED` — GREEN when Part 1 passed (or honestly UNEXERCISED for (c)) and Part 2 ran, or ran as far as the quiet windows allowed with every NOT-RUN leg named.
2. **Plain summary for the owner** — 4–6 short lines, no jargon.
3. Waiting log — when the PC was quiet, how long you waited, any mid-leg interruptions.
4. Part 1 table — leg, attempts, result, key numbers, oracle exit.
5. Part 2 — the 081-18 report sections 3–6 (leg table, trends, ON vs OFF, analysis).
6. Deviations and harness changes.
7. Boundary proof — postflight verdict line.
8. NEEDS-DECISION items — or "none".
9. Wall-clock per step, for the owner's implementer comparison.
