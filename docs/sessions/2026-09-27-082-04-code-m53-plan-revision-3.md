# 082-04 — m53 plan revision 3 for chat's N1–N4 / Δ1–Δ3 ruling, the Relay pin, and Codex delta check #2

**2026-09-27 · Code (Opus 5.5), headless · branch `feat/m53-uv-normal-corruption`, parent `b118a66`, edited in
the scratch worktree `E:\IA_BuildCache\_082_04_m53_wt`.**
**Plan only.** No source change, no build, no cook, no editor or game launch, no bench leg, no CaptureBench
edit, no tag. The `m51` main checkout, `master`, the tags, the cooked containers, `ToCodex\` and `E:\AmmaYT`
were not touched.

## Goal

Brief 082-04:

1. Pin the GDP Relay to Codex `gpt-6-astra` at effort `max`, passed explicitly.
2. Amend the m53 plan for chat's ruling `_reviews/082-04-chat-ruling-m53-revision-n1-n4-delta.md` (N1 = (a),
   N2, N3, N4, and Codex's Δ1–Δ3 and six PARTIALs), with a resolution row per item.
3. Run Codex's delta check #2 through the Relay, and do not act on it.

## What was done

- **Relay (outside any repo, `D:\IntrusiveAnomalies\_relay\`).** The runner's defaults are now `-Model
  gpt-6-astra` and `-Effort max`; both are always passed (`-m`, `-c`), an empty value is refused, and
  `run-meta.txt` records the requested pair beside the rollout's. The README says so.
- **Engine reads (UE 5.1.1), read-only.** Three parallel source reads plus spot checks:
  - the render-target, canvas and copy path (mip count, the deferred update, the canvas pair, RDG copies,
    RHI copy semantics, transitions, module exports);
  - streaming state, runtime LOD bias in a cooked build, the RHI mip layout of a streamed texture on D3D12,
    and explicit-mip material sampling;
  - the cook's texture-format selection on Windows, runtime mesh-asset materials, MID outers, the IoStore
    listing, and the bench's level tools, cook map list and container archives.
- **Offline arithmetic:** the budget with per-mip scratch, and each wrong-copy lever's minimum error on the
  fixture constructions.
- **The plan, revision 3**, amended in place with a splice script (outside the repo); every changed paragraph
  carries 🔁 082-04, and §R0.0 is the new resolution table with this file's line ranges.

## Findings of this session

- **A `UTextureRenderTarget2D` holds more than one mip only when `bAutoGenerateMips` is set, and then the
  full chain** (`TextureRenderTarget2D.cpp:50-62`). `DrawMaterialToRenderTarget` regenerates mips 1…N on such
  a target (`KismetRenderingLibrary.cpp:213-214`), and so would the target's first deferred update. The
  per-mip design therefore draws through `BeginDrawCanvasToRenderTarget` / `EndDrawCanvasToRenderTarget`,
  which never regenerates, and flushes each target's deferred update once, on the empty target, at
  allocation.
- **The material translator adds `View.MaterialTextureMipBias` to an explicit-mip sample** unless the
  expression's `AutomaticViewMipBias` is off (`HLSLMaterialTranslator.cpp:6145-6148`). The corruptors turn it
  off.
- **On D3D12 a streamed texture's RHI texture holds only its resident mips**, so "RHI mip i is cooked mip i"
  needs full residency and `AssetLODBias` 0. That is what P1-3's `snapshot_mip == 0` rule requires anyway; a
  render-thread tripwire makes a broken premise loud.
- **In 5.1 only `TC_Normalmap` produces BC5**, so the "BC5 as data" row had no fixture; it is refused.
- **Scratch changes the budget arithmetic**: a 4096² map needs 106.67 MiB with scratch (fits 128 MiB); drawing
  mip 0 straight into the output keeps scratch at about a quarter of the chain.
- **The ruled NoApply allocates**, so it cannot be the null for the collateral-residency diagnostic; a
  no-allocation null is specified (interpretation I1).

## Deviations

None from the brief's hard rules. Five interpretations made inside the ruling are listed for chat in the
plan's §R15 (I1–I5).

## State

Revision 3 committed and pushed on `feat/m53-uv-normal-corruption`. Codex's delta check #2 runs through the
Relay after this commit, against it; the report lands at `_reviews/082-04-codex-m53-delta2.md`. Code does not
act on it.

## Hand-off

Chat reviews revision 3 with delta check #2 and decides whether S1 starts.
