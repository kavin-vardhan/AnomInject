# m53 — UV / normal-map texture corruption — PRE-DECLARED DESIGN AND GATES

## REVISION 2 — 082-03, 2026-09-27. This is the live design.

**Branch `feat/m53-uv-normal-corruption`, parent `946c1bf` (revision 1, 082-01). PLAN ONLY.** No plugin
source changed, no build, no cook, no editor or game launch, no bench leg, no tag.

What was executed, all read-only:

- engine source reads, UE 5.1.1 at `D:\UESource\UnrealEngine`;
- plugin source reads of `master` `4283fc8` through git objects (`master` is now `b5f15a3`, one docs-only
  commit later; `git diff --stat 4283fc8 master -- Source` is empty, so every plugin line cited here is
  also `master`'s);
- a second offline scan of content `.uasset` files, reading tagged properties (dimensions and authored
  mip settings). Its method and output are Appendix C.

**Inputs.**

- Codex's design review `_reviews/082-02-codex-m53-design-review.md`: CHANGES REQUIRED, 5 P1, 6 P2, 1 P3.
- Chat's disposition `_reviews/082-03-chat-ruling-codex-m53-design-review.md`: all twelve findings
  accepted; D1's residency and fallback revised, D7 reversed, D2 and D6 amended.

**How to read this file.**

- **§R0** is the finding-by-finding resolution table, with line ranges in this file.
- **§R1–§R16** are the design. They replace revision 1's design in full.
- Revision 1 (`946c1bf`) is kept verbatim at the end, under a SUPERSEDED fold.
  - Its **source facts** stay citable as `v1 §x`. §R0.2 lists exactly which.
  - Its **design** is withdrawn and may not be cited: route A, the v1 refusal table, the snapshot rule,
    the restore, the strength classes, the gate table, the costs, the stages, and D1–D7 as written.
- Citation conventions are v1's (see the v1 header inside the fold):
  - engine paths are relative to `Engine/Source`, and `[Shaders]` means `Engine/Shaders`;
  - a bare engine file name lives under `Runtime/Engine/…`;
  - plugin `file:line` is against `master` `4283fc8`.
- Anything measured later that contradicts this file is a **finding**. It is recorded as an amendment,
  never folded in silently.

---

## R0. Resolution table

### R0.1 Findings

| finding | resolved in | resolution | status |
|---|---|---|---|
| **P1-1** texture-pointer intersection is not an active binding | §R2 (L104–205) | Bindings are read from the uniform-expression set of the game-thread shader map of the resource the slot renders with (world feature level, active quality level, static permutation). A binding is a parameter iff its `ParameterInfo.Name` is not None; a constant use is refused `texture_not_parameter`. The full `FMaterialParameterInfo` (name, association, index) is kept and set with `SetTextureParameterValueByInfo`. `GetUsedTextures` and the pointer intersection are gone. Fixtures: the inactive-parameter / active-constant alias and a material-layer parameter (`G-BIND`). | **RESOLVED** in design; feasibility is `G-BIND`'s first check |
| **P1-2** an identity copy of mip 0 is not an identity texture resource | §R3 (L209–355) | An allowlist of admitted pixel formats replaces the compression denylist. Float, 16-bit, BC6H and LQ formats are refused `unsupported_encoding`. The render target takes the full cooked top-mip dimensions, keeps both dimensions and the aspect, and is never capped or downsampled; if it does not fit, the event is refused `over_budget`. The sampler is matched (LOD group, filter, address), and a host `r.MipMapLODBias` that the render target cannot follow is refused. A qualification matrix covers magnification, minification, grazing angles and mip transitions (`G-ID-M`). **One part cannot be done as ruled:** authored, sharpened or alpha-coverage mip chains are **undetectable** in a cooked build (the settings are editor-only data) and, within Core/CoreUObject/Engine, a render target can only **regenerate** its mips. So "refuse chains that cannot be preserved" cannot be implemented as a refusal. | **PARTIAL → counter-proposal N1** |
| **P1-3** force-residency contradicts isolation; "full" undefined | §R4 (L359–433) | The source must already be resident at its full cooked chain (`NumResidentLODs == MaxNumLODs`, with the resource valid and nothing pending in init or streaming); otherwise `not_fully_resident`. Wait is 0 frames / 0 ms. No force-resident call, no `WaitForStreaming`. `snapshot_mip` and the snapshot dimensions are recorded against the cooked chain. A prefetch phase is a separate, later decision, and only if yield demands it. | **RESOLVED** |
| **P1-4** a host-MID clone loses state; animation exception breaks purity | §R5 (L437–456) | A slot whose raw binding is a MID is refused `host_mid`. m53 builds **no clone path** and adds no clone switch. `host_mid_cloned` is reserved and never emitted. | **RESOLVED** |
| **P1-5** a route-B failure does not justify route A | §R11 (L872–886) | Route A is removed from the design, S1 and `G-ID`. A failure narrows the scope (defer `normal_corruption`) or holds, and any narrowing goes to chat with its evidence. A takeover would be a separate product decision. | **RESOLVED** |
| **P2-6** AlphaComposite must clear before every redraw | §R7.3 (L613–636) | Clear to (0,0,0,1), then draw, then regenerate mips, on **every** draw, including every `drift` frame. The snapshot and the output are distinct render targets. `G-RD` is a repeated identity-redraw gate with opaque and fractional-alpha inputs, and it has a can-fail lever that skips the clear. | **RESOLVED** |
| **P2-7** exact restore needs raw slot state and strong references | §R8 (L693–762) | Per slot, before any change: the raw override, the asset material, the resolved material and the effective (Nanite-substituted) material. Nanite routing is inspected from the resolved material. The displaced originals are held strongly until the restore completes. A slot is restored only while it still holds this event's MID. `G4` is extended with GC, an explicit Nanite override, foreign replacement, component recreation and two live ids. | **RESOLVED** |
| **P2-8** "every map coherently" conflicts with partial eligibility and the cap | §R6.3 (L534–552) | Atomic eligibility per slot: the family's whole required map set is transformable, or the slot is refused with the blocking binding's reason. A cap overflow refuses (`map_set_over_cap`) instead of keeping the 8 largest. The 64 px floor is renamed `below_size_policy` and is a policy, not a claim that small means constant. Small textures inside a qualifying set are transformed. Exclusions are counted per reason. | **RESOLVED** |
| **P2-9** refusal vocabulary: wrong size, not a decision tree | §R6 (L460–585) | One ordered decision tree (event → slot → binding → slot aggregation → event aggregation). Every binding and slot gets one counted disposition, and the event gets exactly one final reason. It covers asset and resource readiness, unsupported encoding, host-MID policy and residency. The shader-map checks come before an empty texture list is read as "no textures". §R6.4 names a fixture or lever for every reachable reason and marks the rest UNEXERCISED. | **RESOLVED** (most fixtures need **N2**) |
| **P2-10** gates can go green without qualifying behaviour | §R12 (L890–967) | Every row is marked **Q** (qualification), **D** (diagnostic) or **O** (owner decision). Each fixture has runnable matched controls: a NoApply null, a `corrupted_texture` positive (no fixture gate), and wrong-copy controls that must fail. `G-ID` runs through both corruptors and every admitted encoding. Functionality gates carry minimum counted applications. `G-LYRA` needs one successful counted event per id to claim Lyra support. `G-COST` has a predeclared comparison with m55 held constant, and its acceptance is an owner decision. | **RESOLVED** (fixture **N2**) |
| **P2-11** shader prewarm is not packaged PSO warmup; a void draw is not validation | §R7.4, §R9 (L638–670, L766–824) | The prewarm-only list is separated from the per-frame check. The Apply transaction is defined: decide → reserve → allocate and verify → re-check every draw precondition → enqueue → bind and read back → commit, with rollback. A non-capturing warm-draw phase runs before the lead-in. The cold first fire is **measured** for both corruptors and every format, and its acceptance is an owner decision. | **RESOLVED**; cold-first-fire acceptance is an owner decision |
| **P3-12** `strength_class` can only be a mode prior | §R10 (L828–868) | Renamed `texcorrupt.expected_strength_class` and documented as a mode prior, not a per-event measurement. v1's universal statements are withdrawn. m55 stays out of eligibility, ranking, seeds, verdicts and `observable`. | **RESOLVED** |

**Chat's revised rulings.** D1 → §R1, §R4, §R11 · D2 → §R7.5 · D3 → §R8.4 · D4 → §R12, §R14 ·
D5 → §R12 (`G-RD`, `G-COST`) · D6 → §R9.1–§R9.2 · D7 → §R5.

### R0.2 Revision-1 text that stays citable (source facts, not design)

These v1 sections record what the engine and the content do. They are unaffected by the review and may be
cited as `v1 §x`:

- v1 §1.1–§1.3 — why a takeover reconstructs rather than corrupts;
- v1 §2.2 — which texture fields a cooked build can read;
- v1 §3.1.1 — TexCoord[1] per vertex factory, and why `GetNumUVChannels` returns 0 when cooked;
- v1 §3.2's **draw facts only**: the render-target draw path, Surface domain not UI, Unlit, opaque writes
  A = 0, and the sRGB / normal encoding table rows;
- v1 §3.3's cook and office-host facts (hard references, editor binaries for the cook, runtime-load proof);
- v1 §3.4's **streaming facts only** (not its snapshot rule);
- v1 §4.4 (the target watch) and v1 §4.5's facts about m52 sharing;
- v1 §7.1–§7.4 (two ids, the mode draw, the `G150` precision, no dashboard change);
- v1 Appendix A (the name-table scan) and Appendix B (the citation index).

Everything else inside the fold is withdrawn.

---

## R1. The rulings as applied

- **D1 (revised): route B only, provisional.**
  - The host material is kept. Corrupted copies of its **active texture parameters** are drawn into
    render targets and bound with `…ByInfo` on a MID of the host's own material.
  - Residency is **already resident, or refuse**, with 0 frames / 0 ms of wait (§R4).
  - There is **no automatic route-A fallback** (§R11). Route A survives only inside the SUPERSEDED fold.
- **D2 (amended):** `uv_scramble` is a seeded **bijective** permutation of K×K cells (§R7.5). `channel1`
  is removed along with route A.
- **D3:** two ids, `uv_corruption` and `normal_corruption`, with state, cleanup and counters isolated per
  registration (§R8.4).
- **D4:** both ids are in `GAutoPool`, default OFF. An all-refused census is a yield result, never a
  support claim (§R12, §R14).
- **D5 (provisional):** defaults are UV `tile scramble drift` and normal `invert green_flip noise`.
  `drift` stays conditional on `G-RD` (repeated-draw fidelity) and `G-COST`.
- **D6 (amended):** the prewarm-only list is separated from the per-frame readiness check (§R9.1).
  Shader completeness is not packaged PSO readiness (§R9.2).
- **D7 (reversed):** slots holding host MIDs are refused, and in m53 unconditionally (§R5).
  `host_mid_cloned` is reserved for a future state-following milestone.

---

## R2. Active texture bindings (P1-1)

### R2.1 Where the binding list comes from

For each slot, the **resolved** material `M` is used: the raw override if it is non-null, otherwise the
mesh asset's slot material (§R8.1). The Nanite-substituted effective material is never used; a slot where
it differs has already been refused (§R6, step S4).

1. **The resource the slot renders with:** `Res = M->GetMaterialResource(World->FeatureLevel,
   EMaterialQualityLevel::Num)`.
   - `Num` resolves to the **active** quality level, `GetCachedScalabilityCVars().MaterialQualityLevel`
     (`Material.cpp:2614-2630`; `UnrealEngine.h:461`).
   - An MIC with a static permutation returns its own `StaticPermutationMaterialResources`; otherwise
     the call forwards to the parent (`MaterialInstance.cpp:1684-1697`). This is the same choice the
     render thread makes (`MaterialInstance.cpp:223-248`).
   - A runtime MID never has its own static permutation in a cooked build
     (`MaterialInstance.cpp:666`; set only under editor guards at `:1971-1974`, `:3551-3669`), so it
     forwards to its parent.
   - This uses the **world's** feature level. v1's `GetUsedTextures(…, Num)` path used
     `GMaxRHIFeatureLevel` instead (`Material.cpp:1080-1093`).
2. **Its game-thread shader map:** `SM = Res->GetGameThreadShaderMap()` (`MaterialShared.h:2097-2101`).
   - `Res == nullptr` or `SM == nullptr` ⇒ refuse `shader_map_unavailable`.
   - `!Res->IsGameThreadShaderMapComplete()` (`MaterialShared.h:2109-2113`) ⇒ refuse
     `shader_map_incomplete`. In a cooked build such a material draws through the render-thread
     fallback chain towards the default material (`MaterialShared.cpp:4207-4229`), so any statement
     about "its textures" would describe a material that is not on screen.
3. **The binding list:** `Set = SM->GetUniformExpressionSet()` (`MaterialShared.h:1399`). For each
   `EMaterialTextureParameterType` (`MaterialShared.h:468-478`) and each `i < Set.GetNumTextures(Type)`,
   the entry is `Set.GetTextureParameter(Type, i)` (`:603`, `:605`).
   - This is `LAYOUT_FIELD` data of the cooked shader map (`MaterialShared.h:647`, `:732`, `:1130`).
     Only debug strings and sample-count estimates are editor-only (`:738-751`, `:1134-1137`).
   - **Static-switch-off branches are absent**, because the unselected input is never compiled
     (`MaterialExpressions.cpp:9324-9332`; `StaticSwitchParameter` `:9077-9109`; `QualitySwitch`
     `:9507-9533`).
   - v1 claimed this property for `GetUsedTextures`, which reads the same data. The defect was
     elsewhere: v1 kept only the texture **pointers** and then intersected them with the parameter list,
     which threw away which entry was a parameter.

### R2.2 Parameter or constant

Each entry is an `FMaterialTextureParameterInfo` (`MaterialShared.h:481-502`): `ParameterInfo` (name,
association, index), `TextureIndex`, `SamplerSource`, `VirtualTextureLayerIndex`.

- **A constant texture sample never sets `ParameterInfo`.** Its name stays None
  (`MaterialUniformExpressions.cpp:1399-1404`; the entry is default-constructed at
  `HLSLMaterialTranslator.cpp:1345`; the default is `MaterialTypes.h:96`). The runtime itself reads None
  as "use the referenced texture" (`MaterialUniformExpressions.cpp:1479`).
- **A texture parameter carries its name, its association (`GlobalParameter`, `LayerParameter`,
  `BlendParameter`) and its layer or blend index** (`HLSLMaterialTranslator.cpp:6781-6807`;
  `HLSLMaterialTranslator.h:689-693`).
- **The binding's current texture** is read with the engine's own game-thread resolver,
  `Set.GetGameThreadTextureValue(Type, i, M, *Res, OutTex)` (`MaterialShared.h:606`;
  `MaterialUniformExpressions.cpp:618-634`, `:1477-1483`). It returns what the renderer will sample:
  the instance chain's override for a parameter, the referenced texture for a constant.
- **Everything is keyed by the entry, never by the texture pointer.** One texture object can appear
  under a constant entry and a parameter entry in the same set.
- **Codex's alias case therefore resolves by construction.** An inactive parameter P defaulting to T,
  next to an active constant use of T, produces **no entry for P at all** (P's branch is not compiled).
  T is seen only as a constant and is refused `texture_not_parameter`.
- ⚠ **Named residual, not solved.** An entry means "a texture chunk was accessed during translation"
  (`HLSLMaterialTranslator.cpp:3333-3374`, `:1340-1347`), not "the texel reaches an output". A compiled
  read whose result is multiplied by zero, for example, is still an entry. The design cannot see that
  case. `G-BIND`'s dead-binding fixture reports what it does, and on host content such an event reads as
  no measured change in m55 (a `G-STR` reading, never a pass).

### R2.3 The override

- `FMaterialParameterInfo Info(Entry.ParameterInfo)` (explicit constructor, `MaterialTypes.h:56`,
  `:127-132`). Names are real `FName`s in a cooked build: frozen script names are patched from
  serialised names at load (`Core/Private/Serialization/MemoryImage.cpp:1597-1610`).
- `HostMid->SetTextureParameterValueByInfo(Info, Rt)` (`MaterialInstanceDynamic.h:67`, `.cpp:237-249`).
  A layer parameter is addressed by (name, association, index), so it is set on its own layer and never
  confused with a global of the same name.
- **Read-back:** `HostMid->GetTextureParameterValue(Info, Out)` (`MaterialInterface.h:802`) must return
  `Rt`, else the transaction rolls back (`param_readback_mismatch`, §R7.4).
  - ⚠ **Necessary, not sufficient.** It proves the MID stores the override, not that the shader samples
    it. That proof comes from `G-BIND`, `G-ID`, `G1` and `G2`.

### R2.4 Which entries are candidates

- Only `Standard2D` entries whose current texture is a `UTexture2D`.
- `Cube`, `Array2D`, `ArrayCube`, `Volume` ⇒ `unsupported_type`.
- `Virtual` entries, and `Standard2D` entries whose texture `IsCurrentlyVirtualTextured()`
  (`Texture2D.cpp:1210-1224`) ⇒ `virtual_texture`.
- **Runtime virtual textures are not in this set** (they have their own parameter array) and are never
  touched. With no clone path (§R5), the host's RVT overrides stay on the host's material by construction.

### R2.5 Module boundary

Every symbol used above is `ENGINE_API`, inline in an Engine header, or a virtual call through an Engine
class:

- `GetMaterialResource` is virtual; `GetGameThreadShaderMap` and `IsGameThreadShaderMapComplete` are
  inline; `FUniformExpressionSet::GetTextureParameter` and `GetNumTextures` are inline;
  `GetGameThreadTextureValue` is `ENGINE_API`.
- `FMaterialShaderMap::GetUniformExpressionSet` is inline and calls `GetContent()`, which is inline in
  `RenderCore/Public/Shader.h:2243`. That is a **header-only** use of a RenderCore type through Engine's
  public include path; it links no RenderCore symbol.

⚠ **Stated as a check, not an assumption.** `G-BIND`'s first check is that both build targets link with
the module's declared dependencies unchanged (Core, CoreUObject, Engine, InputCore, Foliage). If they do
not, S1 stops and the dependency question goes to chat, together with N1.

---

## R3. The texture contract: format, dimensions, sampling, mips, memory (P1-2, Q6)

### R3.1 Admitted encodings

`GetPixelFormat()` is the only cooked fact about the encoding (`Texture2D.cpp:376-389`). The choice from
`CompressionSettings` to a pixel format is made at cook time (`Texture.cpp:2995-3342`, entirely inside
`#if WITH_EDITOR` `:3005-3339`). So the rule is an **allowlist on the pixel format**, default deny:

| source pixel format | class | render-target format |
|---|---|---|
| `PF_DXT1`, `PF_DXT5`, `PF_BC7`, `PF_B8G8R8A8`, with `SRGB` true | colour | `RTF_RGBA8_SRGB` |
| the same formats with `SRGB` false; `PF_BC4`; `PF_G8` | data | `RTF_RGBA8` |
| `PF_BC5` with `IsNormalMap()` | normal (`.rg` decode) | `RTF_RGBA8` |
| `PF_BC5` without `IsNormalMap()` | data (two channels) | `RTF_RGBA8` |

- **Refused `unsupported_encoding`, naming the format:**
  - `PF_FloatRGBA`, `PF_R16F`, `PF_R32_FLOAT`, `PF_A32B32G32R32F`, `PF_BC6H`. These come from
    `TC_HDR`, `TC_HDR_F32`, `TC_HalfFloat`, `TC_SingleFloat` and `TC_HDR_Compressed` (`Texture.cpp:3227`,
    `:3231`, `:3265`, `:3273`, `:3277`). Their range and precision do not survive RGBA8. v1's denylist
    missed the half and single float settings (Codex P1-2).
  - `PF_G16` (16-bit grayscale, `:3241-3254`): precision is lost.
  - `PF_R5G6B5_UNORM`, `PF_B5G5R5A1_UNORM` (`TC_LQ`, `:3213-3224`): not qualified by any fixture.
  - A normal map in any format other than `PF_BC5`: not qualified by any fixture.
  - Anything not in the table.
- **sRGB grayscale** cooks to BGRA8 on Windows, because Windows does not support sRGB grayscale
  (`Core/Public/Windows/WindowsPlatformProperties.h:101-104`; `Texture.cpp:3305-3308`). It is covered by
  the BGRA8 colour row.
- **Single-channel formats (`PF_BC4`, `PF_G8`)** are read by the host through a grayscale or alpha
  sampler type, which replicates one channel of the lookup. The render target stores the corruptor's raw
  sample in RGBA8, so whether the host's replication reads the same value from it is not derived here;
  G-ID's BC4 and G8 rows prove it or refuse the class.
- **The contract in one line:** every admitted source decodes to values the render-target format holds
  without range loss. BC interpolants can decode to values between 8-bit steps; the render target stores
  the nearest 8-bit value. That single re-quantisation is what `G-ID`'s tolerance has to bound (§R12, N4).
- `Compat.UseDXT5NormalMaps` non-zero refuses the normal family `dxt5_normal_host`. This is v1's rule,
  kept; the variable is read at runtime (v1 §3.2 encoding table).

### R3.2 Dimensions and memory

- **Size.** The render target is **W×H = the dimensions of the platform's top mip**,
  `PlatformData->Mips[AssetLODBias]` (§R4). On a normally cooked texture that equals `GetSizeX()` ×
  `GetSizeY()`, the full cooked mip-0 size after cook-time stripping (`Texture2D.cpp:327-355`;
  `TextureDerivedData.cpp:2661-2667`).
  - Both dimensions and the aspect are kept. **There is no cap and no resize.** v1's
    `TexCorruptRtMaxSize = 1024` is deleted.
- **Mips.** `bAutoGenerateMips` gives `FloorLog2(max(W, H)) + 1` levels (`TextureRenderTarget2D.cpp:50-62`).
  Nothing in the render-target or mip-generation path rejects non-square or non-power-of-two sizes.
- **Bytes per render target:** `B(W,H) = Σ_i 4 · max(1, W≫i) · max(1, H≫i)` over those levels, computed by
  our own arithmetic. The engine's `CalcTextureMemorySizeEnum` counts only the base level
  (`TextureRenderTarget2D.cpp:68-79`). Reference values:

  | size | bytes | MiB |
  |---|---|---|
  | 256² | 349,524 | 0.33 |
  | 1024² | 5,592,404 | 5.33 |
  | 2048² | 22,369,620 | 21.33 |
  | 4096² | 89,478,484 | 85.33 |

- **`drift` needs two render targets per texture**: the Apply-time snapshot and the per-frame output
  (§R7.3), so `2·B`.
- **The budget is a run-wide cap on live m53 render-target bytes**, both ids together:
  `IAI.Anomaly.TexCorruptMaxRtBytes`, compiled **64 MiB**, in the `AnomalyDefaults` pattern (console >
  ini > compiled, a `G139` echo, out-of-range values refused rather than clamped).
  - At Apply the event **reserves** its whole requirement before allocating anything: every required
    output of every qualifying slot, including `drift` snapshots. If it does not fit, the event is
    refused `over_budget` (§R6, step V2).
  - Released bytes stay counted as `pending_release` until two frames after the release is enqueued.
    That is a bookkeeping rule. GPU deallocation timing is not observable from the game thread, and the
    cap is **not a VRAM guarantee**.
  - `texcorrupt_rt_bytes_peak` reports live plus pending bytes.
- ⚠ **64 MiB is v1's number, chosen when v1 capped the render target at 1024.** Without downsampling it
  refuses every 4096² source outright (one map alone is 85.33 MiB), and every slot whose UV map set
  exceeds three 2048² maps. §R14 shows what that costs on the fixtures. **The value is N3.**

### R3.3 Sampling

- **How each sampler is built:**
  - The render target's sampler comes from **its own** `LODGroup` and `Filter` through the active device
    profile, with MipBias 0 and anisotropy from `r.MaxAnisotropy` (`TextureRenderTarget2D.cpp:647-655`).
  - An ordinary texture's sampler uses the same device-profile filter lookup
    (`Private/Rendering/StreamableTextureResource.cpp:101`; `TextureLODSettings.cpp:358-382`), the same
    `r.MaxAnisotropy` (`RHI/Public/RHIUtilities.h:844-850`), its own `AddressX/Y`
    (`Private/Rendering/Texture2DResource.cpp:81-82`), and **MipBias = `r.MipMapLODBias`**
    (`Private/Rendering/Texture2DResource.cpp:74-83`; `Texture2D.cpp:88-92`, `:1189-1193`).
- **Matched by construction.** Before the resource is created (`InitCustomFormat`,
  `TextureRenderTarget2D.h:150`), the render target gets:
  - `LODGroup = Src->LODGroup` and `Filter = Src->Filter`, so filter and anisotropy match;
  - `AddressX/Y = Src->AddressX/Y` and `MipsAddressU/V = Src->AddressX/Y`.
  - The sampler is rebuilt only when the resource is initialised (`TextureRenderTarget2D.cpp:647-655`),
    which is why these are set first.
- **MipBias cannot be matched.** The render-target sampler has none, and the engine's sampler refresh
  skips render targets (`UnrealEngine.cpp:1016-1057`). ⇒ At Apply, `UTexture2D::GetGlobalMipMapLODBias()`
  (`Texture2D.h:233`) non-zero refuses the event **`host_mip_bias`**. The default is 0; no plugin or bench
  script sets it (`git grep`, 4283fc8 and CaptureBench).
- **Shared samplers.** A material sampling through `SSM_Wrap_WorldGroupSettings` / `Clamp` ignores the
  texture's sampler for the render target and the source alike (`SceneManagement.cpp:901-913`;
  `MaterialUniformExpressions.cpp:963-975`). Equal treatment, nothing to match.
- **sRGB.** Set from `Src->SRGB` through the render-target format before creation (v1 §3.2 facts).

### R3.4 Mips: what can be preserved, and what cannot

- **The render target's lower mips are regenerated after every draw.** `DrawMaterialToRenderTarget` ends
  with `UpdateResourceImmediate(false)` (`KismetRenderingLibrary.cpp:214`), which runs
  `UpdateDeferredResource` → `FGenerateMips` (`TextureRenderTarget2D.cpp:682-719`).
  - On D3D12 each level is one bilinear `SampleLevel` from the level above
    (`RenderCore/Private/GenerateMips.cpp:201-219`; `[Shaders] Private/ComputeGenerateMips.usf:23-40`):
    a 2×2 box for even sizes, an approximation for odd ones.
  - On D3D11 it is the driver's `GenerateMips` (`Runtime/Windows/D3D11RHI/Private/D3D11Texture.cpp:1134`).
- **The source's lower mips were made at cook time** by its `MipGenSettings` (box, sharpen N, blur N,
  leave existing) and optionally by alpha-coverage scaling.
  - **All of that is editor-only data** (`Classes/Engine/Texture.h:1117-1275`: `MipGenSettings` `:1247`,
    `bDoScaleMipsForAlphaCoverage` `:1207`, `AlphaCoverageThresholds` `:1211`, `MaxTextureSize` `:1191`).
  - The device-profile accessor for a group's mip setting is editor-only too
    (`Classes/Engine/TextureLODSettings.h:179-182`).
  - ⇒ **A cooked build cannot tell a box chain from an authored one.**
