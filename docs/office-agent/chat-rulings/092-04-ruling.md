# 092-04 ruling: design round B for UV and normal corruption on virtual textures

Date: 2026-10-06 ~02:05 IST. Author: Claude Code Desktop (orchestrator).
- Report: `_reviews\092-04-codex-vt-transform-design.md` (run `_relay\runs\2026-10-06-092-04-vt-transform-design\`; Astra max, read-only, 2,779 s, exit 0).
- Nothing was built or changed.

**Disposition: ACCEPTED as design research.** It is not a work order. The route and subtype choices below are recommendations to the owner. Any UV/normal-on-VT build needs the owner's go-ahead, because it adds new client-visible subtypes (a compatibility-scope change).

## What I checked myself
- **`FUploadingVirtualTexture`** is declared in an Engine Private header with no `ENGINE_API` (`Engine/Private/VT/UploadingVirtualTexture.h:72`). So the stock cooked-data producer cannot be wrapped from a plugin [confirmed].
- **Material stacks take one producer handle per layer.** `MaterialShared.cpp:3733–3739` sets `ProducerHandle[LayerIndex]` and `ProducerLayerIndex = 0`, so a stack can mix a private producer for one layer with source producers for the others [confirmed].
- **The allocator** (`AllocatedVirtualTexture.cpp:339–353`) checks tile and border and derives a power-of-two mip bias per producer. Differing aspect ratios are cropped [confirmed].
- **The 1-LSB counterexample is real.** The BC4 integer decode in `BlockDXT.cpp:343–378` uses floor division: a=0, b=1, index 2 decodes to 0; the transformed block decodes to 254, not 255 [confirmed by hand]. The index remaps match my own derivation in the 092-01 ruling.
- **Proof A's gate** (`TexCorruptTree.cpp` @ `af79520`) refuses `stack_layout` whenever the source has more than one layer, the compiled stack has more than one layer, or the stack is preallocated. It is a plugin gate, not an engine limit [confirmed].

## Accepted findings
1. **One architecture for all four modes:** a plugin-owned transforming VT producer. Because the stock producer is not exported, it needs its own cooked-page reader (BulkData IO, tile codec expansion), transform and upload. It is bound per target through the Proof A private-texture pattern. **It is a new runtime page pipeline, not a small wrapper.**
2. **Tile:** same virtual size, source mip L+log2(N), page remap where whole pages align, and halo/tail reconstruction elsewhere. This keeps the shipped copy's mip law (the shipped tile copy deliberately samples source mip L+log2 N). The N×-larger variant is rejected (page-table growth, different magnification law).
3. **Scramble:** the shipped CRC/affine cell map on each source mip, with page/block fast paths for aligned interiors. Seams, fractional cells (K=3 etc.) and tails need reconstruction plus a runtime encoder. This is the largest piece of work.
4. **Normal:** BC5 endpoint swap plus index remap (complement R+G for invert, G only for green_flip). It is a byte-exact involution; decoded values are within 1 LSB per channel. Other formats (DXT5nm, BC7, uncompressed) need their own adapters and proofs.
5. **New subtypes** (`tile_vt_mipcopy`, `scramble_vt_cells`, `invert_vt_bc5`, `green_flip_vt_bc5`) until equivalence to the shipped subtypes is demonstrated. Keeping the shipped names would need an explicit owner/chat equivalence ruling.
6. **The mesh route is not selected.** Scramble is still not generic there, and the semantics differ for tile and normal.
7. **Multi-layer stacks are an engine-legal mix of producers.** For blurry, the narrow extension is one private Proof A VT per required layer, with readiness proven on the combined allocation's page table. Codex estimates several days to about a week.
8. **Coverage:** guaranteed gain is 0 until proven and measured. The +1,490 / +1,451 VT buckets are ceilings, and the selected producer does not recover the unnamed bucket (Proof C).

## Effort (Codex estimate, accepted as an order of magnitude)
- BC5 normal proof: 1–2 engineer-weeks, then 1–2 to harden.
- Tile: 1–2 more.
- Scramble: 2–4 more.
- The full family: about 5–10 engineer-weeks.

## Recommended order (for the owner)
0. **Measure before building the UV/normal producer.**
   - The office readings already ruled: the 092-01 refusal counts and the 092-03c census counts.
   - New VT diagnostics: stack-size histogram, components blocked only by `stack_layout`, and the cooked VT normal-format mix by component.
   - These decide whether the BC5 normal route and the producer family are worth 5–10 weeks on Bates.
1. **Multi-layer stack support for blurry `proxy_vt`.** This lifts the main limit of the proof we just accepted.
2. **The BC5 normal producer proof** (identity, then green_flip, then invert, with a mixed-producer stack), only after the owner's go-ahead on the new subtypes and the office numbers.
3. **Tile, then scramble,** on the same producer.
4. **Proof C** (unnamed bindings) separately, if the counts justify it.

Items 0 (diagnostics only) and 1 continue Proof A inside the owner's standing instruction to fix the Bates problem. They add no new subtype and change no shipped behaviour, so I am briefing them as 092-05 tonight on a new branch. Items 2–4 wait for the owner.

Chat review status: per AGENTS.md this is Codex's convention. Chat review NOT REQUIRED for this read-only report or for 092-05 (a bounded branch proof that extends an accepted route; nothing merged). REQUIRED (owner or chat) before any UV/normal VT implementation (new subtypes) and before any merge into a delivery line.
