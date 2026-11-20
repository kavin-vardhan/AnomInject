# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-02 natural foundation + MISS-1/MISS-2: Codex plan + Code plan review (read-only)

# 093-02 — Natural-cause foundation and the first two pilots: run Codex's planning job, then review the plan against chat's ruling. READ-ONLY

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job outlives the tool timeout and the 2-hour background-task limit. Launch it **detached**, as 093-01 did, and poll with bounded waits.
- **Never end the run while the Codex job is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean.

## Why
093-01 is done and ACCEPTED: chat's ruling is `_reviews\093-01-ruling.md`. The next step is a file-by-file plan before any code, covering:
- the base branch;
- the natural-attempt foundation;
- MISS-1 (controlled calibration);
- MISS-2 (natural).

Codex writes the plan, because it will implement it later through the implementer Relay. You review the plan against the ruling, chat rules, and the owner gives one go/no-go.

## Bootstrap
1. Read `_reviews\093-01-ruling.md` and `_reviews\093-02-request.md` in full. Skim `_reviews\093-01-codex-natural-causes-feasibility.md` and `_reviews\093-01-code-verification.md`.
2. Read `_relay\README.md` and `_relay\gdp-relay-block-v1.txt`.
3. Check the relay is free: no run folder under `_relay\runs\` may have `events.jsonl` without `exit-code.txt`. If one does, stop and report NEEDS-DECISION.

## The work
1. **Worktree.** Reuse `E:\IA_BuildCache\_r917\AnomalyInjector` (release `d7eee97`). Confirm it is clean and at that commit.
2. **Codex job** (reviewer Relay, exactly per `_relay\README.md`):
   - Run folder: `_relay\runs\2026-10-13-093-02-natural-foundation-plan\`.
   - `prompt.txt` (UTF-8) = `_reviews\093-02-request.md` verbatim, then one clearly marked Code-added context line with the worktree path, then `gdp-relay-block-v1.txt`.
   - Guard: if `_reviews\093-02-codex-natural-foundation-plan.md` already exists, don't launch; report NEEDS-DECISION.
   - Launch detached with the defaults (`gpt-6-astra`, effort `max`, `-Max 1`). Poll until `exit-code.txt` appears.
   - If the job fails (a non-zero exit, or no report in either `review.md` or `last-message.txt`), relaunch once in a new run folder with the suffix `-r2`. If that also fails, report NEEDS-DECISION with the exact error.
3. **Collect.**
   - Copy `review.md` to `_reviews\093-02-codex-natural-foundation-plan.md`.
   - Add one header line: the run folder, the model and effort from `run-meta.txt`, and the duration.
   - Do not edit the content.
   - Append the ledger row.
4. **Your plan review** (read-only). Write `_reviews\093-02-code-plan-review.md`, covering:
   - **Rulings R1–R11:** does the plan honour each one? Give a table: ruling → where in the plan → OK / GAP / CONFLICT.
   - **The F1 assessment:** check the plan's claims about the saved 091-06 work-in-progress yourself. Run `git apply --check` of `_reviews\2026-10-10-disk-cleanup\r916-091-06-uncommitted-wip.patch` against `1eaf205`, in a new detached worktree under `E:\IA_BuildCache\`. Do not build.
   - **Gates:** every gate has a can-fail; PIE and staged are both covered; no gate is circular.
   - **Label keys:** additive only, and the example records are consistent with the shipped `docs/client-readme.md`.
   - **Job split:** each job fits one bench session and keeps one bench, one implementer.
   - **Your top 5 risks,** and anything the plan gets wrong, with evidence.
5. **Do not act on the plan.** No source edits, no new briefs. Chat rules.

## Hard rules
- **READ-ONLY:** no source edits, builds, cooks, editor or game launches, bench legs, staging, commits, branch or tag changes, or pushes.
  - The only writes allowed are the run folder(s), the two `_reviews\093-02-*` files, the ledger row, and the new detached worktree for the patch check.
- **Never change any security setting.**
- Never kill anything that is not yours, and never by name. If the Codex job must be stopped, stop only the process the relay started, by PID. Leave any other `codex.exe` alone.
- Delete nothing, except scratch files you created, by receipt.
- Don't touch `m51`, `master`, tags, `ToCodex\` or `E:\<personal-folder>`.
- **Codenames only.** Office data never comes home, and none is needed here.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means the plan is collected and your review is done.
2. **Plain summary for the owner** (3 lines, no jargon): what gets built first, roughly how many Codex jobs, and anything the owner must do at the office.
3. The Codex job: the run folder, the model and effort from `run-meta.txt`, the duration, and its VERDICT line.
4. The plan in 8–10 lines: base, identity choice, foundation pieces, MISS-1, MISS-2, gates, jobs.
5. Your review: the counts (OK / GAP / CONFLICT), then each GAP or CONFLICT in one line.
6. Any new module dependencies proposed.
7. Owner product questions the plan raises, for example the controlled/natural UI shape.
8. NEEDS-DECISION items, or "none".
