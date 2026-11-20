> Relay run: _relay\runs\2026-09-27-082-06-m53-s1-source-review | model gpt-6-astra | effort max | sandbox workspace-write | duration 1526 s (2026-09-27T03:27:58 -> 03:53:24) | copied verbatim from review.md by Claude Code

**Independent source review of m53 S1 — 2026-09-27**

The N1(a) draw/copy design is supported by the UE 5.1.1 source I inspected. I found no P1 defect in its normal submission path. I found **six P2 defects** that should be corrected before the affected 082-06/07 checks: the NoApply condition shortcut, incomplete collateral coverage, asynchronous subtype capture, incomplete material-usage admission, partial-allocation accounting, and NoApply 2 target lifetime. There are also three P3 notes below. My verdict is **CHANGES REQUIRED** for this source revision; this is not a rejection of N1(a).

**Chat review: REQUIRED.** These are findings and proposed fixes for the orchestrator's ruling, not instructions already accepted for execution.

**Scope and evidence.** I read the feature through git objects at **193bd35f0743e535839f89b7520caa296a227596**, including the changes from 2a2ab17 through bb048bb. All plugin file:line references below are at **193bd35**, unless explicitly marked otherwise. The plan is revision 3.1 at **ea1eed1**, docs/predictions/2026-09-26-m53-uv-normal-corruption.md; that file is unchanged between ea1eed1 and 193bd35. I read the three previous Codex reviews, chat's 082-02 through 082-05 rulings, the 082-05 implementation report, and the saved offline-check outputs. Engine references are to the local **UE 5.1.1** source under D:/UESource/UnrealEngine/Engine, with paths relative to that directory.

The feature ref and its local origin-tracking ref both read 193bd35; the live checkout remained at m51, 53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67. I performed no builds, cooks, engine/game launches, harness executions, captures, binary changes, or git mutations. Review scratch files and this report were written only in the relay run directory. Saved build results are evidence supplied by Code, not builds independently reproduced here.

**P2-1 — NoApply bypasses the condition measurement it is supposed to test.**

Location: Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:1051, also :1057 and :816. Plan reference: ea1eed1, §R10, lines 1331–1333.

IsVisualConditionHeld returns false immediately when NoApply is nonzero. This directly contradicts the requirement that the same live raw-slot and MID-parameter test measure the applied and null events. The additional SlotsCorrupted/bCommitted shortcuts also let bookkeeping decide the null outcome before its planned bindings are examined.

Concrete failure: a regression makes the ordinary condition path return true for a missing/replaced binding, or accidentally installs a MID during NoApply. NoApply still reports false solely because its lever is enabled. A clean null result then fails to demonstrate that the binding instrument can detect an absent condition. The current normal path does inspect bindings; the defect is in the claimed independent control, not evidence that all current ordinary readings are false.

Proposed fix: evaluate the expected qualified slots and expected parameter bindings against live objects regardless of NoApply. Require a nonempty expected set; NoApply 1 then fails naturally because those MIDs are absent from the slots, and NoApply 2 fails naturally because it created no binding set. Keep bCommitted for restoration ownership, rather than using it to bypass measurement. A deliberate bad-binding/accidental-commit mutation should change this reading and invalidate the control as appropriate.

**P2-2 — G-COLL silently measures the injection candidate set, not the specified visible collateral set.**

Location: Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:342, :353, :363 and :378. Supporting existing helper: Source/AnomalyInjector/Private/AnomalyViewport.cpp:944, :959, :515 and :653. Plan reference: ea1eed1, §R12.3 G-COLL, line 1540.

GetVisibleRenderableActors applies the injection poll radius and minimum screen coverage, then rejects translucent-only targets. IsRenderableComponent also rejects foliage and excluded-target patterns. The defaults are 1800 cm and 6% screen coverage (AnomalyViewport.cpp:38–42). These are selection policies, not a definition of everything visible in the picture. The subsequent UMeshComponent loop further limits coverage. Only the 256-path cap sets collateral_truncated; unavailable shader maps/bindings and a failed view lookup can also silently produce an incomplete or empty set.

Concrete failure: an unobscured prop occupies 2% of the image, or a visible backdrop lies outside the poll radius. RT allocation causes one of its textures to lose a resident mip. Both legs omit it, so the path join has no unpaired row to expose the omission; count can be small, truncated=false, and the measured deficit zero despite visible collateral blur. A mostly excluded scene can produce an apparently complete zero-texture result.

