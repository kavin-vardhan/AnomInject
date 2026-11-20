# 084-08b — Chat ruling on the LOD fixture (GREEN): accepted

Date: 2026-09-29 ~08:05 IST · Author: chat Claude (owner offline) · Branch `fix/m52-label-timing` `37bb750` · Pairing exe `8A6074AA` + utoc `BF06AE61` (archived, not staged)

## Accepted
- **The fixture `CB_LodFixture`:**
  - two targets, orange sphere LOD0 → blue cube LOD1, with distinct per-LOD materials;
  - variant D has Dithered LOD Transition on, variant N has it off;
  - the declared LOD rule is met unchanged (ratio 20, coverage 26.96 %, LOD0 natural).
- **The cook:** the manifest diff is exactly 7 added files under `Content\AnomalyFixtures\`; the descriptor is byte-identical.
- **The honest "prediction 6 partly missed" report:** MainWorld.umap and the shader archives vary between every cook (G371), and one of 1,039 shader chunks was regrouped. Accepted as cook variation, noted.
- **The harness:**
  - 10 new fixture legs, each variant's gate gated on its own premise;
  - fewer than 16 judged events reads INSUFFICIENT, never PASS;
  - the multi-frame fade is reported on D and never excused;
  - the dry-only premise forcing is refused live;
  - dry run 47/47;
  - about 87–100 min.
- **G372** (forcing a LOD disables dithering in engine source) is recorded as a source reading; the bench decides.

## Next
- 084-10 (release hygiene on the fix branch), released now.
- Tonight's 084-09 now runs about 87–100 min.
