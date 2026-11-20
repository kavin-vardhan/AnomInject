# 081-30 — Chat ruling on 081-29 NEEDS-DECISION: C1_G2_S voided as environmental; person-present gate added before the resume

Date: 2026-09-25 (IST) · Author: chat Claude (orchestrator) · Build under test: 3 (`85c99b3`, Game 002805CF) · Branch head `070013f`

## What happened
081-29 resumed c1 at `C1_G8_S`. Four legs passed. `C1_G2_S` then used up all three attempts because a person was using the PC:
- A1 held the foreground for only 82 of 107 samples; `SearchHost.exe` had the rest, and the cursor moved through 58 positions.
- A2 and A3 held it for 0 of 480 and 0 of 485 samples; `explorer.exe` had it, with the cursor parked on the taskbar.
- Input idle read 0.34 s at postflight.

No gate predicate was evaluated. Code stopped correctly: `stop_on_recorded` halted as designed (G290), and it did not hand-edit the ledger or the lock.

## Ruling 1 — `C1_G2_S` A1–A3 are ENV-VOIDED
The three attempts are voided as environmental (a person at the PC), not measurement results.
- Record a dated `ENV-VOIDED` row for `C1_G2_S` that cites this ruling. Use the same mechanism as `C1_G8_N`'s PASS-REEVALUATED (081-26).
- The three attempt rows and the NOT-RUN row stay in the ledger for audit.
- `stop_on_recorded` treats a NOT-RUN that is followed by ENV-VOIDED as cleared, and the leg gets a fresh 3-attempt budget.
- A NOT-RUN with no ENV-VOIDED still halts, exactly as now (G290 behaviour is preserved and must be proven).
- ENV-VOIDED can only clear NOT-RUN. It never clears FAIL, FIXTURE-VALID-FAILURE or NEEDS-DECISION.

## Ruling 2 — person-present gate (PP gate), in the lib, before the resume
The quiet gate sees processes, not a person (G292). Add a person-present check so a human at the keyboard makes the harness **wait** instead of burning attempts.

**(a) Before every attempt, for every leg kind (stack, legacy, nohold, Lyra), and at window start:**
- Wait until there has been **no human input for ≥ 60 s** before launching.
- Read input idle with `GetLastInputInfo` (ctypes, user32), or an equivalent.
- The harness's own synthesised input or focus actions must neither trip the gate nor be mistaken for a person. First find out whether the runners inject input or move the cursor. If they do, exclude those windows and write down how.
- While waiting, poll every ≤ 15 s and log `person-present-wait` with the idle reading.
- The wait counts toward the **same 45-min per-window NOT-QUIET budget**. When the budget runs out, exit 4 (NOT-QUIET class). Never record NOT-RUN for this.

**(b) Classifying an attempt that failed on foreground:**
- The attempt becomes **`ENV-INTERRUPTED`** if **both** of these hold:
  - it would otherwise be INVALID-PRECAPTURE or INVALID-FIXTURE on a foreground/focus reason;
  - there is person evidence inside the attempt window.
- **Person evidence** is any of these:
  - non-harness input registered;
  - the cursor moved while the harness was not moving it;
  - the foreground was taken by a shell process (`SearchHost.exe`, `StartMenuExperienceHost.exe`, `ShellExperienceHost.exe`, `explorer.exe`).
- ENV-INTERRUPTED does **not** count against the 3-attempt budget. The harness goes back to (a) and retries.
- **Cap: 5 ENV-INTERRUPTED per leg.** After that, exit 4 with a clear message. Never write NOT-RUN for it.
- A foreground failure **without** person evidence stays a normal invalid attempt, as it is today.

**(c) Fixture-valid attempts are never reclassified.**
- Record `person_evidence: true/false` on every attempt row.
- A PASS or FAIL keeps its verdict whatever the flag says. If a FAIL ever carries the flag, it goes to chat as NEEDS-DECISION with the flag shown. It is not auto-voided.
- This keeps the gate from ever laundering a measurement.

**(d) No measurement, predicate, threshold, prediction or PASS rule changes.** This is fixture scheduling only.

## Proof required (dry run on banked records, plus synthetic cases), both directions
1. The banked `C1_G2_S` A1, A2 and A3 classify as ENV-INTERRUPTED with person evidence, and the reasons are listed.
2. The four 081-29 passes (`C1_G8_S`, `C1_G9_N`, `C1_G9_S`, `C1_G2_N`) and every banked c0/c1 record show **no** person evidence. This is the false-positive check; list the counts.
3. A foreground failure without person evidence (banked if one exists, otherwise synthetic) stays INVALID-FIXTURE.
4. Pre-attempt gate, synthetic:
   - idle 0.3 s → waits;
   - idle ≥ 60 s → proceeds;
   - the harness's own injection alone → not a person;
   - budget exhaustion → exit 4, and no NOT-RUN is written.
5. The 5-cap → exit 4, and no NOT-RUN is written.
6. A fixture-valid FAIL with the flag set → verdict unchanged and routed to NEEDS-DECISION.
7. The ENV-VOIDED row clears `C1_G2_S` with a fresh budget of 3. A synthetic NOT-RUN without ENV-VOIDED still halts. ENV-VOIDED on a FAIL row does not clear it.
8. Re-issue the boundary (lib hash, feature head) and run the full dry run with every leg green.

## Sequencing
- **081-30** is bench-free (xhigh): Rulings 1 and 2, the proofs, the boundary re-issue, docs (journal, and G292 marked mitigated), then push.
- **081-31** is the bench run (high), released only after 081-30 is GREEN: c1 resumes at `C1_G2_S`, then c2 and c3.
