093-08 — CONTINUE J2 (second continuation): add on-screen admission for targeted fires, then requalify J2 in the K7 order on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-14. Launched through the implementer Relay by Claude Code (mailbox), who reviews your work; chat rules.

BINDING
- _reviews\093-07-ruling.md (L1–L7). This request implements L2 to L6.
- _reviews\093-06-ruling.md (K1–K8) and _reviews\093-07-request.md (everything there still applies unless L1–L7 change it).
- Your 093-07 report (report.md in this run folder) and the evidence under E:\IA_BuildCache\_r937.
- 093-02 P10 as amended by K3.

BRANCH
- fix/followup-capture, local head 9514242 (product f9b9328), worktree E:\IA_BuildCache\_r933\AnomalyInjector. The remote stays at 1bf7054 until every gate is green.
- No natural-cause code. Nothing merges into master, a release branch or any delivery branch.

PART A — ON-SCREEN ADMISSION FOR TARGETED FIRES (L2)
1. Design note FIRST. Commit it with the predictions before any product change. It lists every path that applies an anomaly to a named object target without an on-screen check: start with BeginFire -> TryFireSpecific -> ApplyAnomaly, and include any dashboard or console single-target fire.
2. The check runs before a fire is applied to a named object target. Test the target's bounds against the engine projection of the resolved capture owner (the 093-07 projector).
   - Frustum only. Occlusion stays with the existing mask veto.
   - Off screen: refuse, never apply, count targeted_refused_off_screen.
   - No owner projection yet: refuse, count targeted_refused_no_projection.
   - Both counters go in run_summary.json only (additive diagnostics). A refused fire is retried on the existing cadence.
   - Frame-wide anomalies keep their own eligibility.
3. Scope: the fire-admission path and the two counters. STOP (NEEDS-DECISION) for:
   - a needed change outside that scope;
   - an engine edit;
   - a new module dependency;
   - a private header beyond what capture already uses.

PART B — HARNESS ITEMS (L4; harness rules apply)
1. Rebuild and verify the Lyra Editor helper (its Switches command was missing) before any camera-switch leg.
2. The camera-switch reader counts pending pre-switch samples as drops.
3. Run the prepared archived-binary replay for the mask-based 0.813 can-fail.
4. Produce the office reader's healthy I PASS line from a new engine session.

PART C — PREDICTIONS AND GATES (L5)
1. Before any leg, re-freeze the predictions the admission changes, for example event counts where fires used to land before the owner view resolved. Give the reason for each.
   - The floors never move: v>=3; edges 0; every labelled frame with a measured mask; w=r.
2. PF-1 re-run (the exact 093-06/093-07 arrangement):
   - 0 rows visible;
   - 0 applied fires on the off-screen target;
   - at least one counted targeted_refused_off_screen.
   - Can-fail: your 093-07 PF-1 session (4 applied fires) must be rejected by the checker.
3. Then everything else in 093-07 PART C, unchanged: PF-2, PF-3, PF-4, the camera switch, the full 093-06 matrix and the reader self-tests.

TIMING (L3): labels, masks and visibility flags come from the frozen serving record, as built. Selection and admission may use the last completed camera. The mask veto is the backstop, and the camera-switch leg measures it.

ORDER (K7, unchanged)
1. PIE: PF-1 re-run, then PF-2 to PF-4, then the J2 matrix on both fixtures, then the camera switch.
2. The cook.
3. Every staged gate.
4. Push fix/followup-capture (fast-forward) only when every gate is green.

FINISH
- As in 093-07: builds, strict build, tests, string scan, lever audit, archive (the new candidate), restore hosts and staging by receipt.
- Docs: a gotcha for the admission (next free number, re-checked across all refs at commit), the architecture note, the journal and the status block.

STOP RULE (ruling 093-04 D2, unchanged)
- Your own harness problems: fix them in-job and disclose them, at most 2 attempts each. Never change a gate, threshold or prediction to get past one. Re-freezing under PART C item 1 happens before any leg, with reasons.
- Stop only for:
  - a product gate failing its prediction;
  - a needed change outside this request's scope;
  - the idle gate not passing within 4 h;
  - a security block;
  - a disk floor.

FINAL REPORT (report.md and your final message)
As in 093-07, plus:
- the admission design: the paths found, and where the check runs;
- the two counters' values per leg;
- the PF-1 re-run line and its can-fail;
- the re-frozen predictions, each with its reason.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
