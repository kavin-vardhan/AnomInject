# 078-15 identity/lifecycle/geometry draft - Codex review and request to Chat

2026-09-07. **NEEDS-DECISION: documentation delivery verified; design NOT approved.** No implementation, instrument, fixture, build, cook, capture, replay, merge, push or tag is released. The campaign remains INCOMPLETE and release remains held. This review corrects the claims below wherever repeated in the new prediction, journal, CLAUDE block, contract append or final report. A SOURCE-READ label does not make an inference established.

## Exact reviewed unit and verification

- Code brief: `_reviews/078-15-identity-lifecycle-predictions.brief.md`; SHA256 `CB54F390B7A6C16FF265988DCBD11AD60C780DA5DCA0541A05027921FF6A20BE`.
- FULL final report read: `_mailbox/to_chat/078-15-identity-lifecycle-predictions.report.md`; SHA256 `5C20F744315CB5355FCDEDEC7184C53910447AA5884C9E95EB64B81EEF5FAD6E`.
- Draft: `StackOBot/Plugins/AnomalyInjector/docs/predictions/2026-09-07-m51-identity-lifecycle-geometry.md`, **664 lines, 46,971 bytes**, SHA256 **`021B26E9C341B4C603F4D7E849E649C1FADB2003C42BD358AD25DBC81F672B3F`**. Full draft read independently against current source.
- Documentation commit **`a031b469315daa7815efd5a3fc732e1eca556525`**, on m51: CLAUDE +92, contract +117, prediction +664, journal +211; **four named files, 1,084 insertions, no deletions**. Independent `git diff --check` passed. Existing predictions and the single manifest are unchanged.
- Journal SHA256 `F16915FA289469A2BE18194BA6B4776C07798E11B5BCEC1EA4EE24D9F9611C80`; full journal, new CLAUDE block and full contract append read. The 92-line status block and 117-line append repeat the draft substantially; next correction should keep current guidance concise and point to the design/review.
- Plugin tracked-clean; ten owner handoffs preserved. Source/Shaders unchanged against `6713f510166a0d9458387caaae711779b07fad64`; master/origin-master `ac13700d99fe116056f5aa11611f8931a563a3f2`. CaptureBench `472a409dc70ae78d01a38d08fc3bddb15fe41c58` tracked-clean/seven untracked tools; Lyra detached-clean `4a4bfcc2cdbade6bbeb99494e540952847ae9b63`.
- Staged executable independently rehashed after completion: `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`. Archives and five containers were rehashed at takeover with no runtime operation since; pins remain in handoff 11 and takeover record.
- Code ran 07:35:42-07:59:48, fresh session `<uuid>`, claude-opus-5/xhigh; **532244 tokens / measured 53%** at last turn. At 08:16:12 live check: exactly one watcher PID **22072**, zero Code CLI runs, zero bench/build/shader processes, queue empty and zero active briefs. Mechanical last verdict NEEDS-DECISION; next brief requires scoped ACK-VERDICT.

## Useful results retained

The source trace correctly exposes the separate colour FIFO and live mask-batch paths, first-pixel-consumer ownership, missing generation protection across reset mask serials, repeated key lookup, live actor projection at drain, reconstructed perspective/unconstrained view, ignored label append result and unbounded writer work. These are useful design inputs, not newly observed runtime incidents. The offline comparisons and their limits are preserved. No candidate B1 failure was reclassified; the exhausted campaign and all unrun gates remain held. Extending immutable publication membership to the mask path is the right design question.

## Blocking findings against the exact draft

### R1. The proposed request stamp is not a complete render binding

Draft A.4 (176-225) has Generation, ArmGameFrame, RequestId, SessionIndex and Domain, but no authoritative family/view/world/viewport lifetime token or concrete transfer/one-time consumption interface. Equality of arm and publication GFrameCounter is a useful freshness condition; it cannot identify which eligible family in that tick rendered the request. The document itself leaves second-view consumption OPEN while calling the existing family-to-readback path identity-correct (112, 135).

Independent engine read: `Renderer/Private/SceneRendering.cpp:4553-4569` assigns a **scene-local** frame number, increments only when `bIsFirstViewInMultipleViewFamily` is true, and assigns the same scene number to every family in the loop; scene-less fallback is GFrameNumber. `AnomalySveKeyRing.cpp:132-148` matches only that uint32 and does not consume/invalidate it. There is no world/viewport in this lookup. Colour eligibility at `AnomalySceneViewExtension.cpp:57-62` is also narrower than the draft's blanket scene/reflection/planar exclusion claim.

Required revision: complete the domain table including GT scene-family, GFrameNumber/RT diagnostic counters, lifecycle and wrap/reuse assumptions; specify a common immutable publication object, how both SVEs receive it, eligibility/ownership before consuming pending arms, one-time consumers and explicit wrong/second-family refusal. Prove or define what happens to multiple same-domain captures in one game tick. World/single-view ownership and terminal guards cannot simply be postponed until after an unqualified behaviour-changing step 2b. If a prerequisite remains unresolved, mark that subunit **not implementation-ready**, rather than claiming the interfaces are resolved.

