# Architecture (living — current as-built)

> **Reflects: three capture-delivery fixes (m16).** (1) **Client token auto-populate** — the control server
> reads a fixed `[AnomalyControlServer] Token` from `DefaultGame.ini` (GConfig at StartListening); present →
> that token, absent/empty → the existing random per-session GUID + log line (owner in-editor unchanged). The
> dashboard carries a matching token and auto-connects with zero client copy-paste (a static
> shared secret; localhost-only tradeoff — `ws://` ignores CORS so the token still gates arbitrary web
> origins; G71). **Dashboard side updated by M2 (2026-07-21):** that token is no longer baked into the
> bundle at build time (`VITE_CONTROL_TOKEN`) — it is read at startup from a runtime `config.json` served
> beside the app, so it can be changed without a rebuild and cannot silently vanish from a clean-checkout
> assembly. The ENGINE side described here is unchanged. See `PRE-DELIVERY-CHECKLIST.md` + G84. (2) **Focus-gated capture start** — a Start ARMS immediately (clean-slate reverts + auto
> pause happen now) but holds the first frame until the game window has foreground focus (new
> `ECapturePhase::ArmedPending` resolved in `Tick` via `FViewport::IsForegroundWindow`); skipped when there
> is no game window (headless / MainWorld Simulate), with an `IAI.Capture.FocusGate <0|1>` override +
> `[AnomalyCapture] bFocusGateDefault` + a 30 s safety timeout; the timing-critical setup (StartFrame,
> run manifest, fixed timestep) is deferred out of `StartRun` into `BeginActualRun` at focus-in, and a
> cancel-before-focus writes no artifacts and deletes the empty session dir (`bRunBegun` guard; G72). (3)
> **Preview-pause hardening** — the control server's `PushFrames` suppresses live-preview JPEG generation
> while a capture is active, engine-side and immediate (no snapshot round-trip), so the synchronous
> preview `ReadPixels` can't drag sustained fps at the start of a run (G73). A single `bRunning` /
> `IsCaptureActive()` signal (true from arm → finish, armed-pending included) drives BOTH the focus-gate
> machine and the preview suppression. Catalog stays 8; no new module dependency (GConfig is Core; focus via
> Engine `FViewport`; AnomalyCapture already links Slate/ApplicationCore in non-Shipping). See
> `sessions/2026-07-13-022-m16-capture-delivery-fixes.md`, `docs/client-delivery.md`. Below this it still
> reflects:
>
> **Reflects:** **Content-clock-aware fps stamp (m14)** — the m11 honest
> stamp (a slow run stamps the sustained wall rate) is correct for REAL-TIME-driven content but WRONG
> for GAME-CLOCK-driven content (StackOBot under fixed step), where every frame is an exact `1/target`
> game-time slice and the natural stamp is TARGET — stamping sustained there plays the video
> `speed_ratio`× too slow. A setting selects the clock: **`IAI.Capture.ContentClock <game|wall>`**
> (mid-run guarded), default **wall** (m15 — briefly `game` in m14, reverted after the owner tested the
> client titles wall-clock on the office machine), packaged default `DefaultGame.ini [AnomalyCapture]
> ContentClockDefault` (GConfig at Initialize, absent → wall). **wall** (default) = unchanged m11
> behavior (ratio>tol → sustained); correct for the client's wall-clock titles (natural speed; video
> length reflects real capture time). **game** = `video.fps` stamped at TARGET at any ratio (game-clock
> content like StackOBot plays natural; a high ratio warns only that the LIVE capture ran slow — a perf
> issue, not a video defect), set via ini in the StackOBot build. (m14's mixed-clock "OPEN" note is
> CLOSED: the client titles tested wall-clock; a `game` default would play them ~2× fast = Issue-2.)
> `run_summary.json` gains `content_clock`; annotation stays client-clean (its
> `video.fps` already encodes the decision). Only AnomalyCapture (subsystem stamp branch + warnings +
> the setting) + the run_summary field changed; fixed timestep / pacing / labeling / ground-truth all
> UNCHANGED. See `sessions/2026-07-13-020-m14-content-clock.md`,
> `docs/capture-fps.md`. Below this it still reflects:
> **Client delivery mode (m12)** — a capture DELIVERY MODE that
> limits what a run writes to disk to only client-facing files, for shipping the plugin to an external
> client who runs capture in their own build (no post-processing step between their capture and them —
> whatever capture writes IS what the client gets). A boolean `bDeliveryMode`, **default OFF** (full
> fidelity, byte-identical to m11 except the D3 annotation change below). **ON writes ONLY**
> `Actual_Frames/` + `Video_Clip/` (host mp4) + `run_summary.json` + `annotation.json`, and **suppresses**
> `labels.jsonl` + `run.json` (never created — the label record is still COMPUTED, just not written; the
> path is uniform). run_summary.json is kept because the host encode_watcher keys off it (its
> done-signal); annotation.json is the client's primary artifact. Because `run.json` holds the seed,
> delivery mode also withholds the seed → a delivered session is intentionally NOT client-reproducible
> (repro metadata stays owner-side; G68). Toggle: `IAI.Capture.Delivery <0|1>` (mid-run guarded like the
> other capture setters); the **packaged-build default** is read at subsystem Initialize from
> `GConfig` — `DefaultGame.ini [AnomalyCapture] bDeliveryModeDefault=True` — so the owner sets it before
> packaging a client build with no editor (the console command overrides per session; no SaveConfig from
> console; G69). run_summary.json gains a neutral `delivery_mode` bool. **D3 (both modes, always):**
> `schema_version` and per-anomaly `source_id` are removed from annotation.json (internal tags, no
> downstream consumer) — the only annotation diff vs m11 in the OFF path. No new module dependency
> (GConfig is Core); capture behavior (pacing/ground-truth/labeling compute) is identical in both modes —
> only disk writes + those two annotation fields differ. NOTE: our own QA tools overlay_watcher.py and
> tools/verify_capture.py require labels.jsonl and therefore no-op on delivery sessions BY DESIGN (G67).
> See `sessions/2026-07-12-018-m12-delivery-mode.md` + the client-build handoff note
> `docs/client-delivery.md`. Below this it still reflects:
> **Capture pacing + honest fps stamping (m11)** — capture runs are
> REAL-TIME PACED by default (`IAI.Capture.Pace <0|1>`, default ON): the capture subsystem holds every tick to
> >= `1/VideoFps` of wall time (drift-free coarse-sleep + spin at the top of its Tick; no catch-up after hitches),
> so game clock == wall clock == video clock and the delivered mp4 plays at natural speed for BOTH content clock
> families (game-clock-driven like StackOBot AND real-time-driven like the client games — the two-clock model, G64;
> UE's own limiter is bypassed under fixed timestep so this is the only pacer, G65). Every armed frame is
> wall-stamped (`t_wall` per labels.jsonl row, both async + sync paths); at finalize
> `speed_ratio = wallSpan/gameSpan` over the same first/last armed frames (settle gaps cancel) and
> `sustained_wall_fps = VideoFps/ratio`. ONE-SIDED honest stamp: ratio > 1.02 → `annotation.video.fps` = sustained
> (fractional, 3 decimals; encode watcher float-parses) + warnings (one-shot early at >=30 armed frames + finalize);
> otherwise fps = VideoFps exactly (a faster-than-target run — only possible with Pace 0 — keeps VideoFps; stamping
> the faster rate would speed up game-clock content). `annotation.video.target_fps` always recorded (internal);
> `run.json` += `target_fps`/`paced`; `run_summary.json` += `target_fps`/`sustained_wall_fps`/`speed_ratio`/
> `stamped_fps`/`paced`. NO frame duplication / NO VFR — the 1:1 Actual_Frames/labels/mp4 mapping is inviolate.
> WS `capture_stopped`/`capture_status` replies carry `{targetFps, stampedFps, speedRatio, paced}` (valid-gated);
> the dashboard CapturePanel shows a post-run badge from that payload when the stamp fell back. See
> `docs/capture-fps.md` (rewritten: two-clock model) + `sessions/2026-07-11-017-m11-capture-pacing.md`. Below this
> it still reflects:
> **Targeted capture modes + pre-run clean slate + entry-point parity (m10)** — a capture
> run now fires in one of two modes: **auto-pool** (unchanged: random mix from the enabled pool) or
> **targeted** (`IAI.Capture.Start ... [anomaly] [targetActor]` / WS `capture_start {anomaly, target}` —
> each burst fires exactly that anomaly on exactly that actor via the new
> `UAnomalyAutoInjectorSubsystem::TryFireSpecific`, which keeps the MaxConcurrent / one-instance-per-id /
> one-anomaly-per-actor guards and the `=` exact-match token; visibility-independent — G61). `run.json`
> records `mode`/`target_anomaly`/`target_actor`. **Pre-run clean slate:** `StartRun` reverts the auto
> layer's live fires AND calls `Injector->RevertAllActive()`, so manual injects of any scope can never
> leak into the dataset unlabeled (G63). **Entry-point parity:** StartRun itself pauses the
> auto-injector's Run and FinishRun resumes it (teardown-guarded by `bDeinitializing` — G62); the WS
> handler's local pause/resume is deleted — console and dashboard paths are the same code path. Console
> placeholders: `""` in leading arg slots resolves to the default (tokenizer quirk — G60). Sessions are
> named `session_<YYYYMMDD-HHMMSS>` (seed lives in `run.json`; same-second collisions get `-2/-3/...`),
> and `annotation.json` emits the client-shaped `affected_frames` object
> (`{start_frame, end_frame, frame_count, frame_indices}`; hide-type fires list only the observed hidden
> out-frames).
>
> **`annotation.json` field semantics (m22).**
> - **`frame_count` is a TRUE COUNT** = `len(frame_indices)`. It was a *span* (`end-start+1`) up to m21,
>   which disagreed with the index list on gapped (multi-toggle) events — that was G81, now fixed. The span
>   is still recoverable as `end_frame - start_frame + 1`.
> - **`anomaly_subtype` for `blink` is always `"disappear_reappear"`.** There is no per-event derivation;
>   the old transition-counting logic is deleted. `"flicker"` has left the blink family entirely and is
>   **reserved for the future separate `flickering` anomaly class** (unbuilt). Blink events may still
>   contain multiple visibility toggles — that is intended behaviour, not a defect, and does not change
>   the subtype. The `anomaly_subtype` field is retained in the schema for that future class.
> - **`affected_objects.nodes[]` carries `asset_name`, `component_class` and `bounds {origin, extent}`**
>   (m22/B1) so an auto-named level actor (`StaticMeshActor_###`) is identifiable. ⚠ **These four values
>   are sampled ONCE, at the event's ANCHOR FRAME** (the first captured frame of the event), exactly like
>   `global_position` and the camera block. **They are NOT per-frame truth** — for an actor that moves
>   during the event, `bounds` describes its anchor-frame pose, not its pose on every affected frame.
>   `asset_name`/`component_class` come from the actor's first visible `UMeshComponent`.
> - **`coverage_pct`** (m22/B2) is the event's screen-coverage percentage at the anchor frame — client-visible,
>   emitted in **both** modes, next to the existing `coverage_ratio`.
>
> **`selection_provenance.json` (m22/B2) — INTERNAL sidecar, NOT shipped.** Written at run finish next to
> `run.json`, **suppressed in delivery mode** (same gate as `labels.jsonl`/`run.json`). One record per fired
> event: `{anomaly_id, target, anchor_index, valid, coverage_pct, occlusion_samples_passed,
> occlusion_samples_total, poll_distance}`. It answers "why was this target selectable?" when a session is
> audited. Produced by `AnomalyViewport::EvaluateSelectionProvenance`, which is **observational only** — it is a
> standalone function that no selection code calls, so the early-out occlusion boolean still decides selection
> and the seeded selection sequence is unaffected. `poll_distance` is distance to the component's bounds
> **sphere**, so it goes negative when the pawn is inside those bounds. The dashboard is capture-first: Targeted/Auto-pool toggle, the auto panel is now the
> "Capture pool", and the manual Inject/Arg panels are deleted. This sits on the m9 session-capture layer
> (the quarantined `AnomalyCapture` module: N-frame cap, `session_<ts>/Actual_Frames`, native
> multi-anomaly `annotation.json`, host-side mp4 encode) and the fixed-timestep native-fps capture
> (`IAI.Capture.Fps`, `docs/capture-fps.md`) — both on master, not yet detailed in this header. See
> `sessions/2026-07-11-016-m10-targeted-capture.md`. Below this it still reflects:
> **Missing-Texture Anomaly (m8, VersionName 0.9.0)** — an 8th anomaly `missing_texture` (object-scoped):
> per-component `UMeshComponent::SetMaterial` swaps every renderable static/skeletal slot to a plugin-**shipped** Lit
> gray/white UV-checker material (object isolation; per-slot original captured for an exact revert). This is the plugin's
> **first `Content/` asset** — the cook guarantee is a CDO hard-ref (`FObjectFinder` + a non-transient `UPROPERTY`) in the
> injector subsystem, plus `"CanContainContent": true`, with no host config (gotchas G45/G47/G48). The material declares all
> mesh usage flags so it renders on Nanite/skeletal/ISM at runtime (G49). `IAnomaly` untouched; deps still
> `Core/CoreUObject/Engine/InputCore`; catalog now **8**. A flat-magenta variant + a `mode` arg are **deferred** (unlit-emissive
> magenta lit the Lumen scene / "glowed" — G50). See `sessions/2026-06-21-013-missing-texture.md`. Below this it still reflects:
> **Labeled Frame-Capture + 2D BBox Labeling (m7)** — a capture/labeling layer that produces an
> ML-friendly labeled image sequence from a LIVE auto-injection run. Housed in the **`AnomalyControlServer`**
> module (reuses its game-viewport capture primitive + ImageWrapper; gated by `ANOMALY_CONTROL_SERVER`, compiled
> out of Shipping). A new **`UAnomalyCaptureSubsystem`** (Game+PIE, dormant) drives the m6 auto-injector's
> deterministic core in **bursts** (`[pre] → FireOnce → [settle] → [positives] → RevertAllLiveFires → [settle]
> → [post]`, looped) and, per captured frame, writes the game-viewport image + a JSONL label record sourced from
> the auto-injector's own live-fire ground truth, stamped with one `GFrameCounter` (exact image↔label alignment).
> The 2D box projects the fired actor's persisted bounds via `AnomalyViewport::ProjectActorBoundsToScreenRect`
> (works even when the anomaly hid the actor). **Three sanctioned core exposures only** (`ProjectActorBoundsToScreenRect`;
> `FAutoLiveFireInfo` widened with the target actor + start-frame; `RevertAllLiveFires` exposed) — `IAnomaly`,
> the injector core, the anomalies, the leaf helpers, the `=` match, and `GetVisibleRenderableActors` are
> **byte-clean**. **No new dependency.** VersionName **0.8.0**. **Catalog: still 7** (capture is infrastructure,
> not a new anomaly). See `sessions/2026-06-20-012-frame-capture-labeling.md`. Below this it still reflects:
> **Automatic Injection (m6)** — a new, separate **`UAnomalyAutoInjectorSubsystem`**
> (Game+PIE world subsystem) that, while running, fires the four object-scoped anomalies **randomly on the
> renderable objects currently on-screen**, each auto-reverting after a randomized hold. Concurrent but
> **collision-free by construction** via a one-anomaly-per-actor scheduler invariant (no coordinator — G30);
> all randomness from one seeded `FRandomStream`. It calls only the injector's public `Apply`/`Revert` — the
> injector, `IAnomaly`, the anomalies, and the leaf helpers are **untouched**. Same explicit-core / thin-shell
> split as m5: a deterministic core (`AdvanceTime`/`TryFireOnce`, bridge-driveable as `IAI.Auto.Step`/`IAI.Auto.FireOnce`)
> under two thin shells (`IAI.Auto.*` console + a raw-input/`UDebugDrawService` HUD). Two switches, both default
> OFF → **dormant → existing gates byte-identical**: `IAI.Auto.Enable <0|1>` (HUD/keys) and `IAI.Auto.Run <0|1>`
> (firing). **No new dependency** (`FRandomStream` = Core). VersionName **0.7.0**. **Catalog: still 7 anomalies**
> (auto-injection is orchestration over the existing catalog — no new types).
> Below this it still reflects the **Object Selector + Inject UI (m5)** — a separate **`UAnomalySelectorSubsystem`**
> (Game+PIE world subsystem) that lets the player **select a visible on-screen object** (Tab-cycle over the
> m4 visible set) and **inject** one of the four object-scoped anomalies on it, then revert. It calls the
> existing injector's public `Apply`/`Revert` — the injector subsystem, `IAnomaly`, and the anomalies are
> **untouched**. Targeting is made precise by a new **`=` exact-match sentinel** in `AnomalyTargeting`
> (the only leaf-helper change; additive, substring path byte-identical). Activation is opt-in via
> **`IAI.SelectorUI <0|1>` (default OFF)** → dormant → existing gates byte-identical. First dependency
> addition since M0: **`InputCore`** (FKey/EKeys for raw input polling); the HUD is immediate-mode
> (`UDebugDrawService` + `UCanvas` + `DrawDebug*`, all Engine) so **no Slate/UMG** (m5 was VersionName 0.6.0).
> Below this it still reflects the **Viewport-Visibility Layer (m4)** — a shared **`AnomalyViewport`**
> helper (frustum AND occlusion against an explicit view) and an **opt-in** toggle `IAI.SetViewportScoping <0|1>`
> (default **OFF**) that routes the four object-scoped, primitive-backed anomalies — `missing_object`,
> `blinking`, `lod_corruption`, `lod_popping` — through it so they affect only objects visible in the
> player's viewport. Built on M3 (LOD breadth + `AnomalyLod`), M2 (component/global anomalies + A1/A3
> helpers), M2.5/M2.6 (5.1 port + bridge sever). **Catalog: still 7 anomalies** (the selector is UI over the
> existing catalog — no new types). Required **no `IAnomaly` change** (the M1 lock held again).
> **State-gated green** (clean Development-Editor compile on 5.1, exit 0; selector model driven over the bridge
> in a `MainWorld` Simulate session + OFF-is-byte-identical regression, session 009, 2026-06-19; viewport
> synthetic-view frustum+occlusion + regression gates, session 008, 2026-06-18). Detail in
> `sessions/2026-06-19-009-selector-inject-ui.md`, `sessions/2026-06-18-008-viewport-visibility-layer.md`,
> `sessions/2026-06-13-006-m3-lod-breadth.md`,
> `sessions/2026-06-10-005-m2.5-m2.6-5.1-port-bridge-sever.md`,
> `sessions/2026-06-09-004-m2-breadth-round-1.md`; M1 in `sessions/2026-06-09-003-m1-implementation.md`.
> **Maintenance:** update this file to match the code at the end of every milestone; describe only
> what is built. Forward plans and design rationale live in the session journals.