Proposed fix: use a dedicated, conservative collection of visible scene primitives/materials which bypasses target-eligibility policies. Include renderable material users such as foliage and translucent objects even when they cannot be injection targets. Retain the deterministic path cap and weak references. Report unresolved view/material/resource coverage as incomplete/unmeasured, not clean; treat -1 residency values as unknown when pairing. This can remain a CPU/public-engine-API probe. G-COLL is diagnostic rather than a Q gate, but its current coverage cannot support the specified purity reading.

**P2-3 — annotation subtype comes from completion-time live state rather than the captured event.**

Location: Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:6022; async caller :4265; output :6143. Supporting lookup: Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:61–74 and :96–102.

ProcessCompletedFrames accumulates a saved snapshot's event, but AccumulateFrameEvents obtains its subtype with GetLiveModeName(World, F.Id). The lookup neither uses the snapshot nor matches StartFrame/target. It returns empty after Revert, and can return the mode of a replacement event of the same id. Subtype is assigned when the earliest completed frame becomes the anchor, so later completions do not reliably repair it.

Concrete failure: a short tile-probe event ends while its first readback is pending. Its eventual annotation has an empty subtype. If an identity event of the same id starts before completion, the old tile event can instead be labelled identity. Correct captured pixels and per-frame telemetry can therefore disagree with annotation.json.

Proposed fix: snapshot the subtype with the event's capture-time state, or use that snapshot's already captured texcorrupt.mode telemetry (captured at :4981). Pass the value into accumulation, keyed by the existing event identity. The synchronous capture path should use the same representation. Validation should include completion after revert and after a subsequent event of the same id.

**P2-4 — the default-material guard checks one usage flag where the renderer can require several.**

Location: Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:140–175, especially :153–157, and :424–430. This also rejects declared deviation 5's claim to mirror the engine exactly.

NeedsUsage returns on the first component category. An instanced static mesh is checked for InstancedStaticMeshes and never checked for static lighting or Nanite. A Nanite static mesh likewise exits before the lighting branch. The lighting branch itself considers only LOD0 build data. UE's static scene proxy derives its light/shadow maps per LOD, with a separate shared-LOD0 case, then checks StaticLighting (Source/Runtime/Engine/Private/StaticMeshRender.cpp:2184–2204, :2225). The instanced proxy separately checks InstancedStaticMeshes for those sections (Source/Runtime/Engine/Private/InstancedStaticMesh.cpp:1413–1423). These checks are not mutually exclusive.

Concrete failure: a lightmapped instanced mesh has a loaded opaque material with InstancedStaticMeshes enabled but StaticLighting disabled. Its normal material shader map can be complete, and this tree admits its textures. The scene proxy still substitutes the default material. Apply and condition_held can say success while the new texture binding is never rendered. Unique lighting on another renderable LOD gives a similar gap. This can make a census/yield reading false; a purpose-built skeletal-only reason fixture does not cover it.

Proposed fix: gather and check every usage requirement applicable to the actual component/rendering route. Match the engine's effective static-lighting conditions across relevant LODs, including shared-lighting and volumetric-lighting exclusions, rather than treating LOD0 data existence as the complete predicate. Preserve side-effect-free flag reads and the S7 precedence. Add a combined-usage case to the admission verification.

**P2-5 — rollback undercounts scratch resources after a partial scratch-set allocation failure.**

Location: Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:649–666; cleanup :171–195; allocation failure helper Source/AnomalyInjector/Private/Anomalies/TexCorruptDraw.cpp:64–67.

AllocatedBytes gains a scratch set's entire size only after every level succeeds. Earlier successfully allocated levels are already held and will be released if a later level fails, but ReleaseAllTargets transfers only AllocatedBytes into pending release. The remainder of ReservedBytes is immediately unreserved. A target that is created and then rejected inside AllocateTarget has the same accounting boundary problem: the caller sees only nullptr.

Concrete failure: output allocation and scratch levels 1 and 2 succeed; level 3 returns a handled allocation failure. Cleanup queues release of all successful targets, but only the output bytes enter the two-frame pending ledger. The first two scratch levels become available budget immediately despite queued resource lifetime. A subsequent event can exceed the intended live-plus-pending cap. This is specifically a handled partial-failure path; it is not a claim that every D3D12 OOM is recoverable.

