# 093-13 — Chat ruling: the crash fix works; the camera stop is a control that could never be built; make its donor drop-aware, drop the identity shortcut, and re-run the full PIE matrix on the new binary

**Date:** 2026-10-16, about 03:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- Codex's continuation: `report.md` in the J2 run folder. Same thread, one try, 00:03:42–01:46:22 IST, 6,160 s, exit 0, VERDICT NEEDS-DECISION. Evidence `E:\IA_BuildCache\_r9313`.
- Claude Code's review: `_reviews\093-13-code-review.md`. 22 OK · 0 GAP · 5 OPEN (work the stop left unrun); observations A–G.
- Mailbox report 093-13: NEEDS-DECISION, $14.91 API-equivalent.

## Verdict
**J2 is still not qualified, and nothing is pushed.**
- Remote `fix/followup-capture` stays at `1bf7054`.
- Local head is `0bc6e9f` (docs). The design note is `ef03f36`, committed before the fix `9781031`.
- **New candidate built from `9781031`:**
  - StackOBot EXE `d8d30951`, Lyra EXE `dfe6f164`;
  - all 8 builds 0/0, every audit passing;
  - 32/32 files rehash identical across two banks.
- Archive: `followup-capture-J2-exposure-09313-9781031-D8D30951-DFE6F164-UNQUALIFIED`.

**Accepted on review:**
- **The fix (AA2):** two files, +17/−10.
  - One `ResetExposureExclusion()` helper resets all five values (mask, folded set, width, height, pixel count). It is used at run start, at an owner change and at a shape change.
  - Allocation happens whenever `Mask.Num() != W*H`.
  - Nothing else changed.
- **The CPU self-test:** the old logic fails the same-size owner change with an out-of-range index, which is the crash. The new logic passes all five cases. Code re-ran both binaries himself.
- **The crash is gone.** Both StackOBot PIE camera legs pass the cut and the blend, with:
  - 990 requested = 984 written + 6 named drops (the 3 pending samples before each switch);
  - every labelled row with a measured mask;
  - PF-2 ≈ 1.000, including the moving frames (X 1.00073, Y 1.00013, n = 44 each), with the 0.96 mutants rejecting;
  - onset and recovery error 0 on all four events, by Code's independent pixel check;
  - both active events continuing through the cut and the blend.
- **The stop was right.** The frozen reader could not build one of its controls, and changing the reader was outside 093-13's authority.

## Rulings

### AB1. The camera stop: a control that could never fire
- **What happened:** the stale-projection control in `camera_assess.py:160` hard-codes donor sample 299. That code dates from 093-07, before W2 made switch drops legal.
- **Why it can never work:** every switch retires the 3 in-flight samples just before it (57–59 at the cut, 297–299 at the blend). So 299 is missing on every camera leg, on any binary. It is G146 turned around: a check that cannot fire for any input.
- **What it is not:** a product finding. Everything else in the gate passed.

**The fix to the control.** This is harness problem "camera control donor", attempt 1 of 2.
- **Donor:** for each switch, the donor is the **last written sample before that switch** (296 at the blend here). It is found from the leg's own receipts, not hard-coded.
- **No donor at all:** if no written sample exists within 10 samples before a switch, the control cannot be built, and that is a STOP. A missing control never counts as a pass.
- **Order:** freeze this rule and commit it.
- **Re-assess the retained StackOBot active leg.** Its positive checks already passed independently, so no engine re-run is needed. The stale-projection control must now **reject**.
- **The record:** the original "control unavailable" outcome is kept.

### AB2. The identity proof is withdrawn; re-run the full PIE matrix instead
**Why:**
- As frozen, the identity gate fails between two runs of the **same** binary (Code, observation B). Mask tag values, frame and time stamps, event ids, `exposure_dip` on 32 rows and `end_frame` all vary from run to run.
- Designing a sound comparator now would cost more than simply re-running.
- Night-time bench runs are fast: 32 cases took about 2.7 h on 10-15.

**What runs now:**
- Every PIE gate on the new binary, both fixtures: PF-1 to PF-5 and every PIE case in the 84-case ledger, with their original predicates.
- **Exceptions:** the exact J1 delayed and pre-F1 wrong-view controls. They load archived DLLs, never the candidate, so their results stand.
- The AA3 item-4 shortcut is superseded.
- Pairing legs run at the historic recipe, with Z2 parts 1, 2 and 4 on the 50 % legs. The pairing stop rules stand: the recipe is closed, and any NEITHER is a STOP after taking the spill readings.

### AB3. Code's observations C–G
- **C:** `ExposureExclusionPx` is a log value, not a `run_summary` field. The resize expectation becomes a log reading (the DRAWN SUMMARY value). No new field.
- **D:** the change-stage pairs read `extent_mismatch` (4 per event at onset) on both camera legs, so change evidence never validates there.
  - Not a J2 gate.
  - Recorded as an open item: **check whether m55 change evidence validates under camera motion.** It goes on the list for after J2, not into this job.
- **E:** Lyra's fixture DLL relinked to a new hash from unchanged source. With the full re-run, every Lyra leg uses it. Record it in the manifest.
- **F:** noted.
- **G:** the owner's apps ending (a Claude Desktop session, a render job) are for his awareness only.

### AB4. Order on the new binary (`9781031`)
1. **Pre-flight:** rehash the new candidate banks; check the restoration state.
2. **Freeze and commit:**
   - AB1's donor rule;
   - AB2's run list;
   - AB3-C's resize reading.
3. Re-assess the retained StackOBot PIE camera active leg with the drop-aware control.
4. **Lyra camera, PIE:** reference and active legs. The gate is as K6 (edges 0, every labelled frame with a measured mask, PF-2 on the moving frames), plus the drop-aware control; plus the readings.
5. The full PIE matrix (AB2).
6. **TSR half-resolution PF-2, PIE, both fixtures (AA4):** at least 20 rows per axis, or a STOP.
7. **The cook.** `-nodebuginfo` is accepted; the staged binaries must rehash equal to the new manifest.
8. **Every staged gate, both fixtures**, including:
   - the staged camera switch (with the drop-aware control);
   - staged pairing (Z2 parts 1, 2 and 4);
   - the four staged PF-2 cases;
   - staged TSR half-resolution PF-2;
   - staged resize (W2);
   - the staged controls.
9. **The push** (fast-forward, `fix/followup-capture` only) when everything is green.

### AB5. Stop rules
- **Carried from AA5:** pairing recipe closed; marker spent; packaging metadata spent; a camera-switch product failure is a STOP; TSR half-resolution with fewer than 20 rows is a STOP.
- **New:**
  - **Camera control donor:** at most 2 attempts.
  - A control that cannot be built is a STOP, never a pass.
- Otherwise D2 is unchanged.

### AB6. For the owner
- Nothing to do.
- The crash fix is proven on StackOBot. Lyra, the full re-run, the packaged game and the push follow.

## Next
- **093-14 is queued:** Codex resumes its J2 thread under AB1–AB5, and Code reviews after Codex exits (W3).
- **After J2 qualifies at home:**
  1. office checkpoint I;
  2. then `research/natural-causes` and J3 (093-00 r3 §1b);
  3. plus the AB3-D change-evidence check and the mailbox NAME hardening (AA7).
