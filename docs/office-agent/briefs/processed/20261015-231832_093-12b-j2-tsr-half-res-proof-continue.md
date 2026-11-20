# ACK-VERDICT: 093-12 and 093-12R (RED, derived) never started: the percent sign in their NAME header broke the generated .cmd line (cmd stripped everything from the percent sign to 'D:', including the stdin redirect), so Claude got no prompt. This is the same 093-12 brief, re-dropped with a NAME free of special characters. 093-11 (NEEDS-DECISION) is ruled in _reviews\093-11-ruling.md: the pairing cause is proven (lighting), no PREVIOUS appeared anywhere, and Z2 re-states the half-resolution proof for TSR.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-12 J2: TSR half-res proof, rest of the matrix, camera switch, cook, staged, push (Codex resumes, Code reviews after)

# 093-12 — Resume Codex's J2 session: re-state the 50% proof for TSR, then finish J2 to the push

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job takes many hours. Launch it **detached** and poll with bounded waits of **10 minutes or less**.
- **Never end the run while it is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean; roll at 70 %.

## Why
- 093-11 proved the pairing cause:
  - C1 (lighting restored) read 52/0/0 with its control rejected;
  - C2 (motion blur off) still read NEITHER.
  - Then the historic recipe was adopted after Lyra's amended leg read 49/0/9.
- It stopped on the historic StackOBot native 50% leg. Pairing passed (56/0/0), but the frozen size check expected 640×496 and the picture is 1280×992.
- Chat's ruling, `_reviews\093-11-ruling.md`:
  - **Z1:** your observation A is upheld. TSR upscales the 640×496 internal render to the output before the grab. It is not a product finding.
  - **Z2:** the 50% proof is re-stated in four parts: the internal-rect evidence from the log, one size throughout, PF-2 box agreement per axis, and the unchanged pairing predicate. A fresh leg is the gate.
  - **Z3:** continue the K7 order at the historic recipe, with the stop rules, including the Lyra spill readings before any NEITHER stop.
  - **Z4–Z5:** your observations, and owner items.
- **Branch policy** (093-02 P10, amended by K3): no natural-cause code. Nothing merges into `master`, a release branch or any delivery branch.

## Rules carried over
- **W3:** while Codex runs, write notes only in `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\code-receipts-093-12\`.
  - Publish `_reviews\093-12-code-review.md` only after Codex exits.
  - If something must change Codex's course, stop the job by PID (only the relay-started process) and report.
- **W4:** launch the relay with `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` removed from the child environment. Report whether your own environment has them.

## Bootstrap
1. Read in full:
   - `_reviews\093-11-ruling.md` and `_reviews\093-12-request.md`;
   - `_reviews\093-11-code-review.md`;
   - Codex's last report, `report.md` in the J2 run folder (it covers 093-11).
2. Read `_relay\README.md` and `relay_resume_codex_impl_v1.ps1`.
3. **Check the run folder:**
   - Confirm no Codex process for thread `<uuid>` is alive.
   - Confirm the run folder holds the 093-11 `exit-code.txt`.
   - The resume script will rename that file to `exit-code-attempt5.txt`, though 093-11 was the sixth attempt. Add the mapping to `code-receipts-093-12\attempt-numbering.txt`. Change no script.
4. Before launch, take your own baselines: staging inventory, both `Content` trees, refs and `ls-remote`, worktree heads, processes, host binaries, host config.

## The work
1. **Resume prompt:** `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\resume-prompt-093-12.txt` (UTF-8, through `cmd /c` redirection), made of:
   - `_reviews\093-12-request.md` verbatim;
   - then clearly marked Code-added context lines (paths only);
   - then `gdp-impl-block-v1.txt`.
2. **Resume detached** with the two `GIT_*_DATE` variables removed:
   - run `relay_resume_codex_impl_v1.ps1 -Run D:\IntrusiveAnomalies\_relay\runs\2026-10-14-093-06-j2-office-robust-f1 -Workdir E:\IA_BuildCache\_r53_src\AnomalyInjector -ResumePrompt <that file> -OwnerApprovedFullAccess`, with the defaults;
   - poll until a new `exit-code.txt` appears.
3. **On a capacity or interruption failure:**
   - the relay retries by itself;
   - if it gives up, resume once more with a short "continue" prompt;
   - if that fails too, report NEEDS-DECISION. Don't edit any relay script.
4. **After Codex exits: your review**, in `_reviews\093-12-code-review.md`. One row per item: Codex result → your check → OK / GAP.
   - **Pre-flight:** your own candidate rehash against the `eff797c` manifest, and the restoration state.
   - **The Z2 freeze:** its content against Z2 and Z3, committed before the first leg (commit time against leg start). Reader, tolerance and predicate unchanged (hashes).
   - **The fresh StackOBot native 50% gate.** Check all four Z2 parts yourself, read-only, from scratch copies:
     - the internal-rect predicate from the leg's log, and its can-fail on the historic 100% leg's log;
     - picture and mask sizes on every frame;
     - the PF-2 estimator per axis (your `pf2_mine.py`): n and scale, plus the 0.96 mutant on a gating axis;
     - the frozen pairing reader: C/P/N, and the shifted copy.
   - **The stopped 093-11 leg under Z2:** confirm it is reported as a reading only.
   - **Every other pairing leg:** recount at least one Lyra leg and one staged 50% leg (all four Z2 parts on the staged one).
   - **The rest of the PIE matrix:** recount the exact J1 delayed control and one admission probe.
   - **The camera switch:** the gate, plus the readings (drops per switch, event continuation and reason).
   - **The cook:** the staged EXE and DLLs rehash equal to the candidate manifest.
   - **The staged gates:** recount at least one staged leg per fixture, a staged PF-2 case and a staged resize leg under W2.
   - **Can-fails:** confirm they really failed.
   - **Harness fixes:** each harness-only and within budget. The pairing recipe, packaging metadata and marker budgets are all spent.
   - **Docs:** the Z2 freeze, the G485 extension, the journal and the status block.
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
  - Leave alone the owner's Codex desktop processes, UE 5.7, Blender, any `UnrealTraceServer` daemon, and his other apps.
  - The owner may be using the PC.
- Delete nothing, except your own scratch, by receipt.
- Don't touch `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\` or `E:\<personal-folder>`.
- **Codenames only.** Office data never comes home.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner** (3 lines, no jargon).
3. **Your environment check** (the `GIT_*_DATE` variables) and the attempt numbering.
4. **The Codex job:** duration, its VERDICT line, heads pushed, hashes, harness fixes.
5. **The 50% proof:** the fresh gate's four Z2 parts (with the PF-2 n and scale per axis), and the stopped leg's reading.
6. **Your review:** counts (OK / GAP / OPEN), then each GAP in one line.
7. **The reader lines:** the healthy PASS from a new J2 session (if J2 qualified), and any others you ran.
8. **Ready for office checkpoint I?** Yes or no. If yes, give the pushed head and what the owner needs:
   - the build to pull;
   - cases 11–14 on the second office host;
   - Python 3.10+ with numpy and Pillow;
   - the contact-sheet glance (`g=`);
   - no window resize during a session;
   - one box read on an older delivered-build session per host.
9. NEEDS-DECISION items, or "none".