Proposed fix: account each successfully created resource immediately, and carry enough allocation/release information out of AllocateTarget to account a created-but-rejected resource too. On rollback, move actual created bytes into pending and unreserve only bytes never created. The current FailStep 2 fires before any allocation (:634–638), so it cannot prove this branch; add failure injection after an allocation ordinal or equivalent allocation-ledger coverage.

**P2-6 — NoApply 2's weak-pointer check is not equivalent to an EndPlay watch.**

Location: Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:595–604, watch registration :838–843, weak backstop :899. This is declared deviation 10.

The early NoApply 2 return skips target-watch registration. An actor can receive EndPlay(RemovedFromWorld) while its UObject and weak pointer remain valid. UE explicitly changes play/initialization state and broadcasts EndPlay without making that equivalent to weak-pointer invalidation (Source/Runtime/Engine/Private/Actor.cpp:2604–2652).

Concrete failure: the target's streaming level is removed during the event, with GC delayed or another reference retaining the actor. The applied event gets the watch callback and reverts; the no-allocation null stays active until its ordinary timeout or later GC. The null's labelled-frame residency samples and post-revert sample now cover a different interval. This is not an RT leak in NoApply 2, which allocated nothing, but it breaks the claimed lifetime/control equivalence.

Proposed fix: register the same target watches on this successful early-return path. A watch need not reserve, allocate, draw or touch a material, so it preserves the approved no-allocation control. Verify RemovedFromWorld/EndPlay while the actor is still a valid UObject, as well as Destroy and GC.

**P3-1 — the post-revert sample is scheduled in ticks, not frame numbers.**

Location: Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp:880–888 and :1041–1044; tick dispatch Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp:486–501.

If Revert runs before TickAlways in frame F, the countdown immediately becomes 1 in F and logs its “2 frames after revert” sample in F+1. Revert after that tick gives F+2 instead. Tick-order changes can therefore move the endpoint or leave it unpaired. The logged offset is truthful, which helps identify the issue; the textual promise is not stable. Proposed fix: store RevertFrame + 2 and sample at the first tick reaching that GFrameCounter, using the same approach as the release ledger. Report this as an endpoint mismatch until corrected.

**P3-2 — the Delta1 chain walk accepts an unexamined tail at its depth limit.**

Location: Source/AnomalyInjector/Private/Anomalies/TexCorruptTree.cpp:105–137 (MaxParentDepth is 16).

The MID/transient-package/RF_WasLoaded checks are correct for inspected links. If 16 links are traversed while Link remains non-null, the function nevertheless returns “no runtime link.” A longer inheritance chain with a runtime-created ancestor beyond the limit would pass this guard, contrary to “anywhere in the chain.” I did not establish such a chain on a current fixture; this is a bounded-walk correctness note. Proposed fix: distinguish reaching a terminal material from exhausting the bound; fail closed on an unverified tail/cycle and report that diagnostic, or walk to completion with cycle detection.

**P3-3 — a peak cannot prove that released bytes returned to zero.**

Location: Source/AnomalyInjector/Private/Anomalies/TexCorruptState.cpp:355–358; plan ea1eed1 §R9.4, lines 1273–1274.

The code correctly maintains a monotonic run peak. The plan's repeated-event recipe asks texcorrupt_rt_bytes_peak to return to zero after each event, which it cannot do. A checker following that wording would fail correct release behavior. Proposed fix: judge balance from live and pending bytes after the release grace period, and retain peak for maximum usage. Revert logs current live/pending, but a frame-qualified terminal ledger reading would make this check less ambiguous. Do not reset the peak after each event to satisfy the wording.

**S1 — implementation against the plan.**

The architecture and most ordered decisions are implemented faithfully. Event E1–E7 is explicit and early-returning (TexCorruptTree.cpp:649–731). Per-slot S1–S8 and per-binding T1–T10 follow the stated order. Virtual precedes unsupported type; m52 hold/restoring precedes readiness; readiness precedes bias; cinematic/per-texture/streaming-budget bias precedes full residency. The no-platform-data exception is declared. A2 and A3 precede A4 as the plan specifies. Aggregation uses the shared rank selectors, ignores intentionally untouched slots for V1, and records exactly one event-final refusal. Slot/binding disposition counters are additional diagnostics, not extra event refusals. The usage-check gap is P2-4.

