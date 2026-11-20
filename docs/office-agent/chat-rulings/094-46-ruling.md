# 094-46 — Chat ruling: GN4 is done, and D3D11's three combined sessions pass H4 and the strict texture gates. camera_clipping's spans are exact, but changing the near plane itself adds ±2-level speckle. Measure that as a second, near-plane floor (option b), then finish D3D12, the default-only run and the push in 094-46r

**Date:** 2026-11-05, about 09:35 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-46:** NEEDS-DECISION. It ran 07:39–09:27 IST.
- `_reviews\094-46-codex-report.md` and `_reviews\094-46-code-review.md`.

## Rulings

### GO1. Accepted (local `eea2acd`)
- **GN4:** `label_transition_camera_clipping_unconfirmed_entries` is ported with 5.1's meaning. All 56 of 5.1's camera_clipping keys are now emitted.
- **The combined bench schedule** is accepted: the default 2,352-frame section, then six 48-frame near/far holds, 2,640 frames in all.
- **D3D11's floor:** two near-10 controls differ by at most 1 level, on at most 560,155 px. It was sealed before the positive run, and Code's reader agrees.
- **D3D11's D4, the parts that pass:**
  - exactly 3 spans, labelled everywhere they are recorded;
  - every labelled frame changes all 921,600 px;
  - both baseline controls label 0;
  - the near plane is restored.
- **D3D11's GN3 sessions:**
  - H4 passes (780 masks, `unavailable` 0);
  - texture IoU 1.000, with 0 spill, 0 of 140 edge frames changed and 0 of 35 reverts with residue;
  - G5 is clean;
  - no key was removed and no type changed.

### GO2. Option (b): seal a near-plane floor and judge against the larger of the two floors
- **What the data says:**
  - Two near-10 sessions never differ by 2 levels.
  - The positive session differs from both controls by up to 254 px at 2 levels, scattered over the lower 60 % of the frame, averaging about 0 and never in the same place twice.
  - So changing the near plane on its own changes the render slightly (most likely through depth precision feeding screen-space effects). That holds whether or not anything is clipped.
  - 5.1 holds the near plane for the whole session in the same way. The speckle is part of how camera_clipping renders, not a labelling error.
- **The rule:**
  1. Before the positive run, also seal a **near-plane floor**: a held near-100 session with **no near holds**, so the camera never comes within the slab and nothing can be clipped, paired against a near-10 control with the same seed.
     - Record its max |Δ| and its max count at that level, frame by frame.
     - Its labels must read 0 frames.
     - It needs one guarded bench switch (`-IAIBench`, EA_Core only) that keeps the near-wall holds out.
  2. **An unlabelled frame matches** if it stays within the **larger** of the session floor (GN2) and the near-plane floor.
  3. **Labelled frames** must still change far above both floors.
- **Not adopted:**
  - (a) would make camera_clipping unpassable on a real behaviour;
  - (c) is a fixed tolerance, where a measured floor is the stronger proof.

### GO3. GN3's default-only run: one run is enough
- The output keys don't depend on the RHI, so one default-only run (D3D12 1279×719) is compared against `c5d0919`'s outputs.
- It must show no key removed and no type changed, and the card's read commands must work on it.
- The D3D11 and D3D12 combined legs carry the same information-only key diff.

### GO4. 094-46r continues from `eea2acd`
- **D3D11:** the near-plane floor pair, then D4 judged against GO2.
- **D3D12 1280×720:** the session floor pair, the near-plane floor pair and the positive, then D4.
- **The other GN3 legs:** the D3D12 1280×720 and 1279×719 combined legs (H4 and strict texture G1–G3).
- **The default-only run** (GO3).
- **Then** Code's scan, push and card update.
- The D3D11 combined sessions already accepted in GO1 are reused, not re-run.

## For the owner
- 🟢 **Camera clipping clips exactly the right frames,** and the textures still pass perfectly alongside it.
- 🟡 **One wrinkle:** moving the camera's near plane, which is how this anomaly works (the same as in 5.1), makes a few hundred pixels flicker by 2 brightness levels. That's invisible to the eye and has nothing to do with clipping. We now measure that effect on its own and allow for exactly that much, nothing more. Then D3D12, the push and the card.
