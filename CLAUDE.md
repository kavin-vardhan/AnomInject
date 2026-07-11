# AnomalyInjector — canonical context

Personal research project **"GDP: Anomaly Injection"** (intrusive UE5 track). This is a
UE5 plugin that injects **labeled visual anomalies** (graphics bugs — missing objects,
lighting mismatch, LOD corruption, blinking, etc.) into UE5 games, to generate synthetic
training data for bug-detection ML. It is **game-agnostic** (public UE APIs only) and is
tested on Stack O Bot. A separate non-intrusive tool exists elsewhere and is out of scope.

This file is the **canonical entry point**. The folder it lives in is its own git repository
and is the single source of truth for the project.

## Current status — keep this current; it is the cold-start "you are here"
- **IN FLIGHT (branch `feature/stencil-capture` off `master` `d4a77db`, NOT on `master`): Occlusion-correct stencil bounding boxes + async unified capture.**
  Multi-stage; **Stage 1 COMPLETE, owner re-eyeball GREEN (2026-06-30), committed on the branch** (`refactor(capture)`, no tag). A new
  **quarantined `AnomalyCapture` module** (gated `ANOMALY_CAPTURE`; render/`RHI`/`RenderCore`/`Slate`/`SlateCore`/`ApplicationCore` + a
  `bBuildEditor`-only `UnrealEd` dep, all compiled OUT of Shipping — `Renderer`/Renderer-private deferred to Stage 3) extracted from
  `AnomalyControlServer`: the m7 capture (`UAnomalyCaptureSubsystem` + `AnomalyLabelWriter` + `AnomalyPreviewCapture`) **MOVED** there with
  its own log cat `LogAnomalyCapture`; `AnomalyControlServer` now **depends on** `AnomalyCapture` (DAG: core ← AnomalyCapture ← ControlServer).
  New **async, non-blocking capture** that grabs the REAL player frame (**game UI IN**): `FAnomalyFrameCapturer` hooks
  `FSlateRenderer::OnBackBufferReadyToPresent` (post-Slate), clips to the game-viewport rect (FFrameGrabber `TargetWindowPtr`+`CaptureRect`
  pattern → no editor chrome even in docked PIE), stages an `FRHIGPUTextureReadback`, the render thread does only the lock-copy-out, and a
  **thread-pool `FAnomalyAsyncWriter`** does convert+encode+write OFF the game thread (G53 — fixes a per-frame stall/animation judder). Frame↔state
  carry keyed by submit `GFrameCounter`; `IAI.Capture.Async <0|1>` falls back to the sync `ReadPixels` path. **Only OUR overlays** are suppressed
  for a run via a generalized core flag `AnomalyViewport::SetOverlaysSuppressed` (poll-radius sphere + selector HUD/box + auto HUD + the heartbeat
  **actively evicted** each tick, G54); the PIE mouse-control-label is disabled **per-PIE-session** at subsystem `Initialize` (G55). A `DrainTail`
  FSM phase makes clean burst-count runs **0-drop** (G56). The m7 projected label box is UNCHANGED this stage (the stencil box is Stage 3; color and
  stencil are now two grab points joined by frame id — G52). **Clean 5.1 Dev-Editor compile (exit 0); core dep set unchanged (render deps quarantined);
  `IAnomaly` untouched; catalog stays 8.** Gotchas **G52–G57**; journal `docs/sessions/2026-06-30-015-stencil-capture-stage1.md`.
  **Next:** Stage 2 — custom-stencil tagging (`r.CustomDepth 3`, set/restore), then Stage 3 (stencil/depth SVE + occlusion-correct box), Stage 4 (multi-actor + docs + tag).
