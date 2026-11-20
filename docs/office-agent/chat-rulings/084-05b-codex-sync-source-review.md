<!-- Relay run runs\2026-09-29-084-05b-sync-source-review | model gpt-6-astra | effort max | duration 1465 s | collected by Claude Code (084-05b), content unedited -->
**084-05b — Independent source review of 084-05a and 086-02**

2026-09-29. Reviewed `44584e7..ce84394` and `ce84394..f51f124` on `fix/m52-label-timing`, including the two reports, both session journals, the governing rulings, and the relevant UE 5.1 implementations. All plugin line references below refer to `f51f124`. This is a review for chat Claude to adjudicate; the proposed remedies are not accepted instructions to Claude Code.

**The combined change does not merit an unqualified GREEN.** The main m52 asynchronous emission fix is coherent, but the broader G350 invariant still fails for visible phases of live hide events. The new camera implementation also has a concrete false negative for ordinary scaled instanced meshes, before its SAT test even runs. There are additional limits in synchronous capture, renderer-view matching, transition history across runs, and restoration of recycled stencil values. Shipping exclusion of the new controls is supported by the source.

| Question | Answer |
|---|---|
| Can an unlabelled row still set `anomaly_present`? | **Yes.** A live blinking event in its visible phase has `FireLabelled=0` but retains `Normal` emission. The writer sets presence from emission mode, not that flag. |
| Can a labelled frame fail to set it? | **Yes across products in the supported sync code path.** A positive session-global camera event can enter annotation accounting after an empty/negative labels row has already been written. No equivalent omission was found in the normal, resolved m52 asynchronous membership-to-emission path. |
| Are transitions correct? | The ordered, within-run m52 onset/off windows and detached hide return entries are implemented coherently. Live hide returns still set presence incorrectly; history is discarded between runs. Values remain provisional and are not pixel-qualified by these tests. |
| Can recycled mask IDs alias? | The normal readback, coalesced-render and census guards appear sound. **A conditional component-state alias remains:** retirement skips a component with custom depth disabled but the retired stencil value still stored. |
| Is the slab sound? | The convex slab/OBB SAT is a reasonable geometric test for its stated inputs. Candidate collection has an ISM false negative, and bounds intersection is not a proof of a changed rendered pixel. |
| Is it the captured view? | Moving the usual evaluation to tick end repairs the ordinary stale camera-cache read. It does not bind the verdict to the actual captured `FSceneView`, projection or render-family identity. |
| Are new controls Shipping-excluded? | **Yes by source configuration.** No Shipping binary was built or inspected in this review. |
| Are all runs without the two anomalies byte-identical? | **No as stated.** Hide transition rows, new run-summary fields and allocation after pool exhaustion are counterexamples. A narrower unchanged-row argument is valid. |

The applicable product contract is the broad presence rule in 084-03 decision 1, the transition rules in its decision 2 and 086-01 decision 1, and the pixel-derived sync requirement in the 085-00 addendum. The approved release threshold is 50% of the effect, with strict and 10% counts alongside it. The requirement that a gate fail a broken build remains separate from a source test passing. Restricting G350 to m52 reconciles one compatibility goal but does not satisfy the literal broad ruling; that conflict requires a ruling, not a declaration that both goals were met.

**F1 — High: G350 remains false for non-m52 entries, including the newly flagged live hide-return frame.**

Evidence: `Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp:40–81,107–108,243–262`; `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp:5646–5652,5758–5777,6898–6944,7043–7100`.

`EmitAt` defaults to `Normal`. `CountEntries` marks the frame present if any entry is `Normal`; neither it nor `BuildFrameLabelRecord` receives `FireLabelled`. `ApplyRenderTruthToSnapshot` assigns suppression/transition-only modes only after `IsRenderTruthFire`, which is the m52 path. Other entries retain the default.

A direct counterexample is a live blinking burst:

