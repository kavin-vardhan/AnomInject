093-06 — IMPLEMENT J2: the office-robust F1 (capture-view ownership) on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-14. Launched through the implementer Relay by Claude Code (mailbox), who reviews your work; chat rules.

BINDING
- _reviews\093-05-ruling.md (E1–E5).
- _reviews\093-02-ruling.md (P2, P6, P10).
- The plan _reviews\093-02-codex-natural-foundation-plan.md: §A3 (F1 boundaries 1–5) and §F2 (checkpoint I).
- Claude Code's plan review _reviews\093-02-code-plan-review.md §2 (the WIP's NotGameView rejection; the deleted harness).
- The J1 evidence and journals under E:\IA_BuildCache\_r933.

BRANCH POLICY
- Work on fix/followup-capture at 1bf7054 (pushed, J1 qualified). Use the worktree E:\IA_BuildCache\_r933\AnomalyInjector, or a fresh one from it; say which.
- No natural-cause code.
- Nothing merges into master, a release branch or any delivery branch.

PART 0 — J1 closure (before any J2 change)
1. The UV/normal "wrong object" can-fail from the J1 predictions (line 33).
   - Build it on COPIES of the existing J1 UV captures, PIE and staged: relabel an event onto a different object's target and mask, so the visible change lies outside the claimed mask.
   - The visibility checks must reject it. If they don't, stop and report: that is a checker defect.
2. Correct the status-block sentence that says every can-fail ran.
3. Record the masked-alpha caveat as a known limitation, in a gotcha and in the readme's limitations:
   - The mask follows the event-state silhouette, so pixels the corrupted alpha cuts away change outside the mask. Edges stay 0 frames off.
   - Future item, regular line: mask the union of the pre-event and event silhouettes.
4. One line: why both staged proxy runs produced 3 events instead of 4 (the refusal reason, from the log).
Commit these docs (Conventional Commits).

PART 1 — J2
1. Apply the saved work: _reviews\2026-10-10-disk-cleanup\r916-091-06-uncommitted-wip.patch plus the files in r916-untracked\. Its base is 1eaf205; Code showed it applies cleanly on 5d6352b and on 5d6352b plus the release delta.
2. Complete it within the plan's §A3 boundaries:
   a. Activation decides participation only. Resolve the display viewport from the capture world's local players on the game thread, at run start and before each issue. Reject an ambiguous selection. Refresh on change.
   b. Freeze (run_epoch, owner_generation, scene, target, local player/view) into every colour, mask, census and probe request. Bind a unique family_id/view_index before dequeue, and require it again on the render thread. A viewport change invalidates pending requests and baselines.
   c. A resolved owner target is required. The target-unknown / game-flag fallback is allowed only after a bounded discovery step proves exactly one eligible game view and binds its real target. Never first-wins.
   d. Single-view scope. Split-screen, stereo and multiple same-world game displays are refused and counted.
   e. Keep the F1 family ids, same-sample expiry and dropped-result accounting.
3. NotGameView (Code's finding): it is a new way to drop every frame, even with a known owner.
   - Count it by sub-reason: Game show flag / game-thread view / controller id.
   - Prove the healthy path on both home fixtures.
4. Diagnostics: an additive capture_ownership block in run_summary.json only, never in labels. It holds:
   - owner known at start (scene, target);
   - owner updates;
   - rejections by reason, including the NotGameView sub-reasons;
   - accepted ambiguous (must be 0);
   - unpaired and dropped;
   - stale generation.
   Correct the WIP's warning to point at this block.
5. The office reader (ruling P6): tools\office_checkpoint_i.py.
   - It reads one office session and prints ONE short line per case: the case code, PASS/FAIL, and the handful of numbers chat needs (requested, written, visible events, max edge error, wrong-view count, accepted ambiguous), plus a check digit.
   - On FAIL, it also prints the I-failure counters (plan §F2).
   - Host codes: 1 = first office host, 2 = second office host.
   - Self-test it on home sessions, with one PASS case and one FAIL case.
6. Gates, PIE AND staged, on StackOBot plus the Lyra worktree. Write the predictions first; each gate has its can-fail:
   - Delayed viewport (rebuild the harness): J2 captures every requested frame from the right view. J1's E41ABF93 drops them all (the can-fail).
   - New-window PIE with a second realtime editor view showing a different picture: wrong-view count 0, non-owner rejections > 0. Release 218BA4C7 captures the wrong view (the can-fail).
   - Selected-viewport and two-client PIE: correct.
   - Viewport resize or reopen mid-run: the owner generation changes, and no stale result is accepted.
   - Ambiguity: two same-world game views are refused and counted, never first-wins.
   - NotGameView: the healthy path passes; a forced negative controller id, or a non-game view, is refused and counted.
   - J1 regression on the new head: the F1 UV and proxy legs and the label-pixel gate stay 0 frames off, with measured masks.
   - The office reader self-test.
7. Finish:
   - build, strict build, tests, string scan, lever audit, archive;
   - restore the host and staging by receipt;
   - push fix/followup-capture (fast-forward) only when every gate is green.

STOP RULE (ruling 093-04 D2)
- Your own harness problems: fix them in-job and disclose them, at most 2 attempts each.
- Stop only for:
  - a product gate failing its prediction;
  - a needed change outside this job's scope;
  - the idle gate not passing within 4 h;
  - a security block;
  - a disk floor.

FINAL REPORT (report.md and your final message)
1. A plain summary in 3 lines.
2. Part 0 results.
3. J2 as built (files).
4. The gate table with can-fails: PIE and staged, both fixtures.
5. An example capture_ownership block.
6. The office reader's two self-test lines.
7. Heads pushed, hashes, archive.
8. The harness fixes.
9. NEEDS-DECISION items, or "none".
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