Active bindings come from the complete compiled shader map's FUniformExpressionSet for the world feature level, not a pointer intersection with all authoring parameters (TexCorruptTree.cpp:582–630). Each record retains the complete FMaterialParameterInfo, and installation/readback uses that info (:797–804 in Anomaly_TexCorrupt.cpp). This preserves layer/global identity and leaves a constant texture entry non-parameter even if some inactive parameter aliases its texture. A uniform entry is still not proof that its contribution survives final material arithmetic; the plan already treats the dead-binding fixture as diagnostic.

Residency admission uses platform mip shape, resource/pending-state checks, a valid streamable state, AssetLODBias zero, the cached/cinematic restrictions, and resident=max=cooked M (TexCorruptTree.cpp:177–212, :300–363). It does not call force-residency or wait for streaming. The per-texture streaming-budget predicate implements revision 3.1's conservative rule, including positive fractional bias. The m52 query includes both held and restoring textures. Delta1 is implemented using MID type, transient outermost package and absence of RF_WasLoaded on Resolved and distinct Effective chains, subject to P3-2.

The atomic set is per slot, as approved. UV requires every binding; normal requires every normal-map binding; only a failed, proven one-mip 1x1 texture is exempt. The eight-map cap counts bindings, while allocation deduplicates by texture pointer. Between-slot partial application is deliberate and labelled (TexCorruptTree.cpp:441–545). Qualified slots use a new host MID parented to the resolved material; a shared material is not edited in place.

Budget arithmetic shares the actual pure helpers: one BGRA8 output chain per distinct source, plus one lower-level scratch set per (W,H,sRGB) class, and live plus pending against the cap. Two same-class 4096-square maps require 192 MiB, not 213.33 MiB. There is no size reduction to fit. S1 has identity/static tile and forced identity redraw, so omission of S2 drift snapshots/modes is appropriate. Successful static scratch release, redraw-held scratch and final output release balance under the specified two-frame bookkeeping rule. P2-5 is the partial-allocation exception. This ledger estimates the plan's texel bytes; it is not a D3D12 physical-memory/fence measurement.

The transaction orders reservation, all allocations, all corruptor MID setup/readback, draw preconditions, enqueue, host MID setup/readback, then slot commit. Failure exits before commit release resources and strong references and count one final reason (Anomaly_TexCorrupt.cpp:627–848). FailStep 5 usefully exercises cleanup after enqueued work. There is no recoverable post-commit failure stage to roll back; SetMaterial is a void synchronous game-thread mutation. The pre-commit checks do not constitute GPU success acknowledgement, which §R7.4 expressly leaves to packaged gates.

**Judgment of the ten declared deviations.**

| # | Judgment | Reason |
|---|---|---|
| 1 | Acceptable | optional_not_resident describes the public count evidence without asserting private mount state. It remains not_fully_resident at T10. |
| 2 | Acceptable | snapshot_px_w / snapshot_px_h preserve both dimensions in the scalar record representation. The eventual checker/consumer needs those actual keys. |
| 3 | Acceptable with the stated limit | E1 parameter-name/type existence validation is useful installation checking. It does not prove active graph wiring, sampler type, explicit mip mode or correct output arithmetic. |
| 4 | Acceptable | Refusing missing platform data at the format step is conservative and avoids invalid reads. “Cannot happen on cooked content” should be read as healthy loaded cooked assets, not a guarantee about every runtime-created or malformed UTexture2D. |
| 5 | Not acceptable as claimed | It does not mirror all component/LOD usage requirements; see P2-4. |
| 6 | Acceptable for S1 | Product modes are deferred. Bench levers select identity/redraw/tile; any supplied mode argument and a fire without a bench mode refuse mode_invalid after E1–E5. Census intentionally bypasses mode admission. |
| 7 | Acceptable | draw_precondition_failed is an appropriate transaction failure name. A reserve rejection occurs before writes; rollback_1 is distinguishable from allocated cleanup, and the summary's extra-key handling preserves it. |
| 8 | Acceptable | The bench flags and partial-set counter are additive diagnostics. collateral_count/truncated are already required by revision 3.1, rather than new deviations. P2-2 concerns their completeness, not permission to emit them. |
| 9 | Acceptable | Checking asset/object references rather than banning the fixture path's literal bytes accommodates the approved bench lever. A literal path in a guarded command is not itself a cooked fixture dependency. |
| 10 | Not acceptable as equivalent lifetime handling | A weak pointer does not observe every EndPlay; see P2-6. The watch can be installed without compromising NoApply 2. |