1. One captured frame is logically hidden and labelled.
2. On the next captured frame the object returns, while the burst remains live.
3. `IsFireLabelledThisFrame` returns false for that visible object, and the frozen `FireLabelled` value is zero.
4. `StepHideTransitions` sets `EntryTransition[i]=1`, but does not change `EntryEmit[i]` to `TransitionOnly`.
5. The labels row therefore contains the blinking entry and sets both `transition_present=true` and `anomaly_present=true`. With a valid projected box it also sets `visible_positive=true`; asynchronous positive-frame accounting uses the same entry-count result.

The next visible in-burst frame remains positive even after the transition flag expires. The same default-mode problem applies to other live state-driven anomalies during an unlabelled phase. The detached return case is different: it is appended through `TransitionFires` and correctly does not set presence.

This behavior was substantially inherited, but retaining it is an explicit scope choice of 084-05a. The report itself acknowledges that applying G350 generally would change blinking's visible rows. Its claim that all three ruled fixes are complete is therefore too broad. Here, “labelled” refers to the frozen internal flag and downstream membership; the JSON does not contain a general per-entry `labelled` boolean.

**Proposed remedy for chat's consideration:** derive entry emission and frame positivity from a single finalized per-entry label decision for every supported anomaly. A returned live hide entry would be transition-only while flagged, then suppressed when unlabelled. Keep observability separate from label membership. A serialized mixed-event test should compare row presence, entry membership, positive-frame counts, annotation indices and m55 flags. If preserving legacy rows takes precedence, explicitly narrow the ruling and public claim instead.

There is also a schema clarity issue: `transition=1` describes both a labelled onset and an unlabelled residual. On a mixed positive frame, the root presence flag cannot identify which transition entry is labelled. An explicit per-entry membership or transition-phase field would make that distinction available to clients; internal `EEntryEmit` is not serialized.

**F2 — High: the new ISM candidate query can discard geometry actually crossing the clipping slab.**

Evidence: `Source/AnomalyInjector/Private/AnomalyViewport.cpp:1071–1108`, particularly line 1088. In the local UE 5.1 source, `Engine/Source/Runtime/Engine/Private/InstancedStaticMesh.cpp:3969–3995` implements `UInstancedStaticMeshComponent::GetInstancesOverlappingBox` using each instance's translation plus/minus the mesh's **untransformed** bounds extent. It ignores instance rotation, instance scale and the mesh bounds origin. The later correct full-transform OBB calculation cannot recover an instance omitted by that query.

A simple source-level counterexample, in centimetres:

| Parameter | Value |
|---|---|
| Camera | Origin, looking along +X; ordinary centred 90-degree perspective |
| Baseline / anomalous near | 10 / 100 |
| Cube mesh bounds | Centre `(0,0,0)`, half extents `(10,10,10)` |
| Component transform | Identity |
| Instance transform | Translation `(150,0,0)`, scale `(10,1,1)` |
| Actual world X extent | `[50,250]` |
| X extent used by UE's overlap query | `[140,160]` |

The front face at X=50 lies inside the slab and comfortably inside the frustum. Pushing the near plane to 100 removes that baseline-visible face. The component's aggregate bounds include the scaled instance (`InstancedStaticMesh.cpp:2658–2669`), so it passes the first component broad phase. The per-instance query then tests `[140,160]` against slab X `[10,100]`, returns no instance, and the implementation reports no clipping when this is the only candidate. This needs no hollow mesh, WPO, translucency or small-pixel argument.

HISM has an override that transforms the mesh box when its cluster tree is available (`HierarchicalInstancedStaticMesh.cpp:3551–3585`). That does not fix ordinary ISM; HISM's no-tree fallback calls the same faulty base query. The review's arithmetic reproduction is recorded in `review-evidence.json`. It is an analytic counterexample, not a launched engine test.

**Proposed remedy:** use a candidate query proven conservative for full instance transforms and mesh bounds origins, or enumerate appropriately transformed instance bounds before SAT. Add ordinary ISM scale, rotation and off-centre mesh cases, plus the HISM fallback case. The standalone slab selftest bypasses this UE candidate-query integration, so its 93 passing checks cannot catch this defect.

