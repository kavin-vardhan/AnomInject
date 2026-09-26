# m53 — UV / normal-map texture corruption — PRE-DECLARED DESIGN AND GATES

**Written 2026-09-26, session 082-01, branch `feat/m53-uv-normal-corruption` off `master` `4283fc8`.**
**PLAN ONLY. No plugin source changed, no build, no cook, no game or editor launch, no bench leg, no tag.**
The only thing executed was a read-only, offline name-table scan of the StackOBot and Lyra content
trees (Appendix A). It opens no engine and touches no binary. The script lives outside the repo, and
its method is recorded here so it can be re-run.

Engine read against the canonical engine `D:\UESource\UnrealEngine`, **UE 5.1.1** (`Engine/Build/Build.version`).
Every engine claim carries `path:line`, relative to `Engine/Source` unless it is marked `[Shaders]`
(= `Engine/Shaders`). Path abbreviations:

- `Private/…`, `Classes/…` and `Public/…` mean `Runtime/Engine/Private/…` etc.
- A bare engine file name (`Material.cpp`, `MaterialShared.cpp`, `MaterialInstance.cpp`, …) lives
  under `Runtime/Engine/Private/Materials/`, or `Runtime/Engine/Private/` for texture and component
  files.
- Other modules are named explicitly (`RHI/…`, `Renderer/…`, `RenderCore/…`, `Developer/…`).

Every plugin claim carries `file:line` against `master` `4283fc8`.

Anything measured later that contradicts this file is a **finding**, and the finding wins. It is
recorded as an amendment, never folded in silently.

---

## 0. What this file is for, and the shape it starts from

m53 is the owner's next client anomaly class: **UV corruption** and **normal-map corruption**. Today
texture corruption covers only `missing_texture` (lit checker) and `corrupted_texture` (lit magenta).
Both replace the object's whole material with a constant look (`Anomaly_CorruptedTexture.cpp:112-124`).

**The shape agreed with the owner on 2026-09-22, quoted as the starting design:**

- Reuse the `corrupted_texture` material takeover, with two plugin materials, `M_CorruptedUV` and
  `M_CorruptedNormal`.
- Pull the host's own textures from the target's material with `GetUsedTextures`: the base colour is
  the sRGB texture, the normal map is the `TC_Normalmap` one. Create one MID per target.
- UV modes: tile, drift, U↔V swap, lerp to TexCoord[1]. Normal modes: green-channel flip, invert,
  drop (flat), noise.
- Modes are written as additive label keys, with no schema break. One cook covers both materials.
- Targets are opaque and single-sided only; everything else is refused.
- Mesh-level UV rewrite is rejected, because render data is shared and CPU vertex data is stripped.

This file answers the eight questions in brief 082-01 against source. **It also reports one finding
that the source and the content contradict in the agreed shape (§1).** That finding is the first
NEEDS-DECISION and it decides which of the two designs below is built. Both designs are specified in
full, so the ruling can be made on this file alone.

---

## 1. 🚨 THE HEADLINE FINDING — the takeover does not corrupt the host's material, it REPLACES it with a reconstruction

### 1.1 What the takeover actually draws

A takeover MID has **our** material as its parent. The MID reuses the parent's shader map and never
compiles (`Private/Materials/MaterialInstance.cpp:1695-1696`, `:238-244`, `:2122-2124`). So every
pixel of the target is shaded by **our** graph. All the takeover can copy from the host is the texture
**objects** it is handed. It cannot copy the host's material **graph**, because in a cooked build the
graph is stripped:

- `GetTexturesInPropertyChain`, the only API that says which texture feeds BaseColor or Normal, is
  `#if WITH_EDITOR` (`Classes/Materials/MaterialInterface.h:766-784`,
  `Private/Materials/Material.cpp:5647-5687`).
- The expression graph lives in `UMaterialEditorOnlyData` (`Classes/Materials/Material.h:296`,
  `:304-305`, `:319-320`, `:381-382`), which is stripped from cooked builds.
- **Sampler type is not stored anywhere at runtime** (`Public/MaterialTypes.h:393-438`;
  `MaterialUniformExpressions.h:100-103`).

⇒ Whatever the host graph did with its textures is **lost**: tint and colour parameters, vertex colour,
world-aligned or triplanar projection, packed ORM, detail layers, masks, parallax, material-function
logic. Our graph samples the two textures on UV0 with fixed lighting constants.

### 1.2 How often that matters — the content evidence (Appendix A; EVIDENCE, not a measurement)

| fixture | target family (measured target assets, m52 §3.2/3.3) | host material chain | what the takeover would lose |
|---|---|---|---|
| StackOBot MainWorld | 6 modular targets (`SM_Ramp`, `SM_SpawnPad_Base`, `SM_Modules_Platform`, `SM_Elevator`, `SM_Fan_Frame`, `SM_Modular_WallDoor`) + `SM_PressurePlate_Frame`, `SM_GratIng` | `MI_Carbon/Metal/Plastic/Glow` → `M_TileMaster` → `ML_Base` → **`MF_WorldSpaceUV`**, **`WorldAlignedTexture`**, vertex colour, RVT | **World-space projection, tint parameters, vertex-colour blending.** The only sRGB-class candidates the scan reaches are `T_SandTileabe_BC` and `T_WhitePixel`, so "the sRGB texture" is either a sand overlay or a 1-pixel constant. |
| StackOBot MainWorld | `SM_rock`, `SM_rock_02` (measured non-Nanite targets: m52 080-03 banked `target_pixels` ~16k–25k) | `MI_rock_0x` → `M_Master_kit_detail_Kit_rock` | Detail normal, AORM pack, **`WorldAlignedTexture`** detail. Closest to "texture on UV0" of anything on the fixture. |
| StackOBot MainWorld | `SM_FloorBase` (the most-fired measured target, m52 080-03) | `MI_GridObjects` → `M_Grid` | **Only texture is a mask (`T_Grid_A`).** No base colour, no normal. The takeover has nothing to pull. |
| StackOBot MainWorld | `SKM_Bot`, `SM_Bush`, `SM_Tree`, `SM_GenericPlane` | `BLEND_Masked` + `TwoSided` / translucent | Refused by the agreed opaque-single-sided rule. **The hero character is not eligible.** |
| Lyra `L_ShooterGym` | `Cube*` (m52 080-04's Lyra targets, `T_Paint_Normal`) | `MI_MS_*` → **`M_MS_Surface_Material_TriPlanar`** → `MF_WorldGrid`, `WorldAlignedTextureMip` | **Triplanar projection and the colour tint** (Blue/Orange/Red instances share `T_Paint_Diffuse`). |
| Lyra | `SM_Rifle`, `SM_Pistol`, `SM_Shotgun`, `SM_grenade` | `MI_Weapon_*` → `M_Weapon` | AORM + mask packs. Closest to "texture on UV0". These meshes carry the Nanite signature (CLAUDE.md 072 block), so the mask cannot measure them (`G134`). |
| Lyra | `SKM_Manny`, `SKM_Quinn` | `MI_*` → `M_Mannequin` (`BLEND_Masked` token, clear coat) | Refused by the agreed opaque rule. |

⚠ **Limits of the scan, stated with it** (the `m52_texture_sharing_scan.py` weaknesses, Appendix A):

- It reads asset **defaults**, not what a placed component overrides.
- A token in a name table is evidence of a reference, not proof of what a shader samples.
- 🔻 Two of the scan's own classifiers were **proven blind**:
  - `VirtualTextureStreaming` appears in 127 of 134 StackOBot texture name tables, but m52 measured only
    **2 of 188** loaded textures as virtual at runtime (080-04 §5).
  - The `SRGB` token is present on every texture, **including `T_rock_01_D`, a diffuse map**.
  - Both are registry tag names, not serialised values. **VT status and sRGB can only be read at
    runtime**, and the plan reads them there (§2).
  - The normal-map classifier is not blind: `TC_Normalmap` / normal LOD groups and the `_N` / `_Normal`
    name heuristic agree on **31 of 32** StackOBot normal maps. One texture was flagged by only one of
    the two signals.

### 1.3 Why this is a finding and not a cosmetic

**(a) The label claims a UV (or normal) bug, and the pixels show a material replacement plus that bug.**
On the modular kit and the Lyra gym cubes, the reconstruction alone changes colour, projection and
specular response. Only then does the named mode add its own change.

**(b) It is a shortcut feature for the client's model.** A detector trained on these positives can
learn "the object's colour/material changed" instead of "the texture is stretched / the lighting is
inside-out". It would then **miss** a real UV bug, where the colour is unchanged. The failure runs in
the direction the dataset exists to prevent.

**(c) The weak modes would be dominated by the reconstruction.** `normal_flat` and a small `drift`
change little by themselves. m55 would measure mostly the reconstruction, and the number would be read
as the mode.

**(d) The agreed "opaque, single-sided only" rule is a consequence of the takeover, not a property of
the anomaly.** It is needed because our material would otherwise change the target's blend mode and
culling. It removes the StackOBot hero (`SKM_Bot`) and the Lyra characters from the eligible set.

### 1.4 The alternative the source supports — HOST-PRESERVING, TEXTURE-SPACE corruption (route **B**)

Keep the host's material. Corrupt only the **texture** it samples.

1. For each eligible slot, create a MID whose parent is **the slot's own material**, not ours.
   - A MID can parent a `UMaterial` or a `UMaterialInstanceConstant`. **A MID cannot parent another
     MID — it renders the default material** (`MaterialInstance.cpp:3076-3083`, `:779-789`).
   - A slot that already holds a runtime MID (the StackOBot Bot does, `G46`) is therefore **cloned**:
     `UMaterialInstanceDynamic::Create(HostMID->Parent, Outer)` + `CopyParameterOverrides(HostMID)`
     (`Private/Materials/MaterialInstanceDynamic.cpp:484-503`, cooked-available).
2. Find which texture **parameters** of that material resolve to the textures the slot actually uses.
   - `GetAllTextureParameterInfo` + `GetTextureParameterValue` both work in a cooked build
     (`MaterialInterface.h:568`, `:802`; `MaterialInterface.cpp:1007-1010`, `:828-837`).
   - The result is intersected with `GetUsedTextures`, because the parameter list includes switched-off
     parameters (`MaterialCachedData.cpp:263-306` vs `Material.cpp:1113-1119`).
3. For each texture to corrupt, draw a plugin **corruptor** material into a `UTextureRenderTarget2D`
   **once, at Apply**.
   - The corruptor samples the host texture with the mode's transform: tile / swap / scramble / drift
     for UV, green-flip / invert / flat / noise for normal. §3.2 has the details and citations.
4. Set the MID's texture parameter to the render target and put the MID on the slot.
   - The host graph then shades the object exactly as before: its tint, its projection, its ORM, its
     blend mode, its two-sidedness, its usage flags, its Nanite path.
   - **Only the texels the graph reads are corrupted.**

What route B buys, each point against source:

- **Fidelity.** Everything but the corrupted texture is the host's. **The null twin becomes a real
  null**: an identity corruptor reproduces the original texture, up to 8-bit re-quantisation. That is a
  gate that can pass or fail (§6 `G-ID`), which the takeover cannot offer.
- **No new scene shaders or PSOs.** A MID uses its parent's shader map (`MaterialInstance.cpp:1695-1696`).
  The PSO is keyed by shaders and render state, and both are the host's. In a cooked 5.1 build PSO
  precaching is **off** (`RHI/Private/PipelineStateCache.cpp:103-110`, `:2131-2139`), so every *new*
  material × vertex-factory pair builds its PSO on first draw (`:2067-2094`).
  - Under the takeover that is one first-draw hitch per VF family per session, in the labelled
    window's first frame.
  - Under B the only new PSO is the corruptor's, drawn off-scene into a render target at Apply.
- **No usage-flag hazard in the scene.** Usage flags are read from the root `UMaterial`
  (`MaterialInstance.cpp:1432-1438`); a missing one swaps in the default material in a cooked game
  (`Material.cpp:1792-1819`). That is the defect class that already bit this project on Concorde
  (`G49`, `G157`, `set_material_usage_flags.py:15-38`). Under B the host material is already drawing
  that component, so its flags are correct by construction.
- **Blend mode and two-sidedness are preserved**, so the opaque-single-sided restriction is
  unnecessary. B refuses only translucent-only targets, which the picker already excludes
  (`AnomalyViewport.cpp:690-718`).
- **A UV mode corrupts every map coherently.** Base colour, normal and ORM all shift together, which is
  what a real UV bug looks like. The takeover can only move the two maps it reconstructs.

What route B costs, each point stated rather than discovered:

- **A texture must be a PARAMETER to be overridable.** A host graph that samples a hard-coded texture
  cannot be touched; that texture is refused (`texture_not_parameter`). Content evidence:
  - StackOBot's rocks, modular kit and Lyra's weapons and gym cubes are all MIC chains, which override
    parameters (Appendix A).
  - `M_Rock` on `SM_RockFlats_*` is a bare `UMaterial`, and its parameters are not established offline.
- **`uv_channel1` (lerp to TexCoord[1]) cannot be expressed in texture space.** It is a mesh-UV
  property. It is replaced by **`uv_scramble`**: a K×K cell permutation of the texture, the
  texture-space analogue of "wrong UV islands". The takeover's `channel1` is kept on the table only as
  an optional route-A mode (§10 `D2`).
- **Render-target memory:** one RGBA8 target + mips per corrupted texture (§8).
- **Snapshot resolution:** a render target is drawn at Apply from the texture's then-resident top mip,
  so a camera that later approaches sees the snapshot's resolution (§3.4, §8).
- **Nanite override materials:** a MID whose parent has a Nanite override is replaced by that override
  on Nanite meshes, and the MID's parameters are ignored (`Components/StaticMeshComponent.cpp:2670-2675`,
  `MaterialInstance.cpp:1748-1758`). Refused `nanite_override`.
