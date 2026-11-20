# REPORT for 070-01-europa-bench-audit-and-e1-plan.md
brief: 070-01-europa-bench-audit-and-e1-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 16:11:02 | finished: 2026-09-03 16:40:53 | duration: 30 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\070-01-europa-bench-audit-and-e1-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
Pushed. `HEAD == origin/master == 489c29d`, tree clean.

---

## 1. TOOLCHAIN VERDICT

> ✅ **YES — C++ builds against 4.25 on this machine TODAY, no install needed**, using the already-present **VS2017 Build Tools 15.9 / MSVC 14.16.27023** — **provided two pins are set** (`WindowsPlatform.Compiler = VisualStudio2017`, `WindowsPlatform.WindowsSdkVersion = "10.0.17763.0"`). They are **configuration, not installation**, and belong in the 4.25 host's `Target.cs`.

⚠ **Without the pins the build fails, and it fails misleadingly.** 4.25's `WindowsCompiler` enum stops at `VisualStudio2019` (`UEBuildWindows.cs:19-55`), but its VS2019 filter has **only a lower bound** (`:991`), so VS2022 (17.x) enumerates as "2019" and `GetDefaultCompiler` tests 2019 **first** (`:864`) → it would select **MSVC 14.42**. Separately, `PreferredWindowsSdkVersions` is `10.0.18362`/`10.0.16299` (`:627-630`) — **neither installed** — so the fallback takes the newest, **`10.0.22621.0`**. Both defaults are wrong; both are one line to fix.

🎯 `14.16.27023` is literally on 4.25's own `PreferredVisualCppVersions` list (`:618-621`).

## 2. STATE

Branch `master`, **HEAD `489c29d` == `origin/master`**, pushed. Tree clean; all seven owner `CHAT-HANDOFF-*.md` remain untracked (path-scoped `git add` of exactly two files, verified before commit).

**Written:** `docs/europa/PHASE1-bench-audit-and-plan.md` (new) + `PHASE0-port-surface-4.25.md` updated in place (13 targeted edits, +192/−60 — not the whole-file encoding smell; emoji and no-BOM verified). ⛔ Nothing built, no source changed, **nothing written into `E:\Unreal Projects\StylizedParisStreet`**.

**Engine:** `E:\EpicGames\UE_4.25`, **4.25.4** CL 14469661, **launcher-installed** (`InstalledBuild.txt` present; no `Setup.bat`/`GenerateProjectFiles.bat`/`.git`; `LauncherInstalled.dat` confirms). Disk: `C:` 29.31 / `D:` 188.21 / `E:` 365.13 GB.

⚠ **`HKCU\...\Builds` holds exactly one entry** (the 5.1 GUID) and **needs no second** — a launcher engine resolves by version string. ⚠ **`HKLM\SOFTWARE\EpicGames\Unreal Engine` lists 4.26–5.6 under `D:\EpicGames\UE_*` and none of those directories exist — stale.** I nearly concluded 4.25 wasn't installed from it.

## 3. MEASURED ON STOCK 4.25

**Score vs Phase 0: 5 better, 3 worse, 1 new item.**

🔻 **WORSE — call these out first:**
- 🚨 **`OnWorldTickEnd` is ABSENT** — zero hits across all of `Engine/Source/Runtime`. Phase 0's highest-risk row went the bad way. **But the fallback is better than feared:** `OnWorldPostActorTick` broadcasts at `LevelTick.cpp:1694` under the **same `bDoingActorTicks` guard** (`:1690`) that encloses `TickObjects` at `:1630` (opened `:1545`) ⇒ **after every tickable, still pre-draw — m40's anchor position.** Its alignment gate must still be re-run on 4.25 (an E2 entry condition).
- 🔻 **`PrePostProcessPass_RenderThread` is ABSENT too** — Phase 0 asserted 4.25 has it. 4.25's hooks are `PreRenderViewFamily/PreRenderView/PostRenderBasePass/PostRenderViewFamily/PostRenderView_RenderThread`, **all taking `FRHICommandListImmediate&`, none an `FRDGBuilder`**.
- 🔻 **`bCastContactShadow` is ABSENT** (zero hits in all of `Runtime/Engine`); Phase 0 graded the m45 flag set "PRESENT (likely)". 7 of 8 present. ⚠ Not cosmetic — that shadowing path can't be silenced on 4.25; an open pixel question, not something to assume away.