**S2 — N1(a), render states, mip indices and first sampling.**

On the successful path, the code uses the relevant UE 5.1 runtime APIs correctly:

1. AllocateTarget sets the sampler-related properties before resource creation, allocates PF_B8G8R8A8 with the intended gamma flag and expected chain length, and calls UpdateResourceImmediate(true) before content draws (TexCorruptDraw.cpp:23–69). UE derives the full mip count when bAutoGenerateMips is true (Source/Runtime/Engine/Private/TextureRenderTarget2D.cpp:29–65). Its immediate update runs the initialization clear/generation and removes the target from the deferred list (:163–174, :682–719). That initial empty-content generation therefore does not overwrite authored per-mip content later.
2. DrawLevel clears, begins the canvas target, draws the full material tile, and ends the canvas (TexCorruptDraw.cpp:77–99). UE's EndDraw flushes the canvas and enqueues the resolve (Source/Runtime/Engine/Private/KismetRenderingLibrary.cpp:761–812). For the default single texture backing used here, TransitionAndCopyTexture returns it to SRVMask without generating mips (Source/Runtime/RHI/Public/RHIUtilities.h:802–825). Each level has its own corruptor MID, avoiding a last-parameter-value alias between levels.
3. Mip 0 is drawn directly. Every other mip is drawn into its own one-mip scratch size, then copied (Anomaly_TexCorrupt.cpp:291–323; TexCorruptDraw.cpp:101–132). FRHICopyTextureInfo defaults to source mip 0, one mip, slice 0 and one slice; setting DestMipIndex=m and Size=(Wm,Hm,1) is correct for these non-array 2D textures (Source/Runtime/RHI/Public/RHI.h:1966–1993). There is no source compressed-texture copy or accidental array/cube subresource.
4. Both copy operands are BGRA8 with the same sRGB flag. AddCopyTexturePass checks equal formats and declares CopySrc/CopyDest accesses (Source/Runtime/RenderCore/Private/RenderGraphUtils.cpp:225–270). RDG adds the transitions and the external texture epilogue returns to SRVMask (Source/Runtime/RenderCore/Public/RenderGraphResources.h:303; Private/RenderGraphBuilder.cpp:3011). D3D12 performs a copy of the requested mip/slice subresource (Source/Runtime/D3D12RHI/Private/D3D12Texture.cpp:2880–2960). The copy moves encoded bytes; it does not apply a second gamma conversion. UE creates the corresponding sRGB SRV/RTV formats from the target flag (:1407–1410).
5. The clear/draw/resolve/copy commands are submitted before host MID parameter updates and SetMaterial's subsequent proxy update. Shared scratch reuse follows the previous copy in the same command stream. Static scratch ReleaseResource commands are enqueued only after those copies. UTexture::ReleaseResource enqueues deletion of the captured resource (Source/Runtime/Engine/Private/Texture.cpp:206–229); it is not an immediate destruction of the render-thread object. Normal UE RHI lifetime and ordering cover GPU execution after render-thread submission. Adding a blocking fence is not indicated by this source review.

Thus **I found no successful-path opportunity for the host to sample a partially populated output** under the supported synchronous game-thread fire path. Commands from an earlier frame use the old binding; commands using the new binding follow its initialization. This is a source-ordering conclusion, not a cooked-GPU pixel result.

There is an explicit failure boundary: EnqueueMipCopy can increment the tripwire and return without copying, and Apply does not wait for or veto on that render-thread outcome. In that error case the committed texture can contain initialization/partial content. The same applies to an underlying GPU/material draw failure. This matches the plan's stated absence of GPU acknowledgement, but means “fail-closed” must not be read as an automatic GPU-failure rollback guarantee. A nonzero tripwire or failed G-ID/G-ID-M remains a stop condition for the leg's evidence.

