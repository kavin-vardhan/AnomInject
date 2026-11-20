# ACK-VERDICT: 081-26 NEEDS-DECISION resolved by chat ruling 081-26 — PQ-A, PQ-B and the G270 empty_region clause amended before those legs run; G8 correction and G290 hardening accepted
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: high
# NAME: 081 m55 pre-run prediction amendments (Code, Opus 5.5)

# 081-27 — apply three pre-run prediction amendments (PQ-A, PQ-B, G270 empty_region), prove them, re-issue the boundary (bench-free)

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. If the Bash tool stalls on this box, use the PowerShell tool.

## Read first
1. `D:\IntrusiveAnomalies\_reviews\081-26-chat-ruling-prediction-amendments-pre-run.md` — the governing ruling (three amendments, exact wording).
2. `_mailbox\to_chat\081-26-m55-predicate-correction-and-audit.report.md` and `_reviews\081-26-evidence\` (audit table, g8 proof, boundary diff, dry-run wrapper).
3. The harness: `_reviews\081-23-lib.py` (incl. `:52-62` peak census, `:211` view_rejected, `lyra_g270_eval`), `081-23-evidence\prep-boundary.json`, `081-23-evidence\predictions.md`; on the branch (head `148050f`): the requalification predictions doc, `docs/change-evidence-fields.md`.

## The work
1. Apply the three amendments exactly as the ruling words them, in `081-23-lib.py` and as a dated **pre-run** amendment in the requalification predictions (branch + `_reviews\081-27-evidence\predictions-amendment.md`).
2. Prove each both ways: PASS on real stand-ins (Stage-1 V6 G2/G15 for PQ-A; a two-closure stand-in or a faithful synthetic for PQ-B; c0 L1's 14 `empty_region` rows for the G270 clause) and FAIL on mutations (a `view_rejected` ≥ 1 on a gate that must be 0; a missing or unequal last peak line / an unclosed census; a required pair refused as `closure_timeout` or `budget_exceeded`). Write `_reviews\081-27-evidence\amendment-proof.json`.
3. Re-issue `prep-boundary.json` for exactly the changed hashes (lib, feature head), with `_reviews\081-27-evidence\boundary-diff.json`; replay the preflight offline for c1, c2, c3.
4. Re-run the dry-run suite through the 081-26 redirecting wrapper: all prior expectations unchanged plus the new cases.
5. Docs: journal, CLAUDE.md status block; commit and push the feature branch (scratch worktree).

## Hard rules
**No Unreal launch, no build, no bench, no process control** — the owner is rendering on the PC. No plugin source change. Change only `081-23-lib.py`, `prep-boundary.json` and new files; list every file changed. Never touch `master`, `m51`, tags, cooked containers, CaptureBench, `ToCodex\`. Codenames only. Conventions: no comments in code; Conventional Commits. If you find another predicate whose **prediction** (not encoding) looks wrong, report it — do not change it.

## Do NOT start
Windows c1–c3 (next brief, when the owner frees the PC). Stage 3.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED` — GREEN when the three amendments are applied and proven, the boundary is re-issued and replays, and the dry run passes.
2. **Plain summary for the owner** — 3–4 short lines.
3. Amendment proof table (amendment → stand-ins passed → mutations failed).
4. Boundary diff; dry-run totals.
5. Commits; files changed.
6. NEEDS-DECISION items — or "none".
7. Wall-clock.