- **How common authored chains are** (Appendix C; offline, from the editor assets):
  - StackOBot: **8 of 131** textures, including `T_Eyes_Atlas` on the Bot's face (`LeaveExistingMips`).
  - Lyra: **22 of 324**, including **every Manny and Quinn diffuse, mask and normal** (`Sharpen1` /
    `Sharpen2`).
  - No texture on either fixture uses alpha-coverage scaling.
- ⇒ **Chat's instruction "refuse authored lower-mip or alpha-coverage chains that cannot be preserved"
  cannot be implemented as a refusal** inside Core/CoreUObject/Engine. This is **counter-proposal N1**
  (§R15):
  - **(a) Preserve by construction.** Declare RenderCore and RHI dependencies on `AnomalyInjector` (the
    invariant already lists them as anticipated) and draw **each output mip from the source's own mip m**
    into a scratch render target, copied into mip m of the output. Every authored chain is carried
    through, and nothing needs detecting.
  - **(b) Stay inside Engine and narrow the claim.** Admit, regenerate mips, disclose it, and qualify
    only box-chain fixtures. This accepts an undetectable failure class.
  - **(c) Hold the anomaly.**
  - **Recommended: (a).** The design below is written for (b)'s mechanism, because it is the one the
    current dependency set allows. `G-ID-M` reports which fixtures are box-chain, and the support claim
    is restricted to them. **S1 does not start until N1 is ruled**, because (a) changes the render-target
    module.
