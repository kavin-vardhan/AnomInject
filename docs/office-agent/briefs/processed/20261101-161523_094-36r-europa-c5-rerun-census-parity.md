# ACK-VERDICT: 094-36 RED was a login failure before any work (Claude Code's OAuth session had expired; 0 turns, nothing ran, nothing changed); this is 094-36 unchanged, re-staged after the owner's re-login. 094-35 NEEDS-DECISION is ruled in _reviews\094-35-ruling.md (FT1-FT6): C5's census, exact restore, player view and 5.1 schema are accepted; nothing could be scored because FS2's 5 % pre-pass rule was ill-posed (grain, wind, fog, sky) and the 4.25 census re-tags every actor every frame, flipping a strip at the wall pose; this job restores 5.1's batch holding, leaves the game's own custom-depth actors alone, proves no interference with the census on vs off, removes the grain for scoring, scores stable pixels only, fixes the annotation values, and re-runs C5.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-36 Europa C5 re-run: 4.25 census parity, no-interference proof, well-posed pre-pass, annotation values; Codex implements and runs; Code gives a short review

# 094-36 — C5 re-run on the Paris street map: 4.25 census parity, proof that the census changes no pixels, a well-posed pre-pass, and the annotation values, under the lean profile. Resume Codex, then give a short review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner's priority:**
- capture, fast, under the lean profile (FJ1);
- **labels and masks in sync with what is visible is the most important part of the plugin.**

This job builds the Paris host's 4.25 targets and may launch the 4.25 editor and game. It does so only through the armed kit, and only Codex starts them.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.**
  - Every write you make uses an absolute path.
  - Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-36\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`):
  - HEAD `53bf725`, index `526aba7f`;
  - 0 tracked changes, and the 15 untracked entries.
  - Any difference: stop with NEEDS-DECISION.
- **Launch hold:** D: at least 25 GiB free, and E: at least 15 GiB.
- **Read only:**
  - `_reviews\094-36-request.md`;
  - `_reviews\094-35-ruling.md`;
  - `_reviews\094-35-code-review.md`.
- **Take your own hashes** of the Paris host's `Content`, `Config`, descriptor and `.uproject`, for the after-check. The street map must stay byte-identical.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-36.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, memory, and the feature HEAD;
    3. the GDP implementer block.
  - Resume thread `01a14334`, detached, with W4: same `-Run` and `-Workdir`, plus `-OwnerApprovedFullAccess`.
  - Confirm that a new turn started. If the resume fails twice, start a fresh thread.

## Poll
- **Keep your poll running for the whole job.**
  - Every 2–3 minutes while a session is likely.
  - Otherwise, light bounded loops of about 10 minutes.
- **One compact line per poll** in `code-receipts-094-36\midjob-notes.txt`:
  - free space and commit;
  - any build process, with its priority and parallel actions;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs.
- **Never end the run while Codex runs.** The cap is 24 h.
- **Never steer Codex.**

## The short review (after Codex exits)
Write `_reviews\094-36-code-review.md`, about one page.

1. **The diff,** against the cited 5.1 code:
   - batch holding until readback;
   - event targets tagged for the whole event;
   - the game's own custom-depth actors skipped;
   - the bench census switch is off by default;
   - the annotation values;
   - the FR3 reader fix.
2. **The session, from the raw files.** Fix your rules before you read the results.
   - **FR3:** the maps were frozen before the first scored frame was written.
   - **The grain setting:** cited and recorded on every row. Static content is bit-stable in the census-off holds.
   - **Per pose:**
     - the stable share;
     - the self-check;
     - kept or dropped, by the rule.
   - **Interference:**
     - your own census-on vs census-off comparison per pose, with pose 3's strip in particular;
     - any z-fighting claim, checked against its evidence.
   - **Per anomaly:**
     - the scored events;
     - C2 by your own label-free check on the affected label;
     - IoU, precision, spill and completeness against the frozen gates;
     - both negatives.
   - **The census:**
     - more than 55 drawn actors;
     - a scored late-entering target;
     - no ledger exhaustion.
   - **Custom depth:**
     - the before, mid and after samples;
     - the reserved value;
     - the game's own custom-depth actors never changed.
   - **The view and the annotation schema and values,** against 5.1.

   Open one mask per anomaly, and say what it covers.
3. **Safety:**
   - any crash or upload sign;
   - the Paris Content (including the street map), Config, descriptor and `.uproject` hash-equal, by your own hashes;
   - `m51`, by git;
   - the binaries changed only in the builds.
4. **What's left before an office capture smoke on Europa's fork,** in 2–3 lines.

**Where you may write:**
- `code-receipts-094-36\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-36-*` files;
- `D:\IA_Europa\Receipts\CodeReview09436\`;
- `D:\IA_Europa\Temp\CodeReview09436\`.

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
   - **GREEN:** every gate passes, by the readers and by your own numbers.
   - **NEEDS-DECISION:** a gate is short, or a fix couldn't be made quickly.
   - **RED:** a second crash, an upload sign, or a change to the Paris Content or Config.
2. **A plain summary for the owner** (3 lines): does capture now handle a full real scene with a moving camera on 4.25, without our tagging changing the picture?
3. **C5:**
   - interference, per pose;
   - per anomaly;
   - the census;
   - custom depth;
   - the view;
   - the annotations.
4. **Safety,** or "none".
5. **NEEDS-DECISION items,** or "none".
