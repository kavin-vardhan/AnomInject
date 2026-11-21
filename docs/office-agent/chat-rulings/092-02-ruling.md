# 092-02 ruling: Codex port study, AnomalyInjector to Europa (UE 4.25 fork)

Date: 2026-10-05. Author: Claude Code Desktop (orchestrator). Report: `_reviews\092-02-codex-europa-port-study.md` (run `_relay\runs\2026-10-05-092-02-europa-port-study\`; Astra, max effort, read-only, 2,751 s, exit 0). I read the full report, including the include ledger. Europa's engine is referred to only as "Europa's fork".

**Disposition: ACCEPTED as research.** Verdict accepted: **feasible in stages, not an easy full port.** No port is authorized by this ruling; that is the owner's decision.

## Independent checks against stock 4.25.4 source
1. **Capture hooks.** `SceneViewExtension.h:116–136` offers only Pre/PostRenderViewFamily, Pre/PostRenderView and PostRenderBasePass, on `FRHICommandListImmediate`. There is no post-processing-pass subscription. Today's colour grab (VisualizeDepthOfField subscription) and the mask's Tonemap subscription both need a new 4.25 backend [confirmed].
2. **WebSocket transport.** The 4.25 WebSocketNetworking plugin has only private `WebSocketServer.h`/`WebSocket.h` and no public server interface [confirmed]. The control server, and therefore the new dashboard, needs a new transport on 4.25. Until then Europa would be console-only.
3. **Descriptor keys.** 4.25 reads `WhitelistTargets`/`BlacklistTargetConfigurations` (`ModuleDescriptor.cpp:180`, `:185`). Our `.uplugin` uses `TargetAllowList`/`TargetConfigurationDenyList` (lines 22–47) [confirmed]. On 4.25 the Shipping exclusion would silently not apply. This is correctness-critical: use the legacy keys, which 5.1 accepts as a fallback.

## Build mechanism: superseded by a proven route
The report proposes a disposable `-XmlConfigCache` wrapper and notes the Target.cs route as "plausible, not compiled". **The Target.cs route is now proven on this PC** (13:56 IST): the bench editor built with VS2017 14.16.27051 and SDK 10.0.17763.0, and the global xml is untouched.
- Recipe: pin the cl.exe product version 14.16.27051 and set `bOverrideBuildEnvironment = true` in both targets.
- The flag is needed because a launcher engine's `ValidateSharedEnvironment` builds the UE4Editor base rules, which inherit the global pin.
- **Ruled:** use the Target.cs route for 4.25 hosts. It also covers editor-triggered builds, which a wrapper would not. The cache wrapper is not needed.
- **Caveat:** with `bOverrideBuildEnvironment`, the bench target must not change shared-environment settings beyond the compiler.
- Codex independently confirmed the cl.exe product-version detail and corrected the old "XML beats Target.cs" claim.

## Accepted plan shape and estimates
Codex's estimates, INFERRED; office latency and fork surprises excluded:
- **A, narrow proof (3–7 days):**
  - the literal UHT tick-subsystem shim;
  - common descriptor, pointer and include changes;
  - genuinely excluding unported files;
  - a basic injection compile and smoke.
- **B, six basic anomalies (+2–5 days):**
  - missing_object, blinking, lighting_mismatch, lod_corruption, missing_texture and corrupted_texture;
  - 4.25-authored materials.
- **C, trustworthy capture, masks and labels (8–15 days):**
  - new 4.25 grab backend, family-receipt pairing, scene-texture/stencil adapter and pitch-aware readbacks;
  - re-run m40 alignment, mask, cadence, transitions and change evidence.
- **D, first office integration (2–5 days + latency):** unbounded if the fork changed hooks.
- **E, texture parity (10–20 days):** all three are partial rewrites; hold needs a redesigned streaming-state adapter.
- **F, transport/dashboard, packaging and remaining parity (5–10 days).**
- Totals: about **3–5 engineer-weeks** for a dataset-worthy six-anomaly subset, and **6–10+ weeks** for parity.
- 5.1 must re-prove everything the shared shim and descriptor touch. "The 5 path is in `#else`" is not evidence.

## Orchestrator assessment for the owner's decision
- The Europa pivot is **not a shortcut** for the three texture anomalies: Europa needs A through E (6–10+ weeks) before they work there. The Bates VT routes from 092-01 are the smaller path for those three: blurry Proof A takes 2–4 days and 1–3 weeks to harden; UV/normal producer work is estimated at 3–8 weeks.
- A Europa port makes sense if Europa itself is wanted (another client title, VT off, no Nanite). It is not cheaper.
- If the port goes ahead:
  - work in a dedicated worktree/branch, not a junction into the frozen release or the main checkout;
  - the converted bench (`E:\Unreal Projects\StylizedParisStreet`) is the native stock 4.25 host;
  - Phase A carries the descriptor-key fix and the tick shim, with 5.1 re-gates;
  - `tools/strict_include_build.py` needs 4.25 adaptations: no IncludeOrderVersion, UE4Editor paths, an explicit engine root.

Chat review status: per AGENTS.md this is Codex's convention. Implementation would change the 5.1 code (shim, descriptor), so it needs the owner's go-ahead and a reviewed brief. This read-only ruling needed none.