- **Transformed outputs have their own limit (Codex P1-2), named rather than solved.** Tiling ×N baked
  into a W×H render target stores each tile at `log2(N)` mips less detail. The corruptor's
  derivative-based sampling picks exactly that source mip, which is what a real ×N UV bug fetches at the
  same screen footprint. **Under magnification** a real UV bug would show sharper tiles than the baked
  target can hold. Recorded per event as `texcorrupt.tile_detail_mips = log2(N)`.

### R3.5 What qualifies the contract

`G-ID-M` (§R12) runs the identity comparison on dedicated geometry in the synthetic fixture (N2): a
target filling the view (magnification), a far target (minification), a grazing-angle plane
(anisotropic), and geometry that crosses mip transitions. It does **not** use `r.MipMapLODBias` to force
minification: that would refuse the event (§R3.3). On host fixtures, `G-ID-M` reads only what the bench
pose shows.

---

## R4. Residency (P1-3)

**Policy.** At Apply, every texture in the event's required set must already be resident at its full
cooked chain. Otherwise the event is refused `not_fully_resident`.

- **Wait: 0 frames and 0 ms.**
- **Never** `SetForceMipLevelsToBeResident`, `bForceMiplevelsToBeResident`, `WaitForStreaming` or
  `WaitForPendingInitOrStreaming`. The last two loop on `FlushRenderingCommands` and `Sleep` with no
  timeout (`StreamableRenderAsset.cpp:334-368`), and they guarantee only that nothing is pending, not full
  residency.
- **A prefetch phase is a separate design decision**, raised only if `G7`'s yield demands it. Codex's
  conditions travel with it (a non-blocking phase outside labelled capture, frame **and** wall-time
  cutoffs, identical preparation in clean controls, ownership of changed residency settings, m52
  exclusion throughout, cleanup on timeout, cancel and EndPlay, and never a labelled event while it is
  pending).

**The check.** Game thread, all public or inline:

1. `Tex->GetResource() != nullptr` and `!Tex->HasPendingRenderResourceInitialization()`
   (`Texture.h:1667-1668`; `Texture.cpp:1121-1124`). Else `resource_not_ready`.
2. `!Tex->HasPendingInitOrStreaming(false)` (`StreamableRenderAsset.cpp:229-265`). Else
   `streaming_pending`.
   - It clears the engine's own cached init hint on the game thread (`:239-249`). That is the engine
     recording an observed fact; residency does not change.
3. `S = Tex->GetStreamableResourceState()` (`StreamableRenderAsset.h:174-177`) must be valid. Else
   `resource_not_ready`. (An invalid state also describes virtual-texture resources, which step T2 has
   already refused.)
4. **`S.NumResidentLODs == S.MaxNumLODs`:** every mip the resource can hold is resident. Else
   `not_fully_resident`, and the diagnostic line gives `resident_lods`, `max_lods` and the resident top
   dimensions.
   - `MaxNumLODs` already excludes the cook-stripped and runtime non-cinematic bias, and
     `AssetLODBias + MaxNumLODs == Mips.Num()` for a `UTexture2D` (`Texture.cpp:1400-1522`). So this
     compares against the chain the platform can show.
   - **Optional mips.** If the top mips are optional and not mounted, `NumResidentLODs` can never reach
     `MaxNumLODs` (the streamer clamps to `NumNonOptionalLODs`, `Private/Streaming/StreamingTexture.cpp:224-229`).
     The event is refused `not_fully_resident` with the sub-reason `optional_unmounted`: an unmounted mip
     cannot be restored at runtime.
   - **Cinematic mips** (`NumCinematicMipLevels`, `StreamableRenderAsset.h:256`) are inside `MaxNumLODs`,
     so a texture with cinematic mips is refused unless they are resident. Stricter than necessary;
     stated.
   - **Non-streaming resources** (`!S.bSupportsStreaming`) pass when fully loaded, because their resident
     count is set at init (`Texture.cpp:1468-1500`). A non-streaming texture whose optional first mip is
     missing has `NumResidentLODs = NumNonOptionalLODs < MaxNumLODs` and is refused, correctly.
   - ⛔ **Not `IsFullyStreamedIn()`.** It returns true for an invalid state and for every non-streaming
     resource whatever was dropped, and it subtracts a non-cinematic runtime bias twice
     (`StreamableRenderAsset.cpp:318-332`).
5. **Top-mip dimensions.** `AssetMip = S.LODCountToAssetFirstLODIdx(S.NumResidentLODs)`
   (`Public/Streaming/StreamableRenderResourceState.h:69-72`), and `PlatformData->Mips[AssetMip].SizeX/SizeY`
   (`Public/TextureResource.h:44-89`).
   - Recorded as **`snapshot_mip`** (the asset mip index relative to the cooked chain; 0 on a normal
     cooked texture) and **`snapshot_px`** = [W, H]. The render target takes these dimensions (§R3.2).
   - A texture whose runtime `AssetLODBias` is above 0 (a device-profile `MaxLODSize` tighter than the
     cook) is admitted with `snapshot_mip = AssetLODBias`: that **is** the platform's full chain.
   - ⚠ `FTexturePlatformData::GetNumNonOptionalMips` and its neighbours are not `ENGINE_API`, so the
     design uses the state struct's `NumNonOptionalLODs` instead.

**Why nothing can race the check.** Apply is one synchronous game-thread call. The residency check, the
m52 check (below) and the draw's enqueue all happen inside it. A stream-out already in flight would have
failed step 2. Anything the streamer requests afterwards is enqueued after our draw on the same render
command queue (§R7.4). `drift` redraws only from the snapshot, never from the source (§R7.3).

**m52 interplay.** Before the residency check, a new additive query
`AnomalyStuckMip::IsTextureHeldOrRestoring(const UTexture2D*)` covers **both** m52's held list and its
restoring list, texture-wide. It is needed because no such public query exists today: the private
`IsAwaitingRestore` checks only the restoring list (`Anomaly_StuckLowMip.cpp:768-782`), and the public
`IsAnomalyCurrentlyAnomalous` returns false once the anomaly is inactive (`AnomalyInjectorSubsystem.cpp:800-808`).
A hit refuses the binding `held_by_stuck_low_mip`. There is no preparation phase, so there is no window in
which a new m52 hold could start between the check and the draw. The reverse direction is v1 §4.5's (m52's
auto-pool shared-user gate), and a **targeted** m52 fire still bypasses that gate by design; that is not
evidence of isolation.

