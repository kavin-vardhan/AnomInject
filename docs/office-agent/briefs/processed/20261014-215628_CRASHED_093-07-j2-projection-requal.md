# ACK-VERDICT: 093-06 (NEEDS-DECISION) is ruled in _reviews\093-06-ruling.md. J2 is not yet qualified; the stop was correct. The pre-existing projection defect is fixed on fix/followup-capture (P10 amended by K3), the office reader is fixed, and J2 is requalified in full. That is this brief.
# SESSION: new-default
# MODEL: claude-opus-5-5
# EFFORT: xhigh
# NAME: 093-07 J2 continuation: projection fix, reader fixes, full requalification (Codex resumes, Code reviews)

# 093-07 — Resume Codex's J2 session to fix the projection defect and the office reader, requalify J2, then review it

You are Claude Code, running **headless** from the GDP mailbox in a **fresh session**. Nobody answers questions during the run. **Your final message is the only thing anyone reads**; it goes to chat Claude (the orchestrator).

## How your turns end — standing instruction from the owner, the owner
- A message with no tool call ends the whole run. Do not end a turn to report progress, to announce the next step, or to offer options. Put status notes in the same message as your next tool call and keep going.
- End the run only when every step is done, a stop condition fires, or something needs the owner. The final message follows the structure at the end and starts with a `VERDICT:` line.
- The Codex job takes many hours. Launch it **detached** and poll with bounded waits.
- **Never end the run while it is still running.** The cap is 14 h; past that, leave it running and report.
- Keep context lean; roll at 70 %.

## Why
- Chat ruled 093-06 (`_reviews\093-06-ruling.md`). J2 stopped on a real defect that predates it: the product rebuilds the camera projection from the FOV, which is wrong on MaintainYFOV hosts such as Lyra (K2).
- This brief resumes **the same Codex session** to:
  - fix the projection with the engine's own projection (K3);
  - fix the office reader (K5);
  - measure the camera-switch case (K6);
  - requalify J2 in full on the final binary (K7).
- It pushes `fix/followup-capture` only when everything is green.
- **Branch policy (093-02 P10, amended by K3):** this branch carries J1, J2 and the projection fix, and no natural-cause code. Nothing merges into `master`, a release branch or any delivery branch.

## Bootstrap
1. Read in full:
   - `_reviews\093-06-ruling.md` and `_reviews\093-07-request.md`;
   - `_reviews\093-06-code-review.md`;
   - Codex's report `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\report.md`;
   - `_reviews\078-16-draft-first-read.md` §C.3 only.
2. Read `_relay\README.md` and `_relay\relay_resume_codex_impl_v1.ps1`; for the fallback, `relay_run_codex_gdp_impl_v1.ps1` and `gdp-impl-block-v1.txt`.
3. Check the relay is free: no run folder may have `events.jsonl` without `exit-code.txt`. If one does, stop and report NEEDS-DECISION.
4. Before launch, take your own baselines: staging inventory, both `Content` trees, refs and `ls-remote`, worktree heads, processes, host binaries.

## The work
1. **Resume prompt.** `_relay\runs\2026-10-14-093-06-j2-office-robust-f1\resume-prompt-093-07.txt` (UTF-8, through `cmd /c` redirection) =
   - `_reviews\093-07-request.md` verbatim;
   - then clearly marked Code-added context lines (paths only);
   - then `gdp-impl-block-v1.txt`.
2. **Resume detached.** Run `relay_resume_codex_impl_v1.ps1 -Run D:\IntrusiveAnomalies\_relay\runs\2026-10-14-093-06-j2-office-robust-f1 -Workdir E:\IA_BuildCache\_r53_src\AnomalyInjector -ResumePrompt <that file> -OwnerApprovedFullAccess`, with the defaults.
   - Poll until a new `exit-code.txt` appears.
   - If the session cannot be resumed, launch a new implementer job instead:
     - run folder `_relay\runs\<date>-093-07-j2-projection-requal\`;
     - `-Sandbox danger-full-access -OwnerApprovedFullAccess -OrchestratorBrief 093-07-j2-projection-requal.md`;
     - prompt = the 093-06 and 093-07 requests in order, then a line pointing at the 093-06 report and `E:\IA_BuildCache\_r936`, then the impl block.
3. **On a capacity or interruption failure:** resume once with a short "continue" prompt. If that fails too, report NEEDS-DECISION.
4. **Your review** (reading and recounting only). Write `_reviews\093-07-code-review.md`, one row per item: Codex result → your check → OK / GAP.
   - **Part A, the diff read in full.** It changes the shipped targeting and label path. Check:
     - the design note was committed before the product change;
     - no FOV/aspect reconstruction is left in the targeting or label path (search the source yourself);
     - which engine route was used, and its timing;
     - the scope bound; no new module dependency or private header;
     - no label key changes meaning;
     - the `projection` block is in `run_summary.json` only.
   - **PF gates, recounted with your own read-only computation:**
     - PF-1;
     - at least two PF-2 legs (one Lyra MaintainYFOV, one StackOBot MaintainXFOV);
     - the PF-2 checker on the archived `f608d56` Lyra sessions (it must reject them);
     - PF-3 on one StackOBot leg.
   - **J2 recounts:**
     - the reframed selected-viewport positive on Lyra;
     - one staged leg;
     - the J1 UV regression.
   - **Camera switch:** the gate result and the readings.
   - **Run the office reader yourself** (read-only, on saved sessions) and quote each line:
     - one PASS session;
     - the 093-06 null-mask FAIL session;
     - a box read on one StackOBot and one Lyra `f608d56` session.
   - **The can-fails really failed.**
   - **Harness fixes:** each harness-only, at most two attempts, no gate, threshold or prediction changed.
   - **Restoration:** staging, hosts, content and refs against **your own** baselines.
   - **Push:** only if every gate is green, `fix/followup-capture` only, fast-forward. Confirm with `ls-remote`.
5. **Do not fix anything.** Chat rules.

## Hard rules
- **You yourself:** no editor or game launch, no build, cook or stage, and no git write. Running the read-only office reader on saved sessions is allowed.
- **Never change any security setting.** If `dbghelp.dll` blocks the cook, Codex reports it; nobody works around it.
- **Never kill anything that is not yours, and never by name.** You may stop only a Codex process the relay started, by PID. Leave the owner's Codex desktop processes, UE 5.7 and Blender alone.
- Delete nothing, except your own scratch, by receipt.
- Don't touch `m51`, `master`, `release/2026-10-05`, tags, `ToCodex\` or `E:\<personal-folder>`.
- **Codenames only.** Office data never comes home.

## Final message structure
1. `VERDICT: GREEN | NEEDS-DECISION | RED`.
2. **Plain summary for the owner** (3 lines, no jargon).
3. **The Codex job:** resumed or new, duration, its VERDICT line, heads pushed, hashes, harness fixes.
4. **Part A in 3 lines:** the engine route used, the files, the scope check.
5. **Your review:** the counts (OK / GAP), then each GAP in one line.
6. **The reader lines:** PASS, FAIL, and the two box reads.
7. **Ready for office checkpoint I?** Yes or no, and what the owner needs (build to pull, cases to run, the reader's Python requirement).
8. NEEDS-DECISION items, or "none".