## Engine support
- **Supported engine: UE 5.1 (canonical).** The two real target games are on 5.1, so 5.1 is the engine
  of record. Originally built and validated on a source-built **UE 5.4.4**; ported to 5.1 in M2.5 with
  **zero plugin-source changes** — all port watch-items (UTickableWorldSubsystem signatures,
  `GetComponents<T>`, `SetVisibility`/`SetLightColor`, `SetForcedLodModel`/`GetNumLODs`,
  `GNearClippingPlane`/`r.SetNearClipPlane`, dep set) were unchanged between 5.4 and 5.1. The module
  pins no `CppStandard`, inheriting 5.1's C++17 default (5.4 defaulted to C++20); the code uses no
  C++20-only syntax. Build-version constants are a **host** concern (gotcha G17): the 5.1 host targets
  use `BuildSettingsVersion.V2` / `EngineIncludeOrderVersion.Unreal5_1`.
- **Canonical engine + host:** source-built UE 5.1 at `D:\UESource\UnrealEngine`; host project
  `D:\IntrusiveAnomalies\StackOBot` (natively-5.1 StackOBot). The old 5.4 host is retired.

## Purpose
AnomalyInjector injects labeled visual anomalies (graphics bugs) into a running UE5 game to
generate synthetic training data for bug-detection ML. Game-agnostic (public UE APIs only); tested
on the Stack O Bot sample.

## Module & load
- One **Runtime** module `AnomalyInjector`, `LoadingPhase Default`, `EnabledByDefault: true`
  (project-plugin scoped — gotcha G6), `VersionName 0.6.0`. Build.cs deps: `Core`, `CoreUObject`, `Engine`,
  **`InputCore`** — InputCore is the **first dep added since M0** (m5 selector: `FKey`/`EKeys` for raw input polling +
  configurable keybinds). It is already a *public* dependency of Engine so it was transitively available; it is
  declared explicitly for IWYU hygiene. The selector's HUD is **immediate-mode** (`UDebugDrawService` + `UCanvas` +
  `DrawDebug*`, all Engine) so **no Slate/SlateCore/UMG** was needed (gotcha G27). Everything through the viewport
  milestone stayed Engine/Core only: the A1 component finder, A3 arg parsing, the `AnomalyLod` LOD helper, every
  anomaly, and the `AnomalyViewport` helper. `AnomalyViewport`'s frustum
  (`FConvexVolume` / `GetViewFrustumBounds` / `FMinimalViewInfo`), line-trace occlusion
  (`UWorld::LineTraceSingleByChannel`), and live view resolution (`APlayerController` / `APlayerCameraManager`)
  are all Engine; `FReversedZPerspectiveMatrix` is Core. `camera_clipping` drives the near clip via the
  `r.SetNearClipPlane` console command + the `GNearClippingPlane` global (Core), deliberately avoiding a
  `RenderCore` dependency (gotcha G13).

## The anomaly abstraction — `IAnomaly`  (`Public/IAnomaly.h`)
Plain C++ polymorphic interface (NOT a UCLASS — dispatch needs no reflection). One instance per type.
```
GetId() / GetDescription() / GetUsage()   // identity + help
Apply(UWorld*, const TArray<FString>&)->bool   // true iff an observable effect was applied
Tick(float DeltaSeconds)                  // no-op default; override only for ticking anomalies
Revert()                                  // undo everything; leaves IsActive()==false
IsActive()->bool
```
Contract: `Apply` returns **false** (and stays inactive) when an actor anomaly matches zero actors
(AMB-2); re-applying an active anomaly **reverts-then-reapplies** (no state leak); anomalies cache
their targets/world as `TWeakObjectPtr` inside `Apply` (GC-safe) so `Tick` needs only `DeltaSeconds`.

## Core component — `UAnomalyInjectorSubsystem` (the manager)
- A `UTickableWorldSubsystem` (UCLASS): one per world, auto-ticks, `GetWorld()`. **Game + PIE only**
  via `DoesSupportWorldType` (gotcha G7).
- **Owns the registry** `TMap<FName, TUniquePtr<IAnomaly>>` — plain C++, not a UPROPERTY. Registers
  one instance of each anomaly type in `Initialize` (explicit, no self-registration macros). Needs an
  out-of-line destructor (gotcha G9).
- **Tick:** drives `Tick(Dt)` on the active anomalies; the 2 s heartbeat now reports `(active: N/Total)`.
- **Deinitialize:** `RevertAllActive()` then `Super` — the generalized auto-restore-on-teardown.
- **Re-entrancy:** dispatch is thin; each anomaly's `Apply` does the revert-then-reapply.

## Lifecycle / targeting shapes (why these seven anomalies)
The catalog deliberately spans the axes the interface must cover, proving it generalizes. M1 proved
the lifecycle axis (static / ticking / global); M2 proved the targeting & mutation axis (component-
scoped across two component types, and a global driven by a console command); M3 proved a **heterogeneous
target set** (one apply spanning static + skeletal components, dispatched per type behind a shared helper):
| anomaly | scope | ticks? | proves |
|---|---|---|---|
| `missing_object` | actor | no | the static, actor-scoped baseline (re-homes the M0 hide) |
| `blinking` | actor | **yes** | the `Tick` path |
| `time_dilation` | world-global | no | the interface does **not** assume actor-scoping |
| `lighting_mismatch` | **component** (ULightComponent) | no | component-level targeting (A1) + per-target full-state capture + multi-mode args |
| `lod_corruption` | **component** (static + skeletal mesh) | no | one capture convention over a **heterogeneous** target set; static/skeletal dispatch via `AnomalyLod` (M3) |
| `lod_popping` | **component** (static + skeletal mesh) | **yes** | the `Tick` path reused (blinking mechanics) over the `AnomalyLod` LOD dispatch (M3) |
| `camera_clipping` | global (near-clip) | no | global capture/restore via a console **command** (no cvar, no new dep) |

## Shared helpers

### Targeting — `AnomalyTargeting`  (`Public/AnomalyTargeting.h`)
Free functions (deliberately not a base class). Single source of truth for the label-free match rule.
- `FindActorsMatching(World, Query)` — matches by `Actor->GetName()` **or**
  `Actor->GetClass()->GetName()` `.Contains(query)` (case-insensitive), **never** `GetActorLabel()`
  (editor-only — gotcha G2). Returns weak-ptrs. Used by `missing_object`, `blinking`.
  **Exact-match sentinel (m5):** a leading `=` on `Query` (e.g. `=SM_Ramp2_UAID_…`) strips the `=` and switches to
  full-name **equality** (`Equals(IgnoreCase)`) instead of substring — so the selector's `InjectSelected()` (which passes
  `"=" + Actor->GetName()`) targets **exactly** the selected actor and never a same-prefixed sibling (`=Cube` ≠ `Cube2`).
  Object names cannot contain `=`, so this never collides with a real console substring query, and the substring path is
  **byte-identical** when there is no `=`. Because every object-scoped path funnels through here (also via
  `FindComponentsMatching<T>` / `AnomalyLod` / the `AnomalyViewport` finders), all four object-scoped anomalies inherit
  exact targeting with **no anomaly edits and no `IAnomaly` change**. This is also the primitive the future
  auto-injection path will use. Exact-*name* is the v1 identity ceiling (streamed-sublevel duplicate names would need
  pointer identity = an `IAnomaly` change — accepted limit; gotcha G28).
- `FindComponentsMatching<T>(World, Substring)` (**A1**, header-only template) — resolves matching
  actors via `FindActorsMatching` (same rule), then gathers each actor's components of type `T`
  (`AActor::GetComponents<T>`). Returns `TArray<TWeakObjectPtr<T>>`. Handles standalone light/mesh
  actors (class name carries the substring, e.g. `APointLight`) and lights/meshes-as-components on
  other actors uniformly. Used by `lighting_mismatch` (`<ULightComponent>`) and `lod_corruption`
  (`<UStaticMeshComponent>`) — two consumers, two component types.

### Argument parsing — `AnomalyArgs`  (`Public/AnomalyArgs.h` / `Private/AnomalyArgs.cpp`)
**A3.** `GetFloat / GetInt (value, Index, Default, Min, Max)` and `GetString (value, Index, Default)`.
Consolidates the AMB-6 parse/clamp/warn behavior: missing index → `Default` (silent); non-numeric →
warn + `Default`; out-of-range → warn + clamp; **never fails `Apply`**. Used by the M2 anomalies and
by `lod_popping` (Hz). (M1's `blinking`/`time_dilation` keep their inline parse — validated code left
untouched; the cosmetic divergence is intentional, not a TODO.)

### LOD forced-LOD dispatch — `AnomalyLod`  (`Public/AnomalyLod.h` / `Private/AnomalyLod.cpp`)  **(M3)**
Free functions (AnomalyTargeting/AnomalyArgs convention), justified by **2 consumers** (`lod_corruption`,
`lod_popping`). Single source of truth for forced-LOD across the two LOD-forceable component families —
`UStaticMeshComponent` (`SetForcedLodModel`/`ForcedLodModel`, count via `GetStaticMesh()->GetNumLODs()`)
and `USkinnedMeshComponent` (`SetForcedLOD`/`GetForcedLOD`, count via the component's own `GetNumLODs()`;
`USkeletalMeshComponent` derives). Both APIs are **1-based** (0 = auto/off; N forces LOD N-1). Surface:
- `ResolveLodComponents(World, Substring)` → merges `FindComponentsMatching<UStaticMeshComponent>` and
  `<USkinnedMeshComponent>` (disjoint siblings → duplicate-free) into `TArray<TWeakObjectPtr<UMeshComponent>>`.
