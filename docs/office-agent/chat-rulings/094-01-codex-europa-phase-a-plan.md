<!-- Collected by Claude Code (brief 094-01) from relay run folder D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan | model=gpt-6-astra effort=max (run-meta.txt) | duration_s=3069 (11:04:04-11:55:13) | content below is review.md unedited -->
# 094-01 — Europa port: Phase A plan for Chat ruling

**16 October 2026 · independent, read-only review · Chat review REQUIRED before implementation.**

Propose `feat/europa-port` from **d7eee97**, **reimplementing** `1f5e305`. Phase A shares the literal shim, descriptor and pointer/compat changes across engines. On 4.25 it proves **blinking, lighting_mismatch, lod_corruption and bench-only missing_object**, with engine screenshots and restoration. Capture/control/shaders/bench implementations remain disabled; 5.1 must pass the full reach-based regression gates below.

| Decision for Chat | Proposed disposition |
|---|---|
| Hosts | Physical D: `D:\IA_Europa\StylizedParisStreet` (4.25), `D:\IA_Europa\ReGate51` (5.1 candidate), `D:\IA_Europa\Baseline51` (d7eee97 control). Each plugin mount is a worktree. |
| Size | Measured Paris copy **2.757 GiB / 1,479 files**. Three copies/worktrees **8.31 GiB** before outputs; proposed total reservation **92 GiB**. Recheck resolved-volume floors throughout. |
| Existing work | Preserve J2's **currently installed candidate state**. Both existing 5.1 hosts are unsuitable as release baselines. |
| Main ruling | **P8 write-routing hold:** stock UBT/worker/crash-handler ancillary writes are not all redirected to D:. P10 excludes the cache-wrapper route. Chat must resolve this before builds/launches (§B/I). |
| Other rulings | Confirm AA-off zero-edge arbiter, inert reflected shells and operative W3/W4 recipe (§I). |
| Work | **12 jobs**, **37–56 hands-on hours**, plus build/shader/idle latency. **3 owner legs:** office read card, first 4.25 editor/preview, eye check. |

Only `review.md` was written; no implementation, bench or git mutation was performed. J2 evidence directories were not inspected. Measurements below are read-only; future behaviour/costs are predictions, not qualification results.

## A. Base, branch, history, and future merge cost

### A1. Authority and corrections

The binding read order was 094-00 P1–P14; 092-02 and its ruling; 070 and the two europa-e1 audit documents through git objects; then release rules/source. Older instructions about a junction, changing the main checkout, or using the E: host directly are history and **lose to P2/P8/P9**. No such operation is proposed.

The three independent 092-02 checks remain upheld: stock 4.25 has no post-processing-pass SVE subscription; its WebSocketNetworking plugin lacks the public server interface consumed here; and it ignores the modern descriptor allow/deny keys. **P10 and the 092-02 ruling supersede 092-02 §5's preferred XmlConfigCache wrapper**: use both existing Target.cs pins, including `bOverrideBuildEnvironment=true`. The old claim that XML invariably beats a derived Target.cs assignment is not used.

P4 also supersedes a *verbatim* 5.1 tickable clone: 4.25 never calls `IsAllowedToTick`. P7/P11 supersede 070's nine-anomaly/no-pixel E1: the current proof is narrower in features and stronger in evidence. Phase A does not change the 4.25 capture anchor; capture is disabled. The old claim that PostActorTick equals the complete end-world point, or that RDG presence makes current masks trivial, is rejected in accordance with 092-02.

**Gotcha-number collision:** the release's G232–G235 are now about shader-scope counters, exposure and invalid baselines (`docs/gotchas.md:5839,5861,5902,5925`). The Europa UHT/C#/compiler lessons with those numbers are in **europa-e1's** gotchas and 070. Both sets were read; references below identify which is meant. Release G460 (`:10118`) governs defining includes and engine-specific strictness.

### A2. What d7eee97 actually contains

Read-only `rev-parse` returned `d7eee97731812384d4fd00af9032254923363ada`; tracked status produced no change entries. The supplied branch/remote receipt agrees. `d7eee97` itself is the readme limitations/AA-tip commit; production code is that of `23098fa`. That commit changes temporal transition defaults to **On/Off/Hide = 0/0/0** and preserves explicit overrides. Do not silently reintroduce the preceding 3/40/1 default or J1/J2 corrections.

The tree contains five runtime modules, all denied Shipping, with AnomalyBench limited to Game/Editor. It includes m52 hold/proxy, m53 UV/normal corruption, current capture family pairing/change evidence/census, strict-include hardening, and schema 2.1 transition export. There are five UTickableWorldSubsystem subclasses, nine `TObjectPtr` declarations (one is an array), and shader-dependent texture code **inside Injector**, not just Capture. `CLAUDE.md:14–38`, `docs/architecture.md:3–30`, `AnomalyInjector.uplugin:18–55`, and `Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:66–123,242–285` are the relevant base evidence.

The registry has **12 ordinary entries plus two gated bench twins**. That is a source fact, not an instruction to call missing_object a client anomaly. This plan treats missing_object as bench-only for Europa, as requested, and does not change the existing 5.1 registry under the neutrality requirement.

### A3. Reimplement 1f5e305

`1f5e305^` to d7eee97, restricted to the old commit's paths, shows:

| Old patch path | Actual divergence at d7eee97; expected cherry-pick issue |
|---|---|
| `Source/AnomalyCapture/AnomalyCapture.Build.cs` | Parent blob unchanged. Availability/GetModuleDirectory hunk should apply cleanly, but `>=5` violates the new exact-version policy. |
| `Source/AnomalyControlServer/AnomalyControlServer.Build.cs` | Parent blob unchanged. Dependency move should apply; public header leakage remains unfixed. |
| `Source/AnomalyShaders/AnomalyShaders.Build.cs` | Parent blob unchanged. Switch applies, but must use the exact supported version. |
| `Source/AnomalyInjector/AnomalyInjector.Build.cs` | **4 additions/1 deletion** since that parent: Foliage-only dependencies now include RenderCore, RHI, AnomalyShaders. Adjacent insertion/context needs reconciliation. Preserve the 5.1 dependencies; do not install the abandoned `ANOMALY_UE4_TICKABLE_WORLD_SUBSYSTEM` macro. A textual merge may resolve the adjacent hunk; a mechanical cherry-pick is not a design solution. |
| `Source/AnomalyInjector/Private/AnomalyHiddenClass.cpp` | Only one added ActorComponent include since the parent; old contact-shadow hunks should apply with shifted context. Implement through Compat instead of spreading three feature branches. |
| `docs/europa/PHASE1-bench-audit-and-plan.md` | Parent blob unchanged, so old additions apply textually but assert superseded timing/artifact policies. Do not import them as current rules. |
| `docs/gotchas.md` | **4,405 additions/1 deletion** since the parent. Competing end-of-file additions and reused G232–G235 numbers are a documentary/textual conflict. Preserve current history; write a new port note. |
| `docs/sessions/2026-09-03-070-europa-e1-stopped-two-blockers.md` | New history file from the old commit; would add cleanly but carries obsolete compiler/junction statements. No need to import. |

These are **diff-based conflict predictions**, not results of a trial cherry-pick. No merge/index operation was run. Reimplementation is smaller and avoids importing obsolete policy while retaining the shipped availability mechanism.

### A4. J2 merge cost

`git merge-base d7eee97 7754fd2` is **d7eee97731812384d4fd00af9032254923363ada**. The local branch is the requested `7754fd2`, not remote `1bf7054`. The exact requested stat follows; it is repository-object inspection only.

```text
 CLAUDE.md                                          | 233 +++++++++++++
 .../AnomalyCapture/Private/AnomalyCaptureFamily.h  |  48 +++
 .../Private/AnomalyCaptureOwnerState.h             |  39 +++
 .../Private/AnomalyCaptureOwnership.h              | 155 +++++++++
 .../Private/AnomalyCaptureSubsystem.cpp            | 382 ++++++++++++++++++++-
 Source/AnomalyCapture/Private/AnomalyCensus.cpp    |  17 +
 Source/AnomalyCapture/Private/AnomalyCensus.h      |   2 +
 .../Private/AnomalyChangeFamilyData.cpp            |  17 +-
 .../Private/AnomalyChangeFamilyData.h              |   5 +
 .../AnomalyCapture/Private/AnomalyChangeStage.cpp  |  10 +-
 Source/AnomalyCapture/Private/AnomalyChangeStage.h |   7 +-
 .../AnomalyCapture/Private/AnomalyFrameCapturer.h  |   4 +
 .../AnomalyCapture/Private/AnomalyLabelWriter.cpp  |   8 +-
 Source/AnomalyCapture/Private/AnomalyLabelWriter.h |   8 +
 .../Private/AnomalyMaskSceneViewExtension.cpp      | 168 ++++++++-
 .../Private/AnomalyMaskSceneViewExtension.h        |  20 ++
 Source/AnomalyCapture/Private/AnomalyMaskServe.h   |   2 +-
 Source/AnomalyCapture/Private/AnomalyMaskTypes.h   |   2 +
 .../Private/AnomalySceneViewExtension.cpp          |  58 +++-
 .../AnomalyCapture/Private/AnomalyStencilTag.cpp   |  17 +-
 .../AnomalyCapture/Private/AnomalySveCapturer.cpp  | 172 +++++++++-
 Source/AnomalyCapture/Private/AnomalySveCapturer.h |  34 +-
 .../AnomalyCapture/Private/AnomalySveKeyRing.cpp   |   8 +-
 Source/AnomalyCapture/Private/AnomalySveKeyRing.h  |   5 +-
 .../Public/AnomalyCaptureSubsystem.h               |   3 +
 .../Private/Anomalies/Anomaly_CameraClipping.cpp   |  14 +-
 .../Private/Anomalies/Anomaly_CameraClipping.h     |   3 +
 .../Private/Anomalies/Anomaly_TexCorrupt.cpp       |  52 +--
 .../Private/Anomalies/Anomaly_TexCorrupt.h         |   1 +
 .../Private/Anomalies/TexCorruptPure.h             |  11 +-
 .../Private/Anomalies/TexCorruptTree.cpp           |   5 +-
 .../Private/AnomalyInjectorSubsystem.cpp           |  71 +++-
 Source/AnomalyInjector/Private/AnomalyViewport.cpp | 191 +++++------
 .../Public/AnomalyInjectorSubsystem.h              |  12 +
 Source/AnomalyInjector/Public/AnomalyLabelSync.h   |   6 +
 .../AnomalyInjector/Public/AnomalyNearClipSlab.h   |  81 ++---
 Source/AnomalyInjector/Public/AnomalyViewport.h    |  18 +-
 docs/architecture.md                               | 222 +++++++++++-
 docs/client-readme.md                              |  56 ++-
 docs/gotchas.md                                    | 338 ++++++++++++++++++
 docs/predictions/093-06-j2-capture-ownership.md    |  83 +++++
 docs/predictions/093-07-engine-projection.md       | 121 +++++++
 docs/predictions/093-08-targeted-admission.md      |  59 ++++
 docs/predictions/093-09-fov-drop-accounting.md     |  82 +++++
 docs/predictions/093-11-pairing-recipe.md          |  89 +++++
 docs/predictions/093-12-tsr-half-resolution.md     |  93 +++++
 docs/predictions/093-13-exposure-exclusion.md      |  32 ++
 docs/predictions/093-14-camera-donor-full-pie.md   | 145 ++++++++
 .../093-15-tsr-served-receipts-staged.md           |  90 +++++
 docs/predictions/093-natural-foundation-miss.md    |  47 +++
 docs/sessions/2026-10-04-091-05-review-fixes.md    |  58 ++++
 .../2026-10-13-093-03-followup-capture-base.md     | 134 ++++++++
 ...-10-13-093-04-followup-capture-qualification.md |  86 +++++
 ...-10-14-093-05-followup-capture-qualification.md | 139 ++++++++
 .../2026-10-14-093-06-j2-capture-ownership.md      | 227 ++++++++++++
 .../2026-10-14-093-07-engine-projection.md         | 250 ++++++++++++++
 .../2026-10-14-093-08-targeted-admission.md        | 174 ++++++++++
 ...-10-15-093-09-fov-accounting-requalification.md | 216 ++++++++++++
 ...-10-15-093-10-crash-recovery-requalification.md | 246 +++++++++++++
 ...026-10-15-093-11-pairing-recipe-continuation.md | 149 ++++++++
 .../2026-10-15-093-12-tsr-pairing-continuation.md  | 184 ++++++++++
 .../2026-10-16-093-13-exposure-exclusion-design.md |  71 ++++
 .../2026-10-16-093-13-exposure-exclusion.md        |  77 +++++
 .../2026-10-16-093-14-camera-donor-full-pie.md     | 116 +++++++
 ...2026-10-16-093-15-tsr-served-receipts-staged.md |  40 +++
 docs/setup-runbook.md                              |  20 ++
 tools/camera_clipping_slab_selftest.cpp            |  29 +-
 tools/capture_ownership_selftest.cpp               | 110 ++++++
 tools/mask_serve_selftest.cpp                      |   3 +-
 tools/office_checkpoint_i.md                       |  91 +++++
 tools/office_checkpoint_i.py                       | 332 ++++++++++++++++++
 tools/stencil_scope_selftest.cpp                   |  18 +
 tools/test_annotation_transitions.py               |  20 ++
 tools/test_office_checkpoint_i.py                  |  78 +++++
 tools/texcorrupt_uniform_selftest.cpp              |  10 +
 tools/verify_capture.py                            |  17 +-
 76 files changed, 5944 insertions(+), 297 deletions(-)
```

Phase A's planned source overlap is:

- Capture: `Private/AnomalyCaptureSubsystem.cpp`, `Private/AnomalyCensus.cpp`, `Private/AnomalyChangeFamilyData.cpp`, `Private/AnomalyChangeStage.cpp`, `Private/AnomalyFrameCapturer.h`, `Private/AnomalyLabelWriter.cpp`, `Private/AnomalyLabelWriter.h`, `Private/AnomalyMaskSceneViewExtension.cpp`, `Private/AnomalySceneViewExtension.cpp`, `Private/AnomalyStencilTag.cpp`, `Private/AnomalySveCapturer.cpp`, `Private/AnomalySveKeyRing.cpp`, `Public/AnomalyCaptureSubsystem.h`.
- Injector: `Private/Anomalies/Anomaly_CameraClipping.cpp`, `Private/Anomalies/Anomaly_CameraClipping.h`, `Private/Anomalies/Anomaly_TexCorrupt.cpp`, `Private/Anomalies/Anomaly_TexCorrupt.h`, `Private/Anomalies/TexCorruptTree.cpp`, `Private/AnomalyInjectorSubsystem.cpp`, `Private/AnomalyViewport.cpp`, `Public/AnomalyInjectorSubsystem.h`.
- Documentation: `CLAUDE.md` and `docs/architecture.md` if updated as proposed. New port docs/tools have distinct names. Existing `verify_capture.py`, `AnomalyLabelSync.h`, `AnomalyNearClipSlab.h`, `AnomalyViewport.h`, TexCorruptPure, ownership/family headers and J2 office tools are **not planned Phase A edits**.

