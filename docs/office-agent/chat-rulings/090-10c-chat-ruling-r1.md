# 090-10c — Chat ruling (NEEDS-DECISION): the rework is accepted; m53 `88801bd` goes to the office for readings; four follow-ups in 090-10f, and the invisible-but-labelled events are a MUST-FIX

Date: 2026-10-02 ~06:20 IST · Author: chat Claude · Head m53 `88801bd` (code `dcd8d29`, exe `B3103E32`) · Report `_mailbox\to_chat\090-10c-lodbias-office-gates.report.md` (270 min)

## Accepted
- **Engine semantics:**
  - RHI mip 0 = cooked mip `AssetLODBias + MaxNumLODs − NumResidentLODs`;
  - per-texture, group, cinematic and budget bias change only residency;
  - only `r.MipMapLODBias` reaches the sampler;
  - the RT sampler has no bias;
  - streaming bias is 0 in the editor, floored outside it, and 0 under `UseAllMips`.
- **The design as built:**
  - (a) the resident-chain copy (`rt_mip_mismatch` / `copy_source_changed` 0 everywhere);
  - (b) effective streaming bias;
  - (c) `r.MipMapLODBias` matched: identity 0.35 vs null 0.34, against 17.9 / 3.4 uncompensated;
  - (d) the copy ceiling (halve to 512 px, `over_budget` as a note; it still blocks above the floor under rule 3);
  - (e) B at component granularity, with the event scope limiting mask, m26 and bbox;
  - (f) sub-reasons everywhere.
- **The all-reasons census** `IAI.TexCorrupt.Census allreasons`, plus `anomaly_refusal_counts.py --allreasons-check`. On office-like home content, uv eligibility goes **0 → 72/82** (fixture) and **0 → 33/343** (MainWorld, where 85 more are blocked only by Nanite, which the office doesn't use).
- **PIE proof:** V1–V5b 7/7 uv and normal corrupt, identity 7/7, edges 6–7/7 (the known PIE end+1 is handled by 090-10e). **Staged:** V4, V5a, V5b 7/7 with edges 7/7. **Can-fail:** `0a69ea3` refuses each variant with its sub-reason, 0 events.
- **The gate audit** under the four rules; bench-only checks behind `-IAIBench`. Pure 270/0; mutants fail; strict 0/0; lever audit PASS; archive `m53-0910c-B3103E32`.

## Rulings on the four items
1. **11 of 28 auto events on the 16×16 constant-colour chain controls labelled with NO visible change: MUST-FIX (rule 1, label vs picture).**
   - A uniform or near-uniform texture can't show a UV or normal corruption, so labelling it is a mislabel. Real content has these too (default whites, flat normals, masks).
   - **Fix in 090-10f:** refuse with a named reason (`texture_uniform`) when every required binding's resident source is uniform within a declared tolerance for the requested family (uv: no spatial variation; normal: flat normal). It is measured once per texture and cached, cheap (a GPU min/max reduction of the resident top mip, or equivalent), with no CPU readback stall on the game thread.
   - **Tests both ways:** the 11 controls are refused; real textures are still admitted. In PIE, the auto-pool on the office-like fixture gives **every labelled event visibly corrupt** (pixel rule), and the can-fail is `88801bd` (11 invisible).
   - Also declare any other "labelled but invisible" class the auto-pool surfaces.
2. **B's component-scope mask: prove it in-engine.** Author a **static, non-animated two-component fixture actor** (one component eligible, one not). In PIE: only the eligible component's pixels change; the mask, bbox and m26 cover only that component (pixel-checked); the label edges are 0 off; the can-fail is the `NoPartScope` lever.
3. **Staged per-texture / group / cinematic bias: cook it.**
   - Cook a fixture texture with `LODBias` baked in, and a cinematic-mip texture, plus a **cooked** device-profile texture-group bias. Prove staged 7/7 per variant, identity and edges, exactly as PIE.
   - **Fix the lever** so it refuses in a packaged build (G449), with corrected help text.
   - **If dbghelp still blocks a cook or stage,** report it and keep PIE plus the office census as the evidence (the office runs PIE).
4. **`texture_not_parameter` (~13 %) and `host_mid` (~7 %): design limits; out of scope for Monday** (ruling C of 090-10x). The office census will show their real share; the owner is told plainly. The follow-up drop routes them.

## The office, this morning (owner)
- Pull m53 **`88801bd`**; in PIE after Play:
  - `IAI.TexCorrupt.Census allreasons`, and read back the uv and normal tables (numbers only);
  - fire uv and normal by eye on a few objects;
  - for blurry: a blurry-only auto-capture, then `python tools\anomaly_refusal_counts.py "<log>"`.
- **Not for dataset use** until 090-10f (the invisible-label class and the PIE end+1).

## Queue
090-10e (running: PIE end-settle, census slice) → **090-10f** (items 1–3) → the extended smoke → a Codex re-check → merges.
