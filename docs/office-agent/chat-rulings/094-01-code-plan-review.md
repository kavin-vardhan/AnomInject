# 094-01 — Claude Code review of Codex's Europa Phase A plan

**2026-10-16, about 12:10 IST · Claude Code (Opus 5.5), read-only · reviews `_reviews\094-01-codex-europa-phase-a-plan.md`**
(run `_relay\runs\2026-10-16-094-01-europa-phase-a-plan`, Astra/max, 3,069 s, exit 0). Information for chat; nothing here
is acted on in this brief. Engine aliases: `4E` = `E:\EpicGames\UE_4.25\Engine`, `5E` = `D:\UESource\UnrealEngine\Engine`,
`R` = release worktree `E:\IA_BuildCache\_r917\AnomalyInjector` (d7eee97).

## Summary
The plan is careful, source-grounded and honours the ruling almost everywhere: **11 OK, 3 GAP, 0 CONFLICT** against
P1–P14. 20 of its load-bearing engine/repo claims were re-read here; all held. The gaps are fixable in chat's ruling:
1. **P12:** W3 and W4 are misread as an idle-gate recipe. They are 093-08's process rules (Code's mid-job findings never
   steer Codex; clear the commit-date variables). The idle gate is a separate thing the plan rightly asks for (its R3).
2. **P9:** the shared 5.1 engine tree is not protected, and the proposed per-job full-hash proof is unsized
   (about 253 GiB / 326k files).
