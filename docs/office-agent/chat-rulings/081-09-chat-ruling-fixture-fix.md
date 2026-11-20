# 081-09 — chat ruling: shared failure signature ACCEPTED as sufficient; fix the fixture (placement + look-input lock), then validate 5/5 under focus (chat Claude, orchestrator, 2026-09-21)

**Ruling: R1 step 3 applies. No further diagnosis repeat.** The Stage 1 binary fails B1 too (1/5 raw, and its only focused-throughout passes are the ones with 90/90 foreground), so this is not a Stage 2 regression; and the table itself shows the mechanism well enough to act on:

- Every failure row has **partial** foreground (70/90, 35/90, 26/90, 31/90). Every pass row has foreground either **all** samples (90/90) or **none** (0/90). Origin never moves; only orientation does.
- That is the signature of focus *transitions* feeding mouse deltas into the player controller's look input: the game gains or loses foreground mid-settle, the accumulated cursor delta lands as a yaw/pitch, the camera swings. No focus change ⇒ no delta ⇒ stable pose, whether the game is in front or not.
- The contending foreground owner is `SearchHost` (Windows Search / Start UI) on the owner's PC — a host-machine confound, not plugin code. It is reported to the owner separately; the fixture must be robust to it regardless.

## Fix (bench-side, both in the Stage 2 rebuild; no host source, no cook)
F1 **Look-input lock.** A bench-only lever (gated like `IAI.Bench.*`, Game target too) that calls `SetIgnoreLookInput(true)` (and `SetIgnoreMoveInput(true)`) on the fixture player controller at world-ready, held until run end. Bench legs set it before settle. Log on/off. This removes the mechanism; it does not depend on focus.
F2 **L2 placement on StackOBot.** The `AnomalyBench` placement extended to the Game target and to the StackOBot fixture: after settle, set control rotation (and pawn/camera origin, from the measured offset) to the B1 calibrated pose — cite the calibrated values — once, then the unchanged B1/A56 check on a fresh snapshot. Placement never self-certifies. Log camera and pawn origins as on Lyra.
F3 **Focus is still recorded, and the existing A63 focus rule still applies to measurement legs** (foreground required for the whole leg); F1 makes a focus blip harmless to the pose, it does not make it acceptable for capture. Foreground PID and sample count go into every leg's audit as they did here.

## Validation before any allowance renews
Rebuild, stage, hash, archive (`2C21EDE5` stays archived as the pre-fix Stage 2 candidate). Then the preflight series again, **no captures**: 5 launches on the fixed binary with F1+F2 on, and 3 with F1+F2 off (control, same session, alternating). Predeclare: on ≥ 5/5, settle sample 0, modal pose 0/0/0, coverage 100 %, B1 PASS, and foreground recorded (report the counts; a pass with 0/90 foreground is still a pass for this preflight, per the table). If the fixed binary is < 5/5, stop and report — do not renew allowances.

## Then
On 5/5: every Stage 2 leg gets a fresh 3-attempt allowance (accepted twins stay accepted; DELAY_N's three stay uncredited), and R2–R4 run per 081-08 on the fixed binary — including a re-run of the four static twins on it (they are cheap and the binary changed), the six-leg legacy comparison against `AEBD09EA`, and the mandatory teardown gate. R5 Lyra last, unchanged. Report as `081-10` (fix + preflight validation) and `081-11` (R2–R4), `081-12` (R5); bounds as before.

Gotcha to add: *B1 pose failures on CB_GateLevel correlate with partial foreground during settle (focus transitions feed mouse deltas into look input; `SearchHost` was the contender on 2026-09-21). Fixture legs lock look/move input and place the view after settle; focus is recorded and still required for measurement legs.*