- **Latest as-built (post-m8, NO tag — both on `master`, 2026-06-22): Screen-coverage candidate cull + its dashboard slider.**
  **(1) Cull** (commit `a96f8bb`) — an optional **actor-level** cull on the renderable-visible set in `AnomalyViewport`: an actor
  is an injectable target iff its on-screen footprint (the **clamped projected union** of its renderable-visible component bounds)
  covers **≥ P%** of the viewport. `IAI.SetMinScreenCoverage <pct>` (plain world-independent cmd; `P <= 0` = OFF = byte-identical) +
  `IAI.DumpCoverage` (ascending-coverage tuning diagnostic). Applied to **both** live entry points
  (`GetVisibleRenderableActors` + `GetVisibleRenderableActorInfos`) through a new shared per-actor classifier
  **`ClassifyRenderableVisibleLive`** (OFF = byte-identical in result **and** cost via the kept first-match short-circuit; ON = one
  union pass, no double tracing) so the `IAI.DumpVisible` set-identity gate holds with the cull ON. Reuses the clamped
  `ProjectBoundsToScreenRect` fed the visible-component union (NOT the m7 type-only/unclamped projector). Touches only
  `AnomalyViewport.{h,cpp}` + docs; **no `IAnomaly`/dep change**. Gotcha **G51**; journal
  `docs/sessions/2026-06-22-014-screen-coverage-cull.md`. **(2) Dashboard slider** (plugin `81bc841` + dashboard repo `8c148b6`) —
  surfaced as a **throttled live slider** on the Tier-2 dashboard via a new `AnomalyControlServer` WS command
  `set_min_screen_coverage {pct}` (→ `SetMinScreenCoveragePct`) + a `session.minScreenCoverage` snapshot field (→ `GetMinScreenCoveragePct`),
  cloned from the poll-radius precedent; server stays compiled out of Shipping. Session-level live value only — **NOT** per-actor
  `FRenderableActorInfo` (still deferred). New dashboard `src/lib/throttle.ts` (~10/sec + authoritative send on release; numeric %
  snapshot-bound). **No new anomaly — catalog stays 8** (cull = targeting infrastructure, slider = UI). No tag, no version bump
  (same framing as the poll-radius pair). FF-merged into `master` in both repos; review branches deleted.
- **Latest milestone (as-built): Missing-Texture Anomaly — COMPLETE (tagged `m8`, VersionName 0.9.0) (2026-06-21).**
  New **8th** anomaly **`missing_texture`** (object-scoped, `Private/Anomalies/Anomaly_MissingTexture.{h,cpp}`): per-component
  `UMeshComponent::SetMaterial` swaps every renderable static/skeletal mesh slot to a plugin-**shipped Lit gray/white UV-checker**
  material (per-component override = object isolation, never touches the shared mesh/material asset; per-slot original +
  `bWasExplicitOverride` captured for an exact revert; skip stale). **First `Content/` asset in the plugin** — cook guarantee =
  a **CDO hard-ref** (`ConstructorHelpers::FObjectFinder` → non-transient `UPROPERTY TObjectPtr` on `UAnomalyInjectorSubsystem`)
  + flip `"CanContainContent": true`; **no host `DefaultGame.ini`** (G45). Found the hard way: the cook runs on **editor** binaries
  (rebuild before cooking — G47); 5.1 **IoStore** puts cooked assets in `.ucas`/`.utoc`, not `.pak` (verify by runtime load — G48).
  Material declares all mesh **usage flags** (skeletal/nanite/ISM/morph/spline) or it renders **default-gray** at runtime (G49).
  Reproducible authoring via `tools/create_missing_texture_materials.py`. Wired: `Register()`; `GetAuthoredSpec` (Object, **no
  args**); added to the selector `GAnomalyChoices` + the auto `GAutoPool` (`NumPoolKeys` 4→5, key `5`/`pool5`). **`IAnomaly`
  untouched; deps `Core/CoreUObject/Engine/InputCore`; catalog 7→8.** Gates driven green over the bridge: G-Compile (DumpCatalog=8),
  G-Apply (static multi-slot `SM_Ramp3` + skeletal `SKM_Bot`, exact revert incl. both override branches), G-Isolation
  (`SM_RockFlats_02`/`M_Rock` sibling untouched), G-BornComplete (selector cycle+inject, auto FireOnce, a 14-burst capture run).
  **DEFERRED — the flat-magenta variant + a `mode` arg:** unlit-emissive magenta lit the Lumen scene ("glowed" onto neighbours);
  the canonical fix is Lit base-colour but the owner is revisiting the look (G50). One `feat(missing-texture)` commit, tagged
  **`m8`**. **NOTE:** the unreal-mcpython bridge was unstable this session (crashed the editor on some calls) — the lit-checker
  live render is owner-eyeball-pending. → `docs/sessions/2026-06-21-013-missing-texture.md`.
