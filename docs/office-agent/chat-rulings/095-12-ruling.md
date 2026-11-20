# 095-12 — Chat ruling: the m59 review follow-ups are ACCEPTED; three decisions ruled

**Date:** 2026-11-14, about 12:15 IST · **Author:** chat Claude (orchestrator)
**Input:** mailbox report 095-12 (GREEN, 38 min, $19.33 API-equivalent; session `57e05b8a`, xhigh). Light brief: code, Editor-only compiles, offline tests. Nothing launched, no Game build.

## What landed
- `fix/m59-forced-lod-refusals` = `d23ab16` (pushed, fast-forward from `af1b08f`).
- `integ/m56-m58` = `e15d06d` (pushed; true merge `85e90e6`, no conflicts, plus a status commit).
- **S10 `d8077fc`:** the single-LOD refusal counts only LODs the engine can draw (minimum LOD, component override, empty LODs excluded). Static and skinned, both LOD anomalies.
- **S9 `91a87c2`:** `lod_corruption` refuses a forced LOD that is not coarser than the one on screen (`not_coarser_than_current`); when the current LOD cannot be read it refuses `current_lod_undetermined`.
- **S3 `c5113e3`:** when only some meshes of an object are forced, the label box, mask, visibility count and asset/class name cover the forced meshes only.
- **S11 + N14 `2b4661c`:** the `IAI.Anomaly.LodAllowSkeletal` help, readme and log lines are corrected; the read-back shows the value in use and the value waiting.
- Docs `d23ab16`: gotcha **G581** (G580 is on `feat/m57`; next free is G582), journal `docs/sessions/2026-11-14-095-12-m59-review-follow-ups.md`.
- Tests: LOD selftest 141 checks, 13 mutants all fail; integration tip 18/18 pure selftests, label rule 94, pixel gate 101, kit 102, oracle 35, lever audit PASS (57 names). Editor compile 0 errors, 0 warnings.
- Predictions: `code-receipts-095-09\predictions-095-09.txt` AMENDMENT 2, dated before any leg. New legs L59-8 (S3), L59-9 (S9), L59-10 (S10), L59-11 (switch read-back).

## Accepted without change
- **`current_lod_undetermined` refuses.** Right direction: a label nobody can show to be true is the defect this unit removes.

## Decisions
- **D1. The old `lod_popping` timer on `fix/m59` stays as it is.** It still fires, with a warning, on a target already at its coarsest LOD. m57 replaces that timer and ships in the same delivery, so the path does not reach a client.
  - **Carry-over:** m57 keeps the timer as the bench-only `periodic` mode. That mode must refuse a not-coarser target the same way. Check it in 095-13's result; if it is missing, it goes in the next light brief.
  - **If m57 slips out of the delivery:** extend S9 to the timer on `fix/m59` before anything ships.
- **D2. Keep the node-name change; do not scope `node.bounds`.**
  - The name fix is in shared code, so a texture-corruption event with skipped parts now names a corrupted component. That is more accurate, and it stays.
  - `node.bounds` keeps covering the whole object. The readme already says so, the label box and mask are already scoped, and changing a delivered field's meaning for every texture event is not worth it inside this unit.
  - **Consequences, for the next light brief:** one readme changelog row (2.1 → 2.2 table) naming the change; one dated predictions amendment so the release-versus-integration label comparison allows this name change on texture events with skipped parts and nothing else new.
- **D3. The two missing fixtures are assigned to the next light brief,** as options of the existing fixture script, written and unit-checked without launching the editor:
  - `pair_cube` → `CB_LodPair` (the 2-LOD fixture mesh plus a 1-LOD cube on one object), for L59-8.
  - `minlod1` → `CB_LodFix_MinLod1` (the fixture mesh with minimum LOD 1), for L59-10.
  - 095-07 runs the script in the editor as its first step, before the cook, with `IAI_LODFIX_ADD=second_pair,instanced,pair_cube,minlod1`.

## State
- Host Editor DLLs match `e15d06d`'s Source. The Game exe is still the 24 Sep build and matches no current branch. No cook exists.
- E: 16.1 GiB free, D: 63.2 GiB. The heavy run (095-07) still needs about 85 GiB free on E:.

## Next
1. **095-13 (running since 12:04 IST, fresh session `e3188d21`, xhigh):** the m57 review fixes, with the owner's one-second view-entry hold.
2. **095-14 (light, to draft after 095-13 reports):** fixture options (D3), the readme row and predictions amendment (D2), the `periodic` guard if 095-13 did not cover it (D1), plus whatever 095-13 leaves.
3. **095-07 (heavy):** on the owner's word that E: has the space.
