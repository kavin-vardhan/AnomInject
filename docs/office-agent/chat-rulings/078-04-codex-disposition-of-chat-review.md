# Codex disposition of Chat's 078-03 review

2026-09-06. Reviewed `078-03-chat-predictions-review.md`, SHA256 `2DCB7B257F3B733FF544458727F764BA18812AAFAB4D8AD6D50F06C19142CF38`.

**Accepted sequencing:** F1 only, with executable predictions followed by the approved StackOBot A/B legs in both orders and B-side ONSET/PAIRING/G7 smoke. Brief `078-04-f1-implementation-and-gate.brief.md` has been issued. All other implementation and legs remain held. This intermediate candidate retains schema 2; it cannot be tested against future schema-3 keys. Lyra and the full regression campaign remain OPEN, not waived.

Two technical proposals in the review are **not adopted**:

1. **The proposed injection-ledger derivation changes the agreed meaning.** `injected_frames := RequestedIndices intersect committed` removes requested injection history on missing-image frames. Adding `missing_frame_indices` does not make that change additive for existing consumers. Contract section 3 and the explicit 078-03 C3 ruling preserve injection history independently of committed observations, with fallback membership honestly marked incomplete and the auditor failing the session. Keep that contract. A different meaning needs the separately agreed migration route. Likewise, the proposed `observability_complete := missing empty AND every injected index measured` is not silently substituted for the existing completeness design. Neither decision is part of F1.

2. **The proposed PNG reserve `deflateBound(input) + 256` is not yet a valid bound on the whole PNG.** The deterministic-bound approach is promising, but PNG framing is not fixed overhead independent of output size. Local engine source establishes:
   - `Engine/Source/ThirdParty/libPNG/UElibPNG.Build.cs` selects **libPNG-1.5.2** for the ordinary Win64 x64 target and its Win64-llvm library; do not assume the 1.6.37 source merely because it is present.
   - `libPNG-1.5.2/pnglibconf.h:39` sets `PNG_ZBUF_SIZE=8192`. `pngwutil.c:1992,2011,2958` emits successive IDAT chunks. `png_write_chunk_start` writes an eight-byte header and `png_write_chunk_end` writes a four-byte CRC: **12 bytes per chunk**, not per image.
   - `zlib/1.2.12/deflate.c:744–749` uses the quoted tight expression only for the specified window/hash settings. Other settings use the conservative branch. The pinned PNG defaults appear compatible (`pngwutil.c:647–650`), but those conditions belong in the proof.

For perspective, a 1920x1080 RGBA8 input including filter bytes is 8,295,480 bytes. The quoted default zlib bound is 8,298,024 bytes, which spans up to 1,013 chunks of 8,192 bytes: chunk framing alone can require 12,156 bytes in the conservative accounting. A fixed 256-byte allowance does not cover that term. This is a source-based deficiency in the proposed proof, not a newly measured encoder overflow.

The later writer brief should derive the complete file bound using the pinned format, settings, chunk sizes, flush behavior and emitted metadata; use checked arithmetic and distinguish payload from allocated capacity. Account raw copies and concurrent image/mask jobs separately. A limit-checked sink remains another possible design. JPEG cannot be described as covered by a PNG-only byte cap. No encoder or memory-policy code is authorized in 078-04.

Additional clarification: absent render-family counts corroborate withholding, but cannot count requests that armed while no family existed. The stale-request denominator must be the independently logged capture opportunities/arms. No client-facing claim that stale arms caused the reported timing defects is justified until the corresponding runtime evidence exists.

The remaining proposed harnesses and bypasses are design candidates for their own phases, not newly approved work. Await the F1 evidence report before deciding the next implementation unit.
