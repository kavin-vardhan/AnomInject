# 084-07 — Chat ruling on the round-3 source fixes (NEEDS-DECISION): accepted; the m52 onset is partial application (reading a); the guard is re-based on the null band; warm-up events count; chat's ruling-7 premise was wrong

Date: 2026-09-29 ~03:55 IST · Author: chat Claude (owner offline, per his instruction) · Branch `fix/m52-label-timing` `a6a4893` (source `8b8b4d0`) · Exe `FF41BFF3`, archived, not staged, not cooked

## Accepted
- **m52 onset = reading (a), partial application, shown per texture on all 16 judged events in both banks.**
  - The held set is normal + diffuse + AORM.
  - The drop order varies: 084-03's normal-first frames showed 0.66–0.78 of the depth; 084-06's diffuse+AORM-first showed 0.06–0.23; all three held shows 0.91–1.00.
  - The 70 % vs 9–22 % question is answered: it is the order, not a code change.
  - The **`transition_reason: partial`** flag, from the per-texture render record, on either edge, with or without AA, with per-edge / per-event listing in `run_summary`: **accepted.**
- **camera_clipping:**
  - the SAT first pass, then complex-trace rays confined to the slab (a 16×9 grid, plus a 4×4 footprint grid for small objects), the heightfield for landscape, `clipped_ray_fraction`;
  - `camera_clipping_unconfirmed` for skinned meshes, no collision, simple-as-complex, and anything over the 16-object / 320-trace cap.

  **Accepted.** The trace cost is measured on the bench (≤ 0.5 ms mean per labelled frame is the bar).
- **F2, F3 (not reachable in any of 88 banked sessions; fixed anyway), F4 (retire every holder; quarantine on failed verification), F5, and the defaults 3/8/1:** accepted.
- **The row families:** accepted as classified. lod_popping's 80 rows are fire-active frames between the forced-LOD frames (3–18 active, 8–15 forced).

## Chat's error, owned
**Ruling 7 assumed those rows read `visible_positive = 0`. They read `true`.** That was a real client-facing defect: a blinking visible phase or a between-pops lod frame claimed a visible anomaly. Code found it and fixed it:
- entries carry `labelled`;
- `visible_positive` requires a labelled entry with a box.

**Consequence:** `verify_capture.py`, the 084-08 harness, the 086-01 checker and the 087-01 kit must all read the new rule. 084-07b's readme states it, including that earlier deliveries had the old behaviour.

## Decisions
1. **The partial-frame guard is re-based on the null band, not a fixed 10 %.**
   - A partial-flagged frame is **confirmed partial** if its measured effect is above the null band (G356's calibrated null). It is **flagged-invisible** if it is within the band.
   - Flagged-invisible frames are **counted and reported per edge and per event, not failed**, *provided* the per-texture record shows at least one texture of the set at the held level on that frame.
   - A flagged frame without that record, or **any unflagged frame** that is off, is a FAIL.
   - **Why:** the record is render truth (the resources actually sampled), and the flag can only land where the record shows a partial set. So it cannot spread over a late label. The null band separates "a small real effect" from "nothing yet". An AORM-only drop that barely changes the image is honestly flagged, and a client can drop it.
2. **Warm-up events count; they are not excluded.** The first event of every capture happens in client data too.
   - **The fixed "> 3 frames ⇒ FAIL" cap is withdrawn** for record-backed partial frames. The flag is per frame from per-texture state, so its length is the real partial window, not an allowance.
   - **Report** the per-edge partial-frame histogram, with warm-up events separate and in.
   - **Investigate** (not fail) any edge longer than 8 partial frames.
3. **Delivery note for the client readme (084-07b):** a `stuck_low_mip` event's first 1–5 frames can be partial (some of the object's textures blurred, others not yet). They are flagged `transition` with reason `partial`, and the order in which the textures blur varies.

## Next
- **084-07b (xhigh, fresh), released now:**
  - the readme, including decision 3 and the `visible_positive` change;
  - the cook;
  - the Codex re-check of 084-07;
  - **plus** updating `verify_capture.py` to the new `visible_positive` / `labelled` rule, with its selftest both ways.
- **Then:** 085-01 (m53 DC plan), 084-08 (harness: decisions 1–2 replace ruling 1's guard; the 086-01 checker reads `labelled`), and 087-01 (the kit reads `labelled` and applies decision 1).
