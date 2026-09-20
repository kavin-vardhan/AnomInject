# m52 — `stuck_low_mip` — PRE-DECLARED DESIGN AND GATES

**Written 2026-09-20, session 080-01, branch `feat/m52-stuck-mip` off `master` `3ff88db`.**
**PLAN ONLY. No plugin source changed, no build, no cook, no capture leg, no tag.**
The only executable artifact this session produced is a read-only offline scanner,
`tools/m52_texture_sharing_scan.py`, which opens no engine and touches no binary.

Engine read against the canonical engine: `D:\UESource\UnrealEngine`, **UE 5.1.1,
`++UE5+Release-5.1`** (`Engine/Build/Build.version`). Every engine claim below carries
`file:line`. Every plugin claim carries `file:line` against `master` `3ff88db`.

---

## 0. What this file is for

`m52` adds a ninth-plus anomaly, **`stuck_low_mip`** (texture class): a target actor's
textures are held at a LOW resident mip for the event span — the object looks blurry
while everything else stays sharp — then restored. Owner priority for the next client
release; `m53` (flicker) and `m54` (UV corruption) follow.

This file fixes, **before any implementation**, (a) which lever is used and why the
others are refused, (b) the sharing policy and the measurements behind it, (c) what
per-frame evidence ships, (d) the perceptibility rule, and (e) the gate list with its
pre-declared readings. Anything measured later that contradicts this file is a
**finding**, and the finding wins — but it must be recorded as a correction, not
silently folded in.

---

## 1. The lever — four candidates evaluated at source, one chosen

### 1.0 The two engine facts everything else hangs off

**(i) The streamer decides the resident mip count every frame, and it decides it from
`MaxAllowedMips`.**

```
FStreamingRenderAsset::UpdateDynamicData   StreamingTexture.cpp:170-247
    LODBias        = max(0, RenderAsset->GetCachedLODBias() - AssetLODBias)   :188-192
                     (skipped entirely if Settings.bUseAllMips)               :189
    MaxAllowedMips = Clamp(MaxNumLODs - LODBias, NumNonStreamingLODs, ...)    :229, :233
```

and **every** wanted-mips path is then clamped to `MaxAllowedMips`:

```
GetWantedMipsFromSize   ... Clamp(WantedMipsInt, MinAllowedMips, MaxAllowedMips)   StreamingTexture.cpp:310
SetPerfectWantedMips_Async
    if (MaxNumForcedLODs >= MaxAllowedMips) Visible = Hidden = Forced = MaxAllowedMips   :346-348
    VisibleWantedMips = max(GetWantedMipsFromSize(...), NumForcedMips)                   :361
    HiddenWantedMips  = max(GetWantedMipsFromSize(...), NumForcedMips)                   :366
```

⇒ **a per-texture reduction of `MaxAllowedMips` binds unconditionally, including under
force-fully-load.** This is the guarantee that satisfies requirement (b).

**(ii) The streamer actively CANCELS a stream request it did not issue.**

```
FRenderAssetStreamingMipCalcTask::UpdateLoadAndCancelationRequests_Async   AsyncTextureStreaming.cpp:778
    else if (RequestedMips > max(ResidentMips, WantedMips + 1) ||
             RequestedMips < min(ResidentMips, WantedMips))
    {  CancelationRequests.Add(AssetIndex);  }                             AsyncTextureStreaming.cpp:803-808
```
executed on the game thread at `StreamingManagerTexture.cpp:1333-1339` →
`FStreamingRenderAsset::CancelStreamingRequest` (`StreamingTexture.cpp:536-543`) →
`UStreamableRenderAsset::CancelPendingStreamingRequest` → `PendingUpdate->Abort()`
(`StreamableRenderAsset.cpp:221-227`).

A manual `StreamOut(floor)` on a **visible** texture sets `RequestedMips = floor` while
`WantedMips` is high, so `RequestedMips < min(ResidentMips, WantedMips)` is TRUE ⇒ the
request is put up for cancellation. ⇒ **a foreign stream-out is not merely undone
afterwards; it is raced for cancellation before it completes.**

### 1.1 `L1` — bare `StreamOut(NewMipCount)` — **REFUSED**

`UTexture2D::StreamOut` (`Texture2D.cpp:1687-1705`, declared
`StreamableRenderAsset.h:85-89`) is game-thread-only (`check(IsInGameThread())`,
`Texture2D.cpp:1689`), requires `!HasPendingInitOrStreaming()` (`:1692`), and clamps the
request to `NumNonStreamingLODs` (`StreamableRenderResourceState.h:123-128`).

It fails requirement (b) **twice**: by §1.0(ii) it can be cancelled before it lands, and
even if it lands, `FStreamingRenderAsset::StreamWantedMips_Internal` streams it straight
back in (`StreamingTexture.cpp:562-595`, the `StreamIn` at `:589`).

**Kept as a mechanism, not as the lever:** `StreamIn(baseline, bHighPrio=true)` is used
on the RESTORE side, where the streamer wants the same thing we do, so no race exists.

### 1.2 `L2` — `StreamOut` then `UnlinkStreaming()` — **REFUSED as primary, retained as a named fallback**

`LinkStreaming`/`UnlinkStreaming` are `ENGINE_API` (`StreamableRenderAsset.h:186-188`,
impl `StreamableRenderAsset.cpp:273-316`) and an unlinked asset is genuinely untouchable
by the streamer (`RemoveStreamingRenderAsset`, `StreamingManagerTexture.cpp:832-862`).

**It cannot be ordered correctly.**
- *Unlink first, then StreamOut*: the only thing that advances a pending update is
  `FStreamingRenderAsset::UpdateStreamingStatus` → `RenderAsset->TickStreaming(...)`
  (`StreamingTexture.cpp:262-278`, the call at `:268`). An unlinked asset is not in the
  manager's array, so nothing ticks it and the stream-out **never completes**. We cannot
  tick it ourselves: `UStreamableRenderAsset::TickStreaming` is **not** `ENGINE_API`
  (`StreamableRenderAsset.h:201`) — a plugin call link-fails on the modular editor
  target. `WaitForStreaming()` *is* exported (`:199`) but blocks the game thread, i.e. it
  buys the hitch requirement (a) forbids.
- *StreamOut first, then unlink*: phase one is exactly the cancellation race of
  §1.0(ii).

⇒ **`UnlinkStreaming` is retained only as an optional post-hold LOCK** (§1.5), behind a
knob, default OFF, to be enabled only if a fixture measures the streamer winning.

### 1.3 `L3` — `LODBias` / `MaxTextureSize` + `UpdateResource()` — **REFUSED, and one half of it is a trap**

Two separate refusals.

**(a) `UpdateResource()` is refused on cost and blast radius** — it recreates the RHI
resource for every user of the texture. Not measured; refused on shape, because §1.4
achieves the same end with no resource recreation at all. The engine's own
editor-side "hold the current mips" helper, `UTexture2D::TemporarilyDisableStreaming()`,
is **`#if WITH_EDITOR` only** (`Texture2D.cpp:1178-1187`) and therefore does not exist in
a packaged game — worth stating so nobody reaches for it.

**(b) 🚨 `UTexture::LODBias` IS IGNORED ON A COOKED PLATFORM. This is the most important
negative finding in this file.**

`UTexture::LODBias` is a runtime, public, `BlueprintReadWrite` `UPROPERTY`
(`Texture.h:1292-1294`, explicitly under the *"Properties needed at runtime below"*
banner at `Texture.h:1277-1279`), and `UpdateCachedLODBias()` is `ENGINE_API`
(`Texture.h:1453`) and is **not** inside a `WITH_EDITOR` guard (`TextureDerivedData.cpp:2765-2768`;
the preceding guard closes at `:2761` and the next opens at `:2770`). So
`Texture->LODBias = N; Texture->UpdateCachedLODBias();` compiles and runs in a packaged
build — and does **nothing**:

```
UTextureLODSettings::CalculateLODBias   TextureLODSettings.cpp:146
    if (!FPlatformProperties::RequiresCookedData())
    {
        // When cooking, LODBias and LODGroupInfo.LODBias are taken into account to strip the top mips.
        // Considering them again here would apply them twice.
        UsedLODBias += LODBias;                                  TextureLODSettings.cpp:176-186
```

⇒ **it works in the editor and is a silent no-op in the configuration the client
receives.** That is `G119`'s shape (the source value is an input, not the artifact) and
`G114`'s shape (a lever that does nothing produces a clean null indistinguishable from a
clean result). **`Texture->LODBias` must never be used as the m52 lever, and the m52
gates must be read from a PACKAGED leg, not from PIE (`G76`).**

### 1.4 `L4′` — **CHOSEN**: per-texture streaming bias via `NumCinematicMipLevels` + `UpdateCachedLODBias()`

This is not one of the four the brief enumerated. It is what the source read produced,
and it is chosen because it is the only candidate that satisfies (a)–(d) without
fighting the streamer.

**The mechanism.** `NumCinematicMipLevels` is a **public, runtime `UPROPERTY`** on
`UStreamableRenderAsset` (`StreamableRenderAsset.h:253-256` — note the `public:` at
`:253`). `UpdateCachedLODBias()` calls `CalculateLODBias(this, bIncCinematicMips = true)`
(default argument, `TextureLODSettings.h:154`), and that term is added **outside** the
cooked gate:

```
UTextureLODSettings::CalculateLODBias
    if (!bVirtualTexture) { UsedLODBias += NumCinematicMipLevels; }      TextureLODSettings.cpp:171-175   <-- NOT cooked-gated
    if (!FPlatformProperties::RequiresCookedData()) { UsedLODBias += LODBias; ... }   :176-186   <-- cooked-gated
    WantedMaxLOD = Clamp(TextureMaxLOD - UsedLODBias, MinLOD, MaxLOD)    :203-205
```

