# 094-32 — Chat ruling: C1 and C2 pass on stock 4.25. The plugin captures the game's own 10-bit picture, and the labels match it on every frame. Next: UI-free frames (5.1 parity) and C3 masks, tonight (094-33)

**Date:** 2026-11-01, about 02:15 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-32:** GREEN. It ran 00:37–01:52 IST.
- **Codex's report:** `_reviews\094-32-codex-report.md`.
- **Code's review:** `_reviews\094-32-code-review.md`. Code recounted everything independently.

## Verdict
**C1 PASS:**
- **PIE on EA_Core:** 60 frames, indices 0–59, no black frame. It matches the independent Phase A reference, with a mean error of 0.50 against a limit of 8.
- **`-game` on EA_Travel:** 60 frames (Game02). Game01's failure was a wrong reference image (EA_Core's white material), not a capture fault.
- **The format:** 18 (10-bit), on D3D11, with 0 frames dropped.
- **No backbuffer override on any argv** (FN2 holds).
- **The 10-bit conversion is exact,** checked four ways, including an exhaustive test of all 1,024 values.

**C2 PASS: labels are in sync.**
- **The count:** 480 labelled frames across both tick orders, with **zero mismatches and zero ambiguous frames**. Code's own pixel classifier, with no shared references, agrees.
- **Per anomaly:** 3 apply and 3 revert edges in each order, plus 6 blink toggles per order for blinking.
- **The pacing block** ran on frames 240–479.
- **All three one-frame shift negatives** (apply, revert, blink interior) fail as they should, in both readers.
- **The 12 frames after travelling to EA_Travel** are clean.

**This is the core of the port.** On stock 4.25 the plugin now produces frames and labels that agree frame for frame.

**Code's notes:**
- events came from a test driver, not the game's own firing path;
- the label exporter is shaped for this test;
- **the plugin's green heartbeat text is burned into every 4.25 frame.**

## Rulings

### FO1. C1 and C2 are accepted on stock 4.25
- **Europa's fork** is still the office's question: D-lite now, and a capture smoke later.
- **The real firing path** (auto-injection on visible targets) is revisited after C3.

### FO2. Frames must be free of UI and on-screen text, as on 5.1 (Code's note 1)
**The rule:**
- **5.1 captures through its SceneViewExtension path,** before UI. 4.25 reads the backbuffer, so frames carry the plugin's heartbeat and would carry any game UI.
- **On 4.25, move colour capture to the end of the view family's render**, before Slate, the canvas and on-screen debug text are drawn. Use `PostRenderViewFamily_RenderThread`, or whichever 4.25 hook you cite from source.

**The gate:**
- the heartbeat region is clean in every frame, against a control frame with the heartbeat visible;
- then re-run the C1 gate (both sessions) and the C2 gate on the new path.

**If 4.25 has no pre-UI point,** fall back:
- suppress on-screen debug messages and the plugin's heartbeat during capture;
- report that any game UI would remain, as NEEDS-DECISION;
- carry on to C3.

### FO3. C3 masks: per-anomaly metrics, frozen here (FL4), never lowered after results
**The scope:**
- **AnomalyShaders compiled in on 4.25** per the plan's §C3, with custom stencil per the plan;
- **a shader that fails to compile at boot is a crash,** handled under FI4.

**Reference silhouettes** come from the plugin's own stencil and custom-depth, on clean frames.

| Anomaly | Metric |
|---|---|
| blinking | drawn frames: IoU(mask, the target's silhouette) ≥ 0.90; hidden frames: IoU(mask, the counterfactual silhouette) ≥ 0.90 |
| missing_object | IoU(mask, the counterfactual silhouette from the last clean pre-apply frame) ≥ 0.90 |
| lod_corruption | IoU(mask, the target's silhouette in the same frame) ≥ 0.90, and ≥ 0.95 of the changed pixels (against the pre-apply clean frame) inside the mask dilated by 1 px |
| lighting_mismatch | ≥ 0.95 of the changed pixels inside the mask dilated by 1 px; mask pixels only on receiver actors (≤ 1 % outside their silhouettes dilated by 1 px) |
| all four | spill ≤ 1 % of mask pixels outside the reference region dilated by 1 px |

**The negatives:**
- a one-frame mask shift fails;
- a mask taken from the wrong target fails.

### FO4. Housekeeping (Code's note 3)
- `docs/gotchas.md` is append-only.
  - Restore the original 094-31b paragraph.
  - Append the correction as a new numbered entry, at the next free number.

### FO5. 094-33, now: FO2, then C3, in one job
- **FM1's build gate** and **FJ1's lean profile** apply.
- **If FO2's re-run of C1 or C2 fails,** stop before C3.
- **The window for real sessions:** until 12:00 IST on 11-01.

## For the owner
- **Nothing to do.**
- **Tonight:** making frames UI-free like on 5.1, then masks.
