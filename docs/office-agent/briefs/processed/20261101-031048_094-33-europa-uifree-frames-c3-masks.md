# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-33 Europa UI-free 4.25 frames (5.1 parity) with C1 and C2 re-run, then C3 masks: Codex implements, builds and runs; Code gives a short review

# 094-33 — UI-free frames on 4.25 (FO2) with the C1 and C2 gates re-run, then C3 masks with the frozen per-anomaly metrics (FO3), under the lean profile. Resume Codex, then give a short review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner's priority:**
- capture, fast, under the lean profile (FJ1);
- **labels and masks in sync with what is visible is the most important part of the plugin.**

This job builds the Paris host's 4.25 targets and may launch the 4.25 editor and game, but only through the armed kit, and only Codex starts them.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.**
  - Every write you make uses an absolute path.
  - Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-33\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries.
  - Any difference: stop with NEEDS-DECISION.
- **Launch hold:** D: at least 25 GiB and E: at least 15 GiB.
- **Read only:**
  - `_reviews\094-33-request.md`;
  - `_reviews\094-32-ruling.md`;
  - `_reviews\094-32-code-review.md`;
  - the plan's §C3, in the port worktree's `docs/europa/PHASEC-capture-port-plan.md`.
- **Take your own hashes** of the Paris host's `Content`, `Config`, descriptor and `.uproject`, for the after-check.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-33.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, memory, and the feature HEAD;
    3. the GDP implementer block.
  - Resume thread `01a14334` (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4. Confirm that a new turn started. If the resume fails twice, start a fresh thread.

## Poll
- Light, bounded loops of about 10 minutes. One compact line per poll in `code-receipts-094-33\midjob-notes.txt`:
  - free space and commit;
  - any build process, with its priority and parallel actions;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs.
- Never end the run while Codex runs. The cap is 24 h. Never steer Codex.

## The short review (after Codex exits)
Write `_reviews\094-33-code-review.md` (one to two pages):
1. **FO2:**
   - check the cited 4.25 hook against source;
   - open 3 frames per session and confirm the heartbeat text is gone, against the control;
   - recount the re-run C1 gate (both sessions) and C2 gate from the raw files;
   - run the three one-frame-shift negatives yourself again.
2. **FO3:**
   - the shader compile and load evidence;
   - recompute each anomaly's metrics yourself from the raw masks and frames, against the frozen thresholds;
   - run both negatives (one-frame mask shift, wrong target) yourself;
   - open one mask per anomaly and say what it covers.
3. **FO4:** `docs/gotchas.md` is restored and appended, not rewritten.
4. **Safety:**
   - any crash or upload sign;
   - the Paris Content, Config, descriptor and `.uproject` hash-equal, from your own hashes;
   - `m51` by git;
   - the binaries changed only in the builds.
5. **What's left before an office capture smoke on Europa's fork,** in 2–3 lines.

**Where you may write:** `code-receipts-094-33\`, the resume prompt, the preserved report outputs, the `_reviews\094-33-*` files, `D:\IA_Europa\Receipts\CodeReview09433\` and `D:\IA_Europa\Temp\CodeReview09433\`.

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.**
- **Never touch the editor, the game, their windows or their dialogs.**
- **Delete nothing.** No reparse points. Never open files inside a foreign drive-root entry.
- **Never print or copy a secret value.**
- **No security, paging, power or machine setting changes. No elevation.**
- **Kill nothing** except, if necessary, the relay's Codex process, by PID.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow`, `E:\UEProjects\LyraStarterGame` or `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
   - **GREEN:** FO2's re-runs and C3 all pass, by the readers and by your own numbers.
   - **NEEDS-DECISION:** a stage failed and couldn't be fixed quickly, or FO2's fallback was used.
   - **RED:** a second crash, an upload sign, or a Paris Content or Config change.
2. **Plain summary for the owner** (3 lines): are frames clean like on 5.1, and do the masks line up?
3. **FO2:** the hook, the heartbeat check, and the C1 and C2 re-runs, in one line each.
4. **C3:** per anomaly, the metric against its threshold, plus the negatives.
5. **Safety** ("none" if none).
6. **NEEDS-DECISION items,** or "none".
