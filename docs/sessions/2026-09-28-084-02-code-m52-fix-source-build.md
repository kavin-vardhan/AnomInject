# Session 084-02 — `m52` label-sync fix: source, builds, offline proofs, the part B harness, and a Codex source review

**Brief 084-02, Claude Code (Opus 5.5), headless, 2026-09-28.** Spec: `_reviews/084-01-chat-ruling-m52-fix-contract.md`
(ruling items 1–4). Governing rule: `085-00` ADDENDUM — label sync is the product.

⛔ **No game launch, no staging, nothing that took focus.** The staged m53 set (`2FCDF059` / `20DA6F98` / `534C5863`) was
hashed and left untouched. The main checkout stayed on `m51` `53bf725`. `master`, tags, `ToCodex\`, `E:\AmmaYT` and the m53
S2 files were not touched. Compiling happened on a fresh scratch host, `E:\IA_BuildCache\_r84_host\StackOBot`, whose plugin
is a branch worktree of `fix/m52-label-timing` (runbook §8.6a pattern, but with **no `Content` junction**: nothing on this
host can write the real project).

## 0. The answer

- **The label window now follows the render thread, not the game thread.** For every captured frame of a `stuck_low_mip`
  event, the colour SVE pass reads each held texture's `FStreamableTextureResource::GetState()` inside the same render
  command that drew the frame. The frame is labelled iff any held texture was drawn with fewer resident mips than its
  baseline. The label, `stuck_mip.held`, `observable`-eligibility, the annotation window and the delivered target mask all
  come from that one record.
- **The event outlives the revert.** A restore trail keeps the event attached to every captured frame after `Revert` until
  a record shows every held texture back at baseline on 2 consecutive frames, plus `IAI.StuckMip.SettleTailFrames` (default
  0). The trail is a captured phase (`RestoreTrail`), so no frame of it is skipped, and it gates the next burst.
- **Timeout never labels clean.** After the restore timeout (the existing `IAI.Anomaly.StuckMipRestoreTimeout`, default 120,
  counted in captured frames) the event is marked `restore_unresolved`. Further m52 fires are refused with that reason for
  the rest of the run, and the target is reserved. The trail stays attached, so every still-held frame keeps its label.
- **Wrong object: fixed by whole-loaded-world purity.** A texture is held only if it has exactly one user component in the
  whole loaded world. Every component type is counted (all primitives, including ISM/HISM, landscape and foliage, plus
  decals), and visibility is not consulted. The rule applies to targeted fires too.

## 1. The wrong-object cause, from source (`master` `b5f15a3`, before any change)

The co-affected gate is `BuildVisibleTextureUserCounts` (`Anomaly_StuckLowMip.cpp:118-144`), consulted at `:406-417`. It
counts a texture's other users only among:
1. **the renderable-visible actors at fire time** (`AnomalyViewport::GetVisibleRenderableActors`,
   `AnomalyViewport.cpp:944-966`): in the frustum, not occluded (9-ray first-clear-ray test), within the 18 m poll radius of
   the player pawn, and at or above the 6 % screen-coverage cull; and
2. within those actors, **only components passing `IsRenderableComponent`** (`AnomalyViewport.cpp:653-...`): visible, not
   on an `AInstancedFoliageActor`, not matched by `ExcludedTargetNamePatterns`, ISM with instances, static or skinned only
   (`G33`).

In addition:
3. **Targeted fires bypass the gate entirely** (`MaxCoAffected = TNumericLimits<int32>::Max()`, `:292`).
4. **The count is taken once, at `Apply`.** Nothing re-checks during the hold.

So a texture shared with a user that is off-screen, occluded, beyond 18 m, under 6 % coverage, hidden, foliage,
pattern-excluded, or a non-static/skeletal component (landscape, decal) is held with `co_affected_visible=0`. That blurs
the other user while the label and mask name only the target. **This matches the owner's glove/wall reading**: mask on
the glove, blur on the wall.
- ⛔ **Which of these predicates excluded the office wall is NOT established.** There is no office data. The mechanism
  class is confirmed from source; the office instance is not.
- **Bench check (name-table evidence, not a measurement):** the bench rock's three held textures `T_rock_02_{D,N,AORM}` sit
  in `MI_rock_02`, used only by `SM_rock_02`, which is placed exactly once in MainWorld (1 of 1,624 external-actor files).
  The rock therefore passes the new rule, and part B can still fire on it.

## 2. The fix, file by file

| File | Change |
|---|---|
| `AnomalyInjector/Public/AnomalyStuckMipWindow.h` (new) | The pure logic, plain C++ with no UE dependencies: per-texture state from a record, `Combine` (ANY held opens; unknown without held is **unknown, never clean**; no watched texture is `BeforeApply`), per-frame membership, the trail state machine (tail, 2 confirming frames, timeout, gating) and `ClassifyPurity` (exactly one user component, and it is a target component). Compiled into both modules and into the standalone unit test. |
| `AnomalyInjector/Public/IAnomaly.h` | `FAnomalyRenderTruthTexture`; `UsesRenderResidencyTruth()` and `GetRenderTruthTextures()`, both default false. |
| `AnomalyInjector/Public/AnomalyInjectorSubsystem.h`, `.cpp` | Passthroughs; a named per-id refusal checked first in `ApplyAnomaly` (logs `REFUSED <reason>`, records no fire); a reserved-actor set. |
| `AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp` | `IsActorLive` also returns true for a reserved actor, so nothing fires on an unresolved m52 target. |
| `AnomalyInjector/Public/AnomalyStuckMipStats.h` | Additive counters; the legacy-purity lever getter and setter. |
| `AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.{h,cpp}` | **Purity:** one pass over every loaded actor's components (`TActorIterator`, every `UPrimitiveComponent` plus `UDecalComponent`, visibility ignored), timed and logged; refusal `shared_world` names up to 6 users. **Baseline guard:** refuse a texture whose stream operation is in flight at `Apply` (`baseline_pending`), because its game-thread count may not be the drawn count. **`already-back` race** (084-01 F4): a texture that reads baseline but is still busy at revert is now tracked, and `RESTORE VERIFIED` also requires `!HasPendingInitOrStreaming()`. **Render-truth accessor:** the held textures with their baselines. Telemetry gains `world_users`. |
| `AnomalyCapture/Private/AnomalyFrameCapturer.h` | `FAnomalyRenderMipSample` (resident, first mip, status, resource id); `FAnomalyCapturedFrame` carries `RenderMips` and `bRenderRecord`. |
| `AnomalyCapture/Private/AnomalySveCapturer.{h,cpp}`, `AnomalySceneViewExtension.cpp` | A watch list per request id is stored at `ArmWanted`. In `AfterPass_RenderThread`, for the wanted request only, `TakeRenderWatch_RenderThread` plus `SampleRenderMips_RenderThread` read each texture's RT resource (`UTexture::GetResource()` on the RT) and `GetState()`. The record rides the in-flight item to the drained frame. Null or uninitialised resources, non-streamable resources and **partially resident (virtual) resources report a status, not a count**, and classify as unknown (Codex 084-01b row 12). |
| `AnomalyCapture/Private/AnomalyLabelWriter.{h,cpp}` | The snapshot carries the watch list and trailing flags. `run_summary` gains `stuck_mip_label_source`, `_render_*`, `_trailing_*`, `_settle_tail_*`, `_trails_*`, `_restore_unresolved*`, `_gt_mirror_disagree_frames`, `_mask_deferred_dropped`, `_refused_shared_world`, `_refused_baseline_pending`, `_restore_tracked_while_busy` and `_purity_enumeration_ms_max`, **only on runs that fired m52**. |
| `AnomalyCapture/Public/AnomalyCaptureSubsystem.h`, `Private/AnomalyCaptureSubsystem.cpp` | Detailed below. |

**In the capture subsystem:**
- **Arm:** `BuildRenderWatch` collects the live m52 event's held textures plus any attached trail's textures, keeps them
  alive for the run (`TStrongObjectPtr`), and passes them with the request. A frame with no m52 event arms no watch.
- **Label:** `FinalizeArmedLabel` appends the trailing fire to the frame's fires.
- **Drain:** only for a batch that carries a render-truth frame, the batch is ordered by request id. Each frame's per-event
  verdict and membership are computed once (`ComputeRenderMembership`; trailing frames step the trail in SI order).
  `ApplyRenderTruthToSnapshot` then overrides `FireActive`, `FireLabelled`, `ConditionHeld` and `stuck_mip.held`, and adds
  `stuck_mip.held_gt_mirror`, `.render_state`, `.trailing`, `.label_source` and `.render_textures[]`. The game-thread
  per-texture fields stay as diagnostics. An unknown frame is labelled with `observable: null`.
- **Masks:** `ArmTargetMaskOwn` pre-arms every captured frame of a live or trailing m52 event and records its tags as
  gated. `ServiceTargetMask` holds a gated mask until its frame's record exists, then strips any non-member tag before
  the PNG, counts, bounds and m55 mask are produced. A mask whose record never arrives is dropped after 16 ticks and
  counted.
- **m55:** `Observe` is queued in SI order for frames with an m52 event, patched from the record, then flushed. Frames
  without m52 still call `Observe` directly. `EndEvents` is deferred from the revert to the trail's closure.
- **FSM:** `BeginRevert` opens a trail and enters `RestoreTrail`, which captures every tick and releases `PostGap` only
  when the trail closes or goes unresolved. `ServiceStuckMipTrails` handles closure, timeout, refusal and reservation, and
  ages deferred masks and queued observes.
- **Run start:** the label source is decided and echoed (`=== Capture(m52): EFFECTIVE FOR THIS RUN ...`). m52 is refused
  with `no_render_record` if the run is not SVE and async. Refusals and reservations are cleared at start and end.
- **Kept on the game-thread mirror, deliberately:** the deferred-onset scheduler (when the positive budget starts) and the
  m26 veto's arm budget. Both are scheduling or measurement budgets, not label inputs. The m26 silhouette count does not
  depend on mips.

**The levers:** both register only when `-IAIBench` is on the command line. They are registered at runtime in
`Initialize`, because static init runs before the command line exists, and they sit inside `#if ANOMALY_CAPTURE`, which is
`0` for Shipping (`AnomalyCapture.Build.cs:340-359`).
- `IAI.Bench.StuckMipLegacyTiming <0|1>` restores today's timing for the next run: the game-thread mirror, the event ending
  at revert, and `SettleAfterRevert` uncaptured. It is the sync gate's can-fail.
