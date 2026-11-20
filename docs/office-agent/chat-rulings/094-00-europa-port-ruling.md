# 094-00 — Chat ruling: J2 is paused; the Europa (UE 4.25) port opens; binding decisions for the Phase A plan

**Date:** 2026-10-16, about 10:45 IST · **Author:** chat Claude (orchestrator)

**Inputs:**
- **Owner, 10-16 about 10:15 IST:** "We need to stop this research for now and work on the 4.25 porting on priority. I had downloaded the StylizedParisStreet for 4.25 at E:\Unreal Projects\StylizedParisStreet. I have stopped the Watcher."
- **The 092-02 port study** (`_reviews\092-02-codex-europa-port-study.md`, Codex Astra max, read-only, audited release `d7eee97`) and its ruling (`_reviews\092-02-ruling.md`, Claude Code Desktop, 10-05). Both are accepted research.
- **History:** `CHAT-HANDOFF-070-europa-e1-blocked-and-parked.md` (09-03) and `_reviews\091-00-code-desktop-orchestrator-STATE.md` (10-05).
- **Chat's own reads today**, via the device link:
  - The converted host project is as 091-00 describes.
  - The editor has never been opened on it: there is no `Saved\` folder.
  - Free space: C: 13.1 GiB, D: about 160 GiB, E: about 51.5 GiB.

## Verdict
1. **J2 (`fix/followup-capture`) is paused, not abandoned.**
   - Nothing is pushed: remote `1bf7054`, local `7754fd2`.
   - The pause state and how to resume are in `CHAT-HANDOFF-093-J2-paused-europa-port-opened.md`.
2. **The Europa port is now the priority** (owner's call).
   - Europa is the 4.25 office title. Its engine is called only "Europa's fork".
   - Phases A–F follow 092-02 §8.
   - **This ruling authorizes the Phase A plan (094-01) only.** Implementation waits for chat's ruling on that plan.

## Binding decisions

### P1. Phases and the first step
- The plan shape is 092-02 §8, with each phase's exit decision as written there.
- **Phase A (narrow technical proof) is the only phase planned in detail now.**
  - The plan sketches Phase B.
  - It lists what C–F would need, for the owner's target decision.
- The owner's target call (how far the first Europa delivery goes) is asked separately. Phase A is identical under every answer, so nothing waits for it.

### P2. Base: `release/2026-10-05` at `d7eee97`
- The new branch is **`feat/europa-port`**, cut from `d7eee97`.
- **Why `d7eee97`:**
  - It is the build the client has (M3).
  - It is fully qualified.
  - It is the exact tree 092-02 audited.
- **Not these, and why:**
  - **`fix/followup-capture`:** J2 is unqualified and paused.
  - **`master`:** stale.
  - **`europa-e1` (`1f5e305`):** 415 commits behind. Its module-availability changes are source material only. The plan decides, with evidence, whether to cherry-pick or re-implement them.
- Later merges into the port branch are deliberate, separate steps. That covers J1/J2 once qualified, and any later delivery.
  - The plan reports the merge-base and the expected conflict surface with `fix/followup-capture` (`git diff --stat d7eee97..7754fd2`), so the cost is known now.

### P3. One source tree; 5.1 behaviour unchanged, and proven
- **One tree for both engines.** The port branch builds on 5.1 and on 4.25 from one tree. This is the plugin's game-agnostic promise, and it is how one codebase keeps serving 5.1 hosts.
- **Coding rules** (092-02 §2–§3, G232):
  - Use exact engine-version predicates (4.25 and 5.1, named explicitly).
  - No `#if` around any reflected declaration.
  - No macro base classes.
  - Engine-specific code goes in `.cpp` files or non-reflected `Compat` helpers.
  - The 5.1 call paths stay as they are.
- **5.1 neutrality is a gate result, never an argument.** "The 5.1 path is in `#else`" proves nothing (092-02 §3).

### P4. The tickable shim: 092-02 §2's design, not a verbatim clone
- **The class:** `UAnomalyTickableWorldSubsystem : public UWorldSubsystem, public FTickableGameObject` is the one literal parent on both engines.
- **The 4.25 gap:** stock 4.25 never calls `IsAllowedToTick`. So 4.25 needs its own initialized and CDO gate, through `IsTickable` and the tick type.
- **On 5.1 it must keep, exactly:**
  - the engine class's `IsAllowedToTick`;
  - the CDO and initialized-state behaviour;
  - `GetTickableGameObjectWorld`;
  - stat identity;
  - Initialize/Deinitialize chaining.
