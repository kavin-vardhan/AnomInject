# ACK-VERDICT: 093-10 (NEEDS-DECISION) is ruled in _reviews\093-10-ruling.md. The pairing stop is ruled a probe-lighting problem in the test recipe (no pairing slip in evidence), to be proven by a one-factor control. PF-1 to PF-5 and the 32 passed J2 cases stand for eff797c. This brief continues J2 under Y2–Y6 and reviews.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-11 J2: pairing-recipe control, rest of the matrix, camera switch, cook, staged, push (Codex resumes, Code reviews after)

# 093-11 — Resume Codex's J2 session: prove the pairing cause, then finish J2 to the push

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job takes many hours. Launch it **detached** and poll with bounded waits of **10 minutes or less**.
- **Never end the run while it is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean; roll at 70 %.

## Why
- 093-10 stopped at the first native pairing leg (`stack_P_pair_native_100`): NEITHER on all 56 decidable frames, PREVIOUS 0.
- Chat's ruling, `_reviews\093-10-ruling.md`:
  - **Y1:** your diagnosis is upheld as a reading. At the AA-off arbiter lighting, the probe's camera-facing face renders black against a black sky, so the reader can't see it. The mask sits on the cube at its current position. No pairing slip is in evidence.
  - **Y2:** Codex proves the cause with a one-factor control under a frozen decision table, then runs every pairing leg at the adopted recipe.
    - C1 is the gate: the J2 recipe minus the three lighting overrides.
    - C2 is a reading: the failed recipe with motion blur off.
  - **Y3–Y6:**
    - the B19 correction;
    - your observations B–E;
    - the spent marker budget;
    - the order.
- **Branch policy** (093-02 P10, amended by K3): no natural-cause code. Nothing merges into `master`, a release branch or any delivery branch.

## Rules carried over
- **W3:** while Codex runs, write notes only in `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\code-receipts-093-11\`.
  - Publish `_reviews\093-11-code-review.md` only after Codex exits.
  - If something must change Codex's course, stop the job by PID (only the relay-started process) and report.
- **W4:** launch the relay with `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` removed from the child environment. Report whether your own environment has them.

## Bootstrap
1. Read in full:
   - `_reviews\093-10-ruling.md` and `_reviews\093-11-request.md`;
   - `_reviews\093-10-code-review.md`;
   - Codex's last report, `report.md` in the J2 run folder (it covers 093-10).
2. Read `_relay\README.md` and `relay_resume_codex_impl_v1.ps1`.
3. **Check the run folder:**
   - Confirm no Codex process for thread `<uuid>` is alive (process list, by command line).
   - Confirm the run folder holds the 093-10 `exit-code.txt`.
   - The resume script will rename it to `exit-code-attempt4.txt`, though it was the fifth attempt (Y4-D). Write the attempt-to-file mapping in `code-receipts-093-11\attempt-numbering.txt`. Change no script.
4. Before launch, take your own baselines: staging inventory, both `Content` trees, refs and `ls-remote`, worktree heads, processes, host binaries.

## The work
1. **Resume prompt:** `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\resume-prompt-093-11.txt` (UTF-8, through `cmd /c` redirection), made of:
   - `_reviews\093-11-request.md` verbatim;
   - then clearly marked Code-added context lines (paths only);
   - then `gdp-impl-block-v1.txt`.
2. **Resume detached** with the two `GIT_*_DATE` variables removed:
   - run `relay_resume_codex_impl_v1.ps1 -Run D:\IntrusiveAnomalies\_relay\runs\2026-10-14-093-06-j2-office-robust-f1 -Workdir E:\IA_BuildCache\_r53_src\AnomalyInjector -ResumePrompt <that file> -OwnerApprovedFullAccess`, with the defaults;
   - poll until a new `exit-code.txt` appears.
3. **On a capacity or interruption failure:**
   - the relay retries by itself;
   - if it gives up, resume once more with a short "continue" prompt;
   - if that fails too, report NEEDS-DECISION. Don't edit any relay script.
4. **After Codex exits: your review**, in `_reviews\093-11-code-review.md`. One row per item: Codex result → your check → OK / GAP.
   - **Pre-flight:** your own candidate rehash against the `eff797c` manifest, and the restoration state.
   - **The freeze:**
     - the amendment, the decision table, the readings and the retained failed leg;
     - committed before the first pairing leg (commit time against leg start);
     - reader, tolerance and predicate unchanged (hashes).
   - **C1:**
     - Run the frozen pairing reader yourself, read-only, from a scratch copy, on the C1 session. Quote C/P/N, decidable and floor.
     - Confirm the shifted-mask control rejected.
   - **C2:** the reading, and the exact motion-blur command used.
   - **Probe coverage:** recompute it yourself on the failed leg and on C1.
   - **The decision:** confirm the one taken matches the table.
   - **Every other pairing leg:** recount at least one Lyra leg and one staged leg.
   - **The rest of the PIE matrix:** recount the exact J1 delayed control and one admission probe.
   - **The camera switch:** the gate, plus the readings (drops per switch, event continuation and reason).
   - **The cook and the staged gates:** recount at least one staged leg per fixture, including a staged resize leg under W2.
   - **Can-fails:** confirm they really failed.
   - **Harness fixes:** each harness-only and within budget. The pairing recipe allows at most 2; the marker has 0 left.
   - **Docs:**
     - the B19 correction;
     - the counter-scope doc and the gotcha;
     - the `office_checkpoint_i.md` sentence, with every `tools\*.py` byte-unchanged.
   - **Restoration:** against **your own** baselines.
   - **Push:** only if every gate is green, `fix/followup-capture` only, fast-forward. Confirm with `ls-remote`.
   - **If J2 qualifies:** run the office reader yourself on one new healthy J2 session and quote the line.
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
  - Leave alone the owner's Codex desktop processes, UE 5.7, Blender, the `UnrealTraceServer` daemon, and his other apps.
  - The owner may be using the PC today.
- Delete nothing, except your own scratch, by receipt.
- Don't touch `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\` or `E:\<personal-folder>`.
- **Codenames only.** Office data never comes home.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner** (3 lines, no jargon).
3. **Your environment check** (the `GIT_*_DATE` variables) and the attempt numbering.
4. **The Codex job:** duration, its VERDICT line, heads pushed, hashes, harness fixes.
5. **The pairing decision:**
   - C1 and C2 results;
   - probe coverage and `speed_ratio`;
   - which recipe was adopted.
6. **Your review:** counts (OK / GAP / OPEN), then each GAP in one line.
7. **The reader lines:** the healthy PASS from a new J2 session (if J2 qualified) and any others you ran.
8. **Ready for office checkpoint I?** Yes or no. If yes, the pushed head, and what the owner needs:
   - the build to pull;
   - cases 11–14 on the second office host;
   - Python 3.10+ with numpy and Pillow;
   - the contact-sheet glance (`g=`);
   - no window resize during a session;
   - one box read on an older delivered-build session per host.
9. NEEDS-DECISION items, or "none".
