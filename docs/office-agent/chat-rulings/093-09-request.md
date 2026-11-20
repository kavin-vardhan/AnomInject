093-09 — CONTINUE J2 (third continuation): fix the commit dates and fovDeg, settle the drop accounting, then run the gates in the K7 order on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-15. Launched through the implementer Relay by Claude Code (mailbox), who reviews your work after you finish; chat rules.

BINDING
- _reviews\093-08-ruling.md (W1–W6). This request implements them.
- _reviews\093-07-ruling.md (L1–L7) and _reviews\093-06-ruling.md (K1–K8), plus the 093-06, 093-07 and 093-08 requests, unless W1–W6 change them.
- Your reports (report.md, report-093-07.md) and the evidence under E:\IA_BuildCache\_r936, _r937 and _r938.
- Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BRANCH
- fix/followup-capture, local head 1071ca1 (product 41c1a0c), worktree E:\IA_BuildCache\_r933\AnomalyInjector. The remote stays at 1bf7054 until every gate is green.
- No natural-cause code. Nothing merges into master, a release branch or any delivery branch.

STEP 1 — COMMIT DATES (W4)
- Check that GIT_AUTHOR_DATE and GIT_COMMITTER_DATE are NOT set in your environment. If they are, stop and report.
- Re-create 7f4329c, 41c1a0c and 1071ca1 with correct current dates: the same trees, messages and author identity.
- Record old -> new hashes in the journal. Nothing is pushed or deleted.

STEP 2 — fovDeg (W1)
- Source the fovDeg label metadata from PlayerCameraManager->GetFOVAngle() again, in AnomalyViewport.cpp. Boxes and visibility stay on the engine projection.
- Commit (Conventional Commits).

STEP 3 — PREDICTIONS (W1, W2). Re-freeze only what W1 and W2 touch, before any leg, with reasons:
- PF-5, locked FOV: one StackOBot PIE leg with the FOV locked through the player controller (for example "fov 70").
  - Labels' fovDeg equals the locked value.
  - PF-2 agreement holds.
  - Admission counters behave normally.
  - Can-fail: show that the f9b9328 metadata route reports the unlocked value, by CPU or source-level check or by running it, whichever is cheaper. Never invent a number.
- Resize and camera-switch legs (W2):
  - r = w + counted drops, with every missing frame attributed to a named drop reason;
  - zero stale or mismatched frames accepted;
  - the other floors apply: edges 0 where events exist, measured masks, PF-2 agreement on written frames.
  - Revert your 1071ca1 w=r change for these legs, and restore the 093-06 resize accounting.
  - w=r stays the floor for every other leg.
- PF-1 checker (W5 E): also count applied fires through the generic apply receipt or log line, not only texcorrupt_fires_applied.

STEP 4 — BUILD, then the K7 order (W6)
1. PIE: PF-1 re-run, then PF-2, PF-3, PF-4, PF-5, then the full 093-06 J2 matrix on both fixtures, then the camera switch.
2. The cook.
3. Every staged gate.
4. Push fix/followup-capture (fast-forward) only when every gate is green.
Also in this job (L4, still open): the archived-binary 0.813 replay, and the healthy office-reader I PASS line from a new engine session.

DOCS
- The readme's dev notes (W5 C): object anomalies need Play (PIE or a game build), not Simulate.
- The journal: the date re-creation mapping and the fovDeg restore.
- The status block.

FINISH
As in 093-07 and 093-08: builds, strict build, tests, string scan, lever audit, archive (the new candidate), restore hosts and staging by receipt.

STOP RULE (ruling 093-04 D2, unchanged)
- Your own harness problems: fix them in-job and disclose them, at most 2 attempts each. Never change a gate, threshold or prediction to get past one; re-freezing happens only under STEP 3, before any leg.
- Stop only for:
  - a product gate failing its prediction;
  - a needed change outside this request's scope;
  - the idle gate not passing within 4 h;
  - a security block;
  - a disk floor.

FINAL REPORT (report.md and your final message)
As in 093-08, plus:
- the commit-date mapping;
- the fovDeg diff and the PF-5 result with its can-fail;
- the drop accounting for the resize and camera-switch legs (r, w, drops by reason, stale accepted = 0);
- the admission counters per leg;
- the 0.813 replay result;
- the healthy I PASS line.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
