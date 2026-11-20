# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-31 Europa C1 plugin frames on 4.25: Codex implements, builds and runs the C1 gate; Code gives a short review

# 094-31 — C1: plugin frames on 4.25. Implement the plan's C1 list, build both 4.25 targets, and pass the C1 gate in two real sessions, under the lean profile. Resume Codex, then give a short review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner's priority:** capture, fast, under the lean profile (FJ1). This job builds the Paris host's 4.25 targets and may launch the 4.25 editor and game, but only through the armed kit, and only Codex starts them.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.**
  - Every write you make uses an absolute path.
  - Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-31\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries.
  - Any difference: stop with NEEDS-DECISION.
- **Launch hold:** D: at least 25 GiB and E: at least 15 GiB.
- **Read only** `_reviews\094-31-request.md`, `_reviews\094-30-ruling.md` and `_reviews\094-30-code-review.md`.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-31.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, memory, and the feature HEAD;
    3. the GDP implementer block.
  - Resume thread `01a14334` (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4. Confirm that a new turn started. If the resume fails twice, start a fresh thread.
- **Before Codex starts building,** take your own hashes of the Paris host's `Content`, `Config`, descriptor and `.uproject`, for the after-check.

## Poll
- Light, bounded loops of about 10 minutes. One compact line per poll in `code-receipts-094-31\midjob-notes.txt`:
  - free space and memory;
  - any build (UnrealBuildTool or compiler) process and its priority class;
  - our editor, game and crash-monitor PIDs.
- Never end the run while Codex runs. The cap is 24 h. Never steer Codex.

## The short review (after Codex exits)
Collect Codex's report to `_reviews\094-31-codex-report.md`, then write `_reviews\094-31-code-review.md` (about one page):
1. **The build:**
   - the first-build error counts per module, from the log;
   - the final build is clean for both targets;
   - UnrealBuildTool ran at below-normal priority.
2. **The diff:**
   - the 5.1 paths are intact behind their guards;
   - SHADERS and CONTROL_SERVER are still 0 on 4.25;
   - FL3's adapter rule is implemented, and the negative test covers padded, undersized and 0×0;
   - G488.
3. **Both C1 sessions, from the raw files:**
   - count 60, with no missing or duplicate index;
   - 1280×720, no black frame, and the check region correct;
   - the RHI and dropped count;
   - a normal exit.
   Open 3 frames per session and say what they show.
4. **Safety:**
   - any crash;
   - any upload sign;
   - the Paris Content, Config, descriptor and `.uproject` hash-equal, from your own before and after hashes;
   - `m51` by git at the end;
   - the binaries changed only during the build, and the manifest was re-bound.
5. **The next blocker for C2** (labels), in one line.

**Where you may write:** `code-receipts-094-31\`, the resume prompt, the preserved report outputs, the two `_reviews\094-31-*` files, `D:\IA_Europa\Receipts\CodeReview09431\` and `D:\IA_Europa\Temp\CodeReview09431\`.

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
   - **GREEN:** both C1 sessions pass.
   - **NEEDS-DECISION:** a build or session failure that couldn't be fixed.
   - **RED:** a second crash, an upload sign, or a Paris Content or Config change.
2. **Plain summary for the owner** (3 lines): does the plugin capture frames on 4.25 yet?
3. **The build:** first-build error counts, the fixes, and the final result.
4. **The sessions:** one line each.
5. **Safety** ("none" if none).
6. **NEEDS-DECISION items,** or "none".
