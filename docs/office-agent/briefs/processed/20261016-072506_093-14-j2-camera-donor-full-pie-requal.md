# ACK-VERDICT: 093-13 (NEEDS-DECISION) is ruled in _reviews\093-13-ruling.md. The crash fix 9781031 works. The camera stop was a control that could never fire (hard-coded donor 299), so AB1 makes it drop-aware. AB2 withdraws the identity shortcut and re-runs the full PIE matrix on the new binary.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-14 J2: camera donor fix, full PIE rerun on 9781031, Lyra camera, TSR half-res boxes, cook, staged, push (Codex resumes, Code reviews after)

# 093-14 — Resume Codex's J2 session: drop-aware camera control, full PIE re-run on the new binary, then cook, staged and push

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job takes many hours. Launch it **detached** and poll with bounded waits of **10 minutes or less**.
- **Never end the run while it is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean; roll at 70 %.

## Why
- 093-13 fixed the camera-cut crash in `9781031`, which you verified independently, including the CPU self-test.
- Both StackOBot PIE camera legs then passed every measured check. The gate stopped only on the stale-projection control's hard-coded donor 299 (your observation A).
- Chat's ruling, `_reviews\093-13-ruling.md`:
  - **AB1:** the donor becomes the last written sample before each switch, found from receipts.
    - If none exists within 10 samples, that is a STOP.
    - This is attempt 1 of 2.
    - The retained StackOBot active leg is re-assessed with it; no engine re-run.
  - **AB2:** your observation B is upheld; the identity shortcut is withdrawn. Every PIE gate re-runs on the new binary (PF-1 to PF-5 and every PIE case in the ledger), except the archived-DLL controls (exact J1 delayed, pre-F1 wrong-view), whose results stand.
  - **AB3:**
    - C: `ExposureExclusionPx` is a log reading.
    - D: change evidence under camera motion is an open item for after J2.
    - E: the Lyra fixture DLL hash goes in the manifest.
  - **AB4–AB5:** the order and the stop rules.
- **Branch policy** (093-02 P10, amended by K3): no natural-cause code. Nothing merges into `master`, a release branch or any delivery branch.

## Rules carried over
- **W3:** while Codex runs, write notes only in `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\code-receipts-093-14\`.
  - Publish `_reviews\093-14-code-review.md` only after Codex exits.
  - If something must change Codex's course, stop the job by PID (only the relay-started process) and report.
- **W4:** launch the relay with `GIT_AUTHOR_DATE` and `GIT_COMMITTER_DATE` removed from the child environment. Report whether your own environment has them.

## Bootstrap
1. Read in full:
   - `_reviews\093-13-ruling.md` and `_reviews\093-14-request.md`;
   - `_reviews\093-13-code-review.md`;
   - Codex's last report, `report.md` in the J2 run folder (it covers 093-13).
2. Read `_relay\README.md` and `relay_resume_codex_impl_v1.ps1`.
3. **Check the run folder:**
   - Confirm no Codex process for thread `<uuid>` is alive.
   - Confirm the run folder holds the 093-13 `exit-code.txt`.
   - Add the attempt-to-file mapping to `code-receipts-093-14\attempt-numbering.txt`. 093-13 was the 8th physical attempt, and the resume script will rename its file to `exit-code-attempt7.txt`. Change no script.
4. Before launch, take your own baselines: staging inventory, both `Content` trees, refs and `ls-remote`, worktree heads, processes, host binaries, host config.

## The work
1. **Resume prompt:** `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\resume-prompt-093-14.txt` (UTF-8, through `cmd /c` redirection), made of:
   - `_reviews\093-14-request.md` verbatim;
   - then clearly marked Code-added context lines (paths only);
   - then `gdp-impl-block-v1.txt`.
2. **Resume detached** with the two `GIT_*_DATE` variables removed:
   - run `relay_resume_codex_impl_v1.ps1 -Run D:\IntrusiveAnomalies\_relay\runs\2026-10-14-093-06-j2-office-robust-f1 -Workdir E:\IA_BuildCache\_r53_src\AnomalyInjector -ResumePrompt <that file> -OwnerApprovedFullAccess`, with the defaults;
   - poll until a new `exit-code.txt` appears.
3. **On a capacity or interruption failure:**
   - the relay retries by itself;
   - if it gives up, resume once more with a short "continue" prompt;
   - if that fails too, report NEEDS-DECISION. Don't edit any relay script.
4. **After Codex exits: your review**, in `_reviews\093-14-code-review.md`. One row per item: Codex result → your check → OK / GAP.
   - **Pre-flight:** your own rehash of the `9781031` banks, including the Lyra fixture DLL, and the restoration state.
   - **The AB1 freeze:**
     - committed before the re-assessment and before the first leg;
     - the donor rule found from receipts, not hard-coded;
     - the 10-sample no-donor STOP.
     - Confirm the reader change is limited to the donor selection.
   - **The re-assessed StackOBot active leg:** rebuild the stale-projection control yourself with the drop-aware donor (`cam_mine.py`). It must reject.
   - **Lyra camera, PIE:**
     - the gate: edges (your independent pixel check), masks, PF-2 on the moving frames, the drop-aware control;
     - the readings: drops per switch, event continuation and reason, extent_mismatch.
   - **The full PIE matrix on the new binary:**
     - recount at least PF-1, one PF-2 case, PF-3, PF-5, one refusal case, resize (W2 plus the AB3-C log reading) and one admission probe;
     - run the pairing reader yourself on at least one Lyra 50 % leg (Z2 parts 1, 2 and 4);
     - check that every case in the ledger has a result on the new binary, except the two archived-DLL controls.
   - **TSR half-resolution PF-2, both fixtures, PIE and staged:** n and scale per axis (`pf2_mine.py`), and the 0.96 mutants.
   - **The cook:** the staged EXE and DLLs rehash equal to the new manifest.
   - **The staged gates:** recount at least one leg per fixture, the staged camera switch with the drop-aware control, a staged 50 % pairing leg, a staged PF-2 case and a staged resize leg under W2.
   - **Can-fails:** confirm they really failed.
   - **Harness fixes:** each harness-only and within budget. The camera donor allows at most 2; pairing recipe, packaging metadata and marker are spent.
   - **Docs:** the donor gotcha, the AB3-D open item, the journal, the status block.
   - **Restoration:** against **your own** baselines.
   - **Push:** only if every gate is green, `fix/followup-capture` only, fast-forward. Confirm with `ls-remote`.
   - **If J2 qualifies:** run the office reader yourself on one new healthy J2 session from the new binary, and quote the line.
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
5. **The camera switch:** the re-assessed StackOBot control, then Lyra's gate and readings (PIE and staged).
6. **The full PIE matrix and the TSR half-resolution PF-2:** the per-case table (counts), and n and scale per axis.
7. **Your review:** counts (OK / GAP / OPEN), then each GAP in one line.
8. **The reader lines:** the healthy PASS from a new J2 session on the new binary (if J2 qualified), and any others you ran.
9. **Ready for office checkpoint I?** Yes or no. If yes, give the pushed head and what the owner needs:
   - the build to pull;
   - cases 11–14 on the second office host;
   - Python 3.10+ with numpy and Pillow;
   - the contact-sheet glance (`g=`);
   - no window resize during a session;
   - one box read on an older delivered-build session per host.
10. NEEDS-DECISION items, or "none".
