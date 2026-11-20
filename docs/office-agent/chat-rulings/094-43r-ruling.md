# 094-43r — Chat ruling: GREEN. FZ2 is in. The outline leak is gone (0 new magenta on 2,352 frames), the labels are identical to the old method, Paris passes on D3D11 and D3D12, `93613ae` is pushed, and the office card is written. Phase B (094-44) starts now

**Date:** 2026-11-04, about 05:05 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Mailbox report 094-43r:** GREEN. It ran 02:23–04:54 IST.
- `_reviews\094-43r-codex-report.md`, `_reviews\094-43r-code-review.md`, `_reviews\094-43r-prepush-scan.md` and `_reviews\094-43-office-card-europa.md`.

## Rulings

### GF1. Accepted: GE1, Stage A, Stage B, H1–H5 and the push
- **GE1** (`d2f481d`):
  - the ref policy self-test passes 8 of 8, with the four mutants failing;
  - the full P9 `main()` passes at entry and at the tip.
- **Stage A** (spike `42ea37e`):
  - GD3's tolerance is 4/(2²⁴−1), committed before any launch, and 0 pixels break the rule;
  - A-P3 reaches IoU 1.0 everywhere, A-P4 finds 0 new magenta (the same reader finds it on 474 of 588 legacy frames), and A-P2 and A-P5 hold.
- **Stage B** (`ea08258`, `59c1282`, recorded in `93613ae`):
  - every remaining custom-depth write is behind `Legacy()`, with three bench-only conditions;
  - capture without the private pass refuses outright, which is stricter than FZ3 and is accepted.
- **H1–H5:**
  - H1: 0 new magenta;
  - H2: IoU 1.0, with 222 non-empty hidden-blink frames;
  - H3: clean;
  - H4: passes on D3D11 1280×720, D3D12 1280×720 and D3D12 1279×719, with 0 dropped frames, 69 events and 0 `unavailable`;
  - H5: the GPU adds about 0.16 ms.
- **The push:** `feat/europa-port` `d1cdc7b..93613ae`, fast-forward, confirmed with `ls-remote`.
- **The scan** is clean. The carried item, the owner's Windows profile name in build warnings, is already public on origin and accepted as before.

### GF2. The office card `_reviews\094-43-office-card-europa.md` (commit `93613ae`) replaces 094-40's
- At the office it fetches and checks out `93613ae`, with blinking + lod_corruption by default.
- It reads size, RHI, client, `polluted`, tagged and `unavailable` as numbers only, under the HARD RULE.
- **The office re-test no longer blocks home work.** It runs on the owner's next office day.

### GF3. Notes, with no action now
- **The backbuffer:** pixel format 18 is `PF_A2B10G10R10` (10-bit) on both RHIs, so the report's table is right and Code's mid-run line was wrong. The capture handles it, as H4 shows.
- **D3D12 render-thread submit is about 1.6 ms per captured frame,** against 0.16 ms on D3D11. Acceptable for an offline capture, and FUTURE for optimizing.
- **Gotcha numbering:** G524 was reused, so Codex added G527. The next free number is G528.

### GF4. Live coverage gap: instanced, hierarchical-instanced and skeletal meshes have only unit tests
- 094-44 adds them live to the outline fixture: GE3's SkeletalCube, plus one instanced static mesh actor with several instances.
- Each is gated the same way: H2's IoU and H1's no-leak, on the fixture.

### GF5. Stage 094-44 now (Phase B)
- **Its design:** the request `_reviews\094-44-request.md`, built on GE3/GE4 and the Phase B plan, plus GF4.
- **The py27 notes from Code:**
  - drop `used_with_nanite`;
  - replace the shading-model check with whatever 4.25 exposes, verified on readback.

## For the owner
- 🟢 **The outline leak is fixed and pushed.** With the game's outline effect on, zero outline pixels appear on labelled objects in all 2,352 test frames, and the labels are identical to before. Paris passes on D3D11 and D3D12.
- 🟢 **New office card:** `_reviews\094-43-office-card-europa.md`. Use it on your next office day. It defaults to blinking + LOD and reads numbers only.
- 🟢 **The new anomalies start now (094-44):** missing_texture, corrupted_texture and camera_clipping, as in 5.1.
