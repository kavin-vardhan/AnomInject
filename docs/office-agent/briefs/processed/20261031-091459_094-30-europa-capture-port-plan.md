# ACK-VERDICT: 094-29 (D-lite prep: pre-push scan and office card) does not gate this planning-only job, whatever its verdict; chat reviews 094-29 separately.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-30 Europa Phase C capture port plan (owner's priority): Codex plans frames, labels-in-sync and masks on 4.25; Code gives a short review

# 094-30 — Plan the capture port to 4.25 (Phase C), on the owner's priority. Planning only, with no editor launch. Resume Codex, then give a short review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner's priority (10-31):**
- capture comes next, because that's where an older engine will bite;
- the lean profile (FJ1) applies;
- label sync is the most important part of the plugin.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.**
  - Every write you make uses an absolute path.
  - Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-30\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries.
  - Any difference: stop with NEEDS-DECISION.
- **Read only** `_reviews\094-30-request.md`, plus `_reviews\094-27c-ruling.md`'s FJ1.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-30.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, and the feature HEAD;
    3. the GDP implementer block.
  - Resume thread `01a14334` (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4. Confirm that a new turn started. If the resume fails twice, start a fresh thread.

## Poll
- Light, bounded loops of about 10 minutes. One compact line per poll in `code-receipts-094-30\midjob-notes.txt`.
- **Expected:** no `UE4Editor` and no game. A UnrealBuildTool or compiler process is allowed only for the request's optional scratch compile. Record its argv and output folder.
- Never end the run while Codex runs. The cap is 12 h. Never steer Codex.

## The short review (after Codex exits)
Collect Codex's report to `_reviews\094-30-codex-report.md`, then write `_reviews\094-30-code-review.md` (about one page):
1. **Spot-check 6 of the inventory's "different/missing" rows** against 4.25 source.
2. **If a scratch compile ran:**
   - its output stayed in owned scratch;
   - the Paris host's binaries, Content and Config are hash-equal before and after, from your own hashes of those folders;
   - the error list matches the log.
3. **The stages and gates:**
   - is each gate concrete and measurable in one real session?
   - is C2 (labels in sync) strong enough to catch a one-frame offset?
4. **The C1 file list:** complete against the compile errors or the inventory? Name anything missing.
5. **Your top 3 risks,** if they differ from Codex's.

**Where you may write:** `code-receipts-094-30\`, the resume prompt, the preserved report outputs, the two `_reviews\094-30-*` files, and `D:\IA_Europa\Temp\CodeReview09430\`.

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.**
- **Delete nothing.** No reparse points.
- **Never print or copy a secret value.**
- **No security, paging, power or machine setting changes. No elevation.**
- **Kill nothing** except, if necessary, the relay's Codex process, by PID.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow`, `E:\UEProjects\LyraStarterGame` or `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
   - **GREEN:** the plan is written and committed, with concrete gates and a C1 file list.
   - **RED:** a protected or Paris change, or a launch.
2. **Plain summary for the owner** (3 lines): what porting capture to 4.25 will take, and the biggest risk.
3. **The stages and gates,** one line each, with Codex's time estimate.
4. **The measured compile errors,** in counts by module, or why there was no compile.
5. **The top risks,** Codex's and yours.
6. **NEEDS-DECISION items,** or "none".
