# ACK-VERDICT: 081-25 NEEDS-DECISION resolved by chat ruling 081-25 — harness predicate-encoding defect; correct, audit all predicates, re-evaluate C1_G8_N from the bank; bench resumes in a later brief
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 081 m55 predicate correction + audit (Code, Opus 5.5)

# 081-26 — correct the G8 "no receipt" predicate, audit every gate predicate, re-evaluate C1_G8_N from the bank (bench-free)

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. If the Bash tool stalls on this box, use the PowerShell tool.

## Read first
1. `D:\IntrusiveAnomalies\_reviews\081-25-chat-ruling-g8-predicate-correction.md` — **the governing ruling**, full.
2. `_mailbox\to_chat\081-25-m55-windows-c1-c3.report.md` and `_reviews\081-25-evidence\c1-g8-cause-read.md`.
3. The harness: `_reviews\081-23-lib.py` (all gate and §D predicates), `081-23-chunk1.py` … `chunk3.py`, `081-23-window.py`, `081-23-preflight.py`, `081-23-postflight.py`, `081-23-evidence\prep-boundary.json`, `081-23-evidence\c1\` (the stopped window's state), `081-23-evidence\predictions.md`.
4. On the branch (`git show feat/m55-change-evidence:<path>`, head `1ca1ab3`): `Source/AnomalyCapture/Private/AnomalyChangeStage.cpp` (the serialiser, reason assignment), `docs/change-evidence-fields.md` (incl. the F5 reason mapping), gotchas G272–G289.

## The work
1. **Correct the G8 clause** exactly as ruling 1 says; prove it both ways (PASS on the banked Stage-1 G15/G4 "no receipt" stubs and on `C1_G8_N_A1`; FAIL on a mutated SI8 row with any single delivery field populated).
2. **Audit every gate/§D predicate** in `081-23-lib.py` (and any evaluator the chunks call) against what the serialiser actually writes for the predicted condition. Output a table: predicate (file:line), prediction in words, serialised form expected, where the source writes it, banked stand-in (path) or "none", verdict (OK / ENCODING-DEFECT fixed / PREDICTION-QUESTION). Fix encoding defects only, each declared, dated and proven both ways. Do **not** change a predicate whose prediction looks wrong — list it as a question.
3. **Re-evaluate `C1_G8_N_A1`** from its banked evidence under the corrected clause; record PASS-REEVALUATED (or FAIL) in the c1 state and the ledger, keeping the original evaluation beside it. Make the window driver resume c1 at `C1_G8_S` (do not re-run `C1_G8_N`). Prove the resume logic offline (a dry run that shows which leg it would start with, without launching anything).
4. **Re-issue `prep-boundary.json`** for the changed files and the new feature head; write the diff (old hash → new hash, reason) to `_reviews\081-26-evidence\boundary-diff.json`.
5. **Dry run again:** re-run the 081-23 dry-run suite with the corrected lib — every known-good still passes, every known-bad still fails, plus the new G8 cases.
6. **Docs:** a dated amendment in the requalification predictions (committed), journal, gotcha if a new lesson, CLAUDE.md status block; commit and push the feature branch (scratch worktree).

## Hard rules
**No Unreal launch, no build, no bench, no process control** — the owner is using the PC for rendering. Do not touch `master`, `m51`, tags, cooked containers, CaptureBench, `ToCodex\`, or existing `_reviews\` files except the 081-23 harness files this ruling authorises you to change (lib, the c1 state, the boundary) — list every changed file. No source change to the plugin. Codenames only. Conventions: no comments in code; Conventional Commits.

## Do NOT start
Windows c1–c3 (a later brief resumes them once the owner frees the PC). Stage 3.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED` — GREEN when the clause is corrected and proven, the audit is complete with no open PREDICTION-QUESTION that blocks c1–c3, C1_G8_N is re-evaluated, the boundary is re-issued and the dry run passes.
2. **Plain summary for the owner** — 3–5 short lines, no jargon.
3. The corrected clause and its two-way proof.
4. Audit table.
5. C1_G8_N re-evaluation and the resume proof.
6. Boundary diff and dry-run results.
7. Commits pushed; every harness file changed.
8. NEEDS-DECISION items — or "none".
9. Wall-clock per step.
