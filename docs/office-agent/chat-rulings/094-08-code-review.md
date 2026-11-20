# 094-08 — Claude Code review of Codex's Europa A4a (W3: written after Codex exited)

**Date:** 2026-10-24, 21:49–22:05 IST · **Reviewer:** Claude Code (mailbox brief 094-08-europa-a4a-425-builds)
**Job:** resume of thread `<uuid>` in `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\`,
gpt-6-astra / max / danger-full-access, 16:54:57–21:48:25 IST, 17,608 s, exit 0, one try. Codex's verdict:
`GREEN | A4a clean 4.25 trio and strict pass complete on 489c828; same-source ReGate51 Editor passes; P9 v2 and ledgers pass; A4b pending; no push`.
Codex's report: `_reviews\094-08-codex-report.md` (report.md sha256 `DA623406…`). My receipts:
`…\code-receipts-094-08\` (snapshots, `review\`) and `D:\IA_Europa\Receipts\CodeReview09408\` (scans, self-tests).

## Summary
- **A4a holds up.** Production SHA `489c828`; result HEAD `55764fd` changes only 4 docs/reach files. The only production
  change in `204724a..489c828` is one line, `#include "UObject/WeakObjectPtr.h"`, inside the whole-file
  `#if ANOMALY_ENGINE_425` boundary of `Compat/AnomalyUnsupportedTexture.cpp` (C3: 5.1 effect N).
- **The final clean trio on `489c828`**, each from empty owned output roots and read by me from the logs and responses:
  - Editor Development: 35 actions (11 project / 24 plugin / 0 engine).
  - Game Development: 16 actions (7 / 9 / 0).
  - Shipping: 5 actions (5 / 0 / 0).
  - All three: exit 0, 0 errors, 0 compiler or linker warnings, `-NoEngineChanges`, and the toolchain line
    `Visual Studio 2017 14.16.27051 … Windows 10.0.17763.0 SDK`. Every 4.25 response carries the MSVC `14.16.27023`
    (cl 19.16.27051) and SDK `10.0.17763.0` include paths.
- **Shipping excludes every IA and probe module, by four independent readings:**
  - the action list (shared PCH, rc, `StylizedParisStreet.cpp`, link, target);
  - the target receipt's BuildProducts (the Shipping exe and pdb only);
  - the Shipping UHT manifest (no Anomaly module; Game Development lists Bench, Injector, Capture and ControlServer);
  - a both-encodings string scan of the Shipping exe: 0 hits for all 11 IA and probe needles, while the Development
    Game exe hits every needle (the scan can see them).
- **The strict pass is real and complete:**
  - 77/77 plugin headers, each with a compiled TU (7 reflected, 38 private), 64/64 ordinary `.cpp` and 11 `.gen.cpp`,
    on both targets;
  - all 152 plugin responses `/std:c++17`, with no `/Yu` `/Yc` `/Fp` and no unity source;
  - the real include mutant fails with the predicted C2027;
  - the 5 `Build.cs` files restore byte-exact;
  - normal rebuilds pass.
- **P9 v2 is unchanged:** my re-hash is 18,264/18,264 OK. The protected state is equal at start and after exit. The cache
  is unchanged (the owner has not deleted). The 4.25 install is equal. The 5.1 engine got only EG1 bookkeeping:
  369 files, 1,296,969 B.
- **Counts: 36 OK · 0 GAP · 7 OPEN · 0 STOP · 0 CONFLICT.**

