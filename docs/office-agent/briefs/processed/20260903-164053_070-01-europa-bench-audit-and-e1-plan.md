# SESSION: new-default
# NAME: 070 europa-e1
# EFFORT: xhigh

SESSION 070, BRIEF 01 — EUROPA PHASE 1, STAGE 0: BENCH AUDIT + E1 PLAN. READ-ONLY on all source and on the host project; one new doc, one commit, push. Headless: nobody answers questions; your FINAL MESSAGE is the only thing read. DO NOT IMPLEMENT ANYTHING — this brief ends in a plan.

## THE NEW FACT
The owner has installed **UE 4.25 on this bench** and created a host project at `E:\Unreal Projects\StylizedParisStreet` (BP-only: `EngineAssociation "4.25"`, no `Source/`, no `Plugins/`, `Content/ParisStreet` — a marketplace street scene). This answers Phase 0 NEEDS-DECISION 3: **the bench hosts 4.25. Yes.**

🚨 **THE DISTINCTION THAT GOVERNS THIS WHOLE BRIEF.** `StylizedParisStreet` on a stock 4.25 is a **BENCH FIXTURE**. It is **NOT EUROPA**. Europa is a title on a possibly-MODIFIED 4.25 on the owner's office PC and remains unmeasured. Therefore: anything you read out of the stock 4.25 tree is **MEASURED (stock 4.25)** — a fact — and simultaneously a **PREDICTION for Europa**. Grade every row that way, in two columns if that is clearest. Never write a sentence that could be read as "we measured Europa" (G120: an unverified mechanism must never become a scope decision). The bench fixture needs no codename; Europa's codename discipline is unchanged.

## SCOPE — five parts, in this order. Part A is a gate on the rest.

### A. 🚨 TOOLCHAIN GO/NO-GO (do this first; it decides whether E1 can start at all)
4.25 predates VS2022 support. If this machine only carries the 5.1 toolchain, E1 stops here and the owner has a software-install errand — I want to know that in 20 minutes, not after an hour of planning.
Report, as facts with paths/versions:
 1. The 4.25 install: exact root path, and **launcher-installed vs source build** (say how you determined it). Its `Engine/Build/Build.version`.
 2. Every Visual Studio installation present, every MSVC toolset (`14.2x` / `14.3x`) and every Windows 10/11 SDK version, from disk and/or `vswhere`.
 3. Whether 4.25's UBT will accept what is installed — read `Engine/Source/Programs/UnrealBuildTool/Platform/Windows/*` (compiler/toolchain/SDK version discovery and any minimum/maximum) rather than reciting the release notes.
 4. `HKCU\Software\Epic Games\Unreal Engine\Builds` — current entries.
 5. Free space on `C:`, `D:`, `E:`.
 6. **VERDICT, one line: can C++ be built against 4.25 on this machine TODAY — YES / NO / NEEDS-INSTALL(exact component names and versions).**