The tripwire checks the source RHI's mip count and top extent against admission, before and after each output sequence (TexCorruptDraw.cpp:135–160). It catches a truncated/remapped resident chain whose RHI shape differs from M/W/H, and the copy path separately catches a missing destination or unavailable destination mip. **It does not validate texel content or SrcMip selection.** A wrong source-level scalar, wrong lower-mip contents, mipgen, or a skipped valid copy with unchanged shape can pass the shape tripwire. The assigned wrong-copy image controls are indispensable. The corruptor graphs are absent, so explicit SampleLevel, disabled AutomaticViewMipBias, sampler types and alpha/normal encoding still require the authored-asset and cooked checks.

**S3 — identity and host-visible purity.**

With intentional wrong-copy/foreign-replacement levers off, the code does not directly edit other slots' material parameters or mutate a shared original material/texture. It commits only qualified slots, uses per-event host MIDs, and deduplicates them only inside that event. Refused slots keep their raw bindings. Components elsewhere using the same original material keep it. The separate owner sweep at revert acts only on this event's exact MID pointers, including an event MID the host copied elsewhere on the same owners; that is the plan's cleanup rule, not proof of exact restore for those swept slots.

Raw/asset/effective capture and the Nanite-override guard avoid the known “edit one material, render another” Nanite path (TexCorruptTree.cpp:390–402, :734–760; UE Source/Runtime/Engine/Private/Components/StaticMeshComponent.cpp:2264–2267, :2655–2675). The broader default-material-path gap is P2-4. I did not find a new reason to replace a Nanite material or to alter its routing.

LODGroup, Filter and AddressX/Y are copied to the output before InitCustomFormat. UE's RT sampler uses those settings with zero sampler bias (TextureRenderTarget2D.cpp:647–655); the original texture sampler uses its corresponding settings and mip bias (Source/Runtime/Engine/Private/Rendering/StreamableTextureResource.cpp:240–269). The tree refuses the admitted runtime bias cases instead of silently sampling a sharper output. Shared samplers continue to be selected by the unchanged host shader. Runtime changes to sampler/profile/streaming conditions during an event are not frozen by these Apply-time checks; identity-redraw also samples the original source again. The specified S1 redraw fixtures are never streamed, so a streamed-host redraw must not be substituted without requalification.

The warm phase allocates tiny, unbound plugin targets and uses the shipped noise asset rather than host textures (TexCorruptDraw.cpp:201–303). It has its own strong holds/releases and no host-slot commit. Capture's dedicated phase performs it outside emitted frames and before lead-in. It is reasonable preparation, not proof that every real Apply PSO was warmed; the code's own log makes that limitation explicit. The warm phase can change time/streaming/cache history, so paired legs need the same warm policy as the plan requires.

G-COLL itself reads uniforms and streamable-state counts; it does not draw scene pixels, create a scene capture, or force mips. Its problem is coverage (P2-2), not a hidden texture mutation by the probe.

**Identity is not proved universally pixel-neutral by this source.** Replacing compressed samples with RGBA8 introduces the accepted quantization, and a host material can amplify a small sample difference through a mask threshold or other nonlinear arithmetic into a much larger final-pixel difference. Also, strong UObject references do not pin resident mips: RT memory pressure and changed material streaming demand can affect other textures, including untouched slots on the target and shared users. An admission cap and a CPU observer do not prevent that. The plan's fixture tolerance and collateral/restore measurements are therefore meaningful remaining evidence, not consequences of a correct pointer swap. G-COLL's definition excludes the target itself; it does not cover purity of that target's other slots by itself. Source review should not extend the fixture <=2 result into an arbitrary-host pixel guarantee.

**S4 — lifetime, rollback and ownership.**

