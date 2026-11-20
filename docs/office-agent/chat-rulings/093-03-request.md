093-03 — IMPLEMENT J1: the integration base for the natural-cause work. Requested by chat Claude (the orchestrator) on 2026-10-13. Launched through the implementer Relay by Claude Code (mailbox), who reviews your work; chat rules.

BRANCH POLICY (owner, 2026-10-13; ruling 093-02 P10)
- This job's branch is fix/followup-capture. It carries the follow-up fixes only (the release plus the merge of 5d6352b; J2 adds the office-robust F1). It contains no natural-cause code.
- It is not a delivery. Nothing is merged into master, a release branch or any delivery branch. The owner decides when these fixes reach a regular build.
- The natural work comes later, on its own branch research/natural-causes, created from this branch's head after J2.

READ FIRST (binding)
1. _reviews\093-02-ruling.md (P1 base by MERGE; P2 can-fail sourcing; P7; P8) and _reviews\093-01-ruling.md.
2. Your plan, _reviews\093-02-codex-natural-foundation-plan.md: §A2 (re-proof list), §E (gates) and the J1 row of §F1, as amended by ruling P1 (merge, do not cherry-pick).
3. Claude Code's plan review, _reviews\093-02-code-plan-review.md §2–§3: the merge evidence, and the archives deleted on 2026-10-10.
4. The release's docs/PRE-DELIVERY-CHECKLIST.md §0, and the repository's own CLAUDE.md rules.

THE JOB (no natural mechanism, no F1 work-in-progress and no new label keys in this job)
1. Worktree and branch.
   - Create the new worktree from the existing source worktree, never from the main checkout:
     git -C E:\IA_BuildCache\_r53_src\AnomalyInjector worktree add -b fix/followup-capture E:\IA_BuildCache\_r933\AnomalyInjector d7eee97
   - If _r933 exists, pick the next unused name and say which.
   - Never touch: the main checkout (m51), release/2026-10-05, master, or tags.
2. Merge. Run git merge 5d6352b (feat/m52-proxy-blur).
   - Resolve the one expected conflict (CLAUDE.md) by keeping the release's status and the follow-up notes, clearly dated.
   - Any other conflict: stop and report NEEDS-DECISION with the hunks.
3. Check the merged result.
   - The temporal transition defaults read 0/0/0 (AnomalyLabelSync.h).
   - The existing explicit-cvar override test still passes.
   - git diff 5d6352b HEAD -- Source Shaders tools equals the release's d660b5d..d7eee97 delta for those paths.
4. Write the predictions file before the first leg, covering every gate in step 6.
5. Build and static checks.
   - Editor and game builds on the build host E:\IA_BuildCache\_r53_host (a detached checkout of your head, restored by receipt afterwards to the commit it had before).
   - Strict include build, unit and pure tests, string scan, lever audit.
6. Re-prove at 0/0/0, in PIE AND staged, each gate with its can-fail (plan §A2):
   - the F1 owner guard in its home modes, as 091-05 proved them;
   - F2: C1/C2 changed-component mask scope and tag retirement;
   - F3: the opaque-alpha no-effect / masked-alpha positive pair;
   - F6: the wrong-event-id check;
   - dropped-mask accounting;
   - a short m52/m53 controlled regression: blurry hold and proxy, UV, normal;
   - the label-pixel gate, 0 frames off.
   Where 091-05 used C7FB5212 as the pre-F1 control, use the shipped release exe 218BA4C7 (_binary_baselines\release-2026-10-05-23098fa-218BA4C7). The 091-05 archives were deleted on 2026-10-10.
7. Archive this build (exe hash) under _binary_baselines. It is also the F1 can-fail for J2: guard present, work-in-progress absent.
8. Finish.
   - Commit with Conventional Commits; push fix/followup-capture only, fast-forward.
   - Write the journal and the CLAUDE.md status block.
   - Restore the build host and any staging by receipt.

STOP CONDITIONS
- An unexpected merge conflict.
- A gate fails.
- The idle gate does not pass within 4 hours for the legs (the owner uses this PC for UE 5.7 and Blender; never touch those processes).
In each case, report NEEDS-DECISION with exactly what is missing. Never weaken or re-tune a gate.

FINAL REPORT (report.md and your final message), ending with its own line VERDICT: GREEN | NEEDS-DECISION | RED:
1. A plain summary in 3 lines.
2. Heads pushed; the merge result (conflicts and how you resolved them); the defaults check; the diff equality check.
3. Builds and hashes; the archive path.
4. A gate table: gate → prediction → result → can-fail result, for PIE and staged.
5. Anything that differs from the 091-05 evidence, and why.
6. NEEDS-DECISION items, or "none".