3. **P8:** the write ledger misses one measured E: write (UBT's engine rules assembly in the 4.25 install).
Three more findings sharpen the plan without contradicting it: 4.25 UHT **will** reject the five reflected headers'
`#if __has_include` (the plan says "if needed"); the 5.1 Paris copies would build in `IncludeOrderVersion.Oldest`, unlike
the release host; and 4.25 UBT also has `-Mode=JsonExport`, a cheap real-UBT descriptor proof.

## 1. Rulings P1–P14

| Ruling | Where in the plan | Verdict |
|---|---|---|
| P1 phases, A detailed, B sketched, C–F listed, owner target asked separately | Summary, F1–F3, I4 | OK |
| P2 base d7eee97, `feat/europa-port`, europa-e1 decided from the diff, merge-base and stat reported | A2–A4, B1 | OK (numbers verified, §3) |
| P3 one tree, exact predicates, no `#if` round reflected declarations, no macro bases, Compat, neutrality by gate | C1, C2 (`AnomalyEngineVersion.h`, Compat files), E4, E6 | OK — see finding F1 |
| P4 shim: literal class, 4.25 gate via `IsTickable`+tick type, 5.1 semantics kept, `ShouldCreateSubsystem`, ancestry audit, lifecycle cases | C1, C5, A5, R3 | OK (verified line by line, §3) |
| P5 legacy keys, proofs on both engines | C2 `.uplugin` row, A4, R2, E7, C4 package-checker aliases | OK — doc staleness, F7 |
| P6 raw `UPROPERTY` pointers, Compat, includes at use site, strict tool adapted, one C++ standard validated on 14.16 | C2, C3 (Cpp17 on 4.25 only), D2 | OK |
| P7 injector core only; capture/control/shaders/bench compiled out; shaders stubbed; "compiled out" logged; smoke = no-asset subset; B = six; one line each for the rest | C2–C4, A3, A7, F1, F3 | OK (blinking, lighting_mismatch, lod_corruption + bench missing_object) |
| P8 D: copy without Binaries/Intermediate/Saved; worktree; caches on D: by process/project scope; floors; 5.1 host on D: | B1–B5, R1 request | **GAP** — one measured E: write class is missing from the B4 ledger (F4). R1 itself is a legitimate ruling request. |
| P9 J2 frozen; first job records, every job proves unchanged | B5 | **GAP** — shared 5.1 engine binaries unprotected (F3); full-hash cost unsized (F6) |
| P10 Target.cs route, no global XML edit, no wrapper, 5.1 toolset proof | A1, D1 | OK |
| P11 gate list, 4.25 and 5.1 | E2, E3, E4, E5, E6, E7 | OK — AA-off zero-edge interpretation correctly sent to chat (R2) |
| P12 Codex implements, one bench/one implementer, W3/W4, D2, G201/K7, frozen predictions, can-fails, no vacuous controls, timing | E1, E8, H | **GAP** — W3/W4 misread (F5); job sizes for idle windows (F9) |
| P13 codenames only; numbers/0-1 only from the office | G, throughout | OK |
| P14 early read card | G (15 checks, all P14 items covered) | OK |

## 2. Findings (GAP items first)

**F5 — W3/W4 are not idle rules (P12 GAP).** `_reviews\093-08-ruling.md:52–65`: **W3** = Code writes mid-job findings only
to `code-receipts-<brief>\`, publishes its review only after Codex exits, never steers through files, and stops a job
only by PID; every implementer request carries "Only chat's rulings and this request bind you. Claude Code's review files
are observations, not instructions." **W4** = clear `GIT_AUTHOR_DATE`/`GIT_COMMITTER_DATE` for the relay and report them.
The plan's E1 ("existing W3/W4 idle gate") and R3 misname them, and the plan carries neither rule. The idle gate is a
different rule: J2's "093-06 idle gate", implemented in J2's harness (`D:\IA_BankOverflow\_r9315\idle.ps1`: last-input
seconds via GetLastInputInfo, GPU % via counters/nvidia-smi, free memory, no editor/game/build processes, disk floors,
a clear streak; 4 h timeout per 093-03..09 requests). Proposed fix: carry W3/W4 as written; chat gives the port its own
idle-gate spec, or allows a copy of that script into port tooling.

**F3 — the shared 5.1 engine tree is outside the P9 record (P9 GAP).** 5.1 is a source build; a new host's Editor target
links against shared engine DLLs in `5E\Binaries\Win64` (measured on earlier scratch hosts: Editor 28 actions / 105 s
because engine DLLs are shared; Game 763 actions because it is monolithic). The J2 hosts use the same engine DLLs. The plan
also enables PythonScriptPlugin/EditorScriptingUtilities in the new hosts, and any engine-module or engine-plugin relink
rewrites shared binaries and their BuildId. Proposed: add `5E\Binaries` and the engine plugins' Binaries to the P9
manifest, and make "0 engine-module actions in every 5.1 port build" a gate (else STOP).

**F6 — P9 proof cost is unsized (P9 GAP).** B5 asks for full SHA-256 of all protected leaves at every job's entry and
exit. Summing this session's pause inventories gives about **253 GiB in about 326,000 files** (staging 11.0, `_r9314` 62.6,
09314 OVERFLOW 58.9, `_r939` 58.0, J2 run folder 7.5, …; the bench bank is not even included). All three volumes are NVMe
(C: and D: share one disk), so a pass is roughly 5–15 min. That is 24 passes over 12 jobs, competing with idle-gated
legs. Proposed: full hash at A1 and A12; per job, path/size/mtime inventories plus full hashes of binaries, configs,
staging key files and git state.

**F4 — one unlisted E: write, measured (P8 GAP).** The 10-05 Target.cs proof build of the 4.25 host wrote 6 files
(4,259,817 B) into `4E\Intermediate\Build\BuildRules\` (`UE4Rules.dll/.pdb/Manifest.json`, `UE4ProgramRules.*`), by
mtime 13:40–14:10. In the same window C: received only `%LOCALAPPDATA%\UnrealEngine\XmlConfigCache-E+EpicGames+UE_4.25.bin`
(190 B; `4E\Source\Programs\UnrealBuildTool\System\XmlConfig.cs:77–83,127–132`) and `%LOCALAPPDATA%\UnrealBuildTool\
Log_UHT.txt` (323 KB; no `-Log=` was passed then). B4 names the C: residuals but not the E: rules assembly. The sizes
support R1's bounded-exception option; the inventory should add the E: item.

**F1 — 4.25 UHT will reject today's reflected headers (sharpens P3/P6).** `4E\Source\Programs\UnrealHeaderTool\Private\
HeaderParser.cpp:3480–3518` accepts `#if` only for `0/1`, `WITH_EDITORONLY_DATA`, `WITH_EDITOR`, `WITH_HOT_RELOAD*` and
`!CPP`, and throws "Unknown define '#if …' in class or global scope" for anything else; `#elif` throws at `:3544`;
`#ifdef`/`#ifndef` pass at `:3534–3537`. All five release reflected headers open with `#if __has_include(...)` at global
scope (`AnomalyInjectorSubsystem.h:4,8`, `AnomalyAutoInjectorSubsystem.h:4,8`, `AnomalySelectorSubsystem.h:4,8`,
`AnomalyCaptureSubsystem.h:4,8,11`, `AnomalyControlServerSubsystem.h:4,8`), and the control header also has
`#if ANOMALY_CONTROL_SERVER` at `:52`. UHT parses Capture/Control headers even at `ANOMALY_*=0`. The plan's
non-reflected `AnomalyStatsIncludes.h` is the right fix, but C1 says "if needed" and the C2 rows for the Injector and
Capture headers don't mention it. It should be mandatory for all five headers, with "no `#if` other than the UHT
allowlist in any reflected header" as an A2 static check.

**F2 — 5.1 Paris copies compile in a different include mode from the release host.** Removing only the 4.25 pins leaves
`BuildSettingsVersion.V2` with no `IncludeOrderVersion`, which resolves to `Oldest` (`5E\Source\Programs\UnrealBuildTool\
Configuration\TargetRules.cs:485`) and adds `[Upgrade]` diagnostics (`:2462–2466`). The release was qualified on StackOBot
with `IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1` (both targets; G17). Not a build error (the property
carries no shared-environment attribute), but it weakens "warnings no worse than the release" and the strict-include
representativeness. Proposed: set `Unreal5_1` in both 5.1 copies.

**F8 — a cheap, real-UBT descriptor proof exists on both engines.** `4E\Source\Programs\UnrealBuildTool\Modes\
JsonExportMode.cs:15` (`-Mode=JsonExport`); PRE-DELIVERY already uses it for the Shipping module list (`:291–293`).
That gives each target/config's actual module set in seconds without compiling, as A4/R2's level 1, instead of a
re-implemented predicate evaluator.

**F7 — descriptor docs go stale.** `docs/PRE-DELIVERY-CHECKLIST.md:291` and `docs/architecture.md:26` quote
`"TargetConfigurationDenyList"` literally; the plan's C4 ledger doesn't list them. `tools/lever_audit.py:333,604` already
accepts both spellings (no change needed; name it in the reach table as verified).

**F9 — job sizing for idle windows.** A4 holds six builds, including two fresh 5.1 Game targets that compile the whole
engine into each host (measured 763 actions / 4,401–5,412 s on earlier scratch hosts), plus strict passes (13–42 min per
target measured). That is about 3–4 h of build latency the plan excludes from estimates. A10/A11 are all-route
idle-gated runtime on baseline and candidate; J2's last job waited ≥19 min in the gate without clearing. Proposed: split
A10/A11 by route family, and schedule the two 5.1 Game prewarms early as background builds.

**F10 — the 5.1 re-gate rebuilds the qualification stack.** A9–A11 recreate the m40 reader, gate/texture fixtures and
pose calibration on a Paris host, without CaptureBench. That is defensible under P9, but it is the largest cost and the
likeliest place for a new-reader defect to "certify" neutrality. Earlier scratch hosts used `git archive` of the
CaptureBench repo, not J2's installed copies. Chat could rule whether that repo at a frozen commit may be used (its
`make_gate_level.py`, `make_texcorrupt_fixture.py`, `check_pose.py`, `run_leg.ps1`). A Paris-based CB_GateLevel still
needs its own pose calibration: `CALIB_BBOX` is one StackOBot target's bbox in pixels (G117).