🎯 **BETTER — and one of these resizes the project:**
- 🎯 **The mask pass is a PORT, not a rewrite.** `FRDGBuilder(FRHICommandListImmediate&)` at `RenderGraphBuilder.h:20` + `Execute()` at `:167` ⇒ a 4.25 SVE hook can build its own graph. `FPixelShaderUtils` (`PixelShaderUtils.h:18`, `AddFullscreenPass` `:80`), `FComputeShaderUtils` (`RenderGraphUtils.h:99`), `FRHIGPUTextureReadback`/`FRHIGPUBufferReadback` (`RHIGPUReadback.h:92`/`:75`) — **all present, all public.** Phase 0's decisive question answered well.
- 🎯 **The shader is a ONE-LINE change**, not a rewrite: only `AnomalyVisibleMask.usf:31`'s `CalcSceneCustomStencil` (absent on 4.25) → `SceneTexturesStruct.CustomStencilTexture.Load(...) STENCIL_COMPONENT_SWIZZLE` (`DeferredShadingCommon.ush:635`). Lines `:38-39` already match 4.25's idiom.
- 🎯 **`ESceneTextureId` is line-for-line identical to 5.1** — `m41`'s host-PP preflight ports unchanged.
- 🎯 **LOD accessors unchanged — zero work** (`SkinnedMeshComponent.h:673,689,693`; `StaticMeshComponent.h:176,381`; `StaticMesh.h:1073`).
- 🎯 **`bVisibleInRayTracing` present and unguarded** (`PrimitiveComponent.h:327`) — nothing to drop.
- 🎯 **`OnBackBufferReadyToPresent` has a byte-identical signature** to 5.1 — the backbuffer path ports with zero signature change.

🎯 **m45 survives, measured on three links:** the proxy takes `bRenderCustomDepth` straight from the component (`PrimitiveSceneProxy.cpp:144`, `.h:495`); `bHasCustomDepthPrimitives` is set from it alone (`SceneVisibility.cpp:2069-2071`); and the relevance gates are **disjunctions** (`:2248`, `:2704`, `:2709`). **And there is no Nanite ⇒ `G134` does not exist on 4.25.**

🆕 **NEW ITEM Phase 0 never enumerated:** 4.25's `UWorldSubsystem` is 24 lines with **no `DoesSupportWorldType`**. Our Game+PIE restriction must move to `USubsystem::ShouldCreateSubsystem(Outer)` (`Subsystem.h:61`) — a different virtual, different signature. **A correctness item, not a compile item:** dropped, the subsystems instantiate on editor-preview worlds.

Also: `WebSocketNetworking` present with source+binaries; `ERGBFormat::Gray = 2`; the tick-pin probe **returns NOT FOUND and does not misfire** on stock; `.uplugin` `EngineVersion` should stay **unset** (no build-time enforcement — `PluginDescriptor.cs:115,365`; setting it would pin to one engine).

## 4. STILL UNKNOWN FOR EUROPA — office PC only

⛔ Nothing on this bench touches these. **A stock-tree fact is evidence about the 4.x surface and no evidence about a fork.**

1. 🚨 **Is Europa's renderer modified, and does `FViewInfo::ViewRect` survive there?** *(stock: yes, `SceneRendering.h:865`, and the launcher ships private headers — that does not constrain a fork).* Highest port risk.
2. 🚨 **Does the Europa title already use custom depth/stencil?** Still the highest-value question — our `200–254` reservation depends on it.
3. Forward or deferred renderer.
4. Screen percentage / dynamic resolution; TAA method (decides if an AA-off identity arbiter exists there).
5. Is Europa's tick decoupled — read the probe echo on the first build there.
6. IoStore (`.utoc`) or pak-only.
7. 🆕 **Did the fork touch `UWorld::Tick`?** The whole 4.25 anchor argument rests on that ordering. Stock-verified, Europa-unverified.

## 5. MOUNTING + SEPARABILITY

