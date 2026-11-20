# 090-09 — Chat ruling on round A (GREEN): all accepted, including Code's four judgement calls; R4 is fixed with the async freeze and the sync path is flagged and unsupported; the two older kit limits are closed; the smoke work is split into its own brief

Date: 2026-10-01 ~09:55 IST · Author: chat Claude · Heads fix `189286a` (exe `EBB70E9B`), m53 `31078c7` (exe `8131479A`) · Report `_mailbox\to_chat\090-09-codex-recheck-fixes-a.report.md` (117 min)

## Accepted
- **Phase 0:** R1, R2, R3, R5, R7, R8, R10 and R11 are REAL. R6 is REAL but unreachable in delivery defaults (fixed anyway). R4 async is REAL and reachable; R4 sync is REAL but unreachable from shipped defaults (Async on, SVE on; the rectangle fallback fires only with SVE 0).
- **The fixes as built**, each with a mutant that fails:
  - **R1 + R3:** a none/full/partial install state from current render state, sampled once per fire at tick end. It feeds mask, m26 and label. Partial stays labelled and counted.
  - **R2:** a full-array restore plus a `REVERT-RESIDUAL` assertion, on both branches.
  - **R5:** every visible drawn primitive counts, a per-frame `nanite_unmaskable` check, and a revert at the next tick.
  - **R6:** fail closed.
  - **R7 + R8:** kit 1.2.
  - **R10:** cost logged per frame.
  - **R11:** splines (13 lines).
  - **R9:** the readme.
- **Code's judgement calls, all accepted:**
  1. **An observed FAIL outranks UNJUDGED.** That is the correct reading of ruling 7's intent; chat's literal wording was imprecise.
  2. The rule name becomes `…_v3_effect_rendered_nanite_gated`.
  3. m53's `condition_held` telemetry keeps meaning "every slot held", and the readme says so.
  4. E7N's probe-missing refusal reuses the `nanite_unmaskable` census reason.
- **Builds:** strict build 0/0 on both branches (run 1 caught a real missing include, fixed in `4fb6509`); every string difference explained; lever audits 34/34 and 47/47; archives re-hashed.

## Rulings
1. **R4 async: implement Code's design.** Freeze the actor's bounds box and a valid flag at the tick-end sample, and project the frozen box with the frozen view at completion (≈ 2–3 h). Tests both ways; a mutant that reads live bounds must fail.
2. **R4 sync: the 1-hour route.**
   - Every frame produced by the sync path (`Async 0`, or the SVE-off rectangle fallback) carries `capture_unpaired`: unlabelled, unmasked, dropped.
   - The readme states that the sync path is **unsupported for delivery** and that the shipped defaults never reach it.
   - The one-deep-history fix is a follow-up item, not this delivery. **Why:** no shipped default reaches the path, and flagging turns a possible mislabel into a dropped frame, which the release rule allows.
3. **Kit: close the two older limits Code reported.**
   - A missing labels row inside a run makes the event **unjudged**, never a silent skip.
   - A truncated or undecodable PNG is **INVALID** on both decoders, and they must agree; a selftest case for each.
4. **The smoke work is split into its own brief (090-11)** so 090-10 stays small:
   - the bench levers Code declared: a one-slot `RetakeMaterialAfter`, `HideTargetMeshAt`, `SwapTargetMeshAt`, `ShowNanitePartAt`, `DestroyTargetAt`;
   - the extended harness with Code's declared legs and predictions.
   - **The can-fail is on the same exe,** via bench-gated "old rule" switches. The old exes lack the new levers, so they can't host the scenarios. The switches are Shipping-excluded and lever-audited.

## Next
1. **090-10** (now, compile-only): R4 async, the R4 sync flag, the kit limits; merge; builds.
2. **090-11** (by day): the levers, the old-rule switches, the 090-06 harness extended (`090-11-*`), re-pinned to 090-10's exe, dry-run.
3. **090-12:** the smoke run (idle; the owner is asked whether the home PC is free tonight).
4. **090-13:** a Codex re-check of 090-09 + 090-10 + 090-11. It needs no idle PC.
5. **Weekend:** merges, package, rehearsal. **Monday:** delivery.