**Smaller notes.** B1 runs `git worktree add` with `-C` pointed at `_r917` (a P9 worktree). Only common-dir state changes
either way, but addressing the common repo directly reads cleaner. The new opt-in `AnomalyTickAudit` adds a call on every
5.1 tick path; the host probe module could carry it instead. The robocopy `/XD` bare names were checked: Paris has no
nested folders with those names.

## 3. Spot-checks (read here, not taken from the plan)

| # | Claim | Result |
|---|---|---|
| 1 | 4.25 tick loop never calls `IsAllowedToTick` | CONFIRMED `4E\Source\Runtime\Engine\Private\Tickable.cpp:139–145`; `4E\…\Public\Tickable.h` has no such method; tick type read at drain `:41` |
| 2 | 5.1 engine class semantics | CONFIRMED `5E\…\Subsystems\WorldSubsystem.cpp:51–55` (template ⇒ Never), `:57–63` (`!IsTemplate() && bInitialized`, `final` at `.h:63`), `:65–68` checkf, `:70–79` init/deinit flag only (no `UWorldSubsystem` chain, as the plan says) |
| 3 | 5.1 loop calls it | CONFIRMED `5E\…\Tickable.cpp:139–147` (`IsAllowedToTick` at :141; pause/TimeOnly clause identical to 4.25 :145) |
| 4 | Moving world-type restriction is neutral | CONFIRMED: all five overrides return Game‖PIE; the only 5.1 engine caller is `WorldSubsystem.cpp:35` inside `ShouldCreateSubsystem` |
| 5 | 4.25 `ShouldCreateSubsystem`; shim must be Abstract | CONFIRMED `4E\…\Public\Subsystems\Subsystem.h:61`; `SubsystemCollection.cpp:192,204` instantiates every non-Abstract derived class; plan has `UCLASS(Abstract)` |
| 6 | Descriptor keys | CONFIRMED 4.25 C++ `Projects\Private\ModuleDescriptor.cpp:180,185`, UBT `System\ModuleDescriptor.cs:262,280` (legacy only); 5.1 C++ `:215,220`, UBT `:296,314` (modern + fallback); 5.1 fallback is silent (`JsonExtensions.cs:32–35`, `@TODO: Warn here?`), so no new 5.1 warning |
| 7 | 4.25 DDC graphs | CONFIRMED `4E\Config\BaseEngine.ini:1614–1616`, `:1645` (source graph has EnvPathOverride), `:1667–1678` (installed graph Boot/Local under user dirs, Local without EnvPathOverride) |
| 8 | DDC selection and overrides | CONFIRMED `DerivedDataBackends.cpp:66` (`-DDC=`), `:85` (installed default), `:480–505` (env, then command line), `:527–530` (editor setting) |
| 9 | Shader working dir | CONFIRMED `Paths.cpp:290–299`, `:1585` (`ShaderWorkingDir=`), `WindowsPlatformProcess.cpp:1108–1110` (`%TEMP%\UnrealShaderWorkingDir`), `ShaderCompiler.cpp:1851,1865,1267,1370` |
| 10 | `-UserDir`, UAT, CRC, XML cache | CONFIRMED `Paths.cpp:1557`; `CommandEnvironment.cs:22–25`; `WindowsPlatformCrashContext.cpp:461,521,887–890`; `XmlConfig.cs:77–83,127–132` (file exists on C:, 190 B) |
| 11 | C# version symbols / API | CONFIRMED 4.25 `RulesAssembly.cs:204–206` (`UE_4_17..25_OR_LATER` only); 5.1 `:233–239` (adds `UE_5_0/1_OR_LATER`); 4.25 `ModuleRules.cs` has `CppStandard` (:978) and no `GetModuleDirectory(string)` |
| 12 | 4.25 API gaps the Compat layer covers | CONFIRMED no `TArray::IsEmpty` in `Containers\Array.h`; `StaticMesh.h:511` public `TUniquePtr RenderData`; no contact-shadow member in `PrimitiveComponent.h` |
| 13 | Mannequin templates | CONFIRMED 4.25: 26 files / 14,813,474 B; 5.1: 32 files / 41,208,714 B |
| 14 | Paris copy size | CONFIRMED 1,479 files / 2,960,638,517 B (2.757 GiB) without Binaries/Intermediate/Saved; 32 excluded files are 1.292 GiB |
| 15 | J2 merge cost | CONFIRMED merge-base d7eee97; 54 commits; 76 files, +5,944/−297; 21 of J2's 36 Source files overlap the plan's ledger |
| 16 | europa-e1 | CONFIRMED 1f5e305 = 8 files (+295/−7); merge-base with d7eee97 is 489c29d; 415 commits behind |
| 17 | Pointer count | CONFIRMED 9 `TObjectPtr` declarations (Injector 6 incl. the array; Bench 3) |
| 18 | Derived tick/ctor facts | CONFIRMED all five call `Super::Tick`; `FVTableHelper` constructor only in Capture (`AnomalyCaptureSubsystem.cpp:704`) |
| 19 | 4.25 newer-package handling | CONFIRMED `LinkerLoad.cpp:1180–1183, 1211–1213` refuse with warnings, consistent with the plan's C5 STOP rule for mounted 5.1 plugin Content |
| 20 | Free space | CONFIRMED C 11.67–11.74 / D 159.45 / E 57.26–57.27 GiB (11:12 and 12:06) |