**F3 — High for the blanket label contract, inherited: synchronous capture still writes a different event set from annotation accounting.**

Evidence: `AnomalyCaptureSubsystem.cpp:5131–5205`; `AnomalyLabelWriter.cpp:561–623`.

The sync writer fetches only `Auto->GetLiveFires()` and serializes them with default emission options. Only after it has saved that row does `CaptureCurrentFrame` call `AppendSessionGlobalFires` and accumulate those globals into event accounting. `bPositive` was also computed from the auto live-fire count alone.

A camera-only sync capture with a positive slab verdict can consequently write `anomalies:[]`, `anomaly_present:false` and no positive-frame increment, while the same session index is recorded for the camera event in annotation accounting. Sync hide rows also receive no new hide-return transition processing. This applies to explicitly selected sync capture and the retained async-to-sync fallback when reached. Normal m52 render-truth capture has its own SVE/async requirements; this finding is not a claim that the ordinary m52 path silently runs synchronously.

086-02 accurately says the sync path is unchanged, but unchanged is not evidence that the complete label contract holds there. **Proposed remedy:** serialize and account from the same finalized snapshot on both paths, or explicitly refuse a capture mode that cannot supply the required semantics. This finding should be recorded as an inherited coverage gap rather than attributed to the new SAT code.

**F4 — Medium, concrete conditional alias: retirement does not clear every component still holding the retired stencil value.**

Evidence: `Source/AnomalyCapture/Private/AnomalyStencilTag.cpp:125–139,207–229`; `Source/AnomalyCapture/Private/AnomalyMaskMeasure.cpp:146–197`.

`RestoreComponentsCarrying` skips a component if `!Prim->bRenderCustomDepth`, even when the component is in `GTaggedComponents` and `CustomDepthStencilValue` still equals the retired value. `ReclaimReleasableTag` marks the old record recycled and returns the tag regardless of how many components were restored.

Possible sequence:

1. Event A owns tag 200 on a component; its original host state is saved.
2. All A readbacks and trails drain. The host temporarily disables custom depth without changing the stored value 200.
3. Pool exhaustion retires A. The restoration helper skips that component; event B receives 200.
4. The host re-enables custom depth on A's old component. A and B now draw under the same value.

This is a source-supported conditional defect, not a claimed observation on the banked bench. It is distinct from an unread-readback race. Restricting cleanup to components currently belonging to the old actor also deserves care if components can be transferred during an event; the ownership map tracks component identities, but the cleanup enumerates the actor's current components.

**Proposed remedy:** retire ownership using the saved component identities and matching value even when custom depth is currently off, or quarantine the value if restoration cannot be established. Preserve explicit host ownership changes deliberately, rather than treating “not drawing right now” as “cannot draw this value again.” Validate the retirement result before handing the value to another event.

**What is correct about m52 emission and transition routing.**

In the supported asynchronous path, resolved m52 membership writes `FireActive`, `FireLabelled` and `ConditionHeld` together. It selects `Normal` for members, `TransitionOnly` for an off-window nonmember, and `Suppress` for other nonmembers (`AnomalyCaptureSubsystem.cpp:7078–7099`; `Source/AnomalyInjector/Public/AnomalyLabelSync.h`). I found no path in that normal mapping where a member is suppressed solely because it is carried or trailing. `Unknown` remains a conservative member; that avoids silently declaring a missing receipt clean, but is not proof that the picture was anomalous.

The relevant downstream routes are consistent for actual m52 transition-only entries:

