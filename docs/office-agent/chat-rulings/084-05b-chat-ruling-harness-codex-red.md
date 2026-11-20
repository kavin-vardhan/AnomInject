# 084-05b — Chat ruling on the sync harness and Codex's RED source review: bench tonight on E9FF019A; transition 3/8; fix F2/F4/F5 (and investigate F3) tomorrow; F1 is a documented schema, not a defect; Lyra skipped tonight

Date: 2026-09-29 01:55 IST · Author: chat Claude (owner offline, per his instruction) · Branch `fix/m52-label-timing` `b3ba294` · Build under test `E9FF019A`

## Accepted
- **The 084-06 harness:**
  - edge-local thresholds, the null-divided ratio with cross-correlated lag (r = 0.9995), which beats the scene cycle;
  - the transition-aware gate;
  - **the added fade-at-label-end rule (G353)**, the added B5 null, the added second B-CC-MW null, and "ambiguous on a censored edge". All sound.
- **The proofs:** 26/26 offline and 55/55 on real captures, both ways. The MASK55 gate fails the pre-fix capture (560 unmasked labelled frames from event 55).
- **Bank re-evaluation:**
  - **B9_FIX (AA off) is 0/0 on 16/16;** the baseline fails;
  - under AA, with flags, B0_FIX and B7_FIX pass 16/16;
  - B0L fails as required;
  - the 086-01 second verdict agrees in direction.

## Decisions
1. **Run 084-06 TONIGHT on `E9FF019A`.**
   - None of tonight's legs exercises Codex's F2 (no scaled instanced meshes on the B-CC fixture), and F4 (aliasing) is exactly what the MASK55 gate checks.
   - Tonight gives the runtime evidence; the source fixes follow by day.
2. **Transition values: 3 / 8** (`IAI.Label.TransitionOnFrames 3`, `OffFrames 8`, `HideFrames 1`).
   - A missed transition frame is a mislabel, but an extra flagged frame only costs the client one uncertain frame. So erring wider is the safer error for the dataset.
   - The onset blind spot this creates is covered by the strict AA-off legs.
   - **Tonight:** pass `IAI.Label.TransitionOnFrames 3` in the fix legs' ExecCmds (the one allowed harness edit; show the diff).
   - **Tomorrow's build:** the source defaults become 3 / 8.
3. **Codex's findings:**
   - **F1 (hide anomalies keep `anomaly_present` on visible in-burst frames):** **not a defect under the schema.** `anomaly_present` means **the event is active on this frame**; per-frame visibility is `visible_positive` / `observable` / `transition`, and 086-01 proved blinking's hidden-phase edges 0 frames off. m52's rule stays: its active window *is* the render-held window.
     - **Action:** the client readme must state these field semantics exactly, per anomaly, before delivery.
   - **F2 (camera_clipping misses scaled instanced meshes): FIX tomorrow.** Per-instance transforms including scale and rotation, with a unit test both ways.
   - **F3 (the sync capture path writes a different event set from what the annotation counts; inherited): INVESTIGATE tomorrow,** and fix if it affects delivered labels. It's label consistency, so it's in scope.
   - **F4 (mask-ID retirement skips custom-depth-off holders, so a recycled ID can alias): FIX tomorrow.** Retirement must clear every holder, with a unit test both ways. Tonight's MASK55 gate reports whether it happens in practice.
   - **F5 (transition history lost across a run boundary): FIX tomorrow,** with a unit test.
4. **B-LYRA: SKIPPED tonight.** The plugin move and LyraEditor rebuild aren't worth the risk at 02:00. Realistic content comes from B-REAL tonight and the owner's environment-pack project later.

## Next
- **084-06 (high, fresh session):** tonight's bench.
- **Tuesday by day, 084-07 (xhigh):** F2, F4, F5, the F3 investigation, the defaults at 3/8, and the readme field semantics; rebuild; a Codex re-check of those fixes only. Then the **m53 delivery cut** starts in parallel planning.
