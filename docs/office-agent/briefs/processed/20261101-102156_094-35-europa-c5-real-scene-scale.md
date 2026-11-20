# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-35 Europa C5 real-scene scale with 5.1 parity (census, custom-depth restore, player view, annotations) on the Paris street map with a moving view: Codex implements and runs; Code gives a short review

# 094-35 — C5: real-scene scale for 4.25 capture with 5.1 parity, on the Paris street map with a turn-and-hold view and a clean pre-pass, under the lean profile. Resume Codex, then give a short review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner's priority:**
- capture, fast, under the lean profile (FJ1);
- **labels and masks in sync with what is visible is the most important part of the plugin.**

This job builds the Paris host's 4.25 targets and may launch the 4.25 editor and game. It does so only through the armed kit, and only Codex starts them.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.**
  - Every write you make uses an absolute path.
  - Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-35\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`):
  - HEAD `53bf725`, index `526aba7f`;
  - 0 tracked changes, and the 15 untracked entries.
  - Any difference: stop with NEEDS-DECISION.
- **Launch hold:** D: at least 25 GiB free, and E: at least 15 GiB.
- **Read only:**
  - `_reviews\094-35-request.md`;
  - `_reviews\094-34-ruling.md`;
  - `_reviews\094-34-code-review.md`.
- **Take your own hashes** of the Paris host's `Content`, `Config`, descriptor and `.uproject`, for the after-check. The street map must stay byte-identical.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-35.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, memory, and the feature HEAD;
    3. the GDP implementer block.
  - Resume thread `01a14334`, detached, with W4: same `-Run` and `-Workdir`, plus `-OwnerApprovedFullAccess`.
  - Confirm that a new turn started. If the resume fails twice, start a fresh thread.

## Poll
- **Keep your poll running for the whole job.**
  - Every 2–3 minutes while a session is likely.
  - Otherwise, light bounded loops of about 10 minutes.
- **One compact line per poll** in `code-receipts-094-35\midjob-notes.txt`:
  - free space and commit;
  - any build process, with its priority and parallel actions;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs.
- **Never end the run while Codex runs.** The cap is 24 h.
- **Never steer Codex.**

## The short review (after Codex exits)
Write `_reviews\094-35-code-review.md`, about one page.

1. **The diff,** against the cited 5.1 code:
   - the census, with its floor and ceiling;
   - the ledger restore;
   - the player-view capture;
   - the annotation schema;
   - the bench driver and pre-pass are off by default.
2. **The C5 session, from the raw files.** Fix your label-free rule before you read the results.
   - FR3: the poses, references and unstable maps were written before the scored pass started (timestamps). Any dropped pose follows the rule.
   - The scored events per anomaly, with both edges in scored frames.
   - C2, by your own label-free check against the pose references, on the affected label: 0 mismatches.
   - IoU, precision, spill and completeness, against the frozen gates.
   - Both negatives.
   - The census:
     - the peak visible count is above 55;
     - at least one scored event targets an actor not visible at session start;
     - no ledger exhaustion.
   - Custom depth:
     - the before, mid and after samples, including the driver's non-default actors;
     - the reserved stencil value was never handed out.
   - The view matches the player camera on every frame.
   - The annotation schema, against 5.1's.

   Open one mask per anomaly, and say what it covers, including any partly occluded target.
3. **Safety:**
   - any crash or upload sign;
   - the Paris Content (including the street map), Config, descriptor and `.uproject` hash-equal, by your own hashes;
   - `m51`, by git;
   - the binaries changed only in the builds.
4. **What's left before an office capture smoke on Europa's fork,** in 2–3 lines.

**Where you may write:**
- `code-receipts-094-35\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-35-*` files;
- `D:\IA_Europa\Receipts\CodeReview09435\`;
- `D:\IA_Europa\Temp\CodeReview09435\`.

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.**
- **Never touch the editor, the game, their windows or their dialogs.**
- **Delete nothing.** No reparse points. Never open files inside a foreign drive-root entry.
- **Never print or copy a secret value.**
- **No security, paging, power or machine-setting changes. No elevation.**
- **Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** every FS2 gate passes, by the readers and by your own numbers.
   - **NEEDS-DECISION:** a gate is short, or a fix couldn't be made quickly.
   - **RED:** a second crash, an upload sign, or a change to the Paris Content or Config.
2. **A plain summary for the owner** (3 lines): does capture handle a full real scene with a moving camera on 4.25, as the office game will need?
3. **C5:**
   - per anomaly;
   - the census;
   - custom depth;
   - the view;
   - the annotations.
4. **Safety,** or "none".
5. **NEEDS-DECISION items,** or "none".
