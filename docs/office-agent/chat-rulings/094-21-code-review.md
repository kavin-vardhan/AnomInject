# 094-21 — Claude Code review of the exit-path fix / wider static check / six rebuilds (W3: written after Codex exited)

**Date:** 2026-10-28, 03:50–04:30 IST · **Author:** Claude Code (mailbox brief 094-21) · **Inputs:** Codex's report `_reviews\094-21-codex-report.md` (thread `01a14334`, resume 2026-10-28 01:49:07 – 03:49:29 IST, 7,223 s, exit 0, ledger row `094-01-europa-phase-a-plan-resume19` once), chat's `094-20-ruling.md` (EV1–EV7), `094-21-request.md` and the rulings they bind. My receipts: `code-receipts-094-21\` (and `step0\`, `protect\`, `ek1\`, `review\`), `D:\IA_Europa\Receipts\CodeReview09421\`, `D:\IA_Europa\Temp\CodeReview09421\`. No account identifier is reproduced.

## Summary
- **Codex ended GREEN in 2.0 h of its 9 h.** Commits on `feat/europa-port` (local, unpushed, no tags): `40a8eaf` (source + tooling) and `7039df7` (3 docs files only).
  - **EV5:** every root change goes through a guarded `ChangeRoot`; `ReleaseGC` and `ScreenshotDone` skip at purge; the world map is keyed by `UPTRINT`, so no weak pointer is built from a dying world; the static check now refuses `LoadClass`, `StaticLoadClass`, `TObjectIterator`/`TObjectRange`, `FindFirstObject` and root changes outside the guarded helper.
  - **ES4:** six rebuilds exit 0 on the first attempt, EI4 query 0 actions, manifests re-bound 27/39/44 (110 files).
- **My independent results all agree**, with one non-blocking GAP (G1) and four OPENs.
  - Static: Codex's check PASS on 3 hosts + templates, 25/25 self-tests each; **my 21 named-family mutants 21/21 detected**; 8 sibling-API mutants outside EV5's list survive (O2), none of those APIs occurs in the probe.
  - Builds: receipts, flags, EO3, EI2/EV4 admissions, binary re-hash and manifest re-hash (110/110) all check.
  - **EN1 holds on all six makefiles once the name test is scoped to variables.** My raw-byte test fails on all six because the canary **name** sits inside one value: Codex's wrapper variable `EUROPA_BOOTSTRAP_PROOF` (a JSON receipt of removed names plus SHA-256s), new since 094-20 and carried into makefiles for the first time by these builds (G1).
  - 0 EU2 pauses (D: never below 73.54 GiB), so no edit could fall inside one (EV1). EU1: 117 filtered + 1 direct read-only + 0 direct write-capable, equal to Codex.
  - P9 v2 18,264/18,264; m51 = P9; protected state PASS; content equal; 0 launches; canary value 0 in 2,580 files; full EK1 0 values.

## 1. Rulings
| # | Ruling / item | Evidence (mine unless stated) | Status |
|---|---|---|---|
| 1 | EV1 no edit while a pause is open | No pause window existed: D: ≥ 73.54 GiB on 439 readings at 15 s (`events-offset-timeline.tsv`) and 121 at 60 s (`space-series.txt`); the only pause-named receipts are self-test fixtures under `Receipts\09421\disk-tests\`. The 22 file-edit items are mapped to 15-s windows (`review\ev1-item-times.json`), all run-folder helper scripts. Codex additionally checked edits against supervisor intervals: none | OK (measured) |
| 2 | EV2 report-only past the limit | Not reached (2.0 h) | N/A |
| 3 | EV3 D: 40/30/25 | `disk_pause.LIMITS` and `write_ledger.floors()` changed only the D: tuple (diff read). `disk_pause.py --self-test` 12/12. **My can-fails re-scaled to EV3** (19/29/31 GiB, port growth 50 GiB): A pause→resume (75), B port-caused STOP (2), C 4 h TIMEOUT (76), D sub-warning reset then RESUMED at 180 s; mutant (port growth zeroed) detected. D's "polls == 5" expectation is my harness's, already recorded wrong in 094-20 (4 polls is correct) (`review\eu2-canfail-ev3-*`). My launch hold used 40 GiB | OK |
| 4 | EV4 EI2 start gate 10 GiB | All six build admissions: policy physical 4 / commit 10 / stop 6, admitted on the 3rd 20-s poll; EI4 query kept 16 GiB. Running commit minimum 14.34 GiB, no stop event. Physical free touched 0.48 and 0.08 GiB during the two 5.1 Game links (O3). Codex's EV3/EV4 policy tests 12/12 | OK |
| 5 | EV5 O1: no UObject access after the exit purge | Diff `1722348..40a8eaf` read and every path walked (§2). `Stop()` after the purge: cvar restore (plain int `FAutoConsoleVariableRef`, no callback), GLog/delegate/console removal, `ReleaseGC` skip branch (resets containers, `Emit(nullptr)`), `ChangeRoot` returns at its guard, `Emit` reads only the cached map. `Cleanup`/`Cleaned` use the cached `UPTRINT` map. Root changes now exist only inside `ChangeRoot` | OK |
| 6 | EV5 G3: wider static check + mutants | Codex's check PASS on Paris, Baseline51, ReGate51 and templates, 25/25 self-tests each (1 real + 24 mutants). **My mutants, inserted at function end so the entry-guard rule cannot explain a hit:** LoadClass ×2, StaticLoadClass, TObjectIterator ×2, TObjectRange, FindFirstObject ×2, RemoveFromRoot in Stop (plain and spaced), Cleaned, EndFrame; AddToRoot in ReleaseGC's purge branch; three guard mutations; plus my 094-20 set — **21/21 detected by the named family** (`review\static-mutants-ev5.json`) | OK · OPEN O2 |
| 7 | EV5 one source, `ENGINE_MAJOR_VERSION`, no production edit | 5/5 files SHA-equal across the 3 hosts (`CAD31FA0`/`1F5135C2`/`12903D62`; Module/Build.cs unchanged) and byte-equal to the templates at `40a8eaf`; `ENGINE_MAJOR_VERSION` at 9 sites; `git diff 1722348 7039df7 -- Source Content Shaders Config *.uplugin *.Build.cs *.uproject` empty | OK |
| 8 | EV6 glass-normal baseline | No content change: Paris 1,453 files and plugin 5 equal | OK (carried to A7) |
| 9 | EV7 carried | Launcher still the old instance (pid 26116, 2026-10-27 10:38); EU3/EU4/ER1 G3/ER3/alias tests not exercised | OPEN (carried) |
| 10 | EU1 command audit | `review\eu1_audit21.py` over `events.jsonl` from the launch offset: **118 commands — 117 through the wrapper** (item 1 called the old `filtered09420.ps1` without its Program argument and was refused, exit 1, before any child), **1 direct read-only** (item 2, `Get-Content` of the old wrapper, the ruling, the entry script and `launch-start.txt`), **0 direct write-capable**. Equal to Codex | OK |
| 11 | EU2 foreign dip pauses | 0 real pauses (row 1). Supervisor present (EU2 variables in every build child) | OK |
| 12 | EU3 / EU4 | For the arming job | N/A (carried) |
| 13 | EU5 | Ruled by EV6 | OK (closed) |
| 14 | EU6 order | Entry → EV5 (source 02:03, commit 02:06) → builds 02:10–02:30 → EI4 02:35 → re-bind → scans → exit proofs → docs commit 03:21 | OK |
| 15 | ET1 | Not re-run | OK |
| 16 | ET2 `EditorRuns` marker | `31124` 0 B, 2026-10-26T18:43:59.9006068Z, empty hash, only entry: identical at start and after exit; 4.25 install listing equal | OK |
| 17 | ET3 launcher writes foreign | `ProgramData\Epic`: EOS Host/InstallHelper logs at 21:13 UTC, `catcache.bin`, `.lock`, `Revision.json`, `*.egi` — launcher family | OK |
| 18 | ET4 | No runtime leg | N/A |
| 19 | ET5 incl. 5.1 GC-time lookup | Lookups only in `CacheClasses` (lines 83/85), guarded, called only from `Initialized` | OK |
| 20 | ET6 | Closed by EU5/EV6 | OK |
| 21 | ET7 owner-slot temp files | Unchanged (670,796 B and 0 B `.tmp`, `ecache.bin`); Codex read them with Read\|Write\|Delete sharing | OK |
| 22 | ES1 / ES3 | Not re-run | OK / N/A |
| 23 | ES2 probe defect fixed in our probe | Rows 5–7 | OK |
| 24 | ES4 rebuild and re-bind | §3 | OK |
| 25 | ES5 no re-arm | Kit `4b5b2812…`/`5d1d1ab5…` unchanged, not run. It still binds the 09415 manifest, which now mismatches 4/27 → it would refuse at its own manifest check | OK |
| 26 | ER1 G3 / ER3 | Carried | OPEN (carried) |
| 27 | ER2 / ER4 | Not exercised | N/A |
| 28 | EO1 reader | Used for every leg; executed log lines joined to native graphs | OK |
| 29 | EO2 fallback | Unused | N/A |
| 30 | EO3 makefile refresh | Six receipts each match my before-snapshot hash (`9543f9dd`, `0e76412b`, `1e77c7f5`, `6250c45a`, `f806943c`); 4.25 Editor "No existing makefile"; EI4's refresh `51ce2576` = the makefile 51base-editor regenerated | OK |
| 31 | EO4 one probe source | Row 7 | OK |
| 32 | EO5 closeout | EA8 entry/exit PASS (Codex); report collected byte-identical; ledger row once; author = committer time on both commits. `report.md` and `last-message.txt` differ only in backtick formatting of one line | OK |
| 33 | EO6 | — | N/A |
| 34 | EN1 makefiles | Mine on the six regenerated makefiles: MSVC/SDK pins hold (4.25 14.16.27023 + 10.0.17763.0; 5.1 14.38.33130 + 10.0.22621.0; compiler exes the pinned MSVC); base PATH a byte-identical suffix; only `VC_COMPILER_DIR`/`VC_COMPILER_PATH` added; only TEMP/TMP routing differ; 0 blocked names; canary value absent. **Raw-byte canary name present once per makefile, inside `EUROPA_BOOTSTRAP_PROOF`'s value** → my strict test fails 6/6; scoped to variable names it passes 6/6 (`review\mk-en1.json`, `mk-en1-name-scoped.json`). Checker self-test 17/17 | OK (scoped) · **GAP G1** |
| 35 | EN2 rebuild before query | EI4 ran after the rebuilds, 0 actions | OK |
| 36 | EN3 | — | N/A |
| 37 | EN4 my self-test writes | Only under `CodeReview09421` | OK |
| 38 | EN5 hard STOPs | None fired; Codex claims no new exception; its three applied items are cited to source | OK |
| 39 | EM2 `CLAUDECODE` | Absent from all 125 recorded child environments; present in the launch base (positive control) | OK |
| 40 | EL1 denylist at every boundary | 133 child receipts: 125 with environments PASS (59 python, 59 git, 7 UnrealBuildTool), base preserved, 0 tool/secret names; 8 without (7 time-budget notes, 1 failure note) | OK |
| 41 | EL2 canary | Launch positive control PASS. By value from 20:19:06Z (`review\canary-search-postjob.json`, 14 roots): **2,580 files, 0 value hits, 0 other canary-shaped**. By name in child environments: 0. Name in files: 18 (the 6 makefiles via G1, Codex's scan receipts/scripts, my receipts) | OK |
| 42 | EL3 / EL4 / EL5 | — / kit not run / only `CLAUDE_CODE_MESSAGING_TOKEN` removed | N/A / OK / OK |
| 43 | EK1 secrets | Pre-scan 42,188 files 0 values; post-scan of files changed since launch 1,762 files 0 values; **full re-scan after exit 43,037 files 0 values**, live-value match 0, date-shaped 0; 2 read errors = the two owner-slot `.tmp` files (my scanner opens without Delete sharing). Codex's full scan 42,685 files 0 values | OK |
| 44 | EK2 / EK3 | Makefiles decoded 6/6 / `GIT_OPTIONAL_LOCKS=0` in children; m51 index unchanged | OK |
| 45 | EI2 memory | Row 4 | OK |
| 46 | EI3 engine work | 0 engine actions in every leg | OK |
| 47 | EI4 clean query | Baseline51 Editor, after its rebuild, 0 actions, exit 0 | OK |
| 48 | EG1 5.1 bookkeeping | 5.1 `Engine\Intermediate\Build` 622 and engine-plugin `Intermediate\Build` 400 changed paths, 0 added/removed, classes `LiveCodingInfo.json`/`UHT\Timestamp`/`ActionHistory.bin` — the same counts as 094-15. 5.1 Binaries, plugin Binaries, BuildRules, Programs\UBT, UbtRuns, Saved: equal. 4.25 install equal. `%LOCALAPPDATA%\UnrealEngine\XmlConfigCache-E+EpicGames+UE_4.25.bin` rewritten at 02:13:48 (same 190 B, 4.25 UBT), classified by Codex as a port C: exception | OK |
| 49 | EG2 receipts | Every leg: invocation, admission, effective environment, actions, native graph, result, UBT log | OK |
| 50 | P9 v2 / protected / m51 | P9 v2 re-hash 18,264 OK (`review\p9-rehash-console.txt`); protected PASS at start and after exit; m51 PASS at start, dry run, launch, after exit, final; its five can-fail cases pass | OK |
| 51 | Launches | 0 launch-class processes of ours on 13 polls, no sighting file; 4.25 install and 5.1 Saved/UbtRuns unchanged; only UBT/cl/link children | OK |
| 52 | Commits / push / tags | `40a8eaf`, `7039df7` local; no upstream, no remote ref, no tags. ReGate51 detached at `40a8eaf`, Paris on the branch at `7039df7`, Baseline51 at `d7eee97`; all clean | OK |
| 53 | Self-test suites | §5 | OK |

**Count (primary status per row, 53 rows): 43 OK · 8 N/A · 2 OPEN (carried) · 0 GAP rows · 0 STOP · 0 stop violations.** GAP overall: G1 (on an OK row). OPEN: O1–O4 plus the carried items.

## 2. EV5, path by path
- **Exit order (both engines, from 094-20 §3, unchanged):** `AppPreExit` → `StaticExit` sets `GExitPurge` and purges → only then `ShutdownModule` → `Probe.Stop()`.
- **`Stop()`** touches no UObject after the purge (row 5). It records `root_release=skipped` when exit, purge or GC is active.
- **`ReleaseGC`**: the purge branch resets `GCInjector`, `GCFields` (raw `Original` pointers dropped, not dereferenced) and `GCArrayObject`, then emits `gc_restore_skipped`. Note: `Stop()` emits it at every exit, even when no GC probe ran (cosmetic).
- **`ScreenshotDone`**: guarded; at exit/purge/GC it drops the pending picture with a `screenshot_skipped` receipt.
- **World map**: `TMap<UPTRINT, FWorldState>` with a weak `World` inside the state; `Emit` and `Cleaned` use only the address key and the cached identity/name. This removes Codex's own finding that `Worlds.Find(World)` constructed a weak key (which dereferences the object) during the purge.
- **Entry guards**: `Identity`, `IsInspectable`, `ChangeRoot`, `Start`, `CacheClasses`, `Command`, `EndFrame`, `Execute`, `ScreenshotDone`, `ReleaseGC` begin with the exit/purge/GC guard; `CaptureWorld`, `LoadScript`, `RequestPicture`, `GCProbe`, `LifetimeProbe`, `TickStart`, `TickEnd`, `MakeDormant` begin with `UnsafeSnapshotReason(World)`.
- **Behaviour change outside teardown (O4):** `LoadScript`, `RequestPicture` and `MakeDormant` now refuse a null, unreachable, pending-kill or tearing-down world (`Europa.Probe script <path>` with a null world now returns false). `Identity()` returns `unavailable` from the moment exit is *requested*, before the purge.

## 3. The rebuilds
| Leg | Commit | Seconds | Executed | EO3 receipt | New bound binary SHA-256 |
|---|---|---:|---|---|---|
| 425-editor | 40a8eaf | 58.7 | 3 compiles, lib, dll, target | none (absent) | `UE4Editor-EuropaPhaseAProbe.dll` 9458cb95… |
| 425-game | 40a8eaf | 62.2 | 3 compiles, exe, target | 9543f9dd | `StylizedParisStreet.exe` a2db7a02… |
| 51base-editor | d7eee97 | 74.7 | 3 compiles, lib, dll, metadata | 0e76412b | `UnrealEditor-EuropaPhaseAProbe.dll` ac3b1ca4… |
| 51base-game | d7eee97 | 85.1 | 3 compiles, exe, metadata | 1e77c7f5 | `StackOBot.exe` b6def734… |
| 51port-editor | 40a8eaf | 61.8 | 3 compiles, lib, dll, metadata | 6250c45a | `UnrealEditor-EuropaPhaseAProbe.dll` f8cab87d… |
| 51port-game | 40a8eaf | 83.1 | 3 compiles, exe, metadata | f806943c | `StackOBot.exe` 5b0fb171… |
| EI4 query | d7eee97 | 47.7 | none | 51ce2576 | — |

- All exit 0 with `-NoEngineChanges -NoBuildUHT -MaxParallelActions=4`, 0 engine actions, gate PASS, effective environment PASS; diagnostics only the two known 4.25 user-XML schema warnings. Each leg's recorded binary hashes re-hash equal now.
- **Re-bound manifests** (`Receipts\09421\qualification\`): 425 `c501895c…` 27/27, Baseline51 `b05e6f8c…` 39/39, ReGate51 `3865fe0c…` 44/44 — **110/110 re-hashed OK** (`review\rehash-new-manifests.json`). Labels: source `40a8eaf` (Baseline51 `d7eee97`), probe/tooling `40a8eaf`, prior manifest and hash recorded.
- **Changed bindings vs 09415:** only probe-linked outputs — 425: game exe/pdb, probe dll/pdb (4); each 5.1 host: game exe/pdb, `.target`, probe dll/pdb (5). Every plugin DLL is byte-unchanged. The old 09415 manifest bytes are unchanged and now mismatch 4/5/5, as intended.

## 4. G1 — the wrapper's proof variable reaches the makefiles
- **What:** `EUROPA_BOOTSTRAP_PROOF`, set by Codex's wrapper (`filtered09420/09421.ps1`, `bootstrap09421.py`; introduced in 094-20, when no build ran), is part of every child environment, so UBT stores it in all six makefiles (pairs 98/100 in 094-15 → 101/103 now, with `EUROPA_DISK_SUPERVISOR_CREATED`/`_PID`).
- **Its value (shape only):** `result: PASS`, `child_blocked_names: []`, `removed_names` (15 names, including `IA_CANARY_TOKEN`, `CLAUDECODE`, `CLAUDE_CODE_*`, `CODEX_*`), `canary_sha256` (equals `canary.txt`), and `preserved_sha256_by_name` — SHA-256s of 84 preserved variable **values**, none secret-class, but including identifier-class ones (`USERNAME`, `COMPUTERNAME`, `USERDOMAIN`, `USERPROFILE`).
- **Effect:** no secret value and no canary value anywhere; the canary *name* now appears in makefile bytes, which 094-15's EN1 state did not have, and low-entropy identifiers appear as hashes. Non-blocking under EK1/EL1/EL2 as written.
- **Chat's call:** accept it as wrapper metadata, or keep the proof out of build children (UBT persists the environment into makefiles, and a runtime leg launched through the same wrapper would carry it into the engine process).

## 5. Self-tests (re-run by me)
- `owner_eq_self_tests.py` **159/159** (11 harmless children); `owner_kit_self_tests.py` **62/62 + 48 measured**; `idle_gate.py --self-test` **48/48** — on copies changing only the fixture TEMP base, under `CodeReview09421`.
- `disk_pause.py --self-test` **12/12** (EV3 values); `probe_teardown_check.py --self-test` **25/25 ×4**.
- EA11 `check_reflected_headers.py --self-test`: Paris **PASS**, ReGate51 **PASS**; release control Baseline51 `d7eee97` **FAIL, 12 violations** (expected pre-port); synthetic mutant detected in all three.
- Mine: EV5 mutants 21/21 + 8 siblings; EU2 can-fails 4/4 + mutant under EV3; EN1 checker 17/17; m51 gate can-fail 5/5; canary search; EK1 pre/post/full; build and manifest re-hash.

## 6. Writes and snapshots
- **Mine:** `code-receipts-094-21\`; `resume-prompt-094-21.txt`; the preserved `094-20-*` copies (hash-equal to `code-receipts-094-20\`); `_reviews\094-21-codex-report.md` (body byte-identical, `E0D784E2…`); this file. In `D:\IA_Europa` only `Receipts\CodeReview09421\` and `Temp\CodeReview09421\` (pre-EV5 probe copies, harness copies, EU2 roots). I launched nothing, ran no build, deleted nothing, and stopped no process other than my EU2 harness's own python sleepers (stopped by the code under test, as the can-fail requires).
- **Before → after (61 listings): 34 equal.** The 27 that differ are expected: the three hosts' Binaries/Intermediate/Source (rebuilds, probe source), both 5.1 plugins' Intermediate and `UnrealEditor.modules` (metadata), CaptureBench `.modules`, EG1 engine bookkeeping, `%LOCALAPPDATA%` 4.25 XmlConfigCache, refs/worktree heads (`feat/europa-port` and ReGate51), `D:\IA_Europa` +744/~1,024 (740 `Receipts\09421`, host trees, worktree docs/tools), `ProgramData\Epic` (launcher), free space, processes, drive roots, tooling sizes.
- **Still running (owner's/foreign):** the Epic Games Launcher (26116, old instance) and helpers; both mailbox watchers.

## 7. GAPs and OPENs
- **G1 (EN1/EL2 scope, non-blocking):** §4.
- **O1:** the exit-path fix is proven statically only; no runtime leg has exercised an exit inside the dormant window or a GC probe on either engine.
- **O2 (static check coverage):** 8 sibling mutants pass the check — `StaticFindFirstObject`, `FObjectIterator`, `ForEachObjectOfClass`, `GetObjectsOfClass`, `ClearInternalFlags(RootSet)`, `TSoftObjectPtr::LoadSynchronous`, a weak pointer built from a raw world in `Cleaned`, and a plain dereference in `Stop()`. None occurs in the probe today; the last two are beyond what a pattern check can see.
- **O3 (memory):** physical free fell to 0.08/0.48 GiB during the two 5.1 Game links; EV4 has no physical in-run stop and commit stayed ≥ 14.3 GiB.
- **O4 (behaviour change for the arming job):** §2's last bullet.
- **Carried:** EV7's arming items (EU3, EU4, ER1 G3, ER3, alias tests), the launcher restart, EO1 budget, A9, EJ1/EJ3/EJ5, O3 (ET7) for A8.

## 8. Top 5 risks for the arming job and the first automated leg
1. **The exit fix has never run.** The first runtime leg on each engine should show `probe_stop` with `root_release`, `gc_restore_skipped` where applicable, and a clean exit; a crash there means a path the static check cannot see (O2's last two classes).
2. **The owner kit is bound to the 09415 manifest, which no longer matches the binaries.** It fails closed today; arming must re-bind it to the 09421 manifests, or any leg using it refuses (or, if hand-edited, runs unverified binaries).
3. **EV5 tightened non-teardown entry points (O4).** A scripted automated leg must issue `Europa.Probe` commands with a live, non-tearing-down world, and receipts written after exit is requested carry `unavailable` identities.
4. **G1 travels with the wrapper.** A leg launched through it puts `EUROPA_BOOTSTRAP_PROOF` (the canary name, identifier hashes) into the engine process environment, where crash handling or logs could capture it; settle G1 before EU4's upload decision is tested.
5. **The machine's headroom is thin.** Physical memory reached 0.08 GiB during a 5.1 link with only foreign load (`wlanext` 9.4 GB and growing); a runtime leg next to a full editor will be tighter. The owner restart (EV7) should precede it; EI2's 16 GiB gate still governs full builds.

VERDICT: REVIEW DONE — 43 OK / 8 N/A / 2 OPEN rows (carried) / 0 GAP rows, 1 GAP overall (G1 bootstrap-proof variable in makefiles, non-blocking) / OPEN O1–O4 / 0 STOP / 0 stop violations. EV5 verified from source; widened check 25/25 ×4; my 21/21 named mutants detected, 8 siblings survive (none present). Six rebuilds + EI4 exit 0, 0 engine actions; 110/110 bound files re-hash; manifests c501895c/b05e6f8c/3865fe0c. EN1 6/6 scoped (raw name in proof var). 0 pauses, 0 edits in pauses; EU1 117 filtered / 1 read-only / 0 write-capable. Canary 0 in 2,580 files; EK1 0 values (full 43,037). P9 v2 18,264/18,264. m51 = P9. 0 launches.
