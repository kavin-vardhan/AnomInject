# ACK-VERDICT: 093-09 (RED, session killed) is ruled in _reviews\093-09-ruling.md. The PC went down at about 04:29 IST mid-job. PF-1 and PF-2 (12/12, plus the 0.813 replay) had passed per Codex's stream. This brief recovers the bench, continues from PF-3, and reviews both portions.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-10 J2: crash recovery, continue from PF-3, full review (Codex resumes, Code reviews after)

# 093-10 — Recover after the PC shutdown, resume Codex's J2 session from PF-3, then review 093-09 and 093-10

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job takes many hours. Launch it **detached** and poll with bounded waits of **10 minutes or less**.
- **Never end the run while it is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean; roll at 70 %.

## Why
- **What happened** (`_reviews\093-09-ruling.md`):
  - At about 04:29 IST the PC went down while 093-09 was running.
  - Codex's process died without an exit code, and the previous mailbox session ended with no report.
  - Per Codex's event stream, by then it had: fixed the commit dates and `fovDeg`, built candidate `eff797c` (8 builds, 0/0), passed PF-1, passed PF-2 12/12 (after a launcher-marker harness repair), reproduced the 0.813 can-fail, and produced the reader's healthy PASS. It was about to repeat PF-3 after diagnosing a viewport-setup mismatch.
- **This brief:** resume the same Codex session to recover and continue (X2), then review **both** the 093-09 and 093-10 work (X3).
- **Branch policy (093-02 P10, amended by K3):** no natural-cause code. Nothing merges into `master`, a release branch or any delivery branch.

## Rules carried over
- **W3:** while Codex runs, write notes only in `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\code-receipts-093-10\`. Publish `_reviews\093-10-code-review.md` only after Codex exits. If something must change Codex's course, stop the job by PID (only the relay-started process) and report.
- **W4:** launch the relay with `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` removed from the child environment, and report whether your own environment has them.

## Bootstrap
1. Read in full:
   - `_reviews\093-09-ruling.md` and `_reviews\093-10-request.md`;
   - `_reviews\093-08-ruling.md` and `_reviews\093-08-code-review.md`;
   - Codex's last report, `report.md` in the J2 run folder (it covers 093-08).
2. Read `_relay\README.md` and `relay_resume_codex_impl_v1.ps1`; for the fallback, `relay_run_codex_gdp_impl_v1.ps1` and `gdp-impl-block-v1.txt`.
3. **The dead attempt.** The J2 run folder has `events.jsonl` but **no** `exit-code.txt`, because the 093-09 attempt was killed by the shutdown.
   - Confirm no Codex process for thread `<uuid>` is alive (process list, by command line).
   - Write a note `code-receipts-093-10\dead-attempt-093-09.txt` saying what you found. **Never fabricate an exit code.** Then treat the relay as free.
   - If any other run folder has `events.jsonl` without `exit-code.txt`, stop and report NEEDS-DECISION.
4. Before launch, take your own baselines: staging inventory, both `Content` trees, refs and `ls-remote`, worktree heads, processes, host binaries.

## The work
1. **Resume prompt.** `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\resume-prompt-093-10.txt` (UTF-8, through `cmd /c` redirection) =
   - `_reviews\093-10-request.md` verbatim;
   - then clearly marked Code-added context lines (paths only);
   - then `gdp-impl-block-v1.txt`.
2. **Resume detached** with the two `GIT_*_DATE` variables removed. Run `relay_resume_codex_impl_v1.ps1 -Run D:\IntrusiveAnomalies\_relay\runs\2026-10-14-093-06-j2-office-robust-f1 -Workdir E:\IA_BuildCache\_r53_src\AnomalyInjector -ResumePrompt <that file> -OwnerApprovedFullAccess`, with the defaults.
   - Poll until a new `exit-code.txt` appears.
   - **If the resume script refuses** (for example, because of the missing exit code), don't edit any relay script. Launch a new implementer job instead:
     - run folder `_relay\runs\<date>-093-10-j2-crash-recovery-continue\`;
     - `-Sandbox danger-full-access -OwnerApprovedFullAccess -OrchestratorBrief 093-10-j2-crash-recovery-continue.md`;
     - prompt = the 093-06 to 093-10 requests in order, then a line pointing at the reports, the old run's `events.jsonl` and `E:\IA_BuildCache\_r936` to `_r939`, then the impl block.
3. **On a capacity or interruption failure:** the relay retries by itself. If it gives up, resume once more with a short "continue" prompt. If that fails too, report NEEDS-DECISION.
4. **After Codex exits: your review**, in `_reviews\093-10-code-review.md`, one row per item (Codex result → your check → OK / GAP). It has two parts.

   **Part A, the 093-09 portion** (from git, receipts and the saved sessions):
   - W4: the date re-creation (trees equal, mapping in the journal, no 2001 dates anywhere).
   - W1: the `fovDeg` diff.
   - Builds and audits at `eff797c`.
   - PF-1, recounted.
   - PF-2: recount at least three of the 12 legs, including Lyra Y/942×869 and StackOBot Y/1280×720 (the marker case).
   - The marker repair, its two attempts, and the retained failed capture.
   - The 0.813 replay.
   - The healthy reader PASS. Run the reader yourself on that session and quote the line.

   **Part B, the 093-10 work:**
   - The crash-recovery receipt.
   - The candidate rehash.
   - The PF-3 repeat and its can-fail.
   - PF-4.
   - PF-5 and its can-fail.
   - J2 recounts: the reframed selected-viewport positive on Lyra, the resize leg (W2 accounting), one staged leg, and the J1 UV regression.
   - The camera switch: the gate and the readings.
   - The can-fails really failed.
   - Harness fixes: each harness-only, at most two attempts, no gate, threshold or prediction changed.
   - Restoration: against **your own** baselines.
   - Push: only if every gate is green, `fix/followup-capture` only, fast-forward. Confirm with `ls-remote`.
5. **Do not fix anything.** Chat rules.

## Hard rules
- **You yourself:** no editor or game launch, no build, cook or stage, and no git write. Running the read-only office reader on saved sessions is allowed.
- **Never change any security setting.** If `dbghelp.dll` blocks the cook, Codex reports it; nobody works around it.
- **Never kill anything that is not yours, and never by name.** You may stop only a Codex process the relay started, by PID. Leave the owner's Codex desktop processes, UE 5.7, Blender and his other apps alone. The owner may be using the PC today.
- Delete nothing, except your own scratch, by receipt.
- Don't touch `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\` or `E:\<personal-folder>`.
- **Codenames only.** Office data never comes home.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner** (3 lines, no jargon).
3. **The dead-attempt note and your environment check** (the `GIT_*_DATE` variables).
4. **The Codex job:** resumed or new, duration, its VERDICT line, heads pushed, hashes, harness fixes.
5. **Your review:** Part A and Part B counts (OK / GAP), then each GAP in one line.
6. **The reader lines:** healthy PASS, null-mask FAIL, and the two box reads.
7. **Ready for office checkpoint I?** Yes or no, and what the owner needs:
   - the build to pull;
   - cases 11–14 on the second office host;
   - Python 3.10+ with numpy and Pillow;
   - the contact-sheet glance (`g=`);
   - one box read on an older delivered-build session per host.
8. NEEDS-DECISION items, or "none".