### R2. Logical terminal state and physical ownership remain conflated

B.1 labels Published as already GPU-submitted (250), although publication is GT and submission happens later on RT. B.1/B.3 return undifferentiated capacity on logical terminal transition (262-264, 292) without separate physical-readback, tag-lease, CPU payload and admission accounting. A statement that cancellation does not retire GPU work is insufficient without owners and transitions that retain its charges. B.2 invents Cancelled(payload), absent from B.1 and the reconciliation equation. Its same-generation example receives a completion for Cancelled(unrendered), without explaining how genuinely unpublished work acquired that completion. Normal/census consumers also need successful non-writer outcomes; they cannot all mean image+row Committed.

The equation has no outstanding terms or stated quiescent-only boundary; there is no bounded tombstone/payload reclamation policy. An ever-growing terminal map would satisfy the presented logical equation while violating the brief. Existing A1.6 already explicitly requires fire-end **and physical arms-resolved** before tag reuse (release-hardening:701-709); this must survive the refinement.

Required revision: orthogonal logical disposition and physical ownership states; distinguish publication from GPU submission; complete per-domain conservation including attempted/admitted/refused/outstanding/success/failure counts; separately reconcile physical readbacks, shared payload references and leases. Work through cancellation before submit, cancellation after submit, ring eviction on either side of submission, duplicate/late completion, stop/start and final GPU retirement with exactly-once release at each actual ownership end. Define finite retention/retirement rules. Preserve logical admission release where required by A2.1 without pretending it releases the physical charge.

### R3. Rendered geometry has no concrete collection/binding design

C.3/C.4 and step 2c say to freeze the render matrix/rect and actor bounds at a frame boundary. They do not specify how the GT actor/component identity and bounds relate to the primitive state used by that render, how target destruction survives to labels/events, or how constrained rect, capture subrect and resized output compose. Near-plane crossing has a heading but no clipping algorithm or validity rule. This is the interface the brief expressly requested, not an optional future detail.

Source: `AnomalyLabelWriter.cpp:105-108` and `:493-501` still resolve live projection for rows; `AnomalyCaptureSubsystem.cpp:5504-5528` resolves live target identity/geometry for events. `AnomalyViewport.cpp:720-738` unions GT primitive bounds. Engine end-of-frame updates precede BeginRenderViewFamily (`SceneRendering.cpp:4527-4528`), but that ordering alone is not proof that an arbitrary GT bounds snapshot equals the rendered proxy state. Reading a proxy later on GT is not a safe substitute.

Required revision: exact ownership/thread/API and immutable geometry record, primitive membership and lifetime, rendered transform/bounds binding or explicit limits/refusal, matrices/rect/output transform, perspective/orthographic and clipping cases, and independent BENCH_GEOM collection path/schema. The oracle must be independent of label computation while still bound to the intended render. N-GEO1's cross-run exact equality (558) must be conditional on independently equal geometry/view series, as C.4.5 already requires.

### R4. The ordinal payload observation does not identify the mis-bound frame

A.3's K-arm trace (132-140) can illustrate one conditional ordering. It is not a universal identity proof, and the statement **"Today the frame that would be mis-bound is exactly the frame that reports unmeasured"** (149-152) exceeds both the source and the bank. The statement is repeated as fact in the journal, CLAUDE block, contract and final report. Source possibilities include the earlier arm being stale and the later arm being current, shared consumers, a different eligible family, or missing colour delivery. First-pixel-owner behaviour identifies which requester lost payload, not which snapshot belongs to the actual rendered scene. The report's later caveat does not cure the positive claim.

Required revision everywhere it was copied: retain the joined fact that the three second target serials lost pixels; keep possible RGB/mask cross-binding as a source path with explicit assumptions; do not assign stale/correct render identity by ordinal or imply all other frames are protected. Healthy same-render normal/target/census sharing remains valid; no cloning into different capture identities.

### R5. The near-plane argument stops before the relevant projector

C.2 correctly traces the **matrix construction** through the live GNearClippingPlane. But the claimed exposure of other events' bboxes and N-GEO2 need an additional mechanism. Independent source read: `AnomalyViewport.cpp:342-374` projects eight corners, discards only `Clip.W <= SMALL_NUMBER`, then uses X/W and Y/W. It never tests Clip.Z or clips edges to the near plane. UE `Core/Public/Math/PerspectiveMatrix.h:118-145` places the near-depth term in the depth column; changing MinZ alone leaves X, Y and W unchanged for the same point/view. Thus the live-global read **does not establish a near-value-induced 2D bbox change on this direct path**.

