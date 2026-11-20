# 078-17 independent source checks (no execution)

Read-only verification while Code owns the single documentation brief. This is supporting evidence, not acceptance of an unfinished revision. No checker, build or physical leg ran.

## Actual engine and subsystem paths

StackOBot.uproject EngineAssociation {B34F356C-4AE7-256A-F0E1-318A632BB902} maps through HKCU Software/Epic Games/Unreal Engine/Builds to D:/UESource/UnrealEngine. CLAUDE.md conventions identify source-built UE5.1 there. All engine ranges below are relative to D:/UESource/UnrealEngine/Engine/Source/Runtime. The dispatched brief's E:/EpicGames/UE_5.7 hint is incorrect; Code's live feed shows it independently checking the5.1 paths. Brief's AnomalyWorldSubsystem.cpp filename is also incorrect: actual Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp, whose line ranges match. These errors are ours, not source drift. Do not use UE5.7 reads as project proof.

## Publication and copies — R1

- Engine/Private/SceneViewExtension.cpp:118-126 sorts descending GetPriority, higher first.
- Renderer/Private/SceneRendering.cpp:4553-4598 assigns scene-local shared family numbers, calls GT extensions, then creates renderers. FViewFamilyInfo copies FSceneViewFamily at2401; FSceneRenderer copies the family at2417 and creates copied FViewInfo views at2450, assigning new Family pointer2453. FViewInfo constructor819 copies FSceneView then calls Init. None is a specified plugin identity carrier.
- Plugin AnomalySceneViewExtension.cpp:49-65 takes colour FIFO request then publishes; mask header BeginRenderViewFamily is empty and mask RT body batches all pending. Actual activation predicate is .cpp, not the header declaration.
- AnomalyCaptureSubsystem.cpp:4199-4234 snapshots frame counter/session index and calls colour ArmWanted inside CaptureCurrentFrame. OnWorldTickEnd:830-865 separately gates target arms on TargetMaskArmedTick equality, arms normal, target and census. Proposed common freshness/publication is not already implemented by this arrangement.
- Needed publication seal/expected membership, duplicate lifetime, generation/context lifetime and copied-family RT carrier remain design obligations. Current code does not justify them automatically.

## Lifecycle and physical ownership — R2

- AnomalySceneViewExtension.cpp:93-105 lookup miss returns before readback allocation147, AddEnqueueCopyPass149, SubmitInFlight159. Invalid capture inputs/rect/extent can return before these too. A rendered image is not proof of plugin readback submission.
- AnomalySveCapturer.cpp:135-155 SubmitInFlight records pipeline bookkeeping after pass scheduling. Its Submits counter does not prove the later GPU enqueue executed. Matches minus Submits is a raw boundary-sensitive counter difference; non-consuming lookup/repeated views and outstanding work prevent unconditional request reconciliation.
- RenderCore/Private/RenderGraphUtils.cpp:842-879 AddEnqueueCopyPass calls GraphBuilder.AddPass and captures raw Readback; the later lambda calls EnqueueCopy. Reservation/allocation, scheduled pass, GPU enqueue and retirement require distinct ownership/lifetime treatment.
- Frozen release-hardening A2.1 logical capacity release at FIRST terminal must stand independently of physical charges. Tag fire-ended AND physical user resolution remains from A1.6.
- A late notification on a terminal request cannot be another terminal. Monotonic issuance does not imply ordered retirement or justify dropping per-index records/live references behind a watermark.
- CPU payload ownership is wider than RGB writer jobs: mask reduction/pixels shared among consumers; AnomalyCensus.cpp:797-802 can request pixels. Outstanding terms or quiescent scope are needed for physical balances.

Additional full callback trace during snapshot review: do NOT generalize that census never has a writer job or writes an image/row. AnomalyCensus.cpp:576-608 handles the optional requested pixels and calls OnMaskDump.Execute. AnomalyCaptureSubsystem.cpp:3417-3425 binds that callback to EnqueueCensusMaskDump when the non-delivery bench setting is enabled;2044-2058 constructs a gray-mask FAnomalyAsyncWriter::FJob, writes to census_mask/tick_<ArmTick>.png through Writer->Enqueue, and2061-2074 appends census_mask_map.jsonl. Ordinary reduction-only census can resolve without image work; the optional bench dump has both a writer job and map append. Shared payload accounting must include both. This is a source-backed correction to the draft's blanket wording under existing R2, not a new runtime observation or authorization to patch writer code.

## Rendered geometry — R3

- Engine/Private/LevelTick.cpp:1110-1199 invokes deferred component updates, possibly parallel plus GT work.
- Engine/Private/Components/PrimitiveComponent.cpp:619-632 runs UpdateBounds and calls Scene UpdatePrimitiveTransform for eligible components.
- Renderer/Private/RendererScene.cpp:2194-2202 captures bounds/transform/proxy in UpdateParams;2248-2264 ENQUEUEs UpdatePrimitiveTransform_RenderThread. This is a source-backed update/order path, not synchronous arbitrary GT/proxy equality.
- Primitive/event/component registration lifetime, actual render-time observation, final RT view/jitter/rect correspondence, output resize transforms/clipping and independent geometry oracle collection/join remain open. Moving a GT sample later does not resolve these or authorize changed box semantics.

## Preserved evidence and gate scope — R4/R5/R6/R7

- Request-ID summary401057FA…DC1EB and payload outcomes0533C275…B69F5 pinned in supplemental JSON. Three joined second-target payload losses per failing run remain; PixelOwner ordinal is not render identity. Same-render sharing remains legitimate.
- Near-depth term affects Clip.Z; existing projector reads X/Y/W. Preserve withdrawn direct near-induced bbox-motion claim, separate missing clipping and held known answer.
- Withhold capture opportunities depend on FSM capture phases, not all ticks. Undefined clocks/intervals/floors/restoration make proposed schedules unschedulable. All-unmeasured cannot PASS; N-LC2 and encoded probe devices remain held.
- ForceMiss registrations and all-publication counter behaviour remain separate from wanted requests. Frozen manifest contradictions stay visible and unchanged.
- Frozen pairing A2.3 same-row throughput telemetry has no pacing-validity band. Release T separately requires <=1.05 under its full native1080p/600-request/30fps/fixtures/orders conditions, timing-only warmup60 and zero natural drops/holes. This is the saved correction to Codex's own wording; no gate changes.

**Chat review NOT REQUIRED for these source-backed documentation corrections.** The next mandatory checkpoint remains coherent corrected design before implementation. Source checks alone cannot release that design, a leg, a checker or the campaign.
