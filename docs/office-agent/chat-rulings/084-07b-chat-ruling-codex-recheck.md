# 084-07b — Chat ruling on the readme/cook round and Codex's RED re-check of 084-07: all eight findings are fixed in one bounded round (084-07c); N1 blocks everything; the loop is bounded after this round

Date: 2026-09-29 ~04:30 IST · Author: chat Claude (owner offline, per his instruction) · Branch `fix/m52-label-timing` `46e5894` · Exe `FF41BFF3` + cook utoc `87A46A27` (archived, not staged)

## Accepted
- **The cook:** 60 s, 794 packages, 0 warnings. The descriptor is byte-identical to the branch, with `AnomalyBench` listed and excluded from Shipping. The content manifest (2,182 files) is identical before and after.
- **The readme:** §8.6, §8.6a, §8.6b and §8.7, including the training-label recipe.
- **`verify_capture.py --label-rule`:** 15 new cases; the existing selftests are unchanged; it reproduces 084-07's row-family counts exactly.
- **The packaging-checklist item** for G354.
- **G361 / G362.**
- **Codex's re-check** (`_reviews\084-07-codex-sync-recheck.md`, RED): F3 RESOLVED; the rest PARTIAL, with eight findings. All of them are **accepted**.

## Rulings
**N1 (high): FireWindow events are written `labelled: false` / `visible_positive: false`.** This covers `missing_texture` and `corrupted_texture` today, and m53's `uv_corruption` / `normal_corruption` after the merge.
- It is **in the data on every texture-anomaly frame, so it blocks everything.**
- **Fix:** `labelled` comes from **one function, the same authority that puts a frame into `annotation.json`'s frame list for that event, per policy class** (FireWindow → the event's affected frames; AnomalyState; ActorHidden; the m52 render-held window). Both writers call it.
- **Unit tests:** one per policy class, both ways. A broken variant that reads the activity bit must fail.
- **`verify_capture.py --label-rule`** must fail the `FF41BFF3` shape and pass the fix. It runs on every leg of the next bench.

**N2 (high): a planar mesh in the slab becomes a clean negative** (zero-length traces).
- **Rule:** a SAT-positive candidate may become negative **only** on valid, non-degenerate traces. When a clipped segment is shorter than the engine's trace minimum, use the full slab segment along that view ray.
- A candidate with fewer valid traces than the grid needs is **`camera_clipping_unconfirmed`**, never a miss.
- **Unit tests:** the zero-thickness wall must be positive, and the degenerate-only case must be flagged.

**N3 (high): a rotated instance under non-uniform component scale.** Use the **matrix product** the renderer uses (instance matrix × component matrix), not `FTransform` composition. Unit test from the engine formula, both ways.

**N4 (medium): unknown texture state.** Use three states: known-full, known-partial, and unresolved.
- An unresolved member never establishes a full-set boundary. The frame carries the additional reason `unresolved`.
- **The 084-07 decision-1 exemption covers record-backed partial frames only.** An off frame whose only backing is `unresolved` is a FAIL to investigate.
- An invalid held endpoint means unresolved, not `AtHeld`.

**N5 (medium): the 32-frame cap.** Store exact ranges (run-length), so every flagged frame is identifiable with no overflow.

**N6 (medium): detached tails across two run boundaries.** Carried tails must be carried again. Unit test with two boundaries.

**N7 (medium): a restored holder outside the census.** Verification runs against **every identity the retiring value was ever applied to**, kept until verified, not the current map. The test must use the production retirement path.

**N8 (medium): `IAI.Capture.Shot` keeps the old semantics.**
- Pass a fully sampled snapshot, so it follows the same rule.
- If that is disproportionate, the one-shot output must carry an explicit rule marker (`label_rule: legacy_shot`), and the readme says so.
- **No client-facing output may carry the old `visible_positive` meaning unmarked.**

## Bounding the review loop (delivery week)
After 084-07c, **only a HIGH finding on a path the bench exercises, or a finding in the data, blocks.** A medium finding on a path the bench shows never occurs becomes a documented limitation in the readme. This is the 082-era review-loop rule, applied.

## Next
- **084-07c (xhigh, fresh), released now:**
  - N1–N8;
  - rebuild;
  - **re-cook**, archived, not staged;
  - **the Codex re-check v2 of N1–N8 only**, in the background, collected and not acted on.
- **Then:** 085-01 (m53 DC plan). Its integration dry run uses the 084-07c head.
- **Then:** 084-08 (harness), which must run `verify_capture.py --label-rule` on every leg.
- **Then:** 087-01.
