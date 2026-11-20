# ACK-VERDICT: 093-14 (NEEDS-DECISION) is ruled in _reviews\093-14-ruling.md. The camera fix and all 42 PIE cases pass on 9781031, and the TSR boxes measure inside tolerance. The TSR setup-proof matcher cannot pass on a sparse-mask fixture, so AC1 rebuilds it from the MASK-SERVED receipts. AC2 routes new evidence to physical D:. Then Lyra TSR, cook, staged, push.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-15 J2: TSR setup matcher fix, Lyra TSR, cook, staged, push (Codex resumes, Code reviews after)

# 093-15 — Resume Codex's J2 session: fix the TSR setup matcher, then Lyra TSR, cook, every staged gate and the push

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job takes many hours. Launch it **detached** and poll with bounded waits of **10 minutes or less**.
- **Never end the run while it is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean; roll at 70 %.

## Why
- 093-14 passed the drop-aware camera controls (StackOBot re-assessed, Lyra PIE) and all 42 fresh PIE ledger cases on `9781031`.
- The dedicated StackOBot TSR half-resolution leg measured box agreement X 0.99611 / Y 0.99448 (n = 96).
- It stopped on the setup proof. `assess_z2.reduced` assumes request id = sample + 1, but on this fixture masks are armed only on labelled samples (your observation A).
- Chat's ruling, `_reviews\093-14-ruling.md`:
  - **AC1:** requests map to samples through the leg's own `MASK-SERVED` receipts.
    - The predicate: every estimator row is served by a half-size pass, and every pass is half-size.
    - The can-fails: a mutated receipt with one full-size pass, and the 100 % leg.
    - Attempt 1 of 2.
    - The retained StackOBot TSR leg is re-assessed with it; no engine re-run.
  - **AC2:** new evidence goes to physical D:. Free space is checked on the resolved volume, and the 50 GiB floor applies on every volume written. The incomplete 093-14 archive stays untouched.
  - **AC3:** your observations. C (the TSR scale margin) is a reading and an open item.
  - **AC4–AC5:** the order and the stop rules.
- **Branch policy** (093-02 P10, amended by K3): no natural-cause code. Nothing merges into `master`, a release branch or any delivery branch.

