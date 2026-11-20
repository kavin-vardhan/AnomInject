# 082-06c — Chat ruling on 082-06b (NEEDS-DECISION) and Codex's fix delta (CHANGES REQUIRED): two small remainders accepted

Date: 2026-09-27 (IST) · Author: chat Claude (orchestrator) · Delta `_reviews/082-06b-codex-m53-s1-fix-delta.md` (Astra/max, 649 s) · Head `40aa07a`

Codex's delta resolved seven of the nine findings: P2-1, P2-3, P2-4, P2-5, P2-6, P3-1 and P3-2. It found no new P1 or P2. Two remainders are accepted:

- **P2-2 remainder.** The collateral collector must include a drawn mesh whose slot is empty or null. The engine renders the **default material** there, so the collector should resolve that material and measure its textures.
  - Any texture entry the collector cannot resolve increments an `collateral_unresolved` counter and makes the collateral result **incomplete**. It is never clean.
  - Proof: harness rows for a null slot (measured through the default material) and an unresolvable entry (counted as incomplete, never reported clean).
- **P3-3 / new P3: the rollback balance criterion.** The line logged at the moment of rollback may legitimately show pending bytes > 0. The balance check is a **frame-stamped ledger reading two frames after the rollback**, which must show live = 0 and pending = 0.
  - Fix the log line and the plan text, then add a harness row: rollback-line pending > 0 is allowed, and the reading two frames later must be 0/0.

## Next
- **082-06c (xhigh, small):** the two fixes, then the comment strip, the G10 rebuild on the warm host (archive, don't stage), the harness and mutant run, and a Relay mini-delta limited to these two items. Collect it; don't act on it.
- If that is clean, **082-06 (authoring + cook)** follows.