| Product | Source behavior |
|---|---|
| `anomaly_present` / `visible_positive` | `TransitionOnly` does not set presence or add to the positive bounding-box count. A different normally emitted entry can legitimately make a mixed frame positive. |
| `positive_frames` | Async accounting uses the same entry counts as the writer, so m52 transition-only rows alone do not increment it. F1's live hide entries are still `Normal`, which is why they remain an exception. |
| `annotation.json` `frame_indices` | Accumulation can retain bookkeeping frames, but final event indices for `ActorHidden`/`AnomalyState` use active membership. Detached `TransitionFires` are not added to that event accumulation (`AnomalyCaptureSubsystem.cpp:8423–8510,8581–8616`). |
| Target-mask pixels | m52 tags whose resolved membership is false are removed from the kept event-tag set before PNG filtering (`AnomalyCaptureSubsystem.cpp:1605–1635`). A transition entry can still display an assigned numeric `mask_value`; that is not a claim that pixels with that value were retained. Detached transition entries use no per-fire mask slot. |
| m55 | The queued m52 `bLabelled` value is replaced by resolved membership (`AnomalyCaptureSubsystem.cpp:7166–7177`). Negative observations and offset evidence can still appear around an event by design; they are not labelled positives. Detached transition-only entries are not inserted as new positive observations. |

`FEventTransitionTrack::Observe` implements first K_on labelled observations and an off window within K_off session-index steps after the previous member (`AnomalyLabelSync.h:177–209`). With ordered frames, defaults 2/8, and no run boundary, the familiar sequence is correct: the first two members are labelled and flagged; subsequent members remain labelled; the next eight negative indices are transition-only; later negatives are suppressed. Reopening membership restarts the off window without inventing a new event onset. Negative gaps inside a continuing event can also be flagged; that is a defensible conservative streaming interpretation of residuals, rather than advance knowledge of the eventual last member.

The temporal-AA resolver gives 2/8/1 for negative/default settings under TAA/TSR, clamps explicit values to its supported range, and returns zero without temporal AA even if an explicit positive value was set. That is consistent with the ruling's “under temporal AA” qualification. It is resolved at run start, not continuously if AA changes during a run. The eventual 2/8 lock still needs the ruled bank/confirmation calibration; the source tests do not establish that every real residual fits those windows.

The implemented unit is the capture session index/observed capture, not every engine-rendered frame. Skipped capture periods, failed writes or a hide/return that occurs entirely between observations are not a proof of first-rendered-frame coverage. The ordering support also does not retroactively rewrite an already emitted transition decision: diagnostic handling of late observations should not be described as arbitrary-order output equivalence.

**F5 — Medium: cross-run transition history is lost even though restoration history is carried.**

Evidence: `AnomalyCaptureSubsystem.cpp:324–330,6297–6327,7088–7094,7563–7635`.

`FinishRun` samples pending state, opens trails for live cuts before reverting, drains asynchronous work, and retains still-attached restoration trails. That protects the existing captured m52 membership from being rewritten merely because the run ended. The next run adopts both carried and orphan restoration trails, so still-held frames can still be labelled. These are useful protections.

However, `ResetLabelSync` clears both transition maps. The adopted trail is rebased into the new session, and a new inherited transition track merely sets `bOnsetPast`. It carries neither the previous last-member position nor the outstanding off budget. If the first captured frame of an immediately following run is already at render baseline but still has temporal history from the preceding held frame, the new track has no previous member and cannot flag the residual. A fully detached event with only an outstanding temporal tail has no restoration trail to adopt at all. Hide-return history is also reset.

This is not a demonstrated pixel failure on either reviewed binary. It is an uncovered boundary in the claimed residual rule. **Proposed remedy:** carry/rebase enough transition history across an uninterrupted renderer history, or establish a measured clean-start/history-reset condition. Treat remaining onset allowance separately from an inherited event's usual “onset already passed” case. A two-run cut/restore test would distinguish these cases; the existing window ownership test is not that test.

**Mask reuse guards: no ordinary pending-readback or census alias found.**

`RefreshMaskTagReleasability` checks the exact live event, attached/unprocessed trail receipts, every pending snapshot containing that event, and the union of pending target-mask tag sets (`AnomalyCaptureSubsystem.cpp:7004–7040`). `FAnomalyMaskMeasure` additionally checks m26 arms in flight and whether further arms may be issued. The allocator rechecks arm ownership, rejects a census claim, and rejects another unrecycled record with the same tag before reuse (`AnomalyMaskMeasure.cpp:104–197`). Recycled records cannot issue new normal/probe measurement arms.

This is deliberately stronger than “the fire ended.” In particular:

- An unread target-mask request continues to block the tag, including a result deferred until m52 membership resolves. Removing a request from the renderer's pending-arm array does not by itself release the corresponding game-thread pending tag.
- For G295, a physical render can serve several request IDs. The per-request target-mask bookkeeping remains until those results are consumed. An empty payload is handled as unmeasured, not a successful mask that permits silent substitution from another frame.
- Census cannot acquire an event-claimed value, including a recycled one that stays in `EventClaimed`. Retirement also rejects `CensusClaimed`. Census release restores an actor only if it still carries that batch's tag, so a later event tag is not blindly restored away (`Source/AnomalyCapture/Private/AnomalyCensus.cpp:497–546,697–758`).
- Once pixels and tag tables are copied to CPU-owned work, a queued writer does not require the live component to retain its old stencil value. The pending GPU/receipt lifetime is the relevant boundary.

Thus I found no source counterexample in which this new allocator deliberately restores/reassigns an event tag while one of the named, tracked old readbacks or a same-value census claim remains live. F4 prevents turning that narrower conclusion into “aliasing is impossible.” Lost/untracked work and external state mutations are not established safe by the pure release-predicate test.

Recycling only on exhaustion is a reasonable compatibility choice: eligibility can be established after the last consumer, while physical reuse is delayed until needed. The victim is the oldest eligible record by its recorded eligibility time, not necessarily the event with the oldest start/end time. Immediate restoration at every event end would change census eligibility and possibly target selection on short runs. Chat should accept or reject that interpretation explicitly.

One remaining long-run limitation is separate from aliasing: ever-used event tags stay in `EventClaimed`, so they never become `Ledger->IsFree` for census. `ArmNextBatch` stops when free values reach its headroom threshold. This change can sustain event-tag reuse but does not restore long-run census capacity. That behavior was already latent in the old allocator; the new >55-event claim should not be read as proving census liveness or identical long-run selection.

I independently checked the five cited historical G295 cases. Each has `captureArms=2`, a request different from the pixel owner, and a later zero-payload warning:

| Bank leg under `_bench_sessions_bank` | Session index | `MASK-SERVED` / unavailable log lines |
|---|---:|---:|
| `M52FIX_B3_FIX_A1` | 1171 | 14013 / 14035 |
| `M52FIX_B7_FIX_A1` | 126 | 3650 / 3662 |
| `M52FIX_B7_FIX_A1` | 613 | 10267 / 10276 |
| `M52FIX_B7_FIX_A1` | 615 | 10295 / 10304 |
| `M52FIX_B9_FIX_A1` | 194 | 4493 / 4506 |

The report's G295 attribution is supported by these `_leg/game.log` records, not merely by `pixels=0` alone. Extracts are retained in `g295-evidence.json`. This is evidence from the older bank, not runtime qualification of `2AC0523E` or `E9FF019A`. Keeping such a mask unmeasured is honest; copying an earlier frame's mask would not repair frame identity. The standing exception for those five cases does not prove the required new >55-event leg has a mask on every labelled frame.

**Camera geometry: the SAT is materially better than the collision sphere, but still a bounds predicate.**

The helper in `Source/AnomalyInjector/Public/AnomalyNearClipSlab.h:43–73,85–137,185–241` builds a perspective frustum segment and applies the relevant slab face, box face and edge-cross-edge separating axes. For ordinary centred perspective with the corresponding axis/aspect rules, its formulation is reasonable. It rejects the behind/beside sphere cases, includes non-colliding render components and does not deliberately ignore the pawn. Those directly address the three named risks in 086-01 §5.3. Empty slab handling for equal baseline/anomalous near planes is appropriate for a null leg. Orthographic or an independent positive per-camera near override is also treated as unaffected by the global near-plane change.

That geometric result says that selected **bounds overlap the slab**, not that something actually drawn was clipped. Relevant disagreements remain:

