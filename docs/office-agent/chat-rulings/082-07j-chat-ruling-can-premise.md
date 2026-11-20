# 082-07j — Chat ruling on G4 CAN: a gate-premise defect on the focus axis (G332). Re-encode it as STOP-BEFORE-FIRE, move cancel-before-focus to S2, and review all unrun legs for runtime conditions before resuming

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `848651e` · Staged `2FCDF059` / `20DA6F98` / `534C5863`

## What changed in S1 this run
- **Three more refusals are correct:** A5 `below_size_policy:min_64`, A6 `map_set_over_cap:9_of_8` and V2 `over_budget` (step V2). G-REASON Q rows now stand at 17 PASS.
- **G4 FIN-UV PASS:** one apply and one revert, a balanced ledger, and `M_TC_UC1` restored exactly.
- **The stop.** `G4_CAN_UV` is NOT-RUN on 3 of 3 attempts with CANCEL-LINE-NOT-SEEN.
  - `StartRun` waits for focus only when the window is unfocused (`AnomalyCaptureSubsystem.cpp:3362-3373`).
  - The runner's `Hold-Focus` forces focus from 400 ms on, so the run starts synchronously, and Stop finishes it with 0 frames.
  - The feature did nothing (no m53 line). The premise is wrong.

## Rulings
1. **G4 CAN is a gate-premise defect, not a feature defect.** Options:
   - **(b), a runner mode without forced focus: REJECTED.** It is not deterministic.
   - **(a) and (c) together: ADOPTED.**
     - **Now:** CAN-UV and CAN-NM are re-encoded as **STOP-BEFORE-FIRE** (class Q). A Stop in the same batch as the start must give a FINISH with 0 frames, `fires_applied = 0`, and **no m53 APPLIED / REFUSED / LEDGER line**.
       - **Proven both ways:** the banked attempts fit the encoding; a synthetic APPLIED line FAILS; frames > 0 FAIL.
       - The three attempts and the NOT-RUN record are **superseded**, with their history kept. **Both legs re-run fresh.** Do not pass them from the bank: the attempts carry person evidence (ruling 3).
     - **Owed in S2:** the real cancel-before-focus branch. It needs a bench-only "treat as unfocused" lever, and S2 builds anyway for its GC, level-change and re-created-component levers.
2. **Before the resume: a runtime-conditions review of all 35 unrun legs.** Three premise defects have now shown up only on the real bench (fixture, time, focus), because the stub proofs cannot model the runtime.
   - For each unrun leg and row, name every runtime condition its premise depends on: focus, input, window state, streaming or residency, timing, GC, level load, process lifetime.
   - Check each one against the **real runner** (`082-07-run.ps1`, including `Hold-Focus`) and against **banked real logs** from legs of the same kind.
   - Give one line per row: unaffected (with the reason), or affected, with a proven encoding fix or a declared disposition. G4, FailStep (P2-5), the watches and `EndPlay` (P2-6) and G-COLL (P2-2) especially.
3. **Person evidence on the CAN attempts.** Establish, from timestamps, whether the 1–2 input events are the runner's own synthetic Alt in `[M53Fg]::Force`.
   - **If they are:** say whether any unrun leg could be voided by it. If so, fix the detector so it excludes self-generated input, proven both ways: a real input still counts.
   - **If they are not:** report the evidence.

## Sequencing
- **082-07k (xhigh, continue the 082-07j session, bench-free):**
  - the runtime-conditions review;
  - the person-evidence read;
  - AMENDMENT 4: STOP-BEFORE-FIRE, the S2 debt, and any changes from the review;
  - supersede entry 5;
  - the proofs, no-flip and dry run;
  - the docs commit, the boundary re-issue and the preflight replay;
  - the 082-07l invocation.
- **082-07l (high, fresh session):** the remaining legs, with P2-2, P2-5, P2-6 and `MW-FLOOR`.
- **Then:** the Codex evidence review of S1 through the Relay.