- `IAI.Bench.StuckMipLegacyPurity <0|1>` restores the visible-only co-affected rule and the targeted bypass. It is the
  purity gate's can-fail.
- Also under `-IAIBench`: every m52 fire logs an **independent** texture-user enumeration
  (`Capture(m52-bench): TEXUSERS ...`). It uses a different traversal from the product: `TObjectIterator` over components
  and decals, then `GetUsedMaterials` and each material's `GetUsedTextures` across all quality and feature levels.

## 3. Builds

- **Host:** `E:\IA_BuildCache\_r84_host\StackOBot`, created for this brief:
  - copies of the real project's `StackOBot.uproject`, `Source/` and `Config/`;
  - `Plugins/AnomalyInjector` = a **branch** worktree of `fix/m52-label-timing`;
  - CaptureBench = `git archive HEAD` (`2c5ec56`; its C++ is unchanged since 2026-08-16);
  - `unreal-mcp` copied, and RoomGenerator's `.uplugin`;
  - **no `Content` junction**, so nothing on this host can write the real project.
  - The m53 host `_r53_host` was not touched.
- **Game prewarm** (engine modules on the unchanged source, started first): 763 actions at 2 processes (UBT capped at
  ~2.4–4 GB free RAM), 5,412 s. It ended exit 6 because the plugin sources were edited while it ran and UHT had generated
  from the pre-edit header (stale `GENERATED_BODY` line: C2509/C2143). That is an artefact, not a defect (§9, G344).