| Case | Possible disagreement |
|---|---|
| Hollow/concave mesh, room shell or loose bounds | The filled box intersects the slab while no surface does. A room surrounding the eye can stay positive at every pose even when all walls lie beyond the pushed plane. `eye_inside_box` is a diagnostic, not a correction or invalidation. |
| Per-view hidden or show-only primitives | Component visibility and a scene proxy can be valid while the actual view excludes the primitive. UE explicitly applies `View.HiddenPrimitives` and `ShowOnlyPrimitives` in `Renderer/Private/SceneVisibility.cpp:611–625`; the new visitor does not consult them. A cube inside the slab but hidden only from this view is a simple false positive. |
| Other render relevance/culling | Component flags are not the complete renderer predicate: maximum draw distance, view show flags, proxy relevance, RVT-only drawing and related view decisions can remove candidates from the picture. The source does check registration, component visibility, owner hiding, main-pass status and owner-see rules; this is not a claim that all those checks are absent. |
| Masked/translucent/back-facing geometry | An intersecting box can contain no surviving fragments or no material contribution. The test does not evaluate material opacity or raster coverage. |
| WPO, skeletal deformation and special proxies | Geometry can move into or out of the slab independently of the CPU/local bounds used by the helper. Correct conservative bounds reduce misses but do not make the result exact. |
| FX | `UFXSystemComponent` is deliberately excluded at `AnomalyViewport.cpp:1045,1062–1065`; clipped particles can therefore change the picture with no positive slab verdict if no other geometry qualifies. This is a product-scope exception needing acceptance, not pixel equivalence. |
| Grazing/sub-pixel intersections | Inclusive geometric contact can occupy no output sample, especially after output resizing. Even a real removed fragment need not create a material colour difference from what replaces it. |
| Temporal history | Slab state tracks current geometry; delivered TAA/TSR can retain a prior picture. The new transition machinery does not add a camera-specific temporal window. That needs evidence at the approved pixel threshold. |

I agree with rejecting a blanket “ignore boxes containing the eye” heuristic: it would hide real wall penetrations as well as hollow-box false positives. But merely counting such boxes cannot justify the stronger phrase “something actually drawn is cut away.” The field/rule name `view_slab_render_bounds_v1` is a more accurate description.

The `anomalous <= baseline` empty-slab rule also assumes this anomaly only pushes the plane outward. Configuration clamps the requested near value to a positive minimum, not necessarily to the baseline; a smaller requested near plane can reveal geometry while this label remains false. If inward changes are intentionally outside `camera_clipping`, that precondition should be enforced or named.

**Proposed remedy for the scope problem:** fix F2 independently, then either derive the production decision from render-visible geometry for the accepted captured view, or define and enforce a narrower supported-content/view contract and establish it from independent pixels. A bounds-only fixture pass is not evidence of general rendered-pixel truth. The source audit's suggested bounds slab was a useful direction, not a waiver of the later delivery rule.

**Camera timing: the common-path improvement is real; exact captured-view identity is not established.**

In this UE 5.1 tree, `Engine/Source/Runtime/Engine/Private/LevelTick.cpp` runs tickable objects at line 1606, updates camera managers at 1621, broadcasts post-actor tick at 1672, and broadcasts world tick end at 1814. Deferring camera globals from `FinalizeArmedLabel` to `AppendViewDependentGlobals` in `SampleDeferredActiveState` therefore fixes an important ordinary-path timing problem (`AnomalyCaptureSubsystem.cpp:5290–5297,5500–5614,5634–5637`). It reads a later camera cache and keeps the added event arrays aligned.

Three qualifications prevent calling it the exact view/projection used by the capture:

