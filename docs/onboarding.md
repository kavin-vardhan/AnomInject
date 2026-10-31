# Onboarding

## What this is
`AnomalyInjector` is a UE5 plugin that injects **labeled visual anomalies** (graphics
bugs) into UE5 games to produce synthetic training data for bug-detection ML. It is
game-agnostic and tested on the Stack O Bot sample. See [../CLAUDE.md](../CLAUDE.md) for
the full project framing, the **Current status**, and the locked architecture. The living
current-state design + anomaly catalog is [architecture.md](architecture.md).

## How the work is run
- **Two-Claude split.** An orchestrating "chat Claude" makes design decisions; Kavin
  (project owner) ferries them to the implementing Claude, which writes the code. When the
  implementer hits a real design fork, it stops and surfaces the ambiguity rather than
  guessing.
- **Plan-before-code.** Each milestone starts with a file-by-file plan that must be approved
  before any code is written.
- **Doc discipline.** Each session produces a journal in `docs/sessions/`. The setup runbook
  and architecture.md are kept current as living documents; gotchas are append-only. The full
  maintenance protocol (what every milestone must update) is in `../CLAUDE.md`.

## Where things live
```
Plugins/AnomalyInjector/            <- this plugin = its own git repo, single source of truth
├─ AnomalyInjector.uplugin          <- Runtime module, LoadingPhase Default, EnabledByDefault
├─ CLAUDE.md                           <- canonical context + Current status (read first)
├─ docs/                               <- onboarding, architecture, runbook, gotchas, session journals
└─ Source/AnomalyInjector/
   ├─ AnomalyInjector.Build.cs      <- deps: Core, CoreUObject, Engine, InputCore (InputCore = m5: FKey/EKeys; no Slate/UMG; m6 added none)
   ├─ Public/
   │  ├─ AnomalyInjectorLog.h       <- LogAnomaly category
   │  ├─ IAnomaly.h                 <- the locked anomaly interface (plain C++, not a UCLASS)
   │  ├─ AnomalyTargeting.h                <- FindActorsMatching (+ "=" exact-match sentinel) + FindComponentsMatching<T> (A1)
   │  ├─ AnomalyArgs.h                     <- GetFloat/GetInt/GetString (A3, shared arg parse/clamp/warn)
   │  ├─ AnomalyLod.h                      <- forced-LOD dispatch over static+skeletal meshes (M3, shared LOD helper)
   │  ├─ AnomalyViewport.h                 <- frustum+occlusion visibility tests over an explicit view (viewport milestone)
   │  ├─ AnomalyInjectorSubsystem.h <- UAnomalyInjectorSubsystem (manager + registry + viewport-scoping toggle)
   │  ├─ AnomalySelectorSubsystem.h <- UAnomalySelectorSubsystem (m5: select visible object + inject UI; separate subsystem)
   │  └─ AnomalyAutoInjectorSubsystem.h <- UAnomalyAutoInjectorSubsystem (m6: auto-fire on-screen + auto-revert; separate subsystem)
   └─ Private/
      ├─ AnomalyInjectorModule.cpp  <- module boilerplate + log category definition
      ├─ AnomalyTargeting.cpp
      ├─ AnomalyArgs.cpp
      ├─ AnomalyLod.cpp                    <- AnomalyLod impl (Cast<>-dispatched static/skinned forced-LOD)
      ├─ AnomalyViewport.cpp               <- AnomalyViewport impl (reversed-Z frustum + line-trace occlusion + live resolver)
      ├─ AnomalyInjectorSubsystem.cpp <- lifecycle, heartbeat, registry, dispatch, console commands, viewport-scoping toggle
      ├─ AnomalySelectorSubsystem.cpp <- m5: selection model + raw input poll + immediate-mode HUD + IAI.Selector.* commands
      ├─ AnomalyAutoInjectorSubsystem.cpp <- m6: seeded scheduler (AdvanceTime/TryFireOnce) + raw input poll + HUD + IAI.Auto.* commands
      └─ Anomalies/                    <- one IAnomaly impl per file (7 anomalies)
         ├─ Anomaly_MissingObject.{h,cpp}   <- static, actor-scoped
         ├─ Anomaly_Blinking.{h,cpp}        <- ticking, actor-scoped
         ├─ Anomaly_TimeDilation.{h,cpp}    <- world-global, no tick
         ├─ Anomaly_LightingMismatch.{h,cpp}<- component-scoped (lights), per-target capture
         ├─ Anomaly_LodCorruption.{h,cpp}   <- component-scoped (static + skeletal mesh), forced-LOD (AnomalyLod)
         ├─ Anomaly_LodPopping.{h,cpp}      <- ticking, component-scoped (static + skeletal), LOD pop (AnomalyLod)
         └─ Anomaly_CameraClipping.{h,cpp}  <- global near-clip, console-command capture/restore
```
Host-only scaffolding lives **outside** the plugin, in the StackOBot project:
`Source/StackOBot/` (minimal game module) + `Source/*.Target.cs`. This is what makes the
otherwise Blueprint-only StackOBot a buildable code project; it is not part of the plugin.

