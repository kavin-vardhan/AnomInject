# Session 007 — Refactor: strip the "GDP" prefix from the plugin (2026-06-18)

## Goal
Remove the early-oversight **"GDP" prefix** from the plugin's *code artifacts* — symbols,
file/folder names, module name, the `*_API` macro, the log category, and the console-command
surface — with **no behavior change** (pure mechanical refactor; no new anomaly, no feature work).
The research-project **identity "GDP: Anomaly Injection" is retained** — this strips the code
prefix only, not the project's name (owner ruling: Decision D, code-prefix-only).

## Rename mapping (applied)
| Current | New |
|---|---|
| module / plugin / folder / `Build.cs` class / `.uplugin` module Name `GDPAnomalyInjector` | `AnomalyInjector` (kept "Injector") |
| `UGDPAnomalyInjectorSubsystem` | `UAnomalyInjectorSubsystem` |
| `IGDPAnomaly` | `IAnomaly` |
| `FGDPAnomaly_*` (7 structs) | `FAnomaly_*` |
| `FGDPAnomalyInjectorModule` | `FAnomalyInjectorModule` (Decision E — `IMPLEMENT_MODULE` token must match module name) |
| API macro `GDPANOMALYINJECTOR_API` | `ANOMALYINJECTOR_API` (auto-derived from module name by UHT) |
| log category `LogGDPAnomaly` | `LogAnomaly` (1 DECLARE + 1 DEFINE + all `UE_LOG` sites) |
| helper namespaces `GDPTargeting` / `GDPArgs` / `GDPLod` | `AnomalyTargeting` / `AnomalyArgs` / `AnomalyLod` (NOT bare — collision risk) |
| console commands `GDP.*` | `IAI.*` (`Apply` / `Revert` / `RevertAll` / `ListAnomalies` / `ListActors`) |
| `*.generated.h` include (subsystem — the only `UCLASS`) | `AnomalyInjectorSubsystem.generated.h` |
| on-screen heartbeat prefix `[GDP]` | `[IAI]` (Decision F1) |
| module log strings `"GDPAnomalyInjector module …"` | `"AnomalyInjector module …"` (Decision F2) |
| `.uplugin` `FriendlyName` `"GDP Anomaly Injector"` / `Category` `"GDP"` | `"Anomaly Injector"` / `"Anomaly Injection"` (Decisions B/C) |
| internal command vars `GGDP*Cmd` / `GGDPHeartbeatKey` | `G*Cmd` / `GAnomalyHeartbeatKey` (Decision F3) |
| deferred-helper comment `GDPCvar` | `AnomalyCvar` (Decision F4) |

**KEPT (project identity, not code):** copyright header `// Copyright GDP Anomaly Injection Project`
(D1), `.uplugin` `CreatedBy` `"GDP Anomaly Injection Project"` (D2), `"GDP: Anomaly Injection"` /
`"GDP M<x>"` prose in living docs, the **bridge's 5 historical comments**, and **all session
journals** (dated record). The heartbeat magic number `0x47445048` ("GDPH") is unchanged — it is
just a unique on-screen-message id (F3); the identifier around it was renamed.

