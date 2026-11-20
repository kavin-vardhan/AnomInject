# 082-13 — Chat ruling on Codex's mini-delta (CHANGES REQUIRED): M1–M4 accepted; no further Codex round for S2-0; build the S2-0 harness bench-free

Date: 2026-09-28 (IST) · Author: chat Claude · Plan `9d622e0` · Mini-delta `_reviews\082-13-codex-m53-s2-minidelta.md`

## Status
- **Resolved:** D1–D5, P2-2 and P2-7. **Partial:** D6, P2-1 and P2-3. No P1.
- **Runnable on the current build without a new binary:** Code's token, hash and batch checks show it, and Codex reproduced them.

## Rulings (all ACCEPTED)
1. **M1:** PLUGIN-WIDE-RESTORE-DEFECT requires an **actual Effective-material mismatch after revert on both arms**, together with the same trace. A `left-to-game` or `unresolved` count alone never classifies. Anything short of that reads "CAUSE UNRESOLVED".
2. **M2:** drop the impossible bank-proof clause for T1–T8. Each timing check is proven by:
   - a synthetic negative (a mutated schedule must FAIL);
   - where the bank has the lines, a bank positive;
   - **first-leg live validation,** which stops the window on a mismatch (already in the plan).

   State the source of each check's proof.
3. **M3:** drift offsets are counted in **engine frames** (k = 1, 4, 5 … 102, ending at 0.85 UV, per Codex). Recompute the drift expectations.
4. **M4:** strike the stale "≤ 8" in §6.2's G-MODE-N rule column.

## Why there is no further Codex round before S2-0
- S2-0 is a **diagnostic** (class D): its outcome comes to chat for a ruling and cannot pass or fail the feature.
- With M1–M4 applied, chat checks the fixes in the next report.
- Codex's next checkpoints are the S2-1 source review and the S2 evidence review, as planned.

## Next
- **082-14 (xhigh, fresh session, bench-free; no compiles):**
  - apply M1–M4;
  - build the S2-0 harness (runner reuse, the evaluator with the §3A.7/§3A.8 classifications, the timing checks, the trace and state readings);
  - prove it both ways on the bank and on synthetic data, including Codex's own shadow numbers reproduced exactly;
  - a dry run of all 24 batches with the launch stubbed;
  - lock S2-0's predictions and harness, and give the S2-0 invocation.
- **082-15 (high): the S2-0 run.** Only when the owner confirms the PC is **idle** (nobody using it), because the runner takes focus and input.