- `GetWorstLod(Comp)` / `GetForcedLod(Comp)` / `SetForcedLod(Comp, n)` — dispatch on the concrete type via
  `Cast<>` internally, so callers hold one record keyed to the common base `UMeshComponent`.
- `ResolveTargetLod(Comp, RequestedOrSentinel)` — `WorstLodSentinel` → that component's worst LOD; an
  explicit 1-based index → clamped to `[1, max(WorstLod,1)]` (the default-worst / explicit-clamp rule).
The skinned LOD count uses the **runtime render-data** accessor `USkinnedMeshComponent::GetNumLODs()`,
the analog of the static `UStaticMesh::GetNumLODs()` — **not** the asset's authored `GetLODNum()`
(gotcha G19). All types are in `Engine` → **no new module dependency**.

### Viewport visibility — `AnomalyViewport`  (`Public/AnomalyViewport.h` / `Private/AnomalyViewport.cpp`)  **(viewport milestone)**
Free functions (the AnomalyTargeting/AnomalyArgs/AnomalyLod convention: light Public header, heavy includes +
backend dispatch in the .cpp). Single source of truth for "is this object actually visible to the player" =
**inside the camera frustum AND not occluded**. Lets the four object-scoped anomalies be opt-in scoped so a
corrupted frame is labeled-and-visible (the ML-relevant case), not labeled-but-invisible.
- **Core operates on an explicit view spec** `FAnomalyViewInfo { Origin, Rotation, HorizontalFOVDeg, AspectRatio,
  bValid }` + the world. Pure function of (view, world) → deterministic and **state-gatable with a synthetic
  view** (no live player needed). This split is deliberate: it keeps the live lookup thin and separately validated.
- **Frustum (always; synchronous):** assemble the reversed-Z VP from the view spec exactly as the engine's live
  path (`FReversedZPerspectiveMatrix` via `FMinimalViewInfo::CalculateProjectionMatrix` + the world→view basis
  swap), `GetViewFrustumBounds(VP, bUseNearPlane=true, bUseFarPlane=false)`, then `FConvexVolume::IntersectSphere`
  then `IntersectBox` against `UPrimitiveComponent::Bounds` (gotcha G24).
