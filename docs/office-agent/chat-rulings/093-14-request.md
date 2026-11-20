093-14 — CONTINUE J2 on the new binary (9781031): make the camera reader's stale-projection donor drop-aware (AB1), re-run the full PIE matrix (AB2), then Lyra camera, TSR half-resolution PF-2, the cook, every staged gate and the push, on fix/followup-capture. Requested by chat Claude (the orchestrator) on 2026-10-16. Launched through the implementer Relay by Claude Code (mailbox), who reviews after you finish; chat rules.

BINDING
- _reviews\093-13-ruling.md (AB1–AB6). This request implements AB1–AB5.
- _reviews\093-12-ruling.md (AA1–AA7), 093-11 (Z1–Z5), 093-10 (Y1–Y7), 093-09 (X1–X4), 093-08 (W1–W6), 093-07 (L1–L7) and 093-06 (K1–K8), plus the 093-06 to 093-13 requests, unless AB1–AB5 change them. AA3 item 4 (the identity proof) is superseded by AB2.
- Your own 093-13 report, and the evidence under E:\IA_BuildCache\_r9313 (and _r936 to _r9312).
- Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

WHERE YOU STOPPED
The fix 9781031 works: both StackOBot PIE camera legs pass the cut and the blend. The facts:
- 990 = 984 + 6 named drops;
- every labelled row has a measured mask;
- PF-2 ≈ 1.000, including the moving frames;
- the edges are 0;
- both active events continued through both switches.
The gate failed only because camera_assess.py:160 hard-codes donor sample 299 for the stale-projection control, and every switch correctly drops 297–299. That is a control that can never fire (AB1).

STEP 1 — PRE-FLIGHT
1. Rehash the new candidate banks against the 9781031 manifest (StackOBot EXE d8d30951, Lyra EXE dfe6f164), including the relinked Lyra fixture DLL (record its hash).
2. Hosts, live staging and content match your 093-13 restoration receipt. Worktree at 0bc6e9f, clean.

STEP 2 — FREEZE (one docs/predictions commit, before any assessment or leg)
1. AB1, the drop-aware donor:
   - for each switch, the donor is the last written sample before that switch, found from the leg's own receipts;
   - if no written sample exists within 10 samples before a switch, the control cannot be built, and that is a STOP;
   - harness problem "camera control donor", attempt 1 of 2.
2. AB2, the run list: every PIE gate on the new binary, both fixtures (PF-1 to PF-5 and every PIE case in the 84-case ledger), except the exact J1 delayed and pre-F1 wrong-view controls (archived DLLs), whose results stand.
3. AB3-C, the resize expectation: ExposureExclusionPx is a log reading (the DRAWN SUMMARY value), not a run_summary field.

STEP 3 — RE-ASSESS THE RETAINED STACKOBOT PIE CAMERA ACTIVE LEG
- Use the drop-aware control on the retained _r9313 session. No engine re-run.
- The stale-projection control must reject.
- Keep the original "control unavailable" outcome on record.

STEP 4 — LYRA CAMERA, PIE
- Run the reference and active legs with the same switch spec (cut at 60, blend at 300).
- Gate (K6): edges 0, every labelled frame with a measured mask, PF-2 on the moving frames, and the drop-aware control rejecting.
- Readings: drops per switch, whether each active event continued or ended with its reason, and the change-stage extent_mismatch count.

STEP 5 — THE FULL PIE MATRIX ON THE NEW BINARY (AB2)
- Every case with its original predicates and can-fails.
- Pairing at the historic recipe, with Z2 parts 1, 2 and 4 on the 50% legs.
- The resize legs carry the AB3-C log reading.

STEP 6 — TSR HALF-RESOLUTION PF-2, PIE, BOTH FIXTURES (AA4)
- The frozen 093-13 spec.
- Gate: 1.000 ± 0.010 per axis, with at least 20 qualifying rows per axis; the 0.96 mutant rejects.
- Fewer than 20 rows on an axis is a STOP.

STEP 7 — THE COOK
-nodebuginfo is accepted, provided the staged EXE and DLLs rehash equal to the new manifest.

STEP 8 — EVERY STAGED GATE, BOTH FIXTURES
This includes:
- the staged camera switch (with the drop-aware control);
- staged pairing (Z2 parts 1, 2 and 4);
- the four staged PF-2 cases;
- staged TSR half-resolution PF-2;
- staged resize (W2);
- the staged controls.

STEP 9 — PUSH
Push fix/followup-capture (fast-forward) only when every gate is green.

STOP RULES (AB5)
- Pairing recipe: closed. PREVIOUS > 0 is a STOP. NEITHER > 0 is a STOP, after taking the Z3 spill readings.
- Marker: spent. Packaging metadata: spent.
- Camera control donor: at most 2 attempts. A control that cannot be built is a STOP, never a pass.
- A camera-switch product failure is a STOP.
- A TSR half-resolution leg with fewer than 20 qualifying rows on an axis is a STOP.
- Otherwise D2 is unchanged.

NOT AUTHORISED
- any product source change;
- a reader, tolerance or predicate change beyond AB1;
- a fixture content change;
- re-running a leg until it passes.

FINISH, DOCS
- Audits, archive (QUALIFIED only if every gate is green), restore by receipt.
- Docs:
  - a gotcha for the donor lesson (a can-fail must survive the W2 drops);
  - the journal, including the AB1 freeze and its timing;
  - the open item for AB3-D (change evidence under camera motion);
  - the status block.
- The owner may be using the PC. The idle gate protects that; never touch his processes.

FINAL REPORT (report.md and your final message)
As in 093-13, plus:
- the AB1 freeze commit and its time against the re-assessment and the first leg;
- the re-assessed StackOBot camera control;
- the Lyra camera gate and readings;
- the full PIE matrix table on the new binary, with can-fails;
- the TSR half-resolution PF-2 per axis (n and scale), PIE and staged;
- the cook's staged hashes against the new manifest;
- the full staged matrix with can-fails;
- the drop accounting and the admission counters per leg;
- the push.
End with its own line: VERDICT: GREEN | NEEDS-DECISION | RED.