- **Game `StackOBot Win64 Development`: exit 0**, 5 actions (AnomalyCapture, ControlServer, Bench, link), 85 s.
  - `StackOBot.exe` = **`562EE7A4`** (`562ee7a4994c2126…80ededee`), 241,835,520 B.
  - **A44 (UTF-16 scan):** every new token is present, including the `AnomalyInjector`-side ones compiled during the
    prewarm (`shared_world`, `baseline_pending`, `PURITY ENUMERATION`, `TRACKED rather than counted already-back`). The
    scan is sound, not blind: `IAI.Bench.StuckMipNoHold` 5 and `IAI.Capture.TargetMask` 6.
- **Editor `StackOBotEditor Win64 Development`: exit 0**, 28 actions, 84 s. It recompiled `Module.AnomalyInjector.cpp` from
  the final source.
- **Warnings: 0 in plugin code.** The only compiler warnings in either target are two pre-existing C4996 deprecations in
  the host project's own `Source/StackOBot/MidReproActor.cpp` (m17's validation actor), which is not plugin code and not
  touched.
- **Comment strip:** `_strip_comments.py` over the worktree: 0 of 116 files changed.
- **Diffstat:** 17 files, +1,928/−30. `AnomalyCaptureSubsystem.cpp` +961/−7; all 7 deletions are intentional replacements
  (G115 check: no encoding rewrite).
