# REPORT for 082-06-m53-s1b-authoring-cook.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-06-m53-s1b-authoring-cook.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-27 05:08:25 | finished: 2026-09-27 06:05:17 | duration: 57 min | exit code: 0
context at last turn: 780622 tokens (78% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-06-m53-s1b-authoring-cook.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner.** The new corruption materials, the test level (101 assets, 77 objects) and its offline correctness checks are all built and verified. Nothing outside the allowed folders changed, before or after the cook. The cook itself ran cleanly, but its final gate failed for two reasons, so the new build was **not** put on the bench, which still runs `E0BE6F0A`:
1. The `MainWorld` map cooks to slightly different bytes every time, even from identical inputs, so the gate's "unchanged" check can never pass as written.
2. A bug in my level tool meant 4 of the test meshes never received their assigned materials. The fix is known but not applied, because a stop condition had already fired.

**3. Assets created**
- **Plugin, commit `c9f8089`, script `tools/create_texcorrupt_assets.py`:**
  - `/AnomalyInjector/Materials/M_CorruptTex_UV`: Material, Surface, Unlit, AlphaComposite, two-sided.
    - Textures: `SrcColor` (Color sampler), `SrcData` (LinearColor sampler), `SrcNormal` (Normal sampler).
    - Scalars at their defaults: `SrcKind` 0, `SrcMip` 0, `UvScale` 1, `UvOffsetU` 0, `UvOffsetV` 0, `UvSwap` 0, `ScrambleOn` 0, `ScrambleK` 8, `ScrambleAInv` 1, `ScrambleB` 0; `DbgChanSwap`, `DbgSrgbTwice`, `DbgTexelShift`, `TexelSizeU`, `TexelSizeV`, `DbgSkipNormalEncode`, `DbgOpaque` all 0.
  - `/AnomalyInjector/Materials/M_CorruptTex_Normal`: Material, Surface, Unlit, Opaque.
    - Textures: `SrcNormal`, `NoiseNormal`.
    - Scalars: `SrcMip`, `NoiseMip`, `NormalSignX`/`NormalSignY` (1), `FlatMix`, `NoiseAmp`, `DbgChanSwap`, `DbgTexelShift`, `TexelSizeU`, `TexelSizeV`, `DbgSkipNormalEncode`.
  - `/AnomalyInjector/Textures/T_CorruptTex_NoiseN`: Texture2D, 256², tileable, normal-map compression, never streamed.
  - Every source sample reads an explicit mip with view mip bias off and the texture's own sampler.
  - Checks passed: both materials compile (143 and 132 pixel-shader instructions); the parameter lists match the C++ contract exactly; the subsystem's hard references resolve all three assets.
- **Fixture in `/Game/CaptureBenchTexCorrupt/`** (CaptureBench `8e53dea`):
  - 49 textures, 46 materials, 1 material-layer function, 4 duplicated meshes, and the level `CB_TexCorruptLevel` with 77 actors.
  - It covers the 13 encoding rows, the alpha, shared-sampler and redraw rows, 6 mip-chain textures × 4 viewing geometries, the reference-bug copies, the layer, alias and dead-binding targets, the asset-slot-MID target and all the refusal-reason producers.
  - Actor names and roles are in `texcorrupt_fixture_actors.json`.

**4. Offline wrong-copy proofs** (30 fault/row pairs, all ≥ 32)

| fault | rows | minimum level |
|---|---|---|
| chanswap | U-C1–4, U-D1–4, shared sampler / U-D5, U-D6 / U-N1, N-N1 | 120 / 56 / 69, 74 |
| srgbtwice | U-C1–4 / U-C5 | 73 / **37** (lowest) |
| alpha | U-C2, U-C3, U-D2, U-D3 | 72 |
| normal | U-N1, N-N1 | 89 |
| texelshift (along U only, Codex A) | chain (b) / chain (d), mip 0 | **128 / 89, on every texel** |
| mipshift / mipgen | chain (a) | 80 / 80 |
| noclear | redraw opaque / redraw alpha | 112 / 51 |

**5. M0 / M1 / M2**
- A re-check of M0 before authoring was clean.
- **M1 and M2 are both CLEAN, and identical to each other.** All 2,082 M0 files are byte-identical, including `CB_GateLevel.umap`, `MainWorld.umap` and all 421 of `MainWorld`'s external actors. Nothing was removed.
- The 104 added files are all allowed: 101 under `CaptureBenchTexCorrupt\` plus the 3 plugin assets.
- I confirmed the byte check can fail before relying on it.

**6. Archive, cook, stage**
- **Archive:** `_binary_baselines\m53-s1-precook-container-67EA1FE0\` holds exe `E0BE6F0A` and the container `67EA1FE0` / `2CEFB8F4` / `E03C6610` + `C70ECDAA` / `A16A18A8`. All 6 files were hash-checked at the destination. The IoStore listing of the archive has 1,972 chunks.
- **Cook:**
  - It ran on the E: scratch host with the real project content linked in. The editor build was already up to date at the S1 head.
  - Result: BUILD SUCCESSFUL, 2 min 05 s, 894 packages, 0 warnings, 0 errors.
  - It packaged exe `2FCDF059` (the 082-06c build) with container `651656EF` / `F01CAFC0` / `EC4EB9A7` + `462B8AC6` / `BB05CF99`.
- **A44 scan** of that exe (in the output folder, since it was not staged): all new m53 strings present, alongside the older control strings.
- **Map gate:** exit 0, with `CB_TexCorruptLevel` named in the required list.
- **G-COOK: FAIL.**
  - Passed: M1/M2 clean, archive verified, map gate exit 0, `CB_GateLevel` chunk identical.
  - ❌ `MainWorld`'s chunk hash changed (`0B2F9571` → `A9D83F53`, same size). A second cook from identical inputs changed it again (`2B46C64E`), so the check as written cannot pass. The difference from the old container lies entirely inside the region that already varies between two identical cooks (696 of 697 differing 4-byte words, none outside). The cause is not established.
  - ❌ 5 fixture packages are missing from the container. My tool edited copies of the meshes' material-slot entries, so the changes never saved; the three static duplicates kept `WorldGridMaterial` and the skeletal one kept `None`. As a result the `slot_empty`, asset-slot-MID, `nanite_override` and `default_material_path` producers are wrong in this cook.
- **Staging: none.** The bench is unchanged.

**7. Deviations**
- `SrcData` uses the LinearColor sampler instead of Masks. No `TC_Masks` texture exists in engine content to serve as its default, and the two sampler types produce the same shader code.
- The readout materials use the sampler type the engine requires for each texture, so U-C5 reads through Grayscale and U-D6 through LinearGrayscale rather than the plan table's wording.
- The grazing geometry is a ceiling strip rather than an angled wall plane, which kept every object on screen.
- The cook ran on the scratch host with the content linked in, because the D: project builds the `m51` plugin.
- I ran one extra cook from identical inputs as a diagnostic control. It cannot change the gate result.
- The importer turned on virtual texturing for the two 4096² textures by itself. My verify pass caught it and the tool now sets the flag explicitly.
- I found and fixed two defects in my own tool before the cook: a re-run that deleted assets the subsystem holds references to, and an initial layout that pushed objects off screen or behind each other.

**8. NEEDS-DECISION**
1. **The `MainWorld` gate row is unsatisfiable as written.** I propose, without applying it, the control-pair form already used for G-S3a-1: identical-input cooks define the region allowed to vary, and the new cook's difference must fall inside it, with the cooked `.umap` header byte-identical. `CB_GateLevel` would keep exact identity. Today's reading under that rule passes: 696 of 697 differing words inside, header identical.
2. **Authorise the fixture fix and a re-cook.** The fix is to set mesh slots with `SetMaterial`, build new material entries for the skeletal mesh, and have the verify pass read the slots back. Also rule on whether cooks stay on the scratch host: its `Content` now links to the real project, so anything an editor saves there lands in the D: project.

Journal: `docs/sessions/2026-09-27-082-06-code-m53-s1-authoring-cook.md`. The branch is at `e070c6c` and pushed; `m51`, `master` and tags are untouched. Evidence is in `D:\IntrusiveAnomalies\_reviews\082-06-evidence\`.

Several MCP connectors (Gmail, Calendar and the plugin connectors) need authorisation in claude.ai or `/mcp`; none was needed for this brief.
api-equivalent cost (not billed on Max): $26.88 | turns: 133 | running total across all briefs: $2,626.85
