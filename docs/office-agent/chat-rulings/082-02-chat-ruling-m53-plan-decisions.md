# 082-02 — Chat ruling on the m53 plan (082-01, NEEDS-DECISION): route B accepted pending Codex's design review; D1–D7 ruled

Date: 2026-09-26 (IST) · Author: chat Claude (orchestrator) · Branch `feat/m53-uv-normal-corruption` `946c1bf` · Plan `docs/predictions/2026-09-26-m53-uv-normal-corruption.md`

## What the plan found
The shape agreed on 2026-09-22 (a material takeover that rebuilds a stand-in material from the host's textures) **changes more than the corruption**:
- it loses tints, world-aligned and triplanar mapping, and packed masks;
- it has no cooked API to tell which texture is the base colour;
- it risks first-draw PSO hitches and usage-flag default-material swaps (the `G49`/`G157` class).

For training data, that means a label that says "UV broken" while the picture also changes its colour and mapping.

## Rulings
**D1: route B, provisionally.** The host material is kept, and a corrupted copy of each of its texture parameters is drawn into a render target at Apply and bound through a MID.
- This matches the owner's standing product default, **label purity over yield** (m52).
- `G-ID` (the identity round-trip) is the first, falsifiable gate. If it fails beyond the null-twin tolerance, fall back to route A as specified.
- **It is final only after Codex's design review** (next checkpoint).

**Chat addition to D1: snapshot residency.** A render-target snapshot taken from a partly streamed texture would bake blur into the corruption. That would be a second, unlabelled anomaly: the `stuck_low_mip` look.
- At Apply, the source must be resident at its full top mip. Force residency with a bounded wait (frames and ms declared in the plan).
- Otherwise refuse with `not_fully_resident`.
- Record an additive `texcorrupt.snapshot_mip` key (0 = full).
- `G-ID` runs at full source resolution.

**D2: accepted.** `uv_scramble` replaces the TexCoord[1] lerp. Route A's `channel1` survives only if the fallback is taken.

**D3: accepted.** Two ids, `uv_corruption` and `normal_corruption`.

**D4: accepted as the interim state.** Both ids go in the pool, **default OFF**. Default-on is the owner's product call once `G7` (yield) and `G-STR` (strength) are measured; chat will bring him the numbers.

**D5: accepted provisionally.** UV defaults are `tile`, `scramble`, `drift`; normal defaults are `invert`, `green_flip`, `noise`. They are re-ruled after `G-STR`. `swap` and `flat` stay off by default.

**D6: accepted.** The new materials go in the m47 prewarm list only; the per-frame pending count is untouched, so the G298 widening is avoided.

**D7: accepted, with a visibility key.** Cloning a slot that already holds a runtime MID is allowed.
- Record an additive `texcorrupt.host_mid_cloned: true` on those events, so the frozen-host-animation limit is visible per event in the data.
- The existing `IAI.Anomaly.TexCorruptCloneHostMids` switch stays available to refuse such slots.

## Next
1. **Codex design review** of the plan plus these rulings. The owner pastes `_reviews/082-02-codex-m53-design-review-request.md`.
2. **Plan revision** by Code, folding in Codex's findings and the residency rule.
3. **S1**: route B core, the identity mode, `G-ID`.

No implementation starts before step 2 is accepted.