- **Prior milestone (as-built): Labeled Frame-Capture + 2D BBox Labeling — COMPLETE (tagged `m7`, VersionName 0.8.0)
  (2026-06-20).** A capture/labeling layer producing an ML-friendly **labeled image sequence** from a LIVE
  auto-injection run (labels = the injector's OWN ground truth, L1). New **`UAnomalyCaptureSubsystem`** + `AnomalyLabelWriter`
  **housed in the `AnomalyControlServer` module** (reuses its game-viewport capture primitive + ImageWrapper; gated by
  `ANOMALY_CONTROL_SERVER`, compiled out of Shipping — dataset capture is a dev/research activity in a packaged
  Development/Test build, never retail Shipping). Drives the m6 deterministic core in **capture-driven bursts**
  (`[pre] → FireOnce → [settle K] → [positives] → RevertAllLiveFires → [settle K] → [post]`, looped); per captured frame
  writes `frame_<GFrameCounter>.png` (opaque, native res) + a JSONL label record + `run.json`/`run_summary.json`, all
  same-tick + `GFrameCounter`-stamped (exact image↔label alignment). The 2D bbox projects the fired actor's PERSISTED
  bounds (works when hidden). **Three sanctioned core exposures only — `IAnomaly`/injector/anomalies/leaf-helpers/`=`-match/
  `GetVisibleRenderableActors` byte-clean:** `AnomalyViewport::ProjectActorBoundsToScreenRect` (type-only bounds union,
  NOT `IsVisible`-gated — G38); `FAutoLiveFireInfo` widened with `TargetActor` + `StartFrame`; `RevertAllLiveFires()`
  exposed (keeps `GetLiveFires()` accurate). **No new dep; catalog stays 7.** Settle-K SYMMETRIC at both boundaries (G37);
  view-lag **L=0 validated** (tickables tick before the camera update → the view already lags 1 frame; L=0 = "1 render-frame
  back", FPS-invariant — G41); **`visible_positive`** = present + a valid box (off-screen-during-hold frames kept as hard
  negatives — G42). `tools/verify_capture.py` overlays boxes (Pillow). Gates 1/2/3 GREEN incl. owner moving-eyeball.
  **Closed as TWO commits (Plan A):** `ff1be3c` `feat(control-server): Slice-1 dashboard …` (the parallel track's
  uncommitted Slice-1 WIP promoted first, NO tag — it owns the shared capture primitive) + the m7 commit on top (tagged
  **`m7`**). The async backbuffer capture path (`OnBackBufferReadyToPresent` + GPU readback) is the documented superseder,
  REQUIRED before framerate-bug anomalies + for exact-under-motion (G40). → `docs/sessions/2026-06-20-012-frame-capture-labeling.md`,
  `docs/post-m7-capture-labeling-handoff.md`.
- **Prior as-built (post-m6 viewport fixes, 2026-06-20):** two surgical, owner-locked fixes to the shared
  **renderable-visible set** in `AnomalyViewport` (the one source of truth consumed by the M5 selector, the m6
  auto-injector, AND — new since m6 — the control-server **A4 read-back** `GetVisibleRenderableActorInfos` + the
  `IAI.DumpVisible` set-identity gate). **(1) VFX removed (G33):** dropped the `UFXSystemComponent` clause from
  `IsRenderableComponent` → the set is **SM ∥ SK only** (reverses the G29/R1 VFX inclusion; HARD REMOVE). The `=name`
  console escape hatch still reaches VFX actors (it bypasses the predicate). **(2) Changeable poll-radius distance
  cull (G34):** `IAI.SetPollRadius <cm>` adds an optional cull — actor in the set iff renderable AND within R of the
  **player pawn** (sphere-approx bounds metric) AND in-frustum AND unoccluded; `R <= 0` disables it (default OFF,
  byte-identical); applied identically at both live entry points (DumpVisible MATCH preserved); a dev debug sphere
  visualizes R around the live pawn. **Only `AnomalyViewport.{h,cpp}` touched** (+ docs); `IAnomaly`/injector/anomalies/
  selector/auto/control-server cores untouched; **no new dep**, no `.uplugin` bump. **Clean Development-Editor compile
  on 5.1 (exit 0)** before each commit (control-server module re-links clean against the changed header). **Two atomic
  path-scoped commits, NO tag:** `9bbd398` `fix(viewport): remove VFX from renderable-visible set` +
  `<fix2>` `feat(viewport): add changeable poll-radius distance cull` (the uncommitted control-server WIP in the tree
  was left untouched). Owner smoke-test pending. → `docs/sessions/2026-06-20-011-viewport-vfx-removal-poll-radius.md`.
- **Prior as-built (control server, in flight):** the Tier-2 runtime control surface is under construction — committed
  slices `2645236` (transport spike: WS server + auth + loopback gate + backbuffer→JPEG) and `4c05344` (core read-back
  / A4: `GetVisibleRenderableActorInfos` + `FRenderableActorInfo`), plus uncommitted WIP. Separate `AnomalyControlServer`
  module with its own log category (G32). Not yet journaled as a milestone.
- **Latest milestone (as-built):** **Automatic Injection — COMPLETE (committed `41ba104`, tagged `m6`) (2026-06-19).** New
  **separate** `UAnomalyAutoInjectorSubsystem`
  (`Public/AnomalyAutoInjectorSubsystem.h` + `Private/AnomalyAutoInjectorSubsystem.cpp`, `UTickableWorldSubsystem`,
  Game+PIE) that auto-fires the **4** object-scoped anomalies **randomly on the renderable objects currently on-screen**
  (drawn from `AnomalyViewport::GetVisibleRenderableActors` + applied via the `=` exact-match token), each
  **auto-reverting** after a randomized hold. **Concurrent but collision-free by construction** (no coordinator) via two
  invariants: **(i)** one live fire per id (the registry's one-instance-per-id) + **(ii)** **one anomaly per actor**
  (`OVERRIDE-1` — subsumes both conflict groups *and* the hide-masks-LOD case, so **no id→group table**; supersedes the
  planning turn's per-group guard). All randomness from **one seeded `FRandomStream`** (console-settable seed, default
  time-based) on a **fixed draw protocol** independent of apply-result (R-SEED). **Explicit-core / thin-shell split (as
  m4/m5):** the deterministic core `AdvanceTime`/`TryFireOnce` is bridge-driveable as `IAI.Auto.Step`/`IAI.Auto.FireOnce`
  **without real time and without Enable/Run**; two thin shells drive it — the `IAI.Auto.*` console (bridge gate) +
  raw-input poll (keys `1-4`/`J`/`K`, distinct from the selector's) + a right-anchored immediate-mode HUD (eyeball).
  **Two switches, both default OFF → dormant → existing gates byte-identical:** `IAI.Auto.Enable <0|1>` (HUD/keys) and
  `IAI.Auto.Run <0|1>` (firing; forced OFF when !Enabled). Fires **auto-revert** after a randomized hold (R-LIFE;
  `IAI.Auto.Persist` flag, default off). **Self-scoping** — does NOT touch `IAI.SetViewportScoping` (warns if it is ON);
  no view → fire nothing (never blind). **Manual selector/console injection of a pool id during an auto run is
  unsupported → warn-not-block (R-COEXIST).** **No `IAnomaly`/injector/anomaly/leaf-helper change; no new dep**
  (`FRandomStream` = Core; deps stay `Core/CoreUObject/Engine/InputCore`); **catalog stays 7** (orchestration over the
  existing catalog). VersionName → **0.7.0**. **Clean Development-Editor compile on 5.1 (exit 0).** **Bridge state-gates
  GREEN (MainWorld Simulate):** deterministic headless fire + `=` exact-match (1 of 21 EnergyOrb siblings hit),
  auto-revert on hold-elapse, collision-free concurrent (3 distinct ids × 3 distinct actors, no 4th fire — invariants
  (i)+(ii)+cap), seed-reproducible target, OFF-regression byte-identical (`SM_Ramp`→2), both coexistence warnings fire
  without blocking. **Owner real-Play eyeball green — ACCEPTED.** → `docs/sessions/2026-06-19-010-auto-injection.md`.
- **Prior milestone (as-built):** **Object Selector + Inject UI (minimal) — COMPLETE (committed `aa2a3a4`, tagged `m5`) (2026-06-19).**
  A new **separate** `UAnomalySelectorSubsystem` (`Public/AnomalySelectorSubsystem.h` + `Private/AnomalySelectorSubsystem.cpp`,
  `UTickableWorldSubsystem`, Game+PIE only) that lets the player **select a visible on-screen object** (Tab-cycle over the
  **renderable-visible set** — frustum AND occlusion AND renders-to-screen) and **inject** one of the four object-scoped anomalies on it (default args), then
  revert — calling the existing injector's public `ApplyAnomaly`/`RevertAnomaly`. **Explicit-core / thin-shell split (as m4):**
  public methods `AdvanceSelection`/`SelectPrevious`/`CycleAnomalyChoice`/`InjectSelected`/`RevertSelected` + readbacks
  `GetSelectedActorName`/`GetVisibleActorNames`/`GetAnomalyChoice` are the bridge-gatable surface; two thin shells drive them —
  the `IAI.Selector.*` console commands (bridge gate) and per-tick **raw input polling** + an **immediate-mode HUD**
  (real-Play eyeball). Targeting is made exact by a new **`=` sentinel** in `AnomalyTargeting::FindActorsMatching`
  (leading `=` → `GetName().Equals(IgnoreCase)`; substring path **byte-identical** with no `=`); `InjectSelected` passes
  `"=" + GetName()` so it hits only the selected actor (the **only** leaf-helper change — additive; verify-item 5 pre-authorized).
  HUD = `UDebugDrawService::Register("Game", …)` (host-blind, no game HUD class — G25) drawing a visible-names list + an
  anomaly list + a `DrawDebugBox`/label on the selection; input = `WasInputKeyJustPressed`/`IsInputKeyDown` raw key state
  (no host mappings — G26); defaults Tab/Shift+Tab/C/G/H, rebindable via `IAI.SelectorBind`. Activation **`IAI.SelectorUI <0|1>`,
  default OFF → dormant → existing gates byte-identical.** **First dep since M0: `InputCore`** (FKey/EKeys; transitive via Engine,
  declared for IWYU) — **no Slate/UMG** (immediate-mode). **Renderable-target filter folded in** (m5 follow-on): the selector's
  visible set means **renderable-visible** — new additive `AnomalyViewport::IsRenderableComponent` (`IsVisible()` + a
  static/skeletal/`UFXSystemComponent` base-type allowlist; VFX caught with no Niagara dep) excludes volumes/spawn-points/
  debug/landscape (the m4 visibility funcs stay byte-identical); a HUD `LastInjectResult` line surfaces the AMB-2 zero-match;
  `GetVisibleRenderableActors` returns empty on no-view (offer nothing, never blind). This is the set **auto-injection** will
  consume (gotcha G29). **No `IAnomaly` change, injector subsystem + all 7 anomalies untouched;
  catalog stays 7.** VersionName → **0.6.0**. **Clean Development-Editor compile on 5.1 (exit 0).** Combined gate **green**
  over the bridge (MainWorld Simulate): selection cycles the name-sorted renderable-visible set; `=` exact-match inject
  hits exactly the selected actor (1 of 17 prefix-siblings); the renderable filter excludes RVTVolume / PlayerStart /
  GameplayDebugger / zero-instance-grass LandscapeStreamingProxy while keeping meshes + foliage + NiagaraActors +
  RoomBuilderSquare *(NiagaraActors/VFX were later removed from the set — G33, 2026-06-20)*;
  zero-match (Niagara + `lod_corruption`) surfaced; OFF-regression byte-identical (`SM_Ramp`→2,
  `=SM_Ramp2…`→1). **Owner real-Play eyeball green — ACCEPTED.** → `docs/sessions/2026-06-19-009-selector-inject-ui.md`.
- **Prior milestone (as-built):** **Viewport-Visibility Layer — COMPLETE (committed `7c34275`, tagged `m4`) (2026-06-18).**
  New shared helper **`AnomalyViewport`** (`Public/AnomalyViewport.h` + `Private/AnomalyViewport.cpp`,
  AnomalyTargeting/Args/Lod convention) = "is this object visible to the player" via **frustum AND occlusion**
  over an explicit view spec `FAnomalyViewInfo` (deterministic, synthetic-view-gatable) + a thin live resolver
  `GetActiveViewInfo` (first local player's POV; treat-as-unscoped + warn on no view). Occlusion backend (AMB-V1)
  = **multi-sample camera-to-bounds line trace** (`ECC_Visibility`, center+8 corners), private behind the
  backend-agnostic API; `GetLastRenderTimeOnScreen()` is the documented live backend for the future
  capture/live-injection milestone (.cpp-only swap — G22). New opt-in toggle **`IAI.SetViewportScoping <0|1>`
  (default OFF)** + diagnostic **`IAI.TestVisibility`** (synthetic-gate driver). The **4** object-scoped
  primitive-backed anomalies (`missing_object`, `flicker`, `lod_corruption`, `lod_popping`) consult the toggle and
  route through `AnomalyViewport` only when ON; `lighting_mismatch` + the two globals are excluded by design.
  **No `IAnomaly` change, no new module dependency** (frustum/traces/camera = Engine, `FReversedZPerspectiveMatrix` =
  Core; both locks held). **Clean Development-Editor compile on 5.1 (exit 0)**; over the bridge (MainWorld Simulate):
  synthetic frustum gate (behind→out, far→in, in-cone→in — reversed-Z VP validated, G24), synthetic occlusion gate
  (controlled wall: blocked→0 / clear→1 at frustum=1), and **OFF-is-byte-identical regression** (`missing_object`
  + `lod_corruption` round-trips M-identical, ListAnomalies still 7) all **green**. Catalog unchanged at **7**.
  VersionName → **0.5.0**. → `docs/sessions/2026-06-18-008-viewport-visibility-layer.md`.
- **Prior as-built:** **Refactor — "GDP" prefix removed from the plugin — COMPLETE + COMMITTED `351c7e8` (2026-06-18).**
  Pure mechanical rename, **no behavior change**: module/plugin/folder/`Build.cs`/`.uplugin` `GDPAnomalyInjector`→`AnomalyInjector`;
  `UGDPAnomalyInjectorSubsystem`→`UAnomalyInjectorSubsystem`; `IGDPAnomaly`→`IAnomaly`; `FGDPAnomaly_*`→`FAnomaly_*`;
  API macro `GDPANOMALYINJECTOR_API`→`ANOMALYINJECTOR_API`; log category `LogGDPAnomaly`→`LogAnomaly`;
  helpers `GDPTargeting/GDPArgs/GDPLod`→`AnomalyTargeting/AnomalyArgs/AnomalyLod`; console commands `GDP.*`→`IAI.*`.
  Project identity **"GDP: Anomaly Injection"** retained (code-prefix strip only; copyright/`CreatedBy` unchanged). Clean
  Development-Editor compile on 5.1 (exit 0) + light bridge re-gate green (module loads under the new name, `IAI.ListAnomalies`
  lists the **7** sorted under `LogAnomaly`, `IAI.Apply/Revert missing_object SM_Ramp` round-trips). One `refactor:` commit, **no tag**;
  bridge/host unchanged (G21). → `docs/sessions/2026-06-18-007-rename-strip-gdp-prefix.md`.
- **Prior milestone:** **M3 — LOD breadth fill — COMPLETE (committed `c54351a`, tagged `m3`).**
  `lod_corruption` extended to **static OR skeletal** meshes (same ID — one "LOD corruption" category; mesh
  type is an implementation detail), new ticking **`lod_popping`** (flicker mechanics), and a new shared
  helper **`AnomalyLod`** (`Public/AnomalyLod.h`+`Private/AnomalyLod.cpp`) absorbing the static/skeletal forced-LOD
  dispatch (2 consumers). Registry lists **7** (sorted). **No `IAnomaly` change** (M1 lock held again)
  and **no new module dependency**. Clean Development-Editor compile on 5.1 (exit 0); all 9 state gates
  driven green over the bridge in a `MainWorld` Simulate session — incl. the static **regression**
  (M2-identical), the **heterogeneous** apply (`lod_corruption Bot` = 1 static + 2 skinned in one apply),
  `lod_popping` oscillation, re-apply no-leak, RevertAll, teardown. **The Bot is single-LOD → skeletal
  anomalies are state-validated, no Bot visual** (G20). VersionName → 0.4.0.
  → `docs/sessions/2026-06-13-006-m3-lod-breadth.md`.
- **Prior as-built:** **M2.5 (UE 5.1 port) + M2.6 (bridge sever) — COMPLETE (2026-06-10).** **UE 5.1 is now
  the canonical engine** (the two real target games are on 5.1). Host = `D:\IntrusiveAnomalies\StackOBot`
  (natively-5.1); source engine = 5.1 at `D:\UESource\UnrealEngine`. The six anomalies compile clean on 5.1
  with **zero plugin-source changes** (all 7 port watch-items unchanged; only host-target build constants
  changed — G17), and all **10** stage gates were re-driven **green over the MCP bridge** + owner-confirmed
  visuals (flicker blink, magenta movable sun, near-clip). The `unreal-mcpython` bridge was ported to 5.1 by
  **severing its `BehaviorTreeEditor` dependency** (G8) — costs only the 2 BT-authoring tools.
  → `docs/sessions/2026-06-10-005-m2.5-m2.6-5.1-port-bridge-sever.md`.
- **Earlier:** **M2 — Breadth Round 1 — COMPLETE (all 8 stage gates passed).**
  Adds two shared helpers — **A1** `AnomalyTargeting::FindComponentsMatching<T>` (component targeting) and
  **A3** `AnomalyArgs` (parse/clamp/warn) — and three anomalies: `lighting_mismatch` (component, ULightComponent),
  `lod_corruption` (component, UStaticMeshComponent, static-only), `camera_clipping` (global near-clip).
  Registry lists **6** (sorted). **No `IAnomaly` change was needed — the M1 lock held.** Clean headless
  compile + gates 2–7 verified live in PIE `MainWorld` (unreal-mcpython bridge + owner eyeball, 2026-06-09).
  → `docs/sessions/2026-06-09-004-m2-breadth-round-1.md`, `docs/architecture.md`.
- **Resolved (M3):** **AMB-1 → skinned LOD count via `USkinnedMeshComponent::GetNumLODs()`** (runtime
  render-data count — the analog of static `GetNumLODs()`; not the asset's authored `GetLODNum()`) — G19.
  **AMB-2 → single tagged capture record keyed to the common base `UMeshComponent`** + `Cast<>` dispatch in
  `AnomalyLod` (not two typed lists); this is what lets one apply span a heterogeneous static+skeletal set.
  **AMB-3 → `lod_popping` default 2 Hz, ceiling 30 Hz.** Supersedes G16's static-only scope.
- **Resolved (M2):** **AMB-M2-1 → defer A2/`AnomalyCvar`** — near-clip is a console *command* + the
  `GNearClippingPlane` global, not an `IConsoleVariable`, so `camera_clipping` is self-contained (no
  `RenderCore` dep); AnomalyCvar lands with its first real cvar consumer (G13). **AMB-M2-2 → static-only
  `lod_corruption`** was the M2 stopgap; **resolved in M3** (static + skeletal via `AnomalyLod`, G19). M2 ships 2 helpers (A1, A3).
- **Resolved (M1):** **AMB-3 → capture-baseline** — `time_dilation` Revert restores the pre-Apply value.
  Generalized in M2 to the **per-target/global state-capture convention** (see architecture.md). G11.
- **In flight:** the **Tier-2 runtime control server** (`AnomalyControlServer` module — WS transport + A4 read-back;
  committed `2645236`/`4c05344` + uncommitted WIP in the tree; not yet journaled as a milestone). The two post-m6
  viewport fixes (G33 VFX removal + G34 poll-radius cull) are committed (`9bbd398` + `<fix2>`, no tag); owner smoke-test
  pending. **Next action:** finish the control-server slice, then the High-priority new bug types (born viewport-aware
  AND auto-injectable). The `flicker→blinking` rename is **DONE** (`refactor(blinking)`, no tag). Also still queued: a new `flickering` anomaly (scene-region / light toggling; handoff §2.3),
  region-darkening (§2.4), the selector's screen-X ordering polish. Bridge/host stay unversioned (G8 unchanged).
- Milestones: M0 (`…-001`), M1 (`…-003`), M2 (`…-004`), M2.5+M2.6 (`…-005`), M3 (`…-006`) fully passed
  + tagged; rename refactor (`…-007`) committed `351c7e8` (no tag); **Viewport-Visibility Layer (`…-008`) committed
  `7c34275`, tagged `m4`**; **Object Selector + Inject UI (`…-009`) committed `aa2a3a4`, tagged `m5`**;
  **Automatic Injection (`…-010`) committed `41ba104`, tagged `m6`**; viewport VFX-removal + poll-radius (`…-011`,
  no tag); **Labeled Frame-Capture + 2D BBox Labeling (`…-012`) tagged `m7`** (control-server Slice-1 promoted first
  as `ff1be3c`, no tag).

## Documentation system — how these docs fit together (read in this order)
- **CLAUDE.md** (this file) — canonical context, environment, invariants, workflow rules, and the
  **Current status** above. Start here.
- **[docs/architecture.md](docs/architecture.md)** — **living** current-as-built design reference
  + the **anomaly catalog**. "The whole picture in one read." Describes only what is in the code
  *now*; forward plans live in the journals, never here.
- **[docs/onboarding.md](docs/onboarding.md)** — what this is, how the work is run, where things live.
- **[docs/setup-runbook.md](docs/setup-runbook.md)** — **living** recipe to build/run from scratch.
- **[docs/gotchas.md](docs/gotchas.md)** — **append-only** non-obvious lessons (G1, G2, …).
- **[docs/sessions/](docs/sessions/)** — one journal per session, `YYYY-MM-DD-NNN-slug.md`: the
  chronological record (Goal / What done / Problem→Resolution / Deviations / State / Hand-off) and
  the home for milestone **plans** and **design decisions** (including open/blocking ones).

## Environment
- Engine: **source-built UE 5.1** (Release-5.1) at `D:\UESource\UnrealEngine`, registered to the
  `.uproject`'s `EngineAssociation` GUID `{B34F356C-4AE7-256A-F0E1-318A632BB902}` under
  `HKCU\Software\Epic Games\Unreal Engine\Builds`. (Originally validated on source-built UE 5.4.4 — see
  the Engine support note in architecture.md. After any engine re-sync, **rebuild ShaderCompileWorker** — G18.)
- Host project: **StackOBot** at `D:\IntrusiveAnomalies\StackOBot` (natively-5.1; the old 5.4 host at
  `D:\Unreal Projects\StackOBot` is retired).
- Plugin in-tree at `D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector\` (its own git repo, `master`).
- Windows, MSVC. Build target: **StackOBotEditor / Development / Win64**. Host-target build constants:
  `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1` (G17).
- Functional smoke tests run in **PIE via the `unreal-mcpython` MCP bridge** (host tooling, NOT part of
  this repo — see gotcha G8; on 5.1 its `BehaviorTreeEditor` dependency is severed). State/log reads close
  the non-visual gates; the owner eyeballs visuals.

## Architecture (current as-built: M0 — full detail in docs/architecture.md)
- One **Runtime** module `AnomalyInjector`, `LoadingPhase = Default`, `EnabledByDefault: true`.
- Build.cs deps: `Core`, `CoreUObject`, `Engine` (later may add `Renderer`, `RenderCore`, `RHI`,
  `Slate`, `InputCore`).
- Core injector = a `UTickableWorldSubsystem` (`UAnomalyInjectorSubsystem`) — auto-ticks,
  world-scoped, gives `GetWorld()`. Restricted to **Game + PIE** worlds via `DoesSupportWorldType`
  (never the editor preview world).
- Control surface = console commands via `FAutoConsoleCommandWithWorldAndArgs`, module-scoped,
  resolving the subsystem from the world the console passes in, null-guarded.
- M0 anomaly = ONE hardcoded hide (`IAI.HideActor` / `IAI.ShowAllActors`). The general anomaly
  **interface + registry is the M1 design** (see Current status + journal 002), not yet in code.

## Invariants (do not violate)
- **Source carries NO comments — by deliberate convention.** Every source file (C++ `.h/.cpp`, C#
  `.Build.cs`, Python, `.bat`) is kept comment-free, *including* the top-of-file copyright/banner header.
  **Do NOT add comments; strip any before committing.** (Feature work keeps re-introducing them — first
  stripped in `d4a77db`, re-stripped 2026-07-08 after the `AnomalyCapture` module re-added them.) Enforced
  with a deterministic, byte-preserving stripper kept in the workspace root alongside the two repos
  (`_strip_comments.py`): run `python _strip_comments.py <repo-root>`. It removes only comments while
  preserving every other byte — all string/char/template/regex literal contents, CRLF endings, and the BOM
  (idempotent; validated byte-identical against the original strip). Put rationale and design notes in commit
  messages, `docs/`, and the session journals — **never in code.** `LICENSE.txt` and the `.uplugin` JSON are
  intentionally exempt (not source).
- **Plugin stays game-agnostic.** The `AnomalyInjector` module may depend only on
  `Core`/`CoreUObject`/`Engine` (later `Renderer`/`RenderCore`/`RHI`/`Slate`/`InputCore`)
  and must **never `#include` or reference host game-module types** (e.g. anything from the
  `StackOBot` module). Host-specific buildability lives in the project, never in the plugin.
- **Matching is label-free.** Targeting matches by actor Name or Class only.
  `GetActorLabel()` is editor-only and absent in cooked builds — `ListActors` may print the
  label (guarded by `WITH_EDITOR`) but nothing matches on it.

## Workflow & doc-maintenance rules
- **Two-Claude split.** Design decisions come from an orchestrating "chat Claude" and are
  ferried by Kavin (project owner). The implementing Claude implements. Genuine design forks
  or ambiguities are surfaced back (listed standalone), not improvised.
- **Reports to Chat go in a copy block.** Any status/gate/handoff report meant to be ferried
  back to the orchestrating chat Claude must be emitted as a single fenced code block (so the
  owner can copy-paste it verbatim). Applies to stage-gate results, plan summaries, and any
  "report back" deliverable.
- **Plan-before-code.** A new milestone's first response is a file-by-file plan only; no
  implementation until approved.
- **Commits — Conventional Commits.** Prefixes: `feat:` (new anomaly or capability), `fix:` (bug),
  `docs:` (doc-only), `refactor:` (no behavior change), `chore:` (build/tooling). Scope anomaly-specific
  changes, e.g. `feat(blinking): …`. **Tag each milestone** with `git tag m<N>` after its commit so
  milestones diff cleanly (`m1..m2`, and a changelog can be auto-derived later). The git repo is the
  plugin folder (`master`); host scaffolding lives outside it and is not committed here. **Before every
  commit, run the comment stripper (see Invariants) — the source must stay comment-free.**
- **Doc discipline — leave the docs able to (a) cold-start a fresh session and (b) explain the
  whole plugin to any UE dev.** When you start or advance a milestone you MUST, before the session
  closes:
  1. Update **Current status** (above) — the single "you are here" marker (latest as-built /
     in flight / open decisions / next action).
  2. Update **docs/architecture.md** to match the new as-built state, including the **anomaly
     catalog** — describe current code only, never aspirational.
  3. Write/append the **session journal** under `docs/sessions/` (history + the milestone plan +
     design decisions, including any open/blocking decisions).
  4. **Append** new lessons to `docs/gotchas.md` (never delete; supersede).
  5. Keep `docs/setup-runbook.md` and `docs/onboarding.md` current with the build/run steps and
     the control surface as they actually are.
  - Division of labor: **architecture.md = current state** ("what it is"); **journals = history +
    plans** ("how we got here / where we're going"); **runbook = repro**; **gotchas = lessons**.
