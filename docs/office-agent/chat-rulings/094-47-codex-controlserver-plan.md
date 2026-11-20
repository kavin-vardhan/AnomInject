# 094-47 — UE 4.25 control-server port (Codex read-only plan)

> Saved by chat Claude from the owner's paste on 2026-11-05. Codex's Plan Mode prevented it from writing this file itself. The content is Codex's; chat's ruling is `094-47-plan-ruling.md`.

## Summary and boundaries

Enable AnomDash to connect to a packaged Development build of Europa, list its anomalies, and start/stop the existing C5 capture pipeline.

Two changes are necessary: a UE 4.25 WebSocket transport and a bridge into C5's capture lifecycle. Changing the build condition alone would expose missing transport APIs and still invoke C1 capture.

Reviewed sources:

- `feat/europa-port`: `886e1a0a28d2521559b4122ac0610e7be803d665`
- UE 5.1 reference: `d7eee97731812384d4fd00af9032254923363ada`
- Dashboard: `D:/IntrusiveAnomalies/anomaly-dashboard`, HEAD `7ea2e51bf55e19c2949641bbff2ec72afb4ae493`
- Installed engine: UE **4.25.4**, changelist **14469661**

Owner decisions: retain the dashboard's connection protocol and capture controls; qualify a C5-compatible preset. Include the requested small dashboard change to show **LOD corruption**.

Shipping remains excluded. Preview streaming, arbitrary capture durations, JPEG, resizing, targeted captures and export parity are outside this bounded port.

## Verified behavior and API compatibility

Plugin citations below identify the revision and repository-relative file. Engine citations are relative to `E:/EpicGames/UE_4.25/Engine`.

### What the 5.1 server does

At `d7eee97`, `Source/AnomalyControlServer/Private/AnomalyControlServerSubsystem.cpp`:

- Creates a server through `IWebSocketNetworkingModule::CreateServer()` and pumps it from the world subsystem's game-thread tick.
- Starts only after `IAI.Server.Start [port]`; default port **8077**. `IAI.Server.Stop` and `IAI.Server.Status` manage/report it.
- Advertises `ws://127.0.0.1:8077`; connection admission rejects non-loopback peers. The advertised address alone does not establish the listener's actual bind address.
- Reads `[AnomalyControlServer] Token` through `GConfig` and `GGameIni`; otherwise generates a GUID token for that listener start.
- Requires `{"type":"hello","token":"…","v":1}`. Success returns `welcome`; an incorrect token returns `error` with `code:"bad_token"`. Unauthenticated peers become rejected after five seconds.
- Sends UTF-8 JSON in **binary WebSocket messages**, with `Send(..., false)` disabling the engine networking length prefix.
- Sends optional preview messages as **16-byte little-endian `AIF1` headers followed by JPEG bytes**. Preview capture is suspended while capture is active.
- Defaults to snapshots every 0.2 seconds and previews every 0.5 seconds; subscriptions can change those intervals.

Relevant baseline locations: listener/token `:124`, ticking `:222`, peer admission `:272`, authentication `:350`, binary sending `:805`, previews `:866`.

| Messages | Destination or result |
|---|---|
| `hello` | Authentication; `welcome` or `error` |
| `list_anomalies` | Injector catalog → `catalog` |
| `subscribe`, `request_frame` | Snapshot/frame subscriptions and preview requests |
| `inject`, `revert`, `revert_all`, `set_viewport_scoping` | `UAnomalyInjectorSubsystem` |
| `set_hud` | Selector or auto-injector subsystem |
| `set_poll_radius`, `set_min_screen_coverage` | `AnomalyViewport` |
| `auto_config`, `auto_run`, `auto_step`, `auto_fire_once` | `UAnomalyAutoInjectorSubsystem` |
| `capture_start` | `UAnomalyCaptureSubsystem::StartRun()` |
| `capture_stop` | `StopRun()`, then `capture_stopped` with final status |
| `capture_status` | Capture status, including frame cap, directory, session ID and optional pacing |
| Four `bench_*` messages | Existing flag/map/inactive-capture-gated bench commands |

