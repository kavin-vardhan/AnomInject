# ACK-VERDICT: 094-36r NEEDS-DECISION is ruled in _reviews\094-36-ruling.md (FU1-FU6): the 4.25 capture path is accepted on the real map (census parity at scale, no tagging interference by the switch test, exact restore with the game's own actors untouched, player view, 5.1 schema); real-scene label-free grading moves off the port's path as dataset-quality items D1-D5 for both engines; 094-37 cleans old receipts on D: by receipt (owner-approved), builds the pending fixes, adds 5.1's far plane and lost-batch timeout, a parity read of the four anomalies and a capture self-check line, runs a short street-map smoke, then Code writes the pre-push scan and the office capture card.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-37 Europa finish the 4.25 capture port: D: cleanup, parity fixes, self-check line, street-map smoke; then Code's pre-push scan and office capture card

# 094-37 — Finish the 4.25 capture port for the office: Codex cleans D: by receipt, builds the fixes and runs a short smoke; then Code reviews, runs the pre-push scan and writes the office capture-smoke card (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

**The owner's priority:** finish the port fast, under the lean profile (FJ1), and get to the office test.

This job builds the Paris host's 4.25 targets and may launch the 4.25 editor and game. It does so only through the armed kit, and only Codex starts them.

## Start (quick)
- **The watcher starts you inside the protected `m51` checkout.**
  - Every write you make uses an absolute path.
  - Your receipts go in `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-37\`.
- **Check `m51` against P9 with read-only git** (`GIT_OPTIONAL_LOCKS=0`):
  - HEAD `53bf725`, index `526aba7f`;
  - 0 tracked changes, and the 15 untracked entries.
  - Any difference: stop with NEEDS-DECISION.
- **Launch hold:** E: at least 15 GiB free. D: is low (about 20 GiB) until Codex's cleanup in step 2 of the request. Don't hold the resume for D:; the kit's own gate guards the session.
- **Read only:**
  - `_reviews\094-37-request.md`;
  - `_reviews\094-36-ruling.md`;
  - `_reviews\094-36-code-review.md`;
  - `_reviews\D-lite-office-card-europa.md`.
- **Take your own hashes** of the Paris host's `Content`, `Config`, descriptor and `.uproject`, for the after-check.
- **The resume:**
  - Preserve the run folder's previous report outputs.
  - Write `resume-prompt-094-37.txt` (UTF-8, no BOM), containing:
    1. the request, verbatim;
    2. short Code-added context: `m51` status, free space, memory, and the feature HEAD;
    3. the GDP implementer block.
  - Resume thread `01a14334`, detached, with W4: same `-Run` and `-Workdir`, plus `-OwnerApprovedFullAccess`.
  - Confirm that a new turn started. If the resume fails twice, start a fresh thread.

## Poll
- **Keep your poll running for the whole job.**
  - Every 2–3 minutes while a session is likely.
  - Otherwise, light bounded loops of about 10 minutes.
- **One compact line per poll** in `code-receipts-094-37\midjob-notes.txt`:
  - free space and commit;
  - any build process, with its priority and parallel actions;
  - our editor, game, crash-monitor and ShaderCompileWorker PIDs.
- **Never end the run while Codex runs.** The cap is 24 h.
- **Never steer Codex.**

## After Codex exits: three documents

### 1. The short review: `_reviews\094-37-code-review.md`, about one page
1. **The cleanup:**
   - the receipt against FU4's scope and keep-list;
   - spot-check that `P9`, `09434`+, the referenced paths, the FH2 probe files and 09427e's contact sheets still exist;
   - the space freed.
2. **The diff,** against the cited 5.1 code:
   - the lost-batch timeout and discard;
   - the far plane;
   - `coverage_pct`;
   - G500;
   - the self-check line;
   - check Codex's parity read for the four anomalies against the 5.1 source.
3. **The smoke, from the raw files:** every FU5 gate, by your own numbers. Open one mask per anomaly, and say what it covers.
4. **Safety:**
   - any crash or upload sign;
   - the Paris Content, Config, descriptor and `.uproject` hash-equal, by your own hashes;
   - `m51`, by git;
   - the binaries changed only in the builds;
   - nothing deleted outside FU4's scope.

### 2. The pre-push scan: `_reviews\094-37-prepush-scan.md` (only if the smoke passes)
- **What to scan:** what pushing `feat/europa-port` would send, for the commits since `4f46a9c`.
- **Check for:**
  - secrets;
  - account identifiers;
  - codenames (no real title, studio or fork name, and no office codenames);
  - binaries or large files.
- **Same method as 094-29's scan.** Never print a secret value; report locations only.

### 3. The office card: `_reviews\094-37-office-card-europa-capture.md` (only if the smoke passes)
- **Extend the D-lite card,** in the office-rdp-card style.
- **Pin the build** to the smoke's passing commit (detached checkout).
- **The steps:**
  1. build;
  2. a short seeded auto-injection capture (4242, the four smoke anomalies) on an ordinary level of Europa's fork, from the player's own view;
  3. the reads.
- **Typed reads only, under the HARD RULE:** no office data comes home. Each read has its expected value and what to do if it differs. The reads:
  - the self-check line;
  - event counts per anomaly;
  - frame and mask counts;
  - whether `annotation.json` is present;
  - "crash: none".
- **Disk space:** say how much the capture needs.

**Where you may write:**
- `code-receipts-094-37\`;
- the resume prompt;
- the preserved report outputs;
- the `_reviews\094-37-*` files;
- `D:\IA_Europa\Receipts\CodeReview09437\`;
- `D:\IA_Europa\Temp\CodeReview09437\`.

## Hard rules
- **No source edits, builds, launches, commits or pushes by you.**
- **Never touch the editor, the game, their windows or their dialogs.**
- **Delete nothing.** FU4's cleanup is Codex's step, not yours. No reparse points. Never open files inside a foreign drive-root entry.
- **Never print or copy a secret value.**
- **No security, paging, power or machine-setting changes. No elevation.**
- **Kill nothing,** except, if necessary, the relay's Codex process, by PID.
- **Don't touch:** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow`, `E:\UEProjects\LyraStarterGame`, `E:\Unreal Projects\StylizedParisStreet`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message
1. **`VERDICT: GREEN | NEEDS-DECISION | RED`.**
   - **GREEN:** every FU5 gate passes by your own numbers, the cleanup stayed in scope, and the scan and card are written.
   - **NEEDS-DECISION:** a gate is short, or a fix couldn't be made quickly.
   - **RED:** a second crash, an upload sign, a change to the Paris Content or Config, or a deletion outside FU4's scope.
2. **A plain summary for the owner** (3 lines): is the 4.25 port ready for the office capture test, and how much space did the cleanup free?
3. **The cleanup; the parity read; the smoke,** per gate.
4. **The scan:** findings, or "clean". **The card:** the pinned commit, and its read list.
5. **Safety,** or "none".
6. **NEEDS-DECISION items,** or "none".