- **Frozen host animation:** a cloned host MID stops following later parameter animation the game
  writes to its own MID (a hit flash, for example) during the event. Recorded as a named limit;
  `IAI.Anomaly.TexCorruptCloneHostMids` can refuse such slots instead.
- **More moving parts than the takeover:** render targets, their formats, their release, and a
  per-frame redraw for `drift`.

### 1.5 Recommendation (`D1`, chat and owner rule)

**Build route B for both families. Keep the agreed shape's intent — pull the host's own textures, two
plugin materials, one cook, one MID per target, additive keys — and change only where the corruption
happens.** Specifically:

- The two plugin materials become **render-target corruptors**, `M_CorruptTex_UV` and
  `M_CorruptTex_Normal`, instead of scene materials.
- The opaque-single-sided rule relaxes to "not translucent".
- `uv_channel1` becomes `uv_scramble`.

Stage 1 (§9) is built so the decision stays falsifiable. Its first gate is `G-ID`, the identity
round-trip on the fixtures. **If route B cannot reproduce the host picture within the null twin's
band, Stage 1 stops, and route A (the takeover, specified in full in §3.1) is the fallback.** Route A
then carries the reconstruction disclosure of §5.4.

---

## 2. Q1 — Texture discovery

### 2.1 How reliable `GetUsedTextures` is in a cooked build

| question | answer | source |
|---|---|---|
| Data source | The compiled **uniform expression set** of the active quality and feature level's shader map. Each slot is resolved through the leaf instance, so MIC and MID overrides are honoured. | `Material.cpp:1069-1158` (`:1103`, `:1113-1119`); `MaterialInstance.cpp:1053-1119` (`:1110`, `:1034`); `MaterialShared.cpp:926-941` |
| Cooked? | **Yes.** `UniformExpressionSet` is a plain `LAYOUT_FIELD`, its neighbours are the editor-only ones, and shader maps are serialised inline in cooked packages. | `MaterialShared.h:732` vs `:738-751`; `Material.cpp:720-797` |
| Parameter override chain | Followed. `GetTextureParameterValue` → `UMaterialInstance::GetParameterValue` walks `FMaterialInheritanceChain` and reads each instance's `TextureParameterValues`. | `MaterialUniformExpressions.cpp:1477-1483`; `MaterialInterface.cpp:828-837`; `MaterialInstance.cpp:935-978` |
| Other quality / feature levels | Only what was cooked **and** loaded for the running platform (`r.DiscardUnusedQuality`, active QL only). | `Material.cpp:841`, `:848-852`, `:2300-2312` |
| Static-switch-off branches | **Excluded** — only compiled chunks register uniforms. | `MaterialExpressions.cpp:9324-9332`, `HLSLMaterialTranslator.cpp:3333-3374` |
| Material functions and layers | Included if compiled; layer indices are remapped per instance level. | `HLSLMaterialTranslator.cpp:6785-6797`; `MaterialInstance.cpp:973` |
| Streaming virtual textures | Returned (the `Virtual` bucket). Tell them apart with `IsCurrentlyVirtualTextured()` (`Texture2D.cpp:1210-1224`). | `Material.cpp:1113-1116`; `Texture2D.cpp:1002-1009` |
| Runtime virtual textures | **Never returned** — `URuntimeVirtualTexture` is not a `UTexture`. | `Classes/VT/RuntimeVirtualTexture.h:15`; `MaterialUniformExpressions.h:314-337` |
| No shader map loaded | 🚨 **Returns an EMPTY list with no warning.** An empty list is therefore NOT evidence that the material has no textures. | `MaterialShared.cpp:929-935`; `Material.cpp:1073` (server-only is empty too) |
| Streaming state | Irrelevant to *which* textures are returned. Residency matters only to what a render-target snapshot captures (§3.4). | — |

**Consequences built into the design:**

- The per-component call is `UPrimitiveComponent::GetUsedTextures(Out, EMaterialQualityLevel::Num)`,
  exactly as m52 uses it (`Anomaly_StuckLowMip.cpp:101-116`).
- Per slot, the call is `Slot->GetUsedTextures(Out, Num, true, GMaxRHIFeatureLevel, false)`.
- An empty result is refused as **`no_textures`**, and the log line names **both** readings: "the
  material has none" and "no shader map is loaded for it". The two cannot be told apart at runtime.
  This is `G119`'s rule applied to a silent zero.

### 2.2 Which runtime texture fields can be read in a cooked build

`SRGB` (`Classes/Engine/Texture.h:1327`), `CompressionSettings` (`:1298`), `LODGroup` (`:1310`),
`VirtualTextureStreaming` (`:1357-1359`), `IsCurrentlyVirtualTextured()` (`Texture2D.h:302`),
`AddressX/Y` (`Texture2D.h:55-61`), `GetPixelFormat()` (`Texture2D.h:150`), `GetSizeX()` (`:147`),
`GetNumResidentMips()` (`:141`). **None is inside `WITH_EDITORONLY_DATA`.**

- `UTexture::IsNormalMap()` is runtime-safe: `CompressionSettings == TC_Normalmap`
  (`Texture.h:1720-1724`).
- ⚠ **The pixel format is not a normal-map test.** BC5 vs DXT5n is a cook-time choice
  (`Texture.cpp:3233-3236`, inside `#if WITH_EDITOR` 3005-3339).
- ⚠ **No cooked API says which texture feeds BaseColor or Normal** (§1.1). The closest runtime fact is
  `UMaterial::IsPropertyConnected(MP_Normal / MP_BaseColor)` (`Material.h:1739`, `Material.cpp:3961-3964`).
  It says whether the input is connected, not what feeds it. The plan uses it as a **guard** only: a
  normal-family fire on a material whose Normal input is unconnected is refused `normal_unconnected`,
  because a `TC_Normalmap` texture there is being used for something else.

### 2.3 Classifying each used texture

Applied per texture, in this order. The first rule that matches decides.

| # | test (runtime fields only) | class | effect |
|---|---|---|---|
| 1 | not a `UTexture2D` (cube, array, volume, render target) | `unsupported_type` | skipped |
| 2 | `IsCurrentlyVirtualTextured()` | `virtual_texture` | **skipped, never touched.** Binding a non-VT render target to a VT parameter renders **black**, with one warning; the reverse renders black with none (`MaterialShared.cpp:3711-3726`, `MaterialUniformExpressions.cpp:1230-1239`, `Texture2D.cpp:1335-1337`) |
| 3 | `LODGroup` ∈ {UI, Lightmap, Shadowmap, Terrain_Heightmap, Terrain_Weightmap, Bokeh} | `excluded_group` | skipped — the exact list m52 uses (`Anomaly_StuckLowMip.cpp:51-65`) |
| 4 | `GetSizeX() < 64` or `GetSizeY() < 64` | `trivial_texture` | skipped. A 1×1..32×32 texture (`T_WhitePixel`, `127grey`, `BlackPlaceholder`, `T_white`/`T_black`) is a **constant**, and no UV transform of a constant changes a pixel. Pre-declared as a design floor, not a calibrated threshold. |
| 5 | `CompressionSettings` ∈ {HDR, HDRCompressed, Alpha, DistanceFieldFont, Displacementmap, VectorDisplacementmap, EditorIcon} | `unsupported_format` | skipped — encoding any of these through an 8-bit target would change more than the mode (§3.2) |
| 6 | `IsNormalMap()` | **`normal`** | normal family: corrupted; UV family: transformed coherently |
| 7 | `SRGB` | **`colour`** | UV family: transformed |
| 8 | otherwise (`TC_Masks`, `TC_Grayscale`, linear default) | **`data`** | UV family: transformed (ORM / masks move with the UVs); normal family: untouched |

**Base vs normal is decided by `IsNormalMap()` first. Name heuristics (`_N`, `_Normal`, `_NRM`) are a
FALLBACK ONLY**, used solely to break ties inside the normal class. They never admit a texture the
flag rejects.

- On StackOBot the scan found flag and name disagreeing on two textures, one in each direction
  (32 by flag, 32 by name, 31 by both). One is `T_GratingTileable_Opacity`, flagged by a normal token
  and not by name (Appendix A). That is why the flag decides and the name only breaks ties.
- **In route B the base-vs-normal distinction matters only for the normal family and for the
  render-target encoding** (§3.2). Route B needs no "which one is THE base colour" decision at all.
- In route A it does, and the rule is in §3.1.

### 2.4 Zero, one or several candidates — the refusal vocabulary

A fire's candidate set is the union over the target's eligible slots of the textures that pass §2.3
**and**, in route B, resolve through a texture parameter (§1.4 step 2).

| candidates after §2.3 | UV family | normal family |
|---|---|---|
| 0 overall | refuse **`no_eligible_texture`** (counters say how many were virtual / trivial / excluded / unsupported / not a parameter) | same |
| no `normal` class | fire on the `colour` + `data` textures | refuse **`no_normal_map`** |
| several `normal` | transform all of them | corrupt **all** of them. A material with a main normal plus a detail normal (`T_rock_02_N` + `T_detail_N`) gets both. The label lists each texture, and ties are never resolved by name. |
| more than `TexCorruptMaxTextures` (compiled **8**) | keep the 8 largest by `GetSizeX()·GetSizeY()`; count the rest `texture_cap` | same |

