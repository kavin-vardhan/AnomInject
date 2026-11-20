# 093-01 — Claude Code verification of the Codex natural-causes feasibility review

Written 2026-10-13 by Claude Code (brief 093-01). Read-only.

- **Review checked:** `_reviews\093-01-codex-natural-causes-feasibility.md` (relay run `_relay\runs\2026-10-13-093-01-natural-causes-feasibility\`, `gpt-6-astra` / `max`, 3,981 s).
- **Sources read:** UE 5.1.1 at `D:\UESource\UnrealEngine` (paths below are relative to `Engine\Source\Runtime\` unless they start with `Engine\` or `Config\`); the release worktree `E:\IA_BuildCache\_r917\AnomalyInjector` at `release/2026-10-05` `d7eee97` (clean; paths below start `Source\`).
- **One empirical, read-only check:** a string scan of the bench's existing cooked container index `StackOBot\Builds\BenchGate\Windows\StackOBot\Content\Paks\StackOBot-Windows.utoc` (built 2026-10-04), both encodings, with a negative control. Nothing was built, cooked, launched or edited.
- **Verdicts:** CONFIRMED = the source says what the claim says. CORRECTED = the source contradicts or materially narrows it. UNVERIFIABLE = not decidable read-only, or a negative claim that spot checks agree with but cannot prove.
- Where a row says "(draft)", the claim came from the 093-00 draft rather than the review.

## Counts

**53 claims checked: 46 CONFIRMED · 4 CORRECTED · 3 UNVERIFIABLE.**

## Q4 — engine fallbacks

| # | Claim | Verdict | Evidence |
|---|---|---|---|
| 1 | A material without a usable shader map falls back to the domain default material. Base pass walks the proxy's fallback chain. | CONFIRMED | `Renderer\Private\BasePassRendering.cpp:1747-1760`; `Engine\Private\Materials\MaterialShared.cpp:4207-4219`; default proxy's own fallback `Engine\Private\Materials\Material.cpp:419-427` |
| 2 | A missing usage flag in a cooked/game context logs a warning and the caller uses the default material. | CONFIRMED | `Material.cpp:1790-1819` |
| 3 | (draft) "PIE auto-fixes usage flags"; (review Q4a) editor repair "includes ordinary PIE editor operation". | **CORRECTED** | The repair branch needs `GIsEditor && !FApp::IsGame()` (`Material.cpp:1749`). `FApp::IsGame()` returns true whenever `GIsPlayInEditorWorld` is set (`Core\Public\Misc\App.h:154-161`). The PIE world tick and the PIE viewport draw both run inside `SetPlayInEditorWorld` (`Editor\UnrealEd\Private\EditorEngine.cpp:1837`, `:2137`; flag set at `Editor\UnrealEd\Private\Editor.cpp:947-952`). So a usage check made while PIE is ticking or drawing takes the in-game branch: warning, default material (`Material.cpp:1792-1819`). Repair (and `MarkPackageDirty`, `:1775`) still happens for checks made outside PIE scope: editor viewports, load time, and possibly (INFERRED) the deferred game-thread task the off-thread path dispatches (`Material.cpp:1687-1699`). The conclusion stands that stripping host flags is unusable. What changes is that PIE does not reliably hide this fallback, and a flagless **plugin-owned** fixture would mostly show the real fallback in PIE too, with a residual risk of dirtying the plugin's own package. |
| 4 | A non-default material compile failure reports "Default Material will be used"; failure of the default material itself is fatal. | CONFIRMED | `Engine\Private\ShaderCompiler\ShaderCompiler.cpp:4656-4674` |
| 5 | The surface default is the configured `DefaultMaterialName` (`/Engine/EngineMaterials/WorldGridMaterial`), resolved per domain and asserted at init. | CONFIRMED | `Engine\Config\BaseEngine.ini:147`; `Material.cpp:503-520`, `:559-569` (`checkf`), `:940-947`. Addition: because of the `checkf`, a build missing its configured default cannot start, so the *configured* default is always present in a working build. The stock path is guaranteed only if the host has not overridden it. |
| 6 | A MID texture parameter set to null is a no-op: an existing override stays, and a null entry is never pushed to the render thread. | CONFIRMED | `Engine\Private\Materials\MaterialInstance.cpp:3425-3437`; RT lookup ignores null and walks to the parent: `MaterialInstanceSupport.h:293-317`, `MaterialInstance.cpp:284-291`. MISS-3 as drafted does nothing. |
| 7 | A null or resource-less 2D texture at uniform-buffer fill binds `GWhiteTexture`, not `GEngine->DefaultTexture`. | CONFIRMED | `Engine\Private\Materials\MaterialUniformExpressions.cpp:963`, `:994-1000` |
| 8 | White sampled through the normal-map unpack gives (1,1,0), not a flat normal. | CONFIRMED | `Engine\Shaders\Private\Common.ush:1373-1383` |
| 9 | `GEngine->DefaultTexture` = `/Engine/EngineResources/DefaultTexture`, rooted when cooked. | CONFIRMED | `BaseEngine.ini:149`; `Engine\Private\UnrealEngine.cpp:2904-2913` |
| 10 | (draft) `DefaultTexture` is "the engine's default checker". | UNVERIFIABLE | Asset identity confirmed (row 9); its pixels were not decoded or rendered. |
| 11 | (review Q4e) The flat-normal resource is `/Engine/EngineMaterials/DefaultNormal`. | **CORRECTED** | A separate `Engine\Content\EngineMaterials\FlatNormal.uasset` exists (4,223 B, source `FlatNormal.tga`). `DefaultNormal.uasset` is 1,934,902 B, imported from `DefaultMaterial/DefaultNormal.tga` (the default material's normal map). Its flatness is not established, and its size argues against it. Use `FlatNormal` for NRM-3. `FlatNormal` is not in BaseEngine's startup lists (`BaseEngine.ini:1132-1157`, `:1666-1684` name `DefaultNormal`, not `FlatNormal`), so it needs an explicit plugin cook reference, as the review already recommends for new assets. |
| 12 | There is no unconditional "always cooked" promise; the cooker records startup-loaded packages and their soft references. | CONFIRMED | `Editor\UnrealEd\Private\CookOnTheFlyServer.cpp:5734-5743`, `:9420-9434`. Empirical (one host, the bench, not the office hosts): the cooked index contains `WorldGridMaterial` (1), `DefaultTexture` (4), `DefaultNormal` (2), `FlatNormal` (1), `DefaultDiffuse` (2); the negative control reads 0. |

## Q5 — streaming and VT

| # | Claim | Verdict | Evidence |
|---|---|---|---|
| 13 | The named `r.Streaming.*` cvars exist (PoolSize, UseFixedPoolSize, PoolSizeForMeshes, DropMips, MipBias, UsePerTextureBias, UseAllMips, FullyLoadUsedTextures, FramesForFullUpdate, MaxTempMemoryAllowed, LimitPoolSizeToVRAM, HiddenPrimitiveScale, AmortizeCPUToGPUCopy, …). | CONFIRMED | `Engine\Private\Streaming\TextureStreamingHelpers.cpp:62-292`; `StreamingManagerTexture.cpp:1300`. 20 names spot-checked, not all 120. |
| 14 | A live pool resize happens only with `UseFixedPoolSize=0`, and is re-read on every pending-state update. | CONFIRMED | `StreamingManagerTexture.cpp:1473-1499`, `:643-645` |
| 15 | Restoring `PoolSize=-1` can leave the squeezed budget in place (it keeps the current `GTexturePoolSize` unless the VRAM percentage and memory stat are positive). | CONFIRMED | `StreamingManagerTexture.cpp:1479-1488` |
| 16 | The global `r.Streaming.MipBias` is forced to 0 in any editor process. | CONFIRMED | `TextureStreamingHelpers.cpp:299`. Addition: `LimitPoolSizeToVRAM` is also forced off in editor (`:304`), so BLUR-2 behaves differently in PIE and packaged. |
| 17 | A cvar `Set` with lower priority is ignored, and every accepted `Set` overwrites the set-by bits. Setting with the saved priority needs no elevation. | CONFIRMED | `Core\Private\HAL\ConsoleManager.cpp:164-170` (`NewPri >= OldPri`), `:217`. Note: the release precedent elevates and does not restore set-by (`r.CustomDepth` set/restored with `ECVF_SetByCode`, `Source\AnomalyCapture\Private\AnomalyStencilTag.cpp:659`, `:670`). |
| 18 | Float cvars stringify with `%g` (round-trip loss risk). | CONFIRMED | `ConsoleManager.cpp:783-786` |
| 19 | In any editor process (PIE included), `r.VT.MaxUploadsPerFrame` is ignored and `r.VT.MaxUploadsPerFrameInEditor` (default 32) applies. | CONFIRMED | `Engine\Private\VT\VirtualTextureScalability.cpp:16-30`, `:227-235`. This matters because office capture runs in PIE. |
| 20 | Changing `r.VT.PoolSizeScale` releases and recreates every streaming VT and RVT resource. | CONFIRMED | `VirtualTextureScalability.cpp:155-199` |
| 21 | The "TEXTURE STREAMING POOL OVER … BUDGET" banner is a Canvas draw in `DrawMapWarnings`, gated by `GAreScreenMessagesEnabled` and `bSuppressMapWarnings`, present in Test; `AddOnScreenDebugMessage` is compiled out of Test. | CONFIRMED | `UnrealEngine.cpp:11723-11732`, `:11930-11942`, `:9537-9558`, `:10849` |
| 22 | The plugin's SVE colour grab precedes the HUD, PostRender and stats canvas, so the banner never reaches SVE-captured frames; the backbuffer path would carry it. | CONFIRMED | Grab at `EPostProcessingPass::VisualizeDepthOfField`: `Source\AnomalyCapture\Private\AnomalySceneViewExtension.cpp:149-152`. Canvas draws: `Engine\Private\GameViewportClient.cpp:1803`, `:1851`, `:1874`. Backbuffer hook: `Source\AnomalyCapture\Private\AnomalyFrameCapturer.cpp:191`. |
| 23 | A pool squeeze cannot be aimed: retention priority protects visible (+1024), non-huge (+512) and small textures, and the budget loop drops the lowest priority first. | CONFIRMED | `Engine\Private\Streaming\StreamingTexture.cpp:389-402`; `AsyncTextureStreaming.cpp:647-706` |
| 24 | `UTexture2D::GetNumResidentMips()` on a VT returns a nominal count, not page residency. | CONFIRMED | `Engine\Private\Texture2D.cpp:455-480` |
| 25 | VT textures ignore cinematic mips, so the BLUR-1 lever cannot blur a VT. | CONFIRMED | `Engine\Private\TextureLODSettings.cpp:171-175`. Release also refuses virtual textures (`Source\AnomalyInjector\Private\Anomalies\Anomaly_StuckLowMip.cpp:802`). |

## Q2 — probe APIs (MISS-1, MISS-2, BLUR-1, BLUR-2, GONE-1)

| # | Claim | Verdict | Evidence |
|---|---|---|---|
| 26 | The release render-thread residency observer reads `NumResidentLODs` and first mip per watched texture, keyed to the capture `RequestId` at arm. `GetState()` is render-thread-only. | CONFIRMED | `Source\AnomalyCapture\Private\AnomalySveCapturer.cpp:56-104`, `:106-117`; `Engine\Public\Rendering\StreamableTextureResource.h:38-40` |
| 27 | `FMaterialRenderProxy::GetMaterialWithFallback` is exported and render-thread. | CONFIRMED | `Engine\Public\MaterialShared.h:2538` (`class ENGINE_API`), `:2602` |
| 28 | (added; not in the review) `UMaterialInterface::CheckMaterialUsage_Concurrent` is a public virtual, giving a game-thread pre-arm predictor for the usage-flag fallback specifically. | CONFIRMED | `Engine\Classes\Materials\MaterialInterface.h:486` (public section from `:317`) |
| 29 | The MID texture setter queues a render-thread parameter update (asynchronous). | CONFIRMED | `MaterialInstance.cpp:3430-3432` |
| 30 | `RegisterMipLevelChangeCallback` is exported; it is a game-thread threshold callback, not a per-frame probe. | CONFIRMED | `Engine\Classes\Engine\StreamableRenderAsset.h:137` |
| 31 | `WasRecentlyRendered` is a world-time window over `LastRenderTime`. `LastRenderTimeOnScreen` updates only for "definitely unoccluded" primitives. | CONFIRMED | `Engine\Private\Components\PrimitiveComponent.cpp:4229-4238`; `Renderer\Private\SceneVisibility.cpp:2491-2493`. Additions: shadow passes also bump `LastRenderTime` (`Renderer\Private\ShadowSetup.cpp:1672`, `:1909`), and the value is written cross-thread into the component (`Renderer\Private\PrimitiveSceneInfo.cpp:2137-2141`). It is a time stamp, not a frame key. |
| 32 | No public API gives an exact per-object, per-view, per-frame draw/binding receipt. | UNVERIFIABLE | A negative claim. Spot checks agree: the visibility maps live on the private `FViewInfo`; the public getters above report state, not draws. |

## GONE-1 and Q9 — cull distance, masks, visibility layer

| # | Claim | Verdict | Evidence |
|---|---|---|---|
| 33 | `SetCullDistance` and `SetCachedMaxDrawDistance` are public; the latter does `MarkRenderStateDirty`, a full proxy recreate. | CONFIRMED | `Engine\Classes\Components\PrimitiveComponent.h:2533`, `:2567`, `:2572`; `PrimitiveComponent.cpp:1818-1827` |
| 34 | `bNeverDistanceCull` or an HLOD parent forces "no cull", making GONE-1 a no-op. | CONFIRMED | `PrimitiveComponent.cpp:1820-1821` |
| 35 | The restore must also put back `CachedMaxDrawDistance`: `SetCullDistance(savedLD)` alone can wipe a cull-distance-volume value. | CONFIRMED | `PrimitiveComponent.cpp:1802-1805` (`LD < Cached` ⇒ cached set to LD, e.g. 0) |
| 36 | A cull distance of 0 means unlimited. | CONFIRMED | `Engine\Private\PrimitiveSceneProxy.cpp:299` |
| 37 | The cull compares the distance to the **bounds origin** with `MaxCullDistance × scale`, and the review's scale is `ViewDistanceScale`. | **CORRECTED** (scale) | Origin distance confirmed: `SceneVisibility.cpp:825-848`. But the primitive-cull path's scale is `ViewDistanceScale × CalculateFieldOfViewDistanceScale(View.DesiredFOV)` (`SceneVisibility.cpp:937`; `Engine\Public\UnrealEngine.h:430-438`). The FOV factor is 1 unless the host configures differing `r.ViewDistanceScale.FieldOfView*` scales. `ViewDistanceScale` also folds in the secondary scale (`UnrealEngine.cpp:757-761`). |
| 38 | "For a guaranteed non-fading step choose `C×scale + fadeRadius < cameraDistance`." | **CORRECTED** | The distance-cull fade applies only to proxies that report `IsUsingDistanceCullFade()` (default false; for a static mesh, only when its material uses DistanceCullFade): `Engine\Public\PrimitiveSceneProxy.h:461-464`; `StaticMeshRender.cpp:1924-1927`; `SceneVisibility.cpp:834-838`, `:983-995`. For every other material the cull is an instant one-frame pop. The 1,000 cm margin (`r.DistanceFadeMaxTravel`, `SceneVisibility.cpp:239-240`) is needed only for DistanceCullFade materials. Applied universally, it would make every target nearer than ~10 m ineligible, since `C×scale + 1000 < d` is impossible when `d < 1000` cm. That is most targets under the 1,800 cm poll radius. |
| 39 | A distance- or frustum-culled primitive writes no custom depth/stencil: relevance and CustomDepth commands are built only from `PrimitiveVisibilityMap`. | CONFIRMED | `SceneVisibility.cpp:846-865`, `:887-888`, `:3027`, `:2727-2729` |
| 40 | The plugin's visibility layer does not depend on "was rendered" (bounds frustum + 9 line traces + `IsVisible()`/type/exclusions). | CONFIRMED | `Source\AnomalyInjector\Private\AnomalyViewport.cpp:228-305`, `:794-829`. GONE-1 leaves it saying "should be visible". |
| 41 | A static mesh with custom depth on cannot be occlusion-culled (it is marked definitely unoccluded), so plugin tagging defeats GONE-2. | CONFIRMED | `StaticMeshRender.cpp:1919-1922`; `SceneVisibility.cpp:1279-1284` |
| 42 | m26 would delete a GONE-1 event: the pass-ran test is view-level, and a culled target with other tags live reads `MeasuredZero`, which vetoes. | CONFIRMED | `Source\AnomalyCapture\Private\AnomalyMaskMeasure.cpp:702`, `:797`; `Source\AnomalyCapture\Private\AnomalyCaptureSubsystem.cpp:8749-8751`, `:8980-8986` |
| 43 | The census excludes a measured-zero entry from selection. | CONFIRMED | `Source\AnomalyCapture\Private\AnomalyCensus.cpp:326-332` |
| 44 | `bIgnoreInstanceForTextureStreaming` exists and stops a component's streaming demand. | CONFIRMED | `Engine\Classes\Components\StaticMeshComponent.h:321`; `StaticMeshComponent.cpp:1083` |
| 45 | The proposed equivalent-reference-surface (companion component) mask route works. | UNVERIFIABLE | A design; not runnable read-only. Its public flag setters exist (rows 33, 44; `AnomalyHiddenClass.cpp:101-127`). Equivalence needs a bench proof. |

## Q3 and other catalog claims

| # | Claim | Verdict | Evidence |
|---|---|---|---|
| 46 | Release `stuck_low_mip` hold = `NumCinematicMipLevels` + `UpdateCachedLODBias` on the original texture, refused when shared (`shared_world`); route `auto` sends shared textures to a private **reduced-mip copy** (not "cached bias on a private copy", as the draft says). | CONFIRMED | `Anomaly_StuckLowMip.cpp:732-779`, `:845-871`, `:950-985`; `Source\AnomalyInjector\Public\AnomalyProxyBlurPolicy.h:9-12`; dropped-mip copy `Source\AnomalyInjector\Private\Anomalies\Anomaly_TexCorrupt.cpp:922-925` |
| 47 | Blinking mode 0 hides with `SetActorHiddenInGame(true)` and `Show` sets false unconditionally; mode 1 drops main/depth/shadow/GI/RT/decals and keeps custom depth. | CONFIRMED | `Source\AnomalyInjector\Private\AnomalyHiddenClass.cpp:81-84`, `:143-146`, `:101-127`, `:163-171` |
| 48 | `time_dilation` and `lighting_mismatch` are registered in the release build. | CONFIRMED | `Source\AnomalyInjector\Private\AnomalyInjectorSubsystem.cpp:254-255` |
| 49 | (draft) "Magenta is other engines' convention, not Unreal's" is wrong for VT: UE 5.1 highlights VT decode errors in hot pink, on by default. | CONFIRMED (the review's correction of the draft) | `Engine\Private\VT\VirtualTextureTranscodeCache.cpp:20-38` |
| 50 | The shader-pending counter counts only the plugin's own swap materials. | CONFIRMED | `AnomalyCaptureSubsystem.cpp:3033-3046`, `:5382-5387` |
| 51 | `r.MeshStreaming` defaults to 0 and is read-only (LOD-3). | CONFIRMED | `Engine\Private\ContentStreaming.cpp:35-40` |
| 52 | `r.SetNearClipPlane` clamps to ≥1 cm and flushes rendering (CAM-1 restore). | CONFIRMED | `UnrealEngine.cpp:16958-16968` |
| 53 | An ordinary texture bound into a compiled VT slot is skipped with a warning (MISS-2/NRM-3 on VT slots). | CONFIRMED | `MaterialShared.cpp:3711-3725` |

## Where I disagree with the review, and why

1. **PIE and the usage-flag fallback (Q4a, row 3).** The review says editor repair includes ordinary PIE operation. The source says the opposite for checks made while PIE ticks or draws: `FApp::IsGame()` is true there, so UE warns and uses the default material, as in a cooked build. Two consequences:
   - The draft's worry that "PIE hides the fallback" is mostly unfounded.
   - A plugin-owned flagless material would show the engine's *real* fallback path in PIE and in cooked builds. That is closer to a genuine MISS-1 trigger than the review allows. The cost is a residual risk that a check made outside PIE scope repairs and dirties that plugin asset.

   I still agree MISS-1 should ship first as the review's R1 direct-default calibration.
2. **NRM-3 asset (row 11).** The flat normal is `/Engine/EngineMaterials/FlatNormal`, not `DefaultNormal`, and it is not a startup package. NRM-3 needs `FlatNormal` with an explicit cook reference.
3. **GONE-1 aiming rule (rows 37–38).**
   - The review's universal fade margin is unnecessary for ordinary materials, which pop instantly. Applied as written, it would make most targets inside the 18 m poll radius ineligible, wrongly lowering GONE-1's yield.
   - The scale needs the FOV factor as well as `ViewDistanceScale`.
   - The review's other GONE-1 points stand: the restore order, the never-cull/HLOD exclusions, the zero-mask/m26 veto, and the need for a reference mask.
4. **Smaller additions, no disagreement:**
   - `CheckMaterialUsage_Concurrent` is a usable game-thread predictor for the usage-flag fallback (row 28).
   - In PIE, `LimitPoolSizeToVRAM` is also forced off (row 16), so a BLUR-2 study would differ further between PIE and packaged.
   - Plugin custom-depth tagging also forces the tagged object "definitely unoccluded", which feeds `LastRenderTimeOnScreen` (rows 31, 41). Tagged targets would therefore look "rendered on screen" to a recent-render probe whenever they are in the frustum.
5. **Naming convention (not technical).** The review names the two office hosts by their codenames, following the relay rules block, where this brief uses "first/second office host". It also cites a 092-01 review filename that carries the retired second codename. No game title appears. I did not edit the review.
