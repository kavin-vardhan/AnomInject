---
name: ue425-build-on-this-pc
description: How UE 4.25 projects build on the home PC despite the per-user UBT pin to MSVC 14.38 (Europa bench); which overrides fail and which work
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-10-05T08:27:04.541Z
---

UE 4.25.4 builds on the home PC (2026-10-05, Europa bench `E:\Unreal Projects\StylizedParisStreet`, editor built in 56 s) **without touching** the per-user `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml`. That file pins CompilerVersion 14.38.33130 for the 5.1 projects and must never be edited or swapped.

The project's own `Target.cs` (both Game and Editor targets) needs:
- `WindowsPlatform.Compiler = WindowsCompiler.VisualStudio2017;`
- `WindowsPlatform.CompilerVersion = "14.16.27051";`. Use the cl.exe PRODUCT version: 4.25 UBT reads it (`UEBuildWindows.cs` `IsValidToolChainDir2017or2019`), not the folder name `14.16.27023`.
- `WindowsPlatform.WindowsSdkVersion = "10.0.17763.0";`
- `bOverrideBuildEnvironment = true;`. Without it, `UEBuildTarget.ValidateSharedEnvironment` builds the engine's UE4Editor/UE4Game base rules. Those never see our Target.cs, so they inherit the global 14.38 pin and fail: 4.25 sees that toolchain as 14.38.33144, not 33130.

**Why:**
- In 4.25, `TargetRules`'s base constructor applies the XML first (`TargetRules.cs:1474`), so the project's Target.cs wins for its own target. This corrects the Sept-2026 belief (G234) that the XML always beats Target.cs.
- Overrides that FAIL:
  - a Documents BuildConfiguration.xml: 5.1 reads it too (`XmlConfig.cs:289`), so it would break 5.1;
  - an engine-local `Engine\Saved` xml: loads before AppData, so it loses;
  - process-scoped `APPDATA`/`USERPROFILE` env: .NET Framework `GetFolderPath` ignores them (tested).

**How to apply:** use this recipe for any 4.25 code project or the AnomalyInjector Europa port. Editor launches that trigger UBT use the same Target.cs, so no wrapper is needed. Related: [[europa-port-2026-10]].
