# 084-08 part A cook — declared before the cook runs (2026-09-29)

- **Branch / head:** `fix/m52-label-timing` at `4bf0915` (084-08 part A source, pushed). Host `E:\IA_BuildCache\_r84_host\StackOBot`,
  plugin = the branch worktree. Editor + Game binaries already rebuilt at `4bf0915` (editor 13 actions, game 6, exit 0) — G47.
- **Content:** as 084-07b/07c — two temporary junctions, `Content` → `D:\IntrusiveAnomalies\StackOBot\Content` and
  `Plugins\RoomGenerator\Content` → `D:\IntrusiveAnomalies\StackOBot\Plugins\RoomGenerator\Content`, removed after the cook with
  `[IO.Directory]::Delete(junction, $false)`. Guard: `content-manifest-before.txt` / `-after.txt` (path|size|mtime|SHA-256).
- **Map set (declared in writing, G119):** `/Game/CaptureBenchGate/CB_GateLevel`, `/Game/StackOBot/UI/MainMenu/MainMenu`,
  `/Game/StackOBot/Maps/MainWorld`; `Entry` expected as a dependency. `CB_TexCorruptLevel` deliberately NOT cooked.
- **Command:** 084-07c's with the archive directory `E:\IA_BuildCache\_r84_cookout_d1`. No `-run`.
- **Predictions:**
  1. The Game target is up to date, so the staged exe is **`8A6074AA`** (242,127,360 B), byte-identical to the build and the archive.
  2. The cooked `AnomalyInjector.uplugin` is byte-identical to the branch's (the descriptor was not touched since 084-07c:
     SHA-256 `9EFB9B49…`), five modules, `AnomalyBench` first with `TargetAllowList [Editor, Game]` and
     `TargetConfigurationDenyList [Shipping]`.
  3. `verify_cooked_maps.ps1` finds `CB_GateLevel`, `MainMenu`, `MainWorld` and `Entry`; `CB_TexCorruptLevel` MISSING.
  4. The D: content manifest is byte-identical before and after (and equal to 084-07c's `E4C7F38D…` if content is unchanged).
  5. The global shader containers equal 084-07c's (`462B8AC6` / `BB05CF99`): no shader changed.
- **Not done:** staging (the m53 set stays staged — see `staged-set-before.txt`), any launch, any leg.