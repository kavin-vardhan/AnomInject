094-33c — RE-SCORE C3 on corrected rules. Blinking writes no mask on frames labelled clean (FQ3, 5.1's m44). Add the independent precision check (FQ2). Rebuild, then re-run C3 with auto-exposure locked for scoring. Under chat's ruling 094-33b (FQ1–FQ4), with FM1 and the lean profile (FJ1). Requested by chat Claude (the orchestrator) on 2026-11-01.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-33b-ruling.md (FQ1–FQ4). It is new.
   - FQ2, precision: the fraction of a mask's pixels whose colour visibly changed between the clean pre-apply frame and the anomaly frame, beyond the ±1-level grain.
     - The scoring session locks auto-exposure by a command-line override cited from 4.25 source. No config edit.
     - Gates (frozen before any measurement, never lowered):
       - blinking, frames labelled anomalous: counterfactual IoU ≥ 0.90 and precision ≥ 0.90;
       - missing_object: counterfactual IoU ≥ 0.90 and precision ≥ 0.90;
       - lod_corruption: current-silhouette IoU ≥ 0.90 and precision ≥ 0.80;
       - lighting_mismatch: receivers only (≤ 1 % outside their silhouettes dilated by 1 px) and precision ≥ 0.80;
       - all four: spill ≤ 1 %.
     - Negatives: the one-frame shift fails, and the wrong target fails for all four.
   - FQ3: blinking follows 5.1's m44. Hidden frames (labelled anomalous) carry the counterfactual silhouette; drawn frames carry no mask. Gate: 0 mask pixels on every frame labelled clean. FO3's drawn-frame clause and FO3's coverage metric are withdrawn.
   - FQ4: this job. One C3 session plus one retry slot. The C2 reader runs on the same session. The window for real sessions ends at 23:59 IST on 11-01.
2. _reviews\094-33-ruling.md (FP1–FP3); _reviews\094-32-ruling.md (FO2 done, FO3 as amended); _reviews\094-31-ruling.md (FM1); _reviews\094-31b-ruling.md (FN2); your plan's §C3 and the 5.1 m44 rule in the plugin's docs; _reviews\094-27c-ruling.md (FJ1) and _reviews\094-27d-ruling.md (FK1), with the rulings they keep by name.
3. 094-33b's results: _reviews\094-33b-codex-report.md and _reviews\094-33b-code-review.md.

STARTING POINT
- feat/europa-port at d43635d.
- 4.25 masks match the stencil (IoU 1.000, spill 0), and C2 holds. The descriptor fix is in.
- Blinking currently writes masks on drawn frames, against m44. Coverage of all changed pixels failed for lighting and LOD because the metric was ill-posed.

ORDER
1. Entry: FJ1's fast checks.
2. FQ3: blinking writes no mask on a frame labelled clean, matching 5.1's m44 (cite it). Test it.
3. FQ2:
   - add the precision metric to the C3 reader;
   - prove the wrong-target negative under precision on synthetic frames;
   - add the auto-exposure command-line lock for the scoring session, cited from 4.25 source.
4. Rebuild under FM1 if native code changed. Re-bind the manifest.
5. One C3 session under FJ1's gate: all four anomalies, at least 3 events each, auto-exposure locked. Score it against FQ2 and FQ3, run both negatives, and run the C2 reader on the same session.
   If it fails and a quick, testable fix exists, use the one retry slot.
6. Exit: FJ1's fast checks; commit, with W4. No push.

STOP RULES
- Only the armed kit launches the editor or the game. Builds are authorized in step 4, and for a retry's rebuild.
- Any change to the Paris host's Content, Config, a descriptor or the .uproject is a STOP. Binaries change only in the builds.
- An upload sign is a STOP. A second runtime crash ends the run (FI4).
- m51, master, release/2026-10-05, the tags, the 5.1 engine and StackOBot stay untouched. No push. No tags.
- No security, paging, power or machine setting changes. No elevation. Delete nothing except by receipt.
- Foreign activity never ends the job; record it (EZ1).

DELIVERABLE
A short report (report.md):
- a summary;
- the FQ3 change and its test;
- the precision metric, its negative and the auto-exposure override, with citations;
- the build;
- per anomaly, IoU, precision and spill against FQ2;
- mask pixels on clean frames;
- the negatives;
- C2 on the same session;
- any crash, upload sign or protected change (or "none");
- the commit.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
GREEN means all four anomalies pass FQ2 and FQ3, both negatives fail as they should, and C2 holds.
