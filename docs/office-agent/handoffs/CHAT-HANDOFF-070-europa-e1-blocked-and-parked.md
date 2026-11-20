# CHAT HANDOFF — Europa E1 opened, blocked, and PARKED · bench now hosts UE 4.25
**Date:** 2026-09-03 ~17:35 local (chat session 070, "europa-e1" — thread B). **Supersedes:** `CHAT-HANDOFF-m44-m46-shipped-europa-phase0.md` only on Europa matters; everything it says about m44–m46, Bates and delivery week still stands.
**Confidentiality:** projects/hosts by INTERNAL codename only — **Concorde, Bates, Europa**. Never a real title, studio, product, packaged-exe or fork name. `StylizedParisStreet` is the owner's own marketplace bench project and needs no codename.

---

## 0. READ THIS FIRST

Three things a cold reader must know before touching anything:

1. 🚨 **The bench working tree is parked on branch `europa-e1`.** This blocked thread A's m47 run today. **Whoever takes the bench next runs `git checkout master` FIRST.** Nothing is at risk — `europa-e1` is pushed and equals its origin.
2. **Europa E1 is BLOCKED and PARKED by owner ruling** in favour of Bates delivery week. It is not abandoned; it is one design ruling away from unblocking (§3.9).
3. 🎯 **The bench now hosts UE 4.25** — that is the headline change. This answers Phase 0's NEEDS-DECISION 3 permanently.

## 1. Current state (verified at close)

- **`master` == `origin/master` == `489c29d`** `docs(europa): 4.25 bench audit and phase 1 plan`. **Docs only — no source changed on master this session.**
- **`europa-e1` == `origin/europa-e1` == `1f5e305`** `feat(europa): E1 phase A - module availability switch, verified inert on 5.1`. Pushed. **Not merged, no tag.**
- **Engine:** UE **4.25.4 CL 14469661**, **launcher install** at `E:\EpicGames\UE_4.25`. Disk C: 29 / D: 188 / E: 365 GB.
- **Bench host fixture:** `E:\Unreal Projects\StylizedParisStreet` — blueprint-only, `EngineAssociation "4.25"`, marketplace street scene. **Untouched: not converted to a code project, no `Plugins\` folder, no junction created yet.**
- **New docs:** `docs/europa/PHASE1-bench-audit-and-plan.md`; `docs/europa/PHASE0-port-surface-4.25.md` updated in place with measured 4.25 rows. **New gotchas G231–G235.**
- **E1 gates:** `E1-G1b` ✅ PASS · `E1-G1` ⛔ BLOCKED · `E1-G0, G2–G7` ⚪ UNREACHED.
- Mailbox: three GDP briefs from this thread (070-01, 070-02) plus thread A's 069-24. Running API-equivalent total ≈ **$550**.

## 2. What is now MEASURED on stock 4.25 (and the limit on that word)

⚠ **Stock 4.25 ≠ Europa.** Every row below is a *fact* about stock 4.25 and only a *prediction* for Europa's possibly-modified fork (`G120`). Chat independently verified the load-bearing ones against the engine tree; do not re-derive them.

**Verified twice (Code + chat, citations exact):**
- 🚨 `FWorldDelegates::OnWorldTickEnd` **ABSENT** — `World.h` has only `OnWorldPreActorTick:3676` / `OnWorldPostActorTick:3679`.
- ✅ `OnWorldPostActorTick.Broadcast` at `LevelTick.cpp:1694`, inside `if (bDoingActorTicks)` at `:1690`; `FTickableGameObject::TickObjects` at `:1630` inside the same guard opened `:1545` ⇒ **it fires after every tickable, still pre-draw — an m40-anchor-equivalent position.**
- ✅ `FRDGBuilder(FRHICommandListImmediate&)` public at `RenderGraphBuilder.h:20` ⇒ **the mask pass is a PORT, not a rewrite.**
- 🔻 `bCastContactShadow` **ABSENT** from `PrimitiveComponent.h` (Phase 0 graded it "PRESENT (likely)"). 7 of m45's 8 hide flags present.
- ✅ `bVisibleInRayTracing:327`, `bRenderInMainPass:331`, `bRenderInDepthPass:335`, `bRenderCustomDepth:516`.
- ✅ `UWorldSubsystem` (`Engine/Public/Subsystems/WorldSubsystem.h`) is **24 lines, derives from `USubsystem`, and has NO `DoesSupportWorldType`.**

**Code-measured, not independently re-checked:** `FPixelShaderUtils`, `FComputeShaderUtils`, `FRHIGPUTextureReadback`/`FRHIGPUBufferReadback` all present and public · `ESceneTextureId` identical to 5.1 · LOD accessors unchanged (zero work) · `OnBackBufferReadyToPresent` byte-identical signature · the mask shader is a **one-line** change (`CalcSceneCustomStencil` → `SceneTexturesStruct.CustomStencilTexture.Load(...) STENCIL_COMPONENT_SWIZZLE`) · `WebSocketNetworking` present · `ERGBFormat::Gray = 2` · the TICKPIN probe returns NOT FOUND and does not misfire · **no Nanite ⇒ `G134` does not exist on 4.25.**

**Toolchain:** VS2017 BuildTools 15.9 / MSVC **14.16.27023 (Version 14.16.27051)** present and on 4.25's own preferred list; Windows SDK pin `10.0.17763.0`. **No installs needed.**

## 3. Decisions this session, with the WHY (this is the part that is nowhere else)

1. **The bench fixture is not codenamed.** `StylizedParisStreet` is the owner's own marketplace project on his home PC — it is the 4.25 analogue of StackOBot. Codename discipline covers office titles and hosts only.
2. **Junction, not copy, not worktree.** The 4.25 project's `Plugins\AnomalyInjector` will be a Windows **directory junction** to the single repo tree. One codebase enforced by the filesystem. Verified: UBT has zero reparse-point handling, and 4.25 (`UE4Editor-*`, `Intermediate\Build\Win64\UE4Editor\`) and 5.1 (`UnrealEditor-*`, `…\Win64\x64\UnrealEditor\`) outputs do not collide; `.gitignore` already covers both. Worktree rejected — it reintroduces the drift the junction removes.
   🚨 **`mklink /J` needs NO elevation** (only `/D` symlinks do) — Code's plan said otherwise; corrected. 🚨 **Deleting the junction with `rmdir /S` recurses into the target and would destroy the repo** — use `rmdir` (no `/S`) or `fsutil reparsepoint delete`.
3. **Compile-out on 4.25 reuses the shipped `ANOMALY_CAPTURE=0` / `ANOMALY_CONTROL_SERVER=0` switch** rather than a hand-written stub — Code's refinement, ratified over chat's original wording. **Why:** it is a path every Shipping build already exercises; `IMPLEMENT_MODULE` sits outside the guard so the module still loads and logs *"compiled out"*, making `E1-G6` **observable rather than merely absent**; and one ANDed condition is byte-inert on 5.1. Adding a second untested compile-out path beside a proven one is the worse trade.
4. **Tick anchor: 5.1 keeps `OnWorldTickEnd`; 4.25 binds `OnWorldPostActorTick`, at the binding site only.** Moving 5.1's anchor would re-open **P9**, which is closed and validated on Bates. No `#if` inside the handler or any gate-bearing algorithm.
   📌 **Pre-declared: re-running m40's label-alignment gate on 4.25 is an E2 ENTRY CONDITION**, written as a gate, not a note. E1 cannot certify it — there are no labels in E1.