**Target-level refusals** (closed vocabulary, each with a `run_summary` counter and a loud `REFUSED`
line in the `lod_popping` / `stuck_low_mip` shape — `Anomaly_StuckLowMip.cpp:555-567`):

| reason | when |
|---|---|
| `no_mesh` | the substring resolves no static or skinned mesh component |
| `translucent_only` | every slot is translucent. Route A adds `masked` and `two_sided` here (the agreed rule). |
| `no_textures` | `GetUsedTextures` returned nothing for every slot (§2.1) |
| `no_eligible_texture` | §2.4 table |
| `no_normal_map` / `normal_unconnected` | normal family (§2.2) |
| `texture_not_parameter` | route B: none of the eligible textures resolves through a parameter |
| `nanite_override` | route B: the slot is on a Nanite component and its material has a Nanite override (§1.4) |
| `held_by_stuck_low_mip` | an eligible texture is currently held or awaiting restore by `stuck_low_mip` — the snapshot would bake the blur into the render target (§4.5) |
| `mode_inapplicable` | targeted fire only: the requested mode cannot apply (route A `channel1` on a single-UV mesh, §3.1) |
| `rt_budget` | route B: the event's render targets would exceed `TexCorruptMaxRtBytes` (§8) |
| `dxt5_normal_host` | route B: `Compat.UseDXT5NormalMaps` reads non-zero, so the host unpacks `.ag`, which the render-target encoding does not serve (§3.2). The normal family is refused; the UV family skips `normal`-class textures and transforms the rest. |
| `alpha_texture` | route B, **only if S1 falls back**: an alpha-bearing source format while the alpha-preserving draw is unproven (§3.2) |

---

## 3. Q2 — Material design

### 3.1 Route A (the agreed takeover) — specified in full, as the fallback

**`M_CorruptedUV`** — Lit, **opaque**, **single-sided** (so it matches single-sided targets and does not
reveal back faces), `MSM_DefaultLit`.

- Parameters:
  - `BaseTex`: `TextureSampleParameter2D`, sampler **Color**.
  - `NormalTex`: sampler **Normal**, default = the engine's flat normal.
  - `UvScale` (vector), `UvOffset` (vector, written by C++ per tick for `drift`), `UvSwap` (scalar 0/1),
    `Uv1Mix` (scalar 0/1).
- UV expression: `uv = lerp(lerp(TexCoord0, TexCoord0.yx, UvSwap), TexCoord1, Uv1Mix) * UvScale + UvOffset`.
  **Both textures sample the same corrupted UV**, so the maps move together.
- Lighting constants: Roughness 0.6, Metallic 0, Specular 0.5, AO 1. These are plausible mid-values,
  not the host's, and that is the reconstruction loss §1.3 describes.

**`M_CorruptedNormal`** — same shading. Both textures sample `TexCoord0` unchanged, and the unpacked
normal `n` is transformed:

- `n.xy * NormalSign` (vector; (1,−1) = green flip, (−1,−1) = invert);
- `lerp(n, (0,0,1), FlatMix)`;
- `+ NoiseAmp * (shipped tileable noise normal)`, renormalised.

Route A needs these fixes; each would be a defect otherwise:

- **Two-sidedness must be FALSE.** The plugin's `_fresh_material` authors every material two-sided
  (`tools/create_anomaly_materials.py:28`), which is right for the magenta (a back-face hit must not
  draw the original). For m53 it would reveal back faces the host culls.
- **All seven usage flags** (`set_material_usage_flags.py:112-120`). Without `StaticLighting` the
  default material is drawn on a lightmapped static mesh (`StaticMeshRender.cpp:2225-2228`; the
  Concorde defect).
- **`BaseTex` must be fed an sRGB texture.** Color and LinearColor samplers generate identical code, and
  sRGB decode comes from the texture's own flag in hardware (`[Shaders] Private/MaterialTexture.ush:144-147`,
  `:168-171`; `StreamableTextureResource.cpp:113`). A linear texture in `BaseTex` would draw with the
  wrong gamma.
- **A partial, name-based guard against projected mapping is possible and is WEAK.** A cooked material
  keeps `CachedExpressionData.FunctionInfos` (`Public/MaterialCachedData.h:297-299`), so route A could
  refuse hosts that call the engine's `WorldAlignedTexture*` functions. Host-authored projections
  (StackOBot's `MF_WorldSpaceUV`, Lyra's `MF_WorldGrid`) carry host names, and matching them would be a
  host-specific list, which the game-agnostic invariant forbids. ⇒ It catches the engine-function
  case only, and the §5.4 disclosure stays mandatory.
- **The base colour is chosen as:** the §2.3 `colour` class; then `_D` / `_BC` / `_BaseColor` /
  `_Albedo` / `_Diffuse` / `_Color` as a tiebreak; then the largest texture. Anything still tied is
  refused **`ambiguous_base`**. On the modular kit this refuses or picks `T_SandTileabe_BC` (§1.2).
- 🚨 **`channel1` needs a second UV channel.** The vertex-factory behaviour for a missing TexCoord[1] is
  in §3.1.1. The mode is refused `mode_inapplicable` on a single-UV LOD, read from
  `FStaticMeshLODResources::GetNumTexCoords()` / `FSkeletalMeshLODRenderData::GetNumTexCoords()`.

#### 3.1.1 TexCoord[1] on a mesh with one UV channel

What the GPU returns for a missing channel **depends on the vertex factory, and none of the answers
is a visible "wrong channel":**

| path | TexCoord[1] on a one-UV LOD | source |
|---|---|---|
| `FLocalVertexFactory`, manual vertex fetch (PC SM5/SM6 default) | **clamped to the last channel = UV0** ⇒ `channel1` changes nothing | `[Shaders] Private/LocalVertexFactory.ush:614-622`; `RHI/Private/RHI.cpp:2490`; `Config/Windows/DataDrivenPlatformInfo.ini` |
| `FLocalVertexFactory` without manual fetch | the last UV stream fills empty slots | `LocalVertexFactory.cpp:481-499` |
| GPU skin VF (no skin cache) | `TexCoords0.zw`: the graphics API's default fill, likely (0,1) ⇒ a constant UV — **inference, API behaviour, not UE code** | `GPUSkinVertexFactory.cpp:608-627`; `[Shaders] Private/GpuSkinVertexFactory.ush:95-111`, `:348-361` |
| GPU skin cache (passthrough local VF) | clamped = UV0 | `Public/GPUSkinVertexFactory.h:479`; `GPUSkinVertexFactory.cpp:482-492` |
| Nanite (SM6) | **zero** | `[Shaders] Private/Nanite/NaniteAttributeDecode.ush:390-419`; `NaniteVertexFactory.ush:186-196` |

⇒ `channel1` is refused `mode_inapplicable` unless the drawn LOD has ≥ 2 UV channels.

🚨 **The channel count must NOT be read with `UStaticMesh::GetNumUVChannels`: in a cooked build it
returns 0**, because its body is `#if WITH_EDITORONLY_DATA` (`StaticMesh.cpp:5150-5162`, declared
unguarded at `StaticMesh.h:1525-1530`). A check written with it would refuse `channel1` everywhere in a
package and pass in the editor. Use these instead, all cooked-available:

- `UStaticMesh::GetNumTexCoords(LOD)` / `FStaticMeshLODResources::GetNumTexCoords()`
  (`StaticMesh.h:1747-1750`; `StaticMesh.cpp:911-914`, `:3443-3451`);
- `NaniteResources.NumInputTexCoords` (`Rendering/NaniteResources.h:279`);
- `FSkeletalMeshLODRenderData::GetNumTexCoords()` (`Rendering/SkeletalMeshLODRenderData.h:250-253`).

CPU vertex data is **not** kept for static meshes in a cooked build (`StaticMesh.cpp:498-505`), which
confirms the agreed rejection of a mesh-level UV rewrite.

### 3.2 Route B (recommended) — the two render-target corruptors

Both are **Surface-domain, Unlit** materials, drawn into a render target and never onto a mesh.

**How the draw works — `UKismetRenderingLibrary::DrawMaterialToRenderTarget`, available in a cooked build**
(`Private/KismetRenderingLibrary.cpp:152-217`; its only `WITH_EDITOR` blocks are `:28-34` and `:554-646`).

- It builds an `FCanvas`, draws one `FCanvasTileItem`, flushes it, then calls
  `UpdateResourceImmediate(false)`, which is what regenerates auto mips (`:191-214`;
  `UserInterface/Canvas.cpp:2056-2073`; `UserInterface/CanvasItem.cpp:475-518`).
- **No `FlushRenderingCommands` anywhere on this path**, so the game thread never blocks.
- The tile is drawn by `FTileVertexFactory : FLocalVertexFactory` with **one UV channel**
  (`CanvasTypes.h:1052-1060`; `TileRendering.cpp:98`, `:132-135`) through the base pass, with only
  render target 0 bound, load action `ELoad`, no depth and **no clear** (`Renderer/Private/Renderer.cpp:125-426`,
  `:249-250`, `:342`, `:370-392`).
- 🚨 **The material must be Surface domain, not UI.** `FLocalVertexFactory::ShouldCompilePermutation`
  returns `!!WITH_EDITOR` for `MD_UI` (`LocalVertexFactory.cpp:280-289`). A UI-domain corruptor would
  have no shaders in a cooked build and would draw the default material — while working in the editor
  and PIE, so the defect would be invisible until packaged.
- **No usage flag is needed** (`Classes/Materials/MaterialInterface.h:53-76`).
- **The value written is Emissive**, with no gamma applied in the shader and pre-exposure 1
  (`[Shaders] Private/BasePassPixelShader.usf:1392`, `:1437-1441`, `:2040-2044`; `[Shaders] Private/Common.ush:791`).
  That is why the corruptors are **Unlit**: a Lit model adds the base pass's diffuse and indirect terms.
- 🚨 **An opaque material writes alpha = 0** (`BasePassPixelShader.usf:1916`;
  `[Shaders] Private/LightAccumulator.ush:101`). A translucent one writes `old alpha × (1 − Opacity)`
  (`BasePassPixelShader.usf:1884-1886`; `BasePassRendering.cpp:261-279`). See "Alpha" below.
- **Engine-side readiness:** the call runs `Material->EnsureIsComplete()` first (`:183`). That is a
  blocking compile in the editor and a no-op in a cooked build (`MaterialInterface.cpp:1468-1480`). So
  an editor leg cannot bake the grey fallback into a render target. It can still hitch, which is what
  `D6`'s prewarm addresses.
- **Cost per draw (render thread):** a fresh `FSceneViewFamily` + `FSceneView`, tile vertex and index
  buffers, and a `FViewInfo`, with the engine's own comment "this is going to be slow for each tile"
  (`TileRendering.cpp:89-149`, `:272-291`; `Renderer.cpp:130-131`). Negligible once at Apply; it is
  why `drift`'s per-frame redraw is costed separately in `G-COST`.

**Render targets** (`Classes/Engine/TextureRenderTarget2D.h`, `Private/TextureRenderTarget2D.cpp`):

- `RTF_RGBA8` and `RTF_RGBA8_SRGB` both map to `PF_B8G8R8A8` (`TextureRenderTarget2D.h:18-67`).
- `IsSRGB()` gives the resource the sRGB flag, and D3D12 then gives both the render-target view and the
  shader-resource view `_SRGB` formats, so linear values round-trip (`TextureRenderTarget2D.h:225-247`;
  `TextureRenderTarget2D.cpp:583-604`; `D3D12RHI/Private/D3D12Texture.cpp:1407-1410`).