The capture-start handler acknowledges dispatch, rather than proving admission or completion. Acceptance must use authoritative status and artifacts.

### Why Europa currently disables it

`feat/europa-port:Source/AnomalyControlServer/AnomalyControlServer.Build.cs:10` requires `AnomalyEngineRules.Is51(Target) && Target.Configuration != UnrealTargetConfiguration.Shipping`. Consequently, UE 4.25 receives `ANOMALY_CONTROL_SERVER=0` and omits the capture/WebSocket/JSON dependencies. The branch already provides a compatible tickable-world base and moves transport ownership behind a private state pointer. Preserve those corrections.

### UE 4.25 equivalents

| 5.1 dependency or API | Installed UE 4.25 evidence and planned treatment |
|---|---|
| `IWebSocketNetworkingModule`, `CreateServer()` | No equivalent public factory. `Plugins/Experimental/WebSocketNetworking/Source/WebSocketNetworking/Private/WebSocketNetworkingPlugin.cpp:6` defines only a private `IModuleInterface` implementation. Supply a plugin-owned factory. |
| `IWebSocketServer::Init/Tick` | Private, non-exported `FWebSocketServer` exists: `…/Private/WebSocketServer.h:8`, `:16`, `:19`. Do not depend on its private headers or linker visibility. |
| `INetworkingWebSocket` | Private, non-exported `FWebSocket` at `…/Private/WebSocket.h:13`. Provide transport-owned connections instead. |
| Networking delegates and receive callback | Old declarations use `FWebSocket*` and the misspelled `FWebSocketPacketRecievedCallBack` (`…/Private/WebSocketNetworkingPrivate.h:16`); `SetRecieveCallBack` at `…/Private/WebSocket.h:34`. Adapt through the new transport boundary. |
| `SetSocketClosedCallBack` | No equivalent. Use libwebsockets' `LWS_CALLBACK_CLOSED` (`Source/ThirdParty/libWebSockets/libwebsockets/include/Win64/VS2015/libwebsockets.h:1203`). |
| `RemoteEndPoint(false)` | Use public `lws_get_peer_simple` (bundled header `:5447`). |
| `Send(data,size,false)` | **Not interchangeable** with 4.25's `FWebSocket::Send`, which prepends a four-byte size (`…/Private/WebSocket.cpp:151`; receive expects it at `:320`). Use raw payloads through `lws_write` (`:4931`). |
| Complete-message delivery | Reassemble fragments with `lws_remaining_packet_payload` and `lws_is_final_fragment` (`:5231`, `:6002`). |
| `UTickableWorldSubsystem`, `DoesSupportWorldType` | Branch's existing `UAnomalyTickableWorldSubsystem` already adapts (`WorldSubsystem.h:16`, `Subsystem.h:61`, `Tickable.h:92`). |
| Optional stats headers | Keep the `__has_include` checks and `Compat/AnomalyStatsIncludes.h`. |
| Descriptor configuration exclusions | Europa already uses `BlacklistTargetConfigurations` (`ModuleDescriptor.cs:464`). Keep the Shipping exclusion. |

World subsystem lookup, JSON, UTF-8 conversion and config access all exist in 4.25. WebSocketNetworking is present, but its private implementation is unsuitable. Bundled libwebsockets 3.0.0 is available (`lws_config.h:34`; Win64 static linkage `libWebSockets.Build.cs:80`; `websockets_static.lib` present). HTTPServer is present but provides no WebSocket API; do not substitute HTTP endpoints.

### Dashboard expectations and the separate C5 gap

AnomDash opens `new WebSocket(url)` without a subprotocol, sends `hello`, subscribes to snapshots and frames, and requests the catalog (about 5 Hz snapshots, 6 Hz previews). It accepts binary or text JSON and recognizes `AIF1` (`src/transport/AnomalyClient.ts:78,91,102,134,172`; `src/transport/protocol.ts:1`).

