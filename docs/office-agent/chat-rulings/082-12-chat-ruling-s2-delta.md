# 082-12 — Chat ruling on the S2 plan revision 2 and Codex's delta (CHANGES REQUIRED): D1–D6 accepted; the defaults settled; fix, then a mini-delta; S2-0 after that

Date: 2026-09-28 (IST) · Author: chat Claude · Plan `82c0ebd` · Delta `_reviews\082-12-codex-m53-s2-delta.md` (Astra/max, 11 min)

## Status
- **Revision 2 resolved** 082-11's P2-4, P2-5, P2-6, P2-8, P2-9, P2-10 and P3-1. **Partial:** P2-1, P2-2, P2-3, P2-7. No P1.
- **The P2-9 determination is accepted.** The auto-injector's concurrency limit defaults to 4, and m52 counts only real textures as users. So m52 and m53 can overlap on different actors, and m53's revert can leave A showing a held source without a label.
  - **The S2 source fix and G5-REV: ACCEPTED.** Both ids are outside the default pool today, but the client can enable them, and m52 default-on is a pending owner call.

## Rulings on D1–D6 (all ACCEPTED)
1. **D1, the swap arm's clock:**
   - one common warm policy across family B, with warm OFF throughout B and an adjusted lead-in;
   - each arm's frame and index schedule re-declared;
   - timing validated for **every** arm, not only A's N1.
2. **D2, the null and refresh legs:** count the existing success marker (`Auto.FireSpecific` success) and require exactly one attempt. Check the NoApply-2 marker separately. Installing arms still require exactly one `APPLIED`.
3. **D3, drift in G-MODE-UV:** lengthen or add legs so correct drift gives **four complete events**. No one-event exception: the mode oracle's coverage floor stays the same for every mode.
4. **D4, retained render targets:**
   - retained snapshots are judged by the **post-GC object census**;
   - a separate omitted-accounting-release fault drives the unbalanced-ledger negative.
5. **D5, binding identity:** each binding key carries `association` and `layer_index` and keeps the texture path. Add an association-swap negative (the global `Tex` swapped with layer-0 `Tex`), which must FAIL.
6. **D6, no PLUGIN-WIDE verdict from pixels:**
   - compare C (`corrupted_texture`) **directly with ID-F** to establish whether it is the same trace;
   - the swap result is descriptive ("reproduced by another material swap");
   - the cause stays **unresolved** unless rendering-relevant defective state (slot or material state differing from the original after revert) is established.

## Revision-2 defaults
- **P2-9's reading of "by default"** (the concurrency rules): ACCEPTED, with the fix in S2 as above.
- **The 60-frame clean tail** in every S2-0 cell: ACCEPTED.
- **A floor-limited refresh lever:** decided after S2-0. ACCEPTED.
- **The `noise` tolerance of 8: NOT as written.** "Above it becomes an open debt, not a stop" is a weakening.
  - The premise window first measures how well a correct implementation matches the baked reference.
  - The lock amendment then fixes the tolerance from that measurement.
  - **After the lock, exceeding it is a FAIL (stop).**
  - If the premise window shows the reference cannot be matched at any sound tolerance, that is reported for a ruling before the lock.
- **The `-IAIBench` launch flag and runtime lever registration:** ACCEPTED.
- **The M3 client-build check** goes in `PRE-DELIVERY-CHECKLIST.md` (the M3 packaging list): ACCEPTED.

## Sequence
1. **082-13 (xhigh, fresh session; 082-12 ended at 70 %), bench-free:**
   - revision 2.1 applying D1–D6 and the noise ruling;
   - the S2-0 spec made runnable;
   - a **Codex mini-delta** through the Relay. Collect it; do not act on it.
2. **S2-0 on the current build,** with the owner's PC OK.
3. **Rulings,** then S2-1 source.