- `bAutoGenerateMips` sizes the mip chain and generates it with `MipsSamplerFilter` / `MipsAddressU/V`
  (`TextureRenderTarget2D.cpp:50-62`, `:682-719`). The corruptor copies the source's `AddressX/Y`
  into both the target and its mip addressing. A host material's shared sampler still overrides the
  address mode where it has one (`MaterialUniformExpressions.cpp:967-975`).
- A render target is accepted as a texture-parameter value: it reports `MCT_Texture2D`
  (`TextureRenderTarget2D.cpp:81-84`; `MaterialUniformExpressions.cpp:955-963`), and no sampler type is
  checked at runtime (`MaterialInstance.cpp:3428`).

**`M_CorruptTex_UV`** — blend mode **AlphaComposite** (see "Alpha" below). A scalar `AlphaFromSource`
(0/1) forces `Opacity = 0`, i.e. A = 1, **without a re-cook**. That matters because a MID cannot
override blend mode — it forwards to its parent (`MaterialInstance.cpp:1598-1641`) — so the alpha
fallback has to be a parameter, not a second material.

- Three source parameters: `SrcColor` (Color sampler), `SrcData` (Masks sampler), `SrcNormal` (Normal
  sampler).
- `SrcKind` (scalar 0/1/2) selects one. All three are sampled; the unused two carry 4×4 placeholders.
- Scalar parameters: `UvScale`, `UvOffset` (drift), `UvSwap`, `ScrambleCells` (K), `ScrambleSeed`.
- The output UV of each render-target texel is `f(uv)`:
  - **tile** — `frac(uv·N)`;
  - **swap** — `uv.yx`;
  - **drift** — `frac(uv + offset(t))`;
  - **scramble** — the K×K cell index permuted by a seeded hash, the local cell UV kept.

**`M_CorruptTex_Normal`** — blend mode **Opaque** (a normal map's alpha is never read on the default
`.rg` path, see the encoding table):

- One source parameter `SrcNormal` (Normal sampler), unpacked to `n`.
- Output: `NormalSign` (green flip / invert), `FlatMix` (flat), `NoiseAmp` with a shipped tileable
  noise-normal texture (noise).
- **`noise` uses a shipped texture, not the Noise node**, so the pattern is deterministic across runs
  and hosts.

**Render-target encoding — the part that must be exactly right.** Each encoding is checked by the
identity twin (`G-ID`), never assumed:

| source class | render-target format | written value | why |
|---|---|---|---|
| `colour` (`SRGB` true) | `RTF_RGBA8_SRGB` | the hardware-decoded linear colour; the sRGB write re-encodes it | The host's Color sampler decodes in hardware from the resource's sRGB flag. Color and LinearColor samplers emit identical code (`[Shaders] Private/MaterialTexture.ush:144-147`, `:168-171`; `StreamableTextureResource.cpp:113`), so the target's sRGB flag must equal the source's. |
| `data` (`SRGB` false) | `RTF_RGBA8` (linear; `bForceLinearGamma` default true, `TextureRenderTarget2D.cpp:29-44`) | the raw sample | same reason, other way round |
| `normal` | `RTF_RGBA8` (linear) | **`n.xy · 0.5 + 0.5` in R and G** (B = `n.z·0.5+0.5`, ignored) | The host's Normal sampler runs `UnpackNormalMap`, which reads `.rg` then `*2−1` and rebuilds z — unless `DXT5_NORMALMAPS`, when it reads `.ag` (`[Shaders] Private/Common.ush:1373-1384`). `DXT5_NORMALMAPS` comes from `Compat.UseDXT5NormalMaps` at shader compile and defaults to **0** (`ShaderCompiler.cpp:6056-6059`; `Core/Private/HAL/ConsoleManager.cpp:2842-2847`). The `.ag` form is **not** served, because the opaque normal corruptor writes A = 0. So the normal family **reads that console variable at Apply and refuses `dxt5_normal_host` when it is non-zero** — a host setting read at runtime, never assumed. |

⚠ **Alpha — the one encoding the default draw cannot reproduce.** Some hosts pack data into a
texture's alpha: opacity in base-colour alpha, or roughness. An **opaque** corruptor writes A = 0
(above), so a `colour`/`data` texture whose alpha the host reads would lose it. Nothing at runtime says
whether the alpha is read. The design therefore:

- **(primary)** draws `colour` and `data` sources with an **AlphaComposite** corruptor after clearing the
  target to (0,0,0,1), outputting `Emissive = src.rgb` and `Opacity = 1 − src.a`. From the
  translucent output and blend state
  (`BasePassPixelShader.usf:1884-1886`; `BasePassRendering.cpp:261-279`) this yields
  `RGB = src.rgb + 0·(1 − Opacity)` and `A = 1·(1 − Opacity) = src.a`.
  - ⚠ **That is a derivation, not a measurement.** `G-ID` has to prove it on a target whose host reads
    base-colour alpha. On StackOBot the masked Bot face (`MI_BotFace`) is the candidate. If no fixture
    target reads alpha, `G-ID` writes **UNEXERCISED** for the alpha half.
  - The extra clear is one more draw at Apply (`UKismetRenderingLibrary::ClearRenderTarget2D`).
- **(fallback)** if S1 cannot prove the primary path, refuse alpha-bearing source formats
  (`GetPixelFormat()` ∈ {DXT3, DXT5, BC7, B8G8R8A8, …}) as `alpha_texture`. **That is a yield cost**,
  because BC7 is common for albedo whose alpha is unused.

**Usage flags:** none. The tile vertex factory is a local vertex factory and `EMaterialUsage` has no
entry for it (`MaterialInterface.h:53-76`), so the seven-flag list does not apply and the permutation
count stays at the floor. **Material domain: Surface, never UI** (above).

**Lighting plausibility is inherited.** The host's graph does the lighting, so there is no constant to
choose and nothing to glow. `G50`'s emissive-bleed hazard does not arise, because the Unlit emissive
goes into an off-screen render target and never into the scene's Lumen surface cache.

### 3.3 Permutations, the first-use hitch, `m47`, and the cook

- **Permutations.**
  - Route A: two Lit materials × 7 usage flags × the project's vertex factories.
  - Route B: two Unlit materials × the draw path's one VF.
  - Both are one cook.
- **First-use hitch in a packaged build.** PSO precaching is off by default in 5.1
  (`PipelineStateCache.cpp:103-110`; `IsComponentPSOPrecachingEnabled` `PSOPrecache.cpp:36-39`), so the
  first draw of each new material × VF pair creates its PSO and the RHI thread waits for it
  (`PipelineStateCache.cpp:2067-2094`). **The pixels are still correct; it is a stall, not a wrong
  frame.**
  - Route A pays it in the scene, on the first labelled frame, once per VF family per session.
  - Route B pays it once per session on the render-target draw at Apply. It is **also** absent from the
    scene, where the MID reuses the host's shaders and render state.
  - Measured by `G-COST` (§6). No number is predicted.
- **Shader readiness (`m47`).** `GatherAnomalySwapMaterials` lists only the checker and the magenta
  today (`AnomalyCaptureSubsystem.cpp:2399-2412`).
  - **Route B:** the engine's render-target draw already calls `EnsureIsComplete()` on the corruptor
    before drawing (`KismetRenderingLibrary.cpp:183`). An editor leg therefore **cannot** bake the
    grey fallback into a target — it blocks instead, a hitch inside the Apply frame. Adding the two
    corruptors to the m47 prewarm moves that block to run start, which is the point of the prewarm.
  - **Route A:** the materials draw in the scene, so without the prewarm an editor leg's first frame
    could draw the grey fallback (`G231`). The prewarm is **required** there.
  - ⚠ Adding them also widens `G298`'s run-wide false-positive mark: one more material that can read
    incomplete and mark every frame.
  - **Recommended:** add them to the **prewarm** list and **not** to the per-frame
    `AnomalyMaterialsIncomplete` count (`:4442`) until `G298`'s filed fix (per-anomaly readiness) lands.
    `D6`.
- **Cook.** One cook carries both new `.uasset`s.
  - They are hard-referenced from the subsystem CDO exactly like the magenta
    (`AnomalyInjectorSubsystem.cpp:53-55`; `G45`), and `CanContainContent` is already true.
  - The cook runs on **editor** binaries, so the editor target is rebuilt first (`G47`, runbook §8.6
    step 3.5).
  - Presence is proven by a **runtime load and a use**, not by a `.pak` listing (`G48`).
- **Office hosts.** Concorde and Bates build and cook the plugin themselves, so both new assets must
  reach their content tree by git pull.
  - The office procedure gains a runtime-load read-back: a new one-time startup log line from the
    injector subsystem, naming each shipped material and whether it resolved non-null. No such line
    exists today for the checker or the magenta; their presence has been inferred from their use.
  - Route A would additionally need the `missing bUsedWith` log grep that `G157` prescribes.
  - Route B adds **no usage-flag dependency** on the host's lighting setup. Route A needs the static
    lighting flag on Concorde (`G157`), and this bench cannot verify that flag: StackOBot sets
    `r.AllowStaticLighting=False` (`Config/DefaultEngine.ini:23`).

### 3.4 Snapshot resolution and texture streaming

**How the streamer learns a component's textures.**

- `UStaticMeshComponent` hands `GetStreamingTextureInfoInner` its prebuilt data **only for Static
  mobility** (`Components/StaticMeshComponent.cpp:1101`).
- `FStreamingTextureLevelContext::ProcessMaterial` then uses either the component's prebuilt list
  (cooked-only branch, `Streaming/TextureStreamingBuild.cpp:649-670`) or `Material->GetUsedTextures`
  (`:672`). It skips non-streamable textures (`:680-686`), and when a texture has no entry it falls back
  to "a sampling scale of 1 using the UV channel 0" (`:712-713`).
- Material `TextureStreamingData` is cooked but **built only by editor tools** (`MaterialInterface.h:260-262`;
  `Developer/MaterialUtilities/Private/MaterialUtilities.cpp:2103-2248`).

**What `SetMaterial` does to a static component.**

- It calls `NotifyPrimitiveUpdated_Concurrent` (`MeshComponent.cpp:78-82`).
- That **adds** a dynamic registration and does **not** remove the static references
  (`Streaming/StreamingManagerTexture.cpp:1054-1064`; `DynamicTextureInstanceManager.cpp:183-185`).
- ⇒ **The original material's textures keep their static streaming entries for the rest of the level's
  life.** Residency is per texture and is the max over all users (`AsyncTextureStreaming.cpp:187-195`,
  `:213`; `TextureInstanceView.cpp:510-511`).

**Route B — the snapshot rule.**

- **The render target is drawn ONCE at Apply** from the source texture's then-resident mips, at a
  resolution of `min(top resident mip width, TexCorruptRtMaxSize)`. The resident mip count is read from
  `GetNumResidentMips()` (`Texture2D.cpp:455-491`), and the top width is the m52 arithmetic
  (`Anomaly_StuckLowMip.cpp:44-49`).
- So the corrupted texture has exactly the detail the viewer was seeing at the moment of the fire.
- `drift` redraws every frame from that **snapshot**, never from the source, so a later stream-out of
  the source cannot change the event's content.
- A render target is not a streamable asset. After Apply it is fixed-resolution for the event.
- 📌 **Named limit.** If the camera approaches during the event, the host would have streamed in finer
  mips while the snapshot stays at Apply-time detail, so the object can look softer than an
  uncorrupted neighbour. On a settled bench pose this cannot occur. The telemetry records
  `src_resident_top_px` and `rt_size` per texture, so the case is visible rather than hidden.
