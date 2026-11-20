# ACK-VERDICT: 094-04 NEEDS-DECISION is ruled in _reviews\094-04-ruling.md (EE1-EE7): a narrow C: exception for 4.25 UHT's own settings, the host Target.cs probe rooting ratified, a 4.25-only PCH policy, C5043 handling, 5.1 capability pass-through, tooling fixes, and the owner's E:\IA_BuildCache cleanup with P9 v2.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 094-05 Europa A3 resume and build cache cleanup: Code cleans, Codex implements, Code reviews after

# 094-05 — Clear the owner's build cache and write P9 v2, then resume Codex on Europa job A3, then review after it exits (W3)

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run.
- Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner.
- The final message follows the structure at the end and starts with a `VERDICT:` line.
- Launch the Codex job **detached**, as 094-04 did, and poll with bounded waits.
- **Never end the run while the Codex job is still running.** The cap is 12 h after launch; past that, leave it running and report.
- Keep context lean. Your 094-04 predecessor reached 70 % of its context, so summarize big outputs into receipts instead of printing them.

## Why
- **094-04 stopped correctly** on a tiny C: write by 4.25's header tool. Chat ruled it in `_reviews\094-04-ruling.md` (EE1–EE7).
- **The owner (15:35 IST):** "there's again more than 350gb in the build cache folder. We need to clear that up. The only focus now is porting."
  - EE7 makes that your job: clear `E:\IA_BuildCache` by receipt before Codex starts, and write P9 v2.
  - J2 is shelved. Its code stays in git.
- **Then Codex resumes A3** from `668c075` under `_reviews\094-05-request.md`.

## Bootstrap
1. Read in full:
   - `_reviews\094-04-ruling.md`;
   - `_reviews\094-05-request.md`;
   - `_reviews\094-04-request.md`;
   - `_reviews\094-04-code-review.md`.
2. Skim `_reviews\094-04-codex-report.md`: the summary, the working-compile section and the resume point.
3. Reuse your predecessor's tooling in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-04\`: `snap.ps1`, `launch.ps1`, `poll.ps1`, `window_scan.py`, `p9_rehash.py`, and its midjob-notes for the launch recipe.

## The work

### Step 0. The machine, headroom and toolchain
Same as 094-04 Step 0, except the E: audit:
- relay free; watcher state; uptime;
- headroom: C: 13 GiB or more, D: 70 or more, E: 25 or more (ED1);
- toolchain facts: 14.38.33130, 14.16.27023 (compiler 14.16.27051), the SDKs, and the Visual Studio version (still 17.12 expected);
- load: the owner's heavy processes, recorded only.