Capture start sends `{"type":"capture_start","dir":"<capture base folder>","format":"png","seed":4242,"maxFrames":2352}`; stop sends `{"type":"capture_stop"}` (`src/components/SetupPanel.tsx:323`).

The current Europa handler calls generic `StartRun`, which on 4.25 starts the C1 path without C5 admission, routes, warmup or scheduling (`AnomalyCaptureSubsystem.cpp:3604`; C5 console controller `Compat/AnomalyCaptureLabels425.cpp:422`; unconditional completion exit `:383`; warmup-timeout exit and uncancellable ticker `:430`; preview stubs `AnomalyCaptureSubsystem.cpp:11639`). C5's schedule is 2,352 frames (`AnomalyStreetBench425.h:13`); camera-regression flags can alter it (`AnomalyCameraSession425.cpp:19`), so the bridge must consult that function.

## File-by-file implementation

### Plugin transport and dispatch

| File | Change |
|---|---|
| `Source/AnomalyControlServer/AnomalyControlServer.Build.cs` | Enable `(Is425 \|\| Is51) && !Shipping`. Share `AnomalyCapture` and `Json`. Keep `WebSocketNetworking` for 5.1; use `Sockets`, `libWebSockets`, `OpenSSL`, `zlib` for 4.25. |
| `Private/Compat/AnomalyControlTransport.h` **new** | Private transport interface: start, tick, stop, binary send; stable connection IDs; open/message/close events. |
| `Private/Compat/AnomalyControlTransport51.cpp` **new** | Wrap the existing 5.1 factory/server/socket callbacks unchanged in behaviour. |
| `Private/Compat/AnomalyControlTransport425.cpp` **new** | libwebsockets implementation; bind IPv4 loopback; root-path handshake with no subprotocol; binary messages without length prefix. |
| `Private/Compat/AnomalyControlTransportState.h` | Replace direct server/socket ownership with the transport and connection IDs. |
| `Public/AnomalyControlServerSubsystem.h` | Connection IDs instead of socket pointers; keep base, constructors, out-of-line destructor. |
| `Private/AnomalyControlServerSubsystem.cpp` | Select transport by engine; keep token/log markers and protocol v1; route 4.25 capture to the C5 API; return an error for refused 4.25 starts. |

Transport requirements: service on the game thread and dispatch after `lws_service` returns; `LWS_PRE` storage and writable callbacks; reassemble fragments; bounds of 1 MiB inbound, 16 MiB outbound per connection, 8 connections; single teardown with callbacks suppressed after deinit; keep bad-token and unauthenticated refusal; keep `ControlProtocol.*`, `AIF1` and `ControlSnapshot.cpp` wire structure; keep the Shipping exclusion; no engine-source or host net-driver changes.

### C5 lifecycle bridge