That is **21 source files plus 2 documentation files** with planned same-file overlap, not 23 guaranteed textual conflicts. Most capture overlap is a guard/include envelope; the serious semantic merge sites are capture lifecycle, Injector admission/registration, viewport projection and texture teardown. A future J2 merge must preserve its newly added files within the same 4.25-disabled dependency closure. Budget a separate merge/reach review and repeat affected gates after J2 is qualified; neither branch may inherit the other's green status by ancestry alone.

## B. Hosts, checkouts, write routing, disk, and P9

### B1. Exact layout and creation proposal

| Role | Project | Plugin worktree |
|---|---|---|
| 4.25 port | `D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject` | `D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector`, branch `feat/europa-port` |
| 5.1 candidate | `D:\IA_Europa\ReGate51\StylizedParisStreet.uproject` | `D:\IA_Europa\ReGate51\Plugins\AnomalyInjector`, detached at the **same committed port SHA** |
| 5.1 release control | `D:\IA_Europa\Baseline51\StylizedParisStreet.uproject` | `D:\IA_Europa\Baseline51\Plugins\AnomalyInjector`, detached at d7eee97 |
| Evidence and generated output | `D:\IA_Europa\Evidence\<job>\<leg>`, `D:\IA_Europa\Packages\{UE425,UE51Base,UE51Port}` | No existing bank or staging path used. |
| Caches | `D:\IA_Europa\Caches\UE425`, `D:\IA_Europa\Caches\UE51` | Separate engine versions; 5.1 baseline/candidate may share only this new cache, sequentially. |

The 5.1 hosts are fresh copies of the **allowed Paris source**, not clones of either J2 host. Forward conversion happens only in the new 5.1 copies. Retain the project's existing target/module names so commands below are exact; remove the 2017 compiler/SDK/override assignments in the two **5.1-only copied** Target.cs files and let the existing 5.1 toolchain selection act. Keep the pins byte-identical in the 4.25 copy. Use explicit engine roots, not association-driven shell launching. No change to any global XML or engine registration.

Future commands, **not executed in this job**; create destinations only after resolving every ancestor and proving physical D:, and refuse pre-existing nonempty destinations:

```powershell
$repo = 'E:\IA_BuildCache\_r917\AnomalyInjector'
$src = 'E:\Unreal Projects\StylizedParisStreet'
New-Item -ItemType Directory -Path 'D:\IA_Europa\Receipts' -Force
robocopy $src 'D:\IA_Europa\StylizedParisStreet' /E /XJ /COPY:DAT /DCOPY:DAT /XD Binaries Intermediate Saved Plugins .vs /R:1 /W:1 /LOG:D:\IA_Europa\Receipts\copy425.log
robocopy $src 'D:\IA_Europa\ReGate51' /E /XJ /COPY:DAT /DCOPY:DAT /XD Binaries Intermediate Saved Plugins .vs /R:1 /W:1 /LOG:D:\IA_Europa\Receipts\copy51.log
robocopy $src 'D:\IA_Europa\Baseline51' /E /XJ /COPY:DAT /DCOPY:DAT /XD Binaries Intermediate Saved Plugins .vs /R:1 /W:1 /LOG:D:\IA_Europa\Receipts\copy51base.log
git -c safe.directory=* -C $repo worktree add -b feat/europa-port 'D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector' d7eee97731812384d4fd00af9032254923363ada
git -c safe.directory=* -C $repo worktree add --detach 'D:\IA_Europa\Baseline51\Plugins\AnomalyInjector' d7eee97731812384d4fd00af9032254923363ada
git -c safe.directory=* -C $repo worktree add --detach 'D:\IA_Europa\ReGate51\Plugins\AnomalyInjector' feat/europa-port
```

Inspect each exit immediately: robocopy codes 0–7 require inventory verification; >=8 fails. No `/MIR`, source deletion or junction. Only use command-local safe.directory where ownership requires it. The 5.1 candidate advances by `git switch --detach <frozen-port-sha>` **inside its own clean worktree**, between jobs, with old products/evidence identified separately. Never use `--force` to attach the same branch twice. No fetch, remote update, push, tag, or merge is required for A.

A new host-only probe module and fixture scripts are installed identically into baseline/candidate hosts where applicable (§C). No fixture is taken from a J2 content directory. Configure a simple local spectator/game mode: the original DefaultEngine.ini references a ThirdPersonBP game mode absent from its Content tree. Explicitly choose `/Game/ParisStreet/Levels/SampleMap` for the visual Paris leg; the source defaults to **AssetsOverview**, not SampleMap.

### B2. Measured size and reservation

Measured at about 11:09–11:12 IST by file lengths, without following any new copy or launching Unreal:

| Input | Bytes / count | Meaning |
|---|---:|---|
| Paris excluding Binaries/Intermediate/Saved | 2,960,638,517 / 1,479 files | 2.757 GiB; includes source art as well as Unreal packages. |
| Paris Content | 2,960,507,507 / 1,453 files | 881 uassets and 2 maps; remaining files include FBX/texture source art. |
| Paris Config | 36,168 / 18 files | Config plus bundled JSON metadata. |
| Paris Source | 1,603 / 5 files | Two targets and the small runtime module. |
| d7eee97 tracked plugin blobs | 11,216,637 / 468 files | About 10.70 MiB per checkout before generated products; read by `git ls-tree -rl`. Working-tree newline/cluster costs are additional. |
| Free physical volumes, `shutil.disk_usage` | C: 12,532,531,200; D: 171,209,748,480; E: 61,486,989,312 bytes | Approximately **11.67 / 159.45 / 57.26 GiB**. Get-PSDrive returned null free sizes in the reviewer sandbox, so it was not used as a measurement. |

Reservation, **estimates, not measured build sizes**: copies/worktrees 8.31 GiB; 4.25 build/strict output 8; two 5.1 build sets 24; DDC/shader caches 24; staged packages 12; fixtures 2; pictures/evidence 8; transient compiler/UAT/temp space 5. Total **about 92 GiB**, predicting D: near 67 GiB if all are retained. No engine copy is planned. Cache growth, shader debug output, PDBs and cooking can exceed these allowances; estimates never override a floor. Record actual allocation after each job and revise the next job's reservation before starting it.

### B3. DDC and shader routing from the actual 4.25 source

Source aliases here: `4E = E:\EpicGames\UE_4.25\Engine`; engine source citations are relative to `4E\Source`.

- `4E/Config/BaseEngine.ini:1614–1616` distinguishes source and installed DDC graphs and documents `-DDC=<section>`.
- The installed graph at `:1667–1678` writes Boot to `%ENGINEUSERDIR%DerivedDataCache/Boot.ddc` and Local to `%ENGINEVERSIONAGNOSTICUSERDIR%DerivedDataCache`. **Its Local node has no EnvPathOverride**, unlike the source graph at `:1645`.
- `Developer/DerivedDataCache/Private/DerivedDataBackends.cpp:66–91` reads a named graph from **GEngineIni** and falls back to the installed/default graph when invalid. `:480–505` reads Path, environment, then command-line override; `:527–543` can override again from EditorSettings. A local environment variable alone is therefore neither sufficient nor authoritative.

Proposed project-only `Config/DefaultEngine.ini` additions in the **D: 4.25 copy**:

```ini
[Europa425DDC]
Root=(Type=KeyLength, Length=120, Inner=AsyncPut)
AsyncPut=(Type=AsyncPut, Inner=Hierarchy)
Hierarchy=(Type=Hierarchical, Inner=Boot, Inner=Local)
Boot=(Type=Boot, Filename="D:/IA_Europa/Caches/UE425/Boot.ddc", MaxCacheSize=512)
Local=(Type=FileSystem, ReadOnly=false, Clean=false, Flush=false, PurgeTransient=true, DeleteUnused=true, UnusedFileAge=34, FoldersToClean=-1, Path="D:/IA_Europa/Caches/UE425/Local")

[InstalledDerivedDataBackendGraph]
Root=(Type=KeyLength, Length=120, Inner=AsyncPut)
AsyncPut=(Type=AsyncPut, Inner=Hierarchy)
Hierarchy=(Type=Hierarchical, Inner=Boot, Inner=Local)
Boot=(Type=Boot, Filename="D:/IA_Europa/Caches/UE425/Boot.ddc", MaxCacheSize=512)
Local=(Type=FileSystem, ReadOnly=false, Clean=false, Flush=false, PurgeTransient=true, DeleteUnused=true, UnusedFileAge=34, FoldersToClean=-1, Path="D:/IA_Europa/Caches/UE425/Local")
```

No Shared node is reachable. No EditorOverrideSetting or EnvPathOverride is included. Overriding the installed graph as well prevents the known default fallback from quietly selecting C:. Verify merged effective entries, not just the source text. Use `-DDC=Europa425DDC` for editor/cook invocations; a fallback warning still fails the routing gate. This costs a cold cache but avoids unknown user overrides. Use a separately named, similarly scoped graph for the 5.1 copies.

The engine launch wrapper, process-local only, sets `TEMP` and `TMP` to `D:\IA_Europa\Temp\<job>` and passes:

```text
-UserDir=D:/IA_Europa/StylizedParisStreet/
-ShaderWorkingDir=D:/IA_Europa/Caches/UE425/ShaderWorking/
-DDC=Europa425DDC
-abslog=D:/IA_Europa/Evidence/<job>/<leg>/Editor425.log
-AbsCrashReportClientLog=D:/IA_Europa/Evidence/<job>/<leg>/CrashClient425.log
```

For a packaged Game use a distinct `-UserDir=D:/IA_Europa/Runtime/UE425/<leg>/` and log name. The wrapper creates its own paths, uses explicit argument arrays, and restores its own process environment in `finally`; it never calls SetEnvironmentVariable at User/Machine scope. The owner launches through that wrapper too. UAT's cook child must receive the same engine arguments via its supported additional-cooker-options route and prove them in its actual child command line.

`Runtime/Core/Private/Misc/Paths.cpp:245–260,274–299,1549–1599` implements UserDir and ShaderWorkingDir. `Runtime/Core/Private/Windows/WindowsPlatformProcess.cpp:975–990,1108–1110` otherwise uses Windows TEMP for shader working space. `Runtime/Engine/Private/ShaderCompiler/ShaderCompiler.cpp:1850–1867` creates GUID work directories and Saved/ShaderDebugInfo; `:1267–1316,1370–1417,2053–2084` passes the work directory to ShaderCompileWorker. DDC and this temporary shader traffic are separate mechanisms.

Persistent shader-related files are also covered: `Runtime/RHI/Private/PipelineFileCache.cpp:1425,1432,2682` places writable `.upipelinecache` and `CollectedPSOs` under **ProjectSavedDir**; `Runtime/RenderCore/Private/ShaderCodeLibrary.cpp:1013,1098,1151,1198,1350,1391,1715` places cook shader intermediates/libraries/debug data under **ProjectSavedDir/Shaders**. They therefore follow the D: project/UserDir route, independently of DDC. GPU-driver caches are the separate residual in B4.

For UAT, scope `uebp_EngineSavedFolder=D:\IA_Europa\Temp\<job>\UAT\Saved`, `uebp_LogFolder=D:\IA_Europa\Evidence\<job>\UATWorking` and `uebp_FinalLogFolder=D:\IA_Europa\Evidence\<job>\UATFinal` to the process. `Programs/AutomationTool/AutomationUtils/CommandEnvironment.cs:22–25,88–123` reads these overrides. Its `:112–115,272–289` clears the working log folder, so each invocation gets a **new dedicated directory**, never an evidence root containing another leg. Confirm the child logs and cooker command line; an archive directory alone is not a log redirect.

### B4. Write-destination ledger — and the unresolved P8 issue

| Producer | Planned D: destination / evidence | Residual or condition |
|---|---|---|
| Editor/project | D: host Saved (logs, screenshots, config, autosave, profiling), Intermediate, host Content only for explicitly authored fixtures | UserDir selects ProjectUserDir, not every Windows known folder. Project source/art remains unchanged unless the fixture recipe explicitly writes its new destination. |
| Packaged Game | `D:\IA_Europa\Runtime\UE425\<leg>\Saved` plus explicit log | Engine screenshots under Saved/Screenshots/Windows unless an explicit absolute screenshot filename is supplied. Cooked outputs/packages stay under D: host/Packages. |
| DDC and material/global shader derived data | D: graph above, Boot and Local | Check both effective graph and observed file traffic; named-graph failure is not allowed to become a green fallback. Plugin global shaders register **zero** permutations on 4.25. Host/engine shaders still compile. |
| ShaderCompileWorker | GUID job inputs/outputs under D: ShaderWorking; debug data under D: project Saved; inherited D: TEMP/TMP | Worker executable is read from E:. Its own logging/crash policy must be observed; stock launcher does not include its program source in this install. Parent UserDir is **not automatically forwarded** by `LaunchWorker`; subprocess command line starts with Multiprocess (`Core/Private/Misc/CommandLine.cpp`). Do not assert all worker writes are redirected. |
| UBT, compiler, linker, UHT | Host/plugin Binaries and Intermediate on D:, explicit `-Log=D:/.../UBT425.txt`, compiler TEMP on D:. UHT gets an adjacent absolute log (`Programs/UnrealBuildTool/System/ExternalExecution.cs:1160–1166`). | **UBT XML cache remains a possible C: write**: installed-engine `XmlConfig.cs:77–83,127–132` uses LocalAppData/UnrealEngine/XmlConfigCache-E+EpicGames+UE_4.25.bin and refreshes it. `Utils.cs:1000–1021` uses .NET KnownFolder, not the TEMP/APPDATA environment string. Target pins do not relocate that cache. |
| UAT/cooker | New D: build/stage/archive directories; process TEMP; scoped UAT Saved/working/final log variables from B3; explicit child logs and cooker arguments | Verify the actual child command lines in setup. No UAT default log under the engine/user tree is accepted merely because the archive directory is D:. |
| Crash report payload | A normal in-process report uses ProjectSavedDir/Crashes (`WindowsPlatformCrashContext.cpp:816`; `GenericPlatformCrashContext.cpp:1132–1146`). Explicit CRC log is forwarded at `WindowsPlatformCrashContext.cpp:461–464,887–891`; monitor PID temp file uses UserTempDir (`:521`). | The out-of-process monitor is started during static initialization (`:450–484,1106–1110`) and does **not forward UserDir**. Its own Saved/config/recovery data, and early/worker failures, are not proven D:-only. CRC/known-folder and OS crash writes can remain on C:. |
| Engine-global settings / driver / OS | No manual global change proposed | `WindowsPlatformProcess.cpp:995–1007` gets LocalAppData via SHGetKnownFolderPath. `Paths.cpp:146–162` puts EngineUserDir there. GPU driver caches, WER and OS paging are outside the project redirect. `-nowrite` suppresses FConfigFile writes (`ConfigCacheIni.cpp:1478–1480`), not arbitrary filesystem writes. |

**Ruling request R1:** interpret P8 as requiring all controllable bulk project/DDC/shader/build/evidence writes on D:, with explicitly inventoried stock-tool/CRC/OS ancillary C: exceptions and the 10 GiB floor; or retain its literal wording and require a separate process-isolation design before any build/launch. The proposed ordinary stock commands cannot honestly satisfy literal “every cache/log/build write on D:” alongside P10. I do **not** propose changing KnownFolders, registry, global XML, engine source, or resurrecting the toolchain cache wrapper. If Chat retains the literal requirement, job A1 ends **STOP/needs revised routing**, not “setup passed.” The rest of the port plan remains reviewable now.

### B5. Disk floors and P9 proof