- ⛔ **No force-resident call** (`SetForceMipLevelsToBeResident`, `StreamableRenderAsset.cpp:218`): it
  raises residency **for every user of the texture** (`AsyncTextureStreaming.cpp:269-276`), and on a
  visible co-user that is an unlabelled change.

**Route A — streaming density under tiling.**

- A takeover MID has no streaming data of its own. `SetTextureParameterValue` records a
  placeholder-to-host rename (`MaterialInstanceDynamic.cpp:229-231`, `:505-519`), so the host texture
  gets the placeholder's density if the plugin material was built with streaming data, and the
  scale-1 UV0 fallback otherwise.
- 🔻 **Tiling does NOT blur under that fallback.** ×8 tiling means a sampling scale of ~8, which *lowers*
  the needed mips (`MaterialInterface.cpp:1373`; `[Shaders] Private/DebugViewModePixelShader.usf:86-96`).
  The streamer over-requests; only a UV scale below 1 would under-request.
- Virtual textures are outside the streamer altogether (`Texture2D.cpp:959-963`,
  `StreamableRenderAsset.cpp:277-293`), which is one more reason §2.3 skips them.

---

## 4. Q3 — Takeover (or override) and restore

### 4.1 Slot selection, and meshes with several materials

- Components come from `AnomalyLod::ResolveLodComponents` (`AnomalyLod.cpp:11-30`), the resolver all
  texture anomalies share. Viewport scoping applies when on (`Anomaly_CorruptedTexture.cpp:84-92`
  pattern).
- **Per slot**, not per component: a slot is corrupted iff its material passes the route's slot test
  and at least one of its textures passes §2.3 / §2.4.
  - **Route B** slot test: not translucent.
  - **Route A** slot test: opaque, not two-sided, not masked.
- A multi-material mesh therefore gets a **partial** corruption: glass stays glass. The label records
  `slots_corrupted / slots_total`.
- One fire covers the target's eligible slots on every matched component, as `corrupted_texture` does
  (`:99-127`). A target with zero eligible slots is refused.

### 4.2 MID lifetime, and sharing

- **One MID per distinct source material within an event.** When several slots or components of the
  target use the same source material, they share that one MID; its parameters are identical by
  construction. **Never across events and never across actors**: siblings sharing the host material
  keep the original, and that isolation is the anomaly's point (`G46`).
- **The render targets are keyed per (source texture, mode transform) within an event**, so a texture
  used by two slots is drawn once.
- Outer = the component, so the MID and the render targets are GC-reachable only through the
  component's override while applied, plus a strong `TArray<TObjectPtr<>>` held by the anomaly.
  - The anomaly is not a `UObject`, so the strong reference lives on the injector subsystem as a
    `UPROPERTY` array, the `CorruptedTexturePink` pattern (`AnomalyInjectorSubsystem.h:133-137`).
  - They are released at revert, so nothing outlives the event.
- A MID is never re-parented. A new fire always creates new instances.

### 4.3 Exact restore — the `m17` contract, mirrored

The contract is `Anomaly_CorruptedTexture.cpp:136-253`, copied rather than extracted (the m29
precedent, `G46`/`m17`):

1. **Apply** captures, per slot: owner, component `FName`, slot index, original material, and
   `bWasExplicitOverride` (`:114-121`).
2. **Revert** re-finds the live component (weak pointer, otherwise by name on the owner) and touches a
   slot **only if it still holds OUR MID**. That is tested by identity against the event's MID set,
   because there is no pink to walk to.
3. The slot is set back to the captured original if it was an explicit override, otherwise
   `SetMaterial(i, nullptr)` → the mesh asset's slot material (`MeshComponent.cpp:48-109`;
   `StaticMeshComponent.cpp:2655-2678`; `SkinnedMeshComponent.cpp:1252-1264`).
4. **Sweep** every mesh component of each touched owner for any slot still holding one of our MIDs
   (component re-created after apply).
5. **Log** `restored / default-reset / left-to-game / unresolved / swept / re-found`.
6. **Release** the render targets and the MID references.

⚠ **`SetTextureParameterValue(nullptr)` cannot clear an override** — a null texture enqueues nothing
(`MaterialInstance.cpp:3428`). The design never tries to; the restore swaps the whole slot back, which
is why the MID is ours and disposable.

### 4.4 Destruction, level change, teardown

- The target actor is **watched** through `AActor::OnEndPlay`, with a weak-pointer poll as backstop,
  exactly as `stuck_low_mip` does (`WantsTargetLostNotification` / `OnTargetLost`,
  `Anomaly_StuckLowMip.cpp:618-640`, `:784-797`; `IAnomaly.h:40-44`).
- On loss the anomaly reverts at once. Unlike m52, **no shared asset is touched**, so there is nothing
  left to restore on a destroyed actor. The watch exists to stop `IsVisualConditionHeld` labelling a
  target that is gone, and to release the render targets promptly.
- `FinishRun`, cancel-before-focus and world teardown reach `Revert` through `RevertAllActive`
  (`AnomalyInjectorSubsystem.cpp:199-230`).

### 4.5 Interplay with `stuck_low_mip`

m53 mutates **no asset**, so the one-anomaly-per-actor invariant (`G30`) is sufficient against every
anomaly **except** `stuck_low_mip`. That one holds a shared **texture** that an m53 target may also
sample. A render-target snapshot of a held texture would bake the blur in and label it as a UV or
normal bug. Hence the refusal `held_by_stuck_low_mip`, read from a new additive query
`AnomalyStuckMip::IsTextureHeldOrRestoring(const UTexture2D*)` in the same module.

The reverse direction (m52 firing while an m53 event is live) needs no new code:

- **A texture m53 has corrupted** is sampled by the target only through its render-target snapshot,
  so a later m52 hold cannot reach the corrupted target at all.
- **A texture m53 left untouched** is still sampled by the target's MID (e.g. the base colour during a
  normal-family event). `Component->GetUsedTextures` therefore still counts the target as a visible
  co-user, and m52's auto-pool gate (`StuckMipMaxCoAffected`, compiled 0) refuses it.
- A **targeted** m52 fire bypasses that gate by design, as it always has.

---

## 5. Q4 — Visibility evidence

### 5.1 What m55 gives, and what it may not be used for

- m55 measures, per labelled phase, the first four frames' changed-pixel statistics inside the
  target's delivered mask (`chg_*`), against the rest of the picture (`ctl_*`) and against the
  pre-onset frame (`ref_*`) (`docs/change-evidence-fields.md:40-77`).
- **It carries no verdict, and nothing it measures may feed labels, selection or observability**
  (`:7-21`; client readme §9.1).
- ⇒ **m53 cannot use an m55 number as a floor in v1.** Every weak-mode rule below is either a
  pick-time rule computed from bounds and texture fields, or a label caveat. m55 is the **bench
  instrument** that calibrates those rules and the **client-side evidence** the client can filter on.

### 5.2 Predicted strength per mode — DECLARED, no numbers

"Strong" means m55 onset `chg_gt8 / chg_n` clearly above the same leg's `null_effect` twin and control.
"Weak" means it may not be. ⚠ **m55 measures pixel difference, not perceptibility.** A transposed rock
texture changes nearly every pixel and still looks like rock. That limit is stated, not solved (§5.4).

