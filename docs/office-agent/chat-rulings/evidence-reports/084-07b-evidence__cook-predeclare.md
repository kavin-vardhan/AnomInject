# 084-07b cook — declared before the cook runs (2026-09-29)

- **Branch / head:** `fix/m52-label-timing` at `a6a4893` (source `8b8b4d0`; `a6a4893` is docs only). Host
  `E:\IA_BuildCache\_r84_host\StackOBot`, plugin = the branch worktree.
- **Content:** the host has none of its own. For this cook it gets two junctions, the same arrangement the m53 host uses:
  `Content` → `D:\IntrusiveAnomalies\StackOBot\Content` and `Plugins\RoomGenerator\Content` →
  `D:\IntrusiveAnomalies\StackOBot\Plugins\RoomGenerator\Content`. Hazard: a commandlet that saves `/Game/` would write the
  D: project. Guard: `content-manifest-before.txt` (2,182 files, path|size|mtime|SHA-256) is re-taken after the cook; any
  difference is reported.
- **Map set (declared in writing, G119):** `/Game/CaptureBenchGate/CB_GateLevel`, `/Game/StackOBot/UI/MainMenu/MainMenu`,
  `/Game/StackOBot/Maps/MainWorld`. `Entry` is expected to arrive as a dependency, as in every earlier 4-map cook.
  **`CB_TexCorruptLevel` (the m53 fixture) is deliberately NOT cooked:** this branch carries no m53 code.
- **Command:** `RunUAT BuildCookRun -project=<host uproject> -platform=Win64 -clientconfig=Development -cook -stage -pak
  -archive -archivedirectory=E:\IA_BuildCache\_r84_cookout_a1 -build -utf8output -nocompileeditor -map=<the three>` —
  the 082-06d / 082-07c command minus the m53 level. No `-run`: nothing is launched.
- **Editor binaries first (G47):** `Build.bat StackOBotEditor` must report the target up to date (or rebuild) before the
  cook, since the cook runs on editor binaries.
- **Predictions:**
  1. The Game target is up to date, so the staged exe is **`FF41BFF3`** (the 084-07 archive), byte-identical.
  2. The cooked `AnomalyInjector.uplugin` (UnrealPak extract of the `.pak`) lists **five** modules, the first being
     `AnomalyBench` with `TargetAllowList [Editor, Game]` and `TargetConfigurationDenyList [Shipping]`; it is byte-identical
     to the branch's `AnomalyInjector.uplugin`.
  3. `verify_cooked_maps.ps1` finds `CB_GateLevel`, `MainMenu`, `MainWorld` and `Entry`, and not `CB_TexCorruptLevel`.
  4. The D: Content manifest is unchanged after the cook.
- **Not done:** staging (the m53 set stays staged), any launch, any leg.