Measure **resolved volume free bytes** before host creation; after each copy; before and after every build/strict pass/cook; before first editor shader compilation; before every editor/game leg; periodically during shader compilation/cook/long runs; and at job exit. Open an existing ancestor handle and resolve all reparse points (e.g. GetFinalPathNameByHandle plus GetDiskFreeSpaceEx), including mounted-volume junctions. Do not decide from a drive-letter prefix or a one-level LinkType check. Use C: >=10 GiB, D:/E: >=50 GiB and a per-job projected-space reserve. If resolution/free-space measurement fails, it is unknown, not zero or sufficient.

**STOP on a floor breach or insufficient projected reserve:** do not start another operation; end only the port's owned runtime cleanly and retain logs. Do not delete J2 banks, restore J2 binaries, kill foreign processes, or move protected data to recover space. A monitor warning before the floor (suggest D/E 55 GiB, C 11 GiB) permits controlled shutdown; these are warning margins, not relaxed floors. E: stays monitored despite new output being D:, because J2 aliases/staging resolve there. C: has only about 1.67 GiB above its floor now.

First implementation job creates `D:\IA_Europa\Receipts\P9\baseline` and records the actual current protected state, comparing it to the supplied 11:00 receipt. Reuse the **logic**, not execute the existing `pause-baseline.ps1` blindly: it hardcodes writes into `_reviews`, contains broad BuildCache enumeration, short eight-character hashes and a network ref query. Adapt it to an explicit P9 allowlist and new D: output, full SHA-256, no network, no unlisted J2 paths, and error-fatal enumeration. Missing/denied input is a failed proof.

Derive the frozen bank/archive-root allowlist from the named pause receipts and P9 paths; do not discover it by scanning the BuildCache parent. If an archive's identity is absent from those receipts, ask the orchestrator for that exact root before including it. Use `GIT_OPTIONAL_LOCKS=0` on protected worktree status reads, hash indexes before/after the read, and never refresh or repair them.

