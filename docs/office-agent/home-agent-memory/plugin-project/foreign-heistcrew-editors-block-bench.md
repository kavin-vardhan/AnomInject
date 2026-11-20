---
name: foreign-heistcrew-editors-block-bench
description: "Another workload launches UE 5.7 HeistCrewUE editors on this box mid-round; bench rounds must detect, wait (bounded), and restore/postflight around them."
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-10-25T20:37:59.076Z
---

A separate workload on this machine launches `E:\EpicGames\UE_5.7\...\UnrealEditor.exe` for
`E:\Unreal Projects\HeistCrewUE` without warning: 081-17R (2026-09-24 00:02 IST, one `-unattended` editor,
exited after ~39 min) and 081-18 (from 01:32 IST, four interactive editors on different maps, still running
past the 60-minute bound, round stopped with no leg run).

**Why:** bench rules say never kill a foreign process and wait at most 60 min; a foreign editor also
contaminates any throughput reading (≈1 core each). Name-based idle checks (`Get-Process UnrealEditor`) cannot
tell it from ours, so a restore/postflight that asserts "no UnrealEditor" will refuse to restore the boundary.

**How to apply:** check for it before staging anything; make launch checks refuse on any bench-class process
but make restore/postflight checks refuse only on processes whose path/command line touches our trees
(`D:\UESource`, `IntrusiveAnomalies`, `LyraStarterGame`, `StackOBot`) and record foreign ones. If it appears,
report it as the stop, not as a fixture failure. Related: [[verify-before-reexecuting-a-brief]].

**081-22 (2026-09-24):** the four editors ran 01:32 → ~07:10 IST (~1 core each), the PC was quiet only
07:13–07:18, and a new HeistCrew editor started at 07:18:28. Quiet windows can be minutes long: have every leg
scripted, syntax-checked and comparator-proven BEFORE the window opens, and poll with a foreground call
(≤ 9 min) rather than a background task. A child `powershell` can fail with "Thread failed to start" while
they run (commit charge ~72/85 GB) — make process checks retry.

**094-15 (2026-10-26): a second foreign UE 5.7 project, `E:\DynamicContents\unreal\DCDemo`** (UBT + a
`UnrealEditor-Cmd -run=pythonscript import_content.py`, ~01:44–02:00 IST). It writes `%LOCALAPPDATA%\UnrealBuildTool\Log.txt`/
`Log.json` (rotating `Log-backup-2023.04.13…`), `%LOCALAPPDATA%\UnrealEngine\Common\Zen`, `Common\UnrealTrace`, `5.7\Saved` and
`ProgramData\Epic\Zen\States`. Attribute those to it by the log header / path, not to our 4.25/5.1 builds (ours only rewrite
`XmlConfigCache-E+EpicGames+UE_4.25.bin`). The same night a foreign `E:\Pocket` folder took ~95 GB of E: in 12 minutes.

**081-21 (2026-09-24) refinement from chat:** builds may proceed while foreign editors run (another
engine cannot touch our binaries); only bench LEGS need a quiet PC (no UnrealEditor of any engine).
Classify processes by path/command line, and include `dotnet.exe` — UE 5.1's UnrealBuildTool runs as
`dotnet ... UnrealBuildTool.dll`, invisible to a name list of `UnrealBuildTool.exe`. The same four
HeistCrew editors ran from 01:32 through at least 04:26 IST that night.