### B. TURN PHASE 0'S UNKNOWNS INTO MEASUREMENTS (against the stock 4.25 tree)
`docs/europa/PHASE0-port-surface-4.25.md` §D lists the questions. Answer every one the stock tree can settle, each with an engine `file:line` citation:
 · **D-1 (all five):** `FWorldDelegates::OnWorldTickEnd` — exists? and **where is it broadcast** (the call site, so we know its position in the frame relative to `OnWorldPostActorTick` — this is m40's label anchor and m44's record-birth point, the single highest-value row here); `OnWorldPreActorTick` / `OnWorldPostActorTick`; `UTickableWorldSubsystem` present or must we compose `UWorldSubsystem` + `FTickableGameObject`; `USkinnedMeshComponent::GetNumLODs()` + static/skinned forced-LOD accessors; `UPrimitiveComponent::bVisibleInRayTracing`.
 · **D-1 extension — the m45 hide surface:** confirm every flag in `AnomalyHiddenClass.cpp:102-125` exists on 4.25, and confirm `bRenderCustomDepth` is honoured for a primitive with `bRenderInMainPass=false` (read the 4.25 renderer's custom-depth pass gating, cite it) — that is m45's load-bearing assumption.
 · **D-2 (9,10,11,12,13):** which `ISceneViewExtension` hooks exist and their exact 4.25 signatures; is any handed an `FRDGBuilder`; `FRHIGPUTextureReadback`/`FRHIGPUBufferReadback`; how a 4.25 `.usf` reads CustomStencil/CustomDepth/SceneDepth; `FPixelShaderUtils`/`FComputeShaderUtils`; `FMaterialShaderMap::UsesSceneTexture` + `PPI_*` values. Also: does `Source/Runtime/Renderer/Private/SceneRendering.h` define `FViewInfo` with a `ViewRect` member on stock 4.25, and does a launcher install even ship those private headers?
 · **D-3 (18):** is `WebSocketNetworking` present in this 4.25 install?
 · ⛔ **Leave EXPLICITLY UNANSWERED and say so:** D-2 q6 (is Europa's renderer modified), q7 (forward or deferred on Europa), q8 (does the Europa title already use custom depth/stencil), D-3 q14/q15/q17, and the IoStore question. Those are office-PC observations; nothing on this bench can settle them.
Update `docs/europa/PHASE0-port-surface-4.25.md` in place where a row changes from UNKNOWN to measured — keep the grade legend and add a "measured on stock 4.25" marker so a cold reader cannot mistake it for a Europa measurement.

### C. HOST PROJECT + HOW THE PLUGIN IS MOUNTED
 1. `StylizedParisStreet` is BP-only, so it cannot compile plugin C++. Specify the **minimal** conversion to a code project (exact files: `Source/<Name>/<Name>.Build.cs`, module `.h/.cpp`, `<Name>.Target.cs`, `<Name>Editor.Target.cs`, `.uproject` `Modules` block) — as a plan, do not create them.
 2. **Mounting ruling (mine, for you to confirm or refute with evidence):** the 4.25 project's `Plugins\AnomalyInjector` is a **Windows directory junction** (`mklink /J`) to the one repo working tree at `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector`. One codebase, one git repo, two hosts, zero copy drift — the locked "one codebase" decision, enforced by the filesystem. Verify the risk: do 4.25 and 5.1 build outputs collide inside that shared tree? Check the actual 4.25 output naming (`Binaries\Win64\UE4Editor-*.dll` and `Intermediate\Build\Win64\UE4Editor\…`) against 5.1's (`UnrealEditor-*`, `…\Win64\x64\UnrealEditor\…`), and check `.gitignore` covers both. Also confirm UBT resolves plugins through a junction. **Named fallback if they collide: a second `git worktree` of the same repo on `E:`** (still one codebase, one history) — say which you recommend and why, with the evidence.
 3. Anything else the 4.25 host needs: `.uplugin` `EngineVersion` pin (currently unset — is that a problem on 4.25?), and read `AnomalyCapture.Build.cs`'s **tick-pin fork probe** and say what it will do when pointed at a stock 4.25 tree (Phase 0 flagged it may mis-fire).
 ⛔ Write NOTHING into `E:\Unreal Projects\StylizedParisStreet` in this brief. Read-only there.

### D. MODULE SEPARABILITY — the question that shapes E1
Phase 0's plan step 1 was "reduce the `.uplugin` to `AnomalyInjector` only". **That cannot stand as written: there is ONE `.uplugin` in ONE tree, and reducing it breaks the 5.1 host on the same branch.** So:
 1. Produce the exact cross-module dependency map for `AnomalyShaders` / `AnomalyInjector` / `AnomalyCapture` / `AnomalyControlServer` — Build.cs `Public/PrivateDependencyModuleNames`, plus real `#include`s and symbol use across module boundaries, with file:line. State the direction of each edge.
 2. Answer: **can `AnomalyInjector` compile and run with the other three compiled out, without touching the `.uplugin`?**
 3. **My preferred mechanism, to confirm or refute:** keep the single 4-module `.uplugin`; in each non-injector module's `Build.cs`, branch on `Target.Version.MajorVersion == 4` to compile that module as an **empty stub** (no engine-version-specific sources, `IMPLEMENT_MODULE` only, no dependencies that do not exist on 4.25) so the module still loads and does nothing. That is a module-availability switch at a declaration site, not an `#if` inside a gate-bearing algorithm, so it stays inside the locked rule. If a real dependency edge makes this expensive — e.g. the injector consumes a capture type — say so at file:line and propose the cheapest alternative that keeps one codebase and a green 5.1 build on the same branch.
 4. Say what the injector loses when capture is stubbed (labels, census consumption, the m40 anchor) so E1-G6's "no artifact is produced or claimed" is a statement about code, not a hope.

### E. THE E1 PLAN
File-by-file, ordered, with the branch (`europa-e1` off master; conventional commits `feat(europa): …`; **no tag** — tags stay in the pending office batch). Restate gates **E1-G1..G6** from Phase 0 §C, adapted to this bench host, and for **each** gate give: the exact command or action, **who runs it** — you headless, or the owner by eye — and its pass predicate. Specifically resolve:
 · E1-G1 both targets (`G221`) — the 4.25 editor **and** game target build commands for this host.
 · E1-G3 needs a named actor in the ParisStreet level. **How are actor names obtained headlessly on 4.25?** (a commandlet, an existing `IAI.*` listing command, or an owner action — pick one and say why.)
 · E1-G4 the prove-it-can-fail leg, and E1-G6 stated as a code fact per D-4.
 · Which of the six the owner must run in person, listed separately as his errand sheet.
Also state the residual risks you would accept to start E1, and any you would not.

## DELIVERABLE
`docs/europa/PHASE1-bench-audit-and-plan.md` (new) + the in-place PHASE0 updates from §B. Commit `docs(europa): 4.25 bench audit and phase 1 plan`. Path-scoped `git add` only — never a bare `git add docs` (it swept the untracked CHAT-HANDOFF files once). Verify `HEAD == origin/master` before you start; push when green.

## STANDING RULES
Headless — no questions, nobody answers. On any failed verification or a genuine fork, STOP and make the final message a report marked **NEEDS-DECISION**; never iterate past a failure. Do not implement, do not build, do not create files outside the repo doc. Codename discipline absolute. If Part A's verdict is NO or NEEDS-INSTALL, still complete Parts B–E (they are all reads) and lead the final message with the blocker.

## FINAL MESSAGE STRUCTURE (required)
1. One-line **TOOLCHAIN VERDICT** (YES / NO / NEEDS-INSTALL + exact components).
2. STATE: branch, HEAD, push status, what was written.
3. **MEASURED ON STOCK 4.25** — the D-1/D-2 answers, shortest form, flagged where they change Phase 0's grade. Call out anything that got WORSE than Phase 0 predicted.
4. **STILL UNKNOWN FOR EUROPA** — the office-PC-only list.
5. **MOUNTING + SEPARABILITY** — your ruling on the junction and on the stub-module mechanism, with the evidence.
6. **E1 PLAN** — the ordered step list and the gate table in one block.
7. **OWNER ERRANDS** — anything only a human at the machine can do, as a numbered list.
8. **NEEDS-DECISION** — genuine product/scope forks only, or "none".