## Rules carried over
- **W3:** while Codex runs, write notes only in `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\code-receipts-093-15\`.
  - Publish `_reviews\093-15-code-review.md` only after Codex exits.
  - If something must change Codex's course, stop the job by PID (only the relay-started process) and report.
- **W4:** launch the relay with `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` removed from the child environment. Report whether your own environment has them.

## Bootstrap
1. Read in full:
   - `_reviews\093-14-ruling.md` and `_reviews\093-15-request.md`;
   - `_reviews\093-14-code-review.md`;
   - Codex's last report, `report.md` in the J2 run folder (it covers 093-14).
2. Read `_relay\README.md` and `relay_resume_codex_impl_v1.ps1`.
3. **Check the run folder:**
   - Confirm no Codex process for thread `<uuid>` is alive.
   - Confirm the run folder holds the 093-14 `exit-code.txt`.
   - Add the attempt-to-file mapping to `code-receipts-093-15\attempt-numbering.txt`. 093-14 was the 9th physical attempt, and the resume script will rename its file to `exit-code-attempt8.txt`. Change no script.
4. Before launch, take your own baselines:
   - staging inventory, both `Content` trees, refs and `ls-remote`, worktree heads, processes, host binaries, host config;
   - free space on the resolved E: and D: volumes.

## The work
1. **Resume prompt:** `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\resume-prompt-093-15.txt` (UTF-8, through `cmd /c` redirection), made of:
   - `_reviews\093-15-request.md` verbatim;
   - then clearly marked Code-added context lines (paths only);
   - then `gdp-impl-block-v1.txt`.
2. **Resume detached** with the two `GIT_*_DATE` variables removed:
   - run `relay_resume_codex_impl_v1.ps1 -Run D:\IntrusiveAnomalies\_relay\runs\2026-10-14-093-06-j2-office-robust-f1 -Workdir E:\IA_BuildCache\_r53_src\AnomalyInjector -ResumePrompt <that file> -OwnerApprovedFullAccess`, with the defaults;
   - poll until a new `exit-code.txt` appears.
3. **On a capacity or interruption failure:**
   - the relay retries by itself;
   - if it gives up, resume once more with a short "continue" prompt;
   - if that fails too, report NEEDS-DECISION. Don't edit any relay script.
4. **After Codex exits: your review**, in `_reviews\093-15-code-review.md`. One row per item: Codex result → your check → OK / GAP.
   - **Pre-flight:** your own candidate rehash and restoration state, plus free space on the resolved volumes.
   - **The freeze:** AC1 and AC2 committed before the re-assessment and the first leg. The reader change is limited to the request-to-sample mapping.
   - **The re-assessed StackOBot TSR leg:** rebuild the AC1 setup proof yourself from the `MASK-SERVED` receipts. Both can-fails must reject.
   - **Lyra TSR, PIE:**
     - box agreement, n and scale per axis (`pf2_mine.py`), and the 0.96 mutants;
     - the AC1 setup proof, which **you test against this sparse-mask fixture yourself**.
   - **The cook:** the staged EXE and DLLs rehash equal to the manifest.
   - **The staged gates:** recount at least:
     - one leg per fixture;
     - the staged camera switch with the drop-aware control (both fixtures, if run);
     - a staged 50 % pairing leg (Z2 parts 1, 2 and 4);
     - a staged PF-2 case;
     - a staged TSR leg with its AC1 proof;
     - a staged resize leg under W2.
   - **Can-fails:** confirm they really failed.
   - **Harness fixes:** each harness-only and within budget. TSR matcher at most 2; camera donor 1 of 2 used; pairing recipe, packaging metadata and marker spent.
   - **Disk:** every write went to physical D: as AC2 requires, and the floors held. Confirm the incomplete 093-14 archive is untouched.
   - **Docs:** the gotcha extension, the open items, the journal, the status block.
   - **Restoration:** against **your own** baselines.
   - **Push:** only if every gate is green, `fix/followup-capture` only, fast-forward. Confirm with `ls-remote`.
   - **If J2 qualifies:** run the office reader yourself on one new healthy J2 session from `9781031`, and quote the line.
5. **Do not fix anything.** Chat rules.

## Hard rules
- **You yourself:**
  - no editor or game launch;
  - no build, cook or stage;
  - no git write.
  - Running read-only readers on saved sessions is allowed.
- **Never change any security setting.** If `dbghelp.dll` blocks the cook, Codex reports it; nobody works around it.
- **Never kill anything that is not yours, and never by name.**
  - You may stop only a Codex process the relay started, by PID.
  - Leave alone the owner's Codex desktop processes, UE 5.7, Blender, any `UnrealTraceServer` daemon, and his other apps.
  - The owner may be using the PC.
- Delete nothing, except your own scratch, by receipt.
- Don't touch `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\` or `E:\<personal-folder>`.
- **Codenames only.** Office data never comes home.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner** (3 lines, no jargon).
3. **Your environment check** (the `GIT_*_DATE` variables) and the attempt numbering.
4. **The Codex job:** duration, its VERDICT line, commits with times, heads pushed, hashes, harness fixes.
5. **TSR:** the re-assessed StackOBot setup proof, and Lyra's box agreement and setup proof (PIE and staged).
6. **The cook and the staged matrix:** the hashes, then the per-case table (counts).
7. **Your review:** counts (OK / GAP / OPEN), then each GAP in one line.
8. **The reader lines:** the healthy PASS from a new J2 session on `9781031` (if J2 qualified), and any others you ran.
9. **Ready for office checkpoint I?** Yes or no. If yes, give the pushed head and what the owner needs:
   - the build to pull;
   - cases 11–14 on the second office host;
   - Python 3.10+ with numpy and Pillow;
   - the contact-sheet glance (`g=`);
   - no window resize during a session;
   - one box read on an older delivered-build session per host.
10. NEEDS-DECISION items, or "none".
