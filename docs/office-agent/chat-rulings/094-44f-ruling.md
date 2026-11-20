# 094-44f — Chat ruling: the −5.0 EV calibration is accepted. The Paris leg's camera wasn't looking at the targets (the sweep ran in editor mode, the legs in `-game`), so the bench locks its view and re-qualifies in `-game` before any fire. The kit stops calling an owned stop a crash. 094-45 runs after 15:15: the texture legs and push first, then camera_clipping

**Date:** 2026-11-04, about 12:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-44f:** NEEDS-DECISION. It ran 12:02–12:36 IST.
- `_reviews\094-44f-codex-report.md` and `_reviews\094-44f-code-review.md`.

## Rulings

### GL1. Accepted: GK2's calibration at −5.0 EV
- Codex and Code each derived −5.0 EV from the raw sweep, and all 40 brighter values fail.
- At −5.0 EV: no saturated target pixel, target means of 0.40–0.60, EA_LOD's pink passes on hue, and the checker gaps are 21–51 (Code's whole-silhouette reading gives 40).
- The settings are bench-only and runtime-only, and the Paris host is hash-equal.

### GL2. The bench locks its view, and re-qualifies in `-game`, before any fire
- **Likely cause, not proven:** the sweep ran as an editor session, while the leg ran as `-game` with the host's own pawn and camera, so the view differed. That's why the targets were projected off-screen.
- **The rule for every bench leg on EA_Core, applied by the bench helper only (`-IAIBench`, EA_Core):**
  1. Set the view explicitly to the level's `EA_Camera`, or to the sweep's recorded pose if no such actor exists, and hold it for the whole session.
  2. Before any preview, check that every target's centre is inside the frame and its drawn share is inside the 0.5–25 % band.
  3. Re-qualify GK2's checks at −5.0 EV on `-game` preview frames from the same session.
  4. If the view or the checks fail, re-apply once. If they fail again, stop the leg with NEEDS-DECISION and the numbers.
- **A `-game` sweep is allowed** if −5.0 EV fails in `-game`, under the same GK2 rules (brightest passing exposure).

### GL3. `runtime_exit.py`: an owned stop with a recorded intent is never a crash, even if its reason is empty
- Use GC6's intent record (UTC, PID, reason), with the same PID and the root still running.
- **Can-fail tests:**
  - an owned stop with an empty reason is classified `owned-stop`;
  - a real exit with no intent is still classified `crash`;
  - a stale intent from another PID doesn't count.
- 094-44f's receipt stays as it was recorded.

### GL4. Notes
- **C: dropped 2.6 GiB during the game session.** Record C:'s free space before and after every session from now on, and attribute any drop of 1 GiB or more if possible (shader or pipeline caches are the likely suspects). It is not a stop condition unless C: falls under 10 GiB.
- **EA_LOD's checker gap is thin on Codex's front-face patch** (21 against a floor of 20), but GL2's re-qualification decides it.

### GL5. 094-45 after 15:15 (the owner's blackout runs 13:45–15:15). One job in two gated stages
- **Stage 1, the textures:**
  1. GL3 first;
  2. then GL2's view lock and `-game` re-qualification;
  3. then the three Paris legs: H4, texture G1–G3 whole-frame at 0.98 with clean edges, and G5;
  4. if all three pass, Code scans and pushes, and writes the card (094-44f's scope).
- **Stage 2, camera_clipping:**
  - Runs only after Stage 1 is pushed.
  - Steps 7–8 of `_reviews\094-44-request.md`, under GE3's D4 and D5 and the Phase B plan.
  - Then its own scan, push and card update.
- A Stage 2 failure never un-pushes Stage 1.

## For the owner
- 🟢 **The brightness fix works.** Codex and Code both picked the same setting on their own.
- 🟡 **The Paris test's camera was pointing away from the test objects,** because game mode uses a different camera from the calibration. The bench now locks its camera onto the test objects and re-checks the brightness in game mode before running anything.
- **After 15:15:** Paris with the textures (and the push), then camera clipping, in one job.