| File | Change |
|---|---|
| `AnomalyCapture/Public/AnomalyCaptureSubsystem.h` | Add `StartC5Run(RunDirectory, Seed, Routes, bExitOnFinish, OutError)`. |
| `AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | 4.25 bridge; correct directory, session, seed and planned count; warmup as an active transaction; cancel pending warmup in `StopRun` and deinit; refresh final counts after drain. |
| `Compat/AnomalyCaptureLabels425.h/.cpp` | Extract the C5 console lambda into a shared path, with owning world, ticker handle and request generation. The console keeps its exit policy; the dashboard uses keep-alive. |
| `Compat/AnomalyCaptureSession425.h/.cpp` | Expose real requested/completed counts; unwind failed init; finalize artifacts before publishing the final status. |
| `Compat/AnomalyViewMetadata425.h` | Office include-order fix: `SceneRendering.h` before `SceneViewExtension.h`, inside the 4.25 guard. |

One C5 implementation serves console and dashboard. It keeps:
- `-IAIBench`, route validation, host-depth/private-pass admission, warmup, fixed timestep, scheduling and restoration;
- the 10 s minimum warmup, 2 s quiet interval and 120 s timeout;
- the ticker handle removed on cancel (`Ticker.h:41,59`).

Its rules:
- cancelled or obsolete requests never start later, and duplicate or other-world starts are rejected;
- the dashboard process stays alive on completion, stop and recoverable failure;
- mutations are refused during a capture, while subscriptions, status and stop stay available;
- a disconnect doesn't stop the capture.

For 4.25 `capture_start`:
1. Native PNG auto capture only.
2. Routes come from the auto-injector pool; `lod_corruption` is never aliased to `lod_popping`.
3. An omitted seed comes from the auto-injector.
4. `maxFrames` must equal the C5 schedule.
5. `dir` is a base folder, allocated as a fresh `session_<timestamp>`.
6. `running=true` through warmup, capture and drain.
7. An explicit stop emits `capture_stopped`.

An early stop is partial and never relabelled as complete.

### Dashboard and verification files

| File | Change |
|---|---|
| `anomaly-dashboard/src/store.ts` | Remove `lod_corruption` from `HIDDEN_ANOMALY_IDS` (owner's instruction). |
| `anomaly-dashboard/src/anomalies.ts` | Add "LOD corruption" metadata beside the LOD entry. |
| `Source/AnomalyControlServer/Private/Tests/AnomalyControlTransportTests.cpp` **new** | Framing, fragmentation, teardown and bounded-queue tests. |
| `tools/europa/control_server_425_smoke.ps1` **new** | .NET `ClientWebSocket` protocol checks and receipt comparison against a console reference; never builds or launches. |

The qualified dashboard preset is Random mix, the four default C5 anomalies, PNG, native size, seed 4242, and a custom frame count of 2352. The dashboard ignores non-auth error messages (`AnomalyClient.ts:182`). There is no 4.25 preview stream.

## Packaged Development token, configuration and startup

- Token: `[AnomalyControlServer] Token=` in the project's `Config/DefaultGame.ini` before cooking; otherwise a random token per listener start, announced in the log.
- The server reads merged `GGameIni`: generated `<ProjectSavedDir>/Config/WindowsNoEditor/Game.ini`. A loose ini beside the exe is not an override.
- Logs live in `<ProjectSavedDir>/Logs`; `-abslog` is supported.
- AnomDash discovers the PID, command line and log `LogAnomalyServer` token markers (`src-tauri/src/discovery.rs:149,378,901,1405`). Keep the markers and flush the startup announcement.
- Start the listener with `IAI.Server.Start` (console or `-ExecCmds`), with C5's bench/admission flags. Omit `-IAICaptureExit` for dashboard runs.
- The package must include the 4.25 materials and the private-pass resources.

## Gates

1. **Compile:** 4.25 Development Editor and packaged Development Game; the 5.1 transport regression; Shipping excluded, with no listener.
2. **The real desktop dashboard:**
   - discovers and authenticates from the packaged log;
   - configured and random tokens both work;
   - a bad token is refused;
   - restart and port-conflict cases behave;
   - the listener is bound to loopback;
   - snapshots and the catalog arrive, showing all four defaults including LOD corruption.
3. **The C5 lifecycle:**
   - the dashboard starts C5 with its warmup;
   - stop works during warmup and during frames;
   - there is no delayed start;
   - the game and listener survive for a second capture;
   - reconnecting recovers the status;
   - failure and teardown leave no ticker or state behind.
4. **Receipt equivalence,** per RHI, on the same packaged binary: a console-started C5 against a dashboard-started C5.
   - Seal the predictions first.
   - Same schema and schedule, `requested=written=2352`, 0 dropped, 2,352 paired rows, receipt `PASS`, and restoration.
   - The same seed, routes, label correspondence and private-pass evidence.
   - Normalization only for paths, IDs, absolute frame offsets and wall-clock measurements.
   - The early stop is tested separately.
5. **Regression:**
   - the dashboard tests pass, and the desktop build includes the LOD display;
   - 5.1 connect, start and stop still work;
   - the cooked materials and private pass work from the packaged exe;
   - all existing capture gates are kept.

**Risks:** raw framing, callbacks surviving teardown, stale status, uncancelled warmup, inherited process exit, and cooked-resource failures.

PLAN READY
