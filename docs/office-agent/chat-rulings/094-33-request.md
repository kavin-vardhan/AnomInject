094-33 — MAKE 4.25 FRAMES UI-FREE (FO2, 5.1 parity) and re-run the C1 and C2 gates, then BUILD AND RUN C3 (masks) on 4.25 with the frozen per-anomaly metrics (FO3). One job, under chat's ruling 094-32 (FO1–FO5), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-01.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

THE OWNER'S DECISIONS
- Capture is the priority.
- Lean profile (FJ1).
- Label sync is the most important part. If labels or masks are not in sync with what is visible, the dataset is useless.

BINDING
1. _reviews\094-32-ruling.md (FO1–FO5). It is new.
   - FO2: move 4.25 colour capture to the end of the view family's render, before Slate, the canvas and on-screen debug text. Use PostRenderViewFamily_RenderThread, or whichever 4.25 hook you cite from source.
     - Gate: the heartbeat region is clean in every frame, against a control frame with the heartbeat visible. Then re-run the C1 gate (both sessions) and the C2 gate on the new path.
     - Fallback, only if 4.25 has no pre-UI point: suppress on-screen debug messages and the plugin heartbeat during capture, report that game UI would remain as NEEDS-DECISION, and continue to C3.
   - FO3: C3.
     - AnomalyShaders is compiled in on 4.25 per your plan's §C3, with custom stencil per the plan. A shader that fails to compile at boot is a crash under FI4.
     - Reference silhouettes come from the plugin's own stencil and custom-depth, on clean frames.
     - The metrics are frozen and never lowered after the results:
       - blinking: drawn frames IoU(mask, target silhouette) ≥ 0.90; hidden frames IoU(mask, counterfactual silhouette) ≥ 0.90;
       - missing_object: IoU(mask, counterfactual silhouette from the last clean pre-apply frame) ≥ 0.90;
       - lod_corruption: IoU(mask, target silhouette in the same frame) ≥ 0.90, and ≥ 0.95 of changed pixels against the pre-apply clean frame inside the mask dilated by 1 px;
       - lighting_mismatch: ≥ 0.95 of changed pixels inside the mask dilated by 1 px, and mask pixels only on receiver actors (≤ 1 % outside their silhouettes dilated by 1 px);
       - all four: spill ≤ 1 % of mask pixels outside the reference region dilated by 1 px.
     - Negatives: a one-frame mask shift fails, and a mask taken from the wrong target fails.
   - FO4: docs/gotchas.md is append-only. Restore the original 094-31b paragraph and append the correction as a new numbered entry.
   - FO5: this job. If FO2's re-run of C1 or C2 fails, stop before C3. The window for real sessions ends at 12:00 IST on 11-01.
2. _reviews\094-31-ruling.md (FM1); _reviews\094-31b-ruling.md (FN2: the writer formats, no backbuffer override); _reviews\094-30-ruling.md (FL1–FL6); your plan docs/europa/PHASEC-capture-port-plan.md (§C1–§C3).
3. _reviews\094-27c-ruling.md (FJ1) and _reviews\094-27d-ruling.md (FK1), with the rulings they keep by name.
4. 094-32's results: _reviews\094-32-codex-report.md and _reviews\094-32-code-review.md.

STARTING POINT
- feat/europa-port at e19fefc. C1 and C2 pass on stock 4.25 on the backbuffer path: 10-bit, D3D11, 0 dropped; 480 labelled frames, 0 mismatches.
- The plugin heartbeat text is in every 4.25 frame.
- AnomalyShaders and CONTROL_SERVER are still 0 on 4.25.

ORDER
1. Entry: FJ1's fast checks.
2. FO2:
   - implement the pre-UI capture point, with its 4.25 source citation;
   - test only the changed code;
   - rebuild under FM1;
   - run the heartbeat-region check, then the C1 gate (PIE on EA_Core, -game on EA_Travel) and the C2 gate on the new path.
   If C1 or C2 fails and a quick, testable fix exists, use one more fresh slot. Otherwise stop before C3.
3. FO3:
   - compile AnomalyShaders in on 4.25, with the custom stencil;
   - build the negatives first, on synthetic or kept frames;
   - rebuild under FM1;
   - then one C3 session under FJ1's gate, covering all four anomalies with at least 3 events each.
4. FO4.
5. Exit: FJ1's fast checks; commit, with W4. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in steps 2 and 3 only.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. Binaries change only in the builds.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags.
- No security, paging, power or machine setting changes. No elevation. Delete nothing except by receipt.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report (report.md):
- a summary;
- FO2: the hook and its citation, the heartbeat check, and the C1 and C2 re-runs;
- FO3: the shader compile result, the negatives, and each anomaly's per-frame metric distribution with its pass or fail;
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
GREEN means FO2's re-runs and C3 all pass.
