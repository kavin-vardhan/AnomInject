# 082-06 — m53 S1 part 2: corruptor assets, F-SYN fixture + offline proofs, M1, archive, cook, M2, G-COOK

Session 082-06, 2026-09-27, Claude Code (Opus 5.5), headless from the GDP mailbox. Brief:
`082-06 — m53 S1 part 2` (plan §R13.1 steps 3–5, §R12.1). Rulings: `_reviews/082-05-chat-ruling-s1-authorised.md`,
`082-04-chat-ruling-m53-revision-n1-n4-delta.md` (N2). Branch `feat/m53-uv-normal-corruption`.
Evidence: `D:\IntrusiveAnomalies\_reviews\082-06-evidence\`.

**Outcome: steps 3 and 4 done and verified; the cook ran clean; `G-COOK` FAILED on two rows of stop-list row 11, so the new
container was NOT staged and S1 stops here.** The bench is still exe `E0BE6F0A` + the Phase B container quintet. No leg, no game
launch, no tag.

## 0. Where the work ran

- The main checkout stayed on `m51` at `53bf725`, untouched.
- Plugin edits: the branch worktree `E:\IA_BuildCache\_r53_src\AnomalyInjector`.
- Editor commandlets and the cook: the scratch host `E:\IA_BuildCache\_r53_host\StackOBot` (082-05's), plugin worktree detached,
  moved `1fc20ea → c6a1153 → c9f8089` (no `Source/` change on either move).
- ⚠ **New this session, and it matters to the next reader: the host's `Content` is now a JUNCTION to the real
  `D:\IntrusiveAnomalies\StackOBot\Content`**, and `Plugins\RoomGenerator\Content` a junction to the real RoomGenerator content.
  That is what let the fixture be authored into the real project and the cook read the real content. **Anything an editor on the
  host saves under `/Game/` lands in the D: project.** M1/M2 are what prove nothing but the fixture folder was written.
- The host's `Config` (5 files) is byte-identical to the D: project's (checked file by file).

## 1. Step 3a — the plugin corruptor assets (`c9f8089`, `tools/create_texcorrupt_assets.py`)

| asset | type | settings | parameters |
|---|---|---|---|
| `/AnomalyInjector/Materials/M_CorruptTex_UV` (20,873 B, `9AC530F1…`) | Material | Surface, Unlit, **AlphaComposite**, two-sided, no vertex fog | textures `SrcColor` (Color), `SrcData` (LinearColor, see §6.1), `SrcNormal` (Normal); scalars `SrcKind` 0, `SrcMip` 0, `UvScale` 1, `UvOffsetU/V` 0, `UvSwap` 0, `ScrambleOn` 0, `ScrambleK` 8, `ScrambleAInv` 1, `ScrambleB` 0, `DbgChanSwap`, `DbgSrgbTwice`, `DbgTexelShift`, `TexelSizeU`, `TexelSizeV`, `DbgSkipNormalEncode`, `DbgOpaque` (all 0) |
| `/AnomalyInjector/Materials/M_CorruptTex_Normal` (14,336 B, `A71E9FEB…`) | Material | Surface, Unlit, **Opaque**, two-sided | textures `SrcNormal`, `NoiseNormal` (both Normal); scalars `SrcMip` 0, `NoiseMip` 0, `NormalSignX/Y` 1, `FlatMix` 0, `NoiseAmp` 0, `DbgChanSwap`, `DbgTexelShift`, `TexelSizeU`, `TexelSizeV`, `DbgSkipNormalEncode` (0) |
| `/AnomalyInjector/Textures/T_CorruptTex_NoiseN` (134,644 B, `53E5D130…`) | Texture2D | 256², tileable (8 integer-frequency sinusoids), `TC_Normalmap`, sRGB off, WorldNormalMap group, never streamed; max \|n.xy\| 0.796 | — |

- The parameter sets are exactly `UvCorruptorScalars()/Textures()` and `NormalCorruptorScalars()/Textures()` of `TexCorruptState.cpp`
  (read back with `get_scalar/texture_parameter_names`, no extras); the identity defaults were read back too.
- Every source sample is a `TextureSampleParameter2D` with `TMVM_MipLevel` fed by `SrcMip` (`NoiseMip` for the noise),
  `AutomaticViewMipBias` false and `SSM_FromTextureAsset` (plan §R7.1), read back per node.
- The graph math is in Custom nodes: the UV function (swap → `frac(uv·UvScale + offset)` → scramble inverse per §R7.5 with the
  mod reduced before the multiply so every integer stays < 2²⁴ → `uv.x += DbgTexelShift·TexelSizeU`, U only per 082-05 A), the encode
  (normal `n·0.5+0.5` into RGB, `DbgSkipNormalEncode` raw, exact sRGB encode for `DbgSrgbTwice`, `rgb.bgr` for `DbgChanSwap`,
  `Opacity = 1 − a` or 1 under `DbgOpaque`). `TexelSizeV` is wired in and unused (plan §R7.1).
- Compiled under `-AllowCommandletRendering`: UV 143 PS instructions / 3 samplers, Normal 132 / 2. A fresh editor session then
  resolved all three through the subsystem CDO (no `CDO Constructor … Failed to find`) and recompiled both on load.
- The script **refuses** to author over existing assets: the CDO holds them, so an in-editor delete leaves partially loaded packages
  that cannot be saved (measured on the second run: the first run's files were deleted from disk and the saves failed; recovered by
  re-running on the clean folder). The refusal was proven to fire.
- The hard CDO references already existed (082-05, `AnomalyInjectorSubsystem.cpp:61-69`); nothing changed there.

## 2. Step 3b — fixture images and the offline wrong-copy proofs (`CaptureBench/tools/texcorrupt_fixture_images.py`)

49 images (PNG; DDS for the authored chain and the cube), deterministic, stdlib only, written with
`texcorrupt_fixture_manifest.json` (SHA-256, import settings, role, proofs) to `082-06-evidence\fixture_src\`, and cross-checked
with Pillow and a direct DDS header read. The statistic is the per-texel max-channel |wrong − right| over the region the row reads.

| fault | row(s) | minimum level |
|---|---|---|
| `chanswap` | U-C1, U-C2, U-C3, U-C4, U-D1, U-D2, U-D3, U-D4, shared sampler | 120 |
| `chanswap` | U-D5 (BC4), U-D6 (G8) | 56 |
| `chanswap` | U-N1 / N-N1 (encoded n.x against encoded n.z) | 69 / 74 |
| `srgbtwice` | U-C1, U-C2, U-C3, U-C4 | 73 |
| `srgbtwice` | U-C5 (sRGB grayscale) | **37** (the lowest relied-on proof) |
| `alpha` | U-C2, U-C3, U-D2, U-D3 | 72 |
| `normal` | U-N1, N-N1 | 89 |
| `texelshift` (U only, Codex A) | chain (b) one-texel checker, mip 0 | **128 on every texel** |
| `texelshift` (U only, Codex A) | chain (d) BC5 checker, mip 0 | **89 on every texel** |
| `mipshift` | chain (a), levels 0..M−2 | 80 |
| `mipgen` | chain (a), levels 1..M−1 | 80 |
| `noclear` | redraw opaque BGRA8 linear | 112 |
| `noclear` | redraw fractional-alpha sRGB DXT5 | 51 |

**30 relied-on (fault, row) pairs, every minimum ≥ 32.** Not relied on, by construction and stated in the manifest: `mipshift` at
level M−1 (1×1 clamps to itself, 0) and `mipgen` at level 0 (drawn from mip 0, 0). The (c) chains carry no offline proof (I5).
The predictions of plan §R12.4 hold or are exceeded (the palette keeps channels in [48, 184], inside the plan's [48, 192]).

## 3. Step 3c — the fixture (`make_texcorrupt_fixture.py`) and M1

- `/Game/CaptureBenchTexCorrupt/`: 49 textures, 46 materials + 1 material-layer function, 4 duplicated meshes, the level
  `CB_TexCorruptLevel` (standard level, not World Partition) with 77 actors: 17 encoding/alpha/shared/redraw tiles, the six chains ×
  {magnification, grazing, minification, transition} plus reference-bug copies (×2 UV scale and `uv.yx` swap of chains (a) and (b))
  at the same four geometries, the layer/alias/dead-binding targets, the asset-slot-MID target, the reason producers (translucent,
  VT, cube, UI 256², UI 1×1, HalfFloat, cinematic, 32² pair, nine maps, two 4096², Nanite, skeletal, empty slot, normal→emissive),
  the far streamed 2048², a mesh-less `TargetPoint`, movable sun/sky light/atmosphere and the `PlayerStart` (eye (0, 0, 500), yaw 0).
- Actor object names, labels (= tags) and roles: `082-06-evidence\texcorrupt_fixture_actors.json` (e.g. `TC_UC1` =
  `StaticMeshActor_0`). Layout checked offline by projecting every actor's real bounds through the eye/90° model: 76 of 76 visual
  actors on screen, none behind a nearer one (after one fix: the first layout pushed two grazing strips off-screen and let the 3D cubes
  overlap their neighbours).
- Author mode runs **without** commandlet rendering so the skeletal fixture's material cannot gain `bUsedWithSkeletalMesh`; verify
  mode (rendering on) compiled all 46 materials and checked all 49 textures against the manifest's settings. That check caught and
  proved itself on a real defect first: **the importer auto-enabled virtual texturing on the two 4096² textures**
  (`r.VirtualTextures=True` here), which would have turned the `over_budget` producer into a `virtual_texture` one. The tool now sets
  the VT flag explicitly on every texture.
- Guards: the folder guard refused without `--allow-overwrite-texcorrupt-fixture` (proven); every created path asserted under the
  folder; zero dirty packages outside it on every run.
- **M0 re-check** before any authoring: CLEAN, 2,082/2,082. **M1: CLEAN** — all 2,082 M0 files byte-identical (`CB_GateLevel.umap`
  `1d89de17…`, `MainWorld.umap` `a3849daa…`, MainWorld's 421 external actors/objects), 0 removed, 104 added, all allowed (101 under
  `CaptureBenchTexCorrupt\`, the 3 plugin assets). `texcorrupt_bytecheck.ps1` was first proven able to fail (a mutated M0 produced
  CHANGED, REMOVED and ADDED-DISALLOWED lines and exit 1).

## 4. Step 4 — archive

`_binary_baselines\m53-s1-precook-container-67EA1FE0\`: `StackOBot.exe` `E0BE6F0A`, `StackOBot-Windows.utoc` `67EA1FE0`,
`.ucas` `2CEFB8F4`, `.pak` `E03C6610`, `global.utoc` `C70ECDAA`, `global.ucas` `A16A18A8` — **every file hash-verified at the
destination before the cook** (`archive-precook.json`). IoStore listing of the archive: `iostore-precook.csv`, 1,972 chunks.

## 5. Step 5 — the cook, and G-COOK

- Editor target rebuilt first on the host at `c9f8089`: **Target is up to date** (the source equals `1fc20ea`, built in 082-06c);
  A44 on `UnrealEditor-AnomalyInjector.dll`: `TEXCORRUPT-ASSETS` 1, `M_CorruptTex_UV` 4, `IAI.Bench.TexCorruptNoApply` 5,
  `collateral_unresolved` 1 (an 082-06c token), control `IAI.Bench.StuckMipNoHold` 5, all UTF-16.
- Cook (`cook-command.txt`): runbook §8.6 on the host, `-map=` + `/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel`, archived to
  `E:\IA_BuildCache\_r53_cookout` (not into `Builds\BenchGate`, so nothing on the bench tree could be touched). **BUILD SUCCESSFUL,
  2 m 05 s, 894 packages, 0 warnings, 0 errors**; both build steps "Target is up to date".
- Candidate (cook 1): exe **`2FCDF059`** (the 082-06c game build, = `StackOBot.exe.m53-s1rem-2FCDF059`), `StackOBot-Windows.utoc`
  `651656EF`, `.ucas` `F01CAFC0`, `.pak` `EC4EB9A7`, `global.utoc` `462B8AC6`, `global.ucas` `BB05CF99`. A44 on the candidate exe:
  every new token present beside `IAI.Capture.ShaderPrewarm` 7 and `IAI.Bench.StuckMipNoHold` 5.
- Map gate on the candidate with `-Required CB_GateLevel,MainMenu,MainWorld,Entry,CB_TexCorruptLevel`: **exit 0** (the default set
  names `CB_TexCorruptLevel` as unexpected, so it is named, not silenced).
- **M2: CLEAN**, and M1 = M2 exactly (the cook changed no source byte).

**G-COOK**

| criterion | reading | verdict |
|---|---|---|
| M1 and M2 vs M0 outside the allowed paths | 0 changed, 0 removed, 0 disallowed, both | ✅ |
| archive hash-verified before the cook | 6/6 | ✅ |
| map gate exits 0 with the new level named | exit 0 | ✅ |
| `CB_GateLevel` cooked chunk hash | `0x01AD80B5…` before and after | ✅ |
| **`MainWorld` cooked chunk hash** (its externals are baked into the one `ExportBundleData` chunk) | `0x0B2F9571…` → **`0xA9D83F53…`**, same 7,898,974 B | ❌ |
| **the new packages present** (positive control) | 96 of 101 fixture packages + the 3 plugin assets; **5 fixture packages missing** | ❌ |
| other packages (D) | non-shader chunks: 927 unchanged, 6 changed — `MainWorld`, the container header, the four shader archives (global SM5/SM6 same size, other hash; StackOBot SM5/SM6 grew with the new materials); 0 removed; 106 added (100 export bundles + 6 bulk: the fixture, the plugin assets, and `SkeletalCube_Skeleton` as a dependency). The plugin's `Shaders/` has not changed since m49 Phase B, so the global archive change is a cook-environment reading, not investigated | D |

**⇒ G-COOK FAILS (stop-list row 11, two rows). The container was NOT staged; S1 stops.**

### 5.1 The `MainWorld` row — measured, no mechanism asserted

- The source is unchanged (M1/M2: `MainWorld.umap` and all 421 externals byte-identical to M0). The loose cooked `MainWorld.umap`
  header is byte-identical between the 2026-09-04 cook and cook 1 (`378CFE5D…`); only `MainWorld.uexp` differs: **1,603 bytes in
  451 runs, same size**, in one span (offsets 3,868,679–5,427,978), mostly the fourth float of repeated 16-byte (X, Y, Z, value∈[0,1])
  records along a line of positions (`mainworld-uexp-diff.txt`).
- **Control (a diagnostic, not a retry): a second cook of identical inputs**, same host, same binaries, 50 s later, into
  `E:\IA_BuildCache\_r53_cookout_ctl`: `MainWorld` = **`0x2B46C64E…`**, different again; 1,612 bytes / 451 runs differ from cook 1;
  every other non-shader chunk identical between the two cooks except the two StackOBot shader archives (same size).
  ⇒ **`MainWorld`'s cooked bytes are not reproducible run to run, so "the `MainWorld` chunk hash is unchanged" cannot be met by any
  cook, with or without N2.** `CB_GateLevel` is reproducible.
- Where the N2 cook's difference sits: at 4-byte-word granularity (alignment 1), 696 of the 697 words that differ between the
  2026-09-04 cook and cook 1 also differ between the two identical-input cooks, and 0 fall outside the union of the two comparisons;
  the byte span is identical (`mainworld-words.json`). An observation. **The mechanism (for example unseeded randomness evaluated
  at cook time) is NOT established and was not chased (G120).**

### 5.2 The positive-control row — a defect in my fixture tool

- Missing from the container: `M_TC_AssetMid`, `T_TC_AssetMid`, `M_TC_NaniteBase`, `M_TC_NaniteOverride`, `M_TC_SkelNoUsage` —
  exactly the materials meant for the duplicated meshes' asset slots.
- Cause, read back from the saved assets: **the slot assignments never persisted.** `SM_TC_PlaneAssetMid`, `SM_TC_CubeNanite` and
  `SM_TC_PlaneNoMat` still hold `WorldGridMaterial`; `SK_TC_Cube` holds `None`. The tool modified struct elements taken from
  `get_editor_property("static_materials")` / `("materials")`, and UE's Python returns those elements **by copy**, so the edits were
  dropped and the unmodified array written back. Nothing errored; the verify pass did not read mesh slots. (G310.)
- Consequence in this cook: the `slot_empty`, asset-slot-MID (`host_mid` / `asset_slot`), `nanite_override` and
  `default_material_path` producers do not produce those reasons. Every other fixture row is as designed.
- **Fix, NOT applied (the stop had fired):** set slots with `UStaticMesh::SetMaterial(i, M)` (or build a new struct list and assign
  it), use `SkeletalMesh.set_editor_property("materials", …)` with freshly constructed `SkeletalMaterial` structs, and make verify
  read every duplicated mesh's slots back. Then re-author (`--allow-overwrite-texcorrupt-fixture`), M1, re-cook, G-COOK.

## 6. Deviations, each with its reason

1. **`SrcData` uses the LinearColor sampler, not Masks.** A Masks sampler must default to a `TC_Masks` texture, and a search of all
   `/Engine` textures found none. Masks and LinearColor emit the same raw lookup (`MaterialTexture.ush:168-171`, `:231+`), so the
   corruptor's arithmetic is identical; only the editor-side sampler label differs.
2. **Readout sampler types follow the engine's rule** (`GetSamplerTypeForTexture`), so U-C5 (sRGB grayscale) reads through
   `Grayscale` and U-D6 through `LinearGrayscale`, where §R12.4's table says Color / Grayscale. A mismatched readout does not compile.
3. **The layer target** has the global `Tex` (uniform colour) on one half and a background-layer `Tex` (patterned) on the other, so a
   correct ×2 tile changes only the layer half while every binding is transformed; mis-addressed parameters would show in either half.
4. **Grazing geometry** is a 400 cm ceiling strip 250 cm above the eye (depression ~12°) rather than a yawed wall plane, because a
   yawed plane's near end fans far outside its screen slot.
5. **The cook ran on the scratch host with junctioned content**, not on the D: project, because the D: project builds the `m51`
   plugin. Its config and content inputs are byte-identical (checked); the exe carries the host's provenance strings (081-44 note).
6. **Not staged, and step 5's A44 was taken on the candidate exe in the archive directory**: G-COOK failed before staging.
7. **One diagnostic re-cook** (the identical-input control of §5.1). It cannot change the verdict, which compares against the archived
   pre-cook container.

## 7. State at the end

- Plugin branch: `c9f8089` (assets + script) and this journal/status commit; pushed. `m51` and `master` untouched; no tag.
- CaptureBench (local-only repo, no remote): `8e53dea` — `texcorrupt_bytecheck.ps1` `AB95C795…`, `texcorrupt_fixture_images.py`
  `8CD8BE1D…`, `make_texcorrupt_fixture.py` `CCFF5834…` (SHA-256 prefixes of the committed files).
- The D: project's `Content\CaptureBenchTexCorrupt\` holds the fixture **with the §5.2 defect**; the next author run must use the
  overwrite flag.
- Bench: exe `E0BE6F0A` + `67EA1FE0`/`2CEFB8F4`/`E03C6610` + `A16A18A8`/`C70ECDAA`, unchanged, archived at the path above.
- Cook outputs kept on E: (`_r53_cookout`, `_r53_cookout_ctl`); the scratch host and both worktrees kept for 082-07.

## 8. NEEDS-DECISION

1. **The `MainWorld` Q row is unsatisfiable as written.** A proposal only (not applied): qualify `MainWorld` the way G-S3a-1 was
   amended — a same-input control cook pair establishes the run-to-run difference set, and the N2 cook's difference from the archived
   container must fall inside it (read at word level), with the loose `.umap` header byte-identical; `CB_GateLevel` keeps exact
   identity. Today's reading under that rule: 696/697 words inside the one control pair, 0 outside both comparisons, header identical.
2. **Authorise the §5.2 fixture fix and the re-cook** (and whether the next cook stays on the scratch host with junctioned content).