**Measured consequence (m52's banked readings).** On the StackOBot bench poses the rock textures sat at
resident **11–12 of 13** mips, i.e. a top resident mip of 1024–2048 px, never 4096
(`2026-09-20-080-02-m52-implemented-and-gated.md:51`; `…080-03…:97`). Both rocks are therefore refused
`not_fully_resident` whatever the budget (§R14).

---

## R5. Host MIDs (P1-4, D7)

- A slot whose **raw** binding (§R8.1) is a `UMaterialInstanceDynamic` is refused **`host_mid`**.
- **m53 builds no clone path.**
  - `CopyParameterOverrides` first clears the MID's parameters, runtime-virtual-texture parameters
    included, and never copies RVT parameters back (`MaterialInstanceDynamic.cpp:484-503`;
    `MaterialInstance.cpp:3502-3519`).
  - A clone also stops following the host's later writes to its own MID (a hit flash, animated
    roughness).
  - Both change the picture beyond the named corruption.
- **Setting the parameter on the host's own MID is refused too.** It would mutate a host-owned object that
  anything else in the game may point at, and the restore could not tell which later host writes to undo.
- A MID cannot parent another MID; it renders the default material (v1 §1.4, `MaterialInstance.cpp:3076-3083`).
- **`IAI.Anomaly.TexCorruptCloneHostMids` is not added** (v1 proposed it). `texcorrupt.host_mid_cloned` is
  reserved for a future state-following milestone and **never emitted** by m53.
- **Conditions for that future milestone** (chat's): copy every override type including RVT, define a
  policy for later host updates, and qualify with an RVT-override identity fixture and a host
  parameter-animation fixture.
- **Content:** the StackOBot Bot's slots hold runtime MIDs (`G46`), so the Bot is refused. Whether Lyra's
  characters do is not known offline; `G0` reads it.

---

## R6. The decision tree (P2-9) and the atomic map set (P2-8)

### R6.1 Rules of the tree

- The steps run in a fixed order: event level, then per slot, then per texture binding, then slot
  aggregation, then event aggregation. The first failing step decides.
- **No step has a side effect.** The first side effect, the budget reservation, happens only after the
  tree says APPLY.
- Every texture binding and every slot gets **exactly one** counted disposition.
- The event gets **exactly one** final outcome:
  - `APPLIED`, or
  - `REFUSED` with one reason: the first failing event-level step; or, when every slot is refused, the
    reason of the **earliest step** among the slots' reasons. That makes the reason deterministic.
- A slot is a (component, slot index) pair on every mesh component the target resolves.

### R6.2 The tree

**Event level**

| step | check | reason |
|---|---|---|
| E1 | both corruptor materials and the noise texture resolved non-null (subsystem CDO hard references, v1 §3.3) | `assets_unavailable` — an installation failure, never a content fact |
| E2 | the corruptor this id needs has a complete game-thread shader map at the world's feature level | `corruptor_not_ready` |
| E3 | normal family: `Compat.UseDXT5NormalMaps` reads 0 | `dxt5_normal_host` |
| E4 | `r.MipMapLODBias` reads 0 (§R3.3) | `host_mip_bias` |
| E5 | targeted fire: the requested mode is one of the id's modes | `mode_invalid` |
| E6 | the target resolves ≥ 1 static or skinned mesh component (`AnomalyLod::ResolveLodComponents`, viewport scoping as the other texture anomalies) | `no_mesh` |

**Slot level**, for every slot:

| step | check | disposition |
|---|---|---|
| S1 | the resolved material is non-null | `slot_empty` — intentionally untouched |
| S2 | the resolved material is not translucent | `slot_translucent` — intentionally untouched |
| S3 | the raw binding is not a MID (§R5) | `host_mid` |
| S4 | static mesh component: not (`UseNaniteOverrideMaterials()` and `Resolved->GetNaniteOverride() != nullptr`) (`StaticMeshComponent.cpp:2264-2268`, `:2670-2675`; `MaterialInterface.h:510`) | `nanite_override` |
| S5 | `GetMaterialResource(FL)` and its game-thread shader map are non-null (§R2.1) | `shader_map_unavailable` |
| S6 | that shader map is complete | `shader_map_incomplete` |
| S7 | the root material carries the usage flag this component class needs, read side-effect-free with `UMaterial::NeedsSetMaterialUsage_Concurrent` + `GetUsageByFlag` (`Material.h:1235`, `:1252`; `Material.cpp:1705-1732`). Checked: skeletal → `MATUSAGE_SkeletalMesh`; instanced static → `MATUSAGE_InstancedStaticMeshes`; Nanite static → `MATUSAGE_Nanite`; lightmapped static → `MATUSAGE_StaticLighting`. | `default_material_path` — the slot already renders the default material in game (`Material.cpp:1790-1820`; `StaticMeshRender.cpp:2225-2227`), so its textures are not on screen |
| S8 | the uniform-expression set has ≥ 1 texture entry | `no_textures` — **now meaning** "the compiled material samples no texture"; S5–S6 have already excluded the silent empty list of `MaterialShared.cpp:926-941` |

**Texture-binding level**, for every entry of the set (§R2):

| step | check | disposition |
|---|---|---|
| T1 | `Standard2D` entry and a `UTexture2D` texture | `unsupported_type` |
| T2 | not virtual (entry type, and `IsCurrentlyVirtualTextured()`) | `virtual_texture` |
| T3 | `LODGroup` not in {UI, Lightmap, Shadowmap, Terrain_Heightmap, Terrain_Weightmap, Bokeh} (m52's list, `Anomaly_StuckLowMip.cpp:51-65`) | `excluded_group` — intentionally untouched |
| T4 | `ParameterInfo.Name` is not None | `texture_not_parameter` |
| T5 | pixel format admitted for its class (§R3.1) | `unsupported_encoding` |
| T6 | not held or restoring by `stuck_low_mip` (§R4) | `held_by_stuck_low_mip` |
| T7 | resource valid, initialised, nothing pending (§R4 steps 1–3) | `resource_not_ready` / `streaming_pending` |
| T8 | fully resident (§R4 step 4) | `not_fully_resident` |
| T9 | all passed | `transformable` |

**Slot aggregation**

| step | check | slot reason |
|---|---|---|
| A1 | the family's **required set**. UV: every binding except the intentionally untouched ones (T3). Normal: every binding whose texture `IsNormalMap()`, except T3. | — |
| A2 | normal family: the required set is non-empty | `no_normal_map` |
| A3 | normal family: the root material's `IsPropertyConnected(MP_Normal)` (`Material.cpp:3961-3964`; a guard only, v1 §2.2) | `normal_unconnected` |
| A4 | **every** binding in the required set is `transformable` | the **first** failing binding's reason, by step order. The diagnostic counter `slots_partial_set` also counts slots where some bindings were transformable and others not. |
| A5 | the required set holds ≥ 1 texture at or above the size policy on both axes (`IAI.Anomaly.TexCorruptMinTexturePx`, compiled 64) | `below_size_policy` |
| A6 | the required set has ≤ `IAI.Anomaly.TexCorruptMaxTextures` bindings (compiled 8) | `map_set_over_cap` |

**Event aggregation**

| step | check | reason |
|---|---|---|
| V1 | ≥ 1 slot qualified | the earliest-step slot reason; if every slot was intentionally untouched, `no_eligible_slot` |
| V2 | the whole requirement fits the run-wide cap (§R3.2) | `over_budget` |
| V3 | the transaction completes (§R7.4) | `rt_alloc_failed` / `draw_precondition_failed` / `param_readback_mismatch` (each rolls back) |

### R6.3 The atomic map set, stated plainly

- **Within a slot, the transformation is all or nothing.** A UV family event either moves every map the
  slot's compiled material samples (except intentionally untouched groups), or it does not touch the
  slot. A normal family event corrupts every normal-map binding of the slot, or none.
- **A constant or virtual map in the set blocks the slot** (T4, T2). This is Codex's example: an albedo
  parameter next to a constant-node normal map, or next to a virtual-textured detail map, used to be
  partly transformed; now the slot is refused.
- **The cap refuses; it does not trim** (A6). v1's "keep the 8 largest" is withdrawn.
- **The size floor is a policy** (A5). It asks that an event have at least one texture of meaningful size.
  It does not claim that a small texture is constant. **Small textures inside a qualifying set are
  transformed with the rest** (they cost almost nothing), so a small patterned mask moves with its slot.
- **Between slots, partiality is allowed and labelled.** A slot refused for any reason is left untouched,
  and the label records `slots_corrupted` / `slots_total` plus each untouched slot's reason. This is
  deliberate: m53 is a per-material texture corruption, and a real texture bug is per material. Chat may
  tighten this to event level; it is a stated choice within the ruling ("the slot or event is refused"),
  not a counter-proposal.
- **Counting.** Every binding disposition and every slot reason is counted per run (§R10). Exclusions are
  deterministic because the tree is ordered and has no side effects.

### R6.4 Every reason, and what exercises it

| reason | how it is produced | stage |
|---|---|---|
| `assets_unavailable` | bench lever `IAI.Bench.TexCorruptForceMissingAsset` (nulls the pointer for one fire) | S1 |
| `corruptor_not_ready` | **UNEXERCISED** in a healthy packaged cook; the per-fire check is still built | — |
| `dxt5_normal_host` | **UNEXERCISED**: the variable is read-only at runtime (`Core/Private/HAL/ConsoleManager.cpp:2842-2852`) and changing it changes shader compilation; the bench reads 0 | — |
| `host_mip_bias` | `r.MipMapLODBias 1` before a fire | S1 |
| `mode_invalid` | targeted fire with an unknown mode | S2 |
| `no_mesh` | targeted fire at an actor with no mesh component | S1 |
| `slot_translucent` / `no_eligible_slot` | StackOBot `SM_GenericPlane` (`M_HoloGridFence`, translucent, v1 App. A) | S1 |
| `host_mid` | StackOBot `SKM_Bot` (`G46`) | S1 |
| `nanite_override` | synthetic fixture (N2); none known on host content | S2 |
| `shader_map_unavailable` / `shader_map_incomplete` | **UNEXERCISED** on a healthy cook | — |
| `default_material_path` | synthetic fixture: a skeletal slot whose root material lacks `bUsedWithSkeletalMesh` (the `G49` class) | S2 |
| `no_textures` | `CB_GateLevel` (`BasicShapeMaterial`, v1 §6.1) | S1 |
| `unsupported_type` | synthetic fixture: a cube-texture parameter | S1 |
| `virtual_texture` | Lyra: the three runtime-virtual Megascans textures m52 found (080-04 §5), if a target reaches them; else synthetic fixture | S1/S3 |
| `excluded_group` | synthetic fixture: a UI-group texture parameter | S1 |
| `texture_not_parameter` | synthetic fixture: the alias target (`G-BIND`) | S1 |
| `unsupported_encoding` | synthetic fixture: a `TC_HalfFloat` texture | S1 |
| `held_by_stuck_low_mip` | `G5` (m52 targeted first, held and restoring) | S2 |
| `resource_not_ready` / `streaming_pending` | **UNEXERCISED**: no deterministic producer without a force-residency call, which m53 must not make | — |
| `not_fully_resident` | StackOBot `SM_rock` (measured non-resident, §R4); synthetic fixture: a far, streamed 2048² texture | S1 |
| `no_normal_map` | StackOBot `SM_FloorBase` (mask only) | S1 |
| `normal_unconnected` | synthetic fixture: a normal map feeding a non-normal input | S2 |
| `below_size_policy` | synthetic fixture: a slot whose textures are all 32² | S1 |
| `map_set_over_cap` | synthetic fixture: a slot with 9 parameter maps | S1 |
| `over_budget` | synthetic fixture: a never-streamed 4096² texture (resident, 85.33 MiB) | S1 |
| `rt_alloc_failed` / `draw_precondition_failed` / `param_readback_mismatch` | bench lever `IAI.Bench.TexCorruptFailStep <2..6>` (fails that transaction step once, to prove the rollback) | S1 |

An UNEXERCISED reason is written as UNEXERCISED in the gate report, never as a clean zero.

---

## R7. Corruptors, draws and the Apply transaction

### R7.1 The two corruptor materials

v1 §3.2's draw facts carry over unchanged: Surface domain (a UI-domain material has no local-VF shaders
in a cooked build), Unlit, value written as Emissive, drawn with `DrawMaterialToRenderTarget`, no usage
flag needed.

- **`M_CorruptTex_UV`**: AlphaComposite. Parameters `SrcColor` (Color sampler), `SrcData` (Masks),
  `SrcNormal` (Normal), `SrcKind` (0/1/2), `UvScale`, `UvOffset`, `UvSwap`, and the scramble scalars of
  §R7.5. Output: `f(uv)` per mode (tile `frac(uv·N)`, swap `uv.yx`, drift `frac(uv + offset)`, scramble
  §R7.5), `Emissive = src.rgb`, `Opacity = 1 − src.a`. A normal-class source is re-encoded `n.xy·0.5+0.5`
  into RG (v1 §3.2 encoding table).
- **`M_CorruptTex_Normal`**: Opaque. `SrcNormal` unpacked to `n`, then `NormalSign` (green flip,
  invert), `FlatMix` (flat), `NoiseAmp` with a shipped tileable noise-normal texture (noise), re-encoded
  into RG.
- ⛔ **`AlphaFromSource` is removed.** v1 used it as a no-re-cook fallback that forced A = 1. Under §R11 a
  failing alpha path is a scope decision for chat, not a switch Code flips.

### R7.2 The encoding table

v1 §3.2's table (colour → sRGB target, data → linear target, normal → `.rg` re-encode into a linear
target) is carried, restricted to the admitted formats of §R3.1.

