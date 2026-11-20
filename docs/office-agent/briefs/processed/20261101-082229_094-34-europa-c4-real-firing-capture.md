# ACK-VERDICT: 094-33c NEEDS-DECISION is ruled in _reviews\094-33c-ruling.md (FR1-FR4): C1-C3 are accepted on stock 4.25 (all four anomalies pass FQ2 and FQ3, clean frames carry 0 mask pixels, C2 holds by Code's label-free check); FR2 adds a 0.95 receiver-completeness term to lighting, re-scored from the kept evidence; FR3 fixes C2 references before each run; 094-34 is C4, capture through the real firing path with the test-scene coupling removed.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-34 Europa C4 capture through the real firing path on 4.25: Codex removes the test-scene coupling, runs seeded auto-injection capture at two resolutions; Code gives a short review

# 094-34 — C4: capture through the real firing path on 4.25, with the test-scene coupling removed (FR4), plus lighting's completeness re-score (FR2). Under the lean profile. Resume Codex, then give a short review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner's priority:**
- capture, fast, under the lean profile (FJ1);
- **labels and masks in sync with what is visible is the most important part of the plugin.**

This job builds the Paris host's 4.25 targets and may launch the 4.25 editor and game, but only through the armed kit, and only Codex starts them.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.**
  - Every write you make uses an absolute path.
  - Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-34\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries.
  - Any difference: stop with NEEDS-DECISION.
- **Launch hold:** D: at least 25 GiB and E: at least 15 GiB.
- **Read only** `_reviews\094-34-request.md`, `_reviews\094-33c-ruling.md` and `_reviews\094-33c-code-review.md`.
- **Take your own hashes** of the Paris host's `Content`, `Config`, descriptor and `.uproject`, for the after-check.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-34.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, memory, and the feature HEAD;
    3. the GDP implementer block.
  - Resume thread `01a14334` (same `-Run` and `-Workdir`, `-OwnerApprovedFullAccess`), detached, with W4. Confirm that a new turn started. If the resume fails twice, start a fresh thread.

## Poll
- **Keep your poll running for the whole job,** and catch short sessions by polling every 2–3 minutes while one is likely.
- Otherwise, light bounded loops of about 10 minutes. One compact line per poll in `code-receipts-094-34\midjob-notes.txt`:
  - free space and commit;
  - any build process, with its priority and parallel actions;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs.
- Never end the run while Codex runs. The cap is 24 h. Never steer Codex.

## The short review (after Codex exits)
Write `_reviews\094-34-code-review.md` (about one page):
1. **FR2:**
   - recompute lighting's completeness from the kept 094-33c files;
   - confirm the real mask passes, and that a single object and all-but-the-smallest fail.
2. **FR4:**
   - read the diff against the cited 5.1 target-binding and stencil-ledger code;
   - run the static check yourself: no fixture names, no fixed tags, and no hard resolution in plugin Source.
3. **Both C4 sessions, from the raw files:**
   - the events fired per anomaly;
   - C2 by your own label-free check (0 mismatches);
   - IoU, precision, spill and completeness per anomaly, against the frozen gates;
   - both negatives;
   - the same-seed sequence check;
   - FR3's reference rule was written before the session.
   Open one mask per anomaly and say what it covers.
4. **Safety:**
   - any crash or upload sign;
   - the Paris Content, Config, descriptor and `.uproject` hash-equal, from your own hashes;
   - `m51` by git;
   - the binaries changed only in the builds.
5. **What's left before an office capture smoke on Europa's fork,** in 2–3 lines.

**Where you may write:** `code-receipts-094-34\`, the resume prompt, the preserved report outputs, the `_reviews\094-34-*` files, `D:\IA_Europa\Receipts\CodeReview09434\` and `D:\IA_Europa\Temp\CodeReview09434\`.

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
   - **GREEN:** FR2 holds, the static check is clean, and both C4 sessions pass every gate, by the readers and by your own numbers.
   - **NEEDS-DECISION:** a gate is short, or a fix couldn't be made quickly.
   - **RED:** a second crash, an upload sign, or a Paris Content or Config change.
2. **Plain summary for the owner** (3 lines): does capture now work on whatever's on screen, as it will at the office?
3. **FR2;** then **C4** per session and per anomaly (events, C2, masks), the negatives and the same-seed check.
4. **Safety** ("none" if none).
5. **NEEDS-DECISION items,** or "none".
