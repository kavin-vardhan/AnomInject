---
name: buildcookrun-cookoutputdir-needs-platform
description: "UE 5.1 BuildCookRun: a -CookOutputDir not ending in the platform folder (Windows) makes staging skip the package-store manifest, so RunIoStore throws NullReferenceException after UnrealPak"
metadata:
  node_type: memory
  type: reference
  originSessionId: <uuid>
  modified: 2026-10-13T21:55:56.475Z
---

Measured in 093-04 (2026-10-13), Codex's J1 bench cook. The harness passed `-CookOutputDir=<dir>\cooked`.
- The cook commandlet wrote straight into `<dir>\cooked\StackOBot\...`, manifest included.
- UAT's staging code appends the platform when the folder name is not the platform: `CopyBuildToStagingDirectory.Automation.cs:983-986`. It then loads the manifest only if `<dir>\cooked\Windows` exists (`:1219-1224`).
- Result: the cook exits 0 and UnrealPak exits 0, then `Project.RunIoStore` throws `NullReferenceException` at `:3518` (`SC.PackageStoreManifest.FullPath`). Only a partial `.pak` is left, with no `.utoc` or `.ucas`.

**How to apply:** when isolating a cook, give `-CookOutputDir` a path that ends in `\Windows`, or leave it out. It is a harness argument, not a product source change.
**Verified 093-05 (2026-10-14):** `-CookOutputDir=…\continuation09305\cooked\Windows` with `-stagingdirectory`/`-archivedirectory` → BUILD SUCCESSFUL, exit 0 in 42 s (iterative), full `.pak/.utoc/.ucas` + `global.*`.

The `dbghelp.dll` staging block ([[env-refuses-dbghelp-dll]]) is a separate risk that this failure never reached.
