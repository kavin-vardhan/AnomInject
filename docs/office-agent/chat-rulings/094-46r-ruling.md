# 094-46r — Chat ruling: D4 bounds the level, not the pixel count (option c). Both RHIs have 0 unlabelled pixels above the measured 2-level floor, and the count shifts by about 15 px from one control pair to the next. camera_clipping passes D4 on D3D11 and D3D12. 094-46s finishes the 1279×719 leg and the key diff, then pushes

**Date:** 2026-11-05, about 11:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-46r:** NEEDS-DECISION. It ran 09:32–11:36 IST.
- `_reviews\094-46r-codex-report.md` and `_reviews\094-46r-code-review.md`.

## Rulings

### GP1. Accepted
- **The near-plane floors:** D3D11 2 levels / 255 px, D3D12 2 levels / 240 px. Both were sealed before judging, and both far-only sessions label 0.
- **The session floors:** 1 level on both RHIs.
- **D4's spans:** exact on both RHIs. 144 labelled frames each, every pixel changed; the baseline controls label 0; the near plane is restored.
- **The five new sessions** pass H4, strict texture G1–G3 and G5.
- **The `-IAICameraFarOnly` switch** is accepted: non-Shipping, `-IAIBench`, EA_Core only.
- **Codex's `compact` compression of its own receipt files** is accepted: they are rehashed unchanged, and it saves disk reserve. It may be used on our own receipts only.

### GP2. Option (c): an unlabelled frame matches if **no pixel exceeds the larger floor's level**
- **The evidence:**
  - the near-plane count is not a stable property. The same far-only session paired with the other control reads 254 px instead of 240, on both RHIs.
  - The positive's 2-level count sits *below* the floor pair's on average (median 157 against 172, p99 212 against 227).
  - On both RHIs, **0 unlabelled pixels are above 2 levels**, while every labelled frame changes all 921,600 pixels.
  - Level separates "clipped" from "not clipped" by orders of magnitude; count does not separate anything.
- **The rule from now on,** for camera_clipping's D4:
  - unlabelled frames have max |Δ| ≤ the larger floor's measured max level;
  - labelled frames change far above it;
  - the count is recorded as information, not as a gate.
- **Result: D3D11 passes and D3D12 passes** (0 pixels above 2 levels on 2,496 unlabelled frames each).
- *Why this isn't loosening:* the floor's level is measured, never assumed. Any real clip, or any spill of one, moves pixels far beyond 2 levels. The count bound was testing which control pair we happened to pick.

### GP3. 094-46s, from local `886e1a0`. No new D3D11 or D3D12 1280×720 sessions
1. Recompute D3D12 D4 under GP2 from the existing data.
2. The **D3D12 1279×719 combined leg:** H4, strict texture G1–G3, and D4 under GP2 (it shares D3D12's floors; if its own control is needed, run it).
3. **GO3's default-only run** (D3D12 1279×719) against `c5d0919`: no key removed, no type changed, and the card's read commands work.
4. **Code's scan, push and card.**
   - The card goes in `_reviews\094-46s-office-card-europa.md`.
   - It covers the four defaults, plus how to request camera_clipping for a held session.
   - Its size and client lines are readings only, with no 1280×720 pass/fail (the office read-back showed the old lines false-failing).

### GP4. Next: the build-ready job (094-47). The owner ranks the dashboard as the top priority after camera_clipping
- **What it covers:**
  - the office include-order fix (SceneRendering.h before SceneViewExtension.h in `AnomalyViewMetadata425.h`), folded into the branch;
  - **the control server ported to 4.25** (today `AnomalyControlServer.Build.cs` enables it only on 5.1), so the dashboard connects to a packaged **Development** build, lists the anomalies, and starts and stops a C5 capture;
  - packaging the Paris host as Development with the plugin: the 4.25 materials cooked, the private pass working cooked, and a capture run from the packaged exe;
  - Shipping stays excluded, as on 5.1.
- **Docs:** minimal, as the owner asked.
- **Its design:** Codex's read-only plan `_reviews\094-47-codex-controlserver-plan.md`, if the owner ran it.

## For the owner
- 🟢 **Camera clipping passes on D3D11 and D3D12.** Every clipped frame is fully different, and every "unclipped" frame stays inside the measured 2-level flicker that moving the near plane always causes. The pixel-count limit was just measuring which comparison run we picked, so I've dropped it.
- **Next (094-46s):** the last screen-size test, the "nothing removed" check, then push and a new office card that includes camera clipping.
- **After that:** the dashboard and packaged-build job.