## What was done
1. **`git mv`** of all 27 plugin files into the renamed `Source/AnomalyInjector/` module folder
   (25 source `.h`/`.cpp` + `AnomalyInjector.Build.cs` + `AnomalyInjector.uplugin`); then the
   enclosing repo folder `Plugins/GDPAnomalyInjector/` → `Plugins/AnomalyInjector/` (a plain
   filesystem move — the repo's own `.git/` travels with it; history intact, verified `master` @ `m3`).
2. **Content edits** via an ordered, case-sensitive token pass (longest tokens first). The KEEP
   project-identity strings use `GDP ` (space) or `GDP:` (colon), which none of the rules match,
   so they were preserved automatically. Verified: only intended KEEPs remain in source.
3. **Living docs** (`CLAUDE.md`, `architecture.md`, `onboarding.md`, `setup-runbook.md`, and the
   symbol references in append-only `gotchas.md`) updated to the new names — lessons left intact,
   only tokens swapped in place. A few prose references to code artifacts (`GDP.*` → `IAI.*`,
   "GDP subsystem" → "AnomalyInjector subsystem", the plugin-browser category line, "GDP plugin
   repo") were hand-fixed. **Session journals left entirely unchanged.**
4. **Clean** of stale `Binaries/` + `Intermediate/` (project-root and plugin) — the module rename
   renames the DLL `UnrealEditor-GDPAnomalyInjector.dll` → `UnrealEditor-AnomalyInjector.dll`, so
   the old binary had to go (gotcha G21). `Saved/` untouched.
5. **Rebuild** `StackOBotEditor / Development / Win64` on source UE 5.1 — **clean compile, exit 0**
   (`Compile Module.AnomalyInjector.cpp` → `Link UnrealEditor-AnomalyInjector.dll`; host + bridge
   relinked). Produced `Plugins/AnomalyInjector/Binaries/Win64/UnrealEditor-AnomalyInjector.dll`;
   no GDP-named DLL anywhere.
6. **Light re-gate over the `unreal-mcpython` bridge** (MainWorld Simulate session) — all green:
   - Editor log: `LogPluginManager: Mounting Project plugin AnomalyInjector` + `LogAnomaly: AnomalyInjector module started.` → **renamed module loads, `LogAnomaly` category live.**
   - Bridge connected (127.0.0.1:12029).
   - `IAI.ListAnomalies` → **7** anomalies, **sorted** (`camera_clipping, flicker, lighting_mismatch, lod_corruption, lod_popping, missing_object, time_dilation`) under `LogAnomaly`.
   - `IAI.Apply missing_object SM_Ramp` → matched **2** actors, both `hidden == true`; log `IAI.Apply 'missing_object' -> applied`.
   - `IAI.Revert missing_object` → both `hidden == false`; log `IAI.Revert 'missing_object' -> reverted`.
   - Command surface, registration strings, help text, log category, and module load all survived. (Full 8-gate suite not needed — naming-only change.)

## Decisions (owner-ruled)
- **D — code-prefix-only.** Strip GDP from code artifacts; keep the project identity "GDP: Anomaly Injection".
- **B/C/E/F1/F2/F3/F4** — approved as recommended (see mapping table).
- Helper namespaces keep the `Anomaly` prefix (not bare `Targeting`/`Args`/`Lod`); module/subsystem keep "Injector".
- **Bridge** (`Plugins/unreal-mcp/`, unversioned per G8) needed **no functional edit** — its only
  `GDP` tokens are 5 historical milestone-tag comments, left as an accurate dated record.
- **Host** (`Source/`, `StackOBot.uproject`) needed **no edit** — it never referenced the plugin/module.

## Problem → Resolution
- **`git` rename detection.** 26/27 files tracked as renames at git's default 50% similarity. The
  one outlier, `AnomalyInjectorModule.cpp` (28 lines, GDP tokens are the content-heavy lines →
  38% similarity), is detected as a rename at `-M30%`; trace its history with
  `git log --follow -M30% -- <path>`. Its entire prior history is a single commit (the M0 skeleton),
  so the impact is negligible. All renames were done via `git mv` so history follows.
- **CRLF warnings** on `git add` are pre-existing repo/`autocrlf` noise (working files are LF); the
  token pass preserved line endings (a renamed file's diff is only the changed token lines, not a
  full-file rewrite), confirmed via `git diff --staged -M --stat` on both paths.

## State
- **Clean Development-Editor compile on 5.1 (exit 0)** + light bridge re-gate green (module load,
  command surface, registration, log category all survived). Editor left running post-re-gate.
- Prior milestone **M3** remains committed (`c54351a`) and tagged **`m3`** (unchanged).
- VersionName unchanged at `0.4.0` (no behavior/version change — pure refactor).

## Hand-off
- **Commit pending owner acceptance of the re-gate.** One `refactor:` commit in the plugin repo:
  `refactor: remove GDP prefix from plugin (module/classes/commands/log)`. **No milestone tag**
  (this isn't a milestone). Bridge/host changes stay unversioned (G8 forward-decision unchanged).
- Next: the next breadth round per the chat-Claude brief.