## 1. Rulings
| Ruling | Evidence | Status |
|---|---|---|
| EH1 three clean builds | `builds\{editor,game,shipping}-final` at `489c828` (`invocation.json`, `UBT.log`, `result.json`), read myself (`CodeReview09408\builds-scan.json`). Editor 35, Game 16 and Shipping 5 actions, all executed, `engine_exec` 0, exit 0, 0 error lines, 0 compiler/linker warnings. The superseded `*-clean-1` trio at `cae8fbf` is identical in counts. | OK |
| EH1 `-NoEngineChanges` | All 111 UBT command lines in all 130 job logs carry it (`all-ubt-scan.json`). | OK |
| EH1 toolchain from actual responses | 4.25: 13/13 Editor, 13/13 Game, 2/2 Shipping, 156/156 strict responses contain `MSVC\14.16.27023` and `10.0.17763.0`, and none contain 14.38 or 22621. The UBT line reads 14.16.27051. My step-0 listing: the `14.16.27023` cl.exe is 19.16.27051. ReGate51: 14.38.33144 / 22621. | OK |
| EH1 warnings counted | See EH3. | OK |
| EH1 Shipping exclusion | Five-action list; BuildProducts exe+pdb only; Shipping `.uhtmanifest` has no Anomaly module; no `Shipping` folder under the plugin's Intermediate; exe string scan 0/11 needles both encodings (positive control: Dev Game exe hits all, probe UTF-16 1). The descriptor has `BlacklistTargetConfigurations=Shipping` on all 5 plugin modules and on `EuropaPhaseAProbe` in the `.uproject`. One honest residue: the target receipt keeps `$(ProjectDir)/Plugins/AnomalyInjector/AnomalyInjector.uplugin` as a UFS RuntimeDependency (descriptor, not a module product). See OPEN 3. | OK (build level) |
| EH1 A1 parser can-fails | `strict\attempt4\reader-can-fails.json` and `self-tests\readers.json`: positive PASS; nonzero exit, added error, compiler version, missing compile, missing command, SDK, rules-version-before-schedule, engine and engine-plugin action, strict PCH and strict unity all REJECTED; marked synthetic_only. | OK |
| EH2 clean by receipt | `cleaning\EH2-initial.json`: 4 roots (host `Binaries`, host `Intermediate\Build`, plugin `Binaries` and `Intermediate`), 195 files / 1,706,310,565 B, every file with size, mtime and sha256 before deletion. It equals my own pre-launch hashed listing exactly (195/195, 0 hash or size differences; `cleaning-check.txt`). The three later cleanings (769 / 1,215 / 1,215 files) use the same 4 roots, with 0 files or folders outside them. The probe's outputs are inside the host roots. | OK |
| EH3 warnings | My scan of all 273 job logs: 0 compiler or linker warnings anywhere. Only `BuildConfiguration.xml(16)` and `(22)`, the accepted environment warnings. Codex's 42 + 42 = 22 compiles + 20 queries on 4.25; 5.1 has none. That file was not touched (C: snapshots). | OK |
| EH4 coverage | `strict-coverage.json` (mine): git at `489c828` has 77 `.h` and 64 `.cpp` under `Source`. `header_tus.json` maps 77 TUs to exactly those 77 headers (0 missing, 0 extra, 0 duplicates; 7 reflected, 38 Private). On each target, all 77 TU files appear in compile responses, all 64 `.cpp` compile, and 11 `.gen.cpp` = 7 reflected + 4 module init. | OK |
| EH4 unity and PCH off | Of 156 responses per strict target, 152 are plugin: all `/std:c++17`, 0 `/Yu` `/Yc` `/Fp`, 0 `Module.*.cpp` sources. The 4 PCH responses are host (`StylizedParisStreet` + shared PCH), outside the plugin scope. | OK |
| EH4 zero-TU/zero-action fails | Covered by Codex's UHT reader suite (27 cases incl. zero coverage and missing generated action). I re-ran it: 27 PASS. | OK |
| EH4 restoration | `restoration.json`: 5 rule files, before = after sha256; `status_before` = `status_after` = "". The 5 current `Build.cs` hashes equal `buildcs_backup\`. 0 `StrictIncludeCheck*` folders remain. The Paris worktree has 0 status lines. | OK |
| EH4 normal rebuild after | `normal_StylizedParisStreetEditor` 20 actions and `normal_StylizedParisStreet` 11, exit 0, 0 errors and warnings. | OK |
| EH4 source mutant | `include_negative`: `SIC_MissingDefine09408.cpp(3): error C2027: use of undefined type 'UStaticMeshComponent'`, exit 6, `EXPECTED_FAILURE`; `include_positive` compiles. The mutant is gone. | OK |
| EH4 fix at the use site | The C2504 at `WeakObjectPtrTemplates.h:31`, instantiated from `AnomalyUnsupportedTexture.cpp:30` (`TWeakObjectPtr<AActor>`), is fixed in that file with 4.25's defining header `UObject/WeakObjectPtr.h`, inside the 4.25-only boundary. | OK |
| EH5 re-run on final SHA | Source changed, so the final trio was re-run clean at `489c828` after `EH5-final` cleaning (1,215 files); `builds\regate-final` at `489c828`: 4 actions (unity compile, lib, dll, metadata), exit 0, 0 errors and warnings, all products inside ReGate51. ReGate51 is detached at `489c828`, clean. | OK |
| EH5 JsonExport conditional | `git diff --stat 204724a..55764fd` shows no `.cs`, `.uplugin`, `.uproject` or Config change (the temporary strict `Build.cs` edits are restored byte-exact). Not triggered; I did not re-run it. | OK (not triggered) |
| EH5 final hashes | `qualification\final-425-binaries.json`: 27 files. My re-hash: 27/27 size, mtime and sha256 equal. Exactly these 27 files exist in host and plugin `Binaries`. The report table matches 27/27. ReGate51's `UnrealEditor-AnomalyInjector.dll` `dfc95c83…` and `.pdb` `31ddee9a…` match. | OK |
| EH6 scope | Codex's 230 child-process receipts: python, vswhere, git, UnrealBuildTool (46) only; 0 editor/game/commandlet/cook/UAT/push/tag. Only one 5.1 compile (ReGate51 Editor Development); Baseline51 untouched at `d7eee97`. No such process running now. | OK |
| EG1 bookkeeping | My window scan (`review\engine-window-51.json`, 16:54–21:49): 339,410 engine files, 369 hits = 207 engine UHT Timestamp (367,363 B) + 161 engine-plugin UHT Timestamp (183,375 B) + ActionHistory.bin (746,231 B) = 1,296,969 B; non-EG1 0; all written 21:24:55–21:25:37 (the ReGate51 run). v2 engine lines 339,409: 0 missing, 0 resized. Binaries, BuildRules, Programs\UBT, Saved and UbtRuns (17 markers) unchanged. Codex's charge is the same 1,296,969 B. | OK |
| EG1 clean-engine query | Not re-run in A4a (the 094-08 exit list does not ask for it). Low risk: 0 engine binary or object writes. | OPEN 4 |
| EG2 receipts | Each final `result.json` has `binary_hashes`, `host_binary_inventory`, `actual_compile_responses` and `toolchain_lines`; every 4.25 plugin response carries `/std:c++17`. | OK |
| EG3 `--output` mandatory | Bare `tooling_self_tests.py` exits 2 (`--output` required). | OK |
| EG5 P9 v2 | See §2. | OK |
| EG5 protected state | See §3. | OK |
| EF1 memory gate | 44 admissions, 0 stops or timeouts; total wait 8,561 s, longest 3,000 s (the owner's Blender ran 16:44–17:30, 17:53–18:51 and 19:00–20:30). My poll saw free physical down to 1.97 GiB while no port compile ran. `MaxParallelActions=1` on every command. My re-run of `finish_self_tests.py` asserted all 77 cases before the strict-wiring block (incl. one byte below each threshold, the wait limit's "no memory window", single-low no-stop and second-low stop for both readings), then hit the scope guard (OPEN 6). | OK |
| ED1 disk | Lowest: C 16.83, D 133.06, E 48.06 GiB. Now C 18.72 / D 133.55 / E 48.08. | OK |
| ED2 working compile | ReGate51 Editor Development at the same SHA (above). | OK |
| ED3 toolchain | Unchanged at start and end: VS 2022 17.12.35707.178, BuildTools 2017 15.9.34031.82, MSVC 14.36/14.38/14.42 + 14.16.27023, SDK 17763 + 22621, no 14.44. | OK |
| EE1 UHT C: classes | `%LOCALAPPDATA%\UnrealHeaderTool`: 10 `Saved\Config\Windows\*.ini` changed, 15 `CrashReportClient\UE4CC-Windows-*` added, 2 replaced; all 27 in EE1's classes, 0 outside (1,700 B). `UnrealBuildTool` folder equal. | OK |
| EB1 / EA4 C: and E: | Codex: C 5,655,034 B (conservatively including Windows event-log and WER growth plus the EE1 files and the 4.25 `XmlConfigCache`), E 0. Mine: `ProgramData\Epic`, UE TEMP entries, the D: and E: roots, Lyra binaries and the 4.25 install are all equal. On the C: root only `ClaudeTemp` and `System Volume Information` mtimes moved. | OK |
| EA8 entry and exit | Codex: 23,033 protected hashes and 472,553 files on each side. Mine: §2. | OK |
| EA11 | Mine at `55764fd` (Paris) and `489c828` (ReGate51): 7 reflected, 0 violations; the mutant still fails at line 13. | OK |
| EA14 / W4 commits | 5 local commits `cae8fbf`, `f463e3e`, `d9c91c1`, `489c828`, `55764fd`; author time = committer time on each; W4 receipt shows the child saw neither date variable; `feat/europa-port` absent from origin; refs 72 → 72 with only `feat/europa-port` moved (`204724a` → `55764fd`). | OK |
| Prediction addendum before build | `cae8fbf` 17:12:24 IST; the first UBT call (fresh JsonExport) is at 17:16:02. | OK |
| Comment invariant | No comment line added in any changed `.py` or C++ file (only the `#include` matched my pattern). | OK |
| Ledger row | The relay appended `094-01-europa-phase-a-plan-resume6` (exit 0, GREEN). It is unique: it no longer repeats `resume5`. I did not add a second row. | OK |
| EF3 / EE7 owner delete | Cache inventories at 16:47 and 21:52 are equal to each other and to 094-07's (432,470 files, 41 groups, 3 reparse points). | OPEN 1 (owner, any time) |