1. **The cache is not the final view.** `AnomalyViewport.cpp:947–1021` reads the first controller's cached POV, full viewport size and local-player axis constraint. UE's `LocalPlayer.cpp:690–714` subsequently applies `GetFOVAngle`, `GetPlayerViewPoint` and view extensions' `SetupViewPoint`; projection construction also applies player subrects, stereo/XR offsets and `SetupViewProjectionMatrix` (`LocalPlayer.cpp:1048–1165`). Off-centre projection, split-screen, view extensions and custom view overrides can therefore disagree. The production decision neither consumes the captured `FSceneView` matrices nor refuses all those cases.
2. **An armed snapshot is not guaranteed to publish in its arm tick.** `AnomalySveCapturer.cpp:161–190` consumes the oldest pending request; `FamilyFrameNumber` is logged, not compared with the snapshot's sampled frame. `AnomalySceneViewExtension.cpp:50–127` publishes the consumed request from an eligible family; m55 owner checks do not constitute a general camera-label rebinding. If an arm misses its intended family and is served later, its frozen slab verdict can describe camera n while its image is from n+1. This is an inherited transport exposure, not a claim that it occurred on the unrun new bench.
3. **The evaluation cache omits relevant state.** `Source/AnomalyInjector/Private/Anomalies/Anomaly_CameraClipping.cpp:142–178` keys reuse on frame, camera location/rotation/FOV and stored near settings. It does not include view-target ownership, projection offsets/aspect, per-camera override changes, primitive geometry changes or a competing write of the effective global near plane. If an earlier call evaluates the same-camera frame before one of those changes, the tick-end call may reuse it. The slab uses stored baseline/anomalous settings; `EffectiveNear` is diagnostic, not authority for its endpoints.

Also, `Snap.View` is still obtained through the earlier `SampleViewThisTick`/`ProjectionView` path (`AnomalyCaptureSubsystem.cpp:921,4761,5031–5047`). Moving only the clipping decision does not make the exported label-view pose a same-render-view witness. A benchmark that compares that pose to the new decision needs to account for this distinction.

**Proposed remedy:** associate the verdict, pose and projection with the accepted capture family/view and its request identity, with an explicit stale-arm policy. A narrower alternative would fail closed on unsupported view modifications and prove the ordinary family pairing. Evaluation caching should follow that finalized view/state identity. Ordinary frame-cap draining is sensible, but an early stop calling `FinishRun` is not evidence that a not-yet-published camera snapshot acquired the final rendered view.

**G352 is supported, with a narrower claim than “ON and null must differ on every unclipped frame.”** In this engine's `Private/Components/DirectionalLightComponent.cpp:756–784`, directional CSM split distances start from `View.NearClippingDistance`. Changing the near plane can change shadow partitions and visible shadow pixels despite an empty geometric clipping slab. Whether it does depends on the scene, light and shadow path; it is not universal for every renderer or frame. This explains why an independent ON/null oracle needs a measured shadow/noise floor. It does not make all such differing frames geometric clipping positives, nor establish that one calibrated fixture threshold separates the effects in arbitrary content. If the product definition instead includes every visible consequence of the injected near-plane change, the slab omits some of those consequences by design; chat needs to settle that scope.

**Shipping and compatibility.**

`IAI.Label.TransitionOnFrames`, `IAI.Label.TransitionOffFrames` and `IAI.Label.TransitionHideFrames` are registered inside `#if ANOMALY_CAPTURE` at `AnomalyCaptureSubsystem.cpp:88–108`. `Source/AnomalyCapture/AnomalyCapture.Build.cs:340–359` defines it as zero for Shipping. These controls are available in non-Shipping without `-IAIBench`, which is consistent with using them to set product transition values between runs.

`IAI.Bench.CameraSchedule` is registered only inside the `IAIBench` command-line check at `Source/AnomalyBench/Private/AnomalyBenchModule.cpp:257–264`. Its module has `TargetConfigurationDenyList: ["Shipping"]` in `AnomalyInjector.uplugin:20–24`. Execution also uses the fixture/map/input-lock checks. The new control is therefore excluded by module configuration in Shipping, and command-line gated in allowed builds. Some older bench commands register outside the `IAIBench` check (`AnomalyBenchModule.cpp:248–256`), but that is an inherited registration issue, not a new Shipping leak from these commits.

The unconditional byte-identity language in the 084-05a report needs correction:

- Non-m52/non-camera hide runs can acquire transition flags and detached return entries under temporal AA. The journal acknowledges this exception; the report's broad “every other anomaly” sentence does not.
- `AnomalyLabelWriter.cpp:1004–1021` adds label/mask-reuse summary fields when supplied the run's label-sync summary, including runs without either anomaly.
- Recycling changes allocation/mask results on sufficiently long non-m52 runs. This is intentional and is not limited to stuck-mip events.
- A run that fires no new m52 event can still inherit an unresolved m52 restoration trail, so “never fires m52” alone is not a complete unchanged-output precondition.

The narrower source argument holds for ordinary labels rows with no render-truth emission modes, no hide transition, no camera diagnostic and no relevant allocation change: the writer takes the same normal-entry path, preserves order and reduces presence to the old nonempty-fire rule. No end-to-end byte comparison was performed. Nor should byte identity be used to justify retaining F1 while simultaneously claiming the broad G350 invariant.

**Independent checks and limits of the evidence.**

I replayed only the existing standalone offline selftest executables, after inspecting their sources. No compiler, cook, editor, game, capture or binary staging operation was run. All generated files are confined to this review's run directory; repositories and bench state were not changed.

| Check | Observed result | What it establishes |
|---|---|---|
| Existing `m52_window_selftest.exe` | Exit 0; **189 checks, 0 failures** | The existing executable's window/emission/transition/release-predicate model passes its checks. It does not exercise the UE JSON writer, live hide emission, actual GPU queues or component restoration. |
| Existing `cc_slab_selftest.exe` | Exit 0; **93 checks, 0 failures** | The executable's helper geometry and modeled primitive classification pass the listed checks. It does not call the engine's ISM query or capture pixels. |
| 17 slab cases | Legacy sphere model wrong on **9/17**; slab **0/17**; world-AABB-only **1/17**; no-frustum **5/17** | The finite box fixtures discriminate the intended geometry regressions. The old comparison is a sphere/collision model, not an engine collision invocation. |
| Random slab cross-check | 20,000 boxes; 3,746 SAT hits; no sample-hit/SAT-miss; six hits still unconfirmed at 61³ sampling, maximum minimum SAT overlap 0.366 cm | Useful independent point-sampling evidence. Finite sampling cannot prove absence of a thin intersection; those six are neither proven SAT errors nor exhaustive proof. |
| Test-source provenance | Both selftest sources and the three relevant headers match the read-only final tree after line-ending normalization | The examined sources agree. This does not attest that the pre-existing executables were compiled from them. No rebuild was allowed. |
| Historical G295 attribution | All five named cases have matching coalescence/owner and zero-payload records | Supports the report's diagnosis of those banked cases. It does not qualify the new binaries. |

Evidence artifacts retained beside this report: `m52-selftest-replay.txt`, `cc-selftest-replay.txt`, `review-evidence.json`, `g295-evidence.json`, and their small read-only extraction/check scripts. The JSON records the executable and source SHA-256 values. The source snapshot came from the pinned git tree; I did not inspect whichever different branch happens to be checked out as though it were the reviewed change.

The selftest claims are reproducible to this extent. Their can-fail cases establish useful unit-level discrimination, not the delivery requirement that a broken **captured-pixel** build fail. Neither reviewed game binary has runtime evidence supplied for this task. Build-success and binary provenance claims in the implementation reports were not re-established by running a build, and no new in-engine cost, >55-event mask coverage, renderer-view identity or 0/0 pixel-sync claim is made here.

**Disposition: Chat review REQUIRED.** F1 and F2 are sufficient to reject an unqualified source GREEN. F3 and the view/transition boundary findings limit the breadth of any subsequent acceptance; F4 limits the no-alias claim. The source supports the m52 normal-path correction, the stronger mask readback guards and exclusion of the new levers from Shipping. Proposed next evidence would exercise the actual serialized products, the ISM candidate integration, retired-component reuse, cross-run transitions and captured-view identity, followed by the already ruled independent pixel and >55-event legs. This report does not authorize fixes, staging or bench activity.

VERDICT: RED — G350 remains incomplete and camera_clipping has a concrete ISM false negative; an unqualified source or pixel-sync GREEN is not supported.
