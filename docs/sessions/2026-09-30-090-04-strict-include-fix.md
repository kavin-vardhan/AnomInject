# 090-04 — Strict-include fix for the second office host: the plugin's own includes completed and a strict-include build that proves it; then the fix branch merged into m53 again (2026-09-30)

**Brief:** 090-04 (Code, Opus 5.5, headless), released by `_reviews\090-02-chat-ruling-codex-evidence.md` item 1 ("the
second office host is blocked"). Includes only; no game launch, no cook, no staging.
**Branches:** `fix/m52-label-timing` (warm host `_r84_host`, branch worktree) from `0347e8c` → **`01d34d3`**; then
`feat/m53-uv-normal-corruption` (worktree `_r53_src`, warm host `_r53_host`) from `da902d6` → merge **`a74bf7f`** — the m53 half of
this journal is §7 onwards (it exists only on m53).
⛔ No game launch, no cook, no UE staging, nothing took focus. `m51`, `master`, tags, `ToCodex\` and `E:\AmmaYT` untouched. No tag,
no merge to `master`. Security settings untouched. A foreign UE 5.7 editor was resident throughout (not ours, not touched); it held
5.4 GB, so UBT ran one compile at a time (1.5 GB-per-action cap, 4.3 GB free).

## 0. The report, and what it means

The second office host (plugin folder replaced wholesale with `da902d6`) failed with 8 errors and `MSB3073 … exited with code 6`:
C2061 `'ELevelTick'` at `AnomalyInjectorSubsystem.h:174`, the C2665 / C2511 / C2352 / C2597 cascade in `AnomalyInjectorSubsystem.cpp`
(263, 528, 530, 534), and C2027 `'UMaterial'` at `AnomalyCaptureSubsystem.cpp:10368`. This box and the first office host compile the
same commit clean.

**Why only there, from source:** our Game and Editor builds compile each plugin module as unity files with `/Yu` on
`SharedPCH.Engine.ShadowErrors.h` (read from the response files), and that PCH is built from `EngineSharedPCH.h` — 627 lines of engine
includes, among them `Engine/EngineBaseTypes.h` (line 463, declares `ELevelTick`) and `Materials/Material.h` (line 572). Stock 5.1
`Subsystems/WorldSubsystem.h` → `Engine/EngineTypes.h` / `Tickable.h` does not reach `EngineBaseTypes.h`. So a plugin file compiled
on its own, without that PCH, fails exactly as reported. ⚠ **Which build setting on the second office host causes that is not
observed** (adaptive unity on freshly pasted files, or no shared PCH, are candidates); the fix does not depend on it.

## 1. The instrument — `tools/strict_include_build.py`

- Per plugin module only (a temporary block appended to the end of each `Build.cs` constructor): `bUseUnity = false;`
  `PCHUsage = PCHUsageMode.NoPCHs;` `IncludeOrderVersion = EngineIncludeOrderVersion.Latest;` `bEnforceIWYU = true;`.
- **Include order:** 5.1 offers `Unreal5_0` and `Unreal5_1` only (`Latest = Unreal5_1`, `TargetRules.cs`), and both host targets
  already set `Unreal5_1`, so this axis was already at its strictest; the tool pins it per module anyway (a host on `Unreal5_0`).
- **Header self-containment:** one generated TU per plugin header (`#include "<header>"` only) in
  `Private/StrictIncludeCheckGenerated/` — 55 on the fix branch, 61 on m53. With unity off each `.gen.cpp` also compiles on its own.
- **Lever check (G114's lesson):** after each target, every plugin `.cpp` (including the generated TUs) must have its own
  `<file>.cpp.obj.response` in that target's intermediate folder with no `/Yu` / `/Yc`. ⚠ The first version checked response-file
  **mtimes**; UBT does not rewrite an unchanged response file, so run 2 read "nothing compiled" (a false lever failure). Fixed to
  read content (G424).
- **Restore:** the original `Build.cs` bytes are written back and SHA-256-checked, the generated folders removed, `git status`
  compared before/after; then (unless `--no-normal-rebuild`) the normal Game and Editor builds run and their warnings are counted.
- **Why not `-NoPCH` / `-DisableUnity` / `-ForceIncludeOrder`:** those are target-wide; they change every engine module's compile
  environment (a full engine rebuild for the Game target; for the Editor target, the shared engine DLLs).
- `Build.cs` is only ever touched temporarily; `AnomalyCapture.Build.cs`'s fork-probe text is never altered (restore is
  byte-verified on every run).

## 2. The fix (`01d34d3`, includes only)

| file | added include | why |
| --- | --- | --- |
| `AnomalyInjector/Public/AnomalyInjectorSubsystem.h` | `Engine/EngineBaseTypes.h` | `ELevelTick` in `OnWorldPreActorTickSynth` (the office root error) |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `Materials/Material.h` | `MI->GetMaterial()->MaterialDomain` in the scene-texture probe (the office C2027) |
| `AnomalyInjector/Public/IAnomaly.h` | `UObject/WeakObjectPtr.h` | `TWeakObjectPtr<UTexture2D>` needs `FWeakObjectPtr`; it included only `WeakObjectPtrTemplates.h` (C2504, 23 TUs) |
| `AnomalyControlServer/Public/AnomalyControlServerSubsystem.h` | `IWebSocketServer.h` | `TUniquePtr<IWebSocketServer>` member (C4150 in the header's own TU and its `.gen.cpp`); the header's only includer is its own module |

**Beyond the two known:** the strict build found **two more missing includes** (the last two rows) and **one non-include residual** (§3).

## 3. The residual — NOT include-fixable, held for a ruling

With the four includes, both targets read exactly **3 × C4150** (deletion of pointer to incomplete type `FAnomalyCaptureAsyncState` /
`FAnomalyPreviewTee` / `FAnomalyRunLog`) in **`AnomalyCaptureSubsystem.gen.cpp`**. Line 74 of that UHT file is
`DEFINE_VTABLE_PTR_HELPER_CTOR(UAnomalyCaptureSubsystem)` = `UX::UX(FVTableHelper& Helper) : Super(Helper) {}`; a constructor may
destroy its members, which instantiates `~TUniquePtr<T>` there. `FAnomalyCaptureAsyncState` is defined inside
`AnomalyCaptureSubsystem.cpp` (line 194) and the other two in the module's Private headers, which the Public header cannot include
for other modules — so **no include can fix it**. In a unity build the `.gen.cpp` shares a file with `AnomalyCaptureSubsystem.cpp`, so
it never shows; it shows only when that UHT file compiles alone. ⚠ The office error list does not contain it (nor the C2504 / the
`IWebSocketServer` C4150), which suggests that host does not compile `.gen.cpp` files alone — an inference, not an observation.

**Experiment (run 3, NOT committed, reverted byte-for-byte):** declare `UAnomalyCaptureSubsystem(FVTableHelper& Helper);` beside
the constructor and define `UAnomalyCaptureSubsystem::UAnomalyCaptureSubsystem(FVTableHelper& Helper) : Super(Helper) {}` in the
`.cpp` (3 lines; the same body UHT generates). UHT then omits its own (`UhtHeaderCodeGeneratorCppFile.cs:1034`,
`HasCustomVTableHelperConstructor`) and **the strict build reads 0 errors on Game and Editor** (lever check 55 / 55 header TUs, no
problems). Diff: `_binary_baselines\m52fix-09004-76428F44\experiment_vtablector_plus_includes.diff`. After the revert the two files
were touched so UHT regenerated the stock `.gen.cpp` (checked: `DEFINE_VTABLE_PTR_HELPER_CTOR` back in both targets' UHT output).
→ **NEEDS-DECISION**, G425.

## 4. Strict runs (fix branch, `_r84_host`)

| run | source | Game | Editor |
| --- | --- | --- | --- |
| 1 control | `0347e8c` (unfixed) | exit 6, 116 actions, 831 s, **12 unique errors** | exit 6, 125 actions, 580 s, **the same 12** |
| 2 | + the four includes | exit 6, 77 actions, 379 s, **3** (the §3 residual) | exit 6, 85 actions, 424 s, **3** |
| 3 experiment | + the §3 constructor | **exit 0, 0 errors**, 64 actions, 299 s | **exit 0, 0 errors**, 73 actions, 256 s |

Run 1 is the can-fail proof: it reproduces **every** office error code at the **same** sites (fix-branch line numbers: header 122 ×6
TUs, `.cpp` 199 / 451 / 453 / 457, `AnomalyCaptureSubsystem.cpp` 9993), plus C2504 `FWeakObjectPtr` ×23 and C4150 ×5. Every run
restored all five `Build.cs` byte-identical with `git status` unchanged. Run 3's lever check: per-file no-PCH responses
AnomalyBench 1 · AnomalyCapture 39 · AnomalyControlServer 8 · AnomalyInjector 53 · AnomalyShaders 5; header TUs 55 / 55.

## 5. Normal build and checks (fix branch, source `01d34d3`)

- **Editor 15 actions / 106 s, Game 6 actions / 177 s, both exit 0, 0 warnings** (every warning line counted). Exe `4B804F41` →
  **`76428F44`**; DLLs AnomalyBench `BAF3695E`→`D6C36212`, AnomalyCapture `768F6074`→`3744BE63`, AnomalyControlServer
  `34DF1FE7`→`105E9349`, AnomalyInjector `AD33BDE7`→`9EBFF30A`, AnomalyShaders `23601F71`→`82B823B1` (relink only).
- **String scan:** every ASCII and UTF-16 string (6+ chars), before vs after. Instrument proven first: 0 on a self-pair and on the
  `EB01156E` / `4B804F41` relink pair; non-zero on fix vs m53. Result: **UTF-16 literal sets identical on all five DLLs**; the exe
  differs by one adjacency artefact (`pUAnomalyControlServerSubsystem` vs `UAnomalyControlServerSubsystem`, the same literal after a
  different byte); ASCII differences are instruction bytes (`tNHc=…`, shifted RIP-relative displacements) and the DLLs' `RSDS` PDB
  record. No literal added or removed.
- **Lever audit `--binary`** exe and the five DLLs vs the branch: **PASS 33 / 33** each. `m52_window_selftest` **284 / 0**, camera
  **154 / 0**. Python suites (the 090-01 runner): verify_capture bare / label-rule 24 / pixel-gate 98 / change 35, both unittests,
  lever 11 + PASS, G354 9, bundle 9, kit 32 — all OK (the m53-only tool entries exit 2: those tools do not exist on this branch).
- **Archive:** `_binary_baselines\m52fix-09004-76428F44\` (exe + the five DLLs, 6 / 6 re-hashed; logs, string scan, lever, suites).

## 6. Docs

`docs/PRE-DELIVERY-CHECKLIST.md` §1 gains the strict-include box (after "the editor target builds"), with the §3 residual named so
the box is not ticked on a misread. Gotchas **G424** (the shared PCH hides missing includes; the instrument and its two traps) and
**G425** (the `.gen.cpp` `FVTableHelper` constructor and `TUniquePtr<Forward>` members). Status block updated.

**Not done / limits:** no Shipping-configuration strict build (a Shipping Game target on a scratch host is a full engine build;
the plugin's Shipping paths are `#if`-reduced, not include-reduced); the second office host's exact build mode is not observed.
