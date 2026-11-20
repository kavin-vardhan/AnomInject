# ACK-VERDICT: 090-10f2 (NEEDS-DECISION) was reviewed and ACCEPTED by the Code Desktop orchestrator on 2026-10-02 (_reviews\090-10f2-ruling.md). M3 shipped as release/2026-10-05 @ d7eee97. This brief starts a new, read-only workstream.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-01 natural-cause anomalies: Codex feasibility study + Code verification (read-only)

# 093-01 — Natural-cause anomalies: run Codex's feasibility study, then verify its key claims yourself. READ-ONLY

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job outlives the tool timeout: run it in the background and poll with bounded waits.
- **Never end the run while the Codex job is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean.

## Why
At today's workshop the client asked that, down the line, anomalies happen the way they do in shipped games: produced by the real reason the bug occurs, not painted on, wherever possible. The client's answers:
- every injection must still produce something visible on screen;
- bugs stay frequent;
- today's anomalies stay;
- a cause label is good to have.

Chat has written the design (`_reviews\drafts\093-00-natural-causes-design.DRAFT.md`, r2) and the Codex request (`_reviews\093-01-request.md`). This brief gets the facts checked against UE 5.1 before chat rules on the pilots.

## Bootstrap
1. Read `_reviews\drafts\093-00-natural-causes-design.DRAFT.md` and `_reviews\093-01-request.md` in full.
2. Read `_relay\README.md` (the routine) and `_relay\gdp-relay-block-v1.txt`.
3. Check the relay is free: no run folder under `_relay\runs\` may have `events.jsonl` without `exit-code.txt`. If one does, stop and report NEEDS-DECISION.

## The work
1. **Worktree.**
   - Find a clean worktree at `release/2026-10-05` `d7eee97` (for example `E:\IA_BuildCache\_r917\AnomalyInjector`). Confirm it is clean and at that commit.
   - If none exists, create one from the plugin repo with `git worktree add --detach <new folder under E:\IA_BuildCache\> d7eee97`.
   - Never change the main checkout: it is on `m51` (never touch). Note the path.
2. **Codex job** (reviewer Relay, exactly per `_relay\README.md`):
   - Run folder: `_relay\runs\2026-10-13-093-01-natural-causes-feasibility\`.
   - `prompt.txt` (UTF-8) = `_reviews\093-01-request.md` verbatim, then one clearly marked Code-added context line with the worktree path from step 1, then `gdp-relay-block-v1.txt`.
   - Guard: if `_reviews\093-01-codex-natural-causes-feasibility.md` already exists, don't launch; report NEEDS-DECISION.
   - Launch in the background: `relay_run_codex_gdp_v1.ps1 -Run <run folder>` (defaults: `gpt-6-astra`, effort `max`, `-Max 1`). Poll until `exit-code.txt` appears.
   - If the job fails (a non-zero exit, or no report in either `review.md` or `last-message.txt`), relaunch once in a new run folder with the suffix `-r2`. If that also fails, report NEEDS-DECISION with the exact error.
3. **Collect.**
   - Copy `review.md` to `_reviews\093-01-codex-natural-causes-feasibility.md`.
   - Add one header line: the run folder, the model and effort from `run-meta.txt`, and the duration.
   - Do not edit the content.
   - Append the ledger row.
4. **Your own verification** (read-only, in the 5.1 source at `D:\UESource\UnrealEngine` and in the release worktree). Don't just trust the review. Re-check at least:
   - **Q4:** every engine-fallback claim: failed material, null MI texture parameter, resource-less texture at uniform fill, missing normal map, the default asset paths and whether they are always cooked.
   - **Q5:** the streaming and VT cvars the review relies on (they exist, are runtime-safe, restore exactly), and whether a pool squeeze can hit on-screen textures first.
   - **Q2:** the probe APIs for MISS-1, MISS-2, BLUR-1, BLUR-2 and GONE-1 (they exist, are public or exported, and run on which thread).
   - **Q9:** the mask route for undrawn objects, and whether the visibility layer depends on "was rendered".
   - Anything the review marks INFERRED that decides a pilot.

   Write `_reviews\093-01-code-verification.md`:
   - one row per checked claim (CONFIRMED / CORRECTED / UNVERIFIABLE, with file:line);
   - then a short list of where you disagree with the review, and why.
5. **Do not act on the findings.** No design changes, no new briefs, no source edits. Chat rules.

## Hard rules
- **READ-ONLY:** no source edits, builds, cooks, editor or game launches, bench legs, staging, commits, branch or tag changes, or pushes.
  - The only writes allowed are the run folder(s), the two `_reviews\093-01-*` files, the ledger row, and (if needed) the new detached worktree.
- **Never change any security setting.**
- Never kill anything that is not yours, and never by name. If the Codex job must be stopped, stop only the process the relay started, by PID.
- Delete nothing, except scratch files you created, by receipt.
- Don't touch `m51`, `master`, tags, `ToCodex\` or `E:\<personal-folder>`.
- **Codenames only:** the office hosts are "the first office host" and "the second office host". Office data never comes home, and none is needed here.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means the review is collected and your verification is done.
2. **Plain summary for the owner** (3 lines, no jargon): what we can make natural soon, what needs camera movement, and what can't be done without changing the engine.
3. The Codex job: the run folder, the model and effort from `run-meta.txt`, the duration, and its VERDICT line.
4. Each pilot candidate (MISS-1, MISS-2, BLUR-1, BLUR-2, GONE-1), in one short paragraph: is it feasible, the probe, how it is made reliably visible, the revert, and the main risk.
5. Your verification: the counts (confirmed / corrected / unverifiable), then every correction, one line each.
6. The entries that need engine changes (ids).
7. In one paragraph: the review's recommended pilot order against the draft's §8, and its label-key proposal.
8. NEEDS-DECISION items, or "none".