## The control surface (console commands)
Open the console in PIE (`` ` `` backtick) and run:
- `IAI.ListActors` — log every actor as `Class | Name | Label` (targeting aid).
- `IAI.ListAnomalies` — list registered anomalies as `id - description - usage`.
- `IAI.Apply <id> <args...>` — apply an anomaly, e.g. `IAI.Apply missing_object SatelliteDish`,
  `IAI.Apply blinking SatelliteDish 3`, `IAI.Apply time_dilation 0.2`,
  `IAI.Apply lighting_mismatch Light recolor 1 0 1`, `IAI.Apply lod_corruption Bot` (static or skeletal),
  `IAI.Apply lod_popping Foliage 2`, `IAI.Apply camera_clipping 100`.
- `IAI.Revert <id>` — revert one anomaly.
- `IAI.RevertAll` — revert all active anomalies (also runs automatically on world teardown).
- `IAI.SetViewportScoping <0|1>` — opt-in: scope the four object-scoped anomalies (`missing_object`,
  `blinking`, `lod_corruption`, `lod_popping`) to objects visible in the player's viewport (default OFF).
- `IAI.TestVisibility <substring> <ox oy oz> <pitch yaw roll> [fov] [aspect]` — diagnostic for the
  viewport core against a synthetic view (logs per-component frustum/occlusion/visible).
- **Object Selector + Inject UI (m5):** `IAI.SelectorUI <0|1>` (default OFF) turns on an in-game overlay to
  **select a visible object and inject an anomaly on it**. Real-Play keys: Tab/Shift+Tab cycle the visible object,
  C cycles the anomaly, G injects, H reverts (rebindable via `IAI.SelectorBind`). The bridge drives the same model
  via `IAI.Selector.Next/Prev/Cycle/Inject/Revert` + `IAI.Selector.Status`. See the runbook §6a.
- **Automatic Injection (m6):** `IAI.Auto.Enable <0|1>` (default OFF) shows the auto-injector overlay; `IAI.Auto.Run <0|1>`
  (key J) starts/stops **auto-firing the four object-scoped anomalies randomly on on-screen objects**, each auto-reverting
  after a randomized hold (one anomaly per actor; one seeded `FRandomStream`). Real-Play keys 1-4 toggle the types, K reseeds.
  The bridge drives the deterministic core via `IAI.Auto.Seed`/`IAI.Auto.FireOnce`/`IAI.Auto.Step` + `IAI.Auto.Status`
  (no real time needed). Cadence: `IAI.Auto.Interval/Hold/MaxConcurrent/Persist`. See the runbook §6b.

Output goes to the **Output Log** under the `LogAnomaly` category. The tick heartbeat shows
on-screen (green) as `[IAI] AnomalyInjector ticking (active: N/Total)` and proves the subsystem
is alive. The full anomaly catalog is in [architecture.md](architecture.md).

## Build & run in one screen
See [setup-runbook.md](setup-runbook.md) for the authoritative steps. Short version:
1. Clean stale project-root `Binaries/` + `Intermediate/` if present.
2. Build `StackOBotEditor / Development / Win64` against the source engine.
3. Open `StackOBot.uproject`, confirm the plugin is enabled, load `MainWorld`, press Play.
4. Smoke-test with the `IAI.*` commands above (start with `IAI.ListAnomalies`).

Europa owner-kit state (094-16): the EP1/EP2 three-attempt wrapper is armed for one owner-started preview session. Refusals before launch leave the next slot usable; any launch locks later attempts. Only this owner profile omits human idle. Measured legs retain the full gate. See [09416-owner-kit.md](europa/09416-owner-kit.md) for exact scope and receipts.

Europa owner-kit update (094-17): 09416 is superseded. Reading errors wait, process creation locks the one-session authorization before any bookkeeping, and resolved real paths are refused in dry-run configs. The current 09417 kit passes its real preflight on throwaway slots; all real slots remain unused. Chat supplies the owner card; 094-18 checks the owner leg. See [09417-owner-kit.md](europa/09417-owner-kit.md).

Europa automated authoring update (094-28, 2026-10-30): the 09417 owner kit remains spent. The separate 09428 automated kit uses retained Python 2.7 modules, a mandatory no-create Smoke before EA_Core/EA_Travel, both privacy overrides, FC5 managed-file classes and FC6 owned UBT argv checks. Its real-session outcome is NEEDS-DECISION: Smoke stopped by an over-strict UBT guard; no recipe started and no package was created. This status does not authorize another launch; use only the current chat request and unspent armed slots. See [09428-authoring-receipts.json](europa/09428-authoring-receipts.json) and [09428-authoring-amendments.md](europa/09428-authoring-amendments.md). Product anomaly behavior and the frozen 5.1 records are unchanged.

Europa 094-26b (2026-10-30): FD2 build-action detection and FD3 memory lines (6 GiB physical / 8 GiB commit start, 4 GiB commit stop) are qualified on 09e776b. The stock ValidatePlatforms child passed, but the native Smoke found two absent PostProcessSettings anti-aliasing properties and stopped the run cleanly before either recipe. No new package, crash or upload evidence. The spent Smoke is not retryable; future authoring needs an authorized recipe correction and fresh arming. See [the receipt index](europa/09426b-authoring-receipts.json) and [session journal](sessions/2026-10-30-094-26b-ubt-memory-smoke.md). No dry runs, push or tags.

Europa 094-26c (2026-10-30): the FE1 recipe correction and FE2 event-log exclusion are committed on 9f5ce85; the real Smoke passed all 51 native bindings. Core exited 0 and authored its two packages, then failed the Config check because UE 4.25 wrote Saved/Config/WorldState/1202994220.json. Both packages are quarantined, Travel never started, and the Config addition is preserved as a hard STOP. Existing Content/config and binary hashes remain equal; no crash/upload evidence. A6 remains incomplete. See [the receipt index](europa/09426c-authoring-receipts.json) and [session journal](sessions/2026-10-30-094-26c-fixture-aa-config-stop.md). No retry, dry run, push or tags.

Europa 094-26d (2026-10-30): both A6 recipes completed with clean 4.25 editor exits. Core and Travel each used slot 01; three declared packages remain. FF2 records all host Saved files except protected Saved/Config .ini; project Config, descriptors, existing Content and binaries are unchanged. The prior Core packages stay quarantined. The kit has two spent authorizations and no Smoke slot. No dry run, build, upload, push or tags. Recipe/launcher and FD2/FD3 are unchanged. A7 cold reopen, pictures and owner review remain pending. See [receipt index](europa/09426d-authoring-receipts.json) and [journal](sessions/2026-10-30-094-26d-fixture-authoring.md).

Europa 094-27 (2026-10-30): runtime kit 599ef70 is armed but spent after its Smoke reader failed on the wrong cleanup schema. Smoke exited 0 and found 31/31 bindings; no runtime leg or real picture ran. Raw skipped cleanup and probe_stop exist. Crash-monitor respawn coverage also remains a gap. Protected fixtures/config/binaries and P9 v2 remain equal. No repair/retry, push or tags. See [receipt index](europa/09427-runtime-receipts.json) and [journal](sessions/2026-10-30-094-27-runtime-kit.md).

Europa 094-27b (2026-10-30): Smoke passed. A3 Load exited 3 after the driver reported "Saved camera rotation differs", followed by an access violation with Niagara at the top of the engine stack. FH3 stopped the remaining 16 legs; no A7 pictures were produced. The crash cause remains unresolved. FH2–FH8 fixes passed real-file mutations, Python 2.7 and eight rehearsal cases; both 18-leg dry runs passed. Real sessions: 2; runtime legs started: 1. Run result STOP. Gates: A3 FAIL, A5 NOT_RUN, A6 NOT_RUN, A7 NOT_RUN, A8 NOT_RUN. Counted-event contact sheets: 0. Stop reason: abnormal editor/game exit. Raw evidence and reader verdicts are retained; no verdict was changed by hand. Content, config, descriptors, bound artifacts and marker 31124 remain equal. No build, push or tags. See [receipt index](europa/09427b-runtime-receipts.json) and [FH amendment](europa/09427b-runtime-amendment.md).

Europa 094-27c (2026-10-30): The two A6 maps and shared mesh were quarantined and authored fresh; both camera checks passed before any save. Keyword-only Rotator checks, the real-signature Python 2.7 stub, callback re-entry/single-shot exit and crash counting passed qualification. The prior Niagara shutdown crash root cause remains unconfirmed. Runtime sessions: 18 of 18; crashes: 1. Gates: A3 PASS, A5 FAIL, A6 FAIL, A7 FAIL, A8 FAIL. Counted-event contact sheets: 1. Initial exit canary scan: GAP, 4 unreadable paths outside the tested exclusions, no matches. Raw files and generated reader verdicts are retained. No native build, push or tags. See [receipts](europa/09427c-runtime-receipts.json) and [FI amendment](europa/09427c-runtime-authoring-amendment.md).


### 094-27d lean Phase A closeout (2026-10-31)

094-27d, 2026-10-31 — NEEDS-DECISION. A7 counted events: blinking 0, lighting_mismatch 5, lod_corruption 0, missing_object 6. Lighting uses kept 094-27c evidence; missing-object was re-read with a bounded one-level quantization check; only blinking and LOD were armed again, with a smoke slot because the lean gate and monitor core changed. A3 was already complete. A5, reflected GC, A8 and controls are deferred under FJ1. No native build, Content edit, push or tag. No real session launched before the 07:00 IST deadline; the gate remained below the physical-memory start line.  See `europa/09427d-a7-receipts.json` for the evidence. The 09427d kit is spent; a new session requires a fresh authorized arming.


### 094-27e: Phase A complete (2026-10-31)

094-27e, 2026-10-31 — GREEN. Phase A is complete under FJ2: A3 Load already passed and all four A7 anomalies now have counted apply/revert pictures (blinking 6, lighting_mismatch 5, lod_corruption 6, missing_object 6). Smoke, blinking and LOD passed with exit 0. Lighting retains 094-27c evidence; missing_object retains the 094-27d re-read. Protected files and refs are unchanged; no crash or upload. A5, reflected GC, A8 and controls remain deferred under FJ1; the native probe write self-disable remains deferred under FK4. No native build, push or tags. The 09427e kit is spent. See `europa/09427e-a7-receipts.json` for the four contact sheets and session evidence.


## Europa capture status (094-33b)

Phase A and UI-free C1/C2 are accepted. The repaired 4.25 C3 descriptor runs normally and produces 240 masks with zero colour/label mismatch. Blinking drawn/hidden and missing_object pass frozen metrics. Lighting and LOD fail changed-pixel coverage (about 56.46% and 4.94%, required 95%); their colour changes extend beyond the permitted reference silhouettes. C3 remains NEEDS-DECISION. Thresholds, raw FAIL and evidence are retained; a descriptor retry is now qualified, with coverage still expected to fail. No crash, upload sign or protected change. See [receipts](europa/09433b-mask-receipts.json) and [journal](sessions/2026-11-01-094-33b-mask-flags.md).

Final log review found a separate handled D3D11 ensure in slot 01: the reduction buffer lacked BUF_SourceCopy (Runtime.log:642-643; stock D3D11Commands.cpp:2320). No crash or upload occurred. The copy-source descriptor negative now fails the old usage and passes the repaired usage. The one FP3 retry is qualified to verify that fix after a new FM1 build pair; frozen colour-coverage failures are expected to remain and are not hidden or relaxed.