- **Occlusion (backend-agnostic; PRIVATE to the .cpp):** v1 = multi-sample **camera-to-bounds line trace**
  (`UWorld::LineTraceSingleByChannel`, `ECC_Visibility`; bounds center + 8 corners; ignore the target's own
  actor; unoccluded if **any** sample's path is unblocked). Synchronous, deterministic, synthetic-gatable.
  `UPrimitiveComponent::GetLastRenderTimeOnScreen()` is the documented drop-in **live** backend for the future
  capture/live-injection milestone — a .cpp-only swap (gotcha G22). Trade-off: the trace over-includes on
  no-collision / translucent occluders (safe direction — never drops a visible target).
- **Public surface:** `IsComponentVisible(View, World, Comp)`, `IsActorVisible(View, World, Actor)` (disjunction
  over the actor's primitive components — actor granularity); filter entry points `FilterVisibleActors(...)` and
  header-only `FilterVisibleComponents<T>(...)`; composed convenience finders wrapping AnomalyTargeting —
  `FindVisibleActorsMatching(World, Sub)` and header-only `FindVisibleComponentsMatching<T>(World, Sub)`. Also
  `IsComponentInFrustum(View, Comp)` — **frustum-only, NOT the visibility predicate** (visibility = frustum AND
  occlusion is the load-bearing invariant; this primitive exists for the synthetic-gate diagnostics / frustum
  calibration and must not be read as "is visible"). The occlusion test stays private to the .cpp.
- **Live resolver:** `GetActiveViewInfo(World, OutView)` fills the view spec from the first local player's POV
  (`APlayerController::GetPlayerViewPoint` + `PlayerCameraManager->GetFOVAngle()`; aspect from the game viewport).
  On no usable view it logs **one** warning, leaves `bValid=false`, and returns false; the convenience finders
  then return the **full matched set (treat-as-unscoped)**, never dropping targets. No editor-viewport fallback
  (no UnrealEd dep). A StackOBot Simulate session *does* expose a usable view (gotcha G23).
- **No per-frame cache in v1** (matched sets are small; visibility is tested once at Apply, not per tick). A
  frame-keyed memo of (view + per-primitive result) slots in here for the future live-injection milestone.
- All types are in `Core`/`Engine` → **no new module dependency**.

**Renderable-visible set (m5 follow-on; additive — the functions above are unchanged).** For the object selector and
future auto-injection, "visible" must mean **visible AND renderable**: in-frustum, unoccluded, and actually drawing
geometry. A pure frustum+occlusion test also passes non-rendering primitives (collision boxes, capsules, RVT bounds
boxes, editor billboards, landscape, debug/streaming actors), which must never be injectable targets. New entry points:
- `IsRenderableComponent(Comp)` = `Comp->IsVisible()` **AND** a base-TYPE allowlist
  `IsA<UStaticMeshComponent>() || IsA<USkinnedMeshComponent>()`. A capability/type test, **not a class blocklist**
  (game-agnostic). **VFX removed (G33):** the original `|| IsA<UFXSystemComponent>()` clause (Niagara + Cascade) was
  deliberately dropped — particles are not useful injectable-geometry targets for the selector / auto-injector /
  dashboard set; the `=name` console escape hatch still reaches VFX actors directly (it bypasses this predicate).
  `IsVisible()` (not `ShouldRender()`) is deliberate — `ShouldRender()` has a non-shipping branch that returns true for
  hidden collision components, a determinism footgun (gotcha G29). `ULandscapeComponent` is a documented one-line
  extension point in the predicate, intentionally inactive (landscape excluded for v1). **Empty-instance refinement:**
  an instanced static mesh / HISM with **zero instances** draws nothing, so it is treated as non-renderable
  (`GetInstanceCount() > 0` required for ISMs) — this drops 0-instance landscape-grass ISMs (which would otherwise leak a
  `LandscapeStreamingProxy` in) while keeping real foliage and populated ISMs. So **renderable = a visible SM/SK
  component that actually draws something (instanced ⇒ instance count > 0)**.
- `IsActorRenderableVisible(View, World, Actor)` (any component passes), `FilterRenderableVisibleActors(View, World, In)`,
  and `GetVisibleRenderableActors(World)` (resolve view → enumerate all actors → filter; the selector/auto-injection
  entry point). The renderability check runs **first** in the per-component test, before the occlusion traces (perf win).
- **No-view contract:** `GetVisibleRenderableActors` returns **empty** on no resolvable view (offer nothing, never
  blind) — deliberately **distinct** from the `FindVisible*Matching` finders' treat-as-unscoped. Two callers, two safe
  directions (console = act-don't-drop; selector/auto-injection = offer-nothing). Do not reconcile them (G29).
- **Poll-radius distance cull (changeable; default OFF — G34).** An optional cull layered onto the two LIVE poll
  entry points (`GetVisibleRenderableActors` + `GetVisibleRenderableActorInfos`): with a positive radius **R**, an actor
  is in the set iff renderable AND within **R** of the **player pawn** (sphere-approx bounds metric
  `Dist(PollOrigin, Bounds.Origin) - SphereRadius <= R`) AND in-frustum AND unoccluded. Single shared state in
  `AnomalyViewport`, set via `IAI.SetPollRadius <cm>`; `R <= 0` disables it → byte-identical to no-cull. The cull runs
  after the renderable type-test and before the occlusion traces (cheapest-cull-first), is threaded through the shared
  chokepoint so **both** live entry points apply it identically (the `IAI.DumpVisible` set-identity gate holds), and the
  explicit-view functions (`IsActorRenderableVisible` / `FilterRenderableVisibleActors`) pass radius 0 (synthetic
  surface stays byte-identical). When `R > 0` a dev debug sphere (drawn from a pre-render world-tick hook, `FWorldDelegates::
  OnWorldPostActorTick` — G34) visualizes the radius around the live pawn; it is **suppressed during a capture run** so it
  is not baked into captured frames (`SetDebugSphereSuppressed`, visual-only — the cull is unaffected, G44). Origin is the
  **pawn** (not the camera); the dashboard's `Distance` field stays camera-relative — distinct on purpose. All types are in
  `Core`/`Engine` → no new dep.
- **Screen-coverage candidate cull (changeable; default OFF — G51).** A second optional cull layered onto the two LIVE
  poll entry points, sibling to the poll-radius cull but **actor-level**: with a positive percentage **P**, an actor is in
  the set iff renderable-visible (renderable AND in-frustum AND unoccluded AND within the poll radius) **AND** its
  on-screen footprint covers **≥ P%** of the viewport. Footprint = the **clamped [0,1] screen AABB of the UNION of the
  actor's renderable-VISIBLE component bounds** (only the components that passed the per-component test — so one anomaly
  on a multi-part bot reads the whole bot), projected with the same reversed-Z VP the frustum uses; coverage = that
  rect's area (the rect is normalized, so the viewport area = 1). **Clamp-before-area** means a huge object with only a
  tiny on-screen sliver reads as the sliver, not full coverage; a behind-camera / off-screen union reads 0 → culled.
  Single shared state in `AnomalyViewport`, set via `IAI.SetMinScreenCoverage <pct>`; `P <= 0` disables it → byte-identical
  to no-cull. **Shared classifier (the key structural change):** the two live entry points had no single actor-aggregation
  point (each ran its own actor loop, sharing only the per-component worker `FirstRenderableVisibleComponent`); coverage
  is applied through a new shared per-actor decision `ClassifyRenderableVisibleLive`, which **both** loops now call so the
  cull is applied identically (the `IAI.DumpVisible` set-identity gate holds with the cull ON). **OFF is byte-identical in
  result AND cost** — the classifier keeps the cheap first-match short-circuit and runs no union pass / projection when
  `P <= 0`; **ON** does a single union pass that yields the first match + the union bounds (no double occlusion tracing),
  then the coverage gate. `GetVisibleRenderableActors` now builds the VP explicitly and derives the frustum from it
  (identical to `GetVisibleRenderableActorInfos`) so both pass the same projector to the gate. The explicit-view functions
  (`IsActorRenderableVisible` / `FilterRenderableVisibleActors`) are **not** culled (synthetic surface, conceptually
  `P = 0`). It is the **most expensive, actor-level** gate, so it runs **last**; composes with the poll-radius cull
  (independent gates). The projection reuses the existing clamped `ProjectBoundsToScreenRect` (the dashboard/A4 rect
  projector) fed the visible-component union — **not** the m7 type-only/unclamped `ProjectActorBoundsToScreenRect`
  (coverage wants the true clamped visible footprint, per the granularity + clamp-before-area rules). Tuning companion:
  `IAI.DumpCoverage` logs every renderable-visible (pre-coverage) actor's coverage %, ascending, marking which the current
  threshold would cull. All types are in `Core`/`Engine` → no new dep. **Also exposed on the Tier-2 dashboard** via the
  control-server WS command `set_min_screen_coverage {pct}` → `SetMinScreenCoveragePct`, plus the `session.minScreenCoverage`
  snapshot field (percent; 0 = OFF) for the live slider — mirroring poll-radius (`set_poll_radius` / `session.pollRadius`).
- All types are in `Engine` → **still no new module dependency**.

## Control surface (console commands)
Module-scoped `FAutoConsoleCommandWithWorldAndArgs`, resolved from the console's world, null-guarded
(warns outside Game/PIE). Output → Output Log, category `LogAnomaly`.
- `IAI.ListActors` — log `Class | Name | Label` per actor (targeting aid, not an anomaly).
- `IAI.ListAnomalies` — list registered anomalies as `id - description - usage` (sorted).
- `IAI.Apply <id> <args...>` — look up id, apply (reverts-then-reapplies if active).
- `IAI.Revert <id>` — revert one active anomaly.
- `IAI.RevertAll` — revert all active anomalies.
- `IAI.SetViewportScoping <0|1>` — toggle viewport-visibility scoping for the four object-scoped anomalies
  (default **OFF**; see "Viewport-visibility scoping" below).
- `IAI.SetPollRadius <cm>` — set the renderable-visible **poll-radius distance cull** around the player pawn (G34);
  `<= 0` disables it. **Default `1800` cm (= 18 m), i.e. the cull is ON out of the box (m19).** Affects the selector /
  auto-injector / dashboard set (all consume the live renderable-visible poll). No argument → prints the current radius.
  Registered in `AnomalyViewport.cpp` (it sets a world-independent global, so unlike the other `IAI.*` commands it is a
  plain `FAutoConsoleCommand`).
- `IAI.SetMinScreenCoverage <pct>` — set the renderable-visible **screen-coverage cull** (percent of viewport area, G51);
  `<= 0` disables it. **Default `6` %, i.e. the cull is ON out of the box (m19).** Affects the selector / auto-injector /
  dashboard set (all consume the live renderable-visible poll). No argument → prints the current value. Registered in
  `AnomalyViewport.cpp` as a plain world-independent `FAutoConsoleCommand` (like `IAI.SetPollRadius`).
- **Targeting defaults + who owns them (m19).** The three targeting defaults are **hardcoded engine constants and the
  ENGINE IS AUTHORITATIVE** — a packaged client build with no dashboard starts correct on its own:
  `GPollRadius = 1800.0f` and `GMinScreenCoveragePct = 6.0f` (`AnomalyViewport.cpp`, file-scope globals — NOT
  ini-backed; ini-backing via GConfig remains available as a follow-up if per-title tuning is ever wanted), and the
  auto-pool's **default-enabled** set `GAutoPoolDefaultEnabled = { blinking, missing_texture,
  corrupted_texture, lod_popping, camera_clipping }` (`AnomalyAutoInjectorSubsystem.cpp`, consumed in
  `Initialize`; m30 — the M2 pool). `GAutoPool` offers all six ids
  (`missing_object` remains selectable, just **not enabled by default**), and `SetAllAnomaliesEnabled(true)` still means
  *all* of `GAutoPool` — it is an explicit action, not a default. **The dashboard has NO defaults of its own for these:**
  the sliders and the pool checkboxes are pure mirrors of the snapshot (`session.pollRadius`,
  `session.minScreenCoverage`, `auto.pool[id]` ← `Auto->IsAnomalyEnabled`), so engine and UI cannot drift. **Note the
  poll-radius cull subtracts the bounds sphere radius**, so very large actors are never distance-culled; and a non-zero
  *default* does not register the dev debug sphere (that only happens on an explicit `SetPollRadius` OFF→ON
  transition) — which is what a client build wants. G80.
- `IAI.DumpCoverage` — **diagnostic** (not a cull): log every renderable-visible (pre-coverage) actor with its on-screen
  coverage %, sorted ascending, marking which the current `IAI.SetMinScreenCoverage` threshold would cull. The threshold
  tuning companion. Registered in `AnomalyViewport.cpp` (world-dependent, so a `FAutoConsoleCommandWithWorldAndArgs`).
- `IAI.TestVisibility <substring> <ox oy oz> <pitch yaw roll> [fovDeg] [aspect]` — **diagnostic** (not an
  anomaly): test the `AnomalyViewport` core against a **synthetic** view and log per-component
  `frustum / unoccluded / visible`. The deterministic synthetic-view state-gate driver.
- `IAI.Anomaly.BlinkHalfPeriod <frames|default>` / `IAI.Anomaly.LodHalfPeriod <frames|default>` (session 051) —
  set the **AUTO-POOL** default half-period for `blinking` / `lod_popping`, in FRAMES. Registered in
  `AnomalyDefaults.cpp` as plain world-independent `FAutoConsoleCommand`s. Range `[1..600]`; an out-of-range
  value is **REFUSED, never clamped** (a clamp turns a typo into a different cadence that still looks
  deliberate — `G144`'s shape). `default` clears the override. Both print an `EFFECTIVE READ-BACK`.
  **PRECEDENCE, four levels: a TARGETED fire's own argument > this console override > `DefaultGame.ini`
  `[AnomalyInjector]` `BlinkingHalfPeriodFramesDefault` / `LodPoppingHalfPeriodFramesDefault` > the compiled
  default (3 / 8).** The console form exists because **`G88` — a loose ini beside a package is a NO-OP, the
  cooked config wins — so on a packaged client build an ini-only lever would still need a re-cook, and the
  client ships an AUTO-POOL config.** ⚠ **Absent override and absent ini key ⇒ compiled default, byte-identical
  to a build without them; this is a LEVER, not a change.** The canonical constants live in `AnomalyDefaults`
  and each anomaly's `Apply` carries a `static_assert` tying its own constant and clamp to them, so a drift
  breaks the build rather than the artifact.

Object Selector + Inject UI (m5) — drive the `UAnomalySelectorSubsystem` (these are the bridge **thin-shell** over
its public methods; the keys + HUD are the separate real-Play eyeball shell):
- `IAI.SelectorUI <0|1>` — enable/disable the selector UI (default **OFF**; dormant when OFF).
- `IAI.Selector.Next` / `IAI.Selector.Prev` — select the next/previous visible actor (name-sorted).
- `IAI.Selector.Cycle` — cycle the chosen anomaly across the four object-scoped ids.
- `IAI.Selector.Inject` — inject the chosen anomaly on the selected actor (exact-name target, default args).
- `IAI.Selector.Revert` — revert the last anomaly the selector injected.
- `IAI.Selector.Status` — log the selected actor, the visible-set names, and the chosen anomaly (the state-gate readback).
- `IAI.SelectorBind <next|prev|cycle|inject|revert> <KeyName>` — rebind a key (validated via `EKeys::GetKeyDetails`).

Automatic Injection (m6) — drive the `UAnomalyAutoInjectorSubsystem` (console thin-shell; the keys + HUD are the
separate real-Play eyeball shell). `Step`/`FireOnce` drive the deterministic core directly (no Enable/Run needed):
- `IAI.Auto.Enable <0|1>` — eyeball shell on/off (HUD + key polling). Default **OFF** → dormant.
- `IAI.Auto.Run <0|1>` — start/stop the auto-tick firing loop (requires Enable; re-seeds + arms the first interval).
- `IAI.Auto.Seed <int>` — set the run seed (re-initializes the stream now; default seed is time-based).
- `IAI.Auto.Pool <id|all> <0|1>` — enable/disable a pool id (or all four) for firing.
- `IAI.Auto.Interval <minSec> <maxSec>` / `IAI.Auto.Hold <minSec> <maxSec>` — inter-fire interval / per-fire hold ranges.
- `IAI.Auto.MaxConcurrent <n>` — cap on concurrent live fires (also naturally bounded by the enabled-id count).
- `IAI.Auto.Persist <0|1>` — persist-until-manual (default OFF = auto-revert after the hold).
- `IAI.Auto.Step <seconds>` — advance the scheduler by N seconds (deterministic core drive; the bridge gate timing knob).
- `IAI.Auto.FireOnce` — force one fire attempt now (deterministic core drive).
- `IAI.Auto.Status` — log enable/run state, seed, cadence, the enabled set, and the live fires (the state-gate readback).
- `IAI.Auto.Bind <pool1|pool2|pool3|pool4|run|reseed> <KeyName>` — rebind a key (validated via `EKeys::GetKeyDetails`).

Capture & Labeling (m7) — drive the `UAnomalyCaptureSubsystem` (in the `AnomalyControlServer` module; only when
`ANOMALY_CONTROL_SERVER=1`). Narrow the fired types via `IAI.Auto.Pool` first; the auto-injector's `Run` must be OFF:
- `IAI.Capture.Shot [outDir] [png|jpeg]` — capture ONE labeled frame now (default dir `<ProjectSaved>/AnomalyCaptures/manual`).
- `IAI.Capture.Config <settleK> <preFrames> <positiveFrames> <postFrames> <burstCount>` — set the burst schedule (burstCount 0 = until Stop).
  ⚠ **`preFrames` is a ONE-TIME LEAD-IN — it runs once per RUN, not once per burst. The CLEAN GAP between
  annotated windows is governed by `postFrames` (`G160`).** Measured: `2 14 8 4 0` leaves the gap at **4**;
  `2 4 8 14 0` gives **14**. The schedule applies identically on the TARGETED and the AUTO-POOL paths — the
  capture FSM is targeting-agnostic and `bTargetedMode` only selects which fire route `BeginFire()` takes.
  **That gap is the CEILING on any offset measurement whose baseline comes from the annotation** (about ±2
  frames on the shipped `2 4 8 4 0`), so a diagnostic capture wants `2 4 8 14 0` — and
  `tools/measure_label_offset.py --require-gap N` should enforce it rather than anyone trusting arithmetic.
- `IAI.Capture.TickPin <0|1>` (session 051) — **THE BISECT SWITCH FOR THE CAPTURE-TIME ENGINE TICK-MODE PIN**,
  in the `IAI.Capture.SVE 0` tradition: one setting reaching the other behaviour with **no rebuild and no
  re-cook**. On a decoupled-tick engine fork the pin forces the fixed-sim/variable-render mode OFF for the
  duration of a capture (saved at run start, **re-applied every capture tick as a SET, never a toggle**, because
  the host re-asserts it; restored at finish). **Precedence: this console override > `DefaultGame.ini`
  `[AnomalyCapture]` `bTickModePinDefault` > compiled default (on where the fork is detected).** It exists
  because **`G88`** — without it the unpinned control leg would cost a second COOK. Mid-run changes ignored.
  ⚠ **On a build where the pin compiled out the command STILL EXISTS and says so** — a silently missing command
  on the host that matters is the failure mode we refuse. All fork-touching code sits behind the build-time
  probe define `ANOMINJECT_FW_TICKPIN` (marker file `FWNetSubsystem.cpp`; the probe logs its result either way)
  and compiles out entirely on stock engines, so the core module stays fork-blind. Exactly one greppable
  `TICKPIN` line per run states the effective value and its source; `run_summary` carries
  `tickpin_compiled/applied/saved/reasserts`, `capture_game_ticks` and `ticks_per_captured_frame`
  (**`annotation.json`'s field set does NOT move** — the m27 precedent).
- `IAI.Capture.ViewLag <frames>` — bbox-projection view-lag L (default **0**; see "Capture & Labeling" below).
- `IAI.Capture.Start [outDir] [png|jpeg] [seed]` — start a burst run (default dir `<ProjectSaved>/AnomalyCaptures`; png; seed = auto-injector's current).
- `IAI.Capture.Stop` — stop the run (reverts in-flight fire, writes `run_summary.json`; cancels an armed-pending run cleanly).
- `IAI.Capture.Status` — log run state, config, counters.
- `IAI.Capture.FocusGate <0|1>` — gate the first captured frame on game-window focus (default ON; packaged default `[AnomalyCapture] bFocusGateDefault`). Start arms immediately but holds the first frame until the game window has focus; skipped when there is no game window (headless/Simulate); 30 s safety timeout starts anyway. **(m16)**
*(M0's `IAI.HideActor` / `IAI.ShowAllActors` were removed — superseded by `IAI.Apply missing_object`
/ `IAI.RevertAll`.)*

## Anomaly catalog
| id | shape | usage | effect | revert | status |
|----|-------|-------|--------|--------|--------|
| `missing_object` | static, actor-scoped | `IAI.Apply missing_object <sub>` | `SetActorHiddenInGame(true)` on matches | un-hide / RevertAll / teardown | **as-built (M1)** |
| `blinking` | ticking, actor-scoped | `IAI.Apply blinking <sub> [hz]` | toggle hidden each half-period (default 5 Hz, clamp 60) | restore visible (any phase) | **as-built (M1)** |
| `time_dilation` | world-global, no tick | `IAI.Apply time_dilation <scale>` | `SetGlobalTimeDilation(scale)` (clamped — G11) | restore captured baseline (AMB-3) | **as-built (M1)** |
| `lighting_mismatch` | component (ULightComponent) | `IAI.Apply lighting_mismatch <sub> [off\|dim <f>\|recolor <r g b>\|noshadow]` | per mode: `SetVisibility(false)` / `SetIntensity(orig*f)` (def 0.1) / `SetLightColor(r,g,b)` (def magenta) / `SetCastShadows(false)`; default mode `dim` | restore captured intensity/color/visibility/cast-shadow per live comp; skip stale | **as-built (M2)** |
| `lod_corruption` | component (static **+ skeletal** mesh) | `IAI.Apply lod_corruption <sub> [lod-index]` | force each matched comp to a LOD via `AnomalyLod` (1-based; default worst per comp; explicit index clamped per comp). Static `SetForcedLodModel` / skinned `SetForcedLOD` | restore captured forced-LOD per live comp; skip stale | **as-built (M3)** — static + skeletal (G19; was static-only in M2, G16) |
| `lod_popping` | component (static **+ skeletal** mesh), **ticking** | `IAI.Apply lod_popping <sub> [half_period_frames]` | each half-period **counted in FRAMES** (default **8**, range 1..600), snap every matched comp between its captured baseline LOD and its worst LOD via `AnomalyLod`. **A matched component qualifies only if `AnomalyLod::HasMultipleLods` (≥2 LODs); all-refused ⇒ Apply returns false ⇒ no fire, no label** | restore captured baseline per live comp regardless of phase; reset frame counter/phase | **as-built (M3; m29 = frames + ≥2-LOD guard)** — ⚠ **the guard is NECESSARY BUT NOT SUFFICIENT, see below** |
| `camera_clipping` | global (near-clip), no tick | `IAI.Apply camera_clipping [near]` | `r.SetNearClipPlane <near>` console command (default 100), pushing `GNearClippingPlane` out | restore captured baseline (~10) via the same command | **as-built (M2)** |
| `missing_texture` | actor-scoped (per-component `SetMaterial`), no tick | `IAI.Apply missing_texture <sub>` | swap every renderable SM/SK component's material slots to the plugin-shipped **Lit gray/white UV-checker** (per-component override → object isolation; never mutates the shared mesh/material asset) | **re-find + guarded restore (m17, see below)** — never trusts the saved component ptr | **as-built (m8, revert hardened m17)** — the flat-magenta variant deferred at m8 shipped instead as its own id, `corrupted_texture` |
| `corrupted_texture` | actor-scoped (per-component `SetMaterial`), no tick | `IAI.Apply corrupted_texture <sub>` | swap every renderable SM/SK component's material slots to the plugin-shipped **Lit solid-magenta, OPAQUE, two-sided** material `M_CorruptedTexture_Pink` (same per-component override → object isolation) | **the m17 contract, mirrored**: re-find live component → touch a slot only if it still holds OUR pink → restore / default-reset → sweep successors | **as-built (m29)** — distinct from `missing_texture`, which is CHECKERED |

### ⚠ `lod_popping`'s ≥2-LOD guard is NECESSARY BUT NOT SUFFICIENT (m29, MEASURED)

The guard refuses a component with a single LOD, because forcing a LOD there pops it **to itself**:
no visible change, positive label. 🚨 **It does NOT catch the other route to the same outcome — a
multi-LOD mesh that is simply TOO FAR AWAY / TOO SMALL ON SCREEN for its LODs to differ visibly.**

Measured on `SM_rock` (4 LODs, non-Nanite), LOD 1 vs LOD 4, two legs at a matched camera, whole-frame
pixels differing by ≥8/255: **33.04 % bounds coverage → 66,615 px (plainly visible)** ·
**9.35 % → 12,489 px (visible)** · farther rungs → **14 px** and **8 px (not visible)**. The missing
variable is **ON-SCREEN SIZE**, not LOD authoring quality.

🚨 **Nothing downstream catches the admitted case. The m26 mask veto CANNOT** — the object still
draws, so it reads `MEASURED_NONZERO` and the event survives; and the mask measures the
**silhouette**, which is what a distant LOD swap barely moves. **The gate is therefore at PICK TIME.**

✅ **CLOSED AT m30 — the proximity gate.** `lod_popping` requires its own minimum
**bounds-projected screen coverage at pick time**, `MinCoveragePct = 7.0`, stacked on the ≥2-LOD
guard. Below it, Apply returns false through the same AMB-2 matched-zero path — no fire, no label.
Bounds only, no pixel read (`G127`-safe). Calibrated, not chosen: **9.3453 % visible / 3.9045 %
invisible**, with the visible signal collapsing three orders of magnitude between them; the threshold
sits at **1.79× above the invisible anchor and 1.34× below the visible one**, biased toward refusing
because a positive label with no visible change is the dataset-poisoning direction.

⚠ **ONE QUANTITY THROUGHOUT: bounds coverage, never drawn extent.** They differ by ~4× — the
MainWorld rock reads **11.83 % bounds while drawing 2.78 % of frame** — so mixing them would silently
move the threshold. The gate, both anchors and every recorded number all name the same quantity.

### `camera_clipping` is a SESSION-GLOBAL pool member (m30)

The first Global-scoped id in the pool. It is **held for the whole capture session** — applied in
`BeginActualRun` (after `StartRun`'s clean slate, so the slate cannot revert it) and reverted in
`FinishRun` — and it **NEVER routes through `TryFireOnce`**, which now skips Global-scoped ids. That
removes the `"=ActorName"` misparse structurally: the token is never built for it.

🚨 **A frame is labelled positive ONLY when geometry is within the anomalous near-clip radius**, by a
per-frame sphere overlap at the camera (bounds only, no pixel read). The near plane being wrong is
not the same as the viewer seeing anything wrong, and labelling a whole session positive would ship
thousands of frames showing nothing — which **the m26 mask veto cannot catch, because there is no
target and therefore no mask.**

**`P6` does not move.** The existing event shape carries it: whole-frame as `coverage_ratio = 1` and
per-frame `bbox_norm = 0,0,1,1`, empty `asset_name`, and `coverage_pct` left at its `-1` sentinel
(it comes from selection provenance, and a global anomaly has no selected actor).

### The auto-pool checkbox set is a THREE-STAGE derivation (m29 recon)

Reading it wrong is how an id ends up rendering a **live-looking but inert** checkbox:

1. **Engine, snapshot** — `ControlSnapshot.cpp` keys `auto.pool` on **every OBJECT-SCOPED CATALOG
   ENTRY**, i.e. on `GetAnomalyCatalog()` filtered by `E.Scope == EAnomalyScope::Object`. It is
   **NOT** keyed on `GAutoPool`.
2. **Client, denylist** — `store.ts` `HIDDEN_ANOMALY_IDS` removes ids from **both** the pool object
   **and the catalog** (`setCatalog` filters through the same set), so a denied id disappears from
   the Targeted dropdown too.
3. **Client, render** — `AutoPanel.tsx` renders what survives; the checked state is
   `auto.pool[id]` ← `IsAnomalyEnabled`.

🚨 **Catalog membership makes a checkbox APPEAR; `GAutoPool` membership makes it WORK.**
`SetAnomalyEnabled` rejects any id not in `GAutoPool`, logging a warning the dashboard user never
sees. Adding an object-scoped id to the catalog **without** adding it to `GAutoPool` therefore ships
a checkbox that renders, toggles in the UI, and silently does nothing.

### `missing_texture` save-state + revert contract (m17)
The only anomaly whose revert does **not** follow the plain "restore captured value per live comp; skip stale"
convention — because it is the only one whose captured value is an **object pointer the game may own and re-create**
(runtime MIDs on modular/merged characters), rather than a plain value (intensity, LOD index, near-clip) that stays
valid. See gotchas G74–G76.
- **Apply captures, per slot:** the component weak ptr **+ its owning actor + its `FName`** (identity that survives
  re-creation), the slot index, the original material weak ptr, and `bWasExplicitOverride`. The anomaly also stores
  the checker it actually applied (`AppliedChecker`), because `IAnomaly::Revert()` has no world access and the guard
  needs to identify our own material. Apply's visible behavior is unchanged from m8.
- **Revert, per captured slot:** (1) resolve the component — saved ptr if alive, else **re-find a live same-named
  component on the owning actor**; (2) **guard** — act only if the slot's CURRENT material still is our checker, or a
  material instance whose parent chain reaches it; if the game re-took the slot, leave it; (3) restore the saved
  original if it is still alive and was an explicit override, **else `SetMaterial(i, nullptr)`** so the mesh's
  built-in default renders and the owning system re-takes the slot on its next re-assertion.
- **Then a sweep:** every live mesh component of each touched actor is checked for slots still holding our checker
  (corruption that rode `OverrideMaterials` onto a **successor** component that was never captured) and reset to the
  mesh default. The guard makes this idempotent.
- **Every revert logs** `restored / default-reset / left-to-game / unresolved / swept / re-found`, warning per
  unresolved or swept slot. A revert can no longer fail silently. If `AppliedChecker` is unresolvable, revert
  refuses to touch anything (logs Error) rather than risk stomping game materials.
- **Guarantee boundary:** components **on the matched actor**. Sub-parts owned by a *different* actor are not
  reached; a swept successor can only be reset to the mesh default (no saved original exists for it).

## Viewport-visibility scoping (opt-in; default OFF)
The subsystem holds one flag `bViewportScopingEnabled` (default **OFF**), toggled by `IAI.SetViewportScoping <0|1>`
and read by anomalies via the static `UAnomalyInjectorSubsystem::IsViewportScopingEnabled(World)`. **Only the four
object-scoped, primitive-backed anomalies consult it:** `missing_object`, `blinking` (actor granularity — visible iff
**any** primitive component is visible) and `lod_corruption`, `lod_popping` (component granularity). When **ON**, each
routes target resolution through `AnomalyViewport` so it affects only objects visible in the player's view; when
**OFF**, each takes its original resolution path **byte-identical to before** (the regression gate). Excluded by
design: `lighting_mismatch` (a `ULightComponent` is not a primitive — that's the future region-darkening anomaly's
concern) and the two globals `time_dilation` / `camera_clipping` (whole-frame). **v1 semantics:** a matched object is
affected iff visible **at Apply time** (ticking anomalies fix their visible set at Apply; the tick does not re-test).
If scoping is ON but no live view resolves, the anomaly treats-as-unscoped (full matched set + one warning), so ON
never silently drops every target. The catalog rows above are unchanged in effect; scoping only narrows *which*
matched targets are acted on.

## Object Selector + Inject UI — `UAnomalySelectorSubsystem` (m5)
A **separate** `UTickableWorldSubsystem` (Game + PIE only, gotcha G7) — not the injector — that turns the m4 visible
set into an interactive **select-an-object-and-inject** loop. It owns selection state + input polling + an immediate-mode
HUD, and calls the injector's existing public `ApplyAnomaly` / `RevertAnomaly`. **The injector subsystem, `IAnomaly`,
and all seven anomalies are untouched** (the no-core-change streak holds; this is UI over the existing catalog).

- **Explicit-core / thin-shell split (mirrors m4).** The state-gatable surface is a set of public methods:
  `AdvanceSelection()` / `SelectPrevious()` / `CycleAnomalyChoice()` / `InjectSelected()` / `RevertSelected()` plus
  readbacks `GetSelectedActorName()` / `GetVisibleActorNames()` / `GetAnomalyChoice()`. **Two thin shells drive that
  core:** (1) the `IAI.Selector.*` console commands (the MCP-bridge gate), and (2) per-tick raw-key input polling + the
  HUD draw (the owner's real-Play eyeball). Only the methods/commands are bridge-driveable.
- **Candidate set = the renderable-visible set** (m5 follow-on). Each refresh calls
  `AnomalyViewport::GetVisibleRenderableActors(World)`, which resolves the live view and returns actors that are
  in-frustum, unoccluded, **and** carry a *renderable* component (see "Renderable-visible set" under the
  `AnomalyViewport` section). This excludes non-rendering actors that a pure frustum+occlusion test would pass —
  volumes (e.g. `RuntimeVirtualTextureVolume`), spawn points (`PlayerStart`), debug/replication/streaming actors,
  landscape — so they are never injectable targets. Selection is tracked by `TWeakObjectPtr` identity; if the selected
  actor leaves the set (or is destroyed) the selection clears. *No-view rule:* the selector offers **nothing** when no
  view resolves (never select/inject blind) — deliberately distinct from the anomaly finders' treat-as-unscoped (G23/G29).
- **Cycle order = name-sorted (alphabetical) in v1** — deterministic, so the bridge gate can assert the exact cycle
  sequence. **Screen-X (left-to-right) ordering is the intended next UX polish** (it directly serves the "intuitive"
  goal); deferred only to keep v1 minimal + testable.
- **Inject path.** `InjectSelected()` calls `ApplyAnomaly(<chosen id>, { "=" + Actor->GetName() })` — the `=`
  exact-match sentinel targets **only** the selected actor (never a same-prefixed sibling). Default args only. The four
  offered ids are `missing_object`, `blinking`, `lod_corruption`, `lod_popping`; globals (`time_dilation`,
  `camera_clipping`) and `lighting_mismatch` stay console-only for v1. The selector **does not** touch
  `IAI.SetViewportScoping` — it is self-scoping (you pick from the visible set), so injecting an exact-named,
  already-confirmed-visible actor needs no further viewport re-filter. **Keep `IAI.SetViewportScoping 0` while using the
  selector:** with scoping ON the injector re-tests visibility on apply, which can *drop* the target if it became
  occluded in the sub-second between select and inject — redundant and surprising for this path.
- **Revert path.** `RevertSelected()` reverts the **last id this selector injected** (`LastInjectedId`) via the
  injector. Because the registry holds **one instance per id** (last-writer-wins per id), injecting the same anomaly id
  on a second object reverts-then-reapplies — i.e. **only one object can carry a given anomaly type at a time**; the
  first reappears (existing registry reality, G12-style — stated so it's not a surprise).
- **HUD (immediate-mode, host-blind).** Registered via `UDebugDrawService::Register("Game", …)` (drawn by
  `GameViewportClient::Draw` with no host HUD class — gotcha G25); unregistered on **both** disable and teardown, guarded
  against double-register. Draws: a list of visible actor names (selected one marked, capped with a "+N more"),
  a list of the four anomaly ids (chosen one marked), a **last inject/revert result line** (`LastInjectResult`) that
  surfaces the AMB-2 zero-match case in real Play ("0 matched") instead of it being log-only — e.g. an LOD anomaly that
  resolves no mesh component on the selected actor. (Since VFX left the set (G33), a pure-VFX actor is no longer
  selectable; the zero-match-on-VFX case is now reached only via the `=name` console escape hatch.) Also: an on-screen
  name label anchored to the selected actor (`Canvas->Project`), and a world-space
  `DrawDebugBox` around the selected actor's bounds (dev-only, `ENABLE_DRAW_DEBUG`).
- **Input (raw, mapping-independent).** Per-tick poll of the local PC (`World->GetFirstPlayerController()`) via
  `WasInputKeyJustPressed` / `IsInputKeyDown` (raw `KeyStateMap`, no project mapping needed — gotcha G26). Default
  keybinds **Tab** (next) / **Shift+Tab** (prev) / **C** (cycle) / **G** (inject) / **H** (revert), all rebindable via
  `IAI.SelectorBind` (the default prev gesture is Shift+Tab; a dedicated prev key can be bound). *Steam-overlay caveat:
  Shift+Tab is grabbed by the Steam overlay in a Steam-launched build — fine in PIE; rebind escapes it (G26).*
- **Activation = `IAI.SelectorUI <0|1>`, default OFF.** When OFF the subsystem is **dormant** (Tick early-returns, no
  HUD delegate registered) → every existing M0–m4 gate is byte-identical (the regression gate).
- **Refresh cadence.** Only while enabled: throttled (~0.1 s) for the HUD list, plus an on-demand refresh at the start
  of `AdvanceSelection` / `SelectPrevious`. Bounds the synchronous occlusion-trace cost (occlusion runs only for
  in-frustum actors).

## Automatic Injection — `UAnomalyAutoInjectorSubsystem` (m6)
A **third, separate** `UTickableWorldSubsystem` (Game + PIE only) — not the injector, not the selector — that
auto-fires the four object-scoped anomalies on the renderable objects currently on-screen and auto-reverts them.
It calls only the injector's public `ApplyAnomaly` / `RevertAnomaly`; **the injector, `IAnomaly`, the anomalies,
and the leaf helpers are untouched** (auto-injection is orchestration over the existing catalog). v1 pool = the four
object-scoped ids (`missing_object`, `blinking`, `lod_corruption`, `lod_popping`); globals + `lighting_mismatch` are a
future non-object track.

- **Concurrent but collision-free BY CONSTRUCTION (G30), via two scheduler invariants** — no ref-count coordinator:
  - **(i) one live fire per id.** The injector registry holds one instance per id (re-Apply reverts-then-reapplies),
    so the scheduler never re-fires a still-live id. Clean revert accounting + the natural concurrency ceiling
    (max live ≤ distinct enabled-id count).
  - **(ii) one anomaly per actor.** Targets are drawn from `V − {actors hosting ANY live fire}`. This one invariant
    subsumes **both** conflict groups (bHidden: `missing_object`/`blinking`; forced-LOD: `lod_corruption`/`lod_popping`)
    **and** the hide-masks-LOD case (a hide hiding a LOD change = an invisible/mislabeled sample). So there is no
    id→group table. (The deferred ref-count coordinator from G12 is only needed for *deliberate* compound/stacked
    same-actor anomalies.)
- **Explicit, deterministic core / thin shells (mirrors m4/m5).** The core is `AdvanceTime(DeltaSeconds)` (service
  auto-reverts one pass, then at most one timed fire window) and `TryFireOnce()` (force one attempt). It is a pure
  function of (seeded stream, enable-set, cadence, the renderable-visible set) and is **driveable over the bridge
  without real time** — `IAI.Auto.Step` → `AdvanceTime`, `IAI.Auto.FireOnce` → `TryFireOnce`. Two thin shells drive it:
  the `IAI.Auto.*` console commands (bridge gate) and per-tick raw-key polling + an immediate-mode `UDebugDrawService`
  HUD (real-Play eyeball; anchored to the right so it does not overlap the selector's top-left HUD).
- **Three separated states.** `Enable` (`IAI.Auto.Enable`) = the eyeball shell only (registers the HUD, polls keys);
  `Run` (`IAI.Auto.Run`) = the auto-tick auto-feed (Tick → `AdvanceTime(DeltaTime)`), forced OFF when !Enabled;
  `Step`/`FireOnce` = direct manual core drive, working regardless of Enable/Run (given a configured enable-set + seed).
  **Both switches default OFF → the subsystem is dormant (Tick early-returns, no HUD delegate) → every existing M0–m5
  gate is byte-identical** (the regression guarantee).
- **Self-scoping targeting (R-CAD).** Each fire attempt draws candidates from
  `AnomalyViewport::GetVisibleRenderableActors(World)` directly and applies via the `=` exact-match token
  (`"=" + Actor->GetName()`) so it hits only that actor. It does **not** use `IAI.SetViewportScoping` (keeping it ON
  would make the `=` apply redundantly re-test visibility and could drop a target — m5 fact #3; a warning fires at
  Run-start if scoping is ON). **No view → fire nothing this window** (the empty-on-no-view contract, R6/G29 — never
  inject blind). A drawn id that resolves no matching component legitimately yields a zero-match: it is surfaced
  ("0 matched") and not registered, never a silent slot leak. (Since VFX left the set (G33), the auto pool no longer
  targets pure-VFX actors, so the original LOD-on-VFX zero-match example no longer arises from the auto path.)
- **Lifecycle (R-LIFE).** Each fire records `{id, target weak-ptr, name, secondsRemaining}`; on its hold elapsing the
  scheduler calls `RevertAnomaly(id)` and frees the slot. `IAI.Auto.Persist 1` (default off) suppresses auto-revert
  (fires persist until run-stop / manual revert / teardown). On Run-stop and disable it reverts its own live fires; on
  `Deinitialize` it only unregisters the HUD + clears (no inject calls — the injector restores everything on its own
  teardown, since subsystem teardown order is unspecified).
- **Determinism (R-SEED).** All randomness is one `FRandomStream`, seeded once per run (console-settable; default
  time-based via `FPlatformTime::Cycles`). The draw protocol is **fixed and independent of `ApplyAnomaly`'s result**:
  skip-paths (cap, empty eligible, empty view) consume zero draws; a Candidates-empty skip consumes exactly the Id
  draw; a real attempt draws Id, Target, Hold (in that order) then registers on success only; an interval draw arms
  each fire window. The seed reproduces the *choices* given the same sequence of visible sets (and Step granularity) —
  full run reproducibility with fixed visible sets is a capture/replay concern, not v1.
- **Coexistence (R-COEXIST).** Manual selector/console injection of a pool id during an auto run is **unsupported** (it
  clobbers via the registry's one-instance-per-id; the auto-injector tracks only its own fires) — detected cases
  (selector UI on; viewport scoping on) are **warned, not blocked**.
- **Defaults.** Keys `1`/`2`/`3`/`4` toggle the four types, `J` start/stop, `K` reseed (distinct from the selector's
  Tab/C/G/H, rebindable via `IAI.Auto.Bind`); interval [4,9]s, hold [3,6]s, MaxConcurrent 4 (tuned for clear
  eyeballing — tighten later for dataset density). All console-settable.

## Capture & Labeling — `UAnomalyCaptureSubsystem` (m7; relocated to the `AnomalyCapture` module — stencil-capture S1)
Produces an ML-friendly **labeled image sequence** from a LIVE auto-injection run (L1: labels are the injector's own
ground truth, not a replay diff).

> **AS-BUILT UPDATE (`feature/stencil-capture` Stage 1, 2026-06-30 — see `docs/sessions/2026-06-30-015-stencil-capture-stage1.md`).**
> Capture was **extracted from `AnomalyControlServer` into a new quarantined module `AnomalyCapture`** (gated `ANOMALY_CAPTURE`,
> own log cat `LogAnomalyCapture`, render/Slate + `bBuildEditor`-only `UnrealEd` deps compiled out of Shipping; `ControlServer`
> now depends on it). The default grab is now **async + UI-inclusive**: `FAnomalyFrameCapturer` reads the post-Slate composited
> **backbuffer** (`OnBackBufferReadyToPresent`, the REAL player frame with game UI) clipped to the game-viewport rect, via a
> staged `FRHIGPUTextureReadback`, with convert+encode+write on a **thread pool** (`FAnomalyAsyncWriter`) — non-blocking, no
> game-thread stall. Only OUR dev overlays are suppressed during a run (`AnomalyViewport::SetOverlaysSuppressed` + heartbeat
> eviction + PIE mouse-label disable at `Initialize`). `IAI.Capture.Async <0|1>` toggles back to the legacy **synchronous
> `ReadPixels`** path described below (now the fallback). The 2D projected box below is UNCHANGED this stage; the occlusion-
> correct stencil box is Stage 3 (color and stencil become two grab points joined by submit frame id). Gotchas G52–G57.
> The rest of this section documents the m7 sync/labeling internals, which the fallback path still uses.

The m7 housing (now the fallback path): reuses the game-viewport capture primitive +
ImageWrapper + JSON; gated by `ANOMALY_CAPTURE`, so compiled out of Shipping — dataset capture is a dev/research
activity in a packaged Development/Test build, never a retail Shipping build, satisfying L5.

- **Three sanctioned core exposures (the ONLY `AnomalyInjector` touches).** `IAnomaly`, the injector core, the seven
  anomalies, the leaf helpers, the `=` exact-match, and `GetVisibleRenderableActors` are **byte-clean**:
  1. `AnomalyViewport::ProjectActorBoundsToScreenRect(View, Actor, OutMin, OutMax)` — the L2 2D-bbox projection, built on
     the SAME private reversed-Z VP path the frustum / `GetVisibleRenderableActorInfos` pass uses. It unions the actor's
     static/skeletal-mesh component bounds **by TYPE only — NOT `IsVisible()`-gated** — so a hidden `missing_object` /
     `blinking` actor still projects ("where the hole is"); returns the **unclamped** normalized rect; false only
     behind-camera / fully off-screen (gotcha G38).
  2. `FAutoLiveFireInfo` widened with `TWeakObjectPtr<AActor> TargetActor` + `uint64 StartFrame` (the fired actor for
     bounds projection + the fire's start `GFrameCounter`).
  3. `UAnomalyAutoInjectorSubsystem::RevertAllLiveFires()` — exposed (was private): reverts each live fire via the injector
     **and** clears the tracking list, so `GetLiveFires()` stays accurate. Capture drives burst reverts through this — NOT
     the injector's `RevertAll`, which would leave the list stale (post-roll would mislabel positive).
- **Burst state machine (deterministic, capture-driven).** Drives the m6 core directly (`TryFireOnce` / `RevertAllLiveFires`,
  never `AdvanceTime` — that could fire a second interval-anomaly): `[pre M negatives] → FireOnce → [settle K skipped] →
  [positives P] → RevertAllLiveFires → [settle K skipped] → [post M negatives]`, looped (post-roll doubles as the next
  burst's pre-roll); `BurstCount 0` = until Stop. The **settle-K is SYMMETRIC at BOTH boundaries** because a game-thread
  mutation reaches the rendered frame >=1 frame later (`r.OneFrameThreadLag`); skipping K frames after both the fire and
  the revert keeps boundary frames from mislabeling (gotcha G37, default K=2). One frame per tick; each captured frame is
  stamped with `GFrameCounter`.
- **Per-frame labeling (same-tick, exact alignment — L3/Q6).** In one game-thread call: snapshot `Auto->GetLiveFires()` +
  synchronous `FViewport::ReadPixels` (native resolution, opaque-alpha — gotcha G39) + project each fired actor's bounds →
  write `frame_<GFrameCounter>.png` + append one JSONL record. The label falls out of the live-fire set: `anomaly_present`
  = game-state truth (any live fire); per fire a pixel bbox `[x,y,w,h]` (clamped) + the unclamped normalized rect +
  `bbox_valid`. **`visible_positive` = `anomaly_present && (≥1 bbox_valid)`** is the detection-relevant positive — under
  camera motion a fired actor can leave the viewport mid-hold (`present=true` + all `bbox_valid=false`); those frames are
  KEPT as hard negatives, not dropped (gotcha G42). The in-frustum-but-occluded sub-case is the deferred
  `GetLastRenderTimeOnScreen` refinement (G22).
- **View-lag L (default 0) — the spatial analogue of settle-K, but distinct.** A per-tick view ring; each capture projects
  with the view from L ring-entries ago. **L=0 is validated and correct (not "zero lag") FOR THE SYNC PATH:** the capture
  subsystem (a `FTickableGameObject`) ticks *before* `UpdateCameraManager` (LevelTick.cpp:1606 vs 1621), so
  `GetActiveViewInfo` at the capture tick already returns the previous frame's camera POV — exactly the view that rendered
  the `ReadPixels` frame; the two 1-frame lags cancel. FPS-invariant (frame-count relationship). The `IAI.Capture.ViewLag`
  knob stays for the future async path (gotcha G41). **OPEN (m18):** that async re-derivation was never done — the async
  grab returns the ARM TICK's own render (camera N), while the ring still yields camera N-1, so the projected bbox is
  predicted to be one frame stale under camera motion on the async path. Unmeasured (the validation scene has a static
  camera); the m18 label fix below deliberately did NOT touch L. See G78.
- **Live preview — a backbuffer TEE, not a viewport read (m19).** The dashboard preview no longer uses
  `FViewport::ReadPixels`: a packaged game viewport has no render target, so that read zero-filled and reported success
  → a **black preview in ANY packaged build** (it was never editor-gated; it ran and sent black JPEGs — G79). The preview
  now tees off the same `OnBackBufferReadyToPresent` stream the capture path uses, via its **own**
  `FAnomalyFrameCapturer` instance (`FAnomalyPreviewTee`, `Private/AnomalyPreviewTee.{h,cpp}`) — it cannot share
  capture's grab, because m16 suppression makes the two mutually exclusive in time and the capturer's arm/queue are
  single-consumer. `UAnomalyCaptureSubsystem` owns the tee and exposes `PreviewPump()`/`PreviewArm(epoch)`/
  `PreviewPoll(...)`; the control server drives them from its Tick, so all render/RHI stays quarantined in
  `AnomalyCapture` and the control server gains **no render deps**. Arm cadence = the existing `subscribe` frameHz
  (~6 Hz); the hook early-outs with no arm pending. **m16 suppression gates the ARM (not just the send)**, while the
  pump still drains-and-discards so an in-flight readback cannot leak. Encode (`ConvertTightToBGRA` + `EncodePixels`)
  runs on a background task; the WS send stays on the game thread. `ViewEpoch` is stamped at arm. The AIF1 wire format
  is unchanged (no dashboard change). The preview now shows what the player sees (game UI included), matching captures.
  Honest scope: **no capture speedup** (m16 already suppressed preview during captures); the win is a working packaged
  preview + no ~6 Hz game-thread flush outside capture. G79.
- **Label state stamp — END OF TICK on the async path (m18).** What a frame's label *says* is sampled from
  `Auto->GetLiveFires()`; what the frame *shows* is the state at the end of its own tick, because the async capture grabs
  the render of the tick that armed it. `BeginFire()`/`BeginRevert()` run later in that same `Tick` than
  `CaptureCurrentFrame()`, so sampling the fire state at arm time described the PRE-transition world for a frame that
  renders the POST-transition world — the label span sat one frame LATE at both burst boundaries. The async path therefore
  arms the frame and stores the snapshot *without* the fire state, and `FinalizeArmedLabel()` (last statement of `Tick`,
  after the phase switch) fills in `Fires`/`FireHidden`/`FirePos` post-transition. Because `anomaly_present`, the per-anomaly
  bbox/`bbox_valid`, `AffectedFrames` and `HiddenIndices` all derive from that one sample, hide-type and non-hide shift
  coherently and `visible_positive` stays consistent by construction. **The sync path is untouched and was already correct**
  (its `ReadPixels` returns the *previous* frame, which pairs with an arm-time stamp) — the fix is async-only. Phase timing
  and settle-K are byte-unchanged. G78.
- **Hidden-state stamp — ONE TICK LATER than the fire stamp (m20).** End-of-our-Tick is the right sample point only for
  state OUR Tick changes (`Apply`/`Revert` via `BeginFire`/`BeginRevert` — e.g. `missing_object`). An anomaly that toggles
  in its OWN tick (`blinking`, driven by `UAnomalyInjectorSubsystem::Tick`, a *different* tickable that runs AFTER the
  capture subsystem) is still holding the previous frame's value at that point, while the frame renders with the new one —
  measured as `annotation(G) == pixels(G-1)` on every blink edge. So `FinalizeArmedLabel` stamps only `Fires`/`FirePos`,
  and **`SampleDeferredHidden()` fills `FireHidden` at the TOP of the next Tick** — the first instant the world equals what
  the previous frame rendered (all subsystems have ticked). It runs before `ProcessCompletedFrames` (the earliest a frame
  armed at N can drain) and at the top of `FinishRun` (before `RevertAllLiveFires`) for a `StopRun` between ticks. **Safe
  by construction: `FireHidden` never reaches labels.jsonl** — it feeds only annotation's hidden set + transition count —
  so the m18-validated per-frame span is byte-unchanged. G81.
- **annotation.json event stats are ORDER-INDEPENDENT (m20).** Frames arrive out of session order (`Drain_RenderThread`
  fills `Completed` in reverse, `PopCompleted` is FIFO), so no order-sensitive statistic may be accumulated on arrival.
  `FSessionEventAccum` stores `TMap<int32,uint8> HiddenByIndex`, and `WriteSessionAnnotationFile` derives **both** the
  hide-type `frame_indices` and the `Transitions` count from the **sorted** keys at write time — one source, no drift, no
  spurious transitions. Subtype derivation stays data-driven: `blinking` → `≤2 transitions = disappear_reappear`,
  `≥3 = flicker`; every other id mirrors its type (`missing_texture` → `missing_texture`). G81.
- **Frame indices are 0-BASED** and match `frame_%05d.png` exactly (`host-tools/encode_watcher.py` encodes the PNGs 1:1,
  also 0-based). A 1-based video player therefore displays index N as "frame N+1" — that is a reading convention, not an
  off-by-one. G81.
- **Reproducibility (S4).** The run seeds the auto-injector stream at start → same seed + same visible-set sequence (a fixed
  vantage) reproduces the fired (id, target) sequence — NOT pixel-identity (ambient scene motion). **Coexistence:** the
  auto-injector's `Run` must be OFF during a capture run (capture owns firing) — warned, not blocked (A2); and the
  poll-radius dev debug sphere is suppressed for the run so it is not baked into captured frames (`SetDebugSphereSuppressed`;
  visual-only — the poll-radius cull keeps shrinking the set, G44). A zero-match burst (empty eligible/visible) records
  negatives only and advances (A6).
- **Output.** `run_<seed>_<timestamp>/` with `frame_<GFrameCounter>.png` + `labels.jsonl` (one record/line) + `run.json`
  (manifest at start: seed, K, L, pre/positive/post, burstCount, viewport, format, schema version, start frame/time) +
  `run_summary.json` (at stop). `tools/verify_capture.py` overlays the boxes onto frames + prints a per-frame table +
  present/visible-positive/off-screen tallies (Pillow). **Capture primitive:** `AnomalyPreview::CaptureGameViewportEncoded`
  (PNG/JPEG, opaque, native res) — shared infra physically committed with the control-server Slice-1 (`ff1be3c`).
- **Deferred (the async exact path).** The synchronous `ReadPixels` flush is an observer-effect stall; the render-thread
  async path (`OnBackBufferReadyToPresent` + `FRHIGPUTextureReadback`) is the documented superseder — REQUIRED before
  framerate-bug anomalies enter the pool (the flush would corrupt the framerate label) and for exact-under-motion
  view-matching (gotcha G40; re-derive L there).

## Grab points — SVE / B′ (DEFAULT since S4) and backbuffer (the UI-on option)

**As-built:** the capture subsystem can obtain a frame from **two** grab points, selected by
`IAI.Capture.SVE <0|1>` (**default 1 since S4**, mid-run guarded; overridable from `DefaultGame.ini`
`[AnomalyCapture] bSveCaptureDefault`, though the shipped default deliberately needs **no ini key** —
which is what makes it immune to **G88**).

**The backbuffer path is unchanged and is KEPT as the UI-on option. `IAI.Capture.SVE 0` is its NAMED
BISECT SWITCH** — the one setting that reaches it with no rebuild and no re-cook, in the same polarity
S3 certified, which is why the switch was deliberately **not renamed**. The startup banner reports the
grab point **and where its default came from** (compiled-in vs ini).

| | **backbuffer** (`IAI.Capture.SVE 0` — the UI-on option) | **SVE / B′** (DEFAULT) |
|---|---|---|
| hook | `FSlateRenderer::OnBackBufferReadyToPresent` | `FSceneViewExtensionBase`, after `EPostProcessingPass::VisualizeDepthOfField` |
| content | the presented frame, **game UI included** | scene colour post-tonemap, **pre-Slate ⇒ UI-free by construction** |
| rect | the Slate **window** rect | the **view** rect |
| frame↔state pairing | **arm → next present** (order) | **identity** via the key ring (see below) |
| implementation | `FAnomalyFrameCapturer` | `FAnomalySceneViewExtension` + `FAnomalySveCapturer` |

**The key ring (`AnomalySveKeyRing`) is the whole of B′.** The game thread publishes
`(FSceneViewFamily::FrameNumber → GFrameCounter, wanted)` at **`BeginRenderViewFamily`** — the only hook
where `ViewFamily.FrameNumber` is assigned (`SetupViewFamily` still reports `UINT_MAX`). The
render-thread pass looks the key up by `View.Family->FrameNumber` and recovers the `GFrameCounter`.
Fixed capacity 64, oldest-evicted, four counters plus a corruption counter.

**The seam is `FAnomalyCapturedFrame::RequestId`.** That id is already the key
`Async->PendingSnapshots` uses, so the SVE path swaps the *producer* of the id and touches **no
consumer** — the label record, the event accumulator, the async writer, `labels.jsonl` and
`annotation.json` are all identical between grab points by construction.

**A lookup miss is loud and lossy, never a guess:** the miss is counted, a warning carrying the ring
counters is logged, and **the frame is dropped**. No frame is ever labelled from an unrecovered key.
`IAI.Capture.SVE.ForceMiss <N>` (0 off / 1 every key / N>1 every Nth) and
`IAI.Capture.SVE.ForceMissPhase <P>` exist solely to make that guard testable;
`IAI.Capture.SVE.RingTest [n]` exercises the ring headlessly.

**Telemetry:** `run_summary.json` carries **`capture_path`** — `"sve"` or `"backbuffer"` — on **BOTH**
paths since **S4-3**, so a delivered session states what produced it. An absent field used to be
indistinguishable from a pre-S3 build. `key_ring_{published,consumed,missed,wrapped,corrupted}` remain
**SVE-only**; they have no meaning on the backbuffer path.

⚠ **`key_ring_published` / `consumed` / `wrapped` are RUN-UNIQUE, not invariant** — measured on a
same-binary, same-config control pair, they vary with how many view families render before capture
starts. Only `missed` and `corrupted` are invariant. *(m24 reported all five as identical across its
pairs; that was true of those pairs and does not generalise. Corrected here, m24's verdicts undisturbed
— they rest on `missed == corrupted == 0`, not on the publish count.)*

⛔ **C1's leak-check invariant — "a switch-OFF `run_summary` is byte-identical to the pre-S3 shape" — is
FORMALLY RETIRED as of S4-3.** It existed to prove S3a was inert when off; once SVE is the default it
has no remaining job. **This is a RULING, not a side effect of the change.**

**Build cost:** `AnomalyCapture` gained `Renderer` **and a Renderer PRIVATE include path**, non-Shipping
only, because the post-process hook takes `FPostProcessMaterialInputs` (**G100** — an engine bump breaks
this far from its cause, and the `class FViewInfo;` forward declaration in
`AnomalySceneViewExtension.cpp` must not be tidied away).

### The resolution matrix (S4-1, measured) — RECT EQUIVALENCE

Ten packaged legs, `CB_GateLevel`, `VideoFps` 30 pinned. Four rect sources compared per leg: the
delivered **PNG** (read from its IHDR chunk — ground truth), `labels.jsonl` width/height,
`annotation.video.resolution` and `run.json` viewport. **All four agree on every leg.**

| leg | config | rect | dW/dH | B1 | A54 |
|---|---|---|---|---|---|
| M0s / M0b | 1280×720 windowed, SVE / backbuffer | 1280×720 | 0/0 | yes / n/a | ALL-ALIGNED 7/7 |
| M1a | 1280×720, desktop **150 %**, engine default | 1280×720 | 0/0 | yes | ALL-ALIGNED 7/7 |
| M1b | 1280×720, **150 %**, process forced **DPI-AWARE** | 1280×720 | 0/0 | yes | ALL-ALIGNED 7/7 |
| M1c | **1001×721**, 150 %, DPI-aware (non-multiple) | 1001×721 | 0/0 | blocked | unjudgeable |
| M2 | 1280×1024 (5:4) | 1280×1024 | 0/0 | blocked | unjudgeable |
| M3 | 1920×1080 **fullscreen** | 1920×1080 | 0/0 | blocked | unjudgeable |
| M4a / M4b | `r.ScreenPercentage` **50** / **170** @1280×720 | 1280×720 | 0/0 | yes | ALL-ALIGNED 7/7 |
| M5 | **1281×721** (odd) | 1281×721 | 0/0 | yes | ALL-ALIGNED 7/7 |

**The SVE grab is at OUTPUT resolution in every case**, including both screen-percentage directions.
42 counted events across the six judgeable legs, **42 ALIGNED, 0 SHIFTED, 0 ABSENT, 42/42 decidable**,
in-leg positive control decisive in both directions on every one.

⚠ **THE THREE CAVEATS ON THE SCREEN-PERCENTAGE PRIOR, which travel with it.** Journal 028
(`docs/sessions/2026-07-29-028-…`, the journal that MEASURED it — not the handoff that summarises it)
found the SVE frame stayed at output resolution at SP 170/320. That figure was measured **(a)** with the
**CaptureBench A/B probe plugin**, not production `FAnomalySceneViewExtension`; **(b)** at **1920×1080**,
not 1280×720; and **(c)** **above SP 100 only**. M4b reproduces it on the production path at 1280×720;
**M4a extends it into the upsample direction 028 never tested.**

⚠ **NOT MEASURED, and M2 is explicitly NOT a substitute: a TRUE camera-constrained letterbox**
(`bConstrainAspectRatio` on the camera). It needs a content change to `CB_GateLevel`, therefore
`make_gate_level.py` (destructive by default — **G99**) and a re-cook (**G92**). The gap is stated, not
papered over.

### Capture OUTPUT RESOLUTION — a downscale ON WRITE (`m28`)

**The render is ALWAYS native. Only the WRITTEN frame is resampled.** This shape was chosen because
the `m26`/`m27` stencil mask counts at the view's render resolution — `SceneColor.ViewRect` at the
Tonemap pass (`AnomalyMaskSceneViewExtension.cpp`), reduced over `Item.ViewRectSize` — and **never
sees the capture output buffer**, so a write-time downscale **structurally cannot reach the veto**.
Selection, labelling geometry and the mask are untouched: `AnomalyViewport.*`, `AnomalyMaskMeasure.*`
and `AnomalyMaskSceneViewExtension.*` gained **not one line**.

| | |
|---|---|
| **the knob** | **ONE: target output HEIGHT.** `0` = NATIVE (no resample; the written bytes are identical to a pre-`m28` build). |
| **width** | ⛔ **THERE IS NO WIDTH PARAMETER.** It is derived from each frame's own aspect. A non-aspect-preserving output is **UNREPRESENTABLE, not guarded against** — the projection matrix takes its aspect from `GetViewportSize()` (`AnomalyViewport.cpp`), so a stretched output would silently invalidate every `bbox` and coverage figure. |
| **derivation** | `AnomalyLabel::DeriveOutputSize` — the ONE place `D2`'s rules exist. `H` snapped to the nearest EVEN, min 2; `W = SnapEven(round(H·Wsrc/Hsrc))`, min 2, never exceeding `Wsrc`. **NEVER UPSCALES:** a request at or above the frame's own height yields native and no resample runs. |
| **when** | **PER FRAME, from the frame in hand** — never from `GetViewportSize()`. |
| **the filter** | **AREA / BOX**, fractional edge weights, in `AnomalyLabel::ResampleAndEncodeBGRA`. **Not point, not bilinear** — point sampling destroys thin features, and thin features are the evidence class this project protects. |
| **the ONE site** | `ResampleAndEncodeBGRA` is the **sole** resample in the codebase. Two invocation points, both "the last moment BGRA exists before encode": the async writer path and `CaptureLabeledShot` (which serves both the sync fallback and `IAI.Capture.Shot`). |
| ⚠ **why not `EncodePixels`** | It is the nearest common function of all three write paths **and it is CONTAMINATED**: `AnomalyPreviewTee.cpp` calls it too. Resampling there would silently downscale the live preview, which is the coordinate frame the dashboard's click-to-select maps against. **The preview is untouched BY CONSTRUCTION.** |

**ONE derived pair reaches every consumer, and each one is a single argument change:**
the resampler · `labels.jsonl` `width`/`height` · **every `bbox_px`** (it is already computed from the
same `W,H` at `AnomalyLabelWriter.cpp`, so it follows for free — **no new scaling code was written**,
which is the entire reason the pair is threaded rather than re-derived) · `annotation.video.resolution`.

🎯 **`annotation.video.resolution` NOW COMES FROM THE FIRST WRITTEN FRAME'S REAL DIMENSIONS**, reported
back by the writer, not from `GetViewportSize()` at `StartRun`. A later frame of differing size emits a
loud WARNING naming both pairs. **A session that writes no frame reports `[0,0]` plus a warning** — it
deliberately does NOT fall back to the viewport, which is the quantity that was wrong. **`run.json`'s
`viewport` is UNCHANGED and still reports `GetViewportSize()`** — the two fields now answer different
questions on purpose. **No new fields and no new counters were added to any delivered artifact.**

#### 🚨 THE TWO RESOLUTION FIELDS LEGITIMATELY DISAGREE — THAT IS THE FEATURE, NOT A BUG

**A cold reader who sees these two numbers differ in one session folder must not file it.** Since
`m28` they answer DIFFERENT QUESTIONS and are SUPPOSED to diverge whenever a downscale is requested:

| field | answers | source |
|---|---|---|
| `annotation.json` → `video.resolution` | **"how big are the PIXELS I was actually given?"** | the FIRST WRITTEN FRAME, measured |
| `run.json` → `viewport` | **"how big was the game's viewport?"** | `GetViewportSize()` at `StartRun` |

**MEASURED, `m28` `GATE C`, the two legs that make the point:**

| leg | frame_00000 IHDR (ground truth) | `video.resolution` | `run.json` `viewport` |
|---|---|---|---|
| native | 875×869 | **875×869** | 875×869 |
| downscale `oh=360` | 362×360 | **362×360** | **875×869** |

⚠ **THE NATIVE LEG PROVES NOTHING ON ITS OWN AND MUST NOT BE CITED AS IF IT DID** — there the two
fields agree, and they would have agreed before `m28` too, for the old and wrong reason. **THE
DOWNSCALE LEG IS THE PROOF:** `video.resolution` follows the delivered pixels while `viewport` stays
with the window. This was pre-declared as prediction `P-B` and it held.

📌 **WHICH ONE SHOULD A CONSUMER USE? `video.resolution`, always, for anything that touches pixels** —
it is the only field that describes the frames on disk, and it is the one `labels.jsonl`
`width`/`height` and every `bbox_px` are computed from. `viewport` is provenance about the capture
environment, not a description of the output.

🚨 **AND IN DELIVERY MODE `video.resolution` IS THE ONLY RECORD THERE IS.** `labels.jsonl` is not
written (`bWriteLabels = !bDeliveryMode`) and `run.json` is not shipped at all — a delivered session
folder contains `annotation.json` + `run_summary.json` + the frames. **Before `m28` a delivered
dataset therefore contained NO artifact stating the true dimensions of its own pixels.** That is the
defect this change exists to fix, and `GATE I` is the leg that proves it reaches the shipped mode.

**PRECEDENCE, highest first**, resolved in `StartRun`, with **`-1` meaning ABSENT and `0` meaning a
deliberate request for NATIVE** — the sentinel is what keeps every level distinguishable:

| # | source | how |
|---|---|---|
| 1 | per-run argument | dashboard `outputHeight` in `capture_start`; console `IAI.Capture.Start … oh=<n>` |
| 2 | between-runs override | `IAI.Capture.OutputHeight <height\|0\|-1>` (`-1` clears) |
| 3 | ini | `DefaultGame.ini` `[AnomalyCapture] CaptureOutputHeightDefault` |
| 4 | compiled default | `0` (native) |

**ONE UNCONDITIONAL `StartRun` line names the requested height and WHICH LEVEL WON** (`G139`), and a
separate **first-frame line carries the MEASURED `native WxH -> output WxH`** — the authoritative pair.
⚠ **The `StartRun` line deliberately carries NO `WxH`:** at `StartRun` no frame has been grabbed, so the
only available source would be `GetViewportSize()`. The viewport-vs-frame disagreement is already
instrumented by the **RESOLUTION DELTA (3-rect)** line; a second predictor of the same quantity is
duplication, not evidence.

📌 **`resamples_performed` is INTERNAL/LOG ONLY** and is deliberately **not** in `run_summary.json`. It
increments **only on the successful-write branch**, in lockstep with `FramesWritten`, so it reads
EXACTLY `0` on a native run and EXACTLY `framesWritten` on a downscaled one.

⚠ **THE ONE DECLARED RESIDUAL:** because `H_out` is snapped even and `W_out` is derived and snapped
even, the delivered image's aspect can differ from the render's by up to about **one part in a
thousand**. Labels stay EXACTLY consistent with the delivered pixels — `bbox_px`, `labels`
`width`/`height` and `video.resolution` all use the **same** derived pair. What drifts is only the
render projection aspect versus the output image aspect. **Sub-pixel, declared in advance, not gated.**

⛔ **NO ALIGNMENT CLAIM IS EXTENDED.** The matrix above certifies alignment at **1280×720 and 1281×721
only**. `m28` can produce output sizes outside that set and **certifies nothing at them.**

📌 **The host encoder needs no change:** `encode_watcher.py` already pads odd dimensions to even
(`-vf pad=ceil(iw/2)*2:ceil(ih/2)*2`), and a derived pair is always even, so the pad becomes a no-op on
downscaled runs. `verify_capture.py` / `overlay_watcher.py` also need none — they draw from
`labels.jsonl` onto the frames, and both now carry the output pair.

⚠ **ALIGNMENT is certified at 1280×720 and 1281×721 ONLY.** B1's pose precondition compares a **PIXEL**
bbox against `CALIB_BBOX`, frozen at 1280×720 with an 8 px tolerance, so it **cannot run at any other
resolution** — four legs were blocked by it and **three of the four had a provably motionless camera**
(`modal_rot` stable, `distinct=1`, modal 100 %). M3's per-component ratio is a uniform **1.5**, exactly
the resolution ratio. **This is an INHERITED gap, not one S4 introduces: it is unverifiable on BOTH grab
points.** Normalising `CALIB_BBOX` to NDC would unblock it and is **filed alongside B2** as a definition
change needing its own eight-control gate. → **G107**'s family.

⚠ **Packaged builds are DPI-UNAWARE** (**G114**) — Windows reports 96 DPI to the process whatever the
desktop scale is. A display-scale change therefore does **not** reach the process, and a null measured
that way is an artifact of insulation, not a result. M1b was re-run with the process **forced DPI-aware
and verified** by `GetProcessDpiAwareness` before the leg; that is the leg the table cites.

⚠ **Certification scope:** S3a certified the mechanism at **ratio ≈ 1.0 only**. Ratio-independence,
behaviour under stall, and marker-verified frame identity were certified at **S3b** (`m24`).
`node.bounds`/UI/resolution consequences of flipping the default belong to **S4**, which plans it as a
**client-visible change** (the SVE image has no UI in it).

**The UI claim's limit, verbatim — it is REASONING past the measured case and does not get promoted:**

> **UI exclusion is verified in pixels against Canvas AHUD output in `CB_GateLevel` at 1280×720
> windowed. Exclusion of Slate/UMG follows from compositing order and is REASONED, NOT MEASURED.
> Not verified in a gameplay level.**

## Per-target / global state-capture convention
The generalization of M1's AMB-3 capture-baseline rule, followed by **every** state-mutating anomaly:
- **Capture exactly the state you mutate, before mutating it.** Globals: one baseline (e.g.
  `time_dilation` captures `GetGlobalTimeDilation`; `camera_clipping` captures `GNearClippingPlane`).
  Component/actor anomalies: a **small per-target record keyed to the weak ptr** (e.g.
  `lighting_mismatch` stores intensity/color/visibility/cast-shadow per `ULightComponent`;
  `lod_corruption` / `lod_popping` store the prior forced-LOD per `UMeshComponent`).
- **The record may key to a common base over a heterogeneous target set (M3).** `lod_corruption` and
  `lod_popping` key their record to `TWeakObjectPtr<UMeshComponent>` and let `AnomalyLod` dispatch the
  static-vs-skeletal getter/setter via `Cast<>`. A single `Apply` therefore captures/forces/reverts a
  **mixed** set — e.g. one `lod_corruption Bot` handles the Bot's static mesh component *and* its two
  skinned components together. The convention is unchanged; only the record's pointer type widened.
- **Revert restores the captured state and skips stale weak ptrs** (same GC-safety as `missing_object`).
- **Re-apply reverts-then-reapplies** so there is always exactly one capture set (no stacking — e.g.
  `lighting_mismatch recolor` re-applied never strands a recolored light; `lod_popping` re-applied
  mid-oscillation re-captures the *true* baseline, never a popped value).

## How to add an anomaly
1. Implement `IAnomaly` in `Private/Anomalies/Anomaly_<Name>.{h,cpp}`. In `Apply`: resolve+cache
   targets as weak-ptrs (`AnomalyTargeting::FindActorsMatching` for actors, `FindComponentsMatching<T>` for
   components, `AnomalyLod::ResolveLodComponents` for LOD-forceable static+skeletal meshes), parse args via
   `AnomalyArgs`, **capture per-target/global state before mutating** (convention above), mutate, return
   `false` if an actor/component anomaly matched zero targets (AMB-2). Undo in `Revert` (restore
   captured state per live target; skip stale). Override `Tick(float)` only if it ticks.
2. Register it in `UAnomalyInjectorSubsystem::Initialize`: `Register(MakeUnique<FAnomaly_<Name>>())`.
3. Include the header path-relative from `Private/`: `#include "Anomalies/Anomaly_<Name>.h"` (gotcha G10).
4. Add a catalog row above and a smoke line to the runbook.
No interface change is needed for actor-, component-, world-, or global/console-driven shapes (proven
across all seven anomalies — the M1 `IAnomaly` lock held through M3, including the ticking
`lod_popping` and the heterogeneous static+skeletal `lod_corruption`).

## Deferred (intentional — not forgotten)
- **`AnomalyCvar` (generic cvar capture/restore, the planned A2)** → post-process / scalability milestone.
  Deferred because its only would-be M2 consumer (`camera_clipping`) is driven by a console **command**,
  not an `IConsoleVariable` (gotcha G13), so A2 had zero real consumers and would have violated the
  ≥2-consumers bar. It lands with its first genuine `IConsoleVariable` anomaly.
- **`color_corruption` / `aliasing` / `blur`** → a dedicated post-process milestone (needs a deliberate
  injection-point decision: global PP volume vs camera `PostProcessSettings` vs PP material).
- **high/low-speed** → substantially covered by `time_dilation`; a `GlobalAnimRateScale` variant can be
  added later only if the label taxonomy needs the distinction.

## Limitations
- **Cross-anomaly target overlap** = last-writer-wins on the single `bHidden` flag (gotcha G12). Fine
  for one-anomaly-at-a-time use (terminal state after `RevertAll` is always visible). Compound /
  simultaneous anomalies will need a subsystem-level "hidden-by" coordinator — addable **without**
  touching `IAnomaly`. Flagged, not built. **The m6 auto-injector avoids this on its own path** with a
  one-anomaly-per-actor scheduler invariant (G30), so it never contends the flag; the coordinator is only
  for *deliberate* compound same-actor anomalies.
- **Manual + auto injection together is unsupported (m6).** Manual selector/console injection of a pool id
  while the auto-injector is running clobbers via the registry's one-instance-per-id (the auto-injector can
  only track its own fires). Detected and warned, not blocked (R-COEXIST).