## 4. Disk and routing (Step 3 measurements)
- **Now (12:06):** C 11.74 GiB (1.74 above its floor), D 159.45, E 57.26 GiB. C: and D: are partitions of one NVMe
  (Intel SSDPEKNU010TZ); E: is a separate NVMe.
- **4.25 host copy set:** 2.757 GiB (above). The plan's 92 GiB reservation leaves D: about 67 GiB if all retained.
- **Does routing keep 4.25 writes off C: and E:?** Mostly, and by real mechanisms: a project-scoped graph override plus
  `-DDC=` (no Shared node, no editor override), `-ShaderWorkingDir`, process-scoped TEMP/TMP, `-UserDir`, `-Log`/`-abslog`,
  and `uebp_*`. Residuals that remain: C: — XmlConfigCache (190 B), EngineUserDir under `%LOCALAPPDATA%\UnrealEngine\4.25`,
  the CRC monitor and its PID file, GPU/OS caches; E: — UBT's engine rules assembly (4.26 MB, F4). All are small against
  the floors. Stock tools offer no project- or process-scoped route for them, so R1's bounded exception is the honest
  framing.

## 5. 5.1 reach analysis
E6 maps every planned file to gates and requires a real `git diff --name-only` join before the first leg. That is complete
for code. Gaps are environmental, not file-level: F2 (compile mode), F3 (shared engine binaries), F7 (docs). The new tick
audit is correctly mapped (shim row). The descriptor rename also reaches `tools/lever_audit.py`, which is already
alias-aware.