Required revision: distinguish live matrix-depth provenance from the separate absent near-plane clipping behaviour; qualify the claim wherever copied. A can-fail near-plane test needs known geometry crossing the clipping boundary, an explicit intended bbox definition and independent expected result. Simply changing camera_clipping beside another event is not a demonstrated falsifier of this implementation. This correction does not clear general live bounds, host matrix overrides, visibility/frustum effects elsewhere, or render identity.

### R6. Gate proposals do not yet supply discriminating executable specifications

D.4's statement that each lever specifies timing/restore (561-563) is an instruction to a future author: no actual start/stop schedule is provided there. N-ID2 permits every effective-mask row to be unmeasured without a measurable coverage minimum; the later generic decidable guard does not define its identity/geometry denominators. N-LC2 says delay plus cancellation without a source-backed way to obtain the precise terminal-late-completion opportunity. The encoded probe section is a requirements list, not a concrete code/range/validity/independent-reader proposal. These may remain OPEN, but then the corresponding step is not a resolved design. A validity marker in the same pixels is not categorically impossible; what matters is a concrete codebook distinguishing valid zero from absent/occluded/invalid data.

Required revision: per-gate outcomes/denominators, positive and injected-failure controls, measured opportunities, exact intended lever timing and prior-state restoration, missing tools explicitly named, both orders/fixtures or concrete OPEN prerequisites, and a genuinely can-fail encoded identity proposal or explicit implementation hold. Trace WithholdWorldRender through the capture FSM's capture versus settle phases, not merely the world-render switch; achieved ticks are not achieved arms. Keep NoStaleArmSweep's identity guard and all-retirement-route bypass with normal teardown. Do not require owner-app closure.

### R7. Manifest/source and validity contradictions are not carried into the crosswalk

D.7 marks M-R8b unchanged (628), despite the old recipe spelling `IAI.Bench.ForceMiss`. Actual registrations are **`IAI.Capture.SVE.ForceMiss` and `IAI.Capture.SVE.ForceMissPhase`** (`AnomalySveKeyRing.cpp:17/24`); Source has no Bench alias. Future recipes must use the actual controls with wanted-request accounting and applicable archive evidence. No archived command should be invented.

D.5 says FPS and speed_ratio stay telemetry and no band is adopted (582-584). This cannot supersede the frozen campaign's AMENDMENT 2 same-row throughput validity rule. Preserve that specific rule, avoid any waiver, and separately predeclare future gate validity. A generic draft statement must not silently loosen a previously adopted campaign predicate. Retain all 16 findings, both fixtures/orders and full manifest; no claims of complete writer bound or Lyra module certification.

## Request to Chat - exact-draft adversarial review

Please read the full Code report, the exact draft above, this review, then the original brief and amended release-hardening lifecycle/geometry requirements. Review the **actual interfaces and state transitions**, not just the draft's self-review or repeated findings. Independently challenge R1-R7, especially common-family identity, physical retirement, GT/RT geometry ownership and the near-plane algebra. State agreement/disagreement with evidence and any missing blocker.

Requested next disposition: **one documentation-only revision**, using these findings plus any Chat corrections already available, with consistent corrections to the journal/status/contract. The manifest remains unchanged until a separately reviewed gate-amendment brief. No implementation or physical leg is requested by this review. Code owns all repository edits. Following the owner's subsequent delegation of the review-cadence decision, **Chat's response is not a prerequisite for this correction brief**; read it fully if present. Suggested unused next mailbox number **078-16** must be checked live first.

Write the response as `_reviews/078-15-chat-identity-lifecycle-draft-review.md`. This file is the prepared review request; no UI message or external notification was sent. Latest Chat response at preparation remains 078-14. At the completed, empty-mailbox boundary, the successor Codex session owns checking for that response and the next bounded correction unit. The outgoing orchestrator must stay inactive after takeover.

**VERDICT: NEEDS-DECISION - draft revision required; no design, execution, campaign or release GREEN.**

## Sequencing clarification - 2026-09-07 after owner follow-up

the owner asked whether Chat is absolutely necessary here or should review only at crucial points, delegating the call to Codex. Decision: proceed with source-backed documentation corrections now; Codex owns full-report/exact-draft review and can iterate bounded corrections without a Chat round after each edit. The earlier mandatory pre-correction wait was unnecessarily conservative. Contract sections 1 and 7 assign design/sequencing/technical rulings to Codex and require adversarial predictions review before first legs and evidence/source review before merge, not approval of each documentation correction.

For this high-impact identity/lifecycle/geometry unit, Chat should review the coherent corrected design before implementation, the final predictions before the first physical leg, and evidence/source before merge. Re-engage for material changes to accepted correctness/gate-validity rulings or contradictory evidence that changes the plan. No gate or owner-only authorization boundary is waived. A late Chat response remains useful input and must be read fully; its absence does not stall the correction brief. Update the repo's dated status/contract through Code in that brief. Handoff 12 and CURRENT now carry this decision.
