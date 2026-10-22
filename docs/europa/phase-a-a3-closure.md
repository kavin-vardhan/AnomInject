# Europa Phase A, A3 shared C++ closure

2026-10-22, under ED1–ED7, EC1–EC6, EB1–EB5 and EA1–EA15. This records the source design before working Editor Development compiles. It is not runtime or final-build qualification. Evidence is in `D:/IA_Europa/Receipts/09404`; the run report records results and the exact tested commits.

## Shared reflected contract

`Source/AnomalyInjector/Public/Compat/AnomalyTickableWorldSubsystem.h` declares the literal parent `UWorldSubsystem, FTickableGameObject`. Its implementation is the matching Private/Compat file. Injector, AutoInjector, Selector, Capture and ControlServer inherit this same class on both versions; none retains a DoesSupportWorldType override.

| Contract | Implementation | 4.25 | 5.1 |
|---|---|---|---|
| Creation | ShouldCreateSubsystem | Super first, then Game or PIE only | Super first, then Game or PIE only |
| Registration | One FTickableGameObject base | One automatic registration | One automatic registration |
| CDO tick type | GetTickableTickType | Never; other instances retain the base default | Same engine expression |
| Initialized state | bInitialized, Initialize, Deinitialize | Starts false; checked false→true→false | Same checks and order as engine tickable subsystem; no extra Super chain |
| Tick admission | IsTickable | !IsTemplate() && bInitialized; no Always override | Base FTickableGameObject IsTickable |
| Unconditional veto | IsAllowedToTick, final | Declared for the shared class; 4.25 tick loop uses IsTickable | Exact engine expression !IsTemplate() && bInitialized, final |
| Tick world | GetTickableGameObjectWorld | GetWorld() | GetWorld() |
| Tick check | Tick | IsInitialized check | Same engine check |
| Stats | GetStatId | Engine-style PURE_VIRTUAL fallback, concrete classes retain stats | Same |
| Audit | Private cpp only | !Shipping, default off; off path one branch, no allocation/lock | Same; cost awaits R6 |

The audit identifies instance, class, world, global frame, world time and lifecycle. A5's independent host probe supplies the per-world eligible-update denominator; TimeOnly/paused updates remain reported but excluded. Audit is off in timing/pixel regression legs. The host probe currently has an empty module only, no behavior or shim-audit access.

## Reflected surface and later backends

The full annotation inventory is `audits/reflected-surface.json`. Capture has no UPROPERTY or UFUNCTION members. Its native Async, PreviewTee and RunLog are already opaque, non-reflected holders. Native `FAnomalyCapturedFrame` and `AnomalyLabel::FCaptureSnapshot` appear only as forward-declared references/pointers in non-reflected methods. Renderer/readback ownership stays inside Async. Control's WebSocket types already moved into `Private/Compat/AnomalyControlTransportState.h` in A2.

Texture facade users retain portable public types: UWorld, AActor, UMaterialInterface, UTexture2D, UObject and value/stat structures. The only reflected method is Injector's `OnWatchedTargetEndPlay(AActor*, EEndPlayReason::Type)`, which is portable. No RHI, skinned-asset, ticker or WebSocket type remains in a reflected member/method.

Nine properties replace TObjectPtr: Injector's MissingTextureChecker, CorruptedTexturePink, TexCorruptUv, TexCorruptNormal, TexCorruptNoise and TexCorruptStrongRefs; Bench's Root, PartEligible and PartIneligible. Names/specifiers remain unchanged. Eight raw scalar pointers initialize to nullptr; the raw-pointer TArray initializes empty. All remain UPROPERTY-traced strong references.