## 6. P9 proof
Yes, the first job can record J2's state: this session's `_reviews\094-01-j2-pause-receipts\` is a usable starting
manifest, and the plan rightly wants it re-done with full hashes, an explicit allowlist and no network. Every job can prove
it unchanged, provided F3 (engine tree) and F6 (cost) are settled. Note for chat: the P9 baseline must start from the
post-pause state, which already differs from 093-15's before-receipts in two unrepaired ways (see the Step 0 notes):
the hosts carry J2's installed candidate, and the 093-14 partial E: archive is gone.

## 7. Job split
12 jobs, one implementer, one bench. Each is plausible for one session except A4 (build latency) and A10/A11
(idle-window size); F9 proposes splits. Owner legs: the office card, the first 4.25 editor launch with one mesh preview
(A6), and the eye check (A7).

## 8. Top 5 risks
1. **5.1 neutrality certified on a brand-new fixture/reader stack** (F10, F2): cost, and false greens from a new reader.
2. **Port builds touching shared 5.1 engine binaries** (F3): J2's resume state changes without any J2 file being edited.
3. **4.25 compile closure** (F1, C5): UHT preprocessor rules, mounted 5.1 plugin Content, header TUs of compiled-out
   modules in the strict pass.
4. **Idle-time scarcity** against the size of idle-gated legs (F9), and the absent port idle-gate spec (F5).
5. **C: margin** of under 2 GiB above its floor on a disk shared with D:, plus small residual C:/E: writes (F4).

VERDICT: REVIEW DONE — 11 OK / 3 GAP (P8, P9, P12) / 0 CONFLICT; 20 spot-checks confirmed; chat rulings needed on R1–R4 plus F3, F5, F6