- **World type:** the restriction moves to `ShouldCreateSubsystem`. It keeps Super's decision and Game/PIE-only creation.
- **The plan must also cover:**
  - an audit of type-ancestry consumers, because a cast to `UTickableWorldSubsystem` stops matching;
  - double registration, PIE and editor worlds, multiple worlds, pause, travel and teardown.

### P5. The plugin descriptor uses the legacy keys, proven on both engines
- **The keys:** `WhitelistTargets` and `BlacklistTargetConfigurations` (and their siblings).
- **Why:** on 4.25, today's `TargetAllowList` and `TargetConfigurationDenyList` are silently ignored, so the Shipping exclusion would not apply (092-02 ruling, check 3).
- **Positive proof on both engines:**
  - **4.25:** the excluded modules are absent from the build outputs.
  - **5.1:** the module set for each target and configuration is identical to the release's.

### P6. Shared-code mechanics
- **Pointers:** `TObjectPtr` members become raw `UPROPERTY` pointers in shared reflected code. GC reachability is kept.
- **Missing APIs:** UE5-only containers and APIs go behind compat helpers.
- **Includes:** at the use site.
- **Strict include build:** `tools/strict_include_build.py` gets the 4.25 adaptations listed in 092-02 §6:
  - no `IncludeOrderVersion`;
  - `UE4Editor` paths;
  - an explicit engine root.
  It runs only in the port's own checkouts.
- **C++ standard:** choose one, and validate it on MSVC 14.16. 5.1's selection stays unchanged.

### P7. Phase A scope on 4.25
- **Injector core only.**
- **Compiled out on 4.25:** capture, the control server, shaders and the bench module, through the shipped `ANOMALY_CAPTURE=0` / `ANOMALY_CONTROL_SERVER=0` path (070 §3.3). `AnomalyShaders` is stubbed (070 §3.7).
- **Observable:** the plugin still loads and logs "compiled out".
- **The Phase A smoke:** the subset of 092-02's six anomalies that needs no plugin-authored asset.
- **Phase B:** the six anomalies, including 4.25-authored materials. Assets saved by 5.1 cannot load in 4.25.
- **Outside A and B until the owner's target call:** time_dilation, camera_clipping, lod_popping and the three texture anomalies. The plan says in one line what each would need.

### P8. Hosts and disk
- **The 4.25 bench host is a copy on physical D:** of the converted `E:\Unreal Projects\StylizedParisStreet`.
  - Copy it without its `Binaries`, `Intermediate` and `Saved` folders. The plan proposes the path, for example `D:\IA_Europa\StylizedParisStreet`.
  - The E: original stays **untouched**: it is the owner's copy and the reference.
  - **Why:** E: is about 1.5 GiB above its 50 GiB floor, and C: has about 13 GiB. The first editor launch alone writes shader caches and DDC.
- **The port's plugin folder** in that copy is a `git worktree` of `feat/europa-port`. Rule 5: a worktree, never a junction.
- **4.25 caches stay on physical D:.** DDC, shader caches, logs and builds all land there. The mechanism must be process-scoped or project-scoped only: no per-user, machine or global setting changes.
- **The 5.1 re-gate host:** a new host on physical D: with its own worktree of `feat/europa-port`, never a J2 host (P9). If the plan proposes a cheaper arrangement, it must explain how it leaves P9 untouched.
- **Disk floors:**
  - D: and E: stay at 50 GiB or more, measured on the **resolved** volume.
  - C: stays at 10 GiB or more.
  - A breach is a STOP.

### P9. J2 is frozen for a clean resume
None of this may change:
- **Worktrees:**
  - `_r933` (`fix/followup-capture`), `_r53_src` and `_r917` (the release);
  - `_r53_host` and its host project;
  - the Lyra host and its plugin worktree.