## 2. P9 v2
- v2 is `8e3ed095…2c74`, 472,552 lines, unchanged.
- **My re-hash** (`review\p9_rehash_v2.py`, seed 9408):
  - 18,244 protected exe/dll/pak/ucas/utoc plus 20 random v2 files = 18,264 OK;
  - 0 mismatches, 0 mtime differences;
  - 17.1 GB in 62 s.
- **Worktree heads and index hashes** (`worktree-heads-before/after.txt`): equal, except Paris (`204724a` → `55764fd`, on
  `feat/europa-port`) and ReGate51 (→ `489c828`, detached), both expected and both clean. Baseline51 is at `d7eee97`,
  clean. `m51` is at `53bf725` with 0 tracked changes.
- **Paris original** `E:\Unreal Projects\StylizedParisStreet`: hashed inventory, 1,512 lines, equal.

## 3. Protected state (EG5)
- **At start (16:46) and after exit (21:51),** every item equal:
  - the release baseline 8/8, size and sha256;
  - six junctions resolve;
  - `D:\IntrusiveAnomalies\StackOBot\Content` byte-identical to its 094-05 snapshot (2,744 lines);
  - `D:\IA_BankOverflow`: 0 missing and 0 changed against v1. The one extra file is again `09102-final-binaries.zip`
    (pre-v1, outside v1's roots).
  - `E:\IA_BuildCache\StackOBot\Binaries` (27 files, sha256) equal to my 094-07 after-snapshot;
  - StackOBot Config, Source and `.uproject` equal.
- **No RED condition.**

## 4. Engine trees
- **4.25:** the full install listing (179,503 lines incl. BuildRules) is equal before and after. The stock
  `UE4Editor.exe` is `4a6d8326…` (read, not launched).
- **5.1:** the EG1 row above. `Engine\Binaries\Win64` (1,608), all engine-plugin Binaries (2,469), BuildRules,
  `Programs\UnrealBuildTool`, `Engine\Saved` and UbtRuns are equal.
- **UBT logs:** `-NoEngineChanges` in all 111 command lines. The 22 action lists hold 1,021 actions, with 0 produced
  items in either engine tree and 0 outside the three port hosts.

## 5. Source and closure (`git diff --stat 204724a..55764fd`: 13 files, +769/−66)
- **Production:** `AnomalyUnsupportedTexture.cpp`, +1 line, inside `#if ANOMALY_ENGINE_425` (C3: no replacement
  definitions on 5.1; the 5.1 TU sees an empty file). It is 4.25's defining header for `FWeakObjectPtr`, the base
  `TWeakObjectPtr` needs.
- **Tooling (C):**
  - `a4_build.py`, `a4_evidence.py` and `uht_reader_self_tests.py` (new);
  - `strict_include_build.py`, `finish_self_tests.py`, `process_wrapper.py` and `write_ledger.py`.
- **Docs:** CLAUDE.md, the predictions addendum, the session journal, `09408-a4a-result.md` and the reach manifest.
- **Retained, not hidden:** strict attempt 1 stopped before UBT (an import path). Attempt 2 found the real C2504.
  Attempt 3 compiled but the coverage reader misread resources and Game UHT paths. Attempt 4 is the qualifying run.
  The source-fix count is 1, so D2 holds.

## 6. Self-tests (mine, `--output` under `D:\IA_Europa\Receipts\CodeReview09408\st\`)
- `tooling_self_tests.py` 31 PASS · `ledger_delta_self_tests.py` 9 PASS · `ledger_budget_self_tests.py` 5 PASS ·
  `resume_self_tests.py` 16 PASS · `uht_reader_self_tests.py` 27 PASS · `idle_gate.py --self-test` PASS (no runtime
  probe).
- `finish_self_tests.py`: 77 of 83 cases asserted (see EF1), then `RuntimeError: Unscoped build host or receipt
  directory` from `a4_build.command`. That guard requires any output under `write_ledger.JOB` =
  `D:\IA_Europa\Receipts\09408`, which I may not write. Codex's in-job run: 83/83
  (`self-tests\strict-import-fix\results.json`).
- The port worktree stayed at 0 status lines.
- **My own scratch, not deleted:** 7 empty folders (`CodeReview09408\{tooling,finish,delta,budget,resume,uht,idle}`)
  from a first attempt that the tools refused because the folder already existed.

## 7. OPEN items
1. **The owner's hand delete:** not done yet, any time (EG5).
2. **5.1 runtime parity and A5–A11:** unproven. Working and strict builds certify neither label/picture sync nor 5.1
   equivalence.
3. **The Shipping receipt keeps `AnomalyInjector.uplugin` as a UFS RuntimeDependency.** It carries no code, so a
   staged Shipping build would still carry the descriptor. Build-level exclusion is proven; package/staging exclusion
   is A6's.
4. **EG1's Baseline51 clean-engine query was not re-run in A4a.** Chat to say whether it is per-job or per-phase.
5. **5.1 UHT layout and the 5.1 strict pass have synthetic reader coverage only;** A4b runs them for real.
6. **The tooling is pinned to this job.** `write_ledger.JOB` is `Receipts/09408` and `a4_build.PAIRS` admits only the
   three Paris pairs plus ReGate51 Editor. A4b must re-scope both. While they stay pinned, `finish_self_tests.py` cannot
   be fully re-run outside the job folder, including by the reviewer.
7. **Build provenance for A6–A8:** the 27-file manifest above is the binding set. Any re-build changes it (G201).

## 8. Top 5 risks for A4b (5.1 monolithic builds) and A5/A6
1. **The 5.1 Game and Shipping builds are full engine compiles.** The engine tree has no `UnrealGame` intermediates.
   The main StackOBot's own 5.1 Game Development build holds 277 engine-module folders and 838 unity objects (4.72 GB)
   in its project Intermediate. So each of A4b's four monolithic builds (Baseline51 and ReGate51 × Development and
   Shipping) compiles about 840 unity actions at `MaxParallelActions=1`. That is many hours each (estimate, not
   measured) plus tens of GB on D:. Chat should set parallelism, ordering and the EF1 wait limit with that in mind.
