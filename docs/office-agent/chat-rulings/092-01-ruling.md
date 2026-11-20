# 092-01 ruling: Codex research on virtual textures on Deimos (first office host)

Date: 2026-10-05. Author: Claude Code Desktop (orchestrator). Report: `_reviews\092-01-codex-deimos-vt-research.md` (run `_relay\runs\2026-10-05-092-01-deimos-vt-research\`; Astra, max effort, read-only, 2,115 s, exit 0). I read the full report and spot-checked the citations listed below. Today's delivery (`release/2026-10-05` @ `d7eee97`) is untouched.

**Disposition: ACCEPTED as research.** It is not a work order. Any build waits for the owner's go-ahead and a brief.

## Accepted findings (my independent checks in brackets)
1. **The VT refusal is substantive, not caution.**
   - A non-VT resource on a VT stack layer logs "Material expects texture to be Virtual" and the layer is skipped (`MaterialShared.cpp:3711–3725`) [checked].
   - A VT parameter requires a virtual texture (`MaterialInstance.cpp:1280`) [checked].
   - **Correction to my earlier statement to the owner:** the result is a warning plus a skipped layer, not a crash or garbage. The swap silently fails to render, which is still a label/picture disagreement (rule 1).
2. **The blurry hold cannot work on VT.** UE ignores cinematic mips for VT (`TextureLODSettings.cpp:171–174`) [checked]. `GetNumResidentMips()` is meaningless for VT, so held-telemetry must not be reused.
3. **Strongest narrow route, blurry on VT:**
   - A plugin-owned VT resource built with the exported `FVirtualTexture2DResource(owner, VTData, FirstMipToUse)` [checked: `TextureResource.h:165`, `ENGINE_API`]. The stock uploading producer adds that offset to every page request [checked: `UploadingVirtualTexture.cpp:64`, `130`].
   - A private genuine VT bound only on the target gives per-object blur. It reuses cooked tiles and does not touch the host source or its other users.
   - Limit: it never drops below one tile (`Texture2D.cpp:1247–1250`) [checked]. It is "proxy blur on VT", with its own route label, and not a hold.
4. **The tangent recipe in my brief was wrong. Codex's correction is accepted.**
   - `invert` = negate tangent X and keep the handedness sign.
   - `green_flip` = flip the handedness sign only.
   - Negating both, as my brief said, flips X only.
   - Equivalence to the texture-side modes holds only for simple tangent-space materials. On complex graphs the route needs its own subtype (e.g. `tangent_basis_flip`).
5. **Mesh-side UV×N is a restricted tile.** Different UV channels, panners/offsets and world or procedural coordinates differ, so it needs its own subtype (e.g. `uv_channel_tile`) unless equivalence is proven. Mesh-side scramble is not a generic option. Mesh routes also carry real costs: cooked CPU access or GPU readback, every LOD, `SetStaticMesh` side effects (physics recreation, navigation), ISM whole-component scope, lighting identity, and mask integration for any replacement component.
6. **The hold does not need texture parameters.** It collects textures through `GetUsedTextures` (`Anomaly_StuckLowMip.cpp:265–279`) [checked]. Deimos's blurry `texture_not_parameter` counts (635 + 392) therefore belong to the proxy route. The census's four route counts (`hold_eligible`, `proxy_eligible`, `auto_hold`, `auto_proxy`) are needed before sizing.
7. **The constants wrapper (private material resource with indexed references) needs proof.** It is not yet feasible-in-production.
8. **The five zero-fire PIE captures are unexplained.** The census proves 59/59/48 eligible across all loaded objects, not in view. The pool/modes may not have been enabled: UV, normal and blurry all ship OFF by default.

## Coverage ceilings
These are ceilings on the identified buckets only. Hidden later gates can lower them; zero gain is guaranteed.
- VT bucket:
  - UV: +1,490
  - Normal: +1,451
  - Blurry: +594
- Unnamed-binding bucket:
  - UV: +343
  - Normal: +346
  - Blurry: +1,027

## Orchestrator additions for the next design round
INFERRED, not verified; for Codex to confirm or refute:
- **Tile on VT as a page remap.** For N a power of two, mip L of the N×N-tiled texture is N×N copies of source mip L+log2(N). A wrapping producer could forward each page request to the source producer with remapped level and address, using compressed pages directly with no decode. Wrap borders and the mip tail need checking.
- **Scramble on VT.** At mips where a cell spans whole pages, scramble is a page permutation. Below that it is a 4×4 compressed-block permutation inside a page (cells ≥ 4 px). Borders need care.
- **Normal invert on BC5 VT pages.** Invert may be an exact compressed-block transform: per-channel 255−v with an endpoint swap and index remap to keep the BC4 mode. That avoids decode/encode.
- If these hold, a single "transforming VT producer" covers tile, scramble and invert with the shipped subtype meanings, and is likely cheaper than the mesh route.

## Recommended order
For the owner's decision; one implementer at a time.
0. **Office readings first.** This is cheap and settles the zero-fire question. For each of the five captures, run `anomaly_refusal_counts.py` on that capture's log interval. Also run `IAI.TexCorrupt.Census` (no argument) at the capture view, including the blurry route counts. Numbers only.
1. **Proof A:** blurry private coarse VT resource, starting with an identity control. Rough 2–4 days. Home fixture: the Home-B rock textures (`Content/Effects/Textures/Rocks/ultkedqjw_4K_*`), which are genuine runtime VTs.
2. **Design round B (read-only Codex):** the page-remap and block-transform producer for tile, scramble and invert, against the mesh route. Pick one before any UV/normal build.
3. **Proof C:** the constants wrapper, only if the office route counts show the unnamed bucket matters for the routes we keep.

Chat review status: per AGENTS.md this is Codex's convention. The route and subtype design is a compatibility-scope change, so it needs the owner's or chat's ruling before implementation. This read-only ruling needed none.