- **Archive:** `_binary_baselines\m52fix-562EE7A4\` (exe, both build logs, README). **Nothing was staged**: the m53 set
  `2FCDF059` / `20DA6F98` / `534C5863` (plus `.pak` `fd766b7b` and `global.*` `462b8ac6` / `bb05cf99`) was hashed and left
  in place.

## 4. Offline and unit proofs

- **Unit (`tools/m52_window_selftest.cpp`, MSVC `/W4 /WX`, standalone):** **41 checks, 0 failures.** Covered:
  - texture classification and `Combine` (ANY-held, ALL-baseline, unknown is never clean, no watch means before-apply);
  - **onset** against a synthetic mirror lagging by one frame (the render window equals the render-held frames; the mirror
    opens one frame late, the defect reproduced);
  - **offset** with tail 0 (revert-tick plus five held trailing frames labelled; closes after 2 consecutive baseline
    frames);
  - tail 3;
  - a flap and an unknown frame;
  - **timeout and gating** (unresolved stops gating, stays attached, still labels 40 held frames, and a late restore
    closes it);
  - **purity** (exactly one; two other users; the off-screen user, i.e. the glove/wall case; two target components; no
    users; and the legacy rule firing the off-screen-shared texture while the new rule refuses it).
- **Non-m52 byte identity: source-level argument, not a replay.** A replay needs the engine. Every new path is keyed on
  a render-truth fire, a render record, a trail, a deferred mask or a queued observe, and each is empty in a run that
  never fires m52. §5 lists each hunk with its guard. The batch sort, the one hunk that could have reordered output on any
  SVE run, was narrowed to batches that carry a render-truth frame before the build. New `run_summary` keys appear only on
  runs that fired m52. The only unconditional change is one extra log line per run.

## 5. Non-m52 identity — hunk by hunk

"No m52" means no `stuck_low_mip` fire in the run. `bRenderTruthRun` is true on every SVE + async run, so each hunk is
guarded by something narrower than that flag.

| Hunk | Guard | In a run without m52 |
|---|---|---|
| `IAnomaly` virtuals | default `false` | other anomalies unchanged |
| `ApplyAnomaly` refusal | `RefusedIds` is set only for `stuck_low_mip` (`no_render_record`, or `restore_unresolved`), and cleared at start and end | never consulted for another id |
| `IsActorLive` reservation | set only on an unresolved m52 trail, cleared at start and end | empty set, identical |
| m52 anomaly changes | m52 only | not executed |
| `ArmWanted` watch | `BuildRenderWatch` returns textures only for a live or trailing m52 event | `nullptr`, no RT read, empty `RenderMips` (never serialised) |
| Batch sort in `ProcessCompletedFrames` | only when the batch carries a render record, an armed watch, a trailing flag or an m52 fire | **no sort**; the reverse drain order is kept (narrowed before the build) |
| `ComputeRenderMembership` | per frame, iterates only render-truth fires | no map entry |
| `ApplyRenderTruthToSnapshot` | returns early without a map entry | not applied |
| Observable override | render-truth fire | not entered |
| `FinalizeArmedLabel` trailing flags | appended only for attached trails | zero array, never serialised |
| Tick-end held count | skipped when `bRenderTruthRun`, counted at drain for m52 instead | the key never exists without m52: 0 either way |
| m55 `Observe` queue | gated labels or a non-empty queue | direct `Observe`, identical |
| `ArmTargetMaskOwn` | trailing fires appended only for attached trails; `bGated` false otherwise | identical |
| `ServiceTargetMask` | deferred and gated maps are empty | identical path |
| `BeginRevert` | `OpenStuckMipTrails` opens only for m52 | `SettleAfterRevert` as before |
| `BeginFire` TEXUSERS | `-IAIBench` and an m52 fire | not executed |
| `run_summary` keys | `FiresApplied > 0` or trails or records | field set unchanged |
| Run-start echo, `-IAIBench` registration line | — | **log only**: one extra line |

The argument is by construction, not by measurement. Part B will not re-measure non-m52 labels, because every one of its
legs fires m52. The label-sync audit of the other anomalies (085-00) is the natural place to re-verify it.

## 6. The part B harness (`_reviews/084-03-*`)

The full description, invocation and predeclared expectations are in `_reviews/084-03-README.md`. In brief:
- **Reused from S1:**
  - the launcher (`084-03-run.ps1` = `082-07-run.ps1` plus one `-ExtraArgs` parameter);
  - process classification, the quiet check, the person-present gate, banking with manifest verification, and hardlink
    aliases (`M52FIX_<leg>`).
- **Staging:**
  - back up the m53 set (all six files) to `E:\IA_BuildCache\_r84_m53_staged_backup` and verify it;
  - stage BASELINE (`E0BE6F0A` plus `67EA1FE0`), then FIX (`562EE7A4` plus `67EA1FE0`), with a hash receipt for each;
  - restore M53 in a `finally` block, verified.
- **Legs:** B0, B5 and B3 on both builds; B9 on the baseline; B0L, P0 and P1 on the fix; optional B7 on both and B9 on the
  fix.
- **Gates:** (i) sync from pixels, (ii) render-record window == label == mask (fix), (iii) purity from the independent
  TEXUSERS lines (fix). Can-fail: doctored labels (offline), the timing lever, and the purity lever.
- **The pixel oracle was rebuilt twice during the dry run, and both changes are measurements:**
  1. A flat null μ/σ is dominated by MainWorld's camera ease (first ~25–85 frames) and by a slow game-time drift of ±7 %.
     The null's σ read 6 % of μ, and no event was measurable.
  2. A pre/post-detrended reference misreads the streamer's own long re-sharpening as blur. On F-H event 577 the rock
     sat at ~3.9 before the fire and climbed to ~4.8 over ~150 frames afterwards with no hold active, which gave a false
     12-frame-early onset.

  The final oracle uses the **flat pre-event level**, with the threshold = `max(5σ_resid, 2 %, null worst drop)`. It
  reproduces the diagnosed baseline shape on banked F-H event 577: first visible 507 vs label 508 (1 early), last visible
  521 vs label end 514 (7 unlabelled). → **G344**.
- **Dry run** (`084-03-window.py dry`: stubbed launch and staging, banked stand-ins D-H, D-N and F-H):
  - preflight clean with the real fix archive; receipts written; legs banked to a stub bank;
  - B5 null PASS (worst drop 2.73 %);
  - B3 stand-in: (i) FAIL, 7 unlabelled after, onset 1 early;
  - **doctored-label can-fail PROVEN on real pixels**: the pixel-repaired window passes; onset +2 and offset −5 both fail
    (i) and (ii).
  - The purity parser was proven both ways on known-answer lines built from the source's log formats: pure → PASS,
    shared-held → FAIL, refused → PASS-REFUSED, nothing → NO-ENUMERATION.

## 7. Codex source review

The Relay run `runs\2026-09-28-084-02-m52-fix-review` (Astra, max, 933 s, exit 0; `run-meta` agrees with the request)
was collected unedited to `_reviews/084-02-codex-m52-fix-review.md`, with a ledger row. It reviewed git objects
`6405efb..67afa94`. **VERDICT: CHANGES-REQUIRED.** **Collected, not acted on** (Relay rule: chat rules).
- **F1 (P1):** the trail closes on 2 baseline receipts without a restoration fence. A stream-out still pending after an
  early revert can land after closure and be drawn detached from the event.
- **F2 (P1):** sorting one drain batch does not make trail updates sequential. Split RT publication gives a
  baseline / late held / baseline order that closes early; `bClosed` never reopens; a dropped SI does not break
  `CleanRun`.
- **F3 (P1):** "no watched texture ⇒ before_apply ⇒ out" is an unproven invariant. An arm on the last LeadIn or PostGap
  tick is served by a later family after the drop, and the anomaly going inactive outside `BeginRevert` leaves an empty
  watch.
- **F4 (P1):** purity skips inactive-but-loaded levels (`TActorIterator` default `OnlyActiveLevels`) and unregistered
  components. It is not maintained after `Apply`. TEXUSERS shares the registration blind spot, and the harness does not
  join enumeration lines to each event and texture.
- **F5 (P1):** unresolved restoration ownership is dropped at `FinishRun` and `StartRun`; a frame-cap cut in a live
  window is not counted.
- **F6 (P2):** a forced m55 `Observe` submits a provisional false, and `BeginClosure` before the drain discards late
  queued observations.
- **F7 (P2):** a deferred mask's age resets on every service, so the 16-tick drop never fires.
- **Shipping:** the lever registration and TEXUSERS guards PASS. The legacy-purity storage and branch are compiled
  unconditionally in the injector module (no Shipping console route, but not literally excluded).
- **Render hook (question a):** sound for the captured family on the ordinary resource path. The identity joins are
  inherited, and a resource replacement is not detected (`ResourceId` recorded, not used).
- **Harness:** a synthetic counterexample passes gates (i) and (ii) with four clean frames labelled and masked after
  recovery via `settle_tail`. Gate (i) does not reject over-labelling past the last visible frame, and the doctored proof
  works on in-memory maps rather than rebuilt mask PNGs.
- **Question (e):** no intended dataset change without m52 activity; literal log output differs (one line).
- Also noted: `CLAUDE.md` was not in the reviewed commit; it is in this docs commit.

## 8. Limitations, named

- **A texture user that streams in during an event** is not seen by the `Apply`-time enumeration. World Partition can load
  a cell after the fire. It is not detected in this build.
- **Available mip, not sampled mip.** The record proves which mips were resident when the frame was drawn, not which mip
  each fragment sampled or how temporal history mixed them (Codex 084-01b row 12). Visibility is reported separately
  (m55, `observable`), and part B's pixel gate measures it.
- **The deferred-onset scheduler still keys on the game-thread mirror**, so the 8-frame positive budget starts at the
  mirror's acknowledgement. The label does not.
- **The settle tail is 0 until part B measures the post-restore residual** (ruling item 1).

## 9. State and hand-off

- **Branch `fix/m52-label-timing`:** `67afa94` (the fix) plus this docs commit, pushed. **No tag, no merge.** `master`, `m51`
  (`53bf725`), tags, the m53 branch and its host, `ToCodex\` and `E:\AmmaYT` were not touched.
- **Scratch host `E:\IA_BuildCache\_r84_host\StackOBot` is KEPT warm** (about 17 GB). Its plugin is the branch worktree.
  A plugin-only rebuild is about 85 s.
- **The UHT artefact** (G344 side note): the prewarm's exit 6 came from editing headers while it built. The final Game
  and Editor builds reran UHT and exited 0.
- **Part B (084-03):** `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live`. Add
  `--with-optional` to include B7 on both builds and B9 on the fix. Expected 45–60 min, up to about 90 with retries. It
  stages BASELINE, then FIX, and always restores and verifies the m53 set.
  - ⚠ **Chat decides first**, given Codex's CHANGES-REQUIRED: whether part B runs tonight on `562EE7A4` as the
    ordinary-path measurement (baseline vs fix, plus the settle-tail number), or F1–F7 and the gate contract are
    corrected and rebuilt first.
  - Codex's counterexample (gate (i) does not reject over-labelling after recovery) matters for either choice.
- **Not done here by design:** no leg ran, so the fix has no runtime evidence yet. The settle tail stays 0. Codex's
  review is collected, not acted on.