The result lands in `CachedCombinedLODBias`, which §1.0(i) shows reduces
`MaxAllowedMips` on **every** streamer update, and §1.0(i) shows every wanted-mips path
is clamped by it.

**So:**
- **The streamer itself performs the stream-out**, via its own
  `StreamWantedMips_Internal` (`StreamingTexture.cpp:584`). `RequestedMips == WantedMips`
  by construction, so the cancellation predicate of §1.0(ii) is FALSE and nothing races
  us. *The streamer stops being the adversary and becomes the enforcer.*
- **No RHI resource recreation, no `UpdateResource()`, no blocking wait.** The work is
  `FTexture2DStreamOut_AsyncReallocate`, which is entirely render-thread
  (`PushTask(..., TT_Render, ...)`, `Texture2DStreamOut_AsyncReallocate.cpp:22-41`) and
  is the identical path the engine runs for every texture the camera walks away from.
- **Revert is exact and symmetric**: restore the saved `NumCinematicMipLevels`, call
  `UpdateCachedLODBias()` again, and (belt) `StreamIn(baseline, bHighPrio=true)` — a
  request the streamer *agrees with*, so §1.0(ii) cannot cancel it in the harmful
  direction.
- **No global cvar, no streaming-pool change, no per-group setting** — requirement (d).

**The achievable depth of blur.** The floor is `NumNonStreamingLODs`
(`StreamableRenderResourceState.h:126-128`; `MinAllowedMips` at `StreamingTexture.cpp:245`),
which for a cooked streaming texture is driven by
`GMinTextureResidentMipCount = NUM_INLINE_DERIVED_MIPS = 7`
(`TextureDerivedData.cpp:3535`, `TextureDerivedDataTask.h:31`). Seven resident mips means
the largest resident mip is **64×64** — a 32× reduction for a 2048² texture. The group
clamp does not get in the way on the default `TEXTUREGROUP_World`, whose `MinLODSize=1`
(`BaseDeviceProfiles.ini:126`) makes `MinLOD = CeilLogTwo(1) = 0` at
`TextureLODSettings.cpp:193`, so the full bias range is available.
⛔ **The implementation must READ the floor from
`GetStreamableResourceState().NumNonStreamingLODs`, never assume 7** — the constant is
the *expected* value, and the gate reports the measured one.

### 1.5 Named failure modes of the chosen lever — all detectable, none silent

| # | Condition | Effect | How we see it |
|---|---|---|---|
| F-a | `r.Streaming.UseAllMips != 0` (default **0**, `TextureStreamingHelpers.cpp:194`) | `LODBias` term skipped entirely (`StreamingTexture.cpp:189`) ⇒ hold never engages | read the cvar back at `Apply`; `held:false` on every frame |
| F-b | `r.TextureStreaming = 0` (default **1**, `TextureStreamingHelpers.cpp:93`) | nothing is streamable; `LinkStreaming` unlinks (`StreamableRenderAsset.cpp:277`) ⇒ `IsStreamable()` false | `NOT_APPLICABLE` at pick time, counted |
| F-c | host calls `SetForceMipLevelsToBeResident(secs, mask)` with a mask covering the texture's LOD group | sets `bUseCinematicMipLevels` (`StreamableRenderAsset.cpp:205-219`); combined with `bForceFullyLoad` the streamer **subtracts our bias back out** (`StreamingTexture.cpp:186, 192`) | `held:false`; `frames_condition_lost` rises |
| F-d | host calls `UpdateResource()` on the texture mid-span | re-links and re-creates the resource ⇒ hold breaks | `held:false` |
| F-e | texture is virtual (`IsCurrentlyVirtualTextured()`) | not in the render-asset streamer at all; `GetNumResidentMips()` returns a VT constant (`Texture2D.cpp:466-479`) | `NOT_APPLICABLE` at pick time, counted |

**None of F-a…F-e is assumed away.** Every one is either refused at pick time and
counted, or shows up as `held:false` in the per-frame evidence — which, because the
label is driven by the same measurement (§4), means **those frames are not labelled
positive**. That is the whole design: the anomaly cannot claim a frame it did not change.

---

## 2. Ordering — why the evidence is sampled at `OnWorldTickEnd`

Within one engine frame:

```
UWorld::Tick           ... FWorldDelegates::OnWorldTickEnd.Broadcast   LevelTick.cpp:1814
UGameEngine::Tick      ... RedrawViewports();                          GameEngine.cpp:1891   <-- the frame is DRAWN here
                       // "Update resource streaming after viewports have had a chance to update view information."
                       ... IStreamingManager::Get().Tick(DeltaSeconds); GameEngine.cpp:1899-1901
```

⇒ **a `GetNumResidentMips()` sample taken at `OnWorldTickEnd` describes the state the
SAME frame's `RedrawViewports()` will draw with.** That is `m40`'s rule holding for this
quantity, and it is why the m52 evidence rides the existing tick-end sampler
(`AnomalyCaptureSubsystem.cpp:4600-4625`) rather than a new hook.

⇒ **Corollary, and it is a prediction:** because the streamer ticks *after* the draw, a
bias set during `UWorld::Tick` cannot take effect in that frame. **Onset is latent by at
least one captured frame and probably several.** The latency is measured, not assumed
(gate `G4`).

---

## 3. The sharing hazard

A texture is an **asset**. Holding it low blurs every component that samples it, while
the label names one target. An unlabelled blurry object in a labelled frame is not a
missing label — it teaches the model *"blurry is normal"*, which is worse.

### 3.1 What was measured, and what it does not measure

`tools/m52_texture_sharing_scan.py` (read-only, offline, no engine) builds a
`mesh → material → texture` reference graph from the name tables of every `.uasset`
under a content root, and reports, per texture, how many **other meshes** reach it.
Target names are not invented: they are the **measured** target asset set harvested from
`affected_objects.nodes[].asset_name` across the banked sessions
(`_bench_sessions_bank`, 393 StackOBot `annotation.json` + all 6 Lyra ones).

⛔ **Declared weaknesses, which travel with every number below** (the
`nanite_signature_scan.py` precedent — EVIDENCE, not a measurement):
1. **LEVEL-WIDE, NOT VIEW-WIDE.** It counts assets anywhere in the project. The question
   the policy actually needs — *how many other actors VISIBLE IN THIS FRAME* — needs a
   runtime probe. These numbers are therefore an **upper bound on sharing** and a **lower
   bound on exclusivity**.
2. **ASSET-WIDE, NOT INSTANCE-WIDE.** One shared mesh placed 50 times counts once.
3. Runtime-created dynamic material instances are invisible to it.
4. A reference is not proof the texture is sampled by the shader that draws the target.
5. Head-truncation: StackOBot 421 of 3,974 files, Lyra 1,237 of 4,688 truncated at
   192 KB; a truncated tail loses references, which biases sharing **downward**.
Classifier both-ways control (`G96`): of the assets carrying the `Texture2D` token, the
property conjunction **accepted** 647 and **rejected** 386 on StackOBot (803 / 510 on
Lyra) — so the classifier discriminates rather than accepting everything.

### 3.2 StackOBot — 18 unique measured target assets

*"sharers" below = other MESH assets reaching the texture; the target's own master
material and material instances are excluded by the 1-hop/2-hop split.*

| target | #tex | textures exclusive | least-shared texture |
|---|---|---|---|
| `SM_rock_02` | 4 | **3** | 0 |
| `SM_GratIng` | 10 | **3** | 0 |
| `SKM_Bot` | 6 | **2** | 0 |
| `SM_rock` | 4 | 0 | 1 |
| `SM_Bush` | 4 | 0 | 1 |
| `SM_GenericPlane` | 1 | 0 | 1 |
| `SM_Tree` | 7 | 0 | 1 |
| `SM_RockFlats_01` | 3 | 0 | 2 |
| `SM_Ramp`, `SM_SpawnPad_Base`, `SM_Modules_Platform`, `SM_Elevator`, `SM_Fan_Frame`, `SM_Modular_WallDoor` | 9 each | 0 | **17** |
| `SM_PressurePlate_Frame` | 5 | 0 | **37** |
| `SM_SlopeWarpLandscape` | 2 | 0 | **25** |
| `Cube`, `Cylinder` (`/Engine/BasicShapes/*`, as ASSETS) | 2 | 0 | **25** |

- **ALL textures exclusive: 0 of 18 (0 %).**
- **At least one exclusive texture: 3 of 18 (17 %).**
- **Least-shared texture ≤ 1 other mesh: 7 of 18 (39 %).**
- The heavy sharers are **utility/tiling** textures — `BaseFlattenNormalMap` = 84,
  `127grey` = 74, `T_Grunge_A` = 58, `T_WhitePixel` / `T_Metal_Painted_N` = 37,
  `T_ConcreteTileable_*` = 34. Six modular targets sit on **one identical 9-texture
  family**: blurring any of them blurs all six plus 17–84 other meshes.

🚨 **`CB_GateLevel` CANNOT HOST THIS ANOMALY AT ALL — AND THE REASON IS STRONGER THAN
SHARING: ITS TARGETS HAVE NO TEXTURE TO HOLD.** The row above is about the `BasicShapes`
mesh *assets*, whose default material slot reaches the grid textures. **The gate level
overrides that.** `make_gate_level.py:60` loads `/Engine/BasicShapes/BasicShapeMaterial`
and `:68-69` assigns it to slot 0 of **every** target component — and that material
references **zero textures**: the asset (9,557 B) contains no `Texture2D` token and no
outbound object reference but its own path. The script says so itself at `:75-76`:
*"the targets use the plain white BasicShapeMaterial"*. Only the non-target **floor**
gets `WorldGridMaterial` (`:79-81`).