### R7.3 Every draw clears first (P2-6)

For **every** draw — Apply's draws and every `drift` redraw — in this order:

1. `UKismetRenderingLibrary::ClearRenderTarget2D(World, Rt, FLinearColor(0,0,0,1))`
   (`KismetRenderingLibrary.h:43`; `.cpp:46-67`). It enqueues a clear of mip 0. It fails silently when its
   guard fails, so the guard (render target, resource, world) is checked by us first (§R7.4 step 4).
2. `DrawMaterialToRenderTarget(World, Rt, CorruptorMid)`. It enqueues the canvas draw, then
   `UpdateResourceImmediate(false)`, which regenerates the mips (`KismetRenderingLibrary.cpp:180-216`).

- The render target's own `ClearColor` defaults to (0,0,0,1) (`TextureRenderTarget2D.cpp:38`); it is also
  set explicitly, because a fresh target's first draw runs a deferred clear with that colour
  (`TextureRenderTarget.cpp:149-161`).
- **With the clear**, AlphaComposite gives `RGB = src.rgb` and `A = src.a` on every draw (v1's derivation,
  which Codex's Q1 found sound).
- **Without it**, the recurrence is `RGB_n = src.rgb + RGB_{n−1}·src.a` and `A_n = A_{n−1}·src.a`: an
  opaque source with RGB 0.4 goes 0.4 → 0.8 → 1.0, and fractional alpha decays (Codex P2-6).
- **`drift`.**
  - At Apply, an identity draw copies the source into the **snapshot** target.
  - Every frame, the corruptor samples the **snapshot** (never the source) into the **output** target.
    The snapshot and the output are distinct objects.
  - The per-frame cost (clear, draw and mip regeneration, per texture) is part of `G-COST`.
  - A snapshot adds one more 8-bit re-quantisation (source → snapshot → output). `G-RD` measures its
    effect.

### R7.4 The Apply transaction (P2-11)

Steps, in order. The slot is touched only at the last step.

| # | step | failure |
|---|---|---|
| 0 | the decision tree (§R6) says APPLY; nothing has been changed yet | the tree's reason |
| 1 | **reserve** the event's bytes under the run-wide cap (§R3.2) | `over_budget` |
| 2 | **allocate** every render target: `NewObject<UTextureRenderTarget2D>` in the transient package, strongly held by the subsystem; set format, sRGB, `ClearColor`, `LODGroup`, `Filter`, `AddressX/Y`, `MipsAddressU/V` and `bAutoGenerateMips`; then `InitCustomFormat(W, H, Format, bForceLinearGamma)`. Verify each: `GameThread_GetRenderTargetResource() != nullptr`, the size equals W×H, and `GetNumMips()` equals the expected count. | `rt_alloc_failed`; roll back |
| 3 | create the corruptor MIDs (one per texture and mode), set their parameters, and read each back | `param_readback_mismatch`; roll back |
| 4 | **re-check every precondition of every draw ourselves**: `FApp::CanEverRender()`, the world is valid, the material is non-null, the render target's resource is non-null — the exact exits of `DrawMaterialToRenderTarget` (`KismetRenderingLibrary.cpp:156-179`) — plus E2's corruptor shader-map completeness. After this step no draw can take a silent exit. | `draw_precondition_failed`; roll back |
| 5 | **enqueue** every clear and draw (snapshots first for `drift`) | — |
| 6 | create one host MID per distinct resolved material (parent = the resolved material, never a MID), `SetTextureParameterValueByInfo` for every binding in each qualifying slot's set, and read each back (§R2.3) | `param_readback_mismatch`; roll back |
| 7 | **commit:** `SetMaterial(i, HostMid)` on every qualifying slot, and record ownership (§R8) | — |

- **Ordering, and why no wait or fence is needed.**
  - Steps 5–7 enqueue render commands on the single render-command queue, which runs in submission order
    (`RenderCore/Public/RenderingThread.h:256-291`; the named-thread queue is a FIFO,
    `Core/Private/Async/TaskGraph.cpp:876-878`, `Core/Public/Containers/LockFreeList.h:523-524`, `:811`).
    The material-instance parameter updates use the same queue (`MaterialInstance.cpp:390-416`).
  - The same frame's proxy recreate from `SetMaterial` (`SendAllEndOfFrameUpdates`,
    `Renderer/Private/SceneRendering.cpp:4528`) and its scene draw (`FDrawSceneCommand`, `:4650`) are
    enqueued **later**.
  - ⇒ Every render target is drawn and its mips regenerated before any scene draw can sample it, on the
    frame the label begins.
- **Rollback** releases everything steps 2–6 created (`ReleaseResource()`, then the strong references are
  dropped), un-reserves the bytes, and touches no slot, because the commit is step 7 alone. Each rollback
  is counted per step.
- **What the transaction cannot prove.** It cannot prove that the GPU executed the draw correctly; a void
  call reports nothing. That proof is `G-ID` (content) and `G11` (the packaged shader path draws
  non-default content).
- **Where it runs:** inside the fire tick. The labelled window begins on that frame, and `G1` (ONSET)
  checks that the picture changes on it.

### R7.5 `uv_scramble` as a true permutation (D2)

- Cells are indexed `i = y·K + x`, with `N = K²` cells (K compiled 8, a knob).
- The permutation is `π(i) = (a·i + b) mod N`:
  - `a` is chosen among values coprime to N, so π is a bijection;
  - `b` is chosen in [0, N);
  - the pair (1, 0), the identity, is excluded.
- **(a, b) are derived by hashing the run seed with the event's fire index.** Nothing is drawn from the
  auto-injector's stream, so `R-SEED`'s draw protocol (v1 §7.2) is unchanged, and the pair does not
  depend on whether an earlier Apply succeeded.
- The corruptor needs the inverse: output cell j samples source cell `π⁻¹(j) = a⁻¹·(j − b) mod N`. C++
  computes `a⁻¹ mod N` with the extended Euclidean algorithm and passes `(a⁻¹, b, K)` as scalars. The
  local cell UV is kept.
- **Apply verifies bijectivity** by enumerating all N cells on the CPU. The result is logged as
  `scramble_bijective`; a failure would refuse the event, and is unreachable by construction.
- Float arithmetic on integers below 2²⁴ is exact in the shader (K ≤ 64 ⇒ N ≤ 4096).
- It is deterministic across hosts. It is also seam-producing: sampling across a cell boundary is part of
  the look.

---

## R8. Slots, ownership and exact restore (P2-7)

### R8.1 What is captured per slot, before any change

| field | source |
|---|---|
| `Raw` | `Comp->OverrideMaterials.IsValidIndex(i) ? Comp->OverrideMaterials[i] : nullptr` (public, `Classes/Components/MeshComponent.h:26-28`), plus `RawArrayLen = OverrideMaterials.Num()` |
| `Asset` | static: `GetStaticMesh()->GetMaterial(i)` (`StaticMesh.cpp:7685-7693`); skinned: `GetSkinnedAsset()->GetMaterials()[i].MaterialInterface` (`SkinnedMeshComponent.cpp:1252-1264`) |
| `Resolved` | `Raw ? Raw : Asset` — the bound material before any Nanite substitution, exactly as the components resolve it (`StaticMeshComponent.cpp:2655-2667`; `SkinnedMeshComponent.cpp:1252-1264`) |
| `Effective` | `Comp->GetMaterial(i)`, with the Nanite substitution on static meshes (`StaticMeshComponent.cpp:2670-2675`) |

- **Nanite is inspected from `Resolved`** (S4): `UseNaniteOverrideMaterials()` and
  `Resolved->GetNaniteOverride()`. v1 read only `GetMaterial()`, which returns the override B rather
  than the slot's material A; saving B and restoring B would lose A, and checking B for an override could
  miss the refusal. `Effective != Resolved` is logged as a cross-check.
- **Strong references.** `Raw`, `Asset` and `Resolved` go into a `UPROPERTY` array on the injector
  subsystem (the `CorruptedTexturePink` pattern, `AnomalyInjectorSubsystem.h:133-137`) from capture until
  that slot's restore, or its abandonment, completes. The actor and the component stay weak.
  - This closes Codex's GC case: `corrupted_texture` keeps its original weakly
    (`Anomaly_CorruptedTexture.h:30`), so a runtime material held only by the slot could be collected
    mid-event and the revert would reset to the mesh default. m53 does not copy that.

### R8.2 Ownership

- An event owns a set of (weak component, component `FName`, owner, slot index) keys, each mapped to the
  host MID it installed, plus the render targets and corruptor MIDs it created.
