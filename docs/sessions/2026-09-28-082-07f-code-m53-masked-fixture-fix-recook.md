# 082-07f — m53 S1: `TC_Masked` diagnosed, every fixture row audited, fixed, re-cooked, G-COOK passes, STAGED; the PROBE stops on a neighbourhood halo

Session 082-07f, 2026-09-28 (IST), Claude Code (Opus 5.5), headless from the GDP mailbox, continuing 082-07e. Ruling:
`_reviews/082-07e-chat-ruling-masked-fixture.md`. Branch `feat/m53-uv-normal-corruption`. Evidence:
`D:\IntrusiveAnomalies\_reviews\082-07f-evidence\`. No leg, no plugin source change, no plugin build, no tag.

**Outcome.**
- The cause is named: `M_TC_Masked` was saved with `bCanMaskedBeAssumedOpaque = true`, so `UMaterial::GetBlendMode()`
  returned **Opaque** (G323). The audit of all 42 channel-dependent fault rows and 46 identity rows finds no other
  defect. The fixture is fixed and proven from disk, the offline MASKED proof exists, G-COOK passes, and the build is
  staged.
- The PROBE shows the tile now clips ((a) PASS). ⛔ (b) and (c) read up to 7 and 5 **within 48 px of the re-authored tile
  only** (far field ≤ 1): that is the brief's stop condition, reported, not waived. Same-build evidence (§7.3) shows it
  is a renderer neighbourhood halo that any tile change produces. It also makes G-BIND (3)'s "outside the target ≤ 2"
  clause unsatisfiable, which the amendment must change.

## 1. Diagnosis (§1 of the brief), evidence-first

| candidate (ruling 2) | reading | source |
|---|---|---|
| the actor's slot material | `StaticMeshActor_13` (`TC_Masked`) `StaticMeshComponent0` override = resolved = `M_TC_Masked`; in-game `TEXCORRUPT-DECIDE … resolved=M_TC_Masked effective=M_TC_Masked` on all 8 fires | `diag_masked.json` (level loaded from disk), 082-07e `NULL_MASKED` game.log |
| blend mode (authored) | `BLEND_MASKED` | `diag_masked.json` |
| OpacityMask connection | `TextureSampleParameter2D` `Tex`, output **`A`**; Emissive ← the same sampler, `RGB` | `diag_masked.json` |
| clip value | 0.5 | `diag_masked.json` |
| a MIC override | none: the slot holds the `Material` itself | `diag_masked.json` |
| `T_TC_UC2` settings | `TC_DEFAULT`, sRGB, `compression_no_alpha` false; alphas 72 / 104 / 152 / 184 in four equal quarters | `diag_masked.json`, the source PNG |
| alpha survives the cook | the U-C2 null tile (Opaque `AlphaSplitReadout`, same `T_TC_UC2`, same build) shows four alpha greys (127 / 153 / 180 / 192) that map back through its colour half to a ≈ 0.28 / 0.41 / 0.60 / 0.72 | 082-07e bank `M53S1_NULL_UC2_A1` |
| **the effective blend mode** | **`GetBlendMode()` = `BLEND_OPAQUE`** as loaded from disk; `BLEND_MASKED` after an in-memory `RecompileMaterial` (not saved); every other fixture material's effective blend equals its authored one | `diag_blend.json`, `diag-blend.log` |

- **Cause (named, with evidence).** `UMaterial::GetBlendMode()` returns `BLEND_Opaque` for a Masked material whose
  `bCanMaskedBeAssumedOpaque` is true (`Material.cpp:6177-6185`). That flag is a saved `UPROPERTY`, recomputed **only** in
  `PostEditChangePropertyInternal` (`Material.cpp:4383`: true iff OpacityMask has no expression).
- `make_texcorrupt_fixture.py`'s `readout(kind="masked")` did three things in this order:
  1. set `blend_mode` in `new_material`, which runs PostEditChange;
  2. set `opacity_mask_clip_value`, which runs PostEditChange again, with OpacityMask still unconnected;
  3. connected OpacityMask via `MaterialEditingLibrary::ConnectMaterialProperty`, which does not run PostEditChange.

  Nothing recompiled it before the save, so the flag stayed true.
- The saved package carries the name `bCanMaskedBeAssumedOpaque` (as does every fixture material; it is harmless
  wherever the blend mode is not Masked).
- **Consequence:** the host tile renders Opaque in every leg, in both builds, including legs that never target it
  (`CTL_A`, `ID_UC1`), and its custom-depth mask covers the full 4,096 px.
- **It is in CaptureBench content, not an engine or project setting.** `Config/*.ini` sets nothing that affects masking,
  and the leg ExecCmds carry no masking cvar.

## 2. The audit (G322 on every row, before any cook)

`audit_fixture.py` joins:
- the locked leg list (read-only import);
- the post-fix disk read-back of the level's slots and every material's inputs (`diag_audit.json`, from
  `diag_masked.py`);
- the verify's effective blend modes;
- the offline proofs (082-06 manifest plus the two new ones).

Run on the pre-fix read-back (`audit-prefix.json`), it flags exactly `WC_MASKED_ALPHA` (the alpha does not reach the clip:
effective blend Opaque) and `ID_MASKED` (effective ≠ authored). Post-fix (`audit.json`): **0 defects**.

| leg | row | class | actor | slot material | fault | channel route (disk read-back) | offline proof (min) | verdict |
|---|---|---|---|---|---|---|---|---|
| `WC_UC1_CHANSWAP` | U-C1 | Q | `StaticMeshActor_0` | `M_TC_UC1` | chanswap | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 120 (082-06 manifest T_TC_UC1/chanswap) | OK |
| `WC_UC1_SRGBTWICE` | U-C1 | Q | `StaticMeshActor_0` | `M_TC_UC1` | srgbtwice | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 73 (082-06 manifest T_TC_UC1/srgbtwice) | OK |
| `WC_UC2_CHANSWAP` | U-C2 | Q | `StaticMeshActor_1` | `M_TC_UC2` | chanswap | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | 120 (082-06 manifest T_TC_UC2/chanswap) | OK |
| `WC_UC2_SRGBTWICE` | U-C2 | Q | `StaticMeshActor_1` | `M_TC_UC2` | srgbtwice | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | 73 (082-06 manifest T_TC_UC2/srgbtwice) | OK |
| `WC_UC2_ALPHA` | U-C2 | Q | `StaticMeshActor_1` | `M_TC_UC2` | alpha | alpha reaches Emissive through AlphaSplitReadout | 72 (082-06 manifest T_TC_UC2/alpha) | OK |
| `WC_UC3_CHANSWAP` | U-C3 | Q | `StaticMeshActor_2` | `M_TC_UC3` | chanswap | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | 120 (082-06 manifest T_TC_UC3/chanswap) | OK |
| `WC_UC3_SRGBTWICE` | U-C3 | Q | `StaticMeshActor_2` | `M_TC_UC3` | srgbtwice | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | 73 (082-06 manifest T_TC_UC3/srgbtwice) | OK |
| `WC_UC3_ALPHA` | U-C3 | Q | `StaticMeshActor_2` | `M_TC_UC3` | alpha | alpha reaches Emissive through AlphaSplitReadout | 72 (082-06 manifest T_TC_UC3/alpha) | OK |
| `WC_UC4_CHANSWAP` | U-C4 | Q | `StaticMeshActor_3` | `M_TC_UC4` | chanswap | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 120 (082-06 manifest T_TC_UC4/chanswap) | OK |
| `WC_UC4_SRGBTWICE` | U-C4 | Q | `StaticMeshActor_3` | `M_TC_UC4` | srgbtwice | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 73 (082-06 manifest T_TC_UC4/srgbtwice) | OK |
| `WC_UC5_SRGBTWICE` | U-C5 | Q | `StaticMeshActor_4` | `M_TC_UC5` | srgbtwice | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 37 (082-06 manifest T_TC_UC5/srgbtwice) | OK |
| `WC_UD1_CHANSWAP` | U-D1 | Q | `StaticMeshActor_5` | `M_TC_UD1` | chanswap | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 120 (082-06 manifest T_TC_UD1/chanswap) | OK |
| `WC_UD2_CHANSWAP` | U-D2 | Q | `StaticMeshActor_6` | `M_TC_UD2` | chanswap | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | 120 (082-06 manifest T_TC_UD2/chanswap) | OK |
| `WC_UD2_ALPHA` | U-D2 | Q | `StaticMeshActor_6` | `M_TC_UD2` | alpha | alpha reaches Emissive through AlphaSplitReadout | 72 (082-06 manifest T_TC_UD2/alpha) | OK |
| `WC_UD3_CHANSWAP` | U-D3 | Q | `StaticMeshActor_7` | `M_TC_UD3` | chanswap | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | 120 (082-06 manifest T_TC_UD3/chanswap) | OK |
| `WC_UD3_ALPHA` | U-D3 | Q | `StaticMeshActor_7` | `M_TC_UD3` | alpha | alpha reaches Emissive through AlphaSplitReadout | 72 (082-06 manifest T_TC_UD3/alpha) | OK |
| `WC_UD4_CHANSWAP` | U-D4 | Q | `StaticMeshActor_8` | `M_TC_UD4` | chanswap | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 120 (082-06 manifest T_TC_UD4/chanswap) | OK |
| `WC_UD5_CHANSWAP` | U-D5 | Q | `StaticMeshActor_9` | `M_TC_UD5` | chanswap | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 56 (082-06 manifest T_TC_UD5/chanswap) | OK |
| `WC_UD6_CHANSWAP` | U-D6 | Q | `StaticMeshActor_10` | `M_TC_UD6` | chanswap | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 56 (082-06 manifest T_TC_UD6/chanswap) | OK |
| `WC_UN1_NORMAL` | U-N1 | Q | `StaticMeshActor_11` | `M_TC_UN1` | normal | NormalReadout -> Emissive, Tex -> Normal (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 89 (082-06 manifest T_TC_UN1/normal) | OK |
| `WC_UN1_CHANSWAP` | U-N1 | Q | `StaticMeshActor_11` | `M_TC_UN1` | chanswap | RGB reaches Emissive (MaterialExpressionCustom:NormalReadout.) | 69 (082-06 manifest T_TC_UN1/chanswap) | OK |
| `WC_NN1_NORMAL` | N-N1 | Q | `StaticMeshActor_12` | `M_TC_NN1` | normal | NormalReadout -> Emissive, Tex -> Normal (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 89 (082-06 manifest T_TC_NN1/normal) | OK |
| `WC_NN1_CHANSWAP` | N-N1 | Q | `StaticMeshActor_12` | `M_TC_NN1` | chanswap | RGB reaches Emissive (MaterialExpressionCustom:NormalReadout.) | 74 (082-06 manifest T_TC_NN1/chanswap) | OK |
| `WC_MASKED_ALPHA` | MASKED | Q | `StaticMeshActor_13` | `M_TC_Masked` | alpha | alpha reaches the clip: OpacityMask MaterialExpressionTextureSampleParameter2D:Tex.A, effective blend BLEND_MASKED | 138 (082-07f offline MASKED proof (crossing alphas [152, 184])) | OK |
| `WC_SHARED_CHANSWAP` | SHARED | Q | `StaticMeshActor_14` | `M_TC_Shared` | chanswap | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 120 (082-06 manifest T_TC_Shared/chanswap) | OK |
| `WC_MA_MIN_MIPSHIFT` | M-A-MIN | Q | `StaticMeshActor_37` | `M_TC_ChainA` | mipshift | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 80 (082-06 manifest T_TC_ChainA/mipshift) | OK |
| `WC_MA_MIN_MIPGEN` | M-A-MIN | Q | `StaticMeshActor_37` | `M_TC_ChainA` | mipgen | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 80 (082-06 manifest T_TC_ChainA/mipgen) | OK |
| `WC_MA_GRAZE_MIPSHIFT` | M-A-GRAZE | Q | `StaticMeshActor_27` | `M_TC_ChainA` | mipshift | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 80 (082-06 manifest T_TC_ChainA/mipshift) | OK |
| `WC_MA_TRANS_MIPSHIFT` | M-A-TRANS | Q | `StaticMeshActor_47` | `M_TC_ChainA` | mipshift | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 80 (082-06 manifest T_TC_ChainA/mipshift) | OK |
| `WC_MA_TRANS_MIPGEN` | M-A-TRANS | Q | `StaticMeshActor_47` | `M_TC_ChainA` | mipgen | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 80 (082-06 manifest T_TC_ChainA/mipgen) | OK |
| `WC_MB_MAG_TEXELSHIFT` | M-B-MAG | Q | `StaticMeshActor_18` | `M_TC_ChainB` | texelshift | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 128 (082-06 manifest T_TC_ChainB/texelshift) | OK |
| `WC_MCSHARP_MIN_MIPGEN_D` | M-CSHARP-MIN | D | `StaticMeshActor_39` | `M_TC_ChainCSharpen` | mipgen | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | NO PROOF for T_TC_ChainCSharpen/mipgen | D: not provable offline, reported only |
| `WC_MCSHARP_TRANS_MIPGEN_D` | M-CSHARP-TRANS | D | `StaticMeshActor_49` | `M_TC_ChainCSharpen` | mipgen | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | NO PROOF for T_TC_ChainCSharpen/mipgen | D: not provable offline, reported only |
| `WC_MCBLUR_MIN_MIPGEN_D` | M-CBLUR-MIN | D | `StaticMeshActor_40` | `M_TC_ChainCBlur` | mipgen | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | NO PROOF for T_TC_ChainCBlur/mipgen | D: not provable offline, reported only |
| `WC_MCBLUR_TRANS_MIPGEN_D` | M-CBLUR-TRANS | D | `StaticMeshActor_50` | `M_TC_ChainCBlur` | mipgen | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | NO PROOF for T_TC_ChainCBlur/mipgen | D: not provable offline, reported only |
| `WC_MCACOV_MIN_MIPGEN_D` | M-CACOV-MIN | D | `StaticMeshActor_41` | `M_TC_ChainCAlphaCov` | mipgen | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | NO PROOF for T_TC_ChainCAlphaCov/mipgen | D: not provable offline, reported only |
| `WC_MCACOV_TRANS_MIPGEN_D` | M-CACOV-TRANS | D | `StaticMeshActor_51` | `M_TC_ChainCAlphaCov` | mipgen | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | NO PROOF for T_TC_ChainCAlphaCov/mipgen | D: not provable offline, reported only |
| `WC_MD_MAG_TEXELSHIFT` | M-D-MAG | Q | `StaticMeshActor_22` | `M_TC_ChainD` | texelshift | RGB reaches Emissive (MaterialExpressionCustom:NormalReadout.) | 89 (082-06 manifest T_TC_ChainD/texelshift) | OK |
| `WC_MD_MAG_NM_TEXELSHIFT` | M-D-MAG_NM | Q | `StaticMeshActor_22` | `M_TC_ChainD` | texelshift | RGB reaches Emissive (MaterialExpressionCustom:NormalReadout.) | 89 (082-06 manifest T_TC_ChainD/texelshift) | OK |
| `RDNOCLEAR_OPAQUE` | RD-OPAQUE | Q | `StaticMeshActor_15` | `M_TC_RedrawOpaque` | noclear | RGB reaches Emissive (MaterialExpressionTextureSampleParameter2D:Tex.RGB) | 112 (082-06 manifest T_TC_RedrawOpaque/noclear) | OK |
| `RDNOCLEAR_ALPHA` | RD-ALPHA | Q | `StaticMeshActor_16` | `M_TC_RedrawAlpha` | noclear | RGB reaches Emissive (MaterialExpressionCustom:AlphaSplitReadout.) | 51 (082-06 manifest T_TC_RedrawAlpha/noclear) | OK |
| `BIND_TILE` | BIND-LAYER | Q | `StaticMeshActor_57` | `M_TC_Layer` | tile_probe_2 | both halves reach Emissive through LayerSplit | 16 (082-07f offline tile-probe proof: layer max 120, 12288/16384 texels >= 16; global max 0) | OK |

Identity rows (46, F-SYN): `ID_UC1`, `ID_UC2`, `ID_UC3`, `ID_UC4`, `ID_UC5`, `ID_UD1`, `ID_UD2`, `ID_UD3`, `ID_UD4`, `ID_UD5`, `ID_UD6`, `ID_UN1`, `ID_NN1`, `ID_MASKED`, `ID_SHARED`, `ID_MA_MAG`, `ID_MA_MIN`, `ID_MA_GRAZE`, `ID_MA_TRANS`, `ID_MB_MAG`, `ID_MB_MIN`, `ID_MB_GRAZE`, `ID_MB_TRANS`, `ID_MCSHARP_MAG`, `ID_MCSHARP_MIN`, `ID_MCSHARP_GRAZE`, `ID_MCSHARP_TRANS`, `ID_MCBLUR_MAG`, `ID_MCBLUR_MIN`, `ID_MCBLUR_GRAZE`, `ID_MCBLUR_TRANS`, `ID_MCACOV_MAG`, `ID_MCACOV_MIN`, `ID_MCACOV_GRAZE`, `ID_MCACOV_TRANS`, `ID_MD_MAG`, `ID_MD_MAG_NM`, `ID_MD_MIN`, `ID_MD_MIN_NM`, `ID_MD_GRAZE`, `ID_MD_GRAZE_NM`, `ID_MD_TRANS`, `ID_MD_TRANS_NM`, `RD_OPAQUE`, `RD_ALPHA`, `BIND_ID` - every slot holds its readout, every Emissive is connected, and every effective blend equals the authored one: **0 defects**.

- **Rows with no channel dependence:**
  - G3 is judged applied against null at the same index.
  - G4, G-REASON, G6 and G-BIND (2)/(4) are judged from logs.
  - Their slots are covered by the identity-row read-back.
- **The D-class `mipgen` (c) rows** are declared not provable offline (plan I5) and remain readings.

## 3. The fixture fix (CaptureBench `2c5ec56`, `tools/make_texcorrupt_fixture.py`, SHA-256 `B4A4DD28…`)

- **Author path:** `readout(kind="masked")` now calls `RecompileMaterial` after the OpacityMask connection, and fails
  unless `GetBlendMode()` reads Masked.
- **New `masked` mode** (`TEXCORRUPT_FIXTURE_MODE=masked`, no rendering):
  - loads only `M_TC_Masked`;
  - requires its graph to be the one diagnosed;
  - recompiles it and requires the effective blend mode Masked;
  - saves that package only, and fails on any other dirty package.
- **Verify additions:**
  - `blend_proofs`: every fixture material's effective blend mode equals its authored one;
  - `masked_proofs`: `M_TC_Masked` read from disk, with authored and effective blend Masked, clip 0.5,
    OpacityMask ← `Tex`.A, Emissive ← the same sampler's RGB, `Tex` = `T_TC_UC2` with alpha;
  - two offline proofs, from the source PNGs through a pure-Python reader, which matches Pillow on every fixture PNG.
- **Negative control first:** the new verify on the unfixed fixture failed on **exactly 2 checks**, both `M_TC_Masked`
  (blend proof and masked proof), and nothing else (`verify-negative\`).
- **Read-back after the fix:** `masked` mode reads `BLEND_OPAQUE → BLEND_MASKED`, with the file SHA-256 going
  `49f132a6… → b687fa92…`. Verify in a separate process reports 0 problems. All 4 slot proofs and all 4 normal proofs pass.
  46 materials compile and 49 textures match the manifest.
- **The offline MASKED proof** (`texcorrupt_fixture_verify.json` → `offline_masked`):

  | alpha | quarter colour (stored sRGB) | right copy (A = a) | alpha fault (A = 0) | crosses the 0.5 clip | min \|d\| vs the sky box |
  |---|---|---|---|---|---|
  | 72 (0.28) | (176,112,56) orange | clipped | clipped | no | 0 (not relied on) |
  | 104 (0.41) | (56,64,176) blue | clipped | clipped | no | 0 (not relied on) |
  | 152 (0.60) | (56,184,176) cyan | drawn | clipped | **yes** | **138** |
  | 184 (0.72) | (176,160,56) yellow | drawn | clipped | **yes** | **140** |

  - **Predicted min |d| = 138**, against a proof floor of 32 and a gate of 16.
  - The sky box, per channel R 12–36 / G 27–46 / B 47–69, is the one non-image input. It was measured beside the tile in
    082-07e's null leg (1,792 px on each of 5 frames).
  - Cells are 16 × 16 uniform texels, so mips 0–3 keep each cell's alpha exactly (the tile samples mip 1).
- **The offline tile-probe proof (G-BIND (3)):**
  - Tile probe 2 = mip 1 repeated 2 × 2.
  - `T_TC_LayerLayer`: max d 120, with 12,288 of 16,384 texels ≥ 16.
  - `T_TC_LayerGlobal`: one colour, max d 0.

## 4. Manifests (CaptureBench `texcorrupt_bytecheck.ps1`, baseline 082-05 M0)

| manifest | when | vs M0 | vs the previous |
|---|---|---|---|
| M1pre | before anything | CLEAN (2,082 unchanged, 104 allowed) | ≡ 082-07c's M2″ (0 differences) |
| M1a | after the masked run | CLEAN | exactly `CaptureBenchTexCorrupt/Materials/M_TC_Masked.uasset` changed |
| **M1‴** | after the verify run | CLEAN | ≡ M1a (verify wrote nothing) |
| M1‴-b | after the post-fix read-back | CLEAN | ≡ M1‴ |
| M2‴-prestage | after the cooks | CLEAN | ≡ M1‴ |
| **M2‴** | after staging | CLEAN | ≡ M1‴ |

## 5. Cooks and G-COOK

- The rule was written first: `gcook-predeclare.md`, SHA-256 `36F325E0…`.
  - Cooks: **f1 the candidate**, f2 and f3 the controls.
  - Region R = 082-06d's 13 cooks ∪ 082-07c's 3 ∪ these 3.
- The cook command is the same as 082-06d and 082-07c. The runner used `MSBUILDDISABLENODEREUSE=1` and `WaitForExit` (G316),
  and no node processes were left.
- **f1 / f2 / f3:** BUILD SUCCESSFUL, 899 packages each, 0 warnings, 0 errors; 64 s / 47 s / 47 s.

| cook | exe | utoc | ucas | pak | global.utoc / .ucas |
|---|---|---|---|---|---|
| **f1 (candidate)** | `2FCDF059` | `20DA6F98` | `534C5863` | `FD766B7B` | `462B8AC6` / `BB05CF99` |
| f2 | `2FCDF059` | `A6B75C71` | `190AACA3` | `FD766B7B` | same |
| f3 | `2FCDF059` | `2A7AD2C1` | `1ECB35C7` | `FD766B7B` | same |

| row | reading | verdict |
|---|---|---|
| map gate on f1 (`mapgate-f1.txt`) | five maps present, exit 0; the default-set run exits 2 on `CB_TexCorruptLevel` (the known negative control) | ✅ |
| Q-GATE | `CB_GateLevel` chunk `0x01AD80B5B5F83E…` in A, f1, f2, f3 | ✅ |
| Q-MW | `.uheader` `b50c9352…` identical in **20 containers**; `.uexp` 7,728,196 B; diff(A, f1) 1,598 bytes, **0 words outside R at every alignment**, 0 bytes (and 0 outside the prior region alone) | ✅ |
| Q-FIX | fixture **101/101** + plugin **3/3** in f1, f2, f3 | ✅ |
| Q-MASK (reading) | e1 → f1 non-shader: **`M_TC_Masked`** + the two shader archives + `MainWorld` + the container header; 0 removed, 0 added; shader chunks 1,075 → 1,077 | D |

**G-COOK: PASS** (`gcook-082-07f.json` / `.txt`).

## 6. Staging, A44, archive

- **Rollback archive:** `_binary_baselines\m53-s1-normalfix-cook-30FE0FDE\` was re-verified 6/6 against the staged files
  before staging.
- **Candidate archive:** `_binary_baselines\m53-s1-maskedfix-cook-20DA6F98\`, full SHA-256 verified 6/6 at the destination
  (`archive-candidate.json`).
- **Staged 01:27 IST**, with 0 StackOBot processes (`staging-receipt.json`), each file verified at the destination:
  - utoc `30FE0FDE → 20DA6F98`;
  - ucas `11231356 → 534C5863`;
  - exe `2FCDF059`, pak `FD766B7B`, `462B8AC6` and `BB05CF99` unchanged.
- **Provenance:** an entry in `_binary_baselines\README.md`.
- **A44 on the staged exe** (`a44-staged.txt`): the token table is identical to 082-07c's (the same exe). Every m53 token
  appears in UTF-16 only, and the controls read 7 / 5 / 5.

## 7. The PROBE (not a gate; label `PROBE_M53_082_07F_NULL_MASKED` everywhere)

### 7.1 Setup and results

- **Launcher.** `probe_082_07f.py` uses the harness read-only, with `IAI_R53_ROOT` pointed at `probe_run\`. The harness's
  event log, wait budget and input windows are all written there.
- **Recipe.** It builds the exact `NULL_MASKED` recipe from the locked leg list (`Identity 1` + `NoApply 1` on
  `StaticMeshActor_13`, 100 frames).
- **Gates.** It passed the harness's `wait_quiet` (QUIET) and `pp_gate`, then launched through `082-07-run.ps1`.
- **Session.** It moved the session off the exe side, verified by hash, 165 files.
- **Results:**
  - runner COMPLETE;
  - harness `validity()` 0 errors, with the pose equal to the live reference;
  - no person evidence.
- **Probe postflight (snapshot-based) PASS:**
  - no process of ours left;
  - the live run dir, the bank listing and the exe side byte-unchanged;
  - `m51` and the feature head unchanged;
  - the staged build equals the new set.
- **The locked `082-07-postflight.py`** (run against `probe_run\`) reports exactly the expected three:
  - staged utoc changed (the intended re-stage);
  - staged ucas changed (the intended re-stage);
  - "feature head moved" (the 082-07e docs commit `570e6f4` is after the 082-07d boundary).

### 7.2 Readings against 082-07e's banked `M53S1_NULL_MASKED_A1`, paired by `session_index` (100 of 100)

- **(a) PASS: the masked tile now clips.**
  - On every frame, every cell with alpha 72 or 104 reads the sky (0 of the cells' pixels outside the sky box ± 2).
  - The target mask drops from **4,096 px to 2,048 px**, half the tile.
  - `probe-tile-si20-ref-over-probe.png` shows it: the reference above, the probe below.
- **(b) FAIL as written (the stop condition): max |d| 7 outside the tile.**
  - Every frame carries the same static halo, confined to the tile's neighbourhood:

    | distance from the tile | max | px > 2 over 100 frames |
    |---|---|---|
    | 1–4 px | 7 | 19,199 |
    | 4–8 | 2 | 0 |
    | 8–16 | 7 | 14,223 |
    | 16–32 | 7 | 17,197 |
    | 32–48 | 3 | 25 |
    | 48–64 | 2 | 0 |
    | ≥ 64 (far field) | **1** | 0 |

  - The > 2 pixels are:
    - the right 22 px of the lit `TC_NN1` tile (x 938–959), **all brighter** (312 of 312);
    - a sky ring within 3 px of the tile, **all darker** (197 of 197).
- **(c) FAIL as written: the drawn cells (alpha 152 and 184) differ by up to 5.** They are inside the same halo. Their
  readout is intact: same colours, clean edges.

### 7.3 Cause read, and why this is not the re-cook

- **Same-build precedent** (`samebuild-halo.json`, no cook involved): on 082-07e's own build, each wrong copy against its
  null changes pixels just outside its tile. Within 16 px:

  | pair | 1–4 px | 4–16 px |
  |---|---|---|
  | U-C1 chanswap | 8 | 3 |
  | U-D5 chanswap | 17 | 6 |
  | U-N1 normal | 11 | 5 |
  | N-N1 normal | 10 | 5 |

  Beyond 16 px it is ≤ 2 everywhere, and ≤ 1 at 48 px and beyond.
- **So any change to a tile's pixels leaks a few levels into its neighbourhood** on this bench. The mechanism is not
  established. Candidates, untested: bloom for the darker sky ring; screen-space AO on the lit neighbour for the brighter
  NN1 edge, which is plausibly wider here because the clip changes depth, not only colour.
- **The cook changed nothing else:**
  - Q-MASK: the only non-shader content change is `M_TC_Masked`;
  - the far field reads ≤ 1 on all 100 frames;
  - the halo is centred on the re-authored tile and identical in every frame.
- ⛔ **Nothing was waived.** The brief's stop fired and is reported. Staging stands, and the rollback is one archive away.

## 8. For AMENDMENT 2 (082-07g), beyond MASKED's PRE row

1. **The MASKED PRE row:** before the wrong copy is judged, the null leg shows the clip. The probe measured it: sky in
   every alpha < 0.5 cell, mask 2,048 px.
2. **G-BIND (3)'s "everything outside the target reads max d ≤ 2"** (`ev_bind_layer`: `d[~reg].max()` over the whole
   frame, TOL 2) **cannot pass on this bench.** The tile probe changes the layer half by up to 120, and the same-build
   halo puts 8–17 levels just outside any changed tile. Needed: a neighbourhood exclusion, for example a ring around the
   union mask with a far-field test, or the clause restated. **The row is predicted to FAIL as written.**
3. **The probe criteria (b) and (c)**, if re-used as a cross-build check, need the same neighbourhood rule.
4. **The build under test** is now `2FCDF059` + `20DA6F98` / `534C5863` / `FD766B7B`. ⚠ `082-07-common.py` hard-codes the
   staged hashes (`STAGED`), so the re-issue must change a locked harness file, as well as the boundary.
5. **G-BIND (3)'s offline proof** (§3) should be cited: it is a new manifest-independent entry.

## 9. State at the end

- **CaptureBench** (local-only): `2c5ec56`.
- **Plugin branch:** this journal, the status block and G323–G324. One `docs` commit,
  pushed. `m51` (`53bf725`), `master` and tags untouched; no tag.
- **Bench:** exe `2FCDF059` + `20DA6F98` / `534C5863` / `FD766B7B` + `462B8AC6` / `BB05CF99`. Rollback:
  `_binary_baselines\m53-s1-normalfix-cook-30FE0FDE\`.
- **Kept on E::** `_r53_cookout_f1…f3`, `_r53_extract\f1…f3`.
- ⛔ **The 082-07 boundary is stale:** head, staged build and `common.py`'s `STAGED`. 082-07g re-issues it.
