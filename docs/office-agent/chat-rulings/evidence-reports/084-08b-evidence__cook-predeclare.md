# 084-08b cook — declared before the cook runs (2026-09-29)

- **Branch / head:** `fix/m52-label-timing` at `17b9d5d` (source `4bf0915`, unchanged; no source edits in 084-08b). Host
  `E:\IA_BuildCache\_r84_host\StackOBot`, plugin = the branch worktree; editor and game binaries as built by 084-08 (G47 holds:
  nothing to rebuild).
- **Content:** the D: project now holds the 7 new fixture files under `Content\AnomalyFixtures\` (manifest
  `content-manifest-2-after-copy.txt`, 2,189 files, `7A5D8D8D…`; the diff against `E4C7F38D…` is exactly those 7 files). Two
  temporary junctions as in 084-07b/084-08 (`Content` → the D: project Content, `Plugins\RoomGenerator\Content` → its D:
  content), removed after the cook with `[IO.Directory]::Delete(junction, $false)`. The host's own authored copy is moved to
  `E:\IA_BuildCache\_r84_selftest\lodfix_authored_content_0848b` first.
- **Map set (declared in writing, G119):** `/Game/CaptureBenchGate/CB_GateLevel`, `/Game/StackOBot/UI/MainMenu/MainMenu`,
  `/Game/StackOBot/Maps/MainWorld`, **`/Game/AnomalyFixtures/CB_LodFixture`**; `Entry` expected as a dependency.
  `CB_TexCorruptLevel` deliberately NOT cooked.
- **Command:** 084-08's with the fixture map appended and the archive directory `E:\IA_BuildCache\_r84_cookout_e1`. No `-run`.
- **Predictions:**
  1. The Game target is up to date: the staged exe is **`8A6074AA`** (242,127,360 B), byte-identical to the build and the
     084-08 archive.
  2. The cooked `AnomalyInjector.uplugin` is byte-identical to the branch's (`9EFB9B49…`): five modules, `AnomalyBench`
     first, `TargetAllowList [Editor, Game]`, `TargetConfigurationDenyList [Shipping]`.
  3. `verify_cooked_maps.ps1` finds `CB_GateLevel`, `MainMenu`, `MainWorld`, `Entry` **and `CB_LodFixture`**;
     `CB_TexCorruptLevel` MISSING.
  4. The D: content manifest is byte-identical before and after the cook (`7A5D8D8D…`).
  5. The global shader containers equal 084-08's (`462B8AC6` / `BB05CF99`): no global shader changed.
  6. **Container diff against the 084-08 cook `4BF63FB8` (UnrealPak `-List`, per-chunk hash):** every named chunk of the old
     container is present with the same hash except the project shader archives / shader metadata; the added named chunks are
     only `StackOBot/Content/AnomalyFixtures/*` (the 7 packages and any bulk data of the two meshes); no named chunk removed;
     unnamed `<ShaderCode>` chunks only added (the four new materials), none removed.
  7. The pak (`26FFC026` at 084-08) changes only in the asset-registry / shader-info entries (it holds no fixture package).
- **Not done:** staging (the m53 set stays staged — receipts before/after), any launch, any leg.