- Phase C: `Public/Compat/AnomalyCaptureBackend.h` declares IAnomalyCaptureBackend and CreateCaptureBackend425; its 4.25 implementation will land in `Source/AnomalyCapture/Private/Compat/AnomalyCaptureBackend425.cpp`, owned through the existing native Async holder.
- Phase E: `Public/Compat/AnomalyTextureAdapter.h` declares IAnomalyTextureAdapter and CreateTextureAdapter425; its 4.25 implementation will land in `Source/AnomalyInjector/Private/Compat/AnomalyTextureAdapter425.cpp`, behind IAnomaly and the current portable facade.
- Phase F: the existing opaque control transport holder receives its 4.25 backend in `Source/AnomalyControlServer/Private/Compat/AnomalyControlTransport425.cpp`.

C/E are declarations only. The 5.1 backends and callback order are not rewired. No later backend needs another reflected declaration change for these seams.

## Admission and disabled closure

AnomalyCapabilities is the common registry, auto and selector admission policy. Stock 4.25 admits blinking, lighting_mismatch and lod_corruption; missing_object additionally requires -IAIBench. Other and unknown IDs fail closed. Existing 5.1 catalogue, nine pool keys, arrays, default enabled set and draw order remain. The capability filter runs before candidate selection/random draw. Existing selector/auto lists are filtered, not enlarged; lighting_mismatch and lod_corruption remain available through the injector's manual interface.

The 4.25 constructor compiles no plugin-asset finders and leaves asset fields null. It registers a stock-no-Nanite component probe in Injector without changing AllowNanite; 5.1 Capture remains probe owner. Compat preserves 5.1 API operations while selecting 4.25's SkeletalMesh identity, RenderData owner, camera cache POV/global near plane, absent contact shadows and PhysX triangle-mesh presence. The final collision accessor is an additional ordinary API substitution in the already-reached Viewport file, with no new renderer dependency or projection algorithm.

All 18 planned Capture implementation TUs are audited: eleven require an outer ANOMALY_CAPTURE guard, seven already have a sufficient boundary. Complete disabled PreviewTee, Async and RunLog types cover normal/FVTableHelper/destructor paths. Preview facade exports clear outputs and return false. Control's protocol/snapshot implementations are guarded; its inert subsystem reports compiled-out and creates no listener.

Thirteen advanced Injector implementation TUs compile only on exact 5.1. The 4.25 AnomalyUnsupportedTexture facade returns unavailable/false, empty modes, zero measurements and zero stats; it never reports an applied, ready or measured result. Existing plugin shaders and bench implementations remain the A2 inert shells.

## Working-build and strict tooling

The scoped wrapper admits only StylizedParisStreetEditor Win64 Development and ReGate51 StackOBotEditor Win64 Development. Every working build requires a clean committed SHA, the same candidate SHA, successful toolchain/descriptor admissions, -NoEngineChanges and -NoBuildUHT. It exports outdated actions before executing them and rejects every output outside the owned host. Compile actions use one worker initially because the owner's apps remain active. These builds are working results; A4a/A4b own gates A1, A2 and R1.

4.25 UBT source preflight is `precompile/425-write-routing.json`, including numbered source excerpts. Installed source/dependency/action caches and makefiles go to project Intermediate. Read-only engine UHT modules do not export generated headers or timestamps. UHT -abslog derives from scoped UBT -Log. Its process UE-CmdLineArgs supplies UserDir before config/log creation; UHT Saved/Config is not a new C exception. EngineUserDir 4.25 and installed XML cache remain declared C exceptions. Only the existing installed BuildRules directory is allowed on E.

The ledger stores one initial inventory and hashed per-operation deltas, includes root folders, and records compiler descendants using read-only process snapshots plus UBT action/command receipts. Build-window Windows/WER growth is conservatively charged to C. Successful 5.1 builds with zero engine actions may change only the ED4 metadata names under Engine/Intermediate/Build, subject to 100 MB. No engine binary/source write is admitted.

`tools/strict_include_build.py` now requires explicit engine/plugin/project roots, restricts work to the three owned hosts, selects UE4Editor paths for 4.25, emits no 4.25 IncludeOrderVersion, and checks actual no-PCH per-file/header/generated compile actions. It restores exact rule bytes and generated-file receipts and performs a normal build afterward. It has not been run in A3; A4 must exercise its can-fail and final-build proofs.
