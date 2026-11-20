# ACK-VERDICT: 081-31 NEEDS-DECISION ruled in _reviews\081-32-chat-ruling-legacy-identity-amendment-5.md - legacy CB-N: A coalesced-arm unmeasured frame excused under signature+cap (pre-m55 mechanism), B tags compared modulo one leg-wide bijection, C cross-pair clause honours the historical run-unique set; CB-N re-evaluated from the bank; bench-free
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 081 m55 legacy AMENDMENT 5 (Code, Opus 5.5)

# 081-32 — m55 requalification: legacy-identity AMENDMENT 5, prove it both ways, re-evaluate CB-N from the bank, re-issue the boundary (BENCH-FREE)

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. If the Bash tool stalls on this box, use the PowerShell tool.

## Context
081-31 ran c1 from `C1_G2_S`:
- All 14 remaining stack legs passed, including the first runtime proofs of the refusal census and G10.
- Legacy group CB-N FAILed on three independent phenomena, which your cause read separated: A = a coalesced-arm unmeasured frame on CB_N2, B = a bijective tag relabel on CB_N5, C = `census_cycles` flagged by the cross-pair clause.

Chat has ruled. Branch head `cfff033`.

## Read first
1. **The ruling: `_reviews\081-32-chat-ruling-legacy-identity-amendment-5.md`.** It is the spec. Implement Rulings A, B and C exactly, run the pre-conditions and all 8 proofs.
2. `_mailbox\to_chat\081-31-m55-windows-c1-c3-run.report.md` and `_reviews\081-31-evidence\c1-legacy-cb-n-cause-read.md`.
3. The harness (`_reviews\081-23-*.py`), `081-23-evidence\prep-boundary.json`, `081-23-evidence\c1\legacy-CB-N-comparison.json`, `081-22-evidence\legacy-historical-run-unique.json`, the P-C7 v2/v3 bijection code, and how 081-26 re-evaluated `C1_G8_N` from the bank and how 081-30 re-issued the boundary.

## The work
1. **Pre-conditions (ruling A):**
   - run the bank-wide signature search, with counts per binary and evidence state;
   - run the source diff on the PixelOwner path.

   If the search trips the stop rule, stop there with NEEDS-DECISION.
2. Declare **AMENDMENT 5** in the requalification predictions: dated, cites the ruling, and says plainly that it touches legacy groups only, with no stack predicate, threshold or prediction changed. Then implement A, B and C in the lib.
3. **Proofs 1–6**, expected vs actual.
   - Any proof that doesn't come out as expected is a stop condition. Report it; do not bend the proof.
   - Proof 6 (no verdict flips across all earlier legacy comparisons) is mandatory in full.
4. **Proof 7:** re-evaluate CB-N from the bank → PASS-REEVALUATED, recorded with the 081-26 mechanism. If it is not PASS under the amended rules, stop and report.
5. **Proof 8:** boundary re-issue and a full dry run, all green. c1 resumes at `CB_S1`. Give the exact invocation and budget tag for 081-33.
6. **Docs:**
   - journal;
   - gotchas: the pre-existing coalesced-arm yield issue, with the FUTURE fix note, plus G294 marked resolved;
   - CLAUDE.md status block.

   Commit and push the feature branch (scratch worktree).

## Hard rules
- **BENCH-FREE.** Do not launch the game, a window, a chunk or any leg. Work only on banked records.
- No build and no plugin source change.
- Never kill anything that is not yours, and never by name.
- Never touch `master`, `m51`, tags, cooked containers, CaptureBench or `ToCodex\`.
- Never touch existing `_reviews\` files other than the harness files this ruling changes and the ledger row the re-evaluation mechanism writes. Put new evidence under `_reviews\081-32-evidence\`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN = pre-conditions clear, AMENDMENT 5 declared and implemented, proofs 1–8 as expected, CB-N PASS-REEVALUATED, boundary re-issued, dry run green, pushed.
2. **Plain summary for the owner**, 4–6 short lines.
3. Pre-conditions: signature counts per binary and evidence state; source-diff result.
4. Proof table: 8 rows, expected vs actual, evidence path.
5. CB-N re-evaluation: the A excusal (lines quoted), the B π, the C excusal.
6. Boundary: old → new hashes and feature head; dry-run count; the exact 081-33 invocation.
7. Deviations.
8. Commits pushed.
9. NEEDS-DECISION items, or "none".
