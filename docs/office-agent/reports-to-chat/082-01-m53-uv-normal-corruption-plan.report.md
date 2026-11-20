# REPORT for 082-01-m53-uv-normal-corruption-plan.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-01-m53-uv-normal-corruption-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-26 14:47:27 | finished: 2026-09-26 15:16:47 | duration: 29 min | exit code: 0
context at last turn: 693942 tokens (69% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-01-m53-uv-normal-corruption-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. The plan is written, pushed, and answers questions 1–8, but it finds that the agreed takeover design doesn't do what we want. Choosing the route (`D1`) blocks all implementation.

## 2. Plain summary for the owner

- **What the anomaly will look like.** One object's surface looks broken while everything else in the frame stays normal:
  - UV modes: its texture tiles too finely, slides, gets scrambled into patches, or is transposed.
  - Normal modes: its surface lighting is inverted, flattened, noisy, or has its vertical component flipped.
- **The problem with the agreed design.** The "takeover" copies only the object's textures. It loses everything else the object's material does: colour tints, world-aligned or triplanar mapping, packed masks. On most of our test content the object would change material as well as getting the UV/normal bug, so the client's model could learn "the colour changed" instead of "the texture is broken".
- **What I recommend instead.** Keep the object's own material and corrupt only its textures. The plugin draws a corrupted copy of each texture once, when the anomaly fires, and the object uses that copy. The picture stays identical except for the corruption.
- **Risky.** This alternative has more moving parts, and its "draw a faithful copy" step is derived from source, not measured. Stage 1's first gate (`G-ID`) tests exactly that; if it fails, we fall back to the agreed takeover. Some modes (flat normals, U↔V swap) may be invisible on plain textures.

## 3. Answers to 1–8 (engine paths relative to `Engine/Source`, UE 5.1.1)

**1. Texture discovery**
- `GetUsedTextures` works in a cooked build and returns the textures after instance overrides (`Material.cpp:1069-1158`, `MaterialInstance.cpp:1053-1119`).
- It **returns an empty list with no warning when no shader map is loaded** (`MaterialShared.cpp:929-935`). An empty result is refused as `no_textures`, and the log names both possible causes.
- No cooked API says which texture feeds BaseColor or Normal. That graph is editor-only (`MaterialInterface.h:766-784`), and sampler types aren't stored at runtime.
- The runtime-safe fields are sRGB, compression, LOD group and virtual-texture status. `IsNormalMap()` (`CompressionSettings == TC_Normalmap`, `Texture.h:1720-1724`) decides base vs normal; name heuristics only break ties.
- Virtual textures are skipped, because binding across the virtual/non-virtual boundary renders **black** in a cooked build (`MaterialShared.cpp:3711-3752`, `Texture2D.cpp:1335-1337`).
- Textures smaller than 64 px are skipped as constants. The plan has a closed list of 14 refusal reasons.

**2. Material design**
- **Route B (recommended):** two corruptor materials that draw into render targets, `M_CorruptTex_UV` and `M_CorruptTex_Normal`. They must be **Surface-domain Unlit**, because a UI-domain material has no shaders in a cooked build (`LocalVertexFactory.cpp:280-289`). No usage flags are needed.
- The draw call works in cooked builds and never blocks the game thread (`KismetRenderingLibrary.cpp:152-217`).
- **An opaque draw writes alpha = 0** (`BasePassPixelShader.usf:1916`). The UV corruptor therefore uses an AlphaComposite blend after a clear. That is derived, not measured; `G-ID` proves it, and a fallback that needs no re-cook exists.
- Normal maps are written into the red and green channels. `Compat.UseDXT5NormalMaps` is read at runtime, and a host with it on is refused.
- **Route A (the agreed takeover):** specified in full as the fallback. It needs to be single-sided and carry all 7 usage flags. **`UStaticMesh::GetNumUVChannels` returns 0 in a cooked build** (`StaticMesh.cpp:5150-5162`), so the TexCoord[1] mode must check `GetNumTexCoords` instead.
- **First-use hitch:** PSO precaching is off in 5.1 (`PipelineStateCache.cpp:103-110`). Route A would stall on the first labelled frame; route B stalls once, off-screen, when the anomaly fires.
- **m47 readiness:** add the new materials to the prewarm list only (`D6`), because G298's run-wide marking would otherwise widen.
- **Cook:** one cook. The materials are hard-referenced the way the magenta is (G45); the editor target is rebuilt first (G47); presence is proven by loading them at runtime (G48).

**3. Takeover and restore**
- Corruption is per material slot, so a glass slot on the same mesh stays glass.
- One material instance serves all slots of a target that share a source material. Nothing is shared across events or actors.
- **A material instance cannot have another runtime instance as its parent; it renders the default material** (`MaterialInstance.cpp:3076-3083`). A slot that already holds one is cloned with `CopyParameterOverrides` (`MaterialInstanceDynamic.cpp:484-503`).
- Restore mirrors the existing m17 contract: re-find the live component, restore only slots still holding our instance, sweep for leftovers, log the counts, release the render targets.
- The target is watched for destruction through `OnEndPlay`, as `stuck_low_mip` does.
- New refusal `held_by_stuck_low_mip`, so a render-target copy never bakes in a blurred texture.

**4. Visibility evidence**
- m55 carries no verdict and may not feed labels or selection, so there is no floor based on measured change.
- Chosen handling for weak modes:
  - a per-mode set for the auto-pool, with `swap` and `flat` off by default;
  - floors that need no pixel read;
  - a declared `texcorrupt.strength_class` label key (`strong` / `medium` / `weak`), not a measurement.
- Predicted strong: tile, scramble, invert. Medium: green-flip, noise, drift (weak in a single still). Weak: swap, flat.
- `G-STR` measures each mode against twins at the same pose and re-rules the defaults.

**5. Gates and fixtures**
- `CB_GateLevel` refuses everything: its material has no textures.
- Arbiters: StackOBot `SM_rock`, and Lyra `Cube4`.
- Gates run are `G0`, `G-ID`, `G1`–`G11`, `G-STR`, `G-COST`, `G-LYRA` and route-A-only `G-FID`, with both tick orders where alignment is asserted.
- `G-ID` is self-contained on MainWorld, because the `null_effect` twin only runs on the two bench maps.
- The can-fail lever is `IAI.Bench.TexCorruptNoApply`.

**6. Auto-injection**
- Two ids: `uv_corruption` and `normal_corruption`. Pool keys go from 7 to 9.
- One extra seeded draw picks the mode, and only when an m53 id is drawn.
- 🔻 **G150 is imprecise.** A pool member that isn't enabled doesn't change the draw (`AnomalyAutoInjectorSubsystem.cpp:241-252`), so shipping both ids off by default keeps old recorded runs comparable.

**7. Cost and risk**
- About 5.6 MB per corrupted texture at 1024 px, capped at 64 MiB per event.
- `drift` redraws every frame. Frame cost and the first-use stall are measured by `G-COST`; no number is predicted.
- The plan lists ten named risks (§8.2).

**8. Stages** — see 6 below.

## 4. Where the source contradicts the agreed shape

| Agreed | What the source says |
|---|---|
| Reuse the takeover | It rebuilds the material rather than corrupting it (§1). |
| Base colour = "the sRGB texture" | No API identifies it. On StackOBot's modular kit the only sRGB candidates are a sand overlay and a 1-pixel constant. |
| Opaque, single-sided only | Only needed by the takeover. Under it, the hero Bot and Lyra's characters are refused. |
| Lerp to TexCoord[1] | On a single-UV mesh it returns UV0 (invisible) or zero on Nanite. It also can't be done on a texture copy, so route B replaces it with `uv_scramble`. |
| Materials authored two-sided like the magenta | For route A that would reveal back faces the host hides. |
| Mesh-level UV rewrite rejected | **Confirmed.** CPU vertex data isn't kept in cooked builds. |

## 5. Owner's product calls

- **Pool:** add both ids to the pool but **not** default-on. Flip `uv_corruption` on after the yield (`G7`) and strength (`G-STR`) numbers. The m52 precedent was "on if at least 25 % of visible targets are eligible" — a future decision rule, not a gate. Keep `normal_corruption` off until content with detailed normal maps shows strength.
- **Default modes:** UV `tile scramble drift`; normal `invert green_flip noise`.
- **Route:** B. This is the owner's call too, because it changes the agreed shape.

## 6. Stages and estimates

- **S1:** route B core with the identity mode only. 2 builds and 1 cook. **Stops if `G-ID` fails** and falls back to the takeover. If only the alpha part fails, a parameter switch covers it with no re-cook.
- **S2:** all eight modes, both ids. 2 builds, no cook.
- **S3:** measurement (`G-STR`, `G7`, `G-COST`, `G-LYRA`), then the pool and mode ruling.
- **S4:** docs, comment strip, merge.
- **Total:** about 4 implementation sessions, 1 cook (2 only on fallback), and roughly 8 builds.

## 7. Commits pushed

- `946c1bf` on the new branch `feat/m53-uv-normal-corruption` (off `master` `4283fc8`), pushed, and origin matches. It adds the plan (`docs/predictions/2026-09-26-m53-uv-normal-corruption.md`), the journal (`docs/sessions/2026-09-26-082-01-code-m53-plan.md`) and a status entry in `CLAUDE.md`. No tag.
- I removed the scratch worktree. The `m51` checkout is untouched at `53bf725`, `master` is unchanged, and nothing was killed.
- The content-scan script is outside the repo at `C:\ClaudeTemp\m53scan\scan.py`; its method is in the plan's Appendix A.

## 8. NEEDS-DECISION

- **D1** — route B (recommended) or route A. **Blocks S1.**
- **D2** — drop the TexCoord[1] mode for `uv_scramble` (recommended), or keep it as a route-A-only mode.
- **D3** — two ids rather than one (recommended: two).
- **D4** — in the pool but not default-on (recommended).
- **D5** — the default mode sets above.
- **D6** — new materials go in the m47 prewarm only, not the per-frame pending count.
- **D7** — allow cloning slots that already hold a runtime material instance, accepting that the host's own later parameter animation freezes during the event (recommended).

Several connectors (Gmail, Calendar and a number of plugin servers) need authorising in claude.ai settings or via `/mcp`. The PDF viewer failed to connect. None of them were needed for this brief.
api-equivalent cost (not billed on Max): $49.12 | turns: 132 | running total across all briefs: $2,444.25