| Concern | Source assessment |
|---|---|
| Displaced originals and GC during an event | Raw, asset and resolved originals are held before replacement (Anomaly_TexCorrupt.cpp:613–624). Outputs, per-level corruptor MIDs and host MIDs are also held. These holds reach a subsystem UPROPERTY array, not merely raw C++ arrays (Public/AnomalyInjectorSubsystem.h:158–159; Private/AnomalyInjectorSubsystem.cpp:98–110). MID parents/parameters and the explicit holds keep sources reachable. |
| Raw versus effective capture | OverrideMaterials is read directly for Raw; asset slots are read independently; Resolved selects raw or asset; Effective uses GetMaterial. A null raw slot is therefore distinguishable from an explicit override. |
| Restore only owned slots | Revert tests the actual raw slot against that event's HostMID before restoring the exact raw pointer or null default (:954–995). Foreign replacements are left to the game. It does not restore an entire old OverrideMaterials array or shrink away later changes. The plan permits a grown array containing null entries. |
| Replaced components and sweep | Weak component lookup falls back to owner plus component name. The fallback still requires exact MID ownership. The owner sweep clears only this event's MID pointers and counts sweep separately (:998–1021). An unresolved component or sweep is not silently reported as exact restoration. |
| Destroy / EndPlay / world teardown | Ordinary applied and NoApply 1 events register owner watches. The subsystem gathers callback ids before dispatch, so Revert can clear watches safely. A weak primary-owner check is a secondary backstop; teardown explicitly calls event cleanup and warm/bench cleanup. NoApply 2 is the P2-6 exception. |
| Rollback | All currently handled transaction failures precede slot commit. Targets are released before held references are dropped; the event becomes inactive and Apply returns false. Complete allocation/draw/MID failures balance the ledger; partial scratch allocation is P2-5. Existing fault levers are not a proof of every partial-construction branch. |
| Scratch and outputs | Static scratch is released after all draws/copies; redraw scratch remains live until revert. Outputs remain until revert. Warm resources have separate cleanup and do not bind a host. The two-frame ledger is bookkeeping, not an assertion that a GPU has retired every allocation within two frames. |
| Isolation of the two ids | Each id owns separate event objects, host MID pointers, target records and restoration sets. The shared UPROPERTY holds are added/removed with multiplicity, so one event does not remove another's hold on the same original. The ledger intentionally aggregates both ids. S3 also refuses a runtime MID installed by another event if encountered on a slot. |
| Game/render thread use-after-free | I found no normal-path use-after-free. Copy/tripwire lambdas capture resource pointers rather than using unprotected UObjects on the render thread; queued release follows their work. UTexture GC uses release fences before final destruction (UE Texture.cpp:969–1028). This conclusion assumes the normal engine game-thread resource lifecycle, not unsupported concurrent host mutation. |

The supported one-actor LiveFires orchestration remains significant. This review does not turn unsupported overlapping raw console Apply calls into an ownership guarantee, nor claim that an event can control arbitrary host material edits during its lifetime.

**S5 — refusal reachability and diagnostics.**

Every named final-reason branch in the plan has a corresponding path in the source; I found no missing enum-only final reason. There is exactly one event-final counter per tree refusal or transaction failure. slot_empty and slot_translucent intentionally aggregate to no_eligible_slot when all slots are untouched; they are slot dispositions, not missing event-final refusal counters. Normal-family no_normal_map/normal_unconnected and unsupported required binding precedence follow A2/A3/A4, rather than applying UV-family failure reasoning to irrelevant color bindings.

The important limits are that some producers remain **UNEXERCISED**, as the plan explicitly says; missing assets currently mask every later tree reason; and P2-4 means not every actual default-material path reaches its intended refusal. The healthy-cook fixture producer expectations still need the corresponding cooked assets and real resource state. I do not call static reachability proof of a G-REASON pass.

assets_unavailable is sound for missing references: all three CDO hard references are tested before allocation or host mutation (AnomalyInjectorSubsystem.cpp:60–70; TexCorruptTree.cpp:664–685). A corruptor missing a required scalar or texture parameter name is also rejected there. The code tests the parameter's existence in the default-value API, which is stronger than merely setting and reading a newly added MID override. It is weaker than proving that a connected, correctly typed sample consumes that parameter. E2 tests only the selected corruptor's shader-map readiness, as specified. A wrongly authored graph with the expected names can pass both and must fail the image/asset checks instead.

For a fresh supported fire, a tree refusal or handled transaction refusal returns false before SetMaterial, so I found no path that leaves a new label or touched host slot behind. The Auto path adds a LiveFire only after successful Apply. NoApply success intentionally produces a labelled null event and is not a refusal. The render-thread diagnostic failures discussed under S2 are also not tree/transaction refusals; their counter must invalidate the evidence even though the event may have been committed.