The baseline includes protected worktree HEAD, symbolic branch, index hash, tracked status/diff and untracked inventory; main checkout m51 and its Config; complete host Config/Content/plugin-folder and binary inventories (including Review0936/CaptureBench and J2's extra fixture content); resolved live Builds inventory plus exe/DLL/pak/ucas/utoc hashes; and manifests of the **P9-named** evidence/bank/archive roots, run folder and `_r9315`. Protected leaf contents must be hashed, not merely timestamps. C3's D8D30951/DFE6F164 are identifiers for the installed candidate, not permission to reinstall the release. Full hashes, not these prefixes, bind subsequent comparisons. Enumerate roots without following unexpected junctions; separately record approved targets. This job itself did not inventory these protected roots.

Every later job compares against the same immutable baseline at **entry and exit**, including jobs ending early or failing. A resumed interrupted job first proves the previous state. Store compare results and changed-path lists in the new receipts. A known protected change is a STOP requiring reconciliation by the orchestrator, never an automatic restore. A later job does not silently adopt a changed baseline.

New `feat/europa-port` refs, objects and its three worktree-registration records are the explicitly expected changes to the common git repository; compare the **protected worktree/ref subset** exactly, and account for new port records separately. This avoids the impossible demand that `git worktree list` be byte-identical after adding a worktree. The original E: Paris source gets its own manifest before/after copying and remains untouched throughout.

## C. Phase A file-by-file implementation

### C1. Shared shim contract

Add `Source/AnomalyInjector/Public/Compat/AnomalyTickableWorldSubsystem.h` with the unconditional literal declaration:

```cpp
UCLASS(Abstract)
class ANOMALYINJECTOR_API UAnomalyTickableWorldSubsystem
    : public UWorldSubsystem, public FTickableGameObject
{
    GENERATED_BODY()
    // Literal declarations; implementation and engine selection live in .cpp.
};
```

It exposes Initialize/Deinitialize/Tick/GetTickableGameObjectWorld/GetTickableTickType/IsTickable/IsInitialized/ShouldCreateSubsystem, the engine-style `GetStatId` declaration with `PURE_VIRTUAL(..., return TStatId();)` fallback (so UHT-generated construction remains legal), and a virtual `IsAllowedToTick() const final`. The latter can be declared **without the `override` keyword**: it overrides the 5.1 virtual and is an ordinary new virtual on 4.25. No version-switched base, UCLASS, UPROPERTY or UFUNCTION. Header-only optional include logic is moved to a **non-reflected** include helper if needed by 4.25 UHT; no reliance on 5.1 UHT silently skipping an unknown condition.

| Semantics | 4.25 | 5.1 |
|---|---|---|
| Registration | One FTickableGameObject base registers once; never register another ticker/delegate to emulate its tick. | Same one registration. |
| Tick type | Template/CDO: Never; other instances: Conditional. Invariant across Initialize/Deinitialize; never return Never just because not initialized. | Exactly engine `IsTemplate() ? Never : FTickableGameObject::GetTickableTickType()` (Conditional today). |
| `IsTickable` | `!IsTemplate() && bInitialized` (with normal base eligibility if needed). This is the live lifetime gate; Always would bypass it. | Preserve FTickableGameObject's current true result; initialized/CDO gating belongs in IsAllowedToTick. |
| `IsAllowedToTick` | Same defined result for probes, but **not called by stock 4.25's tick loop**. | Exactly `!IsTemplate() && bInitialized`, final, as engine class. |
| World | GetTickableGameObjectWorld returns GetWorld. No null-world fallback or global tick. | Identical. |
| Initialize/deinitialize | Initially false; Initialize asserts false then sets true; Deinitialize asserts true then sets false. Preserve each derived class's existing Super call placement. | Match `5/Runtime/Engine/Private/Subsystems/WorldSubsystem.cpp:70–79` exactly. That engine implementation does **not** itself call UWorldSubsystem::Initialize/Deinitialize; do not invent a new chain and call it a clone. Derived-to-shim chaining is retained. |
| Tick | Assert initialized at Super::Tick; no added game clock or frame-step semantics. | Same check as engine `:65–68`; retain derived GetStatId implementations and STATGROUP_Tickables identities. |
| Pause/editor | Inherit false IsTickableWhenPaused and IsTickableInEditor. Eligible unpaused world updates tick once; paused/TimeOnly do not acquire a new ticking path. | Same. |

Source proof: 4.25 Tickable.h:70,80,112–125 and Tickable.cpp:38–45,83–102,139; 5.1 WorldSubsystem.h:53–76, WorldSubsystem.cpp:51–79 and Tickable.cpp:139–150. All five current derived classes call Super::Tick and supply their own stat identity; the implementation audit rechecks this before editing. No derived class may switch tick type to Always on 4.25 or bypass the gate. The shim's constructors, including the FVTableHelper route used by Capture, must initialize its state and register only once.

`ShouldCreateSubsystem(Outer)` on the shim calls **Super first**, preserves false, validates/casts Outer to UWorld, and returns Game or PIE only. Remove the five DoesSupportWorldType overrides. On 5.1 Super still calls the engine's default DoesSupportWorldType (Game/Editor/PIE), then the shim restricts it; on 4.25 Super is the USubsystem decision. Do not reject based on the receiver being a CDO: this virtual is deliberately queried on the CDO. CDO tick exclusion and world creation eligibility are different questions.

### C2. Effect notation and exact source ledger

**N = changed but meant to be neutral on 5.1. C = changed on 5.1 in the stated tooling/host/diagnostic surface. B = byte-identical retained file.** N never promises identical binaries: reflection ancestry, layout and recompilation change. A guard that leaves the enabled algorithm's tokens intact still receives N and a gate. Every row names concrete files; grouped rows apply the stated operation to **each named file**. Paths below are relative to the release plugin root; `I`, `Cpt`, `Ctl`, `S`, `Bch` abbreviate `Source/AnomalyInjector`, `Source/AnomalyCapture`, `Source/AnomalyControlServer`, `Source/AnomalyShaders`, `Source/AnomalyBench`.

| File(s) | Planned change and reason | 5.1 |
|---|---|---|
| `AnomalyInjector.uplugin` | Rename TargetAllowList to **WhitelistTargets** and all five TargetConfigurationDenyList fields to **BlacklistTargetConfigurations**. Keep module order/types/loading phases, content capability and WebSocket dependency unchanged. No EngineVersion pin and no claim of version-specific descriptor support. | N |
| `I/Public/Compat/AnomalyTickableWorldSubsystem.h` **new**; `I/Private/Compat/AnomalyTickableWorldSubsystem.cpp` **new** | Implement C1, export class across modules; include World, Tickable, SubsystemCollection and defining stats headers at use sites. | N |
| `I/Public/Compat/AnomalyEngineVersion.h` **new** | Non-reflected exact `ENGINE_MAJOR_VERSION==4 && ENGINE_MINOR_VERSION==25` and `==5 && ==1` predicates, capability constants and explicit unsupported-version diagnostic. No macro reflected parents. | N; support promise is only 4.25/5.1 |
| `I/Public/Compat/AnomalyEngineCompat.h` **new**; `I/Private/Compat/AnomalyEngineCompat.cpp` **new** | Non-reflected adapters: skinned asset identity (SkeletalMesh vs GetSkinnedAsset); static RenderData; cached camera POV/global-vs-override near clip; contact-shadow get/set; container empty helper if needed. Keep exact 5.1 operations in its branch. Include defining 4.25 SkeletalMesh/EngineTypes/CameraTypes/StaticMeshResources/PrimitiveComponent headers here. | N |
| `I/Public/Compat/AnomalyStatsIncludes.h` **new** | Non-reflected home for version/optional stats include differences, used by reflected headers without putting unrecognized feature blocks into their UHT parse. Retain existing 5.1 defining includes. | N |
| `I/Public/AnomalyInjectorSubsystem.h` | Literal shim parent/include; remove world-type override; replace **five** TObjectPtr fields with explicitly null-initialized raw pointers and `TArray<TObjectPtr<UObject>>` with `UPROPERTY(Transient) TArray<UObject*>`. Keep property names/specifiers. No weak-pointer substitution. | N |
| `I/Public/AnomalyAutoInjectorSubsystem.h`; `I/Public/AnomalySelectorSubsystem.h` | Literal shim parent/include; remove DoesSupportWorldType declarations; legal stats includes. Preserve NumPoolKeys=9 and input storage shape. | N |
| `Cpt/Public/AnomalyCaptureSubsystem.h` | Literal shim parent/include; remove DoesSupportWorldType; retain existing out-of-line destructor and FVTableHelper constructor. Ordinary portable method/field declarations remain unconditional. | N |
| `Ctl/Public/AnomalyControlServerSubsystem.h` | Literal shim parent/include; remove DoesSupportWorldType and unconditional IWebSocketServer include. Move transport ownership behind unconditional forward-declared `FAnomalyControlTransportState` and TUniquePtr; out-of-line normal/FVTableHelper constructors and destructor. No `#if ANOMALY_CONTROL_SERVER` in the reflected class. | N; ownership/layout changed |
| `Ctl/Private/AnomalyControlTransportState.h` **new** | Non-reflected transport state, FControlConn/Server storage, complete 5.1 interface inside existing availability guard; inert complete state on 4.25. Preserve callback and socket lifetimes. | N |
| `I/Private/AnomalyInjectorSubsystem.cpp` | Remove world override; keep Super lifecycle/stat calls. Version/capability boundary around constructor asset finders, implementation includes and registry entries. 4.25 loads **no 5.1-authored plugin asset**; fields stay null. A's registry exposes three ordinary proof types plus missing_object only with bench flag. Keep entire 5.1 registry/CDO paths. Move the two concrete stuck-mip restoration queries (`:957,967`) behind portable capability dispatch, avoiding links to omitted classes. | N |
| `I/Private/AnomalyAutoInjectorSubsystem.cpp` | Remove world override. 4.25 capability filter at pool initialization/toggle/admission boundaries, with explicit unsupported response; preserve nine key slots and exact 5.1 arrays, random draw order and defaults. Do not make an unsupported ID consume a successful draw. | N |
| `I/Private/AnomalySelectorSubsystem.cpp` | Remove world override; filter selectable IDs against 4.25 capabilities so absent material/advanced entries are not presented as working. Preserve exact 5.1 choices/order. | N |
| `I/Private/Compat/AnomalyCapabilities.cpp` **new**; `I/Public/Compat/AnomalyCapabilities.h` **new** | Single non-reflected Phase-A capability policy for registry/UI/admission. 5.1 admits the existing catalogue unchanged. 4.25 supported set is explicit and logged; unknown/unported requests fail closed. | N |
| `I/Private/AnomalyInjectorModule.cpp` | On exact 4.25 register an explicit **stock-no-Nanite** component probe and remove it on shutdown; 5.1 Capture remains the probe owner. Log scoped capability profile. Missing probe must still refuse, so do not globally set AllowNanite=1. | N |
| `I/Private/AnomalyHiddenClass.cpp` | Route three bCastContactShadow accesses through Compat. 4.25 returns a declared unsupported/default saved value and performs no nonexistent member write; all other hide/restore flags stay. Public saved struct may remain unchanged. | N |
| `I/Private/AnomalyLod.cpp` | GetRenderData via Compat: 4.25 `Mesh->RenderData.Get()` (TUniquePtr owner), 5.1 current getter. Preserve forced-LOD conventions (0 automatic, setter 1-based, reported level 0-based), screen-size computation and skeletal APIs. | N |
| `I/Private/AnomalyViewport.cpp` | Replace SkinnedAsset-only include/identity with Compat; include EngineTypes on 4.25 for FHitResult and HitResult.h on 5.1 at the use site. Camera access adapters retain the 5.1 cache/override/refusal path; 4.25 uses GetCameraCachePOV and GNearClippingPlane with no per-camera override. No new projection algorithm or J2 fix imported. | N |
| `I/AnomalyInjector.Build.cs` | Exact supported versions; retain the existing Core/CoreUObject/Engine/InputCore/Foliage/RenderCore/RHI/AnomalyShaders dependency set on both engines initially. All named engine modules exist in 4.25, and AnomalyShaders is a stub there; removing a dependency is unnecessary to compile out its implementations and risks core helper/include exports. Explicit Cpp17 on 4.25 only. Publish capability defines to dependent modules where required. | N |
| `Cpt/AnomalyCapture.Build.cs` | Existing ANOMALY_CAPTURE is 1 only for exact 5.1 non-Shipping, 0 for 4.25. Promote Injector to public dependency because the public reflected base now lives there. Gate rendering/editor dependencies behind enabled capture. Preserve 5.1 tick-fork probe and defines. Guard the **C# symbol reference** to GetModuleDirectory with `#if UE_5_1_OR_LATER && !UE_5_2_OR_LATER` (or use shared Path.Combine API with identical resolved path); runtime if alone is insufficient. | N |
| `Ctl/AnomalyControlServer.Build.cs` | Existing ANOMALY_CONTROL_SERVER is 0 on 4.25; WebSocketNetworking only in enabled branch. Public Injector dependency for reflected base; preserve Capture/Json dependencies as needed by the enabled implementation. Cpp17 for 4.25. | N |
| `S/AnomalyShaders.Build.cs` | Existing ANOMALY_SHADERS=0 on 4.25; only minimal stub dependency Core there; exact 5.1 non-Shipping path keeps existing dependencies. Cpp17 for 4.25. | N |
| `Bch/AnomalyBench.Build.cs` | New ANOMALY_BENCH=0 on 4.25, 1 on exact 5.1 non-Shipping Game/Editor. Core/UObject/Engine only for inert shell on 4.25; existing Injector/Capture dependency set on 5.1. Cpp17 for 4.25. | N |
| `Bch/Private/AnomalyBenchModule.cpp` | Wrap fixture implementation **including UE5-only includes**, with a minimal module entry outside guard and explicit compiled-out log on 4.25. Register no bench commands there. | N |
| `Bch/Private/AnomalyBenchTwoPartActor.h` | Convert Root/PartEligible/PartIneligible to raw UPROPERTY pointers initialized nullptr, preserving metadata and literal UCLASS. | N |
| `Bch/Private/AnomalyBenchTwoPartActor.cpp` | Guard component creation with ANOMALY_BENCH; 4.25 constructor is inert. Reflection shell remains linkable but no functional fixture is registered. | N |
| `S/Private/AnomalyShadersModule.cpp` | Keep IMPLEMENT_MODULE outside guard; add an explicit compiled-out diagnostic in the 0 branch. No source-directory mapping or shader registration on 4.25. | N |
| `Ctl/Private/AnomalyControlServerSubsystem.cpp` | Remove world override, implement transport-state access/lifetime, keep enabled algorithm/callback order; define disabled Start/Stop/status behaviour and complete type destruction. No socket/listener on 4.25. | N |
| `Ctl/Private/ControlProtocol.cpp`; `Ctl/Private/ControlSnapshot.cpp` | Put implementation/includes behind ANOMALY_CONTROL_SERVER; any required public disabled-call facade returns an explicit unavailable result. Do not leave active snapshot/encoding work behind the disabled listener. | N |
| `Cpt/Private/AnomalyCaptureSubsystem.cpp` | Remove world override; retain all existing ANOMALY_CAPTURE=0 public-method fallbacks. Move RHI/Slate/advanced includes inside availability guard. Complete Async/PreviewTee/RunLog types where constructors and destructor instantiate deletes, including the FVTableHelper path. No 4.25 OnWorldTickEnd substitution in A. | N |
| `Cpt/Private/AnomalyFrameCapturer.h`; `Cpt/Private/AnomalyLabelWriter.h` | Move renderer/PixelFormat and unavailable dependency includes inside ANOMALY_CAPTURE so their independently generated strict-header TUs are legal at 0. Keep portable declarations needed by the shell. | N |
| `Cpt/Private/AnomalyPreviewTee.h` | Provide complete inert class in the disabled branch for TUniquePtr destruction; enabled layout/body remains. | N |

The following **18 Capture implementation files each get the same include-boundary audit/edit**: keep their enabled bodies, place unavailable implementation/includes inside the existing `ANOMALY_CAPTURE` path, and leave only link-required disabled facades. This is not a second renderer backend:

| File | Specific reason / disabled completion | 5.1 |
|---|---|---|
| `Cpt/Private/AnomalyAsyncWriter.cpp` | Async writer cannot emit capture artifacts at 0. | N |
| `Cpt/Private/AnomalyCaptureLetterbox.cpp` | No capture viewport callbacks/cvars at 0. | N |
| `Cpt/Private/AnomalyCensus.cpp` | Early RHI includes must be inside guard; no census provider at 0. | N |
| `Cpt/Private/AnomalyChangeFamilyData.cpp` | No 4.25 ISceneViewFamilyExtentionData dependency. | N |
| `Cpt/Private/AnomalyChangeMeasurement.cpp` | No event/window measurement writer at 0; containers remain enabled-only. | N |
| `Cpt/Private/AnomalyChangeStage.cpp` | Early scene/RHI dependency closure. | N |
| `Cpt/Private/AnomalyFrameCapturer.cpp` | Early RHI/access/readback includes; no backend at 0. | N |
| `Cpt/Private/AnomalyLabelWriter.cpp` | No label/annotation writer at 0; header self-containment. | N |
| `Cpt/Private/AnomalyMaskMeasure.cpp` | Disabled interface closure; no mask work. | N |
| `Cpt/Private/AnomalyMaskSceneViewExtension.cpp` | Early RDG/RHI and private-renderer includes must not be parsed on 4.25. | N |
| `Cpt/Private/AnomalyMeasurability.cpp` | Early shader-platform includes and Nanite implementation stay enabled-only. | N |
| `Cpt/Private/AnomalyPreviewCapture.cpp` | Currently not wholly guarded. At 0 exported preview/encode functions clear outputs and return false; no viewport read or encoding. | N |
| `Cpt/Private/AnomalyPreviewTee.cpp` | No capturer acquisition/tee callbacks; match complete disabled type. | N |
| `Cpt/Private/AnomalyRunLog.cpp` | Currently portable but unguarded. At 0 constructor/destructor stay linkable and writes/registration are inert. Keep its portable class declaration. | N |
| `Cpt/Private/AnomalySceneViewExtension.cpp` | No newer SVE/RDG includes outside guard. | N |
| `Cpt/Private/AnomalyStencilTag.cpp` | No capture tag ownership service at 0. Injector hide flags remain independent. | N |
| `Cpt/Private/AnomalySveCapturer.cpp` | Early texture/readback includes inside guard; no GPU receipt service. | N |
| `Cpt/Private/AnomalySveKeyRing.cpp` | No capture-family publication/service at 0. | N |

For simple already-complete guards, an audit may find no edit necessary; then mark that path B and remove it from the **actual** change/reach manifest before freezing. Do not add churn solely to make the planned file count true. Conversely, any newly discovered compile fix is added to that manifest and reach analysis before runtime legs.

### C3. Injector sources that must genuinely stop compiling on 4.25

UBT discovers `.cpp` files regardless of whether Initialize registers their anomaly. Place a whole implementation boundary **before unavailable includes** in each below, using the exact-version capability from the non-reflected compat header. No UE5 expression is parsed in the disabled 4.25 TU. Public ordinary contracts remain portable and unsupported calls fail closed.

| File(s), each individually in scope | Change | 5.1 |
|---|---|---|
| `I/Private/Anomalies/Anomaly_StuckLowMip.cpp`; `I/Private/Anomalies/Anomaly_TexCorrupt.cpp`; `I/Private/Anomalies/TexCorruptBench.cpp`; `I/Private/Anomalies/TexCorruptDraw.cpp`; `I/Private/Anomalies/TexCorruptState.cpp`; `I/Private/Anomalies/TexCorruptTree.cpp`; `I/Private/Anomalies/TexCorruptUniform.cpp` | Disable the complete renderer/readiness/streaming implementations on 4.25. In particular this excludes FTSTicker, StreamableRenderAsset, shader-map completeness, RDG stats/readbacks, texture getters and SkinnedAsset factories. These are Phase E work, not Phase A adapters. | N |
| `I/Private/Anomalies/TexCorruptCore.h`; `I/Private/Anomalies/Anomaly_TexCorrupt.h`; `I/Private/Anomalies/Anomaly_StuckLowMip.h` | Guard non-reflected backend declarations/includes for strict-header TUs. Do not expose their concrete types to common call sites. | N |
| `I/Private/Compat/AnomalyUnsupportedTexture.cpp` **new** | Define the portable exported facade on 4.25: IsTexCorruptId still recognizes the IDs; eligibility returns false with `unsupported_engine_phase_a`; modes empty, warm/measurement work unavailable, restore queries false only because no instance can have been created. Supply every referenced public symbol in AnomalyTexCorrupt/AnomalyStuckMipStats; no false “applied/ready/measured” result. Compile no replacement definitions on 5.1. | N |
| `I/Private/Anomalies/Anomaly_TimeDilation.cpp`; `I/Private/Anomalies/Anomaly_LodPopping.cpp`; `I/Private/Anomalies/Anomaly_CameraClipping.cpp`; `I/Private/Anomalies/Anomaly_MissingTexture.cpp`; `I/Private/Anomalies/Anomaly_CorruptedTexture.cpp`; `I/Private/Anomalies/Anomaly_ChangeCase.cpp` | Implementation boundaries on 4.25 for features outside A or requiring assets. No registration/direct reference on 4.25. Enabled 5.1 body retained. The TArray `Args.IsEmpty()` in ChangeCase is either in that disabled body or uses `Num()==0`; do not substitute FString::IsEmpty indiscriminately. | N |
| `I/Private/Anomalies/Anomaly_CameraClipping.h` | Guard any non-reflected unavailable declaration/include needed by independent header TUs; body remains 5.1-only in A. | N |

`AnomalyStuckMipRoute.cpp`, ordinary public statistics/policy headers and TexCorruptPure remain portable unless the strict build proves otherwise. **Do not “port” E by replacing readiness with HasValidGameThreadShaderMap or inventing a row pitch.** The simple hide, blinking, light and LOD implementations remain intact; their only required port mechanics are shared helper/include changes above.

C++ choice: **Cpp17 explicitly for 4.25 plugin modules**, using the enum already present in 4.25 UBT; 5.1's existing Default choice remains unchanged (Cpp17). Validate `/std:c++17` with the actual 14.16 compiler response files. This does not repair missing UE APIs. Both entire C# rule files must compile under both UBTs; runtime `if` protects values, not missing symbol references (Europa G233). No new C# language feature beyond the older compiler is needed.

### C4. Tools, host-only files, and unchanged surfaces

| File(s) | Planned work and reason | 5.1 |
|---|---|---|
| `tools/strict_include_build.py` | Version/root/path adaptation and actual compile-action coverage, §D. | C, verification tool |
| `tools/check_package_descriptor.py` | Canonicalize the explicitly supported legacy/modern module-filter key aliases for semantic comparison; preserve raw descriptor hash, all field/value restrictions, dependency/fixture checks and unknown-field strictness. Reject contradictory dual spellings. Extend its selftest with equivalent aliases, dropped restrictions and contradictory aliases. Keep existing `--unrealpak` support. Needed because 5.1 writers can normalize legacy input to modern keys. | C, verification tool |
| `tools/europa_phase_a_setup.ps1` **new** | Explicit creation recipe, copy checks, own-host target/config setup, floor/allowlist validation; no global settings or automatic protected cleanup. | C, host setup |
| `tools/europa_phase_a_run.ps1` **new** | Scoped environment/arguments, process ownership, idle gate, foreground sampling, timeouts and final P9 comparison. Implementation stays held on R1. | C, harness |
| `tools/europa_phase_a_inventory.py` **new** | Resolved-path/free-space and P9 manifests; output only new port receipts; hashing errors fatal. | C, evidence tool |
| `tools/europa_phase_a_gate.py` **new** | Picture/state/tick readers and synthetic corrupted-receipt controls; no gate threshold tuning after results. | C, evidence tool |
| `tools/europa_phase_a_descriptor.py` **new** | Evaluate actual UBT/runtime module predicates for target/config matrix and compare actual output/receipt evidence to release; engine argument explicit. Supplements, does not replace G354. | C, evidence tool |
| `tools/europa_phase_a_fixtures.py` **new** | Build fresh local fixtures from engine/Paris assets and deterministic generated 5.1 texture assets, plus camera and dynamic light. 4.25 A creates no plugin-authored material. Read actual LOD count at runtime. | C, host content |
| `I/Public/Compat/AnomalyTickAudit.h`; `I/Private/Compat/AnomalyTickAudit.cpp` **new** | Opt-in bench-gated, non-reflected lifecycle/tick audit called by the shim; record world/object/frame/init epochs, duplicates, before-init/after-deinit, once at shutdown. No ticking/delegate ordering change. Closed by default and excluded from Shipping. | C when enabled; N when disabled; speed measured disabled |
| `docs/predictions/094-europa-phase-a.md` **new** | Frozen predictions/control matrix before first build/runtime leg (§E). | C, documentation |
| `docs/europa/PHASE-A-425-PLAN.md`; `docs/europa/PHASE-A-RESULTS.md` **new** | Approved plan and subsequent actual change/reach/gate inventory. | C, documentation |
| `docs/architecture.md`; `CLAUDE.md` | Port profile, supported-version limits, state and references. No claim of 4.25 capture or delivery readiness. | C, documentation |
| Each D: host's `StylizedParisStreet.uproject`, `Config/DefaultEngine.ini`, `Config/DefaultGame.ini` | Explicit probe-module declarations, local fixture/game-mode/map/cook selections and DDC roots. Add EditorScriptingUtilities/PythonScriptPlugin only in these new hosts for authoring. Exclude plugin 5.1 Content from the 4.25 A cook, including cook-all/always-cook paths. | C, isolated hosts |
| `ReGate51/Source/StylizedParisStreet.Target.cs`, `StylizedParisStreetEditor.Target.cs`; same two under `Baseline51` | Remove only 4.25 pins/override from copies; paired target settings otherwise identical. 4.25 two targets remain byte-identical. | C, isolated hosts |
| Each host `Source/EuropaPhaseAProbe/EuropaPhaseAProbe.Build.cs`, `Public/EuropaPhaseAWorldProbe.h`, `Private/EuropaPhaseAWorldProbe.cpp`, `Private/EuropaPhaseAProbeModule.cpp`, `Private/EuropaPhaseAProbeDriver.cpp` **new** | Host-only runtime/editor probe, separately denied Shipping: lifecycle observer, independent frame denominator, positive editor-preview sentinel, controlled screenshot/apply/revert/pause/travel/GC driver. Optional shim audit access only when available; baseline must also compile. Does not reenable AnomalyBench on 4.25. | C, isolated probe |
| New host `Content/EuropaPhaseA/EA_Core.umap`, `EA_Travel.umap`; 5.1-only `Content/CaptureBenchGate/CB_GateLevel.umap` and `Content/CaptureBenchTexCorrupt/CB_TexCorruptLevel.umap` plus a recipe-enumerated fixture asset manifest | Authored later by the script, in new hosts only. Static models/materials reference engine/Paris assets on 4.25. 5.1 generated fixtures cover all delivered routes and known-bad controls; names satisfy existing bench-map gates. Freeze the exact generated list/hash before a leg. | C, isolated fixtures |

For the 5.1 skeletal/motion fixture, the permitted source **`D:\UESource\UnrealEngine\Templates\TemplateResources\High\Mannequin\Content` exists: 32 files, 41,208,714 bytes**. Its SK_Mannequin references `/Game/Mannequin/...`. Propose an exact manifest copy into **each new 5.1 host's `Content/Mannequin`** (about 78.60 MiB combined, inside the 2 GiB fixture reservation), then use one skeletal actor in the fixture. This is engine-template content, not a J2 asset. Preserve the mount-relative paths; verify all dependencies and actual render LOD count. If another LOD is needed, duplicate the mesh as `Content/EuropaPhaseA/SK_EA_Lod.uasset` and use the local skeletal reduction API, testing its result and vertex-count difference before pixels. Source template assets are never resaved. Every copied relative filename joins the host manifest; both 5.1 hosts get the identical frozen result. 4.25 A does not need a skeletal fixture.

The source ledger names every presently proposed plugin edit/addition. Generated host fixture files get their own manifest because their exact count cannot be established before authoring. Unexpected new production-file reach returns to Chat if it changes this design or a ruled behaviour; ordinary include fixes are recorded and gated.

**B/retained:** all plugin `.usf` shaders, the three shader class/registration pairs (already under ANOMALY_SHADERS), original plugin Content binaries, ordinary hide/blink/light/LOD anomaly algorithms, capture timing/label policies, schema, pure policy math and existing pixel checkers. The package checker's only proposed semantic extension is the alias handling above; its engine executable stays an explicit argument. No 5.1 asset is overwritten with a 4.25 save. No global BuildConfiguration.xml or engine source change.

### C5. Ancestry and closure audits

The release Source scan found **exactly the five declarations** referring to UTickableWorldSubsystem, and no GetSubsystemArray/IsChildOf consumer of that base. This is a bounded repository finding, not a claim about office code or binary assets. Before runtime: scan all tracked source/tools and each new host for Cast/IsA/IsChildOf/StaticClass/GetDerivedClasses/GetSubsystemArray and serialized class-path assumptions; inspect Blueprint dependencies in the fresh host with the asset registry. Concrete `GetSubsystem<UAnomaly…>()` users stay valid. Casts to **UTickableWorldSubsystem will no longer match on 5.1**. Do not add misleading redirects from an engine class to the plugin shim. Office numeric ancestry check is in §G; an actual external consumer is a compatibility issue for Chat, not silently repaired by reparenting someone else's code.

Build closure audit must show: UHT sees the same literal class/property declarations; all nine raw properties retain GC traversal; every compiled-out translation unit has portable includes; every exported/common referenced symbol links; no disabled 5.1 asset is loaded by a 4.25 CDO; no texture shader entry survives; and public module dependencies support isolated generated TUs. Specifically test Capture's three TUniquePtr destruction paths and Control's new opaque transport in modular Editor builds, not just a monolithic Game link (G221/G425).

“No 5.1 asset load” means no UObject package load or cook of those five original plugin assets. The editor asset registry may still inspect mounted package headers; cook exclusion alone is **not** proof of scan exclusion. Record any newer-package scan warning separately, and never browse/load those thumbnails in the 4.25 fixture. If the stock registry/cook cannot tolerate the mounted incompatible packages, stop and return a content-isolation design to Chat rather than changing shared descriptor content support or resaving the files. B must close this content-packaging boundary before office delivery.

The **four Phase A smoke anomalies** need no plugin material: missing_object and blinking use hide/restore state; lighting_mismatch uses existing light properties; lod_corruption uses existing mesh LODs. Missing_object remains a bench test. Add a dynamic local light if the Paris lights are baked, and use an existing multi-source-model Paris mesh whose runtime LOD/pixel difference is verified. Missing_texture and corrupted_texture wait for B's native material assets. The lighting/LOD tests remain explicit console tests even if not in the auto UI pool; the seeded auto smoke uses blinking.

## D. Builds and the strict-include tool

All commands here are **future commands in new port hosts**, conditional on Chat's ruling, P9 preflight, sufficient space and the R1 routing disposition. Run one build at a time, in a wrapper that saves the complete command, environment overrides, start/end time, exit code, warning counts and output hashes. A PowerShell transcript alone is not an exit-code check. Builds do not require the owner to leave the machine; editor, game and rendering/cooking commandlets do.

### D1. Exact normal build commands

The engine-root paths are explicit. The 4.25 Target.cs assignments already read, in **both** targets, VisualStudio2017 / `14.16.27051` / `10.0.17763.0` / `bOverrideBuildEnvironment=true`; no compiler command-line override or global XML edit is added.

```powershell
& 'E:\EpicGames\UE_4.25\Engine\Build\BatchFiles\Build.bat' StylizedParisStreetEditor Win64 Development '-Project=D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build425\Editor.txt'
& 'E:\EpicGames\UE_4.25\Engine\Build\BatchFiles\Build.bat' StylizedParisStreet Win64 Development '-Project=D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build425\Game.txt'
& 'E:\EpicGames\UE_4.25\Engine\Build\BatchFiles\Build.bat' StylizedParisStreet Win64 Shipping '-Project=D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build425\Shipping.txt'

& 'D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat' StylizedParisStreetEditor Win64 Development '-Project=D:\IA_Europa\Baseline51\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build51Base\Editor.txt'
& 'D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat' StylizedParisStreet Win64 Development '-Project=D:\IA_Europa\Baseline51\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build51Base\Game.txt'
& 'D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat' StylizedParisStreet Win64 Shipping '-Project=D:\IA_Europa\Baseline51\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build51Base\Shipping.txt'

& 'D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat' StylizedParisStreetEditor Win64 Development '-Project=D:\IA_Europa\ReGate51\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build51Port\Editor.txt'
& 'D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat' StylizedParisStreet Win64 Development '-Project=D:\IA_Europa\ReGate51\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build51Port\Game.txt'
& 'D:\UESource\UnrealEngine\Engine\Build\BatchFiles\Build.bat' StylizedParisStreet Win64 Shipping '-Project=D:\IA_Europa\ReGate51\StylizedParisStreet.uproject' -WaitMutex -NoHotReloadFromIDE '-Log=D:\IA_Europa\Evidence\Build51Port\Shipping.txt'
```

Each line runs only after the preceding result is accepted; create log directories first. These cover the release's Editor/Game production build surfaces plus the explicit Shipping exclusion proof. Client/Server filtering is also evaluated with the real descriptor predicates, but the Paris project has no existing Client/Server targets: do not report either as a built target. If a delivery requires one, add its target/build to the future delivery job.

The first UBT invocation on each engine must **actually compile rules and a plugin TU**. Save the toolchain-selection log and compiler response/command paths. Read the selected `cl.exe` file version and SDK include/lib paths; assert:

- 4.25: compiler **product version 14.16.27051**, VS2017, Windows SDK **10.0.17763.0**. A containing MSVC directory named 14.16.27023 is not by itself a failure: product version and UBT's selected-version reading decide (092-02 ruling).
- 5.1 baseline and candidate: **14.38.33130**, same selected compiler executable and SDK as each other. Do not force 5.1 to 2017 to get a superficially common build.
- Project pins, global BuildConfiguration.xml hash, and engine Build.version hashes are recorded before/after. No XML content is changed. Count all warnings; compare 5.1 by warning code/location/category, not just a total that can hide a new warning behind a removed one.

### D2. Adaptation of `tools/strict_include_build.py`

The current tool already accepts `--engine-root`, but its fallback/sentinel and response discovery assume 5.1 (`:16,192–204,249–254`). The proposed change makes that argument **required**, reads Engine/Build/Build.version, accepts only 4.25/5.1 and validates that the uproject, plugin and all temporary files resolve inside one of the three new hosts. It refuses any protected/J2/main checkout, junction out of the host, stale injected block, dirty rule file, or pre-existing generated strict directory.

For 4.25, inject the supported unity/PCH-disabling settings and **no IncludeOrderVersion token anywhere in the generated C#**. Use an engine-neutral comment sentinel instead of searching for `EngineIncludeOrderVersion.Latest`. For 5.1 retain the existing latest-include-order strict setting. Use `UE4Editor` versus `UnrealEditor` when locating module compile outputs, and account for the actual architecture subdirectory from UBT output instead of assuming all versions have the same directory layout. Game responses are checked against the Game target's path too.

The internal `run_build` must pass an explicit **`-Log=<per-build-log>.ubt.log`** to UBT and `-NoHotReloadFromIDE`; `--log-dir` currently redirects only the Python stdout copy and does not relocate UBT's own default log. Use different filenames for the two writers. Inherit only the approved process-scoped TEMP/UAT environment, and save/restore it.

Generate one independent TU per plugin header, including private headers and reflected headers with their generated include dependencies. Verify UHT-generated `.gen.cpp` compilation as well as ordinary source files. A pass requires actual non-unity compile actions and per-file responses with no PCH use, not just previously existing response files or “up to date.” Newly added Compat and disabled-header paths join the coverage denominator. A zero-TU/zero-action pass fails. Maintain finally-based exact restoration of all rule files, removal of only the tool's own generated directory, matching pre/post tracked status, and a successful **normal rebuild** after restoration. Do not use `--no-header-check` or `--no-normal-rebuild` for qualification.

```powershell
python 'D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\tools\strict_include_build.py' --uproject 'D:\IA_Europa\StylizedParisStreet\StylizedParisStreet.uproject' --engine-root 'E:\EpicGames\UE_4.25' --targets StylizedParisStreetEditor StylizedParisStreet --log-dir 'D:\IA_Europa\Evidence\Strict425'
python 'D:\IA_Europa\ReGate51\Plugins\AnomalyInjector\tools\strict_include_build.py' --uproject 'D:\IA_Europa\ReGate51\StylizedParisStreet.uproject' --engine-root 'D:\UESource\UnrealEngine' --targets StylizedParisStreetEditor StylizedParisStreet --log-dir 'D:\IA_Europa\Evidence\Strict51'
```

The adapted runner may read baseline compile products, but never patches baseline d7eee97 rule files or the release worktree to run a comparison. Both qualifying strict runs operate on the port's own source. A missing defining include is repaired at its use site with the appropriate version's defining header, then both engines' strict checks are rerun for that change. G460 is precisely why a green 5.1 strict run cannot substitute for 4.25 strictness.

The strict can-fail is a disposable, port-owned source mutant that omits a genuinely necessary defining include from a chosen isolated TU and produces the predicted compile error, plus a reader test that marks a response `/Yu` or unity and must reject it. That intentional **compile-failure test** is distinct from a runtime negative-control binary: the latter must build successfully, boot and exercise its mutant, or P12 requires STOP.

## E. Gates, reach, pictures, staging, and frozen predictions

### E1. Evidence and common admission rules

Every row below is a proposed gate with a predicate and a demonstrated failure path. **S** means static/build-only (no idle requirement); **P** PIE/editor; **G** packaged Game. Every P/G leg, first shader compilation, and editor-based fixture/cook commandlet requires the existing **W3/W4 idle gate**. Builds alone may run while the owner uses the PC. This plan does not invent a new idle duration: Chat should attach/identify the operative W3/W4 recipe before scheduling a runtime job (R3). Log foreground PID and sampling coverage continuously for measurement legs; losing the gate invalidates the leg.

Do not run two hosts simultaneously. D2 allows at most **two harness-repair attempts per problem**, not indefinite changes of fixture, threshold or negative control; planned repetitions in a frozen protocol are not repair attempts. Unbuildable or ineffective runtime controls STOP the affected claim. Zero counted events, zero expected worlds, an absent preview world, no actual compiled TU, and a gate whose “negative” does not reach the tested code are **unqualified**, never green.

Evidence binds to the final source SHA, host configuration/fixture manifest, engine version, compiler, actual exe/DLL hashes and all effective cvars. Keep mutant binaries/receipts separately. Restore the final source, rebuild it, verify hashes and repeat all gates reached by a subsequent change (G201/K7). A pre-mutant or pre-fix run cannot qualify a newly built final binary by name alone. Default-off audit logging is disabled for speed/pixel regression legs; audit-on lifecycle and audit-off regression are separate configurations of the same declared implementation.

### E2. 4.25 gates

| ID / predicate | Can-fail required | Context / idle / owner |
|---|---|---|
| **A0 Environment**: P9 manifests equal; D/E >=50 GiB and C >=10 GiB; every proposed write resolves to the approved roots or an explicitly ruled R1 exception; actual DDC graph/worker arguments match §B. | Feed the inventory checker a one-byte changed protected-file receipt, a junction resolving to E:, and a below-floor free-space reading; all reject. Validate the graph reader against an intentionally wrong graph configuration in an isolated config input. Do not deliberately cause a prohibited C: write to test the alarm. Actual child/write tracing then confirms the real leg. | S before/after every job; P/G routing observation requires idle. Owner only for the first launch. |
| **A1 Build/toolchain**: Editor + Game Development and Game Shipping exit 0, zero errors; warning counts recorded and all new port warnings resolved or individually presented to Chat; actual 14.16/SDK selection as §D. Both entire C# rule sets compile. | Parser rejects a nonzero exit, one added error, unexpected compiler version or missing compile command. A controlled rules-version mismatch is rejected before scheduling a build; no toolchain fallback accepted. | S; no owner. |
| **A2 Strict**: both target strict builds and normal restoration rebuild exit 0; header/UHT/source coverage complete; unity and PCH actually off; pre/post source hashes match. | Missing-include and response mutants in §D; the expected compile failure is confined to this test. | S; no owner. |
| **A3 Load/compile-out**: editor and Development package boot normally; core plus module stubs start once, explicitly log capture/control/shaders/bench compiled out; generated module defines are 0; no capture start, listener, shader registration or AnomalyBench fixture command. Positive core command/list/visible apply must work. Injector's explicitly gated diagnostic levers remain available for the declared controls. | Reader rejects an omitted startup/define receipt. A buildable module-startup mutant omits one compiled-out diagnostic and must fail the completeness check; a core admission mutant returns unsupported for blinking and must fail the positive smoke. No attempt to compile unavailable capture as the control. | P and G; idle; first P launch by owner. |
| **A4 Descriptor**: old-key predicates work; Shipping excludes all five IA modules from UBT actions, receipts and new build/package outputs; non-Shipping Game/Editor contain the expected stubs/core. No stale output mistaken for inclusion/exclusion. | Descriptor mutant removes one Shipping blacklist and the **real engine descriptor predicate** must now select that module and fail the matrix. Actual package checker also rejects a known omitted/extra module artifact. | S build/predicate plus G package/load; cook/run idle; no additional owner action. |
| **A5 Worlds/lifecycle**: all five concrete subsystem shells exist exactly once per Game/PIE world, none in observed Editor/EditorPreview worlds. CDO tick count=0. No double registration. Each receives exactly one tick per eligible world update, zero before Initialize/after Deinitialize; pause has zero, resume restores cadence; two PIE worlds remain isolated; travel and three PIE end/restart cycles balance initialize/deinitialize. | Buildable ShouldCreate mutant admits EditorPreview and is detected by a **positive preview sentinel**. Separately bypass the 4.25 initialized gate and probe an uninitialized/deinitialized object; violation count >0. A duplicate audit/call mutant must be detected, not hidden by a set that deduplicates records. | P with two PIE worlds, pause/resume, travel and preview; G with pause/travel/exit. All idle. **Owner opens the first 4.25 editor and a mesh preview once**; probe must actually observe both non-game world types. |
| **A6 Reflected GC**: five material fields/strong-reference array retain transient objects through forced GC; after deliberate release, weak observers expire. Component pointers retain valid owned components on the 5.1 bench twin; 4.25 inert shell remains constructible. | Buildable removal of the strong-reference array's UPROPERTY in a disposable mutant loses a transient object and fails. Checker also rejects a fabricated “alive” receipt with no GC cycle recorded. | P and G for core references; idle; no owner. |
| **A7 Four smoke types**: at least **3 completed visible apply/revert events per anomaly**, each with before/during/after engine screenshots; attempted=applied+refused; applied>=3 and reverted=applied; state residue=0, pixel restoration meets E3. Blinking includes visible and hidden subframes. Invalid target and unsupported ID are refused with no state change. | Host-driver NoApply control still records a request but omits execution: the active-picture predicate fails. A buildable SkipRestore mutant leaves a changed hide/light/LOD state: state and restoration gates fail. Both test inputs are explicitly labelled controls. | P and G; idle; **owner eye check** of the four real effects and return state, once after numeric gates. Missing_object is bench-only. |
| **A8 Admission/auto/teardown**: unsupported material/advanced requests cannot enter pools; registry is 3 ordinary + bench-only missing_object under the flag; same seed 4242 gives the same nonempty blinking target/event sequence twice; no shader/material package load from plugin 5.1 Content; no residue after stop/travel/teardown. | A capability mutant admits an unsupported ID and fails the exact registry/pool assertion; a known forced second target changes a recorded selection; SkipRestore control fails teardown. | P and G; idle; no extra owner. |

Tick denominator: independently record **world update epoch + world identity**, ELevelTick, pause flag, lifecycle interval and process frame number. Two PIE worlds may share a process frame; counting once per process frame would be wrong. Exclude documented TimeOnly/paused updates from the eligible denominator, but count and report them. Capture/control shells have no functional work on 4.25, yet their FTickableGameObject entry/lifetime semantics must still be observable. The shim records the entry before feature early-returns. CDO and dormant instance probes are created/held by the host harness, not by re-enabling AnomalyBench.

### E3. Picture evidence without AnomalyCapture

Use the engine's `FScreenshotRequest::RequestScreenshot` with absolute D: PNG paths, one outstanding request at a time, plus completion acknowledgement from the engine screenshot delegate. Include the request/world/update ID and actual completion ID in the host probe receipt. Do not call capture/preview APIs from the disabled module. No HighResShot tiling, resized output, or UI-inclusive frame is accepted for comparison.

Freeze one camera/viewport at **1280x720, 100% screen percentage, dynamic resolution off, AA off, fixed exposure, no motion blur, no overlays**, with static background and a deliberate movable light. On 4.25 use the engine's actual AA control (`r.PostProcessAAQuality=0` and the fixture's post-process settings), not a possibly nonexistent 5.1 `r.AntiAliasingMethod` cvar. Wait for shader/streaming readiness, then take 20 clean frames to qualify stability. This is a picture/state smoke, **not a frame-accurate label or mask claim**.

