# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-42 Europa interim guard, client-only routes, outline fixture, shader-route spike

# 094-42 — Codex adds the interim custom-depth guard and client-only capture routes, re-runs the EA_Core smoke, builds an outline fixture on E: to reproduce and block the leak, and sizes the shader route; then Code reviews

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**Context:** 094-41 confirmed that the 4.25 capture's custom-depth tagging lights up a host's outline effect on the very frames we save (label leakage). Chat ruled FZ1–FZ10. This job builds and launches, but only through the armed kit, and only Codex starts them.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.** Every write uses an absolute path. Receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-42\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`): HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the 15 untracked entries. Any difference: stop with NEEDS-DECISION.
- **Check `feat/europa-port` = origin = `d1cdc7b`, clean.** If not, stop with NEEDS-DECISION.
- **Launch hold:** E: at least 40 GiB free (the fixture). The kit's own gate guards D: and memory.
- **Read only:** `_reviews\094-42-request.md`, `_reviews\094-41-ruling.md`, `_reviews\094-41-code-review.md`, `_reviews\094-41-codex-plan.md` (sections B, C and E).
- **Take your own hashes** of the Paris host's `Content`, `Config`, descriptor and `.uproject`, for the after-check.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-42.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, memory, the feature HEAD, and the fact that the 094-41 plan came from fresh thread `01a19d4a`;
    3. the GDP implementer block.
  - Resume thread `01a14334`, detached, with W4: same `-Run` and `-Workdir`, plus `-OwnerApprovedFullAccess` (this job builds; FZ8).
  - Confirm that a new turn started. If the resume fails twice, start a fresh full-access thread.

## Poll
- **Keep the poll running for the whole job:** every 2–3 minutes while a build or session is likely; otherwise light bounded loops of about 10 minutes.
- **One compact line per poll** in `code-receipts-094-42\midjob-notes.txt`:
  - free space (C:, D:, E:) and commit;
  - the top memory holders when the commit is under 12 GiB;
  - any build process, with its priority and parallel actions;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs.
- **Never end the run while Codex runs.** The cap is 24 h. **Never steer Codex.**

## After Codex exits: the review, `_reviews\094-42-code-review.md`, about one page
1. **The diff:**
   - FZ3: no planted tag can land outside the bench fixture's `EA_` actors; the scanner's three results; the refusal comes before any tag, by code path.
   - FZ4: the default is blinking + lod_corruption; the list is validated; the routes are logged and written to the receipt.
2. **The EA_Core smoke, from the raw files:** every gate by your own numbers, with the explicit four-route list.
3. **The fixture:**
   - the path, and the authoring script's hash;
   - it is outside every protected tree;
   - the repro gate: the pixel-difference numbers, and one frame where you say what the outline covers;
   - the guard gate: the log line, and no tagging in the receipt.
4. **The spike:**
   - (a) open every citation and say whether the depth-only route is buildable;
   - (b) the recompile numbers against the baseline;
   - the spike branch is unmerged and unpushed.
5. **Safety:**
   - any crash or upload sign;
   - the Paris Content, Config, descriptor and `.uproject` hash-equal, by your own hashes;
   - `m51` by git;
   - binaries changed only in the builds;
   - nothing deleted except by receipt;
   - no push.

**Where you may write:**
- `code-receipts-094-42\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-42-*` files;
- `D:\IA_Europa\Receipts\CodeReview09442\`;
- `D:\IA_Europa\Temp\CodeReview09442\`.

## Hard rules
- **No source edits, builds, launches, commits, pushes or history rewrites by you.** Codex does the work; nothing is pushed by anyone.
- **Never touch the editor, the game, their windows or their dialogs.**
- **Delete nothing.** No reparse points.
- **Never print or copy a secret value.**
- **No security, paging, power or machine-setting changes. No elevation.**
- **Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only:** "Europa" and "Europa's fork". No office data exists at home and none may be requested.

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** the smoke, the repro and the guard gates pass by your own numbers, and both spike results are reported.
   - **NEEDS-DECISION:** a gate is short, or a spike result is missing.
   - **RED:** a second crash, an upload sign, a change to the Paris Content or Config, a push, or a merged spike branch.
2. **A plain summary for the owner** (3 lines): is the safety lock in, and which fix route do the numbers point to?
3. **FZ3, FZ4; the smoke,** per gate; **the fixture,** with the repro and the guard; **the spike,** (a) and (b).
4. **Safety,** or "none".
5. **Decisions chat must make,** or "none".
