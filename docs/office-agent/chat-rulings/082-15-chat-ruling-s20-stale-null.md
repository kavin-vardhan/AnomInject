# 082-15 — Chat ruling on S2-0: STALE-NULL. No restore defect, in m53 or in the shipped material swaps; MainWorld comparisons need a refresh-matched null

Date: 2026-09-28 (IST) · Author: chat Claude · Head `d117a9d`

## Result (accepted)
- The trace **reproduces exactly** with the shadow cache on, and is gone or at the noise floor with it off.
- **The trace is reproduced by the shipped `corrupted_texture` swap AND by a plain render refresh that changes no material.** C vs ID-F and R vs ID-F both read max 2, px 0.
- **Effective material is restored on 30/30 slots on all 24 legs.** Raw and Resolved are unchanged.
- **Classification STALE-NULL:** the untouched control keeps a stale cached shadow from load. Any render-state refresh (install, swap or plain re-register) shows the refreshed shadow.
- **m53 leaves nothing behind.** There is no plugin-wide restore defect in the shipped anomalies. The owner is told as good news; nothing is escalated as a bug.

## Rulings on the NEEDS-DECISION items
1. **The cache-off B cells:** record them as "absent at the noise floor" (max 3 on 1 of 60 si, 0 px ≥ 16). The flags stay as emitted, and nothing is re-run. For any future window, the absent bound carries a one-level margin over the null bound (G342).
2. **Classification:** STALE-NULL is primary. "REPRODUCED-BY-SWAP, CAUSE UNRESOLVED" stays as the descriptive companion, as the rule required. No mechanism is asserted; the shadow cache remains a candidate.
3. **S2-1:**
   - **no m53 restore fix;**
   - the override-array growth stays a named limit with no effect on rendering;
   - **the MainWorld comparator rule, for S3's MainWorld arbiter and any MainWorld pixel row:** the control must experience the **same render-state refresh** (a refresh-matched null), **not** a region-K exclusion. The stale shadow could sit anywhere on other hosts, so excluding one known region is not sound.
   - The per-component refresh lever is deferred to S3's arbiter design, with the ~16 % refresh cost budgeted there.
   - S2's MainWorld D rows record the caveat.
4. **Region R before the event** (si 28): note it for future windows. No action now.

## Findings for the docs
- **S4 client docs:** any material-swap anomaly refreshes the cached shadows near its target. Frames after revert can therefore differ slightly from a never-touched run in contact-shadow regions. This is correct rendering, not residue.
- **The future replay/pair capture tool:** a clean pass and an injected pass will differ in cached shadows outside the anomaly windows. Pair generation needs a refresh-matched clean pass. Record it in the roadmap.

## Next
- **m53 S2 stays paused.**
- **The owner's priority, the m52 label-timing fix:** 084-01 part A (bench-free, Codex in parallel) now. Part B (bench) runs when the PC is free.