Per anomaly freeze a visible target and ROI before activation. Hide/light shadow ROIs include the expected shadow footprint; LOD uses a mesh/camera for which at least two real render LODs produce a visible change. Define N as the maximum consecutive clean-frame ROI mean absolute RGB difference (0..255 scale). Require **N <=0.25**, otherwise the fixture is not qualified. During each effect:

- active ROI mean difference from its clean reference **>=4.0 and >=4N**;
- at least **1% of ROI pixels** have maximum-channel absolute difference >=8;
- after revert, ROI mean difference **<=max(0.5, 2N)** and <=0.1% of pixels differ by >=8; verify a second settled after-frame;
- recorded hide/shadow flags, light intensity/color/visibility/shadow setting, and forced-LOD values equal the pre-event snapshot exactly. Original object/material identities must also match where applicable.

These are proposed frozen thresholds, aligned with the release's >=4/4x-null visibility rule; Chat approves before measurement. If a legitimate effect is too subtle, select a stronger fixed fixture **before** the qualifying leg and refreeze its prediction, not a looser threshold after seeing a result. NoApply and SkipRestore must fail these exact readers. Save PNGs, absolute-difference PNGs, metrics CSV/JSON and contact sheets in the new evidence root. The owner checks ordinary images of all four effects plus their return state; numeric evidence cannot stand in for that leg.