- A render target is never shared across events or actors. One host MID per distinct resolved material
  within the event (v1 §4.2's sharing rule, kept for the MID only).

### R8.3 Restore

The m17 contract, mirrored, with the raw state:

1. Re-find the component: the weak pointer, otherwise by `FName` on the owner.
2. Touch slot i **only if `OverrideMaterials[i]` is still this event's host MID.** A raw read is correct
   here, because the host MID's parent has no Nanite override (S4). If it is not ours, the slot is left
   alone and logged `left-to-game`; the host replaced it and we do not stomp.
3. If `Raw` was non-null, `SetMaterial(i, Raw)`: the strongly held original **object**, which is an
   exact restore. Otherwise `SetMaterial(i, nullptr)`, and the slot falls back to `Asset`.
   - **Named limit:** `OverrideMaterials` may stay longer than `RawArrayLen`, with null entries.
     `SetMaterial` grows it with `AddZeroed` (`MeshComponent.cpp:59-62`), and nothing shrinks it without
     touching other slots. That renders identically but is not a byte-identical component state.
     `override_len_before/after` is logged.
4. **Sweep** every mesh component of each touched owner for a slot still holding one of our host MIDs
   (the component was re-created after Apply). That is **cleanup**, logged `swept`, and never evidence of
   an exact restore.
5. **Release:** each render target's `ReleaseResource()` and its reference; the corruptor and host MIDs;
   then, only after the slot is restored, the strongly held originals. Bytes move from live to
   `pending_release` (§R3.2).
6. **Log per slot:** `restored-exact` (the raw object re-set) / `restored-default` (raw was null) /
   `left-to-game` / `unresolved` / `swept`.

⚠ Pointer restoration and dropped references do not prove byte-identical future frames or a synchronous
GPU deallocation. `G3` measures the frames.

### R8.4 Two ids, and which fire paths are supported

- `uv_corruption` and `normal_corruption` keep separate ownership arrays, render-target sets and counters.
- **Supported fire paths:** auto-pool `TryFireOnce` and targeted `TryFireSpecific`. Both exclude an actor
  already in `LiveFires` (`AnomalyAutoInjectorSubsystem.cpp:286`; `:454`, `:459-469`), so the
  one-anomaly-per-actor rule (`G30`) holds there.
- **Not supported:** raw `ApplyAnomaly` (`AnomalyInjectorSubsystem.cpp:705-730`) and its callers — console
  `IAI.Apply`, the control server's `inject`, the selector, and session globals. They do not check
  `LiveFires`, and m53 does not claim isolation for a raw apply that overlaps `missing_texture` or
  `corrupted_texture` on the same actor.

### R8.5 Exits

Normal revert, `FinishRun`, cancel before focus, target destroyed (`OnEndPlay` plus a weak-pointer poll,
v1 §4.4), level change, world teardown, and transaction rollback (§R7.4). Every one reaches §R8.3.

---

## R9. Prewarm, first use, cost (P2-11, D6)

### R9.1 The lists are split

- Today `GatherAnomalySwapMaterials` (`AnomalyCaptureSubsystem.cpp:2399-2412`) feeds both the prewarm
  (`PrewarmAnomalyShaders`, `:2518`) and the per-frame count (`CountIncompleteAnomalyMaterials`, `:2418`,
  called per frame at `:4442`).
- m53 adds a new `GatherAnomalyPrewarmMaterials` = that list plus the two corruptors, used **only** by the
  prewarm. `CountIncompleteAnomalyMaterials` keeps calling the unchanged `GatherAnomalySwapMaterials`, so
  the per-frame pending count does not widen (`G298` unchanged).

### R9.2 What the prewarm is not

- In a cooked build `EnsureIsComplete` is a no-op: its whole body is `#if WITH_EDITOR`
  (`MaterialInterface.cpp:1468-1480`). The m47 prewarm's own log says so (`AnomalyCaptureSubsystem.cpp:2540-2541`).
- PSO precaching is off in 5.1 (`RHI/Private/PipelineStateCache.cpp:103-110`, `:2131-2139`). The first
  draw of each new combination of shaders, render state and render-target format creates its PSO, and the
  command list waits for it (`:2067-2094`). That is a stall, not a wrong frame.
- **Distinct first-use PSOs:** the UV corruptor × {RGBA8, RGBA8_SRGB}; the normal corruptor × RGBA8; the
  clear; and the mip-generation pass per format (`RenderCore/Private/GenerateMips.cpp`). At least five.

### R9.3 The warm draw

- **Where.** The m47 prewarm runs in `BeginActualRun` (`AnomalyCaptureSubsystem.cpp:3517`), and the very
  next tick captures the first lead-in frame (`:718`). There is no uncaptured frame to use on the direct
  path.
- **What.** A new **non-capturing** phase, `TexCorruptWarm`, between `BeginActualRun` and `LeadIn`,
  modelled on `SettleAfterFire` (`:722-725`), which counts down without calling `CaptureCurrentFrame`.
  - It is entered only when an m53 id is enabled in the pool or is the targeted anomaly. Every other run
    is byte-identical.
  - It lasts 2 frames. It draws each corruptor once into a small scratch target of each format (clear,
    draw, mips) and releases them.
  - Knob `IAI.Capture.TexCorruptWarmDraw`, compiled on.
- **What it can and cannot claim.** It moves first-use PSO creation out of the captured frames **if** the
  scratch draws hit the same PSOs (same shaders, blend and formats). Engine offers no PSO introspection,
  so that is not proved.
  - Instead it is **measured**: `G-COST` compares the first labelled Apply frame's time with the warm draw
    on and off, for both corruptors and every target format.
  - The residual cold-first-fire cost is a **measurement with an owner acceptance decision** (O3).

### R9.4 `G-COST`, the predeclared method

- **Setup:** paced 30 fps, the same build, m55 held constant (on, on both sides). Order A,B,B,A after one
  declared discard leg (`G186`).
  - A = both m53 ids disabled.
  - B = targeted m53 legs on the fixture slot with the **most required maps that fit the budget**, one
    static mode; and separately `drift`.
- **Readings:**
  - per-frame wall time, median and p99; `speed_ratio`; capture throughput (frames written per wall
    second);
  - the first Apply frame's time with the warm draw on and off;
  - the `drift` per-frame redraw time, and its clear and mip work;
  - the cost of a cancel and of a rollback;
  - repeated allocation and release over ≥ 20 events, with `texcorrupt_rt_bytes_peak` returning to 0
    after each event.
- **Acceptance.** A difference no larger than the within-build spread across positions reads "below the
  resolution of this instrument" (`G169`). Anything larger is reported with its number and goes to the
  owner. **No threshold is invented here**; accepting the cost, the cold first fire and `drift` is an
  **owner decision** (O3).

---

## R10. Strength prior and labels (P3-12)

- **`texcorrupt.expected_strength_class`** ∈ {`strong`, `medium`, `weak`} is a **mode prior**. The client
  readme will say: "the class this mode is expected to fall in on typical content; it is not a
  measurement of this event."
  - Per-event pixel change is m55's (`chg_*`). Perceptibility is measured by nobody.
  - v1's universal statements are withdrawn. A constant or permutation-invariant texture above the size
    policy can be unchanged by tile or scramble; a flat normal map is unchanged by sign flips; lighting
    controls how much a normal change shows.
  - Priors: `strong` tile, scramble, invert; `medium` green_flip, noise, drift; `weak` swap, flat.
- **m55 stays out** of eligibility, ranking, seeds, verdicts and `observable` (the m55 v1 contract).
- **`G-STR`** (S3) may amend the default mode sets and the prior classes from offline measurement. The
  key's values move with that amendment.

**Label keys.** Additive; `label_schema` stays 2 (the m52 ruling, `2026-09-20-m52-stuck-low-mip.md:672-673`).

- **`annotation.json`:** `anomaly_type` = the id; `anomaly_subtype` = the mode. These are new values in
  existing fields, so the field set does not move (`P6`).
- **`labels.jsonl`, inside m53 anomaly entries only:**
  - `texcorrupt.mode`, `texcorrupt.expected_strength_class`;
  - `texcorrupt.slots_corrupted`, `texcorrupt.slots_total`, `texcorrupt.slots_untouched[]` of
    `{slot, reason}`;
  - `texcorrupt.condition_held`;
  - per-mode parameters: `texcorrupt.tile`, `texcorrupt.tile_detail_mips`, `texcorrupt.uv_offset`
    (`drift`, per frame), `texcorrupt.scramble_cells`, `texcorrupt.noise_amp`;
  - `texcorrupt.textures[]` of `{name, param, association, layer_index, class, pixel_format, snapshot_mip,
    snapshot_px, rt_bytes}`.
- **Removed from v1:** `texcorrupt.route`, `texcorrupt.reconstructed`, `texcorrupt.strength_class`,
  `src_resident_top_px`, `rt_size`.
- **Reserved, never emitted:** `texcorrupt.host_mid_cloned`.
- **`run_summary.json`:**
  - `texcorrupt_fires_applied`;
  - one `texcorrupt_refused_<reason>` per event-final reason;
  - two dispositions objects, `texcorrupt_slot_dispositions` and `texcorrupt_binding_dispositions`
    (reason → count), which keeps the key set bounded;
  - `texcorrupt_rt_bytes_peak`, `texcorrupt_rollback_<step>`;
  - `texcorrupt_restored_exact`, `texcorrupt_restored_default`, `texcorrupt_left_to_game`,
    `texcorrupt_swept`.
- **`condition_held`** is read from live state, not special-cased: the slot's raw binding is this event's
  host MID **and** that MID's parameter still resolves to our render target. `NoApply` (§R12) therefore
  reads false because nothing was installed, not because a lever says so.

---

## R11. Failure policy (P1-5): no route A

| finding in S1 or S2 | action |
|---|---|
| `G-ID` fails for the **normal corruptor** only, and every UV path passes, including the UV corruptor's normal-class re-encode | defer `normal_corruption`; continue UV |
| `G-ID` fails on the **UV corruptor's normal-class path** | STOP and report. The proposal to chat: UV restricted to slots whose required set holds no normal-class binding. That is a narrower product, so chat rules it. |
| `G-ID` fails on a **colour, data or alpha** path | STOP and report. The proposal: refuse the failing class (`unsupported_encoding` naming it), if the remaining subset is non-empty. Chat rules it. |
| `G-ID-M` fails at minification or a mip transition on a box-chain fixture | STOP; it bears on N1 |
| a **wrong-copy control does not fail** | the instrument is invalid; STOP (`G96`) |
| `G3` or `G4` fails (restore, lifecycle, rollback) | STOP |

- **Never:** switch to the takeover, relax a tolerance, or widen a fixture so that a gate passes. A
  takeover is a separate product decision.
- **Code does not narrow the product silently.** A restriction is proposed to chat with the `G-ID`
  evidence, and applied only after a ruling.

---

## R12. Gates (P2-10)

**Classes.** **Q** = qualification gate: it can fail, and a failure stops the stage. **D** = diagnostic
reading: reported, no pass or fail. **O** = owner decision: numbers go to the owner.

### R12.1 Fixtures

- **F-SYN** — a synthetic bench level `CB_TexCorruptLevel` with its fixture materials (**N2**).
  - A sibling of `CB_GateLevel`, which it never edits (`G99`), authored by a CaptureBench tool in the
    `make_lod_calib_level.py` pattern, and excluded from the shipping cook like `CB_LodCalib`.
  - Settled camera, as `CB_GateLevel`.
  - Contents, all ≤ 1024 px and never streamed unless stated:
    - one opaque target per admitted encoding: sRGB DXT1; sRGB DXT5 whose alpha feeds a **non-threshold**
      input (fractional alpha); BC7; BGRA8; BC4; G8; BC5 normal;
    - a masked target whose opacity mask reads base-colour alpha (alpha at silhouettes);
    - a material-layer parameter target; the alias target; a dead-binding target;
    - reason targets: 9 parameter maps; all maps 32²; a translucent slot; a cube-texture parameter; a
      UI-group texture; a `TC_HalfFloat` texture; a normal map feeding a non-normal input; a streamed
      2048² texture placed far away; a never-streamed 4096² texture; a Nanite mesh whose material has a
      Nanite override; a skeletal slot whose material lacks `bUsedWithSkeletalMesh`;
    - geometry for `G-ID-M`, carrying the colour and normal encodings: a target filling the view, a far
      target, a grazing-angle plane, and a surface crossing mip transitions;
    - a target whose material samples through a shared sampler (`SSM_Wrap_WorldGroupSettings`);
    - **reference-bug materials** for transformed outputs: copies of a fixture material with a real ×N UV
      scale, and a real `uv.yx` swap, in their own graphs. They show what a true UV bug looks like, so a
      baked `uv_tile` / `uv_swap` can be compared with it (`G-ID-M`, transformed rows).
- **F-MW** — StackOBot `MainWorld` at its settled pose: `SM_FloorBase`, the rocks, `SKM_Bot`,
  `SM_GenericPlane`, the modular kit. Real-host readings.
- **F-GATE** — `CB_GateLevel`: refusals only (`no_textures`).
- **F-LYRA** — `L_ShooterGym` at Lyra's own defaults (TSR, Lumen, auto-exposure on): the cubes, the
  weapons, the characters.

### R12.2 Controls, available on every fixture

- **Null (matched): `IAI.Bench.TexCorruptNoApply`.** The same recipe, the same draws, the same MIDs;
  only step 7 of the transaction (the slot commit) is skipped.
- **Positive (matched): `corrupted_texture`**, targeted at the same actor from the same pose. It has no
  fixture gate and fires on MainWorld (`AnomalyInjectorSubsystem.cpp:185`; `Anomaly_CorruptedTexture.cpp:57-134`).
  On `L_ShooterGym`, m55's `solid_swap` and `null_effect` are also available; they are fixture-gated
  (`Anomaly_ChangeCase.cpp:21-23`) and that gate is **not** widened.
- **Wrong copy (must fail): `IAI.Bench.TexCorruptWrongEncoding <srgb|normal|alpha|mip|noclear>`.**
  - `srgb` writes a colour source into a linear target;
  - `normal` skips the `·0.5+0.5` re-encode;
  - `alpha` draws the colour corruptor as opaque (A = 0);
  - `mip` skips the mip regeneration after the draw;
  - `noclear` skips the clear before a redraw.
  - Bench-only, console-only, default off, loudly echoed, never in a client payload.
- **Same-build control pair:** two NoApply legs. They establish the noise band at the AA-off arbiter
  (the m45 precedent: 0 frames differing).
- **Other bench levers:** `TexCorruptForceMissingAsset`, `TexCorruptFailStep <n>`,
  `TexCorruptForeignReplace` (sets a foreign material on the slot mid-event), `TexCorruptIdentity`
  (identity mode; v1's lever, kept), `TexCorruptIdentityRedraw` (identity with a forced per-frame redraw).
  All under `IAI.Bench.`, with the same rules as above.

### R12.3 The gate table

| id | class | stage | fixture | recipe | reading / criterion | minimum count |
|---|---|---|---|---|---|---|
| **G0** | D | S1 | all | eligibility census | per target, every slot's `Raw`/`Asset`/`Resolved`/`Effective` material and Nanite routing; every texture entry (type, name, association, index, texture, class, pixel format, dimensions, `resident/max` LODs, `AssetLODBias`); each binding's and slot's disposition; the final reason. A disagreement with §R14's predictions is a **finding**, never a pass. | every target on F-MW, F-LYRA; every F-SYN target |
| **G-BIND** | Q | S1 | F-SYN | targeted identity and a strong mode on the alias, layer and dead-binding targets | (1) both build targets link with the declared dependencies unchanged; (2) the alias target is refused `texture_not_parameter`; (3) on the layer target, only the layer's region changes against NoApply; (4) the dead-binding target is reported (D) | 3 applied events on the layer target |
| **G-ID** 🚨 | Q | S1 | F-SYN (+ `SM_FloorBase` on F-MW if admitted) | identity through **both** corruptors, every admitted encoding; AA-off arbiter, native order; identity leg vs NoApply leg | **EXACT** if 0 pixels differ in the target mask on every frame. **PASS** if max \|d\| ≤ q = 2 (8-bit, any channel) on every frame. Every wrong-copy control must read max \|d\| ≥ 8q = 16 on the same mask, or the instrument is invalid. The alpha half runs on the fractional-alpha and masked targets; if no admitted target reads alpha it is **UNEXERCISED**, never a pass. q is **N4**. | 3 applied events per (corruptor, encoding) |
| **G-ID-M** | Q on F-SYN box-chain targets; D elsewhere | S1 (identity), S2 (transformed) | F-SYN; F-MW / F-LYRA as readings | G-ID's comparison on the magnification, minification, grazing, mip-transition and shared-sampler targets; in S2, baked `uv_tile` and `uv_swap` against the reference-bug materials at the same geometry | identity: G-ID's criterion. Transformed: a **D** reading of the difference between the baked mode and the real UV bug, per geometry (the `tile_detail_mips` limit of §R3.4 shows up under magnification). On host content with authored chains (Appendix C) it is a reading only, pending **N1**. | 3 applied events per geometry |
| **G-RD** | Q | S1 (identity), S2 (`drift`) | F-SYN | `TexCorruptIdentityRedraw` for ≥ 90 frames, opaque and fractional-alpha inputs; then `drift` | every frame within G-ID's PASS against NoApply, **and** the applied leg's frame N vs its frame 1 reads 0 differing pixels (no drift, no brightening, no alpha decay). The `noclear` wrong copy must fail (it brightens). | 1 leg per input |
| **G1** | Q | S2 | F-SYN; F-MW `SM_FloorBase` if admitted (else D) | each mode, both tick orders | ONSET: the first labelled frame is the first frame differing from NoApply (m44), except `drift`, whose onset is read from `ref_*` | 4 applied events per mode per order |
| **G2** | Q | S2 | F-SYN | NoApply | `condition_held` false and `observable` false on every labelled frame; `frames_condition_lost` counts them; `injected_frames` non-empty and `affected_frames` empty; the event stays and is not vetoed (the host still draws, so m26 reads non-zero) | 4 events |
| **G3** | Q | S1 | F-SYN; F-MW if admitted | revert, then settle | the first post-revert frame vs the pre-onset reference over the **whole frame**, AA-off native: inside the same-build control band (0 differing); every touched slot logged `restored-exact` or `restored-default` with `Raw` pointer-identical; the event's render targets released | 3 events per id |
| **G4** | Q | S1 | F-SYN | exits and failures | normal revert, `FinishRun`, cancel before focus, target destroyed mid-span, level change; a forced garbage collection mid-event (the engine console `obj gc` if it is available packaged; else UNEXERCISED); `TexCorruptForeignReplace` → `left-to-game`; a re-created component → `swept`; both ids live on two actors → independent ownership and release counts; `TexCorruptFailStep 2..6` → rollback, no slot touched, bytes un-reserved, nothing leaked. After every exit the live render-target bytes return to 0. | each exit once per id |
| **G5** | Q | S2 | F-MW | m52 targeted first (held, then restoring), then m53 on a target sharing the texture; and the reverse order | refused `held_by_stuck_low_mip` by name in both m52 states; in reverse order the auto-pool m52 pick refuses the shared texture. The targeted-m52 bypass is stated, not tested as isolation. | 1 per state |
| **G6** | Q (refusals), D (Nanite admit) | S2 | F-SYN; F-MW modular kit | Nanite paths | `nanite_override` refused on the F-SYN target; `host_mid` refused on the Bot; a Nanite target without an override, if it reaches APPLY, is labelled with `observability_measured` false (the m50 admit path). A case with no fixture is UNEXERCISED, never a pass. | 1 each |
| **G-REASON** | Q | S1–S2 | per §R6.4 | each reason's producer | named in the REFUSED line and counted in `run_summary`; each UNEXERCISED reason stays listed | 1 each |
| **G7** | O | S3 | F-MW, F-LYRA | auto-pool with both ids enabled explicitly | attempted, applied, refused per final reason, plus the slot and binding dispositions | one census per fixture per id |
| **G8** | Q | S2 | F-MW | `P-C7 v3` against a pre-m53 control pair | `labels.jsonl` field set unchanged without m53; `run_summary` adds exactly the §R10 keys; the `annotation.json` field set unchanged (`P6`); new `anomaly_subtype` values only for the new ids | 1 pair |
| **G-STR** | D → O | S3 | F-SYN, F-MW, F-LYRA | every mode with m55 on | m55 onset and `ref_*` per mode against the matched NoApply and `corrupted_texture` twins (and `solid_swap` / `null_effect` on L_ShooterGym); the evidence for amending the default modes and priors | 3 events per mode per fixture where admitted |
| **G-COST** | O | S3 | F-SYN | §R9.4 | §R9.4's readings; acceptance is the owner's (O3) | §R9.4 |
| **G9** | D | S2 | S2 legs | `--label-pixel-gate` + `--change-oracle` | readings only; the evidence rows must be present (exit 0 on absent evidence is not accepted) | all S2 legs |
| **G10** | Q | every build | — | editor and game targets | exit 0, zero warnings, declared dependencies unchanged | every build |
| **G11** | Q | S1, S2 | packaged | startup read-back + G-ID + G1 | a startup line names both corruptors and the noise texture and says each resolved non-null; every (corruptor, target format) pair drew non-default content, shown by G-ID's applied legs (a default-material draw fails G-ID); the noise texture's own sampling is proven in S2 by G1's `normal_noise` row, since identity draws it at zero amplitude | covered by G-ID and G1 |
| **G-LYRA** | Q per id, for a Lyra support claim | S3 | F-LYRA | targeted and auto-pool | at least 1 successful counted event per id. A named refusal alone is a yield reading, and that id is then **unqualified on Lyra**. Pixel identity is not claimed there (TSR). | 1 applied event per id |

---

## R13. Stages, builds and cooks

| stage | content | builds and cooks | gates | stop if |
|---|---|---|---|---|
| **S0** | this revision ruled, including N1–N4 | — | — | — |
| **S1** — route B core, identity only | the decision tree, the transaction, restore, the residency check, the budget, the host-MID refusal, the split prewarm list and the warm-draw phase, the bench levers; the two corruptor assets and the noise texture; the F-SYN level and materials (N2); both ids registered, the identity lever only, nothing in the auto pool yet | **2 builds (editor + game), 1 cook** (plugin assets + F-SYN), plus the editor authoring run that creates F-SYN's content | G0, G-BIND, G-ID, G-ID-M, G-RD (identity), G3, G4, G-REASON (S1 rows), G10, G11 | §R11: G-BIND, G-ID, G3 or G4 fails; a wrong-copy control does not fail |
| **S2** — every mode, both ids | the eight modes; the auto-pool mode draw (`R-SEED`, v1 §7.2); `drift` only if G-RD passes | **2 builds, 0 cooks**, if S1's graphs carry every mode's parameters (they are specified to) | G1, G2, G5, G6, G8, G9, G-RD (`drift`), G-REASON (S2 rows), G10 | a mode fails ONSET, or G2's falsifier cannot fire; `drift` failing G-RD defers `drift` (D5) |
| **S3** — measurement | yield, strength, cost, Lyra | 0–1 bench builds + a Lyra worktree build (shared-tree rule 5) | G7, G-STR, G-COST, G-LYRA | a restore, lifecycle or rollback defect found on host content (a G3/G4 shape) stops S3. Otherwise S3 produces numbers: an id with no counted application on a fixture is recorded **unqualified** there, and default-on, the budget and cost acceptance wait for the owner (O1–O3). |
| **S4** — docs and merge | client readme (the prior, the refusals, the limits), architecture, checklist, catalogue; comment strip | 1 build pair | G10 | — |

- **Counts:** about **8 bench builds** (2 per stage) plus 1–2 Lyra builds; **1 cook**. A second cook is
  needed only if an S1 graph lacks a parameter S2 needs.
- **"One cook" means one content iteration.** Lyra and each office host build and cook for themselves
  (Codex Q7). The office procedure still gains the startup read-back line (v1 §3.3).
- **Estimate: 5 implementation sessions** (S1 is two: it now carries the fixture level, the transaction
  and the levers). v1 said 4.
- **S1 cannot start before N1 and N2 are ruled.** N1 decides the render-target module; N2 decides where
  G-ID can run.

---

## R14. Expected yield impact

**Method.** Three sources, none of them a measurement of m53:

- the 082-01 name-table scan (material chains and texture classes; v1 Appendix A);
- the 082-03 tagged-property scan (dimensions and authored mip settings; Appendix C);
- m52's banked residency readings (§R4).

These are **predictions**. `G0` and `G7` read the real values.

### R14.1 Per target, first failing step

| fixture target | textures (imported → cooked top) | v1 prediction | v2 prediction, UV family | v2 prediction, normal family |
|---|---|---|---|---|
| MainWorld `SM_rock`, `SM_rock_02` | `T_rock_0x_D/N/AORM` 4096²; `T_detail_N` 2048² | eligible, both | **`not_fully_resident`** (measured resident 11–12 of 13 at the bench pose). Even if resident, **`over_budget`** (one 4096² map is 85.33 MiB). If the world-aligned detail map is a constant, `texture_not_parameter` comes first. | same |
| MainWorld `SM_FloorBase` (most-fired m52 target) | `T_Grid_A` 1024² mask (5.33 MiB) | UV eligible if a parameter | **eligible** iff `T_Grid_A` is a parameter and fully resident (its residency at the pose is unknown: m52 held it above its floor, which does not prove full). The only StackOBot target with a plausible UV event, and **data class only**. | `no_normal_map` |
| MainWorld `SKM_Bot` | `T_Bot_*` 4096²; `T_Eyes_Atlas` 2048² (`LeaveExistingMips`) | eligible via clone | **`host_mid`** (S3 comes before any texture step) | same |
| MainWorld modular kit (Nanite) | `T_SandTileabe_BC`, `T_ConcreteTileable_N`, `T_Metal_Painted_N` 4096² | eligible if parameters | **`over_budget`** (a 4096² map in every material instance), likely not resident either | same |
| MainWorld `SM_RockFlats_*` | `T_RockTileable_BC` 2048², `T_SandTileabe_BC` 4096² | parameters unknown | `over_budget` if the sand map is active | `no_normal_map` |
| MainWorld `SM_GenericPlane` | translucent | refused | `no_eligible_slot` | same |
| `CB_GateLevel` | none | refused | `no_textures` | same |
| Lyra `Cube*` | `T_Paint_Diffuse` 2048², `T_Paint_Normal` 2048², `T_Paint_Glossiness` 2048² with LODBias 1 (→ 1024²), `T_Paint_Opacity` 2048² | eligible, both | **fits (48 MiB)** if three maps are active; **`over_budget` (69.3 MiB)** if the opacity map is active in the opaque material. **`drift` never fits** (≥ 96 MiB). Residency plausible (the cube was ~1537 px across at m52's bench pose); virtual-texture status unknown. | **fits (21.33 MiB)** |
| Lyra weapons (Nanite) | `T_Rifle_*` 4096² with LODBias 1 (→ 2048²) × 4 | eligible | **`over_budget` (85.33 MiB)** | fits (21.33 MiB); observability unmeasured (Nanite, `G134`) |
| Lyra `SKM_Manny/Quinn` | `T_Manny_01_*` 8192² with `MaxTextureSize` 4096 (→ 4096²), `Sharpen1`/`Sharpen2` | refused (v1 route A) / clone (B) | **`over_budget`**, and `host_mid` first if their slots hold MIDs; sharpened chains (N1) | same |

### R14.2 What that adds up to

- **StackOBot MainWorld:** at most **1 of the 5** measured non-Nanite targets (`SM_FloorBase`, UV only,
  data class only) is predicted to produce an event. **No StackOBot host target exercises the colour or
  normal encodings at all**, which is why G-ID needs F-SYN (N2).
- **Lyra:** the cubes (UV static modes and normal) and the weapons (normal only, Nanite).
- **`drift`** is predicted to refuse `over_budget` on every host target except `SM_FloorBase`
  (2 × 5.33 = 10.67 MiB).
- **Which refusal binds, and where:**
  - **residency** binds first on large, streamed textures at the bench poses (the rocks);
  - **the budget** binds on every 4096² source and on multi-map 2048² sets (the kit, the weapons, the
    characters, cube `drift`);
  - **host MID** removes the StackOBot hero;
  - **atomic coherence** removes slots mixing parameters with constants or virtual maps. Its count is
    unknown until `G0`; the rocks' world-aligned detail map is the likely case.

### R14.3 Budget sensitivity (N3), arithmetic only

| budget | newly fits | still refused |
|---|---|---|
| 64 MiB (current) | FloorBase UV; cube UV (3 maps); cube normal; weapon normal | everything above |
| 128 MiB | cube UV with the opacity map; cube `drift` (96); weapon UV (85.33); a single 4096² map (85.33) | rock and kit sets (residency, and more than one 4096² map); weapon `drift` (170.67) |
| 256 MiB | two 4096² maps (170.67); weapon `drift` | a rock UV set (3 × 85.33 + 21.33 = 277.33); rock and kit residency |

Raising the budget cannot recover the rocks at the bench poses: residency refuses them first. That lever
is a prefetch design, deferred by ruling (§R4).

---

## R15. NEEDS-DECISION

- **N1 — mip-chain preservation (the counter-proposal to P1-2).**
  - (a) Declared RenderCore + RHI dependencies on `AnomalyInjector` and a per-mip draw: each output mip
    drawn from the source's own mip into a scratch render target and copied into that mip of the output.
    It preserves authored, sharpened and alpha-coverage chains by construction.
  - (b) Stay inside Engine: regenerate the mips, disclose it, qualify only box-chain fixtures, and accept
    an undetectable class (on the fixtures: 8 of 131 StackOBot textures and 22 of 324 Lyra textures,
    including every Lyra character map).
  - (c) Hold m53.
  - **Recommended: (a).** **Blocks S1.**
- **N2 — the synthetic fixture level F-SYN**, authored by a CaptureBench tool: bench-only, a sibling of
  `CB_GateLevel`, excluded from the shipping cook.
  - It is needed because host content cannot supply the alias and layer fixtures (P1-1), the reason
    fixtures (P2-9), the encoding, alpha and mip-geometry matrix (P1-2, P2-6), or **any** colour or normal
    encoding on StackOBot (§R14).
  - It touches CaptureBench, which this brief fences.
  - **Blocks S1.**
- **N3 — the render-target budget under no downsampling.** 64 MiB is kept as the default; §R14.3 is the
  arithmetic. Not needed before S1; needed before `G7`'s census means anything.
- **N4 — `G-ID`'s tolerance.** q = 2 LSB (8-bit, any channel) with an 8q separation from the wrong-copy
  controls. Its basis is one 8-bit re-quantisation of BC-interpolated or sRGB-decoded texels, then
  filtering. The alternative is EXACT-only (0 differing pixels). Chat accepts or tightens it before S1.
- **Later owner decisions:**
  - **O1** default modes and pool membership (`G7` + `G-STR`);
  - **O2** the budget value (with N3 and `G-COST`);
  - **O3** acceptance of the cold first fire and of `drift`'s cost (`G-COST`);
  - **O4** a prefetch design, only if `G7` shows residency is the binding refusal.

---

## R16. What this revision does not claim

- ⛔ **No yield, strength or cost number.** §R14 is derived from offline scans and banked m52 readings,
  and `G0` / `G7` / `G-STR` / `G-COST` produce the real ones.
- ⛔ **No claim that the encodings are correct** (`G-ID`) or that the binding read links inside the
  declared dependency set (`G-BIND`).
- ⛔ **Authored mip chains are not solved** (N1).
- ⛔ **No perceptibility claim.**
- ⛔ **No incidence claim about the office hosts' content.**
- ⛔ **Nothing here changes** `m51` (held at `53bf725`), `master`, any tag, any cooked container,
  CaptureBench (N2 is a proposal) or `ToCodex\`.

---

## Appendix C — the 082-03 tagged-property scan (EVIDENCE, not a measurement)

**Method.**

- For every `.uasset` under the content roots (StackOBot `Content`; Lyra `Content`, `ShooterCore`,
  `ShooterMaps`, `LyraExampleContent`), read the first 4 MiB.
- Parse the package summary's name map, then find tagged-property byte patterns: `ImportedSize`
  (`IntPoint`), `CompressionSettings` / `LODGroup` / `MipGenSettings` (enum), `MaxTextureSize` / `LODBias`
  (int), and `SRGB` / `bDoScaleMipsForAlphaCoverage` / `CompressionNoAlpha` (bool).
- The scripts are outside the repo: `C:\ClaudeTemp\m53scan\texprops.py` and `mipcensus.py`.

**What it can and cannot say.**

- These are **editor** assets, so they carry import and cook **settings**. The cooked top mip follows from
  `ImportedSize`, reduced by `MaxTextureSize` and by cook-time `LODBias` stripping
  (`TextureLODSettings.cpp:176-186`; `TextureDerivedData.cpp:2435`).
- **Default values are not serialised**, so an absent key means the default. Checked both ways: `SRGB: 0`
  appears on every normal map (where the editor turned it off) and is absent on the diffuse maps (default
  on).
- Device-profile group limits (`MaxLODSize`) are not applied. Placed-component overrides, Nanite status
  and virtual-texture status at runtime are not read.

**Dimensions of the targets' textures** (imported size; `LODBias` and `MaxTextureSize` where set):

| fixture | texture | imported | notes |
|---|---|---|---|
| StackOBot | `T_rock_01_D/N/AORM`, `T_rock_02_D/N/AORM` | 4096² | |
| StackOBot | `T_detail_N`, `T_grunge_mask`, `T_black`, `T_white`, `T_default_AORM` | 2048² | `T_default_normal` 2048², `MaxTextureSize` 512 |
| StackOBot | `T_SandTileabe_BC`, `T_ConcreteTileable_N`, `T_Metal_Painted_N` | 4096² | |
| StackOBot | `T_Grunge_A`, `T_ConcreteTileable_M`, `T_Grid_A` | 1024² | |
| StackOBot | `T_Ribbing_A`, `T_Ribbing_N` | 256² | `LODBias` 3 |
| StackOBot | `Wind` | 2048² | `TC_VectorDisplacementmap` |
| StackOBot | `T_WhitePixel` | 1×1 | |
| StackOBot | `T_Bot_Albedo/Normal/M_R_AO/Masks` | 4096² | |
| StackOBot | `T_Eyes_Atlas` | 2048² | `TMGS_LeaveExistingMips` |
| StackOBot | `T_RockTileable_BC` / `T_RockTint` | 2048² / 1×256 | |
| Lyra | `T_Paint_Diffuse`, `T_Paint_Normal`, `T_Paint_Opacity` | 2048² | |
| Lyra | `T_Paint_Glossiness` | 2048² | `LODBias` 1 |
| Lyra | `T_Rifle_D/Combined_N/AORM/Masks`, `T_Pistol_D/N` | 4096² | `LODBias` 1 |
| Lyra | `T_Weapon_D/N/AORM` | 512² | `MaxTextureSize` 32 |
| Lyra | `T_Manny_01_D/N/MSK` | 8192² | `MaxTextureSize` 4096; `T_Manny_01_D` `TMGS_Sharpen1` |
| Lyra | `T_Detail_Normal`, `T_OrangePeel_N` | 1024² | |

**Mip-generation census, all textures with an `ImportedSize` tag:**

| fixture | textures | `MipGenSettings` | alpha-coverage scaled | non-box chains |
|---|---|---|---|---|
| StackOBot | 131 | default 118 · `NoMipmaps` 5 · `LeaveExistingMips` 7 · `Sharpen10` 1 | 0 | **8** (`T_Eyes_Atlas`, `T_lens_flare_03`, 6 hash-named) |
| Lyra | 324 | default 179 · `NoMipmaps` 122 · `Sharpen1` 4 · `Sharpen2` 9 · `Sharpen4` 3 · `Sharpen7` 1 · `Blur1` 2 · `Blur5` 1 · `LeaveExistingMips` 2 · `SimpleAverage` 1 | 0 | **22** (every `T_Manny_0x_D/MSK1` and `T_Quinn_0x_*` map among them) |

**Size histogram (longest side):** StackOBot 4096: 44 · 2048: 55 · 1024: 8 · ≤ 720: 24. Lyra 8192: 27 ·
4096: 40 · 2048: 23 · 1024: 33 · ≤ 512: 201.

---
---

<details>
<summary><b>⛔ SUPERSEDED 2026-09-27 — revision 1, the 082-01 plan at <code>946c1bf</code>, kept verbatim. See
<code>_reviews/082-02-codex-m53-design-review.md</code> and
<code>_reviews/082-03-chat-ruling-codex-m53-design-review.md</code>. Its DESIGN is withdrawn and may not be
cited: route A and its fallback, the §2.3/§2.4 refusal tables, the §3.4 snapshot rule, the §4.2/§4.3 restore,
the §5.3 strength classes, the §6 gates, the §8 costs, the §9 stages and the §10 D1–D7 as written. Only the
SOURCE FACTS listed in §R0.2 above remain citable, as <code>v1 §x</code>.</b></summary>

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

</details>
