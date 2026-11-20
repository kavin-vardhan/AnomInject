# 084-07c cook — declared before the cook runs (2026-09-29)

- **Branch / head:** `fix/m52-label-timing` at `3d6f702` (084-07c source; the journal is committed after the cook as docs
  only). Host `E:\IA_BuildCache\_r84_host\StackOBot`, plugin = the branch worktree.
- **Content:** as 084-07b — two temporary junctions, `Content` → `D:\IntrusiveAnomalies\StackOBot\Content` and
  `Plugins\RoomGenerator\Content` → `D:\IntrusiveAnomalies\StackOBot\Plugins\RoomGenerator\Content`, removed after the
  cook. Guard: `content-manifest-before.txt` (2,182 files, path|size|mtime|SHA-256, manifest SHA-256 `E4C7F38D…`, identical
  to 084-07b's before and after) is re-taken after the cook; any difference is reported.
- **Map set (declared in writing, G119):** `/Game/CaptureBenchGate/CB_GateLevel`, `/Game/StackOBot/UI/MainMenu/MainMenu`,
  `/Game/StackOBot/Maps/MainWorld`; `Entry` expected as a dependency. `CB_TexCorruptLevel` deliberately NOT cooked.
- **Command:** 084-07b's `cook-a1-command.txt` with the archive directory `E:\IA_BuildCache\_r84_cookout_c1`. No `-run`.
- **Editor binaries first (G47):** rebuilt at `3d6f702` (4 actions, exit 0) before the cook.
- **Predictions:**
  1. The Game target is up to date, so the staged exe is **`16053B80`** (242,115,072 B), byte-identical to the build.
  2. The cooked `AnomalyInjector.uplugin` is byte-identical to the branch's (SHA-256 `9EFB9B49…`), five modules,
     `AnomalyBench` first with `TargetAllowList [Editor, Game]` and `TargetConfigurationDenyList [Shipping]`.
  3. `verify_cooked_maps.ps1` finds `CB_GateLevel`, `MainMenu`, `MainWorld` and `Entry`; `CB_TexCorruptLevel` MISSING.
  4. The D: content manifest is unchanged after the cook.
  5. The global shader containers equal 084-07b's (`462B8AC6` / `BB05CF99`): no shader changed.
- **Not done:** staging (the m53 set stays staged: exe `2FCDF059` + `20DA6F98` / `534C5863` / `FD766B7B`), any launch, any leg.
