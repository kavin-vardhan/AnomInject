# 082-07k — Chat ruling: AMENDMENT 4 accepted; run the last 35 legs (082-07l)

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `7a5b67b` · Boundary `1c3a348f` · Staged `2FCDF059` / `20DA6F98` / `534C5863`

## Accepted
- **The runtime-conditions review (§A4.3).** Every unrun leg was checked against the real runner and banked real logs, with citations. It caught three more false stops before any leg ran:
  - **The MainWorld camera drift** (~85 frames). The pixel legs now match a pinned camera path taken from 146 of 161 banked launches (held-out check 73/73). A mismatch stops as NOT-RUN with its reason, never as a wrong verdict.
  - **The GC detector was blind** (`LogGarbage` at Warning). It is now switched on per leg, with a "detector blind" reason.
  - **TWO's frame-1 apply** had never run on the bench. A named refusal now reads FIXTURE-CANNOT-EXERCISE.
    - Accepted as declared. If it fires, it joins the S2 list with T10 and CAN.
- **STOP-BEFORE-FIRE, and TRIP reading `run_summary.json`.** Proven 46/46 both ways on real data.
- **The record-level supersede key.** Without it, a label-less NOT-RUN would have stopped every window. Good catch.
- **The person-evidence read.** 10 of 13 flags are the runner's own final startup keystroke, and no unrun leg can be voided. The detector stays unchanged, as the ruling's condition wasn't met.
- **Proofs.**
  - `proof-evaluators` at 207/210: the 3 failures are the checks this amendment replaces, by design.
  - P5a is still the known 10 of 991.
  - No-flip is 511/511.

## Added to the S2 list
- the unexplained input at 51.7 s in two CAN attempts, 47 s after the runner's last action;
- save input timestamps, and excuse the runner's final startup keystroke in the detector;
- TWO, if it refuses.

## Next
- **082-07l (high, fresh session; 082-07k ended at 77 %):** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07l 540`. It runs the 35 legs from `G4_CAN_UV` (A4), with P2-2, P2-5, P2-6 and `MW-FLOOR`.
- **Then:** the Codex evidence review of S1 through the Relay.