**Junction: ✅ CONFIRMED, with evidence.**
- **UBT has zero reparse-point handling** — a recursive grep of all of 4.25's UBT for `ReparsePoint`/`IsSymlink` returns nothing. Junctions are invisible to it.
- **Outputs do not collide, measured on both sides:** 4.25 writes `Binaries\Win64\UE4Editor-*.dll` (confirmed on `Paper2D`) and `Intermediate\Build\Win64\{UE4Editor,UE4}\…`; 5.1 writes `UnrealEditor-*` and `Intermediate\Build\Win64\UnrealEditor\…`. **`.gitignore` already covers both wholesale — no change needed.**
- ⚠ Residuals: a hand-deletion of `Intermediate\` kills both engines' state; the cross-volume `E:`→`D:` junction is **unverified by execution** (owner errand 1).
- **Worktree fallback rejected unless the junction fails:** it reintroduces exactly the drift the junction removes, and collides with the standing shared-tree rule (*worktrees are never for builds*).

**Separability: ✅ `AnomalyInjector` is a LEAF — measured twice.** Build.cs declares only `Core/CoreUObject/Engine/InputCore/Foliage`; and a grep for every cross-module include/API macro/subsystem type returns **zero references**. All edges point down: `ControlServer → {Injector, Capture}`, `Capture → {Injector, Shaders(non-Shipping)}`.

**Stub mechanism: ✅ shape confirmed, implementation refined — and this is a deviation from the brief's literal wording, so I'm flagging it.** `Target.Version.MajorVersion` is valid on 4.25 (`TargetRules.cs:193`, `BuildVersion.cs:272`). **But do not hand-write a new empty stub — the exact stub already exists and every Shipping build exercises it:** `ANOMALY_CAPTURE=0` / `ANOMALY_CONTROL_SERVER=0` guard 13 `.cpp` files at the top, and **`IMPLEMENT_MODULE` sits outside the guard** (`AnomalyCaptureModule.cpp:24`), so the module loads and logs *"compiled out"*. The change is **one condition per Build.cs** — `&& Target.Version.MajorVersion >= 5` ANDed into the existing `!= Shipping` gate — **byte-inert on 5.1**. Reusing a proven switch beats adding a second, untested compile-out path beside it.

⇒ **E1-G6 becomes a code fact:** the label writers are literally not in the binary.

## 6. E1 PLAN

| # | step | where |
|---|---|---|
| 1 | Host conversion: 5 files + `.uproject` `Modules` edit, **carrying the two toolchain pins** | 4.25 project (outside repo) |
| 2 | `mklink /J` mount | owner |
| 3 | `MajorVersion >= 5` condition ×3 Build.cs (**incl. `AnomalyShaders` — `G129` makes its global shaders a fatal boot failure otherwise**) | repo |
| 4 | Subsystem base swap ×3 | repo |
| 5 | 🆕 `DoesSupportWorldType` → `ShouldCreateSubsystem` ×3 | repo |
| 6 | Tick anchor: 5.1 keeps `OnWorldTickEnd`; 4.25 binds `OnWorldPostActorTick`, **binding site only** | repo |
| 7 | Drop `bCastContactShadow` on 4.25 | repo |
| 8–9 | LOD accessors, `bVisibleInRayTracing` — 🎯 **measured no-ops** | — |

| gate | predicate | who |
|---|---|---|
| **E1-G0** *(new)* | actor name obtained via `IAI.ListActors` (injector-side, survives the stub); match on `GetName()`, not the editor-only label | **me** |
| **E1-G1** | 4.25 **editor AND game** targets both exit 0 — `Build.bat StylizedParisStreetEditor Win64 Development -Project=…` and the game target. **Only the editor target can see a missing `MODULE_API` export (`G221`)** | **me** |
| **E1-G1b** *(new)* | 🚨 **5.1 still builds, both targets** — the "one branch, two engines" gate | **me** |
| **E1-G2** | `IAI.ListAnomalies` → 9 sorted ids | **me** |
| **E1-G3** | `missing_object` hides/reverts; `IAI.DumpActive` 0 after | state **me** / **eye OWNER** |
| **E1-G4** | prove-it-can-fail: `=NoSuchActor_ZZZ` → AMB-2 refusal **and** `DumpActive` 0 | **me** |
| **E1-G5** | `IAI.Auto.FireOnce` fires + auto-reverts; same seed → same target/id across two runs | **me** |
| **E1-G6** | no artifacts **and** `ANOMALY_CAPTURE=0` in the build log + "compiled out" at runtime | **me** |

⛔ **Not in E1:** any pixel gate, mask, census or arbiter. **Step 6 changes the 4.25 label anchor and E1 does not certify it** — there are no labels to certify with. **Re-running `m40`'s alignment gate on 4.25 is an E2 entry condition** and must be pre-declared there, not discovered.

**Would not start without:** stubbing `AnomalyShaders` (fatal boot otherwise); the toolchain pins; keeping 5.1 on `OnWorldTickEnd` (moving it re-opens `P9`, closed and validated on Bates).

## 7. OWNER ERRANDS

1. 🚨 **Create the junction** (elevated): `mklink /J "E:\Unreal Projects\StylizedParisStreet\Plugins\AnomalyInjector" "D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector"` (create `Plugins\` first). **Report success** — it's the one unverified link in the mounting ruling.
2. **`E1-G3`'s eye half** — confirm `missing_object` visibly removes and `IAI.Revert` visibly restores, in the ParisStreet level.

⛔ **Nothing else.** No engine install, no VS component install, no registry edit.

## 8. NEEDS-DECISION

1. **Phase 0's NEEDS-DECISION 1 (what the 4.25 deliverable is: injection-only / +backbuffer labels / full parity) is unchanged in shape but its inputs moved materially in favour of the larger option.** The capture half's two biggest unknowns resolved well (RDG usable; shader is one line), so option (c) full parity is cheaper than Phase 0 costed — I'd revise to the **lower half of the 1.5–4 week range**. ⛔ **I would still not commit to (c) before the §4 fork questions are answered**, and none of them moved.
2. **Ratify or revert my §D.3 refinement** — the brief specified a hand-written empty stub; I implemented it as *reuse of the shipped `ANOMALY_CAPTURE=0` define* instead. Strictly less new mechanism, but it is a deviation from the literal wording and it touches three `Build.cs` files that 5.1 also builds.

Phase 0's NEEDS-DECISION 2 (does Europa use custom depth/stencil) and 3 (does the bench host 4.25) are unaffected and **answered YES** respectively.
api-equivalent cost (not billed on Max): $18.22 | turns: 104 | running total across all briefs: $523.28