⇒ on `CB_GateLevel`, `GetUsedTextures` returns **nothing eligible** for every target, and
`stuck_low_mip` refuses 100 % of the fixture with reason `not_streamable` **before the
sharing gate is ever consulted**. This is `G135` again — a calibration environment built
from a restricted asset set cannot exhibit defect classes outside that set, and the
blindness presents as a clean pass. It is not a defect in the fixture; it is a limit of
it, and it decides §6's gate plan.

### 3.3 Lyra — 5 unique resolved target assets

| target | #tex | textures exclusive | least-shared texture |
|---|---|---|---|
| `SKM_Manny` | 16 | **6** | 0 |
| `SM_Rifle` | 4 | 0 | 1 |
| `SM_Pistol` | 4 | 0 | 1 |
| `SM_Shotgun` | 4 | 0 | 1 |
| `Cube` | 2 | 0 | **34** |

- **At least one exclusive texture: 1 of 5 (20 %).**
- **Least-shared ≤ 1: 4 of 5 (80 %).**
- The weapons' four textures each have **exactly one** other mesh reaching them —
  almost certainly the weapon's own variant mesh, which may or may not be
  simultaneously visible. Offline cannot tell; stated, not guessed.
- ⚠ `MeshA_53AD96` and its five siblings (the `L_Convolution_Blockout` targets `LG-9`
  fires at) **were not found in the content tree** — they are generated/merged blockout
  meshes. Six of Lyra's measured targets are therefore **unmeasured** by this scan, and
  that is stated rather than papered over.

**The differential is the result:** Lyra's authored content is near-exclusive
(4 of 5 at ≤ 1 sharer); StackOBot's modular kit is heavily shared (6 targets on one
17–84-way family); and the bench gate level is maximally shared. A policy calibrated on
StackOBot alone would be calibrated on the worst case in the project.

### 3.4 The policy — **S4, per-TEXTURE gating on the VISIBLE set** (recommended)

The brief's three options are each refused by the measurement:

- **S1 (restrict eligibility to targets whose texture has no other visible user)** —
  refused *as stated* because it is a **per-target** filter: on StackOBot it admits at
  most 3 of 18 targets level-wide, and 0 on the bench fixture. It throws away the target
  rather than the offending texture.
