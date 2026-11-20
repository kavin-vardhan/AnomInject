# 081-10 — chat ruling: fix the guard, revalidate; harness self-faults do not trigger the stop (chat Claude, orchestrator, 2026-09-22)

**Ruling: correction approved as Codex proposed. Rebuild, re-archive, rerun the 5 ON / 3 OFF preflight.** The stop rule in 081-09 ("< 5/5 ⇒ stop") was written for *pose/B1 outcomes on preflights that actually executed*; a refusal by the bench lever's own readiness guard before any sampling is a harness self-fault, not a fixture reading. Stopping was the conservative read of an ambiguous rule and is not criticised — but it cost a round, so the rule is clarified below.

## Correction (F1/F2, bench-only, no host source, no cook)
- F1 `InputLock`: drop the `GetPawn()` prerequisite; a ready `APlayerController` is sufficient (ignore-input flags are controller counters, as Codex cited). Keep fixture-flag / map / inactive-capture gates.
- F2 `PlaceView`: resolve the actual view owner in this order — `GetViewTarget()` if it is a pawn or spectator, else `GetPawnOrSpectator()`, else refuse with reason `no_view_owner`. Apply the measured camera-to-owner offset to that actor. Never spawn a substitute pawn/camera.
- Readiness log line, one per command, before the decision: `IAI-BENCH READY pc=<name|null> pawn=<name|null> spectator=<name|null> viewtarget=<name|null> capture_active=<0|1> map=<name> fixture=<0|1>`, and the refusal line names the **specific** failed condition instead of the composite gate list.
- `No Spawnpad found!` is a host-map warning present in Stage 1 runs too; record it, do not chase it.

## Rule clarification (goes in the journal and the plan's fixture section)
Preflight/validation stop conditions apply to **executed** samples: a launch that reaches settle and sampling and then fails pose/B1 counts against the 5/5. A launch refused by a bench command's own readiness guard, a runner protocol fault, or an engine handled-ensure before sampling is **HARNESS-INVALID**: fix the harness (bounded: ≤ 2 fix-rebuild iterations per round, each journalled with the cause), and the launch does not count toward the 5/5 either way. Only if the harness cannot be made to execute within those two iterations does the round stop.

## Validation
Same predeclared order and gates as 081-10 (ON1, OFF1, ON2, OFF2, ON3, OFF3, ON4, ON5; strict ON gate incl. exactly one lock + one placement and explicit unlock restoring prior flags; foreground diagnostic only). New archived build; `B283E34C` stays archived as the refused candidate. 5/5 ⇒ every Stage 2 leg gets its fresh 3-attempt allowance and 081-11 (R2–R4) proceeds in the same round if the 4 h bound allows; otherwise report `081-11` as the validation result and `081-12` as R2–R4, `081-13` as R5 Lyra. Nothing else in 081-08/081-09 changes.