2. **Action classification.** Those engine-source compiles produce items in the host's Intermediate, so a
   ProducedItems classifier calls them "project" actions. A4b's ruling should say how they are counted, so that
   "0 engine actions" stays meaningful and the shared engine tree stays untouched (EG1 cap: about 1.3 MB per 5.1
   invocation today).
3. **Memory.** The owner's Blender cost A4a about 2.4 h of EF1 waits. 5.1 monolithic links are heavier, so the 4-hour
   wait limit or an in-run stop is likely on an evening schedule.
4. **The 5.1 strict pass.** It needs PCH and unity off on 5.1, where the plugin normally uses shared PCHs. It meets 5.1
   IWYU first-include diagnostics, and its header TUs parse the `ANOMALY_ENGINE_51` bodies that A3's include moves only
   touched lightly. The 4.25 strict pass proves nothing about them.
5. **A6 packaging.** Build-level Shipping exclusion rests on `BlacklistTargetConfigurations`, the 4.25 key name that
   the shared descriptor also uses on 5.1 (the 09405 matrix showed equal membership). The descriptor still stages. A6
   must check packaged content and plugin enablement on both engines, and A9 the runtime registry on 5.1.

VERDICT: REVIEW DONE — 36 OK / 0 GAP / 7 OPEN (owner delete; runtime parity A5–A11; Shipping receipt keeps the .uplugin as UFS staging dependency; EG1 Baseline51 query not re-run; 5.1 UHT/strict synthetic-only; tooling pinned to 09408; A6–A8 bind to the 27-file manifest) / 0 STOP / 0 CONFLICT; A4a complete at 489c828 (Editor 35, Game 16, Shipping 5 actions, 0 errors, 0 C++ warnings, 0 engine actions, 14.16.27051 / 10.0.17763.0); Shipping has no IA/probe module by actions, receipt products, UHT manifest and exe strings; strict 77/64/11 both targets, PCH/unity off, mutant C2027, exact restoration; ReGate51 489c828 4 actions clean; EG1 369 files / 1.30 MB only; P9 v2 18,264/18,264; protected state equal at start and end
