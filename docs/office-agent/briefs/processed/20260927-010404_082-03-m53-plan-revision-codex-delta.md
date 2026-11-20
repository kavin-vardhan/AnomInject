# ACK-VERDICT: 083-03 NEEDS-DECISION ruled in _reviews\082-03-chat-ruling-codex-m53-design-review.md - the existing 082-02 review is the owner's desktop run (no re-run); run-meta.txt accepted; Codex's 12 findings all accepted, D1/D7 revised
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 082 m53 plan revision + Codex delta (Code, Opus 5.5)

# 082-03 — m53: revise the plan for Codex's 12 findings and chat's revised rulings (PLAN ONLY), then run Codex's delta check through the Relay

You are Claude Code, running **headless** from the GDP mailbox. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, to offer options, or because a milestone is done or the run has been long — put status notes in the same message as your next tool call and keep going. End the run only when (a) every step below is done, (b) a stop condition below fires, or (c) something you need is deliberately protected from you or needs the owner. Every final message follows the structure at the end and starts with a `VERDICT:` line. **The Codex delta check can run for hours:** launch it in the background and poll it with bounded PowerShell wait loops; never end the run while Codex is still running (cap 14 h, per the Relay README). If the Bash tool stalls on this box, use the PowerShell tool.

## Context
- Your m53 plan (`946c1bf`) went to Codex for a design review. The owner ran it in the desktop app, and it came back **CHANGES REQUIRED**: 5 P1, 6 P2, 1 P3, all source-backed.
- Chat accepted every finding and revised its own rulings: no force-residency, no automatic route-A fallback, host-MID clones refused by default.
- You built the GDP Relay in 083-03. This brief is its first real use.

## Read first
1. **Chat's disposition: `_reviews\082-03-chat-ruling-codex-m53-design-review.md`.** It is the spec for the revision.
2. **Codex's review: `_reviews\082-02-codex-m53-design-review.md`.** Every finding has locations, engine evidence and a fix.
3. Your plan and journal on `feat/m53-uv-normal-corruption`, and chat's earlier `082-02-chat-ruling-m53-plan-decisions.md`.
4. `D:\IntrusiveAnomalies\_relay\README.md` and the runner.

## Part 1 — revise the plan (PLAN ONLY)
1. Revise `docs/predictions/2026-09-26-m53-uv-normal-corruption.md` to resolve **every** finding as the ruling lists it, or record a reasoned, source-cited counter-proposal against that finding for chat to rule on.
   - Add a finding-by-finding **resolution table** at the top: finding → plan section and lines → resolution or counter-proposal.
   - Keep superseded text under a SUPERSEDED fold, not deleted, as the m55 plan did.
2. Re-state S1–S4, the build and cook count, and the **expected yield impact** of the stricter refusals (not-resident, over-budget, host-MID, atomic coherence), using the 082-01 content scan.
3. A short journal; commit and push to the branch (scratch worktree). No tag.

## Part 2 — Codex delta check through the Relay
1. Run folder `D:\IntrusiveAnomalies\_relay\runs\2026-09-27-082-03-m53-delta-check\`.
   - `prompt.txt` = `_reviews\082-03-codex-m53-delta-check-request.md` verbatim, plus one line naming the new plan head commit, plus the relay block.
   - Launch it with the runner in the background and poll it.
2. Copy `review.md` to `_reviews\082-03-codex-m53-delta-check.md`, with the header line (run folder, model, effort, duration). Append a row to the ledger.
3. **Do not act on the delta check.** Chat rules first.

## Hard rules
- **Plan only:** no source change, no build, no cook, no game launch, no bench.
- Never kill anything that is not yours, and never by name.
- Don't touch `m51`, `master`, tags, cooked containers, CaptureBench, `ToCodex\` or `E:\<personal-folder>`.
- Codenames only.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN = the revision is pushed, every finding is resolved or has a counter-proposal, and the delta check is collected.
2. **Plain summary for the owner**, 4–6 short lines. Say what changed about the new anomaly, what got stricter, the expected yield impact, and what Codex's delta check says.
3. Resolution table, condensed: finding → resolution / counter-proposal.
4. Revised S1–S4 and counts.
5. The delta check: duration, model and effort, the `VERDICT:` line, and each finding's RESOLVED / PARTIAL / NOT status, plus any new findings.
6. Commits pushed.
7. NEEDS-DECISION items (counter-proposals chat must rule on), or "none".