5. **`DoesSupportWorldType` → `USubsystem::ShouldCreateSubsystem`** (4.25's `UWorldSubsystem` lacks the former). This is a **correctness** item, not a compile item: dropped, the subsystems instantiate on editor-preview worlds. It gets its own gate, **E1-G7**, graded `PASS (game leg) / PENDING (editor leg)` because the editor leg needs a human to launch the editor once.
6. **`E1-G1b` demands POSITIVE proof, not absence of failure:** the 5.1 build log's generated defines must still read `ANOMALY_CAPTURE 1` etc. "It compiled" is not evidence that a new condition is inert.
7. **`AnomalyShaders` is stubbed on 4.25 prophylactically**, and its justification is recorded as a **PREDICTION, not a measurement** (`G120`): a global-shader module at `PostConfigInit` failing to compile its `.usf` would be a fatal boot; that failure was never measured on 4.25 and no build cycle was spent to.
8. **Branch discipline: nothing merges to `master` until Bates ships.** Master is the delivery-week asset.
9. 🎯 **THE WALL-1 DESIGN — the most important thing in this document, and it exists nowhere else yet.**
   **The blocker as Code found it:** 4.25 has no `UTickableWorldSubsystem`; five classes derive from it. UHT refuses `UCLASS` inside `#if` (except `WITH_EDITORONLY_DATA`) and will not expand a macro base class (`Unable to find parent class type named 'ANOMALY_TICKABLE_WORLD_SUBSYSTEM'`). So the base token must be **literal and identical on both engines**. Code concluded the only way out changes 5.1's base class and correctly stopped.
   **Chat's resolution — put the shim one level LOWER.** Do not try to vary the parent. Instead **clone 5.1's `UTickableWorldSubsystem` as our own class**, built on bases that exist identically in *both* engines:
   ```
   UCLASS(Abstract)
   class UAnomalyTickableWorldSubsystem : public UWorldSubsystem, public FTickableGameObject { … }
   ```
   `UWorldSubsystem` exists in 4.25 (verified) and 5.1; `FTickableGameObject` exists in both; 5.1's own `UTickableWorldSubsystem` has exactly this shape (body quoted at `WorldSubsystem.cpp:46-80`). The five subsystems then name **one literal parent, identical on both engines, with no conditional compilation anywhere near a `UCLASS`.**
   ⚠ **It is still a 5.1 change** — the five subsystems stop deriving from the engine's class and derive from our verbatim clone. Expected to be behaviour-neutral; **that expectation must be gated, not assumed.**
   ⚠ Also measured en route (`G232`): the two UHTs treat an unknown `#if` **asymmetrically** — 5.1 skips it (`UhtHeaderFileParser.cs:888`), 4.25 **hard-errors** (`HeaderParser.cpp:3517`). Never rely on 5.1's tolerance.
10. **Scoped waiver of failure-policy (iii):** the change in §3.9 may be made **on `europa-e1` only**, with the **full 5.1 re-gate run on that branch** (tick-order legs, m40 alignment, a `speed_ratio`/throughput check, and `E1-G1b` re-run) — because it touches the tick machinery where **P9** lived. `master` stays frozen; merging is a separate decision after Bates ships. **Rationale:** what delivery week protects is `master`; work on a branch does not endanger it. This overrides Code's recommendation to wait entirely.
11. **Blocker 2 — the machine-global compiler pin — is NOT to be touched while another thread is doing toolchain or editor-build work.** `%APPDATA%\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml` pins `CompilerVersion 14.38.33130`; it is per-user (not per-engine), 4.25 cannot use that toolset, it **beats `Target.cs`**, has **no command-line form**, and the engine-scoped `Engine\Saved\…` file **also loses** (`XmlConfig.cs:215` vs `:227`). Code proved this, then deleted its own experiment rather than leave something that looks like it works (`G234`).
    **Mechanisms to evaluate next, in order:** (a) read both engines' `XmlConfig.cs` load order and find a location that legitimately wins or is per-engine — note `My Documents\Unreal Engine\UnrealBuildTool\` loads *after* `%APPDATA%`; (b) a **process-scoped** `APPDATA` override for the 4.25 build invocation only, which leaves nothing behind and cannot affect another process. ⛔ **Explicitly rejected: swap-and-restore of the global file** — a crash mid-build leaves the wrong compiler pinned and silently breaks the other thread's 5.1 builds.
12. **Priority ruling (owner, confirmed): Bates > Europa in delivery week.** E1 parked. The contended resource is not git — it is the **one physical bench** (which branch is checked out, plus the staged exe and cooked container).

## 4. Forward plan, in order

1. **Whoever takes the bench next: `git checkout master`.** Nothing is at risk.
2. Thread A finishes **m47** (editor-build shader readiness / black-frame gate). Bates delivery continues per the previous handoff: Section F reads → G-R7(ii) physical gate → client cook → ship.
3. **E1 unblock brief (070-03), effort xhigh, on `europa-e1` only:** implement §3.9's clone-parent shim; move the world-type restriction to `ShouldCreateSubsystem`; then the **full 5.1 re-gate on the branch** per §3.10. Nothing involving 4.25 in this brief.
4. **070-04:** resolve the compiler-pin mechanism per §3.11(a) then (b); create the junction (Code, no elevation); convert the host project (5 files + `.uproject` `Modules`, carrying both toolchain pins, original backed up as `.uproject.bak`); 4.25 editor + game builds; gates `E1-G0, G1, G2, G4, G5, G6, G7`.
5. **Owner legs:** `E1-G3`'s eye half (an anomaly visibly fires and reverts in the ParisStreet level) and `E1-G7`'s editor leg (launch the 4.25 editor once; Code reads the log after).
6. **E2 entry:** m40's alignment gate re-run on 4.25 (§3.4) before any label or capture work is graded there.
7. **Merge decision** for `europa-e1` → `master`: after Bates ships. Not before.

## 5. Open vs locked

**🔴 Open — owner (Europa, office PC only; nothing on the bench can answer these):**
1. **Does the Europa title already use custom depth/stencil?** Highest value — the plugin reserves stencil `200–254` and m26's collision detectors assume nothing else writes there.
2. **Is Europa's renderer modified, and does `FViewInfo::ViewRect` survive there?** (Stock: `SceneRendering.h:865`, and a launcher install does ship private headers — neither constrains a fork.) Highest port risk.
3. Forward or deferred · screen percentage / dynamic resolution · TAA method · is the tick decoupled (read the probe echo on the first Europa build) · IoStore or pak-only · 🆕 **did the fork touch `UWorld::Tick`?** — §2's whole anchor argument rests on that ordering.

**🔴 Open — owner (product):** the **4.25 deliverable** — (a) injection only / (b) injection + backbuffer capture + labels, no masks / (c) full parity incl. masks. **Deferred until the questions above are answered.** Chat's standing recommendation is (b) first with (c) built on top; Code revised (c)'s cost *down* to the lower half of 1.5–4 weeks after the RDG and shader answers came back well. E1 is identical under all three, so this gates nothing today.

**Open — technical:** the compiler-pin mechanism (§3.11); the 5.1 re-gate result for §3.9; everything from the previous handoff (m39 honest bbox, m42 persist tags, within-frame comparator, mask handshake hygiene, fog-card cause, watcher archive-at-launch).

**Locked (added this session):** bench hosts 4.25 · junction mounting and its `rmdir /S` footgun · `ANOMALY_CAPTURE=0` reuse as the 4.25 compile-out · 5.1 keeps `OnWorldTickEnd` · m40 re-run as an E2 entry condition · `ShouldCreateSubsystem` as the shared world-type path · `E1-G1b` positive-proof form · no merge to master until Bates ships · Bates > Europa in delivery week.

## 6. Corrections — discard these

- ~~"`GetModuleDirectory` exists in 4.25's `ModuleRules`"~~ (Phase 0 §5) → **it does not**; `CS0103`. Fixed with `#if UE_5_0_OR_LATER`. Lesson `G233`: **`Build.cs` is C#, so a runtime `if` does not protect an API that does not exist — every branch must compile.**
- ~~"4.25 has `PrePostProcessPass_RenderThread`"~~ → **absent.** 4.25's SVE hooks are `PreRenderViewFamily / PreRenderView / PostRenderBasePass / PostRenderViewFamily / PostRenderView_RenderThread`, all taking `FRHICommandListImmediate&`, none an `FRDGBuilder`.
- ~~"`bCastContactShadow` present (likely)"~~ → **absent.**
- ~~"the junction needs elevation"~~ → it does not.
- ~~"an MSVC toolset's folder name is its version"~~ → `MSVC\14.16.27023` reports `Version=14.16.27051`; Microsoft services these in place (`UEBuildWindows.cs:621`). `G235`.
- ~~"a per-engine base class is impossible"~~ → dissolved by §3.9. The impossibility was only ever at `UTickableWorldSubsystem`; at `UWorldSubsystem` it disappears.
- ~~"masks/labels need the 4.25 tick anchor changed in E1"~~ → **no E1 change at all**: `AnomalyCaptureSubsystem.cpp` guards its `OnWorldTickEnd` bindings (`:347` encloses `:349-350`; `:548` encloses `:557,562`), so at `ANOMALY_CAPTURE=0` none is compiled.

## 7. Working notes for the next chat

- **Two chat threads share this mailbox** (thread A "069 m47" on the 5.1/Bates track, thread B "070 europa-e1"). It works: briefs interleave, a side thread marks its brief `# SESSION: new` so it is not adopted as the mailbox default, and each names the other. **The contention is the physical bench, not git** — and git alone cannot show whether the other thread is still live there.
- **The mail-signal wake has a 0–5 minute lag** — GitHub's raw CDN cache. Measured again today at ~5 min. **That is the design; do not debug it.**
- ⛔ **Never run git *write* commands against `D:\IntrusiveAnomalies` from a cloud chat session's Linux mount.** The repo is Windows/CRLF, so git there reports every file modified and a checkout would rewrite the working copy. Chat reads only; Code does all git work on Windows.
- Chat now has read access to `E:\EpicGames\UE_4.25\Engine\Source` and can check Code's engine citations directly. **Recursive greps over that tree across the link time out at 120 s — use targeted file reads.**

## 8. Pointers

`docs/europa/PHASE1-bench-audit-and-plan.md` (the full audit + gate table) · `docs/europa/PHASE0-port-surface-4.25.md` (now carrying measured 4.25 rows) · gotchas **G231–G235** · `CLAUDE.md` status block · journal 070 · the previous handoff `CHAT-HANDOFF-m44-m46-shipped-europa-phase0.md` for everything Bates and m44–m46 · `claude/mailbox-protocol.md` · `E:\MailBox\README.md`.
