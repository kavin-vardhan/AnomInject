# 086-01 — Chat ruling on the label-sync audit, part A: threshold, mask-ID limit, the unmeasured anomalies, and the m53 offset gate

Date: 2026-09-28 (IST) · Author: chat Claude · Branch `docs/label-sync-audit` `ca9c98f` · Findings `_reviews\086-01-label-sync-audit.md`

## Accepted
- **SYNC-PROVEN from pixels, 0 frames off:** blinking (307 events), missing_object (36, synthetic content only), missing_texture (25, thin) and corrupted_texture (130). Both tick orders, anti-aliasing on and off.
- **The checker is proven to fail:**
  - the +3 known answer on 12/12;
  - the null twin silent on 186/186;
  - 98/98 doctored labels fail;
  - it caught a flaw in its own first rule before reading any verdict.
  - m55 is an independent witness (248/248).
- **stuck_low_mip DESYNC** on master: it confirms 084-01 and the owner's reading. The fix is `B725678B`, and its proof is 084-03.
- **Circular gate history found in lod_popping (m30) and camera_clipping (m30),** as with m52. Chat approved those gates too; owned.

## Decisions (chat, technical)
1. **The release-gate visibility threshold is the majority (50 %) of the effect,** with the 10 % and strict counts always reported beside it.
   - Blinking's +1 at 10 % (4/592 edges, anti-aliasing on) is the picture catching up after an object returns: an 84–88 % visible object is correctly labelled "not missing". Extending the label would be the larger error.
   - **Add the per-frame key `transition_residual = 1`** on the first frame after a hidden object returns, so clients can drop it. It is small source work, in the next build.
2. **084-03 also runs the 086-01 checker on its legs,** as a second, independent verdict next to its own gates.
3. **m53:** the B-M53 offset pixel gate is adopted into the m53 delivery-cut plan (085-01). Per 085-00, m53 slips before sync does.
4. **Part B scheduling:**
   - **by day on Tue:** the part B harness (B-LOD, B-REAL, B-LYRA, and B-CC with its bench-only lever, built by day);
   - **Tue night:** the bench.
   - Tonight is 084-03 only; one bench at a time.

## For the owner (product)
- **The mask-ID limit:** a session with more than ~55 events loses target masks on the later events. Chat recommends **fixing it now** (recycle mask IDs once an event's frames are fully captured, proven by a >55-event leg). Long auto-injection captures are exactly what the client runs.
- **lod_popping and camera_clipping (UNMEASURED):**
  - B-LOD and B-CC are measured on the Tue-night bench.
  - Any that still fail or stay unmeasured on Thursday go to the owner's Thursday pull decision (his rule of 2026-09-28).
