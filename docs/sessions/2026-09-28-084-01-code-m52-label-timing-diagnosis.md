# Session 084-01 — `m52` (`stuck_low_mip`) label timing: diagnosis from source and bank (part A, bench-free)

**Brief 084-01, Claude Code, headless, 2026-09-28.** Part A only:
- source reading, plugin and UE 5.1 engine;
- measurement of the existing bank from pixels;
- an independent Codex diagnosis through the Relay, a reconciliation, and one Codex consult round;
- a part-B bench plan.

⛔ **Nothing was launched, compiled, cooked or staged. No source, tool, fixture or schema file changed.** Plugin source is
read at `master` `b5f15a3` (`git show master:...`). m53 S2 is paused and was not touched. This branch
(`fix/m52-label-timing`, off `master`) carries this journal and `G343` only.

Reviews: `_reviews/084-01-codex-m52-label-timing-diagnosis.md` (Codex, **ROOT-CAUSE-ESTABLISHED**) and
`_reviews/084-01b-codex-m52-consult.md` (Codex consult, **AGREED-WITH-CORRECTIONS**). Every correction the consult made
that verified against source or bank is **adopted below and marked 🔻**. Measurement scripts and per-event output:
`D:\IntrusiveAnomalies\_reviews\084-01-code-m52-measure\`. That folder includes `my_findings_before_codex.md`, written
before either Codex report was read. It is historical: its `MipBiasFade`, "≥1 by construction" and "all flip together"
lines are withdrawn below.

## 0. The answer

1. **Onset is late because the window waits for a game-thread mirror.** The label, and the target mask, open when
   `UTexture2D::GetNumResidentMips()` reads below baseline. That value is `CachedSRRState.NumResidentLODs`. The engine
   refreshes it only when a later `TickStreaming` finds the pending update complete, and that completion is marked only
   after the render-thread swap. So the first frame rendered with the low mip normally carries no label.
   - **Bank:** the dominant blur step is **one captured frame before the label on 43 of 43** static-camera StackOBot
     events, with smaller departures up to 4 frames earlier.
   - 🔻 This is **not a guarantee of ≥1**. A same-poll race is allowed (§2.3).
2. **Offset is early because the window ends with the request.** `BeginRevert` removes the fire in the same tick, so the
   revert-tick frame is already labelled clean, and `Revert()` sets `bActive=false`. The picture stays low until the
   stream-in lands.
   - **Bank:** every one of 42 complete static events is still below half-depth for **5–14** captured frames after the
     label ends (mode 5–6).
   - First recovery shows at revert +6 engine frames; all textures verify at +7 on 42 of 42.
3. **Masks move with labels.** Both use `IsFireLabelledThisFrame` over `GetLiveFires()`. So does `observable`, through
   `IsVisualConditionHeld`. Their agreement proves nothing about the picture.
4. **What separates the office host's "correct" offsets is NOT established.** The bench has no correct-offset complete
   event. Candidates:
   - restore latency against the frames that are not captured;
   - textures whose blur is imperceptible (`G270`);
   - the `already-back` race (§3).

## 1. The symptom (owner's readings on an office host, `master` `b5f15a3`; no office data)

- Blur is visible from frame 102, but labels and target masks start at 104, and they agree with each other.
- Some events switch off correctly. On others the blur is still visible about 5 frames after the label switches off.
- `corrupted_texture` on the same host shows no desync.

## 2. Onset

### 2.1 The label predicate

- `stuck_low_mip` resolves to `EAnomalyActiveSource::AnomalyState` (`AnomalyCaptureSubsystem.cpp:300`).
- `IsFireLabelledThisFrame` calls `Injector->IsAnomalyCurrentlyAnomalous` (`:4955-4971`).
- That calls `FAnomaly_StuckLowMip::IsCurrentlyAnomalous`: `bActive && ANY held texture GetNumResidentMips() <
  BaselineResidentMips` (`Anomaly_StuckLowMip.cpp:892-907`).
- It is sampled at `OnWorldTickEnd` of the captured frame's tick (`SampleDeferredActiveState`, `:4846-4876`).
- The deferred-onset FSM waits on the same predicate (`BurstAwaitsDeferredOnset`, `:4315-4344`, used at `:731-753`).
- `observable` additionally requires `ConditionHeld` (`:4053-4071`). For m52 that is `IsVisualConditionHeld()`, whose default
  is `IsCurrentlyAnomalous()` (`IAnomaly.h:32`): the same mirror again.

### 2.2 Mirror versus picture

- **What the mirror is.** `GetNumResidentMips()` returns the game-thread copy `CachedSRRState.NumResidentLODs`
  (`Texture2D.cpp:481-483`). On success it is written only in `UStreamableRenderAsset::TickStreaming` when
  `PendingUpdate->IsCompleted()` (`StreamableRenderAsset.cpp:172-179`).
- **When the picture changes.** The render-thread `Finalize` → `DoFinishUpdate` → `FStreamableTextureResource::FinalizeStreaming`
  swaps `TextureRHI` and calls `RHIUpdateTextureReference`, and only **then** marks the update successfully finished
  (`Texture2DStreamOut_AsyncReallocate.cpp:35-42`, `Texture2DUpdate.cpp:147-161`, `StreamableTextureResource.cpp:214-237`).
  Scene render commands queued after the swap sample the reduced texture. Update tasks reach the render thread through
  `ENQUEUE_RENDER_COMMAND` (`RenderAssetUpdate.cpp:258-278`), FIFO with the scene commands.
- **Frame order.** Each engine frame runs:
  1. the world tick, which includes our sample at `OnWorldTickEnd` (`GameEngine.cpp:1775`);
  2. `RedrawViewports`, which queues this frame's scene (`:1891`);
  3. `IStreamingManager::Get().Tick` (`:1901`).
- **The streamer is staged.** `r.Streaming.FramesForFullUpdate` defaults to 5 (`TextureStreamingHelpers.cpp:257-260`). A cycle
  is stage 0, then 5 slice stages, then a final stage that waits for the async mip-calc task (`StreamingManagerTexture.cpp:1647-1762`,
  `:1736`).
  - An asset's `TickStreaming` runs when its slice is processed (`UpdateStreamingRenderAssets`, `:1174-1297`, via
    `UpdateStreamingStatus`, `StreamingTexture.cpp:262-278`), and again in the final stage's in-flight loop (`:1740-1744`).
  - The stream-out is issued from `StreamWantedMips` in the final stage (`StreamingTexture.cpp:562-595`).
  - 🔻 In packaged builds (`r.Streaming.OverlapAssetAndLevelTicks` = `!WITH_EDITOR`, `:42-46`), the slice stage is
    dispatched as a task on `AnyHiPriThreadHiPriTask`, tagged as parallel game-thread work (`:1608-1619`, `:1715-1729`).
    It is **not** a deferred task on the game thread; the editor runs it inline.
- **D3D12 with an RHI thread.** `RHIAsyncReallocateTexture2D` goes through the immediate command list to
  `AsyncReallocateTexture2D_RenderThread` (`RHICommandList.h:5717-5720`, `:4562-4566`). That queues the copy for the RHI
  thread unless the list is in bypass (`D3D12Texture.cpp:1237-1264`; counter decremented at `:272`). The render-thread task
  loop then suspends on `TaskSynchronization` (`RenderAssetUpdate.inl:58-61`). So scheduling `Finalize` needs another
  `TickStreaming` (`:83-90`).

### 2.3 How late — measured, not guaranteed

- 🔻 **"≥1 captured frame by construction" is withdrawn** (consult row 7). Source guarantees only that the render-thread
  update precedes the successful game-thread acknowledgement. The allowed race runs like this:
  1. A poll schedules `Finalize` behind scene F.
  2. `Tick` releases its lock (`RenderAssetUpdate.cpp:203`).
  3. The render thread runs scene F (old mips) and then the swap before the same poll reaches `IsCompleted()`
     (`StreamableRenderAsset.cpp:172-178`; `IsCompleted` is `TaskState == TS_Done`, `RenderAssetUpdate.h:61-65`).
  4. Then sample F+1 and render F+1 agree.

  Unlikely, but not excluded. Capture gaps also stop any render-frame bound becoming a saved-frame bound.
- **Measured:** the dominant step is 1 captured frame before the label on every static StackOBot event (§5). The magnitude
  depends on the stage and slice at which each texture's swap lands, IO and render queue progress, capture cadence, and
  which texture dominates the image.
- ⛔ **A constant "subtract N" correction is refuted** by the bank itself (dominant 1, earlier departures up to 4).

### 2.4 The design's false premise

- The m52 predictions asserted that the tick-end resident count describes the same frame's render
  (`docs/predictions/2026-09-20-m52-stuck-low-mip.md:226-229`, `:443-446`).
- They gated onset as "first labelled frame has resident < baseline", which passes by construction (`:740`) and was never
  checked against pixels.
- The label-vs-pixel verifier could only return `NO-TRACE` for this class, and disclosed `edges extrapolated`
  (journal 080-02 §4). **m52's edges were never gated against pixels.**

## 3. Offset

- **The revert-tick frame is labelled clean.**
  - The positive budget ends with `CaptureCurrentFrame(); --PhaseFramesLeft; … BeginRevert()` in the same tick
    (`AnomalyCaptureSubsystem.cpp:756-759`).
  - `BeginRevert` calls `RevertAllLiveFires` (`:4400-4409`), which clears the live-fire list.
  - `FinalizeArmedLabel`, at the end of that tick, then reads the empty list (`:806`, `:4666-4668`).
  - So `positive=8` yields 7 labelled frames on 56 of 60 measurable bench events.
- **The anomaly goes inactive at once.** `FAnomaly_StuckLowMip::Revert`:
  1. restores `NumCinematicMipLevels`;
  2. calls `StreamIn(baseline)` unless the asset is busy;
  3. tracks each texture still below baseline in `Restoring`;
  4. clears `Held` and sets `bActive=false` immediately (`Anomaly_StuckLowMip.cpp:698-765`).
  `TickAlways` keeps polling the restore (`:784-867`), but nothing it learns reaches a label, mask or `observable`.
- **The capture phases.** `SettleAfterRevert` frames are not captured, and the tick in which a phase transitions does not
  capture either (`AnomalyCaptureSubsystem.cpp:722-724`, `:762-765`). `PostGap` frames are captured (`:767-768`). The next
  `BeginFire` can come before the previous target's picture has recovered.
- **The `already-back` race (Codex, source-supported, UNMEASURED).** `Revert` counts a texture as `already-back` and does not
  track it when the cached count reads baseline (`Anomaly_StuckLowMip.cpp:719`), with no `bBusy` test. A stream-out still in
  flight, as after a HOLD TIMEOUT, can then complete after the event and leave an unlabelled low-mip picture until the
  streamer's own restore.
- **Recovery shape.** The picture stays at held level until revert +5; it first recovers at +6 (or +9 where +6…+8 were not
  captured). The game-thread `RESTORE VERIFIED` for **all** of the event's textures lands at +7 (§5). After the first
  recovery, sharpness keeps creeping back for 10–30+ engine frames.
- ⚠ **Withdrawn during this session:** I first attributed the creep to `FMipBiasFade`
  (`StreamableTextureResource.cpp:222-224`, `RenderResource.cpp:1052-1125`).
  - `CalcMipBias()` is called only inside `SetNewMipCount` (`RenderResource.cpp:1096`) and, through `IsFading()`, by
    `UTexture::HasPendingLODTransition` (🔻 `Texture.cpp:1126-1129` calls `IsFading()`). No renderer consumer exists; it is
    bookkeeping, not a sampling fade.
  - 🔻 The consult also found that, on the F-H event it examined, all three textures had acknowledged restore **before** the
    creep. So an unfinished restore of one of them does not explain it either.
  - The creep is **measured and unattributed**. Candidates: temporal history, later swaps of other resources, scene drift.

## 4. Masks and `observable`

- `ArmTargetMaskOwn` skips any fire where `!IsFireLabelledThisFrame(F)` (`AnomalyCaptureSubsystem.cpp:1179-1190`).
- m26's record window uses the same predicate (`UpdateMaskRecordLabelledWindow`, `:1141-1157`).
- `observable` reads `ConditionHeld` (§2.1).
- All three are sampled in the same tick end (`OnWorldTickEndCombined`, `:877-883`). The owner's "labels and masks agree" is
  expected and says nothing about the picture.
- All iterate `GetLiveFires()`. So **no predicate change can label the post-revert tail**: the event must outlive the
  revert.
- The mask must also already be armed on the first low-mip frame, which the game thread cannot know about in time.

## 5. Bank measurements

### 5.1 Method (read-only)

- **Sessions.** 72 bank aliases carry `stuck_low_mip`: **30 unique sessions**, 20 StackOBot and 10 Lyra, with every row's
  PNG present.
- **Region.** The event's own target-mask pixels (its `mask_value`) unioned over its labelled frames when ≥400 px; otherwise
  the median projected bbox.
- **Metric.** `S` = mean absolute horizontal + vertical grey-level gradient in the region. It is a sharpness measure: a held
  low mip removes high-frequency detail.
- **Pre level and noise.**
  - The pre level is the median `S` over the first ≤9 captured frames after Apply; `σ` comes from the same frames.
  - `thr = max(5σ, 2 % of pre)`.
  - An event is **measurable** if its labelled frames' median drops by ≥3·thr.
  - ⚠ Both the depth and the offset search use the candidate labels, so this is a **diagnostic, not an independent
    acceptance oracle** (consult P2).
- **Edges.**
  - Onset "half-step": the first frame below half the held depth.
  - Onset "first departure": the first of two frames below `pre − thr`.
  - Offset: the first frame after the label at or above half-depth, and the first frame recovering by ≥20 % of the depth.
- **Joins.** Engine frames come from `labels.jsonl.frame_index`. Log lines join on the `[gfc % 1000]` prefix.
- 🔻 **Census corrected after the consult** (`recensus.py`):
  - camera excursion is now the **total** displacement from Apply, translation and `view.rot` rotation (the first pass
    read a non-existent `rotation` key and the largest adjacent step);
  - "static" means ≤0.5 cm and ≤0.05°;
  - the restore join takes **all** of the event's textures, not the first verify line.

### 5.2 Results

| | StackOBot | Lyra |
|---|---|---|
| measurable / unique events | **60 / 138**, all one target, the MainWorld rock `T_rock_02_*` (64 SHALLOW, 14 short pre-roll) | **10 / 198** (188 SHALLOW) |
| static-camera measurable events | **43** (42 complete) | **1** (Lyra's camera moves on the others; no Lyra edge statistics) |
| onset: dominant half-depth step before the label | **1 frame: 43 / 43** | 1 (n=1) |
| onset: first departure before the label | 1: 30 · 2: 4 · **3: 7 · 4: 2** | 1 |
| captured frames after label end still below half-depth (42 complete) | **5: 16 · 6: 13** · 7: 4 · 8: 1 · 9: 3 · 10: 2 · 13: 2 · 14: 1 | >30 (n=1) |
| revert → first recovering frame (engine frames) | **+6: 24**, +9: 18 (+7/+8 not captured) | — |
| revert → half-depth recovered | +9: 16 · +10: 13 · +11–14: 10 · +18–19: 3 | — |
| revert → ALL of the event's textures `RESTORE VERIFIED` | **+7 on 42 / 42** | first +34, one texture unverified in 300 frames |

- ⚠ The SHALLOW majority is `G270`'s class (grids, holograms, flat paint and normal maps): no measurable in-region change at
  all. Those events can show neither edge, and they are not evidence that their edges are right.
- ⚠ 🔻 **"Half-depth" is a dominant-step diagnostic, not "last visible".** On F-H `lab 159..165` it puts the last positive at
  171 (six unlabelled saved frames). Codex's eye reading of "171 clearly detailed" was corrected by Codex itself: residual
  softness is visible at 171 on enlarged crops. The noise-band endpoint is not stable either: 192 crosses it, and 193–199 fall
  back below it. **The bank does not certify a single last-visible frame.**
- Lyra's restore spread from its logs, all cameras: all-texture verification from +1 to beyond +34 frames, and one texture
  needing 15 re-issued stream-ins over 166 frames. That spread is the useful Lyra reading.

### 5.3 Per-texture acknowledgement

- On most records every held texture's game-thread count flips on the same captured frame. On `G4_TEARDOWN2` si 73–76,
  for example, `S` goes 0.985 → 0.949 → 0.943 → **0.553**, then the label opens at 76 with all six counts at 7.
- 🔻 **Not universal:** the consult counts simultaneous first drops in **65 of 70** measurable records. In
  `M52C_M52C_G6_M52_NOPACE` si 45, four rock textures read 7 while `_D` and `_AORM` stay at 11 until si 51 (verified).
- A shared staged poll is a **plausible contributor** to the earlier departures. No per-texture render-thread trace exists,
  so it is not established.

### 5.4 Phase regularity (reading, not cause)

- 🔻 The F-H leg's pre-rolls are 19 (11 bursts), 17 (2) and 15 (3), not uniformly 19.
- Its early burst cadence is 35 engine frames = 5 × the nominal 7-frame streamer cycle, and all-texture verification is +7 on
  every static event.
- That regularity may be an **outcome** of the FSM waiting for the acknowledgement, not an independent phase lock.
- Either way, the bench shows a narrow band of latencies. **It cannot show the variance another host's cadence, load or
  content will show, and it has no correct-offset event.** This is `G135`'s shape.

## 6. Hypotheses

| # | Hypothesis | Status |
|---|---|---|
| O1 | Onset: the label reads a game-thread mirror acknowledged after the render-thread swap | **CONFIRMED** (source + bank, 43/43 main steps at label −1) |
| O2 | The lag is set by stage, slice, IO and queue, not a constant | **CONFIRMED as "not constant"** (1 dominant, departures to −4); per-texture slice causation **OPEN** |
| O3 | The office host's 2-frame onset is O1 at a different phase or texture mix | **OPEN** (no office data) |
| F1 | Offset: the window closes at revert, the picture at restore | **CONFIRMED** (source + bank, 42/42 complete events ≥5 frames below half-depth) |
| F2 | The revert-tick frame is labelled clean while fully held | **CONFIRMED** |
| F3 | What makes some office offsets look correct | **OPEN**: restore latency against uncaptured frames; imperceptible textures; F4 |
| F4 | `already-back` race (`Anomaly_StuckLowMip.cpp:719`) | source-supported, **UNMEASURED** |
| F5 | The creep is `MipBiasFade` | **WITHDRAWN** |
| F6 | The creep is temporal history | plausible, **UNTESTED** (no AA ablation, no matched long null) |
| M1 | Masks and `observable` follow the same window | **CONFIRMED** |

## 7. Part B — bench plan (corrected per consult P2)

**Build.** The archived m55-merge build is `master`'s code line:
- exe `_binary_baselines\StackOBot.exe.m55-merge-strip-E0BE6F0A`;
- the pre-m53 container `_binary_baselines\m53-s1-precook-container-67EA1FE0\` (project `pak`/`ucas`/`utoc` plus both
  `global.*`, present 2026-09-28).

Staging:
1. Hash the currently staged m53 S1 set (exe `2FCDF059`, utoc `20DA6F98`, ucas `534C5863`) against
   `StackOBot.exe.m53-s1rem-2FCDF059` and `m53-s1-maskedfix-cook-20DA6F98\`.
2. Copy the exe and the five container files over the staged set.
3. A44-scan the staged exe for a known m52 symbol before any leg.
4. After the legs, restore the m53 set and re-verify every hash. The m55-merge build must not be left staged while m53 S2 is
   paused.

**Premise audit, recorded per leg:**
- focus acquired at start (`start_frame` sane, not a focus timeout);
- camera: **total** excursion over the event span, translation and `view.rot`;
- streaming state at the decision frame, read back from the log: `r.TextureStreaming`, `UseAllMips`, `FramesForFullUpdate`,
  `OverlapAssetAndLevelTicks`, texture-quality scalability;
- run log on, so every `HOLD`, `revert` and `RESTORE VERIFIED` line exists;
- AA method echoed, delivery off, both tick orders where stated.

🔻 **Capture gaps are structural.** Settle frames and every phase-transition tick are not captured (`AnomalyCaptureSubsystem.cpp:722-724`,
`:762-765`). Even `K=0` leaves one uncaptured tick per transition. So "every frame captured" means **every frame the FSM
captures, with the gfc gaps declared per edge**. Where an edge falls in a gap it is reported as censored, not passed.

🔻 Size legs by **completed measurable events**. At post 30, a burst is about 19+8+30 = 57 saved frames, so 600 frames gives
about 10 bursts.

| Leg | Recipe | Tests | Reading |
|---|---|---|---|
| B0 | targeted on the MainWorld rock, `IAI.Capture.Config 2 4 8 30 0`, 1,200 frames, native order | O1, F1, F2 with a long post-gap; ≥15 complete events on one target | main step at label −1; ≥5 captured frames below half-depth after every complete label |
| B1 | B0 with `r.Streaming.FramesForFullUpdate 2` | O2 | lag and restore distributions **with the stage and slice recorded**. ⛔ A changed or unchanged distribution alone does not confirm or refute the slice mechanism (consult row 8) |
| B2 | `FramesForFullUpdate 8` with a burst cadence not tied to the cycle (e.g. post 29) | O2, §5.4 | the band of latencies across events, reported as an outcome |
| B3 | auto-pool, 1,200 frames, ≥3 targets with measurable texture detail (the rock, a detailed prop, a second rock), long post-gap | ≥20 measurable events across ≥3 targets | per-target lag and tail distributions |
| B5 | `IAI.Bench.StuckMipNoHold 1` on the B0 recipe | the null: no blur, no label | `S` in the noise band across the **whole** comparison window, not only early frames |
| B6 | forced HOLD TIMEOUT (`IAI.Capture.DeferredOnsetTimeout 3`) | F4 | only valid if the log proves a stream-out was **pending at revert**; otherwise it tests nothing |
| B7 | B0 in `IAI.Bench.SynthTickOrder` | order independence | same edges as B0 |
| B8 | a static-camera Lyra target with detail | second host, heavier streamer | lag and tail, if any Lyra target measures |
| B9 | AA ablation: B0 with `r.AntiAliasingMethod 0` | F6 | whether the creep is temporal history |

🔻 **Dropped:** "B4 amortized copies as a restore-delay lever". Those cvars throttle **manager-issued** load requests
(`StreamingManagerTexture.cpp:1341-1367`), while m52 calls `StreamIn` directly (`Anomaly_StuckLowMip.cpp:715`, `:844`). At
most it is a stress leg, not a lever.

**Minimum:** B0, B3 and B5 (≥20 events across ≥3 targets plus nulls). Then B7 and B9.

**Needs a code lever, so it belongs to the fix (§8):** a controlled delayed restoration (two textures, one restored late) and
a controlled delayed game-thread acknowledgement.

## 8. Draft fix design (NOT implemented; for chat's ruling; corrected per consult rows 11–13 and P1)

**Principle.** For `stuck_low_mip`, a captured frame's membership, its target mask, `stuck_mip.held` and `observable` come
from **one authority**: a render-thread receipt of the texture state that frame's own render used. The event outlives the
revert until a receipt shows every held texture back, for this restoration generation, with no anomaly-owned transition
pending. The game-thread mirror stays a **diagnostic**, labelled as such.

| File | Change |
|---|---|
| `Source/AnomalyInjector/Public/IAnomaly.h` | `virtual bool GetRenderReceiptTextures(TArray<FAnomalyReceiptTexture>&) const { return false; }`. Per texture: a strong object reference, event id, restoration generation, and a baseline taken **from a pre-Apply render receipt**, not the game-thread count. The default is false, so every other anomaly is untouched. |
| `Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.{h,cpp}` | Report `Held` and `Restoring` per event. Keep a restoring record until a render receipt says baseline for this generation. Keep tracking when a stream-out is pending, even if the cached count reads baseline (the `already-back` race). A restore timeout **continues** as an explicit unresolved outcome. |
| SVE pass (`AnomalySceneViewExtension.cpp` / `AnomalySveCapturer.*`) | In the matched family's `AfterPass_RenderThread` (`:143-184`), before submitting that request (`:235-237`): resolve each texture's resource **on the render thread**, and record `GetStreamableTextureResource()->GetState()` resident and first mips. `GetState()` is RT-coherent (`StreamableTextureResource.h:37-42`). Return them with the RGB result keyed by request id, run epoch and family. A null, uninitialised, replaced or unsupported resource returns **unknown**, never zero and never "restored". The partially-resident (virtual) path has setters only, no verified getter (`Texture2DStreamOut_Virtual.cpp:35-43`, `DynamicRHI.h:983,991`), so it is **explicitly unsupported** until a transition record is proven. |
| `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` | (a) Attach the receipt textures to the snapshot at arm time. (b) Decide membership at drain time from the receipt: ANY held texture below baseline opens, ALL back closes. Membership is per frame, so holes inside an event stay holes. `stuck_mip.held` and `ConditionHeld`/`observable` read the same result. (c) Keep a **trailing** event after `BeginRevert` in the frame's fire list until the receipt closes it. (d) A **restore-settle gate** stops the next `BeginFire` while a trailing event is open, and **keeps capturing** meanwhile (a captured phase, not `SettleAfterRevert`). A timeout ends the run or refuses further fires with an explicit unresolved outcome; it never silently labels a still-anomalous picture clean. (e) **Pre-arm** the target mask for a deferred-onset or trailing event on every captured frame from Apply until the receipt closes, and keep masks only where membership is true (m44's "mask only on a labelled frame", kept at write time). (f) Missing or stale receipts never fall back to the mirror. |
| `Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp` | Per-texture receipt fields in `stuck_mip.textures[]` (additive), with the game-thread fields renamed or marked as the mirror. The `annotation.json` field set is unchanged (P6). |
| m26 veto (`AnomalyMaskMeasure.cpp`) | Its arm budget counts receipt-held frames (080-06's gate kept, re-keyed). |

**Contract limits, declared.** The receipt proves **available mip state**. It does not prove the sampled mip, the material's
contribution, or final-RGB history. A distant surface already samples below the held mip, a flat texture looks identical
(`G270`), and temporal history can outlive the final restore. So the fix is a **residency-window** fix. "Visible" stays a
separate reading and gets its own gate.

**Gates.**
- **G-RES (residency window).** A controlled single-texture target with stable detail and no temporal accumulation (AA off),
  both tick orders: first member == first frame whose pixels step, and last member == last frame before the pixels restore.
  Plus a **two-texture** case where one restores late (ALL-to-close), and a **delayed game-thread acknowledgement** case
  (membership must follow the render transitions, not the mirror).
- **G-VIS (visible edge, reported and gated separately).** A fixed interior ROI, matched same-view null pictures, and a
  significance threshold calibrated on independent nulls **before** looking at candidate edges (e.g.
  `max(5σ_null, 0.02 × S_null)`). The last positive is the frame before three consecutive clean, continuously captured
  frames. An edge in a capture gap, or with no stable clean suffix, is **censored**, not passed. ROI, reference and search
  interval are never recomputed from the candidate's labels.
- **Can-fail.** Offline metadata corruptions on the same RGB (onset +2 pictures, offset −5 pictures, label and mask moved
  together) must FAIL both gates. A bench lever restoring today's timing (mirror onset, close at revert) must FAIL on B0.
  B5 null.
- **Lifecycle.** Missing or out-of-order receipts, resource replacement, early revert with a pending stream-out, restore
  timeout, target loss and capture cutoff each give their declared outcome.
- **Standing set.** ONSET, MASK-PICTURE-PAIRING (m51 remains an independent obligation), P-C7 in both orders, and the `G270`
  observable reading.

**Other anomalies.** Only `stuck_low_mip` overrides `HasDeferredOnset` (`Anomaly_StuckLowMip.h:22`; default false,
`IAnomaly.h:34`), and only it has a restore queue, so the fix is scoped to it.
- **Named, not tested:** `lod_popping`, which labels a requested forced-LOD phase (`Anomaly_LodPopping.h:20`,
  `.cpp:246-275`). A streamed or skeletal LOD transition could lag the same way.
- m53's generated textures, on the paused branch, were not examined.
- Material swaps (`corrupted_texture`, `missing_texture`) and hides take effect in the same frame's render (m18/m40/m44
  gates), consistent with the owner's clean `corrupted_texture`. That control does not by itself retire m51.

## 9. Codex and the reconciliation

- **Codex 084-01** (independent; Astra/max, 1,581 s): **ROOT-CAUSE-ESTABLISHED**.
- **Consult 084-01b** (1,044 s): **AGREED-WITH-CORRECTIONS**.

### 9.1 Reconciliation table

| # | Question | Claude Code | Codex | Result |
|---|---|---|---|---|
| 1 | onset signal | GT mirror, acknowledged after the RT swap | same | **agree** |
| 2 | offset signal | fire removed at `BeginRevert`; revert-tick frame clean | same | **agree** |
| 3 | masks | same predicate over `LiveFires` | same, plus m26 and `observable` | **agree** |
| 4 | bank onset | main step at label −1 (43/43 static); departures to −4 | D/E si 75 at label −1 against an exact null; departures si 72–74 | **agree** |
| 5 | last visible frame | ≥5 captured frames below half-depth; creep to ~R+30 | first said 171 "clearly detailed"; consult: CANNOT-DECIDE, half-depth last positive 171 (6 frames) | **half-depth agreed; last-visible undecided** (§5.2) |
| 6 | creep = `MipBiasFade` | withdrawn | agree to withdraw | **resolved: withdrawn** |
| 7 | onset ≥1 guaranteed | claimed | disagree: same-poll race | **Codex upheld; claim withdrawn** (§2.3) |
| 8 | all flip together; constant phase | claimed | disagree: 65/70; F pre-roll 19/17/15 | **Codex upheld; claims narrowed** (§5.3–5.4) |
| 9 | separator for correct offsets | OPEN | OPEN | **agree** |
| 10 | `already-back` race | agree, unmeasured | source-supported | **agree** |
| 11 | retention vs next burst | restore-settle gate | agree for this scope; must keep capturing; timeout must not manufacture clean frames | **agree, corrected** (§8 d) |
| 12 | receipt read point | SVE-pass `GetState()` | valid, with RT resolve, generation, identity join, virtual unsupported | **agree, corrected** (§8) |
| 13 | gates | pixel-edge equality, two levers, null | insufficient; residency ≠ visible; add metadata corruption, delayed restore, delayed acknowledgement | **Codex upheld** (§8 gates) |
| P1 | one authority for the window | not covered | `stuck_mip.held` and `ConditionHeld` stay on the mirror | **adopted** (§8) |
| P2 | census defects (`rot` key, adjacent-step metric, first-verify join); plan executability | — | found | **adopted and re-measured** (§5, §7) |

## 10. State

- Branch `fix/m52-label-timing` off `master` `b5f15a3`: this journal and `G343`. No tag, no merge.
- The main checkout was left on `m51` (`53bf725`), untouched. The docs were written in a doc-only worktree at
  `D:\IntrusiveAnomalies\_wt_084_01_m52` (shared-tree rule 2), outside `Plugins/`.
- Bank, bench, staged binaries, m53 S2 plan, harness and lock: untouched.
- Relay rows `084-01-m52-diagnosis` and `084-01b-m52-consult` are in `_relay/ledger.csv`.