### E4. 5.1 gates

“Matches release” means a **fresh d7eee97 build in Baseline51 using the same fixture/configuration and engine/compiler**, not J2's installed binaries or an old claim of green. A baseline that fails its own required predicate is a blocker/known limitation for Chat; it is not permission to lower a candidate threshold.

| ID / predicate | Can-fail required | Context / idle / owner |
|---|---|---|
| **R0 Environment**: A0/P9/floors and own host/output roots; baseline/candidate fixtures/configs equal except declared engine/profile metadata. | Same corrupted inventory/floor/path inputs as A0; an unequal fixture hash is rejected. | S plus runtime observation when needed; no owner. |
| **R1 Builds/toolset/defines**: release Editor/Game Development builds and Shipping exclusion build succeed; warnings no worse by category/location than d7eee97; strict passes and restores; 14.38.33130 unchanged; **generated** E1-G1b defines show ANOMALY_CAPTURE=1, ANOMALY_CONTROL_SERVER=1, ANOMALY_SHADERS=1 and enabled bench profile in non-Shipping. Tick-pin compile setting matches baseline (stock source expected off), not forced on. | Build/checker controls from A1/A2; deliberately wrong 0 define and wrong compiler reading fail the evidence parser. An enabled control run must actually produce capture frames and answer the server probe, so textual defines alone cannot pass. | S for builds/strict; runtime positives in R5/R8 require idle. |
| **R2 Descriptor**: actual 4.25 and 5.1 descriptor readers accept legacy keys; 5.1 module sets equal d7eee97 for every valid target/config combination. Development Game/Editor =5; non-Shipping Client/Server =4 (no bench); Program =0 for these Runtime modules; Shipping =0 IA modules. Preserve dependency list, loading phases and runtime types. Compare actual built/staged receipts as well. | Real-engine matrix with removed blacklist/whitelist must differ and fail. Package checker selftest plus wrong-module extracted descriptor fail. | S matrix/build; G Development and Shipping package proof, idle for cook/run. |
| **R3 Worlds/ticks/GC**: A5/A6 semantics on the 5.1 candidate, same native tick/CDO/initialized/world behaviour documented in C1; concrete subsystem access unchanged; no duplicate updates; pause, two PIE worlds, travel, editor exclusion, PIE-end clean. GC references and bench actor components survive as intended. | Buildable IsAllowedToTick=true mutant exposes dormant lifetime violation; ShouldCreate-all-worlds and removed-UPROPERTY mutants fail their respective readers. A positive native 5.1 world observation prevents an empty-world pass. | P and G; idle. No extra owner: 5.1 preview automation may create the observed preview world. |
| **R4 Tick order/m40 alignment**: native order and `IAI.Bench.SynthTickOrder 1` both produce >=3 decidable complete blink events, onset/off sets aligned with pixels (0-frame displacement) at the frozen 2/4/8/4/0, paced 30 fps configuration. Verify lever echo and actual dispatch order. Also cover pause/resume, travel, low capture rate/high render rate and frame gaps. | In a **buildable current-tree timing mutant**, suppress the end-world active-state sample and resample at the next capture Tick, reproducing pre-m40 ordering; with SynthTickOrder ON the pixel reader must fail. Also run reader known-answer sets with +/-1 shifts at each edge. Lever ON alone on the corrected implementation is a positive leg, not a negative control. | P required in A; G representative native/reversed alignment in A's staged core proof. Idle; no owner. |
| **R5 Pixel/label parity**: every delivered anomaly/mode in E5 is visibly exercised, >=3 decidable events per route, both edges **0 frames off**, SHIFT=0 and NOT-VISIBLE=0 with AA off; baseline and candidate pass. Retain 0/0/0 transition defaults. Whole-frame black gate passes its fixture-qualified threshold and no unaccounted frame/mask gaps. | `verify_capture.py --label-pixel-gate --selftest`, black-frame selftest, real NoApply/identity/null controls and the timing mutant; copied evidence with shifted onset/off must fail independently. All-unmeasurable or zero-event legs cannot pass. | **P for every delivered route** in A; G scope in E7. Idle; no owner. |
| **R6 Speed**: paced=true; speed_ratio <=1.05, and absolute candidate–baseline difference <=0.02 in matched fixed-duration legs; >=3 repetitions each, same fps/resolution/content and instrumentation off. Compare frame counts/durations and reject insufficient sampling. | A buildable bench-only delay of more than one frame budget (e.g. 50 ms at 30 fps) per captured-frame cycle must break the speed bound. A pace-off leg must fail the paced predicate even if its ratio happens to fall inside a one-sided limit. | P; representative G pair in A. Idle/foreground; no owner. |
| **R7 Renderer/material/state**: capture family/frame association, hide/depth/shadow restoration, LOD/camera geometry, owned texture restoration and co-user purity remain correct; both shader-readiness refusal directions and healthy admission work; mask pairing at 100% and all-miss/intermittent receipt handling match the **release** contract. Census has nonzero eligible cases, all-reasons completeness and no >100 ms frame under its stress fixture. | Existing HideOmitShadowSilencing/HideOmitDepthPassSilencing, MaskPairingProbe known-answer reader, StuckMipNoHold and TexCorruptAllReasonsFirstOnly controls where applicable; a stale/mismatched family receipt is rejected by the checker. A restoration mutant changes one saved state/owner and must fail residue checks. | P plus §0-required G routes in E7; idle. No J2 50%-screen-percentage/F1 claim inherited. |
| **R8 Control and teardown**: enabled server accepts a valid authenticated connection, produces real snapshot and preview, starts/stops a real run, rejects invalid authentication, reconnects, shuts down/travels without dangling callbacks/socket state; capture async/preview/run-log objects construct/destruct correctly. | Invalid auth fails admission while valid auth succeeds; a buildable suppressed snapshot/preview-response mutant fails the positive exchange; teardown/connection reader rejects incomplete close and leaked callback counts. | P and G Development; idle. Local loopback only; no messages to people or office connection. |
| **R9 Catalogue/UI/bench**: 5.1 source registry remains 12+2 gated twins; default pools/modes/seed sequence and selectors equal release; ordinary startup closes bench controls, explicit fixture startup opens only allowed controls; changed include/constructor boundaries do not erase a route. | Capability-filter mutation drops one 5.1 entry and fails exact catalogue comparison; attempt gated command without flag must fail while authorized fixture twin succeeds; altered deterministic sequence fails paired comparison. | P and G; idle; no owner. |

The R4 alignment reader may reuse the **documented** m40 set/SEP_RATIO=5 contract in a new independent reader under `tools/europa_phase_a_gate.py`; do not read J2's installed CaptureBench tooling to obtain it. Freeze known-answer cases before results. G230 says SynthTickOrder can host an **alignment** test, not a run-to-run pixel-identity arbiter: active image identity comparisons are performed on native-order deterministic legs. It is not required that the two different tick orders produce identical blink-frame sets, only that each order's labels match its own pictures.

R7 also covers the touched **change-evidence and annotation pipeline**: run the release's transition/annotation consistency readers and recompute measured change pairs from their PNGs; use null_effect for a real clean negative and solid_swap for a measured positive; exercise lost/mismatched/out-of-order receipts and shutdown drain. Preserve schema 2.1 meanings, partial-footprint/route fields, unmeasured versus measured-zero distinctions, and tag ownership. Corrupting a paired receipt or a delivered pixel must make the corresponding reader fail. Where a stock release defect is already documented, record the paired baseline result and return the unmet proposed predicate to Chat; do not repair it under an include-guard label.

### E5. Delivered routes and the zero-edge interpretation

Freeze the tested catalogue from d7eee97's registry/readme before legs. Conservatively exercise **all twelve current registry IDs**, so an ambiguous “delivered” subset cannot conceal reach:

| Route | Pixel/state observation required on 5.1 |
|---|---|
| missing_object (bench use), blinking | Visible geometry and shadow disappear/return; blinking's hidden-frame sets, not just one bounding interval. |
| lighting_mismatch | Off/dim/recolor/no-shadow light modes on a dynamic lit patch; values and pictures restore. |
| lod_corruption | Static and skeletal forced-LOD state and visible silhouette/detail restore. |
| missing_texture, corrupted_texture | Native 5.1 checker/pink material swap visibly changes a stable slot and restores original material identity. |
| time_dilation | Moving clock/object in the ROI; simulation-rate change/restoration and event timing, rather than an unchanged static frame. |
| camera_clipping | Camera/collision/near-clip fixture; projection and residue, refusal route where appropriate. |
| lod_popping | Moving distance/LOD fixture with distinguishable levels and explicit return to automatic LOD. |
| stuck_low_mip | Hold and proxy admitted separately, visible blur versus NoHold/null; baseline/restored mip state, resource ownership and co-user purity. |
| uv_corruption | All delivered tile/scramble modes visibly alter texture sampling; identity exact, ready/refused twins, owned-state restoration. |
| normal_corruption | All delivered invert/green-flip modes visibly alter lighting; identity exact, ready/refused twins, owned-state restoration. |

The twelve-registry coverage does **not** rename missing_object a client anomaly or expand A's 4.25 profile. The two gated twins are controls, not products. If the generic m49 checker cannot measure a route (global time/light effects in particular), supply a predeclared route-specific pixel/state reader with its own can-fail; report unmeasured edges as such until it exists. A registry event/count is never a substitute for visible effect.

**R2 for Chat:** P11's literal zero-edge wording and the release's accepted temporal-AA residuals cannot both mean “0 frames for every AA setting.” Proposal: zero is mandatory at the AA-off arbiter, as PRE-DELIVERY §0 already prescribes. Add paired TAA/TSR characterization preserving defaults and accepted limitations; do not silently widen transition windows or import J2 timing fixes. A baseline AA-off failure still blocks the affected gate and goes to Chat. This interpretation needs a ruling, not an after-the-fact waiver.

### E6. Exhaustive 5.1 reach mapping

This table assigns **every C-section planned file** to a reach class. Within a class each named/grouped file inherits every listed gate. Before the first leg generate an actual `git diff --name-only d7eee97..<port-sha>` manifest and join it to this table; an unassigned path fails admission. Byte-identical retained files are not counted as changed, but their reached behaviour is still tested. Tool/document changes cannot be marked “no reach” if they choose hosts, mutate build settings, select measurements or assert a qualification result.

| C-section files (exhaustive group reference) | 5.1 behaviour reached | Required gates |
|---|---|---|
| Descriptor and **all five Build.cs files** | Module/target/config inclusion, dependency exports, UHT/linking, compiler mode, feature defines, runtime startup and packaging. | R1, R2, R8, R9 |
| Shim .h/.cpp; five subsystem headers; three Injector subsystem .cpp files; CaptureSubsystem.cpp; ControlServerSubsystem.cpp; TickAudit .h/.cpp | All worlds and lifetimes; registration/tick order; cadence; GC; admission/auto/selection; capture timing/export; server ownership. This is the broadest reach. | **R3–R9**, plus R1 |
| EngineVersion/EngineCompat/StatsIncludes/Capabilities .h/.cpp; InjectorModule.cpp; AnomalyHiddenClass.cpp; AnomalyLod.cpp; AnomalyViewport.cpp | Type/LOD/near-clip/contact-shadow semantics, Nanite admission, target selection, compile boundaries, visibility/projection; indirectly every pixel/label route. | R1, R3, R4, R5, R7, R9 |
| Seven advanced texture implementation .cpp files and three non-reflected backend headers in C3; new AnomalyUnsupportedTexture.cpp | Shader readiness/streaming, hold/proxy and UV/normal material draws, identity/purity/restore; link completeness of common facade. The empty 5.1 facade branch must contribute no duplicate symbol. | R1, R5, R7, R9 |
| Six other anomaly .cpp boundaries plus CameraClipping.h in C3 | Time, camera, LOD popping, material swaps and bench change/null twins retain registration and execution. | R1, R4–R7, R9 |
| **All 18 Capture .cpp files listed in C2**, FrameCapturer.h, LabelWriter.h, PreviewTee.h | Colour/mask/receipt pairing; capture frame cadence; labels/schema/windows; render callbacks, readback/writer lifetime, Nanite provider, preview and run-log lifecycle. No guard is presumed neutral without enabled runtime proof. | R1, R4–R8 |
| ControlTransportState.h; ControlProtocol.cpp; ControlSnapshot.cpp | Validated transport, snapshots, preview, async close/reconnect, data serialization. | R1, R8, R9 |
| AnomalyShadersModule.cpp | Shader source mapping and enabled module initialization; all shader-dependent captures/texture routes. | R1, R2, R5, R7 |
| BenchModule.cpp; BenchTwoPartActor.h/.cpp | Fixture availability/flag filtering, owned reflected components, mask/geometry controls and no fixture leakage. | R1–R3, R7, R9 |
| strict_include_build.py | Temporary rule injection, defining include/UHT coverage, exact restore and final rebuild. | R1 plus §D strict can-fail/restore checks |
| New setup/run/inventory/descriptor/gate tools; check_package_descriptor.py | Protected-path safety, environment selection, disk/runtime admission, package truth/alias equivalence, completeness and falsifiability of evidence. | R0–R2 and each reader's own corrupted-input/runtime can-fail; final manifest completeness |
| New fixture script; each host's .uproject, two 5.1 copied Target.cs files, two config files, five probe module files, all generated fixture assets | Host compilation, map/game-mode/world layout, engine shader/material population, deterministic pixels, probe denominator and timing. Baseline/candidate receive the same declared fixture. | R0–R9 as applicable, fixed asset/config hashes and baseline positive controls |
| New prediction/plan/results docs; architecture.md; CLAUDE.md | Gate scope, supported profile and release claims. | Review actual diff/manifest/results against frozen predictions; deliberately omitted failed leg must fail completeness reader. No runtime neutrality inferred from prose. |