- **The main checkout:** `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector` stays on `m51` with no changes, and the StackOBot `Config\` files stay as they are.
- **The staging folder:** `StackOBot\Builds`.
- **Evidence:** the banks, the archives, the J2 run folder and `D:\IA_BankOverflow\_r9315`.

How it is checked:
- The first port job records this state.
- Every port job ends by proving it unchanged.

### P10. 4.25 builds use the Target.cs route
- **The recipe** (092-02 ruling, proven 10-05; already in the converted host): VS2017, `CompilerVersion = "14.16.27051"`, SDK `10.0.17763.0`, and `bOverrideBuildEnvironment = true` in both targets.
- **Never edited:** the per-user `BuildConfiguration.xml`.
- **Not needed:** the XmlConfigCache wrapper.
- **Proof from the 5.1 build log:** 5.1 still selects its own toolset (14.38.33130).

### P11. Phase A gates (the minimum)
Every gate needs a can-fail.

**On 4.25:**
- **Builds:** Editor and Game build with exit 0; warnings counted.
- **Strict includes:** the strict-include pass, with unity and PCH off.
- **Loading:** modules load in the editor and the game, with no fatal error at boot.
- **Compile-out:** visible in the log.
- **Descriptor:** the exclusion proof (P5).
- **World type:**
  - subsystems are created in Game/PIE worlds only;
  - the editor-preview leg is the owner launching the 4.25 editor once.
- **Ticks:**
  - each subsystem ticks once per frame;
  - nothing ticks before Initialize or after Deinitialize;
  - pause, travel and PIE-end teardown behave.
- **Smoke:** each Phase A anomaly, per anomaly:
  - is applied to a visible target and reverts fully;
  - attempted/applied/refused/reverted counts are recorded;
  - residue is 0.
  - **Picture evidence:** engine screenshots before, during and after, with pixel diffs. This is not a label-grade check.
  - **Plus** the owner's eye check.

**On 5.1** (the shim, descriptor, pointers and includes all touch 5.1):
- **A reach analysis first:** every changed 5.1 file, and every behaviour it can reach.
- **Builds:** every release target builds, with warnings no worse than the release.
- **Strict include:** the strict-include pass still passes.
- **E1-G1b positive proof:** the generated defines still read `ANOMALY_CAPTURE 1` and so on.
- **Toolset:** 5.1's compiler selection is unchanged.
- **Descriptor:** the positive proof (P5).
- **Worlds and ticks:** subsystem worlds and tick counts are correct.
- **Tick order:** both tick-order legs, plus m40's pixel alignment, with its known-bad lever failing.
- **Speed:** speed_ratio matches the release baseline.
- **Label sync on every delivered anomaly**, in PIE: both edges 0 frames off, measured from pixels.

**Staged on 5.1:**
- A staged run on 5.1 is required **before any 5.1 delivery or merge** from this branch.
- The plan says which staged checks Phase A itself needs. For example, the descriptor proof may need a packaged build.

### P12. Process
- **Who does what:**
  - Codex implements later, through the implementer Relay.
  - Claude Code orchestrates and reviews.
- **One bench, one implementer.**
- **Carried rules:**
  - W3 and W4;
  - D2 (at most 2 harness attempts per problem);
  - G201/K7 (gates bind to the final binary);
  - predictions frozen before legs;
  - a can-fail for every gate;
  - no vacuous controls;
  - a control that cannot be built is a STOP.
- **Timing:**
  - Builds may run while the owner uses the PC.
  - Editor and game legs wait for the idle gate.
  - The first 4.25 editor launch (shader compile) is an idle-time leg of its own.

### P13. Confidentiality
- **Names:**
  - "Europa" is the office title.
  - "Europa's fork" is its engine.
  - StylizedParisStreet needs no codename.
- **Office data never comes home.** What does come home from Europa is numbers, 0/1 flags and sanitized error text that the owner types himself (092-02 §6 return format).
- No real title, studio or fork name appears anywhere.

### P14. An early Europa read card
- **What:** the plan includes a short, read-only card the owner can run at the office in week 1. It turns the riskiest fork unknowns into numbers early.
- **What it covers:**
  - version and CL;
  - compiler and SDK;
  - whether the fork has `OnWorldPostActorTick`, `ShouldCreateSubsystem` and the legacy descriptor keys;
  - the SceneViewExtension hook list;
  - forward or deferred;
  - the AA method;
  - screen percentage and dynamic resolution;
  - whether the project uses custom depth/stencil;
  - whether simulation and render are decoupled.
- **How it is answered:** with numbers and 0/1 only, per P13.

## Next
- **094-01 is queued:**
  1. Codex writes the Phase A plan, read-only, through the reviewer Relay.
  2. Claude Code reviews it against P1–P14.
  3. Chat rules.
- **Then:** 094-02, the Phase A implementation (Codex implementer Relay, Code reviews after).
- **J2 resumes only on the owner's word**, per the pause handoff.
