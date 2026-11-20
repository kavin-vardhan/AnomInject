# 082-07g — Chat ruling: AMENDMENT 2 accepted, all four locked defaults accepted, resume the legs (082-07h)

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `18336a9` · Boundary `6520d84d…` · Staged `2FCDF059` / `20DA6F98` / `534C5863`

## Accepted
- **AMENDMENT 2 as committed (§A2.1–A2.10).**
  - **R = 42 px,** measured exactly (distance transform over 27 banked pairs, cross-checked on tile rectangles). My "48" was a band edge; 42 stands.
  - **PRE-CLIP MASKED** is proven both ways, including single-pixel flips and the stop through the sequencer and a resume.
  - **The G-BIND (3) outside split** is proven with real-pixel leaks inside and beyond the ring, 27/27 each way, plus the end-to-end transplant.
  - **No-flip:** 150/150 on the locked lib; 138/138 after the supersede.
- **The ruling-4 review.** 415 not-yet-run rows were considered, and each disposition is listed. It caught three more clauses that would have falsely stopped 082-07h, before any leg ran. Good work.

## NEEDS-DECISION rulings
1. **G-BIND (3) global half < 32: ACCEPTED.** The halo reaches inside a half that sits 3 px from a 149-level change. Cross-binding, channel swap and a +32 shift are still caught.
   - **The declared cost:** a uniform shift below 32 now passes. To keep that cost visible, 082-07h computes this **read-only reading** from the bank (not a gate):
     - the global half's max |d| restricted to pixels **> 16 px** from the layer half (16 px is the banked wrong-copy halo extent);
     - its pixel count.

     A reading above 2 there is a finding chat will rule on at the evidence review.
2. **Locating the halves by a uniformity range below 16: ACCEPTED.** It is a locator, not a verdict check, and the measured separation (≤ 8 vs ≥ 156) is wide.
   - The 8-level gradient on the global half goes on the S2 cause-read list, with the halo and the start-up element.
3. **G-RD drift compared within the same engine-frame phase (mod 8), still 0 px: ACCEPTED.** The dither is a named, measured cause (G318), and all 203 same-phase pairs read 0. It is proven both ways, and a +1 drift and a +1 patch still fail.
4. **G0_FSYN re-runs first: ACCEPTED.** The census must be of the build under test, and the old census saw TC_Masked as opaque. The cost is about 35 s.

## Housekeeping
- **The noflip citation.** The next docs commit points at `noflip-pre-supersede-locked-lib.json` instead of the intermediate-lib file.
- **`082-07-common.py`'s `ARCHIVE` path** still names the `30FE0FDE` archive. Nothing reads it at run time, so fix it at the next harness change, not now; changing it would force a boundary re-issue.
- **082-07g ended at 67 % context,** so 082-07h starts a fresh default session.

## Next
- **082-07h (high, fresh session):** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07h 540` (143 legs), plus the owed in-engine proofs and the reading in ruling 1.
- **After it:** the Codex evidence review of all of S1 through the Relay.