### E7. What is staged during A, and what waits

**Phase A itself includes new packages**, not only PIE. On 4.25: Development core/stub load, lifecycle, the four picture smokes and Shipping exclusion evidence. On 5.1: fresh baseline/candidate Development bench packages, a Shipping exclusion package/build-output proof, descriptor extraction with the matching UnrealPak, boot/load, worlds/GC/travel, native/reversed alignment sample, speed pair, core smoke and server lifecycle.

Because raw references, shim and include/guard changes reach the material/texture routes, **PRE-DELIVERY §0's visibly-working and readiness checks cannot all be deferred**. In A, also run UV/normal modes and bias/readiness twins, hold/proxy blur/restoration, auto pool and census timing/all-reasons in PIE and the required staged counterparts on the new 5.1 fixture. Use existing public tools/readers where available from d7eee97; recreate missing host-only fixtures in these new hosts. The 15,000-actor stress fixture is generated, not copied from the office/J2. Record any inability to reproduce a stock release predicate as a blocker, never as a blanket waiver for a “port-only” change.

The descriptor proof has two levels: (1) actual engine predicate matrix over all valid target/config combinations, with invalid combinations explicitly marked, and (2) actual compiled/staged Development and Shipping products. Extract the cooked descriptor rather than reading only the source. Run `check_package_descriptor.py --bench-cook --unrealpak <matching-engine-UnrealPak.exe>` on the Development bench package; compare semantic module membership to d7eee97 because the descriptor's key spelling intentionally differs. Shipping may strip the plugin descriptor entirely; prove **no IA modules/actions/receipt entries/binaries** instead of assuming a missing descriptor alone suffices. Use fresh per-config output manifests to exclude stale Development DLLs. No assertion of excluded WebSocketNetworking engine-plugin code follows from IA server compile-out.

Parser/writer evidence: 4.25 `Runtime/Projects/Private/ModuleDescriptor.cpp:180,185` and `Programs/UnrealBuildTool/System/ModuleDescriptor.cs:262,280` read the legacy keys. 5.1 counterparts use deprecated fallbacks at C++ `:215,220` and C# `:296,314`; 5.1 writers emit modern names (C++ `:321–374`, C# `:379` onward). Therefore the package checker must allow **equivalent spellings**, not accidentally demand raw JSON identity or ignore a genuine lost restriction. No such normalization is applied to the source descriptor sent to 4.25.

**Before any later merge or 5.1 delivery**, run a fresh staged qualification on the final merge/delivery SHA and intended delivery profile: full PRE-DELIVERY checklist, all delivered routes/modes and temporal-AA characterizations, recipient-relevant material/renderer population, delivery-mode CLI/auth/config checks, comment-strip/final-build binding where applicable, allowlisted delivery staging, **AnomalyBench and fixture-content removal**, G354 extracted descriptor, lever audit/no bench flags, bundle manifest/schema/docs and actual smoke. A bench package with all five modules is not that client package. If that later job needs a protected J2 host, it waits for a separate ruling/resume; A never acquires that permission.

### E8. Frozen prediction file

Propose committing `docs/predictions/094-europa-phase-a.md` on feat/europa-port **before the first build or runtime leg**, after Chat's ruling and before observing outcomes. Contents:

- base/branch/engine/compiler identities, approved R1/R2/R3 decisions, intended exact source-file/reach manifest and host/fixture recipe versions;
- every A/R gate ID and each leg's positive/negative role, engine/target/config, flags/cvars/AA/seed, expected module/capability set, event/world/action count floors and numeric thresholds;
- tick denominator/lifetime/pause/travel expectations, zero-edge definition, screenshot ROIs/null qualification and the three-event minimum;
- negative-control patch/lever identity, expected failure reason, build/restore procedure and known-answer reader inputs;
- baseline/candidate pairing, warning comparison rule, speed bound, staged versus PIE scope, P9/floor/foreground/idle admission, D2 and stop branches;
- blank result references, with subsequent results written to a separate append-only results/receipt file; final binary and asset hashes are added to each leg receipt before it starts.

Any newly discovered fix changes the source/reach manifest and may require a new prediction revision committed **before the affected rerun**. Keep the old prediction and failed/unqualified result; do not edit history to predict what already happened. Phase A exits only when mandatory predicates and their effective controls pass, owner legs are recorded, P9 remains unchanged, and Chat accepts the explicit remaining limitations.

## F. Phase B sketch and the C–F target decision

### F1. Six-anomaly basic profile

Phase B qualifies **missing_object (bench only), blinking, lighting_mismatch, lod_corruption, missing_texture, corrupted_texture** on 4.25. It does not turn on capture, the server, GPU shaders or the old bench module. Its result is a basic injection integration profile, not a dataset claim. Carry A's state/picture/refusal/teardown gates forward, adding the two material anomalies and mesh/material-slot variants. The 092-02 estimate remains **2–5 additional hands-on days**, conditional on A and native asset creation succeeding.

Read-only Paris findings and minimal proposed fixtures:

| Anomaly / need | What the source actually shows | Smallest addition / B verification |
|---|---|---|
| missing_object / blinking | 166 static-mesh packages and two maps; ample potential visible geometry. Byte inspection is not proof that every placed component is eligible. | Select one opaque, non-instanced visible mesh with a shadow receiver; add a second target for seed/selection checks. Snapshot every hide/depth/shadow flag and prove exact restoration. Keep missing_object bench-only. |
| lighting_mismatch | Both AssetsOverview and SampleMap name DirectionalLight and SkyLight classes; SampleMap also names PointLight and SpotLight. These are serialized class references, **not measured actor counts, mobility or effective illumination**. | One Movable point/spot light and a neutral lit plane/mesh in the local fixture; qualify off/dim/recolor/no-shadow modes. Do not infer success by hiding a baked light actor while baked illumination remains. |
| static lod_corruption | Serialized SourceModels arrays in 166 packages: **3 with one, 156 with two, 7 with four**. Many include LODScreenSize data. Counts are offline source-model counts, not guaranteed cooked render LODs or perceptible differences. | Select one existing mesh with >=2 actual render LODs; read runtime count/forced state, choose a camera exposing the difference. If none qualifies, add one simple native fixture mesh with deliberately different silhouette at two LODs through scripted import. No need to alter Paris originals. |
| skeletal lod_corruption | No SkeletalMesh class marker was identified in the inspected Paris packages. This supports **no identified skeletal fixture**, not proof that a complete asset-registry load would find none. The stock 4.25 High/Mannequin template content exists: 26 files, 14,813,474 bytes. | Start with one native template skeletal actor, copying only the needed manifest/dependencies into the new host's `/Game/Mannequin` mount. If only one LOD exists, duplicate locally and generate a visibly reduced second LOD; `EditorSkeletalMeshLibrary.h:35` exposes RegenerateLOD and reports failure if reduction is unavailable. Otherwise import a tiny explicit two-LOD skinned fixture. One-LOD content does not test the route. Skeletal qualification remains untested until the runtime/pixel proof exists. |
| missing_texture | Paris supplies mesh/material slots, but it supplies no validated replacement for the plugin's 5.1-authored checker material. | Create a native 4.25 checker material matching the existing contract; one one-slot mesh plus a two-slot fixture proves complete swap/restore and strong-reference GC. |
| corrupted_texture | Same slot population; no validated native equivalent of the existing pink material. | Create the native lit pink material; test swapped slots, shared-original material users, restoration and packaged cooking. Preserve original material identity and unaffected co-users. |

The package count came from a bounded offline scan of the allowed Paris Content only: 881 `.uasset` files and 2 `.umap` files. The maps are `/Game/ParisStreet/Levels/AssetsOverview` and `/Game/ParisStreet/Levels/SampleMap`. A real asset-registry/runtime inspection in the later new hosts must confirm actors, materials, render LODs and mobility before claiming a measured smoke. No J2 fixture/content inspection is needed.

### F2. Material authoring without owner editor clicks

**Preferred: editor Python on stock 4.25 in the new D: host**, using PythonScriptPlugin, EditorScriptingUtilities and MaterialEditingLibrary/asset tools already available in that engine. Adapt the existing `tools/create_anomaly_materials.py` recipe, not the saved 5.1 assets: create lit opaque, two-sided checker and pink materials with the intended roughness/specular and explicit supported static/skeletal usage flags. Do not set `used_with_nanite` on 4.25. Use a new idempotent tool such as `tools/create_europa425_materials.py` and refuse overwriting an unexpected existing package. Launch the unattended editor script through the same approved cache/log wrapper during an idle window; the owner does not click through material graphs.

Save the new assets under **`Content/Compat/UE425/Materials/`** in the port plugin, with a version-specific loader in non-reflected code. Existing 5.1 material paths/blobs remain untouched; 4.25 must not load/cook the original 5.1 plugin material directory. Explicit cook selections and dependency audits are required—having both asset versions in one tree does not make cook-all safe, or prevent asset-registry header scans (C5). A separate 4.25 content plugin/profile is an alternative if directory exclusions prove unreliable, but that would be a B design ruling, not an undeclared A file.

| Option | Advantages | Main risks / proof needed |
|---|---|---|
| **Editor Python, preferred** | Declarative native material graphs and save/cook; no owner graph editing; small repeatable recipe. | 4.25 Python/API differences, missing editor plugin or material expression/usage setter, unattended shader completion and wrong saved paths. Verify asset class/package engine version, graph outputs/usage, no overwrite, shaders ready, cold reopen and packaged appearance. Existing script's deletion/recreation behaviour must not target shared 5.1 paths. |
| **Editor C++ commandlet** | Stronger typed control of graph creation, usage flags and save; independent of Python exposure gaps. | New editor-only module/UHT/export/UnrealEd dependencies and more 4.25 code to maintain; shader compilation/save sequencing can still be wrong. Keep it host/tool-only and excluded from runtime/Shipping. Same asset/picture/cook gates; no engine edit. |
| **Runtime MIDs from existing engine materials** | Can avoid new saved graph assets when a suitable parent already exists and is cooked. | A MID can change only parameters the parent exposes. It cannot invent the required checker graph or guarantee a lit pink material; engine default/grid materials may be editor-only, differently shaded or uncooked. Runtime graph construction/shader compilation is not a dependable packaged path. Use only after inspecting the actual parent and proving the same semantic contract; otherwise this is a reduced diagnostic, not qualification of the two anomaly types. |

B's exit: both native materials load after a cold editor start and in a cooked 4.25 Game; all six anomalies meet apply/restore/refusal/picture gates, including skeletal LOD if that route is promised; no 5.1 asset changed; affected 5.1 gates pass again. Only then decide on an office injection-only integration. The early office card can precede all of this.

### F3. Later phases remain separate decisions

| Phase | What it needs / exit decision from 092-02 retained | Estimate, not authorization |
|---|---|---|
| **C — capture/masks/evidence** | New 4.25 colour/SVE backend (no assumed after-pass hook), actual output/AA metadata, family receipt lifetime/claim design without ISceneViewFamilyExtentionData, stencil/depth ownership, row-pitch-aware readbacks, m40 timing at a proved anchor, mask pairing, transitions/change/census. PostActorTick is a candidate, not equal to end-world. Admit datasets only after both-order/pixel/receipt/negative-control gates. | 8–15 working days; more if an engine/fork bridge is required. |
| **D — office integration** | Owner-local build/readings on Europa's fork, sanitized includes/exports/errors, renderer/timing/stencil/restore proof against actual content. Only hand-typed numeric/boolean/sanitized results leave. Stop or reduce the supported profile if ownership/timing cannot be proved. | 2–5 hands-on days plus office latency; fork changes can make this unbounded. |
| **E — texture parity** | Explicit 4.25 material-readiness/vertex-factory adapters and draw pipeline for UV/normal; streamer/resource/identity snapshots and separate hold/proxy restoration proof for stuck_low_mip. No readiness shortcut or inferred pitch. Route-by-route admission, no parity from a single working proxy. | 10–20 working days. |
| **F — remaining parity/delivery** | Optional alternate WebSocket server/backend (stock 4.25 lacks the required public interface), remaining anomalies, targets, packaging/tooling, full 5.1 regression and supported-profile docs. Chat chooses merge/delivery only after proof. | 5–10 working days, some scheduling overlap with validation. |

Outside A/B, one-line work statements: **time_dilation** needs simulation/render cadence and restoration validation; **camera_clipping** needs 4.25 camera/projection/near-clip/collision and label geometry proof; **lod_popping** needs LOD accessor plus temporal switching/restore proof; **stuck_low_mip** needs streamer hold/proxy and ownership adapters; **uv_corruption** needs native material readiness/draw/uniform adapters; **normal_corruption** needs the same draw/readiness work with correct normal transform/lighting and restore evidence. These are not silently admitted because a shared header compiles.

Owner's later product choice is still 092-02's: an injection-only integration, a six-anomaly capture/mask subset (roughly 3–5 engineer-weeks in that study), or wider parity (6–10+ weeks). Those are conditional planning ranges with office latency outside them. **No target answer changes Phase A's obligations or pauses its technical planning.**

## G. Early Europa office read card (P14)

Run this in week 1, preferably before spending time on capture design. **Read only:** inspect already-local source/config/build/version information or query settings in an already-running authorized session. No build, capture, upload, copy, source export or new editor launch is required for this card. If a runtime override cannot be known without launching, return **-1 (unknown)** rather than changing settings or guessing. The owner manually types the numbered numeric answers; no clipboard/source/log/screenshot attachment comes home. No office path, real title, studio, fork name, asset name or source text belongs in the answer. Europa and Europa's fork are the only names used here.

Fifteen checks; tuples contain only numbers. 0 means a checked negative, 1 a checked positive, -1 unknown. These are local read prompts, not instructions to send the inspected file.

