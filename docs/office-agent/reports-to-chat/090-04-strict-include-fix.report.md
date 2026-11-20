# REPORT for 090-04-strict-include-fix.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-04-strict-include-fix.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-30 10:43:37 | finished: 2026-09-30 13:06:30 | duration: 143 min | exit code: 0
context at last turn: 553641 tokens (55% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-04-strict-include-fix.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**For the owner:** both office hosts should pull **`feat/m53-uv-normal-corruption` at `fee8c7a`**; a host on the fix branch pulls `fix/m52-label-timing` at `37e01f0`. That commit fixes the second office host's compile errors (`ELevelTick`, `UMaterial`) with added includes only, and both branches build clean here with no warnings.

**Why NEEDS-DECISION:** the brief's gate was a strict build with 0 errors. Includes alone get it down to 3 errors that no include can fix. A tested 3-line change clears them, but it isn't an include, so chat decides.

**1. Includes added** (4 lines in `01d34d3`, merged into m53; nothing else in `Source/` changed)

| File | Added header | What it fixes |
| --- | --- | --- |
| `AnomalyInjector/Public/AnomalyInjectorSubsystem.h` | `Engine/EngineBaseTypes.h` | `ELevelTick`, the office root error |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | `Materials/Material.h` | `UMaterial`, the office C2027 |
| `AnomalyInjector/Public/IAnomaly.h` | `UObject/WeakObjectPtr.h` | C2504 `FWeakObjectPtr`, in 23 compile units |
| `AnomalyControlServer/Public/AnomalyControlServerSubsystem.h` | `IWebSocketServer.h` | C4150 on a `TUniquePtr` member; only its own module includes this header |

- **Found beyond the two known:** 2 more missing includes on the fix branch (the last two rows), and none in m53's own files.
- **One residual that is not an include:** 3 × C4150 in the generated `AnomalyCaptureSubsystem.gen.cpp`, on both branches and both targets.
  - That generated file defines the class's `FVTableHelper` constructor, which needs the complete types of three `TUniquePtr` members.
  - One of those types is defined inside `AnomalyCaptureSubsystem.cpp` and the other two in the module's Private headers, so no include can reach them.

**2. Strict build**
- **Flags:** per plugin module only, via a temporary block in each `Build.cs`: `bUseUnity=false`, `PCHUsage=NoPCHs`, `IncludeOrderVersion=Latest`. It also compiles one extra unit per plugin header (55 on the fix branch, 61 on m53).
- **Include order:** 5.1 only has `Unreal5_0` and `Unreal5_1`, and both hosts already use `Unreal5_1`.
- **Engine-wide flags not used:** `-NoPCH`/`-DisableUnity` would rebuild the whole engine and the shared editor DLLs.
- **Checks built into the tool:** it confirms from the compile response files that every plugin file really compiled alone without the PCH. It restores every `Build.cs` byte-for-byte (hash-checked; restore OK on all 4 runs).

| Run | Source | Game | Editor |
| --- | --- | --- | --- |
| 1 (control) | unfixed `0347e8c` | 12 errors | same 12 |
| 2 | with the 4 includes | 3 (the residual) | 3 |
| 3 (experiment, not committed) | plus the 3-line constructor | 0 | 0 |
| 4 | m53 `a74bf7f` | 3 (the residual only) | 3 |

Run 1 reproduced every error code in the office report at the same code sites, so the tool demonstrably catches this failure. Which build setting on the second office host causes it was not observed. Its error list lacks the 23 C2504s a fully non-unity build shows, so it very likely never hits the residual either. That is an inference.

**3. Normal build unchanged**

| | Fix branch (`_r84_host`) | m53 (`_r53_host`) |
| --- | --- | --- |
| Editor / Game | 15 / 6 actions, exit 0, 0 warnings | 15 / 6 actions, exit 0, 0 warnings |
| Exe before → after | `4B804F41` → `76428F44` | `1A99DDDA` → `45FD344C` (same size) |
| String scan | text literals identical; only instruction bytes, one extraction artefact and the debug-symbol record differ | 0 differing strings on the exe and all five DLLs |
| Lever audit (exe + DLLs) | PASS 33/33 | PASS 46/46 |
| Suites | selftests 284/0 and 154/0; Python suites all OK | C++ base 6/6, all 23 mutants fail as intended; Python 18/18 |

The string-scan tool reads 0 on a relinked identical build and 15,269 differences on fix-vs-m53, so its zeros are real readings. The comment stripper changed 0 files on both branches.

Archives (6/6 files re-hashed at the destination): `_binary_baselines\m52fix-09004-76428F44\` and `_binary_baselines\m53-09004-45FD344C\`. The experiment's diff is in the first.

**4. Commits pushed**
- `fix/m52-label-timing`: `01d34d3` (includes, `tools/strict_include_build.py`, a new strict-include box in the pre-delivery checklist) and `37e01f0` (journal `docs/sessions/2026-09-30-090-04-strict-include-fix.md`, status block, gotchas G424–G425).
- `feat/m53-uv-normal-corruption`: merges `a74bf7f` (source) and `1449d9e` (docs), then `fee8c7a` (the m53 half of the journal and status block).
- Merge conflicts were only in `CLAUDE.md` and `gotchas.md`, where both branches had appended.
- `m51`, `master`, tags, `ToCodex\` and `E:\<personal-folder>` untouched. No game launch, cook or staging. Only my own processes were stopped.

**5. NEEDS-DECISION**
1. **Approve the 3-line constructor?** Declare `UAnomalyCaptureSubsystem(FVTableHelper& Helper);` in the header, and define it in `AnomalyCaptureSubsystem.cpp` as `: Super(Helper) {}` — the same body UHT generates today. UHT then stops generating its own copy, and the strict build reads 0 errors on both targets (run 3). Without it, the new checklist box has to accept exactly those 3 errors; the box says so.
2. **Shipping configuration:** it was not strict-built, because that needs a full engine build on the scratch host.

MCP note: several claude.ai connectors need authorization in their connector settings, and the pdf-viewer server failed to connect; none was needed for this task.
api-equivalent cost (not billed on Max): $18.45 | turns: 140 | running total across all briefs: $3,894.93
