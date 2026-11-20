093-13 — FIX the camera-cut crash and RE-QUALIFY J2 on the new binary: a scoped PIE re-run backed by an identity proof, TSR half-resolution box agreement on a proper fixture, the cook, every staged gate, then the push, on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-15. Launched through the implementer Relay by Claude Code (mailbox), who reviews after you finish; chat rules.

BINDING
- _reviews\093-12-ruling.md (AA1–AA7). This request implements AA2–AA5.
- _reviews\093-11-ruling.md (Z1–Z5), 093-10 (Y1–Y7), 093-09 (X1–X4), 093-08 (W1–W6), 093-07 (L1–L7) and 093-06 (K1–K8), plus the 093-06 to 093-12 requests, unless AA1–AA5 change them.
- Your own 093-12 report, and the evidence under E:\IA_BuildCache\_r9312 (and _r936 to _r9311).
- Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

WHERE YOU STOPPED
The StackOBot camera-switch reference leg crashed at the first cut (sample 60). Chat's ruling (AA1) accepts your diagnosis, which Code confirmed independently:
- the ownership reset at AnomalyCaptureSubsystem.cpp:4731-4733 empties the exposure-exclusion mask but keeps its width, height and pixel count;
- the cut keeps 1280x992, so :1486 skips allocation, and FoldExposureExclusion reads the empty array at :1524.
The block came in with J2's e13ddc52. It is absent from the pushed J1 (1bf7054) and from the release (d7eee97), so no delivered build is affected.

STEP 1 — PRE-FLIGHT
1. The eff797c candidate banks rehash identical (they are the identity-proof baseline).
2. Hosts, live staging and content match your 093-12 restoration receipt. Worktree at 7049614, clean.

STEP 2 — DESIGN NOTE AND PREDICTIONS (committed before any product code)
1. Every site that resets or reallocates the exposure-exclusion state.
2. The fix (AA2):
   - one helper resets mask, folded mask, width, height and ExposureExclusionPx together, at every reset site;
   - allocation at :1486 is keyed on the array's actual element count against width x height, not on a cached size change.
3. The fix's reach: every code path and every J2 case it can alter. At minimum, the delayed viewport, resize and the camera switch.
4. The identity-proof spec (AA3 item 4), with the run_summary fields expected to differ, and why.
5. The TSR half-resolution PF-2 spec (AA4), frozen: fixture arrangement, recipe, estimator, the n >= 20 rule, the 0.96 mutant.
6. A CPU self-test, if the existing self-test harness can host it: an owner change at the same size, then a fold. The old logic must fail it; the new logic must pass.

STEP 3 — THE FIX
- Scope: the exposure-exclusion state in AnomalyCaptureSubsystem.cpp, plus its header if the helper needs a declaration.
- Each of these is a STOP (NEEDS-DECISION): a change anywhere else, an engine edit, a new module dependency, or a private header beyond what capture already uses.
- One product commit (Conventional Commits, scoped capture).

STEP 4 — BUILD THE NEW CANDIDATE
All 8 builds with 0 errors and 0 warnings, the binary and lever audits, the string audit, and a new manifest. Rehash before every leg as before.

STEP 5 — PIE ON THE NEW BINARY (AA3)
1. Camera switch (K6), both fixtures. Run the same leg specs (cut and blend).
   - Gate: edges 0, every labelled frame with a measured mask, PF-2 agreement on the moving frames.
   - Readings: frames dropped per switch; whether each active event continued or ended, with its reason; and Code's observation D (change-stage extent_mismatch on the identity-mode reference).
   - Can-fail: the retained 093-12 crash on eff797c with the same leg spec. Do not re-run it.
2. The fix's reach, both fixtures, in full with the original predicates: at minimum the delayed viewport and resize, plus any other case the design note lists.
3. Identity proof, AA-off arbiter recipe, both fixtures. Re-run the UV regression and the healthy single-view case, and compare each with its eff797c session.
   - Gate: the PF-3 comparator (same events, targets, edges; identical visible_positive; bbox within 0.5 px); masks byte-identical; label rows field-identical; run_summary identical except the fields the note names.
   - Reading: pictures byte-identical.
4. TSR half-resolution box agreement (AA4), both fixtures.
   - Setup: the PF-2 arrangement (target well inside the frame), the historic recipe (TSR), r.ScreenPercentage 50.
   - Gate: 1.000 ± 0.010 on each axis, with at least 20 qualifying rows per axis. The 0.96 mutant rejects.
   - Fewer than 20 rows on an axis is a STOP, not a reading.
5. Every other completed PIE case stands on eff797c through the identity proof: PF-1 to PF-5, the refusal cases, pairing, alignment, the J1 and pre-F1 controls, and the admission probes.

STEP 6 — THE COOK
-nodebuginfo is accepted, provided the staged EXE and DLLs rehash equal to the new candidate manifest.

STEP 7 — EVERY STAGED GATE, BOTH FIXTURES, NEW BINARY
This includes:
- the staged camera switch;
- staged pairing at the historic recipe, with Z2 parts 1, 2 and 4 on the 50% legs (part 3 is retired for pairing legs, AA4);
- the four staged PF-2 cases;
- the staged TSR half-resolution PF-2 legs (AA4);
- staged resize (W2);
- the staged controls.

STEP 8 — PUSH
Push fix/followup-capture (fast-forward) only when every gate is green.

STOP RULES (AA5)
- Pairing recipe: closed. Any PREVIOUS > 0 is a STOP. Any NEITHER > 0 is a STOP, after taking the Z3 spill readings.
- Marker: spent. Packaging metadata: spent.
- A camera-switch failure on the new binary is a STOP. The fix is not retried.
- An identity-proof difference outside the named fields is a STOP.
- A TSR half-resolution leg with fewer than 20 qualifying rows on an axis is a STOP.
- Otherwise D2 is unchanged.

NOT AUTHORISED
- any product change beyond AA2;
- a reader, tolerance or predicate change beyond AA4;
- a fixture content change;
- re-running a leg until it passes.

FINISH, DOCS
- Audits, archive (QUALIFIED only if every gate is green), restore by receipt.
- Docs:
  - a gotcha for the regression (an owner change at the same size must reallocate);
  - the architecture note on the exposure-exclusion state;
  - the journal and the status block.
- The owner may be using the PC. The idle gate protects that; never touch his processes.

FINAL REPORT (report.md and your final message)
As in 093-12, plus:
- the design note and fix commits, and their times;
- the diff summary;
- the new candidate's hashes;
- the self-test result, or why it could not be hosted;
- the camera-switch gate and readings, both fixtures, PIE and staged;
- the reach legs;
- the identity-proof comparison table;
- the TSR half-resolution PF-2 per axis (n and scale), PIE and staged;
- the cook's staged hashes against the new manifest;
- the full staged matrix with can-fails;
- the drop accounting and the admission counters per leg;
- the push.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
