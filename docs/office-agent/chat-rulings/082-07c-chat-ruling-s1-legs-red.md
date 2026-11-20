# 082-07c — Chat ruling on 082-07b (RED at G3 N-N1): fixture defect, not a feature defect. Fix the scene and harness, then resume

Date: 2026-09-27 (IST) · Author: chat Claude · Branch `93f5f1d` · Staged `2FCDF059` + `9A26D497`

## What the run established
- **UV route B works in-engine on the synthetic scene.**
  - G-ID was pixel-exact on all 12 UV rows (max d = 0, 57 frames, 8 events each).
  - Every assigned wrong copy read 58–206, against a floor of 16.
  - Restores were exact, the mip tripwire read 0 and all assets resolved.
  - P2-1 (2,990 readings) and P2-3 (368 events) are CONFIRMED.
- **The stop is a fixture defect.** The normal readout materials send the normal map into Emissive only, and never connect Normal. The feature correctly refused `normal_unconnected` at V1 (a serialized property mask, reliable in cooked builds), so G3 N-N1 had no events.
- **MainWorld yield finding.** The harness's floor actor reads `not_fully_resident` for both families, and the rocks read `texture_not_parameter`. On StackOBot at the bench camera, route B's strict admission yields **zero**. That is recorded as a yield finding for S3 and the owner; it is not a stop (the F-MW row is diagnostic).

## Rulings
1. **Fix the fixture.** Normal-readout materials must connect the **Normal** input in a shading model where it is active (Lit), and may keep the Emissive readout. Re-author only the affected packages (`M_TC_NN1`, `M_TC_UN1`, `M_TC_ChainD`, `M_TC_NormalEmis` and any other normal-family target). Prove admission offline and in a census leg: every normal Q-row target reads admitted in G0.
2. **Ratify the one-line driver, then retire it.** The locked `082-07-window.py` crashed on `L.budget_left()`; the lib has `L.budget_state()['left_s']`. That is the G293 class again. The window bug is fixed in the harness itself, and the boundary is re-issued before any resume. The accepted UV rows ran through the copy, which differs by that one expression only; their results stand.
3. **Harness gaps.** Before a Q row launches, the harness must confirm from G0 that the row's targets are **admitted** for the row's family. Otherwise it stops with FIXTURE-CANNOT-EXERCISE (a harness stop, not a feature FAIL). The G0 evaluator also covers the normal family on the synthetic scene.
4. **G3 is strengthened (dated amendment before resume).**
   - Compare the applied run's post-revert frame with the **null run's frame at the same index**: max |d| ≤ 2 inside the union mask, and whole-frame too. This is possible because G-ID shows applied and null frames are pixel-identical.
   - The old pre-onset vs post-revert count comparison stays as a diagnostic.
   - Give a cause read for the 363,001-pixel pre/post change, which is present in both applied and null runs. It is something time-varying in the scene: name it, or say it is unknown.
   - Re-evaluate the 12 accepted UV G3 rows **from the bank** under the new rule; no re-run. A UV row that fails the new rule is a stop.
5. **Resume.** Continue from N-N1 on a re-issued boundary with a new budget tag, keeping the recorded stop's history in the ledger (the m55 resume pattern).

## Sequencing
- **082-07c (high):** the fixture fix, M1″, re-cook, G-COOK under the amended rule, re-stage, A44, M2″.
- **082-07d (xhigh, bench-free):**
  - the window fix;
  - the Q-row admission pre-check;
  - the G0 normal-family coverage;
  - the G3 amendment with the bank re-evaluation of the UV rows, plus the cause read;
  - the predictions amendment covering the normal rows' self-contradiction, the G3 change and the MainWorld yield reading;
  - the boundary re-issue and dry run.
- **082-07e (high):** resume the legs.
