093-07 — CONTINUE J2: fix the projection defect, fix the office reader, then requalify J2 on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-14. Launched through the implementer Relay by Claude Code (mailbox), who reviews your work; chat rules.

BINDING
- _reviews\093-06-ruling.md (K1–K8). This request implements K3–K7.
- _reviews\093-06-request.md and your own 093-06 report (the J2 matrix, harness and receipts under E:\IA_BuildCache\_r936).
- _reviews\093-06-code-review.md (GAP 1, GAP 2; observations A–D).
- _reviews\078-16-draft-first-read.md §C.3 (the reconstruction's gaps and the "geometry record" idea).
- _reviews\093-02-ruling.md P2, P6 and P10 (P10 as amended by K3).

BRANCH
- fix/followup-capture, local head bb9e741 (product frozen at f608d56), worktree E:\IA_BuildCache\_r933\AnomalyInjector. The remote stays at 1bf7054 until every gate is green.
- P10 as amended: this branch carries J1, J2 and this projection fix. No natural-cause code.
- Nothing merges into master, a release branch or any delivery branch.

PART A — THE PROJECTION FIX (K3)
1. Design note FIRST. Commit it with the predictions before any product change. It states:
   - the route: ULocalPlayer::GetProjectionData for the resolved capture owner's viewport, on the game thread after that frame's camera update; or the owner view's own (unjittered) matrices and rects taken from the existing scene view extension and frozen with the request;
   - every product use of the camera FOV or aspect, and for each: moved to the engine projection, or why it does not depend on it;
   - thread and timing;
   - the mapping from the engine's view rect to the captured picture's pixels, through normalised coordinates of the same rect the capture copies, so screen percentage and output resampling cannot shift a box.
2. Implement it.
   - No FOV/aspect reconstruction may remain in the targeting or label path.
   - Scope: the projector and its direct callers in the targeting and label path.
   - STOP (NEEDS-DECISION) for: a needed change outside that scope; an engine edit; a private header beyond what capture already uses; a new module dependency.
3. Labels.
   - No key changes meaning. fovDeg stays the camera's configured FOV.
   - Add an additive "projection" diagnostic block to run_summary.json only: effective axis constraint, camera aspect, viewport size, view rect, projection mode. Never in labels.

PART B — THE OFFICE READER (K5), tools\office_checkpoint_i.py and its doc
1. Remove accepted_ambiguous from the capture_ownership block and a= from the line.
2. Never crash on a labelled row whose mask_file is null. Count those rows as m= (labelled without a measured mask; PASS needs 0), and still compute everything else.
3. No hand-written input.
   - Replace the inspection file with a contact sheet the reader writes next to the session: 20 distributed frames, the frame number under each. It stays on the office PC.
   - The owner appends " g=<how many don't show the game view>" by hand. PASS needs g=0.
   - Drop the manual edge-pair inspection: edges are measured.
4. Line format.
   - PASS: I<h>.<case> PASS v=<visible events> c=<axis constraint: 0 X, 1 Y, 2 major> b=<box scale x1000> #<check>
   - PASS also requires w=r=360, e=0, x=0, m=0, masks paired, |b-1000|<=10.
   - FAIL: the full numbers r w v e x m c b, the check digit, then the I-failure counters.
5. Box-read mode for an existing session folder, including sessions captured with delivered builds: B<h> n=<rows used> b=<scale x1000> #<check>. Use the same estimator as PF-2.
6. Python: no 3.11-only calls (hashlib.file_digest); no packages beyond what the office kit already uses. State the minimum version in office_checkpoint_i.md, and prove it in the self-test.
7. Self-tests, quoting each line:
   - a PASS;
   - a FAIL (the 093-06 null-mask Lyra session);
   - a box-read PASS (a StackOBot f608d56 session);
   - a box-read FAIL (a Lyra f608d56 session).

PART C — GATES. Write the predictions first; each gate has its can-fail.
1. PF-1, the observed failure kept as a negative test.
   - The setup: the exact 093-06 stop arrangement (Lyra, selected viewport 942x869, MaintainYFOV, target just off the left edge).
   - The fixed build must label the target visible on no row. No event fires on it, or the attempt is refused with a counted reason.
   - Can-fail: the retained f608d56 session (96 false rows). The checker must reject it.
2. PF-2, box agreement.
   - The check: the projected box against the mask-drawn box, per axis. A scale estimate of the two boxes' centres about the view centre must be 1.000 ± 0.010. Freeze the exact estimator, including which rows count, before any leg.
   - PIE matrix: both fixtures x {MaintainXFOV, MaintainYFOV} x {942x869, 1123x870, 1280x720}.
   - Staged: both fixtures x both constraints at 1280x720.
   - Set the constraint by a process-local override only. Never edit a host config file.
   - Can-fail: the archived f608d56 Lyra sessions must read about 0.967 and 0.813 and be rejected; the StackOBot ones must pass.
3. PF-3, no change on MaintainXFOV hosts. StackOBot regression legs, fixed build against f608d56:
   - same events, targets and edges;
   - identical visible_positive;
   - bbox_px within 0.5 px.
   Freeze the comparison method in the predictions.
4. PF-4, letterbox. One StackOBot PIE leg with a constrained-aspect camera (black bars): correct boxes, or a counted refusal. Never a wrong box.
5. Camera switch (K6). One leg per fixture, PIE and staged, with scripted view-target switches during active events, both a cut and a blend.
   - Gate: no label/picture disagreement (edges 0, every labelled frame with a measured mask, PF-2 agreement on the moving frames).
   - Readings, not gates: frames dropped per switch; whether each active event continued or ended, and its recorded reason.
6. The full 093-06 matrix on the final candidate: both fixtures, PIE and staged, every case with its can-fail. The selected-viewport positive is reframed so the target is truly on screen, judged by the engine's projection. The original framing is PF-1.
7. The office reader self-tests (Part B, item 7).

ORDER (K7)
1. PIE: PF-1 to PF-4, then the J2 matrix on both fixtures, then the camera switch.
2. The cook.
3. Every staged gate.
4. Push fix/followup-capture (fast-forward) only when every gate is green.

FINISH
- Build, strict build, tests, string scan, lever audit, archive (the new candidate).
- Restore the hosts and staging by receipt.
- Docs:
  - a gotcha (next free number, re-checked across all refs at commit);
  - the architecture note;
  - the journal and the CLAUDE.md status block;
  - replace the two literal "?" characters Code found (observation C).

STOP RULE (ruling 093-04 D2, unchanged)
- Your own harness problems: fix them in-job and disclose them, at most 2 attempts each. Never change a gate, threshold or prediction to get past one.
- Stop only for:
  - a product gate failing its prediction;
  - a needed change outside this request's scope (Part A item 2);
  - the idle gate not passing within 4 h;
  - a security block;
  - a disk floor.

FINAL REPORT (report.md and your final message)
1. A plain summary in 3 lines.
2. Part A: the route chosen, the design note's FOV/aspect list, the files changed.
3. Part B: the line formats, and the four self-test lines.
4. The gate table: PF-1 to PF-4, the camera-switch readings and the J2 matrix, PIE and staged, both fixtures, with can-fails.
5. An example "projection" block.
6. Heads pushed, hashes, archive.
7. The harness fixes.
8. NEEDS-DECISION items, or "none".
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