- **S2 (label every visible user as an affected target in the same event)** — the
  schema would take it (`affected_objects.nodes[]` is already an array carrying
  `asset_name` / `component_class` / `bounds` since `m22`), but the *machinery* would
  not: one stencil tag is allocated per event (`m50`'s `FAnomalyStencilTagLedger`), the
  ownership rule is *"an actor under a live fire belongs to its event"* (`m44`), and
  `target_pixels` / `bbox_drawn_px` / the `m26` veto are all one-target quantities.
  Multi-target events are a real architectural change with their own gate set.
  **Filed as the Tier-2 fallback, not built.**
- **S3 (exclude shared textures entirely)** — identical outcome to S1, and worse: on
  StackOBot it excludes every texture of 15 of 18 targets.

**S4, recommended.** Gate **per texture**, not per target, and gate on the **visible**
set rather than the asset graph:

1. At pick time, enumerate the target component's textures via
   `UPrimitiveComponent::GetUsedTextures(Out, EMaterialQualityLevel::Num)`
   (`PrimitiveComponent.h:1952`, impl `.cpp:548-571` — virtual, so no export needed;
   it is what the engine's own streamer uses at `StreamingManagerTexture.cpp:712`).
2. Keep only textures that are `UTexture2D`, `IsStreamable()`,
   `RenderResourceSupportsStreaming()`, **not** `IsCurrentlyVirtualTextured()`, **not**
   `NeverStream`, and not in an excluded `LODGroup`
   (`TEXTUREGROUP_UI`, `_Lightmap`, `_Shadowmap`, `_Terrain_Heightmap`,
   `_Terrain_Weightmap`, `_Bokeh`).
3. For each surviving texture, count **other currently-visible primitive components**
   that use it, reusing the renderable-visible set
   `AnomalyViewport::GetVisibleRenderableActors` already computes (`G33` chokepoint).
4. **Hold exactly those textures whose visible co-user count `<= IAI.Anomaly.StuckMipMaxCoAffected`
   (compiled default `0`).**
5. If the held set is empty, or fails the perceptibility rule (§5), **REFUSE the target**
   through the `AMB-2` matched-zero path — `Apply` returns false, no fire, no label —
   with a per-reason counter and a loud `REFUSED` line naming the reason in dataset
   terms. This is `lod_popping`'s exact shape (`Anomaly_LodPopping.cpp:100-103`,
   `:116-118`).
6. As with `lod_popping`, **the gates apply on the AUTO-POOL path only**
   (`Anomaly_LodPopping.cpp:82-85`), so a targeted fire can deliberately produce a shared
   case for the can-fail lever.

**Why this beats S1:** on `SM_rock` it holds `T_rock_01_D/N/AORM` (1 asset-wide sharer
each) and leaves `T_black` (39 sharers) alone — the rock goes blurry, the shared utility
texture does not. S1 would have discarded the whole target.

⚠ **The cost is real and is declared in advance, not discovered:** under
`MaxCoAffected = 0` the six-target StackOBot modular family is expected to be **refused**
whenever any sibling is on screen. **No yield number is predicted** — the level-wide
scan bounds it from above and cannot produce it. Measuring the auto-pool yield on both
fixtures is gate `G7`, and if it is too thin, the knob is the owner's dial and Tier-2
(S2) is the escalation. ⛔ **No threshold is invented here beyond the conservative `0`.**

---

## 4. Evidence — the m49 bar

### E1 — engine fact per frame (the part the verifier can never give)

Sampled at `OnWorldTickEnd` (§2), written into each `labels.jsonl` **anomaly entry** for
the span (the object at `AnomalyLabelWriter.cpp:72-130`), additive, and emitted **only on
a `stuck_low_mip` entry** so a run without this anomaly gains no key (the `m47`/`m48`
additive-key precedent):

| key | type | meaning |
|---|---|---|
| `stuck_mip.resident_mips` | int | `UTexture2D::GetNumResidentMips()` (`Texture2D.h:141`, impl `Texture2D.cpp:455-484`, which returns `CachedSRRState.NumResidentLODs`) for the **primary** held texture this frame |
| `stuck_mip.baseline_mips` | int | resident mip count captured immediately before `Apply` |
| `stuck_mip.full_mips` | int | `GetNumMips()` (`Texture2D.h:149`) |
| `stuck_mip.floor_mips` | int | `GetStreamableResourceState().NumNonStreamingLODs` — the measured floor, never the assumed 7 |
| `stuck_mip.top_resident_px` | int | `GetSizeX() >> (full_mips - resident_mips)` — the actual top resident mip width |
| `stuck_mip.held` | bool | `resident_mips < baseline_mips` **measured this frame** |
| `stuck_mip.textures_held` | int | how many of the target's textures are in the held set |
| `stuck_mip.co_affected_visible` | int | other visible components using the held set, re-counted this frame |
| `stuck_mip.texture` | string | the primary held texture's object name |

`run_summary` gains `stuck_mip_refused_shared`, `stuck_mip_refused_not_streamable`,
`stuck_mip_refused_virtual`, `stuck_mip_refused_imperceptible`, `stuck_mip_frames_held`.
⛔ **`annotation.json`'s field set must not move** (`P6`) — the per-frame keys live in
`labels.jsonl` only; the pre-declared `P-C7 v3` reading is *added = exactly the five
`run_summary` keys, removed = 0, `annotation.json` diff EMPTY*.

### E2 — visibility: the m49 A1 rule applies unchanged, **confirmed at source**

```
const bool bObservable = bLabelled && bHeld && Px >= ObservableMinPixels;   AnomalyCaptureSubsystem.cpp:3906
if (Px == GTargetPixelsUnmeasured) -> Observable::Unmeasured                                     :3900-3903
```

`stuck_low_mip` does not touch the silhouette, so `target_pixels` is measured by the
existing `m26`/`m43` target mask exactly as for `missing_texture` and
`corrupted_texture`. **No change to the observability path is required.**

The two hooks are wired as follows, and this is the load-bearing decision:

- Register `stuck_low_mip` as **`EAnomalyActiveSource::AnomalyState`** in
  `ResolveAnomalyActiveSource`'s map (`AnomalyCaptureSubsystem.cpp:278-296`) — the
  `lod_popping` slot, not the `missing_texture` slot.
- `IsCurrentlyAnomalous()` returns **`bActive && the mip is measurably below baseline`**,
  mirroring `Anomaly_LodPopping.h:20` (`bActive && bPoppedPhase`).
- `IsVisualConditionHeld()` inherits it (the `IAnomaly.h:28` default).

🔑 **Consequence, and it is the point: the frames between `Apply` and the mip actually
dropping are NOT labelled.** `injected_frames` starts when the pixels start changing,
so `m44`'s ONSET gate — first-mask frame == first-label frame == first-differing-picture
frame — is satisfiable **by construction** rather than by hoping the latency is zero.
The latency itself is not lost: it is visible in `stuck_mip.resident_mips` on the
unlabelled frames and in the run log.

### E3 — perceptibility floor

A low mip only *looks* blurry if the object's on-screen footprint out-resolves the mip.
Without this, `stuck_low_mip` ships "injected but invisible" labels — the client's `F2`
complaint.

**Rule, pre-declared:** let `P = max(bbox_px.width, bbox_px.height)` from the existing
projector (`AnomalyViewport::ProjectActorBoundsToScreenRect`) and
`R = top_resident_px` of the intended held mip. Require

```
P >= StuckMipMinTexelRatio * R          compiled default StuckMipMinTexelRatio = 4.0
```

i.e. one held texel must cover at least 4 screen pixels across the target's box.
Evaluated **at pick time**, bounds only, no pixel read — so it stays on the correct side
of `G127` and can run in the selector.

⚠ **Stated weakness, up front:** this assumes the texture maps roughly once across the
object. A **tiling** texture repeats N times, so the true texel size is `P/(N·R)` and the
rule over-admits. UV density is not available at pick time without the streaming
build data. ⛔ **Therefore the rule is a PICK-TIME FILTER ONLY and never decides
`observable`** — `observable` stays the E2 measurement. The two are independent, and a
disagreement between them is a reading worth having: `G3` reports it.

🔑 **The engine's own answer to the same question exists and we cannot read it**:
`FStreamingRenderAsset::WantedMips` is precisely *"how many mips does this need at this
screen size"* (`GetWantedMipsFromSize`, `StreamingTexture.cpp:304-311`), but
`FStreamingRenderAsset` lives in `Engine/Private/Streaming/` and is not exported.
`WantedMips − ResidentMips` would be the ideal perceptibility metric. **Filed, not
built** — and named here so nobody re-derives the wish.

---

## 5. Control surface

| knob | ini key (`[AnomalyInjector]`) | console | compiled default |
|---|---|---|---|
| held mip depth | `StuckMipLevelsDefault` | `IAI.Anomaly.StuckMipLevels` | `-1` = **to the floor** |
| visible co-user tolerance | `StuckMipMaxCoAffectedDefault` | `IAI.Anomaly.StuckMipMaxCoAffected` | `0` |
| perceptibility ratio | `StuckMipMinTexelRatioDefault` | `IAI.Anomaly.StuckMipMinTexelRatio` | `4.0` |
| unlink lock (§1.2) | `StuckMipUnlinkLockDefault` | `IAI.Anomaly.StuckMipUnlinkLock` | `false` |

⚠ **Deviation from the brief, stated:** the brief suggested `IAI.Anomaly.StuckMip.*`.
The shipped convention has **no dot after the anomaly name** —
`IAI.Anomaly.BlinkHalfPeriod`, `IAI.Anomaly.LodHalfPeriod`,
`IAI.Anomaly.LodMaxDistance`. The existing convention is followed; flagged rather than
silently changed.

Each knob follows the `AnomalyDefaults` shape exactly
(`AnomalyDefaults.h:36-66`): `…Compiled` constant, `…Key()`, `Get…()`, `Describe…()`,
`Set…Override()`, `Clear…Override()`, with **console > ini > compiled** precedence and a
`G139` provenance echo on the existing run-config line. Out-of-range values are
**REFUSED, not clamped** (the `m51`-era rule).

**Bench lever (can-fail, console-only, default OFF, never in a client payload):**
`IAI.Bench.StuckMipNoHold` — applies the anomaly's bookkeeping and **does not write the
bias**, so the mip never drops. Its pre-declared reading is `held:false` on every frame,
`observable:false` on every frame, `injected_frames` **empty**, and the event still
present in `annotation.json` with `observability_measured: false`. Without it, a green
`held:true` proves nothing (`G96`).

**Dashboard: ZERO changes required.** The pool checkbox list is a pure mirror of the
engine catalog — `Pool->SetBoolField(E.Id.ToString(), Auto->IsAnomalyEnabled(E.Id))`
(`ControlSnapshot.cpp:184-195`) — so adding the id to `GAutoPool` surfaces the toggle by
itself. This is the `m19` finding (*"the ENGINE IS AUTHORITATIVE and the dashboard has no
defaults of its own"*) and adding a dashboard-side default would re-create the second
source of truth `m19` removed.

**Default-pool membership — RECOMMENDATION, owner's call.** Add `stuck_low_mip` to
`GAutoPool` (`AnomalyAutoInjectorSubsystem.cpp:22-30`, bumping `NumPoolKeys` 6 → 7 at
`AnomalyAutoInjectorSubsystem.h:40` and the `static_assert` at `.cpp:32`), and **NOT** to
`GAutoPoolDefaultEnabled` (`.cpp:34-40`) for the first release — the yield under
`MaxCoAffected = 0` is unmeasured on real client content, and `lod_popping`'s precedent
(`m29` shipped the anomaly, `m30` decided pool membership after calibration) is the one
to copy. Flip it once `G7` has a number.
⚠ **`G140`/`G150` BOUNDARY: adding a seventh pool id re-rolls the seeded draw, so every
banked auto-pool run becomes non-comparable across this commit.** Regression legs must be
**TARGETED**.

---

## 6. Gates — pre-declared, with their readings

All legs **packaged**, not PIE (`G76`, and §1.3(b) makes this non-negotiable). Both tick
orders where the gate asserts a per-frame alignment (the `m44` standing rule). Fixtures:
**StackOBot MainWorld** and **Lyra**; `CB_GateLevel` is used only where §3.2 permits.

| id | gate | pre-declared reading |
|---|---|---|
| **G0** | **Preconditions read back, never assumed** — `r.TextureStreaming`, `r.Streaming.UseAllMips`, and per texture `IsStreamable()` / `IsCurrentlyVirtualTextured()` / `NumNonStreamingLODs`, echoed at `Apply` | `1`, `0`, streamable, non-virtual, floor `7` on StackOBot — *reported*, and any other value routes to the `NOT_APPLICABLE` counter rather than to a silent pass |
| **G1** | **Hold engages** — a targeted fire on a MainWorld target drops `resident_mips` from baseline to the floor | `held:true` on the span after the onset latency; `top_resident_px` = 64 for a 2048² texture |
| **G2** | 🚨 **Can-fail (`G96`)** — same leg with `IAI.Bench.StuckMipNoHold 1` | `held:false` on **every** frame, `observable:false` on every frame, `injected_frames` empty, event retained with `observability_measured:false`. **A G1 pass without this is not a result.** |
| **G3** | **Hold survives streaming pressure** — a camera sweep that would normally stream the mip back in (approach the target, then orbit), both fixtures | `held:true` on every captured span frame; `stuck_mip.resident_mips` constant at the floor; **zero** frames where the streamer wins. Any frame where it does wins is reported with its `co_affected_visible` and cvar read-back, not smoothed. |
| **G4** | **Onset latency, MEASURED** — frames between `Apply` and first `held:true` | *no value predicted*. The number becomes the required `SettleAfterApply` and it is reported whatever it is. `m44` ONSET (first-mask == first-label == first-differing-picture) must pass **because** the label is `AnomalyState`-driven (§4 E2). |
| **G5** | **Restore on every exit** — normal revert; `FinishRun`; capture cancelled before focus; target actor destroyed mid-span; level change mid-span | `resident_mips` returns to `baseline_mips` in every one of the five, and `NumCinematicMipLevels` is byte-restored to its saved value. Post-revert restore latency measured and required `<= SettleAfterRevert`, or the config is changed — **not** the gate. |
| **G6** | **Hitch** — paced 30 fps leg with and without the anomaly | `speed_ratio` and max frame time within the within-build spread. ⚠ A difference not larger than that spread is *"below the resolution of this instrument"* (`G169`), never *"no cost"*. |
| **G7** | **Auto-pool yield and refusal census**, both fixtures | *no value predicted*. Reports fires attempted / refused-shared / refused-not-streamable / refused-virtual / refused-imperceptible. This is the number the pool-membership decision needs. |
| **G8** | **Lyra gate** — one leg on `L_ShooterGym` or `L_Convolution_Blockout` at Lyra's own render defaults | the anomaly fires or refuses **for a named reason**; `NOT_APPLICABLE` counts reported. ⚠ Lyra sets `r.VirtualTextures=True` (`DefaultEngine.ini:73`) — but that is the *feature*, not the per-texture flag; the per-texture reading is what decides. |
| **G9** | **Schema additivity** — `P-C7 v3` against a pre-`m52` control pair | `labels.jsonl` field set **unchanged** on a run with no `stuck_low_mip` event; `run_summary` adds exactly the five `stuck_mip_*` keys; **`annotation.json` diff EMPTY (`P6` does not move)** |
| **G10** | **Verifier consistency read** — `tools/verify_capture.py --label-pixel-gate` on the m52 session | **READINGS ONLY.** `NO-TRACE` is the only failure and it is *not* expected to be informative here: the verifier cannot attribute a target change to the anomaly (079). Report the per-edge observations; do **not** treat `CONSISTENT` as confirmation (`docs/verifier-characterisation.md`). |
| **G11** | **Both build targets** — packaged `StackOBot` and modular `StackOBotEditor` | exit 0, **zero warnings**. ⚠ Load-bearing for m52 specifically: see the devirtualisation hazard in §7. |

🚨 **The `CB_GateLevel` question, decided in advance.** §3.2 shows its targets all carry
`BasicShapeMaterial`, which references **no texture at all**, so `stuck_low_mip` has
nothing to hold and refuses 100 % of the fixture. **`CB_GateLevel` is therefore NOT an
m52 gate fixture**, and it must not be edited to become one (`G99`:
`make_gate_level.py:50` deletes `LEVEL_PATH` before authoring, and the frozen level is
the A-side of every banked calibration). Two sanctioned routes, chat's call:
- **(a) recommended** — run `G1`–`G7` on **MainWorld** and **Lyra**, which both have
  authored, partially-exclusive content, and accept that m52 has no settled-camera
  arbiter;
- **(b)** author a **sibling** calibration level (`CB_MipLevel`) by the route
  `make_gate_level.py` already provides, giving each target its own distinct texture.
  Cost: a cook, and a new fixture to maintain.

---

## 7. Implementation notes that are easy to get wrong

🚨 **Call `StreamIn`/`StreamOut` through a `UStreamableRenderAsset*`, never a
`UTexture2D*`.** `UTexture2D::StreamOut`/`StreamIn` are declared **`final override`**
(`Texture2D.h:142-143`) and are **not** `ENGINE_API`. A `final` virtual call is a
standard devirtualisation target; if MSVC devirtualises it, the call becomes a direct
reference to an unexported symbol and the **modular editor target fails to link** while
the monolithic packaged target succeeds. Calling through the base pointer, where the
function is virtual-but-not-final (`StreamableRenderAsset.h:85-102`), forces vtable
dispatch. **This is exactly the class of defect `G221` was minted for, and gate `G11`
is what catches it.** Everything else m52 needs is either `ENGINE_API`
(`GetNumResidentMips`, `GetNumMips`, `GetSizeX/Y`, `UpdateCachedLODBias`,
`Link/UnlinkStreaming`), `FORCEINLINE` in a header (`GetStreamableResourceState`,
`IsStreamable`, `RenderResourceSupportsStreaming`), a public `UPROPERTY`
(`NumCinematicMipLevels`), or virtual (`GetUsedTextures`, `IsCurrentlyVirtualTextured`).

⚠ **No new module dependency.** All of the above is `Engine` + `CoreUObject`, which
`AnomalyInjector` already has. The game-agnostic invariant holds: no host types, no new
deps.

⚠ **The anomaly mutates a shared ASSET, not a component.** Unlike `missing_texture`
(per-component `SetMaterial`), m52 writes a field on a `UTexture2D` that every user
shares. The revert must therefore be at least as hardened as `m17`'s: capture the
texture by weak pointer **and** by object path, restore only if the current
`NumCinematicMipLevels` is still the value we wrote (the `IsCheckerDerived` guard's
analogue — never stomp a value the game changed), and log
`restored / left-to-game / unresolved` counts on every revert.

---

## 8. Open questions for chat

1. **Pool membership** — ship `stuck_low_mip` in `GAutoPool` but *not*
   `GAutoPoolDefaultEnabled` until `G7` produces a yield number? (recommended)
2. **`CB_GateLevel`** — route (a) MainWorld + Lyra only, or route (b) a sibling
   `CB_MipLevel`? (recommended: (a) now, (b) only if a settled-camera arbiter is needed)
3. **`label_schema`** — bump 2 → 3, or leave at 2? Recommended **leave at 2**: the new
   keys appear only inside a `stuck_low_mip` anomaly entry, an anomaly type no v2
   consumer has ever seen, so no v2 reader changes behaviour. `client-readme.md` §8 gains
   the field rows either way.
4. **`MaxCoAffected` default `0`** — accepted, or relaxed to `1`/`2` to protect yield
   before `G7` measures it? (recommended: keep `0`; it is the only value that guarantees
   no unlabelled blurry object, and the knob exists precisely so the owner can move it)
5. **Tier-2 (S2) multi-node disclosure** — filed, not built. Worth scheduling only if
   `G7`'s yield is unacceptable.
6. **Held set: all eligible textures, or only the largest?** Recommended **all eligible**
   (a real stuck-mip bug blurs the whole object; holding only base colour leaves normals
   sharp and reads as a subtler, less honest artefact).

---

## 9. File-by-file implementation plan

*Nothing below is written. Sizes are estimates.*

| file | change |
|---|---|
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.h` | **NEW.** `FAnomaly_StuckLowMip : IAnomaly`. `FHeldTexture { TWeakObjectPtr<UTexture2D>; FString Path; int32 SavedCinematicMips; int32 BaselineMips; }`, `TArray<FHeldTexture> Held`, `bActive`. Overrides `Apply`/`Revert`/`IsActive`/`IsCurrentlyAnomalous`. |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp` | **NEW, ~350 lines.** Resolve components via `AnomalyLod::ResolveLodComponents` (the shared resolver both texture anomalies use); `GetUsedTextures` per component; eligibility filter (§3.4 step 2); visible co-user count; perceptibility filter (§4 E3); apply bias + `UpdateCachedLODBias()`; per-reason refusal counters and `REFUSED` logs (`lod_popping` shape); `Revert()` with the `m17`-grade guard + `StreamIn(baseline, true)`; `IsCurrentlyAnomalous()` = measured hold. |
| `Source/AnomalyInjector/Public/AnomalyDefaults.h` / `Private/AnomalyDefaults.cpp` | Four knob blocks (§5), mirroring `LodPoppingMaxDistance` exactly. |
| `Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp` | `Register(MakeUnique<FAnomaly_StuckLowMip>())` at `:164`; `GetAuthoredSpec` arm at `:137` (`EAnomalyScope::Object`, optional int arg `mip_levels`). |
| `Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp` | add the id to `GAutoPool` (`:22-30`); **not** to `GAutoPoolDefaultEnabled` (`:34-40`); bench lever `IAI.Bench.StuckMipNoHold`. |
| `Source/AnomalyInjector/Public/AnomalyAutoInjectorSubsystem.h` | `NumPoolKeys` 6 → 7 (`:40`); the `static_assert` at `.cpp:32` then holds. |
| `Source/AnomalyInjector/Public/AnomalyViewport.h` / `Private/AnomalyViewport.cpp` | **additive** helper `CountVisibleComponentsUsingTexture(View, World, UTexture2D*, IgnoreComponent)`. ⛔ It must sit beside `ClassifyRenderableVisibleLive`, **not** inside `IsRenderableComponent` — `G96`'s trap (`m49` A2 hit its twin). |
| `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | one entry in `ResolveAnomalyActiveSource`'s map (`:280-290`) → `AnomalyState`; per-frame `stuck_mip.*` sampling in the tick-end block (`:4600-4625`); five `run_summary` counters. |
| `Source/AnomalyCapture/Private/AnomalyLabelWriter.{h,cpp}` | emit the `stuck_mip.*` keys inside the anomaly entry (`:72-130`), **only when present**. |
| `docs/client-readme.md` §8 | field rows for the new keys + one paragraph on what the anomaly is and its named limits. |
| `docs/architecture.md` | catalog entry; the lever paragraph; the sharing policy. |
| `docs/gotchas.md` | append: *`UTexture::LODBias` is ignored on a cooked platform* (§1.3b) and *the streamer cancels a stream request it did not issue* (§1.0ii). |
| `PRE-DELIVERY-CHECKLIST.md` | one categorical box: the delivered pool count, and the m52 `NOT_APPLICABLE` counters read back from a delivered `run_summary`. |

**Sequencing:** knobs + anomaly + registration → `G0`/`G1`/`G2` on MainWorld (stop if
`G2` cannot fire) → `G3`–`G7` → Lyra `G8` → `G9`–`G11` → docs. Estimated 1–2
implementation sessions after this plan is ruled.

---

## 10. What this file does NOT claim

- ⛔ **No incidence claim.** The sharing numbers are level-wide and asset-wide (§3.1).
  How often a *visible* co-user exists is unmeasured and is gate `G7`.
- ⛔ **No yield prediction** for the auto-pool under `MaxCoAffected = 0`.
- ⛔ **No hitch claim.** `G6` measures it; §1.4 argues only that the work is the same
  class the engine already performs continuously.
- ⛔ **No claim that the chosen lever holds on a host we have not run.** F-a…F-e are the
  named ways it can fail, and each one is detected rather than assumed absent.
- ⛔ **Nothing here is a verdict about `m51`**, which stays HELD at `53bf725`.

---

# AMENDMENT 1 — chat rulings and the pre-declared gate readings

**Appended 2026-09-20, session 080-02, BEFORE any gate leg ran and BEFORE any result was read.**
Nothing above this line is edited; the plan stands as written and this amendment records what
chat ruled, what the implementation actually did, and what each gate must read.

## A1.1 Rulings on §8, verbatim in effect

1. **Pool:** `stuck_low_mip` goes into `GAutoPool`; it is **NOT** in `GAutoPoolDefaultEnabled` until
   `G7` yields a number. Owner decides default-on after; chat's steer is default-on if **>= 25 %** of
   visible targets are eligible on MainWorld. ⛔ That 25 % is a FUTURE DECISION RULE, not a gate
   threshold, and `G7` passes or fails on nothing — it prints.
2. **Fixture:** route (a) — StackOBot **MainWorld** + **Lyra**. No `CB_MipLevel` cook now. If MainWorld
   yield proves too low to gate reliably, `CB_MipLevel` is the named future bench fixture.
3. **`label_schema` stays 2.** The new keys live inside the `stuck_low_mip` anomaly object only;
   m51's schema-3 bump absorbs them later. Recorded here as the schema note.
4. **`MaxCoAffected` default 0 — kept.**
5. **S2 multi-node disclosure — Tier-2 only, not built.**
6. **Hold ALL eligible textures of the target** (not just the largest).

Lever `L4'`, sharing policy `S4`, and `AnomalyState` with `IsCurrentlyAnomalous()` = *mip measurably
below baseline* are ACCEPTED. Console names follow the shipped convention. Gates are packaged-only
(Development, the capture build; capture is compiled out of Shipping).

**Each of the five failure modes F-a..F-e must have a label/telemetry field AND a gate that
exercises it** — that is a ruling, and A1.4 below records which gate exercises which.

## A1.2 Deviations the implementation made from §9, stated

1. **`AnomalyViewport` is UNCHANGED.** §9 planned an additive
   `CountVisibleComponentsUsingTexture` helper there. It was not needed:
   `AnomalyViewport::GetVisibleRenderableActors(World)` is already public
   (`AnomalyViewport.h:113`), so the visible-set co-user map is built inside the anomaly's own
   translation unit. Smaller blast radius than planned, and it keeps the `G33` chokepoint untouched
   — which is what §9's own warning was about.
2. **Telemetry is a GENERIC bag, not stuck-mip-specific plumbing.** `IAnomaly` gains one defaulted
   virtual `GetTelemetry(FAnomalyTelemetry&)` (the `m49` `IsVisualConditionHeld` precedent), and
   `FAnomalyTelemetry` is an int/bool/string key-value bag the label writer emits verbatim into the
   anomaly object. The writer needs no per-anomaly knowledge and `m53`/`m54` reuse it. A run with no
   `stuck_low_mip` event emits no key, because an inactive anomaly returns false.
3. **The unlink lock is a BENCH LEVER, not an ini knob.** §5 listed
   `StuckMipUnlinkLockDefault`. It is a fallback mechanism, not client-facing tuning, so it ships as
   `IAI.Bench.StuckMipUnlinkLock` — **and it is DELIBERATELY INERT**: §1.2 proves neither ordering
   works, so enabling it changes nothing today. The command exists to hold the place and to say so.
4. **Three knobs, not four:** `IAI.Anomaly.StuckMipLevels` (int, compiled `-1` = to the floor),
   `IAI.Anomaly.StuckMipMaxCoAffected` (int, compiled `0`), `IAI.Anomaly.StuckMipMinTexelRatio`
   (float, compiled `4.0`). Each with its ini key under `[AnomalyInjector]` and console > ini >
   compiled precedence; out of range REFUSED, never clamped.
5. **The bias is applied by SEARCH, not by algebra.** The needed
   `NumCinematicMipLevels` delta is computed, written, `UpdateCachedLODBias()` called, and the
   resulting `MaxAllowedMips` **read back** and corrected in a bounded loop (<= 8 steps). The group
   `MinLODSize` clamp and any other bias term therefore cannot silently change the achieved depth,
   and the achieved value is logged beside the requested one.

## A1.3 What already happened this session, before any gate leg

- **Both targets built, exit 0, ZERO warnings.** `StackOBotEditor Win64 Development` (modular — the
  only configuration that links module-to-module and therefore the only one that can catch the
  devirtualisation/missing-export hazard of §7) and `StackOBot Win64 Development` (monolithic).
- **Binary identity (`G121`):** built == staged == archived, **`A8742A4A`**, 241,468,416 B.
  Predecessor **`D50DDE78`** (the m51 F1 candidate) was hash-verified AT ITS ARCHIVE
  (`_binary_baselines\StackOBot.exe.m51-f1-candidate-D50DDE78`) **before** the staged copy was
  overwritten, per `A62`. New archive: `StackOBot.exe.m52-stuckmip-A8742A4A`.
- **Container quintet BYTE-UNCHANGED across the swap** — `67EA1FE0` / `2CEFB8F4` / `E03C6610` +
  `A16A18A8` / `C70ECDAA`, hashed before AND after. Code-only hot-swap, **no cook** (`G103`): m52
  adds no shader and no shader parameter struct.
- **`A44` scan of the STAGED artifact, both encodings:** eleven new symbols present in UTF-16 and
  absent in ASCII; three pre-existing controls also present (so the scan is SOUND, not blind); two
  invented symbols absent (so it DISCRIMINATES).

## A1.4 Pre-declared gate readings

⚠ **Gate numbering follows the 080-02 brief, which differs from §6's.** §6's `G0` (precondition
read-back) is folded into `G1` here. Where §6 and this amendment disagree on a NUMBER, this
amendment governs; where they disagree on a READING, that is a finding and must be reported.

| id | what it exercises | PRE-DECLARED reading |
|---|---|---|
| **G1** | unit + registry + precondition read-back | catalog lists **10** anomaly types (was 9) with `stuck_low_mip` scope `object` and one int arg `mip_levels` default `-1`; `GAutoPool` has **7** entries; the default-enabled pool still has **4** and does **NOT** contain `stuck_low_mip`; `r.TextureStreaming` reads **1** and `r.Streaming.UseAllMips` reads **0**, both READ BACK from the live console and echoed at Apply |
| **G2** | 🚨 **can-fail (`G96`)** — `IAI.Bench.StuckMipNoHold 1` | `stuck_mip.held` **false on every frame**, `observable` **false on every frame**, **zero labelled frames** for the event, `stuck_mip.bench_no_hold` true. **A `G3` pass without this is not a result.** |
| **G3** | hold through the span on MainWorld, under a camera sweep that would normally stream the mip back in | `stuck_mip.held` **true** on every captured span frame after onset; `stuck_mip.resident_mips` constant at `stuck_mip.forced_mips`; **zero** frames where the streamer wins. Any frame where it does win is REPORTED with its co-affected count and cvar read-back, not smoothed. Exercises **F-c** (`fail_force_resident`) and **F-d** (`fail_host_changed_bias`) — both expected **absent**, and both have a field so their absence is a reading rather than silence |
| **G4** | restore on every exit: normal revert · `FinishRun` · cancel before focus · target destroyed mid-span · level change | resident mip count returns to `stuck_mip.baseline_mips` in **all five**, and `NumCinematicMipLevels` is byte-restored to its saved value. Post-revert restore latency measured; if it exceeds `SettleAfterRevert`, **the config changes, not the gate** |
| **G5** | onset — m44 ONSET satisfied by construction | the **first labelled frame** has `stuck_mip.resident_mips < stuck_mip.baseline_mips`. Frames between Apply and the drop are **UNLABELLED**, and their `stuck_mip.*` keys are absent because an unlabelled frame carries no anomaly entry. *No latency value is predicted* — it is measured and reported |
| **G6** | hitch | **max frame time with the anomaly <= baseline + 2 ms** at 1280x720 on the bench recipe. ⚠ A difference not larger than the within-build spread is *below the resolution of this instrument* (`G169`), never *no cost* |
| **G7** | yield on the visible set, MainWorld and Lyra, 600-frame session | **PRINTED, NO THRESHOLD.** eligible/visible targets per frame, plus the five `run_summary` refusal counters. This is the number the pool-membership decision needs |
| **G8** | Lyra hold + restore | the anomaly fires or refuses **for a named reason**; `NOT_APPLICABLE` counts reported. Exercises **F-e** (`stuck_mip_refused_virtual`) — Lyra sets `r.VirtualTextures=True`, but that is the FEATURE flag and the per-texture `IsCurrentlyVirtualTextured()` is what decides |
| **G9** | schema additive + client-readme field test | `labels.jsonl` field set **UNCHANGED** on a run with no `stuck_low_mip` event; `run_summary` adds exactly the **eight** `stuck_mip_*` keys; **`annotation.json` diff EMPTY (`P6` does not move)**; `label_schema` still **2** |
| **G10** | both tick orders | native and `IAI.Bench.SynthTickOrder` both produce the same per-frame alignment: first labelled frame == first frame with `held` true, in both |
| **G11** | verifier consistency read (079 tool) | **READINGS ONLY.** Expected `CONSISTENT` / `OFFSET-NOTE`, **zero `NO-TRACE`**. 🚨 **If a `NO-TRACE` appears that is a FINDING about m52's labels — report it, do not tune.** A `CONSISTENT` run never confirms a label (`docs/verifier-characterisation.md`) |
| **G0-BUILD** | both build targets | **ALREADY READ, A1.3: exit 0, zero warnings, both targets.** |

**F-a (`fail_use_all_mips`) and F-b (`fail_streaming_off`)** are exercised by `G1`'s read-back: the
cvars are read from the live console at every Apply, and either one non-default sets its per-frame
flag. ⛔ They are not forced on a gate leg, because forcing `r.TextureStreaming 0` changes the whole
fixture's streaming behaviour and would make every other reading on that leg incomparable. **Their
detection path is proven by construction (the flag is written from the same read the log echoes);
their FIRING is not gated, and that is stated rather than implied.**

## A1.5 What this amendment does not do

- ⛔ It sets **no new threshold**. `G6`'s +2 ms is chat's, fixed here before the measurement.
- ⛔ It predicts **no onset latency**, **no yield**, and **no refusal counts**.
- ⛔ It does not permit tuning any default after a reading. A gate that misses is a NEEDS-DECISION
  with numbers.
---

# AMENDMENT 2 — the 080-03 rulings, and the gate readings they change

**Appended 2026-09-20, session 080-03, BEFORE any gate leg ran and BEFORE any result was read.**
Nothing above this line is edited. AMENDMENT 1 stands except where a reading is superseded below,
and where AMENDMENT 1 and this amendment disagree on a READING, **this one governs and the
difference is stated rather than quietly applied**.

## A2.1 What chat ruled on the 080-02 NEEDS-DECISION

1. **F1 is a DEFECT to fix, not accepted behaviour.** The mechanism named in 080-02 §5 (the
   revert's `StreamIn` is guarded by `!HasPendingInitOrStreaming()` and is therefore silently
   skipped exactly when a stream operation is in flight) is a hypothesis and is **diagnosed first,
   then fixed** — the fix is a VERIFIED restore plus an eligibility gate, not a wider guard.
2. **Onset latency is handled by starting the labelled window at the first held frame**, not by
   lengthening the settle. `F2` is therefore not a tuning problem.
3. **F3's reporting shape is ruled:** per-texture rows, and the ambiguous scalar names are made
   explicit.
4. **`MaxCoAffected` stays 0** — dataset purity over yield. **`GAutoPoolDefaultEnabled` stays
   off.** 18.75 % is below chat's 25 % steer and no default moves on this session's numbers either.
5. **`G4` and `G8` are run now**; the verifier gains a `stuck_low_mip` NO-TRACE route.

## A2.2 What is built, and the one thing each piece must not do

| ruling | built as | the thing it must NOT do |
|---|---|---|
| **R1** window at first held frame | `IAnomaly::HasDeferredOnset()` (defaulted **false**) + a pre-roll in the capture FSM's `Positives` phase that CAPTURES the frame and does **not** decrement `PhaseFramesLeft` until the fire is measurably held. Timeout `IAI.Capture.DeferredOnsetTimeout`, compiled **30** captured pre-roll frames | **It must not touch any other anomaly.** `HasDeferredOnset()` is false for all nine others, so `BurstAwaitsDeferredOnset` returns false and the phase code is the pre-m52 statement. That is STRUCTURAL inertness, and `G10`/`G6`'s blinking control is what reads it back |
| **R2** verified restore | revert clears the bias, then each texture whose resident count is below its recorded baseline is TRACKED and re-asserted every frame from `IAnomaly::TickAlways` until the engine's own `GetNumResidentMips()` reaches the baseline. `IAI.Anomaly.StuckMipRestoreTimeout`, compiled **120** | **The timeout must not stop the polling.** It COUNTS and NAMES the shortfall; the texture stays tracked until it is actually back |
| **R3** eligibility | a fire is refused `not_restored` if ANY of its candidate textures is still tracked, and `already_held` if the requested target is the one currently held | **It must not refuse silently.** Both are logged by name and counted in `run_summary` |
| **R4** telemetry shape | `stuck_mip.textures[]` per-texture rows; `stuck_mip.resident_mips`/`baseline_mips` RENAMED `primary_*`; added `held_all` and `onset_latency_frames` | **It must not move `annotation.json`.** `P6` does not move; the rename is outright because the old names shipped in no release |
| **R5** verifier | `stuck_low_mip` earns NO-TRACE **only** on runs where `stuck_mip.held` is true on EVERY labelled frame | **It must not widen the tool.** An unheld or missing flag leaves the run UNASSESSABLE — the verdict the class already had |
| **R7** hitch instrument | max and p99 of consecutive `t_wall` deltas from `labels.jsonl` | **It must not be read off a PACED leg.** The pacer absorbs exactly the variance being measured (the `m35` `G-M6` lesson), so the R7 pair runs `-Pace 0` and a paced reading is labelled as measuring the pacer |

⚠ **`hold_timeout` is a LOG LINE AND A COUNTER, NEVER AN `annotation.json` FIELD.** The brief
says a timed-out fire is `manifested:false` *with reason `hold_timeout`*. That reason ships in
the capture log and in `run_summary.stuck_mip_hold_timeouts`; putting it in `annotation.json`
would move `P6`, which `G9` forbids. The `manifested:false` half needs no new code — an
`AnomalyState` event with no active frame already lands there.

## A2.3 Schema movement this amendment PREDICTS, so `G9` reads it rather than discovering it

- `labels.jsonl` anomaly keys for a `stuck_low_mip` entry: **11 → 14**. Removed **2**
  (`stuck_mip.resident_mips`, `stuck_mip.baseline_mips`), added **5**
  (`primary_resident_mips`, `primary_baseline_mips`, `held_all`, `onset_latency_frames`,
  `textures`). ⚠ **This is the first REMOVAL m52 has made**, and it is only permissible because
  those two names have never left this branch.
- `run_summary`: **8 → 16** `stuck_mip_*` keys; added exactly `refused_not_restored`,
  `refused_already_held`, `hold_timeouts`, `restore_timeout`, `restore_frames_max`,
  `textures_awaiting_restore`, `onset_latency_max`, and nothing else.
- `labels.jsonl` ROW keys **13 → 13**; `annotation.json` root **4 → 4**; `label_schema` **2**.

## A2.4 Pre-declared readings — only the gates whose reading CHANGES are restated

| id | PRE-DECLARED reading |
|---|---|
| **G2** can-fail | unchanged in intent, **re-run on the new binary**: `held` false on every frame, `observable` **null** (AMENDMENT: 080-02 pre-declared `false`; the shipped value is `null` = unmeasured, which is the m49 A1 tri-state doing its job and is the honest value — restated here, not tuned), events present with `manifested:false` and empty `injected_frames`. 🚨 **With R1 live, the can-fail leg must ALSO hit the hold timeout**: no fire can ever hold, so every burst must spend `DeferredOnsetTimeoutFrames` pre-roll frames and be counted in `stuck_mip_hold_timeouts`. **A can-fail leg that does NOT time out means R1's wait is not wired.** |
| **G3** hold | ≥ **6 of 7** fires manifest on MainWorld (080-02 read 4 of 7 with the window being eaten by latency); **0** frames go held→unheld mid-window; every manifested event carries **exactly `PositiveFrames` = 8** labelled frames. ⚠ *The 6-of-7 figure is chat's pre-declared bar, fixed before the leg; a miss is a NEEDS-DECISION with numbers, never a re-run for a better one.* |
| **G3b** 🚨 F1 | the 080-02 `K=8` recipe is re-run on the new binary against its banked `A8742A4A` A-side, which read **ZERO held frames** and 24 × *"already at or below the requested resident mip count"*. **Two readings are acceptable and they are different claims**: (a) the `already at or below` skips are GONE and later fires hold ⇒ R2 restored the texture, the defect is fixed; (b) they are replaced by explicit `not_restored` refusals ⇒ R2 did NOT restore it and R3 made the failure visible instead of silent. ⛔ **A third reading — `already at or below` still appearing — is a FAILURE**, because it means a fire got past R3 with a depressed baseline |
| **G4** restore, five exits | each exit reports `restored=N left-to-game=0 unresolved=0` **and** a `RESTORE VERIFIED` line per tracked texture. Where a path cannot be produced on this fixture it is reported **UNRUN with the reason**, never as a pass, and the structural argument for it is stated separately from any measurement |
| **G5** onset | the **first labelled frame is the first held frame** and the labelled window is contiguous — which is also the cross-check that the FSM's tick-time read and the label's tick-END read agree, since a disagreement would open the window one frame early and leave an unheld frame inside it |
| **G6** hitch | **R7**: max and p99 frame time, `-Pace 0`, m52 vs the blinking control on the same binary and map. ⚠ **No threshold is added.** AMENDMENT 1's +2 ms stands as chat's bar for the MAX; p99 is reported beside it with no bar at all |
| **G7** yield | re-measured with the refusal table split out to include `not_restored`, `already_held` and `hold_timeouts`. **PRINTED, NO THRESHOLD.** If it reaches ≥ 25 % that is REPORTED to chat, and **no default is flipped in this session** |
| **G8** Lyra | hold + restore on the second fixture, and the virtual-texture `NOT_APPLICABLE` path must fire **at least once** or Lyra is stated to contain no virtual-textured candidate and the counter is reported as **never exercised** rather than as zero |
| **G11** verifier | `NO-TRACE 0` **and** the class now AVAILABLE: at least one run must print a `stuck_low_mip` outcome that is NOT *"NO-TRACE unavailable for class"*. 🚨 **080-02's G11 zero was weaker than it looked and said so; this is the gate that makes it a reading.** A NO-TRACE that appears is a FINDING about m52's labels, reported and not tuned |

## A2.5 What this amendment does not do

- ⛔ It sets **no new threshold**. 30 and 120 are chat's, fixed here before the measurement; the
  6-of-7 bar is chat's; the +2 ms is AMENDMENT 1's.
- ⛔ It predicts **no yield**, **no onset latency**, **no restore latency** and **no refusal counts**.
- ⛔ It does not permit flipping `GAutoPoolDefaultEnabled` or raising `MaxCoAffected` on any
  reading this session produces.

---

# AMENDMENT 3 — the 080-04 rulings, the ratio that drives the depth, and the S2' proposal

**Appended 2026-09-20, session 080-04, BEFORE any gate leg ran and BEFORE any result was read.**
Nothing above this line is edited. AMENDMENTS 1 and 2 stand except where a reading is superseded
below, and where they and this amendment disagree on a READING, **this one governs and the
difference is stated rather than quietly applied**.

## A3.1 What chat ruled on the 080-03 NEEDS-DECISION

1. **`G11`'s two NO-TRACE events are ACCEPTED AS TRUE FINDINGS** - labels unsupported by pixels,
   not a tool defect.
2. **`R1`** the footprint/mip ratio gate stays a pick-time filter, but the forced mip is chosen so
   that the top resident mip is `<= bbox_px / 8`. `mip_levels -1` = as deep as the ratio rule
   requires, floored at 4 resident mips. New telemetry `stuck_mip.ratio_at_pick` and
   `stuck_mip.forced_top_px`.
3. **`R2`** a predeclared retest on the SAME auto-pool recipe as 080-03's `G11` leg. **If ANY
   NO-TRACE survives on a `held:true` event: STOP the perceptibility line and report
   NEEDS-DECISION; chat escalates to `m55`. Do not iterate the ratio.**
4. **`R3`** the 8th-frame FSM behaviour is accepted as pre-existing; documented, not changed.
5. **`R4`** bind the target's destruction, revert immediately, do not label frames after it,
   counter `stuck_mip_revert_on_destroy`, bench lever `IAI.Bench.DestroyTarget`, gate `G4e`.
6. **`R5`** the teardown limit stands; add `stuck_mip_unverified_at_teardown` and a client line.
7. **`R6`** exercise the virtual-texture path once, or record it UNEXERCISED by name.
8. **S2' is DESIGNED, NOT BUILT.** Default-on deferred. The veto/observability disagreement is
   queued as 080-05.

## A3.2 🔻 A CORRECTION TO `R1`'s PREMISE, STATED BEFORE THE RETEST RAN

**"As deep as the ratio rule requires, floored at 4 mips" CANNOT REACH 4 MIPS ON A COOKED
TEXTURE, AND NO BIAS CAN MAKE IT.** The streamer clamps the per-texture ceiling to the asset's
own non-streaming floor and then asserts it:

```
MaxAllowedMips = FMath::Clamp(ResourceState.MaxNumLODs - LODBias,
                              ResourceState.NumNonStreamingLODs, ...)   StreamingTexture.cpp:229, :233
check( MaxAllowedMips >= ResourceState.NumNonStreamingLODs );           StreamingTexture.cpp:236
```

On this cooked bench `NumNonStreamingLODs` is **7** on every candidate texture measured, so the
deepest achievable top resident mip is **64 px**, and a 401 px target can reach a ratio of at most
**6.27** however deep the request. ⇒ **the depth half of `R1` cannot bite here; what bites is the
FILTER.** A target that cannot reach the ratio at the deepest achievable hold is REFUSED.

**As built, and the deviation is stated rather than folded in:**
- `mip_levels -1` resolves to `max(NumNonStreamingLODs, StuckMipMinResidentMips)` -- the deepest
  the lever can reach, with a new compiled guard of **4 resident mips**. ⛔ **The guard is INERT on
  this fixture** (the engine floor of 7 is already deeper) and exists for a host whose floor is
  lower. ⚠ It is a compiled constant with no ini key and no console override, which is a smaller
  surface than the `AnomalyDefaults` convention; that is deliberate for a value nothing here can
  exercise, and it is flagged so the omission is a decision rather than an oversight.
- ⛔ **`-1` is NOT read as "back off to exactly the ratio".** Holding a large object SHALLOWER
  because the ratio is already satisfied would reduce the visible signal, which is the opposite of
  the ruling's purpose. `-1` goes as deep as the lever allows, which satisfies the ratio *a
  fortiori* wherever it is satisfiable at all.
- `StuckMipMinTexelRatio` compiled default **4.0 -> 8.0**. ⚠ This is a DIRECTED change, fixed by
  chat before the retest, not a tune after a reading; 080-03's instruction was "do not tune the
  ratio to make `G11` green", and this is chat changing the predicate in advance instead.

## A3.3 The two refusal reasons, and why they are two

| reason | fires when | what it says |
|---|---|---|
| `too_small_for_ratio` | depth came from the ratio rule (`-1`) and the DEEPEST achievable hold still misses the ratio | the object is too small on screen for any blur this lever can produce |
| `imperceptible` | an EXPLICIT `mip_levels` was requested and that depth misses the ratio | the requested depth is too shallow; a deeper one would pass |

The auto-pool always uses `-1`, so an auto-pool census shows `too_small_for_ratio` and leaves
`imperceptible` at 0. **That split is itself the reading**: it says whether the floor or the
operator is the binding constraint. No key is removed; `imperceptible` keeps its name and its
meaning narrows.

## A3.4 Schema movement this amendment PREDICTS, so `G9` reads it rather than discovering it

- `labels.jsonl` anomaly keys on a `stuck_low_mip` entry: **14 -> 16**, added exactly
  `stuck_mip.ratio_at_pick` and `stuck_mip.forced_top_px`, removed **0**.
- Per-texture record keys: **7 -> 9**, added `forced_top_px` and `ratio_at_pick`.
- `run_summary`: **15 -> 18** `stuck_mip_*` keys, added exactly `refused_too_small_for_ratio`,
  `revert_on_destroy`, `unverified_at_teardown`, removed **0**.
- `labels.jsonl` ROW keys **13 -> 13**; `annotation.json` root **4 -> 4** and per-event **16 -> 16**;
  `label_schema` **2**.

## A3.5 Pre-declared readings - only the gates whose reading CHANGES are restated

| id | PRE-DECLARED reading |
|---|---|
| **G4e** 🚨 NEW | fire, destroy the target mid-window with `IAI.Bench.DestroyTarget`, expect: a revert line naming the destruction, `restored=N left-to-game=0 unresolved=0`, a `RESTORE VERIFIED` line per texture, `stuck_mip_revert_on_destroy` **1**, **no labelled frame after the destroy tick**, `stuck_mip_textures_awaiting_restore` **0** at `FinishRun`, no crash, leg exit 0 |
| **G7** yield | the refusal census gains `too_small_for_ratio` as its own row. **PRINTED, NO THRESHOLD.** No yield is predicted |
| **G11** 🚨 the R2 retest | **NO-TRACE 0 across the leg.** The two 080-03 `SM_rock` events, or their equivalents at that seed, read CONSISTENT/OFFSET-NOTE **or do not occur at all** because the ratio gate refuses them - a refusal is an acceptable route to zero, and which route it took must be REPORTED from the refusal log rather than inferred. 🚨 **ANY surviving NO-TRACE on a `held:true` event STOPS the perceptibility line** |
| **R6** VT probe | EXERCISED (a runtime-virtual texture was found, classified `NOT_APPLICABLE`, and counted) **or** UNEXERCISED with the reason named. ⛔ A zero from a branch that was never reachable is BLINDNESS and must not be written as a clean read |

## A3.6 S2' — ONE HOLD, N EVENTS — **PROPOSAL ONLY, NOT BUILT, NOT AUTHORISED**

**The problem.** A texture is an asset. On authored content the same texture is sampled by several
visible actors, so the shipped `MaxCoAffected 0` refuses the hold outright: Lyra's `L_ShooterGym`
yields **0 events** at the shipped default for that reason alone. Raising the cap admits the hold
**and blurs neighbours the label does not name**, which teaches the model that blurry is normal.

**The proposal.** One texture hold produces **one event per VISIBLE co-affected actor**: each with
its own target, its own stencil tag, its own mask, its own observability, and a shared
`stuck_mip.shared_hold_id` linking them. `MaxCoAffected` becomes the maximum number of visible
co-affected actors ADMITTED rather than TOLERATED; proposed default **4**.

### The seven questions, answered from this session's measurements

**1. Stencil-pool cost per fire. 🚨 THIS IS THE BLOCKER.** The pool is `200..254` = **55**
assignable values, `m50` reserves **8** for the census, and `EventClaimed` is **never released
mid-run by design** (`mask_map.json` maps value -> event for the whole session). Measured
co-affected distribution over refused textures:

| fixture | co=1 | co=2 | co=3 | co=4 | co>=6 | total |
|---|---|---|---|---|---|---|
| StackOBot `G7` 600 frames | 24 | 14 | 14 | 0 | 28 | 80 |
| Lyra `L_ShooterGym` 120 frames | 0 | 0 | **21** | 0 | 0 | 21 |

Today's `G7` leg spends **15** tags on 15 events. Under S2' at cap 4 each admitted fire spends
`1 + N` tags, and **52 of the 80 shared refusals sit at `co <= 4`**, so a comparable session would
spend on the order of **50-80 tags against a ceiling of 47**. ⇒ **S2' AT CAP 4 EXHAUSTS THE STENCIL
POOL WITHIN ONE 600-FRAME SESSION ON THIS FIXTURE**, and `m50`'s exhaustion path then ships targets
as unmeasurable. ⛔ **S2' cannot be built as proposed without either a shorter session, a lower cap,
or a tag-recycling change that `m50` deliberately refused.** That is the first thing chat must rule on.

**2. `m44` one-target ownership.** `m44`'s rule is *"an actor under a live fire belongs to its
event"*. S2' does not break it -- each co-affected actor gets its OWN event and its OWN tag, so
every actor still belongs to exactly one event. What it does break is the **implicit** assumption
that one FIRE produces one EVENT: `LiveFires` is keyed per anomaly id, and `stuck_low_mip` holds
one `Held` array. The N events would have to be siblings of one fire, which means either N entries
in `LiveFires` for one anomaly id (violating the one-instance-per-id registry invariant) or a new
event-fan-out at the capture layer. **The second is the only route that does not touch the
registry.**

**3. `m26` veto.** Per event and unchanged, which is the right shape: a co-affected actor that
draws zero pixels is vetoed on its own evidence and its siblings survive. ⚠ Cost: a fire can now
lose some of its events and keep others, so `vetoed_events` stops being comparable across the
change (`G140`'s shape on a new axis).

**4. `annotation.json` shape.** N events sharing `stuck_mip.shared_hold_id`. ⛔ **That field cannot
live in `annotation.json` without moving `P6`.** It rides `labels.jsonl` (where the other
`stuck_mip.*` keys already live) and the join is by `(anomaly_type, start_frame)`, which the
artifact already carries. **No `annotation.json` field is added.**

**5. Verifier reading.** Each event is judged on its own mask, so `R5`'s HELD-GATED rule applies
per event unchanged. ⚠ **And this session says that is not enough**: today's Lyra bench-override
leg produced `NO-TRACE 3` on events whose ratio was **26.76-31.59**, far above the gate, so the
per-event verdict would flag siblings the pick-time rule cannot predict.

**6. The Lyra yield it would give.** All **21** of Lyra leg 1's shared refusals sit at exactly
`co_affected = 3`, i.e. entirely inside a cap of 4. ⇒ **S2' would take `L_ShooterGym` from 0 events
to a non-zero yield, at 4 events (1 + 3) per admitted fire.** That is the strongest argument FOR
S2' in this file, and it sits directly against the pool arithmetic in (1).

**7. Implementation size.** `Anomaly_StuckLowMip.{h,cpp}` +120 (record the co-affected actor set
per held texture, expose it); `AnomalyCaptureSubsystem.cpp` +150 (fan one fire out to N event
accumulators, N tags, N mask records); `AnomalyAutoInjectorSubsystem` +30 (live-fire fan-out);
`AnomalyLabelWriter` +20 (`shared_hold_id`); plus a gate set of its own. **~320 lines and a new
gate campaign** - comparable to `m52` itself, and NOT a small change.

⛔ **Nothing in S2' is authorised. It is costed so chat can decide, and the pool arithmetic in (1)
is a stop, not a caveat.**

## A3.7 What this amendment does not do

- ⛔ It sets **no new threshold** other than the ratio chat fixed at 8.0 before the retest.
- ⛔ It predicts **no yield**, **no refusal counts**, and **no restore latency**.
- ⛔ It does not permit flipping `GAutoPoolDefaultEnabled` or raising `MaxCoAffected`.
- ⛔ It does not build S2', and it does not touch the `m26`/`m49` disagreement queued as 080-05.