### Step 1. Clear `E:\IA_BuildCache` by receipt (EE7)
Receipts go to `code-receipts-094-05\cleanup\`.

1. **Inventory before:**
   - each top-level child, and each child of `_binary_baselines` and `StackOBot`, with size, file count and newest time;
   - every reparse point inside, with its target;
   - every D: junction that points into `E:\IA_BuildCache` (expected: `D:\IntrusiveAnomalies\StackOBot\Builds`, `Intermediate` and `Saved`, plus `D:\IntrusiveAnomalies\_binary_baselines` and `_bench_sessions_bank`);
   - the git worktrees inside (expected: `AnomalyInjector1` `_r53_host`, `2` `_r53_src`, `3` `_r933`, `4` `_r52p_src`, `7` `_r917`).
2. **The keep list (never deleted):**
   - `_binary_baselines\release-2026-10-05-23098fa-218BA4C7`, hashed before and after;
   - any `StackOBot\Builds` subfolder named for the release (`release-2026-10-05`, `23098fa` or `d7eee97`);
   - every folder a D: junction targets, emptied but kept.
3. **Save work first, for each worktree in the delete set:**
   - With `GIT_OPTIONAL_LOCKS=0`, record `git status --porcelain=v1 --untracked-files=all`.
   - If anything tracked is modified, save `git diff --binary` as a patch.
   - Zip untracked non-build source and docs (`.cpp .h .cs .py .ps1 .md .json .ini .uplugin .uproject`, outside `Binaries`, `Intermediate`, `Saved` and `DerivedDataCache`) if their total is under 50 MB; otherwise list them.
   - Check that each detached HEAD is reachable from a ref (`git branch -a --contains` or `git merge-base --is-ancestor`). If one is not, keep that worktree and report it.
   - **Do not run `git worktree prune`.**
4. **Nothing running from the delete set:** check process image paths and command lines. If something is, skip that root and report it. Never kill anything.
5. **Delete:**
   - Remove each reparse point in the delete set as a link first (`cmd /c rmdir "<link>"`, without `/s`), and check that its target's inventory is unchanged. One link is expected to point into `D:\IA_BankOverflow`, which stays.
   - Then delete each real folder with `cmd /c rd /s /q`.
   - **Never use PowerShell `Remove-Item -Recurse`**: it can follow junctions.
   - If `rd` reports paths that are too long, retry that root once with the `\\?\` prefix (D2 attempt 1).
   - Order: the `_r*` folders, then the rest of `_binary_baselines`, then `_bench_sessions_bank`'s contents, then the contents of `StackOBot\Builds`, `Intermediate` and `Saved`.
6. **Inventory after:** what is left, and E:'s free space before and after (expect about 360 GiB more).
7. **If your own safety controls refuse a delete:** don't work around them.
   - Stop the cleanup there.
   - Write `cleanup\OWNER-DELETE-LIST.md`, with the exact folders still to delete in Explorer (Shift+Delete) after this job ends.
   - Build P9 v2 only for what is actually gone, or keep v1 if nothing is.
   - Say which in the Code-added context.

### Step 2. P9 v2 (EE7)
Write to `D:\IA_Europa\Receipts\P9\baseline-v2\`:
- `full-sha256.jsonl`: v1 (`D:\IA_Europa\Receipts\P9\baseline\full-sha256.jsonl`, `db884f1e…`) minus every line whose path lies under a deleted root (case-insensitive, normalized). No re-hash.
- `removed-manifest.jsonl`: the removed lines verbatim.
- `v2-receipt.json`:
  - the deleted roots and the kept paths;
  - line counts and bytes for v1, v2 and removed;
  - the SHA-256 of all three files;
  - a check that v1 = v2 + removed, by sorted merge.
- The kept release baseline's lines stay in v2.

### Step 3. Pre-launch snapshots and the resume
- **Your own before-snapshots,** as in 094-04, taken after the cleanup, with the deleted roots dropped from your protected lists.
- **Preserve** 094-04's `report.md`, `last-message.txt` and `exit-code.txt`, hash-equal, in `code-receipts-094-05\`.
- **W4.**
- **Resume prompt** `resume-prompt-094-05.txt`, UTF-8 with no BOM. Contents, in order:
  1. `_reviews\094-05-request.md` verbatim;
  2. clearly marked Code-added context:
     - free space now;
     - the toolchain facts;
     - **the cleanup result:** what was deleted and kept, sizes, any skipped root and why, any saved patch or zip;
     - **the P9 v2 paths and SHA-256s,** or "v1 still applies";
     - verified paths;
  3. the GDP implementer block, with the usual note that the request's binding order governs.
- **Resume thread `01a14334`** with `relay_resume_codex_impl_v1.ps1 -Run D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan -Workdir D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan -ResumePrompt <file> -OwnerApprovedFullAccess`, detached, through a wrapper with W4 applied.
- **Record** the process tree and the events offset.
- **If the resume fails twice:** start a fresh implementer thread as in 094-04 Step 1, and report it.

### Step 4. Poll (W3)
- Bounded waits until `exit-code.txt` appears.
- Mid-job notes go only to `code-receipts-094-05\`. Never steer Codex.
- Note free space at each poll.
- Stop Codex only by PID, and only if necessary.

### Step 5. Collect and review (after Codex exits)
**Collect:**
- Copy Codex's report to `_reviews\094-05-codex-report.md`, with a header line.
- Append the ledger row.

**Write `_reviews\094-05-code-review.md`, covering:**
- **The rulings:** EE1–EE7 and ED1–ED7, plus the EC, EB and EA items they touch, as a table: ruling → evidence → OK / GAP / OPEN.
- **The cleanup:** your receipts summarized; the keep list intact (the release baseline's hashes unchanged); every junction target intact; `D:\IA_BankOverflow` and J2's Lyra host untouched.
- **P9 v2:** Codex's re-derivation against yours. Your independent re-hash of the protected exe, dll, pak, ucas and utoc that remain, plus 20 random remaining files; worktree heads, index hashes and refs; `m51` clean.
- **The engine trees:**
  - before/after listings;
  - from every UBT log: `-NoEngineChanges`, and engine-module and engine-plugin action counts (expected 0, now including 5.1);
  - any ED4 metadata-cache changes, against the 100 MB limit.
- **EB1 routing:** in-window scans, the EE1 UHT classes only, foreign writes separated.
- **EE3–EE6:** each with its evidence. For EE5, the 5.1 tokens of Register, the registry, the asset finders and the unknown-ID responses, against `d7eee97`.
- **The closure:** `git diff --stat 668c075..feat/europa-port`, and read every source diff against C1–C5.
- **Working compiles:** read every compile log; 0 errors on the same final SHA on both engines; the warnings list with causes; the C5043 counts.
- **JsonExport:** re-run the full set, with the three mutants, through the approved wrapper on the final source, and compare.
- **Self-tests:** the ledger (including the revived budget suite) and the idle gate.
- **Your top 5 risks** for A4a and A4b.

## Hard rules
- **No source edits, builds, editor or game launches, commits or pushes by you.** The only exceptions are the JsonExport re-run, EA11, the ancestry scans and the self-test re-runs, all through the approved tooling.
- **Your only deletions are EE7's,** by receipt, exactly as listed. Never delete anything outside `E:\IA_BuildCache`, and never anything on the keep list.
- **The only writes allowed to you:**
  - `code-receipts-094-05\` (the cleanup receipts, patches and zips included);
  - `D:\IA_Europa\Receipts\P9\baseline-v2\`;
  - the resume prompt;
  - the two `_reviews\094-05-*` files;
  - the ledger row;
  - your JsonExport re-run outputs under `D:\IA_Europa\Receipts\CodeReview09405`.
- **P9 applies in full** to everything that remains.
- **`E:\Unreal Projects\StylizedParisStreet` is read-only.**
- **Never change any security setting. No ETW, no USN, no elevation.**
- **Kill nothing** except, if necessary, the Codex process the relay started, by PID. Never kill by name.
- **Don't touch** `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\`, `E:\<personal-folder>`, `D:\IA_BankOverflow` or `E:\UEProjects\LyraStarterGame`.
- **Codenames only:** "Europa" and "Europa's fork".

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`. GREEN means:
   - the cleanup is done as ruled;
   - A3 is complete, with both working compiles at 0 errors on the same final SHA;
   - there is no blocking GAP;
   - P9 v2 is unchanged.
2. **Plain summary for the owner** (3 lines, no jargon). One line says how much space came back on E:.
3. **The cleanup:** deleted and kept (sizes), anything skipped and why, and E:'s free space before and after.
4. **P9 v2:** counts and hashes; Codex's re-derivation.
5. **The Codex job:** run folder, thread, model and effort, duration, exit, and its VERDICT line.
6. **A3** in 8–10 lines: the fixes, the two compiles, and the EE items.
7. **Your review:** the counts, each GAP or OPEN in one line, and your independent P9, engine-tree and routing results.
8. Anything the owner must do.
9. NEEDS-DECISION items, or "none".
