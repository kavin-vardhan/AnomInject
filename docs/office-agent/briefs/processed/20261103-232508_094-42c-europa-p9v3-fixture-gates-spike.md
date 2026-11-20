# ACK-VERDICT: 094-42d NEEDS-DECISION is resolved: the owner deleted D:\IA_BankOverflow on 11-03 (at most an empty parent remains); 094-42c writes P9 v3 and updates owner_p9.py and root_attribution.py; chat drops the folder from its brief text and the implementer block (its overflow rule becomes "stop and report"). 094-42r's fixture gates and FZ6(b) run here per _reviews\094-42r-ruling.md GA2-GA5.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-42c Europa P9 v3 (BankOverflow retired), outline fixture repro and guard gates, FZ6(b) shader cost

# 094-42c — Codex retires BankOverflow from the checks (P9 v3), authors the outline fixture, proves the leak and the guard, and measures the shader-route cost; then Code reviews

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:** 094-42r landed the guard and the client-only routes, but stopped before the fixture's gates, on a missing `-nowrite`. The owner has deleted `D:\IA_BankOverflow`. This job builds and launches, only through the armed kit, and only Codex starts them. **The owner may be rendering:** respect the kit's admission gates, and GA5's 3 h bound.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-42c\`.
- **Check `m51` against P9** with read-only git (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port` is at `f7b32f0`** (origin `d1cdc7b`), clean. If not, stop with NEEDS-DECISION.
- **Read only:**
  - `_reviews\094-42c-request.md`;
  - `_reviews\094-42r-ruling.md`;
  - `_reviews\094-42r-code-review.md`;
  - `_reviews\094-42d-code-findings.md`.
- **Take your own hashes** of the Paris host's `Content`, `Config`, descriptor and `.uproject`.
- **`D:\IA_BankOverflow` is gone by the owner's decision.** It is not a finding. Do not snapshot it.
- **The resume:**
  - Preserve the previous report outputs.
  - Write `resume-prompt-094-42c.txt` (UTF-8, no BOM). It holds the request verbatim, short Code-added context, and the GDP implementer block, with two edits to that block:
    - drop `D:\IA_BankOverflow` from the allowed writes;
    - replace its overflow line with: "If E: falls below its floor, stop and report; never create a new overflow folder."
  - Resume thread `01a14334`, detached, with W4, plus `-OwnerApprovedFullAccess`.
  - If the resume fails twice, start a fresh full-access thread.

## Poll
- **Cadence:** every 2–3 minutes while a build or session is likely; otherwise light loops of about 10 minutes.
- **One line per poll** in `code-receipts-094-42c\midjob-notes.txt`:
  - free space (C:, D:, E:) and commit;
  - the top memory holders when the commit is under 12 GiB;
  - any build processes;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs;
  - how long any admission gate has held.
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**

## After Codex exits: the review, `_reviews\094-42c-code-review.md`, about one page
1. **P9 v3:**
   - v3 equals v2 minus exactly the BankOverflow entries;
   - the receipt;
   - the three `owner_p9.py` lines and the `root_attribution.py` line;
   - both tests (a pass on the current disk, and a fail on a planted missing file).
2. **The fixture:** `-nowrite` was on every launch; the assets, with hashes; nothing outside `E:\IA_Fixtures\Outline425\` changed.
3. **The repro gate:** the pixel-difference numbers by your own check, and one frame where you say what the outline covers.
4. **The guard gate:** the log line, and no tagging in the receipt.
5. **FZ6(b):** the recompile numbers against the baseline; the link-risk result; the spike branch unmerged and unpushed.
6. **Safety:**
   - any crash or upload sign;
   - the Paris Content, Config, descriptor and `.uproject` hash-equal;
   - `m51` checked by git;
   - binaries changed only in the builds;
   - nothing deleted except by receipt;
   - no push.

**Where you may write:**
- `code-receipts-094-42c\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-42c-*` files;
- `D:\IA_Europa\Receipts\CodeReview09442c\`;
- `D:\IA_Europa\Temp\CodeReview09442c\`.

## Hard rules
- **No work by you:** no source edits, builds, launches, commits, pushes or history rewrites. Codex does the work, and nothing is pushed.
- **Never touch the editor, the game, their windows or their dialogs.** Never touch the owner's render processes.
- **Delete nothing.** No reparse points.
- **Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **Never print or copy a secret value.**
- **No security, paging, power or machine-setting changes. No elevation.**
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only:** "Europa" and "Europa's fork". No office data exists at home, and none may be requested.

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** P9 v3 passes, the repro shows outlines, the guard refuses, and FZ6(b) is measured.
   - **NEEDS-DECISION:** a gate is short, or GA5's bound was hit.
   - **RED:** a second crash, an upload sign, a change to the Paris Content or Config, a push, or a merged spike branch.
2. **A plain summary for the owner** (3 lines): did we reproduce the outline leak at home, does the lock stop it, and is the cheap fix route confirmed?
3. **P9 v3; the fixture; the repro; the guard; FZ6(b).**
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