| # | Local read and numeric answer | What it decides / phase de-risked |
|---|---|---|
| 01 | Engine **major, minor, patch** tuple. | Confirms exact supported version predicate; A/D. |
| 02 | Engine **changelist integer** (and compatible CL integer if separately defined). | Separates stock assumptions from fork revision; A/C/D. |
| 03 | Selected compiler product-version numeric tuple, plus `_MSC_VER` integer if known from the last local build. | Whether the 14.16 language/toolchain choice transfers; A/D. |
| 04 | Selected Windows SDK numeric version tuple from that build. | SDK availability/ABI/header risk; A/D. |
| 05 | Three flags in order: **OnWorldPostActorTick exists; OnWorldTickEnd exists; ShouldCreateSubsystem exists**. | Feasible world-creation adapter and candidate capture timing anchors; A/C/D. Presence alone is not ordering proof. |
| 06 | Four parser flags: **WhitelistTargets; BlacklistTargetConfigurations; TargetAllowList; TargetConfigurationDenyList** actually read by the local descriptor code. | Legacy compatibility and Shipping exclusion risk; A/D/F. |
| 07 | Six SVE hook flags in order: **PreRenderViewFamily_RenderThread; PreRenderView_RenderThread; PrePostProcessPass_RenderThread; SubscribeToPostProcessingPass; PostRenderViewFamily_RenderThread; PostRenderView_RenderThread**. | Renderer-backend design options; C. Only existence is returned, no signature/source. |
| 08 | Renderer code: **0 deferred, 1 forward, 2 custom/other**, -1 unknown. | Whether the stock deferred plan applies; C/E. |
| 09 | Effective AA code: **0 off, 1 FXAA, 2 TAA, 3 MSAA, 4 TSR/backport, 5 custom/other**. | Pixel-edge/history/output assumptions and matrix size; C/D. |
| 10 | **Output width, height, screen percentage, dynamic-resolution-enabled 0/1**; min/max percentage if known. | Output extent/resampling/pairing and performance scope; C/D. Unknown runtime override stays -1. |
| 11 | **r.CustomDepth integer**, plus **host itself writes custom depth/stencil 0/1**. | Shared-stencil ownership collision; C/D. Enabled buffer and host ownership are different facts. |
| 12 | **Simulation/render decoupled 0/1**, plus **explicit decoupled-tick control exists 0/1**. | Whether stock world-to-render timing transfers or a fork bridge is likely; C/D. |
| 13 | **RHI code** (0 D3D11, 1 D3D12, 2 Vulkan, 3 other), **stereo/multiview 0/1**, **virtual texturing used 0/1**. | Mono/RHI/readback/material profile assumptions and scope escalation; C/E/F. |
| 14 | **UTickableWorldSubsystem exists 0/1; tick loop calls IsAllowedToTick 0/1; count of host/plugin consumers requiring that specific ancestry** (including subclass/cast/IsA queries). | Shim semantics/backport and external type-ancestry compatibility; A/D. Count without names/source. |
| 15 | Counts in a prospective small local test area: **eligible visible opaque static meshes with >=2 LODs; movable lights; skeletal meshes with >=2 LODs**. | Minimal office fixture needs and whether six-route/skeletal coverage is possible; B/D. 0 is informative, not a failure. |

Return format is simply `01: <numbers>` through `15: <numbers>` typed by the owner. No screenshots are necessary. An unknown or contradictory read becomes a Chat design question; it does not permit private evidence transfer. Reserve roughly 45–75 minutes of owner read time; if that is impractical, prioritize 01–07, 11–12 and 14, with the rest explicitly unknown.

## H. Implementer Relay job split

These are proposed sessions after Chat's ruling. One implementer owns one bench. Each job has a durable receipt and P9 entry/exit proof; a long compiler wait may span wall-clock time, but no job carries an unbounded runtime hunt. A failure leaves a precise resume point and stops at D2 rather than extending the same session indefinitely. Job names below are planning identities, not already assigned relay numbers.

| Order / job | Bounded deliverable and dependencies | Hands-on estimate | Owner / idle timing |
|---|---|---:|---|
| **A1 — baseline and scoped setup** | Record P9 and source-host baseline; resolve/floor-check roots; create three copies/worktrees; verify pins; implement inventory/scoped routing design approved under R1. Commit approved plan/predictions before first build. Exit with manifests and all routing uncertainties dispositioned. | 2–3 h | No runtime. Owner early office card may proceed independently. |
| **A2 — availability/build rules/descriptor** | Exact-version C# rules, shared capability/version header, old descriptor keys, minimal shader/bench/module stubs, C++17 policy, matrix checker. No renderer port. | 3–4 h | Builds allowed while PC in use; no editor/game. |
| **A3 — shared C++ closure** | Shim and five parents; pointers/opaque Control lifetime; compat/includes/core admission; guard advanced/Capture implementation closure and complete disabled symbols. Supply the minimum host-probe module sources needed by declared .uproject modules before A4. Modular/link review. | 5–8 h | Source/build session; no owner. May stop with compile blocker; no bench leg. |
| **A4 — build and strict closure** | First real 4.25 Editor/Game/Shipping and 5.1 baseline/candidate builds; adapt/run strict tool on both port checkouts; toolset/warnings/defines and exact restore proof. Freeze final runtime candidate or report blocker. | 4–6 h | No interactive runtime. Build latency additional. |
| **A5 — probe/fixture and reader readiness** | Finish portable host probe and all reader can-fails; generate the exact fixture recipes/manifests; prepare engine screenshot driver, independent tick denominator, new 5.1 material/texture/motion fixtures. Normal-rebuild any changed probe/host sources and bind new hashes before runtime. Test harness logic without claiming runtime proof. Commit prediction revision before first engine authoring leg. | 4–6 h | Source-only parts anytime. Actual editor-based asset authoring waits for A6/idle. |
| **A6 — first 4.25 editor and world leg** | **Separate cold first-launch/shader session**, routing evidence, fixture authoring, owner preview sentinel, basic load, worlds/ticks/pause/multi-PIE/travel/GC controls. Stop if routing/cache/preview is unproved. | 2–3 h | **Owner leg 2:** launches via approved wrapper, opens one mesh preview. Idle throughout; shader time additional. |
| **A7 — 4.25 pictures and auto** | Four anomalies, >=3 events each, null/NoApply/SkipRestore controls, seeded auto/admission/teardown, all screenshots/diffs/state receipts; then owner eye check. | 3–4 h | Idle. **Owner leg 3:** eye acceptance after numerical evidence, including return state. |
| **A8 — 4.25 package and exclusion** | Development cook/package core smoke/world/GC, Shipping module exclusion, effective descriptor/define proof and output manifest; restore final binary after controls. | 2–3 h | Idle for cook/run. No new owner action. |
| **A9 — 5.1 baseline and lifecycle/control** | Author/freeze identical baseline/candidate fixtures, fresh d7eee97 positive baseline, candidate worlds/GC/transport/selector/bench gating and relevant mutants. No J2 assets/binaries. | 3–5 h | Idle for all editor/runtime work. |
| **A10 — 5.1 timing/pixels/speed** | Both tick orders plus effective timing mutant; all twelve-route PIE coverage/modes, AA-off edges and visibility; baseline/candidate speed pairs and known-bad controls; characterize accepted temporal limitations. | 4–6 h | Idle/foreground. If fixture/reader setup fails twice, return blocker; do not squeeze failures into a pass. |
| **A11 — 5.1 staged/material/descriptor** | New Development/Shipping products, package descriptor proof, staged core/control/timing/speed and §0 material/texture/readiness/census/purity checks including their controls; final hashes. | 4–6 h | Idle for cook/runtime; no owner. |
| **A12 — final consistency review** | Check all actual changed paths have reach/gates, final-binary binding, prediction/result completeness, no stale controls, no new warnings, P9 unchanged. Produce Phase A results and proposed B brief for Code review then Chat decision. No merge or delivery. | 1–2 h | Read-only result review; no owner. |

Total **37–56 hands-on hours**, roughly 5–8 working days with normal review/debugging, plus first shader/build/cook time and idle/office scheduling. This is a slightly wider A allowance than 092-02's 3–7 days because P11 adds explicit owner/world/picture and substantial final-binary 5.1 regression work. It is not a cap that permits skipping a failed gate. If A3/A4 expose a new renderer dependency or A10's fresh fixture needs a new measurement design, split the follow-up session and return the scope/estimate to Chat.

The **three owner legs** are: (1) early office read card, independent of home build progress; (2) first 4.25 editor/preview session at A6; (3) eye check at A7. Ordinary idle windows are scheduling prerequisites, not requests for repeated permission to perform already approved steps. The later product-target decision can be requested alongside the office card and need not block A. An office compile, installation or dataset collection would be a later separately ruled job.

## I. Risks, possible wrong assumptions, and rulings requested

### I1. Highest risks

1. **Write-routing versus stock tool behaviour.** R1 is a real P8/P10 boundary, not a disk-size estimate. C: currently has under 2 GiB above its floor. Environment variables and `-UserDir` do not prove all Windows KnownFolder consumers moved. No runtime implementation should claim all-D compliance without a ruling and effective write trace.
2. **A compile-only port can silently disable the core.** Capture normally supplies the Nanite probe; CDOs load 5.1 assets; advanced code remains in Injector; UBT compiles every .cpp; UHT and FVTableHelper destructors cross DLL boundaries. The explicit probe, capability/guard closure, public dependency, modular build and positive smoke address these failure modes.
3. **Tick/ancestry changes reach every 5.1 route.** 4.25's missing IsAllowedToTick makes a verbatim clone wrong; 5.1's new ancestry can break an external cast even if all plugin tests pass. Multi-world denominators and real dormant/preview negative controls are necessary. Office consumer count matters.
4. **Qualification infrastructure is part of the cost.** Both existing 5.1 hosts are J2 candidate installations. Fresh Paris-based 5.1 fixtures must reproduce meaningful timing, texture, skeletal, camera and readiness stressors; a green empty fixture has no value. A10/A11 are substantial work, not a quick boot check.
5. **Descriptor success can be false.** Legacy keys parsing is separate from actual package inclusion, stale DLLs and client bench stripping. Module compile-out is not descriptor exclusion; reflected shells and the enabled engine WebSocketNetworking dependency must be described accurately.
6. **Disk and final-binary drift.** Three hosts, strict rebuilds, PDBs, cold DDC and retained PNGs can exceed the estimate. The intended D: path may traverse a junction; existing E: banks are not spare D: capacity. Mutant restoration/source fixes invalidate reached gates until rebound to the final binary.
7. **The fork may invalidate stock assumptions.** Different hooks, decoupled rendering, stencil ownership, forward/MSAA/stereo/VT or toolchain exports can widen C–F considerably. The early numeric card reduces this risk without breaching confidentiality; it cannot prove runtime timing by itself.

### I2. Where the ruling or study may be wrong or incomplete

- **092-02 wrapper preference versus P10:** superseded explicitly. The known good derived Target.cs assignments remain the build selector. The fact that stock UBT can still write its ordinary XML cache is a separate routing problem, not a reason to reinstate the wrapper without a ruling.
- **P8's all-D wording may be stricter than stock binaries can satisfy** under process/project-only controls. Stock UBT, ShaderCompileWorker/CrashReportClient and Windows/driver ancillary paths have different roots. A valid plan must expose the unresolved paths; it cannot substitute `LOCALAPPDATA=D:...` and assert that SHGetKnownFolderPath obeys it.
- **P4 “chaining” can be misread:** the 5.1 tickable class does not call UWorldSubsystem's Initialize/Deinitialize from its own implementation. Preserve the actual engine semantics and the five derived Super call placements, not an intuitively tidier extra chain. Also provide the engine-style non-pure fallback GetStatId declaration in the abstract UCLASS so generated construction is legal while each concrete subclass retains its existing stat.
- **P7 “compiled out” needs a structural interpretation:** module/reflected shells remain loadable to satisfy stable declarations and diagnostic startup, while functional capture/server/shader/bench work is absent. Descriptor Shipping exclusion is complete absence. If P7 instead means there may be no reflected Capture/Control/Bench classes at all on 4.25, that conflicts with P3's shared literal declarations and requires another design ruling.
- **P11 zero edges across unspecified AA settings is underspecified.** d7eee97 deliberately retains temporal-AA limitations and 0/0/0 defaults. A's AA-off zero arbiter is proposed; no baseline defect becomes a silent port fix. Similarly, “once per frame” must mean each eligible update of the subsystem's world, not one tick across all PIE worlds per global frame.
- **070's nine-anomaly, no-pixel, junction and PostActorTick-equivalence statements are obsolete.** P7/P8/P11 and the 092-02 independent timing read prevail. The old G232–G235 numbering cannot be applied blindly to the current gotcha file.
- **The Paris content survey is a bounded offline inference.** SourceModels are not render-data truth; serialized class references are not actor counts; absence of a marker is not an asset-registry proof. The plan intentionally does not declare the LOD/light/skeletal gates already feasible at runtime.
- **5.1 “neutral” cannot mean byte-identical binary or ancestry.** Pointers, class layout, module exports and constructors change. Same-source enabled algorithms and green reach gates are the actual proposed promise; unknown third-party ancestry users remain an integration risk.

### I3. Questions for Chat (rulings, not actions already accepted)

**R1 — resolve P8 write scope before implementation launches/build writes.** Recommended bounded option: retain strict physical-D routing for all port-owned DDC, shader working/debug data, host builds/intermediates, logs, temp, packages and evidence; allow only specifically documented unavoidable stock-tool/OS ancillary KnownFolder writes, with before/after trace, quantified size, no global/per-user settings change and unchanged C/E floors. Chat must decide whether that exception is acceptable. If P8 is literal with no exception, keep the launch/build hold and commission a separate isolation design; no “all writes D:” claim is supported today. This report does not authorize the exception.

**R2 — confirm the P11 pixel and tick interpretation.** Approve AA-off as the mandatory zero-edge arbiter, paired temporal-AA characterization preserving release defaults/limitations, and “once per eligible world update” as the tick denominator. If zero edges are required under all AA modes, A includes a release defect investigation beyond neutrality and needs a new scope/estimate.

**R3 — provide/identify the operative W3/W4 idle recipe.** P12 carries the names but the allowed sources read here do not establish the complete current numeric scheduling recipe. The plan preserves those gates and requires their actual recipe before runtime; it does not replace them with an invented timer. No further J2 evidence read is needed to answer this.

**R4 — accept structural scope and qualification cost.** Confirm inert reflected shells at ANOMALY_CAPTURE/CONTROL_SERVER/BENCH=0, exact-version support, bench-only missing_object on the 4.25 profile, new host-only probe/fixtures, the 12-job split and A's staged 5.1 minimum. This avoids treating “compile-out” as removal of UHT declarations or shrinking broad 5.1 reach to a boot test.

No source permission is sought in this reviewer job. Chat's plan-before-code ruling remains the required next decision. Baseline failure, ineffective controls, additional production behaviour change, or an external ancestry dependency discovered later returns to Chat with evidence and a proposed narrower decision.

### I4. Owner product question

After the early read card, choose the first useful Europa target: **injection-only basic profile**, **six-anomaly capture/mask profile**, or **wider parity**; specify whether skeletal LOD and remote/dashboard transport are needed in that first delivery. Missing_object stays a bench diagnostic under every choice. This choice sets B–F priorities and office work; it does not change or delay A's required proof.

VERDICT: PLAN READY FOR RULING — P8 WRITE-ROUTING HOLD | base=d7eee97 | europa-e1=reimplement | 4.25 host=D:\IA_Europa\StylizedParisStreet | 5.1 host=D:\IA_Europa\ReGate51 | phase A jobs=12 | owner legs=3