| mode | expected m55 onset (window 0) | expected perceptibility | why |
|---|---|---|---|
| `uv_tile` ×8 | **strong** on any textured surface with spatial content | strong where the texture has features (panels, logos, ribbing); weaker on stochastic textures (rock, concrete) | the texel frequency jumps 8× |
| `uv_swap` | strong on anisotropic textures; still a high pixel change on stochastic ones | **weak on isotropic stochastic textures** | transpose preserves texture statistics |
| `uv_scramble` (B) / `uv_channel1` (A) | **strong** | strong | cell seams / island mismatch are content-independent |
| `uv_drift` | window 0: **small** (one frame's offset); windows 1–3: accumulating in `ref_*` | strong **in video**, weak in a still | §9.3 of the client readme is exactly this shape |
| `normal_invert` | strong under a directional key light; weak in flat ambient light | same | the lighting sign flips |
| `normal_green_flip` | medium: only the lighting's vertical component flips | medium | half the invert signal |
| `normal_flat` | **weak** on low-frequency normals (the m52 `T_Paint_Normal` finding, 080-04 §6); stronger on high-detail normals | weak/medium | removes detail rather than adding error |
| `normal_noise` | medium to strong, scaling with the amplitude | medium | adds high-frequency lighting noise |

### 5.3 How weak cases are handled — chosen, and why

**Chosen: the per-mode auto-pool mode set, plus pick-time floors that need no pixel read, plus a label
caveat. Not a measured-change floor and not a runtime refusal on measured change.**

1. **Mode set.** `IAI.Anomaly.UvCorruptModes` / `NormalCorruptModes` (ini + console, `AnomalyDefaults`
   pattern) decide which modes the **auto-pool** may draw. Targeted fire can always request any mode.
   - Compiled defaults, recommended and pending `G-STR`: UV = `tile scramble drift`; normal = `invert
     green_flip noise`.
   - `uv_swap` and `normal_flat` are **available but off by default**, because §5.2 predicts both can be
     imperceptible on common content.
   - The set is **re-ruled from `G-STR`'s measurement** in an amendment, the m52 ratio precedent
     (4.0 → 8.0 after measurement).
2. **Pick-time floors, bounds and fields only** (they stay on the right side of `G127`):
   - the §2.3 trivial-texture floor (constants cannot be corrupted);
   - the existing auto-pool coverage floor (`GMinScreenCoveragePct` 6 %, `AnomalyViewport.cpp:42`),
     unchanged;
   - normal family only: `normal_unconnected`.
   - ⛔ **No invented texel or screen-size threshold.** m52's size ratio was measured blind to texture
     content (080-04 §6), and this plan does not repeat it.
3. **Label caveat.** Each m53 anomaly entry carries `texcorrupt.strength_class`, written as the
   **declared** class from §5.2 and never a measurement, so a client can exclude weak modes without
   parsing m55:
   - `strong`: `tile`, `scramble`, `channel1`, `invert`;
   - `medium`: `green_flip`, `noise`, and `drift` (weak in a single still, strong in video);
   - `weak`: `swap`, `flat`.

   The default mode sets are exactly the strong and medium classes. `G-STR` either confirms the
   classes or corrects them in an amendment, and the correction moves the key's values and the
   default sets together. `observable` keeps the m49 A1 rule unchanged:
   `bLabelled && bHeld && Px >= ObservableMinPixels` (`AnomalyCaptureSubsystem.cpp:4072`), with `bHeld`
   from `IsVisualConditionHeld` (`:4884-4887`).

**Why not a floor on measured change:**

- m55's contract forbids it.
- The measurement arrives after the frame is written.
- A floor that deletes events on measured change is `m26`'s veto. Re-using that shape for a
  **perceptual** question would need a calibration campaign with complex content, and `G135` / `m26`
  ruled that no such campaign exists yet.

**Why not a refusal:** at pick time nothing tells a flat normal map from a detailed one. The texture's
CPU data is not available in a cooked build, and the GPU data is not readable at pick time without a
readback (`G127`).

### 5.4 If route A is ruled — the extra disclosure it needs

- Every route-A event writes `texcorrupt.reconstructed: true`.
- The client readme gains a paragraph: the object's material is replaced by a simplified copy built
  from its base-colour and normal textures; tints, packed maps, projections and masks are dropped; and
  the visible change **includes** that simplification.
- `G-FID` (§6) then **measures the reconstruction alone** (identity mode) per fixture target and
  reports it beside the mode's own change. That gives the client a number for how much of the change
  is not the named mode.

---

## 6. Q5 — Gates and fixtures

### 6.1 Which targets qualify, per fixture — PREDICTED from the scan, to be READ at runtime by `G0`

| fixture | target | route B prediction | route A prediction |
|---|---|---|---|
| StackOBot `MainWorld` | `SM_rock` (`…_2048592804`), `SM_rock_02` | **eligible, both families** — MIC chain overrides `T_rock_0x_D/N/AORM` parameters | eligible, both families (single-sided opaque; base `T_rock_0x_D`) |
| StackOBot `MainWorld` | modular kit (`SM_Ramp` & co.) | UV family eligible **if** its textures are parameters; normal family eligible (`T_*_N`). ⚠ mostly Nanite ⇒ mask-unmeasurable (`G134`) | base = `T_SandTileabe_BC` or `ambiguous_base`; high reconstruction delta |
| StackOBot `MainWorld` | `SM_FloorBase` | **refused `no_eligible_texture`** for normal (mask only), UV eligible on `T_Grid_A` (data class) if a parameter | refused `no_base` |
| StackOBot `MainWorld` | `SKM_Bot` | eligible (masked is fine in B); its slots hold runtime MIDs ⇒ **clone path** (§1.4) | **refused** `masked` / `two_sided` |
| StackOBot `CB_GateLevel` | every target | **refused 100 % `no_textures`/`no_eligible_texture`** — `BasicShapeMaterial` references no texture (m52 §3.2; `make_gate_level.py:60`, `:68-69`) | same |
| Lyra `L_ShooterGym` | `Cube*` (MI_MS_* triplanar) | **eligible, both** (`T_Paint_Diffuse` colour, `T_Paint_Normal` normal); ⚠ `T_Paint_Normal` is low-frequency ⇒ `normal_flat` predicted weak (080-04 §6) | eligible; tint + triplanar lost |
| Lyra `L_ShooterGym` | weapons `SM_*` | eligible; Nanite ⇒ measured only if a non-Nanite instance is on screen | eligible |
| Lyra | `SKM_Manny/Quinn` (`B_Quinn_C_10` measured in 080-04) | eligible, clone path if MID | refused `masked` |

⇒ **`CB_GateLevel` is a REFUSAL fixture only**, exactly as it was for m52. It is not an m53 gate
fixture and must not be edited into one (`G99`). **The settled-camera arbiter for m53 is a
`MainWorld` pose on `SM_rock`**, plus Lyra `Cube4` at `L_ShooterGym`'s bench pose (the m55 twin pose).

### 6.2 Twins

- **Null twin = the identity mode** (`uv_tile` with N = 1 and offset 0, route B), fired at the same
  target and pose. Route B predicts it indistinguishable from "nothing happened": its onset pair reads
  like the phase's later, unchanged windows, and on `L_ShooterGym` like m55's `null_effect`, whose
  banked onsets read < 0.01 on StackOBot and < 0.02 on Lyra 720p
  (`2026-09-24-m55-081-23-requalification.md:135`).
  - It is a **bench-only lever** (`IAI.Bench.TexCorruptIdentity`, console only, default OFF, never in a
    client payload), because a label naming a mode that changes nothing is exactly what must never
    ship.
- **Positive twin = `solid_swap`** (m55, unchanged), giving the full-change reference at the same pose.
- **Can-fail lever (`G96`):** `IAI.Bench.TexCorruptNoApply`. The anomaly does all its bookkeeping, draws
  the render targets, and deliberately does not set the slot. Its reading must be
  `IsVisualConditionHeld` false on every frame and `observable` false. Without it a green
  `condition_held` proves nothing.

### 6.3 The gate list — pre-declared readings

All legs are **packaged** (`G76`). **Both tick orders** (native + `IAI.Bench.SynthTickOrder`) wherever
a gate asserts per-frame alignment (the m44 standing rule). Pixel-identity claims are made at the
**AA-off arbiter in native order** only, with the same-build control reading 0 (the m45 identity
arbiter; `G228`, `G230`).

| id | gate | pre-declared reading |
|---|---|---|
| **G0** | Eligibility read-back per target: slots, blend mode, the per-texture §2.3 class, parameter mapping, VT, size | the §6.1 predictions are **read**, never assumed; every disagreement with §6.1 is reported as a finding (the scan is evidence, not truth) |
| **G-ID** 🚨 | route B identity round-trip, `SM_rock` + Lyra `Cube4`, AA-off arbiter, native order | Self-contained per phase: window-0 `chg_gt8` (original → identity) **inside the band of the same phase's windows 1–3**, where nothing changes by construction, and `ref_gt8` on windows 1–3 inside the same band. On `L_ShooterGym` the reading is **also** checked against the `null_effect` twin at the same pose. That twin is fixture-gated to `CB_GateLevel` / `L_ShooterGym` (`Anomaly_ChangeCase.cpp:21-23`), so MainWorld has only the self-contained form. **If it fails, Stage 1 STOPS and route A is the fallback (§1.5).** |
| **G1** | each mode engages, targeted, `SM_rock`, both orders | label source `FireWindow`: first labelled frame == first differing frame (m44 ONSET) for every mode except `uv_drift`, whose onset is sub-`tau` by prediction and is read from `ref_*` |
| **G2** | can-fail (`TexCorruptNoApply`) | `texcorrupt.condition_held` false and `observable` false on every labelled frame. `frames_condition_lost` counts them (`AnomalyCaptureSubsystem.cpp:4061`). `injected_frames` is non-empty and `affected_frames` is empty. The event **stays** in `annotation.json` (the m49 ruling) and is **not** vetoed: the host still draws, so `m26` reads non-zero. m55 onset reads inside the `null_effect` band. **A G1 pass without this is not a result (`G96`).** |
| **G-STR** | per-mode strength, the §5.2 table, both fixtures, m55 on | m55 onset and `ref_*` per mode **reported against the null and positive twins at the same pose**; the §5.2 ranks are either confirmed or corrected in an amendment; **this reading re-rules the default mode sets** |
| **G-FID** | route A only: reconstruction delta (identity mode) per target | **reported per target**, no threshold; published beside every mode's reading |
| **G3** | restore identity | after revert + settle, the post-revert frame vs the pre-onset reference inside the window-0 mask: `|d|>8` count within the null twin's band. `IsVisualConditionHeld` false from the revert frame on. Slot materials pointer-identical to the captured originals (log line). |
| **G4** | restore on every exit: normal revert, `FinishRun`, cancel before focus, target destroyed mid-span, level change | every slot restored or default-reset per §4.3; render targets released (a count read back); zero `swept` on a static fixture |
| **G5** | `held_by_stuck_low_mip` interplay | fire `stuck_low_mip` (targeted), then m53 on a target that samples a held texture ⇒ refused `held_by_stuck_low_mip` by name. In the reverse order, an auto-pool m52 pick on a texture the live m53 target still samples ⇒ m52 refuses it as shared. |
| **G6** | Nanite + clone paths | a Nanite target (modular kit) fires and is labelled with `observability_measured` false (the m50 admit path); a Bot slot holding a runtime MID is cloned and restored; a `nanite_override` case is refused by name, or **UNEXERCISED** is written if the fixture has none (never a clean zero) |
| **G7** | auto-pool yield and refusal census, both fixtures, both ids | **no value predicted**; fires attempted, applied, refused per reason; the number the default-pool decision needs |
| **G8** | schema additivity: `P-C7 v3` against a pre-m53 control pair | `labels.jsonl` field set unchanged on a run without m53; `run_summary` adds exactly the `texcorrupt_*` keys; `annotation.json` field set unchanged (`P6`); `anomaly_subtype` values are new strings for new ids only |
| **G-COST** | paced 30 fps, A/B on the same build, apply-heavy targeted leg | `speed_ratio` and the m55 cost statistic within the within-build spread; the first-use PSO hitch **measured and reported** (first-fire frame time vs the rest); "below the resolution of this instrument" is the most a small difference can read (`G169`) |
| **G9** | verifier read, `--label-pixel-gate` + `--change-oracle` on the m53 sessions | readings only; `--change-oracle` exit 0 (arithmetic); `NO-TRACE` on a weak mode is a reading for `G-STR`, not a failure |
| **G10** | both build targets, packaged + editor | exit 0, zero warnings (`G221` — every new export crosses a DLL boundary only in the editor target) |
| **G11** | cook + load | both new assets load at runtime in the packaged build, and one fire draws a non-default render target (`G45`, `G48`) |
| **G-LYRA** | one leg per id on `L_ShooterGym` at Lyra's own render defaults (TSR, Lumen, AE on) | fires or refuses **for a named reason**; clone path exercised on a character if one is on screen |

### 6.4 Yield

`G7` gives the per-fixture numbers. **No yield is predicted.** The scan says StackOBot's measurable
(non-Nanite) textured targets are few — the rocks, plus the Bot under route B — so a thin MainWorld
yield is **expected and is not a defect**.

---

## 7. Q6 — Auto-injection

### 7.1 Two ids, not one

`uv_corruption` and `normal_corruption` are **separate ids**, each with its own `mode` enum:

- separate pool keys, so the owner can ship the strong family without the weak one;
- separate `anomaly_type` values, so the client's classes stay distinct;
- separate refusal counters.

The catalogue grows by two. `NumPoolKeys` goes 7 → 9 (`AnomalyAutoInjectorSubsystem.h:40`), with keys
`EKeys::Eight` / `Nine` and the `IAI.Auto.Bind` usage string extended (`AnomalyAutoInjectorSubsystem.cpp:1012`).

### 7.2 Mode draw — keeping `R-SEED`

The auto-injector today draws id, target and hold, then calls `ApplyAnomaly(Id, {Token})` with no mode
(`AnomalyAutoInjectorSubsystem.cpp:272`, `:374-380`). **Recommended:** one extra `Stream.RandHelper`
draw **only when the drawn id declares an enum `mode` arg in its catalogue spec**, over the id's
enabled mode set, passed as the second arg.

- The draw count stays a deterministic function of earlier draws, so the same seed gives the same
  modes.
- The protocol stays independent of the apply result (`R-SEED`).
- Every run that never draws an m53 id is **byte-identical** in its draws.

🔻 **Precision to `G150`, stated not silently applied.** `G150` says adding a pool member re-rolls every
seeded draw. At source, the draw is over `Eligible`, which filters by `EnabledIds`
(`AnomalyAutoInjectorSubsystem.cpp:241-252`). So **a pool member that is not enabled does not change
`Eligible.Num()` and re-rolls nothing**. m29's instance re-rolled because its id was default-enabled.
⇒ Shipping both ids **default OFF** keeps every banked auto-pool comparison valid for runs that do not
turn them on.

### 7.3 Pool membership — RECOMMENDATION; the owner's product call

**Recommendation: both ids in `GAutoPool`, NOT in `GAutoPoolDefaultEnabled`, for the first release.**
Flip `uv_corruption` to default-on after `G7` + `G-STR`. That follows the m52 / m29→m30 precedent
(shipped first, then pool-default decided on a measured yield).

What the owner needs to rule on default-on:

1. `G7`'s applied / attempted fraction on MainWorld **and** Lyra.
2. `G-STR`'s per-mode strength table against the twins, with the frames themselves for a by-eye look.
3. `G-COST`'s first-use hitch number.
4. The chat-side steer m52 used: **default-on if at least 25 % of visible targets are eligible on
   MainWorld**, which m52 recorded as a **future decision rule, not a gate threshold** — `G7` prints
   and passes or fails on nothing (m52 AMENDMENT 1 §A1.1 ruling 1,
   `2026-09-20-m52-stuck-low-mip.md:666-669`).

`normal_corruption` is recommended to stay default-off until a host with high-detail normals (Lyra's
characters and weapons, or the office content) shows `G-STR` strength.

### 7.4 Dashboard

**No dashboard change is needed.** The pool checkbox list mirrors the engine catalogue
(`ControlSnapshot.cpp:184-195`; the m19 rule, m52 §5), so the two ids appear by themselves.

---

## 8. Q7 — Cost and risk

### 8.1 Expected cost (declared; `G-COST` measures)

| item | route B | route A |
|---|---|---|
| Apply | 1 MID per distinct source material + 1 render target per corrupted texture, drawn once; `GetAllTextureParameterInfo` + `GetUsedTextures` per slot | 1 MID per distinct source material; `GetUsedTextures` per slot |
| First use | one render-target PSO per session | one scene PSO per material × VF family per session, **inside the labelled window** |
| Per frame | nothing, except `drift`: one render-target redraw per corrupted texture per frame, sampling the Apply-time snapshot | nothing, except `drift`: one `SetVectorParameterValue` per frame (one render command, `MaterialInstance.cpp:390-416`) |
| Memory | a render target of `min(resident top mip, TexCorruptRtMaxSize = 1024)`² × 4 B × 4/3 per texture ≈ **5.6 MB at 1024**. `drift` needs two per texture (the Apply-time snapshot + the per-frame output). Capped by `TexCorruptMaxRtBytes` (compiled **64 MiB**) ⇒ refusal `rt_budget` | none beyond the MID |

### 8.2 Risks, named

1. **Route A's reconstruction** (§1) — the reason for `D1`.
2. **Route B's render-target encoding** (sRGB, normal convention, alpha): identity-tested (`G-ID`),
   never assumed. Two traps are found at source and designed around, not discovered later:
   - a **UI-domain** corruptor draws the default material in a cooked build while working in the
     editor (`LocalVertexFactory.cpp:280-289`);
   - an **opaque** corruptor writes alpha = 0 (`BasePassPixelShader.usf:1916`).
3. **Parameter coverage**: route B refuses textures that are not parameters. `G7` measures what that
   costs.
4. **MID-of-MID**: handled by cloning, with the frozen-animation limit (§1.4).
5. **Nanite override materials**: refused by name (§2.4).
6. **Snapshot resolution** on camera approach (§3.4).
7. **The m47 `G298` false-positive** widens if the new materials enter the per-frame count (§3.3, `D6`).
8. **Weak modes** are invisible on some content (§5), handled by the mode set, not hidden.
9. **Virtual textures**: a VT texture is **skipped** at the texture level, because binding across the
   VT boundary renders black (§2.3 #2). A host whose textures are all VT (Lyra's Megascans surface,
   080-04 §5) is refused, and `G0` must reach the `virtual_texture` branch on Lyra, or **UNEXERCISED**
   is written.
10. **Office hosts**: route B needs nothing from the host but its own material. Route A needs its usage
    flags to match the host's lighting (§3.3).

### 8.3 Where the agreed shape is contradicted by the source (summary; details in the sections cited)

| agreed | source / content says | § |
|---|---|---|
| reuse the takeover | a takeover cannot carry the host graph (editor-only), so it reconstructs; on most fixture content that changes tint / projection / packing | §1.1–1.3 |
| base colour = "the sRGB texture" | there is no cooked API for "feeds BaseColor"; on the modular kit the only sRGB candidates are a sand overlay and a 1-pixel constant | §1.2, §2.2 |
| opaque, single-sided only | a consequence of the takeover; under route B unnecessary, and under route A it removes the hero character | §1.3(d) |
| lerp to TexCoord[1] | needs a second UV channel: on a one-UV mesh the local VF returns UV0 (invisible) and Nanite returns zero; and the obvious channel-count API returns 0 in a cooked build. Not expressible in texture space. | §3.1.1, §1.4 |
| `M_Corrupted*` are Lit, two-sided like the magenta | two-sided would reveal culled back faces (route A). Route B's corruptors are Unlit, **Surface-domain** (a UI-domain one has no shaders in a cooked build), AlphaComposite render-target materials. | §3.1, §3.2 |
| mesh-level UV rewrite rejected | **confirmed** — the render data is shared; this plan does not revisit it | — |

---

## 9. Q8 — Implementation stages, briefs and estimates

*Nothing below is written. Sizes are estimates.*

### 9.1 Files

| file | change |
|---|---|
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.{h,cpp}` | **NEW.** One class, parameterised by family, registered twice (`uv_corruption`, `normal_corruption`). Discovery §2, slot policy §4.1, MID + render targets §1.4 / §3.2, the m17 restore §4.3, the target watch §4.4, `GetTelemetry` §9.3, the `drift` redraw in `TickAlways`. ~700 lines. |
| `Source/AnomalyInjector/Private/AnomalyTexCorruptRt.{h,cpp}` | **NEW.** Render-target creation / format / draw / release, isolated so its correctness is gated on its own. ~200 lines. |
| `Source/AnomalyInjector/Public/AnomalyTexCorruptStats.h` | **NEW.** Run stats + refusal counters (the `AnomalyStuckMipStats.h` shape). |
| `Source/AnomalyInjector/Public/AnomalyInjectorSubsystem.h` / `.cpp` | two `FObjectFinder` hard refs + getters (`:49-56` pattern); `UPROPERTY` strong-ref arrays for the live MIDs and render targets (§4.2); `Register` ×2 (`:177-190`); `GetAuthoredSpec` arms: `mode` enum + `strength` float (`:100-164`); a one-time `Initialize` line naming every shipped material and whether it resolved (the office read-back, §3.3) |
| `Source/AnomalyInjector/Public/AnomalyDefaults.h` / `Private/AnomalyDefaults.cpp` | knobs: mode sets ×2, tile N (compiled 8), drift speed (compiled 0.25 UV/s), scramble K (compiled 8), noise amplitude, `RtMaxSize` (1024), `MaxRtBytes` (64 MiB), `MaxTextures` (8), `CloneHostMids` (on). Each follows the `…Compiled` / `Key()` / `Get` / `Describe` / `Set…Override` / `Clear…Override` block exactly (`AnomalyDefaults.h:36-44`, `:72-88`): console > ini > compiled, a `G139` echo, out-of-range **refused, not clamped**. The compiled values are design parameters, not calibrated thresholds; `G-STR` reads them. |
| `Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp` / `Public/…h` | `GAutoPool` +2 (not default-enabled); `NumPoolKeys` 7 → 9; the mode draw (§7.2); bench levers `IAI.Bench.TexCorruptIdentity` / `TexCorruptNoApply` |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` + `AnomalyStuckMipStats.h` | additive query `IsTextureHeldOrRestoring` (§4.5) |
| `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `ResolveAnomalyActiveSource` +2 → `FireWindow` (`:287-306`); `MapAnomalyToClient` subtype = the event's mode (`:266-278`); `GatherAnomalySwapMaterials` +2 for the prewarm only (§3.3, `D6`); `run_summary` counters |
| `Source/AnomalyCapture/Private/AnomalyLabelWriter.{h,cpp}` | `texcorrupt_*` `run_summary` keys (the `:740-759` block pattern); per-frame keys already flow through the generic telemetry path (`:133-156`) |
| `Content/Materials/M_CorruptTex_UV.uasset`, `M_CorruptTex_Normal.uasset` + a 256² tileable noise-normal texture | **NEW**, authored by `tools/create_anomaly_materials.py` (extended), cooked once |
| `tools/verify_capture.py` | restore-identity reading for `G3` (pre-onset reference vs first post-revert frame, masked by the window-0 mask) |
| docs | catalogue rows + architecture (route, restore, limits); `client-readme.md` §8 field rows + the anomaly paragraph + the `strength_class` caveat; `PRE-DELIVERY-CHECKLIST.md` categorical box (runtime load of both assets; the pool line); gotchas as found |

### 9.2 Stages, each with a stop point

| stage | content | build / cook | stop if |
|---|---|---|---|
| **S0** | this plan ruled (`D1`–`D7`) | — | — |
| **S1** — route B core, **identity only** | asset authoring + one cook; render-target module; MID / clone / restore; `uv_corruption` with the identity lever only; `G0`, `G-ID` (colour, data, normal, and the alpha half), `G3`, `G4`, `G10`, `G11` | 2 builds (editor + game), **1 cook** | 🚨 **`G-ID` fails on colour/data/normal ⇒ STOP, report, fall back to route A** (a new S1 with the takeover; the same asset cook budget). If **only the alpha half** fails ⇒ switch `AlphaFromSource` off, adopt the `alpha_texture` refusal, and continue — no re-cook. |
| **S2** — all modes, both ids | the eight modes; `G1`, `G2`, `G5`, `G6`, `G8`, `G9`; both tick orders | 2 builds, **0 cooks** if S1's graphs already carry every mode's parameters (they are specified to) | any mode fails ONSET, or the can-fail cannot fire |
| **S3** — measurement | `G-STR`, `G7`, `G-COST`, `G-LYRA`; mode-set and pool amendment | 0–1 build (Lyra worktree build per shared-tree rule 5) | — |
| **S4** — docs + merge | client readme, architecture, checklist, catalogue; strip comments; both targets zero warnings | 1 build pair | — |

**Estimate: 4 implementation sessions after the ruling. 1 cook, and a 2nd only if S1 falls back to
route A or a material graph needs a parameter S1 did not author. About 8 builds across both targets.**

### 9.3 Label keys (additive; `label_schema` stays **2**, the m52 ruling `2026-09-20-m52-stuck-low-mip.md:672-673`, which leaves them for m51's schema-3 bump)

- **`annotation.json`**: `anomaly_type` = the id; **`anomaly_subtype` = the mode**
  (`tile|drift|swap|scramble|green_flip|invert|flat|noise`). This is a new **value** in an existing
  field (`AnomalyLabelWriter.cpp:889`), so the field set does not move (`P6`).
- **`labels.jsonl`, inside m53 anomaly entries only** (the m52 per-frame key precedent; generic path
  `AnomalyCaptureSubsystem.cpp:4890-4915`):
  - `texcorrupt.mode`, `texcorrupt.route` (`host_param` | `takeover`), `texcorrupt.strength_class`;
  - `texcorrupt.slots_corrupted`, `texcorrupt.slots_total`;
  - `texcorrupt.condition_held`;
  - per-mode parameters: `texcorrupt.tile`, `texcorrupt.uv_offset` (drift, per frame),
    `texcorrupt.scramble_cells`, `texcorrupt.noise_amp`;
  - route A only: `texcorrupt.reconstructed`;
  - an array `texcorrupt.textures[]` of `{name, class, param, src_size, src_resident_top_px, rt_size}`.
- **`run_summary.json`**: `texcorrupt_fires_applied`, one `texcorrupt_refused_<reason>` per closed
  reason (§2.4), `texcorrupt_textures_corrupted`, `texcorrupt_rt_bytes_peak`,
  `texcorrupt_clone_host_mid`, `texcorrupt_swept`.

---

## 10. NEEDS-DECISION

- **`D1` — route.** Route B (host-preserving, texture-space; recommended) or route A (the agreed
  takeover, with the §5.4 disclosure). **Blocks S1.**
- **`D2` — `uv_channel1`.** Drop it and ship `uv_scramble` in its place (recommended under B), or keep
  it as a route-A-only mode (a second mechanism in one anomaly).
- **`D3` — two ids** (`uv_corruption` / `normal_corruption`) rather than one. Recommended: two.
- **`D4` — pool.** In `GAutoPool`, **not** default-enabled until `G7` + `G-STR` (recommended); the
  owner's product call.
- **`D5` — default mode sets.** UV `tile scramble drift`, normal `invert green_flip noise`, with
  `swap` / `flat` off until `G-STR` (recommended).
- **`D6` — `m47`.** New materials in the prewarm list only, not in the per-frame pending count, until
  `G298`'s per-anomaly fix (recommended).
- **`D7` — cloned host MIDs.** Allow them, with the frozen-animation limit recorded (recommended), or
  refuse slots holding runtime MIDs.

---

## 11. What this file does NOT claim

- ⛔ **No yield, strength or cost number.** `G7`, `G-STR` and `G-COST` produce them.
- ⛔ **No claim that route B's encoding is correct.** It is derived from source and has to pass `G-ID`.
- ⛔ **No perceptibility claim.** m55 measures pixel change; §5.2's perceptibility column is a
  prediction.
- ⛔ **No incidence claim about the office hosts' content.** The scan covers StackOBot and Lyra only.
- ⛔ **Nothing here changes `m51` (held at `53bf725`), `master`, any tag, any cooked container, or
  CaptureBench.**

---

## Appendix A — the offline content scan (EVIDENCE, not a measurement)

**Method.**

- Read the first 256 KiB of every `.uasset` under the content roots. Extract object-path references
  from the name table (the `m52_texture_sharing_scan.py` method, `tools/m52_texture_sharing_scan.py:13-34`).
- Classify textures with m52's conjunction test.
- For each measured target mesh asset, follow mesh → material → parent chain, and report:
  - the chain's tokens: `BLEND_Masked`, `BLEND_Translucent`, `TwoSided`, `bOverride_TwoSided`,
    `SAMPLERTYPE_Normal`, `WorldAlignedTexture`, `MaterialExpressionVertexColor`, `WorldPositionOffset`,
    `RuntimeVirtualTexture`, `MSM_*`;
  - each reached texture's class. `TC_Normalmap` or a normal `TEXTUREGROUP_*` token means normal;
    `TC_Masks` / `TC_Grayscale` / `TC_HDR` means data.
- Roots:
  - StackOBot `Content` + `/Engine/BasicShapes`;
  - Lyra `Content`, `ShooterCore`, `ShooterMaps`, `LyraExampleContent`.

**Both-ways controls, and what they showed.**

- **Normal classifier:** the flag and the name heuristic agree on 31 of 32 StackOBot normals (20 of 20
  by name on Lyra). **It discriminates.**
- **`VirtualTextureStreaming` token:** 127 / 134 StackOBot textures vs **2 / 188 runtime-virtual**
  (m52 080-04 §5). **BLIND** — it is a registry tag name.
- **`SRGB` token:** present on every texture, including known diffuse maps. **BLIND**, same reason.

**Declared weaknesses** (they travel with every row of §1.2 / §6.1):

1. **Asset defaults, not placed-component overrides.** m52's `CB_GateLevel` lesson (the scan tool's
   weakness 6).
2. **A chain's defaults include textures the instance overrides away.** `T_Weapon_D` is `M_Weapon`'s
   default while `MI_Weapon_Rifle` uses `T_Rifle_D`. `GetUsedTextures` at runtime resolves this; the
   scan does not.
3. Head truncation: 165 StackOBot and 891 Lyra files exceeded 256 KiB.
4. A token is a reference, not proof of a sample.
5. **Nanite status is not read by this scan.** It is quoted from the project's earlier
   `nanite_signature_scan` records (CLAUDE.md 072 / 074 blocks) and from m52's measured
   `target_pixels`.

---

## Appendix B — engine citation index

All paths are relative to `Engine/Source`, UE 5.1.1.

| topic | citation |
|---|---|
| `GetUsedTextures` (UMaterial / MI), uniform-expression source | `Private/Materials/Material.cpp:1069-1158`; `MaterialInstance.cpp:1012-1119`; `MaterialShared.cpp:902-941`; `MaterialShared.h:732` |
| Silent empty list with no shader map | `MaterialShared.cpp:929-935`; `Material.cpp:1073`, `:2310` |
| Parameter resolution through the instance chain | `MaterialUniformExpressions.cpp:1477-1483`; `MaterialInterface.cpp:828-837`; `MaterialInstance.cpp:935-978` |
| `GetAllTextureParameterInfo` (cooked) | `MaterialInterface.h:568`; `MaterialInterface.cpp:985-1010`; `MaterialCachedData.cpp:650-693` |
| Graph / property chain is editor-only | `MaterialInterface.h:766-784`; `Material.cpp:5647-5687`; `Material.h:296-382` |
| `IsPropertyConnected` (cooked) | `Material.h:1739`; `Material.cpp:3961-3964` |
| Texture runtime fields | `Classes/Engine/Texture.h:1298`, `:1310`, `:1327`, `:1357-1359`, `:1720-1724`; `Texture2D.h:55-61`, `:141`, `:147`, `:150`, `:302`; `Texture2D.cpp:1210-1224` |
| Sampler codegen, hardware sRGB | `HLSLMaterialTranslator.cpp:5740-5800`; `[Shaders] Private/MaterialTexture.ush:144-171`; `StreamableTextureResource.cpp:98-113` |
| `UnpackNormalMap` (`.ag` vs `.rg`) | `[Shaders] Private/Common.ush:1373-1384` |
| No runtime sampler / VT validation of overrides | `MaterialInstance.cpp:3408-3439`, `:1209-1292`, `:2773-2776` |
| VT across the boundary ⇒ black | `MaterialShared.cpp:3711-3752`; `MaterialUniformExpressions.cpp:953-1000`, `:1230-1239`; `Texture2D.cpp:1335-1337`; `RHI/Public/RHIResources.h:2012-2025` |
| MID create / parent rules (no MID parent) | `MaterialInstanceDynamic.cpp:67-127`; `MaterialInstance.cpp:779-804`, `:3062-3117` |
| MID never compiles (parent shader map) | `MaterialInstance.cpp:666`, `:1695-1696`, `:2122-2124`, `:238-244` |
| MID setter cost; unknown parameter; null texture | `MaterialInstance.cpp:390-416`, `:3339-3346`, `:3415-3428`; `MaterialShared.cpp:82-87`, `:4000-4044` |
| `CopyParameterOverrides` | `MaterialInstanceDynamic.cpp:484-503` |
| Usage flags → default material in game | `Material.cpp:1660-1823`; `MaterialInstance.cpp:1411-1438`; `StaticMeshRender.cpp:2225-2228`; `SkeletalMesh.cpp:5788-5802`; `InstancedStaticMesh.cpp:1420-1423`; `SplineMeshSceneProxy.cpp:76-79` |
| Nanite audit / fixup / override | `Rendering/NaniteResources.cpp:565-567`, `:1918-2066`; `StaticMeshComponent.cpp:2264-2268`, `:2660-2675`; `MaterialInstance.cpp:1748-1758` |
| Material Time node | `MaterialExpressions.cpp:5738-5742`; `SceneView.cpp:2684-2688`; `Classes/Engine/World.h:4315-4320` |
| MIC overrides honoured; MID forwards to parent | `MaterialInstance.cpp:1598-1641`, `:1987-2095`, `:4121-4164` |
| PSO precaching off; first-draw PSO | `RHI/Private/PipelineStateCache.cpp:67-76`, `:103-110`, `:2067-2094`, `:2131-2139`; `Engine/Private/PSOPrecache.cpp:18-39` |
| Default-material fallback on the render proxy | `MaterialShared.cpp:4207-4229`; `BasePassRendering.cpp:1745-1760` |
| `SetMaterial` / `GetMaterial` / `GetNumMaterials` | `Components/MeshComponent.cpp:30-109`, `:195-198`; `StaticMeshComponent.cpp:2617-2678`; `SkinnedMeshComponent.cpp:1242-1264` |
| Streaming: component info, fallback, cooked-only branch | `StaticMeshComponent.cpp:869-882`, `:1101`; `MeshComponent.cpp:496-514`; `Streaming/TextureStreamingBuild.cpp:643-727` |
| Streaming: `SetMaterial` keeps the static entries | `MeshComponent.cpp:76-82`; `StreamingManagerTexture.cpp:1054-1064`; `DynamicTextureInstanceManager.cpp:183-198` |
| Streaming: per-texture max; wanted mips | `AsyncTextureStreaming.cpp:148-213`; `TextureInstanceView.cpp:510-511`; `StreamingTexture.cpp:188-503` |
| Tiling lowers the needed mips | `MaterialInterface.cpp:1373`; `[Shaders] Private/DebugViewModePixelShader.usf:86-96` |
| Force-resident semantics and cost | `StreamableRenderAsset.h:122-127`, `:161-162`; `StreamableRenderAsset.cpp:218`; `AsyncTextureStreaming.cpp:269-276` |
| `DrawMaterialToRenderTarget` (cooked, no flush) | `Private/KismetRenderingLibrary.cpp:152-217`; `UserInterface/Canvas.cpp:2056-2073`; `UserInterface/CanvasItem.cpp:475-518` |
| Tile VF (one UV); base pass into target 0, no clear | `Public/CanvasTypes.h:1052-1060`; `TileRendering.cpp:89-149`, `:272-291`; `Renderer/Private/Renderer.cpp:125-426` |
| UI domain has no local-VF shaders when cooked | `LocalVertexFactory.cpp:280-289` |
| Emissive written; opaque A = 0; translucent alpha | `[Shaders] Private/BasePassPixelShader.usf:1392`, `:1437-1441`, `:1884-1886`, `:1916`; `[Shaders] Private/LightAccumulator.ush:101`; `BasePassRendering.cpp:261-279` |
| Render-target formats, sRGB, mips | `Classes/Engine/TextureRenderTarget2D.h:18-67`, `:225-247`; `TextureRenderTarget2D.cpp:29-62`, `:583-719`; `D3D12RHI/Private/D3D12Texture.cpp:1407-1410` |
| Render target accepted as a texture parameter | `TextureRenderTarget2D.cpp:81-84`; `MaterialUniformExpressions.cpp:955-1000` |
| `DXT5_NORMALMAPS` source and default | `ShaderCompiler.cpp:6056-6059`; `Core/Private/HAL/ConsoleManager.cpp:2842-2847` |
| Missing TexCoord[1] per VF | `[Shaders] Private/LocalVertexFactory.ush:614-622`; `LocalVertexFactory.cpp:481-499`; `GPUSkinVertexFactory.cpp:482-492`, `:608-627`; `[Shaders] Private/Nanite/NaniteAttributeDecode.ush:390-461` |
| UV-channel count: `GetNumUVChannels` is 0 when cooked; use `GetNumTexCoords` | `StaticMesh.cpp:5150-5162`, `:3443-3451`, `:911-914`; `StaticMesh.h:1525-1530`, `:1747-1750`; `Rendering/SkeletalMeshLODRenderData.h:250-253` |
| CPU vertex data not kept (static) | `StaticMesh.cpp:498-505` |
| Cooked `CachedExpressionData`; MIC/MID return the base parameter list | `Public/MaterialCachedData.h:177-338`; `MaterialInterface.cpp:155-190`; `MaterialInstance.cpp:874-889` |