- **Auto-injection reproducibility is over the bridge / Step granularity, not real Play.** The seed
  reproduces the choices given the same visible-set sequence; full run reproducibility with fixed visible
  sets is a capture/replay-pipeline concern (G30).

## Game-agnostic invariant
The module depends only on `Core`/`CoreUObject`/`Engine`/`InputCore`/**`Foliage`** and never references host (StackOBot) types.
⚖ **WIDENED BY OWNER RULING, 2026-08-21 (`m31`): the invariant is not just "never reference host TYPES" —
it is "NEVER LET CORRECTNESS DEPEND ON ANYTHING A HOST CAN REDEFINE."** The m31 defect was this invariant
violated through an ENGINE GLOBAL rather than a host type: the SVE capture handshake compared two
independent reads of `GFrameCounter` (game-side arm vs publish-side check), a pairing that holds only
under the stock engine loop's increment placement. The first host running a forked loop (Firewalk:
fixed sim + variable render) redefined that global's cadence — as a fork is entitled to — and the
shipping capture path silently wrote zero frames. **The cure class is structural, not compensatory:
mint identity ONCE at a site the plugin owns, carry it BY VALUE, pair by ORDER — never by comparing two
independent reads of any engine global, and never with a tolerance window (clocks at different rates
diverge without bound). No sniffing an engine mode and branching: immunity by construction, not by
detection.** The backbuffer path already embodied this shape (arm-minted id, FIFO consume, no
render-side frame-number read) and is why it survived the fork untouched; m31 re-keys the SVE path to
the same shape and replaces the one residual engine-global dependence in both paths
(`GFrameCounter`-as-token uniqueness) with a plugin-owned serial.
⚖ **`Foliage` is the `m27` addition (2026-08-20, owner ruling)** — a **PRIVATE** dependency so it does not
propagate to `AnomalyCapture`/`AnomalyControlServer`, on an **ENGINE Runtime** module present in every UE build
(`Runtime/Foliage/Foliage.Build.cs`: no editor gating, no `ModuleType` override). It exists so
`IsRenderableComponent` can exclude `AInstancedFoliageActor` **by TYPE** rather than by class-name string.
🚨 **A name match was refused because it would fail SILENTLY on a rename; a type reference breaks the build,
and a compile error is the loudest failure available.** ⛔ No string match is to be added alongside it.
📌 `AInstancedFoliageActor` is declared `MinimalAPI`, so its **member functions are not exported** — but
`MinimalAPI` **does** export `StaticClass()`, which is all `IsA<T>()` needs. `IsA` links; a member call would not.
(`InputCore` is the m5 addition — `FKey`/`EKeys` for raw input polling + keybinds; no Slate/UMG. **m6 added no
dependency** — the auto-injector's `FRandomStream` is Core, its HUD/input reuse the same Engine/InputCore types.) The
selector AND the auto-injector are game-agnostic by construction: their HUDs draw via `UDebugDrawService` (no host HUD
class — G25) and their input is raw key polling (no host input mappings — G26); the auto-injector's pool/keybinds are
about the plugin's own four ids, never host types. All anomalies use public UE APIs only —
`SetActorHiddenInGame`, `UGameplayStatics`, `ULightComponent`
setters, `UStaticMeshComponent::SetForcedLodModel`, `USkinnedMeshComponent::SetForcedLOD`/`GetForcedLOD`/
`GetNumLODs`, and the `r.SetNearClipPlane` console command + `GNearClippingPlane` global. **Neither M2
nor M3 added a dependency** — the M3 LOD work touches only Engine component types (and specifically
still avoids `RenderCore` — G13). The Bot match in M3's gates is by class substring (`BP_Bot_C`), never
a host type or label.

## Verification model
Non-visual gates are checked in PIE via the `unreal-mcpython` MCP bridge (state/log reads: match
counts, `IsActive`, world time-dilation value, blinking toggle logs); the owner eyeballs the visual
gates (blinking, felt slowdown). See gotcha G8 for the bridge setup.
- **Bridge on 5.1 (M2.6):** the bridge (host tooling, not part of this plugin) is **GenOrca
  UnrealMCPython**, which targets UE 5.6+. To build on 5.1 its **`BehaviorTreeEditor` dependency was
  severed** (those graph-node UCLASSes are unexported pre-5.6 — G8). The bridge's BT-graph **authoring**
  tools (`build_behavior_tree`, `get_selected_bt_nodes`) are therefore **unavailable on 5.1**; everything
  this project's verification uses — `execute_python`, Output-Log reads, actor/component state reads —
  is intact. M2.5's full re-gate (Simulate session in an `EWorldType::PIE` world) was driven over the
  severed bridge. Full diagnosis + the restore-on-5.6 recipe are in G8.