**S6 — what the offline harness proves.**

TexCorruptPure.h is genuinely shared. tools/texcorrupt_pure_test.cpp:5 includes that header; the saved build command uses the plugin's Anomalies include directory with /W4 /WX. The plugin calls its chain/scratch arithmetic, EventRequirement, Fits, format classifier, admitted-chain predicate, rank selectors, SourceMipFor and bias predicates. This is not a separate reimplementation of those functions.

The saved outputs contain 83 checks with zero failures, and the two-fault variant has eight failures: five scratch-sharing/budget rows and three mip-clamp rows. I inspected those outputs and the harness source; I did not rerun or compile them. The results substantiate the reported examples of helper arithmetic, shape/format classification, supplied-rank selection, mip selection and the specified bias booleans.

They do **not** execute EvaluateTree, its E/S/T condition ordering, material-chain traversal, UE texture extraction, usage requirements, the mutable FLedger, allocation accounting, parameter setup, rollback, restoration, render commands or capture timing. For example, the precedence tests at tools/texcorrupt_pure_test.cpp:177–202 supply arrays of ranks to selectors; reordering an E check in TexCorruptTree.cpp could leave all 83 green. The chain-shape cases concern mip dimensions, not the Delta1 material-parent predicate. Budget table expectations are formatted to two decimal MiB places (:79–81), so those particular rows do not prove exact byte totals, although separate Fits tests cover an exact boundary and one byte over. A2's rank constant also does not prove all normal-family aggregation behavior.

Consequently the helpers cannot drift independently while that exact header is used, but the plugin's extraction, sequencing and state transitions can drift around them. P2-5 is a concrete example that all 83 checks miss. Code's explicit caveat that running-engine behavior is not covered is fair; “decision-tree precedence” should be understood as selector/rank coverage, not a full tree proof. Focused additions for partial allocation, combined usage, control measurement and capture-time subtype would address actual gaps rather than duplicate the helper implementation.

**S7 — interpretation of the next evidence and remaining work.**

The Build.cs declares exactly Core, CoreUObject, Engine, Foliage, InputCore, RenderCore and RHI, with the two new modules private (Source/AnomalyInjector/AnomalyInjector.Build.cs:9–22). Saved final build-status records show successful editor and game G10 builds; Code reports zero warnings and unchanged other module sets. I did not independently relink, audit every response file, or run those binaries. Build success cannot settle canvas gamma, sampler graphs, normal encoding, actual copy content or D3D12 cooked execution.

The following distinctions matter when evaluating the forthcoming legs:

- A zero shape-tripwire count and 83/83 helper checks cannot substitute for G-ID/G-ID-M, authored-chain rows, engine-authored chains, or the assigned wrong-copy controls. All those controls need the prescribed failing magnitude as well as the identity tolerance.
- NoApply 1 is correctly the allocation/draw control for identity comparison; NoApply 2 correctly skips reservation/allocation/draw/commit for collateral comparison. Their roles should remain distinct while P2-1 and P2-6 are fixed.
- Correct ordinary restore is a pointer/ownership fact; full-frame restore purity additionally depends on streaming and rendering history. Post-revert readings need the intended frame offset (P3-1), not merely a “post_revert” string.
- An all-assets-missing run is evidence of E1 refusal, not evidence that later reasons, draws or source binding work. Creating assets in a process whose CDO already cached missing references also does not by itself establish a fresh runtime startup readback.
- Identity's expected_strength_class is currently “none” (Anomaly_TexCorrupt.cpp:45–48), outside §R10's product-mode vocabulary. That is a reasonable bench-only value to document for S1 checkers; it is not a measured strength or an approved new client product mode.
- G-COLL remains an observation of resource levels, with the coverage qualifications above. A zero result cannot establish absence of arbitrary host pixel changes, and count subtraction across legs would still be wrong.

The bounded review leaves the authored corruptor/noise assets, fixture/import/byte manifests, cook/archive gates, actual cooked draw content, temporal onset, real GC/EndPlay/partial-failure exercises and collateral readings for their authorized later stages. Those were neither run nor reported as passing here. I found enough source evidence to support the normal N1(a) mechanics and ownership design, but the six P2 corrections are needed for reliable admission and bench evidence from this implementation.

VERDICT: CHANGES REQUIRED
